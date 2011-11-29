** rkms.F
*--------1---------2---------3---------4---------5---------6---------7--
* A.Chikanian, Yale
* Apr.2011: dx=300cm
*---------------------------------------------------------------
* Rigidity reconstruction program for AMS02 with permanet magnet
* Taking care of the Multipole Scattering in AMS Tracking detector:
* Reconstruct track's kinematical parameters with full (nondiagonal)
* Covar.Error Metrix due to Multipole Scattering in Si tracking planes
* as well as in TRD, TOF and RICH.
*---------------------------------------------------------------------
      subroutine rkmsinit(zposl)
      implicit none
#include "rkms.h"
*----------------
      integer k
      real zposl(NPma)
      do k=1,NPma
        trkZ0(k)=zposl(k)
      enddo
C      write(*,*)'RKMS-I-InitTo '
C      write(6,'(8f8.3,f9.3)')trkZ0
      end

*------------------------------
c. Add by Zhili.Weng 2009-11-11
#ifdef __AMSVMC__
#include "VMCrename.h"
#endif

*--------1---------2---------3---------4---------5---------6---------7--
      subroutine rkms_rig(npo,npl,xyz,dxyz,ipa,Rini ,Out)
*     interface to "rkms_fit"
*--------------------------------------------------------
* Input:   npo        - # points
*          npl        - plane #, associated with each point
*          xyz(3,npl) - 3d coordinates
*          dxyz(3,npl)- errors (intrinsic Si planes resolution)
*          ipa        - particle Geant ID#
*          Rini       - initial Rigidity value (try the best guess !!!)
*          --------------------------------------
* Output:  Out(*)     - reconstructed parameters
*          Out(1-3)   - x, y, z 
*          Out(4-5)   - Theta, Phi
*          Out(6)     - Rigidity * charge_sign
*          Out(7)     - Chi2
*          Out(8)     - 0 is o'k, NO convergence if NOT 0
*          Out(9)     - err(1/Rigidity)=err(R)/(R*R)
*---------------------------------------------------
      implicit none
*----------------
#include "rkms.h"
*----------------
      integer    NPm2
      parameter (NPm2=2*NPma)
      double precision charge,sigcha ! for GRKUTA
      common/rktrk/    charge,sigcha
!$OMP threadprivate(/rktrk/)
      real         dx3
      common/dx300/dx3(NPm2)
!$OMP threadprivate(/dx300/)
*     ------------------------------
      integer npo,npl(npo),ipa,i,ier
      real xyz(3,npo),dxyz(3,npo)
      real x(NPma),y(NPma),z(NPma),dx(NPma),dy(NPma)
      real Rini,Out(9),Chi2,R,Rin,doR
      double precision Xout(5)
*     --------------------------------
      ier=0
      Rin=Rini
      R=0.
      do i=1,npo
         x(i)= xyz(1,i)
         y(i)= xyz(2,i)
         z(i)= xyz(3,i)
        dx(i)=dxyz(1,i)
        dy(i)=dxyz(2,i)
        if(dx(i).gt.33.3) dx(i)=33.3 !!! ~ same as missing plane !!!
        dx3(i    )=dx(i)
        dx3(i+npo)=dy(i)
      enddo
*     WARNING ========================================================
*     Due to Error Matrix depends on Rini, pick the best possible Rini
*     OR do final (additional) minimization loop with Rini~Rrec
*     =========================================================
      if(abs(Rin).lt.0.3.or.abs(Rin).gt.333.)
     +                               call R_init(npo,x,y,z,Rin)
      call rkms_fit(npo,npl,x,y,z,dx,dy,ipa,Rin,Xout,doR,Chi2,ier)
