//  $Id$
#include "TrExtAlignDB.h"
#include "TkDBc.h"
#include "TFile.h"
#include "random.h"
#include "tkdcards.h"
#include "timeid.h"
#include "commonsi.h"
#include "TrRecHit.h"
#include "VCon.h"
#include "DynAlignment.h"

#ifdef __ROOTSHAREDLIBRARY__
#include "root.h"
#endif

// Use TKGEOMFFKEY.MaxAlignedRun instead
//#define  MAXALIGNEDRUN 1374300000


int  UpdateExtLayer(int type=0,int lad1=-1,int lad9=-1);
ClassImp(TrExtAlignPar);
ClassImp(TrExtAlignDB);

TrExtAlignDB* TrExtAlignDB::Head=0;
int TrExtAlignDB::ForceFromTDV=0;
int TrExtAlignDB::ForceLocalAlign=0;

int TrExtAlignDB::version = 2;
int TrExtAlignDB::NoCiematLocal=0;


int TrExtAlignDB::Ciemat=0;
float TrExtAlignDB::SL1[18]={0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0 };
float TrExtAlignDB::SL9[18]={0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0 };

float TrExtAlignDB::Sofs[4]={2.5e-4, 2.5e-4, 6.5e-4, 5.8e-4};
float TrExtAlignDB::Sofs2[12]={0,         0, 3.7e-4, 6.0e-4,
			       10e-4, 20e-4,  51e-4,  20e-4,
			        6e-6,     0,      0,      0};

void TrExtAlignDB::TuneSofs2(uint time)
{
  if (TKGEOMFFKEY.L2AlignPar[2] > 0) {
    TKGEOMFFKEY.L2AlignPar[2] = 0;
    Double_t y2s = 3600*24*365;
    Double_t yr  = (TKGEOMFFKEY.L2AlignPar[0]-time)/y2s;
    Double_t dy1 = yr*1.45e-4;
    Double_t dy9 = yr*0.98e-4;
    TrExtAlignDB::Sofs2[0] += dy1; TrExtAlignDB::Sofs2[1] += dy9;
    TrExtAlignDB::Sofs2[2] += dy1; TrExtAlignDB::Sofs2[3] += dy9;

    static int n = 0;
    if (n++ < 10)
      cout << "TrExtAlignDB::TuneSofs2-I-yr,Sofs2= " << yr << " "
	   << TrExtAlignDB::Sofs2[0]*1e4 << " "
	   << TrExtAlignDB::Sofs2[1]*1e4 << " "
	   << TrExtAlignDB::Sofs2[2]*1e4 << " "
	   << TrExtAlignDB::Sofs2[3]*1e4 << endl;
  }
}

Double_t TrExtAlignDB::CorrectRigidity(uint time, Double_t rgt,
				                  Double_t factor)
{
  if (rgt == 0) return 0;

  Double_t t0  = 1347000000;   // TKGEOMFFKEY.L2AlignPar[0]
  Double_t y2s = 3600*24*365;
  Double_t p0  =  0.049*(time-t0)/y2s;
  Double_t p1  = -0.015*(time-t0)/y2s;
  Double_t p2  =  0.768;
  Double_t p3  =  0.438;
  Double_t lr  = TMath::Log10(TMath::Abs(rgt));
  Double_t dr  = (p0+p1*TMath::Erfc((lr-p2)/p3))*1e-3;

  return 1/(1/rgt+dr*factor);
}


void TrExtAlignDB::ForceLatestAlignmentFromTDV(int pgversion,const char* CIEMAT_name){
/*
  TrExtAlignDB::ForceFromTDV=1;
  TkDBc::ForceFromTDV=1;
  DynAlManager::need2bookTDV=true;
  TrExtAlignDB::version = 2;
*/
  DynAlManager::SetTDVName(CIEMAT_name,1);
}

void TrExtAlignPar::Print(Option_t *) const
{
  cout << Form("dpos %6.1f %6.1f %6.1f",
	       dpos[0]*1e4, dpos[1]*1e4, dpos[2]*1e4);

  if (angles[0] != 0 || angles[1] != 0 || angles[2] != 0)
    cout << Form("  angles %6.1f %6.1f %6.1f",
		 angles[0]*30e4, angles[1]*30e4, angles[2]*30e4);
  cout << endl;
}

uint TrExtAlignDB::Find(int lay, uint time) const
{
  ealgITc it;
  if (lay == 8 && (it = L8.lower_bound(time)) != L8.end()) return it->first;
  if (lay == 9 && (it = L9.lower_bound(time)) != L9.end()) return it->first;

  return 0;
}

const TrExtAlignPar *TrExtAlignDB::Get(int lay, uint time) const
{
  ealgITc it1 = (lay == 8) ? L8.lower_bound(time)
                           : L9.lower_bound(time);
  if (lay == 8 && it1 == L8.end()) return 0;
  if (lay == 9 && it1 == L9.end()) return 0;

  ealgITc it0 = it1; it0--;
  if (lay == 8 && it0 == L8.begin()) return &(it1->second);
  if (lay == 9 && it0 == L9.begin()) return &(it1->second);

  unsigned int tt = (it0->first+it1->first)/2;
  if (time > tt) {
    it0 = it1; it1++;
    if (lay == 8 && it1 == L8.end()) return &(it0->second);
    if (lay == 9 && it1 == L9.end()) return &(it0->second);
  }

  int tt1 = (it0->first+it1->first)/2;
  int tt0 = tt1-(it1->first-it0->first);
  int dt0 =   time-tt0;
  int dt1 = -(time-tt1);
  if (dt0 < 0 || dt1 < 0) return 0;

  static TrExtAlignPar eapar;
#pragma omp threadprivate (eapar)
  eapar = (it0->second*dt1+it1->second*dt0)/(dt0+dt1);

  return &eapar;
}

