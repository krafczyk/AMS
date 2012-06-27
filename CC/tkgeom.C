#include <strstream>

#ifdef _PGTRACK_
#include "gvolume.h"  // for AMSgvolume

#include "TkDBc.h"
#include "TkLadder.h"
#include "TkPlane.h"
#include "TkCoo.h"

namespace amsgeom{
extern void tkgeom02(AMSgvolume &);
extern void magnetgeom(AMSgvolume &);
extern void magnetgeom02(AMSgvolume &);
extern void ext1structure02(AMSgvolume &);
extern void magnetgeom02o(AMSgvolume &);
extern void magnetgeom02Test(AMSgvolume &);
extern void tofgeom02(AMSgvolume &);
extern void antigeom02(AMSgvolume &);
extern void antigeom002(AMSgvolume &);
extern void ext2structure(AMSgvolume &);
#ifdef __G4AMS__
extern void antigeom02g4(AMSgvolume &);
extern void testg4geom(AMSgvolume &);
#endif
extern void richgeom02(AMSgvolume &,float zshift=0);
extern void ecalgeom02(AMSgvolume &);
extern void trdgeom02(AMSgvolume &, float zshift=0);
//extern void srdgeom02(AMSgvolume &);
extern void Put_rad(AMSgvolume *,integer);
extern void Put_pmt(AMSgvolume *,integer);
};
using namespace amsgeom;

// Ladder support thickness in cm
static double sup_foam_w   = 0.5;
// Ladder support z-offset in cm
static double sup_foam_tol = 0.05;

int _nrot = 0;


void        BuildHoneycomb    (AMSgvolume *mvol, int plane);
AMSgvolume *BuildPlaneEnvelop (AMSgvolume *mvol, int plane);
AMSgvolume *BuildLadder       (AMSgvolume *mvol, int tkid);
void        BuildSensor       (AMSgvolume *mvol, int tkid, int s);
void        BuildHybrid       (AMSgvolume *mvol, int tkid);
void        BuildSupport      (AMSgvolume *mvol, int tkid);
void      BuildPlane1NSupport (AMSgvolume* mvol);


void amsgeom::tkgeom02(AMSgvolume &mother)
{
// Build tracker geometry

  // Temporary arbitary assignment
  _nrot = 501;

  int nladder = 0;
  if(TkDBc::Head->GetSetup()==3){ // PLANE 5N aka 7
    AMSgvolume *vplane = BuildPlaneEnvelop(&mother, 7);
    // Build plane support
    BuildHoneycomb(vplane, 7);
    BuildPlane1NSupport(&mother);

  }
  // Loop on planes
  for (int plane=1; plane<=TkDBc::Head->GetNPlanes();plane++){

    // Build a plane geometry (made of VACUUM)
    AMSgvolume *vplane = BuildPlaneEnvelop(&mother, plane);
    // Build plane support
    BuildHoneycomb(vplane, plane);
    
    // Loop on Si layer attached to the plane
    int maxp_lay=1;
    int minp_lay=0;
    if(plane==1)      {minp_lay= 1;maxp_lay=2;}
    else if(plane==5) {minp_lay= 0;maxp_lay=1;}
    else              {minp_lay= 0;maxp_lay=2;}

    if(TkDBc::Head->GetSetup()==3 &&plane==6) 
      {minp_lay=-1;maxp_lay=0;}


    for (int p_lay = minp_lay; p_lay<maxp_lay; p_lay++) {
      int layer=2*(plane-1)+p_lay;
         
      // Loop for each slot
      for (int slot = 1; slot <= trconst::maxlad; slot++) {
	
	// Loop for each side
	for (int side = -1; side <= 1; side += 2) {
	  
	  // Check TkId is valid or not
	  int tkid = side*(layer*100+slot);
	  TkLadder *lad = TkDBc::Head->FindTkId(tkid);
	  if (!lad) continue;
	  
	  // Build ladder
	  AMSgvolume *vlad = BuildLadder(vplane, tkid);
	  nladder++;
	  
	  // Build electronics
	  BuildHybrid(vplane, tkid);
	  
	  // Build ladder support
	  BuildSupport(vplane, tkid);

	} //side
      } //slot
      
    }//lay

  }//plane
  std::cout << "<---- TKGeom-I-" << nladder
            << " Active halfladders initialized" 
            << std::endl << std::endl;
}
//============================================================================
AMSgvolume *BuildPlaneEnvelop(AMSgvolume *mvol, int plane)
{
  // Build Plane container geometry (plane number begins from 1)
  char name[5];
  geant coo[3];
  geant par[3];
  AMSgvolume* volout;
  number nrm[3][3]={{1,0,0},{0,1,0},{0,0,1}};
  std::ostrstream ost(name,sizeof(name));
  ost << "STK" << plane << std::ends;

  TkPlane *pl = TkDBc::Head->GetPlane(plane);
  if(!pl && plane!=7){
    printf(" AMSgvolume *BuildPlaneEnvelop: cannot find plane #%d\n I Give Up!!\n",plane);
      exit(2);
  }
  if(plane ==7) {//PLAN -B DUMMY plane 5N
    TkPlane *pl = TkDBc::Head->GetPlane(1);
    coo[0] = pl->GetPos()[0]+pl->GetPosT()[0];
    coo[1] = pl->GetPos()[1]+pl->GetPosT()[1];
    coo[2] = -1*(pl->GetPos()[2]+pl->GetPosT()[2]+
		 pl->GetRotMat().GetEl(2,2)*TkDBc::Head->_dz[0]);
      
    par[0] = 0;
    par[1] = TkDBc::Head->_plane_d1[0]; //container radius
    par[2] = TkDBc::Head->_plane_d2[0]; //container half thickness
  
    
    AMSRotMat lrm0 = pl->GetRotMatT();
    AMSRotMat lrm = lrm0*pl->GetRotMat();
  
    for (int ii = 0; ii < 9; ii++) nrm[ii/3][ii%3] = lrm.GetEl(ii/3,ii%3);
    volout=(AMSgvolume*)mvol
      ->add(new AMSgvolume("VACUUM", _nrot++, name, "TUBE",
                          par, 3, coo, nrm, "ONLY", 0, plane, 1));    
    }
  else if(plane==6){ //PLAN -B ECAL PLANE 
      // Center is offset because of the ladder asymmetry
      coo[0] = pl->GetPos()[0]+pl->GetPosT()[0];
      coo[1] = pl->GetPos()[1]+pl->GetPosT()[1];
      coo[2] = pl->GetPos()[2]+pl->GetPosT()[2]+TkDBc::Head->_dz[plane-1];
      par[0]=TkDBc::Head->Plane6EnvelopSize[0]/2.;
      //       plan6E Y size/2.
      par[1]=TkDBc::Head->Plane6EnvelopSize[1]/2.;
      //       plan6E thickness/2.
      par[2]=TkDBc::Head->Plane6EnvelopSize[2]/2.;
      cout <<" Placing the Tk  Plan 6 Vol at "<< coo[0]<<"  "<< coo[1]<<"  "<< coo[2]<<
	"with half dim "<<par[0]<<"  "<<par[1]<<"  "<<par[2]<<endl;
    
      AMSRotMat lrm0 = pl->GetRotMatT();
      AMSRotMat lrm = lrm0*pl->GetRotMat();
    
      for (int ii = 0; ii < 9; ii++) nrm[ii/3][ii%3] = lrm.GetEl(ii/3,ii%3);
    volout= (AMSgvolume*)mvol
        ->add(new AMSgvolume("VACUUM", _nrot++, name, "BOX",
                             par, 3, coo, nrm, "ONLY", 0, plane, 1));
  }else{ //STD AMS PLANES
  
    coo[0] = pl->GetPos()[0]+pl->GetPosT()[0];
    coo[1] = pl->GetPos()[1]+pl->GetPosT()[1];
    coo[2] = pl->GetPos()[2]+pl->GetPosT()[2]+
      pl->GetRotMat().GetEl(2,2)*TkDBc::Head->_dz[plane-1];
  
    par[0] = 0;
    par[1] = TkDBc::Head->_plane_d1[plane-1]; //container radius
    par[2] = TkDBc::Head->_plane_d2[plane-1]; //container half thickness
  
    AMSRotMat lrm0 = pl->GetRotMatT();
    AMSRotMat lrm = lrm0*pl->GetRotMat();
  
    for (int ii = 0; ii < 9; ii++) nrm[ii/3][ii%3] = lrm.GetEl(ii/3,ii%3);
    volout=(AMSgvolume*)mvol
      ->add(new AMSgvolume("VACUUM", _nrot++, name, "TUBE",
                           par, 3, coo, nrm, "ONLY", 0, plane, 1));    
    
  }
  if(plane>0){
    printf("\nPlane Envelope %d\n",plane);
    printf(" Coo %f %f %f\n ",coo[0],coo[1],coo[2]);    
    printf(" Half dim %f %f %f\n ",par[0],par[1],par[2]);
    printf(" Matr  %f %f %f \n",nrm[0][0],nrm[0][1],nrm[0][2]);
    printf("       %f %f %f \n",nrm[1][0],nrm[1][1],nrm[1][2]);
    printf("       %f %f %f \n",nrm[2][0],nrm[2][1],nrm[2][2]);
  }
  return volout; 
}
//============================================================================

