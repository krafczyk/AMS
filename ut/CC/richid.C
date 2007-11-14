#include"richid.h"
#include"commons.h"
#include "job.h"
#include <fstream.h>

#ifndef __USERICHPMTMANAGER__
// This code is obsolete

geant *AMSRICHIdGeom::_pmt_p[3]={0,0,0};
integer *AMSRICHIdGeom::_pmt_orientation=0;
integer *AMSRICHIdGeom::_pmt_crate=0;
integer *AMSRICHIdGeom::_npmts_crate=0;
integer *AMSRICHIdGeom::_crate_pmt=0;
integer AMSRICHIdGeom::_ncrates=12;
integer AMSRICHIdGeom::_npmts=1000;


void AMSRICHIdGeom::Init(){
    _ncrates=8;
    _npmts=680;


    // Alloc space for pmt position
    _pmt_p[0]=new geant[_npmts];
    _pmt_p[1]=new geant[_npmts];
    _pmt_p[2]=new geant[_npmts];

    _pmt_orientation=new integer[_npmts];
  
    // Alloc space for pmt->crate table
    _pmt_crate=new integer[_npmts];
  
    // Alloc space fot crate->first pmt table
    _crate_pmt=new integer[_ncrates];

    // Alloc scape for number of pmts per crate
    _npmts_crate=new integer[_ncrates];
 

    // Fill tables: manual filling


    // Crate positioning is according to next drawing
    //
    //          3| 2 |1
    //          -+---+-
    //          4|   |0
    //          -+---+-
    //          5| 6 |7


    _npmts_crate[0]=143;
    _npmts_crate[1]=27;
    _npmts_crate[2]=143;
    _npmts_crate[3]=27;
    _npmts_crate[4]=143;
    _npmts_crate[5]=27;
    _npmts_crate[6]=143;
    _npmts_crate[7]=27;

    // Setups the look-up table 
    _crate_pmt[0]=0;
    for(int i=1;i<8;i++) _crate_pmt[i]=_crate_pmt[i-1]+_npmts_crate[i-1];

    integer pmtnb=0;

    // Setup crates 0,4: It's a bit tricky due to the pmts arrangement
    {
      const int nrows=17;
      const int pmts[nrows]={6,8,8,9,9,9,9,9,9,9,9,9,9,9,8,8,6};
      const int offset[nrows]={1,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,1};
      const int max_pmts=10;
      const int block_number=6; 
      
      //      geant xedge=RICHDB::hole_radius+pitch/2+RICpmtsupport;
      //      geant yedge=-RICHDB::hole_radius+pitch/2;  


      // Block 0 and 4 specifics
      geant xedge=34.65;
      geant yedge=-29.6;

      for(int i=0;i<nrows;i++)
	
	for(int j=0;j<pmts[i];j++){
	  
          *(_pmt_p[0]+_crate_pmt[0]+pmtnb)=xedge+(j+offset[i])*pitch;
          *(_pmt_p[1]+_crate_pmt[0]+pmtnb)=yedge+i*pitch;
          *(_pmt_p[2]+_crate_pmt[0]+pmtnb)=-RICHDB::pmt_pos();
	  
	  
          *(_pmt_p[0]+_crate_pmt[4]+pmtnb)=-(xedge+(j+offset[i])*pitch);
          *(_pmt_p[1]+_crate_pmt[4]+pmtnb)=-(yedge+i*pitch);
          *(_pmt_p[2]+_crate_pmt[4]+pmtnb)=-RICHDB::pmt_pos();
	  
	  
	  pmtnb++;
	  
        }
    }


    pmtnb=0;


    // Setup crates 2,6: It's a bit tricky due to the pmts arrangement
    {
      const int nrows=17;
      const int pmts[nrows]={6,8,8,9,9,9,9,9,9,9,9,9,9,9,8,8,6};
      const int offset[nrows]={1,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,1};
      const int max_pmts=10;
      const int block_number=6; 
      
      //      geant xedge=RICHDB::hole_radius+pitch/2+RICpmtsupport;
      //      geant yedge=-RICHDB::hole_radius+pitch/2;  


      // Block 2 and 6 specifics
      geant xedge=29.85;
      geant yedge=34.4;
      geant shift=0;

      for(int i=0;i<nrows;i++)
	for(int j=0;j<pmts[i];j++){
	  
	  shift=i<9?0:-.5;
	  
          *(_pmt_p[0]+_crate_pmt[2]+pmtnb)=xedge-i*pitch+shift;
          *(_pmt_p[1]+_crate_pmt[2]+pmtnb)=yedge+(j+offset[i])*pitch;
          *(_pmt_p[2]+_crate_pmt[2]+pmtnb)=-RICHDB::pmt_pos();
	  
	  
          *(_pmt_p[0]+_crate_pmt[6]+pmtnb)=-(xedge-i*pitch+shift);
	  *(_pmt_p[1]+_crate_pmt[6]+pmtnb)=-(yedge+(j+offset[i])*pitch);
          *(_pmt_p[2]+_crate_pmt[6]+pmtnb)=-RICHDB::pmt_pos();
	  
	  
	  pmtnb++;
	  
        }
    }




    pmtnb=0;


    { //Setup crates 1,3,5,7
      const int nrows=7;
      const int pmts[nrows]={6,6,5,4,3,2,1};
      const int offset[nrows]={1,0,0,0,0,0,0};
      const int max_pmts=6;
          
      //      geant xedge=RICHDB::hole_radius+pitch/2+RICpmtsupport;
      //      geant yedge=RICHDB::hole_radius+pitch/2+RICpmtsupport;

      geant xedge=34.65;
      geant yedge=34.4;

      for(int i=0;i<nrows;i++)
       for(int j=0;j<pmts[i];j++){

        *(_pmt_p[0]+_crate_pmt[1]+pmtnb)=xedge+(j+offset[i])*pitch;
        *(_pmt_p[1]+_crate_pmt[1]+pmtnb)=yedge+i*pitch;
        *(_pmt_p[2]+_crate_pmt[1]+pmtnb)=-RICHDB::pmt_pos();

        *(_pmt_p[0]+_crate_pmt[3]+pmtnb)=-(xedge+(j+offset[i])*pitch);
        *(_pmt_p[1]+_crate_pmt[3]+pmtnb)=yedge+i*pitch;       
        *(_pmt_p[2]+_crate_pmt[3]+pmtnb)=-RICHDB::pmt_pos();

        *(_pmt_p[0]+_crate_pmt[5]+pmtnb)=-(xedge+(j+offset[i])*pitch);
        *(_pmt_p[1]+_crate_pmt[5]+pmtnb)=-(yedge+i*pitch);       
        *(_pmt_p[2]+_crate_pmt[5]+pmtnb)=-RICHDB::pmt_pos();

        *(_pmt_p[0]+_crate_pmt[7]+pmtnb)=xedge+(j+offset[i])*pitch;
        *(_pmt_p[1]+_crate_pmt[7]+pmtnb)=-(yedge+i*pitch);
        *(_pmt_p[2]+_crate_pmt[7]+pmtnb)=-RICHDB::pmt_pos();

        pmtnb++;

       }

    }

    for(integer crate=0;crate<_ncrates;crate++)
      for(pmtnb=0;pmtnb<_npmts_crate[crate];pmtnb++)
        _pmt_crate[pmtnb+_crate_pmt[crate]]=crate;
        
    for(pmtnb=0;pmtnb<_npmts;pmtnb++)
      _pmt_orientation[pmtnb]=0;  // To be correctly filled in the future


    RICHDB::total=_npmts;

}




