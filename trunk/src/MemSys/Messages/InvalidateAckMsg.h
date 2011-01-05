#pragma once
#include "BaseMsg.h"

namespace Memory
{
	class InvalidateAckMsg : public InvalidateResponseMsg
	{
	public:
		virtual MsgType Type() const { return mt_InvalidateAck; }
	};
}

