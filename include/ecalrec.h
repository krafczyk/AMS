//  $Id: ecalrec.h,v 1.62 2012/04/27 22:43:17 paniccia Exp $
//
// 28.09.1999 E.Choumilov
// last update 22.04.2008 E.Choumilov, EcalHit-constructor for 1DClusters corrected by V.Choutko
//
#ifndef __AMSECALREC__
#define __AMSECALREC__
#include "typedefs.h"
#include "point.h"
#include "link.h"
#include "event.h"
#include <stdlib.h>
#include <time.h>
#include "ecaldbc.h"
#include "ecid.h"
//
//---------------------------------------
class AMSEcalRawEvent: public AMSlink{
  // this is one-pixel object (+common Dynode info after validation stage)
 private:
  static uinteger StartRun;//first run of the job
  static time_t StartTime;//first run time
  static uinteger trigfl;//=MN, M=0/1/2/3->FTE(Ener)Flag=No/1prj@2prj_req/FTE&1Proj/FTE&2proj,
  //                 N=0/1/2/3->LVL1(Angle)Flag=Undef(noFTE)/0prj@FTEfound/OrLVL1/AndLVL1; M&N are masked.
  static uinteger trigconf;//Proj.OR/AND flag for FTE/LVL1(MN, M(N)=2/1/0=> AND/OR/No of proj) 
  static number trigtm; // ECAL FTE-trig abs. time at JLV1
  static geant trsum;// Trigger sum(dynodes,tempor in gev)
  static geant dynadc[ecalconst::ECSLMX][ecalconst::ECPMSMX];//dynode adc's (ped-subtracted)
  static int16u trpatt[6][3];//dyn.trig.patt([6]=>suplayers:2-7; [3]=>3x16bits used for triggered dynodes)
#pragma omp threadprivate(trigfl,trigconf,trigtm,trsum,dynadc,trpatt)
  AMSECIds _id;  // AMSECIds-type id
  integer _aside; // H/W active side#(1-2) 
  integer _idsoft; //short swid=LTTP (L->supLayer(1-9),TT->pmTube(1-36), P->Pixel#(1-4) in pmTube)
  geant _padc[3];//pulse heights(ADC-counts, ped-subtracted if not PedCal job)[HighGain,LowGain, Dynode]
 public:

  AMSEcalRawEvent(integer idsoft, integer status, integer side,  
		  geant padc[2]):AMSlink(status,0),_aside(side),_idsoft(idsoft){
    for(int i=0;i<2;i++)_padc[i]=padc[i];
    _padc[2]=0;
  }



    //
    ~AMSEcalRawEvent(){};
    AMSEcalRawEvent * next(){return (AMSEcalRawEvent*)_next;}
    //
    integer operator == (AMSlink & o)const{
      AMSEcalRawEvent *p =dynamic_cast<AMSEcalRawEvent*>(&o);
      return p && (p->_idsoft)/10 == _idsoft/10;
    }

