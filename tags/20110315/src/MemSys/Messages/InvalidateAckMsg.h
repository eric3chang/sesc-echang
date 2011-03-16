#pragma once
#include "InvalidateResponseMsg.h"

namespace Memory
{
	class InvalidateAckMsg : public InvalidateResponseMsg
	{
	public:
		virtual MsgType Type() const { return mt_InvalidateAck; }
	};
}

