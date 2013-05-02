//  $Id: mccluster.C,v 1.87 2013/05/02 21:07:22 zhukov Exp $
// Author V. Choutko 24-may-1996
 

#include "mccluster.h"
#include "extC.h"
#include "commons.h"
#include <math.h>
#include "job.h"
#include "event.h"
#include "amsstl.h"
#include "cont.h"
#include "ntuple.h"
#include "richid.h"
#include "richdbc.h"
#include "ecaldbc.h"
#ifdef __G4AMS__
#include "g4util.h"
#endif
using namespace std;
extern "C" void indetra_(float&);

number AMSEcalMCHit::impoint[2];
number AMSEcalMCHit::leadedep[ecalconst::ECSLMX];

integer AMSTRDMCCluster::_NoiseMarker(555);

void AMSTRDMCCluster::sitrdhits(
				integer idsoft , geant vect[],geant edep, geant ekin, geant step, integer ipart, integer itra, integer gtrkid ){


        AMSPoint xgl(vect[0],vect[1],vect[2]);
        AMSDir xvec(vect[3],vect[4],vect[5]);
      AMSEvent::gethead()->addnext(AMSID("AMSTRDMCCluster",0),
      new AMSTRDMCCluster(idsoft,xgl,xvec,step,ekin,edep,ipart,itra, gtrkid));

}

void AMSTRDMCCluster::sitrdnoise(){
}


void AMSTRDMCCluster::_writeEl(){
  integer flag =    (IOPA.WriteAll%10==1)
                 || (IOPA.WriteAll%10==0 && checkstatus(AMSDBc::USED));
  if(AMSTRDMCCluster::Out(flag)){
#ifdef __WRITEROOT__
    AMSJob::gethead()->getntuple()->Get_evroot02()->AddAMSObject(this);
#endif
/*
  TRDMCClusterNtuple* TRDMCClusterN = AMSJob::gethead()->getntuple()->Get_trdclmc();
  
  if (TRDMCClusterN->Ntrdclmc>=MAXTRDCLMC) return;

    TRDMCClusterN->Layer[TRDMCClusterN->Ntrdclmc]=_idsoft.getlayer();
    TRDMCClusterN->Ladder[TRDMCClusterN->Ntrdclmc]=_idsoft.getladder();
    TRDMCClusterN->Tube[TRDMCClusterN->Ntrdclmc]=_idsoft.gettube();
    TRDMCClusterN->Edep[TRDMCClusterN->Ntrdclmc]=_edep;
    TRDMCClusterN->Ekin[TRDMCClusterN->Ntrdclmc]=_ekin;
    TRDMCClusterN->ParticleNo[TRDMCClusterN->Ntrdclmc]=_ipart;
//    TRDMCClusterN->TrackNo[TRDMCClusterN->Ntrdclmc]=_itra;
    for(int i=0;i<3;i++)TRDMCClusterN->Xgl[TRDMCClusterN->Ntrdclmc][i]=_xgl[i];
    TRDMCClusterN->Step[TRDMCClusterN->Ntrdclmc]=_step;
    TRDMCClusterN->Ntrdclmc++;
*/
  }   
}

void AMSTRDMCCluster::_copyEl(){
}

void AMSTRDMCCluster::_printEl(ostream & stream){
stream <<"AMSTRDMCCluster "<<_idsoft<<" "
 <<_edep<<" "<<_ekin<<" "<<_itra<<" "<<_xgl<<endl;
}

void AMSTRDMCCluster::init(){
// at the momenent : homemade trd also for g4

#ifdef __G4AMS__
if(MISCFFKEY.G3On){
#endif
if(TRDMCFFKEY.mode<3 && TRDMCFFKEY.mode>=0){
  indetra_(TRDMCFFKEY.Gdens);
cout<< "<---- AMSJob::_sitrdinitjob-I-TRDOption "<<TRDMCFFKEY.mode<<" Initialized"<<endl<<endl;
}
else if(TRDMCFFKEY.mode==3){
cerr<< "<---- AMSJob::_sitrdinitjob-F-Option "<<TRDMCFFKEY.mode<<" NotYetImplemented"<<endl<<endl;
exit(1);
}
#ifdef __G4AMS__
}
#endif
}


