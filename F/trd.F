        SUBROUTINE SIMTRD(g3trd)
        implicit none
C
C...TR-simulation main program
C
#include    "geant321/gctrak.inc"
#include "geant321/gcbank.inc"
#include "geant321/gckine.inc"
#include "geant321/gcking.inc"
#include "geant321/gcflag.inc"
#include "geant321/gcvolu.inc"
#include "geant321/gccuts.inc"
#include "geant321/gconst.inc"
#include "geant321/gcsets.inc"
#include "geant321/gclist.inc"
#include "geant321/gctmed.inc"
#include "geant321/gcnum.inc"
#include "trdf.h"
        integer g3trd
      DIMENSION ETR(1000),STR(1000),VECTS(7)
      real etr,str,vects,gamma,eloss
      integer ntr,jtm,n
*        DATA COR/0.48/                      !... for Polypr Foam
*         DATA COR/0.68/                      !... for Polypr Fibre
C...



         integer nwbuf
C...
*         some how we have to get also ions 
        IF(ABS(CHARGE).EQ.0) RETURN

C...
        if(step.le.0)return
        GAMMA          = GETOT/AMASS
        IF(GAMMA.LE.100.) RETURN
        NTR            = 0

*
*  only trd like media will play
*
      IF (JTMED.LE.0)GO TO 99
      IF (NUMED.LE.0)GO TO 99
      IF (NUMED.GT.NTMED)GO TO 99
      JTM = LQ(JTMED- NUMED)
      IF (JTM.LE.0)GO TO 99
      NWBUF = IQ(JTM-1) - 14
      IF(NWBUF.LT.4) GO TO 99
      if(Q(JTM+14+1).ne.g3trd.or.
     +(Q(JTM+14+2).gt.2.or.Q(JTM+14+2).lt.0))goto 99
      if(Q(JTM+14+3).eq.3)then
       CALL GENTRDI(ABS(CHARGE),GAMMA,STEP,NTR,ETR,STR)
      else
       CALL GENTRD(ABS(CHARGE),GAMMA,Q(JTM+14+4),STEP,NTR,ETR,STR)
      endif


C...
C..    TR-Photon generated means NTR =|= 0
C.
        IF(NTR.EQ.0) RETURN
        
c        write(*,*)'generated: ',ntr,Q(JTM+14+3),Q(JTM+14+4)
        CALL UCOPY(VECT,VECTS,7)
        ELOSS          = 0
        DO 1 N         = 1,NTR


        NGKINE         = NGKINE+1
        GKIN(1,NGKINE) = ETR(N)*VECTS(4)
        GKIN(2,NGKINE) = ETR(N)*VECTS(5)
        GKIN(3,NGKINE) = ETR(N)*VECTS(6)
        GKIN(4,NGKINE) = ETR(N)
        if(Q(JTM+14+2).lt.2)then
         GKIN(5,NGKINE) = 48.
        else
         GKIN(5,NGKINE) = 1.
        endif
        GPOS(1,NGKINE) = VECTS(1)-STR(N)*VECTS(4)
        GPOS(2,NGKINE) = VECTS(2)-STR(N)*VECTS(5)
        GPOS(3,NGKINE) = VECTS(3)-STR(N)*VECTS(6)
        VECT(1)        = VECTS(1)-STR(N)*VECTS(4)
        VECT(2)        = VECTS(2)-STR(N)*VECTS(5)
        VECT(3)        = VECTS(3)-STR(N)*VECTS(6)
        TOFD (NGKINE)  = 0.
        IFLGK(NGKINE)  = 0
*        IFLGK(NGKINE)  = 1
        ELOSS          = ELOSS+ETR(N)



         CALL GSKING(NGKINE)




         NGKINE         = NGKINE-1
1       CONTINUE
C... Energy primary particle correction
        CALL UCOPY(VECTS,VECT,7)
        GETOT          = GETOT-ELOSS
        GEKIN          = GEKIN-ELOSS
        VECT(7)        = SQRT(GEKIN*(GEKIN+2.*AMASS))
        CALL GEKBIN
C...
 99    E N D


C-------------------------------------------------------
        FUNCTION SRADIUS (VECT,STEP,Q)
        implicit none
C
C... *** Calculate the min.distance of the track to wire ***
C
        DIMENSION VECT(7),R1(3),R1D(3),R2D(3),A(2)
        real step,q,vect,r1,r1d,r2d,a,s,sradius
C...if step = 0
C
        IF(STEP.NE.0.AND.Q.NE.0.) GO TO 2
        CALL GMTOD(VECT,R1D,1)
1       SRADIUS = SQRT(R1D(1)**2+R1D(2)**2)
*   STEP=0 used by SPACTRD
C
        RETURN
C
C...if step.ne.0.
C
2       R1(1)    = VECT(1)-STEP*VECT(4)
        R1(2)    = VECT(2)-STEP*VECT(5)
        R1(3)    = VECT(3)-STEP*VECT(6)
C
        CALL GMTOD(R1  ,R1D,1)
        CALL GMTOD(VECT,R2D,1)
        A(1)     = (R2D(1)-R1D(1))/STEP
        A(2)     = (R2D(2)-R1D(2))/STEP
        S        = -(R1D(1)*A(1)+R1D(2)*A(2))/(A(1)**2+A(2)**2)
        IF(S.LE.0.  ) GO TO 1
        IF(S.LT.STEP) GO TO 3
        SRADIUS  = SQRT(R2D(1)**2+R2D(2)**2)
        RETURN
C
3       SRADIUS  = SQRT((R1D(1)+S*A(1))**2+(R1D(2)+S*A(2))**2)
        RETURN
        E N D
C----------------------------------------------------------
        SUBROUTINE SPACTRD (VECT,STEP,DESTEP,NF,NUMS,Q)
        implicit none
C...
#       include "trdf.h"
C...
        DIMENSION VECT(7),VEC(3)
        real vect,step,destep,q,r,sradius,st,vec
        integer nf,nums,ih,n
C...
        IF(DESTEP.EQ.0) RETURN
        IF(Q.NE.0..AND.NCLESTEP.NE.0) GO TO 1
        R     = SRADIUS(VECT,0.,Q)
*        IH    = MIN(50,MAX(1,INT(R/.004)))
        IH=MIN(100,MAX(1,INT(SRADIUS(VECT,0.,Q)/.003)))
        SPTRD(IH,NUMS,NF) = SPTRD(IH,NUMS,NF)+DESTEP
*        PRINT *,'1,ih,nf',ih,nf,DESTEP,SPTRD(IH,NUMS,NF)
        RETURN
C...
1       DO 2 N = 1,NCLESTEP
        ST     = CLESTEP(2,N)
        VEC(1) = VECT(1)-VECT(4)*ST
        VEC(2) = VECT(2)-VECT(5)*ST
        VEC(3) = VECT(3)-VECT(6)*ST
*        IH     = MIN(50,MAX(1,INT(SRADIUS(VEC,0.,Q)/.004)))
        IH=MIN(100,MAX(1,INT(SRADIUS(VEC,0.,Q)/.003)))
        SPTRD(IH,NUMS,NF) = SPTRD(IH,NUMS,NF)+CLESTEP(1,N)
C...
2         CONTINUE
C...
        RETURN
        END
C
C ************************************************************
C *       PROGRAM FOR DE/DX-SIMULATION WITHIN STRAWS         *
C ************************************************************
C
        SUBROUTINE SIMDE(g3trd)
        implicit none
C
C... *** Simulation energy deposition within straw ***
C
#include    "geant321/gckine.inc"
#include    "geant321/gctrak.inc"
#include    "geant321/gcvolu.inc"
#include    "geant321/gccuts.inc"
#include     "geant321/gcbank.inc"
#include     "geant321/gcnum.inc"
#include     "geant321/gctmed.inc"
#include     "trdf.h"
        integer g3trd,nwbuf,jtm
        real gamma,de,derand
        IF(ISTOP      .NE.0 .OR.STEP         .LE.0.    ) RETURN
        IF(ABS(CHARGE).NE.1.)                            RETURN

*
*  only trd like media will play
*
      IF (JTMED.LE.0)GO TO 99
      IF (NUMED.LE.0)GO TO 99
      IF (NUMED.GT.NTMED)GO TO 99
      JTM = LQ(JTMED- NUMED)
      IF (JTM.LE.0)GO TO 99
      NWBUF = IQ(JTM-1) - 14
      IF(NWBUF.LT.4) GO TO 99
      if(Q(JTM+14+1).ne.g3trd.or.
     + Q(JTM+14+2).ne.0
     + .or.Q(JTM+14+3).ne.3)goto 99     ! 3 trd-gas
      
        
