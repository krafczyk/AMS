#include <trcalib.h>
#include <event.h>
#include <math.h>
#include <timeid.h>
#include <trrawcluster.h>
#include <float.h>
#include <mccluster.h>
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
 _ADCRaw[i]=0;
 _ADC2Raw[i]=0;
 _BadCh[i]=0;
 for(int j=0;j<nrho;j++)_ADCRho[j][i]=0;
 _ADCMax[i]=-FLT_MAX;
}

for(i=0;i<10;i++){
 for(int j=0;j<ms;j++)_CmnNoiseC[i][j]=0;
 for(j=0;j<ms;j++)_CmnNoise[i][j]=0;
}
}

void AMSTrIdCalib::offmonhist(){
HBOOK1(500000+1,"Sigmas K Crate 32",200,0.,20.,0.);
HBOOK1(500000+2,"Sigmas S Crate 32",200,0.,20.,0.);
HBOOK1(500000+10+1,"Sigmas K Crate 72",200,0.,20.,0.);
HBOOK1(500000+10+2,"Sigmas S Crate 72",200,0.,20.,0.);
HBOOK1(600000+1,"Number of Bad Channels Side K Crate 32",70,0.5,70.5,0.);
HBOOK1(600000+2,"Number of Bad Channels Side S Crate 32",70,0.5,70.5,0.);
HBOOK1(600000+10+1,"Number of Bad Channels Side K Crate 72",70,0.5,70.5,0.);
HBOOK1(600000+10+2,"Number of Bad Channels Side S Crate 72",70,0.5,70.5,0.);
  geant badch[70];
  int i,j,k,l,m;
   for(l=0;l<2;l++){
    for(k=0;k<2;k++){
     for(i=0;i<70;i++)badch[i]=0;
     for(i=0;i<AMSDBc::nlay();i++){
       for(j=0;j<AMSDBc::nlad(i+1);j++){
        AMSTrIdSoft cid(i+1,j+1,k,l,0);
        if(cid.dead())continue;
        for(m=0;m<AMSDBc::NStripsDrp(i+1,l);m++){
         cid.upd(m);
         if(cid.checkstatus(AMSDBc::BAD))badch[i*10+j+1]++;
         HF1(500000+l+1+10*k,cid.getsig(),1.);
        }
       }
     }
      for(i=0;i<70;i++)HF1(600000+l+1+10*k,float(i),badch[i]);
    }
   }
   


}


