#  $Id: ActiveClient.pm,v 1.5 2002/01/08 13:43:51 choutko Exp $
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
      StatusType=>"Permanent",
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

