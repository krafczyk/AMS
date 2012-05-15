#include"richlip.h"
// #include<math.h>
// #include"richid.h"


short int pmtconv_ams2lip[680] = {712,713,714,715,716,717,702,703,704,705,
				  706,707,708,709,693,694,695,696,697,698,
				  699,700,684,685,686,687,688,689,690,691,
				  692,675,676,677,678,679,680,681,682,683,
				  666,667,668,669,670,671,672,673,674,657,
				  658,659,660,661,662,663,664,665,648,649,
				  650,651,652,653,654,655,656,639,640,641,
				  642,643,644,645,646,647,630,631,632,633,
				  634,635,636,637,638,621,622,623,624,625,
				  626,627,628,629,612,613,614,615,616,617,
				  618,619,620,603,604,605,606,607,608,609,
				  610,611,594,595,596,597,598,599,600,601,
				  602,585,586,587,588,589,590,591,592,576,
				  577,578,579,580,581,582,583,568,569,570,
				  571,572,573,564,561,562,557,558,559,552,
				  553,554,555,546,547,548,549,550,539,540,
				  541,542,543,544,532,533,534,535,536,537,
				  523,524,525,526,527,528,513,514,515,516,
				  517,518,519,520,504,505,506,507,508,509,
				  510,511,495,496,497,498,499,500,501,502,
				  503,486,487,488,489,490,491,492,493,494,
				  477,478,479,480,481,482,483,484,485,468,
				  469,470,471,472,473,474,475,476,459,460,
				  461,462,463,464,465,466,467,450,451,452,
				  453,454,455,456,457,458,441,442,443,444,
				  445,446,447,448,449,432,433,434,435,436,
				  437,438,439,440,423,424,425,426,427,428,
				  429,430,431,414,415,416,417,418,419,420,
				  421,422,405,406,407,408,409,410,411,412,
				  413,396,397,398,399,400,401,402,403,387,
				  388,389,390,391,392,393,394,379,380,381,
				  382,383,384,375,372,373,368,369,370,363,
				  364,365,366,357,358,359,360,361,350,351,
				  352,353,354,355,343,344,345,346,347,348,
				  334,335,336,337,338,339,324,325,326,327,
				  328,329,330,331,315,316,317,318,319,320,
				  321,322,306,307,308,309,310,311,312,313,
				  314,297,298,299,300,301,302,303,304,305,
				  288,289,290,291,292,293,294,295,296,279,
				  280,281,282,283,284,285,286,287,270,271,
				  272,273,274,275,276,277,278,261,262,263,
				  264,265,266,267,268,269,252,253,254,255,
				  256,257,258,259,260,243,244,245,246,247,
				  248,249,250,251,234,235,236,237,238,239,
				  240,241,242,225,226,227,228,229,230,231,
				  232,233,216,217,218,219,220,221,222,223,
				  224,207,208,209,210,211,212,213,214,198,
				  199,200,201,202,203,204,205,190,191,192,
				  193,194,195,186,183,184,179,180,181,174,
				  175,176,177,168,169,170,171,172,161,162,
				  163,164,165,166,154,155,156,157,158,159,
				  145,146,147,148,149,150,135,136,137,138,
				  139,140,141,142,126,127,128,129,130,131,
				  132,133,117,118,119,120,121,122,123,124,
				  125,108,109,110,111,112,113,114,115,116,
				   99,100,101,102,103,104,105,106,107, 90,
				   91, 92, 93, 94, 95, 96, 97, 98, 81, 82,
				   83, 84, 85, 86, 87, 88, 89, 72, 73, 74,
				   75, 76, 77, 78, 79, 80, 63, 64, 65, 66,
				   67, 68, 69, 70, 71, 54, 55, 56, 57, 58,
				   59, 60, 61, 62, 45, 46, 47, 48, 49, 50,
				   51, 52, 53, 36, 37, 38, 39, 40, 41, 42,
				   43, 44, 27, 28, 29, 30, 31, 32, 33, 34,
				   35, 18, 19, 20, 21, 22, 23, 24, 25,  9,
				   10, 11, 12, 13, 14, 15, 16,  1,  2,  3,
				    4,  5,  6,753,750,751,746,747,748,741,
				  742,743,744,735,736,737,738,739,728,729,
				  730,731,732,733,721,722,723,724,725,726};
