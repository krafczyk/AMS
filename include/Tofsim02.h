//  $Id: Tofsim02.h,v 1.2.2.1 2012/07/16 23:52:37 qyan Exp $
// Author Qi Yan. 2010.05.01.
#ifndef __NEWAMSTOF2SIM__
#define __NEWAMSTOF2SIM__
#include "upool.h"
#include "apool.h"
#include "link.h"
#include "typedefs.h"
#include "tofdbc02.h"
#include <stdlib.h>
#include <time.h>
#include <map>
#include "TH1D.h"
//===========================================================================
class TOF2TovtN: public AMSlink{
public:
 static map<integer,TH1D>phmap;//photon number map for bar 
 static map<integer,TH1D>::iterator phmapiter;
 static map<integer,TH1D>::iterator phmapitern;
 TOF2TovtN(){};
//
 ~TOF2TovtN(){};
 TOF2TovtN * next(){return (TOF2TovtN*)_next;}
 static void covtoph(integer idsoft, geant vect[], geant edep,geant tofg, geant tofdt,geant stepl,integer parentid);
 static void build();
 static geant pmsatur(const geant am,int ilay,int ibar,int is,int ipm=0);
 static void totovtn(integer id, geant edep, geant tslice1[][TOF2GC::SCTBMX+1]);//flash_ADC_array->Tovt
//
protected:
 void _printEl(ostream &stream){stream <<"TOF2TovtN: "<<endl;}
 void _writeEl(){};
 void _copyEl(){};
};
//=========================================================================== 
#endif
