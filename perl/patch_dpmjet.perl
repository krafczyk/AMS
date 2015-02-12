#!/usr/local/bin/perl -w       
use strict;
my $initdir=shift @ARGV;
if (not defined $initdir){
    exit();
}
my $dir=shift;

opendir THISDIR ,$initdir or die "unable to open $initdir";
my @allfiles= readdir THISDIR;
closedir THISDIR;

foreach my $file (@allfiles){
    if ( $file =~/\.f$/){
        my $fullfile="$initdir/$file";
        my $f2="$dir/$file";
        open (FILEI,"<$fullfile") or die "Unable to open $fullfile";
        open (FILEO,">$f2") or die "Unable to open $f2 for write";
        my $line="";
        my $cmf=0;
        my $cmname="";
        while ( $line = <FILEI>){
            if($line =~/    SAVE$/){
               
                    next;                
                }
            if($line =~/DATA INIT/){
                my $l2="       INTEGER INIT \n";
                print FILEO $l2;
                $l2='!$OMP THREADPRIVATE'." (INIT) \n";
                print FILEO $l2;
            }
              if($cmf and $line =~/^      / ){
                  $cmf=0;
                  my $l2='!$OMP THREADPRIVATE'." (/$cmname/)\n";
                  print FILEO $l2;
              } 
            if($line=~/   COMMON/ and $line=~/^ /){
                $cmf=1;
                $cmname=(split /\//,$line)[1];    
                if(not defined $cmname or $cmname=~/^XXPY/){
                    print $line;
                    $cmf=0;
                }
            }
            print FILEO $line;
            
          }
        close(FILEI);
        close(FILEO);
    }
}

