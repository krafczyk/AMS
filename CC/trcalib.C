#include <trcalib.h>
#include <event.h>
#include <math.h>
#include <timeid.h>
#include <trrawcluster.h>
#include <float.h>
//PROTOCCALLSFSUB15(E04CCF,e04ccf,INT,DOUBLEV,DOUBLE,DOUBLE,INT,DOUBLEV,DOUBLEV,DOUBLEV,DOUBLEV,DOUBLEV,DOUBLEV,ROUTINE,ROUTINE,INT,INT)
//#define E04CCF(N,X,F,TOL,IW,W1,W2,W3,W4,W5,W6,ALFUN1,MONIT,MAXCAL,IFAIL) CCALLSFSUB15(E04CCF,e04ccf,INT,DOUBLEV,DOUBLE,DOUBLE,INT,DOUBLEV,DOUBLEV,DOUBLEV,DOUBLEV,DOUBLEV,DOUBLEV,ROUTINE,ROUTINE,INT,INT,N,X,F,TOL,IW,W1,W2,W3,W4,W5,W6,ALFUN1,MONIT,MAXCAL,IFAIL)

extern "C" void e04ccf_(int &n, number x[], number &f, number &tol, int &iw, number w1[],number w2[], number w3[], number w4[], number w5[], number w6[],void * alfun, void * monit, int & maxcal, int &ifail, void * p);


AMSTrCalibPar::AMSTrCalibPar(AMSPoint coo, AMSDir dir[3]):_Coo(coo){
 int i;
 for(i=0;i<3;i++)_Dir[i]=dir[i];
}


AMSTrCalibPar::AMSTrCalibPar(AMSPoint coo, number nrm[3][3]):_Coo(coo){
 int i,j;
 for(i=0;i<3;i++)
  for(j=0;j<3;j++)_Dir[i][j]=nrm[i][j];
}

AMSTrCalibPar::AMSTrCalibPar(const AMSPoint & coo, const AMSDir & dir1, 
                             const AMSDir & dir2, const AMSDir & dir3){
 _Coo=coo;
 _Dir[0]=dir1;
 _Dir[1]=dir2;
 _Dir[2]=dir3;
}