void AMSTrIdCalib::_hist(){
  // write down the difference
  HBOOK1(400000+1,"Peds Diff",200,-10.,10.,0.);
  HBOOK1(400010+1,"Peds Calcs",200,0.,2200.,0.);
  HBOOK1(400020+1,"Peds System",200,0.,2200.,0.);
  HBOOK1(400030+1,"CMNNoise",200,0.,100.,0.);
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
  HBNAME(IOPA.ntuple,"PedsSigmas",(int*)(&PS),"PSLayer:I,PSLadder:I,PSHalf:I,PSSide:I, PSStrip:I,Ped:R,Sigma:R,BadCh:R,SigmaRaw:R,Rho(64):R");
  int i,j,k,l,m;
   for(l=0;l<2;l++){
    for(k=0;k<2;k++){
     for(i=0;i<AMSDBc::nlay();i++){
       for(j=0;j<AMSDBc::nlad(i+1);j++){
        AMSTrIdSoft id(i+1,j+1,k,l,0);
        AMSTrIdCalib cid(id);
        if(cid.dead())continue;
        for(m=0;m<AMSDBc::NStripsDrp(i+1,l);m++){
         cid.upd(m);
         if(cid.getcount()>1){
          int ch=cid.getchannel();
          PS.Layer=i+1;
          PS.Ladder=j+1;
          PS.Half=k;
          PS.Side=l;
          PS.Strip=m;
          PS.Ped=_ADC[ch];
          PS.Sigma=_ADC2[ch];
          PS.BadCh=cid.checkstatus(AMSDBc::BAD);
          PS.SigmaRaw=_ADC2Raw[ch];
          PS.Rho[0]=sigmas[ch];
          for(int kk=1;kk<nrho;kk++)PS.Rho[kk]=_ADCRho[kk][ch];
          HFNTB(IOPA.ntuple,"PedsSigmas");
          HF1(400000+1,_ADCRaw[ch]-cid.getped(),1.);
          HF1(400010+1,_ADCRaw[ch],1.);
          HF1(400020+1,cid.getped(),1.);
          HF1(400030+1,cid.getcmnnoise(),1.);
          HF1(400000+2,_ADC2[ch]-sigmas[ch],1.);
          HF1(400010+2,_ADC2[ch],1.);
          HF1(400020+2,sigmas[ch],1.);
          HF1(400000+3,_ADC2[ch]/sqrt(_Count[ch]),1.);

          HF1(400000+(l+1)*100+1,_ADCRaw[ch]-cid.getped(),1.);
          HF1(400010+(l+1)*100+1,_ADCRaw[ch],1.);
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

integer AMSTrIdCalib::CalcBadCh(integer half, integer side){
   int badch=0;
     for(int i=0;i<AMSDBc::nlay();i++){
       for(int j=0;j<AMSDBc::nlad(i+1);j++){
        AMSTrIdSoft id(i+1,j+1,half,side,0);
        if(id.dead())continue;
        for(int m=0;m<AMSDBc::NStripsDrp(i+1,side);m++){
         id.upd(m);
         if(id.checkstatus(AMSDBc::BAD))badch++;  
        }
       }
     }
     return badch;
}

void AMSTrIdCalib::printbadchanlist(){
  if(TRCALIB.PrintBadChList){
  ofstream fcluster;
  ofstream fcommon;
  fcluster.open("../datacards/BadChannelList.Clusters",ios::out);
  fcommon.open("../datacards/BadChannelList.CommonNoise",ios::out);


 int i,j,k,l,m;
   for(l=0;l<2;l++){
    for(k=0;k<2;k++){
     for(i=0;i<AMSDBc::nlay();i++){
       for(j=0;j<AMSDBc::nlad(i+1);j++){
        AMSTrIdSoft id(i+1,j+1,k,l,0);
        if(id.dead())continue;
        for(m=0;m<AMSDBc::NStripsDrp(i+1,l);m++){
         id.upd(m);
         if(!id.getsignsigraw())fcommon << hex<< id.gethaddr()<<endl;
         if(id.checkstatus(AMSDBc::BAD))fcluster << hex<< id.gethaddr()<<endl;
        }
       }
     }
    }
   }







  fcluster.close();
  fcommon.close();

    
  }
}


void AMSTrIdCalib::_calc(){
#ifdef __AMSBADLIST__
  ofstream fcluster;
  ofstream fcommon;
if(TRCALIB.Pass ==2){
  fcluster.open("../datacards/BadChannelList.Clusters",ios::out);
  fcommon.open("../datacards/BadChannelList.CommonNoise",ios::out);
}
#endif


 int i,j,k,l,m;
 integer badp[2]={0,0};
 integer bada[2]={0,0};
 integer bad[2]={0,0};
 integer bad2[2]={0,0};
 integer bad3[2]={0,0};
 integer bad4[2]={0,0};
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
        if(cid.dead())continue;
        for(m=0;m<AMSDBc::NStripsDrp(i+1,l);m++){
         cid.upd(m);
         if(cid.getcount()>1){
          int ch=cid.getchannel();
          _ADC[ch]=(_ADC[ch]-_ADCMax[ch])/(_Count[ch]-1);
          _ADC2[ch]=(_ADC2[ch]-_ADCMax[ch]*_ADCMax[ch])/(_Count[ch]-1);
          _ADC2[ch]=sqrt(fabs(_ADC2[ch]-_ADC[ch]*_ADC[ch]));
          _ADCRaw[ch]=_ADCRaw[ch]/_Count[ch];
          _ADC2Raw[ch]=_ADC2Raw[ch]/_Count[ch];
          _BadCh[ch]=_BadCh[ch]/_Count[ch];
          _ADC2Raw[ch]=sqrt(fabs(_ADC2Raw[ch]-_ADCRaw[ch]*_ADCRaw[ch]));
          //          if(_ADC2[ch]/sqrt(_Count[ch]-1)>4*TRCALIB.PedAccRequired[l]){
          if(cid.checkstatus(AMSDBc::BAD))badp[l]++;
#ifdef __AMSBADLIST__
            if( TRCALIB.Pass == 2){
               // Put Here N. Produit chan
              if(!cid.getsignsigraw())fcommon << hex<< cid.gethaddr()<<endl;
            }
#endif             
          if( TRCALIB.Pass >= 2){
            geant thr=TRCALIB.BadChanThr[1]*_Count[ch];
            if(thr<3)thr=3;
            if(_BadCh[ch]*_Count[ch]>thr || cid.getsig()==0 ){ 
              cid.setstatus(AMSDBc::BAD);
#ifdef __AMSBADLIST__
            if( TRCALIB.Pass == 2){
               // Put Here N. Produit chan
              fcluster << hex<< cid.gethaddr()<<endl;
            }
#endif             
              bad[l]++;
            }
            else cid.clearstatus(AMSDBc::BAD);
            // special ladder#6 layer #3 side# 0
            //               if(cid.getlayer() == 3  && cid.getdrp() == 6 && cid.getside() == 0
            //                  && cid.gethalf()==0){
            //                   cid.setstatus(AMSDBc::BAD);            
            //#ifdef __AMSBADLIST__
            //            if( TRCALIB.Pass == 2){
            //               // Put Here N. Produit chan
            //              fcluster << hex <<cid.gethaddr()<<endl;
            //            }
            //#endif             

            //               }
            // special ladder#9 layer #2 side# 0 half# 1
               //               if(cid.getlayer() == 2  && cid.getdrp() == 9 && cid.getside() == 0
               //                  && cid.gethalf()==1)cid.setstatus(AMSDBc::BAD);            
          }
          if(cid.checkstatus(AMSDBc::BAD))bada[l]++;

          
         }
        }


        for(m=0;m<AMSDBc::NStripsDrp(i+1,l);m++){
         cid.upd(m);
         if(cid.getcount()>1){
          int ch=cid.getchannel();
          number s1=_ADCRho[cid.getstripa()][ch]/_Count[ch];
          integer vamin=m-cid.getstripa();
          for (int kk=cid.getstripa();kk<nrho;kk++){
           _ADCRho[kk][ch]=_ADCRho[kk][ch]/_Count[ch];
           cid.upd(vamin+kk);
           int ch2=cid.getchannel();
           if(TRMCFFKEY.GenerateConst) {
             if(_ADC2Raw[ch] && _ADC2Raw[ch2])
              _ADCRho[kk][ch]=(_ADCRho[kk][ch]-_ADCRaw[ch]*_ADCRaw[ch2])/
                 _ADC2Raw[ch]/_ADC2Raw[ch2];
             else _ADCRho[kk][ch]=0;
           }
           else {
             number a1=(_ADC[ch]*(_Count[ch]-1)+_ADCMax[ch])/_Count[ch];
             number a2=(_ADC[ch2]*(_Count[ch2]-1)+_ADCMax[ch2])/_Count[ch2];
             number s2=ch2==ch?s1:_ADCRho[cid.getstripa()][ch2]/_Count[ch2];
             number s1a=sqrt(fabs(s1-a1*a1));
             number s2a=sqrt(fabs(s2-a2*a2));
                          
             if(s1a && s2a)_ADCRho[kk][ch]=(_ADCRho[kk][ch]-a1*a2)/s1a/s2a;
             else _ADCRho[kk][ch]=0;
           }
          }
         }
        }

        //        for(m=0;m<AMSDBc::NStripsDrp(i+1,l);m++){
        //         cid.upd(m);
        //         if(cid.getcount()>1){
        //          int ch=cid.getchannel();
        //          _ADC[ch]=_ADC[ch]+cid.getcmnnoise();
        //         }
        //        }


       }
     }
    }
#ifdef __AMSBADLIST__
            if( TRCALIB.Pass == 2){
              fcluster.close();
              fcommon.close();
            }
#endif             

    if(bad[l])cout <<"AMSTrIdCalib::_calc-W-bad channels found for side "<<l<<" : "<<bad[l]<<endl;
    if(badp[l])cout <<"AMSTrIdCalib::_calc-W-badp channels found for side "<<l<<" : "<<badp[l]<<endl;
    if(bada[l])cout <<"AMSTrIdCalib::_calc-W-bada channels found for side "<<l<<" : "<<bada[l]<<endl;
   }
   if(TRMCFFKEY.GenerateConst){
   for(l=0;l<2;l++){
    for(k=0;k<2;k++){
     for(i=0;i<AMSDBc::nlay();i++){
       for(j=0;j<AMSDBc::nlad(i+1);j++){
        AMSTrIdSoft id(i+1,j+1,k,l,0);
        AMSTrIdCalib cid(id);
        if(cid.dead())continue;
        geant rhorho[nrho][nrho];
        for(m=0;m<AMSDBc::NStripsDrp(i+1,l);m++){
         integer mstr=(m+1)%nrho;
         integer istr=m%nrho;
         cid.upd(m);
         if(cid.getcount()>1){
         integer vamin=m-cid.getstripa();
         int ch=cid.getchannel();
         for(int kk=istr+1;kk<nrho;kk++)rhorho[kk][istr]=_ADCRho[kk][ch];
         if(mstr==0){
           for(kk=0;kk<nrho;kk++){
            for(int kj=kk+1;kj<nrho;kj++)rhorho[kk][kj]=rhorho[kj][kk];
           } 
           integer xmc=0;
           for(kk=0;kk<nrho;kk++){
            geant xrho=0;
            cid.upd(vamin+kk);
            for(int kj=0;kj<nrho;kj++){
            if(kj!=kk)xrho+=rhorho[kk][kj];
             if(kj==kk || rhorho[kk][kj]< TRCALIB.RhoThrV)cid.clearrhomatrix(kj);
             else cid.setrhomatrix(kj);
            }
            xrho=xrho/(nrho-1);
            if(xrho < TRCALIB.RhoThrA ){
             cid.upd(vamin+kk);
             _ADC2Raw[cid.getchannel()]=-_ADC2Raw[cid.getchannel()];
             bad2[l]++;
            }
            else xmc++;
           }
           if(xmc<2)bad3[l]++;
         }         
         }
        }
       }
     }
    }
    if(bad2[l])cout <<"AMSTrIdCalib::_calc-W-noise channels found for side "
    <<l<<" : "<<bad2[l]<<endl;
    if(bad2[l])cout <<"AMSTrIdCalib::_calc-W-bad VA found for side "
    <<l<<" : "<<bad3[l]<<endl;
   }
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
        if(cid.dead())continue;
        number oldone=0;
        for(m=0;m<AMSDBc::NStripsDrp(i+1,l);m++){
         cid.upd(m);
         if(cid.getcount()){
          total++;
          int ch=cid.getchannel();
          cid.setped()=_ADCRaw[ch];
          cid.setsig()=_ADC2[ch];
          cid.setsigraw()=(_ADC2Raw[ch]==0?0:1./_ADC2Raw[ch]);
          cid.setindnoise()=oldone+
          AMSTrMCCluster::sitknoiseprob(cid ,cid.getsig()*TRMCFFKEY.thr1R[l]);
          oldone=cid.getindnoise();

         }
        }
        for(m=0;m<AMSDBc::NStripsDrp(i+1,l);m+=64){
          number avs=0;
          number navs=0;
          for(int mm=0;mm<64;mm++){
           cid.upd(mm);
           if(cid.getcount()){
           int ch=cid.getchannel();
            avs+=_ADC2[ch]*_ADC2[ch];
            navs++;
           }
           if(navs)avs=avs/navs;
           cid.setcmnnoise()=sqrt(cid.getcmnnoise()-avs);
          }
        }



       }
     }
     }
    if (AMSFFKEY.Update){
    AMSTimeID *ptdv;
     time_t begin,end,insert;
    if(total){
     if(TRMCFFKEY.GenerateConst){
      ptdv = AMSJob::gethead()->gettimestructure(AMSTrRawCluster::getTDVrawsigma(k));
      ptdv->UpdateMe()=1;
      ptdv->UpdCRC();
      time(&insert);
      ptdv->SetTime(insert,_BeginTime-TRCALIB.Validity[0],_CurTime+TRCALIB.Validity[1]);
      cout <<" Tracker H/K  info has been updated for "<<*ptdv;
      ptdv->gettime(insert,begin,end);
      cout <<" Time Insert "<<ctime(&insert);
      cout <<" Time Begin "<<ctime(&begin);
      cout <<" Time End "<<ctime(&end);



     ptdv = AMSJob::gethead()->gettimestructure(AMSTrRawCluster::getTDVrhomatrix(k));
     ptdv->UpdateMe()=1;
     ptdv->UpdCRC();
     time(&insert);
     ptdv->SetTime(insert,_BeginTime-TRCALIB.Validity[0],_CurTime+TRCALIB.Validity[1]);
     cout <<" Tracker H/K  info has been updated for "<<*ptdv;
     ptdv->gettime(insert,begin,end);
     cout <<" Time Insert "<<ctime(&insert);
     cout <<" Time Begin "<<ctime(&begin);
     cout <<" Time End "<<ctime(&end);



     }
     if(TRCALIB.Pass == 1 || TRMCFFKEY.GenerateConst ){  

     ptdv = 
     AMSJob::gethead()->gettimestructure(AMSTrRawCluster::getTDVped(k));
     ptdv->UpdateMe()=1;
     ptdv->UpdCRC();
     time(&insert);
     ptdv->SetTime(insert,_BeginTime-TRCALIB.Validity[0],_CurTime+TRCALIB.Validity[1]);
     cout <<" Tracker H/K  info has been updated for "<<*ptdv;
     ptdv->gettime(insert,begin,end);
     cout <<" Time Insert "<<ctime(&insert);
     cout <<" Time Begin "<<ctime(&begin);
     cout <<" Time End "<<ctime(&end);

     ptdv = AMSJob::gethead()->gettimestructure(AMSTrRawCluster::getTDVsigma(k));
     ptdv->UpdateMe()=1;
     ptdv->UpdCRC();
     time(&insert);
     ptdv->SetTime(insert,_BeginTime-TRCALIB.Validity[0],_CurTime+TRCALIB.Validity[1]);
     cout <<" Tracker H/K  info has been updated for "<<*ptdv;
     ptdv->gettime(insert,begin,end);
     cout <<" Time Insert "<<ctime(&insert);
     cout <<" Time Begin "<<ctime(&begin);
     cout <<" Time End "<<ctime(&end);
     }
     if(TRCALIB.Pass ==1 && k==1 && !TRMCFFKEY.GenerateConst){

      ptdv = 
      AMSJob::gethead()->gettimestructure(AMSTrRawCluster::getTDVCmnNoise());
      ptdv->UpdateMe()=1;
      ptdv->UpdCRC();
      time(&insert);
      ptdv->SetTime(insert,_BeginTime-TRCALIB.Validity[0],_CurTime+TRCALIB.Validity[1]);
      cout <<" Tracker H/K  info has been updated for "<<*ptdv;
      ptdv->gettime(insert,begin,end);
      cout <<" Time Insert "<<ctime(&insert);
      cout <<" Time Begin "<<ctime(&begin);
      cout <<" Time End "<<ctime(&end);


      //      ptdv = 
      //      AMSJob::gethead()->gettimestructure(AMSTrRawCluster::getTDVIndNoise());
      //      ptdv->UpdateMe()=1;
      //      ptdv->UpdCRC();
      //      time(&insert);
      //      ptdv->SetTime(insert,_BeginTime-TRCALIB.Validity[0],_CurTime+TRCALIB.Validity[1]);
      //      cout <<" Tracker H/K  info has been updated for "<<*ptdv;
      //      ptdv->gettime(insert,begin,end);
      //      cout <<" Time Insert "<<ctime(&insert);
      //      cout <<" Time Begin "<<ctime(&begin);
      //      cout <<" Time End "<<ctime(&end);


     }
     if(TRCALIB.Pass ==2){

     ptdv = AMSJob::gethead()->gettimestructure(AMSTrRawCluster::getTDVstatus(k));
     ptdv->UpdateMe()=1;
     ptdv->UpdCRC();
     time(&insert);
     ptdv->SetTime(insert,_BeginTime-TRCALIB.Validity[0],_CurTime+TRCALIB.Validity[1]);
     cout <<" Tracker H/K  info has been updated for "<<*ptdv;
     ptdv->gettime(insert,begin,end);
     cout <<" Time Insert "<<ctime(&insert);
     cout <<" Time Begin "<<ctime(&begin);
     cout <<" Time End "<<ctime(&end);

     }






    cout << "AMSTrIdCalib::_update-I-total of "<<total<<" channels updated"<<endl;
    }
   }




        // Here update dbase

     AMSTimeID * offspring = 
     (AMSTimeID*)((AMSJob::gethead()->gettimestructure())->down());
     while(offspring){
       if(offspring->UpdateMe())cout << " Starting to update "<<*offspring; 
      if(offspring->UpdateMe() && !offspring->write(AMSDATADIR.amsdatadir))
      cerr <<"AMSJob::_timeinitjob-S-ProblemtoUpdate "<<*offspring;
      offspring=(AMSTimeID*)offspring->next();
     }
    }



    _BeginTime=_CurTime;

   }


