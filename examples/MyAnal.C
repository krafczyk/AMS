///   \example MyAnal.C
///
#include <root.h>
#include "TFile.h"
#include "TH1.h"

class MyAnal : public AMSEventR {
public :
      void    UBegin();
      bool    UProcessCut();
      void    UProcessFill();
      void    UTerminate();

      AMSEventList* select_list;
      AMSEventList* badrun_list;

      AMSMyTrack* my_track;

      TFile* hfile;
      TH1F* h_ang;
      TH1F* h_ang2;

      bool Trigger_Flag();
};

void MyAnal::UBegin(){
      hfile = new TFile ("amstest.root", "RECREATE"); 
      h_ang = new TH1F("h_ang","Angular difference (rad)",100,0.0,0.05);
      h_ang2 = new TH1F("h_ang2","Angular difference 2 (rad)",100,0.0,0.05);

      select_list = new AMSEventList;
      badrun_list = new AMSEventList("my_badruns.list");

      my_track = new AMSMyTrack();

}

// UProcessCut() is not mandatory
bool MyAnal::UProcessCut(){
      if (badrun_list->Contains(this)) return false;
      if (Trigger_Flag()==false) return false;

      if (nParticle()!=1) return false;
      if (!Particle(0).pTrTrack()) return false;
      return true;
}

void MyAnal::UProcessFill() {

      if (nMCEventg()<=0) return;

      ParticleR part = Particle(0);
      MCEventgR mcevent = MCEventg(0);

      // Angular difference with respect to usual track
      float phi = part.Phi;
      float theta = part.Theta;
      float dotprod = sin(theta)*cos(phi)*mcevent.Dir[0]
                    + sin(theta)*sin(phi)*mcevent.Dir[1]
                    + cos(theta)         *mcevent.Dir[2];
      float delta_ang = acos(dotprod);
      h_ang->Fill(delta_ang);

      // Select events which are better than 0.01 rad accurate
      if (delta_ang<0.01) select_list->Add(this);

      // Angular difference with respect to refitted track
      my_track->use_hits_from(Particle(0).pTrTrack());
      my_track->Fit();

      phi = my_track->Phi;
      theta = my_track->Theta;
      dotprod = sin(theta)*cos(phi)*mcevent.Dir[0]
                    + sin(theta)*sin(phi)*mcevent.Dir[1]
                    + cos(theta)         *mcevent.Dir[2];
      delta_ang = acos(dotprod);
      h_ang2->Fill(delta_ang);

};


void MyAnal::UTerminate()
{
      // Write histograms into a ROOT file
      hfile->Write();

      // Write list of selected events
      select_list->Write("select.list");

      // Write Header and Particle Information of selected events in a root file
      _Tree->SetBranchStatus("*",0);
      _Tree->SetBranchStatus("ev.",1);
      _Tree->SetBranchStatus("ev.fHeader",1);
      _Tree->SetBranchStatus("ev.fParticle",1);
      select_list->Write((TChain*)_Tree, "select.root");

      printf("\n>>> We have processed %d events\n\n", (int)_Tree->GetEntries());
      printf("\n>>> Histograms saved in '%s'\n\n", hfile->GetName());
}

bool MyAnal::Trigger_Flag() {
    if (nLevel1()<=0) return false;
    Level1R lvl1 = Level1(0);
        
// TOF
    int tofok = 0;
    if (lvl1.TofFlag>=0) tofok = 1; 

// ECAL
    int ecalok = 0;
    if (lvl1.EcalFlag>0) ecalok = 1; 

// Anticounters
    const int ANTICUT = 1;
    const float MAGCUT = 0.7;
    const int MAXANTI = 8;
    int antiok = 0;
    unsigned int antipatt = lvl1.AntiPatt>0;
    int nanti = 0;
    for(int i=0;i<MAXANTI;i++){ 
         unsigned int my_bit = 1<<i;
         if( (antipatt & my_bit)>0 || (antipatt & (my_bit<<8))>0 ) nanti += 1;
    }
    float maglat = fabs(fHeader.ThetaM);
    if (maglat>MAGCUT && nanti==0) antiok=1;
    if (maglat<MAGCUT && nanti<=ANTICUT) antiok=1;

// Ready for trigger flag
//    return tofok || ecalok ;
    return (tofok && antiok) || ecalok ;

};

