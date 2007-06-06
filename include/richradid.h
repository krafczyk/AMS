#ifndef __RICHRADID__
#define __RICHRADID__
#include"richdbc.h"

///////////////////////////////////////////
// This class contains all the information used in the reconstruction
// about the radiator
//////////////////////////////////////////////

class RichRadiatorTile{
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



#endif
