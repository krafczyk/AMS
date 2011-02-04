/**********************************************************
Implementatation of the analysis routines themselve
 **********************************************************/

// Include the definitions
#include "AnalysisFramework.h"

//
// Constants definitions of the tiles geometry
//
const int grid_side_length=11;
const int n_tiles=grid_side_length*grid_side_length;
const double tile_width=0.1+11.5; 

//
// Definition of the dsts. I keep them in a separated file to create the libraries using root
// by writing (within root) .L dst.C+
// This creates dst_C.so, which can be linked in the executable or loaded into the root interpreter 
//
#include "dst.C"


///
/// Analysis structure declaracion
///
NEW_ANALYSIS(RichMass)

//
// Define here the variables that are global to the current analysis
//  

  // Dynamic calibration utilities

  double indexCorrection[n_tiles];
  TH1F   *indexHistos[n_tiles];

  // Histograms to be stored. They should be pointer to avoid problems
  TH1F *InvMass[2];


  // Trees to store further information
  TTree *calibration;

  EventData eventData;
  EventBeta eventBeta;
  EventFlags eventFlags;



//
// Declare here the functions, and define them if thwy are very one-liners
//
  void init();
  bool preProcess();
  void process();
  void postProcess();

  int keepCalibrationUpdated(int peak_events=30,int width=3);
  int GetTileNumber(double x,double y){int nx=int(x/tile_width+5.5);int ny=int(y/tile_width+5.5);int t=ny*grid_side_length+nx;return t>n_tiles?-1:t;};
  double CalibrationFactor(int tile){return (tile<0 || tile>n_tiles || indexCorrection[tile]==-1)?1.0 : indexCorrection[tile];}
  double GetDistanceToTileBorder();
  double betaWidth();
END



void RichMass::init(){
  //
  // All static histograms have to be initialize here due to some root idiosincracy
  //
  for(int i=0;i<n_tiles;indexCorrection[i++]=-1);
  for(int i=0;i<n_tiles;indexHistos[i++]=new TH1F);
  InvMass[0]=new TH1F("InvMassAgl","InvMassAgl",1000,0,20);
  InvMass[1]=new TH1F("InvMassNaF","InvMassNaf",1000,0,20);

  // A tree to store some xtra data
  calibration=new TTree("Calibration","Calibration");
  // Add the branches
  calibration->Branch("Time","EventData",&eventData);
  calibration->Branch("Beta","EventBeta",&eventBeta);
  calibration->Branch("Flag","EventFlags",&eventFlags);
}


/**************************************************
 Definition of all my cuts
 **************************************************/
bool RichMass::preProcess(){
  //
  // Preselection
  //
  SELECT("1 Tracker, RICH particle",(fStatus&0b110011)==0b110001);
  SELECT("No antis",!(fStatus&(0b11<<21)));
  SELECT("At most 1 trd track",(fStatus&(0b11<<8))<=(0b1<<8));
  SELECT("At most 4 tof clusters",(fStatus&(0b111<<10))<=(0b100<<10));
  SELECT("At least 1 tr track",fStatus&(0b11<<13));
  SELECT("At least 1 rich ring",fStatus&(0b11<<15));
  SELECT("At most 1 crossing particle at rich",pParticle(0)->RichParticles<=1);

  RichRingR *ring=Particle(0).pRichRing();

  SELECT("PointersCheck",ring && ring->pTrTrack());

  //
  // Masks
  //
  double x=ring->TrRadPos[0]*ring->TrRadPos[0];
  double y=ring->TrRadPos[0]*ring->TrRadPos[1];
  double r2=x*x+y*y;
  MASK(cleanRing,(ring->Status&1)==0);                   // Remove tails
  MASK(enoughTrHits,ring->pTrTrack()->GetNhitsXY()>=6);  // Remove tails
  MASK(richFiducial,r2<=60*60);                          // Just in case
  MASK(aglNaFBorder,max(fabs(y),fabs(x))>19);              // Remove bump in beta=0.99 for beta=1 events
  MASK(isNaF,(ring->Status&2)!=0);                       // Select NaF
  MASK(farFromTileBorder,ring->DistanceTileBorder()>1.0);// Maybe too strong
  MASK(enoughRichHits,ring->Used>3);                     // Reasonble amount of hits
  MASK(enoughtExpectedRichHits,ring->NpExp>2);           // Maybe not needed
  MASK(probRich,ring->Prob>0.01);                        // Safe
  MASK(betaOne,fabs(Particle(0).Momentum)>10);           // For cosmic muons
  MASK(positiveMomentum,Particle(0).Momentum>0);         // Reduce background for protons
  MASK(ringWidth,betaWidth()<2);                         // Strong, but tails killer far from threshold                         

  //
  // Reports
  //

  EVERY(1000) CUTS_REPORT END;
  


  //
  // This function should return true to continue processing the event 
  //
  return true;
}


