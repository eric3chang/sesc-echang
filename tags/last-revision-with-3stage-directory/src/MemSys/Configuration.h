#pragma once
#include <vector>
#include <string>
#include <iostream>
#include <map>
#include "Debug.h"

namespace Memory
{
	typedef long long ConfigInt;
	typedef double ConfigReal;
	typedef std::string ConfigString;

	class ConfigNode
	{
	public:
		enum NodeType
		{
			cn_Root,
			cn_Int,
			cn_Real,
			cn_String,
			cn_IntSet,
			cn_RealSet,
			cn_StringSet,
		};
	private:
		NodeType type;
		std::string nodeName;
	public:
		ConfigNode(NodeType type, const std::string& nodeName)
		{
			this->type = type;
			this->nodeName = nodeName;
		}
		virtual ~ConfigNode(){}
		virtual void WriteTo(std::ostream& out) const = 0;
		virtual void LoadFrom(std::istream& in) = 0;

		template<class T>
		bool IsA() const
		{
			return ((T*)(this))->IsCastCorrectly();
		}

		NodeType Type() const
		{
			return type;
		}
		const std::string& Name() const
		{
			return nodeName;
		}
	};
	template <class StorageType, ConfigNode::NodeType ContainerType>
	class PrimitiveConfigNode : public ConfigNode
	{
		StorageType value;
	public:
		PrimitiveConfigNode(const std::string& nodeName)
			: ConfigNode(ContainerType,nodeName)
		{}
		virtual ~PrimitiveConfigNode(){}
		virtual void WriteTo(std::ostream& out) const
		{
			out << value << std::endl;
		}
		virtual void LoadFrom(std::istream& in)
		{
			in >> value;
		}

		bool IsCastCorrectly() const
		{
			return Type() == ContainerType;
		}

		const StorageType& Value() const
		{
			return value;
		}
	};
	template <class StorageType, ConfigNode::NodeType ContainerType>
	class PrimitiveSetConfigNode : public ConfigNode
	{
		std::vector<StorageType> valueSet;
	public:
		PrimitiveSetConfigNode(const std::string& nodeName)
			: ConfigNode(ContainerType,nodeName)
		{}
		virtual ~PrimitiveSetConfigNode(){}
		virtual void WriteTo(std::ostream& out) const
		{
			int setSize = (int) valueSet.size();
			out << setSize << " { ";
			for(int i = 0; i < setSize; i++)
			{
				out << valueSet[i] << " ";
			}
			out << "}" << std::endl;
		}
		virtual void LoadFrom(std::istream& in)
		{
			valueSet.clear();
			int setSize;
			char trash;
			in >> setSize;
			in >> trash;
			DebugAssert(trash == '{');
			for(int i = 0; i < setSize; i++)
			{
				StorageType e;
				in >> e;
				valueSet.push_back(e);
			}
			in >> trash;
			DebugAssert(trash == '}');
		}

		bool IsCastCorrectly() const
		{
			return Type() == ContainerType;
		}

