// Author V. Choutko 24-may-1996
// TOF Geometry E. Choumilov 22-jul-1996 
// ANTI Geometry E. Choumilov 2-06-1997 
// ECAL 1.0-version Geometry E. Choumilov 15-09-1999 
// Passive Shield Geometry V. Choutko 22-jul-1996
// CTC (Cherenkov Thresh. Counter) geometry E.Choumilov 26-sep-1996
// ATC (Aerogel Threshold Cerenkov) geometry A.K.Gougas 14-Mar-1997
// 
//
#include <typedefs.h>
#include <node.h>
#include <snode.h>
#include <amsdbc.h>
#include <tofdbc.h>
#include <ctcdbc.h>
#include <antidbc.h>
#include <ecaldbc.h>
#include <gmat.h>
#include <extC.h>
#include <stdlib.h>
#include <gvolume.h>
#include <amsgobj.h>
#include <job.h>
#include <commons.h>
#include <tkdbc.h>
#include <richdbc.h>
extern "C" void mtx_(geant nrm[][3],geant vect[]);
extern "C" void mtx2_(number nrm[][3],geant  xnrm[][3]);
#define MTX mtx_
#define MTX2 mtx2_
void AMSgvolume::amsgeom(){
extern void tkgeom(AMSgvolume &);
extern void tkgeom02(AMSgvolume &);
extern void magnetgeom(AMSgvolume &);
extern void magnetgeom02(AMSgvolume &);
extern void magnetgeom02Test(AMSgvolume &);
extern void tofgeom(AMSgvolume &);
extern void tofgeom02(AMSgvolume &);
extern void antigeom(AMSgvolume &);
extern void antigeom02(AMSgvolume &);
extern void pshgeom(AMSgvolume &);
extern void pshgeom02(AMSgvolume &);
extern void ctcgeom(AMSgvolume &);
extern void richgeom02(AMSgvolume &);
extern void ecalgeom02(AMSgvolume &);
extern void trdgeom02(AMSgvolume &);
extern void srdgeom02(AMSgvolume &);
#ifdef __G4AMS__
extern void testboolgeom(AMSgvolume &);
#endif
AMSID amsid;
geant par[3];
geant parf[3];
geant coo[3]={0,0,0};
number nrm[3][3]={1.,0.,0.,0.,1.,0.,0.,0.,1.};
integer gid=1;
      parf[0]=AMSDBc::ams_size[0]/2+0.2;   // just trying to find geant4 bugs
      parf[1]=AMSDBc::ams_size[1]/2+0.2;
      parf[2]=AMSDBc::ams_size[2]/2+0.2;
      par[0]=AMSDBc::ams_size[0]/2;
      par[1]=AMSDBc::ams_size[1]/2;
      par[2]=AMSDBc::ams_size[2]/2;
      // to be able rotate ams as a whole
static AMSgvolume false_mother("VACUUM",0,AMSDBc::ams_name,"BOX ",parf,3,coo,nrm,"ONLY",  0,gid,1);  // AMS false mother volume
static AMSgvolume mother("VACUUM",AMSDBc::ams_rotmno,"FMOT","BOX",par,3,AMSDBc::ams_coo,AMSDBc::ams_nrm,"ONLY",  0,gid);  // AMS mother volume
AMSJob::gethead()->addup( &false_mother);
false_mother.add(&mother);
if(strstr(AMSJob::gethead()->getsetup(),"AMSSHUTTLE")){
 magnetgeom(mother);
 tofgeom(mother);
 antigeom(mother);
 pshgeom(mother);
 tkgeom(mother);
 ctcgeom(mother);
 cout <<" AMSGeom-I-Shuttle setup selected."<<endl;
}
else if (strstr(AMSJob::gethead()->getsetup(),"AMS02")){
 cout <<" AMSGeom-I-AMS02 setup selected."<<endl;
 magnetgeom02(mother);
 tofgeom02(mother);
 tkgeom02(mother);
 pshgeom02(mother);
 antigeom02(mother);

#ifdef  __G4AMS__
 trdgeom02(mother);
 srdgeom02(mother);
// ecalgeom02(mother);
// richgeom02(mother);
#else
 trdgeom02(mother);
 srdgeom02(mother);
 ecalgeom02(mother);
 richgeom02(mother);
#endif
}
else{ 
 cerr <<" AMSGeom-F-Unknown setup selected. "<<AMSJob::gethead()->getsetup()<<endl;
 exit(1);
}




#ifdef __G4AMS__
if(MISCFFKEY.G3On){
#endif
false_mother.MakeG3Volumes();

  cout << "AMSGeom::-I-"<<getNpv()<<" Physical volumes, "<<getNlv()<<" logical volumes and "<<getNrm()<<" rotation matrixes have been created "<<endl;
// Check recursion
AMSNodeMap geommap;
geommap.map(false_mother);
int abort=0;
int i;
for( i=0;;i++){
 AMSgvolume* p1=(AMSgvolume*)geommap.getid(i);
 AMSgvolume* p2=(AMSgvolume*)geommap.getid(i+1);
 if(!p1 || !p2)break;
 if(!strcmp(p1->getname(), p2->getname()) && p1->VolumeHasG3Attributes() &&
 p2->VolumeHasG3Attributes()){
  if(p1->down() && p2->down()){
   cerr<<" AMSGeom-F-Geant3RecursionProblemDetectedForVolumes "<<
   p1->getname()<<" "<<p1->getid()<<" , "<<p2->getname()<<" "<<p2->getid()<<endl;
   abort=1;
  }
  else if(p1->down() || p2->down()){
//   cerr<<" AMSGeom-W-PossibleGeant3RecursionProblemDetectedForVolumes "<<
//   p1->getname()<<" "<<p1->getid()<<" , "<<p2->getname()<<" "<<p2->getid()<<endl;
  }
 }
}
if(abort){
 cerr<<"AMSGeom-F-Geant3VolumesWithDaugthersMustHaveDifferentNames"<<endl;
 exit(1);
}
GGCLOS();
cout <<" AMSGeom-I-TotalOf "<<GlobalRotMatrixNo()<<" rotation matrixes  created"<<endl;

#ifdef __G4AMS__
}
getNpv()=0;
getNlv()=0;
getNrm()=0;
#endif

//AMSgObj::GVolMap.map(mother);
//#ifdef __AMSDEBUG__
//// if(AMSgvolume::debug)AMSgObj::GVolMap.print();
//#endif
}

void magnetgeom(AMSgvolume & mother){
AMSID amsid;
geant par[6]={0.,0.,0.,0.,0.,0.};
char name[5]="MAGN";
geant coo[3]={0.,0.,0.};
number nrm[3][3]={1.,0.,0.,0.,1.,0.,0.,0.,1.};
integer gid=0;
AMSNode * cur;
AMSNode * dau;
AMSgtmed *p;
      gid=1;
      par[0]=112./2;
      par[1]=129.6/2;
      par[2]=40.;
      dau=mother.add(new AMSgvolume(
      "MAGNET",0,name,"TUBE",par,3,coo,
       nrm, "ONLY",0,gid,1));
      gid=2;
      par[0]=111.4/2.;
      par[1]=112.0/2.;
      par[2]=40.;
      dau=mother.add(new AMSgvolume(
      "1/2ALUM",0,"ALT1","TUBE",par,3,coo,nrm, "ONLY",0,gid,1));
      gid=3;
      par[0]=129.6/2.;
      par[1]=130.4/2.;
      par[2]=40.;
      dau=mother.add(new AMSgvolume(
      "1/2ALUM",0,"ALT2","TUBE",par,3,coo,nrm, "ONLY",0,gid++,1));
      par[0]=112/2.;
      par[1]=165./2.;
      par[2]=0.75;
      coo[2]=40.75;
      dau=mother.add(new AMSgvolume(
      "1/2ALUM",0,"ALT3","TUBE",par,3,coo,nrm, "ONLY",0,gid++,1));
    
      par[0]=112/2.;
      par[1]=165.6/2.;
      par[2]=0.75;
      coo[2]=-40.75;
      dau=mother.add(new AMSgvolume(
      "1/2ALUM",0,"ALT4","TUBE",par,3,coo,nrm, "ONLY",0,gid++,1));
    
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
    amsid.getname(),0,"TOFH","TUBE",par,3,coo,nrm1,"ONLY",1,gid,1));
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
    amsid.getname(),0,"TOFH","TUBE",par,3,coo,nrm1,"ONLY",1,gid,1));
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
    amsid.getname(),nrot,"TOFC","BOX",par,3,coo,nrm,"MANY",1,gid,1));
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
    amsid.getname(),nrot,"TOFS","BOX",par,3,coo,nrm,"ONLY",1,gid,1));
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
    amsid.getname(),nrot,"TOFB","BOX",par,3,coo,nrm,"ONLY",1,gid,1));
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
    amsid.getname(),nrot,"TOFB","BOX",par,3,coo,nrm,"ONLY",1,gid,1));
  }      //   <<<============= end of sc. bars loop ==========
}   //   <<<============= end of sc. planes loop =============
}

// for future AMS02 setup:
//
void tofgeom02(AMSgvolume & mother){ 
number pr[3]={0.,0.,0.};
geant par[6]={0.,0.,0.};
number nrm[3][3]={1.,0.,0., 0.,1.,0., 0.,0.,1.};
number nrm1[3][3]={1.,0.,0., 0.,1.,0., 0.,0.,1.};
number nrm2[3][3]={0.,-1.,0.,1.,0.,0., 0.,0.,1.};// for rotated layers (90degr)
number inrm[3][3];
number dz,dz1,dz2,zc,dx,dxt;
integer btyp,nbm,nrot;
int ii,kk;
char inum[11];
char in[2]="0";
char vname[5];
char name[5];
number co[3]={0.,0.,0.};
geant coo[3]={0.,0.,0.};
integer gid=0;
integer nmed;
AMSNode *ptofc;
AMSNode *cur;
AMSNode *dau;
AMSgtmed *p;
AMSID amsid;
//
strcpy(inum,"0123456789");
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
dau=mother.add(new AMSgvolume(
    "TOF_HONEYCOMB",0,"TOFH","TUBE",par,3,coo,nrm1,"ONLY",1,gid,1));
//--------------
//          <-- create/position bot supp. honeycomb structures
gid=2;
par[0]=0.;                //Ri
par[1]=TOFDBc::supstr(8); //Ro
par[2]=TOFDBc::supstr(7)/2.; //Dz
coo[0]=TOFDBc::supstr(5);    // x-shift from "0" of mother
coo[1]=TOFDBc::supstr(6);    // y-shift ...
coo[2]=TOFDBc::supstr(2)-TOFDBc::supstr(7)/2.;// z-centre of bot supp. honeycomb
dau=mother.add(new AMSgvolume(
    "TOF_HONEYCOMB",0,"TOFH","TUBE",par,3,coo,nrm1,"ONLY",1,gid,1));
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
    gid=100*(ip+1)+ib+1;
    strcpy(vname,"TC");
    kk=ip*SCMXBR+ib+1;//solid numbering of counters
    ii=kk/10;
    in[0]=inum[ii];
    strcat(vname,in);
    ii=kk%10;
    in[0]=inum[ii];
    strcat(vname,in);
    ptofc=mother.add(new AMSgvolume(
    "TOF_SC_COVER",nrot,vname,"BOX",par,3,coo,nrm,"ONLY",1,gid,1));
//-------
//        <-- cr/position scintillator inside counter
//
    par[0]=pr[0]-TOFDBc::plnstr(13);// pure scint. x-size/2
    par[1]=pr[1]-TOFDBc::plnstr(11);// pure scint. y-size/2 (minus light guide)
    par[2]=pr[2]-TOFDBc::plnstr(7);// pure scint. z-size/2
    coo[0]=0.;
    coo[1]=0.;
    coo[2]=0.;
    dau=ptofc->add(new AMSgvolume(
    "TOF_SCINT",0,"TOFS","BOX",par,3,coo,nrm1,"ONLY",gid==101?1:-1,gid,1));
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
//                                   <=== create pmt1-box
    gid=1000+100*(ip+1)+ib+1;
///*
    dau=mother.add(new AMSgvolume(
    "TOF_PMT_BOX",nrot,"TOFB","BOX",par,3,coo,nrm,"ONLY",1,gid,1));
//*/
//-----
    if(TOFDBc::plrotm(ip)==0){
      coo[0]=co[0]-dxt/2.+ib*(dx-TOFDBc::plnstr(4));
      coo[1]=co[1]+pr[1]+TOFDBc::plnstr(9)/2.;
    }
    if(TOFDBc::plrotm(ip)==1){
      coo[0]=co[0]+pr[1]+TOFDBc::plnstr(9)/2.;
      coo[1]=co[1]-dxt/2.+ib*(dx-TOFDBc::plnstr(4));
    }
//                                   <=== create pmt2-box
    gid=2000+100*(ip+1)+ib+1;
///*
    dau=mother.add(new AMSgvolume(
    "TOF_PMT_BOX",nrot,"TOFB","BOX",par,3,coo,nrm,"ONLY",1,gid,1));
//*/
//
  }      //   <<<============= end of sc. bars loop ==========
}   //   <<<============= end of sc. planes loop =============
  cout<<"AMSGEOM: TOF-geometry done!"<<endl;
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
       "ANTI_SUPTB",0,"ASTB","TUBE",par,5,coo,nrm, "ONLY",0,gid,1));
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
       "ANTI_WRAP",0,"AMOT","TUBE",par,5,coo,nrm, "ONLY",0,gid,1));
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
       "ANTI_SCINT",0,"ANTS","TUBS",par,5,coo,nrm, "MANY",1,gid,1));
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
       "ANTI_WRAP",nrot,"AGRV","TUBS",par,5,coo,nrd, "ONLY",1,gid,1));
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
       "ANTI_SCINT",nrot,"ANTB","TUBS",par,5,coo,nrd, "ONLY",1,gid,1));
  }// ---> end of sector loop
