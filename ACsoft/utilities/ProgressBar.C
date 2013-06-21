#include "ProgressBar.hh"

#include <iostream>
#include <iomanip>

#define DEBUG 0
#include <debugging.hh>

ACsoft::Utilities::ProgressBar::ProgressBar( int numberOfTotalSteps, char progressIndicator ) :
  fProgressIndicator(progressIndicator),
  fNumberOfDivisions(100), // must be even!
  fOffsetToLeftMargin(10),
  fNumberOfTotalSteps(numberOfTotalSteps),
  fCurrentStep(-1),
  fCurrentDivision(-1)
{
  fStepsPerDivision = fNumberOfTotalSteps / fNumberOfDivisions;
}

void ACsoft::Utilities::ProgressBar::Update( int currentStep ) {

  if( currentStep > fCurrentStep )
    fCurrentStep = currentStep;

  int division = float(currentStep) / float(fNumberOfTotalSteps) * fNumberOfDivisions;
  if( division > fCurrentDivision ){

    fCurrentDivision = division;
    DEBUG_OUT << "Updating at step " << currentStep << std::endl;
    std::cout << fProgressIndicator << std::flush;
  }

  if( currentStep == fNumberOfTotalSteps-1)
  std::cout << std::endl;
}

void ACsoft::Utilities::ProgressBar::PrintScale() const {

  for( int i=0 ; i<fOffsetToLeftMargin ; ++i )
    std::cout << " ";

  int gap1 = fNumberOfDivisions/2 - 2;
  int gap2 = fNumberOfDivisions/2 - 4;
  std::cout << "0" << std::setw(gap1) << std::setfill('-') << ""  << "50" << std::setw(gap2) << "" << std::setfill(' ') << "100" << std::endl;

  for( int i=0 ; i<fOffsetToLeftMargin ; ++i )
    std::cout << " ";
  std::cout << std::flush;

}