#pragma omp threadprivate(pmtconv_ams2lip)

short int pmtconv_lip2ams[756] = { -1,647,648,649,650,651,652, -1, -1,639,
				  640,641,642,643,644,645,646, -1,631,632,
				  633,634,635,636,637,638, -1,622,623,624,
				  625,626,627,628,629,630,613,614,615,616,
				  617,618,619,620,621,604,605,606,607,608,
				  609,610,611,612,595,596,597,598,599,600,
				  601,602,603,586,587,588,589,590,591,592,
				  593,594,577,578,579,580,581,582,583,584,
				  585,568,569,570,571,572,573,574,575,576,
				  559,560,561,562,563,564,565,566,567,550,
				  551,552,553,554,555,556,557,558,541,542,
				  543,544,545,546,547,548,549,532,533,534,
				  535,536,537,538,539,540,524,525,526,527,
				  528,529,530,531, -1,516,517,518,519,520,
				  521,522,523, -1, -1,510,511,512,513,514,
				  515, -1, -1, -1,504,505,506,507,508,509,
				   -1,498,499,500,501,502,503, -1,493,494,
				  495,496,497, -1,489,490,491,492, -1,486,
				  487,488, -1,484,485, -1,483, -1, -1, -1,
				  477,478,479,480,481,482, -1, -1,469,470,
				  471,472,473,474,475,476, -1,461,462,463,
				  464,465,466,467,468, -1,452,453,454,455,
				  456,457,458,459,460,443,444,445,446,447,
				  448,449,450,451,434,435,436,437,438,439,
				  440,441,442,425,426,427,428,429,430,431,
				  432,433,416,417,418,419,420,421,422,423,
				  424,407,408,409,410,411,412,413,414,415,
				  398,399,400,401,402,403,404,405,406,389,
				  390,391,392,393,394,395,396,397,380,381,
				  382,383,384,385,386,387,388,371,372,373,
				  374,375,376,377,378,379,362,363,364,365,
				  366,367,368,369,370,354,355,356,357,358,
				  359,360,361, -1,346,347,348,349,350,351,
				  352,353, -1, -1,340,341,342,343,344,345,
				   -1, -1, -1,334,335,336,337,338,339, -1,
				  328,329,330,331,332,333, -1,323,324,325,
				  326,327, -1,319,320,321,322, -1,316,317,
				  318, -1,314,315, -1,313, -1, -1, -1,307,
				  308,309,310,311,312, -1, -1,299,300,301,
				  302,303,304,305,306, -1,291,292,293,294,
				  295,296,297,298, -1,282,283,284,285,286,
				  287,288,289,290,273,274,275,276,277,278,
				  279,280,281,264,265,266,267,268,269,270,
				  271,272,255,256,257,258,259,260,261,262,
				  263,246,247,248,249,250,251,252,253,254,
				  237,238,239,240,241,242,243,244,245,228,
				  229,230,231,232,233,234,235,236,219,220,
				  221,222,223,224,225,226,227,210,211,212,
				  213,214,215,216,217,218,201,202,203,204,
				  205,206,207,208,209,192,193,194,195,196,
				  197,198,199,200,184,185,186,187,188,189,
				  190,191, -1,176,177,178,179,180,181,182,
				  183, -1, -1,170,171,172,173,174,175, -1,
				   -1, -1,164,165,166,167,168,169, -1,158,
				  159,160,161,162,163, -1,153,154,155,156,
				  157, -1,149,150,151,152, -1,146,147,148,
				   -1,144,145, -1,143, -1, -1, -1,137,138,
				  139,140,141,142, -1, -1,129,130,131,132,
				  133,134,135,136, -1,121,122,123,124,125,
				  126,127,128, -1,112,113,114,115,116,117,
				  118,119,120,103,104,105,106,107,108,109,
				  110,111, 94, 95, 96, 97, 98, 99,100,101,
				  102, 85, 86, 87, 88, 89, 90, 91, 92, 93,
				   76, 77, 78, 79, 80, 81, 82, 83, 84, 67,
				   68, 69, 70, 71, 72, 73, 74, 75, 58, 59,
				   60, 61, 62, 63, 64, 65, 66, 49, 50, 51,
				   52, 53, 54, 55, 56, 57, 40, 41, 42, 43,
				   44, 45, 46, 47, 48, 31, 32, 33, 34, 35,
				   36, 37, 38, 39, 22, 23, 24, 25, 26, 27,
				   28, 29, 30, 14, 15, 16, 17, 18, 19, 20,
				   21, -1,  6,  7,  8,  9, 10, 11, 12, 13,
				   -1, -1,  0,  1,  2,  3,  4,  5, -1, -1,
				   -1,674,675,676,677,678,679, -1,668,669,
				  670,671,672,673, -1,663,664,665,666,667,
				   -1,659,660,661,662, -1,656,657,658, -1,
				  654,655, -1,653, -1, -1};
