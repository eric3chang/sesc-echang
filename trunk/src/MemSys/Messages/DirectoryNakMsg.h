#pragma once
#include "BaseMsg.h"

namespace Memory
{
	class DirectoryNakMsg : public BaseNakMsg
	{
	public:
		virtual bool IsResponse() const { return false; }
		virtual size_t MsgSize() const { return sizeof(Address) + 1; }
		virtual MsgType Type() const { return mt_DirectoryNak; }
	};
}