c        IF(NADE.NE.'GTRL'.AND.NADE.NE.'GTRS')            RETURN
C...
        GETOT   = GETOT+DESTEP
        GEKIN   = GEKIN+DESTEP
        GAMMA   = GETOT/AMASS
C...
        DE      = DERAND(GAMMA,DCUTE,STEP)
        IF(GEKIN-DE.GT.CUTELE) GO TO 1
C
C...Stop particle
C
        NCLESTEP = 0
        ISTOP   = 2
        DESTEP  = GEKIN
        DESTEL  = DESTEP
        GEKIN   = 0.
        GETOT   = AMASS
        VECT(7) = 0.
        CALL GEKBIN
        RETURN
C...
1       GEKIN   = GEKIN-DE
        GETOT   = GETOT-DE
        DESTEP  = DE
        DESTEL  = DESTEP
        VECT(7) = SQRT(GEKIN*(GEKIN+2.*AMASS))
C
        CALL GEKBIN
 99     RETURN
        E N D
*CMZ :          15/08/94  09.46.59  by  Valeri Saveliev
*-- Author :
C------------------------------------------------------------
        FUNCTION DERAND (GAMMA,ECUT,STEP)
C
C... ***  Simulation of energy loss straggling within straw
C         with energy smaler then ECUT ***
C
         implicit none
