//  $Id: richrec.C,v 1.25 2001/09/11 08:06:44 choutko Exp $
#include <stdio.h>
#include <typedefs.h>
#include <cern.h>
#include <extC.h>
#include <math.h>
#include <point.h>
#include <amsgobj.h>
#include <event.h>
#include <commons.h>
#include <ntuple.h>
#include <richdbc.h>
#include <richrec.h>
#include <mccluster.h>
#include <iostream>
#include <trrec.h>
#include <root.h> 
//#include <vector>
//#include <valarray>

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


  // Construct event: channel signals

   int nhits=0;
   int nnoisy=0;
   geant signal=0;

   for(AMSRichMCHit* hits=(AMSRichMCHit *)AMSEvent::gethead()->getheadC("AMSRichMCHit",0,1);hits;hits=hits->next()){ // loop on signals
   
      uinteger channel=hits->getchannel();


      if(channel>=RICnwindows*RICHDB::total){
         cerr<< "AMSRichRawEvent::mc_build-ChannelNoError "<<channel<<endl;
          break;
      }
      hits->puthit(hitn); // To construct the pointer list

//      if(hits->getid()!=Noise && hits->getid()!=Cerenkov_photon)
//          continue;    

      if(hits->getid()==Cerenkov_photon) nhits++; //else nnoisy++;
      if(hits->getid()==Noise) nnoisy++;

      if(hits->testlast()){ // last signal in the hit so construct it
        if(nnoisy>0) signal=AMSRichMCHit::noise(); 
          else if(nhits>0) signal=AMSRichMCHit::adc_empty();
        signal+=AMSRichMCHit::adc_hit(nhits);
        nnoisy=0;
        nhits=0;
        if(integer(signal)>=integer(RICHDB::c_ped)+integer(RICHDB::ped)){
         AMSEvent::gethead()->addnext(AMSID("AMSRichRawEvent",0),
          new AMSRichRawEvent(channel,integer(signal)));
          hitn++;
          signal=0;
        } else {
       for(AMSRichMCHit* clean=(AMSRichMCHit *)AMSEvent::gethead()->getheadC("AMSRichMCHit",0,1);clean;clean=clean->next()){
         if(clean->gethit()==hitn) clean->puthit(0);
         if(clean->gethit()==-1) break;
       }
	}
      }
   }
}


void AMSRichRawEvent::_writeEl(){
  
  RICEventNtuple* cluster=AMSJob::gethead()->getntuple()->Get_richevent();
  
  if(cluster->Nhits>=MAXRICHITS) return;

  cluster->channel[cluster->Nhits]=_channel;
  cluster->adc[cluster->Nhits]=_counts;
  cluster->x[cluster->Nhits]=RICHDB::x(_channel);
  cluster->y[cluster->Nhits]=RICHDB::y(_channel);
  cluster->Nhits++;

}



void AMSRichRawEvent::reconstruct(AMSPoint origin,AMSPoint origin_ref,
                                  AMSDir direction,AMSDir direction_ref,
				  geant betamin,geant betamax,
#ifdef __SAFE__
				  geant_small_array &betas){
#else
				  geant *betas){
#endif
  // Reconstruct the beta values for this hit. Assumes direction as unitary
  static const geant z=RICradpos-RICHDB::rad_height-RICHDB::height;
  
  geant x=RICHDB::x(_channel);
  geant y=RICHDB::y(_channel);

  betas[0]=0;
  betas[1]=0;
  betas[2]=0;
  
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
  geant theta=atan2(R,fabs(origin[2]-z));
  geant h=origin[2]-RICradpos+RICHDB::rad_height;
  static const geant H=RICHDB::height;
  static const geant n=RICHDB::rad_index;
  
  geant u=fabs(sin(theta)/n);
  integer time_out=0;
  geant delta=1,f,g;

  while(delta>1e-5 && time_out<10){
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
    geant phi=atan2(y-origin[1],x-origin[0]);
    betas[0]=direction[0]*cos(phi)*u+
      direction[1]*sin(phi)*u-direction[2]*sqrt(1-u*u);
    betas[0]=1/RICHDB::rad_index/betas[0];
    if(betas[0]<betamin){ // If the beta as direct is below threshold
      betas[0]=-2.;       // it is the primary passing through the LG so
      return;}            // stop the recontruction.
    else if(betas[0]>betamax) betas[0]=-1;
  }else{
    betas[0]=0;    // Wrong theta
  }
  
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
    u=sin(theta)/RICHDB::rad_index;
    dir.setp(u*cos(phi),u*sin(phi),-sqrt(1-u*u));
    betas[j]=1/RICHDB::rad_index/(dir[0]*direction_ref[0]+
				  dir[1]*direction_ref[1]+
				  dir[2]*direction_ref[2]);

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
  if(j>3) cout <<"****** TOO MANY BETAS"<<endl;
#endif
}


