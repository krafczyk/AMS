#include "richdaq.h"
#include "richid.h"
#include "richrec.h"
#include "timeid.h"

//////////////////////////////////////////////////
// RICH CDP are labelled from 0 to 23 (24 CDPs)

integer DAQRichBlock::_Calib[2][24]={
    { 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0},
    { 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0}
};

int DAQRichBlock::JINFId[RICH_JINFs]={10,11};           // JINFR0 and JINFR1  (link identification from JINJ)
int DAQRichBlock::FirstNode=242;
int DAQRichBlock::LastNode=265;
TH1F **DAQRichBlock::daq_histograms=0;

// Now a table giving, for each pair RICH_JINF link and RDR link, which physical
// RDR it is (counting from 0 clockwise, starting from crate 1) 
int DAQRichBlock::Links[RICH_JINFs][RICH_LinksperJINF]=   // Table of links (really ports) -> given the JINF (0 ot 1 corresponding to ports 10 and 11) and the link number, you get the
                                                          // physical CDP number  
  {
//    0  1  2  3  4  5  6  7  8  9 10 11 12 13 14 15 16 17 18 19 20 21 22 23
    { 7, 5,-1,-1, 6, 9,-1,-1,10,11,-1,-1, 8, 2,-1,-1, 1, 0,-1,-1, 3, 4,-1,-1},  // JINF-R-0-P
    {19,23,-1,-1,18,21,-1,-1,22,17,-1,-1,20,14,-1,-1,13,12,-1,-1,15,16,-1,-1}   // JINF-R-1-P
  };
int DAQRichBlock::Status=DAQRichBlock::kOk;

integer DAQRichBlock::getdaqid(int16u crate){
 return crate<2?JINFId[crate]:-1;
}
integer DAQRichBlock::checkdaqid(int16u id){   // RICH AS PORTS
  // Take a block id and return if the block is a rich block
  for(int i=0;i<RICH_JINFs;i++) if(id==JINFId[i]) return 1;   // It comes through one of the ports (2 JINF for rich)
  return 0;
}


