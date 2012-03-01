//  $Id: richgeom.C,v 1.52 2012/03/01 17:00:09 mdelgado Exp $
#include "gmat.h"
#include "gvolume.h"
#include "commons.h"
#include "richdbc.h"
#include "richid.h"
#include "richradid.h"

#define SQR(x) ((x)*(x))

#define RICHVERSION "2008"

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
extern void richgeom02(AMSgvolume &, float zshift=0);
extern void Put_rad(AMSgvolume *,integer,int);
extern void Put_pmt(AMSgvolume *,integer);
};


using namespace amsgeom;


// Some tweaks for different versions
#ifdef __G4AMS__
// THIS SHOULD SOLVE SOME ISSUES WITH G4, BUT PROBABLY BRING SOME NEW ONES
#define _MANY_ "ONLY"     
#else
#define _MANY_ "MANY"
#endif

#define _VACUUM_ "RICH VACUUM"


void amsgeom::Put_rad(AMSgvolume * mother,integer copia,int tile)
{
  AMSNode *dummy;
  geant par[3],coo[3];
  number nrm[3][3]={1.,0.,0.,0.,1.,0.,0.,0.,1.}; // {vx, vy, vz} 
  number rm45[3][3]={1/sqrt(2.),1/sqrt(2.),0.,
                     -1/sqrt(2.),1/sqrt(2.),0,
                     0,0,1};
  const integer rel=1;
  int kind=RichRadiatorTileManager::get_tile_kind(tile);

  if(kind!=agl_kind) return; // NAF is done in another way
  
  par[0]=RichRadiatorTileManager::get_tile_boundingbox(tile,0);
  par[1]=RichRadiatorTileManager::get_tile_boundingbox(tile,1);
  par[2]=RichRadiatorTileManager::get_tile_boundingbox(tile,2);
  coo[0]=0;
  coo[1]=0;
  coo[2]=RichRadiatorTileManager::get_tile_boundingbox(tile,2)-RICHDB::rad_height/2.;    
  
  //
  // It is important not to change this volumen name because it is used in gtckov to identify agl
  //    
  
  AMSgvolume *b=(AMSgvolume*)mother->add(new AMSgvolume("RICH RAD",
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
  
  // Add the screws
  par[0]=0.8/sqrt(2.);
  par[1]=0.8/sqrt(2.);
  par[2]=RichRadiatorTileManager::get_tile_boundingbox(tile,2);
  coo[0]=RichRadiatorTileManager::get_tile_boundingbox(tile,0);
  coo[1]=RichRadiatorTileManager::get_tile_boundingbox(tile,1);
  coo[2]=0;


  b->add(new AMSgvolume("RICH PORON",
			1,
			"RSCR",
			"BOX",
			par,
			3,
			coo,
			rm45,
			"ONLY", 
			0,
			10*copia+1,
			rel));
  
  
  coo[0]=-RichRadiatorTileManager::get_tile_boundingbox(tile,0);
  coo[1]=RichRadiatorTileManager::get_tile_boundingbox(tile,1);
  coo[2]=0;
  
  b->add(new AMSgvolume("RICH PORON",
			1,
			"RSCR",
			"BOX",
			par,
			3,
			coo,
			rm45,
			"ONLY",
			0,
			10*copia+2,
			rel));
  
  coo[0]=RichRadiatorTileManager::get_tile_boundingbox(tile,0);
  coo[1]=-RichRadiatorTileManager::get_tile_boundingbox(tile,1);
  coo[2]=0;
  
  b->add(new AMSgvolume("RICH PORON",
			1,
			"RSCR",
			"BOX",
			par,
			3,
			coo,
			rm45,
			"ONLY",
			0,
			10*copia+3,
			rel));
  
  coo[0]=-RichRadiatorTileManager::get_tile_boundingbox(tile,0);
  coo[1]=-RichRadiatorTileManager::get_tile_boundingbox(tile,1);
  coo[2]=0;
  
  b->add(new AMSgvolume("RICH PORON",
			1,
			"RSCR",
			"BOX",
			par,
			3,
			coo,
			rm45,
			"ONLY",
			0,
			10*copia+4,
			rel));
}


void amsgeom::Put_pmt(AMSgvolume * lig,integer copia)
{
  AMSNode *dummy;
  AMSgvolume *solid_lg;
  AMSgvolume *wall;
  geant par[13],coo[3],fcoo[3],fpar[11];
  number nrm[3][3]={1.,0.,0.,0.,1.,0.,0.,0.,1.}; // {vx, vy, vz}
  number nrm90[3][3]={0,-1,0,1,0,0,0,0,1};
  number nrm180[3][3]={-1,0,0,0,-1,0,0,0,1};
  number nrm270[3][3]={0,1,0,-1,0,0,0,0,1};
  const integer rel=1; 
  integer flag=1;

// Shielding

  par[0]=45;
  par[1]=360;
  par[2]=4;
  par[3]=2;
  par[4]=-RICshiheight/2;
  par[5]=PMT_electronics/2.;
  par[6]=par[5]+RICpmtshield/2.;
  par[7]=RICshiheight/2;
  par[8]=par[5];
  par[9]=par[6];


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

  par[0]=PMT_electronics/2.;
  par[1]=PMT_electronics/2.;
  par[2]=(RICpmtlength-RICotherthk)/2; 
  
  coo[0]=0;
  coo[1]=0;
  coo[2]=-RICHDB::elec_pos();
  
  
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
  coo[2]=-RICHDB::cato_pos();

#ifdef __G4AMS__ 
  // Solve a rounding problem
  coo[2]+=RICepsln;
  par[2]+=RICepsln/2;
#endif

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



  // LG definition

  coo[0]=0;
  coo[1]=0;
  coo[2]=-RICHDB::lg_pos()+RICHDB::lg_height/2.+RICpmtfoil/2;

  par[0]=RICHDB::lg_length/2.;
  par[1]=RICHDB::lg_length/2.;
  par[2]=RICpmtfoil/2;
				   
  solid_lg=(AMSgvolume*)lig->add(new AMSgvolume("HESAGLASS",
				   0,
				   "PMTV",
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
  solid_lg->Smartless()=-2;
#endif

  coo[0]=0;
  coo[1]=0;
  coo[2]=-RICHDB::lg_pos();

  par[0]=45.;
  par[1]=360.;
  par[2]=4.;
  par[3]=2.;
  par[4]=-RICHDB::lg_height/2.;   // OK 3.0/2
  par[5]=0.;
  par[6]=RICHDB::lg_bottom_length/2.+RIClgthk_bot;  // OK  1.77/2+0.07
  par[7]=RICHDB::lg_height/2.;
  par[8]=0.;
  par[9]=RICHDB::lg_length/2.;

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

  par[0]=RICHDB::lg_height/2.;
  par[1]=0.;
  par[2]=90;
  par[3]=RIClgthk_bot/2.;
  par[4]=RICHDB::lg_bottom_length/2.+RIClgthk_bot;
  par[5]=par[4];
  par[6]=0;                          
  par[7]=RIClgthk_top/2.;
  par[8]=RICHDB::lg_length/2.;
  par[9]=par[8];                     
  par[10]=0;
                                     
  coo[0]=0;
  coo[1]=0;
  coo[2]=0;


  wall=(AMSgvolume*)solid_lg->add(new AMSgvolume(_VACUUM_,
						 0,
						 "MIRC",
						 "TRAP",
						 par,
						 11,
						 coo,
						 nrm,
#ifdef __G4AMS__
						 "BOOL",
#else
						 _MANY_,
#endif
						 0,
						 4*copia-3,
						 rel));
#ifdef __G4AMS__
  wall->Smartless()=-2;
#endif

#ifdef __G4AMS__
  wall->addboolean("TRAP",par,11,coo,nrm90,'+');
#else
  dummy=solid_lg->add(new AMSgvolume(_VACUUM_,
                                     RICnrot,
                                     "MIRC",
                                     "TRAP",
                                     par,
                                     11,
                                     coo,
                                     nrm90,
				     _MANY_,
                                     0,
                                     4*copia-2,
                                     rel));
#endif



  par[0]=RICHDB::lg_height/2.;
  par[1]=RICHDB::lg_mirror_angle(1);
  par[2]=90;
  par[3]=RIClgthk_bot/2.;
  par[4]=RICHDB::lg_bottom_length/2.+RIClgthk_bot;
  par[5]=par[4];
  par[6]=0;
  par[7]=RICepsln/2.;  // This is a patch for G4 -- originally it was 0
  par[8]=RICHDB::lg_length/2.;
  par[9]=par[8];
  par[10]=0;

  coo[0]=0;
  coo[1]=RICHDB::lg_mirror_pos(1);
  coo[2]=0;
  

#ifdef __G4AMS__
  wall->addboolean("TRAP",par,11,coo,nrm,'+');
#else
  dummy=solid_lg->add(new AMSgvolume(_VACUUM_,
                                     0,
                                     "MIRA",
                                     "TRAP",
                                     par,
                                     11, 
                                     coo,
                                     nrm,
				     _MANY_,
                                     0,
                                     4*copia-3,
                                     rel));
#endif


  coo[0]=-RICHDB::lg_mirror_pos(1);
  coo[1]=0;
  

#ifdef __G4AMS__
  wall->addboolean("TRAP",par,11,coo,nrm90,'+');
#else
  dummy=solid_lg->add(new AMSgvolume(_VACUUM_,
                                     RICnrot,     
                                     "MIRA",
                                     "TRAP",
                                     par,
                                     11,
                                     coo,
                                     nrm90,
				     _MANY_,
                                     0,
                                     4*copia-2,
                                     rel));
#endif  
 


  coo[0]=0;
  coo[1]=-RICHDB::lg_mirror_pos(1); 
                                   
#ifdef __G4AMS__
  wall->addboolean("TRAP",par,11,coo,nrm180,'+');
#else
  dummy=solid_lg->add(new AMSgvolume(_VACUUM_,
                                     RICnrot+2,
                                     "MIRA",
                                     "TRAP",
                                     par,
                                     11,
                                     coo,   
                                     nrm180, 
				     _MANY_,
                                     0,  
                                     4*copia-1,
                                     rel));
#endif                                     
                                     


  coo[0]=RICHDB::lg_mirror_pos(1);
  coo[1]=0;

#ifdef __G4AMS__
  wall->addboolean("TRAP",par,11,coo,nrm270,'+');
#else
  dummy=solid_lg->add(new AMSgvolume(_VACUUM_,
                                     RICnrot+3,
                                     "MIRA",
                                     "TRAP",
                                     par,
                                     11,
                                     coo,
                                     nrm270,
				     _MANY_,
                                     0,
                                     4*copia,
                                     rel));
#endif                                        
                                     
                                     
  par[0]=RICHDB::lg_height/2.;
  par[1]=RICHDB::lg_mirror_angle(2);
  par[2]=90;                         
  par[3]=RIClgthk_bot/2.;
  par[4]=RICHDB::lg_bottom_length/2.+RIClgthk_bot;
  par[5]=par[4];                     
  par[6]=0;
  par[7]=RIClgthk_top/2.;                          
  par[8]=RICHDB::lg_length/2.;
  par[9]=par[8];
  par[10]=0;                         
  
  coo[0]=0;
  coo[1]=RICHDB::lg_mirror_pos(2);
  coo[2]=0;

#ifdef __G4AMS__
  wall->addboolean("TRAP",par,11,coo,nrm,'+');
#else
  dummy=solid_lg->add(new AMSgvolume(_VACUUM_,
                                     0,     
                                     "MIRB",
                                     "TRAP",
                                     par,
                                     11,
                                     coo,
                                     nrm,
				     _MANY_,
                                     0,
                                     4*copia-3,
                                     rel));
#endif

  coo[0]=-RICHDB::lg_mirror_pos(2);
  coo[1]=0;                          

#ifdef __G4AMS__
  wall->addboolean("TRAP",par,11,coo,nrm90,'+');
#else
  dummy=solid_lg->add(new AMSgvolume(_VACUUM_,
                                     RICnrot,
                                     "MIRB",
                                     "TRAP",
                                     par,
                                     11,
                                     coo,
                                     nrm90, 
				     _MANY_,
                                     0,  
                                     4*copia-2,
                                     rel));

#endif

  coo[0]=0;
  coo[1]=-RICHDB::lg_mirror_pos(2);  

#ifdef __G4AMS__
  wall->addboolean("TRAP",par,11,coo,nrm180,'+');
#else
  dummy=solid_lg->add(new AMSgvolume(_VACUUM_,
                                     RICnrot+2,
                                     "MIRB",
                                     "TRAP",
                                     par,
                                     11,
                                     coo,   
                                     nrm180,
				     _MANY_,
                                     0, 
                                     4*copia-1,
                                     rel));
#endif

  coo[0]=RICHDB::lg_mirror_pos(2);
  coo[1]=0;
  
#ifdef __G4AMS__
  wall->addboolean("TRAP",par,11,coo,nrm270,'+');
#else
  dummy=solid_lg->add(new AMSgvolume(_VACUUM_,
                                     RICnrot+3,
                                     "MIRB",
                                     "TRAP",
                                     par,
                                     11,
                                     coo,
                                     nrm270,
				     _MANY_,
                                     0, 
                                     4*copia,
                                     rel));
#endif   
                                       

}


void amsgeom::richgeom02(AMSgvolume & mother, float ZShift)
{
  AMSgvolume *rich;
  AMSNode *dummy;
  AMSNode *lig,*rad;
  geant par[20],coo[3];
  number nrm[3][3]={1.,0.,0.,0.,1.,0.,0.,0.,1.}; // {vx, vy, vz}
  number nrma[3][3]={0,-1,0,1,0,0,0,0,1}; // Rotated 90 degrees
  const integer rel=1; 
  integer flag=1;


  // Write the selected geometry
#ifdef __AMSDEBUG__
  cout <<"RICH geometry version dated:"<<RICHVERSION<<endl;
  RICHDB::dump();
#endif

  // This is not so provisional
  par[0]=RICHDB::total_height()/2.;
  par[1]=0.;
  par[2]=RICHDB::top_radius+1.+sqrt(2.)*RICHDB::rad_length;
  par[3]=0.;
  par[4]=RICHDB::top_radius+1.+sqrt(2.)*RICHDB::rad_length;


  coo[0]=0;
  coo[1]=0;
  RICHDB::RICradpos()+=ZShift;
  coo[2]=RICHDB::RICradpos()-RICHDB::total_height()/2;


  // Misalignment
  if(!(AMSJob::gethead()->isRealData())){
    if(strstr(AMSJob::gethead()->getsetup(),"AMS02P") && !RichAlignment::IgnoreDB()){
      // Force the expected misalignment
      RichAlignment::Set(-0.11,0.08,0.93,0,0,0);
      RichAlignment::SetMirrorShift(0,0,0);
    }
    
    AMSPoint test_point(0,0,0);
    test_point=RichAlignment::RichToAMS(test_point);
    coo[0]+=test_point[0];
    coo[1]+=test_point[1];
    coo[2]+=test_point[2];
    // Force angular parameters to zero
    RichAlignment::Set(-test_point[0],-test_point[1],-test_point[2],0,0,0);
    cout<<"amsgeom::richgeom02 -- MC misalignment set to "<<-test_point[0]<<" "<<-test_point[1]<<" "<<-test_point[2]<<endl;
  }
  
  rich=dynamic_cast<AMSgvolume*>(mother.add(new AMSgvolume(_VACUUM_,
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

  par[0]=RICHDB::rich_height/2.;
  par[3]=RICHDB::top_radius;
  par[4]=RICHDB::top_radius+RICmithk+RICepsln;
  par[1]=RICHDB::bottom_radius;
  par[2]=RICHDB::bottom_radius+RICmithk+RICepsln;

  coo[0]=coo[1]=0;
  coo[2]=RICHDB::total_height()/2.-RICHDB::mirror_pos();

  AMSgvolume *mirror=(AMSgvolume *)rich->add(new AMSgvolume(_VACUUM_,  // Material
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
  
  par[3]=RICHDB::top_radius+RICmithk;
  par[4]=RICHDB::top_radius+RICmithk+RICepsln;
  par[1]=RICHDB::bottom_radius;
  par[2]=RICHDB::bottom_radius+RICmithk+RICepsln;

  coo[2]=0;

  mirror->add(new AMSgvolume("RICH CARBON",
                             0,          // No rotation
                             "OMI0",     // Name
                             "CONE",    // Shape
                             par,       // Geant parameters
                             5,         // # of parameters
                             coo,       // coordinates
                             nrm,       // Matrix of normals
                             "ONLY",
                             0,
                             1,
                             rel));

  
  par[3]=RICHDB::top_radius;
  par[4]=RICHDB::top_radius+RICmithk;
  par[1]=RICHDB::bottom_radius;
  par[2]=RICHDB::bottom_radius+RICmithk;

  /*    
  mirror->add(new AMSgvolume("RICH MIRROR",
                             0,          // No rotation
                             "OMI1",     // Name
                             "CONE",    // Shape
                             par,       // Geant parameters
                             5,         // # of parameters
                             coo,       // coordinates
                             nrm,       // Matrix of normals  
                             "ONLY",
                             0,
                             1,
                             rel));
  */

 //SECTOR 1 less Refl
 par[5]=RICHDB::RICmirrors2_s1;
 par[6]=RICHDB::RICmirrors1_s3;
 mirror->add(new AMSgvolume("RICH MIRROR S1",
                             0,          // No rotation
                             "OMI1",     // Name
                             "CONS",    // Shape
                             par,       // Geant parameters
                             7,         // # of parameters
                             coo,       // coordinates
                             nrm,       // Matrix of normals  
                             "ONLY",
                              0,
                             1,
                             rel));


 //SECTOR 2
 par[5]=RICHDB::RICmirrors3_s2;
 par[6]=RICHDB::RICmirrors2_s1;
 mirror->add(new AMSgvolume("RICH MIRROR S2",
                             0,          // No rotation
                             "OMI2",     // Name
                             "CONS",    // Shape
                             par,       // Geant parameters
                             7,         // # of parameters
                             coo,       // coordinates
                             nrm,       // Matrix of normals  
                             "ONLY",
                              0,
                             1,
                             rel));




 //SECTOR 3   
 par[5]=RICHDB::RICmirrors1_s3;
 par[6]=RICHDB::RICmirrors3_s2+360;
 mirror->add(new AMSgvolume("RICH MIRROR S3",
                             0,          // No rotation
                             "OMI3",     // Name
                             "CONS",    // Shape
                             par,       // Geant parameters
                             7,         // # of parameters
                             coo,       // coordinates
                             nrm,       // Matrix of normals  
                             "ONLY",
                              0,
                             1,
                             rel));




  integer copia1=1,copia2=1;
  AMSNode *rad1,*rad2;
  rad1=0;
  rad2=0;

  // Definition of the radiator box
  par[0]=180.0/RICradiator_box_sides;
  par[1]=360;
  par[2]=RICradiator_box_sides;
  par[3]=2;
  par[4]=-RICHDB::rad_height/2-RICHDB::foil_height-RICepsln;  // Radiator and foil are inside this box
  par[5]=0;
  par[6]=RICradiator_box_radius;
  par[7]=RICHDB::rad_height/2;
  par[8]=0;
  par[9]=RICradiator_box_radius;


  coo[0]=0;
  coo[1]=0;
  coo[2]=RICHDB::total_height()/2-RICHDB::rad_pos();

  AMSgvolume *radiator_box=(AMSgvolume*)rich->add(new AMSgvolume(_VACUUM_,
								 0,
								 "RBOX",
								 "PGON",
								 par,
								 10,
								 coo,
								 nrm,
								 "ONLY",
								 0,
								 1,
								 rel));
  par[5]=par[8]=RICradiator_box_radius;
  par[6]=par[9]=RICradiator_box_radius+RICradiator_box_thickness;  

  //  structure around the radiator
  AMSgvolume *radiator_box_border=(AMSgvolume*)rich->add(new AMSgvolume("CARBON",
									0,
									"RBOB",
									"PGON",
									par,
									10,
									coo,
									nrm,
									"ONLY",
									0,
									1,
									rel));


  // FOIL
  par[0]=180.0/RICradiator_box_sides;
  par[1]=360;
  par[2]=RICradiator_box_sides;
  par[3]=2;
  par[4]=-RICHDB::foil_height/2;
  par[5]=0;
  par[6]=RICradiator_box_radius;
  par[7]=RICHDB::foil_height/2;
  par[8]=0;
  par[9]=RICradiator_box_radius;

  coo[0]=0;coo[1]=0;
  coo[2]=-RICHDB::rad_height/2-RICepsln-RICHDB::foil_height/2;

  radiator_box->add(new AMSgvolume("HESAGLASS",
				       0,
				       "FOIL",
				       "PGON",
				       par,
				       10,
				       coo,
				       nrm,
				       "ONLY",
				       0,
				       1,
				       rel));
  



  // This puts the AGL radiator
  for(int n=0;n<RichRadiatorTileManager::get_number_of_tiles();n++){
    if(RichRadiatorTileManager::get_tile_kind(n)==agl_kind){
      // Here we put a tile with aerogel
      par[0]=RichRadiatorTileManager::get_tile_boundingbox(n,0)+RICaethk/2;
      par[1]=RichRadiatorTileManager::get_tile_boundingbox(n,1)+RICaethk/2;
      par[2]=RICHDB::rad_height/2;

      coo[0]=RichRadiatorTileManager::get_tile_x(n);
      coo[1]=RichRadiatorTileManager::get_tile_y(n);
      coo[2]=0;
 
      rad1=radiator_box->add(new AMSgvolume("RICH PORON",
					    0,
					    "RADB",
					    "BOX",
					    par,
					    3,
					    coo,
					    nrm,
					    "ONLY",
					    0,
					    copia1++,
					    rel));

#ifdef __G4AMS__
      if(MISCFFKEY.G4On)
	Put_rad((AMSgvolume *)rad1,copia1-1,n);
#endif
    }  
  }

#ifdef __G4AMS__
  if(MISCFFKEY.G3On){
#endif
    // In geant 3 we only need to fill one of the copies to get all the other filled
    if(rad1)
      for(int n=0;n<RichRadiatorTileManager::get_number_of_tiles();n++)
	if(RichRadiatorTileManager::get_tile_kind(n)==agl_kind){Put_rad((AMSgvolume *)rad1,1,n);break;}

#ifdef __G4AMS__
  }
#endif
  
  // NaF radiator:
  // Since its structure is complex, we make it from a single poron box
  const double full_naf_block_length=4*(RICHDB::naf_length+RICaethk);

  par[0]=full_naf_block_length/2;
  par[1]=par[0];
  par[2]=RICHDB::rad_height/2;
  
  coo[0]=0;
  coo[1]=0;
  coo[2]=0;
  
  
  rad2=radiator_box->add(new AMSgvolume("RICH PORON",
					0,
					"NAFB",
					"BOX",
					par,
					3,
					coo,
					nrm,
					"ONLY",
					0,
					1,
					rel));
  
  // We insert the main naf block 

  par[0]=45;
  par[1]=360;
  par[2]=4;
  par[3]=3;
  par[4]=-RICHDB::naf_height/2;
  par[5]=0;
  par[6]=full_naf_block_length/2;
  par[7]=par[4]+0.1;
  par[8]=0;
  par[9]=full_naf_block_length/2;
  par[10]=RICHDB::naf_height/2;
  par[11]=0;
  par[12]=full_naf_block_length/2-(RICHDB::naf_height-0.1)/sqrt(2.0);
  coo[0]=0;
  coo[1]=0;
  coo[2]=RICHDB::naf_height/2-RICHDB::rad_height/2.;
  
  AMSgvolume *naf_block=(AMSgvolume*)rad2->add(new AMSgvolume("RICH NAF",
							      0,
							      "NAF ",
							      "PGON",
							      par,
							      13,
							      coo,
							      nrm,
							      "ONLY",
							      0,
							      1,
							      rel));
  
  // Add the holes and tile separations
  par[0]=1.6/2.0;
  par[1]=par[0];
  par[2]=RICHDB::naf_height/2;

  coo[0]=(full_naf_block_length/2-par[0]);
  coo[1]=(full_naf_block_length/2-par[0]);
  coo[2]=0;
  dummy=naf_block->add(new AMSgvolume("RICH PORON",
				      0,
				      "NHOL",
				      "BOX",
				      par,
				      3,
				      coo,
				      nrm,
				      "ONLY",
				      0,
				      1,
				      rel));


  coo[0]=-(full_naf_block_length/2-par[0]);
  coo[1]=(full_naf_block_length/2-par[0]);
  coo[2]=0;
  dummy=naf_block->add(new AMSgvolume("RICH PORON",
				      0,
				      "NHOL",
				      "BOX",
				      par,
				      3,
				      coo,
				      nrm,
				      "ONLY",
				      0,
				      2,
				      rel));

  coo[0]=(full_naf_block_length/2-par[0]);
  coo[1]=-(full_naf_block_length/2-par[0]);
  coo[2]=0;
  dummy=naf_block->add(new AMSgvolume("RICH PORON",
				      0,
				      "NHOL",
				      "BOX",
				      par,
				      3,
				      coo,
				      nrm,
				      "ONLY",
				      0,
				      3,
				      rel));

  coo[0]=-(full_naf_block_length/2-par[0]);
  coo[1]=-(full_naf_block_length/2-par[0]);
  coo[2]=0;
  dummy=naf_block->add(new AMSgvolume("RICH PORON",
				      0,
				      "NHOL",
				      "BOX",
				      par,
				      3,
				      coo,
				      nrm,
				      "ONLY",
				      0,
				      4,
				      rel));
  

  // THE MANYS BELOW ARE REQUIRED TO OBTAIN A "OR" BOOLEAN OPERATION
  // Naf tiles separators
  par[0]=full_naf_block_length/2;
  par[1]=RICaethk/2;
  par[2]=RICHDB::naf_height/2;

  coo[0]=0;
  coo[1]=0;
  coo[2]=0;
  naf_block->add(new AMSgvolume("RICH PORON",
				0,
				"NSEX",
				"BOX",
				par,
				3,
				coo,
				nrm,
				_MANY_,
				0,
				1,
				rel));

  coo[0]=0;
  coo[1]=RICHDB::naf_length+RICaethk;
  coo[2]=0;
  naf_block->add(new AMSgvolume("RICH PORON",
				0,
				"NSEX",
				"BOX",
				par,
				3,
				coo,
				nrm,
				_MANY_,
				0,
				2,
				rel));

  coo[0]=0;
  coo[1]=-(RICHDB::naf_length+RICaethk);
  coo[2]=0;
  naf_block->add(new AMSgvolume("RICH PORON",
				0,
				"NSEX",
				"BOX",
				par,
				3,
				coo,
				nrm,
				_MANY_,
				0,
				3,
				rel));



  par[0]=RICaethk/2;
  par[1]=full_naf_block_length/2;
  par[2]=RICHDB::naf_height/2;

  coo[0]=0;
  coo[1]=0;
  coo[2]=0;
  naf_block->add(new AMSgvolume("RICH PORON",
				0,
				"NSEY",
				"BOX",
				par,
				3,
				coo,
				nrm,
				_MANY_,
				0,
				1,
				rel));

  coo[0]=RICHDB::naf_length+RICaethk;
  coo[1]=0;
  coo[2]=0;
  naf_block->add(new AMSgvolume("RICH PORON",
				0,
				"NSEY",
				"BOX",
				par,
				3,
				coo,
				nrm,
				_MANY_,
				0,
				2,
				rel));

  coo[0]=-(RICHDB::naf_length+RICaethk);
  coo[1]=0;
  coo[2]=0;
  naf_block->add(new AMSgvolume("RICH PORON",
				0,
				"NSEY",
				"BOX",
				par,
				3,
				coo,
				nrm,
				_MANY_,
				0,
				3,
				rel));



  // Support structure: simple version
  coo[0]=0;
  coo[1]=RICHDB::hole_radius[1]-RICpmtsupport/2;
  coo[2]=RICHDB::total_height()/2-RICHDB::pmt_pos()+
    (RICHDB::pmtb_height()-RICpmtsupportheight)/2;
  
  par[0]=RICHDB::hole_radius[1];
  par[1]=RICpmtsupport/2;
  par[2]=RICpmtsupportheight/2;

  
  dummy=rich->add(new AMSgvolume("RICH CARBON",
                                 0,            
                                 "PMS0",       
                                 "BOX ",       
                                 par,          
                                 3,            
                                 coo,          
                                 nrm,          
                                 "ONLY",       
                                 0,            
                                 1,            
                                 rel));


  coo[1]*=-1;

  dummy=rich->add(new AMSgvolume("RICH CARBON",
                                 0,
                                 "PMS0",
                                 "BOX ",
                                 par,
                                 3,
                                 coo,
                                 nrm,
                                 "ONLY",
                                 0,
                                 2,
                                 rel));


  coo[0]=RICHDB::hole_radius[0]-RICpmtsupport/2;
  coo[1]=0;
  coo[2]=RICHDB::total_height()/2-RICHDB::pmt_pos()+
    (RICHDB::pmtb_height()-RICpmtsupportheight)/2;

  par[0]=RICpmtsupport/2;
  par[1]=RICHDB::hole_radius[1]-RICpmtsupport;
  par[2]=RICpmtsupportheight/2;

  
  dummy=rich->add(new AMSgvolume("RICH CARBON",
                                 0,
                                 "PMS1",
                                 "BOX ",
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
                                 "PMS1",
                                 "BOX ",
                                 par,
                                 3,
                                 coo,
                                 nrm,
                                 "ONLY",
                                 0,
                                 2,
                                 rel));

  ///// Positioning PMTs
  par[0]=RICHDB::lg_length>PMT_electronics+RICpmtshield?
                           RICHDB::lg_length/2:PMT_electronics/2+RICpmtshield/2;
  par[1]=par[0];
  par[2]=RICHDB::pmtb_height()/2;  

  for(int copia=0;copia<RICmaxpmts;copia++){
    coo[0]=RichPMTsManager::GetRichPMTPos(copia,0);
    coo[1]=RichPMTsManager::GetRichPMTPos(copia,1);
    coo[2]=RichPMTsManager::GetRichPMTPos(copia,2);  // Correct for local position


    lig=rich->add(new AMSgvolume("RICH VACUUM",
				 0,
				 "PMTB",
				 "BOX",
				 par,
				 3,
				 coo,
				 nrm,
				 "ONLY",
				 0,
				 copia+1,
				 rel));

#ifdef __G4AMS__
      if(MISCFFKEY.G4On)
	Put_pmt((AMSgvolume *)lig,copia+1);
#endif

  }


#ifdef __G4AMS__
  if(MISCFFKEY.G3On)
#endif
    Put_pmt((AMSgvolume *)lig,1);

}









