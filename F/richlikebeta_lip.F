* $Id: richlikebeta_lip.F,v 1.1 2003/12/17 13:00:00 mdelgado Exp $
* +LIP  velocity reconstruction 

       subroutine richlikebeta_lip(iproc,ireflec)
*=============================================================
* beta reconstruction steering subroutine                      
*=============================================================
      implicit none

*  DECLARATIONS 
* ==============
#include "../include/richrec_lipf.h"

      integer iproc,ireflec

* .. functions       
      real fliketot_phimat,fliketot_phihint, brentmod1
      external fliketot_phimat,fliketot_phihint,brentmod1
* ..  local variables 
      integer i,j,nn,iter
      real cangrectol
      real fliketot_buf, thetac_buf , xthet, testval , thetac_hint
      real thetbrack1, thetbrack2, thetbrack3, cangrecmat,cangrecmin,
     & fsigma2hint,chi2_evt,prob_evt
     
      REAL TBETA1,TBETA2,TBETA3,TBETA4,TBETA5


*  INITIALIZATION 
* ================

CC      CALL TIMED(TBETA1)

* ..  reconstruction variables and parameters (angles in radians): 
      call vfill(cangrec,ntherecmax,-999.)
      ipthetac = 0
      ntherec=0       
      chi2rec_fit=999.
      nbushits_fit=0
      ireflec=0  
      cangrectol = degrad*0.01 
      if (chradid.eq.'AGL') then
         cangrecup  = acos(1./refindex)*1.05
         cangreclow = 5.0*DEGRAD  ! minimum beta=0.9746(n=1.03)/0.956(n=1.05) 
         PHISTEP    = 10.0        ! phi step for hit chi2 scanning 
         THCSTEP    = 1.0         ! thetac step for Likelihood F. scanning 
         FSIGMA2    = 0.3**2      ! likelihood function width 
         FBACKGR    = 0.2         ! background fraction
      elseif (chradid.eq.'NAF' .or. chradid.eq.'PGL') then
         cangrecup  = acos(1./refindex)*1.05
         cangreclow = acos(1./(refindex*0.97)) ! minimum beta=0.97
         PHISTEP   =  5.     
         THCSTEP    = 1.0         ! thetac step for Likelihood F. scanning 
         FSIGMA2   =  0.7**2 
         FBACKGR   =  0.1    
      endif
      FDISTAN   =  100.                ! detector matrix dimension
      CHI2HTCUT=  (4*sqrt(fsigma2))**2 ! chi2cut for a photon pattern hit

      if (iproc.lt.0) return 

* //////////////////////////////////////////////////////////////
* ==> RECONSTRUCT CERENKOV ANGLE 
* //////////////////////////////////////////////////////////////

CC      CALL TIMED(TBETA2)

* => find minimal solution analitically => phi(hit), best thc

      fliketot_buf = 9999.
      thetac_buf=0.        
      do xthet=cangrecup,cangreclow,-THCSTEP*DEGRAD
          testval = fliketot_phimat(xthet)
          if (testval.lt.fliketot_buf) then
            fliketot_buf = testval
            thetac_buf   = xthet
          endif
CC          print*,'thetac.flike.buf',raddeg*xthet,testval,fliketot_buf   
      enddo   
 
      if(thetac_buf.eq.0) return

CC      CALL TIMED(TBETA3)

* => find minimum numerically => phifit(hit), better thc
      xthet=thetac_buf
      testval = fliketot_phihint(xthet)
      if (testval.lt.fliketot_buf) then
          fliketot_buf = testval
          thetac_buf   = xthet
      endif

CC      CALL TIMED(TBETA4)

* => THC final minimization

      thetac_hint   = thetac_buf  
      thetbrack2    = thetac_buf

      if(chradid.eq.'AGL') then 
        thetbrack1 = thetac_hint-2.*THCSTEP*DEGRAD
        thetbrack3 = thetac_hint+2.*THCSTEP*DEGRAD
      else
        thetbrack1 = thetac_hint-10.*THCSTEP*DEGRAD
        thetbrack3 = thetac_hint+10.*THCSTEP*DEGRAD
      endif

      chi2_evt = brentmod1(thetbrack1,thetbrack2,thetbrack3,               
     +fliketot_phihint,cangrectol,cangrecmin,iter)    
      prob_evt = fliketot_phihint(cangrecmin)