TrExtAlignPar &TrExtAlignDB::GetM(int lay, uint time){
  static TrExtAlignPar dummy;
  dummy.Init(); 
  if (lay==1 ||lay==8) 
    return L8[time];
  else if  (lay==9) 
    return L9[time];
  else    
    return dummy;
}

TrExtAlignPar &TrExtAlignDB::FindM(int lay, uint time)
{
  map<uint,TrExtAlignPar>::iterator lit;
  static TrExtAlignPar dummy;
  dummy.Init(); 

  if (lay == 8||lay==1){
    lit=L8.lower_bound(time);
    if(lit!=L8.begin() && lit!=L8.end())
      dummy=lit->second;
  }

  if (lay == 9){
    lit=L9.lower_bound(time);
    if(lit!=L9.begin() && lit!=L9.end())
      dummy=lit->second;
  }

  return dummy;
}

void TrExtAlignDB::AddM(int lay, uint time, Double_t *par)
{
  TrExtAlignPar &apar = GetM(lay, time);
  for (int i = 0; i < 6; i++) {
    if (i < 3) apar.dpos  [i]   += par[i];
    else       apar.angles[i-3] += par[i];
  }
}

std::vector<uint> TrExtAlignDB::GetVt(int lay, uint tmin, uint tmax) const
{
  std::vector<uint> vt;

  const std::map<uint, TrExtAlignPar> &tmap = (lay == 8) ? L8 : L9;

  ealgITc it = tmap.begin();  
  for (; it != tmap.end(); it++) {
    if (tmin <= it->first && it->first < tmax)
      vt.push_back(it->first);
  }

  return vt;
}

uint TrExtAlignDB::GetTmin(int lay) const
{
  const std::map<uint, TrExtAlignPar> &tmap = (lay == 8) ? L8 : L9;
  ealgITc it = tmap.begin();
  return it->first;
}

uint TrExtAlignDB::GetTmax(int lay) const
{
  const std::map<uint, TrExtAlignPar> &tmap = (lay == 8) ? L8 : L9;
  ealgITc it = tmap.end(); it--;
  return it->first;
}

int TrExtAlignDB::OverLoadFlag = 1;

void TrExtAlignDB::Load(TFile * ff){
  if(!ff) return;
  if (Head && !OverLoadFlag) {
    cout << "TrExtAlignDB::Load-W- TrExtAlignDB already exists, NOT loaded"
	 << endl;
    return;
  }

  if (Head) delete Head;
  Head = (TrExtAlignDB*) ff->Get("TrExtAlignDB");
  if(!Head)
    cout << "TrExtAlignDB::Load-W- Cannot Load the external Layer AlignmentDB"
	 << endl;
  else {
    Head->Check();
  }
  return;
}

Bool_t TrExtAlignDB::Load(TFile *f8, TFile *f9)
{
  TrExtAlignDB *db8 = (TrExtAlignDB *)f8->Get("TrExtAlignDB");
  TrExtAlignDB *db9 = (TrExtAlignDB *)f9->Get("TrExtAlignDB");
  if (!db8 || !db9) return kFALSE;

  vector<uint> v8 = db8->GetVt(8);
  vector<uint> v9 = db9->GetVt(9);

  Head = new TrExtAlignDB;

  for (unsigned int i = 0; i < v8.size(); i++)
    Head->Fill(8, v8.at(i), db8->GetM(8, v8.at(i)));
  for (unsigned int i = 0; i < v9.size(); i++)
    Head->Fill(9, v9.at(i), db9->GetM(9, v9.at(i)));

  return kTRUE;
}

int TrExtAlignDB::Check(void)
{
  // This check should be called only when number of entries are too many
  if (L8.size() <= 500 || L9.size() <= 500) return 0;

  unsigned int rmin = 1300000000;
  int ndel = 0;

  for (ealgIT it = L8.begin(); it != L8.end(); )
    if (it->first < rmin) { L8.erase(it++); ndel++; } else it++;

  for (ealgIT it = L9.begin(); it != L9.end(); )
    if (it->first < rmin) { L9.erase(it++); ndel++; } else it++;

  if (ndel > 0)
    cout << "TrExtAlignDB::Load-I- Removed wrong entries: " << ndel << endl;

  return ndel;
}

Bool_t  TrExtAlignDB::Load(const char *fname, const char *dname)
{
  TFile f(fname);
  if (!f.IsOpen()) return kFALSE;
  if (dname) Load((TFile *)f.Get(dname));
  else Load(&f);
  cout << "TrExtAlignDB::Load-I- Loaded from: " << fname;
  if (dname) cout << "/" << dname;
  cout << " Size= " << Head->GetSize(8) << " " << Head->GetSize(9) << endl;
  return kTRUE;
}


int TrExtAlignDB::UpdateTkDBcDyn(int plane){
#ifdef __ROOTSHAREDLIBRARY__
  if(!AMSEventR::Head()){
    std::cerr << "AMSEventR::Head is null" << std::endl;
    return -1;
  }
  return UpdateTkDBcDyn(AMSEventR::Head()->fHeader.Run,AMSEventR::Head()->fHeader.Time[0],plane);
#endif
return -5;
}


