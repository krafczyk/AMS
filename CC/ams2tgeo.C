#include "ams2tgeo.h"

using namespace std;

bool ams2tgeo::debug_mat = false;
bool ams2tgeo::debug_med = false;
bool ams2tgeo::debug_geo = false;
bool ams2tgeo::debug_pos = false;
int  ams2tgeo::nsha = 1;
int  ams2tgeo::nvol = 1;
int  ams2tgeo::nrot = 1;

TGeoShape* ams2tgeo::AMSgvolume2TGeoShape(AMSgvolume* amsvol) {
  TGeoShape* tshape = 0;
  char shaname[200];
  sprintf(shaname,"%s_%05d",amsvol->getshape(),nsha);

  if      (strcmp(amsvol->getshape(),"BOX ")==0) {
    sprintf(shaname,"BOX_%05d",nsha);
    Double_t dx = amsvol->getpar(0); 
    Double_t dy = amsvol->getpar(1);
    Double_t dz = amsvol->getpar(2);
    tshape = new TGeoBBox(shaname,dx,dy,dz); 
    tshape->SetTransform(0);
    if (debug_geo) 
      cout << "AMSgvolume2TGeoShape::Transformation from BOX to TGeoBBox: " 
           << "  dX " << dx << "  dY " << dy << "  dZ " << dz << endl;
    if (amsvol->getnpar()!=3) cout << "AMSgvolume2TGeoShape-E-Wrong npars BOX (" << amsvol->getnpar() << ")" << endl;
  }
  else if (strcmp(amsvol->getshape(),"TRD1")==0) {
    Double_t dx1 = amsvol->getpar(0); 
    Double_t dx2 = amsvol->getpar(1);
    Double_t dy  = amsvol->getpar(2);
    Double_t dz  = amsvol->getpar(3);
    tshape = new TGeoTrd1(shaname,dx1,dx2,dy,dz); 
    tshape->SetTransform(0);
    if (debug_geo)
      cout << "AMSgvolume2TGeoShape::Transformation from TRD1 to TGeoTrd1: "
           << "  dX1 " << dx1 << "  dX2 " << dx2 << "  dY " << dy << "  dZ " << dz << endl;
    if (amsvol->getnpar()!=4) cout << "AMSgvolume2TGeoShape-E-Wrong npars TRD1 (" << amsvol->getnpar() << ")" << endl;
  }
  else if (strcmp(amsvol->getshape(),"TRD2")==0) {
    Double_t dx1 = amsvol->getpar(0);
    Double_t dx2 = amsvol->getpar(1);
    Double_t dy1 = amsvol->getpar(2);
    Double_t dy2 = amsvol->getpar(3);
    Double_t dz  = amsvol->getpar(4);
    tshape = new TGeoTrd2(shaname,dx1,dx2,dy1,dy2,dz);
    tshape->SetTransform(0);
    if (debug_geo)
      cout << "AMSgvolume2TGeoShape::Transformation from TRD2 to TGeoTrd2: "
           << "  dX1 " << dx1 << "  dX2 " << dx2 << "  dY1 " << dy1 << "  dY2 " << dy2 << "  dZ " << dz << endl;
    if (amsvol->getnpar()!=5) cout << "AMSgvolume2TGeoShape-E-Wrong npars TRD2 (" << amsvol->getnpar() << ")" << endl;
  }
  else if (strcmp(amsvol->getshape(),"TRAP")==0) {
    Double_t dz     = amsvol->getpar(0);
    Double_t theta  = amsvol->getpar(1);
    Double_t phi    = amsvol->getpar(2);
    Double_t h1     = amsvol->getpar(3);
    Double_t bl1    = amsvol->getpar(4);
    Double_t tl1    = amsvol->getpar(5);
    Double_t alpha1 = amsvol->getpar(6);
    Double_t h2     = amsvol->getpar(7);
    Double_t bl2    = amsvol->getpar(8);
    Double_t tl2    = amsvol->getpar(9);
    Double_t alpha2 = amsvol->getpar(10);
    tshape = new TGeoTrap(shaname,dz,theta,phi,h1,bl1,tl1,alpha1,h2,bl2,tl2,alpha2);
    tshape->SetTransform(0);
    if (debug_geo)
      cout << "AMSgvolume2TGeoShape::Transformation from TRAP to TGeoTrap: "
           << "  dZ " << dz << "  Theta " << theta << "  Phi " << phi << "  H1 " << h1 << "  Bl1 " << bl1 
           << "  Tl1 " << tl1 << "  Alpha1 " << alpha1 << "  H2 " << h2 << "  Bl2 " << bl2 << " Tl2 " << tl2 << endl; 
    if (amsvol->getnpar()!=11) cout << "AMSgvolume2TGeoShape-E-Wrong npars TRAP (" << amsvol->getnpar() << ")" << endl;
  }
  else if (strcmp(amsvol->getshape(),"TUBE")==0) {
    Double_t rmin = amsvol->getpar(0);
    Double_t rmax = amsvol->getpar(1);
    Double_t dz   = amsvol->getpar(2);
    tshape = new TGeoTube(shaname,rmin,rmax,dz);
    tshape->SetTransform(0);
    if (debug_geo)
      cout << "AMSgvolume2TGeoShape::Transformation from TUBE to TGeoTube: "
           << "  Rmin " << rmin << "  Rmax " << rmax << "  dZ " << dz << endl;
    tshape->SetTransform(0);
    if (amsvol->getnpar()!=3) cout << "AMSgvolume2TGeoShape-E-Wrong npars TUBE (" << amsvol->getnpar() << ")" << endl;
  }
  else if (strcmp(amsvol->getshape(),"TUBS")==0) {
    Double_t rmin = amsvol->getpar(0);
    Double_t rmax = amsvol->getpar(1);
    Double_t dz   = amsvol->getpar(2);
    Double_t phi1 = amsvol->getpar(3);
    Double_t phi2 = amsvol->getpar(4);
    tshape = new TGeoTubeSeg(shaname,rmin,rmax,dz,phi1,phi2); 
    if (debug_geo)
      cout << "AMSgvolume2TGeoShape::Transformation from TUBS to TGeoTubeSeg: " 
           << "  Rmin " << rmin << "  Rmax " << rmax << "  dZ " << dz << "  Phi1 " << phi1 << "  Phi2 " << phi2 << endl;
    tshape->SetTransform(0);
    if (amsvol->getnpar()!=5) cout << "AMSgvolume2TGeoShape-E-Wrong npars TUBS (" << amsvol->getnpar() << ")" << endl;
  }
  else if (strcmp(amsvol->getshape(),"CONE")==0) {
    Double_t dz    = amsvol->getpar(0);
    Double_t rmin1 = amsvol->getpar(1);
    Double_t rmax1 = amsvol->getpar(2);
    Double_t rmin2 = amsvol->getpar(3);
    Double_t rmax2 = amsvol->getpar(4);
    tshape = new TGeoCone(shaname,dz,rmin1,rmax1,rmin2,rmax2);
    if (debug_geo)
      cout << "AMSgvolume2TGeoShape::Transformation from CONE to TGeoCone: "
           << "  dZ " << dz << "  Rmin1 " << rmin1 << "  Rmax1 " << rmax1 << "  Rmin2 " << rmin2 << "  Rmax2 " << rmax2 << endl;
    tshape->SetTransform(0);
    if (amsvol->getnpar()!=5) cout << "AMSgvolume2TGeoShape-E-Wrong npars CONE (" << amsvol->getnpar() << ")" << endl;
  }
  else if (strcmp(amsvol->getshape(),"CONS")==0) {
    Double_t dz    = amsvol->getpar(0);
    Double_t rmin1 = amsvol->getpar(1);
    Double_t rmax1 = amsvol->getpar(2);
    Double_t rmin2 = amsvol->getpar(3);
    Double_t rmax2 = amsvol->getpar(4);
    Double_t phi1  = amsvol->getpar(5);
    Double_t phi2  = amsvol->getpar(6);
    tshape = new TGeoConeSeg(shaname,dz,rmin1,rmax1,rmin2,rmax2,phi1,phi2);
    if (debug_geo)
      cout << "AMSgvolume2TGeoShape::Transformation from CONE to TGeoCone: "
           << "  dZ " << dz << "  Rmin1 " << rmin1 << "  Rmax1 " << rmax1 << "  Rmin2 " << rmin2 << "  Rmax2 " << rmax2 << "  Phi1 " << phi1 << "  Phi2 " << phi2 << endl;
    tshape->SetTransform(0);
    if (amsvol->getnpar()!=7) cout << "AMSgvolume2TGeoShape-E-Wrong npars CONS (" << amsvol->getnpar() << ")" << endl;
  }
  else if (strcmp(amsvol->getshape(),"PARA")==0) {
    Double_t dx    = amsvol->getpar(0);
    Double_t dy    = amsvol->getpar(1);
    Double_t dz    = amsvol->getpar(2);
    Double_t alpha = amsvol->getpar(3);
    Double_t theta = amsvol->getpar(4);
    Double_t phi   = amsvol->getpar(5);
    tshape = new TGeoPara(shaname,dx,dy,dz,alpha,theta,phi);
    if (debug_geo)
      cout << "AMSgvolume2TGeoShape::Transformation from PARA to TGeoTubePara: "
           << "  dX " << dx << "  dY " << dy << "  dZ " << dz << "  Alpha " << alpha << "  Theta " << theta << "  Phi " << phi << endl;
    tshape->SetTransform(0);
    if (amsvol->getnpar()!=6) cout << "AMSgvolume2TGeoShape-E-Wrong npars PARA (" << amsvol->getnpar() << ")" << endl;
  }
  else if (strcmp(amsvol->getshape(),"PGON")==0) {
    Double_t phi    = amsvol->getpar(0);
    Double_t dphi   = amsvol->getpar(1);
    Int_t    nedges = (int) amsvol->getpar(2);
    Int_t    nz     = (int) amsvol->getpar(3);
    tshape = new TGeoPgon(shaname,phi,dphi,nedges,nz);
    if (debug_geo)
      cout << "AMSgvolume2TGeoShape::Transformation from PGON to TGeoPgon: "
           << "  Phi " << phi << "  dPhi " << dphi << "  nZ " << nz << "  nEdges " << nedges << endl;
    // loop on planes
    for (int iz=0; iz<nz; iz++) {
      Double_t z    = amsvol->getpar(4 + iz*3);
      Double_t rmin = amsvol->getpar(5 + iz*3);
      Double_t rmax = amsvol->getpar(6 + iz*3);
      ((TGeoPgon*)tshape)->DefineSection(iz,z,rmin,rmax);
      if (debug_geo) 
	cout << "  iZ " << iz << "  Z " << z << "  Rmin " << rmin << "  Rmax " << rmax << endl;
    }
    tshape->SetTransform(0);
    if (amsvol->getnpar()!=4+(nz*3)) cout << "AMSgvolume2TGeoShape-E-Wrong npars PGON (" << amsvol->getnpar() << " expected " << 4+(nz*3) << ")" << endl;
  }
  else if (strcmp(amsvol->getshape(),"PCON")==0) {
    Double_t phi    = amsvol->getpar(0);
    Double_t dphi   = amsvol->getpar(1);
    Int_t    nz     = (int) amsvol->getpar(2);
    tshape = new TGeoPcon(shaname,phi,dphi,nz);
    if (debug_geo)
      cout << "AMSgvolume2TGeoShape::Transformation from PCON to TGeoPcon: "
           << "  Phi " << phi << "  dPhi " << dphi << "  nZ " << nz << endl;
    // loop on planes
    for (int iz=0; iz<nz; iz++) {
      Double_t z    = amsvol->getpar(3 + iz*3);
      Double_t rmin = amsvol->getpar(4 + iz*3);
      Double_t rmax = amsvol->getpar(5 + iz*3);
      ((TGeoPcon*)tshape)->DefineSection(iz,z,rmin,rmax);
      if (debug_geo) 
	cout << "  iZ " << iz << "  Z " << z << "  Rmin " << rmin << "  Rmax " << rmax << endl;
    }
    tshape->SetTransform(0);
    if (amsvol->getnpar()!=3+(nz*3)) cout << "AMSgvolume2TGeoShape-E-Wrong npars PCON (" << amsvol->getnpar() << " expected " << 3+(nz*3) << ")" << endl;
  }
  else if (strcmp(amsvol->getshape(),"ELTU")==0) {
    Double_t a   = amsvol->getpar(0);
    Double_t b   = amsvol->getpar(1);
    Double_t dz  = amsvol->getpar(2);
    tshape = new TGeoEltu(shaname,a,b,dz);
    if (debug_geo)
      cout << "AMSgvolume2TGeoShape::Transformation from ELTU to TGeoEltu: "
           << "  A " << a << "  B " << b << "  DZ " << dz << endl;
    tshape->SetTransform(0);
    if (amsvol->getnpar()!=3) cout << "AMSgvolume2TGeoShape-E-Wrong npars ELTU (" << amsvol->getnpar() << ")" << endl;
  }
  else {
    cout << "AMSgvolume2TGeoShape-E-Shape " << amsvol->getshape() << " not implemented" << endl;
  }  
  nsha++;
  return tshape;
}

