//  $Id: TrdRawHit.h,v 1.8 2011/10/21 12:47:18 choutko Exp $
#ifndef __AMSTRDRhit__
#define __AMSTRDRhit__
//  below is the gift from VC to MM
//#define __MLD__
#include "TObject.h"
#include "TrElem.h"

#ifdef __WRITEROOT__
class AMSTRDRawHit;
#endif

using namespace std;
class TrdRawHitR: public TrElem{
 public:
  int Layer;   ///< Layer 0(bottom)...19(top) 
  int Ladder;  ///< Ladder number
  int Tube;    ///< tube number
  float Amp;   ///< amplitude (adc counts)
  int Haddr;   ///< Hardware Address cufhh  c crate[0-1],u udr[0-6] f ufe [0-7] hh channel[0-63]
  
  TrdRawHitR():Layer(-1),Ladder(-1),Tube(-1),Amp(0.),Haddr(-1){};
  TrdRawHitR(int lay, int lad, int tub, float amp):Layer(lay),Ladder(lad),Tube(tub),Amp(amp),Haddr(0){};
   unsigned int getid();  ///< return channel number in TRDPedestals, TRDSigmas,TRDGains structures 
  float getped(int &error);   ///< return ped
  float getsig(int &error);   ///< return sigma
  float getgain(int &error);  ///< return gain  from TRDGains
  float getgain2(int &error);  ///< return gain  from TRDGains2
  
  bool operator==(const TrdRawHitR& other) const {
    if(Layer != other.Layer)return 0;
    if(Ladder!= other.Ladder)return 0;
    if(Tube  != other.Tube)return 0;
    if(Amp   != other.Amp)return 0;
    return 1;
  }
  
  void _PrepareOutput(int opt=0){
    sout.clear();
    sout.append("TrdRawHit _PrepareOutput");
  };

  char* Info(int iRef=0){return "TrdRawHit::Info";};

  std::ostream& putout(std::ostream &ostr = std::cout){
    _PrepareOutput(1);
    return ostr << sout  << std::endl; 
  };

  /// printout
  void Print(int level=0){return;};

  virtual ~TrdRawHitR(){};

#ifdef __WRITEROOT__
  TrdRawHitR(AMSTRDRawHit *ptr);
#endif
#ifdef __MLD__
static int num;
static int numa;
static void print(){cout<<" hits "<<num<<" arrays  "<<numa<<endl;}

void* operator new(size_t t) {
  num++;
  return ::operator new(t);
}

void operator delete(void* p){
if(p){
num--;
::operator delete(p);
p=0;
}
} 


void* operator new[](size_t t) {
  numa++;
  return ::operator new[](t);
}

void operator delete[](void *p){
if(p){
numa--;
::operator delete[](p);
p=0;
}
} 

#endif
  
  ClassDef(TrdRawHitR,4)       //TrdRawHitR
#pragma omp threadprivate(fgIsA)
    };


class TRDHitRZD{ 
 public:
  double r,z; 
  int d; 
  TRDHitRZD():r(0.),z(0.),d(0){};
    TRDHitRZD(double r_, double z_, int d_):r(r_),z(z_),d(d_){};
      TRDHitRZD(TrdRawHitR &hit){
	//   if(hit==NULL) 
	int layer  = hit.Layer;
	int ladder = hit.Ladder;
	int tube   = hit.Tube;

	if(layer<12) ladder++;
	if(layer< 4) ladder++;
   
	if((layer>=16)||(layer<=3)) d = 1;
	else                        d = 0;
   
	z = 85.275 + 2.9*(float)layer;
	if(ladder%2==0) z += 1.45;
   
	r = 10.1*(float)(ladder-9);
   
	if((d==1) && (ladder>=12)) r+=0.78;
	if((d==1) && (ladder<= 5)) r-=0.78;
   
	r += 0.31 + 0.62*(float)tube;
	if(tube >=  1) r+=0.03;
	if(tube >=  4) r+=0.03;
	if(tube >=  7) r+=0.03;
	if(tube >=  9) r+=0.03;
	if(tube >= 12) r+=0.03;
	if(tube >= 15) r+=0.03;
      };

};

#endif
