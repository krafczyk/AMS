package CastorCopy;
#
# Last Edit : November 28, 2002. A.Klimentov
#
my $helpCastorCopy = "
# CastorCopy
# Program copies files from 'local' directory to 'CASTOR'.
# - checks sintaksis
# - checks directory content
# - does copy
# - check directories 
# 
# arguments :
# -from    - source files path
# -to      - target directory path
# -h       - help
# -v       - verbose                             
#
";
my $helpCmp = "
#
# CastorCmp
# Program compare files in local and remote directories
# arguments - local and remote directories paths
# castor.cmp /castor/cern.ch/ams/mydir /f2users/alexei/mydir
#
# November 25, 2002. A.Klimentov
";

use File::Find;
use Time::Local;
use Sys::Hostname;

@CastorCopy::EXPORT= qw(newCopy newCmp doCopy doCmp);

my $nfiles = 0;     # number of files in (input/output) dir 
my @inputFiles;     # list of file names in input dir
my @outputFiles;    # list of file names in output dir
my @inputSizes;     # size in bytes of files from inputFiles
my @outputSizes;    # size in bytes of files from outputFiles

my %fields=(
    verbose=>undef,
    start=>undef,
    hostname=>undef,
    source=>undef,
    target=>undef,
    ok=>undef
            );

sub newCopy {
#
my $input;           # input directory path
my $output;          # output directory path
#
    my $type=shift;
    my $self={
              %fields,
          };
    $self->{start}=time();
    $self->{verbose} = 0;


# check sintaksis
my $inputflag = 0;
my $outputflag = 0;

foreach my $chop  (@ARGV){
    if ($inputflag == 1) {
        $input=$chop;
    }
    if ($outputflag == 1) {
        $output=$chop;
    }
    $inputflag=0;
    $outputflag=0;
    if($chop =~/^-from/){
        $inputflag=1;
    }
    if($chop =~/^-to/){
        $outputflag=1;
    }

    if($chop =~/^-v/){
        $self->{verbose} = 1;
    }

    if($chop =~/^-h/){
        print $helpCastorCopy;
        die "...";
    }
}


if (not defined $input or not defined $output) {
    die "CastorCopy -E- Invalid sintaksis.   castor.cp -from /inputDir/files -to /outputDir/";
 }

# get hostname
  my $hostname = hostname();
  my @host = split '.',$hostname;
  if (defined $host[0]) {
   $self->{hostname}=$host[0];
  } else {
   $self->{hostname}=$hostname;
  } 
 if ($self->{verbose}) {
     print "Hostname : $self->{hostname} \n";
     print "Input  Directory : $input \n";
     print "Output Directory : $output \n";
 }
# check dirpath

if (substr($input, 0, 1) ne '/' or substr($output, 0, 1) ne '/' ) {
    print "castor.cp $input $output\n";
    die "castor.cp -E- Invalid sintaksis.  Specify directory explicitly";
}
    $self->{source} = $input;
    $self->{target} = $output;
#
my $mybless=bless $self,$type;
$CastorCopy::Singleton=$mybless;
return $mybless;

}


sub newCmp {
#
my $input;           # input directory path
my $output;          # output directory path
#
    my $type=shift;
    my $self={
              %fields,
          };
    $self->{start}=time();
    $self->{verbose} = 0;

foreach my $chop  (@ARGV){
    if($chop =~/^-h/){
        print $helpCmp;
        die "...";
    }
}

# check sintaksis
my $inputflag = 0;
my $outputflag = 0;

$input=$ARGV[0];
$output=$ARGV[1];

if (not defined $input or not defined $output) {
    die "CastorCopy -E- Invalid sintaksis.   castor.cp -from /inputDir/files -to /outputDir/";
 }

# get hostname
  my $hostname = hostname();
  my @host = split '.',$hostname;
  if (defined $host[0]) {
   $self->{hostname}=$host[0];
  } else {
   $self->{hostname}=$hostname;
  } 
 if ($self->{verbose}) {
     print "Hostname : $self->{hostname} \n";
     print "Input  Directory : $input \n";
     print "Output Directory : $output \n";
 }
# check dirpath

if (substr($input, 0, 1) ne '/' or substr($output, 0, 1) ne '/' ) {
    print "castor.cp $input $output\n";
    die "castor.cp -E- Invalid sintaksis.  Specify directory explicitly";
}
    $self->{source} = $input;
    $self->{target} = $output;
#
my $mybless=bless $self,$type;
$CastorCopy::Singleton=$mybless;
return $mybless;

}