integer AMSRichRawEvent::reflexo(AMSPoint origin,AMSPoint *ref_point){
  geant false_height=RICHDB::bottom_radius*RICHDB::height/
    (RICHDB::bottom_radius-RICHDB::top_radius);
  geant zk=(RICHDB::bottom_radius/false_height)*
    (RICHDB::bottom_radius/false_height);
  geant c2=zk/(1+zk);
  geant c=sqrt(c2);
  geant alp=1/sqrt(1+zk);
  
  geant xf=RICHDB::x(_channel);
  geant yf=RICHDB::y(_channel);
  geant zf=-false_height;
  //  geant zf=RICradpos-RICHDB::rad_height-false_height;
  geant x=origin[0],y=origin[1],
    //    z=origin[3]-false_height+RICHDB::height;
    z=origin[3]-RICradpos+RICHDB::rad_height-false_height+RICHDB::height;


  AMSPoint initial(x,y,z),final(xf,yf,zf);

  geant f1=x*xf-y*yf,
    f2=x*zf+z*xf,
    f3=x*yf+y*xf,
    f4=y*zf+z*yf;

  geant a4=4*(f3*f3+f1*f1),
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
  geant sols[4];
  
  SOLVE(a3,a2,a1,a0,sols,mt);

  //Count the number of solutions
  mt=0;
  for(integer i=0;i<4;i++) if(sols[i]!=0.) mt++;

  AMSPoint planes[8];
  AMSPoint this_plane;
  integer nsols=0;

  for(integer i=0;i<mt;i++){
    geant b=1/(1+zk)-sols[i]*sols[i];
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
       && ref_point[good][0]*ref_point[good][0]+
          ref_point[good][1]*ref_point[good][1]-zk*
          ref_point[good][3]*ref_point[good][3]<1e-4){

      (ref_point[good]).setp((ref_point[good])[0],
			     (ref_point[good])[1],
			     (ref_point[good])[2]+false_height-
			     RICHDB::height-RICHDB::rad_height+RICradpos);
      good++;
    }

  }

  return good;
}



