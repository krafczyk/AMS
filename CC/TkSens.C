/// $Id: TkSens.C,v 1.24 2014/01/09 15:14:32 pzuccon Exp $ 

//////////////////////////////////////////////////////////////////////////
///
///\file  TkSens.C
///\brief Source file of TkSens class
///
///\date  2008/03/18 PZ  First version
///\date  2008/04/02 SH  Some bugs are fixed
///\date  2008/04/18 SH  Updated for alignment study
///\date  2008/04/21 AO  Ladder local coordinate and bug fixing
///$Date: 2014/01/09 15:14:32 $
///
/// $Revision: 1.24 $
///
//////////////////////////////////////////////////////////////////////////

#include "TkDBc.h"
#include "TrInnerDzDB.h"
#include "TrExtAlignDB.h"

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
  number cooX1,cooX2;  
  TkLadder* ll = TkDBc::Head->FindTkId(lad_tkid);
  if(!ll){
    printf("TkSens::Recalc(): ERROR cant find ladder %d into the database\n",lad_tkid);
    return ;
  } 
  if(ll->IsK7()){
    ReadChanX=GetStripFromLocalCooK7(SensCoo[0],sens,mult);
//     mult = sens*TkDBc::Head->_NReadStripK7/TkDBc::Head->_NReadoutChanK;
     int RR=ReadChanX;
     if(RR<0) RR=close_chanX;
//     int test = (mult*TkDBc::Head->_NReadoutChanK+ReadChanX)/TkDBc::Head->_NReadStripK7;
//     if (test!=sens) mult = mult+1;
    
    cooX1=TkCoo::GetLocalCooK7(640+RR,mult);
    if((LaddCoo[0]-cooX1)>=0) 
      cooX2=TkCoo::GetLocalCooK7(640+RR+1,mult);
    else
      cooX2=TkCoo::GetLocalCooK7(640+RR-1,mult);
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
  int Layer=lad->GetLayer();
  int layJ=lad->GetLayerJ();
  AMSPoint planeA;
  if(Layer>7)
     planeA.setp( TrExtAlignDB::GetDynCorr(layJ,0),
		  TrExtAlignDB::GetDynCorr(layJ,1),
		  TrExtAlignDB::GetDynCorr(layJ,2));
  else
     planeA=pp->GetPosA();
  //Alignment corrected Plane postion
  float dzcorr=0;
  if (Layer<=7) dzcorr= TrInnerDzDB::LDZA[Layer-1];
  AMSPoint LayerZCorrection(0,0,dzcorr);
  
  AMSPoint PPosG=planeA+pp->GetPos()+LayerZCorrection;

  if(IsMC()) PPosG=pp->GetPosT()+pp->GetPos();


  //Alignment corrected Plane Rotation matrix
  AMSRotMat PRotG0=pp->GetRotMat();

  AMSRotMat PRotG1;
  if(Layer>7)
    PRotG1.SetRotAngles(TrExtAlignDB::GetDynCorr(layJ,3),
		      TrExtAlignDB::GetDynCorr(layJ,4),
		      TrExtAlignDB::GetDynCorr(layJ,5));
  else   
    PRotG1=pp->GetRotMatA();

  AMSRotMat PRotG=PRotG0.Invert()*PRotG1.Invert();

  if(IsMC()) {
	  PRotG0=pp->GetRotMat();
	  PRotG1=pp->GetRotMatT();
	  PRotG=PRotG0.Invert()*PRotG1.Invert();
  }


  //Alignment corrected Ladder postion

  AMSPoint PosG=lad->GetPosA()+lad->GetPos();
  if(IsMC()) PosG=lad->GetPosT()+lad->GetPos();

  //Alignment corrected Ladder Rotation matrix
  AMSRotMat RotG0=lad->GetRotMat();
  AMSRotMat RotG1=lad->GetRotMatA();
  AMSRotMat RotG=RotG0.Invert()*RotG1.Invert();
  if(IsMC()) {
	  RotG0=lad->GetRotMat();
	  RotG1=lad->GetRotMatT();
	  RotG=RotG0.Invert()*RotG1.Invert();
  }


  //Convolute with the Plane pos in the space
  AMSPoint oo = PRotG*(GlobalCoo-PPosG);

  //Get the local coo on the Ladder
  SensCoo = RotG*(oo-PosG);

  // PZ Add local Z correction for high precision on z-unaligned cases 
  //Get the global coo from the geometry
  // We donot want this correction for the MC where the Truth rules
  AMSPoint g2;
  if(IsMC()){
    
    g2=GlobalCoo;   //TkCoo::GetGlobalT(lad_tkid,SensCoo[0],SensCoo[1]);

  } else{

    g2=TkCoo::GetGlobalA(lad_tkid,SensCoo[0],SensCoo[1]);
    
    double dz=g2[2]-GlobalCoo[2];
    if(GlobalDir[2]!=0){
      g2[0]=GlobalDir[0]/GlobalDir[2]*dz+GlobalCoo[0];
      g2[1]=GlobalDir[1]/GlobalDir[2]*dz+GlobalCoo[1];
    }else   {
      g2[0]=GlobalCoo[0];
      g2[1]=GlobalCoo[1];
    }
  }
  //Convolute with the Plane pos in the space
  oo = PRotG*(g2-PPosG);

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
    gcoo[2] += lad->_sensz[nsens];

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

AMSPoint TkSens::FindCloseSensorCenter(){
  AMSPoint pret;
  AMSPoint gcoo=GlobalCoo;
  int Layer = -1;
  Layer=GetLayer();
  if (Layer<=0) return pret;
  TkPlane* pp=TkDBc::Head->GetPlane(TkDBc::Head->_plane_layer[Layer-1]);
  int layJ=TkDBc::Head->GetJFromLayer(Layer);
  AMSPoint planeA;
  if(Layer>7)
     planeA.setp( TrExtAlignDB::GetDynCorr(layJ,0),
		  TrExtAlignDB::GetDynCorr(layJ,1),
		  TrExtAlignDB::GetDynCorr(layJ,2));
  else
    planeA=pp->GetPosA();
  //Alignment corrected Plane postion

  float dzcorr=0;
  if (Layer<=7) dzcorr= TrInnerDzDB::LDZA[Layer-1];
  AMSPoint LayerZCorrection(0,0,dzcorr);
  
  AMSPoint PPosG=planeA+pp->GetPos()+LayerZCorrection;
  if(IsMC()) PPosG=pp->GetPosT()+pp->GetPos();

  //Alignment corrected Plane Rotation matrix
  AMSRotMat PRotG0=pp->GetRotMat();
  AMSRotMat PRotG1;
  if(Layer>7)
    PRotG1.SetRotAngles(TrExtAlignDB::GetDynCorr(layJ,3),
			TrExtAlignDB::GetDynCorr(layJ,4),
			TrExtAlignDB::GetDynCorr(layJ,5));
  else   
    PRotG1=pp->GetRotMatA();

  AMSRotMat PRotG=PRotG0.Invert()*PRotG1.Invert();

  if(IsMC()) {
	  PRotG0=pp->GetRotMat();
	  PRotG1=pp->GetRotMatT();
	  PRotG=PRotG0.Invert()*PRotG1.Invert();
  }
  AMSPoint oo = PRotG*(gcoo-PPosG);

  int otkid=0;
  float max=300;
  double hwid = TkDBc::Head->_ssize_inactive[1]/2;
  for(int ii=1;ii<15;ii++){
    int tkid=Layer*100+ii;
    TkLadder* lad=TkDBc::Head->FindTkId(tkid);
    if(lad==0) tkid*=-1;lad=TkDBc::Head->FindTkId(tkid);
    if(lad==0) continue;
    double hlen = TkCoo::GetLadderLength(tkid)/2
      -(TkDBc::Head->_ssize_inactive[0]-TkDBc::Head->_ssize_active[0])/2;

    AMSRotMat rot0 = lad->GetRotMatA();
    AMSRotMat rot = rot0*lad->GetRotMat();

    AMSPoint  pos = lad->GetPos()+lad->GetPosA();
    AMSPoint  loc(hlen, hwid, 0);
    AMSPoint  Center = rot*loc+pos;
    float dist=abs(oo[1]-Center[1]);
    if(dist<max) {otkid=tkid; max=dist;}
  }
  if(!TkDBc::Head->FindTkId(otkid)) return pret;

  int msens=-1;
  vector<float> csM;
  vector<float> csP;
  TkLadder* ladM=TkDBc::Head->FindTkId(abs(otkid)*-1);
  TkLadder* ladP=TkDBc::Head->FindTkId(abs(otkid));
  if(ladM)
    for (int ii=0;ii<ladM->GetNSensors();ii++)
      csM.push_back(ladM->GetPos()[0]+ladM->GetPosA()[0]+TkDBc::Head->_SensorPitchK*ii+TkDBc::Head->_ssize_active[0]/2);
  if(ladP)
    for (int ii=0;ii<ladP->GetNSensors();ii++)
      csP.push_back(ladP->GetPos()[0]+ladP->GetPosA()[0]-TkDBc::Head->_SensorPitchK*ii-TkDBc::Head->_ssize_active[0]/2);

  float distM=9999;
  int sM=-1;
  for (int ii=0;ii<csM.size();ii++){
    float dd=fabs(oo[0]-csM[ii]);
    if( dd< distM) {distM=dd; sM=ii;}
  }

  float distP=9999;
  int sP=-1;
  for (int ii=0;ii<csP.size();ii++){
    float dd=fabs(oo[0]-csP[ii]);
    if( dd< distP) {distP=dd; sP=ii;}
  }
  if(distP<distM){otkid=abs(otkid);msens=sP;}
  else {otkid=abs(otkid)*-1;msens=sM;}
  
  pret=TkCoo::GetGlobalA(otkid,msens*TkDBc::Head->_SensorPitchK+TkDBc::Head->_ssize_active[0]/2,TkDBc::Head->_ssize_inactive[1]/2);
  return pret;
}


//--------------------------------------------------
bool TkSens::IsInsideLadder(TkLadder* lad){
  // Z check: layer check
  if (GetLayer()!=lad->GetLayer()) return false; // AO Bug fix: otherwise takes the first plane column
  // XY check

  int tkid = lad->GetTkId();
  int flag= (_isMC)?1:0;
  AMSPoint diff= GlobalCoo-TkCoo::GetLadderCenter(tkid,flag);


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
int TkSens::GetStripFromLocalCooK7(number X,int Sens,int& mult){

  if( X<0){
    float strip=0;
    int gstrip;
    int rstrip=GetStripFromLocalImplantK7(strip, Sens, gstrip,mult);
    close_chanX=rstrip;
    return -1;
  }
  else if ( X > TkDBc::Head->_ssize_active[0]){
    float strip=384;
    int gstrip;
    int rstrip=GetStripFromLocalImplantK7(strip, Sens, gstrip,mult);
    close_chanX=rstrip;
    return -1;
  }else{
    
    float strip=(X/TkDBc::Head->_ImplantPitchK);
    int gstrip;
    int rstrip=GetStripFromLocalImplantK7(strip, Sens, gstrip,mult);
    close_chanX=rstrip;
    return rstrip;
  }
}

int TkSens::GetStripFromLocalImplantK7(number strip, int Sens, int &gstrip, int& mult){
  
  gstrip=-1;
  
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
  mult=(Sens*224+gstrip)/384;
  return rstrip;
  
}

