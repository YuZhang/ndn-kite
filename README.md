Kite simulation on ndnSIM
================

This simulates ChronoSync Mod + Kite on ndnSIM.  

To enable Interest trace forwarding, two new fields in Interest packet are added. 
We need to install:

`https://github.com/YuZhang/ndnSIM`

After installing the above version of ndnSIM, we need 

first configure: (CAUTION: `--debug` cannot be omitted due to an issue)

`PKG_CONFIG_PATH=/usr/local/lib/pkgconfig ./waf configure --debug`

and then edit and run with visualization:

`./run.sh --vis`

or reproduce the results in Kite paper:

`./sim.pl`

The results and plotting scripts are in `/results` folder.

