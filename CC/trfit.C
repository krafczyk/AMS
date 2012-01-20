#include "root.h"
#include "trfit.h"
#include "timeid.h"
#define mipsFortran
#include "cfortran.h"
#include "DynAlignment.h"
#ifndef _PGTRACK_
const integer nx=41;
const integer ny=41;
const integer nz=180;
extern "C" void tkgetres_(float res[][7], int &nplan);
extern "C" void resetmagstat_();
extern "C" void setmagstat_();
extern "C" void rkmsinit_(float*);
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


AMSTimeID* TrTrackFitR::SAp=0;
AMSTimeID* TrTrackFitR::SetAligDB(const char *name,int datamc,bool force){

if(SAp){
if(force) {
 static int ci=0;
 if(ci++<100)cout<<"TrTackFitR::SetAligDB-W-RedefiningDB "<<endl;
 delete SAp;
}
else return SAp;
}
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

SAp=new AMSTimeID(AMSID(name,datamc==0?0:1),begin,end,sizeof(_gldb),(void*)&(_gldb[0][0][0][0]),AMSTimeID::Standalone,true);
return SAp;
}
AMSTimeID* TrTrackFitR::SetAligDB_TDVR(const char *name,int datamc,bool force){

if(SAp){
if(force) {
 static int ci=0;
 if(ci++<100)cout<<"TrTackFitR::SetAligDB_TDVR-W-RedefiningDB "<<endl;
 delete SAp;
}
else return SAp;
}

    tm begin;
    tm end;
    begin.tm_isdst=0;
    end.tm_isdst=0;
    begin.tm_sec  =1;
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
    end.tm_year=130;
SAp=new AMSTimeID(AMSID(name,datamc==0?0:1),begin,end,sizeof(_gldb),(void*)&(_gldb[0][0][0][0]),AMSTimeID::Standalone,false);
time_t i,b,e;
SAp->gettime(i,b,e);
b=0;
SAp->SetTime(i,b,e);
for (int k=0;k<sizeof(_gldb)/sizeof(AMSEventR::if_t);k++){
 AMSEventR::if_t value;
int err=AMSEventR::Head()->GetTDVEl(name,k,value);
if(err){
  delete SAp;
  SAp=0;
  return 0;
}
else{
memcpy((char*)(((AMSEventR::if_t*)&(_gldb[0][0][0][0]))+k),&value,sizeof(value));
}
}

return SAp;
}
TrTrackFitR::gldb_def  TrTrackFitR::_gldb[trc::maxsen+1][trc::maxlad+1][2][trc::maxlay];
#ifdef __ROOTSHAREDLIBRARY__







int TrTrackFitR::getHit(unsigned int lay, AMSPoint & hit){
  const float sgm[3]={0.003,0.0016,0.003};
  const double zc[9]={159.,53.,29.2,25.2,1.71,-2.29,-25.2,-29.2,-136};
    AMSEventR *h=AMSEventR::Head();
    if(!h)return 2;
    TrTrackR * ptr= dynamic_cast<TrTrackR*>(fTrTrack);
    if(!ptr)return 3;
   if(Alig==0){
    for(int i=0;i<ptr->NTrRecHit();i++){
    TrRecHitR rh=h->TrRecHit(ptr->iTrRecHit(i));
      if(rh.lay()==lay){
         hit=AMSPoint(rh.Hit[0],rh.Hit[1],rh.Hit[2]);
         return 0;
      }
    }
   }   
   else if(Alig==1){
    for(int i=0;i<ptr->NHits();i++){
      if(lay==ptr->Layer(i)+1){
         hit=AMSPoint(ptr->Hit[i][0],ptr->Hit[i][1],ptr->Hit[i][2]);
         return 0;
      }
    }
   }
   else if(Alig==2){
     // Alignment using external tools
     for(int i=0;i<ptr->NHits();i++){
       TrRecHitR rh=h->TrRecHit(ptr->iTrRecHit(i));
       if(lay==1 || lay==9){
	 if(rh.lay()==lay){
	   int ret=DynHit(&rh,hit);
	   if(ret) return 4; else return 0;
	 }
       }else{
	 if(lay==ptr->Layer(i)+1){
	   hit=AMSPoint(ptr->Hit[i][0],ptr->Hit[i][1],ptr->Hit[i][2]);
	   return 0;
	 }
       }
     }
   }
   else if(Alig==3){
//  recreate hits using timeid alignment structure
    if(SAp){
      time_t tm=h->UTime();
      int ret=SAp->validate(tm);
      if(!ret)return 4;
    }
    else return 5;
    for(int i=0;i<ptr->NHits();i++){
      TrRecHitR rh=h->TrRecHit(ptr->iTrRecHit(i));
       if(lay==1 || lay==9){
	 if(rh.lay()==lay){
	   int ret=DynHit(&rh,hit,SAp);
           if(ret)return 4;
           TrRecHitR rh2(rh);
           for(int k=0;k<3;k++)rh2.Hit[k]=hit[k];
           hit=CrHit(&rh2);
           return 0;
	 }
       }
      else{
      if(rh.lay()==lay){
        hit=CrHit(&rh);
        return 0;
      }
     }
    }
   }
    else{
      static unsigned int er=0; 
      if(er++<100)cerr<<"TrTrackFitR::getHit-E-AligNotYetImplemented "<<Alig <<endl;
      return 6;
    }
    double z=lay>0 && lay<=sizeof(zc)/sizeof(zc[0])?zc[lay-1]:0;
    hit=AMSPoint(0,0,z);
    return 1;
}

