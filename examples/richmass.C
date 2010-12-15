#define richmass_cxx
#define _PGTRACK_
#include "/afs/cern.ch/ams/Offline/KSC/AMS/include/root_RVSP.h"
#include "TF1.h"
#include "TH2.h"
#include "TProfile2D.h"
#include "TStyle.h"
#include "TCanvas.h"
#include "TLine.h"
#include "TEventList.h"
#include "TSelector.h"
#include "TChain.h"
#include <TROOT.h>
#include <TTree.h>
#include <fstream.h>

///  This is an example of user class to process AMS Root Files 
/*!
 This class is derived from the AMSEventR class. \n

 The following member functions must be called by the user: \n
    void UBegin():       called everytime a loop on the tree starts,
                         a convenient place to create your histograms. \n
    void UProcessFill(): called in the entry loop for all entries \n
    void UTerminate():   called at the end of a loop on the tree,
                         a convenient place to draw/fit your histograms. \n
 
 The following member function is optional: \n
    bool UProcessCut(): called in the entry loop to accept/reject an event \n

   To use this file, try the following session on your Tree T: \n
 Root > T->Process("stlv.C+") -> compile library if not up to date \n
 Root > T->Process("stlv.C++") -> force always compilation \n
 Root > T->Process("stlv.C")  -> non-compiled (interpreted) mode \n
*/

////// FEW VARIABLES FOR GEOMTRICAL SELECTION
const int grid_side_length=11;
const int n_tiles=grid_side_length*grid_side_length;
const double tile_width=0.1+11.5; 


////// UTILITIES TO SIMPLIFY SELECTION CUTS DEFITIONS
#define SELECT(name,cond) {if(cond){                                    \
      if(!CUTS::selected[CUTS::last_cut])				\
	sprintf(CUTS::cut_name[CUTS::last_cut],name);			\
      CUTS::selected[CUTS::last_cut++]++;				\
    }else return false;}

#define START {CUTS::last_cut=0;if(CUTS::first){CUTS::first=false;CUTS::Init();}}

namespace CUTS{
  const int max_cuts=100; // For the eff. report
  char cut_name[max_cuts][1000];          
  int selected[max_cuts];
  int last_cut=0;
  bool first=true;

  void Init(){for(int i=0;i<max_cuts;selected[i++]=0);}

  void Report(){
    // Output the selection efficiencies
    for(int i=1;selected[i]>0;i++){
      printf("%80s  ENTRIES %10i   Eff %10.5g\n",
	     cut_name[i],
	     selected[i],
	     double(selected[i])/selected[i-1]);
    }
  }
};


//////// The analysis itself
class richmass : public AMSEventR {
   public :

  int events;

  // Fast dynamic calibration (for KSC only in principle)
  double index_correction[n_tiles];
  TH1F   index_histos[n_tiles];


  // Histograms
  TH2F CutTest;
  TH2F CutTestCorrected;
  TH2F WidthTest;
  TH2F BetaDiffTest;
  TH2F UDistTest;

  TH1F InvMass[2];
  TH2F InvMassVsDist;

  // Utils
  int GetTileNumber(double x,double y);  
  double GetDistanceToTileBorder(double x,double y);
  int Calibrate(int peak_events=30,int width=3);
  double betaWidth();
  void Report();


  richmass(TTree *tree=0){ };


/// This is necessary if you want to run without compilation as:
///     chain.Process("stlv.C");
#ifdef __CINT__
#include <process_cint.h>
#endif

   /// User Function called before starting the event loop.
   /// Book Histos
   virtual void    UBegin();

   /// User Function called to preselect events (NOT MANDATORY).
   /// Called for all entries.
   virtual bool    UProcessCut();
   virtual bool    UProcessStatus(unsigned long long status);

   /// User Function called to analyze each event.
   /// Called for all entries.
   /// Fills histograms.
   virtual void    UProcessFill();

   /// Called at the end of a loop on the tree,
   /// a convenient place to draw/fit your histograms. \n
   virtual void    UTerminate();
};




