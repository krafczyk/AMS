#include"richlip.h"
// #include<math.h>
// #include"richid.h"


void RichLIPRec::InitGlobal(){

  // display flag
  LIPC2F.idispflag = RICRECFFKEY.lipdisp;

  // job data
  LIPC2F.jobc_cp_c2f = 0; // means this is simulation - should come from code

  // detector data
  LIPC2F.ztoprad_ams_c2f = RICHDB::RICradpos()+RICHDB::rad_agl_height-RICHDB::rad_height;
  LIPC2F.rcgeom_c2f[0] = 0;                           
  LIPC2F.rcgeom_c2f[1] = 0;
  LIPC2F.levgeom_c2f = 4;
  LIPC2F.levgrad_c2f = 2;
  LIPC2F.levacc_c2f = 0; // 0 = faster version (1 = slower)
  LIPC2F.levghit_c2f = 1;
  LIPC2F.ztarg_c2f = 0.;
  LIPC2F.nradts_c2f = 1;
  LIPC2F.hrad_c2f = RICHDB::rad_agl_height;
  LIPC2F.hrnaf_c2f = RICHDB::naf_height;
  LIPC2F.radtile_pitch_c2f = RICHDB::rad_length+RICaethk;
  LIPC2F.radtile_supthk_c2f = RICaethk;
  LIPC2F.radix_c2f[0] = RICHDB::rad_index;
  LIPC2F.radix_c2f[1] = RICHDB::naf_index;
  LIPC2F.radclarity_c2f = RICHDB::rad_clarity;
  LIPC2F.hpgl_c2f = RICHDB::foil_height;
  LIPC2F.ztmirgap_c2f = RICradmirgap;
  LIPC2F.zbmirgap_c2f = RIClgdmirgap;
  LIPC2F.reflec_c2f = RICmireff;
  LIPC2F.zlgsignal_c2f = 1.8;
  LIPC2F.rtmir_c2f = RICHDB::top_radius;
  LIPC2F.rbmir_c2f = RICHDB::bottom_radius;
  LIPC2F.hmir_c2f = RICHDB::rich_height;
  LIPC2F.nmirsec_c2f = 3;
  LIPC2F.idmirs_c2f[0] = 3;
  LIPC2F.idmirs_c2f[1] = 2;
  LIPC2F.idmirs_c2f[2] = 1;
  LIPC2F.phimirs_c2f[0] = 328./180.*3.14159265;
  LIPC2F.phimirs_c2f[1] = 88./180.*3.14159265;
  LIPC2F.phimirs_c2f[2] = 208./180.*3.14159265;
  LIPC2F.refmirs_c2f[0] = 0.85;
  LIPC2F.refmirs_c2f[1] = 0.85;
  LIPC2F.refmirs_c2f[2] = 0.85;
  LIPC2F.pmtwx_c2f = 3.4; // FIND WHERE THIS IS IN FULL SIM CODE
  LIPC2F.pmtwy_c2f = 3.4; // FIND WHERE THIS IS IN FULL SIM CODE
  LIPC2F.shieldw_c2f = 0.3; // FIND WHERE THIS IS IN FULL SIM CODE
  LIPC2F.pglix_c2f = RICHDB::foil_index;
  LIPC2F.emcxlim_c2f = RICHDB::hole_radius[0];
  LIPC2F.emcylim_c2f = RICHDB::hole_radius[1];
  LIPC2F.lg_top_width_c2f = 3.4; // FIND WHERE THIS IS IN FULL SIM CODE
  LIPC2F.lg_bot_width_c2f = 1.77; // FIND WHERE THIS IS IN FULL SIM CODE
  LIPC2F.lg_pitch_c2f = 3.7; // FIND WHERE THIS IS IN FULL SIM CODE
  LIPC2F.xbc_c2f = 30.1;
  LIPC2F.xbd_c2f = 64.5;
  LIPC2F.yef_c2f = 60.3;
  LIPC2F.xpc_c2f = 0.25;
}


