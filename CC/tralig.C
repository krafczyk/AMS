#include <tralig.h>
#include <event.h>
#include <math.h>
#include <timeid.h>
#include <trrawcluster.h>
#include <float.h>
#include <mccluster.h>
#include <tkdbc.h>
#include <amsstl.h>
#include <event.h>
//PROTOCCALLSFSUB15(E04CCF,e04ccf,INT,DOUBLEV,DOUBLE,DOUBLE,INT,DOUBLEV,DOUBLEV,DOUBLEV,DOUBLEV,DOUBLEV,DOUBLEV,ROUTINE,ROUTINE,INT,INT)
//#define E04CCF(N,X,F,TOL,IW,W1,W2,W3,W4,W5,W6,ALFUN1,MONIT,MAXCAL,IFAIL) CCALLSFSUB15(E04CCF,e04ccf,INT,DOUBLEV,DOUBLE,DOUBLE,INT,DOUBLEV,DOUBLEV,DOUBLEV,DOUBLEV,DOUBLEV,DOUBLEV,ROUTINE,ROUTINE,INT,INT,N,X,F,TOL,IW,W1,W2,W3,W4,W5,W6,ALFUN1,MONIT,MAXCAL,IFAIL)

extern "C" void e04ccf_(int &n, number x[], number &f, number &tol, int &iw, number w1[],number w2[], number w3[], number w4[], number w5[], number w6[],void * alfun, void * monit, int & maxcal, int &ifail, void * p);

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
_Coo=coo;
_Angles=angles;
// construct rot matrix
_a2m();
}



AMSTrAligPar * AMSTrAligPar::SearchDB(uinteger address, integer & found){
  AMSTrAligPar::AMSTrAligDBEntry e(address);
  integer out=AMSbins(getdbtopp(),e,getdbentries());
  if(out >0){
    found=1;
    for(int i=0;i<6;i++){
     *(getparp()+i)=AMSTrAligPar(getdbtopp()+out-1,i);
    }
    return getparp();
  }
  else {
    found=0;
    return 0;
  }
}

AMSTrAligPar::AMSTrAligPar(const AMSTrAligDBEntry * db, integer i){
if(i>=0 && i<6){
  _Coo=AMSPoint (db->coo[i][0],db->coo[i][1],db->coo[i][2]);
  _Angles=AMSPoint (db->angle[i][0],db->angle[i][1],db->angle[i][2]);
  _a2m();
}
else cerr <<"AMSTrAligPar::AMSTrAligPar-S-Wrong plane "<<i<<endl;
}




integer AMSTrAligPar::UpdateDB(uinteger address,  AMSTrAligPar  o[]){
   

   if (getdbentries()>= maxdbentries()){
     cerr<<"AMSTrAligPar::UpdateDB-E-DBIsFull "<<endl;
     return 0;
   }
   else{
    AMSTrAligPar::AMSTrAligDBEntry e(address,o);
    int out=AMSbins(getdbtopp(),e, getdbentries());
    if(out>0 && TRALIG.ReWriteDB==0){
     cerr<<"AMSTrAligPar::UpdateDB-E-ObjectAlreadyExists "<<address<<" "<<getdbentries()<<endl;
     return 0;
    }
    else if(out<=0){
     *(getdbtopp()+getdbentries())=e;
     incdbentries();
     AMSsortNAGa(getdbtopp(),getdbentries());
     cout<<"AMSTrAligPar::UpdateDB-I-NewEntryAdded "<<address<<" "<<getdbentries()<<endl;
     
    }
    else{
     *(getdbtopp()+out-1)=e;
     cout<<"AMSTrAligPar::UpdateDB-I-EntryReplaced"<<address<<" "<<getdbentries()<<endl;
    }
   }
   AMSTimeID *ptdv=AMSJob::gethead()->gettimestructure(getTDVDB());
   ptdv->UpdateMe()=1;
   return 1;
}


// Here AMSTrAligFit

AMSTrAligFit::AMSTrAligFit():_Address(0),_Pattern(0),_NData(0),_PositionData(0),
_pData(0),AMSNode(AMSID("AMSTrAligFit",0)){
  int i;
  for(i=0;i<6;i++){
   _pParC[i]=AMSTrAligPar();
  }
}

