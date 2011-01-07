#pragma once
#include "ReadMsg.h"

namespace Memory
{
	class InterventionMsg : public ReadMsg
	{
	public:
		virtual MsgType Type() const { return mt_Intervention; }
	};
}

