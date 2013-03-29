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
my $sql = "select fevent,levent,nevents from amsdes.datafiles where run = ?";
my $sth = $dbh->prepare($sql);

my @journals = `ls $MERGEDIR/jou/*.jou`;
foreach my $jou (@journals) {
    chomp $jou;
    print "Examining $jou ...\n";
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
    if ($fe <= $ofe and $le >= $ole and $ne > $one and $type == 5 or $ne > $one and $type == 6) {
        if ($ne <= $one) {
            next;
        }
        if ($tag % 256 == 204) { # LAS run, ignore ...
            next;
        }

        print "$run: Type $type\n";
        printf "             %10s%10s%10s\n", "FEVENT", "LEVENT", "NEVENTS";
        printf "Before Merge:%10d%10d%10d\n", $ofe, $ole, $one;
        printf "After Merge: %10d%10d%10d\n\n", $fe, $le, $ne;
        print "Copying $run to $TRCDIR ...";
        print "mv $MERGEDIR/$run $ARCHDIR && mv $jou $ARCHDIR/$run.jou.merged\n\n";
        my $ret = system("cp $MERGEDIR/$run $TRCDIR/ && cp $jou $TRCDIR/jou/ && mv $MERGEDIR/$run $ARCHDIR && mv $jou $ARCHDIR/$run.jou.merged");
        print "done, cp returned $ret.\n\n"
    }
    else {
        system("mv $MERGEDIR/$run $ARCHDIR || mv $jou $ARCHDIR");
    }
}

