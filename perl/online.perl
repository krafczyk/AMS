# $Id: online.perl,v 1.1 2004/01/19 22:37:03 choutko Exp $
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
      next;
     }

     if($fun=~/size/){
       next;
     }

     print FILEO 'AddSet("'.$fun.'");'."\n"; 
     print FILEO '_filled.push_back(new TH1F("'.$fun.'","'.$fun.'",200,0,0));'."\n";

    }
}
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
     my @junk=split /\(/, $line;
     my @junk2 = split / /, $junk[0];
     my $fun=$junk2[$#junk2];
     # two arguments?
     my @junk3=split /\)/, $junk[1];
     if($junk3[0]=~/\,/){
      next;
     }
     my @junk4=split /[ (]/,$junk3[1]; 
     $junk4[1]=~s/p/N/;
     if($fun=~/size/){
       next;
     }
     print FILEO "for(int loc=0;loc<ntuple->$junk4[1]();loc++)_filled[i]->Fill(ntuple->".$fun.'(loc),1.);'."\ni++;\n";
    }
}
print FILEO "}\n";

