#ifndef AMSR_NtupleCommons_H
#define AMSR_NtupleCommons_H

#ifdef __ALPHA__
#define DECFortran
#else
#define mipsFortran
#endif

#include "cfortran.h"
#include "hbook.h"

const int NWPAW=1000000;
struct PAWC_DEF{float hmem[NWPAW];};
#define PAWC COMMON_BLOCK(PAWC,pawc)
COMMON_BLOCK_DEF(PAWC_DEF,PAWC);

struct EVENTH_DEF {
   int eventno,run,runtype,time[2],Rawwords;
   float Grmedphi,Rads;
   float Thetas,Phis,Yaws,Pitchs,Rolls,Velocitys;
   int Particles,Tracks,Betas;
   int Charges,Trrechits,Trclusters,Trrawclusters,Trmcclusters;
   int Tofclusters,Tofmcclusters,Ctcclusters,Ctcmcclusters;
   int Antimcclusters,Anticlusters;
};
#define blkEventh COMMON_BLOCK(EVENTH,eventh)
COMMON_BLOCK_DEF(EVENTH_DEF,blkEventh);

struct BETA_DEF {
   int nbeta,betastatus[100],betapattern[100];
   float beta[100];
   float betaerror[100],betachi2[100],betachi2s[100];
   int betantof[100],betaptof[100][4],betaptr[100];
};
#define blkBeta COMMON_BLOCK(BETA,beta)
COMMON_BLOCK_DEF(BETA_DEF,blkBeta);

struct CHARGE_DEF {
   int ncharge,chargestatus[10],chargebetap[10];
   int chargetof[10],chargetracker[10];
   float probtof[10][7],probtracker[10][7];
};
#define blkCharge COMMON_BLOCK(CHARGE,charge)
COMMON_BLOCK_DEF(CHARGE_DEF,blkCharge);

struct PARTICLE_DEF {
   int npart,pctcp[10][2],pbetap[10],pchargep[10];
   int ptrackp[10],pid[10];
   float pmass[10],perrmass[10],pmom[10],perrmom[10];
   float pcharge[10],ptheta[10],pphi[10],pcoo[10][3],signalctc[10][2];
   float betactc[10][2],errorbetactc[10][2],cooctc[10][2][3],cootof[10][4][3];
   float cooanti[10][2][3],cootr[10][6][3];
};
#define blkParticle COMMON_BLOCK(PARTICLE,particle)
COMMON_BLOCK_DEF(PARTICLE_DEF,blkParticle);

struct TOFCLUST_DEF {
   int ntof,Tofstatus[20],plane[20],bar[20];
   float Tofedep[20];
   float Toftime[20],Tofetime[20],Tofcoo[20][3],Tofercoo[20][3];
};
#define blkTofclust COMMON_BLOCK(TOFCLUST,tofclust)
COMMON_BLOCK_DEF(TOFCLUST_DEF,blkTofclust);

struct TOFMCCLU_DEF {
   int ntofmc,Tofmcidsoft[200];
   float Tofmcxcoo[200][3];
   float Tofmctof[200],Tofmcedep[200];
};
#define blkTofmcclu COMMON_BLOCK(TOFMCCLU,tofmcclu)
COMMON_BLOCK_DEF(TOFMCCLU_DEF,blkTofmcclu);

struct TRCLUSTE_DEF {
   int Ntrcl,Idsoft[200],Statust[200],Neleml[200];
   int Nelemr[200];
   float Sumt[200],Sigmat[200],Meant[200],Rmst[200];
   float Errormeant[200],Amplitude[200][5];
};
#define blkTrcluste COMMON_BLOCK(TRCLUSTE,trcluste)
COMMON_BLOCK_DEF(TRCLUSTE_DEF,blkTrcluste);

struct TRMCCLUS_DEF {
   int ntrclmc,Idsoftmc[200],Itra[200],Left[200][2];
   int Center[200][2],Right[200][2];
   float ss[200][2][5],xca[200][3],xcb[200][3];
   float xgl[200][3],summc[200];
};
#define blkTrmcclus COMMON_BLOCK(TRMCCLUS,trmcclus)
COMMON_BLOCK_DEF(TRMCCLUS_DEF,blkTrmcclus);

struct TRRECHIT_DEF {
   int ntrrh,px[500],py[500],statusr[500],Layer[500];
   float hitr[500][3],ehitr[500][3],sumr[500],difosum[500];
};
#define blkTrrechit COMMON_BLOCK(TRRECHIT,trrechit)
COMMON_BLOCK_DEF(TRRECHIT_DEF,blkTrrechit);

struct TRTRACK_DEF {
   int ntrtr,trstatus[20],pattern[20],nhits[20];
   int phits[20][6],Fastfitdone[20],Geanefitdone[20],Advancedfitdone[20];
   float Chi2strline[20],Chi2circle[20],Circleridgidity[20];
   float Chi2fastfit[20],Ridgidity[20],Errridgidity[20],Theta[20],phi[20];
   float p0[20][3],gchi2[20],gridgidity[20],gerrridgidity[20],gtheta[20];
   float gphi[20],gp0[20][3],hchi2[20][2],Hridgidity[20][2];
   float Herrridgidity[20][2],htheta[20][2],hphi[20][2],hp0[20][2][3];
   float fchi2ms[20],gchi2ms[20],ridgidityms[20],gridgidityms[20];
};
#define blkTrtrack COMMON_BLOCK(TRTRACK,trtrack)
COMMON_BLOCK_DEF(TRTRACK_DEF,blkTrtrack);

