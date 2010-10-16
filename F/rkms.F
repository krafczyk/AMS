*  $Id: rkms.F,v 1.20 2010/10/16 17:24:03 shaino Exp $
*-----------------------------------------------------------------------
* A.Chikanian, Yale, May-June 2010   (Revised version of 2003)
*-----------------------------------------------------------------------
* Rigidity reconstruction program for AMS02 with permanet magnet
* Taking care of the Multipole Scattering in AMS Tracking detector:
* Reconstruct track's kinematical parameters with full (nondiagonal)
* Covar.Error Metrix due to Multipole Scattering in Si tracking planes
* as well as in TRD, TOF and RICH.
*------------------------------------------------------
      subroutine rkmsinit(zposl)
      implicit none
#include "rkms.h"
*----------------
      integer k
      real zposl(NPma)
      integer n
      n=0
      do k=1,NPma
        if (trkZ0(k).ne.zposl(k)) then
           trkZ0(k)=zposl(k)
           n=n+1
        endif
      enddo
      if (n.gt.0) then
         write(*,*)'RKMS-I-InitTo ',trkZ0
      endif
      end


      subroutine rkmsrig(npo,npl,xyz,dxyz,ipa,Rini ,Out)
      integer npo,npl(npo),ipa
      real xyz(3,npo),dxyz(3,npo)
      real Rini,Out(9)
      call rkms_rig(npo,npl,xyz,dxyz,ipa,Rini ,Out)
      end


c. Add by Zhili.Weng 2009-11-11
#ifdef __AMSVMC__
#include "VMCrename.h"
#endif
*--------1---------2---------3---------4---------5---------6---------7--
      subroutine rkms_rig(npo,npl,xyz,dxyz,ipa,Rini ,Out)
*     interface to "rkms_fit"
*----------------------------------------------------------------
* Input:   npo        - # points
*          npl        - plane #, associated with each point
*          xyz(3,npl) - 3d coordinates
*          dxyz(npl)  - errors (intrinsic Si planes resolution)
*          ipa        - particle Geant ID#
*          Rini       - initial Rigidity value (try the best !!!)
*          --------------------------------------
* Output:  Out(*)     - reconstructed parameters
*          Out(1-3)   - x, y, z 
*          Out(4-5)   - Theta, Phi
*          Out(6)     - Rigidity
*          Out(7)     - Chi2
*          Out(8)     - not used
*          Out(9)     - err(1/Rigidity)=err(R)/(R*R)
*---------------------------------------------------
      implicit none
*----------------
#include "rkms.h"
*----------------
      double precision charge,sigcha ! for GRKUTA
      common/rktrk/    charge,sigcha
!$OMP threadprivate(/rktrk/)
*     ------------------------------
      integer npo,npl(npo),ipa,i
      real xyz(3,npo),dxyz(3,npo)
      real x(NPma),y(NPma),z(NPma),dx(NPma),dy(NPma)
      real Rini,Out(9),Chi2,R,dR,Rin
      double precision Xout(5),Eout(5)
*     --------------------------------
      do i=1,npo
         x(i)= xyz(1,i)
         y(i)= xyz(2,i)
         z(i)= xyz(3,i)
        dx(i)=dxyz(1,i)
        dy(i)=dxyz(2,i)
      enddo
*     WARNING ========================================================
*     Due to Error Matrix depends on Rini, pick the best possible Rini
*     OR do final (additional) minimization loop with Rini~Rrec
      Rin=Rini
      if(abs(Rini).lt.0.3.or.abs(Rini).gt.333.)
     +                                 call R_init(npo,x,y,z,Rin)
*     =========================================================
      call rkms_fit(npo,npl,x,y,z,dx,dy,ipa,Rin,Xout,Eout,Chi2)
*     =========================================================
      R = dsqrt(Xout(1)**2+Xout(2)**2+Xout(3)**2)
      dR=dsqrt((Xout(1)*Eout(1))**2 + (Xout(2)*Eout(2))**2
     +        +(Xout(3)*Eout(3))**2)/R
*     =============================================
      Out(1) = Xout(4)                            ! x
      Out(2) = Xout(5)                            ! y
      Out(3) = z(1)                               ! z
      Out(4) = acos(sngl(Xout(3))/R)              ! Theta
      Out(5) = atan2(sngl(Xout(2)),sngl(Xout(1))) ! Phi
      Out(6) = R*sigcha                           ! Rigidity*ChargeSign
      Out(7) = Chi2                               ! Chi2
      Out(8) = 0.                                 ! Not Used
      Out(9) = dR/(R*R)                           ! err(1/Rig)
*     =============================================
      end

*--------1---------2---------3---------4---------5---------6---------7--
      subroutine rkms_fit(npo,npl,x,y,z,dx,dy,ipa,Rini,Xout,Er,Chi2)
*   A.Chikanian, Yale, 2003
*   Fitting track parameters with MINUIT and Runge Kutta 
*-------------------------------------------------------
* Input:   npo   - # points
*          npl   - plane #, associated with each point
*          x(npl),y(npl),z(npl) - 3d coordinates
*          dx(npl),dy(npl) - errors
*          ipa   - particle Geant #
*          Rini  - initial Rigidity value
* Output:  Xout  -  x, y, Rx, Ry, Rz
*          Er    - dx,dy,dRx,dRy,dRz
*          Chi2  - Chi2
*-----------------------------------------
      implicit none !!! commenting helps with realy STUPID problem (see next).
