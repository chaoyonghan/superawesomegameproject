#pragma once
#include <iostream>
#include <string>
#include <deque>
#include <boost\bind.hpp>
#include <boost\asio.hpp>
#include <boost\thread\thread.hpp>
#include "ChatMessage.h"
#include "Singleton.h"

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
		, mDeqWrtMsg()
	{

	}
	
public:
	//Initializes the Network Manager.
	NetworkResult Initialize()
	{
		
	}
	//Terminates the Network Manager.
	void Terminate();	
	//Returns the network address of the local machine.
	void GetLocalAddress()
	{

	}
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

	}
	void Disconnect();	//Terminates a connection with another machine.
	void ResolveAddress();	//Looks up the IP address corresponding to a domain name.
	//Sends a reliable packet to another machine.
	void SendReliablePacket(const ChatMessage& msg)
	{

	}
	void SendUnreliablePacket();	//Sends an unreliable packet to another machine.
	void SendUnorderedPacket();	 //Sends an unordered packet to another machine.
	void SendConnectionlessPacket(); //Sends an unreliable data packet to another machine without establishing a connection.
	void BroadcastPacket();	//Broadcasts a data packet on the LAN.

	//Retrieves the next available incoming data packet.
	char* ReceivePacket(const boost::system::error_code& iError)
	{
	}

	void SetNetworkEventProc()	//Installs a callback function that is called when a network event occurs.
	{

	}
	void NetworkTask();	//Called once per application loop to allow the Network Manager to perform internal processing.

private:

	NetworkMgr& operator= ( const NetworkMgr& );

	unsigned short mPort;
	bool mProtocol;
	boost::asio::io_service mIoService;

	boost::asio::ip::tcp::socket mTcpSocket;
	boost::asio::ip::tcp::resolver mTcpResolver;
	//boost::asio::ip::tcp::resolver::query* mTcpQuery;

	boost::asio::ip::udp::socket mUdpSocket;
	boost::asio::ip::udp::resolver mUdpResolver;
	//boost::asio::ip::udp::resolver::query* mUdpQuery;

	ChatMessage mReadMsg;
	std::deque<ChatMessage> mDeqWrtMsg;
};