AMSgvolume *BuildLadder(AMSgvolume *mvol, int tkid)
{
// Build ladder geometry

    TkLadder *lad = TkDBc::Head->FindTkId(tkid);
  if (!lad) return 0;

  int layer = std::abs(tkid)/100;
  int islot = (tkid > 0) ? tkid%100 : -tkid%100+20;
  int iname = layer*100+islot;

  char name[5];
  std::ostrstream ost(name,sizeof(name));
  ost << "L" << iname << std::ends;

  geant par[3];
  par[0] = TkCoo::GetLadderLength(tkid)/2;
  par[1] = TkDBc::Head->_ssize_inactive[1]/2;
  par[2] = TkDBc::Head->_silicon_z/2;

  double hlen = TkCoo::GetLadderLength(tkid)/2
    -(TkDBc::Head->_ssize_inactive[0]-TkDBc::Head->_ssize_active[0])/2;
  double hwid = TkDBc::Head->_ssize_active[1]/2;

  AMSRotMat rot0 = lad->GetRotMatT();
  AMSRotMat rot = rot0*lad->GetRotMat();

  AMSPoint  pos = lad->GetPos()+lad->GetPosT();
  AMSPoint  loc(hlen, hwid, 0);
  AMSPoint  oo = rot*loc+pos;

  // Get coordinate w.r.t. the mother layer
  geant coo[3];
  coo[0] = oo.x();
  coo[1] = oo.y();
  coo[2] = oo.z();
  if(layer==1) coo[2] -= TkDBc::Head->_dz[0];
  if(layer==8) coo[2] -= TkDBc::Head->_dz[4];
  if(layer==9) coo[2] -= TkDBc::Head->_dz[5];

  AMSRotMat lrm0 = lad->GetRotMatT();
  AMSRotMat lrm = lrm0*lad->GetRotMat();
  

  number nrm[3][3];
  for (int ii = 0; ii < 9; ii++) nrm[ii/3][ii%3] = lrm.GetEl(ii/3,ii%3);

  int gid =abs(tkid)/tkid* (abs(tkid)+1000);
  // printf("Sensor name %s  %+03d   %+9d %f %f %f\n",name,tkid,gid,coo[0],coo[1],coo[2]);
  AMSgvolume* ladd=(AMSgvolume*)mvol
    ->add(new AMSgvolume("NONACTIVE_SILICON", _nrot++, name, "BOX",
			 par, 3, coo, nrm, "ONLY", 1, gid, 1));

  //  printf("Ladder name %s\n",name);
  
  // Build sensors
  for (int sensor = 0; sensor < lad->GetNSensors(); sensor++){
    char nameS[5];
    std::ostrstream ost2(nameS,sizeof(nameS));
    ost2 << "S" << iname << std::ends;

    geant par[3];
    par[0] = TkDBc::Head->_ssize_active[0]/2;
    par[1] = TkDBc::Head->_ssize_active[1]/2;
    par[2] = TkDBc::Head->_silicon_z/2;
    
    // Get coordinate w.r.t. the mother ladder
    geant coo[3];
    coo[0] = -TkCoo::GetLadderLength(tkid)/2+ TkDBc::Head->_ssize_inactive[0]/2.+ sensor * TkDBc::Head->_SensorPitchK;
    coo[1] = coo[2] = 0;



    number nrm[3][3];
    VZERO(nrm, 9*sizeof(nrm[0][0])/4);
    nrm[0][0] = nrm[1][1] = nrm[2][2] = 1;
    int lside= (tkid>0)? 1:0;
    int gid = abs(tkid)+1000*(lside)+10000*(sensor+1); 
    //    printf("Sensor name %s  %+03d   %+9d %f %f %f\n",nameS,tkid,gid,coo[0],coo[1],coo[2]);
   ladd->add(new AMSgvolume("ACTIVE_SILICON", _nrot++, nameS, "BOX",
			     par, 3, coo, nrm, "ONLY", 1, gid, 1));
  }
  return ladd;
}
//============================================================================

