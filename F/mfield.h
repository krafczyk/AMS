      PARAMETER (nx=33,ny=33,nz=227)
      DIMENSION X(nx),y(ny),z(nz)
      DIMENSION Bx(nx,ny,nz), by(nx,ny,nz),
     +          BZ(nx,ny,nz) 
      dimension xyz(nx+ny+nz)    
      DIMENSION BDx(nx,ny,nz,2), bDy(nx,ny,nz,2),
     +          BdZ(nx,ny,nz,2)
      integer na(3)
      integer isec(2),imin(2),ihour(2),iday(2),imon(2),iyear(2)
      common /tkfield/iniok,isec,imin,ihour,iday,imon,iyear,
     +                na,x,y,z,bx,by,bz,xyz,bdx,bdy,bdz
      common /amsdatadir/amsdlength,amsdatadir
      integer amsdlength
      character *64 amsdatadir
