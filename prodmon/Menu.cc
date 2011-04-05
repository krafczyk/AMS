#include "Menu.h"
//#include "InputDialog.h"
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
	m++;
	string info="Time:  ";
	info+=ctime(&cur);
	dirp=gSystem->OpenDirectory(data1_dir.c_str());
	const char *name;
	FileStat_t fs;
	struct stat sb;
	int i=0;
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
		
		pre_time=cur;
		info+=" new files";
	}
	else{
	      info+="Message: No new files";
	}
	timer2->Start(0,kTRUE);
	pbar->SetInfo(info);
}
void Menu::TimerDone2(){
	cout<<"n1="<<data1_filename<<endl;
	cout<<"n2="<<data2_filename<<endl;
	if(data1_filename!=_fdata->data1_filename){
		_fdata->Set_data1_filename(data1_filename);
	}	
	if(data2_filename!=_fdata->data2_filename){
		_fdata->Set_data2_filename(data2_filename);
	}
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
	m=0;
	if(getenv("AMSProdMonRefFile")==NULL){
		printf("Please set env AMSProdMonRefFile\n");
		exit(0);
	}
	data2_filename=getenv("AMSProdMonRefFile");
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
	fMenuFile->AddEntry("Use cmd",7);
        //fMenuFile->AddEntry("Set cmd",8);

	fMenuFile->AddSeparator();
	fMenuFile->AddEntry("Exit", 2);
	//initially use manually mode 
	mode=false;
	//file=new TFile("History.root","RECREATE");
	//file->cd();
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
	fMenuHS=new TGPopupMenu(gClient->GetRoot());
	fMenuHS->AddEntry("nEvent",0);
	fMenuHS->CheckEntry(0);
	fMenuHS->AddEntry("nParticle",1);
	fMenuHS->AddEntry("iTrdTrack",2);
	fMenuHS->AddEntry("iTrdHTrack",3);
	fMenuHS->AddEntry("iTrTrack",4);
	fMenuHS->AddEntry("iRichRing",5);
	fMenuHS->AddEntry("iEcalShower",6);
	fMenuHS->AddEntry("iTrTrack_iTrdTrack",7);
	fMenuHS->AddEntry("iTrTrack_iEcalShower",8);
	fMenuHS->AddEntry("iTrTrack_iRichRing",9);
	AddPopup(new TGHotString("&History"), fMenuHS, new TGLayoutHints(kLHintsTop | kLHintsLeft, 0, 4, 0, 0));
	fMenuHS->Connect("Activated(Int_t)", "Menu", this,
                      "HandleMenu3(Int_t)");
	_fhs=1;	
}
void Menu::draw_history(){
	TCanvas*c=_fhtab->GetCanvas();
	c->cd(1);
	Int_t i=0;
	Int_t flag=0;
	TLegend*l=new TLegend(0.6,0.7,0.9,0.9);
	for(i=0;i<_fdata->_hists_h.size();i++){
	//	cout<<_fhs<<","<<((_fhs>>i)&0x1)<<endl;
		if(((_fhs>>i)&0x1)){
			
			if(flag==0){
				_fdata->_hists_h[i]->Draw("E1");
			        _fdata->_hists_h[i]->SetAxisRange(m-5<0?0:m-5,m+5>_fdata->_hists_h[i]->GetNbinsX()?_fdata->_hists_h[i]->GetNbinsX():m+5);	
				flag=1;
			}
			else{	
				_fdata->_hists_h[i]->Draw("E1 SAME");
			}
			if(i==0){
				_fdata->_hists_h[i]->SetMarkerStyle(22);
				_fdata->_hists_h[i]->SetMarkerColor(2);
			}
			if(i==1){
				_fdata->_hists_h[i]->SetMarkerStyle(22);
				_fdata->_hists_h[i]->SetMarkerColor(3);
			}
			if(i==2){
				_fdata->_hists_h[i]->SetMarkerStyle(22);
				_fdata->_hists_h[i]->SetMarkerColor(4);
			}
			if(i==3){
				_fdata->_hists_h[i]->SetMarkerStyle(22);
				_fdata->_hists_h[i]->SetMarkerColor(5);
			}
			if(i==4){
				_fdata->_hists_h[i]->SetMarkerStyle(22);
				_fdata->_hists_h[i]->SetMarkerColor(6);
			}
			if(i==5){
				_fdata->_hists_h[i]->SetMarkerStyle(22);
				_fdata->_hists_h[i]->SetMarkerColor(7);
			}
			if(i==6){
				_fdata->_hists_h[i]->SetMarkerStyle(21);
				_fdata->_hists_h[i]->SetMarkerColor(2);
			}
			if(i==7){
				_fdata->_hists_h[i]->SetMarkerStyle(21);
				_fdata->_hists_h[i]->SetMarkerColor(3);
			}
			if(i==8){
				_fdata->_hists_h[i]->SetMarkerStyle(21);
				_fdata->_hists_h[i]->SetMarkerColor(4);
			}
			if(i==9){
				_fdata->_hists_h[i]->SetMarkerStyle(21);
				_fdata->_hists_h[i]->SetMarkerColor(5);
			}
			l->AddEntry(_fdata->_hists_h[i],_fdata->_hists_name_h[i].c_str(),"lp");
		}
	}
	l->Draw();
	gPad->SetGrid();
	c->Update();
	//for(i=0;i<10;i++)
	//_fdata->_hists_h[i]->Write();
}
Menu::~Menu(){
	//file->Close();
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
		_fdata->_hists_2[i]->Draw("SAME");
		_fdata->_hists_2[i]->SetLineColor(kBlue);
		if(i==0){
			//TObject* old=gDirectory->GetList()->FindObject("lgr");
			//if(old==NULL){
				l=new TLegend(0.05,0.8,0.99,0.95);
				l->AddEntry(_fdata->_hists_1[0],data1_filename.c_str(),"l");
				l->AddEntry(_fdata->_hists_2[0],data2_filename.c_str(),"l");				
			//}
			l->Draw();
		}	
		c->GetPad(_ftab->Getpid(_fdata->_hists_name[i]))->SetGrid();
	}
	_ftab->GetCanvas()->Update();
	c=_fstab->GetCanvas();
	c->cd(1);
	l=new TLegend(0.5,0.85,0.98,0.95);
	_fdata->_hists_summary[0]->Draw("bar2");
	_fdata->_hists_summary[0]->SetFillColor(42);
	//_fdata->_hists_summary[0]->SetMarkerStyle(25);
	//_fdata->_hists_summary[0]->SetMarkerColor(3);

	_fdata->_hists_summary[1]->Draw("bar2 SAME");
	//_fdata->_hists_summary[1]->SetMarkerStyle(26);
	_fdata->_hists_summary[1]->SetFillColor(3);
	l->AddEntry(_fdata->_hists_summary[0],_fdata->_hists_summary[0]->GetTitle(),"f");
	l->AddEntry(_fdata->_hists_summary[1],_fdata->_hists_summary[1]->GetTitle(),"f");
	l->Draw();
	gPad->SetGrid();
	c->Update();
	draw_history();
}
void Menu::HandleMenu3(Int_t i){
	switch(i){
		case 0:
			if(!fMenuHS->IsEntryChecked(0)){
				_fhs+=nEvent;
				fMenuHS->CheckEntry(0);
			}
			else{
				_fhs-=nEvent;
				fMenuHS->UnCheckEntry(0);
			}
			break;
		case 1:
			if(!fMenuHS->IsEntryChecked(1)){
				_fhs+=nParticle;
				fMenuHS->CheckEntry(1);
			}
			else{
				_fhs-=nParticle;
				fMenuHS->UnCheckEntry(1);
			}
			break;
		case 2:
			if(!fMenuHS->IsEntryChecked(2)){
				_fhs+=iTrdTrack;
				fMenuHS->CheckEntry(2);
			}
			else{
				_fhs-=iTrdTrack;
				fMenuHS->UnCheckEntry(2);
			}
			break;
		case 3:
			if(!fMenuHS->IsEntryChecked(3)){
				_fhs+=iTrdHTrack;
				fMenuHS->CheckEntry(3);
			}
			else{
				_fhs-=iTrdHTrack;
				fMenuHS->UnCheckEntry(3);
			}
			break;
		case 4:
			if(!fMenuHS->IsEntryChecked(4)){
				_fhs+=iTrTrack;
				fMenuHS->CheckEntry(4);
			}
			else{
				_fhs-=iTrTrack;
				fMenuHS->UnCheckEntry(4);
			}
			break;
		case 5:
			if(!fMenuHS->IsEntryChecked(5)){
				_fhs+=iRichRing;
				fMenuHS->CheckEntry(5);
			}
			else{
				_fhs-=iRichRing;
				fMenuHS->UnCheckEntry(5);
			}
			break;
		case 6:
			if(!fMenuHS->IsEntryChecked(6)){
				_fhs+=iEcalShower;
				fMenuHS->CheckEntry(6);
			}
			else{
				_fhs-=iEcalShower;
				fMenuHS->UnCheckEntry(6);
			}
			break;
		case 7:
			if(!fMenuHS->IsEntryChecked(7)){
				_fhs+=iTrTrack_iTrdTrack;
				fMenuHS->CheckEntry(7);
			}
			else{
				_fhs-=iTrTrack_iTrdTrack;
				fMenuHS->UnCheckEntry(7);
			}
			break;
		case 8:
			if(!fMenuHS->IsEntryChecked(8)){
				_fhs+=iTrTrack_iRichRing;
				fMenuHS->CheckEntry(8);
			}
			else{
				_fhs-=iTrTrack_iRichRing;
				fMenuHS->UnCheckEntry(8);
			}
			break;
		case 9:
			if(!fMenuHS->IsEntryChecked(9)){
				_fhs+=iTrTrack_iEcalShower;
				fMenuHS->CheckEntry(9);
			}
			else{
				_fhs-=iTrTrack_iEcalShower;
				fMenuHS->UnCheckEntry(9);
			}
			break;	
	}
	draw_history();
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
        FILE *fp;
        int status;
        char path[1000];
	if(i==7){
                _fcmd+=">lastname.dat";
                status= system(_fcmd.c_str());
                if (status == -1) {
                        printf("Failed to run command %s\n",_fcmd.c_str());
                        return;
                }
                fp=fopen("lastname.dat","r");
                if(fp==NULL){
                        printf("Failed to open file lastname.dat\n");
                        return;
                }
                fscanf(fp,"%s",path);
                data1_filename=path;
                fclose(fp);
		m++;
                //printf("%s find new file %s\n",_fcmd.c_str(),path);
		timer2->Start(0,kTRUE);
                return;
        }
        static char answer[128];
	if(i==8){
           // Prompt for string. The typed in string is returned.
          // new InputDialog("prompt", "defval", answer);
           _fcmd=answer;
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
			
	//		_fdata->Generate_hist();
	//		draw();
			timer2->Start(1000, kTRUE);		
			break;
	//Start automatically generating plots
		case 1:
			timer1->Start(300000,kFALSE);	
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
