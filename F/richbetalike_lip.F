****************************************************************
* In this file
* ------------
*
* --------------------------------------------------------------
*   Thetac Reconstruction
* --------------------------------------------------------------
* - subroutine richbetarec
* - subroutine richlikemin(iproc)
* - subroutine richgetphihint(chopt,nphihints,phihintsv,chi2hintsv,ierr)
* - function fliketotall(cangrecx)
* - function fchi2hit(phphi)
* --------------------------------------------------------------
*   Numerical Libs
* --------------------------------------------------------------
* - subroutine minbrack1(ax,bx,cx,fa,fb,fc,func,ierr)
* - subroutine minbrack(ax,bx,cx,fa,fb,fc,func,ierr)
* - subroutine zbrak(func,x1,x2,n,xb1,xb2,yb1,yb2,nb)
* - subroutine zbrak1(func,x1,x2,n,xb1,xb2,yb1,yb2,nb)
* - subroutine minbrackthe(ax,bx,cx,fa,fb,fc,func,nmax,nstep,ierr)
* - subroutine minbrackphi(ax,bx,cx,fa,fb,fc,func,nmax,nstep,ierr)
* - FUNCTION rtflsp_alt(func,x1,x2,xacc)
* - FUNCTION brent1(ax,bx,cx,f,tol,xmin)
* - FUNCTION brent2(ax,bx,cx,f,tol,xmin)
* - FUNCTION golden1(ax,bx,cx,f,tol,xmin)
* - FUNCTION golden2(ax,bx,cx,f,tol,xmin)
* - FUNCTION brentmod1(ax,bx,cx,f,tol,xmin,iter)
* - FUNCTION brentmod2(ax,bx,cx,f,tol,xmin,iter)
* - FUNCTION goldenmod1(ax,bx,cx,f,tol,xmin,nstep)
* - FUNCTION goldenmod2(ax,bx,cx,f,tol,xmin,nstep)
*
****************************************************************

*******************************
*** CODE FROM richbetarec.F ***
*******************************

* ===============================================================
      subroutine richbetarec
* ===============================================================
* ***************************************************************
* Velocity reconstruction steering routine (LIP rec)
*
* Changes: 26/Jan/05 (added pointer to non assoc hits)
* ***************************************************************

#include "../include/richrec_lipf.h"

      betarec = -999.

      if(rad_kind.eq.-1) return

      if (nbhits_nass.eq.0) return

* --- set reconstruction options
cc    RECBET(1)  = 11   ! unused
      RECBET(4)  = 1    ! minimum search with brent method

* --- LIP thetac reconstruction (minimization)
      call richlikemin(0)

      return
      end


*******************************
*** CODE FROM richlikemin.F ***
*******************************

      subroutine richlikemin(iproc)
***************************************************************
* AIMS: This routine finds the minimal cerenkov angle; it steers
*       the likelihood function minimization (fliketotall)
*
* IN  : iproc
*       <0 : only initialization of  variables directly
*            related to reconstruction is performed
*            - set cerenkov angle boundaries
*            - Likelihood probability parameters
*            - signal/background weight
*            - set particle frame matrix
*
* NOTE: nhitscan not yet implemented!
*
***************************************************************

#include "../include/richrec_lipf.h"

      real fliketotall
      external fliketotall

      real brentmod1, goldenmod1

      integer
     +        iproc, iter, nn,
     +        nscan

      integer nscanmax,nthetachintmax
      parameter(nscanmax=200, nthetachintmax=10)
      integer
     +     nbushits_buf(nscanmax),
     +     nthetachint,
     +     nbushits_thetachint(nthetachintmax)
      real
     +     fliketot_buf(nscanmax), thetac_buf(nscanmax),
     +     fliketot_bufmin,
     +     xthet, thc_hint,
     +     thetbrack1, thetbrack2, thetbrack3,
     +     chi2_evt, prob_evt, cangrecmin,
     +     cangrectol,
     +     h, f1, f2, f3, df1, df2,
     +     der1, der2,
     +     thetachint(nthetachintmax),func_thetachint(nthetachintmax)

***********   Init event by event *****************************

* --- init
      cangrectol = degrad*0.01 !brent tolerance

* --- max number of hits to scan
      nhitscan     = 0 !number of hits to be used on likelihood evaluation

* --- set particle rotation matrix
      if (irotflg.eq.0) then
         call patmatr
         irotflg = 1
      endif

* --- end of initialization
      if (iproc.lt.0) return

***********************************************************************

* ... Establish phi hint mechanism
*     To speed up the algorithm a phi hint mechanism was implemented
*     in routine richgetphihint.F once per event (and not for every thetac
*     being tested);

      IPHIHINTKIND = 1 !phi hint obtained by scanning at thc max
      if (rcgeom(1).eq.0) then !flight
         if (pcoopmtradius.lt.50. .and. abs(refindex-1.03).lt.0.005)
     +   IPHIHINTKIND = 2
         if (pcoopmtradius.lt.40. .and. abs(refindex-1.05).lt.0.005)
     +   IPHIHINTKIND = 2
      elseif (rcgeom(1).eq.9) then !prototype
         if (rcgeom(2).ne.1) IPHIHINTKIND = 2 ! no mirror
      endif
CC      IPHIHINTKIND = 0

* ... Performs thetac scan for sampling likelihood function;
*     the idea is to get the region where the absolute minimum
*     is located
* --> init variables
      ipmin = 0
      fliketot_bufmin = 1.E6
CC      thetac_buf   = 0.0
CC      nbushits_buf = 0
* --> init phi hint flag (used in richgetphihint.F)
      IPHIHINTFLAG = 0
* --> scan
      nscan = 0

      nxthet = int((cangrecup-cangreclow)/(THCSTEP*DEGRAD))
      do ixthet = 0,nxthet
         xthet = cangrecup-ixthet*THCSTEP*DEGRAD
         nscan = nscan + 1
         if (nscan.le.nscanmax) then
            fliketot_buf(nscan) = fliketotall(xthet)
            thetac_buf(nscan)   = xthet
            nbushits_buf(nscan) = nbushits_fit
         endif
         if (fliketot_buf(nscan).lt.fliketot_bufmin) then
            fliketot_bufmin = fliketot_buf(nscan)
            ipmin = nscan
         endif
         if (IPHIHINTKIND.ne.0) IPHIHINTFLAG = 1 !phi hint just done once
      enddo

* --> minimum
      if (ipmin.eq.0) then
         nthetachint = 0
      else
         nthetachint = 1
         thetachint(nthetachint) = thetac_buf(ipmin)
         nbushits_thetachint(nthetachint) = nbushits_buf(ipmin)
         func_thetachint(nthetachint) = fliketot_buf(ipmin)
      endif

/* commented 22/Feb/2007 (FB)
* --> find minima
      nthetachint = 0
      do i=3,nscan
         der1 = (fliketot_buf(i-1)-fliketot_buf(i-2))/thcstep
         der2 = (fliketot_buf(i)-fliketot_buf(i-1))/thcstep
         if (der1.lt.0..and.der2.gt.0.) then !minimum found
            nthetachint = nthetachint + 1
            if (nthetachint.le.nthetachintmax) then
               thetachint(nthetachint) = thetac_buf(i-1)
               nbushits_thetachint(nthetachint) = nbushits_buf(i-1)
            endif
         endif
      enddo
*/

      if (nthetachint.eq.0) then
         ipthetac = 0
         ntherec  = 0
         betarec = -999.
         call vfill(cangrec,ntherecmax,-999.)
         call vzero(nbushits,ntherecmax)
         return
      endif

* PRINT
#ifdef DEBUGPRINT1
      print*,'(richlikemin) nthetachint=',nthetachint
      print*,'(richlikemin)',(thetachint(j)*RADDEG,j=1,nthetachint)
      print*,'(richlikemin)',(func_thetachint(j),j=1,nthetachint)
      print*,'(richlikemin)',(nbushits_thetachint(j),j=1,nthetachint)
#endif
* ... THETAC minimization
      chi2_evt_min=1.E9
      nbushits_min = 0
      ipmin = 0
      do i=1,nthetachint
*         print*,'--- (richlikemin) theta_c hint = ',i
         thc_hint   = thetachint(i)
         thetbrack2 = thc_hint
         thetbrack1 = thc_hint-THCSTEP*DEGRAD
         thetbrack3 = thc_hint+THCSTEP*DEGRAD
         if (int(recbet(4)).eq.0) then
            chi2_evt = goldenmod1(thetbrack1,thetbrack2,thetbrack3,
     +           fliketotall,cangrectol,cangrecmin,iter)
CC            prob_evt = fliketotall(cangrecmin)
         else
            chi2_evt = brentmod1(thetbrack1,thetbrack2,thetbrack3,
     +           fliketotall,cangrectol,cangrecmin,iter)
CC            prob_evt = fliketotall(cangrecmin)
         endif
         if (chi2_evt.lt.chi2_evt_min .and.
     +       nbushits_fit.gt.nbushits_min) then
            chi2_evt_min = chi2_evt
            nbushits_min = nbushits_fit
* --------> store reconstructed values
            if (nbushits_fit.gt.0) then
               ipthetac = 1
               ntherec  = 1
               cangrec(ipthetac)    = cangrecmin
               likerec(ipthetac)    = chi2_evt
               cangrecdif(ipthetac) = -999.
               chi2rec(ipthetac)    = chi2rec_fit/real(nbushits_fit)
               nbushits(ipthetac)   = nbushits_fit
*               print*,'----> NBUSHITS(ipthetac) = ',nbushits(ipthetac)
               do nn=1,nbushits_fit
                  ipushits(ipthetac,nn) = ipushits_fit(nn)
                  iflghit(ipushits_fit(nn)) = 2
               enddo
               do nn=1,min(nbhits,nhitmax)
                  chi2hit(ipthetac,nn)    = chi2hit_fit(nn)
                  nbminshit(ipthetac,nn)  = nbminshit_fit(nn)
                  phihit(ipthetac,nn)     = phihit_fit(nn)
                  ireflechit(ipthetac,nn) = ireflechit_fit(nn)
                  imsechit(ipthetac,nn)   = imsechit_fit(nn)
               enddo
* -----------> store beta reconstruction
               betarec = 1./refindex/cos(cangrec(ipthetac))
* -----------> store likelihood value
               resvlike = fliketotall(cangrec(ipthetac))
            else
               ipthetac = 0
               ntherec  = 0
               betarec = -999.
               call vfill(cangrec,ntherecmax,-999.)
               call vzero(nbushits,ntherecmax)
               call vfill(cangrec,ntherecmax,-999.)
               resvlike = -999.
            endif
         endif
      enddo