*     ============================================================
      if(Chi2.lt.1.e34) then
        R = dsqrt(Xout(1)**2+Xout(2)**2+Xout(3)**2)
        Out(1) = Xout(4)                            ! x
        Out(2) = Xout(5)                            ! y
        Out(3) = z(1)                               ! z
        Out(4) = acos(sngl(Xout(3))/R)              ! Theta
        Out(5) = atan2(sngl(Xout(2)),sngl(Xout(1))) ! Phi
        Out(6) = R*sigcha                           ! Rigidity*ChargeSign
        Out(7) = Chi2                               ! Chi2
        Out(8) = ier                                ! No convergence, if NOT 0
        Out(9) = doR                                ! err(1/Rig)
      else
        Out(1) = 0.
        Out(2) = 0.
        Out(3) = z(1)
        Out(4) = 0.
        Out(5) = 0.
        Out(6) = 0.
        Out(7) = Chi2
        Out(8) = 1.
        Out(9) = 0.
      endif
*     =============================================
      end

*--------1---------2---------3---------4---------5---------6---------7--
      subroutine rkms_fit(npo,npl,x,y,z,dx,dy,ipa,Rini,Xout,doR,Chi2
     +                                                          ,ier)
*   A.Chikanian, Yale, Dec,2010
*   Fitting track parameters with NDC2fit and Runge Kutta 
*--------------------------------------------------------
* Input:   npo   - # points
*          npl   - plane #, associated with each point
*          x(npl),y(npl),z(npl) - 3d coordinates
*          dx(npl),dy(npl) - errors
*          ipa   - particle Geant #
*          Rini  - initial Rigidity value
* Output:  Xout  -  x, y, Rx, Ry, Rz
*          doR   - error of 1/Rigidity
*          Chi2  - Chi2
*          ier   - if not 0 no convergion
*-------------------------------------
      implicit none
      integer    Npa  ,NitMa
      parameter (Npa=5,NitMa=3)
      double precision  Par(Npa),Xout(Npa)
      double precision Xbuf(Npa,NitMa),Chi2B(NitMa),sig(NitMa)
*     --------------------------------------------------------
      integer jB,JR(3)
      integer i,j,it,na/0/
      integer ipa,ier,np2
      integer npo,npl(npo)
      real x(npo),y(npo),z(npo),dx(npo),dy(npo)
      integer nerr/0/
*----------------
#include "rkms.h"
*----------------
c      double precision charge,sigcha ! for GRKUTA
c      common/rktrk/    charge,sigcha
      double precision cha,sigcha ! for GRKUTA
      common/rktrk/    cha,sigcha
!$OMP threadprivate(/rktrk/)
*---------------------------
      real z1,Rini,Chi2,Rad,doR
*     -------------------------
      do i=1,3
        JR(i)=i
      enddo
      ier=1
      if(npo.lt.3) return !!!
      z1=z(1) ! if so Par(4)=x(1), Par(5)=y(1)
      rad = sqrt((x(2)-x(1))**2+(y(2)-y(1))**2+(z(2)-z(1))**2)

*--  refreshing hits information in common/rkms_1/...
      npoc=npo
      np2=npo*2
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
*------------------------------------------------------------------
* First TWO passes in the loop below are for charge sign defenition 
* Third pass is the final iteration with better MS-matrix
*------------------------------------------------------------------
      Chi2=1.e35
      Par(1)=Rini*(x(2)-x(1))/rad                     ! Px0
      Par(2)=Rini*(y(2)-y(1))/rad                     ! Py0
      Par(3)=Rini*(z(2)-z(1))/rad                     ! Pz0
      Par(4)=x(1) + (z1-z(1))/(z(2)-z(1))*(x(2)-x(1)) !  x0
      Par(5)=y(1) + (z1-z(1))/(z(2)-z(1))*(y(2)-y(1)) !  y0

      rkstep=5.                                       ! R/K step(cm)
      JB=0
      DO it=1,NitMa
        if(it.eq.1) sigcha= 1.                        ! for R/K
        if(it.eq.2) sigcha=-1.                        ! for R/K
        if(it.eq.3) then
          sigcha=sig(jB)
          do j=1,Npa
            Par(j)=Xbuf(j,jB)
          enddo
          Rini=dsqrt(Par(1)**2+Par(2)**2+Par(3)**2)
        endif
        sig(it)=sigcha
        if(mod(it,2).eq.1) then