CC      print*,'final >>>>>>>>> thetac.flike.buf',raddeg*cangrecmin,
CC     & prob_evt

CC      CALL TIMED(TBETA5)
 1    continue 

CC      PRINT *,'T.Start           ',TBETA1,TBETA2
CC      PRINT *,'Phi Mat, T.       ',thetac_buf,TBETA3
CC      PRINT *,'Phi Hint, T.      ',thetac_hint,TBETA4
CC      PRINT *,'Thetac brent 2, T.',cangrecmin, TBETA5

* ... store reconstructed values       

      if (nbushits_fit.gt.0) then
         ipthetac = 1
         ntherec  = 1
         cangrec(ipthetac)  = cangrecmin
         likerec(ipthetac)  = prob_evt 
         chi2rec(ipthetac)  = chi2rec_fit
         nbushits(ipthetac) = nbushits_fit
         do nn=1,nbushits_fit
            ipushits(ipthetac,nn) = ipushits_fit(nn)
            iflghit(ipushits_fit(nn)) = 2
         enddo                              
         do nn=1,nbhits
            chi2hit(ipthetac,nn)    = chi2hit_fit(nn)
            nbminshit(ipthetac,nn)  = nbminshit_fit(nn)
            phihit(ipthetac,nn)     = phihit_fit(nn)
            ireflechit(ipthetac,nn) = ireflechit_fit(nn)
            if(ireflechit_fit(nn).gt.0) ireflec=ireflec+1 
         enddo 


         if (ipthetac.ne.0) then
            betarec = 1./refindex/cos(cangrec(ipthetac))
         else
            betarec = -999.
         endif

      endif

      return
      end

      real function fliketot_phimat(cangrecx)
*********************************************************************
* DATE: 25/July/2002
* calculates Likelihood function 
* chi2, and phi(hits) are evaluated through an analytic method
********************************************************************* 
       implicit none 

*  DECLARATIONS 
* ==============
         
#include "../include/richrec_lipf.h"

      real cangrecx
        
* ..  local variables

      integer i,nbhits_near,nmaxiter
      real phitol

      real Hpar,Apar,Bpar,XMC
      real x0,y0,dxdet,dydet
    
      real residual_max
      parameter(residual_max=20.)    

      real gcte, fgaussct 

      double precision chi2min,phih  
 

* .. functions

      double precision rootmin
      external rootmin

*  INITIALIZATION 
* ================
 
      fliketot_phimat = 0.0
      nbushits_fit = 0
      chi2rec_fit = 0.0
      phih=0.0
      hypthc = cangrecx
      if (hypthc.eq.0.0) hypthc = 0.001
      phitol = 0.01*degrad
      nmaxiter = 25

*  likelihood function
* =====================

* --- exception conditions

      if (nbhits.eq.0          .or.
     &    hypthc.lt.cangreclow .or.
     &    hypthc.gt.cangrecup   
     &    ) then
         fliketot_phimat = 1.E+8
         goto 99
      endif

      Hpar=(pcoopmt(3)-pcervtx(3))
      Apar=Hpar/2.*(tan(pthe+cangrecx)-tan(pthe-cangrecx))
      XMC =Hpar/2.*
     +    (tan(pthe+cangrecx)-2*tan(pthe)+tan(pthe-cangrecx))
      Bpar=Hpar*tan(cangrecx)/cos(pthe)
      Bpar=Bpar*Apar/sqrt(Apar**2-XMC**2)

* --- loop on data hits

      nbhits_near=nbhits 

      do 1 i=1,nbhits

       chi2hit_fit(i)   =999.
       phihit_fit(i)    =0.
       ipushits_fit(i)  =0
       nbminshit_fit(i) =0
       ireflechit_fit(i)=0   

       if (iflghit(i).ne.0) then