* ... compute minimum second derivative
* ... (there is currently a bug here since hits are not untagged)
      h   = 1.E-3
      df2 = 0.0
      if (ipthetac.ne.0) cangrecmin = cangrec(ipthetac)
      do while (df2.eq.0.0)
         f1  = fliketotall(cangrecmin+h)
         f2  = fliketotall(cangrecmin-h)
         df2 = (f1+f2-2.*prob_evt)/h**2
         df1 = (f1-f2)/h
         h   = 2.*h
      enddo

      d2loglike = df2

* PRINT
#ifdef DEBUGPRINT
      write(6,*) 'THETAC MINIMIZATION RESULTS'
      do i=1,min(nbhits,nhitmax)
         if (iflghit(i).ne.1) then
            write(6,'(I3,1x,I1,1x,F5.2,1x,A,1x,F6.1,1x)')
     +           i,iflghit(i),phihit(ipthetac,i),'*',chi2hit(ipthetac,i)
         endif
      enddo
#endif

* ... return
      return
      end


*******************************
*** CODE FROM fliketotall.F ***
*******************************

*********************************************************************
      real function fliketotall(cangrecx)
*********************************************************************
*
* AIMS: likelihood function computed as the product of the hits
*       probability to belong to the tested pattern(cangrecx);
*
*   IN: cangrecx = cerenkov angle
*
* DATE: Feb/2007
*
*********************************************************************

#include "../include/richrec_lipf.h"

      real fchi2hit
      external fchi2hit ! distance**2 for each hit

      character chopt*4

      integer nphihintsmax
      parameter (nphihintsmax=10)
      integer nphihints,nphihintsbuf(nhitmax)
      real phihintsv(nphihintsmax),chi2hintsv(nphihintsmax),
     +     phihintsbuf(nphihintsmax,nhitmax)
      save phihintsbuf, nphihintsbuf

      real chi2_hit, chi2hit_min, phihit_min
      integer nmirefhit_min, msechit_min

      real gcte, fgaussct, gcte1, gcte2, fact_weight

*      print*,'pimp = ',pimp
*      print*,'pcoopmt = ',pcoopmt
*      print*,'pthe,pphi(deg) = ',pthe*RADDEG,pphi*RADDEG

* --- init
      fliketotall  = 0.0
      nbushits_fit = 0
      chi2rec_fit  = 0.0
      call vfill(chi2hit_fit,nhitmax,9.99E10)

* ... tolerance for determination minimum
      phitol = 0.01*degrad
      nmaxiter = 25

* --- pass variable
      hypthc = cangrecx
      if (hypthc.eq.0.0) hypthc = 0.001

* --- exception conditions
      if (nbhits.eq.0          .or.
     &    nbhits_nass.eq.0     .or.    ! added 06-Mar-2007 by R. Pereira
     &    hypthc.lt.cangreclow .or.
CC     &    hypthc.gt.cangrecup
     &    hypthc.gt.cangrecup*13./11.
     &    ) then
         fliketotall = 1.E8
*         print*,'---> likelihood value set to 1E8 (1st exception)'
*         print*,'     . nbhits = ',nbhits
*         print*,'     . nbhits_nass = ',nbhits_nass
*         print*,'     . hypthc (deg)     = ',hypthc*raddeg
*         print*,'     . cangreclow (deg) = ',cangreclow*raddeg
*         print*,'     . cangrecup (deg)  = ',cangrecup*raddeg
         return
      endif

*      print*,'nhitscan = ',nhitscan
*      print*,'nbhits_nass = ',nbhits_nass
*      print*,'istephit = ',istephit

* --- how many hits to use on likelihood computation
      if (nhitscan.eq.0) nhitscan = nbhits_nass
      istephit = int(nbhits_nass/nhitscan)
      if (istephit.eq.0) istephit = 1

* --- loop on data hits non-associated to particle
      do 1 n=1,min(nbhits_nass,nhitscan*istephit),istephit
* ...... get pointer to data hits (non-assoc)
         i = ip_nass(n)
*         print*,'N=',n,', I=',i,', iflghit(I)=',iflghit(i)
* ...... exclude associated hits to track (dummy!!!)
         if (iflghit(i).eq.1) goto 1 !next hit
* ...... exclude hits already used by some reconstruction
         if (iflghit(i).eq.2) goto 1 !next hit
* ...... exclude hits farther than 10 cm for maximum cerenkov pattern
         if (iflghit(i).eq.3) goto 1 !next hit
* ...... exclude any other anomalous hits
         if (iflghit(i).gt.0) goto 1 !next hit
* ...... current hit
         ihit = i
         curhit(1) = hitscoo(1,i)
         curhit(2) = hitscoo(2,i)
         curhit(3) = hitscoo(3,i)
* ...... get phi hints for minimization
         if (iphihintflag.eq.0) then ! defined at richlikemin.F
            if (iphihintkind.eq.0) CHOPT='FULL'
            if (iphihintkind.eq.1) CHOPT='SCAN'
            if (iphihintkind.eq.2) CHOPT='GEOM'
            call richgetphihint(CHOPT,nphihints,phihintsv,chi2hintsv,
     +                          ierr)
            if (ierr.ge.2) then !hit too far
               if (ierr.eq.2) iflghit(i) = 3
               goto 1           ! next hit
            endif
            nphihintsbuf(i) = nphihints
            call ucopy(phihintsv,phihintsbuf(1,i),nphihints)

#ifdef DEBUGPRINT
* PRINT
            write(6,'(A,1x,I3,2X,I1,2x,$)') CHOPT,i,iflghit(i)
            do m=1,nphihints
               write(6,'(2x,F4.2,1x,A,1x,F7.1,1x,A,1x$)')
     +         phihintsbuf(m,i),'*',chi2hintsv(m),'|'
            enddo
            write(6,'(A)') ' '
#endif
         endif
* ...... reject hit if no minimum at the hint phase was found on phi
         if (nphihintsbuf(i) .eq.0) goto 1
* ...... Choose minimal phi hint and perform phi minimization
         chi2hit_min = 1.E9
         do 3 j=1,nphihintsbuf(i)
            phibrack1 = phihintsbuf(j,i) - 1.0*degrad
            phibrack2 = phibrack1        + 2.0*degrad
* --------> phi minimum brackecting
            call minbrackphi(phibrack1,phibrack2,phibrack3,
     +                       funcphi1,funcphi2,funcphi3,
     +                       fchi2hit,nmaxiter,nsteps,ierr)
            if (ierr.ne.0) goto 3
* --------> phi minimization
            chi2_hit = brentmod2(phibrack1,phibrack2,phibrack3,
     +                           fchi2hit,phitol,phphimin,iter)
* --------> Get 0 < phphimin < 360 Degree
            if (int(phphimin/twopi).gt.0)then
               phphimin = phphimin - int(phphimin/twopi)*twopi
            endif
            if(phphimin.lt.0.)then
               phphimin = phphimin + (int(phphimin/twopi)+1)*twopi
            endif
* --------> Keep minimum
            if (chi2_hit.lt.chi2hit_min) then
               chi2hit_min   = chi2_hit
               phihit_min    = phphimin
               nmirefhit_min = nmirefhit !from richrec.inc common
               msechit_min   = msechit !from richrec.inc common
            endif
 3       continue
*         print*,'Hit at ',curhit(1),curhit(2),', chi2 = ',chi2hit_min
         if (chi2hit_min.gt.1.E7) goto 1
* ...... store hit variables
         phihit_fit(i)     = phihit_min
         chi2hit_fit(i)    = chi2hit_min
         ireflechit_fit(i) = nmirefhit_min
         imsechit_fit(i) = msechit_min
*         print*,'Current pimp:',pimp
*         print*,'Current pthe,pphi (deg):',pthe*RADDEG,pphi*RADDEG
*         print*,'Current pcoopmt:',pcoopmt
* ...... Compute probability function (Gauss+constant) for the hit
#ifdef DEBUGPRINT0
         write(6,'(A,1x,6(F7.3,1x))') '(fliketotall) LIKELIHOOD FUNC',
     +   FBACKGR, FDISTAN, F1NORMG, F1SIGMA2, F2SIGMA2
#endif
* ----> DOUBLE GAUSSIAN
         gcte1   = F1NORMG/sqrt(twopi*F1SIGMA2)
         gcte2   = (1.-F1NORMG)/sqrt(twopi*F2SIGMA2)
         fgaussct = -log(
     +                   (1.-FBACKGR)*(
     +                   gcte1*exp(-0.5*chi2hit_fit(i)/F1SIGMA2)+
     +                   gcte2*exp(-0.5*chi2hit_fit(i)/F2SIGMA2))+
     +                   FBACKGR/FDISTAN
     +                  )
* ----> weight probability with signal
         if (hitsnpe(i).lt.1.)then
            fact_weight=1.
         else
cc            fact_weight=nint(hitsnpe(i))
            fact_weight=hitsnpe(i)
         endif
         fgaussct = fgaussct*fact_weight
*         fgaussct = fgaussct*hitsnpe(i) ! (weight with signal) 8 Jan 2004
* -----> Compute likelihood function
         fliketotall = fliketotall + fgaussct
* ...... tag hits associated to ring
*         print*,'chi2hit_fit(',i,') = ',chi2hit_fit(i)
         if (chi2hit_fit(i).lt.CHI2HTCUT ) then
*            print*,'Hit ',i,' counted for ring'
            nbushits_fit               = nbushits_fit + 1
            ipushits_fit(nbushits_fit) = i
            chi2rec_fit                = chi2rec_fit + chi2hit_fit(i)
         endif

* --- loop on hits
 1    continue

*      print*,'EVENT ',ievnumb

*      print*,'     . fliketotall = ',fliketotall
*      print*,'     . nbhits = ',nbhits
*      print*,'     . nbushits_fit = ',nbushits_fit

*      do k=1,nbushits_fit
*         print*,'     ... ipushits_fit(',k,') = ',ipushits_fit(k)
*      enddo

* ... no likelihood was calculated
      if (fliketotall.eq.0..and.nbushits_fit.eq.0) then
CC        print*,'(fliketotall) nbushits=0!!!'
         fliketotall = 1.E8
*         print*,'---> likelihood value set to 1E8 (2nd exception)'
         return
      endif

* --- return
      return
      end


**********************************
*** CODE FROM richgetphihint.F ***
**********************************


**************************************************************************
      subroutine richgetphihint(chopt,nphihints,phihintsv,chi2hintsv,
     +                          ierr)
