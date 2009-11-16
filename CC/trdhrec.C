#include "trdrec.h"
#include "trdhrec.h"
#include "event.h"
#include "ntuple.h"
#include "extC.h"
#include "root.h"

static AMSTRDHSegment* pTRDHSegment_[6];
#pragma omp threadprivate (pTRDHSegment_)

static AMSTRDHTrack* pTRDHTrack_[4];
#pragma omp threadprivate (pTRDHTrack_)

static int nhseg,nhtr;
#pragma omp threadprivate (nhseg)
#pragma omp threadprivate (nhtr)

static TH2A* H2A_mvr[2] = {
  new TH2A ("ha_mvr_0", "m vs x", 419, -105.0, 105.0, 189, -3.0, 3.0),
  new TH2A ("ha_mvr_1", "m vs y", 419, -105.0, 105.0, 189, -3.0, 3.0)
};

static TH2V* H2V_mvr[2] = {
  new TH2V ("hv_mvr_0", "m vs x", 419, -105.0, 105.0, 189, -3.0, 3.0),
  new TH2V ("hv_mvr_1", "m vs y", 419, -105.0, 105.0, 189, -3.0, 3.0)
};
#pragma omp threadprivate (H2V_mvr)
#pragma omp threadprivate (H2A_mvr)

integer AMSTRDHSegment::build(int rerun){
// This routine does the TRD Track fitting based on TRDRawHits
// It reconstructs segments separately for the two TRD projections:
// in XZ plane (d=0) from TRD layers 5-16 
// in YZ plane (d=1) from TRD layers 1-4 and 17-20
  for(int i=0;i!=6;i++) pTRDHSegment_[i]=NULL;

  int nhcut=40; //  below 40 nTRDRawHits: vector-histo 
                 // above                 array-histo

  double Hcut=0.0;
  //if(pev->nTRDRawHit()>500) Hcut=30.0;

  int nrh0=0,nrh1=0;
  nrh0= AMSEvent::gethead()->getC("AMSTRDRawHit",0)->getnelem();
  nrh1= AMSEvent::gethead()->getC("AMSTRDRawHit",1)->getnelem();

  int nrh=nrh0+nrh1;

  //  cout << "Raw Hits " << nrh << endl;
  if(nrh<2) return 0;

  AMSTRDRawHit* rhits[nrh]; 
  for(int i=0;i!=nrh;i++)rhits[i]=NULL;

  int n=0;
  for(AMSTRDRawHit* Hi=(AMSTRDRawHit*)AMSEvent::gethead()->getheadC("AMSTRDRawHit",0);Hi;Hi=Hi->next()) rhits[n++]=Hi;
  for(AMSTRDRawHit* Hi=(AMSTRDRawHit*)AMSEvent::gethead()->getheadC("AMSTRDRawHit",1);Hi;Hi=Hi->next()) rhits[n++]=Hi;

  for(int i=0;i!=n;i++){
    TRDHitRZD rzd(rhits[i]);
    
    //    cout << "i rzd" << i << "," << rzd.r << "," << rzd.z << "," << rzd.d << endl;
  }
  if(nrh>nhcut){
    H2A_mvr[0]->Reset();
    H2A_mvr[1]->Reset();
  }else{
    H2V_mvr[0]->Reset();
    H2V_mvr[1]->Reset();
  }

  // Fill all TRDRawHit pair (m,r) into x/y histo:
  double s=0.,o=0.,weight=1.;
  AMSTRDRawHit  *Hi, *Hj;

  for (int i=0; i<nrh; i++){ 
    Hi = rhits[i];
    if(Hi){
      TRDHitRZD RZDi(Hi);
      if(Hi->Amp()<Hcut) continue;
      
      for (int j=i+1; j<nrh; j++){
	Hj = rhits[j];
	if(Hj){
	  TRDHitRZD RZDj(Hj);
	  if(Hj->Amp()<Hcut) continue;
	  
	  if( (RZDi.d == RZDj.d) && (RZDi.z != RZDj.z) ){
	    s = (RZDi.r-RZDj.r)/(RZDi.z-RZDj.z); //slope
	    o = RZDi.r - (RZDi.z-TRDz0)*s; //offset
	    //weight = Hi->Amp*Hj->Amp;
	    if(nrh>nhcut) H2A_mvr[(int)RZDi.d]->Fill(o,s,weight);           
	    else          H2V_mvr[(int)RZDi.d]->Fill(o,s,weight);           
	  } } } } }
  
  // Find Peak(s) in Histo
  PeakXYW pxyw;
  nhseg=0;
  for(int i=0;i!=2; i++){

    if(nrh>nhcut) pxyw = H2A_mvr[i]->GetPeak(1);
    else          pxyw = H2V_mvr[i]->GetPeak(1);
    
    if(pxyw.w>0.) pTRDHSegment_[nhseg++]=new AMSTRDHSegment(i,pxyw.y, pxyw.x, TRDz0, pxyw.w);
  }

  //  printf("nhseg %i \n",nhseg);
  if(nhseg<1)return 0;
  
  // Link TRDRawHits to segments:
  int nh0[nhseg];
  AMSTRDRawHit* hit ;
  for( int j=0; j<nhseg; j++){
    nh0[j]=0;
    for (int i=0; i<nrh; i++){
      hit = rhits[i];
      if(hit!=NULL){
	TRDHitRZD rzd(hit);
	if((rzd.d == pTRDHSegment_[j]->d )
	   && (fabs(pTRDHSegment_[j]->resid(rzd.r,rzd.z,rzd.d))<RSDmax) ){

	  pTRDHSegment_[j]->fTRDRawHit.push_back(hit);
	} } } }
  
  for( int j=0; j<nhseg; j++) pTRDHSegment_[j]->nhits=pTRDHSegment_[j]->fTRDRawHit.size();//(int)nh0[j];
  
  
  int nh1[nhseg];
  for( int j=0; j<nhseg; j++){
    nh1[j]=0;

    int lr =  pTRDHSegment_[j]->LinReg();  // LinReg with Hits linked to segments
    if( lr>=0 ){  // Re-Link TRDRawHits to segments after successful LinReg:
      pTRDHSegment_[j]->fTRDRawHit.clear();
      for (int i=0; i<nrh; i++){
	AMSTRDRawHit* hit = rhits[i];
	if(hit){
	  TRDHitRZD rzd(hit);
	  if(    (rzd.d == pTRDHSegment_[j]->d )
	         && (fabs(pTRDHSegment_[j]->resid(rzd.r,rzd.z,rzd.d))<RSDmax) )

	    pTRDHSegment_[j]->fTRDRawHit.push_back(hit);//[nh1[j]++]=hit;
	}
      }
      if(nh0[j]!=nh1[j])pTRDHSegment_[j]->LinReg();
    }
  }
  
  
  if(nhseg>0){
    for(int i=0;i!=nhseg;i++){
      pTRDHSegment_[i]->calChi2();
      AMSEvent::gethead()->addnext(AMSID("AMSTRDHSegment",i),pTRDHSegment_[i]);
      //      printf("seg %i chi2 %f\n",i,pTRDHSegment_[i]->chi2);
    }
  }
  return nhseg;
}
 
  
integer AMSTRDHTrack::build(int rerun){
  for(int i=0;i!=4;i++) pTRDHTrack_[i]=NULL;
  nhtr=0;

  AMSTRDHSegment *segx=NULL,*segy=NULL,*seg=NULL;
  if(nhseg<2)return 0;

  for(int i=0;i!=nhseg;i++){
    seg = pTRDHSegment_[i];
    if(seg!=NULL&&!isnan(seg->d)){
      if(seg->d==0)segx=seg;
      else if (seg->d==1)segy=seg;
    }
  }

  if(segx==NULL || segy==NULL || isnan(segx->m) || isnan(segy->m)) return 0;

  float pos_[3],dir_[3];
  float mx_, my_, dx_, dy_, dz_, rx_,ry_, rzx_,rzy_;
  
  mx_=segx->m;my_=segy->m;
  rx_=segx->r;ry_=segy->r;
  rzx_=segx->z;rzy_=segy->z;

  dir_[0]=-mx_;
  dir_[1]=-my_;
  dir_[2]=-1.;
  
  float mag=sqrt(pow(dir_[0],2)+pow(dir_[1],2)+pow(dir_[2],2));
  //  printf("mag %f x %f y %f z %f\n",mag,dir_[0],dir_[1],dir_[2]);
  for( int i=0;i!=3;i++)dir_[i]/=mag;

  // assure same origin (center of TRD)
  if(rzx_!=rzy_){
    ry_=ry_ + my_ * (rzx_-rzy_);
    rzy_=rzx_;
  }
  pos_[0]=rx_;pos_[1]=ry_;pos_[2]=rzx_;

  // build AMSTRDHTrack
  AMSTRDHTrack *tr=new AMSTRDHTrack(pos_,dir_);

  // add HSegment pointers to AMSTRDHTrack object
  tr->SetSegment(segx,segy);

  if(TRDFITFFKEY.SaveHistos>0){
    TRDInfoNtuple02* info = AMSJob::gethead()->getntuple()->Get_trdinfo02();
    if(info){
      for(int i=0;i!=2;i++){
	AMSTRDHSegment* seg=tr->fTRDHSegment[i];
	for(int j=0;j!=seg->nhits;j++){
	  AMSTRDIdSoft id(seg->fTRDRawHit[j]->getidsoft());
	  int lay=id.getlayer();
	  int lad=id.getladder();
	  int tub=id.gettube();
	  float amp=seg->fTRDRawHit[j]->Amp();
	  
#ifdef __WRITEROOT__
	  int hid=41000+lay*290+lad*16+tub;
	  //	  TH1F* h=AMSJob::gethead()->getntuple()->Get_evroot02()->h1(hid);
	  //	  if(!h){
	  if(!AMSJob::gethead()->getntuple()->Get_evroot02()->h1(hid)){
	    //      if(info->hit_arr[lay][lad][tub]==0){
	    char hnam[30];
	    sprintf(hnam,"TRD_L%iL%iT%i",lay,lad,tub);
	    AMSJob::gethead()->getntuple()->Get_evroot02()->hbook1(hid,hnam,50,0,200);
	  }
	  AMSJob::gethead()->getntuple()->Get_evroot02()->hfill(hid,amp,0.,1.);
	  
#endif
	  info->hit_arr[lay][lad][tub]++;
	}
      }
    }
  }
  AMSEvent::gethead()->addnext(AMSID("AMSTRDHTrack",0),tr);
  
  return nhtr;
}

