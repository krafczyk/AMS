//
//===============================================================================
//
#include "TrSim.h"
void AMSJob::_sitkdata(){

  TKGEOMFFKEY.init();
  FFKEY("TKGE",(float*)&(TKGEOMFFKEY.ReadGeomFromFile),(sizeof(TKGEOMFFKEY_DEF)-4)/sizeof(integer),
	"MIXED");
  
  TRMCFFKEY.init();
  FFKEY("TRMC",(float*)&(TRMCFFKEY.alpha),(sizeof(TRMCFFKEY_DEF)-4)/sizeof(integer),"MIXED");

  TRCALIB.init();
  FFKEY("TRCALIB",(float*)&(TRCALIB.EventsPerCheck),(sizeof(TRCALIB_DEF)-4)/sizeof(integer),"MIXED");

  TRALIG.init();
  FFKEY("TRALIG",(float*)&(TRALIG.InitDB),(sizeof(TRALIG_DEF)-4)/sizeof(integer),"MIXED");


}


void AMSJob::_retkdata(){

  //number fac=AMSTrRawCluster::ADC2KeV()*0.46/0.4;
  TRCLFFKEY.init();
  FFKEY("TRCL",(float*)&(TRCLFFKEY.recflag),(sizeof(TRCLFFKEY_DEF)-4)/sizeof(integer),"MIXED");
  
  TRFITFFKEY.init();
  FFKEY("TRFIT",(float*)&(TRFITFFKEY.ErrX),(sizeof(TRFITFFKEY_DEF)-4)/sizeof(integer),"MIXED");

  TRCHAFFKEY.init();
  FFKEY("TRCHA",(float*)&(TRCHAFFKEY.ReconType),(sizeof(TRCHAFFKEY_DEF)-4)/sizeof(integer),"MIXED");

}

void AMSJob::_retkinitjob(){
  AMSgObj::BookTimer.book("RETKEVENT");
  AMSgObj::BookTimer.book("TrTrack");
  AMSgObj::BookTimer.book("TrTrack0Find");
  AMSgObj::BookTimer.book("TrTrack1Eval");
  AMSgObj::BookTimer.book("TrTrack2Build");
  AMSgObj::BookTimer.book("TrTrack3Extension");
  AMSgObj::BookTimer.book("TrTrack4Match");
  AMSgObj::BookTimer.book("TrTrack5Fit");
  AMSgObj::BookTimer.book("TrTrack6FitE");
  AMSgObj::BookTimer.book("TrTrack0Vpsel");
  AMSgObj::BookTimer.book("TrTrack1Vfind");
  AMSgObj::BookTimer.book("TrTrack1Vhmlt");
  AMSgObj::BookTimer.book("TrCluster");
  AMSgObj::BookTimer.book("TrClusterRefit");
  AMSgObj::BookTimer.book("TrClusterReorder");
  AMSgObj::BookTimer.book("TrRecHit");
  AMSgObj::BookTimer.book("TrRecHitReorder");
  AMSgObj::BookTimer.book("TrFitRkmsF");
  AMSgObj::BookTimer.book("TrFitRkmsT");
//PZ FIXME CHARGE  if(CHARGEFITFFKEY.TrkPDFileRead%10>0){//(Q) Take ChargeCalibPDFs(mc|rd) from raw files
//     //
//     TrkElosPDF::build();//create TrkElosPDF-objects from TRK raw ChargeCalibFile
//   }
}



