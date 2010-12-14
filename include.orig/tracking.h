#ifndef AMS_MYTRACK
#define AMS_MYTRACK

#include <TString.h>
#include <root_RVS.h>

//!  TrHitA class
/*! 
  Utility class, used by the TrTrackA class
   
  \author juan.alcaraz@cern.ch
  
*/

//#############################################################################
class TrHitA {
  protected:
      void* _Link;
      TString _Type;
  public:
      double Coo[3]; ///< 3D hit coordinate (cm)
      double ECoo[3]; ///< 3D hit uncertainty (cm)
      double Bfield[3]; ///< 3D magnetic field (kG)

      void* GetLink(){return _Link;}; ///< Address to mother object (pointer to TrRecHitR, NULL, ...)
      const char* GetType(){return _Type.Data();}; ///< Type of mother object ("TrRecHitR", "", ...)

      /// Constructor
      TrHitA(float* coo, float* ecoo, float* bfield);
      /// Constructor
      TrHitA(double* coo, double* ecoo, double* bfield);
      /// Constructor
      TrHitA(TrRecHitR* phit);
      /// Default constructor
      TrHitA(){};
      virtual ~TrHitA(){};

      ClassDef(TrHitA,1) //TrHitA
};

//!  TrTrackA class
/*! 
  Utility class, to define and fit your own tracks directly on the ROOT 
  file, with your own selected hits and optionally without magnetic field
  (straight line fit). No multiple scattering is included for the moment.
  Example of usage:

  \include my_tracks.C
   
  \author juan.alcaraz@cern.ch
  
*/

//#############################################################################
class TrTrackA {
  protected:
      vector<TrHitA> _Hit;

      bool _PathIntExist;
      vector<double> _PathLength;
      vector<double> _PathIntegral_x[3];
      vector<double> _PathIntegral_u[3];
      void SetPathInt();

  public:
      double Rigidity;      ///< Rigidity (GV)
      double Chi2;          ///< Chi2 in 3D from fit
      int Ndof;             ///< ndof in 3D from fit
      double Theta;         ///< Theta from fit (rad)
      double Phi;           ///< Phi from fit (rad)
      double U[3];          ///< Unit direction vector
      double P0[3];         ///< Reference point from fit (cm)

      /// Number of hits
      int NHits(){return _Hit.size();}; 
      /// Get hit number l
      TrHitA& Hit(int l){return _Hit[l];}; 

      /// Get prediction (x,y,z,theta,phi,dx,dy) at hit index
      double* Prediction(int index);
      /// Get prediction from straight line (x,y,z,theta,phi,dx,dy) at hit index
      double* PredictionStraightLine(int index);
      /// Get prediction (x,y,z,theta,phi,dx,dy) at hit given by pointer
      double* Prediction(TrHitA* phit);
      /// Get prediction from straight line (x,y,z,theta,phi,dx,dy) at hit given by pointer
      double* PredictionStraightLine(TrHitA* phit);
      /// Get prediction (x,y,z,theta,phi,dx,dy) at hit given by pointer
      double* Prediction(TrRecHitR* phit);
      /// Get prediction from straight line (x,y,z,theta,phi,dx,dy) at hit given by pointer
      double* PredictionStraightLine(TrRecHitR* phit);
      /// Get prediction (x,y,z,theta,phi,dx,dy) at a user defined point
      double* Prediction(double* coo, double* ecoo, double* bfield);
      /// Get prediction from straight line (x,y,z,theta,phi,dx,dy) at a user defined point
      double* PredictionStraightLine(double* coo, double* ecoo, double* bfield);

      /// Perform simple fit inside magnetic field
      int SimpleFit();
      /// Perform istraight line fit
      int StraightLineFit();
      /// Perform fit inside magnetic field (same as SimpleFit)
      int Fit(){return SimpleFit();};

      /// Remove al hits
      void Clear();

      /// Add a hit from pointer
      TrHitA* add_hit(TrRecHitR* phit);
      /// Add a hit at 3D position coo(cm), with 3D error ecoo(cm) and 3D-magnetic field bfield (kG)
      TrHitA* add_hit(float* coo, float* ecoo, float* bfield);
      /// Add a hit at 3D position coo(cm), with 3D error ecoo(cm) and 3D-magnetic field bfield (kG)
      TrHitA* add_hit(double* coo, double* ecoo, double* bfield);
      /// Clear hits and use those from track at ptr
      void use_hits_from(TrTrackR* ptr);
      /// Remove hit at index
      void del_hit(int index);
      /// Remove hit having phit as pointer to "TrHitA"
      void del_hit(TrHitA* phit);

      /// Default constructor
      TrTrackA() : _PathIntExist(false) {};
      /// Constructor
      TrTrackA(TrTrackR* ptr);
      virtual ~TrTrackA(){Clear();};

      ClassDef(TrTrackA,1) //TrTrackA
};

#endif
