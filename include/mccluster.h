// Author V. Choutko 24-may-1996
//
// June 12, 1996. ak. add set/getnumbers function to AMSTrMCCluster
// July 22, 1996. ak. the same for AMSTOFMCCluster
// Aug  07, 1996. ak. V124
// Sep  30, 1996. ak. V127
// Oct  04, 1996. ak _ContPos is moved to AMSLink
//
//

#ifndef __AMSMCCLUSTER__
#define __AMSMCCLUSTER__
#include <upool.h>
#include <apool.h>
#include <gsen.h>
#include <trid.h>
#include <link.h>
#include <commons.h>
#include <stdlib.h>



// MC bank for TOF & anti  - skeleton only

class AMSTOFMCCluster: public AMSlink{
public:
//+
#ifdef __DB__
   friend class AMSTOFMCClusterD;
#endif
//-

 integer  idsoft;
 AMSPoint xcoo;
 number   tof;
 number   edep;
 integer getid() const {return idsoft;}
 static integer Out(integer);

 
 AMSTOFMCCluster(integer _idsoft,AMSPoint _xcoo,number _edep, number _tof) :
 idsoft(_idsoft), xcoo(_xcoo),edep(_edep),tof(_tof){_next=0;};
 AMSTOFMCCluster(){_next=0;};
 ~AMSTOFMCCluster(){};
 void _printEl(ostream &stream){stream <<"AMSTOFMCCluster "<<idsoft<<" "<<edep<<" "<<xcoo<<endl;}
 void _writeEl();
 void _copyEl(){};
 integer operator < (AMSlink & o)const{
   return idsoft < ((AMSTOFMCCluster*)(&o)) ->idsoft ;
 }
 static void sitofhits(integer idsoft , geant vect[],geant edep, geant tofg);
  AMSTOFMCCluster *  next(){return (AMSTOFMCCluster*)_next;}
};
class AMSAntiMCCluster: public AMSlink{
public:

#ifdef __DB__
   friend class AMSAntiMCClusterD;
#endif

 AMSPoint xcoo;

 number tof;
 number edep;

 integer  idsoft;

 AMSAntiMCCluster(integer _idsoft,AMSPoint _xcoo,number _edep, number _tof) :
 idsoft(_idsoft), xcoo(_xcoo),edep(_edep),tof(_tof){_next=0;};
 AMSAntiMCCluster(){_next=0;};
 ~AMSAntiMCCluster(){};
 void _printEl(ostream &stream){stream <<"AMSAntiMCCluster "<<idsoft<<" "<<edep<<endl;}
 void _writeEl(){};
 void _copyEl(){};
 static void siantihits(integer idsoft , geant vect[],geant edep, geant tofg);
  AMSAntiMCCluster *  next(){return (AMSAntiMCCluster*)_next;}
};

// MC bank for CTC

class AMSCTCMCCluster: public AMSlink{
protected:

 integer _idsoft;   // barnumber +(1-if agel;2-if wls)*100
 AMSPoint _xcoo;    // Coordinates of input point
 AMSDir   _xdir;    // dir cos
 number   _step;    // step
 number   _charge;  // charge
 number   _beta;    // particle velocity
 number   _edep;    // energy deposition
 void _writeEl();
 void _copyEl(){};
 void _printEl(ostream &stream){stream <<"AMSCTCMCCluster "<<_idsoft<<" "
 <<_charge<<" "<<_xcoo<<" "<<_step<<" "<<_beta<<" "<<_edep<<endl;}
 static integer Out(integer);
public:
 AMSCTCMCCluster(integer idsoft, AMSPoint xcoo, AMSDir xdir, 
                 number charge, number step, number beta, number edep) :
 _idsoft(idsoft), _xcoo(xcoo), _xdir(xdir), _charge(charge),_step(step), 
 _beta(beta),_edep(edep){_next=0;};
 AMSCTCMCCluster(integer idsoft, AMSPoint xcoo, 
                 number charge, number step, number beta, number edep) :
 _idsoft(idsoft), _xcoo(xcoo), _charge(charge),_step(step), 
 _beta(beta), _edep(edep){_next=0;};
  AMSCTCMCCluster(){_next=0;};
 ~AMSCTCMCCluster(){};
 inline integer getid() const{return _idsoft;}
 inline integer getbarno() const{ return _idsoft%1000;}
 inline integer getdetno() const{ return _idsoft/10000;}
 inline integer getlayno() const{ return (_idsoft/1000)%10;}
 inline number  getbeta() const { return _beta;}
 inline number  getstep() const { return _step;}
 inline number  getedep() const { return _edep;}
 inline number  getcharge2() const { return _charge*_charge;}
 inline AMSPoint  getcoo() const { return _xcoo;}
 inline AMSDir  getdir() const { return _xdir;}
 integer operator < (AMSlink & o)const{
   return _idsoft < ((AMSCTCMCCluster*)(&o)) ->_idsoft ;
 }
 static void sictchits(integer idsoft , geant vect[],geant charge, geant step,
 geant getot, geant edep);
  AMSCTCMCCluster *  next(){return (AMSCTCMCCluster*)_next;}
//+
#ifdef __DB__
   friend class AMSCTCMCClusterD;
#endif
//-
};



// MC  bank for tracker

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
  
public:
 // Constructor for noise &crosstalk
 AMSTrMCCluster(const AMSTrIdGeom & id,integer side,integer nel, number ss[],
 integer itra);
 // Constructor for geant track
 AMSTrMCCluster
 (integer idsoft, AMSPoint xca, AMSPoint xcb, AMSPoint xgl, geant sum,integer itra):
  _idsoft(idsoft),_xca(xca),_xcb(xcb),_xgl(xgl),_sum(sum),_itra(itra)
 {_shower();_next=0;}
 AMSTrMCCluster(){_next=0;};
 ~AMSTrMCCluster(){};
  AMSTrIdGeom getid();
  AMSTrMCCluster *  next(){return (AMSTrMCCluster*)_next;}
  void addcontent(char xy, geant ** p);
  static integer debug;
 static void 
 sitkhits(integer idsoft , geant vect[],geant edep, geant step, 
 integer itra );
 static void sitkcrosstalk();
 static void sitknoise();
 static number sitknoiseprob(const AMSTrIdSoft & id, number threshold);
 static void sitknoisespectrum(const AMSTrIdSoft & id,integer nch,
 number ss[]);
//+
#ifdef __DB__
   friend class AMSTrMCClusterD;
#endif
//-
};
#endif
