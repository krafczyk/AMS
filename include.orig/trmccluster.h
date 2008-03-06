#ifndef __AMSTRMCCLUSTER__
#define __AMSTRMCCLUSTER__
#include "upool.h"
#include "apool.h"
#include "gsen.h"
#include "trid.h"
#include "link.h"
#include "commons.h"
#include <stdlib.h>

const integer _nel=5;
class AMSTrMCCluster: public AMSlink{
protected:
  integer _idsoft;
  integer _itra;
  integer _left[2];
  integer _center[2];
  integer _right[2];
  geant _ss[2][5];
  AMSPoint _xca;
  AMSPoint _xcb;
  AMSPoint _xgl;
  number   _sum;


  void _shower();
  void _printEl(ostream & stream);
  void _writeEl();
  void _copyEl();

  static integer Out(integer);
  static integer _hid[2];
  static integer _ncha;
  static geant _step; 
  static integer _NoiseMarker;
public:
  // Constructor for noise &crosstalk
  AMSTrMCCluster(const AMSTrIdGeom & id,integer side,integer nel, number ss[],
		 integer itra);
  // Constructor for geant track
  AMSTrMCCluster
  (integer idsoft, AMSPoint xca, AMSPoint xcb, AMSPoint xgl, geant sum,integer itra):
    _idsoft(idsoft),_xca(xca),_xcb(xcb),_xgl(xgl),_sum(sum),_itra(itra)
  {_shower();_next=0;}

  // Constructor for daq
  AMSTrMCCluster
  (AMSPoint xgl, integer itra):
    _idsoft(0),_xca(0,0,0),_xcb(0,0,0),_xgl(xgl),_sum(0),_itra(itra)
  {_next=0;
    for(int i=0;i<2;i++){
      _left[i]=0;
      _center[i]=0;
      _right[i]=0;
      for(int k=0;k<5;k++)_ss[i][k]=0;
    }
  }
  AMSTrMCCluster(){_next=0;};
  ~AMSTrMCCluster(){};
  AMSTrIdGeom getid();
  integer IsNoise(){return _itra==_NoiseMarker;}
  AMSPoint getHit(){return _xgl;}
  static integer noisemarker(){return _NoiseMarker;}
  static geant step(){return _step;}
  static integer ncha(){return _ncha;}
  static integer hid(integer l){return l<2?_hid[l]:0;}
  
  AMSTrMCCluster *  next(){return (AMSTrMCCluster*)_next;}
  void addcontent(char xy, geant ** p, integer noise=0);
  void addcontent(const AMSTrIdSoft & id, geant *adc);
  static integer debug;
  static void 
  sitkhits(integer idsoft , geant vect[],geant edep, geant step, 
	   integer itra );
  static void sitkcrosstalk();
  static void sitknoise();
  static number sitknoiseprob(const AMSTrIdSoft & id, number threshold);
  static number sitknoiseprobU(number threshold, number step);
  static void sitknoisespectrum(const AMSTrIdSoft & id, number ss[], number prob);


  // Interface with DAQ


  static int16u getdaqid(){return (15 <<9);}
  static integer checkdaqid(int16u id);
  static integer calcdaqlength(integer i);
  static integer getmaxblocks(){return 1;}
  static void builddaq(integer i, integer n, int16u *p);
  static void buildraw(integer n, int16u *p);

#ifdef __WRITEROOT__
  friend class  TrMCClusterR;
#endif


};


#endif