    integer operator < (AMSlink & o)const{
      AMSEcalRawEvent *p =dynamic_cast<AMSEcalRawEvent*>(&o);
      if (checkstatus(AMSDBc::USED) && !(o.checkstatus(AMSDBc::USED)))return 1;
      else if(!checkstatus(AMSDBc::USED) && (o.checkstatus(AMSDBc::USED)))return 0;
      else return !p || _idsoft < p->_idsoft;
    }
    //
    integer getid() const {return _idsoft;}
    uint16 getasn() const {return _aside;}
    void getpadc(geant padc[3]){for(int i=0;i<3;i++)padc[i]=_padc[i];}
    geant getpadc(uinteger i){return i<3?_padc[i]:0;}
    geant getadc(int16u gain) const{return gain<2?_padc[gain]:-1;}
    void setasn(int16u asn){_aside=asn;}
    void TestThreshold();
    void setadc(geant adc, int16u gain){if(gain<2)_padc[gain]=adc;}
    void setadcd(geant adc){_padc[2]=adc;}
    integer lvl3format(int16 * ptr, integer rest);
    int16 getslay(){return _idsoft/1000-1;}
    //
    static uinteger getsrun(){return StartRun;}
    static time_t getstime(){return StartTime;}
    static void mc_build(int &stat);
    static void BeamTestLinCorr(int gain,integer id,number radc[2],geant ped[2],number &fadc);
    static void validate(int &stat);
    static void settrfl(uinteger trfl){trigfl=trfl;}
    static uinteger gettrfl(){return trigfl;}
    static uinteger gettrconf(){return trigconf;}
    static number gettrtm(){return trigtm;}
    static geant gettrsum(){return trsum;}
    static void gettrpatt(int16u patt[6][3]){
      for(int i=0;i<6;i++)for(int j=0;j<3;j++)patt[i][j]=trpatt[i][j];
    }
    static void cleartrpatt(){
      for(int i=0;i<6;i++)for(int j=0;j<3;j++)trpatt[i][j]=0;
    }
    static void settrpbit(int16u sl, int16u pm){
      //                          0-5       0-35
      //"bits" sl=5, pm=36/37/38/39 --> XA/YA/XF/YF-bits in ETRG-map according to Stefano Di Falco
      int16u word=pm/16;//0-2
      int16u bit=pm%16;//0-15
      trpatt[sl][word]|=(1<<bit);
    }
    static int gettrpbit(int16u sl, int16u pm){
      //                          0-5       0-35
      //"bits" sl=5, pm=36/37/38/39 --> XA/YA/XF/YF-bits in ETRG-map according to Stefano Di Falco
      int16u word=pm/16;//0-2
      int16u bit=pm%16;//0-15
      if((trpatt[sl][word]&(1<<bit))>0)return(1);
      else return(0);
    }
    static geant getadcd(int is, int pm){return dynadc[is][pm];}
    static void setadcd(int is, int pm, geant adc){dynadc[is][pm]=adc;}
    static void init(){
      for(int i=0;i<ecalconst::ECSLMX;i++)for(int j=0;j<ecalconst::ECPMSMX;j++)dynadc[i][j]=0;
      for(int i=0;i<6;i++)for(int j=0;j<3;j++)trpatt[i][j]=0;
      trigfl=0;
      trsum=0;
      trigtm=0;
      trigconf=0;
    }
    //
    // interface with DAQ :
    //
    // static integer checkdaqid(int16u id);
    // static void buildrawRaw(integer n, int16u *p);
    // static integer getmaxblocks(){return AMSECIds::ncrates();}
    // static int16u getdaqid(int i);
    static void setTDV();

    //  TDV

