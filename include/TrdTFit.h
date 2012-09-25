//  $Id: TrdTFit.h,v 1.2 2012/09/25 08:00:30 choutko Exp $
#ifndef _TRDTFIT_
#define _TRDTFIT_
#include <iostream>
//root
#include <TStopwatch.h>
#include <point.h>
#include <cmath>
#include <vector>
#include <map>

class AMSEventR;

using namespace std;

//namespace TRDTFIT{
class Tfunc{ 
 protected:
  double t0_, px_, px2_, py_, py2_, pz_, pz2_,lik_;
  
  vector<double> residuals;
 public:
  Tfunc(double t0, double px, double px2, double py, double py2, double pz, double pz2, double lik)
    : t0_(t0), px_(px), px2_(px2), py_(py), py2_(py2_), pz_(pz), pz2_(pz2), lik_(lik) {};
    
    
    Tfunc(double* x)
      : t0_(x[0]), px_(x[1]), px2_(x[2]), py_(x[3]), py2_(x[4]), pz_(x[5]), pz2_(x[6]), lik_(x[7]) {};
      
      Tfunc()
	: t0_(-1.e6), px_(-1.e6), px2_(-1.e6), py_(-1.e6), py2_(-1.e6), pz_(-1.e6), pz2_(-1.e6), lik_(-1.e6){};
	
	~Tfunc(){residuals.clear();};
	
	double getT0(){return t0_;}

	double getPx(){return px_;}

	double getPy(){return py_;}

	double getPz(){return pz_;}

	double getPx2(){return px2_;}

	double getPy2(){return py2_;}

	double getPz2(){return pz2_;}

	double getLik(){return lik_;}

	void setT0(double value){t0_=value;}

	void setPx(double value){px_=value;}

	void setPy(double value){py_=value;}

	void setPz(double value){pz_=value;}

	void setPx2(double value){px2_=value;}

	void setPy2(double value){py2_=value;}

	void setPz2(double value){pz2_=value;}

	void setLik(double value){lik_=value;}

	void modify(double *x){
	  t0_ = x[0];
	  px_ = x[1];
	  px2_= x[2];
	  py_ = x[3];
	  py2_= x[4];
	  pz_ = x[5];
	  pz2_= x[6];
	}

	void Info(){
	  cout << "global T-Fit parameters (T0, px, px2, py, py2, pz, pz2, lik): " <<  getT0() << "," << getPx() << "," << getPx2() << "," << getPy() << "," << getPy2() << "," << getPz() << "," << getPz2() << "," << getLik() << endl;
	}

	void setResiduals(vector<double> vec){
	  for(vector<double>::iterator it=vec.begin();it!=vec.end();it++){
	    residuals.push_back(*it);
	  }
	}

	float getTempAtPos(AMSPoint pos){
	  return getT0() + pos.x() * getPx() + pos.x() * pos.x() * getPx2() 
	    + pos.y() * getPy() + pos.y() * pos.y() * getPy2()
	    + pos.z() * getPz() + pos.z() * pos.z() * getPz2();
	}
  
	vector<double> getResiduals(){return residuals;}

	void clearResiduals(){return residuals.clear();}

	ClassDef(Tfunc,1);
};

typedef vector<pair<AMSPoint, float> > ownvector;

class TrdTFit
{
 private:
  double MYRANDOM(int &myidum);
  double toBeMinimised(double *x);
  
  void SA(int& N, double* X, double& RT, double& EPS, int& NS, int& NT, 
	  int& NEPS, int& MAXEVL, double* LB, double* UB, double* C, 
	  int& IPRINT, double& T, double* VM, double* XOPT, double& FOPT, 
	  int& NACC, int& NFCNEV, int& NOBDS, int& IER, double* FSTAR, 
	  double* XP, int* NACP /*, TH2F * h2DSpace*/);
  
  
  double simple_pdf(double& Tpos,double& Texp);
  
  double logLIK(Tfunc* gfunc, ownvector::iterator begin_, ownvector::iterator end_, const int& res, vector<double>& residuals, const bool& debug); 
  
  int id__,myidum,ncount,res;
  ownvector::iterator begin;
  ownvector::iterator end;
  
  map<string, AMSPoint> *sensMap;
  ownvector *SensPosAmpVec;

  float chi2;
  static TrdTFit* head;
 public:
  int debug;
  Tfunc *fitfunc;
  
  static TrdTFit* gethead(){
    if(!head)head=new TrdTFit();
    return head;
  }
  
  static void  KillPointer(){
    delete head;
    head=0;
  }
  
  int fill(const char *name,float temp);
  void makeSensorMap(char* mapnam);
  
  void showSensorMap();

  TrdTFit(){
    id__ = 0;
    myidum = -64;
    ncount = 0;
    res = 0;
    sensMap= 0;
    SensPosAmpVec=0;
    fitfunc=0;
    chi2=0;
  };
  
  
  //  template <class Tfunc>
  bool GlobalTFit(Tfunc* gfunc_, ownvector *posTvec);
  
  // read and fit TRD temperatures for given event
  int ReadAndFitTemperatures(AMSEventR *pev);
  
  int DoFit(int time);

  
  ClassDef(TrdTFit,1);
};

//}
#endif
