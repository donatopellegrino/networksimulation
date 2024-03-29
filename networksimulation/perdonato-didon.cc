#include "ns3/network-module.h"

#include "ns3/core-module.h"

#include "ns3/internet-module.h"

#include "ns3/dce-module.h"

#include "ns3/quagga-helper.h"

#include "ns3/point-to-point-helper.h"

#include "ns3/netanim-module.h"

#include "ns3/topology-read-module.h"

#include "ns3/csma-helper.h"

#include "ns3/virtual-net-device.h"

#include <stack>

#include <string>

#include <sstream>

using namespace ns3;
NS_LOG_COMPONENT_DEFINE("DceQuaggaBgpd");

uint32_t stopTime = 10000;





ApplicationContainer GenerateConfigBgp(Ptr < ns3::Node > node, std::string configuration, int id, std::string ASn, int*neighborsN);


int main(int argc, char * argv[]) {
  std::string topoFile = "myscripts/project/topology.intra";
  std::ifstream file(topoFile);
  if (!file) {
    std::cerr << "Unable to open file datafile.txt" << "\n";
    exit(1); // call system to stop
  }
  Ptr < TopologyReader > inFile = 0;
  TopologyReaderHelper topoHelp;
  std::string format("Rocketfuel");
  std::string input(topoFile);
  topoHelp.SetFileName(input);
  topoHelp.SetFileType(format);
  inFile = topoHelp.GetTopologyReader();

  NodeContainer nodes;
  if (inFile != 0) {
    nodes = inFile -> Read();
  }

  if (nodes.GetN() == 0) {
    NS_ASSERT("Problems reading node information the topology file. Failing.");
    return -1;
  }
  if (inFile -> LinksSize() == 0) {
    NS_LOG_ERROR("Problems reading the topology file. Failing.");
    return -1;
  }
  NS_LOG_INFO("Rocketfuel topology created with " << nodes.GetN() << " nodes and " <<
    inFile -> LinksSize() << " links (from " << input << ")");

  NS_LOG_INFO("calculating links types");
  char tempString[100];
  uint32_t lt;
  uint32_t linksTypes[inFile -> LinksSize()];
  for (uint32_t i = 0; i < inFile -> LinksSize(); i++) {
    file >> tempString >> tempString >> lt;
    linksTypes[i] = lt;
    NS_LOG_INFO("lt: " << lt);
  }

  InternetStackHelper stack; // IPv4 is required for GlobalRouteMan
  Ipv4DceRoutingHelper ipv4RoutingHelper;
  stack.SetRoutingHelper(ipv4RoutingHelper);
  stack.Install(nodes);

  int totlinks = inFile -> LinksSize();
  NS_LOG_INFO("creating node containers");
  NodeContainer nc[totlinks];
  TopologyReader::ConstLinksIterator iter;
  int i = 0;
  std::string nodeAS[nodes.GetN()];
  uint32_t nodeCounter = 0;
  for (iter = inFile -> LinksBegin(); iter != inFile -> LinksEnd(); iter++, i++) {
    nc[i] = NodeContainer(iter -> GetFromNode(), iter -> GetToNode());
    if (iter -> GetFromNode() == nodes.Get(nodeCounter)) {
      nodeAS[nodeCounter] = iter -> GetFromNodeName().substr(0, iter -> GetFromNodeName().find(";"));
      NS_LOG_INFO(nodeAS[nodeCounter]);
      nodeCounter++;
    }
    if (iter -> GetToNode() == nodes.Get(nodeCounter)) {
      nodeAS[nodeCounter] = iter -> GetToNodeName().substr(0, iter -> GetToNodeName().find(";"));
      NS_LOG_INFO(nodeAS[nodeCounter]);
      nodeCounter++;
    }
  }
  NS_LOG_INFO("creating net device containers");
  NetDeviceContainer ndc[totlinks];
  PointToPointHelper p2p;
  for (int i = 0; i < totlinks; i++) {
    // p2p.SetChannelAttribute ("Delay", TimeValue(MilliSeconds(weight[i])));
    p2p.SetChannelAttribute("Delay", StringValue("2ms"));
    p2p.SetDeviceAttribute("DataRate", StringValue("5Mbps"));
    ndc[i] = p2p.Install(nc[i]);

  }

  Ipv4AddressHelper ipv4AddrHelper;

  NS_LOG_INFO("creating ip4 addresses");
  Ipv4AddressHelper address;
  address.SetBase("10.0.0.0", "255.255.255.252");

  NS_LOG_INFO("creating ipv4 interfaces");
  Ipv4InterfaceContainer ipic[totlinks];
  for (int i = 0; i < totlinks; i++) {
    ipic[i] = address.Assign(ndc[i]);
    address.NewNetwork();
  }

  //ObjectFactory m_deviceFactory;
  //m_deviceFactory.SetTypeId ("ns3::LoopbackNetDevice"); 
  std::string neighbors[nodes.GetN()];
  
  std::stringstream temp;


/*

  NodeContainer ASs[4];
  ASs[0].Add(nodes.Get(0));
  ASs[1].Add(nodes.Get(1));
  ASs[1].Add(nodes.Get(2));
  ASs[1].Add(nodes.Get(3));
  ASs[2].Add(nodes.Get(4));
  ASs[3].Add(nodes.Get(5));

*/
Ptr<VirtualNetDevice> m_n0Tap[nodes.GetN()];
Ptr<Ipv4> ipv4;
//int i;

  CsmaHelper csma;
  //NetDeviceContainer ndcLoopback[nodes.GetN()];
  //Ptr<LoopbackNetDevice> ndLoopback[nodes.GetN()];
  Ipv4AddressHelper addressLoopback;
  addressLoopback.SetBase("10.10.10.0", "255.255.255.252");
  
  //Ipv4InterfaceContainer intLoopback;


int neighborsN[nodes.GetN()];
for (int i=0; i<nodes.GetN(); i++){
          neighborsN[i] = 0;
  }

  for (int i = 0; i < totlinks; i++) {
    NS_LOG_INFO(i);
    NS_LOG_INFO("device1: " << nc[i].Get(0) -> GetId());
    NS_LOG_INFO("\t" << ipic[i].GetAddress(0, 0));
    NS_LOG_INFO("device2: " << nc[i].Get(1) -> GetId());
    NS_LOG_INFO("\t" << ipic[i].GetAddress(1, 0));

    neighbors[nc[i].Get(0) -> GetId()] += "   neighbor ";
    ipic[i].GetAddress(1, 0).Print(temp);
    neighbors[nc[i].Get(0) -> GetId()] += temp.str();
    neighbors[nc[i].Get(0) -> GetId()] += " remote-as ";
    neighbors[nc[i].Get(0) -> GetId()] += nodeAS[nc[i].Get(1) -> GetId()];
    neighbors[nc[i].Get(0) -> GetId()] += " \n";

    neighborsN[nc[i].Get(0) -> GetId()]++;
 
    temp.str(""); // clear string stream

    neighbors[nc[i].Get(1) -> GetId()] += "   neighbor ";
    ipic[i].GetAddress(0, 0).Print(temp);
    neighbors[nc[i].Get(1) -> GetId()] += temp.str();
    neighbors[nc[i].Get(1) -> GetId()] += " remote-as ";
    neighbors[nc[i].Get(1) -> GetId()] += nodeAS[nc[i].Get(0) -> GetId()];
    neighbors[nc[i].Get(1) -> GetId()] += " \n";

    neighborsN[nc[i].Get(1) -> GetId()]++;

    temp.str(""); // clear string stream
    
    neighbors[nc[i].Get(0) -> GetId()] += "   neighbor ";
        ipic[i].GetAddress(1, 0).Print(temp);
        neighbors[nc[i].Get(0) -> GetId()] += temp.str();
	neighbors[nc[i].Get(0) -> GetId()] += " advertisement-interval 5 \n";
        temp.str("");


	neighbors[nc[i].Get(1) -> GetId()] += "   neighbor ";
        ipic[i].GetAddress(0, 0).Print(temp);
        neighbors[nc[i].Get(1) -> GetId()] += temp.str();
	neighbors[nc[i].Get(1) -> GetId()] += " advertisement-interval 5 \n";
        temp.str("");

   

    if (nodeAS[nc[i].Get(1) -> GetId()] == nodeAS[nc[i].Get(0) -> GetId()]) {

	neighbors[nc[i].Get(0) -> GetId()] += "   neighbor ";
        ipic[i].GetAddress(1, 0).Print(temp);
        neighbors[nc[i].Get(0) -> GetId()] += temp.str();
	neighbors[nc[i].Get(0) -> GetId()] += " next-hop-self \n";
        temp.str("");


	neighbors[nc[i].Get(1) -> GetId()] += "   neighbor ";
        ipic[i].GetAddress(0, 0).Print(temp);
        neighbors[nc[i].Get(1) -> GetId()] += temp.str();
	neighbors[nc[i].Get(1) -> GetId()] += " next-hop-self \n";
        temp.str("");
    }





    //Cagate
    if(i==1){
neighbors[nc[i].Get(0) -> GetId()] += "   neighbor ";
        ipic[i].GetAddress(1, 0).Print(temp);
        neighbors[nc[i].Get(0) -> GetId()] += temp.str();
	neighbors[nc[i].Get(0) -> GetId()] += " weight 3 \n";
        temp.str("");


	neighbors[nc[i].Get(1) -> GetId()] += "   neighbor ";
        ipic[i].GetAddress(0, 0).Print(temp);
        neighbors[nc[i].Get(1) -> GetId()] += temp.str();
	neighbors[nc[i].Get(1) -> GetId()] += " weight 3 \n";
        temp.str("");

    }
    else if(i==2){
neighbors[nc[i].Get(0) -> GetId()] += "   neighbor ";
        ipic[i].GetAddress(1, 0).Print(temp);
        neighbors[nc[i].Get(0) -> GetId()] += temp.str();
	neighbors[nc[i].Get(0) -> GetId()] += " weight 2 \n";
        temp.str("");


	neighbors[nc[i].Get(1) -> GetId()] += "   neighbor ";
        ipic[i].GetAddress(0, 0).Print(temp);
        neighbors[nc[i].Get(1) -> GetId()] += temp.str();
	neighbors[nc[i].Get(1) -> GetId()] += " weight 2 \n";
        temp.str("");

    }
    else if(i==3){
neighbors[nc[i].Get(0) -> GetId()] += "   neighbor ";
        ipic[i].GetAddress(1, 0).Print(temp);
        neighbors[nc[i].Get(0) -> GetId()] += temp.str();
	neighbors[nc[i].Get(0) -> GetId()] += " weight 3 \n";
        temp.str("");


	neighbors[nc[i].Get(1) -> GetId()] += "   neighbor ";
        ipic[i].GetAddress(0, 0).Print(temp);
        neighbors[nc[i].Get(1) -> GetId()] += temp.str();
	neighbors[nc[i].Get(1) -> GetId()] += " weight 3 \n";
        temp.str("");

    }
    else if(i==4){
neighbors[nc[i].Get(0) -> GetId()] += "   neighbor ";
        ipic[i].GetAddress(1, 0).Print(temp);
        neighbors[nc[i].Get(0) -> GetId()] += temp.str();
	neighbors[nc[i].Get(0) -> GetId()] += " weight 500 \n";
        temp.str("");


	neighbors[nc[i].Get(1) -> GetId()] += "   neighbor ";
        ipic[i].GetAddress(0, 0).Print(temp);
        neighbors[nc[i].Get(1) -> GetId()] += temp.str();
	neighbors[nc[i].Get(1) -> GetId()] += " weight 500 \n";
        temp.str("");

    }
    else if(i==5) {
neighbors[nc[i].Get(0) -> GetId()] += "   neighbor ";
        ipic[i].GetAddress(1, 0).Print(temp);
        neighbors[nc[i].Get(0) -> GetId()] += temp.str();
	neighbors[nc[i].Get(0) -> GetId()] += " weight 51 \n";
        temp.str("");


	neighbors[nc[i].Get(1) -> GetId()] += "   neighbor ";
        ipic[i].GetAddress(0, 0).Print(temp);
        neighbors[nc[i].Get(1) -> GetId()] += temp.str();
	neighbors[nc[i].Get(1) -> GetId()] += " weight 51 \n";
        temp.str("");

    }

    else {
neighbors[nc[i].Get(0) -> GetId()] += "   neighbor ";
        ipic[i].GetAddress(1, 0).Print(temp);
        neighbors[nc[i].Get(0) -> GetId()] += temp.str();
	neighbors[nc[i].Get(0) -> GetId()] += " weight 1 \n";
        temp.str("");


	neighbors[nc[i].Get(1) -> GetId()] += "   neighbor ";
        ipic[i].GetAddress(0, 0).Print(temp);
        neighbors[nc[i].Get(1) -> GetId()] += temp.str();
	neighbors[nc[i].Get(1) -> GetId()] += " weight 1 \n";
        temp.str("");

    }
  }

  for(int i=0; i<nodes.GetN();i++){
          NS_LOG_INFO("test 666 nodo" << i << ": " << neighborsN[i]);
  }

  NS_LOG_INFO("commands:");
  for (int i = 0; i < nodes.GetN(); i++) {
    NS_LOG_INFO(neighbors[i]);
  }

  Ipv4GlobalRoutingHelper::PopulateRoutingTables();

  DceManagerHelper processManager;
  processManager.SetNetworkStack("ns3::Ns3SocketFdFactory"); // required for pcap




  processManager.Install(nodes);

  for (int i = 0; i < nodes.GetN(); i++) {
    GenerateConfigBgp(nodes.Get(i), neighbors[i], i, nodeAS[i], neighborsN);
  }
/*

QuaggaHelper quagga;
  //for(i=0;i<4;i++){
//quagga.EnableOspf (ASs[1], "200.10.10.0/23");
quagga.EnableOspf (ASs[1], "10.0.0.0/8");

quagga.EnableOspfDebug (ASs[1]);
      quagga.EnableZebraDebug (ASs[1]);
      quagga.Install (ASs[1]);
  //}

*/
  Ptr < OutputStreamWrapper > routingStream = Create < OutputStreamWrapper > ("routes.log", std::ios::out);
  ipv4RoutingHelper.PrintRoutingTableAllEvery(Seconds(20), routingStream);

NS_LOG_INFO("uuuuuuuuuuuuuuuuuuuuuuuuuuuuuu");
  Simulator::Stop(Seconds(500.0));
  Simulator::Run();
  Simulator::Destroy();

  return 0;
}

