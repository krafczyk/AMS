#include <trcalib.h>
#include <event.h>

AMSTrCalibPar::AMSTrCalibPar(AMSPoint coo, AMSDir dir[3]):_Coo(coo){
 int i;
 for(i=0;i<3;i++)_Dir[i]=dir[i];
}

AMSTrCalibPar::AMSTrCalibPar(AMSPoint coo, number nrm[3][3]):_Coo(coo){
 int i,j;
 for(i=0;i<3;i++)
  for(j=0;j<3;j++)_Dir[i][j]=nrm[i][j];
}


void AMSTrCalibData::Init(AMSBeta *pbeta, AMSTrTrack * ptrack, integer pattern){
  number beta=pbeta->getbeta();
  if(beta < 1){
   number errbeta=pbeta->getebeta();
   const number pmass=0.938;
   number gamma=1./sqrt(1.-beta*beta);
   _Beta=beta;
   _InvMomentum=1/beta/gamma/pmass;
   _ErrInvMomentum=gamma*errbeta/pmass;
  }
  else{
   _Beta=1;
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
int i,j;
integer address=0;
for(i=1;i<layer;i++)
  for(j=1;j<=AMSDBc::nlad(i);j++) address+=AMSDBc::nsen(i,j);
  for(j=1;j<ladder;j++) address+=AMSDBc::nsen(layer,j);
  address+=sensor-1;
  return _pParS+address;
}

void AMSTrCalibFit::Fit(){
  cout << "AMSTrCalibFit::Fit started for pattern "<<_Pattern<<" "<<_Algorithm<<" "<<
    _PositionIter<<endl;
  // Fit Here
    AMSTrCalibPar res[3];
    int i;
    for(i=0;i<3;i++){
      _pParC[i][_PositionIter]=res[i];
    }
    if(++_PositionIter >= _NIter){
     _PositionIter=0;
     Anal();
    }
}

void AMSTrCalibFit::Anal(){
  cout <<" AMSTrCalibFit::Anal called for pattern "<<_Pattern<<" "<<_Algorithm<<endl;
}
