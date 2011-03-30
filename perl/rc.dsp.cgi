#!/usr/bin/perl -w
#  $Id: rc.dsp.cgi,v 1.5 2011/03/30 12:48:34 dmitrif Exp $
use Gtk;
use strict;
use lib::RemoteClient;
if($ENV{MOD_PERL}){
  $#ARGV=-1;
}


my $debug="-d";
#unshift @ARGV, $debug;

unshift @ARGV, "-DOracle:";
unshift @ARGV, "-Fpdb_ams";
unshift @ARGV, "-N/cgi-bin/mon/rc.mon.cgi";


#activate CGI

my $html=new RemoteClient();


#get all info from the db

  my $ok=$html->ConnectDB();
      $ok=$html->listAll('all');

 
#work
 
if (not $ok){
  $html->Warning();
}



