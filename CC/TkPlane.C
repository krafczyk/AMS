//  $Id: TkPlane.C,v 1.5 2012/05/07 09:02:35 pzuccon Exp $

//////////////////////////////////////////////////////////////////////////
///
///\file  TkPlane.C
///\brief Source file of TkPlane class
///
/// author P.Zuccon -- INFN Perugia 20/11/2007 
///
///\date  2008/01/17 PZ  First version
///\date  2008/01/23 SH  Some comments are added
///$Date: 2012/05/07 09:02:35 $
///
///$Revision: 1.5 $
///
//////////////////////////////////////////////////////////////////////////

#include "TkPlane.h"

ClassImp(TkPlane);

TkPlane::TkPlane():TkObject()
{
  _pnumber=0;
  _nslot[0]=0;
  _nslot[1]=0;
}
TkPlane::TkPlane(char* Name,int number, short int nslot[2]):TkObject(Name)
{
  _pnumber=number;
  _nslot[0]=nslot[0];   
  _nslot[1]=nslot[1];
  
}



ostream& TkPlane::putout(ostream& s) const{

   return TkObject::putout(s)<<_pnumber<<" "<<_nslot[0]<<" "<<_nslot[1]<<endl;
  
}


ostream& TkPlane::putoutA(ostream& s){

   return TkObject::putoutA(s)<<_pnumber<<" "<<_nslot[0]<<" "<<_nslot[1]<<endl;
  
}

ostream& TkPlane::putoutT(ostream& s){

   return TkObject::putoutT(s)<<_pnumber<<" "<<_nslot[0]<<" "<<_nslot[1]<<endl;
  
}


istream& TkPlane::putin(istream& s ) {

  TkObject::putin(s);

  s>> _pnumber >> _nslot[0] >> _nslot[1];
  
  return s;
}


istream& TkPlane::putinA(istream& s ) {

  TkObject::putinA(s);

  s>> _pnumber >> _nslot[0] >> _nslot[1];
  
  return s;
}

istream& TkPlane::putinT(istream& s ) {

  TkObject::putinT(s);

  s>> _pnumber >> _nslot[0] >> _nslot[1];
  
  return s;
}


ClassImp(TkPlaneExt);

int TkPlaneExt::AlKind=0;
ostream& TkPlaneExt::putout(ostream& s) const{

  return s<<name<<endl <<pos <<endl<<rot<<
    GetPosA()<<endl<<GetRotMatA()
	  <<_pnumber<<" "<<_nslot[0]<<" "<<_nslot[1]<<endl;
  
}


ostream& TkPlaneExt::putoutA(ostream& s){

 return s<<name<<endl<<
    GetPosA()<<endl<<GetRotMatA()
	  <<_pnumber<<" "<<_nslot[0]<<" "<<_nslot[1]<<endl;
  
}




istream& TkPlaneExt::putin(istream& s){

  s.width(50);

  if(!s.good()){
    cerr <<" Error in TkObject::putin the channel is not good"<<endl;
    return s;
  }
  s>> name;
  if (s.eof()) return s;
  if(!s.good()){
    cerr <<" Error in TkObject::putin the channel is not good"<<endl;
    return s;
  }
  s>> pos;
  if(!s.good()){
    cerr <<" Error in TkObject::putin the channel is not good"<<endl;
    return s;
  }
  s>> rot;
  if(!s.good()){
    cerr <<" Error in TkObject::putin the channel is not good"<<endl;
    return s;
  }
  s>> posvA(AlKind);
  if(!s.good()){
    cerr <<" Error in TkObject::putin the channel is not good"<<endl;
    return s;
  }
  s>> rotvA(AlKind);
  if(!s.good()){
    cerr <<" Error in TkObject::putin the channel is not good"<<endl;
    return s;
  }

  s>> _pnumber >> _nslot[0] >> _nslot[1];

  return s;
}





istream& TkPlaneExt::putinA(istream& s){

  s.width(50);

  if(!s.good()){
    cerr <<" Error in TkObject::putin the channel is not good"<<endl;
    return s;
  }
  s>> name;
  if (s.eof()) return s;
  if(!s.good()){
    cerr <<" Error in TkObject::putin the channel is not good"<<endl;
    return s;
  }
  s>> (posvA(AlKind));
  if(!s.good()){
    cerr <<" Error in TkObject::putin the channel is not good"<<endl;
    return s;
  }
  s>> (rotvA(AlKind));
  if(!s.good()){
    cerr <<" Error in TkObject::putin the channel is not good"<<endl;
    return s;
  }
  s>> _pnumber >> _nslot[0] >> _nslot[1];

  return s;
}
