

#include <trcalib.h>
#include <event.h>
#include <math.h>


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


void AMSTrCalibData::Init(AMSBeta *pbeta, AMSTrTrack * ptrack, integer pattern){
  number beta=pbeta->getbeta();
   _ErrInvBeta==pbeta->getebeta();
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
  int i,j;
  int k=0;
  for(i=0;i<3;i++){
    for(j=0;j<ptrack->getnhits();j++){
     AMSTrRecHit * ph= ptrack->getHitP(j);
     if (ph->getLayer() == pattern+i){
      _Hits[i]=ph->getHit();
      _EHits[i]=ph->getEHit();
      k++;
      break;
     }
    }
  }
  if(k != 3){
    cerr <<"AMSTrCalibData::Init-F-Logic Error "<<k<<" "<<pattern<<endl;
    exit(1);
  }                  
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
              if(PatternMatch(pattern,ptrack->getpattern())){
               Init(pbeta,ptrack,pattern);
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

  integer AMSTrCalibData::PatternMatch(integer patcal, integer pattr){
    const integer dim=7;
    static integer patmat[tkcalpat][dim]={
                                 0,1,2,5, 7, 8, 9,
                                 0,1,5,6, 7,17,18,
                                 0,4,5,6,13,17,21,
                                 0,3,4,6,16,20,21};
    int i;
    if(patcal >0 && patcal <= tkcalpat)
     for(i=0;i<dim;i++)if(pattr == patmat[patcal-1][i])return 1;     
    return 0;
  }

AMSTrCalibFit * AMSTrCalibFit::_pCalFit[2][tkcalpat]={0,0,0,0,
                                                       0,0,0,0};

void AMSTrCalibFit::setHead(integer alg, integer pat, AMSTrCalibFit *ptr){
  if(alg>=0 && alg < 2 && pat >=0 && pat < tkcalpat)
  _pCalFit[alg][pat]=ptr;
}  
AMSTrCalibFit::AMSTrCalibFit():_pData(0),_PositionData(0),_Pattern(0),_pParM(0),_NSen(0),_pParS(0),
  _NData(0),_NIter(0),_PositionIter(0){
  int i;
  for(i=0;i<3;i++){
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

int i,j;
for(i=0;i<3;i++){
 _pParC[i]= new AMSTrCalibPar[_NIter];
}

for(i=_Pattern;i<_Pattern+3;i++)
  for(j=1;j<=AMSDBc::nlad(i);j++) _NSen+=AMSDBc::nsen(i,j);
_pParM=new AMSTrCalibPar[_NSen];
_pParS=new AMSTrCalibPar[_NSen];
}

AMSTrCalibFit::~AMSTrCalibFit(){
 delete [] _pData;
 delete [] _pParM;
 delete [] _pParS;
  int i;
  for(i=0;i<3;i++){
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
    const integer mp=100;
    static number f,x[mp],w1[mp],w2[mp],w3[mp],w4[mp],w5[mp+1],w6[2000];
    integer n=6;    // number of parameters to fit
    integer iw=n+1;
    integer ifail=1;
    integer maxcal=1000;
    number tol=1.e-2;
    int i;
    for(i=0;i<n;i++)x[i]=0;
    e04ccf_(n,x,f,tol,iw,w1,w2,w3,w4,w5,w6,palfun,pmonit,maxcal,ifail,this);
    cout << "AMSTrCalibFit::Fit finished "<<ifail<<endl;
    if(ifail ==0){
     AMSTrCalibPar res[3];
     AMSPoint out(0,0,0);
     res[0].setcoo(out);
     for(i=0;i<3;i++)out[i]=x[i];
     res[1].setcoo(out);
     for(i=0;i<3;i++)out[i]=x[3+i];
     res[2].setcoo(out);
     for(i=0;i<3;i++){
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
  AMSTrCalibPar x[3];
  AMSTrCalibPar x2[3];
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
  for(j=0;j<3;j++){
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
  
  for(i=1;i<4;i++){
   integer address=0;
   integer layer=getlayer(i);
   for(l=1;l<layer;l++)
   for(j=1;j<=AMSDBc::nlad(l);j++) address+=AMSDBc::nsen(l,j);
   _pParM[address]=x[i-1];
   _pParS[address]=x2[i-1];
}
}
integer AMSTrCalibFit::getlayer(integer c){
#ifdef __AMSDEBUG__
assert(c> 0 && c< 4);
#endif
return _Pattern+c-1;
}

void AMSTrCalibFit::alfun(integer &n, number xc[], number &fc, AMSTrCalibFit *p){
  integer i,niter;
  fc=0;
  for(i=0;i<n;i++){
    if(fabs(xc[i])>0.1){
    fc=FLT_MAX;
    return;
    }
  }
  integer npt=3;
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
   hits[1][0]+=xc[0];
   hits[1][1]+=xc[1]-0.03;
   hits[1][2]+=xc[2];
   hits[2][0]+=xc[3];
   hits[2][1]+=xc[4];
   hits[2][2]+=xc[5];
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
  //  for(i=0;i<6;i++)cout <<" "<<xc[i]<<" ";
  //  cout << endl<<" alfun out " <<fc<<" "<<npfit<<endl;
}

void AMSTrCalibPar::sqr(){
int i,j;
for(i=0;i<3;i++)_Coo[i]=sqrt(fabs(_Coo[i]));
for(i=0;i<3;i++){
  for(j=0;j<3;j++)_Dir[i][j]=sqrt(fabs(_Dir[i][j]));
}

}
