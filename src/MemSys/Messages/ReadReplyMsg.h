#pragma once
#include "BaseMsg.h"

namespace Memory
{
	class ReadReplyMsg : public ReadResponseMsg
	{
	public:
		virtual MsgType Type() const { return mt_ReadReply; }
	};
}

