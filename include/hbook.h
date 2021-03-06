//  $Id: hbook.h,v 1.4 2008/12/18 11:19:25 pzuccon Exp $


/*
 Corrections:
 15-Feb-94 GF HGIVE  correct output args
              HGIVEN idem
 18-Feb-94 GF comment headers for routines with more than 10 Arguments
              and break long lines by \ (for the VAX)
 -------------above carried over from previous file on 15-Mar-94
 11-Apr-94 GF remove ALL occurences of ZTRING (these are not used so far)
              correct HOPEN, HROUT calls
 21-Apr-94 GF restore PROTOCCALLSFFUNi lines for functions.
 18-May-94 GF correct passing of external functions, must use ROUTINE
 further comments kept in RCS/CVS
 12-Apr-95 GF delete all non documented routines; this should give a more 
              version for this file.
---------------------------------------
*/

#ifdef __ALPHA__
#define DECFortran
#else
#define mipsFortran
#endif
#include "cfortran.h"


PROTOCCALLSFSUB3(HARRAY,harray,INT,INT,PINT)
#define HARRAY(A1,A2,A3)  CCALLSFSUB3(HARRAY,harray,INT,INT,PINT,A1,A2,A3)
PROTOCCALLSFSUB4(HBANDX,hbandx,INT,FLOAT,FLOAT,FLOAT)
#define HBANDX(A1,A2,A3,A4)  CCALLSFSUB4(HBANDX,hbandx,INT,FLOAT,FLOAT,FLOAT,A1,A2,A3,A4)
PROTOCCALLSFSUB4(HBANDY,hbandy,INT,FLOAT,FLOAT,FLOAT)
#define HBANDY(A1,A2,A3,A4)  CCALLSFSUB4(HBANDY,hbandy,INT,FLOAT,FLOAT,FLOAT,A1,A2,A3,A4)
PROTOCCALLSFSUB1(HBARX,hbarx,INT)
#define HBARX(A1)  CCALLSFSUB1(HBARX,hbarx,INT,A1)
PROTOCCALLSFSUB1(HBARY,hbary,INT)
#define HBARY(A1)  CCALLSFSUB1(HBARY,hbary,INT,A1)
PROTOCCALLSFSUB6(HBFUN1,hbfun1,INT,STRING,INT,FLOAT,FLOAT,ROUTINE)
#define HBFUN1(A1,A2,A3,A4,A5,A6)  CCALLSFSUB6(HBFUN1,hbfun1,INT,STRING,INT,FLOAT,FLOAT,ROUTINE,A1,A2,A3,A4,A5,A6)
PROTOCCALLSFSUB9(HBFUN2,hbfun2,INT,STRING,INT,FLOAT,FLOAT,INT,FLOAT,FLOAT,ROUTINE)
#define HBFUN2(A1,A2,A3,A4,A5,A6,A7,A8,A9)  CCALLSFSUB9(HBFUN2,hbfun2,INT,STRING,INT,FLOAT,FLOAT,INT,FLOAT,FLOAT,ROUTINE,A1,A2,A3,A4,A5,A6,A7,A8,A9)
PROTOCCALLSFSUB2(HBIGBI,hbigbi,INT,INT)
#define HBIGBI(A1,A2)  CCALLSFSUB2(HBIGBI,hbigbi,INT,INT,A1,A2)
PROTOCCALLSFSUB1(HBINSZ,hbinsz,STRING)
#define HBINSZ(A1)  CCALLSFSUB1(HBINSZ,hbinsz,STRING,A1)
PROTOCCALLSFSUB4(HBNAMC,hbnamc,INT,STRING,STRINGV,STRING)
#define HBNAMC(A1,A2,A3,A4)  CCALLSFSUB4(HBNAMC,hbnamc,INT,STRING,STRINGV,STRING,A1,A2,A3,A4)
PROTOCCALLSFSUB4(HBNAME,hbname,INT,STRING,INTV,STRING)
#define HBNAME(A1,A2,A3,A4)  CCALLSFSUB4(HBNAME,hbname,INT,STRING,INTV,STRING,A1,A2,A3,A4)
PROTOCCALLSFSUB3(HBNT,hbnt,INT,STRING,STRING)
#define HBNT(A1,A2,A3)  CCALLSFSUB3(HBNT,hbnt,INT,STRING,STRING,A1,A2,A3)
PROTOCCALLSFSUB5(HBOOKB,hbookb,INT,STRING,INT,FLOATV,FLOAT)
#define HBOOKB(A1,A2,A3,A4,A5)  CCALLSFSUB5(HBOOKB,hbookb,INT,STRING,INT,FLOATV,FLOAT,A1,A2,A3,A4,A5)
PROTOCCALLSFSUB6(HBOOKN,hbookn,INT,STRING,INT,STRING,INT,STRINGV)
#define HBOOKN(A1,A2,A3,A4,A5,A6)  CCALLSFSUB6(HBOOKN,hbookn,INT,STRING,INT,STRING,INT,STRINGV,A1,A2,A3,A4,A5,A6)
PROTOCCALLSFSUB6(HBOOK1,hbook1,INT,STRING,INT,FLOAT,FLOAT,FLOAT)
#define HBOOK1(A1,A2,A3,A4,A5,A6)  CCALLSFSUB6(HBOOK1,hbook1,INT,STRING,INT,FLOAT,FLOAT,FLOAT,A1,A2,A3,A4,A5,A6)
PROTOCCALLSFSUB9(HBOOK2,hbook2,INT,STRING,INT,FLOAT,FLOAT,INT,FLOAT,FLOAT,FLOAT)
#define HBOOK2(A1,A2,A3,A4,A5,A6,A7,A8,A9)  CCALLSFSUB9(HBOOK2,hbook2,INT,STRING,INT,FLOAT,FLOAT,INT,FLOAT,FLOAT,FLOAT,A1,A2,A3,A4,A5,A6,A7,A8,A9)
PROTOCCALLSFSUB2(HBPRO,hbpro,INT,FLOAT)
#define HBPRO(A1,A2)  CCALLSFSUB2(HBPRO,hbpro,INT,FLOAT,A1,A2)
PROTOCCALLSFSUB8(HBPROF,hbprof,INT,STRING,INT,FLOAT,FLOAT,FLOAT,FLOAT,STRING)
#define HBPROF(A1,A2,A3,A4,A5,A6,A7,A8)  CCALLSFSUB8(HBPROF,hbprof,INT,STRING,INT,FLOAT,FLOAT,FLOAT,FLOAT,STRING,A1,A2,A3,A4,A5,A6,A7,A8)
PROTOCCALLSFSUB2(HBPROX,hbprox,INT,FLOAT)
#define HBPROX(A1,A2)  CCALLSFSUB2(HBPROX,hbprox,INT,FLOAT,A1,A2)
PROTOCCALLSFSUB2(HBPROY,hbproy,INT,FLOAT)
#define HBPROY(A1,A2)  CCALLSFSUB2(HBPROY,hbproy,INT,FLOAT,A1,A2)
PROTOCCALLSFSUB3(HBSET,hbset,STRING,INT,PINT)
#define HBSET(A1,A2,A3)  CCALLSFSUB3(HBSET,hbset,STRING,INT,PINT,A1,A2,A3)
PROTOCCALLSFSUB3(HBSLIX,hbslix,INT,INT,FLOAT)
#define HBSLIX(A1,A2,A3)  CCALLSFSUB3(HBSLIX,hbslix,INT,INT,FLOAT,A1,A2,A3)
PROTOCCALLSFSUB3(HBSLIY,hbsliy,INT,INT,FLOAT)
#define HBSLIY(A1,A2,A3)  CCALLSFSUB3(HBSLIY,hbsliy,INT,INT,FLOAT,A1,A2,A3)
PROTOCCALLSFSUB2(HCDIR,hcdir,PSTRING,STRING)
#define HCDIR(A1,A2)  CCALLSFSUB2(HCDIR,hcdir,PSTRING,STRING,A1,A2)
PROTOCCALLSFSUB2(HCOMPA,hcompa,INTV,INT)
#define HCOMPA(A1,A2)  CCALLSFSUB2(HCOMPA,hcompa,INTV,INT,A1,A2)
PROTOCCALLSFSUB3(HCOPY,hcopy,INT,INT,STRING)
#define HCOPY(A1,A2,A3)  CCALLSFSUB3(HCOPY,hcopy,INT,INT,STRING,A1,A2,A3)
PROTOCCALLSFSUB3(HCOPYM,hcopym,INT,INTV,INT)
#define HCOPYM(A1,A2,A3)  CCALLSFSUB3(HCOPYM,hcopym,INT,INTV,INT,A1,A2,A3)
PROTOCCALLSFSUB1(HDELET,hdelet,INT)
#define HDELET(A1)  CCALLSFSUB1(HDELET,hdelet,INT,A1)
PROTOCCALLSFSUB1(HDERIV,hderiv,FLOATV)
#define HDERIV(A1)  CCALLSFSUB1(HDERIV,hderiv,FLOATV,A1)
PROTOCCALLSFSUB4(HDIFF,hdiff,INT,INT,PFLOAT,STRING)
#define HDIFF(A1,A2,A3,A4)  CCALLSFSUB4(HDIFF,hdiff,INT,INT,PFLOAT,STRING,A1,A2,A3,A4)
PROTOCCALLSFSUB7(HDIFFB,hdiffb,INT,INT,FLOAT,INT,STRING,INT,FLOATV)
#define HDIFFB(A1,A2,A3,A4,A5,A6,A7)  CCALLSFSUB7(HDIFFB,hdiffb,INT,INT,FLOAT,INT,STRING,INT,FLOATV,A1,A2,A3,A4,A5,A6,A7)
PROTOCCALLSFSUB1(HDUMP,hdump,INT)
#define HDUMP(A1)  CCALLSFSUB1(HDUMP,hdump,INT,A1)
PROTOCCALLSFSUB1(HERMES,hermes,INT)
#define HERMES(A1)  CCALLSFSUB1(HERMES,hermes,INT,A1)
PROTOCCALLSFSUB7(HFC2,hfc2,INT,INT,STRING,INT,STRING,FLOAT,STRING)
#define HFC2(A1,A2,A3,A4,A5,A6,A7)  CCALLSFSUB7(HFC2,hfc2,INT,INT,STRING,INT,STRING,FLOAT,STRING,A1,A2,A3,A4,A5,A6,A7)
PROTOCCALLSFSUB4(HFF1,hff1,INT,PINT,FLOAT,FLOAT)
#define HFF1(A1,A2,A3,A4)  CCALLSFSUB4(HFF1,hff1,INT,PINT,FLOAT,FLOAT,A1,A2,A3,A4)
PROTOCCALLSFSUB5(HFF2,hff2,INT,PINT,FLOAT,FLOAT,FLOAT)
#define HFF2(A1,A2,A3,A4,A5)  CCALLSFSUB5(HFF2,hff2,INT,PINT,FLOAT,FLOAT,FLOAT,A1,A2,A3,A4,A5)
PROTOCCALLSFSUB4(HFILL,hfill,INT,FLOAT,FLOAT,FLOAT)
#define HFILL(A1,A2,A3,A4)  CCALLSFSUB4(HFILL,hfill,INT,FLOAT,FLOAT,FLOAT,A1,A2,A3,A4)
PROTOCCALLSFSUB3(HFINAM,hfinam,INT,STRINGV,INT)
#define HFINAM(A1,A2,A3)  CCALLSFSUB3(HFINAM,hfinam,INT,STRINGV,INT,A1,A2,A3)
PROTOCCALLSFSUB6(HFITEX,hfitex,INT,PFLOAT,PFLOAT,FLOAT,INT,FLOATV)
#define HFITEX(A1,A2,A3,A4,A5,A6)  CCALLSFSUB6(HFITEX,hfitex,INT,PFLOAT,PFLOAT,FLOAT,INT,FLOATV,A1,A2,A3,A4,A5,A6)
PROTOCCALLSFSUB7(HFITGA,hfitga,INT,PFLOAT,PFLOAT,PFLOAT,FLOAT,INT,FLOATV)
#define HFITGA(A1,A2,A3,A4,A5,A6,A7)  CCALLSFSUB7(HFITGA,hfitga,INT,PFLOAT,PFLOAT,PFLOAT,FLOAT,INT,FLOATV,A1,A2,A3,A4,A5,A6,A7)
PROTOCCALLSFSUB10(HFITH,hfith,INT,ROUTINE,STRING,INT,FLOATV,FLOATV,FLOATV,FLOATV,FLOATV,PFLOAT)
#define HFITH(A1,A2,A3,A4,A5,A6,A7,A8,A9,A10) CCALLSFSUB10(HFITH,hfith,INT,ROUTINE,STRING,INT,FLOATV,FLOATV,FLOATV,FLOATV,FLOATV,PFLOAT,A1,A2,A3,A4,A5,A6,A7,A8,A9,A10)
PROTOCCALLSFSUB10(HFITHN,hfithn,INT,STRING,STRING,INT,FLOATV,FLOATV,FLOATV,FLOATV,FLOATV,FLOAT)
#define HFITHN(A1,A2,A3,A4,A5,A6,A7,A8,A9,A10) CCALLSFSUB10(HFITHN,hfithn,INT,STRING,STRING,INT,FLOATV,FLOATV,FLOATV,FLOATV,FLOATV,PFLOAT,A1,A2,A3,A4,A5,A6,A7,A8,A9,A10)

