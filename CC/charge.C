// Author V. Choutko 5-june-1996
//
//
// Lat Edit : Mar 20, 1997. ak. AMSCharge::Build() check if psen == NULL
// 
#include <beta.h>
#include <commons.h>
#include <math.h>
#include <limits.h>
#include <amsgobj.h>
#include <extC.h>
#include <upool.h>
#include <charge.h>
#include <iostream.h>
#include <fstream.h>
#include <stdlib.h>
#include <amsstl.h>
#include <ntuple.h>
#include <cern.h>

integer AMSCharge::_sec[2];
integer AMSCharge::_min[2];
integer AMSCharge::_hour[2];
integer AMSCharge::_day[2];
integer AMSCharge::_mon[2];
integer AMSCharge::_year[2];
geant AMSCharge::_lkhdTOF[ncharge][100];
geant AMSCharge::_lkhdTracker[ncharge][100];
geant AMSCharge::_lkhdStepTOF[ncharge];
geant AMSCharge::_lkhdStepTracker[ncharge];
integer AMSCharge::_chargeTracker[ncharge]={1,1,2,3,4,5,6,7,8,9};
integer AMSCharge::_chargeTOF[ncharge]={1,1,2,3,4,5,6,7,8,9};
char AMSCharge::_fnam[128]="lkhd_v205+.data";
void AMSCharge::build(){
  // charge finding
  number EdepTOF[4];
  number EdepTracker[6];
    int patb;
    for (patb=0;patb<npatb;patb++){
     AMSBeta *pbeta=(AMSBeta*)AMSEvent::gethead()->
     getheadC("AMSBeta",patb);
     while(pbeta){
       integer nhitTOF=0;
       integer nhitTracker=0;
       AMSTrTrack *ptrack=pbeta->getptrack();
       number rid=ptrack->getrid();
       number theta;
       number phi;
       number sleng;
       AMSPoint P1;
       int i;
       number one=1;
       for ( i=0;i<4;i++){
         AMSTOFCluster * pcluster= pbeta->getpcluster(i);
         if(pcluster){
#if 1  
    // change to 0 when eugeni writes the proper code
         AMSDir ScDir(0,0,1);
         AMSPoint SCPnt=pcluster->getcoo(); 
#else  
    // Eugeni code
#endif 
           ptrack->interpolate(SCPnt, ScDir, P1, theta, phi, sleng);
           AMSDir DTr(sin(theta)*cos(phi), sin(theta)*sin(phi), cos(theta));
           EdepTOF[nhitTOF]=pcluster->getedep()*
           min(one,fabs(pbeta->getbeta()))*
           min(one,fabs(pbeta->getbeta()))*fabs(ScDir.prod(DTr));
           nhitTOF++;
         }
       }
       for (i=0;i<6;i++){
        AMSTrRecHit *phit=ptrack->getphit(i);
        if(phit){
         if (phit->getpsen()) {
          AMSDir SenDir((phit->getpsen())->getinrm(2,0),
          (phit->getpsen())->getinrm(2,1),(phit->getpsen())->getinrm(2,2) );
          AMSPoint SenPnt=phit->getHit();
           ptrack->interpolate(SenPnt, SenDir, P1, theta, phi, sleng);
           AMSDir DTr(sin(theta)*cos(phi), sin(theta)*sin(phi), cos(theta));
           EdepTracker[nhitTracker]=phit->getsum()*
           pow(min(one,pbeta->getbeta()),2)*fabs(SenDir.prod(DTr));
         nhitTracker++;
         } else {
           cout<<"AMSCharge::build -E- phit -> getpsen == NULL "
               <<" for hit wit pos "<<phit ->getpos()<<", ContPos "
               <<phit -> getContPos()<<endl;
         }
        }
       }   
      addnext(rid,pbeta,nhitTOF,nhitTracker, EdepTOF, EdepTracker);
      pbeta=pbeta->next();
     }
    }
}

void AMSCharge::addnext(number rid, AMSBeta *pbeta , integer nhitTOF, 
      
          
        integer nhitTracker, number EdepTOF[4], number EdepTracker[6]){
          AMSCharge * pcharge=new AMSCharge(pbeta);
          pcharge->Fit(fabs(rid), nhitTOF, nhitTracker, EdepTOF, EdepTracker );
          AMSEvent::gethead()->addnext(AMSID("AMSCharge",0),pcharge);
             
}

