#  $Id: ActiveClient.pm,v 1.3 2001/01/23 11:50:38 choutko Exp $
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
  };
    $self->{ars}=[$arf];
    
    return bless $self,$class;
}

