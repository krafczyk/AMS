# Program copies files from 'source' directory to 'target'
# does validation and updates database.
# All files matching to 'source' are copying, only files 
# listed in database are validated.
# arguments :
# -S    - source files path
# -T    - target directory path
# -ior  - path to file with IOR (optional)
# -D    - database handler      (f.e. Oracle:) (optional)
# -F    - database file         (f.e. amsdb)   (optional)
#
#
package Mover;


use CORBA::ORBit idl => [ '../include/server.idl'];
use Error qw(:try);
use Carp;
use strict;
use lib::CID;
use lib::ActiveClient;
use lib::POAMonitor;

use lib::DBServer;
use Time::Local;
use lib::DBSQLServer;

@Mover::EXPORT= qw(new Connect readDB doCopy doRemove doValidate updateDB);

my @srcfiles;
my $nfiles = 0;

my %fields=(
    cid=>undef,
    orb=>undef,
    root_poa=>undef,
    mypoamonitor=>undef,
    myref=>undef,
    myior=>undef,

     arsref=>[],
     arpref=>[],

    ac=>undef,
    start=>undef,
    source=>undef,
    target=>undef,
    sourcedir=>undef,
    targetdir=>undef,
    sourcefile=>undef,
    ok=>undef,
    registered=>0,
    sqlserver=>undef,
    DataMC=>1,
    IOR=>undef,
            );

my $slash = "/";

sub new {
#
my $input;
my $output;
my $idir;
my $ifile;
my $cmd;
my $logfile;
my $status;
my $time;
#
    my $type=shift;
    my $self={
              %fields,
          };
    $self->{start}=time();


# check sintaksis

foreach my $chop  (@ARGV){
    if($chop =~/^-S/){
        $input=unpack("x2 A*",$chop);
    }
    if($chop =~/^-T/){
        $output=unpack("x2 A*",$chop);
    }
}


if (not defined $input or not defined $output) {
    die "move -E- Invalid sintaksis.   move -S/inputDir/files -T/outputDir/";
 }

# check dirpath

if (substr($input, 0, 1) ne '/' or substr($output, 0, 1) ne '/' ) {
    print "move $input $output\n";
    die "move -E- Invalid sintaksis.  Specify directory explicitly";
}
    $self->{source} = $input;
    $self->{target} = $output;

# input directory, file
  ($idir,$ifile) = parseArgs($input); 
  if (not defined $idir or not defined $ifile) {
    die "move -E- cannot parse input.  Specify directory and file ";
  }
  $self->{sourcedir}  = $idir;
  $self->{sourcefile} = $ifile;
# output directory
   $self->{targetdir} = addslash($output);
  
# check disk space 
  my $isize  = getTotalSize($input);
  if ($isize < 1) {
      die "empty directory :  $input";
  }
  my $osize  = getDiskSize($output);
  if ($osize < $isize) {
      print "$cmd\n";
      print "$input  - total $isize KBytes\n";
      print "$output - space left $osize\n";
      die "not enough space on $output";
  }

# Corba part a la new in Monitor.pm
$self->{start}=time();
$self->{cid}=new CID;
$self->{orb} = CORBA::ORB_init("orbit-local-orb");
$self->{root_poa} = $self->{orb}->resolve_initial_references("RootPOA");
$self->{root_poa}->_get_the_POAManager->activate;
$self->{mypoamonitor}=new POAMonitor;
my $id = $self->{root_poa}->activate_object($self->{mypoamonitor});
$self->{myref}=$self->{root_poa}->id_to_reference ($id);
$self->{myior} = $self->{orb}->object_to_string ($self->{myref});
$self->{ac}= new ActiveClient($self->{myior},$self->{start},$self->{cid});
#
#
#
#amsdatadir

my $dir=$ENV{AMSDataDir};
if (defined $dir){
    $self->{AMSDataDir}=$dir;
}
else{
    $self->{AMSDataDir}="/f2dat1/AMS01/AMSDataDir";
    $ENV{AMSDataDir}=$self->{AMSDataDir};
}
#
my $mybless=bless $self,$type;
if(ref($Mover::Singleton)){
    croak "Only Single Mover Allowed\n";
}
$Mover::Singleton=$mybless;
return $mybless;

}




sub doCopy {
    my $self = shift;
    my $input  = $self->{source};
    my $output = $self->{target};

#sqlserver
    $self->{sqlserver}=new DBSQLServer();
    $self->{sqlserver}->Connect();
    $self->readDB($input);
    if ($nfiles < 1) {
     print "doCopy -W- no files matching to path : $input found in DB\n";
    }
    my $ref = $Mover::Singleton;
    $ref->readServer();
    $ref->getValidatedNTuples();
# do copy || file by file if it matches to the DB list
# hostname
#
  my $time = time();
  my $logfile = "/tmp/cp.$time.log";
  my $cmd = "cp -pi -d -v -r $input $output > $logfile";
  my $status = system($cmd);
  if ($status != 0) {
   die "Copy failed :  $cmd"
  }
  if ($self -> doValidate()) {
# sendDSTEnd($action="Create","Delete")
   if ($self -> updateDB()) {
     $self -> commitDB();
     $self -> doRemove();
    } else {
      warn "updateDB failed for $output no commit";
      $self -> ErrorPlus("Copy finished abnormally");

   }
  } else {
      warn "Validation failed for $output";
      $self -> ErrorPlus("Copy finished abnormally");
  }
}

