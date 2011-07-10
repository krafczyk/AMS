/// $Id: TkSens.C,v 1.15 2011/07/10 10:49:14 pzuccon Exp $ 

//////////////////////////////////////////////////////////////////////////
///
///\file  TkSens.C
///\brief Source file of TkSens class
///
///\date  2008/03/18 PZ  First version
///\date  2008/04/02 SH  Some bugs are fixed
///\date  2008/04/18 SH  Updated for alignment study
///\date  2008/04/21 AO  Ladder local coordinate and bug fixing
///$Date: 2011/07/10 10:49:14 $
///
/// $Revision: 1.15 $
///
//////////////////////////////////////////////////////////////////////////

#include "TkDBc.h"

#include "TkSens.h"
#include "TkCoo.h"
#include "tkdcards.h"


//--------------------------------------------------
TkSens::TkSens(bool MC) {
  _isMC=MC;
  Clear();
}


//--------------------------------------------------
TkSens::TkSens(AMSPoint& GCoo,bool MC){
  _isMC=MC;
  Clear();
  GlobalCoo = GCoo;
  for (int ii=0; ii<3; ii++) GlobalDir[ii] = 0;
  Recalc();
}


//--------------------------------------------------
TkSens::TkSens(AMSPoint& GCoo, AMSDir& GDir,bool MC){
  _isMC=MC;
  Clear();
  GlobalCoo = GCoo;
  GlobalDir = GDir;
  Recalc();
}


//--------------------------------------------------
TkSens::TkSens(int tkid, AMSPoint& GCoo,bool MC){
  _isMC=MC;
  Clear();
  GlobalCoo = GCoo;
  for (int ii=0; ii<3; ii++) GlobalDir[ii] = 0;
  lad_tkid  = tkid;
  Recalc();
}


//--------------------------------------------------
TkSens::TkSens(int tkid, AMSPoint& GCoo, AMSDir& GDir,bool MC){
  _isMC=MC;
  Clear();
  GlobalCoo = GCoo;
  GlobalDir = GDir;
  lad_tkid  = tkid; 
  Recalc();
}


//--------------------------------------------------
void TkSens::Clear(){
  lad_tkid = 0;
  sens     = -1;
  mult     = -1;
  for (int ii=0; ii<3; ii++) {
    SensCoo[ii] = -1000;
    SensDir[ii] =     0; 
  }   
  LaddCoo  = SensCoo;
  LaddDir  = SensDir;
  ReadChanX     = -1;
  close_chanX   = -1;
  ReadChanY     = -1;
  ImpactPointX  = -10;
  ImpactPointY  = -10;
  ImpactAngleXZ = -10;
  ImpactAngleYZ = -10;
}


//--------------------------------------------------
bool TkSens::IsInsideLadder(int tkid){
  TkLadder* lad=0;
  lad=TkDBc::Head->FindTkId(lad_tkid);
  if (!lad) {return  false;}
  return IsInsideLadder(lad);
}


//--------------------------------------------------
void TkSens::Recalc(){

  int lay = -1;
  if( lad_tkid==0) {
    lay=GetLayer();
    if(lay)
      lad_tkid=FindLadder(lay);
    if(!IsInsideLadder(lad_tkid)) return;
  }
  
  // makes the hard work!
  sens = GetSens();

  ReadChanY=GetStripFromLocalCooS(SensCoo[1]);
  number cooY1,cooY2;  
  cooY1=TkCoo::GetLocalCooS(ReadChanY);
  if((LaddCoo[1]-cooY1)>=0)
    cooY2=TkCoo::GetLocalCooS(ReadChanY+1);
  else
    cooY2=TkCoo::GetLocalCooS(ReadChanY-1);
  ImpactPointY=(LaddCoo[1]-cooY1)/abs(cooY1-cooY2);
  
  mult = -1;
  int layer=abs(lad_tkid)/100;
  number cooX1,cooX2;  
  TkLadder* ll = TkDBc::Head->FindTkId(lad_tkid);
  if(!ll){
    printf("TkSens::Recalc(): ERROR cant find ladder %d into the database\n",lad_tkid);
    return ;
  } 
  if(ll->IsK7()){
    ReadChanX=GetStripFromLocalCooK7(SensCoo[0],sens);
    mult = sens*TkDBc::Head->_NReadStripK7/TkDBc::Head->_NReadoutChanK;
    int test = (mult*TkDBc::Head->_NReadoutChanK+ReadChanX)/TkDBc::Head->_NReadStripK7;
    if (test!=sens) mult = mult+1;
    cooX1=TkCoo::GetLocalCooK7(640+ReadChanX,mult);
    if((LaddCoo[0]-cooX1)>=0) 
      cooX2=TkCoo::GetLocalCooK7(640+ReadChanX+1,mult);
    else
      cooX2=TkCoo::GetLocalCooK7(640+ReadChanX-1,mult);
  }    
  else{
    ReadChanX=GetStripFromLocalCooK5(SensCoo[0],sens);
    mult = sens*TkDBc::Head->_NReadStripK5/TkDBc::Head->_NReadoutChanK;
    cooX1=TkCoo::GetLocalCooK5(640+ReadChanX,mult);
    if((LaddCoo[0]-cooX1)>=0)
      cooX2=TkCoo::GetLocalCooK5(640+ReadChanX+1,mult);
    else
      cooX2=TkCoo::GetLocalCooK5(640+ReadChanX-1,mult);
  }
  ImpactPointX=(LaddCoo[0]-cooX1)/abs(cooX1-cooX2);

  if(SensDir.z()!=0){
    ImpactAngleXZ = atan(SensDir.x()/SensDir.z());
    ImpactAngleYZ = atan(SensDir.y()/SensDir.z()); 
  }
  else{
    ImpactAngleXZ = 0;
    ImpactAngleYZ = 0; 
  }
  // caveat:
  // - always defined (also in regions of no definition 640 and -0.2)
  // - sensor/multiplicity (880)

  return;
}