integer AMSTRDMCCluster::Out(integer status){
static integer init=0;
static integer WriteAll=0;
if(init == 0){
 init=1;
 integer ntrig=AMSJob::gethead()->gettriggerN();
 for(int n=0;n<ntrig;n++){
   if(strcmp("AMSTRDMCCluster",AMSJob::gethead()->gettriggerC(n))==0){
     WriteAll=1;
     break;
   }
 }
}
return (WriteAll || status);
}




//------------- TRACKER -------------
// PZ moved to trmccluster.C 

//------------- TOF -------------


void AMSTOFMCCluster::sitofhits(integer idsoft , geant vect[],geant edep, 
geant tofg, geant beta, geant edepr, geant step, integer parentid, integer particle, integer gtrkid){
  //        Very Temporary
  AMSPoint pnt(vect[0],vect[1],vect[2]);
   AMSEvent::gethead()->addnext(AMSID("AMSTOFMCCluster",0),
   new AMSTOFMCCluster(idsoft,pnt,edep,tofg,beta,edepr,step,parentid,particle,gtrkid));
}

//----------------PMT hit
void AMSTOFMCPmtHit::sitofpmthits(integer pmtid,integer parentid,geant phtim, geant phtiml,
geant phtral, geant phekin, geant pos[],geant dir[]){
//  
  AMSPoint pp(pos[0],pos[1],pos[2]);
  AMSDir   pd(dir[0],dir[1],dir[2]);
  AMSEvent::gethead()->addnext(AMSID("AMSTOFMCPmtHit",0),
                               new AMSTOFMCPmtHit(pmtid,parentid,phtim,phtiml,phtral,phekin,pp,pd));
}

void AMSTOFMCPmtHit::sitofpmtpar(geant phtimp,geant phamp){
   _phtimp=phtimp;
   _phamp=phamp;
  //cout<<"phtimp="<<_phtimp<<" phamp="<<phamp<<endl;
}

//------------- ANTI -------------

void AMSAntiMCCluster::siantihits(integer idsoft , geant vect[],geant edep, 
geant tofg, integer gtrkid){
  //        Very Temporary
  AMSPoint pnt(vect[0],vect[1],vect[2]);
   AMSEvent::gethead()->addnext(AMSID("AMSAntiMCCluster",0),
   new AMSAntiMCCluster(idsoft,pnt,edep,tofg, gtrkid));
}

//------------- ECAL --------------

void AMSEcalMCHit::siecalhits(integer idsoft , geant vect[],geant edep, 
                                                           geant tofg){
//     
  AMSPoint pnt(vect[0],vect[1],vect[2]);
  int isl;
//
  isl=idsoft/100000;//s-layer(1-9)  
  AMSEvent::gethead()->addnext(AMSID("AMSEcalMCHit",isl-1),
                      new AMSEcalMCHit(idsoft,pnt,edep,tofg));
  //cout <<isl<<" "<<pnt<<endl;
}
void AMSEcalMCHit::_writeEl(){
  integer flag = (((IOPA.WriteAll%10==1) && (ECMCFFKEY.mch2root==1))
                  || (ECMCFFKEY.mch2root==2));
  
  if(AMSEcalMCHit::Out(flag)){
#ifdef __WRITEROOT__
      AMSJob::gethead()->getntuple()->Get_evroot02()->AddAMSObject(this);
#endif
  }
}


//--------RICH------------

