*  $Id: rkms.F,v 1.1 2003/10/29 15:28:53 choutko Exp $
*--------1---------2---------3---------4---------5---------6---------7--
      subroutine rkms_rig(npo,npl,xyz,dxyz,ipa,Rini,Out)
*   A.Chikanian, Yale, 2003
*-------------------------------------------------------
* Input:   npo   - # points
*          npl   - plane #, associated with each point
*          xyz(3,npl) - 3d coordinates
*          dxyz(npl)  - errors
*          ipa   - particle Geant #
*          Rini  - initial Rigidity value
* Output:  Out   - reconstructed parameters
*          Out(1),(2),(3)- x, y, z 
*          Out(4),(5)    - Theta, Phi
*          Out(6)        - Rigidity
*          Out(7)        - Chi2
*          Out(8)        - not used
*          Out(9)        - 1/Rigidity Err.
*------------------------------------------
      implicit none
*     -------------------
#include "rkms.h"
*     -------------------
      integer npo,npl(npo),ipa,i
      real xyz(3,npo),dxyz(3,npo)
      real x(NPma),y(NPma),z(NPma),dx(NPma),dy(NPma)
      real Rini,Out(9),Chi2,R,dR
      double precision Xout(5),Eout(5)
*---------------------------------------------------

      do i=1,npo
        x(i) =xyz(1,i)
        y(i) =xyz(2,i)
        z(i) =xyz(3,i)
        dx(i)=dxyz(1,i)
        dy(i)=dxyz(2,i)
      enddo

c      call R_ini(Rini)
      call  rkms_fit(npo,npl,x,y,z,dx,dy,ipa,Rini,Xout,Eout,Chi2)

      R = dsqrt(Xout(1)**2+Xout(2)**2+Xout(3)**2)
      dR=dsqrt((Xout(1)*Eout(1))**2 + (Xout(2)*Eout(2))**2
     +        +(Xout(3)*Eout(3))**2)/R
*     =============================================
      Out(1) = Xout(4)                            ! x
      Out(2) = Xout(5)                            ! y
      Out(3) = z(1)                               ! z
      Out(4) = acos(sngl(Xout(3))/R)              ! Theta
      Out(5) = atan2(sngl(Xout(2)),sngl(Xout(1))) ! Phi
      Out(6) = R                                  ! Rigidity
      Out(7) = Chi2                               ! Chi2
*     Out(8)                                      ! Nothing
      Out(9) = dR/R/R                             ! (1/Rig) Error
      write(*,*)' rig,err',out(6),out(7),out(9)
*     =============================================
      end

*--------1---------2---------3---------4---------5---------6---------7--
      subroutine rkms_fit(npo,npl,x,y,z,dx,dy,ipa,Rini,Xout,Er,Chi2) ! MINUIT
*   A.Chikanian, Yale, 2003
*--------------------------------------------------------------------
* Input:   npo   - # points
*          npl   - plane #, associated with each point
*          x(npl),y(npl),z(npl) - 3d coordinates
*          dx(npl),dy(npl) - errors
*          ipa   - particle Geant #
*          Rini  - initial Rigidity value
* Output:  Xout  -  x, y, Rx, Ry, Rz
*          Er    - dx,dy,dRx,dRy,dRz
*          Chi2  - chi2
*-----------------------------------------
      implicit none  !!!! commenting helps with realy STUPID problem (see next).
c      real     FunMi !!!! Required by STUPID PAW, but doesn't like it !!!????
      External FunMI
      double precision Par(5),St(5),arglis(3),Xout(5),Er(5),bnd1,bnd2
*------------------------------------------
*              dPx  dPy  dPz   dx    dy    Initial MIGRAD steps 
*             (GeV)(GeV)(GeV) (cm)  (cm)
c      data St/.001000,.001000,.010000,.0010,.0015/  ! 9.0
c      data St/.000100,.000100,.005000,.0010,.0015/  ! 8.99
c      data St/.000100,.000100,.002000,.0010,.0015/  ! 8.99
c      data St/.000100,.000100,.002000,.0010,.0010/  ! 8.99*
       data St/.000100,.000100,.002000,.0005,.0005/  ! 8.98*
c      data St/.000100,.000100,.001000,.0005,.0005/  ! 9.01*
c      data St/.000100,.000100,.000500,.0005,.0005/  ! 9.01*
c      data St/.000100,.000100,.001000,.0003,.0003/  ! 9.01*
c      data St/.000010,.000010,.001000,.0010,.0015/  ! 9.6
*************  .000005,.000005,.000100,.0002,.0002/  ! Deriv.Steps ***
*---------------------------------------------------------------------
      double precision zero
      data             zero/0.d0/
      double precision grad(5),A
      data             grad/5*0.D0/
      integer i,it,ivar,ierflg,na   ,Npa
      data                     na/0/,Npa/5/ ! # of parameters
      character*10 CHNAM
      data         CHNAM/' '/               ! parameter names

      integer npo,npl(npo),NitMa   ,iercou
      data                 NitMa/2/,iercou/0/

      integer ipa,iflag
      real x(npo),y(npo),z(npo),dx(npo),dy(npo)
      integer lun7
      data lun7/8/
