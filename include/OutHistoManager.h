#ifndef OUTHISTOMANAGER_HH
#define OUTHISTOMANAGER_HH

#include <iostream>
#include <string>
#include <TClass.h>
#include <TFile.h>
#include <TGraph.h>
#include <TH1D.h>
#include <TH2D.h>



using namespace std;

/** Simple outputHisto manager TH2F, TH1F
  * 3.07.2012-  VZ 
  */

class OutHistoManager{
public:
  OutHistoManager();
 ~OutHistoManager();
 
int AddH1(TH1D* h);
int AddH2(TH2D* h);
int AddH1(string sname, string stitle, int nb, double xmin, double xmax);
int AddH2(string sname, string stitle, int nb1, double xmin1, double xmax1, int nb2, double xmin2,double xmax2);
int FillH1(string name, double x);
int FillH2(string name, double x, double y);
int WriteAll(string sfile, string dirname="none");
int WriteAll(TFile* ff, string dirname="none");
void ClearAll();


TH1D* GetH1byName(string aname);
TH2D* GetH2byName(string aname);

vector<TH1D*> GetvH1() {return thevh1;}
vector<TH2D*> GetvH2() {return thevh2;}

  private:
vector<TH1D*> thevh1;
vector<TH2D*> thevh2;


int Debug;
};

#endif // OUTHISTOMANAGER_HH
