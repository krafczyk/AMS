//  $Id: amsgeom.C,v 1.143 2003/01/15 19:37:24 choumilo Exp $
// Author V. Choutko 24-may-1996
// TOF Geometry E. Choumilov 22-jul-1996 
// ANTI Geometry E. Choumilov 2-06-1997 
// ECAL 1.0-version Geometry E. Choumilov 15-09-1999 
// Passive Shield Geometry V. Choutko 22-jul-1996
// CTC (Cherenkov Thresh. Counter) geometry E.Choumilov 26-sep-1996
// ATC (Aerogel Threshold Cerenkov) geometry A.K.Gougas 14-Mar-1997
// AMS02 MAGNET geometry v1.0  E.Choumilov 17.11.2000
// AMS02 Radiators/crates/TRD-TOF_supports/USS geometry v1.0  E.Choumilov 4.06.2001
// 
//
#include <typedefs.h>
#include <node.h>
#include <snode.h>
#include <amsdbc.h>
#include <tofdbc02.h>
#include <antidbc02.h>
#include <ecaldbc.h>
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
namespace amsgeom{
extern void tkgeom02(AMSgvolume &);
extern void magnetgeom(AMSgvolume &);
extern void magnetgeom02(AMSgvolume &);
extern void ext1structure02(AMSgvolume &);
extern void magnetgeom02o(AMSgvolume &);
extern void magnetgeom02Test(AMSgvolume &);
extern void tofgeom02(AMSgvolume &);
extern void antigeom02(AMSgvolume &);
extern void antigeom002(AMSgvolume &);
extern void ext2structure(AMSgvolume &);
#ifdef __G4AMS__
extern void antigeom02g4(AMSgvolume &);
extern void testg4geom(AMSgvolume &);
#endif
extern void richgeom02(AMSgvolume &);
extern void ecalgeom02(AMSgvolume &);
extern void trdgeom02(AMSgvolume &);
//extern void srdgeom02(AMSgvolume &);
extern void Put_rad(AMSgvolume *,integer);
extern void Put_pmt(AMSgvolume *,integer);
};
using namespace amsgeom;
//--------------------------------------------------
void AMSgvolume::amsgeom(){
AMSID amsid;
geant par[10];
geant parf[10];
geant coo[3]={0,0,0};
number nrm[3][3]={1.,0.,0.,0.,1.,0.,0.,0.,1.};
integer gid=1;
number amss[3];
if (strstr(AMSJob::gethead()->getsetup(),"BIG")){
 cout<<"AMSGeom-I-BIGSetupSelected"<<endl;

 for(int i=0;i<3;i++)amss[i]=AMSDBc::ams_size[i]*10;

/*
// should not rotate ams due to ecal bug
//but rotate the solar panel instead

 number angle=-10./180*AMSDBc::pi;
 AMSDBc::ams_nrm[0][0]=cos(angle);
 AMSDBc::ams_nrm[1][0]=0;
 AMSDBc::ams_nrm[2][0]=-sin(angle);
 AMSDBc::ams_nrm[0][1]=0;
 AMSDBc::ams_nrm[1][1]=1;
 AMSDBc::ams_nrm[2][1]=0;
 AMSDBc::ams_nrm[0][2]=sin(angle);
 AMSDBc::ams_nrm[1][2]=0;
 AMSDBc::ams_nrm[2][2]=cos(angle);
*/


}
else{
 for(int i=0;i<3;i++)amss[i]=AMSDBc::ams_size[i]*sqrt(2.);
}
//
parf[0]=amss[0]/2;
parf[1]=amss[1]/2;
parf[2]=amss[2]/2;
par[0]=AMSDBc::ams_size[0]/2;
par[1]=AMSDBc::ams_size[1]/2;
par[2]=AMSDBc::ams_size[2]/2;
// to be able rotate ams as a whole
//
//----> create mother volumes:
//
static AMSgvolume false_mother("VACUUM",0,AMSDBc::ams_name,"BOX ",parf,3,coo,nrm,"ONLY",0,gid,1);  // AMS false mother volume
char optn[5];
  if(strstr(AMSJob::gethead()->getsetup(),"BIG"))strcpy(optn,"MANY");
  else strcpy(optn,"ONLY");
static AMSgvolume mother("VACUUM",AMSDBc::ams_rotmno,"FMOT","BOX",par,3,AMSDBc::ams_coo,AMSDBc::ams_nrm,optn,0,gid,1);  // AMS mother volume
AMSJob::gethead()->addup( &false_mother);
false_mother.add(&mother);
//
//----> add solar pannels and part of ISS-base around AMS for "BIG" setup:
//
if(strstr(AMSJob::gethead()->getsetup(),"BIG")){
  number nrm1[3][3];
  integer nrot(1);
  number angle=10./180*AMSDBc::pi;
  nrm1[0][0]=cos(angle);
  nrm1[1][0]=0;
  nrm1[2][0]=-sin(angle);
  nrm1[0][1]=0;
  nrm1[1][1]=1;
  nrm1[2][1]=0;
  nrm1[0][2]=sin(angle);
  nrm1[1][2]=0;
  nrm1[2][2]=cos(angle);
//
// Add Solar Panel  // alpha=beta=90 
//
  geant coosp[3]={500.,0.,0.};
  geant parsp[3]={0.5,300.,1700.};
  AMSgvolume *span = new AMSgvolume("VACUUM",nrot,"SPAN","BOX ",parsp,3,coosp,nrm1,"ONLY",0,1,1);
  false_mother.add(span);

  geant coos[3]={0.,0.,100.};
  geant pars[3]={0.5,300.,800.};
  coos[2]+=pars[2];
  AMSgvolume *solar1 = new AMSgvolume("1/2ALUM",0,"SPA1","BOX ",pars,3,coos,nrm,"ONLY",0,1,1);
  coos[2]*=-1;
  AMSgvolume *solar2 = new AMSgvolume("1/2ALUM",0,"SPA2","BOX ",pars,3,coos,nrm,"ONLY",0,1,1);
  span->add(solar1);
  span->add(solar2);
//
// -----> add ISS payload place around AMS as polygon(6)(ISS "tube"):
//
  geant issang=12./180.*AMSDBc::pi;// AMS slope along ISS "tube" axis
  geant issdfi=360./6;//PGON azimuthal phi-division
  geant isssin=sin(issang);
  geant isscos=cos(issang);
  geant issdz=800.;//"tube" length(its z-size)(limited by +-4m arount AMS att.point)
  geant issrad=200.;//"tube" radious
  geant issxp=issrad*isssin;//"tube" origin x-pos in AMS ref.syst.
  geant issyp=-90.;//"tube" origin y-pos
  geant isszp=-(182.6/isscos+issrad*isscos);//"tube" origin y-pos
  number issrm[3][3]={0.,-isssin,isscos,1.,0.,0.,0.,isscos,isssin};
//
  par[0]=issdfi/2.;
  par[1]=360.;
  par[2]=6;
  par[3]=2;
  par[4]=-issdz/2.;
  par[5]=0.;
  par[6]=issrad;
  par[7]=issdz/2.;
  par[8]=0.;
  par[9]=issrad;
  coo[0]=issxp;
  coo[1]=issyp;
  coo[2]=isszp;
  gid=1;
  AMSgvolume *isst = new  AMSgvolume("ISSTUBEMED",nrot,"ISST","PGON",par,10,coo,issrm,"ONLY",1,gid,1);// ISS "tube"
  false_mother.add(isst);

//-----> update amsg
  for(int i=0;i<3;i++)AMSDBc::ams_size[i]*=10;
}
//-----
//
if (strstr(AMSJob::gethead()->getsetup(),"AMS02")){
 cout <<" AMSGeom-I-AMS02 setup selected."<<endl;
 magnetgeom02(mother);
 tofgeom02(mother);
 ext1structure02(mother);//should be called after tofgeom02 !!!
 tkgeom02(mother);
 ext2structure(mother);
 antigeom02(mother);

#ifdef  __G4AMS__
   //testg4geom(mother);
 trdgeom02(mother);
 ecalgeom02(mother);
 richgeom02(mother);
#else
 trdgeom02(mother);
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
//--------------------------------------------------------------
void amsgeom::magnetgeom(AMSgvolume & mother){
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
// for  final(trapezoidal TOF) AMS02 setup:
//
void amsgeom::tofgeom02(AMSgvolume & mother){ 
geant prc[3]={0.,0.,0.};
geant par[6]={0.,0.,0.,0.,0.,0.};
geant pard[11]={0.,0.,0.,0.,0.,0.,0.,0.,0.,0.,0.};
number nrm[3][3]={1.,0.,0., 0.,1.,0., 0.,0.,1.};
number nrm1[3][3]={1.,0.,0., 0.,1.,0., 0.,0.,1.};// nonrotated (MRS)
//
number nrm2[3][3]={0.,1.,0.,-1.,0.,0., 0.,0.,1.};// sc(1:n-1) in rot.planes(L1,4)
number nrm3[3][3]={0.,1.,0.,1.,0.,0., 0.,0.,-1.};// sc(n)(+Y) in rot.planes(L1,4)
//
number nrm4[3][3]={-1.,0.,0.,0.,1.,0., 0.,0.,-1.};// sc(1)(-x) in nonrot.planes(L2,3)
//                                                  (sc(2:n) are in MRS) 
number rmd1[3][3]={1.,0.,0.,0.,-1.,0.,0.,0.,-1.};// dummy trang.vol. at -Yc
//                                                 (dummy vol.at +Yc is in sc.r.s.)
number inrm[3][3];
geant dz,dz1,dz2,zc,dx,dxi,dxo,dxt;
geant ocxc,ocyc,ocwd,ocewd,brlen,lglen,lgxb;
integer btyp,nbm,irot,nrot(0);
int ii,kk;
char inum[11];
char in[2]="0";
char vname[5];
char name[5];
number co[3]={0.,0.,0.};
geant coc[3]={0.,0.,0.};
geant coo[3]={0.,0.,0.};
geant cow[3]={0.,0.,0.};
geant cod[3]={0.,0.,0.};
integer gid(0),id(0);
integer nmed;
AMSNode *ptofc;
AMSNode *cur;
AMSNode *dau;
AMSgtmed *p;
AMSID amsid;
//
strcpy(inum,"0123456789");
TOF2DBc::readgconf();// read TOF-counters geometry parameters
//------
//          <-- create/position top supp. honeycomb plate
gid=1;
par[0]=TOF2DBc::supstr(7)/2; //dx
par[1]=TOF2DBc::supstr(8)/2; //dy
par[2]=TOF2DBc::supstr(9)/2; //Dz
coo[0]=TOF2DBc::supstr(3);    // x-shift from "0" of mother
coo[1]=TOF2DBc::supstr(4);    // y-shift ...
coo[2]=TOF2DBc::supstr(1)+TOF2DBc::supstr(9)/2;// z-centre of top supp. honeycomb
dau=mother.add(new AMSgvolume(
    "TOF_HONEYCOMB",0,"TOFH","BOX",par,3,coo,nrm1,"ONLY",1,gid,1));
//--------------
//          <-- create/position bot supp. honeycomb plate
gid=2;
par[0]=TOF2DBc::supstr(10)/2; //dx
par[1]=TOF2DBc::supstr(11)/2; //dy
par[2]=TOF2DBc::supstr(12)/2; //Dz
coo[0]=TOF2DBc::supstr(5);    // x-shift from "0" of mother
coo[1]=TOF2DBc::supstr(6);    // y-shift ...
coo[2]=TOF2DBc::supstr(2)-TOF2DBc::supstr(12)/2.;// z-centre of bot supp. honeycomb
dau=mother.add(new AMSgvolume(
    "TOF_HONEYCOMB",0,"TOFH","BOX",par,3,coo,nrm1,"ONLY",1,gid,1));
//----------------------------------------------------------------------
//             <-- create/position S1-S4 sc. planes :
//
dxi=TOF2DBc::plnstr(5);// dx(width) of inner sc.paddles
dz=TOF2DBc::plnstr(6);// dz(thickn)of sc.paddles
//                                 
for (int ip=0;ip<TOF2DBc::getnplns();ip++){ //  <<<=============== loop over sc. planes
//
  dxo=TOF2DBc::outcp(ip,1);//dx(width) of outer sc.paddles
  nbm=TOF2DBc::getbppl(ip);                      // num. of bars in layer ip
  dxt=(nbm-3)*(dxi-TOF2DBc::plnstr(4)); // sc.paddles distance for "normal" counters
//                                    (betw.centers, taking into account overlaping)
  if(ip<2){
    co[0]=TOF2DBc::supstr(3);// <--top TOF-subsystem X-shift
    co[1]=TOF2DBc::supstr(4);// <--top TOF-subsystem Y-shift
  }
  if(ip>1){
    co[0]=TOF2DBc::supstr(5);// <--bot TOF-subsystem X-shift
    co[1]=TOF2DBc::supstr(6);// <--bot TOF-subsystem Y-shift
  }
//
//-----------
  for(int ib=0;ib<nbm;ib++){ // <<<====== loop over sc. counter bars in plane ip
//
//---   <-- cr/position sc.paddles
//
    if(ib==0 || ib==(nbm-1))prc[0]=0.5*dxo;//dx/2 of outer sc.padds
    else prc[0]=0.5*dxi;//dx/2 of inner sc.padds
//
    btyp=TOF2DBc::brtype(ip,ib);
    brlen=TOF2DBc::brlen(ip,ib);//sc.length(loc.y-dir)
    lglen=TOF2DBc::lglen(ip,ib);//eff.lg.length(loc.y-dir)
    prc[1]=brlen/2; // dy/2  
    prc[2]=dz/2.; 
    if(TOF2DBc::plrotm(ip)==0){  // <-- unrotated planes
      coc[0]=TOF2DBc::gettsc(ip,ib);
      coc[1]=co[1];
      if(ib==0){//1st counter
        nrot+=1;
	irot=nrot;
        for(int i=0;i<3;i++)for(int j=0;j<3;j++)nrm[i][j]=nrm4[i][j];
      }
      else irot=0;
    }
    if(TOF2DBc::plrotm(ip)==1){  // <-- rotated planes
      coc[0]=co[0];
      coc[1]=TOF2DBc::gettsc(ip,ib);
      nrot+=1;
      irot=nrot;
      if(ib==(nbm-1)){//last counter
        for(int i=0;i<3;i++)for(int j=0;j<3;j++)nrm[i][j]=nrm3[i][j];
      }
      else{
        for(int i=0;i<3;i++)for(int j=0;j<3;j++)nrm[i][j]=nrm2[i][j];
      }
    }
    coc[2]=TOF2DBc::getzsc(ip,ib);
//
    gid=100*(ip+1)+ib+1;
    strcpy(vname,"TF");
    kk=ip*TOF2GC::SCMXBR+ib+1;//ID's of volumes(diff.from gid=userID !)
    ii=kk/10;
    in[0]=inum[ii];
    strcat(vname,in);
    ii=kk%10;
    in[0]=inum[ii];
    strcat(vname,in);
    ptofc=mother.add(new AMSgvolume(
    "TOF_SCINT",irot,vname,"BOX",prc,3,coc,nrm,"ONLY",0,gid,1));
//
//---        <-- cr/position dummy corners in outer paddles:
//
    if(ib==0 || ib==(nbm-1)){
      ocwd=TOF2DBc::outcp(ip,1);
      ocxc=TOF2DBc::outcp(ip,2);
      ocyc=TOF2DBc::outcp(ip,3);
      ocewd=TOF2DBc::outcp(ip,4);
      pard[0]=dz/2;
      pard[1]=0.;//the
      pard[2]=0.;//phi
      pard[3]=ocyc/2;//dy1(-Z)
      pard[4]=0.;//dx1l
      pard[5]=ocxc/2;//dx1h
      pard[6]=-AMSDBc::raddeg*atan(ocxc/ocyc/2);//alp1
      pard[7]=pard[3];//dy2 (+Z)
      pard[8]=0.;//dx2l
      pard[9]=pard[5];//dx2h
      pard[10]=pard[6];//alp2
#ifdef __G4AMS__
      pard[4]=0.001;// to bypass G4 bug
      pard[8]=0.001;// ................
#endif            
      cow[0]=ocwd/2-ocxc/4;//in paddle RefSyst
      cow[1]=brlen/2-ocyc/2;
      cow[2]=0.;
      dau=ptofc->add(new AMSgvolume(
      "TOF_VAC",0,"TOD1","TRAP",pard,11,cow,nrm1,"ONLY",gid==101?1:-1,gid,1));
      cow[1]=-cow[1];
      dau=ptofc->add(new AMSgvolume(
      "TOF_VAC",nrot++,"TOD2","TRAP",pard,11,cow,rmd1,"ONLY",gid==101?1:-1,gid,1));
    }
//
//---        <-- cr/position eff.light-guides(ext+lg):
//
    if(ib==0 || ib==(nbm-1))par[0]=ocewd/2;//ext.width of outer sc.padds
    else par[0]=dxi/2;//width/2 of inner sc.padds  
    par[1]=lglen/2;//dy/2  
    par[2]=prc[2];//dz/2
    coo[2]=coc[2];//z-pos as for sc.pad
    lgxb=ocwd/2-ocewd/2;//LG x-bias wrt sc.paddle x-center
    if(TOF2DBc::plrotm(ip)==0){          // <-- unrotated planes
      coo[0]=coc[0];//x-pos(transv) as for sc.pad (inner)
      if(ib==0)coo[0]=coo[0]+lgxb;//corr. for LG-bias of outer paddles
      if(ib==(nbm-1))coo[0]=coo[0]-lgxb;
      coo[1]=co[1]-(brlen+lglen)/2;//y-pos(long)
      dau=mother.add(new AMSgvolume(
        "TOF_LG",0,"TOL1","BOX",par,3,coo,nrm1,"ONLY",1,gid,1));//at -Yc
      coo[1]=co[1]+(brlen+lglen)/2;
      dau=mother.add(new AMSgvolume(
        "TOF_LG",0,"TOL2","BOX",par,3,coo,nrm1,"ONLY",1,gid,1));//at +Yc
    }
    else{                                // <-- rotated planes
      coo[1]=coc[1];//y-pos(transv) as for sc.pad (inner)
      if(ib==0)coo[1]=coo[1]+lgxb;//corr. for LG-bias of outer paddles
      if(ib==(nbm-1))coo[1]=coo[1]-lgxb;
      coo[0]=co[0]-(brlen+lglen)/2;//x-pos(long)
      nrot+=1;
      dau=mother.add(new AMSgvolume(
        "TOF_LG",nrot,"TOL1","BOX",par,3,coo,nrm2,"ONLY",1,gid,1));//at -Yc
      coo[0]=co[0]+(brlen+lglen)/2;
      dau=mother.add(new AMSgvolume(
       "TOF_LG",nrot,"TOL2","BOX",par,3,coo,nrm2,"ONLY",1,gid,1));//at +Yc
    }      
//
  }      //   <<<============= end of sc. bars loop ==========
}   //   <<<============= end of sc. planes loop =============
//-------
//        <---  Now put Boxes with PMTs:
//
  geant xbias,ybias,zbias;
//
// --------> for top TOF:
//            common box at +X, -X :                                   
    brlen=TOF2DBc::brlen(0,3);//sc.length Plane-1, middle bar4
    lglen=TOF2DBc::lglen(0,3);//eff.lg.length ...........
    xbias=brlen/2+lglen;
    zbias=TOF2DBc::plnstr(14);//wrt int.surf. of top honeycomb
    par[0]=TOF2DBc::plnstr(11)/2.; // dx/2
    par[1]=TOF2DBc::plnstr(12)/2;  // dy/2
    par[2]=TOF2DBc::plnstr(13)/2;  // dz/2
    coo[0]=TOF2DBc::supstr(3)+xbias+TOF2DBc::plnstr(11)/2+0.1;//"0.1" for safety
    coo[1]=TOF2DBc::supstr(4);
    coo[2]=TOF2DBc::supstr(1)+zbias-TOF2DBc::plnstr(13)/2;// z-center
    gid=1;
    dau=mother.add(new AMSgvolume(
    "TOF_PMT_BOX",0,"TOPB","BOX",par,3,coo,nrm1,"ONLY",1,gid,1));
    gid=2;
    coo[0]=TOF2DBc::supstr(3)-xbias-TOF2DBc::plnstr(11)/2-0.1;//"0.1" for safety
    dau=mother.add(new AMSgvolume(
    "TOF_PMT_BOX",0,"TOPB","BOX",par,3,coo,nrm1,"ONLY",1,gid,1));
//
//           common ring-sector at +Y,-Y : 
//                                   
    zbias=TOF2DBc::plnstr(2)-0.1;//ring int.surf. wrt top.honeyc. int.surf. 
    brlen=TOF2DBc::brlen(1,3);//sc.length Plane-2, middle bar4
    lglen=TOF2DBc::lglen(1,3);//eff.lg.length ...........
    par[0]=0.5*brlen+lglen;    // Rmin
    par[1]=par[0]+TOF2DBc::plnstr(16);    // Rmax 
    par[2]=TOF2DBc::plnstr(17)/2;    // dz/2
    par[3]=90.-0.5*TOF2DBc::plnstr(18);    // phi1
    par[4]=par[3]+TOF2DBc::plnstr(18);    // phi2
    coo[0]=TOF2DBc::supstr(3);     
    coo[1]=TOF2DBc::supstr(4);
    coo[2]=TOF2DBc::supstr(1)-zbias+TOF2DBc::plnstr(17)/2.;// ring Z-center
    gid=1;
    dau=mother.add(new AMSgvolume(
    "TOF_PMT_BOX",0,"TOPR","TUBS",par,5,coo,nrm1,"ONLY",1,gid,1));
    gid=2;
    par[3]=270.-0.5*TOF2DBc::plnstr(18);    // phi1
    par[4]=par[3]+TOF2DBc::plnstr(18);    // phi2
    dau=mother.add(new AMSgvolume(
    "TOF_PMT_BOX",0,"TOPR","TUBS",par,5,coo,nrm1,"ONLY",1,gid,1));
//
// --------> for bot TOF:
//            common box at +X, -X :                                   
    brlen=TOF2DBc::brlen(3,3);//sc.length Plane-4, middle bar4
    lglen=TOF2DBc::lglen(3,3);//eff.lg.length ...........
    xbias=brlen/2+lglen;
    zbias=TOF2DBc::plnstr(14);//wrt int.surf. of bot honeycomb
    par[0]=TOF2DBc::plnstr(11)/2.; // dx/2
    par[1]=TOF2DBc::plnstr(12)/2;  // dy/2
    par[2]=TOF2DBc::plnstr(13)/2;  // dz/2
    coo[0]=TOF2DBc::supstr(5)+xbias+TOF2DBc::plnstr(11)/2+0.1;//"0.1" for safety
    coo[1]=TOF2DBc::supstr(6);
    coo[2]=TOF2DBc::supstr(2)-zbias+TOF2DBc::plnstr(13)/2;// z-center
    gid=3;
    dau=mother.add(new AMSgvolume(
    "TOF_PMT_BOX",0,"TOPB","BOX",par,3,coo,nrm1,"ONLY",1,gid,1));
    gid=4;
    coo[0]=TOF2DBc::supstr(5)-xbias-TOF2DBc::plnstr(11)/2-0.1;//"0.1" for safety
    dau=mother.add(new AMSgvolume(
    "TOF_PMT_BOX",0,"TOPB","BOX",par,3,coo,nrm1,"ONLY",1,gid,1));
//
//           common ring-sector at +Y,-Y : 
//                                   
    zbias=TOF2DBc::plnstr(2)-0.1;//ring int.surf. wrt bot.honeyc. int.surf.
    brlen=TOF2DBc::brlen(2,3);//sc.length Plane-3, middle bar4
    lglen=TOF2DBc::lglen(2,3);//eff.lg.length ...........
    par[0]=0.5*brlen+lglen;    // Rmin
    par[1]=par[0]+TOF2DBc::plnstr(16);    // Rmax 
    par[2]=TOF2DBc::plnstr(17)/2;    // dz/2
    par[3]=90.-0.5*TOF2DBc::plnstr(20);    // phi1
    par[4]=par[3]+TOF2DBc::plnstr(20);    // phi2
    coo[0]=TOF2DBc::supstr(5);     
    coo[1]=TOF2DBc::supstr(6);
    coo[2]=TOF2DBc::supstr(2)+zbias-TOF2DBc::plnstr(17)/2.;// Z-center
    gid=3;
    dau=mother.add(new AMSgvolume(
    "TOF_PMT_BOX",0,"TOPR","TUBS",par,5,coo,nrm1,"ONLY",1,gid,1));
    gid=4;
    par[3]=270.-0.5*TOF2DBc::plnstr(20);    // phi1
    par[4]=par[3]+TOF2DBc::plnstr(20);    // phi2
    dau=mother.add(new AMSgvolume(
    "TOF_PMT_BOX",0,"TOPR","TUBS",par,5,coo,nrm1,"ONLY",1,gid,1));
//
//-----
//          CarbFiber box(it is replaced by solid thin plate)
// 
    zbias=TOF2DBc::plnstr(2)+TOF2DBc::plnstr(3)+
          TOF2DBc::plnstr(6)+TOF2DBc::plnstr(7);//wrt int.surf. of honeycomb
    par[0]=0.; //                  rmin
    par[1]=TOF2DBc::plnstr(9);  // rmax
    par[2]=TOF2DBc::plnstr(8)/2;// dz/2
    coo[0]=TOF2DBc::supstr(3);
    coo[1]=TOF2DBc::supstr(4);
    coo[2]=TOF2DBc::supstr(1)-zbias-TOF2DBc::plnstr(8)/2;// z-center
    gid=1;
    dau=mother.add(new AMSgvolume(
    "TOF_SC_COVER",0,"TOFB","TUBE",par,3,coo,nrm1,"ONLY",1,gid,1));//top TOF
    coo[0]=TOF2DBc::supstr(5);
    coo[1]=TOF2DBc::supstr(6);
    coo[2]=TOF2DBc::supstr(2)+zbias+TOF2DBc::plnstr(8)/2;// z-center
    gid=2;
    dau=mother.add(new AMSgvolume(
    "TOF_SC_COVER",0,"TOFB","TUBE",par,3,coo,nrm1,"ONLY",1,gid,1));//bot TOF
//-----
  cout<<"AMSGEOM: Final TOF02-geometry(G3/G4-compatible) done!"<<endl;
  cout<<"         "<<TOF2DBc::getbppl(0)<<"/"<<TOF2DBc::getbppl(1)<<"/"
  <<TOF2DBc::getbppl(2)<<"/"<<TOF2DBc::getbppl(3)<<" bars/plane selected !"<<endl;
}



//---------------------------------------------------------------
void amsgeom::antigeom002(AMSgvolume & mother){
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
  ANTI2DBc::setgeom();
  nscpad=ANTI2C::MAXANTI;
  scradi=ANTI2DBc::scradi();
  scinth=ANTI2DBc::scinth();
  scleng=ANTI2DBc::scleng();
  wrapth=ANTI2DBc::wrapth();
  groovr=ANTI2DBc::groovr();
  pdlgap=ANTI2DBc::pdlgap();
  stradi=ANTI2DBc::stradi();
  stleng=ANTI2DBc::stleng();
  stthic=ANTI2DBc::stthic();
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
    nrot=TOF2GC::SCROTN+100+i;
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
    nrot=TOF2GC::SCROTN+200+i;
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
  cout<<"AMSGEOM: ANTI02 geometry(G3-version) done !"<<endl;
}
//---------------------------------------------------------------------
//
void amsgeom::magnetgeom02o(AMSgvolume & mother){
// old (a la AMS01) design.
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
//------------------------------------------------
void amsgeom::magnetgeom02(AMSgvolume & mother){
// "real" AMS02 design, but the shape of coils/he-vessel is rectang. 
// with "the same weight(cross-section area)" dimensions.
AMSID amsid;
geant par[15]={0.,0.,0.,0.,0.,0.,0.,0.,0.,0.,0.,0.,0.,0.,0.};
geant coo[3]={0.,0.,0.};
number nrm[3][3]={1.,0.,0.,0.,1.,0.,0.,0.,1.};
int ii,kk;
char inum[11];
char in[2]="0";
char vname[5];
integer gid;
AMSNode * mmoth ;
AMSNode * mivol;
AMSNode * coil;
AMSNode * lhves;
AMSNode * cur;
     geant casl1=40.;     //vac.case half-length at inner radious
     geant casl2;//      .......................... outer .......
     geant casl1i,casl2i; // same for its internal surface
     geant tancon,coscon;
     geant conang=27.;    //flange angl(degr,wrt horizont)
     geant casr1=55.75;    //vac.case inner radious
     geant casr2=134.;    //vac.case outer radious
     geant cylit=0.3;     //inner cyl. thickness
     geant cylot=0.2;     //outer cyl. thickness
     geant flant=0.2;     //flange thickness
//
     strcpy(inum,"0123456789");
//
     tancon=tan(conang/AMSDBc::raddeg);
     coscon=cos(conang/AMSDBc::raddeg);
     casl2=casl1+(casr2-casr1)*tancon;
     casl1i=casl1+cylit*tancon-flant/coscon;
     casl2i=casl1i+(casr2-casr1-cylit-cylot)*tancon;
     par[0]=0.;//phi0
     par[1]=360.;//dphi
     par[2]=4;
     par[3]=-casl2;
     par[4]=casr2;
     par[5]=casr2;
     par[6]=-casl1;
     par[7]=casr1;
     par[8]=casr2;
     par[9]=casl1;
     par[10]=casr1;
     par[11]=casr2;
     par[12]=casl2;
     par[13]=casr2;
     par[14]=casr2;
     coo[0]=0.; 
     coo[1]=0.; 
     coo[2]=0.; 
     gid=1;
     mmoth=mother.add(new AMSgvolume(
          "MVCASEMED",0,"MMOT","PCON",par,15,coo,nrm,"ONLY",1,gid,1));//magnet case vol.
     par[3]=-casl2i;
     par[4]=casr2-cylot;
     par[5]=casr2-cylot;
     par[6]=-casl1i;
     par[7]=casr1+cylit;
     par[8]=casr2-cylot;
     par[9]=casl1i;
     par[10]=casr1+cylit;
     par[11]=casr2-cylot;
     par[12]=casl2i;
     par[13]=casr2-cylot;
     par[14]=casr2-cylot;
     mivol=mmoth->add(new AMSgvolume(
          "MVACMED",0,"MVOL","PCON",par,15,coo,nrm,"ONLY",1,gid,1));//inner vac.volume
//
// -----> return coils:
//
     geant rcrads=33.8;        //ret-coil radial size(Rout-Rint) incl.supp.
     geant rctrth=10.3;        //ret-coil trancv.(x/y) thickness(at mid.radious)
     geant rcradp=77.5;        //ret-coil radial pos.(in x-y plane)
     geant rchozs=39.3;        //ret-coil half out.z-size(rect! shape, 260+pi*169/4 mm)
     geant rchizs=32.8;        //ret-coil half inn.z-size(rect! shape, 260+pi*87/4 mm)
     geant rcrth=8.2;          //ret-coil tot.radial thickness of turns+supp(169-87mm)
     integer nrcoils=12;       //tot.number of ret-coils
     geant rcphis=360.*rctrth/rcradp/AMSDBc::twopi; //ret-coil phi-size (degree)
     geant rcphip[12]={60.,72.,84.,96.,108.,120.,240.,252.,264.,276.,288.,300.};
//                (ret-coils phi-positions)
     for(int nc=0;nc<nrcoils;nc++){
       strcpy(vname,"MR");
       kk=nc+1;
       ii=kk/10;
       in[0]=inum[ii];
       strcat(vname,in);
       ii=kk%10;
       in[0]=inum[ii];
       strcat(vname,in);
       par[0]=rcradp-rcrads/2.;//rmin
       par[1]=rcradp+rcrads/2.;//rmax
       par[2]=rchozs;//dz
       par[3]=rcphip[nc]-rcphis/2.;//phi1  
       par[4]=rcphip[nc]+rcphis/2.;//phi2  
       gid=nc+1;
       coil=mivol->add(new AMSgvolume(
          "MCOILMED",0,vname,"TUBS",par,5,coo,nrm,"ONLY",1,gid,1));//ret-coil
       par[0]=rcradp-rcrads/2.+rcrth;//rmin
       par[1]=rcradp+rcrads/2.-rcrth;//rmax
       par[2]=rchizs;//dz
       cur=coil->add(new AMSgvolume(
          "MVACMED",0,"MRHO","TUBS",par,5,coo,nrm,"ONLY",gid==1?1:-1,gid,1));//ret-coil hole
     }
//
// -----> dipole coils:
//
     geant dczth=19.;       //dip-coil tot.z-thick. of turns+supp(365-175mm)
     geant dczhs=36.5;      //dip-coil z half-size,incl.supp
     geant dcxpos=66.4;     //dip-coil x-pos
     geant dcxth=8.76;      //dip-coil x thickness
     geant dcohys=48.7;     //dip-coil out.half y-size,incl.supp(200+pi*365/4 mm)
     geant dcihys=33.7;     //dip-coil inn.half y-size,incl.supp(200+pi*175/4 mm)
//
     par[0]=dcxth/2.;//dx
     par[1]=dcohys;  //dy
     par[2]=dczhs;   //dz
     coo[0]=dcxpos;//x-pos
     coo[1]=0.;//y-pos
     coo[2]=0.;//z-pos 
     gid=1;
     coil=mivol->add(new AMSgvolume(
          "MCOILMED",0,"MDC1","BOX",par,3,coo,nrm,"ONLY",1,gid,1));//main coil-1
     par[1]=dcihys;//dy
     par[2]=dczhs-dczth;//dz
     coo[0]=0.;
     gid=1;
     cur=coil->add(new AMSgvolume(
          "MVACMED",0,"MDH1","BOX",par,3,coo,nrm,"ONLY",1,gid,1));//coil-1 hole
//----
     par[0]=dcxth/2.;//dx
     par[1]=dcohys;   //dy
     par[2]=dczhs;   //dz
     coo[0]=-dcxpos;//x-pos
     gid=1;
     coil=mivol->add(new AMSgvolume(
          "MCOILMED",0,"MDC2","BOX",par,3,coo,nrm,"ONLY",1,gid,1));//main coil-2
     par[1]=dcihys;//dy
     par[2]=dczhs-dczth;//dz
     coo[0]=0.;
     gid=1;
     cur=coil->add(new AMSgvolume(
          "MVACMED",0,"MDH2","BOX",par,3,coo,nrm,"ONLY",1,gid,1));//coil-2 hole
//
// -----> inter-fixation of dipole and return coils:
//
     geant drfrmn=casr1+cylit+1.5;//inner radious of fixation
     geant drfrmx=dcxpos-dcxth/2.-1.5;//outer .......
     geant drfdz=dczhs-dczth+1.;//z half size(to cover hole of dipole coil)
     geant drfopang=55.;//half opening angle(degr)
     par[0]=drfrmn;
     par[1]=drfrmx;
     par[2]=drfdz;
     par[3]=360.-drfopang;
     par[4]=360.+drfopang;
     coo[0]=0.;
     coo[1]=0.;
     coo[2]=0.;
     gid=1;
     cur=mivol->add(new AMSgvolume(
        "MVCASEMED",0,"DRF1","TUBS",par,5,coo,nrm,"ONLY",1,gid,1));//dip/ret.coil fix.1
     par[3]=180.-drfopang;
     par[4]=180.+drfopang;
     cur=mivol->add(new AMSgvolume(
        "MVCASEMED",0,"DRF2","TUBS",par,5,coo,nrm,"ONLY",1,gid,1));//dip/ret.coil fix.2
//
// -----> liq.helium vessel + helium/vacuum:
//
     geant hvrpos=112.5;    //he_vessel radial pos.
     geant hvrs=33.;        //he_vessel radial size
     geant hvohzs=55.5;     //he_vessel out.half z-size(425+pi*165/4 mm)
     geant hvihzs=55.26;     //he_vessel inn.half z-size(425+pi*162.5/4 mm)
     geant hvwth=0.25;      //he_vessel average wall(radial) thickness
//
     par[0]=hvrpos-hvrs/2.;//rmin
     par[1]=hvrpos+hvrs/2.;//rmax
     par[2]=hvohzs;//dz
     coo[0]=0.; 
     coo[1]=0.; 
     coo[2]=0.; 
     gid=1;
     lhves=mivol->add(new AMSgvolume(
          "MLHVMED",0,"MLHV","TUBE",par,3,coo,nrm,"ONLY",1,gid,1));//vessel
//     
     par[0]=hvrpos-hvrs/2.+hvwth;//rmin
     par[1]=hvrpos+hvrs/2.-hvwth;//rmax
     par[2]=hvihzs;//dz
     if(MAGSFFKEY.magstat>=0){
       cur=lhves->add(new AMSgvolume(
          "MLHEMED",0,"MLHE","TUBE",par,3,coo,nrm,"ONLY",1,gid,1));//liq.helium
       cout<<"AMSGEOM: MAGNET02 - cold state selected"<<endl;
     }
     else{
       cur=lhves->add(new AMSgvolume(
          "MVACMED",0,"MLHE","TUBE",par,3,coo,nrm,"ONLY",1,gid,1));//vacuum
       cout<<"AMSGEOM: MAGNET02 - warm state selected"<<endl;
      }
//
  cout<<"AMSGEOM: MAGNET02-geometry(G3/G4-compatible) done!"<<endl;
}
//-----------------------------------------------------------------
void amsgeom::ext1structure02(AMSgvolume & mother){
// external radiators, crates, TOF-TRD interfaces, USS exept bottom frame and 
//triangle, by E.C.
//
 AMSID amsid;
 geant par[15]={0.,0.,0.,0.,0.,0.,0.,0.,0.,0.,0.,0.,0.,0.,0.};
 geant coo[3]={0.,0.,0.};
 number nrm[3][3]={1.,0.,0.,0.,1.,0.,0.,0.,1.};
 number nrm0[3][3]={1.,0.,0.,0.,1.,0.,0.,0.,1.};
 int i,j,ii;
 char inum[11];
 char in[2]="0";
 char vname[5];
 integer gid,nrot;
 AMSNode * cur;
 AMSNode * dau;
//
// ----------------------------> Radiators:
// -------------> #1(vertical at +Y):
//
 geant r11dx=180.;//section 11 (should match to us1x1 !!!)
 geant r1dy=2.1;
 geant r11dz=180.4;
 geant r1cy=(170.5+r1dy/2.);//Y0
 geant r1cx=0.;//X0 
 geant r11cz=-18.;//Z0 of bott.part
//
 geant r12dx=210.;//section 12
 geant r12dz=36.;
 geant r12cz=90.2;//Z0 of top part
//
 nrot=TOF2GC::SCROTN+200;
//
 par[0]=r11dx/2.;
 par[1]=r1dy/2.;
 par[2]=r11dz/2.;
 coo[0]=r1cx;
 coo[1]=r1cy;
 coo[2]=r11cz;  
 gid=1;
 dau=mother.add(new AMSgvolume(
     "RADMED1",0,"RA1B","BOX",par,3,coo,nrm,"ONLY",1,gid,1));//radiator #1,bott.
//
 gid=2;
 par[0]=r12dx/2.;
 par[2]=r12dz/2.;
 coo[2]=r12cz;  
 dau=mother.add(new AMSgvolume(
     "RADMED1",0,"RA1T","BOX",par,3,coo,nrm,"ONLY",1,gid,1));//radiator #1,top.
//
// ------------> #2(vertical at -Y):
//
 geant r21dx=210.;//section 21
 geant r2dy=r1dy;
 geant r21dz=50.8;
 geant r2cx=0.;//X0
 geant r2cy=-(170.5+r2dy/2.);//Y0
 geant r21cz=-82.8;//Z0
//
 geant r22dx=r11dx;// section 22
 geant r22dz=129.6;
 geant r22cz=7.4;//Z0
//
 geant r23dx=210.;//section 23
 geant r23dz=36.;
 geant r23cz=90.2;//Z0
//
 par[0]=r21dx/2.;
 par[1]=r2dy/2.;
 par[2]=r21dz/2.;
 coo[0]=r2cx;
 coo[1]=r2cy;
 coo[2]=r21cz;  
 gid=1;
 dau=mother.add(new AMSgvolume(
     "RADMED1",0,"RA2B","BOX",par,3,coo,nrm,"ONLY",1,gid,1));//radiator #2,bott.
//
 gid=2;
 par[0]=r22dx/2.;
 par[2]=r22dz/2.;
 coo[2]=r22cz;  
 dau=mother.add(new AMSgvolume(
     "RADMED1",0,"RA2M","BOX",par,3,coo,nrm,"ONLY",1,gid,1));//radiator #2,midd.
//
 gid=3;
 par[0]=r23dx/2.;
 par[2]=r23dz/2.;
 coo[2]=r23cz;  
 dau=mother.add(new AMSgvolume(
     "RADMED1",0,"RA2T","BOX",par,3,coo,nrm,"ONLY",1,gid,1));//radiator #2,top.
//
// ------------> #3(inclined at +Y):
//
// geant r3cy1=112.;// top/unner corner y-pos.(r of TRD  top honeycomb)
 geant r3cy1=120.9;// top/unner corner y-pos.(r of TRD  top honeycomb)
// geant r3cz1=152.;// top/inner corner z-pos(midd. of TRD top honeycomb)
 geant r3cz1=156.2;// top/inner corner z-pos(midd. of TRD top honeycomb)
 geant r3cy2=r1cy;// bot/inner corner y-pos.
 geant r3cz2=r11cz+r11dz/2.+r12dz;//bot/inner corner z-pos
 geant r3dz=2.1;// thickness
 geant r3dx=r12dx;// dx
 geant alp3=atan((r3cz1-r3cz2)/(r3cy2-r3cy1));
 geant cosa=cos(alp3);
 geant sina=sin(alp3);
 number nrm3[3][3]={1.,0.,0.,0.,cosa,sina,0.,-sina,cosa};//rot matrix.v#3
//
 par[0]=r3dx/2.;
 par[1]=sqrt(pow((r3cz1-r3cz2),2)+pow((r3cy2-r3cy1),2))/2.-0.001;//"0.001" for safety
 par[2]=r3dz/2.;
 coo[0]=0.;
 coo[1]=0.5*(r3cy2+r3cy1)+0.5*r3dz*sina;
 coo[2]=0.5*(r3cz1+r3cz2)+0.5*r3dz*cosa;
 gid=1;
 dau=mother.add(new AMSgvolume(
     "RADMED1",nrot++,"RA3T","BOX",par,3,coo,nrm3,"ONLY",1,gid,1));//radiator #3,top.
//
// ------------> #4(inclined at -Y):
//
 number nrm4[3][3]={1.,0.,0.,0.,cosa,-sina,0.,sina,cosa};//rot matrix.v#4
//
 coo[1]=-coo[1];
 gid=1;
 dau=mother.add(new AMSgvolume(
     "RADMED1",nrot++,"RA4T","BOX",par,3,coo,nrm4,"ONLY",1,gid,1));//radiator #4,top.
//
// -----------------------------> Crates:
//
//----> big top crates:
//
 geant cr1dx=210.;// for +Y/top crate
 geant cr1dy=23.;
 geant cr1dz=26.2;
 geant cr1cy=142.5;//inner wall y-pos.
 geant cr1cz=78.3;// bot wall z-pos
//
 par[0]=cr1dx/2.;
 par[1]=cr1dy/2.;
 par[2]=cr1dz/2.;
 coo[0]=0.;
 coo[1]=cr1cy+cr1dy/2.;
 coo[2]=cr1cz+cr1dz/2.;
 gid=1;
 dau=mother.add(new AMSgvolume(
     "CRATEMED1",0,"CRB1","BOX",par,3,coo,nrm,"ONLY",1,gid,1));//crate +Y/top
 coo[1]=-coo[1];
 dau=mother.add(new AMSgvolume(
     "CRATEMED1",0,"CRB2","BOX",par,3,coo,nrm,"ONLY",1,gid,1));//crate -Y/top
//
//----> big bot. crates:
//
 coo[1]=-coo[1];
 coo[2]=-coo[2];
 dau=mother.add(new AMSgvolume(
     "CRATEMED1",0,"CRB3","BOX",par,3,coo,nrm,"ONLY",1,gid,1));//crate +Y/bot
 coo[1]=-coo[1];
 dau=mother.add(new AMSgvolume(
     "CRATEMED1",0,"CRB4","BOX",par,3,coo,nrm,"ONLY",1,gid,1));//crate -Y/bot
//
//----> small crates:
//
 geant cr2dx=134.;//for +Y crate
 geant cr2dy=28.;
 geant cr2dz=28.;
 geant cr2cy=99.5;//center y-pos
 cr2cy+=15.;//tempor solution of ISS overlaping problem
 geant cr2cz=-125.;//top wall z-pos
 cr2cz+=10.;//tempor solution of ISS overlaping problem
//
 par[0]=cr2dx/2.;
 par[1]=cr2dy/2.;
 par[2]=cr2dz/2.;
 coo[0]=0.;
 coo[1]=cr2cy;
 coo[2]=cr2cz-cr2dz/2.;
 gid=1;
 dau=mother.add(new AMSgvolume(
     "CRATEMED1",0,"CRS1","BOX",par,3,coo,nrm,"ONLY",1,gid,1));//small crate +Y
 coo[1]=-coo[1];
 dau=mother.add(new AMSgvolume(
     "CRATEMED1",0,"CRS2","BOX",par,3,coo,nrm,"ONLY",1,gid,1));//small crate -Y
//
//------------> TOF/TRD-interfaces:
// MSF
 geant msfr1=101.4;//(final) M-struct. frame (MSF) internal radious
 geant msfz1=93.64;//(final) MSF Z-pos 
 geant msfdz=6.;//(final) MSF z-thickness
 geant msfdr=6.;//(final) MSF radial thickness
// MS
 geant msdy=6.;//(final) M-structure thickness (MS)
 geant msr2=112.;//(final) MS radial poc.(near bot.edge of TRD top honeyc)
 geant msz2=146.2;//(final) MS z-pos.(near bot.edge of TRD top honeyc)
 geant msdx1=msfr1-22.4;//(final) MS-dx on MSF-side(to fit whole "M" on the frame side)
 geant msdx2=12.6;//(final) MS dx on TRD_top_honeyc side
 geant msang1=atan((msr2-msfr1-msfdr)/(msz2-msfz1-msfdz));
 geant msdz=sqrt(pow(msfr1+msfdr-msr2,2)+pow(msfz1+msfdz-msz2,2)-pow(msdy,2));//MS dz 
 geant msang=atan(msdy/msdz)+msang1;//MS-plane slope angle(~13degr)
 geant cosb=cos(msang);
 geant sinb=sin(msang);
 geant mscr=0.5*(msr2+msfr1+msfdr);//radial dist. of MS "0"
 geant msct=0.5*msdx1;//tang.dist. of MS "0"
 geant mscx[8]={mscr,mscr,msct,-msct,-mscr,-mscr,-msct,msct};//X-pos for 8 MS's
 geant mscy[8]={-msct,msct,mscr,mscr,msct,-msct,-mscr,-mscr};//Y-pos for 8 MS's
 geant mscz=0.5*(msfz1+msfdz+msz2);//Z-pos of MS
 number rmms1[3][3]={0.,cosb,sinb,-1.,0.,0.,0.,-sinb,cosb};//rot.matr.at +X
 number rmms2[3][3]={1.,0.,0.,0.,cosb,sinb,0.,-sinb,cosb};//rot.matr.at +Y
 number rmms3[3][3]={0.,-cosb,-sinb,1.,0.,0.,0.,-sinb,cosb};//rot.matr.at -X
 number rmms4[3][3]={-1.,0.,0.,0.,-cosb,-sinb,0.,-sinb,cosb};//rot.matr.at -Y
// TFHS(TOF-hoheycomb support)
 geant tfsdy=3.;// TFHS thickness
 geant tfsr1=TOF2DBc::supstr(8)+tfsdy;//tof-honeyc.support(TFHS) "low end" radial pos.
 geant tfsz1=TOF2DBc::supstr(1)+0.5*TOF2DBc::supstr(7);//"low end" Z-pos(mid of TOF-hon.side)
 geant tfsr2=msfr1;//"high end" radial pos
 geant tfsdx1=15.;// TFHS dx on TOF-honeycomb side
 geant tfsdx2=35.;// TFHS dx on M-structure side
 geant tfsang1=atan((tfsr2-tfsr1)/(msfz1-tfsz1));
 geant tfsdz=sqrt(pow(tfsr2-tfsr1,2)+pow(msfz1-tfsz1,2)-pow(tfsdy,2));//MS dz 
 geant tfsang=atan(tfsdy/tfsdz)+tfsang1;//TFHS-plane slope angle
 geant cosc=cos(tfsang);
 geant sinc=sin(tfsang);
 geant tfscr=0.5*(tfsr1+tfsr2);//radial pos. of TFHS "0"
 geant tfsct=25.;//tang.dist. of TFHS "0"
 geant tfscx[8]={tfscr,tfscr,tfsct,-tfsct,-tfscr,-tfscr,-tfsct,tfsct};//X-pos for 8 TFHS's
 geant tfscy[8]={-tfsct,tfsct,tfscr,tfscr,tfsct,-tfsct,-tfscr,-tfscr};//Y-pos for 8 TFHS's
 geant tfscz=0.5*(tfsz1+msfz1);//Z-pos of TFHS "0"
 number rmtfs1[3][3]={0.,cosc,sinc,-1.,0.,0.,0.,-sinc,cosc};//rot.matr.at +X
 number rmtfs2[3][3]={1.,0.,0.,0.,cosc,sinc,0.,-sinc,cosc};//rot.matr.at +Y
 number rmtfs3[3][3]={0.,-cosc,-sinc,1.,0.,0.,0.,-sinc,cosc};//rot.matr.at -X
 number rmtfs4[3][3]={-1.,0.,0.,0.,-cosc,-sinc,0.,-sinc,cosc};//rot.matr.at -Y
//
//----> Create MSF(M-structure frame,PGON):
//
 par[0]=45.;//begin phi
 par[1]=360;//dphi
 par[2]=4;//sides
 par[3]=2;// Nz
 par[4]=-msfdz/2.;//z1
 par[5]=msfr1;//r1
 par[6]=msfr1+msfdr;//r2
 par[7]=msfdz/2.;//z2
 par[8]=par[5];
 par[9]=par[6];
 coo[0]=0.;
 coo[1]=0.;
 coo[2]=msfz1+0.5*msfdz;
 gid=1;
 dau=mother.add(new AMSgvolume(
     "MSFMED1",0,"MSFR","PGON",par,10,coo,nrm,"ONLY",1,gid,1));// M-structure frame
//
//----> create TFHS(TOF-honeycomb supports, TRD1 shape): tempor removed !!
//
 par[0]=tfsdx1/2.;
 par[1]=tfsdx2/2.;
 par[2]=tfsdy/2.;
 par[3]=tfsdz/2.-0.001;
 coo[2]=tfscz;
 for(int is=0;is<8;is++){
   coo[0]=tfscx[is];
   coo[1]=tfscy[is];
   ii=is/2;
   if(ii==0)for(i=0;i<3;i++)for(j=0;j<3;j++)nrm0[i][j]=rmtfs1[i][j];
   if(ii==1)for(i=0;i<3;i++)for(j=0;j<3;j++)nrm0[i][j]=rmtfs2[i][j];
   if(ii==2)for(i=0;i<3;i++)for(j=0;j<3;j++)nrm0[i][j]=rmtfs3[i][j];
   if(ii==3)for(i=0;i<3;i++)for(j=0;j<3;j++)nrm0[i][j]=rmtfs4[i][j];
   gid=is+1;
   nrot+=ii;
//   dau=mother.add(new AMSgvolume(
//     "TFSUPMED1",nrot,"TFHS","TRD1",par,4,coo,nrm0,"ONLY",gid==1?1:-1,gid,1));//tempor TOF-honeyc.supp.
 }
//
//----> create M-structure legs:
//
 geant msalf=atan(2.*msdz/(msdx1-msdx2));// "M" triang.angle
 for(int is=0;is<8;is++){
   par[0]=msdx1/2.;
   par[1]=msdx2/2.;
   par[2]=msdy/2.;
   par[3]=msdz/2.-0.001;
   coo[0]=mscx[is];
   coo[1]=mscy[is];
   coo[2]=mscz;
   ii=is/2;
   if(ii==0)for(i=0;i<3;i++)for(j=0;j<3;j++)nrm0[i][j]=rmms1[i][j];
   if(ii==1)for(i=0;i<3;i++)for(j=0;j<3;j++)nrm0[i][j]=rmms2[i][j];
   if(ii==2)for(i=0;i<3;i++)for(j=0;j<3;j++)nrm0[i][j]=rmms3[i][j];
   if(ii==3)for(i=0;i<3;i++)for(j=0;j<3;j++)nrm0[i][j]=rmms4[i][j];
   gid=is+1;
   nrot+=ii;
   dau=mother.add(new AMSgvolume(
     "MSFMED1",nrot,"MSBO","TRD1",par,4,coo,nrm0,"ONLY",gid==1?1:-1,gid,1));//M-structure M-body
#ifndef __G4AMS__
   if(is==0){
#else
   if(MISCFFKEY.G4On || (is==0)){
#endif
     par[0]=msdx1/2.-msdy/sin(msalf);// hole dx1/2
     par[1]=msdy/tan(msalf);// hole dx2/2
     par[2]=msdy/2.;
     par[3]=(msdz-msdy)/2.;
     coo[0]=0.;
     coo[1]=0.;
     coo[2]=-msdy/2.;
     cur=dau->add(new AMSgvolume(
          "MVACMED",0,"MSHO","TRD1",par,4,coo,nrm,"ONLY",0,gid,1));// hole in it (vacuum)
#ifdef __G4AMS__
     ((AMSgvolume*)cur )->Smartless()=-2;
#endif            
   }
 }
//
//------------> USS :
//
//----> bar-1 type(attached to magnet top + TOF/TRD interface):
 geant us1dz=18.4;//final
 geant us1dx=15.2;//final
 geant us1x1=99.5-us1dx/2.;//(final)inner(wrt AMS "0") edge x-pos
 geant us1y1=99.5;//(final) inner edge y-pos(with us1x1 are defined by attach to VC
//                             at R=140.7cm and angle 45 degr.)                            
 geant us1z1=68.12-us1dz/2.;//(final) inner/bot edge z-pos 
 geant us1x2=us1x1;// outer edge x-pos
 geant us1y2=192.;//(final+cutted) outer edge y-pos
 geant us1z2=59.5-us1dz/2.;//(final+cutted) outer edge z-pos
 geant us1dy=sqrt(pow(us1y2-us1y1,2)+pow(us1z1-us1z2,2));
 geant us1ang=atan((us1z1-us1z2)/(us1y2-us1y1));//slope(>0)
 geant cosb1=cos(us1ang);
 geant sinb1=sin(us1ang);
 number rmus1[3][3]={1.,0.,0.,0.,cosb1,sinb1,0.,-sinb1,cosb1};
 number rmus1a[3][3]={1.,0.,0.,0.,cosb1,-sinb1,0.,sinb1,cosb1};
//
//----> bar-2 type(attached to magnet bottom):
//
 geant us2aan=46.3/180.*AMSDBc::pi;//beams angle abs.value
 geant us2dx=12.6;//(final)
 geant us2dz=15.2;//(final)
 geant us2x1=99.5-us2dx/2.;//(final)inner(wrt AMS "0") edge x-pos
 geant us2y1=us1y1;// inner edge y-pos
 geant us2z1=-90.14+0.5*us2dz*cos(us2aan);//(final) inner edge  z-pos
 geant us2x2=us2x1;// outer edge x-pos
 geant us2z2=us1z2-48.;//(final+cutted) outer edge z-pos
 geant us2y2=us2y1+(us2z2-us2z1)/tan(us2aan);//(final) outer edge y-pos
// (this y-pos is calc.from hand-fixed z-pos and known angle 46.3)
 geant us2dy=sqrt(pow(us2y2-us2y1,2)+pow(us2z2-us2z1,2));
 geant us2ang=atan((us2z1-us2z2)/(us2y2-us2y1));// slope(<0) for +Y beams
 geant cosb2=cos(us2ang);
 geant sinb2=sin(us2ang);
 number rmus2[3][3]={1.,0.,0.,0.,cosb2,sinb2,0.,-sinb2,cosb2};
 number rmus2a[3][3]={1.,0.,0.,0.,cosb2,-sinb2,0.,sinb2,cosb2};
//
//----> bar-3 type:
//
 geant us3x=0.;//center x-pos
 geant us3y=211.4;//(final)center y-pos
 geant us3z=57.6;//(final)center z-pos
 geant us3dx=2.*us1x1-0.001;
 geant us3dy=8.;// tempor
 geant us3dz=8.;
//
//------------> create 4 bars of type-1:
//
 par[0]=us1dx/2.;
 par[1]=us1dy/2.;
 par[2]=us1dz/2.;
 coo[0]=-(us1x1+us1dx/2.);
 coo[1]=0.5*(us1y1+us1y2)+sinb1*us1dz/2.;
 coo[2]=0.5*(us1z1+us1z2)+cosb1*us1dz/2.;
 gid=1;
 nrot+=1;
 dau=mother.add(new AMSgvolume(
   "US1BMED",nrot,"US1B","BOX",par,3,coo,rmus1,"ONLY",gid==1?1:-1,gid,1));//at (-x,+y)
 coo[0]=-coo[0];
 gid+=1;
 nrot+=1;
 dau=mother.add(new AMSgvolume(
   "US1BMED",nrot,"US1B","BOX",par,3,coo,rmus1,"ONLY",gid==1?1:-1,gid,1));//at (+x,+y)
 coo[1]=-coo[1];
 gid+=1;
 nrot+=1;
 dau=mother.add(new AMSgvolume(
   "US1BMED",nrot,"US1B","BOX",par,3,coo,rmus1a,"ONLY",gid==1?1:-1,gid,1));//at (+x,-y)
 coo[0]=-coo[0];
 gid+=1;
 nrot+=1;
 dau=mother.add(new AMSgvolume(
   "US1BMED",nrot,"US1B","BOX",par,3,coo,rmus1a,"ONLY",gid==1?1:-1,gid,1));//at (-x,-y)
//
//-----------> create 4 bars of type-2
//
 par[0]=us2dx/2.;
 par[1]=us2dy/2.;
 par[2]=us2dz/2.;
 coo[0]=-(us2x1+us2dx/2.);
 coo[1]=0.5*(us2y1+us2y2)-sinb2*us2dz/2.;
 coo[2]=0.5*(us2z1+us2z2)-cosb2*us2dz/2.;
 gid=1;
 nrot+=1;
 dau=mother.add(new AMSgvolume(
   "US2BMED",nrot,"US2B","BOX",par,3,coo,rmus2,"ONLY",gid==1?1:-1,gid,1));//at (-x,+y)
 coo[0]=-coo[0];
 gid+=1;
 nrot+=1;
 dau=mother.add(new AMSgvolume(
   "US2BMED",nrot,"US2B","BOX",par,3,coo,rmus2,"ONLY",gid==1?1:-1,gid,1));//at (+x,+y)
 coo[1]=-coo[1];
 gid+=1;
 nrot+=1;
 dau=mother.add(new AMSgvolume(
   "US2BMED",nrot,"US2B","BOX",par,3,coo,rmus2a,"ONLY",gid==1?1:-1,gid,1));//at (+x,-y)
 coo[0]=-coo[0];
 gid+=1;
 nrot+=1;
 dau=mother.add(new AMSgvolume(
   "US2BMED",nrot,"US2B","BOX",par,3,coo,rmus2a,"ONLY",gid==1?1:-1,gid,1));//at (-x,-y)
//
//-----------> create 2 bars of type-3 (innored now !!!)
//
 par[0]=us3dx/2.;
 par[1]=us3dy/2.;
 par[2]=us3dz/2.;
 coo[0]=us3x;
 coo[1]=us3y;
 coo[2]=us3z;
 gid=1;
// dau=mother.add(new AMSgvolume(
//   "US3BMED",0,"US3B","BOX",par,3,coo,nrm,"ONLY",gid==1?1:-1,gid,1));//at (+y)
 coo[1]=-coo[1];
 gid+=1;
// dau=mother.add(new AMSgvolume(
//   "US3BMED",0,"US3B","BOX",par,3,coo,nrm,"ONLY",gid==1?1:-1,gid,1));//at (-y)
//-------------------------------------
//----> bar-4 type(attached to magnet bottom and frame around EC):
//
 geant us4dx=11.5;//final
 geant us4dz=14.2;//final
 geant us4r1=106.5;//(final)radial pos. of low end(center)(check matching with low frame)
 geant us4r2=140.7;//(final)radial pos. of high end(center)(match top/bot USS joint radious)
 geant us4z1=-135.9;//final                               ................................
 geant us4z2=-95.;//final(to have 50.1 degr. slope)        ................................
 geant us4dy=sqrt(pow(us4r2-us4r1,2)+pow(us4z2-us4z1,2));
 geant us4bet=atan((us4z2-us4z1)/(us4r2-us4r1));//vert.slope
 geant cosb4=cos(us4bet);
 geant sinb4=sin(us4bet);
 number rmus4[3][3];
 geant us4alf[4]={45.,135.,225.,315.};//bar rot.angle in xy-plane
 geant sina4,cosa4;
//
//-----------> create 4 bars of type-4:
//
  par[0]=us4dx/2.;
  par[1]=us4dy/2.;
  par[2]=us4dz/2.;
  coo[2]=0.5*(us4z1+us4z2);
  for(i=0;i<4;i++){
    sina4=sin(us4alf[i]/AMSDBc::raddeg);
    cosa4=cos(us4alf[i]/AMSDBc::raddeg);
    coo[0]=0.5*(us4r2+us4r1)*sina4;
    coo[1]=0.5*(us4r2+us4r1)*cosa4;
    rmus4[0][0]=cosa4;
    rmus4[0][1]=sina4*cosb4;
    rmus4[0][2]=-sina4*sinb4;
    rmus4[1][0]=-sina4;
    rmus4[1][1]=cosa4*cosb4;
    rmus4[1][2]=-cosa4*sinb4;
    rmus4[2][0]=0.;
    rmus4[2][1]=sinb4;
    rmus4[2][2]=cosb4;
    gid=i+1;
    nrot+=1;
    dau=mother.add(new AMSgvolume(
      "US4BMED",nrot,"US4B","BOX",par,3,coo,rmus4,"ONLY",gid==1?1:-1,gid,1));
  }
//
  cout<<"AMSGEOM: Rads/crates/TOF-TRD_interface/USS geometry(G3/G4-compatible) done!"<<endl;
}
//------------------------------------------------------------------------
void amsgeom::magnetgeom02Test(AMSgvolume & mother){
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
//-------------------------------------------------------------------
void amsgeom::ext2structure(AMSgvolume & mother){
//USS low frame + triang., by V.Choutko(frame parameters modified by E.C
AMSID amsid;
geant par[10]={0.,0.,0.,0.,0.,0.,0.,0.,0.,0.};
number nrm[3][3]={1.,0.,0.,0.,1.,0.,0.,0.,1.};
number inrm[3][3];
integer nrot;
char name[5];
geant coo[3]={0.,0.,0.};
integer gid=0;
AMSNode * cur;
AMSNode * dau;
AMSgtmed *p;
      gid=1;

 nrot=TOF2GC::SCROTN+300;
// Low quadrant(frame)
geant frameztop=-135.95;//(final)Z-pos of the frame(top face)
        
   par[0]=45;   //  phi angle
   par[1]=360;   // open angle
   par[2]=4;     // quadrant
   par[3]=2;
   par[4]=-5.08;//final
   par[5]=130.22/2.;//final
   par[6]=par[5]+5.08*2.;//final
   par[7]=-par[4];
   par[8]=par[5];
   par[9]=par[6];
   coo[0]=0;
   coo[1]=0;
   coo[2]=frameztop+par[4];//z-center of the frame
   mother.add(new AMSgvolume("US5BMED",nrot++,"USLF","PGON",par,10,coo,nrm,"ONLY",0,gid++,1));


// Low Triangle ( two para)

   coo[1]=-(par[5]+par[6])/2/2;
   coo[0]=-(par[5]+par[6])/2;
   coo[2]=coo[2]+par[4];
   par[0]=4.5*2.54/2;
   par[1]=4.5*2.54/2;
   par[2]=4.5*2.54/2 * 3.6;
   coo[1]=-par[6]+par[2]+par[0];   //  implicit 45 deg assumption
   par[3]=0;
   par[4]=45;
   par[5]=0;

   coo[2]=coo[2]-par[2];
   number nrm1[3][3]={ 0, 1,0,
                      -1, 0,0,
                       0, 0,1
                     };
   mother.add(new AMSgvolume("USSALLOY",nrot++,"USS3","PARA",par,6,coo,nrm1,"ONLY",0,gid++,1));

   coo[1]=-coo[1];
   par[5]=180;


   mother.add(new AMSgvolume("USSALLOY",nrot++,"USS2","PARA",par,6,coo,nrm1,"ONLY",0,gid++,1));
//
  cout<<"AMSGEOM: Low_USS_frame geometry(G3/G4-compatible) done!"<<endl;
}
//--------------------------------------------------------------
void amsgeom::tkgeom02(AMSgvolume & mother){

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
int nhalfL=0; 
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
          if(k==0)nhalfL++;
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
         if(k==TKDBc::nhalf(i+1,j+1))nhalfL++;
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
cout <<"TKGeom-I-"<<nhalfL<<" Active halfladders initialized"<<endl;

}

#include <trdid.h>
void amsgeom::trdgeom02(AMSgvolume & mother){
using trdconst::maxco;
using trdconst::maxbulk;
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
AMSgvolume * daug4;
AMSNode * oct[maxo];
 ostrstream ost(name,sizeof(name));

int i;
// changed by VC 11-nov-2002  (removed trd radiator (i==0))
for ( i=1;i<TRDDBc::PrimaryOctagonNo();i++){
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
 if(i<10){
  ost << "TRD"<<i<<ends;
 }
 else if(i==10) ost << "TRDa"<<ends;
 else if(i==11) ost << "TRDb"<<ends;
 else if(i==12) ost << "TRDc"<<ends;
 else{
  cerr <<"Add some more octagons in trdgeom02 here"<<endl;
  abort();
 }
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

  // Put in the bulkheads
 int b,j,k,l;
 for(b=0;b<TRDDBc::BulkheadsNo(i);b++){

   int ip;
   gid=i+1;
   ost.seekp(0);  
   // Need to modify if TRDOctagonNo()>1
   ost << "TRB"<<b<<ends;
   TRDDBc::GetBulkhead(b,i,status,coo,nrm,rgid);
   for(ip=0;ip<4;ip++)par[ip]=TRDDBc::BulkheadsDimensions(i,b,ip);
   int itrd=TRDDBc::NoTRDOctagons(i);
   //         cout <<name<<" "<<j<<" "<<
   //coo[0]<<" "<<coo[1]<<" "<<coo[2]<<" "<<
   //	   par[0]<<" "<<par[1]<<" "<<par[2]<<" "<<par[4]<<endl;

#ifdef __G4AMS__
     if(MISCFFKEY.G4On){
       daug4 = 
      (AMSgvolume *)oct[itrd]->add(new AMSgvolume(TRDDBc::BulkheadsMedia(),
       nrot++,name,"TRD1",par,4,coo,nrm,"BOOL",0,gid,1));
     }
     else
#endif
      dau=oct[itrd]->add(new AMSgvolume(TRDDBc::BulkheadsMedia(),
      nrot++,name,"TRD1",par,4,coo,nrm,"MANY",0,gid,1));

      // Add the cutout daughters here

      for (j=0;j<TRDDBc::LayersNo(i);j++)
	{
	  for(k=0;k<TRDDBc::LaddersNo(i,j);k++){
	    for(l=0;l<TRDDBc::CutoutsNo(i,j,k);l++) {
	      int bhno;
	      bhno=TRDDBc::CutoutsBH(i,j,l);
	      if (bhno == b)
		{
		  ost.seekp(0);  
		  ost << "TRCO"<<ends;
		  gid=i+mtrdo*j+mtrdo*maxlay*k+mtrdo*maxlay*maxlad*l+1;
		  
		  TRDDBc::GetCutout(l,k,j,i,status,coo,nrm,rgid);  
		  int ip;
		  for(ip=0;ip<3;ip++)
		    par[ip]=TRDDBc::CutoutsDimensions(i,j,k,ip);

		  // First one made is j=4, in bulkhead 0
		  
#ifdef __G4AMS__
                 if(MISCFFKEY.G4On)
		  daug4->addboolean("BOX",par,3,coo,nrm,'-');
                 else
#endif
		  dau->add(new AMSgvolume(TRDDBc::CutoutsMedia(),
			 0,name,"BOX",par,3,coo,nrm,"ONLY", 
			 j==4 && b==0 && k==0 && l==0 ?1:-1,gid,1));

		}
	    }
	  }
	}
  }

  // Now the ladders and their contents
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
   // coo[0]<<" "<<coo[1]<<" "<<coo[2]<<" "<<
   //par[0]<<" "<<par[1]<<" "<<par[2]<<endl;

   dau=oct[itrd]->add(new AMSgvolume(TRDDBc::LaddersMedia(),
       nrot++,name,"BOX",par,3,coo,nrm, "ONLY",0,gid,1));
#ifdef __G4AMS__
//   ((AMSgvolume*) dau)->Smartless()=1;
#endif
//
// Tubes & Radiators have no rotation matrix at the moment
// This can be changed at any time
//

   // Radiators not radiators any more since now
   // inner octagon is made of radiator ... just vacuum boxes for the bottom
   // ladders (and possibly other holes).
   ost.seekp(0);  
   ost << "TRDR"<<ends;
   // Bottom layer has no radiator below the higher tubes
   if (j==0)
     {
       TRDDBc::GetRadiator(k,j,i,status,coo,nrm,rgid);
       for(ip=0;ip<3;ip++)par[ip]=TRDDBc::RadiatorDimensions(i,j,k,ip);
       gid=i+mtrdo*j+mtrdo*maxlay*k+1;

       dau->add(new AMSgvolume(TRDDBc::RadiatorMedia(),
	0,name,"BOX",par,3,coo,nrm, "ONLY",i==0 && j==0 && k==0?1:-1,gid,1));    
     }
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
   AMSTRDIdGeom  tmp(j,k,l,i);
   gid=tmp.cmpt();
   //   cout << "Tube "<<i<<" "<<j<<" "<<k<<" "<<l<<" gid "<<gid<<endl;
   AMSNode *ptrd=dau->add(new AMSgvolume(TRDDBc::TubesMedia(),
      0,name,"TUBE",par,3,coo,nrm, "ONLY",i==0 && j==0 && k==0 && l==0?1:-1,gid,1));    
#ifdef __G4AMS__
           ((AMSgvolume*)ptrd )->Smartless()=-2;
#endif            

   }

   for(l=0;l< TRDDBc::TubesNo(i,j,k);l++){
   ost.seekp(0);  
   ost << "TRDT"<<ends;
   TRDDBc::GetTube(l,k,j,i,status,coo,nrm,rgid);
   for(ip=0;ip<3;ip++)par[ip]=TRDDBc::ITubesDimensions(i,j,k,ip);
//   changed by VC to facilitate numb scheme
//   gid=i+mtrdo*j+mtrdo*maxlay*k+mtrdo*maxlay*maxlad*l+1;
   gid=maxtube*maxlad*maxlay*i+maxtube*maxlad*j+maxtube*k+l+1;
   //   cout << "Inner Tube "<<i<<" "<<j<<" "<<k<<" "<<l<<" gid "<<gid<<endl;
   dau->add(new AMSgvolume(TRDDBc::ITubesMedia(),
      0,name,"TUBE",par,3,coo,nrm, "ONLY",i==0 && j==0 && k==0 && l==0?1:-1,gid,1));    
   }
  }
 }
}

{
//  Now Xe Rad Spikes
    
   number nrml[3][3]={1.,0.,0.,0.,1.,0.,0.,0.,1.};
    gid=0;
    for (int i=0;i<4;i++){
     for (int j=0;j<trdconst::maxspikes;j++){
      // spikes go to 6th primary octagon directly
       for(int ip=0;ip<3;ip++)coo[ip]=TRDDBc::getSpikesPar(j,i,ip);
       for(int ip=0;ip<3;ip++)par[ip]=TRDDBc::getSpikesPar(j,i,ip+3);
       oct[5]->add(new AMSgvolume(TRDDBc::XeRadSpikesMedia(),
       nrot++,"TRDS","TUBE",par,3,coo,nrml, "ONLY",1,++gid,1));
    }
   }
}
{
//  Now Xe Rad pipes
    
//  Assuming Mirroring of 4 quadrants  
    gid=0;
    for (int i=0;i<4;i++){
     for (int j=0;j<trdconst::maxpipes;j++){
       for(int ip=0;ip<3;ip++)coo[ip]=TRDDBc::getPipesPar(j,i,ip);
       for(int ip=0;ip<3;ip++)par[ip]=TRDDBc::getPipesPar(j,i,ip+3);
       coo[2]-=par[1];
       geant hdist=TRDDBc::getPipesPar(j,i,6);
       TRDDBc::getPipesNRM(j,i,nrm);

/*
         cout <<"  coo ini "<<coo[0]<<" "<<coo[1]<<" "<<coo[2]<<endl;
         cout <<"  par "<<par[0]<<" "<<par[1]<<" "<<par[2]<<endl;
         cout <<"  hdist "<<hdist<<endl;
         for(int il=0;il<3;il++){
           for (int jl=0;jl<3;jl++)cout <<nrm[il][jl]<<" ";
           cout <<endl;
       }    
*/

//      now add two parallel pipes  with half dist hdist

      // pipes go to 12th octagon  

       if(nrm[0][0] || nrm[1][0] ){
         geant cool[3];
         for (int ip=0;ip<3;ip++)cool[ip]=coo[ip]-hdist*nrm[ip][0];
         oct[11]->add(new AMSgvolume(TRDDBc::PipesMedia(),
         nrot++,"TRDP","TUBE",par,3,cool,nrm, "ONLY",1,++gid,1));
         for (int ip=0;ip<3;ip++)cool[ip]=coo[ip]+hdist*nrm[ip][0];
         oct[11]->add(new AMSgvolume(TRDDBc::PipesMedia(),
         nrot++,"TRDP","TUBE",par,3,cool,nrm, "ONLY",1,++gid,1));
       }
    }
   }
}

cout <<"amsgeom::trdgeom02-I-TRDGeometryDone"<<endl;

}
//----------------------------------------------------------------
void amsgeom::antigeom02(AMSgvolume & mother){
#ifdef __G4AMS__
 if(MISCFFKEY.G4On){
   antigeom02g4(mother);
 }
 else if(MISCFFKEY.G3On){
#endif
 amsgeom::antigeom002(mother);
#ifdef __G4AMS__
}
#endif
}
//------------------------------
#ifdef __G4AMS__
// ANTI AMS02 G4-compatible:
//
void amsgeom::antigeom02g4(AMSgvolume & mother){
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
  ANTI2DBc::setgeom();
  nscpad=ANTI2C::MAXANTI;
  scradi=ANTI2DBc::scradi();
  scinth=ANTI2DBc::scinth();
  scleng=ANTI2DBc::scleng();
  wrapth=ANTI2DBc::wrapth();
  groovr=ANTI2DBc::groovr();
  pdlgap=ANTI2DBc::pdlgap();
  stradi=ANTI2DBc::stradi();
  stleng=ANTI2DBc::stleng();
  stthic=ANTI2DBc::stthic();
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
   cout<<"ANTI02-geometry(G4-version) done !.."<<endl;
}
#endif
//---------------------------------------------------------------------
void amsgeom::ecalgeom02(AMSgvolume & mother){
//
ECALDBc::readgconf();// 
  geant par[6]={0.,0.,0.,0.,0.,0.};
  number nrm[3][3];
  number nrm0[3][3]={1.,0.,0.,0.,1.,0.,0.,0.,1.};
  number nrm1[3][3]={1.,0.,0., 0.,0.,1., 0.,-1.,0.};// for X-proj fibers wrt ECMO
  number nrm2[3][3]={0.,0.,1.,0.,1.,0., -1.,0.,0.}; // for Y-proj fibers
  geant coo[3]={0.,0.,0.};
  geant dx1,dy1,dx2,dy2,dz,dzh,xpos,ypos,zpos,cleft,fpitx,fpitz,fpitzz,zfl1;
  geant dzrad1,zmrad1,alpth,flen,zposfl,dxe;
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
  dzrad1=ECALDBc::gendim(9);// z-size of one SL-radiator(lead only)
  alpth=ECALDBc::gendim(10);// Al(or glue) layer  thickness on top(bot) side of SL
  nsupl=ECALDBc::slstruc(3);// numb.of super-layers
  nflpsl=ECALDBc::slstruc(2);// numb.of fiber-layers per super-layer
  nfpl[0]=ECALDBc::nfibpl(1);// fibers/(1st layer) 
  nfpl[1]=ECALDBc::nfibpl(2);// fibers/(2nd layer)
  dz=(dzrad1+2.*alpth)*nsupl;//ECrad. tot.thickness(incl. Al-plates(now glue), excl. Honeyc)
  cout<<"EcalGeom: EC total radiator(incl.Al-plates) thickness "<<dz<<" cm"<<endl;
  dzh=ECALDBc::gendim(8);// Z-thickness of honeycomb
  xpos=ECALDBc::gendim(5);// x-pos EC-radiator center
  ypos=ECALDBc::gendim(6);// y-pos
  zpos=ECALDBc::gendim(7)-dz/2.;// z-pos of ECAL-center
  fpitx=ECALDBc::fpitch(1);
  fpitz=ECALDBc::fpitch(2);
  fpitzz=dzrad1-(nflpsl-1)*fpitz+2.*alpth;
  cout<<"EcalGeom: fpitchZZ "<<fpitzz<<" cm"<<endl;
//------------------------------------
  par[0]=dx1/2.+dxe;
  par[1]=dy1/2.+dxe;
  par[2]=dz/2.+dzh;
  coo[0]=xpos;
  coo[1]=ypos;
  coo[2]=zpos;
  gid=1;
  ECmother=mother.add(new AMSgvolume(
       "EC_AL_PLATE",0,"ECMO","BOX",par,3,coo,nrm0,"ONLY",0,gid,1));// create ECAL mother volume
//------------------------------------
//  create top/bot honeycomb plate in ECAL-mother:
//
  par[0]=dx1/2.+dxe/2.;
  par[1]=dy1/2.+dxe/2.;
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
// create PM_support_structure(frame) around radiator(in ECAL-mother):
//2 sides along Y:
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
//2 sides along X:
  par[0]=dx1/2.+dxe;
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
//
    zmrad1=dz/2.-alpth-dzrad1/2.;// zmid of  1st SL radiator(in ECMO r.s)
//
    for(isupl=0;isupl<nsupl;isupl++){ // <--- super-layers loop
      ip=isupl%2;
      if(ip==0)iproj=ECALDBc::slstruc(1);// iproj=0 ->X, =1 ->Y
      if(ip==1)iproj=1-ECALDBc::slstruc(1);
      if(iproj==0){ // in X-proj fiber goes along Y-axis
        flen=dy1/2.;
        for(i=0;i<3;i++)for(j=0;j<3;j++)nrm[i][j]=nrm1[i][j];
        par[0]=dx1/2.;
        par[1]=dzrad1/2.;
        par[2]=dy1/2.;
      }
      else{         // in Y-proj fiber goes along X-axis
        flen=dx1/2.;
        for(i=0;i<3;i++)for(j=0;j<3;j++)nrm[i][j]=nrm2[i][j];
        par[0]=dzrad1/2.;
        par[1]=dy1/2.;
        par[2]=dx1/2.;
      }
//------> create/put SL-Radiator:
//
      nrot=ecalconst::ECROTN+isupl;
      strcpy(vname,"ECR");
      k=isupl+1;//numbering 1-9 of superlayers(1-digit)
      in[0]=inum[k];
      strcat(vname,in);
      coo[0]=0.;
      coo[1]=0.;
      coo[2]=zmrad1-isupl*(dzrad1+2.*alpth);
      gid=isupl+1;
      pECrad=ECmother->add(new AMSgvolume(
         "EC_RADIATOR",nrot,vname,"BOX",par,3,coo,nrm,"ONLY",0,gid,1));//cr. EC_rad in ECmother
//
//------> create/put fiber-layer(s) and individual fibers in EC_rad:
//
      for(ifibl=0;ifibl<nflpsl;ifibl++){ // <--- fiber-layers loop in s-layer
        ip=ifibl%2;//even(1)/odd(0) f-layer
	nf=nfpl[ip];// total fibers in layer
	cleft=-(nf-1)*fpitx/2.;// tempor imply nfpl[1]=nfpl[0]+-1 sceme !!!
	if(iproj==0){ // X
          par[0]=dx1/2.;
          par[1]=ECALDBc::rdcell(4)/2.+ECALDBc::rdcell(8)+0.001;// fiber+glue+safety radious
          par[2]=dy1/2.;
	  coo[0]=0.;
	  coo[1]=-(nflpsl-1)*fpitz/2.+ifibl*fpitz;//f-layer pos along the local Y
	}
	else{ // Y
          par[0]=ECALDBc::rdcell(4)/2.+ECALDBc::rdcell(8)+0.001;// fiber+glue+safety radious
          par[1]=dy1/2.;
          par[2]=dx1/2.;
	  coo[0]=-(nflpsl-1)*fpitz/2.+ifibl*fpitz;//f-layer pos along the local X
	  coo[1]=0.;
	} 
	coo[2]=0.;
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
               "EC_RADIATOR",0,vname,"BOX",par,3,coo,nrm0,"ONLY",0,gid,1));//cr. f-layer in ECrad
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
#ifndef __G4AMS__
          if(isupl==0 && ifibl==0 && ifib==0){

#else
          if(MISCFFKEY.G4On || (isupl==0 && ifibl==0 && ifib==0)){
#endif
            par[1]=ECALDBc::rdcell(4)/2.;// fiber-core radious
	    coo[0]=0.;
	    coo[1]=0.;
            pECfsen=pECfib->add(new AMSgvolume(
            "EC_FCORE",0,"ECFC","TUBE",par,3,coo,nrm0,"ONLY",0,gid,1));
#ifdef __G4AMS__
           ((AMSgvolume*)pECfsen )->Smartless()=-2;
#endif            
	  }	 
	} // ---> end of fiber loop
//-----------
      } // ---> end of f-layer loop
//------
    } // ---> end of superlayer loop
//
  cout<<"AMSGEOM: ECAL-NewGeometry done!"<<endl;
}
//------------------------------------------------------------
/*
#include <srddbc.h>
void amsgeom::srdgeom02(AMSgvolume & mother){
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
*/
#ifdef __G4AMS__
void amsgeom::testg4geom(AMSgvolume &mother){
AMSID amsid;
geant par[6]={0.,0.,0.,0.,0.,0.};
char name[]="MyFirstReplica";
char name2[]="My2ndReplica";
geant coo[3]={0.,0.,12.};
number nrm[3][3]={0,0,1,1,0,0,0,1,0};
integer gid=0;
AMSNode * cur;
AMSgvolume * dau;
AMSgtmed *p;
     geant magnetl=86.;
      gid=1;
      par[0]=0.;
      par[1]=10./2;
      par[2]=20./2;
      par[3]=10;
      par[4]=par[1]*2;
      par[5]=0;
      dau=(AMSgvolume*)mother.add(new AMSgvolume(
      "MAGNET",0,name,"TUBE",par,6,coo,
       nrm, "REPX",0,gid,1));
      par[0]=0;
      par[1]=par[1]/2;
      par[2]=par[2]/2;
      coo[0]=0;
      coo[1]=0;
      coo[2]=0;
      dau->add( new AMSgvolume("VACUUM",0,name2,"TUBE",par,3,coo,nrm,"ONLY",
      0,gid+1,1));       
/*
      gid=2;
      coo[2]=+50;
      par[0]=8./2;
      par[1]=8./2;
      par[2]=16./2;
      par[3]=12;
      par[4]=par[0]*2+0.2;
      par[5]=0;
      dau=(AMSgvolume*)mother.add(new AMSgvolume(
      "MAGNET",0,name2,"BOX",par,6,coo,
       nrm, "REPX",0,gid,1));

*/

}
#endif


