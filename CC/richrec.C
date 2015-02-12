//  $Id$
#include <math.h>
#include "commons.h"
#include "ntuple.h"
#include "richrec.h"
#include "richid.h"
#include "richradid.h"
#include "richlip.h"

int RichLIPRec::actual;
int RichLIPRec::totalhits;
int RichLIPRec::hitinlip[11000];

#include "mccluster.h"
#include "tofrec02.h"
#include <fenv.h>
#ifdef __WRITEROOT__
#include "root.h" 
#endif

#ifdef __MIC__
#define __DARWIN__
#endif
#ifdef _OPENMP
#include <omp.h>
#endif


//
// TODO: Check that the reconstructions used the right refractive index
//       Currently it simply calls get_tile_index of richradid

void AMSRichRawEvent::mc_build(){
  // Add noise...
  geant mean_noisy=RICnwindows*RICHDB::total*RICHDB::prob_noisy;
  int dummy1;
  integer n_noisy;
  integer hitn=1;


  POISSN(mean_noisy,n_noisy,dummy1);

  for(int i=0;i<n_noisy;i++){
    // Get a random channel number and make it noisy
    integer channel=integer(RICnwindows*RICHDB::total*RNDM(dummy1));
    AMSRichMCHit::noisyhit(channel);
  }


  // Add dark current
  // This is not used until a parametrization exists 
  //
  //geant mean_noisy=RICnwindows*RICHDB::total*RICHDB::prob_dark;
  //POISSN(mean_noisy,n_noisy,dummy1);
  //for(int i=0;i<n_noisy;i++){
  // Get a random channel number and make it noisy
  //integer channel=integer(RICnwindows*RICHDB::total*RNDM(dummy1));
  //AMSRichMCHit::darkhit(channel);???
  //}


  // Construct event: channel signals

   int nhits=0;
   int nnoisy=0;
   //   int ndark=0;

   for(AMSRichMCHit* hits=(AMSRichMCHit *)AMSEvent::gethead()->getheadC("AMSRichMCHit",0,1);hits;hits=hits->next()){ // loop on signals

      integer channel=hits->getchannel();
      if(channel==-1) continue;  //SKIP charged particles

      if(channel>=RICnwindows*RICHDB::total){
         cerr<< "AMSRichRawEvent::mc_build-ChannelNoError "<<channel<<endl;
          break;
      }
      hits->puthit(hitn); // To construct the pointer list

      if(hits->getid()==Cerenkov_photon) nhits++;
      if(hits->getid()==Noise) nnoisy++;

      //if(hits->getid()==Dark_current) ndark++;   // NOT IMPLEMENTED
      
      
      if(hits->testlast()){ // last signal in the hit so construct it
	// Simulate the gainX5 mode
	geant pedestal=nnoisy>0?AMSRichMCHit::noise(channel,1):AMSRichMCHit::adc_empty(channel,1);
	geant signal=AMSRichMCHit::adc_hit(nhits,channel,1);
	RichPMTChannel calibration(channel);
	geant threshold;
	integer mode=1;

        if(integer(signal+pedestal)>calibration.gain_threshold){
	  // Change to gainX1 mode
	  mode=0;
	  pedestal=nnoisy>0?AMSRichMCHit::noise(channel,0):AMSRichMCHit::adc_empty(channel,0);
	  
	  signal*=calibration.gain[1]>0?calibration.gain[0]/calibration.gain[1]:0.2;  
	}

	threshold=calibration.pedestal_threshold[mode]*calibration.pedestal_sigma[mode]+calibration.pedestal[mode];
	nnoisy=0;
	nhits=0;
	//	ndark=0;

	if(integer(signal+pedestal)>threshold && (calibration.status%10)==Status_good_channel){
	  //	if(integer(signal+pedestal)>threshold){
	  AMSEvent::gethead()->addnext(AMSID("AMSRichRawEvent",0),
				       new AMSRichRawEvent(channel,integer(signal+pedestal-calibration.pedestal[mode]),(mode==0?0:gain_mode)));

	  hitn++;
	  
	//        if(nnoisy>0) pedestal=AMSRichMCHit::noise(channel); 
	//	else if(nhits>0) pedestal=AMSRichMCHit::adc_empty();
	//        signal+=AMSRichMCHit::adc_hit(nhits);
	//        nnoisy=0;
	//        nhits=0;
	//        if(integer(signal)>=integer(RICHDB::c_ped)+integer(RICHDB::ped)){
	//	  AMSEvent::gethead()->addnext(AMSID("AMSRichRawEvent",0),
	//				       new AMSRichRawEvent(channel,integer(signal)));
	//          hitn++;
	//          signal=0;
        } else {
	  for(AMSRichMCHit* clean=(AMSRichMCHit *)AMSEvent::gethead()->getheadC("AMSRichMCHit",0,1);clean;clean=clean->next()){
	    if(clean->gethit()==hitn) clean->puthit(0);
	    if(clean->gethit()==-1) break;
	  }
	}
      }
   }
}

int AMSRichRawEvent::getHwAddress(){
  int pmt,pixel;
  RichPMTsManager::UnpackGeom(_channel,pmt,pixel);
  int pmt_pos=RichPMTsManager::GetPMTID(pmt);
  int hwPixel=RichPMTsManager::GetChannelID(pmt,pixel);

  return  16*pmt_pos+hwPixel;
}


int AMSRichRawEvent::photoElectrons(double sigmaOverQ){
  const int maxComp=10;
  static float rl[maxComp]={-1,-1,-1,-1,-1,-1,-1,-1,-1,-1};
  static float rs[maxComp]={-1,-1,-1,-1,-1,-1,-1,-1,-1,-1};
#pragma omp threadprivate(rl,rs)  

  float Npe=getnpe();
  int org=int(floor(Npe));
  double sum=0;
  double weight=0;
  for(int n=max(org-3,1);n<=org+3;n++){
    float mean=n;
    float rms=n*sigmaOverQ*sigmaOverQ;
    double lkh=0;
    if(n>maxComp){
      // Use gaussian approximation
      lkh=exp(-0.5*(Npe-n)*(Npe-n)/rms-0.5*log(rms));
    }else{
      // Compute the true pdf
      if(rl[n-1]==-1){rms=sqrt(rms);GETRLRS(mean,rms,rl[n-1],rs[n-1]);}
      lkh=PDENS(Npe,rl[n-1],rs[n-1]);
    }
    sum+=n*lkh;
    weight+=lkh;
  }
#ifdef __AMSDEBUG__
  cout<<"NPE="<<Npe<<" RETURNING "<<sum/weight<<endl;
#endif
  return sum/weight;
}


int AMSRichRawEvent::_npart=0;
int AMSRichRawEvent::Npart(){return _npart;}
integer AMSRichRawEvent::_PMT_Status[RICmaxpmts];

void AMSRichRawEvent::build(){
  double signal[RICmaxpmts];
  int hid[RICmaxpmts];
  int test[RICmaxpmts];
  double best_mean=0;
  double best_sigma=1e200;

  //
  // Unflag hits and collect the PMT signal
  //
  
  _npart=0;
  for(int i=0;i<RICmaxpmts;signal[i++]=0);
  for(int i=0;i<RICmaxpmts;_PMT_Status[i++]=0);

  for(AMSRichRawEvent* current=(AMSRichRawEvent *)AMSEvent::gethead()->
	getheadC("AMSRichRawEvent",0);current;current=current->next()){
    current->unsetbit(crossed_pmt_bit);

    // Skip it if its status if cero
    if((current->getchannelstatus()%10)!=Status_good_channel) continue;

    int pmt=current->getchannel()/16;
    if(pmt>=RICHDB::total){
      cerr<< "AMSRichRawEvent::build-ChannelNoError "<<pmt<<endl;
      return;
    }
    signal[pmt]+=current->getnpe();
  }

  //
  // Count nb. of PMTS and fill arrays
  //
  int npmt=0;
  for(int i=0;i<RICmaxpmts;i++){
    if(signal[i]>0) hid[npmt++]=i;
  }
  
  const int minimum_pmts=4;
  const double flag_threshold=10;
  int bootstraps=100;

  if(npmt>=minimum_pmts){
    int pmts=int(npmt*(1-1.0/minimum_pmts));
    
    for(int i=0;i<bootstraps;i++){
      double mean;
      double mean2;
      Select(pmts,npmt,test);
      mean=0;
      mean2=0;
      for(int j=0;j<npmt;j++){
        if(!test[j]) continue;
        mean+=signal[hid[j]];
        mean2+=signal[hid[j]]*signal[hid[j]];
      }
      
      mean/=pmts;
      mean2/=pmts;
      mean2-=mean*mean;
      mean2=sqrt(mean2);
      
      if(mean2<best_sigma){
        best_mean=mean;
        best_sigma=mean2;
      }
    }

    //
    // Flag hits and pmts and count number of good/bad PMTs
    //
    
    for(AMSRichRawEvent* current=(AMSRichRawEvent *)AMSEvent::gethead()->
	  getheadC("AMSRichRawEvent",0);current;current=current->next()){
      current->unsetbit(crossed_pmt_bit);

      if((current->getchannelstatus()%10)!=Status_good_channel) continue;

      int pmt=current->getchannel()/16;
      
      if(signal[pmt]-best_mean>flag_threshold*best_sigma) current->setbit(crossed_pmt_bit);
    }
    
    _npart=0;
    for(int i=0;i<npmt;i++) if(signal[hid[i]]-best_mean>flag_threshold*best_sigma){
      _npart++;
      _PMT_Status[i]=1;
    }
  }

}


double AMSRichRawEvent::RichRandom(){
  static unsigned long int x=1234567;
  static unsigned long int p1=16807;
  static unsigned long int N=2147483647;
#pragma omp threadprivate(x,p1,N)  

  x=(p1*x)%N;
  return ((double)x)/((double)(N+1));
}


void AMSRichRawEvent::Select(int howmany,int size,int lista[]){
  int i,j;
  int true_index;

  for(i=0;i<size;i++) lista[i]=0;

  for(i=0;i<howmany;i++){
    int selected;
    selected=int((size-i)*RichRandom());

    j=0;
    for(true_index=0;true_index<size;true_index++){
      if(!lista[true_index])j++;
      if(j>selected) break;
    }
    lista[true_index]=1;
  }
}


void AMSRichRawEvent::_writeEl(){
  RichPMTChannel channel(_channel);

#ifdef __WRITEROOT__
    float x = channel.x();
    float y = channel.y();
    float z = channel.z();

    AMSJob::gethead()->getntuple()->Get_evroot02()->AddAMSObject(this,x,y,z);
#endif
}



void AMSRichRawEvent::reconstruct(AMSPoint origin,AMSPoint origin_ref,
                                  AMSDir direction,AMSDir direction_ref,
				  geant betamin,geant betamax,
				  geant *betas,geant index,
				  int kind_of_tile=agl_kind){

  // Reconstruct the beta values for this hit. Assumes direction as unitary
  const geant z=RICHDB::RICradpos()-RICHDB::rad_height-RICHDB::foil_height-
    RICradmirgap-RIClgdmirgap-RICHDB::rich_height;
  RichPMTChannel channel(_channel);
  geant x=channel.x();
  geant y=channel.y();

  betas[0]=0;
  betas[1]=0;
  betas[2]=0;
  _beta_hit[0]=betas[0];
  _beta_hit[1]=betas[1];
  _beta_hit[2]=betas[2];

  //---------------------------------------------------------
  
  // * Direct case: it uses the Newton algorithm to find the zero of the
  // function:
  //
  //  f(u)=R-h*u/sqrt(1-u**2)-H*u*n/sqrt(1-n**2*u**2)
  //
  // Being:
  //   u=sin( Cerenkov angle )
  //   R= Distance in the XY plane from teh emission point to the detection
  //      point
  //   h= Distance from the bottom of the radiator to the emision point in the
  //      Z axis
  //   H= Expansion length 
  //   n= radiator refractive index
  //
  
  
  // Compute the initial guess and auxiliar values
  
  geant R=sqrt((origin[0]-x)*(origin[0]-x)+(origin[1]-y)*(origin[1]-y));
  geant theta=atan2(number(R),fabs(origin[2]-z));
  geant h=origin[2]-RICHDB::RICradpos()+RICHDB::rad_height;
  geant H=RICHDB::rich_height+RICHDB::foil_height+
                       RICradmirgap+RIClgdmirgap
                       -RICHDB::foil_height;   // Correction due to high index


  geant n=index;
  
  geant u=fabs(sin(theta)/n);
  integer time_out=0;
  geant delta=1,f,g;

  while(fabs(delta)>1e-5 && time_out<5){
    // Solution does not exist, or it is not continuously reachable from
    // the starting point
    if(fabs(n*u)>=1 || fabs(u)>=1){u=-1; break;}

    f=R-h*u/sqrt(1-u*u)-
      H*u*n/sqrt(1-n*n*u*u);
    
    g=h*(u*u/((1-u*u)*sqrt(1-u*u))+1/sqrt(1-u*u))+
      H*n*(u*u*n*n/((1-n*n*u*u)*sqrt(1-n*n*u*u))+
	   1/sqrt(1-n*n*u*u));
    
    delta=f/g;
    u+=delta;
    time_out++;
  }

  if(u>=0 && u<=1){ // Theta has been reconstructed 
    geant phi=atan2(geant(y-origin[1]),geant(x-origin[0]));
    betas[0]=direction[0]*cos(phi)*u+
      direction[1]*sin(phi)*u-direction[2]*sqrt(1-u*u);
    betas[0]=1/index/betas[0];
    if(betas[0]<betamin){ // If the beta as direct is below threshold
      // Let the hot-spot detection algorithm deal with this
      betas[0]=0;
    }
    else if(betas[0]>betamax) betas[0]=-1;
  }else{
    betas[0]=0;    // Wrong theta
  }

#ifdef __AMSDEBUG__
  if(fabs(direction[2])<0.95){
    // Dump the information about the reconstruction
    cout<<"-----------------RECONSTRUCTING AS DIRECT "<<endl;
    cout<<" R= "<<R<<endl;
    cout<<" h= "<<h<<endl;
    cout<<" H= "<<H<<endl;
    cout<<" index= "<<index<<endl;
    cout<<" beta= "<<betas[0]<<endl;
    cout<<" origin[2] "<<origin[2]<<endl;
    cout<<" z "<<z<<endl;
    cout<<" delta "<<delta<<endl;
    cout<<" timeout "<<time_out<<endl;
    cout<<" U "<<u<<endl;
  }

  return;// TEST
#endif

  //------------------------------------------------------  

  integer j=1;
  AMSPoint puntos[8];

  integer max=reflexo(origin_ref,puntos);

  for(integer i=0;i<max;i++){
    AMSPoint dir;
    dir=puntos[i]-origin_ref;
    dir=dir/sqrt(dir[0]*dir[0]+dir[1]*dir[1]+dir[2]*dir[2]);

    geant phi=atan2(dir[1],dir[0]);
    theta=atan2(sqrt(dir[0]*dir[0]+dir[1]*dir[1]),dir[2]);
    u=sin(theta)/index;
    dir.setp(u*cos(phi),u*sin(phi),-sqrt(1-u*u));
    betas[j]=1/index/(dir[0]*direction_ref[0]+
				  dir[1]*direction_ref[1]+
				  dir[2]*direction_ref[2]);

    // CORRECTION ON THE RECONSTRUCTED INDEX
    if(kind_of_tile==agl_kind) betas[j]/=0.9993;
    if(kind_of_tile==naf_kind) betas[j]/=0.998;


    if(betas[j]<betamin) betas[j]=-2.;
    else
      if(betas[j]>betamax) betas[j]=-1.; 
      else 
	j++;
#ifndef __SAFE__
    if(j==3) return;
#endif    
  }

  // Fill the current beta hit 
  _beta_hit[0]=betas[0];
  _beta_hit[1]=betas[1];
  _beta_hit[2]=betas[2];
}


