

#include <TParticle.h>
#include <TClonesArray.h>
#include <TError.h>
#include <Riostream.h>

#include "amsvmc_MCStack.h"

//**************include file to use rich pmt cut*********
#include "typedefs.h"
#include "cern.h"
#include "mceventg.h"
#include "amsgobj.h"
#include "commons.h"
#include <math.h>
#include "extC.h"
//#include <trigger3.h>
#include "job.h"
#include <sys/types.h>
#include <sys/time.h>
#include <sys/times.h>
#include <unistd.h>
#include <time.h>
#include "gvolume.h"
#include "gmat.h"
#include "event.h"
#include "mccluster.h"
#include "cont.h"
#include "trrec.h"
#include "daqevt.h"
#include "richdbc.h"
#include "richid.h"
//***********************************************



/// \cond CLASSIMP
ClassImp(amsvmc_MCStack)
/// \endcond

//_____________________________________________________________________________
amsvmc_MCStack::amsvmc_MCStack(Int_t size)
  : fParticles(0),
    fCurrentTrack(-1),
    fNPrimary(0)
{
/// Standard constructor
/// \param size  The stack size

  fParticles = new TClonesArray("TParticle", size);
}

//_____________________________________________________________________________
amsvmc_MCStack::amsvmc_MCStack()
  : fParticles(0),
    fCurrentTrack(-1),
    fNPrimary(0)
{
/// Default constructor
}

//_____________________________________________________________________________
amsvmc_MCStack::~amsvmc_MCStack() 
{
/// Destructor

  if (fParticles) fParticles->Delete();
  delete fParticles;
}

// private methods

// public methods

//_____________________________________________________________________________
void  amsvmc_MCStack::PushTrack(Int_t toBeDone, Int_t parent, Int_t pdg,
  	                 Double_t px, Double_t py, Double_t pz, Double_t e,
  		         Double_t vx, Double_t vy, Double_t vz, Double_t tof,
		         Double_t polx, Double_t poly, Double_t polz,
		         TMCProcess mech, Int_t& ntr, Double_t weight,
		         Int_t is) 
{


  //  cout<<"DEBUG in amsvmc_MCStack::PushTrack, about to call"<<endl;


/// Create a new particle and push into stack;
/// adds it to the particles array (fParticles) and if not done to the 
/// stack (fStack).
/// Use TParticle::fMother[1] to store Track ID. 
/// \param toBeDone  1 if particles should go to tracking, 0 otherwise
/// \param parent    number of the parent track, -1 if track is primary
/// \param pdg       PDG encoding
/// \param px        particle momentum - x component [GeV/c]
/// \param py        particle momentum - y component [GeV/c]
/// \param pz        particle momentum - z component [GeV/c]
/// \param e         total energy [GeV]
/// \param vx        position - x component [cm]
/// \param vy        position - y component  [cm]
/// \param vz        position - z component  [cm]
/// \param tof       time of flight [s]
/// \param polx      polarization - x component
/// \param poly      polarization - y component
/// \param polz      polarization - z component
/// \param mech      creator process VMC code
/// \param ntr       track number (is filled by the stack
/// \param weight    particle weight
/// \param is        generation status code


  const Int_t kFirstDaughter=-1;
  const Int_t kLastDaughter=-1;
  
  TClonesArray& particlesRef = *fParticles;
  Int_t trackId = GetNtrack();
  TParticle* particle
    = new(particlesRef[trackId]) 
      TParticle(pdg, is, parent, trackId, kFirstDaughter, kLastDaughter,
		px, py, pz, e, vx, vy, vz, tof);
   
  particle->SetPolarisation(polx, poly, polz);
  particle->SetWeight(weight);
  particle->SetUniqueID(mech);

  if (parent<0) fNPrimary++;  
    
  if (toBeDone && pdg!=50000050)
    { 
      //            cout<<"DEBUG: a track is pushed into fStack, "<<pdg<<", stack number increased to"<<GetNtrack()<<endl;
      //      printf("momemtum: px= %f, py= %f,pz= %f; position:x=%f, y=%f,z=%f;  Total momemtum: %f \n",px,py,pz,vx,vy,vz,particle->P());
      fStack.push(particle);  
    } 

  if (toBeDone && pdg==50000050 )
    { 
      float p=particle->P();
      //pow(px*px+py*py+pz*pz,0.5);
      bool  vmc_richpmtcut=RICHDB::detcer(p);
      //            cout<<"vmc_richpmtcut"<<vmc_richpmtcut<<endl;
      if(vmc_richpmtcut){
	//				cout<<"DEBUG: a Cerenkov photon is produced, "<<pdg<<", P ="<<p<<endl;
	//	cout<<"DEBUG: a Cerenkov photon is pushed into fStack, "<<pdg<<", stack number increased to"<<GetNtrack()<<endl;
	//	printf("momemtum: px= %f, py= %f,pz= %f; position:x=%f, y=%f,z=%f;  tof: %f \n",px*10000,py*10000,pz*10000,vx,vy,vz,tof);
	fStack.push(particle);  
      } 
    }

  ntr = GetNtrack() - 1;   

//   cout<<"DEBUG: in PushTracks:"<<endl;
//   cout<<"parent = "<<parent<<endl;
//   cout<<"trackID:"<<trackId<<endl;
//   cout<<"ntr="<<ntr<<endl;
//   cout<<"Particle is : "<< pdg<<endl;

}			 

