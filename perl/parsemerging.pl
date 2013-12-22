#!/usr/bin/perl

use DBI;
use strict;

if (scalar @ARGV < 2) {
    print "Usage: $0 MERGE_RUNS_DIR TRC_WORKING_DIR\n\n";
    exit 1;
}

my ($MERGEDIR, $TRCDIR) = @ARGV;
my $today = `date +\%F`;
chomp $today;
my $ARCHDIR = "$MERGEDIR/archive/$today";
system("mkdir -p $ARCHDIR/jou");

my $dbh = DBI->connect('dbi:Oracle:pdb_ams', 'amsro', 'eto_amsro') or die $DBI::errstr;
my $sql = "select fevent,levent,nevents from amsdes.datafiles where run = ? and status = 'OK'";
my $sth = $dbh->prepare($sql);

my @journals = `ls $MERGEDIR/jou/*.jou`;
foreach my $jou (@journals) {
    chomp $jou;
    open FH, "<$jou";
    my @buf = <FH>;
    close FH;
    my $run = (grep /Run=/, @buf)[0];
    chomp $run;
    $run =~ s/.*Run=//g;
    my $fe = (grep /FEvent=/, @buf)[0];
    chomp $fe;
    $fe =~ s/FEvent=//g;
    my $le = (grep /LEvent=/, @buf)[0];
    chomp $le;
    $le =~ s/LEvent=//g;
    my $ne = (grep /NEvent=/, @buf)[0];
    chomp $ne;
    $ne =~ s/NEvent=//g;
    my $fet = (grep /TFevent=/, @buf)[0];
    chomp $fet;
    $fet =~ s/TFevent=//g;
    my $let = (grep /TLevent=/, @buf)[0];
    chomp $let;
    $let =~ s/TLevent=//g;
    my $type = (grep /Type0=/, @buf)[0];
    chomp $type;
    $type =~ s/Type0=//g;
    my $tag = (grep /Tag=/, @buf)[0];
    chomp $tag;
    $tag =~ s/Tag=//g;
    $sth->execute($run);
    my ($ofe, $ole, $one);
    $sth->bind_columns(\$ofe, \$ole, \$one);
    $sth->fetch;
    my $runtype;
    if ($type == 5 or $type == 6) {
        if ($type == 5) {
            $runtype = 'SCI';
        }
        else {
            $runtype = 'CAL';
        }
        print "Examining $jou ($runtype)...\n";
        print "$run: Type $type\n";
        printf "             %10s%10s%10s\n", "FEVENT", "LEVENT", "NEVENTS";
        printf "Before Merge:%10d%10d%10d\n", $ofe, $ole, $one;
        printf "After Merge: %10d%10d%10d\n\n", $fe, $le, $ne;
    }
    if (($fe <= $ofe or $ofe == 0) and $le >= $ole and $ne > $one and $type == 5 or $ne > $one and $type == 6) {
        if ($ne <= $one) {
            next;
        }
        if ($tag % 256 == 204) { # LAS run, ignore ...
            next;
        }
        my $percentage = 'new';
        if ($ofe) {
            $percentage = sprintf "%.2f%%", ($ne-$one)/$ne*100;
        }
        my $info = sprintf "(%s, $runtype, %d events, %.0f min)", $percentage, $ne-$one, ($ne-$one)/$ne*($let-$fet)/60;

        print "Found run to MERGE: $run $info\n";
        print "Copying $run to $TRCDIR ...";
        my $ret = system("cp $MERGEDIR/$run $TRCDIR/ && cp $jou $TRCDIR/jou/ && mv $MERGEDIR/$run $ARCHDIR && mv $jou $ARCHDIR/jou/$run.jou.merged");
        print "done, cp returned $ret.\n\n"
    }
    else {
        system("mv $MERGEDIR/$run $ARCHDIR ; mv $jou $ARCHDIR/jou/");
    }
}

my @merged = `ls $ARCHDIR/jou/*.jou.merged | sed -e "s/.*\\///g" -e "s/\\..*//g"`;
foreach my $run (@merged) {
    chomp $run;
    print "Removing old nutples for run $run ... \n\n";
    my $ret = system("cd /Offline/vdev/python &&  ./del.py -dISS.B620/std -r$run");
    print "done, del.py returned $ret.\n\n";
}