*---------------------------------------------
#include "rkms.h"
c      include 'trkz0.inc' !--- trkZ0
*-------------------------
      real z1
      real Rini,Chi2,Rad
*-------------------------------
      logical first
      data    first/.true./
      save first,iercou,CHNAM,zero,npa,na,St,grad

      if(first) then ! ----------------- first
        first = .false.


        open(unit=lun7,form='formatted',file='/dev/null',
     +  status='unknown')
        Call MNINIT(5,lun7,lun7) ! Read, Write, Save
        na = 1             ! # of arguments
        arglis(1)=-1.      ! suppr. output
        Call MNEXCM(FunMI,'SET PRINT',arglis,na,ierflg,0)
        If(ierflg.ne.0) print*,'MNEXCM: Print ierflg=',ierflg
        arglis(1)= 1.
        Call MNEXCM(FunMI,'SET GRAD',arglis,na,ierflg,0) ! Gradients
        If(ierflg.ne.0) print*,'MNEXCM: Grad ierflg=',ierflg
*  Errors calculation !!!### Not sure that it works !!!???
        arglis(1)=1.
        Call MNEXCM(FunMI,'SET ERRo',arglis,na,ierflg,0)
        If(ierflg.ne.0) print*,'MNEXCM: ERRo ierflg=',ierflg
      endif          ! ----------------- end first

      if(npo.lt.3) return !!!

      z1=z(1) ! if so Par(4)=x(1), Par(5)=y(1)
      rad = sqrt((x(2)-x(1))**2+(y(2)-y(1))**2+(z(2)-z(1))**2)
cxy1 ---
      Par(1)=Rini*(x(2)-x(1))/rad                     ! Px0
      Par(2)=Rini*(y(2)-y(1))/rad                     ! Py0
      Par(3)=Rini*(z(2)-z(1))/rad                     ! Pz0
      Par(4)=x(1) + (z1-z(1))/(z(2)-z(1))*(x(2)-x(1)) !  x0
      Par(5)=y(1) + (z1-z(1))/(z(2)-z(1))*(y(2)-y(1)) !  y0
c ### debug1 ###
*-------------  common/mymu/... refreshing hits information
      npoc=npo
      do i=1,NPma
        if(i.le.npo) then
          xc(i)=x(i)
          yc(i)=y(i)
          zc(i)=z(i)
        else
          xc(i)=-999.
          yc(i)=-999.
          zc(i)=-999.
        endif
      enddo
*---------------------------- Initial values setup
      do i=1,Npa
        Call MNPARM(i,CHNAM,Par(i),St(i),zero,zero,IERFLG)! Init.Val.Setup
        If(ierflg.ne.0) write(6,*)'### MNPARM: i,ierflg=',i,ierflg
      enddo
*----------------------------------------------- Minimization  
c1      it=0  ! iteration counter (if #iter. not fixed)
c2        DO it=1,NitMa ! always fixed # of iterations !!!
*     ======================================+
 10   call wxy_matr(npo,npl,dx,dy,ipa,Rini) ! Full Matr.
*     ======================================+
c ### debug2 ###
*     ================================================
      na=0  ! # of arguments in arglis (MNEXCM)      !
      Call MNEXCM(FunMI,'MIGRAD',arglis,na,ierflg,0) !
*     ================================================
      if(ierflg.ne.0.and.iercou.le.1) then        !
        write(6,*)'MNEXCM: ierflg=',ierflg,iercou !
        iercou=iercou+1                           !
      endif                                       !

      do i=1,Npa                                          ! Parameters
        Call MNPOUT(i,CHNAM,Xout(i),Er(i),bnd1,bnd2,ivar) ! Extraction
      enddo

      Chi2=0.
      if(2.*npo-npa.gt.0.) Chi2=Dhi2/(2.*npo-npa)         ! Chi2
c1      it=it+1
c1      if(Chi2.gt.10..and.it.lt.NitMa) go to 10
c2        ENDDO ! it NitMa
c ### debug3 ###
      end

*--------1---------2---------3---------4---------5---------6---------7-- FCN
      Subroutine FunMI(Npa,grad,Fu,par,iflag) ! ,futil)