c      real     FunMi !!! Required (but doesn't like it ???) by STUPID PAW. 
      External FunMi
      double precision Par(5),St(5),arglis(3),Xout(5),Er(5),bnd1,bnd2
      double precision                        Xbuf(5),Chi2B
*--------------------------------------------------------------------------
*               dPx     dPy    dPz(GeV) dx   dy(cm)  Initial MIGRAD steps 
      data St/.000100,.000100,.002000,.0005,.0005/   ! 8.98*min
* see:/group/yaug1/prj/shik/AMS/MYAMS/TRKRKRES/MSMATR/RKMSFIT/derivstep.opt
*--------------------------------------------------------------------------
      double precision zero
      data             zero/0.d0/
      double precision grad(5),A
      data             grad/5*0.D0/
      integer i,j,it,ivar,ierflg,na   ,Npa
      data                       na/0/,Npa/5/ ! # of parameters
      character*10 CHNAM
      data         CHNAM/' '/                 ! parameter names
      integer npo,npl(npo),NitMa
      data                 NitMa/3/
      integer ipa,iflag
      real x(npo),y(npo),z(npo),dx(npo),dy(npo)
      integer lun8
      data    lun8/67/
*----------------
#include "rkms.h"
*----------------
      double precision charge,sigcha ! for GRKUTA
      common/rktrk/    charge,sigcha
!$OMP threadprivate(/rktrk/)
*---------------------------
      real z1
      real Rini,Chi2,Rad
*     ------------------
      logical first
      data    first/.true./
      save first,CHNAM,zero,npa,na,St,grad
      !$OMP threadprivate (first,CHNAM,zero,npa,na,St,grad)
      logical veryfirst
      data    veryfirst/.true./
      integer nerr
      data nerr/0/
      save nerr
      if(veryfirst) then ! --------- veryfirst
        veryfirst = .false.
#if defined __IFC ||  defined __INTEL_COMPILER
        open(unit=lun8,form='formatted',file='/dev/null',
     +  status='unknown',shared)
#else
       open(unit=lun8,form='formatted',file='/dev/null',
     +  status='unknown')
#endif
      endif
      if(first) then ! ----------------- first
        first = .false.
 100    continue
        Call MNINIT(5,lun8,lun8) ! Read, Write, Save
        na = 1             ! # of arguments
        arglis(1)=-1.      ! suppr. output
        Call MNEXCM(FunMI,'SET PRINT',arglis,na,ierflg,0)
        If(ierflg.ne.0) write(6,*)'MNEXCM: Print ierflg=',ierflg
        arglis(1)= 1.
        Call MNEXCM(FunMI,'SET GRAD',arglis,na,ierflg,0) ! Gradients
        If(ierflg.ne.0) write(6,*)'MNEXCM: Grad ierflg=',ierflg
*  Errors calculation !!!### Not sure that it works !!!???
        arglis(1)=1.
        Call MNEXCM(FunMI,'SET ERRo',arglis,na,ierflg,0)
        If(ierflg.ne.0) write(6,*)'MNEXCM: ERRo ierflg=',ierflg
      endif          ! ----------------- end first

      if(npo.lt.3) return !!!
c      if(npo.ne.9) write(6,*)' npo =',npo ! 2010

      z1=z(1) ! if so Par(4)=x(1), Par(5)=y(1)
      rad = sqrt((x(2)-x(1))**2+(y(2)-y(1))**2+(z(2)-z(1))**2)

*--  refreshing hits information in common/rkms_1/...
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
* First TWO passes in loop below are sign of charge checking
* Third pass is iteration with right MS-matrix (mostly for correct Xi2)
      rkstep=5. ! cm
      DO it=1,NitMa
* Initial values
        if(it.eq.NitMa) rkstep=2. ! cm
        if(it.le.2) then
                      sigcha= 1.
          if(it.eq.2) sigcha=-1.
          Par(1)=Rini*(x(2)-x(1))/rad                     ! Px0
          Par(2)=Rini*(y(2)-y(1))/rad                     ! Py0
          Par(3)=Rini*(z(2)-z(1))/rad                     ! Pz0
          Par(4)=x(1) + (z1-z(1))/(z(2)-z(1))*(x(2)-x(1)) !  x0
          Par(5)=y(1) + (z1-z(1))/(z(2)-z(1))*(y(2)-y(1)) !  y0
        else
          do j=1,Npa
            Par(j)=Xout(j)
          enddo
        endif
*       =======================================
        call wxy_matr9(npo,npl,dx,dy,ipa,Rini) ! Full Matr.
c        call w1_matr(npo,dx,dy)                ! Diagon.Matr
*       =======================================
#if defined (__LINUX24__ )
!$OMP CRITICAL (RKMS)
#endif
*       --------------------- Initial values setup
        do i=1,Npa
          Call MNPARM(i,CHNAM,Par(i),St(i),zero,zero,IERFLG)! Init.Val.Setup
          If(ierflg.ne.0) then
             if (nerr.lt.100) write(6,*)'### MNPARM: i,ierflg=',i,ierflg
             nerr = nerr+1
          endif
        enddo
*       ---------------------------------------- Minimization  
        na=0  ! # of arguments in arglis (MNEXCM)      !
        Call MNEXCM(FunMI,'MIGRAD',arglis,na,ierflg,0) !
*       ####  ierflg is always 4 ### !!!???
*       ================================================
        do i=1,Npa                                          ! Parameters
          Call MNPOUT(i,CHNAM,Xout(i),Er(i),bnd1,bnd2,ivar) ! Extraction
        enddo
#if defined (__LINUX24__ )
!$OMP END CRITICAL (RKMS)
#endif
        Chi2=1.e30
        if(Dhi2.lt.1.e30) Chi2=Dhi2
        if(it.eq.1) then
          Chi2B=Chi2
          do i=1,Npa
            Xbuf(i)=Xout(i)
          enddo
        endif
        if(it.eq.2.and.Chi2.gt.Chi2B) then
          sigcha=1.
          do i=1,Npa
            Xout(i)=Xbuf(i)
          enddo
        endif
      ENDDO ! NitMa 
      end

*--------1---------2---------3---------4---------5---------6---------7-- FCN
      Subroutine FunMI(Npa,grad,Fu,par,iflag) ! ,futil)
*   A.Chikanian, Yale, Aug,2003
*   Interface to between MINUIT and Fun
*   -----------------------------------
      implicit none
      integer Npa,iflag,k
      double precision Fu,Fun,grad(npa),par(npa)
*     ------------------------------------------
      if(iflag.eq.2) call FunDer(Npa,par,grad)
      Fu = Fun(Npa,par)
      end

*--------1---------2---------3---------4---------5---------6---------7--
      double precision Function Fun(Npa,par)
*   A.Chikanian, Yale, Aug,2003
*   Function for Minimization
*------------------------------
      implicit none
      integer Npa,i
      double precision chi2su,par(Npa)
*----------------
#include "rkms.h"
*----------------
      double precision pin(3),xin(3),xt(NPma),yt(NPma)

      pin(1) = par(1) ! Px or cx
      pin(2) = par(2) ! Py or cy
      pin(3) = par(3) ! Pz or R
      xin(1) = par(4) ! x
      xin(2) = par(5) ! y
      xin(3) = zc(1)  ! z
      do i=1,Npma
        xt(i)=-999.
        yt(i)=-999.
      enddo
*     ==========================
      call rk_trk(pin,xin,xt,yt)
      Fun = chi2su(xt,yt)
*     ==========================
      end

*--------1---------2---------3---------4---------5---------6---------7--
      subroutine FunDer(npa,par,der)
*   A.Chikanian, Yale, Aug,2003
*---   numerical calculation of Fun and Fun's derivatives.
*--------------------------------------------------------
      implicit none
      integer npa,j
      double precision f1,f2,x0,dp,Fun,par(npa),der(npa)
*==================================================
      double precision dp0(5),xx(5)                !
      data dp0/.000005,.000005,.000100,.0002,.0002/! Optimal Steps
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
      end

*--------1---------2---------3---------4---------5---------6---------7--
      double precision function Chi2Su(x0,y0)
*-------------------------------------------
*   A.Chikanian, Yale, Feb,2003
*             Revised June,2010
*-------------------------------------------------
* Input    x0     - x-coord. for current iteration
*          y0     - y-coord. for current iteration
* Output:  Chi2Su - chi2 value
*-------------------------------------------------
      implicit none
      integer i,j
      double precision xi,yi
*----------------
#include "rkms.h"
*----------------
*  xc,yc - hits coordinates
      double precision x0(NPma),y0(NPma) ! r8
      Chi2Su=wx(1,1)*(xc(1)-x0(1))**2+wy(1,1)*(yc(1)-y0(1))**2
      do i=2,npoc
        xi=xc(i)-x0(i)
        yi=yc(i)-y0(i)
        Chi2Su = Chi2Su + wx(i,i)*xi**2 + wy(i,i)*yi**2
        if(i.lt.npoc) then
          do j=i+1,npoc
            Chi2Su=Chi2Su+2.*(wx(j,i)*xi*(xc(j)-x0(j))
     +                       +wy(j,i)*yi*(yc(j)-y0(j)))
          enddo
        endif
      enddo
      Dhi2=Chi2Su
      end