//
}
//---------------------------------------------------------------------

void ctcgeom(AMSgvolume & mother){
extern void ctcgeomE(AMSgvolume &, integer iflag);
extern void ctcgeomAG(AMSgvolume& );
extern void ctcgeomAGPlus(AMSgvolume& );
if(strstr(AMSJob::gethead()->getsetup(),"CTCAnnecyPlus")){
  ctcgeomAGPlus(mother);
  cout<<" CTCGeom-I-Annecy+Lead setup for CTC selected"<<endl;
}
else{
ctcgeomAG(mother);
  cout<<" CTCGeom-I-Annecy setup for CTC selected"<<endl;
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
    "CTC_HONEYCOMB",0,"SUPP","BOX ",par,3,coo,nrm,"ONLY",0,gid,1));
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
    "CTC_WALL",0,"WALL","BOX ",par,3,coo,nrm,"MANY",0,gid,1));
//---
//  <--- put into this box "dummy_media"("air") to get space for aerogel
//
gid=3;
par[0]=dx-wl;                
par[1]=dy-wl;
par[2]=dz-wl;
coo[2]=zr+dz;
dau=mother.add(new AMSgvolume(
    "CTC_DUMMYMED",0,"DVOL","BOX ",par,3,coo,nrm,"MANY",0,gid,1));
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
    "CTC_WALL",0,"HSEP","BOX ",par,3,coo,nrm,"MANY",0,gid,1));
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
        "CTC_AEROGEL",0,"AGEL","BOX ",par,3,coo,nrm,"ONLY",1,gid,1));
  }
  if(iflag==1){
    gid=11001+2*i;
    coo[0]=xleft+i*step;
    coo[2]=cooz;
    dau=mother.add(new AMSgvolume(
        "CTC_AEROGEL",0,"AGEL","BOX ",par,3,coo,nrm,"ONLY",1,gid,1));//1-st in top row
    gid=12001+2*i;
    coo[2]=cooz-dz-2.*gp-wl;
    dau=mother.add(new AMSgvolume(
        "CTC_AEROGEL",0,"AGEL","BOX ",par,3,coo,nrm,"ONLY",1,gid,1));//1-st in bot row
//        
    gid=11002+2*i;
    coo[0]=xleft+i*step+2.*dxag+2.*gp+wls;
    coo[2]=cooz;
    dau=mother.add(new AMSgvolume(
        "CTC_AEROGEL",0,"AGEL","BOX ",par,3,coo,nrm,"ONLY",1,gid,1));//2-nd in top row
    gid=12002+2*i;
    coo[2]=cooz-dz-2.*gp-wl;
    dau=mother.add(new AMSgvolume(
        "CTC_AEROGEL",0,"AGEL","BOX ",par,3,coo,nrm,"ONLY",1,gid,1));//2-nd in bot row
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
      "CTC_SEP",0,"VSEP","BOX ",par,3,coo,nrm,"ONLY",1,gid,1));
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
        "CTC_WLS",0,"WLS ","BOX ",par,3,coo,nrm,"ONLY",1,gid,1));
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
        "CTC_WLS",0,"WLS ","BOX ",par,3,coo,nrm,"ONLY",1,gid,1));
    gid=22001+i;    // bot row
    coo[2]=cooz-2.*dz-wl-2.*gwls;
    dau=mother.add(new AMSgvolume(
        "CTC_WLS",0,"WLS ","BOX ",par,3,coo,nrm,"ONLY",1,gid,1));
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
  // modified by V. Choutko 11/1/98
  // Change Geom setup to conform the reality

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
      "CTC_HONEYCOMB",0,"SUPP","BOX ",par,3,coo,nrm,"ONLY",0,3000000,1));
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
     coo[0]=0;
     coo[1]=-3.;
     coo[2]=zr+0.5*CTCDBc::getupsize(2)-0.5*CTCDBc::getthcsize(2);
     pLayer=mother.add(new AMSgvolume(
       "CTC_DUMMYMED",0,cdum[ilay-1],"BOX ",par,3,coo,nrm,"MANY",0,1000000,1));
    }
    else{
      for(i=0;i<3;i++)par[i]=0.5*CTCDBc::getupsize(i);
      coo[0]=-6.;
      coo[1]=3.;
      coo[2]=zr-0.5*CTCDBc::getupsize(2)-0.5*CTCDBc::getthcsize(2);
      pLayer=mother.add(new AMSgvolume(
       "CTC_DUMMYMED",0,cdum[ilay-1],"BOX ",par,3,coo,nrm,"MANY",0,2000000,1));
    }

#ifdef __AMSDEBUG__
    cout <<" Layer "<<ilay<<" "<<coo[0]<<" "<<coo[1]<<" "<<coo[2]<<endl;      
    geant cool[3];
    cool[0]=coo[0];
    cool[1]=coo[1];
    cool[2]=coo[2];
#endif


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
        coo[0]-=(par[0]+parwal[0])*2*(CTCDBc::getnx()-1-j);
        coo[1]=(CTCDBc::getny()-1)*(par[1]+pay[1]);
        coo[1]-=(par[1]+pay[1])*2*(CTCDBc::getny()-1-i);
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
            char cvolptf[]="PTAU";
            cvolptf[3]=cdum[ilay-1][0];
            gid=1000000*ilay+1000*(i+1)+100*(j+1)+10+ix+1+2*iy;
            pLayer->add(new AMSgvolume(
            "ATC_PTAE",0,cvolptf,"BOX ",parptf,3,cooptf,nrm,"MANY",1,gid,1));
            // add sensitive ptfe
            for(k=0;k<3;k++)paragl[k]=0.5*CTCDBc::getagelsize(k);
            char cvolptfs[]="PTFU";
            cvolptfs[3]=cdum[ilay-1][0];
            parptf[2]=ptfe/2;
            pLayer->add(new AMSgvolume(
            "ATC_PTFE",0,cvolptfs,"BOX ",parptf,3,cooptf,nrm,"ONLY",1,gid,1));
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
#ifdef __AMSDEBUG__
             
             cout <<" Cell "<<i<<" "<<j<<" "<<ix<<" "<<iy<<" "<<
               cooagl[0]+cool[0]<<" "<<cooagl[1]+cool[1]<<" "<<
               cooptf[2]+cool[2]<<" "<<cooagl[0]<<" "<<cooagl[1]<<" "<<cooptf[2]<<endl;
#endif

             pLayer->add(new AMSgvolume(
             "ATC_AEROGEL",0,cvola,"BOX ",paragl,3,cooagl,nrm,"ONLY",1,gid,1));
            }
            for(k=0;k<3;k++)parpmt[k]=0.5*CTCDBc::getpmtsize(k);
            coopmt[0]=cooptf[0];
            coopmt[1]=cooptf[1];
            coopmt[2]=coo[2]-(0.5*CTCDBc::getcellsize(2)
            -parpmt[2]);  
            char cvolpmt[]="PMAU";
            cvolpmt[3]=cdum[ilay-1][0];
            coopmt[2]+=-CTCDBc::getpmtwindowth()/2;
            parpmt[2]+=-CTCDBc::getpmtwindowth()/2;
            gid=1000000*ilay+1000*(i+1)+100*(j+1)+30+ix+1+2*iy;
            pLayer->add(new AMSgvolume(
            "TOF_PMT_BOX",0,cvolpmt,"BOX ",parpmt,3,coopmt,nrm,"ONLY",1,gid,1));
            char cvolpmtw[]="PMTU";
            cvolpmtw[3]=cdum[ilay-1][0];
            coopmt[2]+=parpmt[2]+CTCDBc::getpmtwindowth()/2;
            parpmt[2]=CTCDBc::getpmtwindowth()/2;
            gid=1000000*ilay+1000*(i+1)+100*(j+1)+30+ix+1+2*iy;
            pLayer->add(new AMSgvolume(
            "TOF_PMT_WINDOW",0,cvolpmtw,"BOX ",parpmt,3,coopmt,nrm,"ONLY",1,gid,1));
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
     coo[0]=0;
     coo[1]=0.;
     coo[2]=zr+0.5*CTCDBc::getupsize(2)-0.5*CTCDBc::getthcsize(2);
     pLayer=mother.add(new AMSgvolume(
       "CTC_DUMMYMED",0,cdum[ilay-1],"BOX ",par,3,coo,nrm,"MANY",0,1000000,1));
    }
    else{
      par[0]= 0.25*CTCDBc::getcellsize(0)+0.5*CTCDBc::getwallsize(0);
      for(i=1;i<3;i++)par[i]=0.5*CTCDBc::getupsize(i);
      coo[0]=-6.+0.5*CTCDBc::getupsize(0)+0.25*CTCDBc::getcellsize(0)+
      CTCDBc::getwallsize(0);
      coo[1]=3.;
      coo[2]=zr-0.5*CTCDBc::getupsize(2)-0.5*CTCDBc::getthcsize(2);
      pLayer=mother.add(new AMSgvolume(
       "CTC_DUMMYMED",0,cdum[ilay-1],"BOX ",par,3,coo,nrm,"MANY",0,2000000,1));
    }
#ifdef __AMSDEBUG__
    cout <<" Layer "<<ilay<<" "<<coo[0]<<" "<<coo[1]<<" "<<coo[2]<<endl;      
    geant cool[3];
    cool[0]=coo[0];
    cool[1]=coo[1];
    cool[2]=coo[2];
#endif

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
        coo[1]-=(par[1]+pay[1])*2*(CTCDBc::getny()-1-i);
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
            char cvolptf[]="PTAU";
            cvolptf[3]=cdum[ilay-1][0];
            gid=1000000*ilay+1000*(i+1)+100*(j+1)+10+ix+1+2*iy;
            pLayer->add(new AMSgvolume(
            "ATC_PTAE",0,cvolptf,"BOX ",parptf,3,cooptf,nrm,"MANY",1,gid,1));
            // add sensitive ptfe
            char cvolptfs[]="PTFU";
            cvolptfs[3]=cdum[ilay-1][0];
            parptf[2]=ptfe/2;
            for(k=0;k<3;k++)paragl[k]=0.5*CTCDBc::getagelsize(k);
            pLayer->add(new AMSgvolume(
            "ATC_PTFE",0,cvolptfs,"BOX ",parptf,3,cooptf,nrm,"MANY",1,gid,1));
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
#ifdef __AMSDEBUG__
             
             cout <<" Cell "<<i<<" "<<j<<" "<<ix<<" "<<iy<<" "<<
               cooagl[0]+cool[0]<<" "<<cooagl[1]+cool[1]<<" "<<
               cooptf[2]+cool[2]<<" "<<cooagl[0]<<" "<<cooagl[1]<<" "<<cooptf[2]<<endl;
