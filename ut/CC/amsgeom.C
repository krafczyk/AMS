// Author V. Choutko 24-may-1996
// TOF Geometry E. Choumilov 22-jul-1996 
// ANTI Geometry E. Choumilov 2-06-1997 
// Passive Shield Geometry V. Choutko 22-jul-1996
// CTC (Cherenkov Thresh. Counter) geometry E.Choumilov 26-sep-1996
// ATC (Aerogel Threshold Cerenkov) geometry A.K.Gougas 14-Mar-1997 
//
#include <typedefs.h>
#include <node.h>
#include <snode.h>
#include <amsdbc.h>
#include <tofdbc.h>
#include <ctcdbc.h>
#include <antidbc.h>
#include <gmat.h>
#include <extC.h>
#include <stdlib.h>
#include <gvolume.h>
#include <amsgobj.h>
#include <job.h>
#include <commons.h>
#include <tkdbc.h>
extern "C" void mtx_(geant nrm[][3],geant vect[]);
extern "C" void mtx2_(number nrm[][3],geant  xnrm[][3]);
#define MTX mtx_
#define MTX2 mtx2_
void AMSgvolume::amsgeom(){
extern void tkgeom(AMSgvolume &);
extern void magnetgeom(AMSgvolume &);
extern void tofgeom(AMSgvolume &);
extern void antigeom(AMSgvolume &);
extern void pshgeom(AMSgvolume &);
extern void ctcgeom(AMSgvolume &);
AMSID amsid;
number nrm[3][3]={1.,0.,0.,0.,1.,0.,0.,0.,1.};
number inrm[3][3];
geant par[3];
integer gid=1;
      par[0]=AMSDBc::ams_size[0]/2;
      par[1]=AMSDBc::ams_size[1]/2;
      par[2]=AMSDBc::ams_size[2]/2;
static AMSgvolume mother("VACUUM",0,AMSDBc::ams_name,"BOX",par,
       3,AMSDBc::ams_coo,nrm,"ONLY",  0,gid);  // AMS mother volume
AMSJob::gethead()->addup( &mother);
magnetgeom(mother);
tofgeom(mother);
antigeom(mother);
if(strstr(AMSJob::gethead()->getsetup(),"AMSSTATION")==0){
 pshgeom(mother);
 cout <<" AMSGeom-I-Shuttle setup selected."<<endl;
}
else{
 cout <<" AMSGeom-I-Station setup selected."<<endl;
}
 tkgeom(mother);
 ctcgeom(mother);

GGCLOS();
AMSgObj::GVolMap.map(mother);
#ifdef __AMSDEBUG__
// if(AMSgvolume::debug)AMSgObj::GVolMap.print();
#endif

}