*         exclude hits associated to track (1)
*         exclude hits already used by some reconstruction (2)
*         exclude very distant hits from maximum cerenkov pattern or clusters(3)
            chi2hit_fit(i) = 999999.
            goto 1
       endif

       x0=(pcoopmt(1)+XMC*cos(pphi))
       y0=(pcoopmt(2)+XMC*sin(pphi))            

       dxdet = hitscoo(1,i)
       dydet = hitscoo(2,i)

       chi2min= 
     & rootmin(dble(Hpar),dble(Apar),dble(Bpar),dble(XMC),
     & dble(dxdet),dble(dydet),dble(cangrecx),phih)

       phihit_fit(i) = real(phih)
       chi2hit_fit(i)= real(chi2min**2)  

* ...... likelihood function (Lorentz function/Gauss+constant) for the hit

       gcte   = 1./sqrt(twopi*fsigma2)
       fgaussct = -log( 
     +                   (1.-FBACKGR)*gcte*
     +                   exp(-0.5*chi2hit_fit(i)/FSIGMA2)+
     +                   FBACKGR/FDISTAN
     +                  )
       fliketot_phimat = fliketot_phimat + fgaussct

* ...... nb hits within a chi2 cut

       if (chi2hit_fit(i).lt.CHI2HTCUT ) then
         nbushits_fit               = nbushits_fit + 1 
         ipushits_fit(nbushits_fit) = i
         chi2rec_fit                = chi2rec_fit + chi2hit_fit(i)   
       endif 

 1    continue

      if(fliketot_phimat.eq.0..and.nbushits_fit.eq.0)then
        fliketot_phimat = 1.E+8        
      endif

 99   continue

      return
      end

*************************************************************
      double precision function rootmin(H,A,B,XMC,xdet,ydet,
     & thc,phiroot)
*************************************************************
       implicit none

*  DECLARATIONS
* ==============
*     GEOMETRY block
*     -------------- 
       real ztoprad_ams,zpmtdet,
     + hmir,rtmir,rbmir,emcxlim,emcylim,
     + ztmirgap,zbmirgap,                                  ! dimensions
     + reflec, 
     + lg_length,lg_height,                                 ! LG
     + pmt_suptk,pmt_shdtk,pmt_sidel


      common /lipgeo/ztoprad_ams,       ! top of radiator (AMS frame) (cm)
     +               zpmtdet,           ! z of PMT matrix (RICH frame)
     +               hmir,              ! mirror height (cm)
     +               rtmir,             ! mirror top radius (cm)
     +               rbmir,             !   "   bottom "    (cm)
     +               emcxlim,           ! hole lim. x    
     +               emcylim,           ! hole lim. y    
     +               ztmirgap,          ! zgap betw. mirror and rad
     +               zbmirgap,          ! zgap betw. mirror and lg
     +               reflec,            ! mirror reflectivity
     +               lg_length,         ! LG length (cm) 
     +               lg_height,         ! LG height(cm) 
     +               pmt_suptk,
     +               pmt_shdtk,
     +               pmt_sidel

       integer radtype,nabsrad
       real hrad,refindex,clarity,rrad,ltile,labsrad,     ! radiator
     + hpgl,pglix                                         ! foil

       common/liprad/ 
     +               hrad,              ! radiator thickness          (cm) 
     +               refindex,          !   "      refractive index
     +               clarity,           !   "      clarity
     +               radtype,           ! 0(empty),1(agl),2(naf)
     +               rrad,              ! radiator radius
     +               ltile,             ! tile dimensions
     +               nabsrad,           ! Nelm( absorption length)
     +               labsrad(44),       ! absorption length                       
     +               hpgl,              ! foil thickness(cm)
     +               pglix              !  "   refractive index 


*     PARTICLE BLOCK       
*     ---------------
      real pimp,pmom,pthe,pphi,pcoopmt,cerang,pbeta,pchg
      common /liptrk/pimp(3),           ! particle ip in radiator (AMS frame)
     +               pmom,
     +               pthe,
     +               pphi,
     +               pcoopmt(3),
     +               cerang,
     +               pbeta,
     +               pchg 

        real pcervtx
        common /richvtxc/ pcervtx(3)  ! cerenkov vertex
        real vtmir, tgmir
        common/miraux/vtmir(3), 
     +              tgmir    

*     CONSTANTS
*     ---------
      double precision PI,TWOPI,DEGRAD,RADDEG 
      PARAMETER (PI=3.14159265358979324)      
      PARAMETER (TWOPI=6.28318530717958648)
      PARAMETER (DEGRAD=0.0174532925199432958)
      PARAMETER (RADDEG=57.2957795130823209)

