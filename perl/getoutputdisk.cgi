#!/usr/bin/perl -w
#  $Id: getoutputdisk.cgi,v 1.2 2005/02/15 16:46:10 alexei Exp $
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
 my ($disk, $gb) = $html->getOutputPath();
 print "Select disk : $disk, $gb GB available \n";

 
#work
# 
#if (not $ok){
#  $html->Warning();
#}
#


