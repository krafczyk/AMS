*  $Id: mfield.h,v 1.13 2009/05/06 14:34:10 choutko Exp $
      integer nx,ny,nz
       PARAMETER (nx=41,ny=41,nz=41)
      real X(nx),y(ny),z(nz)
      real Bx(nx,ny,nz), by(nx,ny,nz),
     +          BZ(nx,ny,nz)
      real Bxc(nx,ny,nz), byc(nx,ny,nz),
     +          BZc(nx,ny,nz)
      real xyz(nx+ny+nz)    
      real BDx(nx,ny,nz,2), bDy(nx,ny,nz,2),
     +          BdZ(nx,ny,nz,2)



      integer nrad,nphi,nzr
      PARAMETER (nrad=8,nphi=73,nzr=23)
      real rad(nrad),phi(nphi),zr(nzr)
      real Bxr(nrad,nphi,nzr), byr(nrad,nphi,nzr),
     +          BZr(nrad,nphi,nzr)
      real Bxrc(nrad,nphi,nzr), byrc(nrad,nphi,nzr),
     +          BZrc(nrad,nphi,nzr)
      real xyzr(nrad+nphi+nzr)    
      real BDxr(nrad,nphi,nzr,2), bDyr(nrad,nphi,nzr,2),
     +          BdZr(nrad,nphi,nzr,2)



      integer na(3),mfile(40)
      integer isec(2),imin(2),ihour(2),iday(2),imon(2),iyear(2)

      common /tkfield/mfile,iniok,isec,imin,ihour,iday,imon,iyear,
     +             na,x,y,z,bx,by,bz,xyz,bdx,bdy,bdz,bxc,byc,bzc
      equivalence (x(1),rad(1))
      equivalence (x(9),phi(1))
      equivalence (z(1),zr(1))
      equivalence (xyz(1),xyzr(1))
      equivalence (bx(1,1,1),bxr(1,1,1))
      equivalence (by(1,1,1),byr(1,1,1))
      equivalence (bz(1,1,1),bzr(1,1,1))
      equivalence (bxc(1,1,1),bxrc(1,1,1))
      equivalence (byc(1,1,1),byrc(1,1,1))
      equivalence (bzc(1,1,1),bzrc(1,1,1))
      equivalence (bdx(1,1,1,1),bdxr(1,1,1,1))
      equivalence (bdy(1,1,1,1),bdyr(1,1,1,1))
      equivalence (bdz(1,1,1,1),bdzr(1,1,1,1))
*
*       iniok=0  initialization
*       iniok=-1 permanent magnet initialization
*       iniok=-2 special procedure to initialize  b...c values 
*       iniok=2  special simulation procedure to generate with b...c
*         
         !$OMP threadprivate(/my_hint/)
      common /amsdatadir/amsdlength,amsdblength,amsdatadir,
     +                   amsdatabase,fname
      integer amsdlength,amsdblength,iniok
      character *128 amsdatadir,amsdatabase
      character *200 fname
      common/my_hint/hint,hintc
       integer hint(3),hintc(3)
*
*  C New Common  For E01TGF
*
c      parameter(mmax=nx*ny*nz)
c      parameter (lrq=10*mmax+7)
c      parameter(liq=2*mmax+1)
c      common /CBXYZ/xe,ye,ze
c      real *8 xe(mmax),ye(mmax),ze(mmax)
c      common /CBBXBYBZ/bxe,bye,bze
c      real *8 bxe(mmax),bye(mmax),bze(mmax)
c      common /tkfieldaddon/iqx,iqy,iqz,rqxr,rqyr,rqzr
c      integer iqx(liq),iqy(liq),iqz(liq)
c      real *4 rqxr(2,lrq),rqyr(2,lrq),rqzr(2,lrq)
c      real *8 rqx(lrq),rqy(lrq),rqz(lrq)
c      equivalence(rqx(1),rqxr(1,1))      
c      equivalence(rqy(1),rqyr(1,1))      
c      equivalence(rqz(1),rqzr(1,1))      
c      real *8 u(1),v(1),w(1),qx(1),qy(1),qz(1),q(1)

       common/MAGSFFKEY/magstat,fscale,ecutge,r0(3),pitch,yaw,roll,rphi
       integer magstat,rphi
       real fscale,ecutge,r0,pitch,yaw,roll
