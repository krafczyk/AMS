//  $Id: TkLadder.C,v 1.10 2012/05/07 09:02:35 pzuccon Exp $

//////////////////////////////////////////////////////////////////////////
///
///\file  TkLadder.C
///\brief Source file of TkLadder class
///
/// author P.Zuccon -- INFN Perugia 20/11/2007 
///
///\date  2008/01/17 PZ  First version
///\date  2008/01/23 SH  Some comments are added
///\date  2008/03/17 SH  Some utils for MC geometry are added
///\date  2008/04/02 SH  Update for alignment correction
///$Date: 2012/05/07 09:02:35 $
///
///$Revision: 1.10 $
///
//////////////////////////////////////////////////////////////////////////

#include "TkLadder.h"
#include "TkDBc.h"

ClassImp(TkLadder);

   //                   0 1 2 3 4 5 6 7 8 9 10 11 12 13 14 15 16 17 18 19 20 21 22 23
int TkLadder::pwg[24]= {1,1,4,4,1,1,4,4,1,1, 4, 4, 2, 2, 3, 3, 2, 2, 3, 3, 2, 2, 3, 3};
   //                   0 1 2 3 4 5 6 7 8 9 10 11 12 13 14 15 16 17 18 19 20 21 22 23
int TkLadder::pwgp[24]={1,2,5,6,3,4,3,4,5,6, 1, 2, 1, 2, 5, 6, 3, 4, 3, 4, 5, 6, 1, 2};

int TkLadder::version=0;

TkLadder::TkLadder():TkObject()
{
  _layer=0;
  _slot=-1;
  _crate=0;
  _tdr=0;
  _nsensors=0;
  for (int i = 0; i < trconst::maxsen; i++){ _sensx[i] =0.; _sensy[i] =0; _sensz[i] = 0.;}
}

TkLadder::TkLadder(char* Name,int layer,int slot,int crate,int tdr, int nsensors):TkObject(Name)
{
  _layer=layer;
  _slot=slot;
  _crate=crate;
  _tdr=tdr;
  _nsensors=nsensors;
  for (int i = 0; i < trconst::maxsen; i++){ _sensx[i] =0.; _sensy[i] =0; _sensz[i] = 0.;}
}

TkLadder::TkLadder(char* Name,int Trid,int HwId,  int nsensors):TkObject(Name){
  _layer=abs(Trid)/100;
  _slot=abs(Trid)/Trid*(abs(Trid)-100*_layer);
  _crate=HwId/100;
  _tdr=HwId-_crate*100;
  _nsensors=nsensors;
  for (int i = 0; i < trconst::maxsen; i++){ _sensx[i] =0.; _sensy[i] =0; _sensz[i] = 0.;}
}

TkPlane* TkLadder::GetPlane()const{
  return TkDBc::Head->GetPlane(GetPlaneId());
}


int TkLadder::GetLayerJ()const{
    return TkDBc::Head->GetJFromLayer(GetLayer());
}


void TkLadder::SetTkId(int Trid){
  _layer=abs(Trid)/100;
  _slot=abs(Trid)/Trid*(abs(Trid)-100*_layer);
}

void TkLadder::SetHwId(int HwId){
  _crate=HwId/100;
  _tdr=HwId-_crate*100;
}



int TkLadder::GetOctant(){
  
  int aa=GetSide();

  if(abs(_slot)<1||abs(_slot)>15||aa>1||aa<0) return -9999;

  if(_layer==1){
    if(aa==0){
      if(abs(_slot)>=7) return 4;
      if(abs(_slot) <7) return 2;
    } 
    else {
      if(abs(_slot)<=9) return 1;
      if(abs(_slot) >9) return 3;
    }
  }

  else if(_layer==8){
    if(aa==0){
      if(abs(_slot)>=7) return 8;
      if(abs(_slot) <7) return 6;
    } 
    else {
      if(abs(_slot)<=9) return 5;
      if(abs(_slot) >9) return 7;
    }
  }
  
  else if (_layer==2 ||_layer==3 ||_layer==4){
    if(abs(_slot)==8)  return -9999;

    if(abs(_slot)<=7 && aa==0) return 2;  
    if(abs(_slot)>=9 && aa==0) return 4;
    
    if(abs(_slot)>=9 && aa==1) return 3;
    if(abs(_slot)<=7 && aa==1) return 1; 
  }

  else if (_layer==5 ||_layer==6 ||_layer==7){
    if(abs(_slot)==8)  return -9999;
  
    if(abs(_slot)>=9 && aa==0) return 8; 
    if(abs(_slot)<=7 && aa==0) return 6; 
    if(abs(_slot)>=9 && aa==1) return 7;
    if(abs(_slot)<=7 && aa==1) return 5; 
  }
  
  return -9999;


}


ostream& TkLadder::putout(ostream& s) const {
  TkObject::putout(s);
  s <<GetTkId()<<"  "<<
    GetHwId()<<"  "<<_nsensors<< endl;
  if(version>=2) {
    char myout[50];
    for(int ii=0;ii<trconst::maxsen;ii++){
      sprintf(myout," %+8.5f  %+8.5f  %+8.5f\n",_sensx[ii],_sensy[ii], _sensz[ii]);
      s << myout;
    }
  }   
  return s; 
	
}

