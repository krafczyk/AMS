//  $Id: tralig.C,v 1.74 2012/02/03 14:49:16 choutko Exp $
#include "tralig.h"
#include <math.h>
#include "timeid.h"
#include "trrawcluster.h"
#include <float.h>
#include "mccluster.h"
#include "tkdbc.h"
#include "amsstl.h"
#include "event.h"
#include <stdlib.h>
#include <unistd.h>
#include <sys/stat.h>
#include <sys/file.h>
#include <signal.h>
#ifdef _PGTRACK_
#include "MagField.h"
#endif
using namespace trconst;
//PROTOCCALLSFSUB15(E04CCF,e04ccf,INT,DOUBLEV,DOUBLE,DOUBLE,INT,DOUBLEV,DOUBLEV,DOUBLEV,DOUBLEV,DOUBLEV,DOUBLEV,ROUTINE,ROUTINE,INT,INT)
//#define E04CCF(N,X,F,TOL,IW,W1,W2,W3,W4,W5,W6,ALFUN1,MONIT,MAXCAL,IFAIL) CCALLSFSUB15(E04CCF,e04ccf,INT,DOUBLEV,DOUBLE,DOUBLE,INT,DOUBLEV,DOUBLEV,DOUBLEV,DOUBLEV,DOUBLEV,DOUBLEV,ROUTINE,ROUTINE,INT,INT,N,X,F,TOL,IW,W1,W2,W3,W4,W5,W6,ALFUN1,MONIT,MAXCAL,IFAIL)

extern "C" void e04ccf_(int &n, number x[], number &f, number &tol, int &iw, number w1[],number w2[], number w3[], number w4[], number w5[], number w6[],void * alfun, void * monit, int & maxcal, int &ifail, void * p);

void AMSTrAligPar::_m2a(){

number l1=_Dir[0][0];
number m1=_Dir[0][1];
number n1=_Dir[0][2];
number l2=_Dir[1][0];
number m2=_Dir[1][1];
number n2=_Dir[1][2];
number l3=_Dir[2][0];
number m3=_Dir[2][1];
number n3=_Dir[2][2];
number roll,pitch,yaw;

if(sqrt(l1*l1+n1*n1)>FLT_MIN){

 roll=atan2(-m3,m2);
 pitch=atan2(n1,l1);
 yaw= fabs(cos(pitch))>FLT_MIN ? atan2(-m1,l1/cos(pitch)) :
                               atan2(-m1,n1/sin(pitch));
}
else{

cout <<" AMSTrAligPar::_m2a-W-MatrixSingular"<<endl;
yaw=atan2(-m1,0);
roll=0;
pitch=atan2(n2,l2); 
}
_Angles[0]=pitch;
_Angles[1]=yaw;
_Angles[2]=roll;

//Checkit

//#ifdef __AMSDEBUG__
_a2m();

if(fabs(l1-_Dir[0][0])>0.001)cout <<" AMSTrAligPar::_m2a-E-ConversionError 0 0 "<<l1<<" " <<_Dir[0][0]<<endl;
if(fabs(m1-_Dir[0][1])>0.001)cout <<" AMSTrAligPar::_m2a-E-ConversionError 0 1 "<<m1<<" " <<_Dir[0][1]<<endl;
if(fabs(n1-_Dir[0][2])>0.001)cout <<" AMSTrAligPar::_m2a-E-ConversionError 0 2 "<<n1<<" " <<_Dir[0][2]<<endl;
if(fabs(l2-_Dir[1][0])>0.001)cout <<" AMSTrAligPar::_m2a-E-ConversionError 1 0 "<<l2<<" " <<_Dir[1][0]<<endl;
if(fabs(m2-_Dir[1][1])>0.001)cout <<" AMSTrAligPar::_m2a-E-ConversionError 1 1 "<<m2<<" " <<_Dir[1][1]<<endl;
if(fabs(n2-_Dir[1][2])>0.001)cout <<" AMSTrAligPar::_m2a-E-ConversionError 1 2 "<<n2<<" " <<_Dir[1][2]<<endl;
if(fabs(l3-_Dir[2][0])>0.001)cout <<" AMSTrAligPar::_m2a-E-ConversionError 2 0 "<<l3<<" " <<_Dir[2][0]<<endl;
if(fabs(m3-_Dir[2][1])>0.001)cout <<" AMSTrAligPar::_m2a-E-ConversionError 2 1 "<<m3<<" " <<_Dir[2][1]<<endl;
if(fabs(n3-_Dir[2][2])>0.001)cout <<" AMSTrAligPar::_m2a-E-ConversionError 2 2 "<<n3<<" " <<_Dir[2][2]<<endl;
//#endif

}

void AMSTrAligPar::_a2m(){
AMSPoint angle=_Angles;
number pitch=angle[0];
number yaw=angle[1];
number roll=angle[2];
  number cp=cos(pitch);
  number sp=sin(pitch);
  number cy=cos(yaw);
  number sy=sin(yaw);
  number cr=cos(roll);
  number sr=sin(roll);
  number l1=cy*cp;
  number m1=-sy;
  number n1=cy*sp;
  number l2=cr*sy*cp-sr*sp;
  number m2=cr*cy;
  number n2=cr*sy*sp+sr*cp;
  number l3=-sr*sy*cp-cr*sp;
  number m3=-sr*cy;
  number n3=-sr*sy*sp+cr*cp;
  _Dir[0]=AMSDir(l1,m1,n1);
  _Dir[1]=AMSDir(l2,m2,n2);
  _Dir[2]=AMSDir(l3,m3,n3);
}

void AMSTrAligPar::setpar(const AMSPoint & coo, const AMSPoint & angles){
_Coo=coo;
_Angles=angles;
// construct rot matrix
_a2m();
}


AMSTrAligPar::AMSTrAligPar(const AMSPoint & coo, const AMSPoint & angles){
_NEntries=0;
_Coo=coo;
_Angles=angles;
// construct rot matrix
_a2m();
}



AMSTrAligPar::AMSTrAligPar(const AMSTrAligDBEntry * db, integer i){
_NEntries=0;
if(i>=0 && i<TKDBc::nlay()){
  _Coo=AMSPoint (db->coo[i][0],db->coo[i][1],db->coo[i][2]);
  _Angles=AMSPoint (db->angle[i][0],db->angle[i][1],db->angle[i][2]);
  _a2m();
}
else cerr <<"AMSTrAligPar::AMSTrAligPar-S-Wrong plane "<<i<<endl;
}




void AMSTrAligPar::UpdateDB(uintl address,  AMSTrAligPar  o[], number fcni, number fcn, number pav, number pav2){
   
if(AMSFFKEY.Update){
    if(DbIsNotLocked())LockDB();
    // read latest db into memory
       AMSTimeID *ptdv=AMSJob::gethead()->gettimestructure(getTDVDB());
       time_t begin,end,insert;
       ptdv->gettime(insert,begin,end);
       ptdv->SetTime(insert,end,begin);
       time_t asktime;
       if(AMSEvent::gethead())asktime=AMSEvent::gethead()->gettime();
       else asktime=0;
       if(!ptdv->readDB(AMSDATADIR.amsdatabase,asktime)){
        cerr<<"AMSTrAligFit-F-ProblemToReadDB "<<*ptdv;
        UnlockDB();
        exit(1);
       }
}
 //  if (getdbentries()>= maxdbentries()){
   if (0){
     cerr<<"AMSTrAligPar::UpdateDB-E-DBIsFull "<<endl;
     return ;
   }
   else{
     // ItSelf
{
    AMSTrAligPar::AMSTrAligDBEntry e(address,1,o,fcni,fcn,pav,pav2);
    int out=AMSbins(getdbtopp(),e, getdbentries());
    int lexplicit=0;
    if(out>0)lexplicit=(getdbtopp()+out-1)->status;
    if(out>0 && TRALIG.ReWriteDB==0 && lexplicit==1  ){
     cerr<<"AMSTrAligPar::UpdateDB-E-ObjectAlreadyExists "<<address<<" "<<getdbentries()<<endl;
    }
    else if(out<=0){
     if(getdbentries()< maxdbentries()){
      *(getdbtopp()+getdbentries())=e;
      incdbentries();
      AMSsortNAGa(getdbtopp(),getdbentries());
      cout<<"AMSTrAligPar::UpdateDB-I-NewEntryAdded "<<address<<" "<<getdbentries()<<endl;
    }
    else{
     cerr<<"AMSTrAligPar::UpdateDB-E-DBIsFull "<<endl;
    }     
    }
    else{
     *(getdbtopp()+out-1)=e;
     cout<<"AMSTrAligPar::UpdateDB-I-EntryReplaced"<<address<<" "<<out-1<<endl;
    }
}
    // childs

    uinteger nchild;
    uintl * pimplicit=AMSTrTrack::getchild(address,nchild);
    for(int i=0;i<nchild;i++){
     uintl addr=pimplicit[i];
     AMSTrAligPar::AMSTrAligDBEntry ec(addr,0,o,fcni,fcn,pav,pav2);
    int out=AMSbins(getdbtopp(),ec, getdbentries());
    int lexplicit=0;
    if(out>0)lexplicit=(getdbtopp()+out-1)->status;
    if(out>0 && (TRALIG.ReWriteDB==0 || lexplicit==1)){
     cerr<<"AMSTrAligPar::UpdateDB-E-ImpObjectAlreadyExists "<<pimplicit[i]<<" "<<getdbentries()<<endl;
    }
    else if(out<=0){
     if(getdbentries()< maxdbentries()){
      *(getdbtopp()+getdbentries())=ec;
      incdbentries();
      AMSsortNAGa(getdbtopp(),getdbentries());
      cout<<"AMSTrAligPar::UpdateDB-I-NewImpEntryAdded "<<pimplicit[i]<<" "<<getdbentries()<<endl;
     }
     else {
      cerr<<"AMSTrAligPar::UpdateDB-E-DBIsFull "<<endl;
     }     
    }
    else{
     *(getdbtopp()+out-1)=ec;
     cout<<"AMSTrAligPar::UpdateDB-I-EntryReplaced"<<pimplicit[i]<<" "<<out-1<<endl;
    }

    }


   }
  // UpdateDB

    if (AMSFFKEY.Update ){
       AMSTimeID *ptdv=AMSJob::gethead()->gettimestructure(getTDVDB());
             ptdv->UpdCRC();
             if( !ptdv->write(AMSDATADIR.amsdatabase)){
                cerr <<"AMSTrAligPar::updateDB-F-ProblemtoUpdate "<<*ptdv;
                UnlockDB();
                exit(1);
             }              
     UnlockDB();
    }
   return ;
}


// Here AMSTrAligFit

AMSTrAligFit::AMSTrAligFit():_Address(0,0),_Pattern(0),_NData(0),_PositionData(0),
_pData(0),_PlaneNo(0),_HalfNo(0),_LadderNo(0),_ParNo(0),AMSNode(AMSID("AMSTrAligFit",0)){
  int i;
  for(i=0;i<TKDBc::nlay();i++){
   _pParC[i]=AMSTrAligPar();
  }
  for(int k=0;k<sizeof(chi2)/sizeof(chi2[0]);k++){
   chi2[k]=chi2i[k]=0;
  }
}

AMSTrAligFit::AMSTrAligFit(uint128 address, integer pattern, integer ndatmax,
          integer alg, integer nodeno):_Address(address),_Pattern(pattern),
          _NData(ndatmax),_Algorithm(alg),_PositionData(0),AMSNode(AMSID("AMSTrAligFit",nodeno)){
          _pData=new AMSTrAligData[ndatmax];  
            assert(_pData !=  NULL);             
if(_Address==1){

// global Fit

  int i;
  for(i=0;i<trconst::maxlad;i++){
   for(int j=0;j<2;j++){
    for(int k=0;k<TKDBc::nlay();k++){   
     for(int m=0;m<trconst::maxsen;m++){
       _pPargl[m][i][j][k].NEntries()=0;
       _pPargl[m][i][j][k].setpar(AMSPoint(),AMSPoint());
       if(_pPargl[m][i][j][k].NEntries())cout <<i<<" "<<j<<" "<<k<<" "<<_pPargl[m][i][j][k].NEntries()<<endl;
    }
   }
  }
  }


_NoActivePar=0;
   for( i=0;i<TKDBc::nlay();i++){
      for(int l=0;l<trconst::maxlad;l++){
       for(int m=0;m<2;m++){
         if(TKDBc::activeladdshuttle(i+1,l+1,m)){
          int nprp=0;
          for(int j=0;j<6;j++){
           if(TRALIG.ActiveParameters[i][j]){
            nprp++;
           }
          }
          _NoActivePar+=nprp;
         }
       }
      
}
       cout <<"  lay "<<i<<" " <<_NoActivePar<<endl;
    }
if(!TRALIG.LaddersOnly && !TRALIG.LayersOnly)_NoActivePar*=trconst::maxsen;
cout <<"GplobalFit-I-NoActivePar "<<_NoActivePar<<" "<<endl;

_PlaneNo= new integer[_NoActivePar];
_ParNo= new integer[_NoActivePar];
_LadderNo=new integer[_NoActivePar];
_HalfNo=new integer[_NoActivePar];
_NoActivePar=0;

    for( i=0;i<TKDBc::nlay();i++){
      for(int l=0;l<trconst::maxlad;l++){
       for(int m=0;m<2;m++){
         bool bok=m==0?TKDBc::nhalf(i+1,l+1)>0 :TKDBc::nhalf(i+1,l+1)<TKDBc::nsen(i+1,l+1);



         if(TKDBc::activeladdshuttle(i+1,l+1,m) && bok ){
          int nprp=0;
          for(int j=0;j<6;j++){
           if(TRALIG.ActiveParameters[i][j]){
            _PlaneNo[_NoActivePar+nprp]=i;
            _ParNo[_NoActivePar+nprp]=j;
            _LadderNo[_NoActivePar+nprp]=l;
            _HalfNo[_NoActivePar+nprp]=m;
            nprp++;
           }
          }
          _NoActivePar+=nprp;
         }
       }
      }
    }
cout <<"GlobalFit-I-NoActivePar "<<_NoActivePar<<" "<<endl;

}
else{
  int i;
  for(i=0;i<TKDBc::nlay();i++){
   _pParC[i]=AMSTrAligPar();
  }
_NoActivePar=0;

for(i=0;i<TKDBc::nlay();i++){
 for(int k=1;k<TKDBc::nlay();k++){
 if(i==TKDBc::patconf(pattern,k)-1){
  int nprp=0;
  for(int j=0;j<TKDBc::nlay();j++){
    if(TRALIG.ActiveParameters[i][j]){
//       _PlaneNo[_NoActivePar+nprp]=i;
//       _ParNo[_NoActivePar+nprp]=j;
       nprp++;
    }
  }
 _NoActivePar+=nprp;
 break;
 }
}
}
_PlaneNo= new integer[_NoActivePar];
_ParNo= new integer[_NoActivePar];
_LadderNo=0;
_HalfNo=0;
_NoActivePar=0;
for(i=0;i<TKDBc::nlay();i++){
 for(int k=1;k<TKDBc::nlay();k++){
 if(i==TKDBc::patconf(pattern,k)-1){
  int nprp=0;
  for(int j=0;j<TKDBc::nlay();j++){
    if(TRALIG.ActiveParameters[i][j]){
       _PlaneNo[_NoActivePar+nprp]=i;
       _ParNo[_NoActivePar+nprp]=j;
       nprp++;
    }
  }
 _NoActivePar+=nprp;
 break;
 }
}
}
//cout << " pattern " <<pattern <<" "<<_NoActivePar<<endl;



}


}