//Generating Config of BGP
  ApplicationContainer GenerateConfigBgp(Ptr<ns3::Node> node, std::string configuration, int id, std::string ASn, int*neighborsN) {
  DceApplicationHelper process;
  ApplicationContainer apps;

  // config generation
  std::stringstream conf_dir1, conf_file1;
  // FIXME XXX
  conf_dir1 << "files-" << node -> GetId() << "";::mkdir(conf_dir1.str().c_str(), S_IRWXU | S_IRWXG);
  conf_dir1 << "/usr/";::mkdir(conf_dir1.str().c_str(), S_IRWXU | S_IRWXG);
  conf_dir1 << "/local/";::mkdir(conf_dir1.str().c_str(), S_IRWXU | S_IRWXG);
  conf_dir1 << "/etc/";::mkdir(conf_dir1.str().c_str(), S_IRWXU | S_IRWXG);

  conf_file1 << conf_dir1.str() << "/zebra.conf";
  std::string zconfi;
  zconfi = "hostname zebra \n"
  "password zebra \n"
  "log stdout \n"
  "debug zebra kernel \n"
  "debug zebra events \n"
  "debug zebra packet \n";

  std::ofstream conf1;
  conf1.open(conf_file1.str().c_str());
  conf1 << zconfi;
  conf1.close();

  process.SetBinary("zebra");
  process.AddArguments("-f", "/usr/local/etc/zebra.conf");
  process.AddArguments("-i", "/usr/local/etc/zebra.pid");
  process.SetStackSize(1 << 16);
  apps.Add(process.Install(node));
  apps.Get(0) -> SetStartTime(Seconds(1.0 + 0.01 * node -> GetId()));
  node -> AddApplication(apps.Get(0));
  //Ptr<BgpConfig> bgp_conf = node->GetObject<BgpConfig> ();

  // config generation
  std::stringstream conf_dir, conf_file;
  // FIXME XXX
  conf_dir << "files-" << node -> GetId() << "";::mkdir(conf_dir.str().c_str(), S_IRWXU | S_IRWXG);
  conf_dir << "/usr/";::mkdir(conf_dir.str().c_str(), S_IRWXU | S_IRWXG);
  conf_dir << "/local/";::mkdir(conf_dir.str().c_str(), S_IRWXU | S_IRWXG);
  conf_dir << "/etc/";::mkdir(conf_dir.str().c_str(), S_IRWXU | S_IRWXG);

  conf_file << conf_dir.str() << "/bgpd.conf";
  //bgp_conf->SetFilename ("/usr/local/etc/bgpd.conf");

  std::ofstream conf;
  conf.open(conf_file.str().c_str());
  //bgp_conf->Print (conf);
  // passing the different configurations according to the node passed
  conf << "hostname bgpd \n"
  "password zebra \n"
  "log stdout \n"
  "debug bgp \n"
  //"debug bgp fsm \n"
  "debug bgp events \n"
  "debug bgp updates \n"
  "router bgp " << ASn << " \n"
  "   bgp router-id 10.0.0."<<id<<" \n"
  "   no synchronization \n"
  "   bgp log-neighbor-changes \n"
  "   network " << std::stoi(ASn) + 1 << "0.0.0.0 mask 255.255.255.0 \n" <<
    configuration <<
    "! \n";
  //bgp_conf->Print (conf);
  conf.close();

  // setting the configuation
  process.ResetArguments();
  process.SetBinary("bgpd");
  process.AddArguments("-f", "/usr/local/etc/bgpd.conf");
  process.AddArguments("-i", "/usr/local/etc/bgpd.pid");
  apps = process.Install(node);
  apps.Get(0) -> SetStartTime(Seconds(5.0 + 0.3 * node -> GetId()));
  //      apps.Get(0)->SetStartTime (Seconds (1.2 + 0.1 * node->GetId ()));
  node -> AddApplication(apps.Get(0));

  // number of neighbors generation
  std::stringstream conf_dir2, conf_file2;
  // FIXME XXX
  conf_dir2 << "files-" << node -> GetId() << "";::mkdir(conf_dir2.str().c_str(), S_IRWXU | S_IRWXG);
  conf_file2 << conf_dir2.str() << "/neighborsN";
  std::ofstream conf2;
  conf2.open(conf_file2.str().c_str());
  conf2 << neighborsN[id];
  conf2.close();

  // votesRequestsFile generation
  std::stringstream conf_dir3, conf_file3;
  // FIXME XXX
  conf_dir3 << "files-" << node -> GetId() << "";::mkdir(conf_dir3.str().c_str(), S_IRWXU | S_IRWXG);
  conf_file3 << conf_dir3.str() << "/votesRequests";
  std::ofstream conf3;
  conf3.open(conf_file3.str().c_str());
  conf3 << "0\n0\n0\n0\n0\n0\n0\n0\n0\n0\n";
  conf3.close();

  // votesFile generation
  std::stringstream conf_dir4, conf_file4;
  // FIXME XXX
  conf_dir4 << "files-" << node -> GetId() << "";::mkdir(conf_dir4.str().c_str(), S_IRWXU | S_IRWXG);
  conf_file4 << conf_dir4.str() << "/votes";
  std::ofstream conf4;
  conf4.open(conf_file4.str().c_str());
  conf4 << "0\n0\n0\n0\n0\n0\n0\n0\n0\n0\n";
  conf4.close();

  // votesNumberFile generation
  std::stringstream conf_dir5, conf_file5;
  // FIXME XXX
  conf_dir5 << "files-" << node -> GetId() << "";::mkdir(conf_dir5.str().c_str(), S_IRWXU | S_IRWXG);
  conf_file5 << conf_dir5.str() << "/votesNumber";
  std::ofstream conf5;
  conf5.open(conf_file5.str().c_str());
  conf5 << "";
  conf5.close();

  // votesNumberFile generation
  std::stringstream conf_dir6, conf_file6;
  // FIXME XXX
  conf_dir6 << "files-" << node -> GetId() << "";::mkdir(conf_dir6.str().c_str(), S_IRWXU | S_IRWXG);
  conf_file6 << conf_dir6.str() << "/votesResponses";
  std::ofstream conf6;
  conf6.open(conf_file6.str().c_str());
  conf6 << "0\n0\n0\n0\n0\n0\n0\n0\n0\n0\n";
  conf6.close();

  return apps;
}