int  TrExtAlignDB::UpdateTkDBcDyn(int run,uint time, int pln,int lad1,int lad9){
  if(DynAlManager::ignoreAlignment) return 0;
  if(!DynAlManager::Available()) return 0;   // If MC it is not available 

  if (!TkDBc::Head) {
    std::cerr << "TkDBc::Head is null" << std::endl;
    return -2;
  }

  
  static int prevTimeWithError=-1;
#pragma omp threadprivate(prevTimeWithError)

#define WALLTIME (60*5)
  int testTime=run>0?run:time;  // Use the run number as test if available, else the event time
  if(run>0 && testTime==prevTimeWithError) return 0;
  if(run<=0 && testTime-prevTimeWithError<WALLTIME) return 0; // Ensure that we are at least WALLTIME seconds away of problems 
  if(testTime<prevTimeWithError) prevTimeWithError=-1; // Going backwards!!!!! reset

  // Update the alignment
  if(!DynAlManager::UpdateParameters(run,time)){
    // Inform of the error for the first time
    if(run>0) cout<<"TrExtAlignDB::UpdateTkDBcDyn-W- Warning no dynamic alignment available for run "<<run<<". Waiting for next run."<<endl;
    else cout<<"TrExtAlignDB::UpdateTkDBcDyn-W- Warning no dynamic alignment available for time "<<time<<". Waiting "<<WALLTIME<<" seconds of data to retry retrieving the alignment parameters."<<endl;
    prevTimeWithError=testTime;
    SL1[6]=SL1[7]=SL1[8]=SL1[9]=SL1[10]=SL1[11]=0.;
    SL9[6]=SL9[7]=SL9[8]=SL9[9]=SL9[10]=SL9[11]=0.;
    return 0;
  }
#undef WALLTIME

  // Retrieve the parameters
  int layerJ[2]={1,9};
  int plane[2]={5,6};
  bool skip[2]={false,false};
  if(lad1!=-1 || lad9!=-1){
    skip[0]=(lad1==-1);
    skip[1]=(lad9==-1);
  }
  for(int i=0;i<2;i++){
    if(skip[i]) continue;
    if(!(pln & (1<<i)))continue;
    TkPlane* pl = TkDBc::Head->GetPlane(plane[i]);
    if (!pl) return i==0?-3:-13;
    
    // Retrieve alignment parameters
    DynAlFitParameters pars;
    if(!DynAlManager::dynAlFitContainers[layerJ[i]].Find(time,pars)) return i==0?-4:-14;
    AMSPoint pos;
    AMSRotMat rot;
    pars.GetParameters(time,0,pos,rot);
    double offset=pars.ZOffset;

    // Apply local alignment
    int ladderId=layerJ[i]==1?lad1:lad9;

    if(ladderId!=-1){
      // Apply local alignment for the hit: modify the rotation matrix
      map<Int_t,DynAlFitParameters>::iterator it=DynAlManager::dynAlFitContainers[layerJ[i]].LocalFitParameters.find(ladderId);
      if(it!=DynAlManager::dynAlFitContainers[layerJ[i]].LocalFitParameters.end()){
	AMSPoint lpos;
	AMSRotMat lrot;
	it->second.GetParameters(0,0,lpos,lrot);
	
	// Compute the new delta
	AMSPoint newDelta=pos+
	  rot*(lpos+AMSPoint(0,0,it->second.ZOffset-pars.ZOffset))
	  -AMSPoint(0,0,it->second.ZOffset-pars.ZOffset);
	double newOffset=it->second.ZOffset;
	AMSRotMat newRot=rot*lrot;
	
	// Copy back
	pos=newDelta;
	offset=newOffset;
	rot=newRot;
      }
    }
    // Take into account the difference in the reference frames
    // between the local geometry and the one used in the computation
    AMSPoint o(0,0,offset);
    pos=pos-pl->GetPos()+o-rot*(o-pl->GetPos()); 

    // Set plane parameters
//     pl->UpdatePosA()=pos;
//     pl->UpdateRotA()=rot;

    double a,b,c;
    rot.GetRotAngles(a,b,c);
    float *ll= (plane[i]==5)?SL1:SL9;
    ll[6]=pos[0]; ll[7]=pos[1];ll[8]=pos[2];
    ll[9]=a;ll[10]=b;ll[11]=c;


    // Simple Hardwire Database
    if(TkLadder::version >= 4 && 
       strcmp("DynAlignmentV5T290713PM5",DynAlManager::tdvdb->getname())==0){
     
#ifdef _CUSTOM_SOFS2_
      if(run>0){
	Sofs2[0+2]=3.7e-4;
	Sofs2[0+6]=51e-4;
	Sofs2[0+10]=0;
	Sofs2[1+2]=6.0e-4;
	Sofs2[1+6]=20e-4;
	Sofs2[1+10]=0;
      }


      if(run>1386071385-30){
	static bool first=true;
	if(first){
	  cout<<"INITIALIZING SOFS2 CORRECTION"<<endl;
	  first=false;
	}
	Sofs2[0+2]=0;
	Sofs2[0+6]=20e-4;
	Sofs2[0+10]=0; 
	Sofs2[1+2]=1.5e-4;
	Sofs2[1+6]=60e-4;
	Sofs2[1+10]=0;
      }
#endif
    }



    if (TkLadder::version == 3) ll[7]+=Sofs[i+2];
    if (TkLadder::version >= 4){ll[7]+=Sofs2[i+2]; ll[8]+=Sofs2[i+6]; ll[9]+=Sofs2[i+10];}
  }
  return 0;
}

