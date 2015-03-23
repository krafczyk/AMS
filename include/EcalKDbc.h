
#ifndef __ECALKDBC__
#define __ECALKDBC__ 1

//! ECAL TDV Class
/*!
 * Tools to interface with DataBase
 * Calibration Categories:
 * 		EcalAttPar: Attenuation parameters for each cell, static
 * 		EcalEquilPar: High gain equilization for each cell, dynamic for each day
 * 		EcalGainRatioPar: Gain ratio for each cell, static
 * \author: hachen@cern.ch
*/

#include "point.h"
//#include "timeid.h"
#include <map>

class AMSTimeID;

using namespace std;
namespace Ecal3DRecDB{

//! Status bit
	const UInt_t   AOVERFLOW = 16384*2*2*2*2*2*2*2*2;
	const UInt_t   ADCLowGainSatu = 3750;
	const UInt_t   ADCHighGainSatu = 3750;
	const UInt_t   MaxIteration = 10;
	const Double_t MinDeltaLkhd = 0.5;

//! Geometry info
	const Int_t nLayer = 18,
							nFibLayer = 90,
							nCell = 72,
							nSL = 9,
							nCol = 36,
							nSL_X = 4, // fiber along Y
							nSL_Y = 5; // fiber along X

//! Fiber Offset, boundary between fiber 242 and 243
	const Double_t F_Offset[nFibLayer] = { 
		0.000,  0.067, -0.002,  0.065, -0.004,
		0.064, -0.005,  0.062, -0.007,  0.060,

		0.019,  0.087,  0.019,  0.087,  0.019,
		0.087,  0.019,  0.087,  0.019,  0.087,

		-0.028,  0.048, -0.012,  0.064,  0.004,
		-0.054,  0.021, -0.038,  0.037, -0.022,

		0.022, -0.038,  0.038, -0.022,  0.054,
		-0.008,  0.068,  0.008,  0.084,  0.024,

		0.008,  0.071, -0.002,  0.061, -0.012,
		0.051, -0.022,  0.041, -0.032,  0.031,

		0.026,  0.088,  0.014,  0.076,  0.002,
		0.066, -0.008,  0.054, -0.018,  0.042,

		-0.042,  0.024, -0.044,  0.022, -0.046,
		0.020, -0.048,  0.018, -0.050,  0.016,

		-0.005,  0.053, -0.025,  0.033, -0.045,
		0.011, -0.067, -0.009, -0.087, -0.029,

		0.000,  0.068, -0.005,  0.059, -0.013,
		0.049, -0.023,  0.041, -0.031,  0.033
	};

	const Double_t F_Zcoord[nFibLayer] = {
		-142.847+0.009, -143.031+0.007, -143.215+0.005, -143.399+0.003, -143.583+0.001,
		-143.767-0.001, -143.951-0.003, -144.135-0.005, -144.319-0.007, -144.503-0.009,
		-144.697+0.009, -144.881+0.007, -145.065+0.005, -145.249+0.003, -145.433+0.001,
		-145.617-0.001, -145.801-0.003, -145.985-0.005, -146.169-0.007, -146.353-0.009,
		-146.547+0.009, -146.731+0.007, -146.915+0.005, -147.099+0.003, -147.283+0.001,
		-147.467-0.001, -147.651-0.003, -147.835-0.005, -148.019-0.007, -148.203-0.009,
		-148.397+0.009, -148.581+0.007, -148.765+0.005, -148.949+0.003, -149.133+0.001,
		-149.317-0.001, -149.501-0.003, -149.685-0.005, -149.869-0.007, -150.053-0.009,
		-150.247+0.009, -150.431+0.007, -150.615+0.005, -150.799+0.003, -150.983+0.001,
		-151.167-0.001, -151.351-0.003, -151.535-0.005, -151.719-0.007, -151.903-0.009,
		-152.097+0.009, -152.281+0.007, -152.465+0.005, -152.649+0.003, -152.833+0.001,
		-153.017-0.001, -153.201-0.003, -153.385-0.005, -153.569-0.007, -153.753-0.009,
		-153.947+0.009, -154.131+0.007, -154.315+0.005, -154.499+0.003, -154.683+0.001,
		-154.867-0.001, -155.051-0.003, -155.235-0.005, -155.419-0.007, -155.603-0.009,
		-155.797+0.009, -155.981+0.007, -156.165+0.005, -156.349+0.003, -156.533+0.001,
		-156.717-0.001, -156.901-0.003, -157.085-0.005, -157.269-0.007, -157.453-0.009,
		-157.647+0.009, -157.831+0.007, -158.015+0.005, -158.199+0.003, -158.383+0.001,
		-158.567-0.001, -158.751-0.003, -158.935-0.005, -159.119-0.007, -159.303-0.009
	};

