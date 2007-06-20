#ifndef __RICHRADID__
#define __RICHRADID__
#include"richdbc.h"

///////////////////////////////////////////
// This class contains all the information used in the reconstruction
// about the radiator
//////////////////////////////////////////////
/*
class RichRadiatorTileManager{
 private:

  number _index;          // Refractive index of current tile
  number _height;         // Height of current tile
  integer _kind;
  AMSPoint _p_direct;
  AMSPoint _p_reflected;
  AMSDir _d_direct;
  AMSDir _d_reflected;
  AMSPoint _p_entrance;
  AMSDir   _d_entrance;


  // ALl the necessary numbers
  static integer _number_of_rad_tiles;       // Number of radiator tiles
  static integer *_kind_of_tile;         // Kind of tile 0-> no tile; 1 agl; 2 NaF
  static geant _eff_indexes[radiator_kinds];
  static geant _rad_heights[radiator_kinds];     
  static geant _mean_height[radiator_kinds][2]; // For reflected and direct cases
  static geant *_abs_length[radiator_kinds];
  static geant *_index_tables[radiator_kinds];
  static geant _clarities[radiator_kinds];


  static void _compute_mean_height(geant *index,
				    geant clarity,
				    geant *abs_len,
				    geant rheight,
				    geant &eff_index,
				    geant &height);

 public:
  static void Init();                  // Init geometry and kinds and so on
  static number getpos(int id,int pos);  // get pos (x,y) of a given tile
  //  RichRadiatorTile(geant x,geant y){};   //Constructor given the position at the bottom: use the static member instead 
  RichRadiatorTile(AMSTrTrack *track); // Constructor given a track 
  RichRadiatorTile(){};
  ~RichRadiatorTile(){};
  


  //////////////////////////////////////////////
  // Some code to access to some global values /
  //////////////////////////////////////////////
  static inline number get_number_of_tiles(){return _number_of_rad_tiles;};
  static inline number get_tile_x(integer tile){
    return int(tile/_number_of_rad_tiles)*RICHDB::rad_length-
      (_number_of_rad_tiles-1)*RICHDB::rad_length/2.;
  };
  static inline number get_tile_y(integer tile){
    return int(tile%_number_of_rad_tiles)*RICHDB::rad_length-
      (_number_of_rad_tiles-1)*RICHDB::rad_length/2.;
  };
  static inline integer get_tile_number(geant x,geant y){
    x/=RICHDB::rad_length;
    y/=RICHDB::rad_length;
    x+=_number_of_rad_tiles/2.;
    y+=_number_of_rad_tiles/2.;
    if(x<0 || y<0 || x>=_number_of_rad_tiles || y>=_number_of_rad_tiles) return -1;
    return int(x)*_number_of_rad_tiles+int(y);
  };
  static inline integer get_tile_kind(integer tile){
    if(tile<0 || tile>=_number_of_rad_tiles*_number_of_rad_tiles) return empty_kind;
    else return _kind_of_tile[tile];
  };




  ////////////////////////////////////////////////////////////////////////
  // Given a tile object constructed with a track return a lot od stuff //  
  ////////////////////////////////////////////////////////////////////////


  inline number getindex(){return _index;};
  inline number getheight(){return _height;};
  inline integer getkind(){return _kind;};
  inline AMSPoint getemissionpoint(int reflected=0){
    return !reflected?_p_direct:_p_reflected;};   // return the intersection point with a given track
  inline AMSDir getemissiondir(int reflected=0)
    {return reflected?_d_direct:_d_reflected;};
  inline AMSPoint getentrancepoint(){return _p_entrance;};
  inline AMSDir getentrancedir(){return _d_entrance;};


  inline geant getclarity(){
    return _kind!=empty_kind?_clarities[_kind-1]:0.;}
  
  inline geant *getabstable(){
    return _kind!=empty_kind?_abs_length[_kind-1]:0;}
  
  inline geant *getindextable(){
    return _kind!=empty_kind?_index_tables[_kind-1]:0;}



};
*/


class RichRadiatorTile{
 private:
  double position[2];         // Position wrt container center
  double bounding_box[3][2];  // Bounding box (3 coordinates, 2 boundary values)   
  int kind;                   // Flag with the kind of tile
  int id;
  
  double mean_refractive_index;
  double mean_height;
  double clarity;
  double effective_scattering_probability;
  double effective_scattering_angle;
  geant *abs_length;
  geant *index_table;
  
 public:
  RichRadiatorTile(){};
  ~RichRadiatorTile(){}

  geant getheight(){return fabs(bounding_box[2][0]-bounding_box[2][1]);}

  // Here we can add geometric models and maps for everything
  friend class RichRadiatorTileManager;  // Only the tile manager can modify those guys
};


class RichRadiatorTileManager{
 private:
  int _current_tile;
  AMSPoint _p_direct;
  AMSPoint _p_reflected;
  AMSDir _d_direct;
  AMSDir _d_reflected;
  AMSPoint _p_entrance;
  AMSDir   _d_entrance;


  // All the necessary numbers
  static integer _number_of_rad_tiles;       // Number of radiator tiles
  static RichRadiatorTile **_tiles;           // The tiles themselves


  static void _compute_mean_height(geant *index,
				    geant clarity,
				    geant *abs_len,
				    geant rheight,
				    geant &eff_index,
				    geant &height);

 public:
  static void Init();                  // Init geometry and kinds and so on
  static void Init_Default();                  // Init geometry and kinds and so on
  static number getpos(int id,int pos);  // get pos (x,y) of a given tile
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

  ////////////////////////////////////////////////////////////////////////
  // Given a tile object constructed with a track return a lot od stuff //  
  ////////////////////////////////////////////////////////////////////////
  number getindex(){if(_current_tile<0) return 0;return _tiles[_current_tile]->mean_refractive_index;};
  number getheight(){if(_current_tile<0) return 0;return fabs(_tiles[_current_tile]->bounding_box[2][0]-_tiles[_current_tile]->bounding_box[2][1]);};
  integer getkind(){if(_current_tile<0) return 0;return _tiles[_current_tile]->kind;};
  geant getclarity(){if(_current_tile<0) return 0;return _tiles[_current_tile]->clarity;}
  geant *getabstable(){if(_current_tile<0) return 0;return _tiles[_current_tile]->abs_length;}
  geant *getindextable(){if(_current_tile<0) return 0;return _tiles[_current_tile]->index_table;}

  AMSPoint getemissionpoint(int reflected=0){return !reflected?_p_direct:_p_reflected;};   // return the intersection point with a given track
  AMSDir getemissiondir(int reflected=0){return reflected?_d_direct:_d_reflected;};
  AMSPoint getentrancepoint(){return _p_entrance;};
  AMSDir getentrancedir(){return _d_entrance;};
};


#endif