*         --------------------------------------------------
          call wxy_matr9(npo,npl,dx,dy,ipa,Rini) ! Full Matr.
          call C1toC2(npo) ! data transw. com->com
*         ================
        endif ! mod(it,2)=1
*       ========================================= Minimization
        call C2Fit(Par,Xout,doR,Npa,np2,Dhi2,ier)
*       -----------------------------------------
        Chi2B(it)=Dhi2
        if(ier.eq.0) then
          JB=it
          JR(it)=0
          do i=1,Npa
            Xbuf(i,it)=Xout(i)
          enddo
          if(it.eq.2.and.JR(1).eq.0.and.Chi2B(1).lt.chi2B(2)) jB=1
        endif
      ENDDO ! NitMa 
      if(jB.eq.0) return
*     ================================
      ier=0  !  final IER redifinition 
      if(NitMa.gt.2.and.JR(NitMa).eq.0) jB=NitMa

      if(jB.le.2) then
        sigcha=sig(jB)
        do i=1,Npa
          Xout(i)=Xbuf(i,jB)
        enddo
      endif
      if(Chi2B(jB).lt.Chi2) Chi2=Chi2B(jB) ! D2S final Chi2
*     ----------------------------------------------------- IER
      if(dabs(Xout(3)).gt.10.D6) ier=8  ! |Pz| < 10000 TeV
c      if(Chi2.gt.1000.) ier=7
      end

*--------1---------2---------3---------4---------5---------6---------7--
      subroutine C1toC2(npo)
*   mooving data:   wxy -> w,  rkms.h -> /MFit/
*   A.Chikanian, Yale, Dec,2010
*------------------------------
      implicit none
      integer npo,np2,i,j
*----------------
#include "rkms.h"
*----------------
*     -------------------------- /MFit/
      integer    NPm2   ,NpaM
      parameter (NPm2=18,NpaM=5)
      real*8      w           ,he      ,ht      ,der
      common/MFit/w(NPm2,NPm2),he(NPm2),ht(NPm2),der(NpaM,NPm2)
!$OMP threadprivate(/MFit/)
*     ---------------------------------------------------------
      np2=npo*2
      do i=1,np2
        if(i.le.npo) he(i)=xc(i)
        if(i.gt.npo) he(i)=yc(i-npo)
        do j=i,np2
          w(j,i)=0.
          if(i.le.npo.and.j.le.npo) w(j,i)=wx(j    ,i    )
          if(i.gt.npo.and.j.gt.npo) w(j,i)=wy(j-npo,i-npo)
          if(i.ne.j) w(i,j)=w(j,i)
        enddo
      enddo
      end

*--------1---------2---------3---------4---------5---------6---------7--
      subroutine C2Fit(Par,Xout,doR,npa,np2,Dhi2,ier)
*   A.Chikanian, Yale, Dec,2010
*----------------------------------
*  Input:  Par - initial parameters
*         Xuot - fitted parameters
*          doR - error of 1/R
*          npa - number of parameters
*          np2 - number of points for fit
* Output: Dhi2 - Chi2 value
*          ier - error flaf, ok if 0
*-----------------------------------
      integer npa,np2,ier,i
      double precision Par(npa),Xout(npa),Er(npa),Dhi2
      real doR
      double precision DP(5,5)
      double precision R2,rx,ry,rz
      doR=0.
*     ----------------------------
      call NDC2Fit(Par,Xout,DP,npa,np2,Dhi2,ier)
