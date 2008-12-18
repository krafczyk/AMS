#ifdef _PGTRACK_

#include "gvolume.h"  // for AMSgvolume

#include "TkDBc.h"
#include "TkLadder.h"
#include "TkPlane.h"
#include "TkCoo.h"

#include <strstream>
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

//! To account for the envelop assimmetry of external planes
static  double dz[5]={-4.,0.,0.,0.,+4.};

void        BuildHoneycomb    (AMSgvolume *mvol, int plane);
AMSgvolume *BuildPlaneEnvelop (AMSgvolume *mvol, int plane);
AMSgvolume *BuildLadder       (AMSgvolume *mvol, int tkid);
void        BuildSensor       (AMSgvolume *mvol, int tkid, int s);
void        BuildHybrid       (AMSgvolume *mvol, int tkid);
void        BuildSupport      (AMSgvolume *mvol, int tkid);


void amsgeom::tkgeom02(AMSgvolume &mother)
{
// Build tracker geometry

  // Temporary arbitary assignment
  _nrot = 501;

  int nladder = 0;

  // Loop on planes
  for (int plane=1; plane<=trconst::nplanes;plane++){

    // Build a plane geometry (made of VACUUM)
    AMSgvolume *vplane = BuildPlaneEnvelop(&mother, plane);
    // Build plane support
    BuildHoneycomb(vplane, plane);
    
    // Loop on Si layer attched to the plane
    int maxp_lay=1;
    int minp_lay=0;
    if(plane==1)      {minp_lay=1;maxp_lay=2;}
    else if(plane==8) {minp_lay=0;maxp_lay=1;}
    else              {minp_lay=0;maxp_lay=2;}

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
  std::ostrstream ost(name,sizeof(name));
  ost << "STK" << plane << std::ends;

  TkPlane *pl = TkDBc::Head->GetPlane(plane);

  geant coo[3];
  coo[0] = pl->pos[0]+pl->GetPosA()[0];
  coo[1] = pl->pos[1]+pl->GetPosA()[1];
  coo[2] = pl->pos[2]+pl->GetPosA()[2]+dz[plane-1];

  geant par[3];
  par[0] = 0;
  par[1] = TkDBc::Head->_plane_d1[plane-1]; //container radius
  par[2] = TkDBc::Head->_plane_d2[plane-1]; //container half thickness

  AMSRotMat lrm = pl->GetRotMatA();
  lrm * pl->GetRotMat();

  number nrm[3][3];
  for (int ii = 0; ii < 9; ii++) nrm[ii/3][ii%3] = lrm.GetEl(ii/3,ii%3);

  return (AMSgvolume*)mvol
    ->add(new AMSgvolume("VACUUM", _nrot++, name, "TUBE",
			 par, 3, coo, nrm, "ONLY", 0, plane, 1));
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
        -TkDBc::Head->_SensorPitchK+TkDBc::Head->_ssize_active[0];
  double hwid = TkDBc::Head->_ssize_active[1]/2;

  AMSRotMat rot = lad->GetRotMatA()*lad->GetRotMat();
  AMSPoint  pos = lad->GetPosA()+lad->GetPos();
  AMSPoint  loc(hlen, hwid, 0);
  AMSPoint  oo = rot*loc+pos;

  // Get coordinate w.r.t. the mother layer
  geant coo[3];
  coo[0] = oo.x();
  coo[1] = oo.y();
  coo[2] = oo.z();
  if(layer==1) coo[2] -= dz[0];
  if(layer==8) coo[2] -= dz[4];

  AMSRotMat lrm = lad->GetRotMatA();
  lrm * lad->GetRotMat();

  number nrm[3][3];
  for (int ii = 0; ii < 9; ii++) nrm[ii/3][ii%3] = lrm.GetEl(ii/3,ii%3);

  int gid =abs(tkid)/tkid* (abs(tkid)+1000);
  AMSgvolume* ladd=(AMSgvolume*)mvol
    ->add(new AMSgvolume("NONACTIVE_SILICON", _nrot++, name, "BOX",
			 par, 3, coo, nrm, "ONLY", 1, gid, 1));

  //  printf("Ladder name %s\n",name);
  
  // Build sensors
  for (int sensor = 0; sensor < lad->_nsensors; sensor++){
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
    //printf("Sensor name %s  %+03d   %+9d \n",nameS,tkid,gid);
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
  int sign  = (tkid > 0) ? 1 : -1;

  char name[5];
  std::ostrstream ost(name,sizeof(name));
  ost << ((tkid < 0) ? "ELL" : "ELR") << layer << std::ends;

  geant par[3];
  par[0] = TkDBc::Head->_zelec[0]/2;
  par[1] = TkDBc::Head->_zelec[1]/2;
  par[2] = TkDBc::Head->_zelec[2]/2;

  double hlen = TkCoo::GetLadderLength(tkid)/2
    -TkDBc::Head->_SensorPitchK+TkDBc::Head->_ssize_active[0];
  double hwid = TkDBc::Head->_ssize_active[1]/2;

  AMSRotMat rot = lad->GetRotMatA()*lad->GetRotMat();
  AMSPoint  pos = lad->GetPosA()+lad->GetPos();
  AMSPoint  loc(hlen, hwid, 0);
  AMSPoint  oo = rot*loc+pos;

  // Get coordinate w.r.t. the mother layer
  geant coo[3];
  coo[0] = oo.x() +sign*(TkCoo::GetLadderLength(tkid)/2+par[0]);
  coo[1] = oo.y();
  coo[2] = (abs(oo.z())/oo.z())*(abs(oo.z())+par[2]); 
  if(layer==1) coo[2] -= dz[0];
  if(layer==8) coo[2] -= dz[4];
  
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
  
  int nsen = lad->_nsensors;
  double signp = (layer%2 == 1) ? -1 : 1;

  geant par[3];
  par[0] = TkDBc::Head->_ssize_inactive[0]*nsen/2;
  par[1] = 7.3/2;//TkDBc::Head->_ladder_Ypitch/2;
  par[2] = sup_foam_w/2;    
  

  double hlen = TkCoo::GetLadderLength(tkid)/2
    -TkDBc::Head->_SensorPitchK+TkDBc::Head->_ssize_active[0];
  double hwid = TkDBc::Head->_ssize_active[1]/2;
  
  AMSRotMat rot = lad->GetRotMatA()*lad->GetRotMat();
  AMSPoint  pos = lad->GetPosA()+lad->GetPos();
  AMSPoint  loc(hlen, hwid, 0);
  AMSPoint  oo = rot*loc+pos;

// Get coordinate w.r.t. the mother layer
  geant coo[3];
  coo[0] = oo.x();
  coo[1] = oo.y();
  coo[2] = (abs(oo.z())/oo.z())*(abs(oo.z())-TkDBc::Head->_silicon_z/2-par[2]-sup_foam_tol);
  if(layer==1) coo[2] -= dz[0];
  if(layer==8) coo[2] -= dz[4];
	     
	     
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

  char name[5];
  std::ostrstream ost(name,sizeof(name));
  ost << "PLA" << plane << std::ends;

  geant par[3];
  par[0] = 0;
  par[1] = TkDBc::Head->_sup_hc_r[plane-1];
  par[2] = TkDBc::Head->_sup_hc_w[plane-1]/2.;

  geant coo[3];
  coo[0] = coo[1] = 0;
  coo[2] = -dz[plane-1];


  number nrm[3][3];
  VZERO(nrm,9*sizeof(nrm[0][0])/4);
  nrm[0][0] = nrm[1][1] = nrm[2][2] = 1;

  mvol->add(new AMSgvolume("Tr_Honeycomb", _nrot++, name,
			   "TUBE", par, 3, coo, nrm, "ONLY", 1, plane, 1));
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