void magnetgeom(AMSgvolume & mother){
AMSID amsid;
geant par[6]={0.,0.,0.,0.,0.,0.};
number nrm[3][3]={1.,0.,0.,0.,1.,0.,0.,0.,1.};
number inrm[3][3];
char name[5]="MAGN";
geant coo[3]={0.,0.,0.};
integer gid=0;
AMSNode * cur;
AMSNode * dau;
AMSgtmed *p;
      gid=1;
      par[0]=112./2;
      par[1]=129.6/2;
      par[2]=40.;
      dau=mother.add(new AMSgvolume(
      "MAGNET",0,name,"TUBE",par,3,coo,nrm, "ONLY",0,gid));
      gid=2;
      par[0]=111.4/2.;
      par[1]=112.0/2.;
      par[2]=40.;
      dau=mother.add(new AMSgvolume(
      "1/2ALUM",0,"ALT1","TUBE",par,3,coo,nrm, "ONLY",0,gid));
      gid=3;
      par[0]=129.6/2.;
      par[1]=130.4/2.;
      par[2]=40.;
      dau=mother.add(new AMSgvolume(
      "1/2ALUM",0,"ALT2","TUBE",par,3,coo,nrm, "ONLY",0,gid++));
      par[0]=112/2.;
      par[1]=165./2.;
      par[2]=0.75;
      coo[2]=40.75;
      dau=mother.add(new AMSgvolume(
      "1/2ALUM",0,"ALT3","TUBE",par,3,coo,nrm, "ONLY",0,gid++));
    
      par[0]=112/2.;
      par[1]=165.6/2.;
      par[2]=0.75;
      coo[2]=-40.75;
      dau=mother.add(new AMSgvolume(
      "1/2ALUM",0,"ALT4","TUBE",par,3,coo,nrm, "ONLY",0,gid++));
    
}
//-------------------------------------------------------------------
void tofgeom(AMSgvolume & mother){ 
number pr[3]={0.,0.,0.};
geant par[6]={0.,0.,0.};
number nrm[3][3]={1.,0.,0., 0.,1.,0., 0.,0.,1.};
number nrm1[3][3]={1.,0.,0., 0.,1.,0., 0.,0.,1.};
number nrm2[3][3]={0.,-1.,0.,1.,0.,0., 0.,0.,1.};// for rotated layers (90degr)
number inrm[3][3];
number dz,dz1,dz2,zc,dx,dxt;
integer btyp,nbm,nrot;
char name[5];
number co[3]={0.,0.,0.};
geant coo[3]={0.,0.,0.};
integer gid=0;
integer nmed;
AMSNode *cur;
AMSNode *dau;
AMSgtmed *p;
AMSID amsid;
//
TOFDBc::readgconf();// read TOF-counters geometry parameters
//------
dz1=TOFDBc::supstr(7)/2.+TOFDBc::plnstr(1)+TOFDBc::plnstr(7)
   +TOFDBc::plnstr(6)/2.+TOFDBc::plnstr(3)/2.;//dz hon_str/outer_sc_fixation
//
dz2=TOFDBc::supstr(7)/2.+TOFDBc::plnstr(2)+TOFDBc::plnstr(7) 
   +TOFDBc::plnstr(6)/2.+TOFDBc::plnstr(3)/2.;//dz hon_str/inner_sc_fixation
//------
//          <-- create/position top supp. honeycomb structures
gid=1;
par[0]=0.;                //Ri
par[1]=TOFDBc::supstr(8); //Ro
par[2]=TOFDBc::supstr(7)/2.; //Dz
coo[0]=TOFDBc::supstr(3);    // x-shift from "0" of mother
coo[1]=TOFDBc::supstr(4);    // y-shift ...
coo[2]=TOFDBc::supstr(1)+TOFDBc::supstr(7)/2.;// z-centre of top supp. honeycomb
amsid=AMSID(0,"TOF_HONEYCOMB");  // set amsid for honeycomb struct.
p= (AMSgtmed *)AMSgObj::GTrMedMap.getp(amsid);
if(p)nmed=p->getmedia();
else{
  cerr<<"TOFGEOM-F-NoSuchMedium "<<amsid;
  exit(1);
    }
dau=mother.add(new AMSgvolume(
    nmed,0,"TOFH","TUBE",par,3,coo,nrm1,"ONLY",1,gid));
//--------------
//          <-- create/position bot supp. honeycomb structures
gid=2;
par[0]=0.;                //Ri
par[1]=TOFDBc::supstr(8); //Ro
par[2]=TOFDBc::supstr(7)/2.; //Dz
coo[0]=TOFDBc::supstr(5);    // x-shift from "0" of mother
coo[1]=TOFDBc::supstr(6);    // y-shift ...
coo[2]=TOFDBc::supstr(2)-TOFDBc::supstr(7)/2.;// z-centre of bot supp. honeycomb
amsid=AMSID(0,"TOF_HONEYCOMB");  // set amsid for honeycomb struct.
p= (AMSgtmed *)AMSgObj::GTrMedMap.getp(amsid);
if(p)nmed=p->getmedia();
else{
  cerr<<"TOFGEOM-F-NoSuchMedium "<<amsid;
  exit(1);
    }
dau=mother.add(new AMSgvolume(
    nmed,0,"TOFH","TUBE",par,3,coo,nrm1,"ONLY",1,gid));
//----------------------------------------------------------------------
//             <-- create/position S1-S4 sc. planes :
//
dx=TOFDBc::plnstr(5)+2.*TOFDBc::plnstr(13);// dx(width) of sc.counter(bar+cover)
dz=TOFDBc::plnstr(6)+2.*TOFDBc::plnstr(7);// dz(thickn)of sc.counter(bar+cover)
pr[0]=dx/2.;
pr[2]=dz/2.; 
//                                 
for (int ip=0;ip<SCLRS;ip++){ //  <<<=============== loop over sc. planes
//
  nbm=SCBRS[ip];                      // num. of bars in layer ip
  dxt=(nbm-1)*(dx-TOFDBc::plnstr(4)); // first-last sc.count. bars distance 
//                                   (betw.centers, taking into account overlaping)
  if(ip<2){
    co[0]=TOFDBc::supstr(3);// <--top TOF-subsystem X-shift
    co[1]=TOFDBc::supstr(4);// <--top TOF-subsystem Y-shift
  }
  if(ip>1){
    co[0]=TOFDBc::supstr(5);// <--bot TOF-subsystem X-shift
    co[1]=TOFDBc::supstr(6);// <--bot TOF-subsystem Y-shift
  }
//
  if(TOFDBc::plrotm(ip)==0){
    nrot=0;                    //  <-- for unrotated planes
    for(int i=0;i<3;i++)for(int j=0;j<3;j++)nrm[i][j]=nrm1[i][j];
  }
  if(TOFDBc::plrotm(ip)==1){
    nrot=SCROTN+ip;           // <-- for rotated planes
    for(int i=0;i<3;i++)for(int j=0;j<3;j++)nrm[i][j]=nrm2[i][j];
  }
//-----------
  for(int ib=0;ib<nbm;ib++){ // <<<====== loop over sc. counter bars in plane ip
//
//   <-- cr/position sc. counter (cover + scint. as solid cover)
//
    btyp=TOFDBc::brtype(ip,ib);
    if(btyp==0)continue;// skip physically missing counters
    pr[1]=TOFDBc::brlen(ip,ib)/2.+TOFDBc::plnstr(11); // dy/2 (sc.length+lg) 
    if(TOFDBc::plrotm(ip)==0){  // <-- unrotated planes
      coo[0]=co[0]-dxt/2.+ib*(dx-TOFDBc::plnstr(4));
      coo[1]=co[1];
      }
    if(TOFDBc::plrotm(ip)==1){  // <-- rotated planes
      coo[0]=co[0];
      coo[1]=co[1]-dxt/2.+ib*(dx-TOFDBc::plnstr(4));
    }
    coo[2]=TOFDBc::getzsc(ip,ib);
    co[2]=coo[2];//(save z-pos. for later use)
    for(int i=0;i<3;i++)par[i]=pr[i];
    amsid=AMSID(0,"TOF_SC_COVER");  // set amsid for sc. counter (sc+cover)
    p= (AMSgtmed *)AMSgObj::GTrMedMap.getp(amsid);
    if(p)nmed=p->getmedia();
    else{
      cerr<<"TOFGEOM-F-NoSuchMedium "<<amsid;
      exit(1);
        }
    gid=100*(ip+1)+ib+1;
    dau=mother.add(new AMSgvolume(
    nmed,nrot,"TOFC","BOX",par,3,coo,nrm,"MANY",1,gid));
//-------
//        <-- cr/position scintillator inside counter
//
    par[0]=pr[0]-TOFDBc::plnstr(13);// pure scint. x-size/2
    par[1]=pr[1]-TOFDBc::plnstr(11);// pure scint. y-size/2 (minus light guide)
    par[2]=pr[2]-TOFDBc::plnstr(7);// pure scint. z-size/2
    amsid=AMSID(0,"TOF_SCINT");  // set amsid for scint. itself
    p= (AMSgtmed *)AMSgObj::GTrMedMap.getp(amsid);
    if(p)nmed=p->getmedia();
    else{
      cerr<<"TOFGEOM-F-NoSuchMedium "<<amsid;
      exit(1);
        }
    dau=mother.add(new AMSgvolume(
    nmed,nrot,"TOFS","BOX",par,3,coo,nrm,"ONLY",1,gid));
//-------
//        <--- now put boxes, equivalent to (PMTs + shielding)
//                                    at both ends of sc. bar.
    par[0]=TOFDBc::plnstr(8)/2.;
    par[1]=TOFDBc::plnstr(9)/2.;
    par[2]=TOFDBc::plnstr(10)/2.;
    if(TOFDBc::plrotm(ip)==0){
      coo[0]=co[0]-dxt/2.+ib*(dx-TOFDBc::plnstr(4));
      coo[1]=co[1]-pr[1]-TOFDBc::plnstr(9)/2.;
    }
    if(TOFDBc::plrotm(ip)==1){
      coo[0]=co[0]-pr[1]-TOFDBc::plnstr(9)/2.;
      coo[1]=co[1]-dxt/2.+ib*(dx-TOFDBc::plnstr(4));
    }
    if(ip==0||ip==2)coo[2]=co[2]+TOFDBc::plnstr(12);
    if(ip==1||ip==3)coo[2]=co[2]-TOFDBc::plnstr(12);
    amsid=AMSID(0,"TOF_PMT_BOX");  //         <=== create pmt1-box
    p= (AMSgtmed *)AMSgObj::GTrMedMap.getp(amsid);
    if(p)nmed=p->getmedia();
    else{
      cerr<<"TOFGEOM-F-NoSuchMedium "<<amsid;
      exit(1);
        }
    gid=1000+100*(ip+1)+ib+1;
    dau=mother.add(new AMSgvolume(
    nmed,nrot,"TOFB","BOX",par,3,coo,nrm,"ONLY",1,gid));
//-----
    if(TOFDBc::plrotm(ip)==0){
      coo[0]=co[0]-dxt/2.+ib*(dx-TOFDBc::plnstr(4));
      coo[1]=co[1]+pr[1]+TOFDBc::plnstr(9)/2.;
    }
    if(TOFDBc::plrotm(ip)==1){
      coo[0]=co[0]+pr[1]+TOFDBc::plnstr(9)/2.;
      coo[1]=co[1]-dxt/2.+ib*(dx-TOFDBc::plnstr(4));
    }
    amsid=AMSID(0,"TOF_PMT_BOX");  //         <=== create pmt2-box
    p= (AMSgtmed *)AMSgObj::GTrMedMap.getp(amsid);
    if(p)nmed=p->getmedia();
    else{
      cerr<<"TOFGEOM-F-NoSuchMedium "<<amsid;
      exit(1);
        }
    gid=2000+100*(ip+1)+ib+1;
    dau=mother.add(new AMSgvolume(
    nmed,nrot,"TOFB","BOX",par,3,coo,nrm,"ONLY",1,gid));
    
  }      //   <<<============= end of sc. bars loop ==========
}   //   <<<============= end of sc. planes loop =============
}
//---------------------------------------------------------------
void antigeom(AMSgvolume & mother){
AMSID amsid;
geant par[6]={0.,0.,0.,0.,0.,0.};
number nrm[3][3]={1.,0.,0.,0.,1.,0.,0.,0.,1.};
number nrd[3][3];
geant coo[3]={0.,0.,0.};
integer i,nrot,gid=0,gidd=0;
geant scradi,scinth,scleng,wrapth,groovr,pdlgap,stradi,stleng,stthic;
geant rs,phi,phib,dphis,dphi,dphig;
geant degrad,raddeg;
integer nscpad;
AMSNode * pAmother;
AMSNode * pSegm;
AMSNode * pGroov;
AMSNode * p;
//
  raddeg=AMSDBc::raddeg;
  degrad=AMSDBc::pi/180.;
  ANTIDBc::setgeom();
  nscpad=MAXANTI;
  scradi=ANTIDBc::scradi();
  scinth=ANTIDBc::scinth();
  scleng=ANTIDBc::scleng();
  wrapth=ANTIDBc::wrapth();
  groovr=ANTIDBc::groovr();
  pdlgap=ANTIDBc::pdlgap();
  stradi=ANTIDBc::stradi();
  stleng=ANTIDBc::stleng();
  stthic=ANTIDBc::stthic();
  rs=scradi+0.5*scinth;
  dphi=360./float(nscpad);
  dphig=raddeg*pdlgap/scradi;// phi-thickness of paddle gap(degree)
  dphis=dphi-dphig;
//
// create ANTI-counter supp.tube volume as  cylinder:
//
  par[0]=stradi;
  par[1]=stradi+stthic;
  par[2]=stleng/2.;
  par[3]=0.;
  par[4]=360.;
  gid=100;
  p=mother.add(new AMSgvolume(
       "ANTI_SUPTB",0,"ASTB","TUBE",par,5,coo,nrm, "ONLY",0,gid));
//
// create ANTI-counter mother volume as wrapper-made cylinder:
//
  par[0]=scradi-wrapth;
  par[1]=scradi+scinth+wrapth;
  par[2]=scleng/2.+wrapth;
  par[3]=0.;
  par[4]=360.;
  gid=200;
  pAmother=mother.add(new AMSgvolume(
       "ANTI_WRAP",0,"AMOT","TUBE",par,5,coo,nrm, "ONLY",0,gid));
//
// ---> Loop to fill A-mother volume with sc.segments,sc.bumps and holes :
//
  gid=0;
  for(i=0;i<nscpad;i++){
    phi=i*dphi;
    phib=phi+dphis;
//
//     create/pos sc. segment in A-mother:
//
    par[0]=scradi;
    par[1]=scradi+scinth;
    par[2]=0.5*scleng;
    par[3]=phi;
    par[4]=phib;
    coo[0]=0.;
    coo[1]=0.;
    coo[2]=0.;
    gid+=1;
    pSegm=pAmother->add(new AMSgvolume(
       "ANTI_SCINT",0,"ANTS","TUBS",par,5,coo,nrm, "MANY",1,gid));
//
//     create/pos groove in A-mother:
//
    nrot=SCROTN+100+i;
    nrd[0][0]=cos(phib*degrad);// rot.matrix(wrt Amother) for groove
    nrd[0][1]=-sin(phib*degrad);
    nrd[0][2]=0.;
    nrd[1][0]=sin(phib*degrad);
    nrd[1][1]=cos(phib*degrad);
    nrd[1][2]=0.;
    nrd[2][0]=0.;
    nrd[2][1]=0.;
    nrd[2][2]=1.;
    coo[0]=rs*cos(phib*degrad);
    coo[1]=rs*sin(phib*degrad);
    coo[2]=0.;
    par[0]=groovr-pdlgap;
    par[1]=groovr;
    par[2]=0.5*scleng;
    par[3]=0.;
    par[4]=180.;
    pGroov=pAmother->add(new AMSgvolume(
       "ANTI_WRAP",nrot,"AGRV","TUBS",par,5,coo,nrd, "ONLY",1,gid));
//
//     create/pos sc. bump in Amother:
//
    nrot=SCROTN+200+i;
    nrd[0][0]=cos(phib*degrad);// rot.matrix(wrt Amother) for bump
    nrd[0][1]=-sin(phib*degrad);
    nrd[0][2]=0.;
    nrd[1][0]=sin(phib*degrad);
    nrd[1][1]=cos(phib*degrad);
    nrd[1][2]=0.;
    nrd[2][0]=0.;
    nrd[2][1]=0.;
    nrd[2][2]=1.;
    coo[0]=rs*cos(phib*degrad);
    coo[1]=rs*sin(phib*degrad);
    coo[2]=0.;
    par[0]=0.;
    par[1]=groovr-pdlgap;
    par[2]=0.5*scleng;
    par[3]=0.;
    par[4]=180.;
    p=pAmother->add(new AMSgvolume(
       "ANTI_SCINT",nrot,"ANTB","TUBS",par,5,coo,nrd, "ONLY",1,gid));
  }// ---> end of sector loop
//
}
//---------------------------------------------------------------------

