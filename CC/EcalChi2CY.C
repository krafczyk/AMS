#include "EcalChi2CY.h"
#define SIZE  0.9

ClassImp(EcalAxis);

EcalPDF::EcalPDF(char* fdatabase){
	has_init=init(fdatabase);
}
EcalPDF::EcalPDF(){
	char* amsdatadir=getenv("AMSDataDir");
	char tempname[100];
	if(amsdatadir){
		sprintf(tempname,"%s/v5.00/EcalChi2CY_prof.20120814.1.root",amsdatadir);
		has_init=init(tempname);
	}
	else{
		has_init=init("/afs/cern.ch/ams/Offline/AMSDataDir/v5.00/EcalChi2CY_prof.20120814.1.root");
	}
}
bool EcalPDF::init(char* fdatabase){
	bool ret=true;
	char tempname[100];
	for(int layer=0;layer<18;layer++){
		for(int i1=0;i1<6;i1++){
			sprintf(tempname,"hrms_lay%.2d_%d_2",layer,i1);
			param_rms_lf[layer][i1] =new TH1F(tempname,tempname,710, -31.905, 31.995) ;
			sprintf(tempname,"hmean_lay%.2d_%d_2",layer,i1);
			param_mean_lf[layer][i1]=new TH1F(tempname,tempname,710, -31.905, 31.995);
		}
		for(int i1=0;i1<2;i1++){
			sprintf(tempname,"hprob_lay%.2d_%d_2",layer,i1);
			param_prob_lf[layer][i1]=new TH1F(tempname,tempname,710, -31.905, 31.995);
		}
	}
	TH1F* hdummy;
	TFile* _fdatabase=TFile::Open(fdatabase);
	
	if(fdatabase==NULL){
        	cout<<"Error happens when loading ECALChi2CY database file : "<<fdatabase<<endl;
        	ret=false;
	}
        else{
                for(int layer=0;layer<18;layer++){
                        for(int i1=0;i1<6;i1++){
				sprintf(tempname,"param_rms_%d_lay%.2d",i1,layer);
				hdummy=(TH1F*)_fdatabase->Get(tempname);
                                if(hdummy==NULL){
                                        cout<<"Can not find ECALChi2CY hist "<<tempname<<" in "<<fdatabase<<endl;
                                        has_init=false;
                                }
				else{
					for(int i2=1;i2<=hdummy->GetXaxis()->GetNbins();i2++)
						param_rms_lf[layer][i1]->SetBinContent(i2,hdummy->GetBinContent(i2));
				}
				hdummy=NULL;
				sprintf(tempname,"param_mean_%d_lay%.2d",i1,layer);
				hdummy=(TH1F*)_fdatabase->Get(tempname);
				if(hdummy==NULL){
                                        cout<<"Can not find ECALChi2CY hist "<<tempname<<" in "<<fdatabase<<endl;
                                        has_init=false;
				}
				else{
        	                        for(int i2=1;i2<=hdummy->GetXaxis()->GetNbins();i2++)
                	                        param_mean_lf[layer][i1]->SetBinContent(i2,hdummy->GetBinContent(i2));				
				}
				hdummy=NULL;
			}
                }
		
		for(int layer=0;layer<18;layer++){
                        for(int i1=0;i1<2;i1++){
				sprintf(tempname,"param_prob_%d_lay%.2d",i1,layer);
                		hdummy=(TH1F*)_fdatabase->Get(tempname);
                                if(hdummy==NULL){
                                        cout<<"Can not find ECALChi2CY hist "<<tempname<<" in "<<fdatabase<<endl;
                                        has_init=false;
                                }
                                else{
                                        for(int i2=1;i2<=hdummy->GetXaxis()->GetNbins();i2++)
                                                param_prob_lf[layer][i1]->SetBinContent(i2,hdummy->GetBinContent(i2));
                                }
				hdummy=NULL;
			}
                }
	}
	cout<<"EcalPDF init OK! DataBase file is "<<fdatabase<<endl;
	_fdatabase->Close();
	return ret;
}
EcalPDF::~EcalPDF(){
	for(int layer=0;layer<18;layer++){
                for(int i1=0;i1<6;i1++){
			if(param_rms_lf[layer][i1]!=NULL){
				delete param_rms_lf[layer][i1];
				param_rms_lf[layer][i1]=NULL;
			}
			if(param_mean_lf[layer][i1]!=NULL){
				delete param_mean_lf[layer][i1];
				param_mean_lf[layer][i1]=NULL;
			}	
		}
		for(int i1=0;i1<2;i1++){
			if(param_prob_lf[layer][i1]!=NULL){	
				delete param_prob_lf[layer][i1];
				param_prob_lf[layer][i1]=NULL;
			}
		}
	}
}
float EcalPDF::get_mean(int flayer,float coo,float Erg,int type){
	if(!has_init){
		cout<<"EcalPDF has not been loaded!"<<endl;
		return -1;
	}
	if(flayer<0||flayer>17)
		return -1;
	float ret,ret0,ret1 ;
	int binx0,binx1     ;
	float x,x0,x1       ;
	float param_lf[6]   ;
	switch(type){
		case 0:
			x=log(Erg);
                        binx0=param_mean_lf[flayer][0]->FindBin(coo);
                        x0=param_mean_lf[flayer][0]->GetBinCenter(binx0);
                        if(coo<x0){
                                x1=param_mean_lf[flayer][0]->GetBinCenter(binx0-1);
                                binx1=binx0-1;
                                for(int i2=0;i2<6;i2++){
                                        param_lf[i2]=param_mean_lf[flayer][i2]->GetBinContent(binx0);
                                }
                                ret0=myfunc_lf(x,param_lf,0);
                                for(int i2=0;i2<6;i2++){
                                        param_lf[i2]=param_mean_lf[flayer][i2]->GetBinContent(binx1);
                                }
                                ret1=myfunc_lf(x,param_lf,0);
                                ret=ret0*(coo-x1)/(x0-x1)+ret1*(x0-coo)/(x0-x1);
                        }
                        else{
                                x1=param_mean_lf[flayer][0]->GetBinCenter(binx0+1);
                                binx1=binx0+1;
                                for(int i2=0;i2<6;i2++){
                                        param_lf[i2]=param_mean_lf[flayer][i2]->GetBinContent(binx0);
                                }
                                ret0=myfunc_lf(x,param_lf,0);
                                for(int i2=0;i2<6;i2++){
                                        param_lf[i2]=param_mean_lf[flayer][i2]->GetBinContent(binx1);
                                }
                                ret1=myfunc_lf(x,param_lf,0);
                                ret=ret0*(x1-coo)/(x1-x0)+ret1*(coo-x0)/(x1-x0);
                        }
			break;
	}
	return ret;
}

float EcalPDF::get_rms (int flayer,float coo,float Erg,int type){
	if(!has_init){
		cout<<"EcalPDF has not been loaded!"<<endl;
		return -1;
	}
	float ret,ret0,ret1 ;
        int binx0,binx1     ;
        float x,x0,x1       ;
        float param_lf[6]   ;
	if(flayer<0||flayer>17)
		return -1;
	switch(type){
		case 0:
			x=log(Erg);
                        binx0=param_rms_lf[flayer][0]->FindBin(coo);
                        x0=param_rms_lf[flayer][0]->GetBinCenter(binx0);
                        if(coo<x0){
                                x1=param_rms_lf[flayer][0]->GetBinCenter(binx0-1);
                                binx1=binx0-1;
                                for(int i2=0;i2<6;i2++){
                                        param_lf[i2]=param_rms_lf[flayer][i2]->GetBinContent(binx0);
                                }
                                ret0=myfunc_lf(x,param_lf,0);
                                for(int i2=0;i2<6;i2++){
                                        param_lf[i2]=param_rms_lf[flayer][i2]->GetBinContent(binx1);
                                }
                                ret1=myfunc_lf(x,param_lf,0);
                                ret=ret0*(coo-x1)/(x0-x1)+ret1*(x0-coo)/(x0-x1);
                        }
                        else{
                                x1=param_rms_lf[flayer][0]->GetBinCenter(binx0+1);
                                binx1=binx0+1;
                                for(int i2=0;i2<6;i2++){
                                        param_lf[i2]=param_rms_lf[flayer][i2]->GetBinContent(binx0);
                                }
                                ret0=myfunc_lf(x,param_lf,0);
                                for(int i2=0;i2<6;i2++){
                                        param_lf[i2]=param_rms_lf[flayer][i2]->GetBinContent(binx1);
                                }
                                ret1=myfunc_lf(x,param_lf,0);
                                ret=ret0*(x1-coo)/(x1-x0)+ret1*(coo-x0)/(x1-x0);
                        }
			break;
	}
	return ret;
}

