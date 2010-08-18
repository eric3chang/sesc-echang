#include "Connection.h"
#include "EventManager.h"

using std::cout;
using std::endl;

namespace Memory
{
	Connection::Connection(std::string linkName, BaseMemDevice* from, BaseMemDevice* to, int toConnectionID, TimeDelta delay, EventManager* ev)
	{
		this->name = linkName;
		this->from = from;
		this->to = to;
		this->ev = ev;
		this->latency = delay;
		this->toConnectionID = toConnectionID;
	}
	void Connection::SendMsg(const BaseMsg* msg, TimeDelta delay)
	{
		ev->ScheduleMessageTransmit(msg,from,to,toConnectionID,delay + latency);
	}
	const std::string& Connection::Name()
	{
		return name;
	}
	void Connection::print() const
	{
	   cout << "name=" << name
	         << " from=" << from->DeviceName()
	         << " to=" << to->DeviceName()
	         << " toConnectionID=" << (int)toConnectionID
	         << endl;
	}
}
