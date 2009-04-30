*  $Id: mfield.h,v 1.12 2009/04/30 14:38:51 choutko Exp $
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
      integer na(3),mfile(40)
      integer isec(2),imin(2),ihour(2),iday(2),imon(2),iyear(2)

      common /tkfield/mfile,iniok,isec,imin,ihour,iday,imon,iyear,
     +                na,x,y,z,bx,by,bz,xyz,bdx,bdy,bdz,bxc,byc,bzc
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