#endif

             pLayer->add(new AMSgvolume(
             "ATC_AEROGEL",0,cvola,"BOX ",paragl,3,cooagl,nrm,"ONLY",1,gid,1));
            }
            for(k=0;k<3;k++)parpmt[k]=0.5*CTCDBc::getpmtsize(k);
            coopmt[0]=cooptf[0];
            coopmt[1]=cooptf[1];
            coopmt[2]=coo[2]-(0.5*CTCDBc::getcellsize(2)
            -parpmt[2]);  
            char cvolpmt[]="PMAU";
            cvolpmt[3]=cdum[ilay-1][0];
            coopmt[2]+=-CTCDBc::getpmtwindowth()/2;
            parpmt[2]+=-CTCDBc::getpmtwindowth()/2;
            gid=1000000*ilay+1000*(i+1)+100*(j+1)+30+ix+1+2*iy;
            pLayer->add(new AMSgvolume(
            "TOF_PMT_BOX",0,cvolpmt,"BOX ",parpmt,3,coopmt,nrm,"ONLY",1,gid,1));
            char cvolpmtw[]="PMTU";
            cvolpmtw[3]=cdum[ilay-1][0];
            coopmt[2]+=parpmt[2]+CTCDBc::getpmtwindowth()/2;
            parpmt[2]=CTCDBc::getpmtwindowth()/2;
            gid=1000000*ilay+1000*(i+1)+100*(j+1)+30+ix+1+2*iy;
            pLayer->add(new AMSgvolume(
            "TOF_PMT_WINDOW",0,cvolpmtw,"BOX ",parpmt,3,coopmt,nrm,"ONLY",1,gid,1));

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
      "CTC_HONEYCOMB",0,"SUPP","BOX ",par,3,coo,nrm,"ONLY",0,3000000,1));
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
       "TUNGSTEN",0,"AMP1","BOX ",par,3,coo,nrm,"ONLY",0,1000001,1));
  

  for(ilay=1;ilay<3;ilay++){
    if(ilay==1){
     for(i=0;i<3;i++)par[i]=0.5*CTCDBc::getupsize(i);
     coo[0]=coo[1]=0.;
     coo[2]=zr+0.5*CTCDBc::getupsize(2)-0.5*CTCDBc::getthcsize(2);
     pLayer=mother.add(new AMSgvolume(
       "CTC_DUMMYMED",0,cdum[ilay-1],"BOX ",par,3,coo,nrm,"MANY",0,1000000,1));
    }
    else{
      for(i=0;i<3;i++)par[i]=0.5*CTCDBc::getupsize(i);
      coo[0]=coo[1]=5.5;
      coo[2]=zr-0.5*CTCDBc::getupsize(2)-0.5*CTCDBc::getthcsize(2)-1.1;   // for W
      pLayer=mother.add(new AMSgvolume(
       "CTC_DUMMYMED",0,cdum[ilay-1],"BOX ",par,3,coo,nrm,"MANY",0,2000000,1));
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
       "CTC_DUMMYMED",0,cdum[ilay-1],"BOX ",par,3,coo,nrm,"MANY",0,1000000,1));
    }
    else{
      par[0]= 0.25*CTCDBc::getcellsize(0)+0.5*CTCDBc::getwallsize(0);
      for(i=1;i<3;i++)par[i]=0.5*CTCDBc::getupsize(i);
      coo[0]=5.5-0.5*CTCDBc::getupsize(0)-0.25*CTCDBc::getcellsize(0)-
      CTCDBc::getwallsize(0);
      coo[1]=5.5;
      coo[2]=zr-0.5*CTCDBc::getupsize(2)-0.5*CTCDBc::getthcsize(2)-1.1;
      pLayer=mother.add(new AMSgvolume(
       "CTC_DUMMYMED",0,cdum[ilay-1],"BOX ",par,3,coo,nrm,"MANY",0,2000000,1));
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



   //cout<<" TKGeom-I-Planes option selected"<<endl;


AMSID amsid;
geant xx[3]={0.9,0.2,0.2};
geant  xnrm[3][3];
geant par[6]={0.,0.,0.,0.,0.,0.};
number nrm[3][3]={1.,0.,0.,0.,1.,0.,0.,0.,1.};
number inrm[3][3];
char name[5];
geant coo[3]={0.,0.,0.};

integer gid=0;
integer nrot=501; // Temporary arbitary assignment
//static AMSgvolume mother(0,0,AMSDBc::ams_name,"BOX",par,3,coo,nrm,"ONLY",
//                         0,gid);  // temporary a dummy volume
AMSNode * cur;
AMSNode * dau;
int i;
for ( i=0;i<TKDBc::nlay();i++){
ostrstream ost(name,sizeof(name));
 ost << "STK"<<i+1<<ends;
 coo[0]=TKDBc::xposl(i);
 coo[1]=TKDBc::yposl(i);
 coo[2]=TKDBc::zposl(i);
 nrm[0][0]=TKDBc::nrml(0,0,i);
 nrm[0][1]=TKDBc::nrml(0,1,i); 
 nrm[0][2]=TKDBc::nrml(0,2,i); 
 nrm[1][0]=TKDBc::nrml(1,0,i); 
 nrm[1][1]=TKDBc::nrml(1,1,i); 
 nrm[1][2]=TKDBc::nrml(1,2,i); 
 nrm[2][0]=TKDBc::nrml(2,0,i); 
 nrm[2][1]=TKDBc::nrml(2,1,i); 
 nrm[2][2]=TKDBc::nrml(2,2,i); 
    int ii;
    for ( ii=0;ii<5;ii++)par[ii]=TKDBc::layd(i,ii);
      gid=i+1;
      integer status=1;
      integer rgid;
      if(TKDBc::update())TKDBc::SetLayer(i+1,status,coo,nrm,gid);
      else               TKDBc::GetLayer(i+1,status,coo,nrm,rgid);
#ifdef __AMSDEBUG__
         if(i==2 && AMSgvolume::debug){
          MTX(xnrm,xx);
          int a1,a2;
          for( a1=0;a1<3;a1++)for( a2=0;a2<3;a2++)nrm[a2][a1]=xnrm[a1][a2];
         }
#endif
      dau=mother.add(new AMSgvolume(
      "VACUUM",nrot++,name,"CONE",par,5,coo,nrm, "ONLY",0,gid,1));
      //      cout <<" layer "<<i<<" "<<nrot<<endl;
      int j;
      for (j=0;j<TKDBc::nlad(i+1);j++){
       int k;
       AMSgvolume * lad[2];
       for(k=0;k<2;k++){
         ost.seekp(0);
         ost << "L" << i+1<<(j+1)*2+k<<ends;
         if(k==0)par[0]=TKDBc::ssize_inactive(i,0)*TKDBc::nhalf(i+1,j+1)/2.;
         else par[0]=TKDBc::ssize_inactive(i,0)*
         (TKDBc::nsen(i+1,j+1)-TKDBc::nhalf(i+1,j+1))/2.;
         par[1]=TKDBc::ssize_inactive(i,1)/2;
         par[2]=TKDBc::silicon_z(i)/2;
         coo[0]=(2*k-1)*(TKDBc::ssize_inactive(i,0)*TKDBc::nsen(i+1,j+1)/2+
         TKDBc::halfldist(i)-par[0]);
         coo[1]=(TKDBc::nlad(i+1)-j)*TKDBc::c2c(i)-
         (TKDBc::nlad(i+1)+1)*TKDBc::c2c(i)/2.;
         coo[2]=TKDBc::zpos(i);
         VZERO(nrm,9*sizeof(nrm[0][0])/4);
         if(k==0){
          nrm[0][0]=1;
          nrm[1][1]=1;
          nrm[2][2]=1;
         }
         else{
          nrm[0][0]=-1;
          nrm[1][1]=-1;
          nrm[2][2]= 1;
         }
      //
      //  Ladder 
      //
        gid=i+1+10*(j+1)+100000;
        integer status=1;
        int rgid;
        if(TKDBc::update())TKDBc::SetLadder(i,j,k,status,coo,nrm,gid);
        else               TKDBc::GetLadder(i,j,k,status,coo,nrm,rgid);
        //        if(i==5){
        //          cout <<"Lad "<<j<<" "<<k<<" "<< coo[0]<<" "<<coo[1]<<" "<<coo[2]<<endl;
        //        }
        lad[k]=(AMSgvolume*)dau->add(new AMSgvolume(
        "NONACTIVE_SILICON",nrot++,name,"BOX",par,3,coo,nrm,"ONLY",1,gid,1));
       }

    
       // Now Sensors

       for ( k=0;k<TKDBc::nhalf(i+1,j+1);k++){
         ost.seekp(0);
         ost << "S" << i+1<<(j+1)*2<<ends;
         par[0]=TKDBc::ssize_active(i,0)/2;
         par[1]=TKDBc::ssize_active(i,1)/2;
         par[2]=TKDBc::silicon_z(i)/2;
         coo[0]=-TKDBc::ssize_inactive(i,0)/2.+
         (2*k+2-TKDBc::nhalf(i+1,j+1))*TKDBc::ssize_inactive(i,0)/2.;
         coo[1]=0;
         coo[2]=0;
         VZERO(nrm,9*sizeof(nrm[0][0])/4);
         nrm[0][0]=1;
         nrm[1][1]=1;
         nrm[2][2]=1;
#ifdef __AMSDEBUG__
         if(i==2 && AMSgvolume::debug){
          MTX(xnrm,xx);
          int a1,a2;
          for( a1=0;a1<3;a1++)for( a2=0;a2<3;a2++)nrm[a2][a1]=xnrm[a1][a2];
         }
#endif
         gid=i+1+10*(j+1)+1000*(k+1);
         if(TKDBc::activeladdshuttle(i+1,j+1,0)){  
          integer status=1;
          if(TKDBc::update())TKDBc::SetSensor(i,j,k,status,coo,nrm,gid);
          else               TKDBc::GetSensor(i,j,k,status,coo,nrm);
          if(status){
           cur=lad[0]->add(new AMSgvolume(
          "ACTIVE_SILICON",nrot++,name,"BOX",par,3,coo,nrm,"ONLY",1,gid,1));
           //          cout <<"add volume "<<name<<" "<<gid<<endl;
           //          cout <<" to "<<*(lad[0]);
          }
         }
       }

       for ( k=TKDBc::nhalf(i+1,j+1);k<TKDBc::nsen(i+1,j+1);k++){
        ost.seekp(0);
        ost << "S" << i+1<<(j+1)*2+1<<ends;
        par[0]=TKDBc::ssize_active(i,0)/2;
        par[1]=TKDBc::ssize_active(i,1)/2;
        par[2]=TKDBc::silicon_z(i)/2;
        coo[0]=-TKDBc::ssize_inactive(i,0)/2.+
        (2*(k-TKDBc::nhalf(i+1,j+1))+2+
         TKDBc::nhalf(i+1,j+1)-TKDBc::nsen(i+1,j+1))*
        TKDBc::ssize_inactive(i,0)/2.;
        coo[1]=0;
        coo[2]=0;
        VZERO(nrm,9*sizeof(nrm[0][0])/4);
        nrm[0][0]=1;
        nrm[1][1]=1;
        nrm[2][2]=1;
#ifdef __AMSDEBUG__
         if(i==2 && AMSgvolume::debug){
          MTX(xnrm,xx);
          int a1,a2;
          for( a1=0;a1<3;a1++)for( a2=0;a2<3;a2++)nrm[a2][a1]=xnrm[a1][a2];
         }
#endif
        gid=i+1+10*(j+1)+1000*(k+1);
        if(TKDBc::activeladdshuttle(i+1,j+1,1)){  
         integer status=1;
         if(TKDBc::update())TKDBc::SetSensor(i,j,k,status,coo,nrm,gid);
         else               TKDBc::GetSensor(i,j,k,status,coo,nrm);
         if(status){
          cur=lad[1]->add(new AMSgvolume(
          "ACTIVE_SILICON",nrot++,name,"BOX",par,3,coo,nrm,"ONLY",1,gid,1));
          //          cout <<"add volume "<<name<<" "<<gid<<endl;
          //          cout <<" to "<<*(lad[1]);
         }
        }
       }


      }
      // Now Support foam
 
      for ( j=0;j<TKDBc::nlad(i+1);j++){
       ost.seekp(0);
       ost << "FOA" << i+1<<ends;
       par[0]=TKDBc::ssize_inactive(i,0)*TKDBc::nsen(i+1,j+1)/2.;
       par[1]=TKDBc::c2c(i)/2;
       par[2]=(TKDBc::support_foam_w(i))/2;    
       coo[0]=0;
       coo[1]=(TKDBc::nlad(i+1)-j)*TKDBc::c2c(i)-(TKDBc::nlad(i+1)+1)*TKDBc::c2c(i)/2.;
       coo[2]=TKDBc::zpos(i)-TKDBc::silicon_z(i)/2.-par[2]-TKDBc::support_foam_tol(i);
       VZERO(nrm,9*sizeof(nrm[0][0])/4);
       nrm[0][0]=1;
       nrm[1][1]=1;
       nrm[2][2]=1;
       gid=i+1+10*(j+1);
       cur=dau->add(new AMSgvolume(
       "Tr_Foam",nrot++,name,"BOX",par,3,coo,nrm,"ONLY",1,gid,1));

  }

      // Now Honecomb Planes

       ost.seekp(0);
       ost << "PLA" << i+1<<ends;
       par[0]=0;
       par[1]=TKDBc::support_hc_r(i);
       par[2]=TKDBc::support_hc_w(i)/2;
       coo[0]=0;
       coo[1]=0;
       coo[2]=TKDBc::zpos(i)-TKDBc::silicon_z(i)/2.+TKDBc::support_hc_z(i);
       VZERO(nrm,9*sizeof(nrm[0][0])/4);
       nrm[0][0]=1;
       nrm[1][1]=1;
       nrm[2][2]=1;
       gid=i+1;
       cur=dau->add(new AMSgvolume(
      "Tr_Honeycomb",nrot++,name,"TUBE",par,3,coo,nrm,"ONLY",1,gid,1));

       // now Markers 1
       for(j=0;j<4;j++){
        ost.seekp(0);
        ost <<"MK1"<<i+1<<ends;
        par[0]=0;
        par[1]=0.3;
        par[2]=TKDBc::support_hc_w(i)/2;
        coo[0]=TKDBc::PlMarkerPos(0,i,j,0);
        coo[1]=TKDBc::PlMarkerPos(0,i,j,1);
        coo[2]=TKDBc::PlMarkerPos(0,i,j,2)-par[2]-TKDBc::support_hc_z(i);
        VZERO(nrm,9*sizeof(nrm[0][0])/4);
        nrm[0][0]=1;
        nrm[1][1]=1;
        nrm[2][2]=1;
        gid=j+1;
        integer status=1;
        if(TKDBc::update())TKDBc::SetMarker(i+1,0,j,status,coo,nrm,gid);
        else               TKDBc::GetMarker(i+1,0,j,status,coo,nrm);
        cur->add(new AMSgvolume(
       "VACUUM",nrot++,name,"TUBE",par,3,coo,nrm,"ONLY",1,gid,1));
       }
       // now Markers 2
        number sq2=sqrt(2.);
       for(j=0;j<4;j++){
        ost.seekp(0);
        ost <<"MK2"<<i+1<<ends;
        par[0]=0;
        par[1]=0.3;
        number r=sqrt(TKDBc::PlMarkerPos(0,i,j,0)*TKDBc::PlMarkerPos(0,i,j,0)
                + TKDBc::PlMarkerPos(0,i,j,1)*TKDBc::PlMarkerPos(0,i,j,1));
        par[2]=(TKDBc::support_hc_r(i)-r-0.3)/2;
        number dc=(par[2]+0.3)/sq2;
        coo[0]=TKDBc::PlMarkerPos(1,i,j,0);
        if(coo[0]>0)coo[0]+=dc;
        else coo[0]+=-dc;
        coo[1]=TKDBc::PlMarkerPos(1,i,j,1);
        if(coo[1]>0)coo[1]+=dc;
        else coo[1]+=-dc;
        coo[2]=TKDBc::PlMarkerPos(1,i,j,2)-TKDBc::support_hc_z(i);
        nrm[0][0]=1./sq2;
        nrm[1][0]=-1/sq2;
        nrm[2][0]=0;
        nrm[0][1]=0;
        nrm[1][1]=0;
        nrm[2][1]=-1;
        nrm[0][2]=1./sq2;
        nrm[1][2]=1./sq2;
        nrm[2][2]=0;
        gid=j+1;
        integer status=1;
        if(TKDBc::update())TKDBc::SetMarker(i+1,1,j,status,coo,nrm,gid);
        else               TKDBc::GetMarker(i+1,1,j,status,coo,nrm);
        cur->add(new AMSgvolume(
       "VACUUM",nrot++,name,"TUBE",par,3,coo,nrm,"ONLY",1,gid,1));
        }

/*
       // Now Elec Left

       for ( j=0;j<TKDBc::nlad(i+1);j++){
        ost.seekp(0);
        ost << "ELL" << i+1<<ends;
        par[0]=TKDBc::zelec(i,1)/2.;
        par[1]=TKDBc::c2c(i)/2.;
        par[2]=TKDBc::zelec(i,0)/2;
        coo[0]=-TKDBc::nsen(i+1,j+1)*TKDBc::ssize_inactive(i,0)/2.-par[0];
        coo[1]=(TKDBc::nlad(i+1)-j)*TKDBc::c2c(i)-
        (TKDBc::nlad(i+1)+1)*TKDBc::c2c(i)/2.;
        coo[2]=TKDBc::zelec(i,2)+par[2];
        VZERO(nrm,9*sizeof(nrm[0][0])/4);
        nrm[0][0]=1;
        nrm[1][1]=1;
        nrm[2][2]=1;
        gid=i+1+10*(j+1);
        cur=dau->add(new AMSgvolume(
       "ELECTRONICS",nrot++,name,"BOX",par,3,coo,nrm,"ONLY",1,gid,1));

       }

       // Now Elec R

       for ( j=0;j<TKDBc::nlad(i+1);j++){
        ost.seekp(0);
        ost << "ELR" << i+1<<ends;
        par[0]=TKDBc::zelec(i,1)/2.;
        par[1]=TKDBc::c2c(i)/2.;
        par[2]=TKDBc::zelec(i,0)/2;
        coo[0]=+TKDBc::nsen(i+1,j+1)*TKDBc::ssize_inactive(i,0)/2.+par[0];
        coo[1]=(TKDBc::nlad(i+1)-j)*TKDBc::c2c(i)-
        (TKDBc::nlad(i+1)+1)*TKDBc::c2c(i)/2.;
        coo[2]=TKDBc::zelec(i,2)+par[2];
        VZERO(nrm,9*sizeof(nrm[0][0])/4);
        nrm[0][0]=1;
        nrm[1][1]=1;
        nrm[2][2]=1;
        gid=i+1+10*(j+1);
        cur=dau->add(new AMSgvolume(
       "ELECTRONICS",nrot++,name,"BOX",par,3,coo,nrm,"ONLY",1,gid,1));
        //        cout <<"elr "<<i<<" "<<j<<" "<<nrot<<endl;
       }

*/

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
      "TOF_SCINT",0,"PSH1","TUBE",par,3,coo,nrm, "ONLY",0,gid,1));
      gid++;

      par[0]=18.;
      par[1]=82.8;
      par[2]=83.8;
      par[3]=82.8;
      par[4]=83.8;
      coo[2]=58.;



      dau=mother.add(new AMSgvolume(
      "TOF_SCINT",0,"PSH2","CONE",par,5,coo,nrm, "ONLY",0,gid++,1));

      par[0]=18.;
      par[1]=82.8;
      par[2]=83.4;
      par[3]=82.8;
      par[4]=83.4;
      coo[2]=-58.;



      dau=mother.add(new AMSgvolume(
      "TOF_SCINT",0,"PSH3","CONE",par,5,coo,nrm, "ONLY",0,gid++,1));





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
      "1/2ALUM",nrot++,"SHU1","TUBS",par,5,coo,nrm, "ONLY",0,gid++,1));

      par[0]=0;
      par[1]=187.;
      par[2]=1.;
      par[3]=90.;
      par[4]=270.;
      coo[0]=-189;
      coo[1]=0;
      coo[2]=0;
      dau=mother.add(new AMSgvolume(
      "1/2ALUM",nrot++,"SHU2","TUBS",par,5,coo,nrm, "ONLY",0,gid++,1));
      par[0]=0.;
      par[1]=187.;
      par[2]=1.;
      par[3]=0.;
      par[4]=360.;
      coo[0]=189;
      coo[1]=0;
      coo[2]=0;
      dau=mother.add(new AMSgvolume(
      "1/2ALUM",nrot++,"SHU3","TUBS",par,5,coo,nrm, "ONLY",0,gid++,1));


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
      "IRON",0,"IR01","BOX ",par,3,coo,nrm, "ONLY",0,gid++,1));

      coo[0]=0;      
      coo[1]=-87.;      
      coo[2]=38;      
      dau=mother.add(new AMSgvolume(
      "IRON",0,"IR02","BOX ",par,3,coo,nrm, "ONLY",0,gid++,1));

      coo[0]=0;      
      coo[1]=87.;      
      coo[2]=-38;      
      dau=mother.add(new AMSgvolume(
      "IRON",0,"IR03","BOX ",par,3,coo,nrm, "ONLY",0,gid++,1));

      coo[0]=0;      
      coo[1]=87.;      
      coo[2]=38;      
      dau=mother.add(new AMSgvolume(
      "IRON",0,"IR04","BOX ",par,3,coo,nrm, "ONLY",0,gid++,1));

      par[0]=3;
      par[1]=84;
      par[2]=4;



      coo[0]=-87;      
      coo[1]=0;      
      coo[2]=-38;      
      dau=mother.add(new AMSgvolume(
      "IRON",0,"IR05","BOX ",par,3,coo,nrm, "ONLY",0,gid++,1));

      coo[0]=-87;      
      coo[1]=0.;      
      coo[2]=38;      
      dau=mother.add(new AMSgvolume(
      "IRON",0,"IR06","BOX ",par,3,coo,nrm, "ONLY",0,gid++,1));

      coo[0]=87;      
      coo[1]=0.;      
      coo[2]=-38;      
      dau=mother.add(new AMSgvolume(
      "IRON",0,"IR07","BOX ",par,3,coo,nrm, "ONLY",0,gid++,1));

      coo[0]=87;      
      coo[1]=0;      
      coo[2]=38;      
      dau=mother.add(new AMSgvolume(
      "IRON",0,"IR08","BOX ",par,3,coo,nrm, "ONLY",0,gid++,1));

      par[0]=4;
      par[1]=3;
      par[2]=38;



      coo[0]=-87;      
      coo[1]=-87.;      
      coo[2]=0;      
      dau=mother.add(new AMSgvolume(
      "IRON",0,"IR09","BOX ",par,3,coo,nrm, "ONLY",0,gid++,1));

      coo[0]=-87;      
      coo[1]=87.;      
      coo[2]=0;      
      dau=mother.add(new AMSgvolume(
      "IRON",0,"IR10","BOX ",par,3,coo,nrm, "ONLY",0,gid++,1));

      coo[0]=87;      
      coo[1]=-87.;      
      coo[2]=0;      
      dau=mother.add(new AMSgvolume(
      "IRON",0,"IR11","BOX ",par,3,coo,nrm, "ONLY",0,gid++,1));

      coo[0]=87;      
      coo[1]=87.;      
      coo[2]=0;      
      dau=mother.add(new AMSgvolume(
      "IRON",0,"IR12","BOX ",par,3,coo,nrm, "ONLY",0,gid++,1));


       
}
void magnetgeom02(AMSgvolume & mother){
AMSID amsid;
geant par[6]={0.,0.,0.,0.,0.,0.};
char name[5]="MAGN";
geant coo[3]={0.,0.,0.};
number nrm[3][3]={1.,0.,0.,0.,1.,0.,0.,0.,1.};
integer gid=0;
AMSNode * cur;
AMSgvolume * dau;
AMSgtmed *p;
     geant magnetl=86.;
      gid=1;
      par[0]=113.2/2;
      par[1]=129.6/2;
      par[2]=magnetl/2.;
      dau=(AMSgvolume*)mother.add(new AMSgvolume(
      "MAGNET",0,name,"TUBE",par,3,coo,
       nrm, "ONLY",0,gid,1));
/*
#ifdef __G4AMS__
// add boolean for testing
      coo[0]=par[1];
      coo[1]=0;
      coo[2]=0;
      par[1]=(par[1]-par[0])/2;
      par[0]=0;
      dau->addboolean("TUBE",par,3,coo,nrm,'-');
            
      coo[0]=-coo[0];
      coo[1]=0;
      coo[2]=0;
      dau->addboolean("TUBE",par,3,coo,nrm,'+');
#endif
*/            
      gid=2;
      coo[0]=0;
      coo[1]=0;
      coo[2]=0;
      par[0]=111.5/2.;
      par[1]=112.0/2.;
      par[2]=magnetl/2.;
      mother.add(new AMSgvolume(
      "1/2ALUM",0,"ALT1","TUBE",par,3,coo,nrm, "ONLY",0,gid,1));


/*
      gid=3;
      par[0]=129.6/2.;
      par[1]=130.4/2.;
      par[2]=magnetl/2.;
      dau=mother.add(new AMSgvolume(
      "1/2ALUM",0,"ALT2","TUBE",par,3,coo,nrm, "ONLY",0,gid++,1));
      par[0]=112/2.;
      par[1]=165./2.;
      par[2]=0.75;
      coo[2]=magnetl/2.+par[2];
      dau=mother.add(new AMSgvolume(
      "1/2ALUM",0,"ALT3","TUBE",par,3,coo,nrm, "ONLY",0,gid++,1));
    
      par[0]=112/2.;
      par[1]=165.6/2.;
      par[2]=0.75;
      coo[2]=-magnetl/2.-par[2];
      dau=mother.add(new AMSgvolume(
      "1/2ALUM",0,"ALT4","TUBE",par,3,coo,nrm, "ONLY",0,gid++,1));
*/
    
}
void magnetgeom02Test(AMSgvolume & mother){
AMSID amsid;
geant par[6]={0.,0.,0.,0.,0.,0.};
char name[5]="MAGN";
geant coo[3]={0.,0.,0.};
number nrm[3][3]={1.,0.,0.,0.,1.,0.,0.,0.,1.};
integer gid=0;
AMSNode * cur;
AMSNode * dau;
AMSgtmed *p;
     geant magnetl=80.;
      gid=1;
      par[0]=113.2/2;
      par[1]=129.6/2;
      par[2]=magnetl/2.;
      dau=mother.add(new AMSgvolume(
      "MAGNET",0,name,"TUBE",par,3,coo,
       nrm, "ONLY",0,gid,1));
      gid=2;
      par[0]=111.4/2.;
      par[1]=112.0/2.;
      par[2]=magnetl/2.;
      dau=mother.add(new AMSgvolume(
      "1/2ALUM",0,"ALT1","TUBE",par,3,coo,nrm, "ONLY",0,gid,1));
      gid=3;
      par[0]=129.6/2.;
      par[1]=130.4/2.;
      par[2]=magnetl/2.;
      dau=mother.add(new AMSgvolume(
      "1/2ALUM",0,"ALT2","TUBE",par,3,coo,nrm, "ONLY",0,gid++,1));
      par[0]=112/2.;
      par[1]=165./2.;
      par[2]=0.75;
      coo[2]=magnetl/2.+par[2];
      dau=mother.add(new AMSgvolume(
      "1/2ALUM",0,"ALT3","TUBE",par,3,coo,nrm, "ONLY",0,gid++,1));
    
      par[0]=112/2.;
      par[1]=165.6/2.;
      par[2]=0.75;
      coo[2]=-magnetl/2.-par[2];
      dau=mother.add(new AMSgvolume(
      "1/2ALUM",0,"ALT4","TUBE",par,3,coo,nrm, "ONLY",0,gid++,1));
    
}
void pshgeom02(AMSgvolume & mother){
}

