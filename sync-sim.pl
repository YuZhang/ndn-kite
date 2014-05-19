#!/usr/bin/perl -w

my $nrun = 50;

sub getresult () {
 my $c = 0;
 open(my $fh, "<", "results/aggregate-trace.txt") or die "Can not open file: $!";
 while(<$fh>) {
  chomp;
  next if (/ApiFace/);
  next unless (/OutInterest/ or /OutData/);
  my @f = split /\s+/, $_;
  $c += $f[5];
 }
 close $fh;
 return $c;
}

my $ff;

open($ff, ">", "results/size-packet-1-test.txt") or die "Can not open file: $!";
foreach my $size ( map {$_*5} (1 .. 6) ) {
  print $ff "$size ";
  my @a = ();
  foreach my $run ( 1 .. $nrun ) {
   # my $cmd = "NS_GLOBAL_VALUE=\"RngRun=${run}\" ./waf --run \"tree-mobile --kite=1 --speed=500 --size=${size} --grid=4 --stop=20 --SyncApp::UpdatePeriod=20 \" "; 
    my $cmd = "NS_GLOBAL_VALUE=\"RngRun=${run}\" ./build/sync --kite=1 --speed=400 --size=${size} --grid=4 --stop=20 --SyncApp::UpdatePeriod=4 --join=2"; 
    system ($cmd); 
    #print $ff " ", &getresult();
    #print " ", &getresult();
    push @a, &getresult();
    print ".";
  }
  my @b = sort {$a <=> $b} @a;
  print $ff "@b[0, int($nrun*0.25), int($nrun*0.5)-1, int($nrun*0.75)-1, $nrun-1, int($nrun*0.05)-1, int($nrun*0.95)-1]"; 
  print $ff "\n";
}
close $ff;
exit 0;

open($ff, ">", "results/size-packet-0-test.txt") or die "Can not open file: $!";
foreach my $size ( map {$_*5} (1 .. 6) ) {
  print $ff "$size ";
  my @a = ();
  foreach my $run ( 1 .. $nrun ) {
   # my $cmd = "NS_GLOBAL_VALUE=\"RngRun=${run}\" ./waf --run \"tree-mobile --kite=1 --speed=500 --size=${size} --grid=4 --stop=20 --SyncApp::UpdatePeriod=20 \" "; 
    my $cmd = "NS_GLOBAL_VALUE=\"RngRun=${run}\" ./build/sync --kite=0 --speed=400 --size=${size} --grid=4 --stop=20 --SyncApp::UpdatePeriod=4 --join=0"; 
    system ($cmd); 
     #print $ff " ", &getresult();
    #print " ", &getresult();
    push @a, &getresult();
    print ".";
  }
  my @b = sort {$a <=> $b} @a;
  print $ff "@b[0, int($nrun*0.25), int($nrun*0.5)-1, int($nrun*0.75)-1, $nrun-1, int($nrun*0.05)-1, int($nrun*0.95)-1]"; 
  print $ff "\n";
}
close $ff;
exit 0;

