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
    $q->start_html( "Connected To Servers Succesfully" );

    print $q->h1( "Connected To Servers Succesfully" );


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
     my $buffer="Producer_ActiveHosts";

     print_table($q,$buffer,$#titles,@titles,@output);




     $#titles=-1;
                        @titles = (
            "ID",
            "HostName",
            "Process ID",
            "Start Time",
            "LastUpdate Time",
            "Run",
            "Ntuple",
            "Status",
                                   );

      $buffer="Producer_ActiveClients";
     @output=Monitor::getactiveclients("Producer");
     print_table($q,$buffer,$#titles,@titles,@output);


      




     $#titles=-1;
                        @titles = (
            "Run",
            "Time",
            "First Event",
            "Last Event",
            "Ntuple",
            "Status ",
                                   );

     $buffer="Producer_Ntuples";

     @output=Monitor::getntuples();
     print_table($q,$buffer,$#titles,@titles,@output);


     $#titles=-1;
                        @titles = (
            "ID",
            "HostName",
            "Process ID",
            "Start Time",
            "LastUpdate Time",
            "Status",
                                   );

     $buffer="Server_ActiveClients";

     @output=Monitor::getactiveclients("Server");
     print_table($q,$buffer,$#titles,@titles,@output);

        @titles = (
            "FileSystem",
            "Total (Mbytes)",
            "Free (Mbytes)",
            "% Free",
#            "OK",
                   );
     @output=Monitor::getdbok();
     $buffer="DiskUsage";
     print_table($q,$buffer,$#titles,@titles,@output);



     $#titles=-1;
                        @titles = (
            "Run",
            "Time",
            "First Event",
            "Last Event",
            "Priority ",
            "History ",
            "Status ",
                                   );

     $buffer="Producer_Runs";
     @output=Monitor::getruns();
     print_table($q,$buffer,$#titles,@titles,@output);


      print $q->end_html;

}

sub print_table{
    my $red="<font color=red>";
    my $green="<font color=green>";
    my $yellow="<font color=yellow>";
    my @color=($green,$yellow,$red);
    my ($q,$name,$mt,@output) = @_;
    print $q->h2($name);
     print "<TABLE BORDER=1>\n<TR>";
     for my $i (0 ... $mt){
         print "<TH>$output[$i]</TH>\n";
     }
     print  "</TR>\n";

     for my $i ($mt+1 ... $#output){
         my @text=@{$output[$i]};
         my $string="<TR>";
         for my $j (0 ...$#text-1){
          $string=$string."<TD>$color[$text[$#text]]$text[$j]</TD>";
         }         
         $string=$string."</TR>\n";
         print $string;
     }
     print $q->end_table;
}



