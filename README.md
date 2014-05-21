Kite simulation on ndnSIM
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