TrExtAlignPar & TrExtAlignDB::GetMDyn(int layerJ,uint time){
  static TrExtAlignPar Pars;
#pragma omp threadprivate(Pars)

  Pars.Init();

  
  if(layerJ!=1 && layerJ!=9) return Pars;
  // Retrieve alignment parameters
  if(!DynAlManager::UpdateParameters(0,time)) return Pars;
  DynAlFitParameters pars;
  if(!DynAlManager::dynAlFitContainers[layerJ].Find(time,pars)) return Pars;
  AMSPoint pos;
  AMSRotMat rot;
  pars.GetParameters(time,0,pos,rot);
  double offset=pars.ZOffset;

  
  // Take into account the difference in the reference frames
  // between the local geometry and the one used in the computation
  TkPlane* pl = TkDBc::Head->GetPlane(layerJ==1?5:6);
  if (!pl) return Pars;
  AMSPoint o(0,0,offset);
  pos=pos-pl->GetPos()+o-rot*(o-pl->GetPos()); 
  
  double alpha,beta,gamma;
  rot.GetRotAngles(alpha,beta,gamma);
  Pars.SetPar(pos[0],pos[1],pos[2],alpha,beta,gamma);

  // Retrieve the errors (not all of them, but the most relevant since covariance matrix is not taken into account)
  double ex,ey,ez;
  DynAlManager::RetrieveAlignmentErrors(time,layerJ,ex,ey,ez);
  Pars.SetErr(ex,ey,ez,0,0,0);

  return Pars;
}

int TrExtAlignDB::RecalcAllExtHitCoo(int kind){
  int alkind=0;
  if (kind<0 ||kind>2) return -1;

  if (kind==1) alkind=1;
  TrExtAlignDB::SetAlKind(alkind);

  int rret=0;

  switch(kind){
  case 0:  rret=UpdateExtLayer(0); break;//PG
  case 1:  rret=UpdateExtLayer(1); break;//CIEMAT
  default: ResetExtAlign();
  }
  if(rret!=0) return -2;

  
  VCon* cont2=GetVCon()->GetCont("AMSTrRecHit");
  if (!cont2) return -1;
  for (int ii=0;ii<cont2->getnelem();ii++){
    TrRecHitR* hit=(TrRecHitR*)cont2->getelem(ii);
    if(hit->GetLayer()>7){
      if(kind==1){
	TrRecHitR*   hit1=(hit->GetLayer()==8)?hit:0;
	TrRecHitR*   hit9=(hit->GetLayer()==9)?hit:0;
	int l1=!hit1?-1:1+hit1->GetSlotSide()*10+hit1->lad()*100;
	int l9=!hit9?-1:9+hit9->GetSlotSide()*10+hit9->lad()*100;
	rret=UpdateExtLayer(1,l1,l9);
	if(rret) {if(cont2)delete cont2;return -3;}
      }
      hit->BuildCoordinate();
    }
  }
  if(cont2) delete cont2;

  return 0;
}


void TrExtAlignDB::ResetExtAlign() 
{

  memset(SL1,0,18*sizeof(SL1[0]));
  memset(SL9,0,18*sizeof(SL9[0]));
//   for (int layer = 8; layer <= 9; layer++) {
//     int plane = (layer == 8) ? 5 : 6;
//     TkPlane* pl = TkDBc::Head->GetPlane(plane);
//     if (!pl) continue;

//     pl->UpdatePosA().setp(0,0,0);
//     pl->UpdateRotA().SetRotAngles(0,0,0);
//   }


//   for (int layer = 8; layer <= 9; layer++) {
//     int plane = (layer == 8) ? 5 : 6;
//     TkPlane* pl = TkDBc::Head->GetPlane(plane);
//     if (!pl) continue;

//     pl->UpdatePosA().setp(0,0,0);
//     pl->UpdateRotA().SetRotAngles(0,0,0);
//   }
  return;
}

int MCscat (AMSPoint &coo, int layj, double prob, double scat, double pwr);
int MCscatq2 (AMSPoint &coo, int layj, float b, float prob);
int DropExtHits(void);

extern "C" double rnormx();