void AMSTRDHSegment::_copyEl(){
#ifdef __WRITEROOT__
  if(PointerNotSet())return;
  TrdHSegmentR & ptr =AMSJob::gethead()->getntuple()->Get_evroot02()->TrdHSegment(_vpos); 
  for (int i=0; i<nhits; i++) {
    AMSTRDRawHit* rh=fTRDRawHit[i];
    if (rh) ptr.fTrdRawHit.push_back(rh->GetClonePointer());
  }
#endif
}

void AMSTRDHSegment::_writeEl(){
  integer flag =    (IOPA.WriteAll%10==1)
    || (checkstatus(AMSDBc::USED));
  if(Out(flag) ){
#ifdef __WRITEROOT__
    AMSJob::gethead()->getntuple()->Get_evroot02()->AddAMSObject(this);
#endif
  }
}

integer AMSTRDHSegment::Out(integer status){
static integer init=0;
static integer WriteAll=1;
if(init == 0){
 init=1;
 integer ntrig=AMSJob::gethead()->gettriggerN();
  for(int n=0;n<ntrig;n++){
    if(strcmp("AMSTRDHSegment",AMSJob::gethead()->gettriggerC(n))==0){
     WriteAll=1;
     break;
    }
  }
}
return (WriteAll || status);
}

