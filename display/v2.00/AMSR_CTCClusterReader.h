//  $Id: AMSR_CTCClusterReader.h,v 1.3 2001/01/22 17:32:38 choutko Exp $
#ifndef AMSR_CTCClusterReader_H
#define AMSR_CTCClusterReader_H

//////////////////////////////////////////////////////////////////////////
//                                                                      //
// AMS Cerenkov Threshold Counter Cluster Reader                        //
//                                                                      //
//////////////////////////////////////////////////////////////////////////

#ifndef ROOT_TObject
#include <TObject.h>
#endif

#include "AMSR_Maker.h"


class AMSR_CTCClusterReader : public AMSR_Maker {

private:

   Int_t          m_Nclusters;

//        histograms
//   TH1F          *m_Multiplicity;


  public:
                  AMSR_CTCClusterReader() {;}
                  AMSR_CTCClusterReader(const char *name, const char *title);
   virtual       ~AMSR_CTCClusterReader() {;}
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
   void           AddCluster(Int_t status, Int_t plane, 
                             Float_t rawsignal, Float_t signal, Float_t ersig,
                             Float_t * coo, Float_t * ercoo,
                             Int_t ntracks=0, TObjArray * tracks=0);
   void           RemoveCluster(const Int_t cluster);
   void           Clear(Option_t *option="");

   ClassDef(AMSR_CTCClusterReader, 1)   // AMS Time of Flight Cluster Maker
};

#endif



