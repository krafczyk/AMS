#include "Data.h"
extern ProgBar* pbar;
Data::Data(vector<string> hists_name,vector<string> hists_name_summary,vector<string> hists_name_h):_hists_name(hists_name),data1_filename("NULL"),data2_filename("NULL"),_hists_name_summary(hists_name_summary),_hists_name_h(hists_name_h){
	Int_t i=0;
	n1_changed=true;
	n2_changed=true;
	nhist=hists_name.size();
	_hists_1.resize(nhist);
	_hists_2.resize(nhist);
	i=hists_name_summary.size();
	_hists_summary.resize(2);//hists_name_summary.size())
	for(i=0;i<nhist;i++){
		_hists_1[i]=NULL;
		_hists_2[i]=NULL;
	}
	for(i=0;i<2;i++){
		_hists_summary[i]=NULL;
	}
	_hists_h.resize(hists_name_h.size());
	for(i=0;i<hists_name_h.size();i++){
		_hists_h[i]=new TH1F(hists_name_h[i].c_str(),"History Record",10,0,10);
		_hists_h[i]->SetBit(TH1::kCanRebin);
        	_hists_h[i]->SetStats(0);
       		//_hists_h[i]->SetBarWidth(0.6);
        	//_hists_h[i]->SetBarOffset(0.1);
	}
}

