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
}


package NominalHost;
package NominalClient;
package ActiveHost;
package ActiveClient;