void TrExtAlignDB::SmearExtAlign()
{
  double rnd[10];
  double rd[4];
#ifdef __ROOTSHAREDLIBRARY__
  AMSEventR::GetRandArray(4173792, 2, 10, rnd);
  AMSEventR::GetRandArray(3375420, 1, 4, rd);
#else
  for (int i = 0; i < 10; i++) rnd[i] = rnormx();
   float d(0); 
  rd[0]=RNDM(d);
  rd[1]=RNDM(d);
  rd[2]=RNDM(d);
  rd[3]=RNDM(d);
#endif

double s1[2]={7.5e-4,8.3e-4};

s1[0]=fabs(TRMCFFKEY.OuterSmearing[0][1]);
s1[1]=fabs(TRMCFFKEY.OuterSmearing[1][1]);

double s2[2]={13e-4,14e-4};
double s3[2]={33e-4,33e-4};
double r[2][3]={{1.,0.12/2.,0.01/2},{1.,0.12/2.,0.003/2.}};
for(int l=0;l<2;l++){
double sum=0;
for(int k=0;k<3;k++)sum+=r[l][k];
for(int k=0;k<3;k++)r[l][k]/=sum;
}

double md[2]={0,0};
double pg[2]={0,0};


for(int k=0;k<2;k++){
{
double r1=rd[0+2*k]; 
if(r1<r[k][0]){
 md[k]+=s1[k];
}
else if(r1<r[k][0]+r[k][1]){
 md[k]+=s2[k];
 }
else md[k]+=s3[k];
}
{
double r1=rd[1+2*k];
if(r1<r[k][0]){
 pg[k]=s1[k];
}
else if(r1<r[k][0]+r[k][1]){
 pg[k]=s2[k];
 }
else pg[k]=s3[k];


}
}

  ResetExtAlign();
  // PG
  SL1[0] = rnd[0]*TRMCFFKEY.OuterSmearing[0][0];
  if(TRMCFFKEY.OuterSmearing[0][1]>=0)SL1[1] = rnd[1]*TRMCFFKEY.OuterSmearing[0][1];
  else {
     SL1[1] = rnd[1]*pg[0]+rnd[8]*(pg[0]+md[0])/2*TRMCFFKEY.OuterSmearingC[0][1];
     }
  SL9[0] = rnd[2]*TRMCFFKEY.OuterSmearing[1][0];
  if(TRMCFFKEY.OuterSmearing[1][1]>=0)SL9[1] = rnd[3]*TRMCFFKEY.OuterSmearing[1][1];
  else {
   SL9[1] = rnd[3]*pg[1]+rnd[9]*(pg[1]+md[1])/2*TRMCFFKEY.OuterSmearingC[1][1];
  }
  // CIEMAT
  SL1[6] = rnd[4]*TRMCFFKEY.OuterSmearing[0][0];
  if(TRMCFFKEY.OuterSmearing[0][1]>=0)SL1[7] = rnd[5]*TRMCFFKEY.OuterSmearing[0][1];
  else {
    SL1[7] = rnd[5]*md[0]+rnd[8]*(pg[0]+md[0])/2*TRMCFFKEY.OuterSmearingC[0][1];
  }
  SL9[6] = rnd[6]*TRMCFFKEY.OuterSmearing[1][0];
  if(TRMCFFKEY.OuterSmearing[1][1]>=0)SL9[7] = rnd[7]*TRMCFFKEY.OuterSmearing[1][1];
  else SL9[7] = rnd[7]*md[1]+rnd[9]*(pg[1]+md[1])/2*TRMCFFKEY.OuterSmearingC[1][1];

#ifdef __ROOTSHAREDLIBRARY__
  // Workaround to retune the MC scatterng
  if (TRMCFFKEY.MCscat[0] > 0) {
    AMSPoint coo1, coo9;
    MCscat(coo1, 1, TRMCFFKEY.MCscat[0], TRMCFFKEY.MCscat[1],
	                                 TRMCFFKEY.MCscat[2]);
    MCscat(coo9, 9, TRMCFFKEY.MCscat[0], TRMCFFKEY.MCscat[1],
	                                 TRMCFFKEY.MCscat[2]);

    SL1[0] += coo1.x(); SL1[1] += coo1.y();
    SL9[0] += coo9.x(); SL9[1] += coo9.y();
    SL1[6] += coo1.x(); SL1[7] += coo1.y();
    SL9[6] += coo9.x(); SL9[7] += coo9.y();

    int nd=DropExtHits();
    static int np=0;
    if(nd && np++<100)cout<<" TrExtAlignDB::SmearExtAlig-I-DropExtHits "<<nd<<endl;
    else if(nd && np==100)cout<<" TrExtAlignDB::SmearExtAlig-W-DropExtHitsLastmessage "<<endl;
  
  }
  else if(TRMCFFKEY.MCscat[0] < 0) {  // alt method using q^2
    AMSPoint coo1, coo9;
    MCscatq2(coo1, 1, TRMCFFKEY.MCscat[0], TRMCFFKEY.MCscat[1]);
    MCscatq2(coo9, 9, TRMCFFKEY.MCscat[0], TRMCFFKEY.MCscat[2]);
    SL1[0] += coo1.x(); SL1[1] += coo1.y();
    SL9[0] += coo9.x(); SL9[1] += coo9.y();
    SL1[6] += coo1.x(); SL1[7] += coo1.y();
    SL9[6] += coo9.x(); SL9[7] += coo9.y();

    int nd=DropExtHits();
    static int np=0;
    if(nd && np++<100)cout<<" TrExtAlignDB::SmearExtAligQ2-I-DropExtHits "<<nd<<endl;
    else if(nd && np==100)cout<<" TrExtAlignDB::SmearExtAligQ2-W-DropExtHitsLastmessage "<<endl;
  }
  #endif
}