/* cannot handle more than 10 parameters
PROTOCCALLSFSUB11(HFITL,hfitl,INT,ROUTINE,INT,DOUBLEV,PFLOAT,INT,PFLOAT,PFLOAT,FLOATV,FLOATV,FLOATV)
#define HFITL(A1,A2,A3,A4,A5,A6,A7,A8,A9,A10,A11)  CCALLSFSUB11(HFITL,hfitl,INT,ROUTINE,INT,DOUBLEV,PFLOAT,INT,PFLOAT,PFLOAT,FLOATV,FLOATV,FLOATV,A1,A2,A3,A4,A5,A6,A7,A8,A9,A10,A11)
PROTOCCALLSFSUB16(HFITN,hfitn,PFLOAT,FLOATV,FLOATV,INT,INT,INT,ROUTINE,INT,DOUBLEV,PFLOAT,INT,PFLOAT,PFLOAT,FLOATV,FLOATV,FLOATV)
#define HFITN(A1,A2,A3,A4,A5,A6,A7,A8,A9,A10,A11,A12,A13,A14,A15,A16)  CCALLSFSUB16(HFITN,hfitn,PFLOAT,FLOATV,FLOATV,INT,INT,INT,ROUTINE,INT,DOUBLEV,PFLOAT,INT,PFLOAT,PFLOAT,FLOATV,FLOATV,FLOATV,A1,A2,A3,A4,A5,A6,A7,A8,A9,A10,A11,A12,A13,A14,A15,A16)
*/