struct MCEVENTG_DEF {
   int nmcg,nskip[20],Particle[20];
   float coo[20][3],dir[20][3];
   float momentum[20],mass[20],charge[20];
};
#define blkMceventg COMMON_BLOCK(MCEVENTG,mceventg)
COMMON_BLOCK_DEF(MCEVENTG_DEF,blkMceventg);

struct CTCCLUST_DEF {
   int nctccl,Ctcstatus[20],Ctclayer[20];
   float ctccoo[20][3];
   float ctcercoo[20][3],ctcrawsignal[20],ctcsignal[20],ctcesignal[20];
};
#define blkCtcclust COMMON_BLOCK(CTCCLUST,ctcclust)
COMMON_BLOCK_DEF(CTCCLUST_DEF,blkCtcclust);

struct CTCMCCLU_DEF {
   int nctcclmc,Ctcmcidsoft[200];
   float Ctcmcxcoo[200][3];
   float Ctcmcxdir[200][3],Ctcstep[200],ctccharge[200],ctcbeta[200];
   float ctcedep[200];
};
#define blkCtcmcclu COMMON_BLOCK(CTCMCCLU,ctcmcclu)
COMMON_BLOCK_DEF(CTCMCCLU_DEF,blkCtcmcclu);

struct ANTICLUS_DEF {
   int nanti,Antistatus[16],Antisector[16];
   float Antiedep[16];
   float Anticoo[16][3],Antiercoo[16][3];
};
#define blkAnticlus COMMON_BLOCK(ANTICLUS,anticlus)
COMMON_BLOCK_DEF(ANTICLUS_DEF,blkAnticlus);

struct ANTIMCCL_DEF {
   int nantimc,Antimcidsoft[200];
   float Antimcxcoo[200][3];
   float Antimctof[200],Antimcedep[200];
};
#define blkAntimccl COMMON_BLOCK(ANTIMCCL,antimccl)
COMMON_BLOCK_DEF(ANTIMCCL_DEF,blkAntimccl);

struct LVL3_DEF {
   int nlvl3,Lvl3toftr[2],Lvl3antitr[2],Lvl3trackertr[2];
   int Lvl3ntrhits[2],Lvl3npat[2],Lvl3pattern[2][2];
   float Lvl3residual[2][2];
   float Lvl3time[2],Lvl3eloss[2];
};
#define blkLvl3 COMMON_BLOCK(LVL3,lvl3)
COMMON_BLOCK_DEF(LVL3_DEF,blkLvl3);

struct LVL1_DEF {
   int nlvl1,Lvl1mode[2],Lvl1flag[2],Lvl1tofpatt[2][4];
   int Lvl1tofpatt1[2][4],Lvl1antipatt[2];
};
#define blkLvl1 COMMON_BLOCK(LVL1,lvl1)
COMMON_BLOCK_DEF(LVL1_DEF,blkLvl1);

struct CTCHIT_DEF {
   int nctcht,Ctchitstatus[50],Ctchitlayer[50];
   int ctchitcolumn[50],ctchitrow[50];
   float ctchitsignal[50];
};
#define blkCtchit COMMON_BLOCK(CTCHIT,ctchit)
COMMON_BLOCK_DEF(CTCHIT_DEF,blkCtchit);

struct TRRAWCL_DEF {
   int ntrraw,rawaddress[500],rawlength[500];
   float s2n[500];
};
#define blkTrrawcl COMMON_BLOCK(TRRAWCL,trrawcl)
COMMON_BLOCK_DEF(TRRAWCL_DEF,blkTrrawcl);

struct ANTIRAWC_DEF {
   int nantiraw,antirawstatus[32],antirawsector[32];
   int antirawupdown[32];
   float antirawsignal[32];
};
#define blkAntirawc COMMON_BLOCK(ANTIRAWC,antirawc)
COMMON_BLOCK_DEF(ANTIRAWC_DEF,blkAntirawc);

struct TOFRAWCL_DEF {
   int ntofraw,tofrstatus[20],tofrplane[20],tofrbar[20];
   float tofrtovta[20][2],tofrtovtd[20][2],tofrsdtm[20][2];
};
#define blkTofrawcl COMMON_BLOCK(TOFRAWCL,tofrawcl)
COMMON_BLOCK_DEF(TOFRAWCL_DEF,blkTofrawcl);

PROTOCCALLSFSUB3(OPENIT,openit,INT,STRING,STRING)
#define OPEN(lun,file,stat) \
        CCALLSFSUB3(OPENIT,openit,INT,STRING,STRING,lun,file,stat)

PROTOCCALLSFSUB1(CLOSEIT,closeit,INT)
#define CLOSE(lun) CCALLSFSUB1(CLOSEIT,closeit,INT,lun)

PROTOCCALLSFFUN1(INT,EXIST,exist,INT)
#define EXIST(id) CCALLSFFUN1(EXIST,exist,INT,id)

#endif