void DAQRichBlock::buildraw(integer length,int16u *p){
  // it is implied that event_fragment is RICH-JINF's one (validity is checked in calling procedure)
  // *p=pointer_to_beggining_of_block_data(i.e. NEXT to len-word !!!)
  // Length counting does not include length-word itself !!!                                              

  // Reset the status bits
  Status=kOk;
  
  // Check that the length size is the correct one: at least I expect to have the node status
  if(length<1) Do(kLengthError);
  
  // This is supposed to be a JINF block: get the status word and check it
  StatusParser status(*(p-1+length));
  // if(!status.isData || status.errors || status.isCDP) Do(kDataError);
  if(status.errors || status.isCDP) Do(kDataError);
  
  // Get the JINF number
  uint id=status.slaveId;  
  int JINF=-1;
  for(int i=0;i<RICH_JINFs;i++) if(JINFId[i]==id) {JINF=i;break;}
  if(JINF==-1) Do(kJINFIdError);
  
  // Go through each CDP
  int16u *pointer=p;
  int CDPFound;          // Counter for the number of CDP found
  
  int low_gain[RICH_PMTperCDP][RICnwindows];       // Buffer to store low gain information just in case it is needed
  for(CDPFound=0;;CDPFound++){
    if(pointer>=p-1+length) break;   // Last fragment processed       
    
    FragmentParser cdp(pointer);
    if(cdp.status.errors) Do(kCDPError);
    
    int CDP=cdp.status.slaveId;
    
    // Process data
    if(!cdp.status.isRaw && !cdp.status.isCompressed) Do(kCalibration);
    
    if(cdp.status.isRaw){

      // Prepare some nice histograms
      if(daq_histograms==0){
	daq_histograms=new TH1F*[RICmaxpmts*RICnwindows*2];   // pixels*gains*pmts
	for(int pmt=0;pmt<RICmaxpmts;pmt++)
	  for(int pixel=0;pixel<RICnwindows;pixel++)
	    for(int gain=0;gain<2;gain++){
	      char histo_name[1024];
	      sprintf(histo_name,"Rich_PMT_%i_pixel_%i_gain_%i",pmt,pixel,gain);
	      daq_histograms[gain+2*pixel+2*RICnwindows*pmt]=new TH1F(histo_name,histo_name,4096,-0.5,4095.5);
	    }
      }

      if(cdp.length!=1+         // Status word
	 RICH_PMTperCDP*RICnwindows*2) Do(kCDPRawTruncated);

      // Fill raw information
      // Simple loop getting all the information

      DSPRawParser channel(cdp.data);

      do{
	// Fill raw histograms
//	daq_histograms[channel.gain+2*channel.pixel+2*RICnwindows*channel.pmt]->Fill(channel.counts);

	// First comes the low gain, the high gain then
	if(channel.gain==0) {low_gain[channel.pmt][channel.pixel]=channel.counts;}
	else{
	  // Get the geom ID for this channel
	  int physical_cdp=Links[JINF][CDP];

	  if(physical_cdp!=-1){
	    int geom_id=RichPMTsManager::GetGeomPMTIdFromCDP(physical_cdp,channel.pmt);
	    if(geom_id>=0){
	      // Get the pixel geom id, substract the pedestal, check that
	      // it is above it and use low gain if necessary
	      int pixel_id=RichPMTsManager::GetGeomChannelID(geom_id,channel.pixel);
	      
	      if(RichPMTsManager::Status(geom_id,pixel_id)%10==Status_good_channel){  // Channel is OK
		int mode=1;                 // High gain
		int counts=channel.counts;
		
		if(channel.counts>RichPMTsManager::GainThreshold(geom_id,pixel_id)){
		  counts=low_gain[channel.pmt][channel.pixel];
		  mode=0;
		}

		float threshold=RichPMTsManager::PedestalThreshold(geom_id,pixel_id,mode)*
		  RichPMTsManager::PedestalSigma(geom_id,pixel_id,mode);
		
		// Add the hit
		int channel_geom_number=RichPMTsManager::PackGeom(geom_id,pixel_id);
		counts-=int(RichPMTsManager::Pedestal(geom_id,pixel_id,mode));
		
		if(!cdp.status.isCompressed)  	   // Just in case it is Mixed mode			
		  if(counts>threshold || mode==0)
		  AMSEvent::gethead()->
		    addnext(
			    AMSID("AMSRichRawEvent",0),
			    new AMSRichRawEvent(channel_geom_number,
						counts,
						mode?0:gain_mode)
			    );
	      }
	    }
	  }
	}
      }while(channel.Next());

    }
  
    if(cdp.status.isCompressed){
      //      if(cdp.status.isRaw)Do(kMixedMode); // Nothing to do indeed
      // Fill compressed mode if something to fill 
      if(cdp.length-1>0){
	DSPCompressedParser channel(cdp.data,cdp.length-1);
	
	do{
	  int physical_cdp=Links[JINF][CDP];
	  //	    assert(physical_cdp!=-1);
	  if(physical_cdp!=-1){
	    int geom_id=RichPMTsManager::GetGeomPMTIdFromCDP(physical_cdp,channel.pmt);
	    
	    if(geom_id<0) Do(kWrongCDPChannelNumber);
	    
	    // Get the pixel geom id, substract the pedestal, check that
	    // it is above it and use low gain if necessary
	    int pixel_id=RichPMTsManager::GetGeomChannelID(geom_id,channel.pixel);
	    
	    if(RichPMTsManager::Status(geom_id,pixel_id)%10==Status_good_channel){  // Channel is OK
	      int mode=channel.gain;                 // High gain
	      int counts=channel.counts;
	      int channel_geom_number=RichPMTsManager::PackGeom(geom_id,pixel_id);
	      
	      AMSEvent::gethead()->addnext(AMSID("AMSRichRawEvent",0),
					   new AMSRichRawEvent(channel_geom_number,
							       counts,
							       mode?0:gain_mode));
	      
	    }
	  }
	}while(channel.Next());
	
      }
    }
    pointer=cdp.next;
  }
  

}