sub readDB {
# check files in database
    my $self = shift;
    my $input = shift;
# check for '*' and replace it by '%'
    $input =~ s/\*/\%/;
    my $sql = "SELECT path FROM ntuples WHERE path LIKE '$input'";
    my $r3=$self->{sqlserver}->Query($sql);
    if(defined $r3->[0][0]){
     foreach my $path (@{$r3}){
         $srcfiles[$nfiles] = $path;
         $nfiles++;
         }
  }
}

sub readServer {
#
    my $ref=shift;
    my $ok=$ref->UpdateARS();
    if(not $ok) {
       return 0;
   }
    my $arsref;
        foreach $arsref (@{$ref->{arpref}}){
            my $length;
            my $ahl;
            try{
                my %cid=%{$ref->{cid}};
                $cid{Type}="Producer";
                ($length,$ahl)=$arsref->getDSTS(\%cid);
                if($length==0){
                    $ref->{dsts}=undef;
                }
                else {
                    $ref->{dsts}=$ahl;
                }

                goto LAST;
            }
        }

LAST :
    return 1;
}

sub getValidatedNTuples {

    my @output=();
    my @text=();
    my @final_text=();
    my @sort=("Validated", "Success");
        foreach my $sort (@sort) {    
        for my $i (0 ... $#{$Mover::Singleton->{dsts}}){
            $#text=-1;
            my $hash=$Mover::Singleton->{dsts}[$i];
            if($hash->{Type} eq "Ntuple"){
                if($hash->{Status} eq $sort){
                    print "$sort : $hash->{Name}\n";
                }
            }
        }
    }
}

sub doValidate {
# validate output RC L645-693
    return 1;
}

sub updateDB {
# update db
    return 1;
}

sub commitDB {
# commit db
}

sub doRemove {
# delete input files
    my $self = shift;
    my $time = time();
    my $input= $self->{source};
  my $logfile = "/tmp/aarm.$time.log";
  my $cmd = "rm -r -f -v $input > $logfile";
#
}

