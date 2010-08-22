#pragma once
#include "BaseMsg.h"

namespace Memory
{
	class InvalidateMsg : public BaseMsg
	{
	public:
		Address addr;
		size_t size;

		virtual bool IsResponse() const { return false; }
		virtual size_t MsgSize() const { return 1 + sizeof(Address); }
		virtual MsgType Type() const { return mt_Invalidate; }
   protected:
      virtual void print() const
		{
		   BaseMsg::print();
		   cout << " addr=" << addr
		   ;
		}
	};
}