    static    AMSID  getTDVped() {return AMSID("Ecalpeds",AMSJob::gethead()->isRealData());}
    static    AMSID  getTDVstatus(){return AMSID("EcalStatus",AMSJob::gethead()->isRealData());}
    static    AMSID  getTDVcalib(){return AMSID("Ecalpmcalib",AMSJob::gethead()->isRealData());}
    static    AMSID  getTDVcalibMS(){return AMSID("EcalpmcalibMS",AMSJob::gethead()->isRealData());}
    static    AMSID  getTDVvpar(){return AMSID("Ecalvpar",AMSJob::gethead()->isRealData());}
    static    AMSID  getTDVcalibTslo(){return AMSID("EcalTslo",AMSJob::gethead()->isRealData());}
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
  integer _idsoft; //readout cell ID=LTTP (sLayer,pmTube,Pixel)
  geant _adc[3]; //raw adc's for later calibration (ovfl-suppressed,in ADC-ch (high,low,dynode if any)
  integer _proj;   //projection (0->X, 1->Y)
  integer _plane;  //continious numbering of planes through 2 projections(0,...17)
  integer _cell;   // numbering in plane(0,...71)
  number  _edep;
  number  _edepc;//value(added to _edep)) used to correct for PM-saturation(type-1 -> devider) 
  number  _coot;   //transv.coord.
  number  _cool;   //long.coord.
  number  _cooz;
  number _attcor; //att length correction
 public:
#ifdef __WRITEROOT__
  friend class EcalHitR;
#endif
  AMSEcalHit(integer status, integer id, geant adc[3], integer proj, integer plane, integer cell,
	     number edep, number ecorr, number coot, number cool, number cooz):AMSlink(status,0),_idsoft(id),
    _proj(proj), _plane(plane),_cell(cell),_edep(edep),_edepc(ecorr),_coot(coot),_cool(cool),_cooz(cooz),_attcor(0)
    {for(int i=0;i<3;i++)_adc[i]=adc[i];}
    AMSEcalHit(integer status, integer proj, integer plane, integer cell,
	       number edep,  number coot, number cool, number cooz,integer idsoft):AMSlink(status,0),
      _idsoft(idsoft),_proj(proj), _plane(plane),_cell(cell),_edep(edep),_edepc(0),_coot(coot),_cool(cool),_cooz(cooz),_attcor(0)
      {for(int i=0;i<3;i++)_adc[i]=0;};
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
      void getadc(float adc[]){for(int i=0;i<3;i++)adc[i]=_adc[i];}
      float getadc(uinteger i){return i<3?_adc[i]:0;}
      integer getplane(){return _plane;}
      integer getcell(){return _cell;}
      number getedep(){return _edep;}
      number getedepc(){return _edepc;}
      number &edep(){return _edep;}
      number getcoot(){return _coot;}
      number getcool(){return _cool;}
      number getcooz(){return _cooz;}
      void attcor(number coo);
      number getattcor()const {return _attcor;}
      //
      static void build(int &stat);
      static integer Out(integer);
      static integer Smooth();
      //
 protected:
      void _printEl(ostream &stream){
	int i;
	stream <<"AMSEcalHit: Proj/Plane/cell= "<<_proj<<" "<<_plane<<" "<<_cell<<endl;
	stream <<"Status="<<hex<<_status<<dec<<" id="<<_idsoft<<"  Edep="<<_edep<<" EdCorr="<<_edepc<<endl; 
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
    number cool=0;
    number edep=0;
    for(int i=_Left;i<=_Right;i++){
      if(ptr && ptr[i]){
	_pHit[_NHits++]=ptr[i];
	cool+=ptr[i]->getcool()*ptr[i]->getedep();
	edep+=ptr[i]->getedep();
      }
    }
    cool/=edep+1.e-10;
    _Coo[1-proj]=cool;
  }
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
      return (pbest && fabs(_EnergyC-pmatch->_EnergyC)>fabs(pmatch->_EnergyC-pbest->_EnergyC))?pbest:this;
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
    void _copyEl();
    //
#ifdef __WRITEROOT__
    friend class EcalClusterR;
#endif
};





class AMSEcal2DCluster: public AMSlink{ 
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
  AMSEcal2DCluster(integer proj, integer nhit, Ecal1DCluster * ptr[], number coo, number tan, number chi2): AMSlink(),_proj(proj),_Coo(coo),_Tan(tan),_Chi2(chi2){
    _NClust=0;
    for(int i=0;i<2*ecalconst::ECSLMX;i++){
      if(ptr[i]){
	_pCluster[_NClust++]=ptr[i];
      }   
    }
    _NClustKernel=_NClust;
  }
    bool Good(){return !(checkstatus(AMSDBc::BAD)  ||  checkstatus(AMSDBc::USED));}
    ~AMSEcal2DCluster(){};
    AMSEcal2DCluster * next(){return (AMSEcal2DCluster*)_next;}
    static bool StrLineFit(Ecal1DCluster *p1[],int maxr,int proj, bool reset,number *pcorrect,int & tot, number &chi2, number &t0, number &tantz);
    integer operator < (AMSlink & o)const{
      AMSEcal2DCluster *p =dynamic_cast<AMSEcal2DCluster*>(&o);
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
      stream <<"AMSEcal2DCluster: Proj= "<<_proj<<endl;
      stream <<"Status="<<hex<<_status<<"  Edep="<<dec<<_EnergyC<<endl; 
      stream <<"Coord="<<_Coo<<" "<<_Tan<<endl;
      stream <<dec<<endl<<endl;
    }

    void _writeEl();
    void _copyEl();
    //
    friend class AMSEcalShower;
#ifdef __WRITEROOT__
    friend class Ecal2DClusterR;
#endif
};
//-----------------------------------






class AMSEcalShower: public AMSlink{
 protected:

  AMSPoint _EntryPoint;
  AMSPoint _ExitPoint;
  AMSDir   _Dir;
  AMSDir   _EMDir;
  number   _Angle3DChi2;
  number   _AngleTrue3DChi2;
  number   _Angle3DError;
  number   _Angle3DErrorPI;
  number   _Angle3DErrorA;

