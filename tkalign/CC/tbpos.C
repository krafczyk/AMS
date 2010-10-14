
#include "point.h"
#include "TMath.h"
#include "TString.h"

class TBpos {

public:
  enum { T416 = 416, T80 = 80, T280 = 280,
	 T60  =  60, // TRACKER60FEB
	 T60A = 960, // TRACKER60        (180 deg rotation in Z-axis)
	 T60R = 860, // TRACKER60FEBBACK ("negative" proton beam)
	 E7   =   7
  };

  int    fPID;   // Position ID
  int    fNpos;  // Number of positions
  float *fXbm;   // X (cm) at Z=0
  float *fYbm;   // Y (cm) at Z=0
  float *fTbm;   // Theta (rad)
  float *fPbm;   // Phi   (rad)

  TBpos(int id, const char *fname = 0);
  TBpos(void);
 ~TBpos();

  void  Init(void);
  int   Read(int id, const char *fname = 0);
  int   Find(AMSPoint post, AMSDir dirt, int ipos = -1, int fpos = -1);

  AMSPoint GetPos(float z, int i) const;
  float    GetDst(int i, AMSPoint post, AMSDir dirt) const;
  float    GetDth(int i,                AMSDir dirt) const;

  static float   fZref;
  static TString fWdir;
  static const char *FileName(int id);
};


TBpos::TBpos(int id, const char *fname)
{
  Init();
  Read(id, fname);
}

TBpos::TBpos(void)
{
  Init();
}

TBpos::~TBpos()
{
  delete [] fXbm;
  delete [] fYbm;
  delete [] fTbm;
  delete [] fPbm;
}

void TBpos::Init(void)
{
  fPID = -1;
  fNpos = 0;
  fXbm = fYbm = fTbm = fPbm = 0;
}

int TBpos::Read(int id, const char *fname)
{
  if (!fname) fname = FileName(id);

  std::ifstream fin(fname);
  if (!fin) {
    std::cerr << "File not found: " << fname << std::endl;
    return -1;
  }
  fPID = id;
  fin >> fNpos;

  fXbm = new float[fNpos];
  fYbm = new float[fNpos];
  fTbm = new float[fNpos];
  fPbm = new float[fNpos];

  for (int i = 0; i < fNpos; i++) {
    float x, y, z, dx, dy, dz;
    fin >> x >> y >> z >> dx >> dy >> dz;

    if (fin.eof()) {
      std::cerr << "Unexpected EOF" << std::endl;
      return -1;
    }

    AMSDir dir(dx, dy, dz);
    fXbm[i] = x;
    fYbm[i] = y;
    fTbm[i] = dir.gettheta();
    fPbm[i] = dir.getphi();
  }
  return fNpos;
}

int TBpos::Find(AMSPoint post, AMSDir dirt, int ipos, int fpos)
{
  int npos = 20;

  int i1 = 0;
  int i2 = fNpos-1;

  if (ipos > 0) {
    i1 = ipos-1;
    i2 = (fpos > 0) ? fpos-1 : ipos+npos;
  }

  int   imin = -1;
  float dmin = 0;

  for (int i = i1; i <= i2; i++) {
    float dd = GetDst(i, post, dirt);
    if (dd < 0) continue;
    if (imin < 0 || dd < dmin) {
      dmin = dd;
      imin = i;
    }
  }

  return imin;
}

AMSPoint TBpos::GetPos(float z, int i) const
{
  if (i < 0 || fNpos <= i) return AMSPoint(0, 0, 0);

  AMSPoint pnt(fXbm[i], fYbm[i], 0);
  AMSDir   dir(fTbm[i], fPbm[i]);

  return pnt+dir/dir.z()*(z-pnt.z());
}

float TBpos::fZref = 200;

float TBpos::GetDst(int i, AMSPoint post, AMSDir dirt) const
{
  if (i < 0 || fNpos <= i) return -1;

  AMSPoint pnt(fXbm[i], fYbm[i], 0);
  AMSDir   dir(fTbm[i], fPbm[i]);

  float dp = post.dist(pnt);
  float dd = (TMath::Pi()-TMath::ACos(dirt.prod(dir)))*fZref;
  return dp+dd;
}

float TBpos::GetDth(int i, AMSDir dirt) const
{
  if (i < 0 || fNpos <= i) return -1;

  AMSDir dir(fTbm[i], fPbm[i]);
  float dth = TMath::ACos(dirt.prod(dir))*TMath::RadToDeg();

  return (dth > 90) ? 180-dth : dth;
}

//TString TBpos::fWdir = "/f2users/shaino/tracker/dat/";
TString TBpos::fWdir = "/amssw/haino/tracker/dat/";

const char *TBpos::FileName(int id)
{
  static TString sfn;

  sfn = "id_unknown";
  if (id == T416) sfn = fWdir+"TRACKER416_ams.txt";
  if (id == T80 ) sfn = fWdir+"TRACKER80_ams.txt";
  if (id == T280) sfn = fWdir+"TRACKER280_ams.txt";
  if (id == T60 ) sfn = fWdir+"TRACKER60_ams.txt";
  if (id == T60A) sfn = fWdir+"TRACKER60r_ams.txt";
  if (id == T60R) sfn = fWdir+"TRACKER60_ams.txt";
  if (id == E7  ) sfn = fWdir+"ECAL7_ams.txt";

  return sfn.Data();
}