*   A.Chikanian, Yale, Aug,2003
*-----------------------------------
      implicit none
      integer Npa,iflag,k
      double precision Fu,Fun,grad(npa),par(npa)
*-------------------------------------------
c      write(6,'(    a3,5f10.5)')'Par',par! debug ###
      if(iflag.eq.2) call FunDer(Npa,par,grad)
      Fu = Fun(Npa,par)
c      write(6,*)'iflag,Fu',iflag,Fu  ! debug ###
      end

*--------1---------2---------3---------4---------5---------6---------7--
      double precision Function Fun(Npa,par)
*   A.Chikanian, Yale, Aug,2003
*-----------------------------------
      implicit none
      integer Npa,i
      double precision chi2su,par(Npa)
*-------------------------
#include "rkms.h"
*-------------------------
c4      real pin(3),xin(3),xt(NPma),yt(Npma)           ! r4
      double precision pin(3),xin(3),xt(NPma),yt(NPma) ! r8

      pin(1) = par(1) ! Px or cx
      pin(2) = par(2) ! Py or cy
      pin(3) = par(3) ! Pz or R
      xin(1) = par(4) ! x
      xin(2) = par(5) ! y
      xin(3) = zc(1)  ! z

      do i=1,Npma     !***   ???
        xt(i)=-999.   !**************
        yt(i)=-999.   !*** let it be !!!
      enddo           !*****************
*     ==========================
      call rk_trk(pin,xin,xt,yt)
      Fun = chi2su(xt,yt)
*     ==========================
c ### debug4 ###
      end

*--------1---------2---------3---------4---------5---------6---------7--
      subroutine FunDer(npa,par,der)
*   A.Chikanian, Yale, Aug,2003
*---   numerical calculation of Fun and derivatives.
*--------------------------------------------------
      implicit none
      integer npa,j
      double precision f1,f2,x0,dp,Fun,par(npa),der(npa)
*==================================================
      double precision dp0(5),xx(5)                !
      data dp0/.000005,.000005,.000100,.0002,.0002/! Opt.Steps
*                 GeV     GeV     GeV    cm    cm  !
*==================================================
      do j=1,npa
        xx(j)=par(j)
      enddo
      do j=1,npa
        dp=dp0(j) ! /2.
        x0=xx(j)
        xx(j)=x0-dp
        f1=Fun(npa,xx)
        xx(j)=x0+dp
        f2=Fun(npa,xx)
        der(j) = .5*(f2-f1)/dp
        xx(j)=x0
      enddo
c      write(6,'(a6,5(1pe12.4))')' der  ',der ! debug ###
c      write(6,'(a6,5(1pe12.4))')'   x  ',xp  ! debug ###
      end

*--------1---------2---------3---------4---------5---------6---------7--
      double precision function Chi2Su(x0,y0)
*   A.Chikanian, Yale, Feb,2003
*---------------------------------------------
* Input    x0     - x-coord. for current iteration
*          y0     - y-coord. for current iteration
* Output:  Chi2Su - chi2 walue
*---------------------------------------------
      implicit none
      integer i,j
*-------------------------
#include "rkms.h"
*-------------------------
c4      real x0(NPma),y0(NPma)           ! r4
      double precision x0(NPma),y0(NPma) ! r8
      double precision xi,yi
*  xc,yc - hits coordinates
*--------------------------------------------------------
      Chi2Su=wxy(1,1)*((xc(1)-x0(1))**2+(yc(1)-y0(1))**2)
      do i=2,npoc
        xi=xc(i)-x0(i)
        yi=yc(i)-y0(i)
        Chi2Su=Chi2Su+wxy(i,i)*(xi**2+yi**2)
        if(i.lt.npoc) then
        do j=i+1,npoc
          Chi2Su=Chi2Su+2.*wxy(j,i)*(xi*(xc(j)-x0(j))+yi*(yc(j)-y0(j)))
        enddo
        endif
      enddo
      Dhi2=Chi2Su
      if(Chi2Su.ge.0.) return