//--------------------------------------------------
int TkSens::GetSens(){
  if(lad_tkid==0) return -1;

  //Get the Ladder Pointer
  TkLadder* lad=TkDBc::Head->FindTkId(lad_tkid);
  if(!lad) return -1;

  //Get The Plane Pointer
  TkPlane * pp=lad->GetPlane();

  //Alignment corrected Plane postion
  AMSPoint PPosG=pp->GetPosA()+pp->GetPos();
  if(IsMC()) PPosG=pp->GetPosT()+pp->GetPos();


  //Alignment corrected Plane Rotation matrix
  AMSRotMat PRotG=pp->GetRotMat().Invert()*pp->GetRotMatA().Invert();
  if(IsMC()) PRotG=pp->GetRotMat().Invert()*pp->GetRotMatT().Invert();

  //Alignment corrected Ladder postion
  AMSPoint PosG=lad->GetPosA()+lad->GetPos();
  if(IsMC()) PosG=lad->GetPosT()+lad->GetPos();

  //Alignment corrected Ladder Rotation matrix
  AMSRotMat RotG=lad->GetRotMat().Invert()*lad->GetRotMatA().Invert();
  if(IsMC()) RotG=lad->GetRotMat().Invert()*lad->GetRotMatT().Invert();

  //Convolute with the Plane pos in the space
  AMSPoint oo = PRotG*(GlobalCoo-PPosG);

  //Get the local coo on the Ladder
  SensCoo = RotG*(oo-PosG);
  LaddCoo = SensCoo;

  /*
  cout << "GLOBAL " << GlobalCoo.x() << " " << GlobalCoo.y() << " " << GlobalCoo.z() << endl;
  cout << "LADDER " << LaddCoo.x()   << " " << LaddCoo.y()   << " " << LaddCoo.z()   << endl;
  cout << "Plane Rotation" << endl;
  for (int irow=0; irow<3; irow++) 
    for (int icol=0; icol<3; icol++) 
       cout << pp->GetRotMat().GetEl(irow,icol) << " ";
  cout << endl;
  cout << "Plane Inverse Rotation" << endl;
  for (int irow=0; irow<3; irow++)
    for (int icol=0; icol<3; icol++)
       cout << PRotG.GetEl(irow,icol) << " ";
  cout << endl;
  cout << "Ladder Rotation" << endl;
  for (int irow=0; irow<3; irow++)
    for (int icol=0; icol<3; icol++)
       cout << lad->GetRotMat().GetEl(irow,icol) << " ";
  cout << endl;
  cout << "Ladder Inverse Rotation" << endl;
  for (int irow=0; irow<3; irow++)
    for (int icol=0; icol<3; icol++)
       cout << RotG.GetEl(irow,icol) << " ";
  cout << endl;
  cout << "GLODIR " << GlobalDir.x() << " " << GlobalDir.y() << " " << GlobalDir.z() << endl;
  cout << "LADDIR " << LaddDir.x() << " " << LaddDir.y() << " " << LaddDir.z() << endl;
  */

  SensDir = RotG*(PRotG*GlobalDir);
  LaddDir = SensDir;

  //Offset of the first strip position
  double Ax= (TkDBc::Head->_ssize_inactive[0]-
	      TkDBc::Head->_ssize_active[0])/2;

  //Sensor number
  int nsens = (int)(abs(SensCoo[0]+Ax)/TkDBc::Head->_SensorPitchK);

  AMSPoint gcoo = GlobalCoo;


  //Sensor alignment correction
  if(
     (!IsMC())  // Sensor disalignement not in simulation
     && (TRCLFFKEY.UseSensorAlign==1)  //explicity enabled in datacard
     && (0 <= nsens && nsens < trconst::maxsen) // within the sensor range
     ) {
    gcoo[0] += lad->_sensx[nsens];
    gcoo[1] += lad->_sensy[nsens];

    //Get the local coo on the Ladder (again)
    AMSPoint oo2 = PRotG*(gcoo-PPosG);
    SensCoo = RotG*(oo2-PosG);
    LaddCoo = SensCoo;
  }

  //Sensor coordinate
  SensCoo[0] -= nsens*TkDBc::Head->_SensorPitchK;

  return nsens;
}