PROTOCCALLSFSUB6(HFITPO,hfitpo,INT,INT,PFLOAT,FLOAT,INT,FLOATV)
#define HFITPO(A1,A2,A3,A4,A5,A6)  CCALLSFSUB6(HFITPO,hfitpo,INT,INT,PFLOAT,FLOAT,INT,FLOATV,A1,A2,A3,A4,A5,A6)
PROTOCCALLSFSUB7(HFITS,hfits,INT,ROUTINE,INT,DOUBLEV,PFLOAT,INT,PFLOAT)
#define HFITS(A1,A2,A3,A4,A5,A6,A7)  CCALLSFSUB7(HFITS,hfits,INT,ROUTINE,INT,DOUBLEV,PFLOAT,INT,PFLOAT,A1,A2,A3,A4,A5,A6,A7)

/* cannot handle more than 10 parameters
PROTOCCALLSFSUB15(HFITV,hfitv,INT,INT,INT,PFLOAT,FLOATV,FLOATV,ROUTINE,STRING,INT,FLOATV,FLOATV,FLOATV,FLOATV,PFLOAT,PFLOAT)
#define HFITV(A1,A2,A3,A4,A5,A6,A7,A8,A9,A10,A11,A12,A13,A14,A15)  CCALLSFSUB15(HFITV,hfitv,INT,INT,INT,PFLOAT,FLOATV,FLOATV,ROUTINE,STRING,INT,FLOATV,FLOATV,FLOATV,FLOATV,PFLOAT,PFLOAT,A1,A2,A3,A4,A5,A6,A7,A8,A9,A10,A11,A12,A13,A14,A15)
*/

