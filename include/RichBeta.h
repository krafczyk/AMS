#ifndef _RICHBETA_
#define _RICHBETA_
#include "TString.h"
#include "RichTools.h"

class RichRingR;

class RichBetaUniformityCorrection{
private:
  RichBetaUniformityCorrection(){_agl=_naf=0;_latest[0]=_latest[1]=_latest[2]=_latest[3];};
  ~RichBetaUniformityCorrection(){if(_agl) delete _agl;if(_naf) delete _naf;};
public:
  static RichBetaUniformityCorrection* _head;
  static RichBetaUniformityCorrection *getHead(){return _head;}
  static bool Init(TString="");

  GeomHashEnsemble *_agl;
  GeomHashEnsemble *_naf;

  float _latest[4];

  // Accessors to the correction
  float getCorrection(RichRingR *ring);
  float getCorrection(float *); 
  float getCorrection(float x,float y,float vx,float vy);
  // Accessors to several quality parameters
  float getWidth(RichRingR *ring);
  float getWidth(float *); 
  float getWidth(float x,float y,float vx,float vy);
  // Accessors to several quality parameters
  float getRms(RichRingR *ring);
  float getRms(float *); 
  float getRms(float x,float y,float vx,float vy);
};



#endif