int  TrExtAlignDB::UpdateTkDBc(uint time) const
{
  if (!TkDBc::Head) {
    std::cerr << "TkDBc::Head is null" << std::endl;
    return -1;
  }

#ifdef __ROOTSHAREDLIBRARY__
  // Deal with MC properly
  if(AMSEventR::Head() && AMSEventR::Head()->nMCEventg()) return 0;


// PZ -- FIXME -- TO be removed and replaced by TDV entry with years validity and zero content
  static int nprint=0;

  int mar=TKGEOMFFKEY.MaxAlignedRun; //MAXALIGNEDRUN;
  if(int(time) > TKGEOMFFKEY.MaxAlignedRun){ //MAXALIGNEDRUN
    if(nprint++<10) printf("TrExtAlignDB::UpdateTkDBc-W- Warning no dyn alignment available after %d, this message will be repeted only 10 times \n",mar);
    SL1[0]=SL1[1]=SL1[2]=SL1[3]=SL1[4]=SL1[5]=0.;
    SL9[0]=SL9[1]=SL9[2]=SL9[3]=SL9[4]=SL9[5]=0.;
    return 0;
  }
#endif
  int errlim=100;
  static int nwar = 0;
  static int first=1;
  int out_of_range = 0;
#ifdef __ROOTSHAREDLIBRARY__
#pragma omp threadprivate (first)
  uint ti8 = Find(8, 0)-119;
  uint ti9 = Find(9, 0)-119;

  uint te8 = ti8+50000;  // Effective time range
  uint te9 = ti9+50000;  // Effective time range

  if (time%50000 < 120) time += 120;

  // For default data
  if (GetSize(8) == 1) { ti8 = GetTmin(8); te8 = 2100000000; }
  if (GetSize(9) == 1) { ti9 = GetTmin(9); te9 = 2100000000; }

  if (GetSize(8) == 2) { ti8 = GetTmin(8); te8 = GetTmax(8); }
  if (GetSize(9) == 2) { ti9 = GetTmin(9); te9 = GetTmax(9); }

  static int nor = 0;
  if (time < ti8 || time < ti9 || 
      time > te8 || time > te9) {
    if (nor++ < 20)
      std::cout << "TrExtAlignDB::UpdateTkDBc-W-Time is out of range: "
		<< ti8 << " " << ti9 << " " << time << " "
		<< te8 << " " << te9  << std::endl;
    out_of_range = 1;
  }
#endif

  uint tf8,tf9;
  int dt8,dt9;
  if(!ForceFromTDV){ // Access the data available in memory(from ROOT file) unless forced not to do it
    tf8 = Find(8, time);
    tf9 = Find(9, time);
    dt8 = (int)tf8-(int)time;
    dt9 = (int)tf9-(int)time;

    if (GetSize(8) <= 2 || GetSize(9) <= 2) dt8 = dt9 = 0;
    
    if ((dt8 < -200 || 200 < dt8) && nwar++ < errlim)
      std::cout << "TrExtAlignDB::UpdateTkDBc-W-Time is too far: "
		<< tf8 << " " << time << " " << tf8-time << std::endl;
    if ((dt9 < -200 || 200 < dt9) && nwar++ < errlim)
      std::cout << "TrExtAlignDB::UpdateTkDBc-W-Time is too far: "
		<< tf9 << " " << time << " " << tf9-time << std::endl;
    
#ifdef __ROOTSHAREDLIBRARY__
    if(out_of_range || (dt8 < -200 || 200 < dt8)|| (dt9 < -200 || 200 < dt9)){  // Try to fall back to TDV if data in memory are no good
      if(nwar++ <errlim) printf("TrExtAlignDB::UpdateTkDBc-I- Trying to Access TDV directly\n");
      int ret=-1;
      ret=GetFromTDV(time, version, 1);
      if(ret<=0) {
	if(nwar++ <errlim)printf("TrExtAlignDB::UpdateTkDBc-E- TDV not accessible, I give up\n");
	return -2;
      }
      tf8 = Find(8, time);
      tf9 = Find(9, time);
      dt8 = (int)tf8-(int)time;
      dt9 = (int)tf9-(int)time;

      if (GetSize(8) <= 2 || GetSize(9) <= 2) dt8 = dt9 = 0;

      if ((dt8 < -200 || 200 < dt8) && nwar++ < errlim)
	std::cout << "TrExtAlignDB::UpdateTkDBc-W-Time is too far: "
		  << tf8 << " " << time << " " << tf8-time << std::endl;
      if ((dt9 < -200 || 200 < dt9) && nwar++ < errlim)
	std::cout << "TrExtAlignDB::UpdateTkDBc-W-Time is too far: "
		  << tf9 << " " << time << " " << tf9-time << std::endl;
      if((dt8 < -200 || 200 < dt8)|| (dt9 < -200 || 200 < dt9)){
	if(nwar++ <errlim) printf("TrExtAlignDB::UpdateTkDBc-E- Also TDV is not valid for this event \n");
	return -2;
      }
      if(nwar++ <errlim) printf("TrExtAlignDB::UpdateTkDBc-I- Successuflly Loaded Align info from TDV\n");
    }
#else
    if((dt8 < -200 || 200 < dt8)|| (dt9 < -200 || 200 < dt9))
      return -2;
#endif



  }else{ // FORCED reading from TDV
    if(first || out_of_range) {GetFromTDV(time, version);first=0;}
    tf8 = Find(8, time);
    tf9 = Find(9, time);
    dt8 = (int)tf8-(int)time;
    dt9 = (int)tf9-(int)time;

    if (GetSize(8) <= 2 || GetSize(9) <= 2) dt8 = dt9 = 0;

    int ret=-1;
    if((dt8 < -200 || 200 < dt8)|| (dt9 < -200 || 200 < dt9)){
      ret=GetFromTDV(time, version);

      if(ret<=0) {
	if(nwar++ <errlim)printf("TrExtAlignDB::UpdateTkDBc-E- TDV not accessible, I give up\n");
	return -2;
      }
    
      tf8 = Find(8, time);
      tf9 = Find(9, time);
      dt8 = (int)tf8-(int)time;
      dt9 = (int)tf9-(int)time;
    }

    if ((dt8 < -200 || 200 < dt8) && nwar++ < errlim)
      std::cout << "TrExtAlignDB::UpdateTkDBc-W-Time is too far: "
		<< tf8 << " " << time << " " << tf8-time << std::endl;
    if ((dt9 < -200 || 200 < dt9) && nwar++ < errlim)
      std::cout << "TrExtAlignDB::UpdateTkDBc-W-Time is too far: "
		<< tf9 << " " << time << " " << tf9-time << std::endl;
    if((dt8 < -200 || 200 < dt8)|| (dt9 < -200 || 200 < dt9)){
      if(nwar++ <errlim) printf("TrExtAlignDB::UpdateTkDBc-E-  TDV is not valid for this event \n");
      return -2;
    }
    if(nwar++ <1) printf("TrExtAlignDB::UpdateTkDBc-I- Successuflly Loaded Align info from TDV\n");
  }




  for (int layer = 8; layer <= 9; layer++) {
    int plane = (layer == 8) ? 5 : 6;
    TkPlane* pl = TkDBc::Head->GetPlane(plane);
    if (!pl) return -3 ;

    const TrExtAlignPar *par = Get(layer, time);
    if (!par && GetSize(layer) == 1) {
      const std::map<uint, TrExtAlignPar> &tmap = (layer == 8) ? L8 : L9;
      par = &(tmap.begin()->second);
    }

    if(!par) return -4;

    float * ll= (layer==8)?SL1:SL9;

    ll[0]=par->dpos[0];  ll[1]=par->dpos[1];   ll[2]=par->dpos[2];
    ll[3]=par->angles[0];ll[4]=par->angles[1]; ll[5]=par->angles[2];

    if (TkLadder::version == 3) ll[1]+=(layer==8)?Sofs[0]:Sofs[1];
    if (TkLadder::version >= 4){ll[1]+=(layer==8)?Sofs2[0]:Sofs2[1];
                                ll[2]+=(layer==8)?Sofs2[4]:Sofs2[5];
                                ll[3]+=(layer==8)?Sofs2[8]:Sofs2[9];}
  }

  return 0;
}

