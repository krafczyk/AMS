#include <ctcrec.h>
#include <point.h>
#include <event.h>
#include <amsgobj.h>
#include <commons.h>
#include <cern.h>
#include <mccluster.h>
#include <math.h>
#include <extC.h>
#include <tofdbc.h>
#include <ctcdbc.h>
#include <ntuple.h>
AMSCTCRawCluster::CTCMap * AMSCTCRawCluster::_pMap=0;
AMSCTCRawCluster::CTCMap * AMSCTCRawCluster::GetMap(integer i){return _pMap+i-1;}
AMSID AMSCTCRawCluster::crgid(integer imat){
    integer geom=CTCDBc::getgeom();
    if(geom < 2){  
     if(imat==0)return AMSID("AGEL",10000+1000*_layer+_row);
     else return AMSID("WLS ",20000+1000*_layer+_row);
    }
    else {
      //      imat == 0   // agl
      //      imat == 1   // pmt
      //      imat == 2   // ptf
      integer y=(_row+1)/2;
      integer x=(_column+1)/2;
      integer iy=(_row+1)%2;
      integer ix=(_column+1)%2;      
      
     integer id=1000000*_layer+1000*y+100*x+2*iy+ix+1;
      if(imat==2){
       if(_layer==1)return AMSID("PTFU",id+10);     
       else if(x<6) return AMSID("PTFL",id+10);     
       else return AMSID("PTFE",id+10);     
      }
      else if(imat==0){
       if(_layer==1)return AMSID("AGLU",id+20);     
       else if(x<6) return AMSID("AGLL",id+20);     
       else return AMSID("AGLE",id+20);     
      }
      else if(imat==1){
       if(_layer==1)return AMSID("PMTU",id+30);     
       else if(x<6) return AMSID("PMTL",id+30);     
       else return AMSID("PMTE",id+30);     
      }
      else{
        cerr <<"AMSCTCRawCluster::crgid-F-Invalid media par "<<imat<<endl;
        exit(1);
      }
    }
}
  void AMSCTCRawCluster::init(){
    integer geom=CTCDBc::getgeom();
     if(geom == 0){
      _pMap=new CTCMap[CTCDBc::getnagel()];
      assert(_pMap != NULL);
      // horizontal readout
      if(CTCDBc::getnwls() > CTCDBc::getnagel()){
        cerr << "AMSCTCRawCluster::init-F-Invalid nwls/nblk "<<CTCDBc::getnwls()<<" "<<
      CTCDBc::getnagel()<<endl;
      exit(1);
      }
    integer ia;
    for(ia=1;ia<CTCDBc::getnagel()+1;ia++){
     AMSCTCRawCluster d(0,ia,1,0);
     AMSgvolume *p= AMSJob::gethead()->getgeomvolume(d.crgid(0));
     assert(p != NULL);
     number left=p->loc2gl(AMSPoint(0,0,0))[0]-p->getpar(0);
     number right=p->loc2gl(AMSPoint(0,0,0))[0]+p->getpar(0);
      integer ib;
      for(ib=1;ib<CTCDBc::getnwls()+1;ib++){
       AMSCTCRawCluster d(0,ib,1,0);
       AMSgvolume *p= AMSJob::gethead()->getgeomvolume(d.crgid(1));
       assert(p !=NULL);
       number a=p->loc2gl(AMSPoint(0,0,0))[0]-p->getpar(0);
       number b=p->loc2gl(AMSPoint(0,0,0))[0]+p->getpar(0);
       if(left >=a && left <=b){
        (_pMap+ia-1)->l=ib;
        (_pMap+ia-1)->r=min(ib+1,CTCDBc::getnwls());
        if(right <=b){
         (_pMap+ia-1)->a=1.;
         (_pMap+ia-1)->b=0.;
        }
        else{
         (_pMap+ia-1)->a=(b-left)/(right-left);
         (_pMap+ia-1)->b=1.-(_pMap+ia-1)->a;
        }
        break;
       }
      }
    }
    }
    else if (geom==1){    // Vertical one
     _pMap=new CTCMap[CTCDBc::getnagel()];
     assert(_pMap != NULL);
      integer ia;
      for(ia=1;ia<CTCDBc::getnagel()+1;ia++){
        (_pMap+ia-1)->l=(ia+1)/2;
        (_pMap+ia-1)->r=(ia+1)/2;
        (_pMap+ia-1)->a=1;
        (_pMap+ia-1)->b=0;
      }
    }
    else if (geom==2){  // Annecy fast


    }     
    else {
     cerr <<"AMSCTCRawCluster::init-F-Geom "<<geom<<" is not supported."<<endl;
     exit(1);
    }
  }