sub parseArgs {
  
  my $string = shift;
  my @chars = split(//,$string);
  my $dir;
  my $file;
  my $i = 0;
  my $s = 0;
  my $j = 0;
  foreach my $char (@chars) {
      if ($char eq $slash)  {
          $s = $i;
      }
      $i++;
  }
 $dir= $chars[$j];
 $j++;
 while ($j<$s+1) {
     $dir= $dir.$chars[$j];
     $j++;
 }
  $file= $chars[$j];
  $j++;
  while ($j<$i) {
     $file= $file.$chars[$j];
     $j++;
 }
  return($dir,$file);
}

sub addslash {
  my $string = shift;
  my @chars = split(//,$string);
  my $i = 0;
  foreach my $char (@chars) {
      $i++;
  }
  if ($chars[$i-1] ne $slash) {
      $string = $string.$slash;
  }
  return $string;
}

sub getTotalSize {
  my $dir = shift;
  my $size = 0;
  my $file;
  my $time = time();
  my $du_file  = "/tmp/aadu".$time;
  my $cmd = "du -k -s -c $dir > $du_file";
  my $status = system($cmd);
  if ($status == 0) {
    open(DUINPUT,"<".$du_file) or die "cannot open $du_file for read";
    while (<DUINPUT>) {
        ($size, $file) = split;
    }
    close (DUINPUT);
 } else {
     die "cannot execute $cmd";
 }
  return $size;
}

sub getDiskSize {
  my $dir = shift;
  my $size = 0;
  my $time = time();
  my $du_file  = "/tmp/aadu".$time;
  my $cmd = "df -k  $dir > $du_file";
  my $status = system($cmd);
  if ($status == 0) {
    open(DUINPUT,"<".$du_file) or die "cannot open $du_file for read";
    while (<DUINPUT>) {
      my ($fs, $tsize, $used, $avail, $perc, $mnt) = split;
      $size = $avail;
    }
    close (DUINPUT);
 } else {
     die "cannot execute $cmd";
 }
  return $size;
}
sub ErrorPlus{
    my $ref=shift;
    die shift;
}


# Corba Relates Subs

sub Connect{
    my $ior;
    my $ref=shift;
    $ref->{ok}=0;
    if(defined $ref->{IOR}){
        return 1;
    }
# check -IOR first and only then getior
    foreach my $chop  (@ARGV){
        if($chop =~/^-ior/){
#            $ior=unpack("x2 A*",$chop);
             my $iorfile = unpack("x4 A*",$chop);
             my $buf;
             open(FILE,"<".$iorfile) or die "Unable to open file [$iorfile] \n";
             read(FILE,$buf,1638400);
             close FILE;
             $ior=$buf;
             print "Connect -I- IOR read from $iorfile \n";
         }
        
    }
    if (not defined $ior) {
     $ior=getior();
    }

    if( not defined $ior){
        return $ref->{ok};
    }
    chomp $ior;
    my $tm={};
    try{
        $tm = $ref->{orb}->string_to_object($ior);
        $ref->{arsref}=[$tm];
        $ref->{ok}=1;
    }
    catch CORBA::MARSHAL with{
        carp "MARSHAL Error "."\n";
    }
    catch CORBA::COMM_FAILURE with{
        carp "Commfailure Error "."\n";
    }
    catch CORBA::SystemException with{
        carp "SystemException Error "."\n";
    };
    return $ref->{ok};
}

sub getior{
    my $pid=$Mover::Singleton->{cid}->{pid};
    my $file ="/tmp/o."."$pid";
    my $fileo ="/tmp/oo."."$pid";
    my $i=system "/usr/local/lsf/bin/bjobs -q linux_server -u all>$file" ;
    if($i){
        unlink $file;
        return getior2();
    }
    open(FILE,"<".$file) or return undef;
    my $ii=0;
    while (<FILE>){
        if (  $_ =~/^\d/){
            if($ii>0 || 1){
                my @args=split ' ';
                $i=system "/usr/local/lsf/bin/bpeek $args[0] >$fileo";
                if($i){
                    next;
                }
                open(FILEO,"<".$fileo) or next;
                my $datamc=0;
                while (<FILEO>){
                    if (/^DumpIOR/){
                        my $tag=substr($_,8,1);
                        if($tag eq $Mover::Singleton->{DataMC}){
                            $datamc=1;
                        }
                    }
                    if ($datamc and /^IOR:/){
                        close (FILEO);
                        unlink $file,$fileo;
                        return $_;
                    }
                }
            }
            $ii++;
        }
    }
    close (FILEO);
    unlink $file,$fileo;
    return getior2();
}
sub getior2{
    my $file ="/tmp/DumpIOR".".$Mover::Singleton->{DataMC}";
    open(FILE,"<".$file) or return undef;
    while (<FILE>){
        if (/^IOR:/){
            close(FILE);
            return $_;
        }
    }
    close(FILE);
    return undef;
}

sub UpdateARS{
 my $ref=shift;
 my $arsref;
 if(not $ref->{ok}){
     return 0;
 }
 foreach $arsref (@{$ref->{arsref}}){
     try{
         my %cid=%{$ref->{cid}};
         $cid{Type}="Server";
      my ($length,$pars)=$arsref->getARS(\%cid,"Any",0,1);
         if($length==0 ){
            carp "updars returns zero \n";
            return 0;
        }
        for(;;){
             my $old=shift @{$ref->{arsref}};
          if(ref($old)){
            undef $old;
          }
          else{
            last;
          }
         }
         my $ior;
         foreach $ior (@$pars){
             try{
              my $newref=$ref->{orb}->string_to_object($ior->{IOR});
              if(rand >0.5){
                  unshift @{$ref->{arsref}}, $newref;
              }
              else{
                  push @{$ref->{arsref}}, $newref;
              }
             }
             catch CORBA::SystemException with{
               carp "getars 2 oops SystemException Error "."\n";
             };

         }   
         $cid{Type}="Producer";
         ($length,$pars)=$arsref->getARS(\%cid,"Any",0,1);

         if($length==0 ){
            carp "updars returns zero \n";
            return 0;
        }
        for(;;){
             my $old=shift @{$ref->{arpref}};
          if(ref($old)){
            undef $old;
          }
          else{
            last;
          }
         }
         foreach $ior (@$pars){
             try{
              my $newref=$ref->{orb}->string_to_object($ior->{IOR});
              if(rand >0.5){
#                  warn "unshift"; 
                  unshift @{$ref->{arpref}}, $newref;
              }
              else{
#                  warn "push"; 
                  push @{$ref->{arpref}}, $newref;
              }
             }
             catch CORBA::SystemException with{
               carp "getars 3 oops SystemException Error "."\n";
             };

         }   

         $cid{Type}="DBServer";
         ($length,$pars)=$arsref->getARS(\%cid,"Any",0,1);

         if($length==0 ){
#            carp "updars returns zero \n";
            return 1;
        }
        for(;;){
             my $old=shift @{$ref->{ardref}};
          if(ref($old)){
            undef $old;
          }
          else{
            last;
          }
         }
         foreach $ior (@$pars){
             try{
              my $newref=$ref->{orb}->string_to_object($ior->{IOR});
              if(rand >0.5){
#                  warn "unshift"; 
                  unshift @{$ref->{ardref}}, $newref;
              }
              else{
#                  warn "push"; 
                  push @{$ref->{ardref}}, $newref;
              }
          }
             catch CORBA::SystemException with{
               carp "getars 3 oops SystemException Error "."\n";
             };

         }   

         last;
     }
    catch CORBA::SystemException with{
     carp "getars oops SystemException Error "."\n";
 };
 }
 return 1;
}

