package CID;
@CID::EXPORT = qw(new);
sub new{
    my $class=shift;
    my $self={
      HostName=>undef,
      uid=>0,
      pid=>$$,
      ppid=>0,
      Type=>"Monitor",
      Status=>"NOP",
      Interface=>"default",
    };
    use Sys::Hostname;
    $self->{HostName}=hostname();
    return bless $self,$class;
}