void ctcgeom(AMSgvolume & mother){
extern void ctcgeomE(AMSgvolume &, integer iflag);
extern void ctcgeomAG(AMSgvolume& );
extern void ctcgeomAGPlus(AMSgvolume& );
if(strstr(AMSJob::gethead()->getsetup(),"CTCAnnecyBasic")){
ctcgeomAG(mother);
  cout<<" CTCGeom-I-Annecy setup for CTC selected"<<endl;
}
else{
  ctcgeomAGPlus(mother);
  cout<<" CTCGeom-I-AnnecyPlus setup for CTC selected"<<endl;
}
}


void ctcgeomE(AMSgvolume & mother, integer iflag){
  // E. Choumilov version   ( New one )

CTCDBc::setgeom(iflag);

geant par[6]={0.,0.,0.,0.,0.,0.};
number nrm[3][3]={1.,0.,0., 0.,1.,0., 0.,0.,1.};
number dx,dy,dxag,dz,zc,zr,gp,gpw,wl,wls,gwls,step,xleft;
char name1[10];
char name2[10];
geant coo[3],cooz;
integer gid=0;
integer nmed,nblk,nwls,i,ind,loop;
AMSNode *cur;
AMSNode *dau;
AMSgtmed *p;
AMSID amsid;
//
zr=CTCDBc::getsupzc();
wl=CTCDBc::getwallth();
wls=CTCDBc::getwlsth();
gp=CTCDBc::getagap();
gpw=gp+wl;
gwls=CTCDBc::getwgap();
//
//          <-- create supporting honeyc. plate in mother
gid=1;
dx=0.5*CTCDBc::gethcsize(0);
dy=0.5*CTCDBc::gethcsize(1);
dz=0.5*CTCDBc::gethcsize(2);
par[0]=dx;                
par[1]=dy;
par[2]=dz;
coo[0]=0.;
coo[1]=0.;
coo[2]=zr-dz; // supp.plate center z-pos
dau=mother.add(new AMSgvolume(
    "CTC_HONEYCOMB",0,"SUPP","BOX ",par,3,coo,nrm,"ONLY",0,gid));
//
//--------------
//
nwls=CTCDBc::getnblk();// one WLS per cell (one big aer.block)
//
//    <-- create outer aerogel-cover (made of WALL material) in mother
gid=2;
dx=0.5*(CTCDBc::getagsize(0)+2.*gpw);
dy=0.5*(CTCDBc::getagsize(1)+2.*gpw);
if(iflag==0)
            dz=0.5*(CTCDBc::getagsize(2)+2.*gpw+wls+gwls);//no middle horiz. refl.wall
else
            dz=0.5*(CTCDBc::getagsize(2)+4.*gp+3.*wl);//incl. middle horiz. refl. wall 
par[0]=dx;                
par[1]=dy;
par[2]=dz;
coo[0]=0.;
coo[1]=0.;
coo[2]=zr+dz;// box center z-pos
dau=mother.add(new AMSgvolume(
    "CTC_WALL",0,"WALL","BOX ",par,3,coo,nrm,"MANY",0,gid));
//---
//  <--- put into this box "dummy_media"("air") to get space for aerogel
//
gid=3;
par[0]=dx-wl;                
par[1]=dy-wl;
par[2]=dz-wl;
coo[2]=zr+dz;
dau=mother.add(new AMSgvolume(
    "CTC_DUMMYMED",0,"DVOL","BOX ",par,3,coo,nrm,"MANY",0,gid));
//--------------
//          <-- add middle horiz. separator for side readout geometry(geom-1)
if(iflag==1){
gid=4;
par[0]=dx-wl;                
par[1]=dy-wl;
par[2]=0.5*wl;
coo[0]=0.;
coo[1]=0.;
coo[2]=zr+dz;// box center z-pos
dau=mother.add(new AMSgvolume(
    "CTC_WALL",0,"HSEP","BOX ",par,3,coo,nrm,"MANY",0,gid));
}
//--------------
//
//          <-- create pure aerogel-blocks in mother
//
nblk=CTCDBc::getnblk(); // number of big aerog. blocks (cells)
dy=0.5*CTCDBc::getagsize(1);
dz=0.5*CTCDBc::getagsize(2);
par[1]=dy;                
coo[1]=0.;
if(iflag==0){
  dx=0.5*(CTCDBc::getagsize(0)-(nblk-1)*(wl+2.*gp))/nblk;// half x-size
  dxag=dx;
  par[0]=dx;
  par[2]=dz;
  cooz=zr+wl+gwls+wls+gp+dz;// aerog. block center z-pos
  step=2.*dx+2.*gp+wl;//x-dist between blocks centers
  xleft=-0.5*CTCDBc::getagsize(0)+dx; // 1-st aerog.block center x-pos
}
else{
  dx=0.5*(CTCDBc::getagsize(0)-nblk*(wls+2.*gp)-(nblk-1)*(wl+2.*gp))/nblk/2;
  dxag=dx;
  par[0]=dx;
  par[2]=dz/2.;// half thickness of big block 
  cooz=zr+gpw+1.5*dz+2.*gp+wl;// for top row  
  step=4.*dx+2.*gp+wls+2.*gp+wl;//big step (till block in next cell)
  xleft=-0.5*CTCDBc::getagsize(0)+dx; // 1-st aerog.block centre x-pos
}
//
for(i=0;i<nblk;i++){ // <---- loop over aerog. blocks ----
  if(iflag==0){
    gid=11001+i;
    coo[0]=xleft+i*step;
    coo[2]=cooz;
    dau=mother.add(new AMSgvolume(
        "CTC_AEROGEL",0,"AGEL","BOX ",par,3,coo,nrm,"ONLY",1,gid));
  }
  if(iflag==1){
    gid=11001+2*i;
    coo[0]=xleft+i*step;
    coo[2]=cooz;
    dau=mother.add(new AMSgvolume(
        "CTC_AEROGEL",0,"AGEL","BOX ",par,3,coo,nrm,"ONLY",1,gid));//1-st in top row
    gid=12001+2*i;
    coo[2]=cooz-dz-2.*gp-wl;
    dau=mother.add(new AMSgvolume(
        "CTC_AEROGEL",0,"AGEL","BOX ",par,3,coo,nrm,"ONLY",1,gid));//1-st in bot row
//        
    gid=11002+2*i;
    coo[0]=xleft+i*step+2.*dxag+2.*gp+wls;
    coo[2]=cooz;
    dau=mother.add(new AMSgvolume(
        "CTC_AEROGEL",0,"AGEL","BOX ",par,3,coo,nrm,"ONLY",1,gid));//2-nd in top row
    gid=12002+2*i;
    coo[2]=cooz-dz-2.*gp-wl;
    dau=mother.add(new AMSgvolume(
        "CTC_AEROGEL",0,"AGEL","BOX ",par,3,coo,nrm,"ONLY",1,gid));//2-nd in bot row
  }
} // ----- end of aerog. blocks loop ----->
//--------------
//
//          <-- create vertical (separator) walls(geo=0/1) in mother
//
dx=0.5*wl;// half x-size
dy=0.5*CTCDBc::getagsize(1)+gp;
par[0]=dx;
par[1]=dy;                
coo[1]=0.;
if(iflag==0){
  dz=0.5*(CTCDBc::getagsize(2)+2.*gp+wls+gwls);
  par[2]=dz;
  coo[2]=zr+wl+dz;// separator center z-pos
  step=2.*dxag+2.*gp+wl;//x-dist between separator centers
  xleft=-0.5*CTCDBc::getagsize(0)+2.*dxag+gp+dx; // 1-st separator center x-pos
//                                       (between 1-st and 2-nd aerog.block)
}
else{
  dz=0.5*(CTCDBc::getagsize(2)+4.*gp+wl);
  par[2]=dz;
  coo[2]=zr+wl+dz; 
  step=4.*dxag+4.*gp+wl+wls;
  xleft=-0.5*CTCDBc::getagsize(0)+4.*dxag+3.*gp+wls+dx; // 1-st wall center x-pos
//                                       
}
//
loop=nblk-1;
for(i=0;i<loop;i++){ // <---- separators loop ----
  gid=51+i;
  coo[0]=xleft+i*step;
  dau=mother.add(new AMSgvolume(
      "CTC_SEP",0,"VSEP","BOX ",par,3,coo,nrm,"ONLY",1,gid));
} // ----- end of separators(WLS's) loop ----->

//--------------
//
//   <-- create bottom WLS-strips(for geom=0) in mother
//
if(iflag==0){
  dx=0.5*(CTCDBc::getagsize(0)+2.*gp-2.*nwls*gwls-(nwls-1)*wl)/nwls; //half x-size
  dy=0.5*CTCDBc::getagsize(1)+gp-gwls;
  dz=0.5*wls;
  par[0]=dx;                
  par[1]=dy;
  par[2]=dz;
  step=2.*dx+2.*gwls+wl;
  xleft=-0.5*(CTCDBc::getagsize(0)+2.*gp)+gwls+dx;// 1-st wls centre x-pos
  coo[1]=0.;
  coo[2]=zr+wl+gwls+dz;// WLS center z-pos
  for(i=0;i<nwls;i++){ // <---- loop over wls-strips ----
    gid=21001+i;
    coo[0]=xleft+i*step;
    dau=mother.add(new AMSgvolume(
        "CTC_WLS",0,"WLS ","BOX ",par,3,coo,nrm,"ONLY",1,gid));
  } // ----- end of wls-strips loop ----->
}
//--------------
//
//          <-- create two rows of vertical WLS's (geom=1) in mother
//
if(iflag==1){
  dx=0.5*wls;
  dy=0.5*CTCDBc::getagsize(1)+gp-gwls;
  dz=0.5*(CTCDBc::getagsize(2)/2.+2.*gp-2.*gwls);
  par[0]=dx;
  par[1]=dy;
  par[2]=dz;
  coo[1]=0.;
  cooz=zr+3.*dz+2.*wl+3.*gwls;// for top row
  step=4.*dxag+wl+4.*gp+wls;
  xleft=-0.5*CTCDBc::getagsize(0)+2.*dxag+gp+dx;
  for(i=0;i<nwls;i++){
    gid=21001+i;    // top row
    coo[0]=xleft+i*step;
    coo[2]=cooz;
    dau=mother.add(new AMSgvolume(
        "CTC_WLS",0,"WLS ","BOX ",par,3,coo,nrm,"ONLY",1,gid));
    gid=22001+i;    // bot row
    coo[2]=cooz-2.*dz-wl-2.*gwls;
    dau=mother.add(new AMSgvolume(
        "CTC_WLS",0,"WLS ","BOX ",par,3,coo,nrm,"ONLY",1,gid));
  }
}
//-----

}
void ctcgeomAG(AMSgvolume & mother){
  // A. Gougas version   
  // modified by V.Choutko 24/04/97
  // The AGLx, PMTx and PTFx (x=L,U,E) volumes are now at the level 2 ( from 0)
  //  and their geant id (== copy number) are :
  //
  // 1000000*layer+1000*i+100*j+kn*10+iz*4+iy*2+ix+1
  //
  // where
  //
  // layer = 1,2    layer number up(1) down(2)
  // i     = 1...4  supercell y index
  // j     = 1...5(6)  supercell x index
  // kn    = 1...3  ptf(1) agl(2) pmt(3)
  // iz    = 0..1   agl only
  // ix    = 0..1   x index inside supercell
  // iy    = 0..1   y index inside supercell
  //
  // Changes by A.K.Gougas 1/05/97
  // The Support structure is 60 mm thick, the PMT's of UPPER layer are
  // in the support structure, the UPPER aerogel layer is close to the TOF
  // (i.e. in both layers PMT's are "face-up")

   CTCDBc::setgeom(2);

  geant par[6]={0.,0.,0.,0.,0.,0.};
  geant pay[6]={0.,0.,0.,0.,0.,0.};
  geant parpmt[6]={0.,0.,0.,0.,0.,0.};
  geant parwal[6]={0.,0.,0.,0.,0.,0.};
  geant parptf[6]={0.,0.,0.,0.,0.,0.};
  number nrm[3][3]={1.,0.,0.,0.,1.,0.,0.,0.,1.};
  geant coo[3],coy[3];
  integer nmed,nptfe,i,j,k,gid;
  //
  number zr=CTCDBc::getsupzc(); //<---- Position of center of Honeycomb
  number ptfe=CTCDBc::getptfeth();//<-- PTFE single layer thickness
  //
  // <--- create supporting honeycomb plate in mother volume
  for(i=0;i<3;i++)par[i]=0.5*CTCDBc::getthcsize(i);
  coo[0]=coo[1]=0.;
  coo[2]=zr; // support structure's centre z-position
  mother.add(new AMSgvolume(
      "CTC_HONEYCOMB",0,"SUPP","BOX ",par,3,coo,nrm,"ONLY",0,3000000));
  //
  // <---- 
  //
  // Create two layers of ctc
  {
  integer ilay;
  AMSNode *pLayer;
  AMSNode *pCell;
  char cdum[2][5];
  strcpy(cdum[0],"UPPE");
  strcpy(cdum[1],"LOWE");
  for(ilay=1;ilay<3;ilay++){
    if(ilay==1){
     for(i=0;i<3;i++)par[i]=0.5*CTCDBc::getupsize(i);
     coo[0]=coo[1]=0.;
     coo[2]=zr+0.5*CTCDBc::getupsize(2)-0.5*CTCDBc::getthcsize(2);
     pLayer=mother.add(new AMSgvolume(
       "CTC_DUMMYMED",0,cdum[ilay-1],"BOX ",par,3,coo,nrm,"MANY",0,1000000));
    }
    else{
      for(i=0;i<3;i++)par[i]=0.5*CTCDBc::getupsize(i);
      coo[0]=coo[1]=5.5;
      coo[2]=zr-0.5*CTCDBc::getupsize(2)-0.5*CTCDBc::getthcsize(2);
      pLayer=mother.add(new AMSgvolume(
       "CTC_DUMMYMED",0,cdum[ilay-1],"BOX ",par,3,coo,nrm,"MANY",0,2000000));
    }
  //<--- Introduce the walls between modules

     for(i=0;i<3;i++)parwal[i] = 0.5*CTCDBc::getwallsize(i);
     coo[0] = 5.*0.5*CTCDBc::getcellsize(0)+5.*parwal[0];
     coo[1] = 0.;
     coo[2] = -0.5*CTCDBc::getupsize(2)+parwal[2];
     for(i=0;i<CTCDBc::getnx()+1;i++){ //<---- Separators (WALL) loop ----
       gid=1000000*ilay+10000*(i+1);
       char cvolw[]="WALU";
       cvolw[3]=cdum[ilay-1][0];
       pLayer->add(new AMSgvolume(
       "CTC_WALL",0,cvolw,"BOX ",parwal,3,coo,nrm,"ONLY",1,gid,1));
       coo[0]+=  -CTCDBc::getcellsize(0)-2.*parwal[0];
     } //<--- End of Separators (WALL) loop ----
     for(i=0;i<CTCDBc::getny();i++){
       for(j=0;j<CTCDBc::getnx();j++){
        for(k=0;k<3;k++)par[k]=0.5*CTCDBc::getcellsize(k);
        for(k=0;k<3;k++)pay[k]=0.5*CTCDBc::getygapsize(k);
        coo[0]=(CTCDBc::getnx()-1)*(par[0]+parwal[0]);
        coo[0]-=(par[0]+parwal[0])*2*j;
        coo[1]=(CTCDBc::getny()-1)*(par[1]+pay[1]);
        coo[1]-=(par[1]+pay[1])*2*i;
        coo[2]=0.;
        coy[0]=coo[0];
        coy[1]=coo[1]-par[1]-pay[1];
        coy[2]=coo[2];
        gid=1000000*ilay+1000*(i+1)+100*(j+1);
        char cvolc[]="CELU";
        cvolc[3]=cdum[ilay-1][0];
        pCell=pLayer->add(new AMSgvolume(
        "CTC_DUMMYMED",0,cvolc,"BOX ",par,3,coo,nrm,"MANY",1,gid,1));
        gid+=100000;
        char cvolg[]="YGAU";
        cvolg[3]=cdum[ilay-1][0];
        pLayer->add(new AMSgvolume(
        "CTC_DUMMYMED",0,cvolg,"BOX ",pay,3,coy,nrm,"ONLY",1,gid,1));
        //  Put 4 pmt,ptfe & agel 
        //<--- UPPER/ LOWER layer PTFE   up, PMT down         
         int ix,iy,iz;
          for(iy=0;iy<2;iy++){
           for (ix=0;ix<2;ix++){
            geant parptf[3],paragl[3],parpmt[3];
            geant cooptf[3],coopmt[3],cooagl[3];
            for(k=0;k<3;k++)parptf[k]=0.5*CTCDBc::getptfesize(k);
            cooptf[0]=coo[0]+parptf[0]*(2*ix-1);
            cooptf[1]=coo[1]+parptf[1]*(2*iy-1);
            cooptf[2]=coo[2]-(-0.5*CTCDBc::getcellsize(2)+
            parptf[2]);
            char cvolptf[]="PTFU";
            cvolptf[3]=cdum[ilay-1][0];
            gid=1000000*ilay+1000*(i+1)+100*(j+1)+10+ix+1+2*iy;
            pLayer->add(new AMSgvolume(
            "ATC_PTFE",0,cvolptf,"BOX ",parptf,3,cooptf,nrm,"MANY",1,gid,1));
            for(iz=0;iz<2;iz++){
             //put two agel blocks
             for(k=0;k<3;k++)paragl[k]=0.5*CTCDBc::getagelsize(k);
             paragl[2]=(paragl[2]-ptfe)/2;
             cooagl[0]=cooptf[0];
             cooagl[1]=cooptf[1];
             cooagl[2]=cooptf[2]+(2*iz-1)*(paragl[2]+ptfe/2);
             gid=1000000*ilay+1000*(i+1)+100*(j+1)+20+ix+1+2*iy+4*iz;
             char cvola[]="AGLU";
             cvola[3]=cdum[ilay-1][0];
             pLayer->add(new AMSgvolume(
             "ATC_AEROGEL",0,cvola,"BOX ",paragl,3,cooagl,nrm,"ONLY",1,gid,1));
            }
            for(k=0;k<3;k++)parpmt[k]=0.5*CTCDBc::getpmtsize(k);
            coopmt[0]=cooptf[0];
            coopmt[1]=cooptf[1];
            coopmt[2]=coo[2]-(0.5*CTCDBc::getcellsize(2)
            -parpmt[2]);  
            char cvolpmt[]="PMTU";
            cvolpmt[3]=cdum[ilay-1][0];
            gid=1000000*ilay+1000*(i+1)+100*(j+1)+30+ix+1+2*iy;
            pLayer->add(new AMSgvolume(
            "TOF_PMT_BOX",0,cvolpmt,"BOX ",parpmt,3,coopmt,nrm,"ONLY",1,gid,1));
           }
         }
       }
     }
  }

 }
   // Create an ETXRa half-sized module
  {
  integer ilay;
  AMSNode *pLayer;
  AMSNode *pCell;
  char cdum[2][5];
  strcpy(cdum[0],"UPPE");
  strcpy(cdum[1],"EXTR");
  for(ilay=2;ilay<3;ilay++){
    if(ilay==1){
     for(i=0;i<3;i++)par[i]=0.5*CTCDBc::getupsize(i);
     coo[0]=coo[1]=0.;
     coo[2]=zr+0.5*CTCDBc::getupsize(2)-0.5*CTCDBc::getthcsize(2);
     pLayer=mother.add(new AMSgvolume(
       "CTC_DUMMYMED",0,cdum[ilay-1],"BOX ",par,3,coo,nrm,"MANY",0,1000000));
    }
    else{
      par[0]= 0.25*CTCDBc::getcellsize(0)+0.5*CTCDBc::getwallsize(0);
      for(i=1;i<3;i++)par[i]=0.5*CTCDBc::getupsize(i);
      coo[0]=5.5-0.5*CTCDBc::getupsize(0)-0.25*CTCDBc::getcellsize(0)-
      CTCDBc::getwallsize(0);
      coo[1]=5.5;
      coo[2]=zr-0.5*CTCDBc::getupsize(2)-0.5*CTCDBc::getthcsize(2);
      pLayer=mother.add(new AMSgvolume(
       "CTC_DUMMYMED",0,cdum[ilay-1],"BOX ",par,3,coo,nrm,"MANY",0,2000000));
    }
  //<--- Introduce the walls between modules

     for(i=0;i<3;i++)parwal[i] = 0.5*CTCDBc::getwallsize(i);
     coo[0] = 0.25*CTCDBc::getcellsize(0)+parwal[0];
     coo[1] = 0.;
     coo[2] = -0.5*CTCDBc::getupsize(2)+parwal[2];
     for(i=CTCDBc::getnx()+1;i<CTCDBc::getnx()+2;i++){ //<---- Separators (WALL) loop ----
       gid=1000000*ilay+10000*(i+1);
       char cvolw[]="WALU";
       cvolw[3]=cdum[ilay-1][0];
       pLayer->add(new AMSgvolume(
       "CTC_WALL",0,cvolw,"BOX ",parwal,3,coo,nrm,"ONLY",1,gid,1));
       coo[0]+=  -CTCDBc::getcellsize(0)-2.*parwal[0];
     } //<--- End of Separators (WALL) loop ----
     for(i=0;i<CTCDBc::getny();i++){
       for(j=CTCDBc::getnx();j<CTCDBc::getnx()+1;j++){
        for(k=0;k<3;k++)par[k]=0.5*CTCDBc::getcellsize(k);
        par[0]=par[0]/2.;
        for(k=0;k<3;k++)pay[k]=0.5*CTCDBc::getygapsize(k);
        pay[0]=pay[0]/2.;
//        coo[0]=(CTCDBc::getnx()-1)*(par[0]+parwal[0]);
//        coo[0]-=(par[0]+parwal[0])*2*j;
//        coo[0]=(par[0]+parwal[0]);
//        coo[0]-=(par[0]+parwal[0])*2;
        coo[0]=0.;
        coo[1]=(CTCDBc::getny()-1)*(par[1]+pay[1]);
        coo[1]-=(par[1]+pay[1])*2*i;
        coo[2]=0.;
        coy[0]=coo[0];
        coy[1]=coo[1]-par[1]-pay[1];
        coy[2]=coo[2];
        gid=1000000*ilay+1000*(i+1)+100*(j+1);
        char cvolc[]="CELU";
        cvolc[3]=cdum[ilay-1][0];
        pCell=pLayer->add(new AMSgvolume(
        "CTC_DUMMYMED",0,cvolc,"BOX ",par,3,coo,nrm,"MANY",1,gid,1));
        gid+=100000;
        char cvolg[]="YGAU";
        cvolg[3]=cdum[ilay-1][0];
        pLayer->add(new AMSgvolume(
        "CTC_DUMMYMED",0,cvolg,"BOX ",pay,3,coy,nrm,"ONLY",1,gid,1));
        //  Put 4 pmt,ptfe & agel 
        //<--- UPPER/ LOWER layer PTFE   up, PMT down         
         int ix,iy,iz;
          for(iy=0;iy<2;iy++){
           for (ix=0;ix<1;ix++){
            geant parptf[3],paragl[3],parpmt[3];
            geant cooptf[3],coopmt[3],cooagl[3];
            for(k=0;k<3;k++)parptf[k]=0.5*CTCDBc::getptfesize(k);
//            cooptf[0]=coo[0]+parptf[0]*(2*ix-1);
            cooptf[0]=coo[0];
            cooptf[1]=coo[1]+parptf[1]*(2*iy-1);
            cooptf[2]=coo[2]-(-0.5*CTCDBc::getcellsize(2)+
            parptf[2]);
            char cvolptf[]="PTFU";
            cvolptf[3]=cdum[ilay-1][0];
            gid=1000000*ilay+1000*(i+1)+100*(j+1)+10+ix+1+2*iy;
            pLayer->add(new AMSgvolume(
            "ATC_PTFE",0,cvolptf,"BOX ",parptf,3,cooptf,nrm,"MANY",1,gid,1));
            for(iz=0;iz<2;iz++){
             //put two agel blocks
             for(k=0;k<3;k++)paragl[k]=0.5*CTCDBc::getagelsize(k);
             paragl[2]=(paragl[2]-ptfe)/2;
             cooagl[0]=cooptf[0];
             cooagl[1]=cooptf[1];
             cooagl[2]=cooptf[2]+(2*iz-1)*(paragl[2]+ptfe/2);
             gid=1000000*ilay+1000*(i+1)+100*(j+1)+20+ix+1+2*iy+4*iz;
             char cvola[]="AGLU";
             cvola[3]=cdum[ilay-1][0];
             pLayer->add(new AMSgvolume(
             "ATC_AEROGEL",0,cvola,"BOX ",paragl,3,cooagl,nrm,"ONLY",1,gid,1));
            }
            for(k=0;k<3;k++)parpmt[k]=0.5*CTCDBc::getpmtsize(k);
            coopmt[0]=cooptf[0];
            coopmt[1]=cooptf[1];
            coopmt[2]=coo[2]-(0.5*CTCDBc::getcellsize(2)
            -parpmt[2]);  
            char cvolpmt[]="PMTU";
            cvolpmt[3]=cdum[ilay-1][0];
            gid=1000000*ilay+1000*(i+1)+100*(j+1)+30+ix+1+2*iy;
            pLayer->add(new AMSgvolume(
            "TOF_PMT_BOX",0,cvolpmt,"BOX ",parpmt,3,coopmt,nrm,"ONLY",1,gid,1));
           }
         }
       }
     }
  }

 }
}