integer AMSRichRawEvent::reflexo(AMSPoint origin,AMSPoint *ref_point){
  geant false_height=RICHDB::bottom_radius*
    RICHDB::rich_height/(RICHDB::bottom_radius-RICHDB::top_radius); 


  double zk=(RICHDB::bottom_radius/false_height)*
    (RICHDB::bottom_radius/false_height);
  double c2=zk/(1+zk);
  double c=sqrt(c2);
  
  RichPMTChannel channel(_channel);
  double xf=channel.x();
  double yf=channel.y();

  double zf=-false_height-RIClgdmirgap;

  double x=origin[0],y=origin[1],
    z=origin[2]-RICHDB::RICradpos()+RICHDB::rad_height-false_height+
                (RICHDB::rich_height+RICHDB::foil_height
                   +RICradmirgap)
                -RICHDB::foil_height; // Correction due to high index


  AMSPoint initial(x,y,z),final(xf,yf,zf);

  // Put everything in local mirror coordinates
  initial=RichAlignment::RichToMirror(initial);
  final=RichAlignment::RichToMirror(final);
  x=initial[0]; y=initial[1]; z=initial[2];
  xf=final[0]; yf=final[1]; zf=final[2];

  double f1=x*xf-y*yf,
    f2=x*zf+z*xf,
    f3=x*yf+y*xf,
    f4=y*zf+z*yf;

  double a4=4*(f3*f3+f1*f1),
    a3=4*c*(f1*f2+f3*f4),
    a2=c2*(f2*f2+f4*f4)+4*(c2-1)*(f1*f1+f3*f3),
    a1=2*c*(c2-1)*(f3*f4+2*f1*f2),
    a0=(c2-1)*((c2-1)*f3*f3+c2*f2*f2);

  if(a4==0.) return 0;
  
  a3/=a4;
  a2/=a4;
  a1/=a4;
  a0/=a4;

  integer mt=0;
  double sols[4];
  
  SOLVE(a3,a2,a1,a0,sols,mt);

  //Count the number of solutions
  mt=0;
  for(integer i=0;i<4;i++) if(sols[i]!=0.) mt++;

  AMSPoint planes[8];
  AMSPoint this_plane;
  integer nsols=0;

  for(integer i=0;i<mt;i++){
    double b=1/(1+zk)-sols[i]*sols[i];
    if(b<0) continue;
    b=sqrt(b);
    for(integer j=0;j<2;j++){
      if(j) b*=-1;
      this_plane.setp(sols[i],b,c);

      if(dist(initial,this_plane)>0 && dist(final,this_plane)>0)
	planes[nsols++]=this_plane;
    }
  }

  integer good=0;
  for(integer i=0;i<nsols;i++){
    ref_point[good]=rpoint(initial,final,planes[i]);
    if((ref_point[good])[2]<initial[2] && (ref_point[good])[2]>-false_height
       && fabs(ref_point[good][0]*ref_point[good][0]+
	       ref_point[good][1]*ref_point[good][1]-zk*
	       ref_point[good][2]*ref_point[good][2])<1e-4){

      (ref_point[good]).setp((ref_point[good])[0],
			     (ref_point[good])[1],
			     (ref_point[good])[2]+false_height-
                              RICHDB::rich_height-RICHDB::foil_height-RICradmirgap-
                              RICHDB::rad_height+RICHDB::RICradpos());

      // Put it again in Rich coordinates
      ref_point[good]=RichAlignment::MirrorToRich(ref_point[good]);

      good++;
    }

  }

  return good;
}




// Default values... currently unused: filled during reconstruction
number AMSRichRing::_index=1.05;
number AMSRichRing::_height=3.;
AMSPoint AMSRichRing::_entrance_p=AMSPoint(0,0,0);
AMSDir   AMSRichRing::_entrance_d=AMSDir(0,0);
AMSPoint AMSRichRing::_emission_p=AMSPoint(0,0,0);
AMSDir   AMSRichRing::_emission_d=AMSDir(0,0);
geant   AMSRichRing::_clarity=0.0113;
geant   *AMSRichRing::_abs_len=0;
geant   *AMSRichRing::_index_tbl=0;

int     AMSRichRing::_kind_of_tile=0;
int     AMSRichRing::_tile_index=0;
geant AMSRichRing::_distance2border=0;

void AMSRichRing::getSobol(float &x,float &y,bool reset){
  const unsigned int MAXBIT=30;
  // Returns two quasi-random numbers for a 2-dimensional Sobel
  // sequence. Adapted from Numerical Recipies in C.

  unsigned int j, k, l;
  unsigned long i, im,ipp;

  // The following variables are "static" since we want their
  // values to remain stored after the function returns. These
  // values represent the state of the quasi-random number generator.

  static float fac;
  static int init=0;
  static unsigned long in, ix[3], *iu[2*MAXBIT+1];
  static unsigned long mdeg[3]={0,1,2};
  static unsigned long ip[3]={0,0,1};
  static unsigned long oiv[2*MAXBIT+1]=
    {0,1,1,1,1,1,1,3,1,3,3,1,1,5,7,7,3,3,5,15,11,5,15,13,9};
  static unsigned long iv[2*MAXBIT+1];
#pragma omp threadprivate(fac,init,in,ix,iu,mdeg,ip,oiv,iv)

  if(reset) init=0;

  // Initialise the generator the first time the function is called.

  if (!init) {
    init = 1;
    for(j=0;j<2*MAXBIT+1;j++) iv[j]=oiv[j];
    for (j = 1, k = 0; j <= MAXBIT; j++, k += 2) iu[j] = &iv[k];
    for (k = 1; k <= 2; k++) {
      for (j = 1; j <= mdeg[k]; j++) iu[j][k] <<= (MAXBIT-j);
      for (j = mdeg[k]+1; j <= MAXBIT; j++) {
        ipp = ip[k];
        i = iu[j-mdeg[k]][k];
        i ^= (i >> mdeg[k]);
        for (l = mdeg[k]-1; l >= 1; l--) {
          if (ipp & 1) i ^= iu[j-l][k];
          ipp >>= 1;
        }
        iu[j][k] = i;
      }
    }
    fac = 1.0/(1L << MAXBIT);
    in = 0;
    ix[0]=ix[1]=ix[2]=0;
  }
  // Now calculate the next pair of numbers in the 2-dimensional
  // Sobel sequence.

  im = in;
  for (j = 1; j <= MAXBIT; j++) {
    if (!(im & 1)) break;
    im >>= 1;
  }
  assert(j <= MAXBIT);
  im = (j-1)*2;
  ix[1] ^= iv[im+1];
  ix[2] ^= iv[im+2];
  x = ix[1] * fac;
  y = ix[2] * fac;
  in++;
}



void AMSRichRing::build(){
  _Start();
  const integer freq=10;
  static integer trig=0;
#pragma omp threadprivate(trig)
  trig=(trig+1)%freq;

  // Build all the tracks 
  AMSTrTrack *track;

  int j=0,k=0;
  for(int id=0;;){
    track=(AMSTrTrack *)AMSEvent::gethead()->getheadC("AMSTrTrack",id++,1);
    if(!track) break;

    for(;track;track=track->next()){
      AMSRichRing *ring=build(track,10);
      if(!ring) {ring=build(track,0);if(ring) ring->setstatus(dirty_ring);}
      if(ring) k++;j++;
      if(trig==0 && _NoMoreTime()){
	throw amsglobalerror(" AMSRichRing::build-E-Cpulimit Exceeded ");
      }

    }
  }
}

// DEFINE THIS AS 1 FOR DEBUG OUTPUT
#define __DEBUGP__ 0  

// New option, if cleanup==10 -> force cleaning and only store cleaned
// rings

float AMSRichRing::_window=9.0;


