#include "ManualNMinusOneCut.hh"

#include <assert.h>
#include <cmath>

#define DEBUG 0
#define INFO_OUT_TAG "ManualNMinusOneCut> "
#include <debugging.hh>

Cuts::ManualNMinusOneCut::ManualNMinusOneCut( std::string description ) :
  NMinusOneCut::NMinusOneCut(description),
  fConditionFulfilled(false),
  fConditionTested(false)
{
}

bool Cuts::ManualNMinusOneCut::Examine(const ACsoft::Analysis::Particle& particle, float value,  bool condition) {

  fLastCutValue = value;
  fLastRigidityOrEnergy = NMinusOneValue(particle, fNMinusOneMode);

  fConditionFulfilled = condition;
  fConditionTested = true;
  return fConditionFulfilled;
}

bool Cuts::ManualNMinusOneCut::TestCondition( const ACsoft::Analysis::Particle& ) {

  if(!fConditionTested) {
    WARN_OUT << "You have forgotten the call to the Examine() function or called Passes() on this particle twice!";
    assert(fConditionTested);
  }

  fConditionTested = false; // use only once...
  return fConditionFulfilled;
}