#pragma omp threadprivate(pmtconv_lip2ams)

//F. Barao
short int RichLIPRec::badpix_ams[680] = {0};
short int RichLIPRec::badpix_lip[756];
short int RichLIPRec::badpmt_lip[756];

////////////////////////////////////////////////////////////////////////

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

  // parameters for alternative velocity algorithm
  // negative values mean defaults (initialized in Fortran)
  LIPC2F.facthcminscanagl_c2f = -999.;  // default 0.0
  LIPC2F.facthcmaxscanagl_c2f = -999.;  // default 1.2
  LIPC2F.facthcminscannaf_c2f = -999.;  // default 0.0
  LIPC2F.facthcmaxscannaf_c2f = -999.;  // default 1.2
  LIPC2F.thcminscanagl_c2f = -999.;  // default 2 deg
  LIPC2F.thcmaxscanagl_c2f = -999.;  // default 80 deg
  LIPC2F.thcstepscanagl_c2f = -999.;  // default 0.5 deg
  LIPC2F.thcminscannaf_c2f = -999.;  // default 2 deg
  LIPC2F.thcmaxscannaf_c2f = -999.;  // default 80 deg
  LIPC2F.thcstepscannaf_c2f = -999.;  // default 0.25 deg
  LIPC2F.dvmaxagl_c2f = -999.;  // default 3 cm/deg
  LIPC2F.dvmaxnaf_c2f = -999.;  // default 10 cm/deg
  LIPC2F.tollinagl_c2f = -999.;  // default 0.01*dvmaxagl*thcstepscanagl
  LIPC2F.tollinnaf_c2f = -999.;  // default 0.01*dvmaxnaf*thcstepscannaf

  // PMT and pixel status

  //int maxbadpix = 3;  // max no. bad pixels for PMT to be flagged as good
  int maxbadpix = 0;  // changed from 3 to 0 in April 2012 update (R. Pereira)

  for(int i=0;i<680;i++) {
    badpix_ams[i] = 0;
    for(int j=0;j<16;j++) {
      if(!(RichPMTsManager::Status(i,j)%10 && RichPMTsManager::Mask(i,j))) {
	badpix_ams[i]++;
	//cout << "Bad pixel detected: PMT " << i << ", pixel " << j << endl;
      }
    }
  }

  // ------------------------------------------------------------------
  // tips for additional PMT exclusion (R. Pereira 18-Apr-2011)
  //
  // additional PMTs may be excluded (i.e. their hits ignored
  // and their surface considered as dead for acceptance calculations)
  // by adding, in this exact location, lines like:
  //
  // badpix_ams[###] = 16;
  //
  // where ### is the number of the PMT to flag.

  // List added in April 2012 update (R. Pereira)

  badpix_ams[29] = 16;
  badpix_ams[43] = 16;
  badpix_ams[205] = 16;
  badpix_ams[225] = 16;
  badpix_ams[305] = 16;
  badpix_ams[323] = 16;
  badpix_ams[366] = 16;
  badpix_ams[373] = 16;
  badpix_ams[439] = 16;
  badpix_ams[446] = 16;
  badpix_ams[452] = 16;
  badpix_ams[454] = 16;
  badpix_ams[456] = 16;
  badpix_ams[458] = 16;
  badpix_ams[460] = 16;
  badpix_ams[466] = 16;
  badpix_ams[473] = 16;
  badpix_ams[493] = 16;
  badpix_ams[510] = 16;
  badpix_ams[511] = 16;
  badpix_ams[512] = 16;
  badpix_ams[513] = 16;
  badpix_ams[514] = 16;
  badpix_ams[515] = 16;
  badpix_ams[628] = 16;
  badpix_ams[647] = 16;
  badpix_ams[652] = 16;
  badpix_ams[664] = 16;

  // ------------------------------------------------------------------

  //cout << "PMT bad pixels (AMS numbering): " << endl;
  for(int i=0;i<680;i++) {
    //cout << badpix_ams[i];
    if(i<679) {
      //cout << ",";
    }
    else {
      //cout << endl;
    }
  }

  //cout << "PMT bad pixels (LIP numbering): " << endl;
  for(int i=0;i<756;i++) {
    badpix_lip[i] = 16;
    if(RichLIPRec::PMTlip2ams(i)>=0) {
      badpix_lip[i] = badpix_ams[RichLIPRec::PMTlip2ams(i)];
    }
    //cout << badpix_lip[i];
    if(i<755) {
      //cout << ",";
    }
    else {
      //cout << endl;
    }
  }

  //cout << "PMT LIP status (LIP numbering): " << endl;
  for(int i=0;i<756;i++) {
    badpmt_lip[i] = 1;  // non-existing PMT
    if(RichLIPRec::PMTlip2ams(i)>=0) { // existing PMT
      if(badpix_lip[i]>maxbadpix) {
	badpmt_lip[i] = 3; // bad
      }
      else {
	badpmt_lip[i] = 0; // good
      }
    }
    LIPC2F.ipmtstat_c2f[i] = badpmt_lip[i];
    //cout << badpmt_lip[i];
    if(i<755) {
      //cout << ",";
    }
    else {
      //cout << endl;
    }
  }  

}


