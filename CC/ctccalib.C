//  $Id: ctccalib.C,v 1.3 2001/01/22 17:32:19 choutko Exp $
#include <point.h>
#include <typedefs.h>
#include <event.h>
#include <amsgobj.h>
#include <commons.h>
#include <cern.h>
#include <mccluster.h>
#include <math.h>
#include <extC.h>
#include <trrec.h>
#include <ctccalib.h>
#include <ctcdbc.h>
#include <particle.h>
#include <iostream.h>
#include <fstream.h>
#include <iomanip.h>
#include <time.h>
//
number CTCGAINcalib::cresp[CTCCCMX];
number CTCGAINcalib::crgain[CTCCCMX];
integer CTCGAINcalib::cevent[CTCCCMX];
//
void CTCGAINcalib::init(){
  int i,j;
  for(i=0;i<CTCCCMX;i++){
    cresp[i]=0.;
    crgain[i]=0.;
    cevent[i]=0;
  }
}
//
void CTCGAINcalib::select(){
  int il,ic,ir;
  number signal;
  AMSCTCRawHit *ptr;
//
  for(il=0;il<CTCDBc::getnlay();il++){// <--container loop
//
    ptr=(AMSCTCRawHit*)AMSEvent::gethead()->getheadC("AMSCTCRawHit",il);
    while (ptr){// <--hit loop
//
      signal=ptr->getsignal();
      ptr=ptr->next();
    }
  }
}