AMSRICHIdGeom::AMSRICHIdGeom(integer channel){
  channel=channel<0?0:channel%(_npmts*RICnwindows);

  _channel=channel;
  _pmt=channel/16;
  _pixel=channel%16;
  _crate=_pmt_crate[_pmt];
  _pmt_in_crate=_pmt-_crate_pmt[_crate];

}

AMSRICHIdGeom::AMSRICHIdGeom(integer pmtnb,geant x,geant y){
  // Better simulation of photocathode effective area


//  x-=AMSRICHIdGeom::pmt_pos(pmtnb,0)-RICcatolength/2;
//  y-=AMSRICHIdGeom::pmt_pos(pmtnb,1)-RICcatolength/2;
//  x/=RICcatolength/sqrt(RICnwindows);
//  y/=RICcatolength/sqrt(RICnwindows);



  x-=AMSRICHIdGeom::pmt_pos(pmtnb,0);
  y-=AMSRICHIdGeom::pmt_pos(pmtnb,1);

  if(fabs(x)>RICcatolength/2 || fabs(y)>RICcatolength/2){_channel=-1;return;}  //error
  if(fabs(x)<RICcatogap/2. || fabs(y)<RICcatogap/2.){_channel=-1;return;}
  if(fabs(fabs(x)-(RICcatogap+cato_inner_pixel))<RICcatogap/2. ||
     fabs(fabs(y)-(RICcatogap+cato_inner_pixel))<RICcatogap/2.) {_channel=-1;return;}

  integer nx,ny;

  if(fabs(x)>RICcatogap+cato_inner_pixel){
    nx=x>0?3:0;
  }else{
    nx=x>0?2:1;
  }
  
  if(fabs(y)>RICcatogap+cato_inner_pixel){
    ny=y>0?3:0;
  }else{
    ny=y>0?2:1;
  }


#ifdef __AMSDEBUG__
  //  cout<<"nx and ny according to new routine "<<nx<<" "<<ny<<endl;
#endif

  integer channel=RICnwindows*pmtnb+integer(sqrt(number(RICnwindows)))*
    ny+nx;
//    integer(y)+integer(x);
  
  _channel=channel;
  _pmt=channel/16;
  _pixel=channel%16;
  _crate=_pmt_crate[_pmt];
  _pmt_in_crate=_pmt-_crate_pmt[_crate];
}



integer AMSRICHIdGeom::get_channel_from_top(geant x,geant y){
  // First, decide in which crate to look for the guy. To do such thing it uses the hole radious
  // Second look for it and decide the PMTnb


  integer crate=-1;

  if(x>RICHDB::hole_radius[0]-RICpmtsupport/2.){
    if(y>RICHDB::hole_radius[1]-RICpmtsupport/2.) crate=1;
    else if(-y>RICHDB::hole_radius[1]-RICpmtsupport/2) crate=7;
    else crate=0;
  } else if(-x>RICHDB::hole_radius[0]-RICpmtsupport/2.){
    if(y>RICHDB::hole_radius[1]-RICpmtsupport/2.) crate=3;
    else if(-y>RICHDB::hole_radius[1]-RICpmtsupport/2) crate=5;
    else crate=4;
  }else{
    if(y>RICHDB::hole_radius[1]-RICpmtsupport/2.) crate=2;
    else if(-y>RICHDB::hole_radius[1]-RICpmtsupport/2) crate=6;
    else return -1;
  }


#ifdef __AMSDEBUG__
  /*
  static int first=1;

  if(first){
    first=0;
    integer crate;

    for(float y=67;y>=-67;y-=2.){
      for(float x=-67;x<=67;x+=2.){
	
		
	if(x>RICHDB::hole_radius[0]-RICpmtsupport/2.){
	  if(y>RICHDB::hole_radius[1]-RICpmtsupport/2.) crate=1;
	  else if(-y>RICHDB::hole_radius[1]-RICpmtsupport/2) crate=7;
	  else crate=0;
	} else if(-x>RICHDB::hole_radius[0]-RICpmtsupport/2.){
	  if(y>RICHDB::hole_radius[1]-RICpmtsupport/2.) crate=3;
	  else if(-y>RICHDB::hole_radius[1]-RICpmtsupport/2) crate=5;
	  else crate=4;
	}else{
	  if(y>RICHDB::hole_radius[1]-RICpmtsupport/2.) crate=2;
	  else if(-y>RICHDB::hole_radius[1]-RICpmtsupport/2) crate=6;
	  else crate=-1;
	}
	
	cout <<crate+1;
	
      }
      cout<<endl;
    }
    }*/
#endif

  
  // Now we now in which crate to look for... doit

  integer pmt_chosen=-1;

  /*
  for(int n=0;n<_npmts;n++){

    if(fabs(x-_pmt_p[0][n])<RICHDB::lg_length/2. &&
       fabs(y-_pmt_p[1][n])<RICHDB::lg_length/2.){
      pmt_chosen=n;
      break;
    }
    }
  */

  for(int n=_crate_pmt[crate];n<_crate_pmt[crate]+_npmts_crate[crate];n++){
    // Check if this guy is within a given pmt light guide group 

    if(fabs(x-_pmt_p[0][n])<RICHDB::lg_length/2. &&
       fabs(y-_pmt_p[1][n])<RICHDB::lg_length/2.){
      pmt_chosen=n;
      break;
    }
  }


#ifdef __AMSDEBUG__
  //  cout<<"    @ get channel: chosen "<<pmt_chosen<<endl;
#endif


  if(pmt_chosen==-1) return -1;

#ifdef __AMSDEBUG__
  //  cout<<"    @ get channel: x,y"<<
  //    _pmt_p[0][pmt_chosen]<<" "<<_pmt_p[1][pmt_chosen]<<endl;
  //  cout <<"   @ get channel: input x,y:"<<
  //    x<<" "<<y<<endl;
#endif


  x=(x-_pmt_p[0][pmt_chosen]+RICHDB::lg_length/2.)*
    4/RICHDB::lg_length;
  y=(y-_pmt_p[1][pmt_chosen]+RICHDB::lg_length/2.)*
    4/RICHDB::lg_length;
  
#ifdef __AMSDEBUG__
  //  cout<<"    @ get channel: nx,ny"<<
  //    int(x)<<" "<<int(y)<<endl;
#endif
  
  //  if(int(y)*integer(sqrt(RICnwindows))+int(x)>15 || 
  //       int(y)*integer(sqrt(RICnwindows))+int(x)<0) {cout <<"ERRORRRRRR"<<endl;exit(1);}

  return int(y)*integer(sqrt(number(RICnwindows)))+int(x);
 
}