void BuildHybrid(AMSgvolume *mvol, int tkid)
{
// Build hybrid geometry

  TkLadder *lad = TkDBc::Head->FindTkId(tkid);
  if (!lad) return;

  int layer = std::abs(tkid)/100;
  int plane= TkDBc::Head->_plane_layer[layer-1];
  int sign  = (tkid > 0) ? 1 : -1;

  char name[5];
  std::ostrstream ost(name,sizeof(name));
  ost << ((tkid < 0) ? "ELL" : "ELR") << layer << std::ends;

  geant par[3];
  if(layer==1 ||layer==8||layer==9){
    par[0] = TkDBc::Head->_zelec[2]/2;
    par[1] = TkDBc::Head->_zelec[1]/2;
    par[2] = TkDBc::Head->_zelec[0]/2;
  }else{
    par[0] = TkDBc::Head->_zelec[0]/2;
    par[1] = TkDBc::Head->_zelec[1]/2;
    par[2] = TkDBc::Head->_zelec[2]/2;
  }
  double hlen = TkCoo::GetLadderLength(tkid)/2
    -(TkDBc::Head->_ssize_inactive[0]-TkDBc::Head->_ssize_active[0])/2;
  double hwid = TkDBc::Head->_ssize_active[1]/2;

  AMSRotMat rot0 = lad->GetRotMatT();
  AMSRotMat rot = rot0*lad->GetRotMat();

  AMSPoint  pos = lad->GetPos()+lad->GetPosT();
  AMSPoint  loc(hlen, hwid, 0);
  AMSPoint  oo = rot*loc+pos;

  // Get coordinate w.r.t. the mother layer
  geant coo[3];
  coo[0] = oo.x() +sign*(TkCoo::GetLadderLength(tkid)/2+par[0]);
  coo[1] = oo.y();
  coo[2] = (abs(oo.z())/oo.z())*(TkDBc::Head->_sup_hc_w[plane-1]/2.+par[2]); 
  if(layer==1) coo[2] -= TkDBc::Head->_dz[0];
  if(layer==8) coo[2] -= TkDBc::Head->_dz[4];
  if(layer==9) coo[2] -= TkDBc::Head->_dz[5];


  
  number nrm[3][3];
  VZERO(nrm,9*sizeof(nrm[0][0])/4);
  nrm[0][0] = nrm[1][1] = nrm[2][2] = 1;

  int gid = tkid+1000;
  mvol->add(new AMSgvolume("ELECTRONICS", _nrot++, name,
			   "BOX", par, 3, coo, nrm, "ONLY", 1, gid, 1));
}
//============================================================================

void BuildSupport(AMSgvolume *mvol, int tkid)
{
// Build ladder support geometry

  TkLadder *lad = TkDBc::Head->FindTkId(tkid);

  if (!lad) return;


  int layer = std::abs(tkid)/100;
  
  char name[5];
  std::ostrstream ost(name,sizeof(name));
  ost << "FOA" << layer << std::ends;
  
  int nsen = lad->GetNSensors();
  double signp = (layer%2 == 1) ? -1 : 1;

  geant par[3];
  par[0] = TkDBc::Head->_ssize_inactive[0]*nsen/2;
  par[1] = 7.3/2;//TkDBc::Head->_ladder_Ypitch/2;
  par[2] = sup_foam_w/2;    
  

  double hlen = TkCoo::GetLadderLength(tkid)/2
    -(TkDBc::Head->_ssize_inactive[0]-TkDBc::Head->_ssize_active[0])/2;
  double hwid = TkDBc::Head->_ssize_active[1]/2;
  
  AMSRotMat rot0 = lad->GetRotMatT();
  AMSRotMat rot = rot0*lad->GetRotMat();

  AMSPoint  pos = lad->GetPos()+lad->GetPosT();
  AMSPoint  loc(hlen, hwid, 0);
  AMSPoint  oo = rot*loc+pos;

// Get coordinate w.r.t. the mother layer
  geant coo[3];
  coo[0] = oo.x();
  coo[1] = oo.y();
  coo[2] = (abs(oo.z())/oo.z())*(abs(oo.z())-TkDBc::Head->_silicon_z/2-par[2]-sup_foam_tol);
  if(layer==1) coo[2] -= TkDBc::Head->_dz[0];
  if(layer==8) coo[2] -= TkDBc::Head->_dz[4];
  if(layer==9) coo[2] -= TkDBc::Head->_dz[5];

	     
	     
  number nrm[3][3];
  VZERO(nrm,9*sizeof(nrm[0][0])/4);
  nrm[0][0] = nrm[1][1] = nrm[2][2] = 1;

  int gid = tkid+1000;
  mvol->add(new AMSgvolume("Tr_Foam", _nrot++, name, "BOX",
			   par, 3, coo, nrm, "ONLY", 1, gid, 1));
}