integer AMSTrCalibData::Init(AMSBeta *pbeta, AMSTrTrack * ptrack, 
integer pattern, integer alg){
// check if patterns match
  if(patpoints[alg][pattern]==0)return 0;
  int i,j;
  int k=0;
  for(i=0;i<patpoints[alg][pattern];i++){
    for(j=0;j<ptrack->getnhits();j++){
      //AMSTrRecHit * ph= ptrack->getHitP(j);
     AMSTrRecHit * ph= ptrack->getphit(j);
     if (ph->getLayer() == patconf[alg][pattern][i])k++;
    }
  }
  if(k !=patpoints[alg][pattern])return 0;
  number beta=pbeta->getbeta();
   _ErrInvBeta=pbeta->getebeta();
   _InvBeta=1/beta;
  if(beta < 1){
   const number pmass=0.938;
   number gamma=1./sqrt(1.-beta*beta);
   _InvMomentum=1/beta/gamma/pmass;
   _ErrInvMomentum=gamma*_ErrInvBeta/pmass;
  }
  else{
   _InvMomentum=0;
   _ErrInvMomentum=0;
  }
  _NHits=patpoints[alg][pattern];
  _Hits= new AMSPoint[_NHits];
  _EHits=new AMSPoint[_NHits];
  for(i=0;i<patpoints[alg][pattern];i++){
    for(j=0;j<ptrack->getnhits();j++){
      //- ak (see trrec.h)     AMSTrRecHit * ph= ptrack->getHitP(j);
     AMSTrRecHit * ph= ptrack->getphit(j);
     if (ph->getLayer() == patconf[alg][pattern][i]){
      _Hits[i]=ph->getHit();
      _EHits[i]=ph->getEHit();

     }
    }
  }

  return 1;
}
integer  AMSTrCalibData::Select(integer alg, integer pattern){
  if(alg >=0 && alg <2){
    AMSContainer * pcnt;
    int i,npart,ncharge;
    int nbeta=0; 
    geant nhits=0;
    for(i=0;;i++){
      pcnt=AMSEvent::gethead()->getC("AMSBeta",i);
      if(pcnt)nbeta+=pcnt->getnelem();
      else break;
    }
    for(i=0;;i++){
      pcnt=AMSEvent::gethead()->getC("AMSTrCluster",i);
      if(pcnt)nhits+=pcnt->getnelem();
      else break;
    }
      pcnt=AMSEvent::gethead()->getC("AMSCharge",0);
      ncharge=pcnt->getnelem();
      pcnt=AMSEvent::gethead()->getC("AMSParticle",0);
      npart=pcnt->getnelem();
      if(npart ==1 && ncharge==1 && nbeta==1){
       AMSParticle *ppart=(AMSParticle*)AMSEvent::gethead()->
       getheadC("AMSParticle",0);
       if(ppart){
         if(ppart->getcharge() == 1.){
           AMSBeta * pbeta=ppart->getpbeta();
           number beta=pbeta->getbeta();
           if(beta > TRCALIB.BetaCut[alg][0] &&              
              beta < TRCALIB.BetaCut[alg][1]){
            AMSTrTrack * ptrack=ppart->getptrack();
            if(nhits/ptrack->getnhits() < TRCALIB.HitsRatioCut[alg]){
              if(Init(pbeta,ptrack,pattern,alg)){
               AMSEvent::gethead()->addnext(AMSID("AMSTrCalibration",0),
               new AMSTrCalibration(pattern));
               return 1;
              }
            }
           }
         }
       }
      }
  }
  return 0;
}

  integer AMSTrCalibData::patpoints[2][tkcalpat]={3,3,3,4,4,
                                                  3,3,3,3,0};
  integer AMSTrCalibData::patconf[2][tkcalpat][6]={
                                                 1,2,4,0,0,0,
                                                 2,3,5,0,0,0,
                                                 3,4,6,0,0,0,
                                                 1,2,4,5,0,0,
                                                 2,3,5,6,0,0,
                                                 1,2,3,0,0,0,
                                                 2,3,4,0,0,0,
                                                 3,4,5,0,0,0,
                                                 4,5,6,0,0,0,
                                                 0,0,0,0,0,0
  };


  AMSTrCalibFit * AMSTrCalibFit::_pCalFit[2][tkcalpat]={0,0,0,0,0,
                                                        0,0,0,0,0
  };




void AMSTrCalibFit::setHead(integer alg, integer pat, AMSTrCalibFit *ptr){
  if(alg>=0 && alg < 2 && pat >=0 && pat < tkcalpat)
  _pCalFit[alg][pat]=ptr;
}  
AMSTrCalibFit::AMSTrCalibFit():_pData(0),_PositionData(0),_Pattern(0),_pParM(0),_NSen(0),_pParS(0),
  _NData(0),_NIter(0),_PositionIter(0){
  int i;
  for(i=0;i<6;i++){
   _pParC[i]=0;
  }
}


AMSTrCalibFit::AMSTrCalibFit(integer pattern, integer data, integer iter, integer alg):
_PositionData(0),_PositionIter(0){
_Pattern=pattern;
_NData=data;
_NIter=iter;
assert(data > 0 && iter >0);
_pData= new AMSTrCalibData[data];
assert(_pData !=  NULL);
_Algorithm = alg;

integer npt=AMSTrCalibData::patpoints[alg][pattern];
int i,j;
for(i=0;i<npt;i++){
 _pParC[i]= new AMSTrCalibPar[_NIter];
}
for(i=0;i<npt;i++)
  for(j=1;j<=AMSDBc::nlad(AMSTrCalibData::patconf[alg][pattern][i]);j++)
   _NSen+=AMSDBc::nsen(AMSTrCalibData::patconf[alg][pattern][i],j);
_pParM=new AMSTrCalibPar[_NSen];
_pParS=new AMSTrCalibPar[_NSen];
}

