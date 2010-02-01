#ifndef _TRPATT_ 
#define _TRPATT_ 

class TrPatt{

  /// Initialize hit patterns, recursively called
  static void BuildHitPatterns(int n = -1, int i = 0, int mask = 0);
  TrPatt(){} // do no want public constructor
  /// PatAllowOption bits
  enum { NO_EXTP = 1, NO_INTP = 2, ALL_5L = 4};
  /// Exception to allow pattern
  static int PatAllowOption;

public:
  ///Initialize patterns
  static void Init(){ if(!HitPatternMask)BuildHitPatterns();}
 /// HitPatternAttrib bits
  enum { O_NMASK = 100000, 
	 O_NP1 = 10000, O_NP2 = 1000, O_NP3 = 100, O_NP4 = 10, O_NP5 = 1 };

  /// Number of hit patterns
  static int NHitPatterns;
  /// Hit pattern masked hit bits
  static int *HitPatternMask;
  /// Hit pattern attribute ( =Nm*100000 +N1*10000 +N2*1000 +N3*100 +N4*10 +N5)
  static int *HitPatternAttrib;
  /// Convert HitPattern number from mask bit
  static int *HitPatternIndex;
  /// First HitPattern number with n masks
  static int *HitPatternFirst;


  /// Get HitPatternMask
  static int GetNHitPatterns(void) {
    if(!HitPatternMask)BuildHitPatterns();
    return NHitPatterns;
  }

  /// Test HitPatternMask
  static bool TestHitPatternMask(int i, int layer) {
    if(!HitPatternMask)BuildHitPatterns();
  if (HitPatternMask && 0 <= i && i < NHitPatterns) 
      return  (HitPatternMask[i] & (1 << (trconst::nlays-layer)));
    else
      return 0;
  }

  /// Get HitPatternMask
  static int GetHitPatternMask(int i) {
    if(!HitPatternMask)BuildHitPatterns();
   if (HitPatternMask && 0 <= i && i < NHitPatterns) 
     return HitPatternMask[i]; 
   else 
     return 0;
  }
  /// Get HitPatternAttrib
  static int GetHitPatternAttrib(int i) {
    if(!HitPatternMask)BuildHitPatterns();
    if (HitPatternAttrib && 0 <= i && i < NHitPatterns) 
     return  HitPatternAttrib[i];
    else
      return  0;
  }

  /// Get number of hits masked for the pattern i
  static int GetHitPatternNmask(int i) {
    if(!HitPatternMask)BuildHitPatterns();
    if (HitPatternAttrib && 0 <= i && i < NHitPatterns)
      return abs(HitPatternAttrib[i]/O_NMASK);
    else 
      return  0;
  }

  /// Get HitPatternIndex
  static int GetHitPatternIndex(int mask) {
    if(!HitPatternMask)BuildHitPatterns();
    if (HitPatternIndex && 0 <= mask && mask < 256) return  HitPatternIndex[mask];
    else 
      return -1;
  }

  /// Get HitPatternFirst
  static int GetHitPatternFirst(int nm) {
    if(!HitPatternMask)BuildHitPatterns();
    if (HitPatternFirst && 0 <= nm && nm < 6)
      return HitPatternFirst[nm];
    else 
      return NHitPatterns;
  }


};


#endif
