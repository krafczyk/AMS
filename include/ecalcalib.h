#include <typedefs.h>
#include <ecaldbc.h>
//  
// v1.0 E.Choumilov 08.12.2000
//
// glob. conts for calibration :
//
const integer ECCLBMX=12;// max. long. bins for uniformity study(should be even)
//const integer ECLBMID=3;// Area (+-bins) from fiber center, used for PM RelGain calibr.
const integer ECLBMID=2;// Area (+-bins) from fiber center, used for PM RelGain calibr.
const integer ECCHBMX=50;// max. bins in hg-channel for h2lr study
const integer ECCADCR=100;//max. HGainADC range to use .............. 
//
// class to manipulate with REUN-calib data:
//                  (REspons UNiformity)
class ECREUNcalib {
  private:
    static integer pxstat[ECPMSL][4];// PM-subcell status(-1/0/1...->unusable/ok/limited
    static number pxrgain[ECPMSL][4];// PM-subcell(pixel) gain(4*(relat.to averaged over PM)) 
    static number pmrgain[ECPMSL];   // PM gain(relative to reference PM) 
    static number pmlres[ECPMSL][ECCLBMX];//to store PM vs Longit.bin responce    
    static number pmlres2[ECPMSL][ECCLBMX];//to store PM vs Longit.bin responce err.    
    static number sbcres[ECPMSL][4];//to store SubCell responce    
    static number hi2lowr[ECPMSL][4];// h/l-gain ratios for each PM-subcell(pixel)
    static number an2dynr[ECPMSL];// Anode/Dynode ratios for each PM
    static number tevpml[ECPMSL][ECCLBMX];// tot.events/Lbin/pm_fired 
    static number tevsbf[ECPMSL][4];// tot.events/subcell_fired 
    static number tevsbc[ECPMSL][4];// tot.events/subcell_crossed 
    static number tevpmc[ECPMSL];// tot.events/pm_crossed 
    static number tevpmf[ECPMSL];// tot.events/pm_fired 
    static number tevpmm[ECPMSL];// tot.events/pm_crossed_at_2_central_long_bins 
    static number tevhlc[ECPMSL*4][ECCHBMX];// tot.events/cell/bin for Hb/Lg calibr 
    static number sbchlc[ECPMSL*4][ECCHBMX];// cell-resp. for Hg/Lg calibr 
    static number sbchlc2[ECPMSL*4][ECCHBMX];// cell-resp.**2 for Hgain/Lgain calibr 
    static number values[ECCLBMX];//working arrays for profile-fit 
    static number errors[ECCLBMX]; 
    static number coords[ECCLBMX];
    static integer nbins; 
    static number slslow;//averaged light att. slopes/%
    static number slfast;
    static number fastfr;
    static integer nfits;// counter for averaging
    static number et2momr;//aver. of Etot/Mom ratios
    static integer net2mom;// number of Etot/Mom measurements
  public:
    static void init();
    static void fill_1(integer,integer,integer,integer, number );
    static void fill_2(integer,integer,integer,number a[2]);
    static void mfun(int &np, number grad[],number &f,number x[],int &flg,int &dum);
    static void mfit();
    static void select();
    static void selecte();
    static void mfite();
}; 