void tkgeom02(AMSgvolume & mother){

   TKDBc::read();





AMSID amsid;
geant xx[3]={0.9,0.2,0.2};
geant  xnrm[3][3];
geant par[6]={0.,0.,0.,0.,0.,0.};
number nrm[3][3]={1.,0.,0.,0.,1.,0.,0.,0.,1.};
number inrm[3][3];
char name[5];
geant coo[3]={0.,0.,0.};

integer gid=0;
integer nrot=501; // Temporary arbitary assignment
//static AMSgvolume mother(0,0,AMSDBc::ams_name,"BOX",par,3,coo,nrm,"ONLY",
//                         0,gid);  // temporary a dummy volume
AMSNode * cur;
AMSgvolume * dau;
int i;
for ( i=0;i<TKDBc::nlay();i++){
ostrstream ost(name,sizeof(name));
 ost << "STK"<<i+1<<ends;
 coo[0]=TKDBc::xposl(i);
 coo[1]=TKDBc::yposl(i);
 coo[2]=TKDBc::zposl(i);
 nrm[0][0]=TKDBc::nrml(0,0,i);
 nrm[0][1]=TKDBc::nrml(0,1,i); 
 nrm[0][2]=TKDBc::nrml(0,2,i); 
 nrm[1][0]=TKDBc::nrml(1,0,i); 
 nrm[1][1]=TKDBc::nrml(1,1,i); 
 nrm[1][2]=TKDBc::nrml(1,2,i); 
 nrm[2][0]=TKDBc::nrml(2,0,i); 
 nrm[2][1]=TKDBc::nrml(2,1,i); 
 nrm[2][2]=TKDBc::nrml(2,2,i); 
    int ii;
      gid=i+1;
      integer status=1;
      integer rgid;
      if(TKDBc::update())TKDBc::SetLayer(i+1,status,coo,nrm,gid);
      else               TKDBc::GetLayer(i+1,status,coo,nrm,rgid);
#ifdef __AMSDEBUG__
         if(i==2 && AMSgvolume::debug){
          MTX(xnrm,xx);
          int a1,a2;
          for( a1=0;a1<3;a1++)for( a2=0;a2<3;a2++)nrm[a2][a1]=xnrm[a1][a2];
         }
#endif
    int npar=3;
    if(npar==5){
     for ( ii=0;ii<npar;ii++)par[ii]=TKDBc::layd(i,ii);
      dau=(AMSgvolume*)mother.add(new AMSgvolume(
      "VACUUM",nrot++,name,"CONE",par,npar,coo,nrm, "ONLY",0,gid,1));
      //      cout <<" layer "<<i<<" "<<nrot<<endl;
    }
    else{
      par[0]=TKDBc::layd(i,1);
      par[1]=TKDBc::layd(i,2);
      par[2]=TKDBc::layd(i,0);
      dau=(AMSgvolume*)mother.add(new AMSgvolume(
      "VACUUM",nrot++,name,"TUBE",par,npar,coo,nrm, "ONLY",0,gid,1));
      //      cout <<" layer "<<i<<" "<<nrot<<endl;
    }
      int j;
      for (j=0;j<TKDBc::nlad(i+1);j++){
       int k;
       AMSgvolume * lad[2];
       for(k=0;k<2;k++){
         ost.seekp(0);
         ost << "L" << i+1<<(j+1)*2+k<<ends;
         if(k==0)par[0]=TKDBc::ssize_inactive(i,0)*TKDBc::nhalf(i+1,j+1)/2.;
         else par[0]=TKDBc::ssize_inactive(i,0)*
         (TKDBc::nsen(i+1,j+1)-TKDBc::nhalf(i+1,j+1))/2.;
         par[1]=TKDBc::ssize_inactive(i,1)/2;
         par[2]=TKDBc::silicon_z(i)/2;
         coo[0]=(2*k-1)*(TKDBc::ssize_inactive(i,0)*TKDBc::nsen(i+1,j+1)/2+
         TKDBc::halfldist(i)-par[0]);
         coo[1]=(TKDBc::nlad(i+1)-j)*TKDBc::c2c(i)-
         (TKDBc::nlad(i+1)+1)*TKDBc::c2c(i)/2.;
         coo[2]=TKDBc::zpos(i);
         VZERO(nrm,9*sizeof(nrm[0][0])/4);
         if(k==0){
          nrm[0][0]=1;
          nrm[1][1]=1;
          nrm[2][2]=1;
         }
         else{
          nrm[0][0]=-1;
          nrm[1][1]=-1;
          nrm[2][2]= 1;
         }
      //
      //  Ladder 
      //
        gid=i+1+10*(j+1)+100000;
        integer status=1;
        int rgid;
        if(TKDBc::update())TKDBc::SetLadder(i,j,k,status,coo,nrm,gid);
        else               TKDBc::GetLadder(i,j,k,status,coo,nrm,rgid);
        //        if(i==5){
        //          cout <<"Lad "<<j<<" "<<k<<" "<< coo[0]<<" "<<coo[1]<<" "<<coo[2]<<endl;
        //        }
        if(par[0]>0)lad[k]=(AMSgvolume*)dau->add(new AMSgvolume(
        "NONACTIVE_SILICON",nrot++,name,"BOX",par,3,coo,nrm,"ONLY",1,gid,1));
        else lad[k]=0; 
        if(lad[k]){
        ost.seekp(0);
        ost << (k==0?"ELL":"ELR") << i+1<<ends;
        par[0]=TKDBc::zelec(i,1)/2.;
        par[1]=TKDBc::c2c(i)/2.;
//        par[2]=TKDBc::zelec(i,0)/2;
        par[2]=(dau->getpar(0)- TKDBc::zelec(i,2))/2;
        coo[0]=lad[k]->getcoo(0)+(2*k-1)*(lad[k]->getpar(0)+par[0]);
        coo[1]=(TKDBc::nlad(i+1)-j)*TKDBc::c2c(i)-
        (TKDBc::nlad(i+1)+1)*TKDBc::c2c(i)/2.;
        coo[2]=TKDBc::zelec(i,2)+par[2];
        VZERO(nrm,9*sizeof(nrm[0][0])/4);
        nrm[0][0]=1;
        nrm[1][1]=1;
        nrm[2][2]=1;
        gid=i+1+10*(j+1);
        cur=dau->add(new AMSgvolume(
       "ELECTRONICS",nrot++,name,"BOX",par,3,coo,nrm,"ONLY",1,gid,1));
        }



       }

    
       // Now Sensors

       for ( k=0;k<TKDBc::nhalf(i+1,j+1);k++){
         ost.seekp(0);
         ost << "S" << i+1<<(j+1)*2<<ends;
         par[0]=TKDBc::ssize_active(i,0)/2;
         par[1]=TKDBc::ssize_active(i,1)/2;
         par[2]=TKDBc::silicon_z(i)/2;
         coo[0]=-TKDBc::ssize_inactive(i,0)/2.+
         (2*k+2-TKDBc::nhalf(i+1,j+1))*TKDBc::ssize_inactive(i,0)/2.;
         coo[1]=0;
         coo[2]=0;
         VZERO(nrm,9*sizeof(nrm[0][0])/4);
         nrm[0][0]=1;
         nrm[1][1]=1;
         nrm[2][2]=1;
#ifdef __AMSDEBUG__
         if(i==2 && AMSgvolume::debug){
          MTX(xnrm,xx);
          int a1,a2;
          for( a1=0;a1<3;a1++)for( a2=0;a2<3;a2++)nrm[a2][a1]=xnrm[a1][a2];
         }
#endif
         gid=i+1+10*(j+1)+1000*(k+1);
         if(TKDBc::activeladdshuttle(i+1,j+1,0)){  
          integer status=1;
          if(TKDBc::update())TKDBc::SetSensor(i,j,k,status,coo,nrm,gid);
          else               TKDBc::GetSensor(i,j,k,status,coo,nrm);
          if(status){
           cur=lad[0]->add(new AMSgvolume(
          "ACTIVE_SILICON",nrot++,name,"BOX",par,3,coo,nrm,"ONLY",1,gid,1));
           //          cout <<"add volume "<<name<<" "<<gid<<endl;
           //          cout <<" to "<<*(lad[0]);
          }
         }
       }

       for ( k=TKDBc::nhalf(i+1,j+1);k<TKDBc::nsen(i+1,j+1);k++){
        ost.seekp(0);
        ost << "S" << i+1<<(j+1)*2+1<<ends;
        par[0]=TKDBc::ssize_active(i,0)/2;
        par[1]=TKDBc::ssize_active(i,1)/2;
        par[2]=TKDBc::silicon_z(i)/2;
        coo[0]=-TKDBc::ssize_inactive(i,0)/2.+
        (2*(k-TKDBc::nhalf(i+1,j+1))+2+
         TKDBc::nhalf(i+1,j+1)-TKDBc::nsen(i+1,j+1))*
        TKDBc::ssize_inactive(i,0)/2.;
        coo[1]=0;
        coo[2]=0;
        VZERO(nrm,9*sizeof(nrm[0][0])/4);
        nrm[0][0]=1;
        nrm[1][1]=1;
        nrm[2][2]=1;
#ifdef __AMSDEBUG__
         if(i==2 && AMSgvolume::debug){
          MTX(xnrm,xx);
          int a1,a2;
          for( a1=0;a1<3;a1++)for( a2=0;a2<3;a2++)nrm[a2][a1]=xnrm[a1][a2];
         }
#endif
        gid=i+1+10*(j+1)+1000*(k+1);
        if(TKDBc::activeladdshuttle(i+1,j+1,1)){  
         integer status=1;
         if(TKDBc::update())TKDBc::SetSensor(i,j,k,status,coo,nrm,gid);
         else               TKDBc::GetSensor(i,j,k,status,coo,nrm);
         if(status){
          cur=lad[1]->add(new AMSgvolume(
          "ACTIVE_SILICON",nrot++,name,"BOX",par,3,coo,nrm,"ONLY",1,gid,1));
          //          cout <<"add volume "<<name<<" "<<gid<<endl;
          //          cout <<" to "<<*(lad[1]);
         }
        }
       }


      }
      // Now Support foam
 
      for ( j=0;j<TKDBc::nlad(i+1);j++){
       ost.seekp(0);
       ost << "FOA" << i+1<<ends;
       par[0]=TKDBc::ssize_inactive(i,0)*TKDBc::nsen(i+1,j+1)/2.;
       par[1]=TKDBc::c2c(i)/2;
       par[2]=(TKDBc::support_foam_w(i))/2;    
       coo[0]=0;
       coo[1]=(TKDBc::nlad(i+1)-j)*TKDBc::c2c(i)-(TKDBc::nlad(i+1)+1)*TKDBc::c2c(i)/2.;
       coo[2]=TKDBc::zpos(i)-TKDBc::silicon_z(i)/2.-par[2]-TKDBc::support_foam_tol(i);
       VZERO(nrm,9*sizeof(nrm[0][0])/4);
       nrm[0][0]=1;
       nrm[1][1]=1;
       nrm[2][2]=1;
       gid=i+1+10*(j+1);
       cur=dau->add(new AMSgvolume(
       "Tr_Foam",nrot++,name,"BOX",par,3,coo,nrm,"ONLY",1,gid,1));

  }

      // Now Honecomb Planes

       ost.seekp(0);
       ost << "PLA" << i+1<<ends;
       par[0]=0;
       par[1]=TKDBc::support_hc_r(i);
       par[2]=TKDBc::support_hc_w(i)/2;
       coo[0]=0;
       coo[1]=0;
       coo[2]=TKDBc::zpos(i)-TKDBc::silicon_z(i)/2.+TKDBc::support_hc_z(i);
       VZERO(nrm,9*sizeof(nrm[0][0])/4);
       nrm[0][0]=1;
       nrm[1][1]=1;
       nrm[2][2]=1;
       gid=i+1;
       cur=dau->add(new AMSgvolume(
      "Tr_Honeycomb",nrot++,name,"TUBE",par,3,coo,nrm,"ONLY",1,gid,1));



}


}