void RichLIPRec::InitEvent() {

  // global detector data were moved to initialization routines in job.C

  // hit data

  LIPC2F.nbhitsmax_ntup_ev = LIP_NHITMAX;

  int actual = 0;
  LIPC2F.nbhits_ev = 0;

  for(AMSRichRawEvent* hit=(AMSRichRawEvent *)AMSEvent::gethead()->
	getheadC("AMSRichRawEvent",0);hit;hit=hit->next()){
    if((hit->getchannelstatus()%10)!=Status_good_channel) continue;

    if(actual>=LIP_NBHITSMAX) {
      cout << "RichLIPRec::InitEvent : Event too long."<<endl;
      break;
    }

    LIPC2F.nbhits_ev++;

    LIPC2F.hitsnpe_ev[actual]=hit->getnpe();
    RichPMTChannel hitch(hit->getchannel());

    coordrich2lip(hitch.x(),hitch.y(),hitch.z(),LIPC2F.hitscoo_ev[actual][0],LIPC2F.hitscoo_ev[actual][1],LIPC2F.hitscoo_ev[actual][2]);

    //LIPC2F.hitscoo_ev[actual][0]=hitch.x();
    //LIPC2F.hitscoo_ev[actual][1]=hitch.y();
    //LIPC2F.hitscoo_ev[actual][2]=RICHDB::RICradpos()+RICHDB::rad_agl_height-RICHDB::rad_height-hitch.z(); // Z : AMS frame -> RICH frame

    int hgain=hit->getbit(gain_mode_bit);

    LIPC2F.hitspmt_ev[actual]=10*(16*hitch.pmt_geom_id+hitch.channel_geom_id)+hgain;

    actual++;
  }

  //cout << "LIPC2F.nbhits_ev = " << LIPC2F.nbhits_ev << endl;

  // TOF cluster data

  // Get TOF cluster data

  LIPC2F.ntofclu = 0;
  LIPC2F.iflag_tof = 0;

  for(int ilay=0;ilay<TOF2GC::SCLRS;ilay++){ // loop over layers
    AMSTOFCluster* ptrc=(AMSTOFCluster*)AMSEvent::gethead()->
                           getheadC("AMSTOFCluster",ilay,0);
    while(ptrc && LIPC2F.ntofclu<LIP_NMAXTOFCLU){ // loop over AMSTofCluster hits in layer
      LIPC2F.istatus_tof[LIPC2F.ntofclu] = ptrc->getstatus();
      LIPC2F.ilayer_tof[LIPC2F.ntofclu] = ptrc->getntof();
      LIPC2F.ibar_tof[LIPC2F.ntofclu] = ptrc->getplane();
      LIPC2F.edep_tof[LIPC2F.ntofclu] = ptrc->getedep();
      LIPC2F.edepd_tof[LIPC2F.ntofclu] = ptrc->getedepd();
      LIPC2F.time_tof[LIPC2F.ntofclu] = ptrc->gettime();
      LIPC2F.errtime_tof[LIPC2F.ntofclu] = ptrc->getetime();

      coordams2lip(ptrc->getcoo()[0],ptrc->getcoo()[1],ptrc->getcoo()[2],LIPC2F.coo_tof[LIPC2F.ntofclu][0],LIPC2F.coo_tof[LIPC2F.ntofclu][1],LIPC2F.coo_tof[LIPC2F.ntofclu][2]);
      for(int i=0;i<3;i++) {
	LIPC2F.errcoo_tof[LIPC2F.ntofclu][i] = ptrc->getecoo()[i];
      }

      //for(int i=0;i<3;i++) {
      //LIPC2F.coo_tof[LIPC2F.ntofclu][i] = ptrc->getcoo()[i];
      //LIPC2F.errcoo_tof[LIPC2F.ntofclu][i] = ptrc->getecoo()[i];
      //}
      // // change Z coordinate to LIP RICH reference frame
      //LIPC2F.coo_tof[LIPC2F.ntofclu][2] = RICHDB::RICradpos()+RICHDB::rad_agl_height-RICHDB::rad_height-LIPC2F.coo_tof[LIPC2F.ntofclu][2];

      //cout << "\nTOF cluster " << LIPC2F.ntofclu << endl;
      //cout << "-------------" << endl;
      //cout << "istatus = " << LIPC2F.istatus_tof[LIPC2F.ntofclu] << endl;
      //cout << "ilayer = " << LIPC2F.ilayer_tof[LIPC2F.ntofclu] << endl;
      //cout << "ibar = " << LIPC2F.ibar_tof[LIPC2F.ntofclu] << endl;
      //cout << "edep = " << LIPC2F.edep_tof[LIPC2F.ntofclu] << endl;
      //cout << "edepd = " << LIPC2F.edepd_tof[LIPC2F.ntofclu] << endl;
      //cout << "time = " << LIPC2F.time_tof[LIPC2F.ntofclu] << endl;
      //cout << "errtime = " << LIPC2F.errtime_tof[LIPC2F.ntofclu] << endl;
      //cout << "coo = " << LIPC2F.coo_tof[LIPC2F.ntofclu][0] << "," << LIPC2F.coo_tof[LIPC2F.ntofclu][1] << "," << LIPC2F.coo_tof[LIPC2F.ntofclu][2] << endl;
      //cout << "errcoo = " << LIPC2F.errcoo_tof[LIPC2F.ntofclu][0] << "," << LIPC2F.errcoo_tof[LIPC2F.ntofclu][1] << "," << LIPC2F.errcoo_tof[LIPC2F.ntofclu][2] << endl;

      LIPC2F.ntofclu++;
      ptrc = ptrc->next();
    }
  }

  // Get track hint in RICH matrix, add it as if it was a hit in a 5th TOF plane

  float richhintnpelimit = 6.; // minimum signal for hint to be used
  float errcoohint = 1.2;  // error (cm) to be used for hint in each of x,y,z

  float richhintx,richhinty,richhintnpe;
  RICHGETMATRIXHINT(richhintx,richhinty,richhintnpe);

  //cout << "RICH hint for TOF cluster list: " << richhintx << ", " << richhinty << ", " << richhintnpe << endl;

  if(richhintnpe>richhintnpelimit) {

    LIPC2F.istatus_tof[LIPC2F.ntofclu] = 0;
    LIPC2F.ilayer_tof[LIPC2F.ntofclu] = 5;  // 5th plane assigned here
    LIPC2F.ibar_tof[LIPC2F.ntofclu] = 0;
    LIPC2F.edep_tof[LIPC2F.ntofclu] = 0;
    LIPC2F.edepd_tof[LIPC2F.ntofclu] = 0;
    LIPC2F.time_tof[LIPC2F.ntofclu] = 0;
    LIPC2F.errtime_tof[LIPC2F.ntofclu] = 0;

    LIPC2F.coo_tof[LIPC2F.ntofclu][0] = richhintx;
    LIPC2F.coo_tof[LIPC2F.ntofclu][1] = richhinty;
    LIPC2F.coo_tof[LIPC2F.ntofclu][2] = LIPC2F.hrad_c2f+LIPC2F.hpgl_c2f+LIPC2F.ztmirgap_c2f+LIPC2F.hmir_c2f+LIPC2F.zbmirgap_c2f+LIPC2F.zlgsignal_c2f;

    LIPC2F.errcoo_tof[LIPC2F.ntofclu][0] = errcoohint;
    LIPC2F.errcoo_tof[LIPC2F.ntofclu][1] = errcoohint;
    LIPC2F.errcoo_tof[LIPC2F.ntofclu][2] = errcoohint;

    LIPC2F.ntofclu++;
  }

  // reset track and reconstruction counters
  LIPC2F.irecnumb = -1;  // value is increased in beginning of cycle, so 1st reconstruction will have number 0
  LIPC2F.itrknumb = -1;  // value is increased in beginning of cycle, so 1st track will have number 0

  int ievnumb = AMSEvent::gethead()->getEvent();

  RICHRECLIP(0,-1); // initialize Fortran variables related to global job data
                    // (this is not done only once in InitGlobal since variables in
                    // LIP Fortran commons are not protected by the copyin statement)
  RICHRECLIP(ievnumb,-2);  // initialize global event data inside LIP Fortran code
}


