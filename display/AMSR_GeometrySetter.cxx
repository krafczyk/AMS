//  $Id: AMSR_GeometrySetter.cxx,v 1.4 2003/07/10 13:56:57 choutko Exp $

//////////////////////////////////////////////////////////////////////////
//                                                                      //
// AMSR_GeometrySetter                                                    //
//                                                                      //
// Utility class to set various options of  AMS geometry                //
//                                                                      //
//////////////////////////////////////////////////////////////////////////

#include <TNode.h>
#include <TDialogCanvas.h>
#include <iostream.h>
#include "AMSR_GeometrySetter.h"
#include "TList.h"
#include "TCONE.h"



//_____________________________________________________________________________
AMSR_GeometrySetter::AMSR_GeometrySetter()
{
   // Default constructor of AMSR_GeometrySetter
   //

   m_Geometry = 0;
}

//  { "TRD9210",              kDrawImmediateSonsOnly, NULL },
AMSR_GeoToggle AMSR_GeometrySetter::m_Toggle[] = {
  { "TRD11632",              kDrawNone, NULL },
 { "TRD9210",              kDrawNone, NULL },
  { "AMSG1",              kDrawSonsOnly, NULL },
  { "FMOT1",              kDrawSonsOnly, NULL },
  { "CRB11231",                kDrawNone, NULL },
  { "CRB21241",                kDrawNone, NULL },
  { "CRB31251",                kDrawNone, NULL },
  { "CRB41261",                kDrawNone, NULL },
  { "CRB41261",                kDrawNone, NULL },
  { "CRS11271",                kDrawNone, NULL },
  { "CRS21281",                kDrawNone, NULL },
  { "MMOT11",               kDrawNone, NULL },
  { "MSBO1301",             kDrawNone, NULL },
  { "MSBO1312",             kDrawNone, NULL },
  { "MSBO1323",             kDrawNone, NULL },
  { "MSBO1334",             kDrawNone, NULL },
  { "MSBO1345",             kDrawNone, NULL },
  { "MSBO1356",             kDrawNone, NULL },
  { "MSBO1367",             kDrawNone, NULL },
  { "MSBO1378",             kDrawNone, NULL },
  { "MSFR1291",             kDrawNone, NULL },
  { "RA1B1161",             kDrawNone, NULL },
  { "RA1T1172",             kDrawNone, NULL },
  { "RA2B1181",             kDrawNone, NULL },
  { "RA2M1192",             kDrawNone, NULL },
  { "RA2T1203",             kDrawNone, NULL },
  { "RA3T1211",             kDrawNone, NULL },
  { "RA4T1221",             kDrawNone, NULL },
  { "ASTB100",              kDrawNone,     NULL },
  { "PLA2632",              kDrawNone,     NULL },
  { "PLA7637",              kDrawNone,     NULL },
  { "AMOT200",               kDrawNone, NULL },
  { "RICH1",              kDrawImmediateSonsOnly, NULL },
  { "ECMO1",              kDrawImmediateSonsOnly, NULL },
  { "ECEB31",              kDrawNone,     NULL },
  { "ECEB42",              kDrawNone,     NULL },
  { "ECEB53",              kDrawNone,     NULL },
  { "ECEB64",              kDrawNone,     NULL },
  { "ECHN11",              kDrawNone,     NULL },
  { "ECHN22",              kDrawNone,     NULL },
  { "STK11",              kDrawImmediateSonsOnly, NULL },
  { "STK22",              kDrawImmediateSonsOnly, NULL },
  { "STK33",              kDrawImmediateSonsOnly, NULL },
  { "STK44",              kDrawImmediateSonsOnly, NULL },
  { "STK55",              kDrawImmediateSonsOnly, NULL },
  { "STK66",              kDrawImmediateSonsOnly, NULL },
  { "STK77",              kDrawImmediateSonsOnly, NULL },
  { "STK88",              kDrawImmediateSonsOnly, NULL },
  { "US1B1381",              kDrawNone,     NULL },
  { "US1B1392",              kDrawNone,     NULL },
  { "US1B1403",              kDrawNone,     NULL },
  { "US1B1414",              kDrawNone,     NULL },
  { "US2B1421",              kDrawNone,     NULL },
  { "US2B1432",              kDrawNone,     NULL },
  { "US2B1443",              kDrawNone,     NULL },
  { "US2B1454",              kDrawNone,     NULL },
  { "US4B1461",              kDrawNone,     NULL },
  { "US4B1472",              kDrawNone,     NULL },
  { "US4B1483",              kDrawNone,     NULL },
  { "US4B1494",              kDrawNone,     NULL },
  { "USLF1",              kDrawNone,     NULL },
  { "USS23",              kDrawNone,     NULL },
  { "USS32",              kDrawNone,     NULL },
  { 0,                    kDrawNone,     NULL }
};

