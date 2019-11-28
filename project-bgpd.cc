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

#include "ns3/applications-module.h"
 //#include "ns3/netanim-module.h"

using namespace ns3;
NS_LOG_COMPONENT_DEFINE("DceQuaggaBgpd");

// Parameters
uint32_t nNodes = 3;
uint32_t stopTime = 10;




int main(int argc, char * argv[]) {
	//
	//  Step 0
	//  Node Basic Configuration
	//

	CommandLine cmd;
	cmd.AddValue("nNodes", "Number of Router nodes", nNodes);
	cmd.AddValue("stopTime", "Time to stop(seconds)", stopTime);
	cmd.Parse(argc, argv);

	//
	//  Step 1
	//  Node Basic Configuration
	//
	NodeContainer nodes;
	nodes.Create(nNodes);

	// Address conf In virtual topology
	PointToPointHelper pointToPoint;
	pointToPoint.SetDeviceAttribute("DataRate", StringValue("5Mbps"));
	pointToPoint.SetChannelAttribute("Delay", StringValue("2ms"));


	NetDeviceContainer ndc[2];
	NodeContainer nc[2];
	nc[0].Add(nodes.Get(0));
	nc[0].Add(nodes.Get(1));
	ndc[0] = pointToPoint.Install(nc[0]);
	nc[1].Add(nodes.Get(1));
	nc[1].Add(nodes.Get(2));
	ndc[1] = pointToPoint.Install(nc[1]);

	




	// Internet stack install
	InternetStackHelper stack; // IPv4 is required for GlobalRouteMan
	Ipv4DceRoutingHelper ipv4RoutingHelper; //required for quagga
	stack.SetRoutingHelper(ipv4RoutingHelper);
	stack.Install(nodes);

	Ipv4AddressHelper ipv4AddrHelper;
	ipv4AddrHelper.SetBase("10.0.0.0", "255.255.255.0");
	Ipv4InterfaceContainer interfaces;
	interfaces = ipv4AddrHelper.Assign(ndc[0]);
	ipv4AddrHelper.NewNetwork();
	interfaces = ipv4AddrHelper.Assign(ndc[1]);
	Ipv4GlobalRoutingHelper::PopulateRoutingTables();
	
	DceManagerHelper processManager;

	processManager.SetNetworkStack("ns3::Ns3SocketFdFactory"); // required for pcap
	processManager.Install(nodes);
	
	QuaggaHelper quagga;
	quagga.EnableBgp(nodes);
	quagga.BgpAddNeighbor(nodes.Get(0), "10.0.0.2", quagga.GetAsn(nodes.Get(1)));
	quagga.BgpAddNeighbor(nodes.Get(1), "10.0.0.1", quagga.GetAsn(nodes.Get(0)));
	quagga.BgpAddNeighbor(nodes.Get(1), "10.0.1.2", quagga.GetAsn(nodes.Get(2)));
	quagga.BgpAddNeighbor(nodes.Get(2), "10.0.1.1", quagga.GetAsn(nodes.Get(1)));
	quagga.BgpAddPeerLink(nodes.Get(0), "10.0.0.2");
	quagga.BgpAddPeerLink(nodes.Get(1), "10.0.0.1");
	quagga.BgpAddPeerLink(nodes.Get(1), "10.0.1.2");
	quagga.BgpAddPeerLink(nodes.Get(2), "10.0.1.1");
	quagga.Install(nodes);
	NS_LOG_ERROR ("ciaoooooooooooooo"<< quagga.GetAsn(nodes.Get(2)));

	
	//pointToPoint.EnablePcapAll("project-bgpd");
		
	//uint32_t systemId = 0;
	DceApplicationHelper dce;
	ApplicationContainer apps;

	dce.SetStackSize(1 << 20);

	// if ( systemId == nodes.Get(0)->GetSystemId () )
	// {
	// Launch ping on node 0
	dce.SetBinary("ping");
	dce.ResetArguments();
	dce.ResetEnvironment();
	dce.AddArgument("-c 10");
	dce.AddArgument("-s 1000");
	dce.AddArgument("10.0.1.1");

	apps = dce.Install(nodes.Get(2));
	apps.Start(Seconds(15.0));
	apps.Stop(Seconds(17.0));

	DceApplicationHelper dce2;
	ApplicationContainer apps2;

	dce2.SetStackSize(1 << 20);

	// if ( systemId == nodes.Get(0)->GetSystemId () )
	// {
	// Launch ping on node 0
	dce2.SetBinary("ping");
	dce2.ResetArguments();
	dce2.ResetEnvironment();
	dce2.AddArgument("-c 10");
	dce2.AddArgument("-s 1000");
	dce2.AddArgument("10.0.0.1");

	apps2 = dce2.Install(nodes.Get(2));
	apps2.Start(Seconds(15.0));
	apps2.Stop(Seconds(17.0));





	// if ( systemId == nodes.Get(2)->GetSystemId ())
	// {
	//   // Launch ping on node 1
	//   dce.SetBinary ("ping");
	//   dce.ResetArguments ();
	//   dce.ResetEnvironment ();
	//   dce.AddArgument ("-c 10");
	//   dce.AddArgument ("-s 1000");
	//   dce.AddArgument ("10.0.0.1");

	//   apps = dce.Install (nodes.Get(2));
	//   apps.Start (Seconds (1.5));
	// }
	
	//
	// Now It's ready to GO!
	//
	Simulator::Stop(Seconds(50.0));
	//AnimationInterface anim("myscripts/project/animation.xml");
	//anim.SetConstantPosition(nodes.Get(0), 10.0, 10.0);
	//anim.SetConstantPosition(nodes.Get(1), 30.0, 30.0);
	Simulator::Run();
	Simulator::Destroy();

	return 0;
}
