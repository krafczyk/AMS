package DirMonitor;
use Getopt::Long;
use File::stat;
#use strict;
use File::Basename;
my %field=(
        warn_level=>32000,
        time_int=>3600,
        src_dirs=>['/afs/cern.ch/ams/local/logs','/afs/cern.ch/ams/local/bsubs','/afs/cern.ch/ams/Offline/AMSDataDirRW/prod.log/scripts','/afs/cern.ch/ams/local/prod.log/Producer','/afs/cern.ch/ams/local/prod.log/MCProducer'],
        dst_dir=>'/fc02dat0/scratch/MC/afsbackup',
        dst_dir2=>'/f2users/scratch/MC/backupafs',
        max_nfiles=>4500,
        file_life=>604800,
				error_counter=>0,
				error_message=>'',
				max_error=>10,
        rm_mode=>0,
        test=>1
);
sub new{
	my $type=shift;
	my $params=shift;
	my $self={%field};
	$self->{warn_level}=$params->{warn_level} if exists $params->{warn_level};
	$self->{time_int}=$params->{time_int} if exists $params->{time_int};
	$self->{src_dirs}=$params->{src_dirs} if exists $params->{src_dirs};
	$self->{dst_dir}=$params->{dst_dir} if exists $params->{dst_dir};
	$self->{dst_dir2}=$params->{dst_dir2} if exists $params->{dst_dir2};
	$self->{max_nfiles}=$params->{max_nfiles} if exists $params->{max_nfiles};
	$self->{file_life}=$params->{file_life} if exists $params->{file_life};
	$self->{error_counter}=$params->{error_counter} if exists $params->{error_counter};
	$self->{error_message}=$params->{error_message} if exists $params->{error_message};
	$self->{max_error}=$params->{max_error} if exists $params->{max_error};
	$self->{rm_mode}=$params->{rm_mode} if exists $params->{rm_mode};
	$self->{test}=$params->{test} if exists $params->{test};
	#check parameters
	my $temp=`date +%F/%T`;
	chomp $temp;
	#check src directory
	print "Checking Src dirs ...\n";
	foreach my $dir (@{$self->{src_dirs}}){
	 	dir_ok($self,$dir);
	}
	#check dst directory
	print "Checking Dst dirs ...\n";
	dir_ok($self,$self->{dst_dir});
	dir_ok($self,$self->{dst_dir2});
	#check warn_level
	if($self->{warn_level}>32000){
        	print "$temp warn level", $self->{warn_level}," should be better smaller than 32000, reset warn_level to 32000\n";
        	$self->{warn_level}=32000;
	}
	if($self->{warn_level}<10000){
        	 print "$temp  Don't you think warn level",$self->{warn_level},"is too small? I will reset it to 20000\n";
         	$self->{warn_level}=10000;
	} 
	#check max_nfiles
#	if($self->{max_nfiles}<10000){
#        	print "$temp  Don't you think max_nfiles ", $self->{max_nfiles},"is too small? I will reset it to 20000\n";
#        	$self->{max_nfiles}=10000;
#	}
	if($self->{max_nfiles}>$self->{warn_level}){
        	print "$temp max_nfiles", $self->{max_nfiles}," should be smaller than warn_level",$self->{warn_level},"\n";
  	       $self->{max_nfiles}=$self->{warn_level}-100;
	}
	#check file life
	if($self->{file_life}<604800){
        	print("$temp file_life", $self->{file_life}," should be better larger than 1 week(604800)\n ");
       		$self->{file_life}=604800;
	}

	#test args input
	print "$temp Start directory monitor, parameters :\n";
	print "src dirs = ";
	foreach my $dir (@{$self->{src_dirs}}){
        	print $dir," ";
	}  
	print "\n";
	print "nprimary dst_dir   = ", $self->{dst_dir},"\n";
	print "secondary dst_dir = ", $self->{dst_dir2},"\n";
	print "max_nfiles= ", $self->{max_nfiles},"\n";
	print "file_life = ", $self->{file_life},"\n";
	print "time_int    = ",$self->{time_int},"\n";
	print "warn_level  = ",$self->{warn_level},"\n";
	if($self->{rm_mode}==1){
        	print "Running on Copy and Remove Mode\n";
	}
	else{
        	print "Running on Copy Mode\n";
	}
	return bless $self,$type;
}
sub run{
	#check dir in src_dirs
	my $self=shift;
	my $cur_time=`date +%s`;
	my $atime;
	chomp $cur_time;
        my $temp=`date +%F/%T`;
	chomp $temp;
  $self->{error_counter}=0;
	$self->{error_message}="";
	print "$temp start to checking .....\n";
	foreach my $dir (@{$self->{src_dirs}}){
	        print "checking $dir ... ";
        	my $ans=dir_ok($self,$dir);
	        my $num=-1;
        	if($ans==1){
                	my @files=`ls -c $dir/* 2>/dev/null`;
	                $num=$#files+1;
        	        #when the number of files in this directory is larger than $max_nfiles, remove old files
                	if($num>$self->{max_nfiles}){
                        foreach my $file (@files){
                                chomp $file;
                                $atime=`ls -l  --time-style=+\%s $file| awk '{print \$6}'`;
                                chomp $atime;
                                if($cur_time-$atime>$self->{file_life}){
                                        my $ret=cp_rm($self,$file,$self->{dst_dir},$self->{rm_mode});
                                        if($ret==0){
                                                #ergent case, try the secondary dst dir
                                                if($num>$self->{warn_level}){
                                                          $self->{error_counter}--;
	                                                        $ret=cp_rm($self,$file,$self->{dst_dir2},$self->{rm_mode});
        	                                                if($ret==0){
                	                                                send_message($self,"$temp Directory: $dir has $num (warn_level $self->{warn_level}) files! Failed Remove Files due to rm fail",1);
                        	                                }
                                     			        				else{
	                                                                $num--;
        	                                                }
	                                               }
	                                        }
	                                        else{
        	                                        $num--;
                	                        }
                        	}
                        }
                        if($num>$self->{warn_level}){
                  	  @files=`ls -c $dir/* 2>/dev/null`;
	                  $num=$#files+1;
				foreach my $file (@files){
                                	chomp $file;
	                                $atime=`ls -l  --time-style=+\%s $file| awk '{print \$6}'`;
        	                        chomp $atime;
                	                if($cur_time-$atime>0.1*$self->{file_life}){
                        	                my $ret=cp_rm($self,$file,$self->{dst_dir},$self->{rm_mode});
                                	        if($ret==0){
                                        	        #ergent case, try the secondary dst dir
                                                	if($num>$self->{warn_level}){
                                                        	  $self->{error_counter}--;
                                                                	$ret=cp_rm($self,$file,$self->{dst_dir2},$self->{rm_mode});
	                                                                if($ret==0){
        	                                                                send_message($self,"$temp Directory: $dir has $num (warn_level $self->{warn_level}) files! Failed Remove Files due to rm fail",1);
                	                                                }
                        	                                        else{
                                 	                                       $num--;
                                        	                        }
                                                	       }
                                                }
                                                else{
                                                        $num--;
                                                }
                              		}
                        	}
				if($num>$self->{warn_level}){
					$self->{error_counter}++;
		                        $self->{error_message}=$self->{error_message}."$temp Directory: $dir has $num (warn_level $self->{warn_level}) files! Failed Remove Files due to file age|can not rm|dst directory untouchable etc";
                                	send_message($self,"$temp Directory: $dir has $num (warn_level $self->{warn_level}) files! Failed Remove Files due to file age|can not rm|dst directory untouchable etc",1);
				}
                        }
                    }
        }
        else{
               # send_message($self,"$temp Directory: $dir Can't be touched\n",1);
        }
        print " totally $num files ... \n";
}
if($self->{error_counter}>0){
         send_message($self,"Check Directory , $self->{error_counter} errors occurs ...\n$self->{error_message}",2);
}
#sleep $time_int;


}
sub cp_rm {
				my $self=shift(@_);
        if($self->{error_counter}>$self->{max_error}){
                  return 0;
        }
        my $file=shift(@_);
        my $dst_prefix=shift(@_);
        my $rm_mode=shift(@_);
        my $prefix=dirname($file);
        my $dst=$dst_prefix."/".$prefix;
        my $ans=dir_ok($self,"$dst_prefix");
        my $temp=`date +%F/%T`;
				chomp $temp;
        my $ret=0;
        if($ans){
            $ret=1;
                if(! -d $dst){
                        $ans=system("/afs/cern.ch/ams/local/bin/timeout --signal 9 60 mkdir -p $dst");
                        if($ans==0){
               							print "$temp Cmd: mkdir -p $dst OK\n";
               					}
               					else{
               					    $self->{error_counter}++;
                            $self->{error_message}=$self->{error_message}."$temp Cmd: timeout --signal 9 60 mkdir -p $dst Failed: $?\n";
                            return 0;
               					}
                }
                $ans=system("/afs/cern.ch/ams/local/bin/timeout --signal 9 300 cp -udp $file $dst");
                if($ans==0){
                        print "$temp Cmd: timeout --signal 9 300 cp -udp $file $dst OK\n";
                        if($rm_mode==1){
                                $ans=system("/afs/cern.ch/ams/local/bin/timeout --signal 9 300 rm $file");
                	#	$ans=0; 
		               if($ans!=0){
                                        print "$temp Cmd: timeout --signal 9 300 rm $file Failed: $?\n";
                                        $ret=0;
                                        $self->{error_counter}++;
                                        $self->{error_message}=$self->{error_message}."$temp Cmd: timeout --signal 9 300 rm $file Failed: $?\n";
                                }
                                else{
                                        print "$temp Cmd: timeout --signal -9 300 rm $file OK\n";
                                }
                        }
                }
                else{
                        $ret=0;
                        print "$temp Cmd: timeout --signal 9 300 cp -udp $file $dst Failed: $?\n";
                        $self->{error_counter}++;
                        $self->{error_message}=$self->{error_message}."$temp Cmd: timeout --signal 9 300 cp -udp $file $dst Failed: $?\n";
                }
        }
        return $ret;
}
sub dir_ok {
	my $self=shift(@_);
  my $dir=shift(@_);
  my $info="cd $dir Ok......\n ";
  my $OK=0;
  my $ans;
  my $temp=`date +%F/%T`;
  chomp $temp;
	if( -d $dir ){
		$OK=1;
	}
	else{	
                $info="$temp Dir: $dir doesn't exists\n";
        	$self->{error_counter}++;
       		 $self->{error_message}=$self->{error_message}.$info;
                $ans=0;
#                $ans=system("touch $dir/00test");
                if($ans!=0){
                        $info="$temp Cmd: touch $dir/00test Failed: $?\n";
                        $self->{error_counter}++;
                        $self->{error_message}=$self->{error_message}.$info;
                }
        }
        print $info;
        return $OK;
}

sub send_message {
	my $self=shift(@_);
        my $message = shift(@_);
        my $temp=`date +%F/%T`;
        chomp $temp;
        my $type=shift(@_);
        my @admins_mails=('kaiwu@cern.ch');
        my @admins_sms=('0041764879401@mail2sms.cern.ch');
        if($type==2){
                foreach my $fadmin (@admins_mails){
                        system("echo  \"$message\"| mail $fadmin") ==0 or print $temp,"system echo  \"$message\"| mail $fadmin Failed: $? \n";
                }
        }
        else{
                foreach my $fadmin (@admins_mails){
                        system("echo  \"$message\"| mail $fadmin") ==0 or print $temp,"system echo  \"$message\"| mail $fadmin Failed: $? \n";
                }
                foreach my $fadmin (@admins_sms){
                        system("echo  \"$message\"| mail $fadmin") ==0 or print $temp,"system echo  \"$message\"| mail $fadmin Failed: $? \n";
                }
        }
}