*--------1---------2---------3---------4---------5---------6---------7--
      subroutine rk_trk(pin,xin,x0,y0)
*---------------------------------------------------
* Runge-Kutta integration for particle in Magn.field
* A.Chikanian, Yale, Feb.,2003
*            Revised June,2010
*-------------------------------------------------------------------
*  Input:  pin(3)   - initial Rigidity Rx,Ry,Rz (GeV/c) for tracking
*          xin(3)   - initial Position  x, y, z (cm)    for tracking
*  Output: x0(NPma) - x,y (cm) coordinates of tragectory crossing
*          y0(NPma) /          with tracking planes
*--------------------------------------------------
      implicit none
      integer i,j,Nst,np
*----------------
#include "rkms.h"
*----------------
      double precision charge,sigcha ! for GRKUTA
      common/rktrk/    charge,sigcha
!$OMP threadprivate(/rktrk/)
*---------------------------
      double precision xm,pin(3),xin(3),x0(NPma),y0(NPma)
      double precision step,vect(7),vout(7),Stot,vni
*                           vect(7) is  x,y,z,px,py,pz,P
*                                   or  x,y,z,cx,cy,cz,P
*                                       1 2 3  4  5  6 7
      data Stot/350./ ! Track's Max.Path Length (cm) (2010)
*     -----------------------------------------------------
      logical first
      data    first/.true./
      double precision w
*     -----------------------------------------------------
      step=rkstep
      Nst=int(Stot/step) ! Max.# of steps
      vni=1.d0
      if(zc(1).lt.zc(2)) vni=-1.d0 ! back tracking
cxy3 ---
      vect(7)=dsqrt(pin(1)**2+pin(2)**2+pin(3)**2)
      do i=1,3
        vect(i  )=xin(i)         !  xyz
        vect(i+3)=pin(i)/vect(7) ! cxyz
      enddo
*     --------------------------- Runge Kutta
      np = 1
      do i=1,Nst
        call DRKUTA(SigCha,STEP,VECT,VOUT)    ! r8
          DO j=1,2 !!!! fixed close planes problem(AMS02)
        if(vni*vect(3).ge.vni*zc(np).and.vni*vout(3).lt.vni*zc(np))then
          w=(zc(np)-vout(3))/(vect(3)-vout(3))
          x0(np) = vect(1)*w + vout(1)*(1.d0-w)
          y0(np) = vect(2)*w + vout(2)*(1.d0-w)
          if(np.eq.npoc) return  ! Done
          np = np+1
        endif
          ENDDO  !### fixing closed planes problem.(small tracking STEP)
        do j=1,7 ! x,y,z,cx,cy,cz,P
          vect(j)=vout(j)
        enddo
      enddo ! Nst 
      end

*--------1---------2---------3---------4---------5---------6---------7--
      subroutine wxy_matr9(npo,npl,dx,dy,ipa,R)   !
*-------------------------------------------------
* Preparing Cov.Err.Matrix with Mult.Scattering
* A.Chikanian Feb.,2003.
*     Revised June,2010
*--------------------------------------------------------
* Missing planes carring by putting dX,dY --> INF.(~22cm)
* COS(Th) - is INDIV. for each planes
* Symmetry of all matrix assumed
*========================================================
* INP: npo      - # of x,y - hits
*      npl(npo) - array with plane # associated with hits
*    dx,dy(npo) - errors of x,y
*          ipa  - particle ID (Geant)
*            R  - Rigidity (gess)
*========================================================
      implicit none
*----------------
#include "rkms.h"
*----------------
      double precision charge,sigcha ! for GRKUTA
      common/rktrk/    charge,sigcha
!$OMP threadprivate(/rktrk/)
*     -------------------------------
      real dZ(NPma,NPma)
      real wzn(Npma),wzi(Npma)
      integer ipa
      real R,RR,Rmin
      data      Rmin/0.300/ ! GeV

      integer npo,i,j,k,ii,jj,is,ier,NMSpl,np1
      integer ir(10),ifail,jfat
      integer npl(NPma),lw(NPma),mw(NPma)
      real dx(npo),dy(npo)
      real dms(NPma,NPma),vx(NPma,NPma),vy(NPma,NPma)
      real dTh0,dTc2,oco(NPma),E,be,P
      integer itr,nw
      real ama,cha,tl,ub
      character*20 pname
*     -----------------------
      logical first
      data    first/.true./
      save    first,dz,dms,NMSpl
      !$OMP threadprivate (first,dz,dms,NMSpl)
*     ----------------------------------------
      if(first) then
        first=.false.
        do i=1,NPma-1  ! -------  dZ
          dz(i,i)=0.
          do j=i+1,NPma
            dz(j,i)=trkZ0(j)-trkZ0(i)
            dz(i,j)=dz(j,i)
          enddo
        enddo
        dz(NPma,NPma)=0.
      endif ! -------------------- first
*     =================  # of planes > 3 checking 
      if(npo.lt.3) then
        write(6,*)'### wxy_matr: # of planes',npo
        return
      endif
*     ===================== Initialization to 0
      do i=1,NPma*NPma
        dms(i,1)=0.
        vx (i,1)=0.
        vy (i,1)=0.
        wx (i,1)=0.
        wy (i,1)=0.
        if(i.le.NPma) oco(i)=0.
      enddo
*     ============================ MS-matrix building
      np1=npl(1)
      if(npl(1).gt.npl(npo)) then ! Down --> Up
        call matr_scatN1(np1,dms) ! Down --> Up
      else                        !
        call matr_scatN9(np1,dms) ! Up --> Down
      endif
*     ============================
      NMSpl = iabs(npl(npo)-npl(1))+1 ! # Mult.Scatt.planes
      call gfpart(ipa,pname,itr,ama,cha,tl,ub,nw)
      RR=max(R,Rmin)
      P  =  RR*cha
      E  =  sqrt(P**2+ama**2)
      be =  P/E ! or from TOF
      dTc2 = 1./(be*RR)**2 ! scatt.angle correction
      charge = cha
c      sigcha = 1.D0
c      if(cha.lt.0.) sigcha = -1.D0 ! for GRKUTA 
*     ------------------------------  Full matrix forming
      vx(1,1) = dx(1)**2
      vy(1,1) = dy(1)**2
      ii=1
      do i=2,npo ! Diagonal elements
        if(iabs(npl(i)-npl(i-1)).gt.1) then ! Missing plane
          do is=1,iabs(npl(i)-npl(i-1))-1
            ii=ii+1
            oco(ii)=dsqrt(1.+((xc(i)-xc(i-1))**2+(yc(i)-yc(i-1))**2)
     +                                      /dz(npl(i),npl(i-1))**2)
c     +                         /(trkZ0(npl(i))-trkZ0(npl(i-1)))**2)
            vx(ii,ii)=dms(ii,ii)*dTc2 + 1000. ! dX,Y~22cm if missing plane
            vy(ii,ii)= vx(ii,ii)
          enddo
        endif
        ii=ii+1
        oco(ii)=dsqrt(1.+((xc(i)-xc(i-1))**2+(yc(i)-yc(i-1))**2)
     +                                  /dz(npl(i),npl(i-1))**2)
