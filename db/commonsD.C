//  $Id: commonsD.C,v 1.14 2001/01/22 17:32:27 choutko Exp $
// AMS common blocks method file
// Objectivity version Oct 08, 1996 ak
// Oct  14, 1996. ak. don't compare ccffkey run
// June 06, 1997. ak. add anti, no geom overwriting.
// Sep  16, 1997. ak. add tkfield
//
// Last Edit : Sep 16, 1997. ak.
//

#include <typedefs.h>
#include <rd45.h>
#include <commonsD.h>


AMScommonsD::AMScommonsD() {
  integer  i;
  integer  l;
  integer* buff;

  cout <<"AMScommonsD -I- constructor "<<endl;
//antigeomffkey
  l = sizeof(ANTIGEOMFFKEY);
  antigeomffkey.resize(l/4);
  buff = new integer[l/4];
  memcpy(buff,&ANTIGEOMFFKEY,l);
  for (i=0; i<l/4; i++) { antigeomffkey.set(i,buff[i]);}
  delete [] buff;

//antimcffkey
  l = sizeof(ANTIMCFFKEY);
  antimcffkey.resize(l/4);
  buff = new integer[l/4];
  memcpy(buff,&ANTIMCFFKEY,l);
  for (i=0; i<l/4; i++) { antimcffkey.set(i,buff[i]);}
  delete [] buff;

//antirecffkey
  l = sizeof(ANTIRECFFKEY);
  antirecffkey.resize(l/4);
  buff = new integer[l/4];
  memcpy(buff,&ANTIRECFFKEY,l);
  for (i=0; i<l/4; i++) { antirecffkey.set(i,buff[i]);}
  delete [] buff;


//tofcaffkey
  l = sizeof(TOFCAFFKEY);
  tofcaffkey.resize(l/4);
  buff = new integer[l/4];
  memcpy(buff,&TOFCAFFKEY,l);
  for (i=0; i<l/4; i++) { tofcaffkey.set(i,buff[i]);}
  delete [] buff;

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

//lvl1ffkey
  l = sizeof(LVL1FFKEY);
  lvl1ffkey.resize(l/4);
  buff = new integer[l/4];
  memcpy(buff,&LVL1FFKEY,l);
  for (i=0; i<l/4; i++) { lvl1ffkey.set(i,buff[i]);}
  delete [] buff;

//lvl3ffkey
  l = sizeof(LVL3FFKEY);
  lvl3ffkey.resize(l/4);
  buff = new integer[l/4];
  memcpy(buff,&LVL3FFKEY,l);
  for (i=0; i<l/4; i++) { lvl3ffkey.set(i,buff[i]);}
  delete [] buff;

//lvl3expffkey
  l = sizeof(LVL3EXPFFKEY);
  lvl3expffkey.resize(l/4);
  buff = new integer[l/4];
  memcpy(buff,&LVL3EXPFFKEY,l);
  for (i=0; i<l/4; i++) { lvl3expffkey.set(i,buff[i]);}
  delete [] buff;

//lvl3simffkey
  l = sizeof(LVL3SIMFFKEY);
  lvl3simffkey.resize(l/4);
  buff = new integer[l/4];
  memcpy(buff,&LVL3SIMFFKEY,l);
  for (i=0; i<l/4; i++) { lvl3simffkey.set(i,buff[i]);}
  delete [] buff;

//tkfield
  l = sizeof(TKFIELD);
  tkfield.resize(l/4);
  buff = new integer[l/4];
  memcpy(buff,&TKFIELD,l);
  for (i=0; i<l/4; i++) { tkfield.set(i,buff[i]);}
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

//trcalib
  l = sizeof(TRCALIB);
  trcalibD.resize(l/4);
  buff = new integer[l/4];
  memcpy(buff,&TRCALIB,l);
  for (i=0; i<l/4; i++) { trcalibD.set(i,buff[i]);}
  delete [] buff;

//chargefitffkey
  l = sizeof(CHARGEFITFFKEY);
  chargefitffkeyD.resize(l/4);
  buff = new integer[l/4];
  memcpy(buff,&CHARGEFITFFKEY,l);
  for (i=0; i<l/4; i++) { chargefitffkeyD.set(i,buff[i]);}
  delete [] buff;

}