AMSTrCalibFit::~AMSTrCalibFit(){
 delete [] _pData;
 delete [] _pParM;
 delete [] _pParS;
  int i;
integer npt=AMSTrCalibData::patpoints[_Algorithm][_Pattern];
  for(i=0;i<npt;i++){
   delete [] _pParC[i];
  }
}

integer AMSTrCalibFit::Test(){
if(_pData[_PositionData].Select(_Algorithm, _Pattern))_PositionData++;
if(_PositionData >= _NData){
 _PositionData=0;
 return 1;
}
else return 0;
}

AMSTrCalibPar * AMSTrCalibFit::getparM(integer layer, integer ladder,
                               integer sensor){
#ifdef __AMSDEBUG__
  assert( sensor > 0 && sensor <=AMSDBc::nsen(layer,ladder));
#endif 
int i,j;
integer address=0;
for(i=1;i<layer;i++)
  for(j=1;j<=AMSDBc::nlad(i);j++) address+=AMSDBc::nsen(i,j);
  for(j=1;j<ladder;j++) address+=AMSDBc::nsen(layer,j);
  address+=sensor-1;
  return _pParM+address;
}


AMSTrCalibPar * AMSTrCalibFit::getparS(integer layer, integer ladder,
                               integer sensor){
#ifdef __AMSDEBUG__
  assert( sensor > 0 && sensor <=AMSDBc::nsen(layer,ladder));
#endif 
int i,j,k,l;
integer address=0;
for(i=1;i<layer;i++)
  for(j=1;j<=AMSDBc::nlad(i);j++) address+=AMSDBc::nsen(i,j);
  for(j=1;j<ladder;j++) address+=AMSDBc::nsen(layer,j);
  address+=sensor-1;
  return _pParS+address;
}

void AMSTrCalibFit::Fit(){
    void (*palfun)(int &n, double x[], double &f, AMSTrCalibFit *p)=&AMSTrCalibFit::alfun;
    void (*pmonit)(number &a, number &b, number sim[], int &n, int &s, int &nca)=
                                &AMSTrCalibFit::monit;
      cout << "AMSTrCalibFit::Fit started for pattern "<<_Pattern<<" "<<_Algorithm<<" "<<    _PositionIter<<endl;
  // Fit Here
    const integer mp=200;
    static number f,x[mp],w1[mp],w2[mp],w3[mp],w4[mp],w5[mp+1],w6[2000];
  // number of parameters to fit
    integer npt=AMSTrCalibData::patpoints[_Algorithm][_Pattern];
    integer n=3*(npt-1); 
    //    n=3;
    integer iw=n+1;
    integer ifail=1;
    integer maxcal=1000;
    number tol=1.e-2;
    int i,j;
    for(i=0;i<mp;i++)x[i]=0;
    e04ccf_(n,x,f,tol,iw,w1,w2,w3,w4,w5,w6,palfun,pmonit,maxcal,ifail,this);
    cout << "AMSTrCalibFit::Fit finished "<<ifail<<endl;
    if(ifail ==0){
     AMSTrCalibPar res[6];
     AMSPoint out(0,0,0);
     res[0].setcoo(out);
     for(i=1;i<npt;i++){
      for(j=0;j<3;j++)out[j]=x[j+(i-1)*3];
     res[i].setcoo(out);
     }

     for(i=0;i<npt;i++){
       _pParC[i][_PositionIter]=res[i];
     }
     if(++_PositionIter >= _NIter){
      _PositionIter=0;
      Anal();
     }
    }
    else cerr <<"FIt-E-IfailNotZero "<<ifail<<endl;
}

