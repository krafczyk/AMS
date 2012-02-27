#include "TrExtAlignDB.h"
#include "TkDBc.h"
#include "TFile.h"
#include "random.h"
#include "tkdcards.h"
#include "timeid.h"

ClassImp(TrExtAlignPar);
ClassImp(TrExtAlignDB);

using namespace  std;
TrExtAlignDB* TrExtAlignDB::Head=0;
int TrExtAlignDB::ForceFromTDV=0;

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
  ealgITc it;
  if (lay == 8 && (it = L8.lower_bound(time)) != L8.end()) return &(it->second);
  if (lay == 9 && (it = L9.lower_bound(time)) != L9.end()) return &(it->second);

  return 0;
//  static TrExtAlignPar dummy;
//  return dummy;
}

TrExtAlignPar &TrExtAlignDB::GetM(int lay, uint time)
{
  if (lay == 8) return L8[time];
  if (lay == 9) return L9[time];

  static TrExtAlignPar dummy;
  dummy.Init(); 
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

std::vector<uint> TrExtAlignDB::GetVt(int lay, uint tmin, uint tmax)
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
  else
    cout << "TrExtAlignDB::Load-I- Loaded from: " << ff->GetName()
	 << " Size= " << Head->GetSize(8) << " " << Head->GetSize(9) << endl;
  return;
}

Bool_t  TrExtAlignDB::Load(const char *fname)
{
  TFile f(fname);
  if (!f.IsOpen()) return kFALSE;
  Load(&f);
  return kTRUE;
}


#include "DynAlignment.h"
#ifdef __ROOTSHAREDLIBRARY__
#include "root.h"
#endif
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


int  TrExtAlignDB::UpdateTkDBcDyn(int run,uint time, int pln){
  if(DynAlManager::ignoreAlignment) return 0;

  if (!TkDBc::Head) {
    std::cerr << "TkDBc::Head is null" << std::endl;
    return -2;
  }

  // Update the alignment
  if(!DynAlManager::UpdateParameters(run,time)){
    // Set the alignment to zero: useful to create the alignment
    int plane[2]={5,6};
    for(int i=0;i<2;i++){
      if(!(pln & (1<<i)))continue;
      TkPlane* pl = TkDBc::Head->GetPlane(plane[i]);
      if (!pl) return i==0?-3:-13;
      pl->posA.setp(0,0,0);
      pl->rotA.Reset();
    }
    return 1;
  }
  
  // Retrieve the parameters
  int layerJ[2]={1,9};
  int plane[2]={5,6};
  int layer[2]={8,9};
  for(int i=0;i<2;i++){
      if(!(pln & (1<<i)))continue;
    TkPlane* pl = TkDBc::Head->GetPlane(plane[i]);
    if (!pl) return i==0?-3:-13;
    
    // Retrieve alignment parameters
    DynAlFitParameters pars;
    if(!DynAlManager::dynAlFitContainers[layerJ[i]].Find(time,pars)) return i==0?-4:-14;
    AMSPoint pos;
    AMSRotMat rot;
    pars.GetParameters(time,0,pos,rot);

    // Take into account the difference in the reference frames
    // between the local geometry and the one used in the computation
    double delta=pl->GetPos()[2]-pars.ZOffset;
    AMSPoint correction(rot.GetEl(0,2)*delta,rot.GetEl(1,2)*delta,0);
    pos=pos+correction;

    // Set plane parameters
    pl->posA=pos;
    pl->rotA=rot;
  }
  return 0;
}


void TrExtAlignDB::ResetExtAlign() 
{

  for (int layer = 8; layer <= 9; layer++) {
    int plane = (layer == 8) ? 5 : 6;
    TkPlane* pl = TkDBc::Head->GetPlane(plane);
    if (!pl) continue;

    pl->posA.setp(0,0,0);
    pl->rotA.SetRotAngles(0,0,0);
  }
  return;
}

