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

int DAQRichBlock::JINFNodes[2][RICH_JINFs][2]=             // List of jinf-r nodes, side a and b, two aliases (a=primary,b=secondary) 

  { // 0           1      -- side  
    {{158,160},{162,164}},     // primary
    {{159,161},{163,165}}      //secondary
  };



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

integer DAQRichBlock::checkdaqidnode(int16u node_word){   // RICH AS NODE
  int id=(node_word>>5)&((1<<9)-1);

#ifdef __AMSDEBUG__
  if(AMSFFKEY.Debug>1)
    cout<<"Checking node "<<id<<endl;
#endif

  for(int secondary=0;secondary<2;secondary++)
    for(int side=0;side<RICH_JINFs;side++)
      for(int alias=0;alias<2;alias++)
	if(id==JINFNodes[secondary][side][alias]){
#ifdef __AMSDEBUG__
	  if(AMSFFKEY.Debug>1)
	    cout<<"Found node "<<id<<" which is side:"<<side<<" secondary:"<<secondary<<endl;
#endif
	  return 1;
	}

  return 0;
}



void DAQRichBlock::buildraw(integer length,int16u *p){
  // The length integer includes the jinj id information in the hsb
  unsigned int jinj=length>>24;
  length&=(1<<24)-1;

  try{
  // Reset the status bits
  Status=kOk;
#ifdef __INSPECT__
  cout<<endl<<endl<<"RICH EVENT INSPECTOR: buildraw"<<endl;
  cout<<endl;
  cout<< " -1(-2) -- LENGTH                   -- "<<length<<" words"<<endl;
  int16u value=*p;
  printf("0       -- 1 0 nodeaddress datatype -- %i %i %i %x\n ",
	 (value&(1<<15))>>15,
	 (value&(1<<14))>>14,
	 (value&(0b0011111111100000))>>5,
	 (value&(0b11111)));

  printf("MASK    -- mask %x \n",*(p-2+length));
  printf("???     -- status %x\n",*(p-1+length));
  StatusParser parsed(*(p-1+length));
  printf("        --  SlaveId %i IsCDP %i is RAW %i isCompressed %i isData %i\n",
	 parsed.slaveId,
	 parsed.isCDP,
	 parsed.isRaw,
	 parsed.isCompressed,
	 parsed.isData);
  cout<<endl<<endl;

  cout<<"DUMPING EVERYTHING "<<endl;
  for(int i=0;i<length;i++) {cout<<"+"<<i<<" ";printf("%x\n",*(p+i));}
#endif

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
  

  // Assume that everything here is primary and decode
  int offset=0;
  if(DAQCFFKEY.DAQVersion==1) offset=-2;  // Take into account the JINF-R MASK
  if(!AMSJob::gethead()->isRealData()) offset=-2;  // Take into account the added JINF-R MASK for MC data

  bool secondary=0;
  if(AMSJob::gethead()->isRealData()) if(jinj==0 || jinj==3) secondary=1;
  DecodeRich(length+offset,p,JINF,secondary);
  }
  catch(int){
    static bool first_call=true;
    if(first_call){
      cout<<"-- Problem in DAQRichBlock::buildraw. Ignoring further errors"<<endl;
      first_call=false;
    }
  }
}


