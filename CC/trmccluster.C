#include "trmccluster.h"
#include <math.h>
#include "event.h"
#include "ntuple.h"
#ifdef __G4AMS__
#include "g4util.h"
#endif




integer AMSTrMCCluster::debug(1);
integer AMSTrMCCluster::_ncha(100);
geant   AMSTrMCCluster::_step(0.05);
integer AMSTrMCCluster::_hid[2]={200103,200104};  
integer AMSTrMCCluster::_NoiseMarker(555);
AMSTrMCCluster::AMSTrMCCluster
(const AMSTrIdGeom & id,integer side,integer nel, number ss[], integer itra){
  _next=0;
  _idsoft=id.cmpt();
  _xca=0;
  _xcb=0;
  _xgl=-1000;
  _itra=itra;
  _sum=0;
  if(nel>5)nel=5;
  VZERO(_ss,10*sizeof(geant)/4);
  integer cside= side==0?1:0;
  _left[side]=max(0,id.getstrip(side)-nel/2); 
  _right[cside]=0; 
  _right[side]=min(id.getstrip(side)+nel/2,id.getmaxstrips(side)-1); 
  _left[cside]=1;
  _center[side]=id.getstrip(side);
  _center[cside]=0;
  for(int i=_left[side];i<_right[side]+1;i++)
    _ss[side][i-_left[side]]=ss[i+nel-_right[side]-1];
  if(side==1){
    number cofgy=id.strip2size(1);
    number cofgx=0;
    AMSgSen *p=
      (AMSgSen*)AMSJob::gethead()->getgeomvolume(id.crgid());
    if(p){
      _xgl=p->str2pnt(cofgx,cofgy);
    }
  }
}

void AMSTrMCCluster::addcontent(const AMSTrIdSoft &  idl, geant *adc){
  integer i;
  integer side=idl.getside();
  if(side==0){
    for(i=_left[side];i<=_right[side];i++){
      AMSTrIdSoft id(AMSTrIdGeom(_idsoft,i,0),side);
      if(idl.getaddr() != id.getaddr())return;
      if(id.dead())continue; 
      adc[id.getstrip()]=  adc[id.getstrip()]+_ss[side][i-_left[side]];
    }
  }  
  else {
    for(i=_left[side];i<=_right[side];i++){
      AMSTrIdSoft id(AMSTrIdGeom(_idsoft,0,i),side);
      if(idl.getaddr() != id.getaddr()  )return;
      if(id.dead())continue; 
      adc[id.getstrip()]=  adc[id.getstrip()]+_ss[side][i-_left[side]];
    }
  }
}

void AMSTrMCCluster::addcontent(char xy, geant ** p, integer noise){
  integer i;
  if(noise && _itra!=_NoiseMarker)return;
  if(xy=='x'){
    for(i=_left[0];i<=_right[0];i++){
      AMSTrIdSoft id(AMSTrIdGeom(_idsoft,i,0),0);
      if(id.dead()){
	continue;
      }
      if(p[id.getaddr()]==0){
	integer isize=sizeof(geant)*id.getmaxstrips();
	p[id.getaddr()]=(geant*)UPool.insert(isize);
	if(p[id.getaddr()]==0){
	  cerr <<" AMSTrMCCLUSTER-S-Memory Alloc Failure"<<endl;
	  return;
	}
	VZERO(p[id.getaddr()],isize/4);
      }
      if(_itra!=_NoiseMarker)(*(p[id.getaddr()]+id.getstrip()))+= _ss[0][i-_left[0]];
      else if(noise)(*(p[id.getaddr()]+id.getstrip()))=_ss[0][i-_left[0]];
      //    cout <<id<<" side 0 adc + "<<id.getstrip()<<" "<<_ss[0][i-_left[0]]<< endl;
    }
  }
  else{
    for(i=_left[1];i<=_right[1];i++){
      AMSTrIdSoft id(AMSTrIdGeom(_idsoft,0,i),1);
      if(id.dead()){
	continue;
      }
      if(p[id.getaddr()]==0){
	integer isize=sizeof(geant)*id.getmaxstrips();
	p[id.getaddr()]=(geant*)UPool.insert(isize);
	if(p[id.getaddr()]==0){
	  cerr <<" AMSTrMCCLUSTER-S-Memory Alloc Failure"<<endl;
	  return;
	}
	VZERO(p[id.getaddr()],isize/4);
      }
      if(_itra!=_NoiseMarker)(*(p[id.getaddr()]+id.getstrip()))+= _ss[1][i-_left[1]];
      else if(noise) (*(p[id.getaddr()]+id.getstrip()))=_ss[1][i-_left[1]];
      //    cout <<id <<" side 1 adc + "<<id.getstrip()<<" "<<_ss[1][i-_left[1]]<< endl;
    }
  }
}

