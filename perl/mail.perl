#!/usr/bin/perl -w
#  $Id: checkfs.cgi,v 1.12 2011/03/30 12:48:35 dmitrif Exp 
use strict;
use Carp;
use lib qw(../perl);
use lib::DBSQLServer;
my $add='amslocal@gmail.com';
my $sub="test";
my $mes=`hostname`;

my $debug="-d";
unshift @ARGV, "-DOracle:";
unshift @ARGV, "-Fpdb_ams";

    open MAIL, "| /usr/lib/sendmail -t -i"
        or die "could not open sendmail: $!";
     print MAIL <<END_OF_MESSAGE2;
To:  $add
Subject: $sub

$mes
END_OF_MESSAGE2
    close MAIL or die "Error closing sendmail: $!";
