

#include "TrPatt.h"
#include "tkdcards.h"
// Hit pattern arrays
int  TrPatt::NHitPatterns     = 0;
int *TrPatt::HitPatternMask   = 0;
int *TrPatt::HitPatternAttrib = 0;
int *TrPatt::HitPatternIndex  = 0;
int *TrPatt::HitPatternFirst  = 0;
int  TrPatt::PatAllowOption = 0;


void TrPatt::BuildHitPatterns(int nmask, int ilyr, int mask)
{
  static int PatternID = 0;

//====================================
// General managing = starting point
//====================================
  if (nmask < 0) {
    NHitPatterns = 256 -1 -8 -8*7/2; // = 219 (Nhit >= 3)
    if (!HitPatternMask)   HitPatternMask   = new int[NHitPatterns];
    if (!HitPatternAttrib) HitPatternAttrib = new int[NHitPatterns];
    if (!HitPatternIndex)  HitPatternIndex  = new int[256];
    if (!HitPatternFirst)  HitPatternFirst  = new int[6];
    for (int j = 0; j < 256; j++) HitPatternIndex[j] = -1;
    for (int j = 0; j <   6; j++) HitPatternFirst[j] = NHitPatterns;

    // Start building HitPattern
    PatternID = 0;
    for (int j = 0; j < 6; j++) {
      HitPatternFirst[j] = PatternID;
      BuildHitPatterns(j);
    }
    return;
  }

//=================================================================
// Build pattern mask bits with n hits masked, recursively called
//=================================================================
  if (nmask > 0) {
    const int MASK_ORDER[8] = { 1, 3, 5, 2, 4, 6, 0, 7 };
    for (int j = ilyr; j < TkDBc::Head->nlays(); j++)
      BuildHitPatterns(nmask-1, j+1, mask | (1<<(TkDBc::Head->nlays()-MASK_ORDER[j]-1)));
    return;
  }

//=================================================================
// Check pattern attribute
// ( = Nmask*100000 +Np1*10000 +Np2*1000 +Np3*100 +Np4*10 +Np5)
//=================================================================
  int atrb = 12221, iatrb = O_NP1;
  for (int i = 0; i < TkDBc::Head->nlays(); i++) {
    if (i%2 == 1) iatrb /= 10;
    if (mask & (1<<(TkDBc::Head->nlays()-i-1))) atrb += -iatrb+O_NMASK;
  }
  HitPatternMask[PatternID] = mask; HitPatternAttrib[PatternID] = atrb;
  HitPatternIndex[mask] = PatternID;

  if (!TRFITFFKEY.patternp02[PatternID]) {
    // Not allowed (1): Both external layers are masked
    if ( (!(PatAllowOption & NO_EXTP) && 
       (atrb/O_NP1)%10 == 0 && (atrb/O_NP5)%10 == 0)
    // Not allowed (2): Both layers on any of internal planes are masked
      || (!(PatAllowOption & NO_INTP) && 
       ((atrb/O_NP2)%10 == 0 || (atrb/O_NP3)%10 == 0 || (atrb/O_NP4)%10 == 0))
    // Not allowed (3): Any external layers are masked and Nmask >= 3
      || (!(PatAllowOption & ALL_5L) && 
	  (atrb/O_NMASK >= 3 && (atrb/10000 == 0 || atrb%10 == 0))) )
      HitPatternAttrib[PatternID] = -atrb;
  }
  PatternID++;
}