PROTOCCALLSFSUB10(HFIT1,hfit1,FLOATV,FLOATV,FLOATV,INT,ROUTINE,INT,DOUBLEV,PFLOAT,INT,PFLOAT)
#define HFIT1(A1,A2,A3,A4,A5,A6,A7,A8,A9,A10)  CCALLSFSUB10(HFIT1,hfit1,FLOATV,FLOATV,FLOATV,INT,ROUTINE,INT,DOUBLEV,PFLOAT,INT,PFLOAT,A1,A2,A3,A4,A5,A6,A7,A8,A9,A10)
PROTOCCALLSFSUB2(HFN,hfn,INT,FLOATV)
#define HFN(A1,A2)  CCALLSFSUB2(HFN,hfn,INT,FLOATV,A1,A2)
PROTOCCALLSFSUB1(HFNT,hfnt,INT)
#define HFNT(A1)  CCALLSFSUB1(HFNT,hfnt,INT,A1)
PROTOCCALLSFSUB2(HFNTB,hfntb,INT,STRING)
#define HFNTB(A1,A2)  CCALLSFSUB2(HFNTB,hfntb,INT,STRING,A1,A2)
PROTOCCALLSFSUB4(HFPAK1,hfpak1,INT,PINT,FLOATV,INT)
#define HFPAK1(A1,A2,A3,A4)  CCALLSFSUB4(HFPAK1,hfpak1,INT,PINT,FLOATV,INT,A1,A2,A3,A4)
PROTOCCALLSFSUB2(HFUNC,hfunc,INT,ROUTINE)
#define HFUNC(A1,A2)  CCALLSFSUB2(HFUNC,hfunc,INT,ROUTINE,A1,A2)
PROTOCCALLSFSUB3(HF1,hf1,INT,FLOAT,FLOAT)
#define HF1(A1,A2,A3)  CCALLSFSUB3(HF1,hf1,INT,FLOAT,FLOAT,A1,A2,A3)
PROTOCCALLSFSUB4(HF2,hf2,INT,FLOAT,FLOAT,FLOAT)
#define HF2(A1,A2,A3,A4)  CCALLSFSUB4(HF2,hf2,INT,FLOAT,FLOAT,FLOAT,A1,A2,A3,A4)
PROTOCCALLSFSUB7(HGFIT,hgfit,INT,PINT,PINT,PFLOAT,PFLOAT,PFLOAT,PSTRINGV)
#define HGFIT(A1,A2,A3,A4,A5,A6,A7)  CCALLSFSUB7(HGFIT,hgfit,INT,PINT,PINT,PFLOAT,PFLOAT,PFLOAT,PSTRINGV,A1,A2,A3,A4,A5,A6,A7)
PROTOCCALLSFSUB10(HGIVE,hgive,INT,STRING,PINT,PFLOAT,PFLOAT,PINT,PFLOAT,PFLOAT,PINT,PINT)
#define HGIVE(A1,A2,A3,A4,A5,A6,A7,A8,A9,A10)  CCALLSFSUB10(HGIVE,hgive,INT,STRING,PINT,PFLOAT,PFLOAT,PINT,PFLOAT,PFLOAT,PINT,PINT,A1,A2,A3,A4,A5,A6,A7,A8,A9,A10)
PROTOCCALLSFSUB6(HGIVEN,hgiven,INT,PSTRING,PINT,PSTRINGV,PFLOAT,PFLOAT)
#define HGIVEN(A1,A2,A3,A4,A5,A6)  CCALLSFSUB6(HGIVEN,hgiven,INT,PSTRING,PINT,PSTRINGV,PFLOAT,PFLOAT,A1,A2,A3,A4,A5,A6)
PROTOCCALLSFSUB5(HGN,hgn,INT,PINT,INT,PFLOAT,PINT)
#define HGN(A1,A2,A3,A4,A5)  CCALLSFSUB5(HGN,hgn,INT,PINT,INT,PFLOAT,PINT,A1,A2,A3,A4,A5)
PROTOCCALLSFSUB4(HGNF,hgnf,INT,INT,PFLOAT,PINT)
#define HGNF(A1,A2,A3,A4)  CCALLSFSUB4(HGNF,hgnf,INT,INT,PFLOAT,PINT,A1,A2,A3,A4)
PROTOCCALLSFSUB2(HGNPAR,hgnpar,INT,STRING)
#define HGNPAR(A1,A2)  CCALLSFSUB2(HGNPAR,hgnpar,INT,STRING,A1,A2)
PROTOCCALLSFSUB3(HGNT,hgnt,INT,INT,INT)
#define HGNT(A1,A2,A3)  CCALLSFSUB3(HGNT,hgnt,INT,INT,INT,A1,A2,A3)
PROTOCCALLSFSUB4(HGNTB,hgntb,INT,STRING,INT,INT)
#define HGNTB(A1,A2,A3,A4)  CCALLSFSUB4(HGNTB,hgntb,INT,STRING,INT,INT,A1,A2,A3,A4)
PROTOCCALLSFSUB3(HGNTF,hgntf,INT,INT,PINT)
#define HGNTF(A1,A2,A3)  CCALLSFSUB3(HGNTF,hgntf,INT,INT,PINT,A1,A2,A3)
PROTOCCALLSFSUB5(HGNTV,hgntv,INT,STRINGV,INT,INT,INT)
#define HGNTV(A1,A2,A3,A4,A5)  CCALLSFSUB5(HGNTV,hgntv,INT,STRINGV,INT,INT,INT,A1,A2,A3,A4,A5)
PROTOCCALLSFSUB2(HIDALL,hidall,INTV,PINT)
#define HIDALL(A1,A2)  CCALLSFSUB2(HIDALL,hidall,INTV,PINT,A1,A2)
PROTOCCALLSFSUB2(HIDOPT,hidopt,INT,STRING)
#define HIDOPT(A1,A2)  CCALLSFSUB2(HIDOPT,hidopt,INT,STRING,A1,A2)
PROTOCCALLSFSUB2(HID1,hid1,PINT,PINT)
#define HID1(A1,A2)  CCALLSFSUB2(HID1,hid1,PINT,PINT,A1,A2)
PROTOCCALLSFSUB2(HID2,hid2,PINT,PINT)
#define HID2(A1,A2)  CCALLSFSUB2(HID2,hid2,PINT,PINT,A1,A2)
PROTOCCALLSFSUB5(HIJXY,hijxy,INT,INT,INT,PFLOAT,PFLOAT)
#define HIJXY(A1,A2,A3,A4,A5)  CCALLSFSUB5(HIJXY,hijxy,INT,INT,INT,PFLOAT,PFLOAT,A1,A2,A3,A4,A5)
PROTOCCALLSFSUB0(HINDEX,hindex)
#define HINDEX() CCALLSFSUB0(HINDEX,hindex)
PROTOCCALLSFSUB4(HIPAK1,hipak1,INT,PINT,INTV,INT)
#define HIPAK1(A1,A2,A3,A4)  CCALLSFSUB4(HIPAK1,hipak1,INT,PINT,INTV,INT,A1,A2,A3,A4)
PROTOCCALLSFSUB0(HISTDO,histdo)
#define HISTDO() CCALLSFSUB0(HISTDO,histdo)
PROTOCCALLSFSUB3(HIX,hix,INT,INT,PFLOAT)
#define HIX(A1,A2,A3)  CCALLSFSUB3(HIX,hix,INT,INT,PFLOAT,A1,A2,A3)
PROTOCCALLSFSUB4(HLABEL,hlabel,INT,INT,STRINGV,STRING)
#define HLABEL(A1,A2,A3,A4)  CCALLSFSUB4(HLABEL,hlabel,INT,INT,STRINGV,STRING,A1,A2,A3,A4)
PROTOCCALLSFSUB2(HLDIR,hldir,STRING,STRING)
#define HLDIR(A1,A2)  CCALLSFSUB2(HLDIR,hldir,STRING,STRING,A1,A2)
PROTOCCALLSFSUB2(HLIMAP,hlimap,INT,STRING)
#define HLIMAP(A1,A2)  CCALLSFSUB2(HLIMAP,hlimap,INT,STRING,A1,A2)
PROTOCCALLSFSUB1(HLIMIT,hlimit,INT)
#define HLIMIT(A1)  CCALLSFSUB1(HLIMIT,hlimit,INT,A1)
PROTOCCALLSFSUB2(HLOCAT,hlocat,INT,PINT)
#define HLOCAT(A1,A2)  CCALLSFSUB2(HLOCAT,hlocat,INT,PINT,A1,A2)
PROTOCCALLSFSUB2(HMAXIM,hmaxim,INT,FLOAT)
#define HMAXIM(A1,A2)  CCALLSFSUB2(HMAXIM,hmaxim,INT,FLOAT,A1,A2)
PROTOCCALLSFSUB2(HMDIR,hmdir,STRING,STRING)
#define HMDIR(A1,A2)  CCALLSFSUB2(HMDIR,hmdir,STRING,STRING,A1,A2)
PROTOCCALLSFSUB2(HMINIM,hminim,INT,FLOAT)
#define HMINIM(A1,A2)  CCALLSFSUB2(HMINIM,hminim,INT,FLOAT,A1,A2)
PROTOCCALLSFSUB2(HNOENT,hnoent,INT,PINT)
#define HNOENT(A1,A2)  CCALLSFSUB2(HNOENT,hnoent,INT,PINT,A1,A2)
PROTOCCALLSFSUB2(HNORMA,hnorma,INT,FLOAT)
#define HNORMA(A1,A2)  CCALLSFSUB2(HNORMA,hnorma,INT,FLOAT,A1,A2)
PROTOCCALLSFSUB6(HOPERA,hopera,INT,STRING,INT,INT,FLOAT,FLOAT)
#define HOPERA(A1,A2,A3,A4,A5,A6)  CCALLSFSUB6(HOPERA,hopera,INT,STRING,INT,INT,FLOAT,FLOAT,A1,A2,A3,A4,A5,A6)
PROTOCCALLSFSUB1(HOUTPU,houtpu,INT)
#define HOUTPU(A1)  CCALLSFSUB1(HOUTPU,houtpu,INT,A1)
PROTOCCALLSFSUB1(HPAGSZ,hpagsz,INT)
#define HPAGSZ(A1)  CCALLSFSUB1(HPAGSZ,hpagsz,INT,A1)
PROTOCCALLSFSUB2(HPAK,hpak,INT,FLOATV)
#define HPAK(A1,A2)  CCALLSFSUB2(HPAK,hpak,INT,FLOATV,A1,A2)
PROTOCCALLSFSUB2(HPAKAD,hpakad,INT,FLOATV)
#define HPAKAD(A1,A2)  CCALLSFSUB2(HPAKAD,hpakad,INT,FLOATV,A1,A2)
PROTOCCALLSFSUB2(HPAKE,hpake,INT,FLOATV)
#define HPAKE(A1,A2)  CCALLSFSUB2(HPAKE,hpake,INT,FLOATV,A1,A2)
PROTOCCALLSFSUB7(HPARAM,hparam,INT,INT,FLOAT,INTV,PDOUBLE,PINT,PINT)
#define HPARAM(A1,A2,A3,A4,A5,A6,A7)  CCALLSFSUB7(HPARAM,hparam,INT,INT,FLOAT,INTV,PDOUBLE,PINT,PINT,A1,A2,A3,A4,A5,A6,A7)

