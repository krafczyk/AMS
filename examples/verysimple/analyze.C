#include <signal.h>
#include <cstdio>
#include "root_RVSP.h"
#include "amschain.h"
#include "HistoMan.h"

#define MIN(a,b) (a<b)?a:b
int stop=0;

void ProcessEvent(AMSEventR* ev);
void BookHistos();
void shandler(int sig){
  stop=1;
  return;
}



int main(int argc,char ** argv){
int num;
 char datafiles[255];
   if(argc==2){
     sprintf(datafiles,"%s",argv[1]);
     num=10000;
   }
   else if(argc==3){
     sprintf(datafiles,"%s",argv[1]);
     num=atoi(argv[2]);
   }else{
     printf("Usage: %s  <fileset_to_analyze> [max_number_of_events]\n",argv[0]);
     exit(-1);
   }
 
 signal(SIGTERM, shandler);
 signal(SIGINT, shandler);
 hman.Enable();
 hman.Setname("HistoOut.root");

 BookHistos();

 AMSChain ch;
 ch.Add(datafiles);

 
 int num2=MIN(ch.GetEntries(),num);
 for(int ii=0;ii<num2;ii++){
   if(ii%10000==0) printf("Processed %7d out of %7d\n",ii,num2);
   AMSEventR* ev=ch.GetEvent(ii);
   ProcessEvent(ev);
   if(stop==1) break;
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
