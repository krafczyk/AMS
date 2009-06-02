*  $Id: readmf.h,v 1.2 2009/06/02 17:32:55 choutko Exp $
      integer nrad,nphi,nzr
      PARAMETER (nrad=15,nphi=72,nzr=38)
      real rad(nrad),phi(nphi),zr(nzr)
      real Br(nrad,nphi,nzr), bphi(nrad,nphi,nzr),
     +          BZ(nrad,nphi,nzr),brcoo(3,nrad,nphi,nzr),
     +          bphicoo(3,nrad,nphi,nzr),bzcoo(3,nrad,nphi,nzr)
      common /my_common/rad,phi,zr,br,bphi,bz,brcoo,bphicoo,bzcoo