		int SetSize() const
		{
			return (int)(valueSet.size());
		}
		const StorageType& Value(int index) const
		{
			return valueSet[index];
		}
	};
	typedef PrimitiveConfigNode<ConfigInt,ConfigNode::cn_Int> IntConfigNode;
	typedef PrimitiveConfigNode<ConfigReal,ConfigNode::cn_Real> RealConfigNode;
	typedef PrimitiveConfigNode<ConfigString,ConfigNode::cn_String> StringConfigNode;
	typedef PrimitiveSetConfigNode<ConfigInt,ConfigNode::cn_Int> IntSetConfigNode;
	typedef PrimitiveSetConfigNode<ConfigReal,ConfigNode::cn_Real> RealSetConfigNode;
	typedef PrimitiveSetConfigNode<ConfigString,ConfigNode::cn_String> StringSetConfigNode;
	class RootConfigNode : public ConfigNode
	{
		std::vector<ConfigNode*> nodeSet;
	public:
		RootConfigNode(const std::string& nodeName)
			: ConfigNode(ConfigNode::cn_Root,nodeName)
		{}
		virtual ~RootConfigNode()
		{
			for(size_t i = 0; i < nodeSet.size(); i++)
			{
				delete nodeSet[i];
			}
		}
		virtual void WriteTo(std::ostream& out) const
		{
			std::string nodeType;
			std::string nodeName;

			out << "Begin" << std::endl;
			for(size_t i = 0; i < nodeSet.size(); i++)
			{
				nodeName = nodeSet[i]->Name();
				switch(nodeSet[i]->Type())
				{
				case ConfigNode::cn_Root:		nodeType = "Root";		break;
				case ConfigNode::cn_Int:		nodeType = "Int";		break;
				case ConfigNode::cn_Real:		nodeType = "Real";		break;
				case ConfigNode::cn_String:		nodeType = "String";	break;
				case ConfigNode::cn_IntSet:		nodeType = "IntSet";	break;
				case ConfigNode::cn_RealSet:	nodeType = "RealSet";	break;
				case ConfigNode::cn_StringSet:	nodeType = "StringSet";	break;
				default:	DebugAssert(0);
				}
				out << nodeType << " " << nodeName << " ";
				nodeSet[i]->WriteTo(out);
			}
			out << "End" << std::endl;
		}
		virtual void LoadFrom(std::istream& in)
		{
			for(size_t i = 0; i < nodeSet.size(); i++)
			{
				delete nodeSet[i];
			}
			nodeSet.clear();

			std::string nodeType;
			std::string nodeName;
			std::string marker;
			in >> marker;
			DebugAssert(marker == "Begin");
			while(true)
			{
				in >> nodeType;
				if(nodeType == "End")
				{
					break;
				}
				in >> nodeName;
				ConfigNode* n;
				if(nodeType == "Int")
				{
					n = new IntConfigNode(nodeName);
				}
				else if(nodeType == "Real")
				{
					n = new RealConfigNode(nodeName);
				}
				else if(nodeType == "String")
				{
					n = new StringConfigNode(nodeName);
				}
				else if(nodeType == "IntSet")
				{
					n = new IntSetConfigNode(nodeName);
				}
				else if(nodeType == "RealSet")
				{
					n = new RealSetConfigNode(nodeName);
				}
				else if(nodeType == "StringSet")
				{
					n = new StringSetConfigNode(nodeName);
				}
				else if(nodeType == "Root")
				{
					n = new RootConfigNode(nodeName);
				}
				else
				{
					DebugAssert(0);
				}
				n->LoadFrom(in);
				nodeSet.push_back(n);
			}
		}

		bool IsCastCorrectly() const
		{
			return Type() == cn_Root;
		}