AMSRichRing* AMSRichRing::build(AMSTrTrack *track,int cleanup){
  // All these arrays are for speed up the reconstruction
  // They should be move to a dynamic list (like the containers)
  // using, for example, a structure (~completely public class)

  int ARRAYSIZE=(AMSEvent::gethead()->getC("AMSRichRawEvent",0))->getnelem();
  if(ARRAYSIZE==0) return 0;

  // Fast but not safe
  geant recs[RICmaxpmts*RICnwindows][3];
  geant mean[RICmaxpmts*RICnwindows][3];
  geant probs[RICmaxpmts*RICnwindows][3];
  integer size[RICmaxpmts*RICnwindows][3];
  integer mirrored[RICmaxpmts*RICnwindows][3];
  geant meanW[RICmaxpmts*RICnwindows][3];
  int sizeW[RICmaxpmts*RICnwindows][3];

  int bit=(AMSEvent::gethead()->getC("AMSRichRing",0))->getnelem();

  if(bit>16){
    //    cout<<"AMSRichRing::build -- Too many rings -- aborting "<<endl;
    return 0;
  }


  if(bit==int(crossed_pmt_bit)){
    //cout<<" AMSRichRing::build-too-many-tracks "<<endl;
    return 0;
  }

      
  //============================================================
  // Here we should check if the track goes through the radiator
  //============================================================

  
  RichRadiatorTileManager crossed_tile(track);
  
  if(crossed_tile.getkind()==empty_kind) return 0;
  
  _index=crossed_tile.getindex();
  _height=crossed_tile.getheight();
  _entrance_p=crossed_tile.getentrancepoint();
  _entrance_d=crossed_tile.getentrancedir();
  _clarity=crossed_tile.getclarity();
  _abs_len=crossed_tile.getabstable();
  _index_tbl=crossed_tile.getindextable();
  _kind_of_tile=crossed_tile.getkind();
  _tile_index=crossed_tile.getcurrenttile();
  _distance2border=crossed_tile.getdistance();
  
#ifdef _OPENMP
  if(__DEBUGP__){
    cerr<<" *** "<<omp_get_thread_num()<<"      INDEX CROSSED "<<_index<<endl;
    cerr<<" *** "<<omp_get_thread_num()<<"             height "<<_height<<endl;
    cerr<<" *** "<<omp_get_thread_num()<<"       ENTRANCE P   "<<_entrance_p[0]<<" "<<_entrance_p[1]<<" "<<_entrance_p[2]<<endl;
    cerr<<" *** "<<omp_get_thread_num()<<"       ENTRANCE D   "<<_entrance_d[0]<<" "<<_entrance_d[1]<<" "<<_entrance_d[2]<<endl;
  }
#endif

  //============================================================
  // PARAMETRISATION OF THE HIT ERROR
  // Obtained using the same conditions as the reconstruction 
  // height parametrisation. This depends in the light guides 
  // dimensions
  //============================================================
  
  
  // Parameters

  
  geant A=(-2.81+13.5*(_index-1.)-18.*
	   (_index-1.)*(_index-1.))*
    _height/(RICHDB::rich_height+RICHDB::foil_height+
			RICradmirgap+RIClgdmirgap)*40./2.;
  
  geant B=(2.90-11.3*(_index-1.)+18.*
	   (_index-1.)*(_index-1.))*
    _height/(RICHDB::rich_height+RICHDB::foil_height+
			RICradmirgap+RIClgdmirgap)*40./2.;

  // Fine tunning  
  A*=1.15;B*=1.15;


  if(_kind_of_tile==naf_kind) // For NaF they are understimated
    {A*=3.44;B*=3.44;}
  
  // Reconstruction threshold: maximum beta admited
  geant betamax=1.+5.e-2*(A+B);


  // Next obtained by Casaus: minimum beta admited to avoid noise caused
  // by the particle going through the PMTs
  // Value corrected by Carlos D.
  geant betamin=(1.+RICthreshold*(_index-1.))/_index;
  
  //==================================================
  // The reconstruction starts here
  //==================================================
  

  // Reconstructing it as direct


  AMSPoint dirp,refp;
  AMSDir   dird,refd;

  dirp=crossed_tile.getemissionpoint();
  dird=crossed_tile.getemissiondir();
  refp=crossed_tile.getemissionpoint(1);
  refd=crossed_tile.getemissiondir(1);

  _emission_p=dirp;
  _emission_d=dird;

  if(RICCONTROLFFKEY.tsplit)AMSgObj::BookTimer.start("RERICHHITS"); //DEBUG
  integer actual=0;
  AMSRichRawEvent *hitp[RICmaxpmts*RICnwindows];
  
  for(AMSRichRawEvent* hit=(AMSRichRawEvent *)AMSEvent::gethead()->
	getheadC("AMSRichRawEvent",0);hit;hit=hit->next()){
    if((hit->getchannelstatus()%10)!=Status_good_channel) continue;

    // Checks bounds
    if(actual>=RICmaxpmts*RICnwindows) {
      cerr << "AMSRichRing::build : Event too long."<<endl;
      break;
    }
    
    // Reconstruct one hit
    hit->reconstruct(dirp,refp,
		     dird,refd,
		     betamin,betamax,recs[actual],_index,_kind_of_tile);

    hit->unsetbit(bit);
    
    if(recs[actual][0]>0 || recs[actual][1]>0 || recs[actual][2]>0){	
      hitp[actual]=hit;
      actual++;

      if(actual>100 && actual%20==0 && _NoMoreTime()) throw amsglobalerror("AMSRichRing::build-E-Cpulimit Exceeded ");

    }
  }
  // Look for clusters
  if(RICCONTROLFFKEY.tsplit)AMSgObj::BookTimer.stop("RERICHHITS"); //DEBUG
  uinteger current_ring_status=_kind_of_tile==naf_kind?naf_ring:0;


  AMSRichRing *first_done=0;

  do{
    if(RICCONTROLFFKEY.tsplit)AMSgObj::BookTimer.start("RERICHBETA"); //DEBUG
    integer best_cluster[2]={0,0};
    geant best_prob=-1;

    for(integer i=0;i<actual;i++)
      for(integer j=0;j<3;j++)
	if(recs[i][j]>0){
	  mean[i][j]=recs[i][j];
	  size[i][j]=1;mirrored[i][j]=j>0?1:0;
	  probs[i][j]=0;
	  // Weighted reconstruction
	  int npe=hitp[i]->photoElectrons();
	  meanW[i][j]=recs[i][j]*npe;
	  sizeW[i][j]=npe;
	}
    
    
    
    for(integer i=0;i<actual;i++){
      if(i>50 && i%20==0 && _NoMoreTime()) throw amsglobalerror("AMSRichRing::build-E-Cpulimit Exceeded ");
      if(recs[i][0]==-2.) continue; // Jump if direct is below threshold
      if((cleanup && current_ring_status&dirty_ring) || (cleanup/10)%10) 
	if(hitp[i]->getbit(crossed_pmt_bit)) continue;

      for(integer k=0;k<3;k++){
	if(recs[i][k]<betamin) continue; 
	for(integer j=0;j<actual;j++){
	  if(recs[j][0]==-2.) continue;
	  if(i==j) continue;
	  if((cleanup && current_ring_status&dirty_ring) || (cleanup/10)%10)  
            if(hitp[j]->getbit(crossed_pmt_bit)) continue;

	  integer better=AMSRichRing::closest(recs[i][k],recs[j]);
	  
	  geant prob=(recs[i][k]-
		      recs[j][better])*
	    (recs[i][k]-
	     recs[j][better])/
	    AMSRichRing::Sigma(recs[i][k],A,B)/
	    AMSRichRing::Sigma(recs[i][k],A,B);
	  if(prob<_window){ //aprox. (3 sigmas)**2
	    probs[i][k]+=exp(-.5*prob);
	    mean[i][k]+=recs[j][better];
	    if(better>0) mirrored[i][k]++;
	    size[i][k]++;
	    
	    // Weighted reconstrution
	    int pe=hitp[j]->photoElectrons();
	    meanW[i][k]+=recs[j][better]*pe;
	    sizeW[i][k]+=pe;
	  }
	}

	if(best_prob<probs[i][k]){ 
	  best_prob=probs[i][k];	
	  best_cluster[0]=i;
	  best_cluster[1]=k;
	}
	
      }
    }
    uinteger cleaning=current_ring_status;
    current_ring_status&=~dirty_ring;

    if(best_prob>0){
      // This piece is a bit redundant: computes chi2 and weighted beta
      geant wsum=0,wbeta=0;      
      geant chi2=0.;
      geant beta_track=0;
      integer beta_used=0;
      integer mirrored_used=0;

      if(best_prob>0){
	beta_track=recs[best_cluster[0]][best_cluster[1]];
	
	for(integer i=0;i<actual;i++){
	  hitp[i]->unsetbit(bit);
	  if(recs[i][0]==-2.) continue;
	  if((cleanup && cleaning&dirty_ring)  || (cleanup/10)%10) 
	    if(hitp[i]->getbit(crossed_pmt_bit)) continue;
	  
	  integer closest=
	    AMSRichRing::closest(beta_track,recs[i]);
	  
	  if(recs[i][closest]<betamin) continue;
	  geant prob=(recs[i][closest]-beta_track)*
	    (recs[i][closest]-beta_track)/
	    AMSRichRing::Sigma(beta_track,A,B)/
	    AMSRichRing::Sigma(beta_track,A,B);
	  
	  if(prob>=_window) continue;
	  hitp[i]->setbit(bit);
	  if(cleanup) current_ring_status|=hitp[i]->getbit(crossed_pmt_bit)?dirty_ring:0; 
	  chi2+=prob;
	}
	beta_used=size[best_cluster[0]][best_cluster[1]];
	mirrored_used=mirrored[best_cluster[0]][best_cluster[1]];
	beta_track=mean[best_cluster[0]][best_cluster[1]]/geant(beta_used);
	wsum=sizeW[best_cluster[0]][best_cluster[1]];
	wbeta=meanW[best_cluster[0]][best_cluster[1]];
	
	if(wsum>0) wbeta/=wsum; else wbeta=0.;       
      }

      // Event quality numbers:
      // 0-> Number of used hits
      // 1-> chi2/Ndof
      
      // Fill the container
      if(RICCONTROLFFKEY.tsplit)AMSgObj::BookTimer.stop("RERICHBETA"); //DEBUG

      AMSRichRing* done=(AMSRichRing *)AMSEvent::gethead()->addnext(AMSID("AMSRichRing",0),
								    new AMSRichRing(track,
										    beta_used,
										    mirrored_used,
										    beta_track,
										    chi2/geant(beta_used-1),
										    wbeta,
										    recs[best_cluster[0]][best_cluster[1]],
										    recs,hitp,actual,bit,
										    current_ring_status,  //Status word
										    (RICRECFFKEY.recon[1]%10)
										    ));


#ifdef _OPENMP
      if(__DEBUGP__){
	cerr<<" *** "<<omp_get_thread_num()<<"             BETA   "<<beta_track<<endl;
	cerr<<" *** "<<omp_get_thread_num()<<"             USED   "<<beta_used<<endl;
      }
#endif
      if(!first_done) first_done=done;
      bit++;  
    } else {
      if(RICCONTROLFFKEY.tsplit)AMSgObj::BookTimer.stop("RERICHBETA"); //DEBUG
    }
  }while(current_ring_status&dirty_ring);   // Do it again if we want to clean it up once


  if(__DEBUGP__){
    cerr<<endl;
  }
  return first_done;
}

AMSRichRing* AMSRichRing::rebuild(AMSTrTrack *ptrack){

  AMSRichRing *ring=(AMSRichRing *)AMSEvent::gethead()->getheadC("AMSRichRing",0);
  if(ring && ring->_ptrack && ring->_ptrack->getpattern()>=0)return 0;
  ring=build(ptrack,10);if(!ring) ring=build(ptrack,0);
  return ring;


}



void AMSRichRing::_writeEl(){
  
#ifdef __WRITEROOT__
    AMSJob::gethead()->getntuple()->Get_evroot02()->AddAMSObject(this);
#endif
}
void AMSRichRing::_copyEl(){
#ifdef __WRITEROOT__
 if(PointerNotSet())return;
 RichRingR & ptr = AMSJob::gethead()->getntuple()->Get_evroot02()->RichRing(_vpos);
   if (_ptrack) ptr.fTrTrack= _ptrack->GetClonePointer();
   else ptr.fTrTrack=-1;

   // Add the hit used status
   for(AMSRichRawEvent* hit=(AMSRichRawEvent *)AMSEvent::gethead()->getheadC("AMSRichRawEvent",0);hit;hit=hit->next()){
     if((hit->getchannelstatus()%10)!=Status_good_channel) continue;

     if((hit->getstatus()>>_vpos)%2){
       ptr.fRichHit.push_back(hit->GetClonePointer());   
     }
   }
#endif
}
void AMSRichRing::CalcBetaError(){
  geant A=(-2.81+13.5*(_index-1.)-18.*
	   (_index-1.)*(_index-1.))*
    _height/(RICHDB::rich_height+RICHDB::foil_height+
                                   RICradmirgap+RIClgdmirgap)*40./2.;
      
  geant B=(2.90-11.3*(_index-1.)+18.*
	   (_index-1.)*(_index-1.))*
    _height/(RICHDB::rich_height+RICHDB::foil_height+
                                   RICradmirgap+RIClgdmirgap)*40./2.;


  if(_kind_of_tile==naf_kind){A*=3.44;B*=3.44;}


  _errorbeta=_used>0?
    sqrt(AMSRichRing::Sigma(_beta,A,B)*
	 AMSRichRing::Sigma(_beta,A,B)/geant(_used)+0.016e-2*0.016e-2):1;
}


////////////////////////////////////
/// charge reconstruction
/// CREDITS TO JORGE CASAUS AND ELISA LANCIOTTI and moi

#define PI 3.14159265359
#define SQR(x) ((x)*(x))
#define ESC(x,y) ((x)[0]*(y)[0]+(x)[1]*(y)[1]+(x)[2]*(y)[2])
#define MOD(x) sqrt(ESC(x,x))


geant AMSRichRing::_Time=0;

