//  $Id: ecaldbc.C,v 1.26 2001/08/01 13:28:43 choutko Exp $
// Author E.Choumilov 14.07.99.
#include <typedefs.h>
#include <math.h>
#include <commons.h>
#include <job.h>
#include <ecaldbc.h>
#include <stdio.h>
#include <iostream.h>
#include <fstream.h>
#include <ecalcalib.h>
//
using namespace ecalconst;
ECcalib ECcalib::ecpmcal[ECSLMX][ECPMSMX];// mem.reserv.for ECAL indiv.PMcell calib. param.
ECPMPeds ECPMPeds::pmpeds[ECSLMX][ECPMSMX];// ..........for ECAL peds,sigmas
ECALVarp ECALVarp::ecalvpar;// .........................for ECAL general run-time param.  
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
geant ECALDBc::_gendim[10]={
   65.8,65.8,0., // i=1-2  x,y-dimentions of EC-radiator; 3->spare
   11.4,          //  =4    dx(dy) thickn.of (PMT+electronics)-volume
   0.,0.,       //  =5,6    center shift in x,y   
  -142.3,       //  =7      Radiator(incl.Al-plate !!!) front face Z-pozition
   5.,          //  =8      top(bot) honeycomb thickness
   1.75,        //  =9      lead thickness of 1 superlayer
   0.05        //  =10      Thickness of Al-plate on top(bot) of superlayer 
};
//
geant ECALDBc::_fpitch[3]={
   0.135,      // i=1   fiber pitch in X(Y) projection
   0.173,0.   // i=2 fiber pitch in Z (inside super-layer); =3->spare
};
//
geant ECALDBc::_rdcell[10]={
   368.,32.5,0.15,//i=1,3  MC-def fib.att.length slow)/fast/fast_fract(real values from DB !!)
   0.094,        // i=4    fiber diameter(0.1-2x0.003cm)
   0.9,          // i=5    size(dx=dz) of "1/4" of PMT-cathod (pixel)
   0.45,         // i=6    abs(x(z)-position) of "1/4" in PMT coord.syst.
   1.8,          // i=7    X(Y)-pitch of PMT's;
   0.008,        // i=8    fiber wall(cladd)+glue thickn(.003+.005cm) to have hole diam.=0.11
   0.,0.         // i=9,10 spare
};
//
integer ECALDBc::_slstruc[6]={
    1,           // i=1   1st super-layer projection(0->X, 1->Y)
   10,           // i=2   numb. of fiber-layers per super-layer
   9,            //  =3   real numb. of super-layers (X+Y)
   36,           //  =4   real numb. of PMT's per super-layer (in X(Y))
   1,1           //  =5-6 readout dir. in X/Y-proj (=1/-1->+/-) for the 1st PM-cell.  
};
//
integer ECALDBc::_nfibpl[2]={
   486,485       // i=1,2 numb. of fibers per 1st/2nd fiber-layer in S-layer
};
//
int ECALDBc::_scalef=2;// MC/Data scale factor used in ADC->DAQ-value conversion.
//
//  member functions :
//
// ---> function to read geomconfig-files 
//                  Called from ECALgeom :
  void ECALDBc::readgconf(){
    int i;
    char fname[80];
    char name[80]="ecalgeom";
    char vers1[3]="01";
    char vers2[4]="001";
    char vers3[3]="02";
    if(strstr(AMSJob::gethead()->getsetup(),"AMSSHUTTLE"))
    {
          cout <<" ECALGeom-I-Shuttle setup selected."<<endl;
          strcat(name,vers1);
    }
    else if (strstr(AMSJob::gethead()->getsetup(),"AMS02")){
          cout <<" ECALGeom-I-AMS02 setup selected."<<endl;
          strcat(name,vers3);
    }
    else
    {
          cout <<" ECALGeom-I-AMS001 setup selected."<<endl;
          strcat(name,vers2);
    }
    strcat(name,".dat");
//    if(ECCAFFKEY.cafdir==0)strcpy(fname,AMSDATADIR.amsdatadir);
//    if(ECCAFFKEY.cafdir==1)strcpy(fname,"");
//    strcat(fname,name);
//    cout<<"ECALDBc::readgconf: Open file : "<<fname<<'\n';
//    ifstream tcfile(fname,ios::in); // open needed config-file for reading
//    if(!tcfile){
//      cerr <<"ECgeom-read: missing geomconfig-file "<<fname<<endl;
//      exit(1);
//    }
//    for(int ic=0;ic<SCBLMX;ic++) tcfile >> _brtype[ic];
  }
