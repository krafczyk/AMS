//  $Id: tralig.h,v 1.30 2011/09/28 07:50:05 choutko Exp $
// Author V. Choutko 13-nov-1998

#ifndef __AMSTRALIG__
#define __AMSTRALIG__
#include <typedefs.h>
#include  <commons.h>
#include <link.h>
#include <point.h>
#include <particle.h>
#include <trid.h>
#include <mceventg.h>
using namespace amsprotected;
class AMSTrAligDB;
class AMSTrAligPar{
public:
class AMSTrAligDBEntry{
public:
 uintl address;
 uinteger status;  // 0 if implicit
                   // 1++ if explicit
 
 geant coo[trconst::maxlay][3];
 geant angle[trconst::maxlay][3];
 geant chi2b; // before
 geant chi2a; // after
 geant pav;       // rel pmom
 geant pav2;      // pmom sigma 
 int operator < (const AMSTrAligDBEntry&o) const{ return address<o.address;}
 int operator == (const AMSTrAligDBEntry&o) const{ return address==o.address;}
 AMSTrAligDBEntry():address(0,0){};
 AMSTrAligDBEntry(uintl _address):address(_address){};
 AMSTrAligDBEntry(uintl _address,  uinteger _status, AMSTrAligPar o[trconst::maxlay], number _fcnb, number _fcna, number _pav, number _pav2):address(_address),status(_status),chi2b(_fcnb), chi2a(_fcna),pav(_pav),pav2(_pav2){
  for(int i=0;i<trconst::maxlay;i++){
   for(int k=0;k<3;k++){
    coo[i][k]=o[i].getcoo()[k];
    angle[i][k]=o[i].getang()[k];
   }
  }
}
};

class AMSTrAligDB{
 public:
 uinteger Nentries;
 AMSTrAligPar::AMSTrAligDBEntry arr[15000];
AMSTrAligDB():Nentries(0){};
};

protected:
integer  _NEntries;
AMSPoint _Coo;
AMSPoint _Angles;
AMSDir _Dir[3];
void _a2m();
void _m2a();
static AMSTrAligDB _traldb;
static AMSTrAligPar par[trconst::maxlay];
public:
AMSTrAligPar():_Coo(0,0,0),_Angles(0,0,0),_NEntries(0){};
AMSTrAligPar(const AMSPoint & coo, const AMSPoint & angles);
AMSTrAligPar(const AMSTrAligDBEntry * ptr, integer i);
AMSPoint  getcoo()const {return _Coo;}
AMSPoint  getang()const {return _Angles;}
void setcoo(const AMSPoint & o) {_Coo=o;}
void setpar(const AMSPoint & coo, const AMSPoint & angle);
integer& NEntries(){return _NEntries;}
integer      AddOne();
void         MinusOne();
AMSDir   getmtx(integer i){assert(i>=0 && i<3);return _Dir[i];}
AMSDir &  setmtx(integer i){assert(i>=0 && i<3);return _Dir[i];}
void updmtx(){_a2m();}
void updangles(){_m2a();}
static AMSID getTDVDB();
AMSTrAligPar  operator +(const AMSTrAligPar &o){
return AMSTrAligPar(_Coo+o._Coo,_Angles+o._Angles);
}         
AMSTrAligPar  operator -(const AMSTrAligPar &o){
return AMSTrAligPar(_Coo-o._Coo,_Angles-o._Angles);
}         
AMSTrAligPar  operator *(const AMSTrAligPar &o){
return AMSTrAligPar(_Coo*o._Coo,_Angles*o._Angles);
}         
AMSTrAligPar  operator /(number o){
return AMSTrAligPar(_Coo/o,_Angles/o);
}
static integer  getdbentries(){return  _traldb.Nentries;}
static void  incdbentries(){  _traldb.Nentries++;}
static integer maxdbentries(){return 15000;}
static AMSTrAligDBEntry * getdbtopp(){return _traldb.arr;}
static AMSTrAligPar * getparp(){return par;}
static void InitDB(){_traldb.Nentries=0;}
static AMSTrAligPar * SearchDB(uintl address, integer & found, number DB[2]);
static void UpdateDB(uintl address,  AMSTrAligPar o[], number _fcnb, number _fcna, number _pav, number _pav2  );
static void LockDB();
static void UnlockDB();
static void _lockunlock(integer lock);
static integer DbIsNotLocked(integer sleep=2);
static AMSTrAligDB * gettraligdbp(){ return & _traldb;}
static integer gettraligdbsize(){ return sizeof(_traldb);}




friend ostream &operator << (ostream &o, const  AMSTrAligPar &b )
  {return o<<" "<<b._Coo<<" "<<b._Angles<<" "<<b._Dir[0]<<" "<<b._Dir[1]<<" "<<b._Dir[2];}
};




