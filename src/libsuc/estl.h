/* 
   SESC: Super ESCalar simulator
   Copyright (C) 2004 University of California, Santa Cruz

   Contributed by Jose Renau

This file is part of SESC.

SESC is free software; you can redistribute it and/or modify it under the terms
of the GNU General Public License as published by the Free Software Foundation;
either version 2, or (at your option) any later version.

SESC is    distributed in the  hope that  it will  be  useful, but  WITHOUT ANY
WARRANTY; without even the implied warranty of MERCHANTABILITY or FITNESS FOR A
PARTICULAR PURPOSE.  See the GNU General Public License for more details.

You should  have received a copy of  the GNU General  Public License along with
SESC; see the file COPYING.  If not, write to the  Free Software Foundation, 59
Temple Place - Suite 330, Boston, MA 02111-1307, USA.
*/
#ifndef ESTL_H
#define ESTL_H

// Diferent compilers have slightly different calling conventions for
// STL. This file has a cross-compiler implementation.

#ifdef _MSC_VER

#include <hash_map>
#include <hash_set>
#include <list>
#include <algorithm>

template <class Key, class Hash, class Compare>
class MS_HashTraits
{
	Hash h;
	Compare c;
public:
	static const size_t bucket_size = 4;
	static const size_t min_buckets = 8;
	MS_HashTraits( )
	{}
	MS_HashTraits( Compare pred )
		: c(pred)
	{}
	size_t operator( )( const Key& k ) const
	{
		return h(k);
	}
	bool operator( )(const Key& k1, const Key& k2) const
	{
		return c(k1,k2);
	}
};
template <class T>
class ReplacementSGIHash
{
public:
	size_t operator()(const T& v) const
	{
		return (size_t) v;
	}
};
template <>
class ReplacementSGIHash<char*>
{
public:
	size_t operator()(const char* v) const
	{
		return (size_t)(v[0]);
	}
};
template <class Key, class Val, class Hash = ReplacementSGIHash<Key>, class Comparator = std::less<Key>>
class SGI_HashMap : public stdext::hash_map<Key, Val, MS_HashTraits<Key, Hash, Comparator>>{};
template <class Key, class Hash = ReplacementSGIHash<Key>, class Comparator = std::less<Key>>
class SGI_HashSet : public stdext::hash_set<Key,MS_HashTraits<Key, Hash,Comparator>>{};
template <class Key, class Val, class Hash = ReplacementSGIHash<Key>, class Comparator = std::less<Key>>
class SGI_HashMultimap : public stdext::hash_multimap<Key,Val,MS_HashTraits<Key, Hash,Comparator>>{};

#define HASH_MAP SGI_HashMap
#define HASH_SET SGI_HashSet
#define HASH_MULTIMAP SGI_HashMultimap
#define HASH ReplacementSGIHash
#define SLIST std::list

#elif __INTEL_COMPILER
#include <ext/hash_map>
#include <ext/hash_set>
#include <ext/slist>
#include <algorithm>
#define HASH_MAP       stlport::hash_map
#define HASH_SET       stlport::hash_set
#define HASH_MULTIMAP  stlport::hash_multimap
#define HASH           std::hash
#define SLIST          std::slist
#elif USE_STL_PORT
/* Sun Studio or Standard compiler -library=stlport5 */
#include <hash_map>
#include <hash_set>
#include <slist>
#include <algorithm>
#define HASH_MAP       std::hash_map
#define HASH_SET       std::hash_set
#define HASH_MULTIMAP  std::hash_multimap
#define HASH           std::hash
#define SLIST          std::slist
#else
/* GNU C Compiler */
#include <ext/hash_map>
#include <ext/hash_set>
#include <ext/slist>
#include <ext/algorithm>
#define HASH_MAP       __gnu_cxx::hash_map
#define HASH_SET       __gnu_cxx::hash_set
#define HASH_MULTIMAP  __gnu_cxx::hash_multimap
#define HASH           __gnu_cxx::hash
#define SLIST          __gnu_cxx::slist
#endif

#endif // ESTL_H
