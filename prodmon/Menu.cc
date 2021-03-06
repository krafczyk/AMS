#include "Menu.h"
#include "InputDialog.h"
#include "TGMsgBox.h"
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
	string info="Current Time:  ";
	info+=ctime(&cur);
	dirp=gSystem->OpenDirectory(data1_dir.c_str());
	const char *name;
	FileStat_t fs;
	struct stat sb;
	int i=0;
	char temp[20];
	FILE *fp;
        int status;
        char path[1000];
	if(mode==1){
		gSystem->GetPathInfo(data1_dir.c_str(),fs);
		i=0;
		if(fs.fMtime>pre_time){
			cout<<"pretime: "<<ctime(&pre_time);
			cout<<"curtime: "<<ctime(&cur);
			while((name=gSystem->GetDirEntry(dirp))){
	//			cout<<name<<endl;
				if(strstr(name,"root")!=NULL){
					data1_filename=data1_dir+"/";
					data1_filename+=name;
//					gSystem->GetPathInfo(data1_filename.c_str(),fs);
				
				lstat(data1_filename.c_str(),&sb);	
			//	printf("%s : time %d , pre_time %d\n",data1_filename.c_str(),sb.st_mtim.tv_sec,pre_time);
				if(sb.st_mtim.tv_sec>pre_time){
						i++;
						cout<<"New file name: "<<data1_filename<<", modify time "<<ctime(&(fs.fMtime));
					//timer2->Start(0,kTRUE);
						TimerDone2();
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
	}
	else if(mode==2){
		_fcmd+=">lastname.dat";
                status= system(_fcmd.c_str());
                if (status == -1) {
                        printf("Failed to run command %s\n",_fcmd.c_str());
                        exit(0);
                }
                fp=fopen("lastname.dat","r");
                if(fp==NULL){
                        printf("Failed to open file lastname.dat\n");
                }
		else{
			fscanf(fp,"%s",path);
                	data1_filename=path;
		}
                fclose(fp);

                //printf("%s find new file %s\n",_fcmd.c_str(),path);
                timer2->Start(0,kTRUE);
		
		info+=ctime(&last_find);
		info+="Message: find";
		info+=path;
		pre_time=cur;
	}
	else if(mode==0){
		printf("mode==0\n");	
	}
	pbar->SetInfo(info);
}
void Menu::TimerDone2(){
	//m++;
	bool flag=false;
	cout<<"n1="<<data1_filename<<endl;
	cout<<"n2="<<data2_filename<<endl;
	if(data1_filename!=_fdata->data1_filename){
		flag=true;
		_fdata->Set_data1_filename(data1_filename);
	}	
	if(data2_filename!=_fdata->data2_filename){
		flag=true;
		_fdata->Set_data2_filename(data2_filename);
	}
	if(mode==2&&flag==true){
		last_find=time(NULL);
		cout<<"Mode=2 , Automatically use cmd mode call\n";
		_fdata->Generate_hist();
		draw();
		history_changed=true;
	}
	else if(mode==1&&flag==true){
		cout<<"Mode=1 , Automatically check dirctory mode call\n";
		_fdata->Generate_hist();
		draw();
		history_changed=true;
	}
	else if(mode !=2&&mode!=1){
		cout<<"Mode="<<mode<<",flag="<<flag<<endl;
		_fdata->Generate_hist();
        	draw();
		history_changed=true;
	}	
}
void Menu::TimerSaver(){
	if(history_changed){
		file->cd();
		_fhtab->GetCanvas()->Write(0,TObject::kOverwrite);
		history_changed=false;
	}
}
Menu::Menu(const TGWindow* p,Data* data,Tab_Frame* tab,Tab_Frame* stab,Tab_Frame* htab):TGMenuBar(p),flag(0),data1_filename("NULL"),data2_filename("NULL"){
	_fdata=data;
	_ftab=tab;
	_fstab=stab;
	_fhtab=htab;
	timer1_on=false;
	timer2_on=false;
	pre_time=time(NULL)-3600;
	//pre_time=130600000;

	time_val=30000;
	data1_dir="/fc02dat1/Data/AMS02/2011B/ISS.B515/std/";
	data2_dir="Data2/*.root";
	data2_filename=data2_dir;
	m=0;
	history_changed=true;
	mode=0;	
	if(getenv("AMSProdMonRefFile")==NULL){
		printf("Please setenv AMSProdMonRefFile, See https://twiki.cern.ch/twiki/bin/view/AMS/AMSProdMonRefFile \n");
		exit(0);
	}
	data2_filename=getenv("AMSProdMonRefFile");
	timer1=new TTimer();
	timer2=new TTimer();
	timer_saver=new TTimer();
	timer_saver->Start(30000,kFALSE);
	timer1->Connect("Timeout()", "Menu",this, "TimerDone1()");
	timer2->Connect("Timeout()", "Menu",this, "TimerDone2()");
	timer_saver->Connect("Timeout()", "Menu",this, "TimerSaver()");
	fMenuFile=new TGPopupMenu(gClient->GetRoot());
	fMenuFile->AddEntry("Manually",3);
	fMenuFile->AddEntry("Automatically",4);
	fMenuFile->AddSeparator();
	fMenuFile->AddEntry("Current file", 0);
  	fMenuFile->AddEntry("Base line file", 1);
	fMenuFile->AddSeparator();
	fMenuFile->AddEntry("Current Dir",5);
	fMenuFile->AddEntry("Base line file",6);
	fMenuFile->DisableEntry(5);
	fMenuFile->DisableEntry(6);
	fMenuFile->AddSeparator();
	fMenuFile->AddEntry("Use cmd",7);
	fMenuFile->AddEntry("Automatically use cmd",9);
        //fMenuFile->AddEntry("Set cmd",8);

	fMenuFile->AddSeparator();
	fMenuFile->AddEntry("Exit", 2);
	//initially use manually mode 
	mode=0;
	time_t t1;
        time(&t1);
        char filename[100];
        struct tm* timeinfo=localtime(&t1);
        char c;
        if(timeinfo->tm_hour>=7&&timeinfo->tm_hour<15)
                c='A';
        else if(timeinfo->tm_hour>=15&&timeinfo->tm_hour<23)
                c='B';
        else
                c='C';
        sprintf(filename,"%d%d%d%c.root",timeinfo->tm_year+1900,timeinfo->tm_mon+1,timeinfo->tm_mday,c);

	file=new TFile(filename,"UPDATE");
	file->cd();
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
	fMenuPlot->AddEntry("Set Time Interval",3);
	fMenuPlot->Connect("Activated(Int_t)", "Menu", this,
                      "HandleMenu2(Int_t)");
	fMenuPlot->DisableEntry(0);
	AddPopup(new TGHotString("&Plot"), fMenuPlot, new TGLayoutHints(kLHintsTop | kLHintsLeft, 0, 4, 0, 0));
	fMenuHS=new TGPopupMenu(gClient->GetRoot());
	fMenuHS->AddEntry("nEvent",0);
	fMenuHS->CheckEntry(0);
	fMenuHS->AddEntry("nParticle",1);
	fMenuHS->CheckEntry(1);
	fMenuHS->AddEntry("iTrdTrack",2);
	fMenuHS->CheckEntry(2);
	fMenuHS->AddEntry("iTrdHTrack",3);
	fMenuHS->CheckEntry(3);
	fMenuHS->AddEntry("iTrTrack",4);
	fMenuHS->CheckEntry(4);
	fMenuHS->AddEntry("iRichRing",5);
	fMenuHS->CheckEntry(5);
	fMenuHS->AddEntry("iEcalShower",6);
	fMenuHS->CheckEntry(6);
	fMenuHS->AddEntry("iTrTrack_iTrdTrack",7);
	fMenuHS->CheckEntry(7);
	fMenuHS->AddEntry("iTrTrack_RichRing",8);
	fMenuHS->CheckEntry(8);
	fMenuHS->AddEntry("iTrTrack_iEcalShower",9);
	fMenuHS->CheckEntry(9);
	AddPopup(new TGHotString("&History"), fMenuHS, new TGLayoutHints(kLHintsTop | kLHintsLeft, 0, 4, 0, 0));
	fMenuHS->Connect("Activated(Int_t)", "Menu", this,
                      "HandleMenu3(Int_t)");
	_fhs=1+2+4+8+16+32+64+128+256+512+1024;	
}
void Menu::draw_history(){
	TCanvas*c=_fhtab->GetCanvas();
	c->SetFillColor(28);
	c->cd(1);
	gPad->SetFillColor(41);
        gPad->SetFrameFillColor(33);
	gPad->SetLogy();
	Int_t i=0;
	Int_t flag=0;
	TLegend*l=new TLegend(0.6,0.7,0.9,0.9);
	for(i=0;i<_fdata->_hists_h.size();i++){
	//	cout<<_fhs<<","<<((_fhs>>i)&0x1)<<endl;
		if(((_fhs>>i)&0x1)){
			
			if(flag==0){
				_fdata->_hists_h[i]->Draw("P0L");
			        _fdata->_hists_h[i]->SetAxisRange(m-10<0?0:m-10,m);	
			_fdata->_hists_h[i]->SetMinimum(1.0);	
			//cout<<"m="<<m<<endl;
				m++;
				flag=1;
			}
			else{	
				_fdata->_hists_h[i]->Draw("P0L SAME");
			}
			if(i==0){
				_fdata->_hists_h[i]->SetMarkerStyle(22);
				_fdata->_hists_h[i]->SetMarkerColor(2);
				_fdata->_hists_h[i]->SetLineColor(2);
			}
			if(i==1){
				_fdata->_hists_h[i]->SetMarkerStyle(22);
				_fdata->_hists_h[i]->SetMarkerColor(3);
				_fdata->_hists_h[i]->SetLineColor(3);
			}
			if(i==2){
				_fdata->_hists_h[i]->SetMarkerStyle(22);
				_fdata->_hists_h[i]->SetMarkerColor(4);
				_fdata->_hists_h[i]->SetLineColor(4);
			}
			if(i==3){
				_fdata->_hists_h[i]->SetMarkerStyle(22);
				_fdata->_hists_h[i]->SetMarkerColor(5);
				_fdata->_hists_h[i]->SetLineColor(5);
			}
			if(i==4){
				_fdata->_hists_h[i]->SetMarkerStyle(22);
				_fdata->_hists_h[i]->SetMarkerColor(6);
				_fdata->_hists_h[i]->SetLineColor(6);
			}
			if(i==5){
				_fdata->_hists_h[i]->SetMarkerStyle(22);
				_fdata->_hists_h[i]->SetMarkerColor(7);
				_fdata->_hists_h[i]->SetLineColor(7);
			}
			if(i==6){
				_fdata->_hists_h[i]->SetMarkerStyle(21);
				_fdata->_hists_h[i]->SetMarkerColor(2);
				_fdata->_hists_h[i]->SetLineColor(2);
			}
			if(i==7){
				_fdata->_hists_h[i]->SetMarkerStyle(21);
				_fdata->_hists_h[i]->SetMarkerColor(3);
				_fdata->_hists_h[i]->SetLineColor(3);
			}
			if(i==8){
				_fdata->_hists_h[i]->SetMarkerStyle(21);
				_fdata->_hists_h[i]->SetMarkerColor(4);
				_fdata->_hists_h[i]->SetLineColor(4);
			}
			if(i==9){
				_fdata->_hists_h[i]->SetMarkerStyle(21);
				_fdata->_hists_h[i]->SetMarkerColor(5);
				_fdata->_hists_h[i]->SetLineColor(5);
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
	c->SetFillColor(28);
	
	TLegend*l;
	for(i=0;i<_fdata->_hists_name.size();i++){
		c->cd(_ftab->Getpid(_fdata->_hists_name[i]));
		gPad->SetFillColor(41);
        	gPad->SetFrameFillColor(33);
		_fdata->_hists_1[i]->Draw();
		_fdata->_hists_1[i]->SetLineColor(kRed);
		_fdata->_hists_2[i]->Draw("SAME");
		_fdata->_hists_2[i]->SetLineColor(kBlue);
		if(i==0){
			//TObject* old=gDirectory->GetList()->FindObject("lgr");
			//if(old==NULL){
				l=new TLegend(0.05,0.8,0.99,0.95);
				l->AddEntry(_fdata->_hists_1[0],Form("Current: %s",data1_filename.c_str()),"l");
				l->AddEntry(_fdata->_hists_2[0],Form("Referen: %s",data2_filename.c_str()),"l");				
			//}
			l->Draw();
		}	
		c->GetPad(_ftab->Getpid(_fdata->_hists_name[i]))->SetGrid();
	}
	_ftab->GetCanvas()->Update();
	c=_fstab->GetCanvas();
	c->cd(1);
	c->SetFillColor(28);
	gPad->SetFillColor(41);
        gPad->SetFrameFillColor(33);
	l=new TLegend(0.5,0.85,0.98,0.95);
	_fdata->_hists_summary[0]->Draw("bar2");
	_fdata->_hists_summary[0]->SetMinimum(1.0);
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
	gPad->SetLogy();
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
	m--;
	draw_history();
}
void Menu::HandleMenu(Int_t i){
	if(i==2)
		gApplication->Terminate(0);
	if(i==3){
		if(!fMenuFile->IsEntryChecked(3)){
			fMenuFile->CheckEntry(3);
			//Manually mode
			mode=0;
			fMenuFile->EnableEntry(0);
			fMenuFile->EnableEntry(1);
			//if(
			//if(!fMenuPlot->IsEntryEnabled(0)&&fMenuFile->IsEntryChecked(0)&&fMenuFile->IsEntryChecked(1))
				fMenuPlot->EnableEntry(0);
			if(fMenuPlot->IsEntryEnabled(1))
				fMenuPlot->DisableEntry(1);
			if(fMenuPlot->IsEntryEnabled(2)){
				timer1->Stop();
				fMenuPlot->DisableEntry(2);
			}
			mode=0;
		}
		if(fMenuFile->IsEntryChecked(4)){
			fMenuFile->UnCheckEntry(4);
			fMenuFile->DisableEntry(5);
                        fMenuFile->DisableEntry(6);
		}
		if(fMenuFile->IsEntryChecked(9))
			fMenuFile->UnCheckEntry(9);
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
			else{
				if(!fMenuPlot->IsEntryEnabled(1)){
                                	fMenuPlot->EnableEntry(1);
						
				}
				if(fMenuPlot->IsEntryEnabled(2)){
                                	fMenuPlot->DisableEntry(2);
					timer1->Stop();
				}
			}
			//Antomatically check directory mode	
			mode=1;
                }
		if(fMenuFile->IsEntryEnabled(9))
			fMenuFile->UnCheckEntry(9);
                if(fMenuFile->IsEntryChecked(3)){
                        fMenuFile->UnCheckEntry(3);
                        fMenuFile->DisableEntry(0);
                        fMenuFile->DisableEntry(1);
                }
		if(!fMenuPlot->IsEntryEnabled(1)){
                        fMenuPlot->EnableEntry(1);
		}
                if(fMenuPlot->IsEntryEnabled(2)){
                        fMenuPlot->DisableEntry(2);
                        timer1->Stop();
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
                        exit(0);
                }
                fp=fopen("lastname.dat","r");
                if(fp==NULL){
			system("cat lastname.dat");
                        printf("Failed to open file lastname.dat\n");
                        return;
                }
                fscanf(fp,"%s",path);
                data1_filename=path;
                fclose(fp);
		
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
	if(i==9){
		if(!fMenuFile->IsEntryChecked(9)){
			if(fMenuFile->IsEntryChecked(4)){
				fMenuFile->UnCheckEntry(4);
				if(fMenuPlot->IsEntryEnabled(2)){
					timer1->Stop();
					fMenuPlot->DisableEntry(2);
					fMenuPlot->EnableEntry(1);
				}
			}	
			fMenuFile->CheckEntry(9);
			//Automatically use cmd mode
			mode=2;
		}
		if(fMenuFile->IsEntryChecked(3)){
			fMenuFile->UnCheckEntry(3);
			fMenuPlot->DisableEntry(0);
			fMenuPlot->EnableEntry(1);
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
			
	//		_fdata->Generate_hist();
	//		draw();
			timer2->Start(1000, kTRUE);		
			break;
	//Start automatically generating plots
		case 1:
			timer1->Start(time_val,kFALSE);	
			timer1_on=true;
			fMenuPlot->DisableEntry(1);
			fMenuPlot->EnableEntry(2);
			break;
	//Stop automaticall generating plots
		case 2:
			timer1->Stop();
			timer1_on=false;
			fMenuPlot->DisableEntry(2);
                        fMenuPlot->EnableEntry(1);
			break;
		case 3:
			char answer[128];
			new InputDialog("Time interval (ms)","30000",answer);
			time_val=atoi(answer);
			if(time_val==0){
				new TGMsgBox(gClient->GetRoot(),this,"Error","Please input integer");	
			}
			else{
				if(timer1_on){
					timer1->Stop();
					timer1->Start(time_val,kFALSE);
				}
			}
			break;
	}
}