/* cannot handle more than 10 parameters
PROTOCCALLSFSUB11(HPARMN,hparmn,PFLOAT,FLOATV,FLOATV,INT,INT,INT,FLOAT,INTV,PDOUBLE,PINT,PINT)
#define HPARMN(A1,A2,A3,A4,A5,A6,A7,A8,A9,A10,A11)  CCALLSFSUB11(HPARMN,hparmn,PFLOAT,FLOATV,FLOATV,INT,INT,INT,FLOAT,INTV,PDOUBLE,PINT,PINT,A1,A2,A3,A4,A5,A6,A7,A8,A9,A10,A11)
*/

PROTOCCALLSFSUB2(HPCHAR,hpchar,INTV,INTV)
#define HPCHAR(A1,A2)  CCALLSFSUB2(HPCHAR,hpchar,INTV,INTV,A1,A2)
PROTOCCALLSFSUB2(HPDIR,hpdir,STRING,STRING)
#define HPDIR(A1,A2)  CCALLSFSUB2(HPDIR,hpdir,STRING,STRING,A1,A2)
PROTOCCALLSFSUB3(HPHIST,hphist,INT,STRING,INT)
#define HPHIST(A1,A2,A3)  CCALLSFSUB3(HPHIST,hphist,INT,STRING,INT,A1,A2,A3)
PROTOCCALLSFSUB1(HPHS,hphs,INT)
#define HPHS(A1)  CCALLSFSUB1(HPHS,hphs,INT,A1)
PROTOCCALLSFSUB1(HPHST,hphst,INT)
#define HPHST(A1)  CCALLSFSUB1(HPHST,hphst,INT,A1)
PROTOCCALLSFSUB0(HPONCE,hponce)
#define HPONCE() CCALLSFSUB0(HPONCE,hponce)
PROTOCCALLSFSUB1(HPRINT,hprint,INT)
#define HPRINT(A1)  CCALLSFSUB1(HPRINT,hprint,INT,A1)
PROTOCCALLSFSUB1(HPRNT,hprnt,INT)
#define HPRNT(A1)  CCALLSFSUB1(HPRNT,hprnt,INT,A1)
PROTOCCALLSFSUB7(HPROJ1,hproj1,INT,INT,INT,ROUTINE,INT,INT,INT)
#define HPROJ1(A1,A2,A3,A4,A5,A6,A7)  CCALLSFSUB7(HPROJ1,hproj1,INT,INT,INT,ROUTINE,INT,INT,INT,A1,A2,A3,A4,A5,A6,A7)
PROTOCCALLSFSUB8(HPROJ2,hproj2,INT,INT,INT,ROUTINE,INT,INT,INT,INT)
#define HPROJ2(A1,A2,A3,A4,A5,A6,A7,A8)  CCALLSFSUB8(HPROJ2,hproj2,INT,INT,INT,ROUTINE,INT,INT,INT,INT,A1,A2,A3,A4,A5,A6,A7,A8)
PROTOCCALLSFSUB3(HPROT,hprot,INT,STRING,INT)
#define HPROT(A1,A2,A3)  CCALLSFSUB3(HPROT,hprot,INT,STRING,INT,A1,A2,A3)
PROTOCCALLSFSUB1(HPSCAT,hpscat,INT)
#define HPSCAT(A1)  CCALLSFSUB1(HPSCAT,hpscat,INT,A1)
PROTOCCALLSFSUB1(HPTAB,hptab,INT)
#define HPTAB(A1)  CCALLSFSUB1(HPTAB,hptab,INT,A1)