void AMSRichRing::ReconRingNpexp(geant window_size,int cleanup){ // Number of sigmas used 
  AMSPoint local_pos=_entrance_p;
  AMSDir   local_dir=_entrance_d;

  // Cleanup arrays containig PMT by PMT information
  for(int i=0;i<680;i++) NpColPMT[i]=NpExpPMT[i]=0;

  local_pos[2]=RICHDB::rad_height-_height;

  // Protect against unexpected sign in _entrance_d 
  if(local_dir[2]>0){
    local_dir[0]*=-1;
    local_dir[1]*=-1;
  }else local_dir[2]*=-1;
  //  local_dir[2]*=-1;

  const integer NSTP=70; // Z=1 optimized
  const geant dphi=2*PI/NSTP;


  geant dfphi[NSTP],dfphih[NSTP];
  geant hitd[RICmaxpmts*RICnwindows],hitp[RICmaxpmts*RICnwindows];
  AMSRichRawEvent *used_hits[RICmaxpmts*RICnwindows];

  geant unused_hitd[RICmaxpmts*RICnwindows];
  AMSRichRawEvent *unused_hits[RICmaxpmts*RICnwindows];

  for(int i=0;i<10;i++){
    _collected_hits_window[i]=0;
    _collected_pe_window[i]=0.0;
  }



  for(int i=0;i<NSTP;i++)
    dfphi[i]=dfphih[i]=0;

  // For the probkl stuff
  integer nh=0,nu=0;
  geant dmax=0.;
  integer nh_unused=0;

  for(AMSRichRawEvent* hit=(AMSRichRawEvent *)AMSEvent::gethead()->
	getheadC("AMSRichRawEvent",0);hit;hit=hit->next()){
    if(hit->getbit((AMSEvent::gethead()->getC("AMSRichRing",0))->
		   getnelem())){
      used_hits[nh]=hit;
      hitd[nh++]=1.e5;
    }else
      if(!(hit->getbit(crossed_pmt_bit))){
	unused_hits[nh_unused]=hit;
	unused_hitd[nh_unused++]=1.e5;
      }
  }
  

  geant l,phi;
  geant efftr,xb,yb,lentr,lfoil,lguide,geftr,
    reftr,beftr,ggen,rgen,bgen;
  geant nexp,nexpg,nexpr,nexpb;
  geant dfnrm=0,dfnrmh=0;
  integer tflag;

  AMSPoint r;

  l=_height/local_dir[2];
  integer i,j,k;

#define SOBOL
#ifdef SOBOL
  // COMPUTE THE NUMBER OF COLLECTED FOR A FIRST CHARGE ESTIMATE
  _collected_npe=0.;
  {
    geant A=(-2.81+13.5*(_index-1.)-18.*
	     (_index-1.)*(_index-1.))*
      _height/(RICHDB::rich_height+RICHDB::foil_height+
	       RICradmirgap+RIClgdmirgap)*40./2.;
    
    geant B=(2.90-11.3*(_index-1.)+18.*
	     (_index-1.)*(_index-1.))*
      _height/(RICHDB::rich_height+RICHDB::foil_height+
	       RICradmirgap+RIClgdmirgap)*40./2.;
    
    if(_kind_of_tile==naf_kind) // For NaF they are understimated
      {A*=3.44;B*=3.44;}
    
    geant sigma=Sigma(_beta,A,B);

    for(AMSRichRawEvent* hit=(AMSRichRawEvent *)AMSEvent::gethead()->
	  getheadC("AMSRichRawEvent",0);hit;hit=hit->next()){  
      if((hit->getchannelstatus()%10)!=Status_good_channel) continue;
      geant betas[3];
      for(int n=0;n<3;n++) betas[n]=hit->getbeta(n);
      geant value=fabs(_beta-betas[closest(_beta,betas)])/sigma;

      if(value<window_size){
	_collected_npe+=(cleanup && hit->getbit(crossed_pmt_bit))?0:hit->getnpe();
      }
    }
  }


  double prev=-HUGE_VAL;
  unsigned int counter=0;
  getSobol(l,phi,true);  // The first is only for initialization 
  nexp=nexpg=nexpr=nexpb=0;

  int limit=4;
  for(int step=0;step<100 && step<limit;step++){
    // Each computation step adds 100 points to the integral  
    for(int i=0;i<100;i++){
      l*=_height/local_dir[2];
      phi*=2*M_PI;
      r=local_pos+local_dir*l; // Move to the emission point
      counter++;

      efftr=trace(r,local_dir,phi,&xb,&yb,&lentr,
		  &lfoil,&lguide,&geftr,&reftr,&beftr,&tflag);

#pragma omp critical (richrec1)
      if(geftr){
	float cnt=generated(lentr,lfoil,lguide,
			    &ggen,&rgen,&bgen);
	

	int phistep=floor(phi*NSTP/2.0/M_PI);
	if(phistep>=NSTP) {cout<<"BIG ERROR IN PHISTEP"<<endl;phistep=NSTP-1;}
	dfphi[phistep]+=efftr*cnt;
	
	nexp+=efftr*cnt;
	nexpg+=geftr*ggen;
	nexpr+=reftr*rgen;
	nexpb+=beftr*bgen;

	// Add the same quantity per PMT
	int channel=RichPMTsManager::FindChannel(xb,yb);
	int pmt,pixel;  RichPMTsManager::UnpackGeom(channel,pmt,pixel);
	if(pmt>-1) NpExpPMT[pmt]+=efftr*cnt;

      }

      for(k=0;k<nh_unused;k++){
	geant d=sqrt(SQR(xb-unused_hits[k]->getpos(0))+
		     SQR(yb-unused_hits[k]->getpos(1)));
	if(d<unused_hitd[k])  unused_hitd[k]=d;
      }


      for(k=0;k<nh;k++){
	geant d=sqrt(SQR(xb-used_hits[k]->getpos(0))+
		     SQR(yb-used_hits[k]->getpos(1)));
	if(d<hitd[k]){
	  hitd[k]=d;
	  hitp[k]=phi;
	}
      }
      getSobol(l,phi);  
    }

    if(nexp==0) continue; // The computation failed miserably:try again

    double currentValue=nexp*_height/local_dir[2]/counter;
    double delta=fabs(currentValue-prev)*2/currentValue; // Overestimated error
    double charge=_collected_npe/currentValue;
    if(charge<1) charge=1;
    double errorEstimate=(0.2*0.2*4-1.36/currentValue)/charge;
    if(errorEstimate<1e-4/charge) errorEstimate=1e-4/charge;
#ifdef __AMSDEBUG__
    cout<<"SOBOL INTEGRATION STEP "<<step<<endl
	<<"      NEXP "<<nexp*_height/local_dir[2]/counter<<" FOR NUMBER OF POINTS "<<counter<<endl
	<<"      CURRENT VALUE "<<currentValue<<"   previous "<<prev<<" Error2 estimate "<<delta*delta<<endl
	<<"      CURRENT CHARGE "<<charge<<endl
	<<"      REQUIRED ERROR2 BOUND "<<errorEstimate<<endl 
	<<"      CURRENT LIMIT "<<limit<<endl; 
    
    if(errorEstimate<0){
      cout<<"BIG PROBLEM WITH THE ERROR ESTIMATE"<<endl;
    }
#endif

    if(delta*delta>errorEstimate) limit=step+4;
    prev=currentValue;
  }

  nexp*=_height/local_dir[2]/counter;
  nexpg*=_height/local_dir[2]/counter;
  nexpr*=_height/local_dir[2]/counter;
  nexpb*=_height/local_dir[2]/counter;
  for(phi=0,i=0;phi<2*PI;phi+=dphi,i++){
    dfphi[i]*=_height/local_dir[2]/counter;
  }
  for(int i=0;i<680;NpExpPMT[i++]*=_height/local_dir[2]/counter);

#else
  geant dL=l/NSTL;

  for(nexp=0,nexpg=0,nexpr=0,nexpb=0,j=0;j<NSTL;j++){
    l=(j+.5)*dL;

    r=local_pos+local_dir*l;
    

    for(phi=0,i=0;phi<2*PI;phi+=dphi,i++){

      efftr=trace(r,local_dir,phi,&xb,&yb,&lentr,
		  &lfoil,&lguide,&geftr,&reftr,&beftr,&tflag);
	
#pragma omp critical (richrec1)
      if(geftr){
	float cnt=generated(lentr,lfoil,lguide,
			    &ggen,&rgen,&bgen)*dL/NSTP;


	dfphi[i]+=efftr*cnt;

	nexp+=efftr*cnt;
	nexpg+=geftr*dL/NSTP*ggen;
	nexpr+=reftr*dL/NSTP*rgen;
	nexpb+=beftr*dL/NSTP*bgen;
      }

      for(k=0;k<nh_unused;k++){
	geant d=sqrt(SQR(xb-unused_hits[k]->getpos(0))+
		     SQR(yb-unused_hits[k]->getpos(1)));
	if(d<unused_hitd[k])  unused_hitd[k]=d;
      }


      for(k=0;k<nh;k++){
	geant d=sqrt(SQR(xb-used_hits[k]->getpos(0))+
		     SQR(yb-used_hits[k]->getpos(1)));
	if(d<hitd[k]){
	  hitd[k]=d;
	  hitp[k]=phi;
	}
      }
    }
  }

#endif


#ifdef __AMSDEBUG__
  HF1(123000,nexpg,1.);
  HF1(123001,nexpr,1.);
  HF1(123002,nexpb,1.);
  HF1(123003,nexp,1.);
#endif



  for(i=0;i<nh;i++){
    if(hitd[i]<1.){
      nu++;
      j=int(hitp[i]/dphi);
      dfphih[j]+=used_hits[i]->getnpe();
    }
  }

  for(i=0;i<NSTP;i++){
    dfnrm+=dfphi[i];
    dfnrmh+=dfphih[i];
  }
  
  // Compute the spread on azimuthal distance for used hits
  Double_t sum2=0;
  for(k=0;k<nh;k++)
    sum2+=(hitp[k]-hitp[0])*(hitp[k]-hitp[0]);
  if(nh>0) _phi_spread=sum2/nh; else _phi_spread=0;

  // Compute the mean of the distribution of 1/distance**2 for not used hits
  sum2=0;
  for(k=0;k<nh_unused;k++)
    if(unused_hitd[k]>0)
      sum2+=1/unused_hitd[k]/unused_hitd[k];
  _unused_dist=sum2;

  // Compute Kullbak distance
  double KullbackD=0;
  if(dfnrmh)
    for(int i=0;i<NSTP;i++){
      if(!dfphih[i]) continue;
      if(!dfphi[i]){
	KullbackD=1e5;
	break;
      }
      KullbackD+=dfphih[i]/dfnrmh*log(dfphih[i]*dfnrm/dfnrmh/dfphi[i])+
	0.5/dfnrmh*log(2*PI*dfphih[i]);  // Correction for low n
      //      KullbackD+=dfphi[i]/dfnrm*log(dfphi[i]*(dfnrmh+1)/dfnrm/(dfphih[i]+1.0/NSTP));
    }else KullbackD=1e5;

  if(KullbackD!=1e5) KullbackD-=0.5/dfnrmh*log(2*PI*dfnrmh); // Correction for low n
  _kdist=KullbackD;


  if(dfnrm>0 && dfnrmh>0){
    dfphi[0]/=dfnrm;
    dfphih[0]/=dfnrmh;
    for(i=1;i<NSTP;i++){
      dfphi[i]=dfphi[i-1]+dfphi[i]/dfnrm;
      dfphih[i]=dfphih[i-1]+dfphih[i]/dfnrmh;
      if(fabs(dfphih[i]-dfphi[i])>dmax) 
      	dmax=fabs(dfphih[i]-dfphi[i]);
    }
  } else dmax=1;


  // Probability for the ring
  if(nu){
    float z=sqrt(geant(nu))*dmax;
    _probkl=PROBKL(z);
  }
  else _probkl=0;
  
  // Charge related variables
  _npexp=nexp;

  _npexpg=nexpg;
  _npexpr=nexpr;
  _npexpb=nexpb;

  // Correction for the MC using the explicit loss function
  if(_kind_of_tile!=naf_kind && AMSJob::gethead()->isMCData()){
    _npexp*=1-RICHDB::scatloss;
    _npexpg*=1-RICHDB::scatloss;
    _npexpr*=1-RICHDB::scatloss;
    _npexpb*=1-RICHDB::scatloss;
    for(int i=0;i<680;NpExpPMT[i++]*=1-RICHDB::scatloss);
  }


  _collected_npe=0.;
  _collected_npe_lkh=0;  

  geant A=(-2.81+13.5*(_index-1.)-18.*
	   (_index-1.)*(_index-1.))*
    _height/(RICHDB::rich_height+RICHDB::foil_height+
                                   RICradmirgap+RIClgdmirgap)*40./2.;
      
  geant B=(2.90-11.3*(_index-1.)+18.*
	   (_index-1.)*(_index-1.))*
    _height/(RICHDB::rich_height+RICHDB::foil_height+
                                   RICradmirgap+RIClgdmirgap)*40./2.;

  if(_kind_of_tile==naf_kind) // For NaF they are understimated
    {A*=3.44;B*=3.44;}

  geant sigma=Sigma(_beta,A,B);

  for(AMSRichRawEvent* hit=(AMSRichRawEvent *)AMSEvent::gethead()->
	getheadC("AMSRichRawEvent",0);hit;hit=hit->next()){  
    if((hit->getchannelstatus()%10)!=Status_good_channel) continue;
    geant betas[3];
    for(int n=0;n<3;n++) betas[n]=hit->getbeta(n);
    geant value=fabs(_beta-betas[closest(_beta,betas)])/sigma;

    for(int ii=0;ii<10;ii++)
      if(value<ii+1){
	_collected_hits_window[ii]+=(cleanup && hit->getbit(crossed_pmt_bit))?0:1;
	_collected_pe_window[ii]+=(cleanup && hit->getbit(crossed_pmt_bit))?0:hit->getnpe();
      }



    if(value<window_size){
      // Computation per PMT 
      int pmt,pixel;
      RichPMTsManager::UnpackGeom(hit->getchannel(),pmt,pixel);
      if(pmt>-1) NpColPMT[pmt]+=(cleanup && hit->getbit(crossed_pmt_bit))?0:hit->getnpe();
      
      // General computation
      _collected_npe+=(cleanup && hit->getbit(crossed_pmt_bit))?0:hit->getnpe();
      _collected_npe_lkh+=(cleanup && hit->getbit(crossed_pmt_bit))?0:hit->photoElectrons();
    }
  }
}


