//  $Id: ecalrec.h,v 1.15 2001/09/11 12:57:06 choumilo Exp $
//
// 28.09.1999 E.Choumilov
//
#ifndef __AMSECALREC__
#define __AMSECALREC__
#include <typedefs.h>
#include <point.h>
#include <link.h>
#include <event.h>
#include <stdlib.h>
#include <time.h>
#include <ecaldbc.h>
//
//---------------------------------------
class AMSEcalRawEvent: public AMSlink{
private:
  static uinteger trigfl; // =1/2/3... -> "mip/low/high, =0->no trig.
  static number trigtm; // ECAL FT abs. time
  integer _idsoft; //readout cell ID=SSPPC (SS->S-layer,PP->PMcell, C->SubCell in PMcell)
  integer _padc[2];// Pulse hight (ADC-channels)[HighGain,LowGain]
public:
  AMSEcalRawEvent(integer idsoft, integer status,  
        integer padc[2]):AMSlink(status,0),_idsoft(idsoft)
  {for(int i=0;i<2;i++)_padc[i]=padc[i];};
//
  ~AMSEcalRawEvent(){};
  AMSEcalRawEvent * next(){return (AMSEcalRawEvent*)_next;}
//
  integer operator < (AMSlink & o)const{
                return _idsoft<((AMSEcalRawEvent*)(&o))->_idsoft;}
//
  integer getid() const {return _idsoft;}
  void getpadc(integer padc[2]){for(int i=0;i<2;i++)padc[i]=_padc[i];}
//
  static void mc_build(int &stat);
  static void validate(int &stat);
  static void settrfl(uinteger trfl){trigfl=trfl;}
  static uinteger gettrfl(){return trigfl;}
  static number gettrtm(){return trigtm;}
//
// interface with DAQ :
//
protected:
  void _printEl(ostream &stream){
    int i;
    stream <<"AMSEcalRawEvent: id="<<dec<<_idsoft<<endl;
    stream <<" Status="<<hex<<_status<<" Adc="<<_padc[0]<<" "<<_padc[1]<<endl;
    stream <<dec<<endl<<endl;
  }
void _writeEl(){};
void _copyEl(){};
//
};
//-----------------------------------
class AMSEcalHit: public AMSlink{
private:
//integer _status; // status (0/1/... -> alive/dead/...) (It is really in AMSlink !!!)
  integer _idsoft; //readout cell ID=SSPPC (SS->S-layer,PP->PMcell, C->SubCell in PMcell)
  integer _adc[2]; //raw adc's for later calibration (ovfl-suppressed, in DAQ-scale !)
  integer _proj;   //projection (0->X, 1->Y)
  integer _plane;  //continious numbering of planes through 2 projections(0,...)
  integer _cell;   // numbering in plane(0,...)
  number  _edep;
  number  _coot;   //transv.coord.
  number  _cool;   //long.coord.
  number  _cooz;
public:
  AMSEcalHit(integer status, integer id, integer adc[2], integer proj, integer plane, integer cell,
         number edep, number coot, number cool, number cooz):AMSlink(status,0),_idsoft(id),
	 _proj(proj), _plane(plane),_cell(cell),_edep(edep),_coot(coot),_cool(cool),_cooz(cooz)
	 {for(int i=0;i<2;i++)_adc[i]=adc[i];};
  AMSEcalHit(integer status, integer proj, integer plane, integer cell,
         number edep, number coot, number cool, number cooz):AMSlink(status,0),_idsoft(0),_proj(proj), _plane(plane),_cell(cell),_edep(edep),_coot(coot),_cool(cool),_cooz(cooz)
	 {for(int i=0;i<2;i++)_adc[i]=0;};
  ~AMSEcalHit(){};
  AMSEcalHit * next(){return (AMSEcalHit*)_next;}
//
  integer operator < (AMSlink & o)const{
   AMSEcalHit *p =dynamic_cast<AMSEcalHit*>(&o);
   if (checkstatus(AMSDBc::USED) && !(o.checkstatus(AMSDBc::USED)))return 1;
   else if(!checkstatus(AMSDBc::USED) && (o.checkstatus(AMSDBc::USED)))return 0;
   else return !p || _cell < p->_cell;
  }
  integer getproj(){return _proj;}
  integer getid(){return _idsoft;}
  void getadc(integer adc[]){for(int i=0;i<2;i++)adc[i]=_adc[i];}
  integer getplane(){return _plane;}
  integer getcell(){return _cell;}
  number getedep(){return _edep;}
  number getcoot(){return _coot;}
  number getcool(){return _cool;}
  number getcooz(){return _cooz;}
//
  static void build(int &stat);
  static integer Out(integer);
//
protected:
  void _printEl(ostream &stream){
    int i;
    stream <<"AMSEcalHit: Proj/Plane/cell= "<<_proj<<" "<<_plane<<" "<<_cell<<endl;
    stream <<"Status="<<hex<<_status<<dec<<" id="<<_idsoft<<"  Edep="<<_edep<<endl; 
    stream <<"Coot/l/z= "<<_coot<<" "<<_cool<<" "<<_cooz<<endl;
    stream <<dec<<endl<<endl;
  }
void _writeEl();
void _copyEl();
//
};


