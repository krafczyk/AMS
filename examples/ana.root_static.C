#include "root.h"
#include "TFile.h"
#include "TH1F.h"

int main(){

      AMSChain ams;
      ams.Add("/f2users/choutko/g3v1g3.root");
      //ams.Add("http://pcamsf0.cern.ch/f2dah1/MC/AMS02/2004A/protons/el.pl1.10200/738197524.0000001.root");
      //ams.Add("rfio:/castor/cern.ch/ams/MC/AMS02/2004A/protons/el.pl1.10200/738197524.0000001.root");

      char* chfile = "amstest.root";
      TFile* hfile = new TFile (chfile, "RECREATE"); 

      TH1F* hrig = new TH1F ("hrig", "Momentum (GeV)", 50, -10., 10.);
      
      int ndata = ams.GetEntries();
      for (int entry=0; entry<ndata; entry++) {
            AMSEventR *pev = ams.GetEvent();
            if (pev==NULL) break;
            AMSEventR &ev = *pev;
            for (int i=0; i<ev.nParticle(); i++) {
                  ParticleR part = ev.Particle(i);
                  hrig->Fill(part.Momentum);
            }
      }

      hfile->Write();
      printf("\n>>> We have processed %d events\n", ndata);
      printf("\n>>> Histograms saved in '%s'\n", chfile);
}
