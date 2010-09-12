#pragma once
#include "BaseMsg.h"
#include "../MSTypes.h"
#include "../StoredFunctionCall.h"
#include <cstdlib>

using std::cout;
using std::endl;

namespace Memory
{
	class InterventionSharedRequestMsg : public ReadMsg
	{
	public:
		virtual MsgType Type() const { return mt_InterventionSharedRequest; }
	};
}
