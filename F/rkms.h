*============================================================= rkms.h
*===  AMS02 2010 (Permanent magnet & Tracker with 9 Si-planes)
*=============================================================
      integer   NPma ! maximum number of planes
      parameter(NPma=9)
*     ------------------------------------------- /rkms_1/
      integer npoc  !  number of planes with hits
      double precision xc,yc,zc,wx,wy,Dhi2
      common/rkms_1/xc(NPma),yc(NPma),zc(NPma)
     +     ,wx(NPma,NPma),wy(NPma,NPma),Dhi2,npoc
*     ------------------------------------------- /rkst/
      double precision rkstep
      common/rkst/     rkstep
!$OMP threadprivate(/rkms_1/,/rkst/)
*     --------------------------------------- position of Si-planes
      common/rkms_init/trkZ0 ! (taken from MC call rkmsinit)
      real trkZ0(NPma)
c      data trkZ0                                                   !#c
c     + /166.92,54.51,29.18,25.32,1.68,-2.18,-25.32,-29.18,-135.48/ !#c
*     ------------------------------------------------------------
