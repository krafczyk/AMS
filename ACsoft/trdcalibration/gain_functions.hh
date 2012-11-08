#ifndef GAIN_FUNCTIONS_HH
#define GAIN_FUNCTIONS_HH

#include <TObject.h>
#include <TVector3.h>

Double_t fdedx(Double_t p);
Double_t fdedx_func( Double_t* x, Double_t* );

int plot_fdedex();

#endif // GAIN_FUNCTIONS_HH