AMSTrAligFit::~AMSTrAligFit(){
 delete [] _pData;
 delete [] _PlaneNo;
 delete [] _LadderNo;
 delete [] _HalfNo;
 delete [] _ParNo;
}

/*
void AMSTrAligFit::Test(integer forced){

AMSNode *pnode =AMSJob::gethead()->getaligstructure();
AMSTrAligFit * pal = (AMSTrAligFit *)pnode->down();

if(forced==0){
 AMSParticle * ppart=0;
 AMSmceventg * ptrg=0;
  if(Select(ppart,ptrg,TRALIG.Algorithm)){
     AMSTrTrack * ptrack= ppart->getptrack();
     integer nnodes=0;
     integer found=0;
      while(pal){
        nnodes++;
        if(pal->AddressOK(ptrack->getaddress(),int(TRALIG.Cuts[9][0]))){
           if(pal->_PositionData<pal->_NData)(pal->_pData[(pal->_PositionData)++]).Init(ptrack,ptrg);
           if(pal->AddressOK(ptrack->getaddress(),1))found=1;
           else if(found==0)found=-1;
        }
        pal=pal->next(); 
     }
     if(found==1)return; 
      if(nnodes<TRALIG.MaxPatternsPerJob){
           integer found=0;
           number db[2];
           if(TRALIG.ReWriteDB ){
            pal=new AMSTrAligFit(ptrack->getaddress(),
            ptrack->getpattern(), TRALIG.MaxEventsPerFit, TRALIG.Algorithm, nnodes+1);
            if(pal->_PositionData<pal->_NData)(pal->_pData[(pal->_PositionData)++]).Init(ptrack,ptrg);
            pnode->add(pal);       
            cout <<"AMSTrAligFit::Test-I-NewPatternAdded " <<ptrack->getaddress()<<" "<<TRALIG.Algorithm<<endl;
           }
      //else cerr<<"AMSTrAligFit::Test-E-CouldNotAddPatternAlrdyExists " <<ptrack->getaddress()<<endl;
      }
      //else cerr<<"AMSTrAligFit::Test-E-CouldNotAddPatternMaxReached " <<ptrack->getaddress()<<endl;
  }
}


else {


   // Open Ntuple Fit/fill Ntuple & update Db

      while(pal){
         pal->Fit();
         pal=pal->next(); 
     }
  // close ntuple

  char hpawc[256]="//PAWC";
  HCDIR (hpawc, " ");
  char houtput[]="//tralig";
  HCDIR (houtput, " ");
  integer ICYCL=0;
  HROUT (0, ICYCL, " ");
  HREND ("tralig");
  CLOSEF(IOPA.hlun+1);




}
}
*/