c ### debug5 ##
*######################################################################! debug
c      write(6, *)'event',ievent                                       ! debug
      write(6, *)'Chi2 =',Chi2Su                                       ! debug
      write(6, *)'npoc',npoc                                           ! debug
      write(6,10)'xc',(xc(i),i=1,npoc)                                 ! debug
      write(6,10)'x0',(x0(i),i=1,npoc)                                 ! debug
      write(6,10)'yc',(yc(i),i=1,npoc)                                 ! debug
      write(6,10)'y0',(y0(i),i=1,npoc)                                 ! debug
      write(6,10)'zc',(zc(i),i=1,npoc)                                 ! debug
      write(6,10)'rxy'                                                 ! debug
     +             ,(dsqrt((xc(i)-x0(i))**2+(yc(i)-y0(i))**2),i=1,npoc)! debug
 10   format(a3,8f11.4)                                                ! debug
      write(6, *)'wxy',npoc                                            ! debug
      do i=1,npoc                                                      ! debug
        write(6,'(8f11.3)')(wxy(j,i),j=1,npoc)                         ! debug
      enddo                                                            ! debug
      do i=1,npoc                                                      ! debug
        write(6,'(i3,a6,8f11.4)')i,' dX,dY',xc(i)-x0(i),yc(i)-y0(i)    ! debug
      enddo                                                            ! debug
      write(6, *)'j,i,wxy*(dX**2+dY**2),Chi2'                          ! debug
      Chi2Su=wxy(1,1)*((xc(1)-x0(1))**2+(yc(1)-y0(1))**2)              ! debug
      write(6,*)1,1,Chi2Su,Chi2Su                                      ! debug
      do i=2,npoc                                                      ! debug
        xi=xc(i)-x0(i)                                                 ! debug
        yi=yc(i)-y0(i)                                                 ! debug
        Chi2Su=Chi2Su+wxy(i,i)*(xi**2+yi**2)                           ! debug
        write(6,*)i,i,wxy(i,i)*(xi**2+yi**2),Chi2Su                    ! debug
        if(i.lt.npoc) then                                             ! debug
        do j=i+1,npoc                                                  ! debug
          Chi2Su=Chi2Su+2.*wxy(j,i)*(xi*(xc(j)-x0(j))+yi*(yc(j)-y0(j)))! debug
          write(6,*)i,j,2.*wxy(j,i)*(xi*(xc(j)-x0(j))+yi*(yc(j)-y0(j)))! debug
     +                                                         ,Chi2Su ! debug
        enddo                                                          ! debug
        endif                                                          ! debug
      enddo                                                            ! debug
      stop                                                             ! debug
*    ##################################################################! debug
      end

*--------1---------2---------3---------4---------5---------6---------7--
      subroutine rk_trk(pin,xin,x0,y0)
*---------------------------------------------------
* Runge-Kutta integration for particle in Magn.field
* A.Chikanian, Yale, Feb.,2003
*----------------------------------------------------
*  Input:  pin(3) - initial momentum Px,Py,Pz (GeV/c)
*          xin(3) - initial position  x, y, z (cm)
*  Output: pou(3) -   final momentum Px,Py,Pz (GeV/c)
*          xou(3) -   final position  x, y, z (cm)
*-------------------------------------------------
*###            W A R N I N G                  ###
*###  Pin is really assumed as Rigidity !!!    ###
*###  If Momentum are dezirable,               ###
*###  Charge of particle should be provided.   ###
*###      call GRKUTA (Charge,STEP,VECT,VOUT)  ###
*###  instead  GRKUTA (SigCha,STEP,VECT,VOUT)  ###
*-------------------------------------------------
      implicit none
      integer i,j,Nst,np
*------------------------
#include "rkms.h"
      double precision charge,sigcha ! for GRKUTA
      common/rktrk/    charge,sigcha
*------------------------
c*     ##############################! debug
c      INTEGER       IDUMM   ,IEVENT ! debug  insted 'geant.inc'
c      COMMON/GCFLAG/IDUMM(8),IEVENT ! debug  because STEP
c*     ##############################! debug
c4      real xm,pin(3),xin(3),x0(NPma),y0(NPma)           ! r4
c4      real step,vect(7),vout(7),Stot,vni                ! r4
      double precision xm,pin(3),xin(3),x0(NPma),y0(NPma) ! r8
      double precision step,vect(7),vout(7),Stot,vni      ! r8
*                           vect(7) is  x,y,z,px,py,pz,P
*                                   or  x,y,z,cx,cy,cz,P
*                                       1 2 3  4  5  6 7
      data Stot/150./ ! Max.range(cm) in Path Length      ! r4/r8
*---------------------------------------------------------
      logical first
      data    first/.true./
      double precision w
      save first,Nst,Step
*--------------------------------- initialisation
      if(first) then
        first=.false.
        step=5. ! cm !!! SO FAR it is NOT critical !!!
        Nst=int(Stot/step) ! Max.# of steps
        write(6,*)'MAX# of steps',Nst,'  Step',step,' cm.'
      endif
*-----------------------------------------------------
      vni=1.d0
      if(zc(1).lt.zc(2)) vni=-1.d0 ! back tracking
cxy3 ---
c4      vect(7)= sqrt(pin(1)**2+pin(2)**2+pin(3)**2) ! r4
      vect(7)=dsqrt(pin(1)**2+pin(2)**2+pin(3)**2)   ! r8
      do i=1,3
        vect(i  )=xin(i)         !  xyz
        vect(i+3)=pin(i)/vect(7) ! cxyz
      enddo