**************************************************************************
*
*  This routine computes the phi hints for the current hit
*  at common /cerwrkc/ (richrec.inc);
*
*  IN : CHOPT = 'FULL' - full scan of phi for current thetac
*               'SCAN' - a scanning is performed over the
*                        the phi values to get fast minimal values
*                        for thetac max
*               'GEOM' - the hit coordinates are converted are
*                        transformed to the particle frame and
*                        converted in an azimuthal angle phi
*
*  OUT: nphihints      - number of phi hints obtained
*       phihintsv      - values of phi hints
*       chi2hintsv     - values of squared distances
*       ierr           =0 ok
*                      =1 nb hits larger than array
*                      =2 hit distance too large
*                      =3 minimum not found
*
*  WARNING: hypthc is changed inside this routine, but
*           the old value is restored at the end.
*
*  Feb/2007 (FB)
****************************************************************

#include "../include/richrec_lipf.h"

      character chopt*4
      real phisteplocal, dif21, dif32, xphi
      integer nscan, ierr
      integer nscanmax
      parameter(nscanmax=200)
      real funcbuf(nscanmax), phibuf(nscanmax), funcbuf_min
      integer nphihintsmax
      parameter (nphihintsmax=10)
      integer nphihints
      real phihintsv(nphihintsmax), chi2hintsv(nphihintsmax)
      real hypthcbuf, chi2_hit
      external fchi2hit

* ... init
      call vfill(phihintsv,nphihintsmax,0.)
      call vfill(chi2hintsv,nphihintsmax,0.)
      nphihints = 0
      ierr = 0

C -----------------------------------------------------
      IF (CHOPT.eq.'SCAN') THEN
C -----------------------------------------------------

* ...... define phi step
         phisteplocal = 5.
* ...... define maximum cerenkov angle
         hypthcbuf = hypthc
         hypthc    = acos(1./refindex)*1.10
* ...... sampling the function fchi2hit (photon distance to pattern)
         nscan=0
         funcbuf_min=1.e9

         nxphi = int(365./phisteplocal)
         do ixphi = 0,nxphi
           xphi = ixphi*phisteplocal
           nscan=nscan+1
           if (nscan.gt.nscanmax) then
              print*,' (richgetphihint) array dim overflow '
              ierr = 1
              goto 98
           endif
           funcbuf(nscan) = fchi2hit(degrad*xphi) !hypthc passed by common
           phibuf(nscan)  = degrad*xphi
           if (funcbuf(nscan).lt.funcbuf_min) funcbuf_min=funcbuf(nscan)
         enddo

* ....,. reject hit if no minimum found
         if (funcbuf_min.gt.1.e7 .or. nscan.eq.0) then
            ierr = 3
            goto 98
         endif

* ...... search phi minimum's
         do i=3,nscan
           dif21=(funcbuf(i-1)-funcbuf(i-2))/phisteplocal
           dif32=(funcbuf(i)-funcbuf(i-1))/phisteplocal
           if (dif21.lt.0..and.dif32.gt.0.) then ! minimum found
             if (chradid.eq.'AGL') then
                if (funcbuf(i-1).lt.250.) then !distance cut
                  nphihints=nphihints+1
                  if (nphihints.le.nphihintsmax) then
                     chi2hintsv(nphihints)=funcbuf(i-1)
                     phihintsv(nphihints)=phibuf(i-1)
                     if (phihintsv(nphihints)*RADDEG.gt.360.) then
                        phihintsv(nphihints)=phihintsv(nphihints)-twopi
                     endif
                  else
                     print*,'(richgetphihint) nb phi hints too large!!!'
                  endif
                endif
             elseif (chradid.eq.'NAF') then
                nphihints=nphihints+1
                if (nphihints.le.nphihintsmax) then
                   chi2hintsv(nphihints)=funcbuf(i-1)
                   phihintsv(nphihints)=phibuf(i-1)
                   if (phihintsv(nphihints)*RADDEG.gt.360.) then
                      phihintsv(nphihints)=phihintsv(nphihints)-twopi
                   endif
                else
                   print*,'(richgetphihint) nb phi hints too large!!!'
                endif
             endif
           endif
         enddo
         if (nphihints.eq.0) then
            ierr = 2
            goto 98
         endif

* ...... restore hypthc
 98      continue
         hypthc = hypthcbuf

C -----------------------------------------------------
      ELSEIF (CHOPT.eq.'GEOM') THEN
C -----------------------------------------------------

         nphihints = 1
* ...... get phi hint
         dxdet = curhit(1)-pcervtx(1)
         dydet = curhit(2)-pcervtx(2)
         dzdet = curhit(3)-pcervtx(3)
         if (pthe*RADDEG.ne.0.) then
            dxpar = - cos(pthe)*cos(pphi)*dxdet
     +              - cos(pthe)*sin(pphi)*dydet
     +              + sin(pthe)*dzdet
            dypar =   sin(pphi)*dxdet
     +              - cos(pphi)*dydet
            phihintsv(nphihints) = atan2(dypar,dxpar)
            if (phihintsv(nphihints).lt.0.)
     +      phihintsv(nphihints)=phihintsv(nphihints)+twopi
         else
            phihintsv(nphihints) = atan2(dydet,dxdet)
            if(phihintsv(nphihints).lt.0.)
     +      phihintsv(nphihints)=phihintsv(nphihints)+twopi
         endif

* ...... tag hits too far from thetac max ring
         hypthcbuf     = hypthc
         hypthc        = acos(1./refindex)
         chi2hintsv(1) = fchi2hit(phihintsv(1))
         hypthc        = hypthcbuf

* ...... distance cut
         if (chi2hintsv(1).gt.500.) nphihints = 0

C -----------------------------------------------------
      ELSEIF (CHOPT.eq.'FULL') THEN
C -----------------------------------------------------

         nphihints = 0
* ...... sampling the function fchi2hit (photon distance to pattern)
         nscan = 0
         ipmin = 0
         funcbuf_min=1.e9

         nxphi = int(360./PHISTEP)
         do ixphi = 0,nxphi
           xphi = ixphi*PHISTEP
           nscan = nscan + 1
           if (nscan.gt.nscanmax) then
              print*,' (richgetphihint) array dim overflow '
              ierr = 1
              return
           endif
           funcbuf(nscan) = fchi2hit(degrad*xphi)
           phibuf(nscan)  = degrad*xphi
           if (funcbuf(nscan).lt.funcbuf_min) then
              funcbuf_min = funcbuf(nscan)
              ipmin       = nscan
           endif
         enddo

* ....,. reject hit
         if (funcbuf_min.gt.1.e7 .or. ipmin.eq.0) then
            ierr = 3
            return
         endif

* ...... output
         nphihints     = 1
         phihintsv(1)  = phibuf(ipmin) !radians
         chi2hintsv(1) = funcbuf(ipmin)

C -----------------------------------------------------
      ENDIF
C -----------------------------------------------------

* ... end
      return
      end


****************************
*** CODE FROM fchi2hit.F ***
****************************


      real function fchi2hit(phphi)

#include "../include/richrec_lipf.h"

      integer nermax
      parameter(nermax=10)
      integer verr(nermax)
      real vdet(3),vdir(3)
      real vdetcopy(3),vdirinv(3)
      real phimir
      integer ierr
      integer imsec
*
      fchi2hit = 0.
      cangrecx = hypthc
      CALL PATRACE(phphi,cangrecx,vdet,vdir,nmiref,nerr,verr)
      nmirefhit = nmiref

* ... mirror phi
      if(nmiref.eq.0) then
         phimir = -999.
         imsec = 0
      else
         vdetcopy(1) = vdet(1)
         vdetcopy(2) = vdet(2)
         vdetcopy(3) = vdet(3)
*         vdirinv(1) = -vdir(1)
*         vdirinv(2) = -vdir(2)
*         vdirinv(3) = -vdir(3)

         call patints(vdetcopy,vdir,'MIRI',ierr)

         phimir = atan2(vdetcopy(2)-vtmir(2),vdetcopy(1)-vtmir(1))
         if(phimir.lt.0.) phimir = phimir+TWOPI
         call getmirsec(phimir,imsec)
*         print*,'[fchi2hit] phimir = ',phimir,', imsec = ',imsec
      endif
      msechit = imsec

*      if (nerr.ne.0 .and. verr(1).ne.19) then
      if (nerr.ne.0) then
         fchi2hit = 1.E8
         return
      endif
      fchi2hit = (curhit(1)-vdet(1))**2 + (curhit(2)-vdet(2))**2
      return
      end


****************************
*** CODE FROM richnumr.F ***
****************************

*******************************************************************************
*    File with subroutines/ functions for numerical computation
*
*
*    INDEX (by apearing order):
*
*        -> Bracketing of minimum (for single-valued functions)
*
*                   minbrack1
*                   minbrack
*        -> Finding of a minimum in a minimum bracket
*                   brent1
*                   brent2
*                   golden1
*                   golden2
*        -> Bracketing of zeros (for single-valued functions)
*
*                   zbrak
*                   zbrak1
*        -> Finding the zero of a function in an interval according
*           to the false position method
*                   RTFLSP_ALT
*
*        -> Modified versions for some of previous routines/functions:
*                   brentmod1
*                   brentmod2
*                   goldenmod1
*                   goldenmod2
*                   minbrackthe
*                   minbrackphi
*
*******************************************************************************



      subroutine minbrack1(ax,bx,cx,fa,fb,fc,func,ierr)
*******************************************************************************
*     this routine makes the bracketing of a minimum of the function...
*
*     extracted from NUMERICAL RECIPES in Fortran
*     Chapter. 10: Function Minimization
* -----------------------------------------------------------------------------
* the output bracket is given by ax,bx,cx where ax<bx<cx (or ax>bx>cx )
* and f(ax)>f(bx) and f(cx)>f(bx)
*
*
********************************************************************************
      implicit none
      REAL ax,bx,cx,fa,fb,fc,func,GOLD,GLIMIT,TINY
      EXTERNAL func
      PARAMETER (GOLD=1.618034, GLIMIT=100., TINY=1.e-20)
      REAL dum,fu,q,r,u,ulim
      integer ierr,nstep,nstepmax
      parameter(nstepmax=50)
      ierr=0
      nstep=0
      fa=func(ax)
      fb=func(bx)
      if(fb.gt.fa)then
        dum=ax
        ax=bx
        bx=dum
        dum=fb
        fb=fa
        fa=dum
      endif
      cx=bx+GOLD*(bx-ax)
      fc=func(cx)
      nstep=nstep+1