#include <trddbc.h>
void trdgeom02(AMSgvolume & mother){
using trdconst::maxlay;
using trdconst::maxlad;
using trdconst::maxo;
using trdconst::mtrdo;
using trdconst::maxtube;
using trdconst::TRDROTMATRIXNO;

   TRDDBc::read();

geant par[10]={0.,0.,0.,0.,0.,0.,0.,0.,0.,0.};
number nrm[3][3]={1.,0.,0.,0.,1.,0.,0.,0.,1.};
number inrm[3][3];
char name[5];
geant coo[3]={0.,0.,0.};
integer gid(0);
uinteger rgid(0);
uinteger status;
integer nrot=TRDROTMATRIXNO; 
AMSNode * cur;
AMSNode * dau;
AMSNode * oct[maxo];
 ostrstream ost(name,sizeof(name));

int i;
for ( i=0;i<TRDDBc::PrimaryOctagonNo();i++){
 ost.seekp(0);  
 ost << "TRD"<<i<<ends;
 TRDDBc::GetOctagon(i,status,coo,nrm,rgid);
 gid=i+1;
 int ip;
 for(ip=0;ip<10;ip++)par[ip]=TRDDBc::OctagonDimensions(i,ip);
       oct[i]=mother.add(new AMSgvolume(TRDDBc::OctagonMedia(i),
       nrot++,name,"PGON",par,10,coo,nrm, "ONLY",1,gid,1));
}


for ( i=TRDDBc::PrimaryOctagonNo();i<TRDDBc::OctagonNo();i++){
 ost.seekp(0);  
 ost << "TRD"<<i<<ends;
 TRDDBc::GetOctagon(i,status,coo,nrm,rgid);
 gid=i+1;
 int ip;
 int po=TRDDBc::GetPrimaryOctagon(i);
 for(ip=0;ip<10;ip++)par[ip]=TRDDBc::OctagonDimensions(i,ip);
       oct[i]=oct[po]->add(new AMSgvolume(TRDDBc::OctagonMedia(i),
       nrot++,name,"PGON",par,10,coo,nrm, "ONLY",1,gid,1));
}


// trd 
for ( i=0;i<TRDDBc::TRDOctagonNo();i++){
 int j,k;
 for(j=0;j<TRDDBc::LayersNo(i);j++){
  for(k=0;k<TRDDBc::LaddersNo(i,j);k++){
   int ip;
   gid=i+mtrdo*j+mtrdo*maxlay*k+1;
   ost.seekp(0);  
   ost << "TR"<<TRDDBc::CodeLad(gid-1)<<ends;
   TRDDBc::GetLadder(k,j,i,status,coo,nrm,rgid);
   for(ip=0;ip<3;ip++)par[ip]=TRDDBc::LaddersDimensions(i,j,k,ip);
   int itrd=TRDDBc::NoTRDOctagons(i);
//   cout <<name<<" "<<j<<" "<<k<<" "<<
//   coo[0]<<" "<<coo[1]<<" "<<coo[2]<<" "<<
//   par[0]<<" "<<par[1]<<" "<<par[2]<<endl;
   dau=oct[itrd]->add(new AMSgvolume(TRDDBc::LaddersMedia(),
       nrot++,name,"BOX",par,3,coo,nrm, "ONLY",0,gid,1));
#ifdef __G4AMS__
//   ((AMSgvolume*) dau)->Smartless()=1;
#endif
//
// Tubes & Radiators has no rotation matrix at the moment
// This can be changed in any time
//
   ost.seekp(0);  
   ost << "TRDR"<<ends;
   TRDDBc::GetRadiator(k,j,i,status,coo,nrm,rgid);
   for(ip=0;ip<3;ip++)par[ip]=TRDDBc::RadiatorDimensions(i,j,k,ip);
   gid=i+mtrdo*j+mtrdo*maxlay*k+1;
   dau->add(new AMSgvolume(TRDDBc::RadiatorMedia(),
      0,name,"BOX",par,3,coo,nrm, "ONLY",i==0 && j==0 && k==0?1:-1,gid,1));    
    
//   ost.seekp(0);  
//   ost << "TRDB"<<ends;
//   TRDDBc::GetTubeBox(k,j,i,status,coo,nrm,rgid);
//   for(ip=0;ip<10;ip++)par[ip]=TRDDBc::TubesBoxDimensions(i,j,k,ip);
//   gid=i+mtrdo*j+mtrdo*maxlay*k+1;
//   dau->add(new AMSgvolume(TRDDBc::TubesBoxMedia(),
//        0,name,"BOX",par,3,coo,nrm, "ONLY",i==0 && j==0 && k==0?1:-1,gid,1));    

   int l;
   for(l=0;l< TRDDBc::TubesNo(i,j,k);l++){
   ost.seekp(0);  
   ost << "TRDW"<<ends;
   TRDDBc::GetTube(l,k,j,i,status,coo,nrm,rgid);
   for(ip=0;ip<3;ip++)par[ip]=TRDDBc::TubesDimensions(i,j,k,ip);
   gid=i+mtrdo*j+mtrdo*maxlay*k+mtrdo*maxlay*maxlad*l+1;
   dau->add(new AMSgvolume(TRDDBc::TubesMedia(),
      0,name,"TUBE",par,3,coo,nrm, "ONLY",i==0 && j==0 && k==0 && l==0?1:-1,gid,1));    
   }

   for(l=0;l< TRDDBc::TubesNo(i,j,k);l++){
   ost.seekp(0);  
   ost << "TRDT"<<ends;
   TRDDBc::GetTube(l,k,j,i,status,coo,nrm,rgid);
   for(ip=0;ip<3;ip++)par[ip]=TRDDBc::ITubesDimensions(i,j,k,ip);
   gid=i+mtrdo*j+mtrdo*maxlay*k+mtrdo*maxlay*maxlad*l+1;
   dau->add(new AMSgvolume(TRDDBc::ITubesMedia(),
      0,name,"TUBE",par,3,coo,nrm, "ONLY",i==0 && j==0 && k==0 && l==0?1:-1,gid,1));    
   }
  }
 }
}
cout <<"amsgeom::trdgeom02-I-TRDGeometryDone"<<endl;

}

