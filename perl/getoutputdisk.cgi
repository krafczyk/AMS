#!/usr/bin/perl -w
#  $Id: getoutputdisk.cgi,v 1.3 2005/02/16 14:29:43 alexei Exp $
use Gtk;
use strict;


use lib::RemoteClient;


my $debug="-d";
#unshift @ARGV, $debug;

unshift @ARGV, "-DOracle:";
unshift @ARGV, "-Famsdb";
unshift @ARGV, "-N/cgi-bin/mon/checkdb.cgi";


#activate CGI

my $html=new RemoteClient();


#get number of used hosts for each site

 my $ok=$html->ConnectOnlyDB();
 my ($disk, $gb) = $html->getOutputPath('AMS02/2005A');
 print "Select disk : $disk, $gb GB available \n";

 
#work
# 
#if (not $ok){
#  $html->Warning();
#}
#


