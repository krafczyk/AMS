//  $Id: AMSTrackReader.h,v 1.2 2001/01/22 17:32:32 choutko Exp $
#ifndef AMSTrackReader_H
#define AMSTrackReader_H

//////////////////////////////////////////////////////////////////////////
//                                                                      //
// AMSRoot TrackReader class.                                           //
//                                                                      //
//////////////////////////////////////////////////////////////////////////

#ifndef AMSMaker_H
#include "AMSMaker.h"
#endif
#ifndef ROOT_TH1
#include <TH1.h>
#endif
#ifndef ROOT_TTree
#include <TTree.h>
#endif

class AMSTrack;

class AMSTrackReader : public AMSMaker {

protected:
   Int_t          m_NTracks;           //Number of tracks
/*
   Float_t        m_MinPT;             //Minimum PT for track
   Float_t        m_MaxEta;            //maximum eta for track
*/

//     Tracks histograms
   TH1F          *m_Mult;              //tracks multiplicity

public:
                  AMSTrackReader();
                  AMSTrackReader(const char *name, const char *title);
   virtual       ~AMSTrackReader();
   AMSTrack      *AddTrack(Int_t code, Int_t mcparticle);
   virtual void   Clear(Option_t *option="");
   virtual void   Finish();
//         void   HelixParameters(Float_t charge, Float_t *vert1, Float_t *pvert1, Float_t *b);
   virtual void   Init(TTree * h1=0);
   virtual void   Make();
   virtual void   PrintInfo();
// virtual Bool_t Enabled(TObject * obj);

//    Getters

//     Getters Tracks histograms
   TH1F          *Mult() {return m_Mult;}


//    Setters for tracks

   ClassDef(AMSTrackReader, 1)   //AMSRoot TrackReader
};

#endif