// Deal with errors with this function. Return 1 if exit needed, 0 otherwise
int DAQRichBlock::Emit(int code){
  static long error_counter=0;
  Status=code;

  if(error_counter==10){
    cerr<<"DAQRichBlock::buildraw -- Too many errors. Neglecting output"<<endl;
  }


  switch(Status)
    {
    case kOk:
      return 0; break;
    case kLengthError:
      if(error_counter++<10)
	cerr<<"DAQRichBlock::buildraw -- Fragment length too short"<<endl;
      return 1; break;
    case kDataError:
      if(error_counter++<10)
	cerr<<"DAQRichBlock::buildraw -- Status does not agree with a JINF fragment"<<endl;
      return 1; break;
    case kJINFError:
      if(error_counter++<10)
	cerr<<"DAQRichBlock::buildraw -- JINF Id int status word does not belong to RICH"<<endl;
      return 1; break;
    case kCDPError:
      if(error_counter++<10)
	cerr<<"DAQRichBlock::buildraw -- CDP status word flag errors"<<endl;
      return 1; break;
    case kCalibration:
      if(error_counter++<10)
	cerr<<"DAQRichBlock::buildraw -- CDP Calibration mode processing is not yet implemented"<<endl;
      return 1; break;
    case kMixedMode:
      if(error_counter++<10)
	cerr<<"DAQRichBlock::buildraw -- CDP Mixed mode: will ignore compressed data"<<endl;
      return 0; break;
    case kTruncated:
      if(error_counter++<10)
	cerr<<"DAQRichBlock::buildraw -- JINF information seems to be truncated: not enough CDPs. Ignored"<<endl;
      return 0; break;
    case kCDPRawTruncated:
      if(error_counter++<10)
	cerr<<"DAQRichBlock::buildraw -- CDP information seems to be truncated."<<endl;
      return 1; break;
    case kWrongCDPChannelNumber:
      if(error_counter++<10)
	cerr<<"DAQRichBlock::buildraw -- CDP in reduced mode refers to an inexistent channel."<<endl;
      return 0; break;
    default:
      return 0;break;
    }
} 

void DAQRichBlock::DSPRawParser::parse(){
  const int PMTs=RICH_PMTperCDP;
  const int channels=RICH_PMTperCDP*RICnwindows;

  pmt=_current_record%PMTs;
  pixel=_current_record/PMTs;
  gain=_current_record%channels;
  int16u *pointer=_root+_current_record;
  counts=*pointer;
}


void DAQRichBlock::DSPCompressedParser::parse(){
  const int PMTs=RICH_PMTperCDP;

  int16u channelid=*(_root+_current_record);
  int16u data=*(_root+_current_record+1);
  pmt=channelid%PMTs;
  pixel=channelid/PMTs;
  gain=data&0x1000?1:0;                    // This is reversed: no bit= high gain (in all the s/w gain mode=1 is high gain)
  counts=data&0x0FFF;
}


integer DAQRichBlock::checkcalid(int16u id){
  // Take a block id and return if the block is a rich block
  if( ((id>>5)&((1<<9)-1))>=FirstNode && ((id>>5)&((1<<9)-1))<=LastNode ) return 1;
  return 0;
}


