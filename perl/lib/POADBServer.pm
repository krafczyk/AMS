#  $Id: POADBServer.pm,v 1.9 2002/02/20 18:00:25 choutko Exp $
package POADBServer;
use Error qw(:try);
use strict;
use Fcntl ":flock";
use Fcntl;
use MLDBM qw(DB_File Storable);
@POADBServer::ISA = qw(POA_DPS::DBServer);
@POADBServer::EXPORT =qw(new);
            
sub new{
    my $type=shift;
    my $self={
    };
    return bless $self, $type;
}

sub getDBFilePath{
    my ($class,$cid)=@_;
    if (defined $DBServer::Singleton->{dbfile}){
        return $DBServer::Singleton->{dbfile};
    }
    else{
        return "";
    }
}

sub ping{
}

sub getRunEvInfoSPerl{
    my $ref=$DBServer::Singleton;
    my ($class,$cid,$run)=@_;

# need to explicitely open db file in every sub 
    my $ok=0;
    my %hash;
    local *DBM;
    my $db;
    if (defined $ref->{dbfile}){
      $db=tie %hash, "MLDBM",$ref->{dbfile},O_RDWR;
    }
    else{
        goto OUT;
    }
     if(not $db){
        goto OUT;
      }
      my $fd=$db->fd;
      $ok=open DBM, "<&=$fd";
      if( not $ok){
        untie %hash;
        goto OUT;
      }
     my $ntry=0;
     $ok=0;
     until (flock DBM, LOCK_EX|LOCK_NB){
         sleep 2;
         $ntry=$ntry+1;
         if($ntry>10){
             untie %hash;
             goto OUT;
         }
     }
    $ok=1;
OUT:
      undef $db;
          if($ok){
            $ref->{rtb}=$hash{rtb};           
            $ref->{rtb_maxr}=$hash{rtb_maxr};           
            my $length=$#{$ref->{rtb}}+1;    
            untie %hash;
            return ($length,$ref->{rtb},$ref->{rtb_maxr});
          }
         else{
              throw DPS::DBProblem message=>"Unable to Open DB File";
         }
}