////////////////////////////////////////////////////////////////////////

void RichLIPRec::InitEvent() {

  // global detector data were moved to initialization routines in job.C

  // hit data

  LIPC2F.nbhitsmax_ntup_ev = LIP_NHITMAX;

  actual = 0;
  LIPC2F.nbhits_ev = 0;

  totalhits = 0;

  for(AMSRichRawEvent* hit=(AMSRichRawEvent *)AMSEvent::gethead()->
	getheadC("AMSRichRawEvent",0);hit;hit=hit->next()){

    totalhits++;
    hitinlip[totalhits-1] = 0;

    if(actual>=LIP_NBHITSMAX) {
      //cout << "RichLIPRec::InitEvent : Event too long."<<endl;
      //break;
      continue;  // instead of break, to allow flagging of all hits up to last one
    }

    hitinlip[totalhits-1] = 1;
    LIPC2F.nbhits_ev++;

    LIPC2F.hitsnpe_ev[actual]=hit->getnpe();
    RichPMTChannel hitch(hit->getchannel());

    coordrich2lip(hitch.x(),hitch.y(),hitch.z(),LIPC2F.hitscoo_ev[actual][0],LIPC2F.hitscoo_ev[actual][1],LIPC2F.hitscoo_ev[actual][2]);

    //LIPC2F.hitscoo_ev[actual][0]=hitch.x();
    //LIPC2F.hitscoo_ev[actual][1]=hitch.y();
    //LIPC2F.hitscoo_ev[actual][2]=RICHDB::RICradpos()+RICHDB::rad_agl_height-RICHDB::rad_height-hitch.z(); // Z : AMS frame -> RICH frame

    int hgain=hit->getbit(gain_mode_bit);

    LIPC2F.hitspmt_ev[actual]=10*(16*hitch.pmt_geom_id+hitch.channel_geom_id)+hgain;
    LIPC2F.hitstat_ev[actual]=0;

    if((hit->getchannelstatus()%10)!=Status_good_channel) LIPC2F.hitstat_ev[actual]=1;  // flag hits in bad channels (from DB)
    if(badpmt_lip[RichLIPRec::PMTams2lip(hit->getchannel()/16)]!=0) LIPC2F.hitstat_ev[actual]=1;  // flag hits in bad PMTs according to LIP criteria

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


////////////////////////////////////////////////////////////////////////

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


////////////////////////////////////////////////////////////////////////


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


int RichLIPRec::PMTams2lip(int PMTams) {

  if(PMTams>=0 && PMTams<=679) {
    return (int) pmtconv_ams2lip[PMTams];
  }
  else {
    return -1;
  }

}


int RichLIPRec::PMTlip2ams(int PMTlip) {

  if(PMTlip>=0 && PMTlip<=755) {
    return (int) pmtconv_lip2ams[PMTlip];
  }
  else {
    return -1;
  }

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
