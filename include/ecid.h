//  $Id: ecid.h,v 1.5 2002/09/27 15:17:43 choutko Exp $
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
int16 _sl;
int16 _pmtno;
int16 _channel;
int16 _channelh;
int16 _haddr;
int16 _crate;
void _channelh2s(); // convert channel from readout to id format
void _channels2h(); // and vs
 static integer _GetGeo[ecalconst::ECRT][ecalconst::ECPMSL][2];   // slayer,pmtno
 static integer _GetHard[ecalconst::ECSLMX][ecalconst::ECPMSMX][2];     // crate, hchan
public:
uinteger makeshortid()const {return (_sl+1)*1000+(_pmtno+1)*10+_channel+1;}
bool dead(){return _dead==1;}
int16 getcrate(){return _crate;}
int16 getchannel(){return _channel;}
int16 getslay(){return _sl;}
int16 getpmtno(){return _pmtno;}
geant getped(int16u gain){return gain<2?ECPMPeds::pmpeds[getslay()][getpmtno()].ped(getchannel(),gain):0;}
geant getsig(int16u gain){return gain<2?ECPMPeds::pmpeds[getslay()][getpmtno()].sig(getchannel(),gain):0;}
geant getpedd(){return ECPMPeds::pmpeds[getslay()][getpmtno()].ped();}
geant getsigd(){return ECPMPeds::pmpeds[getslay()][getpmtno()].sig();}
geant getan2dyr(){return ECcalib::ecpmcal[getslay()][getpmtno()].an2dyr();}
geant getadc2mev(){return ECcalib::ecpmcal[getslay()][getpmtno()].adc2mev();}
AMSECIdSoft():_dead(1){};
AMSECIdSoft(int16 crate, int16 haddr, int16 channelh);
AMSECIdSoft(int sl, int pmt, int chan, int dummy);
AMSECIdSoft(integer idsoft);
static integer ncrates(){return 2;}
static void inittable();
friend class AMSECIdGeom;
};


#endif
