#include <cstdio>
#include "root_RVSP.h"
#include "amschain.h"
#include "HistoMan.h"


void ProcessEvent(AMSEventR* ev);
void BookHistos();
 HistoMan hman;

int analyze_CINT(AMSChain *ch, int num=9999999){
 
  hman.Enable();
  hman.Setname("HistoOut.root");

  BookHistos();
 
 int num2= (ch->GetEntries()<num)?ch->GetEntries():num;
 for(int ii=0;ii<num2;ii++){
   if(ii%10000==0) printf("Processed %7d out of %7d\n",ii,num2);
   AMSEventR* ev=ch->GetEvent(ii);
   ProcessEvent(ev);
 }
 
 hman.Save();

 return 1;
}

void BookHistos(){

  hman.Add( new TH1F("nTrTrack","Number of Tracks; tracks; events",10,0,10));
  hman.Add( new TH1F("Rigidity","Standard Rigidity; GeV; events",100,0,1000));
  hman.Add( new TH1F("RigidityA","Rigidity Alcaraz; GeV; events",100,0,1000));

}

void ProcessEvent(AMSEventR* ev){

  int nn=ev->nTrTrack();
  hman.Fill("nTrTrack",nn);
  if(nn>0){
    TrTrackR* tr=ev->pTrTrack(0);
    hman.Fill("Rigidity",tr->GetRigidity());
    int fcode=tr->iTrTrackPar(2,0,0);
    if(fcode>=0)
      hman.Fill("RigidityA",tr->GetRigidity(fcode));
  }
  return;
}