TGeoCombiTrans* ams2tgeo::AMSgvolume2TGeoCombiTrans(AMSgvolume* amsvol) {
  Double_t dx    = amsvol->getcoo(0);
  Double_t dy    = amsvol->getcoo(1);
  Double_t dz    = amsvol->getcoo(2);
  char name[200];  
  sprintf(name,"MAT_%05d",nrot);
  Double_t theta[3];
  Double_t phi[3];    
  for (int i=0; i<3; i++) {
    Double_t cosine[3] = {0,0,0};
    for (int j=0; j<3; j++) { 
      cosine[j] = amsvol->getnrm(j,i); 
    }
    theta[i]  = atan2(sqrt(pow(cosine[0],2)+pow(cosine[1],2)),cosine[2])*90/acos(0);
    phi[i]    = atan2(cosine[1],cosine[0])*90/acos(0);
  }
  sprintf(name,"ROT_%05d",nrot);  
  TGeoRotation* trot = new TGeoRotation(name,theta[0],phi[0],theta[1],phi[1],theta[2],phi[2]);
  sprintf(name,"COM_%05d",nrot);
  TGeoCombiTrans* tcom = new TGeoCombiTrans(name,dx,dy,dz,trot);
  return tcom;
}

TGeoVolume* ams2tgeo::AMSgvolume2TGeoVolume(TGeoManager* tman, AMSgvolume* amsvol) {
  AMSgtmed* amsmed = amsvol->getpgtmed();
  char medname[200];
  char volname[200];
  sprintf(medname,"%s",amsmed->getname());
  ToUpper(medname);
  TGeoMedium* tmed = tman->GetMedium(medname);
  TGeoShape*  tsha = AMSgvolume2TGeoShape(amsvol); 
  sprintf(volname,"%s_%05d",amsvol->getname(),nvol); 
  TGeoVolume* tvol = new TGeoVolume(volname,tsha,tmed);
  if (debug_geo) cout << "AMSgvolume2TGeoVolume::Creating TGeoVolume " << tvol->GetName() << " type " << tsha->GetName() << " medium " << tmed->GetName() << endl;
  return tvol;
}

