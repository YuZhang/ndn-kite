#NS_LOG=SyncApp:SyncLogic:ndn.fw:ndn.fw.PitForwarding \
#NS_LOG=SyncApp:SyncLogic \
NS_GLOBAL_VALUE="RngRun=3" \
./waf --run "tree-mobile \
--kite=1 --speed=400 --size=5 --grid=4 --stop=20 --SyncApp::UpdatePeriod=4 --join=2" $@