geant AMSRichRing::trace(AMSPoint r, AMSDir u, 
			 geant phi, geant *xb, geant *yb, 
			 geant *lentr, geant *lfoil, 
			 geant *lguide, geant *geff, 
			 geant *reff, geant *beff, 
			 integer *tflag,float beta_gen)
{
  geant r0[3],u0[3],r1[3],u1[3],r2[3],u2[3],r3[3],n[3];
  geant cc,sc,cp,sp,cn,sn,f,l,a,b,c,d,rbase;
  int i;
  const geant exp_len=RICHDB::rich_height+RICradmirgap+RIClgdmirgap; 
  const geant kc=(RICHDB::bottom_radius-RICHDB::top_radius)/RICHDB::rich_height;
  const geant ac=RICHDB::rad_height+RICHDB::foil_height+RICradmirgap-RICHDB::top_radius/kc;
  const geant mir_eff=RICmireff;


  *xb=0;
  *yb=0;
  *lentr=0;
  *lfoil=0;
  *lguide=0;
  *geff=0;
  *reff=0;
  *beff=0;
  *tflag=-1;

  cc=1./_beta/_index;
  sc=sqrt(1-SQR(cc));
  cp=cos(phi);
  sp=sin(phi);
  f=sqrt(SQR(u[0])+SQR(u[1]));

  /* initial values*/
  for(i=0;i<3;i++) r0[i]=r[i];

  if(f>0){
    u0[0]=sc/f*(sp*u[0]*u[2]+cp*u[1])+cc*u[0];
    u0[1]=sc/f*(sp*u[1]*u[2]-cp*u[0])+cc*u[1];
    u0[2]=-f*sc*sp+u[2]*cc;}
  else{
    u0[0]=sc*cp;
    u0[1]=sc*sp;
    u0[2]=cc;}

  if(tile(r0)!=-1)*geff=1;

  /* propagate to radiator end */
  l=(RICHDB::rad_height-r0[2])/u0[2];


  for(i=0;i<3;i++) r1[i]=r0[i]+l*u0[i];

  // Check if the ray crossed the tiles-wall.

  if (tile(r0)==-1 || tile(r1)!=tile(r0)){
    *tflag=6;
    return 0;
  }
  *lentr=l;

  /* exit radiator volume */
  n[0]=0;
  n[1]=0;
  n[2]=1;
  cn=ESC(u0,n);
  sn=sqrt(1-SQR(cn));
  if (RICHDB::foil_height>0){
  /* radiator -> foil */

    if (_index*sn>RICHDB::foil_index){  // Check total reflection
      *tflag=0;
      return 0;
    }
    f=sqrt(1-SQR(_index/RICHDB::foil_index*sn))-_index/RICHDB::foil_index*cn;
    for(i=0;i<3;i++) u1[i]=_index/RICHDB::foil_index*u0[i]+f*n[i];

  /* propagate to foil end */
    l=RICHDB::foil_height/u1[2];
    for(i=0;i<3;i++) r1[i]=r1[i]+l*u1[i];

    if (sqrt(SQR(r1[0])+SQR(r1[1]))>RICHDB::top_radius){
      *tflag=2;
      return 0;
    }
    *lfoil=l;
  /* foil -> vacuum */
    cn=ESC(u1,n);
    sn=sqrt(1-SQR(cn));
    if (RICHDB::foil_index*sn>1){  // Check total reflexion
      *tflag=0;
      return 0;
    }
    f=sqrt(1-SQR(RICHDB::foil_index*sn))-RICHDB::foil_index*cn;
    for(i=0;i<3;i++) u1[i]=RICHDB::foil_index*u1[i]+f*n[i];}
  else{
  /* radiator -> vacuum */
    if (_index*sn>1){
      *tflag=0;
     return 0;
    }
    f=sqrt(1-SQR(_index*sn))-_index*cn;
    for(i=0;i<3;i++) u1[i]=_index*u0[i]+f*n[i];}

  *reff=1;
  
  
  /* propagate to top of mirror */
  l=RICradmirgap/u1[2];
  for(i=0;i<3;i++) r1[i]+=l*u1[i];
  rbase=sqrt(SQR(r1[0])+SQR(r1[1]));
  if (rbase>RICHDB::top_radius){   
    *tflag=1;
    return 0;
  }
  

  
  /* propagate to base of mirror*/
  l=RICHDB::rich_height/u1[2];
  
  for(i=0;i<3;i++) r2[i]=r1[i]+l*u1[i];

  
  /* hole, direct or reflected */
  rbase=sqrt(SQR(r2[0])+SQR(r2[1]));
  
  
  if (rbase<RICHDB::bottom_radius){   // All right
    // Propagate to end
    l=RIClgdmirgap/u1[2];
    for(i=0;i<3;i++) r2[i]+=l*u1[i];
    *xb=r2[0];
    *yb=r2[1];

    // Take into account status and efficiency
    int channel_number=RichPMTsManager::FindChannel(r2[0],r2[1]);
    if(channel_number<0){
      *beff=0;
    }else{
      int pmt,channel;
      RichPMTsManager::UnpackGeom(channel_number,pmt,channel);
      int status=RichPMTsManager::Status(pmt,channel) && RichPMTsManager::Mask(pmt,channel);
      if((status%10)!=Status_good_channel || AMSRichRawEvent::CrossedPMT(pmt)){
	*beff=0;
      }else{
	*beff=RichPMTsManager::Eff(pmt,channel);
      }
    }
    //    *beff=RichPMTsManager::FindPMT(r2[0],r2[1])<0?0:1;

    *tflag=*beff?3:5;
    return (*beff)*lgeff(r2,u1,lguide); 
  }
  else{   // It intersects the mirror
    a=1-(SQR(kc)+1)*SQR(u1[2]);
    b=2*(r1[0]*u1[0]+r1[1]*u1[1]-SQR(kc)*(r1[2]-ac)*u1[2]);
    c=SQR(r1[0])+SQR(r1[1])-SQR(kc*(r1[2]-ac));
    d=SQR(b)-4*a*c;
    if(d<0){
#ifdef __AMSDEBUG__
      printf("AMSRichRing::trace Crossing Point not found\n");
      printf(" kc %f, ac %f\n",kc,ac);
      printf(" a %f, b %f, c %f\n",a,b,c);
#endif
      return 0;}
    l=(-b+sqrt(d))/2./a;
    if(l<0){
#ifdef __AMSDEBUG__
      printf("AMSRichRing::trace Crossing Point negative \n");
      printf(" kc %f, ac %f\n",kc,ac);
      printf(" a %f, b %f, c %f\n",a,b,c);
      printf(" l %f\n",l);
#endif
      return 0;}
    
    for(i=0;i<3;i++) r2[i]=r1[i]+l*u1[i];

    double sector=atan2(r2[1],r2[0])*180/M_PI+M_PI; // Sector of the mirror in deg between 0 and 2 PI
    
    f=1./sqrt(1+SQR(kc));
    n[0]=-f*r2[0]/sqrt(SQR(r2[0])+SQR(r2[1]));
    n[1]=-f*r2[1]/sqrt(SQR(r2[0])+SQR(r2[1]));
    n[2]=f*kc;
    
    f=2*ESC(u1,n);
    for(i=0;i<3;i++) u2[i]=u1[i]-f*n[i];
    
    l=(exp_len+RICHDB::rad_height+RICHDB::foil_height-r2[2])/u2[2];


    for(i=0;i<3;i++) r3[i]=r2[i]+l*u2[i];
    rbase=sqrt(SQR(r3[0])+SQR(r3[1]));

    if(rbase>RICHDB::bottom_radius){
      *tflag=8;
      return 0;
    }


    *xb=r3[0];
    *yb=r3[1];
    
    int channel_number=RichPMTsManager::FindChannel(r3[0],r3[1]);
    if(channel_number<0){
      *beff=0;
    }else{
      int pmt,channel;
      RichPMTsManager::UnpackGeom(channel_number,pmt,channel);
      int status=RichPMTsManager::Status(pmt,channel) && RichPMTsManager::Mask(pmt,channel);
      if((status%10)!=Status_good_channel || AMSRichRawEvent::CrossedPMT(pmt)){
	*beff=0;
      }else{
	*beff=RichPMTsManager::Eff(pmt,channel)*mir_eff;
	// Take into account change in reflectivity
	if(sector<RICHDB::RICmirrors1_s3 && sector>RICHDB::RICmirrors2_s1)
	  *beff*=RICmireffs1/RICmireff;
      }
    }
    *tflag=*beff?4:5;
    return *beff*lgeff(r3,u2,lguide);
    
  }
  
  
}



int AMSRichRing::tile(AMSPoint r){ // Check if a track hits the radator support struycture
  integer tile=RichRadiatorTileManager::get_tile_number(r[0],r[1]);
  if(RichRadiatorTileManager::get_tile_kind(tile)==empty_kind) return -1;
  // Next should be guaranteed by RichRadiatorTileManager
  //  if(fabs(RichRadiatorTileManager::get_tile_x(tile)-r[0])>RICHDB::rad_length/2.-RICaethk/2.
  //     ||fabs(RichRadiatorTileManager::get_tile_y(tile)-r[1])>RICHDB::rad_length/2.-RICaethk/2.) return 0;
  return tile;
}



// Several static arrays within generated moved to static members of AMSRichRing
float AMSRichRing::_l[RICmaxentries][_TILES_]; 
float AMSRichRing::_r[RICmaxentries][_TILES_]; 
float AMSRichRing::_a[RICmaxentries][_TILES_]; 
float AMSRichRing::_b[RICmaxentries][_TILES_]; 
float AMSRichRing::_g[RICmaxentries][_TILES_]; 
float AMSRichRing::_t[RICmaxentries][_TILES_];
float AMSRichRing::_effg[_NRAD_][_TILES_]; 
float AMSRichRing::_ring[_NRAD_][_TILES_];
float AMSRichRing::_effr[_NRAD_][_NFOIL_][_TILES_]; 
float AMSRichRing::_rinr[_NRAD_][_NFOIL_][_TILES_];
float AMSRichRing::_effb[_NRAD_][_NFOIL_][_TILES_]; 
float AMSRichRing::_rinb[_NRAD_][_NFOIL_][_TILES_];
float AMSRichRing::_effd[_NRAD_][_NFOIL_][_NGUIDE_][_TILES_]; 
float AMSRichRing::_rind[_NRAD_][_NFOIL_][_NGUIDE_][_TILES_];
int AMSRichRing::_generated_initialization=1;
int AMSRichRing::_first_radiator_call[_TILES_];


float AMSRichRing::generated(geant length,
			   geant lfoil,
			   geant lguide,
			   geant *fg,
			   geant *fr,
			   geant *fb){

  const int NRAD=_NRAD_;
  const int NFOIL=_NFOIL_;
  const int NGUIDE=_NGUIDE_;
  const float ALPHA=0.0072973530764; 
  const float k=2*PI*ALPHA;
  const float tl=4*RICHDB::foil_index/SQR(1+RICHDB::foil_index);
#ifdef __AMSDEBUG__
  const float abslref=(RICHDB::lg_abs[0]+RICHDB::lg_abs[1])/2;
#endif
  const float factor=1.;
  const int ENTRIES=RICmaxentries;

  float rmn=0,rmx=2.0*_height;
  float fmn=RICHDB::foil_height,fmx=1.5*RICHDB::foil_height;
  float gmn=RICHDB::lg_height,gmx=1.7*RICHDB::lg_height;
  int i,lr,lf,lg,nf;
  float f=0.;

  if(_generated_initialization){for(int i=0;i<_TILES_;i++) _first_radiator_call[i]=1;_generated_initialization=0;}

  if(_kind_of_tile==naf_kind){
    rmx=6.0*_height;
    fmx=3.0*RICHDB::foil_height;}

  if(_first_radiator_call[_tile_index]){
    
    _first_radiator_call[_tile_index]=0;
#ifdef __AMSDEBUG__
    printf("\nLight Guide Absorption Parameter\n"
             "--------------------------------\n"
             "       Ref. AbsLength :  %f\n",
             abslref);
#endif
    for(i=0;i<ENTRIES-1;i++){
      float dl=1.e-3*(RICHDB::wave_length[i]-RICHDB::wave_length[i+1]);
      float q=1.e-2*(RICHDB::eff[i]+RICHDB::eff[i+1])/2;
      _l[i][_tile_index]=1.e-3*(RICHDB::wave_length[i]+RICHDB::wave_length[i+1])/2;
      _r[i][_tile_index]=(_index_tbl[i]+_index_tbl[i+1])/2;                // Here enters the refractive index: should be one per tile at least
      _a[i][_tile_index]=(_abs_len[i]+_abs_len[i+1])/2;
      _b[i][_tile_index]=(RICHDB::lg_abs[i]+RICHDB::lg_abs[i+1])/2;
      _g[i][_tile_index]=q*dl/SQR(_l[i][_tile_index]);
      _t[i][_tile_index]=4*_r[i][_tile_index]/SQR(1+_r[i][_tile_index]);
      //      if(RICHDB::foil_height>0) _t[i][_tile_index]*=RICHDB::foil_index*(1+_r[i][_tile_index])/(_r[i][_tile_index]+SQR(RICHDB::foil_index));
      if(RICHDB::foil_height>0) _t[i][_tile_index]*=SQR(4*RICHDB::foil_index/SQR(1+RICHDB::foil_index)); // There are two refractions: entering and exiting
    }
    nf=RICHDB::foil_height>0?NFOIL:1;
#ifdef __AMSDEBUG__
    printf("\nTabulating Effective no. Photons & Refractive Index\n"
             "---------------------------------------------------\n"
             "      %4d bins in Radiator Thickness\n"
             "      %4d bins in Foil     Thickness\n"
             "      %4d bins in LGuide   Thickness\n",
            NRAD,nf,NGUIDE);
#endif

#define F(x) (1.0/SQR((x)))

    for(lr=0;lr<NRAD;lr++){
     float rl=rmn+lr*(rmx-rmn)/NRAD;
     _effg[lr][_tile_index]=0;
     _ring[lr][_tile_index]=0;
     for(lf=0;lf<nf;lf++){
      float fl=fmn+lf*(fmx-fmn)/nf;
      _effr[lr][lf][_tile_index]=0;
      _rinr[lr][lf][_tile_index]=0;
      _effb[lr][lf][_tile_index]=0;
      _rinb[lr][lf][_tile_index]=0;
      for(lg=0;lg<NGUIDE;lg++){
       _effd[lr][lf][lg][_tile_index]=0;
       _rind[lr][lf][lg][_tile_index]=0;
       for(i=0;i<ENTRIES-1;i++){
         float cr=1./exp(rl*_clarity/SQR(SQR(_l[i][_tile_index])));     // Here enters the clarity
         float ar=1./exp(rl/_a[i][_tile_index]);
         float af=1./exp(fl/_b[i][_tile_index]);
	 //         float al=1./exp(gl*(1./_b[i][_tile_index]-1./abslref));
#ifndef __RICH_SINGLE_LG_TABLE__
         float al=1./exp(gl*(1./_b[i][_tile_index]));
#else
	 float al=1.0;
#endif
         _effd[lr][lf][lg][_tile_index]+=_g[i][_tile_index]*_t[i][_tile_index]*cr*ar*af*al;
         _rind[lr][lf][lg][_tile_index]+=F(_r[i][_tile_index])*_g[i][_tile_index]*_t[i][_tile_index]*cr*ar*af*al;
         if(!lg){
          if(!lf){
           _effg[lr][_tile_index]+=_g[i][_tile_index];
           _ring[lr][_tile_index]+=F(_r[i][_tile_index])*_g[i][_tile_index];}
          _effr[lr][lf][_tile_index]+=_g[i][_tile_index]*_t[i][_tile_index]*cr*ar*af;
          _rinr[lr][lf][_tile_index]+=F(_r[i][_tile_index])*_g[i][_tile_index]*_t[i][_tile_index]*cr*ar*af;
          _effb[lr][lf][_tile_index]+=_g[i][_tile_index]*_t[i][_tile_index]*cr*ar*af*tl;
          _rinb[lr][lf][_tile_index]+=F(_r[i][_tile_index])*_g[i][_tile_index]*_t[i][_tile_index]*cr*ar*af*tl;}
       }
       _rind[lr][lf][lg][_tile_index]/=_effd[lr][lf][lg][_tile_index];
      }
      _rinr[lr][lf][_tile_index]/=_effr[lr][lf][_tile_index];
      _rinb[lr][lf][_tile_index]/=_effb[lr][lf][_tile_index];
     }
     _ring[lr][_tile_index]/=_effg[lr][_tile_index];
    }
#ifdef __AMSDEBUG__
    printf("....End of Tables!\n\n");
#endif
  }

  lr=int((floor)(NRAD*(length-rmn)/(rmx-rmn)+.5));

  if(lr>NRAD-1){
#ifdef __AMSDEBUG__
    static char guard=0;
    if(!guard){
      printf("AMSRichRing::generated WARNING: length too big %f\nMore warning messages suppressed\n",length);
      guard=1;
    }
#endif
    lr=NRAD-1;}
  else if(lr<0)lr=0;

  lf=RICHDB::foil_height>0?int((floor)(NFOIL*(lfoil-fmn)/(fmx-fmn)+.5)):0;

  if(lf>NFOIL-1){
#ifdef __AMSDEBUG__
    static char guard=0;
    if(!guard){
      printf("AMSRichRing::generated WARNING: lfoil  too big %f\nMore warning messages suppressed\n",lfoil);
      guard=1;
    }
#endif
    lf=NFOIL-1;}
  else if(lf<0)lf=0;
  lg=int((floor)(NGUIDE*(lguide-gmn)/(gmx-gmn)+.5));


  if(lg>NGUIDE-1){
#ifdef __AMSDEBUG__
    static char guard=0;
    if(!guard){
      printf("AMSRichRing::generated WARNING: lguide too big %f\nMore warning messages suppressed\n",lguide);
      guard=1;
    }
#endif
    lg=NGUIDE-1;}
  else if(lg<0)lg=0;

  f=1.e4*k*(1.-1./SQR(_beta)*_rind[lr][lf][lg][_tile_index])*factor*_effd[lr][lf][lg][_tile_index];
  *fg=1.e4*k*(1.-1./SQR(_beta)*_ring[lr][_tile_index])*_effg[lr][_tile_index];
  *fr=1.e4*k*(1.-1./SQR(_beta)*_rinr[lr][lf][_tile_index])*_effr[lr][lf][_tile_index];
  *fb=1.e4*k*(1.-1./SQR(_beta)*_rinb[lr][lf][_tile_index])*_effb[lr][lf][_tile_index];
  return f;

}