sub getRunEvInfo(){

    my $ref=$DBServer::Singleton;
    my ($class,$cid)=@_;
#        my ($ok,%hash)=$ref->OpenDBFile();
# need to explicitely open db file in every sub 
    my $ok=0;
    my %hash;
    local *DBM;
    my $db;
    if (defined $ref->{dbfile}){
      $db=tie %hash, "MLDBM",$ref->{dbfile},O_RDWR;
    }
    else{
        goto OUT;
    }
     if(not $db){
        goto OUT;
      }
      my $fd=$db->fd;
      $ok=open DBM, "<&=$fd";
      if( not $ok){
        untie %hash;
        goto OUT;
      }
     my $ntry=0;
     $ok=0;
     until (flock DBM, LOCK_EX|LOCK_NB){
         sleep 2;
         $ntry=$ntry+1;
         if($ntry>10){
             untie %hash;
             goto OUT;
         }
     }
    $ok=1;
OUT:
      undef $db;


          if($ok){
            $ref->{rtb}=$hash{rtb};           
            $ref->{rtb_maxr}=$hash{rtb_maxr};           
            $ref->{dsti}=$hash{dsti};
            my $host=$cid->{HostName};
            my $dv;
            foreach my $dst (@{$ref->{dsti}}){
                if($dst->{HostName} =~ /^$host/){
                    $dv=$dst;
                    last;
                }
            }
            if (not defined $dv){
                $dv=${$ref->{dsti}}[0];
            }       
           $dv->{DieHard}=0;
           my $i=-1;
           sub prio { $b->{Priority}  <=> $a->{Priority};}
           my @sortedrtb=sort prio @{$ref->{rtb}};
            foreach my $rtb (@sortedrtb){
                $i=$i+1;
             if ($rtb->{Status} eq "ToBeRerun" or $cid->{StatusType} eq "OneRunOnly" ){
                if($rtb->{History} ne "Failed" and ($cid->{StatusType} ne "OneRunOnly" or ($cid->{uid} eq $rtb->{Run} and ($rtb->{Status} eq "Allocated" or $rtb->{Status} eq "Foreign")))){
   if(($rtb->{Status} eq "Allocated" || $cid->{uid} ne 0) and $rtb->{Status} ne "Foreign"){
   $sortedrtb[$i]->{Status}="Processing";
}
else {
 $sortedrtb[$i]->{Status}="Allocated";
}
  if($cid->{uid} ne 0){
   $sortedrtb[$i]->{cuid}=$cid->{uid};
  }
  else{
    $sortedrtb[$i]->{cuid}=$rtb->{Run};
  }
                    $hash{rtb}=\@sortedrtb;
                    untie %hash;
                    return ($rtb,$dv);
                }
            }
            }
            $i=-1;
            foreach my $rtb (@sortedrtb){
                $i=$i+1;
             if ($rtb->{Status} eq "ToBeRerun" or $cid->{StatusType} eq "OneRunOnly"){
                if($rtb->{History} eq "Failed" && $host ne $rtb->{cinfo}->{HostName} and ($cid->{StatusType} ne "OneRunOnly" or ($cid->{uid} eq $rtb->{Run} and ($rtb->{Status} eq "Allocated" or $rtb->{Status} eq "Foreign")))){
   if(($rtb->{Status} eq "Allocated" || $cid->{uid} ne 0) and $rtb->{Status} ne "Foreign"){
   $sortedrtb[$i]->{Status}="Processing";
}
else {
 $sortedrtb[$i]->{Status}="Allocated";
}
  if($cid->{uid} ne 0){
   $sortedrtb[$i]->{cuid}=$cid->{uid};
  }
  else{
    $sortedrtb[$i]->{cuid}=$rtb->{Run};
  }
                    $hash{rtb}=\@sortedrtb;
                    untie %hash;
                    return ($rtb,$dv);
                }
             }
            }
                   $dv->{DieHard}=1;
                    untie %hash;
                    return (${$ref->{rtb}}[0],$dv);
         }
       else{
             throw DPS::DBProblem message=>"Unable to Open DB File";
       } 
   }