1     if(fb.ge.fc)then
        nstep=nstep+1
        if(nstep.lt.nstepmax)then
        r=(bx-ax)*(fb-fc)
        q=(bx-cx)*(fb-fa)
        u=bx-((bx-cx)*q-(bx-ax)*r)/(2.*sign(max(abs(q-r),TINY),q-r))
        ulim=bx+GLIMIT*(cx-bx)
        if((bx-u)*(u-cx).gt.0.)then
          fu=func(u)
          if(fu.lt.fc)then
            ax=bx
            fa=fb
            bx=u
            fb=fu
            return
          else if(fu.gt.fb)then
            cx=u
            fc=fu
            return
          endif
          u=cx+GOLD*(cx-bx)
          fu=func(u)
        else if((cx-u)*(u-ulim).gt.0.)then
          fu=func(u)
          if(fu.lt.fc)then
            bx=cx
            cx=u
            u=cx+GOLD*(cx-bx)
            fb=fc
            fc=fu
            fu=func(u)
          endif
        else if((u-ulim)*(ulim-cx).ge.0.)then
          u=ulim
          fu=func(u)
        else
          u=cx+GOLD*(cx-bx)
          fu=func(u)
        endif
        ax=bx
        bx=cx
        cx=u
        fa=fb
        fb=fc
        fc=fu
        else
          ierr=1
          return
        endif
        goto 1
      endif

      return
      END


C=============================================================================

      subroutine minbrack(ax,bx,cx,fa,fb,fc,func,ierr)
*******************************************************************************
*     this routine makes the bracketing of a minimum of the function...
*
*     extracted from NUMERICAL RECIPES in Fortran
*     Chapter. 10: Function Minimization
*
********************************************************************************
      implicit none
      REAL ax,bx,cx,fa,fb,fc,func,GOLD,GLIMIT,TINY
      EXTERNAL func
      PARAMETER (GOLD=1.618034, GLIMIT=100., TINY=1.e-20)
      REAL dum,fu,q,r,u,ulim
      integer ierr,nstep,nstepmax
      parameter(nstepmax=50)
      ierr=0
      nstep=0
      fa=func(ax)
      fb=func(bx)
      if(fb.gt.fa)then
        dum=ax
        ax=bx
        bx=dum
        dum=fb
        fb=fa
        fa=dum
      endif
      cx=bx+GOLD*(bx-ax)
      fc=func(cx)
      nstep=nstep+1
1     if(fb.ge.fc)then
        nstep=nstep+1
        if(nstep.lt.nstepmax)then
        r=(bx-ax)*(fb-fc)
        q=(bx-cx)*(fb-fa)
        u=bx-((bx-cx)*q-(bx-ax)*r)/(2.*sign(max(abs(q-r),TINY),q-r))
        ulim=bx+GLIMIT*(cx-bx)
        if((bx-u)*(u-cx).gt.0.)then
          fu=func(u)
          if(fu.lt.fc)then
            ax=bx
            fa=fb
            bx=u
            fb=fu
            return
          else if(fu.gt.fb)then
            cx=u
            fc=fu
            return
          endif
          u=cx+GOLD*(cx-bx)
          fu=func(u)
        else if((cx-u)*(u-ulim).gt.0.)then
          fu=func(u)
          if(fu.lt.fc)then
            bx=cx
            cx=u
            u=cx+GOLD*(cx-bx)
            fb=fc
            fc=fu
            fu=func(u)
          endif
        else if((u-ulim)*(ulim-cx).ge.0.)then
          u=ulim
          fu=func(u)
        else
          u=cx+GOLD*(cx-bx)
          fu=func(u)
        endif
        ax=bx
        bx=cx
        cx=u
        fa=fb
        fb=fc
        fc=fu
        else
          ierr=1
          return
        endif
        goto 1
      endif

      return
      END

* ====================================================================
      FUNCTION brent1(ax,bx,cx,f,tol,xmin)
* ====================================================================
      implicit none
      INTEGER ITMAX
      REAL brent1,ax,bx,cx,tol,xmin,f,CGOLD,ZEPS
      EXTERNAL f
      PARAMETER (ITMAX=100,CGOLD=.3819660,ZEPS=1.0e-10)
      INTEGER iter
      REAL a,b,d,e,etemp,fu,fv,fw,fx,p,q,r,tol1,tol2,u,v,w,x,xm
      a=min(ax,cx)
      b=max(ax,cx)
      v=bx
      w=v
      x=v
      e=0.
      fx=f(x)
      fv=fx
      fw=fx
      do 11 iter=1,ITMAX
        xm=0.5*(a+b)
        tol1=tol*abs(x)+ZEPS
        tol2=2.*tol1
        if(abs(x-xm).le.(tol2-.5*(b-a))) goto 3
        if(abs(e).gt.tol1) then
          r=(x-w)*(fx-fv)
          q=(x-v)*(fx-fw)
          p=(x-v)*q-(x-w)*r
          q=2.*(q-r)
          if(q.gt.0.) p=-p
          q=abs(q)
          etemp=e
          e=d
          if(abs(p).ge.abs(.5*q*etemp).or.p.le.q*(a-x).or.p.ge.q*(b-x))
     *goto 1
          d=p/q
          u=x+d
          if(u-a.lt.tol2 .or. b-u.lt.tol2) d=sign(tol1,xm-x)
          goto 2
        endif
1       if(x.ge.xm) then
          e=a-x
        else
          e=b-x
        endif
        d=CGOLD*e
2       if(abs(d).ge.tol1) then
          u=x+d
        else
          u=x+sign(tol1,d)
        endif
        fu=f(u)
        if(fu.le.fx) then
          if(u.ge.x) then
            a=x
          else
            b=x
          endif
          v=w
          fv=fw
          w=x
          fw=fx
          x=u
          fx=fu
        else
          if(u.lt.x) then
            a=u
          else
            b=u
          endif
          if(fu.le.fw .or. w.eq.x) then
            v=w
            fv=fw
            w=u
            fw=fu
          else if(fu.le.fv .or. v.eq.x .or. v.eq.w) then
            v=u
            fv=fu
          endif
        endif
11    continue
      print*,'brent exceed maximum iterations'
#ifdef __AMSDEBUG__
      pause ' '
#endif
3     xmin=x
      brent1=fx
      return
      END
* ====================================================================
      FUNCTION brent2(ax,bx,cx,f,tol,xmin)
* ====================================================================
      implicit none
      INTEGER ITMAX
      REAL brent2,ax,bx,cx,tol,xmin,f,CGOLD,ZEPS
      EXTERNAL f
      PARAMETER (ITMAX=100,CGOLD=.3819660,ZEPS=1.0e-10)
      INTEGER iter
      REAL a,b,d,e,etemp,fu,fv,fw,fx,p,q,r,tol1,tol2,u,v,w,x,xm
      a=min(ax,cx)
      b=max(ax,cx)
      v=bx
      w=v
      x=v
      e=0.
      fx=f(x)
      fv=fx
      fw=fx
      do 11 iter=1,ITMAX
        xm=0.5*(a+b)
        tol1=tol*abs(x)+ZEPS
        tol2=2.*tol1
        if(abs(x-xm).le.(tol2-.5*(b-a))) goto 3
        if(abs(e).gt.tol1) then
          r=(x-w)*(fx-fv)
          q=(x-v)*(fx-fw)
          p=(x-v)*q-(x-w)*r
          q=2.*(q-r)
          if(q.gt.0.) p=-p
          q=abs(q)
          etemp=e
          e=d
          if(abs(p).ge.abs(.5*q*etemp).or.p.le.q*(a-x).or.p.ge.q*(b-x))
     *goto 1
          d=p/q
          u=x+d
          if(u-a.lt.tol2 .or. b-u.lt.tol2) d=sign(tol1,xm-x)
          goto 2
        endif
1       if(x.ge.xm) then
          e=a-x
        else
          e=b-x
        endif
        d=CGOLD*e
2       if(abs(d).ge.tol1) then
          u=x+d
        else
          u=x+sign(tol1,d)
        endif
        fu=f(u)
        if(fu.le.fx) then
          if(u.ge.x) then
            a=x
          else
            b=x
          endif
          v=w
          fv=fw
          w=x
          fw=fx
          x=u
          fx=fu
        else
          if(u.lt.x) then
            a=u
          else
            b=u
          endif
          if(fu.le.fw .or. w.eq.x) then
            v=w
            fv=fw
            w=u
            fw=fu
          else if(fu.le.fv .or. v.eq.x .or. v.eq.w) then
            v=u
            fv=fu
          endif
        endif
11    continue
      print*,'brent exceed maximum iterations'
#ifdef __AMSDEBUG__
      pause ' '
#endif
3     xmin=x
      brent2=fx
      return
      END

********************************************************
********************************************************

      FUNCTION golden1(ax,bx,cx,f,tol,xmin)
      implicit none
      REAL golden1,ax,bx,cx,tol,xmin,f,R,C
      EXTERNAL f
      PARAMETER (R=.61803399,C=1.-R)
      REAL f1,f2,x0,x1,x2,x3
      x0=ax
      x3=cx
      if(abs(cx-bx).gt.abs(bx-ax))then
        x1=bx
        x2=bx+C*(cx-bx)
      else
        x2=bx
        x1=bx-C*(bx-ax)
      endif
      f1=f(x1)
      f2=f(x2)
1     if(abs(x3-x0).gt.tol*(abs(x1)+abs(x2)))then
        if(f2.lt.f1)then
          x0=x1
          x1=x2
          x2=R*x1+C*x3
          f1=f2
          f2=f(x2)
        else
          x3=x2
          x2=x1
          x1=R*x2+C*x0
          f2=f1
          f1=f(x1)
        endif
      goto 1
      endif
      if(f1.lt.f2)then
        golden1=f1
        xmin=x1
      else
        golden1=f2
        xmin=x2
      endif
      return
      END

********************************************************
********************************************************

      FUNCTION golden2(ax,bx,cx,f,tol,xmin)
      implicit none
      REAL golden2,ax,bx,cx,tol,xmin,f,R,C
      EXTERNAL f
      PARAMETER (R=.61803399,C=1.-R)
      REAL f1,f2,x0,x1,x2,x3
      x0=ax
      x3=cx
      if(abs(cx-bx).gt.abs(bx-ax))then
        x1=bx
        x2=bx+C*(cx-bx)
      else
        x2=bx
        x1=bx-C*(bx-ax)
      endif
      f1=f(x1)
      f2=f(x2)
1     if(abs(x3-x0).gt.tol*(abs(x1)+abs(x2)))then
        if(f2.lt.f1)then
          x0=x1
          x1=x2
          x2=R*x1+C*x3
          f1=f2
          f2=f(x2)
        else
          x3=x2
          x2=x1
          x1=R*x2+C*x0
          f2=f1
          f1=f(x1)
        endif
      goto 1
      endif
      if(f1.lt.f2)then
        golden2=f1
        xmin=x1
      else
        golden2=f2
        xmin=x2
      endif
      return
      END






