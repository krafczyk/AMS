#include"richid.h"
#include<fstream> 

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
short int RichPMTsManager::_rdr_starts[RICH_JINFs*RICH_CDPperJINF];
short int RichPMTsManager::_rdr_pmt_count[RICH_JINFs*RICH_CDPperJINF];


void RichPMTsManager::Init(char *filename){
//#pragma omp barrier 
//#pragma omp master
  {
    Init_Default();
    
    
    // Read the default 
    //    char name[801];
    //    sprintf(name,"RichDefaultCalibration.dat");
    ReadFromFile(filename);
    
    
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
//#pragma omp barrier 
}


//
// Calibration file format
//
// pmtpos_number
// status pedestal_low_gain pedestal_high_gain ped_sigma_low ped_sigma_high ped_threshold_low ped_threshol_high gain_low gain_high gain_sigma_low gain_sigma_high gain_threshold relative_eff : pixel 0 (hadr id)
// status pedestal_low_gain pedestal_high_gain ped_sigma_low ped_sigma_high ped_threshold_low ped_threshol_high gain_low gain_high gain_sigma_low gain_sigma_high gain_threshold relative_eff : pixel 1 (hadr id)
// ... up to pixel 15
// pmtpos_number
// ...



void RichPMTsManager::ReadFromFile(const char *filename){
  cout<<"RichPMTsManager::ReadFromFile: reading calibration from file"<<endl;
  fstream calib(filename,ios::in); // open  file for reading
  
  if(calib){
    cout<<"RichPMTsManager::ReadFromFile: "<<filename<<" Opened"<<endl;
  }else{
    cerr <<"RichPMTsManager::ReadFromFile: missing "<<filename<<endl;
    exit(1);
  }

  for(int geom_id=0;geom_id<RICmaxpmts;geom_id++){
      // Read the identification information
      int pos,pmtaddh,pmtaddc,pmtnumb;

      //      calib >> pos >> pmtaddh >> pmtaddc >> pmtnumb;
      calib >> pos;

      // Use the information to search the geom id and check that everything
      // is correct
      int found=-1;
      for(int i=0;i<RICmaxpmts;i++){
	if(_pmts[i]._pos==pos)
	  //	  if(pmtaddh != _pmts[i]._pmtaddh || pmtaddc!=_pmts[i]._pmtaddc || pmtnumb!=_pmts[i]._pmtnumb){
	  //	    cerr<<"RichPMTsManager::ReadFromFile: Found pmt at pos "<<pos<<" but identification fails: "<<pmtaddh<<" vs "<<_pmts[i]._pmtaddh<<" -- "<<pmtaddc<<" vs "<<_pmts[i]._pmtaddc<<" -- "<<pmtnumb<<" -- "<<_pmts[i]._pmtnumb<<endl;
	  //	    exit(1);
	  //	  }else{
	  //	    found=i;
	  //	    break;
	  //	  }
	  {found=i;break;}
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

	// Apply some consistent checks and recover them for simulation
	if(_Status(pmt,cat)){  // It is a good channel accordingly to calibration tables
	  if(_GainSigma(pmt,cat,0)<=0) _GainSigma(pmt,cat,0)=_Gain(pmt,cat,0)*0.5;
	  if(_GainSigma(pmt,cat,1)<=0) _GainSigma(pmt,cat,1)=_Gain(pmt,cat,1)*0.5;
	}


      }
  }
  calib.close();
}



void RichPMTsManager::SaveToFile(const char *filename){
#pragma omp single
  {
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
      //      calib << _pmts[geom_id]._pos << " " << _pmts[geom_id]._pmtaddh << " " << _pmts[geom_id]._pmtaddc << " " << _pmts[geom_id]._pmtnumb<<endl;
      calib << _pmts[geom_id]._pos<<endl;
      
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
//#pragma omp barrier 
//#pragma omp master
  {
    Finish_Default();
    
  }
//#pragma omp barrier 
}

void RichPMTsManager::Init_Default(){
#include "richid_default_values.h"  // Tables with default values of PMT information
  
  // Crate positioning is according to next drawing
  //
  //          8| 1 |2
  //          -+---+-
  //          7|   |3   <-> x->-x and y->-y accrodingly to cosmic runs 
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
    pmt._local_position[0]=-pos_x[geom_id];  // Swap corrected
    pmt._local_position[1]=-pos_y[geom_id];  // Swap corrected
    pmt._local_position[2]=0;

    pmt._global_position[0]=-grid_x[pmt.grid()-1]+pmt._local_position[0]; //Swap corrected
    pmt._global_position[1]=-grid_y[pmt.grid()-1]+pmt._local_position[1]; //Swap corrected
    pmt._global_position[2]=RICHDB::total_height()/2-RICHDB::pmt_pos();


    // CHECK THIS!!!! 
    for(int i=0;i<RICnwindows;i++)
      if(pmt._orientation==0)
	//	pmt._channel_id2geom_id[i]=i;
	pmt._channel_id2geom_id[i]=15-(i%4)-(i/4)*4;  // Swap corrected
      else
	//	pmt._channel_id2geom_id[i]=15-(i%4)-(i/4)*4;
	pmt._channel_id2geom_id[i]=i; // Swap corrected

    // Channel position, with Z in half the PMT
    for(int i=0;i<RICnwindows;i++){
      pmt._channel_position[i][0]=(2*(i%4)-3)*RICHDB::lg_length/8.0+pmt._global_position[0];
      pmt._channel_position[i][1]=(2*(i/4)-3)*RICHDB::lg_length/8.0+pmt._global_position[1];
      pmt._channel_position[i][2]=pmt._global_position[2];
    }

    /*
    // TEMPORAL FIX
    // Correct for x<->-x and y<->-y swapping
    pmt._local_position[0]*=-1;
    pmt._local_position[1]*=-1;
    pmt._global_position[0]*=-1;
    pmt._global_position[1]*=-1;
    for(int i=0;i<RICnwindows;i++){
      // THIS HAVE TO BE CHECKED
      pmt._channel_position[i][0]*=-1;
      pmt._channel_position[i][1]*=-1;
    }
    */

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
	_Gain(pmt,cat,0)=gain_low;
	_Gain(pmt,cat,1)=gain;
	_GainSigma(pmt,cat,1)=sigma_gain;
	_GainSigma(pmt,cat,0)=sigma_gain_low;
	_GainThreshold(pmt,cat)=3000;
	_Eff(pmt,cat)=1;
      }
  }


  // Initialize tables to get the pmt position from rdr number and pmt number
  for(int i=0;i<RICH_JINFs*RICH_CDPperJINF;i++) {_rdr_starts[i]=0;_rdr_pmt_count[i]=0;} 

  int last=-1;
  int position=0;
  for(int i=0;i<RICmaxpmts;i++)
    if(pmtaddh[i]!=last){
      _rdr_starts[position++]=i;
      last=pmtaddh[i];
    }
  
  for(int i=0;i<position;i++){
    int count=0; 
    for(int j=_rdr_starts[i];pmtaddh[_rdr_starts[i]]==pmtaddh[j];j++){
      count++;
    }
    _rdr_pmt_count[i]=count;
  }

#ifdef __AMSDEBUG__

  for(int i=0;i<RICH_JINFs*RICH_CDPperJINF;i++){
    cout<<"CDP: "<<i<<"   Geom PMTid: "<<_rdr_starts[i]<<"  Number of PMTs: "<<_rdr_pmt_count[i]<<endl; 
  }

#endif

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
   geant ru=0;

   if(100*ru<deteff) return 1;
   return 0;
}