void antigeom02(AMSgvolume & mother){
#ifdef __G4AMS__
 extern void antigeom02g4(AMSgvolume &);
 if(MISCFFKEY.G4On){
   antigeom02g4(mother);
 }
 else if(MISCFFKEY.G3On){
#endif
 antigeom(mother);
#ifdef __G4AMS__
}
#endif
}

#ifdef __G4AMS__
//---------------------------------------------------------------
void antigeom02g4(AMSgvolume & mother){
AMSID amsid;
geant par[6]={0.,0.,0.,0.,0.,0.};
number nrm[3][3]={1.,0.,0.,0.,1.,0.,0.,0.,1.};
number nrd[3][3];
geant coo[3]={0.,0.,0.};
integer i,nrot,gid=0,gidd=0;
geant scradi,scinth,scleng,wrapth,groovr,pdlgap,stradi,stleng,stthic;
geant rs,phi,phib,dphis,dphi,dphig,phigr;
geant degrad,raddeg;
integer nscpad;
AMSNode * pAmother;
AMSgvolume * pSegm;
AMSNode * pGroov;
AMSNode * p;
AMSgvolume *dummy;
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
       "ANTI_SUPTB",0,"ASTB","TUBS",par,5,coo,nrm, "ONLY",0,gid,1));
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
       "ANTI_WRAP",0,"AMOT","TUBS",par,5,coo,nrm, "ONLY",0,gid,1));
//
// ---> Loop to fill A-mother volume with (sc.segment+bumps-groove) :
//
  gid=0;
  for(i=0;i<nscpad;i++){
    phi=i*dphi;
    phib=phi+dphis;
    phigr=phi-dphig;
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
    pSegm=(AMSgvolume *)pAmother->add(new AMSgvolume(
       "ANTI_SCINT",0,"ANTS","TUBS",par,5,coo,nrm,"BOOL",1,gid,1));
//
//     Subtr. groove from Segm:
//
    coo[0]=rs*cos(phigr*degrad);
    coo[1]=rs*sin(phigr*degrad);
    coo[2]=0.;
    par[0]=0.;
    par[1]=groovr;
    par[2]=0.5*scleng;
    pSegm->addboolean("TUBE",par,3,coo,nrm,'-');
//
//     add sc. bump to Segm:
//
    coo[0]=rs*cos(phib*degrad);
    coo[1]=rs*sin(phib*degrad);
    coo[2]=0.;
    par[0]=0.;
    par[1]=groovr-pdlgap;
    par[2]=0.5*scleng;
    pSegm->addboolean("TUBE",par,3,coo,nrm,'+');
  }// ---> end of sector loop