	const Double_t F_Rotation[nLayer] = {
		-0.0005, -0.0005, -0.0008, -0.0008, -0.0001, -0.0001, -0.0003, -0.0003, 0.0010, 0.0010,
		0.0000,  0.0000,  0.0000,  0.0000, -0.0003, -0.0003,  0.0016,  0.0016
	};

	const Double_t EcalZ[nLayer]  = {
		-143.215-0.005, -144.135+0.005, -145.065-0.005, -145.985+0.005, -146.915-0.005, -147.835+0.005,
		-148.765-0.005, -149.685+0.005, -150.615-0.005, -151.535+0.005, -152.465-0.005, -153.385+0.005,
		-154.315-0.005, -155.235+0.005, -156.165-0.005, -157.085+0.005, -158.015-0.005, -158.935+0.005
	};

//! Geometry
	const Double_t Cell_Size = 0.9; // cm
	const Double_t F_Diameter = 0.135; // cm
	const Double_t Fiber_Pitch = 0.135; // cm
	const Double_t L_Thickness = 0.925; // cm
	const Double_t Fiber_Radius = 0.050; // cm

//! PMT efficiency model
	const Double_t L_Eff_PW = 0.3; // Plateau width
	const Double_t L_Eff_MinEff = 0.60; // Minimum efficiency
	const Double_t L_Eff_Par = (1.0-L_Eff_MinEff)/(0.5-L_Eff_PW); // Slope
	const Double_t L_Eff[5] = { 1.0, 1.0, 1.0, 1.0, 1.0 };

//! Shower clustering parameters
	const Double_t Cutoff_MIP = 0.005; // GeV
	const Double_t Cutoff_SEED = 0.020; // GeV
	const Int_t    Cutoff_NNB = 4; 
	const Int_t    Cutoff_NPL = 4;

//! Data Type
	const Short_t _ToyMC_DATA = 3;
	const Short_t _MC_DATA = 2;
	const Short_t _TB_DATA = 1;
	const Short_t _ISS_DATA = 0;

//! Constant used for calibration
	const float ADC2MEV = 0.4704;  //ADC to Mev
	const float CellWidth = 0.9;   //Cell Width
	const float ADC2MeV_HC = 0.956324; // Calibration constant from Hai

//! Constant for attenuation and Temperature effect, Will be removed by Hai
	const float ATTRATIO= 0.2183;
	const float ATTXS   = 10.43;
	const float ATTXL   = 237.5;
	const float TEMPEFFA = -0.0025;
	const float TEMPEFFB= 0;

//! Fiber Sampling Fraction 
	const Double_t fNorm = 0.005495*0.790; // sampling fraction and integration over fiber diameter
	const Int_t kNS = 3,	// maximum number of showers
				kNL = 18, 		// number ECAL layers
				kNC = 80; 		// +-4 cells to recover lateral leakage
	const Int_t kNpar=8; // number of shower parameters
}

//! Template class for various categories
template <typename T1>
class EcalTDVTool{
public:
    int   TDVSize;
    int   TDVParN;
    T1*   TDVBlock;
    char *TDVName;
    bool  Isload;
    int   BRun;//begin run id
    int   ERun;//end   run id
    //----
public:
    EcalTDVTool():TDVSize(0),TDVBlock(0),TDVName(0),Isload(0),BRun(0),ERun(0),TDVParN(0){};
    virtual void LoadTDVPar(){};
    virtual void GetTDVPar(){};
    virtual int  ReadTDV(int rtime,int real=1);
    virtual int  WriteTDV(int brun,int erun,int real=1);
    virtual int  WriteTDV(int real=1){return WriteTDV(BRun,ERun,real);}
    virtual void PrintTDV(){};
public:
		static const int ECNLayer=18;
		static const int ECNCell=72;
};