float EcalPDF::get_prob(int flayer,float coo,float Erg,int type){
	if(!has_init){
		cout<<"EcalPDF has not been loaded!"<<endl;
		return -1;
	}
	if(flayer<0||flayer>17)
		return -1;
	float ret,ret0,ret1 ;
        int binx0,binx1     ;
        float x,x0,x1       ;
        float param_lf[2]   ;
	switch(type){
		case 0:
			x=log(Erg);
                        binx0=param_prob_lf[flayer][0]->FindBin(coo);
                        x0=param_prob_lf[flayer][0]->GetBinCenter(binx0);
                        if(coo<x0){
                                x1=param_prob_lf[flayer][0]->GetBinCenter(binx0-1);
                                binx1=binx0-1;
                                for(int i2=0;i2<2;i2++){
                                        param_lf[i2]=param_prob_lf[flayer][i2]->GetBinContent(binx0);
                                }
                                ret0=myfunc_lf(x,param_lf,1);
                                for(int i2=0;i2<2;i2++){
                                        param_lf[i2]=param_prob_lf[flayer][i2]->GetBinContent(binx1);
                                }
                                ret1=myfunc_lf(x,param_lf,1);
                                ret=ret0*(coo-x1)/(x0-x1)+ret1*(x0-coo)/(x0-x1);
                        }
                        else{
                                x1=param_prob_lf[flayer][0]->GetBinCenter(binx0+1);
                                binx1=binx0+1;
                                for(int i2=0;i2<2;i2++){
                                        param_lf[i2]=param_prob_lf[flayer][i2]->GetBinContent(binx0);
                                }
                                ret0=myfunc_lf(x,param_lf,1);
                                for(int i2=0;i2<2;i2++){
                                        param_lf[i2]=param_prob_lf[flayer][i2]->GetBinContent(binx1);
                                }
                                ret1=myfunc_lf(x,param_lf,1);
                                ret=ret0*(x1-coo)/(x1-x0)+ret1*(coo-x0)/(x1-x0);
                        }
			break;
		case 1:
			x=log(Erg);
                        binx0=param_prob_lf[flayer][0]->FindBin(coo);
                        x0=param_prob_lf[flayer][0]->GetBinCenter(binx0);
                        if(coo<x0){
                                x1=param_prob_lf[flayer][0]->GetBinCenter(binx0-1);
                                binx1=binx0-1;
                                for(int i2=0;i2<2;i2++){
                                        param_lf[i2]=param_prob_lf[flayer][i2]->GetBinContent(binx0);
                                }
                                ret0=myfunc_lf(x,param_lf,2);
                                for(int i2=0;i2<2;i2++){
                                        param_lf[i2]=param_prob_lf[flayer][i2]->GetBinContent(binx1);
                                }
                                ret1=myfunc_lf(x,param_lf,2);
                                ret=ret0*(coo-x1)/(x0-x1)+ret1*(x0-coo)/(x0-x1);
                        }
                        else{
                                x1=param_prob_lf[flayer][0]->GetBinCenter(binx0+1);
                                binx1=binx0+1;
                                for(int i2=0;i2<2;i2++){
                                        param_lf[i2]=param_prob_lf[flayer][i2]->GetBinContent(binx0);
                                }
                                ret0=myfunc_lf(x,param_lf,2);
                                for(int i2=0;i2<2;i2++){
                                        param_lf[i2]=param_prob_lf[flayer][i2]->GetBinContent(binx1);
                                }
                                ret1=myfunc_lf(x,param_lf,2);
                                ret=ret0*(x1-coo)/(x1-x0)+ret1*(coo-x0)/(x1-x0);
                        }
                        break;	
	}
	return ret;
}
double EcalPDF::myfunc_lf(float x,float* par,int type){
        double ret;
        if(x<=0)
                return -1;
        if(x<log(5))
                x=log(5);
        if(x>log(600))
                x=log(600);
        if(type==0){
                double a0,a1,a2,a3,a4,a5;
                a0=par[0];a1=par[1];a2=par[2];
                a3=par[3];a4=par[4];a5=par[5];
                ret=a0+a1*x+a2*x*x+a3*x*x*x+a4*x*x*x*x+a5*x*x*x*x*x;
                if(ret>1)
                        ret=1.0;
                else if(ret<0.0)
                        ret=0.0;
        }
        else if(type==1){
                double a0,a1,x0;
                a0=par[0];
                a1=par[1];
                if(a1==0)
	                x0=1.5;
        	else
                	x0=(log(11.)-a0)/a1;
		ret=11.0;
                if(x<x0){
                        if(a0+a1*x>10)
                                ret=11.0;
                        else if(a0+a1*x<-10)
                                ret=0.0;
                        else
                                ret=exp(a0+a1*x);
                }
                if(ret>=11.0)
                        ret=1.0;
                else
                        ret=1.0-exp(-ret);

        }
        else if (type==2){
                double a0,a1,x0;
                a0=par[0];
                a1=par[1];
                if(a1==0)
	                x0=1.5;
        	else
                	x0=(log(11.)-a0)/a1;
		ret=11.0;
                if(x<x0){
                        if(a0+a1*x>10)
                                ret=11.0;
                        else if(a0+a1*x<-10)
                                ret=0.0;
                        else
                                ret=exp(a0+a1*x);
                }
		if(ret>=11.0)
                        ret=11.0;		
        }
        return ret;
}

//End of Ecal PDF Class
//////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

/////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
//Class Ecal Chi2
/////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
EcalChi2::EcalChi2(int ftype){
	char* amsdatadir=getenv("AMSDataDir");
	char tempname[100];
        if(amsdatadir){
		sprintf(tempname,"%s/v5.00/EcalChi2CY_prof.20120814.1.root",amsdatadir);
                init(tempname,ftype);
        }
        else{
                init("/afs/cern.ch/ams/Offline/AMSDataDir/v5.00/EcalChi2CY_prof.20120814.1.root",ftype);
        }		
}
EcalChi2::EcalChi2(char* fdatabase,int ftype){
	init(fdatabase,ftype);
}