c     +                     /(trkZ0(npl(i))-trkZ0(npl(i-1)))**2)
        vx(ii,ii)=dms(ii,ii)*dTc2*oco(ii) + dx(i)**2
        vy(ii,ii)=dms(ii,ii)*dTc2*oco(ii) + dy(i)**2
      enddo
      do i=3,NMSpl !  Nondiagonal elements
        do j=2,i-1
          vx(i,j)=dms(i,j)*dTc2*sqrt(oco(i)*oco(j))
          vy(i,j)= vx(i,j)
        enddo
      enddo
*     -------------------------------- Inversion
      call rsinv(NMSpl,vx,NPma,ifail) ! F012 Symm.Matr.Inversion
      if(ifail.ne.0) write(6,*)'### rsinv x: ifail=',ifail
      call rsinv(NMSpl,vy,NPma,ifail) ! F012 Symm.Matr.Inversion
      if(ifail.ne.0) write(6,*)'### rsinv y: ifail=',ifail
*     --------------------- Matrix Shrinking
      wx(1,1)=vx(1,1)
      wy(1,1)=vy(1,1)
      do j=2,npo
        jj=iabs(npl(j)-npl(1))+1
        do i=j,npo
          ii=iabs(npl(i)-npl(1))+1
          wx(i,j)=vx(ii,jj)
          wy(i,j)=vy(ii,jj)
        enddo
      enddo
      end

*---------1---------2---------3---------4---------5---------6---------7--
      subroutine matr_scatN9(npl1,dms)
* Cov.Err.Matrix with Mult.Scattering for Up --> Down track
* A.Chikanian May,2010.
*---------------------------------
* Input:  npl1 - first plane
* Output: dms  - scattering matrix
*---------------------------------
      implicit none
*----------------
#include "rkms.h"
*----------------
      real   dms(NPma,NPma)   ! Output MS matrix 
*     ------------------------
      real dmsN9(NPma,NPma,7) ! internal array
      save dmsN9              ! keep it save
*     ------------------------
      integer npl,npl1,N,i,j,k,jj,ii
      real sq3
      parameter (sq3=1.73205078)
*  --------------------------------------------------------- Geometry
*  L8(1N) TRD   TOF  HC2 L1 L2 L3 L4 L5 L6 L7  TOF  RICH  L9
*    z1  p1-p2 p3-p4  p5 z2 z3-z4 z5-z6 z7-z8 p6-p7 p8-p9 z9
*  ---------------------------------------------------------
      real       p(9)/ 156.15, 78.95  !  TRD  p1,p2
     +               ,  65.5 , 61.0   !  TOF  p3,p4
     +               ,  56.46         !  HC2  p5
     +               , -61.0 ,-65.5   !  TOF2 p6,p7
     +               , -74.5 ,-75.0/  !  RICH p8,p9
      real       z(9)/ 166.92         !  z1   PL8 (1N)
     +               ,  53.06         !  z2   PL1
     +               ,  29.2          !  z3   PL2
     +               ,  27.5          !  z4   PL3
     +               ,   1.7          !  z5   PL4
     +               ,  -1.7          !  z6   PL5
     +               , -27.5          !  z7   PL6
     +               , -29.2          !  z8   PL7
     +               ,-135.48/        !  z9   PL9
*   -----------------------------------------------------
*   X_Si = X_HC = 0.32054%
*   X0_Rad = X_pl*den_pl/den_read=42.4*1.032/0.06=729cm
*   X_TRD  = X_HC+X_rad+X_gaz=X_HC = 1.+ 6.+ 0.64 = 7.64%
*   X_TOF  = 2cm/42.4cm =0.047 = 4.7%  (2cm of scintilator)
*   X_RICH = 0.5cm/14.91cm = 0.0335 = 3.35%  (0.5cm of NaF)
*   ----------------------------------------------------------------
*                       Si      TRD    TOF   Si or HC     TOF   RICH
      real  xRL(16)/0.0032054,0.0764,0.047,11*0.0032054,0.047,0.0335/
      real   DT(16)/16*0./
      real dz(9,16)/144*0./
      integer IS(9)/0,4,5,7,8,10,11,13,16/ ! right
      real beta/1./,rigid/1./ ! 1 GeV/c
      logical first/.true./
       save first,z,dz,dt
      !$OMP threadprivate (first,z,dz,DT,dmsN9)
*     ----------------------------------------
      if(first) then ! calculating and storring dmsN9 array
        do k=1,NPma
          z(k)=trkZ0(k)
        enddo
        do k=1,7*NPma*NPma
          dmsN9(k,1,1)=0.
        enddo
*       --------------- scattering angles
        do i=1,16
          if(xRL(i).gt.0.0001)
     +     DT(i)=xRL(i)*(0.0136/(beta*rigid)*(1.+0.038*alog(xRL(i))))**2.
        enddo
*       ---------------  dz matrix
        do i=2,9
          dz(i,1)= z(i)-z(1)
          dz(i,2)=(p(2)-p(1))/sq3+z(i)-p(2)
          dz(i,3)=(p(4)-p(3))/sq3+z(i)-p(4)  !*
          dz(i,4)=(z(2)-p(5))/sq3            !*
          if(i.ge.3) then
            dz(i,5)= z(i)-z(2)
            if(i.ge.4) then
              dz(i,6)= z(i)-z(3)
              dz(i,7)=(z(4)-z(3))/sq3
              if(i.ge.5) then
                dz(i,8)= z(i)-z(4)
                if(i.ge.6) then
                  dz(i, 9)= z(i)-z(5)
                  dz(i,10)=(z(6)-z(5))/sq3
                  if(i.ge.7) then
                    dz(i,11)= z(i)-z(6)
                    if(i.ge.8) then
                      dz(i,12)= z(i)-z(7)
                      dz(i,13)=(z(8)-z(7))/sq3
                      if(i.eq.9) then
                        dz(i,14)= z(i)-z(8)
                        dz(i,15)=(p(7)-p(6))/sq3+z(i)-p(7)
                        dz(i,16)=(p(9)-p(8))/sq3+z(i)-p(9)
                      endif
                    endif
                  endif
                endif
              endif
            endif
          endif
        enddo
*       -------------------- dmsN9 filling
        do N=1,7
          do i=N+1,NPma ! toge
            ii=i-N+1
            do j=i,NPma
              jj=j-N+1
              dmsN9(ii,jj,N)=0.
              do k=IS(N)+1,IS(i)
                dmsN9(ii,jj,N)=dmsN9(ii,jj,N)+dz(i,k)*dz(j,k)*DT(k)
              enddo
              if(ii .ne. jj)   dmsN9(jj,ii,N)=dmsN9(ii,jj,N)
            enddo
          enddo
        enddo
        write(6,*)'---- matr_scatN9 initialisation completed ----'
        first=.false.
      endif  !  -----------   end first
*     ---------------------------------
      if(npl1.lt.1.or.npl1.gt.7) return
      do k=1,NPma*NPma
        dms(k,1)=0.
      enddo
      npl=Npma-npl1+1
      do i = 1,npl ! 2
        do j=1,npl ! 2
          dms(i,j)=dmsN9(i,j,npl1)
        enddo
      enddo
      end

