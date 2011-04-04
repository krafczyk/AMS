#include "TGMenu.h"
#include "TGFileDialog.h"
#include "Tab_Frame.h"
#include "Data.h"
#include "TTimer.h"
#include "time.h"
#include "ProgBar.h"
#include "TSystem.h"
#include <string>

using namespace std;
#ifndef Menu_H_
#define Menu_H_
extern void myzoom();
extern ProgBar* pbar;
class Menu:public TGMenuBar{
public:
	Menu(const TGWindow* p ,Data* data,Tab_Frame* tab,Tab_Frame* stab,Tab_Frame* htab);
	~Menu();
	string Get_data1_filename(){return data1_filename;};
	string Get_data2_filename(){return data2_filename;};
	TGPopupMenu*fMenuFile;
	TGPopupMenu*fMenuPlot;
	void TimerDone1();
	void TimerDone2();
	void HandleMenu(Int_t i);
	void HandleMenu2(Int_t i);

private:
	TTimer* timer1,*timer2;
	string data1_filename;
	string data2_filename;
	string data1_dir;
	string data2_dir;
		
	
//0: no data file has been selected or all old
//1: data1 file has been selected
//2: data2 file has been selected 
//3: both data1&data2 files have been selected or updated
	Int_t flag;
//false: Manually
//true:  Automatically
	Bool_t mode;
	Data* _fdata;
	Tab_Frame* _ftab;
	Tab_Frame* _fstab;
	Tab_Frame* _fhtab;
	time_t pre_time;	
	void draw();
	ClassDef(Menu,1)
};

#endif
