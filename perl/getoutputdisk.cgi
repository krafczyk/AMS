#!/usr/bin/perl -w
#  $Id: getoutputdisk.cgi,v 1.4 2011/03/30 12:48:34 dmitrif Exp $
use Gtk;
use strict;


use lib::RemoteClient;


my $debug="-d";
#unshift @ARGV, $debug;

unshift @ARGV, "-DOracle:";
unshift @ARGV, "-Fpdb_ams";
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


