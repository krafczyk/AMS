#include "richdaq.h"
#include "richid.h"
#include "richrec.h"

//////////////////////////////////////////////////
// RICH CDP are labelled from 0 to 23 (24 CDPs)

int DAQRichBlock::JINFId[RICH_JINFs]={10,11};           // JINFR0 and JINFR1  (link identification from JINJ)


// Now a table giving, for each pair RICH_JINF link and RDR link, which physical
// RDR it is (counting from 0 clockwise, starting from crate 1) 
int DAQRichBlock::Links[RICH_JINFs][RICH_LinksperJINF]=   // Table of links (really ports) -> given the JINF (0 ot 1 corresponding to ports 10 and 11) and the link number, you get the
                                                          // physical CDP number  
  {
//    0  1  2  3  4  5  6  7  8  9 10 11 12 13 14 15 16 17 18 19 20 21 22
    { 7, 5,-1,-1, 6, 9,-1,-1,10,11,-1,-1, 8, 2,-1,-1, 1, 0,-1,-1, 3, 4,-1},  // JINF-R-0-P
    {19,23,-1,-1,18,21,-1,-1,22,17,-1,-1,20,14,-1,-1,13,12,-1,-1,15,16,-1}   // JINF-R-1-P
  };
int DAQRichBlock::Status=DAQRichBlock::kOk;


integer DAQRichBlock::checkdaqid(int16u id){
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
      if(cdp.length!=1+         // Status word
	 RICH_PMTperCDP*RICnwindows*2) Do(kCDPRawTruncated);

      // Fill raw information
      // Simple loop getting all the information

      DSPRawParser channel(cdp.data);

      do{
	// First comes the low gain, the high gain then
	if(channel.gain==0) {low_gain[channel.pmt][channel.pixel]=channel.counts;}
	else{
	  // Get the geom ID for this channel
	  int physical_cdp=Links[JINF][CDP];
	  assert(physical_cdp!=-1);
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
	      
	      // Add the hit
	      int channel_geom_number=RichPMTsManager::PackGeom(geom_id,pixel_id);
	      counts-=RichPMTsManager::Pedestal(geom_id,pixel_id,mode);
	      
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
      }while(channel.Next());

    }
    
    if(cdp.status.isCompressed){
      if(cdp.status.isRaw)Do(kMixedMode);
      else{
	// Fill compressed mode if something to fill 
	if(cdp.length-1>0){
	  DSPCompressedParser channel(cdp.data,cdp.length-1);

	  do{
	    int physical_cdp=Links[JINF][CDP];
	    assert(physical_cdp!=-1);
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
	  }while(channel.Next());
	    
	}
      }
    }

    pointer=cdp.next;
  }

  
  //  if(CDPFound!=RICH_CDPperJINF) Do(kTruncated);  // This is only valid in RAW mode

}


// Deal with errors with this function. Return 1 if exit needed, 0 otherwise
int DAQRichBlock::Emit(int code){
  Status=code;

  switch(Status)
    {
    case kOk:
      return 0; break;
    case kLengthError:
      cout<<"DAQRichBlock::buildraw -- Fragment length too short"<<endl;
      return 1; break;
    case kDataError:
      cout<<"DAQRichBlock::buildraw -- Status does not agree with a JINF fragment"<<endl;
      return 1; break;
    case kJINFError:
      cout<<"DAQRichBlock::buildraw -- JINF Id int status word does not belong to RICH"<<endl;
      return 1; break;
    case kCDPError:
      cout<<"DAQRichBlock::buildraw -- CDP status word flag errors"<<endl;
      return 1; break;
    case kCalibration:
      cout<<"DAQRichBlock::buildraw -- CDP Calibration mode processing is not yet implemented"<<endl;
      return 1; break;
    case kMixedMode:
      cout<<"DAQRichBlock::buildraw -- CDP Mixed mode: will ignore compressed data"<<endl;
      return 0; break;
    case kTruncated:
      cout<<"DAQRichBlock::buildraw -- JINF information seems to be truncated: not enough CDPs. Ignored"<<endl;
      return 0; break;
    case kCDPRawTruncated:
      cout<<"DAQRichBlock::buildraw -- CDP information seems to be truncated."<<endl;
      return 1; break;
    case kWrongCDPChannelNumber:
      cout<<"DAQRichBlock::buildraw -- CDP in reduced mode refers to an inexistent channel."<<endl;
      return 1; break;
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
  gain=data&0x1000?0:1;                    // This is reversed: no bit= high gain (gina mode=1)
}