sub castorDir {
    my $dir     = shift;
    my $mode    = shift;
    my $time    = time();
    my $logfile = "/tmp/castor.dir.$time.log";
    my $cmd     = "touch $logfile";
    $status     = systemCmd($cmd);
    if ($status == 0) {
     $cmd = "nsls -R -l $dir >> $logfile";
     $status     = systemCmd($cmd);
     if ($status == 0) {
       parseLog($mode,$logfile);
     } 
     $cmd = "rm $logfile";
     systemCmd($cmd);
    }      
}

sub parseLog {
#
# parse log files of nsls -R -l /castor/cern.ch/whatever
# substitue ':' by '/'
# assumed sintaksis '/castor/cern.ch/ams/MC:'
#
    my $mode    =  shift;
    my $logfile =  shift;
    my $dir    => undef;
    print "parse $logfile as $mode \n";
    open(FILE,"<".$logfile) or die "parseLog -E- Unable to open file $logfile \n"; 
    my @lines= <FILE>;
    close FILE;
    my $i = 0;
    my $n = 0; # file name index in input(output)List
    while ($i <= $#lines){     
     if($lines[$i]  =~/^\//) {      #directory
      $lines[$i] =~ s/:/\//;
      $lines[$i] =~ s/\n//;
      $dir=$lines[$i];
      print "Directory : $dir \n";
     } else {                  # file
#  0           1   2      3     4       5  6  7     8
# mrw-r--r--   1 casadmva va  26400000 Nov 26 12:20 101108000.1.hbk
         my ($prot,$unk,$owner,$grp,$size,$mon,$day,$time,$file) 
         = split " ",$lines[$i];
         if (defined $file && defined $size) {
          $file = $dir.$file;
          if ($mode eq "input") {
              $inputFiles[$n] = $file;
              $inputSize[$n] = $size;
          }
          if ($mode eq "output") {
              $outputFiles[$n] = $file;
              $outputSize[$n] = $size;
          }
         $n++;
      } 
     }
     $i++;
 }
}

sub inputList {
    $inputFiles[$nfiles] = $File::Find::name;
#    if (-d $inputFiles[$nfiles]) {
#        print "Directory : $inputFiles[$nfiles] \n";
#    } else {
#        print ".........................$inputFiles[$nfiles] \n";
#    }
    $nfiles++;
}

sub outputList {
    $outputFiles[$nfiles] = $File::Find::name;
#    if (-d $outputFiles[$nfiles]) {
#        print "Directory : $outputFiles[$nfiles] \n";
#    } else {
#        print ".........................$outputFiles[$nfiles] \n";
#    }
    $nfiles++;
}