bool AMSTrAligFit::Fillgl(AMSNode *pnode){
if(MAGSFFKEY.magstat>0){
 resetmagstat_();  
 time_t timenow;
time(&timenow);

AMSTimeID *ptid=  AMSJob::gethead()->gettimestructure();
AMSTimeID * offspring=dynamic_cast<AMSTimeID*>(ptid->down());
while(offspring){
  if(!strstr(offspring->getname(),"MagneticFieldMap") || !offspring->Verify()){
    offspring=(AMSTimeID*)offspring->next();
    continue;
  }
  integer nb=offspring->GetNbytes();
  if(offspring->validate(timenow)){
    cout <<"AMSEvent::_validate-I-"<<offspring->getname()<<" "<<offspring->getid()<<
      " validated. ("<<nb-sizeof(uinteger)<<" bytes ) CRC = "<<
      offspring->getCRC()<<endl;                                              
   }
    else {
      cerr<<"AMSEvent::_validate-F-"<<offspring->getname()<<" not validated."<<endl;
      time_t b,e,i;
      offspring->gettime(i,b,e);
      cerr<<" Time: "<<ctime(&timenow)<<endl;
      cerr<<" Begin : " <<ctime(&b)<<endl; 
      cerr<<" End : " <<ctime(&e)<<endl; 
      cerr<<" Insert : " <<ctime(&i)<<endl; 
      throw amsglobalerror("TDV Not    Validated ",3);
 
    }
    offspring=(AMSTimeID*)offspring->next();
  }

}


    char hfile[161];
    UHTOC(TRALIG.gfile,40,hfile,160);
         int totalall=0;
       if(hfile[0]=='\0'){
        return false;
       }
       else{
       InitDB();
       ifstream ftxt;
       ftxt.open(hfile);
        integer nh;
        geant chi2;
        AMSPoint hits[TKDBc::nlay()],ehits[TKDBc::nlay()],cooa[TKDBc::nlay()];
        geant hit[3],ehit[3],rig,mcrig;
        integer lay,lad,half,sen[TKDBc::nlay()],pattern,add1,add2;        
         
        int ntot=0;
   if(ftxt && 0){
        while(ftxt.good() && !ftxt.eof()){
         ftxt>>nh>>chi2>>pattern>>add1>>add2>>rig;
         for(int i=0;i<nh;i++){
          ftxt>>hit[0]>>hit[1]>>hit[2]>>ehit[0]>>ehit[1]>>ehit[2]>>lay>>lad>>half>>sen[lay-1];
         }
         ntot++;
        }
       }
        ftxt.clear();
        ftxt.close();
        ftxt.open(hfile);
        ntot=TRALIG.MaxEventsPerFit;
        if(ftxt){
           cout<<"TRALIG-I-EventFound "<<ntot<<endl;;
           AMSTrAligFit * pal =new AMSTrAligFit(1,0, ntot, TRALIG.Algorithm, 1);
            pnode->add(pal);       
            cout <<"AMSTrAligFit::FillGl-I-PatternAdded " <<1<<" "<<TRALIG.Algorithm<<endl;
        const int ntry=2;
        for(int ktry=0;ktry<ntry;ktry++){
        int num[2];
         num[0]=num[1]=0;
        while(ftxt.good() && !ftxt.eof()){
          rig=10000;
          totalall++;
          mcrig=10000;
          uint64 address;
//         ftxt>>nh>>chi2>>pattern>>add1>>add2>>rig;
         ftxt.read((char*)&nh,sizeof(nh));
         ftxt.read((char*)&chi2,sizeof(chi2));
         ftxt.read((char*)&pattern,sizeof(pattern));
         ftxt.read((char*)&address,sizeof(address));
       //  ftxt.read((char*)&add1,sizeof(add1));
      //   ftxt.read((char*)&add2,sizeof(add2));
         ftxt.read((char*)&rig,sizeof(rig));
         ftxt.read((char*)&mcrig,sizeof(mcrig));
         if(mcrig==0)mcrig=400;
//            mcrig=400;
         //ftxt>>nh>>chi2>>pattern>>add1>>add2;
         //uintl address(add1,add2);
         int ilad[maxlay]={0,0,0,0,0,0,0,0,0};
         int ihalf[maxlay]={0,0,0,0,0,0,0,0,0};
         for(int i=0;i<TKDBc::nlay();i++){
          sen[i]=0;
         }
         for(int i=0;i<nh;i++){
           ftxt.read((char*)&(hit[0]),sizeof(hit[0]));
           ftxt.read((char*)&(hit[1]),sizeof(hit[1]));
           ftxt.read((char*)&(hit[2]),sizeof(hit[2]));
           ftxt.read((char*)&(ehit[0]),sizeof(ehit[0]));
           ftxt.read((char*)&(ehit[1]),sizeof(ehit[1]));
           ftxt.read((char*)&(ehit[2]),sizeof(ehit[2]));
           ftxt.read((char*)&lay,sizeof(lay));
           ftxt.read((char*)&lad,sizeof(lad));
           ftxt.read((char*)&half,sizeof(half));
           ilad[lay-1]=lad;
           ihalf[lay-1]=half;
           ftxt.read((char*)&(sen[lay-1]),sizeof(sen[0]));
           if(sen[lay-1]==0){
            cerr<<"  sensor 0!!!"<<lay<<" "<<lad<<" "<<half<<endl;
            continue;
           }
           bool ok0=false;
if(lay==5 && half==1 && lad==3){
               ok0=true; 
           }
  //if(lay==4 && half==1 && lad==2)cout<<" ok 1"<<endl;
//           cout <<" nh "<<i<<" "<<rig<<" "<<mcrig<<" "<<nh<<" "<<lay<<" "<<lad<<" "<<half<<" "<<sen<<endl;
//          ftxt>>hit[0]>>hit[1]>>hit[2]>>ehit[0]>>ehit[1]>>ehit[2]>>lay>>lad>>half>>sen;
          hits[i]=AMSPoint(hit);
          ehits[i]=AMSPoint(ehit);
     integer sensor= half==0? sen[lay-1]: TKDBc::nhalf(lay,lad)+sen[lay-1];
     AMSTrIdGeom amd(lay,lad,sensor,0,0);
     AMSgvolume * psen= AMSJob::gethead()->getgeom(amd.crgid());
        if(TRALIG.LaddersOnly){
        psen=psen->up();
        if(psen && TRALIG.LayersOnly){
          psen=psen->up();
        }
       }
     if(!psen){
       cerr<< " AMSTrAligPar::Fillgl-E-SensorNotfound "<<amd;
       return false;
     }
          cooa[i]=psen->getcooA();

          
         }
         bool cut=abs(num[1]-num[0])<TRALIG.Cuts[9][0] || (num[1]-num[0])*rig>0;
         if(TRALIG.Cuts[9][1]!=0)cut= (TRALIG.Cuts[9][1]*rig>0);
//changed for pr 400 gev only
         cut=true;
         bool smart=false;
          int nentries=100000000;
             integer ladder[2][maxlay]={0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0};
             AMSTrTrack::decodeaddress(ladder,address);
             for(int k=0;k<TKDBc::nlay();k++){
                  ladder[0][k]=ilad[k];
                  ladder[1][k]=ihalf[k];
             } 
             for(int i=0;i<TKDBc::nlay();i++){
              if(ladder[0][i]){
                int ptr=TRALIG.LaddersOnly?0:sen[i]-1;
                int add=_pPargl[ptr][ladder[0][i]-1][ladder[1][i]][i].NEntries();
                if(add<nentries)nentries=add;
              }
             }

         for(int k=0;k<4;k++){
           int shift=k==0?0:k-ktry;
           smart=smart || (nh>=TRALIG.Cuts[7][0]-k && nentries<(TRALIG.SingleLadderEntryLimit>>shift));
         }
          int mult=TRALIG.LaddersOnly?100:1;
         if(smart && chi2<TRALIG.Cuts[7][1]*mult && fabs(rig)>TRALIG.Cuts[8][0]/1.5 &&cut){
          if(pal->_PositionData<pal->_NData){
            // UPdateGlobalParSpace;
             AMSTrTrack::decodeaddress(ladder,address);
           for(int k=0;k<TKDBc::nlay();k++){
                  ladder[0][k]=ilad[k];
                  ladder[1][k]=ihalf[k];
             }

             int add=0;
             int i;
             for(i=0;i<TKDBc::nlay();i++){
              if(ladder[0][i]){
                int ptr=TRALIG.LaddersOnly?0:sen[i]-1;
                add+=_pPargl[ptr][ladder[0][i]-1][ladder[1][i]][i].AddOne();
              }
             }
                    for(int i1=0;i1<maxlay;i1++){
                        ladder[0][i1]=ilad[i1];
                        ladder[1][i1]=ihalf[i1];
                    }
                    uint64 a1=AMSTrTrack::encodeaddress(ladder);
                    if(a1!=address){
                      address=a1;
                      //cerr<<" a1 "<<a1<<" "<<address;
                    }

             if(add){
                 int ll[3][maxlay];
                   for(int k=0;k<3;k++)for(int i=0;i<maxlay;i++)ll[k][i]=0; 
                 for(int k=0;k<TKDBc::nlay();k++){
                  ll[0][k]=ladder[0][k];
                  ll[1][k]=ladder[1][k];
                  ll[2][k]=sen[k];
                 }
                 uint128 Address=AMSTrTrack::encodeaddressS(ll);
                 int lll[3][maxlay];
        for(int k=0;k<3;k++)for(int i=0;i<maxlay;i++)lll[k][i]=0;

                AMSTrTrack::decodeaddressS(lll,Address);
                for(int k=0;k<maxlay;k++){
                 for(int k3=0;k3<3;k3++){
                   if(ll[k3][k]!=lll[k3][k]){
                    cerr<< " decodeerro "<<ll[k3][k]<<" "<<lll[k3][k]<<" "<<k3<<k<<endl;
                    uint128 a11=AMSTrTrack::encodeaddressS(ll);
                    AMSTrTrack::decodeaddressS(lll,a11);
//                    for(int i1=0;i1<maxlay;i1++){
//                        ladder[0][i1]=ilad[i1];
//                        ladder[1][i1]=ihalf[i1];
//                    }
//                   cerr <<"  kwa"<<endl;
                   }
                 }
                }

                (pal->_pData[(pal->_PositionData)++]).Init(pattern,Address,hits,ehits,cooa,mcrig);
                if(rig>0)num[0]++;
                else num[1]++;
             }
             else{
              for( i=0;i<TKDBc::nlay();i++){
               if(ladder[0][i]){
                int ptr=TRALIG.LaddersOnly?0:sen[i]-1;
                _pPargl[ptr][ladder[0][i]-1][ladder[1][i]][i].MinusOne();
               }
              }
             }
           }
          else{
           cout<<"TRALIG-W-AligCapacityReached"<< pal->_PositionData<<endl;
           break;
          }          
         }
         else{
            int cfalse=0;
         }          
                      
        }
        ftxt.clear();
        ftxt.close();
        ftxt.open(hfile);
       }
    if(pal){
        if(TRALIG.Algorithm<3){
         pal->Fitgl();
        }
        else{
         pal->RebuildNoActivePar();
         bool alreadydone=false;
again:
         int what=-2;
         geant arr[11][9];
         int fixpar[6][9];
         int alg=TRALIG.Algorithm;
          geant chi2m=0;
          geant xf[2]; 
          geant chi2[1000][2];
          geant rigmin=0;
          geant rigra[1000];
          int itermin=0;
          xf[0]=mcrig;
         FIT(arr,fixpar,chi2m,alg,what,xf,chi2,rigra,rigmin,itermin);
         what=0;
         cout <<" Total "<<pal->_PositionData<<" out of" <<totalall<<endl;
         for(int ip=0;ip<pal->_PositionData-1;ip++){
             if(ip<TRALIG.Skip)continue;
             if(ip>TRALIG.Skip+TRALIG.Max)continue;
             integer ladder[3][maxlay];
             AMSTrTrack::decodeaddressS(ladder,pal->_pData[ip]._Address);
             VZERO(arr,sizeof(arr)/sizeof(arr[0][0]));
             VZERO(fixpar,sizeof(fixpar)/sizeof(fixpar[0][0]));
             integer npt=TKDBc::patpoints((pal->_pData)[ip]._Pattern);
             // cout <<" npt "<<npt<<endl;
             if(npt<4){
               cerr<<"  npt "<<npt<<endl;
            }
             for(int i=0;i<npt;i++){
              int plane=TKDBc::patconf((pal->_pData)[ip]._Pattern,i)-1;
              if(plane==0 || plane==8){
//              no external planes
                static int warn=0;
                if(warn++<1){
                  cerr<<" TRALIG-W_NoExtPlanes !!! "<<endl;
                }
                continue;
              } 
              lad=ladder[0][plane];
              half=ladder[1][plane];
              int sens=ladder[2][plane];
              if(sens>trconst::maxsen || sens==0){
                  cerr<<" sens out of range "<<sens<<endl;
                  goto next;
              }
              if(lad==0 || lad>TKDBc::nlad(plane+1)){
                  cerr<<" lad out of range "<<lad<<endl;
                  goto next;
              } 
               int ptr=TRALIG.LaddersOnly?0:sens-1;
              for(int j=0;j<3;j++)arr[j][plane]=_pPargl[ptr][lad-1][half][plane].getcoo()[j]+_pPargl[ptr][lad-1][half][plane].getmtx(j).prod(((pal->_pData)[ip]._Hits[i]));
//              for(int j=0;j<3;j++)arr[j][plane]=((pal->_pData)[ip]._Hits[i])[j];
              for(int j=0;j<3;j++)arr[j+5][plane]=((pal->_pData)[ip]._EHits[0])[j];
              for(int j=0;j<3;j++)arr[j+7][plane]=((pal->_pData)[ip]._CooA[i])[j];
              arr[3][plane]=TRALIG.LaddersOnly?lad:(lad-1)*(trconst::maxsen+1)+sens;
            
              arr[4][plane]=half+1;
              arr[10][0]=1./(pal->_pData)[ip]._InvRigidity;
              for(int j=0;j<6;j++){
               fixpar[j][plane]=TRALIG.ActiveParameters[plane][j];
                if(_pPargl[ptr][lad-1][half][plane].NEntries()<TRALIG.MinEventsPerFit && TRALIG.LaddersOnly)fixpar[j][plane]=0;
              }
             }
     xf[0]=mcrig;

             FIT(arr,fixpar,chi2m,TRALIG.Algorithm,what,xf,chi2,rigra,rigmin,itermin);
next:
             if(ip<10+TRALIG.Skip){
              int ims=0;
              int ialgo=1;
              const integer maxhits=10;
              static geant hits[maxhits][3];
              static geant sigma[maxhits][3];
              static geant normal[maxhits][3];
              static integer layer[maxhits];
              for(int i=0;i<TKDBc::nlay();i++){
                normal[i][0]=0;
                normal[i][1]=0;
                normal[i][2]=-1;
              }
              int i=-1;
              int nptt=npt;
              for(int ii=0;ii<npt;ii++){
               int plane=TKDBc::patconf((pal->_pData)[ip]._Pattern,ii)-1;
              if(plane==0 || plane==8){
//              no external planes
                static int warn=0;
                if(warn++<1){
                  cerr<<" TRALIG-W_NoExtPlanes !!! "<<endl;
                }
                nptt--;
                continue;
              } 
                i++;
                layer[i]=plane+1;
                for(int j=0;j<3;j++){
                  hits[i][j]=arr[j][plane];
                  sigma[i][j]=((pal->_pData)[ip]._EHits[0])[j];
                }
              }
              geant out[40];
              int pid=5;
              out[0]=1./10000000.;
                        int ialgg=ialgo%10;
              TKFITG(nptt,hits,sigma,normal,pid,ialgg,ims,layer,out);
              cout <<"  fit "<<" "<<npt<<" "<<out[6]<<endl;
             }      
            }
            what=1;
            chi2m=TRALIG.Cuts[7][1];
            rigmin=TRALIG.Cuts[8][0];
            itermin=TRALIG.Cuts[8][1];
            cout <<" chi2m "<<chi2m<<"rigmin "<<rigmin<<" itermin "<<itermin<<endl;
              xf[0]=mcrig;
            FIT(arr,fixpar,chi2m,TRALIG.Algorithm,what,xf,chi2,rigra,rigmin,itermin);

             number nchi2a=0;
             float chi2a=0;
             number chi22a=0;
             for(int i=0;i<sizeof(chi2)/sizeof(chi2[0][0])/2;i++){
              if(chi2[i][1]>0 && chi2[i][1]<chi2m){
               nchi2a++;
               chi2a+=chi2[i][1];
               chi22a+=chi2[i][1]*chi2[i][1];
             }
             }
            if(nchi2a>0){
              chi2a/=nchi2a;
              chi22a/=nchi2a;
             }
             if(chi2a<chi2m/5)chi2a=chi2m/5;
            pal->_fcnI=xf[0];
            pal->_fcn=xf[1];
            for(int i=0;i<sizeof(pal->chi2)/sizeof(pal->chi2[0]);i++){
              pal->chi2i[i]=chi2[i][0];
              pal->chi2[i]=chi2[i][1];
              pal->rigra[i]=rigra[i];
            }
             cout <<"  Chi2 Changed "<<chi2a<<endl;
             what=-1;
             itermin=0;
              xf[0]=mcrig;
             FIT(arr,fixpar,chi2a,TRALIG.Algorithm,what,xf,chi2,rigra,rigmin,itermin);

            what=2;
            int whato;
            do{
             whato=what;             
             FIT(arr,fixpar,chi2m,TRALIG.Algorithm,what,xf,chi2,rigra,rigmin,itermin);
             AMSPoint outc;
             AMSPoint outa;
             AMSPoint coo;
             for(int i=0;i<TKDBc::nlay();i++){
//              number addon[8]={-1e-2,0,-6e-3,0,0,0,4e-3,0};
              number addon[9]={0,0,0,0,0,0,0,0,0};
              outc=AMSPoint(arr[0][i],arr[1][i],arr[2][i]+addon[i]);
              outa=AMSPoint(arr[3][i],arr[4][i],arr[5][i]); 
              coo=AMSPoint(arr[8][i],arr[9][i],arr[10][i]);
              int lad=arr[6][i]-1;
              int sens=0;
              if(!TRALIG.LaddersOnly){
                lad=int(arr[6][i])/(trconst::maxsen+1);
                sens=int(arr[6][i])%(trconst::maxsen+1)-1;
              }
              cout <<" i "<<i<<" "<<lad<<" "<<sens<<" "<<outc<<" "<<coo<< " "<<outa<<endl;
              int half=arr[7][i]-1;
              (_pPargl[sens][lad][half][i]).setpar(outc,outa); 
//               cout <<" "<<i<<" "<<lad<<" "<<half<<" "<<outc<<" "<<outa<<endl;
              for(int j=0;j<3;j++)outc[j]=outc[j]+coo[j]-(_pPargl[sens][lad][half][i].getmtx(j)).prod(coo);
//             cout <<" "<<i<<" "<<lad<<" "<<half<<" "<<outc<<" "<<outa<<endl;

               _pPargl[sens][lad][half][i].setpar(outc,outa);
             }
            }while(what!=whato);
           if(!alreadydone){
            alreadydone=true;
            pal->Analgl();
            goto again;
           }
       }
     }           
     // remove node
 //    pnode->down()->remove();
 //    pnode->setDown(0);
  char hpawc[256]="//PAWC";
  HCDIR (hpawc, " ");
  char houtput[]="//tralig";
  HCDIR (houtput, " ");
  integer ICYCL=0;
  HROUT (0, ICYCL, " ");
  HREND ("tralig");
  CLOSEF(IOPA.hlun+1);
     return true;
        }
        else{
         cerr<<"TRALIG-E-FillglUnabletoOpenFile "<<hfile<<endl;
          return false;
       }  
      }
 return false;
      }

bool AMSTrAligFit::Fillgle(AMSNode *pnode){
return true;
}
/*

    char hfile[161];
    UHTOC(TRALIG.gfile,40,hfile,160);
       if(hfile[0]=='\0'){
        return false;
       }
         int what=-2;
         int fixpar[6][20];
         for(int i=0;i<6;i++){
           for(int j=0;j<20;j++)fixpar[i][j]=0;
         }  
         geant arr[13][20];  // array :  x,y,z,nlad,nhalf,ex,ey,xr,yr,zr,mcrig
// first tof for the moment
         int nplan=2;  
          geant chi2m=0;
          geant xf[2]; 
          geant chi2[1000][2];
          geant rigmin=0;
          int itermin=0;
         FITE(arr,fixpar,chi2m,nplan,what,xf,chi2,rigmin,itermin);


        ifstream ftxt;
        int ntot=0;
        ftxt.open(hfile);
        ntot=TRALIG.MaxEventsPerFit;
        int ngood=0;
        while(ftxt.good() && !ftxt.eof()){
          float rig=10000;
          float mcrig=10000;
          float tofcoo[4][6];
          float TRDCoo[2][3];
          float TOFCoo[4][3];
          float EcalCoo[3][3];    
          float nxz,nyz;
//      ftxt.read((char *)clcoo,sizeof(clcoo));
      for(int i=0;i<4;i++){
         for(int j=0;j<6;j++){
          ftxt.read((char *)&(tofcoo[i][j]),sizeof(tofcoo[0][0]));
       }
      } 
          ftxt.read((char *)&(nxz),sizeof(nxz));
          ftxt.read((char *)&(nyz),sizeof(nyz));
      for(int i=0;i<2;i++){
         for(int j=0;j<3;j++){
          ftxt.read((char*)&(TRDCoo[i][j]),sizeof(TRDCoo[0][0]));
         }
      }
      for(int i=0;i<4;i++){
         for(int j=0;j<3;j++){
          ftxt.read((char*)&(TOFCoo[i][j]),sizeof(TOFCoo[0][0]));
         }
      }
      for(int i=0;i<3;i++){
         for(int j=0;j<3;j++){
          ftxt.read((char*)&(EcalCoo[i][j]),sizeof(EcalCoo[0][0]));
         }
      }
//    TOF 1-2
//  fill  the arrays       


             what=0;
             VZERO(arr,sizeof(arr)/sizeof(arr[0][0]));
             VZERO(fixpar,sizeof(fixpar)/sizeof(fixpar[0][0]));
              bool good=true;

              for(int plane=0;plane<nplan;plane++){
               for(int j=0;j<3;j++)arr[j][plane]=tofcoo[plane+2][j];
               for(int j=0;j<3;j++)arr[j+7][plane]=TOFCoo[plane+2][j];
               for(int j=0;j<2;j++)arr[j+5][plane]=tofcoo[plane+2][j+4];
               good=good && tofcoo[plane][3]>=0;
               if(plane+2==0 || plane+2==3){
                 arr[5][plane]=-100000;
               }
               else{
                 arr[6][plane]=-100000;
               } 
// / *
              nplan=20;
              
               for(int plane=0;plane<nplan;plane++){
               for(int j=xi20;j<3;j++)arr[j][plane]=trdcoo[plane][j];
               arr[7][plane]=TRDCoo[0][0]+nxz*(trdcoo[plane][2]-TRDCoo[0][2]);
               arr[8][plane]=TRDCoo[0][1]+nyz*(trdcoo[plane][2]-TRDCoo[0][2]);
               arr[9][plane]=trdcoo[plane][2];
               for(int j=0;j<2;j++)arr[j+5][plane]=trdcoo[plane][j+4];
               good=good && tofcoo[plane][3]>=0;
               if(plane+2==0 || plane+2==3){
                 arr[5][plane]=-100000;
               }
               else{
                 arr[6][plane]=-100000;
               }
// * / 
              arr[3][plane]=1;
               arr[4][plane]=1;
               arr[10][plane]=nxz;
               arr[11][plane]=nyz;
               arr[12][plane]=10000;
              }
              if(good){
                ngood++;
               FITE(arr,fixpar,chi2m,nplan,what,xf,chi2,rigmin,itermin);
              }
             if(ngood>200000)break;
                      
        }
        ftxt.clear();
        ftxt.close();
            what=1;
            chi2m=TRALIG.Cuts[7][1];
            rigmin=TRALIG.Cuts[8][0];
            itermin=TRALIG.Cuts[8][1];
            cout <<" chi2m "<<chi2m<<"rigmin "<<rigmin<<" itermin "<<itermin<<endl;
            FITE(arr,fixpar,chi2m,nplan,what,xf,chi2,rigmin,itermin);
             number nchi2a=0;
             float chi2a=0;
             number chi22a=0;
             for(int i=0;i<sizeof(chi2)/sizeof(chi2[0][0])/2;i++){
              if(chi2[i][1]>0 && chi2[i][1]<chi2m){
               nchi2a++;
               chi2a+=chi2[i][1];
               chi22a+=chi2[i][1]*chi2[i][1];
             }
             }
            
            if(nchi2a>0){
              chi2a/=nchi2a;
              chi22a/=nchi2a;
             }
            what=2;
            int whato;
            do{
             whato=what;             
             FITE(arr,fixpar,chi2m,nplan,what,xf,chi2,rigmin,itermin);
             AMSPoint outc;
             AMSPoint outa;
             AMSPoint coo;
              outc=AMSPoint(arr[0][0],arr[1][0],arr[2][0]);
              outa=AMSPoint(arr[3][0],arr[4][0],arr[5][0]); 
              coo=AMSPoint(arr[8][0],arr[9][0],arr[10][0]);
              (_pPargl[0][0][0][0]).setpar(outc,outa); 
//              for(int j=0;j<3;j++)outc[j]=outc[j]+coo[j]-(_pPargl[sens][lad][half][i].getmtx(j)).prod(coo);
//             cout <<" "<<i<<" "<<lad<<" "<<half<<" "<<outc<<" "<<outa<<endl;

//               _pPargl[sens][lad][half][i].setpar(outc,outa);
             break;
            }while(what!=whato);
            return true;
        }
*/