void AMSTrMCCluster::_shower(){
  //  cout <<"*** start shower " <<endl;
  AMSgObj::BookTimer.start("TrMCCluster");
  //   Non active silicon as geant volume introduced ; skip some lines below
  //
  //  // As we have no non-active silicon as geant volume 
  //  // we should do something manually in case we are in it;
  //  // Very crude method (but fast)
  //  // gives some overestimation of dead space by ~100-200 mkm == ~10-20%
  //     number sa[2],sb[2];
  //     if((_xca[0]<0 || _xca[0]>id.getmaxsize(0)) ||
  //        (_xca[1]<0 || _xca[1]>id.getmaxsize(1)) ||
  //        (_xcb[0]<0 || _xcb[0]>id.getmaxsize(0)) ||
  //        (_xcb[1]<0 || _xcb[1]>id.getmaxsize(1))   )_sum=0; 
  //
  //  // End active check
  //  //

  AMSTrIdGeom id(_idsoft);
  AMSPoint entry=(_xca+_xcb)/2.;
  AMSPoint dentry=(_xcb-_xca)/2;
  AMSDir dir=_xcb-_xca;
  geant cofg=0;
  geant esumx=0;
  geant esumy=0;
  integer ierr;
  integer i;
  geant xpr=_sum*TRMCFFKEY.dedx2nprel;
  for(i=0;i<_nel;i++)_ss[0][i]=0;
  for(i=0;i<_nel;i++)_ss[1][i]=0;
  _center[0]=id.size2strip(0,entry[0]);
  _center[1]=id.size2strip(1,entry[1]);
  _left[0]= max(0,_center[0]-(_nel/2));
  _left[1]= max(0,_center[1]-(_nel/2));
  _right[0]=min(id.getmaxstrips(0)-1,_center[0]+(_nel/2));
  _right[1]=min(id.getmaxstrips(1)-1,_center[1]+(_nel/2));
  if(xpr >0.){
    for (integer k=0;k<2;k++){
      if(fabs(dentry[k])/(xpr)<TRMCFFKEY.fastswitch){
	// fast algo
	id.upd(k,_center[k]);  // update geom id
	number e=entry[k]-id.strip2size(k);
	number s=id.strip2size(k);
	for (int ii=_left[k];ii<=_right[k];ii++){
	  number a1,a2;
	  id.upd(k,ii);  // update geom id
	  a1=id.strip2size(k)-s;
	  a2=a1+id.getsize(k);
	  number r=a2-a1;
	  _ss[k][ii-_left[k]]=TRMCFFKEY.delta[k]*xpr*sitkfint2_(a1,a2,e,dentry[k],r);
	}     
      }
      else{
	//slow algo
	for(int kk=0;kk<floor(xpr);kk++){
	  geant e,dummy=0;
	  e=entry[k]-dentry[k]+2*RNDM(dummy)*dentry[k];
	  number a1,a2;
	  for (int ii=_left[k];ii<=_right[k];ii++){
	    id.upd(k,ii);  // update geom id
	    a1=id.strip2size(k)-e;
	    a2=a1+id.getsize(k);
	    _ss[k][ii-_left[k]]=_ss[k][ii-_left[k]]+TRMCFFKEY.delta[k]*sitkfints_(a1,a2);
	    //       _ss[k][ii-_left[k]]=_ss[k][ii-_left[k]]+TRMCFFKEY.delta[k]*sitkfint2_(a1,a2,0.,0.,1.);
	  }
	}
      }
      integer nedx;
      for(int ii=_left[k];ii<=_right[k];ii++){
	_ss[k][ii-_left[k]]=_ss[k][ii-_left[k]]*(1+rnormx()*TRMCFFKEY.gammaA[k]);
      }
    }
  }

  AMSgObj::BookTimer.stop("TrMCCluster");
  // cout <<"** end shower"<<endl;
}