/* cannot handle more than 10 parameters
PROTOCCALLSFSUB11(HQUAD,hquad,INT,STRING,INT,FLOAT,FLOAT,PINT,PFLOAT,PINT,PFLOAT,PFLOAT,PINT)
#define HQUAD(A1,A2,A3,A4,A5,A6,A7,A8,A9,A10,A11)  CCALLSFSUB11(HQUAD,hquad,INT,STRING,INT,FLOAT,FLOAT,PINT,PFLOAT,PINT,PFLOAT,PFLOAT,PINT,A1,A2,A3,A4,A5,A6,A7,A8,A9,A10,A11)
*/

PROTOCCALLSFSUB3(HRDIR,hrdir,INT,PSTRINGV,PINT)
#define HRDIR(A1,A2,A3)  CCALLSFSUB3(HRDIR,hrdir,INT,PSTRINGV,PINT,A1,A2,A3)
PROTOCCALLSFSUB8(HREBIN,hrebin,INT,PFLOAT,PFLOAT,PFLOAT,PFLOAT,INT,INT,INT)
#define HREBIN(A1,A2,A3,A4,A5,A6,A7,A8)  CCALLSFSUB8(HREBIN,hrebin,INT,PFLOAT,PFLOAT,PFLOAT,PFLOAT,INT,INT,INT,A1,A2,A3,A4,A5,A6,A7,A8)
PROTOCCALLSFSUB2(HRECOV,hrecov,INT,STRING)
#define HRECOV(A1,A2)  CCALLSFSUB2(HRECOV,hrecov,INT,STRING,A1,A2)
PROTOCCALLSFSUB1(HREND,hrend,STRING)
#define HREND(A1)  CCALLSFSUB1(HREND,hrend,STRING,A1)
PROTOCCALLSFSUB2(HRESET,hreset,INT,STRING)
#define HRESET(A1,A2)  CCALLSFSUB2(HRESET,hreset,INT,STRING,A1,A2)
PROTOCCALLSFSUB3(HRFILE,hrfile,INT,STRING,STRING)
#define HRFILE(A1,A2,A3)  CCALLSFSUB3(HRFILE,hrfile,INT,STRING,STRING,A1,A2,A3)
PROTOCCALLSFSUB3(HRGET,hrget,INT,STRING,STRING)
#define HRGET(A1,A2,A3)  CCALLSFSUB3(HRGET,hrget,INT,STRING,STRING,A1,A2,A3)
PROTOCCALLSFSUB3(HRIN,hrin,INT,INT,INT)
#define HRIN(A1,A2,A3)  CCALLSFSUB3(HRIN,hrin,INT,INT,INT,A1,A2,A3)
PROTOCCALLSFSUB3(HRNDM2,hrndm2,INT,PFLOAT,PFLOAT)
#define HRNDM2(A1,A2,A3)  CCALLSFSUB3(HRNDM2,hrndm2,INT,PFLOAT,PFLOAT,A1,A2,A3)
PROTOCCALLSFSUB6(HROPEN,hropen,INT,STRING,STRING,STRING,PINT,PINT)
#define HROPEN(A1,A2,A3,A4,A5,A6)  CCALLSFSUB6(HROPEN,hropen,INT,STRING,STRING,STRING,PINT,PINT,A1,A2,A3,A4,A5,A6)
PROTOCCALLSFSUB3(HROUT,hrout,INT,PINT,STRING)
#define HROUT(A1,A2,A3)  CCALLSFSUB3(HROUT,hrout,INT,PINT,STRING,A1,A2,A3)
PROTOCCALLSFSUB3(HRPUT,hrput,INT,STRING,STRING)
#define HRPUT(A1,A2,A3)  CCALLSFSUB3(HRPUT,hrput,INT,STRING,STRING,A1,A2,A3)
PROTOCCALLSFSUB2(HSCALE,hscale,INT,FLOAT)
#define HSCALE(A1,A2)  CCALLSFSUB2(HSCALE,hscale,INT,FLOAT,A1,A2)
PROTOCCALLSFSUB3(HSCR,hscr,INT,INT,STRING)
#define HSCR(A1,A2,A3)  CCALLSFSUB3(HSCR,hscr,INT,INT,STRING,A1,A2,A3)
PROTOCCALLSFSUB2(HSETPR,hsetpr,STRING,FLOAT)
#define HSETPR(A1,A2)  CCALLSFSUB2(HSETPR,hsetpr,STRING,FLOAT,A1,A2)
PROTOCCALLSFSUB3(HSMOOF,hsmoof,INT,INT,FLOAT)
#define HSMOOF(A1,A2,A3)  CCALLSFSUB3(HSMOOF,hsmoof,INT,INT,FLOAT,A1,A2,A3)
PROTOCCALLSFSUB5(HSPLI1,hspli1,INT,INT,INT,INT,PFLOAT)
#define HSPLI1(A1,A2,A3,A4,A5)  CCALLSFSUB5(HSPLI1,hspli1,INT,INT,INT,INT,PFLOAT,A1,A2,A3,A4,A5)
PROTOCCALLSFSUB5(HSPLI2,hspli2,INT,INT,INT,INT,INT)
#define HSPLI2(A1,A2,A3,A4,A5)  CCALLSFSUB5(HSPLI2,hspli2,INT,INT,INT,INT,INT,A1,A2,A3,A4,A5)
PROTOCCALLSFSUB1(HSQUEZ,hsquez,STRING)
#define HSQUEZ(A1)  CCALLSFSUB1(HSQUEZ,hsquez,STRING,A1)
PROTOCCALLSFSUB1(HTITLE,htitle,STRING)
#define HTITLE(A1)  CCALLSFSUB1(HTITLE,htitle,STRING,A1)
PROTOCCALLSFSUB4(HUNPAK,hunpak,INT,FLOATV,STRING,INT)
#define HUNPAK(A1,A2,A3,A4)  CCALLSFSUB4(HUNPAK,hunpak,INT,FLOATV,STRING,INT,A1,A2,A3,A4)
PROTOCCALLSFSUB4(HUNPKE,hunpke,INT,PFLOAT,STRING,INT)
#define HUNPKE(A1,A2,A3,A4)  CCALLSFSUB4(HUNPKE,hunpke,INT,PFLOAT,STRING,INT,A1,A2,A3,A4)
PROTOCCALLSFSUB5(HUWFUN,huwfun,INT,INT,STRING,INT,STRING)
#define HUWFUN(A1,A2,A3,A4,A5)  CCALLSFSUB5(HUWFUN,huwfun,INT,INT,STRING,INT,STRING,A1,A2,A3,A4,A5)
PROTOCCALLSFSUB3(HXI,hxi,INT,FLOAT,PINT)
#define HXI(A1,A2,A3)  CCALLSFSUB3(HXI,hxi,INT,FLOAT,PINT,A1,A2,A3)
PROTOCCALLSFSUB5(HXYIJ,hxyij,INT,FLOAT,FLOAT,PINT,PINT)
#define HXYIJ(A1,A2,A3,A4,A5)  CCALLSFSUB5(HXYIJ,hxyij,INT,FLOAT,FLOAT,PINT,PINT,A1,A2,A3,A4,A5)
 PROTOCCALLSFFUN1(LOGICAL,HEXIST,hexist,INT)