sub doCopy {
    my $status=>undef;
    my $self = shift;
    my $input  = $self->{source};
    my $output = $self->{target};

    find (\&inputList, $input);
    my $i = 0;
    my $dir  = $output;
    my $file =>undef;     
    my $sdir => undef;
    my @subdir;

    my $nbytes  = 0;  # bytes copied
    my $nnfiles = 0;  # files copied
     my $starttime = time();
     my $logfile = "/tmp/castor.cp.$starttime.log";
     my $cmd = "touch $logfile";
     $status = systemCmd($cmd);

    while ($i <= $#inputFiles) {
     if (-d $inputFiles[$i]) {
      $dir = $inputFiles[$i];
      @subdir = split $input,$dir;
      $dir=$output;
      $sdir=$input;
      if ($#subdir > -1) {
          $dir=$dir.$subdir[$#subdir];
          $sdir=$sdir.$subdir[$#subdir];
          $cmd = "rfmkdir $dir >> $logfile";
          if ($self->{verbose}) {
           print "/usr/local/bin/rfmkdir $dir \n";
       }
       systemCmd($cmd);
       $status = 0;
      }
     } else {
      $file = $inputFiles[$i];
      my @cfile = split $sdir,$file;
      $file=$dir.$cfile[1];
      $cmd = "/usr/local/bin/rfcp $inputFiles[$i] $file >> $logfile";
      if ($self->{verbose}) {
       print "rfcp $inputFiles[$i] $file \n";
      }
      $filesize = (stat($inputFiles[$i])) [7];
      $nbytes   = $nbytes + $filesize;
      $nnfiles++;
      $status = systemCmd($cmd);
    }
    if ($status != 0) {
     die "castor.cp -E- Copy failed :  $cmd \n";
     
#     $self->doRemove();
    }
     $i++;
 }
  if ($self->{verbose}) {
   print "castor.cp -I- $#inputFiles files are copied to $output\n";
  }
    $status = 1;
LAST : 
    my $endtime = time();
    my $secs    = $endtime - $starttime;
    my $start   = localtime($starttime);
    my $end     = localtime($endtime);
    print "Copy started : $start \n";
    print "     ended   : $end \n";
    my $mbytes = sprintf("%.1f",$nbytes/100000.);
    print "MBYtes : $mbytes \n";
    print "Files  : $nnfiles \n";
    if ($nnfiles > 0) {
     my $fsize= sprintf("%.1f",$mbytes/$nnfiles);
     print "Aver. file size : $fsize [MB] \n";
    }
    if ($secs > 0) {
        my $io = sprintf("%.1f",$nbytes/$secs/1000.);
        print "I/O rate : $io [KBytes/sec] \n";
    }
    return $status;
}

sub doCmp {
    my $status=>undef;

    my $self = shift;
    my $input  = $self->{source};
    my $output = $self->{target};

#
# 4 possibilities 
#
#  input   output
#  ---------------
#   local    local
#   local    castor
#   castor   local
#   castor   castor
#  ----------------
#
    my $inputdir  = "local";
    my $outputdir = "local";

    if($input  =~/castor/){ $inputdir = "castor";}
    if($output =~/castor/){ $outputdir = "castor";}

    print "***** Input directory $input \n";
    if ($inputdir eq "local") {
     $nfiles = 0;
     find (\&inputList, $input);
    } else {
     castorDir($input,"input");
    }

    print "***** Output directory $output \n";
    if ($outputdir eq "local") {
     $nfiles = 0;
     find (\&outputList, $output);
    } else {
     castorDir($output,"output");
    }

    my $ifile  =>undef;
    my $ofile  =>undef;

    my $i        = 0;
    my $j        = 0;
    my $next     = 0;
    my $notfound = 0;
    my $dir;
    my $inpfileflag = 1;
    my $outfileflag = 1;
      while ($i <= $#inputFiles) {
       $inpfileflag = 1;
       if ($inputdir eq "local") {
        if (-d $inputFiles[$i]) {
          $dir = $inputFiles[$i];
          $inpfileflag = 0;
         }
       }
       if ($inpfileflag) {
           $ifile=$inputFiles[$i];
           $ifile =~ s/$input//;
         if (defined $ifile) {
          $next = 0;
          $j    = 0;
          if ($#outputFiles > 0) {
           while ($j <= $#outputFiles && $next < 1) {
            $outfileflag = 1;
            if (defined $outputFiles[$j]) {
             if ($outputdir eq "local") {
              if (-d $outputFiles[$j]) {
               $outfileflag = 0;
              }
             }
            if ($outfileflag) {
             $ofile = $outputFiles[$j];
             $ofile =~ s/$output//;
             if (defined $ofile) {
              if ($ofile eq $ifile) {
               $next = 1;
             }
            }
         }
        }
        $j++;
       }
       if ($next == 0) {
        print "not found... $inputFiles[$i] \n";
        $notfound++;
       }
      }
     }
    }
   $i++;
   }
    print "$input : $#inputFiles Files \n";
    print "$output : $#outputFiles Files \n";
    print "$notfound files not matched in $output \n";

    $status=1;
    return $status;
}


sub parseArgs {
  
  my $string = shift;
  my $slash = "/";
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
  my $slash = "/";
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

sub systemCmd {
    my $cmd = shift;
    my $status = 0;
    if (defined $cmd) {
     $status     = system($cmd);
     if ($status != 0) {
       die "cannot execute $cmd , ret code $status\n";
     }
    }
  return $status;
}
