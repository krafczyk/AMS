#include <extC.h>
#include <event.h>
extern "C" void btempcor_(float& factor) {
  const int NPER = 28;
  const int NORD = 3;
  const int NPAR = NPER*(NORD-1)+2;

  const float XVAL[NPAR] = {
      23.669, -7.1142,  22.913, -42.978,  76.479, -35.207
   ,  70.234, -37.024,  68.963, -18.535, -313.46,  76.966
   ,  314.05, -94.062, -328.91,  57.627, -29.804,  4.4471
   ,  71.312, -8.3056, -47.074,  8.0869,  59.074, -8.3309
   ,  19.560, -1.2509, -132.25,  10.193,  77.015, -5.2367
   , -174.81,  11.297,  158.74, -8.2404, -936.20,  48.581
   ,  76.291, -6.3100,  51.299, 0.33294, -1.6159, -.07606
   , 209.090, -9.9976, -24.021,  3.1084, -31.621,-0.33081
   , 111.541, -3.4566, -250.81,  9.1796,  577.35, -22.539
   , -19.371,  1.4504,  3.8852, 0.18976
  };

  const float TMAX[NPER] = {
    0.40, 0.55, 0.90, 1.30, 1.65, 1.70, 1.90, 2.40, 3.25, 3.45,
    3.65, 4.00, 4.60, 5.40, 5.80, 6.20, 6.60, 6.85, 7.00, 7.35,
    7.55, 7.65, 7.85, 8.00, 8.20, 8.80, 9.20, 9.50
  };

  const int ufirst=896836164;
  const int daysec=86400;

  const float dBdT=0.13e-2;
  const float TETH=18.5;

  static float coef[NPER][NORD+1]={0.};
  static int first=1;

// Default in absence of corrections
  factor = 1.;
  if(!MISCFFKEY.BTempCorrection)return;
// init coefficients
  if (first) {
    first=0;

    int i,k=0;
    for (i=0; i<NPER; i++) {
      for (int j=0; j<=NORD; j++) {
        if (!i || j>1) coef[i][j]=XVAL[k++];
      }
    }

    for (i=0; i<NPER-1; i++) {
      float sum1,sum2;
      int j;

      sum1=0.;
      sum2=0.;
      for (j=1; j<=NORD; j++) {
        sum1+=(float)j*coef[i][j]*pow(TMAX[i],j-1);
        if (j>1) sum2+=(float)j*coef[i+1][j]*pow(TMAX[i],j-1);
      }
      coef[i+1][1]=sum1-sum2;

      sum1=0.;
      sum2=0.;
      for (j=0; j<=NORD; j++) {
         sum1+=(float)coef[i][j]*pow(TMAX[i],j);
         if (j>0) sum2+=coef[i+1][j]*pow(TMAX[i],j);
      }
      coef[i+1][0]=sum1-sum2;

    }

  }

// evaluate temperature
  time_t utime = AMSEvent::gethead()->gettime();
  float t=(float)(utime-ufirst)/daysec;

  if (t<0.) t=1.e-3;
  if (t>TMAX[NPER-1]) return; // Not valid for time > Shuttle Flight Period

  int iper=0;
  while (TMAX[iper]<t) {iper++;}

  float temp=0.;
  for (int j=0; j<=NORD; j++) temp+=coef[iper][j]*pow(t,j);

  factor = 1.-dBdT*(temp-TETH);

//  static int oldevent = -1;
//  if (AMSEvent::gethead()->getEvent() != oldevent) {
//    oldevent = AMSEvent::gethead()->getEvent();
//    cout << "Event= " << oldevent << " Time= " << utime;
//    cout << " BtempCor= " << factor << endl;
//  }

}