//! EcalAttPar
/*!
 * Attenuation Parameters
*/

class EcalAttPar: public EcalTDVTool<float>{
public:
    static const int nattpar=3;
		//! three ecalattpar for each cell, slow & fast length, fast fraction
    std::map<int, float>attpar[nattpar];
public:
    EcalAttPar();
    EcalAttPar(float *arr,int brun,int erun);
    static EcalAttPar *Head;
#ifdef __ROOTSHAREDLIBRARY__
#pragma omp threadprivate (Head)
#endif
    static EcalAttPar *GetHead();
    static void HeadLoadTDVPar(){GetHead()->LoadTDVPar();}
    void LoadTDVPar();
		//! read data from fstream file
    int  LoadFromFile(char *file);
    void PrintTDV();
};

//! EcalEquilPar
/*!
 * PMT Equilization each Cell
*/

class EcalEquilPar: public EcalTDVTool<float>{   
public:
    static const int nequil=1;
		//! one equilization factor for each cell each day, reference to test beam
    std::map<int, float>equil;

public:
    EcalEquilPar();
    EcalEquilPar(float *arr,int brun,int erun);
    static EcalEquilPar *Head;
#ifdef __ROOTSHAREDLIBRARY__
#pragma omp threadprivate (Head) 
#endif  
    static EcalEquilPar *GetHead();
    static void HeadLoadTDVPar(){GetHead()->LoadTDVPar();}
    void LoadTDVPar();
		//! read data from fstream file
    int  LoadFromFile(char *file);
    void PrintTDV();
};

//! EcalGainRatioPar
/*!
 * PMT High/Low Ratio for each cell
*/

class EcalGainRatioPar: public EcalTDVTool<float>{   
public:
    static const int ngainpar=1; 
		//! one gain ratio factor for each cell around 33.0, static correction
    std::map<int, float>gain;

public:
    EcalGainRatioPar();
    EcalGainRatioPar(float *arr,int brun,int erun);
    static EcalGainRatioPar *Head;
#ifdef __ROOTSHAREDLIBRARY__
#pragma omp threadprivate (Head) 
#endif  
    static EcalGainRatioPar *GetHead();
    static void HeadLoadTDVPar(){GetHead()->LoadTDVPar();}
    void LoadTDVPar();
    int  LoadFromFile(char *file);
    void PrintTDV();
};

/*
/////////////////////////////////////
// EcalAlignPar
// Alignment Parameters with respect to Tracker
/////////////////////////////////////

class EcalAlignPar: public EcalTDVTool<float>{
public:
    static const int nalignpar=4;
    //std::map<int, float>alignpar[nalignpar]; 
    static float alignpar[nalignpar];// Alignment Parameters on fours sides of Ecal

public:
    EcalAlignPar();
    EcalAlignPar(float *arr,int brun,int erun);//load 
    static EcalAlignPar *Head;
#ifdef __ROOTSHAREDLIBRARY__
#pragma omp threadprivate (Head)
#endif
    static EcalAlignPar *GetHead();
    static void HeadLoadTDVPar(){GetHead()->LoadTDVPar();}
    void LoadTDVPar();//copy TDV to class 
    int  LoadFromFile(char *file);//read data from file->Block data
    void PrintTDV();
};
*/

//! EcalTDVManager
/*!
 * Class to manage all the TDV
*/

class EcalTDVManager {
public:

    map<string,AMSTimeID*> tdvmap; 
    int isreal;//real par or mc
    unsigned int ntime; //Now Time
    
public:
    EcalTDVManager(int real);
    ~EcalTDVManager(){};
    static EcalTDVManager *Head;
    static EcalTDVManager *GetHead(int real);
    static void ClearHead(){if(Head){Head->Clear();delete Head;Head=0;}}
    
public:
    int    Validate(unsigned int time);
    void   Clear();  
};

#endif