integer * AMSTrIdCalib::_Count=0;
geant * AMSTrIdCalib::_BadCh=0;
number  *  AMSTrIdCalib::_ADC=0;
number *  AMSTrIdCalib::_ADC2=0;
number  *  AMSTrIdCalib::_ADCRaw=0;
number *  AMSTrIdCalib::_ADC2Raw=0;
number * AMSTrIdCalib::_ADCRho[nrho];
number *  AMSTrIdCalib::_ADCMax=0;
integer  AMSTrIdCalib::_CmnNoiseC[10][ms];
geant  AMSTrIdCalib::_CmnNoise[10][ms];


void AMSTrIdCalib::initcalib(){
_Count= new integer[getnchan()];
_BadCh= new geant[getnchan()];
_ADC= new number[getnchan()];
_ADC2= new number[getnchan()];
_ADCRaw= new number[getnchan()];
_ADC2Raw= new number[getnchan()];
for(int i=0;i<nrho;i++)_ADCRho[i]=new number[getnchan()];
_ADCMax= new number[getnchan()];

assert (_Count && _ADC && _ADC2 && _ADCMax && _ADC2Raw && _ADCRaw && _BadCh);
_clear();


  {
    for (int i=0;i<6;i++){
     for(int j=1;j<11;j++){
      HBOOK1(10000+i*10+j,"Noise",200,-10.,10.,0.);
      HBOOK1(20000+i*10+j,"Noise",200,-10.,10.,0.);
      HBOOK1(30000+i*10+j,"Noise",200,-10.,10.,0.);
      HBOOK1(60000+i*10+j,"Noise",200,-10.,10.,0.);
      HBOOK1(40000+i*10+j,"Noise",200,-10.,10.,0.);
      HBOOK1(80000+i*10+j,"Noise",200,-10.,10.,0.);
      HBOOK1(110000+i*10+j,"Noise",200,-10.,10.,0.);
      HBOOK1(120000+i*10+j,"Noise",200,-10.,10.,0.);
      HBOOK1(130000+i*10+j,"Noise",200,-10.,10.,0.);
      HBOOK1(160000+i*10+j,"Noise",200,-10.,10.,0.);
      HBOOK1(140000+i*10+j,"Noise",200,-10.,10.,0.);
      HBOOK1(180000+i*10+j,"Noise",200,-10.,10.,0.);
     }
    }
  }


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
        if(cid.dead())continue;
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
       _update();
       _clear();
       counter=0;
     }
   }
}
}


