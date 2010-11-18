/*
 * stl_container_utils.h
 *
 *  Created on: Nov 17, 2010
 *      Author: erichang
 */

#ifndef STL_CONTAINER_UTILS_H_
#define STL_CONTAINER_UTILS_H_

// utility funcions used for debugging stl containers in gdb

#include <map>
#include <set>
#include "AllMessageTypes.h"
#include "HashContainers.h"
#include "MSTypes.h"
/*
#define SHOW(X) cout << # X " = " << (X) << endl
template <class Key, class Value>
void printGenericMap( HashMap<Key,Value> & m)
{
	SHOW( m.find(i)->first );
	SHOW( m[i] );
	typename HashMap<Key,Value>::const_iterator i = m.begin();
	typename HashMap<Key,Value>::const_iterator j = m.end();

	for(; i != j; ++i)
	{
		std::cout << '[' << i->first << "] = " << i->second << '\n';
	}
}
*/
void printMsgMap (HashMap<unsigned long long, const Memory::BaseMsg*> &m)
{
	HashMap<unsigned long long, const Memory::BaseMsg*>::const_iterator i = m.begin();
	HashMap<unsigned long long, const Memory::BaseMsg*>::const_iterator j = m.end();

	std::cout << "[printMsgMap]" << std::endl;
	for(; i != j; ++i)
	{
		std::cout << '[' << i->first << "]";
		i->second->print(0);
		std::cout << std::endl;
	}
}

#endif /* STL_CONTAINER_UTILS_H_ */
