# $Id: online.perl,v 1.2 2004/01/20 19:59:20 choutko Exp $
#!/usr/bin/perl -w       
use strict;
use Carp;
my $output="../online/AMSEverything.cxx";
my $input="../include/root_methods.h";
open(FILEI,"<".$input) or die "Unable to open file $input\n";
open(FILEO,">".$output) or die "Unable to open file $output\n";
print FILEO "// \n";
print FILEO "// File Is Auto Generated \n";
print FILEO "// All Manual Changes Will Be Lost \n";
print FILEO "// \n";
print FILEO '#include <iostream.h>'."\n";
print FILEO '#include "AMSNtuple.h"'."\n";
print FILEO '#include "AMSDisplay.h"'."\n";
print FILEO '#include <math.h>'."\n";
print FILEO '#include <TPaveText.h>'."\n";
print FILEO '#include <TProfile.h>'."\n";
print FILEO '#include <TF1.h>'."\n";
print FILEO '#include "AMSEverything.h"'."\n";
print FILEO 'void AMSEverything::Book(){'."\n";
print FILEO "int color=18;\n";
my $i=0;
# scan file
my $funo="";
while ( my $line = <FILEI>){
    if($line=~/^\// ){
      next;
    }
    if($line =~/\(/){
     my @junk=split /\(/, $line;
     my @junk2 = split / /, $junk[0];
     my $fun=$junk2[$#junk2];
     my @junk3=split /\)/, $junk[1];
     if($junk3[0]=~/\,/){
#    two arguments
#      next;
         if(not ($line=~/\(j</)){
             next;
         }
         my @a3=split /\,/,$junk3[0];
         if($#a3>1){
          next;
         }
     }

     if($fun=~/size/){
       next;
     }
     @junk=split /\_/,$fun;
     if($funo ne $junk[0]){
         $funo=$junk[0];
          print FILEO 'AddSet("_'.$funo.'");'."\n"; 
          my @junk4=split /[ (]/,$junk3[1]; 
          $junk4[1]=~s/p/N/;
          print FILEO '_filled.push_back(new TH1F("'.$junk4[1].'","'.$junk4[1].'",200,0,0));'."\n";
     } 
     if($junk3[0]=~/\,/){
         my @junk5=split /[\<&]/,$line;
         for my $i (0...$junk5[1]-1){
          print FILEO 'AddSet("'.$fun."\_$i".'");'."\n"; 
          print FILEO '_filled.push_back(new TH1F("'.$fun."\_$i".'","'.$fun."\_$i".'",200,0,0));'."\n";
          }
     }
     else{
      print FILEO 'AddSet("'.$fun.'");'."\n"; 
      print FILEO '_filled.push_back(new TH1F("'.$fun.'","'.$fun.'",200,0,0));'."\n";
 }
    }
}
    $funo="";
    print FILEO ' cout <<"  TotalOf " <<_filled.size()<<" Histos Booked for AMSEveryThing "<<endl;'."\n";
close (FILEI);
open(FILEI,"<".$input) or die "Unable to open file $input\n";
print FILEO "}\n";
print FILEO 'void AMSEverything::ShowSet(Int_t Set){'."\n";
print FILEO "gPad->Clear();
TVirtualPad * gPadSave = gPad;
_filled[Set]->Draw();
gPadSave->cd();
}
void AMSEverything::Fill(AMSNtupleR * ntuple){
int i=0;\n";
while (my $line=<FILEI>){
    if($line=~/^\// ){
      next;
    }
    if($line =~/\(/){
     my @junk=split /\(u/, $line;
     my @junk2 = split / /, $junk[0];
     my $fun=$junk2[$#junk2];
     # two arguments?
     my @junk3=split /\)/, $junk[1];
     my $tag=1;
     if($junk3[0]=~/\,/){
         $tag=4;
         if(not ($line=~/\(j</)){
             next;
         }
         my @a3=split /\,/,$junk3[0];
         if($#a3>1){
          next;
         }


     }
     
     my @junk4=split /[ (]/,$junk3[1]; 
     $junk4[$tag]=~s/p/N/;
     if($fun=~/size/){
       next;
     }
     @junk=split /\_/,$fun;
     if($funo ne $junk[0]){
         $funo=$junk[0];
         print FILEO "_filled[i]->Fill(ntuple->".$junk4[1].'(),1.);'."\ni++;\n";
     }
     if($junk3[0]=~/\,/){
         my @junk5=split /[\<&]/,$line;
          print FILEO "for( int j=0;j\<$junk5[1];j++){\n";
          print FILEO " for(int loc=0;loc<ntuple->$junk4[$tag]();loc++)_filled[i]->Fill(ntuple->".$fun.'(j,loc),1.);'."\ni++;\n";
          print FILEO "}\n";   
     }
     else{
     print FILEO "for(int loc=0;loc<ntuple->$junk4[$tag]();loc++)_filled[i]->Fill(ntuple->".$fun.'(loc),1.);'."\ni++;\n";
     }
    }
}
print FILEO "}\n";

