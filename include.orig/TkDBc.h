// $Id: TkDBc.h,v 1.32 2012/05/05 02:18:44 pzuccon Exp $

#ifndef __TkDBC__
#define __TkDBC__

#include "typedefs.h"

#include <iostream>
#include <iterator>
#include <fstream>
#include <string>
#include <map>

#include "TkLadder.h"
#include "TkPlane.h"

#include "TFile.h"
#include "tkpatt.h"

/*!\class TkDBc
\brief The new AMS Tracker database class
\ingroup tkdbc

 The TkDBc Class is a singleton, that is only one instance
 of this class is admitted. The TkDBc single instance is created
 via the static method TkDBc::CreateTkDBc()
 and is accessibile via the static pointer TkDBc::Head.\n
 The access of to the ladder objects is done
 via the find functions that implement a fast binary search.

 author P.Zuccon -- INFN Perugia 20/11/2007

\date  2008/01/17 PZ  First version
\date  2008/01/21 SH  File name, class name changed to avoid conflicts
                      Some bugs are fixed
\date  2008/01/23 SH  Some comments are added
\date  2008/02/02 SH  New method GetTkId(x,y,z) added
\date  2008/02/26 AO  New method GetEntry() for TkId map added

*/

namespace trconst{
  //! number of planes
  const integer maxplanes=6;
  //! number of layers FIX ME remove this
  const integer maxlay=9;
  //! maximal number of ladders
  const integer maxlad=15;
  //! number of ladders for each layer and side (0 X neg, 1 X pos)
  const integer nlad[2][maxlay]={{15,12,11,10,10,11,12,15,8},{15,12,11,10,10,11,12,15,8}};    
  //! number of crates
  const int ncrt=8;
  //! number of TDRs per crate
  const int ntdr=24;

  //!FIXME  for compatibility
 const integer TrkTypes=3;//for charge algorithm(
  //!FIXME  for compatibility
 const integer TrkPdfBins=100;//max bins in Elos PDF's



}
using namespace trconst;

class TkDBc : public TObject{
  
  private:
  //! the setup index 
  static int _setup;
  static char _setupname[4][30];
  //! number of active planes in the setup
  int nplanes;
  //! number of silicon layers in the setup
  int nlays;
  //! renumbering of the tracker Layer J-style
  int _J_to_lay[10];  
  int _lay_to_J[10];
public:

  //! static pointer to the TkDBc singleton
  /*! The TkDBc data and methods are always accessibile via TkDBc::Head
    Example: TkDBc::Head->_nplanes  or TkDBc::FindTkId(-212);
  */
  static TkDBc* Head;

  // ------- PLANE 1N Support--------------------   
  number P1NSupportCoo[3];
  number P1NSupportRadius;
  number P1NSupportThickness;
  number P1NSupportSkinThickness;
  number P1NSCRadius;
  number P1NSCThickness;

  // ------- PLANES -----------------------------   

  //! nominal X coo of the planes
  number   _xpos[maxplanes];
  //! nominal Y coo of the planes
  number   _ypos[maxplanes];
  //! nominal Z coo of the planes
  number   _zpos[maxplanes];
  //! Number of avalilable slot per planes (top view, bottom view)
  int16     _nslot[maxplanes][2];


  // Plane support thickness in cm
  number _sup_hc_w[maxplanes];

  // plane support skin thickness
  number _sup_hc_skin_w[maxplanes];
  // Plane6 skins of plane(0) and cover (1)
  number  Plane6_skin_w[2];
 
  
  // Plane support radius in cm
  number _sup_hc_r[maxplanes];

  //! Plane envelop (used for GEANT) radius in cm
  double _plane_d1[maxplanes];
  
  //! Plane envelop (used for GEANT) half thickness in cm
  double _plane_d2[maxplanes];
  
  //! To account for the envelop assimmetry of external planes
  double _dz[maxplanes];

  double  Plane6Size[3];

  double  Plane6EnvelopSize[3];

  // ------- LAYERS -----------------------------   
  //! Map correlating  layer to supporting plane
  integer _plane_layer[maxlay];

  //! Z distance of the silicon surface from the middle of the plane;
  number _layer_deltaZ[maxlay];
  //! Alignment correction to Z distance of the silicon surface from the middle of the plane;
  number _layer_deltaZA[maxlay];

  //! number of active ladders for side/layer
  integer  _nlad[2][maxlay];



  //--------------  LADDERS --------------------------------
  //! ladder pitch on Y 
  number _ladder_Ypitch;

  //! X offset of the ladders for the inner planes
  number _ladder_offsetX_inner[maxlad];

  //! X offset of the ladders for the outer planes
  number _ladder_offsetX_outer[maxlad];

  //! Ladder X separation
  number _ladder_Xseparation;