void AMSRichMCHit::sirichhits(integer id,
			      integer pmt, geant position[], // used to compute channel
			      geant origin[],geant momentum[],integer status, integer gtrkid, integer gparentid)
{
  AMSPoint r(origin[0],origin[1],origin[2]);
  double norma=sqrt(momentum[0]*momentum[0]+
		    momentum[1]*momentum[1]+
		    momentum[2]*momentum[2]);
  AMSPoint u(momentum[0]/norma,momentum[1]/norma,momentum[2]/norma);
  geant adc;

  adc=0;
 

 if(pmt>RICHDB::total || pmt<0){
    cerr<<"AMSRichMCHit::sirichhits-ErrorNoPMT " << pmt<<endl;
    return;
  }

 //

 // Move the point ti RICH COORDINATES
 AMSPoint local_position(position);
 local_position=RichAlignment::AMSToRich(local_position);
 RichPMTChannel channel(pmt,local_position[0],local_position[1]);


#ifdef __AMSDEBUG__
/*
  cout <<"Hit position "<<position[0]<<","<<position[1]<<endl
       <<"PMT number "<<pmt<<endl
       <<"RICHDB position "<<RICHDB::pmt_p[pmt][0]<<" "<<RICHDB::pmt_p[pmt][1]<<endl
       <<"RICHIdGeom pos "<<AMSRICHIdGeom::pmt_pos(pmt,0)<<" "<<AMSRICHIdGeom::pmt_pos(pmt,1)<<endl
       <<"Asigned channel "<<channel.getchannel()<<endl
       <<"Asigned pixel "<<channel.getpixel()<<endl
       <<"Asigned position "<<channel.x()<<","<<channel.y()<<endl;
*/
#endif

  // The primary history does not has good channel info so we do it by hand

  if(status==Status_primary_rad ||
     status==Status_primary_tracker ||
     status==Status_primary_tof ||
     status==Status_primary_radb)
    AMSEvent::gethead()->addnext(AMSID("AMSRichMCHit",0),
				 new AMSRichMCHit(id,-1,0,
						  r,u,status, gtrkid, gparentid));
  
  
  else
    
    if(channel.pmt_geom_id>=0){
      // Take into account the quantum efficiency
      geant dummy;
      if(RNDM(dummy)<=channel.rel_eff)
	AMSEvent::gethead()->addnext(AMSID("AMSRichMCHit",0),
				     new AMSRichMCHit(id,channel.GetPacked(),adc,
						      r,u,status, gtrkid, gparentid));
}
}


void AMSRichMCHit::noisyhit(integer channel,int mode){
  AMSPoint r(0,0,0);
  AMSPoint u(0,0,0);
  AMSEvent::gethead()->addnext(AMSID("AMSRichMCHit",0),
			       new AMSRichMCHit(Noise,channel,AMSRichMCHit::noise(channel,mode),r,u,Status_Noise,-2,-2));
}


geant AMSRichMCHit::adc_hit(integer n,integer channel,int mode){ // ADC counts for a set of hits
  geant u1,u2,dummy,r;
  assert(mode==0 || mode==1);

  RichPMTChannel calibration(channel);

  /*
  //  r=sqrt(-2.*log(1.-RNDM(dummy)));
  //  u1=r*sin(RNDM(dummy)*6.28318595886);   // This constant (2pi)should be moved to a namespace


  return rnormx()*calibration.getsgain(mode)*sqrt(double(n))+n*calibration.getgain(mode);
  */
  geant value=0;
  for(int i=0;i<n;i++){  // Start adding hits
    value+=calibration.SimulateSinglePE(mode);

  }
  return value;


}

geant AMSRichMCHit::adc_empty(integer channel,integer mode){ // ADC count without a hit
  geant u1,u2,dummy,r;
  assert(mode==0 || mode==1);
  RichPMTChannel calibration(channel);
  //  r=sqrt(-2.*log(1.-RNDM(dummy)));
  //  u1=r*sin(RNDM(dummy)*6.28318595886);

  
  return rnormx()*calibration.pedestal_sigma[mode]+calibration.pedestal[mode];
}