EcalChi2::~EcalChi2(){
	if(ecalpdf){
		delete ecalpdf;
		ecalpdf=NULL;
	} 
}
void EcalChi2::init(char* database,int _type){
	ecalpdf=new EcalPDF(database);
	float _shiftxy_iss[]=   {0.049183,0.0547541,-0.127118,-0.129285,0.0567233,0.0589902,-0.125686,-0.12495,0.0588771,0.0580914,-0.13321,-0.135947,0.0553363,0.0543928,-0.132608,-0.130619,0.0509601,0.0504522};
        float _shiftxy_bt []=   {0.0649878,  0.0674102,    -0.18013,    -0.176749,   0.0684572,   0.0710825,   -0.175532,   -0.173065,   0.0631457,   0.0679332,   -0.172697,   -0.175777,   0.0636235,   0.0681259,  -0.166372,  -0.166112,  0.0657156,   0.0491425};
        float _shiftxy_sim[]=   {-0.036077,-0.0347022,-0.0483337,-0.0491348,0.0543024,0.0555286,-0.0786445,-0.0791036,0.00598481,0.00624232,-0.0977129,-0.0980029,0.0924151,0.0922796,-0.265521,-0.264891,0.131682,0.131873};
        float _ecalz[]      =   {-142.792,-143.658,-144.643,-145.508,-146.493,-147.357,-148.342,-149.208,-150.193,-151.058,-152.042,-152.908,-153.893,-154.758,-155.743,-156.607,-157.592,-158.458};
        float _ecalz_new[]  =   {-143.195,-144.115,-145.045,-145.965,-146.895,-147.815,-148.745,-149.665,-150.595,-151.515,-152.445,-153.365,-154.295,-155.215,-156.145,-157.065,-157.995,-158.915};
        float _shiftz_iss[] =   {-0.796973,-0.72069,-0.613334,-0.590829,-0.596496,-0.575594,-0.468351,-0.445498,-0.466901,-0.452407,-0.351328,-0.342198,-0.289828,-0.297303,-0.178367,-0.171907,-0.132327,-0.0401191};
        float _shiftz_sim[] =   {-0.399355,-0.275355,-0.207417,-0.163975,-0.140999,-0.103207,-0.0851771,-0.063238,-0.0402442,-0.02793,-0.00142254,0.0105943,0.0404541,0.0559551,0.0963537,0.122534,0.154541,0.22572 };
	
	for(int i1=0;i1<18;i1++){
          //ISS
          if(_type==0){
	    shiftxy[i1]=-31.95-_shiftxy_iss[i1];
	    ecalz0[i1]=_ecalz[i1];
	    shiftz[i1]=_shiftz_iss[i1];
          }
          //Test Beam
          if(_type==1){
            shiftxy[i1]=-31.95-_shiftxy_bt [i1];
            ecalz0[i1]=_ecalz_new[i1];
            shiftz[i1]=0.        ;
          }
          //Simulation
          if(_type==2){
            shiftxy[i1]=-31.95-_shiftxy_sim[i1];
            ecalz0[i1]=_ecalz[i1];
            shiftz[i1]=_shiftz_sim[i1];
          }
        }
	for(int i1=0;i1<18;i1++)
	  ecalz[i1]=ecalz0[i1]+shiftz[i1];

	for(int i1=0;i1<18;i1++){
		for(int i2=0;i2<72;i2++){
			fdead_cell[i1][i2]=0;
		}
	}
	//ISS
	if(_type==0){
		fdead_cell[14][65]=1;
                fdead_cell[6 ][16]=1;
                fdead_cell[6 ][17]=1;
                fdead_cell[7 ][16]=1;
                fdead_cell[7 ][17]=1;	
                fdead_cell[6 ][50]=1;
	}
	//Test Beam
	
	//Simualtion
	if(_type==0){
                fdead_cell[14][65]=1;
	}
	
}
float EcalChi2::process(float* coo,float sign){
	if(coo==NULL)
		return -1;
	for(int i1=0;i1<18;i1++){
		pos[i1]=coo[i1];
	}
	float chi2,chi22,chi23,chi24;
        int   ndof;
        for(int i1=0;i1<18;i1++){
                int bin=(int)((coo[i1]-shiftxy[i1]+0.45)/SIZE);
                ndof             =cal_chi2(bin-20,bin+20,i1,coo[i1],chi2,chi22,chi23,chi24,sign);
                _ndofs[i1]       =ndof;
                _chi2_layer [i1] =chi2;
                _chi22_layer[i1] =chi22;
                _chi23_layer[i1] =chi23;
                _chi24_layer[i1] =chi24;
        }
        _chi2    =0.;
        _chi2x   =0.;
        _chi2y   =0.;
        _variance=0.;
        _skewness=0.;
        _kurtosis=0.;
        tot_ndof =0;
        tot_ndofx=0;
        tot_ndofy=0;
        for(int i1=1;i1<18;i1++){
                _chi2     +=_chi2_layer [i1];
                if(i1%4<2){
                        _chi2y    +=_chi2_layer [i1];
                        tot_ndofy +=_ndofs[i1];
                }
                else{
                        _chi2x    +=_chi2_layer [i1];
                        tot_ndofx +=_ndofs[i1];
                }
                tot_ndof  +=_ndofs[i1];
                _variance +=_chi22_layer[i1];
                _skewness +=_chi23_layer[i1];
                _kurtosis +=_chi24_layer[i1];
        }
        if(tot_ndof>0){
                _chi2  /=tot_ndof;
                if(tot_ndofy>0)
                        _chi2y/=tot_ndofy;
                else
                        _chi2y=50.;
                if(tot_ndofx>0)
                        _chi2x/=tot_ndofx;
                else
                        _chi2x=50.;
                _variance/=tot_ndof;
                _skewness/=tot_ndof;
                _kurtosis/=tot_ndof;

                _variance=sqrt(_variance-_chi2*_chi2);
                _skewness=(_skewness-3.*_chi2*_variance*_variance-_chi2*_chi2*_chi2)/(_variance*_variance*_variance);
                _kurtosis/=(_variance*_variance*_variance*_variance);
        }
	else{
                _chi2         =50.;
                _chi2x        =50.;
                _chi2y        =50.;
                _variance     =-1;
                _skewness     =-1;
                _kurtosis     =-1;
        }
	cal_f2dep();
        return _chi2;
}
int   EcalChi2::set_edep(float edeps[18][72], float erg){
	for(int i1=0;i1<18;i1++){
                for(int i2=0;i2<72;i2++){
                        Edep_raw[i1][i2]=edeps[i1][i2];
                }
        }
        _erg=erg;
	return 0;
}
float EcalChi2::process(TrTrackR*  trtrack, EcalShowerR* esh, int iTrTrackPar){
#ifdef _PGTRACK_
	float sign=-1.;
	if(iTrTrackPar<0){
		cout<<"Error : iTrTrackPar= "<<iTrTrackPar<<endl;
		return -1;
	}
	if(trtrack==NULL||esh==NULL){
		cout<<"Ecal Chi2  Error: trtrack= "<<trtrack<<" esh = "<<esh<<endl;
		return -1;
	}
	AMSPoint pnt;
	AMSDir	 dir;
	if(trtrack->GetRigidity(iTrTrackPar)>0)
		sign=1.0;
	for(int i1=0;i1<18;i1++){
		trtrack->Interpolate(ecalz[i1],pnt,dir,iTrTrackPar);	
		if(i1%4<2)
			pos[i1]=pnt.y();
		else
			pos[i1]=pnt.x();
	}
	for(int i1=0;i1<18;i1++){
		for(int i2=0;i2<72;i2++){
			Edep_raw[i1][i2]=0.;
		}
	}
	_erg=esh->EnergyD/1000.;
	for(int i1=0;i1<esh->NEcal2DCluster();i1++){
		Ecal2DClusterR * ecal2d=esh->pEcal2DCluster(i1);
                for(int i2=0;i2<ecal2d->NEcalCluster();i2++){
			EcalClusterR* ecalclt=ecal2d->pEcalCluster(i2);
                        for(int i3=0;i3<ecalclt->NEcalHit();i3++){
				Edep_raw[ecalclt->pEcalHit(i3)->Plane][ecalclt->pEcalHit(i3)->Cell]=ecalclt->pEcalHit(i3)->Edep;
			}
		}
	}

	float chi2,chi22,chi23,chi24;
	int   ndof;
	for(int i1=0;i1<18;i1++){
		int bin=(int)((pos[i1]-shiftxy[i1]+0.45)/SIZE);
		ndof             =cal_chi2(bin-20,bin+20,i1,pos[i1],chi2,chi22,chi23,chi24,sign);
		_ndofs[i1]       =ndof;
		_chi2_layer [i1] =chi2;
		_chi22_layer[i1] =chi22;
		_chi23_layer[i1] =chi23;
		_chi24_layer[i1] =chi24;
	}
	_chi2    =0.;
	_chi2x   =0.;
	_chi2y   =0.;
	_variance=0.;
	_skewness=0.;
	_kurtosis=0.;
	tot_ndof =0;
	tot_ndofx=0;
	tot_ndofy=0;
	for(int i1=1;i1<18;i1++){
		_chi2     +=_chi2_layer [i1];
		if(i1%4<2){
			_chi2y    +=_chi2_layer [i1];
			tot_ndofy +=_ndofs[i1];
		}
		else{
			_chi2x    +=_chi2_layer [i1];
			tot_ndofx +=_ndofs[i1];
		}
		tot_ndof  +=_ndofs[i1];
		_variance +=_chi22_layer[i1];
		_skewness +=_chi23_layer[i1];
		_kurtosis +=_chi24_layer[i1]; 
	}	
	if(tot_ndof>0){
		_chi2  /=tot_ndof;
		if(tot_ndofy>0)
			_chi2y/=tot_ndofy;
		else
			_chi2y=50.;
		if(tot_ndofx>0)
                        _chi2x/=tot_ndofx;
                else
                        _chi2x=50.;
		_variance/=tot_ndof;
		_skewness/=tot_ndof;
		_kurtosis/=tot_ndof;
	
		_variance=sqrt(_variance-_chi2*_chi2);
		_skewness=(_skewness-3.*_chi2*_variance*_variance-_chi2*_chi2*_chi2)/(_variance*_variance*_variance);
		_kurtosis/=(_variance*_variance*_variance*_variance);
	}
	else{
		_chi2         =50.;
		_chi2x        =50.;
		_chi2y        =50.;
		_variance     =-1;
		_skewness     =-1;
		_kurtosis     =-1;
	}
	cal_f2dep();
	
	return _chi2;
#else
return -1;
#endif

}
float EcalChi2::cal_f2dep(){
        _f2dep=0.;
        for(int i1=0;i1<72;i1++){
                _f2dep+=Edep_raw[0][i1];
        }
        for(int i1=0;i1<72;i1++){
                _f2dep+=Edep_raw[1][i1];
        }
        return _f2dep;
}