*     -------------------------------------
      R2 = Xout(1)**2+Xout(2)**2+Xout(3)**2
      if(R2.eq.0.) return
      rx = Xout(1)/R2
      ry = Xout(2)/R2
      rz = Xout(3)/R2
      doR  =  DP(1,1)*rx*rx+DP(2,2)*ry*ry+DP(3,3)*rz*rz
     +    +2.*DP(1,2)*rx*ry+DP(1,3)*rx*rz+DP(2,3)*ry*rz ! D(R)/R**2
      doR  = dsqrt(doR/R2)     !   sqrt(D(R)/R**2/R**2) ! d(1/R)=d(R)/R**2 
      end

*--------1---------2---------3---------4---------5---------6---------7--
      subroutine tracking(par,Npa,ht,np2)
*   A.Chikanian, Yale, Dec,2010
*------------------------------
      implicit none
      integer Npa,np2,np,i
      double precision par(Npa)
*----------------
#include "rkms.h"
*----------------
      double precision pin(3),xin(3),xt(NPma),yt(NPma)
      double precision ht(NPma*2)
*     ---------------------------
      pin(1) = par(1) ! Px or cx
      pin(2) = par(2) ! Py or cy
      pin(3) = par(3) ! Pz or R
      xin(1) = par(4) ! x
      xin(2) = par(5) ! y
      xin(3) = zc(1)  ! z
      do i=1,Npma*2
        ht(i)=-999.
      enddo
*     ============================
      call rk_trk(pin,xin,xt,yt,np)
      if(np.ne.npoc) then
        np2=-np*2
        return
      endif
*     ============================
      np2=npoc*2
      do i=1,npoc
        ht(i     )=xt(i)
        ht(i+npoc)=yt(i)
      enddo
      end

*--------1---------2---------3---------4---------5---------6---------7--
      subroutine rk_trk(pin,xin,x0,y0,np)
*---------------------------------------------------
* Runge-Kutta integration for particle in Magn.field
* A.Chikanian, Yale, Feb.,2003
*            Revised June,2010
*-------------------------------------------------------------------
*  Input:  pin(3)   - initial Rigidity Rx,Ry,Rz (GeV/c) for tracking
*          xin(3)   - initial Position  x, y, z (cm)    for tracking
*  Output: x0(NPma) - x,y (cm) coordinates of tragectory crossing
*          y0(NPma) /          with tracking planes
*          np # of planes (should be the same as npo) 
*---------------------------------------------------
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
      data Stot/350.D0/ ! Track's Max.Path Length (cm) (2010)
*     -----------------------------------------------------
      logical first
      data    first/.true./
      double precision w
*     -----------------------------------------------------
      integer     ieie
      common/iiii/ieie
*     ----------------
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
c          if(vect(6)*vout(6).lt.0.) write(6,*)'ve,vo',vect(6),vout(6)
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
      subroutine DerivNum(p0,npa,npo,IER)
*-----------------------------------
*  Numerical derivatives calculation
*  A.Chikanian, Dec.7,2010
*-----------------------------------
*  Input:   p0 - initial parameters
*          npa - number of parameters
*          npo - number of points for fit
* Output:  IER - error flaf, ok if 0
*----------------------------------------
*  der - derevatives go's in commom/Mfit/
*----------------------------------------
      implicit none
      integer npa,npo,IER,i,j,np,nn
*     ----------------------------- /MFit/
      integer    NPm2   ,NpaM
      parameter (NPm2=18,NpaM=5)
      real*8      w           ,he      ,ht      ,der
      common/MFit/w(NPm2,NPm2),he(NPm2),ht(NPm2),der(NpaM,NPm2)
!$OMP threadprivate(/MFit/)
*     ---------------------------------------------------------
      real*8 x0,dp
      real*8 p0(npa)
      real*8 h1(Npm2),h2(NPm2)
      real*8 dp0(NpaM),xx(NpaM)
      data dp0/0.000002,0.000002,0.00002,0.0002,0.0002/