void AMSTrMCCluster::_printEl(ostream & stream){
  AMSTrIdGeom id(_idsoft);
  stream << "AMSTrMCCluster-Shower x "<<_itra<<" "<<id.getlayer()<<" "
	 <<_ss[0][0]<<" "<<_ss[0][1]<<" "<<_ss[0][2]<<" "
	 <<_ss[0][3]<<" "<<_ss[0][4]<<endl;
  stream << "AMSTrMCCluster-Shower y "<<_itra<<" "<<id.getlayer()<<" "
	 <<_ss[1][0]<<" "<<_ss[1][1]<<" "<<_ss[1][2]<<" "
	 <<_ss[1][3]<<" "<<_ss[1][4]<<endl;
  stream << "AMSTrMCCluster-Coo "<<_itra<<" "<<id.getlayer()<<" "
	 <<_xca<<" "<<_xcb<<" "<<_xgl<<endl;
}

void AMSTrMCCluster::_copyEl(){
}

void AMSTrMCCluster::_writeEl(){

  integer flag =    (IOPA.WriteAll%10==1)
    || (IOPA.WriteAll%10==0 && checkstatus(AMSDBc::USED))
    || (IOPA.WriteAll%10==2 && !checkstatus(AMSDBc::AwayTOF));

  
  if(AMSTrMCCluster::Out(flag)){
    int i;
#ifdef __WRITEROOT__
    AMSJob::gethead()->getntuple()->Get_evroot02()->AddAMSObject(this);
#endif
/*
    TrMCClusterNtuple* TrMCClusterN = AMSJob::gethead()->getntuple()->Get_trclmc();
  
    if (TrMCClusterN->Ntrclmc>=MAXTRCLMC) return;

    // Fill the ntuple
    TrMCClusterN->Idsoft[TrMCClusterN->Ntrclmc]=_idsoft;
    TrMCClusterN->TrackNo[TrMCClusterN->Ntrclmc]=_itra;
    for(i=0;i<2;i++)TrMCClusterN->Left[TrMCClusterN->Ntrclmc][i]=_left[i];
    for(i=0;i<2;i++)TrMCClusterN->Center[TrMCClusterN->Ntrclmc][i]=_center[i];
    for(i=0;i<2;i++)TrMCClusterN->Right[TrMCClusterN->Ntrclmc][i]=_right[i];
    for(i=0;i<2;i++)TrMCClusterN->SS[TrMCClusterN->Ntrclmc][i][0]=_ss[i][0];
    for(i=0;i<2;i++)TrMCClusterN->SS[TrMCClusterN->Ntrclmc][i][1]=_ss[i][1];
    for(i=0;i<2;i++)TrMCClusterN->SS[TrMCClusterN->Ntrclmc][i][2]=_ss[i][2];
    for(i=0;i<2;i++)TrMCClusterN->SS[TrMCClusterN->Ntrclmc][i][3]=_ss[i][3];
    for(i=0;i<2;i++)TrMCClusterN->SS[TrMCClusterN->Ntrclmc][i][4]=_ss[i][4];
    for(i=0;i<3;i++)TrMCClusterN->Xca[TrMCClusterN->Ntrclmc][i]=_xca[i];
    for(i=0;i<3;i++)TrMCClusterN->Xcb[TrMCClusterN->Ntrclmc][i]=_xcb[i];
    for(i=0;i<3;i++)TrMCClusterN->Xgl[TrMCClusterN->Ntrclmc][i]=_xgl[i];
    TrMCClusterN->Sum[TrMCClusterN->Ntrclmc]=_sum;
    TrMCClusterN->Ntrclmc++;
*/
  }

}