*---------1---------2---------3---------4---------5---------6---------7--
      subroutine matr_scatN1(npl1,dms)
* Cov.Err.Matrix with Mult.Scattering for Down -->Up track
* A.Chikanian June,2010.
*---------------------------------
* Input:  npl1 - first plane
* Output: dms  - scattering matrix
*---------------------------------
      implicit none
*----------------
#include "rkms.h"
*----------------
      real   dms(NPma,NPma)   ! Output MS matrix 
*     ------------------------
      real dmsN1(NPma,NPma,7) ! internal array
      save dmsN1              ! keep it save
*     ------------------------
      integer npl,npl1,N,i,j,k,jj,ii
      real sq3
      parameter (sq3=1.73205078)
*  --------------------------------------------------------- Geometry
*  L8(1N) TRD   TOF  HC2 L1 L2 L3 L4 L5 L6 L7  TOF  RICH  L9
*    z1  p1-p2 p3-p4  p5 z2 z3-z4 z5-z6 z7-z8 p6-p7 p8-p9 z9
*  ---------------------------------------------------------
      real       p(9)/ 156.15, 78.95  !  TRD  p1,p2
     +               ,  65.5 , 61.0   !  TOF  p3,p4
     +               ,  56.46         !  HC2  p5
     +               , -61.0 ,-65.5   !  TOF2 p6,p7
     +               , -74.5 ,-75.0/  !  RICH p8,p9
      real       z(9)/ 166.92         !  z1   PL8 (1N)
     +               ,  53.06         !  z2   PL1
     +               ,  29.2          !  z3   PL2
     +               ,  27.5          !  z4   PL3
     +               ,   1.7          !  z5   PL4
     +               ,  -1.7          !  z6   PL5
     +               , -27.5          !  z7   PL6
     +               , -29.2          !  z8   PL7
     +               ,-135.48/        !  z9   PL9
*   -----------------------------------------------------
*   X_Si = X_HC = 0.32054%
*   X0_Rad = X_pl*den_pl/den_read=42.4*1.032/0.06=729cm
*   X_TRD  = X_HC+X_rad+X_gaz=X_HC = 1.+ 6.+ 0.64 = 7.64%
*   X_TOF  = 2cm/42.4cm =0.047 = 4.7%  (2cm of scintilator)
*   X_RICH = 0.5cm/14.91cm = 0.0335 = 3.35%  (0.5cm of NaF)
*   ----------------------------------------------------------------
*                       Si      RICH   TOF   Si or HC     TOF    TRD
      real  xRL(16)/0.0032054,0.0335,0.047,11*0.0032054,0.047,0.0764/
      real   DT(16)/16*0./
      real dz(9,16)/144*0./
      integer IS(9)/0,3,5,6,8,9,11,12,16/
      real beta/1./,rigid/1./ ! 1 GeV/c
      logical first/.true./

       save first,z,dz,dt
      !$OMP threadprivate (first,z,dz,DT,dmsN1)
*     -----------------------------------------
      if(first) then ! calculating and storring dmsN9 array
        do k=1,NPma
          z(k)=trkZ0(k)
        enddo
        do k=1,7*NPma*NPma
          dmsN1(k,1,1)=0.
        enddo
*       --------------- scattering angles
        do i=1,16
          if(xRL(i).gt.0.0001)
     +     DT(i)=xRL(i)*(0.0136/(beta*rigid)*(1.+0.038*alog(xRL(i))))**2
        enddo
*       ---------------  dz(9,16) matrix
        do i=2,9
          j=9-i+1
          dz(i,1)= z(j)-z(9)
          dz(i,2)=(p(8)-p(9))/sq3+z(j)-p(8)
          dz(i,3)=(p(6)-p(7))/sq3+z(j)-p(6)
          if(i.ge.3) then
            dz(i,4)= z(j)-z(8)
            dz(i,5)=(z(7)-z(8))/sq3
            if(i.ge.4) then
              dz(i,6)= z(j)-z(7)
              if(i.ge.5) then
                dz(i,7)= z(j)-z(6)
                dz(i,8)=(z(5)-z(6))/sq3
                if(i.ge.6) then
                  dz(i,9)= z(j)-z(5)
                  if(i.ge.7) then
                    dz(i,10)= z(j)-z(4)
                    dz(i,11)=(z(3)-z(4))/sq3
                    if(i.ge.8) then
                      dz(i,12)= z(j)-z(3)
                      if(i.eq.9) then
                        dz(i,13)= z(1)-z(2)
                        dz(i,14)=(p(5)-z(2))/sq3+z(1)-p(5)
                        dz(i,15)=(p(3)-p(4))/sq3+z(1)-p(3)
                        dz(i,16)=(p(1)-p(2))/sq3+z(1)-p(1)
                      endif
                    endif
                  endif
                endif
              endif
            endif
          endif
        enddo
*       -------------------- dmsN1 filling
        do N=1,7
          do i=N+1,NPma
            ii=i-N+1
            do j=i,NPma
              jj=j-N+1
              dmsN1(ii,jj,N)=0.
              do k=IS(N)+1,IS(i)
                dmsN1(ii,jj,N)=dmsN1(ii,jj,N)+dz(i,k)*dz(j,k)*DT(k)
              enddo
              if(ii .ne. jj)   dmsN1(jj,ii,N)=dmsN1(ii,jj,N)
            enddo
          enddo
        enddo
        write(6,*)'---- matr_scatN1 initialisation completed ----'
        first=.false.
      endif  !  -----------   end first
*     ---------------------------------
      if(npl1.gt.9.or.npl1.lt.3) return
      do k=1,NPma*NPma
        dms(k,1)=0.
      enddo
      npl=npl1
      do i = 1,npl ! 2
        do j=1,npl ! 2
          dms(i,j)=dmsN1(i,j,NPma-npl1+1)
        enddo
      enddo
      end

*============================ AMS/MYAMS/TRKRKRES/MSMATR/2010/rkmslib.F
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

*--------1---------2---------3---------4---------5---------6---------7--
      subroutine R_init(npo,x,y,z,Rc)
*--- calculate Initial Rigidity by circular fit
*----------------------------------------------
* Input:   npo - # points
*          x(npl),y(npl),z(npl) - 3d coordinates
* Output:  Rc  - reconstructed Rigidity by fiting Circle
*-------------------------------------------------------
      implicit none
      integer npo
      integer cfit,utcfit
      real x(npo),y(npo),z(npo)
*-------------------------------------------
#include "rkms.h"
*-------------------------------------
      real gauss,Fu,Bx
      external   Fu
      real Chi2,Rc,sgn,Rad,Rx,Ry,w1(NPma)
      data                       w1/NPma*1./
      real Sl,dSl,AA,dAA,Cor,Ch2L  ! for linear fit

      logical first
      data    first/.true./
      save first,w1,Bx
      !OMP threadprivate(first,w1,bx)
      if(first) then
        first = .false.
        Bx=gauss(Fu,trkz0(1),trkZ0(NPma),1.e-3)/(trkZ0(NPma)-trkZ0(1))! <Bx>
        write(6,*)'=== R_init: < Bx > =',Bx
      endif ! first

      Rc=10000000.
      if(Bx.eq.0.or.npo.lt.3) return                            !!!!
      cfit = utcfit(z,y,w1,npo,sgn,Rad,Rx,Ry,Chi2)   ! CIRCLE fit