void AMSTrCalibFit::Anal(){
//  cout <<" AMSTrCalibFit::Anal called for pattern "<<_Pattern<<" "<<_Algorithm<<endl;
  static int init=0;  
  // first Make some Histos
  int i,j,k,l;
  const int mpar=18;
  geant rpar[mpar];
  AMSTrCalibPar x[6];
  AMSTrCalibPar x2[6];
  if(init==0){
   geant limit=1000.;
   for(i=0;i<mpar;i++)HBOOK1(100000+i,"Param",500,-limit,limit,0.);
   for(i=0;i<mpar;i++)HBOOK1(200000+i,"Param",500,-limit,limit,0.);
   for(i=0;i<mpar;i++)HBOOK1(300000+i,"Param",500,-limit,limit,0.);
   for(i=0;i<mpar;i++)HBOOK1(1100000+i,"Param",500,-limit,limit,0.);
   for(i=0;i<mpar;i++)HBOOK1(1200000+i,"Param",500,-limit,limit,0.);
   for(i=0;i<mpar;i++)HBOOK1(1300000+i,"Param",500,-limit,limit,0.);
   init=1;
  }
  integer npt=AMSTrCalibData::patpoints[_Algorithm][_Pattern];
  for(j=0;j<npt;j++){
   for(i=0;i<_NIter;i++){
   x[j]=x[j]+_pParC[j][i];
   x2[j]=x2[j]+_pParC[j][i]*_pParC[j][i];
   if(j > 0){
      geant r=(_pParC[j][i].getcoo())[0]*10000;
      HF1(1000000*_Algorithm+100000+(_Pattern-1)*2+j,r,1.);
      r=(_pParC[j][i].getcoo())[1]*10000;
      HF1(1000000*_Algorithm+200000+(_Pattern-1)*2+j,r,1.);
      r=(_pParC[j][i].getcoo())[2]*10000;
      HF1(1000000*_Algorithm+300000+(_Pattern-1)*2+j,r,1.);
   }                      
   }
    x[j]=x[j]/_NIter;
    x2[j]=x2[j]/_NIter;
    x2[j]=x2[j]-x[j]*x[j];
    x2[j].sqr();
    cout <<"ANAL-I-Results "<<" Pattern "<<_Pattern<<" Alg "<<_Algorithm<<endl;
    cout <<j<<" "  <<x[j]<<" "<<x2[j]<<endl;
  }
  
}
integer AMSTrCalibFit::getlayer(integer c){
return AMSTrCalibData::patconf[_Algorithm][_Pattern][c];
}

void AMSTrCalibFit::alfun(integer &n, number xc[], number &fc, AMSTrCalibFit *p){
  integer i,niter;
  fc=0;
  for(i=0;i<n;i++){
    if(fabs(xc[i])>=0.1 ){
    fc=FLT_MAX;
    return;
    }
  }
  integer npt=AMSTrCalibData::patpoints[p->_Algorithm][p->_Pattern];
  const integer maxhits=10;
  static geant hits[maxhits][3];
  static geant sigma[maxhits][3];
  static geant normal[maxhits][3];
  integer ialgo=1;
  integer ims=1;
  integer ipart=14;
  geant out[9];
  for(i=0;i<npt;i++){
     normal[i][0]=0;
     normal[i][1]=0;
     normal[i][2]=-1;
  }
  integer npfit=0;
  for(niter=0;niter<p->_NData;niter++){
   for(i=0;i<npt;i++){
     for(int j=0;j<3;j++){
       hits[npt-i-1][j]=((p->_pData)[niter]._Hits[i])[j];
       sigma[npt-i-1][j]=((p->_pData)[niter]._EHits[i])[j];
     }
   }
   for(i=1;i<npt;i++){
     for (int j=0;j<3;j++){
      hits[i][j]+=xc[j+(i-1)*3];
     }
   }
      hits[1][1]+=-0.03;
   TKFITG(npt,hits,sigma,normal,ipart,ialgo,ims,out); 
   if(out[7]==0 && out[5]>TRCALIB.MomentumCut[p->_Algorithm][0] &&
         out[5]<TRCALIB.MomentumCut[p->_Algorithm][1]){
    const number pm2=0.88;
    number energy=sqrt(out[5]*out[5]+pm2);
    if(p->_Algorithm ==0)fc+=pow(energy/out[5]-(p->_pData)[niter]._InvBeta,2.)/
     (pow((p->_pData)[niter]._ErrInvBeta,2.)+pow(pm2/energy*out[8],2.));    
    else fc+=out[6];
    npfit++;
   }
  }
  if(npfit < n+2)fc=FLT_MAX;
  else fc=fc/(npfit-n);
  //    for(i=0;i<6;i++)cout <<" "<<xc[i]<<" ";
  //    cout << endl<<" alfun out " <<fc<<" "<<npfit<<endl;
}

