#!/usr/bin/perl -w
# To check the symbol links and repair if:
# 1. not exist, re-linking,
# 2. not correct, re-linking, and
# 3. target failed, re-upload-to-disks-data-files.

use strict;
use Carp;
use lib qw(../perl);
use lib::DBSQLServer;
use DBI;

my $dataset='/Data/AMS02/2011A/RawData';
while (scalar @ARGV) {
    if ($ARGV[0] =~ /^-d/) {
        $ARGV[0] =~ s/^-d//g;
        if ($ARGV[0] eq "") {
            shift;
        }
        $dataset = $ARGV[0];
    }
    shift;
}

my $pwd = `cat /var/www/cgi-bin/mon/lib/.oracle.oracle`;
chomp $pwd;
delete $ENV{NLS_LANG};
my $dbh = DBI->connect('dbi:Oracle:pdb_ams', 'amsdes', $pwd) or die $DBI::errstr;
my $sql = "select paths, path, run from amsdes.datafiles where path like '%$dataset%' and type like 'SCI%' and status like 'OK'";
my $sth = $dbh->prepare($sql);
my $ret = $sth->execute;
if ($ret != 0) {
    print "Cannot execute SQL (ret code $ret):\n$sql\n";
    exit 1;
}
my $files = $sth->fetchall_arrayref({});

foreach my $line (@$files) {
    if (not -l $line->{PATHS}) {    # symbol link itself failed
        print "Re-linking $line->{PATHS} --> $line->{PATH}\n";
        if ($line->{PATH} ne "") {
            if (not symlink($line->{PATH}, $line->{PATHS})) {
                print "Failed to make symbol link for $line->{PATHS} --> $line->{PATH}.\n";
                next;
            }
        }
    }
    elsif (readlink($line->{PATHS}) ne $line->{PATH}) {
        print "Correcting $line->{PATHS} --> $line->{PATH}.\n";
    }
    $ret = system("ls -L $line->{PATHS} >/dev/null 2>&1");
    if ($ret != 0) {
        print "Recopying $line->{PATHS}\n";
        system("/afs/cern.ch/ams/Offline/vdev/perl/removefilesdf.cgi -d$dataset -u -v -notverify -force -r$line->{RUN}");
        system("/afs/cern.ch/ams/Offline/vdev/perl/upload2disksdatafiles.cgi -d$dataset -u -v -r$line->{RUN}");
    }
}