c      cfit = utcfit(z,y,w1,min(npo,4),sgn,Rad,Rx,Ry,Chi2)   ! CIRCLE fit
      if(cfit.ne.0) write(6,*)'### utcfit: cfit.ne.0 !!!'
      Rc = 0.0003*Bx*Rad ! *sgn                      ! ragidity (GeV)
      call LinFit(z,x,w1,npo,Sl,dSl,AA,dAA,Cor,Ch2L) ! Liniar fit
      Rc = abs(Rc)*sqrt(Sl**2+1.)                    ! in space
      end

*--------1---------2---------3---------4---------5---------6---------7--
      Real function Fu(z) ! For integration Mag.Field by Gauss
*     --------------------------------------------------------
      Implicit none
      real x,y,z
      data x/0./,y/0./
      real xyz(3)   ,fld(3)
      data xyz/3*0./,fld/3*0./
      save xyz,fld
      !$OMP threadprivate(xyz,fld)
      xyz(3)=z
      call Gufld(xyz,fld)
      Fu = fld(1)
      end

*=======================================================================
      Subroutine LinFit(X,Y,SigmaY,Npts,B,SigmaB,A,SigmaA,R,Chi2)
*                       <--- input ---> <------- output ------->
*     A.Chikanian (Yale University)
* - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
*     The least-squares fit to a line ( y = A + B*x ) through points X,Y
* - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
*      The input & output are REAL*4 variables. 
*      This version handle the high precision requirements.
*      All internal calculations used the REAL*8 variables only.
* - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
*   Input:  X, Y   -  1d arrays with points coordinates
*           SigmaY -  1d array with Y errors
*           Npts   -  # of points
*   Output: A      -  intercept
*           SigmaA -  error on intercept
*           B      -  slope
*           SigmaB -  error on slope
*           R      -  correlation coefficient <A,B>/SigmaA/SigmaB
*           Chi2   -  chi square
*        if Chi2 = -2.  number of points with SIGMAY>0. less then 2
*                  -4.  singularity
* ----------------------------------------------------------------------
      Implicit NONE

* input variables
      Integer          Npts
      Real*4           X(Npts),Y(Npts),SigmaY(Npts)
* output variables
      Real*4           A,SigmaA,B,SigmaB,R,Chi2
* internal variables
      Real*8           Sum,SumX,SumY,SumX2,SumXY,Weight,Det
      Integer          Np,I

      Sum   = 0.
      SumX  = 0.
      SumY  = 0.
      SumX2 = 0.
      SumXY = 0.

      Np = 0
      Do I=1,Npts
        If(SigmaY(I).gt.1.E-15) Then
          Np = Np + 1
          Weight = 1./SigmaY(I)**2
          Sum   = Sum   + Weight
          SumX  = SumX  + Weight*X(I)
          SumY  = SumY  + Weight*Y(I)
          SumX2 = SumX2 + Weight*X(I)**2
          SumXY = SumXY + Weight*X(I)*Y(I)
        EndIf
      EndDo

      Chi2 = -2.                       !
      If(Np.le.1) Return               ! not enough points

      Det=Sum*SumX2-SumX**2
      Chi2 = -4.                       !
      if(Det.le.0.) Return             ! singularity checking

      B=(SumXY*Sum -SumY *SumX)/Det    ! Slope
      A=(SumY*SumX2-SumXY*SumX)/Det    ! Intercept
      SigmaB= dsqrt(Sum/Det)           ! Slope Error
      SigmaA= dsqrt(SumX2/Det)         ! Intercept Error
      R     = -SumX/Det/SigmaB/SigmaA  ! correlation coeff.

      Chi2=0.
      do i=1,Npts
        if(SigmaY(i).gt.1.e-15)
     +     Chi2=Chi2+((A+B*X(i)-Y(i))/SigmaY(i))**2
      enddo

      END

*---  /group/yaug1/prj/shik/MYLIB/GENERAL/circle_fit.f
*===========================================================================
* shik: SGN is not realyable !!! (Why?, see next line)
*       ASSUMED ORIGIN OF TRACK in the (0,0), if so no problem with SIG !!!!
*       But still: WHAT DOES mean SGN in case of full circle ?????

* it is probably not right in case dX not eq. dY
* for example Xav = Sum(Wx(i)*X(i))/Sum(Wx(i)),
*     but not Xav = Sum( W(i)*X(i))/Sum( W(i)) !!!!!!!!!!!!!!! 
* unique w(i) is only possible in case dX(i)=dY(i)
*===========================================================================


* $Id: rkms.F,v 1.20 2010/10/16 17:24:03 shaino Exp $
* $Log: rkms.F,v $
* Revision 1.20  2010/10/16 17:24:03  shaino
* Error message suppression
*
* Revision 1.19  2010/10/16 07:27:05  shaino
* Compatibility between gcc3 and gcc4 (conversion of fortran function with _)
*
* Revision 1.18  2010/10/16 07:17:07  shaino
* PGTRACK: Chikanian fit updated
*
* Revision 1.17  2010/07/12 15:35:56  choutko
* *** empty log message ***
*
* Revision 1.16  2010/07/12 14:12:17  choutko
* *** empty log message ***
*
* Revision 1.15  2010/06/24 11:13:35  zweng
* Add #include "VMCrename.h"
*
* Revision 1.14  2010/03/24 10:19:03  choutko
* make rkms work with perm meg opt
*
* Revision 1.13  2010/03/22 22:18:05  pzuccon
* keep compatibility with SLC4
*
* Revision 1.12  2010/03/21 15:16:38  choutko
* add permanent magnet tracker option (9 layers) via "AMS02P" setup
*
* ----------------------------------------------------------------------
*
* Revision 1.11  2010/02/17 16:12:37  choutko
* parallazing rkms
*
* Revision 1.10  2009/11/18 17:01:29  choutko
* fpe
*
* Revision 1.9  2008/12/08 15:15:18  choutko
* *** empty log message ***
*
* Revision 1.8  2008/04/04 08:49:38  choutko
* *** empty log message ***
*
* Revision 1.7  2008/03/10 22:07:14  choutko
* *** empty log message ***
*
* Revision 1.6  2008/02/28 16:14:54  choutko
* *** empty log message ***
*
* Revision 1.5  2006/02/16 13:21:28  choutko
* work around g77 shared
*
* Revision 1.4  2005/11/17 14:29:32  choutko
* correct /dev/null statement
*
* Revision 1.3  2005/09/22 09:09:15  choutko
* :
*
* Revision 1.2  2003/10/30 09:35:02  choutko
* yale algo ++ ( start working)
*
* Revision 1.2  1998/01/27 00:35:54  fisyak
* Split sources
*
      INTEGER FUNCTION UTCFIT(XCIR,YCIR,WCIR,NCIR,
     >                           SGN,R0,ACENT,BCENT,CHISQ)
*>-----------------------------------------------------------------

* UTCFIT - circle fit

* DESCRIPTION:
* Subroutine that fits circle parameters using algorithm
* described by Chernov and Oskov in Computer Physics
* Communications.

