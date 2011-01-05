#pragma once
#include "BaseMsg.h"

namespace Memory
{
	class InterventionMsg : public ReadMsg
	{
	public:
		virtual MsgType Type() const { return mt_Intervention; }
	};
}

