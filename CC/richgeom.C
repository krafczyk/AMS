#include <typedefs.h>
#include <node.h>
#include <snode.h>
#include <gmat.h>
#include <extC.h>
#include <stdlib.h>
#include <gvolume.h>
#include <amsgobj.h>
#include <job.h>
#include <commons.h>
#include <richdbc.h>

#define SQR(x) ((x)*(x))

namespace amsgeom{
extern void tkgeom(AMSgvolume &);
extern void tkgeom02(AMSgvolume &);
extern void magnetgeom(AMSgvolume &);
extern void magnetgeom02(AMSgvolume &);
extern void magnetgeom02Test(AMSgvolume &);
extern void tofgeom(AMSgvolume &);
extern void tofgeom02(AMSgvolume &);
extern void antigeom(AMSgvolume &);
extern void antigeom02(AMSgvolume &);
#ifdef __G4AMS__
 extern void antigeom02g4(AMSgvolume &);
extern void testg4geom(AMSgvolume &);
#endif
extern void pshgeom(AMSgvolume &);
extern void pshgeom02(AMSgvolume &);
extern void ctcgeom(AMSgvolume &);
extern void ctcgeomE(AMSgvolume &, integer iflag);
extern void ctcgeomAG(AMSgvolume& );
extern void ctcgeomAGPlus(AMSgvolume& );
extern void richgeom02(AMSgvolume &);
extern void ecalgeom02(AMSgvolume &);
extern void trdgeom02(AMSgvolume &);
extern void srdgeom02(AMSgvolume &);
extern void Put_rad(AMSgvolume *,integer);
extern void Put_pmt(AMSgvolume *,integer);
};


using namespace amsgeom;


void amsgeom::Put_rad(AMSgvolume * mother,integer copia)
{
  AMSNode *dummy;
  geant par[3],coo[3];
  number nrm[3][3]={1.,0.,0.,0.,1.,0.,0.,0.,1.}; // {vx, vy, vz}
  const integer rel=1;

  par[0]=RICGEOM.radiator_box_length/2.-RICaethk;
  par[1]=RICGEOM.radiator_box_length/2.-RICaethk;
  par[2]=RICGEOM.radiator_height/2;
  coo[0]=0;
  coo[1]=0;
  coo[2]=0;
  
  
  mother->add(new AMSgvolume("RICH RAD",
			     0,
			     "RAD ",
			     "BOX",
			     par,
			     3,
			     coo,
			     nrm,
			     "ONLY",
			     0,
			     copia,
			     rel));
}


