#include "TObject.h"

class EventData: public TObject{
public:
  int now[2];
  ClassDef(EventData,1)
};

class EventBeta: public TObject{
public:
  int tile;
  double beta;
  double invMass;
  double factor;
  double distance;
  double x;
  double y;
  int used;
  double npexp;
  double prob;
  double momentum;
  double width;
  ClassDef(EventBeta,1)
};

class EventFlags: public TObject{
public:
  bool enoughTrHits;
  bool richFiducial;
  bool isNaF;
  bool farFromTileBorder;
  bool enoughRichHits;
  bool enoughtExpectedRichHits;
  bool probRich;
  bool betaOne;
  bool positiveMomentum;
  bool goodGamma;
  bool ringWidth;
  bool cleanRing;
  ClassDef(EventFlags,1)
};