void AMSCTCRawCluster::sictcdigi(){
  AMSgObj::BookTimer.start("SICTCDIGI");
  if(CTCDBc::getgeom()<2){
  AMSCTCMCCluster * ptr;
  geant * TmpA= (geant*)UPool.insert(sizeof(geant)*CTCDBc::getnagel());
  geant * TmpB= (geant*)UPool.insert(sizeof(geant)*CTCDBc::getnwls());
  int icnt;
  for(icnt=0;icnt<CTCDBc::getnlay();icnt++){
   ptr=(AMSCTCMCCluster*)AMSEvent::gethead()->
   getheadC("AMSCTCMCCluster",icnt,1); // last 1  to test sorted container
   VZERO(TmpA,sizeof(geant)*CTCDBc::getnagel()/4);
   VZERO(TmpB,sizeof(geant)*CTCDBc::getnwls()/4);
   geant value=0;     
   while(ptr){
    integer det=ptr->getdetno()-1;
#ifdef __AMSDEBUG__
    assert (det >=0 && det<2);
#endif
    number z=1.-1./pow(CTCMCFFKEY.Refraction[det],2.)/pow(ptr->getbeta(),2.);
    integer signo;
    signo=ptr->getbarno();
    if(z>0){
      number Response;
      if(det==0){
        AMSCTCRawCluster d(0,ptr->getbarno(),ptr->getlayno(),0);
        AMSgvolume *p= AMSJob::gethead()->getgeomvolume(d.crgid(0));
        assert(p != NULL);
        AMSPoint p1=p->gl2loc(ptr->getcoo());
        AMSPoint p2=p->gl2loc(ptr->getcoo()-ptr->getdir()*ptr->getstep());
        if(CTCDBc::getgeom()==0){
         AMSPoint wls(0,0,-p->getpar(2));
         number z1=p1[2]-wls[2];
         number z2=p2[2]-wls[2];
         #ifdef __AMSDEBUG__
          if (z1<-3.e-4 || z1 > 3.e-4-2*wls[2])
           cerr <<"sictcdigi-E-z1 out of volume" <<z1<<" " <<p1<<endl;
          if (z2<-0.2 || z2 > 0.2-2*wls[2])
           cerr <<"sictcdigi-E-z2 out of volume" <<z2<<" " <<p2<<endl;
         
         #endif
          if(z1<0)z1=0;         
          if(z2<0)z2=0;         
          if(fabs(z1-z2)>2.e-4)
           Response=z*CTCMCFFKEY.AbsLength[det]*
            (exp(-z2/CTCMCFFKEY.AbsLength[det])-
            exp(-z1/CTCMCFFKEY.AbsLength[det]))/(z1-z2);

         else  Response=z*exp(-fabs(z1)/CTCMCFFKEY.AbsLength[det]);
        }
        else if(CTCDBc::getgeom()==1){ 
          number z1,z2;
          number wls=p->getpar(0);
          if(ptr->getbarno()%2 ==0){
           z1=p1[0]+wls;
           z2=p2[0]+wls;
          }
          else {
           z1=-p1[0]+wls;
           z2=-p2[0]+wls;
          }
          if (z1<-3.e-4 || z1 > 3.e-4+2*wls)
           cerr <<"sictcdigi-E-z1 out of volume" <<z1<<" " <<p1<<endl;
          if (z2<-0.2 || z2 > 0.2 + 2*wls)
           cerr <<"sictcdigi-E-z2 out of volume" <<z2<<" " <<p2<<endl;
          if(z1<0)z1=0;         
          if(z2<0)z2=0;         
         if(fabs(z1-z2)>2.e-4)
          Response=z*CTCMCFFKEY.AbsLength[det]*
           (exp(-z2/CTCMCFFKEY.AbsLength[det])-
           exp(-z1/CTCMCFFKEY.AbsLength[det]))/(z1-z2);

         else  Response=z*exp(-fabs(z1)/CTCMCFFKEY.AbsLength[det]);
        }
        else {    // Reconstruction for AG geometry
          // Uniform at the moment
          Response=z;
        }
      }
      else {
        Response=z;
      }
    value+=CTCMCFFKEY.Path2PhEl[det]*ptr->getstep()*Response*ptr->getcharge2();
    }
    // Add scint response
    value+=CTCMCFFKEY.Edep2Phel[det]*ptr->getedep();

    if( ptr->testlast()){
#ifdef __AMSDEBUG__
      if(det==0)assert( signo>0 && signo<CTCDBc::getnagel()+1);     
      else assert( signo>0 && signo<CTCDBc::getnwls()+1);     
#endif
      if(det ==0)TmpA[signo-1]=value;
      else       TmpB[signo-1]=value;
      value=0;
    }            

   ptr=ptr->next();  
   }
  integer ia,ib; 
  for(ia=1;ia<CTCDBc::getnagel()+1;ia++){
   TmpB[GetMap(ia)->l-1]=TmpB[GetMap(ia)->l-1]+TmpA[ia-1]*GetMap(ia)->a;
   TmpB[GetMap(ia)->r-1]=TmpB[GetMap(ia)->r-1]+TmpA[ia-1]*GetMap(ia)->b;
  }
    for(ib=1;ib<CTCDBc::getnwls()+1;ib++){
     integer ierr,ival;
     if(TmpB[ib-1]>0){
      POISSN(TmpB[ib-1],ival,ierr);
      AMSEvent::gethead()->addnext(AMSID("AMSCTCRawCluster",0),
      new AMSCTCRawCluster(0,ib,icnt+1,ival));
     }      
    }
  }
  UPool.udelete(TmpA); 
  UPool.udelete(TmpB); 
}
else {
  AMSCTCMCCluster * ptr;
  int icnt;
  for(icnt=0;icnt<CTCDBc::getnlay();icnt++){
  integer nrow=CTCDBc::getny()*2;
  integer ncol=CTCDBc::getnx(icnt+1)*2;
  geant * Tmp= (geant*)UPool.insert(sizeof(geant)*nrow*ncol);
   ptr=(AMSCTCMCCluster*)AMSEvent::gethead()->
   getheadC("AMSCTCMCCluster",icnt,1); // last 1  to test sorted container
   VZERO(Tmp,sizeof(geant)*nrow*ncol/4);
   geant value=0;     
   while(ptr){
    integer det=ptr->getdetno()-1;
#ifdef __AMSDEBUG__
    assert (det >=0 && det<2);
#endif
    number z=1.-1./pow(CTCMCFFKEY.Refraction[det],2.)/pow(ptr->getbeta(),2.);
    number Response;
    if(z>0)Response=z;
    else Response=0;
    value+=CTCMCFFKEY.Path2PhEl[det]*ptr->getstep()*Response*ptr->getcharge2();
    if( ptr->testlast()){
      integer ind=ptr->getcolno()-1+(ptr->getrowno()-1)*ncol;
#ifdef __AMSDEBUG__
   assert(ind >=0 && ind<nrow*ncol);
#endif
      Tmp[ind]+=value;
      value=0;
    }            

   ptr=ptr->next();  
   }
    for(int i=0;i<nrow*ncol;i++){
     integer ierr,ival;
     if(Tmp[i]>0){
      POISSN(Tmp[i],ival,ierr);
      AMSEvent::gethead()->addnext(AMSID("AMSCTCRawCluster",0),
      new AMSCTCRawCluster(0,i/ncol+1,icnt+1,ival,i%ncol+1));
     }      
    }
   UPool.udelete(Tmp); 
  }
}
  

  AMSgObj::BookTimer.stop("SICTCDIGI");
}