*     LOCAL variables
*     ----------------
      integer i,ir      

      double precision dbpthe, dbpphi  

      double precision H,A,B,XMC,xdet,ydet,thc
      double precision x0m,y0m,x0,y0,xp,yp,xpd,ypd

      double precision Ap,Bp,C,T,Sa,S0,S2,r3,r4
      double precision ct1,ct2,ct3,ct4 

      double precision xe(8), ye(8),xed,yed,dxed,dyed,phiel,phih

      double precision Hr,Hgamma 
      double precision Hg, xg,yg,zg,xgr,ygr,zgr,theroot,tggam,phpg
      double precision stpgl,thpgl,tgpgl,stsnell,tsnell,tggams,xgm,ygm

      double precision phiroot,xeds,yeds,rpds,rpemin,chi2min 
 
*
*  INITIALIZATION 
* ================

        rootmin=999.D0
        phiroot=0.d0

        rpemin=999.d0
        chi2min=999.d0

        ct1=999.d0
        ct2=999.d0
        ct3=999.d0
        ct4=999.d0

        call vzero(xe,8)
        call vzero(ye,8)

        dbpthe=dble(pthe)
        dbpphi=dble(pphi)

*  find CHI2 and PHI 
* ===================

        Hgamma=dble(hpgl+ztmirgap+hmir+zbmirgap)
        Hr=H-hgamma

* .. elipse center coordinates in pmt matrix

        x0m=dble(pcoopmt(1))
        y0m=dble(pcoopmt(2))
        x0=x0m+XMC*dcos(dbpphi)
        y0=y0m+XMC*dsin(dbpphi)
        
* .. hit coordinates wrt elipse center:

        xp = xdet-x0
        yp = ydet-y0            

* .. hit coordinates in elipse frame of ref.

        xpd= xp*dcos(dbpphi)+yp*dsin(dbpphi)
        ypd=-xp*dsin(dbpphi)+yp*dcos(dbpphi)

* .. FIND ROOTS 

        Ap=xpd*A/(B**2-A**2)
        Bp=ypd*B/(B**2-A**2)
        C=2.D0**(1.d0/3.d0)

* .. Elipse roots (See file roots-new.nb )

        T  =  (-1.D0+Ap**2+Bp**2)

        Sa= 108.d0*Ap**2*( 1.D0-Ap**2 + T) + 2.d0*T**3

*        print*,'>>>>>>>>>> ',-4*T**6+Sa**2
        if(-4.D0*T**6 + Sa**2.lt.0) then
*         print*,'>>>>>>>>>> sqrt of negative nr in S0'
         return
        endif
 
        S0=Sa+dsqrt(-4.D0*T**6+Sa**2)
        S0=S0**(1.D0/3.D0) 

        S2  =  Ap**2 + (S0/C - 2*T + C*T**2/S0 )/3.D0

        if(S2.lt.0) then
*         print*,'>>>>>>>>>> sqrt of negative nr in S2'
         return
        endif

        S2  = dsqrt(S2)

* -------------------------------------------------------

        r3=2.D0*Ap**2 - ( S0/C + 4.D0*T +  C*T**2/S0 )/3.D0  

        if(S2.eq.0) then
*         print*,'>>>>>>>>>> divide by 0 in r4'
         return
        endif

        r4=2.D0*Ap*(2.D0 - Ap**2 + T)/S2