void AMSTrCalibPar::sqr(){
int i,j;
for(i=0;i<3;i++)_Coo[i]=sqrt(fabs(_Coo[i]));
for(i=0;i<3;i++){
  for(j=0;j<3;j++)_Dir[i][j]=sqrt(fabs(_Dir[i][j]));
}

}




void AMSTrIdCalib::_clear(){
int i;
for(i=0;i<getnchan();i++){
 _Count[i]=0;
 _ADC[i]=0;
 _ADC2[i]=0;
 _ADCMax[i]=-FLT_MAX;
}

for(i=0;i<10;i++){
 for(int j=0;j<ms;j++)_CmnNoiseC[i][j]=0;
 for(j=0;j<ms;j++)_CmnNoise[i][j]=0;
}
}


void AMSTrIdCalib::_hist(){
  // write down the difference
  HBOOK1(400000+1,"Peds Diff",200,-10.,10.,0.);
  HBOOK1(400010+1,"Peds Calcs",200,0.,2200.,0.);
  HBOOK1(400020+1,"Peds System",200,0.,2200.,0.);
  HBOOK1(400000+2,"Sigmas Diff",200,-2.,8.,0.);
  HBOOK1(400010+2,"Sigmas Calcs",200,0.,20.,0.);
  HBOOK1(400020+2,"Sigmas System",200,0.,20.,0.);
  HBOOK1(400000+3,"Ped Accuracy Estimated",200,0.,1.,0.);
  HBOOK1(400100+1,"Peds Diff",200,-10.,10.,0.);
  HBOOK1(400110+1,"Peds Calcs",200,0.,2200.,0.);
  HBOOK1(400120+1,"Peds System",200,0.,2200.,0.);
  HBOOK1(400100+2,"Sigmas Diff",200,-2.,8.,0.);
  HBOOK1(400110+2,"Sigmas Calcs",200,0.,20.,0.);
  HBOOK1(400120+2,"Sigmas System",200,0.,20.,0.);
  HBOOK1(400100+3,"Ped Accuracy Estimated",200,0.,1.,0.);
  HBOOK1(400200+1,"Peds Diff",200,-10.,10.,0.);
  HBOOK1(400210+1,"Peds Calcs",200,0.,2200.,0.);
  HBOOK1(400220+1,"Peds System",200,0.,2200.,0.);
  HBOOK1(400200+2,"Sigmas Diff",200,-2.,8.,0.);
  HBOOK1(400210+2,"Sigmas Calcs",200,0.,20.,0.);
  HBOOK1(400220+2,"Sigmas System",200,0.,20.,0.);
  HBOOK1(400200+3,"Ped Accuracy Estimated",200,0.,1.,0.);
  PSStr_def PS;
  HBNAME(IOPA.ntuple,"PedsSigmas",(int*)(&PS),"PSLayer:I,PSLadder:I,PSHalf:I,PSSide:I, Ped:R,Sigma:R");
  int i,j,k,l,m;
   for(l=0;l<2;l++){
    for(k=0;k<2;k++){
     for(i=0;i<AMSDBc::nlay();i++){
       for(j=0;j<AMSDBc::nlad(i+1);j++){
        AMSTrIdSoft id(i+1,j+1,k,l,0);
        AMSTrIdCalib cid(id);
        for(m=0;m<AMSDBc::NStripsDrp(i+1,l);m++){
         cid.upd(m);
         if(cid.getcount()>1){
          int ch=cid.getchannel();
          PS.Layer=i+1;
          PS.Ladder=j+1;
          PS.Half=k;
          PS.Side=l;
          PS.Ped=_ADC[ch];
          PS.Sigma=_ADC2[ch];
          HFNTB(IOPA.ntuple,"PedsSigmas");
          HF1(400000+1,_ADC[ch]-cid.getped(),1.);
          HF1(400010+1,_ADC[ch],1.);
          HF1(400020+1,cid.getped(),1.);
          HF1(400000+2,_ADC2[ch]-sigmas[ch],1.);
          HF1(400010+2,_ADC2[ch],1.);
          HF1(400020+2,sigmas[ch],1.);
          HF1(400000+3,_ADC2[ch]/sqrt(_Count[ch]),1.);

          HF1(400000+(l+1)*100+1,_ADC[ch]-cid.getped(),1.);
          HF1(400010+(l+1)*100+1,_ADC[ch],1.);
          HF1(400020+(l+1)*100+1,cid.getped(),1.);
          HF1(400000+(l+1)*100+2,_ADC2[ch]-sigmas[ch],1.);
          HF1(400010+(l+1)*100+2,_ADC2[ch],1.);
          HF1(400020+(l+1)*100+2,cid.getsig(),1.);
          HF1(400000+(l+1)*100+3,_ADC2[ch]/sqrt(_Count[ch]),1.);
         }
        }
       }
     }
    }
   }
   




  for(i=0;i<getnchan();i++){
    if(_Count[i] > 0){

    }
  }

}


