#ifndef CUTOFFCALCULATOR_HH
#define CUTOFFCALCULATOR_HH


namespace Analysis {

class Particle;

/** Access to geomagnetic cutoff value.
  *
  * // FIXME add documentation
  *
  */
class CutoffCalculator
{

public:
  CutoffCalculator();
  ~CutoffCalculator();

  static float CalculateCutoff( const Particle& particle );

};

}

#endif // CUTOFFCALCULATOR_HH