* --------------------------------------------------------

        ir=0 

        if(r3-r4.ge.0) then

         ct1=(-Ap-S2-dsqrt(r3-r4))/2.D0
         if(abs(ct1).le.1) then 
          ir=ir+2
          xe(ir-1)= A*ct1
          xe(ir)= xe(ir-1)
          ye(ir-1)= B*dsqrt(1.D0-ct1**2)
          ye(ir)=-ye(ir-1)
         endif
         ct2=(-Ap-S2+dsqrt(r3-r4))/2.D0
         if(abs(ct2).le.1) then 
          ir=ir+2
          xe(ir-1)= A*ct2
          xe(ir)  = xe(ir-1)
          ye(ir-1)= B*dsqrt(1.D0-ct2**2)
          ye(ir)  =-ye(ir-1)
         endif

        endif
        if(r3+r4.ge.0) then         

         ct3=(-Ap+S2-dsqrt(r3+r4))/2.D0
         if(abs(ct3).le.1) then 
          ir=ir+2
          xe(ir-1)= A*ct3
          xe(ir)  = xe(ir-1)
          ye(ir-1)= B*dsqrt(1.D0-ct3**2)
          ye(ir)  =-ye(ir-1)
         endif
         ct4=(-Ap+S2+dsqrt(r3+r4))/2.D0
         if(abs(ct4).le.1) then 
          ir=ir+2
          xe(ir-1)= A*ct4
          xe(ir)  = xe(ir-1)
          ye(ir-1)= B*dsqrt(1.D0-ct4**2)
          ye(ir)  =-ye(ir-1)
         endif

        endif

        do 10 i=1,ir

* elipse coord. in detector wrt elipse center

         xed   = xe(i)*dcos(dbpphi)-ye(i)*dsin(dbpphi)
         yed   = xe(i)*dsin(dbpphi)+ye(i)*dcos(dbpphi)

* elipse coord. in detector wrt to particle impact point

         dxed=xed+XMC*dcos(dbpphi)
         dyed=yed+XMC*dsin(dbpphi)

* photon phi wrt to particle

* neste referencial:  Z aponta para baixo  

        phih=datan2(dsin(dbpphi)*dxed-dcos(dbpphi)*dyed,
     &  -(dcos(dbpphi)*dxed+dsin(dbpphi)*dyed)*dcos(dbpthe))

*         phih=atan2(ye(i),(xe(i)+XMC)*cos(pthe))

         if(phih.lt.0.) phih=phih+twopi
         if(phih.gt.twopi) phih=phih-twopi

* .. REFRACTION 

* cone (wrt vertex) : xg aligned with pphi 
*(different referential from PATRACE1)


        Hg=Hr/dcos(dbpthe)
        xg=Hg*dtan(thc)*dcos(phih)
        yg=Hg*dtan(thc)*dsin(phih)
        zg=-Hg

* passing to radiator plane 

        xgr=-xg*dcos(dbpthe)-zg*dsin(dbpthe)
        ygr= yg
        zgr= xg*dsin(dbpthe)-zg*dcos(dbpthe)

* angle between photon and rad plane:

        theroot=datan2(sqrt(xgr**2+ygr**2),zgr)
        tggam=dsqrt(xgr**2+ygr**2)/(zgr)

* angle between photon plane and particle plane:

        phpg=datan2(ygr,xgr) 
        if(phpg.lt.0)phpg=phpg+twopi 

* snell
        stpgl=dble(refindex)*dsin(theroot)/dble(pglix)
        if(dabs(stpgl).gt.1.) goto 10
        thpgl=dasin(stpgl)
        tgpgl=dtan(thpgl)        

        stsnell=dble(pglix)*dsin(thpgl)
        if(dabs(stsnell).gt.1.) goto 10
        tsnell=dasin(stsnell)
        tggams=dtan(tsnell)        

* propagate to matrix

        xgm=
     & (Hr*tggam+dble(hpgl)*tgpgl+(hgamma-dble(hpgl))*tggams)*dcos(phpg)
        ygm=
     & (Hr*tggam+dble(hpgl)*tgpgl+(hgamma-dble(hpgl))*tggams)*dsin(phpg)

* rotate to detector frame and add vertex coordinates:

        xeds=xgm*dcos(dbpphi)-ygm*dsin(dbpphi) + dble(pcervtx(1))
        yeds=xgm*dsin(dbpphi)+ygm*dcos(dbpphi) + dble(pcervtx(2))
*
        rpds=(xeds-xdet)**2+(yeds-ydet)**2 
        rpds=dsqrt(rpds)        
         if(rpds.lt.rpemin) then
          phiroot=phih
          rpemin=rpds 
         endif 
 10   continue

      rootmin=rpemin 
 
      return
      end

      real function fliketot_phihint(cangrecx)
*********************************************************************
* last upDATE: 03/12/2003
*
* calculates Likelihood function <- chi2
*
*   a numerical method is used 
*   starting from the (THC,PHI(hits)) obtained previously
******************************************************************** 
      implicit none 

