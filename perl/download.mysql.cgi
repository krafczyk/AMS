#!/usr/bin/perl -w
#  $Id: download.mysql.cgi,v 1.1 2002/08/08 17:18:08 alexei Exp $
use Gtk;
use strict;


use lib::RemoteClient;


my $debug="-d";

unshift @ARGV, "-Dmysql";
unshift @ARGV, "-F:AMSMC02:pcamsf0";
unshift @ARGV, "-N/cgi-bin/mon/download.mysql.cgi";




my $nocgi=1;
my $html=new RemoteClient($nocgi);


$html->Download();