  //! map of the ladder edge distance (mm) from Y axis
  number _LadDeltaX[2][maxlay][maxlad];

  //! Map of the ladder lenght in unit of Silicon sensors
  short int _nsen[2][maxlay][maxlad];

//   //! Map of the ladder power supply in the crate is pwgp*100+pwpos
//   short int _pgid[2][maxlay][maxlad];

  //! Map of the Ladder names
  char _LadName[2][maxlay][maxlad][9];

  //! It returns the position on the plane (Y coordinate) of the first p-side readout channel 
  number GetSlotY(int layer, int slot,int side);
  
  //! It returns the position on the plane (X coordinate) of the first n-side read out channel 
  number GetSlotX(int layer, int slot,int side);

  //! It return the number of slots for the layer [0-7] and side [0 PORT: 1 XXX]
  int GetNslots(int layer, int side){ return nlad[side][layer];}

  //! returns the number of silicon layers
  integer nlay() const {return nlays;}
  
  //! It returns the default (hard-coded) sensor alignment parameter 
  number GetSensAlignX(int tkid, int sens);

  //! It returns the default (hard-coded) sensor alignment parameter 
  number GetSensAlignY(int tkid, int sens);

  //! layer 9 Y pos from mtrology
  float   _lay9Ypos[2][8];
  //! layer 9 Z rot from mtrology
  float   _lay9Zrot[2][8];


  // -------- CABLING ------------------------------
  //! Map of the ladders cabling the number is octatn*100+TDR
  short int _octid[2][maxlay][maxlad];


  /** \brief Map of the crate number corresponting to the octants
      
      Octant are numbered the following way 
      
      NEGATIVE Y corresponds to RAM. 
      POSITIVE Y corresponds to WAKE.
      
      V    Oct X  Y  Z   Crate    JINJ                JMDC
      
                            (slave num)           A    B   P   S  
      
      0 or 1P  + WAKE +     T0       3           166  167 168 169     
      1 or 2M  - WAKE +     T1       9           170  171 172 173
      2 or 3P  + RAM  +     T3       1           178  179 180 181
      3 or 4M  - RAM  +     T2       0           174  175 176 177
      4 or 5P  + WAKE -     T4      16           182  183 184 185
      5 or 6M  - WAKE -     T5      17           186  187 188 189
      6 or 7P  + RAM  -     T7      23           194  195 196 197
      7 or 8M  - RAM  -     T6      22           190  191 192 193
  */
  short int _octant_crate[8];
  //! see _octant_crate
  short int _octant_JinJ[8];

  short int _octant_JMDC[8][4];


  

public:
  
  //! It returns true if a slot contains a ladder
  bool filled_slot(int side,int layer, int slot){return _nsen[side][layer][slot]>0;}
 
  //! Return a string inicating the  postion of the octant
  char * GetOctName(int oct);
  
  //! Return a string inicating the  geographic postion of the crate 
  char * GetCratePos(int crate);

  int GetOctFromCrate(int crate)
  { int oct=-1; for (int ii=0;ii<8;ii++) if(_octant_crate[ii]==crate) oct=ii; return (oct+1); }
 
  int GetOctFromJinJ(int JinJ)
  { int oct=-1; for (int ii=0;ii<8;ii++) if(_octant_JinJ[ii]==JinJ) oct=ii; return (oct+1); }

  int GetCrateFromJinJ(int JinJ)
  { int oct=GetOctFromJinJ(JinJ); oct--; return _octant_crate[oct]; }

  //! Dimension of the ladder electronics
  number  _zelec[3];

  //! Return the internal layer number from the J-scheme one
  int GetLayerFromJ(int aaJ) const {return _J_to_lay[aaJ];}

  //! Return the J-layer number from the internal one
  int GetJFromLayer(int aa) const {return _lay_to_J[aa];}



 //  //Silicon sensors
  //! Sensor active area (0 K  1 S)
  number   _ssize_active[2];
  //! Sensor full  area (0 K  1 S)
  number   _ssize_inactive[2];
  //! Sensor thikness
  number   _silicon_z;

