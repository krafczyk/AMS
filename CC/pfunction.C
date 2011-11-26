//P.Zuccon 2011
#include "pfunction.h"
#include "random.h"
#include <math.h>
#include "TMath.h"


double ProtonSpectrum(double *rin, double *p){

  double R=rin[0];
  double neut=p[0];
  double mp=0.938272;
  double Ek=sqrt(mp*mp+R*R)-mp;
  double P1=12663;
  double P2=1.51;
  double P3=0.00637;
  double P4=1.3;
  double P5=0.229;
  double P6=0.575;
  double P7=0.179;

  double Eta=exp(P5*(log(Ek/P6)-sqrt(pow(log(Ek/P6),2)+P7)));
  double phimin=Eta*P1*pow(Ek+P2*exp(-P3*pow(Ek,P4)),-2.71);

  double alpha=2.52;
  double x=R/4.97;
  double A=-7.3E-4*(neut-4311);
  double epsi=-1.1E-3*(neut-4311);

  double M1=pow(fabs(x),alpha)+pow(epsi,alpha);
  //  printf("M1 %f A %f epsi %f\n",M1,A,epsi);
  double M2=A*(x-pow(M1,1/alpha));
  //printf("M2 %f\n",M2);
  double Mod=exp(M2);

  return phimin*Mod;

}




Double_t MTF1::GetRandom(){
  // Modified by PZuccon to use AMS Random Event Generator

  // Return a random number following this function shape
  //
  //   The distribution contained in the function fname (TF1) is integrated
  //   over the channel contents.
  //   It is normalized to 1.
  //   For each bin the integral is approximated by a parabola.
  //   The parabola coefficients are stored as non persistent data members
  //   Getting one random number implies:
  //     - Generating a random number between 0 and 1 (say r1)
  //     - Look in which bin in the normalized integral r1 corresponds to
  //     - Evaluate the parabolic curve in the selected bin to find
  //       the corresponding X value.
  //   if the ratio fXmax/fXmin > fNpx the integral is tabulated in log scale in x
  //   The parabolic approximation is very good as soon as the number
  //   of bins is greater than 50.

  //  Check if integral array must be build
  if (fIntegral == 0) {
    fIntegral = new Double_t[fNpx+1];
    fAlpha    = new Double_t[fNpx+1];
    fBeta     = new Double_t[fNpx];
    fGamma    = new Double_t[fNpx];
    fIntegral[0] = 0;
    fAlpha[fNpx] = 0;
    Double_t integ;
    Int_t intNegative = 0;
    Int_t i;
    Bool_t logbin = kFALSE;
    Double_t dx;
    Double_t xmin = fXmin;
    Double_t xmax = fXmax;
    if (xmin > 0 && xmax/xmin> fNpx) {
      logbin =  kTRUE;
      fAlpha[fNpx] = 1;
      xmin = TMath::Log10(fXmin);
      xmax = TMath::Log10(fXmax);
    }
    dx = (xmax-xmin)/fNpx;
         
    Double_t *xx = new Double_t[fNpx+1];
    for (i=0;i<fNpx;i++) {
      xx[i] = xmin +i*dx;
    }
    xx[fNpx] = xmax;
    for (i=0;i<fNpx;i++) {
      if (logbin) {
	integ = Integral(TMath::Power(10,xx[i]), TMath::Power(10,xx[i+1]));
      } else {
	integ = Integral(xx[i],xx[i+1]);
      }
      if (integ < 0) {intNegative++; integ = -integ;}
      fIntegral[i+1] = fIntegral[i] + integ;
    }
    if (intNegative > 0) {
      Warning("GetRandom","function:%s has %d negative values: abs assumed",GetName(),intNegative);
    }
    if (fIntegral[fNpx] == 0) {
      delete [] xx;
      Error("GetRandom","Integral of function is zero");
      return 0;
    }
    Double_t total = fIntegral[fNpx];
    for (i=1;i<=fNpx;i++) {  // normalize integral to 1
      fIntegral[i] /= total;
    }
    //the integral r for each bin is approximated by a parabola
    //  x = alpha + beta*r +gamma*r**2
    // compute the coefficients alpha, beta, gamma for each bin
    Double_t x0,r1,r2,r3;
    for (i=0;i<fNpx;i++) {
      x0 = xx[i];
      r2 = fIntegral[i+1] - fIntegral[i];
      if (logbin) r1 = Integral(TMath::Power(10,x0),TMath::Power(10,x0+0.5*dx))/total;
      else        r1 = Integral(x0,x0+0.5*dx)/total;
      r3 = 2*r2 - 4*r1;
      if (TMath::Abs(r3) > 1e-8) fGamma[i] = r3/(dx*dx);
      else           fGamma[i] = 0;
      fBeta[i]  = r2/dx - fGamma[i]*dx;
      fAlpha[i] = x0;
      fGamma[i] *= 2;
    }
    delete [] xx;
  }

  // return random number
  //  Double_t r  = gRandom->Rndm();
  int dd=1;
  Double_t r  =RNDM(dd);
  Int_t bin  = TMath::BinarySearch(fNpx,fIntegral,r);
  Double_t rr = r - fIntegral[bin];

  Double_t yy;
  if(fGamma[bin] != 0)
    yy = (-fBeta[bin] + TMath::Sqrt(fBeta[bin]*fBeta[bin]+2*fGamma[bin]*rr))/fGamma[bin];
  else
    yy = rr/fBeta[bin];
  Double_t x = fAlpha[bin] + yy;
  if (fAlpha[fNpx] > 0) return TMath::Power(10,x);
  return x;
}


