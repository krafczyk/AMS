#!/usr/bin/perl -w
use strict;
use Carp;
use lib qw(../perl);
use lib::DBSQLServer;
use DBI;
use File::Basename;

my $eospath;
my $eos = '/afs/cern.ch/project/eos/installation/0.2.33/bin/eos.select';
my $force = 0;
my $run2p = '';
while (scalar @ARGV) {
    if ($ARGV[0] =~ /-f/) {
        $force = 1;
    }
    elsif ($ARGV[0] =~ /-r/) {
        if ($ARGV[0] eq '-r') {
            shift;
            $run2p = $ARGV[0];
        }
        else {
            $run2p = unpack("x2 A*", $ARGV[0]);
        }
        $run2p = " and run in (" . $run2p . ")";
    }
    else {
        $eospath = $ARGV[0];
    }
    shift;
}

if (not defined($eospath) or $eospath eq '') {
    print "Please specify an EOS path in the command line.\n";
    exit 1;
}

my $TMP = "/tmp/reg_eos_time.$$";
system("mkdir -p $TMP");
my $eoslist = "$TMP/eosfiles.list";
system("$eos ls -l $eospath >$eoslist");

open FH, "<$eoslist";
my @files = <FH>;
close FH;

my $pwd = `cat /var/www/cgi-bin/mon/lib/.oracle.oracle`;
chomp $pwd;
my $dbh = DBI->connect('dbi:Oracle:pdb_ams', 'amsdes', $pwd, { RaiseError => 1, AutoCommit => 0 }) or die $DBI::errstr;
my $sql = "update ntuples set eostime=? where path like ?";
my $sth = $dbh->prepare($sql);

my $prefix = $eospath;
$prefix =~ s#/eos/ams##g;

my $eostimecond = '';
if ($force != 1) {
    $eostimecond = ' and eostime <= castortime';
}
my $sql1 = "select path from ntuples where path like '%$prefix%' $eostimecond $run2p";
my $sth1 = $dbh->prepare($sql1);
$sth1->execute or die "Cannot select from ntuples\n";
my $ntuples = $sth1->fetchall_arrayref({});
print scalar @$ntuples . " records to be updated.\n";

my $lasttime = time;
foreach my $ntuple (@$ntuples) {
    my $base = basename($ntuple->{PATH});
    my @lines = grep (/$base/, @files);
    my $nlines = scalar @lines;
    if ($nlines != 1) {
        print "Found $nlines files in EOS of $ntuple->{PATH}, SKIP!!\n";
        next;
    }
    my $line = $lines[0];
    chomp $line;
    my @junk = split / +/, $line;
    my $ts_eos = `date -d "$junk[5] $junk[6] $junk[7]" +%s`;
    chomp $ts_eos;
    my $ret = $sth->execute($ts_eos, '%' . $ntuple->{PATH});
    print "'\%$ntuple->{PATH}' : $ts_eos -- $ret records updated.\n";
    if (time - $lasttime >= 10) {
        $dbh->commit;
        $lasttime = time;
    }
}
$dbh->commit;

system("rm -rf $TMP");
