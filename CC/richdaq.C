#include "richdaq.h"
#include "richid_default_values.h"

//////////////////////////////////////////////////
// RICH CDP are labelled from 0 to 23 (24 CDPs)

int DAQRichBlock::JINFId[RICH_JINFs]={10,11};           // JINFR0 and JINFR1  (link identification from JINJ)


// Now a table giving, for each pair RICH_JINF link and RDR link, which physical
// RDR it is (counting from 0 clockwise, starting from crate 1) 
int DAQRichBlock::Links[RICH_JINFs][RICH_LinksperJINF]=   // Table of links. Currently dummy 
  {
    { 0, 1, 2, 3, 4, 5, 6, 7, 8, 9,10,11, 0, 1, 2, 3, 4, 5, 6, 7, 8, 9,10,11},       //JINR0
    {12,13,14,15,16,17,18,19,20,21,22,23,12,13,14,15,16,17,18,19,20,21,22,23}        //JINR1
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
  if(!status.isData || status.errors || status.isCDP) Do(kDataError);
  
  // Get the JINF number
  uint id=status.slaveId;  
  int JINF=-1;
  for(int i=0;i<RICH_JINFs;i++) if(JINFId[i]==id) {JINF=i;break;}
  if(JINF==-1) Do(kJINFIdError);
  
  // Go through each RDR
  int16u *pointer=p;
  int RDRFound;          // Counter for the number of RDR found
  
  for(RDRFound=0;;RDRFound++){
    if(pointer>p-1+length) break;   // Last fragment processed       
    
    FragmentParser rdr(pointer);
    if(rdr.status.errors) Do(kRDRError);
    
    int RDR=rdr.status.slaveId;
    
    // Process RDR data
    if(!rdr.status.isRaw && !rdr.status.isCompressed) Do(kCalibration);
    
    if(rdr.status.isRaw){
      // Fill raw information
      // Simple loop getting all the information
    }
    
    if(rdr.status.isCompressed){
      if(rdr.status.isRaw)Do(kMixedMode);
      else{
	// Fill compressed mode
      }
    }
    
    pointer=rdr.next;
  }

  
  if(RDRFound!=RICH_RDRperJINF) Do(kTruncated);

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
    case kRDRError:
      cout<<"DAQRichBlock::buildraw -- RDR status word flag errors"<<endl;
      return 1; break;
    case kCalibration:
      cout<<"DAQRichBlock::buildraw -- RDR Calibration mode processing is not yet implemented"<<endl;
      return 1; break;
    case kMixedMode:
      cout<<"DAQRichBlock::buildraw -- RDR Mixed mode: will ignore compressed data"<<endl;
      return 0; break;
    case kTruncated:
      cout<<"DAQRichBlock::buildraw -- JINF information seems to be truncated: not enough RDRs. Ignored"<<endl;
      return 0; break;
    default:
      return 0;break;
    }
} 