int  TrExtAlignDB::UpdateTkDBc(uint time) const
{
  if (!TkDBc::Head) {
    std::cerr << "TkDBc::Head is null" << std::endl;
    return -1;
  }
  int errlim=100;
  static int nwar = 0;
  static int first=0;
  uint tf8,tf9;
  int dt8,dt9;
  if(!ForceFromTDV){ // Access the data available in memory(from ROOT file) unless forced not to do it
    tf8 = Find(8, time);
    tf9 = Find(9, time);
    dt8 = (int)tf8-(int)time;
    dt9 = (int)tf9-(int)time;
    
    if ((dt8 < -200 || 200 < dt8) && nwar++ < errlim)
      std::cout << "TrExtAlignDB::UpdateTkDBc-W-Time is too far: "
		<< tf8 << " " << time << " " << tf8-time << std::endl;
    if ((dt9 < -200 || 200 < dt9) && nwar++ < errlim)
      std::cout << "TrExtAlignDB::UpdateTkDBc-W-Time is too far: "
		<< tf9 << " " << time << " " << tf9-time << std::endl;
    
#ifdef __ROOTSHAREDLIBRARY__
    if((dt8 < -200 || 200 < dt8)|| (dt9 < -200 || 200 < dt9)){  // Try to fall back to TDV if data in memory are no good
      if(nwar++ <errlim) printf("TrExtAlignDB::UpdateTkDBc-I- Trying to Access TDV directly\n");
      int ret=-1;
      ret=GetFromTDV(time);
      if(ret<=0) {
	if(nwar++ <errlim)printf("TrExtAlignDB::UpdateTkDBc-E- TDV not accessible, I give up\n");
	return -2;
      }
      tf8 = Find(8, time);
      tf9 = Find(9, time);
      dt8 = (int)tf8-(int)time;
      dt9 = (int)tf9-(int)time;
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
    if(first) {GetFromTDV(time);first=0;}
    tf8 = Find(8, time);
    tf9 = Find(9, time);
    dt8 = (int)tf8-(int)time;
    dt9 = (int)tf9-(int)time;
    int ret=-1;
    if((dt8 < -200 || 200 < dt8)|| (dt9 < -200 || 200 < dt9)){
      ret=GetFromTDV(time);

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
    if(nwar++ <errlim) printf("TrExtAlignDB::UpdateTkDBc-I- Successuflly Loaded Align info from TDV\n");
  }




  for (int layer = 8; layer <= 9; layer++) {
    int plane = (layer == 8) ? 5 : 6;
    TkPlane* pl = TkDBc::Head->GetPlane(plane);
    if (!pl) return -3 ;

    const TrExtAlignPar *par = Get(layer, time);
    if(!par) return -4;
    pl->posA.setp(par->dpos[0], par->dpos[1], par->dpos[2]);
    pl->rotA.SetRotAngles(par->angles[0], par->angles[1], par->angles[2]);
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

  int nlin = GetLinearSize()/sizeof(float);
  int ntim = nlin/2/8;
  for (int i = 0; i < nlin; i++) fLinear[i] = 0;

  uint *uptr = (uint *)fLinear;

  for (int i = 0; i < 2; i++) {
    int layer = i+8;
    std::vector<uint> vt = GetVt(layer);

    int n = 0;
    for (int j = 0; j < vt.size(); j++) {
      uint time = vt.at(j);
      if (time == 0) continue;

      TrExtAlignPar par = GetM(layer, time);
      uptr   [(i*ntim+n)*8]   = time;
      fLinear[(i*ntim+n)*8+1] = par.dpos  [0];
      fLinear[(i*ntim+n)*8+2] = par.dpos  [1];
      fLinear[(i*ntim+n)*8+3] = par.dpos  [2];
      fLinear[(i*ntim+n)*8+4] = par.angles[0];
      fLinear[(i*ntim+n)*8+5] = par.angles[1];
      fLinear[(i*ntim+n)*8+6] = par.angles[2];
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

  int nlin = GetLinearSize()/4;
  int ntim = nlin/2/8;

  uint *uptr = (uint *)fLinear;

  Clear();

  for (int i = 0; i < 2; i++) {
    int layer = i+8;

    int n = 0;
    for (int j = 0; j < ntim; j++) {
      uint time = uptr[(i*ntim+j)*8];
      if (time == 0) continue;

      TrExtAlignPar &par = GetM(layer, time);
      par.dpos  [0] = fLinear[(i*ntim+j)*8+1];
      par.dpos  [1] = fLinear[(i*ntim+j)*8+2];
      par.dpos  [2] = fLinear[(i*ntim+j)*8+3];
      par.angles[0] = fLinear[(i*ntim+j)*8+4];
      par.angles[1] = fLinear[(i*ntim+j)*8+5];
      par.angles[2] = fLinear[(i*ntim+j)*8+6];
      n++;
    }

    cout << "size" << layer << "= " << n << " " << GetSize(layer) << endl;
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
    pl->posT.setp(ppar.dpos[0], ppar.dpos[1], ppar.dpos[2]);
    pl->rotT.SetRotAngles(ppar.angles[0], ppar.angles[1], ppar.angles[2]);
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


int  TrExtAlignDB::GetFromTDV(uint time){
  
  time_t tt=time;
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
  TrExtAlignDB::CreateLinear();
  AMSTimeID* db=new AMSTimeID(AMSID("TrackerExtAlign",1),begin,end,
			      TrExtAlignDB::GetLinearSize(),
			      TrExtAlignDB::fLinear,
			      AMSTimeID::Standalone,1,SLin2ExAlign);
  int ret=db->validate(tt);
  if(db) delete db;
  return ret;
}
