//////////////////////////////////////////////////////////
//   This class has been generated by TFile::MakeProject
//     (Fri Oct 18 12:00:20 2002 by ROOT version 3.03/06)
//      from the StreamerInfo in file prv3.root
//////////////////////////////////////////////////////////


#ifndef EventRoot02_h
#define EventRoot02_h

#include "TObject.h"
#include "AMSEventHeaderRoot.h"
#include "TClonesArray.h"

class EventRoot02 : public TObject {

public:
   AMSEventHeaderRoot  Header;            //
   TClonesArray*       fBeta;             //
   TClonesArray*       fCharge;           //
   TClonesArray*       fParticle;         //
   TClonesArray*       fTOFcluster;       //
   TClonesArray*       fECALshower;       //
   TClonesArray*       fECALcluster;      //
   TClonesArray*       fECAL2Dcluster;    //
   TClonesArray*       fECALhit;          //
   TClonesArray*       fTOFMCcluster;     //
   TClonesArray*       fTrCluster;        //
   TClonesArray*       fTrMCCluster;      //
   TClonesArray*       fTRDMCCluster;     //
   TClonesArray*       fTRDrawhit;        //
   TClonesArray*       fTRDcluster;       //
   TClonesArray*       fTRDsegment;       //
   TClonesArray*       fTRDtrack;         //
   TClonesArray*       fTRrechit;         //
   TClonesArray*       fTRtrack;          //
   TClonesArray*       fMCtrtrack;        //
   TClonesArray*       fMCeventg;         //
   TClonesArray*       fAntiCluster;      //
   TClonesArray*       fAntiMCCluster;    //
   TClonesArray*       fLVL3;             //
   TClonesArray*       fLVL1;             //
   TClonesArray*       fTrRawCluster;     //
   TClonesArray*       fAntiRawCluster;   //
   TClonesArray*       fTOFRawCluster;    //
   TClonesArray*       fRICMC;            //
   TClonesArray*       fRICEvent;         //
   TClonesArray*       fRICRing;          //

   EventRoot02();
   virtual ~EventRoot02();

   ClassDef(EventRoot02,1) //
};

#endif