class Ecal1DCluster: public AMSlink{ 
private:
  integer _proj;   //projection (0->X, 1->Y)
  integer _plane;  //continious numbering of planes through 2 projections(0,...)
  number  _Energy3C;  //Corrected Energy MeV (+-1)
  number  _Energy5C;  //Corrected Energy MeV (+-2)
  number  _Energy9C;  //Corrected Energy MeV (+-4)
  number  _EnergyC;   //Corrected Energy MeV  
  number  _SideLeak;  //Side Leak Estimation (MeV)
  number  _DeadLeak;  //Dead PMT Leak Estimation (MeV)
  AMSPoint  _Coo;     // cluster center of gravity coordinates 
  integer _MaxCell;   // Cell with max edep 
  integer _Left;      // most left
  integer _Right;     // most right
  number  _Weight;    // Weight to calculate Coo;
  number _RMS;        //  rms;
  integer _NHits;
  AMSEcalHit *_pHit[2*ecalconst::ECPMSMX]; // pointers to hits
public:
  Ecal1DCluster(integer status, integer proj, integer pl, integer left, integer right, integer max, number en,number en3, number en5, number en9, number leak, number dead,AMSPoint coo, number w, number rms, AMSEcalHit * ptr[]): AMSlink(status,0),_proj(proj), _plane(pl),_Energy3C(en3),_Energy5C(en5),_Energy9C(en9),_EnergyC(en),_SideLeak(leak),_DeadLeak(dead),_Coo(coo),_MaxCell(max),_Left(left),_Right(right),_Weight(w),_RMS(rms){
_NHits=0;
for(int i=_Left;i<=_Right;i++){
 if(ptr && ptr[i])_pHit[_NHits++]=ptr[i];
}
};
  ~Ecal1DCluster(){};
  Ecal1DCluster * next(){return (Ecal1DCluster*)_next;}
//
 uinteger getNHits()const{return _NHits;}
 AMSEcalHit * getphit(uinteger i)const{return i<_NHits?_pHit[i]:0;}
 integer operator < (AMSlink & o)const{
   Ecal1DCluster *p =dynamic_cast<Ecal1DCluster*>(&o);
   if (checkstatus(AMSDBc::USED) && !(o.checkstatus(AMSDBc::USED)))return 1;
   else if(!checkstatus(AMSDBc::USED) && (o.checkstatus(AMSDBc::USED)))return 0;
   else return !p || (_plane<p->_plane);
}
  number getweight()const{return _Weight;}
  integer getproj()const{return _proj;}
  integer getplane()const{return _plane;}
  number getEnergy()const{return _EnergyC;}
  number getec3()const{return _Energy3C;}
  number getec5()const{return _Energy5C;}
  number getec9()const{return _Energy9C;}
  number getsleak()const{return _SideLeak;}
  number getdleak()const{return _DeadLeak;}
  number getrms()const{return _RMS;}
  integer getmaxcell()const {return _MaxCell;}
  AMSPoint getcoo()const {return _Coo;}
//
  static integer build(int stat=0);
  static integer Out(integer);
  number PosError(){return ECREFFKEY.PosError1D;}
  bool Good(){return !(checkstatus(AMSDBc::BAD) || checkstatus(AMSDBc::DELETED) || checkstatus(AMSDBc::JUNK) ||  checkstatus(AMSDBc::USED));}
  number Distance(Ecal1DCluster *p);
  Ecal1DCluster* EnergyMatch(Ecal1DCluster *pmatch, Ecal1DCluster *pbest){
   return (pbest && fabs(_EnergyC-pmatch->_EnergyC)>fabs(_EnergyC-pbest->_EnergyC))?pbest:this;
  }
//
protected:

