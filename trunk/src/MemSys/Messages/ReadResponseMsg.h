#pragma once
#include "BaseMsg.h"
#include "../MSTypes.h"

namespace Memory
{
	class ReadResponseMsg : public BaseMsg
	{
	public:
		Address addr;
		size_t size;
      bool directoryLookup; // TODO 2010/08/04 Eric
		bool satisfied;
		bool blockAttached;
		bool exclusiveOwnership;
		MessageID solicitingMessage;

		virtual bool IsResponse() const { return true; }
		virtual size_t MsgSize() const { return 1 + sizeof(Address) + (blockAttached ? size : 0); }
		virtual MsgType Type() const { return mt_ReadResponse; }
	};
}
