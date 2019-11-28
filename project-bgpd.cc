/* -*- Mode:C++; c-file-style:"gnu"; indent-tabs-mode:nil; -*- */
/*
 * Copyright (c) 2012 Hajime Tazaki, NICT
 *
 * This program is free software; you can redistribute it and/or modify
 * it under the terms of the GNU General Public License version 2 as
 * published by the Free Software Foundation;
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with this program; if not, write to the Free Software
 * Foundation, Inc., 59 Temple Place, Suite 330, Boston, MA  02111-1307  USA
 *
 * Author: Hajime Tazaki <tazaki@nict.go.jp>
 */

#include "ns3/network-module.h"
#include "ns3/core-module.h"
#include "ns3/internet-module.h"
#include "ns3/dce-module.h"
#include "ns3/quagga-helper.h"
#include "ns3/point-to-point-helper.h"
#include "ns3/netanim-module.h"

using namespace ns3;
NS_LOG_COMPONENT_DEFINE ("DceQuaggaBgpd");

// Parameters
uint32_t nNodes = 4;
uint32_t stopTime = 1000;


ApplicationContainer GenerateConfigBgp (Ptr<ns3::Node> node, int configuration);
int main (int argc, char *argv[])
{
    //
    //  Step 0
    //  Node Basic Configuration
    //
    
    CommandLine cmd;
    cmd.AddValue ("nNodes", "Number of Router nodes", nNodes);
    cmd.AddValue ("stopTime", "Time to stop(seconds)", stopTime);
    cmd.Parse (argc,argv);
    
    //
    //  Step 1
    //  Node Basic Configuration
    //
    NodeContainer nodes;
    nodes.Create (nNodes);
    // extra contrainer for node 1 and 2
    NodeContainer nodes1;
    // container for node 3
    NodeContainer nodes2;
    NodeContainer nodes3;

    nodes1.Add(nodes.Get(0));
    nodes1.Add(nodes.Get(1));
    nodes2.Add(nodes.Get(1));
    nodes2.Add(nodes.Get(2));
    nodes3.Add(nodes.Get(2));
    nodes3.Add(nodes.Get(3));

    // Address conf In virtual topology
    PointToPointHelper pointToPoint;
    pointToPoint.SetDeviceAttribute ("DataRate", StringValue ("5Mbps"));
    pointToPoint.SetChannelAttribute ("Delay", StringValue ("2ms"));
    
    NetDeviceContainer devices1;
    NetDeviceContainer devices2;
    NetDeviceContainer devices3;
    devices1 = pointToPoint.Install (nodes1);
    devices2 = pointToPoint.Install (nodes2);
    devices3 = pointToPoint.Install (nodes3);

    
    // Internet stack install
    InternetStackHelper stack;    // IPv4 is required for GlobalRouteMan
    Ipv4DceRoutingHelper ipv4RoutingHelper;	//required for quagga
    stack.SetRoutingHelper (ipv4RoutingHelper);
    stack.Install (nodes);
    
    Ipv4AddressHelper ipv4AddrHelper;
    ipv4AddrHelper.SetBase ("10.0.0.0", "255.255.255.252");
    Ipv4InterfaceContainer interfaces = ipv4AddrHelper.Assign (devices1);
    ipv4AddrHelper.NewNetwork();
    interfaces = ipv4AddrHelper.Assign (devices2);
    ipv4AddrHelper.NewNetwork();
    interfaces = ipv4AddrHelper.Assign (devices3);

    Ipv4GlobalRoutingHelper::PopulateRoutingTables ();
    
    DceManagerHelper processManager;
    processManager.SetNetworkStack ("ns3::Ns3SocketFdFactory");  // required for pcap
   
    processManager.Install (nodes);
    

    /*
     QuaggaHelper quagga;
     quagga.EnableBgp (nodes);
     quagga.BgpAddNeighbor (nodes.Get (0), "10.0.0.2", quagga.GetAsn (nodes.Get (1)));
     quagga.BgpAddNeighbor (nodes.Get (1), "10.0.0.1", quagga.GetAsn (nodes.Get (0)));
     quagga.EnableZebraDebug (nodes);
     quagga.Install (nodes);
     */
    
    ApplicationContainer apps;
    
    // call the generation of the configuration function
    apps.Add(GenerateConfigBgp(nodes.Get(0),1));
    apps.Add(GenerateConfigBgp(nodes.Get(1),2));
    apps.Add(GenerateConfigBgp(nodes.Get(2),3));
    apps.Add(GenerateConfigBgp(nodes.Get(3),4));


    //RunIp (nodes.Get (0), Seconds (20.0), "bgp show");


    Ptr<OutputStreamWrapper> routingStream = Create<OutputStreamWrapper> ("routes.log", std::ios::out);
    ipv4RoutingHelper.PrintRoutingTableAllEvery (Seconds (20), routingStream);

    //pointToPoint.EnablePcapAll ("project-bgpd");
    
    //
    // Now It's ready to GO!
    //
    Simulator::Stop (Seconds (200.0));
    //AnimationInterface anim("myscripts/project/animation.xml");
    //anim.SetConstantPosition(nodes.Get(0), 10.0, 10.0);
    //anim.SetConstantPosition(nodes.Get(1), 30.0, 30.0);
    Simulator::Run ();
    Simulator::Destroy ();
    
    return 0;
}
//Generating Config of BGP
ApplicationContainer GenerateConfigBgp (Ptr<ns3::Node> node, int configuration)
{	




    // start the configuration of the application
    
     DceApplicationHelper process;
    ApplicationContainer apps;




   


  // config generation
  std::stringstream conf_dir1, conf_file1;
  // FIXME XXX
  conf_dir1 << "files-" << node->GetId () << "";
  ::mkdir (conf_dir1.str ().c_str (), S_IRWXU | S_IRWXG);
  conf_dir1 << "/usr/";
  ::mkdir (conf_dir1.str ().c_str (), S_IRWXU | S_IRWXG);
  conf_dir1 << "/local/";
  ::mkdir (conf_dir1.str ().c_str (), S_IRWXU | S_IRWXG);
  conf_dir1 << "/etc/";
  ::mkdir (conf_dir1.str ().c_str (), S_IRWXU | S_IRWXG);

  conf_file1 << conf_dir1.str() << "/zebra.conf";
  std::string zconfi;
  zconfi ="hostname zebra \n"
        "password zebra \n"
        "log stdout\n"
	"debug zebra kernel \n"
	"debug zebra events \n"
	"debug zebra packet \n";

  std::ofstream conf1;
  conf1.open (conf_file1.str ().c_str ());
  conf1 <<zconfi; 
  conf1.close ();


  
    process.SetBinary ("zebra");
    process.AddArguments ("-f", "/usr/local/etc/zebra.conf");
    process.AddArguments ("-i", "/usr/local/etc/zebra.pid");
    process.SetStackSize (1 << 16);
    apps.Add (process.Install (node));
    apps.Get (0)->SetStartTime (Seconds (1.0 + 0.01 * node->GetId ()));
    node->AddApplication (apps.Get (0));
    //Ptr<BgpConfig> bgp_conf = node->GetObject<BgpConfig> ();
     
    
    // config generation
    std::stringstream conf_dir, conf_file;
    // FIXME XXX
    conf_dir << "files-" << node->GetId () << "";
    ::mkdir (conf_dir.str ().c_str (), S_IRWXU | S_IRWXG);
    conf_dir << "/usr/";
    ::mkdir (conf_dir.str ().c_str (), S_IRWXU | S_IRWXG);
    conf_dir << "/local/";
    ::mkdir (conf_dir.str ().c_str (), S_IRWXU | S_IRWXG);
    conf_dir << "/etc/";
    ::mkdir (conf_dir.str ().c_str (), S_IRWXU | S_IRWXG);
    
    conf_file << conf_dir.str () << "/bgpd.conf";
    //bgp_conf->SetFilename ("/usr/local/etc/bgpd.conf");
    
    std::ofstream conf;
    conf.open (conf_file.str ().c_str ());
    //bgp_conf->Print (conf);
    std::string confi;
    // passing the different configurations according to the node passed
    if (configuration == 1)
    {
        confi ="hostname bgpd \n"
	"password zebra \n"
	"log stdout \n"
	"debug bgp \n"
	"debug bgp fsm \n"
	"debug bgp events \n"
	"debug bgp updates \n"
        "router bgp 1 \n"
        "   bgp router-id 10.0.0.1 \n"
        "   neighbor 10.0.0.2 remote-as 1 \n"
	"! \n";
    }
    else if (configuration == 2)
    {
        confi = "hostname bgpd \n"
	"password zebra \n"
	"log stdout \n"
	"debug bgp \n"
	"debug bgp fsm \n"
	"debug bgp events \n"
	"debug bgp updates \n"
        "router bgp 1 \n"
        "   bgp router-id 10.0.0.2 \n"
        "   neighbor 10.0.0.1 remote-as 1 \n"
	"   neighbor 10.0.0.1 next-hop-self \n"
        "   neighbor 10.0.0.6 remote-as 2 \n"
	"! \n";
    }
    else if (configuration == 3){
        confi ="hostname bgpd \n"
	"password zebra \n"
	"log stdout \n"
	"debug bgp \n"
	"debug bgp fsm \n"
	"debug bgp events \n"
	"debug bgp updates \n"
        "router bgp 2 \n"
        "   bgp router-id 10.0.1.2 \n"
        "   neighbor 10.0.0.5 remote-as 1 \n"
	"   neighbor 10.0.0.10 remote-as 2 \n"
	"   neighbor 10.0.0.10 next-hop-self \n"
	"! \n";
    }
    else if (configuration == 4){
        confi ="hostname bgpd \n"
	"password zebra \n"
	"log stdout \n"
	"debug bgp \n"
	"debug bgp fsm \n"
	"debug bgp events \n"
	"debug bgp updates \n"
        "router bgp 2 \n"
        "   bgp router-id 10.0.1.3 \n"
        "   neighbor 10.0.0.9 remote-as 2 \n"
	"! \n";
    }

    //bgp_conf->Print (conf);
    conf<<confi;
    conf.close ();

    // setting the configuation
    process.ResetArguments ();
    process.SetBinary ("bgpd");
    process.AddArguments ("-f", "/usr/local/etc/bgpd.conf");
    process.AddArguments ("-i", "/usr/local/etc/bgpd.pid");
    apps = process.Install (node);
    apps.Get (0)->SetStartTime (Seconds (5.0 + 0.3 * node->GetId ()));
    //      apps.Get(0)->SetStartTime (Seconds (1.2 + 0.1 * node->GetId ()));
    node->AddApplication (apps.Get (0));

    return apps;
}