void richmass::UBegin(){
  CUTS::Init();  // Initialize the simple cuts logger
  events=0;
  for(int i=0;i<n_tiles;index_correction[i++]=-1);
  InvMass[0]=TH1F("InvMassAgl","InvMassAgl",1000,0,20);
  InvMass[1]=TH1F("InvMassNaF","InvMassNaf",1000,0,20);
  InvMassVsDist=TH2F("InvMassVsDist","InvMassVsDist",1000,0,20,1000,0,6);
  CutTest=TH2F("CutTest","CutTest",1000,0.95,1.005,1000,0,6);
  CutTestCorrected=TH2F("CutTestC","CutTestC",1000,0.95,1.005,1000,0,6);
  WidthTest=TH2F("WidthTest","WidthTest",1000,0.95,1.01,1000,0,20);
  BetaDiffTest=TH2F("BetaDiff","BetaDiff",1000,0.95,1.01,1000,-0.01,0.01);
  UDistTest=TH2F("UDist","UDist",1000,0.95,1.01,1000,0,10);
}


bool richmass::UProcessStatus(unsigned long long status){
  if( !(status & 3) ||  !((status>>4)&1))return false;
  else return true;
}


// UProcessCut() is not mandatory
bool richmass::UProcessCut(){
  START;
  SELECT("------------- Clean AMS events ------------------",true);
  SELECT("No antis",nAntiCluster()==0);
  SELECT("1 Particle",nParticle()==1);
  SELECT("At least 1  tr track",nTrTrack()>0 && pParticle(0)->pTrTrack());
  SELECT("At least 6 hits in tr track",pParticle(0)->pTrTrack()->GetNhitsXY()>=6);
  SELECT("At most 1 trd track",nTrdTrack()<=1);
  SELECT("At most 4 tof clusters",nTofCluster()<=4);
  SELECT("No more than 1 rich crossing particles",pParticle(0)->RichParticles<=1);

  SELECT("------------- Rich Ring Quality ------------------",true);
  SELECT("Ring associated to particle",pParticle(0)->pRichRing());
  SELECT("Ring associated to tr track",pParticle(0)->pRichRing()->pTrTrack());
  SELECT("Ring not contaminated by crossing particles",(pParticle(0)->pRichRing()->Status&1)==0);

  RichRingR *ring=pParticle(0)->pRichRing();
  double r2=ring->TrRadPos[0]*ring->TrRadPos[0]+ring->TrRadPos[1]*ring->TrRadPos[1];
  SELECT("Crossing point within fiducial volume",r2<=60*60);
  
  int isNaf=(ring->Status&2)!=0;
  double dist=GetDistanceToTileBorder(ring->TrRadPos[0],ring->TrRadPos[1]);
  
  SELECT("Crossing point not close to  tile border",isNaf ||  dist>1.0);
  //  SELECT("Crossing point not close to  tile border",isNaf ||  dist>0.5);

  SELECT("Enough hits in ring",ring->Used>3); 
  SELECT("Enough expected hits in ring",ring->NpExp>2); 
  SELECT("Ring hits distribution close to expected one ",ring->Prob>0.01); 
  SELECT("Ring Width small enough",betaWidth()<2.5);


  SELECT("------------- Others ------------------",true);  
  int tile=Calibrate();
  SELECT("Already calibrated tile",tile!=-1 && index_correction[tile]!=-1);
  ring->Beta*=index_correction[tile]; // Crude correction. It could fail close to the threshold
  ring->BetaRefit*=index_correction[tile]; // Crude correction. It could fail close to the threshold


  // Few test histograms. Filled here because I include a cut
  // which will avoid filling them  beloq
  if(fabs(pParticle(0)->Momentum)>10){
    CutTest.Fill(ring->Beta/index_correction[tile],dist);
    CutTestCorrected.Fill(ring->Beta,dist);
    WidthTest.Fill(ring->Beta,betaWidth());
    BetaDiffTest.Fill(ring->Beta,ring->Beta-ring->BetaRefit);
    UDistTest.Fill(ring->Beta,ring->UDist);
  }

  SELECT("Positive reconstructed momentum",pParticle(0)->Momentum>0);

  double beta=ring->Beta;
  double gamma2=1/(1-beta*beta);
  SELECT("Gamma within meaningful bounds",gamma2>0 && gamma2<(isNaf?30:100));


  

  return true;
}

void richmass::UProcessFill()
{
  RichRingR *ring=pParticle(0)->pRichRing();
  double beta=ring->Beta;
  int isNaf=(ring->Status&2)!=0;


  // The gamma^2
  double gamma2=1/(1-beta*beta);

  // Compute mass
  double p=pParticle(0)->Momentum;
  double mass2=p*p*(1/beta/beta-1);

  events++;
  if((events%1000)==0){
    cout<<"FILLED "<<events<<" EVENTS"<<endl;
    Report(); // To get some "online" histos  
  }

  InvMass[isNaf].Fill(1/sqrt(mass2));
  if(!isNaf){
    double dist=GetDistanceToTileBorder(ring->TrRadPos[0],ring->TrRadPos[1]);
    InvMassVsDist.Fill(1/sqrt(mass2),dist);
  }
}


