// Author E.Choumilov 14.07.99.
#include <typedefs.h>
#include <math.h>
#include <commons.h>
#include <job.h>
#include <ecaldbc.h>
#include <stdio.h>
#include <iostream.h>
#include <fstream.h>
//
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
   64.8,64.8,18., // i=1-3  x,y,z-dimentions of EC-radiator
   11.4,          //  =4    dx(dy) thickn.of (PMT+electronics)-volume
   0.,0.,       //  =5,6    center shift in x,y   
  -142.3,       //  =7      Radiator(!!!) front face Z-pozition
   5.,          //  =8      top(bot) honeycomb thickness
   0.,0.       //  =9-10   spare
};
//
geant ECALDBc::_fpitch[3]={
   0.135,      // i=1   fiber pitch in X(Y) projection
   0.18,0.2   // i=2,3 fiber pitch in Z (inside super-layer / between neigbour super-layers)
};
//
geant ECALDBc::_rdcell[10]={
   368.,32.5,0.15, // i=1,3  fiber att. length 1st(slow)/2nd(fast), % of 2nd;
   0.094,        // i=4    fiber diameter(0.1-2x0.003cm)
   0.9,          // i=5    size(dx=dz) of "1/4" of PMT-cathod (pixel)
   0.45,         // i=6    abs(x(z)-position) of "1/4" in PMT coord.syst.
   1.8,          // i=7    X(Y)-pitch of PMT's;
   0.008,        // i=8    fiber wall+glue thickness(0.003+0.005cm)
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
   480,479       // i=1,2 numb. of fibers per 1st/2nd fiber-layer in S-layer
};
//
integer ECALDBc::_fibcgr[ECFLSMX][ECFBCMX]={ // fiber groupping in PMcell
   1,1,1,1,1,1, 2,2,2,2,2,2,      // ( SubCell number) 1PM=2x2=4SubCells: 
   1,1,1,1,1,1, 2,2,2,2,2,2,      //  -----
   1,1,1,1,1,1, 2,2,2,2,2,2,      //  |1|2|
   1,1,1,1,1,1, 2,2,2,2,2,2,      //  ----- view along the fibers in +X(Y) direction
   1,1,1,1,1,1, 2,2,2,2,2,2,      //  |3|4|
//                                    -----
   3,3,3,3,3,3, 4,4,4,4,4,4,
   3,3,3,3,3,3, 4,4,4,4,4,4,
   3,3,3,3,3,3, 4,4,4,4,4,4,
   3,3,3,3,3,3, 4,4,4,4,4,4,
   3,3,3,3,3,3, 4,4,4,4,4,4
};
//
integer ECALDBc::_nfibpcl[ECFLSMX]={ // real fibers per layer in PMcell
   12,12,12,12,12,12,12,12,12,12
};
//
geant ECALDBc::_mev2mev=34.33; // MC: Geant dE/dX(MeV)->Emeas(MeV) conv.factor(at 8gev)
int ECALDBc::_scalef=2;// MC/Data scale factor in used in ADC->DAQ-value
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
  integer ECALDBc::fibcgr(integer i, integer j){
    #ifdef __AMSDEBUG__
      if(ECALDBc::debug){
        assert(i>0 && i <= ECFLSMX);
        assert(j>0 && j <= ECFBCMX);
      }
    #endif
    return _fibcgr[i-1][j-1];
  }
//
  integer ECALDBc::nfibpcl(integer i){
    #ifdef __AMSDEBUG__
      if(ECALDBc::debug){
        assert(i>0 && i <= ECFLSMX);
      }
    #endif
    return _nfibpcl[i-1];
  }
//
//---
// fiberID(SSLLFFF) to cellID(SSPPC) conversion
// ("digital" design - no fiber_edep division between neigb.pixels/pmts)
//
  void ECALDBc::fid2cidd(integer fid, integer cid[4], number w[4]){
    integer fidd,fff,ss,ll,nn,pp,rc,fnc;
    fidd=fid/1000;
    fff=fid%1000-1;
    ll=fidd%100-1;
    ss=fidd/100;
    nn=_nfibpcl[ll];
    pp=fff/nn; // readout PMcell 
    fnc=fff%nn;// fiber number inside the cell
    rc=_fibcgr[ll][fnc];// readout sub-cell(pixel) inside PMcell
    cid[0]=rc+10*(pp+1)+1000*ss;
    w[0]=1.;
  }