/*

void AMSTrAligFit::Testgl(integer forced){


static int skip=0;



AMSNode *pnode =AMSJob::gethead()->getaligstructure();
if(!pnode)return;
AMSTrAligFit * pal = (AMSTrAligFit *)pnode->down();

if(forced==0){
 AMSParticle  * ppart=0;
 AMSmceventg * ptrg=0;
  if(Select(ppart,ptrg,TRALIG.Algorithm)){
     AMSTrTrack *ptrack=ppart->getptrack();
     integer nnodes=0;
     integer found=0;
      while(pal){
        nnodes++;
        if( 1 ){
//        if(  pal->AddressOK(1,1) ){
          int itst=GCFLAG.ITEST>10?GCFLAG.ITEST/10:1;        
           if(pal->_PositionData%itst==1)cout <<"AMSTrAligFit::Testgl-I "<<pal->_PositionData<<" events in memory "<<endl;
           if(pal->_PositionData%GCFLAG.ITEST==1){
             for(int ilay=0;ilay<TKDBc::nlay();ilay++){
               for(int iside=0;iside<2;iside++){
                for(int ilad=0;ilad<trconst::maxlad;ilad++){
                 for(int isen=0;isen<trconst::maxsen+1;isen++){  
                 if(_pPargl[isen][ilad][iside][ilay].NEntries()>0){
                   cout << ilay<<" "<<iside<<" "<<ilad<<" "<< _pPargl[ilad][iside][ilay].NEntries()<<endl;
                 }
                }
                }
               }
             }
           }
           if(pal->_PositionData<pal->_NData){
            // UPdateGlobalParSpace;
             integer ladder[2][maxlay];
             AMSTrTrack::decodeaddress(ladder,ptrack->getaddress());
             int add=0;
             int i;
             for(i=0;i<TKDBc::nlay();i++){
              if(ladder[0][i]){
                add+=_pPargl[ladder[0][i]-1][ladder[1][i]][i].AddOne();
              }
             }
             if(add)(pal->_pData[(pal->_PositionData)++]).Init(ptrack,ptrg);
             else{
              for( i=0;i<TKDBc::nlay();i++){
               if(ladder[0][i]){
                _pPargl[ladder[0][i]-1][ladder[1][i]][i].MinusOne();
               }
              }
             }
           }
           else if (TRALIG.LayersOnly){
              cout <<"AMSTrAligFit::Testgl-I-MaxEventNoReached.Terminating 0 "<<" " <<pal->_NData<<endl;
                Testgl(1);
           }
           else {
              cout <<"AMSTrAligFit::Testgl-I-MaxEventNoReached.Terminating 1 "<<pal->_NData<<endl;
                Testgl(1);
//              raise(SIGTERM);
           }
           if(skip++>TRALIG.EventsPerRun){
              //if(GCFLAG.IEORUN==0)GCFLAG.IEORUN=2;
              skip=0;
           }
           return;
        }
        pal=pal->next(); 
     }
      if(nnodes<TRALIG.MaxPatternsPerJob){
           integer found=0;
            pal=new AMSTrAligFit(1,0, TRALIG.MaxEventsPerFit, TRALIG.Algorithm, nnodes+1);
            if(pal->_PositionData<pal->_NData)(pal->_pData[(pal->_PositionData)++]).Init(ptrack,ptrg);
            pnode->add(pal);       
            cout <<"AMSTrAligFit::TestGl-I-PatternAdded " <<1<<" "<<TRALIG.Algorithm<<endl;
           }
      }
  }



else {


   // Open Ntuple Fit/fill Ntuple & update Db

     if(pal){
         pal->Fitgl();
     }
     // remove node
     pnode->down()->remove();
     pnode->setDown(0);
if(forced>1){
 
  // close ntuple


  char hpawc[256]="//PAWC";
  HCDIR (hpawc, " ");
  char houtput[]="//tralig";
  HCDIR (houtput, " ");
  integer ICYCL=0;
  HROUT (0, ICYCL, " ");
  HREND ("tralig");
  CLOSEF(IOPA.hlun+1);

}


}
}

*/

void AMSTrAligFit::Fit(){
    void (*palfun)(int &n, double x[], double &f, AMSTrAligFit *p)=&AMSTrAligFit::alfun;
    void (*pmonit)(number &a, number &b, number sim[], int &n, int &s, int &nca)=
                                &AMSTrAligFit::monit;
      if(_PositionData<TRALIG.MinEventsPerFit){
        cerr << "AMSTrAligFit::Fit-E-TooFewEvents for "<<_Pattern<<" "<<_Address<<" "<<_PositionData<<endl;
        return;
      }
      cout << "AMSTrAligFit::Fit started for pattern "<<_Pattern<<" "<<_Address<<endl;
  // Fit Here
    const integer mp=40;
    number f,x[mp],w1[mp],w2[mp],w3[mp],w4[mp],w5[mp+1],w6[mp*(mp+1)];
  // number of parameters to fit
    integer n=_NoActivePar;
    integer iw=n+1;
    integer ifail=1;
    integer maxcal=1;
    number tol=2.99e-2;
    int i,j;
    for(i=0;i<mp;i++)x[i]=0;
    _flag=3;
    e04ccf_(n,x,f,tol,iw,w1,w2,w3,w4,w5,w6,(void*)palfun,(void*)pmonit,maxcal,ifail,this);
    for(i=0;i<mp;i++)x[i]=0;
     _pfitbefore=_tmppav;
     ifail=0;
     if(fabs(1-_pfitbefore)>TRALIG.GlobalGoodLimit){
      ifail=1;
      maxcal=2000;
      _flag=0;
      e04ccf_(n,x,f,tol,iw,w1,w2,w3,w4,w5,w6,(void*)palfun,(void*)pmonit,maxcal,ifail,this);
      cout << "AMSTrAligFit::Fit finished "<<ifail<<" "<<f<<endl;
      if(ifail ==0 ){
       _flag=2;
       ifail=1;
       number fd;
       integer one(1);
       e04ccf_(n,x,fd,tol,iw,w1,w2,w3,w4,w5,w6,(void*)palfun,(void*)pmonit,one,ifail,this);
       ifail=0;    
      }
     }
     if(ifail==0){
      AMSPoint outc[TKDBc::nlay()];
      AMSPoint outa[TKDBc::nlay()];
      for(i=0;i<TKDBc::nlay();i++){
        outc[i]=0;
        outa[i]=0;
      }
      for(i=0;i<n;i++){
       int plane=_PlaneNo[i];
       int parno=_ParNo[i];
       if(parno<3)outc[plane][parno]=x[i];
       else outa[plane][parno-3]=x[i]*AMSDBc::pi/180.;
      }
 
      for(i=0;i<TKDBc::nlay();i++){
        (_pParC[i]).setpar(outc[i],outa[i]);
      }
      _fcn=f;
      _fcnI=_tmp;
      _pfit=_tmppav;
       cout <<"tmpav "<<_tmppav<<" "<<_tmppsi<<endl;
      _pfits=_tmppsi;
      Anal();
     }
    else cerr <<"FIt-E-IfailNotZero "<<ifail<<endl;

 

}

/*
void AMSTrAligFit::Fitgl(){
    void (*palfun)(int &n, double x[], double &f, AMSTrAligFit *p)=&AMSTrAligFit::alfungl;
    void (*pmonit)(number &a, number &b, number sim[], int &n, int &s, int &nca)=
                                &AMSTrAligFit::monit;
      cout << "Global AMSTrAligFit::Fit started" <<endl;
  // Fit Here
  // number of parameters to fit
    const integer mp=350;
    number f,x[mp],w1[mp],w2[mp],w3[mp],w4[mp],w5[mp+1],w6[mp*(mp+1)];

   // rebuild noactivepar
    RebuildNoActivePar();


    integer n=_NoActivePar;
    integer iw=n+1;
    integer ifail=1;
    integer maxcal=50000;
    number tol=2.e-2;
    int i,j;
     _Chi2Max=0;
    for(i=0;i<mp;i++)x[i]=0;
    _flag=3;
    e04ccf_(n,x,f,tol,iw,w1,w2,w3,w4,w5,w6,(void*)palfun,(void*)pmonit,maxcal,ifail,this);
    cout << "AMSTrAligFit::Fit finished "<<ifail<<" "<<f<<endl;
     number chi2s=0;
     number chi22s=0;
     number chi2n=0;
    for (int i=0;i<sizeof(chi2)/sizeof(chi2[0]);i++){
     if(chi2[i]>0){
      chi2s+=chi2[i];
      chi22s+=chi2[i]*chi2[i];
      chi2n++;
     }
    }
    if(chi2n>1){
      chi2s/=chi2n;    
      chi22s/=chi2n;    
      chi22s=sqrt(chi22s-chi2s*chi2s);
    }
    _Chi2Max=chi2s+chi22s;
    cout <<"AMSTrAligFit::Fit Chi2MaxSet "<<_Chi2Max<<" "<<endl;
//    ifail=1;
//    e04ccf_(n,x,f,tol,iw,w1,w2,w3,w4,w5,w6,(void*)palfun,(void*)pmonit,maxcal,ifail,this);
    cout << "AMSTrAligFit::Fit finished "<<ifail<<" "<<f<<endl;
    if(ifail ==0 || ifail==2){
     _flag=2;
     ifail=1;
     number fd;
     integer one(1);
     _Chi2Max=0;
     e04ccf_(n,x,fd,tol,iw,w1,w2,w3,w4,w5,w6,(void*)palfun,(void*)pmonit,one,ifail,this);
     AMSPoint outc[trconst::maxlad][2][TKDBc::nlay()];
     AMSPoint outa[trconst::maxlad][2][TKDBc::nlay()];
     for(i=0;i<trconst::maxlad;i++){ 
     for(j=0;j<2;j++){
     for(int k=0;k<TKDBc::nlay();k++){
       outc[i][j][k]=AMSPoint(0,0,0);
       outa[i][j][k]=AMSPoint(0,0,0);
     }
     }
     }

     for(i=0;i<n;i++){
      int plane=_PlaneNo[i];
      int ladder=_LadderNo[i];
      int half=_HalfNo[i];
      int parno=_ParNo[i];
      if(parno<3)outc[ladder][half][plane][parno]=x[i];
      else outa[ladder][half][plane][parno-3]=x[i]*AMSDBc::pi/180.;
     }

     for(i=0;i<trconst::maxlad;i++){
     for(j=0;j<2;j++){
     for(int k=0;k<TKDBc::nlay();k++){
        AMSPoint coo(_pPargl[i][j][k].getcoo());
        cout <<"  coo "<<i<<" "<<j<<" "<<k<<" "<<coo<<endl;
       (_pPargl[i][j][k]).setpar(outc[i][j][k],outa[i][j][k]);
        cout <<"  ppargl "<<i<<" "<<j<<" "<<k<<" "<<_pPargl[i][j][k].getcoo()<<endl;
        for(int m=0;m<3;m++)outc[i][j][k][m]=outc[i][j][k][m]+coo[m]-(_pPargl[i][j][k].getmtx(m)).prod(coo);
       (_pPargl[i][j][k]).setpar(outc[i][j][k],outa[i][j][k]);
        cout <<"  ppargl "<<i<<" "<<j<<" "<<k<<" "<<_pPargl[i][j][k].getcoo()<<endl;
     }
     }
     }
     _fcn=f;
     _fcnI=_tmp;
     _pfit=_tmppav;
      cout <<"tmpav "<<_tmppav<<" "<<_tmppsi<<endl;
     _pfits=_tmppsi;
     Analgl();
     }
    else cerr <<"FIt-E-IfailNotZero "<<ifail<<endl;

 

}
*/

AMSTrAligPar::AMSTrAligDB AMSTrAligPar::_traldb;
AMSTrAligPar   AMSTrAligPar::par[trconst::maxlay];