  int _NReadStripK7;
  int _NReadStripK5;
  int _NReadoutChanS;
  int _NReadoutChanK;
  number _SensorPitchK;
  number _PitchS;
  number _PitchK5;
  number _PitchK7;
  number _ImplantPitchK;
//     integer  _nstripssen[maxlay][2]; //number of strips
//     integer  _nstripssenR[maxlay][2];
//     integer  _nstripsdrp[maxlay][2];
  
//     number   _zelec[maxlay][3];
//     number   _c2c[maxlay];
//     number   _halfldist[maxlay];
//     number   _support_foam_w[maxlay];
//     number   _support_foam_tol[maxlay];
//     number   _support_hc_w[maxlay];
//     number   _support_hc_r[maxlay];
//     number   _support_hc_z[maxlay];
//     integer  _nladshuttle[maxlay][2];
//     integer  _boundladshuttle[maxlay][2];
//     number   _PlMarkerPos[maxlay][2][4][3];  // 1st wjb

private:
  //! Map for fast binary search based on TkAssemblyId
  map<int,TkLadder*> tkassemblymap;   //! it is rebuilt when loaded
  //! Map for fast binary search based on TkId 
  map<int,TkLadder*> tkidmap;
  //! Map for fast binary search based on HwId
  map<int,TkLadder*> hwidmap;         //! it is rebuilt when loaded
//   //! Map for fast binary search based on PgId
//   map<int,TkLadder*> pgidmap;         //! it is rebuilt when loaded
  //! Map for fast binary search based on LadName
  map<string,TkLadder*> lnamemap;     //! it is rebuilt when loaded
  //! Map for fast binary search based on JMDCNum
  map<int,TkLadder*> JMDCNumMap;      //! it is rebuilt when loaded

//! Map for special local alignement of external layers PG
  vector<TkLadder> PGlocal;
//! Map for special local alignement of external layers Madrid 
  vector<TkLadder> MDlocal;


  TkLadder* Findmap(map<int,TkLadder*> & tkmap, int key);

  int GetOctant(int side,int _layer,int _slot);

  //Pointer ro the planes;

  vector<TkPlane*>  planes2;

  //! Rebuild all the other maps from tkidmap
  void RebuildMap();


public:
  //! Default constructor for ROOT IO
  TkDBc();
  //! Constructor loaded from root file
  TkDBc(const char *filename) { Load(filename); }

  //! Static function used to create the TkDBc Singleton. If force_delete>0 it deletes and recreates  the single istance of the class
  static void CreateTkDBc(int force_delete=0);
  ~TkDBc();

  //! Do all the initialization stuff
  void init(int setup=3,const char* inputfilename=0, int pri=0);

  //! Returns  pointer to the TkDBc singleton. If no TkDBc object exists, 
  //!  a new one is created and initialized with the par passed (see init documentation)
  static TkDBc* GetHead(int setup=3,const char* inputfilename=0, int pri=0);

  //!  Write the content of the DB to an ascii file
  int  write(const char* filename);
  //!  Read the  content of the DB from an ascii file
  int  read(const char* filename, int pri=0);
  //!  Read Ladder alignement data (pos/rot posA/rotA) from a file Free Format  
  //!   aka  you can put any number of ladder  here 
  //! ( useful for local ext alignment)
  int readAlignmentLadFF(const char* filename, int pri);
  
  //!  Write Ladder alignement data (pos/rot posA/rotA) from a file Free Format  
  //!   aka  you can add any laddder you want in the order you prefer
  //! ( useful for local ext alignment)
  int writeAlignmentLadFF(int tkid, const char* filename, int overwrite=0);
  //!  Read the alignement data (posA/rotA) from a file
  int readAlignment(const char* filename, int pri=0);
  //!  Read the (Dis)alignement data (posT/rotT) from a file
  int readDisalignment(const char* filename, int pri=0);

  //!  Read the alignement data from a file with a format "tkid dx dy dz alpha beta gamma"
  int readAlignmentAngles(const char* filename, int pri=0);

  //!  Write the alignement data (posA/rotA) to a file
  int writeAlignment(const char* filename);
  //!  Write the (Dis)alignement data (posT/rotT) to a file
  int writeDisalignment(const char* filename);

  //!  Read the sensor alignement data from a file with a format "tkid sx[0-14] sy[0-14]"
  int readAlignmentSensor(const char* filename, int pri=0);
  //! Special function to load PG local external alignment from Files
  int LoadPGExtAlign(char *ff="PGExtLocalAlign.txt"){
    return LoadExtLocalAlign(ff,0);
  }
  //! Special function to load PG local external alignment from Files
  int LoadMDExtAlign(char *ff="MDExtLocalAlign.txt"){
    return LoadExtLocalAlign(ff,1);
  }  
  int LoadExtLocalAlign(char *fname, int type,int pri=0);
  
  
  //! Returns the number of active planes
  int GetNPlanes() const { return nplanes;} 
  //! Return the pointer to the iith (ii [1-6]) TkPlane object 
  TkPlane* GetPlane(int ii);
  //  TkPlane* GetPlane(int ii) {if (ii>0&&ii<=nplanes) return &(planes[ii-1]); else return 0;}

  //! TkId map number of elements
  int GetEntries() { return 192; }
  //! TkId map element by iterator index
  TkLadder* GetEntry(int ii);
  //! TkId for the ii entry in the TkId map (0-191)
  int Entry2TkId(int ii);
  //! Entry (0-191) of the TkId index in the map
  int TkId2Entry(int TkId);