int EcalChi2::cal_chi2(int start_cell,int end_cell,int layer,double coo,float& chi2,float& chi22,float& chi23,float& chi24,float sign){
	bool flag;
	int i1=0;
        double cell_mean[72],cell_rms[72], cell_prob[72],cell_probbar[72];
        double summ=0.,sumxm=0.;
	double chisq_prob,chisq_chi2,delta,chisq;
        int k1,k2;
        int count=0;
        if(start_cell<0)
                start_cell=0;
	if(start_cell>71)
		start_cell=71;
	if(end_cell<0)
		end_cell=0;
        if(end_cell>71)
                end_cell=71;
        k1=(int)((coo-shiftxy[layer]+0.45)/SIZE);
	k2=k1;
	flag=false;
	if(k1<start_cell)
		k1=start_cell;
	else{
 	for(;k1>=start_cell;k1--){
		if(k1>0&&k1<72){
			if(Edep_raw[layer][k1-1]==0.0){
				if(k1>1){
					if(Edep_raw[layer][k1-2]==0.0)
						flag=true;
				}
			}
		}
                if(flag)
			break;
        }
	}
	flag=false;
	if(k2>end_cell)
		k2=end_cell;
	else{
        for(;k2<=end_cell;k2++){
		if(k2<71&&k2>-1){
                        if(Edep_raw[layer][k2+1]==0.0){
                                if(k2<70){
                                        if(Edep_raw[layer][k2+2]==0.0)
                                                flag=true;
                                }
                        }
                }
                if(flag)
                        break;
        }
	}
        start_cell=k1;
        end_cell=k2;
	
        for(i1=start_cell;i1<=end_cell;i1++){
                cell_mean[i1]=ecalpdf->get_mean(layer,-1.*sign*(i1*SIZE+shiftxy[layer]-coo),_erg,0);
                cell_rms[i1]=ecalpdf->get_rms(layer,-1.*sign*(i1*SIZE+shiftxy[layer]-coo),_erg,0);
                cell_prob[i1]=ecalpdf->get_prob(layer,-1.*sign*(i1*SIZE+shiftxy[layer]-coo),_erg,0);
                cell_probbar[i1]=ecalpdf->get_prob(layer,-1.*sign*(i1*SIZE+shiftxy[layer]-coo),_erg,1);
		if(fdead_cell[layer][i1]==0&&Edep_raw[layer][i1]>0){
                        summ+=cell_mean[i1];
                        sumxm+=Edep_raw[layer][i1];
                        k1++;
                }
        }
        for(i1=start_cell;i1<=end_cell;i1++){
                if(cell_rms[i1]<=0.0){
                        cell_rms[i1]+=ecalpdf->get_rms(layer,-1.*sign*(i1*SIZE+shiftxy[layer]-coo-0.18),_erg,0);
                        cell_rms[i1]+=ecalpdf->get_rms(layer,-1.*sign*(i1*SIZE+shiftxy[layer]-coo-0.09),_erg,0);
                        cell_rms[i1]+=ecalpdf->get_rms(layer,-1.*sign*(i1*SIZE+shiftxy[layer]-coo+0.09),_erg,0);
                        cell_rms[i1]+=ecalpdf->get_rms(layer,-1.*sign*(i1*SIZE+shiftxy[layer]-coo+0.18),_erg,0);
                        cell_rms[i1]/=4.0;
                }
        }
	count =0  ;
	chi2  =0. ;
	chi22 =0. ;
	chi23 =0. ;
	chi24 =0. ;
	for(i1=start_cell;i1<=end_cell;i1++){
                if(fdead_cell[layer][i1]==1)
                        continue;
                count++;
                chisq_prob=0.;
                chisq_chi2=0.;
                if(Edep_raw[layer][i1]<=0){
                        chisq_prob=cell_probbar[i1];
                }
                else{
			delta=(cell_mean[i1]-Edep_raw[layer][i1]*summ/sumxm);
			chisq_chi2=delta*delta/2.0/cell_rms[i1] ;
			chisq_prob=-1.*log(cell_prob[i1])	;
		}
		if(!(chisq_prob>=0.&&chisq_prob<50.))
			chisq_prob=50.;
		if(!(chisq_chi2>=0.&&chisq_chi2<50.))
			chisq_chi2=50.;
		//cout<<"+"<<cell_mean[i1]<<","<<Edep_raw[layer][i1]*summ/sumxm<<","<<cell_rms[i1]<<"; "<<cell_probbar[i1]<<", "<<-1.*log(cell_prob[i1])<<endl;
		chisq =chisq_chi2+chisq_prob;
		if(chisq>50.)
			chisq=50.;
		chi2 +=chisq;
		chi22+=chisq*chisq;
		chi23+=chisq*chisq*chisq;
		chi24+=chisq*chisq*chisq*chisq;
	}
	return count;	
}

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
//End of Ecal Chi2
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
//Class Ecal Axis
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