/*
void AMSTrAligFit::Anal(){

cout <<" AMSTrAligFit::Anal called for pattern "<<_Pattern<<" "<<_Address<<endl;
  static  AMSTrAligFit::TrAlig_def TRALIGG;
  static integer action=0;
  static integer upddone=0;
  if(action==0){
    action=1;
    char hfile[161];
    UHTOC(IOPA.hfile,40,hfile,160);
    char filename[256];
    strcpy(filename,hfile);
    integer iostat;
    integer rsize=1024;
    char event[80];
    HROPEN(IOPA.hlun+1,"tralig",filename,"NP",rsize,iostat);
    if(iostat){
     cerr << "Error opening tralig ntuple file "<<filename<<endl;
    }
    else cout <<"tralig ntuple file "<<filename<<" opened."<<endl;

   HBNT(IOPA.ntuple+1,"Tracker Alignment"," ");
   HBNAME(IOPA.ntuple+1,"TrAlig",(int*)(&TRALIGG),"Pattern:I,Alg:I,Address(2):I,FCN:R,FCNI:R,PFIT:R,PFITS:R,Coo(3,6):R,Angle(3,6):R");

  }
  int i,j,k,l;
      TRALIGG.Pattern=_Pattern;
      TRALIGG.Alg=_Algorithm;
      TRALIGG.Address[0]=_Address(0);
      TRALIGG.Address[1]=_Address(1);
      TRALIGG.FCN=_fcn;
      TRALIGG.FCNI=_fcnI;
      TRALIGG.Pfit=_pfit;
      TRALIGG.Pfitsig=_pfits;
    for(j=0;j<TKDBc::nlay();j++){
      for(k=0;k<3;k++){
       TRALIGG.Coo[j][k]=_pParC[j].getcoo()[k];
       TRALIGG.Angle[j][k]=_pParC[j].getang()[k];
      }
    }
    HFNT(IOPA.ntuple+1);

   // updatedb
   // I need to convert _pParc to a local ladder level
   integer ladder[2][maxlay];
   AMSTrTrack::decodeaddress(ladder,_Address);
   for(int il=0;il<TKDBc::nlay();il++){
    if(ladder[0][il]){
     integer sensor= ladder[1][il]==0? 1: TKDBc::nhalf(il+1,ladder[0][il])+1;
     AMSTrIdGeom amd(il+1,ladder[0][il],sensor,0,0);
     AMSgvolume * psen= AMSJob::gethead()->getgeom(amd.crgid());
     if(!psen){
       cerr<< " AMSTrAligPar::Anal-E-SensorNotfound "<<amd;
       return; 
     }
     else{
      psen=psen->up();
      AMSPoint coov;
      number coom[3][3];
      for (i=0;i<3;i++){
        coov[i]=psen->getcooA(i);
        for(j=0;j<3;j++){
         coom[i][j]=psen->getnrmA(i,j);
        }
      }

      number nrmN[3][3];
      AMSPoint cooN=_pParC[il].getcoo();
      for( j=0;j<3;j++){
        for( k=0;k<3;k++){
            AMSDir stipud_cxx=_pParC[il].getmtx(j);
            nrmN[j][k]=stipud_cxx[k];
        }
      }
      cooN=cooN+coov.mm3(nrmN);
      amsprotected::mm3(nrmN,coom,0);
      _pParC[il].setcoo(cooN);
      for( j=0;j<3;j++){
       _pParC[il].setmtx(j)= AMSDir(coom[j][0],coom[j][1],coom[j][2]);  
      }
     }
    }
    _pParC[il].updangles();
   }
   
   AMSTrAligPar::UpdateDB(_Address,_pParC,_fcnI,_fcn,_pfit,_pfitbefore);


}

*/


void AMSTrAligFit::Analgl(){

cout <<" AMSTrAligFit::Analgl called for pattern "<<_Pattern<<" "<<_Address<<endl;
  static  AMSTrAligFit::TrAligg_def TRALIGG;
  static integer action=0;
  static integer upddone=0;
  if(action++==0){
    char hfile[161];
    UHTOC(IOPA.hfile,40,hfile,160);
    char filename[256];
    strcpy(filename,hfile);
    integer iostat;
    integer rsize=1024;
    char event[80];
    HROPEN(IOPA.hlun+1,"tralig",filename,"NP",rsize,iostat);
    if(iostat){
     cerr << "Error opening tralig ntuple file "<<filename<<endl;
    }
    else cout <<"tralig ntuple file "<<filename<<" opened."<<endl;

   HBNT(IOPA.ntuple+1,"Tracker Alignment"," ");
   HBNAME(IOPA.ntuple+1,"TrAlig",(int*)(&TRALIGG),"Alg:I,Layer:I,Ladder:I,Sensor:I,Side:I,FCN:R,FCNI:R,CHI2(1000):R,CHI2I(1000):R,rigra(1000):R,ndata:I,PFIT:R,PFITS:R,Coo(3):R,Angle(3):R,Stat:I");
 

  }
  int i,j,k,l;
   for(j=0;j<TKDBc::nlay();j++){
     for(int nsen=0;nsen<trconst::maxsen;nsen++){
     for(int nlad=0;nlad<trconst::maxlad;nlad++){
       for(int side=0;side<2;side++){
        if(TKDBc::activeladdshuttle(j+1,nlad+1,side )|| TRALIG.LayersOnly){
          TRALIGG.Alg=10000*_Algorithm+action-1;
          TRALIGG.Layer=j+1;
          TRALIGG.Ladder=nlad+1;
          TRALIGG.Sensor=nsen+1;
          TRALIGG.Side=side;
          TRALIGG.ndata=_PositionData;
          TRALIGG.FCN=_fcn;
          TRALIGG.FCNI=_fcnI;
          TRALIGG.Pfit=_pfit;
          TRALIGG.Pfitsig=_pfits;
     for(int ll=0;ll<sizeof(chi2)/sizeof(chi2[0]);ll++){
      TRALIGG.CHI2[ll]=chi2[ll];
      TRALIGG.CHI2I[ll]=chi2i[ll];
      TRALIGG.rigra[ll]=rigra[ll];
     }
          TRALIGG.Stat=_pPargl[nsen][nlad][side][j].NEntries();
           for(k=0;k<3;k++){
            TRALIGG.Coo[k]=_pPargl[nsen][nlad][side][j].getcoo()[k];
            TRALIGG.Angle[k]=_pPargl[nsen][nlad][side][j].getang()[k];
           }
           if(!TRALIG.LayersOnly || (nlad==0 &&side==0)){
            HFNT(IOPA.ntuple+1);
           }
          }
         }
    }
        }
   }
  UpdateDBgl();


}




integer AMSTrAligFit::Select(AMSParticle * & ppart, AMSmceventg * & pmcg, integer alg){
      ppart=(AMSParticle*)AMSEvent::gethead()->getheadC("AMSParticle",0);
      pmcg=0;
      if(!ppart )return 0;      // Particle Should be Present always

  // Algorithm == 0 presumes momentum is knows,
  // pmcg may exist, if not create it from datacards
   if(alg==0){ 
         pmcg=(AMSmceventg*)AMSEvent::gethead()->getheadC("AMSmceventg",0);  
         if(!pmcg){
             AMSPoint coo(CCFFKEY.coo[0],CCFFKEY.coo[1],CCFFKEY.coo[2]);
             AMSDir dir(CCFFKEY.dir[0],CCFFKEY.dir[1],CCFFKEY.dir[2]);
             pmcg=new AMSmceventg(GCKINE.ikine, CCFFKEY.momr[0], coo, dir);  
              AMSEvent::gethead()->addnext(AMSID("AMSmceventg",0), pmcg);   
         }
   }

   else if(alg==1){
// Algorithm == 1 presumes getting momentum from beta measurement & from gckine.ikine
 //    always create pmcg
      integer pid=ppart->getgpart();
      AMSBeta * pbeta=ppart->getpbeta();
      number beta=pbeta->getbeta();
       if(fabs(beta)<1. ){
         AMSPoint coo=ppart->gettofcoo(0);
         AMSDir dir(ppart->gettheta(),ppart->getphi());
         geant xmom=0;   
         pmcg= new AMSmceventg(GCKINE.ikine, xmom, coo, dir);  
         number gamma=1./sqrt(1-beta*beta);
         xmom=pmcg->getmass()*fabs(beta)*gamma;
         pmcg->setmom(xmom);
        AMSEvent::gethead()->addnext(AMSID("AMSmceventg",0), pmcg);  
       }
   }
   else if(alg==2){
        //  assumes no mag field ; fitting by str lines
       
   }
   else{
     static int nmsg=0;
     if(nmsg++<10)cerr<<"AMSTrAligFit::Select-F-AlgNo "<<alg<<" IsNotSupportedYet"<<endl;
     return 0;
   }
   // Apply Cuts   
   if(pmcg){
      if(alg==0){
        number charge=pmcg->getcharge();
        number momentum=pmcg->getmom();
        number beta=ppart->getpbeta()->getbeta();
        AMSDir dir=pmcg->getdir();
        number recharge=ppart->getcharge();
        if(ppart->getmomentum()<0)recharge=-recharge;
        if(recharge==charge){
           AMSTrTrack *ptr=ppart->getptrack();              
           number chi2_3d, chi2z, rid;
           ptr->getParSimpleFit(chi2_3d,chi2z,rid);
           number cth=cos(ppart->gettheta())*cos(dir.gettheta())+
           sin(ppart->gettheta())*sin(dir.gettheta())*cos(ppart->getphi()-dir.getphi());
           
                   if(chi2_3d<TRALIG.Cuts[0][1] && cth > TRALIG.Cuts[1][0] &&
                      fabs(ppart->getmomentum())>momentum*TRALIG.Cuts[2][0] &&
                      fabs(ppart->getmomentum())<momentum*TRALIG.Cuts[2][1]  ){
                      if(beta>TRALIG.Cuts[3][0] && beta< TRALIG.Cuts[3][1]){    
                       return 1;
                      }
                    }
        }
      }
      else {
          // Alg 1 cuts

        number pmass=ppart->getmass();
        number gmass=pmcg->getmass();
        number charge=pmcg->getcharge();
        number momentum=pmcg->getmom();
        number beta=ppart->getpbeta()->getbeta();
        number betaerror=ppart->getpbeta()->getebeta();
        AMSDir dir=pmcg->getdir();
        number recharge=ppart->getcharge();
        if(ppart->getmomentum()<0)recharge=-recharge;
        if(recharge==charge){
             static int tot2=0;
             static int tot3=0;
             static int tot4=0;
             static int tot5=0;
             static int tot6=0;
             static int tot7=0;
           AMSTrTrack *ptr=ppart->getptrack();              
           number chi2_3d, chi2z, rid;
           ptr->getParSimpleFit(chi2_3d,chi2z,rid);
           number cth=cos(ppart->gettheta())*cos(dir.gettheta())+
           sin(ppart->gettheta())*sin(dir.gettheta())*cos(ppart->getphi()-dir.getphi());
           
                  //cout <<"tot2 "<<++tot2<<" "<<chi2_3d<<" "<<cth;
                    //cout <<" "<<ppart->getmomentum()<<" "<<momentum<<endl;
                   if(chi2_3d<TRALIG.Cuts[0][1] && cth > TRALIG.Cuts[1][0] &&
                      fabs(ppart->getmomentum())>momentum*TRALIG.Cuts[2][0] &&
                      fabs(ppart->getmomentum())<momentum*TRALIG.Cuts[2][1]  ){
                  //cout <<"tot3 "<<++tot3<<" "<<beta<<" "<<endl;
                      if(beta>TRALIG.Cuts[3][0] && beta< TRALIG.Cuts[3][1]){                            integer betapat=ppart->getpbeta()->getpattern();
                      //   cout <<"tot4 "<<++tot4<<" "<<betapat<<" "<<endl;
                         if(betapat>=TRALIG.Cuts[4][0] && betapat>=TRALIG.Cuts[4][1]){
                        //   cout <<"tot5 "<<++tot5<<" "<<pmass<<" "<<pmass/gmass<<endl;
                           if(pmass/gmass> TRALIG.Cuts[5][0] && pmass/gmass <TRALIG.Cuts[5][1]){
                          cout <<"tot6 "<<++tot6<<" "<<betaerror<<" "<<endl;
                            if(betaerror < TRALIG.Cuts[6][1]){
                             return 1;
                            }
                          }
                         }
                      }
                    }
        }

      }
   }
   if(alg==2){
      // alg 2 cuts
      // only one tr track
      // only  one trd track
      // less than 5 trd segments 
      int ntrd=(AMSEvent::gethead()->getC("AMSTRDTrack",0))->getnelem();
      int ntrds=0;
      for(int i=0;i<5;i++)ntrds+=(AMSEvent::gethead()->getC("AMSTRDSegment",i))->getnelem();
      int ntrk=(AMSEvent::gethead()->getC("AMSTrTrack",0))->getnelem();
      if(ntrk==1 && ntrd==1 && ntrds<6){
      AMSTrTrack *     ptr=ppart->getptrack();              
      if(ptr->getnhits()>TRALIG.Cuts[7][0]){
        int ntof=(AMSEvent::gethead()->getC("AMSTOFCluster",0))->getnelem()+
                  (AMSEvent::gethead()->getC("AMSTOFCluster",1))->getnelem();
        if(ntof==2  ){
           if(ptr->getchi2()<TRALIG.Cuts[7][1]){
            return 1;
           }
           else{
             cerr<<"  event rejected "<<" "<<ptr->getchi2()<<endl;
          }
       }
      }
   }
   }
   return 0;
}



