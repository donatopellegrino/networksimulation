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
uint32_t nNodes = 2;
uint32_t stopTime = 1000;




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

	// Address conf In virtual topology
	PointToPointHelper pointToPoint;
	pointToPoint.SetDeviceAttribute ("DataRate", StringValue ("5Mbps"));
	pointToPoint.SetChannelAttribute ("Delay", StringValue ("2ms"));

	NetDeviceContainer devices;
	devices = pointToPoint.Install (nodes);


	// Internet stack install
	InternetStackHelper stack;    // IPv4 is required for GlobalRouteMan
	Ipv4DceRoutingHelper ipv4RoutingHelper;	//required for quagga
	stack.SetRoutingHelper (ipv4RoutingHelper);
	stack.Install (nodes);

	Ipv4AddressHelper ipv4AddrHelper;
	ipv4AddrHelper.SetBase ("10.0.0.0", "255.255.255.0");
	Ipv4InterfaceContainer interfaces = ipv4AddrHelper.Assign (devices);
	Ipv4GlobalRoutingHelper::PopulateRoutingTables ();

	DceManagerHelper processManager;
	processManager.SetNetworkStack ("ns3::Ns3SocketFdFactory");  // required for pcap
	processManager.Install (nodes);
	
	QuaggaHelper quagga;
	quagga.EnableBgp (nodes);
	quagga.BgpAddNeighbor (nodes.Get (0), "10.0.0.2", quagga.GetAsn (nodes.Get (1)));
	quagga.BgpAddNeighbor (nodes.Get (1), "10.0.0.1", quagga.GetAsn (nodes.Get (0)));
	quagga.EnableZebraDebug (nodes);
	quagga.Install (nodes);
	


	pointToPoint.EnablePcapAll ("project-bgpd");

	//
	// Now It's ready to GO!
	//
	Simulator::Stop (Seconds (stopTime));
	AnimationInterface anim("myscripts/project/animation.xml");
	anim.SetConstantPosition(nodes.Get(0), 10.0, 10.0);
	anim.SetConstantPosition(nodes.Get(1), 30.0, 30.0);
	Simulator::Run ();
	Simulator::Destroy ();

	return 0;
}