integer AMSTRDHSegment::_addnext(integer d, float m, float r, float z, float w, integer nhits, AMSTRDRawHit* pthit[]){
#ifdef __UPOOL__
  cout<< "AMSTRDHSegment::_addnext-F-UPOOL not supported yet"<<endl;
  abort();
#else
  AMSTRDHSegment *ptrack= new AMSTRDHSegment(d,m,r,z,w,nhits,pthit);
#endif
  ptrack->_addnextR(0);

  delete ptrack;  
  return 0;
}

AMSTRDHSegment *AMSTRDHSegment::gethead(uinteger i){
  if(i<TRDDBc::nlayS()){
    return (AMSTRDHSegment*)AMSEvent::gethead()->getheadC("AMSTRDHSegment",i,1);
  }
  else {
#ifdef __AMSDEBUG__
    cout <<"AMSTRDHSegment:gethead-S-Wrong Head "<<i;
#endif
    return 0;
  }
}

AMSTRDHTrack *AMSTRDHTrack::gethead(uinteger i){
  return (AMSTRDHTrack*)AMSEvent::gethead()->getheadC("AMSTRDHTrack",i,1);
#ifdef __AMSDEBUG__
  cout <<"AMSTRDHSegment:gethead-S-Wrong Head "<<i;
#endif
  return 0;
}