void AMSTrIdCalib::buildSigmaPedB(integer n, int16u *p){
   _CurTime=AMSEvent::gethead()->gettime();
   if(_CurRun > 0 && _CurRun != AMSEvent::gethead()->getrun())check(1);
   _CurRun=AMSEvent::gethead()->getrun();
  integer static first=0;
  if(first++ == 0){
    _BeginTime=AMSEvent::gethead()->gettime();
  }

  integer const ms=640;
  integer len;
  static geant id[ms];
  //VZERO(id,ms*sizeof(id[0])/sizeof(integer));
  int i,j,k;
  integer ic=AMSTrRawCluster::checkdaqidRaw(*p)-1;
  int16u * ptr=p+1;
  // Main loop
  integer oldformat=1;
  while (ptr<p+n){
    // Read two tdrs
    integer subl=*ptr;
    integer ntdr = *(ptr+1) & 31;
    if(subl != 3084){
      // Probably new format
      oldformat=0; 
    }
    ptr+=2;
    for(i=0;i<ntdr;i++){
     int16u tdrn=*ptr;
     int16u lrec=*(ptr+1);
     ptr++;
     if(tdrn < 16){
       // S side
       len=640;
       for(j=0;j<2;j++){
         int16u conn,tdrs;
         tdrs=tdrn/2;
         if(tdrn%2==0){
          if(j==0)conn=1;
          else conn=0;
         }
         else {
          if(j==0)conn=3;
          else conn=2;
         }
         int16u haddr=(conn<<10) | (tdrs <<12);
         AMSTrIdSoft idd(ic,haddr);
         if(!idd.dead()){
          // copy to local buffer and subtract peds
          for(k=0;k<320;k++){
           idd.upd(k);
           id[k]=float(*(ptr+2+k+j*(640+128*oldformat))); 
          }
          for(k=320;k<640;k++){
           idd.upd(k);
           id[k]=float(*(ptr+2+k+64*oldformat+j*(640+128*oldformat))); 
          }
          buildpreclusters(idd,len,id);
         }
       }
     }
     else if(tdrn<24){
       // K Side
       len=384;
       for(j=0;j<4;j++){
          int16u conn, tdrk;
          if(tdrn%2 ==0){
            if(j<2)conn=j+2;
            else conn=j-2;
          }
          else {
           if(j<2)conn=j+2;
           else conn=j-2;
           conn+= 4;
          }
          tdrk=(tdrn-16)/2;
          int16u haddr=(10<<6) |( conn<<10) | (tdrk <<13);
          AMSTrIdSoft idd(ic,haddr);
         if(!idd.dead()){
          // copy to local buffer and subtract peds
          for(k=0;k<384;k++){
           idd.upd(k);
          id[k]=float(*(ptr+2+k+j*384)); 
          }
          buildpreclusters(idd,len,id);
         }
       }
     }
     else{
       cerr<<"AMSTrIdCalib::buildSigmaPedB-E-TDRNOutOfRange "<<tdrn<<endl;
     }
     ptr+=lrec;
    }
    if(!oldformat)ptr+=3;
  }





}