void DAQRichBlock::buildrawnode(integer length,int16u *p){
  try{
#ifdef __INSPECT__
  cout<<endl<<endl<<"RICH EVENT INSPECTOR: buildrawnode"<<endl;
  cout<<endl;
  cout<< " -1(-2) -- LENGTH                   -- "<<length<<" words"<<endl;
  int16u value=*p;
  printf("0       -- 1 0 nodeaddress datatype -- %i %i %i %x\n ",
	 (value&(1<<15))>>15,
	 (value&(1<<14))>>14,
	 (value&(0b0011111111100000))>>5,
	 (value&(0b11111)));

  printf("MASK    -- mask %x \n",*(p-2+length));
  printf("???     -- status %x\n",*(p-1+length));
  StatusParser parsed(*(p-1+length));
  printf("        --  SlaveId %i IsCDP %i is RAW %i isCompressed %i isData %i\n",
	 parsed.slaveId,
	 parsed.isCDP,
	 parsed.isRaw,
	 parsed.isCompressed,
	 parsed.isData);
  cout<<endl<<endl;

  cout<<"DUMPING EVERYTHING "<<endl;
  for(int i=0;i<length;i++) {cout<<"+"<<i<<" ";printf("%x\n",*(p+i));}
#endif

#ifdef __AMSDEBUG
  if(AMSFFKEY.Debug>1){
  cout<<"*** IN DAQRichBlock::buildrawnode "<<endl
      <<" Length "<<length<<endl;
  printf(" DUMP %x %x %x %x %x\n",*(p-1),*p,*(p+1),*(p+2),(*p+3));
  }
#endif

  // In p-1 is the node id and so 
  // In p is the event number p
  // In p+1 the first RDR info (its length)
  Status=kOk;
  if(length<1) Do(kLengthError);

  // This is supposed to be a JINF block: get the status word and check it
  //  StatusParser status(*(p-1+length));
  

  int id=((*(p-1))>>5)&((1<<9)-1);
  
  for(int secondary=0;secondary<2;secondary++)
    for(int side=0;side<RICH_JINFs;side++)
      for(int alias=0;alias<2;alias++)
	if(id==JINFNodes[secondary][side][alias]){
	  // Assume that everything here is primary and decode
	  int offset=0;
	  if(DAQCFFKEY.DAQVersion==1) offset=-2;  // Take into account the JINF-R MASK
	  if(!AMSJob::gethead()->isRealData()) offset=-2;  // Take into account the added JINF-R MASK for MC data
	  DecodeRich(length-1+offset,p+1,side,secondary);
	  break;
	}
  return;
  }catch(int){
    static bool first_call=true;
    if(first_call){
      cout<<"-- Problem in DAQRichBlock::buildrawnode. Ignoring further errors"<<endl;
      first_call=false;
    }
  }


  /*  
  // This is supposed to be a JINF block: get the status word and check it
  StatusParser status(*(p-1+length));
  // if(!status.isData || status.errors || status.isCDP) Do(kDataError);
  if(status.errors || status.isCDP) Do(kDataError);
  
  // Get the JINF number
  uint id=status.slaveId;  
  int JINF=-1;
  for(int i=0;i<RICH_JINFs;i++) if(JINFId[i]==id) {JINF=i;break;}
  if(JINF==-1) Do(kJINFIdError);
  

  // Assume that everything here is primary and decode
  DecodeRich(length,p,JINF,0);
  */
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
    case kWrongCDPChannelNumber:  // This is not a real error
      //      if(error_counter++<10)
      //	cerr<<"DAQRichBlock::buildraw -- CDP in reduced mode refers to an inexistent channel."<<endl;
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
  gain=data&0xF000?1:0;                    // This is reversed: no bit= high gain (in all the s/w gain mode=1 is high gain)
  inconsistent=data&0x4000;
  counts=data&0x0FFF;
}


integer DAQRichBlock::checkcalid(int16u id){
  // Take a block id and return if the block is a rich block
  if( ((id>>5)&((1<<9)-1))>=FirstNode && ((id>>5)&((1<<9)-1))<=LastNode ) return 1;
  return 0;
}