  number  _FrontEnergyDep;
  number  _Energy;

  number  _EnergyC;
  number  _EnergyPIC;
  number  _Energy0A[2];  //LAPP EcalShower Energy0 0->X, 1->Y  only impact-point corrected, no rear leakage corr. (M.P.)
  number  _EnergyA;  //LAPP EcalShower Energy (M.P.)

  number  _Energy3C;
  number  _Energy5C;
  number  _Energy9C;

  number  _S13R;
  number  _S13Rpi[2];
  number  _S13Ra[2]; //LAPP S13ratios 0->X, 1->Y (M.P.)
  number  _S35Ra[2]; //LAPP S35ratios 0->X, 1->Y (M.P.)

  number  _DifoSum;

  number  _OrpLeak;
  number  _RearLeak;
  number  _S13Leak;
  number  _SideLeak;
  number  _DeadLeak;
  number  _AttLeak;
  number  _NLinLeak;

  number  _OrpLeakPI;
  number  _RearLeakPI;
  number  _S13LeakXPI;
  number  _S13LeakYPI;
  number  _SideLeakPI;
  number  _DeadLeakPI;
  number  _AttLeakPI;
  number  _NLinLeakPI;
  //LAPP (M.P.) begin:
  number  _OrpLeakA;
  number  _RearLeakA;
  number  _S13LeakXA;
  number  _S13LeakYA; 
  number  _S13LeakXA0;
  number  _S13LeakYA0;
  number  _S35LeakXA;
  number  _S35LeakYA; 
  number  _VarLeakXA;
  number  _VarLeakYA;
  number  _SideLeakA;
  number  _DeadLeakA;
  number  _AttLeakA;
  number  _NLinLeakA;
  //end LAPP
  number  _ErrEnergyC;
  number  _ErrEnergyPIC;
  number  _ErrEnergyA;//LAPP EcalShower Energy error (M.P)

  number  _Orp2DEnergy;
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
  integer _N2dCl;
  integer _Nhits;
  AMSEcal2DCluster *_pCl[20];
  number _TmpFit[20];
  number _SphericityEV[3];
  number _TransFitPar[3];
  number _TransFitChi2;

  // LAPP
  number _S1tot;		  
  number _S1totx;	  
  number _S1toty;	  
  number _S3tot;		  
  number _S3totx;	  
  number _S3toty;	  
  number _S5tot;		  
  number _S5totx;	  
  number _S5toty;	  
  number _ShowerLatDisp;	  
  number _ShowerLatDispx;  
  number _ShowerLatDispy;  
  number _ShowerLongDisp;  
  number _ShowerDepth;	  
  number _ShowerFootprint; 
  number _ShowerFootprintx;
  number _ShowerFootprinty;
  number _ShowerFootprint_b; 
  number _ShowerFootprintx_b;
  number _ShowerFootprinty_b;
  // number _Zmax[2];
  number _Zprofile[4];
  number _ZprofileChi2;
  integer _NbLayerX;
  integer _NbLayerY;
  number _EcalEdepFrac[18];	



  void LAPPVariables();
  void ZProfile();

  void _AttCorr();
  void _writeEl();
  void _printEl(ostream &stream){
    stream << "  AMSEcalShower Energy "<<_EnergyC<<" Status "<<_status<<endl;
  }
  void _copyEl();
  void DirectionFit();
  void ProfileFit();
  void EnergyFit();
  void EMagFit();
  void SphFit();
  static void monit(number & a, number & b,number sim[], int & n, int & s, int & ncall)
    {};
  static void gamfun(integer & n, number xc[], number & fc, AMSEcalShower * ptr);
  static void expfun(integer & n, number xc[], number & fc, AMSEcalShower * ptr);
  static void gamfunr(number& xc, number & fc, AMSEcalShower * ptr);
  void _AngleRes();
  number _Chi2Corr();
  void _EnergyRes();
  void _EnergyCorr();
 public:

  AMSEcalShower(AMSEcal2DCluster *px, AMSEcal2DCluster *py);

  static integer build(int rerun=0);
  static integer Out(integer);

