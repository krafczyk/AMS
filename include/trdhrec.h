#ifndef __AMSTRDHREC__
#define __AMSTRDHREC__
#include "trdrec.h"
#include <vector>

const float TRDz0=113.5;
const float RSDmax=0.6;

class TRDHitRZD{ 
 public:
 double r,z; 
 int d; 
 TRDHitRZD():r(0.),z(0.),d(0){}
 TRDHitRZD(double r_, double z_, int d_):r(r_),z(z_),d(d_){}
 TRDHitRZD(AMSTRDRawHit* hit){
   //   if(hit==NULL) 
   AMSTRDIdSoft id(hit->getidsoft());
   int layer  = id.getlayer();
   int ladder = id.getladder();
   int tube   = id.gettube();

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

class AMSTRDHSegment:public AMSlink{ 
 protected:
  void _init(){}
  void _printEl(ostream &o);
  void _copyEl();
  void _writeEl();
  public:
  integer    d;
  float      m,r,z,w;
  integer    nhits;
  float      chi2;
  vector<AMSTRDRawHit*> fTRDRawHit;
  
  //  static integer _addnext(integer pat, integer nhit, uinteger iseg,AMSTRDRawHit* pthit[]);
  static integer _addnext(integer d, float m, float r, float z, float w, integer nhits, AMSTRDRawHit* pthit[]);
  void _addnextR(uinteger iseg);

  AMSTRDHSegment(integer d_, float m_, float r_, float z_, float w_)
    : AMSlink(),d(d_), m(m_), r(r_), z(z_), w(w_) 
    {};

  AMSTRDHSegment(integer d_, float m_, float r_, float z_, float w_, integer nhits_, AMSTRDRawHit* pthit[])
    : AMSlink(),d(d_), m(m_), r(r_), z(z_) , w(w_), nhits(nhits_)
  {
    for(int i=0;i!=nhits_;i++){
      if(pthit[i])fTRDRawHit[i]=pthit[i];
    }
  };

   void SetHits(integer nhits_, AMSTRDRawHit* pthit[]){
     for(int i=0;i!=nhits_;i++) if(pthit[i])fTRDRawHit[i]=pthit[i];
   }

  float resid(float r_, float z_, int d_){
    if(d_!=d)return -1;
    return (r_ - (r + m*(z_-z)))*cos(atan(m));
  };

  void calChi2(){
    chi2=0.;
    for(int i=0;i!=nhits;i++){
      TRDHitRZD rzd=TRDHitRZD(fTRDRawHit[i]);
      chi2+=pow(resid(rzd.r,rzd.z,rzd.d)/ (0.62/sqrt(12)),2);
    }
  }

  integer operator < (AMSlink & o) const {
    AMSTRDHSegment * p= dynamic_cast<AMSTRDHSegment*>(&o);
    return !p||chi2<p->chi2;
  }

  AMSTRDHSegment *  next(){return (AMSTRDHSegment*)_next;}
  static integer build(int rerun);
  static AMSTRDHSegment * gethead(uinteger i=0);
  static integer Out(integer status);
  void Print(int level=0){
    cout << "AMSTRDHSegmentk - Info" << endl;
    printf("AMSTRDHSegment d %i m %f r %f z %f w %f nhits %i chi2 %f\n", d, m,r,z,w,nhits,chi2);
    if(level>0){
      for(int i=0;i!=nhits;i++){
	AMSTRDIdSoft id(fTRDRawHit[i]->getidsoft());
	printf("  RawHit %i LLT %i %i %i\n",i,id.getlayer(),id.getladder(),id.gettube());
      }
    }
  };
  
  int LinReg(){ // fit  Hit.rz   with  r = A*z + B
    int stat = 0;
    double Sw=0.0,Sr=0.0,Sz=0.0,Srz=0.0,Szz=0.0;
    for(int i=0; i<fTRDRawHit.size(); i++){
      TRDHitRZD rzd(fTRDRawHit[i]);
      Sw  += 1.0;
      Sr  += rzd.r;
      Sz  += rzd.z;
      Srz += rzd.r * rzd.z;
      Szz += rzd.z * rzd.z;
    }
    double DD =  Sw*Szz-Sz*Sz;
    if(DD==0.0){ stat = -1; }  // no fit possible - do nothing
    else{
      double A  = (Sw*Srz-Sr*Sz)/DD;
      double B  = (Sr-A*Sz)/Sw;
      if(fabs(m)<1.0 && (fabs(m-A)>0.2 || fabs(r-(A*z+B))>10.0) ){
	// printf("LinReg nh: %d  m: %10.4f %10.4f  r: %10.2f %10.2f\n", pTrdRawHit.size(),m,A,r,A*z+B);
	stat = 1;
      }
      m = A; r = A*z+B; // replace with new result
    }
    return(stat);
  }


#ifdef __WRITEROOT__
  friend class TrdHSegmentR;
#endif
};

class AMSTRDHTrack:public AMSlink{
 protected:
  void _init(){}
  void _printEl(ostream &o);
  void _copyEl();
  void _writeEl();
 public:
  float pos[3];
  float dir[3];
  float chi2;
  int nhits;
  vector<AMSTRDHSegment*> fTRDHSegment;

  int hit_arr[20][18][16];
  int amp_arr[20][18][16];

  static integer _addnext(float pos[], float dir[], AMSTRDHSegment* pthit[]);
  void _addnextR(uinteger iseg);

  AMSTRDHTrack(float pos_[3],float dir_[3]):AMSlink(),chi2(0.),nhits(0)
   {
     for (int i=0;i!=3;i++){
       pos[i]=pos_[i];
       dir[i]=dir_[i];
     }
   };

 AMSTRDHTrack():AMSlink(),chi2(0.),nhits(0){
   for(int i=0;i!=3;i++){
     pos[i]=0.;
     dir[i]=0.;
   }
 };
 //~AMSTRDHTrackR()

 integer operator < (AMSlink & o) const {
   AMSTRDHTrack * p= dynamic_cast<AMSTRDHTrack*>(&o);
   return !p||chi2<p->chi2;
 }
 
 float Theta(){ 
   return acos(dir[2]);
 }
 
 float Phi(){ 
   return atan2(dir[1],dir[0]);
 }
 
 void SetSegment(AMSTRDHSegment* segx, AMSTRDHSegment* segy){
   fTRDHSegment.push_back(segx);
   fTRDHSegment.push_back(segy);

   nhits=(int)segx->fTRDRawHit.size()+(int)segy->fTRDRawHit.size();
   chi2=segx->chi2+segy->chi2;

 }
 
 float Chi2(){return chi2;}
 
 void setChi2(float chi2_){chi2=chi2_;};
 
 /*
   double d_perp(TrdExtRawHitR* hit) 
   {
   TVector3 v = hit->getPosition();
   TVector3 pos(pos_.x(),pos_.y(),pos_.z());
   TVector3 dir(dir_.x(),dir_.y(),dir_.z());
   
   v -= pos;
   double d = v.Dot(v);
   double f = v.Dot(-dir);
   
   double r2  = d-(f*f);
   if(r2 <=0.0) return 0.0;
   else return sqrt(r2) ;
   }
 */
 
 void Print(){
   cout << "AMSTRDHTrack - Info" << endl;
 };
 AMSTRDHTrack *  next(){return (AMSTRDHTrack*)_next;}
 
 static integer build(int rerun);
 static AMSTRDHTrack * gethead(uinteger i=0);
 static integer Out(integer status);
 
#ifdef __WRITEROOT__
 friend class TrdHTrackR;
#endif
};

class PeakXYW { 
  public:
  double x,y;
  double w;

  PeakXYW(double x_, double y_,double w_):x(x_),y(y_),w(w_){}
  PeakXYW():x(0.),y(0.),w(0.){}
};

class TH2A{   // 2-dim histo with internal int[] array

 private:
  int nbx,nby,i,size;
  double Xlo,Xup,Ylo,Yup,Fx,Fy;
  double *Hxy;

 public:

  TH2A(const char* name, const char* title, int nbx, double Xlo, double Xup, int nby, double Ylo, double Yup)
    : nbx(nbx), Xlo(Xlo), Xup(Xup), nby(nby), Ylo(Ylo), Yup(Yup)
    { Hxy  =  new double[nbx*nby]; 
      Fx   = (double)nbx/(Xup-Xlo);
      Fy   = (double)nby/(Yup-Ylo);
      size =  nbx*nby;             }

  void dump(){
    printf("nbx %i nby %i\n",nbx,nby);
    printf("Xlo %f Xup %f Ylo %f Yup %f Fx %f Fy %f\n",Xlo,Xup,Ylo,Yup,Fx,Fy);
    for(int i=0;i!=size;i++){
      if(Hxy[i]!=0)printf("bin %i entries %f\n",i,Hxy[i]);
    }
  }

  void Fill( double x, double y, double weight ){
    i = int((y-Ylo)*Fy)*nbx + int((x-Xlo)*Fx);
    if(i>0 && i<size) Hxy[i]+=weight;
  }

  void Reset(){ memset(Hxy, 0, sizeof(float)*nbx*nby); }

  PeakXYW GetPeak(int width,int debug=0) {
  
    // find bin with maximum entry:
    double Vmax=0.;
    int ixmx, iymx, imx;
    if(debug)printf("TH2A GetPeak size %i\n",size);
    for( i=0; i<size; i++){
      if( Hxy[i]>Vmax){ Vmax = Hxy[i]; imx=i; }
    }
    if(debug)printf("TH2A GetPeak nbx %i\n",nbx);
    ixmx = imx%nbx; iymx = imx/nbx;

    // get peak as entry-weighed average around maximum +- width:
    double w=0.0, sx=0.0, sy=0.0, sw=0.0;
    for(int ix=ixmx-width; ix<=ixmx+width; ix++){
      for(int iy=iymx-width; iy<=iymx+width; iy++) {
	 i  = iy*nbx+ix;
         if(i>0 && i<size){
            w  =     (double)Hxy[iy*nbx+ix];
           sx += w * (Xlo+((double)ix+0.5)/Fx);
           sy += w * (Ylo+((double)iy+0.5)/Fy);
           sw += w;
	   Hxy[iy*nbx+ix]=0; // clear entry to allow search for next peak 
	 }
      }
    }
    if(debug)printf("TH2A GetPeak sx %f sy %f sw %f\n",sx,sy,sw);
    
    if(sw<-1.e6 || sw==0){
      PeakXYW pxyw(0.,0.,-1.);
      return pxyw;
    }
    else{
      PeakXYW pxyw(sx/sw,sy/sw,sw);
      return pxyw;
    }
    
  }
 
};


class  BIN { 
 public:
  int x,y; 
  double c;
  BIN(int x_,int y_,double c_):x(x_),y(y_),c(c_){}
};
class TH2V{  // 2-dim histo with internal vector<BIN>

 private:

  int nbx,nby,ix,iy;
  bool add;
  double Xlo,Xup,Ylo,Yup,Fx,Fy;
  vector<BIN> histo;

 public:

  TH2V(const char* name, const char* title, int nbx, double Xlo, double Xup, int nby, double Ylo, double Yup)
    : nbx(nbx), Xlo(Xlo), Xup(Xup), nby(nby), Ylo(Ylo), Yup(Yup)
    { Fx =(double)nbx/(Xup-Xlo);
      Fy =(double)nby/(Yup-Ylo);  }

  void dump(){
    printf("nbx %i nby %i ix %i iy %i\n",nbx,nby,ix,iy);
    printf("Xlo %f Xup %f Ylo %f Yup %f Fx %f Fy %f\n",Xlo,Xup,Ylo,Yup,Fx,Fy);
    for(int i=0;i!=histo.size();i++){
      //printf("bin %i x %i y %i c %i\n",i,histo[i].x,histo[i].y,histo[i].c);
    }
  }

  void Fill( double x, double y, double weight ){
    ix = int((x-Xlo)*Fx);
    iy = int((y-Ylo)*Fy);
    add=true;
    for( int i=0; i<histo.size(); i++ ){
      if( ix==histo[i].x && iy==histo[i].y ){
        histo[i].c+=weight;
        add = false;
        break;
      }
    }
    if( add ){
      BIN bin(ix,iy,weight);
      histo.push_back(bin);
    }
  }

  void Reset(){ histo.clear(); }

  PeakXYW GetPeak(int width, int debug=0){

    double Vmax=0.;
    int ixmx, iymx;
    if(histo.size()==0){
      PeakXYW peak(0.,0.,-1.);
      return peak;
    }
    if(debug)printf("TH2V GetPeak size %i\n",histo.size());
    for( int i=0; i<histo.size(); i++){
      if( histo[i].c>Vmax){
          Vmax = histo[i].c; ixmx = histo[i].x; iymx = histo[i].y;
      }
    }
    if(debug)printf("TH2V GetPeak nbx %i\n",nbx);

    double w=0.0, sx=0.0, sy=0.0, sw=0.0;
    for( int i=0; i<histo.size(); i++){
      if( fabs(histo[i].x-ixmx)<=width && 
          fabs(histo[i].x-ixmx)<=width   ){
         w  =     (double)histo[i].c;
        sx += w * (Xlo+(Xup-Xlo)*((double)histo[i].x+0.5)/(double)nbx);
        sy += w * (Ylo+(Yup-Ylo)*((double)histo[i].y+0.5)/(double)nby);
        sw += w;
        histo.erase(histo.begin()+i);   // remove entry to allow search for next peak 
      }
    }
    
    if(debug)printf("TH2V GetPeak sx %f sy %f sw %f\n",sx,sy,sw);

    //    PeakXYW pxyw(sx/sw,sy/sw,sw);
    if(sw<1.e-6){
      PeakXYW pxyw(0.,0.,-1.);
      return pxyw;
    }
    else{
      PeakXYW pxyw(sx/sw,sy/sw,sw);
      return pxyw;
    }
    
    //    return pxyw;
  }

};

#endif
