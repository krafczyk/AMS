// AMS common blocks method file
// Objectivity version Oct 08, 1996 ak
// Oct 14, 1996. ak. don't compare ccffkey run
//
// Last Edit : Oct 16, 1996. ak.
//

#include <typedefs.h>
#include <commonsD.h>
//#include <commons.C>

AMScommonsD::AMScommonsD() {
  integer  i;
  integer  l;
  integer* buff;

  cout <<"AMScommonsD -I- constructor "<<endl;
//tofmcffkey
  l = sizeof(TOFMCFFKEY);
  tofmcffkeyD.resize(l/4);
  buff = new integer[l/4];
  memcpy(buff,&TOFMCFFKEY,l);
  for (i=0; i<l/4; i++) { tofmcffkeyD.set(i,buff[i]);}
  delete [] buff;

//tofrecffkey
  l = sizeof(TOFRECFFKEY);
  tofrecffkeyD.resize(l/4);
  buff = new integer[l/4];
  memcpy(buff,&TOFRECFFKEY,l);
  for (i=0; i<l/4; i++) { tofrecffkeyD.set(i,buff[i]);}
  delete [] buff;

//ctcmcffkey
  l = sizeof(CTCMCFFKEY);
  ctcmcffkeyD.resize(l/4);
  buff = new integer[l/4];
  memcpy(buff,&CTCMCFFKEY,l);
  for (i=0; i<l/4; i++) { ctcmcffkeyD.set(i,buff[i]);}
  delete [] buff;

//ctcgeomffkey
  l = sizeof(CTCGEOMFFKEY);
  ctcgeomffkeyD.resize(l/4);
  buff = new integer[l/4];
  memcpy(buff,&CTCGEOMFFKEY,l);
  for (i=0; i<l/4; i++) { ctcgeomffkeyD.set(i,buff[i]);}
  delete [] buff;

//ctcrecffkey
  l = sizeof(CTCRECFFKEY);
  ctcrecffkeyD.resize(l/4);
  buff = new integer[l/4];
  memcpy(buff,&CTCRECFFKEY,l);
  for (i=0; i<l/4; i++) { ctcrecffkeyD.set(i,buff[i]);}
  delete [] buff;

//trmcffkey
  l = sizeof(TRMCFFKEY);
  trmcffkeyD.resize(l/4);
  buff = new integer[l/4];
  memcpy(buff,&TRMCFFKEY,l);
  for (i=0; i<l/4; i++) { trmcffkeyD.set(i,buff[i]);}
  delete [] buff;

//betafitffkey
  l = sizeof(BETAFITFFKEY);
  betafitffkeyD.resize(l/4);
  buff = new integer[l/4];
  memcpy(buff,&BETAFITFFKEY,l);
  for (i=0; i<l/4; i++) { betafitffkeyD.set(i,buff[i]);}
  delete [] buff;

//trfitffkey
  l = sizeof(TRFITFFKEY);
  trfitffkeyD.resize(l/4);
  buff = new integer[l/4];
  memcpy(buff,&TRFITFFKEY,l);
  for (i=0; i<l/4; i++) { trfitffkeyD.set(i,buff[i]);}
  delete [] buff;

//ccffkey
  l = sizeof(CCFFKEY);
  ccffkeyD.resize(l/4);
  buff = new integer[l/4];
  memcpy(buff,&CCFFKEY,l);
  for (i=0; i<l/4; i++) { ccffkeyD.set(i,buff[i]);}
  delete [] buff;

//trclffkey
  l = sizeof(TRCLFFKEY);
  trclffkeyD.resize(l/4);
  buff = new integer[l/4];
  memcpy(buff,&TRCLFFKEY,l);
  for (i=0; i<l/4; i++) { trclffkeyD.set(i,buff[i]);}
  delete [] buff;

}

