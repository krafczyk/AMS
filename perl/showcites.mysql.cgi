#!/usr/bin/perl -w
#  $Id: showcites.mysql.cgi,v 1.1 2002/08/08 17:18:09 alexei Exp $
use Gtk;
use strict;


use lib::RemoteClient;


my $debug="-d";
#unshift @ARGV, $debug;

unshift @ARGV, "-Dmysql";
unshift @ARGV, "-F:AMSMC02:pcamsf0";
unshift @ARGV, "-N/cgi-bin/mon/rc.mon.cgi";


#activate CGI

my $html=new RemoteClient();


#get all info from the db

 my $ok=$html->ConnectDB();
    $ok=$html->listAll('cites');

 
#work
 
if (not $ok){
  $html->Warning();
}