*                 GeV      GeV      GeV     cm     cm
*     ================================================
      ier=0
      call tracking(p0,npa,ht,np)
      if(np.le.0) then
        ier=10
        return
      endif
      do j=1,npa
        xx(j)=p0(j)
      enddo

      do j=1,npa
        dp=dp0(j)
        x0=xx(j)
        xx(j)=x0-dp
        call tracking(xx,npa,h1,nn)
        if(nn.ne.npo.or.nn.le.0) then
          ier=j*10+1
          return
        endif
        xx(j)=x0+dp
        call tracking(xx,npa,h2,nn)
        if(nn.ne.npo.or.nn.le.0) then
          ier=j*10+2
          return
        endif
        do i=1,npo
          der(j,i) = 0.5*(h2(i)-h1(i))/dp
        enddo
        xx(j)=x0
      enddo
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
*     ------------------------------
      real dZ(NPma,NPma)
      real wzn(Npma),wzi(Npma)
      integer ipa
      real R,RR,Rmin
      data      Rmin/0.300/ ! GeV

      integer npo,i,j,k,ii,jj,is,ier,NMSpl,np1
      integer ir(10),ifail
      integer npl(NPma)
      real dx(npo),dy(npo)
      real dms(NPma,NPma),vx(NPma,NPma),vy(NPma,NPma)
      real dTc2,oco(NPma),E,be,P
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
      if(cha.eq.0)then
        cha=1
        ama=0.938
      endif
      RR=max(R,Rmin)
      P  =  RR*cha
      E  =  sqrt(P**2+ama**2)
      be =  P/E ! or from TOF
      dTc2 = 1./(be*RR)**2 ! scatt.angle correction
      charge = cha
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
            vx(ii,ii)=dms(ii,ii)*dTc2 + 1000. ! dX,Y~32cm if missing plane
            vy(ii,ii)= vx(ii,ii)
          enddo
        endif
        ii=ii+1
        oco(ii)=dsqrt(1.+((xc(i)-xc(i-1))**2+(yc(i)-yc(i-1))**2)
     +                                  /dz(npl(i),npl(i-1))**2)
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
*   Revised: Oct.,2010 (consistent with ams02p geom.)
*----------------------------------------------------
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
      real       p(9)/ 157.60, 79.26  !  TRD  p1,p2
     +               ,  75.85, 60.0   !  TOF  p3,p4
     +               ,  58.           !  HC2  p5
     +               , -59.79,-70.53  !  TOF2 p6,p7
     +               , -74.37,-74.92/ !  RICH p8,p9
      real       z(9)/ 166.9          !  z1   PL8 (1N)
     +               ,  52.96         !  z2   PL1
     +               ,  29.15         !  z3   PL2
     +               ,  25.19         !  z4   PL3
     +               ,   1.69         !  z5   PL4
     +               ,  -2.30         !  z6   PL5
     +               , -25.23         !  z7   PL6
     +               , -29.22         !  z8   PL7
     +               ,-135.72/        !  z9   PL9
*   ----------------------------------------------------------------
*                       Si      TRD    TOF   Si or HC     TOF   RICH
c      real  xRL(16)/0.0032054,0.0764,0.047,11*0.0032054,0.047,0.0335/
c      real  xRL(16)/0.0032054, 0.04,  0.03 ,11*0.0032054,0.03 ,0.02/
*    ------------------------------------------------------------
      real  xRL(16)/0.0032054,0.1624,0.1117 ! Si1,TRD,TOF2  1,2,3
     +  ,  0.007841                         ! HC2           4,
     +  ,2*0.0032054                        ! Si2,3         5,6
     +  ,  0.0025                           ! HC34          7,
     +  ,2*0.0032054                        ! Si4,5         8,9
     +  ,  0.0052                           ! HC56         10,
     +  ,2*0.0032054                        ! Si6,7        11,12
     +  ,  0.0052                           ! HC78         13,
     +  ,  0.0032054                        ! Si8          14,
     +  ,  0.094                            ! TOF2         15,
     +  ,  0.044/                           ! RICH         16