* INPUT ARGUMENTS:
* OUTPUT ARGUMENTS:
* AUTHOR:
*<----------------------------------------------------------------
*
* Modified: 13-NOV-1997 by R.Bossingham
*           Eliminate unused variables.

        IMPLICIT DOUBLE PRECISION (A-H , O-Z)
        INTEGER ITRY, NTRY, NCIR, IERR

        REAL    SGN,R0,ACENT,BCENT,CHISQ
        REAL    XCIR(*),YCIR(*),WCIR(*)

        DOUBLE PRECISION  LAMDA,KAPPA,KAPPA1
        COMMON /CIRCOM/ XAV   ,YAV   ,WSUM  ,RSCALE,COSROT,SINROT,
     +                XXAV  ,XYAV  ,YYAV  ,XRRAV ,YRRAV ,RRRRAV,
     +                ALPHA ,BETA  ,KAPPA

        !$OMP threadprivate(/CIRCOM/)
        DATA DBL1 / 1.0D0/,DBL2 / 2.0D0/, DBL4/ 4.0D0/, DBL64/6.4D1/
        DATA NTRY / 5/
        Save DBL1,DBL2,DBL4,DBL64,NTRY ! Ch
        !$OMP threadprivate(DBL1,DBL2,DBL4,DBL64,NTRY)
*-->  FIRST CALCULATE THE H MATRIX AND OTHER THINGS

        UTCFIT = 0
        IERR = 0

        CALL UTCMAT(XCIR,YCIR,WCIR,NCIR)

*-->  USE THESE TO GET THE COEFFICIENTS OF THE 4-TH ORDER POLYNIMIAL
*-->  DON'T PANIC - THE THIRD ORDER TERM IS ZERO !

        XRRXRR= XRRAV  * XRRAV
        YRRYRR= YRRAV  * YRRAV
        RRRRM1= RRRRAV - DBL1
        XXYY  = XXAV   * YYAV

        C0  =          RRRRM1*XXYY - XRRXRR*YYAV - YRRYRR*XXAV
        C1  =        - RRRRM1      + XRRXRR      + YRRYRR   - DBL4*XXYY
        C2  =   DBL4 + RRRRM1                               - DBL4*XXYY
        C4  = - DBL4

*-->  COEFFICIENTS OF THE DERIVATIVE - USED IN NEWTON-RAPHSON ITERATIONS

        C2D =   DBL2 * C2
        C4D =   DBL4 * C4

*-->  0'TH VALUE OF LAMDA - LINEAR INTERPOLATION BETWEEN P(0) & P(YYAV)

CCCC  LAMDA = YYAV * C0 / (C0 + YRRSQ * (XXAV-YYAV))
        LAMDA = 0.0D0
        DLAMDA= 0.0D0

        CHISCL= WSUM * RSCALE**2
        DLAMAX= 0.001 / CHISCL

        DO 30 ITRY = 1,NTRY

            P = C0 + LAMDA * (C1 + LAMDA * (C2 + LAMDA * LAMDA * C4 ))
            PD = (C1 + LAMDA * (C2D + LAMDA * LAMDA * C4D))

            DLAMDA= - P / PD
            LAMDA = LAMDA + DLAMDA

            IF (DABS(DLAMDA).LT.DLAMAX) GO TO 35

   30   CONTINUE

        ITRY = NTRY

   35   CONTINUE

        CHISQ = CHISCL * LAMDA

        DCHISQ= CHISCL * DLAMDA

*-->  NOW CALCULATE THE MATRIX ELEMENTS FOR ALPHA, BETA & KAPPA

        H11   = XXAV  -     LAMDA
        H14   = XRRAV
        H22   = YYAV  -     LAMDA
        H24   = YRRAV
        H34   = DBL1   + DBL2*LAMDA
        IF(H11.EQ.0.0D0.OR.H22.EQ.0.0D0) THEN
           UTCFIT = 1
C          write(6,*) 'Problems in the UTCFIT' ! Ch
           RETURN
        ENDIF

        ROOTSQ= (H14/H11)**2 + DBL4*H34

        IF (DABS(H22).GE.DABS(H24)) THEN
            RATIO = H24/H22
            ROOTSQ=        RATIO**2  + ROOTSQ
            KAPPA = DBL1 / DSQRT(ROOTSQ)
            BETA  = - RATIO * KAPPA
        ELSE
            RATIO = H22/H24
            ROOTSQ= DBL1 + (RATIO**2) * ROOTSQ
            BETA  = DBL1 / DSQRT(ROOTSQ)
            IF (H24   .GT. 0.0) BETA  = - BETA
            KAPPA = - RATIO * BETA
        ENDIF

        ALPHA = - (H14/H11) * KAPPA

*-->  transform these into the lab coordinate system
*-->  first get kappa and back to real dimensions

        KAPPA1 = KAPPA / RSCALE
        DBR0  =   0.5D0 / KAPPA1

*-->  next rotate alpha and beta and scale

        ALPHAR=     (COSROT * ALPHA - SINROT * BETA)*DBR0
        BETAR =     (SINROT * ALPHA + COSROT * BETA)*DBR0

*-->  then translate by (xav,yav)

        ACENT=-( ALPHAR - XAV)
        BCENT=-( BETAR  - YAV)
        R0   = DBR0

        IF(YRRAV .GT. 0) SGN = -1.0
        IF(YRRAV .LT. 0) SGN =  1.0

        RETURN
        END

* $Id: rkms.F,v 1.20 2010/10/16 17:24:03 shaino Exp $
* $Log: rkms.F,v $
* Revision 1.20  2010/10/16 17:24:03  shaino
* Error message suppression
*
* Revision 1.19  2010/10/16 07:27:05  shaino
* Compatibility between gcc3 and gcc4 (conversion of fortran function with _)
*
* Revision 1.18  2010/10/16 07:17:07  shaino
* PGTRACK: Chikanian fit updated
*
* Revision 1.17  2010/07/12 15:35:56  choutko
* *** empty log message ***
*
* Revision 1.16  2010/07/12 14:12:17  choutko
* *** empty log message ***
*
* Revision 1.15  2010/06/24 11:13:35  zweng
* Add #include "VMCrename.h"
*
* Revision 1.14  2010/03/24 10:19:03  choutko
* make rkms work with perm meg opt
*
* Revision 1.13  2010/03/22 22:18:05  pzuccon
* keep compatibility with SLC4
*
* Revision 1.12  2010/03/21 15:16:38  choutko
* add permanent magnet tracker option (9 layers) via "AMS02P" setup
*
* ----------------------------------------------------------------------
*
* Revision 1.11  2010/02/17 16:12:37  choutko
* parallazing rkms
*
* Revision 1.10  2009/11/18 17:01:29  choutko
* fpe
*
* Revision 1.9  2008/12/08 15:15:18  choutko
* *** empty log message ***
*
* Revision 1.8  2008/04/04 08:49:38  choutko
* *** empty log message ***
*
* Revision 1.7  2008/03/10 22:07:14  choutko
* *** empty log message ***
*
* Revision 1.6  2008/02/28 16:14:54  choutko
* *** empty log message ***
*
* Revision 1.5  2006/02/16 13:21:28  choutko
* work around g77 shared
*
* Revision 1.4  2005/11/17 14:29:32  choutko
* correct /dev/null statement
*
* Revision 1.3  2005/09/22 09:09:15  choutko
* :
*
* Revision 1.2  2003/10/30 09:35:02  choutko
* yale algo ++ ( start working)
*
* Revision 1.2  1998/01/27 00:35:54  fisyak
* Split sources
*
      SUBROUTINE UTCMAT(xcir,ycir,wcir,ncir)
