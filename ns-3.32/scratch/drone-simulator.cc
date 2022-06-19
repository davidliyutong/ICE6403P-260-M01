#include "ns3/core-module.h"
#include "ns3/network-module.h"
#include "ns3/mobility-module.h"
#include "ns3/config-store-module.h"
#include "ns3/wifi-module.h"
#include "ns3/internet-module.h"
#include "ns3/aodv-module.h"
#include "ns3/olsr-module.h"
#include "ns3/dsdv-module.h"
#include "ns3/dsr-module.h"

#include "ns3/energy-module.h"

#include "ns3/netanim-module.h"
#include "ns3/seq-ts-header.h"
#include "ns3/v4ping-helper.h"

#include <iostream>
#include <fstream>
#include <vector>
#include <string>
#include <cstdlib>

using namespace ns3;

NS_LOG_COMPONENT_DEFINE("WifiSimpleAdhoc");

/***************************** Vital parameters ****************************/
#define SIMULATION_TIME 1000  // Number of seconds for the simulation
#define NUMNODES 20           // Number of nodes in the network
#define X_SIZE 7500.0         // Size of X grid in meters
#define Y_SIZE 7500.0         // Size of Y grid in meters
#define DRONE_SPEED 1.0       // Speed of dron in meters
#define WIFI_TX_POWER 0.380   // TX power
#define WIFI_RX_POWER 0.113   // RX power
#define WIFI_RANGE 4000       // Range of Wi-Fi in meters
#define CONFIG_INSTALL_APP 1  // Enable app or not
char routing[] = "DSDV";      // AODV, DSDV, OLSR

#define WIFI_STANDARD WIFI_STANDARD_80211ax_5GHZ
const char* phyMode = "OfdmRate9Mbps";
/***************************************************************************/


/***************************** Applications ********************************/

#define NUMPINGS 2      // Number of apps in the network
struct Apps_data { uint32_t id; uint32_t source; uint32_t dest; Time startTime; Time endTime; };

Apps_data pings[NUMPINGS] =
{
  {0, 0, 16, Seconds(1), Seconds(1000)},
  {1, 8, 10, Seconds(1), Seconds(1000)},
  // {2, 0, 9, Seconds(35), Seconds(50)}
//  3, 2, 3, Seconds(30), Seconds(50),
//  4, 9, 0, Seconds(40), Seconds(50),
};

void Startstreaming(NodeContainer c, Ipv4InterfaceContainer i, int id) {
  V4PingHelper ping(i.GetAddress(pings[id].dest));
  ping.SetAttribute("Verbose", BooleanValue(true));
  ping.SetAttribute("Size", UintegerValue(rand()%2048));
  ping.SetAttribute("Interval", TimeValue(MilliSeconds(100)));
  ApplicationContainer p = ping.Install(c.Get(pings[id].source));
  p.Start(pings[id].startTime);
  p.Stop(pings[id].endTime);
}
/***************************************************************************/


void Endsimulation() {
  Simulator::Stop();
}