//
   cout<<"ANTI-geometry(G4) done !.."<<endl;
}
#endif
//---------------------------------------------------------------------
void ecalgeom02(AMSgvolume & mother){
//
  geant par[6]={0.,0.,0.,0.,0.,0.};
  number nrm[3][3];
  number nrm0[3][3]={1.,0.,0.,0.,1.,0.,0.,0.,1.};
  number nrm1[3][3]={1.,0.,0., 0.,0.,1., 0.,-1.,0.};// for X-proj fibers wrt ECMO
  number nrm2[3][3]={0.,0.,1.,0.,1.,0., -1.,0.,0.}; // for Y-proj fibers
  geant coo[3]={0.,0.,0.};
  geant dx1,dy1,dx2,dy2,dz,dzh,xpos,ypos,zpos,cleft,fpitx,fpitz,fpitzz,zfl1;
  geant flen,zposfl,dxe;
  integer nrot,gid(0),nsupl,nflpsl,nfpl[2],nf;
  integer isupl,ifibl,ifib,iproj,ip;
  int i,j,k;
  integer vlist[10]={0,0,0,0,0,0,0,0,0,0};
  char inum[11];
  char in[2]="0";
  char vname[5];
  AMSNode * pECrad;
  AMSNode * pECfbl;
  AMSNode * pECfib;
  AMSNode * pECfsen;
  AMSNode * ECmother;
  AMSNode * p;
//
  strcpy(inum,"0123456789");
  dx1=ECALDBc::gendim(1);// x-size of EC-radiator
  dxe=ECALDBc::gendim(4);// dx(dy)-thickness of (PMT+electronics)-volume 
  dy1=ECALDBc::gendim(2);// y-size of EC-rad
  dz=ECALDBc::gendim(3);// Z-size of EC-rad
  dzh=ECALDBc::gendim(8);// Z-thickness of honeycomb
  xpos=ECALDBc::gendim(5);// x-pos EC-radiator center
  ypos=ECALDBc::gendim(6);// y-pos
  zpos=ECALDBc::gendim(7)-dz/2.;// z-pos of ECAL-center
  nsupl=ECALDBc::slstruc(3);// numb.of super-layers
  nflpsl=ECALDBc::slstruc(2);// numb.of fiber-layers per super-layerr
  nfpl[0]=ECALDBc::nfibpl(1);// fiber/(1st layer) 
  nfpl[1]=ECALDBc::nfibpl(2);// fiber/(2nd layer)
  fpitx=ECALDBc::fpitch(1);
  fpitz=ECALDBc::fpitch(2);
  fpitzz=ECALDBc::fpitch(3);
  zfl1=(nsupl*(nflpsl-1)*fpitz+(nsupl-1)*fpitzz)/2.;//1st f-layer zpos in ECAL-radiator(Mother) r.s.
//------------------------------------
  par[0]=dx1/2.+dxe;
  par[1]=dy1/2.+dxe;
  par[2]=dz/2.+dzh;
  coo[0]=xpos;
  coo[1]=ypos;
  coo[2]=zpos;
  gid=1;
  ECmother=mother.add(new AMSgvolume(
       "VACUUM",0,"ECMO","BOX",par,3,coo,nrm0,"ONLY",0,gid,1));// create ECAL mother volume
//------------------------------------
//  create top/bot honeycomb plate in ECAL-mother:
//
  par[0]=dx1/2.;
  par[1]=dy1/2.;
  par[2]=dzh/2.;
  coo[0]=0.;
  coo[1]=0.;
  coo[2]=0.+dz/2.+dzh/2.;
  gid=1;
  p=ECmother->add(new AMSgvolume(
       "EC_HONEYC",0,"ECHN","BOX",par,3,coo,nrm0,"ONLY",1,gid,1));// top honeycomb plate
  coo[2]=0.-dz/2.-dzh/2.;
  gid=2;
  p=ECmother->add(new AMSgvolume(
       "EC_HONEYC",0,"ECHN","BOX",par,3,coo,nrm0,"ONLY",1,gid,1));// bot honeycomb plate
//------------------------------------
//
// create EC-electronics boxes in ECAL-mother:
//
  coo[2]=0.;
  par[2]=dz/2.;
  par[0]=dxe/2.;
  par[1]=dy1/2.;
  coo[0]=-dx1/2.-dxe/2.;
  coo[1]=0.;
  gid=1;
  p=ECmother->add(new AMSgvolume(
       "EC_ELBOX",0,"ECEB","BOX",par,3,coo,nrm0,"ONLY",1,gid,1));//-X box
  coo[0]=dx1/2.+dxe/2.;
  gid=2;
  p=ECmother->add(new AMSgvolume(
       "EC_ELBOX",0,"ECEB","BOX",par,3,coo,nrm0,"ONLY",1,gid,1));//+X box
//
  par[0]=dx1/2.;
  par[1]=dxe/2.;
  coo[0]=0.;
  coo[1]=-dy1/2.-dxe/2.;
  gid=3;
  p=ECmother->add(new AMSgvolume(
       "EC_ELBOX",0,"ECEB","BOX",par,3,coo,nrm0,"ONLY",1,gid,1));//-Y box
  coo[1]=dy1/2.+dxe/2.;
  gid=4;
  p=ECmother->add(new AMSgvolume(
       "EC_ELBOX",0,"ECEB","BOX",par,3,coo,nrm0,"ONLY",1,gid,1));//+Y box
//------------------------------------
  if(ECMCFFKEY.fastsim==1){ // <--- simplified (fast) geometry
//
// create Ecal EFFECTIVE Radiator volume as box:
//
    par[0]=dx1/2.;
    par[1]=dy1/2.;
    par[2]=dz/2.;
    coo[0]=0.;
    coo[1]=0.;
    coo[2]=0.;
    gid=100;
    pECrad=ECmother->add(new AMSgvolume(
       "EC_EFFRAD",0,"ECER","BOX",par,3,coo,nrm0,"ONLY",0,gid,1));
  }
//----------------------------------------------------------
  else{                     // <--- accurate (slow) geometry
//---> create/put Radiator
    par[0]=dx1/2.;
    par[1]=dy1/2.;
    par[2]=dz/2.;
    coo[0]=0.;
    coo[1]=0.;
    coo[2]=0.;
    gid=100;
    pECrad=ECmother->add(new AMSgvolume(
       "EC_RADIATOR",0,"ECRD","BOX",par,3,coo,nrm0,"ONLY",0,gid,1));//cr. EC_rad in ECmother
//
//---> create/put fiber-layer(s) and individual fibers in EC_rad:
//
    zposfl=zfl1;// init  pos. of 1st fiber-layer
//
    for(isupl=0;isupl<nsupl;isupl++){ // <--- super-layers loop
      ip=isupl%2;
      if(ip==0)iproj=ECALDBc::slstruc(1);// iproj=0 ->X, =1 ->Y
      if(ip==1)iproj=1-ECALDBc::slstruc(1);
      if(iproj==0){ // in X-proj fiber goes along Y-axis
        flen=dy1/2.;
        for(i=0;i<3;i++)for(j=0;j<3;j++)nrm[i][j]=nrm1[i][j];
      }
      else{         // in Y-proj fiber goes along X-axis
        flen=dx1/2.;
        for(i=0;i<3;i++)for(j=0;j<3;j++)nrm[i][j]=nrm2[i][j];
      }
//------
      for(ifibl=0;ifibl<nflpsl;ifibl++){ // <--- fiber-layers loop in s-layer
        nrot=ECROTN+isupl*nflpsl+ifibl;
        ip=ifibl%2;//even(1)/odd(0) f-layer
	nf=nfpl[ip];// total fibers in layer
	cleft=-(nf-1)*fpitx/2.;// tempor imply nfpl[1]=nfpl[0]+-1 sceme !!!
	if(iproj==0){ // X
          par[0]=dx1/2.;
          par[1]=ECALDBc::rdcell(4)/2.+ECALDBc::rdcell(8);// fiber+glue radious
          par[2]=dy1/2.;
	}
	else{ // Y
          par[0]=ECALDBc::rdcell(4)/2.+ECALDBc::rdcell(8);// fiber+glue radious
          par[1]=dy1/2.;
          par[2]=dx1/2.;
	} 
	coo[0]=0.;
	coo[1]=0.;
	coo[2]=zposfl;
	gid=(ifibl+1)+(isupl+1)*100;
        strcpy(vname,"FL");
	k=isupl*nflpsl+ifibl+1;//solid numbering of f-layers(2-digits)
	i=k/10;
	in[0]=inum[i];
	strcat(vname,in);
	i=k%10;
	in[0]=inum[i];
	strcat(vname,in);
        pECfbl=pECrad->add(new AMSgvolume(
               "EC_RADIATOR",nrot,vname,"BOX",par,3,coo,nrm,"ONLY",0,gid,1));//cr. f-layer in ECrad
//-----------
        for(ifib=0;ifib<nf;ifib++){ // <--- fiber loop in layer
          par[0]=0.;
          par[1]=ECALDBc::rdcell(4)/2.+ECALDBc::rdcell(8);// fiber radious(+glue)
	  par[2]=flen;
          if(iproj==0){
	    coo[0]=cleft+ifib*fpitx;
	    coo[1]=0.;
          }
          else{
  	    coo[0]=0.;
   	    coo[1]=cleft+ifib*fpitx;
          }
	  coo[2]=0.;
	  gid=(ifib+1)+(ifibl+1)*1000+(isupl+1)*100000;
          pECfib=pECfbl->add(new AMSgvolume(
          "EC_FWALL",0,"ECFW","TUBE",par,3,coo,nrm0,"ONLY",isupl==0 && ifibl==0 && ifib==0?1:-1,gid,1));
//
          if(isupl==0 && ifibl==0 && ifib==0){
            par[1]=ECALDBc::rdcell(4)/2.;// fiber-core radious
	    coo[0]=0.;
	    coo[1]=0.;
	    gid=1;
	    pECfsen=pECfib->add(new AMSgvolume(
            "EC_FCORE",0,"ECFC","TUBE",par,3,coo,nrm0,"ONLY",0,gid,1));
	  }	 
	} // ---> end of fiber loop
//-----------
        zposfl-=fpitz;// f-layer z-shifts inside of super-layer 
      } // ---> end of layer loop
//------
      zposfl+=fpitz;// remove extra z-shift
      zposfl-=fpitzz;// add zz-shifts
    } // ---> end of superlayer loop
  }
//
  cout<<"AMSGEOM: ECAL-geometry done!"<<endl;
}
//------------------------------------------------------------
#include <srddbc.h>
void srdgeom02(AMSgvolume & mother){
using srdconst::maxo;
using srdconst::SRDROTMATRIXNO;
   SRDDBc::read();

geant par[10]={0.,0.,0.,0.,0.,0.,0.,0.,0.,0.};
number nrm[3][3]={1.,0.,0.,0.,1.,0.,0.,0.,1.};
number inrm[3][3];
char name[5];
geant coo[3]={0.,0.,0.};
integer gid(0);
uinteger rgid(0);
uinteger status;
integer nrot=SRDROTMATRIXNO; 
AMSNode * cur;
AMSNode * dau;
AMSNode * oct[maxo];
 ostrstream ost(name,sizeof(name));
int i;
for ( i=0;i<SRDDBc::VolumesNo();i++){
 ost.seekp(0);  
 ost << "SRD"<<i<<ends;
 SRDDBc::GetVolume(i,status,coo,nrm,rgid);
 gid=i+1;
 int ip;
 for(ip=0;ip<3;ip++)par[ip]=SRDDBc::VolumeDimensions(i,ip);
       oct[i]=mother.add(new AMSgvolume(SRDDBc::VolumeMedia(i),
       nrot++,name,"BOX",par,3,coo,nrm, "ONLY",1,gid,1));
}

for(i=0;i<SRDDBc::XtallColNo();i++){
 for(int j=0;j<SRDDBc::XtallRowNo();j++){
   ost.seekp(0);  
   ost << "SRDX"<<ends;
   SRDDBc::GetXtall(i,j,status,coo,nrm,rgid);
   for(int ip=0;ip<3;ip++)par[ip]=SRDDBc::XtallDimensions(i,j,ip);
   gid=j+SRDDBc::XtallRowNo()*i+1;
   oct[0]->add(new AMSgvolume(SRDDBc::XtallMedia(),
      0,name,"BOX",par,3,coo,nrm, "ONLY",i==0 && j==0?1:-1,gid,1));    
 }
}


for(i=0;i<SRDDBc::PMTColNo();i++){
 for(int j=0;j<SRDDBc::PMTRowNo();j++){
   ost.seekp(0);  
   ost << "SRDP"<<ends;
   SRDDBc::GetPMT(i,j,status,coo,nrm,rgid);
   for(int ip=0;ip<3;ip++)par[ip]=SRDDBc::PMTDimensions(i,j,ip);
   gid=j+SRDDBc::PMTRowNo()*i+1;
//   cout <<i<<" "<<j<<" "<<coo[0]<<" "<<coo[1]<<" "<<coo[2]<<
//   " "<<par[0]<<" "<<par[1]<<" "<<par[2]<<endl;
   oct[2]->add(new AMSgvolume(SRDDBc::PMTMedia(),
      0,name,"TUBE",par,3,coo,nrm, "ONLY",i==0 && j==0?1:-1,gid,1));    
 }
}
cout <<"amsgeom::srdgeom02-I-SRDGeometryDone"<<endl;

}

#ifdef __G4AMS__
void testboolgeom(AMSgvolume &mother){
  AMSgvolume *dummy;
  geant par[11],coo[3];
  number nrm[3][3]={1.,0.,0.,0.,1.,0.,0.,0.,1.}; // {vx, vy, vz}
  number nrm1[3][3]={0,0,1,1,0,0,0,1,0};
  integer gid=1,  
          rel=1, 
          posp=0;
  par[0]=0;
  par[1]=90;
  par[2]=31;
  coo[0]=0;
  coo[1]=0;
  coo[2]=-103.66;
  dummy=dynamic_cast<AMSgvolume*>(mother.add(new AMSgvolume("IRON",
				0,
				"RICH",
				"TUBE",
				par,
				3,
				coo,
				nrm,
//				"ONLY",
				"BOOL",
				posp,
				gid,
				rel)));
				
  //cyl
  par[0]=0;
  par[1]=10;
  par[2]=31;
  coo[0]=0;
  coo[1]=0;
  coo[2]=0;

                 dummy->addboolean("TUBE",par,3,coo,nrm,'-');
/*
  //box
  par[0]=10;
  par[1]=2;
  par[2]=40;
  coo[0]=-110;
  coo[1]=0;
  coo[2]=0;
                 dummy->addboolean(new AMSgvolume("BOX",par,3,coo,nrm1,'+');
  //cyl
  par[0]=0;
  par[1]=10;
  par[2]=31;
  coo[0]=90;
  coo[1]=0;
  coo[2]=0;

                 dummy->addboolean(new AMSgvolume("TUBE",par,3,coo,nrm,'-');

//two spheres;
par[0]=0;
par[1]=10;
par[2]=0;
par[3]=180;
par[4]=0;
par[5]=360;
  coo[0]=-95;
  coo[1]=-10;
  coo[2]=-11;
                 dummy->addboolean("SPHE",par,6,coo,nrm,'+');
  coo[0]=-95;
  coo[1]=+10;
  coo[2]=-11;
                 dummy->addboolean("SPHE",par,6,coo,nrm,'+');
*/
}

#endif


