#  $Id: CID.pm,v 1.8 2009/02/13 14:11:00 choutko Exp $
package CID;
@CID::EXPORT = qw(new);
sub new{
    my $class=shift;
    my $type=shift;
    my $self={
      HostName=>undef,
      uid=>0,
      pid=>$$,
      ppid=>0,
      threads=>1,
      threads_change=>0,
      Type=>"Monitor",
      Status=>"NOP",
      StatusType=>"Permanent",
      Interface=>"default",
      Mips=>1000,
    };
    if (defined $type){
        $self->{Type}=$type;
    }
    use Sys::Hostname;
    $self->{HostName}=hostname();
    return bless $self,$class;
}