  void _printEl(ostream &stream){
    int i;
    stream <<"AMSEcalCluster: Proj= "<<_proj<<" plane="<<_plane<<endl;
    stream <<"Status="<<hex<<_status<<"  Edep="<<dec<<_EnergyC<<endl; 
    stream <<"Coord="<<_Coo[0]<<" "<<_Coo[1]<<" "<<_Coo[2]<<endl;
    stream <<dec<<endl<<endl;
  }

void _writeEl();
void _copyEl(){};
//
};





class Ecal2DCluster: public AMSlink{ 
private:
  integer _proj;   //projection (0->X, 1->Y)
  number  _Energy3C;  //Corrected Energy MeV (+-1)
  number  _Energy5C;  //Corrected Energy MeV (+-2)
  number  _Energy9C;  //Corrected Energy MeV (+-4)
  number  _Energy;   // UnCorrected Energy MeV  
  number  _EnergyC;   //Corrected Energy MeV  
  number  _OrpLeak;   // Orphaned Clusters Leak;
  number  _SideLeak;  //Side Leak Estimation (MeV)
  number  _DeadLeak;  //Dead pmt Leak Estimation (MeV)
  number _Coo;  // x(y) coo
  number _Tan;  //x(y)z tangent
  number _Chi2;
  integer _NClust; // number of EcalCluster members
  integer _NClustKernel; // number of EcalCluster members (kernel)
  Ecal1DCluster * _pCluster[4*ecalconst::ECSLMX]; // pointers to 1d clusters 
public:
  Ecal2DCluster(integer proj, integer nhit, Ecal1DCluster * ptr[], number coo, number tan, number chi2): AMSlink(),_proj(proj),_Coo(coo),_Tan(tan),_Chi2(chi2){
_NClust=0;
for(int i=0;i<2*ecalconst::ECSLMX;i++){
  if(ptr[i]){
   _pCluster[_NClust++]=ptr[i];
  }   
}
_NClustKernel=_NClust;
}
  bool Good(){return !(checkstatus(AMSDBc::BAD)  ||  checkstatus(AMSDBc::USED));}
  ~Ecal2DCluster(){};
  Ecal2DCluster * next(){return (Ecal2DCluster*)_next;}
   static bool StrLineFit(Ecal1DCluster *p1[],int maxr,int proj, bool reset,number *pcorrect,int & tot, number &chi2, number &t0, number &tantz);
  integer operator < (AMSlink & o)const{
   Ecal2DCluster *p =dynamic_cast<Ecal2DCluster*>(&o);
   if (checkstatus(AMSDBc::USED) && !(o.checkstatus(AMSDBc::USED)))return 1;
   else if(!checkstatus(AMSDBc::USED) && (o.checkstatus(AMSDBc::USED)))return 0;
   else return !p || _EnergyC > p->_EnergyC;
}
//
  integer getproj(){return _proj;}
  number getcoo()const {return _Coo;}
  number gettan()const {return _Tan;}
  number getEnergy()const {return _EnergyC;}
  number getNClustKernel()const {return _NClustKernel;}
  number getNClust()const {return _NClust;}
  Ecal1DCluster *getpClust(uinteger i)const {return i<_NClust?_pCluster[i]:0;} 
//
  static integer build(int stat=0);
  static integer Out(integer);
//
protected:
  static number _ZCorr(Ecal1DCluster *p[],int ipl,int iplmax);
  void _AddOrphane(Ecal1DCluster *ptr);
  void _AddOneCluster(Ecal1DCluster *ptr, bool addpointer=false);
  void _Fit();
  void _printEl(ostream &stream){
    int i;
    stream <<"Ecal2DCluster: Proj= "<<_proj<<endl;
    stream <<"Status="<<hex<<_status<<"  Edep="<<dec<<_EnergyC<<endl; 
    stream <<"Coord="<<_Coo<<" "<<_Tan<<endl;
    stream <<dec<<endl<<endl;
  }

void _writeEl();
void _copyEl(){};
//
friend class EcalShower;
};
//-----------------------------------






