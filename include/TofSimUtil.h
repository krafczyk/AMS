//  $Id: TofSimUtil.h,v 1.3 2012/06/25 02:56:02 qyan Exp $

//Author Qi Yan 2012/Feb/09 23:14 qyan@cern.ch
#ifndef __AMSG4TOF__
#define __AMSG4TOF__

#include "gmat.h"
#include "gvolume.h"
#include "G4AssemblyVolume.hh"
#include "G4LogicalBorderSurface.hh"
#include "G4LogicalSkinSurface.hh"
#include "G4OpticalSurface.hh"
#include "G4MaterialTable.hh"
#include "G4Material.hh"
#include "G4Box.hh"
#include "G4Tubs.hh"
#include "G4Trd.hh"
#include "G4Sphere.hh"
#include "G4TwistedBox.hh"
#include "G4LogicalVolume.hh"
#include "G4ThreeVector.hh"
#include "globals.hh"
#include "G4UnionSolid.hh"
#include "G4Material.hh"
#include "G4VisAttributes.hh"
#include "G4RotationMatrix.hh"
#include "G4VUserDetectorConstruction.hh"
#include "G4LogicalVolumeStore.hh"
#include "G4PhysicalVolumeStore.hh"
#include "G4SolidStore.hh"
#include "G4UserLimits.hh"
#include "G4PhysicsTable.hh"
#include "G4PhysicsOrderedFreeVector.hh"
#include "G4OpBoundaryProcess.hh"

//similar to AMSgvolume just for less geometry, fast mode
class TOFgvolume : public AMSNode{
protected:
 G4int      _npar; 
 G4double*  _par;        //   ! geant volume par
 char*     _matter; 
 char*     _shape;
 G4LogicalVolume * _pg4l;

public:
 TOFgvolume():AMSNode(0),_npar(0),_par(0),_shape(0),_matter(0),_pg4l(0){};
 TOFgvolume (const char matter[],const char shape[],G4int _npar, G4double par[],G4LogicalVolume *pg4l);
 virtual void _init(){};
 G4LogicalVolume *GetSameG4Vol(const char matter[],const char shape[],G4int npar, G4double par[]);
 inline G4int     getnpar()   {return _npar;}
 inline G4double* getpar(){return _par;}
 inline char* getmatter(){return _matter;}
 inline char* getshape(){return _shape;}
 G4LogicalVolume* getvol(){return _pg4l;}
 TOFgvolume* next(){return dynamic_cast<TOFgvolume*>(AMSNode::next());}
 TOFgvolume* prev(){return dynamic_cast<TOFgvolume*>(AMSNode::prev());}
 TOFgvolume* up()  {return dynamic_cast<TOFgvolume*>(AMSNode::up());}
 TOFgvolume* down(){return dynamic_cast<TOFgvolume*>(AMSNode::down());}
};



class G4LogicalVolume;
class G4VPhysicalVolume;
class TofSimUtil
{
  private:
   G4Element* C;
   G4Element* H;
   G4Element* N;
   G4Element* O;
   G4Element* Si;
   G4Material* TOFVa_M;//Vacumm
   G4Material* TOFAl_M;//Al
   G4Material* TOFAlHoney_M;
   G4Material* TOFCA_M;//Carbon
   G4Material* TOFCAFibre_M;
   G4Material* TOFFe_M;//Fe
   G4Material* TOFSc_M;//EJ-200
   G4Material* TOFLg_M;//PMMA
   G4Material* TOFPM_M;//PMT present Si02
   G4MaterialPropertiesTable* TOFSc_Mt;//sci material table
   G4MaterialPropertiesTable* TOFLg_Mt;//lg material table
   G4OpticalSurface* TOFPM_Su;//material surface
   G4OpticalSurface* TOFSC_Su;
   G4OpticalSurface* TOFLG_Su;
   G4UserLimits*     TOF_Limit;
 