// The rotation is not implemented yet

geant AMSRICHIdGeom::x(){

// WARNING This to be mapped to the new setup

  geant x=(2*(_pixel%integer(sqrt(number(RICnwindows))))-3)*
    RICHDB::lg_length/8.+*(_pmt_p[0]+_pmt);

#ifdef __AMSDEBUG__
  /*  if(x>1e6){
    cout<<endl<<"--------"<<endl
	<<"PMT position in x "<<*(_pmt_p[0]+_pmt)<<endl
	<<"Factor "<<2*(_pixel%integer(sqrt(number(RICnwindows))))-3<<endl
	<<"--------"<<endl;
	}*/
#endif

  return x;
}

geant AMSRICHIdGeom::y(){
  geant y=(2*(_pixel/integer(sqrt(number(RICnwindows))))-3)*
    RICHDB::lg_length/8.+*(_pmt_p[1]+_pmt);

  return y;
}


geant AMSRICHIdGeom::z(){
  geant z=RICHDB::RICradpos()+*(_pmt_p[2]+_pmt)+RICHDB::pmtb_height()/2.;

  return z;
}




///////////////////////////////////////////////////////////


uinteger AMSRICHIdSoft::_nchannels=0;
geant * AMSRICHIdSoft::_ped=0;
geant *AMSRICHIdSoft::_sig_ped=0;

geant *AMSRICHIdSoft::_lambda=0;
geant *AMSRICHIdSoft::_scale=0;
geant *AMSRICHIdSoft::_transparency=0;
geant *AMSRICHIdSoft::_lambda_dyn=0;
geant *AMSRICHIdSoft::_scale_dyn=0;


geant *AMSRICHIdSoft::_threshold=0;
integer *AMSRICHIdSoft::_gain_mode_boundary=0;
integer *AMSRICHIdSoft::_status=0;

uinteger AMSRICHIdSoft::_nbins=1000;  // default value
geant *AMSRICHIdSoft::_cumulative_prob=0;
geant *AMSRICHIdSoft::_step=0;

void AMSRICHIdSoft::Init(){
  _nchannels=AMSRICHIdGeom::getpmtnb()*16;  // Assume 16 channels per PMT
  _ped=new geant[2*_nchannels];
  _sig_ped=new geant[2*_nchannels];

  _lambda=new geant[2*_nchannels];
  _scale=new geant[2*_nchannels];
  _transparency=new geant[2*_nchannels];
  _lambda_dyn=new geant[2*_nchannels];
  _scale_dyn=new geant[2*_nchannels];

  _threshold=new geant[2*_nchannels];
  _gain_mode_boundary=new integer[_nchannels];
  _status=new integer[_nchannels];

  _cumulative_prob=new geant[2*_nchannels*_nbins];
  _step=new geant[2*_nchannels];

  // Check

  assert(_ped && _sig_ped && _lambda && _scale && _transparency && 
         _lambda_dyn && _scale_dyn && _threshold && _gain_mode_boundary &&
         _status && _cumulative_prob);

  // Here we should read the calibration TDV  if needed 


  // Here we should fill all the values

  if(RICFFKEY.ReadFile){
    char name[80];

    strcpy(name,"richcal");

    if(AMSJob::gethead()->isMCData()){ // McData 
      strcat(name,"mc");
    }else{  // Real data
      strcat(name,"dt");
    }


    strcat(name,".001");  // Version


    cout<<"AMSRICHIdSoft::Init: Loading default calibration ..."<<endl;
    fstream calib(name,ios::in); // open  file for reading

    if(calib){
      cout<<"AMSRICHIdSoft::Init: Local "<<name<<" Opened"<<endl;
    }else{
      char newname[80];
      strcpy(newname,AMSDATADIR.amsdatadir);
      strcat(newname,name);
      strcpy(name,newname);
      calib.open(newname,ios::in);
      if(calib){
	cout<<"AMSRICHIdSoft::Init: "<<name<<" Opened"<<endl;
      }else{
	cerr <<"AMSRICHIdSoft::Init:: missing "<<name<<endl;
	exit(1);
      }

    }


    for(int i=0;i<_nchannels;i++){
      for(int hl=0;hl<2;hl++){
	calib >> _ped[2*i+hl];
	calib >> _sig_ped[2*i+hl];
	calib >> _threshold[2*i+hl];
	calib >> _lambda[2*i+hl];
	calib >> _scale[2*i+hl];
	calib >> _transparency[2*i+hl];
	calib >> _lambda_dyn[2*i+hl];
	calib >> _scale_dyn[2*i+hl];
      }
      
      calib >> _gain_mode_boundary[i];
      calib >> _status[i];
      
    }
    
    calib.close();


#ifdef __AMSDEBUG__
    /*    for(int i=0;i<_nchannels;i++){
      for(int hl=0;hl<2;hl++){
	
	cout <<"Channel "<<i<<" mode "<<(hl==0?"low":"high")<<
	  _ped[2*i+hl]<<" "<<_sig_ped[2*i+hl]<<" "<<_threshold[2*i+hl]<<" "<<
	  _lambda[2*i+hl]<<" "<<_scale[2*i+hl]<<" "<<_transparency[2*i+hl]<<" "<<
	  _lambda_dyn[2*i+hl]<<" "<<_scale_dyn[2*i+hl]<<endl;
      }
      cout <<"   --> "<< _gain_mode_boundary[i]<<" "<<_status[i]<<endl;

      }*/
#endif    


    
    
  }else{

    cout<<"AMSRICHIdSoft::Init: Using nominal calibration."<<endl;
    for(int i=0;i<_nchannels;i++){
      for(int hl=0;hl<2;hl++){
	// OLD PMT simulation 
	//        _ped[2*i+hl]=0.;
	//        _sig_ped[2*i+hl]=4.;
	//        _threshold[2*i+hl]=4.;
	//        _lambda[2*i+hl]=1.23;
	//        _scale[2*i+hl]=30./(5.-4.*hl);;
	//        _transparency[2*i+hl]=0.;
	//        _lambda_dyn[2*i+hl]=0.;
	//        _scale_dyn[2*i+hl]=0.;

	// New PMT simulation
        _ped[2*i+hl]=0.;
        _sig_ped[2*i+hl]=4.;
        _threshold[2*i+hl]=4.;
        _lambda[2*i+hl]=4.92;
        _scale[2*i+hl]=12.25/(5.-4.*hl);;
        _transparency[2*i+hl]=0.;
        _lambda_dyn[2*i+hl]=0.;
        _scale_dyn[2*i+hl]=0.;

      }
      
      _gain_mode_boundary[i]=3000;
      _status[i]=1;
      
    }
    


  }



  // Fill the probability function for each channel
  fill_probability();

}


