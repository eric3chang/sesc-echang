#pragma once
#include "BaseMsg.h"

namespace Memory
{
	class WritebackRequestMsg : public EvictionMsg
	{
	public:
		virtual MsgType Type() const { return mt_WritebackRequest; }
	};
}
