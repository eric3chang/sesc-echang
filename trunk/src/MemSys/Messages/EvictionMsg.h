#pragma once
#include "BaseMsg.h"

namespace Memory
{
	class EvictionMsg : public BaseMsg
	{
	public:
		Address addr;
		size_t size;
		bool blockAttached;

		virtual bool IsResponse() const { return false; }
		virtual size_t MsgSize() const { return 1 + sizeof(Address) + (blockAttached?size:0); }
		// TODO: Eric 2010/08/04
		virtual MessageID Msgid() const {
		   return 0;
		}

		virtual MsgType Type() const { return mt_Eviction; }
	};
}
