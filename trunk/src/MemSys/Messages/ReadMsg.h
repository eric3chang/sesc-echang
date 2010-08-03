#pragma once
#include "BaseMsg.h"
#include "../MSTypes.h"
#include "../StoredFunctionCall.h"
#include <cstdlib>

namespace Memory
{
	class ReadMsg : public BaseMsg
	{
	public:
		Address addr;
		size_t size;
		bool requestingExclusive;
		bool alreadyHasBlock;

		StoredFunctionBase* onCompletedCallback;

		virtual bool IsResponse() const { return false; }
		virtual size_t MsgSize() const { return sizeof(Address) + 1; }
		virtual MsgType Type() const { return mt_Read; }
		// TODO Eric 2010/08/02
		//void SignalComplete()  // original code
		void SignalComplete() const
		{
			if(onCompletedCallback)
			{
				onCompletedCallback->Call();
			}
		}
	};
}
