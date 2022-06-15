# ICE6403P IoT Design

## Introduction

The main objectives of this project are

- Design a wireless network simulator based on NS-3
- Test the effect of different network parameters and configurations on the simulated network on the basis of the simulator

## Simulator Specs

The simulator must collect the following data so that they could be analyzed and optimized

- Overall transmission speed and individual transmission rate for each drone
- Maximum transmission range for each drone
- Packet loss rate for each drone
- Energy consumed for each drone
- Average delay for packet to arrive at their destination
- Number of online node(drones) at any time

## Drone Limits

Before going any further, we decided to research commercially available drones to ensure that our simulations were realistic. The subject of our research was the DJI Matrice M30, a production quadcopter drone for industrial users. The drone weighs 3.7kg with batteries installed

### Data Rate

The DJI M30 carries a 1920x1080@30Hz flight camera, as well as optional workloads. The drone has no limits on workloads, so they can be HD cameras, IR cameras or even radar. It is almost impossible to stream this huge data in live. A more reasonable requirement is to return the video stream from the flight camera in real time. As a rule of thumb, such a video stream requires about 1Mbps of bandwidth.

### Transmission Limit

The DJI Warp M30 drone is equipped with a Wi-Fi 6 module. The radiation power of this module is:

| Band   | Power |
| ------ | ----- |
| 2.4GHz | 26dBm |
| 5.1GHz | 26dBm |
| 5.8GHz | 26dBm |

The handbook indicates that the M30 drop can be operated within an range of 8km when there is no inteference and no obstacles.

> [matrice-30/specs](https://www.dji.com/cn/matrice-30/specs)

### Battery Limit

The DJS Matrice M30 is equipped with an 131.6 Wh Li-ion battery. It has a no-load power consumption of 8W, a large part of which is occupied by the wireless communication module.

> [matrice-30/specs](https://www.dji.com/cn/matrice-30/specs)

## Simulator Design

We build the simulator with NS-3 based on the examples provided in the course.

