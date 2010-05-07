#pragma once
#include <list>
#include <algorithm>

#if defined (WIN32) || defined (_WIN32)
   #include <hash_map>
   #include <hash_set>
#else
   #include <ext/hash_map>
   #include <ext/hash_set>
#endif

template <class Key, class Hash, class Compare>
class HashTraitsInteropReplacement
{
	Hash h;
	Compare c;
public:
	static const size_t bucket_size = 4;
	static const size_t min_buckets = 8;
	HashTraitsInteropReplacement( )
	{}
	HashTraitsInteropReplacement( Compare pred )
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
class HashInteropReplacement
{
public:
	size_t operator()(const T& v) const
	{
		return (size_t) v;
	}
};

template <>
class HashInteropReplacement<char*>
{
public:
	size_t operator()(const char* v) const
	{
		return (size_t)(v[0]);
	}
};
template <>
class HashInteropReplacement<std::string>
{
public:
	size_t operator()(const std::string& s) const
	{
		return (size_t)(s[0]) + s.size();
	}
};

template <class Key, class Val, class Hash = HashInteropReplacement<Key>, class Comparator = std::less<Key> >
   class HashMap : public HASH_MAP<Key, Val, HashTraitsInteropReplacement<Key, Hash, Comparator> >{};
template <class Key, class Hash = HashInteropReplacement<Key>, class Comparator = std::less<Key> >
   class HashSet : public HASH_SET<Key,HashTraitsInteropReplacement<Key, Hash,Comparator> >{};
template <class Key, class Val, class Hash = HashInteropReplacement<Key>, class Comparator = std::less<Key> >
   class HashMultiMap : public HASH_MULTIMAP<Key,Val,HashTraitsInteropReplacement<Key, Hash,Comparator> >{};