*   -------------------------------------------------------------
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
        write(6,*)'--- matr_scatN9 initialization completed ----'
        first=.false.
      endif  !  -----------   end first
*     ---------------------------------
      if(npl1.lt.1.or.npl1.gt.7) return
      do k=1,NPma*NPma
        dms(k,1)=0.
      enddo
      npl=Npma-npl1+1
      do i = 2,npl
        do j=2,npl
          dms(i,j)=dmsN9(i,j,npl1)
        enddo
      enddo
      end

*---------1---------2---------3---------4---------5---------6---------7--
      subroutine matr_scatN1(npl1,dms)
* Cov.Err.Matrix with Mult.Scattering for Down -->Up track
* A.Chikanian June,2010.
*   Revised:  Oct.,2010 (consistent with ams02p geom.)
*-----------------------------------------------------
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
      real       p(9)/ 157.60, 79.26  !  TRD  p1,p2
     +               ,  75.85, 60.0   !  TOF  p3,p4
     +               ,  58.           !  HC2  p5
     +               , -59.79,-70.53  !  TOF2 p6,p7
     +               , -74.37,-74.92/ !  RICH p8,p9
      real       z(9)/ 166.9          !  z1   PL8 (1N)
     +               ,  52.96         !  z2   PL1
     +               ,  29.15         !  z3   PL2
     +               ,  25.19         !  z4   PL3
     +               ,   1.69         !  z5   PL4
     +               ,  -2.30         !  z6   PL5
     +               , -25.23         !  z7   PL6
     +               , -29.22         !  z8   PL7
     +               ,-135.72/        !  z9   PL9
*    ------------------------------------------------------------
      real  xRL(16)/0.0032054,0.1624,0.1117 ! Si1,TRD,TOF2  1,2,3
     +  ,  0.007841                         ! HC2           4,
     +  ,2*0.003205                         ! Si2,3         5,6
     +  ,  0.0025                           ! HC34          7,
     +  ,2*0.003205                         ! Si4,5         8,9
     +  ,  0.0052                           ! HC56         10,
     +  ,2*0.003205                         ! Si6,7        11,12
     +  ,  0.0052                           ! HC78         13,
     +  ,  0.003205                         ! Si8          14,
     +  ,  0.094                            ! TOF2         15,
     +  ,  0.044/                           ! RICH         16
*   -------------------------------------------------------------
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
        write(6,*)'--- matr_scatN1 initialization completed ----'
        first=.false.
      endif  !  -----------   end first
*     ---------------------------------
      if(npl1.gt.9.or.npl1.lt.3) return
      do k=1,NPma*NPma
        dms(k,1)=0.
      enddo
      npl=npl1
      do i = 2,npl
        do j=2,npl
          dms(i,j)=dmsN1(i,j,NPma-npl1+1)
        enddo
      enddo
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
*----------------
#include "rkms.h"
*----------------
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
c        write(6,*)'=== R_init: < Bx > =',Bx
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

*           Chi2 -  m i n i m i z a t i o n
*--------1---------2---------3---------4---------5---------6---------7--
      Subroutine NDC2Fit(p0,pf,DP,npa,npo,Chi2,ier)
*  Chi2 minimization with non diagonal error matrix
* A.Chikanian, Dec.7,2010
*--------------------------------------------------
*  Input:   p0 - initial parameters
*           pf - fitted  parameters
*          npa - number of parameters
*          npo - number of points for fit
* Output: Chi2 - Chi2 value
*           DP - error matrix for fitted parameters (pf)
*          ier - error flaf, ok if 0
*-----------------------------------------------
      implicit none
      integer npa,npo,ier,iter,i
      real*8 p0(npa),pf(npa),DP(npa,npa)
      real*8 Chi2,dpa,Chi2pr
      real*8 eps/1.d-6/
      real*8 pp(5)
      integer Nit/1000/