  //! Returns the pointer to the ladder object with the required Assembly id. In case of failure returns a NULL pointer
  TkLadder* FindTkAssemblyId( int tkassemblyid){ return Findmap(tkassemblymap,tkassemblyid);}
  //! Returns the pointer to the ladder object with the required tkid. In case of failure returns a NULL pointer
  TkLadder* FindTkId( int tkid){ return Findmap(tkidmap,tkid);}
  //! Returns the pointer to the ladder object with the required HwId. In case of failure returns a NULL pointer
  TkLadder* FindHwId( int hwid){ return Findmap(hwidmap,hwid);}
//   //! Returns the pointer to the ladder object with the required PgId. In case of failure returns a NULL pointer
//   TkLadder* FindPgId(int pgid){ return Findmap(pgidmap,pgid); }
   //! Returns the pointer to the ladder object with the required ladder name. In case of failure returns a NULL pointer
  TkLadder* FindLadName(string& name);
  //! Returns the tkId corresponding to a given HwId
  int HwId2Tkid(int hwid){
    TkLadder* ll= FindHwId(hwid);
    if(ll) return ll->GetTkId();
    else return -1;
  }
  //! Get X-coordinate of layer
  double GetXlayer(int i) {
    return (1 <= i && i <= nlays)
      ? GetPlane(_plane_layer[i-1])->pos[0]: -9999.;} 
  
  //! Get Y-coordinate of layer
  double GetYlayer(int i) {
    return (1 <= i && i <= nlays)
      ? GetPlane(_plane_layer[i-1])->pos[1]: -9999.;} 

  //! Get Aligned Z-coordinate of layer J-scheme
  double GetZlayerAJ(int i) {
    int lay=GetLayerFromJ(i);
    return GetZlayerA(lay);
  }
  //! Get Z-coordinate of layer J-scheme
  double GetZlayerJ(int i) {
    int lay=GetLayerFromJ(i);
    return GetZlayer(lay);
  }
  //! Get Aligned Z-coordinate of layer OLD scheme
  double GetZlayerA(int i) {
    return (1 <= i && i <= nlays) 
      ? GetPlane(_plane_layer[i-1])->pos[2]+
        GetPlane(_plane_layer[i-1])->posA[2]+
        GetPlane(_plane_layer[i-1])->rot.GetEl(2, 2)*
        _layer_deltaZ[i-1] : -99999.;}
  //! Get Z-coordinate of layer OLD scheme
  double GetZlayer(int i) {
    return (1 <= i && i <= nlays) 
      ? GetPlane(_plane_layer[i-1])->pos[2]+
        GetPlane(_plane_layer[i-1])->rot.GetEl(2, 2)*
        _layer_deltaZ[i-1] : -99999.;}
  //! copy the rotation matrix into nrm array
  void GetLayerRot(int lay,number nrm[][3]);

  //! Kill TkLadder
  void KillTkId(int tkid) { tkidmap[tkid] = 0; }

  int NStripsDrp(int type){  return (type==0)?_NReadoutChanK: _NReadoutChanS;}


  //! Load from TDV, ver=2: TrackerAlignPM2, ver3: TrackerAlignPM3
  static int GetFromTDV(unsigned int time, int ver = 2);

  //! Update TDV, ver=2: TrackerAlignPM2, ver3: TrackerAlignPM3
  static int UpdateTDV(unsigned int begin, unsigned int end, int ver = 2);

  //!interface to GBTACH TDV database
  static float* linear;
  //!interface to GBTACH TDV database
  static int  GetLinearSize(){
//     if(_setup==3)
//       return ((192+5)*(1+TkObject::GetSize())*sizeof(float));
//     else
      return ((192*(1+TkLadder::GetSize())+6*(1+TkObject::GetSize()))*sizeof(float));
  }

  //!interface to GBTACH TDV database
  void Align2Lin();
  //!interface to GBTACH TDV database
  void Lin2Align();

  //! Setup the linear space for the DB
  static void CreateLinear(){
    linear= new float[GetLinearSize()/4];}

  //! Load TkDBc object from a ROOT file by file name
  static TkDBc *Load(const char *fname) {
    TFile ff(fname);
    if (ff.IsOpen()) return Load(&ff);
    return 0;
  }
  //! Load TkDBc object from a ROOT file by file pointer
  static TkDBc *Load(TFile *rfile);

  //! Returns the Setup Index
  int GetSetup(){return _setup;}
  //! Returns the Setup Index
  char * GetSetupName(){return _setupname[_setup];}

  static int nthreads;

  ClassDef(TkDBc, 9);
};

typedef map<int,TkLadder*>::const_iterator tkidIT;
typedef map<string,TkLadder*>::const_iterator lnameIT;

void SLin2Align();

#endif