TVirtualFitter* EcalAxis::gMinuit_EcalAxis = NULL;
EcalAxis::EcalAxis(int ftype){
	char* amsdatadir=getenv("AMSDataDir");
	char tempname[100];
        if(amsdatadir){
		sprintf(tempname,"%s/v5.00/EcalChi2CY_prof.20120814.1.root",amsdatadir);
                init(tempname,ftype);
        }
        else{
                init("/afs/cern.ch/ams/Offline/AMSDataDir/v5.00/EcalChi2CY_prof.20120814.1.root",ftype);
        }
}
EcalAxis::EcalAxis(char* fdatabase, int ftype){
	init(fdatabase,ftype);
}
void EcalAxis::init(char* fdatabase, int ftype){
	ecalchi2=new EcalChi2(fdatabase,ftype);
	float _shiftxy_iss[]=   {0.049183,0.0547541,-0.127118,-0.129285,0.0567233,0.0589902,-0.125686,-0.12495,0.0588771,0.0580914,-0.13321,-0.135947,0.0553363,0.0543928,-0.132608,-0.130619,0.0509601,0.0504522};
        float _shiftxy_bt []=   {0.0649878,  0.0674102,    -0.18013,    -0.176749,   0.0684572,   0.0710825,   -0.175532,   -0.173065,   0.0631457,   0.0679332,   -0.172697,   -0.175777,   0.0636235,   0.0681259,  -0.166372,  -0.166112,  0.0657156,   0.0491425};
        float _shiftxy_sim[]=   {-0.036077,-0.0347022,-0.0483337,-0.0491348,0.0543024,0.0555286,-0.0786445,-0.0791036,0.00598481,0.00624232,-0.0977129,-0.0980029,0.0924151,0.0922796,-0.265521,-0.264891,0.131682,0.131873};
        float _ecalz[]      =   {-142.792,-143.658,-144.643,-145.508,-146.493,-147.357,-148.342,-149.208,-150.193,-151.058,-152.042,-152.908,-153.893,-154.758,-155.743,-156.607,-157.592,-158.458};
        float _ecalz_new[]  =   {-143.195,-144.115,-145.045,-145.965,-146.895,-147.815,-148.745,-149.665,-150.595,-151.515,-152.445,-153.365,-154.295,-155.215,-156.145,-157.065,-157.995,-158.915};
	float _shiftz_iss[] =   {-0.796973,-0.72069,-0.613334,-0.590829,-0.596496,-0.575594,-0.468351,-0.445498,-0.466901,-0.452407,-0.351328,-0.342198,-0.289828,-0.297303,-0.178367,-0.171907,-0.132327,-0.0401191};
	float _shiftz_sim[] =   {-0.399355,-0.275355,-0.207417,-0.163975,-0.140999,-0.103207,-0.0851771,-0.063238,-0.0402442,-0.02793,-0.00142254,0.0105943,0.0404541,0.0559551,0.0963537,0.122534,0.154541,0.22572 };
	_type=ftype;
	for(int i1=0;i1<18;i1++){
	  //ISS
	  if(_type==0){
	        shiftxy[i1]=-31.95-_shiftxy_iss[i1];
	  	ecalz0[i1]=_ecalz[i1];
	  	shiftz[i1]=_shiftz_iss[i1];
	  }
	  //Test Beam
	  if(_type==1){
	    shiftxy[i1]=-31.95-_shiftxy_bt [i1];
	    ecalz0[i1]=_ecalz[i1];
	    shiftz[i1]=0.        ;
	  }
	  //Simulation
	  if(_type==2){
	    shiftxy[i1]=-31.95-_shiftxy_sim[i1];
	    ecalz0[i1]=_ecalz_new[i1];
	    shiftz[i1]=_shiftz_sim[i1];
	  }
        }
        for(int i1=0;i1<18;i1++){
	  for(int i2=0;i2<72;i2++){
	    fdead_cell[i1][i2]=0;
	  }
        }
        //ISS
        if(_type==0){
	  fdead_cell[14][65]=1;
	  fdead_cell[6 ][16]=1;
	  fdead_cell[6 ][17]=1;
	  fdead_cell[7 ][16]=1;
	  fdead_cell[7 ][17]=1;
        }
        //Test Beam

        //Simualtion
        if(_type==2){
	  fdead_cell[14][65]=1;
        }
}
EcalAxis::~EcalAxis(){
	if(ecalchi2)
		delete ecalchi2;	
}
bool EcalAxis::init_lf(){
	ecalchi2->set_edep(Edep_raw,_erg);
	TVirtualFitter::SetDefaultFitter("Minuit");	
	gMinuit_EcalAxis=TVirtualFitter::Fitter(0,3);
	gMinuit_EcalAxis->SetFCN(fcn_EcalAxis_Chi2);
	gMinuit_EcalAxis->SetObjectFit(this);
	double arglist[10];
	int ret;
	arglist[0]=-1;
    	gMinuit_EcalAxis->ExecuteCommand("SET PRINT", arglist, 1);
    	gMinuit_EcalAxis->ExecuteCommand("SET NOW", arglist, 0);
	arglist[0]=0.1;
	gMinuit_EcalAxis->ExecuteCommand("SET ERR",arglist, 1);
	
	float init_x0=0;
    	float init_y0=0;
    	float init_dxdz=0;
    	float init_dydz=0;
	
	if((_status&4)!=4){
		if(init_cg())
                        _status+=4;
		else
			return false;
	}
	
	init_x0  =p0_cg[0];
	init_y0  =p0_cg[1];
	init_dxdz=dir_cg[0]/dir_cg[2];
	init_dydz=dir_cg[1]/dir_cg[2];
	
	gMinuit_EcalAxis->SetParameter(0,  "x0"  ,init_x0  ,0.2  ,init_x0-0.9 ,init_x0+0.9 );
    	gMinuit_EcalAxis->SetParameter(1,  "y0"  ,init_y0  ,0.2  ,init_y0-0.9 ,init_y0+0.9 );
    	gMinuit_EcalAxis->SetParameter(2,  "dxdz",init_dxdz,0.1   ,init_dxdz-.2,init_dxdz+.2);
    	gMinuit_EcalAxis->SetParameter(3,  "dydz",init_dydz,0.1   ,init_dydz-.2,init_dydz+.2);

	if(_erg>5)	
		arglist[0]=300;
	else
		arglist[0]=10 ;
	arglist[1]=10.;
	//Test before call Minuit
	int cflag=0 ;
	double p[4] ;
	double chi20;
	double chi21;
	srand(time(NULL));
	p[0]=init_x0  ;
	p[1]=init_y0  ;
	p[2]=init_dxdz;
	p[3]=init_dydz;
	chi20=GetChi2(p);
	for(int i1=0;i1<10;i1++){
		p[0]=init_x0+(rand()%200-100.)/200.*0.9  ;
		p[1]=init_y0+(rand()%200-100.)/200.*0.9	 ;
		p[2]=init_dxdz+(rand()%200-100.)/200.*0.2;
		p[3]=init_dydz+(rand()%200-100.)/200.*0.2;
		chi21=GetChi2(p);
		if(fabs(chi20-chi21)>0.1)
			cflag++;
	}
	if(cflag>0){
	    	ret=gMinuit_EcalAxis->ExecuteCommand("MINI", arglist, 2);
    		//ret=gMinuit_EcalAxis->ExecuteCommand("SIMPLEX",0, 0);
	
		p0_lf[0]=gMinuit_EcalAxis->GetParameter(0);
		p0_lf[1]=gMinuit_EcalAxis->GetParameter(1);
		p0_lf[2]=ecalz[8];
		dir_lf[0]=gMinuit_EcalAxis->GetParameter(2);
		dir_lf[1]=gMinuit_EcalAxis->GetParameter(3);
		dir_lf[2]=1.0;
	}
	else{
		p0_lf[0]=init_x0 ;
		p0_lf[1]=init_y0 ;
		p0_lf[2]=ecalz[8];
		dir_lf[0]=init_dxdz;
		dir_lf[1]=init_dydz;
		dir_lf[2]=1.0	   ;
	}
	double r=sqrt(dir_lf[0]*dir_lf[0]+dir_lf[1]*dir_lf[1]+dir_lf[2]*dir_lf[2]);
	dir_lf[0]/=r;
	dir_lf[1]/=r;
	dir_lf[2]/=r;
	return true;
}

