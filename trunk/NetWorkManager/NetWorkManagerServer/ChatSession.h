#include "ChatPartecipant.h"
#include "ChatRoom.h"
#include <algorithm>
#include <cstdlib>
#include <deque>
#include <iostream>
#include <list>
#include <set>
#include <boost/bind.hpp>
#include <boost/shared_ptr.hpp>
#include <boost/enable_shared_from_this.hpp>
#include <boost/asio.hpp>

class ChatSession : public ChatParticipant, public boost::enable_shared_from_this<ChatSession>
{
public:
	ChatSession(boost::asio::io_service& io_service, ChatRoom& room)
		: socket_(io_service),
		room_(room)
	{
	}

	boost::asio::ip::tcp::socket& socket()
	{
		return socket_;
	}

	void start()
	{
		room_.join(shared_from_this());
		boost::asio::async_read(socket_,
			boost::asio::buffer(read_msg_.data(), ChatMessage::header_length),
			boost::bind(
			&ChatSession::handle_read_header, shared_from_this(),
			boost::asio::placeholders::error));
		std::cout << socket_.remote_endpoint() << std::endl;
	}

	void deliver(const ChatMessage& msg)
	{
		bool write_in_progress = !write_msgs_.empty();
		write_msgs_.push_back(msg);
		if (!write_in_progress)
		{
			boost::asio::async_write(socket_,
				boost::asio::buffer(write_msgs_.front().data(),
				write_msgs_.front().length()),
				boost::bind(&ChatSession::handle_write, shared_from_this(),
				boost::asio::placeholders::error));
		}
	}

	void handle_read_header(const boost::system::error_code& error)
	{
		if (!error && read_msg_.decode_header())
		{
			boost::asio::async_read(socket_,
				boost::asio::buffer(read_msg_.body(), read_msg_.body_length()),
				boost::bind(&ChatSession::handle_read_body, shared_from_this(),
				boost::asio::placeholders::error));
		}
		else
		{
			room_.leave(shared_from_this());
		}
	}

	void handle_read_body(const boost::system::error_code& error)
	{
		if (!error)
		{
			room_.deliver(read_msg_);
			boost::asio::async_read(socket_,
				boost::asio::buffer(read_msg_.data(), ChatMessage::header_length),
				boost::bind(&ChatSession::handle_read_header, shared_from_this(),
				boost::asio::placeholders::error));
		}
		else
		{
			room_.leave(shared_from_this());
		}
	}

	void handle_write(const boost::system::error_code& error)
	{
		if (!error)
		{
			write_msgs_.pop_front();
			if (!write_msgs_.empty())
			{
				boost::asio::async_write(socket_,
					boost::asio::buffer(write_msgs_.front().data(),
					write_msgs_.front().length()),
					boost::bind(&ChatSession::handle_write, shared_from_this(),
					boost::asio::placeholders::error));
			}
		}
		else
		{
			room_.leave(shared_from_this());
		}
	}

private:
	boost::asio::ip::tcp::socket socket_;
	ChatRoom& room_;
	ChatMessage read_msg_;
	ChatMessageQueue write_msgs_;
};