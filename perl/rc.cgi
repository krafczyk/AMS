#!/usr/bin/perl -w
#  $Id: rc.cgi,v 1.1 2002/02/20 17:59:57 choutko Exp $
use Gtk;
use strict;


use lib::RemoteClient;


my $debug="-D";
#unshift @ARGV, $debug;


#activate CGI

my $html=new RemoteClient();


#get all info from the db

 my $ok=$html->Connect();

 
#work
 
if (not $ok){
  $html->Warning();
}



