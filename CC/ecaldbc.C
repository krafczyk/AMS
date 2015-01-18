//  $Id$
// Author E.Choumilov 14.07.99.
// latest update by E.Choumilov 11.06.2009
#include "typedefs.h"
#include "cern.h"
#include "extC.h"
#include <math.h>
#include "commons.h"
#include "job.h"
#include "ecaldbc.h"
#include <stdio.h>
#include "ecalcalib.h"
#include "event.h"
#include "ntuple.h"
#include <fstream>
//
using namespace ecalconst;
ECcalib ECcalib::ecpmcal[ECSLMX][ECPMSMX];// mem.reserv.for ECAL indiv.PMcell calib. param.
uinteger ECcalib::CFlistC[7]; 
ECcalibMS ECcalibMS::ecpmcal[ECSLMX][ECPMSMX];// the same for MC-Seeds params
ECPMPeds ECPMPeds::pmpeds[ECSLMX][ECPMSMX];// ..........for ECAL peds,sigmas
ECALVarp ECALVarp::ecalvpar;// .........................for ECAL general run-time param.  
ECTslope ECTslope::ecpmtslo[ECSLMX][ECPMSMX];// mem.reserv.for ECAL Temperature slopes 
uinteger ECTslope::CFlistC[7]; 
ECDailyMip ECDailyMip::ecmipday[ECSLMX][ECPMMX];// mem.reserv.for ECAL Cell Mip Peaks
uinteger ECDailyMip::CFlistC[7]; 
ECLongTerm ECLongTerm::eclongterm[ECSLMX][ECPMMX];// mem.reserv.for ECAL Cell Mip Peaks
uinteger ECLongTerm::CFlistC[7]; 
//-----------------------------------------------------------------------
//  =====> ECALDBc class variables definition :
//
integer ECALDBc::debug=1;
//
//
// Initialize ECAL geometry (defaults, REAL VALUES are read FROM geomconfig-file)
//
//---> default structural. data:
//
geant ECALDBc::_gendim[20]={// Warning: only alignment params are readout from EcalAlign*.dat file
  65.8,65.8,0., // i=1-2  x,y-dimentions of EC-radiator; 3->spare
  8.2,          //  =4    dx(dy) thickn.of (PMT+electronics)-support(frame)
  0.,0.,        //  =5,6    center shift in x,y (real values are read from EcalAlign* files !!!) 
  -142.3,        //  =7      Radiator(incl.glue) front face Z-pozition (........................)
  4.18,         //  =8      top(bot) honeycomb thickness
  1.83,         //  =9      lead thickness of 1 SuperLayer
  0.01,         //  =10     Thickness of glue on top(bot) side of SL
  0.032145,-0.0583656,0.0217877, 0.015298,0.0655636,  //=11-19 -> superlayer fibers common shift for each of 9 superlayers
  0.0075863,0.0402682,0.0672924,-0.052,
  0.             //  =20     spare    
};
//
geant ECALDBc::_fpitch[3]={
  0.135,      // i=1   fiber pitch in X(Y) projection
  0.184,0.    // i=2 fiber pitch in Z (inside super-layer); =3->spare
};
//
geant ECALDBc::_rdcell[10]={
  368.,32.5,0.15,//i=1,3  MC-def fib.att.length slow)/fast/fast_fract(real values from DB !!)
  0.094,        // i=4    fiber_core diameter(0.1-2x0.003cm)
  0.9,          // i=5    size(dx=dz) of "1/4" of PMT-cathod (pixel)
  0.45,         // i=6    abs(x(z)-position) of "1/4" in PMT coord.syst.
  1.8,          // i=7    X(Y)-pitch of PMT's;
  0.01135,      // i=8    fiber_cladd+glue thickn(.003+.00835cm) to have hole diam.=0.1167
  0.6,0.333     // i=9 -> Pm-pixel eff at the edge, i=10 -> distance where eff. reach 1.(relat. to pix.size at i=5)
};
//
integer ECALDBc::_slstruc[6]={
  1,           // i=1   1st super-layer projection(0->X, 1->Y)
  10,           // i=2   numb. of fiber-layers per super-layer
  9,            //  =3   real numb. of super-layers (X+Y)
  36,           //  =4   real numb. of PMT's per super-layer (in X(Y))
  -1,1           //  =5-6 readout dir. in X/Y-proj (=1/-1->along/oppos Y/X-axes) for 1st PM  
};
//
integer ECALDBc::_nfibpl[2]={
  486,485       // i=1,2 numb. of fibers per 1st/2nd fiber-layer in S-layer
};
//
int ECALDBc::_scalef=2;// MC/Data scale factor used in ADC->DAQ-value conversion.
geant ECALDBc::_ftedel=40.;//tempor: signals delay between EC/JLV1-crates + JLV1 FTE-decision delay
//
//  member functions :
//
// ---> function to read geomconfig-files 
//                  Called from ECALgeom :
void ECALDBc::readgconf(){
  //
  int i;
  char fname[1024];
  char name[80]="EcalAlign";
  char vers2[10]="PreAss";//= first clean room assembly(jan 2008)
  char vers3[10]="Ass1";//= final clean room assembly (oct 2009)
  char vers4[10]="Space";//final
  //  char vers5[10]="PMag";//permanent magnet
  char vers5[15]="ISS_20120512";// ISS alignment (modified May 12, 2012 by S. Di Falco )
  geant ZShift(0);
  //
  if(strstr(AMSJob::gethead()->getsetup(),"AMS02D")){
    cout <<"<------ ECALGeom-I- AMS02D configuration is used..."<<endl;
    strcat(name,vers4);//as space
    ZShift=-AMSDBc::amsdext;
    cout<<"        ZShift="<<ZShift<<endl;
  }
  //
  else if(strstr(AMSJob::gethead()->getsetup(),"AMS02")){
    if(strstr(AMSJob::gethead()->getsetup(),"PreAss")){
      cout <<"<------ ECALGeom-I- AMS02 configuration is used..."<<endl;
      cout <<"      PreAssembly(CleanRoom) setup selected..."<<endl;
      strcat(name,vers2);//clean room
    }
    else if(strstr(AMSJob::gethead()->getsetup(),"Ass1")){
      cout <<"<------ ECALGeom-I- AMS02 configuration is used..."<<endl;
      cout <<"      FinalAssembly(CleanRoom) setup selected..."<<endl;
      strcat(name,vers3);//clean room
    }
    else if(strstr(AMSJob::gethead()->getsetup(),"Space")){
      cout <<"<------ ECALGeom-I- AMS02 configuration is used..."<<endl;
      cout <<"      Space setup selected..."<<endl;
      strcat(name,vers4);//space
    }
    else if(strstr(AMSJob::gethead()->getsetup(),"AMS02P")){
      cout <<"<------ ECALGeom-I- AMS02P(perm.magnet) configuration is used..."<<endl;
      strcat(name,vers5);//perm.magnet
    }
    else{
      cout <<"<------ ECALGeom-I- AMS02 configuration is used..."<<endl;
      cout <<"    Assembly_1(as default) setup selected..."<<endl;
      strcat(name,vers3);//Ass1(default)
    }
  }
  //
  else
    {
      cout <<"<------ Error: unknown setup !??"<<endl;
      exit(1);
    }
  strcat(name,".dat");
  //
  if(ECCAFFKEY.cafdir==0)strcpy(fname,AMSDATADIR.amsdatadir);
  if(ECCAFFKEY.cafdir==1)strcpy(fname,"");
  strcat(fname,name);
  cout<<"        Open file : "<<fname<<'\n';
  ifstream tcfile(fname,ios::in); // open needed config-file for reading
  if(!tcfile){
    cerr <<"<------ EcalGeom-read: missing geomconfig-file "<<fname<<endl;
    exit(1);
  }
  for(i=0;i<3;i++) tcfile >> _gendim[4+i];//1 3-numbers row (dx,dy,Z values)
  _gendim[6]+=ZShift;//Zshift!=0 only for AMS02D configuration !!!
  cout<<"<------ ECALDBc::readgconf: Setup selection is done !"<<endl<<endl;
}
//---
//
geant ECALDBc::gendim(integer i){
#ifdef __AMSDEBUG__
  if(ECALDBc::debug){
    assert(i>0 && i <= 20);
  }
#endif
  return _gendim[i-1];
}
//
geant ECALDBc::fpitch(integer i){
#ifdef __AMSDEBUG__
  if(ECALDBc::debug){
    assert(i>0 && i <= 3);
  }
#endif
  return _fpitch[i-1];
}
//
geant ECALDBc::rdcell(integer i){
#ifdef __AMSDEBUG__
  if(ECALDBc::debug){
    assert(i>0 && i <= 10);
  }
#endif
  return _rdcell[i-1];
}
//

 
integer ECALDBc::slstruc(integer i){
#ifdef __AMSDEBUG__
  if(ECALDBc::debug){
    assert(i>0 && i <= 6);
  }
#endif
  return _slstruc[i-1];
}
//
integer ECALDBc::nfibpl(integer i){
#ifdef __AMSDEBUG__
  if(ECALDBc::debug){
    assert(i>0 && i <= 2);
  }
#endif
  return _nfibpl[i-1];
}
//
//---
number ECALDBc::segarea(number r, number ds){//smaller_segment area fraction (wrt full disk)
  //                                    r-radious, ds-horde_distance(from center)
  number sina,cs,sn,a;
  if(fabs(ds)>=r)return(0.);
  cs=fabs(ds)/r;
  sn=sqrt(1.-cs*cs);
  sina=2.*cs*sn;
  a=2.*asin(sn);//sector opening angle:(0->pi) when ds (r->0)
  return ((a-sina)/2./AMSDBc::pi);//(0.5->0) when ds  (0->r)
}
//---
// fiberID(SSLLFFF) to cellID(SSPPC) conversion
// ("analog" design - fiber_edep division between neigb.pixels/pmts with fraction w)
//
void ECALDBc::fid2cida(integer fid, integer cid[4], number w[4]){
  integer fidd,fff,ss,ll,ip,nfl,npm,pm,cell,bran,tbc;
  number cleft,tleft,bdis,ztop,ct,cz,ww;
  geant pit,piz,pmdis,dist,pmsiz,pxsiz,fr;
  geant fshift,effmn,deffmx,slope;
  cid[0]=0;
  cid[1]=0;
  cid[2]=0;
  cid[3]=0;
  nfl=_slstruc[1];// numb.of fiber-layers per super-layer
  npm=_slstruc[3];// numb.of PM's per super-layer
  pit=_fpitch[0];// fiber pitch(transv)
  piz=_fpitch[1];// fiber pitch(in z)
  pmsiz=_rdcell[4]*2.;//PM_size
  pxsiz=_rdcell[4];// SubCell(pixel) size
  fr=_rdcell[3]/2;//   fiber radious 
  fidd=fid/1000;
  fff=fid%1000-1;//fiber number (0-...)
  ll=fidd%100-1;//fiber layer number (0-9) 
  ss=fidd/100;//super-layer number (0-8)
  fshift=_gendim[10+ss];//fibers shift from ideal(simmetric) position
  effmn=ECMCFFKEY.effmn;//pm eff. at the edge of pixel
  deffmx=ECMCFFKEY.deffw*_rdcell[4];//dist.from edge where the efficiency reach 1.
  slope=(1-effmn)/deffmx;
  ip=ll%2;
  if(ip==0)cleft=-(_nfibpl[0]-1)*pit/2.;//     fiber from 1st layer of s-layer
  else cleft=-(_nfibpl[1]-1)*pit/2.;//         fiber from 2nd layer of s-layer
  cleft+=fshift;//correction for fibers set shift (all shifted by the same value)
  //("out-of-lead_volume" case is impossible due to lead size and limited max.number of fibers)
  ct=cleft+fff*pit;//       transv.coord. of fiber in ECAL r(eference) s(ystem)
  ztop=(nfl-1)*piz/2.;//       z-pos of 1st(from top) f-layer of s-layer
  cz=ztop-ll*piz;//      z-pos of fiber in Slayer r.s.(z=0->middle of super-layer)
  tleft=-npm*pmsiz/2.;//     low-edge PM-bin transv.position in ECAL r.s.
  dist=ct-tleft;//           fiber-center dist from the 1st PM (its left edge)
  //    cout <<fid<<" "<<pm<<endl;
  if(dist<=-fr)return;//out of sensitive area (left side)
  if((dist-npm*pmsiz)>=fr)return;//out of sensitive area (right side)
  pm=integer(floor(fabs(dist)/pmsiz));
  if(pm==npm)pm-=1;//   number of fired PM  0-(npm-1)   (IMPLY pmpit=pmsiz !!!)
  //
  if(cz>fr)tbc=0;// below i imply no sharing in Z (even number of f-layers/s-layer)
  else if(cz<-fr)tbc=2;
  else {
    cerr<<"ECALDBc::fid2cida: bad fiber-PM matching in Z, cz="<<cz<<endl;
    exit(1);;
  }
  //
  pmdis=dist-pm*pmsiz;//   fiber-center dist from the left edge of current(pm) PM
  if(pmdis<fr)bran=1;
  else if(pmdis>(pmsiz-fr))bran=2;
  else bran=3;
  switch(bran){
  case 1:  //<-- near the left PM-boundary
    cell=0+tbc;
    bdis=pmdis;//><0(<0 possible only for 1st PM special case)
    ww=segarea(fr,bdis);
    if(bdis<0){//special case: fiber is outside 1st PM, butslightly(<=fr) overlape with its left side
      w[0]=ww;//smaller overlape area 
      cid[0]=1000*ss+(pm+1)*10+(cell+1);
      w[0]*=effmn;//fixed at pixel edge level (0.6)
      return;	    
    }
    //bdis>0(normal case):
    w[0]=1.-ww;
    cid[0]=1000*ss+(pm+1)*10+(cell+1);
    if(bdis<deffmx)w[0]*=(effmn+slope*bdis);//corr.for pixel eff.uniformity
    if(pm==0)return;// 1st PM
    w[1]=1.-w[0];
    cell=1+tbc;
    cid[1]=1000*ss+(pm)*10+(cell+1);// left neighbour
    w[1]*=effmn;//fixed 
    break;
    //        
  case 2:  //<-- near the right PM-boundary
    cell=1+tbc;
    bdis=pmsiz-pmdis;//><0(<0 only for last PM special case)
    ww=segarea(fr,bdis);
    if(bdis<0){//special case: fiber is outside of last PM, but slightly(<=fr) overlape with its right side
      w[0]=ww;//smaller overlape area
      cid[0]=1000*ss+(pm+1)*10+(cell+1);
      w[0]*=effmn;//fixed
      return;
    }
    //bdis>0(normal case):
    w[0]=1.-ww;
    cid[0]=1000*ss+(pm+1)*10+(cell+1);
    if(bdis<deffmx)w[0]*=(effmn+slope*bdis);//corr.for pixel eff.uniformity
    if(pm==(npm-1))return;// last PM
    w[1]=1.-w[0];
    cell=0+tbc;
    cid[1]=1000*ss+(pm+2)*10+(cell+1);// right neighbour
    w[1]*=effmn;//fixed 
    break;
    //        
  case 3:  //<-- completely inside PM
    bdis=pmdis-pxsiz;//<>0, f-center dist from vertical boundary of 2 cells of given PM
    if(bdis<=-fr){// <-- completely in the left half of PM
      cell=0+tbc;
      cid[0]=1000*ss+(pm+1)*10+(cell+1);
      w[0]=1.;
      if(-bdis<deffmx)w[0]*=(effmn-slope*bdis);//corr.for pixel eff.uniformity (at right side)
      else if((pxsiz+bdis)<deffmx)w[0]*=(effmn+slope*(pxsiz+bdis));//..........(at left side)
    }
    else if(bdis>=fr){//>0, <-- completely in the right half of PM
      cell=1+tbc;
      cid[0]=1000*ss+(pm+1)*10+(cell+1);
      w[0]=1.;
      if(bdis<deffmx)w[0]*=(effmn+slope*bdis);//corr.for pixel eff.uniformity(at left side)	  
      else if((pxsiz-bdis)<deffmx)w[0]*=(effmn+slope*(pxsiz-bdis));//........(at rigth side)
    }
    else{ // <-- left-right sharing (|bdis|<fr)
      ww=segarea(fr,bdis);
      cell=0+tbc;
      cid[0]=1000*ss+(pm+1)*10+(cell+1);
      cell=1+tbc;
      cid[1]=1000*ss+(pm+1)*10+(cell+1);
      if(bdis<0.){//bigger area is in left pix
	w[0]=1.-ww;	    
	if(-bdis<deffmx)w[0]*=(effmn-slope*bdis);//corr.for pixel eff.uniformity
	w[1]=ww;
	w[1]*=effmn;//fixed	    
      }
      else{//bigger area is in right pix
	w[0]=ww;	    
	w[0]*=effmn;//fixed	    
	w[1]=1.-ww;	    
	if(bdis<deffmx)w[1]*=(effmn+slope*bdis);//corr.for pixel eff.uniformity
      }
    }
    break;
    //
  default:
    cerr<<"ECALDBc::fid2cida: problem with PM/Fiber transv.matching, pmdis="<<pmdis<<endl;
    exit(1);
  }
}
//
//---------
// <--- function to get info about SubCell("analog" design) :
//
//input: isl->S-layer(0-...);pmc->PMCell(0-...);sc->SubCell(0-3)
//output:pr->Proj(0-X,1-Y);pl->Plane(0-..);cell->0-...;ct/l/z-coord. in AMS.r.s
//
//  PmCells/SubCells(pixels) numbering:
//                |0|1|
// PM1(at -X/Y)>> ----- >>PM36(at +X/Y)
//                |2|3|
//
void ECALDBc::getscinfoa(integer isl, integer pmc, integer sc,
			 integer &pr, integer &pl, integer &cell, number &ct, number &cl, number &cz){
  //
  int i,ip,sl,fl,fi,pm,nf[4];
  number z,z11,t,tleft;
  static int first=0;
  static number coot[ECPMSMX][4];
  static number cooz[ECSLMX][4];
  //
  int nsl=_slstruc[2];// numb.of super-layers
  int nfl=_slstruc[1];// numb.of fiber-layers per super-layer
  int npm=_slstruc[3];// numb.of PM's per super-layer
  geant dzrad1=_gendim[8];// z-size of 1SL radiator(lead)
  geant alpth=_gendim[9];// glue(on rad. top/bot) thickness
  geant pmpit=_rdcell[6];//  PM-pitch(transv)
  geant pxsiz=_rdcell[4];//  SubCell(pixel) size
  geant piz=_fpitch[1];//  fibers pitch in z
  geant pizz,dz;
  //
#ifdef __AMSDEBUG__
  if(ECALDBc::debug){
    assert(isl>=0 && isl<nsl);
    assert(pmc>=0 && isl<npm);
    assert(sc>=0 && sc<4);
  }
#endif
  //
  if(first==0){// <--- first call actions(prepare some static tables):
    if(nfl%2 != 0){// imply even number (no fiber sharing between top/bot pixels)
      cerr<<"ECALDBc::getscinfoa: wrong number of fiber layers per super layer, nfl="<<nfl<<endl;
      exit(1);
    }
    first=1;
    pizz=dzrad1-(nfl-1)*piz+2.*alpth;// fib-layer dist. in two adjacent super-layers
    dz=(dzrad1+2.*alpth)*nsl;//EC rad. total thickness(incl. Al-plates, excl. Honeyc)
    z11=(nsl*(nfl-1)*piz+(nsl-1)*pizz)/2.;//zpos(ECAL r.s.) of 1st f-layer of 1st S-layer
    z=z11+(_gendim[6]-dz/2.);//zpos(AMS r.s.) of 1st f-layer of 1st S-layer
    //      cout<<"ECALDBc::getscinfoa: 1st fiber-layer Zpos="<<z<<endl;
    for(sl=0;sl<nsl;sl++){// <--- S-layer loop for Z-calc.
      for(i=0;i<4;i++){
	nf[i]=0;
	cooz[sl][i]=0.;
      }
      for(fl=0;fl<nfl;fl++){// <--- F-layer loop
	if(fl < nfl/2)fi=1;
	else fi=3;
	nf[fi-1]+=1;
	cooz[sl][fi-1]+=z;
	nf[fi]+=1;
	cooz[sl][fi]+=z;
	z-=piz;
      }
      z+=piz;//remove extra piz-shift
      z-=pizz;//add pizz-shift while going from one s-layer to the next one
      for(i=0;i<4;i++)cooz[sl][i]/=geant(nf[i]);// z-COG's (AMS r.s.)
    }
    // <--- now calc. tranv. COG's for all SubCells of ONE S-layer:
    //      (neglecting fiber sharing effects)
    //
    tleft=-(npm-1)*pmpit/2.;//1st PM-center transv.position in ECAL r.s.
    for(pm=0;pm<npm;pm++){// <--- PMCell loop for t-coo. calc.
      t=tleft+pm*pmpit;
      coot[pm][0]=t-pxsiz/2;
      coot[pm][2]=coot[pm][0];
      coot[pm][1]=t+pxsiz/2;
      coot[pm][3]=coot[pm][1];
    }
#ifdef __AMSDEBUG__
    cout<<"ECALDBc::getscinfo-test:"<<endl;
    cout<<"1st SubCell in 1st plane coot(EC r.s.)/cooz="<<coot[0][0]<<" "<<cooz[0][0]<<endl;      
    cout<<"last SubCell in 1st plane coot(EC r.s.)/cooz="<<coot[npm-1][1]<<" "<<cooz[0][1]<<endl;      
    cout<<"1st SubCell in 2nd plane coot(EC r.s.)/cooz="<<coot[0][2]<<" "<<cooz[0][2]<<endl;      
    cout<<"1st SubCell in last plane coot(EC r.s.)/cooz="<<coot[0][2]<<" "<<cooz[nsl-1][2]<<endl;      
    cout<<"last SubCell in last plane coot(EC r.s.)/cooz="<<coot[npm-1][3]<<" "<<cooz[nsl-1][3]<<endl;      
    //
#endif
  }
  //----------- each call actions :
  else{
    ip=isl%2;
    if(ip==0)pr=_slstruc[0];// proj (0/1->X/Y)
    else pr=1-_slstruc[0];
    pl=isl*2+(sc/2);// plane (continious numbering of SubCell-planes over 2 proj)
    cell=pmc*2+(sc%2);// Cell in SubCell-plane
    cz=cooz[isl][sc];// z-coo (AMS r.s.)
    ct=coot[pmc][sc];// transv.coo (ECAL r.s.)
    if(pr==0){
      ct+=_gendim[4];//t-coo for X-proj (AMS r.s.)
      cl=_gendim[5];// l-coo .....
    }
    else{
      ct+=_gendim[5];//t-coo for Y-proj (AMS r.s.)
      cl=_gendim[4];// l-coo .....
    }
  }
  // 
}
//----------------
// this function return Cell abs.coo: Ctransv/Clongit/Cz->ico=0/1/2;
// plane:0-...; cell:0-...; coo: in cm
number ECALDBc::CellCoo(integer plane, integer cell, integer ico){
  integer isl,pmc,sc,pr,pl,cel;
  number ct,cl,cz,coo(0);
  isl=plane/2;
  pmc=cell/2;
  if(plane%2==0)sc=cell%2;
  else sc=cell%2+2;
  ECALDBc::getscinfoa(isl, pmc, sc, pr, pl, cel, ct, cl, cz);
  if(ico==0)coo=ct;
  if(ico==1)coo=cl;
  if(ico==2)coo=cz;
  return coo;
}