ooStatus AMScommonsD::CmpConstants(integer Write) {

//
// Write == AMSFFKEY.Write
//
 ooStatus rstatus = oocSuccess;
 integer  i;
 integer  lm, ld;
 integer* buff;

  cout<<"AMScommonsD::CmpConstants -I- started "<<endl;

  cout<<"AMScommonsD::CmpConstants -I- TOFMCFFKEY"<<endl;
  lm = sizeof(TOFMCFFKEY);
  ld = tofmcffkeyD.size();
  if (lm/4 != ld) {
   cout <<"AMScommonsD::CmpConstants -W- TOFMCFFKEY and tofmcffkeyD size are"
        <<" not the same"<<endl;
   goto tofrecffkey;
  }
  buff = new integer[lm/4];
  memcpy(buff,&TOFMCFFKEY,lm);
  for (i=0; i<ld; i++) {
   if (buff[i] != tofmcffkeyD[i]) {
    cout<<"AMScommonsD::CmpConstants -W- TOFMCFFKEY element "<<i
        <<" is different "<<endl;
    }
  }
  delete [] buff;

tofrecffkey:
  cout<<"AMScommonsD::CmpConstants -I- TOFRECFFKEY"<<endl;
  lm = sizeof(TOFRECFFKEY);
  ld = tofrecffkeyD.size();
  if (lm/4 != ld) {
   cout <<"AMScommonsD::CmpConstants -W- TOFRECFFKEY and tofrecffkeyD size are"
        <<" not the same"<<endl;
   goto ctcmcffkey;
  }
  buff = new integer[lm/4];
  memcpy(buff,&TOFRECFFKEY,lm);
  for (i=0; i<ld; i++) {
   if (buff[i] != tofrecffkeyD[i]) {
    cout<<"AMScommonsD::CmpConstants -W- TOFRECFFKEY element "<<i
        <<" is different "<<endl;
    }
  }
  delete [] buff;

ctcmcffkey:
  cout<<"AMScommonsD::CmpConstants -I- CTCMCFFKEY"<<endl;
  lm = sizeof(CTCMCFFKEY);
  ld = ctcmcffkeyD.size();
  if (lm/4 != ld) {
   cout <<"AMScommonsD::CmpConstants -W- CTCMCFFKEY and ctcmcffkeyD size are"
        <<" not the same"<<endl;
   goto ctcgeomffkey;
  }
  buff = new integer[lm/4];
  memcpy(buff,&CTCMCFFKEY,lm);
  for (i=0; i<ld; i++) {
   if (buff[i] != ctcmcffkeyD[i]) {
    cout<<"AMScommonsD::CmpConstants -W- CTCMCFFKEY element "<<i
        <<" is different "<<endl;
    }
  }
  delete [] buff;

ctcgeomffkey:
  cout<<"AMScommonsD::CmpConstants -I- CTCGEOMFFKEY"<<endl;
  integer set_geom = 0;
  lm = sizeof(CTCGEOMFFKEY);
  ld = ctcgeomffkeyD.size();
  if (lm/4 != ld) {
   cout <<"AMScommonsD::CmpConstants -E- CTCGEOMFFKEY and ctcgeomffkeyD size"
        <<"  are not the same"<<endl;
   return oocError;
  }

   buff = new integer[lm/4];
   memcpy(buff,&CTCGEOMFFKEY,lm);
   for (i=0; i<ld; i++) {
    if (buff[i] != ctcgeomffkeyD[i]) {
     cout<<"AMScommonsD::CmpConstants -W- CTCGEOMFFKEY element "<<i
         <<" is different "<<endl;
     set_geom = 1;
     }
   }
   delete [] buff;

   if (Write != 0 && set_geom !=0 ) {
    cout<<"AMScommonsD::CmpConstants -E- please write new CTCGEOM"<<endl;
    set_geom = 0;
    return oocError;
   }
     
   if(set_geom != 0) {
    set_geom = 0;
    cout<<"AMScommonsD::CmpConstants -W- CTCGEOMFFKEY  are different "
        <<" and will be set to DBASE values"<<endl;
    buff = new integer[ld];
    for (i=0; i< ld; i++) {buff[i] = ctcgeomffkeyD[i]; }
    memcpy (&CTCGEOMFFKEY,buff,ld*4);
    delete [] buff;
   }

ctcrecffkey:
  cout<<"AMScommonsD::CmpConstants -I- TRMCFFKEY"<<endl;
  lm = sizeof(TRMCFFKEY);
  ld = trmcffkeyD.size();
  if (lm/4 != ld) {
   cout <<"AMScommonsD::CmpConstants -W- TRMCFFKEY and trmcffkeyD size are"
        <<" not the same"<<endl;
   goto betafitffkey;
  }
  buff = new integer[lm/4];
  memcpy(buff,&TRMCFFKEY,lm);
  for (i=0; i<ld; i++) {
   if (buff[i] != trmcffkeyD[i]) {
    cout<<"AMScommonsD::CmpConstants -W- TRMCFFKEY element "<<i
        <<" is different "<<endl;
    }
  }
  delete [] buff;

betafitffkey:
  cout<<"AMScommonsD::CmpConstants -I- BETAFITFFKEY"<<endl;
  lm = sizeof(BETAFITFFKEY);
  ld = betafitffkeyD.size();
  if (lm/4 != ld) {
   cout <<"AMScommonsD::CmpConstants -W- BETAFITFFKEY and betafitffkeyD "
        <<" size are not the same"<<endl;
   goto trfitffkey;
  }
  buff = new integer[lm/4];
  memcpy(buff,&BETAFITFFKEY,lm);
  for (i=0; i<ld; i++) {
   if (buff[i] != betafitffkeyD[i]) {
    cout<<"AMScommonsD::CmpConstants -W- BETAFITFFKEY element "<<i
        <<" is different "<<endl;
    }
  }
  delete [] buff;

trfitffkey:
  cout<<"AMScommonsD::CmpConstants -I- TRFITFFKEY"<<endl;
  lm = sizeof(TRFITFFKEY);
  ld = trfitffkeyD.size();
  if (lm/4 != ld) {
   cout <<"AMScommonsD::CmpConstants -W- TRFITFFKEY and trfitffkeyD "
        <<" size are not the same"<<endl;
   goto ccffkey;
  }
  buff = new integer[lm/4];
  memcpy(buff,&TRFITFFKEY,lm);
  for (i=0; i<ld; i++) {
   if (buff[i] != trfitffkeyD[i]) {
    cout<<"AMScommonsD::CmpConstants -W- TRFITFFKEY element "<<i
        <<" is different "<<endl;
    }
  }
  delete [] buff;

ccffkey:
  cout<<"AMScommonsD::CmpConstants -I- CCFFKEY"<<endl;
  lm = sizeof(CCFFKEY);
  ld = ccffkeyD.size();
  if (lm/4 != ld) {
   cout <<"AMScommonsD::CmpConstants -W- CCFFKEY and ccffkeyD "
        <<" size are not the same"<<endl;
   goto trclffkey;
  }
  buff = new integer[lm/4];
  memcpy(buff,&CCFFKEY,lm);
  for (i=0; i<ld; i++) {
   if (buff[i] != ccffkeyD[i]) {
    cout<<"AMScommonsD::CmpConstants -W- CCFFKEY element "<<i
        <<" is different "<<endl;
    }
  }
  delete [] buff;

trclffkey:
  cout<<"AMScommonsD::CmpConstants -I- TRCLFFKEY"<<endl;
  lm = sizeof(TRCLFFKEY);
  ld = trclffkeyD.size();
  if (lm/4 != ld) {
   cout <<"AMScommonsD::CmpConstants -W- TRCLFFKEY and trclffkeyD "
        <<" size are not the same"<<endl;
   goto cmpend;
  }
  buff = new integer[lm/4];
  memcpy(buff,&TRCLFFKEY,lm);
  for (i=0; i<ld; i++) {
   if (buff[i] != trclffkeyD[i]) {
    cout<<"AMScommonsD::CmpConstants -W- TRCLFFKEY element "<<i
        <<" is different "<<endl;
    }
  }
  delete [] buff;

cmpend:

  if (rstatus == oocError) 
      cout<<"AMScommonsD::CmpConstants -W- done, discrepancy found "<<endl;
  if (rstatus == oocSuccess) 
      cout<<"AMScommonsD::CmpConstants -I- done"<<endl;

return rstatus;
}

