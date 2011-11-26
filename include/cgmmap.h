// P.Zuccon 2011
#ifndef cgmmap_h
#define cgmmap_h

class CGMmap{
public:
  
  static  float _cgmap[121][360][2];
  static int _nglat; 
  static int _nglon;
  
  CGMmap(){}
  ~CGMmap(){}
  
  void Read(char *fnam="map.txt");
  /// Takes input geo (lat,long) and gives back CGM (clat,clon) all coo in deg
  static int GetCGM(double lat,double lon,double &clat,double &clon);
  static int GetCGMRad(double lat,double lon,double &clat,double &clon);
};


#endif

