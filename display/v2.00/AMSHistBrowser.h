//  $Id: AMSHistBrowser.h,v 1.2 2001/01/22 17:32:37 choutko Exp $
#ifndef AMSHistBrowser_H
#define AMSHistBrowser_H

//////////////////////////////////////////////////////////////////////////
//                                                                      //
// AMSHistBrowser                                                       //
//                                                                      //
// helper class to browse AMSRoot Makers histograms.                    //
//                                                                      //
//////////////////////////////////////////////////////////////////////////

#ifndef ROOT_TNamed
#include <TNamed.h>
#endif

class AMSHistBrowser : public TNamed {

public:
                     AMSHistBrowser();
   virtual          ~AMSHistBrowser() {;}
   virtual void      Browse(TBrowser *b);
   Bool_t            IsFolder() {return kTRUE;}

   ClassDef(AMSHistBrowser, 0)   //helper class to browse AMSRoot Makers histograms
};

#endif
