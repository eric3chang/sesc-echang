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
		//bool isMemory;
		NodeID originalRequestingNode;

		StoredFunctionBase* onCompletedCallback;

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
		virtual void print(DeviceID destinationDeviceID) const
		{
		   BaseMsg::print(destinationDeviceID);
		   cout << " adr=" << addr
		         << " reqEx=" << requestingExclusive
		         << " hasBlk=" << alreadyHasBlock
		         << " dirLook=" << directoryLookup
		         //<< " mem=" << isMemory
               << " ogReqNode=" << BaseMsg::convertNodeIDToDeviceID(originalRequestingNode)
		   ;
		}
	};
}
