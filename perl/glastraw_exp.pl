#!/usr/bin/perl -w

use Tk;
use Tk::Font;
use strict;

my $mw = MainWindow->new;
my  $font = $mw->Font(-family=> 'arial', -size  => 20);
$mw->geometry("400x160");
$mw->title("SOC Console");
my $status;
my $message;
my $time;
my $lastupdate;
my $output;
my $clastupdate="";
my $updateinterval = 600000;
my $timerefreshinterval = 60000;
my $ssherrorcount = 0;
my $footLabel = $mw->Label(-textvariable => \$time, -borderwidth => 2,
           -relief => 'groove')->pack(-fill => 'x',
                                      -side => 'bottom');
my $messageLabel = $mw->Label(-textvariable => \$message, -font => $font, -relief => 'groove', -background => 'yellow')->pack(-fill => 'x', -side => 'top', -expand => 0);
my $statusLabel = $mw->Label(-textvariable => \$status, -font => $font, -relief => 'groove')->pack(-fill => 'both', -expand => 1);

my $defaultcolor = $messageLabel->cget( -background );

refreshStatus();
timerefresh();

MainLoop;

sub timerefresh{
    my $minupdated = int((time() - $lastupdate) / 60);
     my $ms=$minupdated<2?"":"s";
    $time = $minupdated==0?"Last update: $clastupdate":"Last update: $clastupdate;  $minupdated minute$ms ago";
    $mw->after($timerefreshinterval, \&timerefresh);
}

sub refreshStatus{
 $clastupdate=localtime();
    $time = "Last update $clastupdate" ;
    $lastupdate = time();
    while (time() - $lastupdate <= 600) {
        $output = `timeout 30 ssh amslocal\@pcamsf41 "cd /Offline/vdev/perl/; ./rdlastraw.perl -ok @ARGV"`;
        chomp $output;
        if  ($output eq "") {
            sleep 5;
        }
        else {
            last;
        }
    }
    if ($output eq "") {
        my $pingmsg = `ping -c 3 pcamsf41 | grep transmitted | awk '{print \$4}'`;
        chomp($pingmsg);
        if ($pingmsg eq '0') {
            $statusLabel->configure(-background => "red");
            $status = 'Cannot reach pcamsf41';
            $message = 'Call expert: 16 4733';
        }
        else {
            if ($ssherrorcount > 1) {
                $statusLabel->configure(-background => "red");
                $status = 'Cannot ssh pcamsf41';
                $message = 'Call expert: 16 9642';
            }
            else {
                $ssherrorcount++;
            }
        }
    }
    else {
        $ssherrorcount = 0;
        my @outarray = split(/,/, $output);
        $message = $outarray[0];
    
        $outarray[1] =~ s/ OK//g;
        $outarray[1] =~ s/^ //g; 
        $outarray[1] =~ s/ $//g; 
        if ($outarray[1] =~ m/^$/) {
            $statusLabel->configure(-background => "green");
            $status = 'OK';
        }
        else {
            $statusLabel->configure(-background => "red");
            chomp($outarray[1]);
            $outarray[1] =~ s/ /\n/g;
            $outarray[1] =~ s/_/ /g;
            $status = $outarray[1];
        }
    }

    $mw->update;
    $mw->after($updateinterval, \&refreshStatus);
    $mw->after($timerefreshinterval, \&timerefresh);
}