void AMSTrAligFit::alfun(integer &n, number xc[], number &fc, AMSTrAligFit *p){
  integer i,niter;
  fc=0;
  for(i=0;i<n;i++){
    if(fabs(xc[i])>=0.199 ){
    fc=FLT_MAX;
    return;
    }
  }
  const integer maxhits=10;
  static geant hits[maxhits][3];
  static geant hits1[maxhits][3];
  static geant sigma[maxhits][3];
  static geant sigma1[maxhits][3];
  static geant normal[maxhits][3];
  static integer layer[maxhits];
  integer ialgo=11;
  integer ims=0;
  geant out[40];
  for(i=0;i<TKDBc::nlay();i++){
     normal[i][0]=0;
     normal[i][1]=0;
     normal[i][2]=-1;
  }
  integer npfit=0;
   // convert parameters
   AMSTrAligPar par[6];
   static integer idontwanttofit=0;
   static number fool_e04ccf=0; 
 {
   if(p->_flag==0 && idontwanttofit){
     fc=fool_e04ccf;
     return;
    }     
     AMSPoint outc[TKDBc::nlay()];
     AMSPoint outa[TKDBc::nlay()];
     for(i=0;i<TKDBc::nlay();i++){
       outc[i]=0;
       outa[i]=0;
     }
     for(i=0;i<n;i++){
      int plane=p->_PlaneNo[i];
      int parno=p->_ParNo[i];
      if(parno<3)outc[plane][parno]=xc[i];
      else outa[plane][parno-3]=xc[i]*AMSDBc::pi/180.;
     }

     for(i=0;i<6;i++){
       par[i].setpar(outc[i],outa[i]);
     }


 }
   number pav=0;
   number pav2=0;
   //if(p->_flag)HBOOK1(p->_Address+p->_flag,"my distr",50,0.,2.,0.);
  for(niter=0;niter<p->_PositionData;niter++){
   integer npt=TKDBc::patpoints((p->_pData)[niter]._Pattern);
   for(i=0;i<npt;i++){
     int plane=TKDBc::patconf((p->_pData)[niter]._Pattern,i)-1;
     for(int j=0;j<3;j++){
       hits[i][j]=(par[plane].getcoo())[j]+
       (par[plane].getmtx(j)).prod((p->_pData)[niter]._Hits[i]);
       sigma[i][j]=(par[plane].getmtx(j)).prod((p->_pData)[niter]._EHits[0]);
     }
   }
   out[0]=p->_pData[niter]._InvRigidity;
   TKFITG(npt,hits,sigma,normal,p->_pData[niter]._Pid,ialgo,ims,layer,out);
   if(out[7]==0 && out[5]!=0 ){
    if(ialgo/10 && p->_flag){
      geant out1[9];
      int ia=1;
      TKFITG(npt,hits,sigma,normal,p->_pData[niter]._Pid,ia,ims,layer,out1);
      out[5]=out1[5];
      geant xx=out[5]*p->_pData[niter]._InvRigidity;
//      HF1(p->_Address+p->_flag,xx,1.);
      pav+=1./out[5]/p->_pData[niter]._InvRigidity;
      pav2+=1./out[5]/out[5]/p->_pData[niter]._InvRigidity/p->_pData[niter]._InvRigidity;
    }
    number error=out[8];
    fc+=out[6];
    npfit++;
   }
  }
  if(npfit < n+2)fc=FLT_MAX;
  else fc=fc/(npfit-n);
  //cout <<" fc "<<fc<<" "<< npfit<<endl;
//  if(p->_flag)HPRINT(p->_Address+p->_flag);
  if(p->_flag==2 && npfit>1){
       pav/=npfit;
       pav2/=npfit;
       pav2=sqrt(fabs(pav2-pav*pav));
       p->_tmppav=pav;
       p->_tmppsi=pav2;
       for(i=0;i<6;i++)cout <<" "<<xc[i]<<" ";
       cout << endl<<" alfun out " <<fc<<" "<<npfit<<" "<<pav<<" "<<pav2<<" "<<n<<endl;
       p->_flag=0;
  }
      if(p->_flag==3){
       if(npfit>1){
        pav/=npfit;
        pav2/=npfit;
        pav2=sqrt(fabs(pav2-pav*pav));
       }
       for(i=0;i<6;i++)cout <<" "<<xc[i]<<" ";
       cout << endl<<" alfun out " <<fc<<" "<<npfit<<" "<<pav<<" "<<pav2<<" "<<n<<endl;
       p->_tmp=fc;
       p->_flag=0;
       p->_tmppav=pav;
       p->_tmppsi=pav2;
       fool_e04ccf=fc;
//       if(fabs(1-pav)<TRALIG.GlobalGoodLimit)idontwanttofit=1;
//       else idontwanttofit=0;
      }
}

/*

void AMSTrAligFit::alfungl(integer &n, number xc[], number &fc, AMSTrAligFit *p){
if(MAGSFFKEY.magstat>0){
  integer i,niter;
  fc=0;
  for(i=0;i<n;i++){
    if(fabs(xc[i])>=0.199 ){
    fc=FLT_MAX;
    return;
    }
  }
  static number fool_e04ccf=0;
  if(!p->_flag){
    int allzero=1;
    for(int k=0;k<n;k++){
     if(xc[k]!=0){
       allzero=0;
       break;
     }
    }
    if(allzero){
     fc=fool_e04ccf;
     return;
    }
  }
  const integer maxhits=10;
  static geant hits[maxhits][3];
  static geant hits1[maxhits][3];
  static geant sigma[maxhits][3];
  static geant sigma1[maxhits][3];
  static geant normal[maxhits][3];
  static integer layer[maxhits];
  integer ialgo=11;
  integer ims=0;
  geant out[40];
  for(i=0;i<TKDBc::nlay();i++){
     normal[i][0]=0;
     normal[i][1]=0;
     normal[i][2]=-1;
  }
  integer npfit=0;
   // convert parameters
   AMSTrAligPar par[trconst::maxlad][2][TKDBc::nlay()];
 {
     AMSPoint outc[trconst::maxlad][2][TKDBc::nlay()];
     AMSPoint outa[trconst::maxlad][2][TKDBc::nlay()];
     for(i=0;i<trconst::maxlad;i++){
     for(int j=0;j<2;j++){
     for(int k=0;k<TKDBc::nlay();k++){
       outc[i][j][k]=0;
       outa[i][j][k]=0;
     }
     }
     }
     for(i=0;i<n;i++){
      int plane=p->_PlaneNo[i];
      int parno=p->_ParNo[i];
      int ladder=p->_LadderNo[i];
      int half=p->_HalfNo[i];
      if(parno<3)outc[ladder][half][plane][parno]=xc[i];
      else outa[ladder][half][plane][parno-3]=xc[i]*AMSDBc::pi/180.;
     }

     for(i=0;i<trconst::maxlad;i++){
     for(int j=0;j<2;j++){
     for(int k=0;k<TKDBc::nlay();k++){
       par[i][j][k].setpar(outc[i][j][k],outa[i][j][k]);
     }
     }
     }

 }
   number pav=0;
   number pav2=0;
   //if(p->_flag)HBOOK1(p->_Address+p->_flag,"my distr",50,0.,2.,0.);
  for(niter=0;niter<p->_PositionData;niter++){
   integer npt=TKDBc::patpoints((p->_pData)[niter]._Pattern);
   uintl address=(p->_pData)[niter]._Address;
   integer lad[2][maxlay];
   AMSTrTrack::decodeaddress(lad,address);
   for(i=0;i<npt;i++){
     int plane=TKDBc::patconf((p->_pData)[niter]._Pattern,i)-1;
     int ladno;
     int halfno;
     if(TRALIG.LayersOnly){
       ladno=0;
       halfno=0;
     }
     else{
      ladno=lad[0][plane]-1;
      halfno=lad[1][plane];
     }
     if(!_pPargl[ladno][halfno][plane].NEntries() ){
        out[7]=1;
        goto mbreak;
     }
     for(int j=0;j<3;j++){
//       hits[i][j]=(par[ladno][halfno][plane].getcoo())[j]+(par[ladno][halfno][plane].getmtx(j)).prod((p->_pData)[niter]._Hits[i]);
       hits[i][j]=(par[ladno][halfno][plane].getcoo())[j]+((p->_pData)[niter]._CooA[i])[j]+(par[ladno][halfno][plane].getmtx(j)).prod((p->_pData)[niter]._Hits[i]-(p->_pData)[niter]._CooA[i]);
       sigma[i][j]=(par[ladno][halfno][plane].getmtx(j)).prod((p->_pData)[niter]._EHits[0]);
     }
       _pPargl[ladno][halfno][plane].setpar(((p->_pData)[niter]._CooA[i]),AMSPoint());
   }
   out[0]=p->_pData[niter]._InvRigidity;
   TKFITG(npt,hits,sigma,normal,p->_pData[niter]._Pid,ialgo,ims,layer,out);
mbreak: 
   if(out[7]==0 && out[5]!=0 ){
    if(ialgo/10 && p->_flag){
       
      geant out1[9];
      int ia=1;
      TKFITG(npt,hits,sigma,normal,p->_pData[niter]._Pid,ia,ims,layer,out1);
      out[5]=out1[5];
      geant xx=out[5]*p->_pData[niter]._InvRigidity;;
      //HF1(p->_Address+p->_flag,xx,1.);
    }
    pav+=1/out[5]/p->_pData[niter]._InvRigidity;
    pav2+=1/out[5]/out[5]/p->_pData[niter]._InvRigidity/p->_pData[niter]._InvRigidity;
    number error=out[8];
    fc+=out[6];
    npfit++;
   }
   
  }
  if(npfit < n+2)fc=FLT_MAX;
  else fc=fc/(npfit-n);
  //if(p->_flag)HPRINT(p->_Address+p->_flag);
  if(p->_flag==2 && npfit>1){
       pav/=npfit;
       pav2/=npfit;
       pav2=sqrt(fabs(pav2-pav*pav));
       p->_tmppav=pav;
       p->_tmppsi=pav2;
       for(i=0;i<6;i++)cout <<" "<<xc[i]<<" ";
       cout << endl<<" alfun out " <<fc<<" "<<npfit<<" "<<pav<<" "<<pav2<<" "<<n<<endl;
       p->_flag=0;

  }
       for(i=0;i<6;i++)cout <<" "<<xc[i]<<" ";
       cout << endl<<" alfun out " <<fc<<" "<<npfit<<" "<<pav<<" "<<pav2<<" "<<n<<endl;
      if(p->_flag==3){
       p->_tmp=fc;
       p->_flag=0;
       fool_e04ccf=fc;

      }

}
else{

// no mag field
  integer i,niter;
  fc=0;
  for(i=0;i<n;i++){
    if(fabs(xc[i])>=0.199 ){
    fc=FLT_MAX;
    return;
    }
  }
  static number fool_e04ccf=0;
  if(!p->_flag){
    int allzero=1;
    for(int k=0;k<n;k++){
     if(xc[k]!=0){
       allzero=0;
       break;
     }
    }
    if(allzero){
     fc=fool_e04ccf;
     return;
    }
  }
  const integer maxhits=10;
  static geant hits[maxhits][3];
  static geant hits1[maxhits][3];
  static geant sigma[maxhits][3];
  static geant sigma1[maxhits][3];
  static geant normal[maxhits][3];
  static integer layer[maxhits];
  integer ialgo=11;
  integer ims=0;
  geant out[40];
  for(i=0;i<TKDBc::nlay();i++){
     normal[i][0]=0;
     normal[i][1]=0;
     normal[i][2]=-1;
  }
  integer npfit=0;
  integer npfitp=0;
   // convert parameters
   AMSTrAligPar par[trconst::maxlad][2][TKDBc::nlay()];
 {
     AMSPoint outc[trconst::maxlad][2][TKDBc::nlay()];
     AMSPoint outa[trconst::maxlad][2][TKDBc::nlay()];
     for(i=0;i<trconst::maxlad;i++){
     for(int j=0;j<2;j++){
     for(int k=0;k<TKDBc::nlay();k++){
       outc[i][j][k]=0;
       outa[i][j][k]=0;
     }
     }
     }
     for(i=0;i<n;i++){
      int plane=p->_PlaneNo[i];
      int parno=p->_ParNo[i];
      int ladder=p->_LadderNo[i];
      int half=p->_HalfNo[i];
      if(parno<3)outc[ladder][half][plane][parno]=xc[i];
      else outa[ladder][half][plane][parno-3]=xc[i]*AMSDBc::pi/180.;
     }

     for(i=0;i<trconst::maxlad;i++){
     for(int j=0;j<2;j++){
     for(int k=0;k<TKDBc::nlay();k++){
       par[i][j][k].setpar(outc[i][j][k],outa[i][j][k]);
     }
     }
     }

 }
   number pav=0;
   number pav2=0;
  // if(p->_flag)HBOOK1(p->_Address+p->_flag,"my distr",50,0.,2.,0.);
  for(niter=0;niter<p->_PositionData;niter++){
   integer npt=TKDBc::patpoints((p->_pData)[niter]._Pattern);
   uintl address=(p->_pData)[niter]._Address;
   integer lad[2][maxlay];
   AMSTrTrack::decodeaddress(lad,address);
   for(i=0;i<npt;i++){
     int plane=TKDBc::patconf((p->_pData)[niter]._Pattern,i)-1;
     int ladno;
     int halfno;
     if(TRALIG.LayersOnly){
       ladno=0;
       halfno=0;
     }
     else{
      ladno=lad[0][plane]-1;
      halfno=lad[1][plane];
     }
     if(!_pPargl[ladno][halfno][plane].NEntries() ){
        out[7]=1;
        goto mbreak2;
     }
     for(int j=0;j<3;j++){
       hits[i][j]=(par[ladno][halfno][plane].getcoo())[j]+((p->_pData)[niter]._CooA[i])[j]+
       (par[ladno][halfno][plane].getmtx(j)).prod((p->_pData)[niter]._Hits[i]-(p->_pData)[niter]._CooA[i]);
       sigma[i][j]=(par[ladno][halfno][plane].getmtx(j)).prod((p->_pData)[niter]._EHits[0]);
     }
       _pPargl[ladno][halfno][plane].setpar(((p->_pData)[niter]._CooA[i]),AMSPoint());
   }
   out[0]=p->_pData[niter]._InvRigidity;
   TKFITG(npt,hits,sigma,normal,p->_pData[niter]._Pid,ialgo,ims,layer,out);
mbreak2: 
   if(out[7]==0 && (p->_Chi2Max==0 || out[6]<p->_Chi2Max)){
//    cout <<" i "<<i<<" "<<out[6]<<endl;
    fc+=out[6]*npt;
    if(p->_flag==3){
     //p->chi2[npfit]=0;
     if(npfit<sizeof(p->chi2)/sizeof(p->chi2[0]))p->chi2i[npfit]=out[6];
    }
    else{
     if(npfit<sizeof(p->chi2)/sizeof(p->chi2[0]))p->chi2[npfit]=out[6];
     //p->chi2i[npfit]=0;
    }
    npfit++;
    npfitp+=npt;
   }
   
  }
  if(npfitp < n+2)fc=FLT_MAX;
  else fc=fc/(npfitp-n);
  //if(p->_flag)HPRINT(p->_Address+p->_flag);
  if(p->_flag==2 && npfit>1){
       for(i=0;i<6;i++)cout <<" "<<xc[i]<<" ";
       cout << endl<<" alfun out " <<fc<<" "<<npfit<<" "<<endl;
       p->_flag=0;

  }
       for(i=0;i<6;i++)cout <<" "<<xc[i]<<" ";
       cout << endl<<" alfun out " <<fc<<" "<<npfit<<" "<<pav<<" "<<pav2<<" "<<n<<endl;
      if(p->_flag==3){
       p->_tmp=fc;
       p->_flag=0;
       fool_e04ccf=fc;
      }

}
}

*/

