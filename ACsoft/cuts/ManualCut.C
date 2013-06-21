#include "ManualCut.hh"

#include <assert.h>
#include <cmath>

#define DEBUG 0
#define INFO_OUT_TAG "ManualCut> "
#include <debugging.hh>

Cuts::ManualCut::ManualCut( std::string description ) :
  Cuts::Cut(description),
  fConditionFulfilled(false),
  fConditionTested(false)
{
}

/** Define the condition to be tested.
  *
  * \param[in] condition the condition to be tested by the ManualCut
  */
bool Cuts::ManualCut::Examine( bool condition ) {

  fConditionFulfilled = condition;
  fConditionTested = true;
  return fConditionFulfilled;
}

bool Cuts::ManualCut::TestCondition( const ACsoft::Analysis::Particle& ) {

  if(!fConditionTested){
    WARN_OUT << "You have forgotten the call to the Examine() function or called Passes() on this particle twice!";
    assert(fConditionTested);
  }

  fConditionTested = false; // use only once...
  return fConditionFulfilled;
}