geant AMSRichRing::lgeff(AMSPoint r, 
			 float u[3],
			 geant *lguide)
{
  float v[3],w[3];
  float f=0;
  int wnd,iw;

  const float LG_Tran=4*RICHDB::foil_index/SQR(1+RICHDB::foil_index);
  const float bwd=0.04;

#ifdef __AMSDEBUG__
  static int first=1;

  if(first){
    first=0;

    printf("\nLight Guide Parameters\n"
             "---------------------------\n"
             "LG_Tran   (Transmittance): %f\n"
             "Eff_Area  (LG-size/Pitch): %f\n",
             LG_Tran,Eff_Area);



    for(int i=0;i<RIC_NWND;i++)
      for(int j=0;j<RIC_NPHI;j++)
	for(int k=0;k<RIC_NTH;k++){

	  cout<<i<<" "<<j<<" "<<k<<" "<<RICHDB::lg_dist_tbl[i][j][k]<<" "<<
	      RICHDB::lg_eff_tbl[i][j][k]<<endl;

	}

  }
#endif	  

  wnd=RichPMTsManager::FindWindow(r[0],r[1]);
  if(wnd==-1){
    *lguide=0;
    return 0;
  }


  refract(1.,RICHDB::foil_index,u,v);

  if(locsmpl(wnd,&iw,v,w)){
   float x=acos(-w[2]);
   if(x<RIC_NTH*bwd){
    int phi,tl,th;
    float y=atan2(-w[1],-w[0]);
    phi=int((floor)(RIC_NPHI*(y+PI)/(2*PI)));
    if(phi==12) phi=0; /*EL*/
    tl=int((floor)((x-bwd/2.)/bwd));
    if(tl<0)tl=0;
    th=tl<RIC_NTH-1?tl+1:tl;
    f=RICHDB::lg_eff_tbl[iw][phi][tl]+(x-(tl+.5)*bwd)*
      (RICHDB::lg_eff_tbl[iw][phi][th]-
       RICHDB::lg_eff_tbl[iw][phi][tl])/bwd;
    *lguide=RICHDB::lg_height*(
     RICHDB::lg_dist_tbl[iw][phi][tl]+(x-(tl+.5)*bwd)*
     (RICHDB::lg_dist_tbl[iw][phi][th]-RICHDB::lg_dist_tbl[iw][phi][tl])/bwd);
   }
  }else *lguide=0;
#ifdef __AMSDEBUG__

  if(locsmpl(wnd,&iw,v,w)){
#pragma omp critical (hf1)
{
    HF1(1231001,f,1.);
    HF1(1231002,*lguide,1.);
}
  }
  *lguide=RICHDB::lg_height;
  return 1;

#endif


  return LG_Tran*f;
}


void AMSRichRing::refract(geant r1,
			  geant r2, 
			  geant *u,
			  geant *v)
{
  int i;
  float f,cn,s2n;
  const float n[3] = { 0., 0., 1. };
  float rr=r1/r2;
  cn=ESC(u,n);

  s2n=1-SQR(cn);
  f=sqrt(1.-SQR(rr)*s2n)-rr*cn;
  for(i=0;i<3;i++)v[i]=rr*u[i]+f*n[i];
  // v=rr*u+f*n;
}

int AMSRichRing::locsmpl(int id,
			 int *iw, 
			 geant *u, 
			 geant *v)
{
  const int a[16][2][2] = {
   -1, 0, 0,-1,   0,-1,-1, 0,   0,-1, 1, 0,   0,-1, 1, 0,
   -1, 0, 0,-1,  -1, 0, 0,-1,   0,-1, 1, 0,   1, 0, 0,-1,
   -1, 0, 0, 1,   0, 1,-1, 0,   1, 0, 0, 1,   1, 0, 0, 1,
    0, 1,-1, 0,   0, 1,-1, 0,   0, 1, 1, 0,   1, 0, 0, 1};
  const int wnd[16] = { 
    2, 1, 1, 2,   1, 0, 0, 1,   1, 0, 0, 1,   2, 1, 1, 2};
  int i,j;
  int ok=0;
  if(id>=0 && id<16){
    *iw=wnd[id];
    for(i=0;i<2;i++)
     for(j=0,v[i]=0;j<2;j++) v[i]+=a[id][i][j]*u[j];
    v[2]=-u[2];
    ok=1;}
  return ok;
}



#include "trrec.h"
#include "richrec.h"
#include "richradid.h"

geant AMSRichRing::ring_fraction(AMSTrTrack *ptrack ,geant &direct,geant &reflected,
			    geant &length,geant beta){

  number phi;  // Track parameter
  integer i;
  const integer NPHI=400;
  const geant twopi=3.14159265359*2;

  direct=0;
  reflected=0;
  length=0;

  // Obtain the track parameters

  RichRadiatorTileManager crossed_tile(ptrack);
  AMSDir u;
  AMSPoint r;  

  if(crossed_tile.getkind()==empty_kind) return 0.;

  geant rad_index=crossed_tile.getindex();
  geant rad_height=crossed_tile.getheight();
  r=crossed_tile.getemissionpoint();
  u=crossed_tile.getemissiondir();
  if(fabs(u[2])==0) return 0.;

  length=rad_height/fabs(u[2]);


  
  r[2]=-(r[2]-RICHDB::RICradpos());
  u[2]*=-1;


  // Here comes the Fast-Tracing routine

  //Init
  geant exp_len=RICHDB::rich_height+RICradmirgap+RIClgdmirgap;
  geant kc=(RICHDB::bottom_radius-RICHDB::top_radius)/RICHDB::rich_height;
  geant ac=RICHDB::rad_height+RICHDB::foil_height+RICradmirgap-RICHDB::top_radius/kc;
  geant mir_eff=RICmireff;
  

  for(phi=0;phi<twopi;phi+=twopi/NPHI){
    geant cc,sc,cp,sp,cn,f,sn;
    geant r0[3],u0[3],r1[3],u1[3],r2[3],u2[3],n[3],r3[3];


    cc=1./beta/rad_index; 
    sc=sqrt(1-cc*cc);
    cp=cos(phi);
    sp=sin(phi);
    f=sqrt(u[0]*u[0]+u[1]*u[1]);

    for(i=0;i<3;i++) r0[i]=r[i];

    if(f>0){
      u0[0]=sc/f*(sp*u[0]*u[2]+cp*u[1])+cc*u[0];
      u0[1]=sc/f*(sp*u[1]*u[2]-cp*u[0])+cc*u[1];
      u0[2]=-f*sc*sp+u[2]*cc;}
    else{
      u0[0]=sc*cp;
      u0[1]=sc*sp;
      u0[2]=cc;}

    // Check if it is whithin a radiator tile
    if(tile(r0)==-1) continue;
    
    if(u0[2]==0) continue;
    geant l=(RICHDB::rad_height-r0[2])/u0[2];  

    for(i=0;i<3;i++) r1[i]=r0[i]+l*u0[i];

    if (tile(r0)==-1 || tile(r1)!=tile(r0)) continue;

  /* exit radiator volume */
    n[0]=0;
    n[1]=0;
    n[2]=1;
    cn=ESC(u0,n);
    sn=sqrt(1-SQR(cn));

  if (RICHDB::foil_height>0){
    if (rad_index*sn>RICHDB::foil_index) continue;

    f=sqrt(1-SQR(rad_index/RICHDB::foil_index*sn))-rad_index/RICHDB::foil_index*cn;    
    for(i=0;i<3;i++) u1[i]=rad_index/RICHDB::foil_index*u0[i]+f*n[i];

  /* propagate to foil end */
    if(u1[2]==0) continue;
    l=RICHDB::foil_height/u1[2];
    for(i=0;i<3;i++) r1[i]=r1[i]+l*u1[i];
    if (sqrt(SQR(r1[0])+SQR(r1[1]))>RICHDB::top_radius) continue;
    cn=ESC(u1,n);
    sn=sqrt(1-SQR(cn));
    if (RICHDB::foil_index*sn>1) continue;
    f=sqrt(1-SQR(RICHDB::foil_index*sn))-RICHDB::foil_index*cn;
    for(i=0;i<3;i++) u1[i]=RICHDB::foil_index*u1[i]+f*n[i];
  }else{
    if (rad_index*sn>1) continue;
    f=sqrt(1-SQR(rad_index*sn))-rad_index*cn;
    for(i=0;i<3;i++) u1[i]=rad_index*u0[i]+f*n[i];
  }

  
  /* propagate to base of mirror*/
  if(u1[2]==0) continue;
  l=RICHDB::rich_height/u1[2];
  
  for(i=0;i<3;i++) r2[i]=r1[i]+l*u1[i];
  
  
  /* hole, direct or reflected */
  geant rbase=sqrt(SQR(r2[0])+SQR(r2[1]));
  
  if(rbase<RICHDB::bottom_radius){
    l=RIClgdmirgap/u1[2];
    for(i=0;i<3;i++) r2[i]+=l*u1[i];
    
    geant beff=RichPMTsManager::FindPMT(r2[0],r2[1])<0?0:1;
    
    direct+=beff/NPHI;
    continue;
  }
  
  double a=1-(SQR(kc)+1)*SQR(u1[2]);
  double b=2*(r1[0]*u1[0]+r1[1]*u1[1]-SQR(kc)*(r1[2]-ac)*u1[2]);
  double c=SQR(r1[0])+SQR(r1[1])-SQR(kc*(r1[2]-ac));
  double d=SQR(b)-4*a*c;
  if(d<0) continue;
  l=(-b+sqrt(d))/2./a;
  if(l<0) continue;
  
  for(i=0;i<3;i++) r2[i]=r1[i]+l*u1[i];
  
  f=1./sqrt(1+SQR(kc));
  n[0]=-f*r2[0]/sqrt(SQR(r2[0])+SQR(r2[1]));
  n[1]=-f*r2[1]/sqrt(SQR(r2[0])+SQR(r2[1]));
  n[2]=f*kc;
  
  f=2*ESC(u1,n);
  for(i=0;i<3;i++) u2[i]=u1[i]-f*n[i];
    
  if(u2[2]==0) continue; 
  l=(exp_len+RICHDB::rad_height+RICHDB::foil_height-r2[2])/u2[2];
  
  
  for(i=0;i<3;i++) r3[i]=r2[i]+l*u2[i];
  rbase=sqrt(SQR(r3[0])+SQR(r3[1]));

  if(rbase>RICHDB::bottom_radius) continue; 
  geant beff=mir_eff*(RichPMTsManager::FindPMT(r3[0],r3[1])<0?0:1);
  reflected+=beff/NPHI;
  }

  return reflected+direct;
}


