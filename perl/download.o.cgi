#!/usr/bin/perl -w
#  $Id: download.o.cgi,v 1.2 2003/11/18 10:34:12 alexei Exp $
use Gtk;
use strict;


use lib::RemoteClient;


my $debug="-d";
unshift @ARGV, "-DOracle:";
unshift @ARGV, "-Famsdb";
unshift @ARGV, "-N/cgi-bin/mon/download.o.cgi";




my $nocgi=1;
my $html=new RemoteClient($nocgi);


$html->DownloadSA();






