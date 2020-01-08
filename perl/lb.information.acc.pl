#!/usr/bin/perl -w
use strict;
use warnings;
use DBI;
use LB;
use Net::FTP;
use Data::Dumper;
use Time::HiRes qw/usleep/;

use constant DEBUG            => 5;

use constant LB_API_HOSTNAME      => '127.0.0.1';
use constant LB_API_USERNAME      => 'admin';
use constant LB_API_PASSWORD      => 'aaebkgba4a';


use constant FTP_HOSTNAME      => '85.209.2.3:21021';
use constant FTP_USERNAME      => 'uploaduser';
use constant FTP_PASSWORD      => 'gyu_gY678YGh_fgdr55UY_IG0';
use constant FTP_FILENAME      => 'fixednetid_test.txt';

use constant LOCAL_FILENAME    => 'local_accounts.txt';


my $ftp = Net::FTP->new(FTP_HOSTNAME, Debug => 0) or die "Cannot connect to ".FTP_HOSTNAME.": $@";
$ftp->login(FTP_USERNAME, FTP_PASSWORD) or die "Cannot login ", $ftp->message;
$ftp->cwd("/landing") or die "Cannot change working directory ", $ftp->message;

my $lb = new LB(host => LB_API_HOSTNAME, 
                user => LB_API_USERNAME, 
                pass => LB_API_PASSWORD);

$lb->connect() || failed("LB connection failed");
my @accounts = $lb->accounts();
exit(1) if scalar @accounts == 0;

open(FH, '>', LOCAL_FILENAME) or die $!;
foreach my $account (@accounts) {
  #next if $account->{'type'} eq 1;
  my $login = $account->{'login'};
  my $staff = $account->{'full'}->{'staff'};
  if (ref($staff) eq 'ARRAY') {
    foreach my $staff_el(@{$staff}) {
      my $ip = $staff_el->{'ipmask'}->{'ip'};
      my $mask = $staff_el->{'ipmask'}->{'mask'};
      print FH qq|$login;$ip/$mask\n|;
    }
  } else {
    my $ip = $staff->{'ipmask'}->{'ip'} || 'unknown';
    my $mask = $staff->{'ipmask'}->{'mask'}  || 'unknown';
    print FH qq|$login;$ip/$mask\n|;
  }   
}
close(FH);
$ftp->put(LOCAL_FILENAME, FTP_FILENAME);
$lb->disconnect();
$ftp->quit;
exit(0);

