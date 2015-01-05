#ifndef __TrPdf__
#define __TrPdf__

/*! 
 \file  TrPdf.h
 \brief Header file of TrPdf class

 \class TrPdf
 \brief The tracker PDF class 
 
 $Date$

 $Revision$
*/

#include "TGraph.h"
#include "TGraphErrors.h"
#include "TF1.h"
#include "TCanvas.h"
#include "TH1.h"
#include "TFile.h"
#include "TRandom.h"
#include "TString.h"

#include "typedefs.h"
#include <vector>

#define VERBOSE 0


class TrPdf : public TObject {
	
 protected:

  //////////////////////////
  /// Members  
  //////////////////////////
	
  /// TrPdf Name
  TString        Name;     
  /// Number of points
  int            N;        
  /// X values
  vector<double> X;        
  /// Y values
  vector<double> Y;        

  //////////////////////////
  /// Service members (integral, fits, ...) 
  //////////////////////////
	
  /// Integral values
  vector<double> IY; //!        
  /// Display
  TGraph*  Graph; //! 
  /// Log-Log order 5 polynomial fit 
  TF1* LogLog; //!
  /// "Spectral index" fit 
  TF1* SpeInd; //!
	
 public:

  //////////////////////////
  /// Accessors
  //////////////////////////	

  /// Default constructor
  TrPdf() { Init(); }
  /// Name constructor 
  TrPdf(char* name) { Init(); Name = name; }
  /// Name constructor 
  TrPdf(char* name, char* filename);
  /// Constructor with TH1
  TrPdf(char* name, TH1* histo, bool divide, bool normalize);
  /// Constructor with TF1
  TrPdf(char* name, TF1* fun, int nbins, double xlow, double xhig, bool normalize);
  //! Copy constructor
  TrPdf(const TrPdf& that) { Copy(that); }
  //! Assignment operator
  TrPdf& operator=(const TrPdf& that);
  //! Destructor
  virtual ~TrPdf() { Clear(); }
  //! Initializer
  void    Init();
  //! Clear 
  void    Clear(Option_t* option="");
  //! Copy  
  void    Copy(const TrPdf& that) ;
  /// Print informations 
  void    Info(int verbosity = 1);

  /// TrPdf Name
  TString GetName()    { return Name; }
  /// Number of points
  int     GetN()       { return N; }
  /// X value of the i-th point
  double  GetX(int i)  { return (i<GetN()&&i>=0) ? X.at(i) : 0; }
  /// Y value of the i-th point
  double  GetY(int i)  { return (i<GetN()&&i>=0) ? Y.at(i) : 0; }
  /// Set TrPdf name
  void    SetName(char* name) { Name = name; }
  /// Set the X value of the i-th point 
  void    SetX(int i, double x) { if (i<GetN()) X.at(i) = x; }
  /// Set the Y value of the i-th point
  void    SetY(int i, double y) { if (i<GetN()) Y.at(i) = y; }
  /// Add point (insert with sorting)
  void    AddPoint(double x, double y);

  //////////////////////////
  /// Math  
  //////////////////////////
 
  /// Normalize the pdf (area = 1)
  void    Normalize(); 
  /// Scale Y values and errors by a number
  void    ScaleY(double scale);
  /// Evaluate function (linear or logarithmic interpolation)
  double  Eval(double x, bool logx = false, bool logy = false);
  /// Multiply by another function (evaluated in the function points)
  void    Multiply(TrPdf* that, bool logx = false, bool logy = false);
  /// Divide by another function (evaluated in the function points)
  void    Divide(TrPdf* that, bool logx = false, bool logy = false);
  /// Sum another function (evaluated in the function points)
  void    Sum(TrPdf* that, bool logx = false, bool logy = false);
  /// Subtract another function (evaluated in the function points)
  void    Subtract(TrPdf* that, bool logx = false, bool logy = false); 
  /// From function to function*x^{index}
  void    MultiplyPowerLaw(double index);
 
  /// Integral 
  double  Integral() { return Integral(0,GetN()-1); }   
  /// Integral from one value to another
  double  Integral(int first, int last);
  /// Integral from xlow to xhig
  double  Integral(double xlow, double xhig);
  /// Create the integral function, first point is zero.  
  void    CreateIntegral(int force = 0);
  /// Get the integral up to point 
  double  GetCumulative(int i) { return (i<GetN()) ? IY.at(i) : 0.; }
  /// Get the normalized integral point
  double  GetNormCumulative(int i) { return GetCumulative(i)/GetCumulative(N-1); }
  /// Invert normalized cumulative (needed by random distribution extraction) 
  double  GetInvNormCumulative(double y); 

  //////////////////////////
  /// Reading 
  //////////////////////////

  /// Read a text file (two columns: X and Y, is normalized?)
  void    ReadTextFile(char* filename, bool normalize = false);
  /// Read a TF1 (make nbins equally spaced bins from xlow and xhig)
  void    ReadTF1(TF1* fun, int nbins, double xlow, double xhig, bool normalize = false);
  /// Read a TH1 (have you divided for the bin width? is it normalized?) 
  void    ReadTH1(TH1* histo, bool divide = false, bool normalize = false);

  //////////////////////////
  /// Fitting & Drawing 
  //////////////////////////

  /// Make the log-log polynomial fit
  void    FitLogLog(double min, double max);
  /// Retrieve the TF1 log-log polynomial fit
  TF1*    GetLogLog() { return LogLog; }
  /// Make the spectral index fit
  void    FitSpeInd(double min, double max);
  /// Retrieve the TF1 spectral index fit
  TF1*    GetSpeInd() { return SpeInd; }
  /// (Re-)create graph
  TGraph* CreateGraph();  
  /// Get graph (force construction by default)
  TGraph* GetGraph(int force = 1);
  /// Clear graph
  void    ClearGraph() { if (Graph!=0) Graph->Delete(); Graph = 0; }
  /// Draw the current function
  void    Draw(Option_t* option = "") { GetGraph()->Draw(option); }

  using TObject::Copy;
  using TObject::GetName;
  using TObject::Info;

  /// ROOT definition
  ClassDef(TrPdf,1);
};

#endif