*----------------------------------------------- Runge Kutta
      np = 1
      do i=1,Nst
c4        call GRKUTA(SigCha,STEP,VECT,VOUT) ! r4
        call DRKUTA(SigCha,STEP,VECT,VOUT)    ! r8
          DO j=1,2 !!!! fixed close planes problem(AMS02)
        if(vni*vect(3).ge.vni*zc(np).and.vni*vout(3).lt.vni*zc(np))then
          w=(zc(np)-vout(3))/(vect(3)-vout(3))
          x0(np) = vect(1)*w + vout(1)*(1.d0-w)
          y0(np) = vect(2)*w + vout(2)*(1.d0-w)
c          write(6,'(a3,i2,a3,8f10.5)')' np',np,' x0',x0 ! debug ###
c          write(6,'(   5x,a3,8f10.5)')         ' y0',y0 ! debug ###
          if(np.eq.npoc) return  !!!!!!!!! Done
          np = np+1
        endif
          ENDDO  !### fixing closed planes probl.(or run with small STEP)
        do j=1,7 ! x,y,z,cx,cy,cz,P
          vect(j)=vout(j)
        enddo
      enddo ! Nst
c      write(6,*)'## rk_trk: planes: np,npoc',np-1,npoc,i-1,ievent ! debug ###
c      write(6,*)'np,Nst',np-1,Nst     ! debug ###
c      write(6,'(a3,8f10.5)')'Pin',pin ! debug ###
c      write(6,'(a3,8f10.5)')'Xin',xin ! debug ###
c      write(6,'(a3,8f10.5)')' x0',x0  ! debug ###
c      write(6,'(a3,8f10.5)')' y0',y0  ! debug ###
c      write(6,'(a3,8f10.5)')' xc',xc  ! debug ###
c      write(6,'(a3,8f10.5)')' yc',yc  ! debug ###
c      write(6,'(a3,8f10.5)')' zc',zc  ! debug ###
c      stop
      end

*--------1---------2---------3---------4---------5---------6---------7--
      subroutine wxy_matr(npo,npl,dx,dy,ipa,R)   ! 
*-------------------------------------------------
* Preparing Cov.Err.Matrix with Mult.Scattering
* A.Chikanian Feb,2003.
*------------------------------------------------------------
* Missing planes carring by putting dX,dY --> INF.(~22cm)
* COS(Th) - is INDIV. for each planes
* Symmetry of all matrix assumed
*===========================================================
* INP: npo      - # of x,y - hits
*      npl(npo) - array with plane # associated with hits
*    dx,dy(npo) - errors of x,y
*          ipa  - particle ID (Geant)
*            R  - Rigidity (gess)
*===========================================================
      implicit none
*------------------------
#include "rkms.h"
c      include 'trkz0.inc'!
      double precision charge,sigcha ! for GRKUTA
      common/rktrk/    charge,sigcha
*-------------------------------
      real dZ(NPma,NPma)
      integer ipa
      real R,RR,Rmin
      data      Rmin/0.300/ ! GeV
      integer npo,i,j,k,ii,jj,is,ier,nmspl
      integer ir(10),ifail,jfat
      integer npl(NPma),lw(NPma),mw(NPma)
      real dx(npo),dy(npo)
      real dms(NPma,NPma),ww(NPma,NPma)
      real dTh0,dTc2,oco2(NPma),E,be,P
      integer itr,nw
      real ama,cha,tl,ub
      character*20 pname
c      real det              !## MINV
c      integer ll(10),mm(10) !## MINV
*-----------------------
      logical first
      data    first/.true./
      save    first,dz,dTh0,dms,NMSpl

      if(first) then
        first=.false.
c ### debug6 ###
*------ For print only
        call gfpart(ipa,pname,itr,ama,cha,tl,ub,nw)
        P  =  R*cha
        E  =  sqrt(P**2+ama**2)
        be =  P/E !  or be from TOF
        dTh0 = 13.6e-3/(be*R)*sqrt(zz0)*(1.+0.038*alog(zz0)) ! dTh-plane
        write(6,*)'=== wxy ====================='
        write(6,*)'=== ipa,M,ch',ipa,ama,cha
        write(6,*)'=== R,E,be =',R,E,be
        write(6,*)'=== dTh0 =',dTh0
