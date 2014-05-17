Kite simulation on ndnSIM
================

This simulates ChronoSync Mod + Kite on ndnSIM.  

To enable Interest trace forwarding, two new fields in Interest packet are added. 

`https://github.com/YuZhang/ndnSIM`

After installing the above version of ndnSIM, we need first 

first configure: (CAUSTION: `--debug` cannot be omitted due to a unfixed issue)

`PKG_CONFIG_PATH=/usr/local/lib/pkgconfig ./waf configure --debug`

and then run with visualization:

`./run.sh --vis`

or reproduce the results:

`./reproduce.sh`

