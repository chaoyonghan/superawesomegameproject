#include<iostream>
#include "Singleton.h"
#include "NetWorkManager.h"

int main()
{
	NetworkMgr::Instance()->SetPortNumber(13000);
	NetworkMgr::Instance()->Connect("192.168.1.130");
	boost::system::error_code LolBoot;
	ChatMessage msg;
	ChatMessage Rmsg;
	while(1)
	{
		char line[512+1];
		while (std::cin.getline(line, 512 + 1))
		{
			using namespace std; // For strlen and memcpy.
			msg.body_length(strlen(line));
			memcpy(msg.body(), line, msg.body_length());
			msg.encode_header();
			NetworkMgr::Instance()->SendReliablePacket(msg);
		}
	}
	return 0;
}