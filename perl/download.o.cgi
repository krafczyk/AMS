#!/usr/bin/perl -w
#  $Id: download.o.cgi,v 1.3 2011/03/30 12:48:34 dmitrif Exp $
use Gtk;
use strict;


use lib::RemoteClient;


my $debug="-d";
unshift @ARGV, "-DOracle:";
unshift @ARGV, "-Fpdb_ams";
unshift @ARGV, "-N/cgi-bin/mon/download.o.cgi";




my $nocgi=1;
my $html=new RemoteClient($nocgi);


$html->DownloadSA();