void AMSTrIdCalib::_calc(){

 int i,j,k,l,m;
 integer bad[2]={0,0};
 for(i=0;i<10;i++){
   for(j=0;j<ms;j++){
     if(_CmnNoiseC[i][j]>0){
      _CmnNoise[i][j]=_CmnNoise[i][j]/_CmnNoiseC[i][j];
     }
   }
 }
   for(l=0;l<2;l++){
    for(k=0;k<2;k++){
     for(i=0;i<AMSDBc::nlay();i++){
       for(j=0;j<AMSDBc::nlad(i+1);j++){
        AMSTrIdSoft id(i+1,j+1,k,l,0);
        AMSTrIdCalib cid(id);
        for(m=0;m<AMSDBc::NStripsDrp(i+1,l);m++){
         cid.upd(m);
         if(cid.getcount()>1){
          int ch=cid.getchannel();
          _ADC[ch]=(_ADC[ch]-_ADCMax[ch])/(_Count[ch]-1);
          _ADC2[ch]=(_ADC2[ch]-_ADCMax[ch]*_ADCMax[ch])/(_Count[ch]-1);
          _ADC2[ch]=sqrt(fabs(_ADC2[ch]-_ADC[ch]*_ADC[ch]));
          if(_ADC2[ch]/sqrt(_Count[ch]-1)>4*TRCALIB.PedAccRequired[l]){
           bad[l]++;
           //  cid.setstatus(AMSDBc::BAD);
          }
          _ADC[ch]=_ADC[ch]+cid.getcmnnoise();
         }
        }
       }
     }
    }
    if(bad[l])cout <<"AMSTrIdCalib::_calc-W-bad channels found for side "<<l<<" : "<<bad[l]<<endl;
   }
}