void richgeom02(AMSgvolume & mother)
{
  // New Rich Geometry by Carlos Delgado (CIEMAT)

  AMSNode *rich;
  AMSNode *dummy;
  geant par[11],coo[3];
  number nrm[3][3]={1.,0.,0.,0.,1.,0.,0.,0.,1.}; // {vx, vy, vz}
  number nrma[3][3]={0,-1,0,1,0,0,0,0,1}; // Rotated 90 degrees
  integer gid=1,  
          rel=1, 
          posp=0,
          nrot=30001; // Numbre of the 90 degrees rotation

  
  // Define the RICH volume


  par[0]=0;
  par[1]=90;
  par[2]=31;
  coo[0]=0;
  coo[1]=0;
  coo[2]=-103.66;

  rich=mother.add(new AMSgvolume("VACUUM",
				0,
				"RICH",
				"TUBE",
				par,
				3,
				coo,
				nrm,
				"ONLY",
				posp,
				gid,
				rel));
				
  // Inside RICH put all the elements

  // Radiator

  par[0]=0;
  par[1]=63.6;
  par[2]=1;

  coo[2]=30;  
  
  dummy=rich->add(new AMSgvolume("RICH RAD", // Material: Aerogel in the future
				0,          // No rotation
				"RAD ",     // Name 
				"TUBE",     // Shape
				par,        // Geant parameters
				3,          // # of parameters
				coo,        // coordinates 
				nrm,        // Matrix of normals
				"ONLY",    
				posp,
				gid,
				rel));
  
  // Lateral walls

  par[0]=25;
  par[1]=80;
  par[2]=80+.5;
  par[3]=63.6; 
  par[4]=63.6+.5;

  coo[2]=4;

  dummy=rich->add(new AMSgvolume("RICH MIRRORS",  // Material
				0,         // No rotation
				"OMIR",     // Name 
				"CONE",    // Shape
				par,       // Geant parameters
				5,         // # of parameters
				coo,       // coordinates 
				nrm,       // Matrix of normals
				"ONLY",    
				posp,
				gid,
				rel));
  
  // Inner mirror... maybe it won't exist in the future
  
  

  par[0]=25;  
  par[3]=0;   
  par[4]=0.5;     
  par[1]=40-.5;
  par[2]=40;   

  dummy=rich->add(new AMSgvolume("RICH MIRRORS",  // Material
				 0,         // No rotation
				 "IMIR",     // Name 
				 "CONE",    // Shape
				 par,       // Geant parameters
				 5,         // # of parameters
				 coo,       // coordinates 
				 nrm,       // Matrix of normals
				 "ONLY",    
				 posp,
				 gid,
				 rel));
  

  /*************************************************/

  // The PMT array geometry... first version


  // Here we define one PMTBOX and we fill it



  // It is necessary to add the electronics and the 
  // to group the PMTs in 2x4 arrays.


  geant xedge=1.5,yedge=1.5,lg,cl;
  integer copia=1,paredes=1,espejosh=1,espejosv=1;
  posp=1;
  AMSNode *p;

#define SQR(x) ((x)*(x))


  do{
    lg=SQR(xedge+1.5)+SQR(yedge+1.5);
    cl=SQR(xedge-1.5)+SQR(yedge-1.5);

    if(lg>SQR(40) && cl<SQR(80)) // Put a PMT here
      {
	coo[0]=xedge;
	coo[1]=yedge;
	coo[2]=-31+5;
	par[0]=1.5;
	par[1]=1.5;
	par[2]=5;
	
	// Put the box

	p=rich->add(new AMSgvolume("VACUUM",
				  0,
				  "PMTB",   // Defined and filled above
				  "BOX",
				  par,
				  3,
				  coo,
				  nrm,      
				  "ONLY",    
				  posp,
				  copia++,
				  rel));
	// Fill the box:

	if(copia==2) // Only once.
	  {
	    
	    // SHIELDING: We use the TOF_PMT_BOX material... to be changed in
	    // the future

	    
	    par[0]=1.5;
	    par[1]=0.05;
	    par[2]=5;
	    coo[0]=0;
	    coo[1]=1.5-0.05;
	    coo[2]=0;
	    
	    dummy=p->add(new AMSgvolume("RICH SHIELD",
				       0,
				       "SHI1",
				       "BOX",
				       par,
				       3,
				       coo,
				       nrm,
				       "ONLY",
				       posp,
				       1,
				       rel));
	    
	    coo[1]*=-1;
	    
	    dummy=p->add(new AMSgvolume("RICH SHIELD",
				       0,
				       "SHI1",
				       "BOX",
				       par,
				       3,
				       coo,
				       nrm,
				       "ONLY",
				       posp,
				       2,
				       rel));
	    
	    par[0]=.05;
	    par[1]=1.5-.1;
	    coo[0]=1.5-.05;
	    coo[1]=0;
	    
	    dummy=p->add(new AMSgvolume("RICH SHIELD",
				       0,
				       "SHI2",
				       "BOX",
				       par,
				       3,
				       coo,
				       nrm,
				       "ONLY",
				       posp,
				       1,
				       rel));
	    
	    coo[0]*=-1;

	    dummy=p->add(new AMSgvolume("RICH SHIELD",
				       0,
				       "SHI2",
				       "BOX",
				       par,
				       3,
				       coo,
				       nrm,
				       "ONLY",
				       posp,
				       2,
				       rel));
	       
			 
	    // Photocatode: 

	    par[0]=0.875;
	    par[1]=0.875;
	    par[2]=0.05;

	    coo[0]=0;
	    coo[1]=0;
	    coo[2]=5-3-.05;
	    
	    dummy=p->add(new AMSgvolume("RICH PMTS",
					0,
					"CATO",
					"BOX",
					par,
					3,
					coo,
					nrm,
					"ONLY",
					posp,
					1,
					rel));
	    

	    // Glue

	    par[0]=1.5-.1;
	    par[1]=1.5-.1;
	    par[2]=(4.5-.1)/2;

	    coo[0]=0;
	    coo[1]=0;
	    coo[2]=5-3-.1-par[2];

	    dummy=p->add(new AMSgvolume("RICH GLUE",
					0,
					"GLUE",
					"BOX",
					par,
					3,
					coo,
					nrm,
					"ONLY",
					posp,
					1,
					rel));

	    


	    // ELECTRONICS: to be added in the future


	    // Light guides: It is quite complicated, so it uses 
	    // the "MANY" flag... I will change this in the future


	    AMSNode *lg;

	    par[0]=1.5-.1;
	    par[1]=1.5-.1;
	    par[2]=1.5;

	    coo[0]=0;
	    coo[1]=0;
	    coo[2]=5-1.5;

	    lg=p->add(new AMSgvolume("VACUUM",
				     0,
				     "LGBO",
				     "BOX",
				     par,
				     3,
				     coo,
				     nrm,
				     "ONLY",
				     posp,
				     1,
				     rel));




	    // 5 mirrors

	    par[0]=1.5;
	    par[1]=9.926245;
	    par[2]=90;
	    par[3]=0.025;
	    par[4]=0.875;
	    par[5]=0.875;
	    par[6]=0;
	    par[7]=0.025;
	    par[8]=1.5-0.1;
	    par[9]=1.5-0.1;
	    par[10]=0;

	    coo[0]=0;
	    coo[1]=1.1125;
	    coo[2]=0;

	    dummy=lg->add(new AMSgvolume("RICH MIRRORS",
					 0,
					 "MIRA",
					 "TRAP",
					 par,
					 11,
					 coo,
					 nrm,
					 "MANY",
					 posp,
					 1,
					 rel));
	    
	    
//	    cout << "RICH: LG1 finished" <<endl;
	    

	    
	    par[0]=1.5;
	    par[1]=5.000645;
	    par[2]=90;
	    par[3]=0.025;
	    par[4]=0.875;
	    par[5]=0.875;
	    par[6]=0;
	    par[7]=0.025;
	    par[8]=1.5-0.1;
	    par[9]=1.5-0.1;
	    par[10]=0;

	    coo[0]=0;
	    coo[1]=.54375;
	    coo[2]=0;



	    dummy=lg->add(new AMSgvolume("RICH MIRRORS",
					 0,
					 "MIRB",
					 "TRAP",
					 par,
					 11,
					 coo,
					 nrm,  // Rotated 90 degrees
					 "MANY",
					 posp,
					 1,
					 rel));
	    
//	    cout << "RICH: LG2 finished" <<endl;  
	    

	    par[0]=1.5;
	    par[1]=0;
	    par[2]=90;
	    par[3]=0.025;
	    par[4]=0.875;
	    par[5]=0.875;
	    par[6]=0;
	    par[7]=0.025;
 	    par[8]=1.5-0.1;
	    par[9]=1.5-0.1;
	    par[10]=0;

	    coo[0]=0;
	    coo[1]=0;
	    coo[2]=0;



	    dummy=lg->add(new AMSgvolume("RICH MIRRORS",
					 0,
					 "MIRC",
					 "TRAP",
					 par,
					 11,
					 coo,
					 nrm,  // Rotated 90 degrees
					 "MANY",
					 posp,
					 1,
					 rel));
	    
//	    cout << "RICH: LG3 finished" <<endl;


	    par[0]=1.5;
	    par[1]=9.926245;
	    par[2]=270;
	    par[3]=0.025;
	    par[4]=0.875;
	    par[5]=0.875;
	    par[6]=0;
	    par[7]=0.025;
	    par[8]=1.5-0.1;
	    par[9]=1.5-0.1;
	    par[10]=0;

	    coo[0]=0;
	    coo[1]=-1.1125;
	    coo[2]=0;

	    dummy=lg->add(new AMSgvolume("RICH MIRRORS",
					 0,
					 "MIRD",
					 "TRAP",
					 par,
					 11,
					 coo,
					 nrm,
					 "MANY",
					 posp,
					 1,
					 rel));
	    

//	    cout << "RICH: LG4 finished" <<endl;


	    
	    par[0]=1.5;
	    par[1]=5.000645;
	    par[2]=270;
	    par[3]=0.025;
	    par[4]=0.875;
	    par[5]=0.875;
	    par[6]=0;
	    par[7]=0.025;
	    par[8]=1.5-0.1;
	    par[9]=1.5-0.1;
	    par[10]=0;

	    coo[0]=0;
	    coo[1]=-.54375;
	    coo[2]=0;



	    dummy=lg->add(new AMSgvolume("RICH MIRRORS",
					 0,
					 "MIRE",
					 "TRAP",
					 par,
					 11,
					 coo,
					 nrm,  // Rotated 90 degrees
					 "MANY",
					 posp,
					 1,
					 rel));
	    
//	    cout << "RICH: LG5 finished" <<endl;
	    


	    // And now put the other 5 copies

	   
	    par[0]=1.5-.1;
	    par[1]=1.5-.1;
	    par[2]=1.5;

	    coo[0]=0;
	    coo[1]=0;
	    coo[2]=5-1.5;

	    dummy=p->add(new AMSgvolume("VACUUM",
					nrot,
					"LGBO",
					"BOX",
					par,
					3,
					coo,
					nrma,
					"ONLY",
					posp,
					2,
					rel)); 



//	    cout<< "RICH: LG finished" << endl;

	    
	  }
      
	// Here we add the other 3 parts
	
	coo[0]=-xedge;
	coo[1]=yedge;
	coo[2]=-31+5;
	par[0]=1.5;
	par[1]=1.5;
	par[2]=5;
	
	// Put the box
	
	p=rich->add(new AMSgvolume("VACUUM",
				   0,
				   "PMTB",   // Defined and filled above
				   "BOX",
				   par,
				   3,
				   coo,
				   nrm,      
				   "ONLY",    
				   posp,
				   copia++,
				   rel));
	
	
	
	coo[0]=xedge;
	coo[1]=-yedge;
	coo[2]=-31+5;
	par[0]=1.5;
	par[1]=1.5;
	par[2]=5;
	
	// Put the box

	p=rich->add(new AMSgvolume("VACUUM",
				  0,
				  "PMTB",   // Defined and filled above
				  "BOX",
				  par,
				  3,
				  coo,
				  nrm,      
				  "ONLY",    
				  posp,
				  copia++,
				  rel));	
	    
	coo[0]=-xedge;
	coo[1]=-yedge;
	coo[2]=-31+5;
	par[0]=1.5;
	par[1]=1.5;
	par[2]=5;
	
	// Put the box

	p=rich->add(new AMSgvolume("VACUUM",
				  0,
				  "PMTB",   // Defined and filled above
				  "BOX",
				  par,
				  3,
				  coo,
				  nrm,      
				  "ONLY",    
				  posp,
				  copia++,
				  rel));      


      }
    
    xedge+=3.;
    
    if(xedge>80) {xedge=1.5;yedge+=3;}
    
  }while(yedge<80);

  cout<< "RICH geometry finished" << endl;

}  