void AMSRICHIdSoft::fill_probability(){
  if(RICFFKEY.ReadFile){
    for(int channel=0;channel<_nchannels;channel++)
      for(int mode=0;mode<2;mode++){
	geant gain,sigma;
	geant lambda,scale;
	
	lambda=_lambda[2*channel+mode];
	scale=_scale[2*channel+mode];	
	GETRMURSG(lambda,scale,gain,sigma);
	
	geant upper_limit=gain*10;
	_step[2*channel+mode]=upper_limit/_nbins;
	
	// Use the trapezoid rule to compute the integral
	_cumulative_prob[2*channel*_nbins+mode*_nbins]=0;
	for(int i=1;i<_nbins;i++){
	  float value,x1,x2;
	  
	  x1=i*_step[2*channel+mode];
	  x2=(i-1)*_step[2*channel+mode];
	  value=PDENS(x1,lambda,scale)+PDENS(x2,lambda,scale);
	  value*=_step[2*channel+mode]/2;
	  _cumulative_prob[2*channel*_nbins+mode*_nbins+i]=_cumulative_prob[2*channel*_nbins+mode*_nbins+i-1]+value;
	}

	for(int i=0;i<_nbins;i++)
	  _cumulative_prob[2*channel*_nbins+mode*_nbins+i]/=
	    _cumulative_prob[2*channel*_nbins+mode*_nbins+_nbins-1];

      }
  }
  else{
    // Nominal calibration


      for(int mode=0;mode<2;mode++){
	int channel=0;
	geant gain,sigma;
	geant lambda,scale;
	
	lambda=_lambda[2*channel+mode];
	scale=_scale[2*channel+mode];	
	GETRMURSG(lambda,scale,gain,sigma);
	
	geant upper_limit=gain*10;
	_step[2*channel+mode]=upper_limit/_nbins;
	for(int chan=1;chan<_nchannels;chan++) _step[2*chan+mode]=_step[2*channel+mode];

	// Use the trapezoid rule to compute the integral
	_cumulative_prob[2*channel*_nbins+mode*_nbins]=0;
	for(int i=1;i<_nbins;i++){
	  float value,x1,x2;
	  
	  x1=i*_step[2*channel+mode];
	  x2=(i-1)*_step[2*channel+mode];
	  value=PDENS(x1,lambda,scale)+PDENS(x2,lambda,scale);
	  value*=_step[2*channel+mode]/2;
	  _cumulative_prob[2*channel*_nbins+mode*_nbins+i]=_cumulative_prob[2*channel*_nbins+mode*_nbins+i-1]+value;
	}
	
	for(int i=0;i<_nbins;i++){
	  _cumulative_prob[2*channel*_nbins+mode*_nbins+i]/=
	    _cumulative_prob[2*channel*_nbins+mode*_nbins+_nbins-1];
	  for(int chan=1;chan<_nchannels;chan++)
	    _cumulative_prob[2*chan*_nbins+mode*_nbins+i]=_cumulative_prob[2*channel*_nbins+mode*_nbins+i];
	}
	
      }
  }
  
}



geant AMSRICHIdSoft::simulate_single_pe(int mode){
  // Sample from the _cumulative_prob for _address and mode
  geant dummy=0;
  geant value=RNDM(dummy);
  for(int i=0;i<_nbins;i++){
    if(value<=_cumulative_prob[2*_nbins*_address+(mode<=0?0:1)*_nbins+i])
      return i*_step[2*_address+(mode<=0?0:1)];
  }
}

#endif

////////////////////////////////////////////////////////////////////

#ifdef __USERICHPMTMANAGER__

RichPMTChannel::RichPMTChannel(int packed_id){
  int pmt,channel;
  RichPMTsManager::UnpackGeom(packed_id,pmt,channel);
  Init(pmt,channel);
}


RichPMTChannel::RichPMTChannel(int geom_pmt,int geom_channel){
  Init(geom_pmt,geom_channel);
}

RichPMTChannel::RichPMTChannel(int geom_pmt,geant x,geant y){
  //
  // Get the channel given the position in the cathode
  // 
  RichPMT &pmt=RichPMTsManager::GetPMT(geom_pmt);
  
  
  x-=pmt._global_position[0];
  y-=pmt._global_position[1];


  if(fabs(x)>RICcatolength/2 || fabs(y)>RICcatolength/2){pmt_geom_id=-1;return;}
  if(fabs(x)<RICcatogap/2. || fabs(y)<RICcatogap/2.){pmt_geom_id=-1;return;}
  if(fabs(fabs(x)-(RICcatogap+cato_inner_pixel))<RICcatogap/2. ||
	   fabs(fabs(y)-(RICcatogap+cato_inner_pixel))<RICcatogap/2.){pmt_geom_id=-1;return;}

  integer nx,ny;

  if(fabs(x)>RICcatogap+cato_inner_pixel){
    nx=x>0?3:0;
  }else{
    nx=x>0?2:1;
  }
  
  if(fabs(y)>RICcatogap+cato_inner_pixel){
    ny=y>0?3:0;
  }else{
    ny=y>0?2:1;
  }
  integer channel=integer(sqrt(number(RICnwindows)))*ny+nx;

  Init(geom_pmt,channel);
}


RichPMTChannel::RichPMTChannel(geant x,geant y){
  //
  // Get the channel given coordinates AT THE TOP of light guides
  //

  int packed=RichPMTsManager::FindChannel(x,y);
  assert(packed>0);

  int geom_pmt,channel;
  RichPMTsManager::UnpackGeom(packed,geom_pmt,channel);
  Init(geom_pmt,channel);
}