void AMSTrIdCalib::_update(){
 int i,j,k,l,m;
   for(k=0;k<2;k++){
    int total=0;
     for(l=0;l<2;l++){
     for(i=0;i<AMSDBc::nlay();i++){
       for(j=0;j<AMSDBc::nlad(i+1);j++){
        AMSTrIdSoft id(i+1,j+1,k,l,0);
        AMSTrIdCalib cid(id);
        for(m=0;m<AMSDBc::NStripsDrp(i+1,l);m++){
         cid.upd(m);
         if(cid.getcount()){
          total++;
          int ch=cid.getchannel();
          cid.setped()=_ADC[ch];
          cid.setsig()=_ADC2[ch];
         }
        }
       }
     }
     }
    if(total){
     AMSTimeID * ptdv = 
     AMSJob::gethead()->gettimestructure(AMSTrRawCluster::getTDVped(k));
     ptdv->UpdateMe()=1;
     ptdv->UpdCRC();
     time_t begin,end,insert;
     time(&insert);
     ptdv->SetTime(insert,_BeginTime-TRCALIB.Validity[0],AMSEvent::gethead()->gettime()+TRCALIB.Validity[1]);
     cout <<" Tracker H/K  info has been updated for "<<*ptdv;
     ptdv->gettime(insert,begin,end);
     cout <<" Time Insert "<<ctime(&insert);
     cout <<" Time Begin "<<ctime(&begin);
     cout <<" Time End "<<ctime(&end);
 
     ptdv = AMSJob::gethead()->gettimestructure(AMSTrRawCluster::getTDVsigma(k));
     ptdv->UpdateMe()=1;
     ptdv->UpdCRC();
     time(&insert);
     ptdv->SetTime(insert,_BeginTime-TRCALIB.Validity[0],AMSEvent::gethead()->gettime()+TRCALIB.Validity[1]);
     cout <<" Tracker H/K  info has been updated for "<<*ptdv;
     ptdv->gettime(insert,begin,end);
     cout <<" Time Insert "<<ctime(&insert);
     cout <<" Time Begin "<<ctime(&begin);
     cout <<" Time End "<<ctime(&end);

     ptdv = AMSJob::gethead()->gettimestructure(AMSTrRawCluster::getTDVstatus(k));
     ptdv->UpdateMe()=1;
     ptdv->UpdCRC();
     time(&insert);
     ptdv->SetTime(insert,_BeginTime-TRCALIB.Validity[0],AMSEvent::gethead()->gettime()+TRCALIB.Validity[1]);
     cout <<" Tracker H/K  info has been updated for "<<*ptdv;
     ptdv->gettime(insert,begin,end);
     cout <<" Time Insert "<<ctime(&insert);
     cout <<" Time Begin "<<ctime(&begin);
     cout <<" Time End "<<ctime(&end);





    cout << "AMSTrIdCalib::_update-I-total of "<<total<<" channels updated"<<endl;
    }
   }



    if (AMSFFKEY.Update){

        // Here update dbase

     AMSTimeID * offspring = 
     (AMSTimeID*)((AMSJob::gethead()->gettimestructure())->down());
     while(offspring){
      if(offspring->UpdateMe() && !offspring->write(AMSDATADIR.amsdatadir))
      cerr <<"AMSJob::_timeinitjob-S-ProblemtoUpdate "<<*offspring;
      offspring=(AMSTimeID*)offspring->next();
     }
    }



    _BeginTime=AMSEvent::gethead()->gettime();

   }


integer * AMSTrIdCalib::_Count=0;
number  *  AMSTrIdCalib::_ADC=0;
number *  AMSTrIdCalib::_ADC2=0;
number *  AMSTrIdCalib::_ADCMax=0;
integer  AMSTrIdCalib::_CmnNoiseC[10][ms];
geant  AMSTrIdCalib::_CmnNoise[10][ms];


void AMSTrIdCalib::initcalib(){
_Count= new integer[getnchan()];
_ADC= new number[getnchan()];
_ADC2= new number[getnchan()];
_ADCMax= new number[getnchan()];
assert (_Count && _ADC && _ADC2 && _ADCMax);
_clear();
}


