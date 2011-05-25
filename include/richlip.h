// LIP initialization class, added November 2010

#ifndef __RICHLIP__
#define __RICHLIP__ 


#include "job.h"
#include "commons.h" 
#include "mceventg.h"
#include "richrec.h"
#include "richradid.h"
#include "richdbc.h"
#include "tofrec02.h"
#ifdef __WRITEROOT__
#include "root.h" 
#endif


PROTOCCALLSFSUB2(RICHRECLIP,richreclip,INT,INT)
#define RICHRECLIP(I1,I2) CCALLSFSUB2(RICHRECLIP,richreclip,INT,INT,I1,I2)


class RichLIPRec{

 public:

  static int actual;
#pragma omp threadprivate(actual)
  static int totalhits;
#pragma omp threadprivate(totalhits)
  static int hitinlip[11000];
#pragma omp threadprivate(hitinlip)

  static void InitGlobal();
  static void InitEvent();
  static void InitTrack(AMSPoint, AMSDir);
  static int goodLIPREC();
  static void coordams2lip(float pxams, float pyams, float pzams, float &pxlip, float &pylip, float &pzlip);
  static void coordlip2ams(float pxlip, float pylip, float pzlip, float &pxams, float &pyams, float &pzams);
  static void angleams2lip(float theams, float phiams, float &thelip, float &philip);
  static void anglelip2ams(float thelip, float philip, float &theams, float &phiams);
  static void coordrich2lip(float pxrich, float pyrich, float pzrich, float &pxlip, float &pylip, float &pzlip);
  static void coordlip2rich(float pxlip, float pylip, float pzlip, float &pxrich, float &pyrich, float &pzrich);
  static void anglerich2lip(float therich, float phirich, float &thelip, float &philip);
  static void anglelip2rich(float thelip, float philip, float &therich, float &phirich);

};


#endif
