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

  par[0]=RICGEOM.radiator_box_length/2-RICmithk/2;
  par[1]=RICGEOM.radiator_box_length/2-RICmithk/2;
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
  geant par[11],coo[3];
  number nrm[3][3]={1.,0.,0.,0.,1.,0.,0.,0.,1.}; // {vx, vy, vz}
  number nrma[3][3]={0,-1,0,1,0,0,0,0,1}; // Rotated 90 degrees
  const integer rel=1; 
  integer flag=1;

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
  par[8]=RICGEOM.light_guides_length/2-RICpmtshield;
  par[9]=RICGEOM.light_guides_length/2-RICpmtshield;
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
   par[8]=RICGEOM.light_guides_length/2-RICpmtshield;
   par[9]=RICGEOM.light_guides_length/2-RICpmtshield;
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
   par[8]=RICGEOM.light_guides_length/2-RICpmtshield;
   par[9]=RICGEOM.light_guides_length/2-RICpmtshield;
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
    par[8]=RICGEOM.light_guides_length/2-RICpmtshield;
    par[9]=RICGEOM.light_guides_length/2-RICpmtshield;
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
    par[8]=RICGEOM.light_guides_length/2-RICpmtshield;
    par[9]=RICGEOM.light_guides_length/2-RICpmtshield;
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


  par[0]=0;
  par[1]=90;
  par[2]=RICHDB::total_height()/2;
  coo[0]=0;
  coo[1]=0;
  coo[2]=RICradpos-RICHDB::total_height()/2;

  rich=dynamic_cast<AMSgvolume*>(mother.add(new AMSgvolume("VACUUM",
							   0,
							   "RICH",
							   "TUBE",
							   par,
							   3,
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

  dummy=rich->add(new AMSgvolume("RICH MIRRORS",  // Material
				 0,         // No rotation
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
  
  

  ///////////////////




  //// Radiator


  geant xedge=RICGEOM.radiator_box_length/2,
    yedge=RICGEOM.radiator_box_length/2,
    lg,
    cl;
  integer copia=1;

  
  while(yedge<RICGEOM.radiator_radius){
    
    cl=SQR(xedge-RICGEOM.radiator_box_length/2)+SQR(yedge-RICGEOM.radiator_box_length/2);
    

    coo[2]=RICHDB::rad_pos();
      
    if(cl<SQR(RICGEOM.radiator_radius)) // Put a PMT here
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
    
    xedge+=RICGEOM.radiator_box_length;
    
    if(xedge>RICGEOM.radiator_radius) {
      xedge=RICGEOM.radiator_box_length/2;
      yedge+=RICGEOM.radiator_box_length;}


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
  par[4]=(RICpmtlength+RICeleclength)/2+RICGEOM.light_guides_height/2;
  par[5]=RICGEOM.hole_radius;
  par[6]=RICGEOM.hole_radius+RICpmtsupport;
  par[7]=-((RICpmtlength+RICeleclength)/2+RICGEOM.light_guides_height/2);
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
  
  coo[0]=RICGEOM.bottom_radius/2+RICGEOM.hole_radius/2+RICpmtsupport/2;
  coo[1]=0;
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

  coo[0]*=-1;

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

  coo[1]=RICGEOM.hole_radius+RICpmtsupport/2;

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

  coo[0]*=-1;

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
 
  coo[1]*=-1;

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
				 5,
				 rel));

  coo[0]*=-1;

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
				 6,
				 rel));

  coo[0]=0;
  coo[1]=RICGEOM.bottom_radius/2+RICGEOM.hole_radius/2+RICpmtsupport/2;

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


  coo[1]*=-1;

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
  
  coo[0]=RICGEOM.hole_radius+RICpmtsupport/2;

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
				 9,
				 rel));

  coo[1]*=-1;

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
				 10,
				 rel));

  coo[0]*=-1;

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
				 11,
				 rel));
  

  coo[1]*=-1;

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
				 12,
				 rel));


  ///// Positioning PMTs


  copia=1;
  
  // The PMt array is made of two kind of blocks.
  // Put the first one

  xedge=RICGEOM.hole_radius+RICGEOM.light_guides_length/2+RICpmtsupport+RICpmtshield/2;
  yedge=RICGEOM.light_guides_length/2+RICpmtsupport/2+RICpmtshield/2;

  for(int i=0;i<RICHDB::n_rows[0];i++)
    for(int j=0;j<RICHDB::n_pmts[i][0];j++){

      coo[2]=RICHDB::pmt_pos();
      
      par[0]=45;
      par[1]=360;
      par[2]=4;
      par[3]=2;
      par[4]=(RICpmtlength+RICeleclength)/2+RICGEOM.light_guides_height/2;
      par[5]=(RICGEOM.light_guides_length/2-RICpmtshield);
      par[6]=RICGEOM.light_guides_length/2;
      par[7]=-((RICpmtlength+RICeleclength)/2+RICGEOM.light_guides_height/2);
      par[8]=(RICGEOM.light_guides_length/2-RICpmtshield);
      par[9]=RICGEOM.light_guides_length/2;
      
      coo[0]=xedge+j*RICGEOM.light_guides_length;
      coo[1]=yedge+i*RICGEOM.light_guides_length;
      

      RICHDB::pmt_p[copia-1][0]=coo[0];
      RICHDB::pmt_p[copia-1][1]=coo[1];

      lig=rich->add(new AMSgvolume("RICH SHIELD",
				   0,
				   "PMTB",
				   "PGON",
				   par,
				   10,
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

      lig=rich->add(new AMSgvolume("RICH SHIELD",
				   0,
				   "PMTB",
				   "PGON",
				   par,
				   10,
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

      lig=rich->add(new AMSgvolume("RICH SHIELD",
				   0,
				   "PMTB",
				   "PGON",
				   par,
				   10,
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

      lig=rich->add(new AMSgvolume("RICH SHIELD",
				   0,
				   "PMTB",
				   "PGON",
				   par,
				   10,
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
      coo[1]=xedge+j*RICGEOM.light_guides_length;

      RICHDB::pmt_p[copia-1][0]=coo[0];
      RICHDB::pmt_p[copia-1][1]=coo[1];

      lig=rich->add(new AMSgvolume("RICH SHIELD",
				   0,
				   "PMTB",
				   "PGON",
				   par,
				   10,
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

      lig=rich->add(new AMSgvolume("RICH SHIELD",
				   0,
				   "PMTB",
				   "PGON",
				   par,
				   10,
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

      lig=rich->add(new AMSgvolume("RICH SHIELD",
				   0,
				   "PMTB",
				   "PGON",
				   par,
				   10,
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

      lig=rich->add(new AMSgvolume("RICH SHIELD",
				   0,
				   "PMTB",
				   "PGON",
				   par,
				   10,
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

  xedge=RICGEOM.hole_radius+RICGEOM.light_guides_length/2+RICpmtsupport+RICpmtshield/2;
  yedge=RICGEOM.hole_radius+RICGEOM.light_guides_length/2+RICpmtsupport+RICpmtshield/2;

  for(int i=0;i<RICHDB::n_rows[1];i++)
    for(int j=0;j<RICHDB::n_pmts[i][1];j++){
      
      coo[0]=xedge+j*RICGEOM.light_guides_length;
      coo[1]=yedge+i*RICGEOM.light_guides_length;

      RICHDB::pmt_p[copia-1][0]=coo[0];
      RICHDB::pmt_p[copia-1][1]=coo[1];

      lig=rich->add(new AMSgvolume("RICH SHIELD",
				   0,
				   "PMTB",
				   "PGON",
				   par,
				   10,
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

      lig=rich->add(new AMSgvolume("RICH SHIELD",
				   0,
				   "PMTB",
				   "PGON",
				   par,
				   10,
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

      lig=rich->add(new AMSgvolume("RICH SHIELD",
				   0,
				   "PMTB",
				   "PGON",
				   par,
				   10,
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

      lig=rich->add(new AMSgvolume("RICH SHIELD",
				   0,
				   "PMTB",
				   "PGON",
				   par,
				   10,
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