//--------------------------------------------------
int  TkSens::GetLayerJ(){
  return TkDBc::Head->GetJFromLayer(GetLayer());
}

int TkSens::GetLayer() {

///\param z  Z coordinate of global position (cm)
///
/// For the Z-coordinate, +/- 1 cm of the layer position is accepted
/// For the X,Y-coordinates, inactive area is also accepted
  
  // Estimate layer
  double z=GlobalCoo[2];
  double toll=1.; // 1. cm 
  int layer;
  if      (  z < TkDBc::Head->GetZlayer(1)+toll  &&   
             z > TkDBc::Head->GetZlayer(1)-toll ) layer = 1; 
  else if (  z < TkDBc::Head->GetZlayer(2)+toll  &&   
             z > TkDBc::Head->GetZlayer(2)-toll ) layer = 2; 
  else if (  z < TkDBc::Head->GetZlayer(3)+toll  &&   
             z > TkDBc::Head->GetZlayer(3)-toll ) layer = 3; 
  else if (  z < TkDBc::Head->GetZlayer(4)+toll  &&   
             z > TkDBc::Head->GetZlayer(4)-toll ) layer = 4; 
  else if (  z < TkDBc::Head->GetZlayer(5)+toll  &&   
             z > TkDBc::Head->GetZlayer(5)-toll ) layer = 5; 
  else if (  z < TkDBc::Head->GetZlayer(6)+toll  &&   
             z > TkDBc::Head->GetZlayer(6)-toll ) layer = 6; 
  else if (  z < TkDBc::Head->GetZlayer(7)+toll  &&   
             z > TkDBc::Head->GetZlayer(7)-toll ) layer = 7; 
  else if (  z < TkDBc::Head->GetZlayer(8)+toll  &&   
             z > TkDBc::Head->GetZlayer(8)-toll ) layer = 8; 
  else if (  z < TkDBc::Head->GetZlayer(9)+toll  &&   
             z > TkDBc::Head->GetZlayer(9)-toll ) layer = 9; 
  else    
                                                  layer = 0;
 return layer;	
}



//--------------------------------------------------
//! It takes a point in global coo and chek if it is inside the ladder pointed by the tkid
int TkSens::FindLadder(){
  TkLadder* lad=0;
  bool found=0;
  for(int ii=0;ii<TkDBc::Head->GetEntries();ii++){
    lad=TkDBc::Head->GetEntry(ii);
    if (!lad->IsActive()) continue;
    found=IsInsideLadder(lad);
    if(found) break;
  }
  if(!found) return 0;
  else return lad->GetTkId();
  
}


//--------------------------------------------------
//! It takes a point in global coo and chek if it is inside the ladder pointed by the tkid
int TkSens::FindLadder(int lay){
  TkLadder* lad=0;
  bool found=0;

  int nslot = (lay == 9) ? 8 : 15;
  for (int slot = -nslot; slot <= nslot; slot++) {
    if (slot == 0) continue;
    int tkid = (slot > 0) ? lay*100+slot : -(lay*100-slot);
    lad=TkDBc::Head->FindTkId(tkid);
    if (!lad || !lad->IsActive()) continue;
    if (IsInsideLadder(lad)) return tkid;
  }
  return 0;
}


