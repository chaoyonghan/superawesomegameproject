#pragma once
#include <boost\bind.hpp>
#include <boost\asio.hpp>
#include <boost\thread\thread.hpp>

class NetworkMgr;

class NetworkMgr
{
public:
	typedef boost::asio::io_service IoService;
	typedef boost::asio::ip::tcp::socket Socket;

	int Initialize()
	{

	}
	
	NetworkMgr()
	{

	}

	NetworkMgr(std::string iIP,std::string iPort)
	{
		IoService mIoService;
		boost::asio::ip::tcp::resolver mResolver(mIoService);
		boost::asio::ip::tcp::resolver::query mQuery(iIP, iPort);
		boost::asio::ip::tcp::resolver::iterator mIterator = mResolver.resolve(mQuery);
	}

private:

	//IoService& mIoService;
	Socket mSocket;
};