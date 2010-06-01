#ifndef __AMSTRDRhit__
#define __AMSTRDRhit__

#include "TObject.h"

#ifdef __WRITEROOT__
class AMSTRDRawHit;
#endif

class TrdRawHitR {
public:
  int Layer;   ///< Layer 0(bottom)...19(top) 
  int Ladder;  ///< Ladder number
  int Tube;    ///< tube number
  float Amp;   ///< amplitude (adc counts)
  int Haddr;   ///< Hardware Address cufhh  c crate[0-1],u udr[0-6] f ufe [0-7] hh channel[0-63]
  
 TrdRawHitR():Layer(-1),Ladder(-1),Tube(-1),Amp(-1),Haddr(-1){};
 TrdRawHitR(int lay, int lad, int tub, float amp):Layer(lay),Ladder(lad),Tube(tub),Amp(amp),Haddr(0){};

#ifdef __WRITEROOT__
   TrdRawHitR(AMSTRDRawHit *ptr);
#endif

   virtual ~TrdRawHitR(){};
  ClassDef(TrdRawHitR,1)       //TrdRawHitR
#pragma omp threadprivate(fgIsA)
};


class TRDHitRZD{ 
 public:
 double r,z; 
 int d; 
 TRDHitRZD():r(0.),z(0.),d(0){};
 TRDHitRZD(double r_, double z_, int d_):r(r_),z(z_),d(d_){};
 TRDHitRZD(TrdRawHitR* hit){
   //   if(hit==NULL) 
   int layer  = hit->Layer;
   int ladder = hit->Ladder;
   int tube   = hit->Tube;

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
