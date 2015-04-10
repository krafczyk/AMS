//Author P. Zuccon -- MIT
#ifndef HistoProofMan_h
#define HistoProofMan_h
#include "TObject.h"
#include "hashtable.h"
#include "TH1.h"
#include <string>
#include <vector>

class threp{
public:
  enum hkinds{knull,kth1f,kth2f,ktprofile,kth1d,kth2d,ktprofile2d,kth3f,kth3d};
  hkinds htype;
  std::string name;
  std::string title;
  std::string opt;
  int nbin[3];
  std::vector<double> xlim;
  std::vector<double> ylim;
  std::vector<double> zlim;
public:
  int getdimension();
  hkinds getkind(const TH1* hin);
  
public:
  threp();
  threp(const TH1* hin);
  threp(const threp& orig);
  TH1* gethisto() const;
  
  virtual ~threp(){}
  threp& operator=(const threp& orig);

  ClassDef(threp,0);

};

class HistoProofMan: public TObject{
public:
  hashtable<threp> rlist;
  hashtable<TH1*> hlist;
  char fname[200];  

public:
  HistoProofMan(const char * nam="HistoOut.root"){sprintf(fname,"%s",nam);}
  ~HistoProofMan();
  /// Add a new histogram
  void Add(TH1* hist );
  void Fill(const char * name, double a,double  b=1.,double w=1.);
  void SetOutputName(const char* ff){sprintf(fname,"%s",ff);}
  const char* GetOutputName(){return fname;}
  void Save();
  
  TH1* Get(const char * name);
  const threp* GetRep(const char* nn);  
  void AddH(TH1* hist ){hlist.Add(hist->GetName(),hist);}
  TH1* GetNew(const char * name);
  

  void CreateHistos();
  

  
  ClassDef(HistoProofMan,0);
};

#endif


