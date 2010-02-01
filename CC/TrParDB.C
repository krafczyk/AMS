//  $Id: TrParDB.C,v 1.3 2010/02/01 12:44:05 shaino Exp $

//////////////////////////////////////////////////////////////////////////
///
///\file  TrParDB.C
///\brief Source file of TrParDB class
///
///\date  2008/06/19 AO  First version
///$Date: 2010/02/01 12:44:05 $
///
///$Revision: 1.3 $
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
  } 
  else { 
    printf("Only one instance of TrParDB allowed.\n");
    printf("Use TrParDB:Head->Load() to load a new parameters set.\n");
  }

  // default values
  _pngain = 0;
  for (int ii=0; ii<0; ii++) _asymmetry[ii] = 0.;
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
  for(int oct=0; oct<8; oct++){
    int crate = TkDBc::Head->_octant_crate[oct];
    for (int tdr=0; tdr<24; tdr++) {
      int hwid = crate*100+tdr;
      TrLadPar* lad = FindPar_HwId(hwid);
      lad->PrintInfo();
    }
  }
}


void TrParDB::Clear(const Option_t*aa){
  TObject::Clear(aa);
  for ( trparIT pp=trpar_hwidmap.begin();pp!=trpar_hwidmap.end();++pp)
    if(pp->second)  pp->second->Clear();
  if(linear) delete [] linear;
}

void TrParDB::ParDB2Lin(){

  if(! linear) {
    printf("TrParDB::ParDB2Lin()-INFO the linear space is created NOW\n");
    linear= new float[GetLinearSize()/4];
  }
  linear[0]= _pngain;
  linear[1]= _asymmetry[0];
  linear[2]= _asymmetry[1];

  for (trparIT aa=trpar_hwidmap.begin(); aa!=trpar_hwidmap.end();aa++){
    int hwid=aa->second->GetHwId();
    int crate=hwid/100;
    int tdr  =hwid%100;
    int offset=PARDBOFF+(crate*24+tdr)*TrLadPar::GetSize();
    aa->second->Par2Lin(&(linear[offset]));
  }
			
}
    


void TrParDB::Lin2ParDB(){

  if(! linear){
    printf("TrParDB::Lin2ParDB()- Error! the linear space pointer is NULL!\n");
    printf(" Parameters are NOT updated!!!\n");
    return;
  }
  _pngain       = linear[0];
  _asymmetry[0] = linear[1];
  _asymmetry[1] = linear[2];
  
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


void SLin2parDB(){
  if(TrParDB::Head)
    TrParDB::Head->Lin2ParDB();
  return;
}


void SLin2ParDB(){
  if(TrParDB::Head)
    TrParDB::Head->Lin2ParDB();
  return;
}
