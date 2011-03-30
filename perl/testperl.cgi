#!/usr/bin/perl -w
#  $Id: testperl.cgi,v 1.2 2011/03/30 12:48:35 dmitrif Exp $
#use Gtk;
# test any perl text in conn with db
use strict;


use lib::RemoteClient;


my $debug="-d";
#unshift @ARGV, $debug;

unshift @ARGV, "-DOracle:";
unshift @ARGV, "-Fpdb_ams";


#activate CGI

my $html=new RemoteClient(1);


#get number of used hosts for each site
my $ronly=1;
 my $ok=$html->ConnectOnlyDB($ronly);
  $html->TestPerl(18921);  
 



