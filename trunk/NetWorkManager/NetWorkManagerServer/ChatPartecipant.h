#pragma once

#include <deque>
#include "ChatMessage.h"

class ChatParticipant
{
protected:


public:

	virtual ~ChatParticipant() {}
	virtual void deliver(const ChatMessage& msg) = 0;

};

typedef boost::shared_ptr<ChatParticipant> ChatParticipantPtr;
typedef std::deque<ChatMessage> ChatMessageQueue;