//_____________________________________________________________________________
TParticle* amsvmc_MCStack::PopNextTrack(Int_t& itrack)
{
/// Get next particle for tracking from the stack.
/// \return       The popped particle object
/// \param track  The index of the popped track
//  cout<<"DEBUG: in PopNextTrack:"<<endl;
  itrack = -1;
  if  (fStack.empty()) return 0;
  TParticle* particle = fStack.top();
  fStack.pop();
  if (!particle) return 0;
  fCurrentTrack = particle->GetSecondMother();
  itrack = fCurrentTrack;
  //  cout << "fCurrentTrack:   " <<  fCurrentTrack << endl;
  //  fCurrentTrack = particle->GetID();
  //  itrack = fCurrentTrack;
  return particle;
}    

//_____________________________________________________________________________
TParticle* amsvmc_MCStack::PopPrimaryForTracking(Int_t i)
{
/// Return \em i -th particle in fParticles.
/// \return   The popped primary particle object
/// \param i  The index of primary particle to be popped

  if (i < 0 || i >= fNPrimary)
    Fatal("GetPrimaryForTracking", "Index out of range"); 
  
  return (TParticle*)fParticles->At(i);
}     

//_____________________________________________________________________________
void amsvmc_MCStack::Print(Option_t* /*option*/) const 
{
/// Print info for all particles.

//   cout << "amsvmc_MCStack Info  " << endl;
//   cout << "Total number of particles:   " <<  GetNtrack() << endl;
//   cout << "Number of primary particles: " <<  GetNprimary() << endl;

  for (Int_t i=0; i<GetNtrack(); i++)
    GetParticle(i)->Print();
}

//_____________________________________________________________________________
void amsvmc_MCStack::Reset()
{
/// Delete contained particles, reset particles array and stack.

  fCurrentTrack = -1;
  fNPrimary = 0;
  fParticles->Clear();

  //  cout<<"fStack Size"<<fStack.size()<<endl;



}       

//_____________________________________________________________________________
void  amsvmc_MCStack::SetCurrentTrack(Int_t track) 
{
/// Set the current track number to a given value.
/// \param  track The current track number

  fCurrentTrack = track;
}     

//_____________________________________________________________________________
Int_t  amsvmc_MCStack::GetNtrack() const 
{
/// \return  The total number of all tracks.

  return fParticles->GetEntriesFast();
}  

//_____________________________________________________________________________
Int_t  amsvmc_MCStack::GetNprimary() const 
{
/// \return  The total number of primary tracks.

  return fNPrimary;
}  

//_____________________________________________________________________________
TParticle*  amsvmc_MCStack::GetCurrentTrack() const 
{
/// \return  The current track particle

  TParticle* current = GetParticle(fCurrentTrack);

  if (!current)    
    Warning("GetCurrentTrack", "Current track not found in the stack");

  return current;
}  

//_____________________________________________________________________________
Int_t  amsvmc_MCStack::GetCurrentTrackNumber() const 
{
/// \return  The current track number

  return fCurrentTrack;
}  

//_____________________________________________________________________________
Int_t  amsvmc_MCStack::GetCurrentParentTrackNumber() const 
{
/// \return  The current track parent ID.

  TParticle* current = GetCurrentTrack();

  if (current) 
    return current->GetFirstMother();
  else 
    return -1;
}  

//_____________________________________________________________________________
TParticle*  amsvmc_MCStack::GetParticle(Int_t id) const
{
/// \return   The \em id -th particle in fParticles
/// \param id The index of the particle to be returned

  if (id < 0 || id >= fParticles->GetEntriesFast())
    Fatal("GetParticle", "Index out of range"); 
   
  return (TParticle*)fParticles->At(id);
}


