*  $Id: mfield.h,v 1.17 2010/03/21 15:16:38 choutko Exp $
      integer nx,ny,nz
c       PARAMETER (nx=41,ny=41,nz=41,nzo=41)
       PARAMETER (nx=41,ny=41,nz=180,nzo=41)
c
c      ams02 rectangular map disabled
c      use nz=41 to enable it
c      ams02 rphi map ok
c

c       PARAMETER (nx=90,ny=90,nz=164)
      real X(nx),y(ny),z(nz)
      real Bx(nx,ny,nz), by(nx,ny,nz),
     +          BZ(nx,ny,nz)
      real Bxc(nx,ny,nz), byc(nx,ny,nz),
     +          BZc(nx,ny,nz)
      real xyz(nx+ny+nz)    
      real BDx(nx,ny,nz,2), bDy(nx,ny,nz,2),
     +          BdZ(nx,ny,nz,2)
      real zzz(nz+nx+ny+nz+9*nx*ny*nz)
      save bxc,byc,bzc

      integer nrad,nphi,nzr
      PARAMETER (nrad=9,nphi=73,nzr=37)
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

c      common /tkfield/mfile,iniok,isec,imin,ihour,iday,imon,iyear,
c     +             na,x,y,z,bx,by,bz,xyz,bdx,bdy,bdz,bxc,byc,bzc
      common /tkfield/mfile,iniok,isec,imin,ihour,iday,imon,iyear,
     +             na,x,y,zzz
      equivalence (x(1),rad(1))
      equivalence (x(10),phi(1))
      equivalence (zzz(1),zr(1))
      equivalence (zzz(nzo+1),bxr(1,1,1))
      equivalence (zzz(nzo+1+nx*ny*nzo),byr(1,1,1))
      equivalence (zzz(nzo+1+2*nx*ny*nzo),bzr(1,1,1))
      equivalence (zzz(nzo+1+3*nx*ny*nzo),xyzr(1))
      equivalence (zzz(nzo+1+3*nx*ny*nzo+nx+ny+nzo),bdxr(1,1,1,1))
      equivalence (zzz(nzo+1+5*nx*ny*nzo+nx+ny+nzo),bdyr(1,1,1,1))
      equivalence (zzz(nzo+1+7*nx*ny*nzo+nx+ny+nzo),bdzr(1,1,1,1))
      equivalence (zzz(nzo+1+9*nx*ny*nzo+nx+ny+nzo),bxrc(1,1,1))
      equivalence (zzz(nzo+1+10*nx*ny*nzo+nx+ny+nzo),byrc(1,1,1))
      equivalence (zzz(nzo+1+11*nx*ny*nzo+nx+ny+nzo),bzrc(1,1,1))
      equivalence (zzz(nz+1),bx(1,1,1))
      equivalence (zzz(1),z(1))
      equivalence (zzz(nz+1+nx*ny*nz),by(1,1,1))
      equivalence (zzz(nz+1+2*nx*ny*nz),bz(1,1,1))
      equivalence (zzz(nz+1+3*nx*ny*nz),xyz(1))
      equivalence (zzz(nz+1+3*nx*ny*nz+nx+ny+nz),bdx(1,1,1,1))
      equivalence (zzz(nz+1+5*nx*ny*nz+nx+ny+nz),bdy(1,1,1,1))
      equivalence (zzz(nz+1+7*nx*ny*nz+nx+ny+nz),bdz(1,1,1,1))
c     equivalence (zzz(nz+1+9*nx*ny*nz+nx+ny+nz),bxc(1,1,1))
c     equivalence (zzz(nz+1+10*nx*ny*nz+nx+ny+nz),byc(1,1,1))
c     equivalence (zzz(nz+1+11*nx*ny*nz+nx+ny+nz),bzc(1,1,1))

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
      common/my_hint/hint,hintc,bcorr
       integer hint(3),hintc(3)
       real bcorr
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