void RichLIPRec::InitTrack(AMSPoint entrance_p, AMSDir entrance_d) {

#define PI 3.14159265359
#define SQR(x) ((x)*(x))

  // LIPVAR values (kept for debugging)
  LIPVAR.liphused = 0;
  LIPVAR.lipbeta = 0.;
  LIPVAR.lipthc = 0.;
  LIPVAR.liplikep = 99999.;
  LIPVAR.lipchi2 = 99999.;
  LIPVAR.liprprob = 99999.;

  // simulated main track parameters

  if(AMSJob::gethead()->isSimulation()){
    AMSmceventg *pmcg=(AMSmceventg*)AMSEvent::gethead()->getheadC("AMSmceventg",0);
    number pmass=pmcg->getmass();
    LIPC2F.pmom_sim = pmcg->getmom();
    LIPC2F.pbeta_sim = LIPC2F.pmom_sim/sqrt(SQR(LIPC2F.pmom_sim)+SQR(pmass));
    LIPC2F.pchg_sim = pmcg->getcharge();
  }
  else {
    LIPC2F.pmom_sim = -999.;
    LIPC2F.pbeta_sim = -999.;
    LIPC2F.pchg_sim = -999.;
  }

  LIPC2F.pthe_sim = 0.; // SHOULD BE FILLED IF SIMULATION
  LIPC2F.pphi_sim = 0.; // SHOULD BE FILLED IF SIMULATION

  LIPC2F.pimp_sim[0] = 0.; // SHOULD BE FILLED IF SIMULATION
  LIPC2F.pimp_sim[1] = 0.; // SHOULD BE FILLED IF SIMULATION
  LIPC2F.pimp_sim[2] = 0.; // SHOULD BE FILLED IF SIMULATION

  // main track parameters (errors assumed to be zero for the moment)
  anglerich2lip(entrance_d.gettheta(),entrance_d.getphi(),LIPC2F.pthe_main,LIPC2F.pphi_main);
  // swap theta/phi vector if it points upwards (added by R. Pereira 01-Apr-2011)
  if(LIPC2F.pthe_main>3.14159265/2.) {
    LIPC2F.pthe_main = 3.14159265-LIPC2F.pthe_main;
    LIPC2F.pphi_main = LIPC2F.pphi_main+3.14159265;
    if(LIPC2F.pphi_main>3.14159265) {
      LIPC2F.pphi_main = LIPC2F.pphi_main-2.*3.14159265;
    }
  }

  LIPC2F.epthe_main = 0.;
  LIPC2F.epphi_main = 0.;

  AMSPoint ptmp = entrance_p;
  //cout << "INTERMEDIATE: ptmp = " << ptmp[0] << "," << ptmp[1] << "," << ptmp[2] << endl;

  float plocal0 = ptmp[0]+tan(LIPC2F.pthe_main)*cos(LIPC2F.pphi_main)*(ptmp[2]-(RICHDB::RICradpos()+RICHDB::rad_agl_height-RICHDB::rad_height));
  float plocal1 = ptmp[1]+tan(LIPC2F.pthe_main)*sin(LIPC2F.pphi_main)*(ptmp[2]-(RICHDB::RICradpos()+RICHDB::rad_agl_height-RICHDB::rad_height));
  float plocal2 = 0.;
  //cout << "INTERMEDIATE: plocal = " << plocal0 << "," << plocal1 << "," << plocal2 << endl;

  LIPC2F.pimp_main[0] = plocal0;
  LIPC2F.pimp_main[1] = plocal1;
  LIPC2F.pimp_main[2] = plocal2;

  //cout << "RESULT: pimp = " << LIPC2F.pimp_main[0] << "," << LIPC2F.pimp_main[1] << "," << LIPC2F.pimp_main[2] << endl; 

  LIPC2F.epimp_main[0] = 0.;
  LIPC2F.epimp_main[1] = 0.;
  LIPC2F.epimp_main[2] = 0.;

  LIPC2F.pmom_main = 0.; // SHOULD BE FILLED (currently not used in LIP rec)
  LIPC2F.prad_main = 0; // SHOULD BE FILLED (currently not used in LIP rec)
}


