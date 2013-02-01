#ifndef PROGRESSBAR_HH
#define PROGRESSBAR_HH

namespace ACsoft {

namespace Utilities {

/** A simple ascii progress bar.
  *
  * For a process that consists of a total number of \p n steps, initialize the progress bar
  * \code
  * ProgressBar pb(n);
  * \endcode
  *
  * Before starting the process, print the scale (0% to 100%) of the progress bar:
  * \code
  * pb.PrintScale();
  * \endcode
  *
  * Then, during the process, update the progress bar with the current step number \p step:
  * \code
  * pb.Update(step);
  * \endcode
  *
  * No additional output should be printed in the program while the progress bar is used.
  *
  */
class ProgressBar
{

public:

  ProgressBar( int numberOfTotalSteps, char progressIndicator = '|' );

  void Update( int currentStep );
  void PrintScale() const;

private:

  char fProgressIndicator;
  int fNumberOfDivisions;
  int fOffsetToLeftMargin;
  int fNumberOfTotalSteps;
  int fCurrentStep;
  int fCurrentDivision;
  int fStepsPerDivision;

};
}

}

#endif // PROGRESSBAR_HH
