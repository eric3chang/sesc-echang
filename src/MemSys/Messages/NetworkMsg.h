#pragma once
#include "BaseMsg.h"
#include "../MSTypes.h"
#include <cstdlib>

namespace Memory
{
	class NetworkMsg : public BaseMsg
	{
	public:
		NodeID sourceNode;
		NodeID destinationNode;

		const BaseMsg* payloadMsg;

		virtual bool IsResponse() const { return payloadMsg->IsResponse(); }
		virtual size_t MsgSize() const { return payloadMsg->MsgSize() + 4; }
		virtual MsgType Type() const { return mt_Network; }
		virtual bool getIsOverrideSource() const {return isOverrideSource;}
		virtual NodeID getOverrideSource() const {return overrideSource;}
		virtual void setIsOverrideSource(bool isOverrideSource)
		{
		   this->isOverrideSource = isOverrideSource;
      }
		virtual void setOverrideSource(NodeID overrideSource)
      {
		   this->overrideSource = overrideSource;
      }

   protected:
		virtual void print(DeviceID destinationDeviceID) const
		{
		   BaseMsg::print(destinationDeviceID);
		   cout << " srcNode=" << sourceNode
            << " destNode=" << destinationNode
		   ;
		}
   private:
		bool isOverrideSource;
		NodeID overrideSource;
	};
}