void richmass::UTerminate()
{
  cout<<"DUMPING "<<endl;
  Report();
  //  cout<<endl<<endl<<"SELECTION EFF ****************************************"<<endl;
  //  CUTS::Report();
}
//////////////// Utilities

int richmass::GetTileNumber(double x,double y){
  int nx=int(x/tile_width+5.5);
  int ny=int(y/tile_width+5.5);
  int tile=ny*grid_side_length+nx;
  if(tile<0 || tile>n_tiles){
    cout<<"GetTileNumber returns a nonsensical value: "<<endl
	<<"   x="<<x<<"  y="<<y<<" nx= "<<nx<<" ny="<<ny<<" tile="<<tile<<endl;
    return -1;
  }
  return tile;
}

double richmass::GetDistanceToTileBorder(double x,double y){
  double dx=fabs(fmod(x+tile_width/2,tile_width))-tile_width/2;
  double dy=fabs(fmod(y+tile_width/2,tile_width))-tile_width/2;
  return tile_width/2-(fabs(dx+dy)/2+fabs(dx-dy)/2);
}


int richmass::Calibrate(int peak_events,int width){
  // This is only valid for sea level cosmics 
  RichRingR *ring=pParticle(0)->pRichRing();
  int tile=GetTileNumber(ring->TrRadPos[0],ring->TrRadPos[1]);
  int isNaf=(ring->Status&2)!=0;
  
  if(tile==-1) goto next;
  if(fabs(pParticle(0)->Momentum)<10) goto next;
  
  double dist=GetDistanceToTileBorder(ring->TrRadPos[0],ring->TrRadPos[1]);
  CutTest.Fill(ring->Beta,dist);
  
  if(index_histos[tile].GetNbinsX()==1){
    // Initialize it
    if(!isNaf)
      index_histos[tile]=TH1F("","",100,0.95,1.005);
    else
      index_histos[tile]=TH1F("","",100,1/1.33,1.015);
  }
  
  
  index_histos[tile].Fill(ring->Beta);
  if(index_histos[tile].GetEntries()<peak_events) goto next;
  
  // Compute the peak position
  int peak_bin=index_histos[tile].GetMaximumBin();
  
  double weight=0;
  double sum=0;
  for(int i=-width;i<=width;i++){
    weight+=index_histos[tile].GetBinContent(peak_bin+i);
    sum+=index_histos[tile].GetBinContent(peak_bin+i)*index_histos[tile].GetXaxis()->GetBinCenter(peak_bin+i);
  }
  
  
  if(weight==0){
    static int counter=0;
    cerr<<"BIG PROBLEM COMPUTING THE NEW PEAK POSITION. DUMPING TO FILE "<<endl;
    counter++;
    TFile f("problems.root","UPDATE");
    index_histos[tile].Write(Form("TILE %i Problem %i",tile,counter));
    f.Close();
  }
  
  sum/=weight;
  index_histos[tile].Reset();
  
  if(index_correction[tile]<0)  index_correction[tile]=1.0/sum; 
  else index_correction[tile]=1.0/sum*0.5+index_correction[tile]*0.5; // Slow correction
  
  
  
 next:
  return tile;
}


double richmass::betaWidth(){
  RichRingR *ring=pParticle(0)->pRichRing();
  double weight=0;
  double sum=0;
  for(int i=0;i<10;i++){
    //    cout<<" "<<i<<"=="<<ring->UsedWindow[i];
    double w=ring->UsedWindow[i]-(i>0?ring->UsedWindow[i-1]:0);
    weight+=w;
    sum+=(0.5+i)*(0.5+i)*w;
  }
  //  cout<<"  gives "<<sqrt(2*sum/weight)<<endl;
  return sqrt(2*sum/weight);
}


void richmass::Report(){
  TFile results("RichMass.root","RECREATE");
  InvMass[0].Write();
  InvMass[1].Write();
  InvMassVsDist.Write();
  CutTest.Write();
  CutTestCorrected.Write();
  WidthTest.Write();
  BetaDiffTest.Write();
  UDistTest.Write();
  results.Close();
  cout<<endl<<endl<<"SELECTION EFF ****************************************"<<endl;
  CUTS::Report();
}

