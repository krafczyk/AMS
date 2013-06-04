#ifndef __RICHRADID__
#define __RICHRADID__
#include"richdbc.h"
#include "timeid.h"
#include "job.h"
#include "trrec.h"
#ifndef __ROOTSHAREDLIBRARY__
#include "richrec.h"
#include "richcal.h"
#endif

///////////////////////////////////////////
// This class contains all the information used in the reconstruction
// about the radiator
//////////////////////////////////////////////
class RichRadiatorTile{
 private:
  double position[2];         // Position wrt container center
  double bounding_box[3][2];  // Bounding box (3 coordinates, 2 boundary values)   
  int kind;                   // Flag with the kind of tile
  int id;
  
  double clarity;
  double effective_scattering_probability;
  double effective_scattering_angle;
  double index;
  geant *abs_length;
  geant index_table[RICmaxentries];
  
  // Fine mesh map
  int number_of_nodes;
  float *node_x;
  float *node_y;
  float *node_index;
  float index_bias;

  // These are computed a posteriori
  double mean_refractive_index;
  double mean_height;
  //  geant LocalIndex(geant dx,geant dy){return index;}
  geant LocalIndex(geant dx,geant dy);
  
  // Here we can add all the required histograms

 public:
  RichRadiatorTile():number_of_nodes(0),node_x(0),node_y(0),node_index(0),index_bias(0){};
  ~RichRadiatorTile(){if(node_x) delete node_x;if(node_y) delete node_y;if(node_index) delete node_index;}

  geant getheight(){return fabs(bounding_box[2][0]-bounding_box[2][1]);}

  // Here we can add geometric models and maps for everything 



  friend class RichRadiatorTileManager;  // Only the tile manager can modify those guys
};


class RichRadiatorTileManager{
 private:
  static bool _IgnoreDB;

  int _current_tile;
  AMSPoint _p_direct;
  AMSPoint _p_reflected;
  AMSDir _d_direct;
  AMSDir _d_reflected;
  AMSPoint _p_entrance;
  AMSDir   _d_entrance;
  double _local_index;    // Index for the current position
  double _distance2border;

  // All the necessary numbers
  static integer _number_of_rad_tiles;       // Number of radiator tiles
  static RichRadiatorTile **_tiles;          // The tiles themselves

  // Gemetry and alignment
  //  static geant   _alignment[9+3];                    // Alignment parameters:  position (3)+orientation(9) // UNUSED
  //  static integer _kind[RICmaxtiles];                 // Kind of material: UNUSED
  //  static geant   _position[RICmaxtiles*5];           // X and Y position + bounding box (XYZ Half length): UNUSED

  // Pointers to TDV for future use 
  static geant   _optical_parameters[RICmaxtiles*4]; // Mean index, clarity, scattering prob, scattering angle


  //
  // Time id of current used calibration
  //
  static time_t _parameters_begin,_parameters_insert,_parameters_end;  // Monitor changes in tables 


  static void _compute_mean_height(geant *index,
				    geant clarity,
				    geant *abs_len,
				    geant rheight,
				    geant &eff_index,
				    geant &height);

  static void _compute_tables();
  static void ReadFromFile(const char *filename);
  static void ReadFineMeshFromFile(const char *filename);
 public:
  static void Init();                    // Init geometry and kinds and so on
  //  static void Init_File(char *filename);
  static void Init_Default();            // Init geometry and kinds and so on
  static void Finish();
  //  static void Finish_File(char *filename);
  static void Finish_Default();


  static number getpos(int id,int pos);       // get pos (x,y) of a given tile in AMS coordinates 
  RichRadiatorTileManager(AMSTrTrack *track); // Constructor given a track 
  ~RichRadiatorTileManager(){};
  

  //////////////////////////////////////////////
  // Some code to access to some global values /
  //////////////////////////////////////////////
  static number get_number_of_tiles(){return _number_of_rad_tiles;};
  static number get_tile_x(integer tile){return _tiles[tile]->position[0];};
  static number get_tile_y(integer tile){return _tiles[tile]->position[1];};
  static integer get_tile_number(geant x,geant y);
  static integer get_tile_kind(integer tile){if(tile>=0 && tile<_number_of_rad_tiles) return _tiles[tile]->kind; else return empty_kind;}
  static number get_tile_boundingbox(int tile,int dim){return fabs(_tiles[tile]->bounding_box[dim][0]);}
  static geant get_refractive_index(geant x,geant y,geant wavelength);      // This is used for simulation purposes. It returns the local refractive index
  static void UpdateOpticalParametersTable(float x,float y);
  integer getcurrenttile(){if(_current_tile<0) return -1;return _current_tile;};


  ////////////////////////////////////////////////////////////////////////
  // Given a tile object constructed with a track return a lot od stuff //  
  ////////////////////////////////////////////////////////////////////////
  //  number getindex(){if(_current_tile<0) return 0;return _tiles[_current_tile]->mean_refractive_index;};
  number getindex(){if(_current_tile<0) return 0;return _local_index;};
  number getheight(){if(_current_tile<0) return 0;return fabs(_tiles[_current_tile]->bounding_box[2][0]-_tiles[_current_tile]->bounding_box[2][1]);};
  integer getkind(){if(_current_tile<0) return 0;return _tiles[_current_tile]->kind;};
  geant getclarity(){if(_current_tile<0) return 0;return _tiles[_current_tile]->clarity;}
  geant *getabstable(){if(_current_tile<0) return 0;return _tiles[_current_tile]->abs_length;}
  geant *getindextable(){if(_current_tile<0) return 0;return _tiles[_current_tile]->index_table;}

  AMSPoint getemissionpoint(int reflected=0){return !reflected?_p_direct:_p_reflected;};   // return the intersection point with a given track
  AMSDir getemissiondir(int reflected=0){return reflected?_d_direct:_d_reflected;};
  AMSPoint getentrancepoint(){return _p_entrance;};
  AMSDir getentrancedir(){return _d_entrance;};
  double getdistance(){return _distance2border;}

  static void recompute_tables(int current,double new_index=0.0);

  //
  // TDV values utilities 
  //
  static void DumpToTDV();
  static void GetFromTDV();
  static void UpdateDB(AMSID my_id);

  friend class AMSJob;
  friend class AMSRichCal;
};


#endif