TKFIELD_DEF TKFIELD;

int TrTrackFitR::Fit( TrTrackR *ptr){
  const float sgm[3]={0.003,0.0016,0.003};
  const double zc[9]={159.,53.,29.2,25.2,1.71,-2.29,-25.2,-29.2,-136};
  int npt=NHits();
  const int maxhits=10;
   if(npt>maxhits)return 1;

   float hits[maxhits][3];
   float hitss[maxhits][3];
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
    AMSEventR *h=AMSEventR::Head();
    if(!h)return 4;
   if(Alig==0){
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
   else if(Alig==1){
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
   else if(Alig==2){
     for(int i=0;i<ptr->NHits();i++){
       int lay=ptr->Layer(i)+1;
       if(CheckLayer(lay)){
	 if(lay==1 || lay==9){
	   TrRecHitR rh=h->TrRecHit(ptr->iTrRecHit(i));
	   AMSPoint hit;
	   int ret=DynHit(&rh,hit);
	   if(!ret) return 2;
	   for(int k=0;k<3;k++)hits[nh][k]=hit[k]; 
	   for(int k=0;k<3;k++)sigma[nh][k]=rh.EHit[k]; 
	 }else{
	   for(int k=0;k<3;k++)hits[nh][k]=ptr->Hit[i][k]; 
	   for(int k=0;k<3;k++){
	     sigma[nh][k]=ptr->EHit[i][k]; 
	     if(sigma[nh][k]<1.e-4 || sigma[nh][k]!=sigma[nh][k]){
	       sigma[nh][k]=sgm[k];
	     }
	   }
	 }	
	 layer[nh]=lay;
	 nh++;
       }
     }
   }
   else if(Alig==3){
//  recreate hits using timeid alignment structure
    if(SAp){
      time_t tm=h->UTime();
      int ret=SAp->validate(tm);
      if(!ret){
          return 2;
      }
    }
    else return 5;
    for(int i=0;i<ptr->NHits();i++){
      TrRecHitR rh=h->TrRecHit(ptr->iTrRecHit(i));
      int lay=ptr->Layer(i)+1;
      if(CheckLayer(lay)){
        AMSPoint Hit;
	 if(lay==1 || lay==9){
	   int ret=DynHit(&rh,Hit);
	   if(!ret) return 2;
            TrRecHitR rh2(rh);
           for(int k=0;k<3;k++)rh2.Hit[k]=Hit[k];
           Hit=CrHit(&rh2);
        }
        else Hit=CrHit(&rh);
        for(int k=0;k<3;k++)hits[nh][k]=Hit[k]; 
        for(int k=0;k<3;k++)sigma[nh][k]=rh.EHit[k]; 
        layer[nh]=lay;
        nh++;
      }
     }
    }
    else{
      static unsigned int er=0; 
      if(er++<100)cerr<<"TrTrackFitR::Fit-E-AligNotYetImplemented "<<Alig <<endl;
    }
     int algo=0;
    if(Algo==0)algo=1;
    else if(Algo==1)algo=4;
    else if(Algo==2)algo=5;
     int ms=MS%100;
      for(int i=0;i<maxhits;i++){
          for(int j=0;j<3;j++)hitss[i][j]=hits[i][j];
      }
      TKFITG(nh,hitss,sigma,normal,ipart,algo,ms,layer,out);
      float _res[9][7];
      TrSCooR coos[9]; 
      int ml=9;      
      tkgetres_(_res,ml);
      for(int k=0;k<nh;k++){
         coos[k].Coo=AMSPoint(_res[k][0],_res[k][1],_res[k][2]);
      }
      Rigidity=out[5];
      ErrRigidity=out[8];
      Chi2=(out[7]!=0 && out[7]!=2)?FLT_MAX:out[6];
      Coo=AMSPoint(out[0],out[1],out[2]);  
      Theta=out[3];
      Phi=out[4];     

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
     for(int i=0;i<maxhits;i++){
          for(int j=0;j<3;j++)hitss[i][j]=hits[i][j];
      }

          TKFITG(nh,hitss,sigma,normal,ipart,algo,ms,layer,out);
}


        }
   AMSDir pntdir(0,0,-1);
    AMSPoint pntplane(ptr->Hit[i0][0],ptr->Hit[i0][1],ptr->Hit[i0][2]);
    getHit(k+1,pntplane);

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


   
    

//      if(out[7]!=0){
//          cerr<<"  out[7] "<<out[7]<<endl;
//      }
      return out[7]!=0?3:0;
}