istream& TkLadder::putin(istream& s){
  int hw,tk;
  TkObject::putin(s);
  if(s.eof()) return s;
  if(!s.good()){
    cerr<< "Error TkLadder::putin the channel is not good before"<<endl;
  }
  s>>tk>>hw>>_nsensors;
  if(!s.good()){
    cerr<< "Error TkLadder::putin the channel is not good  after"<<endl;
  }
  SetTkId(tk);
  SetHwId(hw);
  if (version >=2) 
    for(int ii=0;ii<trconst::maxsen;ii++){
      s >>_sensx[ii] >> _sensy[ii] >> _sensz[ii] ;
      if(!s.good()){
	cerr<< "Error TkLadder::putin the channel is not good  after"<<endl;
      }
    }
  return s;

}


istream& TkLadder::putinA(istream& s){
  int hw,tk;
  TkObject::putinA(s);
  if(s.eof()) return s;
  if(!s.good()){
    cerr<< "Error TkLadder::putin the channel is not good before"<<endl;
  }
  s>>tk>>hw>>_nsensors;
  if(!s.good()){
    cerr<< "Error TkLadder::putin the channel is not good  after"<<endl;
  }
 if (version >=2) 
   for(int ii=0;ii<trconst::maxsen;ii++){
     s >>_sensx[ii] >> _sensy[ii] >> _sensz[ii] ;
     if(!s.good()){
       cerr<< "Error TkLadder::putin the channel is not good  after"<<endl;
     }
   }
  
 return s;
 
}


ostream& TkLadder::putoutA(ostream& s){
  TkObject::putoutA(s);
  s<<  GetTkId()<<"  "<<
    GetHwId()<<"  "<<_nsensors<<endl;
  if(version>=2) {
    char myout[80];
    for(int ii=0;ii<trconst::maxsen;ii++){
      sprintf(myout," %+8.5f  %+8.5f  %+8.5f\n",_sensx[ii],_sensy[ii], _sensz[ii]);
      s << myout;
    }
  }   
  
  return s;
}


istream& TkLadder::putinT(istream& s){
  int hw,tk;
  TkObject::putinT(s);
  if(s.eof()) return s;
  if(!s.good()){
    cerr<< "Error TkLadder::putin the channel is not good before"<<endl;
  }
  s>>tk>>hw>>_nsensors;
  if(!s.good()){
    cerr<< "Error TkLadder::putin the channel is not good  after"<<endl;
  }
  
  return s;

}


ostream& TkLadder::putoutT(ostream& s){

    return TkObject::putoutT(s)<<
      GetTkId()<<"  "<<
      GetHwId()<<"  "<<_nsensors<<endl;

}



istream& TkLadder::putinS(istream& s){
  for (int i = 0; i < trconst::maxsen; i++) s >> _sensx[i];
  for (int i = 0; i < trconst::maxsen; i++) s >> _sensy[i];

  if(s.eof()) return s;
  if(!s.good()){
    cerr<< "Error TkLadder::putinS the channel is not good before"<<endl;
  }
  
  return s;

}


ostream& TkLadder::putoutS(ostream& s){

  s<<GetTkId()<<endl<<"  ";
  for (int i = 0; i < trconst::maxsen; i++) s << _sensx[i]<<" ";
  s<<endl<<"  ";
  for (int i = 0; i < trconst::maxsen; i++) s << _sensy[i]<<" ";
  s<<endl;

  return s;
}

void TkLadder::Align2Lin(float * off){
  off[0]=posA[0];
  off[1]=posA[1];
  off[2]=posA[2];
  double a,b,c;
  GetRotAnglesA(a,b,c);
  off[3]=a;
  off[4]=b;
  off[5]=c;
  if(version<2) return;
  int ioff=6;
  for(int ii=0; ii< trconst::maxsen;ii++){
    off[ioff+ii*3+0]=_sensx[ii];
    off[ioff+ii*3+1]=_sensy[ii];
    off[ioff+ii*3+2]=_sensz[ii];
  }
  return ;
}


void TkLadder::Lin2Align(float * off){
  posA[0]=off[0];
  posA[1]=off[1];
  posA[2]=off[2];
  
  SetRotAnglesA(off[3],off[4],off[5]);
  if(version<2) return;
  int ioff=6;
  for(int ii=0; ii< trconst::maxsen;ii++){
    _sensx[ii]=off[ioff+ii*3+0];
    _sensy[ii]=off[ioff+ii*3+1];
    _sensz[ii]=off[ioff+ii*3+2];
  }
  
  return;
}

void TkLadder::ResetSensorAlign(){

  for (int i = 0; i < trconst::maxsen; i++) {
    _sensx[i] = TkDBc::Head->GetSensAlignX(GetTkId(), i);
    _sensy[i] = TkDBc::Head->GetSensAlignY(GetTkId(), i);
    _sensz[i] = 0;
  }
  return;
}