*     ------------------
      do i=1,npa
        pp(i)=p0(i)
      enddo
      ier=0
      Chi2pr = 1.d35
      iter=0
 10   call OneIter(pp,pf,npa,npo,Chi2,ier)

      if(ier.ne.0) return
      iter=iter+1
      if(Chi2.gt.Chi2pr) go to 100
      dpa=0.          
      do i=1,npa
        dpa = dpa + (pp(i)-pf(i))**2
        pp(i)=pf(i)
      enddo
      if((dpa.gt.eps.or.dabs(Chi2pr-Chi2).gt.eps).and.iter.lt.Nit) then
        Chi2pr=Chi2
        go to 10
      endif
 100  continue
      call ParEM(npa,npo,DP,ier)
      end

*--------1---------2---------3---------4---------5---------6---------7--
      Subroutine OneIter(p0,pf,npa,npo,Chi2,IER)
*    One minimization iteration
*    A.Chikanian, Dec.7,2010
*    A.Chikanian, Apr.26,2011 dx=300
*----------------------------------
*  Input:   p0 - initial parameters
*           pf - fitted  parameters
*          npa - number of parameters
*          npo - number of points for fit
* Output: Chi2 - Chi2 value
*          IER - error flaf, ok if 0
*-----------------------------------
      implicit none
*     ------------------------- /MFit/
      integer    NPma  ,NPm2       ,NpaM
      parameter (NPma=9,NPm2=2*NPma,NpaM=5)
      real*8      w           ,he      ,ht      ,der
      common/MFit/w(NPm2,NPm2),he(NPm2),ht(NPm2),der(NpaM,NPm2)
!$OMP threadprivate(/MFit/)
      real         dx3
      common/dx300/dx3(NPm2)
!$OMP threadprivate(/dx300/)
*     ---------------------------------------------------------
      integer npa,npo,IER
      real*8 p0(npa),pf(npa)
      real*8 CN(NpaM,NpaM),BN(NpaM)
      real*8 eps/1.d-6/
      real*8 Chi2,Chi2Fun
       integer i,j,k,m
*     -----------------------------
      Chi2=1.E35
      do i=1,npa
        pf(i)=0.
      enddo
      call DerivNum(p0,npa,npo,IER)
      if(ier.ne.0) return
*     ---------- Normal equation preparing
      do k=1,npa
        BN(k)=0.
        do i=1,npo
          if(dx3(i).lt.30.) then
            do j=1,npo
              if(dx3(j).lt.30.) then
                if(w(i,j).ne.0.) then
                  BN(k)=BN(k)+w(i,j)*2.*der(k,j)*(he(i)-ht(i))
                endif
              endif
            enddo
          endif
        enddo
        do m=k,npa
          CN(m,k)=0.
          do i=1,npo
            if(dx3(i).lt.30.) then
              do j=1,npo
                if(dx3(j).lt.30.) then
                  if(w(i,j).ne.0.) then
                    CN(m,k) = CN(m,k) + 2.*w(i,j)*der(k,j)*der(m,i)
                  endif
                endif
              enddo
            endif
          enddo
          if(m.ne.k) CN(k,m)=CN(m,k)
        enddo
      enddo
*     ------------------------------- Solwing Normal Equation
      Call DSEQN(npa,CN,NpaM,ier,1,BN)
      if(ier.ne.0) return
*     ---------------------------------------------
      do m=1,npa
        pf(m)=p0(m)+BN(m)
      enddo
      Chi2=Chi2Fun(npo)
      end

*--------1---------2---------3---------4---------5---------6---------7--
      subroutine ParEM(npa,npo,DP,ier)
*    Calculate full Error matrix of parameters
*    A.Chikanian, Dec.16,2010
*-------------------------------------
*  Input:  npa - number of parameters
*          npo - number of points for fit
* Output:   DP - full (5x5) error matrix
*          ier - error flaf, ok if 0
*-----------------------------------
      implicit none