void AMSCTCRawCluster::_writeEl(){
}


void AMSCTCRawCluster::_copyEl(){
}


void AMSCTCRawCluster::_printEl(ostream & stream){
  stream <<"AMSCTCRawCluster  "<<_row<<" "<<" "<<_layer<<" "<<_signal<<endl;
}

void AMSCTCCluster::_writeEl(){

static integer init=0;
static CTCClusterNtuple CTCCLN;
int i;
if(AMSCTCCluster::Out( IOPA.WriteAll ||  getstatus(AMSDBc::USED ))){

if(init++==0){
//book the ntuple block
  HBNAME(IOPA.ntuple,"CTCClust",CTCCLN.getaddress(),
  "CTCCluster:I*4,CTCStatus:I*4, CTCLayer:I*4, CTCCoo(3):R*4,CTCErCoo(3):R*4,CTCRawSignal:R*4,CTCSignal:R*4,CTCESignal:R*4");

}
  CTCCLN.Event()=AMSEvent::gethead()->getid();
  CTCCLN.Status=_Status; 
  CTCCLN.Layer=_Layer;
  for(i=0;i<3;i++)CTCCLN.Coo[i]=_Coo[i];
  for(i=0;i<3;i++)CTCCLN.ErrCoo[i]=_ErrorCoo[i];
  CTCCLN.RawSignal=_Signal;
  CTCCLN.Signal=_CorrectedSignal;
  CTCCLN.ErrorSignal=_ErrorSignal;
  HFNTB(IOPA.ntuple,"CTCClust");

}
}
void AMSCTCCluster::_copyEl(){
}