void AMSTrIdCalib::buildSigmaPed(integer n, int16u *p){
   _CurTime=AMSEvent::gethead()->gettime();
   _CurRun=AMSEvent::gethead()->getrun();
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
      for (j=0;j<len;j+=maxva){
         cid.upd(j);
         vamin=j-cid.getstripa();
         vamax=j+maxva-cid.getstripa();
         geant cmn=0;
         geant avsig=0;
         for (l=vamin;l<vamax;l++){
           cid.upd(l);
           cmn+=(id[l]-cid.getped())*cid.getsignsigraw();
           avsig+=cid.getsignsigraw();
         }
         if(avsig>1)cmn=cmn/avsig;
         cid.updcmnnoise(cmn);
         cid.updcmnnoiseC();
         for(l=vamin;l<vamax;l++){
           cid.upd(l);
           cid.updADCRaw(id[l]);         
           cid.updADC2Raw(id[l]);         
           if(TRMCFFKEY.GenerateConst){
             integer vamin=l-cid.getstripa();
             for(int kk=cid.getstripa();kk<nrho;kk++)
             cid.updADCRho(id[l],id[vamin+kk],kk);
           }
           id[l]+=-cmn;
         }         
      }
      for(j=0;j<len;j++){
       cid.upd(j);
       integer ids=(cid.getlayer()-1)*10+cid.getdrp();
       if(cid.getsig()!=0 && TRCALIB.Pass>1){
          geant r=(id[j]-cid.getped())/cid.getsig();
          if(r<-TRCALIB.BadChanThr[0])cid.updBadCh();
          if(TRCALIB.Pass >= 2){
           HF1(100000*cid.gethalf()+(cid.getside()+1)*10000+ids,r,1.);
           if(cid.checkstatus(AMSDBc::BAD)){
              HF1(100000*cid.gethalf()+(cid.getside()+1)*40000+ids,r,1.);
           }
           else HF1(100000*cid.gethalf()+(cid.getside()+1)*30000+ids,r,1.);
          }
       }
       cid.updADC(id[j]);
       cid.updADC2(id[j]);
       cid.updADCMax(id[j]);
       if(!TRMCFFKEY.GenerateConst && TRCALIB.Pass==1){
           integer vamin=j-cid.getstripa();
          for(int kk=cid.getstripa();kk<nrho;kk++)
           cid.updADCRho(id[j],id[vamin+kk],kk);
       }
       cid.updcounter();
      }
     }
    ptr=ptr+len+1;

  }
}