AMSRichRing::AMSRichRing(AMSTrTrack* track,
			 int used,
			 int mused,
			 geant beta,
			 geant quality,
			 geant wbeta,
			 geant seed_beta,
			 geant recs[RICmaxpmts*RICnwindows][3],AMSRichRawEvent *hitp[RICmaxpmts*RICnwindows],uinteger size,int ring,
			 uinteger status,integer build_charge):
  AMSlink(status),_ptrack(track),_used(used),_mused(mused),_beta(beta),_wbeta(wbeta),_quality(quality)
{

#ifdef _OPENMP
  if(__DEBUGP__){
    cerr<<" *** "<<omp_get_thread_num()<<" STORING RING WITH BETA "<<_beta<<endl;
  }
#endif

  _tile_id=_tile_index;
  CalcBetaError();
  
#ifdef __AMSDEBUG__
  cout<<"RING WITH USED: "<<used<<" MUSED "<<mused<<endl; 
#endif
  
  _npexp=0;
  _collected_npe=0;
  _probkl=0;
  _kdist=1e6;
  _phi_spread=1e6;
  _unused_dist=-1;
  
  // Fill all the track parameters (useful for alignment)
  RichRadiatorTileManager crossed_tile(track);
  AMSPoint dirp; AMSDir dird;
  dirp=crossed_tile.getemissionpoint();
  dird=crossed_tile.getemissiondir();

  // Change it to AMS coordinates
  AMSPoint amsp; AMSDir amsd;
  amsp=RichAlignment::RichToAMS(dirp);
  amsd=RichAlignment::RichToAMS(dird);
  for(int i=0;i<3;i++) _crossingtrack[i]=amsp[i];
  _crossingtrack[3]=amsd.gettheta();
  _crossingtrack[4]=amsd.getphi();
 


  
  // Copy the residues plots
  _beta_direct.clear();
  _beta_reflected.clear();
  _hit_pointer.clear();
  _hit_used.clear();

  if(RICRECFFKEY.store_rec_hits)
  for(unsigned int i=0;i<size;i++){
    int reflected=fabs(recs[i][1]-seed_beta)<=fabs(recs[i][2]-seed_beta)?1:2;
    int closest=AMSRichRing::closest(seed_beta,recs[i]);
    _beta_direct.push_back(recs[i][0]);
    _beta_reflected.push_back(recs[i][reflected]);
    int used=(closest==0?0:1);
    int which=0;
    for(AMSRichRawEvent* hit=(AMSRichRawEvent *)AMSEvent::gethead()->
	  getheadC("AMSRichRawEvent",0);hit;hit=hit->next(),which++){
      if(hit!=hitp[i]) continue;
      _hit_pointer.push_back(which);
      if(!hit->getbit(ring)) used+=2;
      _hit_used.push_back(used);
      break;
    }
  }
  
  if(build_charge){
    if(RICCONTROLFFKEY.tsplit)AMSgObj::BookTimer.start("RERICHZ");
    //    const float window_sigmas=3;
    const float window_sigmas=sqrt(_window);
#ifndef __DARWIN__
    int env=fegetexcept();
    if(MISCFFKEY.RaiseFPE<=1)fedisableexcept(FE_ALL_EXCEPT);
#endif
    ReconRingNpexp(window_sigmas,!checkstatus(dirty_ring));
#ifndef __DARWIN__
    feclearexcept(FE_ALL_EXCEPT);
    if(env){
      feenableexcept(env);        
    }
#endif
    if((RICHDB::scatprob)>0. && _kind_of_tile==agl_kind){
#define SQR(x) ((x)*(x))
      float SigAng=_errorbeta*sqrt(geant(_used))/_beta/sqrt(SQR(_beta*_index)-1)/sqrt(SQR(_beta)-(SQR(_beta*_index)-1));
      float xmax=window_sigmas*SigAng/(RICHDB::scatang);
      float FscattCorr=1.-(RICHDB::scatprob*(1.-erf(xmax)));//gauss integral
      _npexp*=FscattCorr;
#undef SQR
    }

    if(RICCONTROLFFKEY.tsplit)AMSgObj::BookTimer.stop("RERICHZ");

  }
  
  AMSPoint pnt,point(0,0,RICHDB::RICradpos()-RICHDB::pmt_pos()+RICHDB::pmtb_height()/2.);
  AMSDir dir(0.,0.,-1.);
  number theta,phi,length;
  
  _radpos[0]=_emission_p[0];
  _radpos[1]=_emission_p[1];
  _radpos[2]=_emission_p[2];
  
  _theta=acos(1/_beta/_index);
  _errortheta=_errorbeta/_beta/tan(_theta);

  
  point=RichAlignment::RichToAMS(point);
  dir=RichAlignment::RichToAMS(dir);

  //  track->interpolate(AMSPoint(0,0,RICHDB::RICradpos()-RICHDB::pmt_pos()+RICHDB::pmtb_height()/2.),
  //		     AMSDir(0.,0.,-1.),pnt,theta,phi,length);
  track->interpolate(point,dir,
		     pnt,theta,phi,length);
  
  pnt=RichAlignment::AMSToRich(pnt);

  _pmtpos[0]=pnt[0];
  _pmtpos[1]=pnt[1];
  _pmtpos[2]=pnt[2];
  
  if(fabs(RICHDB::RICradpos()-RICHDB::pmt_pos()+RICHDB::pmtb_height()/2.-_pmtpos[2])>0.01){
    _pmtpos[0]=0;
    _pmtpos[1]=0;
    _pmtpos[2]=0;
  }

  _status|=(int(_distance2border*100)&0x3ff)<<15;

#ifdef _OPENMP
  if(__DEBUGP__){
    cerr<<" *** "<<omp_get_thread_num()<<" STORING RING WITH BETA "<<_beta<<" FINISHED"<<endl;
  }
#endif
}

//////////////////////////////////////////////////////////// LIP

// Default values... currently unused: filled during reconstruction
number AMSRichRingNew::_index=1.05;
number AMSRichRingNew::_height=3.;
AMSPoint AMSRichRingNew::_entrance_p=AMSPoint(0,0,0);
AMSDir   AMSRichRingNew::_entrance_d=AMSDir(0,0);
AMSPoint AMSRichRingNew::_emission_p=AMSPoint(0,0,0);
AMSDir   AMSRichRingNew::_emission_d=AMSDir(0,0);
geant   AMSRichRingNew::_clarity=0.0113;
geant   *AMSRichRingNew::_abs_len=0;
geant   *AMSRichRingNew::_index_tbl=0;

int     AMSRichRingNew::_kind_of_tile=0;

geant AMSRichRingNew::_Time=0;


////////////////////////////////////////// methods

AMSRichRingNew::AMSRichRingNew(AMSTrTrack* track, int Status) {  
_ptrack = track;
_Status = Status;
_HitsResiduals.clear();
_HitsStatus.clear();   
_HitsAssoc.clear();    
_TrackRec.clear();     
}

int AMSRichRingNew::buildlip(){
#ifdef NOTIMPG4MULTITHREADED
// fixme
static int mess=0;
if(!mess++)cerr<<"AMSRichRingNew::buildlip-E-WillnotRunFaulty to bad LIPC2F common "<<endl;
return 0;
#endif
  int ARRAYSIZE=(AMSEvent::gethead()->getC("AMSRichRawEvent",0))->getnelem();
  if(ARRAYSIZE==0) return 0;

  if (_ptrack != NULL ) {

    RichRadiatorTileManager crossed_tile(_ptrack);
    if(crossed_tile.getkind()==empty_kind) return 0;
  
    _index=crossed_tile.getindex();
    _height=crossed_tile.getheight();
    _entrance_p=crossed_tile.getentrancepoint();
    _entrance_d=crossed_tile.getentrancedir();
    _clarity=crossed_tile.getclarity();
    _abs_len=crossed_tile.getabstable();
    _index_tbl=crossed_tile.getindextable();
    _kind_of_tile=crossed_tile.getkind();

    // local aerogel ref index and clarity values replaces global value
    if(_index>1.02 && _index<1.1) {
      LIPC2F.radix_c2f[0] = _index;
      LIPC2F.radclarity_c2f = _clarity;
    }

    RichLIPRec::InitTrack(_entrance_p,_entrance_d);
  }

  int ievnumb = AMSEvent::gethead()->getEvent();

  if (_ptrack == NULL && ( _Status == 3 || _Status == 13) ) {
    RICHTOFTRACKINIT(1);
    if(LIPC2F.iflag_tof==1) {  // TOF track successful
      // replace track data with TOF track data
      for(int i=0;i<3;i++) {
	LIPC2F.pimp_main[i] = LIPC2F.pimp_tof[i];
	LIPC2F.epimp_main[i] = LIPC2F.epimp_tof[i];
      }
      LIPC2F.pthe_main = LIPC2F.pthe_tof;
      LIPC2F.epthe_main = LIPC2F.epthe_tof;
      LIPC2F.pphi_main = LIPC2F.pphi_tof;
      LIPC2F.epphi_main = LIPC2F.epphi_tof;
      LIPC2F.pmom_main = 0.;  // momentum is not measured by TOF
      LIPC2F.prad_main = 0; // SHOULD BE FILLED (currently not used in LIP rec)
    }
    else {
      return 0;
    }
  }

  if (_ptrack == NULL && ( _Status == 5 || _Status == 15) ) {
    RICHTOFTRACKINIT(2);
    if(LIPC2F.iflag_tof==1) {  // TOF track successful
      // replace track data with TOF track data
      for(int i=0;i<3;i++) {
	LIPC2F.pimp_main[i] = LIPC2F.pimp_tof[i];
	LIPC2F.epimp_main[i] = LIPC2F.epimp_tof[i];
      }
      LIPC2F.pthe_main = LIPC2F.pthe_tof;
      LIPC2F.epthe_main = LIPC2F.epthe_tof;
      LIPC2F.pphi_main = LIPC2F.pphi_tof;
      LIPC2F.epphi_main = LIPC2F.epphi_tof;
      LIPC2F.pmom_main = 0.;  // momentum is not measured by TOF
      LIPC2F.prad_main = 0; // SHOULD BE FILLED (currently not used in LIP rec)
    }
    else {
      return 0;
    }
  }

  if (_ptrack == NULL && ( _Status == 6 || _Status == 16) ) {
    RICHTOFTRACKINIT(3);
    if(LIPC2F.iflag_tof==1) {  // TOF track successful
      // replace track data with TOF track data
      for(int i=0;i<3;i++) {
	LIPC2F.pimp_main[i] = LIPC2F.pimp_tof[i];
	LIPC2F.epimp_main[i] = LIPC2F.epimp_tof[i];
      }
      LIPC2F.pthe_main = LIPC2F.pthe_tof;
      LIPC2F.epthe_main = LIPC2F.epthe_tof;
      LIPC2F.pphi_main = LIPC2F.pphi_tof;
      LIPC2F.epphi_main = LIPC2F.epphi_tof;
      LIPC2F.pmom_main = 0.;  // momentum is not measured by TOF
      LIPC2F.prad_main = 0; // SHOULD BE FILLED (currently not used in LIP rec)
    }
    else {
      return 0;
    }
  }

  if (_ptrack == NULL && ( _Status == 7 || _Status == 17) ) {
    RICHTOFTRACKINIT(4);
    if(LIPC2F.iflag_tof==1) {  // TOF track successful
      // replace track data with TOF track data
      for(int i=0;i<3;i++) {
	LIPC2F.pimp_main[i] = LIPC2F.pimp_tof[i];
	LIPC2F.epimp_main[i] = LIPC2F.epimp_tof[i];
      }
      LIPC2F.pthe_main = LIPC2F.pthe_tof;
      LIPC2F.epthe_main = LIPC2F.epthe_tof;
      LIPC2F.pphi_main = LIPC2F.pphi_tof;
      LIPC2F.epphi_main = LIPC2F.epphi_tof;
      LIPC2F.pmom_main = 0.;  // momentum is not measured by TOF
      LIPC2F.prad_main = 0; // SHOULD BE FILLED (currently not used in LIP rec)
    }
    else {
      return 0;
    }
  }


#ifndef __DARWIN__
    int env=fegetexcept();
    if(MISCFFKEY.RaiseFPE<=1)fedisableexcept(FE_ALL_EXCEPT);
#endif
     RICHRECLIP(ievnumb,_Status);

#ifndef __DARWIN__
    feclearexcept(FE_ALL_EXCEPT);
    if(env){
      feenableexcept(env);        
    }
#endif

    if (RichLIPRec::goodLIPREC()) {
    fillresult();
    AMSEvent::gethead()->addnext(AMSID("AMSRichRingNew",0),this);
    return 1;
  } 
  else {
    return 0;
  }

}


