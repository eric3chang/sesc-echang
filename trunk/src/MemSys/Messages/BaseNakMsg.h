#pragma once
#include "BaseMsg.h"

using std::cout;

namespace Memory
{
	class BaseNakMsg : public BaseDeviceMsg
	{
	public:
		MessageID solicitingMsg;

		virtual void print(DeviceID destinationDeviceID) const
		{
			BaseDeviceMsg::print(destinationDeviceID);
         cout
				<< " sMsg=" << solicitingMsg
		   ;
		}
	};
}
