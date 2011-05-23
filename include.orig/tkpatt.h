#ifndef tkpatt_h
#define tkpatt_h
#include <cstdlib>
#include <math.h>
#include <cstdio>
#include <vector>


class tkpatt {
  /// PatAllowOption bits
  enum { NO_EXTP = 1, NO_INTP = 2, ALL_5L = 4};
  /// Exception to allow pattern
  int PatAllowOption;
  //========================================================
  // HitPatterns
  //========================================================
protected:
  class HPat{
  public:
    /// Missing hit mask
    int mask;
    /// Hit pattern attribute ( =Nm*100000 +N1*10000 +N2*1000 +N3*100 +N4*10 +N5)
    int attrib;
    HPat():mask(0),attrib(12221){}
  };

  /// HitPatternAttrib bits
  enum { O_NMASK = 100000, 
         O_NP1 = 10000, O_NP2 = 1000, O_NP3 = 100, O_NP4 = 10, O_NP5 = 1 };
  //
  /// Number of layers for pattern scan (use this instead of trconst::maxlay)
  int SCANLAY;
  /// Number of hit patterns
  int NHitPatterns;
  /// Hit pattern masked hit bits
  std::vector<HPat> HitPattern;
  /// Convert HitPattern number from mask bit
  int *HitPatternIndex;
  // /// First HitPattern number with n masks
  //  int *HitPatternFirst;

  static int PatternID;
#pragma omp threadprivate(PatternID)
public:

  tkpatt(int nlay=7){ 
    SCANLAY=nlay;
    NHitPatterns=0;  
    HitPattern.clear(); 
    HitPatternIndex=0; 
    //    HitPatternFirst=0; 
    PatAllowOption=7;
 }

  ~tkpatt() {Clear();}

  void Clear(){
    SCANLAY=0;
    NHitPatterns=0;
    HitPattern.clear();
    if(HitPatternIndex) {delete[] HitPatternIndex;  HitPatternIndex=0; }
    //    if(HitPatternFirst) {delete[] HitPatternFirst;  HitPatternFirst=0; }
    return;
  }

  /// Initialize hit patterns, recursively called
  void BuildHitPatterns(int n = -1, int i = 0, int mask = 0);


  //========================================================
  // HitPattern utilities
  //========================================================
public:
  void Init(int nlay=7){Clear(); SCANLAY=nlay;BuildHitPatterns();}
  
   int GetSCANLAY(){return SCANLAY;}
  /// Get HitPatternMask
  int GetNHitPatterns(void) {
    return NHitPatterns;
  }
  /// Test HitPatternMask
  bool TestHitPatternMask(int i, int layer) {
    return ( 0 <= i && i < NHitPatterns) 
      ? (HitPattern[i].mask & (1 << (SCANLAY-layer))): 0;
  }
  /// Get HitPatternMask
  int GetHitPatternMask(int i) {
    return ( 0 <= i && i < NHitPatterns) ? HitPattern[i].mask : 0;
  }
  /// Get HitPatternAttrib
  int GetHitPatternAttrib(int i) {
    return ( 0 <= i && i < NHitPatterns) ? HitPattern[i].attrib : 0;
  }
  /// Get allow option of HitPatternAttrib
  int GetHitPatternAllow(int i);
  /// Get number of hits masked for the pattern i
  int GetHitPatternNmask(int i) {
    return ( 0 <= i && i < NHitPatterns) ? abs(HitPattern[i].attrib/O_NMASK) : 0;
  }
  /// Get HitPatternIndex
  int GetHitPatternIndex(int mask) {
    return (HitPatternIndex && 0 <= mask && mask < 256) ? HitPatternIndex[mask] : -1;
  }
//   /// Get HitPatternFirst
  int GetHitPatternFirst(int nm) {return NHitPatterns;}
//     return (HitPatternFirst && 0 <= nm && nm < 6) ? HitPatternFirst[nm] : NHitPatterns;
//   }
  /// Get HitPatternMask as a string
  const char *GetHitPatternStr(int i, char con = '1', char coff = '0');


};

extern tkpatt* patt; 
#ifdef __ROOTSHAREDLIBRARY__
#pragma omp threadprivate (patt)
#endif
#endif