//============================================================================
void BuildHoneycomb(AMSgvolume *mvol, int plane)
{
// Build plane honeycomb support geometry (plane number begins from 1)
  geant par[3];
  geant coo[3];
  number nrm[3][3];
  char name[5];
  char name2[5];
  std::ostrstream ost(name,sizeof(name));
  ost << "PLA" << plane << std::ends;
  if(plane==7){
    par[0] = 0;
    par[1] = TkDBc::Head->_sup_hc_r[0];
    par[2] = TkDBc::Head->_sup_hc_w[0]/2. - TkDBc::Head->_sup_hc_skin_w[0];
    
    coo[0] = coo[1] = 0;
    coo[2] = TkDBc::Head->_dz[0];
    
    
    VZERO(nrm,9*sizeof(nrm[0][0])/4);
    nrm[0][0] = nrm[1][1] = nrm[2][2] = 1;
    
    mvol->add(new AMSgvolume("Tr_HoneyOUT", _nrot++, name,
			     "TUBE", par, 3, coo, nrm, "ONLY", 1, plane, 1));

    par[0] = 0;
    par[1] = TkDBc::Head->_sup_hc_r[0];
    par[2] = TkDBc::Head->_sup_hc_skin_w[0]/2.;
    coo[0] = coo[1] = 0;
    coo[2] = TkDBc::Head->_dz[0]+ TkDBc::Head->_sup_hc_w[0]/2. + TkDBc::Head->_sup_hc_skin_w[0]/2.;
    sprintf(name2,"USK%d",plane);
    mvol->add(new AMSgvolume("Tr_HoneySkin", _nrot++, name2,
			     "TUBE", par, 3, coo, nrm, "ONLY", 1, plane, 1));

    coo[2] = TkDBc::Head->_dz[0]- TkDBc::Head->_sup_hc_w[0]/2. - TkDBc::Head->_sup_hc_skin_w[0]/2.;
    sprintf(name2,"LSK%d",plane);
    mvol->add(new AMSgvolume("Tr_HoneySkin", _nrot++, name2,
			     "TUBE", par, 3, coo, nrm, "ONLY", 1, plane, 1));

  }else if(plane==6){
    //       plan6 X size/2.
    par[0]=TkDBc::Head->Plane6Size[0]/2.;
    //       plan6 Y size/2.
    par[1]=TkDBc::Head->Plane6Size[1]/2.;
    //       plan6E thickness/2.
    par[2]= TkDBc::Head->Plane6Size[2]/2.-TkDBc::Head->Plane6_skin_w[0];    //       plan6E X size/2.

    
    coo[0] = coo[1] = 0;
    coo[2] =-1* (TkDBc::Head->Plane6EnvelopSize[2]  -TkDBc::Head->Plane6Size[2])/2;
    
    
    VZERO(nrm,9*sizeof(nrm[0][0])/4);
    nrm[0][0] = nrm[1][1] = nrm[2][2] = 1;
    
    mvol->add(new AMSgvolume("Tr_HoneyOUT", _nrot++, name,
			     "BOX", par, 3, coo, nrm, "ONLY", 1, plane, 1));

    printf("\nPlane %d\n",plane);
    printf(" Coo %f %f %f\n ",coo[0],coo[1],coo[2]);    
    printf(" Half dim %f %f %f\n ",par[0],par[1],par[2]);
    printf(" Matr  %f %f %f \n",nrm[0][0],nrm[0][1],nrm[0][2]);
    printf("       %f %f %f \n",nrm[1][0],nrm[1][1],nrm[1][2]);
    printf("       %f %f %f \n",nrm[2][0],nrm[2][1],nrm[2][2]);

    //ADD CARBON SKINS
    //       plan6 X size/2.
    par[0]= TkDBc::Head->Plane6Size[0]/2.;
    //       plan6 Y size/2.
    par[1]= TkDBc::Head->Plane6Size[1]/2.;
    //       plan6E thickness/2.
    par[2]= TkDBc::Head->Plane6_skin_w[0]/2.;    //       plan6E X size/2.
    
    
    coo[0] = coo[1] = 0;
    coo[2] =-1* (TkDBc::Head->Plane6EnvelopSize[2]  -TkDBc::Head->Plane6Size[2])/2 - 
      ( TkDBc::Head->Plane6Size[2]+TkDBc::Head->Plane6_skin_w[0])/2.;
    sprintf(name2,"USK%d",plane);
    mvol->add(new AMSgvolume("pl6_HoneySkin", _nrot++, name2,
			     "BOX", par, 3, coo, nrm, "ONLY", 1, plane, 1));

    coo[2] =-1* (TkDBc::Head->Plane6EnvelopSize[2]  -TkDBc::Head->Plane6Size[2])/2 + 
      ( TkDBc::Head->Plane6Size[2]+TkDBc::Head->Plane6_skin_w[0])/2.;

    sprintf(name2,"LSK%d",plane);
    mvol->add(new AMSgvolume("pl6_HoneySkin", _nrot++, name2,
			     "BOX", par, 3, coo, nrm, "ONLY", 1, plane, 1));

    //----------------------------------------------------------------------------------
     // Add the top cover of plane 6
     sprintf(name,"COV%d",plane );

     //       plan6 X size/2.
     par[0]=TkDBc::Head->Plane6Size[0]/2.;
     //       plan6 Y size/2.
     par[1]=TkDBc::Head->Plane6Size[1]/2.;
    //       plan6E thickness/2.
     par[2]= 0.4/2. -TkDBc::Head->Plane6_skin_w[1] ;    //       plan6 cover Z size/2.

    
    coo[0] = coo[1] = 0;
    coo[2] =  (TkDBc::Head->Plane6EnvelopSize[2]  - 0.4 )/2;
    
    
    VZERO(nrm,9*sizeof(nrm[0][0])/4);
    nrm[0][0] = nrm[1][1] = nrm[2][2] = 1;
    // PZ FIXME cross check the material
    mvol->add(new AMSgvolume("Tr_HoneyIN", _nrot++, name,
			     "BOX", par, 3, coo, nrm, "ONLY", 1, plane, 1));

    printf("\nPlane cover %d\n",plane);
    printf(" Coo %f %f %f\n ",coo[0],coo[1],coo[2]);    
    printf(" Half dim %f %f %f\n ",par[0],par[1],par[2]);
    printf(" Matr  %f %f %f \n",nrm[0][0],nrm[0][1],nrm[0][2]);
    printf("       %f %f %f \n",nrm[1][0],nrm[1][1],nrm[1][2]);
    printf("       %f %f %f \n",nrm[2][0],nrm[2][1],nrm[2][2]);


    //ADD CARBON SKINS
    //       plan6 X size/2.
    par[0]=TkDBc::Head->Plane6Size[0]/2.;
    //       plan6 Y size/2.
    par[1]=TkDBc::Head->Plane6Size[1]/2.;
    //       plan6E thickness/2.
    par[2]= TkDBc::Head->Plane6_skin_w[1]/2.;    //       plan6E X size/2.
    
    
    coo[0] = coo[1] = 0;
    coo[2] =(TkDBc::Head->Plane6EnvelopSize[2]  - 0.4 )/2 - 
      ( TkDBc::Head->Plane6Size[2]+TkDBc::Head->Plane6_skin_w[1])/2.;
    sprintf(name2,"USH%d",plane);
    mvol->add(new AMSgvolume("pl6_HoneySkin", _nrot++, name2,
			     "BOX", par, 3, coo, nrm, "ONLY", 1, plane, 1));

    coo[2] =-1* (TkDBc::Head->Plane6EnvelopSize[2]  - 0.4)/2 + 
      ( TkDBc::Head->Plane6Size[2]+TkDBc::Head->Plane6_skin_w[1])/2.;

    sprintf(name2,"LSH%d",plane);
    mvol->add(new AMSgvolume("pl6_HoneySkin", _nrot++, name2,
			     "BOX", par, 3, coo, nrm, "ONLY", 1, plane, 1));



    //-------------------------------------------------------    
    

    // add small Spacers
    for(int kk=-3;kk<=3;kk++){
      sprintf(name,"SPA%d",kk+3 );
      
      //       Spacer X size/2.
      par[0]= 5.0 /2.;
      //       Spacer Y size/2.
      par[1]= 0.7 /2.;
     //       Spacer thickness/2.
      par[2]=  1.9 /2.;    //       plan6 cover Z size/2.
     
     //            Spacers pitch
     coo[0] = kk * 14.2;
     coo[1] = 0;
     coo[2] =TkDBc::Head->Plane6Size[2] -TkDBc::Head->Plane6EnvelopSize[2]/2.  + par[2] ;
    
    
     VZERO(nrm,9*sizeof(nrm[0][0])/4);
     nrm[0][0] = nrm[1][1] = nrm[2][2] = 1;

     mvol->add(new AMSgvolume("pl6_HoneySkin", _nrot++, name,
			      "BOX", par, 3, coo, nrm, "ONLY", 1, kk+3, 1));

    }
    // add long Spacers
    for(int kk=-1;kk<2;kk+=2){
      sprintf(name,"SPC%+d",kk );
      
      //       Spacer X size/2.
      par[0]= 72.0 /2.;
      //       Spacer Y size/2.
      par[1]= 1.4 /2.;
     //       Spacer thickness/2.
     par[2]=  1.9 /2.;    //       plan6 cover Z size/2.
     
     //            Spacers pitch
     coo[0] = 0;
     coo[1] = kk*(34+par[1]);
     coo[2] =TkDBc::Head->Plane6Size[2] -TkDBc::Head->Plane6EnvelopSize[2]/2.  + par[2] ;
    
    
     VZERO(nrm,9*sizeof(nrm[0][0])/4);
     nrm[0][0] = nrm[1][1] = nrm[2][2] = 1;
     // FIX ME  material !!!!!!
     mvol->add(new AMSgvolume("pl6_HoneySkin", _nrot++, name,
			      "BOX", par, 3, coo, nrm, "ONLY", 1, kk+2, 1));

    }



  }
  else{ //normal planes
  
    par[0] = 0;
    par[1] = TkDBc::Head->_sup_hc_r[plane-1];
    par[2] = TkDBc::Head->_sup_hc_w[plane-1]/2.-TkDBc::Head->_sup_hc_skin_w[plane-1];
    
    coo[0] = coo[1] = 0;
    coo[2] = -TkDBc::Head->_dz[plane-1];
    
    
    VZERO(nrm,9*sizeof(nrm[0][0])/4);
    nrm[0][0] = nrm[1][1] = nrm[2][2] = 1;
    char mate[50];
    if(plane==1||plane==5)
      sprintf(mate,"Tr_HoneyOUT");
    else
      sprintf(mate,"Tr_HoneyIN");
    mvol->add(new AMSgvolume(mate, _nrot++, name,
			     "TUBE", par, 3, coo, nrm, "ONLY", 1, plane, 1));

    // ADD CARBON SKINS
    par[0] = 0;
    par[1] = TkDBc::Head->_sup_hc_r[plane-1];
    par[2] = TkDBc::Head->_sup_hc_skin_w[plane-1]/2.;
    coo[0] = coo[1] = 0;
    coo[2] = -TkDBc::Head->_dz[plane-1]+ TkDBc::Head->_sup_hc_w[plane-1]/2. + TkDBc::Head->_sup_hc_skin_w[plane-1]/2.;

    sprintf(name2,"USK%d",plane);
    mvol->add(new AMSgvolume("Tr_HoneySkin", _nrot++, name2,
			     "TUBE", par, 3, coo, nrm, "ONLY", 1, plane, 1));

    coo[2] = -TkDBc::Head->_dz[plane-1]- TkDBc::Head->_sup_hc_w[plane-1]/2. - TkDBc::Head->_sup_hc_skin_w[plane-1]/2.;
    sprintf(name2,"LSK%d",plane);
    mvol->add(new AMSgvolume("Tr_HoneySkin", _nrot++, name2,
			     "TUBE", par, 3, coo, nrm, "ONLY", 1, plane, 1));

  }

  if(plane>0&&plane!=6){
    printf("\nPlane %d\n",plane);
    printf(" Coo %f %f %f\n ",coo[0],coo[1],coo[2]);    
    printf(" Half dim %f %f %f\n ",par[0],par[1],par[2]);
    printf(" Matr  %f %f %f \n",nrm[0][0],nrm[0][1],nrm[0][2]);
    printf("       %f %f %f \n",nrm[1][0],nrm[1][1],nrm[1][2]);
    printf("       %f %f %f \n",nrm[2][0],nrm[2][1],nrm[2][2]);
  }

  return;
}




