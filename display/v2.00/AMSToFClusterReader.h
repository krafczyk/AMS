//  $Id: AMSToFClusterReader.h,v 1.2 2001/01/22 17:32:40 choutko Exp $
#ifndef AMSToFClusterReader_H
#define AMSToFClusterReader_H

//////////////////////////////////////////////////////////////////////////
//                                                                      //
// AMS Time of Flight Cluster Reader                                    //
//                                                                      //
//////////////////////////////////////////////////////////////////////////

#ifndef ROOT_TObject
#include <TObject.h>
#endif
#ifndef ROOT_TTree
#include <TTree.h>
#endif

#include "AMSMaker.h"


class AMSToFClusterReader : public AMSMaker {

private:

   Int_t          m_Nclusters;

//        histograms
//   TH1F          *m_Multiplicity;


  public:
                  AMSToFClusterReader() {;}
                  AMSToFClusterReader(const char *name, const char *title);
   virtual       ~AMSToFClusterReader() {;}
   virtual void   Init(TTree * tree=0);
   virtual void   Finish();

   virtual void   Make();
   virtual void   PrintInfo();
// virtual Bool_t Enabled(TObject * obj);

//
//      Getters
//

//
//      Setters
//
   void           AddCluster(Int_t status, Int_t plane, Int_t bar,
                             Float_t energy, Float_t time, Float_t ertime,
                             Float_t * coo, Float_t * ercoo,
                             Int_t ncells=0, Int_t npart=0,
                             Int_t ntracks=0, TObjArray * tracks=0);
   void           RemoveCluster(const Int_t cluster);
   void           Clear(Option_t *option="");

   ClassDef(AMSToFClusterReader, 1)   // AMS Time of Flight Cluster Maker
};

#endif