//---
//
  geant ECALDBc::gendim(integer i){
    #ifdef __AMSDEBUG__
      if(ECALDBc::debug){
        assert(i>0 && i <= 10);
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
  number ECALDBc::segarea(number r, number ds){//small_segment area fraction (wrt full disk)
//                                    r-radious, ds-horde_distance(from center)
    number sina,cs,sn,a;
    if(ds>=r)return(0.);
    cs=ds/r;
    sn=sqrt(1.-cs*cs);
    sina=2.*cs*sn;
    a=2.*asin(sn);//sector opening angle:(0->pi) when ds:(r->0)
    return ((a-sina)/2./AMSDBc::pi);//0.5->0 when ds  0->r
  }
//---
// fiberID(SSLLFFF) to cellID(SSPPC) conversion
// ("analog" design - fiber_edep division between neigb.pixels/pmts with fraction w)
//
  void ECALDBc::fid2cida(integer fid, integer cid[4], number w[4]){
    integer fidd,fff,ss,ll,ip,nfl,npm,pm,ctbin,czbin,cell,bran,tbc;
    number cleft,tleft,bdis,ztop,ct,cz,ww;
    geant pit,piz,pmdis,dist,pmpit,pmsiz,pxsiz,fr;
    nfl=_slstruc[1];// numb.of fiber-layers per super-layer
    npm=_slstruc[3];// numb.of PM's per super-layer
    pit=_fpitch[0];// fiber pitch(transv)
    piz=_fpitch[1];// fiber pitch(in z)
    pmpit=_rdcell[6];//PM-pitch(transv)
    pmsiz=_rdcell[4]*2.;//PM_size
    pxsiz=_rdcell[4];// SubCell(pixel) size
    fr=_rdcell[3]/2;//   fiber radious 
    fidd=fid/1000;
    fff=fid%1000-1;
    ll=fidd%100-1;
    ss=fidd/100;
    ip=ll%2;
    if(ip==0)cleft=-(_nfibpl[0]-1)*pit/2.;//     fiber from 1st layer of s-layer
    else cleft=-(_nfibpl[1]-1)*pit/2.;//         fiber from 2nd layer of s-layer
    ct=cleft+fff*pit;//       transv.coord. of fiber in ECAL r(eference) s(ystem)
    ztop=(nfl-1)*piz/2.;//       z-pos of 1st(from top) f-layer of s-layer
    cz=ztop-ll*piz;//      z-pos of fiber in Slayer r.s.(z=0->middle of super-layer)
    tleft=-npm*pmsiz/2.;//     low-edge PM-bin transv.position in ECAL r.s.
    dist=ct-tleft;//           fiber-center dist from the 1st PM (its left edge)
    pm=integer(floor(dist/pmsiz));//   number of fired PM  0-(npm-1)   (IMPLY pmpit=pmsiz !!!)
    if(pm<0 || pm>=npm)return;//    (out of sensitive area - no signal is readout)
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
        bdis=pmdis;
	w[0]=1.-segarea(fr,bdis);
	cid[0]=1000*ss+(pm+1)*10+(cell+1);
	if(pm==0)return;// 1st PM
	w[1]=1.-w[0];
	cell=1+tbc;
	cid[1]=1000*ss+(pm)*10+(cell+1);// left neighbour
        break;
//        
      case 2:  //<-- near the right PM-boundary
        cell=1+tbc;
        bdis=pmsiz-pmdis;
	w[0]=1.-segarea(fr,bdis);
	cid[0]=1000*ss+(pm+1)*10+(cell+1);
	if(pm==(npm-1))return;// last PM
	w[1]=1.-w[0];
	cell=0+tbc;
	cid[1]=1000*ss+(pm+2)*10+(cell+1);// right neighbour
        break;
//        
      case 3:  //<-- completely inside PM
        bdis=pmdis-pxsiz;// f-center dist fron vertical pixel boundary
        if(bdis<=-fr){// <-- completely in the left half of PM
          cell=0+tbc;
	  cid[0]=1000*ss+(pm+1)*10+(cell+1);
	  w[0]=1.;
        }
        else if(bdis>=fr){// <-- completely in the right half of PM
          cell=1+tbc;
	  cid[0]=1000*ss+(pm+1)*10+(cell+1);
	  w[0]=1.;
        }
        else{ // <-- lefr-right sharing
	  ww=segarea(fr,fabs(bdis));
          cell=0+tbc;
	  cid[0]=1000*ss+(pm+1)*10+(cell+1);
          cell=1+tbc;
	  cid[1]=1000*ss+(pm+1)*10+(cell+1);
	  if(bdis<0.){
	    w[0]=1.-ww;	    
	    w[1]=ww;	    
	  }
	  else{
	    w[0]=ww;	    
	    w[1]=1.-ww;	    
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
  void ECALDBc::getscinfoa(integer isl, integer pmc, integer sc,
         integer &pr, integer &pl, integer &cell, number &ct, number &cl, number &cz){
//
  int i,ip,sl,fl,fb,fi,pm,ce,nf[4];
  number z,z11,t,tleft;
  static int first=0;
  static number cool[2];
  static number coot[ECPMSMX][4];
  static number cooz[ECSLMX][4];
//
  int nsl=_slstruc[2];// numb.of super-layers
  int nfl=_slstruc[1];// numb.of fiber-layers per super-layer
  int npm=_slstruc[3];// numb.of PM's per super-layer
  int nfb;
  geant dzrad1=_gendim[8];// z-size of 1SL EC radiator(lead)
  geant alpth=_gendim[9];// Al-plate(on rad. top/bot) thickness
  geant pmpit=_rdcell[6];//PM-pitch(transv)
  geant pxsiz=_rdcell[4];// SubCell(pixel) size
  geant piz=_fpitch[1];
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
      cout<<"ECALDBc::getscinfoa: 1st fiber-layer Zpos="<<z<<endl;
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
// temporary -> work for eugeni

    return _rdcell[4];
  }



//==========================================================================
//
//   EcalJobStat static variables definition (memory reservation):
//
integer EcalJobStat::mccount[ECJSTA];
integer EcalJobStat::recount[ECJSTA];
integer EcalJobStat::cacount[ECJSTA];
integer EcalJobStat::srcount[10];
geant EcalJobStat::zprmc1[ECSLMX];// mc-hit average Z-profile(SL-layers) 
geant EcalJobStat::zprmc2[ECSLMX];// mc-hit(+att) average Z-profile(SL(PM-assigned)-layers) 
geant EcalJobStat::zprofa[2*ECSLMX];// average Z-profile(scPlanes) 
geant EcalJobStat::zprofapm[ECSLMX];// average Z-profile(pm-layers) 
geant EcalJobStat::zprofac[ECSLMX];// SL Edep profile (punch-through events)
geant EcalJobStat::nprofac[ECSLMX];// SL profile (punch-through events)
//
//------------------------------------------
void EcalJobStat::clear(){
  int i,j;
  for(i=0;i<ECJSTA;i++)mccount[i]=0;
  for(i=0;i<ECJSTA;i++)recount[i]=0;
  for(i=0;i<ECJSTA;i++)cacount[i]=0;
  for(i=0;i<10;i++)srcount[i]=0;
  for(i=0;i<ECSLMX;i++)zprmc1[i]=0;
  for(i=0;i<ECSLMX;i++)zprmc2[i]=0;
  for(i=0;i<2*ECSLMX;i++)zprofa[i]=0;
  for(i=0;i<ECSLMX;i++)zprofapm[i]=0;
  for(i=0;i<ECSLMX;i++)zprofac[i]=0;
  for(i=0;i<ECSLMX;i++)nprofac[i]=0;
}
//------------------------------------------
// function to print Job-statistics at the end of JOB(RUN):
void EcalJobStat::printstat(){
//
  printf("\n");
  printf("    ====================== ECAL JOB-statistics ======================\n");
  printf("\n");
  printf(" MC: entries                       : % 6d\n",mccount[0]);
  printf(" MC: MCHit->RawEven(ECTrigfl>0) OK : % 6d\n",mccount[1]);
  printf(" MC Trig.procedure: entries(>=MIP) : % 6d\n",srcount[0]);
  printf("                    Efront OK      : % 6d\n",srcount[1]);
  printf("                    Epk/Ebs OK     : % 6d\n",srcount[2]);
  printf("                    Epk/Efr OK     : % 6d\n",srcount[3]);
  printf("                    TrWidth OK     : % 6d\n",srcount[4]);
  printf(" RECO-entries                      : % 6d\n",recount[0]);
  printf(" LVL1-trig includes ECAL           : % 6d\n",recount[1]);
  printf(" Validation OK                     : % 6d\n",recount[2]);
  printf(" RawEvent->EcalHit OK              : % 6d\n",recount[3]);
  printf(" EcalHit->EcalCluster OK           : % 6d\n",recount[4]);
  printf("\n\n");
  if(ECREFFKEY.relogic[1]==1 || ECREFFKEY.relogic[1]==2){
    printf("    ============== RLGA/FIAT part of REUN_Clibration-statistics ===============\n");
    printf("\n");
    printf(" REUN: entries(tof/ec_trigflag OK) : % 6d\n",cacount[0]);
    printf(" REUN: Track found                 : % 6d\n",cacount[1]);
    printf(" REUN: Track charge OK             : % 6d\n",cacount[2]);
    printf(" REUN: Track quality OK            : % 6d\n",cacount[3]);
    printf(" REUN: Track hits EC               : % 6d\n",cacount[4]);
    printf(" REUN: Recognized as PunchTrough   : % 6d\n",cacount[5]);
    printf(" REUN: Etruncated in limits        : % 6d\n",cacount[6]);
    printf(" REUN: At least one PM matched     : % 6d\n",cacount[7]);
    printf("\n\n");
  }
  if(ECREFFKEY.relogic[1]==3){
    printf("    ============== ANOR part of REUN_Clibration-statistics ===============\n");
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
//
}
//------------------------------------------
void EcalJobStat::bookhist(){
  int i,j,k,ich,maxcl,maxpl,maxsl;
  char htit1[60];
  char inum[11];
  char in[2]="0";
//
  maxpl=2*ECSLMX;//MAX planes
  maxcl=2*ECPMSMX;//MAX SubCells per plane
  maxsl=ECSLMX;
  strcpy(inum,"0123456789");
    if(ECREFFKEY.reprtf[0]!=0){ // Book reco-hist
      HBOOK1(ECHISTR+10,"ECRE: RawEvent-hits number",80,0.,400.,0.);
      HBOOK1(ECHISTR+11,"ECRE: RawEvent-hits value(tot,adc,gain-corr)",200,0.,100000.,0.);
      HBOOK1(ECHISTR+12,"ECRE: RawEvent-hits value(tot,adc,gain-corr)",100,0.,500.,0.);
      HBOOK1(ECHISTR+13,"ECRE: EcalHit-hits number",80,0.,160.,0.);
      HBOOK1(ECHISTR+14,"ECRE: EcalHit-hits value(tot,Mev)",200,0.,100000,0.);
      HBOOK1(ECHISTR+15,"ECRE: EcalHit-hits value(tot,Mev)",100,0.,1000,0.);
      HBOOK1(ECHISTR+16,"ECRE: RawEvent-hits value(adc,gain-corr)",200,0.,10000.,0.);
      HBOOK1(ECHISTR+17,"ECRE: RawEvent-hits value(adc,gain-corr)",100,0.,100.,0.);
      HBOOK1(ECHISTR+18,"ECRE: EcalClust per event",60,0.,120.,0.);
      if(ECREFFKEY.reprtf[1]==1){//<--- to store t-profiles, z-prof
        HBOOK1(ECHISTR+19,"ECRE: T-prof in plane 8(X)",maxcl,1.,geant(maxcl+1),0.);
        HBOOK1(ECHISTR+20,"ECRE: T-prof in plane 9(Y)",maxcl,1.,geant(maxcl+1),0.);
        HBOOK1(ECHISTR+21,"ECRE: Z-profile",maxpl,1.,geant(maxpl+1),0.);
      }
      HBOOK1(ECHISTR+22,"ECRE: EcalClust value(tot,Mev)",200,0.,1000000,0.);
      HBOOK1(ECHISTR+23,"ECRE: EcalClust value(tot,Mev)",100,0.,50000,0.);
      HBOOK1(ECHISTR+24,"ECRE: SubCelLayer En-profile(ECHits)",maxpl,1.,geant(maxpl+1),0.);
      HBOOK1(ECHISTR+25,"ECRE: SuperLayer En-profile(ECHits)",maxsl,1.,geant(maxsl+1),0.);
      HBOOK1(ECHISTR+30,"ECRE: Trigger flag(validate)",30,0.,30.,0.);
//
      if(ECREFFKEY.relogic[1]==1 || ECREFFKEY.relogic[1]==2){// RLGA/FIAT part of REUN-calibration
        HBOOK1(ECHISTC,"ECCA: Track COS(theta) at EC front",100,-1.,1.,0.);
        HBOOK1(ECHISTC+1,"ECCA: Track Imp.point X, SL1",70,-70.,70.,0.);
        HBOOK1(ECHISTC+2,"ECCA: Track Imp.point Y, SL1",70,-70.,70.,0.);
        HBOOK1(ECHISTC+3,"ECCA: PM-Track Transv-dist,SL1",50,-5.,5.,0.);
        HBOOK1(ECHISTC+4,"ECCA: PM-Track Transv-dist,SL2",50,-5.,5.,0.);
        HBOOK1(ECHISTC+5,"ECCA: PM-Track Longit-dist,SL1,PM18 ",70,0.,70.,0.);
        HBOOK1(ECHISTC+6,"ECCA: Track-fit Chi2 ",80,0.,20.,0.);
//    hist # +7 is booked inside mfit !!!
        HBOOK1(ECHISTC+8,"ECCA: SubCell Efficiency",50,0.2,1.2,0.);
        HBOOK1(ECHISTC+9,"ECCA: SubCell RelativeGain",50,0.5,1.5,0.);
        HBOOK1(ECHISTC+10,"ECCA: RefPmResp. uniformity",ECCLBMX,1.,geant(ECCLBMX+1),0.);
        HBOOK1(ECHISTC+11,"ECCA: PM relat.gains",100,0.5,1.5,0.);
        HBOOK1(ECHISTC+12,"ECCA: Rigidity (gv)",100,0.,100.,0.);
        HBOOK1(ECHISTC+13,"ECCA: PM-RelGain L-profile",ECSLMX,1.,geant(ECSLMX+1),0.);
	HMINIM(ECHISTC+13,0.9);
	HMAXIM(ECHISTC+13,1.1);
        HBOOK1(ECHISTC+14,"ECCA: SubCell Efficiency L-profile",maxpl,1.,geant(maxpl+1),0.);
        HBOOK1(ECHISTC+15,"ECCA: PM Eff vs SL(full fib.length)",maxsl,1.,geant(maxsl+1),0.);
	if(ECCAFFKEY.truse==1){//He4
	  HMINIM(ECHISTC+14,0.8);
	  HMAXIM(ECHISTC+14,1.);
	  HMINIM(ECHISTC+15,0.85);
	  HMAXIM(ECHISTC+15,1.05);
	}
	else{//prot
	  HMINIM(ECHISTC+14,0.4);
	  HMAXIM(ECHISTC+14,0.8);
	  HMINIM(ECHISTC+15,0.5);
	  HMAXIM(ECHISTC+15,1.1);
	}
        HBOOK1(ECHISTC+16,"ECCA: Edep/SLayer(PunchThrough,mev)",100,0.,200.,0.);
        HBOOK1(ECHISTC+17,"ECCA: Bad(non PunchThrough) scLayers",maxpl+1,0.,geant(maxpl+1),0.);
        HBOOK1(ECHISTC+18,"ECCA: SLayerEdep prof(punch-through)",maxsl,1.,geant(maxsl+1),0.);
	HBOOK2(ECHISTC+19,"ECCA: RefPmSc Alow vs Ahigh",80,20.,260.,30,0.,30.,0.);
        HBOOK1(ECHISTC+20,"ECCA: Slop(h2lcalib,all chan)",80,8.,24.,0.);
        HBOOK1(ECHISTC+21,"ECCA: Offs(h2lcalib,all chan)",80,-40.,40.,0.);
        HBOOK1(ECHISTC+22,"ECCA: Chi2(h2lcalib,all chan)",80,0.,8.,0.);
        HBOOK1(ECHISTC+23,"ECCA: LowChBinRMS/Aver(h2lcalib,all chan)",80,0.,1.,0.);
        HBOOK1(ECHISTC+24,"ECCA: EcalHit Energy(in adc-units)",100,0.,100.,0.);
        HBOOK1(ECHISTC+25,"ECCA: Fired(above thr) SubCells/Layer",80,0.,80.,0.);
        HBOOK1(ECHISTC+26,"ECCA: SubCell eff(even SL) ",80,0.5,1.3,0.);
        HBOOK1(ECHISTC+27,"ECCA: SubCell eff( odd SL) ",80,0.5,1.3,0.);
        HBOOK1(ECHISTC+28,"ECCA: SL number(punch-through)",maxsl,1.,geant(maxsl+1),0.);
        HBOOK1(ECHISTC+29,"ECCA: PM spectrum(trk-matched,X-pr)",100,0.,100.,0.);
        HBOOK1(ECHISTC+30,"ECCA: PM spectrum(trk-matched,Y-pr)",100,0.,100.,0.);
        HBOOK1(ECHISTC+31,"ECCA: PM eff(even SL) ",80,0.5,1.3,0.);
        HBOOK1(ECHISTC+32,"ECCA: PM eff( odd SL) ",80,0.5,1.3,0.);
// test	HBOOK1(ECHISTC+33,"ECCA: TRK imppoint X-accur",60,-0.3,0.3,0.); 
// test	HBOOK1(ECHISTC+34,"ECCA: TRK imppoint Y-accur",60,-0.3,0.3,0.); 
        HBOOK1(ECHISTC+35,"ECCA: Track beta",96,-1.2,1.2,0.);
	HBOOK1(ECHISTC+36,"ECCA: Track Z (from tracker)",16,0.,16.,0.);
      }
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
      }
    }
//
}
//-----------------------------
void EcalJobStat::bookhistmc(){
    if(ECMCFFKEY.mcprtf!=0){ // Book mc-hist
      HBOOK1(ECHIST+1,"Geant-hits number",100,0.,5000.,0.);
      HBOOK1(ECHIST+2,"ECMC: GeantdE/dX-hits value(tot,MeV)",100,0.,500,0.);
      HBOOK1(ECHIST+3,"ECMC: GeantdE/dX-hits value(+att,tot,MeV)",100,0.,500.,0.);
      HBOOK1(ECHIST+4,"ECMC: GeantEmeas(prim.electron)(AnodeTot,MeV)",400,0.,20000.,0.);
      HBOOK1(ECHIST+5,"ECMC: Dyn.hit value(tempor 4xAnodE/a2dr,i.e.in mev",100,0.,100.,0.);
      HBOOK1(ECHIST+6,"ECMC: 4xA-hit/D-hit ratio",50,0.,50.,0.);
      HBOOK1(ECHIST+7,"ECMC: EmcHits SL-profile",ECSLMX,1.,geant(ECSLMX+1),0.);
      HBOOK1(ECHIST+8,"ECMC: EmcHits SL(PM-assigned)-profile",ECSLMX,1.,geant(ECSLMX+1),0.);
      HBOOK1(ECHIST+9,"ECMC: Etot(temp.trig.sum,mev)",100,0.,20000.,0.);
      HBOOK1(ECHIST+10,"ECMC: 1ST 3SL signal(mev)",80,0.,1600.,0.);
      HBOOK1(ECHIST+11,"ECMC: Epk/Ebase Ratio(F,LE)",80,0.,40.,0.);
      HBOOK1(ECHIST+12,"ECMC: Ebase",80,0.,800.,0.);
      HBOOK1(ECHIST+13,"ECMC: Epk/Efr Ratio(F+P2B,HE)",50,0.,10.,0.);
      HBOOK1(ECHIST+14,"ECMC: Pm-signals(sl1,7,2;L-cuts)",100,0.,300.,0.);
      HBOOK1(ECHIST+15,"ECMC: Transv.Width(proj1,L-cuts,LE)",80,0.,80.,0.);
      HBOOK1(ECHIST+16,"ECMC: Pm-signals(sl2,8,2;L-cuts)",100,0.,300.,0.);
      HBOOK1(ECHIST+17,"ECMC: Transv.Width(proj2,L-cuts,LE)",80,0.,80.,0.);
      HBOOK1(ECHIST+18,"ECMC: Etot(trig.sum,L-cuts,mev)",100,0.,20000.,0.);
      HBOOK1(ECHIST+19,"ECMC: ECTriggerFlag",30,0.,30.,0.);
      HBOOK1(ECHIST+20,"ECMC: ECTriggerFlag(when TOFTrFlag)",30,0.,30.,0.);
    }
}
//----------------------------
void EcalJobStat::outp(){
    if(ECREFFKEY.reprtf[0]!=0){ // print RECO-hists
      HPRINT(ECHISTR+10);
      HPRINT(ECHISTR+11);
      HPRINT(ECHISTR+12);
      HPRINT(ECHISTR+13);
      HPRINT(ECHISTR+14);
      HPRINT(ECHISTR+15);
      HPRINT(ECHISTR+16);
      HPRINT(ECHISTR+17);
      HPRINT(ECHISTR+18);
      HPRINT(ECHISTR+22);
      HPRINT(ECHISTR+23);
      if(recount[2]>0)for(int i=0;i<2*ECSLMX;i++)zprofa[i]/=geant(recount[2]);
      if(recount[2]>0)for(int i=0;i<ECSLMX;i++)zprofapm[i]/=geant(recount[2]);
      HPAK(ECHISTR+24,zprofa);
      HPAK(ECHISTR+25,zprofapm);
      HPRINT(ECHISTR+24);
      HPRINT(ECHISTR+25);
      HPRINT(ECHISTR+30);
    }
    if(ECREFFKEY.relogic[1]==1 || ECREFFKEY.relogic[1]==2){ // print RLGA/FIAT-hists
      HPRINT(ECHISTC);
      HPRINT(ECHISTC+6);
      HPRINT(ECHISTC+35);
      HPRINT(ECHISTC+36);
      HPRINT(ECHISTC+1);
      HPRINT(ECHISTC+2);
      HPRINT(ECHISTC+3);
      HPRINT(ECHISTC+4);
      HPRINT(ECHISTC+5);
      ECREUNcalib::mfit();
      HPRINT(ECHISTC+8);
      HPRINT(ECHISTC+9);
//      HPRINT(ECHISTC+10);//already printed inside mfit
      HPRINT(ECHISTC+11);
      HPRINT(ECHISTC+12);
      HPRINT(ECHISTC+13);
      HPRINT(ECHISTC+14);
      HPRINT(ECHISTC+15);
      HPRINT(ECHISTC+16);
      HPRINT(ECHISTC+17);
      for(int i=0;i<ECSLMX;i++)if(nprofac[i]>0)zprofac[i]/=nprofac[i];
      HPAK(ECHISTC+18,zprofac);
      HPRINT(ECHISTC+18);
      HPRINT(ECHISTC+28);
      HPRINT(ECHISTC+19);
      HPRINT(ECHISTC+20);
      HPRINT(ECHISTC+21);
      HPRINT(ECHISTC+22);
      HPRINT(ECHISTC+23);
      HPRINT(ECHISTC+24);
      HPRINT(ECHISTC+25);
      HPRINT(ECHISTC+26);
      HPRINT(ECHISTC+27);
      HPRINT(ECHISTC+29);
      HPRINT(ECHISTC+30);
      HPRINT(ECHISTC+31);
      HPRINT(ECHISTC+32);
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
      ECREUNcalib::mfite();
    }
}
//----------------------------
void EcalJobStat::outpmc(){
    if(ECMCFFKEY.mcprtf!=0){ // print MC-hists
      HPRINT(ECHIST+1);
      HPRINT(ECHIST+2);
      HPRINT(ECHIST+3);
      HPRINT(ECHIST+4);
      HPRINT(ECHIST+5);
      HPRINT(ECHIST+6);
      if(mccount[1]>0){
        for(int i=0;i<ECSLMX;i++){
          zprmc1[i]/=geant(mccount[1]);
          zprmc2[i]/=geant(mccount[1]);
	}
      }
      HPAK(ECHIST+7,zprmc1);
      HPAK(ECHIST+8,zprmc2);
      HPRINT(ECHIST+7);
      HPRINT(ECHIST+8);
      HPRINT(ECHIST+9);
      HPRINT(ECHIST+10);
      HPRINT(ECHIST+11);
      HPRINT(ECHIST+12);
      HPRINT(ECHIST+13);
      HPRINT(ECHIST+14);
      HPRINT(ECHIST+15);
      HPRINT(ECHIST+16);
      HPRINT(ECHIST+17);
      HPRINT(ECHIST+18);
      HPRINT(ECHIST+19);
      HPRINT(ECHIST+20);
    }
}
//==========================================================================
//  ECcalib class functions :
//
void ECcalib::build(){// <--- create ecpmcal-objects (called from reecalinitjob)
  int i,isl,ipm,isc,cnum;
  integer sid,sta[4],endflab;
  geant pmrg,scrg[4],h2lr[4],a2m,a2dr,lfs,lsl,ffr;
  integer slmx,pmmx;
  integer scmx=4;// max.SubCells(pixels) in PM
  slmx=ECSLMX;//max.S-layers
  pmmx=ECPMSMX;//max.PM's in S-layer
  cout<<endl<<"ECcalib::build: total PMs="<<ECPMSL<<endl;
//
  char fname[80];
  char name[80];
  char vers1[3]="mc";
  char vers2[3]="rl";
  char in[2]="0";
  char inum[11];
  int ctyp,ntypes,mcvern[10],rlvern[10];
  int mcvn,rlvn,dig;
//
  integer status[ECPMSL][4];
  geant pmrgn[ECPMSL],pmscgn[ECPMSL][4],sch2lr[ECPMSL][4],an2dyr[ECPMSL],adc2mev;
  geant lfast[ECPMSL],lslow[ECPMSL],fastf[ECPMSL];
//
  strcpy(inum,"0123456789");
//
// ---> read list of calibration-type-versions list (menu-file) :
//
  integer cfvn;
  cfvn=ECCAFFKEY.cfvers%1000;
  strcpy(name,"ecalcvlist");// basic name for file of cal-files list 
  dig=cfvn/100;
  in[0]=inum[dig]; 
  strcat(name,in);
  dig=(cfvn%100)/10;
  in[0]=inum[dig]; 
  strcat(name,in);
  dig=cfvn%10;
  in[0]=inum[dig]; 
  strcat(name,in);
  strcat(name,".dat");
//
  if(ECCAFFKEY.cafdir==0)strcpy(fname,AMSDATADIR.amsdatadir);
  if(ECCAFFKEY.cafdir==1)strcpy(fname,"");
  strcat(fname,name);
  cout<<"ECcalib::build: Open file  "<<fname<<'\n';
  ifstream vlfile(fname,ios::in); // open needed tdfmap-file for reading
  if(!vlfile){
    cerr <<"ECcalib_build:: missing verslist-file "<<fname<<endl;
    exit(1);
  }
  vlfile >> ntypes;// total number of calibr. file types in the list
  for(i=0;i<ntypes;i++){
    vlfile >> mcvern[i];// first number - for mc
    vlfile >> rlvern[i];// second number - for real
  }
  vlfile.close();
//
//------------------------------
//
//   --->  Read status/rel.gains calib. file :
//
  ctyp=1;//1st type of calibration (really may be single)
  strcpy(name,"ecalrlga");
  mcvn=mcvern[ctyp-1]%1000;
  rlvn=rlvern[ctyp-1]%1000;
  if(AMSJob::gethead()->isMCData()) //      for MC-event
  {
       cout <<" ECcalib_build: MC: RLGA-calib file have to be read"<<endl;
       dig=mcvn/100;
       in[0]=inum[dig];
       strcat(name,in);
       dig=(mcvn%100)/10;
       in[0]=inum[dig];
       strcat(name,in);
       dig=mcvn%10;
       in[0]=inum[dig];
       strcat(name,in);
       strcat(name,vers1);
  }
  else                              //      for Real events
  {
       cout <<" ECcalib_build: REAL: RLGA-calib file have to be read"<<endl;
       dig=rlvn/100;
       in[0]=inum[dig];
       strcat(name,in);
       dig=(rlvn%100)/10;
       in[0]=inum[dig];
       strcat(name,in);
       dig=rlvn%10;
       in[0]=inum[dig];
       strcat(name,in);
       strcat(name,vers2);
  }
//
  strcat(name,".dat");
  if(ECCAFFKEY.cafdir==0)strcpy(fname,AMSDATADIR.amsdatadir);
  if(ECCAFFKEY.cafdir==1)strcpy(fname,"");
  strcat(fname,name);
  cout<<"ECcalib::build: Open file : "<<fname<<'\n';
  ifstream rlgfile(fname,ios::in); // open  file for reading
  if(!rlgfile){
    cerr <<"ECcalib_build: missing status/rel.gains file "<<fname<<endl;
    exit(1);
  }
//
// ---> read PM-status:
//
  for(isl=0;isl<slmx;isl++){   
    for(isc=0;isc<4;isc++){   
      for(ipm=0;ipm<pmmx;ipm++){  
        cnum=isl*pmmx+ipm; // sequential PM numbering(0 - SL*PMperSL)(324)
        rlgfile >> status[cnum][isc];
      }
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
    cout<<"ECcalib::build: RLGA-calibration file is successfully read !"<<endl;
  }
  else{cout<<"ECcalib::build: ERROR(problems while reading RLGA-calib.file)"<<endl;
    exit(1);
  }
//==================================================================
//
//   --->  Read fiber-attenuation calib. file :
//
  ctyp=2;//2nd type of calibration 
  strcpy(name,"ecalfiat");
  mcvn=mcvern[ctyp-1]%1000;
  rlvn=rlvern[ctyp-1]%1000;
  if(AMSJob::gethead()->isMCData()) //      for MC-event
  {
       cout <<" ECcalib_build: MC: FIAT-calib file have to be read"<<endl;
       dig=mcvn/100;
       in[0]=inum[dig];
       strcat(name,in);
       dig=(mcvn%100)/10;
       in[0]=inum[dig];
       strcat(name,in);
       dig=mcvn%10;
       in[0]=inum[dig];
       strcat(name,in);
       strcat(name,vers1);
  }
  else                              //      for Real events
  {
       cout <<" ECcalib_build: REAL: FIAT-calib file have to be read"<<endl;
       dig=rlvn/100;
       in[0]=inum[dig];
       strcat(name,in);
       dig=(rlvn%100)/10;
       in[0]=inum[dig];
       strcat(name,in);
       dig=rlvn%10;
       in[0]=inum[dig];
       strcat(name,in);
       strcat(name,vers2);
  }
//
  strcat(name,".dat");
  if(ECCAFFKEY.cafdir==0)strcpy(fname,AMSDATADIR.amsdatadir);
  if(ECCAFFKEY.cafdir==1)strcpy(fname,"");
  strcat(fname,name);
  cout<<"ECcalib::build: Open file : "<<fname<<'\n';
  ifstream fatfile(fname,ios::in); // open  file for reading
  if(!fatfile){
    cerr <<"ECcalib_build: missing fiber_att.param. file "<<fname<<endl;
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
    cout<<"ECcalib::build: calibration file is successfully read !"<<endl;
  }
  else{cout<<"ECcalib::build: ERROR(problems while reading FIAT-calib.file)"<<endl;
    exit(1);
  }
//================================================================== 
//
//   --->  Read abs.norm. calib. file :
//
  ctyp=3;//3rd type of calibration 
  strcpy(name,"ecalanor");
  mcvn=mcvern[ctyp-1]%1000;
  rlvn=rlvern[ctyp-1]%1000;
  if(AMSJob::gethead()->isMCData()) //      for MC-event
  {
       cout <<" ECcalib_build: MC: ANOR-calib file have to be read"<<endl;
       dig=mcvn/100;
       in[0]=inum[dig];
       strcat(name,in);
       dig=(mcvn%100)/10;
       in[0]=inum[dig];
       strcat(name,in);
       dig=mcvn%10;
       in[0]=inum[dig];
       strcat(name,in);
       strcat(name,vers1);
  }
  else                              //      for Real events
  {
       cout <<" ECcalib_build: REAL: ANOR-calib file have to be read"<<endl;
       dig=rlvn/100;
       in[0]=inum[dig];
       strcat(name,in);
       dig=(rlvn%100)/10;
       in[0]=inum[dig];
       strcat(name,in);
       dig=rlvn%10;
       in[0]=inum[dig];
       strcat(name,in);
       strcat(name,vers2);
  }
//
  strcat(name,".dat");
  if(ECCAFFKEY.cafdir==0)strcpy(fname,AMSDATADIR.amsdatadir);
  if(ECCAFFKEY.cafdir==1)strcpy(fname,"");
  strcat(fname,name);
  cout<<"ECcalib::build: Open file : "<<fname<<'\n';
  ifstream anrfile(fname,ios::in); // open  file for reading
  if(!anrfile){
    cerr <<"ECcalib_build: missing abs.norm. file "<<fname<<endl;
    exit(1);
  }
//
// ---> read common(hope) adc2mev convertion factor(abs.norm):
//
  anrfile >> adc2mev;
//
// ---> read endfile-label:
//
  anrfile >> endflab;
//
  anrfile.close();
  if(endflab==12345){
    cout<<"ECcalib::build: calibration file is successfully read !"<<endl;
  }
  else{cout<<"ECcalib::build: ERROR(problems while reading ANOR-calib.file)"<<endl;
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
      for(isc=0;isc<4;isc++)sta[isc]=status[cnum][isc];
      pmrg=pmrgn[cnum];
      for(isc=0;isc<4;isc++)scrg[isc]=pmscgn[cnum][isc];
      for(isc=0;isc<4;isc++)h2lr[isc]=sch2lr[cnum][isc];
      a2dr=an2dyr[cnum];
      lfs=lfast[cnum];
      lsl=lslow[cnum];
      ffr=fastf[cnum];
      a2m=adc2mev;
      ecpmcal[isl][ipm]=ECcalib(sid,sta,pmrg,scrg,h2lr,a2dr,lfs,lsl,ffr,a2m);
    }
  }
}  
//
//==========================================================================
integer ECcalib::BadCell(integer plane, integer cell){
  integer sl,pm,sc,dbsta;
  sl=plane/2;
  pm=cell/2;
  if(plane%2==0)sc=cell%2;
  else sc=cell%2+2;
  dbsta=ecpmcal[sl][pm].getstat(sc);
  if(dbsta<0)return(1);
  else return(0);
}
//
//==========================================================================
//  ECALVarp class functions :
//
void ECALVarp::init(geant daqth[10], geant cuts[5]){

    int i;
    for(i=0;i<10;i++)_daqthr[i]=daqth[i];
    for(i=0;i<5;i++)_cuts[i]=cuts[i];
}
//==========================================================================
//
void ECPMPeds::build(){// create TOFBPeds-objects for each sc.bar
//
  int i,isl,ipm,isc,cnum;
  integer sid,endflab(0);
  char fname[80];
  char name[80];
  geant pedh[4],sigh[4],pedl[4],sigl[4];
  geant pmpedh[ECPMSL][4],pmsigh[ECPMSL][4],pmpedl[ECPMSL][4],pmsigl[ECPMSL][4];
  integer slmx,pmmx;
  integer scmx=4;// max.SubCells(pixels) in PM
  slmx=ECSLMX;//max.S-layers
  pmmx=ECPMSMX;//max.PM's in S-layer
//
//   --->  Read high/low pedestals file :
//
  strcpy(name,"ecalpeds");
  if(AMSJob::gethead()->isMCData())           // for MC-event
  {
    cout <<" ECPMPeds_build: default MC peds-file is used..."<<endl;
    strcat(name,"mc");
  }
  else                                       // for Real events
  {
    cout <<" ECPMPeds_build: default RealData peds-file is used..."<<endl;
    strcat(name,"rl");
  }
  strcat(name,".dat");
  if(ECCAFFKEY.cafdir==0)strcpy(fname,AMSDATADIR.amsdatadir);
  if(ECCAFFKEY.cafdir==1)strcpy(fname,"");
  strcat(fname,name);
  cout<<"Open file : "<<fname<<'\n';
  ifstream icfile(fname,ios::in); // open pedestals-file for reading
  if(!icfile){
    cerr <<"ECPMPeds_build: missing default pedestals-file "<<fname<<endl;
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
    cout<<"ECPMPeds::build: Peds-file is successfully read !"<<endl;
  }
  else{cout<<"ECPMPeds::build: exit on ERROR(problems while reading of peds-file)"<<endl;
    exit(1);
  }
//---------------------------------------------
//
  for(isl=0;isl<slmx;isl++){
    for(ipm=0;ipm<pmmx;ipm++){
      cnum=isl*pmmx+ipm; // sequential PM numbering(0 - SL*PMperSL)(324)
      sid=(ipm+1)+100*(isl+1);
      for(isc=0;isc<4;isc++)pedh[isc]=pmpedh[cnum][isc];
      for(isc=0;isc<4;isc++)sigh[isc]=pmsigh[cnum][isc];
      for(isc=0;isc<4;isc++)pedl[isc]=pmpedl[cnum][isc];
      for(isc=0;isc<4;isc++)sigl[isc]=pmsigl[cnum][isc];
      pmpeds[isl][ipm]=ECPMPeds(sid,pedh,sigh,pedl,sigl);
    }
  }
}
//
//==========================================================================
