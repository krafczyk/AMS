#!/usr/bin/perl -w
#  $Id: validate.cgi,v 1.1 2002/02/20 17:59:57 choutko Exp $
use Gtk;
use strict;


use lib::RemoteClient;


my $debug="-D";
#unshift @ARGV, $debug;




my $nocgi=1;
my $html=new RemoteClient($nocgi);


$html->Validate();