int EcalAxis::interpolate(float zpl       ,AMSPoint& p0   ,AMSDir  &dir, int algorithm){
	float x0,y0,dxdz,dydz;
	if((algorithm&4)==4){
		if((_status&4)!=4)
			return -1;
		x0    =p0_cg[0];
		y0    =p0_cg[1];
		dxdz  =dir_cg[0]/dir_cg[2];
		dydz  =dir_cg[1]/dir_cg[2];
		dir[0]=dir_cg[0];
		dir[1]=dir_cg[1];
		dir[2]=dir_cg[2];
	}
	else if((algorithm&1)==1){
		if((_status&1)!=1)
                        return -1;
                x0    =p0_cr[0];
                y0    =p0_cr[1];
                dxdz  =dir_cr[0]/dir_cr[2];
                dydz  =dir_cr[1]/dir_cr[2];
		dir[0]=dir_cr[0];
                dir[1]=dir_cr[1];
                dir[2]=dir_cr[2];
	}
	else if((algorithm&2)==2){
                if((_status&2)!=2)
                        return -1;
                x0    =p0_lf[0];
                y0    =p0_lf[1];
                dxdz  =dir_lf[0]/dir_lf[2];
                dydz  =dir_lf[1]/dir_lf[2];
                dir[0]=dir_lf[0];
                dir[1]=dir_lf[1];
                dir[2]=dir_lf[2];
        }
	else
		return -2;
	p0[0]=x0+(zpl-ecalz[8])*dxdz;
	p0[1]=y0+(zpl-ecalz[8])*dydz;
	p0[2]=zpl;
	
	return 0;
}