void TrExtAlignDB::Print(Option_t *) const
{
  for (int lay = 8; lay <= 9; lay++) {
    const std::map<uint, TrExtAlignPar> &tmap = (lay == 8) ? L8 : L9;

    ealgITc it = tmap.begin();
    for (; it != tmap.end(); it++) {
      cout << "L" << lay << " " << it->first << " ";
      it->second.Print();
    }
  }
}


float *TrExtAlignDB::fLinear = 0;

void TrExtAlignDB::ExAlign2Lin()
{
  if (!fLinear) {
    CreateLinear();
    cout << "TrExtAlignDB::ExAlingn2Lin()-I-linear space is created" << endl;
  }

  int npar = GetNpar();
  int nlin = GetLinearSize()/sizeof(float);
  int ntim = nlin/2/npar;
  for (int i = 0; i < nlin; i++) fLinear[i] = 0;

  uint *uptr = (uint *)fLinear;

  for (int i = 0; i < 2; i++) {
    int layer = i+8;
    std::vector<uint> vt = GetVt(layer);

    int n = 0;
    for (unsigned int j = 0; j < vt.size(); j++) {
      uint time = vt.at(j);
      if (time == 0) continue;

      TrExtAlignPar par = GetM(layer, time);
      uptr   [(i*ntim+n)*npar]   = time;
      fLinear[(i*ntim+n)*npar+1] = par.dpos  [0];
      fLinear[(i*ntim+n)*npar+2] = par.dpos  [1];
      fLinear[(i*ntim+n)*npar+3] = par.dpos  [2];
      fLinear[(i*ntim+n)*npar+4] = par.angles[0];
      fLinear[(i*ntim+n)*npar+5] = par.angles[1];
      fLinear[(i*ntim+n)*npar+6] = par.angles[2];

      if (npar > 12) {
	fLinear[(i*ntim+n)*npar+ 7] = par.edpos  [0];
	fLinear[(i*ntim+n)*npar+ 8] = par.edpos  [1];
	fLinear[(i*ntim+n)*npar+ 9] = par.edpos  [2];
	fLinear[(i*ntim+n)*npar+10] = par.eangles[0];
	fLinear[(i*ntim+n)*npar+11] = par.eangles[1];
	fLinear[(i*ntim+n)*npar+12] = par.eangles[2];
      }
      if (npar > 14) {
	fLinear[(i*ntim+n)*npar+13] = par.chisq;
	fLinear[(i*ntim+n)*npar+14] = par.NDF;
      }

      n++;
    }

    cout << "size" << layer << "= " << vt.size() << " " << n << endl;
  }
}


void TrExtAlignDB::Lin2ExAlign()
{
  if (!fLinear) {
    cerr << "TrExtAlignDB::Lin2ExAlingn()-E-linear doesn't exist" << endl;
    return;
  }

  int npar = GetNpar();
  int nlin = GetLinearSize()/4;
  int ntim = nlin/2/npar;

  uint *uptr = (uint *)fLinear;

  Clear();

  for (int i = 0; i < 2; i++) {
    int layer = i+8;

    int n = 0;
    for (int j = 0; j < ntim; j++) {
      uint time = uptr[(i*ntim+j)*npar];
      if (time == 0) continue;

      TrExtAlignPar &par = GetM(layer, time);
      par.dpos  [0] = fLinear[(i*ntim+j)*npar+1];
      par.dpos  [1] = fLinear[(i*ntim+j)*npar+2];
      par.dpos  [2] = fLinear[(i*ntim+j)*npar+3];
      par.angles[0] = fLinear[(i*ntim+j)*npar+4];
      par.angles[1] = fLinear[(i*ntim+j)*npar+5];
      par.angles[2] = fLinear[(i*ntim+j)*npar+6];

      if (npar > 12) {
	par.edpos  [0] = fLinear[(i*ntim+n)*npar+ 7];
	par.edpos  [1] = fLinear[(i*ntim+n)*npar+ 8];
	par.edpos  [2] = fLinear[(i*ntim+n)*npar+ 9];
	par.eangles[0] = fLinear[(i*ntim+n)*npar+10];
	par.eangles[1] = fLinear[(i*ntim+n)*npar+11];
	par.eangles[2] = fLinear[(i*ntim+n)*npar+12];
      }
      if (npar > 14) {
	par.chisq = fLinear[(i*ntim+n)*npar+13];
	par.NDF   = fLinear[(i*ntim+n)*npar+14];
      }
      n++;
    }

    cout << "size" << layer << "= " << n << " " << GetSize(layer) << " "
	 << "time= " << GetTmin(layer) << " " << GetTmax(layer) << endl;
  }
}

void SLin2ExAlign()
{
  TrExtAlignDB::GetHead()->Lin2ExAlign();
}