*---------------------------------
        do i=1,NPma-1  ! ------------------------------  dZ
          do j=i+1,NPma
            dz(j,i)=trkZ0(j)-trkZ0(i)
            dz(i,j)=dz(j,i)
          enddo
        enddo
        dTh0 = sqrt(2.)*13.6e-3*sqrt(zz0)*(1.+.038*alog(zz0))! KEEP IT
      endif ! --------------------------------------- first

      if(npo.lt.3) then
        write(6,*)'### wxy_matr: # of planes',npo
        return
      endif
*================================================== # of planes > 3 
      do i=1,NPma*NPma
        dms(i,1)=0.
        ww (i,1)=0.
        wxy(i,1)=0.
        if(i.le.NPma) oco2(i)=0.
      enddo
*================================== MS-matrix (short code)
      do j=npl(1)+1,npl(npo) ! NPma
        jj=j-npl(1)+1
        do i=npl(1)+1,j ! first plane is plane #1
          ii=i-npl(1)+1
          do k=npl(1),i-1
            dms(jj,ii)=dms(jj,ii)+dz(k,i)*dz(k,j)
          enddo
        enddo
      enddo 
      NMSpl = npl(npo)-npl(1)+1 ! # Mult.Scatt.planes
      call gfpart(ipa,pname,itr,ama,cha,tl,ub,nw)
      RR=max(R,Rmin)
      P  =  RR*cha
      E  =  sqrt(P**2+ama**2)
      be =  P/E ! or be from TOF
      dTc2 = (dTh0/(be*RR))**2 ! space factor 2 in dTh0
      charge = cha
      sigcha = 1.D0
      if(cha.lt.0.) sigcha = -1.D0 ! for GRKUTA 
c ### debug7 ###
*     ________________________________ Diagonal elements

      ww(1,1) = dY(1)**2 + dX(1)**2 ! dms(1,1)=0.
      ii=1
      do i=2,npo !_____________________Diagonal elements
        if(npl(i).gt.npl(i-1)+1) then
          do is=1,npl(i)-npl(i-1)-1
            ii=ii+1
            oco2(ii)=1.+((xc(i)-xc(i-1))**2+(yc(i)-yc(i-1))**2)
     +                                     /dz(npl(i),npl(i-1))**2
            ww(ii,ii)=dms(ii,ii)*dTc2 + 1000. ! dX,Y~22cm if missing plane
          enddo
        endif
        ii=ii+1
        oco2(ii)=1.+((xc(i)-xc(i-1))**2+(yc(i)-yc(i-1))**2)
     +                               /dz(npl(i),npl(i-1))**2
        ww(ii,ii)=dms(ii,ii)*dTc2*oco2(ii)**2 + dY(i)**2 + dX(i)**2
      enddo
c ### debug8 ###
      do i=3,NMSpl !___________________Nondiagonal elements
        do j=2,i-1
          ww(i,j)=dms(i,j)*dTc2*oco2(i)*oco2(j)
c          ww(j,i)=ww(i,j) !  ## MINV
        enddo
      enddo
c ### debug9 ###
      call rsinv(NMSpl,ww,NPma,ifail) ! F012 Symm.Matr.Inversion
      if(ifail.ne.0) write(6,*)'### rsinv: ifail=',ifail
c      call minv(ww,NMSpl,det,ll,mm)
c ### debug10 ###
*--------------------- Matr.Shrinking
      wxy(1,1)=ww(1,1)
      do j=2,npo
        jj=npl(j)-npl(1)+1
        do i=j,npo
          wxy(i,j)=ww(npl(i)-npl(1)+1,jj)
        enddo
      enddo
c ### debug11 ###
      end

*------------ /group/yaug1/prj/shik/AMS/MYAMS/TRKRKRES/MSMATR/drkuta.f
* modified by A.Chikanian Aug.26,2003
* EVERYTHING switched to DOUBLE PRECISION
*----------------------------------------------------------------------
CDECK  ID>, GRKUTA. 
*CMZ :  3.21/02 29/03/94  15.41.23  by  S.Giani
*-- Author :
      SUBROUTINE DRKUTA (CHARGE,STEP,VECT,VOUT)