void ctcgeomAGPlus(AMSgvolume & mother){
  // 3 rad leangth ( 1.05 cm) of Tungsten added between 1 & 2 layers
  // A. Gougas version   
  // modified by V.Choutko 24/04/97
  // The AGLx, PMTx and PTFx (x=L,U,E) volumes are now at the level 2 ( from 0)
  //  and their geant id (== copy number) are :
  //
  // 1000000*layer+1000*i+100*j+kn*10+iz*4+iy*2+ix+1
  //
  // where
  //
  // layer = 1,2    layer number up(1) down(2)
  // i     = 1...4  supercell y index
  // j     = 1...5(6)  supercell x index
  // kn    = 1...3  ptf(1) agl(2) pmt(3)
  // iz    = 0..1   agl only
  // ix    = 0..1   x index inside supercell
  // iy    = 0..1   y index inside supercell
  //
  // Changes by A.K.Gougas 1/05/97
  // The Support structure is 60 mm thick, the PMT's of UPPER layer are
  // in the support structure, the UPPER aerogel layer is close to the TOF
  // (i.e. in both layers PMT's are "face-up")

   CTCDBc::setgeom(2);

  geant par[6]={0.,0.,0.,0.,0.,0.};
  geant pay[6]={0.,0.,0.,0.,0.,0.};
  geant parpmt[6]={0.,0.,0.,0.,0.,0.};
  geant parwal[6]={0.,0.,0.,0.,0.,0.};
  geant parptf[6]={0.,0.,0.,0.,0.,0.};
  number nrm[3][3]={1.,0.,0.,0.,1.,0.,0.,0.,1.};
  geant coo[3],coy[3];
  integer nmed,nptfe,i,j,k,gid;
  //
  number zr=CTCDBc::getsupzc(); //<---- Position of center of Honeycomb
  number ptfe=CTCDBc::getptfeth();//<-- PTFE single layer thickness
  //
  // <--- create supporting honeycomb plate in mother volume
  for(i=0;i<3;i++)par[i]=0.5*CTCDBc::getthcsize(i);
  coo[0]=coo[1]=0.;
  coo[2]=zr; // support structure's centre z-position
  mother.add(new AMSgvolume(
      "CTC_HONEYCOMB",0,"SUPP","BOX ",par,3,coo,nrm,"ONLY",0,3000000));
  //
  // <---- 
  //
  // Create two layers of ctc
  {
  integer ilay;
  AMSNode *pLayer;
  AMSNode *pCell;
  char cdum[2][5];
  strcpy(cdum[0],"UPPE");
  strcpy(cdum[1],"LOWE");
  // Add Tungsten first
      par[0]=12;
      par[1]=45;
      par[2]=1.05/2;
      coo[0]=0;
      coo[1]=5.5;
      coo[2]=zr-0.5*CTCDBc::getthcsize(2)-0.55;   // for W
      pLayer=mother.add(new AMSgvolume(
       "TUNGSTEN",0,"AMP1","BOX ",par,3,coo,nrm,"ONLY",0,1000001));
  

  for(ilay=1;ilay<3;ilay++){
    if(ilay==1){
     for(i=0;i<3;i++)par[i]=0.5*CTCDBc::getupsize(i);
     coo[0]=coo[1]=0.;
     coo[2]=zr+0.5*CTCDBc::getupsize(2)-0.5*CTCDBc::getthcsize(2);
     pLayer=mother.add(new AMSgvolume(
       "CTC_DUMMYMED",0,cdum[ilay-1],"BOX ",par,3,coo,nrm,"MANY",0,1000000));
    }
    else{
      for(i=0;i<3;i++)par[i]=0.5*CTCDBc::getupsize(i);
      coo[0]=coo[1]=5.5;
      coo[2]=zr-0.5*CTCDBc::getupsize(2)-0.5*CTCDBc::getthcsize(2)-1.1;   // for W
      pLayer=mother.add(new AMSgvolume(
       "CTC_DUMMYMED",0,cdum[ilay-1],"BOX ",par,3,coo,nrm,"MANY",0,2000000));
    }
    
  //<--- Introduce the walls between modules

     for(i=0;i<3;i++)parwal[i] = 0.5*CTCDBc::getwallsize(i);
     coo[0] = 5.*0.5*CTCDBc::getcellsize(0)+5.*parwal[0];
     coo[1] = 0.;
     coo[2] = -0.5*CTCDBc::getupsize(2)+parwal[2];
     for(i=0;i<CTCDBc::getnx()+1;i++){ //<---- Separators (WALL) loop ----
       gid=1000000*ilay+10000*(i+1);
       char cvolw[]="WALU";
       cvolw[3]=cdum[ilay-1][0];
       pLayer->add(new AMSgvolume(
       "CTC_WALL",0,cvolw,"BOX ",parwal,3,coo,nrm,"ONLY",1,gid,1));
       coo[0]+=  -CTCDBc::getcellsize(0)-2.*parwal[0];
     } //<--- End of Separators (WALL) loop ----
     for(i=0;i<CTCDBc::getny();i++){
       for(j=0;j<CTCDBc::getnx();j++){
        for(k=0;k<3;k++)par[k]=0.5*CTCDBc::getcellsize(k);
        for(k=0;k<3;k++)pay[k]=0.5*CTCDBc::getygapsize(k);
        coo[0]=(CTCDBc::getnx()-1)*(par[0]+parwal[0]);
        coo[0]-=(par[0]+parwal[0])*2*j;
        coo[1]=(CTCDBc::getny()-1)*(par[1]+pay[1]);
        coo[1]-=(par[1]+pay[1])*2*i;
        coo[2]=0.;
        coy[0]=coo[0];
        coy[1]=coo[1]-par[1]-pay[1];
        coy[2]=coo[2];
        gid=1000000*ilay+1000*(i+1)+100*(j+1);
        char cvolc[]="CELU";
        cvolc[3]=cdum[ilay-1][0];
        pCell=pLayer->add(new AMSgvolume(
        "CTC_DUMMYMED",0,cvolc,"BOX ",par,3,coo,nrm,"MANY",1,gid,1));
        gid+=100000;
        char cvolg[]="YGAU";
        cvolg[3]=cdum[ilay-1][0];
        pLayer->add(new AMSgvolume(
        "CTC_DUMMYMED",0,cvolg,"BOX ",pay,3,coy,nrm,"ONLY",1,gid,1));
        //  Put 4 pmt,ptfe & agel 
        //<--- UPPER/ LOWER layer PTFE   up, PMT down         
         int ix,iy,iz;
          for(iy=0;iy<2;iy++){
           for (ix=0;ix<2;ix++){
            geant parptf[3],paragl[3],parpmt[3];
            geant cooptf[3],coopmt[3],cooagl[3];
            for(k=0;k<3;k++)parptf[k]=0.5*CTCDBc::getptfesize(k);
            cooptf[0]=coo[0]+parptf[0]*(2*ix-1);
            cooptf[1]=coo[1]+parptf[1]*(2*iy-1);
            cooptf[2]=coo[2]-(-0.5*CTCDBc::getcellsize(2)+
            parptf[2]);
            char cvolptf[]="PTFU";
            cvolptf[3]=cdum[ilay-1][0];
            gid=1000000*ilay+1000*(i+1)+100*(j+1)+10+ix+1+2*iy;
            pLayer->add(new AMSgvolume(
            "ATC_PTFE",0,cvolptf,"BOX ",parptf,3,cooptf,nrm,"MANY",1,gid,1));
            for(iz=0;iz<2;iz++){
             //put two agel blocks
             for(k=0;k<3;k++)paragl[k]=0.5*CTCDBc::getagelsize(k);
             paragl[2]=(paragl[2]-ptfe)/2;
             cooagl[0]=cooptf[0];
             cooagl[1]=cooptf[1];
             cooagl[2]=cooptf[2]+(2*iz-1)*(paragl[2]+ptfe/2);
             gid=1000000*ilay+1000*(i+1)+100*(j+1)+20+ix+1+2*iy+4*iz;
             char cvola[]="AGLU";
             cvola[3]=cdum[ilay-1][0];
             pLayer->add(new AMSgvolume(
             "ATC_AEROGEL",0,cvola,"BOX ",paragl,3,cooagl,nrm,"ONLY",1,gid,1));
            }
            for(k=0;k<3;k++)parpmt[k]=0.5*CTCDBc::getpmtsize(k);
            coopmt[0]=cooptf[0];
            coopmt[1]=cooptf[1];
            coopmt[2]=coo[2]-(0.5*CTCDBc::getcellsize(2)
            -parpmt[2]);  
            char cvolpmt[]="PMTU";
            cvolpmt[3]=cdum[ilay-1][0];
            gid=1000000*ilay+1000*(i+1)+100*(j+1)+30+ix+1+2*iy;
            pLayer->add(new AMSgvolume(
            "TOF_PMT_BOX",0,cvolpmt,"BOX ",parpmt,3,coopmt,nrm,"ONLY",1,gid,1));
           }
         }
       }
     }
  }

 }
   // Create an ETXRa half-sized module
  {
  integer ilay;
  AMSNode *pLayer;
  AMSNode *pCell;
  char cdum[2][5];
  strcpy(cdum[0],"UPPE");
  strcpy(cdum[1],"EXTR");
  for(ilay=2;ilay<3;ilay++){
    if(ilay==1){
     for(i=0;i<3;i++)par[i]=0.5*CTCDBc::getupsize(i);
     coo[0]=coo[1]=0.;
     coo[2]=zr+0.5*CTCDBc::getupsize(2)-0.5*CTCDBc::getthcsize(2);
     pLayer=mother.add(new AMSgvolume(
       "CTC_DUMMYMED",0,cdum[ilay-1],"BOX ",par,3,coo,nrm,"MANY",0,1000000));
    }
    else{
      par[0]= 0.25*CTCDBc::getcellsize(0)+0.5*CTCDBc::getwallsize(0);
      for(i=1;i<3;i++)par[i]=0.5*CTCDBc::getupsize(i);
      coo[0]=5.5-0.5*CTCDBc::getupsize(0)-0.25*CTCDBc::getcellsize(0)-
      CTCDBc::getwallsize(0);
      coo[1]=5.5;
      coo[2]=zr-0.5*CTCDBc::getupsize(2)-0.5*CTCDBc::getthcsize(2)-1.1;
      pLayer=mother.add(new AMSgvolume(
       "CTC_DUMMYMED",0,cdum[ilay-1],"BOX ",par,3,coo,nrm,"MANY",0,2000000));
    }
  //<--- Introduce the walls between modules

     for(i=0;i<3;i++)parwal[i] = 0.5*CTCDBc::getwallsize(i);
     coo[0] = 0.25*CTCDBc::getcellsize(0)+parwal[0];
     coo[1] = 0.;
     coo[2] = -0.5*CTCDBc::getupsize(2)+parwal[2];
     for(i=CTCDBc::getnx()+1;i<CTCDBc::getnx()+2;i++){ //<---- Separators (WALL) loop ----
       gid=1000000*ilay+10000*(i+1);
       char cvolw[]="WALU";
       cvolw[3]=cdum[ilay-1][0];
       pLayer->add(new AMSgvolume(
       "CTC_WALL",0,cvolw,"BOX ",parwal,3,coo,nrm,"ONLY",1,gid,1));
       coo[0]+=  -CTCDBc::getcellsize(0)-2.*parwal[0];
     } //<--- End of Separators (WALL) loop ----
     for(i=0;i<CTCDBc::getny();i++){
       for(j=CTCDBc::getnx();j<CTCDBc::getnx()+1;j++){
        for(k=0;k<3;k++)par[k]=0.5*CTCDBc::getcellsize(k);
        par[0]=par[0]/2.;
        for(k=0;k<3;k++)pay[k]=0.5*CTCDBc::getygapsize(k);
        pay[0]=pay[0]/2.;
//        coo[0]=(CTCDBc::getnx()-1)*(par[0]+parwal[0]);
//        coo[0]-=(par[0]+parwal[0])*2*j;
//        coo[0]=(par[0]+parwal[0]);
//        coo[0]-=(par[0]+parwal[0])*2;
        coo[0]=0.;
        coo[1]=(CTCDBc::getny()-1)*(par[1]+pay[1]);
        coo[1]-=(par[1]+pay[1])*2*i;
        coo[2]=0.;
        coy[0]=coo[0];
        coy[1]=coo[1]-par[1]-pay[1];
        coy[2]=coo[2];
        gid=1000000*ilay+1000*(i+1)+100*(j+1);
        char cvolc[]="CELU";
        cvolc[3]=cdum[ilay-1][0];
        pCell=pLayer->add(new AMSgvolume(
        "CTC_DUMMYMED",0,cvolc,"BOX ",par,3,coo,nrm,"MANY",1,gid,1));
        gid+=100000;
        char cvolg[]="YGAU";
        cvolg[3]=cdum[ilay-1][0];
        pLayer->add(new AMSgvolume(
        "CTC_DUMMYMED",0,cvolg,"BOX ",pay,3,coy,nrm,"ONLY",1,gid,1));
        //  Put 4 pmt,ptfe & agel 
        //<--- UPPER/ LOWER layer PTFE   up, PMT down         
         int ix,iy,iz;
          for(iy=0;iy<2;iy++){
           for (ix=0;ix<1;ix++){
            geant parptf[3],paragl[3],parpmt[3];
            geant cooptf[3],coopmt[3],cooagl[3];
            for(k=0;k<3;k++)parptf[k]=0.5*CTCDBc::getptfesize(k);
//            cooptf[0]=coo[0]+parptf[0]*(2*ix-1);
            cooptf[0]=coo[0];
            cooptf[1]=coo[1]+parptf[1]*(2*iy-1);
            cooptf[2]=coo[2]-(-0.5*CTCDBc::getcellsize(2)+
            parptf[2]);
            char cvolptf[]="PTFU";
            cvolptf[3]=cdum[ilay-1][0];
            gid=1000000*ilay+1000*(i+1)+100*(j+1)+10+ix+1+2*iy;
            pLayer->add(new AMSgvolume(
            "ATC_PTFE",0,cvolptf,"BOX ",parptf,3,cooptf,nrm,"MANY",1,gid,1));
            for(iz=0;iz<2;iz++){
             //put two agel blocks
             for(k=0;k<3;k++)paragl[k]=0.5*CTCDBc::getagelsize(k);
             paragl[2]=(paragl[2]-ptfe)/2;
             cooagl[0]=cooptf[0];
             cooagl[1]=cooptf[1];
             cooagl[2]=cooptf[2]+(2*iz-1)*(paragl[2]+ptfe/2);
             gid=1000000*ilay+1000*(i+1)+100*(j+1)+20+ix+1+2*iy+4*iz;
             char cvola[]="AGLU";
             cvola[3]=cdum[ilay-1][0];
             pLayer->add(new AMSgvolume(
             "ATC_AEROGEL",0,cvola,"BOX ",paragl,3,cooagl,nrm,"ONLY",1,gid,1));
            }
            for(k=0;k<3;k++)parpmt[k]=0.5*CTCDBc::getpmtsize(k);
            coopmt[0]=cooptf[0];
            coopmt[1]=cooptf[1];
            coopmt[2]=coo[2]-(0.5*CTCDBc::getcellsize(2)
            -parpmt[2]);  
            char cvolpmt[]="PMTU";
            cvolpmt[3]=cdum[ilay-1][0];
            gid=1000000*ilay+1000*(i+1)+100*(j+1)+30+ix+1+2*iy;
            pLayer->add(new AMSgvolume(
            "TOF_PMT_BOX",0,cvolpmt,"BOX ",parpmt,3,coopmt,nrm,"ONLY",1,gid,1));
           }
         }
       }
     }
  }

 }
}