Double_t MTF1::GetRandom(Double_t xmin, Double_t xmax){
  // Modified by PZuccon to use AMS Random Event Generator

  // Return a random number following this function shape in [xmin,xmax]
  //
  //   The distribution contained in the function fname (TF1) is integrated
  //   over the channel contents.
  //   It is normalized to 1.
  //   For each bin the integral is approximated by a parabola.
  //   The parabola coefficients are stored as non persistent data members
  //   Getting one random number implies:
  //     - Generating a random number between 0 and 1 (say r1)
  //     - Look in which bin in the normalized integral r1 corresponds to
  //     - Evaluate the parabolic curve in the selected bin to find
  //       the corresponding X value.
  //   The parabolic approximation is very good as soon as the number
  //   of bins is greater than 50.
  //
  //  IMPORTANT NOTE
  //  The integral of the function is computed at fNpx points. If the function
  //  has sharp peaks, you should increase the number of points (SetNpx)
  //  such that the peak is correctly tabulated at several points.

  //  Check if integral array must be build
  if (fIntegral == 0) {
    Double_t dx = (fXmax-fXmin)/fNpx;
    fIntegral = new Double_t[fNpx+1];
    fAlpha    = new Double_t[fNpx];
    fBeta     = new Double_t[fNpx];
    fGamma    = new Double_t[fNpx];
    fIntegral[0] = 0;
    Double_t integ;
    Int_t intNegative = 0;
    Int_t i;
    for (i=0;i<fNpx;i++) {
      integ = Integral(Double_t(fXmin+i*dx), Double_t(fXmin+i*dx+dx));
      if (integ < 0) {intNegative++; integ = -integ;}
      fIntegral[i+1] = fIntegral[i] + integ;
    }
    if (intNegative > 0) {
      Warning("GetRandom","function:%s has %d negative values: abs assumed",GetName(),intNegative);
    }
    if (fIntegral[fNpx] == 0) {
      Error("GetRandom","Integral of function is zero");
      return 0;
    }
    Double_t total = fIntegral[fNpx];
    for (i=1;i<=fNpx;i++) {  // normalize integral to 1
      fIntegral[i] /= total;
    }
    //the integral r for each bin is approximated by a parabola
    //  x = alpha + beta*r +gamma*r**2
    // compute the coefficients alpha, beta, gamma for each bin
    Double_t x0,r1,r2,r3;
    for (i=0;i<fNpx;i++) {
      x0 = fXmin+i*dx;
      r2 = fIntegral[i+1] - fIntegral[i];
      r1 = Integral(x0,x0+0.5*dx)/total;
      r3 = 2*r2 - 4*r1;
      if (TMath::Abs(r3) > 1e-8) fGamma[i] = r3/(dx*dx);
      else           fGamma[i] = 0;
      fBeta[i]  = r2/dx - fGamma[i]*dx;
      fAlpha[i] = x0;
      fGamma[i] *= 2;
    }
  }

  // return random number
  Double_t dx   = (fXmax-fXmin)/fNpx;
  Int_t nbinmin = (Int_t)((xmin-fXmin)/dx);
  Int_t nbinmax = (Int_t)((xmax-fXmin)/dx)+2;
  if(nbinmax>fNpx) nbinmax=fNpx;

  Double_t pmin=fIntegral[nbinmin];
  Double_t pmax=fIntegral[nbinmax];

  Double_t r,x,xx,rr;
  do {
    int dd=1;
    //      r  = gRandom->Uniform(pmin,pmax);
    Double_t r0=RNDM(dd);
    r= pmin +(pmax-pmin)*r0;
    Int_t bin  = TMath::BinarySearch(fNpx,fIntegral,r);
    rr = r - fIntegral[bin];

    if(fGamma[bin] != 0)
      xx = (-fBeta[bin] + TMath::Sqrt(fBeta[bin]*fBeta[bin]+2*fGamma[bin]*rr))/fGamma[bin];
    else
      xx = rr/fBeta[bin];
    x = fAlpha[bin] + xx;
  } while(x<xmin || x>xmax);
  return x;
}


