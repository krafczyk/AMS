// $Id: tbpos.C,v 1.3 2011/03/31 10:10:05 haino Exp $
#include "point.h"
#include "TMath.h"
#include "TString.h"

class TBpos {

public:
  enum { T416 = 416, T280 = 280, T80 = 80, T10 = 10, 
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
  int   Find(AMSPoint post, AMSDir dirt, int ipos = fIpos, int fpos = fFpos);

  AMSPoint GetPos(float z, int i) const;
  float    GetDst(int i, AMSPoint post, AMSDir dirt) const;
  float    GetDth(int i,                AMSDir dirt) const;

  static float   fZref;
  static TString fWdir;
  static const char *FileName(int id);

  static int fIpos;
  static int fFpos;
  static int RunToPos(int run);
};


TBpos::TBpos(int id, const char *fname)
{
  Init();
  if (id > 0) Read(id, fname);
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

  std::cout << "TB position data (" << id << "): " << fname << std::endl;

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

TString TBpos::fWdir = _WDIR_"/dat/";

const char *TBpos::FileName(int id)
{
  static TString sfn;

  sfn = "id_unknown";
  if (id == T416) sfn = fWdir+"TRACKER416_ams.txt";
  if (id == T280) sfn = fWdir+"TRACKER280_ams.txt";
  if (id == T80 ) sfn = fWdir+"TRACKER80_ams.txt";
  if (id == T60 ) sfn = fWdir+"TRACKER60_ams.txt";
  if (id == T10 ) sfn = fWdir+"TRACKER10_ams.txt";
  if (id == T60A) sfn = fWdir+"TRACKER60r_ams.txt";
  if (id == T60R) sfn = fWdir+"TRACKER60_ams.txt";
  if (id == E7  ) sfn = fWdir+"ECAL7_ams.txt";

  return sfn.Data();
}

int TBpos::fIpos = -1;
int TBpos::fFpos = -1;

int TBpos::RunToPos(int run)
{
  TString sfn = fWdir+"rundb";

  std::ifstream fin(sfn);
  if (!fin) {
    std::cerr << "File not found: " << sfn.Data() << std::endl;
    return -1;
  }

  TString str;
  for (int il = 0;; il++) {
    str.ReadToken(fin);
    if (fin.eof()) break;

    if (str.Atoi() == run) {
      for (Int_t i = 0; i < 4; i++) str.ReadToken(fin);
      if (!str.Contains("TRACKER") && !str.Contains("ECAL")) {
	std::cerr << "Format error at L= " << il+1 << " RUN= "
		  << run << " str= " << str.Data() << std::endl;
	return -1;
      }
      if (str.Contains("_"))
	fIpos = 
	fFpos = TString(str(str.First('_')+1, 3)).Atoi();
      else
	fIpos = TString(str(str.First(':')+1, 3)).Atoi();

      Int_t tbpos = -1;
      if (str.Contains("TRACKER416")) tbpos = T416;
      if (str.Contains("TRACKER280")) tbpos = T280;
      if (str.Contains("TRACKER80" )) tbpos = T80;
      if (str.Contains("TRACKER10" )) tbpos = T10;
      if (str.Contains("TRACKER60" )) tbpos = T60;
      if (str.Contains("TRACKERA60")) tbpos = T60A;
      if (str.Contains("TRACKER60B")) tbpos = T60R;
      if (str.Contains("ECAL7"))      tbpos = E7;

      if (tbpos == E7) {
	if      (fIpos == 101) fIpos = fFpos = 1;
	else if (fIpos == 110) fIpos = fFpos = 2;
	else if (fIpos == 104) fIpos = fFpos = 3;
	else if (fIpos == 112) fIpos = fFpos = 4;
	else if (fIpos == 113) fIpos = fFpos = 5;
	else { fIpos = 1; fFpos = 5; }
      }

      if (tbpos < 0) {
	std::cerr << "Unknown position: RUN= " << run
		  << " str= " << str.Data() << std::endl;
	return -1;
      }
      if (fFpos >= 0) return tbpos;

      str.ReadLine(fin, kFALSE);
      for (Int_t i = 0; i < 5; i++) str.ReadToken(fin);
      if (str.Contains("TRACKER")) {
	if (str.Contains("_"))
	  fFpos = TString(str(str.First('_')+1, 3)).Atoi()-1;
	else
	  fFpos = TString(str(str.First(':')+1, 3)).Atoi()-1;
      }
      return tbpos;
    }
    str.ReadLine(fin, kFALSE);
  }
  std::cerr << "Position not found: RUN= " << run << std::endl;

  return -1;
}
