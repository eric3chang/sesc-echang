/*
 * dump.h
 *
 *  Created on: Nov 17, 2010
 *      Author: erichang
 */
#ifndef DUMP_H_
#define DUMP_H_

// utility funcions used for debugging stl containers in gdb

#include <map>
#include <set>
#include "AllMessageTypes.h"
#include "HashContainers.h"
#include "MSTypes.h"

namespace Memory
{
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
   template <class Key>
   void dumpTemplate(HashMap<Key,const Memory::BaseMsg*> &m)
   {
      typename HashMap<Key,const Memory::BaseMsg*>::const_iterator i,j;
      i = m.begin();
      j = m.end();

      std::cout << "[" << "dump" << "]" << std::endl;
      for(; i != j; ++i)
      {
         std::cout << '[' << i->first << "]";
         i->second->print(0);
         std::cout << std::endl;
      }
   }

   inline void dump(HashMap<Memory::MessageID, const Memory::BaseMsg*> &m)
   {
      dumpTemplate<Memory::MessageID>(m);
   }

	/*
	void dump (HashMap<MessageID, const Memory::BaseMsg*> &m)
	{
		HashMap<MessageID, const Memory::BaseMsg*>::const_iterator i = m.begin();
		HashMap<MessageID, const Memory::BaseMsg*>::const_iterator j = m.end();

		std::cout << "[dump]" << std::endl;
		for(; i != j; ++i)
		{
			std::cout << '[' << i->first << "]";
			i->second->print(0);
			std::cout << std::endl;
		}
	}
	*/
}  // end namespace Memory

#endif /* DUMP_H_ */