class EcalShower: public AMSlink{
protected:

AMSPoint _EntryPoint;
AMSPoint _ExitPoint;
AMSDir   _Dir;
AMSDir   _EMDir;
number   _Angle3DChi2;
number   _AngleTrue3DChi2;
number   _Angle3DError;


number  _FrontEnergyDep;
number  _EnergyC;
number  _Energy3C;
number  _Energy5C;
number  _Energy9C;
number  _DifoSum;
number  _OrpLeak;
number  _RearLeak;
number  _SideLeak;
number  _DeadLeak;
number  _ErrEnergyC;

AMSPoint _CofG;
number  _ProfilePar[10];
number  _Edep[2*ecalconst::ECSLMX];
number  _Ez[2*ecalconst::ECSLMX];
number  _Zcorr[2*ecalconst::ECSLMX];
integer _Direction;
integer _iflag;
number  _Dz;
number _Et;
integer _ShowerMax;
Ecal2DCluster *_pCl[2];
number _TmpFit[20];

number _TransFitPar[3];
number _TransFitChi2;

void _writeEl();
void _printEl(ostream &stream){
stream << "  EcalShower Energy "<<_EnergyC<<" Status "<<_status<<endl;
}
void _copyEl(){};
void DirectionFit();
void EnergyFit();
void EMagFit();
static void monit(number & a, number & b,number sim[], int & n, int & s, int & ncall)
{};
static void gamfun(integer & n, number xc[], number & fc, EcalShower * ptr);
static void expfun(integer & n, number xc[], number & fc, EcalShower * ptr);
static void gamfunr(number& xc, number & fc, EcalShower * ptr);
void _AngleRes();
number _Chi2Corr();
void _EnergyRes();
void _EnergyCorr();
public:

EcalShower(Ecal2DCluster *px, Ecal2DCluster *py);

  static integer build(int rerun=0);
  static integer Out(integer);

  EcalShower * next(){return (EcalShower*)_next;}
  integer operator < (AMSlink & o)const{
   EcalShower *p =dynamic_cast<EcalShower*>(&o);
   if (checkstatus(AMSDBc::USED) && !(o.checkstatus(AMSDBc::USED)))return 1;
   else if(!checkstatus(AMSDBc::USED) && (o.checkstatus(AMSDBc::USED)))return 0;
   else return !p || _EnergyC > p->_EnergyC;
 }
number getTrue3DChi2(number tantz[2],number t0[2], bool zcorr[2]);
AMSPoint getEntryPoint()const {return _EntryPoint;}
AMSPoint getExitPoint()const {return _ExitPoint;}
AMSPoint getCofG()const {return _CofG;}
AMSDir getDir()const {return _Dir;}
number getEnergy()const {return _EnergyC;}
number getEnergyErr()const{return _ErrEnergyC;}
integer getDirection() const{return _Direction==0?1:-1;}
};

#endif