//_____________________________________________________________________________
AMSR_GeometrySetter::AMSR_GeometrySetter(TGeometry * geo)
{
   // Constructor of AMSR_GeometrySetter
   //
   m_Geometry = geo;
   TNode * node;
   Int_t i = 0;
   while ( m_Toggle[i].name != 0 ) {
     node = m_Geometry->GetNode(m_Toggle[i].name);
     if (node) node->SetVisibility(m_Toggle[i].vis);
     else cerr<<"No Node "<<m_Toggle[i].name<<endl;
     i++;
   }
   

          TNode *first=(TNode*)m_Geometry->GetListOfNodes()->First();
          TObjLink *lnk = first->GetListOfNodes()->FirstLink();
          recur(lnk,"TRDT");          
          lnk = first->GetListOfNodes()->FirstLink();
          recur(lnk,"TRDW");          
}


void    AMSR_GeometrySetter::recur(TObjLink *lnk, char *exclude, bool what){
         while (lnk) {       
          TNode * obj=(TNode*)lnk->GetObject();              
          const char * name=obj->GetName();
          bool off=true;
          for(int k=0;k<strlen(exclude);k++){
           if(strlen(name)<strlen(exclude) || name[k]!=exclude[k]){
            off=false;
            break;
          }
          }
          if(off)obj->SetVisibility(what);
          if(obj->GetListOfNodes())recur(obj->GetListOfNodes()->FirstLink(),exclude,what); 
          lnk = lnk->Next();                                 
         } 
}

//_____________________________________________________________________________
void AMSR_GeometrySetter::TurnOn(char * name)
{
  //
  // turn on drawing of a node, but not its sons
  //
  TNode * node = m_Geometry->GetNode(name);
  if (node) node->SetVisibility(-2);
}


//_____________________________________________________________________________
void AMSR_GeometrySetter::TurnOnWithSons(char * name)
{
  //
  // turn on drawing of a node, but not its sons
  //
  TNode * node = m_Geometry->GetNode(name);
  if (node) node->SetVisibility(3);
}


//_____________________________________________________________________________
void AMSR_GeometrySetter::TurnOff(char * name)
{
  //
  // turn off drawing of a node but not its sons
  //
  TNode * node = m_Geometry->GetNode(name);
  if (node) node->SetVisibility(2);
}


//_____________________________________________________________________________
void AMSR_GeometrySetter::TurnOffWithSons(char * name)
{
  //
  // turn off drawing of a node and its sons
  //
  TNode * node = m_Geometry->GetNode(name);
  if (node) node->SetVisibility(-1);
}


//_____________________________________________________________________________
void AMSR_GeometrySetter::CheckVisibility(char * name)
{
  TNode * node = m_Geometry->GetNode(name);
  Int_t vis;
  if (node) printf("%s vis %d\n", node->GetName(), node->GetVisibility());
}



void AMSR_GeometrySetter::UpdateGeometry(EAMSR_View mview){
  if(mview==kTopView){
          TNode *first=(TNode*)m_Geometry->GetListOfNodes()->First();
          TObjLink *lnk = first->GetListOfNodes()->FirstLink();
          recur(lnk,"T");          
          lnk = first->GetListOfNodes()->FirstLink();
          recur(lnk,"EL");          
          lnk = first->GetListOfNodes()->FirstLink();
          recur(lnk,"L");          
          lnk = first->GetListOfNodes()->FirstLink();
          recur(lnk,"ST");          
          lnk = first->GetListOfNodes()->FirstLink();
          recur(lnk,"RAD");          
          lnk = first->GetListOfNodes()->FirstLink();
          recur(lnk,"FOA");          
  }
  else{
         TNode *first=(TNode*)m_Geometry->GetListOfNodes()->First();
          TObjLink *lnk = first->GetListOfNodes()->FirstLink();
          recur(lnk,"T",true);          
          lnk = first->GetListOfNodes()->FirstLink();
          recur(lnk,"EL",true);          
          lnk = first->GetListOfNodes()->FirstLink();
          recur(lnk,"L",true);          
          lnk = first->GetListOfNodes()->FirstLink();
          recur(lnk,"ST",true);          
          lnk = first->GetListOfNodes()->FirstLink();
          recur(lnk,"RAD",true);          
          lnk = first->GetListOfNodes()->FirstLink();
          recur(lnk,"FOA",true);          
          lnk = first->GetListOfNodes()->FirstLink();
          recur(lnk,"TRDT");          
          lnk = first->GetListOfNodes()->FirstLink();
          recur(lnk,"TRDW");          
  }
}