*  DECLARATIONS
* ==============
#include "../include/richrec_lipf.h"
     
      real cangrecx      

* .. local variables

      integer ievbuff,itkbuff   
      data ievbuff,itkbuff /0,0/
      save ievbuff,itkbuff

      real dxdet,dydet,dzdet,dxpar,dypar 
      integer i,nmaxiter,nsteps,ierr,iter

      real phibrack1, phibrack2, phibrack3, f1,f2,f3,
     &     funcphi1,funcphi2,funcphi3

      real phitol, phphimin, chi2_hit

      real residual_max 
      parameter(residual_max=20)    

      real gcte, fgaussct 

      integer nbhits_near 
      save nbhits_near

      real phihint(nhitmax)  
      save phihint

* ..  functions
      real brentmod2, fchi2hit
      external brentmod2, fchi2hit
 
*
*  INITIALIZATION
* ================

      fliketot_phihint = 0.0
      nbushits_fit = 0

* --- pass variable
      hypthc = cangrecx
      if (hypthc.eq.0.0) hypthc = 0.001

* ... tolerance for determination minimum
      phitol = 0.01*degrad
      nmaxiter = 25
* ... chi2
      chi2rec_fit = 0.0


*  find likelihood
* =================

      if (nbhits.eq.0          .or.
     &    hypthc.lt.cangreclow .or.
     &    hypthc.gt.cangrecup   
     &    ) then
         fliketot_phihint = 1.E+8
         goto 99
      endif

      do 1 i=1,nbhits
       if (iflghit(i).ne.0) then
*           exclude associated hits to track (1)
*           exclude hits already used by some reconstruction (2)
*           exclude very distant hits from maximum cerenkov pattern (3)
            chi2hit_fit(i) = 999999.
            goto 1
       endif

* ...... current hit              

         ihit = i                 
         curhit(1) = hitscoo(1,i) 
         curhit(2) = hitscoo(2,i) 
         curhit(3) = hitscoo(3,i)
         
*        1ST CALL to fliketot_phihint ...
         IF (ievnumb.ne.ievbuff.or.itknumb.ne.itkbuff) THEN
*--------------------------------------------------------------
* CONVERSION TO PATRACE1 FRAME OF REFERENCE (USED IN FIT): 
* Z points up,  x is the same , y changes sign 
*--------------------------------------------------------------
           phihit_fit(i)=twopi-phihit_fit(i) 
           if(phihit_fit(i).gt.twopi) phihit_fit(i)=phihit_fit(i)-twopi 
           if(phihit_fit(i).lt.0)     phihit_fit(i)=phihit_fit(i)+twopi 
         ENDIF ! END OF 1ST CALL !      
         phihint(i)=phihit_fit(i)        

* ..     build bracket by hand

         phibrack1 = phihint(i)- 20.*DEGRAD
         phibrack2 = phihint(i)
         phibrack3 = phihint(i)+ 20.*DEGRAD
         f1        = fchi2hit(phibrack1)
         f2        = fchi2hit(phibrack2)
         f3        = fchi2hit(phibrack3)
         if((f1-f2).lt.0..or.(f2-f3).gt.0)then

* ..     function bracketing 

         phibrack1 = phihint(i)-0.5*DEGRAD
         phibrack2 = phibrack1 +1.0*DEGRAD           
         call minbrackphi(phibrack1,phibrack2,phibrack3,
     &                    funcphi1,funcphi2,funcphi3,
     &                    fchi2hit,nmaxiter,nsteps,ierr)        
           if (ierr.ne.0)goto 1
         endif

         if(fchi2hit(phibrack2).gt.1.e7)goto 1

* ..     find the minimal phi of the hit 

         chi2_hit = brentmod2(phibrack1,phibrack2,phibrack3,
     + fchi2hit,phitol,phphimin,iter)
 
         if (int(phphimin/twopi).gt.0)
     & phphimin = phphimin - int(phphimin/twopi)*twopi
         if(phphimin.lt.0.)
     & phphimin = phphimin + (int(phphimin/twopi)+1)*twopi
*
       phihit_fit(i)     = phphimin                  
       chi2hit_fit(i)    = chi2_hit
       ireflechit_fit(i) = nmirefhit

