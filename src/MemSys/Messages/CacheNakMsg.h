#pragma once
#include "BaseNakMsg.h"

namespace Memory
{
	class CacheNakMsg : public BaseNakMsg
	{
	public:
		virtual bool IsResponse() const { return false; }
		virtual size_t MsgSize() const { return sizeof(Address) + 1; }
		virtual MsgType Type() const { return mt_DirectoryNak; }
	};
}

