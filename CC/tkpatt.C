#include "tkpatt.h"

int tkpatt::PatternID=0;

tkpatt* patt; 

//***************************************************************************
void tkpatt::BuildHitPatterns(int nmask, int ilyr, int mask)
//***************************************************************************
{

  //====================================
  // General managing = starting point
  //====================================
  if (nmask < 0) {
    int ntot=pow(2,SCANLAY);
    NHitPatterns = ntot -1 -SCANLAY -SCANLAY*(SCANLAY-1)/2; // (Nhit >= 3)
    if (HitPattern.size()>0)  HitPattern.clear();
    //HitPattern.reserve(NHitPatterns);

    if (!HitPatternIndex)  delete[] HitPatternIndex;
    HitPatternIndex  = new int[ntot];
    
    //    if (!HitPatternFirst)  delete[]  HitPatternFirst;
    //    HitPatternFirst  = new int[SCANLAY-2];
    for (int j = 0; j <pow(2,SCANLAY) ; j++) HitPatternIndex[j] = -1;
    //   for (int j = 0; j <  SCANLAY-2 ; j++) HitPatternFirst[j] = NHitPatterns;

    // Start building HitPattern
    PatternID = 0;
    for (int j = 0; j < SCANLAY-2; j++) {
      //      HitPatternFirst[j] = PatternID;
      BuildHitPatterns(j);
    }
    if(SCANLAY==7){
      int max=NHitPatterns;
      int ii=0;
      while(ii<max){
	if(HitPattern[ii].mask & 0x40){
	  HPat aa=HitPattern[ii];
	  HitPattern.erase(HitPattern.begin()+ii);
	  HitPattern.push_back(aa);
	  max--;
	}else ii++;
	
      }
    }

    for (int ii=0;ii<NHitPatterns;ii++){
      int jj=HitPattern[ii].mask;
      HitPatternIndex[jj]=ii;
    }
					   
    return;
  }

  //=================================================================
  // Build pattern mask bits with n hits masked, recursively called
  //=================================================================
  if (nmask > 0) {
    const int MASK_ORDER[8] = { 1, 3, 5, 2, 4, 6, 0, 7 };
    for (int j = ilyr; j < SCANLAY; j++)
      BuildHitPatterns(nmask-1, j+1, mask | (1<<(SCANLAY-MASK_ORDER[j]-1)));
    return;
  }

  //=================================================================
  // Check pattern attribute
  // ( = Nmask*100000 +Np1*10000 +Np2*1000 +Np3*100 +Np4*10 +Np5)
  //=================================================================
  int atrb = 12221, iatrb = O_NP1;
  if(SCANLAY==7) {atrb=12220;}
  for (int i = 0; i < SCANLAY; i++) {
    if (i%2 == 1) iatrb /= 10;
    if (mask & (1<<(SCANLAY-i-1))) atrb += -iatrb+O_NMASK;
  }
  HPat aa;
  aa.mask   = mask;
  aa.attrib = atrb;
  HitPattern.push_back(aa);


  
  int palw = GetHitPatternAllow(PatternID);
  //   if ( (!(PatAllowOption & NO_EXTP) && (palw & NO_EXTP))
//        || (!(PatAllowOption & NO_INTP) && (palw & NO_INTP))
//        || (!(PatAllowOption &  ALL_5L) && (palw & ALL_5L)))

    
  if(
      (palw & NO_INTP)||
      (palw & ALL_5L))
    HitPattern[PatternID].attrib = -atrb;

  PatternID++;
}

//***************************************************************************
const char *tkpatt::GetHitPatternStr(int pat, char con, char coff)
  //***************************************************************************
{
  static char sbuf[9];
#pragma omp threadprivate(sbuf)
  
  for (int ly = 1; ly <= SCANLAY; ly++)
    sbuf[ly-1] = (TestHitPatternMask(pat, ly)) ? con : coff;
  sbuf[SCANLAY] = '\0';
  return sbuf;
}



//***************************************************************************
int tkpatt::GetHitPatternAllow(int i) 
//***************************************************************************
{
  int atrb = GetHitPatternAttrib(i);
  int pate = 0;
  if (atrb == 0) return 0;

  if(SCANLAY==8){  // AMS-02SC
    // Not allowed (1): Both external layers are masked
    if ((atrb/O_NP1)%10 == 0 && (atrb/O_NP5)%10 == 0) pate |= NO_EXTP;
    // Not allowed (2): Both layers on any of internal planes are masked
    if ((atrb/O_NP2)%10 == 0 || (atrb/O_NP3)%10 == 0 || 
	(atrb/O_NP4)%10 == 0) pate |= NO_INTP;
    // Not allowed (3): Any external layers are masked and Nmask >= 3
    if ((atrb/O_NMASK >= 3  && ((atrb/O_NP1)%10 == 0 || 
				(atrb/O_NP5)%10 == 0))) pate |= ALL_5L;
  }else if (SCANLAY==7){ // AMS-02PM
    // Not allowed (1): layer 0 is empty
    if ((atrb/O_NP1)%10 == 0 ) pate |= NO_EXTP;
    // Not allowed (2): Both layers on any of internal planes are masked
    if ((atrb/O_NP2)%10 == 0 || (atrb/O_NP3)%10 == 0 || 
	(atrb/O_NP4)%10 == 0) pate |= NO_INTP;

  }else{
    printf("GetHitPatternAllow-W-  SCANLAY is not 7 or 8 What are you doing????????\n");
  }
  return pate;
}
