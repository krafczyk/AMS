//  $Id$
#ifndef __TrMCClusterR__
#define __TrMCClusterR__

//////////////////////////////////////////////////////////////////////////
///
///\class TrMCClusterR
///\brief A class for the rapresentation of the MC stepping in Silicon active material. 
///\ingroup tksim
///
///$Date$
///
///$Revision$
///
//////////////////////////////////////////////////////////////////////////

#include "typedefs.h"
#include "point.h"
#include "TrElem.h"

#include "TObject.h"

class TrSimCluster;

class TrMCClusterR : public TrElem {

 public:

  //! Sensor code [abs(TkId) + 1000*iSide + 10000*(iSensor + 1)]
  int         _idsoft; 
  //! Geant3 PID of the incident particle (negative for secondaries)
  short int   _itra;
  //! Geant4 particle track id
  int         _gtrkid;
  //! Step size in silicon (cm)
  Double32_t  _step;
  //! Middle point of the step (cm,cm,cm)
  AMSPoint    _xgl;
  //! Direction of the incident particle  
  AMSPoint    _dir;
  //! Momentum of the incident particle (GeV/c) 
  Double32_t  _mom;
  //! Energy released in silicon (GeV)
  Double32_t  _edep;

  //! Status word (first 5 bits used for storing |Z| = 0, ..., 30; 31 is for >30)  
  int         Status;

  //! Noise Marker (0: normal cluster, >0 noise generated)
  static int    _NoiseMarker; 
  //! Pointer to the TrSimClusters generated by TrSim2010 (0: n-side, 1: p-side)
  TrSimCluster* _simcl[2]; //!

 public:
 
  /** @name Basic functions */
  //! Std constructor build a dummy cluster
  TrMCClusterR() { Init(); }
  //! Constructor for a digitized hit
  TrMCClusterR(int idsoft, float step, AMSPoint xgl, AMSPoint dir, float mom, float edep, int itra, int gtrkid, int status = 0);
  //! Constructor for daq
  TrMCClusterR(AMSPoint xgl, integer itra, geant edep=0);
  //! Copy constructor
  TrMCClusterR(const TrMCClusterR& that) { Copy(that); }
  //! Assignment operator
  TrMCClusterR& operator=(const TrMCClusterR& that);
  //! Destructor
  virtual ~TrMCClusterR() { Clear(); }
  //! Initializer
  void Init();
  //! Clear
  void Clear();
  //! Copy
  void Copy(const TrMCClusterR& that);
  TrMCClusterR& operator+=(const TrMCClusterR& orig);
  const TrMCClusterR operator+(const TrMCClusterR &other) const {
    TrMCClusterR result = *this;     
    result += other;            
    return result;              
  }
  /**@}*/


  /** @name Basic accessors */
  //! Get the TkId of the ladder  
  int      GetTkId();
  //! Get the sensor number 
  int      GetSensor() { return _idsoft/10000 - 1; }
  //! Get the GEANT3 particle ID 
  int      GetPart() { return abs(_itra); }
 //! Get the GEANT4 track ID
  int      GetGtrkID(){ return _gtrkid; }
  //! Is the particle a primary
  bool     IsPrimary() { return (_itra>0); }       
  //! Get energy released in Silicon (GeV)
  float    Sum() { return _edep; }
  //! Get energy released in Silicon (GeV)
  float    GetEdep() { return _edep; }
  //! Get step (cm) 
  float    GetStep() { return _step; }
  //! Get middle point of the step (cm) 
  AMSPoint GetXgl() { return _xgl; }
  //! Get direction 
  AMSDir   GetDir() { return AMSDir(_dir[0],_dir[1],_dir[2]); } 
  //! Get step starting point
  AMSPoint GetStartPoint() { return GetXgl() - GetDir()*GetStep()/2; } 
  //! Get step ending point
  AMSPoint GetEndPoint() { return GetXgl() + GetDir()*GetStep()/2; }  
  //! Get momentum value (GeV/c)
  float    GetMomentum() { return _mom; }
  //! Get momentum vector (GeV/c)
  AMSPoint GetMom() { return GetDir()*GetMomentum(); }
  /**@}*/