int main(int argc, char* argv[]) {

  NodeContainer c;
  c.Create(NUMNODES);

  /******* Wifi PHYsical layer *******/
  WifiHelper wifi;
  wifi.SetStandard(WIFI_STANDARD);

  // wifi.SetRemoteStationManager("ns3::ConstantRateWifiManager", "DataMode", StringValue("OfdmRate9Mbps"), "RtsCtsThreshold", UintegerValue(100));
  // wifi.SetRemoteStationManager("ns3::ConstantRateWifiManager", "DataMode", StringValue("OfdmRate6Mbps"));
  wifi.SetRemoteStationManager("ns3::ConstantRateWifiManager", "DataMode",
                               StringValue(phyMode), "ControlMode", StringValue(phyMode));

  YansWifiPhyHelper wifiPhyHelper = YansWifiPhyHelper::Default();
  YansWifiChannelHelper wifiChannel = YansWifiChannelHelper::Default();
  wifiChannel.AddPropagationLoss("ns3::RangePropagationLossModel", "MaxRange", DoubleValue(WIFI_RANGE));   // wifi range meters
  // wifiChannel.AddPropagationLoss("ns3::FixedRssLossModel", "Rss", DoubleValue(-81));
  // wifiChannel.AddPropagationLoss("ns3::FriisPropagationLossModel");
  // wifiChannel.AddPropagationLoss("ns3::LogDistancePropagationLossModel");

  wifiChannel.SetPropagationDelay("ns3::ConstantSpeedPropagationDelayModel");

  // wifiPhy.Set("TxPowerStart", DoubleValue(25.0));
  // wifiPhy.Set("TxPowerEnd", DoubleValue(26.0));
  // wifiPhy.Set("TxPowerLevels", UintegerValue(10));
  // wifiPhy.Set("TxGain", DoubleValue(0));
  // wifiPhy.Set("RxGain", DoubleValue(0));
  // wifiPhy.Set("ChannelWidth", UintegerValue(20));
  // wifiPhy.Set("SignalDetectThreshold", DoubleValue(-100));
  // wifiPhy.Set("CcaModelThreshold", DoubleValue(-100));

  wifiPhyHelper.SetChannel(wifiChannel.Create());

  /******* Wifi MAC layer *******/
  WifiMacHelper wifiMac;

  wifiMac.SetType("ns3::AdhocWifiMac");
  NetDeviceContainer devices = wifi.Install(wifiPhyHelper, wifiMac, c);


  /******* Mobility definition *******/
  MobilityHelper mobility;
  // Ptr<ListPositionAllocator> positionAlloc = CreateObject<ListPositionAllocator>();

  mobility.SetPositionAllocator("ns3::RandomDiscPositionAllocator",
                                "X", StringValue(std::to_string(X_SIZE / 2)),
                                "Y", StringValue(std::to_string(Y_SIZE / 2)),
                                "Z", StringValue("1000"),
                                "Rho", StringValue("ns3::UniformRandomVariable[Min=0|Max=" + std::to_string(std::min(X_SIZE, Y_SIZE) / 3) + "]"));
  // mobility.SetPositionAllocator("ns3::RandomDiscPositionAllocator",
  //                               "X", StringValue("3750"),
  //                               "Y", StringValue("3750"),
  //                               "Rho", StringValue("ns3::UniformRandomVariable[Min=0|Max=1000]"));
  mobility.SetMobilityModel("ns3::RandomWalk2dMobilityModel",
                            "Mode", StringValue("Time"),
                            "Time", StringValue("5s"),
                            "Speed", StringValue("ns3::ConstantRandomVariable[Constant=" + std::to_string(DRONE_SPEED) + "]"),
                            "Bounds", StringValue("0|" + std::to_string(X_SIZE) + "|0|" + std::to_string(Y_SIZE)));
  // mobility.SetMobilityModel("ns3::RandomWalk2dMobilityModel",
  //                           "Mode", StringValue("Time"),
  //                           "Time", StringValue("5s"),
  //                           "Speed", StringValue("ns3::ConstantRandomVariable[Constant=" + std::to_string(DRONE_SPEED) + "]"),
  //                           "Bounds", StringValue("0|"+std::to_string(150)+"|0|"+std::to_string(150)));

  mobility.InstallAll();

  /******* Energy module part *******/
  BasicEnergySourceHelper basicSourceHelper;
  basicSourceHelper.Set("BasicEnergySourceInitialEnergyJ", DoubleValue(10000.0));
  EnergySourceContainer sources = basicSourceHelper.Install(c);
  WifiRadioEnergyModelHelper radioEnergyHelper;
  radioEnergyHelper.Set("TxCurrentA", DoubleValue(WIFI_TX_POWER));
  radioEnergyHelper.Set("RxCurrentA", DoubleValue(WIFI_RX_POWER));
  DeviceEnergyModelContainer deviceModels = radioEnergyHelper.Install(devices, sources);

  /******* Choose your routing *******/
  if (strcmp(routing, "AODV") == 0) {
    printf("Routing: AODV\n");
    AodvHelper aodv;
    InternetStackHelper internet;
    internet.SetRoutingHelper(aodv); // has effect on the next Install ()
    internet.Install(c);
  } else if (strcmp(routing, "DSDV") == 0) {
    printf("Routing: DSDV\n");
    DsdvHelper dsdv;
    dsdv.Set("PeriodicUpdateInterval", TimeValue(Seconds(1)));
    InternetStackHelper internet;
    internet.SetRoutingHelper(dsdv); // has effect on the next Install ()
    internet.Install(c);
  } else  //Default
  {
    printf("Routing: OLSR\n");
    OlsrHelper olsr;
    olsr.Set("HelloInterval", TimeValue(Seconds(1)));
    olsr.Set("TcInterval", TimeValue(Seconds(1)));
    InternetStackHelper internet;
    internet.SetRoutingHelper(olsr); // has effect on the next Install ()
    internet.Install(c);
  }

  /******* IPv4 Layer configuration *******/
  Ipv4AddressHelper ipv4;
  NS_LOG_INFO("Assign IP Addresses.");
  ipv4.SetBase("10.1.1.0", "255.255.255.0");
  Ipv4InterfaceContainer i = ipv4.Assign(devices);

  // PING application
#if CONFIG_INSTALL_APP
  for (int j = 0; j < NUMPINGS; j++) {
    Startstreaming(c, i, j);
  }
#endif

  // Tracing
  wifiPhyHelper.EnablePcap(routing, devices);

  // Animation turn on
  AnimationInterface anim("animation.xml");

  // Ending simulation at time X
  Simulator::Schedule(Seconds(SIMULATION_TIME), &Endsimulation);

  // Start simulation
  Simulator::Run();

  for (DeviceEnergyModelContainer::Iterator iter = deviceModels.Begin(); iter != deviceModels.End(); ++iter) {
    double energeConsumed = (*iter)->GetTotalEnergyConsumption();
    std::cout << "Energy consumed on node " << iter - deviceModels.Begin() << ": " << energeConsumed << std::endl;
  }

  Simulator::Destroy();

  return 0;
}
