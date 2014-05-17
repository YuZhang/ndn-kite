#NS_LOG=SyncApp:SyncLogic:SyncRP:ndn.fw:ndn.fw.PitForwarding ./waf --run "ndn-sync --SyncApp::SyncPrefix=/ndn/sync \
#NS_LOG=SyncApp:SyncLogic:SyncRP ./waf --run "large-topo --SyncApp::SyncPrefix=/ndn/sync \
#NS_LOG=SyncApp:SyncLogic:SyncRP ./waf --run "broadcast --SyncApp::SyncPrefix=/ndn/sync \
#NS_LOG=HelloApp:SyncLogic ./waf --run "broadcast --SyncApp::SyncPrefix=/ndn/sync \
#NS_LOG=SyncApp:SyncLogic:ndn.fw:ndn.fw.PitForwarding ./waf --run "tree-mobile --SyncApp::SyncPrefix=/ndn/sync \
#NS_LOG=SyncApp:SyncLogic:SyncRP:ndn.fw:ndn.fw.PitForwarding ./waf --run "tree --SyncApp::SyncPrefix=/ndn/sync \
NS_LOG=SyncApp:SyncLogic:ndn.fw:ndn.fw.PitForwarding ./waf --run "tree-mobile --isKite=true \ 
                                      --SyncApp::SyncPrefix=/ndn/sync \
                                      --SyncApp::UpdatePeriod=10 \
                                      --SyncLogic::SyncPrefix=/ndn/sync \
                                      --SyncLogic::LossRate=0 \
                                      --SyncLogic::MaxLogLength=1000 \
                                      --SyncLogic::JoinPeriod=10 " $@

#NS_LOG=SyncApp:SyncLogic:SyncRP ./waf --run "tree --SyncLogic::JoinPeriod=10" $@ 
#NS_LOG=HelloApp:HelloLogic:HelloState ./waf --run "broadcast" $@ 
#NS_LOG=HelloApp:HelloLogic:HelloState ./waf --run "tree --HelloLogic::JoinPeriod=10" $@ 
