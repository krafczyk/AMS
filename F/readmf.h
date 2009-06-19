*  $Id: readmf.h,v 1.3 2009/06/19 14:31:08 choutko Exp $
      integer nrad,nphi,nzr
      PARAMETER (nrad=15,nphi=72,nzr=37)
      real rad(nrad),phi(nphi),zr(nzr)
      real Br(nrad,nphi,nzr), bphi(nrad,nphi,nzr),
     +          BZ(nrad,nphi,nzr),brcoo(3,nrad,nphi,nzr),
     +          bphicoo(3,nrad,nphi,nzr),bzcoo(3,nrad,nphi,nzr)
      common /my_common/rad,phi,zr,br,bphi,bz,brcoo,bphicoo,bzcoo
