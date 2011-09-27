//  $Id: TkObject.C,v 1.6 2011/09/27 23:50:03 pzuccon Exp $

//////////////////////////////////////////////////////////////////////////
///
///\file  TkObject.C
///\brief Source file of TkObject class
///
/// author P.Zuccon -- INFN Perugia 20/11/2007 
///
///\date  2008/01/17 PZ  First version
///\date  2008/01/23 SH  Some comments are added
///\date  2008/02/21 PZ  Updates for alignment correction
///\date  2008/04/02 SH  putin/putout updated for the alignment correction
///$Date: 2011/09/27 23:50:03 $
///
///$Revision: 1.6 $
///
//////////////////////////////////////////////////////////////////////////

#include "TkObject.h"

ClassImp(TkObject);

TkObject::TkObject(char* namein){
  sprintf(name,"%s",namein);
  pos=0;
  rot.Reset();

  posA=0;
  rotA.Reset();

  posT=0;
  rotT.Reset();
}


TkObject::TkObject(char* namein,AMSPoint posin,AMSRotMat rotin){

  sprintf(name,"%s",namein);
  pos=posin;
  rot=rotin;
  posA=0;
  rotA.Reset();
  posT=0;
  rotT.Reset();

}


istream& TkObject::putin(istream& s){

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
  s>> posA;
  if(!s.good()){
    cerr <<" Error in TkObject::putin the channel is not good"<<endl;
    return s;
  }
  s>> rotA;
  if(!s.good()){
    cerr <<" Error in TkObject::putin the channel is not good"<<endl;
    return s;
  }
/* // SH FIXME posT/rotT are not to be read to keep compatibility
  s>> posT;
  if(!s.good()){
    cerr <<" Error in TkObject::putin the channel is not good"<<endl;
    return s;
  }
  s>> rotT;
  if(!s.good()){
    cerr <<" Error in TkObject::putin the channel is not good"<<endl;
    return s;
  }
*/ // SH FIXME posT/rotT are not to be read to keep compatibility
  return s;
}


void TkObject::PrintT() { cout <<name<<endl<<posT<<endl<<rotT<<endl ; return;}



istream& TkObject::putinA(istream& s){

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
  s>> posA;
  if(!s.good()){
    cerr <<" Error in TkObject::putin the channel is not good"<<endl;
    return s;
  }
  s>> rotA;
  if(!s.good()){
    cerr <<" Error in TkObject::putin the channel is not good"<<endl;
    return s;
  }

  return s;
}



istream& TkObject::putinT(istream& s){

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
  s>> posT;
  if(!s.good()){
    cerr <<" Error in TkObject::putin the channel is not good"<<endl;
    return s;
  }
  s>> rotT;
  if(!s.good()){
    cerr <<" Error in TkObject::putin the channel is not good"<<endl;
    return s;
  }

  return s;
}






void TkObject::RotToMat(number nrm[][3]){
  
  for (int row=0;row<3;row++)
    for (int col=0;col<3;col++)
      nrm[row][col]=rot.GetEl(row,col);
  return;
}

void TkObject::Align2Lin(float * off){
    off[0]=posA[0];
    off[1]=posA[1];
    off[2]=posA[2];
    double a,b,c;
    GetRotAnglesA(a,b,c);
    off[3]=a;
    off[4]=b;
    off[5]=c;
    return;
  }


void TkObject::Lin2Align(float * off){
    posA[0]=off[0];
    posA[1]=off[1];
    posA[2]=off[2];

    SetRotAnglesA(off[3],off[4],off[5]);

    return;
}



void TkObject::MCAlign2Lin(float * off){
    off[0]=posT[0];
    off[1]=posT[1];
    off[2]=posT[2];
    double a,b,c;
    GetRotAnglesT(a,b,c);
    off[3]=a;
    off[4]=b;
    off[5]=c;
    return;
  }


void TkObject::Lin2MCAlign(float * off){
    posT[0]=off[0];
    posT[1]=off[1];
    posT[2]=off[2];

    SetRotAnglesT(off[3],off[4],off[5]);

    return;
}