number ECALDBc::CellSize(integer plane, integer cell, integer ico){
  // icoo - 0 transverse
  //        1  longit
  //        2  z
  if(ico==0){    //transverse(dx=dy)
    return _rdcell[4];
  }
  else if(ico==1){  //longit
    return _gendim[0];//lx=ly(imply square form of EC-radiator)
  }
  else{
    return _rdcell[4];//dx=dy=dz(square form of readout cell)
  }
}



//==========================================================================
//
//   EcalJobStat static variables definition (memory reservation):
//
integer EcalJobStat::mccount[ECJSTA];
integer EcalJobStat::recount[ECJSTA];
integer EcalJobStat::cacount[ECJSTA];
integer EcalJobStat::srcount[20];
number EcalJobStat::zprmc1[ECSLMX];// mc-hit average Z-profile(SL-layers) 
number EcalJobStat::zprmc2[ECSLMX];// mc-hit(+att) average Z-profile(SL(PM-assigned)-layers) 
number EcalJobStat::zprofa[2*ECSLMX];// average Z-profile(scPlanes) 
number EcalJobStat::zprofapm[ECSLMX];// average Z-profile(pm-layers) 
number EcalJobStat::zprofac[ECSLMX];// SL Edep profile (punch-through events)
geant EcalJobStat::nprofac[ECSLMX];// SL profile (punch-through events)
integer EcalJobStat::daqc1[ECJSTA];//daq-decoding counters
integer EcalJobStat::daqc2[ecalconst::ECRT][ECJSTA];
integer EcalJobStat::daqc3[ecalconst::ECRT][ECSLOTS][ECJSTA];
//
//------------------------------------------
void EcalJobStat::clear(){
  int i,j,k;
  for(i=0;i<ECJSTA;i++){
    daqc1[i]=0;
    for(j=0;j<ECRT;j++){
      daqc2[j][i]=0;
      for(k=0;k<ECSLOTS;k++){
        daqc3[j][k][i]=0;
      }
    }
  }
  for(i=0;i<ECJSTA;i++)mccount[i]=0;
  for(i=0;i<ECJSTA;i++)recount[i]=0;
  for(i=0;i<ECJSTA;i++)cacount[i]=0;
  for(i=0;i<20;i++)srcount[i]=0;
  for(i=0;i<ECSLMX;i++)zprmc1[i]=0;
  for(i=0;i<ECSLMX;i++)zprmc2[i]=0;
  for(i=0;i<2*ECSLMX;i++)zprofa[i]=0.;
  for(i=0;i<ECSLMX;i++)zprofapm[i]=0.;
  for(i=0;i<ECSLMX;i++)zprofac[i]=0.;
  for(i=0;i<ECSLMX;i++)nprofac[i]=0;
}
//---
void EcalJobStat::daqs1(integer info){
#ifdef __AMSDEBUG__
  assert(info>=0 && info<ecalconst::ECJSTA );
#endif
#pragma omp critical (ec_daqs1) 
  daqc1[info]+=1;
}
//
void EcalJobStat::daqs2(int16u crat, integer info){
#ifdef __AMSDEBUG__
  assert(info>=0 && info< ecalconst::ECJSTA);
  assert(crat< ecalconst::ECRT);
#endif
#pragma omp critical (ec_daqs2) 
  daqc2[crat][info]+=1;
}
//
void EcalJobStat::daqs3(int16u crat, int16u slot, integer info){
#ifdef __AMSDEBUG__
  assert(info>=0 && info< ecalconst::ECJSTA);
  assert(crat< ecalconst::ECRT);
  assert(slot< ecalconst::ECSLOTS);
#endif
#pragma omp critical (ec_daqs3) 
  daqc3[crat][slot][info]+=1;
}
//---
void EcalJobStat::addmc(int i){
#ifdef __AMSDEBUG__
  assert(i>=0 && i< ecalconst::ECJSTA);
#endif
#pragma omp critical (ec_addmc) 
  mccount[i]+=1;
}
//
void EcalJobStat::addre(int i){
#ifdef __AMSDEBUG__
  assert(i>=0 && i< ecalconst::ECJSTA);
#endif
#pragma omp critical (ec_addre) 
  recount[i]+=1;
}
//
void EcalJobStat::addca(int i){
#ifdef __AMSDEBUG__
  assert(i>=0 && i<  ecalconst::ECJSTA);
#endif
#pragma omp critical (ec_addca) 
  cacount[i]+=1;
}
//
void EcalJobStat::addsr(int i){
#ifdef __AMSDEBUG__
  assert(i>=0 && i< 20);
#endif
#pragma omp critical (ec_addsr) 
  srcount[i]+=1;
}
//------------------------------------------
// function to print Job-statistics at the end of JOB(RUN):
void EcalJobStat::printstat(){
  //
  int crt,fmt;
  //
  printf("\n");
  printf("    =================== ECAL-JOB decoding report ==================\n");
  printf("\n");
  if(daqc1[0]>0){//Normal Data
    printf("     DAQ-decoding report for Data :\n");
    printf("JINFs entries                      : % 7d\n",daqc1[0]);
    printf(" ...........valid, non empty       : % 7d\n",daqc1[1]);
    printf("\n");
    printf("JINF_1/2 notData-type(ignored)     : % 8d % 8d\n",daqc1[2],daqc1[3]);
    printf("\n");
    printf("ReplyStatus:   CRCerr  ASSMerr  AMSWerr  TimeOut FEPOWerr   SEQerr  CDPnode:\n");
    printf("     JINF_1: %8d %8d %8d %8d %8d %8d %8d\n",
	   daqc1[10],daqc1[11],daqc1[12],daqc1[13],daqc1[14],daqc1[15],daqc1[16]); 
    printf("     JINF_2: %8d %8d %8d %8d %8d %8d %8d\n\n",
	   daqc1[17],daqc1[18],daqc1[19],daqc1[20],daqc1[21],daqc1[22],daqc1[23]); 
    printf("JINF_1/2 reply-status bits OK      : % 8d %8d\n",daqc1[4],daqc1[5]);
    printf("\n");
    printf("Finally rejected JINFs(any reasons)               : %7d\n",daqc1[ECJSTA-1]);
    printf("   caused by JINF's badID                         : %7d\n",daqc1[30]);
    printf("   caused by JINF's Empty                         : %7d\n",daqc1[31]);
    printf("   caused by JINF's repl.stat.err(cr1/2)          : %7d %7d\n",daqc1[32],daqc1[33]);
    printf("   caused by JINF's any ped-block wrong length    : %7d %7d\n",daqc1[34],daqc1[35]);
    printf("   caused by JINF's ETRG-block bad length         : %7d %7d\n",daqc1[36],daqc1[37]);
    printf("   caused by JINF's any EDR-block bad length(raw) : %7d %7d\n",daqc1[38],daqc1[39]);
    printf("   caused by JINF's any EDR-block bad length(com) : %7d %7d\n",daqc1[40],daqc1[41]);
    printf("   caused by JINF's any EDR-block bad length(mix) : %7d %7d\n",daqc1[42],daqc1[43]);
    printf("\n");
    printf(" Crate(JINF) sub-blocks statistics,  crate-1:   crate-2:\n");
    printf("\n");
    printf(" RawFMT or OnBoardPed blocks found : % 7d    % 7d\n",daqc2[0][0],daqc2[1][0]);
    printf(" ComprFMT blocks found             : % 7d    % 7d\n",daqc2[0][1],daqc2[1][1]);
    printf(" MixtFMT blocks found              : % 7d    % 7d\n",daqc2[0][2],daqc2[1][2]);
    printf(" Illegal block IDs(rawfmt)         : % 7d    % 7d\n",daqc2[0][3],daqc2[1][3]);
    printf(" Illegal block IDs(compfmt)        : % 7d    % 7d\n",daqc2[0][4],daqc2[1][4]);
    printf(" Illegal block IDs(mixtfmt)        : % 7d    % 7d\n",daqc2[0][5],daqc2[1][5]);
    printf("\n");
    printf("\n");
    //
    for(crt=0;crt<2;crt++){
      if(daqc1[crt]>0){
	cout<<"====> Crate-"<<crt+1<<" statistics slot-by-slot:"<<endl<<endl;
	cout<<" RawFMT(OnBPedTable)-entries: ";
	for(int sl=0;sl<7;sl++)cout<<setw(7)<<daqc3[crt][sl][0];
	cout<<endl;
	cout<<" CompFMT-entries            : ";
	for(int sl=0;sl<7;sl++)cout<<setw(7)<<daqc3[crt][sl][1];
	cout<<endl;
	cout<<" MixFMT-entries             : ";
	for(int sl=0;sl<7;sl++)cout<<setw(7)<<daqc3[crt][sl][2];
	cout<<endl;
	cout<<" BadFMT-entries             : ";
	for(int sl=0;sl<7;sl++)cout<<setw(7)<<daqc3[crt][sl][6];
	cout<<endl;
	cout<<" BadDataTypeBit-entries     : ";
	for(int sl=0;sl<7;sl++)cout<<setw(7)<<daqc3[crt][sl][3];
	cout<<endl;
	cout<<" DownScaled-entries         : ";
	for(int sl=0;sl<7;sl++)cout<<setw(7)<<daqc3[crt][sl][4];
	cout<<endl;
	cout<<" PedBlk-length OK           : ";
	for(int sl=0;sl<7;sl++)cout<<setw(7)<<daqc3[crt][sl][5];
	cout<<endl<<endl;
	//     
	for(fmt=0;fmt<3;fmt++){
	  if(daqc2[crt][fmt]>0){
	    if(fmt==0)cout<<" -----> RawFormat decoding in details: "<<endl;
	    if(fmt==1)cout<<" -----> CompFormat decoding in details: "<<endl;
	    if(fmt==2)cout<<" -----> MixtFormat decoding in details: "<<endl;
	    cout<<" CRC-error        : ";
	    for(int sl=0;sl<7;sl++)cout<<setw(7)<<daqc3[crt][sl][7+12*fmt];
	    cout<<endl;
	    cout<<" Assembl-error    : ";
	    for(int sl=0;sl<7;sl++)cout<<setw(7)<<daqc3[crt][sl][8+12*fmt];
	    cout<<endl;
	    cout<<" AMSwire-error    : ";
	    for(int sl=0;sl<7;sl++)cout<<setw(7)<<daqc3[crt][sl][9+12*fmt];
	    cout<<endl;
	    cout<<" TimeOut-error    : ";
	    for(int sl=0;sl<7;sl++)cout<<setw(7)<<daqc3[crt][sl][10+12*fmt];
	    cout<<endl;
	    cout<<" FEpower-error    : ";
	    for(int sl=0;sl<7;sl++)cout<<setw(7)<<daqc3[crt][sl][11+12*fmt];
	    cout<<endl;
	    cout<<" Sequencer-error  : ";
	    for(int sl=0;sl<7;sl++)cout<<setw(7)<<daqc3[crt][sl][12+12*fmt];
	    cout<<endl;
	    cout<<" EmptySlot        : ";
	    for(int sl=0;sl<7;sl++)cout<<setw(7)<<daqc3[crt][sl][13+12*fmt];
	    cout<<endl;
	    cout<<" CDPnodeBit set   : ";
	    for(int sl=0;sl<7;sl++)cout<<setw(7)<<daqc3[crt][sl][14+12*fmt];
	    cout<<endl;
	    cout<<" All StatusBits OK: ";
	    for(int sl=0;sl<7;sl++)cout<<setw(7)<<daqc3[crt][sl][15+12*fmt];
	    cout<<endl;
	    cout<<" BlockLength OK   : ";
	    for(int sl=0;sl<7;sl++)cout<<setw(7)<<daqc3[crt][sl][16+12*fmt];
	    cout<<endl;
	    if(fmt==2){
	      cout<<" !=0 ComSubl(Mixt): ";
	      for(int sl=0;sl<7;sl++)cout<<setw(7)<<daqc3[crt][sl][17+12*fmt];
	      cout<<endl;
	    }
	    if(fmt>0){
	      cout<<" Wrong ADC-address: ";
	      for(int sl=0;sl<7;sl++)cout<<setw(7)<<daqc3[crt][sl][18+12*fmt];
	      cout<<endl;
	    }
	    cout<<endl;
	  }
	}//--->endof formats loop
	cout<<endl;
      }
    }//--->endof crates loop
  }//--->endof decoding-stat-print check for Data
  //--------
  if(daqc1[50]>0){//OnBoard Ped-calibration
    printf("     DAQ-decoding report for OnBoard Pedestals :\n");
    printf("Total EDR decoding entries                      : % 7d\n",daqc1[50]);
    printf("Requested EDRs                                  : % 7d\n",daqc1[51]);
    printf("Rejected EDRs                                   : % 7d\n",daqc1[ECJSTA-1]);
    cout<<endl;
    for(crt=0;crt<2;crt++){
      cout<<"====> Crate-"<<crt+1<<" statistics slot-by-slot:"<<endl<<endl;
      cout<<" Found&Requested entries      : ";
      for(int sl=0;sl<7;sl++)cout<<setw(7)<<daqc3[crt][sl][0];
      cout<<endl;
      cout<<" Bad CalStatus (fatal)        : ";
      for(int sl=0;sl<7;sl++)cout<<setw(7)<<daqc3[crt][sl][1];
      cout<<endl;
      cout<<" SHT crazy bits found         : ";
      for(int sl=0;sl<7;sl++)cout<<setw(7)<<daqc3[crt][sl][2];
      cout<<endl;
      cout<<" BadBlockLength(fatal)        : ";
      for(int sl=0;sl<7;sl++)cout<<setw(7)<<daqc3[crt][sl][3];
      cout<<endl;
      cout<<" BlocKlLength/CalStatUS OK    : ";
      for(int sl=0;sl<7;sl++)cout<<setw(7)<<daqc3[crt][sl][4];
      cout<<endl;
      cout<<" DataFormat OK                : ";
      for(int sl=0;sl<7;sl++)cout<<setw(7)<<daqc3[crt][sl][5];
      cout<<endl;
      cout<<" No Errors in EDR repl.status : ";
      for(int sl=0;sl<7;sl++)cout<<setw(7)<<daqc3[crt][sl][6];
      cout<<endl<<endl;
      //     
      int fmt=0;
      if(daqc1[52+crt]>0){
	cout<<" -----> Bad EDR reply status in details: "<<endl;
	cout<<" CRC-error        : ";
	for(int sl=0;sl<6;sl++)cout<<setw(7)<<daqc3[crt][sl][7+12*fmt];
	cout<<endl;
	cout<<" Assembl-error    : ";
	for(int sl=0;sl<7;sl++)cout<<setw(7)<<daqc3[crt][sl][8+12*fmt];
	cout<<endl;
	cout<<" AMSwire-error    : ";
	for(int sl=0;sl<7;sl++)cout<<setw(7)<<daqc3[crt][sl][9+12*fmt];
	cout<<endl;
	cout<<" TimeOut-error    : ";
	for(int sl=0;sl<7;sl++)cout<<setw(7)<<daqc3[crt][sl][10+12*fmt];
	cout<<endl;
	cout<<" FEpower-error    : ";
	for(int sl=0;sl<7;sl++)cout<<setw(7)<<daqc3[crt][sl][11+12*fmt];
	cout<<endl;
	cout<<" Sequencer-error  : ";
	for(int sl=0;sl<7;sl++)cout<<setw(7)<<daqc3[crt][sl][12+12*fmt];
	cout<<endl;
	//       cout<<" EmptySlot        : ";
	//       for(int sl=0;sl<7;sl++)cout<<setw(7)<<daqc3[crt][sl][13+12*fmt];
	//       cout<<endl;
	cout<<" CDPnodeBit set   : ";
	for(int sl=0;sl<7;sl++)cout<<setw(7)<<daqc3[crt][sl][14+12*fmt];
	cout<<endl;
	cout<<" All StatusBits OK: ";
	for(int sl=0;sl<7;sl++)cout<<setw(7)<<daqc3[crt][sl][15+12*fmt];
	cout<<endl;
	cout<<" BlockLength OK   : ";
	for(int sl=0;sl<7;sl++)cout<<setw(7)<<daqc3[crt][sl][16+12*fmt];
	cout<<endl;
	cout<<endl;
      }
    }//--->endof crates loop
    return;//don't need rec0-report
  }
  //
  printf("\n");
  //
  if(MISCFFKEY.dbwrbeg>0)return;//dbwriter job, don't need statistics print
  //
  printf("\n");
  printf("    =================== ECAL-JOB reconstruction report ==================\n");
  printf("\n");
  printf(" MC: entries                          : % 6d\n",mccount[0]);
  printf(" MC: MCHit->RawEven(ECTrigfl>0) OK    : % 6d\n",mccount[1]);
  printf(" MCTrigBuild: entries                 : % 6d\n",srcount[0]);
  printf("       !=0 TrigHitsMult in any proj   : % 6d\n",srcount[1]);
  printf("       1proj at 2proj FTE-requir.     : % 6d\n",srcount[2]);
  printf("       FTE(energy=mult) requir.   OK  : % 6d\n",srcount[3]);
  printf("       LVL1(Angle) requir.        OK  : % 6d\n",srcount[4]);
  printf(" RECO-entries                         : % 6d\n",recount[0]);
  printf(" GlobFT(FTC|FTZ|FTE|Ext) found at LVL1: % 6d\n",recount[1]);
  printf(" ECAL_FT(FTE) found at LVL1           : % 6d\n",recount[2]);
  printf(" TOF_FT & ECAL_FT(FTE) found at LVL1  : % 6d\n",recount[3]);
  printf(" Validation: LVL1 OK                  : % 6d\n",recount[7]);
  printf(" Validation: EC_FT OK                 : % 6d\n",recount[8]);
  printf(" Validation: ECFlg>0                  : % 6d\n",recount[9]);
  printf(" Validation OK                        : % 6d\n",recount[4]);
  printf(" RawEvent->EcalHit OK                 : % 6d\n",recount[5]);
  printf(" EcalHit->EcalCluster OK              : % 6d\n",recount[6]);
  printf(" CatastrRearLeak detected             : % 6d\n",recount[10]);
  number rrr(0);
  if(recount[5]>0)rrr=number(srcount[10])/number(recount[5]);
  printf(" Saturated PMTs per EcalHitOK-event: % 6.4f\n",rrr);
  printf("\n\n");
  if(ECREFFKEY.relogic[1]==1 || ECREFFKEY.relogic[1]==2){
    printf("    ============== RLGA/FIAT part of REUN_Calibration-statistics ===============\n");
    printf("\n");
    printf(" REUN: entries(tof/ec_trigflag OK)   : % 6d\n",cacount[0]);
    printf(" REUN: TrkTrack found                : % 6d\n",cacount[1]);
    printf(" REUN: Track charge OK               : % 6d\n",cacount[2]);
    printf(" REUN: Track quality OK              : % 6d\n",cacount[3]);
    printf(" REUN: Track hits EC                 : % 6d\n",cacount[4]);
    printf(" REUN: Nhits match PunchTrough       : % 6d\n",cacount[9]);
    printf(" REUN: Pix/Planes Edep/pattern OK    : % 6d\n",cacount[5]);
    printf(" REUN: Finally selected(Etrunc=Pr/He): % 6d\n",cacount[6]);
    printf(" REUN: Used by Calib(>=1 PMs matched): % 6d\n",cacount[7]);
    printf("\n\n");
  }
  if(ECREFFKEY.relogic[1]==3){
    printf("    ============== ANOR part of REUN_Calibration-statistics ===============\n");
    printf("\n");
    printf(" REUN: entries(tof/ec_trigflag OK) : % 6d\n",cacount[0]);
    printf(" REUN: ANTI OK                     : % 6d\n",cacount[1]);
    printf(" REUN: Track found                 : % 6d\n",cacount[2]);
    printf(" REUN: Track quality OK            : % 6d\n",cacount[3]);
    printf(" REUN: Track hits ECAL             : % 6d\n",cacount[4]);
    printf(" REUN: FiredSubCells pattern OK    : % 6d\n",cacount[5]);
    printf(" REUN: Efront/Epeak/Etail/Etot OK  : % 6d\n",cacount[6]);
    printf(" REUN: Plane1/6 COG matching OK    : % 6d\n",cacount[7]);
    printf(" REUN: Plane1/6 Ebcg/Esig OK       : % 6d\n",cacount[8]);
    printf("\n\n");
  }
  if(ECREFFKEY.relogic[1]==5){
    printf("    ============== DownScaledEvents PedCalibration-statistics ===============\n");
    printf(" Selected events                   : % 6d\n",recount[20]);
  }
  printf("    ===========================================================================\n");
  //
}
//------------------------------------------
void EcalJobStat::bookhist(){
  int maxcl,maxpl,maxsl;
  char inum[11];
  //
  maxpl=2*ECSLMX;//MAX planes
  maxcl=2*ECPMSMX;//MAX SubCells per plane
  maxsl=ECSLMX;
  strcpy(inum,"0123456789");
  if(LVL3FFKEY.histprf>0){// EC-lvl3 histograms
    HBOOK1(ECHISTR+31,"ECLVL3: Etot(mev)",100,0.,100000.,0.);
    HBOOK1(ECHISTR+32,"ECLVL3: Efront",80,0.,1600.,0.);
    HBOOK1(ECHISTR+33,"ECLVL3: Epeak/Ebase",80,0.,40.,0.);
    HBOOK1(ECHISTR+34,"ECLVL3: Epeak/Efront",80,0.,40.,0.);
    HBOOK1(ECHISTR+35,"ECLVL3: X-width",80,0.,80.,0.);
    HBOOK1(ECHISTR+36,"ECLVL3: Y-width",80,0.,80.,0.);
    HBOOK1(ECHISTR+37,"ECLVL3: PM-colx signals(mev)",80,0.,240.,0.);
    HBOOK1(ECHISTR+38,"ECLVL3: PM-coly signals(mev)",80,0.,240.,0.);
    HBOOK1(ECHISTR+39,"ECLVL3: EM-flag",3,-1.,2.,0.);
    HBOOK1(ECHISTR+40,"ECLVL3: Z-cog",50,-180.,-130.,0.);
    HBOOK1(ECHISTR+41,"ECLVL3: T-cog,sl1",80,-40.,40.,0.);
    HBOOK1(ECHISTR+42,"ECLVL3: T-cog,sl2",80,-40.,40.,0.);
    HBOOK1(ECHISTR+43,"ECLVL3: chi2y",50,0.,2.5,0.);
    HBOOK1(ECHISTR+44,"ECLVL3: tany",50,-1.,1.,0.);
    HBOOK1(ECHISTR+45,"ECLVL3: DxClosest",80,-80.,80.,0.);
    HBOOK1(ECHISTR+46,"ECLVL3: chi2x",50,0.,2.5,0.);
    HBOOK1(ECHISTR+47,"ECLVL3: tanx",50,-1.,1.,0.);
    HBOOK1(ECHISTR+48,"ECLVL3: DyClosest",80,-80.,80.,0.);
    HBOOK1(ECHISTR+49,"ECLVL3: SL1-rms",50,0.,20.,0.);
    HBOOK1(ECHISTR+50,"ECLVL3: SL2-rms",50,0.,20.,0.);
    HBOOK1(ECHISTR+51,"ECLVL3: SL8-rms",50,0.,20.,0.);
    HBOOK1(ECHISTR+52,"ECLVL3: SL9-rms",50,0.,20.,0.);
    HBOOK1(ECHISTR+53,"ECLVL3: Xhigh-Xlow",50,0.,50.,0.);
    HBOOK1(ECHISTR+54,"ECLVL3: Yhigh-Ylow",50,0.,50.,0.);
    HBOOK1(ECHISTR+55,"ECLVL3: Xec-Xtr",80,-80.,80.,0.);
    HBOOK1(ECHISTR+56,"ECLVL3: Yec-Ytr",80,-80.,80.,0.);
  }
  //---
  if(ECREFFKEY.reprtf[0]>1){ // Book reco-hist
    HBOOK1(ECHISTR+10,"ECRE::HitBuild: RawEvent-hits tot.number",80,0.,800.,0.);
    HBOOK1(ECHISTR+11,"ECRE::HitBuild: RawEvent-hits ADCtot(adcch,gain-corr)",200,0.,100000.,0.);
    HBOOK1(ECHISTR+12,"ECRE::HitBuild: RawEvent-hits ADCtot(adcch,gain-corr)",100,0.,2000.,0.);
    HBOOK1(ECHISTR+13,"ECRE::HitBuild: EcalHit-hits tot.number",80,0.,160.,0.);
    HBOOK1(ECHISTR+14,"ECRE::HitBuild: RawEvent-hits Etot(NoDynCorr,Mev)",200,0.,300000.,0.);
    HBOOK1(ECHISTR+9,"ECRE::HitBuild: EcalHit-hit Energy(Mev)",100,0.,100.,0.);
    HBOOK1(ECHISTR+15,"ECRE::HitBuild: DyCorrectionEn(tot,Mev)",100,0.,1000,0.);
    HBOOK1(ECHISTR+16,"ECRE::HitBuild: RawEvent-hit value(adc,gain-corr)",200,0.,4000.,0.);
    HBOOK1(ECHISTR+17,"ECRE::HitBuild: RawEvent-hit value(adc,gain-corr)",100,0.,200.,0.);
    //      HBOOK1(ECHISTR+18,"ECRE: EcalClust per event",60,0.,120.,0.);
    if(ECREFFKEY.reprtf[1]==1){//<--- to store t-profiles, z-prof
      HBOOK1(ECHISTR+19,"ECRE: T-prof in plane 8(X)",maxcl,1.,geant(maxcl+1),0.);
      HBOOK1(ECHISTR+20,"ECRE: T-prof in plane 9(Y)",maxcl,1.,geant(maxcl+1),0.);
      HBOOK1(ECHISTR+21,"ECRE: Z-profile",maxpl,1.,geant(maxpl+1),0.);
    }
    //      HBOOK1(ECHISTR+22,"ECRE: EcalClust value(tot,Mev)",200,0.,1000000.,0.);//now not my responsib.
    //      HBOOK1(ECHISTR+23,"ECRE: EcalClust value(tot,Mev)",100,0.,50000.,0.);
    //      HBOOK1(ECHISTR+24,"ECRE: SubCelLayer En-profile(ECHits)",maxpl,1.,geant(maxpl+1),0.);//not implemented
    //      HBOOK1(ECHISTR+25,"ECRE: SuperLayer En-profile(ECHits)",maxsl,1.,geant(maxsl+1),0.);
    HBOOK1(ECHISTR+28,"ECRE: TriggerPatternProjX(when FTE, valid-stage)",120,1.,121.,0.);
    HBOOK1(ECHISTR+29,"ECRE: TriggerPatternProjY(when FTE, valid-stage)",120,1.,121.,0.);
    HBOOK1(ECHISTR+30,"ECRE: ECTrigger flag(when ECTrigFlg>0, valid-stage)",40,0.,40.,0.);
    HBOOK1(ECHISTR+60,"DAQ: swid(LPP)=218, A1_hig(adcch)",80,-10.,790.,0.);
    HBOOK1(ECHISTR+61,"DAQ: swid(LPP)=218, A2_hig(adcch)",80,-10.,790.,0.);
    HBOOK1(ECHISTR+62,"DAQ: swid(LPP)=218, A3_hig(adcch)",80,-10.,790.,0.);
    HBOOK1(ECHISTR+63,"DAQ: swid(LPP)=218, A4_hig(adcch)",80,-10.,790.,0.);
    HBOOK1(ECHISTR+64,"DAQ: swid(LPP)=218, A1_log(adcch)",80,-10.,790.,0.);
    HBOOK1(ECHISTR+65,"DAQ: swid(LPP)=218, A2_log(adcch)",80,-10.,790.,0.);
    HBOOK1(ECHISTR+66,"DAQ: swid(LPP)=218, A3_log(adcch)",80,-10.,790.,0.);
    HBOOK1(ECHISTR+67,"DAQ: swid(LPP)=218, A4_log(adcch)",80,-10.,790.,0.);
    HBOOK1(ECHISTR+68,"DAQ: swid(LPP)=218, Dyn(adcch)",80,-10.,790.,0.);
    //      HBOOK1(ECHISTR+70,"DAQ: ComprFMT EDR-length, Crate_1/Slot_1",50,0.,50.,0.);
    //      HBOOK1(ECHISTR+71,"DAQ: ComprFMT EDR-length, Crate_1/Slot_2",50,0.,50.,0.);
    //      HBOOK1(ECHISTR+72,"DAQ: ComprFMT EDR-length, Crate_1/Slot_3",50,0.,50.,0.);
    //      HBOOK1(ECHISTR+73,"DAQ: ComprFMT EDR-length, Crate_1/Slot_4",50,0.,50.,0.);
    //      HBOOK1(ECHISTR+74,"DAQ: ComprFMT EDR-length, Crate_1/Slot_5",50,0.,50.,0.);
    //      HBOOK1(ECHISTR+75,"DAQ: ComprFMT EDR-length, Crate_1/Slot_6",50,0.,50.,0.);
    //      HBOOK1(ECHISTR+76,"DAQ: ComprFMT EDR-length, Crate_2/Slot_1",50,0.,50.,0.);
    //      HBOOK1(ECHISTR+77,"DAQ: ComprFMT EDR-length, Crate_2/Slot_2",50,0.,50.,0.);
    //      HBOOK1(ECHISTR+78,"DAQ: ComprFMT EDR-length, Crate_2/Slot_3",50,0.,50.,0.);
    //      HBOOK1(ECHISTR+79,"DAQ: ComprFMT EDR-length, Crate_2/Slot_4",50,0.,50.,0.);
    //      HBOOK1(ECHISTR+80,"DAQ: ComprFMT EDR-length, Crate_2/Slot_5",50,0.,50.,0.);
    //      HBOOK1(ECHISTR+81,"DAQ: ComprFMT EDR-length, Crate_2/Slot_6",50,0.,50.,0.);
    //
  }
  //----
  if(ECREFFKEY.relogic[1]==1 || ECREFFKEY.relogic[1]==2){// <==== RLGA/FIAT part of REUN-calibration
    if(ECCAFFKEY.hprintf>1){
      HBOOK1(ECHISTC,"ECCA: Track COS(theta) at EC front",100,-1.,1.,0.);
      HBOOK1(ECHISTC+37,"ECCA: Total hits energy(mev,prev.calib,Trk hits EC)",100,0.,1000.,0.);
      HBOOK1(ECHISTC+38,"ECCA: Total hits(above thr, when Trk hits EC)",100,0.,200.,0.);
      HBOOK1(ECHISTC+1,"ECCA: Track Imp.point X, SL1",70,-70.,70.,0.);
      HBOOK1(ECHISTC+2,"ECCA: Track Imp.point Y, SL1",70,-70.,70.,0.);
      //        HBOOK1(ECHISTC+3,"ECCA: PMCell-Track Transv-dist,SL1",50,-5.,5.,0.);
      //        HBOOK1(ECHISTC+4,"ECCA: PMCell-Track Transv-dist,SL2",50,-5.,5.,0.);
      HBOOK1(ECHISTC+5,"ECCA: Anode/Dynode(Gcorrected_4PixSum/Dyn)",100,0.,50.,0.);
      HBOOK1(ECHISTC+6,"ECCA: Track-fit Chi2 ",80,0.,800.,0.);
      HBOOK1(ECHISTC+12,"ECCA: Rigidity (gv)",100,-500.,500.,0.);
      HBOOK1(ECHISTC+16,"ECCA: TruncAverage  Edep/SLayer(PunchThrough,mev)",100,0.,250.,0.);
      HBOOK1(ECHISTC+17,"ECCA: Bad(non PunchThrough) PixLayers/event",maxpl+1,0.,geant(maxpl+1),0.);
      HBOOK2(ECHISTC+19,"ECCA: RefPmSc Alow vs Ahigh",100,50.,1050.,50,0.,50.,0.);//in fill_2
      HBOOK1(ECHISTC+24,"ECCA: EcalHit(pix) Energy(ECCrossByTrk,adc,gcorr)",100,0.,200.,0.);
      HBOOK1(ECHISTC+25,"ECCA: Fired(above thr) Pixels/PixLayer",80,0.,80.,0.);
      HBOOK1(ECHISTC+28,"ECCA: SuperLayers visibility(fired,punch-through)",maxsl,1.,geant(maxsl+1),0.);
      HBOOK1(ECHISTC+29,"ECCA: PMT(4pix-sum) spectrum(trk-matched pix,center,adc)",100,0.,500.,0.);
      HBOOK1(ECHISTC+30,"ECCA: PMT(4pix-sum) spectrum(trk-matched pix,center,adc,GainCorr)",100,0.,500.,0.);
      // test	HBOOK1(ECHISTC+33,"ECCA: TRK imppoint X-accur",60,-0.3,0.3,0.); 
      // test	HBOOK1(ECHISTC+34,"ECCA: TRK imppoint Y-accur",60,-0.3,0.3,0.); 
      HBOOK1(ECHISTC+35,"ECCA: Particle beta",96,-1.2,1.2,0.);
      HBOOK1(ECHISTC+36,"ECCA: Track Z(charge) (from tracker)",16,0.,16.,0.);
      HBOOK1(ECHISTC+39,"ECCA: Pixel-TrkCross mismatch(when fired, x-proj)",50,-2.5,2.5,0.);
      HBOOK1(ECHISTC+40,"ECCA: Pixel-TrkCross mismatch(when fired, y-proj)",50,-2.5,2.5,0.);
      HBOOK1(ECHISTC+41,"ECCA: RefPMT Pix-amplitudes(center,adc)",100,0,300.,0.);
      HBOOK1(ECHISTC+43,"ECCA: PmtDynode signals(adc-ch,when crossed)",100,0,100.,0.);
      HBOOK1(ECHISTC+44,"ECCA: AllPmt Pix-signals(adc-ch,when crossed)",100,0,200.,0.);
      HBOOK1(ECHISTC+45,"ECCA: AllPmt Pix-signals(adc-ch,GainCorr,when crossed)",100,0,200.,0.);
      HBOOK2(ECHISTC+46,"ECCA: Impact Extention vs SL",9,1.,10.,50,0.,1.,0.);
      HBOOK1(ECHISTC+47,"ECCA: Mom/mass beta",96,-1.2,1.2,0.);
    }
	
    //    hist # +7 is booked inside mfit !!!
    if(ECCAFFKEY.hprintf>0){
      HBOOK1(ECHISTC+115,"ECCA: AHadc,SL6,PM17",100,0.,100.,0.);
      HBOOK1(ECHISTC+116,"ECCA: AHadc,SL6,PM18",100,0.,100.,0.);
      HBOOK1(ECHISTC+117,"ECCA: Dadc,SL6,PM17",100,0.,100.,0.);
      HBOOK1(ECHISTC+118,"ECCA: Dadc,SL6,PM18",100,0.,100.,0.);

      HBOOK1(ECHISTC+8,"ECCA: Pixel Efficiency",60,0.,1.2,0.);
      HBOOK1(ECHISTC+9,"ECCA: Pixel RelativeGain",50,0.,2.,0.);
      HBOOK1(ECHISTC+10,"ECCA: PmtResp. vs LongBinNumber(uniformity)",ECCLBMX,1.,geant(ECCLBMX+1),0.);
      HBOOK1(ECHISTC+11,"ECCA: PMT(4pix-sum) relat.gains",100,0.,3.,0.);
      HBOOK1(ECHISTC+13,"ECCA: PMT RelGain SL-profile",ECSLMX,1.,geant(ECSLMX+1),0.);
      HMINIM(ECHISTC+13,0.5);
      HMAXIM(ECHISTC+13,1.5);
      if(ECCAFFKEY.hprintf>2){//low prior.hist
	HBOOK1(ECHISTC+14,"ECCA: Pixel Efficiency PixPlane-profile",maxpl,1.,geant(maxpl+1),0.);
	HBOOK1(ECHISTC+15,"ECCA: PMT Efficiency SL-profile",maxsl,1.,geant(maxsl+1),0.);
	if(ECCAFFKEY.prtuse==1){//He4
	  HMINIM(ECHISTC+14,0.4);
	  HMAXIM(ECHISTC+14,1.1);
	  HMINIM(ECHISTC+15,0.4);
	  HMAXIM(ECHISTC+15,1.1);
	}
	else{//prot
	  HMINIM(ECHISTC+14,0.4);
	  HMAXIM(ECHISTC+14,1.1);
	  HMINIM(ECHISTC+15,0.4);
	  HMAXIM(ECHISTC+15,1.1);
	}
	HBOOK1(ECHISTC+26,"ECCA: Pixel eff(even SL) ",60,0.,1.2,0.);
	HBOOK1(ECHISTC+27,"ECCA: Pixel eff( odd SL) ",60,0.,1.2,0.);
	HBOOK1(ECHISTC+31,"ECCA: PMT eff(even SL) ",60,0.,1.2,0.);
	HBOOK1(ECHISTC+32,"ECCA: PMT eff( odd SL) ",60,0.,1.2,0.);
      }
      HBOOK1(ECHISTC+18,"ECCA: SLayerEdep prof(punch-through)",maxsl,1.,geant(maxsl+1),0.);
      HBOOK1(ECHISTC+20,"ECCA: Slop(h2lcalib,all chan)",60,20.,50.,0.);
      HBOOK1(ECHISTC+21,"ECCA: Offs(h2lcalib,all chan)",40,-5.,5.,0.);
      HBOOK1(ECHISTC+22,"ECCA: Chi2(h2lcalib,all chan)",100,0.,5.,0.);
      HBOOK1(ECHISTC+42,"ECCA: Non0Bins(h2lcalib,all chan)",50,0.,100.,0.);
      //        HBOOK1(ECHISTC+23,"ECCA: LowChBinRMS/Aver(h2lcalib,all chan)",80,0.,0.25,0.);
      HBOOK1(ECHISTC+23,"ECCA: LowChBinRMS(h2lcalib,all chan)",80,0.,10.,0.);
      HBOOK1(ECHISTC+48,"ECCA: Anode(4pixSumGcorr)/Dynode ratio(final)",100,0.,50.,0.);
      HBOOK1(ECHISTC+49,"ECCA: AnodeAver(4pixSum,center,Gcorr)/EcAverTrunc(CalQuality)",100,0.5,1.5,0.);
      // hist # +50 is booked inside mfit !!!
      HBOOK1(ECHISTC+51,"ECCA: Pixel RelativeGain, GainCorrected",50,0.5,1.5,0.);
    }
  }
  //----
  if(ECREFFKEY.relogic[1]==3){// =====> ANOR part of REUN-calibration
    HBOOK1(ECHISTC,"ECCA: Track COS(theta) at EC front",100,-1.,1.,0.);
    HBOOK1(ECHISTC+1,"ECCA: Track Chi2(FastFit) ",80,0.,40.,0.);
    HBOOK1(ECHISTC+2,"ECCA: Track rigidity (Gv,FastFit)",100,2.,12.,0.);
    HBOOK1(ECHISTC+3,"ECCA: Track dR/R(FastFit)",100,0.,0.05,0.);
    HBOOK1(ECHISTC+4,"ECCA: Track half-rig. assimetry(AdvFit)",80,-0.4,0.4,0.);
    HBOOK1(ECHISTC+5,"ECCA: EcalHit Energy(in adc-units)",100,0.,100.,0.);
    HBOOK1(ECHISTC+6,"ECCA: Fired(above thr) SubCells/Layer",80,0.,80.,0.);
    HBOOK1(ECHISTC+7,"ECCA: Fired(above thr) SubCells/Layer1/2/3/4/5/6",120,0.,120.,0.);
    HBOOK1(ECHISTC+8,"ECCA: Bad SC-Layers(spikes,high multipl.",maxpl+1,0.,geant(maxpl+1),0.);
    HBOOK1(ECHISTC+9,"ECCA: Edep total(mev)",200,0.,20000.,0.);
    HBOOK1(ECHISTC+10,"ECCA: Side Eleak/Etot",100,0.,0.5,0.);
    HBOOK1(ECHISTC+11,"ECCA: Edep in 1st 4X0(mev)",100,0.,1000.,0.);
    HBOOK1(ECHISTC+12,"ECCA: Etail/Epeak",100,0.,1.,0.);
    HBOOK1(ECHISTC+13,"ECCA: Edep/Mom-1(Efr,Epeak/Etail cuts)",100,-1.,1.,0.);
    HBOOK1(ECHISTC+14,"ECCA: Max. dist of Track and SC",100,0.,50.,0.);
    HBOOK1(ECHISTC+15,"ECCA: Track-COG(planes 5,6) dist",100,-10.,10.,0.);
    HBOOK1(ECHISTC+16,"ECCA: Edep/Mom(all cuts)",100,0.,2.,0.);
    HBOOK1(ECHISTC+17,"ECCA: Back Eleak/Etot",100,0.,0.5,0.);
    HBOOK1(ECHISTC+18,"ECCA: Mom/Edep(all cuts)",100,0.,2.,0.);
    HBOOK1(ECHISTC+19,"ECCA: Max.value of SubCell ADC",100,0.,5000.,0.);
    HBOOK1(ECHISTC+20,"ECCA: SCplane1/2/3/4/5/6 holes",120,0.,120.,0.);
    HBOOK1(ECHISTC+21,"ECCA: Max. dist of Track and SC(pl 1)",100,0.,50.,0.);
    HBOOK1(ECHISTC+22,"ECCA: Track-COG(plane 1,2) dist",100,-10.,10.,0.);
    HBOOK1(ECHISTC+23,"ECCA: Track-COG(plane 3,4) dist",100,-10.,10.,0.);
    HBOOK1(ECHISTC+24,"ECCA: Spikes/plane",80,0.,80.,0.);
    HBOOK1(ECHISTC+25,"ECCA: Layers with early showering(holes)",10,0.,10.,0.);
    HBOOK1(ECHISTC+26,"ECCA: Edep(all cuts,attf corr,mev)",100,0.,20000.,0.);
    HBOOK1(ECHISTC+27,"ECCA: Edep(all cuts,no attf.corr,mev)",100,0.,20000.,0.);
    HBOOK1(ECHISTC+28,"ECCA: Rigidity(all cuts,mev)",100,2000.,12000.,0.);
    HBOOK1(ECHISTC+29,"ECCA: Z-profile(all cuts)",maxpl,0.,geant(maxpl),0.);
    HBOOK1(ECHISTC+30,"ECCA: Tracker backgr. Ycl/layer",50,0.,50.,0.);
    HBOOK1(ECHISTC+31,"ECCA: Tracker backgr. Xcl/layer",50,0.,50.,0.);
    HBOOK1(ECHISTC+32,"ECCA: Tracker X-layer backgr.",50,0.,10.,0.);
    HBOOK1(ECHISTC+33,"ECCA: Tracker Y-layer backgr.",50,0.,10.,0.);
    HBOOK1(ECHISTC+34,"ECCA: Track X-layer ampl.",80,0.,400.,0.);
    HBOOK1(ECHISTC+35,"ECCA: Track Y-layer ampl.",80,0.,400.,0.);
    HBOOK1(ECHISTC+36,"ECCA: Track rigidity (Gv,FastFit,BadClam)",100,2.,12.,0.);
    HBOOK1(ECHISTC+37,"ECCA: Track Xcross At ECTop",70,-35.,35.,0.);
    HBOOK1(ECHISTC+38,"ECCA: Track Ycross At ECTop",70,-35.,35.,0.);
    HBOOK1(ECHISTC+39,"ECCA: ANTI sector energy",80,0.,40.,0.);
    HBOOK1(ECHISTC+40,"ECCA: ANTI fired sectors",17,0.,17.,0.);
    HBOOK1(ECHISTC+41,"ECCA: Track rigidity (Gv,FastFit,TrackOK)",100,2.,12.,0.);
    HBOOK1(ECHISTC+42,"ECCA: Track Layer ampl.",80,0.,400.,0.);
    HBOOK1(ECHISTC+43,"ECCA: Track rigidity (Gv,FastFit,BadEbkg)",100,2.,12.,0.);
    HBOOK1(ECHISTC+44,"ECCA: Track beta",96,-1.2,1.2,0.);
    HBOOK1(ECHISTC+50,"ECCA: Dist. of Track and SC(pl1)",80,-8.,8.,0.);
    HBOOK1(ECHISTC+51,"ECCA: Dist. of Track and SC(pl2)",80,-8.,8.,0.);
    HBOOK1(ECHISTC+52,"ECCA: Dist. of Track and SC(pl3)",80,-8.,8.,0.);
    HBOOK1(ECHISTC+53,"ECCA: Dist. of Track and SC(pl4)",80,-8.,8.,0.);
    HBOOK1(ECHISTC+54,"ECCA: Dist. of Track and SC(pl5)",80,-8.,8.,0.);
    HBOOK1(ECHISTC+55,"ECCA: Dist. of Track and SC(pl6)",80,-8.,8.,0.);
    HBOOK1(ECHISTC+56,"ECCA: Emism/Ematch(pl1)",80,0.,4.,0.);
    HBOOK1(ECHISTC+57,"ECCA: Emism/Ematch(pl2)",80,0.,4.,0.);
    HBOOK1(ECHISTC+58,"ECCA: Emism/Ematch(pl3)",80,0.,4.,0.);
    HBOOK1(ECHISTC+59,"ECCA: Emism/Ematch(pl4)",80,0.,4.,0.);
    HBOOK1(ECHISTC+60,"ECCA: Emism/Ematch(pl5)",80,0.,4.,0.);
    HBOOK1(ECHISTC+61,"ECCA: Emism/Ematch(pl6)",80,0.,4.,0.);
    //
    //  WARNING: Hist.ECHISTC+100->ECHISTC+189 are reserved for Ped-Calibration 
    //gchen
    if(ECCAFFKEY.ecshswit==1){// =====> ecal_shower used.
      HBOOK1(ECHISTC+62,"ECCA: shower energy, track ok",80,0.,18.,0.);
      HBOOK1(ECHISTC+63,"ECCA: chi2 of dir fit",100,0.,100.,0.);
      HBOOK1(ECHISTC+64,"ECCA: Energy of Front layers",125,0.,250.,0.);
      HBOOK1(ECHISTC+65,"ECCA: Dif. over sum",80,-1.,0.6,0.);
      HBOOK1(ECHISTC+66,"ECCA: Rel. Side leak",40,0.,0.4,0.);
      HBOOK1(ECHISTC+67,"ECCA: Rel. Rear Leak",40,0.,0.4,0.);
      HBOOK1(ECHISTC+68,"ECCA: Rel. Dead Cell Leak",20,0.,0.2,0.);
      HBOOK1(ECHISTC+69,"ECCA: out of core leak",50,0.,0.5,0.);
      HBOOK1(ECHISTC+70,"ECCA: chi2 of prof. fit",50,0.,35.,0.);
      HBOOK1(ECHISTC+71,"ECCA: chi2 of trans. fit",50,0.,35.,0.);
      HBOOK1(ECHISTC+72,"ECCA: Mom/Edep",100,0.,2.,0.);
      HBOOK1(ECHISTC+73,"ECCA: Eshower/Mom-1",100,-1.,1.,0.);
      HBOOK1(ECHISTC+74,"ECCA: Track-shower entry x dist",100,-10.,10.,0.);
      HBOOK1(ECHISTC+75,"ECCA: Track-shower entry y dist",100,-10.,10.,0.);
      HBOOK1(ECHISTC+76,"ECCA: Track-shower exit x dist",100,-10.,10.,0.);
      HBOOK1(ECHISTC+77,"ECCA: Track-shower exit y dist",100,-10.,10.,0.);
      HBOOK1(ECHISTC+78,"ECCA: Edep/Mom(all cuts)",100,0.,2.,0.);
      HBOOK1(ECHISTC+79,"ECCA: Edep/Mom",100,0.,2.,0.);
      HBOOK1(ECHISTC+80,"ECCA: Mom/Edep(all cuts)",100,0.,2.,0.);
      HBOOK1(ECHISTC+81,"ECCA: Track beta",96,-1.2,1.2,0.);
      HBOOK1(ECHISTC+82,"ECCA: Track rigidity (Gv,FastFit,BadEbkg)",100,2.,12.,0.);
      HBOOK1(ECHISTC+83,"ECCA: Track Chi2(FastFit) ",80,0.,40.,0.);
      HBOOK1(ECHISTC+84,"ECCA: Track rigidity (Gv,FastFit)",100,2.,12.,0.);
      HBOOK1(ECHISTC+85,"ECCA: Track dR/R(FastFit)",100,0.,0.05,0.);
      HBOOK1(ECHISTC+86,"ECCA: Track half-rig. assymetry(AdvFit)",80,-0.4,0.4,0.);
      HBOOK1(ECHISTC+87,"ECCA: Track COS(theta) at EC front",100,-1.,1.,0.);
      HBOOK1(ECHISTC+88,"ECCA: Track rigidity (Gv,FastFit,TrackOK)",100,2.,12.,0.);
      HBOOK1(ECHISTC+89,"ECCA: Track Layer ampl.",80,0.,400.,0.);
      HBOOK1(ECHISTC+90,"ECCA: Rigidity(all cuts,mev)",100,2000.,12000.,0.);
      HBOOK1(ECHISTC+91,"ECCA: ANTI fired sectors",17,0.,17.,0.);
      HBOOK1(ECHISTC+92,"ECCA: Tracker backgr. Ycl/layer",50,0.,50.,0.);
      HBOOK1(ECHISTC+93,"ECCA: Tracker backgr. Xcl/layer",50,0.,50.,0.);
      HBOOK1(ECHISTC+94,"ECCA: Tracker X-layer backgr.",50,0.,10.,0.);
      HBOOK1(ECHISTC+95,"ECCA: Tracker Y-layer backgr.",50,0.,10.,0.);
      HBOOK1(ECHISTC+96,"ECCA: Track X-layer ampl.",80,0.,400.,0.);
      HBOOK1(ECHISTC+97,"ECCA: Track Y-layer ampl.",80,0.,400.,0.);
      HBOOK1(ECHISTC+98,"ECCA: Track rigidity (Gv,FastFit,BadClam)",100,2.,12.,0.);
      HBOOK1(ECHISTC+99,"ECCA: Track Xcross At ECTop",70,-35.,35.,0.);
      HBOOK1(ECHISTC+100,"ECCA: Track Ycross At ECTop",70,-35.,35.,0.);
      HBOOK1(ECHISTC+101,"ECCA: ANTI sector energy",80,0.,40.,0.);
      HBOOK1(ECHISTC+102,"ECCA: energy error of shower",50,0.,5.,0.);
      HBOOK1(ECHISTC+103,"ECCA: Rigidity(track ok)",100,2000.,12000.,0.);
      HBOOK1(ECHISTC+104,"ECCA: shower energy(chi2 ok)",80,0.,18.,0.);
      HBOOK1(ECHISTC+105,"ECCA: Rigidity(chi2 ok)",100,2000.,12000.,0.);
      HBOOK1(ECHISTC+106,"ECCA: shower energy(s/b leak ok)",80,0.,18.,0.);
      HBOOK1(ECHISTC+107,"ECCA: Rigidity(s/b leak ok)",100,2000.,12000.,0.);
      HBOOK1(ECHISTC+108,"ECCA: shower energy(d/o leak ok)",80,0.,18.,0.);
      HBOOK1(ECHISTC+109,"ECCA: Rigidity(d/o leak ok)",100,2000.,12000.,0.);
      HBOOK1(ECHISTC+110,"ECCA: shower energy(difsum ok)",80,0.,18.,0.);
      HBOOK1(ECHISTC+111,"ECCA: Rigidity(difsum ok)",100,2000.,12000.,0.);
      HBOOK1(ECHISTC+112,"ECCA: shower energy(all cuts ok)",80,0.,18.,0.);
      HBOOK1(ECHISTC+113,"ECCA: Rigidity(all cuts ok)",100,2000.,12000.,0.);
    }
  }
  //
}
//-----------------------------
void EcalJobStat::bookhistmc(){
  if(ECMCFFKEY.mcprtf!=0){ // Book mc-hist
    HBOOK1(ECHIST+1,"Geant-hits number",100,0.,5000.,0.);
    HBOOK1(ECHIST+2,"ECMC: GeantdE/dX-hits Etot(MeV)",100,0.,5000,0.);
    HBOOK1(ECHIST+3,"ECMC: GeantdE/dX-hits Etot(+FiberAtt,MeV)",100,0.,500.,0.);
    HBOOK1(ECHIST+4,"ECMC: GeantEvisTot(after mev2mev+NpeFluct)",200,0.,200000.,0.);
    HBOOK1(ECHIST+5,"ECMC: Indiv.Dynode signals(FTinput, mev)",100,0.,50.,0.);
    HBOOK1(ECHIST+6,"ECMC: 4xA/D(FT)-signal ratio",50,0.,10.,0.);
    HBOOK1(ECHIST+7,"ECMC: EmcHits SL-profile",ECSLMX,1.,geant(ECSLMX+1),0.);
    HBOOK1(ECHIST+8,"ECMC: EmcHits SL(PM-assigned)-profile",ECSLMX,1.,geant(ECSLMX+1),0.);
    HBOOK1(ECHIST+9,"ECMC: Etot(DynodeTrigSum,mev)",200,0.,100000.,0.);
    HBOOK1(ECHIST+10,"ECMC: ProjConfFlag(IJ->FTE|LVL1 Proj-Or(1)/And(2))",30,0.,30.,0.);
    HBOOK1(ECHIST+19,"ECMC: TriggerFlag(IJ->FTE|LVL1,0/1(no)/2(1prf)/3(2prj)",40,0.,40.,0.);
    HBOOK1(ECHIST+20,"ECMC: Tot.Anode charge(4subc.sum, pC)",100,0.,20.,0.);
    HBOOK1(ECHIST+21,"ECMC: Max TotAnodeCharge(4subc.sum, pC)",100,0.,1000.,0.);
    HBOOK1(ECHIST+22,"ECMC: Max ADC-H(incl.ped, No ovfl.limit)",100,0.,4100.,0.);
    HBOOK1(ECHIST+23,"ECMC: Max ADC-L(incl.ped, No ovfl.limit)",100,0.,4100.,0.);
    HBOOK1(ECHIST+24,"ECMC: Max ADC-D(incl.ped, No ovfl.limit)",100,0.,4100.,0.);
    HBOOK1(ECHIST+30,"ECMC: TrgDynSignalsMax(mev),SL1",80,0.,400.,0.);
    HBOOK1(ECHIST+31,"ECMC: TrgDynSignalsMax(mev),SL2",80,0.,400.,0.);
    HBOOK1(ECHIST+32,"ECMC: TrgDynSignalsMax(mev),SL3",80,0.,400.,0.);
    HBOOK1(ECHIST+33,"ECMC: TrgDynSignalsMax(mev),SL4",80,0.,400.,0.);
    HBOOK1(ECHIST+34,"ECMC: TrgDynSignalsMax(mev),SL5",80,0.,400.,0.);
    HBOOK1(ECHIST+35,"ECMC: TrgDynSignalsMax(mev),SL6",80,0.,400.,0.);
    HBOOK1(ECHIST+36,"ECMC: TrgDynSignalsMax(mev),SL7",80,0.,400.,0.);
    HBOOK1(ECHIST+37,"ECMC: TrgDynSignalsMax(mev),SL8",80,0.,400.,0.);
    HBOOK1(ECHIST+38,"ECMC: TrgDynSignalsMax(mev),SL9",80,0.,400.,0.);
    HBOOK1(ECHIST+39,"ECMC: NonEmptySLs(BendYproj)",10,0.,10.,0.);
    HBOOK1(ECHIST+40,"ECMC: NonEmptySLs(NonBendXproj)",10,0.,10.,0.);
    HBOOK1(ECHIST+41,"ECMC: MaxCOGShift(Yproj)",50,0.,10.,0.);
    HBOOK1(ECHIST+42,"ECMC: MaxCOGShift(Xproj)",50,0.,10.,0.);
    HBOOK1(ECHIST+43,"ECMC: DyTrigEnerTot(mev)",80,0.,2000.,0.);
  }
}
//----------------------------
void EcalJobStat::outp(){
  geant rzprofac[ecalconst::ECSLMX];
  if(LVL3FFKEY.histprf>0){// EC-lvl3 histograms
    HPRINT(ECHISTR+31);
    HPRINT(ECHISTR+32);
    HPRINT(ECHISTR+33);
    HPRINT(ECHISTR+34);
    HPRINT(ECHISTR+35);
    HPRINT(ECHISTR+36);
    HPRINT(ECHISTR+37);
    HPRINT(ECHISTR+38);
    HPRINT(ECHISTR+39);
    HPRINT(ECHISTR+40);
    HPRINT(ECHISTR+41);
    HPRINT(ECHISTR+42);
    HPRINT(ECHISTR+43);
    HPRINT(ECHISTR+44);
    HPRINT(ECHISTR+45);
    HPRINT(ECHISTR+46);
    HPRINT(ECHISTR+47);
    HPRINT(ECHISTR+48);
    HPRINT(ECHISTR+49);
    HPRINT(ECHISTR+50);
    HPRINT(ECHISTR+51);
    HPRINT(ECHISTR+52);
    HPRINT(ECHISTR+53);
    HPRINT(ECHISTR+54);
    HPRINT(ECHISTR+55);
    HPRINT(ECHISTR+56);
  }
  //
  if(ECREFFKEY.reprtf[0]>1){ // print RECO-hists
    HPRINT(ECHISTR+10);
    HPRINT(ECHISTR+11);
    HPRINT(ECHISTR+12);
    HPRINT(ECHISTR+13);
    HPRINT(ECHISTR+14);
    HPRINT(ECHISTR+9);
    HPRINT(ECHISTR+15);
    HPRINT(ECHISTR+16);
    HPRINT(ECHISTR+17);
    //      HPRINT(ECHISTR+18);
    //      HPRINT(ECHISTR+22);
    //      HPRINT(ECHISTR+23);
    if(recount[2]>0){
      //        for(int i=0;i<2*ECSLMX;i++)rzprofa[i]=geant(zprofa[i]/recount[2]);
      //        for(int i=0;i<ECSLMX;i++)rzprofapm[i]=geant(zprofapm[i]/recount[2]);
      //        HPAK(ECHISTR+24,rzprofa);
      //        HPAK(ECHISTR+25,rzprofapm);
      //        HPRINT(ECHISTR+24);
      //        HPRINT(ECHISTR+25);
    }
    HPRINT(ECHISTR+28);
    HPRINT(ECHISTR+29);
    HPRINT(ECHISTR+30);
      
    HPRINT(ECHISTR+60);
    HPRINT(ECHISTR+61);
    HPRINT(ECHISTR+62);
    HPRINT(ECHISTR+63);
    HPRINT(ECHISTR+64);
    HPRINT(ECHISTR+65);
    HPRINT(ECHISTR+66);
    HPRINT(ECHISTR+67);
    HPRINT(ECHISTR+68);
    //      for(int i=0;i<12;i++)HPRINT(ECHISTR+70+i);
  }
  if(ECREFFKEY.relogic[1]==1 || ECREFFKEY.relogic[1]==2){ // print RLGA/FIAT-hists
    if(ECCAFFKEY.hprintf>1){//mid proirity hist (for selection cuts)
      HPRINT(ECHISTC+12);
      HPRINT(ECHISTC+6);
      HPRINT(ECHISTC+35);
      HPRINT(ECHISTC+47);
      HPRINT(ECHISTC+36);
      HPRINT(ECHISTC);
      HPRINT(ECHISTC+37);
      HPRINT(ECHISTC+38);
      HPRINT(ECHISTC+24);
      HPRINT(ECHISTC+25);
      HPRINT(ECHISTC+17);
      HPRINT(ECHISTC+39);
      HPRINT(ECHISTC+40);
      HPRINT(ECHISTC+41);
      HPRINT(ECHISTC+28);
      HPRINT(ECHISTC+46);
      HPRINT(ECHISTC+16);
      HPRINT(ECHISTC+1);
      HPRINT(ECHISTC+2);
      //        HPRINT(ECHISTC+3);
      //        HPRINT(ECHISTC+4);
      HPRINT(ECHISTC+5);
      HPRINT(ECHISTC+29);
      HPRINT(ECHISTC+30);
      HPRINT(ECHISTC+43);
      HPRINT(ECHISTC+44);
      HPRINT(ECHISTC+45);
      for(int i=0;i<ECSLMX;i++){
	if(nprofac[i]>0)rzprofac[i]=geant(zprofac[i]/nprofac[i]);
	else rzprofac[i]=0;
      }
      HPAK(ECHISTC+18,rzprofac);
      HPRINT(ECHISTC+18);
      HPRINT(ECHISTC+19);
    }
      
    ECREUNcalib::mfit();//fits/write files
      
    if(ECCAFFKEY.hprintf>0){//hi prior.hist(results)
      HPRINT(ECHISTC+115); 
      HPRINT(ECHISTC+116); 
      HPRINT(ECHISTC+117); 
      HPRINT(ECHISTC+118); 
      HPRINT(ECHISTC+8);
      HPRINT(ECHISTC+9);
      HPRINT(ECHISTC+51);
      //      HPRINT(ECHISTC+10);//already printed inside mfit
      HPRINT(ECHISTC+11);
      HPRINT(ECHISTC+13);
      HPRINT(ECHISTC+20);
      HPRINT(ECHISTC+21);
      HPRINT(ECHISTC+22);
      HPRINT(ECHISTC+42);
      HPRINT(ECHISTC+23);
      HPRINT(ECHISTC+48);//a/d ratio final
      HPRINT(ECHISTC+49);//calib.quality
    }
    if(ECCAFFKEY.hprintf>2){//low prior.hist(pix/pm eff)
      HPRINT(ECHISTC+14);
      HPRINT(ECHISTC+15);
      HPRINT(ECHISTC+26);
      HPRINT(ECHISTC+27);
      HPRINT(ECHISTC+31);
      HPRINT(ECHISTC+32);
      //  hist +10 for FIAT is filled/printed inside mfit()
    }
    // test      HPRINT(ECHISTC+33);
    // test      HPRINT(ECHISTC+34);
  }
  if(ECREFFKEY.relogic[1]==3){ // print ANOR-hists
    HPRINT(ECHISTC+39);
    HPRINT(ECHISTC+40);
    HPRINT(ECHISTC);
    HPRINT(ECHISTC+1);
    HPRINT(ECHISTC+2);
    HPRINT(ECHISTC+3);
    HPRINT(ECHISTC+4);
    HPRINT(ECHISTC+44);
    HPRINT(ECHISTC+30);
    HPRINT(ECHISTC+31);
    HPRINT(ECHISTC+32);
    HPRINT(ECHISTC+33);
    HPRINT(ECHISTC+34);
    HPRINT(ECHISTC+35);
    HPRINT(ECHISTC+36);
    HPRINT(ECHISTC+43);
    HPRINT(ECHISTC+42);
    HPRINT(ECHISTC+41);
    HPRINT(ECHISTC+37);
    HPRINT(ECHISTC+38);
    HPRINT(ECHISTC+5);
    HPRINT(ECHISTC+6);
    HPRINT(ECHISTC+7);
    HPRINT(ECHISTC+8);
    HPRINT(ECHISTC+9);
    HPRINT(ECHISTC+10);
    HPRINT(ECHISTC+17);
    HPRINT(ECHISTC+11);
    HPRINT(ECHISTC+12);
    HPRINT(ECHISTC+13);
    HPRINT(ECHISTC+14);
    HPRINT(ECHISTC+16);
    HPRINT(ECHISTC+18);
    HPRINT(ECHISTC+19);
    HPRINT(ECHISTC+20);
    HPRINT(ECHISTC+25);
    HPRINT(ECHISTC+21);
    HPRINT(ECHISTC+22);
    HPRINT(ECHISTC+23);
    HPRINT(ECHISTC+15);
    HPRINT(ECHISTC+24);
    HPRINT(ECHISTC+50);
    HPRINT(ECHISTC+51);
    HPRINT(ECHISTC+52);
    HPRINT(ECHISTC+53);
    HPRINT(ECHISTC+54);
    HPRINT(ECHISTC+55);
    HPRINT(ECHISTC+56);
    HPRINT(ECHISTC+57);
    HPRINT(ECHISTC+58);
    HPRINT(ECHISTC+59);
    HPRINT(ECHISTC+60);
    HPRINT(ECHISTC+61);
    HPRINT(ECHISTC+26);
    HPRINT(ECHISTC+27);
    HPRINT(ECHISTC+28);
    HPRINT(ECHISTC+29);
    //gchen
    if(ECCAFFKEY.ecshswit==1){// =====> ecal_shower used.
      HPRINT(ECHISTC+62);
      HPRINT(ECHISTC+63);
      HPRINT(ECHISTC+64);
      HPRINT(ECHISTC+65);
      HPRINT(ECHISTC+66);
      HPRINT(ECHISTC+67);
      HPRINT(ECHISTC+68);
      HPRINT(ECHISTC+69);
      HPRINT(ECHISTC+70);
      HPRINT(ECHISTC+71);
      HPRINT(ECHISTC+72);
      HPRINT(ECHISTC+73);
      HPRINT(ECHISTC+74);
      HPRINT(ECHISTC+75);
      HPRINT(ECHISTC+76);
      HPRINT(ECHISTC+77);
      HPRINT(ECHISTC+78);
      HPRINT(ECHISTC+79);
      HPRINT(ECHISTC+80);
      HPRINT(ECHISTC+81);
      HPRINT(ECHISTC+82);
      HPRINT(ECHISTC+83);
      HPRINT(ECHISTC+84);
      HPRINT(ECHISTC+85);
      HPRINT(ECHISTC+86);
      HPRINT(ECHISTC+87);
      HPRINT(ECHISTC+88);
      HPRINT(ECHISTC+89);
      HPRINT(ECHISTC+90);
      HPRINT(ECHISTC+91);
      HPRINT(ECHISTC+92);
      HPRINT(ECHISTC+93);
      HPRINT(ECHISTC+94);
      HPRINT(ECHISTC+95);
      HPRINT(ECHISTC+96);
      HPRINT(ECHISTC+97);
      HPRINT(ECHISTC+98);
      HPRINT(ECHISTC+99);
      HPRINT(ECHISTC+100);
      HPRINT(ECHISTC+101);
      HPRINT(ECHISTC+102);
      HPRINT(ECHISTC+103);
      HPRINT(ECHISTC+104);
      HPRINT(ECHISTC+105);
      HPRINT(ECHISTC+106);
      HPRINT(ECHISTC+107);
      HPRINT(ECHISTC+108);
      HPRINT(ECHISTC+109);
      HPRINT(ECHISTC+110);
      HPRINT(ECHISTC+111);
      HPRINT(ECHISTC+112);
      HPRINT(ECHISTC+113);
      HPRINT(ECHISTC+114);
      //	HPRINT(ECHISTC+115);
      //	HPRINT(ECHISTC+116);
      //	HPRINT(ECHISTC+117);
      //	HPRINT(ECHISTC+118);
      HPRINT(ECHISTC+119);
      HPRINT(ECHISTC+120);
      HPRINT(ECHISTC+121);
      HPRINT(ECHISTC+122);
      HPRINT(ECHISTC+123);
      HPRINT(ECHISTC+124);
      HPRINT(ECHISTC+125);
      HPRINT(ECHISTC+126);
      HPRINT(ECHISTC+127);
      HPRINT(ECHISTC+128);
      HPRINT(ECHISTC+129);
      HPRINT(ECHISTC+130);
      HPRINT(ECHISTC+131);
      HPRINT(ECHISTC+132);
      HPRINT(ECHISTC+133);
    }
    ECREUNcalib::mfite();//for Anor-calib
  }
}
//----------------------------
void EcalJobStat::outpmc(){
  geant rzprmc1[ecalconst::ECSLMX],rzprmc2[ecalconst::ECSLMX];
  if(ECMCFFKEY.mcprtf!=0){ // print MC-hists
    HPRINT(ECHIST+1);
    HPRINT(ECHIST+2);
    HPRINT(ECHIST+3);
    HPRINT(ECHIST+4);
    HPRINT(ECHIST+20);
    HPRINT(ECHIST+21);
    HPRINT(ECHIST+22);
    HPRINT(ECHIST+23);
    HPRINT(ECHIST+24);
    HPRINT(ECHIST+5);
    HPRINT(ECHIST+6);
    if(mccount[1]>0){
      for(int i=0;i<ECSLMX;i++){
	rzprmc1[i]=geant(zprmc1[i]/mccount[1]);
	rzprmc2[i]=geant(zprmc2[i]/mccount[1]);
      }
      HPAK(ECHIST+7,rzprmc1);
      HPAK(ECHIST+8,rzprmc2);
      HPRINT(ECHIST+7);
      HPRINT(ECHIST+8);
    }
    HPRINT(ECHIST+9);
    HPRINT(ECHIST+30);
    HPRINT(ECHIST+31);
    HPRINT(ECHIST+32);
    HPRINT(ECHIST+33);
    HPRINT(ECHIST+34);
    HPRINT(ECHIST+35);
    HPRINT(ECHIST+36);
    HPRINT(ECHIST+37);
    HPRINT(ECHIST+38);
    HPRINT(ECHIST+39);
    HPRINT(ECHIST+40);
    HPRINT(ECHIST+41);
    HPRINT(ECHIST+42);
    HPRINT(ECHIST+43);
    HPRINT(ECHIST+10);
    HPRINT(ECHIST+19);
  }
}
//==========================================================================
//  ECcalib class functions :
//
void ECcalib::build(){// <--- create MC/RealData ecpmcal-objects
  int i,isl,ipm,isc,cnum;
  integer sid,sta[4],stad,endflab;
  geant pmrg,scrg[4],h2lr[4],a2m,a2dr,lfs,lsl,ffr;
  geant h2lo[4];
  integer slmx,pmmx;
  slmx=ECSLMX;//max.S-layers
  pmmx=ECPMSMX;//max.PM's in S-layer
  cout<<endl<<"====> ECcalib_build: start with total PMTs="<<ECPMSL<<endl;
  //
  char fname[1024];
  char name[80];
  char datt[3];
  char ext[80];
  int date[2],year,mon,day,hour,min,sec;
  int ctyp,ntypes;
  uinteger iutct;
  tm begin;
  time_t utct;
  uinteger verids[10],verid;
  //
  integer status[ECPMSL][4],statusd[ECPMSL];
  geant pmrgn[ECPMSL],pmscgn[ECPMSL][4],sch2lr[ECPMSL][4],an2dyr[ECPMSL],adc2mev;
  geant sch2lo[ECPMSL][4];
  geant lfast[ECPMSL],lslow[ECPMSL],fastf[ECPMSL];
  //
  // ---> read list of calibration-files version-numbers (menu-file) :
  //
  strcpy(name,"EcalCflist");// basic name for vers.list-file  
  if(AMSJob::gethead()->isMCData()){
    strcpy(datt,"MC");
    sprintf(ext,"%d",ECMCFFKEY.calvern);//MC-versn
  }
  else{
    strcpy(datt,"RD");
    sprintf(ext,"%d",ECREFFKEY.calutc);//RD-utc
  }
  strcat(name,datt);
  strcat(name,".");
  strcat(name,ext);
  //
  if(ECCAFFKEY.cafdir==0)strcpy(fname,AMSDATADIR.amsdatadir);
  if(ECCAFFKEY.cafdir==1)strcpy(fname,"");
  strcat(fname,name);
  cout<<"====> ECcalib::build:Opening Calib_vers_list-file: "<<fname<<'\n';
  ifstream vlfile(fname,ios::in); // open needed tdfmap-file for reading
  if(!vlfile){
    cerr <<"<---- ECcalib_build:Error: missing Calib_vers_list-file "<<fname<<endl;
    exit(1);
  }
  vlfile >> ntypes;// total number of calibr. file types in the list
  for(i=0;i<ntypes;i++){
    vlfile >> verids[i];
    CFlistC[i+1]=verids[i]; 
  }
  CFlistC[0]=ntypes;
  //
  if(AMSJob::gethead()->isMCData()){
    vlfile >> date[0];//YYYYMMDD beg.validity of TofCflistMC.ext file
    vlfile >> date[1];//HHMMSS ......................................
    year=date[0]/10000;//2004->
    mon=(date[0]%10000)/100;//1-12
    day=(date[0]%100);//1-31
    hour=date[1]/10000;//0-23
    min=(date[1]%10000)/100;//0-59
    sec=(date[1]%100);//0-59
    begin.tm_isdst=0;
    begin.tm_sec=sec;
    begin.tm_min=min;
    begin.tm_hour=hour;
    begin.tm_mday=day;
    begin.tm_mon=mon-1;
    begin.tm_year=year-1900;
    utct=mktime(& begin);
    iutct=uinteger(utct);
    cout<<"      EcalCflistMC-file begin_date: year:month:day = "<<year<<":"<<mon<<":"<<day<<endl;
    cout<<"                                     hour:min:sec = "<<hour<<":"<<min<<":"<<sec<<endl;
    cout<<"                                         UTC-time = "<<iutct<<endl;
    CFlistC[ntypes+1]=ECMCFFKEY.calvern;
    CFlistC[ntypes+2]=date[0];
    CFlistC[ntypes+3]=date[1];
  }
  else{
    utct=time_t(TFREFFKEY.calutc);
    printf("      EcalCflistRD-file begin_date: %s",ctime(&utct)); 
    CFlistC[ntypes+1]=ECREFFKEY.calutc;
  }
  //
  vlfile.close();
  //------------------------------
  //
  //   --->  Read RLGA(status/rel.gains) calib. file :
  //
  ctyp=1;//1st type of calibration is RLGA(really may be single)
  verid=verids[ctyp-1];//MC-versn or RD-utc
  strcpy(name,"EcalRlga");
  strcat(name,datt);
  strcat(name,".");
  sprintf(ext,"%d",verid);
  strcat(name,ext);
  if(ECCAFFKEY.cafdir==0)strcpy(fname,AMSDATADIR.amsdatadir);
  if(ECCAFFKEY.cafdir==1)strcpy(fname,"");
  strcat(fname,name);
  cout<<"      Opening RLGA-calib file "<<fname<<" ..."<<endl;
  ifstream rlgfile(fname,ios::in); // open  file for reading
  if(!rlgfile){
    cerr <<"<---- ECcalib_build: Error: missing RLGA-file !!? "<<fname<<endl;
    exit(1);
  }
  //
  // ---> read PM-status:
  //
  for(isl=0;isl<slmx;isl++){   
    for(isc=0;isc<4;isc++){   
      for(ipm=0;ipm<pmmx;ipm++){  
        cnum=isl*pmmx+ipm; // sequential PM numbering(0 - SL*PMperSL)(324)
        rlgfile >> status[cnum][isc];//pixel's status
      }
    }
    for(ipm=0;ipm<pmmx;ipm++){
      cnum=isl*pmmx+ipm; // sequential PM numbering(0 - SL*PMperSL)(324)
      rlgfile >> statusd[cnum];//dynode's status
    }
  } 
  //
  // ---> read PM(sum of 4 SubCells) relative(to some Ref.PM) gains
  //
  for(isl=0;isl<slmx;isl++){   
    for(ipm=0;ipm<pmmx;ipm++){  
      cnum=isl*pmmx+ipm; // sequential PM numbering(0 - SL*PMperSL)(324)
      rlgfile >> pmrgn[cnum];
    }
  }
  //
  // ---> read PM-SubCell relative gains:
  //
  for(isl=0;isl<slmx;isl++){   
    for(isc=0;isc<4;isc++){   
      for(ipm=0;ipm<pmmx;ipm++){  
        cnum=isl*pmmx+ipm; // sequential PM numbering(0 - SL*PMperSL)(324)
        rlgfile >> pmscgn[cnum][isc];
      }
    }
  }
  //
  // ---> read PM-SubCell hi2low ratios:
  //
  for(isl=0;isl<slmx;isl++){   
    for(isc=0;isc<4;isc++){   
      for(ipm=0;ipm<pmmx;ipm++){  
        cnum=isl*pmmx+ipm; // sequential PM numbering(0 - SL*PMperSL)(324)
        rlgfile >> sch2lr[cnum][isc];
      }
    }
  }
  //
  // ---> read PM-SubCell hi2low offsets:
  //
  for(isl=0;isl<slmx;isl++){   
    for(isc=0;isc<4;isc++){   
      for(ipm=0;ipm<pmmx;ipm++){  
        cnum=isl*pmmx+ipm; // sequential PM numbering(0 - SL*PMperSL)(324)
        rlgfile >> sch2lo[cnum][isc];
      }
    }
  }
  //
  // ---> read PM anode(sum of 4-SubCells)-to-Dynode ratious:
  //
  for(isl=0;isl<slmx;isl++){   
    for(ipm=0;ipm<pmmx;ipm++){  
      cnum=isl*pmmx+ipm; // sequential PM numbering(0 - SL*PMperSL)(324)
      rlgfile >> an2dyr[cnum];
    }
  }
  // ---> read endfile-label:
  //
  rlgfile >> endflab;
  //
  rlgfile.close();
  if(endflab==12345){
    cout<<"      RLGA-calibration file is successfully read !"<<endl;
  }
  else{cout<<"<---- ECcalib::build: ERROR: problems while reading RLGA-calib.file !!?"<<endl;
    exit(1);
  }
  //==================================================================
  //
  //   --->  Read FIAT(fiber-attenuation) calib. file :
  //
  ctyp=2;//2nd type of calibration 
  verid=verids[ctyp-1];//MC-versn or RD-utc
  strcpy(name,"EcalFiat");
  strcat(name,datt);
  strcat(name,".");
  sprintf(ext,"%d",verid);
  strcat(name,ext);
  if(ECCAFFKEY.cafdir==0)strcpy(fname,AMSDATADIR.amsdatadir);
  if(ECCAFFKEY.cafdir==1)strcpy(fname,"");
  strcat(fname,name);
  cout<<"      Opening FIAT-calib.file "<<fname<<" ..."<<endl;
  ifstream fatfile(fname,ios::in); // open  file for reading
  if(!fatfile){
    cerr <<"<---- ECcalib_build::Error: missing FIAT-calib.file !!? "<<fname<<endl;
    exit(1);
  }
  //
  // ---> read PM-fibers att.parameters(Latt_fast/Latt_slow/Fast_fraction): 
  //
  for(isl=0;isl<slmx;isl++){   
    for(ipm=0;ipm<pmmx;ipm++){  
      cnum=isl*pmmx+ipm; // sequential PM numbering(0 - SL*PMperSL)(324)
      fatfile >> lfast[cnum];
    }
  }
  //
  for(isl=0;isl<slmx;isl++){   
    for(ipm=0;ipm<pmmx;ipm++){  
      cnum=isl*pmmx+ipm; // sequential PM numbering(0 - SL*PMperSL)(324)
      fatfile >> lslow[cnum];
    }
  }
  //
  for(isl=0;isl<slmx;isl++){   
    for(ipm=0;ipm<pmmx;ipm++){  
      cnum=isl*pmmx+ipm; // sequential PM numbering(0 - SL*PMperSL)(324)
      fatfile >> fastf[cnum];
    }
  }
  // ---> read endfile-label:
  //
  fatfile >> endflab;
  //
  fatfile.close();
  if(endflab==12345){
    cout<<"      FIAT-calibration file is successfully read !"<<endl;
  }
  else{cout<<"<---- ECcalib_build: ERROR:problems while reading FIAT-calib.file !!?"<<endl;
    exit(1);
  }
  //================================================================== 
  //
  //   --->  Read abs.norm. calib. file :
  //
  ctyp=3;//3rd type of calibration 
  verid=verids[ctyp-1];//MC-versn or RD-utc
  strcpy(name,"EcalAnor");
  strcat(name,datt);
  strcat(name,".");
  sprintf(ext,"%d",verid);
  strcat(name,ext);
  if(ECCAFFKEY.cafdir==0)strcpy(fname,AMSDATADIR.amsdatadir);
  if(ECCAFFKEY.cafdir==1)strcpy(fname,"");
  strcat(fname,name);
  cout<<"      Opening ANOR-calib.file "<<fname<<" ..."<<endl;
  ifstream anrfile(fname,ios::in); // open  file for reading
  if(!anrfile){
    cerr <<"<---- ECcalib_build:Error: missing ANOR-calib file !!? "<<fname<<endl;
    exit(1);
  }
  //
  // ---> read common(hope) adc2mev convertion factor(abs.norm):
  //
  anrfile >> adc2mev;
  // DEBUG
  if (ECREFFKEY.reprtf[1]==2) cout << "ADC2MEV=" << adc2mev << endl;
  //
  // ---> read endfile-label:
  //
  anrfile >> endflab;
  //
  anrfile.close();
  if(endflab==12345){
    cout<<"      ANOR-calibration file is successfully read !"<<endl;
  }
  else{cout<<"<---- ECcalib_build: ERROR: problems while reading ANOR-calib.file !!?"<<endl;
    exit(1);
  }
    
  //------------------------------
  //
  //   
  //
  for(isl=0;isl<slmx;isl++){
    for(ipm=0;ipm<pmmx;ipm++){
      cnum=isl*pmmx+ipm; // sequential PM numbering(0 - SL*PMperSL)(32)
      sid=(ipm+1)+100*(isl+1);
      for(isc=0;isc<4;isc++)sta[isc]=status[cnum][isc];//anode pixels
      stad=statusd[cnum];//dynode
      pmrg=pmrgn[cnum];
      for(isc=0;isc<4;isc++)scrg[isc]=1/(pmrg*pmscgn[cnum][isc]);
      //(1/(..) ->  to have multiplication instead of deviding in RECO(for speed); pmrg included
      //  in pmscgn because in simu/reco product of pmrg*pmscgn is used really(or pmrg alone)
      for(isc=0;isc<4;isc++)h2lr[isc]=sch2lr[cnum][isc];
      for(isc=0;isc<4;isc++)h2lo[isc]=sch2lo[cnum][isc];
      a2dr=an2dyr[cnum];
      lfs=lfast[cnum];
      lsl=lslow[cnum];
      ffr=fastf[cnum];
      a2m=adc2mev;
      ecpmcal[isl][ipm]=ECcalib(sid,sta,stad,pmrg,scrg,h2lr,h2lo,a2dr,lfs,lsl,ffr,a2m);
    }
  }
  cout<<"<---- ECcalib::build: successfully done !"<<endl<<endl;
}
//---------------
number ECcalib::pmsatf1(int dir,number q){//simulate PM-anode saturation, i.e. Qmeas/Qin
  //   return 1;
  //(imply satur.origin as voltage drop on last devider resistor,i.e. depends on TOTAL
  // PM charge(4xSubc. peak current)); dir=0/1->(Qin->Qmeas)/(Qmeas->Qin)
  //                                return Qmeas/Qin(dir=0) or  Qin/Qmeas(dir=1)
  int npnt=11;
  int i;
  geant qin[11]={750.,800.,900.,1000.,1250.,1500.,2000.,3000.,4000.,6000.,10000.};
  geant qme[11]={750.,785.,854., 920.,1071.,1210.,1450.,1835.,2125.,2507., 2747.};
  if(dir==0){//Qin->Qmeas
    if(q<=qin[0])return 1;
    else if(q>=qin[npnt-1])return qme[npnt-1]/q;
    else{
      for(i=1;i<npnt;i++){
	if(q<qin[i]){
	  return (qme[i-1]+(qme[i]-qme[i-1])*(q-qin[i-1])/(qin[i]-qin[i-1]))/q;//r=Qme/Qin<1
	}
      }
    }
  }
  else if(dir==1){//Qmeas->Qin
    if(q<=qme[0])return 1;
    else if(q>(0.99*qme[npnt-1])){
      //      cerr<<"ECcalib::pmsatf1:Qmeas saturation !"<<endl;
      return -qin[npnt-1]/q;//r=Qin/Qmeas
    }
    else{
      for(i=1;i<npnt;i++){
	if(q<qme[i]){
	  return (qin[i-1]+(qin[i]-qin[i-1])*(q-qme[i-1])/(qme[i]-qme[i-1]))/q;//r=Qin/Qmeas>1
	}
      }
    }
  } 
  cerr<<"<---- ECcalib_pmsatf1:Error: wrong dir parameter or logic error"<<dir<<endl;
  abort();
  return(0);
}  
//==========================================================================
//  ECcalibMS class functions :
//
void ECcalibMS::build(){// <--- create ecpmcal-objects used as "MC-Seeds"
  int i,isl,ipm,isc,cnum;
  integer sid,sta[4],stad,endflab;
  geant pmrg,scrg[4],h2lr[4],a2dr,lfs,lsl,ffr;
  geant h2lo[4];
  integer slmx,pmmx;
  slmx=ECSLMX;//max.S-layers
  pmmx=ECPMSMX;//max.PMs in S-layer
  cout<<endl<<"====> ECcalibMS::build: start build with total PMs="<<ECPMSL<<endl;
  //
  char fname[1024];
  char name[80];
  char vers1[3]="MC";
  char vers2[3]="SD";
  int ctyp,ntypes,mcvn,mcvern[10];
  char datt[3];
  char ext[80];
  int date[2],year,mon,day,hour,min,sec;
  uinteger iutct;
  tm begin;
  time_t utct;
  //
  integer status[ECPMSL][4],statusd[ECPMSL];
  geant pmrgn[ECPMSL],pmscgn[ECPMSL][4],sch2lr[ECPMSL][4],an2dyr[ECPMSL];
  geant sch2lo[ECPMSL][4];
  geant lfast[ECPMSL],lslow[ECPMSL],fastf[ECPMSL];
  //
  // ---> read list of calibration-type-versions list (menu-file) :
  //
  strcpy(name,"EcalCflistMC");// basic name for file of cal-files list 
  strcat(name,".");
  sprintf(ext,"%d",ECMCFFKEY.calvern);//got TofCflistMC. file extention
  strcat(name,ext);
  //
  if(ECCAFFKEY.cafdir==0)strcpy(fname,AMSDATADIR.amsdatadir);
  if(ECCAFFKEY.cafdir==1)strcpy(fname,"");
  strcat(fname,name);
  cout<<"      Open verslist-file  "<<fname<<'\n';
  ifstream vlfile(fname,ios::in); // open needed tdfmap-file for reading
  if(!vlfile){
    cerr <<"<---- ECcalibMS_build::Error: missing verslist-file !!? "<<fname<<endl;
    exit(1);
  }
  vlfile >> ntypes;// total number of calibr. file types in the list
  for(i=0;i<ntypes;i++){
    vlfile >> mcvern[i];// first number - for mc
  }
  vlfile >> date[0];//YYYYMMDD beg.validity of TofCflistMC.ext file
  vlfile >> date[1];//HHMMSS ......................................
  vlfile.close();
  //
  year=date[0]/10000;//2004->
  mon=(date[0]%10000)/100;//1-12
  day=(date[0]%100);//1-31
  hour=date[1]/10000;//0-23
  min=(date[1]%10000)/100;//0-59
  sec=(date[1]%100);//0-59
  begin.tm_isdst=0;
  begin.tm_sec=sec;
  begin.tm_min=min;
  begin.tm_hour=hour;
  begin.tm_mday=day;
  begin.tm_mon=mon-1;
  begin.tm_year=year-1900;
  utct=mktime(& begin);
  iutct=uinteger(utct);
  cout<<"      EcalCflistMC-file begin_date: year:month:day = "<<year<<":"<<mon<<":"<<day<<endl;
  cout<<"                                      hour:min:sec = "<<hour<<":"<<min<<":"<<sec<<endl;
  cout<<"                                          UTC-time = "<<iutct<<endl;
  //------------------------------------------------
  //
  // --->  Read copy of RealData RLGA calib. file(to use as MC-Seed) :
  //                   (or MC, if silogic[1]=1)
  ctyp=1;//1st type of calibration 
  strcpy(name,"EcalRlga");
  mcvn=mcvern[ctyp-1];
  // 
  cout<<"      Start to read MC/SD-type of RLGA/FIAT-calibr. files as MC-Seed :"<<endl;
  cout<<"      (normally MC(*MC.n)-file is the CalibProg outp.file when the Seed"<<endl;
  cout<<"      -file(*SD.n, normally=RD_copy) was chosen to be read at this stage !!!)"<<endl;
  //
  if(ECMCFFKEY.silogic[1]==0)strcpy(datt,vers1);//mc
  else strcpy(datt,vers2);//sd = copy of rl
  strcat(name,datt);
  strcat(name,".");
  sprintf(ext,"%d",mcvn);//
  strcat(name,ext);
  //
  if(ECCAFFKEY.cafdir==0)strcpy(fname,AMSDATADIR.amsdatadir);
  if(ECCAFFKEY.cafdir==1)strcpy(fname,"");
  strcat(fname,name);
  cout<<"      Opening MC/SD-type of RLGA-calib file "<<fname<<" ..."<<endl;
  ifstream rlgfile(fname,ios::in); // open  file for reading
  if(!rlgfile){
    cerr <<"<---- ECcalibMS_build:Error: missing MC/SD-type of RLGA-calib file !!? "<<fname<<endl;
    exit(1);
  }
  //
  // ---> read PM-status:
  //
  for(isl=0;isl<slmx;isl++){   
    for(isc=0;isc<4;isc++){   
      for(ipm=0;ipm<pmmx;ipm++){  
        cnum=isl*pmmx+ipm; // sequential PM numbering(0 - SL*PMperSL)(324)
        rlgfile >> status[cnum][isc];//pixel's status
      }
    }
    for(ipm=0;ipm<pmmx;ipm++){
      cnum=isl*pmmx+ipm; // sequential PM numbering(0 - SL*PMperSL)(324)
      rlgfile >> statusd[cnum];//dynode's status
    }
  } 
  //
  // ---> read PM(sum of 4 SubCells) relative(to some Ref.PM) gains
  //
  for(isl=0;isl<slmx;isl++){   
    for(ipm=0;ipm<pmmx;ipm++){  
      cnum=isl*pmmx+ipm; // sequential PM numbering(0 - SL*PMperSL)(324)
      rlgfile >> pmrgn[cnum];
    }
  }
  //
  // ---> read PM-SubCell relative gains:
  //
  for(isl=0;isl<slmx;isl++){   
    for(isc=0;isc<4;isc++){   
      for(ipm=0;ipm<pmmx;ipm++){  
        cnum=isl*pmmx+ipm; // sequential PM numbering(0 - SL*PMperSL)(324)
        rlgfile >> pmscgn[cnum][isc];
      }
    }
  }
  //
  // ---> read PM-SubCell hi2low ratious:
  //
  for(isl=0;isl<slmx;isl++){   
    for(isc=0;isc<4;isc++){   
      for(ipm=0;ipm<pmmx;ipm++){  
        cnum=isl*pmmx+ipm; // sequential PM numbering(0 - SL*PMperSL)(324)
        rlgfile >> sch2lr[cnum][isc];
      }
    }
  }
  //
  // ---> read PM-SubCell hi2low offsets:
  //
  for(isl=0;isl<slmx;isl++){   
    for(isc=0;isc<4;isc++){   
      for(ipm=0;ipm<pmmx;ipm++){  
        cnum=isl*pmmx+ipm; // sequential PM numbering(0 - SL*PMperSL)(324)
        rlgfile >> sch2lo[cnum][isc];
      }
    }
  }
  //
  // ---> read PM anode(sum of 4-SubCells)-to-Dynode ratious:
  //
  for(isl=0;isl<slmx;isl++){   
    for(ipm=0;ipm<pmmx;ipm++){  
      cnum=isl*pmmx+ipm; // sequential PM numbering(0 - SL*PMperSL)(324)
      rlgfile >> an2dyr[cnum];
    }
  }
  // ---> read endfile-label:
  //
  rlgfile >> endflab;
  //
  rlgfile.close();
  if(endflab==12345){
    cout<<"   <--"<<datt<<"-type of RLGA-calib file is successfully read as MCSeed !"<<endl;
  }
  else{cout<<"<---- ECcalibMS_build:Error reading MC/SD-type of RLGA-calib.file(as MCSeed) !!?"<<endl;
    exit(1);
  }
  //==================================================================
  //
  //   --->  Read copy of RealData FIAT calib. file(used as MC-Seeds) :
  //                      (or MC)
  ctyp=2;//2nd type of calibration 
  strcpy(name,"EcalFiat");
  mcvn=mcvern[ctyp-1];
  if(ECMCFFKEY.silogic[1]==0)strcpy(datt,vers1);//mc
  else strcpy(datt,vers2);//sd = copy of rl
  strcat(name,datt);
  strcat(name,".");
  sprintf(ext,"%d",mcvn);
  strcat(name,ext);
  //
  if(ECCAFFKEY.cafdir==0)strcpy(fname,AMSDATADIR.amsdatadir);
  if(ECCAFFKEY.cafdir==1)strcpy(fname,"");
  strcat(fname,name);
  cout<<"      Opening MC/SD-type of FIAT-calib file "<<fname<<" ..."<<endl;
  ifstream fatfile(fname,ios::in); // open  file for reading
  if(!fatfile){
    cerr <<"<---- ECcalibMS_build:Error: missing MC/SD-type of FIAT-calib file !!? "<<fname<<endl;
    exit(1);
  }
  //
  // ---> read PM-fibers att.parameters(Latt_fast/Latt_slow/Fast_fraction): 
  //
  for(isl=0;isl<slmx;isl++){   
    for(ipm=0;ipm<pmmx;ipm++){  
      cnum=isl*pmmx+ipm; // sequential PM numbering(0 - SL*PMperSL)(324)
      fatfile >> lfast[cnum];
    }
  }
  //
  for(isl=0;isl<slmx;isl++){   
    for(ipm=0;ipm<pmmx;ipm++){  
      cnum=isl*pmmx+ipm; // sequential PM numbering(0 - SL*PMperSL)(324)
      fatfile >> lslow[cnum];
    }
  }
  //
  for(isl=0;isl<slmx;isl++){   
    for(ipm=0;ipm<pmmx;ipm++){  
      cnum=isl*pmmx+ipm; // sequential PM numbering(0 - SL*PMperSL)(324)
      fatfile >> fastf[cnum];
    }
  }
  // ---> read endfile-label:
  //
  fatfile >> endflab;
  //
  fatfile.close();
  if(endflab==12345){
    cout<<"   <--"<<datt<<"-type of FIAT-calibr. file is successfully read as MCSeed !"<<endl;
  }
  else{cout<<"<---- ECcalibMS_build: Error reading MC/SD-type of FIAT-calib.file(as MCSeed) !"<<endl;
    exit(1);
  }
  //------------------------------
  //
  //   
  //
  for(isl=0;isl<slmx;isl++){
    for(ipm=0;ipm<pmmx;ipm++){
      cnum=isl*pmmx+ipm; // sequential PM numbering(0 - SL*PMperSL)(32)
      sid=(ipm+1)+100*(isl+1);
      for(isc=0;isc<4;isc++)sta[isc]=status[cnum][isc];//anode pixels
      stad=statusd[cnum];//dynode
      pmrg=pmrgn[cnum];
      for(isc=0;isc<4;isc++)scrg[isc]=1/(pmrg*pmscgn[cnum][isc]);
      //(1/(..) ->  to have mult. instead of dev. in RECO(for speed); pmrg included
      //  in pmscgn because in simu/reco product of pmrg*pmscgn is used really(or pmrg alone)
      for(isc=0;isc<4;isc++)h2lr[isc]=sch2lr[cnum][isc];
      for(isc=0;isc<4;isc++)h2lo[isc]=sch2lo[cnum][isc];
      a2dr=an2dyr[cnum];
      lfs=lfast[cnum];
      lsl=lslow[cnum];
      ffr=fastf[cnum];
      ecpmcal[isl][ipm]=ECcalibMS(sid,sta,stad,pmrg,scrg,h2lr,h2lo,a2dr,lfs,lsl,ffr);
    }
  }
  //
  cout<<"<---- ECcalibMS::build: successfully done !"<<endl<<endl;
}  
//==========================================================================
//  ECALVarp class functions :
//
void ECALVarp::init(geant daqth[15], geant cuts[5]){

  int i;
  for(i=0;i<15;i++)_daqthr[i]=daqth[i];
  for(i=0;i<5;i++)_cuts[i]=cuts[i];
}
//==========================================================================
//
void ECPMPeds::build(){// create MC/RD ECPeds-objects for each cell from MC/RD- default_files
  //
  int isl,ipm,isc,cnum;
  integer sid,endflab(0);
  char fname[1024];
  char name[80];
  geant pedh[4],sigh[4],pedl[4],sigl[4],pedd,sigd;
  uinteger stah[4],stal[4],stad;
  geant pmpedh[ECPMSL][4],pmsigh[ECPMSL][4],pmpedl[ECPMSL][4],pmsigl[ECPMSL][4];
  geant pmpedd[ECPMSL],pmsigd[ECPMSL];
  uinteger pmstah[ECPMSL][4],pmstal[ECPMSL][4],pmstad[ECPMSL];
  integer slmx,pmmx;
  slmx=ECSLMX;//max.S-layers
  pmmx=ECPMSMX;//max.PMs in S-layer
  //
  //   --->  Read high/low pedestals default file :
  //
  strcpy(name,"eclp_df");
  if(AMSJob::gethead()->isMCData())           // for MC-event
    {
      cout <<"====> ECPMPeds_build: Default Real_MC-peds file is used..."<<endl;
      strcat(name,"_mc");
    }
  else                                       // for Real events
    {
      cout <<"====> ECPMPeds_build: Default Real_RD-peds file is used..."<<endl;
      strcat(name,"_rl");
    }
  strcat(name,".dat");
  if(ECCAFFKEY.cafdir==0)strcpy(fname,AMSDATADIR.amsdatadir);
  if(ECCAFFKEY.cafdir==1)strcpy(fname,"");
  strcat(fname,name);
  cout<<"      Open file : "<<fname<<'\n';
  ifstream icfile(fname,ios::in); // open pedestals-file for reading
  if(!icfile){
    cerr <<"<---- missing default pedestals-file !!? "<<fname<<endl;
    exit(1);
  }
  //
  // ---> read HighGain peds/sigmas:
  //
  for(isl=0;isl<slmx;isl++){   
    for(isc=0;isc<4;isc++){   
      for(ipm=0;ipm<pmmx;ipm++){  
        cnum=isl*pmmx+ipm; // sequential PM numbering(0 - SL*PMperSL)(324)
        icfile >> pmpedh[cnum][isc];
      }
      for(ipm=0;ipm<pmmx;ipm++){  
        cnum=isl*pmmx+ipm; // sequential PM numbering(0 - SL*PMperSL)(324)
        icfile >> pmsigh[cnum][isc];
      }
      for(ipm=0;ipm<pmmx;ipm++){  
        cnum=isl*pmmx+ipm; // sequential PM numbering(0 - SL*PMperSL)(324)
        icfile >> pmstah[cnum][isc];
      }
    }
  } 
  //
  // ---> read LowGain peds/sigmas:
  //
  for(isl=0;isl<slmx;isl++){   
    for(isc=0;isc<4;isc++){   
      for(ipm=0;ipm<pmmx;ipm++){  
        cnum=isl*pmmx+ipm; // sequential PM numbering(0 - SL*PMperSL)(324)
        icfile >> pmpedl[cnum][isc];
      }
      for(ipm=0;ipm<pmmx;ipm++){  
        cnum=isl*pmmx+ipm; // sequential PM numbering(0 - SL*PMperSL)(324)
        icfile >> pmsigl[cnum][isc];
      }
      for(ipm=0;ipm<pmmx;ipm++){  
        cnum=isl*pmmx+ipm; // sequential PM numbering(0 - SL*PMperSL)(324)
        icfile >> pmstal[cnum][isc];
      }
    }
  } 
  //
  // ---> read Dynode peds/sigmas:
  //
  for(isl=0;isl<slmx;isl++){   
    for(ipm=0;ipm<pmmx;ipm++){  
      cnum=isl*pmmx+ipm; // sequential PM numbering(0 - SL*PMperSL)(324)
      icfile >> pmpedd[cnum];
    }
    for(ipm=0;ipm<pmmx;ipm++){  
      cnum=isl*pmmx+ipm; // sequential PM numbering(0 - SL*PMperSL)(324)
      icfile >> pmsigd[cnum];
    }
    for(ipm=0;ipm<pmmx;ipm++){  
      cnum=isl*pmmx+ipm; // sequential PM numbering(0 - SL*PMperSL)(324)
      icfile >> pmstad[cnum];
    }
  } 
  //
  // ---> read EndFileLabel :
  //
  icfile >> endflab;
  //
  icfile.close();
  //
  if(endflab==12345){
    cout<<"<---- ECPMPeds::build: successfully done !"<<endl;
  }
  else{cout<<"<---- ECPMPeds::build:Error: problem to read default peds-file"<<endl;
    exit(1);
  }
  //---------------------------------------------
  //
  for(isl=0;isl<slmx;isl++){
    for(ipm=0;ipm<pmmx;ipm++){
      cnum=isl*pmmx+ipm; // sequential PM numbering(0 - SL*PMperSL)(324)
      sid=(ipm+1)+100*(isl+1);
      for(isc=0;isc<4;isc++)stah[isc]=pmstah[cnum][isc];
      for(isc=0;isc<4;isc++)stal[isc]=pmstal[cnum][isc];
      for(isc=0;isc<4;isc++)pedh[isc]=pmpedh[cnum][isc];
      for(isc=0;isc<4;isc++)sigh[isc]=pmsigh[cnum][isc];
      for(isc=0;isc<4;isc++)pedl[isc]=pmpedl[cnum][isc];
      for(isc=0;isc<4;isc++)sigl[isc]=pmsigl[cnum][isc];
      pedd=pmpedd[cnum];
      sigd=pmsigd[cnum];
      stad=pmstad[cnum];
      pmpeds[isl][ipm]=ECPMPeds(sid,stah,stal,stad,pedh,sigh,pedl,sigl,pedd,sigd);
    }
  }
}
//==========================================================================
//
void ECPMPeds::mcbuild(){// create MC ECPeds-objects for each cell by smearing of seed-values from DataCards
  //
  int isl,ipm,isc,cnum;
  integer sid;
  geant pedh[4],sigh[4],pedl[4],sigl[4],pedd,sigd;
  uinteger stah[4],stal[4],stad;
  geant pmpedh[ECPMSL][4],pmsigh[ECPMSL][4],pmpedl[ECPMSL][4],pmsigl[ECPMSL][4];
  geant pmpedd[ECPMSL],pmsigd[ECPMSL];
  uinteger pmstah[ECPMSL][4],pmstal[ECPMSL][4];
  uinteger pmstad[ECPMSL];
  integer slmx,pmmx;
  slmx=ECSLMX;//max.S-layers
  pmmx=ECPMSMX;//max.PMs in S-layer
  //
  // ---> create High(Low,dynode)Gain peds/sigmas/sta:
  //
  for(isl=0;isl<slmx;isl++){   
    for(ipm=0;ipm<pmmx;ipm++){  
      cnum=isl*pmmx+ipm; // sequential PM numbering(0 - SL*PMperSL)(324)
      for(isc=0;isc<4;isc++){   
        pmpedh[cnum][isc]=ECMCFFKEY.pedh*(1+0.01*ECMCFFKEY.pedvh*rnormx());
	if(pmpedh[cnum][isc]<1)pmpedh[cnum][isc]=1;
        pmsigh[cnum][isc]=ECMCFFKEY.pedsh*(1+0.01*ECMCFFKEY.pedsvh*rnormx());
	if(pmsigh[cnum][isc]<0.1)pmsigh[cnum][isc]=0.1;
        pmstah[cnum][isc]=0;
	//
        pmpedl[cnum][isc]=ECMCFFKEY.pedl*(1+0.01*ECMCFFKEY.pedvl*rnormx());
	if(pmpedl[cnum][isc]<1)pmpedl[cnum][isc]=1;
        pmsigl[cnum][isc]=ECMCFFKEY.pedsl*(1+0.01*ECMCFFKEY.pedsvl*rnormx());
	if(pmsigl[cnum][isc]<0.1)pmsigl[cnum][isc]=0.1;
        pmstal[cnum][isc]=0;
      }
      pmpedd[cnum]=ECMCFFKEY.pedd*(1+0.01*ECMCFFKEY.peddv*rnormx());
      if(pmpedd[cnum]<1)pmpedd[cnum]=1;
      pmsigd[cnum]=ECMCFFKEY.pedds*(1+0.01*ECMCFFKEY.peddsv*rnormx());
      if(pmsigd[cnum]<0.1)pmsigd[cnum]=0.1;
      pmstad[cnum]=0;
    }
  } 
  //---------------
  //
  for(isl=0;isl<slmx;isl++){
    for(ipm=0;ipm<pmmx;ipm++){
      cnum=isl*pmmx+ipm; // sequential PM numbering(0 - SL*PMperSL)(324)
      sid=(ipm+1)+100*(isl+1);
      for(isc=0;isc<4;isc++)stah[isc]=pmstah[cnum][isc];
      for(isc=0;isc<4;isc++)stal[isc]=pmstal[cnum][isc];
      for(isc=0;isc<4;isc++)pedh[isc]=pmpedh[cnum][isc];
      for(isc=0;isc<4;isc++)sigh[isc]=pmsigh[cnum][isc];
      for(isc=0;isc<4;isc++)pedl[isc]=pmpedl[cnum][isc];
      for(isc=0;isc<4;isc++)sigl[isc]=pmsigl[cnum][isc];
      pedd=pmpedd[cnum];
      sigd=pmsigd[cnum];
      stad=pmstad[cnum];
      pmpeds[isl][ipm]=ECPMPeds(sid,stah,stal,stad,pedh,sigh,pedl,sigl,pedd,sigd);
    }
  }
  cout<<"<---- ECPMPeds::MCbuild: Default peds/sigmas are created !"<<endl; 
}
//
//==========================================================================
//==========================================================================
//  ECTslope class functions :
//
void ECTslope::build(){// <--- create MC/RealData ecpmtslo-objects
  char fname[1024];
  char name[80];
  char datt[3];
  char ext[80];
  int ntypes;
  uinteger verids[10];
  int date[2],year,mon,day,hour,min,sec;
  tm begin;
  time_t utct;
  uinteger iutct;
  int ctyp;
  uinteger verid;
  //
  integer slmx,pmmx;
  slmx=ECSLMX;//max.S-layers
  pmmx=ECPMSMX;//max.PMs in S-layer
  //
  int cnum;
  geant tslope[ECPMSL][4];
  integer endflab;
  integer sid;
  geant tslopesc[4];
  //
  // ---> read list of calibration-files version-numbers (menu-file) :
  //
  strcpy(name,"EcalCflist");// basic name for vers.list-file  
  if(AMSJob::gethead()->isMCData()){
    strcpy(datt,"MC");
    sprintf(ext,"%d",ECMCFFKEY.calvern);//MC-versn
  }
  else{
    strcpy(datt,"RD");
    sprintf(ext,"%d",ECREFFKEY.calutc);//RD-utc
  }
  strcat(name,datt);
  strcat(name,".");
  strcat(name,ext);
  //
  if(ECCAFFKEY.cafdir==0)strcpy(fname,AMSDATADIR.amsdatadir);
  if(ECCAFFKEY.cafdir==1)strcpy(fname,"");
  strcat(fname,name);
  cout<<"====> ECTslope::build:Opening Calib_vers_list-file: "<<fname<<'\n';
  ifstream vlfile(fname,ios::in); // open needed tdfmap-file for reading
  if(!vlfile){
    cerr <<"<---- ECTslope_build:Error: missing Calib_vers_list-file "<<fname<<endl;
    exit(1);
  }
  vlfile >> ntypes;// total number of calibr. file types in the list
  for(int i=0;i<ntypes;i++){
    vlfile >> verids[i];
    CFlistC[i+1]=verids[i]; 
  }
  CFlistC[0]=ntypes;
  //
  if(AMSJob::gethead()->isMCData()){
    vlfile >> date[0];//YYYYMMDD beg.validity of TofCflistMC.ext file
    vlfile >> date[1];//HHMMSS ......................................
    year=date[0]/10000;//2004->
    mon=(date[0]%10000)/100;//1-12
    day=(date[0]%100);//1-31
    hour=date[1]/10000;//0-23
    min=(date[1]%10000)/100;//0-59
    sec=(date[1]%100);//0-59
    begin.tm_isdst=0;
    begin.tm_sec=sec;
    begin.tm_min=min;
    begin.tm_hour=hour;
    begin.tm_mday=day;
    begin.tm_mon=mon-1;
    begin.tm_year=year-1900;
    utct=mktime(& begin);
    iutct=uinteger(utct);
    cout<<"      EcalCflistMC-file begin_date: year:month:day = "<<year<<":"<<mon<<":"<<day<<endl;
    cout<<"                                     hour:min:sec = "<<hour<<":"<<min<<":"<<sec<<endl;
    cout<<"                                         UTC-time = "<<iutct<<endl;
    CFlistC[ntypes+1]=ECMCFFKEY.calvern;
    CFlistC[ntypes+2]=date[0];
    CFlistC[ntypes+3]=date[1];
  }
  else{
    utct=time_t(TFREFFKEY.calutc);
    printf("      EcalCflistRD-file begin_date: %s",ctime(&utct)); 
    CFlistC[ntypes+1]=ECREFFKEY.calutc;
  }
  //
  vlfile.close();
  //------------------------------
  //
  //   --->  Read Tslope calib. file :
  //
  if (ntypes<4) {
    cout << "WARNING: EcalCflist file too hold for T slopes: no T correction applied" << endl;
    for(int isl=0;isl<slmx;isl++){
      for(int ipm=0;ipm<pmmx;ipm++){
	sid=(ipm+1)+100*(isl+1);
	for(int isc=0;isc<4;isc++) tslopesc[isc]=0.;
	ecpmtslo[isl][ipm]=ECTslope(sid,tslopesc);
      }
    }
    return;
  }

  ctyp=4;//1st type of calibration is RLGA(really may be single)
  verid=verids[ctyp-1];//MC-versn or RD-utc
  strcpy(name,"EcalTslo");
  strcat(name,datt);
  strcat(name,".");
  sprintf(ext,"%d",verid);
  strcat(name,ext);
  if(ECCAFFKEY.cafdir==0)strcpy(fname,AMSDATADIR.amsdatadir);
  if(ECCAFFKEY.cafdir==1)strcpy(fname,"");
  strcat(fname,name);
  cout<<"      Opening Tslo-calib file "<<fname<<" ..."<<endl;
  ifstream tslofile(fname,ios::in); // open  file for reading
  if(!tslofile){
    cerr <<"<---- ECTslope_build: Error: missing Tslo-file !!? "<<fname<<endl;
    exit(1);
  }
  // ---> read PM-SubCell Temperature Slopes:
  //
  for(int isl=0;isl<slmx;isl++){   
    for(int isc=0;isc<4;isc++){   
      for(int ipm=0;ipm<pmmx;ipm++){  
        cnum=isl*pmmx+ipm; // sequential PM numbering(0 - SL*PMperSL)(324)
        tslofile >> tslope[cnum][isc];
      }
    }
  }
  // TSLOPE reading DEBUG
  if (ECREFFKEY.reprtf[1]==2){
    for(int isl=0;isl<slmx;isl++){   
      for(int isc=0;isc<4;isc++){   
	for(int ipm=0;ipm<pmmx;ipm++){ 
	  cout << "********* PMT " << ipm << " ";
	  cnum=isl*pmmx+ipm; // sequential PM numbering(0 - SL*PMperSL)(324) 
	  cout <<  tslope[cnum][isc] << endl;
	}
	cout << endl;
      }
      cout << endl;
    }
  }
  // ---> read endfile-label:
  //
  tslofile >> endflab;
  //
  tslofile.close();
  if(endflab==12345){
    cout<<"      Tslo-calibration file is successfully read !"<<endl;
  }
  else{cout<<"<---- ECTslope::build: ERROR: problems while reading Tslo-calib.file !!?"<<endl;
    exit(1);
  }
  //
  for(int isl=0;isl<slmx;isl++){
    for(int ipm=0;ipm<pmmx;ipm++){
      cnum=isl*pmmx+ipm; // sequential PM numbering(0 - SL*PMperSL)(324)
      sid=(ipm+1)+100*(isl+1);
      for(int isc=0;isc<4;isc++) tslopesc[isc]=tslope[cnum][isc];
      ecpmtslo[isl][ipm]=ECTslope(sid,tslopesc);
    }
  }
  cout<<"<---- ECTslope::build: successfully done !"<<endl<<endl;
}
//-----------------------------------------------------------
int ECTslope::GetECALSensorTemper(int superlayer,int column,float* temp){

  const int nCOLUMNs=36;
    
  const int nFACEs=4;
  const int nCOLUMNGROUPs=3;
  const int nSENSORGROUPs=2;
  const int nNODEs=2;

  vector<float> values;
  int stat;

  unsigned int fTime;
  float frac;

  string SensorName[nFACEs][nCOLUMNGROUPs][nSENSORGROUPs]={
    {  
      {"S1","S2"},
      {"S3","S4"},
      {"S5","S6"}},
    {
      {"W1","W2"},
      {"W3","W4"},
      {"W5","W6"}},
    {
      {"P5","P6"},
      {"P3","P4"},
      {"P1","P2"}},
    {
      {"R5","R6"},
      {"R3","R4"},
      {"R1","R2"}}
  };

  string NodeName[nNODEs]={"JPD_A","JPD_D"};

  int columnsingroup;
  columnsingroup= nCOLUMNs/nCOLUMNGROUPs;
  int columngroup;
  columngroup = (int)(column/columnsingroup);
  int face;
  if ( superlayer %2 == 0 ) { // Y view
    if (column%2 == 0 ){ // face A
      face = 0;
    }
    else{ // face C
      face = 2;
    }
  }
  else{ // X view
    if (column%2 == 0 ){ // face B
      face = 1;
    }
    else{ // face D
      face = 3;
    }
  }

  //Header:    
  fTime = AMSEvent::gethead()->gettime();
  frac= float(AMSEvent::gethead()->getusec())/1000000.;
  //cout << "Time= " << fTime << " " << " frac=" << frac << endl;

  int methode(1);

  char elemname[22];
  int nsens=0;
  *temp=0;
  for (int isensorgroup=0;isensorgroup<nSENSORGROUPs;isensorgroup++){
    sprintf(elemname,"ECAL %s",SensorName[face][columngroup][isensorgroup].data());
    if (ECREFFKEY.reprtf[1]==3)    cout << elemname << endl;
    for (int inode=0;inode<nNODEs;inode++){
      stat=AMSNtuple::Get_setup02()->fSlowControl.GetData(elemname,fTime, frac,values,methode,NodeName[inode].data());
      //cout << "STAT=" << stat << endl;
      if (ECREFFKEY.reprtf[1]==3)  cout << NodeName[inode].c_str() << " " << values[0] << endl;
      if (stat == 0) {
	*temp+=values[0];
	nsens++;
      }
    }
  }
  if (nsens==0) {
    *temp=-999.;
    return 1;
  }
   
  *temp/=nsens;

  return 0;
}