void RichPMTChannel::Init(int geom_pmt,int channel_id){
  pmt_geom_id=geom_pmt;
  channel_geom_id=channel_id;
  RichPMT &pmt=RichPMTsManager::GetPMT(geom_pmt);
  gain[0]=RichPMTsManager::Gain(geom_pmt,channel_id,0);
  gain[1]=RichPMTsManager::Gain(geom_pmt,channel_id,1);  
  gain_sigma[0]=RichPMTsManager::GainSigma(geom_pmt,channel_id,0);
  gain_sigma[1]=RichPMTsManager::GainSigma(geom_pmt,channel_id,1);
  pedestal[0]=RichPMTsManager::Pedestal(geom_pmt,channel_id,0);
  pedestal[1]=RichPMTsManager::Pedestal(geom_pmt,channel_id,1);
  pedestal_sigma[0]=RichPMTsManager::PedestalSigma(geom_pmt,channel_id,0);
  pedestal_sigma[1]=RichPMTsManager::PedestalSigma(geom_pmt,channel_id,1);
  rel_eff=RichPMTsManager::Eff(geom_pmt,channel_id)/RichPMTsManager::MaxEff();
  gain_threshold=RichPMTsManager::GainThreshold(geom_pmt,channel_id);
  pedestal_threshold[0]=RichPMTsManager::PedestalThreshold(geom_pmt,channel_id,0);
  pedestal_threshold[1]=RichPMTsManager::PedestalThreshold(geom_pmt,channel_id,1);
  status=RichPMTsManager::Status(geom_pmt,channel_id);

  position[0]=pmt._channel_position[channel_id][0];
  position[1]=pmt._channel_position[channel_id][1];
  position[2]=pmt._channel_position[channel_id][2]-RICHDB::total_height()/2+RICHDB::RICradpos()+RICHDB::pmtb_height()/2.;
}


/////////////////////////////////////////////////////////////////

int   RichPMTsManager::_status[RICmaxpmts*RICnwindows];                  // Channel status word
geant RichPMTsManager::_pedestal[2*RICmaxpmts*RICnwindows];              // Pedestal position (x2 gains) high,low
geant RichPMTsManager::_pedestal_sigma[2*RICmaxpmts*RICnwindows];        // Pedestal width (x2 gains)
geant RichPMTsManager::_pedestal_threshold[2*RICmaxpmts*RICnwindows];             // Pedestal threshold width (x2 gains)
geant RichPMTsManager::_gain[2*RICmaxpmts*RICnwindows];                  // Pedestal position (x2 gains) high,low
geant RichPMTsManager::_gain_sigma[2*RICmaxpmts*RICnwindows];            // Pedestal width (x2 gains)
int RichPMTsManager::_gain_threshold[RICmaxpmts*RICnwindows];        // Pedestal threshold width (x2 gains)
geant RichPMTsManager::_relative_efficiency[RICmaxpmts*RICnwindows];   // Pedestal threshold width (x2 gains)
RichPMT RichPMTsManager::_pmts[RICmaxpmts];

geant RichPMTsManager::_max_eff=0;
geant RichPMTsManager::_mean_eff=0;
int RichPMTsManager::_grid_nb_of_pmts[8];
int *RichPMTsManager::_grid_pmts[8];    

void RichPMTsManager::Init(){
  Init_Default();

  //
  // Here we should load tables or wahtever
  // 
  if(RICFFKEY.ReadFile%10){
    char name[200];

    UHTOC(RICFFKEY.fname_in,200,name,200);

    for(int i=199;i>=0;i--){
      if(name[i]!=' '){
	name[i+1]=0;
	break;
      }
    }

    if(name[0]==' ') name[0]=0;
    
    if(strlen(name)==0){
      strcpy(name,"richcal");
      
      if(AMSJob::gethead()->isMCData()){ // McData 
	strcat(name,"mc");
      }else{  // Real data
	strcat(name,"dt");
      }
      
      
      strcat(name,".002");  // Version
    }
    ReadFromFile(name);
  }


  //
  // Initialize probability tables if is simulation
  // 
  if(AMSJob::gethead()->isSimulation()){
    for(int i=0;i<RICmaxpmts;i++) _pmts[i].compute_tables();
  }

  //
  // Compute efficiency quantities
  //
  get_eff_quantities();

  //
  // Build look up tables to search for pmts within a grid
  //
  int current[8];
  for(int i=0;i<8;i++){_grid_nb_of_pmts[i]=0;current[i]=0;}
  for(int i=0;i<RICmaxpmts;i++){
    _grid_nb_of_pmts[_pmts[i].grid()-1]++;
  }
  for(int i=0;i<8;i++) _grid_pmts[i]=new int[_grid_nb_of_pmts[i]];
  for(int i=0;i<RICmaxpmts;i++){
    int g=_pmts[i].grid()-1;
    _grid_pmts[g][current[g]++]=i;
  }

}

void RichPMTsManager::ReadFromFile(const char *filename){
  cout<<"RichPMTsManager::ReadFromFile: reading calibration from file"<<endl;
  fstream calib(filename,ios::in); // open  file for reading
  
  if(calib){
    cout<<"RichPMTsManager::ReadFromFile: Local "<<filename<<" Opened"<<endl;
  }else{
    char newname[1024];
    strcpy(newname,AMSDATADIR.amsdatadir);
    strcat(newname,filename);
    if(calib){
      cout<<"RichPMTsManager::ReadFromFile: "<<filename<<" Opened"<<endl;
    }else{
      cerr <<"RichPMTsManager::ReadFromFile: missing "<<filename<<endl;
      exit(1);
    }
  }

  for(int geom_id=0;geom_id<RICmaxpmts;geom_id++){
      // Read the identification information
      int pos,pmtaddh,pmtaddc,pmtnumb;

      calib >> pos >> pmtaddh >> pmtaddc >> pmtnumb;

      // Use the information to search the geom id and check that everything
      // is correct
      int found=-1;
      for(int i=0;i<RICmaxpmts;i++){
	if(_pmts[i]._pos==pos)
	  if(pmtaddh != _pmts[i]._pmtaddh || pmtaddc!=_pmts[i]._pmtaddc || pmtnumb!=_pmts[i]._pmtnumb){
	    cerr<<"RichPMTsManager::ReadFromFile: Found pmt at pos "<<pos<<" but identification fails: "<<pmtaddh<<" vs "<<_pmts[i]._pmtaddh<<" -- "<<pmtaddc<<" vs "<<_pmts[i]._pmtaddc<<" -- "<<pmtnumb<<" -- "<<_pmts[i]._pmtnumb<<endl;
	    exit(1);
	  }else{
	    found=i;
	    break;
	  }
      }

      if(found==-1){
	cerr<<"RichPMTsManager::ReadFromFile: pos "<<pos<<" not found"<<endl;
	exit(1);
      }

      for(int window=0;window<RICnwindows;window++){
	// Read the status pedestals sigmas, thresholds gains
	int &pmt=found;
	int cat=_pmts[found]._channel_id2geom_id[window];
	
	calib >> _Status(pmt,cat) 
	      >> _Pedestal(pmt,cat,0) >> _Pedestal(pmt,cat,1) 
	      >> _PedestalSigma(pmt,cat,0) >> _PedestalSigma(pmt,cat,1) 
	      >> _PedestalThreshold(pmt,cat,0) >> _PedestalThreshold(pmt,cat,1)
	      >> _Gain(pmt,cat,0) >> _Gain(pmt,cat,1)
	      >> _GainSigma(pmt,cat,0) >> _GainSigma(pmt,cat,1)
	      >> _GainThreshold(pmt,cat)
	      >> _Eff(pmt,cat);

      
      }
  }
  calib.close();
}