void AMSCTCCluster::_printEl(ostream & stream){
  stream <<"AMSCTCCluster "<<_Status<<" "<<_Layer<<" "<<_Coo<<  " "<<_ErrorCoo<<" "<<
 _Signal<<" " <<_ErrorSignal<<endl;
}


 
void AMSCTCCluster::print(){
int i;
for(i=0;;i++){
 AMSContainer *p =AMSEvent::gethead()->getC("AMSCTCCluster",i);
 if(p)p->printC(cout);
 else break;
}
}



void AMSCTCCluster::build(){
if(CTCDBc::getgeom()<2){
 for(integer kk=0;kk<CTCDBc::getnlay();kk++){
  AMSCTCRawCluster *ptr=(AMSCTCRawCluster*)AMSEvent::gethead()->
  getheadC("AMSCTCRawCluster",0);
  integer const maxpl=200;
  static number xplane[maxpl];
  static integer xstatus[maxpl];
  VZERO(xplane,maxpl*sizeof(number)/4);
  VZERO(xplane,maxpl*sizeof(integer)/4);
  while (ptr){
    if(ptr->getlayno()==kk+1){
    integer plane=ptr->getbarno();
#ifdef __AMSDEBUG__
    assert(plane>0 && plane < maxpl-1);
#endif
    xplane[plane]+=ptr->getsignal();
    xstatus[plane]+=ptr->getstatus();
   }
   ptr=ptr->next();
  }

for (int i=0;i<maxpl;i++){ 
 if(xplane[i] > CTCRECFFKEY.Thr1 && xplane[i]>= xplane[i-1] 
 && xplane[i]>= xplane[i+1] ){
  number edep=0;
  AMSPoint cofg(0,0,0);
  number xsize=1000;
  number ysize=1000;
  number zsize=1000;
  integer status=xstatus[i];
  for(int j=i-1;j<i+2;j++){
   if(xplane[j]>0){
    AMSCTCRawCluster d(0,j,kk+1,0);
    AMSgvolume *p= AMSJob::gethead()->getgeomvolume(d.crgid(1));
    if(p){
     cofg=cofg+p->loc2gl(AMSPoint(0,0,0))*xplane[j];
     edep+=xplane[j];
    }
    else cerr << "AMSCTCCluster::build-S-GeomVolumeNotFound "<<j<<endl;
   }  
  }
  if(edep>0){
    cofg=cofg/edep;
  }
  // GetTypical Error Size - max (aerogel,wls)
    AMSCTCRawCluster d(0,1,1,0);
    AMSgvolume *p0= AMSJob::gethead()->getgeomvolume(d.crgid(0));
    AMSgvolume *p1= AMSJob::gethead()->getgeomvolume(d.crgid(1));
    if(p0 && p1 ){
          xsize=max(p0->getpar(0),p1->getpar(0));
          ysize=max(p0->getpar(1),p1->getpar(1));
          zsize=max(p0->getpar(2),p1->getpar(2));
    }
    else if (p0){
      // now WLS Probably  Annecy setup
          xsize=p0->getpar(0);
          ysize=p0->getpar(1);
          zsize=p0->getpar(2);
    }
  AMSPoint ecoo(xsize,ysize,zsize);
  if(edep>CTCRECFFKEY.ThrS){
    //    cout << cofg <<endl;
   AMSEvent::gethead()->addnext(AMSID("AMSCTCCluster",kk),
   new     AMSCTCCluster(status,kk+1,cofg,ecoo,edep,sqrt(edep)));
   xplane[i-1]=0;
   xplane[i]=0;
   xplane[i+1]=0; 
   i=0;
  }
 }    
}
}
}
else {
 for(integer kk=0;kk<CTCDBc::getnlay();kk++){
  AMSCTCRawCluster *ptr=(AMSCTCRawCluster*)AMSEvent::gethead()->
  getheadC("AMSCTCRawCluster",0);
  integer const maxpl=200;
  static number xplane[maxpl];
  static integer xstatus[maxpl];
  VZERO(xplane,maxpl*sizeof(number)/4);
  while (ptr){
   if(ptr->getlayno()==kk+1){
    integer plane=2*CTCDBc::getnx(kk+1)*(ptr->getrowno()-1)+ptr->getcolno()-1;
#ifdef __AMSDEBUG__
    assert(plane>=0 && plane < maxpl);
#endif
    xplane[plane]+=ptr->getsignal();
    xstatus[plane]+=ptr->getstatus();
   }
   ptr=ptr->next();
  }
  addnew(xstatus,xplane,maxpl,kk+1);
 }
}
}

