//  $Id: richrec.C,v 1.16 2011/11/11 21:43:22 mdelgado Exp $
#include <math.h>
#include "richrec.h"
#include "richradid.h"
#include <iostream>

//#define __AUTOCLEAN__

#ifdef __AUTOCLEAN__
#include<vector>
#include<algorithm>
#endif


using namespace std;



integer RichRawEvent::_PMT_Status[RICmaxpmts];
geant   RichRawEvent::_beta_hit[RICmaxpmts*RICnwindows][3];

void RichRawEvent::build(AMSEventR *event){
  double signal[RICmaxpmts];
  int hid[RICmaxpmts];
  int test[RICmaxpmts];
  double best_mean=0;
  double best_sigma=1e200;

  //
  // Collect the PMT status
  //
  for(int i=0;i<RICmaxpmts;_PMT_Status[i++]=0);

  for(RichRawEvent* current=new RichRawEvent(event);current;current=current->next()){
    int pmt=current->getchannel()/16;
    if(current->getbit(crossed_pmt_bit)) _PMT_Status[pmt]=1;
  }
}


void RichRawEvent::reconstruct(AMSPoint origin,AMSPoint origin_ref,
                                  AMSDir direction,AMSDir direction_ref,
				  geant betamin,geant betamax,
				  geant *betas,geant index,
				  int kind_of_tile=agl_kind){


#ifdef __AMSDEBUG__
  cout<<"RECONSTRUCTING HIT "<<_current<<endl;
  cout<<"   CHANNEL "<<getchannel()<<endl;
  cout<<"   COUNTS  "<<getcounts()<<endl;
  cout<<"   NPE     "<<getnpe()<<endl;

  RichPMTChannel channel_test(getchannel());
  cout<<" POISITION ACCORDING TO RichPMTChannel "<<channel_test.position[0]<<" "<<channel_test.position[1]<<endl;

#endif


  // Reconstruct the beta values for this hit. Assumes direction as unitary
  const geant z=RICHDB::RICradpos()-RICHDB::rad_height-RICHDB::foil_height-
    RICradmirgap-RIClgdmirgap-RICHDB::rich_height;
  RichPMTChannel channel(getchannel());
  geant x=channel.x();
  geant y=channel.y();

  betas[0]=0;
  betas[1]=0;
  betas[2]=0;
  _beta_hit[_current][0]=betas[0];
  _beta_hit[_current][1]=betas[1];
  _beta_hit[_current][2]=betas[2];

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

  // Artificially increase in the expansion length 
  H+=RichRing::DeltaHeight; 

  //  if(kind_of_tile==naf_kind)
  //    H+=RICHDB::foil_height;

  while(fabs(delta)>1e-5 && time_out<5){
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
      //      betas[0]=-2.;       // it is the primary passing through the LG so
      //      return;}            // stop the recontruction.
    }
    else if(betas[0]>betamax) betas[0]=-1;
  }else{
    betas[0]=0;    // Wrong theta
  }

#ifdef __AMSDEBUG__
  {
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

  //  return;// TEST
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

    //CJD:  PROVISIONAL CORRECTION ON THE RADIATOR INDEX FOR REFLECTED
    if(kind_of_tile==agl_kind) betas[j]/=0.9993;
    if(kind_of_tile==naf_kind) betas[j]/=0.998;
    //    betas[j]*=1.0007624;


    if(betas[j]<betamin) betas[j]=-2.;
    else
      if(betas[j]>betamax) betas[j]=-1.; 
      else 
	j++;
#ifndef __SAFE__
    if(j==3) return;
#endif    
  }

#ifdef __AMSDEBUG__
  {
    // Dump the information about the reconstruction
    cout<<"-----------------RECONSTRUCTING AS REFLECTED "<<endl;
  }

#endif
  // Fill the current beta hit 
  _beta_hit[_current][0]=betas[0];
  _beta_hit[_current][1]=betas[1];
  _beta_hit[_current][2]=betas[2];
}


integer RichRawEvent::reflexo(AMSPoint origin,AMSPoint *ref_point){
  geant false_height=RICHDB::bottom_radius*
    RICHDB::rich_height/(RICHDB::bottom_radius-RICHDB::top_radius); 


  double zk=(RICHDB::bottom_radius/false_height)*
    (RICHDB::bottom_radius/false_height);
  double c2=zk/(1+zk);
  double c=sqrt(c2);
  double alp=1/sqrt(1+zk);
  
  RichPMTChannel channel(getchannel());
  double xf=channel.x();
  double yf=channel.y();

  double zf=-false_height-RIClgdmirgap;

  double x=origin[0],y=origin[1],
    z=origin[2]-RICHDB::RICradpos()+RICHDB::rad_height-false_height+
                (RICHDB::rich_height+RICHDB::foil_height
                   +RICradmirgap)
                -RICHDB::foil_height; // Correction due to high index

  z+=RichRing::DeltaHeight; 


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
                              RICHDB::rad_height+RICHDB::RICradpos()
			     -RichRing::DeltaHeight);

      // Put it again in Rich coordinates
      ref_point[good]=RichAlignment::MirrorToRich(ref_point[good]);

      good++;
    }

  }

  return good;
}