geant AMSRichMCHit::noise(int channel,integer mode){ // ADC counts above the pedestal
  AMSgObj::BookTimer.start("SIRINoise");
  assert(mode==0 || mode==1);  
  geant u1,u2,dummy,r;
  
  RichPMTChannel current(channel);

  //  RichPMTsManager::GetPMT(current.pmt_geom_id).compute_tables(false);  // Make sure that the gain is the correct one

  int loops=0;
  do{
    float limit=int(current.pedestal_threshold[mode]*current.pedestal_sigma[mode])+1;
    r=sqrt(limit*limit-2.*log(1.-RNDM(dummy)));
    geant par=RNDM(dummy)*6.28318595886;
    u1=r*sin(par);
    if(u1<current.pedestal_threshold[mode]*current.pedestal_sigma[mode])
      u1=r*cos(par);
    loops++;

#ifdef __AMSDEBUG__
    if(loops>10){
      cout <<"Looping..."<<loops<<endl;
    }
#endif


    if(loops>100){
      cout<<"AMSRichMCHit::noise--too many loops"<<endl; 
      AMSgObj::BookTimer.stop("SIRINoise");
      return current.pedestal_threshold[mode]*current.pedestal_sigma[mode]+current.pedestal[mode]+1.;
    }
  }while(integer(u1+current.pedestal[mode])<integer(current.pedestal_threshold[mode]*current.pedestal_sigma[mode]+current.pedestal[mode]));

  
  //  do u1=current.getped(mode)+current.getsped(mode)*rnormx(); 
  //  while(integer(u1)<=integer(current.getthreshold(1)*current.getsped(mode)+current.getped(mode)));
  //  return u1;

  //  return u1*current.getsped(mode)+current.getped(mode);

#ifdef __AMSDEBUG__
  //cout <<"Returning "<<u1<<" limit at "<<current.getsped(mode)*current.getthreshold(mode) <<endl;
#endif

  AMSgObj::BookTimer.stop("SIRINoise");

  return u1+current.pedestal[mode];
}



void AMSRichMCHit::_writeEl(){

//  RICMCNtuple* cluster=AMSJob::gethead()->getntuple()->Get_richmc();
//  if(cluster->NMC>=MAXRICMC) return; 

  if(_status==Status_Fake) return; // Fake hit
// Here we need a flag with the IOPA to write it or not
#ifdef __WRITEROOT__
     int numgen = 0;
//     if(cluster->NMC==0) numgen = RICHDB::nphgen;
     numgen = RICHDB::nphgen;
      AMSJob::gethead()->getntuple()->Get_evroot02()->AddAMSObject(this,numgen);
#endif
/*
  if(cluster->NMC==0)
    cluster->numgen=RICHDB::nphgen;
  cluster->id[cluster->NMC]=_id;
  cluster->origin[cluster->NMC][0]=_origin[0];
  cluster->origin[cluster->NMC][1]=_origin[1];
  cluster->origin[cluster->NMC][2]=_origin[2];
  cluster->direction[cluster->NMC][0]=_direction[0];
  cluster->direction[cluster->NMC][1]=_direction[1];
  cluster->direction[cluster->NMC][2]=_direction[2];
  cluster->status[cluster->NMC]=_status;
  cluster->eventpointer[cluster->NMC]=_hit;

  cluster->NMC++;

*/

}

//--------END--------------


void AMSTOFMCCluster::_writeEl(){

  if(AMSTOFMCCluster::Out( IOPA.WriteAll%10==1)){
#ifdef __WRITEROOT__
      AMSJob::gethead()->getntuple()->Get_evroot02()->AddAMSObject(this);
#endif
/*
  TOFMCClusterNtuple* TOFMCClusterN = AMSJob::gethead()->getntuple()->Get_tofmc();

  if (TOFMCClusterN->Ntofmc>=MAXTOFMC) return;
  
// Fill the ntuple
    TOFMCClusterN->Idsoft[TOFMCClusterN->Ntofmc]=idsoft;
    for(int i=0;i<3;i++)TOFMCClusterN->Coo[TOFMCClusterN->Ntofmc][i]=xcoo[i];
    TOFMCClusterN->TOF[TOFMCClusterN->Ntofmc]=tof;
    TOFMCClusterN->Edep[TOFMCClusterN->Ntofmc]=edep;
    TOFMCClusterN->Ntofmc++;
*/
  }
}

