package monitorHTML;
use Error qw(:try);
use CGI qw(-unique_headers);;
use Carp;
use strict;
use lib::Warning;
use lib::Monitor;
@monitorHTML::EXPORT =qw(new);

sub new{
my %fields=(
            q=>undef,
            );
    my $type=shift;
    my $self={
        %fields,
    };


    $self->{q}=new CGI;
    
    my $mybless=bless $self,$type;
    if(ref($monitorHTML::Singleton)){
        croak "Only Single monitorHTML Allowed\n";
    }
    $monitorHTML::Singleton=$mybless;
return $mybless;
}
sub Warning{

    Warning::error($monitorHTML::Singleton->{q},"Unable To Connect The Servers");

}

sub Update{

     my ($monitor, $ok)=Monitor::Update();
     if(not $ok){
         Warning();
         return;
     }
    my $q=$monitorHTML::Singleton->{q};

    print $q->header( "text/html" ),
    $q->start_html( "Connected To Servers" );

    print $q->h1( "Producer" );

    print $q->h2( "Producer_ActiveClients" );

    my @titles;
    $#titles=-1;
            @titles = (
	    "HostName",
	    "Running Pr",
	    "Allowed Pr",
	    "Failed Pr",
	    " Ntuples",
            "EventTags ",
	    " Events ",
	    "  % of Total",
	    " Warnings  ",
	    " Errors  ",
	    " CPU/Event ",
	    " Efficiency",
	    "Status ",
	);

    my @output=Monitor::getactivehosts();
     print "<TABLE BORDER=1>\n<TR>";
     foreach  my $name (@titles){
         print "<TH>$name</TH>\n";
     }
     print  "</TR>\n";

     for my $i (0 ... $#output){
         my @text=@{$output[$i]};
         my $string="<TR>";
         for my $j (0 ...$#text-1){
          $string=$string."<TD>$text[$j]</TD>";
         }         
         $string=$string."</TR>\n";
         print $string;
     }
     print $q->end_table;
      print $q->end_html;


}



