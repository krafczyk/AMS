//  $Id: trfit.h,v 1.2 2011/08/30 14:49:01 choutko Exp $
#ifndef __TRFIT__
#define __TRFIT__
#include "TObject.h"
#include "point.h"
class AMSTrTrack;
class AMSEventR;
class TrTrackR;
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
int Pattern;   ///< Pattern in a way xxxxxxxxx  x =0...9  124789 means 6 hits with layers 1,2,4,7,8,9; pattern -1 if object is invalid
short int Algo; ///< Algo 0 Choutko,1 Alc 2 Chik
short int MS;  ///< 0 no MS 1 MS
short int Alig; ///< Alignment applied to a pattern in a same way xxxxxxxxx x=0,1,2,3 0 == no alig, 1=static alig, 2 == dyn alig, 3== reserved 
short int Att; ///< Attribute 0 full path 1 1st half 2 2nd half 3 int only 4 ext only 5 custom 6 not defined
float Rigidity;  ///< Rigidity GV
float ErrRigidity; ///<  Err (1/Rigidity)
AMSPoint Coo;      ///< Track Fitted Coo  (cm)
float Theta;      ///< Track Theta (rad)
float Phi;        ///< Track Phi (rad)
float Chi2;       ///< Chi2
class TrSCooR{
public:
int Id;  ///< !!ssddhl ss sensor(1:xx) dd ladder(1:15) h half(0:1) l layer(1u:9d)
AMSPoint Coo; ///< !!Fitted Coo 
float Theta;  ///< !!Fitted Theta
float Phi;    ///<  !!Fitted Phi
};
unsigned int NHits()const; ///< NumberOfHits


vector<TrSCooR> fTrSCoo;  ///< !!Vector of fitted coo 
TrTrackFitR():Pattern(0),Algo(-1),Alig(-1),MS(-1),Att(kND),Chi2(0),Rigidity(0),ErrRigidity(0),Coo(0,0,0),Theta(0),Phi(0){
fTrSCoo.clear();
}

/// TrTrackFit Constructor for iTrTrackFit function
///  patternoratt can be either positive(bit pattern) or (0,-1,-2,-3,-4) (-kAtt) 
TrTrackFitR( int patternoratt, int algo,int alig,int ms):Algo(algo),Alig(alig),MS(ms),Chi2(0),Rigidity(0),ErrRigidity(0),Coo(0,0,0),Theta(0),Phi(0){
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

TrTrackFitR( int pattern, int algo,int alg,int ms,int att,float r,float er,float chi2,AMSPoint coo,float t, float p,int size,TrSCooR ic[]);



bool operator == (const TrTrackFitR &a ) const{
 return Algo==a.Algo && (a.Att==Att || (Pattern>0 &&a.Pattern==Pattern))  && (Att==kC || a.Pattern==Pattern) && Alig==a.Alig && MS==a.MS;
}
#ifdef __ROOTSHAREDLIBRARY__
int Fit(const TrTrackR &track, AMSEventR * event); ///< Fit for root
#endif
ClassDef(TrTrackFitR,1)       //TrTrackFitR
};



#endif

