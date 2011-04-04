#include <iostream>
#include <vector>
#include <map>
#include <string>

#include "TGFrame.h"
#include "TRootEmbeddedCanvas.h"
#include "TGWindow.h"
#include "TCanvas.h"
#include "TH1F.h"
#include "TGMenu.h"
#include "TApplication.h"
#include "TGLabel.h"
#include "TTimer.h"
#include "TGProgressBar.h"

using namespace std;


#ifndef _ProgBar_H_
#define _ProgBar_H_
class ProgBar:public TGTransientFrame{
public:
        ProgBar(TGWindow* p);
        ~ProgBar();
        void Show(){
//                MapSubwindows();
                Resize(GetDefaultSize());
                MapWindow();
        };
        void SetPosition(float pos){if(fHProg) fHProg->SetPosition(pos);};
        void SetInfo(string info){if(finfo) finfo->SetText(info.c_str());};
        void ShowPosition(){if(fHProg) fHProg->ShowPos(true);};
        void CloseWindows();
private:
        TGHProgressBar* fHProg;
        TGLabel* finfo;
	ClassDef(ProgBar,1)
};
#endif
