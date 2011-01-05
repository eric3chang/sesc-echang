#pragma once
#include "BaseMsg.h"

namespace Memory
{
	class WritebackMsg : public TransferMsg
	{
	public:
		virtual MsgType Type() const { return mt_Writeback; }
	};
}
