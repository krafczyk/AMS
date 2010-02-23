//  $Id: TrParDB.C,v 1.4 2010/02/23 14:59:56 oliva Exp $

//////////////////////////////////////////////////////////////////////////
///
///\file  TrParDB.C
///\brief Source file of TrParDB class
///
///\date  2008/06/19 AO  First version
///$Date: 2010/02/23 14:59:56 $
///
///$Revision: 1.4 $
///
//////////////////////////////////////////////////////////////////////////

#include "TrParDB.h"
#include "TkDBc.h"

#include "TFile.h"

ClassImp(TrParDB);

TrParDB* TrParDB::Head=0;
float* TrParDB::linear=0;

void TrParDB::init(){
  // creating the 192 TrLadPar (default)
  if (Head==0) {
    Head = this;
    for(int oct=0; oct<8; oct++){
      int crate = TkDBc::Head->_octant_crate[oct];
      for (int tdr=0; tdr<24; tdr++) {
	int hwid = crate*100+tdr;
	trpar_hwidmap[hwid] = new TrLadPar(hwid);
      }
    }
 
    _pngain = 1;
    for (int ii=0; ii<2; ii++) _asymmetry[ii] = 0.;
    for (int ix=0; ix<CHLOSSARR; ix++) {
      for (int iy=0; iy<CHLOSSARR; iy++) {
        _chargelossx[ix][iy] = 1.;
        _chargelossy[ix][iy] = 1.;
      }
    }
  } 
  else { 
    printf("Only one instance of TrParDB allowed.\n");
    printf("Use TrParDB:Head->Load() to load a new parameters set.\n");
  }
}

TrParDB::TrParDB(char * filename){
  if (filename) Load(filename);
}

TrParDB::~TrParDB() {
  for (trparIT pp=trpar_hwidmap.begin(); pp!=trpar_hwidmap.end(); ++pp)
    if(pp->second) delete pp->second;
}

TrLadPar* TrParDB::FindPar_TkAssemblyId(int tkassemblyid) {
  TkLadder* ll = TkDBc::Head->FindTkAssemblyId(tkassemblyid);
  if (ll) return FindPar_HwId(ll->GetHwId());
  else return 0;
}

TrLadPar* TrParDB::FindPar_HwId(int hwid){ 
  trparIT aa = trpar_hwidmap.find(hwid);
  if (aa!=trpar_hwidmap.end()) return aa->second; 
  else                         return 0;
}

TrLadPar* TrParDB::FindPar_TkId(int tkid){
  TkLadder* ll = TkDBc::Head->FindTkId(tkid);
  if (ll) return FindPar_HwId(ll->GetHwId());
  else    return 0;
}

TrLadPar* TrParDB::FindPar_LadName(string& name){
  TkLadder* ll=TkDBc::Head->FindLadName(name);
  if (ll) return FindPar_HwId(ll->GetHwId());
  else    return 0;
}

TrLadPar* TrParDB::FindPar_JMDC(int JMDCid){
  int crate = (JMDCid-282)/24;
  int tdr   = JMDCid-crate*24-282;
  int hwid  = crate*100+tdr;
  printf(" crate %d tdr %d hwid %d \n",crate,tdr,hwid);
  return FindPar_HwId(hwid);
}
 
TrLadPar* TrParDB::GetEntry(int ii){
  int count=0;
  for(trparIT pp=trpar_hwidmap.begin();pp!=trpar_hwidmap.end();pp++){
    if(count==ii) return &(*pp->second);
    count++;
  }
  return 0;
}

void TrParDB::Load(char * filename){
  TFile f(filename);
  Head = (TrParDB*) f.Get("TrParDB");
  f.Close();
  return;
}

void TrParDB::PrintInfo() {
  printf("Asymmetry values: %5.3f %5.3f\n",GetAsymmetry(0),GetAsymmetry(1));
  printf("Tracker gain p/n gain: %5.3f\n",GetPNGain());
  printf("Ladder Gains Table\n");
  printf("HwId TkId     P     N  Va00  Va01  Va02  Va03  Va04  Va05  Va06  Va07  Va08  Va09  Va10  Va11  Va12  Va13  Va14  Va15\n");
  for (int iladder=0; iladder<192; iladder++) {
    int tkid = TkDBc::Head->Entry2TkId(iladder);
    int hwid = TkDBc::Head->FindTkId(tkid)->GetHwId();
    TrLadPar* ladpar = FindPar_TkId(tkid);
    ladpar->PrintInfo();
  }
  printf("Charge loss matrices (x = ia, y = ip):\n");
  for (int iside=0; iside<2; iside++) {
    for (int ix=0; ix<10; ix++) {
      for (int iy=0; iy<10; iy++) {  
        printf("%7.3f ",GetChargeLossArrayElem(iside,ix,iy));
      }
      printf("\n");
    }
    printf("\n");
  }
}