void BuildPlane1NSupport(AMSgvolume* mvol){

  //Leps honeycomb support
  float coo[3];
  float par[20];
  number nrm[3][3];
  char name[5];
  float pi=atan(1.)*4;

  sprintf(name,"P1NS");
  coo[0]=TkDBc::Head->P1NSupportCoo[0];
  coo[1]=TkDBc::Head->P1NSupportCoo[1];
  coo[2]=TkDBc::Head->P1NSupportCoo[2];

  //Al- honeycomb
  par[0]=0;    //inital phi
  par[1]=360;  // phi amplitude
  par[2]=8;    // pgon sides
  par[3]=2;    // n z planes
  par[4]=-TkDBc::Head->P1NSupportThickness/2.;   // z1
  par[5]=0;
  par[6]=TkDBc::Head->P1NSupportRadius;
  par[7]=TkDBc::Head->P1NSupportThickness/2.;
  par[8]=0;
  par[9]=TkDBc::Head->P1NSupportRadius;
  VZERO(nrm,9*sizeof(nrm[0][0])/4);
  nrm[0][0] = nrm[1][1] = nrm[2][2] = 1;  
  nrm[0][0] = nrm[1][1] =cos(22.5/180.*pi);
  nrm[0][1] = sin(22.5/180.*pi);
  nrm[1][0] = -1*sin(22.5/180.*pi);
  nrm[2][2] = 1;  
  
  mvol->add(new AMSgvolume("Tr_HoneyOUT", _nrot++, name,
			      "PGON", par, 10, coo, nrm, "ONLY", 1, 1, 1));


  sprintf(name, "P1NK");
  coo[0]=TkDBc::Head->P1NSupportCoo[0];
  coo[1]=TkDBc::Head->P1NSupportCoo[1];
  coo[2]=TkDBc::Head->P1NSupportCoo[2]+
    TkDBc::Head->P1NSupportThickness/2.+
    TkDBc::Head->P1NSupportSkinThickness/2.;

  //Al- honeycomb skin
  par[0]=0;    //inital phi
  par[1]=360;  // phi amplitude
  par[2]=8;    // pgon sides
  par[3]=2;    // n z planes
  par[4]=-TkDBc::Head->P1NSupportSkinThickness/2.;   // z1
  par[5]=0;
  par[6]=TkDBc::Head->P1NSupportRadius;
  par[7]=TkDBc::Head->P1NSupportSkinThickness/2.;
  par[8]=0;
  par[9]=TkDBc::Head->P1NSupportRadius;
  VZERO(nrm,9*sizeof(nrm[0][0])/4);
  nrm[0][0] = nrm[1][1] =cos(22.5/180.*pi);
  nrm[0][1] = sin(22.5/180.*pi);
  nrm[1][0] = -1*sin(22.5/180.*pi);
  nrm[2][2] = 1;  
  
  mvol->add(new AMSgvolume("P1NS_HoneySkin", _nrot++, name,
			      "PGON", par, 10, coo, nrm, "ONLY", 1, 1, 1));

  coo[2]=TkDBc::Head->P1NSupportCoo[2]-
    TkDBc::Head->P1NSupportThickness/2.-
    TkDBc::Head->P1NSupportSkinThickness/2.;

  mvol->add(new AMSgvolume("P1NS_HoneySkin", _nrot++, name,
			   "PGON", par, 10, coo, nrm, "ONLY", 1, 2, 1));



  sprintf(name,"P1NC");
  coo[0]=TkDBc::Head->P1NSupportCoo[0];
  coo[1]=TkDBc::Head->P1NSupportCoo[1];
  coo[2]=TkDBc::Head->P1NSupportCoo[2]-
    TkDBc::Head->P1NSupportThickness/2.-
    TkDBc::Head->P1NSupportSkinThickness-
    TkDBc::Head->P1NSCThickness/2.;

  //Al- honeycomb C struct
  par[0]=0;    //inital phi
  par[1]=360;  // phi amplitude
  par[2]=8;    // pgon sides
  par[3]=2;    // n z planes
  par[4]=-TkDBc::Head->P1NSCThickness/2.;   // z1
  par[5]=TkDBc::Head->P1NSCRadius-0.1;
  par[6]=TkDBc::Head->P1NSCRadius;
  par[7]=TkDBc::Head->P1NSCThickness/2.;
  par[8]=TkDBc::Head->P1NSCRadius-0.1;
  par[9]=TkDBc::Head->P1NSCRadius;
  VZERO(nrm,9*sizeof(nrm[0][0])/4);
  nrm[0][0] = nrm[1][1] = nrm[2][2] = 1;  
  nrm[0][0] = nrm[1][1] =cos(22.5/180.*pi);
  nrm[0][1] = sin(22.5/180.*pi);
  nrm[1][0] = -1*sin(22.5/180.*pi);
  nrm[2][2] = 1;  
  
  mvol->add(new AMSgvolume("P1NS_HoneySkin", _nrot++, name,
			      "PGON", par, 10, coo, nrm, "ONLY", 1, 1, 1));
//
//--->MLI on top of Plane-1 support(simplified structure,tube shape):
//
  int gid(1);
  float mlidz1=0.055;//Alum-parts
  float mlidz2=0.0089;//Mylar-parts
  float mlidz3=0.019;//betacloth
  float mlihdz=0.025;//hole teflon
  float mliri=15;//tempor
  float mliro=TkDBc::Head->P1NSupportRadius+9;// "9" to have outer radious covering whole trkL1supp.
  float logoxs=30;
  float logoys=50;
  float logodz1=0.034;//logo's bcloth
  float logodz2=0.025;//logo's teflon (neglect 25mkm Al)
  float logoxp=40;
//mli:              
  coo[0]=TkDBc::Head->P1NSupportCoo[0];
  coo[1]=TkDBc::Head->P1NSupportCoo[1];
  coo[2]=TkDBc::Head->P1NSupportCoo[2]+
         TkDBc::Head->P1NSupportThickness/2.+
         TkDBc::Head->P1NSupportSkinThickness + 0.01 + 0.01 + mlihdz + mlidz1/2;//0.01=secur.gap
  par[0]=mliri;
  par[1]=mliro;
  par[2]=mlidz1/2;
  mvol->add(new AMSgvolume("ALUMLI",0,"MLI1",
			      "TUBE", par, 3, coo, nrm, "ONLY", 1, gid, 1));//Alum
  par[2]=mlidz2/2;
  coo[2]=coo[2]+mlidz1/2+mlidz2/2;
  mvol->add(new AMSgvolume("MYLARMLI",0,"MLI2",
			      "TUBE", par, 3, coo, nrm, "ONLY", 1, gid, 1));//Mylar
  par[2]=mlidz3/2;
  coo[2]=coo[2]+mlidz2/2+mlidz3/2;
  mvol->add(new AMSgvolume("BCLOTHMLI",0,"MLI3",
			      "TUBE", par, 3, coo, nrm, "ONLY", 1, gid, 1));//betacloth
//hole:
  coo[2]=TkDBc::Head->P1NSupportCoo[2]+
         TkDBc::Head->P1NSupportThickness/2.+
         TkDBc::Head->P1NSupportSkinThickness + 0.01+mlihdz/2;
  par[0]=0;
  par[1]=mliri+5.;
  par[2]=mlihdz/2;
  mvol->add(new AMSgvolume("TEFLONMLI",0,"MLIH",
			      "TUBE", par, 3, coo, nrm, "ONLY", 1, gid, 1));
//Logos:
  coo[0]=TkDBc::Head->P1NSupportCoo[0]-logoxp;
  coo[1]=TkDBc::Head->P1NSupportCoo[1];
  coo[2]=TkDBc::Head->P1NSupportCoo[2]+
         TkDBc::Head->P1NSupportThickness/2.+
         TkDBc::Head->P1NSupportSkinThickness + 0.01 + 0.01 + mlihdz+mlidz1+mlidz2+mlidz3+logodz1/2;
  par[0]=logoxs/2;	 
  par[1]=logoys/2;
  par[3]=logodz1/2;
  mvol->add(new AMSgvolume("BCLOTHMLI",0,"LOG1",
			      "BOX", par, 3, coo, nrm, "ONLY", 1, gid, 1));//"-X" logo,bcloth
  coo[0]=TkDBc::Head->P1NSupportCoo[0]+logoxp;
  mvol->add(new AMSgvolume("BCLOTHMLI",0,"LOG2",
			      "BOX", par, 3, coo, nrm, "ONLY", 1, gid, 1));//"+X" logo,bcloth
			      
  par[3]=logodz2/2;
  coo[0]=TkDBc::Head->P1NSupportCoo[0]-logoxp;
  coo[2]=coo[2]+logodz1/2+logodz2/2;
  mvol->add(new AMSgvolume("TEFLONMLI",0,"LOG3",
			      "BOX", par, 3, coo, nrm, "ONLY", 1, gid, 1));//"-X" logo,teflon
  coo[0]=TkDBc::Head->P1NSupportCoo[0]+logoxp;
  mvol->add(new AMSgvolume("TEFLONMLI",0,"LOG4",
			      "BOX", par, 3, coo, nrm, "ONLY", 1, gid, 1));//"+X" logo,teflon
//
}