void TrExtAlignDB::ProduceDisalignment(unsigned int * tt){
  //                     min  sec/min
  double period=2*M_PI/ (92.*60.);
  double time=tt[0];
  time+=(tt[1]*0.000000001L);
  TrExtAlignPar par[2];
  float p1[6],p9[6];
  float phase1[6],phase9[6];
  for (int ii=0;ii<6;ii++){
    p1[ii]     = TKGEOMFFKEY.L1Amp[ii];
    phase1[ii] = TKGEOMFFKEY.L1Phase[ii];
    p9[ii]     = TKGEOMFFKEY.L9Amp[ii];
    phase9[ii] = TKGEOMFFKEY.L9Phase[ii];
  }
  //par[0].dpos[1]=p1[1];
  par[0].dpos[0]=p1[0]*sin(time*period+phase1[0]);
  par[0].dpos[1]=p1[1]*sin(time*period+phase1[1]);
  par[0].dpos[2]=p1[2]*sin(time*period+phase1[2]);
  par[0].angles[0]=p1[3]*sin(time*period+phase1[3]);
  par[0].angles[1]=p1[4]*sin(time*period+phase1[4]);
  par[0].angles[2]=p1[5]*sin(time*period+phase1[5]);
//   Printf("Time: %d %d\n",tt[0],tt[1]);
//   par[0].Print();

  par[1].dpos[0]=p9[0]*sin(time*period+phase9[0]);
  par[1].dpos[1]=p9[1]*sin(time*period+phase9[1]);
  par[1].dpos[2]=p9[2]*sin(time*period+phase9[2]);
  par[1].angles[0]=p9[3]*sin(time*period+phase9[3]);
  par[1].angles[1]=p9[4]*sin(time*period+phase9[4]);
  par[1].angles[2]=p9[5]*sin(time*period+phase9[5]);
  
  for (int layer = 8; layer <= 9; layer++) {
    int plane = (layer == 8) ? 5 : 6;
    TkPlane* pl = TkDBc::Head->GetPlane(plane);
    if (!pl) continue;
    
    TrExtAlignPar &ppar=par[(layer==8)?0:1];
    pl->UpdatePosT().setp(ppar.dpos[0], ppar.dpos[1], ppar.dpos[2]);
    pl->UpdateRotT().SetRotAngles(ppar.angles[0], ppar.angles[1], ppar.angles[2]);
  }

  return;
}
long long TrExtAlignDB::GetDt(float rate){
  int bb=1;
  float x=RNDM(bb);
  if(rate<=0||x==1.) return 0;
  double aa= -1/rate*log(1-x);
  long long out=aa*1E6;
  return out;
}

const char *TrExtAlignDB::GetTDVName()
{
  static TString stn;
  stn = "TrackerExtAlign";
  if (version >= 2) stn += Form("%d", version);
  return stn.Data();
}

int TrExtAlignDB::GetFromTDV(uint time, int ver, bool force)
{
  time_t tt=time;
  static AMSTimeID* db=0;
#pragma omp threadprivate(db)
  if (db && force) {
    delete db;
    db = 0;
  }

  if(!db) {
    tm begin;
    tm end;
  
    begin.tm_isdst=0;
    end.tm_isdst=0;    
    begin.tm_sec  =0;
    begin.tm_min  =0;
    begin.tm_hour =0;
    begin.tm_mday =0;
    begin.tm_mon  =0;
    begin.tm_year =0;
      
    end.tm_sec=0;
    end.tm_min=0;
    end.tm_hour=0;
    end.tm_mday=0;
    end.tm_mon=0;
    end.tm_year=0;

    version = ver;

    TrExtAlignDB::CreateLinear();

    db=new AMSTimeID(AMSID(GetTDVName(),1),begin,end,
		     TrExtAlignDB::GetLinearSize(),
		     TrExtAlignDB::fLinear,
		     AMSTimeID::Standalone,1,SLin2ExAlign);
  }
  int ret=db->validate(tt);

  return ret;
}

int TrExtAlignDB::UpdateTDV(uint brun, uint erun, int ver)
{
  version = ver;
  CreateLinear();

  cout << "Updating " << GetTDVName() << endl;
 
  TrExtAlignDB::GetHead()->ExAlign2Lin();

  time_t br = brun+3600;
  time_t er = erun+3600;

  tm begin; gmtime_r(&br, &begin);
  tm end;   gmtime_r(&er, &end);
  cout << "Begin: " <<(int)brun<<"  " <<asctime(&begin)<<endl;
  cout << "End  : " <<(int)erun<<"  " <<asctime(&end  )<<endl;

  AMSTimeID *tt = new AMSTimeID(AMSID(GetTDVName(), 1), begin, end,
				TrExtAlignDB::GetLinearSize(),
				TrExtAlignDB::fLinear,
				AMSTimeID::Standalone, 1);
  tt->UpdateMe();
  return tt->write(AMSDATADIR.amsdatabase);
}

AMSPoint  TrExtAlignDB::GetAlDist(TrRecHitR *hit){
#ifndef MAXFLOAT
#define MAXFLOAT 1.0e30
#endif
  if(!hit) return AMSPoint(MAXFLOAT,MAXFLOAT,MAXFLOAT);
  if(hit->GetLayerJ()!=1 && hit->GetLayer()!=9) return AMSPoint(0,0,0);

  TrExtAlignDB::SetAlKind(1);
  int l1=hit->GetLayerJ()==1?1+hit->GetSlotSide()*10+hit->lad()*100:-1;
  int l9=hit->GetLayerJ()==9?1+hit->GetSlotSide()*10+hit->lad()*100:-1;
  if(UpdateExtLayer(1,l1,l9)) return AMSPoint(MAXFLOAT,MAXFLOAT,MAXFLOAT);
  AMSPoint MDcoo=hit->GetGlobalCoordinate(hit->GetResolvedMultiplicity());

  TrExtAlignDB::SetAlKind(0);
  if(UpdateExtLayer(0)) return AMSPoint(MAXFLOAT,MAXFLOAT,MAXFLOAT);
  AMSPoint PGcoo=hit->GetGlobalCoordinate(hit->GetResolvedMultiplicity());

  AMSPoint dist=MDcoo-PGcoo;
  return dist;
}
