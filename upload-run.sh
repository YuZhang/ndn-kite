#NS_LOG=MobileApp:ServerApp:ndn.fw:ndn.fw.PitForwarding \
#NS_LOG=MobileApp:ServerApp \
NS_GLOBAL_VALUE="RngRun=4" \
./waf --run "upload \
--ns3::ndn::fw::PitForwarding::Pull=true \
--kite=1 --speed=500 --size=1 --grid=4 --stop=200" $@