void amsgeom::Put_pmt(AMSgvolume * lig,integer copia)
{
  AMSNode *dummy;
  AMSgvolume *solid_lg;
  geant par[11],coo[3],fcoo[3],fpar[11];
  number nrm[3][3]={1.,0.,0.,0.,1.,0.,0.,0.,1.}; // {vx, vy, vz}
  number nrma[3][3]={0,-1,0,1,0,0,0,0,1}; // Rotated 90 degrees
  const integer rel=1; 
  integer flag=1;
  


// Shielding

  par[0]=45;
  par[1]=360;
  par[2]=4;
  par[3]=2;
  par[4]=-RICshiheight/2; //NEW!
  par[5]=(RICGEOM.light_guides_length/2-RICpmtshield);
  par[6]=RICGEOM.light_guides_length/2;
  par[7]=RICshiheight/2;
  par[8]=(RICGEOM.light_guides_length/2-RICpmtshield);
  par[9]=RICGEOM.light_guides_length/2;

  coo[0]=0;
  coo[1]=0;
  coo[2]=RICshiheight/2-RICHDB::pmtb_height()/2.;

  dummy=lig->add(new AMSgvolume("RICH SHIELD",
				0,
				"PMTS",
				"PGON",
				par,
				10,
				coo,
				nrm,
				"ONLY",
				0,
				copia,
				rel));

#ifdef __G4AMS__
  ((AMSgvolume*)dummy)->Smartless()=-2;
#endif


// Electronics

  par[0]=RICGEOM.light_guides_length/2-RICpmtshield;
  par[1]=RICGEOM.light_guides_length/2-RICpmtshield;
  par[2]=(RICpmtlength-RICotherthk)/2; 
  
  coo[0]=0;
  coo[1]=0;
  coo[2]=RICHDB::elec_pos();
  
  
  dummy=lig->add(new AMSgvolume("RICH GLUE", // Good approx. media
				0,
				"RELE", // R(ich) electronics
				"BOX",
				par,
				3,
				coo,
				nrm,
				"ONLY",
				0,
				copia,
				rel));
  
#ifdef __G4AMS__
        ((AMSgvolume*)dummy)->Smartless()=-2;
#endif

  // Photocathode

  par[0]=RICcatolength/2; // The HAMAMATSU R5900 cathode length
  par[1]=RICcatolength/2;
  par[2]=RICotherthk/2; // Thickness: 1mm
  coo[0]=0;
  coo[1]=0;
  coo[2]=RICHDB::cato_pos();

  dummy=lig->add(new AMSgvolume("RICH PMTS",
				0,
				"CATO",
				"BOX",
				par,
				3,
				coo,
				nrm,
				"ONLY",
				0,
				copia,
				rel));
  

#ifdef __G4AMS__
  ((AMSgvolume*)dummy)->Smartless()=-2;
#endif


#ifndef __HOLLOW__
// Solid light guides
#ifdef __AMSDEBUG__
  cout <<"RICH: Solid LG chosen at compilation time."<<endl;
#endif


  coo[0]=0;
  coo[1]=0;
  coo[2]=RICHDB::lg_pos();

  par[0]=45.;
  par[1]=360.;
  par[2]=4.;
  par[3]=2.;
  par[4]=-RICGEOM.light_guides_height/2.;
  par[5]=0.;
  par[6]=RICcatolength/2.;
  par[7]=RICGEOM.light_guides_height/2.;
  par[8]=0.;
  par[9]=RICGEOM.light_guides_length/2.;

  solid_lg=(AMSgvolume*)lig->add(new AMSgvolume("RICH SOLG",
				   0,
				   "SLGC",
				   "PGON",
				   par,
				   10,
				   coo,
				   nrm,
				   "ONLY",
				   0,
				   copia,
				   rel));
				   
#ifdef __G4AMS__
  solid_lg->Smartless()=-2;
#endif

  coo[2]=0.;
  
  par[0]=RICGEOM.light_guides_height/2;
  par[1]=RICHDB::lg_mirror_angle(1);
  par[2]=90;
  par[3]=RIClgthk/2;
  par[4]=RICcatolength/2;
  par[5]=RICcatolength/2;
  par[6]=0;
  par[7]=RIClgthk/2;
  par[8]=RICGEOM.light_guides_length/2;
  par[9]=RICGEOM.light_guides_length/2;
  par[10]=0;
  
  coo[1]=RICHDB::lg_mirror_pos(1);
  dummy=solid_lg->add(new AMSgvolume("RICH WALLS",
				     0,
				     "MIRA",
				     "TRAP",
				     par,
				     11,
				     coo,
				     nrm,
				     "ONLY", // This seems to be safe
				     0,
				     2*copia-1,
				     rel));

   
#ifdef __G4AMS__
  ((AMSgvolume*)dummy)->Smartless()=-2;
#endif
  
  // Trick to avoid optical contact
  for(integer i=0;i<11;i++)fpar[i]=par[i];
  for(integer i=0;i<3;i++) fcoo[i]=coo[i];
  
  fpar[3]=RICepsln/2.;
  fpar[7]=fpar[3];
  fcoo[1]-=RIClgthk/2.+RICepsln/2.;

  dummy=solid_lg->add(new AMSgvolume("VACUUM",
				     0,
				     "MIA0",
				     "TRAP",
				     fpar,
				     11,
				     fcoo,
				     nrm,
				     "ONLY", // This seems to be safe
				     0,
				     2*copia-1,
				     rel));
  
#ifdef __G4AMS__
  ((AMSgvolume*)dummy)->Smartless()=-2;
#endif
  
  coo[0]=-coo[1];
  coo[1]=0;
  
  dummy=solid_lg->add(new AMSgvolume("RICH WALLS",
				     RICnrot,
				     "MIRA",
				     "TRAP",
				     par,
				     11,
				     coo,
				     nrma,
				     "ONLY",
				     0,
				     2*copia,
				     rel));
  
#ifdef __G4AMS__
  ((AMSgvolume*)dummy)->Smartless()=-2;
#endif  
  
  fcoo[0]=coo[0]+RIClgthk/2.+RICepsln/2.;
  fcoo[1]=0;
  
  dummy=solid_lg->add(new AMSgvolume("VACUUM",
				     RICnrot,
				     "MIA0",
				     "TRAP",
				     fpar,
				     11,
				     fcoo,
				     nrma,
				     "ONLY", // This seems to be safe
				     0,
				     2*copia,
				     rel));
  
#ifdef __G4AMS__
  ((AMSgvolume*)dummy)->Smartless()=-2;
#endif   
  
  par[0]=RICGEOM.light_guides_height/2;
  par[1]=RICHDB::lg_mirror_angle(2);
  par[2]=90;
  par[3]=RIClgthk/2;
  par[4]=RICcatolength/2;
  par[5]=RICcatolength/2;
  par[6]=0;
  par[7]=RIClgthk/2;
  par[8]=RICGEOM.light_guides_length/2;
  par[9]=RICGEOM.light_guides_length/2;
  par[10]=0;
  
  coo[0]=0;
  coo[1]=RICHDB::lg_mirror_pos(2);
  
  
  dummy=solid_lg->add(new AMSgvolume("RICH WALLS",
				     0,
				     "MIRB",
				     "TRAP",
				     par,
				     11,
				     coo,
				     nrm,  
				     "ONLY",
				     0,
				     2*copia-1,
				     rel));
#ifdef __G4AMS__
  ((AMSgvolume*)dummy)->Smartless()=-2;
#endif   
  
  for(integer i=0;i<11;i++)fpar[i]=par[i];
  for(integer i=0;i<3;i++) fcoo[i]=coo[i];
  
  fpar[3]=RICepsln/2.;
  fpar[7]=fpar[3];
  fcoo[1]-=RIClgthk/2.+RICepsln/2.;
  
  dummy=solid_lg->add(new AMSgvolume("VACUUM",
				     0,
				     "MIB0",
				     "TRAP",
				     fpar,
				     11,
				     fcoo,
				     nrm,
				     "ONLY", // This seems to be safe
				     0,
				     4*copia-3,
				     rel));
#ifdef __G4AMS__
  ((AMSgvolume*)dummy)->Smartless()=-2;
#endif
  
  fcoo[1]=coo[1]+RIClgthk/2.+RICepsln/2.;
  
  dummy=solid_lg->add(new AMSgvolume("VACUUM",
				     0,
				     "MIB0",
				     "TRAP",
				     fpar,
				     11,
				     fcoo,
				     nrm,
				     "ONLY", // This seems to be safe
				     0,
				     4*copia-2,
				     rel));
  
  
#ifdef __G4AMS__
  ((AMSgvolume*)dummy)->Smartless()=-2;
#endif
  
  coo[0]=-coo[1];
  coo[1]=0;
  
  dummy=solid_lg->add(new AMSgvolume("RICH WALLS",
				     RICnrot,
				     "MIRB",
				     "TRAP",
				     par,
				     11,
				     coo,
				     nrma,  // Rotated 90 degrees
				     "ONLY",
				     0,
				     2*copia,
				     rel));
  
#ifdef __G4AMS__
  ((AMSgvolume*)dummy)->Smartless()=-2;
#endif
  
  fcoo[0]=coo[0]+RIClgthk/2.+RICepsln/2.;
  fcoo[1]=0;
  
  dummy=solid_lg->add(new AMSgvolume("VACUUM",
				     RICnrot,
				     "MIB0",
				     "TRAP",
				     fpar,
				     11,
				     fcoo,
				     nrma,
				     "ONLY", // This seems to be safe
				     0,
				     4*copia-1,
				     rel));
  
#ifdef __G4AMS__
  ((AMSgvolume*)dummy)->Smartless()=-2;
#endif   
  
  fcoo[0]=coo[0]-RIClgthk/2.-RICepsln/2.;
  dummy=solid_lg->add(new AMSgvolume("VACUUM",
				     RICnrot,
				     "MIB0",
				     "TRAP",
				     fpar,
				     11,
				     fcoo,
				     nrma,
				     "ONLY", // This seems to be safe
				     0,
				     4*copia,
				     rel));
  
#ifdef __G4AMS__
  ((AMSgvolume*)dummy)->Smartless()=-2;
#endif    
  
  par[0]=RICGEOM.light_guides_height/2;;
  par[1]=0;
  par[2]=90;
  par[3]=RIClgthk/2;
  par[4]=RICcatolength/2;
  par[5]=RICcatolength/2;
  par[6]=0;
  par[7]=RIClgthk/2;
  par[8]=RICGEOM.light_guides_length/2;
  par[9]=RICGEOM.light_guides_length/2;
  par[10]=0;
  
  coo[0]=0;
  coo[1]=0;
  
  
  dummy=solid_lg->add(new AMSgvolume("RICH WALLS",
				     0,
				     "MIRC",
				     "TRAP",
				     par,
				     11,
				     coo,
				     nrm,  // Rotated 90 degrees
				     "ONLY",
				     0,
				     2*copia-1,
				     rel));
#ifdef __G4AMS__
  ((AMSgvolume*)dummy)->Smartless()=-2;
#endif   
  
  for(integer i=0;i<11;i++)fpar[i]=par[i];
  for(integer i=0;i<3;i++) fcoo[i]=coo[i];
  
  fpar[3]=RICepsln/2.;
  fpar[7]=fpar[3];

  fcoo[1]-=RIClgthk/2.+RICepsln/2.;
  
  dummy=solid_lg->add(new AMSgvolume("VACUUM",
				     0,
				     "MIC0",
				     "TRAP",
				     fpar,
				     11,
				     fcoo,
				     nrm,
				     "ONLY", // This seems to be safe
				     0,
				     4*copia-3,
				     rel));

#ifdef __G4AMS__
  ((AMSgvolume*)dummy)->Smartless()=-2;
#endif

  fcoo[1]=coo[0]+RIClgthk/2.+RICepsln/2.;
  
  dummy=solid_lg->add(new AMSgvolume("VACUUM",
				     0,
				     "MIC0",
				     "TRAP",
				     fpar,
				     11,
				     fcoo,
				     nrm,
				     "ONLY", // This seems to be safe
				     0,
				     4*copia-2,
				     rel));

#ifdef __G4AMS__
  ((AMSgvolume*)dummy)->Smartless()=-2;
#endif

  dummy=solid_lg->add(new AMSgvolume("RICH WALLS",
				     RICnrot,
				     "MIRC",
				     "TRAP",
				     par,
				     11,
				     coo,
				     nrma,  // Rotated 90 degrees
				     "ONLY",
				     0,
				     2*copia,
				     rel));
 
#ifdef __G4AMS__
  ((AMSgvolume*)dummy)->Smartless()=-2;
#endif
  
  fcoo[0]=coo[0]+RIClgthk/2.+RICepsln/2.;
  fcoo[1]=0;
  
  dummy=solid_lg->add(new AMSgvolume("VACUUM",
				     RICnrot,
				     "MIC0",
				     "TRAP",
				     fpar,
				     11,
				     fcoo,
				     nrma,
				     "ONLY", // This seems to be safe
				     0,
				     4*copia-1,
				     rel));
 
#ifdef __G4AMS__
  ((AMSgvolume*)dummy)->Smartless()=-2;
#endif

  fcoo[0]=coo[0]-RIClgthk/2.-RICepsln/2.;
  dummy=solid_lg->add(new AMSgvolume("VACUUM",
				     RICnrot,
				     "MIC0",
				     "TRAP",
				     fpar,
				     11,
				     fcoo,
				     nrma,
				     "ONLY", // This seems to be safe
				     0,
				     4*copia,
				     rel));
 
#ifdef __G4AMS__
  ((AMSgvolume*)dummy)->Smartless()=-2;
#endif

    par[0]=RICGEOM.light_guides_height/2;
    par[1]=RICHDB::lg_mirror_angle(1);
    par[2]=270;
    par[3]=RIClgthk/2;
    par[4]=RICcatolength/2;
    par[5]=RICcatolength/2;
    par[6]=0;
    par[7]=RIClgthk/2;
    par[8]=RICGEOM.light_guides_length/2;
    par[9]=RICGEOM.light_guides_length/2;
    par[10]=0;
    
    coo[0]=0;
    coo[1]=-RICHDB::lg_mirror_pos(1);

    dummy=solid_lg->add(new AMSgvolume("RICH WALLS",
				       0,
				       "MIRD",
				       "TRAP",
				       par,
				       11,
				       coo,
				       nrm,
				       "ONLY",
				       0,
				       2*copia-1,
				       rel));
#ifdef __G4AMS__
    ((AMSgvolume*)dummy)->Smartless()=-2;
#endif
     
  for(integer i=0;i<11;i++)fpar[i]=par[i];
  for(integer i=0;i<3;i++) fcoo[i]=coo[i];
  
  fpar[3]=RICepsln/2.;
  fpar[7]=fpar[3];
  
  fcoo[1]+=RIClgthk/2.+RICepsln/2.;

  dummy=solid_lg->add(new AMSgvolume("VACUUM",
				     0,
				     "MID0",
				     "TRAP",
				     fpar,
				     11,
				     fcoo,
				     nrm,
				     "ONLY", // This seems to be safe
				     0,
				     2*copia-1,
				     rel));

#ifdef __G4AMS__
  ((AMSgvolume*)dummy)->Smartless()=-2;
#endif

    coo[0]=-coo[1];
    coo[1]=0;

    dummy=solid_lg->add(new AMSgvolume("RICH WALLS",
				       RICnrot,
				       "MIRD",
				       "TRAP",
				       par,
				       11,
				       coo,
				       nrma,
				       "ONLY",
				       0,
				       2*copia,
				       rel));
#ifdef __G4AMS__
     ((AMSgvolume*)dummy)->Smartless()=-2;
#endif

     fcoo[0]=coo[0]-RIClgthk/2.+RICepsln/2.;
     fcoo[1]=coo[1];
     
     dummy=solid_lg->add(new AMSgvolume("VACUUM",
					RICnrot,
					"MID0",
					"TRAP",
					fpar,
					11,
					fcoo,
				     nrma,
					"ONLY", // This seems to be safe
					0,
					2*copia,
					rel));


#ifdef __G4AMS__
     ((AMSgvolume*)dummy)->Smartless()=-2;
#endif

    par[0]=RICGEOM.light_guides_height/2;;
    par[1]=RICHDB::lg_mirror_angle(2);
    par[2]=270;
    par[3]=RIClgthk/2;
    par[4]=RICcatolength/2;
    par[5]=RICcatolength/2;
    par[6]=0;
    par[7]=RIClgthk/2;
    par[8]=RICGEOM.light_guides_length/2;
    par[9]=RICGEOM.light_guides_length/2;
    par[10]=0;
    
    coo[0]=0;
    coo[1]=-RICHDB::lg_mirror_pos(2);
	  

    dummy=solid_lg->add(new AMSgvolume("RICH WALLS",
				       0,
				       "MIRE",
				       "TRAP",
				       par,
				       11,
				       coo,
				       nrm,  // Rotated 90 degrees
				       "ONLY",
				       0,
				       2*copia-1,
				       rel));
#ifdef __G4AMS__
     ((AMSgvolume*)dummy)->Smartless()=-2;
#endif    

  for(integer i=0;i<11;i++)fpar[i]=par[i];
  for(integer i=0;i<3;i++) fcoo[i]=coo[i];
  
  fpar[3]=RICepsln/2.;
  fpar[7]=fpar[3];
  
  fcoo[1]+=RIClgthk/2.+RICepsln/2.;

  dummy=solid_lg->add(new AMSgvolume("VACUUM",
				     0,
				     "MIE0",
				     "TRAP",
				     fpar,
				     11,
				     fcoo,
				     nrm,
				     "ONLY", // This seems to be safe
				     0,
				     4*copia-3,
				     rel));

#ifdef __G4AMS__
  ((AMSgvolume*)dummy)->Smartless()=-2;
#endif

  fcoo[1]=coo[1]-RIClgthk/2.-RICepsln/2.;

  dummy=solid_lg->add(new AMSgvolume("VACUUM",
				     0,
				     "MIE0",
				     "TRAP",
				     fpar,
				     11,
				     fcoo,
				     nrm,
				     "ONLY", // This seems to be safe
				     0,
				     4*copia-2,
				     rel));

#ifdef __G4AMS__
  ((AMSgvolume*)dummy)->Smartless()=-2;
#endif

  coo[0]=-coo[1];
  coo[1]=0;
  
  dummy=solid_lg->add(new AMSgvolume("RICH WALLS",
				     RICnrot,
				     "MIRE",
				     "TRAP",
				       par,
				     11,
				     coo,
				     nrma,  // Rotated 90 degrees
				     "ONLY",
				     0,
				     2*copia,
				     rel));
#ifdef __G4AMS__
  ((AMSgvolume*)dummy)->Smartless()=-2;
#endif    

  fcoo[1]=0;fcoo[0]=coo[0]-RIClgthk/2.-RICepsln/2.;

  dummy=solid_lg->add(new AMSgvolume("VACUUM",
				     RICnrot,
				     "MIE0",
				     "TRAP",
				     fpar,
				     11,
				     fcoo,
				     nrma,
				     "ONLY", // This seems to be safe
				     0,
				     4*copia-1,
				     rel));

#ifdef __G4AMS__
  ((AMSgvolume*)dummy)->Smartless()=-2;
#endif

  fcoo[0]=coo[0]+RIClgthk/2.+RICepsln/2.;

  dummy=solid_lg->add(new AMSgvolume("VACUUM",
				     RICnrot,
				     "MIE0",
				     "TRAP",
				     fpar,
				     11,
				     fcoo,
				     nrma,
				     "ONLY", // This seems to be safe
				     0,
				     4*copia,
				     rel));

#ifdef __G4AMS__
  ((AMSgvolume*)dummy)->Smartless()=-2;
#endif


#else
#ifdef __AMSDEBUG__
  cout <<"RICH: Hollow LG chosen at compilation time."<<endl;
#endif
  
  
  // Hollow light guides
  
  coo[0]=0;
  coo[1]=0;
  coo[2]=RICHDB::lg_pos();
  
  par[0]=RICGEOM.light_guides_height/2;
  par[1]=RICHDB::lg_mirror_angle(1);
  par[2]=90;
  par[3]=RIClgthk/2;
  par[4]=RICcatolength/2;
  par[5]=RICcatolength/2;
  par[6]=0;
  par[7]=RIClgthk/2;
  par[8]=RICGEOM.light_guides_length/2;
  par[9]=RICGEOM.light_guides_length/2;
  par[10]=0;
  
  coo[1]=RICHDB::lg_mirror_pos(1);
  dummy=lig->add(new AMSgvolume("RICH MIRRORS",
				0,
				"MIRA",
				"TRAP",
				par,
				11,
				coo,
				nrm,
				"ONLY", // This seems to be safe
				0,
				2*copia-1,
				rel));
  
#ifdef __G4AMS__
  ((AMSgvolume*)dummy)->Smartless()=-2;
#endif
  
  coo[0]=-coo[1];
  coo[1]=0;
  
  dummy=lig->add(new AMSgvolume("RICH MIRRORS",
				RICnrot,
				"MIRA",
				"TRAP",
				par,
				11,
				coo,
				nrma,
				"ONLY",
				0,
				2*copia,
				rel));
  
#ifdef __G4AMS__
  ((AMSgvolume*)dummy)->Smartless()=-2;
#endif  


   par[0]=RICGEOM.light_guides_height/2;
   par[1]=RICHDB::lg_mirror_angle(2);
   par[2]=90;
   par[3]=RIClgthk/2;
   par[4]=RICcatolength/2;
   par[5]=RICcatolength/2;
   par[6]=0;
   par[7]=RIClgthk/2;
   par[8]=RICGEOM.light_guides_length/2;
   par[9]=RICGEOM.light_guides_length/2;
   par[10]=0;
   
   coo[0]=0;
   coo[1]=RICHDB::lg_mirror_pos(2);


   dummy=lig->add(new AMSgvolume("RICH MIRRORS",
				 0,
				 "MIRB",
				 "TRAP",
				 par,
				 11,
				 coo,
				 nrm,  
				 "ONLY",
				 0,
				 2*copia-1,
				 rel));
#ifdef __G4AMS__
     ((AMSgvolume*)dummy)->Smartless()=-2;
#endif   


   coo[0]=-coo[1];
   coo[1]=0;

   dummy=lig->add(new AMSgvolume("RICH MIRRORS",
				 RICnrot,
				 "MIRB",
				 "TRAP",
				 par,
				 11,
				 coo,
				 nrma,  // Rotated 90 degrees
				 "ONLY",
				 0,
				 2*copia,
				 rel));
   
#ifdef __G4AMS__
     ((AMSgvolume*)dummy)->Smartless()=-2;
#endif

   par[0]=RICGEOM.light_guides_height/2;;
   par[1]=0;
   par[2]=90;
   par[3]=RIClgthk/2;
   par[4]=RICcatolength/2;
   par[5]=RICcatolength/2;
   par[6]=0;
   par[7]=RIClgthk/2;
   par[8]=RICGEOM.light_guides_length/2;
   par[9]=RICGEOM.light_guides_length/2;
   par[10]=0;
   
   coo[0]=0;
   coo[1]=0;
   

   dummy=lig->add(new AMSgvolume("RICH MIRRORS",
				 0,
				 "MIRC",
				 "TRAP",
				 par,
				 11,
				 coo,
				 nrm,  // Rotated 90 degrees
				 "ONLY",
				 0,
				 2*copia-1,
				 rel));
#ifdef __G4AMS__
     ((AMSgvolume*)dummy)->Smartless()=-2;
#endif   


   dummy=lig->add(new AMSgvolume("RICH MIRRORS",
				 RICnrot,
				 "MIRC",
				 "TRAP",
				 par,
				 11,
				 coo,
				 nrma,  // Rotated 90 degrees
				 "ONLY",
				 0,
				 2*copia,
				 rel));
#ifdef __G4AMS__
   ((AMSgvolume*)dummy)->Smartless()=-2;
#endif

    par[0]=RICGEOM.light_guides_height/2;
    par[1]=RICHDB::lg_mirror_angle(1);
    par[2]=270;
    par[3]=RIClgthk/2;
    par[4]=RICcatolength/2;
    par[5]=RICcatolength/2;
    par[6]=0;
    par[7]=RIClgthk/2;
    par[8]=RICGEOM.light_guides_length/2;
    par[9]=RICGEOM.light_guides_length/2;
    par[10]=0;
    
    coo[0]=0;
    coo[1]=-RICHDB::lg_mirror_pos(1);

    dummy=lig->add(new AMSgvolume("RICH MIRRORS",
				  0,
				  "MIRD",
				  "TRAP",
				  par,
				  11,
				  coo,
				  nrm,
				  "ONLY",
				  0,
				  2*copia-1,
				  rel));
#ifdef __G4AMS__
     ((AMSgvolume*)dummy)->Smartless()=-2;
#endif


    coo[0]=-coo[1];
    coo[1]=0;

    dummy=lig->add(new AMSgvolume("RICH MIRRORS",
				  RICnrot,
				  "MIRD",
				  "TRAP",
				  par,
				  11,
				  coo,
				  nrma,
				  "ONLY",
				  0,
				  2*copia,
				  rel));
#ifdef __G4AMS__
     ((AMSgvolume*)dummy)->Smartless()=-2;
#endif


    par[0]=RICGEOM.light_guides_height/2;;
    par[1]=RICHDB::lg_mirror_angle(2);
    par[2]=270;
    par[3]=RIClgthk/2;
    par[4]=RICcatolength/2;
    par[5]=RICcatolength/2;
    par[6]=0;
    par[7]=RIClgthk/2;
    par[8]=RICGEOM.light_guides_length/2;
    par[9]=RICGEOM.light_guides_length/2;
    par[10]=0;
    
    coo[0]=0;
    coo[1]=-RICHDB::lg_mirror_pos(2);
	  

    dummy=lig->add(new AMSgvolume("RICH MIRRORS",
				  0,
				  "MIRE",
				  "TRAP",
				  par,
				  11,
				  coo,
				  nrm,  // Rotated 90 degrees
				  "ONLY",
				  0,
				  2*copia-1,
				  rel));
#ifdef __G4AMS__
     ((AMSgvolume*)dummy)->Smartless()=-2;
#endif    


    coo[0]=-coo[1];
    coo[1]=0;

    dummy=lig->add(new AMSgvolume("RICH MIRRORS",
				  RICnrot,
				  "MIRE",
				  "TRAP",
				  par,
				  11,
				  coo,
				  nrma,  // Rotated 90 degrees
				  "ONLY",
				  0,
				  2*copia,
				  rel));

#ifdef __G4AMS__
     ((AMSgvolume*)dummy)->Smartless()=-2;
#endif

#endif


}