* ===================================================================

* ======================================================
      subroutine zbrak(func,x1,x2,n,xb1,xb2,yb1,yb2,nb)
* ======================================================
      implicit none
      external func
      integer n,nb
      real func,x1,x2,xb1(nb),xb2(nb),yb1(nb),yb2(nb)
      integer i,nbb
      real dx,fc,fp,x
      nbb = 0
      x = x1
      dx = (x2-x1)/n
      fp = func(x)
      do i=1,n
         x = x + dx
         fc = func(x)
         if (fc*fp.le.0.) then
            nbb = nbb + 1
            xb1(nbb) = x-dx
            xb2(nbb) = x
            yb1(nbb) = fp
            yb2(nbb) = fc
            if (nbb.eq.nb) goto 1
         endif
         fp = fc
      enddo
 1    continue
      nb = nbb
      return
      end

* ======================================================
      subroutine zbrak1(func,x1,x2,n,xb1,xb2,yb1,yb2,nb)
* ======================================================
      implicit none
      external func
      integer n,nb
      real func,x1,x2,xb1(nb),xb2(nb),yb1(nb),yb2(nb)
      integer i,nbb
      real dx,fc,fp,x
      nbb = 0
      x = x1
      dx = (x2-x1)/n
      fp = func(x)
      do i=1,n
         x = x + dx
         fc = func(x)
         if (fc*fp.le.0.) then
            nbb = nbb + 1
            xb1(nbb) = x-dx
            xb2(nbb) = x
            yb1(nbb) = fp
            yb2(nbb) = fc
            if (nbb.eq.nb) goto 1
         endif
         fp = fc
      enddo
 1    continue
      nb = nbb
      return
      end


********************************************************************
* False position method to find the root of a function
* in a given interval(9.2)
********************************************************************
* ==================================================================
      FUNCTION rtflsp_alt(func,x1,x2,xacc)
* ==================================================================
      INTEGER MAXIT
      DOUBLE PRECISION rtflsp_alt,x1,x2,xacc,func
      EXTERNAL func
      PARAMETER (MAXIT=30)
      INTEGER j
      DOUBLE PRECISION del,dx,f,fh,fl,swap,xh,xl
      fl=func(x1)
      fh=func(x2)
      if(fl*fh.gt.0.d0) print*,
     +     'root must be bracketed in rtflsp_alt'
      if(fl.lt.0.d0)then
        xl=x1
        xh=x2
      else
        xl=x2
        xh=x1
        swap=fl
        fl=fh
        fh=swap
      endif
      dx=xh-xl
      do 11 j=1,MAXIT
        rtflsp_alt=xl+dx*fl/(fl-fh)
        f=func(rtflsp_alt)
        if(f.lt.0.d0) then
          del=xl-rtflsp_alt
          xl=rtflsp_alt
          fl=f
        else
          del=xh-rtflsp_alt
          xh=rtflsp_alt
          fh=f
        endif
        dx=xh-xl
        if(abs(del).lt.xacc.or.f.eq.0.d0)return
 11         continue
      print*,'rtflsp_alt exceed maximum iterations'
      END
