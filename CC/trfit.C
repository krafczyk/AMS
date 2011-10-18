#include "root.h"
#include "trfit.h"
#include "timeid.h"
#define mipsFortran
#include "cfortran.h"
#ifndef _PGTRACK_
const integer nx=41;
const integer ny=41;
const integer nz=180;
extern "C" void resetmagstat_();
extern "C" void setmagstat_();
class TKFIELD_DEF{
public:
int mfile[40];
int iniok;
int isec[2];
int imin[2];
int ihour[2];
int iday[2];
int imon[2];
int iyear[2];
int na[3];
float  x[nx];
float  y[ny];
float  z[nz];
float bx[nz][ny][nx];
float by[nz][ny][nx];
float bz[nz][ny][nx];
float xyz[nx+ny+nz];
float bdx[2][nz][ny][nx];
float bdy[2][nz][ny][nx];
float bdz[2][nz][ny][nx];
};
#define TKFIELD COMMON_BLOCK(TKFIELD,tkfield)
COMMON_BLOCK_DEF(TKFIELD_DEF,TKFIELD);
#endif


extern "C" void tkfitg_(int &npt,float hits[][3], float sigma[][3],
float normal[][3], int &ipart,  int &ialgo, int &ims, int layer[], float out[]);
#define TKFITG tkfitg_

extern "C" void tkfitpar_(float init[7], float & chrg, float point[6],
                float out[7], double m55[5][5], float & step);
#define TKFITPAR tkfitpar_


#ifdef __ROOTSHAREDLIBRARY__

TKFIELD_DEF TKFIELD;

int TrTrackFitR::Fit( TrTrackR *ptr){
  const float sgm[3]={0.003,0.0016,0.003};
  const double zc[9]={159.,53.,29.2,25.2,1.71,-2.29,-25.2,-29.2,-136};
  int npt=NHits();
  const int maxhits=10;
   if(npt>maxhits)return 1;

   float hits[maxhits][3];
   float sigma[maxhits][3];
   float normal[maxhits][3];
   int layer[maxhits];
   int ipart=14;
    if(ipart==2)ipart=5;
    if(ipart==3)ipart=6;
    float out[9];
    for(int i=0;i<npt;i++){
     normal[i][0]=0;
     normal[i][1]=0;
     normal[i][2]=-1;
    }
//  get hits;
    int nh=0;    
   if(Alig==0){
    AMSEventR *h=AMSEventR::Head();
    if(!h)return 4;
    for(int i=0;i<ptr->NTrRecHit();i++){
    TrRecHitR rh=h->TrRecHit(ptr->iTrRecHit(i));
      int lay=ptr->Layer(i)+1;
      if(CheckLayer(lay)){
        for(int k=0;k<3;k++)hits[nh][k]=rh.Hit[k]; 
        for(int k=0;k<3;k++)sigma[nh][k]=rh.EHit[k];
        layer[nh]=lay;
       nh++;
      }    
   }
   }
   else{
    for(int i=0;i<ptr->NHits();i++){
      int lay=ptr->Layer(i)+1;
      if(CheckLayer(lay)){
        for(int k=0;k<3;k++)hits[nh][k]=ptr->Hit[i][k]; 
        for(int k=0;k<3;k++){
          sigma[nh][k]=ptr->EHit[i][k]; 
        if(sigma[nh][k]<1.e-4 || sigma[nh][k]!=sigma[nh][k]){
          sigma[nh][k]=sgm[k];
        }
        }
        layer[nh]=lay;
        nh++;
      }
     }
     }
     int algo=0;
    if(Algo==0)algo=1;
    else if(Algo==1)algo=4;
    else if(Algo==2)algo=5;
     int ms=MS%100;
      TKFITG(nh,hits,sigma,normal,ipart,algo,ms,layer,out);
      Rigidity=out[5];
      ErrRigidity=out[8];
      Chi2=(out[7]!=0 && out[7]!=2)?FLT_MAX:out[6];
      Coo=AMSPoint(out[0],out[1],out[2]);  
      Theta=out[3];
      Phi=out[4];     
      TrSCooR coos[9]; 

      int extres=MS/100;
      const int maxlay=9;
      for (int k=0;k<maxlay;k++){
      int i0=-1;
      for(int i=0;i<ptr->NHits();i++){
        int lay=ptr->Layer(i);
        if (lay==k){
         i0=i;
         break; 
        }
       }
       if(i0>-1){
        bool fitdone=false;
        if(extres && nh>3){


resetmagstat_();
for(int ik=0;ik<nh;ik++){
  if(layer[ik]==k+1){
          double sgm[3];
          float hits_a[10][3];
          float sigma_a[10][3];
          int layer_a[10];
          float normal_a[10][3];
          for(int ika=0;ika<ik;ika++){
            layer_a[ika]=layer[ika];
            for(int ika3=0;ika3<3;ika3++){
             sigma_a[ika][ika3]=sigma[ika][ika3];
             normal_a[ika][ika3]=normal[ika][ika3];
             hits_a[ika][ika3]=hits[ika][ika3];
            }  
          }   
          for(int ika=ik+1;ika<npt;ika++){
            layer_a[ika-1]=layer[ika];
            for(int ika3=0;ika3<3;ika3++){
             sigma_a[ika-1][ika3]=sigma[ika][ika3];
             normal_a[ika-1][ika3]=normal[ika][ika3];
             hits_a[ika-1][ika3]=hits[ika][ika3];
            }  
          }   
          int npt1=nh-1;
          TKFITG(npt1,hits_a,sigma_a,normal_a,ipart,algo,ms,layer_a,out);

          fitdone=true;
          break;
  }
}
if(!fitdone){
          TKFITG(nh,hits,sigma,normal,ipart,algo,ms,layer,out);
}


        }
   AMSDir pntdir(0,0,-1);
    AMSPoint pntplane(ptr->Hit[i0][0],ptr->Hit[i0][1],ptr->Hit[i0][2]);

  float outa[7];
  double m55[5][5]={{0,0,0,0,1},{0,0,0,1,0},{0,0,1,0,0},{0,1,0,0,0},{1,0,0,0,0}};
  float init[7];
  float point[6];
  float charge=1;
  AMSDir dir(out[3],out[4]);
  init[0]=out[0];
  init[1]=out[1];
  init[2]=out[2];
  init[3]=dir[0];
  init[4]=dir[1];
  init[5]=dir[2];
  init[6]=out[5];
  point[0]=pntplane[0];
  point[1]=pntplane[1];
  point[2]=pntplane[2];
  point[3]=pntdir[0];
  point[4]=pntdir[1];
  point[5]=pntdir[2];
  float slength;
  int size=k;
  TKFITPAR(init, charge,  point,  outa,  m55, slength);
  coos[size].Coo=AMSPoint(outa[0],outa[1],outa[2]);
  AMSDir d1(outa[3],outa[4],outa[5]);
  if(dir.prod(d1)<0){
   for(int k=0;k<3;k++)outa[3+k]=-outa[3+k];
  }
  coos[size].Theta=acos(outa[5]);
  coos[size].Phi=atan2(outa[4],outa[3]); 
    AMSEventR *h=AMSEventR::Head();
    if(h){
     TrRecHitR rh=h->TrRecHit(ptr->iTrRecHit(i0));
      if(rh.lay()==k+1){
       coos[size].Id=rh.Id;
      }
      else {
         cerr<<"TrTrackFitR::Fit-E-TrRecHitLayerLogicError" <<rh.Id<<" "<<k+1<<endl;
         coos[size].Id=k+1;
      }
     }
     else{
       if(fTrSCoo.size()>i0){
        coos[size].Id=fTrSCoo[k].Id;
       }
       else{
        coos[size].Id=k+1;
       }
     }
      
    }
    else{
// No hit but still extrapolation is needed
    AMSDir pntdir(0,0,-1);
    AMSPoint pntplane(0,0,zc[k]);
    if(fTrSCoo.size()>k)pntplane=fTrSCoo[k].Coo;

  float outa[7];
  double m55[5][5]={{0,0,0,0,1},{0,0,0,1,0},{0,0,1,0,0},{0,1,0,0,0},{1,0,0,0,0}};
  float init[7];
  float point[6];
  float charge=1;
  AMSDir dir(out[3],out[4]);
  init[0]=out[0];
  init[1]=out[1];
  init[2]=out[2];
  init[3]=dir[0];
  init[4]=dir[1];
  init[5]=dir[2];
  init[6]=out[5];
  point[0]=pntplane[0];
  point[1]=pntplane[1];
  point[2]=pntplane[2];
  point[3]=pntdir[0];
  point[4]=pntdir[1];
  point[5]=pntdir[2];
  float slength;
  int size=k;
  TKFITPAR(init, charge,  point,  outa,  m55, slength);
  coos[size].Coo=AMSPoint(outa[0],outa[1],outa[2]);
  AMSDir d1(outa[3],outa[4],outa[5]);
  if(dir.prod(d1)<0){
   for(int k=0;k<3;k++)outa[3+k]=-outa[3+k];
  }
  coos[size].Theta=acos(outa[5]);
  coos[size].Phi=atan2(outa[4],outa[3]); 
  if(fTrSCoo.size()>i0){
    coos[size].Id=fTrSCoo[k].Id;
  }
  else coos[size].Id=k+1;
 }
}
fTrSCoo.clear();
for(int k=0;k<sizeof(coos)/sizeof(coos[0]);k++)fTrSCoo.push_back(coos[k]);


   
    

  
      return out[7];
}