void amsgeom::richgeom02(AMSgvolume & mother)
{
  AMSgvolume *rich;
  AMSNode *dummy;
  AMSNode *lig,*rad;
  geant par[11],coo[3];
  number nrm[3][3]={1.,0.,0.,0.,1.,0.,0.,0.,1.}; // {vx, vy, vz}
  number nrma[3][3]={0,-1,0,1,0,0,0,0,1}; // Rotated 90 degrees
  const integer rel=1; 
  integer flag=1;


  // Write the selected geometry

#ifdef __AMSDEBUG__
  cout << "RICH geometry according to December(99) meeting" <<endl;

  cout << "************** RICH GEOMETRY ***************" << endl;
  cout << "Top radius:" << RICGEOM.top_radius << endl;
  cout << "Bottom radius:" << RICGEOM.bottom_radius << endl;
  cout << "Hole radius:" << RICGEOM.hole_radius << endl;
  cout << "Height:" << RICGEOM.height << endl;
  //  cout << "Inner mirror height:" << RICGEOM.inner_mirror_height << endl;
  cout << "Radiator radius:" << RICGEOM.radiator_radius << endl;
  cout << "Radiator height:" << RICGEOM.radiator_height << endl;
  cout << "Radiator tile size:" << RICGEOM.radiator_box_length << endl;
  cout << "Light guides height:" << RICGEOM.light_guides_height << endl;
  cout << "Light guides tile size:" << RICGEOM.light_guides_length <<endl;
  cout << "******************************************" << endl;
#endif





  /// Define the RICH volume: fixed


  //  par[0]=0;
  //  par[1]=90;
  //  par[2]=RICHDB::total_height()/2;
  //  coo[0]=0;
  //  coo[1]=0;
  //  coo[2]=RICradpos-RICHDB::total_height()/2;

  //  rich=dynamic_cast<AMSgvolume*>(mother.add(new AMSgvolume("VACUUM",
  //							   0,
  //							   "RICH",
  //							   "TUBE",
  //							   par,
  //							   3,
  //							   coo,
  //							   nrm,
  //							   "ONLY",
  //							   0,
  //							   1,
  //							   rel)));
  

  // This is provisional


  par[0]=RICHDB::total_height()/2.;
  par[1]=0.;
  par[2]=RICHDB::total_height()*(RICHDB::bottom_radius-RICHDB::top_radius)/
    RICHDB::height+RICHDB::top_radius+1.;
  par[3]=0.;
  par[4]=RICHDB::top_radius+1.;

  coo[0]=0;
  coo[1]=0;
  coo[2]=RICradpos-RICHDB::total_height()/2;
  rich=dynamic_cast<AMSgvolume*>(mother.add(new AMSgvolume("VACUUM",
							   0,
							   "RICH",
							   "CONE",
  							   par,
  							   5,
  							   coo,
  							   nrm,
  							   "ONLY",
  							   0,
  							   1,
  							   rel)));
  
#ifdef __G4AMS__
  rich->Smartless()=-2;
#endif


  ////////// Mirror

  par[0]=RICGEOM.height/2;
  par[1]=RICGEOM.bottom_radius;
  par[2]=par[1]+RICmithk;
  par[3]=RICGEOM.top_radius; 
  par[4]=par[3]+RICmithk;

  coo[2]=RICHDB::mirror_pos();

  dummy=rich->add(new AMSgvolume("RICH MIRROR",  // Material
				 0,          // No rotation
				 "OMIR",     // Name 
				 "CONE",    // Shape
				 par,       // Geant parameters
				 5,         // # of parameters
				 coo,       // coordinates 
				 nrm,       // Matrix of normals
				 "ONLY",    
				 0,
				 1,
				 rel));
  
  




  //// Radiator


  geant xedge=RICGEOM.radiator_box_length/2,
    yedge=RICGEOM.radiator_box_length/2,
    lg,
    cl;
  integer copia=1;

  while(yedge-RICGEOM.radiator_box_length/2<RICGEOM.radiator_radius){
    cl=SQR(xedge-RICGEOM.radiator_box_length/2)+SQR(yedge-RICGEOM.radiator_box_length/2);

    coo[2]=RICHDB::rad_pos();
      
    if(cl<SQR(RICGEOM.radiator_radius)){
      for(int i=0;i<4;i++){
	
	coo[0]=xedge*(1-2*(i%2));
	coo[1]=yedge*(1-2*(i>1));
	
	par[0]=RICGEOM.radiator_box_length/2;
	par[1]=RICGEOM.radiator_box_length/2;
	par[2]=RICGEOM.radiator_height/2;
	
	rad=rich->add(new AMSgvolume("RICH CARBON",
				     0,
				     "RADB",
				     "BOX",
				     par,
				     3,
				     coo,
				     nrm,
				     "ONLY",
				     0,
				     copia++,
				     rel));
	
#ifdef __G4AMS__
	if(MISCFFKEY.G4On)
	  Put_rad((AMSgvolume *)rad,copia-1);
#endif



      }
    
    xedge+=RICGEOM.radiator_box_length;}else{
      yedge+=RICGEOM.radiator_box_length;
      xedge=RICGEOM.radiator_box_length/2;
    }
  }
  
 
#ifdef __G4AMS__
  if(MISCFFKEY.G3On){
#endif
 
    Put_rad((AMSgvolume *)rad,1);

#ifdef __G4AMS__
  }
#endif


  /// PMT support structure

  coo[0]=0;
  coo[1]=0;
  coo[2]=RICHDB::pmt_pos();  

  par[0]=45;
  par[1]=360;
  par[2]=4;
  par[3]=2;
  par[4]=-((RICpmtlength+RICeleclength)/2+RICGEOM.light_guides_height/2);
  par[5]=RICGEOM.hole_radius;
  par[6]=RICGEOM.hole_radius+RICpmtsupport;
  par[7]=((RICpmtlength+RICeleclength)/2+RICGEOM.light_guides_height/2);
  par[8]=RICGEOM.hole_radius;
  par[9]=RICGEOM.hole_radius+RICpmtsupport;
   

  dummy=rich->add(new AMSgvolume("RICH CARBON",
				 0,
				 "PMS1",
				 "PGON",
				 par,
				 10,
				 coo,
				 nrm,
				 "ONLY",
				 0,
				 1,
				 rel));
  

  par[0]=(RICGEOM.bottom_radius-RICGEOM.hole_radius-RICpmtsupport)/2;
  par[1]=RICpmtsupport/2;
  par[2]=(RICpmtlength+RICeleclength)/2+RICGEOM.light_guides_height/2;
  

//NEW!

geant xpos,ypos;

  xpos=RICGEOM.bottom_radius/2+RICGEOM.hole_radius/2+RICpmtsupport/2;
  ypos=RICGEOM.hole_radius+RICpmtsupport/2.;

  coo[0]=xpos;
  coo[1]=ypos;
  coo[2]=RICHDB::pmt_pos();

  dummy=rich->add(new AMSgvolume("RICH CARBON",
				 0,
				 "PMS2",
				 "BOX",
				 par,
				 3,
				 coo,
				 nrm,
				 "ONLY",
				 0,
				 1,
				 rel));

  coo[0]=-xpos;

  dummy=rich->add(new AMSgvolume("RICH CARBON",
				 0,
				 "PMS2",
				 "BOX",
				 par,
				 3,
				 coo,
				 nrm,
				 "ONLY",
				 0,
				 2,
				 rel));

  coo[0]=xpos;coo[1]=-ypos;

  dummy=rich->add(new AMSgvolume("RICH CARBON",
				 0,
				 "PMS2",
				 "BOX",
				 par,
				 3,
				 coo,
				 nrm,
				 "ONLY",
				 0,
				 3,
				 rel));

  coo[0]=-xpos;

  dummy=rich->add(new AMSgvolume("RICH CARBON",
				 0,
				 "PMS2",
				 "BOX",
				 par,
				 3,
				 coo,
				 nrm,
				 "ONLY",
				 0,
				 4,
				 rel));
 

  coo[0]=ypos;coo[1]=xpos;

  dummy=rich->add(new AMSgvolume("RICH CARBON",
				 RICnrot,
				 "PMS2",
				 "BOX",
				 par,
				 3,
				 coo,
				 nrma,
				 "ONLY",
				 0,
				 5,
				 rel));


  coo[0]=-ypos;

  dummy=rich->add(new AMSgvolume("RICH CARBON",
				 RICnrot,
				 "PMS2",
				 "BOX",
				 par,
				 3,
				 coo,
				 nrma,
				 "ONLY",
				 0,
				 6,
				 rel));
  
  coo[0]=ypos;coo[1]=-xpos;

  dummy=rich->add(new AMSgvolume("RICH CARBON",
				 RICnrot,
				 "PMS2",
				 "BOX",
				 par,
				 3,
				 coo,
				 nrma,
				 "ONLY",
				 0,
				 7,
				 rel));

  coo[0]=-ypos;

  dummy=rich->add(new AMSgvolume("RICH CARBON",
				 RICnrot,
				 "PMS2",
				 "BOX",
				 par,
				 3,
				 coo,
				 nrma,
				 "ONLY",
				 0,
				 8,
				 rel));

  ///// Positioning PMTs


  copia=1;
  
  // The PMt array is made of two kind of blocks.
  // Put the first one

  xedge=RICGEOM.hole_radius+RICGEOM.light_guides_length/2+RICpmtsupport;
  yedge=RICGEOM.hole_radius+
      (1-2*RICHDB::n_rows[0])*RICGEOM.light_guides_length/2;//NEW!

  for(int i=0;i<RICHDB::n_rows[0];i++)
    for(int j=0;j<RICHDB::n_pmts[i][0];j++){

      coo[2]=RICHDB::pmt_pos();
      
      par[0]=RICGEOM.light_guides_length/2;
      par[1]=RICGEOM.light_guides_length/2;
      par[2]=RICHDB::pmtb_height()/2; //NEW!

      coo[0]=xedge+(j+RICHDB::offset[i][0])*RICGEOM.light_guides_length;
      coo[1]=yedge+i*RICGEOM.light_guides_length;
      

      RICHDB::pmt_p[copia-1][0]=coo[0];
      RICHDB::pmt_p[copia-1][1]=coo[1];

      lig=rich->add(new AMSgvolume("VACUUM",
				   0,
				   "PMTB",
				   "BOX",
				   par,
				   3,
				   coo,
				   nrm,
				   "ONLY",
				   0,
				   copia++,
				   rel));
#ifdef __G4AMS__
      if(MISCFFKEY.G4On)
	Put_pmt((AMSgvolume *)lig,copia-1);
#endif
      
      coo[0]*=-1;

      RICHDB::pmt_p[copia-1][0]=coo[0];
      RICHDB::pmt_p[copia-1][1]=coo[1];

      lig=rich->add(new AMSgvolume("VACUUM",
				   0,
				   "PMTB",
				   "BOX",
				   par,
				   3,
				   coo,
				   nrm,
				   "ONLY",
				   0,
				   copia++,
				   rel));

      
#ifdef __G4AMS__
      if(MISCFFKEY.G4On)
	Put_pmt((AMSgvolume *)lig,copia-1);
#endif

      coo[1]*=-1;

      RICHDB::pmt_p[copia-1][0]=coo[0];
      RICHDB::pmt_p[copia-1][1]=coo[1];

      lig=rich->add(new AMSgvolume("VACUUM",
				   0,
				   "PMTB",
				   "BOX",
				   par,
				   3,
				   coo,
				   nrm,
				   "ONLY",
				   0,
				   copia++,
				   rel));


#ifdef __G4AMS__
      if(MISCFFKEY.G4On)
	Put_pmt((AMSgvolume *)lig,copia-1);
#endif


      coo[0]*=-1;

      RICHDB::pmt_p[copia-1][0]=coo[0];
      RICHDB::pmt_p[copia-1][1]=coo[1];

      lig=rich->add(new AMSgvolume("VACUUM",
				   0,
				   "PMTB",
				   "BOX",
				   par,
				   3,
				   coo,
				   nrm,
				   "ONLY",
				   0,
				   copia++,
				   rel));


#ifdef __G4AMS__
      if(MISCFFKEY.G4On)
	Put_pmt((AMSgvolume *)lig,copia-1);
#endif



      coo[0]=yedge+i*RICGEOM.light_guides_length;
      coo[1]=xedge+(j+RICHDB::offset[i][0])*RICGEOM.light_guides_length;

      RICHDB::pmt_p[copia-1][0]=coo[0];
      RICHDB::pmt_p[copia-1][1]=coo[1];

      lig=rich->add(new AMSgvolume("VACUUM",
				   0,
				   "PMTB",
				   "BOX",
				   par,
				   3,
				   coo,
				   nrm,
				   "ONLY",
				   0,
				   copia++,
				   rel));


#ifdef __G4AMS__
      if(MISCFFKEY.G4On)
	Put_pmt((AMSgvolume *)lig,copia-1);
#endif
      
      coo[0]*=-1;

      RICHDB::pmt_p[copia-1][0]=coo[0];
      RICHDB::pmt_p[copia-1][1]=coo[1];

      lig=rich->add(new AMSgvolume("VACUUM",
				   0,
				   "PMTB",
				   "BOX",
				   par,
				   3,
				   coo,
				   nrm,
				   "ONLY",
				   0,
				   copia++,
				   rel));


#ifdef __G4AMS__
      if(MISCFFKEY.G4On)
	Put_pmt((AMSgvolume *)lig,copia-1);
#endif

      coo[1]*=-1;

      RICHDB::pmt_p[copia-1][0]=coo[0];
      RICHDB::pmt_p[copia-1][1]=coo[1];

      lig=rich->add(new AMSgvolume("VACUUM",
				   0,
				   "PMTB",
				   "BOX",
				   par,
				   3,
				   coo,
				   nrm,
				   "ONLY",
				   0,
				   copia++,
				   rel));


#ifdef __G4AMS__
      if(MISCFFKEY.G4On)
	Put_pmt((AMSgvolume *)lig,copia-1);
#endif


      coo[0]*=-1;

      RICHDB::pmt_p[copia-1][0]=coo[0];
      RICHDB::pmt_p[copia-1][1]=coo[1];
      
      lig=rich->add(new AMSgvolume("VACUUM",
				   0,
				   "PMTB",
				   "BOX",
				   par,
				   3,
				   coo,
				   nrm,
				   "ONLY",
				   0,
				   copia++,
				   rel));



#ifdef __G4AMS__
      if(MISCFFKEY.G4On)
	Put_pmt((AMSgvolume *)lig,copia-1);
#endif

    }



  //  Put the other block

//NEW!
  xedge=RICGEOM.hole_radius+RICGEOM.light_guides_length/2+RICpmtsupport;
  yedge=RICGEOM.hole_radius+RICGEOM.light_guides_length/2+RICpmtsupport;

  for(int i=0;i<RICHDB::n_rows[1];i++)
    for(int j=0;j<RICHDB::n_pmts[i][1];j++){
      
      coo[0]=xedge+(j+RICHDB::offset[i][1])*RICGEOM.light_guides_length;
      coo[1]=yedge+i*RICGEOM.light_guides_length;

      RICHDB::pmt_p[copia-1][0]=coo[0];
      RICHDB::pmt_p[copia-1][1]=coo[1];

      lig=rich->add(new AMSgvolume("VACUUM",
				   0,
				   "PMTB",
				   "BOX",
				   par,
				   3,
				   coo,
				   nrm,
				   "ONLY",
				   0,
				   copia++,
				   rel));



#ifdef __G4AMS__
      if(MISCFFKEY.G4On)
	Put_pmt((AMSgvolume *)lig,copia-1);
#endif
      
      coo[0]*=-1;

      RICHDB::pmt_p[copia-1][0]=coo[0];
      RICHDB::pmt_p[copia-1][1]=coo[1];

      lig=rich->add(new AMSgvolume("VACUUM",
				   0,
				   "PMTB",
				   "BOX",
				   par,
				   3,
				   coo,
				   nrm,
				   "ONLY",
				   0,
				   copia++,
				   rel));


#ifdef __G4AMS__
      if(MISCFFKEY.G4On)
	Put_pmt((AMSgvolume *)lig,copia-1);
#endif

      coo[1]*=-1;

      RICHDB::pmt_p[copia-1][0]=coo[0];
      RICHDB::pmt_p[copia-1][1]=coo[1];

      lig=rich->add(new AMSgvolume("VACUUM",
				   0,
				   "PMTB",
				   "BOX",
				   par,
				   3,
				   coo,
				   nrm,
				   "ONLY",
				   0,
				   copia++,
				   rel));


#ifdef __G4AMS__
      if(MISCFFKEY.G4On)
	Put_pmt((AMSgvolume *)lig,copia-1);
#endif


      coo[0]*=-1;

      RICHDB::pmt_p[copia-1][0]=coo[0];
      RICHDB::pmt_p[copia-1][1]=coo[1];

      lig=rich->add(new AMSgvolume("VACUUM",
				   0,
				   "PMTB",
				   "BOX",
				   par,
				   3,
				   coo,
				   nrm,
				   "ONLY",
				   0,
				   copia++,
				   rel));


#ifdef __G4AMS__
      if(MISCFFKEY.G4On)
	Put_pmt((AMSgvolume *)lig,copia-1);
#endif
    }

  
 
#ifdef __G4AMS__
  if(MISCFFKEY.G3On)
#endif
    Put_pmt((AMSgvolume *)lig,1);

  RICHDB::total=copia-1;

  cout<< "amsgeom::RICH geometry finished-> " << RICHDB::total << " PMTs found." << endl;

}









