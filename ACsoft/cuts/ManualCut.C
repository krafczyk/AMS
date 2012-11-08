#include "ManualCut.hh"

Cuts::ManualCut::ManualCut( std::string description ) :
  Cuts::Cut(description) {
}

void Cuts::ManualCut::LetPass( const Analysis::Particle&  ) {

  ++fTotalCounter;
  ++fPassedCounter;
}

void Cuts::ManualCut::LetFail( const Analysis::Particle&  ) {

  ++fTotalCounter;
  ++fFailedCounter;
}