void AMSTrAligData::Init(integer pattern, uint128 address, AMSPoint hit[],AMSPoint ehit[],AMSPoint cooa[],geant mcrig){
   _Pattern=pattern;
   _Address=address;
    _InvRigidity=1/mcrig;
   _ErrInvRigidity=0.01*_InvRigidity;
   _Pid=5;
  _NHits=TKDBc::patpoints(pattern);
  _Hits= new AMSPoint[_NHits];
  _EHits=new AMSPoint[1];   // all ehits are the same, conserve memory
  _CooA=new AMSPoint[_NHits];
  for(int i=0;i<_NHits;i++){
      _Hits[i]=hit[i];
      _EHits[0]=ehit[i];
      _CooA[i]=cooa[i];
  }
  return ;
}


void AMSTrAligData::Init(AMSTrTrack *ptrack, AMSmceventg *pmcg){
//    AMSTrTrack * ptrack=ppart->getptrack();
    integer pattern=ptrack->getpattern();
   _Pattern=pattern;
   _Address=ptrack->getaddressS();
   if(pmcg)_InvRigidity=pmcg->getcharge()/pmcg->getmom();
    else _InvRigidity=0;
   _ErrInvRigidity=0.01*_InvRigidity;
   if(pmcg)_Pid=pmcg->pid();
   else _Pid=5;
  _NHits=TKDBc::patpoints(pattern);
  _Hits= new AMSPoint[_NHits];
  _EHits=new AMSPoint[1];
  _CooA=new AMSPoint[_NHits];
  for(int i=0;i<TKDBc::patpoints(pattern);i++){
    for(int j=0;j<ptrack->getnhits();j++){
     AMSTrRecHit * ph= ptrack->getphit(j);
     if (ph->getLayer() == TKDBc::patconf(pattern,i)){
      _Hits[i]=ph->getHit();
      _EHits[0]=ph->getEHit();
       AMSgvolume * psen= ph->getpsen();
        if(TRALIG.LaddersOnly){
        psen=psen->up();
        if(psen && TRALIG.LayersOnly){
          psen=psen->up();
        }
        }
        if(psen)_CooA[i]=psen->getcooA();
        else _CooA[i]=AMSPoint();
//         cout <<_CooA[i]<<endl;      
     }
    }
  }
  return ;
}

AMSID AMSTrAligPar::getTDVDB(){
return AMSID("TrAligDB",AMSJob::gethead()->isRealData());

}
AMSID AMSTrAligFit::getTDVGLDB(){
return AMSID("TrAligglDB04",AMSJob::gethead()->isRealData());

}
AMSID AMSTrAligFit::getTDVAGLDB(){
return AMSID("TrAligglADB04",AMSJob::gethead()->isRealData());

}
AMSID AMSTrAligFit::getTDVGLDBP(){
return AMSID("TrAligglDBP",AMSJob::gethead()->isRealData());

}
AMSID AMSTrAligFit::getTDVAGLDBP(){
return AMSID("TrAligglADBP",AMSJob::gethead()->isRealData());

}

AMSTrAligPar  AMSTrAligFit::_pPargl[trconst::maxsen][trconst::maxlad][2][trconst::maxlay];

/*
integer AMSTrAligFit::AddressOK(uintl address, integer strict){
  
  if(strict || TKDBc::patpoints(_Pattern)<5)return address==_Address;
  integer lad1[2][maxlay];
  integer lad2[2][maxlay];
  AMSTrTrack::decodeaddress(lad1,address);
   AMSTrTrack::decodeaddress(lad2,_Address);
  for(int i=0;i<TKDBc::nlay();i++){
     if(lad1[0][i]!=0){
      for(int j=0;j<2;j++){
       if(lad1[j][i]!=lad2[j][i]){
         return 0;
       }
      }
     }   
  }
  return 1;
}

*/

void AMSTrAligPar::LockDB(){

_lockunlock(1);
}



void AMSTrAligPar::UnlockDB(){

_lockunlock(0);

}


void AMSTrAligPar::_lockunlock(integer lock){

char fnam[256]="";
strcpy(fnam,AMSDATADIR.amsdatabase);
strcat(fnam,".AMSTrAligDB.lock");
ofstream ftxt;
int ntry=0;
const int maxtry=100;
again:
ftxt.open(fnam);
if(ftxt){
 ftxt <<lock<<endl;
 ftxt.close();
 cout <<"AMSTrAligPar::_lockunlock-I-lockstatus "<<lock<<endl;
}
else{
 cerr<<" AMSTrAligPar::_lockunlock-F-UnableToOpenLockFile "<<fnam<<endl;
  if(ntry++<maxtry){
   ftxt.close();
   goto again;
  }
   exit(1);
}


}

integer AMSTrAligPar::DbIsNotLocked(integer delay){
int ntry=0;
const int maxtry=255;
char fnam[256]="";
strcpy(fnam,AMSDATADIR.amsdatabase);
strcat(fnam,".AMSTrAligDB.lock");
ifstream ftxt;
again:
ftxt.open(fnam);
if(ftxt){
 int lock=1;
 ftxt >>lock;
 if(lock && ntry<maxtry){
  cerr<< "AMSTrAligPar::DbIsNotLocked-W-DBIsLocked "<<endl;
  ftxt.close();
  sleep(delay);
  ntry++;
  goto again;
 }
 else if(lock==0){
  ftxt.close();
  return 1;
 }
 else{
  cerr<<" AMSTrAligPar::DbIsNotLocked-E-TooManyTries-GoAhead"<<endl;
  ftxt.close();
  return 0;  
 }
}
else{
 cerr<<" AMSTrAligPar::DbIsNotLocked-F-UnableToOpenLockFile "<<fnam<<" "<<ntry<<endl;
 if(ntry++<maxtry){
    ftxt.close();
    sleep(delay);
    goto again;
 }
 exit(1);
 return 0;
}


}

void AMSTrAligFit::UpdateDBgl(){

if(TRALIG.LayersOnly){
     for(int i=0;i<TKDBc::nlay();i++){
       //update layer
         integer status;
         integer rgid;
         geant coo[3];
         number nrm[3][3];
         TKDBc::GetLayer(i+1,status,coo,nrm,rgid);
         AMSPoint Coo(coo);
         number nrmN[3][3];
         _pPargl[0][0][0][i].updmtx();
         int j;
         for(j=0;j<3;j++){
           for(int k=0;k<3;k++){
            AMSDir stipud_cxx=_pPargl[0][0][0][i].getmtx(j);
            nrmN[j][k]=stipud_cxx[k];
           }
         }
         for(j=0;j<3;j++){
          coo[j]=_pPargl[0][0][0][i].getcoo()[j]+(_pPargl[0][0][0][i].getmtx(j)).prod(Coo);
         }
         amsprotected::mm3(nrmN,nrm,0);
         TKDBc::SetLayer(i+1,status,coo,nrm,rgid);
      }
         TKDBc::write(4);

      // UpdateDB

     for(int i=0;i<TKDBc::nlay();i++){
         _gldb[trconst::maxsen][trconst::maxlad][0][i].nentries=-_pPargl[0][0][0][i].NEntries();
         for(int l=0;l<3;l++){
          _gldb[trconst::maxsen][trconst::maxlad][0][i].coo[l]=_pPargl[0][0][0][i].getcoo()[l];
          _gldb[trconst::maxsen][trconst::maxlad][0][i].ang[l]=_pPargl[0][0][0][i].getang()[l];
         }
        }
}
else if(TRALIG.LaddersOnly){
         integer status;
         integer rgid;
         geant coo[3];
         number nrm[3][3];
int i;
for(i=0;i<TKDBc::nlay();i++){
 integer statusy;
 integer rgidy;
 geant cooy[3];
 number nrmy[3][3];        
 TKDBc::GetLayer(i+1,statusy,cooy,nrmy,rgidy);
 number nrmyT[3][3];
 transpose(nrmy,nrmyT);
 AMSPoint Cooy(cooy);
 for(int nlad=0;nlad<trconst::maxlad;nlad++){
  for(int side=0;side<2;side++){
    if((TKDBc::activeladdshuttle(i+1,nlad+1,side))){
       TKDBc::GetLadder(i,nlad,side,status,coo,nrm,rgid);
       AMSPoint Coo(coo);
       number nrmN[3][3];
       _pPargl[0][nlad][side][i].updmtx();
       int j;
       for(j=0;j<3;j++){
           for(int k=0;k<3;k++){
            AMSDir stipud_cxx=_pPargl[0][nlad][side][i].getmtx(j);
            nrmN[j][k]=stipud_cxx[k];
           }
       }
       number nrmz0[3][3];
       number nrmz1[3][3];
       UCOPY(nrmN,nrmz0,sizeof(nrmz0)/sizeof(integer));
       amsprotected::mm3(nrmyT,nrmz0,0);
       UCOPY(nrmy,nrmz1,sizeof(nrmz0)/sizeof(integer));
       amsprotected::mm3(nrmN,nrmz1,0);
       amsprotected::mm3(nrmyT,nrmz1,0);
        
       for(j=0;j<3;j++){
          AMSDir d0(nrmyT[j][0],nrmyT[j][1],nrmyT[j][2]);
          AMSDir d1(nrmz0[j][0],nrmz0[j][1],nrmz0[j][2]);
          AMSDir d2(nrmz1[j][0],nrmz1[j][1],nrmz1[j][2]);
          coo[j]=d0.prod(_pPargl[0][nlad][side][i].getcoo()-Cooy)+d1.prod(Cooy)+d2.prod(Coo);
       }
       amsprotected::mm3(nrmy,nrm,0);
       amsprotected::mm3(nrmN,nrm,0);
       amsprotected::mm3(nrmyT,nrm,0);
       if(_pPargl[0][nlad][side][i].NEntries()>TRALIG.MinEventsPerFit){
         TKDBc::SetLadder(i,nlad,side,status,coo,nrm,rgid);
         _pPargl[0][nlad][side][i].NEntries()=-_pPargl[0][nlad][side][i].NEntries(); 
       }
       else{
        cerr<<"AMSTrAligFit::UpdateDBgl-W-NentriesTooLowParWillNotBeUpdated "<<
        _pPargl[0][nlad][side][i].NEntries()<<" "<<nlad<<" "<<side<<" "<<i<<endl;
         _pPargl[0][nlad][side][i].NEntries()=0;  
        
       }       
    }  
  }
 }
}

      // UpdateDB

     for(i=0;i<TKDBc::nlay();i++){
      int j;
      for(j=0;j<trconst::maxlad;j++){
        for(int k=0;k<2;k++){
         _gldb[trconst::maxsen][j][k][i].nentries=-_pPargl[0][j][k][i].NEntries();
         for(int l=0;l<3;l++){
          _gldb[trconst::maxsen][j][k][i].coo[l]=_pPargl[0][j][k][i].getcoo()[l];
          _gldb[trconst::maxsen][j][k][i].ang[l]=_pPargl[0][j][k][i].getang()[l];
         }
            cout <<" ijk "<<i<<" "<<j<<" "<<k<<" "<< _gldb[trconst::maxsen][j][k][i].getcoo()<<endl;
        }
      }
     }
    

    TKDBc::write(10+TRALIG.Algorithm);
}



else{
//don't want to set up geometry

/*
         integer status;
         integer rgid;
         geant coo[3];
         number nrm[3][3];
int i;
for(i=0;i<TKDBc::nlay();i++){
 integer statusy;
 integer rgidy;
 geant cooy[3];
 number nrmy[3][3];        
 TKDBc::GetLayer(i+1,statusy,cooy,nrmy,rgidy);
 number nrmyT[3][3];
 transpose(nrmy,nrmyT);
 AMSPoint Cooy(cooy);
 for(int nlad=0;nlad<trconst::maxlad;nlad++){
  for(int side=0;side<2;side++){
    if((TKDBc::activeladdshuttle(i+1,nlad+1,side))){
       TKDBc::GetLadder(i,nlad,side,status,coo,nrm,rgid);
       AMSPoint Coo(coo);
       number nrmN[3][3];
       _pPargl[0][nlad][side][i].updmtx();
       int j;
       for(j=0;j<3;j++){
           for(int k=0;k<3;k++){
            AMSDir stipud_cxx=_pPargl[0][nlad][side][i].getmtx(j);
            nrmN[j][k]=stipud_cxx[k];
           }
       }
       number nrmz0[3][3];
       number nrmz1[3][3];
       UCOPY(nrmN,nrmz0,sizeof(nrmz0)/sizeof(integer));
       amsprotected::mm3(nrmyT,nrmz0,0);
       UCOPY(nrmy,nrmz1,sizeof(nrmz0)/sizeof(integer));
       amsprotected::mm3(nrmN,nrmz1,0);
       amsprotected::mm3(nrmyT,nrmz1,0);
        
       for(j=0;j<3;j++){
          AMSDir d0(nrmyT[j][0],nrmyT[j][1],nrmyT[j][2]);
          AMSDir d1(nrmz0[j][0],nrmz0[j][1],nrmz0[j][2]);
          AMSDir d2(nrmz1[j][0],nrmz1[j][1],nrmz1[j][2]);
          coo[j]=d0.prod(_pPargl[0][nlad][side][i].getcoo()-Cooy)+d1.prod(Cooy)+d2.prod(Coo);
       }
       amsprotected::mm3(nrmy,nrm,0);
       amsprotected::mm3(nrmN,nrm,0);
       amsprotected::mm3(nrmyT,nrm,0);
       if(_pPargl[0][nlad][side][i].NEntries()>TRALIG.MinEventsPerFit){
         TKDBc::SetLadder(i,nlad,side,status,coo,nrm,rgid);
         _pPargl[0][nlad][side][i].NEntries()=-_pPargl[0][nlad][side][i].NEntries(); 
       }
       else{
        cerr<<"AMSTrAligFit::UpdateDBgl-W-NentriesTooLowParWillNotBeUpdated "<<
        _pPargl[0][nlad][side][i].NEntries()<<" "<<nlad<<" "<<side<<" "<<i<<endl;
         _pPargl[0][nlad][side][i].NEntries()=0;  
        
       }       
    }  
  }
 }
}
*/

      // UpdateDB

     for(int i=0;i<TKDBc::nlay();i++){
      int j;
      for(j=0;j<trconst::maxlad;j++){
        for(int k=0;k<2;k++){
          for(int m=0;m<trconst::maxsen;m++){
         _gldb[m][j][k][i].nentries=_pPargl[m][j][k][i].NEntries();
       if(_pPargl[m][j][k][i].NEntries()>TRALIG.MinEventsPerFit){
         _gldb[m][j][k][i].nentries=_pPargl[m][j][k][i].NEntries();
       }
       else    _gldb[m][j][k][i].nentries=0;

         for(int l=0;l<3;l++){
          _gldb[m][j][k][i].coo[l]=_pPargl[m][j][k][i].getcoo()[l];
          _gldb[m][j][k][i].ang[l]=_pPargl[m][j][k][i].getang()[l];
         }
            cout <<" ijk "<<i<<" "<<j<<" "<<k<<" "<< " "<<m<<" "<<_gldb[m][j][k][i].getcoo()<<endl;
        }
      }
     }
     }
    

}


     _gldb[trconst::maxsen][trconst::maxlad][1][0].nentries=1;

    if (AMSFFKEY.Update ){
       AMSTimeID *ptdv=AMSJob::gethead()->gettimestructure(getTDVGLDBP());
             ptdv->UpdCRC();
             time_t begin,end,insert;
            time(&insert);
             ptdv->SetTime(insert,insert-86400*365,insert+86400*1000);
             
             if( !ptdv->write(AMSDATADIR.amsdatabase)){
                cerr <<"AMSTrAligPar::updateDB-F-ProblemtoUpdate "<<*ptdv;
                exit(1);
             }
             else{
      cout <<" TrAligflDB  info has been updated for "<<*ptdv;
      ptdv->gettime(insert,begin,end);
      cout <<" Time Insert "<<ctime(&insert);
      cout <<" Time Begin "<<ctime(&begin);
      cout <<" Time End "<<ctime(&end);
             }              
    }

}










