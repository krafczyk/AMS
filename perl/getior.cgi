#!/usr/bin/perl -w
#  $Id: getior.cgi,v 1.3 2002/03/22 17:56:53 alexei Exp $
use Gtk;
use strict;
use DBI;


my $dbh;
my $output=>undef;


my $help = "
getior selects IOR string from Oracle DB
for the Server with the latest CREATETIME and 
prints it.
-o filename - write output to filename (optional)
-h          - print help
";

#get all info from the db
foreach my $chop  (@ARGV){
    if($chop =~/^-h/){
        print "$help \n";
        die "...";
    }
    if($chop =~/^-o/){
        $output=unpack("x2 A*",$chop);
    }
}
set_oracle_env();
connect_to_oracle();
my $ior=getior();
my $rc = $dbh->disconnect  || warn $dbh->errstr;

    if (not defined $output) {
     print "$ior\n";
 } else {
     open(FILE,">".$output) or die print "Can't open $output for write";
     print FILE "$ior";
     close FILE;
     print "IOR written to $output\n";
 }
# this function sets up the necessary environement variables
# to be able to connect to Oracle
sub set_oracle_env {
    $ENV{"ORACLE_HOME"}='/afs/cern.ch/project/oracle/@sys/prod';
    $ENV{"TNS_ADMIN"}='/afs/cern.ch/project/oracle/admin';
    $ENV{"LD_LIBRARY_PATH"}=$ENV{"ORACLE_HOME"}."/lib";
    1;
}

sub connect_to_oracle {

    my $user   = "amsdes";
    my $pwd    = "ams";
    my $dbname = "DBI:Oracle:amsdb";

# This sets up the HTML header and table


    $dbh = DBI->connect( $dbname,$user, $pwd, 
                      ) || die print "Can't connect : $DBI::errstr";


}

sub getior {
    my $ior = "- no 'Active' servers found -";
    my $sql="SELECT iors FROM Servers WHERE status='Active' Order by createtime DESC";
    my $sth=$dbh->prepare($sql)  or die "Cannot prepare $sql ".$dbh->errstr();
    $sth->execute or die "Cannot execute $sql ".$dbh->errstr();
    my $ret=$sth->fetchall_arrayref();
    $sth->finish();
    if (defined $ret->[0][0]) {
     $ior = $ret->[0][0]
     }
    return $ior;
}
