#  $Id: ActiveClient.pm,v 1.4 2001/02/02 16:22:48 choutko Exp $
package ActiveClient;
use strict;
@ActiveClient::EXPORT = qw(new);
sub new{
    my $class=shift;
    my $ior=shift;
    my $start=shift;
    my $cid=shift;
    my $self={
      id=>$cid,
      ars=>[],
      LastUpdate=>time(),
      Start=>$start,
      Status=>"Active",
    };
    my $arf={
      IOR=>$ior,
      Interface=>$cid->{Interface},
      Type=>$cid->{Type},
      uid=>$cid->{uid},
  };
    $self->{ars}=[$arf];
    
    return bless $self,$class;
}

