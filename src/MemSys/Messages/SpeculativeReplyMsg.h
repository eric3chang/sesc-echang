#pragma once
#include "BaseMsg.h"

namespace Memory
{
	class SpeculativeReplyMsg : public ReadResponseMsg
	{
	public:
		virtual MsgType Type() const { return mt_SpeculativeReply; }
	};
}

