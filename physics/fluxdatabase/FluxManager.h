//  $Id$
//////////////////////////////////////////////////////////////////////////
///
///\file  FluxManager: Tool for Manage Flux
///\data  2014/11/01 Q.Yan Created
///
//////////////////////////////////////////////////////////////////////////
#ifndef _FluxManager_
#define _FluxManager_
#include "TGraphErrors.h"

class FluxManager{
  public:
    FluxManager();
///---Proton Mass
    static const double PMass=0.938;
/// Convert bewteen Rigidity(GV) and Kinetic Energy(GeV/n)
 /*!
   * @param[in] ZVA=Charge/Mass: Proton ZVA=1, He ZVA=1./2.,...
   * @param[in] Energy: raw energy
   * @param[in] opt: 0 Rig->Ekin/n, 1 Ekin/n->Rig
   * @return    conveted energy
 */
    static double ConvertER(double ZVA,double Energy,int opt=1);
/// Convert TGraphErrors Flux bewteen Rigidity(GV) and Kinetic Energy(GeV/n)
 /*!
  * @param[in] ZVA=Charge/Mass: Proton ZVA=1, He ZVA=1./2., ...
  * @param[in] gr: Raw TGraphErrors Flux in unit of Rigidity or Kinetic Energy
  * @param[in] opt: 0 Rig->Ekin/n, 1 Ekin/n->Rig
  * return     conveted TGraphErrors Flux
*/
    static TGraphErrors *ConvertERTGraphErrors(double ZVA,TGraphErrors *gr,int opt=1);
}; 

//========================================================
FluxManager::FluxManager(){
}

//========================================================
double FluxManager::ConvertER(double ZVA,double Energy,int opt){
   double EnergyN=Energy;
   if(opt%10==0)EnergyN =sqrt(pow(ZVA*Energy,2)+PMass*PMass)-PMass;//Rig->Ekin/n
   else         EnergyN=(1./ZVA)*sqrt(pow(Energy+PMass,2)-PMass*PMass);//Ekin/n->Rig
   return EnergyN;
}

//========================================================
TGraphErrors * FluxManager::ConvertERTGraphErrors(double ZVA,TGraphErrors *gr,int opt){
   TGraphErrors* grn=(TGraphErrors* )gr->Clone();
    for(int ip=0;ip<grn->GetN();ip++){
     double xv=0,yv=0,ey=0;
     grn->GetPoint(ip,xv,yv);
     ey=grn->GetErrorY(ip);
//---
     double xvn=0,yvn=0,eyn=0;
     xvn=ConvertER(ZVA,xv,opt);
     double scale=1;
     if(opt%10==0)scale=(xvn+PMass)/(ZVA*ZVA*xv);//dR/dKE
     else         scale=ZVA*ZVA*xvn/(xv+PMass);//(Z/A)^2*R/(KE+MP) dKE/dR
     yvn=scale*yv;
     eyn=scale*ey;
//----
     grn->SetPoint(ip,xvn,yvn);
     grn->SetPointError(ip,0,eyn);
   }
   return grn;
}

#endif 