void AMSTrIdCalib::buildSigmaPedA(integer n, int16u *p){
   _CurTime=AMSEvent::gethead()->gettime();
   _CurRun=AMSEvent::gethead()->getrun();
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
       cerr <<" AMSTrIdCalib::buildSigmaPedA-S-LengthError Max is "<<mss <<" Current is "<<len<<endl;
      len=mss;
     }
     if(!idd.dead()){
     for(j=0;j<len;j++){
      cid.upd(j);
      id[j]=float((*(ptr+1+j)) & 4095);
     }
       buildpreclusters(idd,len,id);
       
     }
    ptr=ptr+len+1;

  }
}



time_t AMSTrIdCalib::_BeginTime=0;
time_t AMSTrIdCalib::_CurTime=0;
uinteger AMSTrIdCalib::_CurRun=-1;



void AMSTrIdCalib::buildpreclusters(AMSTrIdSoft & idd, integer len, geant id[]){
     // calc cmn noise
  integer const maxva=64;
  static geant idlocal[maxva];
  integer ic=idd.gethalf();
      int i,j,k;
      AMSTrIdCalib cid(idd);

     // calc cmn noise
      integer vamin,vamax,l;
      for (j=0;j<len;j+=maxva){
         cid.upd(j);
         vamin=j-cid.getstripa();
         vamax=j+maxva-cid.getstripa();
           geant cmn=0;
           geant avsig=0;
         for (l=vamin;l<vamax;l++){
           cid.upd(l);
           cmn+=(id[l]-cid.getped())*cid.getsignsigraw();
           avsig+=cid.getsignsigraw();
           idlocal[l-vamin]=id[l]-cid.getped();
         }
         if(avsig>1)cmn=cmn/avsig;
         cid.updcmnnoise(cmn);
         cid.updcmnnoiseC();
         for (l=vamin;l<vamax;l++){
           cid.upd(l);
           cid.updADCRaw(id[l]);         
           cid.updADC2Raw(id[l]);         
           if(TRMCFFKEY.GenerateConst){
             integer vamin=l-cid.getstripa();
             for(int kk=cid.getstripa();kk<nrho;kk++)
             cid.updADCRho(id[l],id[vamin+kk],kk);
           }
           if(!cid.getsignsigraw() ){
            geant cmn=0;
            geant avsig=0;
           for(int kk=0;kk<maxva;kk++){
            if(cid.getrhomatrix(kk)){
             cid.upd(kk+vamin);
                          cmn+=idlocal[kk]*fabs(cid.getsigraw());
                          //             cmn+=idlocal[kk];
             avsig++;
             cid.upd(l);
            }
           }
            if(avsig>1 && cid.getsigraw()!=0){
                           cmn=cmn/avsig/fabs(cid.getsigraw());
                           //             cmn=cmn/avsig;
            }          
            id[l]+=-cmn;
           }
           else id[l]+=-cmn;
         }

         for(l=vamin;l<vamax;l++){
          cid.upd(l);
       integer ids=(cid.getlayer()-1)*10+cid.getdrp();
       if(cid.getsig()!=0 && TRCALIB.Pass>1){
          geant r=(id[l]-cid.getped())/cid.getsig();
          if(r<-TRCALIB.BadChanThr[0])cid.updBadCh();
          if(TRCALIB.Pass >= 2) {
           HF1(100000*cid.gethalf()+(cid.getside()+1)*10000+ids,r,1.);
           if(cid.checkstatus(AMSDBc::BAD)){
               HF1(100000*cid.gethalf()+(cid.getside()+1)*40000+ids,r,1.);
           }
           else HF1(100000*cid.gethalf()+(cid.getside()+1)*30000+ids,r,1.);
          }
       }
          cid.updADC(id[l]);
          cid.updADC2(id[l]);
          cid.updADCMax(id[l]);
          if(!TRMCFFKEY.GenerateConst && TRCALIB.Pass ==1){
            integer vamin=l-cid.getstripa();
            for(int kk=cid.getstripa();kk<nrho;kk++)
             cid.updADCRho(id[l],id[vamin+kk],kk);
          }
          cid.updcounter();

        }
      }


      // Dynamically Update pedestals if needed
      if(TRCALIB.DPS && TRCALIB.Pass>=2){
       for(j=0;j<len;j++){
          idd.upd(j);
          if(fabs(id[j]-idd.getped())<TRCALIB.DPS*idd.getsig()){
           idd.setped()=idd.getped()+(id[j]-idd.getped())/TRCALIB.UPDF;
         }           
       }
      }




}