void AMSTrIdCalib::check(integer forcedw){
static integer counter=0;
static integer hist=0;
if(++counter%TRCALIB.EventsPerCheck == 0 || forcedw){
 int i,j,k,l,m;
    number acc[2]={0,0};
    number cnt[2]={0,0};
   for(l=0;l<2;l++){
    for(k=0;k<2;k++){
     for(i=0;i<AMSDBc::nlay();i++){
       for(j=0;j<AMSDBc::nlad(i+1);j++){
        AMSTrIdSoft id(i+1,j+1,k,l,0);
        AMSTrIdCalib cid(id);
        for(m=0;m<AMSDBc::NStripsDrp(i+1,l);m++){
         cid.upd(m);
         if(cid.getcount()){
          int ch=cid.getchannel();
          acc[l]=acc[l]+
          sqrt(fabs(_ADC2[ch]-_ADC[ch]*_ADC[ch]/_Count[ch]))/_Count[ch];
          cnt[l]=cnt[l]+1;
         }
        }
       }
     }
    }
   }

   if(cnt[0]>0 && cnt[1]>0){
     if(!forcedw && (acc[0]/cnt[0] < TRCALIB.PedAccRequired[0] &&  acc[1]/cnt[1] < TRCALIB.PedAccRequired[1])){
       cout << "AMSTrIdCalib::check-I-peds & sigmas succesfully calculated with accuracies "<<
         acc[0]/cnt[0]<<" "<<acc[1]/cnt[1]<<" ( "<<counter<<" ) events."<<endl;
       cout << "AMSTrIdCalib::check-I-peds & sigmas succesfully calculated for  "<< cnt[0]+cnt[1]<< " Channels"<<endl;
       _calc();
       _hist();
       _update();
       _clear();
       counter=0;
       hist=1;
     }
     else if(forcedw && hist==0){
       cout << "AMSTrIdCalib::check-I-peds & sigmas succesfully calculated with accuracies "<<
         acc[0]/cnt[0]<<" "<<acc[1]/cnt[1]<<" ( "<<counter<<" ) events."<<endl;
       cout << "AMSTrIdCalib::check-I-peds & sigmas succesfully calculated for  "<< cnt[0]+cnt[1]<< " Channels"<<endl;
       _calc();
       _hist();
       _clear();
       counter=0;
     }
   }
}
}


void AMSTrIdCalib::buildSigmaPed(integer n, int16u *p){
  integer static first=0;
  if(first++ == 0){
    _BeginTime=AMSEvent::gethead()->gettime();
  }
  integer const maxva=64;
  integer const mss=640;
  static geant id[mss];
  static geant idlocal[maxva];
  VZERO(id,ms*sizeof(id[0])/sizeof(integer));
  int i,j,k,l;
  integer ic=AMSTrRawCluster::checkdaqidRaw(*p)-1;
  int16u * ptr=p+1;
  // Main loop
  while (ptr<p+n){
     AMSTrIdSoft idd(ic,int16u(*(ptr)));
     AMSTrIdCalib cid(idd);
     //aux loop thanks to data format to calculate corr length
     int16u * paux;
     int len=0;
     int16u bit15=1<<15;
     for(paux=ptr+1;paux<p+n;paux++){
      if( !(bit15 & *paux))break;
      else len++;
     }
     if(len > mss ){
       cerr <<" AMSTrIdCalib::buildSigmaPed-S-LengthError Max is "<<mss <<" Current is "<<len<<endl;
      len=mss;
     }
     if(!idd.dead()){
     for(j=0;j<len;j++){
      cid.upd(j);
      id[j]=float((*(ptr+1+j)) & 4095);
     }
     // calc cmn noise
      integer vamin,vamax,l;
      for (j=0;j<len;j+=vamax-vamin+1){
         cid.upd(j);
         vamin=j-cid.getstripa();
         vamax=j+maxva-cid.getstripa();
         for (l=vamin;l<vamax;l++){
           idlocal[l-vamin]=id[l];
         }
         geant cmn=0;
         for(l=0;l<maxva;l++)cmn+=idlocal[l];
         cmn=cmn/(maxva);
         cid.updcmnnoise(cmn);
         cid.updcmnnoiseC();
         for(l=vamin;l<vamax;l++)id[l]+=-cmn;
      }
      for(j=0;j<len;j++){
       cid.upd(j);
       cid.updADC(id[j]);
       cid.updADC2(id[j]);
       cid.updADCMax(id[j]);
       cid.updcounter();
      }
     }
    ptr=ptr+len+1;

  }
}


time_t AMSTrIdCalib::_BeginTime=0;