#else
//==================-----------------============================

#include "typedefs.h"
#include "node.h"
#include "snode.h"
#include "amsdbc.h"
#include "gmat.h"
#include "extC.h"
#include <stdlib.h>
#include "gvolume.h"
#include "amsgobj.h"

#include "commons.h"
#include "tkdbc.h"
extern "C" void mtx_(geant nrm[][3],geant vect[]);
extern "C" void mtx2_(number nrm[][3],geant  xnrm[][3]);
#define MTX mtx_
#define MTX2 mtx2_
namespace amsgeom{
extern void tkgeom02(AMSgvolume &);
extern void tkgeom02d(AMSgvolume &);
extern void magnetgeom(AMSgvolume &);
extern void magnetgeom02(AMSgvolume &);
extern void ext1structure02(AMSgvolume &);
extern void magnetgeom02o(AMSgvolume &);
extern void magnetgeom02Test(AMSgvolume &);
extern void tofgeom02(AMSgvolume &);
extern void antigeom02(AMSgvolume &);
extern void antigeom002(AMSgvolume &);
extern void ext2structure(AMSgvolume &);
#ifdef __G4AMS__
extern void antigeom02g4(AMSgvolume &);
extern void testg4geom(AMSgvolume &);
#endif
extern void richgeom02(AMSgvolume &,float zshift=0);
extern void ecalgeom02(AMSgvolume &);
extern void trdgeom02(AMSgvolume &, float zshift=0);
//extern void srdgeom02(AMSgvolume &);
extern void Put_rad(AMSgvolume *,integer);
extern void Put_pmt(AMSgvolume *,integer);
};
using namespace amsgeom;

