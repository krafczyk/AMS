//  $Id: AMSColorManager.h,v 1.2 2001/01/22 17:32:37 choutko Exp $
#ifndef AMSColorManager_H
#define AMSColorManager_H

//////////////////////////////////////////////////////////////////////////
//                                                                      //
// AMSColorManager                                                       //
//                                                                      //
// class to manage colors used by AMSRoot event display program.        //
//                                                                      //
//////////////////////////////////////////////////////////////////////////

#ifndef ROOT_TNamed
#include <TNamed.h>
#endif

static const int MaxColor = 256;

class AMSColorManager : public TNamed {

protected:
//	struct { Float_t r; Float_t g; Float_t b; } colorTable[MaxColor];
	Float_t red[MaxColor];
	Float_t green[MaxColor];
	Float_t blue[MaxColor];

public:
                     AMSColorManager(char * name="AMSColorManager", char * title="AMS Event Display Color Manager");
   virtual          ~AMSColorManager() {;}
            Int_t    GetColorAt(Int_t i, Float_t *r, Float_t *g, Float_t *b);
           Bool_t    IsValid(Int_t i);
	    Int_t    Migrate(Int_t src, Int_t dest);
	     void    ColorTest(Int_t begin=0, Int_t end=MaxColor-1);

   ClassDef(AMSColorManager, 0)   //helper class to browse AMSRoot Makers histograms
};

#endif
