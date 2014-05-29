Kite simulation on ndnSIM
================
The simulations in this section are set up in a field of 400 X 400 m^{2} with an infrastructural network composed of 16 fixed routers, which are allocated in, and connected via point-to-point links as, a 4 X 4 grid in 300 X 300 m^{2}. Every router and MN has a Wifi device with default setting.

* Simulation Setup in the Upload scenario

In simulation, the MN walks 150m at a constant speed before changing to a random direction, and sends the traced Interests to the CN every 2s. The application in the CN uses a credit-based flow control, and the retransmission timer is 2s. The forwarders do not automatically resend unsatisfied Interests. We simulated the MMS scheme by disabling the pull mechanism of Kite as if the traced Interest from the MN is only to notify the CN of the current location of MN. Each Data is composed of 1024 bytes. For the same speed, the simulation runs 100 times with different seeds for 100s.

* Simulation Setup in the Share scenario

In simulations, MNs are scattered randomly and walk $150m$ at a random speed up to 300 m/s before changing to a random direction. The new updates on the dataset, such as new messages in a chatroom, are generated in a random interval up to 4s, and the outstanding Interests are sent periodically every 2s. The forwarders do not automatically resend unsatisfied Interests. The rendezvous anchor is a router at the left top corner of grid. The Join interests in the Share protocol are sent every 2s. All Interests have a constant lifetime of 2s, and all constant timers are inserted with random jitters. For the same number of MNs, we run the simulations 50 times with different seeds for 20s.

================
This package simulates Upload, and ChronoSync Mod + Kite on ndnSIM.  

To enable Interest trace forwarding, two new optional fields are added into Interest. 

`https://github.com/YuZhang/ndnSIM`

After installing the above version of ndnSIM, we need 

first configure: (CAUTION: `--debug` cannot be omitted due to an unknown issue)

`PKG_CONFIG_PATH=/usr/local/lib/pkgconfig ./waf configure --debug`

and then edit and run with visualization:

`./upload-run.sh --vis`

or

`./sync-run.sh --vis`

or reproduce the results in Kite paper:

`./upload-sim.pl`

or

`./sync-sim.pl`

The results and plotting scripts are in `/results` folder.


