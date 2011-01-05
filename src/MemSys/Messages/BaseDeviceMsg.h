#pragma once
#include "BaseMsg.h"

using std::cout;

namespace Memory
{
	class BaseDeviceMsg : public BaseMsg
	{
	public:
		Address addr;

		virtual void print(DeviceID destinationDeviceID) const
		{
			BaseMsg::print(destinationDeviceID);
         cout
				<< " addr=" << addr
		   ;
		}
	};
}

