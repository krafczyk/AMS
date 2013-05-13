#! /usr/bin/perl
#
use strict;
use DBI;

my $TH_LOST_EVENTS = 0;
my $TH_LOST_TIME = 60*1;
my $BASEDIR = "/afs/ams.cern.ch/user/RawFileInfo";
my $SUMMARY_DIR = "$BASEDIR/Summary";
my $INCOMPLETE_DIR = "$BASEDIR/Incomplete";

my $today;
my ($srun, $erun);

my $iargs = scalar @ARGV;

if ($iargs == 1) {
    $today = $ARGV[0];
}
elsif ($iargs == 2) {
    ($srun, $erun) = @ARGV;
}
elsif ($iargs == 0) {
    $today = `date +%Y-%m-%d`;
}
else {
    print "
Incorrect argument number.

Usage:
  runchecker.pl yyyy-mm-dd     Checking gaps the day before the specified date.
  runchecker.pl <srun> <erun>  Checking gaps between runs <srun> and <erun>.
  runchecker.pl                Checking gaps of yesterday.
"; 
    exit;
}

my $dbh = DBI->connect('dbi:Oracle:pdb_ams', 'amsro', 'eto_amsro') or die $DBI::errstr;

if (not defined $srun) {
    chomp $today;
    my $etime = `date -u -d "$today" +%s`;
    chomp $etime;
    my $stime = $etime - 3600*24;
    $today = Unix2GMT($stime);
    $today =~ s/ .*//g;
    my $sql = "select max(run) from amsdes.datafiles where run < ? and (type like 'SCI%' or type like 'CAL%')";
    my $sth = $dbh->prepare($sql);
    my $run;
    $sth->bind_columns(\$run);
    $sth->execute($stime);
    $sth->fetch;
    $srun = $run;
    $sth->execute($etime);
    $sth->fetch;
    $erun = $run;
}

sub Unix2GMT {
    my $ut = $_[0];
    my ($sec,$min,$hour,$mday,$mon,$year,$wday,$yday,$isdst) = gmtime($ut);
    $year += 1900;
    $mon += 1;
    my $gmt = sprintf("%4d-%02d-%02d %02d:%02d:%02d", $year, $mon, $mday, $hour, $min, $sec);
    return $gmt;
}

system("rm -f $SUMMARY_DIR/summary.$today");
system("rm -f $INCOMPLETE_DIR/incomplete.$today");
open FH, ">$SUMMARY_DIR/summary.$today";

print FH "Region to be checked: [$srun, $erun).\n\n";
my $sql = "select run,type,fevent,levent,nevents,fetime,letime from amsdes.datafiles where run>=$srun and run <= $erun and status = 'OK' order by run";
my $sth = $dbh->prepare($sql);
$sth->execute;
my $runs = $sth->fetchall_arrayref({});
my ($m_e_beginning, $m_e_inside, $m_t_after);
print FH "LEI : Lost Events Inside the RUN.\n";
print FH "T2NR: Time (from the end of this run) to the next RUN.\n\n";
printf FH "%-10s%12s%12s%5s%8s%9s%8s%16s%7s\n", "RUN", "FE_TIME", "LE_TIME", "TYPE", "FEVENT", "LEVENT", "NEVENTS", "LEI", "T2NR";
for (my $i = 0; $i < $#$runs; $i++) {
    $m_e_beginning = $runs->[$i]->{FEVENT} - 1;
    if ($runs->[$i]->{TYPE} =~ /SCI/ or $runs->[$i]->{TYPE} =~ /CAL/) {
        $m_e_inside = $runs->[$i]->{LEVENT} - $runs->[$i]->{FEVENT} + 1 - $runs->[$i]->{NEVENTS};
    }
    else {
        $m_e_inside = "--";
    }
    my $fe_time = Unix2GMT($runs->[$i]->{FETIME});
    my $le_time = Unix2GMT($runs->[$i]->{LETIME});
    my $j;
    for ($j = $i + 1; $j <= $#$runs; $j++) {
        if  ($runs->[$j]->{TYPE} =~ /SCI/ or $runs->[$j]->{TYPE} =~ /CAL/) {
            last;
        }
    }
    $m_t_after = $runs->[$j]->{RUN} - $runs->[$i]->{LETIME};
    my $minutes = int($m_t_after / 60);
    my $seconds = $m_t_after % 60;
    printf FH "%10s%12d%12d%5.3s", $runs->[$i]->{RUN}, $runs->[$i]->{FETIME}, $runs->[$i]->{LETIME}, $runs->[$i]->{TYPE};
    printf FH "%8d", $runs->[$i]->{FEVENT};
    printf FH "%9d%8d", $runs->[$i]->{LEVENT}, $runs->[$i]->{NEVENTS};
    my $m_e_string;
    if ($m_e_inside ne "--" and $m_e_inside) {
        $m_e_string = sprintf "%8s(%5.2f%%)", $m_e_inside, $m_e_inside/$runs->[$i]->{LEVENT}*100;
    }
    else {
        $m_e_string = sprintf "%16s", $m_e_inside;
    }
    print FH $m_e_string;
    printf FH "%1s%3d:%02d\n", "", $minutes, $seconds;
    printf FH "%22s  %s\n", "($fe_time)", "($le_time)";
    if (($runs->[$i]->{TYPE} =~ /SCI/ or $runs->[$i]->{TYPE} =~ /CAL/) and ($m_e_beginning + $m_e_inside > $TH_LOST_EVENTS or $m_t_after > $TH_LOST_TIME)) {
        if (not -f "$INCOMPLETE_DIR/incomplete.$today") {
            open FH1, ">$INCOMPLETE_DIR/incomplete.$today";
        }
        printf FH1 "%10s%12d%12d%5.3s%8d%9d%8d%s%1s%3d:%02d\n%22s  %s\n", $runs->[$i]->{RUN}, $runs->[$i]->{FETIME}, $runs->[$i]->{LETIME}, $runs->[$i]->{TYPE}, $runs->[$i]->{FEVENT}, $runs->[$i]->{LEVENT}, $runs->[$i]->{NEVENTS}, $m_e_string, "", $minutes, $seconds, "($fe_time)", "($le_time)";       
        printf FH1 "[ %d , %d ] : %d\n\n", ($runs->[$i]->{FETIME} < $runs->[$i]->{RUN}) ? $runs->[$i]->{FETIME} : $runs->[$i]->{RUN}, $runs->[$i]->{LETIME} + $m_t_after, $runs->[$i]->{RUN};
    }
}
close FH;
