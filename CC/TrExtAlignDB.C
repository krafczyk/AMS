#include "TrExtAlignDB.h"
#include "TkDBc.h"
#include "TFile.h"

ClassImp(TrExtAlignPar);
ClassImp(TrExtAlignDB);

using namespace  std;
TrExtAlignDB* TrExtAlignDB::Head=0;

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

const TrExtAlignPar &TrExtAlignDB::Get(int lay, uint time) const
{
  ealgITc it;
  if (lay == 8 && (it = L8.lower_bound(time)) != L8.end()) return it->second;
  if (lay == 9 && (it = L9.lower_bound(time)) != L9.end()) return it->second;

  static TrExtAlignPar dummy;
  return dummy;
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

void TrExtAlignDB::UpdateTkDBc(uint time) const
{
  if (!TkDBc::Head) {
    std::cerr << "TkDBc::Head is null" << std::endl;
    return;
  }

  static uint btime = 0;

  for (int layer = 8; layer <= 9; layer++) {
    int plane = (layer == 8) ? 5 : 6;
    TkPlane* pl = TkDBc::Head->GetPlane(plane);
    if (!pl) continue;

    TrExtAlignPar par = Get(layer, time);
    pl->posA.setp(par.dpos[0], par.dpos[1], par.dpos[2]);
    pl->rotA.SetRotAngles(par.angles[0], par.angles[1], par.angles[2]);
  }

  btime = time;
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