 private:
  static const int NLAY=4;
  static const int NBAR=10;
  static const int NSIDE=2;
  static const int NPMTM=3;
  static int Nbar[NLAY];
  static int Npmt[NLAY][NBAR];
  static G4double Overlapw;
  static G4double Normw;
  static G4double Nthick;
  static G4double Zc[NLAY];
  static G4double Zshift;
  static G4double Honshift[2][2];
  static G4double Sci_l[NLAY][NBAR];
  static G4double Sci_w[NLAY][NBAR];
  static G4double Sci_lc[NLAY][NBAR];
  static G4double Sci_wc[NLAY][NBAR];
  static G4double Sci_t[NLAY][NBAR];
  static G4double Lg_l[NLAY][NBAR];
  static G4double Lg_wc[NLAY][NBAR];
  static G4double Lg_lc[NLAY][NBAR];
  static G4double Lgb_a[NLAY][NBAR];
  static G4double Lgb_r[NLAY][NBAR];
  static G4double Lgt_l[NLAY][NBAR];
  static G4double Twista[NLAY][NBAR];
  static G4double Lgc_l[NLAY][NBAR];
  static G4double Pm_s[NLAY][NBAR];
  static G4double DHoneyH[3];
  static G4double PHoneyH[3];
  static G4double DHoneyL[3];
  static G4double PHoneyL[3];
  static G4double DCarbonR;
  static G4double DCarbonZ;
  static G4double PCarbon[4][3];
  G4OpticalSurface*          TOFSC_SuA[NLAY][NBAR];
  G4MaterialPropertiesTable* TOFSc_MtA[NLAY][NBAR];//sci material table all
  G4Material*                TOFSc_MA[NLAY][NBAR];//sci material all

 private:
  int npmt;
  G4double barp[3];
  //G4double barrot;
  G4double barrot[3];//rotation angle
  G4double sci_l;
  G4double sci_w;
  G4double sci_lc;
  G4double sci_wc;
  G4double sci_t;
  G4double lg_l;
  G4double lg_lc;
  G4double lg_wc;
  G4double lgb_r;
  G4double lgb_a;
  G4double lgt_l;
  G4double twista;
  G4double lgc_l;
  G4double pm_s;
  G4VSolid*        sci_sol;
  G4LogicalVolume* sci_log;
  G4VSolid*        lgp_sol;
  G4LogicalVolume* lgp_log;
  G4VSolid*        lgb_sol;
  G4LogicalVolume* lgb_log;
  G4VSolid*        lgt_sol;
  G4LogicalVolume* lgt_log;
  G4VSolid*        lgc_sol;
  G4LogicalVolume* lgc_log;
  G4LogicalVolume* lgc_log1;
  G4VSolid*        pm_sol;
  G4LogicalVolume* pm_log;
  G4LogicalVolume* pm_log1;
  TOFgvolume *tofgvol;
  
  public:
    static void CreateTofG4();
    static TofSimUtil* Head;
    static G4PhysicsTable* TOFPM_Et;
    static G4double QEMAX;
    static G4double SCRIND;
    static G4double LGRIND;
    static G4double VARIND;
    static G4double LOBSIG[NLAY][NBAR];
    static G4double SCEFFC[NLAY][NBAR];
    static G4double PHEFFC[NLAY][NBAR][NSIDE][NPMTM];//effeciency uncertainty correction
  public:
    TofSimUtil();
   ~TofSimUtil();
   void DefineTOF_M();
   void DefineTOF_Mt();
   void DefineTOF_Su();
   void DefineTOF_PME();
   void DefineTOF_Limit();
   void InitialBarPar(int ilay,int ibar);
   G4AssemblyVolume* ConstructSubBar(G4String sci_name);
   void              ConstructBarSurface(G4AssemblyVolume *Asvol,G4VPhysicalVolume *Mvol);
   G4VSolid* SpeTrd(G4String trapname,G4double trdpa[5],G4int idir=0);
//   G4VSolid* SpeTrd(G4String trapname,G4double trdpa[5]);
   void Attributes();
   void init();
  public:
   AMSgmat* GetAMSgmat(G4Material *mat);
   bool     AddTOFgmat(AMSgmat &mat);
   bool     AddTOFgtmed(AMSgtmed &gtmat);
   AMSgvolume *GetAMSgvolume(G4VPhysicalVolume *phyv, int &nrot);
   bool MakeTOFG4Volumes(AMSgvolume *mother); 
};


#endif