void RichPMTsManager::SaveToFile(const char *filename){
  cout<<"RichPMTsManager::SaveToFile: writing calibration file"<<endl;
  fstream calib(filename,ios::out); // open  file for writing

  if(calib){
    cout<<"RichPMTsManager::SaveToFile: Local "<<filename<<" Opened"<<endl;
  }else{
    cerr <<"RichPMTsManager::SaveToFile: missing "<<filename<<endl;
    exit(1);
  }

  for(int geom_id=0;geom_id<RICmaxpmts;geom_id++){
      // Save the identification information
      calib << _pmts[geom_id]._pos << " " << _pmts[geom_id]._pmtaddh << " " << _pmts[geom_id]._pmtaddc << " " << _pmts[geom_id]._pmtnumb<<endl;

      for(int window=0;window<RICnwindows;window++){
	// Save the status pedestals sigmas, thresholds gains
	int cat=_pmts[geom_id]._channel_id2geom_id[window];	
	int &pmt=geom_id;
	calib << _Status(pmt,cat) 
	      << " " << _Pedestal(pmt,cat,0) << " " << _Pedestal(pmt,cat,1) 
	      << " " << _PedestalSigma(pmt,cat,0) << " " << _PedestalSigma(pmt,cat,1) 
	      << " " << _PedestalThreshold(pmt,cat,0) << " " << _PedestalThreshold(pmt,cat,1)
	      << " " << _Gain(pmt,cat,0) << " " << _Gain(pmt,cat,1)
	      << " " << _GainSigma(pmt,cat,0) << " " << _GainSigma(pmt,cat,1)
	      << " " << _GainThreshold(pmt,cat)
	      << " " << _Eff(pmt,cat) <<endl;

      
      }
  }

  calib.close();
}


void RichPMTsManager::get_eff_quantities(){
  _max_eff=0;
  _mean_eff=0;
  for(int i=0;i<RICmaxpmts;i++)
    for(int j=0;j<RICnwindows;j++){
      _mean_eff+=Eff(i,j);
      if(Eff(i,j)>_max_eff) _max_eff=(Eff(i,j));
    }

  _mean_eff/=RICmaxpmts*RICnwindows;
}

void RichPMTsManager::Finish(){
  Finish_Default();

  if(RICFFKEY.ReadFile/10){
    char name[200];
    
    UHTOC(RICFFKEY.fname_out,200,name,200);
    
    for(int i=199;i>=0;i--){
      if(name[i]!=' '){
	name[i+1]=0;
	break;
      }
    }
    
    /*
    if(strlen(name)==0){
      strcpy(name,"richcal");
      
      if(AMSJob::gethead()->isMCData()){ // McData 
	strcat(name,"mc");
      }else{  // Real data
	strcat(name,"dt");
      }
      
      
      strcat(name,".002");  // Version
    }
    */

    SaveToFile(name);
  }

}

void RichPMTsManager::Init_Default(){
#include "richid_default_values.h"  // Tables with default values of PMT information
  
  // Crate positioning is according to next drawing
  //
  //          8| 1 |2
  //          -+---+-
  //          7|   |3
  //          -+---+-
  //          6| 5 |4
  RICHDB::total=RICmaxpmts;  // This is used is other parts in the code
  for(int geom_id=0;geom_id<RICmaxpmts;geom_id++){
    RichPMT &pmt=_pmts[geom_id];

    // PMT id
    pmt._geom_id=geom_id;
    pmt._pos=id[geom_id];
    pmt._orientation=irot[geom_id];
    pmt._pmtaddh=pmtaddh[geom_id];
    pmt._pmtaddc=pmtaddc[geom_id];
    pmt._pmtnumb=pmtnumb[geom_id];

    // PMT position
    pmt._local_position[0]=pos_x[geom_id];
    pmt._local_position[1]=pos_y[geom_id];
    pmt._local_position[2]=0;

    pmt._global_position[0]=grid_x[pmt.grid()-1]+pmt._local_position[0];
    pmt._global_position[1]=grid_y[pmt.grid()-1]+pmt._local_position[1];
    pmt._global_position[2]=RICHDB::total_height()/2-RICHDB::pmt_pos();

    for(int i=0;i<RICnwindows;i++)
      if(pmt._orientation==0)
	pmt._channel_id2geom_id[i]=i;
      else
	pmt._channel_id2geom_id[i]=4*(i%4)+(3-(i/4));  // CHECK!!!!!!

    // Channel position, with Z in half the PMT
    for(int i=0;i<RICnwindows;i++){
      pmt._channel_position[i][0]=(2*(i%4)-3)*RICHDB::lg_length/8.0+pmt._global_position[0];
      pmt._channel_position[i][1]=(2*(i/4)-3)*RICHDB::lg_length/8.0+pmt._global_position[1];
      pmt._channel_position[i][2]=pmt._global_position[2];
     }
  }

  //
  // Fill the channels info
  //
  geant gain,gain_low;
  geant sigma_gain,sigma_gain_low;
  GETRMURSG(4.92,12.25,gain,sigma_gain);
  GETRMURSG(4.92,12.25/5.,gain_low,sigma_gain_low);

  for(int pmt=0;pmt<RICmaxpmts;pmt++){
      for(int cat=0;cat<RICnwindows;cat++){
	// New PMT simulation
	_Status(pmt,cat)=1;
	_Pedestal(pmt,cat,0)=0;
	_Pedestal(pmt,cat,1)=0;
	_PedestalSigma(pmt,cat,0)=4.;
	_PedestalSigma(pmt,cat,1)=4.;
	_PedestalThreshold(pmt,cat,0)=4.;  // In sigmas
	_PedestalThreshold(pmt,cat,1)=4.;
	_Gain(pmt,cat,1)=gain;
	_Gain(pmt,cat,0)=gain_low;
	_GainSigma(pmt,cat,1)=sigma_gain;
	_GainSigma(pmt,cat,0)=sigma_gain_low;
	_GainThreshold(pmt,cat)=3000;
	_Eff(pmt,cat)=1;
      }
  }
       

}


