#pragma once
#include "HashContainers.h"
#include "Debug.h"

template<class Key, class Type>
class Factory
{
public:
	class FactoryNode
	{
	public:
		virtual ~FactoryNode(){}
		virtual Type* Create() = 0;
	};
	template <class SubType>
	class GenericFactoryNode : public FactoryNode
	{
	public:
		virtual Type* Create()
		{
			return new SubType();
		}
	};
private:
	HashMap<Key,FactoryNode*> nodeSet;
public:
	~Factory()
	{
		for( typename HashMap < Key, FactoryNode* >::iterator it = nodeSet.begin(); it != nodeSet.end(); it++)
		{
			delete it->second;
		}
	}
	void RegisterNode(const Key& key, FactoryNode* node)
	{
		DebugAssert(nodeSet.find(key) == nodeSet.end());
		DebugAssert(node);
		nodeSet[key] = node;
	}
	Type* Create(const Key& key)
	{
		DebugAssert(nodeSet.find(key) != nodeSet.end());
		Type* t = nodeSet[key]->Create();
		DebugAssert(t);
		return t;
	}
};