int RichLIPRec::goodLIPREC() {
  int nr = LIPC2F.irecnumb;
  return (LIPF2C.resb_iflag[nr]==1);
}


// conversion between AMS and LIP coordinate systems

void RichLIPRec::coordams2lip(float pxams, float pyams, float pzams, float &pxlip, float &pylip, float &pzlip) {  

  AMSPoint pointams(pxams,pyams,pzams);
  pointams = RichAlignment::AMSToRich(pointams);

  pxlip = pointams.x();
  pylip = pointams.y();
  pzlip = RICHDB::RICradpos()+RICHDB::rad_agl_height-RICHDB::rad_height-pointams.z();

  //cout << "CALL TO coordams2lip" << endl;
  //cout << "In - AMS: " << pxams << "," << pyams << "," << pzams << endl;
  //cout << "Out - LIP: " << pxlip << "," << pylip << "," << pzlip << endl;
}


void RichLIPRec::coordlip2ams(float pxlip, float pylip, float pzlip, float &pxams, float &pyams, float &pzams) {  

  float pxtmp = pxlip;
  float pytmp = pylip;
  float pztmp = RICHDB::RICradpos()+RICHDB::rad_agl_height-RICHDB::rad_height-pzlip;

  AMSPoint pointams(pxtmp,pytmp,pztmp);
  pointams = RichAlignment::RichToAMS(pointams);

  pxams = pointams.x();
  pyams = pointams.y();
  pzams = pointams.z();

  //cout << "CALL TO coordlip2ams" << endl;
  //cout << "In - LIP: " << pxlip << "," << pylip << "," << pzlip << endl;
  //cout << "Out - AMS: " << pxams << "," << pyams << "," << pzams << endl;
}