//---
  number ECALDBc::segarea(number r, number ds){//small_segment area fraction (wrt full disk)
//                                    r-radious, ds-horde_distance(from center)
    number sina,cs,sn,a;
    if(ds>=r)return(0.);
    cs=ds/r;
    sn=sqrt(1.-cs*cs);
    sina=2.*cs*sn;
    a=2.*asin(sn);//=0->pi when ds=r->0
    return ((a-sina)/2./AMSDBc::pi);
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
    cz=ztop-ll*piz;//      z-pos of fiber in ECAL r.s.
    tleft=-npm*pmsiz/2.;//     low-edge PM-bin transv.position in ECAL r.s.
    dist=ct-tleft;//           fiber-center dist from the 1st PM (left edge)
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
        if(bdis<-fr){// <-- completely in the left half of PM
          cell=0+tbc;
	  cid[0]=1000*ss+(pm+1)*10+(cell+1);
	  w[0]=1.;
        }
        else if(bdis>fr){// <-- completely in the right half of PM
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
//---
// <--- function to get info about SubCell("digital" design) :
//input: isl->S-layer(0-...);pmc->PMCell(0-...);sc->SubCell(0-3)
//output:pr->Proj(0-X,1-Y);pl->Plane(0-..);cell->0-...;ct/l/z-coord. in AMS.r.s
//
  void ECALDBc::getscinfod(integer isl, integer pmc, integer sc,
         integer &pr, integer &pl, integer &cell, number &ct, number &cl, number &cz){
//
  int i,ip,sl,fl,fb,fi,pm,ce,nf[4];
  number z,z11,t,t1,t2,tt[ECFLSMX];
  static int first=0;
  static number cool[2];
  static number coot[ECPMSMX][4];
  static number cooz[ECSLMX][4];
//
  int nsl=_slstruc[2];// numb.of super-layers
  int nfl=_slstruc[1];// numb.of fiber-layers per super-layerr
  int npm=_slstruc[3];// numb.of PM's per super-layer
  int nfb;
  geant pit=_fpitch[0];
  geant piz=_fpitch[1];
  geant pizz=_fpitch[2];
//
  #ifdef __AMSDEBUG__
    if(ECALDBc::debug){
      assert(isl>=0 && isl<nsl);
      assert(pmc>=0 && isl<npm);
      assert(sc>=0 && sc<4);
    }
  #endif
//
    z11=(nsl*(nfl-1)*piz+(nsl-1)*pizz)/2.;//zpos(ECAL r.s.) of 1st f-layer of 1st S-layer
    if(first==0){// <--- first call actions(prepare some static tables):
      first=1;
      z=z11+(_gendim[6]-_gendim[2]/2.);//zpos(AMS r.s.) of 1st f-layer of 1st S-layer
      for(sl=0;sl<nsl;sl++){// <--- S-layer loop for Z-calc.
        for(i=0;i<4;i++){
	  nf[i]=0;
	  cooz[sl][i]=0.;
	}
        for(fl=0;fl<nfl;fl++){// <--- F-layer loop
	  nfb=_nfibpcl[fl];
	  for(fb=0;fb<nfb;fb++){// <--- Fiber loop in F-layer of PMCell
	    fi=_fibcgr[fl][fb];
	    if(fi>0){
	      nf[fi-1]+=1;
	      cooz[sl][fi-1]+=z;
	    }
	  }
	  z-=piz;
	}
	z+=piz;//remove extra piz-shift
	z-=pizz;//add pizz-shift while going from one s-layer to the next one
	for(i=0;i<4;i++)cooz[sl][i]/=geant(nf[i]);// z-COG's (AMS r.s.)
      }
// <--- now calc. tranv. COG's for all SubCells of ONE S-layer:
//
      t1=-(_nfibpl[0]-1)*pit/2.;// odd-f-layer 1st fiber t(ransv.) position (ECAL r.s.)
      t2=-(_nfibpl[1]-1)*pit/2.;//even-f-layer ..... (imply nfpl[1]=nfpl[0]+-1 sceme !!!)
//
      for(fl=0;fl<nfl;fl++){// init.1st fiber pos. for all f-layers in 1st PMCell
        ip=fl%2;//even(1)/odd(0) f-layer
	tt[fl]=(1-ip)*t1+ip*t2;// 1st fiber t-pos. of f-layer fl
      }
//
      for(pm=0;pm<npm;pm++){// <--- PMCell loop for t-coo. calc.
        for(i=0;i<4;i++){
	  nf[i]=0;
	  coot[pm][i]=0.;
	}
        for(fl=0;fl<nfl;fl++){// <--- F-layer loop
          ip=fl%2;//even(1)/odd(0) f-layer
	  nfb=_nfibpcl[fl];
	  t=0.;
	  for(fb=0;fb<nfb;fb++){// <--- Fiber loop in F-layer of PMCell
	    fi=_fibcgr[fl][fb];
	    if(fi>0){
	      nf[fi-1]+=1;
	      coot[pm][fi-1]+=(tt[fl]+t);// transversal COG's
	      t+=pit;
	    }
	  }
	  tt[fl]+=t;
	}
	for(i=0;i<4;i++)coot[pm][i]/=geant(nf[i]);// t-COG's (ECAL r.s.)
      }
#ifdef __AMSDEBUG__
// print some control numbers:
      cout<<"ECALDBc::getscinfo-test:"<<endl;
      cout<<"     1st fiber t-coo for 1st/2nd f-layer(t1/t2)="<<t1<<" "<<t2<<endl;
      cout<<"    last fiber t-coo(tt[f-layer]):"<<endl;
      for(fl=0;fl<nfl;fl++){
        tt[fl]-=pit;// remove extra t-shift
        cout<<"                 "<<tt[fl]<<endl;
      }
      cout<<"1st SubCell in 1st plane coot(EC r.s.)/cooz="<<coot[0][0]<<" "<<cooz[0][0]<<endl;      
      cout<<"last SubCell in 1st plane coot(EC r.s.)/cooz="<<coot[npm-1][1]<<" "<<cooz[0][1]<<endl;      
      cout<<"1st SubCell in last plane coot(EC r.s.)/cooz="<<coot[0][2]<<" "<<cooz[nsl-1][2]<<endl;      
      cout<<"last SubCell in last plane coot(EC r.s.)/cooz="<<coot[npm-1][3]<<" "<<cooz[nsl-1][3]<<endl;      
#endif
//
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
  geant pmpit=_rdcell[6];//PM-pitch(transv)
  geant pxsiz=_rdcell[4];// SubCell(pixel) size
  geant piz=_fpitch[1];
  geant pizz=_fpitch[2];
//
  #ifdef __AMSDEBUG__
    if(ECALDBc::debug){
      assert(isl>=0 && isl<nsl);
      assert(pmc>=0 && isl<npm);
      assert(sc>=0 && sc<4);
    }
  #endif
//
    z11=(nsl*(nfl-1)*piz+(nsl-1)*pizz)/2.;//zpos(ECAL r.s.) of 1st f-layer of 1st S-layer
    if(first==0){// <--- first call actions(prepare some static tables):
      if(nfl%2 != 0){// imply even number (no fiber sharing between top/bot pixels)
        cerr<<"ECALDBc::getscinfoa: wrong number of fiber layers per super layer, nfl="<<nfl<<endl;
	exit(1);
      }
      first=1;
      z=z11+(_gendim[6]-_gendim[2]/2.);//zpos(AMS r.s.) of 1st f-layer of 1st S-layer
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
//==========================================================================
//
//   EcalJobStat static variables definition (memory reservation):
//
integer EcalJobStat::mccount[ECJSTA];
integer EcalJobStat::recount[ECJSTA];
geant EcalJobStat::zprofa[2*ECSLMX];// average Z-profile
//
//------------------------------------------
void EcalJobStat::clear(){
  int i,j;
  for(i=0;i<ECJSTA;i++)mccount[i]=0;
  for(i=0;i<ECJSTA;i++)recount[i]=0;
  for(i=0;i<2*ECSLMX;i++)zprofa[i]=0;
}
//------------------------------------------
// function to print Job-statistics at the end of JOB(RUN):
void EcalJobStat::printstat(){
//
  printf("\n");
  printf("    ====================== ECAL JOB-statistics ======================\n");
  printf("\n");
  printf(" MC: entries                       : % 6d\n",mccount[0]);
  printf(" MC: MCHits->RawEvent(Trigfl>0) OK : % 6d\n",mccount[1]);
  printf(" RECO-entries                      : % 6d\n",recount[0]);
  printf(" ECAL trigger(trigfl>0) OK         : % 6d\n",recount[1]);
  printf(" Validation OK                     : % 6d\n",recount[2]);
  printf(" RawEvent->EcalHit OK              : % 6d\n",recount[3]);
  printf(" EcalHit->EcalCluster OK           : % 6d\n",recount[4]);
  printf("\n\n");
//
}
//------------------------------------------
void EcalJobStat::bookhist(){
  int i,j,k,ich,maxcl,maxpl;
  char htit1[60];
  char inum[11];
  char in[2]="0";
//
  maxpl=2*ECSLMX;//MAX planes
  maxcl=2*ECPMSMX;//MAX SubCells per plane
  strcpy(inum,"0123456789");
    if(ECREFFKEY.reprtf[0]!=0){ // Book reco-hist
      HBOOK1(ECHISTR+10,"ECAL: RawEvent-hits number",80,0.,400.,0.);
      HBOOK1(ECHISTR+11,"ECAL: RawEvent-hits value(tot,adc,gain-corr)",200,0.,100000.,0.);
      HBOOK1(ECHISTR+12,"ECAL: RawEvent-hits value(tot,adc,gain-corr)",100,0.,500.,0.);
      HBOOK1(ECHISTR+13,"EcalHit-hits number",80,0.,160.,0.);
      HBOOK1(ECHISTR+14,"EcalHit-hits value(tot,Mev)",200,0.,100000,0.);
      HBOOK1(ECHISTR+15,"EcalHit-hits value(tot,Mev)",100,0.,1000,0.);
      HBOOK1(ECHISTR+16,"ECAL: RawEvent-hits value(adc,gain-corr)",200,0.,10000.,0.);
      HBOOK1(ECHISTR+17,"ECAL: RawEvent-hits value(adc,gain-corr)",100,0.,100.,0.);
      HBOOK1(ECHISTR+18,"EcalClusters per event",60,0.,120.,0.);
      if(ECREFFKEY.reprtf[1]==1){//<--- to store t-profiles, z-prof
        HBOOK1(ECHISTR+19,"T-prof in plane 8(X)",maxcl,1.,geant(maxcl+1),0.);
        HBOOK1(ECHISTR+20,"T-prof in plane 9(Y)",maxcl,1.,geant(maxcl+1),0.);
        HBOOK1(ECHISTR+21,"Z-profile",maxpl,1.,geant(maxpl+1),0.);
      }
      HBOOK1(ECHISTR+22,"EcalCluster value(tot,Mev)",200,0.,1000000,0.);
      HBOOK1(ECHISTR+23,"EcalCluster value(tot,Mev)",100,0.,50000,0.);
      HBOOK1(ECHISTR+24,"Z-profile(average)",maxpl,1.,geant(maxpl+1),0.);
      HBOOK1(ECHISTR+30,"ECAL: Trigger flag(validate)",10,0.,10.,0.);
    }
//
}
//-----------------------------
void EcalJobStat::bookhistmc(){
    if(ECMCFFKEY.mcprtf!=0){ // Book mc-hist
      HBOOK1(ECHIST+1,"Geant-hits number",100,0.,5000.,0.);
      HBOOK1(ECHIST+2,"ECAL-MC: GeantdE/dX-hits value(tot,MeV)",100,0.,500,0.);
      HBOOK1(ECHIST+3,"ECAL-MC: GeantdE/dX-hits value(+att,tot,MeV)",100,0.,500.,0.);
      HBOOK1(ECHIST+4,"ECAL-MC: GeantEmeas(prim.electron)(AnodeTot,MeV)",400,0.,20000.,0.);
      HBOOK1(ECHIST+5,"ECAL-MC: Dyn.hit value(tempor 4xAnodE/a2dr,i.e.in mev",100,0.,100.,0.);
      HBOOK1(ECHIST+6,"ECAL-MC: 4xA-hit/D-hit ratio",50,0.,50.,0.);
      HBOOK1(ECHIST+7,"ECAL-MC: 1ST 4X0 signal(mev)",100,0.,2000.,0.);
      HBOOK1(ECHIST+8,"ECAL-MC: Tail2Peak Ratio",50,0.,1.,0.);
      HBOOK1(ECHIST+9,"ECAL-MC: Trigger flag(mctrigger)",30,0.,30.,0.);
      
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
      HPAK(ECHISTR+24,zprofa);
      HPRINT(ECHISTR+24);
      HPRINT(ECHISTR+30);
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
      HPRINT(ECHIST+7);
      HPRINT(ECHIST+8);
      HPRINT(ECHIST+9);
    }
}
//==========================================================================
//  ECcalib class functions :
//
void ECcalib::build(){// <--- create ecpmcal-objects (called from reecalinitjob)
  int i,isl,ipm,isc,cnum;
  integer sid,sta[4],endflab;
  geant pmrg,scrg[4],h2lr[4],a2m,a2dr;
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
//   --->  Read status/energy-calibration file :
//
  ctyp=1;//1st type of calibration (really may be single)
  strcpy(name,"ecalencf");
  mcvn=mcvern[ctyp-1]%1000;
  rlvn=rlvern[ctyp-1]%1000;
  if(AMSJob::gethead()->isMCData()) //      for MC-event
  {
       cout <<" ECcalib_build: MC:energy-calibration is used"<<endl;
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
       cout <<" TOF2Brcal_build: REAL:energy-calibration is used"<<endl;
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
  ifstream encfile(fname,ios::in); // open  file for reading
  if(!encfile){
    cerr <<"ECcalib_build: missing status/energy-calibration file "<<fname<<endl;
    exit(1);
  }
//
// ---> read PM-status:
//
  for(isl=0;isl<slmx;isl++){   
    for(isc=0;isc<4;isc++){   
      for(ipm=0;ipm<pmmx;ipm++){  
        cnum=isl*pmmx+ipm; // sequential PM numbering(0 - SL*PMperSL)(324)
        encfile >> status[cnum][isc];
      }
    }
  } 
//
// ---> read PM(sum of 4 SubCells) relative(to some Ref.PM) gains
//
  for(isl=0;isl<slmx;isl++){   
    for(ipm=0;ipm<pmmx;ipm++){  
      cnum=isl*pmmx+ipm; // sequential PM numbering(0 - SL*PMperSL)(324)
      encfile >> pmrgn[cnum];
    }
  }
//
// ---> read PM-SubCell relative gains:
//
  for(isl=0;isl<slmx;isl++){   
    for(isc=0;isc<4;isc++){   
      for(ipm=0;ipm<pmmx;ipm++){  
        cnum=isl*pmmx+ipm; // sequential PM numbering(0 - SL*PMperSL)(324)
        encfile >> pmscgn[cnum][isc];
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
        encfile >> sch2lr[cnum][isc];
      }
    }
  }
//
// ---> read PM anode(sum of 4-SubCells)-to-Dynode ratious:
//
  for(isl=0;isl<slmx;isl++){   
    for(ipm=0;ipm<pmmx;ipm++){  
      cnum=isl*pmmx+ipm; // sequential PM numbering(0 - SL*PMperSL)(324)
      encfile >> an2dyr[cnum];
    }
  } 
//
// ---> read common(hope) adc2mev convertion factor:
//
  encfile >> adc2mev;
//
// ---> read endfile-label:
//
  encfile >> endflab;
//
  encfile.close();
  if(endflab==12345){
    cout<<"ECcalib::build: calibration file is successfully read !"<<endl;
  }
  else{cout<<"ECcalib::build: exit on ERROR(problems while reading of calib.file)"<<endl;
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
      a2m=adc2mev;
      ecpmcal[isl][ipm]=ECcalib(sid,sta,pmrg,scrg,h2lr,a2dr,a2m);
    }
  }
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
