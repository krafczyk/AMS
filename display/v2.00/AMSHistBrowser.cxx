
//////////////////////////////////////////////////////////////////////////
//                                                                      //
// AMSHistBrowser                                                      //
//                                                                      //
// helper class to browse AMSRoot Makers histograms.                    //
//                                                                      //
//////////////////////////////////////////////////////////////////////////

#include <TBrowser.h>
#include "AMSRoot.h"
#include "AMSMaker.h"
#include "AMSHistBrowser.h"

ClassImp(AMSHistBrowser)



//_____________________________________________________________________________
AMSHistBrowser::AMSHistBrowser() 
                : TNamed("Histograms","AMSRoot Histograms browser")
{

}

//_____________________________________________________________________________
void AMSHistBrowser::Browse(TBrowser *b)
{

//printf("Entering Browse, gATLFast=%x\n",gATLFast);
//printf("gATLFast->Makers()=%x\n",gATLFast->Makers());

  TIter next(gAMSRoot->Makers());
//printf("after TIter definition\n");
  AMSMaker *maker;
  while (maker = (AMSMaker*)next()) {
//printf("in loop makers\n");
//printf("b=%x, maker=%x, maker->Histograms()=%x, name=%s\n",b, maker, maker->Histograms(),maker->GetName());
     b->Add(maker->Histograms(),maker->GetName());
   }

}
