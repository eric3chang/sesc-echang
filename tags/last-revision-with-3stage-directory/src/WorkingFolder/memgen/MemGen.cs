public static class MemGen
{
	public enum EvictionPolicy
	{
		LRU,
		Random,
	}
	public static void AddCache(string deviceName, int associativity, int setCount, int lineSize, int missTime, int hitTime, EvictionPolicy ev)
	{
		output.WriteLine("Root MemoryDevice Begin");
		output.WriteLine("String DeviceType MOESICache");
		output.WriteLine("String DeviceName " + deviceName);
		output.WriteLine("Int DeviceID " + (index++));
		output.WriteLine("Int LineSize " + lineSize);
		output.WriteLine("Int Associativity " + associativity);
		output.WriteLine("Int SetCount " + setCount);
		output.WriteLine("Int HitTime " + hitTime);
		output.WriteLine("Int MissTime " + missTime);
		if(deviceName.Contains("L1"))
		{
			output.WriteLine("Int TopCache 1");
		}
		else
		{
			output.WriteLine("Int TopCache 0");
		}
		if(ev == EvictionPolicy.LRU)
		{
			output.WriteLine("String EvictionPolicy LRU");
		}
		else
		{
			output.WriteLine("String EvictionPolicy Random");
		}
		output.WriteLine("End");
	}
	public static void Bus(string deviceName, int transmitLatency, int perByte)
	{
		output.WriteLine("Root MemoryDevice Begin");
		output.WriteLine("String DeviceType SnoopyBus");
		output.WriteLine("String DeviceName " + deviceName);
		output.WriteLine("Int DeviceID " + (index++));
		output.WriteLine("Int TimePerTransfer " + transmitLatency);
		output.WriteLine("Int TimePerByte " + perByte);
		if(perByte == 0)
		{
			output.WriteLine("String LatencyCalculation Constant");
		}
		else
		{
			output.WriteLine("String LatencyCalculation Linear");
		}
		output.WriteLine("String ArbitrationType RemoteFirstRandom");
		output.WriteLine("End");
	}
	public static void MainMemory(int readLatency, int writeLatency)
	{
		output.WriteLine("Root MemoryDevice Begin");
		output.WriteLine("String DeviceType TestMemory");
		output.WriteLine("String DeviceName MainMemory");
		output.WriteLine("Int DeviceID " + (index++));
		output.WriteLine("Int ReadLatency " + readLatency);
		output.WriteLine("Int WriteLatency " + writeLatency);
		output.WriteLine("End");
	}
	public static void Connection(string dev1, string dev2, string con1, string con2, int latency)
	{
		output.WriteLine("Root Connection Begin");
		output.WriteLine("String FromDevice " + dev1);
		output.WriteLine("String ToDevice " + dev2);
		output.WriteLine("String FromName " + con1);
		output.WriteLine("String ToName " + con2);
		output.WriteLine("Int Latency " + latency);
		output.WriteLine("End");
	}
	public static void AddSESCInterface(string nodeName)
	{
		output.WriteLine("Root MemoryDevice Begin");
		output.WriteLine("String DeviceType SESCInterface");
		output.WriteLine("String DeviceName " + nodeName);
		output.WriteLine("Int DeviceID " + (index++));
		output.WriteLine("End");
	}
	public static void AddDirectory(string name, int totalNodes, int myNode, int memoryNode)
	{
		output.WriteLine("Root MemoryDevice Begin");
		output.WriteLine("Int DeviceID " + (index++));
		output.WriteLine("String DeviceType OriginDirectory");
		output.WriteLine("String DeviceName " + name);
		output.WriteLine("Int LocalSendTime " + 4);
		output.WriteLine("Int RemoteSendTime " + 4);
		output.WriteLine("Int LookupRetryTime " + 4);
		output.WriteLine("Int LookupTime " + 4);
		output.WriteLine("Int SatisfyTime " + 4);
		output.WriteLine("Int NodeID " + myNode);
		output.WriteLine("Root MemoryNodeCalculator Begin");
		output.WriteLine("String Type HashedPageCalculator");
		output.WriteLine("Int PageSize 64");
		output.WriteLine("IntSet NodeIDSet " + 1 + " { " + (totalNodes + 10) + " }");
		output.WriteLine("End");
		output.WriteLine("Root DirectoryNodeCalculator Begin");
		output.WriteLine("String Type HashedPageCalculator");
		output.WriteLine("Int PageSize 64");
		output.Write("IntSet NodeIDSet " + totalNodes + " { ");
		for (int i = 0; i < totalNodes; i++)
		{
			output.Write(i + " ");
		}
		output.WriteLine("}");
		output.WriteLine("End");
		output.WriteLine("End");
	}
    public static void AddMainMemory(string deviceName, int readLatency, int writeLatency)
    {
        output.WriteLine("Root MemoryDevice Begin");
        output.WriteLine("String DeviceType TestMemory");
        output.WriteLine("String DeviceName " + deviceName);
        output.WriteLine("Int DeviceID " + (index++));
        output.WriteLine("Int ReadLatency " + readLatency);
        output.WriteLine("Int WriteLatency " + writeLatency);
        output.WriteLine("End");
    }
	public static void AddNetwork(string name, int totalNodes, int randomMin, int randomMax, float perPacket)
	{
		output.WriteLine("Root MemoryDevice Begin");
		output.WriteLine("Int DeviceID " + (index++));
		output.WriteLine("String DeviceType RandomLoadNetwork");
		output.WriteLine("String DeviceName " + name);
		for (int i = 0; i < totalNodes; i++)
		{
			output.WriteLine("Root Connection Begin");
			output.WriteLine("String LinkName Connection_" + i);
			output.WriteLine("Int NodeName " + i);
			output.WriteLine("End");
		}
		output.WriteLine("Root Connection Begin");
		output.WriteLine("String LinkName MemoryConnection");
		output.WriteLine("Int NodeName " + (totalNodes + 10));
		output.WriteLine("End");
		output.WriteLine("Root CalculatorConfig Begin");
		output.WriteLine("String Type RandomLoaded");
		output.WriteLine("Int MinTime " + randomMin);
		output.WriteLine("Int MaxTime " + randomMax);
		output.WriteLine("Int TimePerByte 1");
		output.WriteLine("Real TimePerPacket " + perPacket);
		output.WriteLine("Int EnforceOrder 1");
		output.WriteLine("End");
		output.WriteLine("End");
	}
	public static void AddNetworkMemoryInterface(string name, int node)
	{
		output.WriteLine("Root MemoryDevice Begin");
		output.WriteLine("String DeviceType NetworkMemoryInterface");
		output.WriteLine("String DeviceName " + name);
		output.WriteLine("Int DeviceID " + (index++));
		output.WriteLine("Int NodeID " + node);
		output.WriteLine("End");
	}
	static System.IO.StreamWriter output;
	static int index;
	public static void OutSimpleMemory(int nodeCount)
	{
		output = new System.IO.StreamWriter("memoryConfigs\\Simple" + nodeCount + ".memory");
		index = 1;
		output.WriteLine("Begin");
		MainMemory(400, 300);
		Bus("MainMemBus", 5, 2);
		for (int i = 0; i < nodeCount; i++)
		{
			AddSESCInterface("ProcessorInterface_" + i);
			AddCache("L1_" + i, 4, 4, 64, 3, 1, EvictionPolicy.LRU);
			Connection("ProcessorInterface_" + i, "L1_" + i, "Connection", "LocalConnection", 0);
			Connection("L1_" + i, "MainMemBus", "RemoteConnection", "LocalConnection", 0);
		}
		Connection("MainMemBus", "MainMemory", "RemoteConnection", "Connection", 0);
		output.WriteLine("End");
		output.Close();
	}
	public static void OutSimpleMemory1(int nodeCount, int l1)
	{
		output = new System.IO.StreamWriter("memoryConfigs\\SimpleOneLevel_p" + nodeCount + "_c" + l1 + "L1.memory");
		index = 1;
		output.WriteLine("Begin");
		MainMemory(400, 300);
		l1 = l1 * 1024;
		Bus("MainMemBus", 5, 2);
		for (int i = 0; i < nodeCount; i++)
		{
			AddSESCInterface("ProcessorInterface_" + i);
			AddCache("L1_" + i, 4, l1 / (4 * 64), 64, 3, 1, EvictionPolicy.LRU);
			Connection("ProcessorInterface_" + i, "L1_" + i, "Connection", "LocalConnection", 0);
			Connection("L1_" + i, "MainMemBus", "RemoteConnection", "LocalConnection", 0);
		}
		Connection("MainMemBus", "MainMemory", "RemoteConnection", "Connection", 0);
		output.WriteLine("End");
		output.Close();
	}
	public static void OutSimpleMemory2(int nodeCount, int l1, int l2)
	{
		output = new System.IO.StreamWriter("memoryConfigs\\SimpleTwoLevel_p" + nodeCount + "_c" + l1 + "L1-" + l2 + "L2.memory");
		index = 1;
		output.WriteLine("Begin");
		MainMemory(400, 300);
		l1 = l1 * 1024;
		l2 = l2 * 1024;
		Bus("MainMemBus", 5, 2);
		for (int i = 0; i < nodeCount; i++)
		{
			AddSESCInterface("ProcessorInterface_" + i);
			AddCache("L1_" + i, 4, l1 / (4 * 64), 64, 3, 1, EvictionPolicy.LRU);
			AddCache("L2_" + i, 4, l2 / (4 * 64), 64, 7, 4, EvictionPolicy.LRU);
			Connection("ProcessorInterface_" + i, "L1_" + i, "Connection", "LocalConnection", 0);
			Connection("L1_" + i, "L2_" + i, "RemoteConnection", "LocalConnection", 0);
			Connection("L2_" + i, "MainMemBus", "RemoteConnection", "LocalConnection", 0);
		}
		Connection("MainMemBus", "MainMemory", "RemoteConnection", "Connection", 0);
		output.WriteLine("End");
		output.Close();
	}
	public static void OutSimpleMemory3(int nodeCount, int l1, int l2, int l3)
	{
		output = new System.IO.StreamWriter("memoryConfigs\\SimpleThreeLevel_p" + nodeCount + "_c" + l1 + "L1-" + l2 + "L2-" + l3 + "L3.memory");
		index = 1;
		output.WriteLine("Begin");
		MainMemory(400, 300);
		l1 = l1 * 1024;
		l2 = l2 * 1024;
		l3 = l3 * 1024;
		Bus("MainMemBus", 5, 2);
		AddCache("L3", 4, l3 / (4 * 64), 64, 20, 15, EvictionPolicy.LRU);
		for (int i = 0; i < nodeCount; i++)
		{
			AddSESCInterface("ProcessorInterface_" + i);
			AddCache("L1_" + i, 4, l1 / (4 * 64), 64, 3, 1, EvictionPolicy.LRU);
			AddCache("L2_" + i, 4, l2 / (4 * 64), 64, 7, 4, EvictionPolicy.LRU);
			Connection("ProcessorInterface_" + i, "L1_" + i, "Connection", "LocalConnection", 0);
			Connection("L1_" + i, "L2_" + i, "RemoteConnection", "LocalConnection", 0);
			Connection("L2_" + i, "MainMemBus", "RemoteConnection", "LocalConnection", 0);
		}
		Connection("MainMemBus", "L3", "RemoteConnection", "LocalConnection", 0);
		Connection("L3", "MainMemory", "RemoteConnection", "Connection", 0);
		output.WriteLine("End");
		output.Close();
	}
	public static void OutDirectoryMemory(int nodeCount, int l1, int l2)
	{
		output = new System.IO.StreamWriter("memoryConfigs\\Directory_p" + nodeCount + "_c" + l1 + "L1-" + l2 + "L2.memory");
		index = 1;
		output.WriteLine("Begin");
		MainMemory(400, 300);
		AddNetwork("Network", nodeCount, 4, 20, 0.1f);
		AddNetworkMemoryInterface("NMInt", nodeCount + 10);
		l1 *= 1024;
		l2 *= 1024;
		for (int i = 0; i < nodeCount; i++)
		{
			AddSESCInterface("ProcessorInterface_" + i);
			AddCache("L1_" + i, 4, l1 / (4 * 64), 64, 3, 1, EvictionPolicy.LRU);
			AddCache("L2_" + i, 4, l2 / (4 * 64), 64, 7, 4, EvictionPolicy.LRU);
			AddDirectory("Directory_" + i, nodeCount, i, nodeCount + 10);
			Connection("ProcessorInterface_" + i, "L1_" + i, "Connection", "LocalConnection", 0);
			Connection("L1_" + i, "L2_" + i, "RemoteConnection", "LocalConnection", 0);
			Connection("L2_" + i, "Directory_" + i, "RemoteConnection", "LocalConnection", 0);
			Connection("Directory_" + i, "Network", "RemoteConnection", "Connection_" + i, 0);
		}
		Connection("Network", "NMInt", "MemoryConnection", "NetworkConnection", 0);
		Connection("NMInt", "MainMemory", "MemoryConnection", "Remoteconnection", 0);
		output.WriteLine("End");
		output.Close();
	}
    public static void OutOriginDirectoryMemory(int nodeCount, int l1, int l2)
    {
        output = new System.IO.StreamWriter("memoryConfigs\\OriginDirectory_p" + nodeCount + "_c" + l1 + "L1-" + l2 + "L2.memory");
        index = 1;
        output.WriteLine("Begin");
        //MainMemory(400, 300);
        AddNetwork("Network", nodeCount, 4, 20, 0.1f);
        //AddNetworkMemoryInterface("NMInt", nodeCount + 10);
        l1 *= 1024;
        l2 *= 1024;
        for (int i = 0; i < nodeCount; i++)
        {
            AddSESCInterface("ProcessorInterface_" + i);
            AddCache("L1_" + i, 4, l1 / (4 * 64), 64, 3, 1, EvictionPolicy.LRU);
            AddCache("L2_" + i, 4, l2 / (4 * 64), 64, 7, 4, EvictionPolicy.LRU);
            AddDirectory("Directory_" + i, nodeCount, i, nodeCount + 10);
            AddMainMemory("MainMemory_" + i, 400, 300);
            Connection("ProcessorInterface_" + i, "L1_" + i, "Connection", "LocalConnection", 0);
            Connection("L1_" + i, "L2_" + i, "RemoteConnection", "LocalConnection", 0);
            Connection("L2_" + i, "Directory_" + i, "RemoteConnection", "LocalCacheConnection", 0);
            Connection("MainMemory_" + i, "Directory_" + i, "RemoteConnection", "LocalMemoryConnection", 0);
            Connection("Directory_" + i, "Network", "RemoteConnection", "Connection_" + i, 0);
        }
        //Connection("Network", "NMInt", "MemoryConnection", "NetworkConnection", 0);
        //Connection("NMInt", "MainMemory", "MemoryConnection", "Remoteconnection", 0);
        output.WriteLine("End");
        output.Close();
    }
	public static void Main(string[] args)
	{
		System.IO.Directory.CreateDirectory("memoryConfigs");
		for (int i = 1; i <= 128; i *= 2)
		{
			for (int l1 = 1; l1 <= 1024; l1 *= 2)
			{
//				OutSimpleMemory1(i, l1);
				for (int l2 = l1 * 2; l2 <= 8 * 1024; l2 *= 2)
				{
					OutOriginDirectoryMemory(i, l1, l2);
					//OutSimpleMemory2(i, l1, l2);
					for (int l3 = l2 * 2; l3 <= 64 * 1024; l3 *= 2)
					{
//						OutSimpleMemory3(i, l1, l2, l3);
					}
				}
			}
		}
	}
}
