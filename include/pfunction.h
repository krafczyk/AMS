#ifndef pfunction_h
#define pfunction_h

#include "TF1.h"

class MTF1: public TF1{
public:
  MTF1():TF1(){}
  MTF1(const char *name, const char *formula, Double_t xmin=0, Double_t xmax=1)
    :TF1(name, formula, xmin,xmax){}
  MTF1(const char *name, Double_t xmin, Double_t xmax, Int_t npar)
    :TF1(name,xmin, xmax,npar){}
  MTF1(const char *name,  double (*fcn) (double *,double*) , Double_t xmin, Double_t xmax, Int_t npar)
    :TF1( name, fcn,  xmin,  xmax, npar){}
  ~MTF1(){}
  virtual Double_t GetRandom();
  virtual Double_t GetRandom(Double_t xmin, Double_t xmax);
  ClassDef(MTF1,0);
};


double ProtonSpectrum(double *rin, double *p);

#endif