void AMSJob::_tkendjob(){
#ifdef _PGTRACK_
  printf(" In TKENDJOB \n");

  // TrCalDB::Head->Write();

#endif

//PZ FIXME  CALIB  if(isCalibration() & CTracker){
//     if(TRALIG.UpdateDB){
//       if(!TRALIG.GlobalFit)AMSTrAligFit::Test(1);
//       else AMSTrAligFit::Testgl(2);
//     }
//   }
//   if((isCalibration() & AMSJob::CTracker) && TRCALIB.CalibProcedureNo == 1){
//     AMSTrIdCalib::check(1);
//     AMSTrIdCalib::printbadchanlist();
//   }
//   if((isCalibration() & AMSJob::CTracker) && TRCALIB.CalibProcedureNo == 2){
//     int i,j;
//     for(i=0;i<nalg;i++){
//       if(TRCALIB.Method==0 || TRCALIB.Method==i){
// 	for(j=TRCALIB.PatStart;j<tkcalpat;j++){
// 	  if(AMSTrCalibFit::getHead(i,j)->Test(1)){
// 	    AMSTrCalibFit::getHead(i,j)->Fit();
// 	  }
// 	}
//       }
//     }

//     for(i=0;i<nalg;i++){
//       if(TRCALIB.Method==0 || TRCALIB.Method==i){
// 	for(j=TRCALIB.PatStart;j<tkcalpat;j++){
// 	  AMSTrCalibFit::getHead(i,j)->Anal();
// 	}
//       }
//     }
//     AMSTrCalibFit::SAnal();

//   }
//   if((isCalibration() & AMSJob::CTracker) && TRCALIB.CalibProcedureNo == 3){
//     AMSTrIdCalib::ntuple(AMSEvent::getSRun());
//   }
//   if(isMonitoring() & (AMSJob::MTracker | AMSJob::MAll)){
//     //   AMSTrIdCalib::offmonhist();    
//   }



}
void AMSJob::_sitkinitjob(){
//PZ FIXME  NOISE SIM  if(TRMCFFKEY.GenerateConst){
//     AString fnam(AMSDATADIR.amsdatadir);
//     fnam+="trsigma.hbk";
//     int iostat;
//     int rstat=1024;
//     HROPEN(1,"input",fnam.operator char*(),"P",rstat,iostat);
//     if(iostat){
//       cerr << "Error opening TrSigmaFile file "<<fnam<<endl;
//       throw amsglobalerror("UnableToOpenHistoFile",3);
//     }
//     char input[]="//input";
//     HCDIR(input," ");
//     HRIN(0,9999,0);
//     //    HPRINT(101);
//     //    HPRINT(102);
//     //    HPRINT(201);
//     //    HPRINT(202);
//     for(int l=0;l<2;l++){
//       for (int i=0;i<TKDBc::nlay();i++){
// 	for (int j=0;j<TKDBc::nlad(i+1);j++){
// 	  for (int s=0;s<2;s++){
//             AMSTrIdSoft id(i+1,j+1,s,l,0);
//             if(id.dead())continue;
//             number oldone=0;
//             for(int k=0;k<TKDBc::NStripsDrp(i+1,l);k++){
// 	      id.upd(k);
// 	      geant d;
// 	      id.setped()=TRMCFFKEY.ped[l]*(1+RNDM(d));
// 	      id.clearstatus(~0);
// 	      if(RNDM(d)<TRMCFFKEY.BadCh[l]){
// 		id.setstatus(AMSDBc::BAD);
// 		id.setsig()=0.85*HRNDM1(201+l);
// 	      }
// 	      else{
// 		id.setsig()=(l==0?0.76:0.84)*HRNDM1(101+l);
// 		if(id.getsig()<1){
// 		  id.setstatus(AMSDBc::BAD);
// 		}
// 		else if(id.getsig()>20){
// 		  id.setsig()=HRNDM1(101+l);
// 		  if(id.getsig()<1){
// 		    id.setstatus(AMSDBc::BAD);
// 		  }
// 		}
// 		else if(id.getsig()>10){
// 		  if(RNDM(d)<0.1*(id.getsig()-10)){
// 		    id.setsig()=HRNDM1(101+l);
// 		    if(id.getsig()<1){
// 		      id.setstatus(AMSDBc::BAD);
// 		    }
// 		  }
// 		}
               
// 	      } 
// 	      if(TRMCFFKEY.GenerateConst>1)id.setsig()=TRMCFFKEY.sigma[l]*(0.9+0.2*RNDM(d));
// 	      id.setgain()=TRMCFFKEY.gain[l];
// 	      id.setcmnnoise()=TRMCFFKEY.cmn[l]*(1+RNDM(d));
// 	      id.setindnoise()=oldone+
// 		AMSTrMCCluster::sitknoiseprob(id ,id.getsig()*TRMCFFKEY.thr1R[l]);
// 	      oldone=id.getindnoise();
//             }
// 	  }
// 	}
//       }
//     }
//     HREND("input");
//     CLOSEF(1);
//     char pawc[]="//PAWC";
//     HCDIR(pawc," " );
//     HDELET(101);
//     HDELET(102);
//     HDELET(201);
//     HDELET(202);
//     HBOOK1(101,"sigx",800,0.,200.,0.);
//     HBOOK1(102,"sigy",800,0.,200.,0.);
//     HBOOK1(201,"sigx",800,0.,200.,0.);
//     HBOOK1(202,"sigy",800,0.,200.,0.);
//     for(int l=0;l<2;l++){
//       for (int i=0;i<TKDBc::nlay();i++){
// 	for (int j=0;j<TKDBc::nlad(i+1);j++){
// 	  for (int s=0;s<2;s++){
//             AMSTrIdSoft id(i+1,j+1,s,l,0);
//             if(id.dead())continue;
//             for(int k=0;k<TKDBc::NStripsDrp(i+1,l);k++){
// 	      id.upd(k);
// 	      float sig=id.getsig();
// 	      if(id.checkstatus(AMSDBc::BAD)){
// 		HF1(201+l,sig,1.);
// 	      }
// 	      else {
// 		HF1(101+l,sig,1.);
// 	      }
//             }
// 	  }
// 	}
//       }
//     }

//   }

//   else TRMCFFKEY.year[1]=TRMCFFKEY.year[0]-1;

//   for(int l=0;l<2;l++){
//     // book noise prob
//     geant a=TRMCFFKEY.thr1R[l]-AMSTrMCCluster::step()/2.;
//     geant b=a+AMSTrMCCluster::ncha()*AMSTrMCCluster::step();
//     HBOOK1(AMSTrMCCluster::hid(l),"x prob",AMSTrMCCluster::ncha(),a,b,0.);
//   }
  AMSgObj::BookTimer.book("GEANTTRACKING");
  AMSgObj::BookTimer.book("GUSTEP",10);
  AMSgObj::BookTimer.book("AMSGUSTEP",10);
  AMSgObj::BookTimer.book("SYSGUSTEP",10);
  AMSgObj::BookTimer.book("TrdRadiationGen",10);
  AMSgObj::BookTimer.book("SITKHITS");
  AMSgObj::BookTimer.book("SITKNOISE");
  AMSgObj::BookTimer.book("SITKDIGI");
  AMSgObj::BookTimer.book("SITKDIGIa");
  AMSgObj::BookTimer.book("SITKDIGIb");
  AMSgObj::BookTimer.book("SITKDIGIc");
  AMSgObj::BookTimer.book("TrMCCluster");

  AMSgObj::BookTimer.book("SiTkSimuAll");

  AMSgObj::BookTimer.book("SiTkDigiAll");
  AMSgObj::BookTimer.book("SiTkDigiShow");
  AMSgObj::BookTimer.book("SiTkDigiLadd");
  AMSgObj::BookTimer.book("SiTkDigiNoise");
  AMSgObj::BookTimer.book("SiTkDigiDsp");
  AMSgObj::BookTimer.book("SiTkDigiFake");
  cout << "SiTkInitJob-I-Initialize TrSimSensors" << endl;
  TrSim::InitSensors();
}

//------------------------------------------------------


void AMSJob::_catkinitjob(){
// PZ FIXME CALIB  cout <<" AMSJob::_catkinitjob()-i_Initialized for Proc no  "<<TRCALIB.CalibProcedureNo<<endl;
//   AMSgObj::BookTimer.book("CalTrFill");
//   AMSgObj::BookTimer.book("CalTrFit");
//   if(TRALIG.UpdateDB){

//     AMSTrAligFit::Fillgl(gethead()->addup( new AMSTrAligFit()));
//     //  AMSTrAligFit::Fillgle(gethead()->addup( new AMSTrAligFit()));
//   }
//   if(TRCALIB.CalibProcedureNo == 1 || TRCALIB.CalibProcedureNo==4){
//     AMSTrIdCalib::initcalib();
//   }
//   else if(TRCALIB.CalibProcedureNo == 2){
//     int i,j;
//     for(i=0;i<nalg;i++){
//       for(j=0;j<tkcalpat;j++){
// 	AMSTrCalibFit::setHead(i,j, new 
// 			       AMSTrCalibFit(j,TRCALIB.EventsPerIteration[i],TRCALIB.NumberOfIterations[i],i,GCKINE.ikine));
//       }
//     }
//   }
}