int TrTrackFitR::InitMF(unsigned int time){
static int initdone=0;
if(!initdone++){
 
 tm begin;
  tm end;

  begin.tm_isdst=0;
  end.tm_isdst=0;
  begin.tm_sec  =0;
  begin.tm_min  =0;
  begin.tm_hour =0;
  begin.tm_mday =0;
  begin.tm_mon  =0;
  begin.tm_year =0;

  end.tm_sec=0;
  end.tm_min=0;
  end.tm_hour=0;
  end.tm_mday=0;
  end.tm_mon=0;
  end.tm_year=0;
  char FieldMapName[255];
  const unsigned int t1=1283012891+86400; 
  if(time<t1){
    strcpy(FieldMapName,"MagneticFieldMapP");
  }
  else{ 
     strcpy(FieldMapName,"MagneticFieldMapP");
  }
int ssize=sizeof(TKFIELD_DEF)-sizeof(TKFIELD.mfile)-sizeof(TKFIELD.iniok);
 AMSTimeID *tdvdb=new AMSTimeID(AMSID(FieldMapName,1),
                        begin,end,ssize,
                        (void*)TKFIELD.isec,AMSTimeID::Standalone,true);

  time_t tmt=time;
  int ret=tdvdb->validate(tmt);
  setmagstat_();
  resetmagstat_();
  cout <<"TrTrackFitR-I-MagneticFieldMap "<<FieldMapName <<" initialized "<<time<<endl;
  return ret==1?0:1;

}
else return -1;
}
#else
int TrTrackFitR::Fit( TrTrackR * ptr){
return -1;
}
int TrTrackFitR::InitMF(unsigned int time){
return -1;
}
#endif