* .. LIKELIHOOD function (Lorentz function/Gauss+constant) for the hit

       gcte   = 1./sqrt(twopi*fsigma2)

       fgaussct = -log( 
     +                   (1.-FBACKGR)*gcte*
     +                   exp(-0.5*chi2hit_fit(i)/FSIGMA2)+
     +                   FBACKGR/FDISTAN
     +                  )

       fliketot_phihint = fliketot_phihint + fgaussct

* .. nb hits within a chi2 cut

       if (chi2hit_fit(i).lt.CHI2HTCUT) then
             nbushits_fit               = nbushits_fit + 1 
             ipushits_fit(nbushits_fit) = i
             chi2rec_fit                = chi2rec_fit + chi2hit_fit(i)   
       endif 
 1    continue

      if(fliketot_phihint.eq.0..and.nbushits_fit.eq.0)then
        fliketot_phihint = 1.E+8        
      endif

      if (ievnumb.ne.ievbuff)then
         ievbuff = ievnumb
      endif
      if (itkbuff.ne.itknumb) then
         itkbuff = itknumb
      endif

 99   continue

      return
      end


* ////////////////////////////////////////////////////////////

*=======================================================
      real function fchi2hit(phphi)
*=======================================================
      implicit none 
*----------------
* DECLARATIONS
*----------------
#include "../include/richrec_lipf.h"
      real phphi 
* .. local 
      integer nerr,nmiref 

      integer nermax  
      parameter(nermax=10)

      integer verr(nermax)

      real cangrecx,xdet,ydet

*----------------
* INITIAIZATION
*----------------

      fchi2hit = 0.
      nerr=0
      cangrecx = hypthc

*------------------
* .. GO ...
*------------------
      CALL PATRACE1(phphi,cangrecx,xdet,ydet,nmiref,nerr,verr) 
*      print*,'>>>>>>>>>>>>>>',nerr,verr(nerr)  

      nmirefhit = nmiref

      if (nerr.ne.0) then
         fchi2hit = 1.E+8
         return
      endif

      fchi2hit = (curhit(1)-xdet)**2 + (curhit(2)-ydet)**2

      return
      end


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
*        -> Numerical Derivative by Ridders method 
*                   dfridr 
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
      pause 'brent exceed maximum iterations'
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
      pause 'brent exceed maximum iterations'
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
* numerical derivative by Ridders' method (5.7)
********************************************************************
* ==================================================================
      FUNCTION dfridr(func,x,h,err)
* ==================================================================
      implicit none
      INTEGER NTAB
      REAL dfridr,err,h,x,func,CON,CON2,BIG,SAFE
      PARAMETER (CON=1.4,CON2=CON*CON,BIG=1.E30,NTAB=10,SAFE=2.)
      EXTERNAL func
CU    USES func
      INTEGER i,j
      REAL errt,fac,hh,a(NTAB,NTAB)
      if(h.eq.0.) pause 'h must be nonzero in dfridr'
      hh=h
      a(1,1)=(func(x+hh)-func(x-hh))/(2.0*hh)
      err=BIG
      do 12 i=2,NTAB
        hh=hh/CON
        a(1,i)=(func(x+hh)-func(x-hh))/(2.0*hh)
        fac=CON2
        do 11 j=2,i
          a(j,i)=(a(j-1,i)*fac-a(j-1,i-1))/(fac-1.)
          fac=CON2*fac
          errt=max(abs(a(j,i)-a(j-1,i)),abs(a(j,i)-a(j-1,i-1)))
          if (errt.le.err) then
            err=errt
            dfridr=a(j,i)
          endif
11      continue
        if(abs(a(i,i)-a(i-1,i-1)).ge.SAFE*err)return
12    continue
      return
      END


*#####################################################################*
*                                                                     *
*               MODIFIED SUBROUTINES                                  *
*                                                                     *
*#####################################################################*


* ====================================================================
      real function brentmod1(ax,bx,cx,f,tol,xmin,iter)                      
* ====================================================================         
      implicit none
      INTEGER ITMAX
*      REAL brentmod1
      REAL ax,bx,cx,tol,xmin,f,CGOLD,ZEPS
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
      pause 'brentmod1 exceed maximum iterations'
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
      pause 'brentmod2 exceed maximum iterations'
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


















