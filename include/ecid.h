//  $Id: ecid.h,v 1.1 2002/09/24 07:15:51 choutko Exp $
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
uinteger makeshortid(){return (_sl+1)*1000+(_pmtno+1)*10+_channel+1;}
bool dead(){return _dead==1;}
int16 getcrate(){return _crate;}
int16 getchannel(){return _channel;}
int16 getslay(){return _sl;}
AMSECIdSoft():_dead(1){};
AMSECIdSoft(int16 crate, int16 haddr, int16 channelh);
AMSECIdSoft(integer idsoft);
static integer ncrates(){return 2;}
static void inittable();
friend class AMSECIdGeom;
};


#endif