#define HEXIST(A2)  CCALLSFFUN1(HEXIST,hexist,INT,A2)
 PROTOCCALLSFFUN2(FLOAT,HI,hi,INT,INT)
#define HI(A2,A3)  CCALLSFFUN2(HI,hi,INT,INT,A2,A3)
 PROTOCCALLSFFUN2(FLOAT,HIE,hie,INT,INT)
#define HIE(A2,A3)  CCALLSFFUN2(HIE,hie,INT,INT,A2,A3)
 PROTOCCALLSFFUN2(FLOAT,HIF,hif,INT,INT)
#define HIF(A2,A3)  CCALLSFFUN2(HIF,hif,INT,INT,A2,A3)
 PROTOCCALLSFFUN3(FLOAT,HIJ,hij,INT,INT,INT)
#define HIJ(A2,A3,A4)  CCALLSFFUN3(HIJ,hij,INT,INT,INT,A2,A3,A4)
 PROTOCCALLSFFUN1(FLOAT,HMAX,hmax,INT)
#define HMAX(A2)  CCALLSFFUN1(HMAX,hmax,INT,A2)
 PROTOCCALLSFFUN1(FLOAT,HMIN,hmin,INT)
#define HMIN(A2)  CCALLSFFUN1(HMIN,hmin,INT,A2)
 PROTOCCALLSFFUN1(FLOAT,HRNDM1,hrndm1,INT)