#include "trdf.h"
C
C NCLESTEP     - Total number clasters,
C CLESTEP(1,#) - energy of the # claster,
C CLESTEP(2,#) - coordinate of the # claster.
C
        DIMENSION ANGL(200),RNDM(2)
        real angl,rndm,gamma,ecut,step,gaml,derand,ec,dgg
        real anmin,anmax,dan,amu,r,ee,e
        integer i,k,n,ier,k1,k2
C...
        NCLESTEP = 0
        EC       = 1.E9*ECUT
        DERAND   = 0.
        IF(STEP.LE.0.) RETURN
C...
        GAML     = ALOG(GAMMA)
        IF(GAML.GE.GAME(21)) THEN
        CALL UCOPY(ANL(1,21),ANGL,NWE)
        GO TO 4
        END IF
C...
        DO 1 I  = 1,21
        IF(GAML.LE.GAME(I)) GO TO 2
1         CONTINUE
2       I       = MIN(21,MAX(I,2))
        DGG     = (GAML-GAME(I-1))/(GAME(I)-GAME(I-1))
C...
        DO 3 K  = 1,NWE
        ANGL(K) = ANL(K,I-1)+(ANL(K,I)-ANL(K,I-1))*DGG
3         CONTINUE
C...
4       ANMIN   = EXP(ANGL(NWE))
        ANMAX   = EXP(ANGL( 1 ))
C...
        DAN     = ANMAX-ANMIN
        AMU     = DAN*STEP
        CALL POISSN(AMU,N,IER)
        IF(N.LE.0.) RETURN
        DO 8 I  = 1,N
        K1      = 1
        K2      = NWE
        CALL GRNDM(RNDM,2)
        R       = ALOG(RNDM(1)*DAN+ANMIN)
6       K       = (K1+K2)/2
        IF(R.NE.ANGL(K)) GO TO 7
        EE      = EXP(WE(K))
        DERAND  = DERAND+EE
        GO TO 8
7       IF(R.GT.ANGL(K)) K2 = K
        IF(R.LT.ANGL(K)) K1 = K
        IF(K2-K1.GT.1  ) GO TO 6
C...
        E       = WE(K2)+(R-ANGL(K2))*(WE  (K1)-WE  (K2))/
     +                                (ANGL(K1)-ANGL(K2))
        EE      = EXP(E)
        IF(EE.GT.EC) GO TO 8
C
C..fill clasters bank
C
        NCLESTEP = MIN(4000,NCLESTEP+1)
        CLESTEP(1,NCLESTEP) = EE*1.E-9
        CLESTEP(2,NCLESTEP) = STEP*RNDM(2)
C
C...Total energy calculation
C
        DERAND  = DERAND+EE
8       CONTINUE
C...
        DERAND  = DERAND*1.E-9
        RETURN
        END
*CMZ :          16/06/99  14.57.04  by  AMS TRD: Kirn,Raupach,Siedenburg
*-- Author :
C---------------------------------------------------------
        FUNCTION SIMNOIS (E)
C
C... *** Electronic noise simulation ***
C
        implicit none
        real  simnois,fwhm,e,ee,spacechar,r1
        DATA FWHM/.27/


      REAL AMVEC
      DATA AMVEC / 100.0 /
      INTEGER NRVEC

C
C...Space charge effect simulation
C
        EE      = E*1.E6
        SIMNOIS = SPACECHAR(EE)
C...
        IF(FWHM.LE.0.) GO TO 1

*        CALL RANNOR(R1,R2)
         CALL GPOISS(AMVEC,NRVEC,1)     ! based on RANECU
         R1 = (real(NRVEC)-AMVEC)/SQRT(AMVEC)

        SIMNOIS = SIMNOIS+R1*FWHM/2.35*SQRT(6.*SIMNOIS)
1       SIMNOIS = SIMNOIS*1.E-6
        RETURN
        END


*CMZ :          27/03/94  12.17.14  by  Valeri Saveliev
*-- Author :
C--------------------------------------------------------
        FUNCTION SPACECHAR (E)
C
C... *** Space charge effect simulation (Sosnovcev gas gain
C        for mixture 50%Xe+25%CO2+25%CF4 ***
C
        implicit none
        real e,spacechar
        SPACECHAR = E
        IF(E.LE.2.) RETURN
        SPACECHAR = AMAX1(.2*E,E*(1.0314-.0157*E))
        RETURN
        END



*CMZ :          03/09/99  20.32.51  by  AMS TRD: Kirn,Raupach,Siedenburg
*-- Author :
C
C *********************************************************
C *      TR-photon absorbtion                             *
C *********************************************************
C
        SUBROUTINE TRPHOTON(g3trd)
        implicit none
C
C... *** TR-photon absorbtion simulation ***
C
#include "geant321/gctrak.inc"
#include "geant321/gctmed.inc"
#include "geant321/gckine.inc"
#include "geant321/gcbank.inc"
#include "geant321/gcking.inc"
#include "geant321/gcvolu.inc"
#include "geant321/gcnum.inc"
#include "trdf.h"

C...
      integer nwbuf,jtm,im,g3trd
      real stea,sabs
C...


*      Volume Types:
*      0: (Default) Convert TRD-Geantino to ordinary Photon
*      1: (RADIAT.) if absorbed then 4 else continue tracking
*      2: (TBWALL)  like 1
*      3: (GAS)     if absorbed then (new K,L TRD);4 else cont. tr.
*      4: (WIRE)    Stop TRD-Geantino   DESTEP=Etrd
*      5: (AIR)     like 1
*     10: (Cu)      like 1

*      DATA NAMM/'TRLB','TRSB','STRA','GTRL','GTRS','WIRE'/
*      DATA IMM /    1 ,    1 ,    2 ,    3 ,    3 ,    4 /
C...
c       DATA NAMM/'TRAD','STFA','TBMU','TBWL','TBML','TBGS','TBWI',
c     >           'STFM','STFS','STFF','TCLY','TRDU','TRDL',
c     >           'TBSS','TBSW'/
c       DATA IMM /    1,     1,     2,     2,     2,     3,     4,
c     >               5,    10,     2,     5,     5,     5,
c     >               2,     2 /
cC...

C...


C...
        IF(IPART.NE.48.OR.ISTOP.NE.0.OR.STEP.LE.0.) RETURN
C...


*  only trd like media will play
*
      IF (JTMED.LE.0)GO TO 1
      IF (NUMED.LE.0)GO TO 1
      IF (NUMED.GT.NTMED)GO TO 1
      JTM = LQ(JTMED- NUMED)
      IF (JTM.LE.0)GO TO 1
      NWBUF = IQ(JTM-1) - 14
      IF(NWBUF.LT.4) GO TO 1
      if(Q(JTM+14+1).ne.g3trd.or.
     +(Q(JTM+14+2).gt.1.or.Q(JTM+14+2).lt.0))goto 1


        IM=Q(JTM+14+3)
        goto 2
 1      continue 
C
C... stop Geantino and apparition Gamma
C


c         write(*,*)'absorbed somewhere ',gekin,vect(1),vect(2),vect(3)
        NGKINE         = NGKINE+1
        GKIN(1,NGKINE) = GEKIN*VECT(4)
        GKIN(2,NGKINE) = GEKIN*VECT(5)
        GKIN(3,NGKINE) = GEKIN*VECT(6)
        GKIN(4,NGKINE) = GEKIN
        GKIN(5,NGKINE) = 1.
*>>
        GPOS(1,NGKINE) = VECT(1)
        GPOS(2,NGKINE) = VECT(2)
        GPOS(3,NGKINE) = VECT(3)
*<<
        TOFD  (NGKINE) = 0.
        IFLGK (NGKINE) = 0
*        IFLGK (NGKINE) = 1
        CALL GSKING(NGKINE)

***


        NGKINE         = NGKINE-1
        ISTOP   = 1
        DESTEP  = 0.
        DESTEL  = 0.
        GEKIN   = 0.
        GETOT   = 0.
        VECT(7) = 0.
        CALL GEKBIN
        RETURN

C... stop TR-photon
2      continue 
       IF(IM.NE.4) GO TO 4

3       IF(IM.EQ.3) CALL EPHTRD(GEKIN)



        ISTOP   = 2
        DESTEP  = GEKIN
        DESTEL  = DESTEP
        GEKIN   = 0.
        GETOT   = 0.
        VECT(7) = 0.
c         write(*,*)'photon absorbed in ',Q(JTM+14+3),GEKIN,DESTEP
        CALL GEKBIN
        RETURN

C... Absorbtion simulation
4        STEA    = SABS(GEKIN,STEP,IM)
         IF(STEA.GT.STEP) then
c         write(*,*)'passed  trd ',im,gekin,vect(1),vect(2),vect(3)
            RETURN
         endif
c         write(*,*)'absorbed in trd ',im,gekin,vect(1),vect(2),vect(3)
         VECT(1) = VECT(1)+(STEA-STEP)*VECT(4)
         VECT(2) = VECT(2)+(STEA-STEP)*VECT(5)
         VECT(3) = VECT(3)+(STEA-STEP)*VECT(6)
         GO TO 3
C...
         E N D





*CMZ :          03/05/99  23.29.46  by  AMS TRD: Kirn,Raupach,Siedenburg
*-- Author :
C------------------------------------------------------------
        SUBROUTINE GENTRD (QCH,GAMMA,COR,STEP,NTR,ETR,STR)
C
C... *** Simulation TR-photon in STEP-length radiator ***
C
C  QCH       - abs charge
C  GAMMA     - Lorentz factor,
C  COR       - correction factor,
C  NTR       - number   TR photons,
C  ETR       - energy   TR photons (GeV),
C  STR       - distance of each photon.
C
          implicit none
#include "trdf.h"
C...
        DIMENSION ETR(1000),STR(1000),DNWMG(81)
        DIMENSION RNDM(2)
        real etr,str,dnwmg,step,cor,gamma
        real rndm,qch
        real gamtab,anmin,anmax,gl,dgg,dan,am
        integer ntr,jg,nfoil,i,jg1,n,jtab,ier
        integer k1,k2,k
        real r,e
        DATA GAMTAB/0./,DNWMG/81*0./,ANMIN/0./,ANMAX/0./
        DATA JG/0/,GL/0./

C...
        NTR     = 0
        NFOIL   = STEP/(FOIL+GAS)

**        WRITE(6,'(A,I6,3F12.6)')
**     >  ' ===> NFOIL,STEP,FOIL,GAS= ',NFOIL,STEP,FOIL,GAS

        IF(GAMMA.LT.EXP(GINWM(1)).OR.NFOIL.LE.0 ) RETURN
        IF(ABS(GAMMA-GAMTAB).LE.5.              ) GO TO 6
C...
        GAMTAB  = GAMMA
        GL      = ALOG(GAMMA)
*       print *,'*gentrd*',gamma,gl,ginwm(1),ginwm(20)
*       print *,'*gentrd*',FOIL,GAS,STEP
        IF(GL.LT.GINWM(20)) GO TO 3
        DO 2 I  = 1,81
2       DNWMG(I)= DNM(I,20)
        JG      = 20
        GO TO 5
C...
3       JG      = JTAB(GL,20,GINWM)
        JG1     = JG+1
        DGG     = (GL-GINWM(JG))/(GINWM(JG1)-GINWM(JG))
        DO 4 I  = 1,81
4       DNWMG(I)= (DNM(I,JG1)-DNM(I,JG))*DGG+DNM(I,JG)
C...
5       ANMIN   = EXP(DNWMG(81))
        ANMAX   = EXP(DNWMG(1 ))
C...
6       DAN     = ANMAX-ANMIN
        AM      = COR*DAN*NFOIL*QCH**2
C...
C...... DANGER
*       AM = 0.
        CALL POISSN(AM,N,IER)

**        WRITE(6,'(A,F12.6,I6,2F12.6,I6)'))
**     >  ' *gentrd DAN,NFOIL,COR,AM,N= ',DAN,NFOIL,COR,AM,N

        IF(N.LE.0) RETURN
C...
        N       = MIN(N,1000)
        DO 10 I = 1,N
        K1      = 1
        K2      = 81
        CALL GRNDM(RNDM,2)
        R       = ALOG(RNDM(1)*DAN+ANMIN)
7       K       = (K1+K2)/2
        IF(R.NE.DNWMG(K)) GO TO 8
        E       = EXP(WTR(K))
        K2      = K
        GO TO 9
8       IF(R.GT.DNWMG(K)) K2 = K
        IF(R.LT.DNWMG(K)) K1 = K
        IF(K2-K1.GT.1) GO TO 7
C...
        E       = WTR(K2)+(R-DNWMG(K2))*(WTR  (K1)-WTR  (K2))/
     +                                  (DNWMG(K1)-DNWMG(K2))
        E       = EXP(E)
C...
9       NTR         = NTR+1
        ETR(NTR)    = E*1.E-6
        STR(NTR)    = STEP*RNDM(2)
10      CONTINUE
C...
        RETURN
        E N D









*CMZ :          18/11/95  19.45.02  by  Valeri Saveliev
*-- Author :
C---------------------------------------------------------
        SUBROUTINE GENTRDI (QCH,GAMMA,STEP,NTR,ETR,STR)
C
C... *** TR-generation within straw ***
C
         implicit none
#include "trdf.h"
C...
        DIMENSION RNDM(2),ETR(1000),STR(1000)
        real QCH,GAMMA,STEP,ETR,STR
        integer ntr
        real gaml,ant,wmi,rndm,wma,w0,s1,s2,g2,a0,e,e2
        DATA GAML/0./,ANT/0./
C...
        NTR     = 0
        WMI     =       TDATA(2,3)*2. E-3
        WMA     = GAMMA*TDATA(2,4)*.25E-3
        IF(WMA.LE.WMI) RETURN
        CALL GRNDM(RNDM,2)
C...
        IF(ABS(GAMMA-GAML).GT.5.) THEN
        GAML    = GAMMA
        W0      = (WMA+WMI)/2.
        S1      = (1.E-3*TDATA(2,3)/W0)**2
        S2      = (1.E-3*TDATA(2,4)/W0)**2
        G2      = 1./GAMMA**2
        A0      = 2.323E-3*((S1+S2+2.*G2)/(S1-S2)*
     +            ALOG((G2+S1)/(G2+S2))-2.)
        ANT     = A0*ALOG(WMA/WMI)
        END IF
C...
        IF(RNDM(1).GT.ANT) RETURN
        E        = WMI*(WMA/WMI)**(RNDM(2))
        IF(E.GT.5.) RETURN
        NTR      = 1
        ETR(1)   = E*1.E-6
        STR(1)   = STEP
        RETURN
        E N D
*CMZ :          11/08/99  20.26.17  by  AMS TRD: Kirn,Raupach,Siedenburg
*-- Author :
C----------------------------------------------------------
        SUBROUTINE EPHTRD (E0)
C
C... *** Simulate res within straw in case photon absorb ***
C
C...Kinematics of generated secondaries
C
            implicit none
            real e0
#include    "geant321/gckine.inc"
#include    "geant321/gcking.inc"
#include    "geant321/gctrak.inc"
#include    "geant321/gcvolu.inc"
             

C...

        DIMENSION RNDM(3)
        real rndm
        real ef,pm,pl,pk,e2
        real eabs(3)
        data EABS/34.56,4.8,0.6/
C...





        EF     = E0*1.E6
        CALL FRA(EF,PM,PL,PK,EABS)
        CALL GRNDM(RNDM,2)
C...
*
* Bug Fixed V. Choutko 29.02.2000
*
        IF(PM+PL+PK.eq.0.or.RNDM(1).LT.PM)goto 999
        IF(RNDM(1).GT.(PM+PL))then
*
* K
*
         IF(RNDM(2).GT.0.88) goto 999   !no fluo
         E2=(EABS(1)-EABS(2))*1.e-6
        else
*
* L
*
         IF(RNDM(2).GT..11)        goto 999       ! no fluo
         E2     = (EABS(2)-EABS(3))*1.e-6
        endif
         E0=E0-E2


C
C... Simulation new photon
C
        NGKINE   = NGKINE+1
        CALL ISOTR(GKIN(1,NGKINE))
        GKIN(1,NGKINE) = GKIN(1,NGKINE)*E2
        GKIN(2,NGKINE) = GKIN(2,NGKINE)*E2
        GKIN(3,NGKINE) = GKIN(3,NGKINE)*E2
        GKIN(4,NGKINE) = E2
        GKIN(5,NGKINE) = 48.
        TOFD  (NGKINE) = 0.
        IFLGK (NGKINE) = 0
*        IFLGK (NGKINE) = 1
*>>
        GPOS(1,NGKINE) = VECT(1)
        GPOS(2,NGKINE) = VECT(2)
        GPOS(3,NGKINE) = VECT(3)
*<<
        CALL GSKING(NGKINE)


***

        NGKINE         = NGKINE-1
 999    continue
c        write(*,*)'xe interacton ',ef,e0,e2
        if(e0.lt.0)then
         write(*,*)' bug********************* '
        endif

C...
       E N D

*CMZ :          27/03/94  12.17.14  by  Valeri Saveliev
*-- Author :
C---------------------------------------------------------
        SUBROUTINE FRA (E,PM,PL,PK,EABS)
        real EABS(3)
         
C
C... *** Calculate fraction different mode of interaction
C        photons with K,L,M-electron shell of XE
C        PM,PL,PK - propability to interact with M,L,K-shells ***
C
C
C...real photon
C
        IF(E.GT.EABS(1))then
         PK      = .83
         PL      = .145
         PM      = .025
        else if(E.GT.EABS(2))then
         PK=0
         PL      = .84
         PM      = .16
        else if(E.GT.EABS(3))then
         PK=0
         PL=0
         PM=1
        else
         PK=0
         PL=0
         PM=0
        endif
        E N D
*CMZ :          27/03/94  12.17.14  by  Valeri Saveliev
*-- Author :
C------------------------------------------------------
        SUBROUTINE I S O T R (P)
C
C... *** Simulation isotropic direction ***
C
        DIMENSION P(3),RNDM(3)
C...
1       CALL GRNDM(RNDM,3)
        DO 2 K = 1,3
        P(K)   = RNDM(K)-.5
2         CONTINUE
C...
        SP     = P(1)**2+P(2)**2+P(3)**2
        IF(SP.GT..25) GO TO 1
        SQSP   = SQRT(SP)
        DO 3 K = 1,3
        P(K)   = P(K)/SQSP
3         CONTINUE
        RETURN
        E N D
*CMZ :          28/04/99  18.59.14  by  AMS TRD: Kirn,Raupach,Siedenburg
*-- Author :
C---------------------------------------------------------
        FUNCTION SABS (E0,STEP,IM)
        implicit none
C...
#include "trdf.h"
C...
      
        DIMENSION RNDM(1)
        real rndm,sabs,e0,step,e,am,amuf
        integer im
C...
        E      = E0*1.E6              ! E in keV
        IF(IM.EQ.1) then
          AM = (AMUF(E,1)*FOIL+AMUF(E,2)*GAS)/(GAS+FOIL) !RAD
        else IF(IM.EQ.2)then
           AM = AMUF(E,3)   ! Straw
        else IF(IM.EQ.3)then
           AM = AMUF(E,4)   ! Gas
        else IF(IM.EQ.5) then
            AM = AMUF(E,2)   ! Air
        else IF(IM.EQ.10) then
          AM = AMUF(E,10)  ! Copper
        else IF(IM.EQ.6) then
          SABS=1.e10        ! Vacuum
          goto 999 
        else
         SABS=0
         write(*,*) 'sabs error '
         goto 999
        endif
C...
        CALL GRNDM(RNDM,1)
        SABS   = -1./AM * ALOG(AMAX1(1.E-20,RNDM(1)))
c        write(*,*)' mean path length for ',im,1./am,e,step,sabs

 999    RETURN
        E N D
*CMZ :          03/05/99  23.40.28  by  AMS TRD: Kirn,Raupach,Siedenburg
*-- Author :
C----------------------------------------------------------
        FUNCTION A M U F (X,IM)
C
*       X:  E in keV
*       IM: Material

#include "trdf.h"
       

        INTEGER    NBIN_cu
        PARAMETER (NBIN_cu=12)
        REAL       RHO_cu, MU_cu(NBIN_cu), EBIN_cu(NBIN_cu)
        PARAMETER (RHO_cu=8.96)                 ! g/cm**3
        DATA MU_cu   / 1.0e4, 4.2e3, 2.1e3, 720.0, 330.0, 180.0,
     >                 110.0,  51.0, 210.0,  73.0,  34.0,  11.0 /
        DATA EBIN_cu /   1.0,   1.5,   2.0,   3.0,   4.0,   5.0,
     >                   6.0,   8.0,  10.0,  15.0,  20.0,  30.0 /

C...
        IF(IM.GE.1.AND.IM.LE.4) THEN   ! from generated Tables:

          AMUF = TABLOG(X,30,WT,AMUTT(1,IM))*TDATA(1,IM)

        ELSEIF( IM.EQ.10 ) THEN        ! Copper

          DO i=NBIN_cu,1,-1
            IF( X.gt.EBIN_cu(i) ) GOTO 10
          ENDDO
          IF( i.lt.1 ) i=1
 10       AMUF = MU_cu(i)*RHO_cu
**          WRITE(6,'(A,F12.1,I4,F12.1)') 'AMUF Cu: ', X,i,1.e4/AMUF

        ELSE                           ! This should not happen

          PRINT *,'ERR AMUF',IM
          STOP

        ENDIF

C...
        RETURN
        E N D


*CMZ :          18/02/99  14.40.26  by  Valeri Saveliev
*-- Author :
C----------------------------------------------------------
        integer FUNCTION   J T A B (X,N,XM)
C
*       DIMENSION XM(20)
        DIMENSION XM(30)
C...
*       WRITE(6,*)' N= ',N
        IF(X.GT.XM(1)) GO TO 1
        JTAB  = 1
        RETURN
1       IF(X.LT.XM(N)) GO TO 2
        JTAB  = N-1
        RETURN
2       J1    = 1
        J2    = N
3       J     = (J1+J2)/2
        IF(X.NE.XM(J)) GO TO 4
        JTAB  = J
        RETURN
4       IF(X.LT.XM(J)) J2 = J
        IF(X.GT.XM(J)) J1 = J
        IF(J2-J1.GT.1) GO TO 3
        JTAB  = J1
        RETURN
        E N D
*CMZ :          18/02/99  14.40.01  by  Valeri Saveliev
*-- Author :
C-----------------------------------------------------------
        FUNCTION T A B L O G (X,N,XM,YM)
C
C... *** EXTRAC FROM TABLE BY USE LOG-LINEAR EXTRAPOLATION ***
C
*       DIMENSION XM(20),YM(20)
        DIMENSION XM(30),YM(30)
        INTEGER K,K1,K2,N
C...
        TABLOG = 0.
        IF(N.LE.0) RETURN
        K1     = 1
        K2     = N
1       K      = INT((K1+K2)/2.)
*       WRITE(6,*)' K= ',X,K,K1,K2
        IF(X.NE.XM(K)) GO TO 2
        TABLOG = YM(K)
        RETURN
2       IF(X.LT.XM(K)) K2 = K
        IF(X.GT.XM(K)) K1 = K
        IF(K2-K1.GT.1) GO TO 1
        Y1     = ALOG(YM(K1))
        Y2     = ALOG(YM(K2))
        X1     = ALOG(XM(K1))
        X2     = ALOG(XM(K2))
C...
        X0     = ALOG(X)
        Y      = Y1+(Y2-Y1)/(X2-X1)*(X0-X1)
        TABLOG = EXP(Y)
        RETURN
        E N D
*CMZ :          19/03/99  13.30.03  by  Valeri Saveliev
*-- Author :
C----------------------------------------------------------
        SUBROUTINE INDETRA
        implicit none
C
C... *** Input information from DETRA1,DETRA2,DETRA3 files ***
C
#include "trdf.h"
#include "mfield.h"
         real ac,zc,roc
         integer nnwe,k,i,ig
C
C
C...read mumber of collision per unit length for gas of straw
C

         OPEN (21,FILE=amsdatadir(1:amsdlength)//'trd.aachen.dat',
     +          FORM='FORMATTED',STATUS='OLD')
         READ (21,*) AC,ZC,ROC,NWE,WE,ANL,GAME
         READ (21,*)TDATA,WT,AMUTT,FOIL,GAS,GINWM,DNM,WTR
         READ (21,*) YTSYN,FTSYN
         CLOSE(21)
         CLOSE(21)
*        print *,'*AC  *',ac
*        print *,'*ZC  *',zc
*        print *,'*ROC *',roc
*        print *,'*NWE *',nwe
*        print *,'*WE  *',we
*        print *,'*ANL *',anl
*        print *,'*GAME*',game
C-----------------------------------------------------------------------
C...
        NNWE     = 0
        DO 2 K   = 1,NWE
        WE(K)    = ALOG(WE(K))
        DO 1 I   = 1,21
        IF(ANL(K,I).LE.0.) GO TO 3
        ANL(K,I) = ALOG(ANL(K,I))
1         CONTINUE
        NNWE     = NNWE+1
2         CONTINUE
3       NWE      = NNWE
C...
        DO 4 I   = 1,21
        GAME(I)  = ALOG(GAME(I))
4         CONTINUE
C
C...read TR and absorbtion tables
C
*        print *,'*TDATA*',tdata
*        print *,'*WT   *',wt
*        print *,'*AMUTT*',amutt
c         print *,'*FOIL *',foil
c         print *,'*GAS  *',gas
*        print *,'*GINWM*',ginwm
*        print *,'*DNM  *',dnm
*        print *,'*WTR  *',wtr
C...
        DO 5 IG   = 1,20
        GINWM(IG) = ALOG(GINWM(IG))
        DO 5 K    = 1,81
        IF(IG.EQ.1) WTR(K) = ALOG(WTR(K))
        DNM(K,IG) = ALOG(DNM(K,IG))
5         CONTINUE
C
C...read tables for the synchrotron radiation simulation
C
*        print *,'*YTSIN*',ytsin
*        print *,'*FTSYN*',ftsin

C...
        RETURN
        E N D
*CMZ :          02/08/99  19.30.02  by  AMS TRD: Kirn,Raupach,Siedenburg

      SUBROUTINE GPHYSI
C.
C.    ******************************************************************
C.    *                                                                *
C     *       Initialise material constants for all the physics        *
C.    *       mechanisms used by GEANT                                 *
C.    *                                                                *
C.    *    ==>Called by : <USER>, UGINIT                               *
C.    *       Author    R.Brun  *********                              *
C.    *                                                                *
C.    ******************************************************************
C.
#include "geant321/gcbank.inc"
#include "geant321/gcphys.inc"
#include "geant321/gccuts.inc"
#include "geant321/gcflag.inc"
#include "geant321/gcjloc.inc"
#include "geant321/gclist.inc"
#include "geant321/gcmulo.inc"
#include "geant321/gctmed.inc"
#include "geant321/gcmate.inc"
#include "geant321/gcnum.inc"
#include "geant321/gconsp.inc"
#include "geant321/gctime.inc"
#include "geant321/gctrak.inc"
#include "geant321/gcunit.inc"
      DIMENSION CUTS(10),UCUT(10),MECA(5,13)
      EQUIVALENCE (CUTS(1),CUTGAM),(MECA(1,1),IPAIR)
      CHARACTER*4 DNAME,KCUT(10)
      CHARACTER*20 CHTITL
      LOGICAL NUCRIN
C.
C.    ------------------------------------------------------------------
C.
C              Write RUN parameters, version numbers and CUTS
C
      WRITE(CHMAIL,10000)
      CALL GMAIL(0,0)
      WRITE(CHMAIL,10100)
      CALL GMAIL(0,0)
      WRITE(CHMAIL,10200)GVERSN,IGDATE,IGTIME
      CALL GMAIL(0,0)
      WRITE(CHMAIL,10100)
      CALL GMAIL(0,0)
      WRITE(CHMAIL,10300)IDRUN
      CALL GMAIL(0,0)
      WRITE(CHMAIL,10100)
      CALL GMAIL(0,0)
      WRITE(CHMAIL,10400)
      CALL GMAIL(0,0)
      WRITE(CHMAIL,10100)
      CALL GMAIL(0,0)
      WRITE(CHMAIL,10500)
      CALL GMAIL(0,0)
      WRITE(CHMAIL,10600)
      CALL GMAIL(0,0)
      WRITE(CHMAIL,10100)
      CALL GMAIL(0,0)
C
C              Get the version number of the original INIT structure
C
      OLDGVE=BIG
*
*        Set NUMOLD to 0 to force recalculation of
*        pointers in the tracking routines
      NUMOLD=0
      IF(JRUNG.NE.0)THEN
         OLDGVE =  Q(JRUNG+21)
         IQ(JRUNG+11)=IGDATE
         IQ(JRUNG+12)=IGTIME
         Q(JRUNG+21)=GVERSN
         Q(JRUNG+22)=ZVERSN
C
         DNAME='INIT'
         WRITE(CHMAIL,10700) DNAME,IQ(JRUNG+11),IQ(JRUNG+12), Q(JRUNG+
     +   21), Q(JRUNG+22)
         CALL GMAIL(0,0)
         WRITE(CHMAIL,10100)
         CALL GMAIL(0,0)
         DNAME='KINE'
         WRITE(CHMAIL,10700) DNAME,IQ(JRUNG+13),IQ(JRUNG+14), Q(JRUNG+
     +   23), Q(JRUNG+24)
         CALL GMAIL(0,0)
         WRITE(CHMAIL,10100)
         CALL GMAIL(0,0)
         DNAME='HITS'
         WRITE(CHMAIL,10700) DNAME,IQ(JRUNG+15),IQ(JRUNG+16), Q(JRUNG+
     +   25), Q(JRUNG+26)
         CALL GMAIL(0,0)
         WRITE(CHMAIL,10100)
         CALL GMAIL(0,0)
         DNAME='DIGI'
         WRITE(CHMAIL,10700) DNAME,IQ(JRUNG+17),IQ(JRUNG+18), Q(JRUNG+
     +   27), Q(JRUNG+28)
         CALL GMAIL(0,0)
         WRITE(CHMAIL,10100)
         CALL GMAIL(0,0)
         IF(NRNDM(1).EQ.0.AND.NRNDM(2).EQ.0) THEN
*
*             The random number sequence has not been explicitely
*             initialised via a data card. See whether we can initialise
*             it with the words 19/20 of the JRUNG data structure.
            IF(IQ(JRUNG+19).NE.0.OR.IQ(JRUNG+20).NE.0) THEN
               NRNDM(1) = IQ(JRUNG+19)
               NRNDM(2) = IQ(JRUNG+20)
               CALL GRNDMQ(NRNDM(1), NRNDM(2), 0, 'S')
            ENDIF
         ENDIF
         CALL GRNDMQ(IQ(JRUNG+19), IQ(JRUNG+20), 0, 'G')
         WRITE(CHMAIL,10900) IQ(JRUNG+19), IQ(JRUNG+20)
         CALL GMAIL(0,0)
         WRITE(CHMAIL,11000)
         CALL GMAIL(0,0)
         WRITE(CHMAIL,10100)
         CALL GMAIL(0,0)
      ENDIF
C
C             Create energy loss and cross-section banks
C
      IF(NEKBIN.LE.0.OR.NEKBIN.GT.199)NEKBIN=100
      IF(EKMIN.GE.EKMAX.OR.EKMIN.LE.0.)THEN
         EKMIN=1.E-6
         EKMAX=1.E+4
      ENDIF
      IF(NEKBIN.EQ.90.AND.EKMIN.EQ.1.E-5.AND.EKMAX.EQ.1.E4)THEN
         EKMIN=1.E-6
         EKMAX=1.E+4
         NEKBIN=100
      ENDIF
      NEK1=NEKBIN+1
      EKINV=1./LOG10(EKMAX/EKMIN)
      EKBIN(1)=LOG10(EKMIN)
      ELOW(1)=EKMIN
      GEKA=NEKBIN*EKINV
      GEKB=1.-GEKA*EKBIN(1)
      DO 10 I=2,NEK1
         EL=EKBIN(1)+(I-1)/GEKA
         EKBIN(I)=EL
         ELOW(I)=10.**EL
   10 CONTINUE
      ILOW=0
      IF(NMATE.LE.0)GO TO 999
      IF(JMATE.LE.0)GO TO 999
      IF(JTMED.LE.0)GO TO 999
C
      IF(IQ(JTMED-1).LT.40) THEN
         NPUSH=40-IQ(JTMED-1)
         CALL MZPUSH(IXCONS,JTMED,0,NPUSH,'I')
      END IF
      Q(JTMED+31)=ILABS
      Q(JTMED+32)=ISYNC
      Q(JTMED+33)=ISTRA
*
*             If Landau fluctuations activated, cancel delta rays
      KLOS=Q(JTMED+21)
      IF (KLOS .EQ. 0) Q(JTMED+15) = 0.
      IF (KLOS .EQ. 2) THEN
         Q(JTMED+ 8)=9999.
         Q(JTMED+ 9)=9999.
         Q(JTMED+15)=0.
      ENDIF
*
* If Cerenkov generation is on, activate Light absorbtion unless
* explicitely switched off by the user
*
      KLABS=Q(JTMED+31)
      IF(ITCKOV.NE.0) THEN
         IF(KLABS.EQ.-1) THEN
            Q(JTMED+31)=1
         ENDIF
      ENDIF
      Q(JTMED+31)=MAX(Q(JTMED+31),0.)
*
*             If BCUTE,BCUTM,DCUTE,DCUTM,PPCUTM not initialized (=BIG)
*             Set them to CUTGAM,CUTGAM,CUTELE,CUTELE respectively
*
      IF(Q(JTMED+ 6).GT.0.9*BIG)Q(JTMED+ 6)=Q(JTMED+1)
      IF(Q(JTMED+ 7).GT.0.9*BIG)Q(JTMED+ 7)=Q(JTMED+1)
      IF(Q(JTMED+ 8).GT.0.9*BIG)Q(JTMED+ 8)=Q(JTMED+2)
      IF(Q(JTMED+ 9).GT.0.9*BIG)Q(JTMED+ 9)=Q(JTMED+2)
      IF(Q(JTMED+10).GT.0.9*BIG)Q(JTMED+10)=0.010
      IF(Q(JTMED+10).LT.4.*EMASS)Q(JTMED+10)=4.*EMASS
C
      DO 20 K=1,10
   20 CALL GEVKEV(Q(JTMED+K),UCUT(K),KCUT(K))
      WRITE(CHMAIL,10800)
      CALL GMAIL(0,0)
      WRITE(CHMAIL,10100)
      CALL GMAIL(0,0)
      WRITE(CHMAIL,11100)
      CALL GMAIL(0,0)
      WRITE(CHMAIL,11200)
      CALL GMAIL(0,0)
      WRITE(CHMAIL,10100)
      CALL GMAIL(0,0)
      WRITE(CHMAIL,11300) (UCUT(K),KCUT(K),K=1,3)
      CALL GMAIL(0,0)
      WRITE(CHMAIL,11400) (UCUT(K),KCUT(K),K=4,5)
      CALL GMAIL(0,0)
      WRITE(CHMAIL,11500) (UCUT(K),KCUT(K),K=6,7)
      CALL GMAIL(0,0)
      WRITE(CHMAIL,11600) (UCUT(K),KCUT(K),K=8,10)
      CALL GMAIL(0,0)
      WRITE(CHMAIL,11700) (Q(JTMED+K),K=11,13)
      CALL GMAIL(0,0)
      WRITE(CHMAIL,11800) (Q(JTMED+K),K=14,16)
      CALL GMAIL(0,0)
      IF(Q(JTMED+18).EQ.3.) THEN
         NUCRIN = .TRUE.
         Q(JTMED+18)=1.
      ELSE
         NUCRIN = .FALSE.
      ENDIF
      WRITE(CHMAIL,11900) (Q(JTMED+K),K=17,19)
      CALL GMAIL(0,0)
      WRITE(CHMAIL,12000) (Q(JTMED+K),K=20,22)
      CALL GMAIL(0,0)
      WRITE(CHMAIL,12100) Q(JTMED+23),Q(JTMED+31),Q(JTMED+32)
      CALL GMAIL(0,0)
      WRITE(CHMAIL,12110) Q(JTMED+33)
      CALL GMAIL(0,0)
      WRITE(CHMAIL,10100)
      CALL GMAIL(0,0)
      IF(NUCRIN) THEN
         WRITE(CHMAIL,10100)
         CALL GMAIL(0,0)
         WRITE(CHMAIL,12800)
         CALL GMAIL(0,0)
         WRITE(CHMAIL,12900)
         CALL GMAIL(0,0)
         WRITE(CHMAIL,10100)
         CALL GMAIL(0,0)
      ENDIF
*
* ***    Here we clean up the old cross section tables if any
      DO 40  IMA=1,NMATE
         JMA=LQ(JMATE-IMA)
         IF(JMA.NE.0) THEN
            DO 30  J=1,20
               IF(LQ(JMA-J).NE.0.AND.J.NE.5) THEN
                  CALL MZDROP(IXCONS,LQ(JMA-J),'L')
               ENDIF
   30       CONTINUE
         ENDIF
   40 CONTINUE
*
* *** Call initialisation of the phtotelectric effect constants
      CALL GPHINI
      DO 180 ITM=1,NTMED
         JTM=LQ(JTMED-ITM)
         IF(JTM.LE.0) GO TO 180
         NL=10-IQ(JTM-2)
         IF(NL.GT.0)THEN
            CALL MZPUSH(IXCONS,JTM,NL,0,'I')
            JTM=LQ(JTMED-ITM)
         ENDIF
*         IF(IQ(JTM-1).LT.40) THEN
*           NPUSH=40-IQ(JTM-1)
*           CALL MZPUSH(IXCONS,JTM,0,NPUSH,'I')
*           JTM=LQ(JTMED-ITM)
*         ENDIF
         ISVOL = Q(JTM + 7)
         IFIELD = Q(JTM + 8)
         FIELDM = Q(JTM + 9)
         TMAXFD = Q(JTM + 10)
         STEMAX = Q(JTM + 11)
         DEEMAX = Q(JTM + 12)
         EPSIL = Q(JTM + 13)
         STMIN = Q(JTM + 14)
         IF (TMAXFD.LE.0..OR. (IGAUTO.NE.0.AND.TMAXFD.GT.20.)) THEN
            TMAXFD=20.
            Q(JTM+10) = TMAXFD
         ENDIF
         NMAT = Q(JTM+6)
         JMA = LQ(JMATE-NMAT)
         IF(JMA.LE.0)THEN
            WRITE(CHMAIL,12200)NMAT,ITM
            CALL GMAIL(1,1)
            GO TO 180
         ENDIF
C
C=====>       Get material parameters
C
         A=Q(JMA+6)
         Z=Q(JMA+7)
         DENS=Q(JMA+8)
         RADL=Q(JMA+9)
         IF (Z.LT.1.) THEN
            DEEMAX=0.
            STMIN =0.
            JTP=LQ(JTM)
            IF(JTP.EQ.0) THEN
               CALL MZBOOK(IXCONS,JTP,JTM,0,'TCUT',0,0,40,3,0)
               IQ(JTP-5)=ITM
               DO 50 I=1,23
                  Q(JTP+I)=Q(JTMED+I)
   50          CONTINUE
               Q(JTP+31)=Q(JTMED+31)
               Q(JTP+32)=Q(JTMED+32)
               Q(JTP+33)=Q(JTMED+33)
            ELSEIF(IQ(JTP-1).LT.40) THEN
               NPUSH=40-IQ(JTP-1)
               CALL MZPUSH(IXCONS,JTP,0,NPUSH,'I')
               JTP=LQ(JTM)
               Q(JTP+31)=Q(JTMED+31)
               Q(JTP+32)=Q(JTMED+32)
               Q(JTP+33)=Q(JTMED+33)
            ENDIF
C
C=====>     decay and synch. rad. in vacuum
C
            DO 60 I=11,23
               Q(JTP+I)=0.
   60       CONTINUE
            Q(JTP+20) = Q(JTMED+20)
            Q(JTP+31) = 0.
            Q(JTP+32) = Q(JTMED+32)
            Q(JTP+33) =0.
         ENDIF
C
C=====>       Get tracking medium parameters
C
         JTP=JTMED
         IF(LQ(JTM).NE.0)JTP=LQ(JTM)
         IF(JTP.NE.JTMED)THEN
            IF(IQ(JTP-1).LT.40) THEN
               NPUSH=40-IQ(JTP-1)
               CALL MZPUSH(IXCONS,JTP,0,NPUSH,'I')
               JTP=LQ(JTM)
               Q(JTP+31)=Q(JTMED+31)
               Q(JTP+32)=Q(JTMED+32)
               Q(JTP+33)=Q(JTMED+33)
            ENDIF
            KLOS=Q(JTP+21)
            IF (KLOS .EQ. 2) THEN
               Q(JTP+ 8)=9999.
               Q(JTP+ 9)=9999.
               Q(JTP+15)=0.
            ENDIF
*
* If Cerenkov generation is on, activate Light absorbtion unless
* explicitely switched off by the user
*
            KLABS=Q(JTP+31)
            IF(ITCKOV.NE.0) THEN
               IF(KLABS.EQ.-1) THEN
                  Q(JTP+31)=1
               ENDIF
            ENDIF
            Q(JTP+31)=MAX(Q(JTP+31),0.)
            IF(Q(JTP+ 6).GT.0.9*BIG)Q(JTP+ 6)=Q(JTP+1)
            IF(Q(JTP+ 7).GT.0.9*BIG)Q(JTP+ 7)=Q(JTP+1)
            IF(Q(JTP+ 8).GT.0.9*BIG)Q(JTP+ 8)=Q(JTP+2)
            IF(Q(JTP+ 9).GT.0.9*BIG)Q(JTP+ 9)=Q(JTP+2)
            IF(Q(JTP+10).GT.0.9*BIG)Q(JTP+10)=0.010
            IF(Q(JTP+10).LT.4.*EMASS)Q(JTP+10)=4.*EMASS
*
            CALL UHTOC(IQ(JTM+1),4,CHTITL,20)
            LAST=LNBLNK(CHTITL)
            IF(LAST.GT.0) THEN
               IF(CHTITL(LAST:LAST).EQ.'$') LAST=LAST-1
               IF(LAST.LT.20) CHTITL(LAST+1:20)=' '
            ENDIF
*
            DO 70 K=1,10
   70       CALL GEVKEV(Q(JTP+K),UCUT(K),KCUT(K))
            WRITE(CHMAIL,10100)
            CALL GMAIL(0,0)
            WRITE(CHMAIL,12300)ITM,CHTITL
            CALL GMAIL(0,0)
            WRITE(CHMAIL,12400)
            CALL GMAIL(0,0)
            WRITE(CHMAIL,11300) (UCUT(K),KCUT(K),K=1,3)
            CALL GMAIL(0,0)
            WRITE(CHMAIL,11400) (UCUT(K),KCUT(K),K=4,5)
            CALL GMAIL(0,0)
            WRITE(CHMAIL,11500) (UCUT(K),KCUT(K),K=6,7)
            CALL GMAIL(0,0)
            WRITE(CHMAIL,11600) (UCUT(K),KCUT(K),K=8,10)
            CALL GMAIL(0,0)
            WRITE(CHMAIL,11700) (Q(JTP+K),K=11,13)
            CALL GMAIL(0,0)
            WRITE(CHMAIL,11800) (Q(JTP+K),K=14,16)
            CALL GMAIL(0,0)
            IF(Q(JTP+18).EQ.3.) THEN
               NUCRIN = .TRUE.
               Q(JTP+18)=1.
            ELSE
               NUCRIN = .FALSE.
            ENDIF
            WRITE(CHMAIL,11900) (Q(JTP+K),K=17,19)
            CALL GMAIL(0,0)
            WRITE(CHMAIL,12000) (Q(JTP+K),K=20,22)
            CALL GMAIL(0,0)
            WRITE(CHMAIL,12100) Q(JTP+23),Q(JTP+31),Q(JTP+32)
            CALL GMAIL(0,0)
            WRITE(CHMAIL,12110) Q(JTP+33)
            CALL GMAIL(0,0)
            WRITE(CHMAIL,10100)
            CALL GMAIL(0,0)
            IF(NUCRIN) THEN
               WRITE(CHMAIL,10100)
               CALL GMAIL(0,0)
               WRITE(CHMAIL,12800)
               CALL GMAIL(0,0)
               WRITE(CHMAIL,12900)
               CALL GMAIL(0,0)
               WRITE(CHMAIL,10100)
               CALL GMAIL(0,0)
            ENDIF
         ENDIF
C
         DO 80 I=1,10
            CUTS(I)=Q(JTP+I)
   80    CONTINUE
         DO 90 I=1,13
            MECA(1,I)=Q(JTP+10+I)
   90    CONTINUE
         ILABS=Q(JTP+10+21)
         ISYNC=Q(JTP+10+22)
         ISTRA=Q(JTP+10+23)
C
         IF(ILOW.EQ.0)THEN
            DO 100 I=1,10
               IF(Q(JTP+I).LT.EKMIN)THEN
                  WRITE(CHMAIL,12500)
                  CALL GMAIL(1,1)
                  ILOW=1
               ENDIF
  100       CONTINUE
         ENDIF
C
C             Check consistency of different tracking media
C             referencing the same material
C
         DO 120 ITM2=ITM+1,NTMED
            JTM2=LQ(JTMED-ITM2)
            IF(JTM2.NE.0)THEN
               NMAT2=Q(JTM2+6)
               IF(NMAT2.EQ.NMAT)THEN
                  JTP2=JTMED
                  IF(LQ(JTM2).NE.0)JTP2=LQ(JTM2)
                  IF(JTP.NE.JTP2)THEN
                     IF(JTP2.NE.JTMED)THEN
                        KLOS=Q(JTP2+21)
                        IF (KLOS .EQ. 2) THEN
                           Q(JTP2+ 8)=9999.
                           Q(JTP2+ 9)=9999.
                           Q(JTP2+15)=0.
                        ENDIF
                        IF(Q(JTP2+ 6).GT.0.9*BIG)Q(JTP2+ 6)=Q(JTP2+1)
                        IF(Q(JTP2+ 7).GT.0.9*BIG)Q(JTP2+ 7)=Q(JTP2+1)
                        IF(Q(JTP2+ 8).GT.0.9*BIG)Q(JTP2+ 8)=Q(JTP2+2)
                        IF(Q(JTP2+ 9).GT.0.9*BIG)Q(JTP2+ 9)=Q(JTP2+2)
                        IF(Q(JTP2+10).GT.0.9*BIG)Q(JTP2+10)=0.010
                        IF(Q(JTP2+10).LT.4.*EMASS)Q(JTP2+10)=4.*EMASS
                     ENDIF
                     DO 110 I=6,10
                        IF(Q(JTP+I).NE.Q(JTP2+I))THEN
                           WRITE(CHMAIL,12600)NMAT
                           CALL GMAIL(1,0)
                           WRITE(CHMAIL,12700)ITM,ITM2
                           CALL GMAIL(0,1)
                           GO TO 120
                        ENDIF
  110                CONTINUE
                  ENDIF
               ENDIF
            ENDIF
  120    CONTINUE
         IF (DEEMAX.LT.0.) THEN
            IF(ISVOL.EQ.0)THEN
               DEEMAX=0.25
               IF(RADL.GT.2.)DEEMAX=0.25-0.2/SQRT(RADL)
            ELSE
               DEEMAX = 0.2/SQRT(RADL)
            ENDIF
         ENDIF
         IF(OLDGVE.LT.3.15.OR.STEMAX.LE.0.) THEN
*
*       Before version 3.15 there was no STEMAX, so we put it to BIG
            STEMAX=BIG
         ENDIF
         Q(JTM+11) = STEMAX
         Q(JTM+12) = DEEMAX
C
*
*       It can happen that several tracking media refer to the
*       same material. In this case we do not fill the cross section
*       tables more than once. But we still fill the banks of the
*       tracking medium.
         IF(LQ(JMA-1).NE.0) GOTO 160
         NPUSH=20-IQ(JMA-2)
         IF(NPUSH.GT.0)THEN
            CALL MZPUSH(IXCONS,JMA,NPUSH,0,'I')
            JTM=LQ(JTMED-ITM)
            JMA=LQ(JMATE-NMAT)
         ENDIF
*
*     Energy loss and cross-section tables
         IF(ISTRA.EQ.0) THEN
         CALL MZBOOK(IXCONS,LBANK,JMA, -1,'MAEL',0,0,2*NEK1,3,0)
         CALL MZBOOK(IXCONS,LBANK,JMA, -2,'MAMU',0,0, NEK1,3,0)
         ELSE
         CALL MZBOOK(IXCONS,LBANK,JMA, -1,'MAEL',0,0,3*NEK1,3,0)
         CALL MZBOOK(IXCONS,LBANK,JMA, -2,'MAMU',0,0,2*NEK1,3,0)
         ENDIF
         CALL MZBOOK(IXCONS,LBANK,JMA, -3,'MAAL',0,0, NEK1,3,0)
         CALL MZBOOK(IXCONS,JPROB,JMA, -4,'MAPR',0,0, 40,3,0)
         CALL MZBOOK(IXCONS,JPHOT,JMA, -6,'MAPH',2,2, NEK1,3,0)
         CALL MZBOOK(IXCONS,JANNI,JMA, -7,'MAAN',0,0, NEK1,3,0)
         CALL MZBOOK(IXCONS,JCOMP,JMA, -8,'MACO',0,0, NEK1,3,0)
         CALL MZBOOK(IXCONS,JBREM,JMA, -9,'MABR',0,0,3*NEK1,3,0)
         CALL MZBOOK(IXCONS,JPAIR,JMA,-10,'MAPA',0,0,2*NEK1,3,0)
         CALL MZBOOK(IXCONS,JDRAY,JMA,-11,'MADR',0,0,3*NEK1,3,0)
*
* *** Special case for heavy materials, photo-fission
         IF(A.GE.230..AND.A.LE.240..AND.IPFIS.NE.0)THEN
            CALL MZBOOK(IXCONS,JPFIS,JMA,-12,'MAPF',0,0,2*NEK1,3,0)
         ENDIF
*
* *** Rayleigh effect
         CALL MZBOOK(IXCONS,JRAYL,JMA,-13,'MARA',0,0,2*NEK1,3,0)
*
* *** Muon nuclear interactions
         IF(IMUNU.EQ.0)THEN
            JMUNU=0
         ELSE
            CALL MZBOOK(IXCONS,JMUNU,JMA,-14,'MAMN',0,0,NEK1,3,0)
         ENDIF
*
* *** stopping range
         CALL MZBOOK(IXCONS,LBANK,JMA,-15,'MASE',0,0,2*NEK1,3,0)
         CALL MZBOOK(IXCONS,LBANK,JMA,-16,'MASM',0,0,2*NEK1,3,0)
*
* *** Special for photeffect
         CALL GPHXSI
*
* *** coefficients for energy loss
         CALL MZBOOK(IXCONS,LBANK,JMA,-17,'MACE',0,0,6*NEK1,3,0)
         CALL MZBOOK(IXCONS,LBANK,JMA,-18,'MACM',0,0,6*NEK1,3,0)
*
* *** auxiliary tables for integration of dE/dx
         CALL GWORK(NEKBIN*4)
*
         DO 130 JWORK=1, NEKBIN*4
            WS(JWORK) = 0.
  130    CONTINUE
*
* *** Straggling for thin layers, if in effect
         IF(ISTRA.GT.0) THEN
            CALL MZBOOK(IXCONS,JTSTRA,JMA,-19,'MAST',2,2,1,3,0)
#if defined(CERNLIB_ASHO)
            IF(ISTRA.EQ.2) THEN
               CALL MZBOOK(IXCONS,JTASHO,JMA,-20,'MASH',0,0,106,3,0)
            ENDIF
#endif
         ENDIF
*
         DO 140 J=1,20
            JB=LQ(JMA-J)
            IF(JB.NE.0)IQ(JB-5)=NMAT
  140    CONTINUE
C
         JPROB=LQ(JMA-4)
         JMIXT=LQ(JMA-5)
         JPFIS=LQ(JMA-12)
*
* *** Fill above tables (energy losses,cross-sections,stopping ranges)
*
         CALL GPROBI
C
         DO 150 IEKBIN=1,NEK1
C
            CALL GDRELA
            CALL GBRELA
            CALL GPRELA
C
            CALL GPHOTI
            CALL GRAYLI
            CALL GANNII
            CALL GCOMPI
            CALL GBRSGA
            CALL GPRSGA
            CALL GDRSGA
            CALL GMUNUI
            CALL GPFISI
  150    CONTINUE
*
*           Stopping ranges
*
         CALL GRANGI
*
*           Energy loss coefficients
*
         CALL GCOEFF
* *** The table for the energy loss in thin gas layers if the tracking
*     media is defined as such
*
         IF(ISTRA.GT.0) THEN
            CALL GSTINI
#if defined(CERNLIB_ASHO)
            IF (ISTRA.EQ.2) THEN
               CALL GIASHO
            ENDIF
#endif
         ENDIF
*
* *** Multiple scattering,energy-loss and mag.field steps
  160    DO 170 J=1,2
            IF(LQ(JTM-J).NE.0) THEN
               CALL MZDROP(IXCONS,LQ(JTM-J),'L')
            ENDIF
  170    CONTINUE
         CALL MZBOOK(IXCONS,LBANK,JTM, -1,'MUEL',0,0,NEK1+2,3,0)
         IQ(LBANK-5)=ITM
         CALL MZBOOK(IXCONS,LBANK,JTM, -2,'MUMU',0,0,NEK1+2,3,0)
         IQ(LBANK-5)=ITM
         CALL GMULOF
C
  180 CONTINUE
*
      WRITE(CHMAIL,10100)
      CALL GMAIL(0,0)
      WRITE(CHMAIL,10400)
      CALL GMAIL(0,2)
C
10000 FORMAT(
     +'1************************************************************')
10100 FORMAT(
     +' *                                                          *')
10200 FORMAT(
     +' *    G E A N T  Version',F7.4,'      DATE/TIME',I7.6,'/',
     +   I4.4,2X,'*')
10300 FORMAT(
     +' *                      R U N  ',I5,10X,'              *')
10400 FORMAT(
     +' ************************************************************')
10500 FORMAT(
     +' *      Data structure   Date   Time    GVERSN    ZVERSN    *')
10600 FORMAT(
     +' *      --------------   ----   ----    ------    ------    *')
10700 FORMAT(' *',11X,A,6X,I7.6,2X,I4.4,3X,F7.4,2X,F7.2,5X,'*')
10800 FORMAT(
     +' *----------------------------------------------------------*')
10900 FORMAT(' *     Random number seeds: ',3X,I10,3X,I10,6X,'*')
11000 FORMAT(
     +' *     --------------------                                 *')
11100 FORMAT(
     +' *              Standard TPAR for this run are              *')
11200 FORMAT(
     +' *              ------------------------------              *')
11300 FORMAT(
     +' *  CUTGAM=',F6.2,A4,'  CUTELE=',F6.2,A4,'  CUTNEU=',F6.2,A4,1X,
     + '*')
11400 FORMAT(
     +' *  CUTHAD=',F6.2,A4,'  CUTMUO=',F6.2,A4,20X,'*')
11500 FORMAT(
     +' *  BCUTE =',F6.2,A4,'  BCUTM =',F6.2,A4,20X,'*')
11600 FORMAT(
     +' *  DCUTE =',F6.2,A4,'  DCUTM =',F6.2,A4,'  PPCUTM=',F6.2,A4,1X,
     + '*')
11700 FORMAT(
     +' *  IPAIR =',F10.0,'  ICOMP =',F10.0,'  IPHOT =',F10.0,1X,'*')
11800 FORMAT(
     +' *  IPFIS =',F10.0,'  IDRAY =',F10.0,'  IANNI =',F10.0,1X,'*')
11900 FORMAT(
     +' *  IBREM =',F10.0,'  IHADR =',F10.0,'  IMUNU =',F10.0,1X,'*')
12000 FORMAT(
     +' *  IDCAY =',F10.0,'  ILOSS =',F10.0,'  IMULS =',F10.0,1X,'*')
12100 FORMAT(
     +' *  IRAYL =',F10.0,'  ILABS =',F10.0,'  ISYNC =',F10.0,1X,'*')
12110 FORMAT(
     +' *  ISTRA =',F10.0, 39X,                                  '*')
 
12200 FORMAT(' ***** GPHYSI error, Material Nr=',I3,
     + ' referenced by Medium Nr=',I3)
12300 FORMAT(
     +' *     Special TPAR for TMED',I4,3X,A,5X,'*')
12400 FORMAT(
     +' *     -------------------------                            *')
12500 FORMAT(' ***** GPHYSI error, CUTS must be',
     + ' greater than EKMIN KeV *****')
12600 FORMAT(' ***** GPHYSI error for material nr ',I4)
12700 FORMAT(7X,'Tracking medium NR',I4,' and',I4,
     +' have different parameters')
12800 FORMAT(
     +' *  IHADR=3 not supported any more. GHEISHA will handle     *')
12900 FORMAT(
     +' *  hadronic interactions for the above tracking medium     *')
  999 END
