#!/usr/bin/perl -w

use strict;

$SIG{INT}=sub{exit()};
$SIG{QUIT}=sub{exit()};

use lib::Mover;

 my $mover=new Mover();
 my $ok = $mover -> Connect();
 if ($ok) {
    $ok = $mover-> doCopy;
 } 
