#pragma once
#include "BaseDeviceMsg.h"

using std::cout;

namespace Memory
{
	class BaseNakMsg : public BaseDeviceMsg
	{
	public:
		MessageID solicitingMessage;
		size_t size;

		virtual void print(DeviceID destinationDeviceID) const
		{
			BaseDeviceMsg::print(destinationDeviceID);
         cout
				<< " sMsg=" << solicitingMessage
		   ;
		}
	};
}
