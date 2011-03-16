#pragma once
#include "EvictionMsg.h"

namespace Memory
{
	class WritebackRequestMsg : public EvictionMsg
	{
	public:
		virtual MsgType Type() const { return mt_WritebackRequest; }
	};
}