void TrParDB::SetChargeLossArray(int icoo, float array[CHLOSSARR][CHLOSSARR]) {
  if (icoo>1) return;
  for (int ix=0; ix<CHLOSSARR; ix++) {
    for (int iy=0; iy<CHLOSSARR; iy++) {
      if      (icoo==0) _chargelossx[ix][iy] = array[ix][iy];
      else if (icoo==1) _chargelossy[ix][iy] = array[ix][iy];
    }
  }
}

float TrParDB::GetChargeLossArrayElem(int icoo, int ind_ip, int ind_ia) {
  if      (icoo>1) return 1;   // no side
  if      (ind_ip<0)         ind_ip = 0; 
  else if (ind_ip>CHLOSSARR) ind_ip = CHLOSSARR-1;
  if      (ind_ia<0)         ind_ia = 0;
  else if (ind_ia>CHLOSSARR) ind_ia = CHLOSSARR-1;
  if      (icoo==0) return _chargelossx[ind_ip][ind_ia];
  else if (icoo==1) return _chargelossy[ind_ip][ind_ia];
  else              return 0; 
}

float TrParDB::GetChargeLoss(int icoo, float ip, float ia) {
  if (icoo>1) return 1;   // no side
  float perip = ip - ceil(ip-0.5); // periodicity [-0.5,0.5]
  float absip = fabs(perip);       //symmetry around 0 axis
  int ind_ip = (int) (absip/(0.5/CHLOSSARR));
  int ind_ia = (int) (fabs(ia)/(40./CHLOSSARR));
  return GetChargeLossArrayElem(icoo,ind_ip,ind_ia);
}


void TrParDB::Clear(const Option_t*aa){
  TObject::Clear(aa);
  for ( trparIT pp=trpar_hwidmap.begin();pp!=trpar_hwidmap.end();++pp)
    if (pp->second) pp->second->Clear();
  if (linear) delete [] linear;

  _pngain = 1;
  for (int ii=0; ii<2; ii++) _asymmetry[ii] = 0.;
  for (int ix=0; ix<CHLOSSARR; ix++) {
    for (int iy=0; iy<CHLOSSARR; ix++) {
      _chargelossx[ix][iy] = 0;
      _chargelossy[ix][iy] = 0;
    }
  }
}



void TrParDB::ParDB2Lin(){
  if(! linear) {
    printf("TrParDB::ParDB2Lin()-INFO the linear space is created NOW\n");
    linear= new float[GetLinearSize()/4];
  }
  linear[0] = _pngain;
  for (int ii=0; ii<2; ii++) linear[ii+1] = _asymmetry[ii];
  for (int ix=0; ix<CHLOSSARR; ix++) {
    for (int iy=0; iy<CHLOSSARR; iy++) {
      linear[3 + ix*10+iy]     = _chargelossx[ix][iy];
      linear[3 + 100+ix*10+iy] = _chargelossy[ix][iy];
    }
  }
  for (trparIT aa=trpar_hwidmap.begin(); aa!=trpar_hwidmap.end();aa++){
    int hwid=aa->second->GetHwId();
    int crate=hwid/100;
    int tdr  =hwid%100;
    int offset=PARDBOFF+(crate*24+tdr)*TrLadPar::GetSize();
    aa->second->Par2Lin(&(linear[offset]));
  }			
}
    


void TrParDB::Lin2ParDB(){
  if(!linear){
    printf("TrParDB::Lin2ParDB()- Error! the linear space pointer is NULL!\n");
    printf(" Parameters are NOT updated!!!\n");
    return;
  }
  _pngain = linear[0];
  for (int ii=0; ii<2; ii++) _asymmetry[ii] = linear[ii+1];
  for (int ix=0; ix<CHLOSSARR; ix++) {
    for (int iy=0; iy<CHLOSSARR; iy++) {
      _chargelossx[ix][iy] = linear[3 + ix*10+iy];
      _chargelossy[ix][iy] = linear[3 + 100+ix*10+iy];
    }
  }
  for (trparIT aa=trpar_hwidmap.begin(); aa!=trpar_hwidmap.end();aa++){
    int hwid=aa->second->GetHwId();
    //printf("Filling Parame %03d\n",hwid);
    int crate=hwid/100;
    int tdr  =hwid%100;
    int offset=PARDBOFF+(crate*24+tdr)*TrLadPar::GetSize();
    aa->second->Lin2Par(&(linear[offset]));
    aa->second->SetFilled();
  }
  return;
}


void SLin2ParDB(){
  if(TrParDB::Head)
    TrParDB::Head->Lin2ParDB();
  return;
}



