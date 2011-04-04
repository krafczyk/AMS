#include "Data.h"
extern ProgBar* pbar;
Data::Data(vector<string> hists_name,vector<string> hists_name_summary):_hists_name(hists_name),data1_filename("NULL"),data2_filename("NULL"),_hists_name_summary(hists_name_summary){
	Int_t i=0;
	nhist=hists_name.size();
	_hists_1.resize(nhist);
	_hists_2.resize(nhist);
	i=hists_name_summary.size();
	_hists_summary.resize(i);//hists_name_summary.size())
	for(i=0;i<nhist;i++){
		_hists_1[i]=NULL;
		_hists_2[i]=NULL;
	}
	for(i=0;i<hists_name_summary.size();i++){
		_hists_summary[i]=NULL;
	}
	_hists_h=new TH1F("_hists_h","History Record: nEvents",5,0,5);
	_hists_h->SetBit(TH1::kCanRebin);
        _hists_h->SetStats(0);
        _hists_h->SetBarWidth(0.6);
        _hists_h->SetBarOffset(0.1);
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
	Int_t i=0;
	for(i=0;i<nhist;i++){
		if(_hists_1[i]!=NULL){
			TObject* old=gDirectory->GetList()->FindObject(Form("%s_1",_hists_name[i].c_str()));
			gDirectory->GetList()->Remove(old);
			delete _hists_1[i];
			_hists_1[i]=NULL;
		}
		_hists_1[i]=new TH1F(Form("%s_1",_hists_name[i].c_str()),_hists_name[i].c_str(),100,-100,100);
		if(_hists_2[i]!=NULL){
			TObject* old=gDirectory->GetList()->FindObject(Form("%s_2",_hists_name[i].c_str()));
                        gDirectory->GetList()->Remove(old);
			delete _hists_2[i];
			_hists_2[i]=NULL;
		}
		_hists_2[i]=new TH1F(Form("%s_2",_hists_name[i].c_str()),_hists_name[i].c_str(),100,-100,100);
	}
	for(i=0;i<_hists_summary.size();i++){
		if(_hists_summary[i]!=NULL){
			TObject* old=gDirectory->GetList()->FindObject(Form("%s",_hists_name_summary[i].c_str()));
                        gDirectory->GetList()->Remove(old);
			delete _hists_summary[i];
			_hists_summary[i]=NULL;
		}
		_hists_summary[i]=new TH1F(Form("%s",_hists_name_summary[i].c_str()),_hists_name_summary[i].c_str(),10,0,10);
		_hists_summary[i]->SetBit(TH1::kCanRebin);
		_hists_summary[i]->SetStats(0);
		_hists_summary[i]->SetBarWidth(0.6);
		_hists_summary[i]->SetBarOffset(0.1);
	}
	AMSChain ams1;
	AMSChain ams2;
	ams1.Add(data1_filename.c_str());
	ams2.Add(data2_filename.c_str());
	AMSEventR* evt;
	pbar->Show();
	Int_t j=0,nevt1,nevt2;
	nevt1=ams1.GetEntries();
	nevt2=ams2.GetEntries();
	cout<<"There are "<<nevt1<<" events in "<<data1_filename.c_str()<<endl;
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
			if(evt->pParticle(j)->iTrTrack()!=-1)
				_hists_summary[0]->Fill("iTrTrack",1);
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
			pbar->SetPosition(110.0*(float)i/(float)(nevt1+nevt2));
			pbar->ShowPosition();
		}
	}
	time_t t=time(NULL);
	_hists_h->Fill(ctime(&t),_hists_summary[0]->GetBinContent(1));
	for(i=0;i<nevt2;i++){
		evt=ams2.GetEvent(i);
		for(j=0;j<evt->nParticle();j++){
			_hists_2[0]->Fill(evt->pParticle(j)->Mass);
			_hists_2[1]->Fill(evt->pParticle(j)->Charge);
			_hists_2[2]->Fill(evt->pParticle(j)->Momentum);
			_hists_2[3]->Fill(evt->pParticle(j)->Particle);
		}
		if(i%1000==0){
                        pbar->SetPosition(110.0*(float)(nevt1+i)/(float)(nevt1+nevt2));
                        pbar->ShowPosition();
                }
	}
}
