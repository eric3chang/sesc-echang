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
		//NodeID directoryNode;
		NodeID originalRequestingNode;

		StoredFunctionBase* onCompletedCallback;

      ReadMsg() : directoryLookup(false) {}
		virtual bool IsResponse() const { return false; }
		virtual size_t MsgSize() const { return sizeof(Address) + 1; }
		virtual MsgType Type() const { return mt_Read; }

		void SignalComplete() const
		{
			if(onCompletedCallback)
			{
				onCompletedCallback->Call();
			}
		}
	protected:
		virtual void print(DeviceID destinationDeviceID) const
		{
		   BaseMsg::print(destinationDeviceID);
		   cout << " addr=" << addr
		         << " requestingEx=" << requestingExclusive
		         << " alreadyHasBlk=" << alreadyHasBlock
		         << " dirLookup=" << directoryLookup
		   ;
		}
	};
}
