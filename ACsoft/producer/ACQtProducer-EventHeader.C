#include "ACQtProducer-include.C"


static inline bool GoodHW(DaqEventR *daq){
  /*
  Authors A.Oliva -  M.Duranti
  see http://ams.cern.ch/AMS/DAQsoft/daqsoft_note.pdf

  Reply Status
       bit     15 - DATA bit, set by master when assembling group reply;
       bits 14-11 - reply code (see Section 6.4), set by master;
       bit     10 - build conditions error (Sub-Detector specific);
       bit      9 - build errors (sequencer errors (CDP) or event assembly (CDDC));
       bit      8 - cumulative node status (OR of bits 11-8 in the Node Status Word);
       bit      7 - COMPRESSED event building mode;
       bit      6 - RAW event building mode;
       bit      5 - internal structure bit (0 means group assembly, 1        bits   4-0 - slave ID, set by master;

  Node Status Word
       bit     11 - Buffer Memory Test errors detected at Node Init;
       bit     10 - Program Memory test errors detected at Node Init;
       bit      9 - Data Memory test errors detected at Node Init;
       bit      8 - Program self-test errors;
  */

  if(!daq) return false;

  //  unsigned int bitstocheck = 0x7F;// Reply Status Word from 8 to 15. This leaves also the "cumulative node address" as bit to be checked. This contains DSP errors from the checked nodes (JINJ, JINF, JLV1 and SDR) even if we decided not to check the DSP errors
  unsigned int bitstocheck = 0x7E;// Reply Status Word from 9 to 15.

  bool errors=false;
  for (int ii=0; ii<4; ii++ ) errors |= (bool)((daq->JINJStatus[ii]>>8) & bitstocheck);
  for (int ii=0; ii<24; ii++) {
    if (ii!=10 && ii!=11) {
      errors |= (bool)(daq->JError[ii] & bitstocheck);
    }
  }

  return !errors;
}


static inline bool GoodHW_deeper(DaqEventR *daq){
  /*
  Authors A.Oliva -  M.Duranti
  see DaqEventR class, root.h-root.C

  bool L3ProcError(){return (L3Error & (1ULL<<63))?true:false;}
  bool L3RunError(){return (L3Error & (1ULL<<62))?true:false;}
  bool L3EventError(){return (L3Error & (1ULL<<61))?true:false;}
  unsigned int L3NodeError(int i); ///< Return number of nodes with errors for (ETRG<LV1,SDR,EDR,RDR,UDR,TDR 0...6
  unsigned int DaqEventR::L3NodeError(int i){
  int j=i*8;
  return (unsigned int)((L3Error>>j)&255);
  }

  unsigned long long  L3Error;  ///<Lvl3 event error

  bit 63  proc error --> if during the "unpacking" of the event there were some error/inconsistency
  bit 62  desync error --> desync at any level (even between JINJ and JMDC)
  bit 61  event error --> consistency of the reply of the node (deeper that just looking the Reply Status Word)
  byte 6  number of TDR  with fatal errors
  byte 5  number of UDR  with fatal errors
  byte 4  number of RDR  with fatal errors
  byte 3  number of EDR  with fatal errors
  byte 2  number of SDR  with fatal errors
  byte 1  number of LV1  with fatal errors
  byte 0  number of ETRG  with fatal errors
  */

  if(!daq) return false;

  // Whatever bad REPLY at JINJ, JINF or LVL1 (ROOM, Sync, 0-replies, ...) 
  bool assembly_error = daq->L3EventError();  

  // Desync error at whatever level 
  bool sync_error = daq->L3RunError();

  // Decoding procedure error
  bool proc_error = daq->L3ProcError();

  // Whatever board with bad REPLY
  bool node_error = false;
  for (int inode=0; inode< 7; inode++) {
    if (inode!=4) { //we don't want to check RICH for electron/positrons!!!
      if (daq->L3NodeError(inode)>0) { //whit this code there is still the possibility that we're checking the DSP error (and of course in the wrong way) for these nodes...
	node_error = true;
	//cdp_errors[inode] += daq->L3NodeError(inode);
      }
    }
  }

  bool errors=false;
  errors|=assembly_error;
  errors|=sync_error;
  errors|=proc_error;
  errors|=node_error;

  return !errors;
}






bool ACsoft::ACQtProducer::ProduceEventHeader() {

  static TRandom3* fRandomNumberGenerator = 0;
  if (!fRandomNumberGenerator)
    fRandomNumberGenerator = new TRandom3(0);

  Q_ASSERT(fEvent);
  Q_ASSERT(fAMSEvent);

  AC::EventHeader eventHeader;
  eventHeader.fRandom = int(256.0 * fRandomNumberGenerator->Uniform()); // RandomNr from 0..255
  eventHeader.fStatus = fAMSEvent->fStatus;
  eventHeader.fEvent = fAMSEvent->Event();

  // int nDAQ = fAMSEvent->NDaqEvent();
  // bool goodHW=true, goodHWdeeper=true;
  // if(nDAQ>0){
  //   goodHW       = GoodHW(       fAMSEvent->pDaqEvent(0));
  //   goodHWdeeper = GoodHW_deeper(fAMSEvent->pDaqEvent(0));
  // }
  // printf("===== Event %6d ====nDAQ:%3d = HW:%1d == HWdeeper:%1d ======\n",fAMSEvent->Event(),nDAQ,goodHW,goodHWdeeper);
  // eventHeader.fEvent |= (!goodHW)       << 30; //  bad bit in second highest Event-bit
  // eventHeader.fEvent |= (!goodHWdeeper) << 31; //  bad bit in        highest Event-bit

  eventHeader.fTimeStamp = AC::ConstructTimeStamp(fAMSEvent->UTime(), fAMSEvent->Frac());
  eventHeader.fUTCTime = fAMSEvent->UTCTime();

  eventHeader.fMagneticLatitude = fAMSEvent->fHeader.ThetaM;
  eventHeader.fMagneticLongitude = fAMSEvent->fHeader.PhiM;
  eventHeader.fISSLatitude = fAMSEvent->fHeader.ThetaS;
  eventHeader.fISSLongitude = fAMSEvent->fHeader.PhiS;
  eventHeader.fISSDistanceFromEarthCenter = fAMSEvent->fHeader.RadS;
  eventHeader.fISSRoll = fAMSEvent->fHeader.Roll;
  eventHeader.fISSPitch = fAMSEvent->fHeader.Pitch;
  eventHeader.fISSYaw = fAMSEvent->fHeader.Yaw;
  eventHeader.fISSVelocityLatitude = fAMSEvent->fHeader.VelTheta;
  eventHeader.fISSVelocityLongitude = fAMSEvent->fHeader.VelPhi;

  if(fAMSEvent->UTime() != UTimeCutOffConeMax) { // update StoermerCutOffConeMax
    UTimeCutOffConeMax = fAMSEvent->UTime();
    fAMSEvent->GetMaxGeoCutoff( 35.0, 1.0, MaxCutOffCone); 
  }
  eventHeader.fMaxCutOffConeNegative = MaxCutOffCone[0];
  eventHeader.fMaxCutOffConePositive = MaxCutOffCone[1];

  fEvent->fEventHeader = eventHeader;
  return true;
}
