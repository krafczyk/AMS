package POAMonitor;
use Error qw(:try);
@POAMonitor::ISA = qw(POA_DPS::Monitor);
@POAMonitor::EXPORT =qw(new);
sub new{
    my $type=shift;
    my $self={
    };
    return bless $self, $type;
}


sub HostStatusChanged{
    my ($class,$cid,$ah)=@_;

}
sub  ClientToKill{
    my ($class,$cid,$ac)=@_;
    
    return 1;
}

sub ping{
}

package NominalHost;
package NominalClient;
package ActiveHost;