// Default values... currently unused: filled during reconstruction
number RichRing::_index=1.05;
number RichRing::_height=3.;
AMSPoint RichRing::_entrance_p=AMSPoint(0,0,0);
AMSDir   RichRing::_entrance_d=AMSDir(0,0);
AMSPoint RichRing::_emission_p=AMSPoint(0,0,0);
AMSDir   RichRing::_emission_d=AMSDir(0,0);
geant   RichRing::_clarity=0.0113;
geant   *RichRing::_abs_len=0;
geant   *RichRing::_index_tbl=0;
bool    RichRing::ComputeNpExp=true;
bool    RichRing::UseDirect=true;
bool    RichRing::UseReflected=true;
double  RichRing::DeltaHeight=0;

int     RichRing::_kind_of_tile=0;
int     RichRing::_tile_index=0;

AMSEventR *RichRing::_event=0;


RichRing *RichRing::build(AMSEventR *event,AMSPoint p,AMSDir d){
  memset(RichRawEvent::_beta_hit,0,RICmaxpmts*RICnwindows*3*sizeof(geant));  // Reset the beta hit arrays 
  RichRawEvent::build(event); // Indetify hot spots

  // Build all the tracks 
  _event=event;

  TrTrack track(p,d);
  return build(&track,10);
}

RichRing *RichRing::build(AMSEventR *event,TrTrackR *tr){
  memset(RichRawEvent::_beta_hit,0,RICmaxpmts*RICnwindows*3*sizeof(geant));  // Reset the beta hit arrays 
  RichRawEvent::build(event); // Indetify hot spots

  // Build all the tracks 
  _event=event;
#ifdef __AMSDEBUG__
  cout<<"RichRing::build(AMSEventR *event,TrTrackR *tr)-> CALLING TRTRACK"<<endl;
#endif
  TrTrack track(tr);
  if(event->Version()>=373){
    if(event->nParticle()==0) return 0;
    if(!event->pParticle(0)) return 0;
    RichRingR *ring=event->pParticle(0)->pRichRing();
    if(!ring) return 0;
    if(fabs(ring->AMSTrPars[0])>1e-9 && fabs(ring->AMSTrPars[1])>1e-9 && fabs(ring->AMSTrPars[2])>1e-9){
      track._r.setp(ring->AMSTrPars[0],
		    ring->AMSTrPars[1],
		    ring->AMSTrPars[2]);
      track._d.SetTheta(ring->AMSTrPars[3]);
      track._d.SetPhi(ring->AMSTrPars[4]);
    }
  }

#ifdef __AMSDEBUG__
  cout<<"RichRing::build(AMSEventR *event,TrTrackR *tr)-> build(TrTrack,int)"<<endl;
#endif
  return build(&track,10);
}

// New option, if cleanup==10 -> force cleaning and only store cleaned
// rings

float RichRing::_window=9.0;

