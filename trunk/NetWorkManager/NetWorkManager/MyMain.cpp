#include<iostream>
#include "Singleton.h"
#include "NetWorkManager.h"

int main()
{
	NetworkMgr::Instance()->SetPortNumber(13000);
	NetworkMgr::Instance()->Connect("192.168.1.130");
	boost::system::error_code LolBoot;
	char* msg;
	
	while(1)
	{
		//if ( ( msg = NetworkMgr::Instance()->ReceivePacket(LolBoot)) != nullptr)
			//std::cout << msg;
	}
	return 0;
}