		void AddSubNode(ConfigNode* node)
		{
			nodeSet.push_back(node);
		}
		int SubNodeCount() const
		{
			return (int)nodeSet.size();
		}
		int SubNodeCount(const std::string& name) const
		{
			int counter = 0;
			for(size_t i = 0; i < nodeSet.size(); i++)
			{
				if(nodeSet[i]->Name() == name)
				{
					counter++;
				}
			}
			return counter;
		}
		const std::string& SubNodeName(int id) const
		{
			DebugAssert(id < (int)nodeSet.size());
			return nodeSet[id]->Name();
		}
		bool ContainsSubNode(const std::string& name) const
		{
			for(size_t i = 0; i < nodeSet.size(); i++)
			{
				if(nodeSet[i]->Name() == name)
				{
					return true;
				}
			}
			return false;
		}
		const ConfigNode& SubNode(int id) const
		{
			DebugAssert(id < (int)nodeSet.size());
			return *(nodeSet[id]);
		}
		const ConfigNode& SubNode(const std::string& name, int iteration = 0) const
		{
			DebugAssert(iteration >= 0);
			for(size_t i = 0; i < nodeSet.size(); i++)
			{
				if(nodeSet[i]->Name() == name)
				{
					if(iteration > 0)
					{
						iteration--;
					}
					else
					{
						return *(nodeSet[i]);
					}
				}
			}
			DebugFail("Failed to find specified node");
			return *this;
		}
	};
	class Config
	{
	public:
		static ConfigString GetString(const ConfigNode& n)
		{
			DebugAssert(n.IsA<StringConfigNode>());
			return ((const StringConfigNode&)n).Value();
		}
		static ConfigInt GetInt(const ConfigNode& n)
		{
			DebugAssert(n.IsA<IntConfigNode>());
			return ((const IntConfigNode&)n).Value();
		}
		static ConfigReal GetReal(const ConfigNode& n)
		{
			DebugAssert(n.IsA<RealConfigNode>());
			return ((const RealConfigNode&)n).Value();
		}
		static ConfigString GetString(const ConfigNode& n, int index)
		{
			DebugAssert(n.IsA<StringSetConfigNode>());
			const StringSetConfigNode& s = (const StringSetConfigNode&)n;
			DebugAssert(index < s.SetSize());
			DebugAssert(index >= 0);
			return s.Value(index);
		}
		static ConfigInt GetInt(const ConfigNode& n, int index)
		{
			DebugAssert(n.IsA<IntSetConfigNode>());
			const IntSetConfigNode& s = (const IntSetConfigNode&)n;
			DebugAssert(index < s.SetSize());
			DebugAssert(index >= 0);
			return s.Value(index);
		}
		static ConfigReal GetReal(const ConfigNode& n, int index)
		{
			DebugAssert(n.IsA<RealSetConfigNode>());
			const RealSetConfigNode& s = (const RealSetConfigNode&)n;
			DebugAssert(index < s.SetSize());
			DebugAssert(index >= 0);
			return s.Value(index);
		}
		static ConfigString GetString(const ConfigNode& n, const std::string& nodeName, int nodeIndex = 0)
		{
			DebugAssert(n.IsA<RootConfigNode>());
			const RootConfigNode& r = (const RootConfigNode&) n;
			DebugAssert(r.ContainsSubNode(nodeName));
			DebugAssert(nodeIndex >= 0);
			DebugAssert(nodeIndex < r.SubNodeCount(nodeName));
			return GetString(r.SubNode(nodeName,nodeIndex));
		}
		static ConfigInt GetInt(const ConfigNode& n, const std::string& nodeName, int nodeIndex = 0)
		{
			DebugAssert(n.IsA<RootConfigNode>());
			const RootConfigNode& r = (const RootConfigNode&) n;
			DebugAssert(r.ContainsSubNode(nodeName));
			DebugAssert(nodeIndex >= 0);
			DebugAssert(nodeIndex < r.SubNodeCount(nodeName));
			return GetInt(r.SubNode(nodeName,nodeIndex));
		}
		static ConfigReal GetReal(const ConfigNode& n, const std::string& nodeName, int nodeIndex = 0)
		{
			DebugAssert(n.IsA<RootConfigNode>());
			const RootConfigNode& r = (const RootConfigNode&) n;
			DebugAssert(r.ContainsSubNode(nodeName));
			DebugAssert(nodeIndex >= 0);
			DebugAssert(nodeIndex < r.SubNodeCount(nodeName));
			return GetReal(r.SubNode(nodeName,nodeIndex));
		}
		static ConfigString GetString(const ConfigNode& n, const std::string& nodeName, int nodeIndex, int elementIndex)
		{
			DebugAssert(n.IsA<RootConfigNode>());
			const RootConfigNode& r = (const RootConfigNode&) n;
			DebugAssert(r.ContainsSubNode(nodeName));
			DebugAssert(nodeIndex >= 0);
			DebugAssert(nodeIndex < r.SubNodeCount(nodeName));
			return GetString(r.SubNode(nodeName,nodeIndex), elementIndex);
		}
		static ConfigInt GetInt(const ConfigNode& n, const std::string& nodeName, int nodeIndex, int elementIndex)
		{
			DebugAssert(n.IsA<RootConfigNode>());
			const RootConfigNode& r = (const RootConfigNode&) n;
			DebugAssert(r.ContainsSubNode(nodeName));
			DebugAssert(nodeIndex >= 0);
			DebugAssert(nodeIndex < r.SubNodeCount(nodeName));
			return GetInt(r.SubNode(nodeName,nodeIndex), elementIndex);
		}
		static ConfigReal GetReal(const ConfigNode& n, const std::string& nodeName, int nodeIndex, int elementIndex)
		{
			DebugAssert(n.IsA<RootConfigNode>());
			const RootConfigNode& r = (const RootConfigNode&) n;
			DebugAssert(r.ContainsSubNode(nodeName));
			DebugAssert(nodeIndex >= 0);
			DebugAssert(nodeIndex < r.SubNodeCount(nodeName));
			return GetReal(r.SubNode(nodeName,nodeIndex), elementIndex);
		}
		static int GetSetSize(const ConfigNode& n)
		{
			DebugAssert(n.IsA<IntSetConfigNode>() || n.IsA<RealSetConfigNode>() || n.IsA<StringSetConfigNode>());
			if(n.IsA<IntSetConfigNode>())
			{
				return ((const IntSetConfigNode&)n).SetSize();
			}
			else if(n.IsA<RealSetConfigNode>())
			{
				return ((const RealSetConfigNode&)n).SetSize();
			}
			else if(n.IsA<StringSetConfigNode>())
			{
				return ((const StringSetConfigNode&)n).SetSize();
			}
			else
			{
			   DebugFail("Error: Config::GetSetSize(const ConfigNode& n)");
			}
		}
		static int GetSetSize(const ConfigNode& n, const std::string& name, int index = 0)
		{
			DebugAssert(n.IsA<RootConfigNode>());
			const RootConfigNode& r = (const RootConfigNode&) n;
			DebugAssert(r.ContainsSubNode(name));
			DebugAssert(r.SubNodeCount(name) > index);
			return GetSetSize(r.SubNode(name,index));
		}
		static RootConfigNode& GetSubRoot(ConfigNode& n, const std::string& subNodeName)
		{
			DebugAssert(n.IsA<RootConfigNode>());
			RootConfigNode& r = (RootConfigNode&) n;
			DebugAssert(r.ContainsSubNode(subNodeName));
			DebugAssert(r.SubNode(subNodeName).IsA<RootConfigNode>());
			return (RootConfigNode&) r.SubNode(subNodeName);
		}
		static RootConfigNode& GetSubRoot(ConfigNode& n, const std::string& subNodeName, int index)
		{
			DebugAssert(n.IsA<RootConfigNode>());
			RootConfigNode& r = (RootConfigNode&) n;
			DebugAssert(r.ContainsSubNode(subNodeName));
			DebugAssert(index >= 0);
			DebugAssert(index < r.SubNodeCount(subNodeName));
			DebugAssert(r.SubNode(subNodeName,index).IsA<RootConfigNode>());
			return (RootConfigNode&) r.SubNode(subNodeName,index);
		}
		static const RootConfigNode& GetSubRoot(const ConfigNode& n, const std::string& subNodeName)
		{
			DebugAssert(n.IsA<RootConfigNode>());
			const RootConfigNode& r = (const RootConfigNode&) n;
			DebugAssert(r.ContainsSubNode(subNodeName));
			DebugAssert(r.SubNode(subNodeName).IsA<RootConfigNode>());
			return (const RootConfigNode&) r.SubNode(subNodeName);
		}
		static const RootConfigNode& GetSubRoot(const ConfigNode& n, const std::string& subNodeName, int index)
		{
			DebugAssert(n.IsA<RootConfigNode>());
			const RootConfigNode& r = (const RootConfigNode&) n;
			DebugAssert(r.ContainsSubNode(subNodeName));
			DebugAssert(index >= 0);
			DebugAssert(index < r.SubNodeCount(subNodeName));
			DebugAssert(r.SubNode(subNodeName,index).IsA<RootConfigNode>());
			return (const RootConfigNode&) r.SubNode(subNodeName,index);
		}
		static ConfigString GetStringOrElse(const ConfigString&  orValue, const ConfigNode& n)
		{
			if(n.IsA<StringConfigNode>())
			{
				return ((const StringConfigNode&)n).Value();
			}
			else
			{
				return orValue;
			}
		}
		static ConfigInt GetIntOrElse(const ConfigInt&  orValue, const ConfigNode& n)
		{
			if(n.IsA<IntConfigNode>())
			{
				return ((const IntConfigNode&)n).Value();
			}
			else
			{
				return orValue;
			}
		}
		static ConfigReal GetRealOrElse(const ConfigReal&  orValue, const ConfigNode& n)
		{
			if(n.IsA<RealConfigNode>())
			{
				return ((const RealConfigNode&)n).Value();
			}
			else
			{
				return orValue;
			}
		}
		static ConfigString GetStringOrElse(const ConfigString&  orValue, const ConfigNode& n, int index)
		{
			if(n.IsA<StringSetConfigNode>())
			{
				const StringSetConfigNode& s = (const StringSetConfigNode&)n;
				if(index < s.SetSize() && index >= 0)
				{
					return s.Value(index);
				}
			}
			return orValue;
		}
		static ConfigInt GetIntOrElse(const ConfigInt&  orValue, const ConfigNode& n, int index)
		{
			if(n.IsA<IntSetConfigNode>())
			{
				const IntSetConfigNode& s = (const IntSetConfigNode&)n;
				if(index < s.SetSize() && index >= 0)
				{
					return s.Value(index);
				}
			}
			return orValue;
		}
		static ConfigReal GetRealOrElse(const ConfigReal&  orValue, const ConfigNode& n, int index)
		{
			if(n.IsA<RealSetConfigNode>())
			{
				const RealSetConfigNode& s = (const RealSetConfigNode&)n;
				if(index < s.SetSize() && index >= 0)
				{
					return s.Value(index);
				}
			}
			return orValue;
		}
		static ConfigString GetStringOrElse(const ConfigString& orValue, const ConfigNode& n, const std::string& nodeName, int nodeIndex = 0)
		{
			DebugAssert(n.IsA<RootConfigNode>());
			const RootConfigNode& r = (const RootConfigNode&) n;
			if(r.ContainsSubNode(nodeName) && nodeIndex >= 0 && nodeIndex < r.SubNodeCount(nodeName))
			{
				return GetString(r.SubNode(nodeName,nodeIndex));
			}
			return orValue;
		}
		static ConfigInt GetIntOrElse(const ConfigInt&  orValue, const ConfigNode& n, const std::string& nodeName, int nodeIndex = 0)
		{
			DebugAssert(n.IsA<RootConfigNode>());
			const RootConfigNode& r = (const RootConfigNode&) n;
			if(r.ContainsSubNode(nodeName) && nodeIndex >= 0 && nodeIndex < r.SubNodeCount(nodeName))
			{
				return GetInt(r.SubNode(nodeName,nodeIndex));
			}
			return orValue;
		}
		static ConfigReal GetRealOrElse(const ConfigReal&  orValue, const ConfigNode& n, const std::string& nodeName, int nodeIndex = 0)
		{
			DebugAssert(n.IsA<RootConfigNode>());
			const RootConfigNode& r = (const RootConfigNode&) n;
			if(r.ContainsSubNode(nodeName) && nodeIndex >= 0 && nodeIndex < r.SubNodeCount(nodeName))
			{
				return GetReal(r.SubNode(nodeName,nodeIndex));
			}
			return orValue;
		}
		static ConfigString GetStringOrElse(const ConfigString&  orValue, const ConfigNode& n, const std::string& nodeName, int nodeIndex, int elementIndex)
		{
			DebugAssert(n.IsA<RootConfigNode>());
			const RootConfigNode& r = (const RootConfigNode&) n;
			if(r.ContainsSubNode(nodeName) && nodeIndex >= 0 && nodeIndex < r.SubNodeCount(nodeName))
			{
				return GetString(r.SubNode(nodeName,nodeIndex));
			}
			return orValue;
		}
		static ConfigInt GetIntOrElse(const ConfigInt&  orValue, const ConfigNode& n, const std::string& nodeName, int nodeIndex, int elementIndex)
		{
			DebugAssert(n.IsA<RootConfigNode>());
			const RootConfigNode& r = (const RootConfigNode&) n;
			if(r.ContainsSubNode(nodeName) && nodeIndex >= 0 && nodeIndex < r.SubNodeCount(nodeName))
			{
				return GetInt(r.SubNode(nodeName,nodeIndex));
			}
			return orValue;
		}
		static ConfigReal GetRealOrElse(const ConfigReal&  orValue, const ConfigNode& n, const std::string& nodeName, int nodeIndex, int elementIndex)
		{
			DebugAssert(n.IsA<RootConfigNode>());
			const RootConfigNode& r = (const RootConfigNode&) n;
			if(r.ContainsSubNode(nodeName) && nodeIndex >= 0 && nodeIndex < r.SubNodeCount(nodeName))
			{
				return GetReal(r.SubNode(nodeName,nodeIndex));
			}
			return orValue;
		}
	};
}