  void AddOrphan(AMSEcal2DCluster *ptr);
  AMSEcalShower * next(){return (AMSEcalShower*)_next;}
  integer operator < (AMSlink & o)const{
    AMSEcalShower *p =dynamic_cast<AMSEcalShower*>(&o);
    if (checkstatus(AMSDBc::USED) && !(o.checkstatus(AMSDBc::USED)))return 1;
    else if(!checkstatus(AMSDBc::USED) && (o.checkstatus(AMSDBc::USED)))return 0;
    else return !p || _EnergyC > p->_EnergyC;
  }
  number getTrue3DChi2(number tantz[2],number t0[2], bool zcorr[2]);
  AMSPoint getEntryPoint()const {return _EntryPoint;}
  AMSPoint getExitPoint()const {return _ExitPoint;}
  AMSPoint getCofG()const {return _CofG;}
  AMSDir getDir()const {return _Dir;}
  number getEnergy()const {return _Energy;}
  number getEnergyC()const {return _EnergyC;}
  number getEnergyPIC()const {return _EnergyPIC;}
  number getEnergyXY(int proj)const;
  number getEnergyErr()const{return _ErrEnergyC;}
  number getEnergyErrPI()const{return _ErrEnergyPIC;}
  number getEnergyA()const {return _EnergyA;} //LAPP (M.P.)
  number getEnergyErrA()const{return _ErrEnergyA;}//LAPP (M.P.)
  integer getDirection() const{return _Direction==0?1:-1;}
  integer getNhits() const{return _Nhits;}
  //gchen
  number getDirChi2() const {return _AngleTrue3DChi2;}
  number getErDir() const {return _Angle3DError;}
  number getErDirPI() const {return _Angle3DErrorPI;}
  number getErDirA() const {return _Angle3DErrorA;}//LAPP
  number getEnFront() const {return _FrontEnergyDep;}
  number getDifoSum() const {return _DifoSum;}
  number getSLeak() const {return _SideLeak;}
  number getSLeakPI() const {return _SideLeakPI;}
  number getSLeakA() const {return _SideLeakA;}//LAPP (M.P.)
  number getRLeak() const {return _RearLeak;}
  number getRLeakPI() const {return _RearLeakPI;}
  number getRLeakA() const {return _RearLeakA;}//LAPP (M.P.)
  number getS13Leak() const {return _S13Leak;}
  number getS13LeakXPI() const {return _S13LeakXPI;}
  number getS13LeakYPI() const {return _S13LeakYPI;}
  number getS13LeakXA() const {return _S13LeakXA;}//LAPP (M.P)
  number getS13LeakYA() const {return _S13LeakYA;}//LAPP (M.P.)
  number getS13LeakXA0() const {return _S13LeakXA0;}//LAPP (M.P.)
  number getS13LeakYA0() const {return _S13LeakYA0;}//LAPP (M.P.)
  number getDLeak() const {return _DeadLeak;}
  number getDLeakPI() const {return _DeadLeakPI;}
  number getDLeakA() const {return _DeadLeakA;}//LAPP (M.P.)
  number getOLeak() const {return _OrpLeak;}
  number getOLeakPI() const {return _OrpLeakPI;}
  number getOLeakA() const {return _OrpLeakA;}//LAPP (M.P.)
  number getProfChi2() const {return _ProfilePar[4+_Direction*5];}
  number getTransChi2() const {return _TransFitChi2;}

  // LAPP
  number getS1tot(){return _S1tot;}
  number getS1totx(){return _S1totx;}
  number getS1toty(){return _S1toty;}
  number getS3tot(){return _S3tot;}
  number getS3totx(){return _S3totx;}
  number getS3toty(){return _S3toty;}
  number getS5tot(){return _S3tot;}
  number getS5totx(){return _S3totx;}
  number getS5toty(){return _S3toty;}
  number getShowerLatDisp(){return _ShowerLatDisp;}
  number getShowerLatDispx(){return _ShowerLatDispx;}
  number getShowerLatDispy(){return _ShowerLatDispy;}
  number getShowerLongDisp(){return _ShowerLongDisp;}
  number getShowerDepth(){return _ShowerDepth;}
  number getShowerFootprint(){return _ShowerFootprint;}
  number getShowerFootprintx(){return _ShowerFootprintx;}
  number getShowerFootprinty(){return _ShowerFootprinty;}

#ifdef __WRITEROOT__
  friend class EcalShowerR;
#endif
};

#endif



