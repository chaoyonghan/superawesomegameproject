#pragma once
#include <boost\bind.hpp>
#include <boost\asio.hpp>
#include <boost\thread\thread.hpp>

class NetworkMgr;

class NetworkMgr
{
public:

	int Initialize()
	{

	}
	
	NetworkMgr(std::string iIP,std::string iPort)
		: mSocket(mIoService)
		, mResolver(mIoService)
		, mQuery(iIP, iPort)
	{
		mIterator = mResolver.resolve(mQuery);
	}

private:

	boost::asio::ip::tcp::socket mSocket;
	boost::asio::io_service mIoService;
	boost::asio::ip::tcp::resolver mResolver;
	boost::asio::ip::tcp::resolver::query mQuery;
	boost::asio::ip::tcp::resolver::iterator mIterator;
};

class SingletonNetworkMgr : public NetworkMgr
{
	typedef SingletonHolder<SingletonNetworkMgr> SingManager;

	SingletonNetworkMgr( const SingletonNetworkMgr & );

	SingletonNetworkMgr & operator = ( const SingletonNetworkMgr & );

public:

	static SingletonNetworkMgr& Instance( void )
	{
		return SingManager::Instance();
	}

	SingletonNetworkMgr() : NetworkMgr("","")
	{
	
	}

	/// The destructor is not meant to be called directly.
	~SingletonNetworkMgr( void )
	{

	}
};