void DAQRichBlock::buildcal(integer length,int16u *p){
  try{
#ifdef __INSPECT__
  cout<<endl<<endl<<"RICH EVENT INSPECTOR CAL"<<endl;

  cout<<endl;
  cout<< " -1(-2) -- LENGTH                   -- "<<length<<" words"<<endl;
  int16u value=*p;
  printf("0       -- 1 0 nodeaddress datatype -- %i %i %i %x\n ",
	 (value&(1<<15))>>15,
	 (value&(1<<14))>>14,
	 (value&(0b0011111111100000))>>5,
	 (value&(0b11111)));

  printf("MASK    -- mask %x \n",*(p-2+length));
  printf("???     -- status %x\n",*(p-1+length));
  StatusParser parsed(*(p-1+length));
  printf("        --  SlaveId %i IsCDP %i is RAW %i isCompressed %i isData %i\n",
	 parsed.slaveId,
	 parsed.isCDP,
	 parsed.isRaw,
	 parsed.isCompressed,
	 parsed.isData);
  cout<<endl<<endl;

  cout<<"DUMPING EVERYTHING "<<endl;
  for(int i=-4;i<length;i++) {cout<<"+"<<dec<<i<<" ";printf("%x %d\n",*(p+i),*(p+i));}
#endif

  // This is calibration event for RDR
  // Length is in words
  const int block_size=2483;  // The length in words of the calibration table for a single RDR
  const int table_size=RICH_PMTperCDP*RICnwindows;; // 496 entries per table

#ifdef __INSPECT__
  cout<<dec<<"LENGTH "<<length<<" block size "<<block_size<<endl;
#endif

  int version=-1;
  switch(length){
  case block_size:
    version=0; //2008
    break;
  case block_size+2:
    version=1;
    break;
  }
  
  if(version==-1){
    cout<<"-- DAQRichBlock::buildcal unknown calibration data format"<<endl;
    return;
  }

  if(DAQCFFKEY.DAQVersion==1 && version==0){
    cout<<"-- DAQRichBlock::buildcal inconsistent calibration data format. Ignoring"<<endl;
    return;
  }

  //  if(length!=block_size) return;// THIS IS FOR THE OLD (2008) VERSION

  if(version==1){ // -- 2009
    if(DAQEvent::CalibInit(3)){
      cout<<"DAQRichBlock::buildcal-I-InitCalib "<<endl;
      for(int i=0;i<2;i++)
	for (int j=0;j<24;j++)
	  _Calib[i][j]=0;
    }
  }

  p-=1;  // Go to the true starting point of the data

  // General checks
  int16u id=*(p-1);
  int16u node_type=id&31;
  int16u status=*(p-2+length-1);

  if(version==0){  //2008
    if(node_type!=0x14) return;
    if(status!=0x120) return;
  }

  // We should check the status in a more clever way if version==1
  if(version==1){ // 2009
    if(DAQEvent::isError(status)){
      cout<<"-- DAQRichBlock::buildcal Status returns and error"<<endl;
      return;
    }
    if(node_type!=0x13){
      cout<<"-- DAQRichBlock::buildcal Status returns and error. Ignoring."<<endl;
    }

    uint16 cal_status=*(p+1);
    if(cal_status&(0x4000)==0){
      cout<<"-- DAQRichBlock::buildcal Calibration status is not correct"<<endl;
      return;
    }

  }

  // Get which RDR is going to be updated
  int16u node_number=((id>>5)&((1<<9)-1));
  int physical_cdp=node_number-FirstNode;

  if(version==1) p+=2; // Go to the right position keeping backwards compatibility

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
    float sigma_pedx1=sigma_pedx5*(pedx5>0?float(pedx1)/float(pedx5):0.2); 
    int status=*(p+i+4*table_size);
    status=status&0xc000?0:1;       // This is the right computation

    if(sigma_pedx5==0){
      thresholdx5=4096;
      status=0;
    }else{
      thresholdx5=int(thresholdx5/sigma_pedx5+0.5);
    }
    int thresholdx1=thresholdx5;

#ifdef __AMSDEBUG__
    if(AMSFFKEY.Debug>1)
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
    //    RichPMTsManager::_Status(pmt_geom_id,pixel_geom_id)=(status &&  RichPMTsManager::_Status(pmt_geom_id,pixel_geom_id))?status:0;  // Do not let channels to recover
  }
  

  cout<<"DAQRichBlock::buildcal -- found calibration tables for RDR-"<<physical_cdp/12<<"-"<<physical_cdp<<endl;
  for(int i=0;i<2;i++){
    for (int j=0;j<24;j++){
      if(Links[i][j]==physical_cdp){
	_Calib[i][j]=1;
	break;
      }
    }
  }
  // The pragmas here guarantee that everything is done properly
#pragma omp barrier
  cout <<"  in barrier DAQRichBlock::buildcal "<< AMSEvent::get_thread_num()<<endl;
  
  // Update the TDV tables if needed
  bool update=true;

  if(version==0){ // -- 2008
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
    }
  }

  if(version==1){ // -- 2009
    update=DAQEvent::CalibDone(3);  
  }

