//  $Id: ecid.h,v 1.9 2002/10/03 14:35:58 choutko Exp $
#ifndef __AMSECID__
#define __AMSECID__
#include <typedefs.h>
#include <amsdbc.h>
#include <iostream.h>
#include <amsstl.h>
#include <commons.h>
#include <link.h>
#include <job.h>
#include <ecaldbc.h>
class AMSECIdGeom{
protected:
integer _pmt;
// ...  add here ...  Mot Evident Is needed here
public:
AMSECIdGeom(){};
friend class AMSECIdSoft;
};
class AMSECIdSoft{
protected:
int _dead;
int16 _sl;//0->
int16 _pmtno;// 0->
int16 _channel;//software pixel number(0->)
int16 _channelh;// hardware pixel number(0->)
int16 _haddr;
int16 _crate;
void _channelh2s(); // hardware->software pixel_number conversion
void _channels2h(); // software->hardware .............
 static integer _GetGeo[ecalconst::ECRT][ecalconst::ECPMSL][2];   // slayer,pmtno
 static integer _GetHard[ecalconst::ECSLMX][ecalconst::ECPMSMX][2];     // crate, hchan
 static int16 _GetPix[4][2];//hw/sf pixel correspondence
public:
uinteger makeshortid()const {return (_sl+1)*1000+(_pmtno+1)*10+_channel+1;}
bool dead(){return _dead==1;}

bool CHisBad(){return ECPMPeds::pmpeds[getslay()][getpmtno()].HCHisBad(getchannel())
                 || ECcalib::ecpmcal[getslay()][getpmtno()].HCHisBad(getchannel());}
		 
bool HCHisBad(){return ECPMPeds::pmpeds[getslay()][getpmtno()].HCHisBad(getchannel())
                 || ECcalib::ecpmcal[getslay()][getpmtno()].HCHisBad(getchannel());}
		 
bool LCHisBad(){return ECPMPeds::pmpeds[getslay()][getpmtno()].LCHisBad(getchannel())
                 || ECcalib::ecpmcal[getslay()][getpmtno()].LCHisBad(getchannel());}
		 
int16 getcrate(){return _crate;}
int16 getchannel(){return _channel;}
int16 getslay(){return _sl;}
int16 getlayer(){return _sl*2+_channel/2;}
int16 getcell(){return _pmtno*2+_channel%2;}
int16 getpmtno(){return _pmtno;}
geant getped(int16u gain){return gain<2?ECPMPeds::pmpeds[getslay()][getpmtno()].ped(getchannel(),gain):0;}
geant getsig(int16u gain){return gain<2?ECPMPeds::pmpeds[getslay()][getpmtno()].sig(getchannel(),gain):0;}
integer getsta(int16u gain){return gain<2?ECPMPeds::pmpeds[getslay()][getpmtno()].sta(getchannel(),gain):0;}
geant getpedd(){return ECPMPeds::pmpeds[getslay()][getpmtno()].ped();}
geant getsigd(){return ECPMPeds::pmpeds[getslay()][getpmtno()].sig();}
geant gethi2lowr(){return ECcalib::ecpmcal[getslay()][getpmtno()].hi2lowr(getchannel());}
geant getan2dyr(){return ECcalib::ecpmcal[getslay()][getpmtno()].an2dyr();}
geant getadc2mev(){return ECcalib::ecpmcal[getslay()][getpmtno()].adc2mev();}
geant getgain(){return ECcalib::ecpmcal[getslay()][getpmtno()].pmscgain(getchannel())*
             ECcalib::ecpmcal[getslay()][getpmtno()].pmrgain();}
AMSECIdSoft():_dead(1){};
AMSECIdSoft(int16 crate, int16 haddr, int16 channelh);
AMSECIdSoft(int sl, int pmt, int chan, int dummy);
AMSECIdSoft(integer idsoft);
static integer ncrates(){return 2;}
static void inittable();
friend class AMSECIdGeom;
};


#endif