void RichMass::process(){
  RichRingR &ring=*(Particle(0).pRichRing());

  //
  // Dynamic index calibration
  //
  int &tile=eventBeta.tile;
  double &factor=eventBeta.factor;
  double &beta=eventBeta.beta;

  tile=keepCalibrationUpdated();
  factor=CalibrationFactor(tile);
  beta=ring.Beta*factor;

  // 
  // Compute gamma^2 to cut on it
  // 

  double gamma2=1/(1-beta*beta);
  MASK(goodGamma,gamma2>0 && gamma2<(CHECK(isNaf)?30:100));

  // Compute mass
  double p=Particle(0).Momentum;
  double mass2=p*p*(1/beta/beta-1);
  double &invMass=eventBeta.invMass;
  invMass=1/sqrt(mass2);

  // Golden is prob>0.1 && npexp>2 && distance>1 && max(abs(y),abs(x))>19 && width<2 && used>3  

  if(CHECK(positiveMomentum) &&   // Protons have p>0
     CHECK(cleanRing)        &&   // Ring no contaminated with richparticles
     CHECK(enoughTrHits)     &&   // Track with enough hits 
     ring.Used>3             &&   // More than 3 hits
     ring.Prob>0.05          &&   // Good ring quality
     betaWidth()<4           &&   // No garbage aroung ring
     CHECK(goodGamma)){           // Good lorenz factor

    if(CHECK(isNaF) ||
       (CHECK(aglNaFBorder) && ring.DistanceTileBorder()>1))
      InvMass[CHECK(isNaF)]->Fill(1/sqrt(mass2));
  }
}


void RichMass::postProcess(){
  RichRingR &ring=*(pParticle(0)->pRichRing());

  eventData.now[0]=fHeader.Time[0];
  eventData.now[1]=fHeader.Time[1];
  eventBeta.beta=ring.Beta;
  eventBeta.used=ring.Used;
  eventBeta.distance=ring.DistanceTileBorder();
  eventBeta.x=ring.TrRadPos[0];
  eventBeta.y=ring.TrRadPos[1];
  eventBeta.npexp=ring.NpExp;
  eventBeta.prob=ring.Prob;
  eventBeta.momentum=Particle(0).Momentum;
  eventBeta.width=betaWidth();


#define F(_name) eventFlags._name=CHECK(_name)
  F(enoughTrHits);
  F(richFiducial);
  F(isNaF);
  F(farFromTileBorder);
  F(enoughRichHits);
  F(enoughtExpectedRichHits);
  F(probRich);
  F(betaOne);
  F(positiveMomentum);
  F(goodGamma);
  F(ringWidth);
  F(cleanRing);
#undef F    

  calibration->Fill();
}


//
// Keep the index calibration updated, and correct the beta and npexp accordingly
//
int RichMass::keepCalibrationUpdated(int peakEvents,int windowWidth){
  //
  // Get the current tile
  //
  RichRingR &ring=*(Particle(0).pRichRing());
  int tile=GetTileNumber(ring.TrRadPos[0],ring.TrRadPos[1]);
  if(tile<0) return -1;

  //
  // Check that beta=1
  //
  if(!CHECK(betaOne)) return tile;


  //
  // Initialize the histogram if required
  //
  if(indexHistos[tile]->GetNbinsX()==1){
    if(!CHECK(isNaf)) indexHistos[tile]->SetBins(100,0.95,1.005); 
    else indexHistos[tile]->SetBins(100,1/1.33,1.015);
  }

  //
  // Fill entries
  //
  indexHistos[tile]->Fill(ring.Beta);

  //
  // Check if we collected enough data to update the index for the current tile
  //
  if(indexHistos[tile]->GetEntries()<peakEvents) return tile;

  //
  // Find the beta=1 peak in the histogram
  // 
  int peak_bin=indexHistos[tile]->GetMaximumBin();

  //
  // Compute the position using an integration window around this ben os width windowWidth
  //
  double weight=0,sum=0;
  for(int i=-windowWidth;i<=windowWidth;i++){
    weight+=indexHistos[tile]->GetBinContent(peak_bin+i);
    sum+=indexHistos[tile]->GetBinContent(peak_bin+i)*indexHistos[tile]->GetXaxis()->GetBinCenter(peak_bin+i);
  }

  if(weight<peakEvents) return tile;
  sum/=weight;

  //
  // We  got the value, store it
  //
  if(indexCorrection[tile]<0)  indexCorrection[tile]=1.0/sum;       // First correction
  else indexCorrection[tile]=1.0/sum*0.5+indexCorrection[tile]*0.5; // Adiabatic correction

  
  //
  // Reset the histogram to restart the evaluation
  //
  indexHistos[tile]->Reset();

  //
  // Return
  //
  return tile;
}



//
// Determina if there are hits around the ring not associated
//
double RichMass::betaWidth(){
  RichRingR ring=*(Particle(0).pRichRing());
  double weight=0,sum=0;

  for(int i=0;i<10;i++){
    double w=ring.UsedWindow[i]-(i>0?ring.UsedWindow[i-1]:0);
    weight+=w;
    sum+=(0.5+i)*(0.5+i)*w;
  }
  return sqrt(2*sum/weight);
}


double RichMass::GetDistanceToTileBorder(){
  RichRingR *ring=Particle(0).pRichRing();
  double x=ring->TrRadPos[0],y=ring->TrRadPos[1];
  double dx=fabs(fmod(x+tile_width/2,tile_width))-tile_width/2;
  double dy=fabs(fmod(y+tile_width/2,tile_width))-tile_width/2;
  return tile_width/2-(fabs(dx+dy)/2+fabs(dx-dy)/2);
}

#ifdef STANDALONE
#include "amschain.h"

void main(void){
  AMSChain chain;
  chain.Add("/r0fc00/Data/AMS02/2009B/data2010/cosmics.ksc.p3/*.root");

  RichMass example;
  chain.Process(&example,          // Analysis to run
		"richmass.root");  // Output file
  		
}
#endif