void RichPMTsManager::Finish_Default(){
}

integer RichPMTsManager::detcer(geant photen)
{
   integer upper=-1,i;
   
   for(i=1;i<RICHDB::entries;i++) 
     if(2*3.1415926*197.327e-9/RICHDB::wave_length[i]>=photen)
        {upper=i;break;}
   
   if(upper==-1) return 0;
   i=upper;

   geant xufact=RICHDB::eff[i]-RICHDB::eff[i-1];
   xufact/=2*3.1415926*197.327e-9*(1/RICHDB::wave_length[i]-1/RICHDB::wave_length[i-1]);

   geant deteff=RICHDB::eff[i-1]+(photen-2*3.1415926*197.327e-9/RICHDB::wave_length[i-1])*xufact;
   deteff*=_max_eff/_mean_eff;

   geant dummy=0;
   geant ru=RNDM(dummy);

   if(100*ru<deteff) return 1;
   return 0;
}

int RichPMTsManager::FindPMT(geant x,geant y){
  //
  // Search the right PMT using the grid tables
  //

  // Grid positioning is according to next drawing
  //
  //          8| 1 |2
  //          -+---+-
  //          7|   |3
  //          -+---+-
  //          6| 5 |4

  integer grid=-1;

  if(x>RICHDB::hole_radius[0]-RICpmtsupport/2.){
    if(y>RICHDB::hole_radius[1]-RICpmtsupport/2.) grid=2;
    else if(-y>RICHDB::hole_radius[1]-RICpmtsupport/2) grid=4;
    else grid=3;
  } else if(-x>RICHDB::hole_radius[0]-RICpmtsupport/2.){
    if(y>RICHDB::hole_radius[1]-RICpmtsupport/2.) grid=8;
    else if(-y>RICHDB::hole_radius[1]-RICpmtsupport/2) grid=6;
    else grid=7;
  }else{
    if(y>RICHDB::hole_radius[1]-RICpmtsupport/2.) grid=1;
    else if(-y>RICHDB::hole_radius[1]-RICpmtsupport/2) grid=5;
    else return -1;
  }


  assert(grid>0);

  grid--;
  int *index=_grid_pmts[grid];
  int n=_grid_nb_of_pmts[grid];
  
  int pmt=-1;

  for(int i=0;i<n;i++){
    double pmt_x=_pmts[index[i]]._global_position[0];
    double pmt_y=_pmts[index[i]]._global_position[1];

    if(fabs(x-pmt_x)<RICHDB::lg_length/2. && 
       fabs(y-pmt_y)<RICHDB::lg_length/2.){
      pmt=index[i];
      break;
    }
  }

  double pmt_x=_pmts[pmt]._global_position[0];
  double pmt_y=_pmts[pmt]._global_position[1];

  return pmt;
}


int RichPMTsManager::FindChannel(geant x,geant y){
  int geom_pmt=FindPMT(x,y);
  
  if(geom_pmt<0) return -1;
  
  RichPMT &pmt=RichPMTsManager::GetPMT(geom_pmt);

  int channel=-1;
  for(int i=0;i<RICnwindows;i++){
    if(fabs(x-pmt._channel_position[i][0])<RICHDB::lg_length/4./2. &&
       fabs(y-pmt._channel_position[i][1])<RICHDB::lg_length/4./2.){
      channel=i;break;
    }
  }

  if(channel<0) return -1;

  return PackGeom(geom_pmt,channel);
}

////////////////////
// Some accessors //
////////////////////

int RichPMTsManager::Status(int Geom_id,int Geom_Channel){
  if(Geom_id<0 || Geom_id>=RICmaxpmts || Geom_Channel<0 || Geom_Channel>=RICnwindows) return -1;
  return _status[RICnwindows*Geom_id+Geom_Channel];
}

geant RichPMTsManager::Pedestal(int Geom_id,int Geom_Channel,int low_gain){
  if(Geom_id<0 || Geom_id>=RICmaxpmts || Geom_Channel<0 || Geom_Channel>=RICnwindows || low_gain<0 || low_gain>1) return -1;
  return _pedestal[2*RICnwindows*Geom_id+RICnwindows*low_gain+Geom_Channel];
}

geant RichPMTsManager::PedestalSigma(int Geom_id,int Geom_Channel,int low_gain){
  if(Geom_id<0 || Geom_id>=RICmaxpmts || Geom_Channel<0 || Geom_Channel>=RICnwindows || low_gain<0 || low_gain>1) return -1;
  return _pedestal_sigma[2*RICnwindows*Geom_id+RICnwindows*low_gain+Geom_Channel];
}

geant RichPMTsManager::PedestalThreshold(int Geom_id,int Geom_Channel,int low_gain){
  if(Geom_id<0 || Geom_id>=RICmaxpmts || Geom_Channel<0 || Geom_Channel>=RICnwindows || low_gain<0 || low_gain>1) return -1;
  return _pedestal_threshold[2*RICnwindows*Geom_id+RICnwindows*low_gain+Geom_Channel];
}


geant RichPMTsManager::Gain(int Geom_id,int Geom_Channel,int low_gain){
  if(Geom_id<0 || Geom_id>=RICmaxpmts || Geom_Channel<0 || Geom_Channel>=RICnwindows || low_gain<0 || low_gain>1) return -1;
  return _gain[2*RICnwindows*Geom_id+RICnwindows*low_gain+Geom_Channel];
}


geant RichPMTsManager::GainSigma(int Geom_id,int Geom_Channel,int low_gain){
  if(Geom_id<0 || Geom_id>=RICmaxpmts || Geom_Channel<0 || Geom_Channel>=RICnwindows || low_gain<0 || low_gain>1) return -1;
  return _gain_sigma[2*RICnwindows*Geom_id+RICnwindows*low_gain+Geom_Channel];
}


int RichPMTsManager::GainThreshold(int Geom_id,int Geom_Channel){
  if(Geom_id<0 || Geom_id>=RICmaxpmts || Geom_Channel<0 || Geom_Channel>=RICnwindows) return -1;
  return _gain_threshold[RICnwindows*Geom_id+Geom_Channel];
}