number AMSTrMCCluster::sitknoiseprob(const AMSTrIdSoft & id, number threshold){
  number z;
  number sigma=id.getsig();
  if(sigma >0)z=threshold/sigma/sqrt(2.);
  else z=FLT_MAX;
  return DERFC(z)/2;  

}

number AMSTrMCCluster::sitknoiseprobU(number threshold, number step){
  return 0.5*(DERFC(threshold)-DERFC(threshold+step));  
}

void  AMSTrMCCluster::sitknoisespectrum(const AMSTrIdSoft & id, number ss[],
                                        number prob){
  geant d=0;
  if(RNDM(d)>=prob){
#ifdef __G4AMS__
    if(MISCFFKEY.G4On)ss[0]=id.getsig()*AMSRandGeneral::hrndm1(hid(id.getside()));
#else
    ss[0]=id.getsig()*HRNDM1(hid(id.getside()));
#endif
  }
  else  ss[0]=id.getsig()*rnormx();
}


void AMSTrMCCluster::sitkhits(
			      integer idsoft , geant vect[],geant edep, geant step, integer itra ){
  AMSgObj::BookTimer.start("SITKHITS");
  AMSPoint pa(vect[0],vect[1],vect[2]);
  AMSPoint dirg(vect[3],vect[4],vect[5]);
  AMSPoint pb=pa-dirg*step;
  AMSPoint pgl=pa-dirg*step/2;
  AMSgSen *p=(AMSgSen*)AMSJob::gethead()->getgeomvolume(
							AMSTrIdGeom(idsoft).crgid());
  if(p){
    integer ilay=AMSTrIdGeom(p->getid()).getlayer();
#ifdef __AMSDEBUG__
    //  cout <<"Sitkhits - "<<ilay<<" "<<pgl<<endl;
#endif
 
    if(p->getsenstrip(pa) && p->getsenstrip(pb)){
//      cout <<" edep "<<edep<<endl;
      AMSEvent::gethead()->addnext(AMSID("AMSTrMCCluster",0),
				   new AMSTrMCCluster(idsoft,pa,pb,pgl,edep,itra));
    }
  }
  else{
    cerr << "sitkhits Error" << idsoft <<" "
	 <<AMSTrIdGeom(idsoft).crgid()<<endl;
  }
  AMSgObj::BookTimer.stop("SITKHITS");
}




void AMSTrMCCluster::sitknoise(){
  // Only add noise when not in raw mode
  AMSgObj::BookTimer.start("SITKNOISE");
  geant dummy=0;
  number noise,oldone=0;
  integer itra=_NoiseMarker;
  number ss[5];
  number const probthr=0.05;
  float totn[2]={0,0};
  for(int l=0;l<2;l++){
    for (int i=0;i<TKDBc::nlay();i++){
      for (int j=0;j<TKDBc::nlad(i+1);j++){
	for (int s=0;s<2;s++){
	  if(TKDBc::activeladdshuttle(i+1,j+1,s)){
            AMSTrIdSoft id(i+1,j+1,s,l,0);
	    if(!id.dead()){
              int icmpt=id.gettdr();
	      if(TRMCFFKEY.RawModeOn[id.getcrate()][id.getside()][icmpt])continue;
	      id.upd(id.getmaxstrips()-1);
	      geant r=RNDM(dummy);
	      if(r<id.getindnoise()){
		//bsearch
		id.upd(0);
		while(id.getprob(r) <id.getmaxstrips()){
		  r=RNDM(dummy);

		  totn[l]=totn[l]+1;
		  AMSTrIdSoft idx,idy;
		  if(l==0){
		    idx=id;
		    idy=AMSTrIdSoft(i+1,j+1,s,1,0);
		  }
		  else {
		    idy=id;
		    idx=AMSTrIdSoft(i+1,j+1,s,0,0);
		  }
		  integer nambig;
		  AMSTrIdGeom *pid=idx.ambig(idy,nambig);
		  if(pid){
		    // calculate prob to avoid double counitng
		    number prob=(1+TRMCFFKEY.NonGaussianPart[l])*
		      exp(-id.getmaxstrips()*
			  sitknoiseprob(id,TRMCFFKEY.thr1R[l]*id.getsig()));

		    sitknoisespectrum(id,ss,1-prob);
		    HF1(200105+id.getside(),ss[0],1.);
		    AMSEvent::gethead()->addnext(AMSID("AMSTrMCCluster",0),
						 new AMSTrMCCluster(*pid,id.getside(),1,ss,itra));
		  }
		}
	      }
	    }
          }
	}
      }

    }
    HF1(200101+l,totn[l],1.);

  }

  AMSgObj::BookTimer.stop("SITKNOISE");

}