void AMSTOFMCPmtHit::_writeEl(){
  if(AMSTOFMCPmtHit::Out( IOPA.WriteAll%10==1)){
#ifdef __WRITEROOT__
//       cout<<"write tof"<<endl; online mode write AMSRoot
     if((MISCFFKEY.G4On&&G4FFKEY.TFNewGeant4==1)||(TFMCFFKEY.writeall==1)){
       AMSJob::gethead()->getntuple()->Get_evroot02()->AddAMSObject(this);
     } 
#endif
   }
}


void AMSAntiMCCluster::_writeEl(){


  if(AMSAntiMCCluster::Out( IOPA.WriteAll%10==1)){
#ifdef __WRITEROOT__
    AMSJob::gethead()->getntuple()->Get_evroot02()->AddAMSObject(this);
#endif
// fill the ntuple
/*
  ANTIMCClusterNtuple* AntiMCClusterN = AMSJob::gethead()->getntuple()->Get_antimc();
    if (AntiMCClusterN->Nantimc>=MAXANTIMC) return;
    AntiMCClusterN->Idsoft[AntiMCClusterN->Nantimc]=_idsoft;
    for(int i=0;i<3;i++)AntiMCClusterN->Coo[AntiMCClusterN->Nantimc][i]=_xcoo[i];
    AntiMCClusterN->TOF[AntiMCClusterN->Nantimc]=_tof;
    AntiMCClusterN->Edep[AntiMCClusterN->Nantimc]=_edep;
    AntiMCClusterN->Nantimc++;
*/
  }
}





integer AMSTOFMCCluster::Out(integer status){
static integer init=0;
static integer WriteAll=0;
if(init == 0){
 init=1;
 integer ntrig=AMSJob::gethead()->gettriggerN();
 for(int n=0;n<ntrig;n++){
   if(strcmp("AMSTOFMCCluster",AMSJob::gethead()->gettriggerC(n))==0){
     WriteAll=1;
     break;
   }
 }
}
return (WriteAll || status);
}


integer AMSTOFMCPmtHit::Out(integer status){
  static integer init=0;
  static integer WriteAll=0;
  if(init == 0){
    init=1;
    integer ntrig=AMSJob::gethead()->gettriggerN();
    for(int n=0;n<ntrig;n++){
      if(strcmp("AMSTOFMCPmtHit",AMSJob::gethead()->gettriggerC(n))==0){
       WriteAll=1;
       break;
     }
    }
  }
  return (WriteAll || status);
}



integer AMSEcalMCHit::Out(integer status){
static integer init=0;
static integer WriteAll=0;
if(init == 0){
 init=1;
 integer ntrig=AMSJob::gethead()->gettriggerN();
 for(int n=0;n<ntrig;n++){
   if(strcmp("AMSEcalMCHit",AMSJob::gethead()->gettriggerC(n))==0){
     WriteAll=1;
     break;
   }
 }
}
return (WriteAll || status);
}




integer AMSAntiMCCluster::Out(integer status){
static integer init=0;
static integer WriteAll=0;
if(init == 0){
 init=1;
 integer ntrig=AMSJob::gethead()->gettriggerN();
 for(int n=0;n<ntrig;n++){
   if(strcmp("AMSAntiMCCluster",AMSJob::gethead()->gettriggerC(n))==0){
     WriteAll=1;
     break;
   }
 }
}
return (WriteAll || status);
}