void amsgeom::tkgeom02(AMSgvolume & mother){
  TKDBc::read();





  AMSID amsid;
  geant xx[3]={0.9,0.2,0.2};
  geant  xnrm[3][3];
  geant par[6]={0.,0.,0.,0.,0.,0.};
  number nrm[3][3]={1.,0.,0.,0.,1.,0.,0.,0.,1.};
  number inrm[3][3];
  char name[5];
  geant coo[3]={0.,0.,0.};

  integer gid=0;
  integer nrot=501; // Temporary arbitary assignment
  //static AMSgvolume mother(0,0,AMSDBc::ams_name,"BOX",par,3,coo,nrm,"ONLY",
  //                         0,gid);  // temporary a dummy volume
  AMSNode * cur;
  AMSgvolume * dau;
  int i;
  int nhalfL=0; 
  for ( i=0;i<TKDBc::nlay();i++){
    ostrstream ost(name,sizeof(name));
    ost << "STK"<<i+1<<ends;
    coo[0]=TKDBc::xposl(i);
    coo[1]=TKDBc::yposl(i);
    coo[2]=TKDBc::zposl(i);
    nrm[0][0]=TKDBc::nrml(0,0,i);
    nrm[0][1]=TKDBc::nrml(0,1,i); 
    nrm[0][2]=TKDBc::nrml(0,2,i); 
    nrm[1][0]=TKDBc::nrml(1,0,i); 
    nrm[1][1]=TKDBc::nrml(1,1,i); 
    nrm[1][2]=TKDBc::nrml(1,2,i); 
    nrm[2][0]=TKDBc::nrml(2,0,i); 
    nrm[2][1]=TKDBc::nrml(2,1,i); 
    nrm[2][2]=TKDBc::nrml(2,2,i); 
    int ii;
    gid=i+1;
    integer status=1;
    integer rgid;
    if(TKDBc::update())TKDBc::SetLayer(i+1,status,coo,nrm,gid);
    else               TKDBc::GetLayer(i+1,status,coo,nrm,rgid);
#ifdef __AMSDEBUG__
    if(i==2 && AMSgvolume::debug){
      MTX(xnrm,xx);
      int a1,a2;
      for( a1=0;a1<3;a1++)for( a2=0;a2<3;a2++)nrm[a2][a1]=xnrm[a1][a2];
    }
#endif
    int npar=5;
    if(npar==5){
      for ( ii=0;ii<npar;ii++)par[ii]=TKDBc::layd(i,ii);
      dau=(AMSgvolume*)mother.add(new AMSgvolume(
						 "VACUUM",nrot++,name,"CONE",par,npar,coo,nrm, "ONLY",0,gid,1));
      //      cout <<" layer "<<i<<" "<<nrot<<endl;
    }
    else{
      par[0]=TKDBc::layd(i,1);
      par[1]=TKDBc::layd(i,2);
      par[2]=TKDBc::layd(i,0);
      dau=(AMSgvolume*)mother.add(new AMSgvolume(
						 "VACUUM",nrot++,name,"TUBE",par,npar,coo,nrm, "ONLY",0,gid,1));
      //      cout <<" layer "<<i<<" "<<nrot<<endl;
    }
    int j;
    for (j=0;j<TKDBc::nlad(i+1);j++){
      int k;
      AMSgvolume * lad[2];
      for(k=0;k<2;k++){
	ost.clear();
	ost.seekp(0);
	ost << "L" << i+1<<(j+1)*2+k<<ends;
	double pairx=0;
	if(k==0){
	  par[0]=TKDBc::ssize_inactive(i,0)*TKDBc::nhalf(i+1,j+1)/2.;
	  pairx=TKDBc::ssize_inactive(i,0)*
	    (TKDBc::nsen(i+1,j+1)-TKDBc::nhalf(i+1,j+1))/2.;
        }
	else{
	  par[0]=TKDBc::ssize_inactive(i,0)*
	    (TKDBc::nsen(i+1,j+1)-TKDBc::nhalf(i+1,j+1))/2.;
	  pairx=TKDBc::ssize_inactive(i,0)*TKDBc::nhalf(i+1,j+1)/2.;
	}
	par[1]=TKDBc::ssize_inactive(i,1)/2;
	par[2]=TKDBc::silicon_z(i)/2;
	coo[0]=(2*k-1)*(TKDBc::ssize_inactive(i,0)*TKDBc::nsen(i+1,j+1)/2+
			TKDBc::halfldist(i)-par[0]);
	if(pairx==0){
	  //coo[0]+=-(2*k-1)*TKDBc::halfldist(i);
	  //cout <<"  Coo[0] was changed "<<" "<<i<<" "<<coo[0]<<endl;         
	}
	coo[1]=(TKDBc::nlad(i+1)-j)*TKDBc::c2c(i)-
	  (TKDBc::nlad(i+1)+1)*TKDBc::c2c(i)/2.;
	//coo[1]+=(2*k-1)*0.008;
        if(coo[1]<0)coo[1]+=-TKDBc::c2cgap(i)/2;
        else coo[1]+=TKDBc::c2cgap(i)/2;
	coo[2]=TKDBc::zpos(i);
	VZERO(nrm,9*sizeof(nrm[0][0])/4);
	if(k==0){
          nrm[0][0]=1;
          nrm[1][1]=1;
          nrm[2][2]=1;
	}
	else{
          nrm[0][0]=-1;
          nrm[1][1]=-1;
          nrm[2][2]= 1;
	}
#ifdef __AMSDEBUG__
	if(TKDBc::nsen(i+1,j+1)==TKDBc::nhalf(i+1,j+1) ||
	   TKDBc::nhalf(i+1,j+1)==0){
          if(par[0]!=0){
	    cout << i+1<<" "<<j+1<<" "<<coo[0]<<" "<<coo[1]<<" "<<coo[2]<<" "<<nrm[0][0]<<endl;
          }
	}
#endif
	//
	//  Ladder 
	//
        gid=i+1+10*(j+1)+100000;
        integer status=1;
        int rgid;
        if(!par[0]>0){
          status=0;
        }
        //cout <<"Lad "<<i<<" "<<j<<" "<<k<<" "<< coo[0]<<" "<<coo[1]<<" "<<coo[2]<<" "<<par[0]<<" "<<par[1]<<" "<<par[2]<<endl;
        if(TKDBc::update())TKDBc::SetLadder(i,j,k,status,coo,nrm,gid);
        else               TKDBc::GetLadder(i,j,k,status,coo,nrm,rgid);
        if(TKDBc::update()){
	  cout <<"Lad "<<i<<" "<<j<<" "<<k<<" "<< coo[0]<<" "<<coo[1]<<" "<<coo[2]<<" "<<par[0]<<" "<<par[1]<<" "<<par[2]<<endl;
        }
        //        if(i==5){
        //          cout <<"Lad "<<j<<" "<<k<<" "<< coo[0]<<" "<<coo[1]<<" "<<coo[2]<<endl;
        //        }
        if(par[0]>0)lad[k]=(AMSgvolume*)dau->add(new AMSgvolume(
								"NONACTIVE_SILICON",nrot++,name,"BOX",par,3,coo,nrm,"ONLY",1,gid,1));
        else lad[k]=0; 
        if(lad[k]){
	  ost.clear();
	  ost.seekp(0);
	  ost << (k==0?"ELL":"ELR") << i+1<<ends;
	  par[0]=TKDBc::zelec(i,1)/2.;
	  par[1]=TKDBc::c2c(i)/2.;
	  //        par[2]=TKDBc::zelec(i,0)/2;
	  par[2]=(dau->getpar(0)- TKDBc::zelec(i,2))/2;
	  coo[0]=lad[k]->getcoo(0)+(2*k-1)*(lad[k]->getpar(0)+par[0]);
	  coo[1]=(TKDBc::nlad(i+1)-j)*TKDBc::c2c(i)-
	    (TKDBc::nlad(i+1)+1)*TKDBc::c2c(i)/2.;
       if(coo[1]<0)coo[1]+=-TKDBc::c2cgap(i)/2;
        else coo[1]+=TKDBc::c2cgap(i)/2;

	  coo[2]=TKDBc::zelec(i,2)+par[2];
	  VZERO(nrm,9*sizeof(nrm[0][0])/4);
	  nrm[0][0]=1;
	  nrm[1][1]=1;
	  nrm[2][2]=1;
	  gid=i+1+10*(j+1);
	  cur=dau->add(new AMSgvolume(
				      "ELECTRONICS",nrot++,name,"BOX",par,3,coo,nrm,"ONLY",1,gid,1));
        }



      }

    
      // Now Sensors

      for ( k=0;k<TKDBc::nhalf(i+1,j+1);k++){
	ost.clear();
	ost.seekp(0);
	ost << "S" << i+1<<(j+1)*2<<ends;
	par[0]=TKDBc::ssize_active(i,0)/2;
	par[1]=TKDBc::ssize_active(i,1)/2;
	par[2]=TKDBc::silicon_z(i)/2;
	coo[0]=-TKDBc::ssize_inactive(i,0)/2.+
	  (2*k+2-TKDBc::nhalf(i+1,j+1))*TKDBc::ssize_inactive(i,0)/2.;
	coo[1]=0;
	coo[2]=0;
	VZERO(nrm,9*sizeof(nrm[0][0])/4);
	nrm[0][0]=1;
	nrm[1][1]=1;
	nrm[2][2]=1;
#ifdef __AMSDEBUG__
	if(i==2 && AMSgvolume::debug){
          MTX(xnrm,xx);
          int a1,a2;
          for( a1=0;a1<3;a1++)for( a2=0;a2<3;a2++)nrm[a2][a1]=xnrm[a1][a2];
	}
#endif
	gid=i+1+10*(j+1)+1000*(k+1);
	if(TKDBc::activeladdshuttle(i+1,j+1,0)){  
          if(k==0)nhalfL++;
          integer status=1;
          if(TKDBc::update())TKDBc::SetSensor(i,j,k,status,coo,nrm,gid);
          else               TKDBc::GetSensor(i,j,k,status,coo,nrm);
          if(status){
	    cur=lad[0]->add(new AMSgvolume(
					   "ACTIVE_SILICON",nrot++,name,"BOX",par,3,coo,nrm,"ONLY",1,gid,1));
	    //          cout <<"add volume "<<name<<" "<<gid<<endl;
	    //          cout <<" to "<<*(lad[0]);
          }
	}
      }

      for ( k=TKDBc::nhalf(i+1,j+1);k<TKDBc::nsen(i+1,j+1);k++){
	ost.clear();
        ost.seekp(0);
        ost << "S" << i+1<<(j+1)*2+1<<ends;
        par[0]=TKDBc::ssize_active(i,0)/2;
        par[1]=TKDBc::ssize_active(i,1)/2;
        par[2]=TKDBc::silicon_z(i)/2;
        coo[0]=-TKDBc::ssize_inactive(i,0)/2.+
	  (2*(k-TKDBc::nhalf(i+1,j+1))+2+
	   TKDBc::nhalf(i+1,j+1)-TKDBc::nsen(i+1,j+1))*
	  TKDBc::ssize_inactive(i,0)/2.;
        coo[1]=0;
        coo[2]=0;
        VZERO(nrm,9*sizeof(nrm[0][0])/4);
        nrm[0][0]=1;
        nrm[1][1]=1;
        nrm[2][2]=1;
#ifdef __AMSDEBUG__
	if(i==2 && AMSgvolume::debug){
          MTX(xnrm,xx);
          int a1,a2;
          for( a1=0;a1<3;a1++)for( a2=0;a2<3;a2++)nrm[a2][a1]=xnrm[a1][a2];
	}
#endif
        gid=i+1+10*(j+1)+1000*(k+1);
        if(TKDBc::activeladdshuttle(i+1,j+1,1)){  
	  if(k==TKDBc::nhalf(i+1,j+1))nhalfL++;
	  integer status=1;
	  if(TKDBc::update())TKDBc::SetSensor(i,j,k,status,coo,nrm,gid);
	  else               TKDBc::GetSensor(i,j,k,status,coo,nrm);
	  if(status){
	    cur=lad[1]->add(new AMSgvolume(
					   "ACTIVE_SILICON",nrot++,name,"BOX",par,3,coo,nrm,"ONLY",1,gid,1));
	    //          cout <<"add volume "<<name<<" "<<gid<<endl;
	    //          cout <<" to "<<*(lad[1]);
	  }
        }
      }


    }
    // Now Support foam
 
    for ( j=0;j<TKDBc::nlad(i+1);j++){
      ost.clear();
      ost.seekp(0);
      ost << "FOA" << i+1<<ends;
      par[0]=TKDBc::ssize_inactive(i,0)*TKDBc::nsen(i+1,j+1)/2.;
      par[1]=TKDBc::c2c(i)/2;
      par[2]=(TKDBc::support_foam_w(i))/2;    
      coo[0]=0;
      coo[1]=(TKDBc::nlad(i+1)-j)*TKDBc::c2c(i)-(TKDBc::nlad(i+1)+1)*TKDBc::c2c(i)/2.;
//       if(coo[1]<0)coo[1]+=-TKDBc::c2cgap(i)/2;
//        else coo[1]+=TKDBc::c2cgap(i)/2;

      coo[2]=TKDBc::zpos(i)-TKDBc::silicon_z(i)/2.-par[2]-TKDBc::support_foam_tol(i);
      VZERO(nrm,9*sizeof(nrm[0][0])/4);
      nrm[0][0]=1;
      nrm[1][1]=1;
      nrm[2][2]=1;
      gid=i+1+10*(j+1);
      cur=dau->add(new AMSgvolume(
				  "Tr_Foam",nrot++,name,"BOX",par,3,coo,nrm,"ONLY",1,gid,1));

    }

    // Now Honecomb Planes

    ost.clear();
    ost.seekp(0);
    ost << "PLA" << i+1<<ends;
    par[0]=0;
    par[1]=TKDBc::support_hc_r(i);
    par[2]=TKDBc::support_hc_w(i)/2;
    coo[0]=0;
    coo[1]=0;
    coo[2]=TKDBc::zpos(i)-TKDBc::silicon_z(i)/2.+TKDBc::support_hc_z(i);
    VZERO(nrm,9*sizeof(nrm[0][0])/4);
    nrm[0][0]=1;
    nrm[1][1]=1;
    nrm[2][2]=1;
    gid=i+1;
    cur=dau->add(new AMSgvolume(
				"Tr_Honeycomb",nrot++,name,"TUBE",par,3,coo,nrm,"ONLY",1,gid,1));



  }
  cout <<"<---- TKGeom-I-"<<nhalfL<<" Active halfladders initialized"<<endl<<endl;

}

#endif
