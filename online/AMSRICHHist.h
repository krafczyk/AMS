#include "AMSHist.h"
#include "TGraph.h"


class AMSRICHHist : public AMSHist{
private:
  int FindName(TString name);
  bool Select(AMSNtupleR *ntuple); // Perform the selection and fill masks
  bool GetCrossing(AMSNtupleR *ntuple,double &x,double &y);
  int init();

  ////////////////////////// Informatin for temporal monitorings /////////////////////
  void FillTemp(AMSNtupleR *ntuple);

  map<string,int> sets;
  int max_run;
  int min_run;
  int sets_number;
  int last_run;

 public:
  AMSRICHHist(Text_t * name, Text_t * title, Int_t active=1):AMSHist(name,title,active){init();}
  void Book();
  void ShowSet(int );
  void Fill(AMSNtupleR *ntuple);
};
