*  $Id: mfield.h,v 1.6 2001/01/22 17:32:25 choutko Exp $
      integer nx,ny,nz
      PARAMETER (nx=41,ny=41,nz=130)
      real X(nx),y(ny),z(nz)
      real Bx(nx,ny,nz), by(nx,ny,nz),
     +          BZ(nx,ny,nz)
      real xyz(nx+ny+nz)    
      real BDx(nx,ny,nz,2), bDy(nx,ny,nz,2),
     +          BdZ(nx,ny,nz,2)
      integer na(3),mfile(40)
      integer isec(2),imin(2),ihour(2),iday(2),imon(2),iyear(2)
      common /tkfield/mfile,iniok,isec,imin,ihour,iday,imon,iyear,
     +                na,x,y,z,bx,by,bz,xyz,bdx,bdy,bdz
      
      common /amsdatadir/amsdlength,amsdblength,amsdatadir,
     +                   amsdatabase,fname
      integer amsdlength,amsdblength,iniok
      character *128 amsdatadir,amsdatabase
      character *200 fname
