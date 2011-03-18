public static class MemGen
{
	public enum EvictionPolicy
	{
		LRU,
		Random,
	}

    public class NetworkTimingData
    {
        public int randomMin = 0;
        public int randomMax = 0;
    }

    public static NetworkTimingData GetNetworkTimingData(int nodeCount, float multiplyFactor)
    {
        NetworkTimingData _ret = new NetworkTimingData();

        switch (nodeCount)
        {
            case 4:
                _ret.randomMin = 100;
                _ret.randomMax = 110;
                //_ret.randomMax = 116;
                break;
            case 8:
                _ret.randomMin = 131;
                _ret.randomMax = 145;
                //_ret.randomMax = 152;
                break;
            case 16:
                _ret.randomMin = 135;
                _ret.randomMax = 149;
                //_ret.randomMax = 156;
                break;
            case 32:
                _ret.randomMin = 143;
                _ret.randomMax = 159;
                //_ret.randomMax = 166;
                break;
            case 64:
                _ret.randomMin = 161;
                _ret.randomMax = 177;
                //_ret.randomMax = 186;
                break;
            case 128:
                _ret.randomMin = 175;
                _ret.randomMax = 193;
                //_ret.randomMax = 202;
                break;
            default:
                _ret.randomMin = 57;
                _ret.randomMax = 66;
                break;
        }

        _ret.randomMin = 100;
        _ret.randomMax = 110;

        float tempMin = _ret.randomMin;
        float tempMax = _ret.randomMax;

        tempMin *= multiplyFactor;
        tempMax *= multiplyFactor;

        _ret.randomMin = (int)tempMin;
        _ret.randomMax = (int)tempMax;

        return _ret;
    }

    public static void AddBIPCache(string deviceName, int associativity, int setCount, int lineSize, int missTime, int hitTime, EvictionPolicy ev)
    {
        output.WriteLine("Root MemoryDevice Begin");
        output.WriteLine("String DeviceType BIPMOESICache");
        output.WriteLine("String DeviceName " + deviceName);
        output.WriteLine("Int DeviceID " + (index++));
        output.WriteLine("Int LineSize " + lineSize);
        output.WriteLine("Int Associativity " + associativity);
        output.WriteLine("Int SetCount " + setCount);
        output.WriteLine("Int HitTime " + hitTime);
        output.WriteLine("Int MissTime " + missTime);
        if (deviceName.Contains("L1"))
        {
            output.WriteLine("Int TopCache 1");
        }
        else
        {
            output.WriteLine("Int TopCache 0");
        }
        if (ev == EvictionPolicy.LRU)
        {
            output.WriteLine("String EvictionPolicy LRU");
        }
        else
        {
            output.WriteLine("String EvictionPolicy Random");
        }
        output.WriteLine("End");
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
		output.WriteLine("String DeviceType Directory");
		output.WriteLine("String DeviceName " + name);
		//output.WriteLine("Int LocalSendTime " + 4);
        // use the following one to simulate against Origin
        output.WriteLine("Int LocalSendTime " + 60);
		output.WriteLine("Int RemoteSendTime " + 4);
		output.WriteLine("Int LookupRetryTime " + 60);
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

    public static void AddOriginDirectory(string name, int totalNodes, int myNode)
    {
        output.WriteLine("Root MemoryDevice Begin");
        output.WriteLine("Int DeviceID " + (index++));
        output.WriteLine("String DeviceType OriginDirectory");
        output.WriteLine("String DeviceName " + name);
        output.WriteLine("Int LocalSendTime " + 60);
        output.WriteLine("Int RemoteSendTime " + 4);
        output.WriteLine("Int LookupRetryTime " + 60);
        output.WriteLine("Int LookupTime " + 4);
        output.WriteLine("Int SatisfyTime " + 4);
        output.WriteLine("Int NodeID " + myNode);
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

    public static void AddBIPDirectory(string name, int totalNodes, int myNode)
    {
        output.WriteLine("Root MemoryDevice Begin");
        output.WriteLine("Int DeviceID " + (index++));
        output.WriteLine("String DeviceType BIPDirectory");
        output.WriteLine("String DeviceName " + name);
        output.WriteLine("Int LocalSendTime " + 60);
        output.WriteLine("Int RemoteSendTime " + 4);
        output.WriteLine("Int LookupRetryTime " + 60);
        output.WriteLine("Int LookupTime " + 4);
        output.WriteLine("Int SatisfyTime " + 4);
        output.WriteLine("Int NodeID " + myNode);
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
    public static void AddOriginNetwork(string name, int totalNodes, int randomMin, int randomMax, float perPacket)
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
        output.WriteLine("Root CalculatorConfig Begin");
        output.WriteLine("String Type RandomLoaded");
        output.WriteLine("Int MinTime " + randomMin);
        output.WriteLine("Int MaxTime " + randomMax);
        output.WriteLine("Int TimePerByte 1");
        output.WriteLine("Real TimePerPacket " + perPacket);
        output.WriteLine("Int EnforceOrder 0");
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

	public static void OutDirectoryMemory(int nodeCount, int l1, int l2, float multiplyFactor, string filesysSeperator,
       string filenameAddition)
	{
        string outputString = "memoryConfigs" + filesysSeperator + filenameAddition + "directory-p" 
            + nodeCount + "-c" + l1 + "L1-" + l2 + "L2.memory";
        output = new System.IO.StreamWriter(outputString);
		index = 1;
		output.WriteLine("Begin");
        MainMemory(57, 66);
		
        NetworkTimingData myNetworkTimingData = GetNetworkTimingData(nodeCount, multiplyFactor);
        int randomMin = myNetworkTimingData.randomMin;
        int randomMax = myNetworkTimingData.randomMax;
        //AddNetwork("Network", nodeCount, 4, 20, 0.1f);
        AddNetwork("Network", nodeCount, randomMin, randomMax, 0.1f);

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

    public static void OutOriginDirectoryMemory(int nodeCount, int l1, int l2, float multiplyFactor, string filesysSeperator,
        string filenameAddition)
    {
        string outputString = "memoryConfigs" + filesysSeperator + filenameAddition + "origin-p" +
            nodeCount + "-c" + l1 + "L1-" + l2 + "L2.memory";
        output = new System.IO.StreamWriter(outputString);
        index = 1;
        output.WriteLine("Begin");
        //MainMemory(400, 300);
        NetworkTimingData myNetworkTimingData = GetNetworkTimingData(nodeCount, multiplyFactor);
        int randomMin = myNetworkTimingData.randomMin;
        int randomMax = myNetworkTimingData.randomMax;
        AddOriginNetwork("Network", nodeCount, randomMin, randomMax, 0.1f);
        //AddNetworkMemoryInterface("NMInt", nodeCount + 10);
        l1 *= 1024;
        l2 *= 1024;
        for (int i = 0; i < nodeCount; i++)
        {
            AddSESCInterface("ProcessorInterface_" + i);
            AddCache("L1_" + i, 4, l1 / (4 * 64), 64, 2, 1, EvictionPolicy.LRU);
            // AddCache("L2_" + i, 4, l2 / (4 * 64), 64, 7, 4, EvictionPolicy.LRU);
            AddCache("L2_" + i, 4, l2 / (4 * 64), 64, 13, 10, EvictionPolicy.LRU);
            //AddOriginDirectory("OriginDirectory_" + i, nodeCount, i, nodeCount + 10);
            AddOriginDirectory("OriginDirectory_" + i, nodeCount, i);
            AddMainMemory("MainMemory_" + i, 57, 66);
            Connection("ProcessorInterface_" + i, "L1_" + i, "Connection", "LocalConnection", 0);
            Connection("L1_" + i, "L2_" + i, "RemoteConnection", "LocalConnection", 0);
            Connection("L2_" + i, "OriginDirectory_" + i, "RemoteConnection", "LocalCacheConnection", 0);
            Connection("MainMemory_" + i, "OriginDirectory_" + i, "RemoteConnection", "LocalMemoryConnection", 0);
            Connection("OriginDirectory_" + i, "Network", "RemoteConnection", "Connection_" + i, 0);
        }
        //Connection("Network", "NMInt", "MemoryConnection", "NetworkConnection", 0);
        //Connection("NMInt", "MainMemory", "MemoryConnection", "Remoteconnection", 0);
        output.WriteLine("End");
        output.Close();
    }

    public static void OutBIPDirectoryMemory(int nodeCount, int l1, int l2, float multiplyFactor, string filesysSeperator,
        string filenameAddition)
    {
        string outputString = "memoryConfigs" + filesysSeperator + filenameAddition + "bip-p" + nodeCount 
            + "-c" + l1 + "L1-" + l2 + "L2.memory";
        output = new System.IO.StreamWriter(outputString);
        index = 1;
        output.WriteLine("Begin");
        //MainMemory(400, 300);
        NetworkTimingData myNetworkTimingData = GetNetworkTimingData(nodeCount, multiplyFactor);
        int randomMin = myNetworkTimingData.randomMin;
        int randomMax = myNetworkTimingData.randomMax;
        AddOriginNetwork("Network", nodeCount, randomMin, randomMax, 0.1f);
        //AddNetworkMemoryInterface("NMInt", nodeCount + 10);
        l1 *= 1024;
        l2 *= 1024;
        for (int i = 0; i < nodeCount; i++)
        {
            AddSESCInterface("ProcessorInterface_" + i);
            AddBIPCache("L1_" + i, 4, l1 / (4 * 64), 64, 2, 1, EvictionPolicy.LRU);
            // AddCache("L2_" + i, 4, l2 / (4 * 64), 64, 7, 4, EvictionPolicy.LRU);
            AddBIPCache("L2_" + i, 4, l2 / (4 * 64), 64, 13, 10, EvictionPolicy.LRU);
            //AddOriginDirectory("OriginDirectory_" + i, nodeCount, i, nodeCount + 10);
            AddBIPDirectory("BIPDirectory_" + i, nodeCount, i);
            AddMainMemory("MainMemory_" + i, 57, 66);
            Connection("ProcessorInterface_" + i, "L1_" + i, "Connection", "LocalConnection", 0);
            Connection("L1_" + i, "L2_" + i, "RemoteConnection", "LocalConnection", 0);
            Connection("L2_" + i, "BIPDirectory_" + i, "RemoteConnection", "LocalCacheConnection", 0);
            Connection("MainMemory_" + i, "BIPDirectory_" + i, "RemoteConnection", "LocalMemoryConnection", 0);
            Connection("BIPDirectory_" + i, "Network", "RemoteConnection", "Connection_" + i, 0);
        }
        //Connection("Network", "NMInt", "MemoryConnection", "NetworkConnection", 0);
        //Connection("NMInt", "MainMemory", "MemoryConnection", "Remoteconnection", 0);
        output.WriteLine("End");
        output.Close();
    }

	public static void Main(string[] args)
	{
		System.IO.Directory.CreateDirectory("memoryConfigs");

      float networkMultiplyFactor = 1.0f;
      string filesysSeperator = "/";   // unix
      //string filesysSeperator = "\\";   // windows
      string filenameAddition = "network10-";
      //string filenameAddition = "";
        // nodeCount also determines the total number of processors
		//for (int nodeCount = 4; nodeCount <= 32; nodeCount *= 2)
		for (int nodeCount = 2; nodeCount <= 2; nodeCount *= 2)
		{
			for (int l1 = 64; l1 <= 64; l1 *= 2)
            //for (int l1 = 8; l1 <= 1024; l1 *= 2)
			{
//				OutSimpleMemory1(i, l1);
				//for (int l2 = l1 * 2; l2 <= 4 * 1024; l2 *= 2)
            for (int l2 = l1 * 2; l2 <= 128; l2 *= 2)
				//for (int l2 = 1024; l2 <= 1024; l2 *= 2)
				{
                    OutBIPDirectoryMemory(nodeCount, l1, l2, networkMultiplyFactor, filesysSeperator,filenameAddition);
                    //OutDirectoryMemory(nodeCount, l1, l2, networkMultiplyFactor, filesysSeperator,filenameAddition);
					OutOriginDirectoryMemory(nodeCount, l1, l2, networkMultiplyFactor, filesysSeperator,filenameAddition);
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
