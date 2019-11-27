#include <fstream>
#include <iostream>
#include "ns3/core-module.h"
#include "ns3/global-route-manager.h"
#include "ns3/network-module.h"
#include "ns3/internet-module.h"
#include "ns3/bridge-module.h"
#include "ns3/csma-module.h"

using namespace ns3;

int main(int argc, char *argv[])
{
  CommandLine cmd;
  cmd.Parse (argc, argv);

  std::ifstream file("/home/ns3dce/dce-linux-dev/source/ns-3-dce/myscripts/project/topology");
  if (!file) {
    std::cerr << "Unable to open file datafile.txt" << "\n";
    exit(1);   // call system to stop
  }

  int ASs_size;
  file >> ASs_size;

  NodeContainer * ASs = new NodeContainer[ASs_size];
  for(int i=0; i<ASs_size; i++){
    int AS_size;
    file >> AS_size;
    ASs[i].Create(AS_size);
    std::cout << "new AS: " << AS_size << "\n";
  }



  delete [] ASs;
  file.close();
}