  /** @name Local coordinates */
  /**@{*/
  //! Get step middle point in sensor local coordinates (cm,cm,cm) 
  AMSPoint GetSensCoo()      { AMSPoint point; AMSDir dir; GlobalToLocal(GetXgl(),GetDir(),point,dir); return point; }
  //! Get sensor step oriented direction in sensor local coordinates
  AMSDir   GetSensDir()      { AMSPoint point; AMSDir dir; GlobalToLocal(GetXgl(),GetDir(),point,dir); return dir; }
  //! Get projected angle on plane XZ (rad)
  float    GetAngleXZ()      { return atan(GetSensDir().x()/GetSensDir().z()); } 
  //! Get projected angle on plane YZ (rad)
  float    GetAngleYZ()      { return atan(GetSensDir().y()/GetSensDir().z()); }
  //! Get step projection on XZ plane (cm)
  float    GetStepXZ()       { AMSDir dir = GetSensDir(); return GetStep()*sqrt(dir[0]*dir[0] + dir[2]*dir[2]); } 
  //! Get step projection on YZ plane (cm)
  float    GetStepYZ()       { AMSDir dir = GetSensDir(); return GetStep()*sqrt(dir[1]*dir[1] + dir[2]*dir[2]); } 
  //! Get sensor step starting point in sensor local coordinates (cm,cm,cm) 
  AMSPoint GetSensStartCoo() { return GetSensCoo() - GetSensDir()*GetStep()/2; }
  //! Get sensor step ending point in sensor local coordinates (cm,cm,cm)
  AMSPoint GetSensEndCoo()   { return GetSensCoo() + GetSensDir()*GetStep()/2; }
  //! Get the magnetic field associated to the step middle in local coordinates (3 diff. methods) (kG) 
  AMSPoint GetSensMagField(int algo = 2);
  //! Get the Hall-effect potential direction in local coordinate (ExB) 
  AMSDir   GetSensHallDir(double& B);
  //! Get local coordinate at the center of the silicon (Z=0), used for resolution evaluation 
  AMSPoint GetSensMiddleCoo();
  //! Global-to-local coordinate conversion interface
  bool GlobalToLocal(AMSPoint GlobalCoo, AMSDir GlobalDir, AMSPoint& SensorCoo, AMSDir& SensorDir);
  /**@}*/


  /** @name Cluster simulation */
  //! Creation of the simulated clusters objects (TrSimCluster) of TrSim2010
  void GenSimClusters();
  //! Return a pointer to the simulated clusters (if TrSim2010 selected)
  TrSimCluster* GetSimCluster(int side) {
    if (side!=0 && side!=1) return 0;
    return _simcl[side];
  }
  //! Checks the cluster type against the IsNoise flag
  int   IsNoise() { return _itra == _NoiseMarker; }

  //! checkstatus
  unsigned int checkstatus(int c) const { return Status&c; }
  //! getstatus
  unsigned int getstatus  (void)  const { return Status; }
  //! setstatus
  void setstatus  (unsigned int s) { Status |=  s; }
  //! clearstatus
  void clearstatus(unsigned int s) { Status &= ~s; }

  //! Generates the strip distribution (old model)
  void _shower();
  /**@}*/


 protected:

  //! Prepare output  
  std::string _PrepareOutput(int full);

 public:

  /** @name Printout */
  /// Print MC cluster basic information on a given stream
  std::ostream& putout(std::ostream &ostr = std::cout);
  /// ostream operator
  friend std::ostream &operator << 
    (std::ostream &ostr,  TrMCClusterR &cls) { 
    return cls.putout(ostr); 
  }
  /// Print cluster strip variables 
  void Print(int printopt =0);
  /// Return a string with some info (used for event display)
  const char* Info(int iRef);
  /**@}*/


  friend class TrDAQMC;

  /// ROOT definition
  ClassDef(TrMCClusterR,6);
};

#endif
