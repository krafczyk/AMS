// method source file for the object AMSEvent
// May 06, 1996. ak. First try with Objectivity 
//
// last edit Apr 29, 1997. ak.

#include <iostream.h>
#include <string.h>
#include <cern.h>

#include <typedefs.h>
#include <trrechit_ref.h>
#include <tcluster_ref.h>
#include <ttrack_ref.h>
#include <tofrecD_ref.h>
#include <tbeta_ref.h>
#include <chargeD_ref.h>
#include <particleD_ref.h>
#include <recevent.h>


AMSeventD::AMSeventD (integer run, uinteger eventNumber, time_t time)
{
  setrun(run);
  setevent(eventNumber);
  settime(time);
  clearCounts();
}

void AMSeventD::clearCounts() 
{
  _nTrHits      = 0;
  _nTrClusters  = 0;      
  _nTOFClusters = 0;     
  _nCTCClusters = 0;     
  _nTracks      = 0;          
  _nBetas       = 0;           
  _nCharges     = 0;         
  _nParticles   = 0;       
}

void AMSeventD::readEvent(integer& run, integer& event, time_t& time)
{
  run         = Run();
  event       = Event();
  time        = Time();
}

void AMSeventD::addParticle(int npart, ParticleS* particles)
{
  particleS.resize(npart);
  for (int i=0; i<npart; i++) {
   particleS.set(i,particles[i]);
  }
}