//--------------------------------------------------
bool TkSens::IsInsideLadder(TkLadder* lad){
  // Z check: layer check
  if (GetLayer()!=lad->GetLayer()) return false; // AO Bug fix: otherwise takes the first plane column
  // XY check

  int tkid = lad->GetTkId();
  AMSPoint diff = GlobalCoo-TkCoo::GetLadderCenter(tkid);

  if (abs(diff[0]) < TkCoo::GetLadderLength(tkid)/2 &&
      abs(diff[1]) < TkDBc::Head->_ssize_active[1]/2) return true;
  return false;

  // The following code doesn't work for Layer 8 (aka Layer 1N) for AMS02P
  // i.e. in case the plane is rotated
/*
  double X=GlobalCoo[0];
  double Y=GlobalCoo[1];

  AMSRotMat rotG= lad->GetRotMatA()*lad->GetRotMat();
  if(IsMC()) rotG= lad->GetRotMatT()*lad->GetRotMat();
  
  AMSPoint posG=lad->GetPos()+lad->GetPosA();
  if(IsMC()) posG= lad->GetPos()+lad->GetPosT();

  double vx=lad->rot.GetEl(0,0);
  double vy=lad->rot.GetEl(1,1);

  double Ax= posG[0] - (TkDBc::Head->_ssize_inactive[0]-
		     TkDBc::Head->_ssize_active[0])/2. *vx;


  double Ay= posG[1] - (TkDBc::Head->_ssize_inactive[1]-
		     TkDBc::Head->_ssize_active[1])/2. *vy;

  double Dx= Ax + (lad->_nsensors*
	    (TkDBc::Head->_ssize_inactive[0]+0.004)
	    -0.004)*vx;

  double Dy= Ay + TkDBc::Head->_ssize_inactive[1] * vy;

  int good=1;
  if( Ax >Dx){
    if( X > Ax || X < Dx )       good*= 0;
  }
  else{
    if( X < Ax || X > Dx )       good*= 0;
  }

  if( Ay >Dy){
    if( Y > Ay || Y < Dy )       good*= 0;
  }
  else{
    if( Y < Ay || Y > Dy )       good*= 0;
  }
  
  return (good>0);
*/
}


//--------------------------------------------------
int TkSens::GetStripFromLocalCooS(number Y){
  if( Y<0. ||Y > TkDBc::Head->_ssize_active[1])
    return -1;
  
  else if(Y >= 2.5*TkDBc::Head->_PitchS && 
     Y < TkDBc::Head->_ssize_active[1] - 3.*TkDBc::Head->_PitchS)

    return (int)round((Y-TkDBc::Head->_PitchS)/TkDBc::Head->_PitchS);
  
  else if( Y >= 0. && Y < 1.*TkDBc::Head->_PitchS)
    return 0;
  else if( Y >= 1. && Y < 2.5*TkDBc::Head->_PitchS)
    return 1;
  else if( Y >= TkDBc::Head->_ssize_active[1] - 3.*TkDBc::Head->_PitchS
	   && Y <TkDBc::Head->_ssize_active[1] - 1.5*TkDBc::Head->_PitchS )
    return 638;
  else 
    return 639;
}

//--------------------------------------------------
int TkSens::GetStripFromLocalCooK5(number X,int Sens){
  int chan=-1;
  if( X<0){
    close_chanX=0;
    if(Sens%2==1) close_chanX+=192;
    return -1;
  }
  else if ( X > TkDBc::Head->_ssize_active[0]){
    close_chanX=191;
    if(Sens%2==1) close_chanX+=192;
    return -1;
  }
  else if(X>=0 && X<  189.5*TkDBc::Head->_PitchK5)
    chan=(int)round(X/TkDBc::Head->_PitchK5);
  else if( X>=189.5*TkDBc::Head->_PitchK5 &&
	   X< 191*TkDBc::Head->_PitchK5)
    chan=190;
  else
    chan=191;

  close_chanX=chan;    
  if(Sens%2==1){
    close_chanX+=192;
    chan+=192;
  }
  return chan;
}




//--------------------------------------------------
int TkSens::GetStripFromLocalCooK7(number X,int Sens){

  // PZ FIXME is K7 not K5
   if( X<0){
    close_chanX=0;
    if(Sens%2==1) close_chanX+=192;
    return -1;
  }
  else if ( X > TkDBc::Head->_ssize_active[0]){
    close_chanX=191;
    if(Sens%2==1) close_chanX+=192;
    return -1;
  }
  
  int gstrip=-1;
  float strip=(X/TkDBc::Head->_ImplantPitchK);

  if(strip<95.5){
    int tstrip=(int)trunc(strip);
    int rstrip=(int)round(strip);
    int ff=tstrip%3;
    if(ff==2) gstrip=rstrip*2/3;
    else if(ff==0) gstrip=tstrip*2/3;
    else gstrip= (tstrip+1)*2/3;
    
  }else if (strip>=95.5 && strip<287.5){
    int tstrip=(int)trunc(strip)-96;
    int ff=tstrip%2;
    if(ff==0) gstrip=(tstrip)/2+64;
    else gstrip=(tstrip+1)/2+64;
  }
  else{
    int tstrip=(int)trunc(strip)-288;
    int rstrip=(int)round(strip)-288;
    int ff=tstrip%3;
    if(ff==2) gstrip=rstrip*2/3+160;
    else if(ff==0) gstrip=tstrip*2/3+160;
    else gstrip= (tstrip+1)*2/3+160;
  }

  if(gstrip<0) return -1;
  int rstrip=(Sens*224+gstrip)%384;
  close_chanX=rstrip;
  return rstrip;

}