void AMSTRDHSegment::_addnextR(uinteger iseg){
  // permanently add;
  AMSEvent::gethead()->addnext(AMSID("AMSTRDSegment",iseg),this);
}

void AMSTRDHTrack::_copyEl(){
#ifdef __WRITEROOT__
 if(PointerNotSet())return;
  TrdHTrackR & ptr = AMSJob::gethead()->getntuple()->Get_evroot02()->TrdHTrack(_vpos);
    for (int i=0; i<nhseg; i++) {
      AMSTRDHSegment *seg=fTRDHSegment[i];
      if(seg)ptr.fTrdHSegment.push_back(seg->GetClonePointer()); 
    }
#endif
}

void AMSTRDHTrack::_writeEl(){
  integer flag =    (IOPA.WriteAll%10==1)
                 || (checkstatus(AMSDBc::USED));
  if(Out(flag) ){
#ifdef __WRITEROOT__
    AMSJob::gethead()->getntuple()->Get_evroot02()->AddAMSObject(this);
#endif
  }
}

integer AMSTRDHTrack::Out(integer status){
static integer init=0;
static integer WriteAll=1;
if(init == 0){
 init=1;
 integer ntrig=AMSJob::gethead()->gettriggerN();
  for(int n=0;n<ntrig;n++){
    if(strcmp("AMSTRDHTrack",AMSJob::gethead()->gettriggerC(n))==0){
     WriteAll=1;
     break;
    }
  }
}
return (WriteAll || status);
}

void AMSTRDHSegment::_printEl(ostream &o){
  o << " TRDHSegment printout "<<endl;
}

void AMSTRDHTrack::_printEl(ostream &o){
  o <<"TRDHTrack printout"<<endl;
}

integer AMSTRDHTrack::_addnext(float pos[3],float dir[3], AMSTRDHSegment* pthit[]){

#ifdef __UPOOL__
  cout<< "AMSTRDHTrack::_addnext-F-UPOOL not supported yet"<<endl;
  abort();
#else

  AMSTRDHTrack *ptrack=new AMSTRDHTrack(pos,dir);
  if(!pthit[0]&&!pthit[1]) ptrack->SetSegment(pthit[0],pthit[1]);
  
#endif
  
  ptrack->_addnextR(0);
  
  delete ptrack;  
  return 0;
}

void AMSTRDHTrack::_addnextR(uinteger itr){
  int i;
  // permanently add;
  if(pTRDHTrack_[itr]!=NULL)
    AMSEvent::gethead()->addnext(AMSID("AMSTRDHTrack",itr),pTRDHTrack_[itr]);
}