#pragma omp single
  if(update){
    AMSTimeID *ptdv;
    time_t begin,end,insert;
    const int ntdv=4;
    const char* TDV2Update[ntdv]={"RichPMTChannelPedestal",
				  "RichPMTChannelPedestalSigma",
				  "RichPMTChannelPedestalThreshold",
				  "RichPMTChannelStatus"};
    for (int i=0;i<ntdv;i++){
      ptdv = AMSJob::gethead()->gettimestructure(AMSID(TDV2Update[i],AMSJob::gethead()->isRealData()));
      ptdv->UpdateMe()=1;
      ptdv->UpdCRC();
      time(&insert);
      if(CALIB.InsertTimeProc)insert=AMSEvent::gethead()->getrun();
#define min(x,y) ((x)<(y)?(x):(y))
      ptdv->SetTime(insert,min(AMSEvent::gethead()->getrun()-1,AMSEvent::gethead()->gettime()),AMSEvent::gethead()->getrun()-1+864000);
#undef min
      cout <<" RICH info has been updated for "<<*ptdv;
      ptdv->gettime(insert,begin,end);
      cout <<" Time Insert "<<ctime(&insert);
      cout <<" Time Begin "<<ctime(&begin);
      cout <<" Time End "<<ctime(&end);
    }

    if(version==1)
      for(int i=0;i<2;i++)
	for (int j=0;j<24;j++)
	  _Calib[i][j]=0;    
  }
  }catch(int){
    static bool first_call=true;
    if(first_call){
      cout<<"-- Problem in DAQRichBlock::buildcal. Ignoring further errors"<<endl;
      first_call=false;
    }
  }
}