*     -------------------------- /MFit/
      integer    NPma  ,NPm2       ,NpaM
      parameter (NPma=9,NPm2=2*NPma,NpaM=5)
      real*8      w           ,he      ,ht      ,der
      common/MFit/w(NPm2,NPm2),he(NPm2),ht(NPm2),der(NpaM,NPm2)
!$OMP threadprivate(/MFit/)
      real         dx3
      common/dx300/dx3(NPm2)
!$OMP threadprivate(/dx300/)
*     ---------------------------------------------------------
      integer npa,npo,ier
      real*8 DP(npa,npa)
      integer i,j,k,m,ncou/0/
*     ---------- Normal equation preparing
      do k=1,npa
        do m=k,npa
          DP(m,k)=0.
          do i=1,npo
            if(dx3(i).lt.30.) then
              do j=1,npo
                if(dx3(j).lt.30.) then
                  if(w(i,j).ne.0.) DP(m,k) = DP(m,k)
     +                           + 2.*w(i,j)*der(k,j)*der(m,i)
                endif
              enddo
            endif
          enddo
          if(m.ne.k) DP(k,m) = DP(m,k)
        enddo
      enddo
*     ---------------------- Normal Equation Matrix Inversion
      call dsinv(NpaM,DP,NpaM,ier) ! F012 Symm.Matr.Inversion
*     ---------------------------
      if(ier.eq.0) return
      if(ncou.le.10) then
        ncou=ncou+1
        write(6,*)'### ParEM: dsinv: ier=',ier
      endif
      end

*--------1---------2---------3---------4---------5---------6---------7--
      Real*8 Function Chi2Fun(npo)
*    Calculate Chi2 with nondiagonal matrix (x,y-blocks)
*   A.Chikanian, Dec.7,2010, Apr,2011
*----------------------------------------
*  Input:  npo - number of points for fit
*          common/MFit/
* Output:  Chi2Fun
*----------------------------------------
      implicit none
      integer i,j,npo,nh,ih,jh
      real*8 h1,h2
*     ------------------------- /MFit/
      integer    NPma  ,NPm2       ,NpaM
      parameter (NPma=9,NPm2=2*NPma,NpaM=5)
      real*8      w           ,he      ,ht      ,der
      common/MFit/w(NPm2,NPm2),he(NPm2),ht(NPm2),der(NpaM,NPm2)
!$OMP threadprivate(/MFit/)
      real         dx3
      common/dx300/dx3(NPm2)
!$OMP threadprivate(/dx300/)
*     ------------------------ in case NO (x,y)-correlations
      nh=npo/2
      Chi2Fun=0.
      do i=1,npo ! x & y
        if(dx3(i).lt.30.) Chi2Fun = Chi2Fun + w(i,i)*(he(i)-ht(i))**2
      enddo
      do i=2,nh-1
        ih=i+nh
        h1 = he(i )-ht(i )
        h2 = he(ih)-ht(ih)
        do j=i+1,nh
          if(dx3(i).lt.30..and.dx3(j).lt.30.) then
            if(w(i,j).ne.0.)
     +        Chi2Fun = Chi2Fun + 2.*w(i,j)*h1*(he(j)-ht(j))
          endif
          jh=j+nh
          if(w(ih,jh).ne.0.) 
     +      Chi2Fun = Chi2Fun + 2.*w(ih,jh)*h2*(he(jh)-ht(jh))
        enddo
      enddo
      end

*======================================================================== Lib
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
*--- interface Single(GUFLD) -> Double(DUFLD)
*--------------------------------------------
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
      Real function Fu(z) ! For Mag.Field integration by Gauss
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
* also it is probably not right in case dX not eq. dY
* for example Xav = Sum(Wx(i)*X(i))/Sum(Wx(i)),
*     but not Xav = Sum( W(i)*X(i))/Sum( W(i)) !!!!!!!!!!!!!!! 
* unique w(i) is only possible in case dX(i)=dY(i)
*===========================================================================
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