AMSTrAligPar* AMSTrAligFit::SearchAntiDBgl(AMSTrIdGeom*pid,bool anti){
if(anti){
if(!_antigldb[trconst::maxsen][trconst::maxlad][1][0].nentries)return 0;
AMSTrAligPar *alig=AMSTrAligPar::getparp();
bool lyonly=false;
bool laonly=false;
for(int i=0;i<TKDBc::nlay();i++){
if(_antigldb[trconst::maxsen][trconst::maxlad][0][i].nentries){
  lyonly=true;
  break;
}
}
for(int i=0;i<TKDBc::nlay();i++){
if(_antigldb[trconst::maxsen][0][0][i].nentries){
  laonly=true;
  break;
}
}

if(lyonly){
     int i=pid->getlayer()-1;
          (alig+i)->setpar(_antigldb[trconst::maxsen][trconst::maxlad][0][i].getcoo(),_antigldb[trconst::maxsen][trconst::maxlad][0][i].getang());
}   
else if(laonly){
    integer lad[2][TKDBc::nlay()];    
     int i=pid->getlayer()-1;
     lad[0][i]=pid->getladder();
     lad[1][i]=pid->gethalf();
          int ld=lad[0][i]-1;
          if(ld>=0){
           (alig+i)->setpar(_antigldb[trconst::maxsen][ld][lad[1][i]][i].getcoo(),_antigldb[trconst::maxsen][ld][lad[1][i]][i].getang());
          }
          else{
           (alig+i)->setpar(AMSPoint(),AMSPoint());
          }
}
else{
    integer lad[2][TKDBc::nlay()];    
     int i=pid->getlayer()-1;
     lad[0][i]=pid->getladder()-1;
     lad[1][i]=pid->gethalf();
     lad[2][i]=pid->getsensorR()-1;
          if(lad[0][i]>=0){
           (alig+i)->setpar(_antigldb[lad[2][i]][lad[0][i]][lad[1][i]][i].getcoo(),_antigldb[lad[2][i]][lad[0][i]][lad[1][i]][i].getang());
          }
          else{
           (alig+i)->setpar(AMSPoint(),AMSPoint());
          }
}

return alig;
}
else{
if(!_gldb[trconst::maxsen][trconst::maxlad][1][0].nentries)return 0;
AMSTrAligPar *alig=AMSTrAligPar::getparp();
bool lyonly=false;
bool laonly=false;
for(int i=0;i<TKDBc::nlay();i++){
if(_gldb[trconst::maxsen][trconst::maxlad][0][i].nentries){
  lyonly=true;
  break;
}
}
for(int i=0;i<TKDBc::nlay();i++){
if(_gldb[trconst::maxsen][0][0][i].nentries){
  laonly=true;
  break;
}
}

if(lyonly){
     int i=pid->getlayer()-1;
          (alig+i)->setpar(_gldb[trconst::maxsen][trconst::maxlad][0][i].getcoo(),_gldb[trconst::maxsen][trconst::maxlad][0][i].getang());
}   
else if(laonly){
    integer lad[2][TKDBc::nlay()];    
     int i=pid->getlayer()-1;
     lad[0][i]=pid->getladder();
     lad[1][i]=pid->gethalf();
          int ld=lad[0][i]-1;
          if(ld>=0){
           (alig+i)->setpar(_gldb[trconst::maxsen][ld][lad[1][i]][i].getcoo(),_gldb[trconst::maxsen][ld][lad[1][i]][i].getang());
          }
          else{
           (alig+i)->setpar(AMSPoint(),AMSPoint());
          }
}
else{
    integer lad[2][TKDBc::nlay()];    
     int i=pid->getlayer()-1;
     lad[0][i]=pid->getladder()-1;
     lad[1][i]=pid->gethalf();
     lad[2][i]=pid->getsensorR()-1;
          if(lad[0][i]>=0){
           (alig+i)->setpar(_gldb[lad[2][i]][lad[0][i]][lad[1][i]][i].getcoo(),_gldb[lad[2][i]][lad[0][i]][lad[1][i]][i].getang());
          }
          else{
           (alig+i)->setpar(AMSPoint(),AMSPoint());
          }
}
    return alig;
}
}



const char * AMSTrAligFit::GetAligString(){
 static AString w;
 w="";
 char string[1025];
    for(int m=0;m<trconst::maxsen+1;m++){   
  for(int i=0;i<trconst::maxlad+1;i++){
   for(int j=0;j<2;j++){
    for(int k=0;k<TKDBc::nlay();k++){   
       sprintf(string,"%d %d %d %d %d %15.7g %15.7g %15.7g %15.7g %15.7g %15.7g \n",m,i,j,k, _gldb[m][i][j][k].nentries,_gldb[m][i][j][k].coo[0],_gldb[m][i][j][k].coo[1],_gldb[m][i][j][k].coo[2],_gldb[m][i][j][k].ang[0],_gldb[m][i][j][k].ang[1],_gldb[m][i][j][k].ang[2]);
  w+=string;
 }
}
}
}
return (const char*)w;
}


void AMSTrAligFit::RebuildNoActivePar(){
if(TRALIG.LayersOnly){

 _NoActivePar=0;
 for(int i=0;i<TKDBc::nlay();i++){
  int ntr=0;
  for(int l=0;l<trconst::maxlad;l++){
  for(int k=0;k<trconst::maxsen;k++){
   for(int m=0;m<2;m++){
    if(_pPargl[k][l][m][i].NEntries()>0)ntr+=_pPargl[k][l][m][i].NEntries();
   }
  }
  }
  if(ntr>TRALIG.MinEventsPerFit/2){
  _pPargl[0][0][0][i].NEntries()=ntr;
  int nprp=0;
   for(int j=0;j<6;j++){ 
    if(TRALIG.ActiveParameters[i][j]){
      _PlaneNo[_NoActivePar+nprp]=i;
      _ParNo[_NoActivePar+nprp]=j;
      _LadderNo[_NoActivePar+nprp]=0;
      _HalfNo[_NoActivePar+nprp]=0;
      nprp++;
    }
   }
     _NoActivePar+=nprp;
   }
   else {
      cout <<" AMSTrAligFit::RebuildNoActivePar-I-TooFewEvents "<<ntr<<" "<<i<<endl;
      _pPargl[0][0][0][i].NEntries()=0;
   }
 }


}
else{
 _NoActivePar=0;
 for(int i=0;i<TKDBc::nlay();i++){
  for(int l=0;l<trconst::maxlad;l++){
  for(int k=0;k<trconst::maxsen;k++){
   for(int m=0;m<2;m++){
        bool bok=m==0?TKDBc::nhalf(i+1,l+1)>0 :TKDBc::nhalf(i+1,l+1)<TKDBc::nsen(i+1,l+1);


    if(_pPargl[k][l][m][i].NEntries()>TRALIG.MinEventsPerFit/2){
     //cout <<"GLBAL "<<k<<" "<<l<<" "<<m<<" "<<i<<" "<<_pPargl[k][l][m][i].NEntries()<<endl;
     int nprp=0;
     for(int j=0;j<6;j++){ 
      if(TRALIG.ActiveParameters[i][j]){
        _PlaneNo[_NoActivePar+nprp]=i;
        _ParNo[_NoActivePar+nprp]=j;
        _LadderNo[_NoActivePar+nprp]=l;
        _HalfNo[_NoActivePar+nprp]=m;
        nprp++;
      }
     }
     _NoActivePar+=nprp;
    }
    else if(_pPargl[k][l][m][i].NEntries()>=0 && TKDBc::activeladdshuttle(i+1,l+1,m)&& bok){
      if(k==0)cout <<" AMSTrAligFit::RebuildNoActivePar-I-TooFewEvents "<<_pPargl[k][l][m][i].NEntries()<<" "<<l<<" "<<m << " " <<i<<endl;
      _pPargl[k][l][m][i].NEntries()=0;
    }
   }
  }
  }
 }
}
cout <<"GlobalFit-I-NoActivePar "<<_NoActivePar<<endl;

}

void AMSTrAligFit::InitADB(){
  static bool initdbdone=false;
  if(!initdbdone){
  for(int i=0;i<trconst::maxlad+1;i++){
   for(int j=0;j<2;j++){
    for(int k=0;k<TKDBc::nlay();k++){   
     for(int m=0;m<trconst::maxsen+1;m++){
       _antigldb[m][i][j][k].nentries=0;
         for(int l=0;l<3;l++){
          _antigldb[m][i][j][k].coo[l]=0;
          _antigldb[m][i][j][k].ang[l]=0;
         }
         }
    }
   }
  }
  initdbdone=true;
 }
}
void AMSTrAligFit::InitDB(){
  static bool initdbdone=false;
  if(!initdbdone){
  for(int i=0;i<trconst::maxlad+1;i++){
   for(int j=0;j<2;j++){
    for(int k=0;k<TKDBc::nlay();k++){   
     for(int m=0;m<trconst::maxsen+1;m++){
       _gldb[m][i][j][k].nentries=0;
         for(int l=0;l<3;l++){
          _gldb[m][i][j][k].coo[l]=0;
          _gldb[m][i][j][k].ang[l]=0;
         }
    }
    }
   }
  }
  initdbdone=true;
 }
}

AMSTrAligFit::gldb_def AMSTrAligFit::_gldb[trconst::maxsen+1][trconst::maxlad+1][2][maxlay];
AMSTrAligFit::gldb_def AMSTrAligFit::_antigldb[trconst::maxsen+1][trconst::maxlad+1][2][maxlay];

AMSTrAligFit::gldb_def AMSTrAligFit::_gldbP[trconst::maxsen+1][trconst::maxlad+1][2][maxlay];
AMSTrAligFit::gldb_def AMSTrAligFit::_antigldbP[trconst::maxsen+1][trconst::maxlad+1][2][maxlay];



integer AMSTrAligPar::AddOne(){

if(_NEntries>=0)_NEntries++;

if(_NEntries<TRALIG.SingleLadderEntryLimit)return _NEntries>=0;
else return 0;

}

void AMSTrAligPar::MinusOne(){

if(_NEntries>0)_NEntries--;


}

void AMSTrAligFit::rewrite(){
for(int i=0;i<trconst::maxsen+1;i++){
  for(int j=0;j<trconst::maxlad+1;j++){
   for(int k=0;k<2;k++){
    for(int l=0;l<trconst::maxlay-1;l++){
       _gldbP[i][j][k][l]=_gldb[i][j][k][l];
       _antigldbP[i][j][k][l]=_antigldb[i][j][k][l];
    }
   }
  }
 }
}