int TrTrackFitR::InitMF(unsigned int time){
static int initdone=0;
int ret=-1;
#pragma omp master
if(!initdone){
 
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
                        (void*)TKFIELD.isec,AMSTimeID::Standalone,false);

  time_t tmt=time;
  int ret=tdvdb->validate(tmt);
  setmagstat_();
  resetmagstat_();
  cout <<"TrTrackFitR-I-MagneticFieldMap "<<FieldMapName <<" initialized "<<time<<endl;
  float zposl[9]={159.,53.,29.2,25.2,1.71,-2.29,-25.2,-29.2,-136};
  rkmsinit_(zposl);
  initdone=1;
}
while(!initdone){
 usleep(1);
}
  return ret==1?0:1;
}


int TrTrackFitR::DynHit(TrRecHitR * rh,AMSPoint &HitA,AMSTimeID*sap){
  AMSEventR *h=AMSEventR::Head();
  double x=0,y=0,z=0; 
  if(!DynAlManager::FindAlignment(*h,*rh,x,y,z)) return -1;
  HitA=AMSPoint(x,y,z);
  return 0;
}

AMSPoint TrTrackFitR::CrHit(TrRecHitR * rh){
  AMSPoint Hit=AMSPoint(rh->Hit[0],rh->Hit[1],rh->Hit[2]);
  
  gldb_par *par(0);
  if( (par=SearchDB(rh))){
    AMSPoint HitA;
    for(int j=0;j<3;j++)HitA[j]=(par->getcoo())[j]+
      (par->getmtx(j)).prod(Hit);
    
    return HitA;
  }
  else return Hit;
}

TrTrackFitR::gldb_par * TrTrackFitR::SearchDB(TrRecHitR *rh){
static gldb_par par;
#pragma omp threadprivate (par)
if(!_gldb[trc::maxsen][trc::maxlad][1][0].nentries)return 0;
bool lyonly=false;
bool laonly=false;
for(int i=0;i<trc::maxlay;i++){
if(_gldb[trc::maxsen][trc::maxlad][0][i].nentries){
  lyonly=true;
  break;
}
}
for(int i=0;i<trc::maxlay;i++){
if(_gldb[trc::maxsen][0][0][i].nentries){
  laonly=true;
  break;
}
}

if(lyonly){
          int i=rh->lay()-1;
          par.setpar(_gldb[trc::maxsen][trc::maxlad][0][i].getcoo(),_gldb[trc::maxsen][trc::maxlad][0][i].getang());
}   
else if(laonly){
    integer lad[2][trc::maxlay];    
     int i=rh->lay()-1;
     lad[0][i]=rh->lad();
     lad[1][i]=rh->half();
          int ld=lad[0][i]-1;
          if(ld>=0){
           par.setpar(_gldb[trc::maxsen][ld][lad[1][i]][i].getcoo(),_gldb[trc::maxsen][ld][lad[1][i]][i].getang());
          }
          else{
           par.setpar(AMSPoint(),AMSPoint());
          }
}
else{
    integer lad[2][trc::maxlay];    
     int i=rh->lay()-1;
     lad[0][i]=rh->lad()-1;
     lad[1][i]=rh->half();
     lad[2][i]=rh->sen()-1;
          if(lad[0][i]>=0){
           par.setpar(_gldb[lad[2][i]][lad[0][i]][lad[1][i]][i].getcoo(),_gldb[lad[2][i]][lad[0][i]][lad[1][i]][i].getang());
          }
          else{
           par.setpar(AMSPoint(),AMSPoint());
          }
}
    return &par;

}

void TrTrackFitR::gldb_par::setpar(const AMSPoint & coo, const AMSPoint & angles){
_Coo=coo;
_Angles=angles;
double pitch=angles[0];
double yaw=angles[1];
double roll=angles[2];
  double cp=cos(pitch);
  double sp=sin(pitch);
  double cy=cos(yaw);
  double sy=sin(yaw);
  double cr=cos(roll);
  double sr=sin(roll);
  double l1=cy*cp;
  double m1=-sy;
  double n1=cy*sp;
  double l2=cr*sy*cp-sr*sp;
  double m2=cr*cy;
  double n2=cr*sy*sp+sr*cp;
  double l3=-sr*sy*cp-cr*sp;
  double m3=-sr*cy;
  double n3=-sr*sy*sp+cr*cp;
  _Dir[0]=AMSDir(l1,m1,n1);
  _Dir[1]=AMSDir(l2,m2,n2);
  _Dir[2]=AMSDir(l3,m3,n3);



}
#else
int TrTrackFitR::DynHit(TrRecHitR * rh,AMSPoint &HitA,AMSTimeID*sap){
return 0;
}
AMSPoint TrTrackFitR::CrHit(TrRecHitR * rh){
return AMSPoint();
}
TrTrackFitR::gldb_par * TrTrackFitR::SearchDB(TrRecHitR *rh){
return 0;
}
void TrTrackFitR::gldb_par::setpar(const AMSPoint & coo, const AMSPoint & angles){
}
int TrTrackFitR::Fit( TrTrackR * ptr){
return -1;
}
int TrTrackFitR::getHit(unsigned int lay, AMSPoint & hit){
return 2;
}
int TrTrackFitR::InitMF(unsigned int time){
return -1;
}
#endif