void DAQRichBlock::buildcal(integer length,int16u *p){
  // This is calibration event for RDR
  // Length is in words
  const int block_size=2483;  // The length in words of the calibration table for a single RDR
  const int table_size=RICH_PMTperCDP*RICnwindows;; // 496 entries per table

  if(length!=block_size) return;

  p-=1;  // Go to the true starting point of the data

  // General checks
  int16u id=*(p-1);
  int16u node_type=id&31;
  if(node_type!=0x14) return;
  int16u status=*(p-2+length-1);
  if(status!=0x120) return;

  int16u node_number=((id>>5)&((1<<9)-1));
  int physical_cdp=node_number-FirstNode;
  cout<<"DAQRichBlock::buildcal -- found calibration tables for RDR-"<<physical_cdp/12<<"-"<<physical_cdp<<endl;
  for(int i=0;i<2;i++){
    for (int j=0;j<24;j++){
      if(Links[i][j]==physical_cdp){
	_Calib[i][j]=1;
	break;
      }
    }
  }

  // Check if we really want to read these tables
  for(int i=0;i<table_size;i++){
    // Get the address
    int pmt=i%RICH_PMTperCDP;
    int pixel=i/RICH_PMTperCDP;
    // Get the geometric information
    int pmt_geom_id=RichPMTsManager::GetGeomPMTIdFromCDP(physical_cdp,pmt);
    if(pmt_geom_id<0) continue;
    int pixel_geom_id=RichPMTsManager::GetGeomChannelID(pmt_geom_id,pixel);

    int pedx1=*(p+i);
    int pedx5=*(p+i+table_size);
    int thresholdx5=*(p+i+2*table_size)-pedx5;
    float sigma_pedx5=float(*(p+i+3*table_size))/1024;
    float sigma_pedx1=sigma_pedx5*pedx1/pedx5;
    int status=*(p+i+4*table_size);
    status=status&0xc000?0:1;       // This is the right computation

    if(sigma_pedx5==0){
      thresholdx5=4096;
      status=0;
    }else{
      thresholdx5=int(thresholdx5/sigma_pedx5+0.5);
    }
    int thresholdx1=thresholdx5;

    //    RichPMTChannel channel(pmt_geom_id,pixel_geom_id);
#ifdef __AMSDEBUG__
    cout<<"PEDx1 "<<RichPMTsManager::_Pedestal(pmt_geom_id,pixel_geom_id,0)<<" -- "<<pedx1<<endl
	<<"PEDx5 "<<RichPMTsManager::_Pedestal(pmt_geom_id,pixel_geom_id,1)<<" -- "<<pedx5<<endl
	<<"SIGMAx1 "<<RichPMTsManager::_PedestalSigma(pmt_geom_id,pixel_geom_id,0)<<" -- "<<sigma_pedx1<<endl
	<<"SIGMAx5 "<<RichPMTsManager::_PedestalSigma(pmt_geom_id,pixel_geom_id,1)<<" -- "<<sigma_pedx5<<endl
	<<"THRESHOLDx1 "<<RichPMTsManager::_PedestalThreshold(pmt_geom_id,pixel_geom_id,0)<<" -- "<<thresholdx1<<endl
	<<"THRESHOLDx5 "<<RichPMTsManager::_PedestalThreshold(pmt_geom_id,pixel_geom_id,1)<<" -- "<<thresholdx5<<endl
	<<"Status "<<RichPMTsManager::_Status(pmt_geom_id,pixel_geom_id)<<" "<<status<<endl<<endl;
#endif




    // Update the calibration tables
    RichPMTsManager::_Pedestal(pmt_geom_id,pixel_geom_id,0)=pedx1;
    RichPMTsManager::_Pedestal(pmt_geom_id,pixel_geom_id,1)=pedx5;
    RichPMTsManager::_PedestalSigma(pmt_geom_id,pixel_geom_id,0)=sigma_pedx1;
    RichPMTsManager::_PedestalSigma(pmt_geom_id,pixel_geom_id,1)=sigma_pedx5;
    RichPMTsManager::_PedestalThreshold(pmt_geom_id,pixel_geom_id,0)=thresholdx1;
    RichPMTsManager::_PedestalThreshold(pmt_geom_id,pixel_geom_id,1)=thresholdx5;
    RichPMTsManager::_Status(pmt_geom_id,pixel_geom_id)=status;
  }
  
  
  // Update the TDV tables id needed
  bool update=true;
   DAQEvent * pdaq = (DAQEvent*)AMSEvent::gethead()->getheadC("DAQEvent",6);
  int nc=0;
  int ncp=0;

  for(int i=0;i<2;i++){
   for (int j=0;j<24;j++){
     if( (!_Calib[i][j] &&(pdaq && pdaq->CalibRequested(getdaqid(i),j)))){
       update=false;
     }
     if(_Calib[i][j])nc++;
     if( (pdaq && pdaq->CalibRequested(getdaqid(i),j)))ncp++;
   }
  }
  
  cout <<" nc "<<nc<<" "<<ncp<<" "<<update<<endl;
  if(update){
    for(int i=0;i<2;i++){
      for(int j=0;j<sizeof(_Calib)/sizeof(_Calib[0][0])/2;j++){
	_Calib[i][j]=0;
      }
    }
    
    AMSTimeID *ptdv;
    time_t begin,end,insert;
    const int ntdv=4;
    const char* TDV2Update[ntdv]={"RichPMTChannelPedestal",
				  "RichPMTChannelPedestalSigma",
				  "RichPMTChannelPedestalThreshold",
				  "RichPMTChannelStatus"};
    for (int i=0;i<ntdv;i++){
      ptdv = AMSJob::gethead()->gettimestructure(AMSID(TDV2Update[i],AMSJob::gethead()->isRealData()));
      ptdv->UpdCRC();
      ptdv->UpdateMe()=1;
      time(&insert);
      if(CALIB.InsertTimeProc)insert=AMSEvent::gethead()->getrun();
      ptdv->SetTime(insert,AMSEvent::gethead()->getrun()-1,AMSEvent::gethead()->getrun()-1+864000);
      cout <<" RICH info has been updated for "<<*ptdv;
      ptdv->gettime(insert,begin,end);
      cout <<" Time Insert "<<ctime(&insert);
      cout <<" Time Begin "<<ctime(&begin);
      cout <<" Time End "<<ctime(&end);
    }
  }
}
