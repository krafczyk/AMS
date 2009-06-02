*  $Id: readmf.h,v 1.1 2009/06/02 14:04:50 choutko Exp $
      integer nrad,nphi,nzr
      PARAMETER (nrad=15,nphi=72,nzr=38)
      real rad(nrad),phi(nphi),zr(nzr)
      real Br(nrad,nphi,nzr), bphi(nrad,nphi,nzr),
     +          BZ(nrad,nphi,nzr),brcoo(3,nrad,nphi,nzr),
     +          bphicoo(3,nrad,nphi,nzr),bzcoo(3,nrad,nphi,nzr)
      common /my$/rad,phi,zr,br,bphi,bz,brcoo,bphicoo,bzcoo
