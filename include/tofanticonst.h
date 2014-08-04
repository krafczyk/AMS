//  $Id: tofanticonst.h,v 1.1 2010/12/11 18:30:39 choutko Exp $
#ifndef __TOFANTICONST__
#define __TOFANTICONST__
namespace TOFGC{
  const integer AMSDISL=500;//max length of distributions in AMSDistr class
  const integer SCWORM=256;// Max.length (in 16-bits words) in bit-stream class
  const integer SCBITM=SCWORM*16;// same in bits(-> 2.048 mks with 0.5 ns binning)
  const integer SCBADB1=128; // status bit to mark counter with bad "history"(for ANTI - NoFTCoinc.on 2sides)
  const integer SCBADB2=256; // status bit to mark counter with only one_side measurement
  const integer SCBADB3=512; // ... bad for t-meas.based on DB(don't use for tzcalibr/beta-meas)
  const integer SCBADB4=1024; // missing side number(s1->no_bit,s2->set_bit, IF B2 is set !)
  const integer SCBADB5=2048; // set if missing side was recovered (when B2 is set)
  const integer SCBADB6=4096; // set if no "best"_LT-hit/SumHT matching on, at least, one of the alive side
  const integer SCBADB7=8192; // 
}
//-----
// TOF2 global constants definition
namespace TOF2GC{
//geometry :
const integer SCMXBR=10; // max nmb of bars/layer
const integer SCMXBR2=18;//max nmb of bars/top(bot) TOF-subsystem
const integer SCLRS=4; // max nmb of layers in TOF-systems
const integer SCROTN=2; // start nmb of abs. numbering of TOF rot. matrixes
const integer SCBTPN=11; //Real nmb of sc. bar types (different by length now)
const integer SCCHMX=SCLRS*SCMXBR*2; //MAX scint. channels(layers*bars*sides)
const integer SCBLMX=SCLRS*SCMXBR;   //MAX scint. bars*layers
const integer PMTSMX=3; // MAX number of PMTs per side
//MC
const integer TOFPNMX=1000;// max integral-bins  in TOFTpoints class
const integer SCANPNT=15; //max scan points in MC/REC t/a-calibration
const integer SCANTDL=400;//scan time distribution max.length(should be SCANTDL<AMSDISL) 
const integer SCANWDS=4; //max. width-divisions in the sc.paddle scan
const integer SCTBMX=10000;//length of MC "flash-ADC" buffer(-> 1mks with 0.1ns binning)
const integer SESPMX=3;//Max number of PMT single elecr.spectrum parameters
// 
//DAQ
const int16u SCPHBP=16384; // phase bit position in Reduced-format TDC word (15th bit)
const int16u SCPHBPA=32768;// phase bit position in Raw-format address word (16th bit)
const int16u SCADCMX=4095;// MAX. value in ADC (12bits-1)
const int16u SCPUXMX=3700;// MAX. value provided by PUX-chip(adc chan)
const int16u SCTDCCH=8;//input channels per TDC-chip(1 chip per SFET(A))

const integer SCCRAT=4; // number of crates with TOF(+ANTI)-channels (S-crates)
const integer SCSLTM=11;// number of slots(all types) per crate(max)
const integer SCSLTY=5;// slot(card) types(real: sfet,sfea,sfec,sdr,spt) 
const integer SCRCHM=13;// number of readout channels (outputs) per slot(card)(max)
const integer SCRCMX=SCCRAT*SCSLTM*SCRCHM;//total hw-readout-channels(max)
const integer SCCRCMX=SCSLTM*SCRCHM;//readout channel per crate (max)
const integer SCPMOU=4;//  number pm-outputs per side (max)(anode+3dynodes)
const integer SCMTYP=5;// number of measurement types(max)(t,q,FTt,SumHTt,SumSHTt)
const integer SCAMTS=2;// number of Anode measurements types(actualy t,q) per normal bar/side
const integer SCHPMTS=3;// number of half-plane measurement types(actually FT,SumHT,sumSHT) per side
const integer SCDMTS=1;// number of Dynode measurement types(actualy q)
const integer SCFETA=5;// number of SFET+SFEA card per crate(having temp-sensors)(actual)
//
const integer DAQSFMX=3;// number of non-empty format types(raw,compressed,mixed)
//RECO
const integer SCTHMX1=8;//max TDC FTrigger-channel hits
const integer SCTHMX2=16;//max TDC SumHT(SHT)-channel (history) hits  
const integer SCTHMX3=16;//max TDC LTtime-channel hits
const integer SCTHMX4=1;//max adca(anode) hits  
const integer SCJSTA=60;   //size of Job-statistics array
const integer SCCSTA=25;   //size of Channel-statistics array
const integer SCPROFP=6;//max. parameters/side in A-profile(Apm<->Yloc) fit
const integer SCPDFBM=100;//max bins in TOF-eloss Prob Density Functions(need additional 2 for ovfls)
const integer SCTHMX=16; // max(SCTHMX2,SCTHMX3);
//
//      Calibration:
// TDIF
const integer SCTDBM=17; //max. number of coord. bins for TDIF-calibration
// AMPL 
const integer SCACMX=2000;// max. number of accum. events per channel(or bin)
const integer SCPRBM=17;//max.bins for Ampl-profile along the counter
const integer SCBTBN=SCBTPN*SCPRBM;// max. bar_types(i.e.ref.bars) * max.bins
const integer SCELFT=4;     // max. number of param. for dE/dX fit
const integer SCMCIEVM=2000;//max. events for MC A-integrator calibration 
// AVSD
const integer SCACHB=500;//max.bins in Qa for "Qd vs Qa" fit
const number SCACBW=10.;// bin width in Qa binning (adc-channels)
// STRR
const integer SCSRCHB=1200;// max. bin for dtout (2000-6000ns)
const integer SCSRCLB=400;// min. bin for dtout
const integer SCSRCHBMC=1200;// max. bin for dtout(MC)(2000-6000ns)
const integer SCSRCLBMC=400;// min. bin for dtout (MC)
const number SCSRCTB=5.;// time binning for dtout (ns)
}
namespace ANTI2C{
 const integer ANTISRS=16;// number of physical sectors 
 const integer MAXANTI=ANTISRS/2; // number of logical(readout) sectors(8)
 const integer ANCHMX=2*MAXANTI;// number of readout channels(inputs)
 const integer ANAMTS=2;// number of Anode measurements types for normal sectors(time/charge) 
 const integer ANAGMTS=1;// number of Anode measurements types for half-cylinder(FT) 
 const integer ANAHMX=1; // max. number of anode-charge hits  per chan.
 const integer ANTHMX=16; // max. number of LTtime-hits per channel
 const integer ANFADC=2000;//Flash-ADC working channels(ovfl not included)
 const integer ANJSTA=35;// size of Job-statistics array 
 const integer ANCSTA=10;// size of Channel-statistics array
 const integer ANCRAT=4; // number of crates with SFEA cards
 const integer ANSFEA=1; // number of SFEA cards per crate
 const integer ANCHSF=16;// max. number of TDC-channels per SFEA (incl. FT-channel)
 const integer ANCHCH=4;// real number of Anti-channels per TDC-chip (impl. 2chip/SFEA)
 const integer ANSLOT=5; // SFEA card position in crate (sequential slot number(1-6))
// calibr.const:
//---> gain/attlen calib:
 const integer LongBins=20;//max longit.bins
 const integer BinEvsMX=2000;//max event/bin
 const integer CalChans=2*ANTI2C::ANTISRS;//max calib.channels, id=2*(2*logs+phys)+side
 
  const integer ANACMX=2000;// max. number of accum. events per channel(or bin)
  const integer ANPRBM=11;//max.bins for Ampl-profile along the counter
  const integer ANELFT=4;     // max. number of param. for Eloss-shape(landau) fit
  const integer ANPROFP=4;//max. parameters/side in A-profile(Apm<->Yloc) fit
//
}
#endif
