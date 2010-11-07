#!/usr/bin/perl -w
#  $Id: testperl.cgi,v 1.1 2010/11/07 18:30:01 choutko Exp $
#use Gtk;
# test any perl text in conn with db
use strict;


use lib::RemoteClient;


my $debug="-d";
#unshift @ARGV, $debug;

unshift @ARGV, "-DOracle:";
unshift @ARGV, "-Famsdb";


#activate CGI

my $html=new RemoteClient(1);


#get number of used hosts for each site
my $ronly=1;
 my $ok=$html->ConnectOnlyDB($ronly);
  $html->TestPerl(18921);  
 



