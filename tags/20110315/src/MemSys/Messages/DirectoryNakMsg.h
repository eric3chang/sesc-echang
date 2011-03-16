#pragma once
#include "BaseNakMsg.h"

namespace Memory
{
	class DirectoryNakMsg : public BaseNakMsg
	{
	public:
		bool isInvalidateResponse;
		bool isReadResponse;

		virtual bool IsResponse() const { return false; }
		virtual size_t MsgSize() const { return sizeof(Address) + 1; }
		virtual MsgType Type() const { return mt_DirectoryNak; }

		virtual void print(DeviceID destinationDeviceID) const
		{
			BaseNakMsg::print(destinationDeviceID);
         cout
				<< " invRes=" << isInvalidateResponse
				<< " readRes=" << isReadResponse
		   ;
		}
	};
}