TGeoVolume* ams2tgeo::AMSGeometry2TGeometry(TGeoManager* tman, TGeoVolume* tvol_mot, AMSgvolume* amsvol) {
  TGeoVolume*      tvol = AMSgvolume2TGeoVolume(tman,amsvol);
  TGeoCombiTrans*  tcom = AMSgvolume2TGeoCombiTrans(amsvol);
  if (tvol_mot==0) tman->SetTopVolume(tvol);          // world
  else             tvol_mot->AddNode(tvol,nvol,tcom); // adding a daughter to her mother 
  nvol++;
  if (amsvol->down()) {
    amsvol = amsvol->down();
    // cout << "Mother Volume " << tvol->GetName() << endl; 
    while (amsvol) {
      // cout << "  A Daughter  " << amsvol->getname() << " nvol " << nvol << endl;
      AMSGeometry2TGeometry(tman,tvol,amsvol);
      amsvol = amsvol->next();   
    }
  }
  return tvol;
}


void ams2tgeo::ToUpper(char* name) {
  int ii = 0;
  while (name[ii]) {
    name[ii] = toupper(name[ii]);
    ii++;
  }
}


TGeoManager* ams2tgeo::Build(char* name, char* title, char* filename) {

  TGeoManager* tman = new TGeoManager(name,title);

  ////////////////////////////////////////////  
  printf("ams2tgeo-V-Converting AMS materials into TGeoElementTable\n");
  ////////////////////////////////////////////  

  AMSgmat* amsmattab = AMSJob::gethead()->getmat();
  if (amsmattab==0) { printf("ams2tgeo-E-No AMS materials table, returning an empty geometry\n"); tman->CloseGeometry(); return tman; } 
  TGeoElementTable* teltab = new TGeoElementTable();
  AMSgmat* amsmat = amsmattab->down();
  char matname[200];
  char elename[200]; 
 
  while (amsmat) {
    sprintf(matname,"%s",amsmat->getname());
    ToUpper(matname);

    if (debug_mat) cout << "AMSgmat " << matname << " " << amsmat->getnpar() << " " << amsmat->getdensity() << " " << amsmat->getradlen() << " " << amsmat->getintlen() << endl;
    int nelem = amsmat->getnpar();
    float* a = new float[nelem];
    float* z = new float[nelem];
    float* w = new float[nelem];
    amsmat->getNumbers(a,z,w);

    // Create the elements
    for (int i=0; i<nelem; i++) {
      if (debug_mat) cout << "  Element of " << matname << " n. " << i << " --> " << a[i] << " " << z[i] << " " << w[i] << endl;
      sprintf(elename,"%s_ELEM%d",matname,i);
      teltab->AddElement(elename,elename,(int)z[i],(double)a[i]);
    }
    // Create material
    if (nelem==1) { // Only one element
      sprintf(elename,"%s_ELEM0",matname);
      TGeoElement* tel = teltab->FindElement(elename);
      TGeoMaterial* tmat = new TGeoMaterial(matname,tel,amsmat->getdensity());
      tmat->SetRadLen(amsmat->getradlen(),amsmat->getintlen());
    }
    else { // Mixture 
      TGeoMixture* tmix = new TGeoMixture(matname,nelem,amsmat->getdensity());
      for (int i=0; i<nelem; i++) {
        sprintf(elename,"%s_ELEM%d",matname,i);
        TGeoElement* tel = teltab->FindElement(elename);
        tmix->AddElement(tel,w[i]);
        if (debug_mat) cout << "    Adding " << tel->GetName() << " with weight " << w[i] << endl;        
        tmix->SetRadLen(amsmat->getradlen(),amsmat->getintlen());
      }
    }

    delete[] a;
    delete[] z;
    delete[] w; 

    // next material 
    amsmat = amsmat->next();
  }
 

  ////////////////////////////////////////////  
  printf("ams2tgeo-V-Converting AMS media into TGeoMedium\n");
  ////////////////////////////////////////////  

  AMSgtmed* amsmedtab = AMSJob::gethead()->getmed(); 
  if (amsmedtab==0) { printf("ams2tgeo-E-No AMS media table, returning a null geometry\n"); tman->CloseGeometry(); return tman; }
  AMSgtmed* amsmed = amsmedtab->down();
  char medname[200];
  double param[20];
  int im=0;

  // loop on media
  while (amsmed) {
    sprintf(medname,"%s",amsmed->getname());
    ToUpper(medname);
    AMSgmat* amsmat = amsmed->getpgmat();
    sprintf(matname,"%s",amsmat->getname());
    ToUpper(matname);

    // find the right TGeoMaterial
    TGeoMaterial* tmat = tman->GetMaterial(matname);

    if (debug_med) cout << "AMSgtmed " << medname << " of " << matname << " (" << tmat->GetName() << ") " << endl;

    // Tracking properties
    for (int i=0; i<20; ++i) param[i] = 0.;
    param[0] = amsmed->getisvol();      // isvol  - Not used
    param[1] = amsmed->getifield();     // ifield - User defined magnetic field
    param[2] = amsmed->getfieldm();     // fieldm - Maximum field value (in kiloGauss)
    param[3] = amsmed->gettmaxfd();     // tmaxfd - Maximum angle due to field deflection
    param[4] = amsmed->getstemax();     // stemax - Maximum displacement for multiple scat
    param[5] = amsmed->getdeemax();     // deemax - Maximum fractional energy loss, DLS
    param[6] = amsmed->getepsil();      // epsil  - Tracking precision
    param[7] = amsmed->getstmin();      // stmin
    if (debug_med) for (int i=0; i<8; ++i) cout << param[i] << endl;

    TGeoMedium* tmed = new TGeoMedium(medname,im,tmat,param);

    // next medium 
    amsmed = amsmed->next();
    im++;
  }


  ////////////////////////////////////////////  
  printf("ams2tgeo-V-Converting AMS geometry into TGeo\n");
  ////////////////////////////////////////////  

  AMSgvolume* amsmotvol = AMSJob::gethead()->getgeom();
  if (amsmotvol==0) { printf("ams2tgeo-E-No AMS mother volume, returning a null geometry\n"); tman->CloseGeometry(); return tman; }

  // recursive function 
  AMSGeometry2TGeometry(tman,0,amsmotvol);

  // end
  tman->CloseGeometry(); 

  // saving
  tman->Export(filename);

  return tman; 
} 