Data::~Data(){
	Int_t i=0;
	/*if(_hists->size()>0){
		for(i=0;i<_hist.size();i++)
			delete _hists[i];
		delete []_hists;
	}*/
}
Bool_t Data::Generate_hist(){
	bool nref=n1_changed||n2_changed;
	Int_t i=0;
	Double_t temp,temp2,temp3;
	for(i=0;i<nhist;i++){
	if(n1_changed){
		if(_hists_1[i]!=NULL){
			TObject* old=gDirectory->GetList()->FindObject(Form("%s_1",_hists_name[i].c_str()));
			gDirectory->GetList()->Remove(old);
			delete _hists_1[i];
			_hists_1[i]=NULL;
		}
		if(i==0)
		_hists_1[i]=new TH1F(Form("%s_2",_hists_name[i].c_str()),_hists_name[i].c_str(),2000,-500,500);
		if(i==1)
		_hists_1[i]=new TH1F(Form("%s_2",_hists_name[i].c_str()),_hists_name[i].c_str(),50,0,50);
		if(i==2)
		_hists_1[i]=new TH1F(Form("%s_2",_hists_name[i].c_str()),_hists_name[i].c_str(),2000,-500,500);
		if(i==3)
		_hists_1[i]=new TH1F(Form("%s_2",_hists_name[i].c_str()),_hists_name[i].c_str(),50,0,50);
	}
	if(n2_changed){
		if(_hists_2[i]!=NULL){
			TObject* old=gDirectory->GetList()->FindObject(Form("%s_2",_hists_name[i].c_str()));
                        gDirectory->GetList()->Remove(old);
			delete _hists_2[i];
			_hists_2[i]=NULL;
		}
		if(i==0)
		_hists_2[i]=new TH1F(Form("%s_2",_hists_name[i].c_str()),_hists_name[i].c_str(),2000,-500,500);
		if(i==1)
		_hists_2[i]=new TH1F(Form("%s_2",_hists_name[i].c_str()),_hists_name[i].c_str(),50,0,50);
		if(i==2)
		_hists_2[i]=new TH1F(Form("%s_2",_hists_name[i].c_str()),_hists_name[i].c_str(),2000,-500,500);
		if(i==3)
		_hists_2[i]=new TH1F(Form("%s_2",_hists_name[i].c_str()),_hists_name[i].c_str(),50,0,50);
	}
	}
	if(n1_changed||n2_changed)
	for(i=0;i<2;i++){
		if(_hists_summary[i]!=NULL){
			TObject* old=gDirectory->GetList()->FindObject(Form("%s_%d",_hists_name_summary[i].c_str(),i));
                        gDirectory->GetList()->Remove(old);
			delete _hists_summary[i];
			_hists_summary[i]=NULL;
		}
		_hists_summary[i]=new TH1F(Form("%s_%d",_hists_name_summary[i].c_str(),i),_hists_name_summary[i].c_str(),10,0,10);
		_hists_summary[i]->SetBit(TH1::kCanRebin);
		_hists_summary[i]->SetStats(0);
		if(i==0){
			_hists_summary[i]->SetBarWidth(0.5);
			_hists_summary[i]->SetBarOffset(0.1);
		}	
		else{
		
			_hists_summary[i]->SetBarWidth(0.3);
			_hists_summary[i]->SetBarOffset(0.6);
		}
	}
	
	Int_t j=0,nevt1,nevt2;
	AMSChain ams1;
	if(n1_changed){
		ams1.Add(data1_filename.c_str());
		nevt1=ams1.GetEntries();
	}
	AMSChain ams2;
	if(n2_changed){
		ams2.Add(data2_filename.c_str());
		nevt2=ams2.GetEntries();
	}
	AMSEventR* evt;
	pbar->Show();
	cout<<"There are "<<nevt1<<" events in "<<data1_filename.c_str()<<endl;
	static string runid;
	time_t t=time(NULL);
	int k=0,l;
	if(n2_changed){
	for(i=0;i<nevt2;i++){
		evt=ams2.GetEvent(i);
		_hists_summary[1]->Fill("Number of Events",1);
				
		for(j=0;j<evt->nParticle();j++){
			_hists_2[0]->Fill(evt->pParticle(j)->Mass);
			_hists_2[1]->Fill(evt->pParticle(j)->Charge);
			_hists_2[2]->Fill(evt->pParticle(j)->Momentum);
			_hists_2[3]->Fill(evt->pParticle(j)->Particle);
			//0: NEvents
			//1: nParticles
			//2: iTrTrack
			//3: iTrdTrack
			//4: iTrdHTrack
			//5: iRichRing
			//6: iEcalShower
			//7: iTrTrack&iTrdTrack
			//8: iTrTrack&iRichRing
			//9: iTrTrack&iEcalShower	
			_hists_summary[1]->Fill("nParticles ",1);
			if(evt->pParticle(j)->iTrTrack()!=-1)
				_hists_summary[1]->Fill("iTrTrack",1);
			if(evt->pParticle(j)->iTrdTrack()!=-1)
                                _hists_summary[1]->Fill("iTrdTrack",1);
			if(evt->pParticle(j)->iTrdHTrack()!=-1)
                                _hists_summary[1]->Fill("iTrHTrack",1);
			if(evt->pParticle(j)->iRichRing()!=-1)
                                _hists_summary[1]->Fill("iRichRing",1);
			if(evt->pParticle(j)->iEcalShower()!=-1)
                                _hists_summary[1]->Fill("iEcalShower",1);
			if(evt->pParticle(j)->iTrTrack()!=-1&&evt->pParticle(j)->iTrdTrack()!=-1)
                                _hists_summary[1]->Fill("iTrTrack&iTrdTrack",1);
			if(evt->pParticle(j)->iTrTrack()!=-1&&evt->pParticle(j)->iRichRing()!=-1)                          
                                _hists_summary[1]->Fill("iTrTrack&iRichRing",1);
			if(evt->pParticle(j)->iTrTrack()!=-1&&evt->pParticle(j)->iEcalShower()!=-1)                          
                                _hists_summary[1]->Fill("iTrTrack&iEcalShower",1);
		}
		if(i%1000==0){
                        //pbar->SetPosition(110.0*(float)(nevt1+i)/(float)(nevt1+nevt2));
                        //pbar->ShowPosition();
                }
	}
	l=_hists_2[0]->GetEntries();
	/*
	for(k=1;k<_hists_2[0]->GetNbinsX();k++){
		temp=_hists_2[0]->GetBinContent(k);
		_hists_2[0]->SetBinContent(k,temp/(Double_t)l);			
	}
	*/
	 _hists_2[0]->Scale(1.0/(double)l);
	l=_hists_2[1]->GetEntries();
	/*
	for(k=1;k<_hists_2[1]->GetNbinsX();k++){
		temp=_hists_2[1]->GetBinContent(k);
		_hists_2[1]->SetBinContent(k,temp/(Double_t)l);			
	}
	*/
	_hists_2[1]->Scale(1.0/(double)l);
	l=_hists_2[2]->GetEntries();
	/*
	for(k=1;k<_hists_2[2]->GetNbinsX();k++){
		temp=_hists_2[2]->GetBinContent(k);
		_hists_2[2]->SetBinContent(k,temp/(Double_t)l);			
	}
	*/
	_hists_2[2]->Scale(1.0/(double)l);
	l=_hists_1[3]->GetEntries();
	/*
	for(k=1;k<_hists_2[3]->GetNbinsX();k++){
		temp=_hists_2[3]->GetBinContent(k);
		_hists_2[3]->SetBinContent(k,temp/(Double_t)l);			

	}*/
	_hists_2[3]->Scale(1.0/(double)l);
	_hists_summary[1]->SetTitle(Form("Reference Run: %d",evt->Run()));
	n2_changed=false;
	}
	if(n1_changed){
	for(i=0;i<nevt1;i++){
		evt=ams1.GetEvent(i);
		_hists_summary[0]->Fill("Number of Events",1);
		for(j=0;j<evt->nParticle();j++){
			_hists_1[0]->Fill(evt->pParticle(j)->Mass);
			_hists_1[1]->Fill(evt->pParticle(j)->Charge);
			_hists_1[2]->Fill(evt->pParticle(j)->Momentum);
			_hists_1[3]->Fill(evt->pParticle(j)->Particle);
			//0: NEvents
			//1: nParticles
			//2: iTrTrack
			//3: iTrdTrack
			//4: iTrdHTrack
			//5: iRichRing
			//6: iEcalShower
			//7: iTrTrack&iTrdTrack
			//8: iTrTrack&iRichRing
			//9: iTrTrack&iEcalShower	
			_hists_summary[0]->Fill("nParticles ",1);
			if(evt->pParticle(j)->iTrTrack()!=-1){
				_hists_summary[0]->Fill("iTrTrack",1);
			}
			if(evt->pParticle(j)->iTrdTrack()!=-1)
                                _hists_summary[0]->Fill("iTrdTrack",1);
			if(evt->pParticle(j)->iTrdHTrack()!=-1)
                                _hists_summary[0]->Fill("iTrHTrack",1);
			if(evt->pParticle(j)->iRichRing()!=-1)
                                _hists_summary[0]->Fill("iRichRing",1);
			if(evt->pParticle(j)->iEcalShower()!=-1)
                                _hists_summary[0]->Fill("iEcalShower",1);
			if(evt->pParticle(j)->iTrTrack()!=-1&&evt->pParticle(j)->iTrdTrack()!=-1)
                                _hists_summary[0]->Fill("iTrTrack&iTrdTrack",1);
			if(evt->pParticle(j)->iTrTrack()!=-1&&evt->pParticle(j)->iRichRing()!=-1)                          
                                _hists_summary[0]->Fill("iTrTrack&iRichRing",1);
			if(evt->pParticle(j)->iTrTrack()!=-1&&evt->pParticle(j)->iEcalShower()!=-1)                          
                                _hists_summary[0]->Fill("iTrTrack&iEcalShower",1);
		}
		
		if(i%1000==0){
			//pbar->SetPosition(110.0*(float)i/(float)(nevt1+nevt2));
			//pbar->ShowPosition();
		}
	}
	for(j=0;j<evt->nParticle();j++){
			_hists_1[0]->Fill(evt->pParticle(j)->Mass);
			_hists_1[1]->Fill(evt->pParticle(j)->Charge);
			_hists_1[2]->Fill(evt->pParticle(j)->Momentum);
			_hists_1[3]->Fill(evt->pParticle(j)->Particle);
	}
	
		l=_hists_1[0]->GetEntries();
		_hists_1[0]->Scale(1.0/(double)l);
		/*for(k=1;k<_hists_1[0]->GetNbinsX();k++){
			temp=_hists_1[0]->GetBinContent(k);
			_hists_1[0]->SetBinContent(k,temp/(Double_t)l);			
		}*/
		l=_hists_1[1]->GetEntries();
		/*for(k=1;k<_hists_1[1]->GetNbinsX();k++){
			temp=_hists_1[1]->GetBinContent(k);
			_hists_1[1]->SetBinContent(k,temp/(Double_t)l);			
		}*/
		_hists_1[1]->Scale(1.0/(double)l);
		l=_hists_1[2]->GetEntries();
		/*for(k=1;k<_hists_1[2]->GetNbinsX();k++){
			temp=_hists_1[2]->GetBinContent(k);
			_hists_1[2]->SetBinContent(k,temp/(Double_t)l);			
		}*/
		_hists_1[2]->Scale(1.0/(double)l);
		l=_hists_1[3]->GetEntries();
		/*for(k=1;k<_hists_1[3]->GetNbinsX();k++){
			temp=_hists_1[3]->GetBinContent(k);
			_hists_1[3]->SetBinContent(k,temp/(Double_t)l);			
		}*/
		_hists_1[3]->Scale(1.0/(double)l);
		n1_changed=false;
		_hists_summary[0]->SetTitle(Form("Run: %d",evt->Run()));
		for(k=0;k<_hists_h.size();k++){
			_hists_h[k]->Fill(Form("Run: %d;%s",evt->Run(),ctime(&t)),_hists_summary[0]->GetBinContent(k+1));
		}
		}
		else{
			for(k=0;k<_hists_h.size();k++){
				_hists_h[k]->Fill(Form("No new file;%s",ctime(&t)),1);
		}
	}
	cout<<"+++++++++++++++++++nref="<<nref<<endl;
	if(nref){
		temp2=_hists_summary[0]->GetBinContent(1);
		//cout<<"temp2="<<temp2<<endl;
		temp2/=_hists_summary[1]->GetBinContent(1);	
		//cout<<"temp2="<<temp2<<endl;	
		/*for(i=1;i<=10;i++){
			temp=_hists_summary[1]->GetBinContent(i);
			_hists_summary[1]->SetBinContent(i,temp2*temp);
			//cout<<"i="<<i<<","<<_hists_summary[1]->GetBinContent(i)<<","<<_hists_summary[0]->GetBinContent(i)<<endl;
			
		}*/
		_hists_summary[1]->Scale(temp2);
	}
}