void  AMSTrMCCluster::sitkcrosstalk(){
}




integer AMSTrMCCluster::Out(integer status){
  static integer init=0;
  static integer WriteAll=0;
  if(init == 0){
    init=1;
    integer ntrig=AMSJob::gethead()->gettriggerN();
    for(int n=0;n<ntrig;n++){
      if(strcmp("AMSTrMCCluster",AMSJob::gethead()->gettriggerC(n))==0){
	WriteAll=1;
	break;
      }
    }
  }
  return (WriteAll || status);
}



// write some trmc cluster to daq files
//only itra + xgl
// V. Choutko 8-jul-1999


integer AMSTrMCCluster::checkdaqid(int16u id){
  if(id==getdaqid())return 1;
  else return 0;
}

void AMSTrMCCluster::builddaq(integer i, integer length, int16u *p){
  AMSTrMCCluster *ptr=(AMSTrMCCluster*)AMSEvent::gethead()->
    getheadC("AMSTrMCCluster",0);
  p--;
  geant sum=0;
  while(ptr){ 
    const uinteger c=65535;
    *(p+1)=ptr->_itra;
    integer big=10000;
    if(ptr->getsum()==0)goto metka;
    for(int k=0;k<3;k++){
      if(ptr->_xgl[k]+big<=0){
	goto metka;
      }
      uinteger cd=(ptr->_xgl[k]+big)*10000;
      *(p+3+2*k)=int16u(cd&c);
      *(p+2+2*k)=int16u((cd>>16)&c);
    }
    sum=ptr->getsum()*1000000;
    if(sum>c)sum=c;
    *(p+8)=int16u(sum);
//    cout <<" sum "<<ptr->getsum()<<" "<<sum<<endl;
    p+=8;
  metka:
    ptr=ptr->next();
  }
  *(p+1)=getdaqid() ;
}


void AMSTrMCCluster::buildraw(integer n, int16u *p){
  integer ip;
  geant mom;
  int len=n&65535;
  for(int16u *ptr=p;ptr<p+len-1;ptr+=8){ 
    ip=*(ptr);
    uinteger cdx=  (*(ptr+2)) | (*(ptr+1))<<16;  
    uinteger cdy=  (*(ptr+4)) | (*(ptr+3))<<16;  
    uinteger cdz=  (*(ptr+6)) | (*(ptr+5))<<16;  
    geant sum=geant(*(ptr+7))/1000000;
    AMSPoint coo(cdx/10000.-10000.,cdy/10000.-10000.,cdz/10000.-10000.);
//      cout <<ip<<" "<<coo<<" "<<sum<<endl;
    AMSTrMCCluster *ptrhit=(AMSTrMCCluster*)AMSEvent::gethead()->addnext(AMSID("AMSTrMCCluster",0), new
									 AMSTrMCCluster(coo,ip,sum));
    if(ptrhit->IsNoise())ptrhit->setstatus(AMSDBc::AwayTOF);

  }

} 

integer AMSTrMCCluster::calcdaqlength(integer i){
  AMSContainer *p = AMSEvent::gethead()->getC("AMSTrMCCluster");
  int len=1; 
  if(p){
    AMSTrMCCluster *ptr=(AMSTrMCCluster*)AMSEvent::gethead()->
      getheadC("AMSTrMCCluster",0);
    while(ptr){ 
      const integer big=10000;
      if(ptr->_sum!=0 && ptr->_xgl[0]+big>0 && ptr->_xgl[1]+big>0 && ptr->_xgl[2]+big>0)len+=8; 
      ptr=ptr->next();
    }
  }
  return -len;
}

