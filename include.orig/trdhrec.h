#ifndef __AMSTRDHREC__
#define __AMSTRDHREC__
#include "trdrec.h"
#include "vector.h"
#include "event.h"

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
  float      em,er;
  integer    nhits;
  float      chi2;
  vector<AMSTRDRawHit*> fTRDRawHit;
  
  //  static integer _addnext(integer pat, integer nhit, uinteger iseg,AMSTRDRawHit* pthit[]);
  static integer _addnext(integer d, float m, float em, float r, float er,float z, float w, integer nhits, AMSTRDRawHit* pthit[]);
  void _addnextR(uinteger iseg);

  AMSTRDHSegment(integer d_, float m_, float em_, float r_, float er_,float z_, float w_)
    : AMSlink(),d(d_), m(m_), em(em_), r(r_), er(er_), z(z_), w(w_) 
    {};

    AMSTRDHSegment(integer d_, float m_, float em_, float r_, float er_, float z_, float w_, integer nhits_, AMSTRDRawHit* pthit[])
      : AMSlink(),d(d_), m(m_), em(em_), r(r_), er(er_), z(z_) , w(w_), nhits(nhits_)
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
  
  int LinReg2(int debug);

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
  float emx,emy;
  float ex,ey;
  int nhits;
  vector<AMSTRDHSegment*> fTRDHSegment;

  int hit_arr[20][18][16];
  int amp_arr[20][18][16];

  static integer _addnext(float pos[], float dir[], AMSTRDHSegment* pthit[]);
  void _addnextR(uinteger iseg);

  AMSTRDHTrack(float pos_[3],float dir_[3]):AMSlink(),chi2(0.),nhits(0),emx(0.),ex(0.),emy(0.),ey(0.)
   {
     float mag=0.;
     for (int i=0;i!=3;i++){
       pos[i]=pos_[i];
       dir[i]=dir_[i];
       mag+=pow(dir_[i],2);
     }
     for (int i=0;i!=3;i++)dir[i]/=sqrt(mag);

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
 
 float Theta(){ return acos(dir[2]);}

 float ETheta(){
   float r=sqrt(pow(dir[0],2)+pow(dir[1],2));
   float drdx=dir[0]/sqrt(pow(dir[0],2)+pow(dir[1],2));
   float drdy=dir[1]/sqrt(pow(dir[0],2)+pow(dir[1],2));
   float er=sqrt(pow(drdx*emx,2)+pow(drdy*emy,2));

   float dfdr=1./(1+pow(r,2));

   return dfdr*er;
 }

 float Phi(){ return atan2(dir[1],dir[0]);}
 
 float EPhi(){
   float dfdx=dir[1]/(pow(dir[0],2)+pow(dir[1],2));
   float dfdy=1./(1+pow(dir[1]/dir[0],2))/dir[0];
   return sqrt(pow(dfdx*emx,2)+pow(dfdy*emy,2));}

 void SetSegment(AMSTRDHSegment* segx, AMSTRDHSegment* segy){
   fTRDHSegment.push_back(segx);
   fTRDHSegment.push_back(segy);

   nhits=(int)segx->fTRDRawHit.size()+(int)segy->fTRDRawHit.size();
   chi2=segx->chi2+segy->chi2;

 }
 
 float Chi2(){return chi2;}
 
 void setChi2(float chi2_){chi2=chi2_;};
 
 void propagateToZ(float z, float &x , float& y){
   x=pos[0]+dir[0]/dir[2]*(z-pos[2]);
   y=pos[1]+dir[1]/dir[2]*(z-pos[2]);
 };


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

class PeakXYZW {
public:
  float x,y,z;
  float w;
  float zmin,zmax;

 PeakXYZW(float x_, float y_,float z_,float zmin_, float zmax_,float w_):x(x_),y(y_),z(z_),w(w_),zmin(zmin_),zmax(zmax_){}
 PeakXYZW():x(0.),y(0.),z(0.),w(0.){}

  bool operator<(const PeakXYZW& other) const {
    return w < other.w;
  }
  bool operator==(const PeakXYZW& other) const {
    if(w != other.w)return 0;
    if(x != other.x)return 0;
    if(y != other.y)return 0;
    //    if(z != other.z)return 0;
    return 1;//return ( && x == other.x && y == other.y);
  }
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

 
};


class  BIN {
public:
  int x,y;
  float c,z,zmin,zmax;
  BIN(int x_,int y_,int z_, float c_):x(x_),y(y_),z(z_),c(c_),zmin(z_),zmax(z_){}

    bool operator<(const BIN& other) const {
      return c > other.c;
    }
};

inline bool myfunction (PeakXYZW a, PeakXYZW b){return a.w>b.w;};

inline bool binsfunction (BIN a, BIN b){return a.c>b.c;};

class TH2V{  // 2-dim histo with internal vector<BIN>

private:

  int nbx,nby,ix,iy;
  bool add;
  float Xlo,Xup,Ylo,Yup,Fx,Fy;

public:
  vector<BIN> histo;

  TH2V(const char* name, const char* title, int nbx, float Xlo, float Xup, int
       nby, float Ylo, float Yup)
    : nbx(nbx), Xlo(Xlo), Xup(Xup), nby(nby), Ylo(Ylo), Yup(Yup)
  { Fx =(float)nbx/(Xup-Xlo);
    Fy =(float)nby/(Yup-Ylo);  }

  void dump(){
    printf("nbx %i nby %i ix %i iy %i\n",nbx,nby,ix,iy);
    printf("Xlo %f Xup %f Ylo %f Yup %f Fx %f Fy %f\n",Xlo,Xup,Ylo,Yup,Fx,Fy);
    for(int i=0;i!=histo.size();i++){
      //printf("bin %i x %i y %i c %i\n",i,histo[i].x,histo[i].y,histo[i].c);
    }
  }
  void Fill( float x, float y, float za, float zb, float weight ){
    ix = int((x-Xlo)*Fx);
    iy = int((y-Ylo)*Fy);
    //    iz = int((z-Zlo)*Fz);
    add=true;
    for( int i=0; i<histo.size(); i++ ){
      if( ix==histo[i].x && iy==histo[i].y){// && iz == histo[i].z){
        histo[i].c+=weight;
	histo[i].z+=(za+zb)/2.;
	if(za<histo[i].zmin)histo[i].zmin=za;
	if(zb<histo[i].zmin)histo[i].zmin=zb;
	if(za>histo[i].zmax)histo[i].zmax=za;
	if(zb>histo[i].zmax)histo[i].zmax=zb;
        add = false;
        break;
      }
    }
    if( add ){
      BIN bin(ix,iy,(za+zb)/2.,weight);
      histo.push_back(bin);
    }
  }
  void Reset(){ histo.clear(); }

  vector<PeakXYZW> FindPeaks(int npeak=0);
};


int DoPrefit(vector<PeakXYZW> &seg_x, vector<PeakXYZW> &seg_y);
//vector<AMSTRDHSegment*> DoLinReg(vector<PeakXYZW> *segvec_x,vector<PeakXYZW> *segvec_y,int debug=0);
//AMSTRDHSegment* Refit(AMSTRDHSegment* seg,int debug=0);
//vector<AMSTRDHSegment*> clean_segvec(vector<AMSTRDHSegment*> vec,int debug=0);
AMSTRDHTrack* SegToTrack(AMSTRDHSegment *s1, AMSTRDHSegment* s2,int debug=0);
vector<pair<int,int> > check_secondaries();
bool check_geometry(AMSTRDHSegment *s1, AMSTRDHSegment* s2);
bool check_TRtop(AMSTRDHSegment *s1, AMSTRDHSegment* s2,int debug=0);
int  check_TOF(AMSTRDHSegment *s1, AMSTRDHSegment* s2,int debug=0);
int  combine_segments();

#endif