void DAQRichBlock::DecodeRich(integer length,int16u *p,int side,int secondary){
  //
  // This functions perform the actual decoding with the information 
  // of the side and if it is a primary of secondary block 
  //

  int16u *pointer=p;

  int CDPFound;          // Counter for the number of CDP found
  int low_gain[RICH_PMTperCDP][RICnwindows];       // Buffer to store low gain information just in case it is needed
  for(CDPFound=0;;CDPFound++){
#ifdef __AMSDEBUG__
    if(AMSFFKEY.Debug>1)
    cout<<"Pointer at "<<pointer-p<<" fence in "<<length-1<<" fence content "<<*(p-1+length)<<endl; 
#endif

    if(pointer>=p-1+length) break;   // Last fragment processed       
    
    FragmentParser cdp(pointer);
    if(cdp.status.errors) Do(kCDPError);

    int CDP=cdp.status.slaveId;
    if(secondary) if(CDP==1) CDP=9; else if(CDP==9) CDP=1;
    
    // Process data
    //    if(!cdp.status.isRaw && !cdp.status.isCompressed) Do(kCalibration);
    if(cdp.status.isRaw){

      // Prepare some nice histograms
      /*
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
      */

      if(cdp.length!=1+         // Status word
	 RICH_PMTperCDP*RICnwindows*2) Do(kCDPRawTruncated);

      // Fill raw information
      // Simple loop getting all the information

      DSPRawParser channel(cdp.data);

      do{
	//daq_histograms[channel.gain+2*channel.pixel+2*RICnwindows*channel.pmt]->Fill(channel.counts);

	// First comes the low gain, the high gain then
	if(channel.gain==0) {low_gain[channel.pmt][channel.pixel]=channel.counts;}
	else{
	  // Get the geom ID for this channel
	  int physical_cdp=Links[side][CDP];

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
	  int physical_cdp=Links[side][CDP];
	  if(physical_cdp!=-1){
	    int geom_id=RichPMTsManager::GetGeomPMTIdFromCDP(physical_cdp,channel.pmt);
	    
//	    if(geom_id<0) Do(kWrongCDPChannelNumber);
              if(geom_id<0) continue;	    
	    // Get the pixel geom id, substract the pedestal, check that
	    // it is above it and use low gain if necessary
	    int pixel_id=RichPMTsManager::GetGeomChannelID(geom_id,channel.pixel);

	    if(RichPMTsManager::Status(geom_id,pixel_id)%10==Status_good_channel){  // Channel is OK
	      int mode=channel.gain;                 // High gain
	      int counts=channel.counts;

	      if(channel.inconsistent){
		// Substract the Gx1 pedestal
		counts-=int(RichPMTsManager::Pedestal(geom_id,pixel_id,0));
	      }

	      int channel_geom_number=RichPMTsManager::PackGeom(geom_id,pixel_id);

#ifdef __INSPECT__
	      cout<<"ADDING HIT IN COMPRESSED MODE: "<<endl
		  <<"   PMT GEOM ID "<<channel_geom_number<<endl
		  <<"   COUNTS  "<<counts<<endl
		  <<"   HIGH GAIN "<<mode<<endl;
#endif

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

///////////////////////////////////////////////////////////////////////////////////////////////////////
// RAW to DAQ for rich
// By the moment we assume that it is reduced mode



integer DAQRichBlock::calcdaqlength(int jinr_number){
  // Simply loop through all hits and check if it belongs to jinr_number
  const int cdps=24;
  int cdp_used[cdps]={0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0};

  AMSRichRawEvent *ptr=(AMSRichRawEvent*)AMSEvent::gethead()->
    getheadC("AMSRichRawEvent",0);

  // Get CDP range for this block
  int min_cdp,max_cdp;
  switch(jinr_number){
  case 0:
    min_cdp=0;
    max_cdp=11;
    break;
  case 1:
    min_cdp=12;
    max_cdp=23;
    break;
  default:
    cout<<"DAQRichBlock::calcdaqlength -- Unknown block number "<<jinr_number<<endl;
    return 0;
    break;
  }
  
  for(;ptr;ptr=ptr->next()){
    int pmtgeom;
    int pixelgeom;
    RichPMTsManager::UnpackGeom(ptr->getchannel(),pmtgeom,pixelgeom);
    int pmt;
    int cdp=RichPMTsManager::GetCDPFromGeomPMTId(pmtgeom,pmt);

    if(cdp<min_cdp || cdp>max_cdp) continue;  // Hit does not belong
                                              // To current block 

    cdp_used[cdp]++;
  }  


  // Compute the total length for this block:
  int nhits=0;
  int ncdps=0;
  for(int i=0;i<cdps;i++){
    nhits+=cdp_used[i];
    ncdps+=cdp_used[i]!=0?1:0;
  }

  int datawords=nhits*2;     // Number of words is 2 per hit 
  int formatwords=ncdps*2+1; // At least length and status words per CDP + 1 status per block
  if(DAQCFFKEY.DAQVersion==1) formatwords+=2; // Add the mask words
  
  return -(datawords+formatwords);  // We return it as a JINF-R Block
}




void DAQRichBlock::builddaq(integer jinr_number,integer length,int16u *p){
  try{
  const int PMTs=RICH_PMTperCDP;
  *(p+length-1)=(JINFId[jinr_number]&31)|(1<<7);// link+compressed mode 

  // Count the non-empty cdps
  const int cdps=24;
  int cdp_used[cdps]={0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0};

  AMSRichRawEvent *ptr=(AMSRichRawEvent*)AMSEvent::gethead()->
    getheadC("AMSRichRawEvent",0);

  // Get CDP range for this block
  int min_cdp,max_cdp;
  switch(jinr_number){
  case 0:
    min_cdp=0;
    max_cdp=11;
    break;
  case 1:
    min_cdp=12;
    max_cdp=23;
    break;
  default:
    cout<<"DAQRichBlock::calcdaqlength -- Unknown block number "<<jinr_number<<endl;
    return;
    break;
  }

  
  for(;ptr;ptr=ptr->next()){
    int pmtgeom;
    int pixelgeom;
    RichPMTsManager::UnpackGeom(ptr->getchannel(),pmtgeom,pixelgeom);
    int pmt;
    int cdp=RichPMTsManager::GetCDPFromGeomPMTId(pmtgeom,pmt);

    if(cdp<min_cdp || cdp>max_cdp) continue;  // Hit does not belong
                                              // To current block 

    cdp_used[cdp]++;
  }  

  int16u *cdp_p=p;

  // Loop througth cdps, fill the length and status, and the fill the 
  for(int i=0;i<cdps;i++){
    if(cdp_used[i]==0) continue;

    *cdp_p=cdp_used[i]*2+1;     // length of CDP block is 1 status word+2 words per hit
    cdp_p++; // Move pointer to begining of data block

    ptr=(AMSRichRawEvent*)AMSEvent::gethead()->getheadC("AMSRichRawEvent",0);
    for(;ptr;ptr=ptr->next()){
      int pmtgeom;
      int pixelgeom;
      RichPMTsManager::UnpackGeom(ptr->getchannel(),pmtgeom,pixelgeom);
      int pmt;  // store the pmt index within CDP
      int cdp=RichPMTsManager::GetCDPFromGeomPMTId(pmtgeom,pmt);
      
      if(cdp!=i) continue;

      // Get physical pixel id
      int pixel=RichPMTsManager::GetChannelID(pmtgeom,pixelgeom);
      
      // Encode channel position
      int16u data=pmt+PMTs*pixel;+PMTs*16*(ptr->gainx5());
      *cdp_p=data;
	
      // Encode reduced pixel value
      data=ptr->getcounts();
      data&=0x0FFF;
      data|=(ptr->gainx5()?0:1)<<12;
      *(cdp_p+1)=data;
      cdp_p+=2;      


      //      printf("      WRITING HIT GEOM %i ID %x  counts %x\n",ptr->getchannel(),*(cdp_p-2),*(cdp_p-1));
    } 
    int link=-1;
    for(link=0;link<24;link++) if(Links[jinr_number][link]==i)break;
    *cdp_p=(link)|(1<<7)|(0x0020); // Status: link+compressed mode+CDP
    cdp_p++;
  }
  }catch(int){
    static bool first_call=true;
    if(first_call){
      cout<<"-- Problem in DAQRichBlock::builddaq. Ignoring further errors"<<endl;
      first_call=false;
    }
  }
}





/**********************************************************

// There is one block per node (CDP) which gives 24 nodes, numbered from 242 to 245

void DAQRichBlock::builddaq(integer block,integer length,int16u *p){
  const int PMTs=RICH_PMTperCDP;

  AMSRichRawEvent *ptr=(AMSRichRawEvent*)AMSEvent::gethead()->
    getheadC("AMSRichRawEvent",0);

  *p=getdaqid(block); // Check this

  // Loop in loop on RDR for this CDP, extract the signals for each and build the block
  // This includes: setup the block size, fill the info 

  for(int link=0;link<RICH_LinksperJINF;link++){
    if(Links[block][link]==-1) continue;   // Skip unused links 

    int block_size=0;  // Size in sotred channels
    
    // Find how many hits belong to this link
    for(;ptr;ptr=ptr->next()){
      int pmtgeom;
      int pixelgeom;
      RichPMTsManager::UnpackGeom(ptr->getchannel(),pmtgeom,pixelgeom);
      int pmt;
      int cdp=RichPMTsManager::GetCDPFromGeomPMTId(pmtgeom,pmt);
      if(cdp!=Links[block][link]) continue;

      // Store the channel
      block_size+=1;

      // Convert the pixel to physical one
      int pixel=RichPMTsManager::GetChannelID(pmtgeom,pixelgeom);
      
      int counts=ptr->getcounts();
      int high_gain=ptr->getchannelgainmode();
      
      // Encode the data
      int16u channelid=pixel*PMTs+pmt;
      int16u data=counts;
      data|=(1-high_gain)<<12;
      
      // Store it
      *(p++)=channelid;
      *(p++)=data;
    }

    // Store the size and the link id
  }
}

***************************************************************/