void tkgeom(AMSgvolume &mother){
  TKDBc::read();



cout<<" TKGeom-I-Planes option selected"<<endl;


AMSID amsid;
geant xx[3]={0.9,0.2,0.2};
geant  xnrm[3][3];
geant par[6]={0.,0.,0.,0.,0.,0.};
number nrm[3][3]={1.,0.,0.,0.,1.,0.,0.,0.,1.};
number inrm[3][3];
char name[5];
geant coo[3]={0.,0.,0.};
integer gid=0;
integer nrot=500; // Temporary arbitary assignment
//static AMSgvolume mother(0,0,AMSDBc::ams_name,"BOX",par,3,coo,nrm,"ONLY",
//                         0,gid);  // temporary a dummy volume
AMSNode * cur;
AMSNode * dau;
int i;
for ( i=0;i<AMSDBc::nlay();i++){
ostrstream ost(name,sizeof(name));
 ost << "STK"<<i+1<<ends;
 coo[0]=0;
 coo[1]=0;
 coo[2]=AMSDBc::zposl(i);
    int ii;
    for ( ii=0;ii<5;ii++)par[ii]=AMSDBc::layd(i,ii);
      gid=i+1;
      dau=mother.add(new AMSgvolume(
      "VACUUM",++nrot,name,"CONE",par,5,coo,nrm, "MANY",0,gid));
    int j;
    for (j=0;j<AMSDBc::nlad(i+1);j++){
    nrot++;  
    int k;
     for ( k=0;k<AMSDBc::nsen(i+1,j+1);k++){
       ost.seekp(0);
       ost << "SEN" << i+1<<ends;
       par[0]=AMSDBc::ssize_inactive(i,0)/2;
       par[1]=AMSDBc::ssize_inactive(i,1)/2;
       par[2]=AMSDBc::silicon_z(i)/2;
       coo[0]=-AMSDBc::ssize_inactive(i,0)/2.+(2*k+2-AMSDBc::nsen(i+1,j+1))*AMSDBc::ssize_inactive(i,0)/2.;
       //    Temporary
       //       if(k < AMSDBc::nhalf(i+1,j+1))coo[0]=coo[0]-AMSDBc::halfldist(i)/2;
       //       else coo[0]=coo[0]+AMSDBc::halfldist(i)/2;
       coo[1]=(j+1)*AMSDBc::c2c(i)-(AMSDBc::nlad(i+1)+1)*AMSDBc::c2c(i)/2.;
       coo[2]=AMSDBc::zpos(i);
       VZERO(nrm,9*sizeof(nrm[0][0])/4);
       nrm[0][0]=1;
       nrm[1][1]=1;
       nrm[2][2]=1;
#ifdef __AMSDEBUG__
       if(i==5 && AMSgvolume::debug){
        MTX(xnrm,xx);
        int a1,a2;
        for(a1=0;a1<3;a1++)for(a2=0;a2<3;a2++)nrm[a2][a1]=xnrm[a1][a2];
       }
#endif
      //
      //  Sensors notactive
      //
       gid=i+1+10*(j+1)+1000*(k+1)+100000;
       cur=dau->add(new AMSgvolume(
       "NONACTIVE_SILICON",nrot,name,"BOX",par,3,coo,nrm,"MANY",1,gid));


     }
    


    for ( k=0;k<AMSDBc::nsen(i+1,j+1);k++){
      ost.seekp(0);
      ost << "SEA" << i+1<<ends;
      par[0]=AMSDBc::ssize_active(i,0)/2;
      par[1]=AMSDBc::ssize_active(i,1)/2;
      par[2]=AMSDBc::silicon_z(i)/2;
      coo[0]=-AMSDBc::ssize_inactive(i,0)/2.+(2*k+2-AMSDBc::nsen(i+1,j+1))*AMSDBc::ssize_inactive(i,0)/2.;
       if(k < AMSDBc::nhalf(i+1,j+1))coo[0]=coo[0]-AMSDBc::halfldist(i)/2;
       else coo[0]=coo[0]+AMSDBc::halfldist(i)/2;
      coo[1]=(j+1)*AMSDBc::c2c(i)-(AMSDBc::nlad(i+1)+1)*AMSDBc::c2c(i)/2.;
      coo[2]=AMSDBc::zpos(i);
      VZERO(nrm,9*sizeof(nrm[0][0])/4);
      nrm[0][0]=1;
      nrm[1][1]=1;
      nrm[2][2]=1;
#ifdef __AMSDEBUG__
      if(i==5 && AMSgvolume::debug){
       MTX(xnrm,xx);
       int a1,a2;
       for( a1=0;a1<3;a1++)for( a2=0;a2<3;a2++)nrm[a2][a1]=xnrm[a1][a2];
      }
#endif
      //
      //  Sensors active
      //
      gid=i+1+10*(j+1)+1000*(k+1);
      int s = k>=AMSDBc::nhalf(i+1,j+1);
      if(strstr(AMSJob::gethead()->getsetup(),"AMSSTATION") ||
       AMSDBc::activeladdshuttle(i+1,j+1,s)){  
       integer status=1;
       if(TKDBc::update())TKDBc::SetSensor(i,j,k,status,coo,nrm);
       else               TKDBc::GetSensor(i,j,k,status,coo,nrm);

       if(status)cur=dau->add(new AMSgvolume(
      "ACTIVE_SILICON",nrot,name,"BOX",par,3,coo,nrm,"ONLY",1,gid));
      }
    }
    }
  for ( j=0;j<AMSDBc::nlad(i+1);j++){
    nrot++;  
      ost.seekp(0);
      ost << "PLA" << i+1<<ends;
      par[0]=AMSDBc::ssize_inactive(i,0)*AMSDBc::nsen(i+1,j+1)/2.;
      par[1]=AMSDBc::c2c(i)/2;
      par[2]=AMSDBc::support_w(i);     // not /2 ! (600 mkm)
      coo[0]=0;
      coo[1]=(j+1)*AMSDBc::c2c(i)-(AMSDBc::nlad(i+1)+1)*AMSDBc::c2c(i)/2.;
      coo[2]=AMSDBc::zpos(i)-AMSDBc::silicon_z(i)/2.-AMSDBc::support_w(i);
      VZERO(nrm,9*sizeof(nrm[0][0])/4);
      nrm[0][0]=1;
      nrm[1][1]=1;
      nrm[2][2]=1;
      //
      //  Planes
      //
      gid=i+1+10*(j+1);
      cur=dau->add(new AMSgvolume(
      "CARBON",nrot,name,"BOX",par,3,coo,nrm,"ONLY",1,gid));

  }
  for ( j=0;j<AMSDBc::nlad(i+1);j++){
    nrot++;  
      ost.seekp(0);
      ost << "ELL" << i+1<<ends;
      par[0]=AMSDBc::zelec(i,1)/2.;
      par[1]=AMSDBc::c2c(i)/2.;
      par[2]=AMSDBc::zelec(i,0)/2;
      coo[0]=-AMSDBc::nsen(i+1,j+1)*AMSDBc::ssize_inactive(i,0)/2.-par[0];
      coo[1]=(j+1)*AMSDBc::c2c(i)-(AMSDBc::nlad(i+1)+1)*AMSDBc::c2c(i)/2.;
      coo[2]=AMSDBc::zpos(i);
      VZERO(nrm,9*sizeof(nrm[0][0])/4);
      nrm[0][0]=1;
      nrm[1][1]=1;
      nrm[2][2]=1;
#ifdef __AMSDEBUG__
      if(i==5 && AMSgvolume::debug){
       MTX(xnrm,xx);
       int a1,a2;
       for( a1=0;a1<3;a1++)for( a2=0;a2<3;a2++)nrm[a2][a1]=xnrm[a1][a2];
      }
#endif
      //
      //  Elec L
      //
      gid=i+1+10*(j+1);
      cur=dau->add(new AMSgvolume(
      "ELECTRONICS",nrot,name,"BOX",par,3,coo,nrm,"ONLY",1,gid));

  }
  for ( j=0;j<AMSDBc::nlad(i+1);j++){
    nrot++;  
      ost.seekp(0);
      ost << "ELR" << i+1<<ends;
      par[0]=AMSDBc::zelec(i,1)/2;
      par[1]=AMSDBc::c2c(i)/2;
      par[2]=AMSDBc::zelec(i,0)/2;
      coo[0]=+AMSDBc::nsen(i+1,j+1)*AMSDBc::ssize_inactive(i,0)/2.+par[0];
      coo[1]=(j+1)*AMSDBc::c2c(i)-(AMSDBc::nlad(i+1)+1)*AMSDBc::c2c(i)/2.;
      coo[2]=AMSDBc::zpos(i);
      VZERO(nrm,9*sizeof(nrm[0][0])/4);
      nrm[0][0]=1;
      nrm[1][1]=1;
      nrm[2][2]=1;
#ifdef __AMSDEBUG__
      if(i==5 && AMSgvolume::debug){
       MTX(xnrm,xx);
       int a1,a2;
       for( a1=0;a1<3;a1++)for( a2=0;a2<3;a2++)nrm[a2][a1]=xnrm[a1][a2];
      }
#endif
      //
      //  Elec R
      //
      gid=i+1+10*(j+1);
      cur=dau->add(new AMSgvolume(
       "ELECTRONICS",nrot,name,"BOX",par,3,coo,nrm,"ONLY",1,gid));

  }

}
}















