
#include <TCanvas.h>
#include <TF1.h>
#include <TVector3.h>

#include <math.h>

/** Andrei Kounine's dE/dx function.
 *
 * Obtained from Stefan Schael.
 *
 * Calculates the dE/dx for a given momentum. Assumes a proton ???
 *
 * \param p proton momentum (in GeV/c)
 *
 * \todo This function is in dire need of documentation. It also needs a reference. How reliable are those hardcoded numbers.
 *
 * \returns dE/dx in which unit?
 */
Double_t fdedx(Double_t p) {

 Double_t b2, g2, bg, tm, tu, sc, dc;
 Double_t Mp=0.938272;
 Double_t Me=5.11e-4;
 Double_t Ie=5.11e-7;                           // Fit value

// p = abs(p);
 p = fabs(p); ///\todo let Andrei know that there is a bug in his function.
 if ( p<0.5 )  p = 0.5;
 if ( p>999. ) p = 999.;

 b2 = p*p/(p*p+Mp*Mp);
 g2 = (p*p+Mp*Mp)/(Mp*Mp);
 bg = sqrt(b2*g2);
 tm = 2.*Me*b2*g2/(1.+2*sqrt(g2)*Me/Mp);

 // shell correction (PDG-93-06, D.Groom)
 sc = (0.422377/b2/g2 + 0.0304043/b2/b2/g2/g2 - 0.00038106/b2/b2/b2/g2/g2/g2)*(Ie*1.0e+6)*(Ie*1.0e+6)
    + (3.858019/b2/g2 - 0.1667989/b2/b2/g2/g2 + 0.00157955/b2/b2/b2/g2/g2/g2)*(Ie*1.0e+6)*(Ie*1.0e+6)*(Ie*1.0e+6);

 // density correction (PDG-93-06, D.Groom)
 if      ( log(bg) > 10.907576 ) dc = 2.*log(bg) - 12.728;
 else if ( log(bg) > 3.5989405 ) dc = 2.*log(bg) - 12.728 + 0.023690*pow(10.907576-log(bg),2.7414);
 else                            dc = 0.;

 // cutoff energy
 if ( tm > 1.06 ) tu = 1.06;                      // Fit value
 else             tu = tm;

 return 4.42/b2*(0.5*log(2*Me*b2*g2*tu/Ie/Ie) - 0.5*b2*(1.+tu/tm) - sc/54. - dc/2.);

}


Double_t fdedx_func( Double_t* x, Double_t* ){
  return fdedx(x[0]);
}

int plot_fdedex() {

  TF1* func = new TF1("fdedx", fdedx_func, 0.1, 500., 0);

  new TCanvas;
  func->Draw("L");
  gPad->SetLogx();
  gPad->SetGridx();
  gPad->SetGridy();

  return 0;

}