C.
C.    ******************************************************************
C.    *                                                                *
C.    *  Runge-Kutta method for tracking a particle through a magnetic *
C.    *  field. Uses Nystroem algorithm (See Handbook Nat. Bur. of     *
C.    *  Standards, procedure 25.5.20)                                 *
C.    *                                                                *
C.    *  Input parameters                                              *
C.    *       CHARGE    Particle charge                                *
C.    *       STEP      Step size                                      *
C.    *       VECT      Initial co-ords,direction cosines,momentum     *
C.    *  Output parameters                                             *
C.    *       VOUT      Output co-ords,direction cosines,momentum      *
C.    *  User routine called                                           *
C.    *       CALL GUFLD(X,F) (GUFIELD with real*8 param.)             *
C.    *                                                                *
C.    *    ==>Called by : <USER>, GUSWIM                               *
C.    *       Authors    R.Brun, M.Hansroul  *********                 *
C.    *                  V.Perevoztchikov (CUT STEP implementation)    *
C.    *                                                                *
C.    *                                                                *
C.    ******************************************************************
C.
      IMPLICIT DOUBLE PRECISION(A-H,O-Z)
      DOUBLE PRECISION CHARGE, STEP, VECT(*), VOUT(*), F(4)
      DOUBLE PRECISION XYZT(3), XYZ(3), X, Y, Z, XT, YT, ZT
      DOUBLE PRECISION SECXS(4),SECYS(4),SECZS(4),HXP(3)
      EQUIVALENCE (X,XYZ(1)),(Y,XYZ(2)),(Z,XYZ(3)),
     +            (XT,XYZT(1)),(YT,XYZT(2)),(ZT,XYZT(3))
*
      PARAMETER (MAXIT = 1992, MAXCUT = 11)
      PARAMETER (EC=2.9979251D-4,DLT=1D-4,DLT32=DLT/32)
      PARAMETER (ZERO=0, ONE=1, TWO=2, THREE=3)
      PARAMETER (THIRD=ONE/THREE, HALF=ONE/TWO)
      PARAMETER (PISQUA=.986960440109D+01)
      PARAMETER      (IX=1,IY=2,IZ=3,IPX=4,IPY=5,IPZ=6)
*.
*.    ------------------------------------------------------------------
*.
*             This constant is for units CM,GEV/C and KGAUSS
*
      ITER = 0
      NCUT = 0
      DO 10 J=1,7
         VOUT(J)=VECT(J)
   10 CONTINUE
      PINV   = EC * CHARGE / VECT(7)
      TL = 0.
      H      = STEP
*
*
   20 REST  = STEP-TL
      IF (ABS(H).GT.ABS(REST)) H = REST
      CALL DUFLD(VOUT,F)
*
*             Start of integration
*
      X      = VOUT(1)
      Y      = VOUT(2)
      Z      = VOUT(3)
      A      = VOUT(4)
      B      = VOUT(5)
      C      = VOUT(6)
*
      H2     = HALF * H
      H4     = HALF * H2
      PH     = PINV * H
      PH2    = HALF * PH
      SECXS(1) = (B * F(3) - C * F(2)) * PH2
      SECYS(1) = (C * F(1) - A * F(3)) * PH2
      SECZS(1) = (A * F(2) - B * F(1)) * PH2
      ANG2 = (SECXS(1)**2 + SECYS(1)**2 + SECZS(1)**2)
      IF (ANG2.GT.PISQUA) GO TO 40
      DXT    = H2 * A + H4 * SECXS(1)
      DYT    = H2 * B + H4 * SECYS(1)
      DZT    = H2 * C + H4 * SECZS(1)
      XT     = X + DXT
      YT     = Y + DYT
      ZT     = Z + DZT
*
*              Second intermediate point
*
      EST = ABS(DXT)+ABS(DYT)+ABS(DZT)
      IF (EST.GT.H) GO TO 30

      CALL DUFLD(XYZT,F)
      AT     = A + SECXS(1)
      BT     = B + SECYS(1)
      CT     = C + SECZS(1)
*
      SECXS(2) = (BT * F(3) - CT * F(2)) * PH2
      SECYS(2) = (CT * F(1) - AT * F(3)) * PH2
      SECZS(2) = (AT * F(2) - BT * F(1)) * PH2
      AT     = A + SECXS(2)
      BT     = B + SECYS(2)
      CT     = C + SECZS(2)
      SECXS(3) = (BT * F(3) - CT * F(2)) * PH2
      SECYS(3) = (CT * F(1) - AT * F(3)) * PH2
      SECZS(3) = (AT * F(2) - BT * F(1)) * PH2
      DXT    = H * (A + SECXS(3))
      DYT    = H * (B + SECYS(3))
      DZT    = H * (C + SECZS(3))
      XT     = X + DXT
      YT     = Y + DYT
      ZT     = Z + DZT
      AT     = A + TWO*SECXS(3)
      BT     = B + TWO*SECYS(3)
      CT     = C + TWO*SECZS(3)
*
      EST = ABS(DXT)+ABS(DYT)+ABS(DZT)
      IF (EST.GT.2.*ABS(H)) GO TO 30

      CALL DUFLD(XYZT,F)
*
      Z      = Z + (C + (SECZS(1) + SECZS(2) + SECZS(3)) * THIRD) * H
      Y      = Y + (B + (SECYS(1) + SECYS(2) + SECYS(3)) * THIRD) * H
      X      = X + (A + (SECXS(1) + SECXS(2) + SECXS(3)) * THIRD) * H
