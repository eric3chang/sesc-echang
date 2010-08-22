#pragma once
#include "BaseMsg.h"
#include "../MSTypes.h"
#include "../StoredFunctionCall.h"
#include <cstdlib>

using std::cout;
using std::endl;

namespace Memory
{
	class ReadMsg : public BaseMsg
	{
	public:
		Address addr;
		size_t size;
		bool requestingExclusive;
		bool alreadyHasBlock;
		bool directoryLookup;

		StoredFunctionBase* onCompletedCallback;

      ReadMsg() : directoryLookup(false) {}
		virtual bool IsResponse() const { return false; }
		virtual size_t MsgSize() const { return sizeof(Address) + 1; }
		virtual MsgType Type() const { return mt_Read; }

		virtual void print() const
		{
		   BaseMsg::print();
		   cout << " addr=" << addr
		         << " requestingExclusive=" << requestingExclusive
		         << " alreadyHasBlock=" << alreadyHasBlock
		         << " directoryLookup=" << directoryLookup
		   ;
		}

		void SignalComplete() const
		{
			if(onCompletedCallback)
			{
				onCompletedCallback->Call();
			}
		}
	};
}
