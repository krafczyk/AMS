*  $Id: rkms.h,v 1.1 2003/10/29 15:28:53 choutko Exp $
      integer npma
      parameter(NPma=8) 
      integer npoc
      double precision xc,yc,zc,wxy,Dhi2
      common/rkms_1/xc(NPma),yc(NPma),zc(NPma),wxy(NPma,NPma),Dhi2,npoc

                   
      real SiThick,zz0
c      data SiThick/0.0100/    ! plane thicknes in cm. (100mkm Si)
c      data     zz0/0.0010685/ ! plane thicknes in R.L.(100mkm Si)

c      data SiThick/0.0300/    ! plane thicknes in cm. (300mkm Si)
c      data     zz0/0.0032054/ ! plane thicknes in R.L.(300mkm Si)

      data SiThick/0.0600/    ! plane thicknes in cm. (600mkm Si)
      data     zz0/0.0064102/ ! plane thicknes in R.L.(600mkm Si)

c      data SiThick/0.0900/    ! plane thicknes in cm. (900mkm Si)
c      data     zz0/0.0192306/ ! plane thicknes in R.L.(900mkm Si)

      real trkZ0(NPma)
      data trkZ0/53.0,29.2,27.5,1.7,-1.7,-27.5,-29.2,-53.0/ ! AMS02

      save SiThick,zz0,trkZ0
*-------------------------------------------------------
*   pl   1-2   2-3   3-4   4-5   5-6   6-7  7-8
*   dZ   21.8  24.4  15.5, 24.4, 21.8              AMS01
*   dZ   23.8   1.7  25.8   3.4  25.8  1.7  23.8   AMS02
