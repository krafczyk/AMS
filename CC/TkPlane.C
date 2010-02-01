//  $Id: TkPlane.C,v 1.3 2010/02/01 12:44:05 shaino Exp $

//////////////////////////////////////////////////////////////////////////
///
///\file  TkPlane.C
///\brief Source file of TkPlane class
///
/// author P.Zuccon -- INFN Perugia 20/11/2007 
///
///\date  2008/01/17 PZ  First version
///\date  2008/01/23 SH  Some comments are added
///$Date: 2010/02/01 12:44:05 $
///
///$Revision: 1.3 $
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