void AMSCharge::Fit(number rid, integer nhitTOF, integer nhitTracker,
                     number EdepTOF[4], number EdepTracker[6]){
  int i;
  for( i=0;i<ncharge;i++){
   _ProbTOF[i]=0;
   _ProbTracker[i]=0;
   int j;
   for ( j=0;j<nhitTOF;j++){
    integer ia=(integer)floor(EdepTOF[j]/_lkhdStepTOF[i]);
    if(ia<0)ia=0;
    if(ia>=100)ia=99;
    _ProbTOF[i]+=-log(_lkhdTOF[i][ia]);
   }
   for ( j=0;j<nhitTracker;j++){
    integer ia=(integer)floor(EdepTracker[j]/_lkhdStepTracker[i]);
    if(ia<0)ia=0;
    if(ia>=100)ia=99;
    _ProbTracker[i]+=-log(_lkhdTracker[i][ia]);
   }
    
  }
  number minTOF=FLT_MAX;
  number minTracker=FLT_MAX;
  integer iTOF=0;
  integer iTracker=0;
  for(i=0;i<ncharge;i++){
    if(_ProbTracker[i]<minTracker){
     iTracker=i;
     minTracker=_ProbTracker[i];
    }
    if(_ProbTOF[i]<minTOF){
     iTOF=i;
     minTOF=_ProbTOF[i];
    }
   _ProbTOF[i]=exp(-_ProbTOF[i]);
   _ProbTOF[i]=pow(_ProbTOF[i],1./nhitTOF);
   _ProbTracker[i]=exp(-_ProbTracker[i]);
   _ProbTracker[i]=pow(_ProbTracker[i],1./nhitTracker);
  }
  _ChargeTOF=_chargeTOF[iTOF];
  _ChargeTracker=_chargeTracker[iTracker];
  if(_refit(rid,EdepTOF,nhitTOF)){
   number beta=fabs(_pbeta->getbeta());    
   if(beta < 1 && beta !=0){
    number momentum=fabs(rid*(_ChargeTOF+_ChargeTracker)/2);
    number energy=momentum/beta;
    number mass=energy*energy-momentum*momentum;
    integer first=0;
    number btracker=beta;
    for ( i=nhitTOF-1;i>0;i--){
         AMSTOFCluster * pcluster= _pbeta->getpcluster(i);
         if(pcluster){
           energy=energy+pcluster->getedep()/1000;
           momentum=sqrt(energy*energy-mass);
           number b=momentum/energy;
           if(first==0){
             btracker=b;
             first=1;
           }
           EdepTOF[i-1]=EdepTOF[i-1]/beta/beta*b*b;                  
         }
    }
    for(i=0;i<nhitTracker;i++){
      EdepTracker[i]=EdepTracker[i]/beta/beta*btracker*btracker;
    }
    Fit(-rid,  nhitTOF,  nhitTracker, EdepTOF,  EdepTracker);
   }
  }
#ifdef __AMSDEBUG__
    if(getchargeTOF() != getchargeTracker()){
      cout <<" TOF & Tracker disagree . TOF says particle charge is "<<
      getchargeTOF()<<" Tracker - "<<getchargeTracker()<<endl;
    }        
#endif

}

integer AMSCharge::_refit(number rid,number yy[],integer nx){

  if(rid <=0)return 0;
  if(_ChargeTOF != _ChargeTracker)return 1;
  if(nx < 2)return 0;
  number xx[4]={0,0,0,0};
  number xy(0),x2(0),ya(0),xa(0);
  integer i;
  for(i=0;i<nx;i++){
   x2+=i*i;
   xy+=i*yy[i];
   xa+=i;
   ya+=yy[i];
  }
  xa=xa/nx;
  ya=ya/nx;
  x2=x2/nx;
  xy=xy/nx;
  number a,b,chi2(0),da,d;
  d=x2-xa*xa;
  a=(xy-xa*ya)/d;
  b=ya-a*xa;
  for(i=0;i<nx;i++)chi2+=(yy[i]-a*i-b)*(yy[i]-a*i-b);
  chi2=sqrt(chi2/nx);
  da=chi2*ya/d/a/nx;
  if(da==0 || a/da> CHARGEFITFFKEY.Thr)return 1;
  else return 0;
  
}

