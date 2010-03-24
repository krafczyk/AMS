*  $Id: rkms.h,v 1.9 2010/03/24 10:19:03 choutko Exp $
      integer npma
      parameter(NPma=9) 
      integer npoc
      double precision xc,yc,zc,wxy,Dhi2
      common/rkms_1/xc(NPma),yc(NPma),zc(NPma),wxy(NPma,NPma),Dhi2,npoc
      common/rkms_init/trkz0
      double precision rkstep
      common /rkms_2/xt,yt,wt
      double precision xt(NPma),yt(NPma),wt(3,npma)
      common/rkst/     rkstep
!$OMP threadprivate(/rkms_1/,/rkms_2/,/rkst/)
      real SiThick,zz0
      data SiThick/0.0300/    ! plane thicknes in cm. (300mkm Si)
      data     zz0/0.0032054/ ! plane thicknes in R.L.(300mkm Si)

      real trkZ0(NPma)
*=== AMS01 Nov.29,1996
c01      data trkZ0/50.95,29.15,7.75,-7.75,-29.15,-50.95/
*=== AMS02 (Aug.25,04)
c      data trkZ0/54.7,29.2,25.8,1.7,-1.7,-25.8,-29.2,-54.7/
*   taken from MC (Sep.04)
c      data trkZ0/54.51,29.18,25.32,1.68,-2.18,-25.32,-29.18,-54.51/
c      data trkZ0/52.985,29.185,25.215,1.685,-2.285,
c     + -25.215,-29.185,-52.985,-122./

      save SiThick,zz0
*-------------------------------------------------------
*   pl#   1-2   2-3   3-4   4-5   5-6   6-7  7-8
*   dZ   21.8  24.4  15.5  24.4  21.8   -    -    AMS01
*   dZ   25.5   3.4  24.1   3.4  24.1  3.4  25.5  AMS02
*-------------------------------------------------------