void pshgeom(AMSgvolume & mother){
AMSID amsid;
geant par[6]={0.,0.,0.,0.,0.,0.};
number nrm[3][3]={1.,0.,0.,0.,1.,0.,0.,0.,1.};
number inrm[3][3];
integer nrot=400;
char name[5];
geant coo[3]={0.,0.,0.};
integer gid=0;
AMSNode * cur;
AMSNode * dau;
AMSgtmed *p;
      gid=1;
      par[0]=0.;
      par[1]=82.8;
      par[2]=0.75;
      coo[2]=75.25;
      dau=mother.add(new AMSgvolume(
      "TOF_SCINT",0,"PSH1","TUBE",par,3,coo,nrm, "ONLY",0,gid));
      gid++;

      par[0]=18.;
      par[1]=82.8;
      par[2]=83.8;
      par[3]=82.8;
      par[4]=83.8;
      coo[2]=58.;



      dau=mother.add(new AMSgvolume(
      "TOF_SCINT",0,"PSH2","CONE",par,5,coo,nrm, "ONLY",0,gid++));

      par[0]=18.;
      par[1]=82.8;
      par[2]=83.4;
      par[3]=82.8;
      par[4]=83.4;
      coo[2]=-58.;



      dau=mother.add(new AMSgvolume(
      "TOF_SCINT",0,"PSH3","CONE",par,5,coo,nrm, "ONLY",0,gid++));





      gid=1;
      par[0]=187.;
      par[1]=189.;
      par[2]=190.;
      par[3]=90.;
      par[4]=270.;
      coo[2]=0;
      nrm[0][0]=0;
      nrm[1][0]=0;
      nrm[2][0]=1;
      nrm[0][1]=0;
      nrm[1][1]=1;
      nrm[2][1]=0;
      nrm[0][2]=1;
      nrm[1][2]=0;
      nrm[2][2]=0;
      dau=mother.add(new AMSgvolume(
      "1/2ALUM",nrot++,"SHU1","TUBS",par,5,coo,nrm, "ONLY",0,gid++));

      par[0]=0;
      par[1]=187.;
      par[2]=1.;
      par[3]=90.;
      par[4]=270.;
      coo[0]=-189;
      coo[1]=0;
      coo[2]=0;
      dau=mother.add(new AMSgvolume(
      "1/2ALUM",nrot++,"SHU2","TUBS",par,5,coo,nrm, "ONLY",0,gid++));
      par[0]=0.;
      par[1]=187.;
      par[2]=1.;
      par[3]=0.;
      par[4]=360.;
      coo[0]=189;
      coo[1]=0;
      coo[2]=0;
      dau=mother.add(new AMSgvolume(
      "1/2ALUM",nrot++,"SHU3","TUBS",par,5,coo,nrm, "ONLY",0,gid++));


      // Iron support 12 + 10 bins
      nrm[0][0]=1;
      nrm[1][0]=0;
      nrm[2][0]=0;
      nrm[0][1]=0;
      nrm[1][1]=1;
      nrm[2][1]=0;
      nrm[0][2]=0;
      nrm[1][2]=0;
      nrm[2][2]=1;
      gid=1;

      par[0]=84;
      par[1]=3;
      par[2]=4;

      coo[0]=0;      
      coo[1]=-87.;      
      coo[2]=-38;      
      dau=mother.add(new AMSgvolume(
      "IRON",0,"IR01","BOX ",par,3,coo,nrm, "ONLY",0,gid++));

      coo[0]=0;      
      coo[1]=-87.;      
      coo[2]=38;      
      dau=mother.add(new AMSgvolume(
      "IRON",0,"IR02","BOX ",par,3,coo,nrm, "ONLY",0,gid++));

      coo[0]=0;      
      coo[1]=87.;      
      coo[2]=-38;      
      dau=mother.add(new AMSgvolume(
      "IRON",0,"IR03","BOX ",par,3,coo,nrm, "ONLY",0,gid++));

      coo[0]=0;      
      coo[1]=87.;      
      coo[2]=38;      
      dau=mother.add(new AMSgvolume(
      "IRON",0,"IR04","BOX ",par,3,coo,nrm, "ONLY",0,gid++));

      par[0]=3;
      par[1]=84;
      par[2]=4;



      coo[0]=-87;      
      coo[1]=0;      
      coo[2]=-38;      
      dau=mother.add(new AMSgvolume(
      "IRON",0,"IR05","BOX ",par,3,coo,nrm, "ONLY",0,gid++));

      coo[0]=-87;      
      coo[1]=0.;      
      coo[2]=38;      
      dau=mother.add(new AMSgvolume(
      "IRON",0,"IR06","BOX ",par,3,coo,nrm, "ONLY",0,gid++));

      coo[0]=87;      
      coo[1]=0.;      
      coo[2]=-38;      
      dau=mother.add(new AMSgvolume(
      "IRON",0,"IR07","BOX ",par,3,coo,nrm, "ONLY",0,gid++));

      coo[0]=87;      
      coo[1]=0;      
      coo[2]=38;      
      dau=mother.add(new AMSgvolume(
      "IRON",0,"IR08","BOX ",par,3,coo,nrm, "ONLY",0,gid++));

      par[0]=4;
      par[1]=3;
      par[2]=38;



      coo[0]=-87;      
      coo[1]=-87.;      
      coo[2]=0;      
      dau=mother.add(new AMSgvolume(
      "IRON",0,"IR09","BOX ",par,3,coo,nrm, "ONLY",0,gid++));

      coo[0]=-87;      
      coo[1]=87.;      
      coo[2]=0;      
      dau=mother.add(new AMSgvolume(
      "IRON",0,"IR10","BOX ",par,3,coo,nrm, "ONLY",0,gid++));

      coo[0]=87;      
      coo[1]=-87.;      
      coo[2]=0;      
      dau=mother.add(new AMSgvolume(
      "IRON",0,"IR11","BOX ",par,3,coo,nrm, "ONLY",0,gid++));

      coo[0]=87;      
      coo[1]=87.;      
      coo[2]=0;      
      dau=mother.add(new AMSgvolume(
      "IRON",0,"IR12","BOX ",par,3,coo,nrm, "ONLY",0,gid++));


       
}
