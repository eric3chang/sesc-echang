/*
 * hash_fun_extra.h
 *
 *  Created on: may 28, 2010
 *      Author: echang
 */

#ifndef HASH_FUN_EXTRA_H_
#define HASH_FUN_EXTRA_H_

#include <cstddef>

_GLIBCXX_BEGIN_NAMESPACE(__gnu_cxx)

using std::size_t;

template<> struct hash<uint64_t>
{
	size_t operator()(uint64_t __x) const
	{ return __x; }
};

_GLIBCXX_END_NAMESPACE

#endif /* HASH_FUN_EXTRA_H_ */