RichRing* RichRing::build(TrTrack *track,int cleanup){
  // All these arrays are for speed up the reconstruction
  // They should be move to a dynamic list (like the containers)
  // using, for example, a structure (~completely public class)

  int ARRAYSIZE=RichRing::_event->nRichHit();
  if(ARRAYSIZE==0) return 0;

  // Fast but not safe
  geant recs[RICmaxpmts*RICnwindows][3];
  geant mean[RICmaxpmts*RICnwindows][3];
  geant probs[RICmaxpmts*RICnwindows][3];
  integer size[RICmaxpmts*RICnwindows][3];
  integer mirrored[RICmaxpmts*RICnwindows][3];

  int bit=0;

  if(bit>16){
    //    cout<<"RichRing::build -- Too many rings -- aborting "<<endl;
    return 0;
  }


  if(bit==crossed_pmt_bit){
    //cout<<" RichRing::build-too-many-tracks "<<endl;
    return 0;
  }

      
  //============================================================
  // Here we should check if the track goes through the radiator
  //============================================================

#ifdef __AMSDEBUG__
  cout<<"RichRing::build(TrTrack*,int)-> RichRadiatorTileManager(TrTrack*)"<<endl;
#endif
  RichRadiatorTileManager crossed_tile(track);
#ifdef __AMSDEBUG__
  cout<<"done"<<endl<<endl;
#endif  

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

  integer actual=0,counter=0;
  //  RichRawEvent *hitp[RICmaxpmts*RICnwindows];
  RichHitR *hitp[RICmaxpmts*RICnwindows];


  // Sort the hits by amplitude and compute a hit threshold
#ifdef __AUTOCLEAN__
  double signalThreshold=0;
  {
    const double threshold=0.5;  // Remove everything below 20% of the total charge 

    vector<double> amplitude;
    double total=0;
    for(RichRawEvent* hit=new RichRawEvent(_event);
	hit;hit=hit->next()){
      // Skip problematic ones
      if(!hit->getbit(ok_status_bit)) continue;
      if(hit->getpointer()->Status&(1<<crossed_pmt_bit)) continue;
      total+=hit->getpointer()->Npe;
      amplitude.push_back(hit->getpointer()->Npe);
    }  
    
    sort(amplitude.begin(),amplitude.end());
    double acc=0;
    for(vector<double>::iterator i=amplitude.begin();i!=amplitude.end();i++){
      acc+=*i;
      if(acc>threshold*total){signalThreshold=*i;break;}
    }
    if(signalThreshold<1.5) signalThreshold=0;
  }
#endif
  
  for(RichRawEvent* hit=new RichRawEvent(_event);
	hit;hit=hit->next()){
    if(!hit->getbit(ok_status_bit)) continue;
#ifdef __AUTOCLEAN__
    if(hit->getpointer()->Status&(1<<crossed_pmt_bit)) continue;
    if(hit->getpointer()->Npe<signalThreshold) continue;
#endif
    // Checks bounds
    if(actual>=RICmaxpmts*RICnwindows) {
      cerr << "RichRing::build : Event too long."<<endl;
      break;
    }
    
    // Reconstruct one hit
    hit->reconstruct(dirp,refp,
		     dird,refd,
		     betamin,betamax,recs[actual],_index,_kind_of_tile);

    hit->unsetbit(bit);

    if(!UseDirect) recs[actual][0]=0;
    if(!UseReflected) recs[actual][1]=recs[actual][2]=0;


    if(recs[actual][0]>0 || recs[actual][1]>0 || recs[actual][2]>0){	
      hitp[actual]=hit->getpointer();
      actual++;
    }
  }



  // Look for clusters
  uinteger current_ring_status=_kind_of_tile==naf_kind?naf_ring:0;


  RichRing *first_done=0;

  do{
    integer best_cluster[2]={0,0};
    geant best_prob=-1;

    for(integer i=0;i<actual;i++)
      for(integer j=0;j<3;j++)
	if(recs[i][j]>0){
	  mean[i][j]=recs[i][j];
	  size[i][j]=1;mirrored[i][j]=j>0?1:0;
	  probs[i][j]=0;}
    
    
    
    for(integer i=0;i<actual;i++){
      if(recs[i][0]==-2.) continue; // Jump if direct is below threshold
      if((cleanup && current_ring_status&dirty_ring) || (cleanup/10)%10) 
	if(hitp[i]->Status&(1<<crossed_pmt_bit)) continue;

      for(integer k=0;k<3;k++){
	if(recs[i][k]<betamin) continue; 
	for(integer j=0;j<actual;j++){
	  if(recs[j][0]==-2.) continue;
	  if(i==j) continue;
	  if((cleanup && current_ring_status&dirty_ring) || (cleanup/10)%10)  
            if(hitp[j]->Status&(1<<crossed_pmt_bit)) continue;

	  integer better=RichRing::closest(recs[i][k],recs[j]);
	  
	  geant prob=(recs[i][k]-
		      recs[j][better])*
	    (recs[i][k]-
	     recs[j][better])/
	    RichRing::Sigma(recs[i][k],A,B)/
	    RichRing::Sigma(recs[i][k],A,B);
	  if(prob<_window){ //aprox. (3 sigmas)**2
	    probs[i][k]+=exp(-.5*prob);
	    mean[i][k]+=recs[j][better];
	    if(better>0) mirrored[i][k]++;
	  size[i][k]++;
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
	  hitp[i]->Status&=~(1<<bit);
	  if(recs[i][0]==-2.) continue;
	  if((cleanup && cleaning&dirty_ring)  || (cleanup/10)%10) 
	    if(hitp[i]->Status&(1<<crossed_pmt_bit)) continue;
	  
	  integer closest=
	    RichRing::closest(beta_track,recs[i]);
	  
	  if(recs[i][closest]<betamin) continue;
	  geant prob=(recs[i][closest]-beta_track)*
	    (recs[i][closest]-beta_track)/
	    RichRing::Sigma(beta_track,A,B)/
	    RichRing::Sigma(beta_track,A,B);


	  if(prob>=_window) continue;
	  hitp[i]->Status|=(1<<bit);
	  if(cleanup) current_ring_status|=(hitp[i]->Status&(1<<crossed_pmt_bit))?dirty_ring:0; 
	  chi2+=prob;
	  wsum+=hitp[i]->Npe;
	  wbeta+=recs[i][closest]*hitp[i]->Npe;
	}
	
	beta_used=size[best_cluster[0]][best_cluster[1]];
	mirrored_used=mirrored[best_cluster[0]][best_cluster[1]];
	beta_track=mean[best_cluster[0]][best_cluster[1]]/geant(beta_used);
	
	if(wsum>0) wbeta/=wsum; else wbeta=0.;       
      }
      
      // Event quality numbers:
      // 0-> Number of used hits
      // 1-> chi2/Ndof
      
      // Fill the container
      //      RichRing* done=(RichRing *)AMSEvent::gethead()->addnext(AMSID("RichRing",0),
      RichRing* done=new RichRing(track,
			     beta_used,
			     mirrored_used,
			     beta_track,
			     chi2/geant(beta_used-1),
			     wbeta,
			     recs[best_cluster[0]][best_cluster[1]],
			     recs,hitp,actual,bit,
			     current_ring_status,  //Status word
			     ComputeNpExp                     // force charge reconstruction
			     );


      if(!first_done) first_done=done;
    }  
    bit++;  
  }while(current_ring_status&dirty_ring);   // Do it again if we want to clean it up once

  return first_done;
}



void RichRing::CalcBetaError(){
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
    sqrt(RichRing::Sigma(_beta,A,B)*
	 RichRing::Sigma(_beta,A,B)/geant(_used)+0.016e-2*0.016e-2):1;
}


////////////////////////////////////
/// charge reconstruction
/// CREDITS TO JORGE CASAUS AND ELISA LANCIOTTI and moi

#define PI 3.14159265359
#define SQR(x) ((x)*(x))
#define ESC(x,y) ((x)[0]*(y)[0]+(x)[1]*(y)[1]+(x)[2]*(y)[2])
#define MOD(x) sqrt(ESC(x,x))


void RichRing::ReconRingNpexp(geant window_size,int cleanup){ // Number of sigmas used 
 for(int i=0;i<16;i++) _npexplg[i]=_usedlg[i]=0;
 _emitted_rays=0;
 _reflected_rays=0;


  AMSPoint local_pos=_entrance_p;
  AMSDir   local_dir=_entrance_d;

  local_pos[2]=RICHDB::rad_height-_height;

  // Protect against unexpected sign in _entrance_d 
  if(local_dir[2]>0){
    local_dir[0]*=-1;
    local_dir[1]*=-1;
  }else local_dir[2]*=-1;
  //  local_dir[2]*=-1;

  const integer NSTL=15,NSTP=70; // Z=1 optimized
  const geant dphi=2*PI/NSTP;


  geant dfphi[NSTP],dfphih[NSTP];
  geant hitd[RICmaxpmts*RICnwindows],hitp[RICmaxpmts*RICnwindows];
  RichHitR *used_hits[RICmaxpmts*RICnwindows];

  geant unused_hitd[RICmaxpmts*RICnwindows];
  RichHitR *unused_hits[RICmaxpmts*RICnwindows];

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

  for(RichRawEvent* hit=new RichRawEvent(_event)
	;hit;hit=hit->next()){
    // THIS HAS TO BE DONE
    if(hit->getbit(0)){
      used_hits[nh]=hit->getpointer();
      hitd[nh++]=1.e5;
    }else
      if(!(hit->getbit(crossed_pmt_bit))){
	unused_hits[nh_unused]=hit->getpointer();
	unused_hitd[nh_unused++]=1.e5;
      }
  }
  

  geant l,dL,phi;
  geant efftr,xb,yb,lentr,lfoil,lguide,geftr,
    reftr,beftr,ggen,rgen,bgen;
  geant nexp,nexpg,nexpr,nexpb,prob,vice,pvice;
  geant dfnrm=0,dfnrmh=0;
  integer tflag;

  AMSPoint r;

  l=_height/local_dir[2];
  dL=l/NSTL;
  integer i,j,k;

  memset(photons_per_channel,0,sizeof(photons_per_channel[0])*680*16);
  for(nexp=0,nexpg=0,nexpr=0,nexpb=0,j=0;j<NSTL;j++){
    l=(j+.5)*dL;

    r=local_pos+local_dir*l;
    

    for(phi=0,i=0;phi<2*PI;phi+=dphi,i++){
      int channel=-1,pmt=-1;
      _emitted_rays++;
      efftr=trace(r,local_dir,phi,&xb,&yb,&lentr,
		  &lfoil,&lguide,&geftr,&reftr,&beftr,&tflag,channel,pmt);


#pragma omp critical (richrec1)
      if(geftr){
	float cnt=generated(lentr,lfoil,lguide,
			    &ggen,&rgen,&bgen)*dL/NSTP;

	// Store information for the FastMC 
	if(pmt>-1 && channel>-1) 
	  photons_per_channel[pmt*16+channel]+=efftr*cnt;

	dfphi[i]+=efftr*cnt;

	nexp+=efftr*cnt;
	nexpg+=geftr*dL/NSTP*ggen;
	nexpr+=reftr*dL/NSTP*rgen;
	nexpb+=beftr*dL/NSTP*bgen;
	_npexplg[channel]+=efftr*cnt;
      }

      for(k=0;k<nh_unused;k++){
	geant d=sqrt(SQR(xb-unused_hits[k]->Coo[0])+
		     SQR(yb-unused_hits[k]->Coo[1]));
	if(d<unused_hitd[k])  unused_hitd[k]=d;
      }


      for(k=0;k<nh;k++){
	geant d=sqrt(SQR(xb-used_hits[k]->Coo[0])+
		     SQR(yb-used_hits[k]->Coo[1]));
	if(d<hitd[k]){
	  hitd[k]=d;
	  hitp[k]=phi;
	}
      }
    }
  }

  /*
#ifdef __AMSDEBUG__
  HF1(123000,nexpg,1.);
  HF1(123001,nexpr,1.);
  HF1(123002,nexpb,1.);
  HF1(123003,nexp,1.);
#endif
  */


  for(i=0;i<nh;i++){
    if(hitd[i]<1.){
      nu++;
      j=int(hitp[i]/dphi);
      dfphih[j]+=used_hits[i]->Npe;
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

  _collected_npe=0.;
  
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

  for(RichRawEvent* hit=new RichRawEvent(_event)
	;hit;hit=hit->next()){  
    if(!hit->getbit(ok_status_bit)) continue;

    geant betas[3];
    // This has to be done in a different way
    for(int n=0;n<3;n++) betas[n]=hit->getbeta(n);
    geant value=fabs(_beta-betas[closest(_beta,betas)])/sigma;

    for(int ii=0;ii<10;ii++)
      if(value<ii+1){
	_collected_hits_window[ii]+=(cleanup && hit->getbit(crossed_pmt_bit))?0:1;
	_collected_pe_window[ii]+=(cleanup && hit->getbit(crossed_pmt_bit))?0:hit->getnpe();
      }



    if(value<window_size){
      _collected_npe+=(cleanup && hit->getbit(crossed_pmt_bit))?0:hit->getnpe();
      int pmt,channel;
      RichPMTsManager::UnpackGeom(hit->getchannel(),pmt,channel);
      _usedlg[channel]+=1;

    }
  }

}


geant RichRing::trace(AMSPoint r, AMSDir u, 
			 geant phi, geant *xb, geant *yb, 
			 geant *lentr, geant *lfoil, 
			 geant *lguide, geant *geff, 
			 geant *reff, geant *beff, 
			 integer *tflag,int &_channel,int &_pmt)
{
  geant r0[3],u0[3],r1[3],u1[3],r2[3],u2[3],r3[3],n[3];
  geant cc,sc,cp,sp,cn,sn,f,l,a,b,c,d,maxxy,rbase;
  int i,ed;
  const geant exp_len=RICHDB::rich_height+RICradmirgap+RIClgdmirgap; 
  const geant kc=(RICHDB::bottom_radius-RICHDB::top_radius)/RICHDB::rich_height;
  const geant ac=RICHDB::rad_height+RICHDB::foil_height+RICradmirgap-RICHDB::top_radius/kc;
  const geant bx=RICHDB::hole_radius[0];
  const geant by=RICHDB::hole_radius[1];
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
      _channel=channel;
      _pmt=pmt;


      int status=RichPMTsManager::Status(pmt,channel);
      if((status%10)!=Status_good_channel || RichRawEvent::CrossedPMT(pmt)){
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
    _reflected_rays++;
    a=1-(SQR(kc)+1)*SQR(u1[2]);
    b=2*(r1[0]*u1[0]+r1[1]*u1[1]-SQR(kc)*(r1[2]-ac)*u1[2]);
    c=SQR(r1[0])+SQR(r1[1])-SQR(kc*(r1[2]-ac));
    d=SQR(b)-4*a*c;
    if(d<0){
#ifdef __AMSDEBUG__
      printf("RichRing::trace Crossing Point not found\n");
      printf(" kc %f, ac %f\n",kc,ac);
      printf(" a %f, b %f, c %f\n",a,b,c);
#endif
      return 0;}
    l=(-b+sqrt(d))/2./a;
    if(l<0){
#ifdef __AMSDEBUG__
      printf("RichRing::trace Crossing Point negative \n");
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
      _channel=channel;
      _pmt=pmt;


      int status=RichPMTsManager::Status(pmt,channel);
      if((status%10)!=Status_good_channel || RichRawEvent::CrossedPMT(pmt)){
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



int RichRing::tile(AMSPoint r){ // Check if a track hits the radator support struycture
  integer tile=RichRadiatorTileManager::get_tile_number(r[0],r[1]);
  if(RichRadiatorTileManager::get_tile_kind(tile)==empty_kind) return -1;
  // Next should be guaranteed by RichRadiatorTileManager
  //  if(fabs(RichRadiatorTileManager::get_tile_x(tile)-r[0])>RICHDB::rad_length/2.-RICaethk/2.
  //     ||fabs(RichRadiatorTileManager::get_tile_y(tile)-r[1])>RICHDB::rad_length/2.-RICaethk/2.) return 0;
  return tile;
}



// Several static arrays within generated moved to static members of RichRing
float RichRing::_l[RICmaxentries][_TILES_]; 
float RichRing::_r[RICmaxentries][_TILES_]; 
float RichRing::_a[RICmaxentries][_TILES_]; 
float RichRing::_b[RICmaxentries][_TILES_]; 
float RichRing::_g[RICmaxentries][_TILES_]; 
float RichRing::_t[RICmaxentries][_TILES_];
float RichRing::_effg[_NRAD_][_TILES_]; 
float RichRing::_ring[_NRAD_][_TILES_];
float RichRing::_effr[_NRAD_][_NFOIL_][_TILES_]; 
float RichRing::_rinr[_NRAD_][_NFOIL_][_TILES_];
float RichRing::_effb[_NRAD_][_NFOIL_][_TILES_]; 
float RichRing::_rinb[_NRAD_][_NFOIL_][_TILES_];
float RichRing::_effd[_NRAD_][_NFOIL_][_NGUIDE_][_TILES_]; 
float RichRing::_rind[_NRAD_][_NFOIL_][_NGUIDE_][_TILES_];
int RichRing::_generated_initialization=1;
int RichRing::_first_radiator_call[_TILES_];


float RichRing::generated(geant length,
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
  const float abslref=(RICHDB::lg_abs[0]+RICHDB::lg_abs[1])/2;
  const float factor=1.;
  const int ENTRIES=RICmaxentries;

  float rmn=0,rmx=2.0*_height;
  float fmn=RICHDB::foil_height,fmx=1.5*RICHDB::foil_height;
  float gmn=RICHDB::lg_height,gmx=1.7*RICHDB::lg_height;
  int i,lr,lf,lg,nf;
  float beta;
  float f=0.;

  if(_generated_initialization){for(int i=0;i<_TILES_;i++) _first_radiator_call[i]=1;_generated_initialization=0;}

  if(_kind_of_tile==naf_kind){
    rmx=6.0*_height;
    fmx=3.0*RICHDB::foil_height;}

  if(_first_radiator_call[_tile_index]){
    cout<<"Initializing tile "<<_tile_index<<endl;
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
       float gl=gmn+lg*(gmx-gmn)/NGUIDE;
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
      printf("RichRing::generated WARNING: length too big %f\nMore warning messages suppressed\n",length);
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
      printf("RichRing::generated WARNING: lfoil  too big %f\nMore warning messages suppressed\n",lfoil);
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
      printf("RichRing::generated WARNING: lguide too big %f\nMore warning messages suppressed\n",lguide);
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


geant RichRing::lgeff(AMSPoint r, 
			 float u[3],
			 geant *lguide)
{
  float v[3],w[3];
  float f=0;
  int wnd,iw;
  int i,j,k;

  const float LG_Tran=4*RICHDB::foil_index/SQR(1+RICHDB::foil_index);
  const float Eff_Area=SQR(RICHDB::lg_length/pitch);
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
  /*
    HF1(1231001,f,1.);
    HF1(1231002,*lguide,1.);
  */
}
  }
  *lguide=RICHDB::lg_height;
  return 1;

#endif


  return LG_Tran*f;
}


void RichRing::refract(geant r1,
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

int RichRing::locsmpl(int id,
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



# undef ESC
# undef SQR
# undef PI


RichRing::RichRing(TrTrack* track,
			 int used,
			 int mused,
			 geant beta,
			 geant quality,
			 geant wbeta,
			 geant seed_beta,
			 geant recs[RICmaxpmts*RICnwindows][3],RichHitR *hitp[RICmaxpmts*RICnwindows],uinteger size,int ring,
			 uinteger status,integer build_charge):
  _status(status),_ptrack(track),_used(used),_mused(mused),_beta(beta),_quality(quality),_wbeta(wbeta)
{

#ifdef _OPENMP
  if(__DEBUGP__){
    cerr<<" *** "<<omp_get_thread_num()<<" STORING RING WITH BETA "<<_beta<<endl;
  }
#endif


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
  
  
  // Copy the residues plots
  _beta_direct.clear();
  _beta_reflected.clear();
  _hit_pointer.clear();
  _hit_used.clear();
  
  for(int i=0;i<size;i++){
    int reflected=fabs(recs[i][1]-seed_beta)<=fabs(recs[i][2]-seed_beta)?1:2;
    int closest=RichRing::closest(seed_beta,recs[i]);
    _beta_direct.push_back(recs[i][0]);
    _beta_reflected.push_back(recs[i][reflected]);
    int used=(closest==0?0:1);
    int which=0;
    for(RichRawEvent* hit=new RichRawEvent(_event)
	  ;hit;hit=hit->next(),which++){
      if(hit->getpointer()!=hitp[i]) continue;
      _hit_pointer.push_back(which);
      if(!hit->getbit(ring)) used+=2;
      _hit_used.push_back(used);
      break;
    }
  }
  
  if(build_charge){
    //    const float window_sigmas=3;
    const float window_sigmas=sqrt(_window);
    ReconRingNpexp(window_sigmas,!checkstatus(dirty_ring));

    if((RICHDB::scatprob)>0.){
#define SQR(x) ((x)*(x))
      float SigAng=_errorbeta*sqrt(geant(_used))/_beta/sqrt(SQR(_beta*_index)-1)/sqrt(SQR(_beta)-(SQR(_beta*_index)-1));
      float xmax=window_sigmas*SigAng/(RICHDB::scatang);
      float FscattCorr=1.-(RICHDB::scatprob*(1.-erf(xmax)));//gauss integral
      _npexp*=FscattCorr;
#undef SQR
    }

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


}







// A very simplistic MC support class: add fake hits to the RichRawEvent 
void RichRawEvent::Add(int pmt_channel){
  RichHitR *pointer=new RichHitR;
  pointer->Channel=pmt_channel;
  pointer->Npe=1.0;
  RichPMTChannel channel(pmt_channel);
  pointer->Coo[0]=channel.x();
  pointer->Coo[1]=channel.y();
  pointer->Coo[2]=channel.z();
  pointer->Status=(1<<28)|(1<<29);
  _mc_hits.push_back(pointer);
}

#include "TRandom.h" // For the fast MC 
#include "time.h"
RichRing* RichRing::FastMC(TrTrackR *tr,double beta){

  if(beta>0 || !RichRing::ComputeNpExp){
    // Compute npexp
    double beta_backup=_beta;    
    _beta=beta;
    double npexp=_npexp;
    double collected_npe=_collected_npe;
    double probkl=_probkl;
    _npexp=_collected_npe=_probkl=0;
    _beta=beta;
    const float window_sigmas=sqrt(_window);
    ReconRingNpexp(window_sigmas,!checkstatus(dirty_ring));
    
    if((RICHDB::scatprob)>0.){
#define SQR(x) ((x)*(x))
      float SigAng=_errorbeta*sqrt(geant(_used))/_beta/sqrt(SQR(_beta*_index)-1)/sqrt(SQR(_beta)-(SQR(_beta*_index)-1));
      float xmax=window_sigmas*SigAng/(RICHDB::scatang);
      float FscattCorr=1.-(RICHDB::scatprob*(1.-erf(xmax)));//gauss integral
      _npexp*=FscattCorr;
#undef SQR
    }
   
    // Restablish the values
    _beta=beta_backup;
    _npexp=npexp;
    _collected_npe=collected_npe;
    _probkl=probkl;
  }
  




  TrTrack track(tr);
  if(_event->Version()>=373){
    if(_event->nParticle()==0) return 0;
    if(!_event->pParticle(0)) return 0;
    RichRingR *ring=_event->pParticle(0)->pRichRing();
    if(!_ring) return 0;
    track._r.setp(ring->AMSTrPars[0],
                  ring->AMSTrPars[1],
                  ring->AMSTrPars[2]);
    track._d.SetTheta(ring->AMSTrPars[3]);
    track._d.SetPhi(ring->AMSTrPars[4]);
  }


  // A very simple idea: build a fake event using the raytracing methods, and reconstruct it
  memset(RichRawEvent::_beta_hit,0,RICmaxpmts*RICnwindows*3*sizeof(geant));  // Reset the beta hit arrays 

  // Generate the fake ring
  gRandom->SetSeed(time(0));
  
  RichRawEvent fake;
  int addCounter=0;
  for(int i=0;i<680*16;i++){
    if(photons_per_channel[i]==0) continue;
    int number=gRandom->Poisson(photons_per_channel[i]);
    if(number==0) continue;
    fake.Add(i);
    addCounter++;
  }
  if(!addCounter) return 0;



  // Initialization
  
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


  // Fast but not safe
  geant recs[RICmaxpmts*RICnwindows][3];
  geant mean[RICmaxpmts*RICnwindows][3];
  geant probs[RICmaxpmts*RICnwindows][3];
  integer size[RICmaxpmts*RICnwindows][3];
  integer mirrored[RICmaxpmts*RICnwindows][3];

  int bit=0;
  int cleanup=10;

  // Reconstruct it (cut and paste the whole code here
  integer actual=0,counter=0;
  RichHitR *hitp[RICmaxpmts*RICnwindows];

  RichRadiatorTileManager crossed_tile(&track);
  AMSPoint dirp,refp;
  AMSDir   dird,refd;

  dirp=crossed_tile.getemissionpoint();
  dird=crossed_tile.getemissiondir();
  refp=crossed_tile.getemissionpoint(1);
  refd=crossed_tile.getemissiondir(1);

  fake.Rewind();
  for(RichRawEvent* hit=&fake;hit;hit=hit->next()){

    // Reconstruct one hit
    hit->reconstruct(dirp,refp,
		     dird,refd,
		     betamin,
		     betamax,
		     recs[actual],
		     _index,
		     _kind_of_tile);


    if(!UseDirect) recs[actual][0]=0;
    if(!UseReflected) recs[actual][1]=recs[actual][2]=0;
    
    
    if(recs[actual][0]>0 || recs[actual][1]>0 || recs[actual][2]>0){	
      hitp[actual]=hit->getpointer();
      actual++;
    }
  }


  uinteger current_ring_status=_kind_of_tile==naf_kind?naf_ring:0;


  RichRing *first_done=0;

  if(1){
    integer best_cluster[2]={0,0};
    geant best_prob=-1;
    
    for(integer i=0;i<actual;i++)
      for(integer j=0;j<3;j++)
	if(recs[i][j]>0){
	  mean[i][j]=recs[i][j];
	  size[i][j]=1;mirrored[i][j]=j>0?1:0;
	  probs[i][j]=0;}
    
    
    
    for(integer i=0;i<actual;i++){
      if(recs[i][0]==-2.) continue; // Jump if direct is below threshold
      
      for(integer k=0;k<3;k++){
	if(recs[i][k]<betamin) continue; 
	for(integer j=0;j<actual;j++){
	  if(recs[j][0]==-2.) continue;
	  if(i==j) continue;
	  
	  integer better=RichRing::closest(recs[i][k],recs[j]);
	  
	  geant prob=(recs[i][k]-
		      recs[j][better])*
	    (recs[i][k]-
	     recs[j][better])/
	    RichRing::Sigma(recs[i][k],A,B)/
	    RichRing::Sigma(recs[i][k],A,B);
	  if(prob<_window){ //aprox. (3 sigmas)**2
	    probs[i][k]+=exp(-.5*prob);
	    mean[i][k]+=recs[j][better];
	    if(better>0) mirrored[i][k]++;
	    size[i][k]++;
	  }
	}
	if(best_prob<probs[i][k]){ 
	  best_prob=probs[i][k];	
	  best_cluster[0]=i;
	  best_cluster[1]=k;
	}
	
      }
    }

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
	  if(recs[i][0]==-2.) continue;
	  
	  integer closest=
	    RichRing::closest(beta_track,recs[i]);
	  
	  if(recs[i][closest]<betamin) continue;
	  geant prob=(recs[i][closest]-beta_track)*
	    (recs[i][closest]-beta_track)/
	    RichRing::Sigma(beta_track,A,B)/
	    RichRing::Sigma(beta_track,A,B);
	  
	  
	  if(prob>=_window) continue;
	  chi2+=prob;
	  //	  wsum+=hitp[i]->Npe;
	  //	  wbeta+=recs[i][closest]*hitp[i]->Npe;
	}
	
	beta_used=size[best_cluster[0]][best_cluster[1]];
	mirrored_used=mirrored[best_cluster[0]][best_cluster[1]];
	beta_track=mean[best_cluster[0]][best_cluster[1]]/geant(beta_used);
	
	if(wsum>0) wbeta/=wsum; else wbeta=0.;       
      }
      
      // Event quality numbers:
      // 0-> Number of used hits
      // 1-> chi2/Ndof
      
      // Fill the container
      //      RichRing* done=(RichRing *)AMSEvent::gethead()->addnext(AMSID("RichRing",0),
      RichRing* done=new RichRing(&track,
				  beta_used,
				  mirrored_used,
				  beta_track,
				  chi2/geant(beta_used-1),
				  wbeta,
				  recs[best_cluster[0]][best_cluster[1]],
				  recs,hitp,actual,bit,
				  current_ring_status,  //Status word
				  0);
      
      
      first_done=done;
    }  
  }
  
  return first_done;
}