geant RichPMTsManager::Eff(int Geom_id,int Geom_Channel){
  if (Geom_id<0 || Geom_id>=RICmaxpmts || Geom_Channel<0 || Geom_Channel>=RICnwindows) return -1;
  return _relative_efficiency[RICnwindows*Geom_id+Geom_Channel];
}



int& RichPMTsManager::_Status(int Geom_id,int Geom_Channel){
  assert(!(Geom_id<0 || Geom_id>=RICmaxpmts || Geom_Channel<0 || Geom_Channel>=RICnwindows));
  return _status[RICnwindows*Geom_id+Geom_Channel];
}

geant& RichPMTsManager::_Pedestal(int Geom_id,int Geom_Channel,int low_gain){
  assert(!(Geom_id<0 || Geom_id>=RICmaxpmts || Geom_Channel<0 || Geom_Channel>=RICnwindows || low_gain<0 || low_gain>1));
  return _pedestal[2*RICnwindows*Geom_id+RICnwindows*low_gain+Geom_Channel];
}

geant& RichPMTsManager::_PedestalSigma(int Geom_id,int Geom_Channel,int low_gain){
  assert(!(Geom_id<0 || Geom_id>=RICmaxpmts || Geom_Channel<0 || Geom_Channel>=RICnwindows || low_gain<0 || low_gain>1));
  return _pedestal_sigma[2*RICnwindows*Geom_id+RICnwindows*low_gain+Geom_Channel];
}

geant& RichPMTsManager::_PedestalThreshold(int Geom_id,int Geom_Channel,int low_gain){
  assert(!(Geom_id<0 || Geom_id>=RICmaxpmts || Geom_Channel<0 || Geom_Channel>=RICnwindows || low_gain<0 || low_gain>1));
  return _pedestal_threshold[2*RICnwindows*Geom_id+RICnwindows*low_gain+Geom_Channel];
}


geant& RichPMTsManager::_Gain(int Geom_id,int Geom_Channel,int low_gain){
  assert(!(Geom_id<0 || Geom_id>=RICmaxpmts || Geom_Channel<0 || Geom_Channel>=RICnwindows || low_gain<0 || low_gain>1));
  return _gain[2*RICnwindows*Geom_id+RICnwindows*low_gain+Geom_Channel];
}


geant& RichPMTsManager::_GainSigma(int Geom_id,int Geom_Channel,int low_gain){
  assert(!(Geom_id<0 || Geom_id>=RICmaxpmts || Geom_Channel<0 || Geom_Channel>=RICnwindows || low_gain<0 || low_gain>1));
  return _gain_sigma[2*RICnwindows*Geom_id+RICnwindows*low_gain+Geom_Channel];
}


int& RichPMTsManager::_GainThreshold(int Geom_id,int Geom_Channel){
  assert(!(Geom_id<0 || Geom_id>=RICmaxpmts || Geom_Channel<0 || Geom_Channel>=RICnwindows));
  return _gain_threshold[RICnwindows*Geom_id+Geom_Channel];
}


geant& RichPMTsManager::_Eff(int Geom_id,int Geom_Channel){
  assert(!(Geom_id<0 || Geom_id>=RICmaxpmts || Geom_Channel<0 || Geom_Channel>=RICnwindows));
  return _relative_efficiency[RICnwindows*Geom_id+Geom_Channel];
}


int RichPMTsManager::GetGeomPMTID(int pos){
  for(int i=0;i<RICmaxpmts;i++) if(_pmts[i]._pos=pos) return i;
  return -1;
}

int RichPMTsManager::GetGeomChannelID(int pos,int pixel){
  return _pmts[pos]._channel_id2geom_id[pixel];
}


void RichPMTsManager::GetGeomID(int pos,int pixel,int &geom_pos,int &geom_pix){
  geom_pos=GetGeomPMTID(pos);
  if(geom_pos==-1 || pixel<0 || pixel>15) geom_pix==-1;
  else geom_pix=_pmts[geom_pos]._channel_id2geom_id[pixel];
}


/////////////////////////////////////
// RichPMT initialization routines //
/////////////////////////////////////



void RichPMT::compute_tables(){
  for(int channel=0;channel<RICnwindows;channel++)
    for(int mode=0;mode<2;mode++){
      geant gain=RichPMTsManager::Gain(_geom_id,channel,mode);
      geant gain_sigma=RichPMTsManager::GainSigma(_geom_id,channel,mode);

      // Check that the table have not been previously computed
      // (save time if using the default calibration)
      int done=0;
      for(int i=0;i<_geom_id;i++){
	for(int j=0;j<(i==_geom_id)?channel:RICnwindows;j++){
	  if(gain==RichPMTsManager::Gain(i,j,mode) && gain_sigma==RichPMTsManager::GainSigma(i,j,mode)){
	    done=1;
	    memcpy(_cumulative_prob[channel][mode],RichPMTsManager::GetPMT(i)._cumulative_prob[j][mode],sizeof(geant)*RIC_prob_bins);
	    _step[channel][mode]=RichPMTsManager::GetPMT(i)._step[j][mode];
	    break;
	  }
	}
	if(done) break;
      }

      if(done) continue;

      geant lambda,scale;
      GETRLRS(gain,gain_sigma,lambda,scale);

      geant upper_limit=gain*10;                          // 10 photoelectrons
      _step[channel][mode]=upper_limit/RIC_prob_bins;
      
      // Use the trapezoid rule to compute the integral
      _cumulative_prob[channel][mode][0]=0;
      for(int i=1;i<RIC_prob_bins;i++){
	float value,x1,x2;
	
	x1=i*_step[channel][mode];
	x2=(i-1)*_step[channel][mode];
	value=PDENS(x1,lambda,scale)+PDENS(x2,lambda,scale);
	value*=_step[channel][mode]/2;
	_cumulative_prob[channel][mode][i]=_cumulative_prob[channel][mode][i-1]+value;
      }
      
      for(int i=0;i<RIC_prob_bins;i++)
	_cumulative_prob[channel][mode][i]/=
	  _cumulative_prob[channel][mode][RIC_prob_bins-1];
    }

}

////////////////////////
// RichPMT simulation //
////////////////////////

geant RichPMT::SimulateSinglePE(int channel,int mode){
  // Sample from the _cumulative_prob for _address and mode
  geant dummy=0;
  geant value=RNDM(dummy);
  for(int i=0;i<RIC_prob_bins;i++){
    if(value<=_cumulative_prob[channel][mode][i])
      return i*_step[channel][mode];
  }
}

#endif