//==========================================================================
//  ECDailyMip class functions :
//
void ECDailyMip::build(){// <--- create MC/RealData ecmipday-objects
  int cnum;
  //
  char fname[1024];
  char name[80];
  char datt[3];
  char ext[80];
  int ntypes;
  uinteger verids[10];
  int date[2],year,mon,day,hour,min,sec;
  tm begin;
  time_t utct;
  uinteger iutct;
  int ctyp;
  uinteger verid;
  //
  geant mipday[ECPMSL][ECSUBCELLMX];
  integer endflab;
  integer sid;
  geant mip_day[ECSUBCELLMX];
  //
  // ---> read list of calibration-files version-numbers (menu-file) :
  //
  strcpy(name,"EcalCflist");// basic name for vers.list-file  
  if(AMSJob::gethead()->isMCData()){
    strcpy(datt,"MC");
    sprintf(ext,"%d",ECMCFFKEY.calvern);//MC-versn
  }
  else{
    strcpy(datt,"RD");
    sprintf(ext,"%d",ECREFFKEY.calutc);//RD-utc
  }
  strcat(name,datt);
  strcat(name,".");
  strcat(name,ext);
  //
  if(ECCAFFKEY.cafdir==0)strcpy(fname,AMSDATADIR.amsdatadir);
  if(ECCAFFKEY.cafdir==1)strcpy(fname,"");
  strcat(fname,name);
  cout<<"====> ECDailyMip::build:Opening Calib_vers_list-file: "<<fname<<'\n';
  ifstream vlfile(fname,ios::in); // open needed tdfmap-file for reading
  if(!vlfile){
    cerr <<"<---- ECDailyMip_build:Error: missing Calib_vers_list-file "<<fname<<endl;
    exit(1);
  }
  vlfile >> ntypes;// total number of calibr. file types in the list
  for(int i=0;i<ntypes;i++){
    vlfile >> verids[i];
    CFlistC[i+1]=verids[i]; 
  }
  CFlistC[0]=ntypes;
  //
  if(AMSJob::gethead()->isMCData()){
    vlfile >> date[0];//YYYYMMDD beg.validity of TofCflistMC.ext file
    vlfile >> date[1];//HHMMSS ......................................
    year=date[0]/10000;//2004->
    mon=(date[0]%10000)/100;//1-12
    day=(date[0]%100);//1-31
    hour=date[1]/10000;//0-23
    min=(date[1]%10000)/100;//0-59
    sec=(date[1]%100);//0-59
    begin.tm_isdst=0;
    begin.tm_sec=sec;
    begin.tm_min=min;
    begin.tm_hour=hour;
    begin.tm_mday=day;
    begin.tm_mon=mon-1;
    begin.tm_year=year-1900;
    utct=mktime(& begin);
    iutct=uinteger(utct);
    cout<<"      EcalCflistMC-file begin_date: year:month:day = "<<year<<":"<<mon<<":"<<day<<endl;
    cout<<"                                     hour:min:sec = "<<hour<<":"<<min<<":"<<sec<<endl;
    cout<<"                                         UTC-time = "<<iutct<<endl;
    CFlistC[ntypes+1]=ECMCFFKEY.calvern;
    CFlistC[ntypes+2]=date[0];
    CFlistC[ntypes+3]=date[1];
  }
  else{
    utct=time_t(TFREFFKEY.calutc);
    printf("      EcalCflistRD-file begin_date: %s",ctime(&utct)); 
    CFlistC[ntypes+1]=ECREFFKEY.calutc;
  }
  //
  vlfile.close();
  //------------------------------
  //
  //   --->  Read DailyMip calib. file :
  //
  if (ntypes<5) {
    cout << "WARNING: EcalCflist file too hold for Mip values: reference equalization applied (not yet implemented)" << endl;

    for(int isl=0;isl<ECSLMX;isl++){
      for(int ipm=0;ipm<ECPMMX;ipm++){
	cnum=isl*ECPMMX+ipm; // sequential PM numbering(0 - SL*PMperSL)(324)
	sid=(ipm+1)+100*(isl+1);
	for(int isc=0;isc<4;isc++) mip_day[isc]=16.; // to be changed
	ecmipday[isl][ipm]=ECDailyMip(sid,mip_day);
      }
    }
    return;
  }

  ctyp=5;//5th type of calibration is MIP value
  verid=verids[ctyp-1];//MC-versn or RD-utc
  strcpy(name,"EcalMipD");
  strcat(name,datt);
  strcat(name,".");
  sprintf(ext,"%d",verid);
  strcat(name,ext);
  if(ECCAFFKEY.cafdir==0)strcpy(fname,AMSDATADIR.amsdatadir);
  if(ECCAFFKEY.cafdir==1)strcpy(fname,"");
  strcat(fname,name);
  cout<<"      Opening DailyMip-calib file "<<fname<<" ..."<<endl;
  ifstream mipdayfile(fname,ios::in); // open  file for reading
  if(!mipdayfile){
    cerr <<"<---- ECDailyMip_build: Error: missing DailyMip-file !!? "<<fname<<endl;
    exit(1);
  }
  // ---> read Cell MIP values:
  //
  for (int isuperlayer=0;isuperlayer<ECSLMX;isuperlayer++){
    for (int isubcell=0;isubcell<ECSUBCELLMX;isubcell++){
      for (int ipmt=0;ipmt<ECPMMX;ipmt++){
        cnum=isuperlayer*ECPMMX+ipmt; // sequential PM numbering(0 - SL*PMperSL)(324)
	mipdayfile >> mipday[cnum][isubcell];
      }
    }
  }
  // DailyMip reading DEBUG
  int superlayer,pmt,subcell;
  if (ECREFFKEY.reprtf[1]==2){
    for(int ilayer=0;ilayer<ECLAYERMX;ilayer++){ 
      superlayer = ilayer/2;
      for(int icell=0;icell<ECCELLMX;icell++){ 
	pmt = icell/2;
        cnum= superlayer*ECPMMX+pmt; // sequential PM numbering(0 - SL*PMperSL)(324)
	subcell= (ilayer%2==0?icell%2:icell%2+2);
	cout << "********* LAYER " << ilayer << " CELL " << icell << " ";
	cout <<  mipday[cnum][subcell] << endl;
      }
      cout << endl;
    }
    cout << endl;
  }
  // ---> read endfile-label:
  //
  mipdayfile >> endflab;
  //
  mipdayfile.close();
  if(endflab==12345){
    cout<<"      DailyMip-calibration file is successfully read !"<<endl;
  }
  else{cout<<"<---- ECDailyMip::build: ERROR: problems while reading DailyMip-calib.file !!?"<<endl;
    exit(1);
  }
  //
  for(int isl=0;isl<ECSLMX;isl++){
    for(int ipm=0;ipm<ECPMMX;ipm++){
      cnum=isl*ECPMMX+ipm; // sequential PM numbering(0 - SL*PMperSL)(324)
      sid=(ipm+1)+100*(isl+1);
      for(int isc=0;isc<4;isc++) mip_day[isc]=mipday[cnum][isc];
      ecmipday[isl][ipm]=ECDailyMip(sid,mip_day);
    }
  }
  cout<<"<---- ECDailyMip::build: successfully done !"<<endl<<endl;
}
//==========================================================================
//  ECLongTerm class functions :
//
void ECLongTerm::build(){// <--- create MC/RealData eclongterm-objects
  int cnum;
  integer sid,sta[ECSUBCELLMX],stad,endflab;
  geant h2lr[ECSUBCELLMX],a2m,a2dr,lfs[ECSUBCELLMX],lsl[ECSUBCELLMX],ffr[ECSUBCELLMX];
  geant h2lo[ECSUBCELLMX];

  char fname[1024];
  char name[80];
  char datt[3];
  char ext[80];
  int ntypes;
  uinteger verids[10];
  int date[2],year,mon,day,hour,min,sec;
  tm begin;
  time_t utct;
  uinteger iutct;
  int ctyp;
  uinteger verid;
  //
  //
  integer status[ECPMSL][ECSUBCELLMX],statusd[ECPMSL];
  geant sch2lr[ECPMSL][ECSUBCELLMX],an2dyr[ECPMSL],adc2mev;
  geant sch2lo[ECPMSL][ECSUBCELLMX];
  geant lfast[ECPMSL][ECSUBCELLMX],lslow[ECPMSL][ECSUBCELLMX],fastf[ECPMSL][ECSUBCELLMX];


  // ---> read list of calibration-files version-numbers (menu-file) :
  //
  strcpy(name,"EcalCflist");// basic name for vers.list-file  
  if(AMSJob::gethead()->isMCData()){
    strcpy(datt,"MC");
    sprintf(ext,"%d",ECMCFFKEY.calvern);//MC-versn
  }
  else{
    strcpy(datt,"RD");
    sprintf(ext,"%d",ECREFFKEY.calutc);//RD-utc
  }
  strcat(name,datt);
  strcat(name,".");
  strcat(name,ext);
  //
  if(ECCAFFKEY.cafdir==0)strcpy(fname,AMSDATADIR.amsdatadir);
  if(ECCAFFKEY.cafdir==1)strcpy(fname,"");
  strcat(fname,name);
  cout<<"====> ECLongTerm::build:Opening Calib_vers_list-file: "<<fname<<'\n';
  ifstream vlfile(fname,ios::in); // open needed tdfmap-file for reading
  if(!vlfile){
    cerr <<"<---- ECLongTerm_build:Error: missing Calib_vers_list-file "<<fname<<endl;
    exit(1);
  }
  vlfile >> ntypes;// total number of calibr. file types in the list
  for(int i=0;i<ntypes;i++){
    vlfile >> verids[i];
    CFlistC[i+1]=verids[i]; 
  }
  CFlistC[0]=ntypes;
  //
  if(AMSJob::gethead()->isMCData()){
    vlfile >> date[0];//YYYYMMDD beg.validity of TofCflistMC.ext file
    vlfile >> date[1];//HHMMSS ......................................
    year=date[0]/10000;//2004->
    mon=(date[0]%10000)/100;//1-12
    day=(date[0]%100);//1-31
    hour=date[1]/10000;//0-23
    min=(date[1]%10000)/100;//0-59
    sec=(date[1]%100);//0-59
    begin.tm_isdst=0;
    begin.tm_sec=sec;
    begin.tm_min=min;
    begin.tm_hour=hour;
    begin.tm_mday=day;
    begin.tm_mon=mon-1;
    begin.tm_year=year-1900;
    utct=mktime(& begin);
    iutct=uinteger(utct);
    cout<<"      EcalCflistMC-file begin_date: year:month:day = "<<year<<":"<<mon<<":"<<day<<endl;
    cout<<"                                     hour:min:sec = "<<hour<<":"<<min<<":"<<sec<<endl;
    cout<<"                                         UTC-time = "<<iutct<<endl;
    CFlistC[ntypes+1]=ECMCFFKEY.calvern;
    CFlistC[ntypes+2]=date[0];
    CFlistC[ntypes+3]=date[1];
  }
  else{
    utct=time_t(TFREFFKEY.calutc);
    printf("      EcalCflistRD-file begin_date: %s",ctime(&utct)); 
    CFlistC[ntypes+1]=ECREFFKEY.calutc;
  }
  //
  vlfile.close();
  //------------------------------
  //
  //   --->  Read LongTerm calib. file :
  //
  if (ntypes<6) {
    cout << "WARNING: EcalCflist file too hold for Long Term calibration: reference calibration applied (not yet implemented)" << endl;

    for(int isuperlayer=0;isuperlayer<ECSLMX;isuperlayer++){
      for(int ipmt=0;ipmt<ECPMMX;ipmt++){
	cnum=isuperlayer*ECPMMX+ipmt; // sequential PM numbering(0 - SL*PMperSL)(32)
	sid=(ipmt+1)+100*(isuperlayer+1);
	for(int isc=0;isc<ECSUBCELLMX;isc++)sta[isc]=0;//anode pixels
	stad=0;//dynode
	for(int isc=0;isc<ECSUBCELLMX;isc++) h2lr[isc]=33;
	for(int isc=0;isc<ECSUBCELLMX;isc++) h2lo[isc]=0;
	a2dr=1;
	
	for(int isc=0;isc<ECSUBCELLMX;isc++) lfs[isc]=30;
	for(int isc=0;isc<ECSUBCELLMX;isc++) lsl[isc]=300;
	for(int isc=0;isc<ECSUBCELLMX;isc++) ffr[isc]=0.3;
	a2m=0.4704;
	eclongterm[isuperlayer][ipmt]=ECLongTerm(sid,sta,stad,h2lr,h2lo,a2dr,lfs,lsl,ffr,a2m);
      }
    }
    
    return;
  }
  //================================================================== 
  //
  //   --->  Read abs.norm. calib. file :
  //
  ctyp=3;//3rd type of calibration 
  verid=verids[ctyp-1];//MC-versn or RD-utc
  strcpy(name,"EcalAnor");
  strcat(name,datt);
  strcat(name,".");
  sprintf(ext,"%d",verid);
  strcat(name,ext);
  if(ECCAFFKEY.cafdir==0)strcpy(fname,AMSDATADIR.amsdatadir);
  if(ECCAFFKEY.cafdir==1)strcpy(fname,"");
  strcat(fname,name);
  cout<<"      Opening ANOR-calib.file "<<fname<<" ..."<<endl;
  ifstream anrfile(fname,ios::in); // open  file for reading
  if(!anrfile){
    cerr <<"<---- ECcalib_build:Error: missing ANOR-calib file !!? "<<fname<<endl;
    exit(1);
  }
  //
  // ---> read common(hope) adc2mev convertion factor(abs.norm):
  //
  anrfile >> adc2mev;
  // DEBUG
  if (ECREFFKEY.reprtf[1]==2) cout << "ADC2MEV=" << adc2mev << endl;
  //
  // ---> read endfile-label:
  //
  anrfile >> endflab;
  //
  anrfile.close();
  if(endflab==12345){
    cout<<"      ANOR-calibration file is successfully read !"<<endl;
  }
  else{cout<<"<---- ECcalib_build: ERROR: problems while reading ANOR-calib.file !!?"<<endl;
    exit(1);
  }
    


  ctyp=6;//6th type of calibration contains long term calibration constants
  verid=verids[ctyp-1];//MC-versn or RD-utc
  strcpy(name,"EcalLonT");
  strcat(name,datt);
  strcat(name,".");
  sprintf(ext,"%d",verid);
  strcat(name,ext);
  if(ECCAFFKEY.cafdir==0)strcpy(fname,AMSDATADIR.amsdatadir);
  if(ECCAFFKEY.cafdir==1)strcpy(fname,"");
  strcat(fname,name);
  cout<<"      Opening LongTerm-calib file "<<fname<<" ..."<<endl;
  ifstream longtermfile(fname,ios::in); // open  file for reading
  if(!longtermfile){
    cerr <<"<---- ECLongTerm_build: Error: missing LongTerm-file !!? "<<fname<<endl;
    exit(1);
  }

  //
  // ---> read Cells att.parameters(Latt_fast/Latt_slow/Fast_fraction): 
  //
  for (int isuperlayer=0;isuperlayer<ECSLMX;isuperlayer++){
    for (int isubcell=0;isubcell<ECSUBCELLMX;isubcell++){
      for (int ipmt=0;ipmt<ECPMMX;ipmt++){
        cnum=isuperlayer*ECPMMX+ipmt; // sequential PM numbering(0 - SL*PMperSL)(324)
	longtermfile >> lfast[cnum][isubcell];
      }
    }
  }
  //  cout << "LAST lfast=" << lfast[cnum][3] << endl;
  for (int isuperlayer=0;isuperlayer<ECSLMX;isuperlayer++){
    for (int isubcell=0;isubcell<ECSUBCELLMX;isubcell++){
      for (int ipmt=0;ipmt<ECPMMX;ipmt++){
        cnum=isuperlayer*ECPMMX+ipmt; // sequential PM numbering(0 - SL*PMperSL)(324)
	longtermfile >> lslow[cnum][isubcell];
      }
    }
  }
  //  cout << "LAST lslow=" << lslow[cnum][3] << endl;
  for (int isuperlayer=0;isuperlayer<ECSLMX;isuperlayer++){
    for (int isubcell=0;isubcell<ECSUBCELLMX;isubcell++){
      for (int ipmt=0;ipmt<ECPMMX;ipmt++){
        cnum=isuperlayer*ECPMMX+ipmt; // sequential PM numbering(0 - SL*PMperSL)(324)
	longtermfile >> fastf[cnum][isubcell];
      }
    }
  }
  // cout << "LAST fastf=" << fastf[cnum][3] << endl;

  //
  // ---> read anode and dynode status:
  //
  for(int isuperlayer=0;isuperlayer<ECSLMX;isuperlayer++){   
    for(int isc=0;isc<ECSUBCELLMX;isc++){   
      for(int ipmt=0;ipmt<ECPMMX;ipmt++){  
        cnum=isuperlayer*ECPMMX+ipmt; // sequential PM numbering(0 - SL*PMperSL)(324)
        longtermfile >> status[cnum][isc];//pixel's status
      }
    }
    for(int ipmt=0;ipmt<ECPMMX;ipmt++){
      cnum=isuperlayer*ECPMMX+ipmt; // sequential PM numbering(0 - SL*PMperSL)(324)
      longtermfile >> statusd[cnum];//dynode's status
    }
  } 
  //  cout << "LAST anode status=" << status[cnum][3] << endl;
  //  cout << "LAST dynode status=" << statusd[cnum] << endl;

  //
  // ---> read PM-SubCell high/low gain ratios:
  //
  for(int isuperlayer=0;isuperlayer<ECSLMX;isuperlayer++){   
    for(int isc=0;isc<ECSUBCELLMX;isc++){   
      for(int ipmt=0;ipmt<ECPMMX;ipmt++){  
        cnum=isuperlayer*ECPMMX+ipmt; // sequential PM numbering(0 - SL*PMperSL)(324)
        longtermfile >> sch2lr[cnum][isc];
      }
    }
  }
  // cout << "LAST hi/lo ratio=" << sch2lr[cnum][3] << endl;

  //
  // ---> read PM-SubCell high/low gain offsets:
  //
  for(int isuperlayer=0;isuperlayer<ECSLMX;isuperlayer++){   
    for(int isc=0;isc<ECSUBCELLMX;isc++){   
      for(int ipmt=0;ipmt<ECPMMX;ipmt++){  
        cnum=isuperlayer*ECPMMX+ipmt; // sequential PM numbering(0 - SL*PMperSL)(324)
        longtermfile >> sch2lo[cnum][isc];
      }
    }
  }
  //  cout << "LAST hi/lo offset=" << sch2lo[cnum][3] << endl;

  //
  // ---> read PM anode(sum of 4-SubCells)-to-Dynode ratious:
  //
  for(int isuperlayer=0;isuperlayer<ECSLMX;isuperlayer++){ 
    for(int ipmt=0;ipmt<ECPMMX;ipmt++){ 
      cnum=isuperlayer*ECPMMX+ipmt; // sequential PM numbering(0 - SL*PMperSL)(324)
      longtermfile >> an2dyr[cnum];
    }
  }
  //  cout << "LAST anode/dynode=" << an2dyr[cnum] << endl;
  // ---> read endfile-label:
  //
  longtermfile >> endflab;
  //
  longtermfile.close();
  if(endflab==12345){
    cout<<"   ECAL   LONGTERM-calibration file is successfully read !"<<endl;
  }
  else{cout<<"<---- ECLongTerm::build: ERROR: problems while reading EcalLonT-calib.file !!?"<<endl;
    exit(1);
  }  
  //
  for(int isuperlayer=0;isuperlayer<ECSLMX;isuperlayer++){
    for(int ipmt=0;ipmt<ECPMMX;ipmt++){
      cnum=isuperlayer*ECPMMX+ipmt; // sequential PM numbering(0 - SL*PMperSL)(32)
      sid=(ipmt+1)+100*(isuperlayer+1);
      for(int isc=0;isc<ECSUBCELLMX;isc++)sta[isc]=status[cnum][isc];//anode pixels
      stad=statusd[cnum];//dynode
      for(int isc=0;isc<ECSUBCELLMX;isc++) h2lr[isc]=sch2lr[cnum][isc];
      for(int isc=0;isc<ECSUBCELLMX;isc++) h2lo[isc]=sch2lo[cnum][isc];
      a2dr=an2dyr[cnum];

      for(int isc=0;isc<ECSUBCELLMX;isc++) lfs[isc]=lfast[cnum][isc];
      for(int isc=0;isc<ECSUBCELLMX;isc++) lsl[isc]=lslow[cnum][isc];
      for(int isc=0;isc<ECSUBCELLMX;isc++) ffr[isc]=fastf[cnum][isc];
      a2m=adc2mev;
      eclongterm[isuperlayer][ipmt]=ECLongTerm(sid,sta,stad,h2lr,h2lo,a2dr,lfs,lsl,ffr,a2m);
    }
  }
  cout<<"<---- ECLongTerm::build: successfully done !"<<endl<<endl;
}
