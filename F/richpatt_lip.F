#define MIRSEC
#undef LIP_DEBUG

**************************************************
* In this file
* ------------
*      subroutine patmatr
*      subroutine patrace(phig,thc,vdet,vdir,nmiref,nerr,verr)
*      subroutine patrace_full(phig,thc,xdet,ydet,nmiref,nerr,verr)
*      subroutine patrace_gcheck(phpto, ierr)
*      subroutine patracen(phig,thc,pthetemp,pphitemp,pvertex,vdet,vdir,
*                          nmiref,nerr,verr)
*      subroutine patrace_fulln(phig,thc,pthe,pphi,pvertex,xdet,ydet,nmiref,nerr,verr)
*      subroutine patmatrn(pthetemp,pphitemp)
*      subroutine richacc(thetac,richacca,richeffa)                      
*      subroutine dphipath(thetac,nphitot,vphitot,fphitot,
*     +                    dphidir,dphihol,dphimir0,dphimir1,dphiinv,
*     +                    npvis,phimin,phimax,fphipat)
*      subroutine set_phiroots(thetac,nphitot,vphitot,fphitot)
*      subroutine find_phiroots(chopt,thetac,vphival,nmireft)
*      subroutine tileroots(x1,x2,y1,y2,ig,vphival)
*      subroutine HLIMASYM(xlow,xhig,ylow,yhig)
*      subroutine HLIMSYM(xlow,xhig,ylow,yhig)
*      subroutine getphivis(thetac,npvis,phimin,phimax)                      
*      subroutine richaccsmear(thc,zch,richacca,richeffa )
*      subroutine richaccpmtgap(accgap,accgapdir,accgapref)
*      subroutine rcradnint(pardou,npaths,pphimin,pphimax,lphipat,           
*     +                     pnint)                                 
*      subroutine rcradeff(pardou,npaths,pphimin,pphimax,lphipat,           
*     +                    effout)
*      subroutine FSUB12(M,U12,F12,X)                                           
*      subroutine FSUB11(M,U11,F11,X)                                           
*      subroutine rcpmteff(thc,npaths,pphimin,pphimax,effpmt)
*      subroutine rctoteff(pardou,npaths,pphimin,pphimax,lphipat,           
*     +                    efftot)                                 
*      subroutine FSUB22(M,U22,F22,X)                                           
*      subroutine FSUB21(M,U21,F21,X)                                      
*      subroutine zradmean(pardou,npaths,pphimin,pphimax,lphipat,           
*     +                    zoutmean)                                  
*      subroutine FSUB3(M,U12,F12,X)                                           
*      subroutine rtheimean(thc,npaths,pphimin,pphimax,theim)
*      subroutine patphot(phig,thc,coneg)
*      subroutine patints(p0,v0,CHOPT,ierr)
*      subroutine patrefr(CHIN,CHOUT,phdir,ierr)
*      subroutine patrefl(phpto,phdir,ierr)
*      subroutine richaccradwall(accradwalldir, accradwallref)
*      real function fphotdist(phig)
*      real Function fphilmir0(phig)                                          
*      real Function fphilmir1(phig)                                          
*      real Function fphilmir_prot(phig) 
*      real function fphilhole(phig)
*      real function fphilrefl(phig)
*      real Function fphilrad(phig)
*      real Function fphilgap(phig)
*      DOUBLE PRECISION FUNCTION flgeff(dphig)
*      real function zvtxmean(thetac)
*      DOUBLE PRECISION FUNCTION fthei(dphig)
*
****************************************************


***************************
*** CODE FROM patmatr.F ***
***************************

* ==========================================================================
      subroutine patmatr