void AMScommonsD::CopyConstants() {

  integer  i;
  integer  l, ld;
  integer* buff;

  cout <<"AMScommonsD -I- CopyConstants "<<endl;
//tofmcffkey
  l = sizeof(TOFMCFFKEY);
  ld = tofmcffkeyD.size();
  ld = ccffkeyD.size();
  if (ld*4 != l) tofmcffkeyD.resize(l/4);
  buff = new integer[l/4];
  memcpy(buff,&TOFMCFFKEY,l);
  for (i=0; i<ld; i++) { tofmcffkeyD.set(i,buff[i]);}
  delete [] buff;

//tofrecffkey
  l = sizeof(TOFRECFFKEY);
  ld = tofrecffkeyD.size();
  if (ld*4 != l) tofrecffkeyD.resize(l/4);
  buff = new integer[l/4];
  memcpy(buff,&TOFRECFFKEY,l);
  for (i=0; i<ld; i++) { tofrecffkeyD.set(i,buff[i]);}
  delete [] buff;

//ctcmcffkey
  l = sizeof(CTCMCFFKEY);
  ld = ctcmcffkeyD.size();
  if(ld*4 != l) ctcmcffkeyD.resize(l/4);
  buff = new integer[l/4];
  memcpy(buff,&CTCMCFFKEY,l);
  for (i=0; i<ld; i++) { ctcmcffkeyD.set(i,buff[i]);}
  delete [] buff;

//ctcgeomffkey
  l = sizeof(CTCGEOMFFKEY);
  ld = ctcgeomffkeyD.size();
  if (ld*4 != l) ctcgeomffkeyD.resize(l/4);
  buff = new integer[l/4];
  memcpy(buff,&CTCGEOMFFKEY,l);
  for (i=0; i<ld; i++) { ctcgeomffkeyD.set(i,buff[i]);}
  delete [] buff;

//ctcrecffkey
  l = sizeof(CTCRECFFKEY);
  ld = ctcrecffkeyD.size();
  if (ld*4 != l) ctcrecffkeyD.resize(l/4);
  buff = new integer[l/4];
  memcpy(buff,&CTCRECFFKEY,l);
  for (i=0; i<ld; i++) { ctcrecffkeyD.set(i,buff[i]);}
  delete [] buff;

//trmcffkey
  l = sizeof(TRMCFFKEY);
  ld = trmcffkeyD.size();
  if (ld*4 != l) trmcffkeyD.resize(l/4);
  buff = new integer[l/4];
  memcpy(buff,&TRMCFFKEY,l);
  for (i=0; i<ld; i++) { trmcffkeyD.set(i,buff[i]);}
  delete [] buff;

//betafitffkey
  l = sizeof(BETAFITFFKEY);
  ld = betafitffkeyD.size();
  if (ld*4 != l) betafitffkeyD.resize(l/4);
  buff = new integer[l/4];
  memcpy(buff,&BETAFITFFKEY,l);
  for (i=0; i<ld; i++) { betafitffkeyD.set(i,buff[i]);}
  delete [] buff;

//trfitffkey
  l = sizeof(TRFITFFKEY);
  ld = trfitffkeyD.size();
  if (ld*4 != l) trfitffkeyD.resize(l/4);
  buff = new integer[l/4];
  memcpy(buff,&TRFITFFKEY,l);
  for (i=0; i<l/4; i++) { trfitffkeyD.set(i,buff[i]);}
  delete [] buff;

//ccffkey
  l = sizeof(CCFFKEY);
  ld = ccffkeyD.size();
  if (ld*4 != l) ccffkeyD.resize(l/4);
  buff = new integer[l/4];
  memcpy(buff,&CCFFKEY,l);
  for (i=0; i<l/4; i++) { ccffkeyD.set(i,buff[i]);}
  delete [] buff;

//trclffkey
  l = sizeof(TRCLFFKEY);
  ld = trclffkeyD.size();
  if (ld*4 != l) trclffkeyD.resize(l/4);
  buff = new integer[l/4];
  memcpy(buff,&TRCLFFKEY,l);
  for (i=0; i<l/4; i++) { trclffkeyD.set(i,buff[i]);}
  delete [] buff;
}