void AMSRichRingNew::fillresult(){

  // Fill the container

  int nr = LIPC2F.irecnumb;
  
  _Beta = LIPF2C.resb_beta[nr];
  _AngleRec = LIPF2C.resb_thc[nr];
  _AngleRecErr = LIPF2C.resb_thcerr[nr];
  _Likelihood = LIPF2C.resb_like[nr];
  _Used = LIPF2C.resb_nhit[nr];
  _ProbKolm = LIPF2C.resb_probkl[nr];
  _Flatness[0] = LIPF2C.resb_flatsin[nr];
  _Flatness[1] = LIPF2C.resb_flatcos[nr];
  _ChargeRec = LIPF2C.resc_chg[nr];
  for (int i=0;i<3;i++) {
    _ChargeProb[i] = LIPF2C.resc_chgprob[nr][i];
  }
  _ChargeRecDir = LIPF2C.resc_chgdir[nr];
  _ChargeRecMir = LIPF2C.resc_chgmir[nr];
  _NpeRing = LIPF2C.resc_cnpe[nr];
  _NpeRingDir = LIPF2C.resc_cnpedir[nr];
  _NpeRingRef = LIPF2C.resc_cnperef[nr];

  for(int i=0;i<3;i++) {
    _RingAcc[i] = LIPF2C.resc_accgeom[nr][i];
  }

  for(int i=0;i<6;i++) {
    _RingEff[i] = LIPF2C.resc_eff[nr][i];
  }

  _NMirSec = LIPF2C.resc_nmirsec[nr];

  for(int i=0;i<LIPF2C.resc_nmirsec[nr];i++) {
    _RingAccMsec1R[i] = LIPF2C.resc_accmsec[nr][0][i];
    _RingAccMsec2R[i] = LIPF2C.resc_accmsec[nr][1][i];
    _RingEffMsec1R[i] = LIPF2C.resc_effmsec[nr][0][i];
    _RingEffMsec2R[i] = LIPF2C.resc_effmsec[nr][1][i];
  }

  _RingAccRW[0] = LIPF2C.resc_arw[nr][0];
  _RingAccRW[1] = LIPF2C.resc_arw[nr][1];

  for(int i=0;i<LIPF2C.resc_nmirsec[nr];i++) {
    _RingAccRWMsec[i] = LIPF2C.resc_arwmsec[nr][i];
  }

  _UsePixelSegments = 0;  // store segment data by PMT only
  if(AMSRichRingNew::PixelDataStored()) {
    _UsePixelSegments = 1;  // store segment data by pixel (done for 1/10 of events)
  }

  _Segments = LIPF2C.resc_nrseg[nr];

  for(int i=0;i<LIPF2C.resc_nrseg[nr];i++) {
    _SegPMT.push_back(RichLIPRec::PMTlip2ams(LIPF2C.resc_pmtrseg[nr][i]));
    _SegRefStatus.push_back(LIPF2C.resc_refrseg[nr][i]);
    _SegTheta.push_back(LIPF2C.resc_angrseg[nr][i][0]);
    _SegPhi.push_back(LIPF2C.resc_angrseg[nr][i][1]);
    _SegAcceptance.push_back(LIPF2C.resc_effrseg[nr][i][0]);
    _SegEffRad.push_back(LIPF2C.resc_effrseg[nr][i][1]);
    _SegEffFull.push_back(LIPF2C.resc_effrseg[nr][i][2]);
  }

  _PixelSegments = LIPF2C.resc_npseg[nr];

  for(int i=0;i<LIPF2C.resc_npseg[nr];i++) {
    _PixSegPixel.push_back((RichLIPRec::PMTlip2ams(LIPF2C.resc_pixpseg[nr][i]/16))*16+LIPF2C.resc_pixpseg[nr][i]%16);
    _PixSegRefStatus.push_back(LIPF2C.resc_refpseg[nr][i]);
    _PixSegTheta.push_back(LIPF2C.resc_angpseg[nr][i][0]);
    _PixSegPhi.push_back(LIPF2C.resc_angpseg[nr][i][1]);
    _PixSegAcceptance.push_back(LIPF2C.resc_effpseg[nr][i][0]);
    _PixSegEffRad.push_back(LIPF2C.resc_effpseg[nr][i][1]);
    _PixSegEffFull.push_back(LIPF2C.resc_effpseg[nr][i][2]);
  }

  int icurr = 0;

  for(int i=0;i<RichLIPRec::totalhits;i++) {
    if(RichLIPRec::hitinlip[i]==0) {  // hit not carried to LIP rec
      _HitsResiduals.push_back(1.e20);
      _HitsStatus.push_back(-2);
    }
    else {
      _HitsResiduals.push_back(LIPF2C.resb_hres[nr][icurr]);
      _HitsStatus.push_back(LIPF2C.resb_used[nr][icurr]);
      if(LIPF2C.resb_used[nr][icurr]>=0) {
	_HitsAssoc.push_back(i);
      }
      icurr++;
    }
  }

  //if(LIPF2C.resb_itype[nr]==3 || LIPF2C.resb_itype[nr]==4) {  // store rec track data
    for(int i=0;i<3;i++) {
      // pvtx replaces pimp as reference point - R. Pereira 11-Apr-2011
      //_TrackRec.push_back(LIPF2C.resb_pimp[nr][i]);
      //_TrackRec.push_back(LIPF2C.resb_epimp[nr][i]);
      _TrackRec.push_back(LIPF2C.resb_pvtx[nr][i]);
      _TrackRec.push_back(LIPF2C.resb_epvtx[nr][i]);
    }
    _TrackRec.push_back(LIPF2C.resb_pthe[nr]);
    _TrackRec.push_back(LIPF2C.resb_epthe[nr]);
    _TrackRec.push_back(LIPF2C.resb_pphi[nr]);
    _TrackRec.push_back(LIPF2C.resb_epphi[nr]);
    //}

  /*

  // test prints for debugging
  cout << "-----------------------------" << endl;
  cout << "DATA STORED IN AMSRichRingNew" << endl;
  cout << "-----------------------------" << endl;
  cout << "Status = " << this->getStatus() << endl;
  cout << "Beta = " << this->getBeta() << endl;
  cout << "AngleRec (deg) = " << 180./3.14159265*this->getAngleRec() << endl;
  cout << "AngleRecErr (deg) = " << 180./3.14159265*this->getAngleRecErr() << endl;
  cout << "Likelihood = " << this->getLikelihood() << endl;
  cout << "Used = " << this->getUsed() << endl;
  cout << "ProbKolm = " << this->getProbKolm() << endl;
  cout << "Flatness = ";
  for(int i=0;i<2;i++) {
    cout << this->getFlatness(i) << " ";
  }
  cout << endl;
  cout << "ChargeRec = " << this->getChargeRec() << endl;
  cout << "ChargeProb = ";
  for(int i=0;i<3;i++) {
    cout << this->getChargeProb(i) << " ";
  }
  cout << endl;
  cout << "ChargeRecDir = " << this->getChargeRecDir() << endl;
  cout << "ChargeRecMir = " << this->getChargeRecMir() << endl;
  cout << "NpeRing = " << this->getNpeRing() << endl;
  cout << "NpeRingDir = " << this->getNpeRingDir() << endl;
  cout << "NpeRingRef = " << this->getNpeRingRef() << endl;
  cout << "RingAcc = ";
  for(int i=0;i<3;i++) {
    cout << this->getRingAcc(i) << " ";
  }
  cout << endl;
  cout << "RingEff = ";
  for(int i=0;i<6;i++) {
    cout << this->getRingEff(i) << " ";
  }
  cout << endl;
  vector<float> tmpHitsResiduals = this->getHitsResiduals();
  cout << "HitsResiduals = ";
  for(int i=0;i<tmpHitsResiduals.size();i++) {
    cout << tmpHitsResiduals[i] << " ";
  }
  cout << endl;
  vector<int> tmpHitsStatus = this->getHitsStatus();
  cout << "HitsStatus = ";
  for(int i=0;i<tmpHitsStatus.size();i++) {
    cout << tmpHitsStatus[i] << " ";
  }
  cout << endl;
  vector<int> tmpHitsAssoc = this->getHitsAssoc();
  cout << "HitsAssoc = ";
  for(int i=0;i<tmpHitsAssoc.size();i++) {
    cout << tmpHitsAssoc[i] << " ";
  }
  cout << endl;
  vector<float> tmpTrackRec = this->getTrackRec();
  cout << "TrackRec (7-10 in deg) = ";
  for(int i=0;i<tmpTrackRec.size();i++) {
    if(i<6) {
      cout << tmpTrackRec[i] << " ";
    }
    else {
      cout << 180./3.14159265*tmpTrackRec[i] << " ";
    }
  }
  cout << endl;
  // end of test prints for debugging
  
  */

}


void AMSRichRingNew::_writeEl(){
#define LIP_NHITMAX 1000
#ifdef __WRITEROOT__
  AMSJob::gethead()->getntuple()->Get_evroot02()->AddAMSObject(this);
#endif
}


void AMSRichRingNew::_copyEl(){
#ifdef __WRITEROOT__
  if(PointerNotSet())return;
  RichRingBR & ptr = AMSJob::gethead()->getntuple()->Get_evroot02()->RichRingB(_vpos);
  if (_Status==4 || _Status==14) {
    ptr.fTrTrack=-2;
    return;
  }
  if (_ptrack) ptr.fTrTrack= _ptrack->GetClonePointer();
  else ptr.fTrTrack=-1;
#endif
}



//////////////////////////////////////////////////////////// LIP Ring set


// depth of vertex by tile

const double AMSRichRingNewSet::zphemiloc[] = {
  0.6,0.6,0.6,0.6,0.6,0.6,0.6,0.6,0.6,0.6,  //   0-9
  0.6,0.6,0.6,0.6,0.6,0.6,0.6,0.6,0.6,0.6,  //  10-19
  0.6,0.6,0.6,0.6,0.6,0.6,0.6,0.6,0.6,0.6,  //  20-29
  0.6,0.6,0.6,0.6,0.6,0.6,0.6,0.6,0.6,0.6,  //  30-39   AEROGEL
  0.6,0.6,0.6,0.6,0.6,0.6,0.6,0.6,0.6,0.6,  //  40-49    TILES
  0.6,0.6,0.6,0.6,0.6,0.6,0.6,0.6,0.6,0.6,  //  50-59
  0.6,0.6,0.6,0.6,0.6,0.6,0.6,0.6,0.6,0.6,  //  60-69
  0.6,0.6,0.6,0.6,0.6,0.6,0.6,0.6,0.6,0.6,  //  70-79
  0.6,0.6,0.6,0.6,0.6,0.6,0.6,0.6,0.6,0.6,  //  80-89
  0.6,0.6,                                  //  90-91
  //--------------------------------------- // ---------------
  0.5,0.5,0.5,0.5,0.5,0.5,0.5,0.5,          //  92-99     NaF
  0.5,0.5,0.5,0.5,0.5,0.5,0.5,0.5};         // 100-107   TILES

geant AMSRichRingNewSet::_Time=0;


AMSRichRingNewSet::AMSRichRingNewSet () {ringset.clear();}

AMSRichRingNewSet::~AMSRichRingNewSet () {
  for (unsigned int i=0; i<ringset.size(); i++) {
    delete ringset[i];
  }
  ringset.clear();
}

void AMSRichRingNewSet::reset() {
  ringset.clear();
}


AMSRichRingNew* AMSRichRingNewSet::getring(int i) {
  if (i<int(ringset.size())) {
    return ringset[i];
  }
  else {
    return NULL;
  }
}

void AMSRichRingNewSet::AddRing(AMSRichRingNew* ring) {ringset.push_back(ring);}

void AMSRichRingNewSet::init() {
  RichLIPRec::InitEvent();
}

int AMSRichRingNewSet::NumberOfRings() { return (int)ringset.size();}

integer AMSRichRingNewSet::trig=0;

void AMSRichRingNewSet::build() {
  _Start();
  const integer freq=10;
  trig=(trig+1)%freq;

  // Build rec rings for all the tracks 

  int16u lipflag = 0;

  if(RICRECFFKEY.liprflag[0]%10) lipflag+=1;
  if((RICRECFFKEY.liprflag[0]/10)%10) lipflag+=2;
  if((RICRECFFKEY.liprflag[0]/100)%10) lipflag+=4;
  if((RICRECFFKEY.liprflag[0]/1000)%10) lipflag+=8;

  if(RICRECFFKEY.liprflag[1]%10) lipflag+=16;
  if((RICRECFFKEY.liprflag[1]/10)%10) lipflag+=32;
  if((RICRECFFKEY.liprflag[1]/100)%10) lipflag+=64;
  if((RICRECFFKEY.liprflag[1]/1000)%10) lipflag+=128;

  // reset reconstruction flags (internal LIP data set)
  for(int k=0;k<LIP_NMAXLIPREC;k++) {
    LIPF2C.resb_iflag[k] = -1;
    LIPF2C.resb_itype[k] = -999;
    LIPF2C.resb_itrk[k] = -999;
    LIPF2C.resc_iflag[k] = -1;
  }

  //Loop on ams tracks
  AMSTrTrack *track=0;
  AMSRichRingNew *pliprec=0;
  int j=0,k=0;
  for(int id=0;;){
    track=(AMSTrTrack *)AMSEvent::gethead()->getheadC("AMSTrTrack",id++,1);
    if(!track) break;
    for(;track;track=track->next()) {

      //cout << "Event Number = " << AMSEvent::gethead()->getid() << endl;
      //cout << "Track: nb hits = " << track->getnhits() << endl;  
      //cout << "Track: theta   = " << track->gettheta() << endl;  

      LIPC2F.itrknumb++;

      //standard ams track
      if (lipflag&0x0001 || lipflag&0x0010) {
	j++;
        if (lipflag&0x0010) {
	  pliprec = new AMSRichRingNew(track, 12);
        } else {
	  pliprec = new AMSRichRingNew(track, 2);
	}
	if ( pliprec->buildlip() ) {
	  k++;
	  AddRing(pliprec);
	}
        else delete pliprec; 

	if(trig==0 && _NoMoreTime()){
	  //	  throw amsglobalerror(" AMSRichRingNewSet::build-E-Cpulimit Exceeded ");
	  return;
	}
	
      }


      
      //flexible standard ams track
      if (lipflag&0x0002 || lipflag&0x0020) {
	j++;
        if (lipflag&0x0020) {
	  pliprec = new AMSRichRingNew(track, 13);
        } else {
	  pliprec = new AMSRichRingNew(track, 3);
	}
	if ( pliprec->buildlip() ) {
	  k++;
	  AddRing(pliprec);
	} 

	if(trig==0 && _NoMoreTime()){
	  //	  throw amsglobalerror(" AMSRichRingNewSet::build-E-Cpulimit Exceeded ");
	  return;
	}

      }
    }

  }

  // do not proceed to special reconstructions unless there is at least one TrTrack
  //track=(AMSTrTrack *)AMSEvent::gethead()->getheadC("AMSTrTrack",0,1);
  //if(!track) return;

  //additional reconstructions beyond ams tracks

  /* TOF track */
  LIPC2F.itrknumb = -1;
  if (lipflag&0x0004 || lipflag&0x0040) {
    if (lipflag&0x0040) {
      pliprec = new AMSRichRingNew(NULL, 13);
    } else {
      pliprec = new AMSRichRingNew(NULL, 3);
    }
    if ( pliprec->buildlip() ) {
      k++;
      AddRing(pliprec);
    } 

    if(trig==0 && _NoMoreTime()){
      //      throw amsglobalerror(" AMSRichRingNewSet::build-E-Cpulimit Exceeded ");
      return;
    }
    
  }

  /* TOF track number 2 */
  LIPC2F.itrknumb = -3;
  if (lipflag&0x0004 || lipflag&0x0040) {
    if (lipflag&0x0040) {
      pliprec = new AMSRichRingNew(NULL, 15);
    } else {
      pliprec = new AMSRichRingNew(NULL, 5);
    }
    if ( pliprec->buildlip() ) {
      k++;
      AddRing(pliprec);
    } 

    if(trig==0 && _NoMoreTime()){
      //      throw amsglobalerror(" AMSRichRingNewSet::build-E-Cpulimit Exceeded ");
      return;
    }

  }

  /* TOF track number 3 */
  LIPC2F.itrknumb = -4;
  if (lipflag&0x0004 || lipflag&0x0040) {
    if (lipflag&0x0040) {
      pliprec = new AMSRichRingNew(NULL, 16);
    } else {
      pliprec = new AMSRichRingNew(NULL, 6);
    }
    if ( pliprec->buildlip() ) {
      k++;
      AddRing(pliprec);
    } 

    if(trig==0 && _NoMoreTime()){
      //      throw amsglobalerror(" AMSRichRingNewSet::build-E-Cpulimit Exceeded ");
      return;
    }

  }

  /* TOF track number 4 */
  LIPC2F.itrknumb = -5;
  if (lipflag&0x0004 || lipflag&0x0040) {
    if (lipflag&0x0040) {
      pliprec = new AMSRichRingNew(NULL, 17);
    } else {
      pliprec = new AMSRichRingNew(NULL, 7);
    }
    if ( pliprec->buildlip() ) {
      k++;
      AddRing(pliprec);
    } 

    if(trig==0 && _NoMoreTime()){
      //      throw amsglobalerror(" AMSRichRingNewSet::build-E-Cpulimit Exceeded ");
      return;
    }
  }

  /* standalone */
  LIPC2F.itrknumb = -2;
  if (lipflag&0x0008 || lipflag&0x0080) {
    if (lipflag&0x0080) {
      pliprec = new AMSRichRingNew(NULL, 14);
    } else {
      pliprec = new AMSRichRingNew(NULL, 4);
    }
    if ( pliprec->buildlip() ) {
      k++;
      AddRing(pliprec);
    } 

    if(trig==0 && _NoMoreTime()){
      //      throw amsglobalerror(" AMSRichRingNewSet::build-E-Cpulimit Exceeded ");
      return;
    }
  }

}

