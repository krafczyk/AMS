#!/usr/bin/perl -w
#  $Id: validate.cgi,v 1.2 2002/03/11 16:41:40 alexei Exp $
use Gtk;
use strict;


use lib::RemoteClient;


my $debug="-D";
unshift @ARGV, -N/cgi-bin/mon/validate.cgi;




my $nocgi=1;
my $html=new RemoteClient($nocgi);


$html->Validate();






