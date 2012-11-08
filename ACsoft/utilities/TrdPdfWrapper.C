#include "TrdPdfWrapper.hh"

#include <TGraph.h>

#include <math.h>

Utilities::TrdPdfWrapper::TrdPdfWrapper( TGraph* pdfGraph ) :
    fPdfGraph(pdfGraph)
{
}


//**********************************************************************************************************************************
//	x[0] = Eadc
//
double Utilities::TrdPdfWrapper::TrdQtPdf(double *x, double *par) {

    double 	dEdX 	   = x[0];
    double  TrdMinDeDx = par[0];
    double  TrdMaxDeDx = par[1];

    double pdf=0.0;
    if (dEdX>TrdMinDeDx+0.5  && dEdX<TrdMaxDeDx-0.5) pdf = fPdfGraph->Eval(dEdX);
    return 	std::max(1E-12,pdf);

}