ooStatus AMScommonsD::CmpConstants(integer Write) {

//
// Write - if read geometry from DBASE or geometry already exists, then 
//         overwrite geom commons in memory,
//         if they are different from the dbase ones. 
//
 ooStatus rstatus = oocSuccess;
 integer  i;
 integer  lm, ld;
 integer* buff;
 integer  ndif;    // number of differencies

  cout<<"AMScommonsD::CmpConstants -I- started "<<endl;

  ld = tofmcffkeyD.size();
  cout<<"AMScommonsD::CmpConstants -I- TOFMCFFKEY, size "<<ld<<" int words"
      <<endl;
  lm = sizeof(TOFMCFFKEY);
  ld = tofmcffkeyD.size();
  if (lm/4 != ld) {
   cout <<"AMScommonsD::CmpConstants -W- TOFMCFFKEY and tofmcffkeyD size are"
        <<" not the same. Ld (integer) "<<ld<<", Lm "<<lm/4<<endl;
   goto tofrecffkey;
  }
  buff = new integer[lm/4];
  memcpy(buff,&TOFMCFFKEY,lm);
  for (i=0; i<ld; i++) {
   if (buff[i] != tofmcffkeyD[i]) {
    cout<<"AMScommonsD::CmpConstants -W- TOFMCFFKEY element "<<i
        <<" is different "<<buff[i]<<", "<<tofmcffkeyD[i]<<endl;
    }
  }
  delete [] buff;

tofrecffkey:
  ld = tofrecffkeyD.size();
  cout<<"AMScommonsD::CmpConstants -I- TOFRECFFKEY, size "<<ld<<" int words"
      <<endl;
  lm = sizeof(TOFRECFFKEY);
  if (lm/4 != ld) {
   cout <<"AMScommonsD::CmpConstants -W- TOFRECFFKEY and tofrecffkeyD size are"
        <<" not the same. Ld (integer) "<<ld<<", Lm "<<lm/4<<endl;
   goto ctcmcffkey;
  }
  buff = new integer[lm/4];
  memcpy(buff,&TOFRECFFKEY,lm);
  for (i=0; i<ld; i++) {
   if (buff[i] != tofrecffkeyD[i]) {
    cout<<"AMScommonsD::CmpConstants -W- TOFRECFFKEY element "<<i
        <<" is different "<<buff[i]<<", "<<tofrecffkeyD[i]<<endl;
    }
  }
  delete [] buff;

ctcmcffkey:
  ld = ctcmcffkeyD.size();
  cout<<"AMScommonsD::CmpConstants -I- CTCMCFFKEY, size "<<ld<<" int words"
      <<endl;
  lm = sizeof(CTCMCFFKEY);
  if (lm/4 != ld) {
   cout <<"AMScommonsD::CmpConstants -W- CTCMCFFKEY and ctcmcffkeyD size are"
        <<" not the same. Ld (integer) "<<ld<<", Lm "<<lm/4<<endl;
   goto ctcgeomffkey;
  }
  buff = new integer[lm/4];
  memcpy(buff,&CTCMCFFKEY,lm);
  for (i=0; i<ld; i++) {
   if (buff[i] != ctcmcffkeyD[i]) {
    cout<<"AMScommonsD::CmpConstants -W- CTCMCFFKEY element "<<i
        <<" is different "<<buff[i]<<", "<<ctcmcffkeyD[i]<<endl;
    }
  }
  delete [] buff;

ctcgeomffkey:
  ld = ctcgeomffkeyD.size();
  cout<<"AMScommonsD::CmpConstants -I- CTCGEOMFFKEY, size "<<ld
      <<" int words"<<endl;
  integer set_geom = 0;
  lm = sizeof(CTCGEOMFFKEY);
  if (lm/4 != ld) {
   cout <<"AMScommonsD::CmpConstants -E- CTCGEOMFFKEY and ctcgeomffkeyD size"
        <<"  are not the same. Ld (integer) "<<ld<<", Lm "<<lm/4<<endl;
   return oocError;
  }

   buff = new integer[lm/4];
   memcpy(buff,&CTCGEOMFFKEY,lm);
   for (i=0; i<ld; i++) {
    if (buff[i] != ctcgeomffkeyD[i]) {
     cout<<"AMScommonsD::CmpConstants -W- CTCGEOMFFKEY element "<<i
         <<" is different "<<buff[i]<<", "<<ctcgeomffkeyD[i]<<endl;
     set_geom = 1;
     }
   }
   delete [] buff;

   //if (Write != 0 && set_geom !=0 ) {
   if(set_geom) {
    Error("AMScommonsD::CmpConstants: please write new CTCGEOM. Quit");
    set_geom = 0;
    return oocError;
   }
     
   /*
   if(set_geom != 0) {
    set_geom = 0;
    Warning
    ("AMScommonsD::CmpConstants : CTCGEOMFFKEY  will be set to DBASE values");
    buff = new integer[ld];
    for (i=0; i< ld; i++) {buff[i] = ctcgeomffkeyD[i]; }
    memcpy (&CTCGEOMFFKEY,buff,ld*4);
    delete [] buff;
   }
   */
ctcrecffkey:
  ld = trmcffkeyD.size();
  cout
   <<"AMScommonsD::CmpConstants -I- TRMCFFKEY, size "<<ld<<" int words"<<endl;
  lm = sizeof(TRMCFFKEY);
  if (lm/4 != ld) {
   cout <<"AMScommonsD::CmpConstants -W- TRMCFFKEY and trmcffkeyD size are"
        <<" not the same. Ld (integer) "<<ld<<", Lm "<<lm/4<<endl;
   goto betafitffkey;
  }
  buff = new integer[lm/4];
  memcpy(buff,&TRMCFFKEY,lm);
  for (i=0; i<ld; i++) {
   if (buff[i] != trmcffkeyD[i]) {
    cout<<"AMScommonsD::CmpConstants -W- TRMCFFKEY element "<<i
        <<" is different "<<buff[i]<<", "<<trmcffkeyD[i]<<endl;
    }
  }
  delete [] buff;

betafitffkey:
  ld = betafitffkeyD.size();
  cout<<"AMScommonsD::CmpConstants -I- BETAFITFFKEY, size "<<ld
      <<" int words"<<endl;
  lm = sizeof(BETAFITFFKEY);
  if (lm/4 != ld) {
   cout <<"AMScommonsD::CmpConstants -W- BETAFITFFKEY and betafitffkeyD "
        <<" size are not the same. Ld (integer) "<<ld<<", Lm "<<lm/4<<endl;
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
  ld = trfitffkeyD.size();
  cout<<"AMScommonsD::CmpConstants -I- TRFITFFKEY, size "<<ld
      <<" int words"<<endl;
  lm = sizeof(TRFITFFKEY);
  if (lm/4 != ld) {
   cout <<"AMScommonsD::CmpConstants -W- TRFITFFKEY and trfitffkeyD "
        <<" size are not the same. Ld (integer) "<<ld<<", Lm "<<lm/4<<endl;
   goto ccffkey;
  }
  buff = new integer[lm/4];
  memcpy(buff,&TRFITFFKEY,lm);
  for (i=0; i<ld; i++) {
   if (buff[i] != trfitffkeyD[i]) {
    cout<<"AMScommonsD::CmpConstants -W- TRFITFFKEY element "<<i
        <<" is different "<<buff[i]<<", "<<trfitffkeyD[i]<<endl;
    }
  }
  delete [] buff;

ccffkey:
  ld = ccffkeyD.size();
  cout<<"AMScommonsD::CmpConstants -I- CCFFKEY, size "<<ld
      <<" int words"<<endl;
  lm = sizeof(CCFFKEY);
  if (lm/4 != ld) {
   cout <<"AMScommonsD::CmpConstants -W- CCFFKEY and ccffkeyD "
        <<" size are not the same. Ld (integer) "<<ld<<", Lm "<<lm/4<<endl;
   goto trclffkey;
  }
  buff = new integer[lm/4];
  memcpy(buff,&CCFFKEY,lm);
  for (i=0; i<ld; i++) {
   if (buff[i] != ccffkeyD[i]) {
    cout<<"AMScommonsD::CmpConstants -W- CCFFKEY element "<<i
        <<" is different "<<buff[i]<<", "<<ccffkeyD[i]<<endl;
    }
  }
  delete [] buff;

trclffkey:
  ld = trclffkeyD.size();
  cout<<"AMScommonsD::CmpConstants -I- TRCLFFKEY, size "<<ld
      <<" int words"<<endl;
  lm = sizeof(TRCLFFKEY);
  if (lm/4 != ld) {
   cout <<"AMScommonsD::CmpConstants -W- TRCLFFKEY and trclffkeyD "
        <<" size are not the same. Ld (integer) "<<ld<<", Lm "<<lm/4<<endl;
   goto trcalib;
  }
  buff = new integer[lm/4];
  memcpy(buff,&TRCLFFKEY,lm);
  for (i=0; i<ld; i++) {
   if (buff[i] != trclffkeyD[i]) {
    cout<<"AMScommonsD::CmpConstants -W- TRCLFFKEY element "<<i
        <<" is different "<<buff[i]<<", "<<trclffkeyD[i]<<endl;
    }
  }
  delete [] buff;

trcalib:
  ld = trcalibD.size();
  cout<<"AMScommonsD::CmpConstants -I- TRCALIB size "<<ld
      <<" int words"<<endl;
  lm = sizeof(TRCALIB);
  if (lm/4 != ld) {
   cout <<"AMScommonsD::CmpConstants -W- TRCALIB and trcalibD "
        <<" size are not the same. Ld (integer) "<<ld<<", Lm "<<lm/4<<endl;
   goto chargefitffkey;
  }
  buff = new integer[lm/4];
  memcpy(buff,&TRCALIB,lm);
  for (i=0; i<ld; i++) {
   if (buff[i] != trcalibD[i]) {
    cout<<"AMScommonsD::CmpConstants -W- TRCALIB element "<<i
        <<" is different "<<buff[i]<<", "<<trcalibD[i]<<endl;
    }
  }
  delete [] buff;

chargefitffkey:
  ld = chargefitffkeyD.size();
  cout<<"AMScommonsD::CmpConstants -I- CHARGEFITFFKEY, size "<<ld
      <<" int words"<<endl;
  lm = sizeof(CHARGEFITFFKEY);
  if (lm/4 != ld) {
   cout <<"AMScommonsD::CmpConstants -W- CHARGEFITFFKEY and chargefitffkeyD "
        <<" size are not the same. Ld (integer) "<<ld<<", Lm "<<lm/4<<endl;
   goto trigffkey;
  }
  buff = new integer[lm/4];
  memcpy(buff,&CHARGEFITFFKEY,lm);
  for (i=0; i<ld; i++) {
   if (buff[i] != chargefitffkeyD[i]) {
    cout<<"AMScommonsD::CmpConstants -W- CHARGEFITFFKEY element "<<i
        <<" is different "<<buff[i]<<", "<<chargefitffkeyD[i]<<endl;
    }
  }
  delete [] buff;

trigffkey:
  ld = lvl1ffkey.size();
  cout<<"AMScommonsD::CmpConstants -I- LVL1FFKEY, size "<<ld
      <<" int words"<<endl;
  lm = sizeof(LVL1FFKEY);
  if (lm/4 != ld) {
   cout <<"AMScommonsD::CmpConstants -W- LVL1FFKEY and lvl1ffkey "
        <<" size are not the same. Ld (integer) "<<ld<<", Lm "<<lm/4<<endl;
   goto lvl3;
  }
  buff = new integer[lm/4];
  memcpy(buff,&LVL1FFKEY,lm);
  for (i=0; i<ld; i++) {
   if (buff[i] != lvl1ffkey[i]) {
    cout<<"AMScommonsD::CmpConstants -W- LVL1FFKEY element "<<i
        <<" is different "<<buff[i]<<", "<<lvl1ffkey[i]<<endl;
    }
  }
  delete [] buff;
lvl3:
  ld = lvl3ffkey.size();
  cout<<"AMScommonsD::CmpConstants -I- LVL3FFKEY, size "<<ld
      <<" int words"<<endl;
  lm = sizeof(LVL3FFKEY);
  if (lm/4 != ld) {
   cout <<"AMScommonsD::CmpConstants -W- LVL3FFKEY and lvl3ffkey "
        <<" size are not the same. Ld (integer) "<<ld<<", Lm "<<lm/4<<endl;
   goto lvl3sim;
  }
  buff = new integer[lm/4];
  memcpy(buff,&LVL3FFKEY,lm);
  for (i=0; i<ld; i++) {
   if (buff[i] != lvl3ffkey[i]) {
    cout<<"AMScommonsD::CmpConstants -W- LVL3FFKEY element "<<i
        <<" is different "<<buff[i]<<", "<<lvl3ffkey[i]<<endl;
    }
  }
  delete [] buff;

lvl3sim:
  ld = lvl3simffkey.size();
  cout<<"AMScommonsD::CmpConstants -I- LVL3SIMFFKEY, size "<<ld
      <<" int words"<<endl;
  lm = sizeof(LVL3SIMFFKEY);
  if (lm/4 != ld) {
   cout <<"AMScommonsD::CmpConstants -W- LVL3SIMFFKEY and lvl3simffkey "
        <<" size are not the same. Ld (integer) "<<ld<<", Lm "<<lm/4<<endl;
   goto lvl3exp;
  }
  buff = new integer[lm/4];
  memcpy(buff,&LVL3SIMFFKEY,lm);
  for (i=0; i<ld; i++) {
   if (buff[i] != lvl3simffkey[i]) {
    cout<<"AMScommonsD::CmpConstants -W- LVL3SIMFFKEY element "<<i
        <<" is different "<<buff[i]<<", "<<lvl3simffkey[i]<<endl;
    }
  }
  delete [] buff;

lvl3exp:
  ld = lvl3expffkey.size();
  cout<<"AMScommonsD::CmpConstants -I- LVL3EXPFFKEY, size "<<ld
      <<" int words"<<endl;
  lm = sizeof(LVL3EXPFFKEY);
  if (lm/4 != ld) {
   cout <<"AMScommonsD::CmpConstants -W- LVL3SIMFFKEY and lvl3simffkey "
        <<" size are not the same. Ld (integer) "<<ld<<", Lm "<<lm/4<<endl;
   goto antigeom;
  }
  buff = new integer[lm/4];
  memcpy(buff,&LVL3EXPFFKEY,lm);
  for (i=0; i<ld; i++) {
   if (buff[i] != lvl3expffkey[i]) {
    cout<<"AMScommonsD::CmpConstants -W- LVL3EXPFFKEY element "<<i
        <<" is different "<<buff[i]<<", "<<lvl3expffkey[i]<<endl;
    }
  }
  delete [] buff;


antigeom:
  ld = antigeomffkey.size();
  cout<<"AMScommonsD::CmpConstants -I- antigeomffkey, size "<<ld
      <<" int words"<<endl;
  lm = sizeof(ANTIGEOMFFKEY);
  if (lm/4 != ld) {
   cout <<"AMScommonsD::CmpConstants -W- ANTIGEOMFFKEY and antigeomffkey "
        <<" size are not the same. Ld (integer) "<<ld<<", Lm "<<lm/4<<endl;
   return oocError;
  }

   buff = new integer[lm/4];
   memcpy(buff,&ANTIGEOMFFKEY,lm);
   for (i=0; i<ld; i++) { 
     if (buff[i] != antigeomffkey[i]) {
      cout<<"AMScommonsD::CmpConstants -W- ANTIGEOMFFKEY element "<<i
          <<" is different "<<buff[i]<<", "<<antigeomffkey[i]<<endl;
      set_geom = 1;
     }
   }
  delete [] buff;

  //if (Write != 0 && set_geom !=0 ) {
  if(set_geom) {
   Error("AMScommonsD::CmpConstants: please write new AntiGEOM. Quit");
   set_geom = 0;
   return oocError;
  }
   /*
  if(set_geom != 0) {
   set_geom = 0;
   Warning
    ("AMScommonsD::CmpConstants : AntiGEOM  will be set to DBASE values");
    buff = new integer[ld];
    for (i=0; i< ld; i++) {buff[i] = antigeomffkey[i]; }
    memcpy (&ANTIGEOMFFKEY,buff,ld*4);
    delete [] buff;
   }
   */
antimc:
  ld = antimcffkey.size();
  cout<<"AMScommonsD::CmpConstants -I- antimcffkey, size "<<ld
      <<" int words"<<endl;
  lm = sizeof(ANTIMCFFKEY);
  if (lm/4 != ld) {
   cout <<"AMScommonsD::CmpConstants -W- ANTIMCFFKEY and antimcffkey "
        <<" size are not the same. Ld (integer) "<<ld<<", Lm "<<lm/4<<endl;
   goto antirec;
  }
   buff = new integer[lm/4];
   memcpy(buff,&ANTIMCFFKEY,lm);
   for (i=0; i<ld; i++) { 
     if (buff[i] != antimcffkey[i]) {
      cout<<"AMScommonsD::CmpConstants -W- ANTIMCFFKEY element "<<i
          <<" is different "<<buff[i]<<", "<<antimcffkey[i]<<endl;
     }
   }
  delete [] buff;

antirec:
  ld = antirecffkey.size();
  cout<<"AMScommonsD::CmpConstants -I- antirecffkey, size "<<ld
      <<" int words"<<endl;
  lm = sizeof(ANTIRECFFKEY);
  if (lm/4 != ld) {
   cout <<"AMScommonsD::CmpConstants -W- ANTIRECFFKEY and antirecffkey "
        <<" size are not the same. Ld (integer) "<<ld<<", Lm "<<lm/4<<endl;
   goto tofca;
  }
   buff = new integer[lm/4];
   memcpy(buff,&ANTIRECFFKEY,lm);
   for (i=0; i<ld; i++) { 
     if (buff[i] != antirecffkey[i]) {
      cout<<"AMScommonsD::CmpConstants -W- ANTIRECFFKEY element "<<i
          <<" is different "<<buff[i]<<", "<<antirecffkey[i]<<endl;
     }
   }
  delete [] buff;

tofca:
  ld = tofcaffkey.size();
  cout<<"AMScommonsD::CmpConstants -I- tofcaffkey, size "<<ld
      <<" int words"<<endl;
  lm = sizeof(TOFCAFFKEY);
  if (lm/4 != ld) {
   cout <<"AMScommonsD::CmpConstants -W- TOFCAFFKEY and tofcaffkey "
        <<" size are not the same. Ld (integer) "<<ld<<", Lm "<<lm/4<<endl;
   goto tkfld;
  }
   buff = new integer[lm/4];
   memcpy(buff,&TOFCAFFKEY,lm);
   for (i=0; i<ld; i++) { 
     if (buff[i] != tofcaffkey[i]) {
      cout<<"AMScommonsD::CmpConstants -W- TOFCAFFKEY element "<<i
          <<" is different "<<buff[i]<<", "<<tofcaffkey[i]<<endl;
     }
   }
  delete [] buff;

tkfld:
  ld = tkfield.size();
  cout<<"AMScommonsD::CmpConstants -I- tkfield, size "<<ld
      <<" int words"<<endl;
  lm = sizeof(TKFIELD);
  if (lm/4 != ld) {
   cout <<"AMScommonsD::CmpConstants -W- TKFIELD and tkfield "
        <<" size are not the same. Ld (integer) "<<ld<<", Lm "<<lm/4<<endl;
   goto cmpend;
  }
   buff = new integer[lm/4];
   memcpy(buff,&TKFIELD,lm);
   ndif = 0;
   for (i=0; i<ld; i++) { 
     if (buff[i] != tkfield[i]) {
      ndif++;
      if (ndif > 20) {
       cout <<"AMScommonsD::CmpConstants -W- TKFIELD : stop future comparison" 
            <<" too many differencies"<<endl;
       break;
      }
      cout<<"AMScommonsD::CmpConstants -W- TKFIELD element "<<i
          <<" is different "<<buff[i]<<", "<<tkfield[i]<<endl;
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

//trcalib
  l = sizeof(TRCALIB);
  trcalibD.resize(l/4);
  buff = new integer[l/4];
  memcpy(buff,&TRCALIB,l);
  for (i=0; i<l/4; i++) { trcalibD.set(i,buff[i]);}
  delete [] buff;

//chargefitffkey
  l = sizeof(CHARGEFITFFKEY);
  chargefitffkeyD.resize(l/4);
  buff = new integer[l/4];
  memcpy(buff,&CHARGEFITFFKEY,l);
  for (i=0; i<l/4; i++) { chargefitffkeyD.set(i,buff[i]);}
  delete [] buff;

//trigffkey
  l = sizeof(LVL1FFKEY);
  lvl1ffkey.resize(l/4);
  buff = new integer[l/4];
  memcpy(buff,&LVL1FFKEY,l);
  for (i=0; i<l/4; i++) { lvl1ffkey.set(i,buff[i]);}
  delete [] buff;

  l = sizeof(LVL3FFKEY);
  lvl3ffkey.resize(l/4);
  buff = new integer[l/4];
  memcpy(buff,&LVL3FFKEY,l);
  for (i=0; i<l/4; i++) { lvl3ffkey.set(i,buff[i]);}
  delete [] buff;

  l = sizeof(LVL3SIMFFKEY);
  lvl3simffkey.resize(l/4);
  buff = new integer[l/4];
  memcpy(buff,&LVL3SIMFFKEY,l);
  for (i=0; i<l/4; i++) { lvl3simffkey.set(i,buff[i]);}
  delete [] buff;

//antigeomffkey
  l = sizeof(ANTIGEOMFFKEY);
  antigeomffkey.resize(l/4);
  buff = new integer[l/4];
  memcpy(buff,&ANTIGEOMFFKEY,l);
  for (i=0; i<l/4; i++) { antigeomffkey.set(i,buff[i]);}
  delete [] buff;

//antimcffkey
  l = sizeof(ANTIMCFFKEY);
  antimcffkey.resize(l/4);
  buff = new integer[l/4];
  memcpy(buff,&ANTIMCFFKEY,l);
  for (i=0; i<l/4; i++) { antimcffkey.set(i,buff[i]);}
  delete [] buff;

//antirecffkey
  l = sizeof(ANTIRECFFKEY);
  antirecffkey.resize(l/4);
  buff = new integer[l/4];
  memcpy(buff,&ANTIRECFFKEY,l);
  for (i=0; i<l/4; i++) { antirecffkey.set(i,buff[i]);}
  delete [] buff;


//tofcaffkey
  l = sizeof(TOFCAFFKEY);
  tofcaffkey.resize(l/4);
  buff = new integer[l/4];
  memcpy(buff,&TOFCAFFKEY,l);
  for (i=0; i<l/4; i++) { tofcaffkey.set(i,buff[i]);}
  delete [] buff;

//trcalib
  l = sizeof(TKFIELD);
  tkfield.resize(l/4);
  buff = new integer[l/4];
  memcpy(buff,&TKFIELD,l);
  for (i=0; i<l/4; i++) { tkfield.set(i,buff[i]);}
  delete [] buff;


}