* ==========================================================================
*************************************************************
* 
* FIGROTM matrix calculation
* to get (x,y,z) from (x',y',z') 
*
*************************************************************

#include "../include/richrec_lipf.h"

      real paxisx(3),paxisy(3),paxisz(3)
      real axisx(3),axisy(3),axisz(3)

* --- general frame
      axisx(1) = 1. !X 
      axisx(2) = 0.    
      axisx(3) = 0.    
*
      axisy(1) = 0. !Y 
      axisy(2) = 1.    
      axisy(3) = 0. 
*
      axisz(1) = 0. !Z    
      axisz(2) = 0.    
      axisz(3) = 1.    


* --- particle frame axis
      if (pthe.eq.0.) then
         call ucopy(axisx,paxisx,3)
         call ucopy(axisy,paxisy,3)
         call ucopy(axisz,paxisz,3)
      else
        paxisz(1) = sin(pthe)*cos(pphi)   !Z
        paxisz(2) = sin(pthe)*sin(pphi)
        paxisz(3) = abs(cos(pthe))  
        paxiszm = sqrt(paxisz(1)**2+paxisz(2)**2+paxisz(3)**2)
*
        paxisy(1) =  paxisz(2)             !Y
        paxisy(2) = -paxisz(1) 
        paxisy(3) =  0.
        paxisym = sqrt(paxisy(1)**2+paxisy(2)**2+paxisy(3)**2)
        paxisy(1) =  paxisy(1)/paxisym
        paxisy(2) =  paxisy(2)/paxisym
        paxisym = sqrt(paxisy(1)**2+paxisy(2)**2+paxisy(3)**2)

*
        call cross(paxisy,paxisz,paxisx)  !X
        paxisxm = sqrt(paxisx(1)**2+paxisx(2)**2+paxisx(3)**2)
        paxisx(1) = paxisx(1)/paxisxm
        paxisx(2) = paxisx(2)/paxisxm
        paxisx(3) = paxisx(3)/paxisxm
      endif
C      print*,' ***** particle axis ******'
C      print*,(paxisx(i),i=1,3),paxisxm
C      print*,(paxisy(i),i=1,3),paxisym
C      print*,(paxisz(i),i=1,3),paxiszm

* --- matrix elements
      figrotm(1,1) = vdot(axisx,paxisx,3)            
      figrotm(1,2) = vdot(axisx,paxisy,3)            
      figrotm(1,3) = vdot(axisx,paxisz,3)            
*
      figrotm(2,1) = vdot(axisy,paxisx,3)            
      figrotm(2,2) = vdot(axisy,paxisy,3)            
      figrotm(2,3) = vdot(axisy,paxisz,3)            
*
      figrotm(3,1) = vdot(axisz,paxisx,3)            
      figrotm(3,2) = vdot(axisz,paxisy,3)            
      figrotm(3,3) = vdot(axisz,paxisz,3)            
* 
C      print*,' ***** rotation matrix ******'
C      print*,(figrotm(1,j),j=1,3)
C      print*,(figrotm(2,j),j=1,3)
C      print*,(figrotm(3,j),j=1,3)
C      print*,'*****************************'

      return
      end


*******************************
*** CODE FROM richpattv11.F ***
*******************************

******************************************************************
* 
*  PPPPP  AAAA TTTTT TTTTT EEEEE RRRRRR N    N
*  P   P  A  A   T     T   E     R    R N N  N
*  PPPP   AAAA   T     T   EEE   R R    N  N N
*  P      A  A   T     T   E     R  R   N    N 
*  P      A  A   T     T   EEEEE R   R  N    N
*                
******************************************************************
*
* RICH GEOMETRY
* =============
*
*
*                  ----------------------------------  -> Z=0
*                  |                radiator        |  |> HRAD
*                  ----------------------------------  
*                  |                foil            |  |> HPGL
*                  ----------------------------------                
*                                                      |> ZTMIRGAP
*                  ----------------------------------         
*                 /                                  \        |\
*                /                                    \       | \
*               /                                      \      |  \
*              /                                        \     |   \ HMIR
*             /                                          \    |   /
*            /                                            \   |  /
*           /                                              \  | /
*                                                             
*                                                             |> ZBMIRGAP
*           ------------------------------------------------  -> ZPMTDET



***********************************************************************************************
* LIST OF SUBROUTINES AND FUNCTIONS                                         
* ---------------------------------
*                                                                    created      last changed 
* - subroutine patrace                                               20/9         10/02/07
*              (*phig,*thc,vdet*,vdir*,nmiref*,nerr*,verr*)
* - subroutine patrace_full(phig,thc,xdet,ydet,nmiref,nerr,verr)  25/07/02     15/08/03      
* - subroutine patrace_gcheck(phpto, ierr)                           15/08/03 
*
* - subroutine patphot(phig,thc,coneg)                            20/9
* - subroutine patints(p0,v0,CHOPT,ierr)                             20/9
* - subroutine patrefr(CHIN,CHOUT,phdir,ierr)                        20/9
* - subroutine patrefl(phpto,phdir)                              
* - subroutine patmatr     
*
* - real function fphotdist(phig,thetac)                             20/9
*
* ================================================================================
*
*  "clone" subroutines used by the display subroutine (photrace and photrace_prot)
*
* - subroutine patracen
*   (*phig,*thc,*pthe,*pphi,*pvertex,vdet*,vdir*,nmiref*,nerr*,verr*)       10/09/03      
* - subroutine patrace_fulln
*   (*phig,*thc,*pthe,*pphi,*pvertex,xdet*,ydet*,nmiref*,nerr*,verr*)       10/09/03      
* - subroutine patmatrn(*pthe,*pphi)                                           10/09/03
*
*******************************************************************
* ================================================================         
      subroutine patrace(phig,thc,vdet,vdir,nmiref,nerr,verr)
* ================================================================         
******************************************************************         
*       AIMS: Steering routine for pattern cerenkov drawing                
*                                                                          
* PARAMETERS: IN - phig   = photon phi angle                               
*                  thc    = cerenkov angle                                 
*            OUT - vdet   = photon detection point at the PMT plane         
*                  vdir   = direction of the photon at the PMT plane         
*                  nmiref = number of reflections on mirror                
*                  ierr   = error flag                   
*
*      verr(1:20)    photon error coding
*      ----------    -------------------
*              10  - photon from radiator: backward photon
*              11  - photon from radiator: lost at radiator boundaries (outer boundries)
*              12  - photon from radiator: lost at plexiglass boundaries
*              13  - photon from plexiglas: ost at plexiglass boundaries
*              14  - photon from radiator: lost at radiator inner walls
*              15  - total reflection rad-pgl
*              16  - total reflection pgl-air
*              17  - total reflection rad-air
*              18  - 
*              19  - ends in non-active detection region                   
*              20  - side boundaries of the top gap 
*              21  - side boundaries of the bottom gap 
C 
******************************************************************         

#include "../include/richrec_lipf.h"

      integer nphmax  
      parameter(nphmax=10)
      integer nerr,verr(nphmax)                                                
      real phig, thc, phdir(3), phpto(3),phdirn(3), vdet(3),vdir(3)
      character*3 CHIN,CHOUT
      integer ierrmir
      real phi_fotao

* --- init                                                                 
      nerr   = 0                                                           
      call vfill(verr,nphmax,0)
      nmiref = 0 ! number of mirror reflections or out of prototype PMT matrix 
      vdet(1) = -999.                                                         
      vdet(2) = -999.                     

* --- set particle rotation matrix if not already done
      if (irotflg.eq.0) then          
         call patmatr                 
         irotflg = 1                  
      endif                           

* --- TRANSPORT PHOTON TO RADIATOR BOTTOM SURFACE
*     -------------------------------------------

* ... photon emission point                                                
      call ucopy(pcervtx,phpto,3)                                          
* ... generate photon on detector frame                                    
      call patphot(phig,thc,phdir)                                      
* ... test photon direction: backward?                                     
      if (phdir(3).lt.0.) then                                             
         nerr = nerr + 1                                                   
         verr(nerr) = 10                                                   
         return                                                            
      endif                                                                
* ... photon refraction                                                    
      /* test photon origin medium */
      if (chradid.eq.'AGL'.or. chradid.eq.'NAF') then !photons radiated in AGL or NAF
         call patints(phpto,phdir,'RADB',ierr) !intersection with rad bottom  

         if (ierr.ne.0) then                                             
            nerr = nerr + 1                                              
            if (ierr.eq.1) verr(nerr) = 11   !photon going out of radiator outer boundary 
            if (ierr.eq.2) verr(nerr) = 14   !photon absorbed by inner walls (depends on LEVGRAD)
            return                                                       
         endif

         /* test plexiglass support */
         if (HPGL.ne.0.0) then
            chin = chradid 
            call patrefr(chin,'PGL',phdir,ierr)  !refraction 
            if (ierr.ne.0) then                  !total reflection         
               nerr = nerr + 1                                             
               verr(nerr) = 15                                             
               return                                                      
            endif                                                          
            call patints(phpto,phdir,'PGLB',ierr)                          
            if (ierr.ne.0) then                                             
               nerr = nerr + 1                                              
               verr(nerr) = 12                !photon going out of radiator 
               return                                                       
            endif                                                           
            call patrefr('PGL','AIR',phdir,ierr)  !refraction              
            if (ierr.ne.0) then                  !total reflection         
               nerr = nerr + 1                                             
               verr(nerr) = 16                                             
               return                                                      
            endif
         else
            chin = chradid 
            call patrefr(chin,'AIR',phdir,ierr)  !refraction              
            if (ierr.ne.0) then                  !total reflection         
               nerr = nerr + 1                                             
               verr(nerr) = 17                                             
               return                                                      
            endif                                                          
         endif
      /* photon starting in plexiglass */
      elseif (chradid.eq.'PGL') then
         if (HPGL.eq.0.0) then
           print*,' >>> patrace1: photons starting in PGL when HPGL=0'
           return 
         endif   
         call patints(phpto,phdir,'PGLB',ierr) !intersection            
         if (ierr.ne.0) then                                             
            nerr = nerr + 1                                              
            verr(nerr) = 13                !photon going out of radiator 
            return                                                       
         endif                                                           
         call patrefr('PGL','AIR',phdir,ierr)  !refraction              
         if (ierr.ne.0) then                  !total reflection         
            nerr = nerr + 1                                             
            verr(nerr) = 16                                             
            return                                                      
         endif                                                          
      endif                      


* --- TRANSPORT PHOTON TO DETECTION PLANE
*     -----------------------------------

*     DIRECT or INDIRECT DETECTION
*     ----------------------------
      if (rcgeom(1).eq.9 .and. rcgeom(2).eq.0) goto 999     !prototype
* ... intersect photon with bottom mirror plane
      call patints(phpto,phdir,'BMIR',ierr)  
* ==> DIRECT
      if (ierr.eq.0) then 
          goto 999
* ==> INDIRECT DETECTION
      else            
          call patints(phpto,phdir,'TMIR',ierr) 
          if (ierr.ne.0) then
             nerr = nerr + 1
*             verr(nerr) = ierr
             verr(nerr) = 20
             goto 999
          else  
             ierrmir = 0
             do while (ierrmir.eq.0 .and. 
     +                  (rcgeom(1).ne.9 .or.                        !flight 
     +                  (rcgeom(1).eq.9 .and. rcgeom(2).eq.1)))     !prototype+mirror
                call patints(phpto,phdir,'MIRA',ierrmir)            !intersect mirror
                if (ierrmir.ne.0)then
                   nerr=nerr+1
                   verr(nerr) = 91
                   return                                             
                endif 
                /* prototype specific */
                if (rcgeom(1).eq.9 .and. rcgeom(2).eq.1) then     !prototype+mirror
                   phimin = mirphild(1)
                   phimax = mirphild(2)
                   arcmir = phimax-phimin
                   if (arcmir.lt.0.) arcmir=arcmir+twopi
*
                   phi_fotao=atan2(phpto(2)-vtmir(2),phpto(1)-vtmir(1))
                   if(phi_fotao.lt.0.) phi_fotao = phi_fotao + twopi
                   arcphot = phi_fotao-phimin
                   if (arcphot.lt.0.) arcphot = arcphot + twopi
                   if (arcphot.le.arcmir) then
                      call patrefl(phpto,phdir,ierrmir) !photon direction after reflection
                   endif
                   goto 999
                endif
                /* end of prototype specific */
                call patrefl(phpto,phdir,ierrmir) !photon direction after reflection      
                call patints(phpto,phdir,'BMIR',ierr)
                if (ierr.eq.0) then !direct detection
                   ierrmir = 1 !get out of the loop
                /* commented ... 5/jan/07
                else
                   nerr = nerr + 1
*             verr(nerr) = ierr
                   verr(nerr) = 21
                   goto 999
                */
                endif  
*
                nmiref = nmiref + 1                                               
                IF (NMIREF.GT.10)then
                   nerr=nerr+1
                   RETURN
                endif
             enddo
          endif
      endif                 

 999  continue
* ... intersect photon with PMT plane
      call patints(phpto,phdir,'PMTD',ierr)  

* ... output coordinates                                                   
      call ucopy(phpto,vdet,3)
      call ucopy(phdir,vdir,3)                                                

* --- end                                                                  
      return                                                               
      end                                                                     


* ================================================================         
      subroutine patrace_full(phig,thc,xdet,ydet,nmiref,nerr,verr)
* ================================================================         
******************************************************************         
*       AIMS: Steering routine for pattern cerenkov drawing                
*                                                                          
* PARAMETERS: IN - phig   = photon phi angle                               
*                  thc    = cerenkov angle                                 
*            OUT - xdet   = x-coord of the photon at the PMT plane         
*                  ydet   = y-coord of the photon at the PMT plane         
*                  nmiref = number of reflections on mirror                
*                  ierr   = error flag                                     
*
*      verr(1:20)    photon error coding
*      ----------    -------------------
*              10  - photon from radiator: backward photon
*              11  - photon from radiator: lost at radiator boundaries (outer boundries)
*              12  - photon from radiator: lost at plexiglass boundaries
*              13  - photon from plexiglas: ost at plexiglass boundaries
*              14  - photon from radiator: lost at radiator inner walls
*              15  - total reflection rad-pgl
*              16  - total reflection pgl-air
*              17  - total reflection rad-air
*              18  - 
*              19  - photon ends in non-active detection region
*
*              25  - photon ends in matrix region but near particle track
*
* Note: the patrace_full routine checks if the traced photon ends in
*       a visible or invisible region;
*       the radiator walls and Lguide gaps are not considered on this scheme
*       because their impact on the ring acceptance was made independent
*       of the analytical phi roots scheme. 
******************************************************************         

#include "../include/richrec_lipf.h"

      integer nphmax
      parameter(nphmax=10)
      integer nerr, verr(nphmax), nmiref
      real phig, thc, vdet(3),vdir(3)
 
* ... get photon impact impact point and direction on detection matrix
      call patrace(phig,thc,vdet,vdir,nmiref,nerr,verr)
      if (nerr.ne.0) return

* ... check if photon is falling in DETECTION MATRIX HOLES 
      call patrace_gcheck(vdet,ierr)
      if (ierr.ne.0) then
         nerr = nerr + 1
         verr(nerr) = ierr
      endif

* ... check particle track near ring
      if (ierr.eq.0 .and. LEVGHIT.eq.1) then
         if ( (vdet(1)-pcoopmt(1))**2+(vdet(2)-pcoopmt(2))**2.lt.
     +                                            trhitass2) then
            nerr = nerr + 1
            verr(nerr) = 25
         endif 
      endif

* ... output coordinates                                                   
      xdet = vdet(1)                                                      
      ydet = vdet(2)                   

* --- end                                                                  
      return                                                               
      end                                                                             


* ================================================================         
      subroutine patrace_full_phimir
     +     (phig,thc,xdet,ydet,nmiref,nerr,verr,phimir)
* ================================================================         
******************************************************************         
*
*     Similar to patrace_full, but returns mirror phi
*     for reflected photons
*
*     R.Pereira - 07/May/2009
*
******************************************************************         

#include "../include/richrec_lipf.h"

      integer nphmax
      parameter(nphmax=10)
      integer nerr,verr(nphmax)
      real vdet(3),vdir(3)
      real phimir

      real vdetcopy(3)
      real vdirinv(3)

* ... get photon impact impact point and direction on detection matrix
      call patrace(phig,thc,vdet,vdir,nmiref,nerr,verr)
      if (nerr.ne.0) return

* ... check if photon is falling in DETECTION MATRIX HOLES 
      call patrace_gcheck(vdet,ierr)
      if (ierr.ne.0) then
         nerr = nerr + 1
         verr(nerr) = ierr
      endif
*LA 13Out10
      if (nerr.ne.0) return


* ... check particle track near ring
      if (ierr.eq.0 .and. LEVGHIT.eq.1) then
         if ( (vdet(1)-pcoopmt(1))**2+(vdet(2)-pcoopmt(2))**2.lt.
     +                                            trhitass2) then
            nerr = nerr + 1
            verr(nerr) = 25
         endif 
      endif

* ... output coordinates                                                   
      xdet = vdet(1)                                                      
      ydet = vdet(2)                   

* ... mirror phi
      if(nmiref.eq.0) then
         phimir = -999.
      else
         vdetcopy(1) = vdet(1)
         vdetcopy(2) = vdet(2)
         vdetcopy(3) = vdet(3)

* LA FB 29Set10  !Test
*         vdirinv(1) = -vdir(1)
*         vdirinv(2) = -vdir(2)
*         vdirinv(3) = -vdir(3)

         call patints(vdetcopy,vdir,'MIRI',ierr)
         phimir = atan2(vdetcopy(2)-vtmir(2),vdetcopy(1)-vtmir(1))
         if(phimir.lt.0.) phimir = phimir+TWOPI
      endif
*      print*,'phimir = ',phimir

* --- end                                                                  
      return                                                               
      end
 

* ------------------------------------------------------------------
      subroutine patrace_gcheck(phpto,ierr)
* ------------------------------------------------------------------
*********************************************************************
* Check if photon falls in a dead area of the detection matrix plane
*
* IN : phpto  - photon x,y,z coordinates
* OUT: ierr   - error flag (acording to patrace coding) 
*
* OBS: geometry level (LEVGEOM) is passed by common (richgeo.inc)
*
*********************************************************************

#include "../include/richrec_lipf.h"

      INTEGER ierr
      REAL phpto(3)
      LOGICAL LEVG0,LEVG1,LEVG2,LEVG3

* ... init
      IERR = 0

*     ---------------------------------------
      IF (RCGEOM(1).NE.9) THEN  ! ===> FLIGHT 
*     ---------------------------------------

         IFLFL=0
         pitchx = pmtwx+shieldw
         pitchy = pmtwy+shieldw
*
         XLIMV=(17*PITCHX + JUMP)/2.
         XLIMH=XLIMV + DMECX 
         YLIMH=(17*PITCHY)/2.
         YLIMV= YLIMH + DMECY 

*        ----------------------------------------------------------------------------
*        GEOMETRY LEVEL
*
*        The following geometry logical flags are set to TRUE if the photon is falling
*        in a dead zone;
*
*        -----------------------------------------------------------------------------

         LEVG0 = .FALSE.
         LEVG1 = .FALSE.
         LEVG2 = .FALSE.
         LEVG3 = .FALSE.
*
* =====> LEVEL/0
*
         if ( (abs(phpto(1)).lt.XLIMH .and.        ! EMC HOLE 
     +         abs(phpto(2)).lt.YLIMV) ) LEVG0 = .TRUE.

*
* =====> LEVEL/1
*
         if ( abs(phpto(1)).lt.XLIMH+pitchx .and.         ! EMC CORNERS 
     +        abs(phpto(1)).gt.XLIMV-pitchx .and.
     +        abs(phpto(2)).lt.YLIMV+pitchy .and.
     +        abs(phpto(2)).gt.YLIMH-pitchy  ) LEVG1 = .TRUE.

*
* =====> LEVEL/2
*
         if ((phpto(2).gt.YLIMV            .and.   ! OUTSIDE DIVISION BETWEEN MODULES (TOP) 
     +        phpto(2).lt.(YLIMV+9*PITCHY) .and.
     +        phpto(1).lt.-DX0             .and.
     +        phpto(1).gt.-JUMP-DX0)     
     +       .OR.
     +       (phpto(2).lt.-YLIMV           .and.
     +        phpto(2).gt.-(YLIMV+9*PITCHY).and.
     +        phpto(1).gt. DX0             .and.
     +        phpto(1).lt. JUMP+DX0)               ! OUTSIDE DIVISION BETWEEN MODULES (BOTTOM)   
     +       .OR.       
     +       (ABS(PHPTO(2)).GT.YLIMV       .AND.
     +        ABS(PHPTO(1)).GT.XLIMV       .AND.
     +        ABS(PHPTO(1)).LT.XLIMH)              ! OUTSIDE MECH. DIV.
     +       .OR.       
     +       (ABS(PHPTO(1)).GT.XLIMH       .AND.
     +        ABS(PHPTO(2)).GT.YLIMH       .AND.
     +        ABS(PHPTO(2)).LT.YLIMV)              ! OUTSIDE MECH. DIV.  
     +      ) LEVG2 = .TRUE.

*
* =====> LEVEL/3
*
         if ( abs(phpto(1)).gt.(XLIMH+9*PITCHX) .or.        ! OUTSIDE PLANE
     +        abs(phpto(2)).gt.(YLIMV+9*PITCHY)             ! OUTSIDE PLANE
     +      ) LEVG3= .TRUE.      

         CALL PMTNMB_FLIGHT(PHPTO(1),PHPTO(2),IPMTNB,IFLFL)
         IF (IFLFL.GT.0 .OR. IPMTNB.LT.0) LEVG3 = .TRUE.
         
         IF ( (levgeom.eq.0 .and. LEVG0)  .or.
     +        (levgeom.eq.1 .and. (LEVG0.or.LEVG1))  .or.
     +        (levgeom.eq.2 .and. (LEVG0.or.LEVG1.or.LEVG2))  .or.
     +        (levgeom.eq.3 .and.(LEVG0.or.LEVG1.or.LEVG2.or.LEVG3)).or.
     +        (levgeom.eq.4 .and.(LEVG0.or.LEVG1.or.LEVG2.or.LEVG3))
     +      ) then
            ierr = 19  
         endif

*     ---------------------------------------
      else  ! ===> PROTOTYPE
*     ---------------------------------------

         IFLPR = 0
         pitchx = pmtwx+shieldw
         pitchy = pmtwy+shieldw
         XLIM=XLIMPROT
         YLIM=YLIMPROT

*        print*,'patt',xlim,ylim,xlim-9*pitchx,ylim-11*pitchy

        if(abs(phpto(1)-XCPMM).gt.xlim.or.
     +     abs(phpto(2)-ycpmm).gt.ylim
     +     )then
             IERR = 19
        else
          call pmtnmb_protn(phpto(1),phpto(2),XCNT,YCNT,IPMTNB,IFLPR)
          if(IFLPR.GT.0) THEN
             IERR = 19
*             print*,'pmtnb',ipmtnb
          endif
        endif 

*     ----------------------------------
      endif                             
*     ----------------------------------                                


* --- end
      return
      end




********************************************************
********************* NEW TRACING VERSION **************
********************************************************

* ================================================================         
      subroutine patracen(phig,thc,pthetemp,pphitemp,pvertex,vdet,vdir,
     +                    nmiref,nerr,verr)
* ================================================================         
******************************************************************         
*       AIMS: Steering routine for pattern cerenkov drawing                
*                                                                          
* PARAMETERS: IN - phig    = photon phi angle                               
*                  thc     = cerenkov angle                                 
*                  pthe    = particle polar angle
*                  phphi   = particle azimuthal angle
*                  pcervtx = photons emission point
*            OUT - xdet    = x-coord of the photon at the PMT plane         
*                  ydet    = y-coord of the photon at the PMT plane         
*                  nmiref  = number of reflections on mirror                
*                  ierr    = error flag                                     
*                                                                          
*-----------------------------------------------------------------      
* 10/09/03
*          The only difference between patracen and the old patrace
*          is the passing arguments (pthe,pphi,pvertex).
*          This way it is possible to trace photons from a different
*          emission point (than the one of richrec.inc - pcervtx) 
*          and associated to a different track than the one described 
*          by the common of richtrk.inc.
*-----------------------------------------------------------------      
* 04-Jul-2006 (R. Pereira)
*          Code changed to use original PATRACE instead of duplicate
*          code. Variables pthe/pphi/irotflg are temporarily changed
*          during this call, and are restored to their original values
*          after the routine ends.
******************************************************************

#include "../include/richrec_lipf.h"

      integer nphmax  
      parameter(nphmax=10)
      integer nerr,verr(nphmax)                                                
      real phig, thc, vdet(3),vdir(3)

      integer irotflgorig
      real ptheorig,pphiorig
      real pthetemp,pphitemp
      real pvertex(3)

* ... store particle direction in common (why???) 
      irotflgorig = irotflg
      ptheorig = pthe
      pphiorig = pphi

* ... load particle direction and allow rotation matrix
      pthe    = pthetemp
      pphi    = pphitemp
      irotflg = 0

* ... photon propagation
      call patrace(phig,thc,vdet,vdir,nmiref,nerr,verr)

      irotflg = irotflgorig
      pthe = ptheorig
      pphi = pphiorig

      return
      end

* ================================================================         
      subroutine patrace_fulln(phig,thc,ptheloc,pphiloc,pvertex,
     +                         xdet,ydet,nmiref,nerr,verr)
* ================================================================         
******************************************************************         
*       AIMS: Steering routine for pattern cerenkov drawing                
*                                                                          
* PARAMETERS: IN - phig   = photon phi angle                               
*                  thc    = cerenkov angle                                 
*            OUT - xdet   = x-coord of the photon at the PMT plane         
*                  ydet   = y-coord of the photon at the PMT plane         
*                  nmiref = number of reflections on mirror                
*                  ierr   = error flag                                     
*
*      verr(1:20) : photon error coding
*              10  - backward photon
*              11  - 
*              12  - 
*              13  - starts in plexiglas and leaving
*              14  - 
*              15  - 
*              16  - starts in plexiglas and is reflected
*              17  - 
*              18  - 
*              19  - ends in non-active detection region 
*-----------------------------------------------------------------      
* 10/09/03
*          The only difference between patrace_fulln and the old 
*          patrace_full, is the passing arguments (ptheloc,pphiloc,pvertex).
*          This way it is possible to trace photons from a different
*          emission point (than the one of richrec.inc - pcervtx) 
*          and associated to a different track than the one described 
*          by the common of richtrk.inc.
*          
******************************************************************         

#include "../include/richrec_lipf.h"

      integer nphmax
      parameter(nphmax=10)
      integer nerr,verr(nphmax)
      real phig, thc, ptheloc,pphiloc,pvertex(3), vdet(3), vdir(3)
      real pmatrix(3)

* ... get photon impact impact point and direction on detection matrix
      call patracen(phig,thc,ptheloc,pphiloc,pvertex,
     +     vdet,vdir,nmiref,nerr,verr)

      if (nerr.ne.0) return

* ... check if photon is falling in DETECTION MATRIX HOLES 
      call patrace_gcheck(vdet,ierr)
      if (ierr.ne.0) then
         nerr = nerr + 1
         verr(nerr) = ierr
      endif

* ... check particle track near ring
      if (ierr.eq.0 .and. LEVGHIT.eq.1) then
         pmatrix(1) = pvertex(1)
     +        +(zpmtdet-pvertex(3))*tan(ptheloc)*cos(pphiloc)
         pmatrix(2) = pvertex(2)
     +        +(zpmtdet-pvertex(3))*tan(ptheloc)*sin(pphiloc)
         pmatrix(3) = zpmtdet
        if ( (vdet(1)-pmatrix(1))**2+(vdet(2)-pmatrix(2))**2.lt.
     +                                            trhitass2) then
            nerr = nerr + 1
            verr(nerr) = 25
         endif 
      endif

* ... output coordinates                                                   
      xdet = vdet(1)                                                      
      ydet = vdet(2)                   
* --- end                                                                  
      return                                                               
      end                                                                             


* ==========================================================================
      subroutine patmatrn(pthetemp,pphitemp)
* ==========================================================================
*************************************************************
* 
* FIGROTM matrix calculation
* to get (x,y,z) from (x',y',z') 
*
*************************************************************
*-----------------------------------------------------------------      
* 04-Jul-2006 (R. Pereira)
*          Code changed to use original PATMATR instead of duplicate
*          code. Variables pthe/pphi are temporarily changed
*          during this call, and are restored to their original values
*          after the routine ends.
*************************************************************

#include "../include/richrec_lipf.h"

      real ptheorig,pphiorig
      real pthetemp,pphitemp

      ptheorig = pthe
      pphiorig = pphi

      pthe = pthetemp
      pphi = pphitemp

      call patmatr

      pthe = ptheorig
      pphi = pphiorig

      return
      end


********************************************************
********************* FUNCTIONS ************************
********************************************************

* ==========================================================================
      real function fphotdist(phig)
* ==========================================================================
****************************************************************************
*  It determines the distance crossed by a photon in the radiator
****************************************************************************

#include "../include/richrec_lipf.h"

      common/parfunc/par(10)
      !$OMP THREADPRIVATE(/parfunc/)
      real phpto(3),phdir(3)

* --- init
      fphotdist = -999.
      thetac=par(2)
* --- rotation matrix between detector frame and particle frame             
      if (irotflg.eq.0) then                                                
         call patmatr                                                       
         irotflg = 1                                                        
      endif 

* --- get photon cosine directors
      call patphot(phig,thetac,phdir)
      if (phdir(3).lt.0.) return

* --- get photon intersection point at radiator
      call ucopy(pcervtx,phpto,3)
      call patints(phpto,phdir,'RADB',ierr)
      if (ierr.ne.0) return

* --- set distance acrossed in radiator
      dist2 = (pcervtx(1)-phpto(1))**2 +
     +        (pcervtx(2)-phpto(2))**2 +
     +        (pcervtx(3)-phpto(3))**2 
      fphotdist = sqrt(dist2)

* --- end
      return
      end


******************************
*** CODE FROM richacpv11.F ***
******************************

***************************************************************************
*                                                                         *
* LIST OF SUBROUTINES AND FUNCTIONS INSIDE                                *
* ----------------------------------------                                *
*                                                                         *
* - subroutine richacc(thetac,richacca,richeffa,                          *
*     +                richaccmseca,richeffmseca)                         *
* - subroutine dphipath(thetac,nphitot,vphitot,fphitot,                   *
*     +              dphidir,dphihol,dphimir0,dphirmir1,dphiinv,          *
*     +              npvis,phimin,phimax,fphipat)                         *
* - subroutine set_phiroots(thetac,nphitot,vphitot,fphitot)     20/9/2001 *
* - subroutine find_phiroots(chopt,thetac,vphival,nmireft)                *
*                                                                         *
* - real Function fphilmir(phig)                                          *
* - real function fphilhole(phig)                                         *
* - real function fphilrefl(phig)                                         *
* - real Function fphilrad(phig)                                          *
* - real Function fphilgap(phig)                                          *
* - real Function HLIMSYM(xl,xh,yl,yh)                                    *
* - real Function HLIMASYM(xl,xh,yl,yh)                                   *
*   FUNCTION : to find the roots of the intersection                      *
*              betweeen the CK pattern and the detection plane            *
*                                                                         *
*   NOTE     : the non active regions have to be excluded by flaging in   *
*              patrace_full (and patrace2) in richpatvX.F
*
* ====== UTIL ROUTINES ===================================================
*
* - subroutine getphivis(thetac,npvis,phimin,phimax)
*   get photon phi's from visible paths (acording to the geometry
*                                        level chosen)
*
*
* HISTORY
* -------
* *** geometry level handled by richacc through an input parameter
* levgeo = 0 | Flight -> radiator boundaries (cilinder)
*                     -> + conical mirror
*                     -> + Ecal hole (square+corners)
*            | prototype 
*
* levgeo = 1 | Flight -> + mechanical gaps
* levgeo = 2 | Flight -> + PMT external contour 
*                                                                         *
***************************************************************************
* ======================================================================
      subroutine richacc(thetac,richacca,richeffa,
     +                   richaccmseca,richeffmseca)
* ======================================================================
****************************************************************************
*                                                                          *
* PARAMETERS:                                                              *
*         IN: thetac ........ cerenkov angle                               *
*                                                                          *
*        OUT: richacca ...... geometrical acceptances                      *
*             --------                                                     *
*               (1)= INVISIBLE (total)                                     *
*                    (includes: sideways rad exit    +                     *
*                               total reflection     +                     *
*                               radiator inner walls +                     *
*                               pmt matrix holes                           *
*               (2)= VISIBLE: PMT (direct)                                 *
*               (3)= VISIBLE: MIRROR/0 (1 refl)                            *
*               (4)= VISIBLE: MIRROR/0 (2 refl)                            *
*               (5)= HOLE (non-active detection region)                    *
*                                                                          *
*             richeffa ...... rich efficiencies                            *
*             --------                                                     *
*               (1)= radiator efficiency                                   *
*               (2)= radiator * geom acceptance                            *
*               (3)= LG and PMT                                            *
*               (4)= MEAN PHOTON VERTEX (cm)                               *
*               (5)= TOTAL EFF                                             *
*               (6)= TOTAL EFF - direct                                    *
*               (7)= TOTAL EFF - 1 reflection                              *
*               (8)= TOTAL EFF - 2 reflection                              *
*                                                                          *
*             richaccmseca & ... acceptance and efficiency                 *
*             richeffmseca       by mirror sector                          *
*                                                                          *
* Note: the HOLE acceptance is the fraction of photons falling into the    *
*       matrix non-active region; doesn't include the photons lost on      *
*       their way to the matrix                                            *
*                                                                          *
* Changed: 06/May/2009 -  R.Pereira (mirror sectors)                       *
*                                                                          *
****************************************************************************

#include "../include/richrec_lipf.h"

      common/event/ievent
      !$OMP THREADPRIVATE(/event/)
      integer nphmax,nermax,nbranchmax
      parameter(nphmax=NPHIRMAX)
      parameter(nermax=10)
      parameter(nbranchmax=20)
      integer nphitot,
     +        fphitot(nphmax),
     +        fphipat(nphmax),
     +        msecpat(nphmax),
     +        verr(nermax),
     +        npvis,ievent

      real richacca(5), richeffa(8)

      real richaccmseca(nmaxmirsec,2),richeffmseca(nmaxmirsec,8)

      real vphitot(nphmax),
     +     dphidir(nphmax),
     +     dphihol(nphmax),
     +     dphimir0(nphmax),
     +     dphimir1(nphmax),
     +     dphiwall(nphmax),
     +     dphiinv(nphmax),
     +     phimin(nphmax),
     +     phimax(nphmax),
     +     efftot(4),
     +     effrad,
     +     efflgdir,
     +     vefflgref(nbranchmax*nphmax),
     +     vfidminmir(nphmax*nbranchmax),
     +     vfidmaxmir(nphmax*nbranchmax)

      integer nphimsec0(nphmax),
     +        nphimsec1(nphmax)
      real dphimsec0(nmaxmirsec,nphmax),
     +     dphimsec1(nmaxmirsec,nphmax)
      real dphimsec0_tot(nmaxmirsec),
     +     dphimsec1_tot(nmaxmirsec)

      DOUBLE PRECISION  pardou(4),
     +                  fidmin(nphmax),
     +                  fidmax(nphmax)
      integer fphipatmsec(nphmax)

CC      REAL EPS
CC      DATA EPS /1.E-2/
      external fphotdist
      
      SAVE idummy
      integer idummy
      data idummy/0/
      !$OMP THREADPRIVATE(idummy)
      integer i, nmiref, nerr, nphiinv, nphimir0, nphimir1,
     +        nphihol, nphidir, ii
      real radacc, miracc, pmtacc, holacc, wallacc, distphtot,
     +     thetac, xdet, ydet, dphidir_tot, dphihol_tot,
     +     dphimir0_tot, dphimir1_tot, dphiinv_tot, dphitot,
     +     pnint, efflg, zoutmean 

      integer npvismsec
      double precision fidminmsec(nphmax),fidmaxmsec(nphmax)
      real pnint_ms,effrad_ms,efflg_ms,efftot_ms(4),zoutmean_ms

*      print*,'thetac in richacc = ',thetac


#ifdef LIP_DEBUG
* --- message
      if (idummy.eq.0) then
         idummy = 1
         print*,'***********************************************'
         print*,'* richacc / GEOMETRY LEVELS = ',levgeom,levgrad
         print*,'***********************************************'
      endif
#endif

* --- init
      radacc = 0.0
      miracc = 0.0
      pmtacc = 0.0
      holacc = 0.0
      wallacc = 0.0
      distphtot = 0.0   ! photon distance

* --- compute photon phi root values for diferent functions

      call set_phiroots(thetac,nphitot,vphitot,fphitot)
*
      if (ldebug.ge.2) then
         print*,' '
         print'(A)','******** PHI ROOTS *************'    
         do i=1,nphitot
           xdet = -999.
           ydet = -999.               
           call patrace_full(vphitot(i),thetac,xdet,ydet,
     +                       nmiref,nerr,verr)
           print'(A,2x,I2,2x,A,2x,F6.1,2x,A,2x,I3,2x,A,2X,2(F6.1,1x))',
     +           'NB=',i,'PHIG=',vphitot(i)*RADDEG,'FLAG=',fphitot(i),
     +           'X,Y=',xdet,ydet
         enddo
      endif

* --- compute path lengths
      call dphipath(thetac,nphitot,vphitot,fphitot,
     +              dphidir,dphihol,dphimir0,dphimir1,dphiinv,
     +              npvis,phimin,phimax,fphipat,msecpat,
     +              dphimsec0,dphimsec1)

      if (ldebug.ge.2) then     
         print'(A)','******** ACCEPTANCE ********'   
         print*,'DPHIDIR =',(RADDEG*DPHIDIR(I),I=2,INT(DPHIDIR(1))+1)
         print*,'DPHIMIR0=',(RADDEG*DPHIMIR0(I),I=2,INT(DPHIMIR0(1))+1)
         print*,'DPHIMIR1=',(RADDEG*DPHIMIR1(I),I=2,INT(DPHIMIR1(1))+1)
         print*,'DPHIHOL =',(RADDEG*DPHIHOL(I),I=2,INT(DPHIHOL(1))+1) 
         print*,'DPHIINV =',(RADDEG*DPHIINV(I),I=2,INT(DPHIINV(1))+1)
      endif

* --- compute acceptances
      DPHIDIR_tot  = 0.0
      DPHIHOL_tot  = 0.0
      DPHIMIR0_tot = 0.0
      DPHIMIR1_tot = 0.0
      DPHIINV_tot  = 0.0

      do i=1,nmaxmirsec
         dphimsec0_tot(i) = 0.0
         dphimsec1_tot(i) = 0.0
      enddo
*
      nphiinv  = int(dphiinv(1))
      nphimir0 = int(dphimir0(1))
      nphimir1 = int(dphimir1(1))
      nphihol  = int(dphihol(1))
      nphidir  = int(dphidir(1))

      do i=1,nmaxmirsec
         nphimsec0(i) = int(dphimsec0(i,1))
         nphimsec1(i) = int(dphimsec1(i,1))
      enddo
*
      do i=1,nphiinv
         ii = i + 1                            
         dphiinv_tot = dphiinv_tot + dphiinv(ii) 
      enddo
      do i=1,nphimir0                           
         ii = i + 1                             
         dphimir0_tot = dphimir0_tot + dphimir0(ii) 
      enddo                                                              
      do i=1,nphimir1                           
         ii = i + 1                             
         dphimir1_tot = dphimir1_tot + dphimir1(ii) 
      enddo                                   
      do i=1,nphihol                           
         ii = i + 1                             
         dphihol_tot = dphihol_tot + dphihol(ii) 
      enddo                                
      do i=1,nphidir                           
         ii = i + 1                             
         dphidir_tot = dphidir_tot + dphidir(ii)        
      enddo                  

      do i=1,nmaxmirsec
*         print*,'Sector ',i,' - ',
*     +        nphimsec0(i),'+',nphimsec1(i),' segments'
         do j=1,nphimsec0(i)
            jj = j+1
            dphimsec0_tot(i) = dphimsec0_tot(i)+dphimsec0(i,jj)
         enddo
         do j=1,nphimsec1(i)
            jj = j+1
            dphimsec1_tot(i) = dphimsec1_tot(i)+dphimsec1(i,jj)
         enddo
      enddo

* --- control
      dphitot = dphiinv_tot +
     +          dphimir0_tot+
     +         dphimir1_tot+
     +          dphidir_tot
     
      if (ldebug.ge.2) then
         print*,'(richacc) Acceptance / TOTAL :',
     & (DPHITOT-dphiinv_tot)/twopi,' /',DPHITOT/twopi
      endif
            
* --- compute acceptances
      richacca(1) = dphiinv_tot/twopi
      richacca(2) = dphidir_tot/twopi
      richacca(3) = dphimir0_tot/twopi
      richacca(4) = dphimir1_tot/twopi
      richacca(5) = dphihol_tot/twopi

      do i=1,nmaxmirsec
         richaccmseca(i,1) = dphimsec0_tot(i)/twopi
         richaccmseca(i,2) = dphimsec1_tot(i)/twopi
      enddo

*      print*,'ACCEPTANCE RESULTS:'
*      do i=1,5
*         print*,'richacca(',i,') = ',richacca(i)
*      enddo
*      do i=1,nmirsec
*         do j=1,2
*            print*,'richaccmseca(',i,',',j,') = ',richaccmseca(i,j)
*         enddo
*      enddo

* --- compute efficiencies
      pardou(1) = dble(HRAD)
      pardou(2) = dble(pthe)
      pardou(3) = dble(thetac)
      pardou(4) = dble(radint)

      do i=1,npvis     
         fidmin(i) = dble(phimin(i))
         fidmax(i) = dble(phimax(i))
      enddo
*
      if (dphitot-dphiinv_tot.gt.0) then

         if (ldebug.ge.2) then
            call rcradnint(pardou,npvis,fidmin,fidmax,fphipat,pnint)
            call rcradeff(pardou,npvis,fidmin,fidmax,fphipat,effrad)
            call rcpmteff(thetac,npvis,fidmin,fidmax,efflg)
            call zradmean(pardou,npvis,fidmin,fidmax,fphipat,
     +                    zoutmean) 
cc            nbranch=5
cc            call rclgeff(thetac,nbranch,nbmir,vfidminmir,vfidmaxmir
cc     &        ,vefflgref,efflgdir)
         else
            pnint  = -1.
            effrad = -1.
            efflg  = -1.
            zoutmean = -1.
         endif

C --- compute global efficiencies
cc         print*,'[richacc] npvis = ',npvis
         call rctoteff(pardou,npvis,fidmin,fidmax,fphipat,efftot)
        
         richeffa(1) = pnint
         richeffa(2) = effrad
         richeffa(3) = efflg
         richeffa(4) = zoutmean  
         richeffa(5) = efftot(1)
         richeffa(6) = efftot(2)
         richeffa(7) = efftot(3)
         richeffa(8) = efftot(4)

         if (ldebug.ge.2) print*,'richeffa = ',(richeffa(k),k=1,8)

* ...... efficiency for each mirror sector

*         do j=1,npvis
*            print*,'msecpat(',j,') = ',msecpat(j)
*         enddo

         do i=1,nmirsec

            npvismsec = 0
            call vzero(fidminmsec,nphmax)
            call vzero(fidmaxmsec,nphmax)
            call vzero(fphipatmsec,nphmax)

            do j=1,npvis
               if(msecpat(j).eq.i) then
                  npvismsec = npvismsec+1
                  fidminmsec(npvismsec) = dble(phimin(j))
                  fidmaxmsec(npvismsec) = dble(phimax(j))
                  fphipatmsec(npvismsec) = fphipat(j)
               endif
            enddo

*            print*,'Sector ',i,': ',(fphipatmsec(j),j=1,npvismsec)

            if(npvismsec.gt.0) then
               if (ldebug.ge.2) then
                  call rcradnint(pardou,npvismsec,fidminmsec,fidmaxmsec,
     +                 fphipatmsec,pnint_ms)
                  call rcradeff(pardou,npvismsec,fidminmsec,fidmaxmsec,
     +                 fphipatmsec,effrad_ms)
                  call rcpmteff(thetac,npvismsec,fidminmsec,fidmaxmsec,
     +                 efflg_ms)
                  call zradmean(pardou,npvismsec,fidminmsec,fidmaxmsec,
     +                 fphipatmsec,zoutmean_ms)
               else
                  pnint_ms  = -1.
                  effrad_ms = -1.
                  efflg_ms  = -1.
                  zoutmean_ms = -1.

            endif


            call rctoteff(pardou,npvismsec,fidminmsec,fidmaxmsec,
     +           fphipatmsec,efftot_ms)


               richeffmseca(i,1) = pnint_ms
               richeffmseca(i,2) = effrad_ms
               richeffmseca(i,3) = efflg_ms
               richeffmseca(i,4) = zoutmean_ms  
               richeffmseca(i,5) = efftot_ms(1)
               richeffmseca(i,6) = efftot_ms(2)
               richeffmseca(i,7) = efftot_ms(3)
               richeffmseca(i,8) = efftot_ms(4)

            else

               do j=1,8
                  richeffmseca(i,j) = 0.
               enddo

            endif

*            print*,'- richeffmseca(',i,') = ',(richeffmseca(i,k),k=1,8)

         enddo

      endif 

* --- debugging print
      if (ldebug.ge.2) then
         print*,' ******** RADIATOR EFFICIENCY ******** '
         print*,'nb of visible paths=',NPVIS
         print*,'PHIMIN=',(PHIMIN(I),I=1,NPVIS)
         print*,'PHIMAX=',(PHIMAX(I),I=1,NPVIS)
         print*,'PHIFLG=',(FPHIPAT(I),I=1,NPVIS)
         print*,'---------------------------------'
         print*,'PNINTR =',richeffa(1)
         print*,'EFFRAD =',richeffa(2)
         print*,'EFFLGD =',richeffa(3)
         print*,'EFFTOT =',richeffa(5)
      endif

* --- return
      return
      end     

*=================================================================
      subroutine dphipath(thetac,nphitot,vphitot,fphitot,
     +                    dphidir,dphihol,dphimir0,dphimir1,dphiinv,
     +                    npvis,phimin,phimax,fphipat,msecpat,
     +                    dphimsec0,dphimsec1)
*=================================================================
******************************************************************
*     It computes phi differences
*  
*     PARAMETERS:
*             IN: thetac  - cerenkov angle
*                 nphitot - nb of phi roots
*                 vphitot - phi values
*                 fphitot - phi flags (see set_phiroots for meaning)
*            OUT: dphidir  - dphi of direct paths                  |
*                 dphimir0 - dphi of reflected paths (1st order)   > visible paths
*                 dphimir1 - dphi of reflected paths (2nd order)   |
*                 dphiinv  - dphi of invisible paths (all kind)
*                 dphihol  - dphi of matrix hole incident paths
*                 npvis    - nb of visible paths
*                 phimin   - phi lower limit of visible path
*                 phimax   - phi upper limit of visible path
*                 fphipat  - flag =0 non-reflected path
*                                 =1 reflected path (1st order)
*                                 =2 reflected path (2nd order)
*                 msecpat  - mirror sector (1...N) for reflected paths
*                            (0 if not reflected)
*                 dphimsec0 - dphi of reflected paths by mirror sector (1st order)
*                 dphimsec1 - dphi of reflected paths by mirror sector (2nd order)
*
*  Revised: 28/May/2004
*           06/May/2009 - R. Pereira (mirror sectors)
*
******************************************************************

#include "../include/richrec_lipf.h"

      integer nphmax,nermax 
      parameter(nphmax=NPHIRMAX)
      parameter(nermax=10)
      real vphitot(nphmax),
     +     dphidir(nphmax),
     +     dphihol(nphmax),
     +     dphimir0(nphmax),
     +     dphimir1(nphmax),
     +     dphiinv(nphmax),
     +     dphigup(nphmax),
     +     dphigdw(nphmax),
     +     dphitrk(nphmax),
     +     phimin(nphmax),
     +     phimax(nphmax)
      real dphimsec0(nmaxmirsec,nphmax),
     +     dphimsec1(nmaxmirsec,nphmax),
     +     phiminmsec0(nmaxmirsec,nphmax),
     +     phiminmsec1(nmaxmirsec,nphmax),
     +     phimaxmsec0(nmaxmirsec,nphmax),
     +     phimaxmsec1(nmaxmirsec,nphmax)
      integer fphitot(nphmax),
     +        fphipat(nphmax),
     +        msecpat(nphmax),
     +        verr(nermax),
     +        npvis, nphitot
      real EPS
      SAVE EPS
      DATA EPS /1.E-2/

      !$OMP THREADPRIVATE(EPS)

      integer nroot_rad,
     +        nroot_reft, 
     +        nroot_mirr,
     +        nroot_hole,
     +        nroot_trk
      external fphotdist
     
      integer nphidir, nphihol, nphimir0, nphimir1, nphiinv,
     +        ndum, nerr, i, ipc, ipn, ioffset
      real thetac, xdet, ydet, phimean

      integer nphimsec0(nmaxmirsec),
     +        nphimsec1(nmaxmirsec)

      real phimir
      integer imsec

* --- init
      call vfill(dphidir,nphmax,0.)
      call vfill(dphihol,nphmax,0.)
      call vfill(dphimir0,nphmax,0.)
      call vfill(dphimir1,nphmax,0.)
      call vfill(dphiinv,nphmax,0.)
      call vfill(dphigup,nphmax,0.)
      call vfill(dphigdw,nphmax,0.)
      call vfill(dphitrk,nphmax,0.)
      call vfill(dphimsec0,nmaxmirsec*nphmax,0.)
      call vfill(dphimsec1,nmaxmirsec*nphmax,0.)
*
      nphidir  = 1 !nb of path lengths
      nphihol  = 1
      nphimir0 = 1
      nphimir1 = 1
      nphiinv  = 1 !ver adiante que este 1 e subtraido
      nphigup  = 1
      nphigdw  = 1
      nphitrk  = 1
      npvis    = 0 !nb of visible path lengths

      do i=1,nmaxmirsec
         nphimsec0(i) = 1
         nphimsec1(i) = 1
      enddo

      /*
* --- error case (odd number of roots)
      nroot_rad  = 0
      nroot_reft = 0
      nroot_mirr = 0
      nroot_hole = 0      
      nroot_trk  = 0      
      do i=1,nphitot
         if (fphitot(i).eq.1 .or. int(fphitot(i)/10).eq.1)
     +                         nroot_rad  = nroot_rad  + 1
         if (fphitot(i).eq.2 .or. int(fphitot(i)/10).eq.2)
     +                         nroot_reft = nroot_reft + 1
         if (fphitot(i).eq.3 .or. int(fphitot(i)/10).eq.3)
     +                         nroot_mirr = nroot_mirr + 1
         if (fphitot(i).eq.4 .or. int(fphitot(i)/10).eq.4)
     +                         nroot_hole = nroot_hole + 1
         if (int(fphitot(i)/10).eq.6)
     +                         nroot_trk  = nroot_trk  + 1
      enddo
      if (nroot_rad.ne.0 .and. mod(nroot_rad,2).ne.0)
     +     print*,'(dphipath) Odd source: Radiator ',nroot_rad
      if (nroot_reft.ne.0 .and. mod(nroot_reft,2).ne.0)
     +     print*,'(dphipath) Odd source: Total Reflection ',nroot_reft
      if (nroot_mirr.ne.0 .and. mod(nroot_mirr,2).ne.0)
     +     print*,'(dphipath) Odd source: Mirror ',nroot_mirr
      if (nroot_hole.ne.0 .and. mod(nroot_hole,2).ne.0)
     +     print*,'(dphipath) Odd source: Hole ',nroot_hole
      if (nroot_trk.ne.0 .and. mod(nroot_trk,2).ne.0)
     +     print*,'(dphipath) Odd source: Track zone ',nroot_hole
      */

* --- zero roots case         
      if (nphitot.eq.0) then  
         call patrace_full_phimir
     +        (0.,thetac,xdet,ydet,ndum,nerr,verr,phimir)
        if (nerr.ne.0) then /* -------------> all ring invisible */
           nphiinv = nphiinv + 1
           dphiinv(nphiinv)= twopi
           if (verr(1).eq.19.and.rcgeom(1).ne.9) then
              nphihol = nphihol +1
              dphihol(nphihol)= twopi
           endif
        else                                            
           if (ndum.eq.0) then /* ----------> all ring DIRECT */
              nphidir = nphidir + 1
              dphidir(nphidir) = twopi
              npvis = npvis + 1
              phimin(npvis) = 0.
              phimax(npvis) = twopi
              fphipat(npvis) = 0
              msecpat(npvis) = 0
           else

              call getmirsec(phimir,imsec) 

             if (ndum.eq.1) then /* ----------> all ring ONCE REFLECTED */
              if((imsec.lt.1).or.(imsec.gt.nmirsec)) then
                 print*,'(dphipath) Wrong mirror sector number! - ',
     +                imsec
              endif
              nphimir0 = nphimir0 + 1
              dphimir0(nphimir0) = twopi
              npvis = npvis + 1
              phimin(npvis) = 0.
              phimax(npvis) = twopi
              fphipat(npvis) = 1
              msecpat(npvis) = imsec
              nphimsec0(imsec) = nphimsec0(imsec)+1
              dphimsec0(imsec,nphimsec0(imsec)) = twopi
              phiminmsec0(imsec,nphimsec0(imsec)) = 0.
              phimaxmsec0(imsec,nphimsec0(imsec)) = twopi
             elseif (ndum.eq.2) then
              if((imsec.lt.1).or.(imsec.gt.nmirsec)) then
                 print*,'(dphipath) Wrong mirror sector number! - ',
     +                imsec
              endif
              nphimir1 = nphimir1 + 1
              dphimir1(nphimir1) = twopi
              npvis = npvis + 1
              phimin(npvis) = 0.
              phimax(npvis) = twopi
              fphipat(npvis) = 1
              msecpat(npvis) = imsec
              nphimsec1(imsec) = nphimsec1(imsec)+1
              dphimsec1(imsec,nphimsec1(imsec)) = twopi
              phiminmsec1(imsec,nphimsec1(imsec)) = 0.
              phimaxmsec1(imsec,nphimsec1(imsec)) = twopi
             endif
           endif
        endif
      endif

* --- loop on phi roots
      do i=1,nphitot                                                        
* >>>>>> POINTERS                                                        
         ipc = i                ! current hit
         if (i.lt.nphitot) then ! next hit
            ipn = i+1    
            phimean = 0.5*(vphitot(ipn)+vphitot(ipc))
         else                                                               
            ipn = 1                                                         
            phimean = 0.5*((vphitot(ipn)+twopi)+vphitot(ipc))
         endif     
*         print*,'IPC,IPN ----->',vphitot(ipc)*raddeg,vphitot(ipn)*raddeg
*     +           ,phimean*raddeg
* >>>>>> TEST portion                                                       
         call patrace_full_phimir
     +        (phimean,thetac,xdet,ydet,ndum,nerr,verr,phimir)
*         print*,'nerr,ndum,phimir = ',nerr,ndum,phimir

*>>>>>>> visible
         if (nerr.eq.0) then

            if (ndum.eq.0) then  !direct

               nphidir=nphidir+1
               dphidir(nphidir)= vphitot(ipn)-vphitot(ipc)    
               if (dphidir(nphidir).lt.0.)                              
     +              dphidir(nphidir) = twopi-abs(dphidir(nphidir))
               npvis = npvis + 1                           
               phimin(npvis) = vphitot(ipc)                
               phimax(npvis) = vphitot(ipn)                
               fphipat(npvis) = 0     
               msecpat(npvis) = 0

            else

               call getmirsec(phimir,imsec)

               if (ndum.eq.1) then !with one mirror reflection
                if ((imsec.lt.1).or.(imsec.gt.nmirsec)) then
                  print*,'(dphipath) Wrong mirror sector number! - ',
     +                 imsec,' for angle ',phimir
                endif
                nphimir0=nphimir0+1
                dphimir0(nphimir0)= vphitot(ipn)-vphitot(ipc)
                if (dphimir0(nphimir0).lt.0.)                              
     +            dphimir0(nphimir0) = twopi-abs(dphimir0(nphimir0))
                npvis = npvis + 1                           
                phimin(npvis) = vphitot(ipc)                
                phimax(npvis) = vphitot(ipn)                
                fphipat(npvis) = 1     
                msecpat(npvis) = imsec
                nphimsec0(imsec) = nphimsec0(imsec)+1
                dphimsec0(imsec,nphimsec0(imsec))
     +              = vphitot(ipn)-vphitot(ipc)
                if (dphimsec0(imsec,nphimsec0(imsec)).lt.0.)
     +              dphimsec0(imsec,nphimsec0(imsec))
     +              = twopi-abs(dphimsec0(imsec,nphimsec0(imsec)))
                phiminmsec0(imsec,nphimsec0(imsec)) = vphitot(ipc)
                phimaxmsec0(imsec,nphimsec0(imsec)) = vphitot(ipn)
               elseif(ndum.eq.2)then  !with two mirror reflections
                if((imsec.lt.1).or.(imsec.gt.nmirsec)) then
                  print*,'(dphipath) Wrong mirror sector number! - ',
     +                 imsec
                endif
                nphimir1=nphimir1+1
                dphimir1(nphimir1)= vphitot(ipn)-vphitot(ipc)    
                if (dphimir1(nphimir1).lt.0.)                              
     +              dphimir1(nphimir1) = twopi-abs(dphimir1(nphimir1))
                npvis = npvis + 1                           
                phimin(npvis) = vphitot(ipc)                
                phimax(npvis) = vphitot(ipn)                
                fphipat(npvis) = 2     
                msecpat(npvis) = imsec
                nphimsec1(imsec) = nphimsec1(imsec)+1
                dphimsec1(imsec,nphimsec1(imsec))
     +              = vphitot(ipn)-vphitot(ipc)
                if (dphimsec1(imsec,nphimsec1(imsec)).lt.0.)
     +              dphimsec1(imsec,nphimsec1(imsec))
     +              = twopi-abs(dphimsec1(imsec,nphimsec1(imsec)))
                phiminmsec1(imsec,nphimsec1(imsec)) = vphitot(ipc)
                phimaxmsec1(imsec,nphimsec1(imsec)) = vphitot(ipn)
               endif
            endif

*>>>>>>> invisible
         else

           nphiinv = nphiinv + 1                                       
           dphiinv(nphiinv)=vphitot(ipn)-vphitot(ipc)            
           if (dphiinv(nphiinv).lt.0.)                                 
     +     dphiinv(nphiinv) = twopi-abs(dphiinv(nphiinv))
*           if(verr(1).eq.19.and.rcgeom(1).ne.9)then
           if(verr(1).eq.19)then

/********************************************* COMMENTED
              if(LEVGRAD.eq.1)then
                 if(fphitot(ipc).ne.11.and.fphitot(ipn).ne.11)then
                    nphihol = nphihol + 1                                       
                    dphihol(nphihol) = vphitot(ipn)-vphitot(ipc)
                 elseif(fphitot(ipc).eq.11.and.fphitot(ipn).eq.11
     +                  .and.nphitot.eq.2)then
                    nphihol = nphihol + 1
                    dphihol(nphihol)=twopi
                 endif
*                 print*,'----->dphihol',dphihol
              else
                 nphihol = nphihol + 1                                       
                 dphihol(nphihol) = vphitot(ipn)-vphitot(ipc)
              endif
**********************************************/
             nphihol = nphihol + 1                                       
             dphihol(nphihol) = vphitot(ipn)-vphitot(ipc)
              if (dphihol(nphihol).lt.0.)                                 
     +             dphihol(nphihol) = twopi-abs(dphihol(nphihol))
           elseif(verr(1).eq.20)then
              nphigup = nphigup + 1
              dphigup(nphigup) = vphitot(ipn)-vphitot(ipc)
              if (dphigup(nphigup).lt.0.)dphigup(nphigup) = twopi
     &             -abs(dphigup(nphigup))  
           elseif(verr(1).eq.21)then
              nphigdw = nphigdw + 1            
              dphigdw(nphigdw) = vphitot(ipn)-vphitot(ipc)
              if (dphigdw(nphigdw).lt.0.)dphigdw(nphigdw) = twopi
     &             -abs(dphigdw(nphigdw))  
           elseif(verr(1).eq.25)then
              nphitrk = nphitrk + 1            
              dphitrk(nphitrk) = vphitot(ipn)-vphitot(ipc)
              if (dphitrk(nphitrk).lt.0.)dphitrk(nphitrk) = twopi
     &             -abs(dphitrk(nphitrk))  
           endif
        endif

      enddo

* --- check phi interval limits (split in case of phimax<phimin)
      ioffset = 0
      do i=1,npvis
         if (phimax(i).lt.phimin(i)) then
            ioffset = ioffset + 1
            phimin(npvis+ioffset) = 0.
            phimax(npvis+ioffset) = phimax(i)
            fphipat(npvis+ioffset) = fphipat(i)
            msecpat(npvis+ioffset) = msecpat(i)
            phimax(i) = twopi
         endif
      enddo
      npvis = npvis + ioffset
      if (ioffset.gt.1)
     +print*,'(dphipath) phi intervals splitted >1!!!!!!!!!'

      do i=1,nmirsec
         ioffset = 0
         do j=2,nphimsec0(i)
            if(phimaxmsec0(i,j).lt.phiminmsec0(i,j)) then
               ioffset = ioffset + 1
               phiminmsec0(i,nphimsec0(i)+ioffset) = 0.
               phimaxmsec0(i,nphimsec0(i)+ioffset) = phimaxmsec0(i,j)
               phimaxmsec0(i,j) = twopi
            endif
         enddo
         nphimsec0(i) = nphimsec0(i)+ioffset
         if (ioffset.gt.1)
     +        print*,'(dphipath - msec0 ',i,
     +        ') phi ints splitted >1 !!!!!!!!!'
      enddo

      do i=1,nmirsec
         ioffset = 0
         do j=2,nphimsec1(i)
            if(phimaxmsec1(i,j).lt.phiminmsec1(i,j)) then
               ioffset = ioffset + 1
               phiminmsec1(i,nphimsec1(i)+ioffset) = 0.
               phimaxmsec1(i,nphimsec1(i)+ioffset) = phimaxmsec1(i,j)
               phimaxmsec1(i,j) = twopi
            endif
         enddo
         nphimsec1(i) = nphimsec1(i)+ioffset
         if (ioffset.gt.1)
     +        print*,'(dphipath - msec1 ',i,
     +        ') phi ints splitted >1 !!!!!!!!!'
      enddo

* --- combine information
      nphidir  = nphidir-1       
      nphihol  = nphihol-1       
      nphimir0 = nphimir0-1       
      nphimir1 = nphimir1-1       
      nphiinv  = nphiinv-1       
      dphidir(1)  = real(nphidir)
      dphimir0(1) = real(nphimir0)
      dphimir1(1) = real(nphimir1)
      dphiinv(1)  = real(nphiinv)
      dphihol(1)  = real(nphihol)

      do i=1,nmaxmirsec
         nphimsec0(i) = nphimsec0(i)-1
         nphimsec1(i) = nphimsec1(i)-1
         dphimsec0(i,1) = real(nphimsec0(i))
         dphimsec1(i,1) = real(nphimsec1(i))
      enddo
*
      return
      end

*===================================================================
      subroutine set_phiroots(thetac,nphitot,vphitot,fphitot)
*===================================================================
*
*   It computes limits of intervals on phi geratrix variable of
*   Cherenkov Photons :
*   Intervals defining diferent situations:
*
*      - deltaphi where photons leave radiator by side (instead from bottom)
*      - deltaphi where total reflection inside radiator/PGL layer occurs
*      - deltaphi where photons are reflected  by conical mirror (1 or 2 times)
*      - deltaphi where photons reach EMC hole
*      - deltaphi  .....   ...    ... dead pmts 
*
*   For that purpose several functions have been constructed for which
*   roots give these limits:
*                            fphilmir(phi)  
*                            fphilhole(phi) 
*                            fphilrefl(phi) 
*                            fphilrad(phi)  
*     
* --------------------------------------------------------------------
*
* PARAMETERS:
*         IN: thetac  - cerenkov theta
*        OUT: nphitot - number of phi roots
*             vphitot - phi values
*             fphitot - phi flags :
*                       1  - sideways radiator exit
*                       11 - radiator inner walls
*                       2  - total reflection 
*                       31 - mirror (1st order)
*                       32 - mirror (2nd order)
*                       33 - mirror prototype
*                       40 - out of flight matrix without reflection
*                       41 - out of flight matrix with    reflection
*                       45 - out of prototype matrix or dead PMT
*                       51 - sideways upper mirror gap exit
*                       52 - sideways lower mirror gap exit
*                       60 - particle track zone without reflection
*                       61 - particle track zone with    reflection
*                       70 - (TBD?: 71...80) - mirror sector limits 1...10
*
* Revised : 28/May/2004 (F.Barao)
*           17/Nov/2008 (R.Pereira - track zone)
*           06/May/2009 (R.Pereira - mirror sectors)
*       
********************************************************************

#include "../include/richrec_lipf.h"

      integer nphmax,nermax 
      parameter(nphmax=NPHIRMAX)
      parameter(nermax=10)

      integer nphitot,                   !total nb roots
     +        fphitot(nphmax),           !root code
     +        index(nphmax),
     +        fphitotbuf(nphmax)

      real    vphitot(nphmax),           !root values
     +        vphitotbuf(nphmax),
     +        vphiprotbuf(nphmax),
     +        vphiprotint(nphmax),
     +        vphirad(nphmax),
     +        vphigapup(nphmax),
     +        vphimir(nphmax),
     +        vphigapdw(nphmax),
     +        vphiref(nphmax),
     +        vphihol(nphmax),
     +        vphiprot(nphmax),
     +        vphiwall(nphmax),
     +        vphilev(nphmax),
     +        vphitrk(nphmax),
     +        vphimsec(nphmax)

      integer fphilev(nphmax)

      integer verr(nermax)

      character*4 chgeom(4)

      real philhol(nphmax)
      integer nholt,nrefhol(nphmax)
      common/holevar/nholt,philhol,nrefhol
      !$OMP THREADPRIVATE(/holevar/)
      integer ndum, ndum0, ndum1, ndum2, ndum3, ntemptot

      real thetac
      integer npointer, ipointer, ng, i, nrprot, nprot

      integer nph 
      parameter(nph=10)
      integer nerr,verro(nph)                                               
      real par, phdir(3), phpto(3),phdirn(3), vdet(3),vdir(3)
      common /parfunc/ par(10)                                             
      !$OMP THREADPRIVATE(/parfunc/)
*      print*,'set_phiroots called'
* --- init
      nphitot = 0
      call vfill(vphitot,nphmax,0.)
      call vfill(vphitotbuf,nphmax,0.)

* --- compute radiator sideways phi roots                   

      call find_phiroots('RADT',thetac,vphirad,ndum0)                         
*      print*,'roots(RADT) = ',int(vphirad(1))

      if ( int(vphirad(1)).ne.0) then
         ntemptot = nphitot+int(vphirad(1))
         if (ntemptot.le.nphmax ) then             
            call ucopy(vphirad(2),vphitot(nphitot+1),int(vphirad(1)))      
            call vfill(fphitot(nphitot+1),int(vphirad(1)),1)               
            nphitot = nphitot + int(vphirad(1))                            
         else
            print*,'(set_phiroots) nb phi roots to store (',ntemptot,
     +           ') greater than ',nphmax,' option RADT'
         endif
      endif                                                             

* --- compute radiator inner walls roots
  
      if (LEVGRAD.eq.1) then

         call find_phiroots('RADW',thetac,vphirad,ndum0)                   
*         print*,'roots(RADW) = ',int(vphirad(1))

         if (int(vphirad(1)).ne.0) then    
            ntemptot = nphitot+int(vphirad(1))                           
            if (ntemptot.le.nphmax ) then                  
               call ucopy(vphirad(2),vphitot(nphitot+1),int(vphirad(1)))
               call vfill(fphitot(nphitot+1),int(vphirad(1)),11)               
               nphitot = nphitot + int(vphirad(1))                            
            else
               print*,'(set_phiroots) nb phi roots to store (',ntemptot,
     +              ') greater than ',nphmax,' option RADW'
            endif  
         endif

      endif                                                                           
                                      
* --- compute total reflection phi roots                                     

      call find_phiroots('REFL',thetac,vphiref,ndum1)                         
*      print*,'roots(REFL) = ',int(vphiref(1))

      if (int(vphiref(1)).ne.0) then                                    
         ntemptot = nphitot+int(vphiref(1))                           
         if (ntemptot.le.nphmax ) then                  
            call ucopy(vphiref(2),vphitot(nphitot+1),int(vphiref(1)))      
            call vfill(fphitot(nphitot+1),int(vphiref(1)),2)               
            nphitot = nphitot + int(vphiref(1))                          
         else
            print*,'(set_phiroots) nb phi roots to store (',ntemptot,
     +           ') greater than ',nphmax,' option REFL'
         endif  
      endif                                                             

* --- compute upper mirror gap sideways phi roots                   
      if ( rcgeom(1).eq.0 ) then                      !flight configuration
         call find_phiroots('GAPU',thetac,vphigapup,ndum0)    
*         print*,'roots(GAPU) = ',int(vphigapup(1))

         if ( int(vphigapup(1)).ne.0) then
            ntemptot = nphitot+int(vphigapup(1))
            if (ntemptot.le.nphmax ) then             
               call ucopy(vphigapup(2),vphitot(nphitot+1)
     &              ,int(vphigapup(1)))      
               call vfill(fphitot(nphitot+1),int(vphigapup(1)),51)
               nphitot = nphitot + int(vphigapup(1))
            else
               print*,'(set_phiroots) nb phi roots to store (',ntemptot,
     +              ') greater than ',nphmax,' option RADT'
            endif
         endif
      endif      
       
* --- compute mirror phi roots   

      if ( rcgeom(1).eq.0 ) then                      !flight configuration


         call find_phiroots('MIR0',thetac,vphimir,ndum2) !1st reflection
*         print*,'roots(MIR0) = ',int(vphimir(1))

         if (int(vphimir(1)).ne.0) then                 
            ntemptot = nphitot+int(vphimir(1))                           
            if (ntemptot.le.nphmax ) then                                     
               call ucopy(vphimir(2),vphitot(nphitot+1),int(vphimir(1)))      
               call vfill(fphitot(nphitot+1),int(vphimir(1)),31)               
               nphitot = nphitot + int(vphimir(1))                            
            else
               print*,'(set_phiroots) nb phi roots to store (',ntemptot,
     +              ') greater than ',nphmax,' option MIR0'
            endif  
         endif                                             
*         print*,'vphimir',vphimir
         call find_phiroots('MIR1',thetac,vphimir,ndum2) !2nd reflection
*         print*,'roots(MIR1) = ',int(vphimir(1))

         if (int(vphimir(1)).ne.0) then                                    
            ntemptot = nphitot+int(vphimir(1))                           
            if (ntemptot.le.nphmax ) then                                     
               call ucopy(vphimir(2),vphitot(nphitot+1),int(vphimir(1)))      
               call vfill(fphitot(nphitot+1),int(vphimir(1)),32)               
               nphitot = nphitot + int(vphimir(1))
            else
               print*,'(set_phiroots) nb phi roots to store (',ntemptot,
     +              ') greater than ',nphmax,' option MIR1'                            
            endif      
         endif                
*         print*,'vphimir',vphimir

      elseif (rcgeom(1).eq.9.and.rcgeom(2).eq.1) then !prototype with mirror

         call find_phiroots('MIR0',thetac,vphimir,ndum2) !1st reflection
*         print*,'roots(MIR0) = ',int(vphimir(1))

         if (int(vphimir(1)).ne.0) then                 
            ntemptot = nphitot+int(vphimir(1))                           
            if (ntemptot.le.nphmax ) then                                     
               call ucopy(vphimir(2),vphitot(nphitot+1),int(vphimir(1)))      
               call vfill(fphitot(nphitot+1),int(vphimir(1)),31)               
               nphitot = nphitot + int(vphimir(1))                            
            else
               print*,'(set_phiroots) nb phi roots to store (',ntemptot,
     +              ') greater than ',nphmax,' option MIR0'
            endif  
         endif                            

         call find_phiroots('MPRT',thetac,vphimir,ndum2) !calculate the intersection with the mirror limits
*         print*,'roots(MPRT) = ',int(vphimir(1))

         if (int(vphimir(1)).ne.0) then
            ntemptot = nphitot+int(vphimir(1))                           
            if (ntemptot.le.nphmax ) then                                     
               call ucopy(vphimir(2),vphitot(nphitot+1),int(vphimir(1)))      
               call vfill(fphitot(nphitot+1),int(vphimir(1)),33)
               nphitot = nphitot + int(vphimir(1)) 
            else
               print*,'(set_phiroots) nb phi roots to store (',ntemptot,
     +              ') greater than ',nphmax,' option MPRT'                            
            endif      
         endif

      endif

* --- compute lower mirror gap sideways phi roots                   
      if ( rcgeom(1).eq.0 ) then                      !flight configuration
         call find_phiroots('GAPD',thetac,vphigapdw,ndum0)    
*         print*,'roots(GAPD) = ',int(vphigapdw(1))

         if ( int(vphigapdw(1)).ne.0) then
            ntemptot = nphitot+int(vphigapdw(1))
            if (ntemptot.le.nphmax ) then             
               call ucopy(vphigapdw(2),vphitot(nphitot+1)
     &              ,int(vphigapdw(1)))      
               call vfill(fphitot(nphitot+1),int(vphigapdw(1)),52)
               nphitot = nphitot + int(vphigapdw(1))
            else
               print*,'(set_phiroots) nb phi roots to store (',ntemptot,
     +              ') greater than ',nphmax,' option RADT'
            endif
         endif
      endif      
      
* --- PMT Matrix roots 
      if (rcgeom(1).ne.9) then  !flight configuration

         chgeom(1) = 'HEMC'
         chgeom(2) = 'HCOR'
         chgeom(3) = 'HGAP'
         chgeom(4) = 'HPMT'

         npointer = 0
         ipointer = 0                                             

         do ng=1,levgeom+1 !take into account different geometry levels
            if(ng.gt.4)goto 55
            call find_phiroots(chgeom(ng),thetac,vphihol,ndum3)
*            print*,'roots(',chgeom(ng),') = ',int(vphihol(1))

            do i=1,int(vphihol(1))                                           
               ipointer = ipointer + 1                                        
               vphilev(ipointer) = abs(vphihol(i+1))                          
               if (vphihol(i+1).lt.0.) then                                   
                  fphilev(ipointer) = 41                                     
               else                                                           
                  fphilev(ipointer) = 40                                     
               endif                                                          
            enddo                                                             
            npointer =  npointer + int(vphihol(1)) 

         enddo
 55      continue
*
         ntemptot = nphitot+npointer
         if (ntemptot.le.nphmax ) then                                     
            call ucopy(vphilev(1),vphitot(nphitot+1),npointer)      
            call ucopy(fphilev(1),fphitot(nphitot+1),npointer)               
            nphitot = nphitot + npointer                            
         else
            print*,'(set_phiroots) phi roots from HOLE LEVEL '//
     +             ' over!'
         endif                            
*
         if (ldebug.ge.2) then
            write(*,*) '(set_phiroots) nphitot before/after hole',
     &                  nphitot,nphitot+int(vphihol(1))
         endif


      else ! prototype configuration

         call find_phiroots('PMTL',thetac,vphiprot,ndum)
*         print*,'roots(PMTL) = ',int(vphiprot(1))

         if (int(vphiprot(1)).ne.0) then
            nrprot=int(vphiprot(1))
*
            call ucopy(vphiprot(2),vphiprotint(1),nrprot)
            call sortzv_alt(vphiprotint,index,nrprot,1,0,0)

            do i=1,nrprot                                   
               vphiprot(i) = vphiprotint(index(i))
            enddo   

            nprot=0
            do i=1,nrprot
               if(i.gt.1.and.vphiprot(i).eq.vphiprot(i-1)) then
                  goto 111
               else
                  nprot=nprot+1
                  vphiprotbuf(nprot)=vphiprot(i)
               endif
 111           continue
            enddo

            call ucopy(vphiprotbuf(1),vphitot(nphitot+1),nprot)      

            call vfill(fphitot(nphitot+1),nprot,45)               

            nphitot = nphitot + nprot                 

         endif

      endif        


* --- compute particle track region phi roots   

      if (LEVGHIT.eq.1) then

         if (rcgeom(1).ne.9) then !flight configuration

            call find_phiroots('TRKZ',thetac,vphitrk,ndum3)
*            print*,'roots(TRKZ) = ',int(vphitrk(1))

            if (int(vphitrk(1)).ne.0) then
               ntemptot = nphitot+int(vphitrk(1))
               if (ntemptot.le.nphmax ) then
                  do i=1,int(vphitrk(1))
                     nphitot = nphitot+1
                     vphitot(nphitot) = abs(vphitrk(i+1))
                     if (vphitrk(i+1).lt.0.) then
                        fphitot(nphitot) = 61
                     else
                        fphitot(nphitot) = 60
                     endif
                  enddo
               else
                  print*,'(set_phiroots) nb phi roots to store (',
     +                 ntemptot,') greater than ',nphmax,' option TRKZ'
               endif      
            endif

         endif

      endif

#ifdef MIRSEC
* --- compute mirror sector limit phi roots

      if (rcgeom(1).ne.9) then  !flight configuration

         call find_phiroots('MSEC',thetac,vphimsec,ndum3)
*         print*,'roots(MSEC) = ',int(vphimsec(1))

         if (int(vphimsec(1)).ne.0) then
            ntemptot = nphitot+int(vphimsec(1))
            if (ntemptot.le.nphmax ) then
               do i=1,int(vphimsec(1))
                  nphitot = nphitot+1
                  vphitot(nphitot) = abs(vphimsec(i+1))
                  fphitot(nphitot) = 70 ! TBD?: 70+sector
               enddo
            else
               print*,'(set_phiroots) nb phi roots to store (',ntemptot,
     +              ') greater than ',nphmax,' option MSEC'
            endif      
         endif

      endif
#endif

* --- sorting phi roots                                                 
      if (nphitot.gt.nphmax)
     +   print*, '(set_phiroots) MAXIMUM NUMBER OF PHI ROOTS EXCEEDED'//
     +           ' nphitot=',nphitot
      call sortzv_alt(vphitot,index,nphitot,1,0,0)                          
*      print*,'nphitot = ',nphitot
      do i=1,nphitot
         vphitotbuf(i) = vphitot(index(i))                              
         fphitotbuf(i) = fphitot(index(i))                              
*         print*,i,vphitotbuf(i),fphitotbuf(i)
      enddo
      call ucopy(vphitotbuf,vphitot,nphitot)
      call ucopy(fphitotbuf,fphitot,nphitot)
* --- end
      return
      end

*=============================================================
      subroutine find_phiroots(chopt,thetac,vphival,nmireft)
*=============================================================
***************************************************************
* It returns phi roots for the following cases:
*
*  IN: chopt   = 'RADT'  - radiator boundary
*                'RADW'  - radiator inner walls
*                'REFL'  - total reflection
*                'MIR0'  - mirror 1st reflection
*                'MIR1'  - mirror 2nd reflection
*                'MPRT'  - mirror prototype
*                'HEMC'  - EMC hole (square)
*                'HCOR'  - EMC hole corners
*                'HGAP'  - PMT module gaps
*                'HPMT'  - PMT outer contours
*                'PMTL'  - PMT prototype outer boundary
*                'TRKZ'  - Particle track impact zone
*                'MSEC'  - Mirror sector limits

*      thetac  = cerenkov angle
*
* OUT: vphival = (1)    : nb of phi root values
*                (2,...): phi's
*      nmireft = number of reflections for the phi roots
*
*                        
* last update : 17/09/2002                                      
*               25/05/2004 (L.Arruda,F.Barao)
*               13/11/2008 (R.Pereira - TRKZ)
*               06/05/2009 (R.Pereira - MSEC)
*
***************************************************************

#include "../include/richrec_lipf.h"

      real par                   ! temporary array for passing values to functions
      common /parfunc/ par(10)   ! par(1) coord of vert/hor line
                                 ! par(2) Cherenkov angle
                                 ! par(3) limits of the mirror
                                 ! par(9) = 1 , x is fixed (vert  line)
                                 !        = 2 , y is fixed (horiz line)
      !$OMP THREADPRIVATE(/parfunc/)
      character chopt*4         

      integer nrootmax
      parameter (nrootmax=200)
      real xb1(nrootmax),xb2(nrootmax), yb1(nrootmax),yb2(nrootmax)
*
      integer nphmax,nermax 
      parameter(nermax=10)
      integer verr(nermax)
      parameter(nphmax=NPHIRMAX)
      real philmir(nphmax),  ! temp phi values
     +     philhol(nphmax),
     +     philrad(nphmax),
     +     philrefl(nphmax),
     +     philholl(nphmax),
     +     philgapup(nphmax),
     +     philgapdw(nphmax),
     +     philcir(nphmax),
     +     philmsl(nphmax),
     +     vphival(nphmax)   ! final phi values to be passed out

      real phpto(3),phdir(3)
*
      integer nx,ny,ig
      real xxx,yyy
*
      integer nholt,nrefhol(nphmax),icounter !used to access the hole roots (change in future...)
      common /holevar/ nholt,philhol,nrefhol
      !$OMP THREADPRIVATE(/holevar/)
*
      integer ncirt,nrefcir(nphmax)
      common /circvar/ ncirt,philcir,nrefcir
      !$OMP THREADPRIVATE(/circvar/)
*
      integer nmslt,nrefmsl(nphmax)
      common /mslvar/ nmslt,philmsl,nrefmsl
      !$OMP THREADPRIVATE(/mslvar/)
*

      real EPS
      integer MAXF
      SAVE EPS,MAXF
      data EPS,MAXF /1.E-4,1000/

      !$OMP THREADPRIVATE(EPS,MAXF)

*
      real fphilmir0,fphilmir1,
     +         fphilhole,
     +         fphilrad,
     +         fphilrefl,
     +         fphilgap

      real fphilmir0_2par,fphilmir1_2par,
     +         fphilhole_2par,
     +         fphilrad_2par,
     +         fphilrefl_2par,
     +         fphilgap_2par

      external fphilmir0,fphilmir1, 
     +         fphilhole,
     +         fphilrad,
     +         fphilrefl,
     +         fphilmir_prot,
     +         fphilgap

      external fphilmir0_2par,fphilmir1_2par,
     +         fphilhole_2par,
     +         fphilrad_2par,
     +         fphilrefl_2par,
     +         fphilgap_2par


*
      integer iallmir, nmireft, nprot, nradt, i, nrrad, ierr,
     +     nrrefl, nreflr, nmirt, nrmir, ip, ifl, ix, iy, ngapup, ngapdw
      real pitchx, pitchy, xlimv, xlimh, ylimh, ylimv, thetac,
     +     x1, x2, root, r, xradlimu, xradlimd, yradlimu,
     +     y1, y2, yradlimd, phi_min, phi_max, discont, xphi,
     +     f, fphilmir_prot, fbuf, ylow, yhig, xlow, xhig,
     +     xl, xr, yu, yd, xc, yc, xlim, ylim


* --- init

* ==> INIT VALUES
      iallmir = 0
      nmireft = 0
      nprot   = 0

      call vfill(par,10,0.)

      call vfill(vphival,nphmax,0.)

      call vzero(nrefhol,nphmax)
      call vfill(philmir,nphmax,0.)
      call vfill(philhol,nphmax,0.)
      call vfill(philrad,nphmax,0.)
      call vfill(philrefl,nphmax,0.)
      call vfill(philgapup,nphmax,0.)
      call vfill(philgapdw,nphmax,0.)

* ==> SET GEOMETRY FLIGHT PARAMETERS (change in future...to common richgeo.inc)
      PITCHX=PMTWX+SHIELDW
      PITCHY=PMTWY+SHIELDW
*
      XLIMV=(17*PITCHX + JUMP)/2.
      XLIMH=XLIMV + DMECX
      YLIMH=(17*PITCHY)/2.
      YLIMV= YLIMH + DMECY

* ==> PASS CERENKOV ANGLE TO FUNCTIONS
      par(2) = thetac

*     ------------------------------------
      if (chopt.eq.'RADT') then           
*     ------------------------------------

      if (rcgeom(1).ne.9) then           /* rich flight configuration */

        /* get interval limits for roots search */

        nradt=nrootmax
        call zbrak(fphilrad,0.,twopi,nrootmax,xb1,xb2,yb1,yb2,nradt)

        /* get roots */                                         

        do i=1,nradt                                         
           x1 = xb1(i)                                    
           x2 = xb2(i)                                    
*           call rzero(x1,x2,root,r,EPS,MAXF,fphilrad_2par)     
           call rzero_alt(x1,x2,root,r,EPS,MAXF,fphilrad_2par)

           philrad(i) = root                             
        enddo                     

      else /* rich prototype configuration */

        /* init */
        nradt = 0
        nrrad = 0
        /* x and y limits of radiator in master (particle) frame*/
        xradlimu=xcrad+Xdrad/2.
        xradlimd=xcrad-Xdrad/2.
        yradlimu=ycrad+ydrad/2.
        yradlimd=ycrad-ydrad/2.
*
        /* get phi roots to the 4 side planes */

        par(1) = xradlimu
        par(9) = 1.

        nrrad=nrootmax
        call zbrak(fphilrad,0.,twopi,nrootmax,xb1,xb2,yb1,yb2,nrrad)

        /* get roots */                                         
        do i=1,nrrad                                         
           x1 = xb1(i)                                    
           x2 = xb2(i)                                    
*     call rzero(x1,x2,root,r,EPS,MAXF,fphilrad_2par)
           call rzero_alt(x1,x2,root,r,EPS,MAXF,fphilrad_2par)

* ........ photon emission point        
           call ucopy(pcervtx,phpto,3)
* ........ generate photon on detector frame                  
           call patphot(root,thetac,phdir)                                  
* ........ test photon direction: backward?                                   
           if (phdir(3).lt.0.) return  
* ........ get yint(phpto(2)) in Rad
           call patints(phpto,phdir,'RADB',ierr)
           IF (abs(phpto(2)-YCRAD).lt.YDRAD/2.) then
              nradt = nradt+1
              philrad(nradt) = root                             
           endif
        enddo                     
*
        par(1)=xradlimd
        par(9) = 1.

        nrrad=nrootmax
        call zbrak(fphilrad,0.,twopi,nrootmax,xb1,xb2,yb1,yb2,nrrad)      

        /* get roots */                                         
        do i=1,nrrad                                         
           x1 = xb1(i)                                    
           x2 = xb2(i)                                    
*           call rzero(x1,x2,root,r,EPS,MAXF,fphilrad_2par)
           call rzero_alt(x1,x2,root,r,EPS,MAXF,fphilrad_2par)

* ........ photon emission point        
           call ucopy(pcervtx,phpto,3)                                
* ........ generate photon on detector frame                  
           call patphot(root,thetac,phdir)                                  
* ........ test photon direction: backward?                                   
           if (phdir(3).lt.0.) return  
* ........ get yint(phpto(2)) in Rad
           call patints(phpto,phdir,'RADB',ierr)
           IF(abs(phpto(2)-YCRAD).lt.YDRAD/2.)then
              nradt = nradt+1
              philrad(nradt) = root                             
           endif
        enddo                     



        par(1) = yradlimu
        par(9) = 2.

        nrrad  = nrootmax
        call zbrak(fphilrad,0.,twopi,nrootmax,xb1,xb2,yb1,yb2,nrrad)      
        /* get roots */                                         
        do i=1,nrrad                                         
           x1 = xb1(i)                                    
           x2 = xb2(i)                                    
*           call rzero(x1,x2,root,r,EPS,MAXF,fphilrad_2par)
           call rzero_alt(x1,x2,root,r,EPS,MAXF,fphilrad_2par)
           
* ........ photon emission point        
           call ucopy(pcervtx,phpto,3)                                
* ........ generate photon on detector frame                  
           call patphot(root,thetac,phdir)                                  
* ........ test photon direction: backward?                                   
           if (phdir(3).lt.0.) return  
* ........ get yint(phpto(2)) in Rad
           call patints(phpto,phdir,'RADB',ierr)
           IF(abs(phpto(1)-XCRAD).lt.XDRAD/2.)then
              nradt = nradt+1
              philrad(nradt) = root                             
           endif
        enddo                     

        nrrad  = nrootmax
        par(1) = yradlimd
        par(9) = 2.

        call zbrak(fphilrad,0.,twopi,nrootmax,xb1,xb2,yb1,yb2,nrrad)      
        /* get roots */                                         
        do i=1,nrrad                                         
           x1 = xb1(i)                                    
           x2 = xb2(i)                                    
*           call rzero(x1,x2,root,r,EPS,MAXF,fphilrad_2par)
           call rzero_alt(x1,x2,root,r,EPS,MAXF,fphilrad_2par)
* ........ photon emission point        
           call ucopy(pcervtx,phpto,3)                                
* ........ generate photon on detector frame                  
           call patphot(root,thetac,phdir)                                  
* ........ test photon direction: backward?                                   
           if (phdir(3).lt.0.) return  
* ........ get yint(phpto(2)) in Rad
           call patints(phpto,phdir,'RADB',ierr)
           IF(abs(phpto(1)-XCRAD).lt.XDRAD/2.)then
              nradt = nradt+1
              philrad(nradt) = root                             
           endif
        enddo                     

      endif

C TEMP the previous 4 sets can be put into a subroutine

* --- output                             

      vphival(1) = real(nradt)           
      call ucopy(philrad,vphival(2),nradt)

*     ------------------------------------
      elseif (chopt.eq.'RADW') then !radiator inner walls
*     ------------------------------------

* ........ get rad tile boundaries
           xxx = pcervtx(1)
           yyy = pcervtx(2)
           call uradtile(xxx,yyy,nx,ny) !tile indices
*           print*,'nx,ny',nx,ny
           call uradlim(nx,ny,x1,x2,y1,y2,ig) !tile limits
*           print*,'tile limits',x1,x2,y1,y2
* ........ tile phi roots
           call tileroots(x1,x2,y1,y2,ig,vphival)

*     ------------------------------------
      elseif (chopt.eq.'REFL') then       
*     ------------------------------------

* --- get interval limits for roots: for total reflected dphi 
      nrrefl=nrootmax 
      call zbrak(fphilrefl,0.,twopi,nrootmax,xb1,xb2,yb1,yb2,nrrefl) 
* --- get roots                             :for refl                     

*    17/09
      nreflr=0
      do i=1,nrrefl                                        
       if(abs(yb1(i)-yb2(i)).lt.1.E6)then
*         call rzero(xb1(i),xb2(i),root,r,EPS,MAXF,fphilrefl_2par)     
         call rzero_alt(xb1(i),xb2(i),root,r,EPS,MAXF,fphilrefl_2par)     
         nreflr=nreflr+1
         philrefl(nreflr) = root                             
       endif
      enddo                                               

* --- output                             

      vphival(1) = real(nreflr)           

      call ucopy(philrefl,vphival(2),nreflr)

*     ------------------------------------
      elseif (chopt.eq.'GAPU') then           
*     ------------------------------------

      if (rcgeom(1).ne.9) then           /* rich flight configuration */

        /* get interval limits for roots search */
        par(4)=HRAD + HPGL + ZTMIRGAP
        ngapup=nrootmax
        call zbrak(fphilgap,0.,twopi,nrootmax,xb1,xb2,yb1,yb2,ngapup)

        /* get roots */                                         

        do i=1,ngapup                                         
           x1 = xb1(i)                                    
           x2 = xb2(i)                                    
*           call rzero(x1,x2,root,r,EPS,MAXF,fphilgap_2par)     
           call rzero_alt(x1,x2,root,r,EPS,MAXF,fphilgap_2par)     
           philgapup(i) = root                             
        enddo
                     
      endif
C TEMP the previous 4 sets can be put into a subroutine

* --- output                             

      vphival(1) = real(ngapup)           
      call ucopy(philgapup,vphival(2),ngapup)

*     ------------------------------------
      elseif (chopt.eq.'MIR0') then           
*     ------------------------------------

* --- find limit phi angles of mirror intersection
    
      nmirt = 0
* ==> search for 1st order roots (0 refls ---> >=1 refls)
      nrmir = nrootmax
      call zbrak(fphilmir0,0.,twopi,nrootmax,xb1,xb2,yb1,yb2,nrmir)

      do i=1,nrmir
         if(abs(yb1(i)-yb2(i)).lt.1.E6)then
*            call rzero(xb1(i),xb2(i),root,r,EPS,MAXF,fphilmir0_2par)
            call rzero_alt(xb1(i),xb2(i),root,r,EPS,MAXF,fphilmir0_2par)
            nmirt=nmirt+1
            philmir(nmirt) = root
         endif                       
      enddo                             

* --- output                                                 
      vphival(1) = real(nmirt)
      call ucopy(philmir,vphival(2),nmirt)

*     ------------------------------------
      elseif (chopt.eq.'MIR1') then           
*     ------------------------------------

      nmirt = 0
* ==> search for 2nd order roots (1 refls ---> 2 refls)                       

      nrmir = nrootmax
      call zbrak(fphilmir1,0.,twopi,nrootmax,xb1,xb2,yb1,yb2,nrmir)

      do i=1,nrmir
         if(abs(yb1(i)-yb2(i)).lt.1.E6)then
*            call rzero(xb1(i),xb2(i),root,r,EPS,MAXF,fphilmir1_2par)
            call rzero_alt(xb1(i),xb2(i),root,r,EPS,MAXF,fphilmir1_2par)
            nmirt=nmirt+1
            philmir(nmirt) = root
         endif                       
      enddo

* --- output                                                 
      vphival(1) = real(nmirt)
      call ucopy(philmir,vphival(2),nmirt)

*     ------------------------------------
      elseif (chopt.eq.'MPRT') then
*     ------------------------------------
      phi_min=min(mirphild(1),mirphild(2))
      phi_max=max(mirphild(1),mirphild(2))

      par(3)=phi_min 
      nmirt = 0
* ==> search for reflection roots on the prototype written on the mirror
*     frame
      discont=0.8*twopi*degrad
      i=0
      do xphi=0.,twopi+twopi/real(2*nrootmax),twopi/real(2*nrootmax)
         f=fphilmir_prot(xphi)        
         i=i+1
         if(i.ge.2)then
            if(abs(f-fbuf).lt.1.E6)then
               if((f*fbuf.lt.0.0.and.abs(f-fbuf).lt.discont)
     ,                           .OR.
     ,            (f*fbuf.gt.0.0.and.abs(f-fbuf).gt.discont) 
     ,              )then
                  nmirt=nmirt+1
                  philmir(nmirt) = xphi
               endif                    
            endif
         endif
         fbuf=f
      enddo

      par(3)=phi_max 
* ==> search for reflection roots on the prototype written on the mirror
*     frame
      i=0
      do xphi=0.,twopi+twopi/real(2*nrootmax),twopi/real(2*nrootmax)
         f=fphilmir_prot(xphi)        
         i=i+1
         if(i.ge.2)then
            if(abs(f-fbuf).lt.1.E6)then
               if((f*fbuf.lt.0.0.and.abs(f-fbuf).lt.discont)
     ,                           .OR.
     ,            (f*fbuf.gt.0.0.and.abs(f-fbuf).gt.discont) 
     ,              )then
                  nmirt=nmirt+1
                  philmir(nmirt) = xphi
               endif                    
            endif
         endif
         fbuf=f
      enddo
* --- output                                                 
      vphival(1) = real(nmirt)
      call ucopy(philmir,vphival(2),nmirt)

*     ------------------------------------
      elseif (chopt.eq.'GAPD') then           
*     ------------------------------------

      if (rcgeom(1).ne.9) then           /* rich flight configuration */

        /* get interval limits for roots search */
        par(4)=HRAD + HPGL + ZTMIRGAP + HMIR + ZBMIRGAP
        ngapdw=nrootmax
        call zbrak(fphilgap,0.,twopi,nrootmax,xb1,xb2,yb1,yb2,ngapdw)

        /* get roots */                                         

        do i=1,ngapdw                                         
           x1 = xb1(i)                                    
           x2 = xb2(i)                                    
*           call rzero(x1,x2,root,r,EPS,MAXF,fphilgap_2par)     
           call rzero_alt(x1,x2,root,r,EPS,MAXF,fphilgap_2par)     
           philgapdw(i) = root                             
        enddo
                     
      endif
C TEMP the previous 4 sets can be put into a subroutine

* --- output                             

      vphival(1) = real(ngapdw)           
      call ucopy(philgapdw,vphival(2),ngapdw)


*     ------------------------------------
      elseif (chopt.eq.'HEMC') then
*     ------------------------------------

         nholt = 0                                    
*              
         /* find limit phi angles of hole intersection*/

          par(1) = XLIMH        !xlimit  for hole (right)
          par(9) = 1.
          par(10)= 2.
          YLOW   = 0.
          YHIG   = YLIMV
          XLOW   = 0.
          XHIG   = 999.
          CALL HLIMSYM(xlow,xhig,ylow,yhig)
          par(1) = -XLIMH       !xlimit (left)            
          par(10)= 1.
          CALL HLIMSYM(xlow,xhig,ylow,yhig)
*
          par(1) = YLIMV        !ylimit  for hole (top)
          par(9) = 2.
          par(10)= 2.
          YLOW   =  0
          YHIG   = 999.
          XLOW   = 0.
          XHIG   = XLIMH
          CALL HLIMSYM(xlow,xhig,ylow,yhig)
          par(1) = -YLIMV       !ylimit (bottom)           
          par(10)= 1.
          CALL HLIMSYM(xlow,xhig,ylow,yhig)
* --- output
         vphival(1) = real(nholt)
         call ucopy(philhol,vphival(2),nholt)
         do i=1,nholt
            ip = i + 1
            if (nrefhol(i).ne.0) vphival(ip) = -vphival(ip)
            nmireft=nmireft+nrefhol(i)
*            write(*,*)nmireft,nrefhol(i)
         enddo

*     ------------------------------------
      elseif (chopt.eq.'HCOR') then
*     ------------------------------------

         NHOLT = 0                                    

        /* FIND ECAL CORNERS */
         
         /* RIGHT */
         par(1) = XLIMH + PITCHX
         par(9) = 1.
         par(10)= 2.
         YLOW   = YLIMH-PITCHY
         YHIG   = YLIMV+PITCHY
         XLOW   = 0.
         XHIG   = 999.
         CALL HLIMSYM(xlow,xhig,ylow,yhig)
         par(1) = XLIMV - PITCHX
         par(10)= 1.
         YLOW   = YLIMV
         CALL HLIMSYM(xlow,xhig,ylow,yhig)
*
         /* DOWN */
         par(1) = -YLIMV - PITCHY
         par(9) = 2.
         par(10)= 1.
         XLOW   = XLIMV-PITCHX
         XHIG   = XLIMH+PITCHX
         YLOW   = 0.
         YHIG   = 999.
         CALL HLIMSYM(xlow,xhig,ylow,yhig)
         par(1) = - YLIMH + PITCHY
         par(10)= 2.
         XLOW   = XLIMH
         CALL HLIMSYM(xlow,xhig,ylow,yhig)
*
         /*  LEFT */
         par(1) = -XLIMH - PITCHX
         par(9) = 1.
         par(10)= 1.
         YLOW   = YLIMH -PITCHY
         YHIG   = YLIMV +PITCHY
         XLOW   = 0.
         XHIG   = 999.
         CALL HLIMSYM(xlow,xhig,ylow,yhig)
         par(1) = -XLIMV + PITCHX
         par(10)= 2.
         YLOW   =  YLIMV
         CALL HLIMSYM(xlow,xhig,ylow,yhig)
*
         /* UP */
         par(1) = YLIMV + PITCHY
         par(9) = 2.
         par(10)= 2.
         XLOW   = XLIMV-PITCHX
         XHIG   = XLIMH+PITCHX
         YLOW   = 0.
         YHIG   = 999.
         CALL HLIMSYM(xlow,xhig,ylow,yhig)
         par(1) = YLIMH - PITCHY
         par(10)= 1.
         XLOW   = XLIMH
         CALL HLIMSYM(xlow,xhig,ylow,yhig)

* --- output
         vphival(1) = real(nholt)
         call ucopy(philhol,vphival(2),nholt)
         do i=1,nholt
            ip = i + 1
            if (nrefhol(i).ne.0) vphival(ip) = -vphival(ip)
            nmireft=nmireft+nrefhol(i)
*            write(*,*)nmireft,nrefhol(i)
         enddo

*     ------------------------------------
      elseif (chopt.eq.'HGAP') then
*     ------------------------------------
*        FIND GAPS BETWEEN MODULES

         NHOLT = 0                                    
*
         par(1) = XLIMH        !xlimit   |_|<-         
         par(9) = 1.
         YLOW =  YLIMH +   PITCHY  
         YHIG =  YLIMV + 7*PITCHY
         XLOW   = 0.
         XHIG   = 999.
         CALL HLIMSYM(xlow,xhig,ylow,yhig)
         par(1) = XLIMV       !xlimit            
         CALL HLIMSYM(xlow,xhig,ylow,yhig)
*                                        _
         par(1) = -XLIMH      !xlimit ->|_|
         par(9) = 1.                         
         par(10)= 1.
         YLOW =  YLIMV +   PITCHY  
         YHIG =  YLIMV + 7*PITCHY
         CALL HLIMSYM(xlow,xhig,ylow,yhig)
         par(1) = -XLIMV       !xlimit            
         CALL HLIMSYM(xlow,xhig,ylow,yhig)

         par(1) = YLIMV       !ylimit (top)    |_|           
         par(9) = 2.
         par(10)= 2.
         XLOW   = XLIMH +   PITCHX
         XHIG   = XLIMH + 7*PITCHX
         YLOW   = 0.
         YHIG   = 999.
         CALL HLIMSYM(xlow,xhig,ylow,yhig)
         par(1) = YLIMH                               
         CALL HLIMSYM(xlow,xhig,ylow,yhig)
*                                                       _
         par(1) = -YLIMV
         par(9) = 2.            
         XLOW   = XLIMH +   PITCHX
         XHIG   = XLIMH + 7*PITCHX
         CALL HLIMSYM(xlow,xhig,ylow,yhig)
         par(1) = -YLIMH
         CALL HLIMSYM(xlow,xhig,ylow,yhig)

         /* find roots between modules on the top and the bottom*/

         /*TOP*/

         par(1) = -DX0
         par(9) = 1.
         par(10)= 2.
         YLOW =  YLIMV
         YHIG =  YLIMV+9*PITCHY
         XLOW   = -999.
         XHIG   =  999.
         CALL HLIMASYM(xlow,xhig,ylow,yhig)
         par(1) =  -DX0-JUMP
         par(10)= 1.
         CALL HLIMASYM(xlow,xhig,ylow,yhig)

         /*BOTTOM*/
         par(1) = DX0
         par(9) = 1.
         par(10)= 1.
         YLOW =  -(YLIMV+9*PITCHY)
         YHIG =  -YLIMV
         CALL HLIMASYM(xlow,xhig,ylow,yhig)
         par(1) =  DX0+JUMP
         par(10)= 2.
         CALL HLIMASYM(xlow,xhig,ylow,yhig)

* --- output
         vphival(1) = real(nholt)
         call ucopy(philhol,vphival(2),nholt)
         do i=1,nholt
            ip = i + 1
            if (nrefhol(i).ne.0) vphival(ip) = -vphival(ip)
            nmireft=nmireft+nrefhol(i)
*            write(*,*)nmireft,nrefhol(i)
         enddo


*     ------------------------------------
      elseif (chopt.eq.'HPMT') then
*     ------------------------------------
         /* PMT CONTOUR */

         NHOLT = 0                                    

         /* now find EXTERNAL BOUNDS */

         /*RIGHT*/
         par(1) = XLIMH + 9*PITCHX
         par(9) = 1.
         par(10)= 1.
         YLOW   = -YLIMH
         YHIG   =  YLIMH
         XLOW   = 0.
         XHIG   = 999.
         CALL HLIMSYM(xlow,xhig,ylow,yhig)

         /*TOP*/
         par(1) = YLIMV + 9*PITCHY
         par(9) = 2.
         par(10)= 1.
         YLOW   = 0.
         YHIG   = 999.
         XLOW   = - XLIMV
         XHIG   =  XLIMV
         CALL HLIMSYM(xlow,xhig,ylow,yhig)

         /* LEFT*/
         par(1) = - XLIMH  - 9*PITCHY
         par(9) = 1.
         par(10)= 2.
         YLOW   = -YLIMH
         YHIG   =  YLIMH
         XLOW   = 0.
         XHIG   = 999.
         CALL HLIMSYM(xlow,xhig,ylow,yhig)

         /* BOTTOM*/
         par(1) = - YLIMV - 9*PITCHY
         par(9) = 2.
         par(10)= 2.
         YLOW   = 0.
         YHIG   = 999.
         XLOW   = -XLIMV
         XHIG   =  XLIMV
         CALL HLIMSYM(xlow,xhig,ylow,yhig)

         /* find EXTERNAL BOXES */

         DO I=0,755          
           CALL PMTPOS_FLIGHT(I,XL,XR,YU,YD,XC,YC,IFL)
           IF(IFL.eq.1) THEN
            IF(XC.LT.0.)THEN
             par(1) = XR
             par(10)= 2.   
            ELSE
             par(1) = XL
             par(10)= 1.   
            ENDIF 
             par(9) = 1.
            CALL HLIMSYM(0.,999.,YD,YU)
            IF(YC.LT.0.)THEN
             par(1) = YU
             par(10)= 2.
            ELSE
             par(1) = YD
             par(10)= 1.
            ENDIF
            PAR(9) = 2.
            CALL HLIMSYM(XL,XR,0.,999.)            
           ENDIF
          ENDDO

* --- output
         vphival(1) = real(nholt)
         call ucopy(philhol,vphival(2),nholt)
         do i=1,nholt
            ip = i + 1
            if (nrefhol(i).ne.0) vphival(ip) = -vphival(ip)
            nmireft=nmireft+nrefhol(i)
*            write(*,*)nmireft,nrefhol(i)
         enddo


*     ------------------------------------
      elseif (chopt.eq.'TRKZ') then
*     ------------------------------------

         ncirt = 0                                    
*              
         /* find limit phi angles of track impact zone*/

         xc = pcoopmt(1)
         yc = pcoopmt(2)
         crad = trhitass2**0.5

         par(5) = xc
         par(6) = yc
         par(7) = crad

         CALL HLIMCIRC(xc,yc,crad)

* --- output
         vphival(1) = real(ncirt)
         call ucopy(philcir,vphival(2),ncirt)
         do i=1,ncirt
            ip = i + 1
            if (nrefcir(i).ne.0) vphival(ip) = -vphival(ip)
            nmireft=nmireft+nrefcir(i)
*            write(*,*)nmireft,nrefcir(i)
         enddo


*     ------------------------------------
      elseif (chopt.eq.'MSEC') then
*     ------------------------------------

         nmslt = 0                          
*              
         /* find phi angles where mirror sector changes */

         CALL HLIMMSEC

* --- output
         vphival(1) = real(nmslt)
         call ucopy(philmsl,vphival(2),nmslt)
         do i=1,nmslt
            ip = i + 1
            if (nrefmsl(i).ne.0) vphival(ip) = -vphival(ip)
            nmireft=nmireft+nrefmsl(i)
*            write(*,*)nmireft,nrefmsl(i)
         enddo


*     ------------------------------------
      elseif (chopt.eq.'PMTL') then
*     ------------------------------------

      /*Roots in PMT Matrix - Prototype*/


*************************************************************************
* ORDER OF FILLING (geom_id)
*                                     
*            +-+-+--------+-+     FRAME  CHOSEN    
*            |A|9|87654321|0|                      
*            | | |        |B|     |X       
*            | | |        | |     |                
*            | | |        | |     |                
*            | | |        | |     |                
*            | | |        | |     +------> Y       
*            +-+ |        | |
*              | |        | |
*              | |        | |
*              | |        | |
*              +-+        +-+
*                |   XXXXX|  -> some boxes are empty
*                +--------+               

*           ix=01,iy=11  .....         ix=01,iy=01
*           .                          .         
*           .                          .         
*           .                          .         
*           ix=09,iy=11                ix=09,iy=01
*           ix=10,iy=11  ....          ix=10,iy=01
*     limits :
*               x=-15.50
*               x=-12.40
*               x=-03.10
*               x=+15.50
*
*               y=-17.05
*               y=-13.95
*               y=-10.85
*               y=+13.95
*               y=+17.05
*************************************************************************

*************************************************************************
*                                                                       *
*        finding phi roots with border CONTOUR of PMT Matrix            *
*                                                                       *
*                                                                       *
*************************************************************************

*     <<<<  VERTICAL SEGMENTS >>>>

         if (ldebug.ge.4) then       
           print*,' '
           print*,'>>>>>>>>>>>>  GENERAL MATRIX CONTOUR ROOTS <<<<<<<<'
           print*,' '
         endif

      /* limit of coordinates */
         xlim = xlimprot
         ylim = ylimprot

         nholt = 0                                                    

*  --- init number roots
         /* find limit phi angles of plane intersection*/
 /*TOP X */
          par(10)= 1.
          par(1) = XLIM
          par(9) = 1.
          XLOW   = 0.
          XHIG   = 999.
          YLOW   = 0.
          YHIG   = YLIM
          CALL HLIMSYM(xlow,xhig,ylow,yhig)
  /*BOTTOM X*/
          par(10)= 2.
          par(1) = -XLIM
*+PITCHX
          YLOW   = 0
          YHIG   = YLIM
          CALL HLIMSYM(xlow,xhig,ylow,yhig)
  /*RIGHT Y*/
          par(10)= 1.
          par(1) = YLIM
          par(9) = 2.
          YLOW   = 0.
          YHIG   = 999.
          XLOW   = 0.
          XHIG   = XLIM
          CALL HLIMSYM(xlow,xhig,ylow,yhig)
  /*LEFT Y*/
          par(10)= 2.
          par(1) = -YLIM
          XHIG   = XLIM
          CALL HLIMSYM(xlow,xhig,ylow,yhig)

          nprot   = nholt

   /* find EXTERNAL BOXES */
*
         DO I=0,nbpmtx*nbpmty-1
          CALL PMTPOS_PROTN(I,IX,IY,XL,XR,YU,YD,XC,YC)
          IFL = IFLPMT(IX,IY)
C -------------------------------------------
C  Virtual PMTS (mechanical contour)
c  Only internal sides need to be considered

           IF(IFL.EQ.1) THEN
C -------------------------------------------
               IF(XR.lt.0) THEN              
                 par(1) = XR
                 par(10)= 2.   
               ELSE 
                 par(1) = XL
                 par(10)= 1.   
               ENDIF
                par(9) = 1.
               CALL HLIMASYM(-999.,999.,YD,YU)
               IF(YD.LT.0) THEN
                PAR(1) = YU
                PAR(9) = 2.
                par(10)= 2.
               ELSE
                PAR(1) = YD
                PAR(9) = 2.
                par(10)= 1.
               ENDIF
               CALL HLIMASYM(XL,XR,-999.,999.)
C -----------------------------------------------------
C      Bad or non-existing PMTs can be inside the matrix
C           all sides need to be considered

            ELSEIF(IFL.EQ.2)THEN
C -----------------------------------------------------
              par(9) = 1.  
            
              par(1) = XR
              par(10)= 2.
              CALL HLIMASYM(-999.,999.,YD,YU)
              par(1) = XL
              par(10)= 1.
              CALL HLIMASYM(-999.,999.,YD,YU)
             
              PAR(9) = 2.  
             
              PAR(1) = YU
              par(10)= 2.
              CALL HLIMASYM(XL,XR,-999.,999.)
              PAR(1) = YD
              par(10)= 1.
              CALL HLIMASYM(XL,XR,-999.,999.)
           ENDIF 
          ENDDO
          nprot   = nholt

* --- output
          vphival(1) = real(nprot)
          call ucopy(philhol,vphival(2),nprot)
*     ------------------------------------
      endif                              
*     ------------------------------------
* --- end
      return
      end

*******************************************************************************
*  =========================== FUNCTIONS ======================================
*******************************************************************************

* =======================================================================
      real Function fphilmir0_2par(phig,ii)
* =======================================================================
      real phig
      integer ii

      fphilmir0_2par = fphilmir0(phig)

* --- end
      return
      end

* =======================================================================  
      real Function fphilmir0(phig)                                         
* =======================================================================  
*************************************************************************
*                                                                          
* Function: given by radius(phig) - RBMIR
*           (where radius is computed without refection)
*           Its roots give "intersection" of cerenkov cone (geratrix)
*           with the mirror cone at the detector plane.
*           Takes value =1E8 when not defined because of photons do not
*           reach mirror
*
* function signal meaning:
*                         >0 when photons are reflected by miror
*                         <0 when photons reach PMTplane directly
*
*                                                                          
* VARIABLE :
*            phig = parametric phi of geratrix of Cherenkov cone
*
* PARAMETERS :
*        IN  : thetac (via common)
*        OUT :
*                                                                          
*************************************************************************

#include "../include/richrec_lipf.h"
      
      real phig
      Character chin*3
      common /parfunc/ par(10)   
      !$OMP THREADPRIVATE(/parfunc/)
      real phi, phorig(3)                                                  
                                                                           
      real paxisx(3),paxisy(3),paxisz(3)                                   
      real axisx(3),axisy(3),axisz(3)                                      
                                                                           
      real phpto(3),phdir(3),vgerp(3),vger(3)                              
      real phpto_buf(3)                                                    
      SAVE FPHILMIR_BUF
      DATA FPHILMIR_BUF/1.E8/                                              
      !$OMP THREADPRIVATE(FPHILMIR_BUF)                
                                                                 
                                                                           
* --- init parameters
      thetac   = par(2)
      fphilmir0 = fphilmir_buf
                                                                           
* --- particle impact parameter                                            
      call ucopy(pcervtx,phpto,3)                                          
                                                                           
* --- rotation matrix between detector frame and particle frame
      if (irotflg.eq.0) then                                               
         call patmatr                                                      
         irotflg = 1                                                       
      endif                                                                 
                                                                           
* --- generate photon on detector frame
      call patphot(phig,thetac,phdir)
* --- test photon direction: backward?                                 
      if (phdir(3).lt.0.) return                                             
* --- refraction at radiator exit      
      call patints(phpto,phdir,'RADB',ierr)
*      print*,'IERR',ierr
      if (ierr.ne.0) return                                     
*      if (ierr.eq.1) return                                     
      chin = chradid
      if (HPGL.ne.0.0) then
        call patrefr(chin,'PGL',phdir,ierr) !refraction
        if (ierr.ne.0) return
        call patints(phpto,phdir,'PGLB',ierr)                         
        if (ierr.ne.0) return
        call patrefr('PGL','AIR',phdir,ierr) !refraction             
        if (ierr.ne.0) return
      else
        call patrefr(chin,'AIR',phdir,ierr) !refraction
        if (ierr.ne.0) return
      endif
      rpoint = sqrt((phpto(1)-vtmir(1))**2+(phpto(2)-vtmir(2))**2)
      if(rpoint.ge.RTMIR)return
*      elseif (chradid.eq.'PGL') then
*         if (HPGL.eq.0.0) then
*           print*,' >>> fphilhole: photons starting in PGL when HPGL=0 !!!'
*           return
*         endif  
*         call patints(phpto,phdir,'PGLB',ierr) !intersection           
*         if (ierr.ne.0) return
*         call patrefr('PGL','AIR',phdir,ierr)  !refraction             
*         if (ierr.ne.0) return
*      endif

* --- intersect photon with PMT plane
      call patints(phpto,phdir,'PMTD',ierr)                            

      rpoint2 = (phpto(1)-vtmir(1))**2+(phpto(2)-vtmir(2))**2
      fphilmir0 = rpoint2 - RBMIR**2
* --- end                                                                  
      return                                                               
      end                

* =======================================================================
      real Function fphilmir1_2par(phig,ii)
* =======================================================================
      real phig
      integer ii

      fphilmir1_2par = fphilmir1(phig)

* --- end
      return
      end

                                                  
* =======================================================================  
      real Function fphilmir1(phig)                                         
* =======================================================================  
*************************************************************************
*                                                                          
* Function: given by radius(phig) - RBMIR
*           (where radius is computed without refection)
*           Its roots give "intersection" of cerenkov cone (geratrix)
*           with the mirror cone at the detector plane.
*           Takes value =1E8 when not defined because of photons do not
*           reach mirror
*
* function signal meaning:
*                         >0 when photons are reflected by miror
*                         <0 when photons reach PMTplane directly
*
*                                                                          
* VARIABLE :
*            phig = parametric phi of geratrix of Cherenkov cone
*
* PARAMETERS :
*        IN  : thetac (via common)
*        OUT :
*                                                                          
*************************************************************************

#include "../include/richrec_lipf.h"

      Character chin*3
      common /parfunc/ par(10)   
      !$OMP THREADPRIVATE(/parfunc/)
      real phi, phorig(3)                                                  
                                                                           
      real paxisx(3),paxisy(3),paxisz(3)                                   
      real axisx(3),axisy(3),axisz(3)                                      
                                                                           
      real phpto(3),phdir(3),vgerp(3),vger(3)                              
      real phpto_buf(3)                                                    
      SAVE FPHILMIR_BUF
      DATA FPHILMIR_BUF/1.E8/                                              
      !$OMP THREADPRIVATE (FPHILMIR_BUF)                                                                 
                                                                           
* --- init parameters
      thetac   = par(2)
      fphilmir1 = fphilmir_buf
                                                                           
* --- particle impact parameter                                            
      call ucopy(pcervtx,phpto,3)                                          
                                                                           
* --- rotation matrix between detector frame and particle frame            
      if (irotflg.eq.0) then                                               
         call patmatr                                                      
         irotflg = 1                                                       
      endif                                                                 
                                                                           
* --- generate photon on detector frame
      call patphot(phig,thetac,phdir)
* --- test photon direction: backward?                                 
      if (phdir(3).lt.0.) return                                             
* --- refraction at radiator exit                                 
      call patints(phpto,phdir,'RADB',ierr)                      
*      if (ierr.ne.0) return                                     
      if (ierr.eq.1) return                                     
      chin = chradid
      if (HPGL.ne.0.0) then
        call patrefr(chin,'PGL',phdir,ierr) !refraction
        if (ierr.ne.0) return
        call patints(phpto,phdir,'PGLB',ierr)                         
        if (ierr.ne.0) return
        rpoint = sqrt((phpto(1)-vtmir(1))**2+(phpto(2)-vtmir(2))**2)
        if(rpoint.ge.RTMIR)return
        call patrefr('PGL','AIR',phdir,ierr) !refraction             
        if (ierr.ne.0) return
      else
        call patrefr(chin,'AIR',phdir,ierr) !refraction
        if (ierr.ne.0) return
      endif
      rpoint = sqrt((phpto(1)-vtmir(1))**2+(phpto(2)-vtmir(2))**2)
      if(rpoint.ge.RTMIR)return
*      elseif (chradid.eq.'PGL') then
*         if (HPGL.eq.0.0) then
*           print*,' >>> fphilhole: photons starting in PGL when HPGL=0 !!!'
*           return
*         endif  
*         call patints(phpto,phdir,'PGLB',ierr) !intersection           
*         if (ierr.ne.0) return
*         call patrefr('PGL','AIR',phdir,ierr)  !refraction             
*         if (ierr.ne.0) return
*      endif
     
      call patints(phpto,phdir,'PMTD',ierr)
      rpoint2 = phpto(1)**2+phpto(2)**2
      if(rpoint2.gt.RBMIR**2)then
         call patints(phpto,phdir,'MIRR',ierr) !intersection with mirror
         if (ierr.ne.0) return                                     
         call patrefl(phpto,phdir,ierr) !reflection with mirror         
         call patints(phpto,phdir,'PMTD',ierr)
         rpoint2 = (phpto(1)-vtmir(1))**2+(phpto(2)-vtmir(2))**2
         fphilmir1 = rpoint2 - RBMIR**2
      else
         fphilmir1= FPHILMIR_BUF
      endif

* --- end                                                                  
      return                                                               
      end                                                                  



* ================================================================        
      real Function fphilmir_prot(phig)
* ================================================================
******************************************************************        
*       AIMS: It gives the intersection points of the photons with
*             the limits of the mirror used in the prototype
*
******************************************************************   

#include "../include/richrec_lipf.h"

      common /parfunc/ par(10)
      !$OMP THREADPRIVATE(/parfunc/)
      Character chin*3
      real phi, phorig(3)                                                  
      real paxisx(3),paxisy(3),paxisz(3)                                   
      real axisx(3),axisy(3),axisz(3)          
      integer lim(2) !lim(1)=mirphild(1) lim(2)=mirphild(2)
      real phpto(3),phdir(3),vgerp(3),vger(3)                              
      real phpto_buf(3)                                                    
      SAVE FPHILMIR_BUF
      DATA FPHILMIR_BUF/1.E8/                                              
      !$OMP THREADPRIVATE (FPHILMIR_BUF)


* --- init parameters
      thetac   = par(2)
      fphilmir_prot = fphilmir_buf
                                                                           
* --- particle impact parameter                                            
      call ucopy(pcervtx,phpto,3)                                          
                                                                           
* --- rotation matrix between detector frame and particle frame            
      if (irotflg.eq.0) then                                               
         call patmatr                                                      
         irotflg = 1                                                       
      endif                                                                 
                                                                           
* --- generate photon on detector frame
      call patphot(phig,thetac,phdir)
* --- test photon direction: backward?                                 
      if (phdir(3).lt.0.) return                                             
* --- refraction at radiator exit                                 
      call patints(phpto,phdir,'RADB',ierr)
      if (ierr.ne.0) return                                     
      chin = chradid
      if (HPGL.ne.0.0) then
        call patrefr(chin,'PGL',phdir,ierr) !refraction
        if (ierr.ne.0) return
        call patints(phpto,phdir,'PGLB',ierr)                         
        if (ierr.ne.0) return
        call patrefr('PGL','AIR',phdir,ierr) !refraction             
        if (ierr.ne.0) return
      else
        call patrefr(chin,'AIR',phdir,ierr) !refraction
        if (ierr.ne.0) return
      endif

      ZMIR=ZPMTDET-HMIR
      phpto(3) = ZMIR
      phpto(1) = phpto(1) + phdir(1)/phdir(3)*(ZMIR-phdir(3))
      phpto(2) = phpto(2) + phdir(2)/phdir(3)*(ZMIR-phdir(3))
    
      rpoint = sqrt((phpto(1)-vtmir(1))**2+(phpto(2)-vtmir(2))**2)
      if(rpoint.ge.RTMIR)return

* ... intersect photon with PMT plane
      call patints(phpto,phdir,'PMTD',ierr)
      rpoint = sqrt((phpto(1)-vtmir(1))**2+(phpto(2)-vtmir(2))**2)
      if (rpoint.gt.RBMIR) then 
* --- intersect photon with PMT plane
         call patints(phpto,phdir,'MIRR',ierr)      
         if(ierr.ne.0)return
         phi_int=atan2(phpto(2)-vtmir(2),phpto(1)-vtmir(1))        
         if (phi_int.lt.0.) phi_int = phi_int + twopi
         fphilmir_prot=phi_int-par(3)
c         print'(A,f5.1,A,f5.1,A,f5.1,A,f7.1)',' phicher='
c     ,        ,phig*raddeg,', lim=',par(3)*raddeg,', phipoint=',phi_int
c     ,        *raddeg,', funct=',fphilmir_prot*RADDEG
      endif
     

* --- end                                                                 
      return                                                              
      end                                                                    

* =======================================================================
      real Function fphilhole_2par(phig,ii)
* =======================================================================
      real phig
      integer ii

      fphilhole_2par = fphilhole(phig)

* --- end
      return
      end


* ==========================================================
      real function fphilhole(phig)
* ==========================================================
*************************************************************
*                                                           *
* variable : phig (parameter of geratrix Cherenkov cone )   *
*                                                           *
*                                                           *
* PARAMETERS :                                              *
*         IN : thetac (via common)                          *
*         OUT:                                              *
*                                                           *
* The zeros of this function give the geratrix Cone         *
* intersection phig with vert/horizontal lines at PMT       *
* zlevel.                                                   *
* The function takes an init value=1E8 when not defined     *
* because of photons not reaching this z level              *
* function signal meaning:                                  *
*            >0 when photons reach inside area under study  *
*                              (EMChole/malfunctioningPMT)  *
*            <0 when photons reach outside area under study *
*                                                           *
* in FLIGHT:  used for EMC HOLE intersection phis           *
*                                                           *
* in PROTOT:  used for general PMT matrix                   *
*             and individual PMTS contours                  *
*             intersections                                 *
*                                                           *
*     Rev: 26/02/2002                                       *
*                                                           *
*************************************************************  

#include "../include/richrec_lipf.h"
      real phig

      common /parfunc/ par(10)
      !$OMP THREADPRIVATE(/parfunc/)
      character chin*3,chout*3
      real phpto(3),phdir(3)
      save fphildummy     
      data fphildummy/1.E8/
      !$OMP THREADPRIVATE(fphildummy)
      real cshift

* --- init
      nmiref = 0
      fphilhole = fphildummy

* --- parameter          
      thetac = par(2)
      iflagloc = int(par(9)) /* 1=X line, 2=Y line */      

* --- rotation matrix between detector frame and particle frame            
      if (irotflg.eq.0) then                                               
         call patmatr                                                      
         irotflg = 1                                                       
      endif                                                                   
* --- photon emission point                                   
      call ucopy(pcervtx,phpto,3)                                    
* --- generate photon on detector frame
      call patphot(phig,thetac,phdir)
* --- test photon direction: backward?                                    
      if (phdir(3).lt.0.) return                               
* --- refraction at radiator exit                                 
      if (chradid.eq.'AGL'.or. chradid.eq.'NAF') then !photons radiated in AGL or NAF
        call patints(phpto,phdir,'RADB',ierr)                       
        if (ierr.ne.0) return    
        chin = chradid
        if (HPGL.ne.0.0) then
          call patrefr(chin,'PGL',phdir,ierr) !refraction
          if (ierr.ne.0) return
          call patints(phpto,phdir,'PGLB',ierr)
          if (ierr.ne.0) return
          call patrefr('PGL','AIR',phdir,ierr) !refraction             
          if (ierr.ne.0) return
        else
          call patrefr(chin,'AIR',phdir,ierr) !refraction
          if (ierr.ne.0) return             
        endif
      elseif (chradid.eq.'PGL') then
        if (HPGL.eq.0.0) then
          print*,' >>> fphilhole: photons starting in PGL when HPGL=0!'
          return
        endif  
        call patints(phpto,phdir,'PGLB',ierr) !intersection           
        if (ierr.ne.0) return
        call patrefr('PGL','AIR',phdir,ierr) !refraction             
        if (ierr.ne.0) return
      endif
     
* --- intersect photon with PMT plane
      call patints(phpto,phdir,'PMTD',ierr)                            
* --- mirror reflection                   
      if (rcgeom(1).ne.9.or.(rcgeom(1).eq.9.and.rcgeom(2).eq.1)) then  !flight configuration  
        rpoint = sqrt((phpto(1)-vtmir(1))**2+(phpto(2)-vtmir(2))**2)                        
        do while (rpoint.gt.RBMIR)
          nmiref = nmiref + 1
          if (nmiref.gt.10) return 
          call patints(phpto,phdir,'MIRR',ierr) !intersec with mirror
          if (ierr.ne.0) return                                      
          call patrefl(phpto,phdir,ierr) !reflection with mirror          
          call patints(phpto,phdir,'PMTD',ierr)                      
          rpoint = sqrt(phpto(1)**2+phpto(2)**2)
        enddo                                                         
      endif

*-----parametrization of the function                                      
      If ( iflagloc.eq.1)then    !XLIM Segment
         gfun      = phpto(1) + phdir(1)/phdir(3)*(ZPMTDET-phpto(3))
         CSHIFT    = XCPMM
      else                      !YLIM Segment
        gfun      = phpto(2) + phdir(2)/phdir(3)*(ZPMTDET-phpto(3))
         CSHIFT    = YCPMM
      endif
     
*      fphilhole = par(1) - gfun
      if(int(par(10)).eq.2)then
        fphilhole = (par(1)+CSHIFT) -  gfun
      else
        fphilhole = gfun   - (par(1)+CSHIFT)
      endif

* --- end
      return
      END                                                                    

* =======================================================================
      real Function fphilrefl_2par(phig,ii)
* =======================================================================
      real phig
      integer ii

      fphilrefl_2par = fphilrefl(phig)

* --- end
      return
      end


* =======================================================================   
      real function fphilrefl(phig)
* =======================================================================   
*
*                                                                           
* Function: Check for total reflection at Radiator end
*           Radiator can be AGl/NAF or PGL;
*           its roots give phi limits of this total reflection ! should !
*
* function signal meaning:
*                         >0 when photons suffer total reflection
*                         <0 when photons suffer normal refraction
*
*         
*                         last update : 17/09/2002
*
*************************************************************************

#include "../include/richrec_lipf.h"

      common /parfunc/ par(10)
      !$OMP THREADPRIVATE(/parfunc/)
      real vgerp(3),vger(3),vnormal(3)
      real phpto(3),phdir(3)
      character chin*3
      SAVE VNORMAL
      DATA VNORMAL/0.,0.,-1./!normal to the radiator plane on general frame 
                             !(radiator inward)
      !$OMP THREADPRIVATE(VNORMAL)

      save fphilrefldummy     
      data fphilrefldummy/1.E8/
      !$OMP THREADPRIVATE(fphilrefldummy)

* --- init parameters and function
      thetac=par(2)
      fphilrefl=fphilrefldummy
     
* --- rotation matrix between detector frame and particle frame
      if (irotflg.eq.0) then
         call patmatr
         irotflg = 1
      endif

* --- generate photon on detector frame
      call patphot(phig,thetac,phdir)
* --- test photon direction: backward?        

      if (phdir(3).lt.0.) return             

* --- 1st incident angle (inside 1st medium ; MAIN radiator)
cc      call ucopy(phdir,vger,3)
cc      costhi = - vdot(vger,vnormal,3)     
cc     sinthi = sqrt(1-costhi**2)

* --- transmission angle to air
      call ucopy(pcervtx,phpto,3)
      call patints(phpto,phdir,'RADB',ierr)
      if (ierr.ne.0) return
      rpoint = sqrt((phpto(1)-vtmir(1))**2+(phpto(2)-vtmir(2))**2)     
      if(rpoint.ge.RTMIR)return
      IF (HPGL.EQ.0.0) THEN
         costhi = - vdot(phdir,vnormal,3)
         sinthi = sqrt(1-costhi**2)
         sintht = refindex*sinthi
      ELSE
         chin = chradid
         call patrefr(chin,'PGL',phdir,ierr)
         call patints(phpto,phdir,'PGLB',ierr)
         if (ierr.ne.0) return        
         rpoint = sqrt((phpto(1)-vtmir(1))**2+(phpto(2)-vtmir(2))**2)
         if(rpoint.ge.RTMIR)return
         costhi = - vdot(phdir,vnormal,3)
         sinthi = sqrt(1-costhi**2)
         sintht = PGLIX*sinthi
      ENDIF
      if (costhi.le.0.) return
* --- function value
      fphilrefl= sintht - 1.
      return
      end


* =======================================================================
      real Function fphilrad_2par(phig,ii)
* =======================================================================
      real phig
      integer ii

      fphilrad_2par = fphilrad(phig)

* --- end
      return
      end


* =======================================================================   
      real Function fphilrad(phig)
* =======================================================================   
*************************************************************************
*                                                                           
* Function: intersection of cherenkov cone (geratrix) with the              
*           the outer boundary of the radiator (at the bottom of the rad)
*           - flight: shape = cylindrical
*           - protot: shape = box
*
*
* PARAMETERS
*       IN  : phig - azimuthal photon angle
*
*       BY COMMON:
*          par(1)  = X or Y segment to intersept
*          par(2)  = thetac
*          par(9)  = X or Y segment? (1=X, 2=Y)
*          par(10) = flight: outer boundary or radiator walls? (0=outer, 1=wall)
*
* last update : 26/02/2002
*               17/09/2002
*               25/05/2004 (L.Arruda,F.Barao)                     
*                                                                           
*************************************************************************

#include "../include/richrec_lipf.h"

      common /parfunc/ par(10)
      !$OMP THREADPRIVATE(/parfunc/)
      real cshift, gfun

* --- init
      thetac = par(2)
      iflagloc= int(par(9))

* --- rotation matrix between detector frame and particle frame
      if (irotflg.eq.0) then
         call patmatr
         irotflg = 1
      endif


* --- parametrization of the function               
*     (A1,A2,A3) is the direction of photon generated in detctors frame

      A1 = figrotm(1,1)*sin(thetac)*cos(phig) +                             
     +     figrotm(1,2)*sin(thetac)*sin(phig)  +                             
     +     figrotm(1,3)*cos(thetac)                                         
      A2 = figrotm(2,1)*sin(thetac)*cos(phig) +                              
     +     figrotm(2,2)*sin(thetac)*sin(phig) +                              
     +     figrotm(2,3)*cos(thetac)                                          
      A3 = figrotm(3,1)*sin(thetac)*cos(phig) +                              
     +     figrotm(3,2)*sin(thetac)*sin(phig) +                              
     +     figrotm(3,3)*cos(thetac)                                         


*     if photon is backward A3<0 
*     fphilrad remains unchanged so that rzero procedure can have troubles in
*     leading with
*
*                +
*              +
*             +
*__________________________________ >  
*
*        +++++
*       +          
*      +            
*
      if (A3.lt.0.) goto 99

      ZBOTTOM = HRAD + HPGL

* --- Roots function

      If (RCGEOM(1).ne.9) then !FLIGHT

         if (int(par(10)).eq.0) then

* ...... Radiator outer boundary (cylinder)

            A  = (ZBOTTOM - pcervtx(3))**2 * ((A1/A3)**2+(A2/A3)**2)
            B  = 2*(ZBOTTOM-pcervtx(3))*(pcervtx(1)*A1/A3 +
     +                                   pcervtx(2)*A2/A3)
            C  =  pcervtx(1)**2 + pcervtx(2)**2 - RTMIR**2
            fphilrad = A + B + C

         else

* ...... Radiator walls

            CSHIFT = 0.0
            If (iflagloc.eq.1) then       !XLIM Segment
               gfun      = pcervtx(1) + A1/A3*(ZBOTTOM - pcervtx(3))
               CSHIFT    = XCRAD
            else                          !YLIM Segment
               gfun      = pcervtx(2) + A2/A3*(ZBOTTOM - pcervtx(3))
               CSHIFT    = YCRAD
            endif

            fphilrad = gfun - (par(1)+CSHIFT)
CC            print*,'(fphilrad) phig,f=',phig,fphilrad

         endif

      else !PROTOTYPE (box)

         CSHIFT = 0.0
         If (iflagloc.eq.1) then       !XLIM Segment
            gfun      = pcervtx(1) + A1/A3*(ZBOTTOM - pcervtx(3))
            CSHIFT    = XCRAD
         else                          !YLIM Segment
            gfun      = pcervtx(2) + A2/A3*(ZBOTTOM - pcervtx(3))
            CSHIFT    = YCRAD
         endif

         fphilrad = gfun - (par(1)+CSHIFT)

      endif

* --- end                                                                   
 99   return                                                                
      end                                                                   

* =======================================================================
      real Function fphilgap_2par(phig,ii)
* =======================================================================
      real phig
      integer ii

      fphilgap_2par = fphilgap(phig)

* --- end
      return
      end


* =======================================================================   
      real Function fphilgap(phig)
* =======================================================================   
*************************************************************************
*                                                                           
* Function: intersection of cherenkov cone (geratrix) with the              
*           lateral boundary of the gaps (at the top and at the  bottom of the mirror)

*
* PARAMETERS
*       IN  : phig - azimuthal photon angle
*
*       BY COMMON:
*          par(1)  = X or Y segment to intersept
*          par(2)  = thetac
*          par(4)  = zz coordinate of the bottom of the gap to intersect
*          par(9)  = X or Y segment? (1=X, 2=Y)
*          par(10) = flight: outer boundary or radiator walls? (0=outer, 1=wall)
*
*               9/06/2006 (L.Arruda)                     
*                                                                           
*************************************************************************

#include "../include/richrec_lipf.h"

      common /parfunc/ par(10)
      !$OMP THREADPRIVATE(/parfunc/)
      real cshift, gfun

* --- init
      thetac = par(2)
      iflagloc= int(par(9))

* --- rotation matrix between detector frame and particle frame
      if (irotflg.eq.0) then
         call patmatr
         irotflg = 1
      endif


* --- parametrization of the function               
*     (A1,A2,A3) is the direction of photon generated in detctors frame

      A1 = figrotm(1,1)*sin(thetac)*cos(phig) +                             
     +     figrotm(1,2)*sin(thetac)*sin(phig)  +                             
     +     figrotm(1,3)*cos(thetac)                                         
      A2 = figrotm(2,1)*sin(thetac)*cos(phig) +                              
     +     figrotm(2,2)*sin(thetac)*sin(phig) +                              
     +     figrotm(2,3)*cos(thetac)                                          
      A3 = figrotm(3,1)*sin(thetac)*cos(phig) +                              
     +     figrotm(3,2)*sin(thetac)*sin(phig) +                              
     +     figrotm(3,3)*cos(thetac)                                         


*     if photon is backward A3<0 
*     fphilgap remains unchanged so that rzero procedure can have troubles in
*     leading with
*
*                +
*              +
*             +
*__________________________________ >  
*
*        +++++
*       +          
*      +            
*
      if (A3.lt.0.) goto 99

      ZBOTTOM = par(4)

* --- Roots function

      If (RCGEOM(1).ne.9) then !FLIGHT


* ...... Radiator outer boundary (cylinder)

            A  = (ZBOTTOM - pcervtx(3))**2 * ((A1/A3)**2+(A2/A3)**2)
            B  = 2*(ZBOTTOM-pcervtx(3))*(pcervtx(1)*A1/A3 +
     +                                   pcervtx(2)*A2/A3)
            C  =  pcervtx(1)**2 + pcervtx(2)**2 - RTMIR**2
            fphilgap = A + B + C

      endif

* --- end                                                                   
 99   return                                                                
      end                                                                   

* =======================================================================
      real Function fphicirc_2par(phig,ii)
* =======================================================================
      real phig
      integer ii

      fphicirc_2par = fphicirc(phig)

* --- end
      return
      end


* ==========================================================
      real function fphicirc(phig)
* ==========================================================
*************************************************************
*                                                           *
* variable : phig (parameter of geratrix Cherenkov cone )   *
*                                                           *
*                                                           *
* PARAMETERS :                                              *
*         IN : thetac (via common)                          *
*         OUT:                                              *
*                                                           *
* The zeros of this function give the geratrix Cone         *
* intersection phig with a circumference at PMT zlevel.     *
* The function takes an init value=1E8 when not defined     *
* because of photons not reaching this z level              *
* function signal meaning:                                  *
*            >0 when photons reach outside area under study *
*                              (track impact zone)          *
*            <0 when photons reach inside area under study  *
*                                                           *
* in FLIGHT:  used for exclusion of ring portion            *
*             close to charged particle signal              *
*                                                           *
* in PROTOT:                                                *
*                                                           *
*     13/11/2008                                            *
*                                                           *
*************************************************************  

#include "../include/richrec_lipf.h"

      common /parfunc/ par(10)
      !$OMP THREADPRIVATE(/parfunc/)
      character chin*3,chout*3
      real phpto(3),phdir(3)
      save fphildummy     
      data fphildummy/1.E8/
      !$OMP THREADPRIVATE(fphildummy)
      real cshift

* --- init
      nmiref = 0
      fphicirc = fphildummy

* --- parameter          
      thetac = par(2)
      xc = par(5)
      yc = par(6)
      crad = par(7)

* --- rotation matrix between detector frame and particle frame            
      if (irotflg.eq.0) then                                               
         call patmatr                                                      
         irotflg = 1                                                       
      endif                                                                   
* --- photon emission point                                   
      call ucopy(pcervtx,phpto,3)                                    
* --- generate photon on detector frame
      call patphot(phig,thetac,phdir)
* --- test photon direction: backward?                                    
      if (phdir(3).lt.0.) return                               
* --- refraction at radiator exit                                 
      if (chradid.eq.'AGL'.or. chradid.eq.'NAF') then !photons radiated in AGL or NAF
        call patints(phpto,phdir,'RADB',ierr)                       
        if (ierr.ne.0) return    
        chin = chradid
        if (HPGL.ne.0.0) then
          call patrefr(chin,'PGL',phdir,ierr) !refraction
          if (ierr.ne.0) return
          call patints(phpto,phdir,'PGLB',ierr)
          if (ierr.ne.0) return
          call patrefr('PGL','AIR',phdir,ierr) !refraction             
          if (ierr.ne.0) return
        else
          call patrefr(chin,'AIR',phdir,ierr) !refraction
          if (ierr.ne.0) return             
        endif
      elseif (chradid.eq.'PGL') then
        if (HPGL.eq.0.0) then
          print*,' >>> fphilhole: photons starting in PGL when HPGL=0!'
          return
        endif  
        call patints(phpto,phdir,'PGLB',ierr) !intersection           
        if (ierr.ne.0) return
        call patrefr('PGL','AIR',phdir,ierr) !refraction             
        if (ierr.ne.0) return
      endif
     
* --- intersect photon with PMT plane
      call patints(phpto,phdir,'PMTD',ierr)                            
* --- mirror reflection                   
      if (rcgeom(1).ne.9.or.(rcgeom(1).eq.9.and.rcgeom(2).eq.1)) then  !flight configuration  
        rpoint = sqrt((phpto(1)-vtmir(1))**2+(phpto(2)-vtmir(2))**2)
        do while (rpoint.gt.RBMIR)
          nmiref = nmiref + 1
          if (nmiref.gt.10) return 
          call patints(phpto,phdir,'MIRR',ierr) !intersec with mirror
          if (ierr.ne.0) return                                      
          call patrefl(phpto,phdir,ierr) !reflection with mirror          
          call patints(phpto,phdir,'PMTD',ierr)                      
          rpoint = sqrt((phpto(1)-vtmir(1))**2+(phpto(2)-vtmir(2))**2)
        enddo                                                         
      endif

*-----parametrization of the function                                      
      gfun1      = phpto(1) + phdir(1)/phdir(3)*(ZPMTDET-phpto(3))
      gfun2      = phpto(2) + phdir(2)/phdir(3)*(ZPMTDET-phpto(3))

      fphicirc = sqrt((gfun1-xc)**2.+(gfun2-yc)**2.)-crad

* --- end
      return
      END                                                                    

* =======================================================================
      real Function fphimsec_2par(phig,ii)
* =======================================================================
      real phig
      integer ii

      fphimsec_2par = fphimsec(phig)

* --- end
      return
      end


* ==========================================================
      real function fphimsec(phig)
* ==========================================================
*************************************************************
*                                                           *
* variables : phig (parameter of geratrix Cherenkov cone )  *
*                                                           *
*                                                           *
* PARAMETERS :                                              *
*         IN : thetac (via common)                          *
*         OUT:                                              *
*                                                           *
* The zeros of this function give the points where the      *
* intersection of the geratrix cone with the mirror crosses *
* the border between mirror sectors.                        *
*                                                           *
* in FLIGHT:  used to split reflected portion of ring into  *
*             portions reflected by different mirror        *
*             sectors                                 .     *
*                                                           *
* in PROTOT:  (not used)                                    *
*                                                           *
*     06/05/2009                                            *
*                                                           *
*************************************************************  

#include "../include/richrec_lipf.h"

      common /parfunc/ par(10)
      !$OMP THREADPRIVATE(/parfunc/)
      character chin*3,chout*3
      real phpto(3),phdir(3)
      save fphildummy     
      data fphildummy/1.E8/
      !$OMP THREADPRIVATE(fphildummy)
      real cshift

* --- init
      nmiref = 0
      fphimsec = fphildummy

* --- parameter          
      thetac = par(2)
      xc = par(5)
      yc = par(6)
      crad = par(7)

* --- rotation matrix between detector frame and particle frame            
      if (irotflg.eq.0) then                                               
         call patmatr                                                      
         irotflg = 1                                                       
      endif                                                                   
* --- photon emission point                                   
      call ucopy(pcervtx,phpto,3)                                    
* --- generate photon on detector frame
      call patphot(phig,thetac,phdir)
* --- test photon direction: backward?                                    
      if (phdir(3).lt.0.) return                               
* --- refraction at radiator exit                                 
      if (chradid.eq.'AGL'.or. chradid.eq.'NAF') then !photons radiated in AGL or NAF
        call patints(phpto,phdir,'RADB',ierr)                       
        if (ierr.ne.0) return    
        chin = chradid
        if (HPGL.ne.0.0) then
          call patrefr(chin,'PGL',phdir,ierr) !refraction
          if (ierr.ne.0) return
          call patints(phpto,phdir,'PGLB',ierr)
          if (ierr.ne.0) return
          call patrefr('PGL','AIR',phdir,ierr) !refraction             
          if (ierr.ne.0) return
        else
          call patrefr(chin,'AIR',phdir,ierr) !refraction
          if (ierr.ne.0) return             
        endif
      elseif (chradid.eq.'PGL') then
        if (HPGL.eq.0.0) then
          print*,' >>> fphilhole: photons starting in PGL when HPGL=0!'
          return
        endif  
        call patints(phpto,phdir,'PGLB',ierr) !intersection           
        if (ierr.ne.0) return
        call patrefr('PGL','AIR',phdir,ierr) !refraction             
        if (ierr.ne.0) return
      endif
     
* --- intersect photon with PMT plane
      call patints(phpto,phdir,'PMTD',ierr)                            
* --- mirror reflection                   
      if (rcgeom(1).ne.9.or.(rcgeom(1).eq.9.and.rcgeom(2).eq.1)) then  !flight configuration  
        rpoint = sqrt((phpto(1)-vtmir(1))**2+(phpto(2)-vtmir(2))**2)                        
        do while (rpoint.gt.RBMIR)
          nmiref = nmiref + 1
          if (nmiref.gt.10) return 
          call patints(phpto,phdir,'MIRR',ierr) !intersec with mirror
          if (ierr.ne.0) return                                      
          call patrefl(phpto,phdir,ierr) !reflection with mirror          
          call patints(phpto,phdir,'PMTD',ierr)                      
          rpoint = sqrt((phpto(1)-vtmir(1))**2+(phpto(2)-vtmir(2))**2)                        
        enddo                                                         
      endif

* --- calculation of function value
      if(nmiref.eq.0) then  ! exclude "fake" zeros in non-reflected branches
         fphimsec = 1.e8
      else
         phipoint = atan2(phpto(2)-vtmir(2),phpto(1)-vtmir(1))
         if(phipoint.lt.0.) phipoint = phipoint+TWOPI
         fphimsec = 1.e8
         do i=1,nmirsec
            fphimsec_test = phipoint-phimirs(i)
            if(fphimsec_test.lt.-PI) fphimsec_test = fphimsec_test+TWOPI
            if(abs(fphimsec_test).lt.abs(fphimsec)) then
               fphimsec = fphimsec_test
            endif
         enddo
      endif

* --- end
      return
      END                                                                    

* +++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
* +++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
* +++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++

* =======================================================================   
      subroutine tileroots(x1,x2,y1,y2,ig,vphival)
* =======================================================================   

#include "../include/richrec_lipf.h"

      parameter (nrootmax=200)

      real par
      common /parfunc/ par(10)
      !$OMP THREADPRIVATE(/parfunc/)
      real x1,x2,y1,y2,vphival(*),root,r
      integer ig, nroots, i

      real xb1(nrootmax),xb2(nrootmax),yb1(nrootmax),yb2(nrootmax)

      real fphilrad, fphilrad_2par
      external fphilrad
      external fphilrad_2par

      real EPS
      integer MAXF
      DATA EPS,MAXF /1.E-4,1000/
      SAVE EPS,MAXF
      !$OMP THREADPRIVATE(EPS,MAXF)
      real phpto(3),phdir(3)
      integer ierr

* ... init
      nradt=1
      if (ig.eq.2) goto 99
CC      print*,'(tileroots) limits=',x1,x2,y1,y2

* ... cerenkov angle
      thc=par(2)
*      print*,'thc',thc
* ... intersections with radiator walls
     
* ==> radiator walls intersection controlled with par(10)
      par(10) = 1.     

* ==> X left
      par(1) = x1
      par(9) = 1.
CC      print*,'------> X left'
      call zbrak(fphilrad,0.,twopi,nrootmax,xb1,xb2,yb1,yb2,nroots)
      do 1 i=1,nroots      
*         call rzero(xb1(i),xb2(i),root,R,EPS,MAXF,fphilrad_2par)
         call rzero_alt(xb1(i),xb2(i),root,R,EPS,MAXF,fphilrad_2par)

* ... photon emission point
         call ucopy(pcervtx,phpto,3)
* ... generate photon on detector frame
         call patphot(root,thc,phdir)
* ... photon is bakward?
         if(phdir(3).lt.0.) goto 1
* ... get y intersection in the radiator bottom
         call patints(phpto,phdir,'RADB',ierr)
        
         if(phpto(2).gt.y1.and.phpto(2).lt.y2)then
            nradt=nradt+1
            vphival(nradt)=root
         endif
 1    continue
   

* ==> X right
      par(1) = x2
      par(9) = 1.
CC      print*,'------> X right'
      call zbrak(fphilrad,0.,twopi,nrootmax,xb1,xb2,yb1,yb2,nroots)
      do 2 i=1,nroots      
*         call rzero(xb1(i),xb2(i),root,R,EPS,MAXF,fphilrad_2par)
         call rzero_alt(xb1(i),xb2(i),root,R,EPS,MAXF,fphilrad_2par)

* ... photon emition point
         call ucopy(pcervtx,phpto,3)
* ... generate photon on detector frame
         call patphot(root,thc,phdir)
* ... photon is bakward?
         if(phdir(3).lt.0.) goto 2
* ... get y intersection in the radiator bottom
         call patints(phpto,phdir,'RADB',ierr)
         if(phpto(2).gt.y1.and.phpto(2).lt.y2)then
            nradt=nradt+1
            vphival(nradt)=root
         endif
 2    continue

* ==> y down
      par(1) = y1
      par(9) = 2.
CC      print*,'------> Y down'
      call zbrak(fphilrad,0.,twopi,nrootmax,xb1,xb2,yb1,yb2,nroots)
      do 3 i=1,nroots      
*         call rzero(xb1(i),xb2(i),root,R,EPS,MAXF,fphilrad_2par)
         call rzero_alt(xb1(i),xb2(i),root,R,EPS,MAXF,fphilrad_2par)

* ... photon emition point
         call ucopy(pcervtx,phpto,3)
* ... generate photon on detector frame
         call patphot(root,thc,phdir)
* ... photon is bakward?
         if(phdir(3).lt.0.) goto 3
* ... get y intersection in the radiator bottom
         call patints(phpto,phdir,'RADB',ierr)
         if(phpto(1).gt.x1.and.phpto(1).lt.x2)then
            nradt=nradt+1
            vphival(nradt)=root
         endif
 3    continue

* ==> y up
      par(1) = y2
      par(9) = 2.

      call zbrak(fphilrad,0.,twopi,nrootmax,xb1,xb2,yb1,yb2,nroots)

      do 4 i=1,nroots      
*         call rzero(xb1(i),xb2(i),root,R,EPS,MAXF,fphilrad_2par)
         call rzero_alt(xb1(i),xb2(i),root,R,EPS,MAXF,fphilrad_2par)

* ... photon emition point
         call ucopy(pcervtx,phpto,3)
* ... generate photon on detector frame
         call patphot(root,thc,phdir)
* ... photon is bakward?
         if(phdir(3).lt.0.) goto 4
* ... get y intersection in the radiator bottom
         call patints(phpto,phdir,'RADB',ierr)
*         print*,'phpto',phpto
         if(phpto(1).gt.x1.and.phpto(1).lt.x2)then
            nradt=nradt+1
            vphival(nradt)=root
         endif
 4    continue
*      print*,'nradt',nradt
*      print*,'vphival tileroots',(vphival(i),i=1,nradt+1)
 99   continue
      vphival(1) = real(nradt-1)

      end

* =======================================================================   
       subroutine HLIMASYM(xlow,xhig,ylow,yhig)
* =======================================================================   
*************************************************************************
* PARAMETERS
*       IN  : bounds on x and y
*             imod=0 - take sign of bounds
*             imod=1 - take absolute values of bounds
*       OUT :
*
*      Rev: 16/07/2002
*************************************************************************

#include "../include/richrec_lipf.h"

      integer itmod,nphmax,nholt  ,nermax
      parameter(nphmax=NPHIRMAX)
      parameter(nermax=10)
      integer verr(nermax)       
      parameter (nrootmax=200)
      integer nrefhol(nphmax)
      real philhol(nphmax)
      real xb1(nrootmax),xb2(nrootmax), yb1(nrootmax),yb2(nrootmax)

      integer nrhol
      real r, root
      data EPS,MAXF /1.E-4,1000/
      SAVE EPS,MAXF
      !$OMP THREADPRIVATE(EPS,MAXF)

      real fphilhole,fphilhole_2par
      external fphilhole
      external fphilhole_2par

      common /parfunc/par(10)
      !$OMP THREADPRIVATE(/parfunc/)
      common/holevar/nholt,philhol,nrefhol
      !$OMP THREADPRIVATE(/holevar/)

         thetac=par(2)
         nrhol=nrootmax

         call zbrak(fphilhole,0.,twopi,nrootmax,xb1,xb2,yb1,yb2,nrhol)
         do i=1,nrhol
           dfphilhole=yb1(i)-yb2(i)
           if (abs(dfphilhole).lt.1.E6) then             
*             call rzero(xb1(i),xb2(i),root,r,EPS,MAXF,fphilhole_2par)
              call rzero_alt(xb1(i),xb2(i),root,r,EPS,MAXF,
     +                       fphilhole_2par)

              if (root.ne.0.) then   
                 call patrace_full(root,thetac,xdet,ydet,nmiref,nerr,
     +                             verr)
                 if (
     &                ydet.le.yhig.and.ydet.ge.ylow.and.
     &                xdet.le.xhig.and.xdet.ge.xlow) then
*     nmireft = nmireft + nmiref                         
                    nholt   = nholt + 1       
                    philhol(nholt) = root      
                    nrefhol(nholt) = nmiref
                 endif
              endif                         
           endif                                                   
        enddo                                                      
        
* --- end                                                                   
 99   return                                                                
      end                                                                   



* =======================================================================   
       subroutine HLIMSYM(xlow,xhig,ylow,yhig)
* =======================================================================   
*************************************************************************
* PARAMETERS
*       IN  : bounds on x and y
*             imod=0 - take sign of bounds
*             imod=1 - take absolute values of bounds
*       OUT :
*
*      Rev: 16/07/2002
*************************************************************************

#include "../include/richrec_lipf.h"

      integer itmod,nphmax,nermax 
      parameter(nphmax=NPHIRMAX)
      parameter(nermax=10)
      integer verr(nermax)       

      parameter (nrootmax=200)
      integer nrefhol(nphmax)
      real philhol(nphmax)
      real xb1(nrootmax),xb2(nrootmax), yb1(nrootmax),yb2(nrootmax)
      real root, r
      integer nrhol

      data EPS,MAXF /1.E-4,1000/
      SAVE EPS,MAXF
      !$OMP THREADPRIVATE(EPS,MAXF)

      real fphilhole, fphilhole_2par
      external fphilhole
      external fphilhole_2par

      common /parfunc/par(10)
      !$OMP THREADPRIVATE(/parfunc/)
      common/holevar/nholt,philhol,nrefhol
      !$OMP THREADPRIVATE(/holevar/)

      thetac=par(2)
      nrhol=nrootmax
      
      call zbrak(fphilhole,0.,twopi,nrootmax,xb1,xb2,yb1,yb2,nrhol)
      
      
      do i=1,nrhol
         dfphilhole=yb1(i)-yb2(i)
         if (abs(dfphilhole).lt.1.E6) then  
            
*     call rzero(xb1(i),xb2(i),root,r,EPS,MAXF,fphilhole_2par)
              call rzero_alt(xb1(i),xb2(i),root,r,EPS,MAXF,
     +           fphilhole_2par)
              
              if (root.ne.0.) then   
                 call patrace_full(root,thetac,xdet,ydet,nmiref,nerr,
     +                             verr)
                 if (
     &                abs(ydet).le.yhig.and.abs(ydet).ge.ylow.and.
     &                abs(xdet).le.xhig.and.abs(xdet).ge.xlow) then
*                  nmireft = nmireft + nmiref                         
                    nholt   = nholt + 1       
                    philhol(nholt) = root      
                    nrefhol(nholt) = nmiref
                 endif
              endif                         
           endif                                                   
        enddo                                                      
        
*     --- end                                                                   
 99     return                                                                
        end                                                                   



* =======================================================================   
       subroutine HLIMCIRC(xc,yc,crad)
* =======================================================================   
*************************************************************************
* PARAMETERS
*       IN  : xc,yc - centre of circle
*             crad - radius of circle
*       OUT :
*
*      13/11/2008
*************************************************************************

#include "../include/richrec_lipf.h"

      integer itmod,nphmax,nermax 
      parameter(nphmax=NPHIRMAX)
      parameter(nermax=10)
      integer verr(nermax)       

      integer ncirt
      parameter (nrootmax=200)
      integer nrefcir(nphmax)
      real philcir(nphmax)
      real xb1(nrootmax),xb2(nrootmax), yb1(nrootmax),yb2(nrootmax)

      integer nrcir

      real r, root
      data EPS,MAXF /1.E-4,1000/
      SAVE EPS,MAXF
      !$OMP THREADPRIVATE(EPS,MAXF)
      
      real fphicirc, fphicirc_2par
      external fphicirc
      external fphicirc_2par

      common /parfunc/par(10)
      common/circvar/ncirt,philcir,nrefcir
      !$OMP THREADPRIVATE(/parfunc/)
      !$OMP THREADPRIVATE(/circvar/)
      thetac=par(2)
      nrcir=nrootmax

      call zbrak(fphicirc,0.,twopi,nrootmax,xb1,xb2,yb1,yb2,nrcir)
      do i=1,nrcir
         dfphicirc=yb1(i)-yb2(i)
         if (abs(dfphicirc).lt.1.E6) then             
*            call rzero(xb1(i),xb2(i),root,r,EPS,MAXF,fphicirc_2par)
            call rzero_alt(xb1(i),xb2(i),root,r,EPS,MAXF,fphicirc_2par)

            if (root.ne.0.) then   
               call patrace_full(root,thetac,xdet,ydet,nmiref,nerr,verr)
*               nmireft = nmireft + nmiref                         
               ncirt   = ncirt + 1       
               philcir(ncirt) = root      
               nrefcir(ncirt) = nmiref
            endif                         
         endif                                                   
      enddo                                                      

* --- end                                                                   
 99   return                                                                
      end                                                                   


* =======================================================================   
       subroutine HLIMMSEC
* =======================================================================   
*************************************************************************
*      06/05/2009
*************************************************************************

#include "../include/richrec_lipf.h"

      integer itmod,nphmax,nermax 
      parameter(nphmax=NPHIRMAX)
      parameter(nermax=10)
      integer verr(nermax)       

      integer nmslt
      parameter (nrootmax=200)
      integer nrefmsl(nphmax)
      real philmsl(nphmax)

      real xb1(nrootmax),xb2(nrootmax), yb1(nrootmax),yb2(nrootmax)

      integer nrmsl

      real root,r
      data EPS,MAXF /1.E-4,1000/
      SAVE EPS,MAXF
      !$OMP THREADPRIVATE(EPS,MAXF)

      real fphimsec, fphimsec_2par
      external fphimsec
      external fphimsec_2par

      common /parfunc/par(10)
      common/mslvar/nmslt,philmsl,nrefmsl
      !$OMP THREADPRIVATE(/parfunc/)
      !$OMP THREADPRIVATE(/mslvar/)
      thetac=par(2)
      nrmsl=nrootmax

      call zbrak(fphimsec,0.,twopi,nrootmax,xb1,xb2,yb1,yb2,nrmsl)
      do i=1,nrmsl

         dfphimsec=yb1(i)-yb2(i)
         if (abs(dfphimsec).lt.1.E6) then             
*            call rzero(xb1(i),xb2(i),root,r,EPS,MAXF,fphimsec_2par)
            call rzero_alt(xb1(i),xb2(i),root,r,EPS,MAXF,fphimsec_2par)

            if (root.ne.0.) then   
               call patrace_full(root,thetac,xdet,ydet,nmiref,nerr,verr)
*               nmireft = nmireft + nmiref                         
               nmslt   = nmslt + 1       
               philmsl(nmslt) = root      
               nrefmsl(nmslt) = nmiref
            endif                         
         endif                                                   
      enddo                                                      

* --- end                                                                   
 99   return                                                                
      end                                                                   


* =====================================================================================
* UTILITY ROUTINES
* =====================================================================================

* ======================================================================
      subroutine getphivis(thetac,npvis,phimin,phimax)                     
* ======================================================================
*
* GET PHOTON PHI LIMITS FOR VISIBLE PATHS
*                                                                      
* PARAMETERS:                                                          
*         IN: thetac ........ cerenkov angle                           
*                                                                     
*        OUT:
*             npvis ......... Nb of visible pattern branches
*             phimin,phimax . Corresponding phi limits
*
* ======================================================================

#include "../include/richrec_lipf.h"

      integer nphmax 
      parameter(nphmax=NPHIRMAX)
      parameter(nermax=10)
      integer nphitot,
     +        fphitot(nphmax),
     +        fphipat(nphmax),
     +        msecpat(nphmax),
     +        verr(nermax),
     +        npvis

      real
     +     vphitot(nphmax),
     +     dphidir(nphmax),
     +     dphihol(nphmax),
     +     dphimir0(nphmax),
     +     dphimir1(nphmax),
     +     dphiinv(nphmax),
     +     phimin(nphmax),
     +     phimax(nphmax)

      real dphimsec0(nmaxmirsec,nphmax),
     +     dphimsec1(nmaxmirsec,nphmax)

c      DATA EPS /1.E-2/
c      external fphotdist

c      data idummy /0/

* --- init
      visacc=-999.

* --- compute photon phi root values for diferent functions

*      print*,'Going to call set_phiroots...'
      call set_phiroots(thetac,nphitot,vphitot,fphitot)
*      print*,'End call set_phiroots'
*
      if (ldebug.ge.2) then
         print*,' '
         print'(A)','******** PHI ROOTS *************'    
         print*,' Roots Number',nphitot
         do i=1,nphitot
            xdet = -999.
            ydet = -999.                           
            call patrace_full
     .           (vphitot(i),thetac,xdet,ydet,nmiref,nerr,verr)
            print'(A,2x,I2,2x,A,2x,F6.1,2x,A,2x,I3,2x,A,2X,2(F6.1,1x))',
     +           'NB=',i,'PHIG=',vphitot(i)*RADDEG,'FLAG=',fphitot(i),
     +           'X,Y=',xdet,ydet
         enddo        
      endif

* --- compute path lengths
      call dphipath(thetac,nphitot,vphitot,fphitot,
     +              dphidir,dphihol,dphimir0,dphimir1,dphiinv,
     +              npvis,phimin,phimax,fphipat,msecpat,
     +              dphimsec0,dphimsec1)

      if (ldebug.ge.2) then     
         print*,' '
         print'(A)','******** ACCEPTANCE ********'   
         print'(A,i2)',' Nb Vis branches ',npvis
         print*,'VISIBLE deltaphi ',
     .        (raddeg*(phimax(i)-phimin(i)),i=1,npvis)
         print*,' '
         print*,'DPHIDIR =',(RADDEG*DPHIDIR(I),I=2,INT(DPHIDIR(1))+1)
      endif


* --- return
      return
      end     


* ======================================================================
      subroutine getmirsec(phimir,imsec)                     
* ======================================================================
*
* GET MIRROR SECTOR GIVEN MIRROR ANGLE
*                                                                      
* PARAMETERS:                                                          
*         IN: phimir ........ mirror phi
*        OUT: imsec ......... mirror sector
*
*             22/May/2009 - R.Pereira
*
* ======================================================================

#include "../include/richrec_lipf.h"

      real phimir
      integer imsec

      phitest = phimir

      irawmsec = 1
      if(phitest.lt.phimirs(1)) then
         do i=2,nmirsec
            if(phitest.ge.phimirs(i)) then
               irawmsec = i
            endif
         enddo
      endif

      imsec = idmirs(irawmsec)

*      print*,'(getmirsec) phimir,imsec = ',phimir,imsec

* --- return
      return
      end     


********************************
*** CODE FROM richaccsmear.F ***
********************************

      subroutine richaccsmear(thc,zch,richacca,richeffa,
     +                        richaccmseca,richeffmseca)
***************************************************************************
* Returns the ring acceptance smeared over the ring width
*
* IN : thc = cerenkov angle (radians)
*      zch = charge (1, 2, 3, 4, ...) -> Luisa, FB implementation
*                  (99)              -> Rui Pereira Implementation (TBeam)
*                  
* OUT: richacca = acceptance
*      richeffa = efficiencies (see meaning at richacpv11.F/richacc routine)
*      richaccmseca,
*      richeffmseca = same by mirror sector (R.Pereira 08/May/2009)
*  
***************************************************************************

#include "../include/richrec_lipf.h"
      
      integer zch
      real thc
      real richacca(5),richeffa(8)
      real richaccmseca(nmaxmirsec,2),richeffmseca(nmaxmirsec,8)

* ... ADDITIONAL LOCAL VARIABLES for smeared acceptance calculation
* ... R. Pereira 24-Jun-2005
       real avgrichacca(5),avgricheffa(8)
       real avgrichaccmseca(nmaxmirsec,2),avgricheffmseca(nmaxmirsec,8)
       real radsigma,raddelta,thetacsmear
       integer kas1,kas2

      if (zch.eq.0) zch = 1

* ... Reset variables
      call vzero(avgrichacca,5)
      call vzero(avgricheffa,8)
      call vzero(avgrichaccmseca,nmaxmirsec*2)
      call vzero(avgricheffmseca,nmaxmirsec*8)
      
      IF (ZCH.NE.99) THEN       !-----------------------------------------------------------------
*     ...... thetac resolution
         sigmabetarel =  1.E-2*sqrt( (a_beta/zch)**2 + b_beta**2 )
         sigmathetac  = sigmabetarel/tan(thc)

*     ...... evaluate acceptances for different thetac
         wttot = 0.


         do k=1,5               !  5 rings are taken
            thetacsmear=thc+sigmathetac*(float(k)-3.) !  rings at -2,-1,0,+1,+2 sigma
            call richacc(thetacsmear,richacca,richeffa,
     +           richaccmseca,richeffmseca)
            wt = exp(-(float(k)-3.)**2./2.)
            do j=1,5
               avgrichacca(j) = avgrichacca(j) + richacca(j)*wt   
            enddo
            do j=1,8
               avgricheffa(j) = avgricheffa(j) + richeffa(j)*wt   
            enddo
            do j=1,nmaxmirsec
               do i=1,2
                  avgrichaccmseca(j,i) = avgrichaccmseca(j,i)
     +                 +richaccmseca(j,i)*wt
               enddo
               do i=1,8
                  avgricheffmseca(j,i) = avgricheffmseca(j,i)
     +                 +richeffmseca(j,i)*wt
               enddo
            enddo
            wttot = wttot + wt  
         enddo
         do j=1,5
            richacca(j) = avgrichacca(j)/wttot 
         enddo
         do j=1,8
            richeffa(j) = avgricheffa(j)/wttot
         enddo         
         do j=1,nmaxmirsec
            do i=1,2
               richaccmseca(j,i) = avgrichaccmseca(j,i)/wttot
            enddo
            do i=1,8
               richeffmseca(j,i) = avgricheffmseca(j,i)/wttot
            enddo
         enddo
      

      ELSE                      !-----------------------------------------------------------------

*     ... SMEARED ACCEPTANCE CALCULATION
*     ... R. Pereira 24-Jun-2005
         
*     ... Estimated ring width (cm)
         radsigma = 0.4         ! Fair estimate for any AGL in test beam

         do kas1=1,5            !  5 rings are taken
            raddelta = radsigma*(kas1-3.) !  rings at -2,-1,0,+1,+2 sigma
            
*     ... Equivalence of changed ring width in theta_c
          thetacsmear = acos(((cos(thetac))**2.-2.*raddelta*sin(thetac)*
     +                       (cos(thetac))**3./rich_height)**0.5)

*     ... compute acceptances and efficiencies
            call richacc(thetacsmear,richacca,richeffa,
     +         richaccmseca,richeffmseca)

*         print*,'ring',kas1-3,'dir accep = ',richacca(2)

*     ... add result to total
            do kas2=1,5
               avgrichacca(kas2) = avgrichacca(kas2)
     +              +richacca(kas2)*exp(-(kas1-3.)**2./2.) ! exp accounts for gaussian height
            enddo
            do kas2=1,8
               avgricheffa(kas2) = avgricheffa(kas2)
     +              +richeffa(kas2)*exp(-(kas1-3.)**2./2.) ! exp accounts for gaussian height
            enddo
            do kas2=1,nmaxmirsec
               do i=1,2
                  avgrichaccmseca(kas2,i) = avgrichaccmseca(kas2,i)
     +                 +richaccmseca(kas2,i)*exp(-(kas1-3.)**2./2.) ! exp accounts for gaussian height
               enddo
               do i=1,8
                  avgricheffmseca(kas2,i) = avgricheffmseca(kas2,i)
     +                 +richeffmseca(kas2,i)*exp(-(kas1-3.)**2./2.) ! exp accounts for gaussian height
               enddo
            enddo
            
         enddo
         
*     ... Normalize results, revert to original variables
         do kas2=1,5
            richacca(kas2) = avgrichacca(kas2)/(1+2*exp(-2.)+2*exp(-.5)) ! sum of gaussian heights
         enddo
         do kas2=1,8
            richeffa(kas2) = avgricheffa(kas2)/(1+2*exp(-2.)+2*exp(-.5)) ! sum of gaussian heights
         enddo
         do kas2=1,nmaxmirsec
            do i=1,2
               richaccmseca(kas2,i) = avgrichaccmseca(kas2,i)
     +              /(1+2*exp(-2.)+2*exp(-.5)) ! sum of gaussian heights
            enddo
            do i=1,8
               richeffmseca(kas2,i) = avgricheffmseca(kas2,i)
     +              /(1+2*exp(-2.)+2*exp(-.5)) ! sum of gaussian heights
            enddo
         enddo
         
*     print*,'FINAL DIR ACCEP = ',richacca(2)

      ENDIF
* ... END SMEARED ACCEPTANCE CALCULATION

      !-------------------------------------------------------------------------------------------------

      return
      end


*********************************
*** CODE FROM richaccpmtgap.F ***
*********************************


      subroutine richaccpmtgap(accgap,accgapdir,accgapref,accgapmsec)
**********************************************************************
* Returns the geometrical detection efficiencies due to PMT gaps
*     OUT: accgap    --- detection efficiency due to gaps (geometrical)
*          accgapdir --- direct detection efficiency due to gaps
*          accgapref --- reflected detection efficiency due to gaps
*
* Updated: 28-Nov-2008 by R. Pereira
*
*     12-May-2009: hard-coded value 0.85 replaced by variable REFLEC,
*     (R.Pereira)  richcrd.inc added to get this variable
*
*     15-May-2009 (R.Pereira): mirror sectors included
*
*********************************************************************

#include "../include/richrec_lipf.h"

      real accgap,accgapdir,accgapref
      real accgapmsec(nmaxmirsec)

      real avaccgap,avaccgapdir,avaccgapref
      real avaccgapmsec(nmaxmirsec)

* ... patrace variables
      real vdet(3),vdir(3)
      parameter(nphotmax=10)
      integer nerr,verr(nphotmax)

      real weight, weightotal
      integer xred, yred
      real x0, y0

      integer ngamma0,ngammadet,ngammagap
      integer ngamma0dir,ngammadetdir,ngammagapdir
      integer ngamma0ref,ngammadetref,ngammagapref
      integer ngamma0msec(nmaxmirsec),ngammadetmsec(nmaxmirsec),
     +     ngammagapmsec(nmaxmirsec)

      real spgamma0,spgammadet,spgammagap
      real spgamma0dir,spgammadetdir,spgammagapdir
      real spgamma0ref,spgammadetref,spgammagapref
      real spgamma0msec(nmaxmirsec),spgammadetmsec(nmaxmirsec),
     +     spgammagapmsec(nmaxmirsec)

      real phdir(3)

      real phimir
      integer imsec

      if(ipthetac.eq.0)return
      thetac=cangrec(ipthetac)

* ... thetac resolution
      zch=1.
      sigmabetarel = 1E-2*sqrt( (a_beta/zch)**2 + b_beta**2 )
      sigmathetac  = sigmabetarel/tan(thetac)  

*      print*,'sigmabetarel = ',sigmabetarel
*      print*,'sigmathetac = ',sigmathetac

* ... init efficiencies
      avaccgap    = 0.
      avaccgapdir = 0.
      avaccgapref = 0.
      do i=1,nmaxmirsec
         avaccgapmsec(i) = 0.
      enddo
      weightotal  = 0.

* ... calculate geometrical efficiency correction due to gaps
* ... ring width taken into account      
      do k=1,5                  !  5 rings are taken
* ...... init variables 
         accgap       =-999.
         accgapdir    =-999.
         accgapref    =-999.
         do i=1,nmaxmirsec
            accgapmsec(i) = -999.
         enddo

         ngamma0      = 0
         ngammadet    = 0
         ngammagap    = 0

         ngamma0dir   = 0
         ngammadetdir = 0
         ngammagapdir = 0

         ngamma0ref   = 0
         ngammadetref = 0
         ngammagapref = 0

         do i=1,nmaxmirsec
            ngamma0msec(i)   = 0
            ngammadetmsec(i) = 0
            ngammagapmsec(i) = 0
         enddo

         spgamma0      = 0.
         spgammadet    = 0.
         spgammagap    = 0.
         spgamma0dir   = 0.
         spgammadetdir = 0.
         spgammagapdir = 0.
         spgamma0ref   = 0.
         spgammadetref = 0.
         spgammagapref = 0.
         
         do i=1,nmaxmirsec
            spgamma0msec(i)   = 0
            spgammadetmsec(i) = 0
            spgammagapmsec(i) = 0
         enddo

* ...... thetac
         allthetac=thetac+sigmathetac*(float(k)-3.) !  rings at -2,-1,0,+1,+2 sigma
* ...... ring sampling
         ierr = 0
         do phig=0.,twopi,1.E-2

* ......... trace photon to matrix and check if photon is LG gap
            call patrace_full_phimir(phig,allthetac,xdet,ydet,nmiref,
     +           nerr,verr,phimir)
            call getmirsec(phimir,imsec)
            vdet(1) = xdet
            vdet(2) = ydet
            vdet(3) = ZPMTDET
            if (nerr.eq.0) then
               !get pmt coordinates(x0,y0)
               call gridcoo(vdet(1),vdet(2),igrid,x0,y0)
*               print*,'vdet(1,2) = ',vdet(1),vdet(2)
*               print*,'x0,y0 = ',x0,y0
               xred = int((vdet(1)-x0)*100)
               yred = int((vdet(2)-y0)*100)
*               print*,'xred,yred (a) = ',xred,yred
               xred = mod(xred+7400,370)
               yred = mod(yred+7400,370)
*               print*,'xred,yred (b) = ',xred,yred
               if ( xred .gt. 340 .or. yred .gt. 340 ) then
                  nerr = nerr+1
                  verr(nerr) = 199
               endif
            endif

* ......... Scattering probability (R. Pereira 27-Nov-2008)
* ......... Calculation of interaction length was copied from RICHCHGREC
            if (chradid.eq.'AGL') then
               /* clari   = usrcla ! clarity : radint = lambda^4(micm)/clarity */
               alambda = 377.E-3 ! mean wavelength (micm) no foil (higher with foil)
               if (RADCLARITY.gt.0) then
                  alambda = (0.0327*RADCLARITY**(1.-0.867))**0.25
                  radint  = (alambda)**4/RADCLARITY   
               else
                  radint = 3.
               endif
            elseif(chradid.eq.'NAF')then
               radint = 1.E2
            endif

* ......... photon in detector frame
            call patphot(phig,allthetac,phdir)

            dist = (HRAD-pcervtx(3))/phdir(3) ! works for both radiators
*            print*,'dist = ',dist
*            print*,'phdir(3) = ',phdir(3)

            photprob = exp(-dist/radint)
            if(nmiref.ne.0)then
               photprob = photprob*reflec
            endif
*            print*,'photprob = ',photprob

* ......... counting photons 
            if (nerr.eq.0) then
               ngammadet = ngammadet+1
               spgammadet = spgammadet+photprob
               if(nmiref.eq.0)then
                  ngammadetdir = ngammadetdir+1
                  spgammadetdir = spgammadetdir+photprob
               else
                  if((imsec.lt.1).or.(imsec.gt.nmirsec)) then
                     print*,'(richaccpmtgap, 1st case...)'
                     print*,'Wrong mirror sector no. ! - ',imsec
                  endif
                  ngammadetref = ngammadetref+1
                  spgammadetref = spgammadetref+photprob
                  ngammadetmsec(imsec) = ngammadetmsec(imsec)+1
                  spgammadetmsec(imsec) = spgammadetmsec(imsec)+photprob
               endif
            elseif (verr(nerr).eq.199)then
               ngammagap = ngammagap+1
               spgammagap = spgammagap+photprob
               if (nmiref.eq.0)then
                  ngammagapdir = ngammagapdir+1
                  spgammagapdir = spgammagapdir+photprob
               else
                  if((imsec.lt.1).or.(imsec.gt.nmirsec)) then
                     print*,'(richaccpmtgap, 2nd case...)'
                     print*,'Wrong mirror sector no. ! - ',imsec
                  endif
                  ngammagapref = ngammagapref+1
                  spgammagapref = spgammagapref+photprob

                  ngammagapmsec(imsec) = ngammagapmsec(imsec)+1
                  spgammagapmsec(imsec) = spgammagapmsec(imsec)+photprob
               endif
            endif
         enddo
         
* ...... gap acceptance
* -----> overall
         ngamma0 = ngammadet + ngammagap
         spgamma0 = spgammadet + spgammagap
         if (ngamma0.ne.0) then
*            accgap = float(ngammadet)/float(ngamma0)
            if (spgamma0.gt.0) then
               accgap = spgammadet/spgamma0
            else
               accgap = 0.
            endif
         else
            accgap = -999.
         endif
* -----> direct         
         ngamma0dir = ngammadetdir + ngammagapdir
         spgamma0dir = spgammadetdir + spgammagapdir
         if (ngamma0dir.ne.0) then
*            accgapdir = float(ngammadetdir)/float(ngamma0dir)
            if (spgamma0dir.gt.0) then
               accgapdir = spgammadetdir/spgamma0dir
            else
               accgapdir = 0.
            endif
         else
            accgapdir = -999.
         endif
* -----> reflected         
         ngamma0ref = ngammadetref + ngammagapref
         spgamma0ref = spgammadetref + spgammagapref
         if (ngamma0ref.ne.0) then
*            accgapref = float(ngammadetref)/float(ngamma0ref)
            if (spgamma0ref.gt.0) then
               accgapref = spgammadetref/spgamma0ref
            else
               accgapref = 0.
            endif
         else
            accgapref = -999.
         endif

* -----> mirror sectors
         do i=1,nmaxmirsec
            ngamma0msec(i) = ngammadetmsec(i) + ngammagapmsec(i)
            spgamma0msec(i) = spgammadetmsec(i) + spgammagapmsec(i)
            if (ngamma0msec(i).ne.0) then
*            accgapmsec(i) =
*     +           float(ngammadetmsec(i))/float(ngamma0msec(i))
               if (spgamma0msec(i).gt.0) then
                  accgapmsec(i) = spgammadetmsec(i)/spgamma0msec(i)
               else
                  accgapmsec(i) = 0.
               endif
            else
               accgapmsec(i) = -999.
            endif
         enddo

*         print*,'CHRADID = ',chradid
*         print*,'K = ',k
*         print*,'ALLTHETAC = ',allthetac
*         print*,'N TOT: ',ngammadet,ngamma0,
*     +        float(ngammadet)/float(ngamma0)
*         print*,'N DIR: ',ngammadetdir,ngamma0dir,
*     +        float(ngammadetdir)/float(ngamma0dir)
*         print*,'N REF: ',ngammadetref,ngamma0ref,
*     +        float(ngammadetref)/float(ngamma0ref)

*         do i=1,nmirsec
*            print*,'N MSEC(',i,'): ',ngammadetmsec(i),ngamma0msec(i),
*     +           float(ngammadetmsec(i))/float(ngamma0msec(i))
*         enddo

*         print*,'SP TOT: ',spgammadet,spgamma0,
*     +        spgammadet/spgamma0
*         print*,'SP DIR: ',spgammadetdir,spgamma0dir,
*     +        spgammadetdir/spgamma0dir
*         print*,'SP REF: ',spgammadetref,spgamma0ref,
*     +        spgammadetref/spgamma0ref

*         do i=1,nmirsec
*            print*,'SP MSEC(',i,'): ',spgammadetmsec(i),spgamma0msec(i),
*     +           spgammadetmsec(i)/spgamma0msec(i)
*         enddo

         weight=exp(-0.5*(float(k)-3.)**2.)
         avaccgap    = avaccgap    + accgap*weight
         avaccgapdir = avaccgapdir + accgapdir*weight
         avaccgapref = avaccgapref + accgapref*weight

         do i=1,nmaxmirsec
            avaccgapmsec(i) = avaccgapmsec(i) + accgapmsec(i)*weight
         enddo

         weightotal  = weightotal + weight 
cc         print*,'k,avacc',k,avaccgap,avaccgapdir,avaccgapref
      enddo

      accgap    = avaccgap/weightotal 
      accgapdir = avaccgapdir/weightotal
      accgapref = avaccgapref/weightotal

      do i=1,nmaxmirsec
         accgapmsec(i) = avaccgapmsec(i)/weightotal
      enddo

*      print*,'accgap,accgapdir,accgapref: ',accgap,accgapdir,accgapref
*      print*,'accgapmsec: ',(accgapmsec(i),i=1,nmirsec)

      return
      end
      

******************************
*** CODE FROM richeffv10.F ***
******************************

******************************************************************
* rcradeff: radiator efficiency
* rcpmteff: lg/pmt eff
* rctoteff: total eff
******************************************************************
* ================================================================
       subroutine rcradnint(pardou,npaths,pphimin,pphimax,lphipat,           
     +                      pnint)                                 
* ================================================================         
******************************************************************         
* AIMS: non-interaction probability calculation taking into account:
*       - the radiator absorption and scattering effects    
*                                                                           
* PARAMETERS:                                                              
*         IN: pardou: parhrad - radiator height    [pardou(1)]
*                     parpthe - particle theta     [pardou(2)]
*                     parthec - cerenkov angle     [pardou(3)]   
*                     parlint - interaction length [pardou(4)]
*                     npaths  - number of (visible) path lengths to integrate 
*                     pphimin - phi limits of integration                      
*                     pphimax - """                                        
*                     lphipat - phi range flag 
*                               0=non-reflected
*                               1=1 reflection
*                               2=2 reflections (max at this moment)
*        OUT:                                                              
*             effout  - efficiency                                         
******************************************************************         

#include "../include/richrec_lipf.h"

      integer nphmax  
      parameter(nphmax=NPHIRMAX)
      DOUBLE PRECISION pphimin(nphmax),                                        
     +                 pphimax(nphmax),                                        
     +                 pardou(4),                                          
     +                 parhrad                                             
      INTEGER npaths,lphipat(nphmax)                                           
      REAL pnint                                                          
      DOUBLE PRECISION X(2),DGMLT2_ALT,DPHITOT                                         
      DOUBLE PRECISION FACTOR,PAREFF,Q2NEW,Q2TOT                           
      COMMON/RADEFF/PAREFF(10)                                             
      !$OMP THREADPRIVATE(/RADEFF/)
      EXTERNAL FSUB12                                                       
                                                                           
* ... init                                                                 
      do i=1,4                                                             
         pareff(i) = pardou(i)                                             
      enddo                                                                
* ... integration on Z                                                     
      ipcount = 0                                                          
      dphitot = 0.D0
      Q2TOT   = 0.D0                                                         
      do while (ipcount.lt.npaths)                                         
         ipcount = ipcount + 1                                             
*                                                                          
*         write(*,*)'pattern',lphipat(ipcount)
*         if (lphipat(ipcount).eq.1) then                                   
*            pareff(7) = 0.9                                                
*         else                                                              
            pareff(7) = dble(1.0)
*         endif                                                             
         pareff(5) = pphimin(ipcount)                                      
         pareff(6) = pphimax(ipcount)                                      
*                                                                          
         parhrad = pareff(1)    

         Q2NEW = DGMLT2_ALT(FSUB12,0.D0,parhrad,3,6,X)                          

         Q2TOT = Q2TOT + Q2NEW                                             
*        get phi total path
         dphitot = dphitot+DABS(DBLE(pphimax(ipcount)-pphimin(ipcount)))
         if (pphimax(ipcount).lt.pphimin(ipcount)) then   
            print*, npaths,pphimax(ipcount),pphimin(ipcount)  
            print*,'---------------------'
         endif
       enddo                                                                
      FACTOR = 1.d0/pareff(1)/dphitot                                    
      Q2TOT  = FACTOR*Q2TOT                                                 
* ... output (real)                                                        
      pnint = real(Q2TOT)                                                 
* ... end                                                                  
      return                                                               
      end                                                                  

* ================================================================
      subroutine rcradeff(pardou,npaths,pphimin,pphimax,lphipat,           
     +                    effout)                                 
* ================================================================         
******************************************************************         
* AIMS: efficiency calculation taking into account:
*       - the radiator absorption and scattering effects    
*                                                                           
* PARAMETERS:                                                              
*         IN: pardou: parhrad - radiator height    [pardou(1)]
*                     parpthe - particle theta     [pardou(2)]
*                     parthec - cerenkov angle     [pardou(3)]   
*                     parlint - interaction length [pardou(4)]
*                     npaths  - number of (visible) path lengths to integrate 
*                     pphimin - phi limits of integration                      
*                     pphimax - """                                        
*                     lphipat - phi range flag 
*                               0=non-reflected
*                               1=1 reflection
*                               2=2 reflections (max at this moment)
*
*        OUT:                                                              
*             effout  - efficiency                                         
******************************************************************         

#include "../include/richrec_lipf.h"

      integer nphmax  
      parameter(nphmax=NPHIRMAX)
      DOUBLE PRECISION pphimin(nphmax),                                        
     +                 pphimax(nphmax),                                        
     +                 pardou(4),                                          
     +                 parhrad                                             
      INTEGER npaths,lphipat(nphmax)                                           
      REAL effout                                                          
      DOUBLE PRECISION X(2),DGMLT2_ALT                                         
      DOUBLE PRECISION FACTOR,PAREFF,Q2NEW,Q2TOT                           
      COMMON/RADEFF/PAREFF(10)                                             
      !$OMP THREADPRIVATE(/RADEFF/)
      EXTERNAL FSUB12                                                       
                                                                           
* ... init                                                                 
      do i=1,4                                                             
         pareff(i) = pardou(i)                                             
      enddo                                                                
* ... integration on Z                                                     
      ipcount = 0                                                          
      Q2TOT   = 0.D0                                                         

      do while (ipcount.lt.npaths)                                         
         ipcount = ipcount + 1                                             
*                                                                          
*         if (lphipat(ipcount).eq.2) then                                   
*            pareff(7) = dble(reflec**2) 
*         elseif (lphipat(ipcount).eq.1) then                                   
*            pareff(7) = dble(reflec)
*         elseif (lphipat(ipcount).eq.0) then                                   
            pareff(7) = dble(1.0)
*         endif                                                             
         pareff(5) = pphimin(ipcount)                                      
         pareff(6) = pphimax(ipcount)                                      
*                                                                          
         parhrad = pareff(1)

         Q2NEW = DGMLT2_ALT(FSUB12,0.D0,parhrad,3,6,X)                          

         Q2TOT = Q2TOT + Q2NEW                                             

      enddo                                                                
      FACTOR = 1./dble(twopi)/pareff(1)                                    
      Q2TOT  = FACTOR*Q2TOT                                           

* ... output (real)                                                        
      effout = real(Q2TOT)                                                 
* ... end                                                                  
      return                                                               
      end                                                                  

*****************************************************************           
      subroutine FSUB12(M,U12,F12,X)                                           
*****************************************************************           
      DOUBLE PRECISION PAREFF                                               
      COMMON/RADEFF/PAREFF(10)                                              
      !$OMP THREADPRIVATE(/RADEFF/)
      DOUBLE PRECISION U12(*),F12(*),X(2)                                     
      DOUBLE PRECISION DGMLT1_ALT                                               
      EXTERNAL FSUB11                                                        
                                                                            
* --- function                                                              
      do L=1, M                                                             
         X(2) = U12(L)  !Z variable

         f12(L)= DGMLT1_ALT(FSUB11,pareff(5),pareff(6),4,6,X)                     

      enddo                                                                 
                                                                            
* --- end                                                                   
      return                                                                
      end                                                                   


***************************************************************** 
      subroutine FSUB11(M,U11,F11,X)                                           
***************************************************************** 
      DOUBLE PRECISION U11(*),F11(*),X(2)
      DOUBLE PRECISION PHOTDIST
      DOUBLE PRECISION pareff   
      common/radeff/pareff(10)  
      !$OMP THREADPRIVATE(/radeff/)
* --- function value                                                          
      DO L = 1, M                                                            
         x(1) = U11(L) !phi variable
         photdist = (pareff(1) - X(2))/
     +              (DCOS(pareff(2))*
     +                (DCOS(pareff(3))+DTAN(pareff(2))*DSIN(pareff(3))*
     +                                                 DCOS(X(1))
     +                )
     +              )


        F11(L) = pareff(7)*DEXP(-photdist/pareff(4)) 
      ENDDO                                                                   
                                                                              
* --- end                                                                     
      return                                                                  
      end                                          


* =================================================================
      subroutine rcpmteff(thc,npaths,pphimin,pphimax,effpmt)
* =================================================================

#include "../include/richrec_lipf.h"

      COMMON /RADEFF/ pareff(10)
      !$OMP THREADPRIVATE(/RADEFF/)
      integer nphmax  
      parameter(nphmax=NPHIRMAX)
      DOUBLE PRECISION pphimin(nphmax),pphimax(nphmax)
      DOUBLE PRECISION dgauss,flgeff,DEPS,dx
      DOUBLE PRECISION pareff
      REAL efflg,thc,dphitot
      INTEGER npaths,lphipat(nphmax)
      EXTERNAL FLGEFF
      DATA DEPS/1.D-3/
      SAVE DEPS
      !$OMP THREADPRIVATE(DEPS)
* ... init
      pareff(1) = -1.D0       
      pareff(2) = -1.D0       
      pareff(3) = DBLE(thc)
      pareff(4) = -1.D0       
      effpmt = 0.0
      if (npaths.eq.0) return
* ... integration on photon phi
      ipcount = 0
      DEFF    = 0.D0
      dphitot = 0.0
      do while (ipcount.lt.npaths)
         ipcount = ipcount + 1
         DEFF = DEFF+
     +        DGAUSS(flgeff,pphimin(ipcount),pphimax(ipcount),
     +                   DEPS)
         dphitot = dphitot + pphimax(ipcount)-pphimin(ipcount)
      enddo                 
      effpmt=real(DEFF)/dphitot
*      print*,'...... deff = ',deff
*      print*,'...... dphitot = ',dphitot
*      print*,'...... effpmt = ',effpmt
      return
      end

* ================================================================
      DOUBLE PRECISION FUNCTION flgeff(dphig)
* ================================================================

#include "../include/richrec_lipf.h"

* --------------------------------------------------------------------

      real lgeffd_proto(91),lgeffd_flight(91),lgeffx(91)

      data lgeffx /   0.0,  0.5,  1.5,  2.5,  3.5,
     +                4.5,  5.5,  6.5,  7.5,  8.5,
     +                9.5, 10.5, 11.5, 12.5, 13.5,
     +               14.5, 15.5, 16.5, 17.5, 18.5,
     +               19.5, 20.5, 21.5, 22.5, 23.5,
     +               24.5, 25.5, 26.5, 27.5, 28.5,
     +               29.5, 30.5, 31.5, 32.5, 33.5,
     +               34.5, 35.5, 36.5, 37.5, 38.5,
     +               39.5, 40.5, 41.5, 42.5, 43.5,
     +               44.5, 45.5, 46.5, 47.5, 48.5,
     +               49.5, 50.5, 51.5, 52.5, 53.5,
     +               54.5, 55.5, 56.5, 57.5, 58.5,
     +               59.5, 60.5, 61.5, 62.5, 63.5,
     +               64.5, 65.5, 66.5, 67.5, 68.5,
     +               69.5, 70.5, 71.5, 72.5, 73.5,
     +               74.5, 75.5, 76.5, 77.5, 78.5,
     +               79.5, 80.5, 81.5, 82.5, 83.5,
     +               84.5, 85.5, 86.5, 87.5, 88.5, 89.5/
      
      SAVE lgeffx
      !$OMP THREADPRIVATE(lgeffx) 

*     LIGHT GUIDE EFFICIENCY (PROTO)
*     Data from LG sim, 250 million evts, Jan-2008

      data lgeffd_proto /   0.6252, 0.6252, 0.6249, 0.6248, 0.6241,
     +                      0.6229, 0.6229, 0.6227, 0.6195, 0.6186,
     +                      0.6180, 0.6172, 0.6169, 0.6125, 0.6128,
     +                      0.6096, 0.6102, 0.6088, 0.6058, 0.6020,
     +                      0.5994, 0.5906, 0.5833, 0.5764, 0.5707,
     +                      0.5657, 0.5628, 0.5585, 0.5530, 0.5484,
     +                      0.5425, 0.5355, 0.5285, 0.5171, 0.5050,
     +                      0.4881, 0.4699, 0.4526, 0.4392, 0.4234,
     +                      0.4093, 0.3927, 0.3775, 0.3644, 0.3513,
     +                      0.3356, 0.3219, 0.3044, 0.2897, 0.2739,
     +                      0.2584, 0.2424, 0.2278, 0.2131, 0.2013,
     +                      0.1902, 0.1792, 0.1668, 0.1567, 0.1435,
     +                      0.1310, 0.1184, 0.1047, 0.0928, 0.0826,
     +                      0.0720, 0.0646, 0.0565, 0.0506, 0.0455,
     +                      0.0407, 0.0350, 0.0311, 0.0292, 0.0264,
     +                      0.0264, 0.0275, 0.0266, 0.0257, 0.0241,
     +                      0.0234, 0.0220, 0.0201, 0.0176, 0.0154,
     +                      0.0135, 0.0119, 0.0095, 0.0067, 0.0042,
     +                      0.0013/

      SAVE lgeffd_proto
      !$OMP THREADPRIVATE(lgeffd_proto)

*     LIGHT GUIDE EFFICIENCY (FLIGHT)
*     Data from LG sim, 250 million evts, Dec-2007

      data lgeffd_flight /  0.7561, 0.7561, 0.7572, 0.7587, 0.7557,
     +                      0.7566, 0.7571, 0.7589, 0.7558, 0.7560,
     +                      0.7538, 0.7530, 0.7485, 0.7436, 0.7385,
     +                      0.7319, 0.7293, 0.7229, 0.7165, 0.7098,
     +                      0.7066, 0.7013, 0.6921, 0.6830, 0.6715,
     +                      0.6596, 0.6433, 0.6216, 0.6023, 0.5840,
     +                      0.5670, 0.5521, 0.5383, 0.5218, 0.5077,
     +                      0.4915, 0.4755, 0.4602, 0.4487, 0.4329,
     +                      0.4186, 0.3992, 0.3798, 0.3636, 0.3476,
     +                      0.3301, 0.3146, 0.2985, 0.2837, 0.2699,
     +                      0.2566, 0.2464, 0.2383, 0.2235, 0.2111,
     +                      0.1972, 0.1835, 0.1681, 0.1544, 0.1409,
     +                      0.1267, 0.1152, 0.1033, 0.0909, 0.0828,
     +                      0.0729, 0.0659, 0.0586, 0.0531, 0.0478,
     +                      0.0458, 0.0463, 0.0447, 0.0440, 0.0431,
     +                      0.0417, 0.0393, 0.0373, 0.0350, 0.0321,
     +                      0.0292, 0.0270, 0.0246, 0.0224, 0.0190,
     +                      0.0168, 0.0142, 0.0114, 0.0082, 0.0055,
     +                      0.0015/

      SAVE lgeffd_flight
      !$OMP THREADPRIVATE(lgeffd_flight)
* --------------------------------------------------------------------

      integer nphmax,nermax  
      parameter(nphmax=NPHIRMAX)
      parameter(nermax=10)
        
      DOUBLE PRECISION dphig,V( 1)
      REAL pol3(4),pol1(2),pol5(6),pol9(10), LGeff
      REAL phig,vdet(3),vdir(3),thc,thei
      integer verr(nermax)
      DOUBLE PRECISION  pareff
      COMMON /RADEFF/ pareff(10) !common with working variables
      !$OMP THREADPRIVATE(/RADEFF/)
      character chfile*8

* ... propagate photon
      phig   = real(dphig)
      thc = real(pareff(3))
      call patrace(phig,thc,vdet,vdir,nmiref,nerr,verr)
      if (nerr.ne.0) then
         LGeff = 0.0
         goto 99
      endif
* ... compute angle
      if (abs(vdir(3)).gt.1.) then
         print*,'(flgeff)',nerr,vdir(3)
         LGeff = 0.0
         goto 99
      else
         thei = acos(vdir(3))*raddeg
      endif
* ... interpolate lg efficiency
*      print*,'rcgeom(1) = ',rcgeom(1)
      if(rcgeom(1).eq.9) then  ! prototype
         LGeff=DIVDIF_ALT(lgeffd_proto,lgeffx,91,thei,2)
      else if(rcgeom(1).eq.0) then  ! flight
         LGeff=DIVDIF_ALT(lgeffd_flight,lgeffx,91,thei,2)
      endif
**      call polint(lgeffd,lgeffx,79,thei,LGEFF)



 99   continue
      flgeff=dble(LGeff)

#ifdef LIP_DEBUG
      if(flgeff.gt.-1.E38.or.flgeff.lt.1.E38)then
      else
         print*,'flgeff = ',flgeff
         print*,'phig = ',phig,', thc = ',thc,', thei = ',thei,
     +     ', lgeff = ',lgeff
         
      endif
#endif

      RETURN
      END


* ================================================================
      subroutine rctoteff(pardou,npaths,pphimin,pphimax,lphipat,           
     +                    efftot)                                 
* ================================================================         
******************************************************************         
* AIMS: efficiency calculation taking into account:
*       - the geometrical acceptance               
*       - the radiator absorption and scattering effects                 
*       - the mirror reflectivity
*       - the LG/PMT effect               
*      
* -------------------------------------------------------------- 
*
* PARAMETERS:                                                              
*         IN: pardou:  parhrad - radiator height    [pardou(1)]
*                      parpthe - particle theta     [pardou(2)]
*                      parthec - cerenkov angle     [pardou(3)]   
*                      parlint - interaction length [pardou(4)]
*             npaths : number of path lengths to integrate (visible)
*             pphimin: limits of integration                      
*             pphimax: """                                        
*             lphipat: phi range flag 
*                      0=non-reflected
*                      1=1 reflection
*                      2=2 reflections (max at this moment)
*        OUT:                                                              
*             efftot(1) - overall efficiency     
*             efftot(2) - direct pattern eff
*             efftot(3) - 1-reflection eff 
*             efftot(4) - 2-reflections eff
******************************************************************         

#include "../include/richrec_lipf.h"

      integer nphmax  
      parameter(nphmax=NPHIRMAX)
      DOUBLE PRECISION pphimin(nphmax),                                        
     +                 pphimax(nphmax),                                        
     +                 pardou(4),                                          
     +                 parhrad                                             
      INTEGER npaths,lphipat(nphmax)                                           
      REAL efftot(4)                                                          
      DOUBLE PRECISION X(2),DGMLT2_ALT                                         
      DOUBLE PRECISION FACTOR,PAREFF,Q2NEW,Q2TOT                           
      COMMON/RADEFF/PAREFF(10)                                             
      !$OMP THREADPRIVATE(/RADEFF/)
      EXTERNAL FSUB22                                                       
* ... init                                                                 
      DO i=1,4                                                             
         PAREFF(i) = PARDOU(i)      
         EFFTOT(I) = 0.0
      ENDDO                                                                
      ipcount = 0                                                          
      Q2TOT = 0.D0                                             
      Q2NEW = 0.D0                                             
* ... integration on Z                                                     
      FACTOR = 1./dble(twopi)/pareff(1)                                    
      do while (ipcount.lt.npaths)  !sum on visible paths
         ipcount   = ipcount + 1                                             
         parhrad   = pareff(1)
         pareff(5) = pphimin(ipcount)                                      
         pareff(6) = pphimax(ipcount)

         if (lphipat(ipcount).eq.2) then ! double reflection                                   
            pareff(7) = dble(reflec**2)

            Q2NEW = pareff(7)*DGMLT2_ALT(FSUB22,0.D0,parhrad,3,6,X)              

            EFFTOT(4) = EFFTOT(4) + REAL(Q2NEW)                                              
         elseif (lphipat(ipcount).eq.1) then   ! single reflection 
            pareff(7) = dble(reflec)

            Q2NEW = pareff(7)*DGMLT2_ALT(FSUB22,0.D0,parhrad,3,6,X)              

            EFFTOT(3) = EFFTOT(3) + REAL(Q2NEW)                                                    
         elseif (lphipat(ipcount).eq.0) then   ! direct incidence                                  
            pareff(7) = dble(1.0)

            Q2NEW = pareff(7)*DGMLT2_ALT(FSUB22,0.D0,parhrad,3,6,X)              

            EFFTOT(2) = EFFTOT(2) + REAL(Q2NEW)                                                    
         endif                                                             
         Q2TOT = Q2TOT + Q2NEW

#ifdef LIP_DEBUG   
      
         print*,'pareff =',pareff
         print*,'ipcount = ',ipcount
         print*,'pphimin/pphimax/lphipat = ', pphimin(ipcount),
     +                                        pphimax(ipcount),
     +                                        lphipat(ipcount)    
         print*,'Q2NEW = ', Q2NEW

         if(Q2NEW.gt.-1.E38.or.Q2NEW.lt.1.E38)then
            
         else
            print*,'Q2NEW - II = ', Q2NEW
         endif
#endif

      enddo                                                                

* ... total efficiency
      Q2TOT = FACTOR*Q2TOT                                                 
* ... output (real)                                                        
      EFFTOT(1) = REAL(Q2TOT)              
      EFFTOT(2) = FACTOR*EFFTOT(2)                                   
      EFFTOT(3) = FACTOR*EFFTOT(3)                                   
      EFFTOT(4) = FACTOR*EFFTOT(4)                                   

#ifdef LIP_DEBUG   
      print*,'ievnumb/FACTOR/EFFTOT = ', ievnumb,
     +        FACTOR,(EFFTOT(i),i=1,4)
#endif

* ... end                                                                  
      return                                                               
      end                                                                  

*****************************************************************           
      subroutine FSUB22(M,U22,F22,X)                                           
*****************************************************************           
      DOUBLE PRECISION PAREFF                                               
      COMMON/RADEFF/PAREFF(10)                                              
      !$OMP THREADPRIVATE(/RADEFF/)
      DOUBLE PRECISION U22(*),F22(*),X(2)                                     
      DOUBLE PRECISION DGMLT1_ALT                                               
      EXTERNAL FSUB21                                                        
                                                                            
* --- function                                                              
      do L=1, M                                                             
         X(2) = U22(L)  !Z variable                                          
         f22(L)= DGMLT1_ALT(FSUB21,pareff(5),pareff(6),4,6,X)                     

#ifdef LIP_DEBUG   
         if(f22(L).gt.-1.E38.or.f22(L).lt.1.E38)then
         else
            print*,'FSUB22: L,f22(L) = ', L,f22(L)
         endif
#endif
      enddo                                                                 
                                                                            
* --- end                                                                   
      return                                                                
      end                                                                   


***************************************************************** 
      subroutine FSUB21(M,U21,F21,X)                                      
***************************************************************** 
      DOUBLE PRECISION U21(*),F21(*),X(2)
      DOUBLE PRECISION PHOTDIST
      DOUBLE PRECISION pareff
      DOUBLE PRECISION flgeff   
      DOUBLE PRECISION AA
      EXTERNAL FLGEFF
      common/radeff/pareff(10)  
      !$OMP THREADPRIVATE(/radeff/)

* --- function value                                                          
      DO L = 1, M                                                            
         x(1) = U21(L) !phi variable
         photdist = (pareff(1) - X(2))/
     +              (DCOS(pareff(2))*
     +                (DCOS(pareff(3))+DTAN(pareff(2))*DSIN(pareff(3))*
     +                                                 DCOS(X(1))
     +                )
     +              )

       

         F21(L) = DEXP(-photdist/pareff(4))*flgeff(x(1))


#ifdef LIP_DEBUG
         if(F21(L).gt.-1.E38.or.F21(L).lt.1.E38)then
         else
            print*,'FSUB21: L,F21(L) = ', L,F21(L)
            print*,'photdist = ',photdist
            print*,'pareff(1) = ',pareff(1)
            print*,'pareff(2) = ',pareff(2)
            print*,'pareff(3) = ',pareff(3)
            print*,'pareff(4) = ',pareff(4)
            print*,'x(1) = ', x(1)
            print*,'x(2) = ', x(2)
cc            print*,'AA = ', AA
            print*,'flgeff(x(1)) = ',flgeff(x(1)) 
         endif
#endif

      ENDDO                                                                   
                                                                              
* --- end                                                                     
      return                                                                  
      end                                          





* ---------------------------------------------------------------
* ----------------------------00000000---------------------------
* ---------------------------------------------------------------

* =================================================================
      real function zvtxmean(thetac)
* =================================================================

#include "../include/richrec_lipf.h"

      integer nphmax,nermax  
      parameter(nphmax=NPHIRMAX)
      parameter(nermax=10)
      integer nphitot,
     +        fphitot(nphmax),
     +        fphipat(nphmax),
     +        msecpat(nphmax),
     +        verr(nermax),
     +        npvis
      real 
     +     vphitot(nphmax),
     +     dphidir(nphmax),
     +     dphihol(nphmax),
     +     dphimir0(nphmax),
     +     dphimir1(nphmax),
     +     dphiinv(nphmax),
     +     phimin(nphmax),
     +     phimax(nphmax),
     +     thetac,zoutmean

      integer nphimsec0(nphmax),
     +        nphimsec1(nphmax)
      real dphimsec0(nmaxmirsec,nphmax),
     +     dphimsec1(nmaxmirsec,nphmax)
      real dphimsec0_tot(nmaxmirsec),
     +     dphimsec1_tot(nmaxmirsec)

      DOUBLE PRECISION  pardou(4),
     +                  fidmin(nphmax),
     +                  fidmax(nphmax)

* --- init
      zvtxmean = -999.
*      print*,'thetac',thetac
* --- compute photon phi root values
      call set_phiroots(thetac,nphitot,vphitot,fphitot) 
*      print*,'nphitot',nphitot
*      print*,'vphitot,fphitot',(vphitot(i),fphitot(i),i=1,nphitot)
* --- compute path lengths
      call dphipath(thetac,nphitot,vphitot,fphitot,
     +              dphidir,dphihol,dphimir0,dphimir1,dphiinv,
     +              npvis,phimin,phimax,fphipat,msecpat,
     +              dphimsec0,dphimsec1)
*      print*,'dphidir,dphihol,dphimir0,dphimir1,dphiinv,
*     +     npvis,phimin,phimax,fphipat',dphidir,dphihol,dphimir0,
*     &     dphimir1,dphiinv,npvis,phimin,phimax,fphipat
      if (npvis.eq.0) return

* --- compute mean Z of emitted photons
      pardou(1) = dble(HRAD)
CC      if (chradid.eq.'NAF') pardou(1) = dble(0.5)
      pardou(2) = dble(pthe)
      pardou(3) = dble(thetac)
      pardou(4) = dble(radint) 
*
*      print*,'HRAD',pardou(1),'pthe',pardou(2)*raddeg,'thetac',pardou(3)
*     &     *raddeg,'radint',pardou(4)

      do i=1,10
         fidmin(i) = dble(phimin(i))
         fidmax(i) = dble(phimax(i))
      enddo
*
      call zradmean(pardou,npvis,fidmin,fidmax,fphipat,zoutmean)

* --- output: z fraction
      zvtxmean = zoutmean/real(pardou(1))
*      print*,'zvtxmean',zvtxmean
      return
      end

* =================================================================
      subroutine zradmean(pardou,npaths,pphimin,pphimax,lphipat,           
     +                    zoutmean)                                  
* =================================================================        
******************************************************************         
* AIMS: It computes the mean Z of emitted photons
                                                                           
* PARAMETERS:                                                              
*         IN: pardou: parhrad - radiator height                            
*                     parpthe - particle theta                             
*                     parthec - cerenkov angle                             
*                     parlint - interaction length                         
*                     npaths  - number of path lengths to integrate        
*                     pphimin - limits of integration                      
*                     pphimax - """                                        
*                     lphipat - phi range flag (0=non-reflected/1=reflected
*        OUT:                                                              
*                     zoutmean                     
******************************************************************         

#include "../include/richrec_lipf.h"

      integer nphmax  
      parameter(nphmax=NPHIRMAX)
      DOUBLE PRECISION pphimin(nphmax),                                        
     +                 pphimax(nphmax),                                        
     +                 pardou(4),                                          
     +                 parhrad                                             
      INTEGER npaths,lphipat(nphmax)                                           
      REAL zoutmean
      DOUBLE PRECISION X(2),DGMLT2_ALT                                         
      DOUBLE PRECISION FACTOR,PAREFF,Q2NEW,Q2TOT1,Q2TOT2
      COMMON/RADEFF/PAREFF(10)                                             
      !$OMP THREADPRIVATE(/RADEFF/)
      EXTERNAL FSUB12,FSUB3
                                                                           
* ... init                                                                 
      do i=1,4                                                             
         pareff(i) = pardou(i)                                             
      enddo                                                                
* ... integration on Z (numerator)                                         
      ipcount = 0                                                          
      Q2TOT1 = 0.D0                                                         
      do while (ipcount.lt.npaths)                                         
         ipcount = ipcount + 1                                             
*                                                                          
         if (lphipat(ipcount).eq.0) then                                   
            pareff(7) = 1.0                                         
         elseif (lphipat(ipcount).eq.1) then   
            pareff(7) = reflec  
         elseif (lphipat(ipcount).eq.2) then   
            pareff(7) = reflec**2
         endif                                                             
         pareff(5) = pphimin(ipcount)                                      
         pareff(6) = pphimax(ipcount)                                      
*                                                                          
         parhrad = pareff(1)

         Q2NEW = DGMLT2_ALT(FSUB3,0.D0,parhrad,3,6,X)                          

         Q2TOT1 = Q2TOT1 + Q2NEW                                             
      enddo                                                                

* ... integration on Z (denominator)                                         
      ipcount = 0                                                          
      Q2TOT2 = 0.D0                                                         
      do while (ipcount.lt.npaths)                                         
         ipcount = ipcount + 1                                             
*                                                                          
         if (lphipat(ipcount).eq.0) then                                   
            pareff(7) = 1.0                                         
         elseif (lphipat(ipcount).eq.1) then   
            pareff(7) = reflec  
         elseif (lphipat(ipcount).eq.2) then   
            pareff(7) = reflec**2
         endif                                                             
         pareff(5) = pphimin(ipcount)                                      
         pareff(6) = pphimax(ipcount)                                      
*                                                                          
         parhrad = pareff(1)

         Q2NEW = DGMLT2_ALT(FSUB12,0.D0,parhrad,3,6,X)                          

         Q2TOT2 = Q2TOT2 + Q2NEW                                           
      enddo                                                                
                                                                           
* ... output (real)                                                        
      zoutmean = real(Q2TOT1/Q2TOT2) 
                                                                           
* ... end                                                                  
      return                                                               
      end                                                                  

*****************************************************************           
      subroutine FSUB3(M,U12,F12,X)                                           
*****************************************************************           
      DOUBLE PRECISION PAREFF                                               
      COMMON/RADEFF/PAREFF(10)                                              
      !$OMP THREADPRIVATE(/RADEFF/)
      DOUBLE PRECISION U12(*),F12(*),X(2)                                     
      DOUBLE PRECISION DGMLT1_ALT                                               
      EXTERNAL FSUB11                                                        
                                                                            
* --- function                                                              
      do L=1, M                                                             
         X(2) = U12(L)  !Z variable                                          
         f12(L)= X(2)*DGMLT1_ALT(FSUB11,pareff(5),pareff(6),4,6,X)
      enddo                                                                 
                                                                            
* --- end                                                                   
      return                                                                
      end                                                                   

*****************************************************************           
*****************************************************************           

* =================================================================
      subroutine rtheimean(thc,npaths,pphimin,pphimax,theim)
* =================================================================

#include "../include/richrec_lipf.h"

      COMMON /RADEFF/ pareff(10)
      !$OMP THREADPRIVATE(/RADEFF/)
      integer nphmax  
      parameter(nphmax=NPHIRMAX)
      DOUBLE PRECISION pphimin(nphmax),pphimax(nphmax)
      DOUBLE PRECISION dgauss,flgeff,DEPS,dx
      DOUBLE PRECISION pareff
      REAL efflg,thc,dphitot
      INTEGER npaths,lphipat(nphmax)
      EXTERNAL fthei
      DATA DEPS/1.D-3/
      SAVE DEPS
      !$OMP THREADPRIVATE(DEPS)

* ... init
      pareff(3)=dble(thc)
      theim = 0.0
      if (npaths.eq.0) return
* ... integration on photon phi
      ipcount = 0
      Dtheim  = 0.D0
      dphitot = 0.0
      do while (ipcount.lt.npaths)
         ipcount = ipcount + 1
/*  DGAUSS not working in Parallel!!!! */
         Dtheim = Dtheim+
     +        DGAUSS(fthei,pphimin(ipcount),pphimax(ipcount),
     +                   DEPS)
         dphitot = dphitot + pphimax(ipcount)-pphimin(ipcount)
      enddo                 
      theim=real(Dtheim)/dphitot
      return
      end
* ================================================================
      DOUBLE PRECISION FUNCTION fthei(dphig)
* ================================================================

#include "../include/richrec_lipf.h"

      integer nphmax,nermax  
      parameter(nphmax=NPHIRMAX)
      parameter(nermax=10)
      DOUBLE PRECISION dphig
      REAL phig,vdet(3),vdir(3),thc,thei
      integer verr(nermax)
      DOUBLE PRECISION pareff
      COMMON /RADEFF/ pareff(10)
      !$OMP THREADPRIVATE(/RADEFF/)
      phig   = real(dphig)
      thc = dble(pareff(3))
      call patrace(phig,thc,vdet,vdir,nmiref,nerr,verr)
* ... compute angle
      thei = acos(vdir(3))*raddeg
      
      fthei=dble(thei)
      RETURN
      END


***************************
*** CODE FROM patphot.F ***
***************************

* =======================================================================
      subroutine patphot(phig,thc,coneg)
* =======================================================================
***********************************************************+
*
*     AIMS       : Get photon direction in RICH-frame generated for 
*                  a given phi angle
*
*     PARAMETERS :
*                   IN - PHIG  = photon PHI angle
*                        THC   = cerenkov angle
*                  OUT - CONEG = photon direction 
*                        (g versor of the geratrix cone)
*
***********************************************************+

#include "../include/richrec_lipf.h"

      real phig, thc, pconeg(3),coneg(3)           

* --- geratriz
      pconeg(1) =  sin(thc)*cos(phig) 
      pconeg(2) =  sin(thc)*sin(phig) 
      pconeg(3) =  cos(thc)

* --- change frame
      call vfill(coneg,3,0.)
      do i=1,3
         do j=1,3
            coneg(i) = coneg(i)+figrotm(i,j)*pconeg(j)  
         enddo
      enddo

      return
      end


***************************
*** CODE FROM patints.F ***
***************************

* =======================================================================
      subroutine patints(p0,v0,CHOPT,ierr)
* =======================================================================
******************************************************************
*     AIMS:
*     Intersection of a straight line (photon) with a surface Z=cte
*     
*     PARAMETERS:
*     IN  - P0(3) = straight line point
*           V0(3) = straight line direction
*           CHOPT = defines surface 
*                   'RADB' = radiator botto
*                   'TMIR' = top mirror plane
*                   'BMIR' = bottom mirror plane
*                   'MIRR' = mirror (with line coming from PMTD, point 
*                                    outside mirror)
*                   'MIRA' = mirror (with line coming from TMIR)
*                   'MIRI' = mirror (with line coming from PMTD, point
*                                    inside mirror)
*                   'PMTD' = pmt plane
*     OUT - PINT(3) = intersection point
*           VI(3)   = normal to the plane at the intersection point 
*                   (pointing inwards) 
*
* Revised: 27-05-2004 (F.Barao) RADB option includes radiator inner walls
*                               depending on LEVGRAD level flag
*
*          09-02-2007 (FB,RP)   mirror intersection made possible with
*                               straight lines having reference points
*                               below mirror (MIRR) and above (MIRA)           
******************************************************************

#include "../include/richrec_lipf.h"

      real p0(3), v0(3), pint(3), vi(3), pc(3)
      real srtfroot
      character*4 chopt 
      real arat,brat,dx,dy,dz
      integer ierr
      real xxx,yyy
      integer nxv,nyv, nxi,nyi

* --- init
      ierr = 0

* ------------------------------------
      IF (CHOPT.EQ.'PMTD') THEN       
* ------------------------------------

* --- intersection with detector plane
      PINT(3) = ZPMTDET
      PINT(1) = P0(1) + V0(1)/V0(3)*(ZPMTDET-P0(3))
      PINT(2) = P0(2) + V0(2)/V0(3)*(ZPMTDET-P0(3))


* -------------------------------------  
      ELSEIF (CHOPT(1:3).EQ.'MIR') THEN       
* -------------------------------------

* --- photon direction
      arat = v0(1)/v0(3)
      brat = v0(2)/v0(3)
      dx   = p0(1)-vtmir(1)
      dy   = p0(2)-vtmir(2) 
      dz   = p0(3)-vtmir(3) 

* --- intersection point
      A      = arat**2 + brat**2 - TGMIR**2
      B      = 2.*( arat*(p0(1)-VTMIR(1)) +
     &              brat*(P0(2)-VTMIR(2)) -
     &              TGMIR**2*(P0(3)-VTMIR(3))
     &            )          
      C      = (P0(1)-VTMIR(1))**2 + 
     &         (P0(2)-VTMIR(2))**2 - 
     &         TGMIR**2*(P0(3)-VTMIR(3))**2

      
*      factroot = 4*(2.*arat*brat*dx*dy - 
*     +           2.*tgmir**2*dz*(arat*dx+brat*dy) +
*     +           arat**2*(dy**2-tgmir**2*dz**2) +
*     +           brat**2*(dx**2-tgmir**2*dz**2) -
*     +           tgmir**2*(dx**2+dy**2))
      factroot=4*(TGMIR**2*(dx**2+dy**2-2*dx*dz*arat+dz**2*arat**2-
     +         2*dy*dz*brat+dz**2*brat**2)
     +         -dy**2*arat**2+2*dx*dy*arat*brat-dx**2*brat**2)


*      print*,'argg ',phig,b**2-4*a*c,factroot,acos(v0(3))*RADDEG

      if (factroot.lt.0. .and. abs(factroot).gt.1.E-3) then
         ierr = 1
         return
      endif
      if (factroot.lt.0. .and. abs(factroot).lt.1.E-3) then  
*         print*,'(patints): warning factroot negative and gt 1E-3'
         factroot = 0.0
      endif

      srtfroot = sqrt(factroot)      
      if(abs(A).lt.1.E-5)then
         root_plus = -C/B
         root_minus = -C/B
      else
         root_plus  = 0.5/A*(-B+srtfroot)
         root_minus = 0.5/A*(-B-srtfroot)
      endif

* ... closest intersection
      /* intersection with line starting at PMTD, point outside mirror */
      if (CHOPT(4:4).eq.'R') then
         if (root_minus.lt.0. .and. root_plus.lt.0.) then
            root1 = -min(abs(root_minus),abs(root_plus)) !get shortest negative distance
         else
            ierr = 2
            return
         endif
      /* intersection with line starting at TMIR */
      elseif (CHOPT(4:4).eq.'A') then 
         root1 = max(root_minus,root_plus) !get positive distance
         if (root1.lt.0.) then
            ierr = 2 
            return
         endif
      /* intersection with line starting at PMTD, point inside mirror */
      elseif (CHOPT(4:4).eq.'I') then 
         root1 = root_minus 
         if (root_minus.lt.0.) then !get negative root (shortest, in case many)
            if (root_plus.lt.0.) then
               root1 = min(abs(root_minus),abs(root_plus))
            else
               root1 = root_minus
            endif
         else
C            print*,'(patints):MIRI Problem - no negative root!!!'       
            ierr = 2 
            return
         endif
      endif
* ... intersection point coordinates
      pint(3) = root1 + p0(3)
      pint(1) = p0(1) + arat*(pint(3)-p0(3))
      pint(2) = p0(2) + brat*(pint(3)-p0(3))

* ... errors

* --> check Z coord (on rich frame)
      if (CHOPT(4:4).eq.'R') then
         if (pint(3).lt.(HRAD+HPGL+ZTMIRGAP)) then
            ierr = 3
            return
         endif
      elseif (CHOPT(4:4).eq.'A') then 
         if (pint(3).gt.(HRAD+HPGL+ZTMIRGAP+HMIR)) then
            ierr = 3
            return
         endif
      elseif (CHOPT(4:4).eq.'I') then 
         if (pint(3).lt.(HRAD+HPGL+ZTMIRGAP) .or.
     +       pint(3).gt.(HRAD+HPGL+ZTMIRGAP+HMIR)) then
            ierr = 3
            return
         endif
      endif

* ------------------------------------  
      ELSEIF (CHOPT.EQ.'RADB') THEN     
* ------------------------------------  

* --- radiator boundaries intersection

* ==> BOTTOM plane intersection                  
      pint(3) = HRAD                   
      dist = (pint(3) - p0(3))/v0(3)      
      pint(1) = p0(1) + dist*v0(1)        
      pint(2) = p0(2) + dist*v0(2)        
      radius = sqrt(pint(1)**2+pint(2)**2)

* ==> OUTER BOUNDARY check  
      if (rcgeom(1) .ne.9) then !flight configuration
         if (radius.gt.RTMIR) then         
            ierr = 1                       
            return                         
         endif
      else !prototype configuration
         if ( (abs(pint(1)-XCRAD).gt.XDRAD/2.) .or.
     +        (abs(pint(2)-YCRAD).gt.YDRAD/2.) ) then
            ierr = 1
            return
         endif                                       
      endif               

* ==> INNER WALLS check (idea: check tiles id from emission to intersection)
      if (rcgeom(1) .ne.9) then !flight configuration
         if (LEVGRAD.eq.1) then
            xxx = pcervtx(1)
            yyy = pcervtx(2)
            call uradtile(xxx,yyy,nxv,nyv) !tile indices
            xxx = pint(1)
            yyy = pint(2)
            call uradtile(xxx,yyy,nxi,nyi) !tile indices
            if (nxi.ne.nxv .or.
     +          nyi.ne.nyv) ierr = 2 ! wall crossed
         endif
      endif               

* ------------------------------------  
      ELSEIF (CHOPT.EQ.'PGLB') THEN     
* ------------------------------------  

* --- refraction point                  
      pint(3) = HRAD+HPGL                   
      dist = (pint(3) - p0(3))/v0(3)      
      pint(1) = p0(1) + dist*v0(1)        
      pint(2) = p0(2) + dist*v0(2)        
      radius = sqrt(pint(1)**2+pint(2)**2)  
      if (rcgeom(1).ne.9) then !flight configuration
         if (radius.gt.RTMIR) then         
           ierr = 1                       
           return                         
         endif
      else !prototype configuration
         if ( (abs(pint(1)-XCRAD).gt.XDRAD/2.) .or.
     +        (abs(pint(2)-YCRAD).gt.YDRAD/2.) ) then
            ierr = 1
            return
         endif                                       
      endif                             

* ------------------------------------  
      ELSEIF (CHOPT.EQ.'TMIR') THEN     
* ------------------------------------  

* --- intersection point                  
      pint(3)  = HRAD+HPGL+ZTMIRGAP                   
      dist   = (pint(3) - p0(3))/v0(3)      
      pint(1)  = p0(1) + dist*v0(1)        
      pint(2)  = p0(2) + dist*v0(2)        
      radius = sqrt((pint(1)-vtmir(1))**2+(pint(2)-vtmir(2))**2)  
*      if (rcgeom(1).ne.9) then !flight configuration
         if (radius.gt.RTMIR) then         
           ierr = 1                       
           return                         
         endif
*      endif                             

* ------------------------------------  
      ELSEIF (CHOPT.EQ.'BMIR') THEN     
* ------------------------------------  
      
* --- intersection point                  
      pint(3)  = HRAD+HPGL+ZTMIRGAP+HMIR                   
      dist   = (pint(3) - p0(3))/v0(3)      
      pint(1)  = p0(1) + dist*v0(1)        
      pint(2)  = p0(2) + dist*v0(2)        
      radius = sqrt((pint(1)-vtmir(1))**2+(pint(2)-vtmir(2))**2)  
*      if (rcgeom(1).ne.9) then !flight configuration
         if (radius.gt.RBMIR) then         
           ierr = 1                       
           return                         
         endif
*      endif                             


*------------------------------------
/*      elseif (CHOPT.EQ.'WALL')THEN
*------------------------------------
* --- intersection with detector plane
      PINT(3) = ZPMTDET
      PINT(1) = P0(1) + V0(1)/V0(3)*(ZPMTDET-P0(3))
      PINT(2) = P0(2) + V0(2)/V0(3)*(ZPMTDET-P0(3))

         IF((pint(1)-XCPMM).GT.18.6.AND.
     +        (pint(2)-YCPMM).GT.-3.1.OR.
     +        (pint(2)-YCPMM).LT.15.5)THEN
            IERR=1 
*            RETURN
         ELSEIF((pint(1)-XCPMM).GT.15.5.AND.
     +           (pint(2)-YCPMM).LT.-3.1.OR.
     +           (pint(2)-YCPMM).GT.-12.4)THEN
            IERR=1 
*            RETURN
         ELSEIF((pint(1)-XCPMM).GT.12.4.AND.
     +           (pint(2)-YCPMM).LT.-12.4.OR.
     +           (pint(2)-YCPMM).GT.-15.5)THEN
            IERR=1 
*            RETURN
         ELSEIF((pint(1)-XCPMM).LT.-15.5.AND.
     +           (pint(2)-YCPMM).LT.15.5.OR.
     +           (pint(2)-YCPMM).GT.-12.4)THEN
            IERR=1 
*            RETURN
         ELSEIF((pint(1)-XCPMM).LT.-12.4.AND.
     +           (pint(2)-YCPMM).GT.-15.5.OR.
     +           (pint(2)-YCPMM).LT.-12.4)THEN
            IERR=1 
*            RETURN
         ELSEIF(pint((2)-YCPMM).GT.15.5.AND.
     +           (pint(1)-XCPMM).GT.-15.5.OR.
     +           (pint(1)-XCPMM).LT.18.6)THEN
            IERR=1 
*            RETURN
         ELSEIF((pint(2)-YCPMM).LT.-3.1.AND.
     +           (pint(1)-XCPMM).GT.15.5.OR.
     +           (pint(1)-XCPMM).LT.18.6)THEN
            IERR=1 
*            RETURN
         ELSEIF((pint(2)-YCPMM).LT.-12.4.AND.
     +           ABS(pint(1)-XCPMM).LT.15.5.OR.
     +           (pint(1)-XCPMM).GT.12.4)THEN
            IERR=1 
*            RETURN
         ELSEIF((pint(2)-YCPMM).LT.-15.5.AND.
     +           ABS(pint(1)-XCPMM).LT.12.4)THEN
            IERR=1 
*            RETURN
         ENDIF */
* ------------------------------------
      ENDIF 
* ------------------------------------

* --- output intersection point
      call ucopy(pint,p0,3)


* --- end
      return
      end


***************************
*** CODE FROM patrefr.F ***
***************************

* ===================================================================
      subroutine patrefr(CHIN,CHOUT,phdir,ierr)
* ===================================================================
***************************************************************
*
*     It refracts the photon at the exit of the radiator
*     PARAMETERS:
*     IN     - ierr  = 1 photon not tranmitted due to critical angle
*     IN/OUT - phdir = photon direection

***************************************************************

#include "../include/richrec_lipf.h"

      real phdir(3),phdirn(3)
      real vnormal(3)
      character*3 CHIN,CHOUT

* --- init
      ierr = 0
* --- normal to the radiator plane on the general frame (radiator inward)
      vnormal(1) = 0.
      vnormal(2) = 0.
      vnormal(3) = -1.
* --- incident angle
      costhi = - vdot(phdir,vnormal,3)
      acthi = abs(costhi)
      if (acthi.gt.1.) costhi = costhi/acthi 
      thi    = acos(costhi)
* --- transmission angle
      if (CHIN.eq.'AGL'.or.CHIN.eq.'NAF') refindex_1 = refindex
      if (CHIN.eq.'PGL')                  refindex_1 = pglix 
      if (CHOUT.eq.'AIR')                 refindex_2 = 1.0
      if (CHOUT.eq.'PGL')                 refindex_2 = pglix
*
      sthi = sin(thi)
      sintht = refindex_1/refindex_2*sthi
      if (sintht.ge.1.) then !critical angle    
         ierr = 1                               
         return                                 
      endif                                     
      tht = asin(sintht)                        
* --- new photon direction
      if (thi.ne.0.) then
         A = sintht/sthi
         B = sin(tht-thi)/sthi
         phdirn(1) = A*phdir(1)+B*vnormal(1)
         phdirn(2) = A*phdir(2)+B*vnormal(2)
         phdirn(3) = A*phdir(3)+B*vnormal(3)
      else
         call ucopy(phdir,phdirn,3)
      endif
* --- normalization of new direction
      phdirnm = sqrt(phdirn(1)**2+phdirn(2)**2+phdirn(3)**2)
      do 1 i=1,3
 1       phdirn(i) = phdirn(i)/phdirnm
* --- output
      call ucopy(phdirn,phdir,3)

* --- end
      return
      end


***************************
*** CODE FROM patrefl.F ***
***************************

* =======================================================================
      subroutine patrefl(phpto,phdir,ierr)
* =======================================================================
*****************************************************************
*
*     It reflects the photon at the mirror 
*     
*      Parameters:
*      IN  - phdir(3) - photon direction
*            phpto(3) = intersection point on mirror
*     OUT  - phdir
*****************************************************************

#include "../include/richrec_lipf.h"

      real phdir(3),vnorm(3),phdir_ref(3),phpto(3)

* --- init
      ierr = 0

* --- normalize incident angle
      phdirm = sqrt(phdir(1)**2+phdir(2)**2+phdir(3)**2)
      do 1 i=1,3      
 1       phdir(i) = phdir(i)/phdirm

* --- normal vector at the intersection point
      vnorm(1) = -2.*(phpto(1)-vtmir(1))
      vnorm(2) = -2.*(phpto(2)-vtmir(2))
      vnorm(3) =  2.*TGMIR**2*(phpto(3)-vtmir(3))
      vnormm = sqrt(vnorm(1)**2+vnorm(2)**2+vnorm(3)**2)
      do 2 i=1,3
 2       vnorm(i)=vnorm(i)/vnormm
C      print*,'vnorm=',vnorm

* --- incident angle
      costhi = -vdot(phdir,vnorm,3)
C      print*,'thi=',acos(costhi)

* --- reflected direction of the photon
      phdir_ref(1) = phdir(1) + 2*costhi*vnorm(1)
      phdir_ref(2) = phdir(2) + 2*costhi*vnorm(2)
      phdir_ref(3) = phdir(3) + 2*costhi*vnorm(3)
      phdir_refm = sqrt(phdir_ref(1)**2+phdir_ref(2)**2+phdir_ref(3)**2)
      do 3 i=1,3        
 3       phdir_ref(i) = phdir_ref(i)/phdir_refm

* --- check if photon is coming from in or out of conical mirror?
      if (costhi.le.0.) then
         ierr = 1
         return
      endif

* --- output
      call ucopy(phdir_ref,phdir,3)

* --- end

      return
      end


**********************************
*** CODE FROM richaccradwall.F ***
**********************************

      subroutine richaccradwall(thetac,accradwalldir,accradwallref,
     +                          accradwallmsec)
**********************************************************************
* Returns the fraction of detected photons (visible) crossing the radiator poron walls
*
*     OUT: 
*        thetac        :  Cherenkov angle
*        accradwalldir :  acceptance due to radiator poron walls (direct)
*        accradwallref :  acceptance due to radiator poron walls (reflected)
*        accradwallmsec:  acceptance by mirror sector
*
*     26-Nov-2008: changed by R. Pereira to take into account
*     different scattering probabilities for each photon,
*     and to have thetac as argument to comply with multi-ring
*     acceptance calculation
*
*     28-Nov-2008: reflection coefficient (0.85) incorporated into
*     (R.Pereira)  probability, although it is currently irrelevant
*                  because results for direct and reflected branches
*                  are given separately
*
*     12-May-2009: - Hard-coded value 0.85 replaced by variable REFLEC,
*     (R.Pereira)    richcrd.inc added to get this variable.
*                  - Support for mirror sectors added
*
*********************************************************************

#include "../include/richrec_lipf.h"

      real thetac
      real accradwalldir,accradwallref
      real accradwallmsec(nmaxmirsec)

      real accradwall, ztrial, gamvtx(3), phdir(3), pint(3)

      integer nfotall
      integer nfotalldir, nfotallref
      integer nfotwalldir, nfotwallref

      integer nfotallmsec(nmaxmirsec),
     +        nfotwallmsec(nmaxmirsec)

      real spfotall
      real spfotalldir, spfotallref
      real spfotwalldir, spfotwallref

      real spfotallmsec(nmaxmirsec),
     +     spfotwallmsec(nmaxmirsec)

      integer nphmax
      parameter(nphmax=10)
      integer nerr,verr(nphmax)

      real pcervtx_backup(3)

      real txv,tyv
      real txi,tyi
      integer itflagv,itflagi

      real phimir
      integer imsec

      real photprob
      real photprobmsec

* ... init
      accradwalldir = 0.
      accradwallref = 0.

      nfotall     = 0
      nfotalldir  = 0
      nfotallref  = 0
      nfotwalldir = 0
      nfotwallref = 0

      spfotall     = 0.
      spfotalldir  = 0.
      spfotallref  = 0.
      spfotwalldir = 0.
      spfotwallref = 0.

      do i=1,nmaxmirsec
         accradwallmsec(i) = 0.
         nfotallmsec(i) = 0
         nfotwallmsec(i) = 0
         spfotallmsec(i) = 0.
         spfotwallmsec(i) = 0.
      enddo

* ... safe cut
      if (ipthetac.eq.0) return !no beta reconstructed
      if (chradid.eq.'NAF') return !naf

* ... backup pcervtx
      call ucopy(pcervtx,pcervtx_backup,3)

*      print*,'thetac in richaccradwall = ',thetac

* ... loop on radiator depth
*      do ZFRAC=0,1.,0.1 ! radiator fraction
      do ZFRAC=0.05,1.,0.1 ! radiator fraction
         ZTRIAL = ZTARG+HRAD*ZFRAC
* ...... set photon vertex                                         
         gamvtx(3) = ZTRIAL                                         
         gamvtx(2) = pimp(2) + tan(pthe)*sin(pphi)*(ZTRIAL-pimp(3)) 
         gamvtx(1) = pimp(1) + tan(pthe)*cos(pphi)*(ZTRIAL-pimp(3))
         call uradtile(gamvtx(1),gamvtx(2),nxv,nyv) !tile indices at emission point
* ...... vertex point coords in tile
         txv = gamvtx(1)-nxv*radtile_pitch
         tyv = gamvtx(2)-nyv*radtile_pitch
         if((abs(txv).gt.(radtile_pitch-radtile_supthk)/2.)
     +        .or.(abs(tyv).gt.(radtile_pitch-radtile_supthk)/2.)) then
            itflagv = 0
         else
            itflagv = 1
         endif
*         print*,'txv,tyv,itflagv = ',txv,tyv,itflagv
* ...... copy vertex to common (backup made at init)
         call ucopy(gamvtx,pcervtx,3)
* ...... debug
C         call uradlim(nxv,nyv,x1,x2,y1,y2,ig)
C         print*,gamvtx(1), gamvtx(2), gamvtx(3), '|', x1,x2,y1,y2,ig            
* ...... loop on the photon azimuthal angle
         do 1 phig=0.,twopi,1.E-2
* ......... check if photon is detected
            call patrace_full_phimir(phig,thetac,xdet,ydet,nmiref,nerr,
     +           verr,phimir)
            call getmirsec(phimir,imsec)
            if (nerr.ne.0) goto 1
* ......... photon counter
            nfotall = nfotall + 1
            if (nmiref.eq.0) then             
               nfotalldir = nfotalldir + 1 
            else
               if((imsec.lt.1).or.(imsec.gt.nmirsec)) then
                  print*,'(richaccradwall) Wrong mirror sector no.! - ',
     +                 imsec
               endif
*               print*,'(accradwall) phimir = ',phimir,', imsec = ',imsec
               nfotallref = nfotallref + 1 
               nfotallmsec(imsec) = nfotallmsec(imsec)+1
            endif
* ......... photon in detector frame
            call patphot(phig,thetac,phdir)
* ......... intersection with radiator walls
            pint(3) = HRAD                                      
            dist = (pint(3) - gamvtx(3))/phdir(3)      
            pint(1) = gamvtx(1) + dist*phdir(1)        
            pint(2) = gamvtx(2) + dist*phdir(2)        
            call uradtile(pint(1),pint(2),nxi,nyi) !tile indices at radiator bottom
* ......... bottom point coords in tile
            txi = pint(1)-nxi*radtile_pitch
            tyi = pint(2)-nyi*radtile_pitch
            if((abs(txi).gt.(radtile_pitch-radtile_supthk)/2.).or.
     +           (abs(tyi).gt.(radtile_pitch-radtile_supthk)/2.)) then
               itflagi = 0
            else
               itflagi = 1
            endif
*            print*,'... txi,tyi,itflagi = ',txi,tyi,itflagi

* ......... Scattering probability (R. Pereira 26-Nov-2008)
* ......... Calculation of interaction length was copied from RICHCHGREC

            /* clari   = usrcla ! clarity : radint = lambda^4(micm)/clarity */
            alambda = 377.E-3   ! mean wavelength (micm) no foil (higher with foil)
            if (RADCLARITY.gt.0) then
               alambda = (0.0327*RADCLARITY**(1.-0.867))**0.25
               radint  = (alambda)**4/RADCLARITY   
            else
               radint = 3.
            endif

            photprob = exp(-dist/radint)
            photprobmsec = photprob
            if(nmiref.ne.0)then
               photprob = photprob*reflec
               photprobmsec = photprobmsec*refmirs(imsec)
            endif

* ......... probability sum counter
            spfotall = spfotall + photprob
            if (nmiref.eq.0) then             
               spfotalldir = spfotalldir + photprob
            else
               spfotallref = spfotallref + photprob
               spfotallmsec(imsec) = spfotallmsec(imsec)+photprobmsec
            endif

            if ((nxi.ne.nxv).or.(nyi.ne.nyv)
     +              .or.(itflagv.eq.0).or.(itflagi.eq.0)) then ! wall crossed or vertex out of tile
               if (nmiref.eq.0) then
                  nfotwalldir = nfotwalldir + 1 
                  spfotwalldir = spfotwalldir + photprob
               else
                  nfotwallref = nfotwallref + 1 
                  spfotwallref = spfotwallref + photprob 
                  nfotwallmsec(imsec) = nfotwallmsec(imsec)+1
                  spfotwallmsec(imsec) =
     +                 spfotwallmsec(imsec)+photprobmsec
               endif
            endif
 1       continue
      enddo

* ... lost photons fraction
      accradwalldir = 0.
      accradwallref = 0.

*      if (nfotalldir.ne.0) 
*     + accradwalldir = float(nfotwalldir)/float(nfotalldir)
*      if (nfotallref.ne.0) 
*     + accradwallref = float(nfotwallref)/float(nfotallref)

      if (nfotalldir.ne.0) 
     + accradwalldir = spfotwalldir/spfotalldir
      if (nfotallref.ne.0) 
     + accradwallref = spfotwallref/spfotallref

      do i=1,nmirsec
         if (nfotallmsec(i).ne.0) 
     +        accradwallmsec(i) = spfotwallmsec(i)/spfotallmsec(i)
      enddo

*      print*,'N DIR: ',nfotwalldir,nfotalldir,
*     +     float(nfotwalldir)/float(nfotalldir)
*      print*,'N REF: ',nfotwallref,nfotallref,
*     +     float(nfotwallref)/float(nfotallref)
*      print*,'SP DIR: ',spfotwalldir,spfotalldir,
*     +     spfotwalldir/spfotalldir
*      print*,'SP REF: ',spfotwallref,spfotallref,
*     +     spfotwallref/spfotallref

*      print*,'SP MSEC: '
*      do i=1,nmirsec
*         print*,'(Sector ',i,'):',spfotwallmsec(i),spfotallmsec(i),
*     +        spfotwallmsec(i)/spfotallmsec(i)
*      enddo

* ... recover original pcervtx
      call ucopy(pcervtx_backup,pcervtx,3)

      return
      end
