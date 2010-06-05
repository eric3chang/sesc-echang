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

template <class Key, class Compare> class EqualKeyReplacement
{
	Compare lessThan;
public:
	bool operator() (const Key& k1, const Key& k2) const
	{
		// simulate equals(k1,k2) using the Comparator
		return !(lessThan(k1,k2) || lessThan(k2,k1));
	}
};

#if defined (WIN32) || defined (_WIN32)
template <class Key,class Val,class Hash=HashInteropReplacement<Key>,class Comparator=std::less<Key>>
   class HashMap:public stdext::hash_map<Key,Val,HashTraitsInteropReplacement<Key,Hash,Comparator>>{};
template <class Key, class Hash = HashInteropReplacement<Key>, class Comparator = std::less<Key>>
   class HashSet : public stdext::hash_set<Key,HashTraitsInteropReplacement<Key, Hash,Comparator>>{};
template<class Key,class Val,class Hash=HashInteropReplacement<Key>,class Comparator=std::less<Key>>
   class HashMultiMap:public stdext::hash_multimap<Key,Val,HashTraitsInteropReplacement<Key,Hash,Comparator>>{};
#else
//TODO finish editing the below stuff, there needs to be a EqualKey class after Key, Val, Hash
template <class Key, class Val, class Hash = HashInteropReplacement<Key>, class Comparator = std::less<Key> >
   class HashMap : public __gnu_cxx::hash_map<Key, Val, Hash, EqualKeyReplacement <Key,Comparator> >{};
template <class Key, class Hash = HashInteropReplacement<Key>, class Comparator = std::less<Key> >
   class HashSet : public  __gnu_cxx::hash_set<Key, Hash, EqualKeyReplacement <Key,Comparator> >{};
template <class Key, class Val, class Hash = HashInteropReplacement<Key>, class Comparator = std::less<Key> >
   class HashMultiMap:public __gnu_cxx::hash_multimap<Key,Val,Hash,EqualKeyReplacement<Key,Comparator> >{};
#endif