void EcalAxis::fcn_EcalAxis_Chi2(Int_t &npar, Double_t *gin, Double_t &f, Double_t *par, Int_t iflag){
	EcalAxis* ecalaxis=(EcalAxis*)gMinuit_EcalAxis->GetObjectFit();
	f=ecalaxis->GetChi2(par);
}
float EcalAxis::GetChi2(double* par){
	//par--->
	//0: x0  , 1: y0   @z=ecalz[8]
	//2: dxdz, 3: dydz z=1.
	float pos[18];
	for(int i1=0;i1<18;i1++){
		if(i1%4>1)
			pos[i1]=par[0]+(ecalz[i1]-ecalz[8])*par[2];
		else
			pos[i1]=par[1]+(ecalz[i1]-ecalz[8])*par[3];
	}
	float ret=ecalchi2->process(pos,_sign);
	return ret;
}
void EcalAxis::get_z(){
	for(int i1=0;i1<18;i1++){
		ecalz[i1]=ecalz0[i1]+shiftz[i1];
	}
}
int   EcalAxis::process(AMSEventR* ev, int algorithm, TrTrackR* trtrack){
#ifdef _PGTRACK_ 
	float fedep[1296];
	int   fcell[1296], fplane[1296],nEcalHits,ret;
	float EnergyD, _EnergyE, sign;
	nEcalHits=ev->nEcalHit()     ;
	for(int i1=0;i1<nEcalHits;i1++){
		fedep[i1] =ev->pEcalHit(i1)->Edep;
		fcell[i1] =ev->pEcalHit(i1)->Cell;
		fplane[i1]=ev->pEcalHit(i1)->Plane;
	}
	float maxe=-1;
	int  nmax =-1;
	if(ev->nEcalShower()<1)
		return -1;
	for(int i1=0;i1<ev->nEcalShower();i1++){
		if(ev->pEcalShower(i1)->EnergyE>maxe){
			maxe =ev->pEcalShower(i1)->EnergyE;
			nmax =i1                          ;
		}
	}	
	_EnergyE=ev->pEcalShower(nmax)->EnergyE;
	EnergyD =ev->pEcalShower(nmax)->EnergyD;
	if(trtrack!=NULL)
		sign=trtrack->GetRigidity()>0?1.:-1.;	
	else	
		sign=-1;
	ret= process(fedep,fcell,fplane,nEcalHits,EnergyD,_EnergyE,algorithm,sign);


	return ret;	
#else
	return -1;
#endif	
}
int EcalAxis::process(float* fedep,int* fcell,int* fplane, int nEcalhits,float EnergyD, float _EnergyE,int algorithm,float sign){
	_sign=sign;
        _algorithm=algorithm;
        for(int i1=0;i1<18;i1++){
                for(int i2=0;i2<72;i2++){
                        Edep_raw[i1][i2]=0.;
                }
                Max_layer_edep[i1]= 0;
                Max_layer_cell[i1]=-1;
                Layer_quality [i1]= 0;
                layer_Edep    [i1]= 0;
        }
        _erg   =EnergyD/1000.;
        EnergyE=_EnergyE     ;
	for(int i1=0;i1<nEcalhits;i1++){
        	Edep_raw[fplane[i1]][fcell[i1]]=fedep[i1];
              	if(Max_layer_edep[fplane[i1]]<fedep[i1]){
                      	Max_layer_edep[fplane[i1]]=fedep[i1];
                        Max_layer_cell[fplane[i1]]=fcell[i1];
                }
                layer_Edep[fplane[i1]]+=fedep[i1];
        }

	for(int i1=0;i1<18;i1++){       
                if(Max_layer_cell[i1]-2>-1){
                        if(Edep_raw[i1][Max_layer_cell[i1]-2]>0)
                                Layer_quality [i1]+=1;
                }       
                if(Max_layer_cell[i1]-1>-1){
                        if(Edep_raw[i1][Max_layer_cell[i1]-1]>0)
                                Layer_quality [i1]+=2;
                }
                if(Max_layer_cell[i1]>-1){
                        if(Edep_raw[i1][Max_layer_cell[i1]]>0)
                                Layer_quality [i1]+=4;
                }
                if(Max_layer_cell[i1]+1<72){
                        if(Edep_raw[i1][Max_layer_cell[i1]+1]>0)
                                Layer_quality [i1]+=8;
                }
                if(Max_layer_cell[i1]+2<72){
                        if(Edep_raw[i1][Max_layer_cell[i1]+2]>0)
                                Layer_quality [i1]+=16;
                }
        }
        get_z();
        _status=0;
        if(init_cg())
            _status+=4;
        if((algorithm&2)==2){
                if(init_lf())
                        _status+=2;
        }
        if((algorithm&1)==1){
                if(init_cr())
                        _status+=1;
        }       

	return _status;
}
int EcalAxis::process(EcalShowerR* esh,int algorithm,float sign){
	if(esh==NULL)
		return -1;
	_sign=sign;
	_algorithm=algorithm;
	for(int i1=0;i1<18;i1++){
                for(int i2=0;i2<72;i2++){
                        Edep_raw[i1][i2]=0.;
                }
		Max_layer_edep[i1]= 0;
		Max_layer_cell[i1]=-1;
		Layer_quality [i1]= 0;
        	layer_Edep    [i1]= 0;
	}
        _erg   =esh->EnergyD/1000.;
	EnergyE=esh->EnergyE      ;
        for(int i1=0;i1<esh->NEcal2DCluster();i1++){
                Ecal2DClusterR * ecal2d=esh->pEcal2DCluster(i1);
                for(int i2=0;i2<ecal2d->NEcalCluster();i2++){
                        EcalClusterR* ecalclt=ecal2d->pEcalCluster(i2);
                        for(int i3=0;i3<ecalclt->NEcalHit();i3++){
                                Edep_raw[ecalclt->pEcalHit(i3)->Plane][ecalclt->pEcalHit(i3)->Cell]=ecalclt->pEcalHit(i3)->Edep;
                        	if(Max_layer_edep[ecalclt->pEcalHit(i3)->Plane]<ecalclt->pEcalHit(i3)->Edep){
						Max_layer_edep[ecalclt->pEcalHit(i3)->Plane]=ecalclt->pEcalHit(i3)->Edep;
						Max_layer_cell[ecalclt->pEcalHit(i3)->Plane]=ecalclt->pEcalHit(i3)->Cell;
				}
				layer_Edep[ecalclt->pEcalHit(i3)->Plane]+=ecalclt->pEcalHit(i3)->Edep;
			}
                }
        }

	for(int i1=0;i1<18;i1++){	
		if(Max_layer_cell[i1]-2>-1){
			if(Edep_raw[i1][Max_layer_cell[i1]-2]>0)
				Layer_quality [i1]+=1;
		}	
		if(Max_layer_cell[i1]-1>-1){
                        if(Edep_raw[i1][Max_layer_cell[i1]-1]>0)
                                Layer_quality [i1]+=2;
                }
		if(Max_layer_cell[i1]>-1){
                        if(Edep_raw[i1][Max_layer_cell[i1]]>0)
                                Layer_quality [i1]+=4;
                }
		if(Max_layer_cell[i1]+1<72){
                        if(Edep_raw[i1][Max_layer_cell[i1]+1]>0)
                                Layer_quality [i1]+=8;
                }
		if(Max_layer_cell[i1]+2<72){
                        if(Edep_raw[i1][Max_layer_cell[i1]+2]>0)
                                Layer_quality [i1]+=16;
                }
	}
	get_z();
	_status=0;
	if((algorithm&4)==4){
		if(init_cg())
			_status+=4;
	}
	if((algorithm&2)==2){
		if(init_lf())
			_status+=2;
	}
	if((algorithm&1)==1){
		if(init_cr())
			_status+=1;
	}
	return _status;	
}
bool EcalAxis::init_cg(){
	bool ret;
	float x[18],y[18],ax,bx,ay,by;
	int npoints;
	for(int i1=0;i1<18;i1++){	
		init_raw[i1]=0.;
	}
	for(int i1=0;i1<18;i1++){
		double n=0., sum=0.;
		if((Layer_quality [i1]&1)==1){
			n  +=Edep_raw[i1][Max_layer_cell[i1]-2];
			sum+=(shiftxy[i1]+(Max_layer_cell[i1]-2)*SIZE)*Edep_raw[i1][Max_layer_cell[i1]-2];
		}
		if((Layer_quality [i1]&2)==2){
                        n  +=Edep_raw[i1][Max_layer_cell[i1]-1];
                        sum+=(shiftxy[i1]+(Max_layer_cell[i1]-1)*SIZE)*Edep_raw[i1][Max_layer_cell[i1]-1];
                }
		if((Layer_quality [i1]&4)==4){
                        n  +=Edep_raw[i1][Max_layer_cell[i1]];
                        sum+=(shiftxy[i1]+(Max_layer_cell[i1])*SIZE)*Edep_raw[i1][Max_layer_cell[i1] ];
                }
		if((Layer_quality [i1]&8)==8){
                        n  +=Edep_raw[i1][Max_layer_cell[i1]+1];
                        sum+=(shiftxy[i1]+(Max_layer_cell[i1]+1)*SIZE)*Edep_raw[i1][Max_layer_cell[i1]+1];
                }
		if((Layer_quality [i1]&16)==16){
                        n  +=Edep_raw[i1][Max_layer_cell[i1]+2];
                        sum+=(shiftxy[i1]+(Max_layer_cell[i1]+2)*SIZE)*Edep_raw[i1][Max_layer_cell[i1]+2];
                }
		if(n>0.)
			init_raw[i1]=sum/n;	
	}
	//Fit X direction
	npoints=0;
        for(int i1=2;i1<18;i1+=4){
                if(Layer_quality[i1]>0.&&((Layer_quality[i1]&4)==4)){
                        y[npoints]=init_raw[i1];
                        x[npoints]=ecalz[i1];
                        npoints++;
                }
                if(Layer_quality[i1+1]>0.&&((Layer_quality[i1+1]&4)==4)){
                        y[npoints]=init_raw[i1+1];
                        x[npoints]=ecalz[i1+1];
                        npoints++;
                }
        }
	ret=straight_line_fit(x,y,npoints,ax,bx,chi2x_cg);
	if(!ret)
		return false;
	//Fit Y direction
	npoints=0;
	for(int i1=0;i1<18;i1+=4){
                if(Layer_quality[i1]>0.&&((Layer_quality[i1]&4)==4)){
                        y[npoints]=init_raw[i1];
                        x[npoints]=ecalz[i1];
                        npoints++;
                }
                if(Layer_quality[i1+1]>0.&&((Layer_quality[i1+1]&4)==4)){
                        y[npoints]=init_raw[i1+1];
                        x[npoints]=ecalz[i1+1];
                        npoints++;
                }
        }	
	ret=straight_line_fit(x,y,npoints,ay,by,chi2y_cg);
        if(!ret)
                return false;
	p0_cg[0]=ax+bx*ecalz[8];
	p0_cg[1]=ay+by*ecalz[8];
	p0_cg[2]=ecalz[8];
	double r=sqrt(bx*bx+by*by+1.);
	dir_cg[0]=bx/r;
	dir_cg[1]=by/r;
	dir_cg[2]=1./r;
	return true;
}
//***********My program for Cell Ratio Method 24/05/2012 MV
////***********Last update 13/07/2012 MV
bool EcalAxis::init_cr(){
	bool ret;
        float x[18],y[18],ax,bx,ay,by;
	float slope_cr[18], ratio_cr[18],Xminus1[18];
        float delta[18];	
        int npoints;
        for(int i1=0;i1<18;i1++){
                init_raw_cr[i1]=0.;
        }	
	for(int j=0;j<18;j++){
		if(((Layer_quality[j]&14)==14)&&layer_Edep[j]>50){
			slope_cr[j] = get_slope_cr(j)==0?-999999. : get_slope_cr(j);
            		ratio_cr[j] = get_ratio_cr(j);
            		delta[j] = get_deltaxy_cr(j);

		        if(slope_cr[j]!=-999999.){
              			Xminus1[j] =  (TMath::Log(Edep_raw[j][Max_layer_cell[j]-1]/Edep_raw[j][Max_layer_cell[j]+1])/slope_cr[j]) + ratio_cr[j];
            		}
            		else {
             	 		Xminus1[j] = -999999.;
            		}
			init_raw_cr[j]=-31.95 + SIZE*Max_layer_cell[j] + delta[j] + Xminus1[j];	
		}
	}
	//exit(-1);
	//Fit X direction
	npoints=0;
        for(int j=3;j<16;j++){
          if(j%4>=2){
            if(((Layer_quality[j]&14)==14)&&layer_Edep[j]>50){
              	y[npoints]=init_raw_cr[j];
              	x[npoints]=ecalz[j];
              	npoints++;
            }
          }
        }
        ret=straight_line_fit(x,y,npoints,ax,bx,chi2x_cr);
	if(!ret)
		return false;
	//Fit Y direction
	npoints=0;
	for(int j=4;j<16;j+=4){
		if(((Layer_quality[j]&14)==14)&&layer_Edep[j]>50){
			y[npoints]=init_raw_cr[j];
			x[npoints]=ecalz[j];
			npoints++;
          	}
		if(((Layer_quality[j]&14)==14)&&layer_Edep[j]>50){
			y[npoints]=init_raw_cr[j+1];
			x[npoints]=ecalz[j+1];
			npoints++;
		}
        }
        ret=straight_line_fit(x,y,npoints,ay,by,chi2y_cr);
	if(!ret)
                return false;
	p0_cr[0]=ax+bx*ecalz[8];
	p0_cr[1]=ay+by*ecalz[8];
	p0_cr[2]=ecalz[8];
        double r=sqrt(bx*bx+by*by+1.);
        dir_cr[0]=bx/r;
	dir_cr[1]=by/r;
	dir_cr[2]=1./r;
	return true;
}
float EcalAxis::get_slope_cr(int flayer){
  float slope_cr[18];
  float p0[18]    = {0.,0.,0.,3.35297,7.12363,3.26488,-1.61848,-1.63469,-0.383966,-0.141785, -0.172576, 0.412143,0.799396, 1.08324, -0.10977, -0.0125473, 0., 0.};
  float errp0[18] = {0.,0.,0.,0.346605, 0.257378, 0.218957, 0.20647,0.195194, 0.185196, 0.185387, 0.190938, 0.200293, 0.204818, 0.218977, 0.273406, 0.283186, 0., 0.};
  float p1[18]    = {0.,0.,0.,-17.654,-27.0108,-18.2289, -7.57984, -6.19202, -6.6958, -6.46684,-5.97743, -6.23, -5.77442, -5.66404, -2.18609, -1.84701, 0. , 0.};
  float errp1[18] = {0., 0., 0., 0.870339, 0.636702, 0.537672, 0.508595, 0.479077, 0.449094, 0.447786, 0.462006, 0.48296, 0.489205, 0.521647,0.661204, 0.677242, 0., 0.};
  float p2[18]    = {0.,0.,0., 11.081,19.6107,13.1886, 5.59706 , 4.25144, 3.87708, 3.79357, 3.69053, 3.74557, 3.07479, 2.98225, 0.319258, 0.0521284,0.,0.};
  float errp2[18] = {0.,0.,0.,0.785294,0.566947, 0.475815, 0.451701, 0.424245, 0.393173, 0.39078,0.404119,0.421223, 0.422771,  0.449608, 0.578109, 0.585638, 0. ,0.};
  float p3[18]    = {0.,0.,0.,-2.9457, -6.20248, -4.22114, -1.9128, -1.4114, -1.09021 , -1.12017, -1.18253, -1.18025,-0.890959,-0.870741 ,-0.00477062,0.0734578   ,0.,0.};
  float errp3[18] = {1.,1.,1.,1.,1.,1.,1.,1.,1.,1.,1.,1.,1.,1,1.,1.,1.,1.};//remember to complete this part!!!
  float p4[18]    = {0.,0.,0.,0.28909, 0.725524,0.501178, 0.243317, 0.176231, 0.116297,0.127854, 0.146521, 0.14473 , 0.104466, 0.103275, 5.78124e-05, -0.00796261  ,0.,0.};

  float errp4[18] = {1.,1.,1.,1.,1.,1.,1.,1.,1.,1.,1.,1.,1.,1,1.,1.,1.,1.};//remember to complete this part!!!
  //****Define the slope as a function of logE - using 4th order polinomial
  slope_cr[flayer]= p0[flayer] + p1[flayer]*log10(EnergyE) +  p2[flayer]*pow(log10(EnergyE),2) + p3[flayer]*pow(log10(EnergyE),3) + p4[flayer]*pow(log10(EnergyE),4);

  return slope_cr[flayer];
}


