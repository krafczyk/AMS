      subroutine readmfield
#include "mfield.h"
        real hxy(3,3),x1(3)
       if(iniok.eq.0)then
*       
        write(*,*)'Opening field file...'
        OPEN(21,
     +  FILE=amsdatadir(1:amsdlength)//'fld97int.txt',
     +  STATUS='OLD',form='formatted')
         do k=1,nz
          do j=1,ny
           do i=1,nx
            read(21,*)x(i),y(j),z(k),bx(i,j,k),by(i,j,k),bz(i,j,k)
            x(i)=x(i)/10.
            y(j)=y(j)/10.
            z(k)=z(k)/10.
            bx(i,j,k)=bx(i,j,k)/1000.
            by(i,j,k)=by(i,j,k)/1000.
            bz(i,j,k)=bz(i,j,k)/1000.
           enddo
          enddo
         enddo
         na(1)=nx
         na(2)=ny
         na(3)=nz
         call ucopy(x(1),xyz(1),nx)
         call ucopy(y(1),xyz(1+nx),ny)
         call ucopy(z(1),xyz(1+nx+ny),nz)
         read(21,*)isec
         read(21,*)imin
         read(21,*)ihour
         read(21,*)iday
         read(21,*)imon
         read(21,*)iyear
         iniok=1
         close(21)
         write(*,*)'Closing field file...'
*
* Now calculate field derivatives
*

           call timex(st0)
           write(*,*)'Calculating field derivatives...'
           do i=1,nx
            do j=1,ny
              do k=1,nz
               x1(1)=x(i)
               x1(2)=y(j)
               x1(3)=z(k)
               call tkfldc(x1,hxy)
               bdx(i,j,k,1)=hxy(1,1)
               bdy(i,j,k,1)=hxy(2,1)
               bdz(i,j,k,1)=hxy(3,1)
               bdx(i,j,k,2)=hxy(1,2)
               bdy(i,j,k,2)=hxy(2,2)
               bdz(i,j,k,2)=hxy(3,2)
              enddo
            enddo
           enddo 
           call timex(st1)
           write(*,*)'Finished in ',st1-st0, ' sec'


       endif
      end
      SUBROUTINE GUFLD(VV,B)
#include "mfield.h"
C---------------------------------------------------------------------
      DIMENSION B(3),VV(3)
        if(abs(vv(1)).gt.x(nx).or.abs(vv(2)).gt.y(ny).or.
     +    abs(vv(3)).gt.z(nz))then
            call vzero(b(1),3)
            goto 999
        ENDIF
C
        b(1) = FINT(3,vv(1),NA(1),xyz(1),Bx(1,1,1))
        b(2) = FINT(3,vv(1),NA(1),xyz(1),By(1,1,1))
        b(3) = FINT(3,vv(1),NA(1),xyz(1),Bz(1,1,1))
        
 999    END
      subroutine tkfld(xx,hxy)
#include "mfield.h"
          real xx(3),hxy(3,3)
         if(abs(xx(1)).gt.x(nx).or.abs(xx(2)).gt.y(ny).or.
     +    abs(xx(3)).gt.z(nz))then
           call vzero (hxy(1,1),9)
           goto 999
          endif
          hxy(1,1) = FINT(3,Xx(1),NA(1),xyz(1),Bdx(1,1,1,1))
          hxy(2,1) = FINT(3,Xx(1),NA(1),xyz(1),Bdy(1,1,1,1))
          hxy(3,1) = FINT(3,Xx(1),NA(1),xyz(1),Bdz(1,1,1,1))
          hxy(1,2) = FINT(3,Xx(1),NA(1),xyz(1),Bdx(1,1,1,2))
          hxy(2,2) = FINT(3,Xx(1),NA(1),xyz(1),Bdy(1,1,1,2))
          hxy(3,2) = FINT(3,Xx(1),NA(1),xyz(1),Bdz(1,1,1,2))
999       end