AMSTrAligFit::AMSTrAligFit(uinteger address, integer pattern, integer ndatmax,
          integer alg, integer nodeno):_Address(address),_Pattern(pattern),
          _NData(ndatmax),_Algorithm(alg),_PositionData(0),AMSNode(AMSID("AMSTrAligFit",nodeno)){
          _pData=new AMSTrAligData[ndatmax];  
            assert(_pData !=  NULL);             
  for(int i=0;i<6;i++){
   _pParC[i]=AMSTrAligPar();
  }
_NoActivePar=0;
for(i=0;i<6;i++){
 for(int k=1;k<6;k++){
 if(i==AMSTrTrack::patconf[pattern][k]-1){
  int nprp=0;
  for(int j=0;j<6;j++){
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

}


AMSTrAligFit::~AMSTrAligFit(){
 delete [] _pData;
}

void AMSTrAligFit::Test(integer forced){

AMSNode *pnode =AMSJob::gethead()->getaligstructure();
AMSTrAligFit * pal = (AMSTrAligFit *)pnode->down();

if(forced==0){
 AMSParticle * ptr=0;
 AMSmceventg * ptrg=0;
  if(Select(ptr,ptrg,TRALIG.Algorithm)){
     AMSTrTrack * ptrack= ptr->getptrack();
     integer nnodes=0;
      while(pal){
        nnodes++;
        if(pal->getaddress()==ptrack->getaddress()){
           if(pal->_PositionData<pal->_NData)(pal->_pData[(pal->_PositionData)++]).Init(ptr,ptrg);
           return;
        }
        pal=pal->next(); 
     }
      if(nnodes<TRALIG.MaxPatternsPerJob){
           integer found=0;
           AMSTrAligPar::SearchDB(ptrack->getaddress(),found);
           if(TRALIG.ReWriteDB  || !found){
            pal=new AMSTrAligFit(ptrack->getaddress(),
            ptrack->getpattern(), TRALIG.MaxEventsPerFit, TRALIG.Algorithm, nnodes+1);
            if(pal->_PositionData<pal->_NData)(pal->_pData[(pal->_PositionData)++]).Init(ptr,ptrg);
            pnode->add(pal);       
            cout <<"AMSTrAligFit::Test-I-NewPatternAdded " <<ptrack->getaddress()<<endl;
           }
      else cerr<<"AMSTrAligFit::Test-E-CouldNotAddPatternAlrdyExists " <<ptrack->getaddress()<<endl;
      }
      else cerr<<"AMSTrAligFit::Test-E-CouldNotAddPatternMaxReached " <<ptrack->getaddress()<<endl;
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

  // UpdateDB

    if (AMSFFKEY.Update ){
     AMSTimeID * offspring =
     (AMSTimeID*)((AMSJob::gethead()->gettimestructure())->down());
     while(offspring){
       if(offspring->UpdateMe()){
            cout << " Starting to update "<<*offspring;
            offspring->UpdCRC();
            if( !offspring->write(AMSDATADIR.amsdatabase))
             cerr <<"AMSJob::_dbendjob-S-ProblemtoUpdate "<<*offspring;
       }
             offspring=(AMSTimeID*)offspring->next();
     }
    }



}
}

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
    const integer mp=200;
    static number f,x[mp],w1[mp],w2[mp],w3[mp],w4[mp],w5[mp+1],w6[2000];
  // number of parameters to fit
    integer npt=AMSTrTrack::patpoints[_Pattern];
    integer n=_NoActivePar;
    integer iw=n+1;
    integer ifail=1;
    integer maxcal=1000;
    number tol=2.5e-2;
    int i,j;
    for(i=0;i<mp;i++)x[i]=0;
    _flag=3;
    e04ccf_(n,x,f,tol,iw,w1,w2,w3,w4,w5,w6,palfun,pmonit,maxcal,ifail,this);
    cout << "AMSTrAligFit::Fit finished "<<ifail<<" "<<f<<endl;
    if(ifail ==0 || ifail==2){
     _flag=2;
     ifail=1;
     number fd;
     integer one(1);
     e04ccf_(n,x,fd,tol,iw,w1,w2,w3,w4,w5,w6,palfun,pmonit,one,ifail,this);
     AMSPoint outc[6];
     AMSPoint outa[6];
     for(i=0;i<6;i++){
       outc[i]=0;
       outa[i]=0;
     }
     for(i=0;i<n;i++){
      int plane=_PlaneNo[i];
      int parno=_ParNo[i];
      if(parno<3)outc[plane][parno]=x[i];
      else outa[plane][parno-3]=x[i]*AMSDBc::pi/180.;
     }

     for(i=0;i<6;i++){
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

AMSTrAligPar::AMSTrAligDB AMSTrAligPar::_traldb;
AMSTrAligPar   AMSTrAligPar::par[6];



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
    HROPEN(IOPA.hlun+1,"tralig",filename,"N",rsize,iostat);
    if(iostat){
     cerr << "Error opening tralig ntuple file "<<filename<<endl;
    }
    else cout <<"tralig ntuple file "<<filename<<" opened."<<endl;

   HBNT(IOPA.ntuple+1,"Tracker Alignment"," ");
   HBNAME(IOPA.ntuple+1,"TrAlig",(int*)(&TRALIGG),"Pattern:I,Alg:I,Address:I,FCN:R,FCNI:R,PFIT:R,PFITS:R,Coo(3,6):R,Angle(3,6):R");

  }
  int i,j,k,l;
      TRALIGG.Pattern=_Pattern;
      TRALIGG.Alg=_Algorithm;
      TRALIGG.Address=_Address;
      TRALIGG.FCN=_fcn;
      TRALIGG.FCNI=_fcnI;
      TRALIGG.Pfit=_pfit;
      TRALIGG.Pfitsig=_pfits;
    for(j=0;j<6;j++){
      for(k=0;k<3;k++){
       TRALIGG.Coo[j][k]=_pParC[j].getcoo()[k];
       TRALIGG.Angle[j][k]=_pParC[j].getang()[k];
      }
    }
    HFNT(IOPA.ntuple+1);

   // updatedb
   AMSTrAligPar::UpdateDB(_Address,_pParC);


}


integer AMSTrAligFit::Select(AMSParticle * & ppart, AMSmceventg * & pmcg, integer alg){
      ppart=(AMSParticle*)AMSEvent::gethead()->getheadC("AMSParticle",0);
      pmcg=0;
      if(!ppart)return 0;      // Particle Should be Present always

  // Algorithm == 0 presumes momentum is knows,
  // pmcg may exist, if not create it from datacards
   if(alg==0){ 
         pmcg=(AMSmceventg*)AMSEvent::gethead()->getheadC("AMSmceventg",0);  
         if(!pmcg){
             AMSPoint coo(CCFFKEY.coo[0],CCFFKEY.coo[1],CCFFKEY.coo[2]);
             AMSDir dir(CCFFKEY.dir[0],CCFFKEY.dir[2],CCFFKEY.dir[2]);

             pmcg=new AMSmceventg(GCKINE.ipart, CCFFKEY.momr[0], coo, dir);  
              AMSEvent::gethead()->addnext(AMSID("AMSmceventg",0), pmcg);   
         }
   }

   else if(alg==1){
// Algorithm == 1 presumes getting momentum from beta measurement & from gckine.ipart
 //    always create pmcg

      integer pid=ppart->getgpart();
      AMSBeta * pbeta=ppart->getpbeta();
      number beta=pbeta->getbeta();
       if(fabs(beta)<1. && pid==GCKINE.ipart){
         AMSPoint coo=ppart->gettofcoo(0);
         AMSDir dir(ppart->gettheta(),ppart->getphi());
         geant xmom=0;   
         pmcg= new AMSmceventg(GCKINE.ipart, xmom, coo, dir);  
         number gamma=1./sqrt(1-beta*beta);
         xmom=pmcg->getmass()*fabs(beta)*gamma;
         pmcg->setmom(xmom);
        AMSEvent::gethead()->addnext(AMSID("AMSmceventg",0), pmcg);  
       }
   }
   else{
     cerr<<"AMSTrAligFit::Select-F-AlgNo "<<alg<<" IsNotSupportedYet"<<endl;
      exit(1);
   }
   // Apply Cuts   
   if(pmcg){
      if(alg==0){

        number charge=pmcg->getcharge();
        number momentum=pmcg->getmom();
        AMSDir dir=pmcg->getdir();
        number recharge=ppart->getcharge();
        if(ppart->getmomentum()<0)recharge=-recharge;
        if(recharge==charge){
           AMSTrTrack *ptr=ppart->getptrack();              
           number chi2xy, chi2z, rid;
           ptr->getParSimpleFit(chi2xy,chi2z,rid);
           number cth=cos(ppart->gettheta())*cos(dir.gettheta())+
           sin(ppart->gettheta())*sin(dir.gettheta())*cos(ppart->getphi()-dir.getphi());
                   if(chi2xy<TRALIG.Cuts[0][1] && cth > TRALIG.Cuts[1][0] &&
                      fabs(ppart->getmomentum())>momentum*TRALIG.Cuts[2][0] &&
                      fabs(ppart->getmomentum())<momentum*TRALIG.Cuts[2][1]  ){
                      return 1;
                    }
        }
      }
      else {
       return 1;
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
  integer npt=AMSTrTrack::patpoints[p->_Pattern];
  const integer maxhits=10;
  static geant hits[maxhits][3];
  static geant hits1[maxhits][3];
  static geant sigma[maxhits][3];
  static geant sigma1[maxhits][3];
  static geant normal[maxhits][3];
  integer ialgo=11;
  integer ims=0;
  geant out[9];
  for(i=0;i<npt;i++){
     normal[i][0]=0;
     normal[i][1]=0;
     normal[i][2]=-1;
  }
  integer npfit=0;
   // convert parameters
   AMSTrAligPar par[6];
 {
     AMSPoint outc[6];
     AMSPoint outa[6];
     for(i=0;i<6;i++){
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
   if(p->_flag)HBOOK1(101,"my distr",50,0.,2.,0.);
  for(niter=0;niter<p->_PositionData;niter++){
   for(i=0;i<npt;i++){
     int plane=AMSTrTrack::patconf[p->_Pattern][i]-1;
     for(int j=0;j<3;j++){
       hits[i][j]=(par[plane].getcoo())[j]+
       (par[plane].getmtx(j)).prod((p->_pData)[niter]._Hits[i]);
       sigma[i][j]=(par[plane].getmtx(j)).prod((p->_pData)[niter]._EHits[i]);
     }
   }
   out[0]=p->_pData[niter]._InvRigidity;
   TKFITG(npt,hits,sigma,normal,p->_pData[niter]._Pid,ialgo,ims,out);
   if(out[7]==0 && out[5]!=0 ){
    if(ialgo/10 && p->_flag){
      geant out1[9];
      int ia=1;
      TKFITG(npt,hits,sigma,normal,p->_pData[niter]._Pid,ia,ims,out1);
      out[5]=out1[5];
      geant xx=out[5]*p->_pData[niter]._InvRigidity;;
      HF1(101,xx,1.);
    }
    pav+=out[5]*p->_pData[niter]._InvRigidity;
    pav2+=out[5]*out[5]*p->_pData[niter]._InvRigidity*p->_pData[niter]._InvRigidity;
    const number pm2=0.88;
    number energy=sqrt(out[5]*out[5]+pm2);
    number error=out[8];
    fc+=out[6];
    npfit++;
   }
  }
  if(npfit < n+2)fc=FLT_MAX;
  else fc=fc/(npfit-n);
  if(p->_flag)HPRINT(101);
  if(p->_flag==2 && npfit>1){
       pav/=npfit;
       pav2/=npfit;
       pav2=sqrt(fabs(pav2-pav*pav));
       p->_tmppav=pav;
       p->_tmppsi=pav2;
       for(i=0;i<6;i++)cout <<" "<<xc[i]<<" ";
       cout << endl<<" alfun out " <<fc<<" "<<npfit<<" "<<pav<<" "<<pav2<<endl;
       p->_flag=0;
  }
      if(p->_flag==3){
       for(i=0;i<6;i++)cout <<" "<<xc[i]<<" ";
       cout << endl<<" alfun out " <<fc<<" "<<npfit<<" "<<pav<<" "<<pav2<<endl;
       p->_tmp=fc;
       p->_flag=0;
      }
}



void AMSTrAligData::Init(AMSParticle *ppart, AMSmceventg *pmcg){
    AMSTrTrack * ptrack=ppart->getptrack();
    integer pattern=ptrack->getpattern();
   _InvRigidity=pmcg->getcharge()/pmcg->getmom();
   _ErrInvRigidity=0.01*_InvRigidity;
   _Pid=pmcg->getpid();
  _NHits=AMSTrTrack::patpoints[pattern];
  _Hits= new AMSPoint[_NHits];
  _EHits=new AMSPoint[_NHits];
  for(int i=0;i<AMSTrTrack::patpoints[pattern];i++){
    for(int j=0;j<ptrack->getnhits();j++){
     AMSTrRecHit * ph= ptrack->getphit(j);
     if (ph->getLayer() == AMSTrTrack::patconf[pattern][i]){
      _Hits[i]=ph->getHit();
      _EHits[i]=ph->getEHit();
     }
    }
  }

  return ;
}

AMSID AMSTrAligPar::getTDVDB(){
return AMSID("TrAligDB",AMSJob::gethead()->isRealData());

}