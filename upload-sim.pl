#!/usr/bin/perl -w

my $nrun = 50;

sub getresult () {
 my $c = 0;
 open(my $fh, "<", "results/aggregate-trace.txt") or die "Can not open file: $!";
 while(<$fh>) {
  chomp;
  my @f = split /\s+/, $_;
  next unless ($f[1] eq "0" and /SatisfiedInterests/);
  $c += $f[5];
 }
 close $fh;
 return $c;
}

my $ff;

open($ff, ">", "results/speed-packet-1.txt") or die "Can not open file: $!";
foreach my $size ( map {$_*50} (1 .. 10) ) {
  print $ff "$size ";
  my @a = ();
  foreach my $run ( 1 .. $nrun ) {
    my $cmd = "NS_GLOBAL_VALUE=\"RngRun=${run}\" ./build/upload --ns3::ndn::fw::PitForwarding::Pull=true --kite=1 --speed=${size} --size=1 --grid=4 --stop=100"; 
    system ($cmd); 
    push @a, &getresult();
    print ".";
  }
  my @b = sort {$a <=> $b} @a;
  print $ff "@b[0, int($nrun*0.25), int($nrun*0.5)-1, int($nrun*0.75)-1, $nrun-1, int($nrun*0.05)-1, int($nrun*0.95)-1]"; 
  print $ff "\n";
}
close $ff;

open($ff, ">", "results/speed-packet-0.txt") or die "Can not open file: $!";
foreach my $size ( map {$_*50} (1 .. 10) ) {
  print $ff "$size ";
  my @a = ();
  foreach my $run ( 1 .. $nrun ) {
    my $cmd = "NS_GLOBAL_VALUE=\"RngRun=${run}\" ./build/upload --ns3::ndn::fw::PitForwarding::Pull=false --kite=1 --speed=${size} --size=1 --grid=4 --stop=100"; 
    system ($cmd); 
    push @a, &getresult();
    print ".";
  }
  my @b = sort {$a <=> $b} @a;
  print $ff "@b[0, int($nrun*0.25), int($nrun*0.5)-1, int($nrun*0.75)-1, $nrun-1, int($nrun*0.05)-1, int($nrun*0.95)-1]"; 
  print $ff "\n";
}
close $ff;

exit 0;