void AMSCTCCluster::addnew(integer xstatus[],number xplane[],integer maxpl,integer layer){
number smax=0;
integer imax=-1;
for (int i=0;i<maxpl;i++){ 
 if(xplane[i] > smax){
  smax=xplane[i];
  imax=i;
 }
}
if(imax >=0 && smax >0 && smax>=CTCRECFFKEY.Thr1){
  number edep=0;
  AMSPoint cofg(0,0,0);
  number xsize=1000;
  number ysize=1000;
  number zsize=1000;
  integer status=xstatus[imax];
  integer row=imax/CTCDBc::getnx(layer)/2+1;
  integer col=imax%(CTCDBc::getnx(layer)*2)+1;
  int i,j;
  for(i=-1;i<2;i++){ 
   for(j=-1;j<2;j++){
    integer k=col-1+j+(row-1+i)*CTCDBc::getnx(layer)*2;
    if(k>=0 && k<maxpl && xplane[k]>0){
     AMSCTCRawCluster d(0,row+i,layer,0,col+j);
     AMSgvolume *p= AMSJob::gethead()->getgeomvolume(d.crgid(0));
     if(p){
      cofg=cofg+p->loc2gl(AMSPoint(0,0,0))*xplane[k];
      edep+=xplane[k];
     }
     else cerr << "AMSCTCCluster::build-S-GeomVolumeNotFound "<<row-1+i<<" "<<col-1+j<<endl;
    }  
   }
  }
  if(edep>0)cofg=cofg/edep;
  // GetTypical Error Size - 
  AMSCTCRawCluster d(0,1,1,0,1);
  AMSgvolume *p0= AMSJob::gethead()->getgeomvolume(d.crgid(0));
  if(p0 ){
          xsize=p0->getpar(0);
          ysize=p0->getpar(1);
          zsize=p0->getpar(2)*2;
  }
  AMSPoint ecoo(xsize,ysize,zsize);
  if(edep>=TOFRECFFKEY.ThrS){
   AMSEvent::gethead()->addnext(AMSID("AMSCTCCluster",layer-1),
   new     AMSCTCCluster(status,layer,cofg,ecoo,edep,sqrt(edep)));
   for(i=-1;i<2;i++){ 
    for(j=-1;j<2;j++){
      integer k=col-1+j+(row-1+i)*CTCDBc::getnx(layer)*2;
      if(k>=0 && k<maxpl && xplane[k]>0)xplane[k]=0;
    }
   }
   addnew(xstatus,xplane,maxpl,layer);
  }
}
}


integer AMSCTCCluster::Out(integer status){
static integer init=0;
static integer WriteAll=0;
if(init == 0){
 init=1;
 integer ntrig=AMSJob::gethead()->gettriggerN();
 for(int n=0;n<ntrig;n++){
   if(strcmp("AMSCTCCluster",AMSJob::gethead()->gettriggerC(n))==0){
     WriteAll=1;
     break;
   }
 }
}
return (WriteAll || status);
}



