#  $Id: ActiveClient.pm,v 1.6 2002/02/11 11:14:35 choutko Exp $
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
      TimeOut=>60,
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

