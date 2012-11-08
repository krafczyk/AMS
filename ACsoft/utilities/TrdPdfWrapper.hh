#ifndef TRDPDFWRAPPER_HH
#define TRDPDFWRAPPER_HH

class TGraph;

namespace Utilities {

class TrdPdfWrapper
{
public:
    TrdPdfWrapper( TGraph* pdfGraph );

    double TrdQtPdf(double *x, double *par);

private:

    TGraph* fPdfGraph;

};

}

#endif // TRDPDFWRAPPER_HH