*
      SECXS(4) = (BT*F(3) - CT*F(2))* PH2
      SECYS(4) = (CT*F(1) - AT*F(3))* PH2
      SECZS(4) = (AT*F(2) - BT*F(1))* PH2
      A      = A+(SECXS(1)+SECXS(4)+TWO * (SECXS(2)+SECXS(3))) * THIRD
      B      = B+(SECYS(1)+SECYS(4)+TWO * (SECYS(2)+SECYS(3))) * THIRD
      C      = C+(SECZS(1)+SECZS(4)+TWO * (SECZS(2)+SECZS(3))) * THIRD
*
      EST    = ABS(SECXS(1)+SECXS(4) - (SECXS(2)+SECXS(3)))
     ++        ABS(SECYS(1)+SECYS(4) - (SECYS(2)+SECYS(3)))
     ++        ABS(SECZS(1)+SECZS(4) - (SECZS(2)+SECZS(3)))
*
      IF (EST.GT.DLT .AND. ABS(H).GT.1.E-4) GO TO 30
      ITER = ITER + 1
      NCUT = 0
*               If too many iterations, go to HELIX
      IF (ITER.GT.MAXIT) GO TO 40
*
      TL = TL + H
      IF (EST.LT.(DLT32)) THEN
         H = H*TWO
      ENDIF
      CBA    = ONE/ SQRT(A*A + B*B + C*C)
      VOUT(1) = X
      VOUT(2) = Y
      VOUT(3) = Z
      VOUT(4) = CBA*A
      VOUT(5) = CBA*B
      VOUT(6) = CBA*C
      REST = STEP - TL
      IF (STEP.LT.0.) REST = -REST
      IF (REST .GT. 1.E-5*ABS(STEP)) GO TO 20
*
      GO TO 999
*
**              CUT STEP
   30 NCUT = NCUT + 1
*               If too many cuts , go to HELIX
      IF (NCUT.GT.MAXCUT)       GO TO 40
      H = H*HALF
      GO TO 20
*
**              ANGLE TOO BIG, USE HELIX
   40 F1  = F(1)
      F2  = F(2)
      F3  = F(3)
      F4  = SQRT(F1**2+F2**2+F3**2)
      RHO = -F4*PINV
      TET = RHO * STEP
      IF(TET.NE.0.) THEN
         HNORM = ONE/F4
         F1 = F1*HNORM
         F2 = F2*HNORM
         F3 = F3*HNORM
*
         HXP(1) = F2*VECT(IPZ) - F3*VECT(IPY)
         HXP(2) = F3*VECT(IPX) - F1*VECT(IPZ)
         HXP(3) = F1*VECT(IPY) - F2*VECT(IPX)

         HP = F1*VECT(IPX) + F2*VECT(IPY) + F3*VECT(IPZ)
*
         RHO1 = ONE/RHO
         SINT = SIN(TET)
         COST = TWO*SIN(HALF*TET)**2
*
         G1 = SINT*RHO1
         G2 = COST*RHO1
         G3 = (TET-SINT) * HP*RHO1
         G4 = -COST
         G5 = SINT
         G6 = COST * HP

         VOUT(IX) = VECT(IX) + (G1*VECT(IPX) + G2*HXP(1) + G3*F1)
         VOUT(IY) = VECT(IY) + (G1*VECT(IPY) + G2*HXP(2) + G3*F2)
         VOUT(IZ) = VECT(IZ) + (G1*VECT(IPZ) + G2*HXP(3) + G3*F3)

         VOUT(IPX) = VECT(IPX) + (G4*VECT(IPX) + G5*HXP(1) + G6*F1)
         VOUT(IPY) = VECT(IPY) + (G4*VECT(IPY) + G5*HXP(2) + G6*F2)
         VOUT(IPZ) = VECT(IPZ) + (G4*VECT(IPZ) + G5*HXP(3) + G6*F3)
*
      ELSE
         VOUT(IX) = VECT(IX) + STEP*VECT(IPX)
         VOUT(IY) = VECT(IY) + STEP*VECT(IPY)
         VOUT(IZ) = VECT(IZ) + STEP*VECT(IPZ)
*
      ENDIF
*
  999 END

*---------1---------2---------3---------4---------5---------6---------7--
      Subroutine DUFLD(xyz,Field)
      implicit none
      double precision xyz(3),Field(3)
      real   x(3),F(3)

      x(1)=xyz(1)
      x(2)=xyz(2)
      x(3)=xyz(3)
      call GUFLD(x,F)
      Field(1)=F(1)
      Field(2)=F(2)
      Field(3)=F(3)
      end