void AMSRichRing::build(){

  //  integer track_number=0;
  
  // All these arrays are for speed up the reconstruction
  // They should be move to a dynamic list (like the containers)
  // using, for example, a structure (~completely public class)

  int ARRAYSIZE=(AMSEvent::gethead()->getC("AMSRichRawEvent",0))->getnelem();
  if(ARRAYSIZE==0) return;
  
#ifdef __SAFE__
  //  safe_array<geant_small_array,MAXRICHITS> recs,mean,probs;
  //  safe_array<integer_small_array,MAXRICHITS> size;
  //  safe_array<geant_small_array,3*MAXRICHITS> recs,mean,probs;
  //  safe_array<integer_small_array,3*MAXRICHITS> size;
  safe_array<geant_small_array,1> recs(ARRAYSIZE),mean(ARRAYSIZE),probs(ARRAYSIZE);
  safe_array<integer_small_array,1> size(ARRAYSIZE);

#else
  geant recs[RICmaxpmts*RICnwindows/2][3];
  geant mean[RICmaxpmts*RICnwindows/2][3];
  geant probs[RICmaxpmts*RICnwindows/2][3];
  integer size[RICmaxpmts*RICnwindows/2][3];
#endif
  

  //  for(AMSTrTrack *track=(AMSTrTrack *)AMSEvent::gethead()->
  //   	getheadC("AMSTrTrack",0);track;track=track->next()){
  AMSTrTrack *track;

  for(int id=0;;){
    track=(AMSTrTrack *)AMSEvent::gethead()->getheadC("AMSTrTrack",id++,1);
    if(!track) break;
    for(;track;track=track->next()){

      // Tentative use of higher quality track reconstructed
      //    if(!(track->TOFOK())) continue; //CJD

      //      track_number++;
      //      track_number=track->getpos();
      AMSPoint point;
      number theta,phi,length;
      AMSPoint pnt(0.,0.,RICradpos);
      AMSDir dir(0.,0.,-1.);
      
      track->interpolate(pnt,dir,point,theta,phi,length);
      
      //============================================================
      // Here we should check if the track goes through the radiator
      //============================================================
      
      // This should be done better
      if(point[0]*point[0]+point[1]*point[1]>RICGEOM.top_radius*RICGEOM.top_radius)
	continue;   


      //================================================
      // Compute the best emition point:
      //   -Try numeric integration (only one time). 
      //   -If it fails use old parametrization
      //================================================

      geant height_direct=RICHDB::mean_height();
      geant height_reflected=height_direct-0.1; // 0.2 if NaF

      if(height_direct==-1.){   // Error in the computation

	//============================================================
	// PARAMETRISATION OF THE RECONSTRUCCION HEIGHT
	// Parametrisation optimised in absence of magnetic field,
	// mirror top radius 58.6 cm, and aerogel according (including 
	// rayleigh scattering).
	// TODO: Parametrise it using the AMS simulation
	//============================================================
	
	const geant fd=exp(.78-1.23*RICHDB::rad_height+
			   .131*RICHDB::rad_height*RICHDB::rad_height);
	const geant fr=exp(.90-1.19*RICHDB::rad_height+
			   .130*RICHDB::rad_height*RICHDB::rad_height);
	
	// Distance from the bottom plane of radiator of the optimum 
	// reconstruction point. THIS SHOULD BE REOPTIMISED
	
	height_direct=1/fd-RICHDB::rad_height/fabs(cos(theta))/
	  (exp(RICHDB::rad_height*fd/fabs(cos(theta)))-1);
	height_reflected=1/fr-RICHDB::rad_height/fabs(cos(theta))/
	  (exp(RICHDB::rad_height*fr/fabs(cos(theta)))-1);
	
	// Fine tunning
	
	height_direct+=-.047+.14*RICHDB::rad_height-.073*
	  RICHDB::rad_height*RICHDB::rad_height;
	height_reflected+=-.048+.06*RICHDB::rad_height-.071*
	  RICHDB::rad_height*RICHDB::rad_height;
    }


      //============================================================
      // PARAMETRISATION OF THE HIT ERROR
      // Obtained using the same conditions as the reconstruction 
      // height parametrisation. This depends in the light guides 
      // dimensions
      //============================================================
      
      
      // Parameters
      
      geant A=(-2.81+13.5*(RICHDB::rad_index-1.)-18.*
	       (RICHDB::rad_index-1.)*(RICHDB::rad_index-1.))*
	RICHDB::rad_height/RICHDB::height*40./2.;
      
      geant B=(2.90-11.3*(RICHDB::rad_index-1.)+18.*
	       (RICHDB::rad_index-1.)*(RICHDB::rad_index-1.))*
	RICHDB::rad_height/RICHDB::height*40./2.;
      
      
      // Reconstruction threshold: maximum beta admited
      geant betamax=1.+3.e-2*(A+B);
      
      // Next obtained by Casaus: minimum beta admited to avoid noise caused
      // by the particle going through the PMTs
      //    geant betamin=(1.+0.05*(RICHDB::rad_index-1.))/RICHDB::rad_index;
      // Value corrected by Carlos D.
      geant betamin=(1.+RICthreshold*(RICHDB::rad_index-1.))/RICHDB::rad_index;
      
      
      
      //==================================================
      // The reconstruction starts here
      //==================================================
      
      // Reconstructing it as direct
      
      pnt.setp(0.,0.,RICradpos-RICHDB::rad_height+height_direct);
      
      
      AMSPoint dirp,refp;
      track->interpolate(pnt,dir,dirp,theta,phi,length);
      
      AMSDir dird(theta,phi);
      
      pnt.setp(0.,0.,RICradpos-RICHDB::rad_height+height_reflected);
      track->interpolate(pnt,dir,refp,theta,phi,length);
      
      AMSDir refd(theta,phi); // Reflected case ;)
      
      integer actual=0;
      
      for(AMSRichRawEvent* hit=(AMSRichRawEvent *)AMSEvent::gethead()->
	    getheadC("AMSRichRawEvent",0);hit;hit=hit->next()){
	
	// Checks bounds
	if(actual>=RICmaxpmts*RICnwindows/2) {
	  cout << "AMSRichRing::build : Event too long."<<endl;
	  break;
	}
	
	// Reconstruct one hit
	hit->reconstruct(dirp,refp,
			 dird,refd,
			 betamin,betamax,recs[actual]);
	
	if(recs[actual][0]>0 || recs[actual][1]>0 || recs[actual][2]>0){	
	  
	  // Initialise arrays
	  for(integer j=0;j<3;j++)
	    if(recs[actual][j]>0){
	      mean[actual][j]=recs[actual][j];
	      size[actual][j]=1;
	      probs[actual][j]=0;}
	  actual++;
	}
      }
      
      // Look for clusters
      integer best_cluster[2]={0,0};
      geant best_prob=-1;

      
      for(integer i=0;i<actual;i++){
	if(recs[i][0]==-2.) continue; // Jump if direct is below threshold
	for(integer k=0;k<3;k++){
	  if(recs[i][k]<betamin) continue; 
	  for(integer j=0;j<actual;j++){
	    if(recs[j][0]==-2.) continue;
	    if(i==j) continue;
	    integer better=AMSRichRing::closest(recs[i][k],recs[j]);
	    
	    geant prob=(recs[i][k]-
			recs[j][better])*
	      (recs[i][k]-
	       recs[j][better])/
	      AMSRichRing::Sigma(recs[i][k],A,B)/
	      AMSRichRing::Sigma(recs[i][k],A,B);
	    if(prob<9){ //aprox. (3 sigmas)**2
	      probs[i][k]+=exp(-.5*prob);
	      mean[i][k]+=recs[j][better];
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
      // Add a ring element with the results. Not added to ntuple yet
      if(best_prob>0){
	
	// This piece is a bit redundant: computes chi2
	
	geant chi2=0.;
	geant beta_track=
	  recs[best_cluster[0]][best_cluster[1]];
	
	for(integer i=0;i<actual;i++){
	  if(recs[i][0]==-2.) continue;
	  integer closest=
	    AMSRichRing::closest(beta_track,recs[i]);
	  
	  if(recs[i][closest]<betamin) continue;
	  geant prob=(recs[i][closest]-beta_track)*
	    (recs[i][closest]-beta_track)/
	    AMSRichRing::Sigma(beta_track,A,B)/
	    AMSRichRing::Sigma(beta_track,A,B);
	  if(prob>=9) continue;
	  chi2+=prob;
	}
	
	integer beta_used=size[best_cluster[0]][best_cluster[1]];
	beta_track=mean[best_cluster[0]][best_cluster[1]]/geant(beta_used);

	
	// Event quality numbers:
	// 0-> Number of used hits
	// 1-> chi2/Ndof
	
	// Fill the container
	
	AMSEvent::gethead()->addnext(AMSID("AMSRichRing",0),
				     new AMSRichRing(track,
						     beta_used,
						     beta_track,
						     chi2/geant(beta_used),
						     1.  //Unused by now
						     ));
	
      } else {
//	// Add empty ring to keep track of no recostructed tracks
//	AMSEvent::gethead()->addnext(AMSID("AMSRichRing",0),
//				     new AMSRichRing(track,
//						     0,
//						     0.,
//						     0.,
//						     0. // Unused by now
//						     ));
      }
    }
    
  }
  
}  





void AMSRichRing::rebuild(AMSTrTrack *track){

// in fact copy of build
// should be replace by smth more smart


// check if rich rings lardy exist
    AMSRichRing *ring=(AMSRichRing *)AMSEvent::gethead()->getheadC("AMSRichRing",0);
   if(ring && ring->_ptrack->getpattern()>=0)return;

   

  //  integer track_number=0;
  
  // All these arrays are for speed up the reconstruction
  // They should be move to a dynamic list (like the containers)
  // using, for example, a structure (~completely public class)

  int ARRAYSIZE=(AMSEvent::gethead()->getC("AMSRichRawEvent",0))->getnelem();
  if(ARRAYSIZE==0) return;
  
#ifdef __SAFE__
  //  safe_array<geant_small_array,MAXRICHITS> recs,mean,probs;
  //  safe_array<integer_small_array,MAXRICHITS> size;
  //  safe_array<geant_small_array,3*MAXRICHITS> recs,mean,probs;
  //  safe_array<integer_small_array,3*MAXRICHITS> size;
  safe_array<geant_small_array,1> recs(ARRAYSIZE),mean(ARRAYSIZE),probs(ARRAYSIZE);
  safe_array<integer_small_array,1> size(ARRAYSIZE);

#else
  geant recs[RICmaxpmts*RICnwindows/2][3];
  geant mean[RICmaxpmts*RICnwindows/2][3];
  geant probs[RICmaxpmts*RICnwindows/2][3];
  integer size[RICmaxpmts*RICnwindows/2][3];
#endif
  

  //  for(AMSTrTrack *track=(AMSTrTrack *)AMSEvent::gethead()->
  //   	getheadC("AMSTrTrack",0);track;track=track->next()){


      // Tentative use of higher quality track reconstructed
      //    if(!(track->TOFOK())) continue; //CJD

      //      track_number++;
      //      track_number=track->getpos();
      AMSPoint point;
      number theta,phi,length;
      AMSPoint pnt(0.,0.,RICradpos);
      AMSDir dir(0.,0.,-1.);
      
      track->interpolate(pnt,dir,point,theta,phi,length);
      
      //============================================================
      // Here we should check if the track goes through the radiator
      //============================================================
      
      // This should be done better
      if(point[0]*point[0]+point[1]*point[1]>RICGEOM.top_radius*RICGEOM.top_radius)
	return;   


      //================================================
      // Compute the best emition point:
      //   -Try numeric integration (only one time). 
      //   -If it fails use old parametrization
      //================================================

      geant height_direct=RICHDB::mean_height();
      geant height_reflected=height_direct-0.1; // 0.2 if NaF

      if(height_direct==-1.){   // Error in the computation

	//============================================================
	// PARAMETRISATION OF THE RECONSTRUCCION HEIGHT
	// Parametrisation optimised in absence of magnetic field,
	// mirror top radius 58.6 cm, and aerogel according (including 
	// rayleigh scattering).
	// TODO: Parametrise it using the AMS simulation
	//============================================================
	
	const geant fd=exp(.78-1.23*RICHDB::rad_height+
			   .131*RICHDB::rad_height*RICHDB::rad_height);
	const geant fr=exp(.90-1.19*RICHDB::rad_height+
			   .130*RICHDB::rad_height*RICHDB::rad_height);
	
	// Distance from the bottom plane of radiator of the optimum 
	// reconstruction point. THIS SHOULD BE REOPTIMISED
	
	height_direct=1/fd-RICHDB::rad_height/fabs(cos(theta))/
	  (exp(RICHDB::rad_height*fd/fabs(cos(theta)))-1);
	height_reflected=1/fr-RICHDB::rad_height/fabs(cos(theta))/
	  (exp(RICHDB::rad_height*fr/fabs(cos(theta)))-1);
	
	// Fine tunning
	
	height_direct+=-.047+.14*RICHDB::rad_height-.073*
	  RICHDB::rad_height*RICHDB::rad_height;
	height_reflected+=-.048+.06*RICHDB::rad_height-.071*
	  RICHDB::rad_height*RICHDB::rad_height;
    }


      //============================================================
      // PARAMETRISATION OF THE HIT ERROR
      // Obtained using the same conditions as the reconstruction 
      // height parametrisation. This depends in the light guides 
      // dimensions
      //============================================================
      
      
      // Parameters
      
      geant A=(-2.81+13.5*(RICHDB::rad_index-1.)-18.*
	       (RICHDB::rad_index-1.)*(RICHDB::rad_index-1.))*
	RICHDB::rad_height/RICHDB::height*40./2.;
      
      geant B=(2.90-11.3*(RICHDB::rad_index-1.)+18.*
	       (RICHDB::rad_index-1.)*(RICHDB::rad_index-1.))*
	RICHDB::rad_height/RICHDB::height*40./2.;
      
      
      // Reconstruction threshold: maximum beta admited
      geant betamax=1.+3.e-2*(A+B);
      
      // Next obtained by Casaus: minimum beta admited to avoid noise caused
      // by the particle going through the PMTs
      //    geant betamin=(1.+0.05*(RICHDB::rad_index-1.))/RICHDB::rad_index;
      // Value corrected by Carlos D.
      geant betamin=(1.+RICthreshold*(RICHDB::rad_index-1.))/RICHDB::rad_index;
      
      
      
      //==================================================
      // The reconstruction starts here
      //==================================================
      
      // Reconstructing it as direct
      
      pnt.setp(0.,0.,RICradpos-RICHDB::rad_height+height_direct);
      
      
      AMSPoint dirp,refp;
      track->interpolate(pnt,dir,dirp,theta,phi,length);
      
      AMSDir dird(theta,phi);
      
      pnt.setp(0.,0.,RICradpos-RICHDB::rad_height+height_reflected);
      track->interpolate(pnt,dir,refp,theta,phi,length);
      
      AMSDir refd(theta,phi); // Reflected case ;)
      
      integer actual=0;
      
      for(AMSRichRawEvent* hit=(AMSRichRawEvent *)AMSEvent::gethead()->
	    getheadC("AMSRichRawEvent",0);hit;hit=hit->next()){
	
	// Checks bounds
	if(actual>=RICmaxpmts*RICnwindows/2) {
	  cout << "AMSRichRing::build : Event too long."<<endl;
	  break;
	}
	
	// Reconstruct one hit
	hit->reconstruct(dirp,refp,
			 dird,refd,
			 betamin,betamax,recs[actual]);
	
	if(recs[actual][0]>0 || recs[actual][1]>0 || recs[actual][2]>0){	
	  
	  // Initialise arrays
	  for(integer j=0;j<3;j++)
	    if(recs[actual][j]>0){
	      mean[actual][j]=recs[actual][j];
	      size[actual][j]=1;
	      probs[actual][j]=0;}
	  actual++;
	}
      }
      
      // Look for clusters
      integer best_cluster[2]={0,0};
      geant best_prob=-1;

      
      for(integer i=0;i<actual;i++){
	if(recs[i][0]==-2.) continue; // Jump if direct is below threshold
	for(integer k=0;k<3;k++){
	  if(recs[i][k]<betamin) continue; 
	  for(integer j=0;j<actual;j++){
	    if(recs[j][0]==-2.) continue;
	    if(i==j) continue;
	    integer better=AMSRichRing::closest(recs[i][k],recs[j]);
	    
	    geant prob=(recs[i][k]-
			recs[j][better])*
	      (recs[i][k]-
	       recs[j][better])/
	      AMSRichRing::Sigma(recs[i][k],A,B)/
	      AMSRichRing::Sigma(recs[i][k],A,B);
	    if(prob<9){ //aprox. (3 sigmas)**2
	      probs[i][k]+=exp(-.5*prob);
	      mean[i][k]+=recs[j][better];
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
      // Add a ring element with the results. Not added to ntuple yet
      if(best_prob>0){
	
	// This piece is a bit redundant: computes chi2
	
	geant chi2=0.;
	geant beta_track=
	  recs[best_cluster[0]][best_cluster[1]];
	
	for(integer i=0;i<actual;i++){
	  if(recs[i][0]==-2.) continue;
	  integer closest=
	    AMSRichRing::closest(beta_track,recs[i]);
	  
	  if(recs[i][closest]<betamin) continue;
	  geant prob=(recs[i][closest]-beta_track)*
	    (recs[i][closest]-beta_track)/
	    AMSRichRing::Sigma(beta_track,A,B)/
	    AMSRichRing::Sigma(beta_track,A,B);
	  if(prob>=9) continue;
	  chi2+=prob;
	}
	
	integer beta_used=size[best_cluster[0]][best_cluster[1]];
	beta_track=mean[best_cluster[0]][best_cluster[1]]/geant(beta_used);

	
	// Event quality numbers:
	// 0-> Number of used hits
	// 1-> chi2/Ndof
	
	// Fill the container
	
	AMSEvent::gethead()->addnext(AMSID("AMSRichRing",0),
				     new AMSRichRing(track,
						     beta_used,
						     beta_track,
						     chi2/geant(beta_used),
						     1.  //Unused by now
						     ));
	
      } else {
//	// Add empty ring to keep track of no recostructed tracks
//	AMSEvent::gethead()->addnext(AMSID("AMSRichRing",0),
//				     new AMSRichRing(track,
//						     0,
//						     0.,
//						     0.,
//						     0. // Unused by now
//						     ));
      }
  
}  



void AMSRichRing::_writeEl(){
  
  RICRing* cluster=AMSJob::gethead()->getntuple()->Get_ring();
  
  if(cluster->NRings>=MAXTRTR02) return;

  if(_ptrack->checkstatus(AMSDBc::NOTRACK))cluster->track[cluster->NRings]=-1;
  else if(_ptrack->checkstatus(AMSDBc::TRDTRACK))cluster->track[cluster->NRings]=-1;
  else if(_ptrack->checkstatus(AMSDBc::ECALTRACK))cluster->track[cluster->NRings]=-1;
  else cluster->track[cluster->NRings]=_ptrack->getpos();
  cluster->used[cluster->NRings]=_used;
  cluster->beta[cluster->NRings]=_beta;
  cluster->errorbeta[cluster->NRings]=_errorbeta;
  cluster->quality[cluster->NRings]=_quality;
  cluster->Z[cluster->NRings]=_charge;
  cluster->NRings++;

}

void AMSRichRing::CalcBetaError(){
  geant A=(-2.81+13.5*(RICHDB::rad_index-1.)-18.*
	   (RICHDB::rad_index-1.)*(RICHDB::rad_index-1.))*
    RICHDB::rad_height/RICHDB::height*40./2.;
      
  geant B=(2.90-11.3*(RICHDB::rad_index-1.)+18.*
	   (RICHDB::rad_index-1.)*(RICHDB::rad_index-1.))*
    RICHDB::rad_height/RICHDB::height*40./2.;


  _errorbeta=_used>0?
    sqrt(AMSRichRing::Sigma(_beta,A,B)*
	 AMSRichRing::Sigma(_beta,A,B)/geant(_used)+0.016e-2*0.016e-2):1;
}