C  (C) Copr. 1986-92 Numerical Recipes Software 7L`2.d0


* ======================================================
*      subroutine rzero_alt(x1,x2,root,r,EPS,MAXF,func)
* ======================================================
*      real x1,x2,root,r,EPS,MAXF
*      real func
*      external func
*      real rtflsp_alt
*      external rtflsp_alt
*      root = rtflsp_alt(func,x1,x2,EPS)
*
*      return
*      END






*#####################################################################*
*                                                                     *
*               MODIFIED SUBROUTINES                                  *
*                                                                     *
*#####################################################################*


* ====================================================================
      FUNCTION brentmod1(ax,bx,cx,f,tol,xmin,iter)
* ====================================================================
      implicit none
      INTEGER ITMAX
      REAL brentmod1,ax,bx,cx,tol,xmin,f,CGOLD,ZEPS
      EXTERNAL f
      PARAMETER (ITMAX=100,CGOLD=.3819660,ZEPS=1.0e-10)
      INTEGER iter
      REAL a,b,d,e,etemp,fu,fv,fw,fx,p,q,r,tol1,tol2,u,v,w,x,xm
      a=min(ax,cx)
      b=max(ax,cx)
      v=bx
      w=v
      x=v
      e=0.
      fx=f(x)
      fv=fx
      fw=fx
      do 11 iter=1,ITMAX
        xm=0.5*(a+b)
        tol1=tol*abs(x)+ZEPS
        tol2=2.*tol1
        if(abs(x-xm).le.(tol2-.5*(b-a))) goto 3
        if(abs(e).gt.tol1) then
          r=(x-w)*(fx-fv)
          q=(x-v)*(fx-fw)
          p=(x-v)*q-(x-w)*r
          q=2.*(q-r)
          if(q.gt.0.) p=-p
          q=abs(q)
          etemp=e
          e=d
          if(abs(p).ge.abs(.5*q*etemp).or.p.le.q*(a-x).or.p.ge.q*(b-x))
     *goto 1
          d=p/q
          u=x+d
          if(u-a.lt.tol2 .or. b-u.lt.tol2) d=sign(tol1,xm-x)
          goto 2
        endif
1       if(x.ge.xm) then
          e=a-x
        else
          e=b-x
        endif
        d=CGOLD*e
2       if(abs(d).ge.tol1) then
          u=x+d
        else
          u=x+sign(tol1,d)
        endif
        fu=f(u)
        if(fu.le.fx) then
          if(u.ge.x) then
            a=x
          else
            b=x
          endif
          v=w
          fv=fw
          w=x
          fw=fx
          x=u
          fx=fu
        else
          if(u.lt.x) then
            a=u
          else
            b=u
          endif
          if(fu.le.fw .or. w.eq.x) then
            v=w
            fv=fw
            w=u
            fw=fu
          else if(fu.le.fv .or. v.eq.x .or. v.eq.w) then
            v=u
            fv=fu
          endif
        endif
11    continue
      print*,'brentmod1 exceed maximum iterations'
#ifdef __AMSDEBUG__
      pause ' '
#endif
3     xmin=x
      brentmod1=fx
      return
      END
* ====================================================================
      FUNCTION brentmod2(ax,bx,cx,f,tol,xmin,iter)
* ====================================================================
      implicit none
      INTEGER ITMAX
      REAL brentmod2,ax,bx,cx,tol,xmin,f,CGOLD,ZEPS
      EXTERNAL f
      PARAMETER (ITMAX=100,CGOLD=.3819660,ZEPS=1.0e-10)
      INTEGER iter
      REAL a,b,d,e,etemp,fu,fv,fw,fx,p,q,r,tol1,tol2,u,v,w,x,xm
      a=min(ax,cx)
      b=max(ax,cx)
      v=bx
      w=v
      x=v
      e=0.
      fx=f(x)
      fv=fx
      fw=fx
      do 11 iter=1,ITMAX
        xm=0.5*(a+b)
        tol1=tol*abs(x)+ZEPS
        tol2=2.*tol1
        if(abs(x-xm).le.(tol2-.5*(b-a))) goto 3
        if(abs(e).gt.tol1) then
          r=(x-w)*(fx-fv)
          q=(x-v)*(fx-fw)
          p=(x-v)*q-(x-w)*r
          q=2.*(q-r)
          if(q.gt.0.) p=-p
          q=abs(q)
          etemp=e
          e=d
          if(abs(p).ge.abs(.5*q*etemp).or.p.le.q*(a-x).or.p.ge.q*(b-x))
     *goto 1
          d=p/q
          u=x+d
          if(u-a.lt.tol2 .or. b-u.lt.tol2) d=sign(tol1,xm-x)
          goto 2
        endif
1       if(x.ge.xm) then
          e=a-x
        else
          e=b-x
        endif
        d=CGOLD*e
2       if(abs(d).ge.tol1) then
          u=x+d
        else
          u=x+sign(tol1,d)
        endif
        fu=f(u)
        if(fu.le.fx) then
          if(u.ge.x) then
            a=x
          else
            b=x
          endif
          v=w
          fv=fw
          w=x
          fw=fx
          x=u
          fx=fu
        else
          if(u.lt.x) then
            a=u
          else
            b=u
          endif
          if(fu.le.fw .or. w.eq.x) then
            v=w
            fv=fw
            w=u
            fw=fu
          else if(fu.le.fv .or. v.eq.x .or. v.eq.w) then
            v=u
            fv=fu
          endif
        endif
11    continue
      print*,'brentmod2 exceed maximum iterations'
#ifdef __AMSDEBUG__
      pause ' '
#endif
3     xmin=x
      brentmod2=fx
      return
      END

********************************************************
********************************************************

      FUNCTION goldenmod1(ax,bx,cx,f,tol,xmin,nstep)
      implicit none
      REAL goldenmod1,ax,bx,cx,tol,xmin,f,R,C
      EXTERNAL f
      PARAMETER (R=.61803399,C=1.-R)
      REAL f1,f2,x0,x1,x2,x3
      integer nstep
      nstep=0
      x0=ax
      x3=cx
      if(abs(cx-bx).gt.abs(bx-ax))then
        x1=bx
        x2=bx+C*(cx-bx)
      else
        x2=bx
        x1=bx-C*(bx-ax)
      endif
      f1=f(x1)
      f2=f(x2)
1     if(abs(x3-x0).gt.tol*(abs(x1)+abs(x2)))then
        if(f2.lt.f1)then
          x0=x1
          x1=x2
          x2=R*x1+C*x3
          f1=f2
          f2=f(x2)
        else
          x3=x2
          x2=x1
          x1=R*x2+C*x0
          f2=f1
          f1=f(x1)
        endif
        nstep=nstep+1
      goto 1
      endif
      if(f1.lt.f2)then
        goldenmod1=f1
        xmin=x1
      else
        goldenmod1=f2
        xmin=x2
      endif
      return
      END

********************************************************
********************************************************

      FUNCTION goldenmod2(ax,bx,cx,f,tol,xmin,nstep)
      implicit none
      REAL goldenmod2,ax,bx,cx,tol,xmin,f,R,C
      EXTERNAL f
      PARAMETER (R=.61803399,C=1.-R)
      REAL f1,f2,x0,x1,x2,x3
      integer nstep
      nstep=0
      x0=ax
      x3=cx
      if(abs(cx-bx).gt.abs(bx-ax))then
        x1=bx
        x2=bx+C*(cx-bx)
      else
        x2=bx
        x1=bx-C*(bx-ax)
      endif
      f1=f(x1)
      f2=f(x2)
1     if(abs(x3-x0).gt.tol*(abs(x1)+abs(x2)))then
        if(f2.lt.f1)then
          x0=x1
          x1=x2
          x2=R*x1+C*x3
          f1=f2
          f2=f(x2)
        else
          x3=x2
          x2=x1
          x1=R*x2+C*x0
          f2=f1
          f1=f(x1)
        endif
        nstep=nstep+1
      goto 1
      endif
      if(f1.lt.f2)then
        goldenmod2=f1
        xmin=x1
      else
        goldenmod2=f2
        xmin=x2
      endif
      return
      END





      subroutine minbrackthe(ax,bx,cx,fa,fb,fc,func,nmax,nstep,ierr)
*******************************************************************************
*     this routine makes the bracketing of a minimum of the function
*
*     extracted from NUMERICAL RECIPES in Fortran
*     Chapter. 10: Function Minimization
* -----------------------------------------------------------------------------
* the output bracket is given by ax,bx,cx where ax<bx<cx (or ax>bx>cx )
* and f(ax)>f(bx) and f(cx)>f(bx)
*
*
********************************************************************************
      implicit none
      REAL ax,bx,cx,fa,fb,fc,func,GOLD,GLIMIT,TINY
      EXTERNAL func
      PARAMETER (GOLD=1.618034, GLIMIT=100., TINY=1.e-20)
      REAL dum,fu,q,r,u,ulim
      integer ierr,nstep,nmax
      ierr=0
      nstep=0
      fa=func(ax)
      fb=func(bx)
      if(fb.gt.fa)then
        dum=ax
        ax=bx
        bx=dum
        dum=fb
        fb=fa
        fa=dum
      endif
      cx=bx+GOLD*(bx-ax)
      fc=func(cx)
      nstep=nstep+1
1     if(fb.ge.fc)then
        nstep=nstep+1
        if(nstep.lt.nmax)then
        r=(bx-ax)*(fb-fc)
        q=(bx-cx)*(fb-fa)
        u=bx-((bx-cx)*q-(bx-ax)*r)/(2.*sign(max(abs(q-r),TINY),q-r))
        ulim=bx+GLIMIT*(cx-bx)
        if((bx-u)*(u-cx).gt.0.)then
          fu=func(u)
          if(fu.lt.fc)then
            ax=bx
            fa=fb
            bx=u
            fb=fu
            return
          else if(fu.gt.fb)then
            cx=u
            fc=fu
            return
          endif
          u=cx+GOLD*(cx-bx)
          fu=func(u)
        else if((cx-u)*(u-ulim).gt.0.)then
          fu=func(u)
          if(fu.lt.fc)then
            bx=cx
            cx=u
            u=cx+GOLD*(cx-bx)
            fb=fc
            fc=fu
            fu=func(u)
          endif
        else if((u-ulim)*(ulim-cx).ge.0.)then
          u=ulim
          fu=func(u)
        else
          u=cx+GOLD*(cx-bx)
          fu=func(u)
        endif
        ax=bx
        bx=cx
        cx=u
        fa=fb
        fb=fc
        fc=fu
        else
          ierr=1
          return
        endif
        goto 1
      endif

      return
      END


*==============================================================================

      subroutine minbrackphi(ax,bx,cx,fa,fb,fc,func,nmax,nstep,ierr)
*******************************************************************************
*     this routine makes the bracketing of a minimum of the function...
*
*     extracted from NUMERICAL RECIPES in Fortran
*     Chapter. 10: Function Minimization
*
* -----------------------------------------------------------------------------
*     Here no quadratic interpolation is done
*
*     Borges 23/9/2002
********************************************************************************
      implicit none
      REAL ax,bx,cx,fa,fb,fc,func,GOLD,GLIMIT,TINY
      EXTERNAL func
      PARAMETER (GOLD=1.618034, GLIMIT=100., TINY=1.e-20)
      REAL dum,fu,q,r,u,ulim
      integer ierr,nstep,nmax
      ierr=0
      nstep=0
      fa=func(ax)
      fb=func(bx)
      if(fb.gt.fa)then
        dum=ax
        ax=bx
        bx=dum
        dum=fb
        fb=fa
        fa=dum
      endif
      cx=bx+GOLD*(bx-ax)
      fc=func(cx)
      nstep=nstep+1
1     if(fb.ge.fc)then
        nstep=nstep+1
        if(nstep.lt.nmax)then
          u=cx+GOLD*(cx-bx)
          fu=func(u)
          ax=bx
          bx=cx
          cx=u
          fa=fb
          fb=fc
          fc=fu
        else
          ierr=1
          return
        endif
        goto 1
      endif

      return
      END


* ===============================================================
      subroutine richbetarec_alt(itype)
* ===============================================================
* ***************************************************************
* Velocity reconstruction (LIP rec), alternative method
*
* itype = 1: full scan
* itype = 2: optimized scan
*
* R. Pereira 25-Nov-2011, added to AMS software in December 2011
* ***************************************************************

#include "../include/richrec_lipf.h"
C #include "../include/richrec_lipnew.h"

      integer itype

      real flikefinal

      betarec = -999.

      if(rad_kind.eq.-1) return

      if (nbhits_nass.eq.0) return

      if(itype.ne.1 .and. itype.ne.2) return

*      print*,'Entering RICHBETAREC_ALT, arg ',itype

* --- max number of hits to scan
      nhitscan     = 0 !number of hits to be used on likelihood evaluation

* --- set particle rotation matrix
      if (irotflg.eq.0) then
         call patmatr
         irotflg = 1
      endif

* --- defaults for general parameters
*     (used if values passed in common are negative)

      if(facthcminscanagl.lt.0.) facthcminscanagl = 0.0
      if(facthcmaxscanagl.lt.0.) facthcmaxscanagl = 1.2

      if(facthcminscannaf.lt.0.) facthcminscannaf = 0.0
      if(facthcmaxscannaf.lt.0.) facthcmaxscannaf = 1.2

      if(thcminscanagl.lt.0.) thcminscanagl = 2.*DEGRAD
      if(thcmaxscanagl.lt.0.) thcmaxscanagl = 80.*DEGRAD
      if(thcstepscanagl.lt.0.) thcstepscanagl = 0.5*DEGRAD

      if(thcminscannaf.lt.0.) thcminscannaf = 2.*DEGRAD
      if(thcmaxscannaf.lt.0.) thcmaxscannaf = 80.*DEGRAD
      if(thcstepscannaf.lt.0.) thcstepscannaf = 0.25*DEGRAD

      if(dvmaxagl.lt.0.) dvmaxagl = 3.*RADDEG
      if(dvmaxnaf.lt.0.) dvmaxnaf = 10.*RADDEG

* ... default for linearity deviation:
* ... 2 percent of maximum slope in interval (2 steps)
      if(tollinagl.lt.0.)
     +   tollinagl = 0.01*dvmaxagl*thcstepscanagl  ! cm
      if(tollinnaf.lt.0.)
     +   tollinnaf = 0.01*dvmaxnaf*thcstepscannaf  ! cm

* --- scanning of ring-hit distances at several Cerenkov angles
*     (similar to the one performed in richlikemin
*      but steps are different and hit data are stored)
      if(itype.eq.1) call richscandist
      if(itype.eq.2) call richscandist_opt

* --- get data for distance zeros of each hit
      call richgethitzero

* --- get best likelihood using linearized distances
*     (in principle through fine scan + final quadratic fit)
      call richlikemin_alt

* --- final call to standard likelihood using the previous result
*     to store all hit residuals, etc.

*      print*,'Final result: thcres = ',thcres*RADDEG,' deg'
*      print*,'Final derivative: d2loglike = ',
*     +                          d2loglike*DEGRAD**2,' /deg2'

      flikefinal = fliketotall(thcres)

      if (nbushits_fit.gt.0) then
         ipthetac = 1
         ntherec  = 1
         cangrec(ipthetac)    = thcres
         likerec(ipthetac)    = flikefinal
         cangrecdif(ipthetac) = -999.
         chi2rec(ipthetac)    = chi2rec_fit/real(nbushits_fit)
         nbushits(ipthetac)   = nbushits_fit
         do nn=1,nbushits_fit
            ipushits(ipthetac,nn) = ipushits_fit(nn)
            iflghit(ipushits_fit(nn)) = 2
         enddo
         do nn=1,min(nbhits,nhitmax)
            chi2hit(ipthetac,nn)    = chi2hit_fit(nn)
            nbminshit(ipthetac,nn)  = nbminshit_fit(nn)
            phihit(ipthetac,nn)     = phihit_fit(nn)
            ireflechit(ipthetac,nn) = ireflechit_fit(nn)
            imsechit(ipthetac,nn)   = imsechit_fit(nn)
         enddo
* -----> store beta reconstruction
         betarec = 1./refindex/cos(cangrec(ipthetac))
* -----> store likelihood value
         resvlike = flikefinal
      else
         ipthetac = 0
         ntherec  = 0
         betarec = -999.
         call vfill(cangrec,ntherecmax,-999.)
         call vzero(nbushits,ntherecmax)
         call vfill(cangrec,ntherecmax,-999.)
         resvlike = -999.
      endif

      return
      end


* ===============================================================
      subroutine richscandist
* ===============================================================
* ***************************************************************
* Scanning of ring-hit distances at several Cerenkov angles
* (similar to the one performed in richlikemin
* but steps are different and hit data are stored)
*
* R. Pereira 25-Nov-2011, added to AMS software in December 2011
* ***************************************************************

#include "../include/richrec_lipf.h"
C #include "../include/richrec_lipnew.h"

      real facthcminscan,facthcmaxscan
      real thcminscan,thcmaxscan,thcstepscan

      real cangrecup_alt,cangreclow_alt
      real thcstep_alt

      real fliketot_dummy

      if(refindex.lt.1.1) then  ! aerogel
         facthcminscan = facthcminscanagl
         facthcmaxscan = facthcmaxscanagl
         thcminscan = thcminscanagl
         thcmaxscan = thcmaxscanagl
         thcstepscan = thcstepscanagl
      else  ! NaF
         facthcminscan = facthcminscannaf
         facthcmaxscan = facthcmaxscannaf
         thcminscan = thcminscannaf
         thcmaxscan = thcmaxscannaf
         thcstepscan = thcstepscannaf
      endif

      cangrecup_alt = facthcmaxscan*acos(1./refindex)
      cangreclow_alt = facthcminscan*acos(1./refindex)

      if(thcmaxscan.lt.cangrecup_alt) cangrecup_alt = thcmaxscan
      if(thcminscan.gt.cangreclow_alt) cangreclow_alt = thcminscan

      thcstep_alt = thcstepscan

      IPHIHINTKIND = 1 !phi hint obtained by scanning at thc max
      ipmin = 0
      fliketot_bufmin = 1.E6

* --> init phi hint flag (used in richgetphihint.F)
      IPHIHINTFLAG = 0

* --> scan
      npscan = 0

      do i=1,min(nbhits,nhitmax)
         nthchit_scan(i) = 0
      enddo

      nxthet = int((cangrecup_alt-cangreclow_alt)/thcstep_alt)
      do ixthet = 0,nxthet
         xthet = cangrecup_alt-ixthet*thcstep_alt
*         print*,'Scan in xthet: ',xthet*RADDEG,' deg'
         npscan = npscan+1
         if(npscan.le.npscanmax) then
            fliketot_dummy = fliketotall(xthet)
            do i=1,min(nbhits,nhitmax)
               nthchit_scan(i) = nthchit_scan(i)+1
               thchit_scan(i,npscan) = xthet
               chi2hit_scan(i,npscan) = chi2hit_fit(i)
*               print*,'Hit ',i,', dist = ',sqrt(chi2hit_scan(i,npscan))
               phihit_scan(i,npscan) = phihit_fit(i)
               ireflechit_scan(i,npscan) = ireflechit_fit(i)
               imsechit_scan(i,npscan) = imsechit_fit(i)
            enddo
         endif
         if (IPHIHINTKIND.ne.0) IPHIHINTFLAG = 1 !phi hint just done once
      enddo

*      do i=1,min(nbhits,nhitmax)
*         print*,'Scanning for hit ',i
*         do j=1,nthchit_scan(i)
*            print*,'thc (deg) = ',thchit_scan(i,j)*RADDEG,
*     +             ': d = ',sqrt(chi2hit_scan(i,j))
*         enddo
*      enddo

      return
      end


* ===============================================================
      subroutine richscandist_opt
* ===============================================================
* ***************************************************************
* Optimized version of RICHSCANDIST
*
* R. Pereira 30-Nov-2011, added to AMS software in December 2011
* ***************************************************************

#include "../include/richrec_lipf.h"
C #include "../include/richrec_lipnew.h"

      integer iflghit_bak(nhitmax)

      real facthcminscan,facthcmaxscan
      real thcminscan,thcmaxscan,thcstepscan

      real cangrecup_alt,cangreclow_alt
      real thcstep_alt

      real fliketot_dummy

      integer npnexc
      integer ipexc(npscanmax)

      integer inexc,lnexc
      integer imaxnexc,lmaxnexc

      integer npcalc

      real dvmax
      real dpcalc

      integer ipdone(npscanmax)

*      print*,'entering RICHSCANDIST_OPT'

      if(refindex.lt.1.1) then  ! aerogel
         facthcminscan = facthcminscanagl
         facthcmaxscan = facthcmaxscanagl
         thcminscan = thcminscanagl
         thcmaxscan = thcmaxscanagl
         thcstepscan = thcstepscanagl
      else  ! NaF
         facthcminscan = facthcminscannaf
         facthcmaxscan = facthcmaxscannaf
         thcminscan = thcminscannaf
         thcmaxscan = thcmaxscannaf
         thcstepscan = thcstepscannaf
      endif

      cangrecup_alt = facthcmaxscan*acos(1./refindex)
      cangreclow_alt = facthcminscan*acos(1./refindex)

      if(thcmaxscan.lt.cangrecup_alt) cangrecup_alt = thcmaxscan
      if(thcminscan.gt.cangreclow_alt) cangreclow_alt = thcminscan

      thcstep_alt = thcstepscan

      if(refindex.lt.1.1) then  ! aerogel
         dvmax = dvmaxagl
      else  ! NaF
         dvmax = dvmaxnaf
      endif

      IPHIHINTKIND = 1 !phi hint obtained by scanning at thc max
      ipmin = 0
      fliketot_bufmin = 1.E6

* --> init phi hint flag (used in richgetphihint.F)
      IPHIHINTFLAG = 0

* --> fill test grid angles for each hit
*     (currently each hit uses same grid but this may change)
      npscan = 0
      do i=1,min(nbhits,nhitmax)
         nthchit_scan(i) = 0
      enddo

      nxthet = int((cangrecup_alt-cangreclow_alt)/thcstep_alt)
      do ixthet = 0,nxthet
         xthet = cangrecup_alt-ixthet*thcstep_alt
         npscan = npscan+1
*         print*,'Test angle ',npscan,' at ',xthet*RADDEG,' deg'
         if(npscan.le.npscanmax) then
            do i=1,min(nbhits,nhitmax)
               nthchit_scan(i) = nthchit_scan(i)+1
               thchit_scan(i,npscan) = xthet
               chi2hit_scan(i,npscan) = -999.
            enddo
         endif
      enddo

* --> initial scan at maximum angle
      fliketot_dummy = fliketotall(cangrecup_alt)
      do i=1,min(nbhits,nhitmax)
         chi2hit_scan(i,1) = chi2hit_fit(i)
*         print*,'Hit ',i,',point ',1,', thc = ',
*     +        thchit_scan(i,1)*RADDEG,
*     +        ' deg, d = ',sqrt(chi2hit_scan(i,1))
         phihit_scan(i,1) = phihit_fit(i)
         ireflechit_scan(i,1) = ireflechit_fit(i)
         imsechit_scan(i,1) = imsechit_fit(i)
      enddo
      if (IPHIHINTKIND.ne.0) IPHIHINTFLAG = 1 !phi hint just done once

* --> locate zeros for each hit

* --> replace standard hit flags with mask
      do i=1,min(nbhits,nhitmax)
         iflghit_bak(i) = iflghit(i)
         if(iflghit(i).eq.0) iflghit(i) = 5  ! dummy value to stop calc
      enddo

* --> cycle over hits

      do i=1,min(nbhits,nhitmax)

*         print*,'RICHSCANDIST_OPT: hit ',i,'/',min(nbhits,nhitmax),
*     +        'flag ',iflghit_bak(i)
         if(iflghit_bak(i).gt.0) continue  ! exclude particle/bad hits

* -----> perform chi2 calculation where needed, exclude other regions

         iflghit(i) = 0  ! flag hit to be used in fliketotall
         npnexc = nthchit_scan(i)-1
         ipexc(1) = 1  ! first point treated above
         do j=2,nthchit_scan(i)
            ipexc(j) = 0  ! point not treated/excluded (yet)
         enddo

* -----> exclude points near maximum where possible

         do j=1,nthchit_scan(i)
            if(ipexc(j).eq.0
     +           .and. chi2hit_scan(i,1).lt.1.e6
     +           .and. dvmax*abs(thchit_scan(i,j)
     +           -thchit_scan(i,1))
     +           .lt.sqrt(chi2hit_scan(i,1))) then
*               print*,'Point ',1,' at ',
*     +              thchit_scan(i,1)*RADDEG,'deg',
*     +              ' with d = ',sqrt(chi2hit_scan(i,1)),
*     +              ' excludes point ',j,' at ',
*     +              thchit_scan(i,j)*RADDEG,'deg'
               ipexc(j) = 1     ! exclude neighbour
            endif
         enddo

* -----> cycle of calculations/exclusions until points are exhausted

         do while(npnexc.gt.0)  ! some points not treated/excluded

*            print*,'npnexc = ',npnexc

            npnexc = 0

            imaxnexc = 0  ! first pending point in longest sequence
            lmaxnexc = 0  ! longest pending sequence

            inexc = 0
            lnexc = 0

            do j=1,nthchit_scan(i)
               if(ipexc(j).eq.0) then
                  npnexc = npnexc+1
                  if(lnexc.gt.0) then
                     lnexc = lnexc+1
                  else
                     inexc = j
                     lnexc = 1
                  endif
                  if(lnexc.gt.lmaxnexc) then  ! record length
                     imaxnexc = inexc
                     lmaxnexc = lnexc
                  endif
               else
                  inexc = 0
                  lnexc = 0
               endif
            enddo

            if(npnexc.gt.0) then  ! evaluate next point

               npcalc = imaxnexc+lmaxnexc/2

               fliketot_dummy = fliketotall(thchit_scan(i,npcalc))
               chi2hit_scan(i,npcalc) = chi2hit_fit(i)
*               print*,'Hit ',i,',point ',npcalc,', thc = ',
*     +              thchit_scan(i,npcalc)*RADDEG,
*     +              ' deg, d = ',sqrt(chi2hit_scan(i,npcalc))
               phihit_scan(i,npcalc) = phihit_fit(i)
               ireflechit_scan(i,npcalc) = ireflechit_fit(i)
               imsechit_scan(i,npcalc) = imsechit_fit(i)

               ipexc(npcalc) = 1

               do j=1,nthchit_scan(i)
                  if(ipexc(j).eq.0
     +                 .and. chi2hit_scan(i,npcalc).lt.1.e6
     +                 .and. dvmax*abs(thchit_scan(i,j)
     +                 -thchit_scan(i,npcalc))
     +                 .lt.sqrt(chi2hit_scan(i,npcalc))) then
*                     print*,'Point ',npcalc,' at ',
*     +                    thchit_scan(i,npcalc)*RADDEG,'deg',
*     +                    ' with d = ',sqrt(chi2hit_scan(i,npcalc)),
*     +                    ' excludes point ',j,' at ',
*     +                    thchit_scan(i,j)*RADDEG,'deg'
                     ipexc(j) = 1  ! exclude neighbour
                  endif
               enddo

            endif

         enddo

* -----> perform chi2 calculation for neighbors of possible minima

         do j=1,nthchit_scan(i)
            if(chi2hit_scan(i,j).lt.-1) then  ! distance not calculated
               ipdone(j) = 0
            else
               ipdone(j) = 1
            endif
*            print*,'>>> hit ',i,', point ',j,', done ',ipdone(j)
         enddo

         do j=1,nthchit_scan(i)
            if(ipdone(j).eq.0 .and.
     +           ((ipdone(j-1).eq.1 .and.
     +           sqrt(chi2hit_scan(i,j-1)).lt.dvmax
     +           *abs(thchit_scan(i,j)-thchit_scan(i,j-1)))
     +           .or.(ipdone(j+1).eq.1 .and.
     +           sqrt(chi2hit_scan(i,j+1)).lt.dvmax
     +           *abs(thchit_scan(i,j+1)-thchit_scan(i,j))))) then

*               print*,'>>> hit ',i,', point ',j,', done at final pass'

               fliketot_dummy = fliketotall(thchit_scan(i,j))
               chi2hit_scan(i,j) = chi2hit_fit(i)
               phihit_scan(i,j) = phihit_fit(i)
               ireflechit_scan(i,j) = ireflechit_fit(i)
               imsechit_scan(i,j) = imsechit_fit(i)

            endif
         enddo

         iflghit(i) = 5  ! reset flag

      enddo

* --> restore standard hit flags
      do i=1,min(nbhits,nhitmax)
         iflghit(i) = iflghit_bak(i)
      enddo

*      print*,'leaving RICHSCANDIST_OPT'

      return
      end


* ===============================================================
      subroutine richgethitzero
* ===============================================================
* ***************************************************************
* Get distance zeros of each hit
* using data stored during richscandist
*
* NOTICE THAT THIS ASSUMES EQUAL SPACING IN POINTS!
*
* R. Pereira 25-Nov-2011, added to AMS software in December 2011
* ***************************************************************

#include "../include/richrec_lipf.h"
C #include "../include/richrec_lipnew.h"

      real tollin

      real dcenter,dprev,dnext
      real xcenter,xprev,xnext
      real zeroval,zeroderiv

      if(refindex.lt.1.1) then  ! aerogel
         tollin = tollinagl
      else  ! NaF
         tollin = tollinnaf
      endif

*      print*,'Entering RICHGETHITZERO: nbhits,nhitmax =',
*     +        nbhits,nhitmax

      do i=1,min(nbhits,nhitmax)
*         print*,'Data list for hit ',i
         do j=1,nthchit_scan(i)
            if(chi2hit_scan(i,j).gt.-1.) then
*               print*,'... hit ',i,', point ',j,', d = ',
*     +              sqrt(chi2hit_scan(i,j))
            else
*               print*,'... hit ',i,', point ',j,', d = [N/A]'
            endif
         enddo
      enddo

      do i=1,min(nbhits,nhitmax)

         nchizero(i) = 0

         do j=2,nthchit_scan(i)-1

* ......... Condition to skip excluded regions (points have chi2=-999)
            if(chi2hit_scan(i,j).lt.-1.
     +         .or. chi2hit_scan(i,j-1).lt.-1. 
     +         .or. chi2hit_scan(i,j+1).lt.-1.) continue

            dcenter = sqrt(chi2hit_scan(i,j))
            dprev = sqrt(chi2hit_scan(i,j-1))
            dnext = sqrt(chi2hit_scan(i,j+1))

            xcenter = thchit_scan(i,j)
            xprev = thchit_scan(i,j-1)
            xnext = thchit_scan(i,j+1)

            if((dcenter.lt.dprev).and.(dcenter.lt.dnext)) then  ! local minimum

               if(abs(dcenter-0.5*abs(dprev-dnext)).lt.tollin) then  ! good linearity
                  if(dprev.lt.dnext) then
                     zeroval = xprev
     +                    +(xcenter-xprev)*dprev/(dprev+dcenter)
                     zeroderiv = abs((dprev+dcenter)/(xcenter-xprev))
                  else
                     zeroval = xcenter
     +                    +(xnext-xcenter)*dcenter/(dcenter+dnext)
                     zeroderiv = abs((dcenter+dnext)/(xnext-xcenter))
                  endif

                  if(nchizero(i).lt.nchizeromax) then
                     nchizero(i) = nchizero(i)+1
                     chizero(i,nchizero(i)) = zeroval
                     chideriv(i,nchizero(i)) = zeroderiv

*                     print*,'Hit ',i,': zero at ',
*     +                      zeroval*RADDEG,' deg, deriv = ',
*     +                      zeroderiv*DEGRAD,' cm/deg'

                  endif

               endif
            endif

         enddo
      enddo

      return
      end


* ===============================================================
      subroutine richlikemin_alt
* ===============================================================
* ***************************************************************
* Get best Cerenkov angle with likelihood value using linearized
* distances (through fine scan + final quadratic fit)
*
* Result is stored in thcres
*
* R. Pereira 25-Nov-2011, added to AMS software in December 2011
* ***************************************************************

#include "../include/richrec_lipf.h"
C #include "../include/richrec_lipnew.h"

      real cangrecup_lscan,cangreclow_lscan
      real thcstep_lscan

      integer nlikeval
      real vthe(10000)
      real vlike(10000)

      integer nlikebest
      real vthebest
      real vlikebest

      cangrecup_lscan = acos(1./refindex)*1.20
      cangreclow_lscan = 2.*DEGRAD
      thcstep_lscan = 0.1*DEGRAD

      nlikeval = 0

      nxthet = int((cangrecup_lscan-cangreclow_lscan)/thcstep_lscan)
      do ixthet = 0,nxthet
         xthet = cangreclow_lscan+ixthet*thcstep_lscan
         if(nlikeval.lt.10000) then
            nlikeval = nlikeval+1
            vthe(nlikeval) = xthet
            vlike(nlikeval) = fliketotall_alt(xthet)
*            print*,'At ',xthet*RADDEG,' deg, vlike = ',vlike(nlikeval)
         endif
      enddo

      nlikebest = -999
      vthebest = -999.
      vlikebest = 1.e20

      do i=2,nlikeval-1
         if(vlike(i).lt.vlikebest) then
            nlikebest = i
            vthebest = vthe(i)
            vlikebest = vlike(i)
         endif
      enddo

      thcres = 1.e20
      d2loglike = -1.e20

      if(nlikebest.gt.1) then

         vlikeprev = vlike(nlikebest-1)
         vlikenext = vlike(nlikebest+1)

         thcres = vthebest+(vlikeprev-vlikenext)*thcstep_lscan
     +        /(2.*vlikeprev-4.*vlikebest+2.*vlikenext)

         d2loglike = (vlikeprev-2.*vlikebest+vlikenext)
     +        /thcstep_lscan**2.

*      print*,'Data on final minimization:'
*      print*,'thc points (deg): ',(vthebest-thcstep_lscan)*RADDEG,
*     +                            " ",vthebest*RADDEG," ",
*     +                            (vthebest+thcstep_lscan)*RADDEG
*      print*,'likelihood values: ',vlikeprev," ",
*     +                             vlikebest," ",vlikenext
*      print*,'optimal thc (deg): ',thcres*RADDEG
*      print*,'likelihood 2nd derivative (/deg2): ',
*     +                            d2loglike*DEGRAD**2

      endif

      return
      end


*********************************************************************
      real function fliketotall_alt(cangrecx)
*********************************************************************
*
* AIMS: likelihood function computed as the product of the hits
*       probability to belong to the tested pattern(cangrecx),
*       uses linearized distances instead of fchi2hit
*
*   IN: cangrecx = cerenkov angle
*
* R. Pereira 25-Nov-2011, added to AMS software in December 2011
*
*********************************************************************

#include "../include/richrec_lipf.h"
C #include "../include/richrec_lipnew.h"

      real chi2hit_lin
      integer nbushits_alt
      real chi2rec_alt

      real gcte, fgaussct, gcte1, gcte2, fact_weight

* --- init
      fliketotall_alt  = 0.0
      nbushits_alt = 0
      chi2rec_alt  = 0.0

* --- pass variable
      hypthc = cangrecx
      if (hypthc.eq.0.0) hypthc = 0.001

* --- exception conditions
      if (nbhits.eq.0          .or.
     &    nbhits_nass.eq.0     .or.
     &    hypthc.lt.cangreclow .or.
CC     &    hypthc.gt.cangrecup
     &    hypthc.gt.cangrecup*13./11.
     &    ) then
         fliketotall = 1.E8
         return
      endif

* --- how many hits to use on likelihood computation
      if (nhitscan.eq.0) nhitscan = nbhits_nass
      istephit = int(nbhits_nass/nhitscan)
      if (istephit.eq.0) istephit = 1

* --- loop on data hits non-associated to particle
      do 1 n=1,min(nbhits_nass,nhitscan*istephit),istephit
* ...... get pointer to data hits (non-assoc)
         i = ip_nass(n)
* ...... exclude associated hits to track (dummy!!!)
         if (iflghit(i).eq.1) goto 1 !next hit
* ...... exclude hits already used by some reconstruction
         if (iflghit(i).eq.2) goto 1 !next hit
* ...... exclude hits farther than 10 cm for maximum cerenkov pattern
         if (iflghit(i).eq.3) goto 1 !next hit
* ...... exclude any other anomalous hits
         if (iflghit(i).gt.0) goto 1 !next hit

         chi2hit_lin = flindist(cangrecx,i)**2.

* ----> DOUBLE GAUSSIAN
         gcte1   = F1NORMG/sqrt(twopi*F1SIGMA2)
         gcte2   = (1.-F1NORMG)/sqrt(twopi*F2SIGMA2)
         fgaussct = -log(
     +                   (1.-FBACKGR)*(
     +                   gcte1*exp(-0.5*chi2hit_lin/F1SIGMA2)+
     +                   gcte2*exp(-0.5*chi2hit_lin/F2SIGMA2))+
     +                   FBACKGR/FDISTAN
     +                  )
* ----> weight probability with signal
         if (hitsnpe(i).lt.1.)then
            fact_weight=1.
         else
cc            fact_weight=nint(hitsnpe(i))
            fact_weight=hitsnpe(i)
         endif
         fgaussct = fgaussct*fact_weight
* -----> Compute likelihood function
         fliketotall_alt = fliketotall_alt + fgaussct
* ...... tag hits associated to ring
         if (chi2hit_lin.lt.CHI2HTCUT) then
            nbushits_alt = nbushits_alt + 1
            chi2rec_alt = chi2rec_alt + chi2hit_lin
         endif

* --- loop on hits
 1    continue

* ... no likelihood was calculated
      if (fliketotall.eq.0..and.nbushits_alt.eq.0) then
         fliketotall = 1.E8
         return
      endif

      return
      end


*********************************************************************
      real function flindist(cangrecx,j)
*********************************************************************
*
* AIMS: result of linearized distance from hit 'j' to pattern
*       for Cerenkov angle cangrecx
*
*   IN: cangrecx = Cerenkov angle
*       ihit = hit number
*
* R. Pereira 25-Nov-2011, added to AMS software in December 2011
*
*********************************************************************

#include "../include/richrec_lipf.h"
C #include "../include/richrec_lipnew.h"

      real dist

      flindist = 1000.  ! equivalent to infinity

      do i=1,nchizero(j)  ! cycle over zeros
         dist = abs(cangrecx-chizero(j,i))*chideriv(j,i)
         if(dist.lt.flindist) then
            flindist = dist
         endif
      enddo

      return
      end