sub sendRunEvInfo{
    my $ref=$DBServer::Singleton;
    my ($class,$ri,$rc)=@_;
#        my ($ok,%hash)=$ref->OpenDBFile();
# need to explicitely open db file in every sub 
    my $ok=0;
    my %hash;
    local *DBM;
    my $db;
    if (defined $ref->{dbfile}){
      $db=tie %hash, "MLDBM",$ref->{dbfile},O_RDWR;
    }
    else{
        goto OUT;
    }
     if(not $db){
        goto OUT;
      }
      my $fd=$db->fd;
      $ok=open DBM, "<&=$fd";
      if( not $ok){
        untie %hash;
        goto OUT;
      }
     my $ntry=0;
     $ok=0;
     until (flock DBM, LOCK_EX|LOCK_NB){
         sleep 2;
         $ntry=$ntry+1;
         if($ntry>10){
             untie %hash;
             goto OUT;
         }
     }
    $ok=1;
OUT:
      undef $db;
          if($ok){
              $ref->{rtb}=$hash{rtb};           
              $ref->{rtb_maxr}=$hash{rtb_maxr}; 
             if($rc eq "Update"){
                 for my $i (0 ... $#{$ref->{rtb}} ){
                     my $arel=$ref->{rtb}[$i];
                     if($ri->{uid} ==$arel->{uid}){
                         $ref->{rtb}[$i]=$ri;
                         $hash{rtb}=$ref->{rtb};
                         untie %hash;
                         return;
                     } 
                 }
      
             }
             elsif($rc eq "Delete"){
                 for my $i (0 ... $#{$ref->{rtb}}){
                     my $arel=$ref->{rtb}[$i];
                     if($ri->{uid} ==$arel->{uid}){
                         $ref->{rtb}[$i]=$ref->{rtb}[$#{$ref->{rtb}}];
                         $#{$ref->{rtb}}=$#{$ref->{rtb}}-1;
                         $hash{rtb}=$ref->{rtb};
                         untie %hash;
                         return;
                     } 
                 }
      
             }
             elsif($rc eq "Create"){
                 if($hash{rtb_maxr}<=$ri->{uid}){
                    $hash{rtb_maxr}=$ri->{uid}+1;
                 }
                         $#{$ref->{rtb}}=$#{$ref->{rtb}}+1;
                         $ref->{rtb}[$#{$ref->{rtb}}]=$ri;
                         $hash{rtb}=$ref->{rtb};
                         untie %hash;
                         return;
      
             }
             elsif($rc eq "ClearCreate"){
                    $hash{rtb_maxr}=$ri->{uid}+1;
                    $#{$ref->{rtb}}=0;
                         $ref->{rtb}[$#{$ref->{rtb}}]=$ri;
                         $hash{rtb}=$ref->{rtb};
                         untie %hash;
                         return;
      
             }
                         untie %hash;
              throw DPS::DBProblem message=>"Unable to $rc the rtb $ri->{uid}";
          }
          else{
             throw DPS::DBProblem message=>"Unable to Open DB File";
          } 
}

sub sendDSTEnd{
    my $ref=$DBServer::Singleton;
    my ($class,$cid,$ri,$rc)=@_;
#        my ($ok,%hash)=$ref->OpenDBFile();
# need to explicitely open db file in every sub 
    my $ok=0;
    my %hash;
    local *DBM;
    my $db;
    if (defined $ref->{dbfile}){
      $db=tie %hash, "MLDBM",$ref->{dbfile},O_RDWR;
    }
    else{
        goto OUT;
    }
     if(not $db){
        goto OUT;
      }
      my $fd=$db->fd;
      $ok=open DBM, "<&=$fd";
      if( not $ok){
        untie %hash;
        goto OUT;
      }
     my $ntry=0;
     $ok=0;
     until (flock DBM, LOCK_EX|LOCK_NB){
         sleep 2;
         $ntry=$ntry+1;
         if($ntry>10){
             untie %hash;
             goto OUT;
         }
     }
    $ok=1;
OUT:
      undef $db;
          if($ok){
              $ref->{dsts}=$hash{dsts};           
             if($rc eq "Update"){
                 for my $i (0 ... $#{$ref->{dsts}}){
                     my $arel=$ref->{dsts}[$i];
                     if($ri->{Name} eq $arel->{Name}){
                         $ref->{dsts}[$i]=$ri;
                         $hash{dsts}=$ref->{dsts};
                         untie %hash;
                         return;
                     } 
                 }
      
             }
             elsif($rc eq "Delete"){
                 for my $i (0 ... $#{$ref->{dsts}}){
                     my $arel=$ref->{dsts}[$i];
                     if($ri->{Name} eq $arel->{Name}){
                         $ref->{dsts}[$i]=$ref->{dsts}[$#{$ref->{dsts}}];
                         $#{$ref->{dsts}}=$#{$ref->{dsts}}-1;
                         $hash{dsts}=$ref->{dsts};
                         untie %hash;
                         return;
                     } 
                 }
      
             }
             elsif($rc eq "Create"){
                         $#{$ref->{dsts}}=$#{$ref->{dsts}}+1;
                         $ref->{dsts}[$#{$ref->{dsts}}]=$ri;
                         $hash{dsts}=$ref->{dsts};
                         untie %hash;
                         return;
      
                     }
             elsif($rc eq "ClearCreate"){
                         $#{$ref->{dsts}}=0;
                         $ref->{dsts}[$#{$ref->{dsts}}]=$ri;
                         $hash{dsts}=$ref->{dsts};
                         untie %hash;
                         return;
      
                     }
                         untie %hash;
              throw DPS::DBProblem message=>"Unable to $rc the dsts $ri->{uid}";
          }
          else{
             throw DPS::DBProblem message=>"Unable to Open DB File";
          } 
}
sub sendDSTInfo{
    my $ref=$DBServer::Singleton;
    my ($class,$ri,$rc)=@_;
#        my ($ok,%hash)=$ref->OpenDBFile();
# need to explicitely open db file in every sub 
    my $ok=0;
    my %hash;
    local *DBM;
    my $db;
    if (defined $ref->{dbfile}){
      $db=tie %hash, "MLDBM",$ref->{dbfile},O_RDWR;
    }
    else{
        goto OUT;
    }
     if(not $db){
        goto OUT;
      }
      my $fd=$db->fd;
      $ok=open DBM, "<&=$fd";
      if( not $ok){
        untie %hash;
        goto OUT;
      }
     my $ntry=0;
     $ok=0;
     until (flock DBM, LOCK_EX|LOCK_NB){
         sleep 2;
         $ntry=$ntry+1;
         if($ntry>10){
             untie %hash;
             goto OUT;
         }
     }
    $ok=1;
OUT:
      undef $db;
          if($ok){
              $ref->{dsti}=$hash{dsti};           
             if($rc eq "Update"){
                 for my $i (0 ... $#{$ref->{dsti}}){
                     my $arel=$ref->{dsti}[$i];
                     if($ri->{uid} eq $arel->{uid}){
                         $ref->{dsti}[$i]=$ri;
                         $hash{dsti}=$ref->{dsti};
                         untie %hash;
                         return;
                     } 
                 }
      
             }
             elsif($rc eq "Delete"){
                 for my $i (0 ... $#{$ref->{dsti}}){
                     my $arel=$ref->{dsti}[$i];
                     if($ri->{uid} eq $arel->{uid}){
                         $ref->{dsti}[$i]=$ref->{dsti}[$#{$ref->{dsti}}];
                         $#{$ref->{dsti}}=$#{$ref->{dsti}}-1;
                         $hash{dsti}=$ref->{dsti};
                         untie %hash;
                         return;
                     } 
                 }
      
             }
             elsif($rc eq "Create"){
                         $#{$ref->{dsti}}=$#{$ref->{dsti}}+1;
                         $ref->{dsti}[$#{$ref->{dsti}}]=$ri;
                         $hash{dsti}=$ref->{dsti};
                         untie %hash;
                         return;
      
                     }
             elsif($rc eq "ClearCreate"){
                         $#{$ref->{dsti}}=0;
                         $ref->{dsti}[$#{$ref->{dsti}}]=$ri;
                         $hash{dsti}=$ref->{dsti};
                         untie %hash;
                         return;
      
                     }
                         untie %hash;
              throw DPS::DBProblem message=>"Unable to $rc the dsti $ri->{uid}";
          }
          else{
             throw DPS::DBProblem message=>"Unable to Open DB File";
          } 
}
sub sendAC{
    throw DPS::DBProblem message => 'No such routine';
}
sub sendACPerl{
    my $ref=$DBServer::Singleton;
    my ($class,$cid,$ri,$rc)=@_;
#        my ($ok,%hash)=$ref->OpenDBFile();
# need to explicitely open db file in every sub 
    my $ok=0;
    my %hash;
    local *DBM;
    my $db;
    if (defined $ref->{dbfile}){
      $db=tie %hash, "MLDBM",$ref->{dbfile},O_RDWR;
    }
    else{
        goto OUT;
    }
     if(not $db){
        goto OUT;
      }
      my $fd=$db->fd;
      $ok=open DBM, "<&=$fd";
      if( not $ok){
        untie %hash;
        goto OUT;
      }
     my $ntry=0;
     $ok=0;
     until (flock DBM, LOCK_EX|LOCK_NB){
         sleep 2;
         $ntry=$ntry+1;
         if($ntry>10){
             untie %hash;
             goto OUT;
         }
     }
    $ok=1;
OUT:
      undef $db;
    my $tag;
          if($ok){
              if($cid->{Type} eq "Server"){
               $tag="asl";
              }
              elsif($cid->{Type} eq "Producer"){
               $tag="acl";
              }
              elsif($cid->{Type} eq "DBServer"){
               $tag="adbsl";
              }
              elsif($cid->{Type} eq "Monitor"){
               $tag="aml";
              }
              else{
                  untie %hash;
                  return; 
              }
               $ref->{$tag}=$hash{$tag};           
             if($rc eq "Update"){
                 for my $i (0 ... $#{$ref->{$tag}}){
                     my $arel=$ref->{$tag}[$i];
                     if($cid->{uid} ==$arel->{id}->{uid}){
                         $ref->{$tag}[$i]=$ri;
                         $hash{$tag}=$ref->{$tag};
                         untie %hash;
                         return;
                     } 
                 }
      
             }
             elsif($rc eq "Delete"){
                 for my $i (0 ... $#{$ref->{$tag}}){
                     my $arel=$ref->{$tag}[$i];
                     if($cid->{uid} ==$arel->{id}->{uid}){
                         $ref->{$tag}[$i]=$ref->{$tag}[$#{$ref->{$tag}}];
                         $#{$ref->{$tag}}=$#{$ref->{$tag}}-1;
                         $hash{$tag}=$ref->{$tag};
                         untie %hash;
                         return;
                     } 
                 }
      
             }
             elsif($rc eq "Create"){
                      if( not defined $hash{$tag."_maxc"} or $hash{$tag."_maxc"}<$cid->{uid}){
                         $hash{$tag."_maxc"}=$cid->{uid};
                     }
                         $#{$ref->{$tag}}=$#{$ref->{$tag}}+1;
                         $ref->{$tag}[$#{$ref->{$tag}}]=$ri;
                         $hash{$tag}=$ref->{$tag};
                         my $ar=$hash{$tag};
                         untie %hash;
                         return;
      
                     }
             elsif($rc eq "ClearCreate"){
                         $hash{$tag."_maxc"}=$cid->{uid};
                         $#{$ref->{$tag}}=0;
                         $ref->{$tag}[$#{$ref->{$tag}}]=$ri;
                         $hash{$tag}=$ref->{$tag};
                         my $ar=$hash{$tag};
                         untie %hash;
                         return;
      
                     }
                         untie %hash;
              throw DPS::DBProblem  message=>"Unable to $rc the $tag $cid->{uid}";
          }
          else{
             throw DPS::DBProblem message=>"Unable to Open DB File";
          } 
}

sub sendAH{
    my $ref=$DBServer::Singleton;
    my ($class,$cid,$ri,$rc)=@_;
#        my ($ok,%hash)=$ref->OpenDBFile();
# need to explicitely open db file in every sub 
    my $ok=0;
    my %hash;
    local *DBM;
    my $db;
    if (defined $ref->{dbfile}){
      $db=tie %hash, "MLDBM",$ref->{dbfile},O_RDWR;
    }
    else{
        goto OUT;
    }
     if(not $db){
        goto OUT;
      }
      my $fd=$db->fd;
      $ok=open DBM, "<&=$fd";
      if( not $ok){
        untie %hash;
        goto OUT;
      }
     my $ntry=0;
     $ok=0;
     until (flock DBM, LOCK_EX|LOCK_NB){
         sleep 2;
         $ntry=$ntry+1;
         if($ntry>10){
             untie %hash;
             goto OUT;
         }
     }
    $ok=1;
OUT:
      undef $db;
    my $tag;
          if($ok){
              if($cid->{Type} eq "Server"){
               $tag="ahls";
              }
              elsif($cid->{Type} eq "Producer"){
               $tag="ahlp";
              }
              else{
                  untie %hash;
                  return; 
              }
               $ref->{$tag}=$hash{$tag};           
             if($rc eq "Update"){
                 for my $i (0 ... $#{$ref->{$tag}}){
                     my $arel=$ref->{$tag}[$i];
                     $ri->{HostName}=~/^(.*?)(\.|$)/;
                     my $m1=$1; 
                     $arel->{HostName}=~/^(.*?)(\.|$)/;
                         if($1 eq $m1){
                         $ref->{$tag}[$i]=$ri;
                         $hash{$tag}=$ref->{$tag};
                         untie %hash;
                         return;
                     } 
                 }
      
             }
             elsif($rc eq "Delete"){
                 for my $i (0 ... $#{$ref->{$tag}}){
                     my $arel=$ref->{$tag}[$i];
                     $ri->{HostName}=~/^(.*?)(\.|$)/;
                     my $m1=$1; 
                     $arel->{HostName}=~/^(.*?)(\.|$)/;
                         if($1 eq $m1){
                         $ref->{$tag}[$i]=$ref->{$tag}[$#{$ref->{$tag}}];
                         $#{$ref->{$tag}}=$#{$ref->{$tag}}-1;
                         $hash{$tag}=$ref->{$tag};
                         untie %hash;
                         return;
                     } 
                 }
      
             }
             elsif($rc eq "Create"){
                         $#{$ref->{$tag}}=$#{$ref->{$tag}}+1;
                         $ref->{$tag}[$#{$ref->{$tag}}]=$ri;
                         $hash{$tag}=$ref->{$tag};
                         untie %hash;
                         return;
      
                     }
             elsif($rc eq "ClearCreate"){
                         $#{$ref->{$tag}}=0;
                         $ref->{$tag}[$#{$ref->{$tag}}]=$ri;
                         $hash{$tag}=$ref->{$tag};
                         untie %hash;
                         return;
      
                     }
                         untie %hash;
              throw DPS::DBProblem message=>"Unable to $rc the $tag $ri->{id}->{uid}";
          }
          else{
             throw DPS::DBProblem message=>"Unable to Open DB File";
          } 
}

sub sendGeneric{
    my $ref=$DBServer::Singleton;
    my ($cid,$ri,$rc,$tag,$uid)=@_;
#        my ($ok,%hash)=$ref->OpenDBFile();
# need to explicitely open db file in every sub 
    my $ok=0;
    my %hash;
    local *DBM;
    my $db;
    if (defined $ref->{dbfile}){
      $db=tie %hash, "MLDBM",$ref->{dbfile},O_RDWR;
    }
    else{
        goto OUT;
    }
     if(not $db){
        goto OUT;
      }
      my $fd=$db->fd;
      $ok=open DBM, "<&=$fd";
      if( not $ok){
        untie %hash;
        goto OUT;
      }
     my $ntry=0;
     $ok=0;
     until (flock DBM, LOCK_EX|LOCK_NB){
         sleep 2;
         $ntry=$ntry+1;
         if($ntry>10){
             untie %hash;
             goto OUT;
         }
     }
    $ok=1;
OUT:
      undef $db;
          if($ok){
               $ref->{$tag}=$hash{$tag};           
             if($rc eq "Update"){
                 for my $i (0 ... $#{$ref->{$tag}}){
                     my $arel=$ref->{$tag}[$i];
                     if($ri->{$uid} eq $arel->{$uid}){
                         $ref->{$tag}[$i]=$ri;
                         $hash{$tag}=$ref->{$tag};
                         untie %hash;
                         return;
                     } 
                 }
      
             }
             elsif($rc eq "Delete"){
                 for my $i (0 ... $#{$ref->{$tag}}){
                     my $arel=$ref->{$tag}[$i];
                     if($ri->{$uid} eq $arel->{$uid}){
                         $ref->{$tag}[$i]=$ref->{$tag}[$#{$ref->{$tag}}];
                         $#{$ref->{$tag}}=$#{$ref->{$tag}}-1;
                         $hash{$tag}=$ref->{$tag};
                         untie %hash;
                         return;
                     } 
                 }
      
             }
             elsif($rc eq "Create"){
                         $#{$ref->{$tag}}=$#{$ref->{$tag}}+1;
                         $ref->{$tag}[$#{$ref->{$tag}}]=$ri;
                         $hash{$tag}=$ref->{$tag};
                         untie %hash;
                         return;
      
                     }
             elsif($rc eq "ClearCreate"){
                         $#{$ref->{$tag}}=0;
                         $ref->{$tag}[$#{$ref->{$tag}}]=$ri;
                         $hash{$tag}=$ref->{$tag};
                         untie %hash;
                         return;
      
                     }
                         untie %hash;
              throw DPS::DBProblem message=>"Unable to $rc the $tag $ri->{id}->{uid}";
          }
          else{
             throw DPS::DBProblem message=>"Unable to Open DB File";
          } 
}
sub getGeneric{
    my $ref=$DBServer::Singleton;
    my ($cid,@tag)=@_;
#        my ($ok,%hash)=$ref->OpenDBFile();
# need to explicitely open db file in every sub 
    my $ok=0;
    my %hash;
    local *DBM;
    my $db;
    if (defined $ref->{dbfile}){
      $db=tie %hash, "MLDBM",$ref->{dbfile},O_RDWR;
    }
    else{
        goto OUT;
    }
     if(not $db){
        goto OUT;
      }
      my $fd=$db->fd;
      $ok=open DBM, "<&=$fd";
      if( not $ok){
        untie %hash;
        goto OUT;
      }
     my $ntry=0;
     $ok=0;
     until (flock DBM, LOCK_EX|LOCK_NB){
         sleep 2;
         $ntry=$ntry+1;
         if($ntry>10){
             untie %hash;
             goto OUT;
         }
     }
    $ok=1;
OUT:
      undef $db;
          if($ok){
              foreach my $tag (@tag){
               $ref->{$tag}=$hash{$tag};           
              }
               untie %hash;
           }
          else{
             throw DPS::DBProblem message=>"Unable to Open DB File";
         }
}

sub sendNC{
    my $ref=$DBServer::Singleton;
    my ($class,$cid,$ri,$rc)=@_;
    my $tag;
              if($cid->{Type} eq "Server"){
               $tag="nsl";
              }
              elsif($cid->{Type} eq "Producer"){
               $tag="ncl";
              }
    else{
        return;
    }      
    sendGeneric($cid,$ri,$rc,$tag,"uid");
}
sub sendNK{
    my $ref=$DBServer::Singleton;
    my ($class,$cid,$ri,$rc)=@_;
    my $tag;
               $tag="nkl";
    sendGeneric($cid,$ri,$rc,$tag,"uid");
}

sub sendNH{
    my $ref=$DBServer::Singleton;
    my ($class,$cid,$ri,$rc)=@_;
    my $tag;
               $tag="nhl";
    sendGeneric($cid,$ri,$rc,$tag,"HostName");
}



sub getAHS{
    my $ref=$DBServer::Singleton;
    my ($class,$cid)=@_;

# need to explicitely open db file in every sub 
    my $tag;
              if($cid->{Type} eq "Server"){
               $tag="ahls";
              }
              elsif($cid->{Type} eq "Producer"){
               $tag="ahlp";
              }
              elsif($cid->{Type} eq "DBServer"){
               $tag="ahls";
              }
              else{
               throw DPS::DBProblem message=>"Unable to getahs for $cid->{Type}";
              }
   
    getGeneric($cid,$tag);
    return ($#{$ref->{$tag}}+1,$ref->{$tag});
}
sub getNC{
    my $ref=$DBServer::Singleton;
    my ($class,$cid)=@_;

# need to explicitely open db file in every sub 
    my $tag;
              if($cid->{Type} eq "Server"){
               $tag="nsl";
              }
              elsif($cid->{Type} eq "Producer"){
               $tag="ncl";
              }
              else{
               throw DPS::DBProblem message=>"Unable to getnc for $cid->{Type}";
              }
   
    getGeneric($cid,$tag);
    return ($#{$ref->{$tag}}+1,$ref->{$tag});
}

sub getNK{
    my $ref=$DBServer::Singleton;
    my ($class,$cid)=@_;

# need to explicitely open db file in every sub 
    my $tag="nkl";
   
    getGeneric($cid,$tag);
    return ($#{$ref->{$tag}}+1,$ref->{$tag});
}

sub getNHS{
    my $ref=$DBServer::Singleton;
    my ($class,$cid)=@_;

# need to explicitely open db file in every sub 
    my $tag="nhl";
   
    getGeneric($cid,$tag);
    return ($#{$ref->{$tag}}+1,$ref->{$tag});
}
sub getACS{
    my $ref=$DBServer::Singleton;
    my ($class,$cid)=@_;

# need to explicitely open db file in every sub 
    my @tag;
    $#tag=-1;
              if($cid->{Type} eq "Server"){
               push @tag,"asl","asl_maxc";
              }
              elsif($cid->{Type} eq "Producer"){
               push @tag,"acl","acl_maxc";
              }
              elsif($cid->{Type} eq "DBServer"){
               push @tag,"adbsl","adbsl_maxc";
              }
              elsif($cid->{Type} eq "Monitor"){
               push @tag,"aml","aml_maxc";
              }
              else{
               throw DPS::DBProblem message=>"Unable to getacs for $cid->{Type}";
              }
   
    getGeneric($cid,@tag);
    return ($#{$ref->{$tag[0]}}+1,$ref->{$tag[0]},$ref->{$tag[1]});
}


            sub Clock { 
                if($a->{Status} eq $b->{Status}){
                    return $b->{Clock}<=>$a->{Clock};
                }
                elsif($a->{Status} ne $b->{Status}){
                    if($b->{Status} eq "OK"){
                     return 1;
                    }elsif($a->{Status} ne "NoResponse"){
                    return -1;
                    }else{
                     return 0;
                    }
                }
           }
sub getFreeHost{
    my $ref=$DBServer::Singleton;
    my ($class,$cid)=@_;
    if($cid->{Type} eq "Server"){
        getGeneric($cid,"ahls","nsl","asl");
        my $hash=$ref->{nsl}[0];
        if ($#{$ref->{asl}}+1 < $hash->{MaxClients}){
        my @sortedahl=sort Clock @{$ref->{ahls}};
              foreach my $ahl (@sortedahl){
                  if ($ahl->{Status} ne "NoResponse" and $ahl->{Status} ne "InActive" ){
                      if ($ahl->{ClientsRunning}<$ahl->{ClientsAllowed}){
                          $ahl->{Status}="InActive";
                          POADBServer::sendAH("Class",$cid,$ahl,"Update");
                          return (1,$ahl);                
                      }
                  }
              }
 }
               return (0,$ref->{ahls}[0]);             
}elsif($cid->{Type} eq "Producer"){
        getGeneric($cid,"ahlp","ncl","acl","rtb");
        my $hash=$ref->{ncl}[0];
        if ($#{$ref->{acl}}+1 < $hash->{MaxClients}){
        my $runstorerun=0;
        foreach my $run (@{$ref->{rtb}}){
         if($run->{Status} eq "ToBeRerun"){
          $runstorerun=$runstorerun+1;
      }
        }
        if($#{$ref->{acl}}+1 <$runstorerun){
        my @sortedahl=sort Clock @{$ref->{ahlp}};
              foreach my $ahl (@sortedahl){
                  if ($ahl->{Status} ne "NoResponse" and $ahl->{Status} ne "InActive" ){
                      if ($ahl->{ClientsRunning}<$ahl->{ClientsAllowed}){
                          $ahl->{Status}="InActive";
                           sendAH("Class",$cid,$ahl,"Update");
                          return (1,$ahl);                
                      }
                  }
              }
    }
}
               return (0,$ref->{ahlp}[0]);             
}
              else{
               throw DPS::DBProblem message=>"Unable to getahs for $cid->{Type}";
              }
}