void RichLIPRec::angleams2lip(float theams, float phiams, float &thelip, float &philip) {

  AMSDir dirams(theams,phiams);
  dirams=RichAlignment::AMSToRich(dirams);

  float thetmp = dirams.gettheta();
  float phitmp = dirams.getphi();

  thelip = 3.14159265-thetmp;
  philip = phitmp;

  //cout << "CALL TO angleams2lip" << endl;
  //cout << "In - AMS (theta,phi in deg): " << theams*180./3.14159265 << "," << phiams*180./3.14159265 << endl;
  //cout << "Out - LIP (theta,phi in deg): " << thelip*180./3.14159265 << "," << philip*180./3.14159265 << endl;
}


void RichLIPRec::anglelip2ams(float thelip, float philip, float &theams, float &phiams) {

  float thetmp = 3.14159265-thelip;
  float phitmp = philip;

  AMSDir dirams(thetmp,phitmp);
  dirams=RichAlignment::RichToAMS(dirams);

  theams = dirams.gettheta();
  phiams = dirams.getphi();

  //cout << "CALL TO anglelip2ams" << endl;
  //cout << "In - LIP (theta,phi in deg): " << thelip*180./3.14159265 << "," << philip*180./3.14159265 << endl;
  //cout << "Out - AMS (theta,phi in deg): " << theams*180./3.14159265 << "," << phiams*180./3.14159265 << endl;
}


// conversion between RICH and LIP coordinate systems

void RichLIPRec::coordrich2lip(float pxrich, float pyrich, float pzrich, float &pxlip, float &pylip, float &pzlip) {  

  pxlip = pxrich;
  pylip = pyrich;
  pzlip = RICHDB::RICradpos()+RICHDB::rad_agl_height-RICHDB::rad_height-pzrich;

  //cout << "CALL TO coordrich2lip" << endl;
  //cout << "In - RICH: " << pxrich << "," << pyrich << "," << pzrich << endl;
  //cout << "Out - LIP: " << pxlip << "," << pylip << "," << pzlip << endl;
}


void RichLIPRec::coordlip2rich(float pxlip, float pylip, float pzlip, float &pxrich, float &pyrich, float &pzrich) {  

  pxrich = pxlip;
  pyrich = pylip;
  pzrich = RICHDB::RICradpos()+RICHDB::rad_agl_height-RICHDB::rad_height-pzlip;

  //cout << "CALL TO coordlip2rich" << endl;
  //cout << "In - LIP: " << pxlip << "," << pylip << "," << pzlip << endl;
  //cout << "Out - RICH: " << pxrich << "," << pyrich << "," << pzrich << endl;
}


void RichLIPRec::anglerich2lip(float therich, float phirich, float &thelip, float &philip) {

  thelip = 3.14159265-therich;
  philip = phirich;

  //cout << "CALL TO anglerich2lip" << endl;
  //cout << "In - RICH (theta,phi in deg): " << therich*180./3.14159265 << "," << phirich*180./3.14159265 << endl;
  //cout << "Out - LIP (theta,phi in deg): " << thelip*180./3.14159265 << "," << philip*180./3.14159265 << endl;
}


void RichLIPRec::anglelip2rich(float thelip, float philip, float &therich, float &phirich) {

  therich = 3.14159265-thelip;
  phirich = philip;

  //cout << "CALL TO anglelip2rich" << endl;
  //cout << "In - LIP (theta,phi in deg): " << thelip*180./3.14159265 << "," << philip*180./3.14159265 << endl;
  //cout << "Out - RICH (theta,phi in deg): " << therich*180./3.14159265 << "," << phirich*180./3.14159265 << endl;
}


////////////////////////////////////////////////////////
// Wrapper to call some member functions from FORTRAN //
////////////////////////////////////////////////////////


extern "C" void lipgetrad_(float *x,float *y,float *z,float *theta,float *phi, int *radkind, double *radindex, int *radtile){
  
  float xams,yams,zams;
  float thetaams,phiams;
  number radentp[3];
  number radentd[3];

  // Convert arguments to AMS coordinates
  RichLIPRec::coordlip2ams(*x,*y,*z,xams,yams,zams);
  RichLIPRec::anglelip2ams(*theta,*phi,thetaams,phiams);

  // All the coordinates should be in the AMSPoint
  AMSTrTrack track(thetaams,phiams,AMSPoint(xams,yams,zams));
  RichRadiatorTileManager crossed_tile(&track);  
  
  // Save output to corresponding variables
  *radkind = crossed_tile.getkind();
  *radindex = crossed_tile.getindex();
  *radtile = crossed_tile.getcurrenttile();
}
 
 
extern "C" void lipfzphemi_(int *radtile, float *zphemiloc){
  *zphemiloc = AMSRichRingNewSet::getzphemiloc(*radtile);
}
