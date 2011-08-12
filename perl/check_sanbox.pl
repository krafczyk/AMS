#!/usr/bin/perl

###############################################################################
###############################################################################
##
##  Copyright (C) Sistina Software, Inc.  1997-2003  All rights reserved.
##  Copyright (C) 2004-2007 Red Hat, Inc.  All rights reserved.
##  
##  This copyrighted material is made available to anyone wishing to use,
##  modify, copy, or redistribute it subject to the terms and conditions
##  of the GNU General Public License v.2.
##
###############################################################################
###############################################################################

use Getopt::Std;
use Net::Telnet ();

# Get the program name from $0 and strip directory names
$_=$0;
s/.*\///;
my $pname = $_;

$opt_o = 'disable';        # Default fence action

# WARNING!! Do not add code bewteen "#BEGIN_VERSION_GENERATION" and 
# "#END_VERSION_GENERATION"  It is generated by the Makefile

#BEGIN_VERSION_GENERATION
$FENCE_RELEASE_NAME="";
$REDHAT_COPYRIGHT="";
$BUILD_DATE="";
#END_VERSION_GENERATION


sub usage
{
    print "Usage:\n";
    print "\n";
    print "$pname [options]\n";
    print "\n";
    print "Options:\n";
    print "  -a <ip>          IP address or hostname of switch\n";
    print "  -h               usage\n";
    print "  -l <name>        Login name\n";
    print "  -n <num>         Port number to check\n";
    print "  -o <string>      do nothing for a moment";
    print "  -p <string>      Password for login\n";
    print "  -S <path>        Script to run to retrieve login password\n";
    print "  -q               quiet mode\n";
    print "  -V               version\n";

    exit 0;
}

sub fail
{
  ($msg) = @_;
  print $msg."\n" unless defined $opt_q;
  $t->close if defined $t;
  exit 1;
}

sub fail_usage
{
  ($msg)=@_;
  print STDERR $msg."\n" if $msg;
  print STDERR "Please use '-h' for usage.\n";
  exit 1;
}

sub version
{
  print "$pname $FENCE_RELEASE_NAME $BUILD_DATE\n";
  print "$REDHAT_COPYRIGHT\n" if ( $REDHAT_COPYRIGHT );

  exit 0;
}

my $host="doesnotexist";

    my @hosts=split /\./,$host;
     $host=$hosts[0];
$FILE="</etc/cluster/cluster.conf";
#$FILE="<./cluster.conf";
    open FILE or die "unable to open cluster file $FILE\n";
      my $hfound=0;
    my $login=undef;
    my $password=undef;
    my $ipaddr=undef;
    my $port=undef;
    my $fence=" ";
 foreach my $chop  (@ARGV){
    if($chop =~/^-f/){
      $fence=unpack("x2 A*",$chop);
    }
}
    while (my $line = <FILE>){
         if($hfound && $line=~/port=/){
          my @pat=split /port=/,$line;
          my @spat=split /\"/,$pat[1];
          $port=$spat[1];
          $hfound=0;  
         }
         elsif($line=~/$host.hrdl/ or $line=~/$host.grdl/){
             $hfound=1;
         }
         if($line=~/fence_sanbox2/ and $line=~/$fence/){
             my @pat=split /ipaddr=/,$line;
             my @spat=split /\"/,$pat[1];
             $ipaddr=$spat[1];
             @pat=split /login=/,$line;
             @spat=split /\"/,$pat[1];
             $login=$spat[1];
             @pat=split /passwd=/,$line;
             @spat=split /\"/,$pat[1];
             $password=$spat[1];
             
         }
     }
    close(FILE);
    if(not defined $ipaddr or not defined $password or not defined $login ){
        die "not defined ip pas log  $ipaddr $password $login  \n";
    }

#
# Set up and log in
#

$t = new Net::Telnet;

$t->open($ipaddr);

$t->waitfor('/login:/');

$t->print($login);

$t->waitfor('/assword:/');

$t->print($password);

$t->waitfor('/\>/');



#
# Do a portshow on the port and look for the DISABLED string to verify success
#
unlink "/tmp/check_sanbox.txt";
$FILE=">/tmp/check_sanbox.txt";
#    open FILE or die "unable to open message file $FILE\n";

for my $port (0...19){
    my $pport=$port+1;
$t->print("show port $pport");
($text, $match) = $t->waitfor('/\>/');


if (($text =~ /AdminState\ *Online/i) && !($text =~ /OperationalState\ *Online/i))
{
   print " Sanbox $fence Port $port OperationalState not Online \n";
   #print FILE " Sanbox $fence Port $port OperationalState not Online \n";
}

}

close(FILE);
exit 0;

sub get_options_stdin
{
    my $opt;
    my $line = 0;
    while( defined($in = <>) )
    {
        $_ = $in;
        chomp;

	# strip leading and trailing whitespace
        s/^\s*//;
        s/\s*$//;

	# skip comments
        next if /^#/;

        $line+=1;
        $opt=$_;
        next unless $opt;

        ($name,$val)=split /\s*=\s*/, $opt;

        if ( $name eq "" )
        {  
           print STDERR "parse error: illegal name in option $line\n";
           exit 2;
	}
	
        # DO NOTHING -- this field is used by fenced
	elsif ($name eq "agent" ) { } 

        elsif ($name eq "ipaddr" ) 
	{
            $opt_a = $val;
        } 
	elsif ($name eq "login" ) 
	{
            $opt_l = $val;
        } 

        elsif ($name eq "option" )
        {
            $opt_o = $val;
        }
	elsif ($name eq "passwd" ) 
	{
            $opt_p = $val;
        }
	elsif ($name eq "passwd_script" )
	{
		$opt_S = $val;
	}
	elsif ($name eq "port" ) 
	{
            $opt_n = $val;
        } 

        # FIXME should we do more error checking?  
        # Excess name/vals will be eaten for now
	else 
	{
           fail "parse error: unknown option \"$opt\"";
        }
    }
}