#define HRNDM1(A2)  CCALLSFFUN1(HRNDM1,hrndm1,INT,A2)
 PROTOCCALLSFFUN4(FLOAT,HSPFUN,hspfun,INT,FLOAT,INT,INT)
#define HSPFUN(A2,A3,A4,A5)  CCALLSFFUN4(HSPFUN,hspfun,INT,FLOAT,INT,INT,A2,A3,A4,A5)
 PROTOCCALLSFFUN4(FLOAT,HSTATI,hstati,INT,INT,STRING,INT)
#define HSTATI(A2,A3,A4,A5)  CCALLSFFUN4(HSTATI,hstati,INT,INT,STRING,INT,A2,A3,A4,A5)
 PROTOCCALLSFFUN1(FLOAT,HSUM,hsum,INT)
#define HSUM(A2)  CCALLSFFUN1(HSUM,hsum,INT,A2)
 PROTOCCALLSFFUN2(FLOAT,HX,hx,INT,FLOAT)
#define HX(A2,A3)  CCALLSFFUN2(HX,hx,INT,FLOAT,A2,A3)
 PROTOCCALLSFFUN2(FLOAT,HXE,hxe,INT,FLOAT)
#define HXE(A2,A3)  CCALLSFFUN2(HXE,hxe,INT,FLOAT,A2,A3)
 PROTOCCALLSFFUN3(FLOAT,HXY,hxy,INT,FLOAT,FLOAT)
#define HXY(A2,A3,A4)  CCALLSFFUN3(HXY,hxy,INT,FLOAT,FLOAT,A2,A3,A4)

#if defined(vms) || defined(__vms)

 PROTOCCALLSFFUN3(INT,HCREATEG,hcreateg,STRING,INT,INT)
#define HCREATEG(A2,A3,A4)  CCALLSFFUN3(HCREATEG,hcreateg,STRING,INT,INT,A2,A3,A4)
 PROTOCCALLSFFUN3(INT,HMAPG,hmapg,STRING,INT,PINT)
#define HMAPG(A2,A3,A4)  CCALLSFFUN3(HMAPG,hmapg,STRING,INT,PINT,A2,A3,A4)

#else

PROTOCCALLSFSUB2(HLIMAP,hlimap,INT,STRING)
#define HLIMAP(A1,A2)  CCALLSFSUB2(HLIMAP,hlimap,INT,STRING,A1,A2)

#endif
