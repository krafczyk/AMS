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
#include <trrawcluster.h>
#include <job.h>
integer AMSCharge::_sec[2]={0,0};
integer AMSCharge::_min[2]={0,0};
integer AMSCharge::_hour[2]={0,0};
integer AMSCharge::_day[2]={0,0};
integer AMSCharge::_mon[2]={0,0};
integer AMSCharge::_year[2]={97,96};
geant AMSCharge::_lkhdTOF[ncharge][100];
geant AMSCharge::_lkhdTracker[ncharge][100];
geant AMSCharge::_lkhdStepTOF[ncharge];
geant AMSCharge::_lkhdStepTracker[ncharge];
integer AMSCharge::_chargeTracker[ncharge]={1,1,2,3,4,5,6,7,8,9};
integer AMSCharge::_chargeTOF[ncharge]={1,1,2,3,4,5,6,7,8,9};
char AMSCharge::_fnam[128]="lkhd_v214+.data";
integer AMSCharge::getvotedcharge(){
int i;
number mp=0;
integer charge=0;
for(i=0;i<ncharge;i++){
  if(mp<_ProbTOF[i]*_ProbTracker[i]){
    mp=_ProbTOF[i]*_ProbTracker[i];
    charge=_chargeTracker[i];
  }
}
// TOF cannot get charge for charge > 3 imply the tracker only
if(charge>3){
  charge=_ChargeTracker;
}
else if(charge==2 && _ProbTracker[2]*_ProbTOF[2]<0.025){
  // Probably charge ==1
  if(_TrMeanTracker+_TrMeanTOF*20<CHARGEFITFFKEY.OneChargeThr)charge=1;
}
return charge;
}

integer AMSCharge::build(integer refit){
    number EdepTOF[4];
    number EdepTracker[6];
// charge finding
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
       number _TrMeanTOF=0;
       number smax=0;
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
           _TrMeanTOF+=pcluster->getedep();
           if(smax<pcluster->getedep())smax=pcluster->getedep();
           EdepTOF[nhitTOF]=pcluster->getedep()*
           min(one,fabs(pbeta->getbeta()))*
           min(one,fabs(pbeta->getbeta()))*fabs(ScDir.prod(DTr));
           nhitTOF++;
         }
       }
       if(nhitTOF-1)_TrMeanTOF=(_TrMeanTOF-smax)/(nhitTOF-1);
       smax=0;
       number  _TrMeanTracker=0;
       for (i=0;i<6;i++){
        AMSTrRecHit *phit=ptrack->getphit(i);
        if(phit){
         if (phit->getpsen()) {
          AMSDir SenDir((phit->getpsen())->getnrmA(2,0),
          (phit->getpsen())->getnrmA(2,1),(phit->getpsen())->getnrmA(2,2) );
          AMSPoint SenPnt=phit->getHit();
           ptrack->interpolate(SenPnt, SenDir, P1, theta, phi, sleng);
           AMSDir DTr(sin(theta)*cos(phi), sin(theta)*sin(phi), cos(theta));
           int good=!phit->checkstatus(AMSTrRecHit::FalseX) && 
             !phit->checkstatus(AMSTrRecHit::FalseTOFX);
           geant sum=phit->getsum();
           _TrMeanTracker+=sum;
           if(smax<sum)smax=sum;
           EdepTracker[nhitTracker]=AMSTrRawCluster::ADC2KeV()*
             (good?sum:2*sum)*pow(min(one,pbeta->getbeta()),2)*
             fabs(SenDir.prod(DTr));
         nhitTracker++;
         } else {
           cout<<"AMSCharge::build -E- phit -> getpsen == NULL "
               <<" for hit wit pos "<<phit ->getpos()<<", ContPos "
               <<phit -> getContPos()<<endl;
         }
        } 
       }
       if(nhitTracker-1)_TrMeanTracker=(_TrMeanTracker-smax)/(nhitTracker-1);
      addnext(rid,pbeta,nhitTOF,nhitTracker, EdepTOF, EdepTracker,_TrMeanTracker, _TrMeanTOF);
      pbeta=pbeta->next();
     }
    }
    return 1;
}

void AMSCharge::addnext(number rid, AMSBeta *pbeta , integer nhitTOF, 
      
          
        integer nhitTracker, number EdepTOF[4], number EdepTracker[6], number trtr, number trtof){
          AMSCharge * pcharge=new AMSCharge(pbeta,trtr,trtof);
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
   setstatus(1);
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
  ChargeNtuple* CN = AMSJob::gethead()->getntuple()->Get_charge();

  if (CN->Ncharge>=MAXCHARGE) return;

// Fill the ntuple 
  CN->Status[CN->Ncharge]=_status;
  CN->BetaP[CN->Ncharge]=_pbeta->getpos();
  CN->ChargeTOF[CN->Ncharge]=_ChargeTOF;
  CN->ChargeTracker[CN->Ncharge]=_ChargeTracker;
  int i;
  for(i=0;i<10;i++)CN->ProbTOF[CN->Ncharge][i]=_ProbTOF[i];
  for(i=0;i<10;i++)CN->ProbTracker[CN->Ncharge][i]=_ProbTracker[i];
  CN->TrunTOF[CN->Ncharge]=_TrMeanTOF;
  CN->TrunTracker[CN->Ncharge]=_TrMeanTracker;
  CN->Ncharge++;
}

void AMSCharge::_copyEl(){
}


void AMSCharge::print(){
 AMSContainer *p =AMSEvent::gethead()->getC("AMSCharge",0);
 if(p)p->printC(cout);
}


void AMSCharge::init(){
  //#ifdef __ALPHA__
  //  // yet another dec cxx compiler bug
  //  AMSCommonsI cmni;
  //  cmni.init();
  //#endif
int i,j;
char fnam[256]="";
strcpy(fnam,AMSDATADIR.amsdatadir);
strcat(fnam,_fnam);
if(AMSJob::gethead()->isRealData())strcat(fnam,".1");
else strcat(fnam,".0");
  ifstream iftxt(fnam,ios::in);
  if(!iftxt){
     cerr <<"AMSCharge::init-F-Error open file "<<fnam<<endl;
     
      exit(1);

  }
  else cout <<"AMSCharge::init-I-Open file "<<fnam<<endl;

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
  cout << "AMSCharge::init()-I-Completed"<<endl;
}



AMSChargeI::AMSChargeI(){
  // if(_Count++==0)AMSCharge::init();
}
integer AMSChargeI::_Count=0;