*>----------------------------------------------------------------------
* UTCMAT - auxiliary routine for the circle fitting
*<----------------------------------------------------------------------

*      calculates weighted averages needed                    *
*      for circle fitting, shifts, rotates and scales coordi  *
*      nate system so the whole procedure is simpler          *

      IMPLICIT DOUBLE PRECISION   (A-H , O-Z)
      DOUBLE PRECISION   KAPPA
      INTEGER NCIR,I

      COMMON /CIRCOM/ XAV   ,YAV   ,WSUM  ,RSCALE,COSROT,SINROT,
     >                XXAV  ,XYAV  ,YYAV  ,XRRAV ,YRRAV ,RRRRAV,
     >                ALPHA ,BETA  ,KAPPA
      real xcir(*),ycir(*),wcir(*)
        !$OMP threadprivate(/CIRCOM/)

*-->  CIRCLH CALCULATES AVERAGES USED IN THE H MATRIX OF THE CIRCLE FIT
*-->  H IS CALCULATED IN THE COORDINATE SYSTEM WHERE :
*-->  <X>    = 0
*-->  <Y>    = 0
*-->  <XY>   = 0
*-->  <R**2> = 1

*-->  IN THIS COORDINATE SYSTEM ONLY 5 ELEMENTS OF H ARE NEITHER 0 OR 1
*-->  THEY ARE : <X**2>, <Y**2>, <X R**2>, <Y R**2> & <R**4>
*-->  <X>  AND <Y>  BEFORE TRANSLATION ARE NEEDED TO GET BACK TO THE LAB
*-->  COSROT & SINROT ARE NEEDED TO GET BACK TO THE LAB
*-->  SIGN(Q) CAN BE DETERMINED WITH THE SIGNS CHOSEN FOR CROT & SINROT
*-->  WSUM   IS REQUIRED TO SCALE THE CHISQUARE
*-->  RSCALE IS REQUIRED TO SCALE KAPPA and CHISQUARE

*-->  FIRST CALCULATE <X> AND <Y> IN THE ORIGINAL CORRDINATE SYSTEM

        WSUM  = 0.0D0

        XAV   = 0.0D0
        YAV   = 0.0D0

        DO 10 I = 1,NCIR

            WI = WCIR(I)
            XI = XCIR(I)
            YI = YCIR(I)

            WSUM = WSUM + WI

            XAV = XAV + WI * XI
            YAV = YAV + WI * YI

   10   CONTINUE

        XAV   = XAV    / WSUM
        YAV   = YAV    / WSUM

*-->  CALCULATE <X**2>, <XY>, AND <Y**2> WITH <X> = 0, & <Y> = 0

        XXAV  = 0.0D0
        XYAV  = 0.0D0
        YYAV  = 0.0D0

        DO 20 I = 1,NCIR

            WI = WCIR(I)
            XI = XCIR(I)
            YI = YCIR(I)

            XI = XI - XAV
            YI = YI - YAV

            WIXI = WI * XI
            WIYI = WI * YI

            XXAV = XXAV + WIXI*XI
            XYAV = XYAV + WIXI*YI
            YYAV = YYAV + WIYI*YI

   20   CONTINUE

        XXAV  = XXAV   / WSUM
        XYAV  = XYAV   / WSUM
        YYAV  = YYAV   / WSUM

*-->  ROTATE COORDINATES SO THAT <XY> = 0

*-->  SIGN(C**2 - S**2) = SIGN(XXAV - YYAV) >
*-->  &                                     > ==> NEW : (XXAV-YYAV) > 0
*-->  SIGN(S) = SIGN(XYAV)                  >

        A     = DABS( XXAV - YYAV )
        B     = 4.0 * XYAV * XYAV

        ASQPB = A**2 + B
        RASQPB= DSQRT(ASQPB)

        SPLUS  = 1.0D0 + A / RASQPB
        SMINUS = B / (ASQPB * SPLUS)

        SPLUS = DSQRT(0.5 * SPLUS )
        SMINUS= DSQRT(0.5 * SMINUS)

*-->  FIRST REQUIRE : SIGN(C**2 - S**2) = SIGN(XXAV - YYAV)

        IF (XXAV .LE.YYAV ) THEN
           COSROT= SMINUS
           SINROT= SPLUS
        ELSE
           COSROT= SPLUS
           SINROT= SMINUS
        ENDIF

*-->  REQUIRE : SIGN(S) = SIGN(XYAV) * SIGN(C) (ASSUMING SIGN(C) > 0)

        IF (XYAV  .LT. 0.0D0) SINROT = - SINROT

*-->  WE NOW HAVE THE SMALLEST ANGLE THAT GUARANTEES <X**2> > <Y**2>
*-->  TO GET THE SIGN OF THE CHARGE RIGHT, THE NEW X-AXIS MUST POINT
*-->  OUTWARD FROM THE ORGIN.  WE ARE FREE TO CHANGE SIGNS OF BOTH
*-->  COSROT AND SINROT SIMULTANEOUSLY TO ACCOMPLISH THIS.

*-->  CHOOSE SIGN OF C WISELY TO BE ABLE TO GET THE SIGN OF THE CHARGE

        IF((COSROT*XAV + SINROT*YAV).GT. 0.0D0) GO TO 35

        COSROT= - COSROT
        SINROT= - SINROT

   35   CONTINUE

*-->  NOW GET <R**2> AND RSCALE= SQRT(<R**2>)

        RRAV  = XXAV  + YYAV
        RSCALE= DSQRT(RRAV)

        XXAV  = 0.0D0
        YYAV  = 0.0D0
        XRRAV = 0.0D0
        YRRAV = 0.0D0
        RRRRAV= 0.0D0
        XYAV  = 0.0D0

        DO 40 I = 1,NCIR

            WI = WCIR(I)
            XOLD = XCIR(I)
            YOLD = YCIR(I)

            XOLD = XOLD - XAV
            YOLD = YOLD - YAV

*-->  ROTATE SO THAT <XY> = 0 & DIVIDE BY RSCALE SO THAT <R**2> = 1

            XI = ( COSROT * XOLD + SINROT * YOLD ) / RSCALE
            YI = ( - SINROT * XOLD + COSROT * YOLD ) / RSCALE

            XIXI = XI * XI
            YIYI = YI * YI
            RIRI = XIXI + YIYI

            WIRIRI= WI * RIRI

            XYAV = XYAV + WI * XI * YI
            XXAV = XXAV + WI * XIXI
            YYAV = YYAV + WI * YIYI

            XRRAV = XRRAV + WIRIRI * XI
            YRRAV = YRRAV + WIRIRI * YI
            RRRRAV= RRRRAV + WIRIRI * RIRI

   40   CONTINUE

*-->  DIVIDE BY WSUM TO MAKE AVERAGES

        XXAV  = XXAV   / WSUM
        YYAV  = YYAV   / WSUM
        XRRAV = XRRAV  / WSUM
        YRRAV = YRRAV  / WSUM
        RRRRAV= RRRRAV / WSUM
        XYAV  = XYAV   / WSUM

        RETURN
        END
