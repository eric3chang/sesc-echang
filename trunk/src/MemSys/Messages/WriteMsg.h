#pragma once
#include "BaseMsg.h"
#include "../MSTypes.h"
#include "../StoredFunctionCall.h"
#include <cstdlib>

namespace Memory
{
	class WriteMsg : public BaseMsg
	{
	public:
		Address addr;
		size_t size;

		StoredFunctionBase* onCompletedCallback;

		virtual bool IsResponse() const { return false; }
		virtual size_t MsgSize() const { return sizeof(Address) + size + 2; }
		virtual MsgType Type() const { return mt_Write; }
		void SignalComplete()
		{
			if(onCompletedCallback)
			{
				onCompletedCallback->Call();
			}
		}
	};
}