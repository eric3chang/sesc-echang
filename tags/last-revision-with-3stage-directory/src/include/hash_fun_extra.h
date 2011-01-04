/*
 * hash_fun_extra.h
 *
 *  Created on: may 28, 2010
 *      Author: echang
 */

#ifndef HASH_FUN_EXTRA_H_
#define HASH_FUN_EXTRA_H_

#include <cstddef>

using std::size_t;

namespace __gnu_cxx
{
   template<> struct hash<uint64_t>
   {
   	size_t operator()(uint64_t __x) const
    	{ return __x; }
   };
}

#endif /* HASH_FUN_EXTRA_H_ */
