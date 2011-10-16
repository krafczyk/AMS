*  $Id: mfield.F,v 1.39 2011/10/16 09:10:12 choutko Exp $

c. Add by Zhili.Weng 2009-11-11
#ifdef __AMSVMC__
#include "VMCrename.h"
#endif


      subroutine readmfield_rphi
c     should be called if rphi!=0
c
#      include "mfield.h"
       real radc(nrad),phic(nphi),zrc(nzr)
       real hxy(3,3),x1(3),b1(3)
       real *8 stval,step,bnd1,bnd2,val,error,fmin,fedm,arglis(10)
       external fcn
       external fcn2
       pi=acos(0.)*2
      if(iniok.eq.0.or.iniok.eq.-2)then
*       
          write(*,*)'Opening field rphi file...'
c          OPEN(21,FILE=amsdatadir(1:amsdlength)//'fld97int.txt',
          OPEN(21,FILE=amsdatadir(1:amsdlength)//fname,
     +     STATUS='OLD',form='formatted')
          fac=1.
          do i=1,nrad
           do j=1,nphi
            do k=1,nzr
             read(21,*)rad(i),phi(j),zr(k),
     +       bxr(i,j,k),byr(i,j,k),bzr(i,j,k)
             radc(i)=rad(i)
             phic(j)=phi(j)
             zrc(k)=zr(k)
             rad(i)=rad(i)/10.
             phi(j)=phi(j)*pi/180.
             zr(k)=zr(k)/10.
             bxr(i,j,k)=bxr(i,j,k)*10 *fac
             byr(i,j,k)=byr(i,j,k)*10. *fac
             bzr(i,j,k)=bzr(i,j,k)*10. *fac
            enddo
           enddo
          enddo
          na(1)=nrad
          na(2)=nphi
          na(3)=nzr
          call ucopy(rad(1),xyzr(1),nrad)
          call ucopy(phi(1),xyzr(1+nrad),nphi)
          call ucopy(zr(1),xyzr(1+nrad+nphi),nzr)
*         read(21,*)isec
*         read(21,*)imin
*         read(21,*)ihour
*         read(21,*)iday
*         read(21,*)imon
*         read(21,*)iyear
          close(21)
          write(*,*)'Closing field rphi file...'
*
* Now calculate field derivatives
*
          imagst=magstat
          magstat=1
          call timex(st0)
          write(*,*)'Calculating field derivatives...'
          do i=1,nrad
           do j=1,nphi
             do k=1,nzr
              x1(1)=rad(i)*cos(phi(j))
              x1(2)=rad(i)*sin(phi(j))
              x1(3)=z(k)
              call tkfldc(x1,hxy)
              bdxr(i,j,k,1)=hxy(1,1)
              bdyr(i,j,k,1)=hxy(2,1)
              bdzr(i,j,k,1)=hxy(3,1)
              bdxr(i,j,k,2)=hxy(1,2)
              bdyr(i,j,k,2)=hxy(2,2)
              bdzr(i,j,k,2)=hxy(3,2)
              brx=bxr(i,j,k)*hxy(1,1)+byr(i,j,k)*hxy(1,2)+
     +        bzr(i,j,k)*hxy(1,3)
              bry=bxr(i,j,k)*hxy(2,1)+byr(i,j,k)*hxy(2,2)+
     +        bzr(i,j,k)*hxy(2,3)
              brz=bxr(i,j,k)*hxy(3,1)+byr(i,j,k)*hxy(3,2)+
     +        bzr(i,j,k)*hxy(3,3)
              brxm=hxy(1,1)**2+hxy(1,2)**2+hxy(1,3)**2
              brym=hxy(2,1)**2+hxy(2,2)**2+hxy(2,3)**2
              brzm=hxy(3,1)**2+hxy(3,2)**2+hxy(3,3)**2
              bb=sqrt(bxr(i,j,k)**2+byr(i,j,k)**2+bzr(i,j,k)**2)
              brm=sqrt(brxm+brym+brzm)
              if(bb.gt.0)then
                br=sqrt(brx**2+bry**2+brz**2)/bb
              else
                br=0
              endif
*
*              rotor
*
               rx=hxy(2,3)-hxy(3,2)
               ry=hxy(3,1)-hxy(1,3)
               rz=hxy(1,2)-hxy(2,1)

              write(32,*)x1,bxr(i,j,k),byr(i,j,k),bzr(i,j,k),br,brm,
     +        rx,ry,rz
             enddo
           enddo
          enddo
          call timex(st1)
          write(*,*)'Finished in ',st1-st0, ' sec'
          magstat=imagst
        if(iniok.eq.-2)then
*
*       here do the initialization of b...c  values
*
*       
        if(magstat.ge.0)then

          write(*,*)'Opening design rphi field file...'
          OPEN(22,FILE=amsdatadir(1:amsdlength)//'design.'//fname,
     +     STATUS='OLD',form='formatted')
          fac=1.
          do i=1,nrad
           do j=1,nphi
            do k=1,nzr
             read(22,*)rad(i),phi(j),zr(k),bxrc(i,j,k),byrc(i,j,k),
     +       bzrc(i,j,k)
             rad(i)=rad(i)/10.
             phi(j)=phi(j)*pi/180.
             zr(k)=zr(k)/10.
             bxrc(i,j,k)=bxrc(i,j,k)*10 *fac
             byrc(i,j,k)=byrc(i,j,k)*10. *fac
             bzrc(i,j,k)=bzrc(i,j,k)*10. *fac
            enddo
           enddo
          enddo
          close(22)
          write(*,*)'Closing design rph field file...'

/*
          write(*,*)'Opening design xyz field file...'
          OPEN(22,FILE=amsdatadir(1:amsdlength)//'design.'//fname,
     +     STATUS='OLD',form='formatted')
          fac=0.498e-3
          do i=1,nx
           do j=1,ny
            do k=1,nz
             read(22,*)x(i),y(j),z(k),bxc(i,j,k),byc(i,j,k),bzc(i,j,k)
             x(i)=x(i)/10.
             y(j)=y(j)/10.
             z(k)=z(k)/10.
             bxc(i,j,k)=bxc(i,j,k)*10 *fac
             byc(i,j,k)=byc(i,j,k)*10. *fac
             bzc(i,j,k)=bzc(i,j,k)*10. *fac
            enddo
           enddo
          enddo
          na(1)=nx
          na(2)=ny
          na(3)=nz
          call ucopy(x(1),xyz(1),nx)
          call ucopy(y(1),xyz(1+nx),ny)
          call ucopy(z(1),xyz(1+nx+ny),nz)
          close(22)
          write(*,*)'Closing design xyz field file...'
*
*  open and write corresponding r-phi file
*          
          x1(1)=0
          x1(2)=0
          x1(3)=0
          rphi=0
          call gufldrc(x1,b1)
          write(*,*)' b1',b1
          OPEN(22,FILE=amsdatadir(1:amsdlength)//'design_rphi.'//fname,
     +     STATUS='new',form='formatted')
          do i=1,nrad
           do j=1,nphi
            do k=1,nzr
             x1(1)=radc(i)/10.*cos(phic(j)/180.*pi)
             x1(2)=radc(i)/10.*sin(phic(j)/180.*pi)
             x1(3)=zrc(k)/10.
             rphi=0
             call gufldrc(x1,b1)
             write(22,*)radc(i),phic(j),zrc(k),
     +       b1(1)/10,b1(2)/10,b1(3)/10
            enddo
           enddo
          enddo
          close (22)
          write(*,*)'Closing design rphi field file...'
          call exit 
*/
        else 
          write(*,*)'Creating distorted rphi field file...'
          OPEN(22,FILE=amsdatadir(1:amsdlength)//'distorted.'//fname,
     +     STATUS='NEW',form='formatted')
          do i=1,nrad
           do j=1,nphi
            do k=1,nzr
 17          continue
              x1(1)=rad(i)*cos(phi(j))
              x1(2)=rad(i)*sin(phi(j))
              x1(3)=zr(k)
             sigmax=0.03
             do l=1,3
              call rannor(r1,r2)
               x1(l)=x1(l)+sigmax*r1
              enddo
              call gufldr(x1,b1)             
              x1(1)=rad(i)
              x1(2)=phi(j)
              x1(3)=zr(k)
             x1(1)=x1(1)*10.
             x1(2)=x1(2)*180/pi
             x1(3)=x1(3)*10.
             b1(1)=b1(1)/10*fscale
             b1(2)=b1(2)/10*fscale
             b1(3)=b1(3)/10*fscale
              sigmab=0.2e-3
              do l=1,3
              call rannor(r1,r2)
              b1(l)=b1(l)+sigmab*r2
              enddo
             write(22,*)x1,b1
            enddo
           enddo
          enddo
           
           close(22)
        endif

         if(magstat.eq.0)then
*
*        Do the fitting normal field->design field as B (R)= A*B'*R'
*          R'=R0+R*T
*
*
           
         call mninit(5,6,6)
         call mnseti('Fitting mf')
         stval=1
 10      continue
         call mnparm(1,'scale',stval,0.05d0,0.2d0,1.1d0,ierflg)
         call mnparm(2,'x',0.d0,0.01d0,-1d0,1d0,ierflg)
         call mnparm(3,'y',0.d0,0.01d0,-1d0,1.d0,ierflg)
         call mnparm(4,'z',0.d0,0.01d0,-1d0,1d0,ierflg)
         call mnparm(5,'pitch',0.d0,0.02d0,-2.d0,2.d0,ierflg)
         call mnparm(6,'yaw',0.d0,0.02d0,-2.d0,2.d0,ierflg)
         call mnparm(7,'roll',0.d0,0.02d0,-2.d0,2.d0,ierflg)
         ierflg=0
         arglis(1)=0
         call mnexcm(fcn2,'MINIMIZE 100000',arglis,0,ierflg,0)
         ierflg=0
c         call mnexcm(fcn2,'MINOS',arglis,0,ierflg,0)
         endif 
         if(stval.eq.1)then
            stval=fscale
            goto 10
         else
          ierflg=0
          arglis(1)=3
         call mnexcm(fcn2,'CALL FCN',arglis,1,ierflg,0)

         endif
          iniok=2
      else
          iniok=1
      endif      
      endif      
      end



      subroutine readmfield
#      include "mfield.h"
       real hxy(3,3),x1(3),b1(3)
       real *8 stval,step,bnd1,bnd2,val,error,fmin,fedm,arglis(10)
       external fcn
       external fcn2
       if(rphi.ne.0)then
         call readmfield_rphi()
         return
       endif
      if(iniok.eq.0.or.iniok.eq.-2)then
*       
          write(*,*)'Opening field file...'
c          OPEN(21,FILE=amsdatadir(1:amsdlength)//'fld97int.txt',
          OPEN(21,FILE=amsdatadir(1:amsdlength)//fname,
     +     STATUS='OLD',form='formatted')
          fac=1.
*
*        !!!   new fac
           if(index(fname,'.table').gt.0)then
           fac=1.e-3
           endif
*
          do i=1,nx
           do j=1,ny
            do k=1,nz
             read(21,*)x(i),y(j),z(k),bx(i,j,k),by(i,j,k),bz(i,j,k)
             x(i)=x(i)/10.
             y(j)=y(j)/10.
             z(k)=z(k)/10.
             bx(i,j,k)=bx(i,j,k)*10 *fac
             by(i,j,k)=by(i,j,k)*10. *fac
             bz(i,j,k)=bz(i,j,k)*10. *fac
            enddo
           enddo
          enddo
          na(1)=nx
          na(2)=ny
          na(3)=nz
          call ucopy(x(1),xyz(1),nx)
          call ucopy(y(1),xyz(1+nx),ny)
          call ucopy(z(1),xyz(1+nx+ny),nz)
*         read(21,*)isec
*         read(21,*)imin
*         read(21,*)ihour
*         read(21,*)iday
*         read(21,*)imon
*         read(21,*)iyear
          close(21)
          write(*,*)' field qat 0',x(1),y(1),z(1),
     +    bx(1,1,1),by(1,1,1),bz(1,1,1)
          write(*,*)'Closing field file...'
*
* Now calculate field derivatives
*
          imagst=magstat
          magstat=1
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
              brx=bx(i,j,k)*hxy(1,1)+by(i,j,k)*hxy(1,2)+
     +        bz(i,j,k)*hxy(1,3)
              bry=bx(i,j,k)*hxy(2,1)+by(i,j,k)*hxy(2,2)+
     +        bz(i,j,k)*hxy(2,3)
              brz=bx(i,j,k)*hxy(3,1)+by(i,j,k)*hxy(3,2)+
     +        bz(i,j,k)*hxy(3,3)
              brxm=hxy(1,1)**2+hxy(1,2)**2+hxy(1,3)**2
              brym=hxy(2,1)**2+hxy(2,2)**2+hxy(2,3)**2
              brzm=hxy(3,1)**2+hxy(3,2)**2+hxy(3,3)**2
              bb=sqrt(bx(i,j,k)**2+by(i,j,k)**2+bz(i,j,k)**2)
              brm=sqrt(brxm+brym+brzm)
              if(bb.gt.0)then
                br=sqrt(brx**2+bry**2+brz**2)/bb
              else
                br=0
              endif
*
*              rotor
*
               rx=hxy(2,3)-hxy(3,2)
               ry=hxy(3,1)-hxy(1,3)
               rz=hxy(1,2)-hxy(2,1)

              write(31,*)x1,bx(i,j,k),by(i,j,k),bz(i,j,k),br,brm,
     +        rx,ry,rz
             enddo
           enddo
          enddo
          call timex(st1)
          write(*,*)'Finished in ',st1-st0, ' sec'
          magstat=imagst
        if(iniok.eq.-2)then
*
*       here do the initialization of b...c  values
*
*       
        if(magstat.ge.0)then
          write(*,*)'Opening design field file...'
          OPEN(22,FILE=amsdatadir(1:amsdlength)//'design.'//fname,
     +     STATUS='OLD',form='formatted')
          fac=1.
*
*        !!!   new fac
           if(index(fname,'.table').gt.0)then
           fac=1.e-3
            endif
*
          do i=1,nx
           do j=1,ny
            do k=1,nz
             read(22,*)x(i),y(j),z(k),bxc(i,j,k),byc(i,j,k),bzc(i,j,k)
             x(i)=x(i)/10.
             y(j)=y(j)/10.
             z(k)=z(k)/10.
             bxc(i,j,k)=bxc(i,j,k)*10 *fac
             byc(i,j,k)=byc(i,j,k)*10. *fac
             bzc(i,j,k)=bzc(i,j,k)*10. *fac
            enddo
           enddo
          enddo
          close(22)
          write(*,*)'Closing design field file...'
        else 
          write(*,*)'Creating distorted field file...'
          OPEN(22,FILE=amsdatadir(1:amsdlength)//'distorted.'//fname,
     +     STATUS='NEW',form='formatted')
          do i=1,nx
           do j=1,ny
            do k=1,nz
              x1(1)=x(i)
              x1(2)=y(j)
              x1(3)=z(k)
             sigmax=0.03
              do l=1,3
              call rannor(r1,r2)
               x1(l)=x1(l)+sigmax*r1
              enddo
              call gufldr(x1,b1)             
              x1(1)=x(i)
              x1(2)=y(j)
              x1(3)=z(k)
             x1(1)=x1(1)*10.
             x1(2)=x1(2)*10.
             x1(3)=x1(3)*10.
             b1(1)=b1(1)/10*fscale
             b1(2)=b1(2)/10*fscale
             b1(3)=b1(3)/10*fscale
              sigmab=0.2e-3
              do l=1,3
              call rannor(r1,r2)
              b1(l)=b1(l)+sigmab*r2
              enddo
             write(22,*)x1,b1
            enddo
           enddo
          enddo
           
           close(22)
        endif

         if(magstat.eq.0)then
*
*        Do the fitting normal field->design field as B (R)= A*B'*R'
*          R'=R0+R*T
*
*
           
         call mninit(5,6,6)
         call mnseti('Fitting mf')
         stval=1.
 10      continue
         call mnparm(1,'scale',stval,0.05d0,0.05d0,1.1d0,ierflg)
         call mnparm(2,'x',0.d0,0.01d0,-1d0,1d0,ierflg)
         call mnparm(3,'y',0.d0,0.01d0,-1d0,1.d0,ierflg)
         call mnparm(4,'z',0.d0,0.01d0,-1d0,1d0,ierflg)
         call mnparm(5,'pitch',0.d0,0.02d0,-2.d0,2.d0,ierflg)
         call mnparm(6,'yaw',0.d0,0.02d0,-2.d0,2.d0,ierflg)
         call mnparm(7,'roll',0.d0,0.02d0,-2.d0,2.d0,ierflg)
         call mnparm(8,'rscale',1.d0,0.02d0,.9d0,1.1d0,ierflg)

         ierflg=0
         arglis(1)=0
         call mnexcm(fcn2,'MINIMIZE 100000',arglis,0,ierflg,0)
         ierflg=0
c         call mnexcm(fcn2,'MINOS',arglis,0,ierflg,0)
         endif 
         if(stval.eq.1)then
            stval=fscale
            goto 10
         else
         ierflg=0
         arglis(1)=3
         call mnexcm(fcn2,'CALL',arglis,1,ierflg,0)
         endif
          iniok=2
      else
          iniok=1
      endif      
      else if(iniok.eq.-1)then

*
*  special procedure for 2L  Permanent Magnet
*
*       
          xlfac=2
          write(*,*)'Special ...  Opening field file...'
c          OPEN(21,FILE=amsdatadir(1:amsdlength)//'fld97int.txt',
          OPEN(21,FILE=amsdatadir(1:amsdlength)//fname,
     +     STATUS='OLD',form='formatted')
          fac=1.1
          do k=1,nz
           do i=1,nx
            do j=1,ny
             read(21,*)x(i),y(j),z(k),bx(i,j,k),by(i,j,k),bz(i,j,k)
             x(i)=x(i)
             y(j)=y(j)
             z(k)=z(k)*xlfac
             bx(i,j,k)=bx(i,j,k)/1000. *fac
             by(i,j,k)=by(i,j,k)/1000. *fac
             bz(i,j,k)=bz(i,j,k)/1000. *fac
            enddo
           enddo
          enddo
          na(1)=nx
          na(2)=ny
          na(3)=nz
          call ucopy(x(1),xyz(1),nx)
          call ucopy(y(1),xyz(1+nx),ny)
          call ucopy(z(1),xyz(1+nx+ny),nz)
*         read(21,*)isec
*         read(21,*)imin
*         read(21,*)ihour
*         read(21,*)iday
*         read(21,*)imon
*         read(21,*)iyear
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
              brx=bx(i,j,k)*hxy(1,1)+by(i,j,k)*hxy(1,2)+
     +        bz(i,j,k)*hxy(1,3)
              bry=bx(i,j,k)*hxy(2,1)+by(i,j,k)*hxy(2,2)+
     +        bz(i,j,k)*hxy(2,3)
              brz=bx(i,j,k)*hxy(3,1)+by(i,j,k)*hxy(3,2)+
     +        bz(i,j,k)*hxy(3,3)
              brxm=hxy(1,1)**2+hxy(1,2)**2+hxy(1,3)**2
              brym=hxy(2,1)**2+hxy(2,2)**2+hxy(2,3)**2
              brzm=hxy(3,1)**2+hxy(3,2)**2+hxy(3,3)**2
              bb=sqrt(bx(i,j,k)**2+by(i,j,k)**2+bz(i,j,k)**2)
              brm=sqrt(brxm+brym+brzm)
              if(bb.gt.0)then
                br=sqrt(brx**2+bry**2+brz**2)/bb
              else
                br=0
              endif
*              write(31,*)x1,bx(i,j,k),by(i,j,k),bz(i,j,k),br,brm
             enddo
           enddo
          enddo
          call timex(st1)
          write(*,*)'Finished in ',st1-st0, ' sec'
c          call rmf2
      endif
      end

c      subroutine rmf2
c#      include "mfield.h"
c*
c* Initialization for E01
c*
c          write(*,*)'x ',x
c          write(*,*)'y ',y
c          write(*,*)'z ',z
c          write(*,*)'Calculating E01GTF parameters'
c        call timex(st0)
c      do k=1,nz
c       do j=1,ny
c        do i=1,nx
c         l=i+(j-1)*nx+(k-1)*nx*ny
c         xe(l)=x(i)
c         ye(l)=y(j)
c         ze(l)=z(k)
c         bxe(l)=bx(i,j,k)         
c         bye(l)=by(i,j,k)         
c         bze(l)=bz(i,j,k)         
c        enddo
c       enddo
c      enddo
c      nq=0
c      nw=0
c      ifail=0
c      write(*,*)'x started'
c      call e01tgf(mmax,xe,ye,ze,bxe,nw,nq,iqx,liq,rqx,lrq,ifail)
c      write(*,*)'x finished'
c      do i=1,mmax
c        tmp1=rqx(i)
c        tmp2=rqx(i)-tmp1
c        rqxr(1,i)=tmp1
c        rqxr(2,i)=tmp2
c      enddo
c      nq=0
c      nw=0
c      ifail=0
c      call e01tgf(mmax,xe,ye,ze,bye,nw,nq,iqy,liq,rqy,lrq,ifail)
c      write(*,*)'y finished'
c      do i=1,mmax
c        tmp1=rqy(i)
c        tmp2=rqy(i)-tmp1
c        rqyr(1,i)=tmp1
c        rqyr(2,i)=tmp2
c      enddo
c      nq=0
c      nw=0
c      ifail=0
c      call e01tgf(mmax,xe,ye,ze,bze,nw,nq,iqz,liq,rqz,lrq,ifail)
c      do i=1,mmax
c        tmp1=rqz(i)
c        tmp2=rqz(i)-tmp1
c        rqzr(1,i)=tmp1
c        rqzr(2,i)=tmp2
c      enddo
c          call timex(st1)
c          write(*,*)'Finished in ',st1-st0, ' sec'
c      aa=0.1
c      call hbook1(-701,'dr',100,-aa,aa,0.)
c      call hbook2(-702,'drxy',50,-aa*100,aa*100,50,-aa*100,
c     +aa*100,0.)
c      call hbook1(-703,'dr',100,-aa,aa,0.)
c      call hbook2(-704,'drxy',50,-aa*100,aa*100,50,-aa*100,
c     +aa*100,0.)
c      call hbook1(-705,'dr',100,-aa,aa,0.)
c      call hbook2(-706,'drxy',50,-aa*100,aa*100,50,-aa*100,
c     +aa*100,0.)
c      call hbook1(-801,'dr',100,-aa,aa,0.)
c      call hbook2(-802,'drxy',50,-aa*100,aa*100,50,-aa*100,
c     +aa*100,0.)
c      call hbook1(-803,'dr',100,-aa,aa,0.)
c      call hbook2(-804,'drxy',50,-aa*100,aa*100,50,-aa*100,
c     +aa*100,0.)
c      call hbook1(-805,'dr',100,-aa,aa,0.)
c      call hbook2(-806,'drxy',50,-aa*100,aa*100,50,-aa*100,
c     +aa*100,0.)
c      call hbook1(-901,'dr',100,-aa,aa,0.)
c      call hbook2(-902,'drxy',50,-aa*100,aa*100,50,-aa*100,
c     +aa*100,0.)
c      call hbook1(-903,'dr',100,-aa,aa,0.)
c      call hbook2(-904,'drxy',50,-aa*100,aa*100,50,-aa*100,
c     +aa*100,0.)
c      call hbook1(-905,'dr',100,-aa,aa,0.)
c      call hbook2(-906,'drxy',50,-aa*100,aa*100,50,-aa*100,
c     +aa*100,0.)
c      end

c      subroutine rmf3
c#      include "mfield.h"
c*
c* Initialization for E01
c*
c      do k=1,nz
c       do j=1,ny
c        do i=1,nx
c         l=i+(j-1)*nx+(k-1)*nx*ny
c         xe(l)=x(i)
c         ye(l)=y(j)
c         ze(l)=z(k)
c         bxe(l)=bx(i,j,k)         
c         bye(l)=by(i,j,k)         
c         bze(l)=bz(i,j,k)         
c        enddo
c       enddo
c      enddo
c      do i=1,mmax
c        tmp1=rqxr(1,i)
c        tmp2=rqxr(2,i)
c        rqx(i)=tmp1
c        rqx(i)=rqx(i)+tmp2
c        tmp1=rqyr(1,i)
c        tmp2=rqyr(2,i)
c        rqy(i)=tmp1
c        rqy(i)=rqy(i)+tmp2
c        tmp1=rqzr(1,i)
c        tmp2=rqzr(2,i)
c        rqz(i)=tmp1
c        rqz(i)=rqz(i)+tmp2
c      enddo
c      end
      SUBROUTINE GUFLD(VV,B)
*     ======================
#      include "mfield.h"
      data jj/0/
      data init/0/ 
      data hint/0,0,0/
      data hintc/0,0,0/
      data bcorr/-1/
      data zmax /80/
      DIMENSION B(3),VV(3),AVV(3)
        if(rphi.eq.0)then
        if(abs(vv(1)).gt.x(nx).or.abs(vv(2)).gt.y(ny).or.
     +    abs(vv(3)).gt.z(nz)
     +    .or.magstat.le.-1)then
            call vzero(b(1),3)
            goto 999
        ENDIF
#ifdef __G4AMSU__
        b(1)=7
        b(2)=0
        b(3)=0
        goto 999
#endif

*
* Special trick if need
*       
        sx=sign(1.,vv(1))
        sy=sign(1.,vv(2))
        sz=sign(1.,vv(3))
        do i=1,2
          avv(i)=abs(vv(i))
        enddo
*
* Special trick if need
*
#ifdef __ROOTSHAREDLIBRARY__
           xfac=1
#else
        CALL BZCORR(xfac)
#endif
        avv(3)=abs(vv(3))*xfac

 
c        jj=jj+1
         if(iniok.eq.2)then
c           if(mod(jj,1000).eq.1)write(*,*)' calling bxc ',bxc(1,1,1),
c     +      iniok
          call fint_my(3,avv(1),NA(1),xyz(1),
     +    Bxc(1,1,1),Byc(1,1,1),Bzc(1,1,1),b(1),hintc(1))
         else
c           if(mod(jj,1000).eq.2)write(*,*)' calling bx ',bx(1,1,1),iniok
          call fint_my(3,avv(1),NA(1),xyz(1),
     +    Bx(1,1,1),By(1,1,1),Bz(1,1,1),b(1),hint(1))
         endif
#ifdef __ROOTSHAREDLIBRARY__
         factor=1
#else
         CALL BTEMPCOR (factor)
#endif
         b(1) = b(1) * factor
         b(2) = b(2)*sx*sy * factor
         b(3) = b(3)*sx*sz * factor
        else
          avv(1)=sqrt(vv(1)**2+vv(2)**2)
          avv(2)=atan2(vv(2),vv(1))
          if(avv(2).lt.phi(1))then
            avv(2)=avv(2)+4*acos(0.)
          endif
          avv(3)=vv(3)
        if(abs(avv(1)).gt.rad(nrad).or.abs(avv(2)).gt.phi(nphi).or.
     +    abs(avv(3)).gt.zr(nzr).or.magstat.le.-1)then
            call vzero(b(1),3)
            goto 999
        ENDIF
         if(iniok.eq.2)then
          call fint_my(3,avv(1),NA(1),xyzr(1),
     +    Bxrc(1,1,1),Byrc(1,1,1),Bzrc(1,1,1),b(1),hintc(1))
         else
          call fint_my(3,avv(1),NA(1),xyzr(1),
     +    Bxr(1,1,1),Byr(1,1,1),Bzr(1,1,1),b(1),hint(1))
         endif

        endif

        

 999    continue
        do i=1,3
           b(i)=b(i)*fscale
           if(bcorr>=0)b(i)=b(i)*bcorr
        enddo
        END



      SUBROUTINE GUFLDR(VV,B)
*     ======================
#      include "mfield.h"

*
*      rotated field
*
      data jj/0/
      data init/0/ 
      DIMENSION B(3),VV(3),AVV(3),ab(3)
*
*     transform the coordinates      
*
      double precision rot(3,3)
      save rot
      if(init.eq.0)then
       call a2m(rot)
       init=1
       write(*,*)'r0',r0
       write(*,*)' rotmatrix',rot
      endif
        do i=1,3
          avv(i)=r0(i)
         do j=1,3
          avv(i)=avv(i)+vv(j)*rot(i,j)
         enddo
        enddo
        if(rphi.eq.0)then
        if(abs(avv(1)).gt.x(nx).or.abs(avv(2)).gt.y(ny).or.
     +    abs(avv(3)).gt.z(nz))then
            call vzero(b(1),3)
            goto 999
        ENDIF
          call fint_my(3,avv(1),NA(1),xyz(1),
     +    Bx(1,1,1),By(1,1,1),Bz(1,1,1),ab(1),hint(1))
        else
          t1=sqrt(avv(1)**2+avv(2)**2)
          t2=atan2(avv(2),avv(1))
          avv(1)=t1
          avv(2)=t2
          if(avv(2).lt.phi(1))then
            avv(2)=avv(2)+4*acos(0.)
          endif
          if(avv(1).gt.rad(nrad))avv(1)=rad(nrad)
          if(avv(3).gt.zr(nzr))avv(3)=zr(nzr)
          if(avv(3).lt.zr(1))avv(3)=zr(1)
        if(abs(avv(1)).gt.rad(nrad).or.abs(avv(2)).gt.phi(nphi).or.
     +    abs(avv(3)).gt.zr(nzr))then
            call vzero(b(1),3)
            goto 999
        ENDIF
          call fint_my(3,avv(1),NA(1),xyzr(1),
     +    Bxr(1,1,1),Byr(1,1,1),Bzr(1,1,1),ab(1),hint(1))

        endif
*
* transform the field itself
*
        do i=1,3
          b(i)=0
         do j=1,3
          b(i)=b(i)+ab(j)*rot(j,i)
         enddo
        enddo
          

 999    END





      SUBROUTINE GUFLDRC(VV,B)
*     ======================
#      include "mfield.h"

*
*      rotated field
*
      data jj/0/
      data init/0/ 
      DIMENSION B(3),VV(3),AVV(3),ab(3)
*
*     transform the coordinates      
*
      double precision rot(3,3)
      save rot
       call a2m(rot)
        do i=1,3
          avv(i)=r0(i)
         do j=1,3
          avv(i)=avv(i)+vv(j)*rot(i,j)
         enddo
        enddo
        if(rphi.eq.0)then
        if(abs(avv(1)).gt.x(nx).or.abs(avv(2)).gt.y(ny).or.
     +    abs(avv(3)).gt.z(nz))then
            call vzero(b(1),3)
            goto 999
        ENDIF
          call fint_my(3,avv(1),NA(1),xyz(1),
     +    Bxc(1,1,1),Byc(1,1,1),Bzc(1,1,1),ab(1),hintc(1))
        else
          t1=sqrt(avv(1)**2+avv(2)**2)
          t2=atan2(avv(2),avv(1))
          avv(1)=t1
          avv(2)=t2
          if(avv(2).lt.phi(1))then
            avv(2)=avv(2)+4*acos(0.)
          endif
        if(abs(avv(1)).gt.rad(nrad).or.abs(avv(2)).gt.phi(nphi).or.
     +    abs(avv(3)).gt.zr(nzr))then
            call vzero(b(1),3)
c            write(*,*)' outside!!!'
            goto 999
        ENDIF
          call fint_my(3,avv(1),NA(1),xyzr(1),
     +    Bxrc(1,1,1),Byrc(1,1,1),Bzrc(1,1,1),ab(1),hintc(1))

        endif
*
* transform the field itself
*
        do i=1,3
          b(i)=0
         do j=1,3
          b(i)=b(i)+ab(j)*rot(j,i)
         enddo
        enddo
          

 999    END



      SUBROUTINE TKFLD(XX,HXY)
*     ========================
#      include "mfield.h"
       
          real xx(3),axx(3),hxy(3,3)
         if(abs(xx(1)).gt.x(nx).or.abs(xx(2)).gt.y(ny).or.
     +    abs(xx(3)).gt.z(nz))then
           call vzero (hxy(1,1),9)
           goto 999
          endif
#ifdef __G4AMSU__
           call vzero (hxy(1,1),9)
           goto 999
#endif          

c          sx=sign(1.,xx(1))
c          sy=sign(1.,xx(2))
c          sz=sign(1.,xx(3))
          do i=1,2
c            axx(i)=abs(xx(i))
            axx(i)=xx(i)
          enddo
          CALL BZCORR(xfac)
c          axx(3)=abs(xx(3))*xfac
          axx(3)=(xx(3))*xfac
c          if(iniok.eq.1)then

c           hxy(1,1) = FINT(3,aXx(1),NA(1),xyz(1),Bdx(1,1,1,1))
c           hxy(2,1) = FINT(3,aXx(1),NA(1),xyz(1),Bdy(1,1,1,1))
c           hxy(3,1) = FINT(3,aXx(1),NA(1),xyz(1),Bdz(1,1,1,1))
c           hxy(1,2) = FINT(3,aXx(1),NA(1),xyz(1),Bdx(1,1,1,2))
c           hxy(2,2) = FINT(3,aXx(1),NA(1),xyz(1),Bdy(1,1,1,2))
c           hxy(3,2) = FINT(3,aXx(1),NA(1),xyz(1),Bdz(1,1,1,2))
c           call fint_my(3,aXx(1),NA(1),xyz(1),Bdx(1,1,1,1),
c     +     Bdy(1,1,1,1),Bdz(1,1,1,1),hxy(1,1),hint(1))
c           call fint_my(3,aXx(1),NA(1),xyz(1),Bdx(1,1,1,2),
c     +     Bdy(1,1,1,2),Bdz(1,1,1,2),hxy(1,2),hint(1))
           call fint_my_2(3,aXx(1),NA(1),xyz(1),Bdx(1,1,1,1),
     +     Bdy(1,1,1,1),Bdz(1,1,1,1),Bdx(1,1,1,2),
     +     Bdy(1,1,1,2),Bdz(1,1,1,2),hxy(1,1),hint(1))
c          else
c           u(1)=axx(1)
c           v(1)=axx(2)
c           w(1)=axx(3)
c           ifail=-1
c           call  e01thf(mmax,xe,ye,ze,bxe,iqx,liq,rqx,lrq,1,
c     +     u,v,w,q,qx,qy,qz,ifail)
c           if(ifail.ne.0)then
c            write(*,*)' e01thf failed on xd ',ifail
c            hxy(1,1) = FINT(3,aXx(1),NA(1),xyz(1),Bdx(1,1,1,1))
c            hxy(1,2) = FINT(3,aXx(1),NA(1),xyz(1),Bdx(1,1,1,2))
c          else
cc           d1=qx(1)
cc           d2=qy(1)
cc           call hf1(-703,d1-hxy(1,1),1.)
cc           call hf1(-705,d2-hxy(1,2),1.)
cc           call hf2(-704,d1,hxy(1,1),1.)
cc           call hf2(-706,d2,hxy(1,2),1.)
c           hxy(1,1)=qx(1)
c           hxy(1,2)=qy(1)
c          endif
c          ifail=-1
c          call  e01thf(mmax,xe,ye,ze,bye,iqy,liq,rqy,lrq,1,
c     +    u,v,w,q,qx,qy,qz,ifail)
c          if(ifail.ne.0)then
c           write(*,*)' e01thf failed on yd ',ifail
c            hxy(2,1) = FINT(3,aXx(1),NA(1),xyz(1),Bdy(1,1,1,1))
c            hxy(2,2) = FINT(3,aXx(1),NA(1),xyz(1),Bdy(1,1,1,2))
c          else
cc            d1=qx(1)
cc            d2=qy(1)
cc            call hf1(-803,d1-hxy(2,1),1.)
cc            call hf1(-805,d2-hxy(2,2),1.)
cc            call hf2(-804,d1,hxy(2,1),1.)
cc            call hf2(-806,d2,hxy(2,2),1.)
c           hxy(2,1)=qx(1)
c           hxy(2,2)=qy(1)
c          endif
c          ifail=-1
c          call  e01thf(mmax,xe,ye,ze,bze,iqz,liq,rqz,lrq,1,
c     +    u,v,w,q,qx,qy,qz,ifail)
c          if(ifail.ne.0)then
c           write(*,*)' e01thf failed on zd ',ifail
c            hxy(3,1) = FINT(3,aXx(1),NA(1),xyz(1),Bdz(1,1,1,1))
c            hxy(3,2) = FINT(3,aXx(1),NA(1),xyz(1),Bdz(1,1,1,2))
c          else
cc            d1=qx(1)
cc            d2=qy(1)
cc            call hf1(-903,d1-hxy(3,1),1.)
cc            call hf1(-905,d2-hxy(3,2),1.)
cc            call hf2(-904,d1,hxy(3,1),1.)
cc            call hf2(-906,d2,hxy(3,2),1.)
c           hxy(3,1)=qx(1)
c           hxy(3,2)=qy(1)
c          endif
c         endif

          if(magstat.gt.0 )then
c           CALL BTEMPCOR (factor)
c           hxy(1,1) = hxy(1,1) * sx * factor
c           hxy(2,1) = hxy(2,1) * sy * factor
c           hxy(3,1) = hxy(3,1) * sz * factor
c           hxy(1,2) = hxy(1,2) * sy * factor
c           hxy(2,2) = hxy(2,2) * sx * factor
c           hxy(3,2) = hxy(3,2) * sx*sy*sz * factor
          else
           hxy(1,1) = 0
           hxy(2,1) = 0
           hxy(3,1) = 0
           hxy(1,2) = 0
           hxy(2,2) = 0
           hxy(3,2) = 0
          endif
999       end




            subroutine a2m(rot)
#include    "mfield.h"
            double precision rot(3,3)
              real *8 cp,sp,cy,sy,cr,sr,l1,m1,n1,l2,m2,n2,l3,m3,n3,pirad
             pirad=2*acos(0.)/180.
             cp=cos(pitch*pirad)
             sp=sin(pitch*pirad)
             cy=cos(yaw*pirad)
             sy=sin(yaw*pirad)
             cr=cos(roll*pirad)
             sr=sin(roll*pirad)
             l1=cy*cp
             m1=-sy
             n1=cy*sp
             l2=cr*sy*cp-sr*sp
             m2=cr*cy
             n2=cr*sy*sp+sr*cp
             l3=-sr*sy*cp-cr*sp
             m3=-sr*cy
             n3=-sr*sy*sp+cr*cp
             rot(1,1)=l1
             rot(1,2)=m1
             rot(1,3)=n1
             rot(2,1)=l2
             rot(2,2)=m2
             rot(2,3)=n2
             rot(3,1)=l3
             rot(3,2)=m3
             rot(3,3)=n3
             end

             subroutine fcn(np,grad,f,xc,iflag,idum)
#include "mfield.h"
             real *8 grad(1),f,xc(1)
             real *4 x1(3),b1(3),b1c(3)
             f=0
                  r0(1)=xc(2)
                  r0(2)=xc(3)
                  r0(3)=xc(4)
                  pitch=xc(5)
                  yaw=xc(6)
                  roll=xc(7)
                  fscale=xc(1)
             if(rphi.eq.0)then     
             do i=1,nx
               do j=1,ny
                 do k=1,nz
                  x1(1)=x(i)
                  x1(2)=y(j)
                  x1(3)=z(k)
                  call gufld(x1,b1)
                  call gufldrc(x1,b1c)
                   r=sqrt(x1(1)**2+x1(2)**2)
                    if(r.le.41..and.abs(x1(3)).le.45)then
                  do l=1,3
                    f=f+(b1(l)-b1c(l)*fscale)**2
                  enddo
                  endif
                 enddo
               enddo
              enddo
              else
            do i=1,nrad
               do j=1,nphi
                 do k=1,nzr
                  x1(1)=rad(i)*cos(phi(j))
                  x1(2)=rad(i)*sin(phi(j))
                  x1(3)=zr(k)
                  call gufld(x1,b1)
                  call gufldrc(x1,b1c)
                   r=sqrt(x1(1)**2+x1(2)**2)
                    if(r.le.41..and.abs(x1(3)).le.45)then
                  do l=1,3
                    f=f+(b1(l)-b1c(l)*fscale)**2
                  enddo
                  endif
                 enddo
               enddo
              enddo

              endif
             if(iflag.eq.3)then
*
* final call
*
              write(*,*)'finally',(xc(k),k=1,np),f
             else
             endif
             end


             subroutine fcn2(np,grad,f,xc,iflag,idum)
#include "readmf.h"

       common/MAGSFFKEY/magstat,fscale,ecutge,r0(3),pitch,yaw,roll,rphi
       integer magstat,rphi
       real fscale,ecutge,r0,pitch,yaw,roll
      common /amsdatadir/amsdlength,amsdblength,amsdatadir,
     +                   amsdatabase,fname
      integer amsdlength,amsdblength,iniok
      character *128 amsdatadir,amsdatabase
      character *200 fname
             real *8 grad(1),f,xc(1)
             real *4 x1(3),b1(3),b1c(3)
             data init /0/
             if(init.eq.0)then
                init=1
                call readfieldmeasured
             endif     
             if(iflag.eq.3)then
              open(35,file='/f2users/choutko/mfd230_'//fname,
     +                status='new',form='formatted')         
             endif
             f=0
                  r0(1)=xc(2)
                  r0(2)=xc(3)
                  r0(3)=xc(4)
                  pitch=xc(5)
                  yaw=xc(6)
                  roll=xc(7)
                  fscale=xc(1)
c                  pitch=0
                  yaw=0
c                  roll=0
                  r0(1)=0
                  r0(2)=0
c                  r0(3)=0
             ntot=0
             do i=1,nrad
               do j=1,nphi
                 do k=1,nzr
                  call vzero(x1(1),3)
                  do l=1,3
                   x1(l)=brcoo(l,i,j,k)
                  enddo
                  if(.not.(x1(1).eq.0.and.x1(2).eq.0.and.x1(3).eq.0)
     +            .and.((abs(zr(k)).le.76..and.abs(rad(i)).lt.41.)
     +            .or.rphi.eq.0))then

                     ntot=ntot+1
                  else
                     goto 21
                  endif 
c                 x1(1)=x1(1)*xc(8) 
c                  x1(2)=x1(2)*xc(8)
c                   x1(3)=x1(3)*xc(8)
                  call gufldrc(x1,b1)
                  if(sqrt(b1(1)**2+b1(2)**2+b1(3)**2).eq.0)then
                    ntot=ntot-1
                    goto 21
                  endif
                  xphi=(phi(j))/180.*2*acos(0.)
                  xbr=b1(1)*cos(xphi)+b1(2)*sin(xphi)
                  f=f+(br(i,j,k)-xbr*fscale)**2 
                  do l=1,3
                   x1(l)=bphicoo(l,i,j,k)
                  enddo
c                  x1(1)=x1(1)*xc(8)
c                   x1(2)=x1(2)*xc(8)
c                   x1(3)=x1(3)*xc(8)

                  call gufldrc(x1,b1)
                  xbp=-b1(1)*sin(xphi)+b1(2)*cos(xphi)
                  f=f+(bphi(i,j,k)-xbp*fscale)**2 
                  do l=1,3
                   x1(l)=bzcoo(l,i,j,k)
                  enddo
c                  x1(1)=x1(1)*xc(8)
c                  x1(2)=x1(2)*xc(8)
c                   x1(3)=x1(3)*xc(8)
                  call gufldrc(x1,b1)
                  xbz=b1(3)
                  f=f+(bz(i,j,k)-xbz*fscale)**2
                  if(iflag.eq.3)then
                   write(35,*)br(i,j,k),bphi(i,j,k),bz(i,j,k),
     +             xbr*fscale,xbp*fscale,xbz*fscale,
     +             rad(i),phi(j),zr(k)
                   endif
 21               continue
                 enddo
               enddo
              enddo
             f=f/9e-6/ntot/3
             if(iflag.eq.3)then
*
* final call
*
              write(*,*)'finally',(xc(k),k=1,np),f,ntot
              close(35)
             else
             endif
             end

             subroutine readfieldmeasured
             implicit none
#include "readmf.h"
                         
              character *256 fnam
              real xzz(nzr+1)
              real xrad,xphi,xz,xbr,xr_x,xr_y,xr_z,xphir,
     +        xbp,xp_x,xp_y,xp_z,xbz,xz_x,xz_y,xz_z
              integer i,j,k,l,m,iphi1,ir1,iz1,ntot
              data xzz/-80,-75,-70,-65,-60,-55,-50,-45,-40,-35,-30,-25,
     +        -20,-15,-10,-5,-4,-2,0,2,4,5,10,15,20,25,30,35,40,
     +        45,50,55,60,65,70,75,77,80/
              open(21,file='/f2users/choutko/mfield_230.txt',
     +        STATUS='OLD',form='formatted')
              call vzero(rad(1),nrad+nphi+nzr+nrad*nphi*nzr*12)
              ntot=0
 10           continue
              read(21,*,err=11,end=11)xrad,xphi,xz,xphir,
     +        xbr,xr_x,xr_y,xr_z,
     +        xbp,xp_x,xp_y,xp_z,xbz,xz_x,xz_y,xz_z
              ntot=ntot+1
              ir1=(xrad+5.9/2+41.3)/5.9+1
              iphi1=(xphi+5./2)/5.+1
              iz1=0
              do k=1,nzr
                  if(xz+0.5.gt.xzz(k).and.xz+0.5.le.xzz(k+1))then
                    iz1=k
                  endif
              enddo
             if(ir1.lt.1)stop' ir1 0'
             if(iphi1.lt.1)stop' iphi1 0'
             if(iz1.lt.1)stop' iz1 0'
             if(ir1.gt.nrad)then
               write(*,*)xrad,ir1
               stop 'r1 too big'
             endif
             if(iphi1.gt.nphi)then
               write(*,*)xphi,iphi1
               stop 'phi1 too big'
             endif
             if(iz1.gt.nzr)then
               write(*,*)xz,iz1
               stop 'z1 too big'
             endif
             rad(ir1)=xrad
             phi(iphi1)=xphir
             zr(iz1)=xz
             br(ir1,iphi1,iz1)=xbr
             bphi(ir1,iphi1,iz1)=xbp
             bz(ir1,iphi1,iz1)=xbz
             brcoo(1,ir1,iphi1,iz1)=xr_x
             brcoo(2,ir1,iphi1,iz1)=xr_y
             brcoo(3,ir1,iphi1,iz1)=xr_z
             bphicoo(1,ir1,iphi1,iz1)=xp_x
             bphicoo(2,ir1,iphi1,iz1)=xp_y
             bphicoo(3,ir1,iphi1,iz1)=xp_z
             bzcoo(1,ir1,iphi1,iz1)=xz_x
             bzcoo(2,ir1,iphi1,iz1)=xz_y
             bzcoo(3,ir1,iphi1,iz1)=xz_z
             goto 10
 11          continue
             close(21)
             write(*,*)' readfieldmeasured ntot=',ntot
             end

#ifdef __ROOTSHAREDLIBRARY__
          subroutine tkfldc(xx,hxy)
          common /tkigufldC/i,j,xxy(3)
          real xx(3),hxy(3,3)
          real *8 xval,hbase,der(14),erest(14),tkigufld
          end
#else
     
      subroutine tkfldc(xx,hxy)
          common /tkigufldC/i,j,xxy(3)
          real xx(3),hxy(3,3)
          real *8 xval,hbase,der(14),erest(14),tkigufld
          external tkigufld
          do i=1,3
            do j=1,3
          ifail=1
          nder=1
          hbase=0.5
          call ucopy(xx(1),xxy(1),3)
          xval=xx(i)
          call d04aaf(XVAL, NDER, HBASE, DER, EREST,
     +    tkigufld,ifail)
            hxy(j,i)=der(1)
            enddo
          enddo
          end
          real *8 function tkigufld(x)
          common /tkigufldC/i,j,xx(3)
          real h(3)
          real *8 x
          xx(i)=x
          call gufld(xx,h)
          tkigufld=h(j)
          end
#endif
          subroutine setbcorr(bcor)
#      include "mfield.h"
          real bcor
          bcorr=bcor
          end
