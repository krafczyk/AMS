//  $Id: trfit.h,v 1.5 2011/10/25 09:45:40 choutko Exp $
#ifndef __TRFIT__
#define __TRFIT__
#include "TObject.h"
#include "point.h"
class AMSTimeID;
class AMSTrTrack;
class AMSEventR;
class TrTrackR;
class TrRecHitR;
namespace trc{
 const int maxlay=9;
 const int maxlad=17;
 const int maxsen=15;
}
class TrTrackFitR {
public: 
enum kAtt{
   kF,
   kH1,
   kH2,
   kI,
   kE,
   kC,
   kND
};
int Pattern;   ///< bit pattern  bits xxxxxxxx where x =0 1 and layer 1 corresponds to the least significant bit ; pattern -1 if object is invalid
short int Algo; ///< Algo 0 Choutko,1 Alc 2 Chik
short int MS;  ///< 0 no MS 1 MS +100 extres
short int Alig; ///< Alignment applied to a pattern in a same way xxxxxxxxx x=0,1,2,3 0 == no alig, 1=static alig, 2 == dyn alig, 3== recreates hit using alignment structure read in from timeid database 
short int Att; ///< Attribute 0 full path 1 1st half 2 2nd half 3 int only 4 ext only 5 custom 6 not defined
float Rigidity;  ///< Rigidity GV
float ErrRigidity; ///<  Err (1/Rigidity)
AMSPoint Coo;      ///< Track Fitted Coo  (cm)
float Theta;      ///< Track Theta (rad)
float Phi;        ///< Track Phi (rad)
float Chi2;       ///< Chi2
float Velocity;    ///<  velocity used in fit
class TrSCooR{
public:
int Id;  ///< !!ssddhl ss sensor(1:xx) dd ladder(1:15) h half(0:1) l layer(1u:9d)
AMSPoint Coo; ///< !!Fitted Coo 
float Theta;  ///< !!Fitted Theta
float Phi;    ///<  !!Fitted Phi
};
class gldb_def{
public:
 int nentries;
 float coo[3];
 float ang[3];
 gldb_def():nentries(0){for( int k=0;k<3;k++)coo[k]=ang[k]=0;}; 
 AMSPoint getcoo(){return AMSPoint(coo);}
 AMSPoint getang(){return AMSPoint(ang);}
};
class gldb_par{
public:
AMSPoint _Coo;
AMSPoint _Angles;
AMSDir _Dir[3];
public:
gldb_par():_Coo(0,0,0),_Angles(0,0,0){};
AMSPoint  getcoo()const {return _Coo;}
AMSPoint  getang()const {return _Angles;}
void setpar(const AMSPoint & coo, const AMSPoint & angle);
AMSDir   getmtx(integer i){assert(i>=0 && i<3);return _Dir[i];}
};
protected:
 static AMSTimeID* SAp; 
#pragma omp threadprivate (SAp)
 AMSPoint CrHit(TrRecHitR *rh);  ///< Create Hits using readin alignment db 
 gldb_par * SearchDB(TrRecHitR * rh);
 static gldb_def _gldb[trc::maxsen+1][trc::maxlad+1][2][trc::maxlay];
#pragma omp threadprivate (_gldb)

public:
/// Function to Set up alignment pars via AMSTimeID
/// name == alignment TDV name datamc 0/1 mc/data; force=true to redefine prev defined alignment pars
static AMSTimeID * SetAligDB(const char *name, int datamc,bool force=false);  ///< Function to set up alignment 
/// Function to Set up alignment pars via TDVR in root file
/// name == alignment TDV name datamc 0/1 mc/data; force=true to redefine prev defined alignment pars
static AMSTimeID * SetAligDB_TDVR(const char *name, int datamc,bool force=false);  ///< Function to set up alignment from TDVR /use mainly fopr debug purposes/ 


unsigned int NHits()const; ///< NumberOfHits


vector<TrSCooR> fTrSCoo;  ///< !!Vector of fitted coo 
TrTrackFitR():Pattern(0),Algo(-1),Alig(-1),MS(-1),Att(kND),Chi2(0),Rigidity(0),ErrRigidity(0),Coo(0,0,0),Theta(0),Phi(0),Velocity(1.){
fTrSCoo.clear();
}

/// TrTrackFit Constructor for iTrTrackFit function
///  patternoratt can be either positive(bit pattern) or (0,-1,-2,-3,-4) (-kAtt) 
TrTrackFitR( int patternoratt, int algo,int alig,int ms,float vel=1):Algo(algo),Alig(alig),MS(ms),Chi2(0),Rigidity(0),ErrRigidity(0),Coo(0,0,0),Theta(0),Phi(0),Velocity(vel){
fTrSCoo.clear();
if(patternoratt>0){
 Pattern=patternoratt;
 Att=kC;
}
else{
 Pattern=-1;
 Att=-patternoratt;
}
}

kAtt getAtt();  ///< return attribute in katt form
bool CheckLayer(int layer) const {return (Pattern &(1<<(layer-1)));} ///< Return true if layer (1...9) is is in the bit pattern

TrTrackFitR( int pattern, int algo,int alg,int ms,int att,float r,float er,float chi2,AMSPoint coo,float t, float p,int size,TrSCooR ic[],float vel=1);



bool operator == (const TrTrackFitR &a ) const{
 return Algo==a.Algo && (a.Att==Att || (Pattern>0 &&a.Pattern==Pattern))  && (Att==kC || a.Pattern==Pattern) && Alig==a.Alig && MS==a.MS;
}
int Fit(TrTrackR *fit); ///< Fit for root; return 0 if success, 1 if error , 2 if fit not defined 4 if no AMSEventR is present 5 if noalig structrure present for alig=3
static int InitMF(unsigned int time); ///< MagField Initialization return 0 if success;
ClassDef(TrTrackFitR,4)       //TrTrackFitR
};



#endif

