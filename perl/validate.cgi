#!/usr/bin/perl -w
#  $Id: validate.cgi,v 1.3 2002/03/13 08:49:35 choutko Exp $
use Gtk;
use strict;


use lib::RemoteClient;


my $debug="-d";
unshift @ARGV, "-N/cgi-bin/mon/validate.cgi";




my $nocgi=1;
my $html=new RemoteClient($nocgi);


$html->Validate();