void AMSCharge::_writeEl(){
  // fill the ntuple 
static integer init=0;
static ChargeNtuple CN;
  int i;
if(init++==0){
  //book the ntuple block
  HBNAME(IOPA.ntuple,"Charge",CN.getaddress(),
  "ChargeEvent:I*4, ChargeBetaP:I*4, ChargeTOF:I*4, ChargeTracker:I*4, ProbTOF(7):R*4, ProbTracker(7):R*4");

}
  CN.Event()=AMSEvent::gethead()->getid();
  CN.BetaP=_pbeta->getpos();
  CN.ChargeTOF=_ChargeTOF;
  CN.ChargeTracker=_ChargeTracker;
  for(i=0;i<ncharge;i++)CN.ProbTOF[i]=_ProbTOF[i];
  for(i=0;i<ncharge;i++)CN.ProbTracker[i]=_ProbTracker[i];
  HFNTB(IOPA.ntuple,"Charge");
}

void AMSCharge::_copyEl(){
}


void AMSCharge::print(){
 AMSContainer *p =AMSEvent::gethead()->getC("AMSCharge",0);
 if(p)p->printC(cout);
}


void AMSCharge::init(){
#ifdef __ALPHA__
  // yet another dec cxx compiler bug
  AMSCommonsI cmni;
  cmni.init();
#endif
geant v[3],x[3];
GUFLD(x,v);
int i,j;
char fnam[256]="";
strcpy(fnam,AMSDATADIR.amsdatadir);
strcat(fnam,_fnam);
  ifstream iftxt(fnam,ios::in);
  if(!iftxt){
     cerr <<"AMSCharge::init-F-Error open file "<<fnam<<endl;
     
      exit(1);
  }
  for( i=0;i<ncharge;i++){
    iftxt >> _lkhdStepTOF[i];
       
  }
  for( i=0;i<ncharge;i++){
    iftxt >> _lkhdStepTracker[i];
  }
  for( i=0;i<3;i++){
    for( j=0;j<100;j++){
     iftxt >> _lkhdTOF[i][j];
    }
  }
    for( j=0;j<100;j++){
     iftxt >> _lkhdTOF[6][j];
    }
  for( i=0;i<3;i++){
    for(j=0;j<100;j++){
     iftxt >> _lkhdTracker[i][j];
    }
  }
    for(j=0;j<100;j++){
     iftxt >> _lkhdTracker[6][j];
    }
    if(iftxt.eof() ){
      cerr<< "AMSCharge::init-F-Unexpected EOF"<<endl;
      exit(1);
    }
    for(j=0;j<2;j++) {
     iftxt >> _sec[j];
    }

    if(iftxt.eof() ){
      cerr<< "AMSCharge::init-F-Unexpected EOF"<<endl;
      exit(1);
    }
    for(j=0;j<2;j++) {
     iftxt >> _min[j];
    }

    if(iftxt.eof() ){
      cerr<< "AMSCharge::init-F-Unexpected EOF"<<endl;
      exit(1);
    }
    for(j=0;j<2;j++) {
     iftxt >> _hour[j];
    }

    if(iftxt.eof() ){
      cerr<< "AMSCharge::init-F-Unexpected EOF"<<endl;
      exit(1);
    }
    for(j=0;j<2;j++) {
     iftxt >> _day[j];
    }

    if(iftxt.eof() ){
      cerr<< "AMSCharge::init-F-Unexpected EOF"<<endl;
      exit(1);
    }
    for(j=0;j<2;j++) {
     iftxt >> _mon[j];
    }

    if(iftxt.eof() ){
      cerr<< "AMSCharge::init-F-Unexpected EOF"<<endl;
      exit(1);
    }
    for(j=0;j<2;j++) {
     iftxt >> _year[j];
    }

    if(iftxt.eof() ){
      cerr<< "AMSCharge::init-F-Unexpected EOF"<<endl;
      exit(1);
    }

  iftxt.close();


  for(i=1;i<4;i++){
   UCOPY(_lkhdTOF[2],_lkhdTOF[2+i],100*sizeof(_lkhdTOF[0][0])/4);
   UCOPY(_lkhdTracker[2],_lkhdTracker[2+i],100*sizeof(_lkhdTracker[0][0])/4);
   UCOPY(_lkhdTOF[6],_lkhdTOF[6+i],100*sizeof(_lkhdTOF[0][0])/4);
   UCOPY(_lkhdTracker[6],_lkhdTracker[6+i],100*sizeof(_lkhdTracker[0][0])/4);
  }
#ifdef __AMSDEBUG__
  cout << "AMSCharge::init()-I-Completed"<<endl;
#endif
}



AMSChargeI::AMSChargeI(){
 if(_Count++==0)AMSCharge::init();
}
integer AMSChargeI::_Count=0;