int RichPMTsManager::FindPMT(geant x,geant y){
  //
  // Search the right PMT using the grid tables
  //

  // Grid positioning is according to next drawing
  //
  //          8| 1 |2                           4| 5 |6
  //          -+---+-
  //          7|   |3  -> DUE TO SWAPPING ->    3|   |7  
  //          -+---+-
  //          6| 5 |4                           2| 1 |8

  integer grid=-1;

  if(x>RICHDB::hole_radius[0]-RICpmtsupport/2.){
    if(y>RICHDB::hole_radius[1]-RICpmtsupport/2.) grid=6;//grid=2;
    else if(-y>RICHDB::hole_radius[1]-RICpmtsupport/2) grid=8;//grid=4;
    else grid=7;//grid=3;
  } else if(-x>RICHDB::hole_radius[0]-RICpmtsupport/2.){
    if(y>RICHDB::hole_radius[1]-RICpmtsupport/2.) grid=4;//grid=8;
    else if(-y>RICHDB::hole_radius[1]-RICpmtsupport/2) grid=2;//grid=6;
    else grid=3;//grid=7;
  }else{
    if(y>RICHDB::hole_radius[1]-RICpmtsupport/2.) grid=5;//grid=1;
    else if(-y>RICHDB::hole_radius[1]-RICpmtsupport/2) grid=1;//grid=5;
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
  
  if(pmt==-1) return -1;
  
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

geant RichPMTsManager::Pedestal(int Geom_id,int Geom_Channel,int high_gain){
  if(Geom_id<0 || Geom_id>=RICmaxpmts || Geom_Channel<0 || Geom_Channel>=RICnwindows || high_gain<0 || high_gain>1) return -1;
  return _pedestal[2*RICnwindows*Geom_id+RICnwindows*high_gain+Geom_Channel];
}

geant RichPMTsManager::PedestalSigma(int Geom_id,int Geom_Channel,int high_gain){
  if(Geom_id<0 || Geom_id>=RICmaxpmts || Geom_Channel<0 || Geom_Channel>=RICnwindows || high_gain<0 || high_gain>1) return -1;
  return _pedestal_sigma[2*RICnwindows*Geom_id+RICnwindows*high_gain+Geom_Channel];
}

geant RichPMTsManager::PedestalThreshold(int Geom_id,int Geom_Channel,int high_gain){
  if(Geom_id<0 || Geom_id>=RICmaxpmts || Geom_Channel<0 || Geom_Channel>=RICnwindows || high_gain<0 || high_gain>1) return -1;
  return _pedestal_threshold[2*RICnwindows*Geom_id+RICnwindows*high_gain+Geom_Channel];
}


geant RichPMTsManager::Gain(int Geom_id,int Geom_Channel,int high_gain){
  if(Geom_id<0 || Geom_id>=RICmaxpmts || Geom_Channel<0 || Geom_Channel>=RICnwindows || high_gain<0 || high_gain>1) return -1;
  return _gain[2*RICnwindows*Geom_id+RICnwindows*high_gain+Geom_Channel];
}


geant RichPMTsManager::GainSigma(int Geom_id,int Geom_Channel,int high_gain){
  if(Geom_id<0 || Geom_id>=RICmaxpmts || Geom_Channel<0 || Geom_Channel>=RICnwindows || high_gain<0 || high_gain>1) return -1;
  return _gain_sigma[2*RICnwindows*Geom_id+RICnwindows*high_gain+Geom_Channel];
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

geant& RichPMTsManager::_Pedestal(int Geom_id,int Geom_Channel,int high_gain){
  assert(!(Geom_id<0 || Geom_id>=RICmaxpmts || Geom_Channel<0 || Geom_Channel>=RICnwindows || high_gain<0 || high_gain>1));
  return _pedestal[2*RICnwindows*Geom_id+RICnwindows*high_gain+Geom_Channel];
}

geant& RichPMTsManager::_PedestalSigma(int Geom_id,int Geom_Channel,int high_gain){
  assert(!(Geom_id<0 || Geom_id>=RICmaxpmts || Geom_Channel<0 || Geom_Channel>=RICnwindows || high_gain<0 || high_gain>1));
  return _pedestal_sigma[2*RICnwindows*Geom_id+RICnwindows*high_gain+Geom_Channel];
}

geant& RichPMTsManager::_PedestalThreshold(int Geom_id,int Geom_Channel,int high_gain){
  assert(!(Geom_id<0 || Geom_id>=RICmaxpmts || Geom_Channel<0 || Geom_Channel>=RICnwindows || high_gain<0 || high_gain>1));
  return _pedestal_threshold[2*RICnwindows*Geom_id+RICnwindows*high_gain+Geom_Channel];
}


geant& RichPMTsManager::_Gain(int Geom_id,int Geom_Channel,int high_gain){
  assert(!(Geom_id<0 || Geom_id>=RICmaxpmts || Geom_Channel<0 || Geom_Channel>=RICnwindows || high_gain<0 || high_gain>1));
  return _gain[2*RICnwindows*Geom_id+RICnwindows*high_gain+Geom_Channel];
}


geant& RichPMTsManager::_GainSigma(int Geom_id,int Geom_Channel,int high_gain){
  assert(!(Geom_id<0 || Geom_id>=RICmaxpmts || Geom_Channel<0 || Geom_Channel>=RICnwindows || high_gain<0 || high_gain>1));
  return _gain_sigma[2*RICnwindows*Geom_id+RICnwindows*high_gain+Geom_Channel];
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

int RichPMTsManager::GetPMTID(int geom_id){
  if(geom_id<0 || geom_id>=RICmaxpmts) return -1;
  return _pmts[geom_id]._pos;
}

int RichPMTsManager::GetGeomChannelID(int pos,int pixel){
  return _pmts[pos]._channel_id2geom_id[pixel];
}

int RichPMTsManager::GetChannelID(int pmtgeom,int pixel){
  for(int i=0;i<RICnwindows;i++) if(_pmts[pmtgeom]._channel_id2geom_id[i]==pixel) return i;
  return -1;
}

int RichPMTsManager::GetGeomPMTIdFromCDP(int CDP,int pmt){
  assert(CDP>=0 && CDP<RICH_CDPperJINF*RICH_JINFs);
#ifdef __AMSDEBUG__
  //cout<<"Count is "<<_rdr_pmt_count[CDP]<<" CDP "<<CDP<<" pmt "<<pmt<<endl;
#endif
  if(pmt<0 || pmt>=_rdr_pmt_count[CDP]) return -1;
  return _rdr_starts[CDP]+pmt;
}


int RichPMTsManager::GetCDPFromGeomPMTId(int pmt,int &pmt_cdp){
  pmt_cdp=-1;
  for(int CDP=0;CDP<RICH_CDPperJINF*RICH_JINFs;CDP++)
    if(pmt>=_rdr_starts[CDP] && pmt<_rdr_starts[CDP]+_rdr_pmt_count[CDP]){
      pmt_cdp=pmt-_rdr_starts[CDP];
      return CDP;
    }
  return -1;
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
      /*
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
      */

      geant lambda,scale;
      GETRLRS(gain,gain_sigma,lambda,scale);

      // In some case the function does not return meaningfull values. Since this
      // is only used int MC simulation, we correct it by allowing a smales sigma_gain
      if(lambda<=0 || scale<=0){
	GETRLRS(gain,gain*0.5,lambda,scale);
      }

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
      
      for(int i=0;i<RIC_prob_bins;i++){
	if(_cumulative_prob[channel][mode][RIC_prob_bins-1]>0)
	  _cumulative_prob[channel][mode][i]/=
	    _cumulative_prob[channel][mode][RIC_prob_bins-1];
	else
	  _cumulative_prob[channel][mode][i]=(i==0?1:0);
      }
    }
}

////////////////////////
// RichPMT simulation //
////////////////////////


geant RichPMT::SimulateSinglePE(int channel,int mode){
  // Sample from the _cumulative_prob for _address and mode
  geant dummy=0;
  geant value=0;

  //  return BSearch(channel,mode,value)*_step[channel][mode]; // Does not work!!! Fix!
  
  for(int i=0;i<RIC_prob_bins;i++){
    if(value<=_cumulative_prob[channel][mode][i])
      return i*_step[channel][mode];
  }
  
  return RIC_prob_bins*_step[channel][mode];  // Just in case
}


int RichPMT::BSearch(int channel,int mode,geant value){
  int upper=RIC_prob_bins-1;
  int lower=0;

  while(upper-lower>1){
    int middle=(upper+lower)>>1;
    if(_cumulative_prob[channel][mode][middle]>value) upper=middle;
    else if(_cumulative_prob[channel][mode][middle]<value) lower=middle;
    else break;
  }

  return upper;

}
