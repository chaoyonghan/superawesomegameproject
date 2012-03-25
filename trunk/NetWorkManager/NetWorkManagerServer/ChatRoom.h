#pragma once
#include "ChatPartecipant.h"
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

class ChatRoom
{
public:

	void join(ChatParticipantPtr participant)
	{
		mParticipants.insert(participant);
		std::for_each(mRecentMsgs.begin(), mRecentMsgs.end(),
			boost::bind(&ChatParticipant::deliver, participant, _1));
	}

	void leave(ChatParticipantPtr participant)
	{
		mParticipants.erase(participant);
	}

	void deliver(const ChatMessage& msg)
	{
		mRecentMsgs.push_back(msg);
		while (mRecentMsgs.size() > MAX_RECENT_MSG)
			mRecentMsgs.pop_front();

		std::for_each(mParticipants.begin(), mParticipants.end(),
			boost::bind(&ChatParticipant::deliver, _1, boost::ref(msg)));
	}

private:

	std::set<ChatParticipantPtr> mParticipants;
	enum { MAX_RECENT_MSG = 100 };
	ChatMessageQueue mRecentMsgs;
};