float EcalAxis::get_ratio_cr(int flayer){
	float ratio_cr[18] ={0.,0.,0.,-0.005,0.02,0.02,-0.01,-0.002,0.005,0.,0.,0.003,0.,0.004,0.004,0.002,0.,0.};
	return ratio_cr[flayer];
}


float EcalAxis::get_deltaxy_cr(int flayer){
	float deltaxy_cr[18] ={-0.07,-0.07,0.13,0.13,-0.07,-0.07,0.13,0.13,-0.07,-0.07,0.13,0.13,-0.07,-0.07,0.13,0.13,-0.07,-0.07};
    	return deltaxy_cr[flayer];
}



bool EcalAxis::straight_line_fit(float *x ,float*y, int npoints,float &a, float&b,float &fchi2,float* weight){
	double Ax[2][2],Bx[2];
	double w=1.0;
        double detx,ret;
	double err[18],sumerr=0.,sumerr2=0.,merr,errrms;
	int flag[18];
        Ax[0][0]=0.0;Ax[0][1]=0.0;Ax[1][0]=0.0;Ax[1][1]=0.0;Bx[0]=0.0;Bx[1]=0.0;
	for(int i1=0;i1<npoints;i1++){
		if(weight!=NULL)
			w=1./weight[i1];
		else
			w=1.;
		Ax[0][0]+=w;
		Ax[0][1]+=double(x[i1])*w;
		Ax[1][0]+=double(x[i1])*w;
		Ax[1][1]+=double(x[i1])*double(x[i1])*w;
		Bx[0]+=double(y[i1])*w;
		Bx[1]+=double(x[i1])*double(y[i1])*w;
	}
        detx=Ax[0][0]*Ax[1][1]-Ax[0][1]*Ax[1][0];
	if(detx==0)
		return false;
        a=1.0/detx*(Ax[1][1]*Bx[0]-Ax[0][1]*Bx[1]);
        b=1.0/detx*(-Ax[1][0]*Bx[0]+Ax[0][0]*Bx[1]);
	for(int i1=0;i1<npoints;i1++){
		err[i1]=(y[i1]-a-b*x[i1])/sqrt(1+b*b);
		sumerr+=err[i1];
		sumerr2+=err[i1]*err[i1];
	}
	merr=sumerr/npoints;
	errrms=(sumerr2-merr*merr);
	errrms=sqrt(errrms/npoints);
	
	vector<float> f;
	vector<int> f_ind;
	for(int i1=0;i1<npoints;i1++){
                f.push_back(fabs(err[i1]-merr));
                f_ind.push_back(i1);
	}
	bool flg;
        float temp;
        int tempi;
        for(int i1=0;i1<npoints;i1++){
                flg=false;
                for(int i2=npoints-1;i2>i1;i2--){
                        if(f[i2]>f[i2-1]){
                                temp=f[i2];
                                f[i2]=f[i2-1];
                                f[i2-1]=temp;
                                tempi=f_ind[i2];
                                f_ind[i2]=f_ind[i2-1];
                                f_ind[i2-1]=tempi;
                                flg=true;
                        }
                }
                if(!flg)
                        break;
        } 
	
	int count=0;
	for(int i1=0;i1<npoints;i1++){	
		if(f[i1]>2*errrms&&(npoints-count)>4){
			flag[f_ind[i1]]=0;
			count++;
		}
		else{
			flag[f_ind[i1]]=1;
		}
	}
	w=1.0;
        Ax[0][0]=0.0;Ax[0][1]=0.0;Ax[1][0]=0.0;Ax[1][1]=0.0;Bx[0]=0.0;Bx[1]=0.0;
        for(int i1=0;i1<npoints;i1++){
		if(flag[i1]==0)
			continue;
                if(weight!=NULL)
                        w=1./weight[i1];
                else
                        w=1.;
                Ax[0][0]+=w;
                Ax[0][1]+=double(x[i1])*w;
                Ax[1][0]+=double(x[i1])*w;
                Ax[1][1]+=double(x[i1])*double(x[i1])*w;
                Bx[0]+=double(y[i1])*w;
                Bx[1]+=double(x[i1])*double(y[i1])*w;
        }
        detx=Ax[0][0]*Ax[1][1]-Ax[0][1]*Ax[1][0];
        if(detx==0)
                return false;
        a=1.0/detx*(Ax[1][1]*Bx[0]-Ax[0][1]*Bx[1]);
        b=1.0/detx*(-Ax[1][0]*Bx[0]+Ax[0][0]*Bx[1]);	

	for(int i1=0;i1<npoints;i1++){
		if(flag[i1]==1)
	                ret+=(y[i1]-a-b*x[i1])*(y[i1]-a-b*x[i1]);
	}
	
	fchi2=(ret/(npoints-count))/errrms;
	return true;
}


////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
//End of Ecal Axis 
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

