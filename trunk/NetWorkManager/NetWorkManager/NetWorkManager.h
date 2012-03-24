#pragma once
#include <iostream>
#include <string>
#include <boost\bind.hpp>
#include <boost\asio.hpp>
#include <boost\thread\thread.hpp>
#include "ChatMessage.h"

enum NetworkResult
{
	kNetworkOkay,
	kNetworkInitFailed
};

enum NetworkProtocolType
{
	TCPConnect,
	UDPConnect
};

class NetworkMgr;

class NetworkMgr : public SingletonHolder<NetworkMgr>
{
	friend SingletonHolder<NetworkMgr>;

	NetworkMgr()
		: mTcpResolver(mIoService)
		, mTcpSocket(mIoService)
		, mUdpResolver(mIoService)
		, mUdpSocket(mIoService)
		, mReadMsg()
	{

	}
	
public:
	//Initializes the Network Manager.
	NetworkResult Initialize()
	{
		
	}
	//Terminates the Network Manager.
	void Terminate();	

	void GetLocalAddress();	//Returns the network address of the local machine.
	void GetConnectionCount();	//Returns returns the number of connections which are currently established.
	void GetMaxConnectionCount();	//Returns the maximum number of connections that the Network Manager will allow at one time.
	void SetMaxConnectionCount();	//Sets the maximum number of connections that the Network Manager will allow at one time.
	NetworkProtocolType GetProtocol();	//Returns the protocol identifier used by the current application.
	void SetProtocol(NetworkProtocolType iNpt);	//Sets the protocol identifier used by the current application.
	unsigned short GetPortNumber();	//Returns the port number on which the local machine will send and receive communications.
	//Sets the port number on which the local machine will send and receive communications.
	void SetPortNumber(unsigned short iPort)
	{
		mPort = iPort;
	}
	void GetBroadcastPortNumber();	//Returns the port number to which broadcasted messages are sent.
	void SetBroadcastPortNumber();	//Sets the port number to which broadcasted messages are sent.
	void GetReliableResendTime();	//Returns the time interval between reliable packet transmission attempts.
	void SetReliableResendTime();	//Sets the time interval between reliable packet transmission attempts.
	void GetReliableResendCount();	//Returns the time interval between reliable packet transmission attempts.
	void SetReliableResendCount();	//Sets the time interval between reliable packet transmission attempts.
	//Attempts to establish a connection with another machine.
	void Connect(const std::string& iAddress)
	{
		std::stringstream tmp;
		tmp << mPort;
		boost::asio::ip::tcp::resolver::query mTcpQuery(iAddress,tmp.str());
		boost::asio::async_connect(
			mTcpSocket,
			mTcpResolver.resolve(mTcpQuery),
			boost::bind(&NetworkMgr::TcpHandleConnection,this,boost::asio::placeholders::error)
			);
		boost::thread t(boost::bind(&boost::asio::io_service::run, &mIoService));
	}
	void Disconnect();	//Terminates a connection with another machine.
	void ResolveAddress();	//Looks up the IP address corresponding to a domain name.

	void SendReliablePacket();	//Sends a reliable packet to another machine.
	void SendUnreliablePacket();	//Sends an unreliable packet to another machine.
	void SendUnorderedPacket();	 //Sends an unordered packet to another machine.
	void SendConnectionlessPacket(); //Sends an unreliable data packet to another machine without establishing a connection.
	void BroadcastPacket();	//Broadcasts a data packet on the LAN.

	//Retrieves the next available incoming data packet.
	char* ReceivePacket(const boost::system::error_code& iError)
	{
		if(!iError)
		{
			return nullptr;
		}
		else
		{
			//mTcpSocket.close();
			return nullptr;
		}
	}

	void SetNetworkEventProc()	//Installs a callback function that is called when a network event occurs.
	{

	}
	void NetworkTask();	//Called once per application loop to allow the Network Manager to perform internal processing.

private:

	NetworkMgr& operator= ( const NetworkMgr& );

	void TcpHandleConnection( const boost::system::error_code& iError)
	{
		if(!iError)
		{
			boost::asio::async_read(
				mTcpSocket,
				boost::asio::buffer(mReadMsg.data(),chat_message::header_length),
				boost::bind(&NetworkMgr::HandleReadHeader, this, boost::asio::placeholders::error)
				);
		}
	}

	void HandleReadHeader( const boost::system::error_code& iError)
	{
		if (!iError && mReadMsg.decode_header())
		{
			boost::asio::async_read(mTcpSocket,
				boost::asio::buffer(mReadMsg.body(), mReadMsg.body_length()),
				boost::bind(&NetworkMgr::HandleReadBody, this,
				boost::asio::placeholders::error));
		}
		else
		{
			mTcpSocket.close();
		}
	}

	void HandleReadBody( const boost::system::error_code& iError)
	{
		if (!iError)
		{
			std::cout.write(mReadMsg.body(), mReadMsg.body_length());
			std::cout << "\n";
			boost::asio::async_read(mTcpSocket,
				boost::asio::buffer(mReadMsg.data(), chat_message::header_length),
				boost::bind(&NetworkMgr::HandleReadHeader, this,
				boost::asio::placeholders::error));
		}
		else
		{
			mTcpSocket.close();
		}
	}
	unsigned short mPort;
	bool mProtocol;
	boost::asio::io_service mIoService;

	boost::asio::ip::tcp::socket mTcpSocket;
	boost::asio::ip::tcp::resolver mTcpResolver;
	//boost::asio::ip::tcp::resolver::query* mTcpQuery;

	boost::asio::ip::udp::socket mUdpSocket;
	boost::asio::ip::udp::resolver mUdpResolver;
	//boost::asio::ip::udp::resolver::query* mUdpQuery;

	chat_message mReadMsg;

};
//
//class SingletonNetworkMgr : public NetworkMgr
//{
//	typedef SingletonHolder<SingletonNetworkMgr> SingManager;
//
//	SingletonNetworkMgr( const SingletonNetworkMgr & );
//
//	SingletonNetworkMgr & operator = ( const SingletonNetworkMgr & );
//
//public:
//
//	static SingletonNetworkMgr& Instance( void )
//	{
//		return SingManager::Instance();
//	}
//
//	SingletonNetworkMgr() : NetworkMgr("","")
//	{
//	
//	}
//
//	/// The destructor is not meant to be called directly.
//	~SingletonNetworkMgr( void )
//	{
//
//	}
//};