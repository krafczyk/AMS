#include "Menu.h"
#include "TLegend.h"
#include <stdio.h>
#include <sys/types.h>
#include <sys/stat.h>

ClassImp(Menu);

const char *filetypes[] = { "ROOT files",    "*.root",
			    "All files",     "*",                            
//                            "ROOT macros",   "*.C",
//                            "Text files",    "*.[tT][xX][tT]",
                            0,               0 };
void Menu::TimerDone1(){
	time_t cur;
	cur=time(NULL);
	void *dirp;
	string info="Time:  ";
	info+=ctime(&cur);
	dirp=gSystem->OpenDirectory(data1_dir.c_str());
	const char *name;
	FileStat_t fs;
	struct stat sb;
	int i=0;
	static int m=0;
	char temp[20];
	gSystem->GetPathInfo(data1_dir.c_str(),fs);
	i=0;
	if(fs.fMtime>pre_time){
		//cout<<"pretime: "<<ctime(&pre_time);
		//cout<<"curtime: "<<ctime(&cur);
		while((name=gSystem->GetDirEntry(dirp))){
			cout<<name<<endl;
			if(strstr(name,"root")!=NULL){
				data1_filename=data1_dir+"/";
				data1_filename+=name;
//				gSystem->GetPathInfo(data1_filename.c_str(),fs);
			lstat(data1_filename.c_str(),&sb);	
			if(sb.st_mtim.tv_sec>pre_time){
					i++;
					//cout<<"New file name: "<<data1_filename<<", modify time "<<ctime(&(fs.fMtime));
				}
			}
		}
		sprintf(temp," %d ",i);
		info+="Message: find";
		info+=temp;
		info+=" new files";
		if(i>0){
			_fdata->data1_filename=data1_filename;	
			_fdata->data2_filename=data2_filename;
			timer2->Start(0,kTRUE);
		}
		pre_time=cur;
	}
	else{
	      info+="Message: No new files";
	}
	if(i==0){
		TCanvas* c=_fhtab->GetCanvas();
		c->cd();
		_fdata->_hists_h->Fill(ctime(&cur),0);
		_fdata->_hists_h->Draw("E1");
		_fdata->_hists_h->SetMarkerStyle(3);
		_fdata->_hists_h->SetMarkerColor(kRed);
//		cout<<"min="<<_fdata->_hists_h->GetXaxis()->GetXmin()<<",max="<<_fdata->_hists_h->GetXaxis()->GetXmax()<<",nbinx="<<_fdata->_hists_h->GetNbinsX()<<endl;
		_fdata->_hists_h->SetAxisRange(m-5<0?0:m-5,m+5>_fdata->_hists_h->GetNbinsX()?_fdata->_hists_h->GetNbinsX():m+5);	
		gPad->SetGrid();
		m++;
		c->Update();
	}
	pbar->SetInfo(info);
	
}
void Menu::TimerDone2(){
	_fdata->Generate_hist();
        draw();	
}
Menu::Menu(const TGWindow* p,Data* data,Tab_Frame* tab,Tab_Frame* stab,Tab_Frame* htab):TGMenuBar(p),flag(0),data1_filename("NULL"),data2_filename("NULL"){
	_fdata=data;
	_ftab=tab;
	_fstab=stab;
	_fhtab=htab;
	pre_time=time(NULL);
	data1_dir="Data1/";
	data2_dir="Data2/*.root";
	data2_filename=data2_dir;
	timer1=new TTimer();
	timer2=new TTimer();
	timer1->Connect("Timeout()", "Menu",this, "TimerDone1()");
	timer2->Connect("Timeout()", "Menu",this, "TimerDone2()");
	fMenuFile=new TGPopupMenu(gClient->GetRoot());
	fMenuFile->AddEntry("Manually",3);
	fMenuFile->AddEntry("Automatically",4);
	fMenuFile->AddSeparator();
	fMenuFile->AddEntry("Current file", 0);
  	fMenuFile->AddEntry("Base line file", 1);
	fMenuFile->AddSeparator();
	fMenuFile->AddEntry("Current Dir",5);
	fMenuFile->AddEntry("Base line file",6);
	fMenuFile->AddSeparator();
	fMenuFile->AddEntry("Exit", 2);
	//initially use manually mode 
	mode=false;
	fMenuFile->DisableEntry(5);
	fMenuFile->DisableEntry(6);
	fMenuFile->CheckEntry(3);
	fMenuFile->Connect("Activated(Int_t)", "Menu", this,
                      "HandleMenu(Int_t)");
	AddPopup(new TGHotString("&File"), fMenuFile, new TGLayoutHints(kLHintsTop | kLHintsLeft, 0, 4, 0, 0));
	fMenuPlot=new TGPopupMenu(gClient->GetRoot());
	fMenuPlot->AddEntry("Generate", 0);
	fMenuPlot->AddSeparator();
	fMenuPlot->AddEntry("Start",1);
	fMenuPlot->AddEntry("Stop",2);
	fMenuPlot->DisableEntry(1);
	fMenuPlot->DisableEntry(2);
	fMenuPlot->Connect("Activated(Int_t)", "Menu", this,
                      "HandleMenu2(Int_t)");
	fMenuPlot->DisableEntry(0);
	AddPopup(new TGHotString("&Plot"), fMenuPlot, new TGLayoutHints(kLHintsTop | kLHintsLeft, 0, 4, 0, 0));
}
Menu::~Menu(){
	if(fMenuFile)
		delete fMenuFile;
	if(fMenuPlot)
		delete fMenuPlot;
}
void Menu::draw(){
	Int_t i=0;
	TCanvas*c=_ftab->GetCanvas();
	TLegend*l;
	for(i=0;i<_fdata->_hists_name.size();i++){
		c->cd(_ftab->Getpid(_fdata->_hists_name[i]));
		_fdata->_hists_1[i]->Draw();
		_fdata->_hists_1[i]->SetLineColor(kRed);
		_fdata->_hists_2[i]->Draw("same");
		_fdata->_hists_2[i]->SetLineColor(kBlue);
		if(i==0){
			//TObject* old=gDirectory->GetList()->FindObject("lgr");
			//if(old==NULL){
				l=new TLegend(0.3,0.7,0.98,0.9);
				l->AddEntry(_fdata->_hists_1[0],data1_filename.c_str(),"l");
				l->AddEntry(_fdata->_hists_2[0],data2_filename.c_str(),"l");				
			//}
			l->Draw();
		}	
		c->GetPad(_ftab->Getpid(_fdata->_hists_name[i]))->SetGrid();
	}
	_ftab->GetCanvas()->Update();
	c=_fstab->GetCanvas();
	for(i=0;i<_fdata->_hists_name_summary.size();i++){
		c->cd();
		_fdata->_hists_summary[i]->Draw("bar2");
		_fdata->_hists_summary[i]->SetFillColor(42);
		gPad->SetGrid();
	}
		
	c->Update();
	c=_fhtab->GetCanvas();
	c->cd();
	_fdata->_hists_h->Draw("E1");
	_fdata->_hists_h->SetMarkerStyle(3);
	_fdata->_hists_h->SetMarkerColor(kRed);
	
	gPad->SetGrid();
	
	c->Update();
}
void Menu::HandleMenu(Int_t i){
	if(i==2)
		gApplication->Terminate(0);
	if(i==3){
		if(!fMenuFile->IsEntryChecked(3)){
			fMenuFile->CheckEntry(3);
			mode=false;
			fMenuFile->EnableEntry(0);
			fMenuFile->EnableEntry(1);
			if(!fMenuPlot->IsEntryEnabled(0)&&flag==3)
				fMenuPlot->EnableEntry(0);
			if(fMenuPlot->IsEntryEnabled(1))
				fMenuPlot->DisableEntry(1);
			if(fMenuPlot->IsEntryEnabled(2)){
				timer1->Stop();
				fMenuPlot->DisableEntry(2);
			}
		}
		if(fMenuFile->IsEntryChecked(4)){
			fMenuFile->UnCheckEntry(4);
			fMenuFile->DisableEntry(5);
                        fMenuFile->DisableEntry(6);
		}
		return;
	}
	if(i==4){
                if(!fMenuFile->IsEntryChecked(4)){
			mode=true;
                        fMenuFile->CheckEntry(4);
                        fMenuFile->EnableEntry(5);
                        fMenuFile->EnableEntry(6);
			if(fMenuPlot->IsEntryEnabled(0))
				fMenuPlot->DisableEntry(0);
			if(!fMenuPlot->IsEntryEnabled(1))
                                fMenuPlot->EnableEntry(1);
			if(fMenuPlot->IsEntryEnabled(2))
                                fMenuPlot->DisableEntry(2);

                }
                if(fMenuFile->IsEntryChecked(3)){
                        fMenuFile->UnCheckEntry(3);
                        fMenuFile->DisableEntry(0);
                        fMenuFile->DisableEntry(1);
                }
                return;
        }

	static TString dir(".");
	TGFileInfo fi;
	fi.fFileTypes = filetypes;
	fi.fIniDir    = StrDup(dir);
	fi.fFilename==NULL;
	TGFileDialog* dig=new TGFileDialog(gClient->GetRoot(), this, kFDOpen, &fi);
	dir = fi.fIniDir;	
	if(fi.fFilename!=NULL)
		switch(i){
			case 0:
				fMenuFile->CheckEntry(i);
				data1_filename=fi.fFilename;
				flag==0?flag=1:flag=3;
				break;
			case 1:
				fMenuFile->CheckEntry(i);
				data2_filename=fi.fFilename;
				flag==0?flag=2:flag=3;
				break;	
			case 5:
				fMenuFile->CheckEntry(i);
                                data1_dir=fi.fFilename;
				break;
			case 6:
				fMenuFile->CheckEntry(i);
                                data2_dir=fi.fFilename;
				data2_filename=data2_dir;
				break;
		}
	if(flag==3&&!mode)
		fMenuPlot->EnableEntry(0);
	else
		fMenuPlot->DisableEntry(0);
}
void Menu::HandleMenu2(Int_t i){
	switch(i){
		case 0:
	//Generate plots manually
			_fdata->data1_filename=data1_filename;	
			_fdata->data2_filename=data2_filename;
			
	//		_fdata->Generate_hist();
	//		draw();
			timer2->Start(1000, kTRUE);		
			break;
	//Start automatically generating plots
		case 1:
			timer1->Start(5000,kFALSE);	
			fMenuPlot->DisableEntry(1);
			fMenuPlot->EnableEntry(2);
			break;
	//Stop automaticall generating plots
		case 2:
			timer1->Stop();
			fMenuPlot->DisableEntry(2);
                        fMenuPlot->EnableEntry(1);
			break;
	}
}