class AMSTrAligData{
protected:
  integer _NHits;
  integer _Pattern;
  uint128 _Address;
  AMSPoint * _Hits;
  AMSPoint *_CooA;
  AMSPoint * _EHits;
  integer _Pid;
  geant _InvRigidity;
  geant _ErrInvRigidity;
  
public:
AMSTrAligData():_NHits(0),_Hits(0),_EHits(0),_Pid(0),
_InvRigidity(0),_ErrInvRigidity(0), _Pattern(0), _Address(0,0){};
void Init(AMSTrTrack * ptr, AMSmceventg * pgen);
void Init(integer patter, uint128 address, AMSPoint hit[],AMSPoint ehit[],AMSPoint cooa[], geant mcrig=0);
friend class AMSTrAligFit;
~AMSTrAligData(){ delete [] _Hits; delete[] _EHits; delete[] _CooA;}
};


class AMSTrAligFit: public AMSNode{



class TrAlig_def{
public:
integer Pattern;
integer Alg;
integer Address[2];
geant FCN;
geant FCNI;
geant Pfit;
geant Pfitsig;
geant Coo[trconst::maxlad][3];
geant Angle[trconst::maxlad][3];
};
class gldb_def{
public:
 integer nentries;
 geant coo[3];
 geant ang[3];
 AMSPoint getcoo(){return AMSPoint(coo);}
 AMSPoint getang(){return AMSPoint(ang);}
};
class TrAligg_def{
public:
integer Alg;
int Layer;
int Ladder;
int Sensor;
int Side;
geant FCN;
geant FCNI;
geant CHI2[1000];
geant CHI2I[1000];
geant rigra[1000];
int ndata;
geant Pfit;
geant Pfitsig;
geant Coo[3];
geant Angle[3];
integer Stat;
};
protected:
uint128 _Address;
integer  _Pattern; 
integer* _PlaneNo;
integer* _ParNo;
integer* _LadderNo;
integer* _HalfNo;
integer _NoActivePar;
integer _PositionData;
integer _NData;
integer _Algorithm;
AMSTrAligData * _pData;
integer _flag;    // 
number _tmp;
number _tmppav;
number _tmppsi;
number _fcn;   // pointer to fcns;
number _fcnI;   // pointer to fcns;
number _pfit;  //pointer to fitterd mom
number _pfitbefore;  //pointer to fitterd mom before
number _pfits;  //pointer to fitterd mom sigma
number chi2[1000];
number chi2i[1000];
number rigra[1000];
number _Chi2Max;
AMSTrAligPar _pParC[trconst::maxlad];
static AMSTrAligPar _pPargl[trconst::maxsen][trconst::maxlad][2][trconst::maxlay];
static gldb_def _gldb[trconst::maxsen+1][trconst::maxlad+1][2][trconst::maxlay];
static gldb_def _gldbP[trconst::maxsen+1][trconst::maxlad+1][2][trconst::maxlay];
static gldb_def _antigldb[trconst::maxsen+1][trconst::maxlad+1][2][trconst::maxlay];
static gldb_def _antigldbP[trconst::maxsen+1][trconst::maxlad+1][2][trconst::maxlay];
static void monit(number & a, number & b,number sim[], int & n, int & s, int & ncall)
{};
static void alfun(integer & n, number xc[], number & fc, AMSTrAligFit * ptr);
static void alfungl(integer & n, number xc[], number & fc, AMSTrAligFit * ptr);
static void UpdateDBgl();
static void ReadDBgl();
void _init(){};
public:
  AMSTrAligFit *  next(){return (AMSTrAligFit*)_next;}           
AMSTrAligFit();
AMSTrAligFit(uint128 _Address, integer pattern, integer data, integer alg, integer nodeno);
static AMSTrAligPar * SearchAntiDBgl(AMSTrIdGeom*pid,bool anti=true);
static void rewrite();
static const char * GetAligString();
static gldb_def * gettraliggldbp(){ return &(_gldb[0][0][0][0]);}
static gldb_def * gettraliggladbp(){ return &(_antigldb[0][0][0][0]);}
static gldb_def * gettraliggldbPp(){ return &(_gldbP[0][0][0][0]);}
static gldb_def * gettraliggladbPp(){ return &(_antigldbP[0][0][0][0]);}
static integer gettraliggldbsize(){return sizeof(_gldb);}
static integer gettraliggladbsize(){return sizeof(_antigldb);}
static integer gettraliggldbPsize(){return sizeof(_gldbP);}
static integer gettraliggladbPsize(){return sizeof(_antigldbP);}
static void InitDB();
static void InitADB();
static void Test(int i=0){};
static void Testgl(int i=0){};
static AMSID getTDVGLDB();
static AMSID getTDVAGLDB();
static AMSID getTDVGLDBP();
static AMSID getTDVAGLDBP();
static integer Select(AMSParticle * & ptr, AMSmceventg * & mcg, integer alg);
integer AddressOK(uintl address, integer strict=0);
void Fit();
void Fitgl(){};
static bool Fillgl(AMSNode *pal);
static bool Fillgle(AMSNode *pal);
void RebuildNoActivePar();
void Anal(){};
void Analgl();

uint128 getaddress(){ return _Address;}
~AMSTrAligFit();

};





#endif
