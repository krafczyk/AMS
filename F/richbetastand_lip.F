***************************************************************************************
* In this file
* ------------
*      subroutine richtoftrackinit(itype)  //steering routine for tof track hints ("init" means hint tracks)
*      subroutine richtoftrackinit1(itype)
*      subroutine richtoftrackinit2(itype)
*      subroutine richtoftrackinit3(itype)
*      subroutine richtoftrackinit4(itype)
*      subroutine richrecflex
*      subroutine richrecstand
*      subroutine richinitstand(irshint,minsigquo,ierr)
*      subroutine richlikemin5parxypart(thetac,x0m,y0m,x0v,y0v,errhint,vlike,ierr)
*      REAL FUNCTION FLIKETOT5PARXYPART(VPAR)
*      SUBROUTINE amoeba(p,y,mp,np,ndim,ftol,funk,iter,ierr)
*      FUNCTION amotry(p,y,psum,mp,np,ndim,funk,ihi,fac)
*
****************************************************************************************


***********************************************
*** CODE FOR:                               ***
***   - Reconstruction using TOF+RICH data  ***
***   - Reconstruction using RICH data only ***
***********************************************


**********************************************************
*** RICHGETMATRIXHINT: get track hint from RICH matrix ***
**********************************************************

      subroutine richgetmatrixhint(strpmtx,strpmty,strpmtnpe)

#include "../include/richrec_lipf.h"
#include "../include/richrec_lipc.h"

      real xmax,ymax,sigmax(5),fracmax,sigmed
      integer nhmax(5),imax(5)

      integer istrhit,istrhitpmt
      real strhitnpe,strhitpmtnpe
      real strpmtx,strpmty

* --- store hit data (because this is done before LIP rec is called)

      nbhits = 0
      i = 0

      do while(i.lt.min(nbhits_ev,nhitmax))
         i = i+1
         if(hitsnpe_ev(i).ge.0.) then ! exclude bad hits
            nbhits = nbhits+1
            hitscoo(1,nbhits)   = hitscoo_ev(1,i)
            hitscoo(2,nbhits)   = hitscoo_ev(2,i)
            hitscoo(3,nbhits)   = hitscoo_ev(3,i)
            hitsnpe(nbhits)     = hitsnpe_ev(i)
            hitspmt(nbhits)     = hitspmt_ev(i) ! pmt number : int(int(hitspmt(i)/10)/16)
                                                ! pixel      : mod(int(bevent_hid(i)/10),16)
                                                ! gain       : mod(bevent_hid(i),10)
            hitsnpe_sim(nbhits) = hitsnpe_sim_ev(i)
            hitstat(nbhits)     = 0 ! good hit
         endif
      enddo

* --- Find PMTs with strongest signals in detection matrix
      call getpmtmax(xmax,ymax,imax,sigmax,nhmax,fracmax,sigmed)

*      print*,'sigmax = ',sigmax(1),' ... sigmed = ',sigmed,
*     +     ' ... quotient = ',sigmax(1)/sigmed,' ... nhmax = ',nhmax(1)

      istrhit = -1
      strhitnpe = -1.e38

      istrhitpmt = -1
      strhitpmtnpe = -1.e38

      strpmtx = 0.
      strpmty = 0.
      strpmtnpe = 0.

* --- Filling of strongest hit data & strongest PMT barycentre
      do i=1,min(nbhits,nhitmax)
         if(hitsnpe(i).gt.strhitnpe) then
            istrhit = i
            strhitnpe = hitsnpe(i)
         endif
*         print*,'Hit ',i,', PMT ',1+hitspmt(i)/10/16
         if(1+hitspmt(i)/10/16.eq.imax(1)) then
            strpmtx = strpmtx+hitsnpe(i)*hitscoo(1,i)
            strpmty = strpmty+hitsnpe(i)*hitscoo(2,i)
            strpmtnpe = strpmtnpe+hitsnpe(i)
            if(hitsnpe(i).gt.strhitpmtnpe) then
               istrhitpmt = i
               strhitpmtnpe = hitsnpe(i)
            endif
         endif
      enddo

      if(nbhits.gt.0) then
         strpmtx = strpmtx/strpmtnpe
         strpmty = strpmty/strpmtnpe
      else
         strpmtx = 0.
         strpmty = 0.
      endif

      return

      end


***********************************************************
*** RICHTOFTRACKINIT: initialization of TOF-based track ***
***********************************************************

      subroutine richtoftrackinit(itype)

      integer itype

      if(itype.eq.1) then
         call richtoftrackinit1(0)
         return
      elseif(itype.eq.14) then
         call richtoftrackinit1(4)
         return
      elseif(itype.eq.13) then
         call richtoftrackinit1(3)
         return
      elseif(itype.eq.131) then
         call richtoftrackinit1(31)
         return
      elseif(itype.eq.132) then
         call richtoftrackinit1(32)
         return
      elseif(itype.eq.133) then
         call richtoftrackinit1(33)
         return
      elseif(itype.eq.134) then
         call richtoftrackinit1(34)
         return
      elseif(itype.eq.2) then
         call richtoftrackinit2(0)
         return
      elseif(itype.eq.24) then
         call richtoftrackinit2(4)
         return
      elseif(itype.eq.23) then
         call richtoftrackinit2(3)
         return
      elseif(itype.eq.231) then
         call richtoftrackinit2(31)
         return
      elseif(itype.eq.232) then
         call richtoftrackinit2(32)
         return
      elseif(itype.eq.233) then
         call richtoftrackinit2(33)
         return
      elseif(itype.eq.234) then
         call richtoftrackinit2(34)
         return
      elseif(itype.eq.3) then
         call richtoftrackinit3(0)
         return
      elseif(itype.eq.32) then
         call richtoftrackinit3(2)
         return
      elseif(itype.eq.31) then
         call richtoftrackinit3(1)
         return
      elseif(itype.eq.311) then
         call richtoftrackinit3(11)
         return
      elseif(itype.eq.312) then
         call richtoftrackinit3(12)
         return
      elseif(itype.eq.4) then
         call richtoftrackinit4(0)
         return
      elseif(itype.eq.42) then
         call richtoftrackinit4(2)
         return
      elseif(itype.eq.41) then
         call richtoftrackinit4(1)
         return
      elseif(itype.eq.413) then
         call richtoftrackinit4(13)
         return
      elseif(itype.eq.414) then
         call richtoftrackinit4(14)
         return
      endif

      print*,'RICHTOFTRACKINIT: invalid TOF-based reconstruction type'

      return

      end


***************************************************************
*** RICHTOFTRACKINIT1: initialization of TOF track (type I) ***
***************************************************************

      subroutine richtoftrackinit1(itype)

#include "../include/richrec_lipf.h"
#include "../include/richrec_lipc.h"

      integer itype

      real tol

      integer ncand4p,ncand3p
      real cand4p(100,4)
      real cand3p(100,3)
      real currcand(4)
      real extrapx,extrapy
      real extrapdist

      real px(4),perrx(4),pwx(4)
      real py(4),perry(4),pwy(4)
      real pz(4),perrz(4)
      real resax,resbx,resvarx
      real resay,resby,resvary

*      print*,'Entering RICHTOFTRACKINIT1'

*      print*,'ntofclu = ',ntofclu
*      do i=1,ntofclu
*         print*,'coo_tof(*,',i,') = ',
*     +        coo_tof(1,i),coo_tof(2,i),coo_tof(3,i)
*      enddo

      if(itype.ne.0
     +     .and.itype.ne.4
     +     .and.itype.ne.3
     +     .and.itype.ne.31
     +     .and.itype.ne.32
     +     .and.itype.ne.33
     +     .and.itype.ne.34) then
         print*,'RICHTOFTRACKINIT1: invalid TOF-based rec type'
      endif

* --- maximum distance for cluster search
      tol = 10. !5. ! cm

      ncand4p = 0
      ncand3p = 0

* --- reset reconstruction flag
      iflag_tof = 0

* --- cycle over possible pairs of clusters in planes 1,4

      do i=1,ntofclu
         if(ilayer_tof(i).eq.1) then

            currcand(1) = i

            do j=1,ntofclu
               if(ilayer_tof(j).eq.4) then

                  currcand(4) = j

* --------------- look for nearby clusters in planes 2,3

                  flagplane2 = 0
                  flagplane3 = 0

                  currcand(2) = -1
                  currcand(3) = -1

                  do k=1,ntofclu

                     if((ilayer_tof(k).eq.2)
     +                    .or.(ilayer_tof(k).eq.3)) then

*                        print*,'Testing',i,j,k

                        extrapx = coo_tof(1,i)
     +                       +(coo_tof(1,j)-coo_tof(1,i))
     +                       *(coo_tof(3,k)-coo_tof(3,i))
     +                       /(coo_tof(3,j)-coo_tof(3,i))
                        extrapy = coo_tof(2,i)
     +                       +(coo_tof(2,j)-coo_tof(2,i))
     +                       *(coo_tof(3,k)-coo_tof(3,i))
     +                       /(coo_tof(3,j)-coo_tof(3,i))

                        extrapdist2 = (extrapx-coo_tof(1,k))**2.
     +                               +(extrapy-coo_tof(2,k))**2.

*                        print*,'Extrap = ',extrapx,extrapy
*                        print*,'Dist = ',extrapdist2**0.5

                        if(extrapdist2.lt.tol**2.) then

                           if(ilayer_tof(k).eq.2) then
                              flagplane2 = 1
                              currcand(2) = k
                           endif

                           if(ilayer_tof(k).eq.3) then
                              flagplane3 = 1
                              currcand(3) = k
                           endif

                        endif

                     endif

                  enddo

* --------------- save sets of 3 and 4 aligned clusters
*                 (only one set is saved per 1-4 pair)

                  if(currcand(2).gt.0.and.currcand(3).gt.0) then
                     if(itype.eq.0.or.itype.eq.4) then
                        ncand4p = ncand4p+1
                        do n=1,4
                           cand4p(ncand4p,n) = currcand(n)
                        enddo
                     endif
                  else if(currcand(2).gt.0.and.currcand(3).le.0) then
                     if(itype.eq.0.or.itype.eq.3.or.itype.eq.33) then
                        ncand3p = ncand3p+1
                        cand3p(ncand3p,1) = currcand(1)
                        cand3p(ncand3p,2) = currcand(2)
                        cand3p(ncand3p,3) = currcand(4)
                     endif
                  else if(currcand(2).le.0.and.currcand(3).gt.0) then
                     if(itype.eq.0.or.itype.eq.3.or.itype.eq.32) then
                        ncand3p = ncand3p+1
                        cand3p(ncand3p,1) = currcand(1)
                        cand3p(ncand3p,2) = currcand(3)
                        cand3p(ncand3p,3) = currcand(4)
                     endif
                  endif

               endif
            enddo
         endif
      enddo

* --- cycle over possible pairs of clusters in planes 2,4

      currcand(1) = 0

      do i=1,ntofclu
         if(ilayer_tof(i).eq.2) then

            currcand(2) = i

            do j=1,ntofclu
               if(ilayer_tof(j).eq.4) then

                  currcand(4) = j

* --------------- look for nearby clusters in plane 3

                  flagplane3 = 0

                  currcand(3) = -1

                  do k=1,ntofclu

                     if(ilayer_tof(k).eq.3) then

*                        print*,'Testing',i,j,k

                        extrapx = coo_tof(1,i)
     +                       +(coo_tof(1,j)-coo_tof(1,i))
     +                       *(coo_tof(3,k)-coo_tof(3,i))
     +                       /(coo_tof(3,j)-coo_tof(3,i))
                        extrapy = coo_tof(2,i)
     +                       +(coo_tof(2,j)-coo_tof(2,i))
     +                       *(coo_tof(3,k)-coo_tof(3,i))
     +                       /(coo_tof(3,j)-coo_tof(3,i))

                        extrapdist2 = (extrapx-coo_tof(1,k))**2.
     +                               +(extrapy-coo_tof(2,k))**2.

*                        print*,'Extrap = ',extrapx,extrapy
*                        print*,'Dist = ',extrapdist2**0.5

                        if(extrapdist2.lt.tol**2.) then

                           flagplane3 = 1
                           currcand(3) = k

                        endif

                     endif

                  enddo

* --------------- save sets of 3 aligned clusters
*                 (only one set is saved per 2-4 pair)

                  if(currcand(3).gt.0) then
                     if(itype.eq.0.or.itype.eq.3.or.itype.eq.31) then
                        ncand3p = ncand3p+1
                        do n=1,3
                           cand3p(ncand3p,n) = currcand(n+1)
                        enddo
                     endif
                  endif

               endif
            enddo
         endif
      enddo

* --- cycle over possible pairs of clusters in planes 1,3

      currcand(1) = 0

      do i=1,ntofclu
         if(ilayer_tof(i).eq.1) then

            currcand(2) = i

            do j=1,ntofclu
               if(ilayer_tof(j).eq.3) then

                  currcand(4) = j

* --------------- look for nearby clusters in plane 2

                  flagplane2 = 0

                  currcand(3) = -1

                  do k=1,ntofclu

                     if(ilayer_tof(k).eq.2) then

*                        print*,'Testing',i,j,k

                        extrapx = coo_tof(1,i)
     +                       +(coo_tof(1,j)-coo_tof(1,i))
     +                       *(coo_tof(3,k)-coo_tof(3,i))
     +                       /(coo_tof(3,j)-coo_tof(3,i))
                        extrapy = coo_tof(2,i)
     +                       +(coo_tof(2,j)-coo_tof(2,i))
     +                       *(coo_tof(3,k)-coo_tof(3,i))
     +                       /(coo_tof(3,j)-coo_tof(3,i))

                        extrapdist2 = (extrapx-coo_tof(1,k))**2.
     +                               +(extrapy-coo_tof(2,k))**2.

*                        print*,'Extrap = ',extrapx,extrapy
*                        print*,'Dist = ',extrapdist2**0.5

                        if(extrapdist2.lt.tol**2.) then

                           flagplane2 = 1
                           currcand(3) = k

                        endif

                     endif

                  enddo

* --------------- save sets of 3 aligned clusters
*                 (only one set is saved per 1-3 pair)

                  if(currcand(3).gt.0) then
                     if(itype.eq.0.or.itype.eq.3.or.itype.eq.34) then
                        ncand3p = ncand3p+1
                        do n=1,3
                           cand3p(ncand3p,n) = currcand(n+1)
                        enddo
                     endif
                  endif

               endif
            enddo
         endif
      enddo

*      print*,'ncand4p = ',ncand4p
*      print*,'ncand3p = ',ncand3p

* --- Generate TOF track from sets of clusters

      if(ncand4p.eq.1) then

         iflag_tof = 1

         do i=1,4
            px(i) = coo_tof(1,cand4p(1,i))
            py(i) = coo_tof(2,cand4p(1,i))
            pz(i) = coo_tof(3,cand4p(1,i))
            perrx(i) = errcoo_tof(1,cand4p(1,i))
            perry(i) = errcoo_tof(2,cand4p(1,i))
            perrz(i) = errcoo_tof(3,cand4p(1,i))
            pwx(i) = 1./perrx(i)
            pwy(i) = 1./perry(i)
         enddo

*         print*,'LFITW data (ver1, 4p):'
*         print*,'x = ',px
*         print*,'y = ',py
*         print*,'z = ',pz
*         print*,'wx = ',pwx
*         print*,'wy = ',pwy

         call lfitw(px,pz,pwx,4,1,resax,resbx,resvarx)
         call lfitw(py,pz,pwy,4,1,resay,resby,resvary)

      else if(ncand4p.eq.0.and.ncand3p.eq.1) then

         iflag_tof = 1

         do i=1,3
            px(i) = coo_tof(1,cand3p(1,i))
            py(i) = coo_tof(2,cand3p(1,i))
            pz(i) = coo_tof(3,cand3p(1,i))
            perrx(i) = errcoo_tof(1,cand3p(1,i))
            perry(i) = errcoo_tof(2,cand3p(1,i))
            perrz(i) = errcoo_tof(3,cand3p(1,i))
            pwx(i) = 1./perrx(i)
            pwy(i) = 1./perry(i)
         enddo

         px(4) = -999.
         py(4) = -999.
         pz(4) = -999.
         perrx(4) = -999.
         perry(4) = -999.
         perrz(4) = -999.
         pwx(4) = -999.
         pwy(4) = -999.

*         print*,'LFITW data (ver1, 3p):'
*         print*,'x = ',px
*         print*,'y = ',py
*         print*,'z = ',pz
*         print*,'wx = ',pwx
*         print*,'wy = ',pwy

         call lfitw(px,pz,pwx,3,1,resax,resbx,resvarx)
         call lfitw(py,pz,pwy,3,1,resay,resby,resvary)

      endif

* --- Calculate final TOF track variables

      if(iflag_tof.eq.1) then

         pimp_tof(1) = -resbx/resax
         pimp_tof(2) = -resby/resay
         pimp_tof(3) = ZTARG

         pthe_tof = atan(((1./resax)**2.+(1./resay)**2.)**0.5)
         pphi_tof = atan2(1./resay,1./resax)

* ------ THE ERRORS BELOW SHOULD BE REPLACED BY REALISTIC VALUES
         epimp_tof(1) = 1.
         epimp_tof(2) = 1.
         epimp_tof(3) = 1.
         epthe_tof = 1.
         epphi_tof = 1.

*         print*,'===== TOF track successful ====='
*         print*,'pimp_tof    = ',pimp_tof
*         print*,'pthe_tof(d) = ',pthe_tof*RADDEG
*         print*,'pphi_tof(d) = ',pphi_tof*RADDEG

      else
*         print*,'===== TOF track failed ====='

      endif

      return

      end


****************************************************************
*** RICHTOFTRACKINIT2: initialization of TOF track (type II) ***
****************************************************************

      subroutine richtoftrackinit2(itype)

#include "../include/richrec_lipf.h"
#include "../include/richrec_lipc.h"

      integer itype

      real tol

      integer ncand4p,ncand3p
      real cand4p(100,5)
      real cand3p(100,4)
      real currcand(5)
      real extrapx,extrapy
      real extrapdist

      real px(5),perrx(5),pwx(5)
      real py(5),perry(5),pwy(5)
      real pz(5),perrz(5)
      real resax,resbx,resvarx
      real resay,resby,resvary

*      print*,'Entering RICHTOFTRACKINIT2'

*      print*,'ntofclu = ',ntofclu
*      do i=1,ntofclu
*         print*,'coo_tof(*,',i,') = ',
*     +        coo_tof(1,i),coo_tof(2,i),coo_tof(3,i)
*      enddo

      if(itype.ne.0
     +     .and.itype.ne.4
     +     .and.itype.ne.3
     +     .and.itype.ne.31
     +     .and.itype.ne.32
     +     .and.itype.ne.33
     +     .and.itype.ne.34) then
         print*,'RICHTOFTRACKINIT2: invalid TOF-based rec type'
      endif

* --- maximum distance for cluster search
      tol = 10. !5. ! cm

      ncand4p = 0
      ncand3p = 0

* --- reset reconstruction flag
      iflag_tof = 0

* --- cycle over possible pairs of clusters in planes 1,5

      do i=1,ntofclu
         if(ilayer_tof(i).eq.1) then

            currcand(1) = i

            do j=1,ntofclu
               if(ilayer_tof(j).eq.5) then

                  currcand(5) = j

* --------------- look for nearby clusters in planes 2,3,4

                  flagplane2 = 0
                  flagplane3 = 0
                  flagplane4 = 0

                  currcand(2) = -1
                  currcand(3) = -1
                  currcand(4) = -1

                  do k=1,ntofclu

                     if((ilayer_tof(k).eq.2)
     +                    .or.(ilayer_tof(k).eq.3)
     +                    .or.(ilayer_tof(k).eq.4)) then

*                        print*,'Testing',i,j,k

                        extrapx = coo_tof(1,i)
     +                       +(coo_tof(1,j)-coo_tof(1,i))
     +                       *(coo_tof(3,k)-coo_tof(3,i))
     +                       /(coo_tof(3,j)-coo_tof(3,i))
                        extrapy = coo_tof(2,i)
     +                       +(coo_tof(2,j)-coo_tof(2,i))
     +                       *(coo_tof(3,k)-coo_tof(3,i))
     +                       /(coo_tof(3,j)-coo_tof(3,i))

                        extrapdist2 = (extrapx-coo_tof(1,k))**2.
     +                               +(extrapy-coo_tof(2,k))**2.

*                        print*,'Extrap = ',extrapx,extrapy
*                        print*,'Dist = ',extrapdist2**0.5

                        if(extrapdist2.lt.tol**2.) then

                           if(ilayer_tof(k).eq.2) then
                              flagplane2 = 1
                              currcand(2) = k
                           endif

                           if(ilayer_tof(k).eq.3) then
                              flagplane3 = 1
                              currcand(3) = k
                           endif

                           if(ilayer_tof(k).eq.4) then
                              flagplane4 = 1
                              currcand(4) = k
                           endif

                        endif

                     endif

                  enddo

* --------------- save sets of 3 and 4 aligned clusters
*                 (only one set is saved per 1-5 pair)

                  if(currcand(2).gt.0.and.currcand(3).gt.0
     +                 .and.currcand(4).gt.0) then
                     if(itype.eq.0.or.itype.eq.4) then
                        ncand4p = ncand4p+1
                        do n=1,5
                           cand4p(ncand4p,n) = currcand(n)
                        enddo
                     endif
                  else if(currcand(2).gt.0.and.currcand(3).gt.0
     +                    .and.currcand(4).le.0) then
                     if(itype.eq.0.or.itype.eq.3.or.itype.eq.34) then
                        ncand3p = ncand3p+1
                        cand3p(ncand3p,1) = currcand(1)
                        cand3p(ncand3p,2) = currcand(2)
                        cand3p(ncand3p,3) = currcand(3)
                        cand3p(ncand3p,4) = currcand(5)
                     endif
                  else if(currcand(2).gt.0.and.currcand(3).le.0
     +                    .and.currcand(4).gt.0) then
                     if(itype.eq.0.or.itype.eq.3.or.itype.eq.33) then
                        ncand3p = ncand3p+1
                        cand3p(ncand3p,1) = currcand(1)
                        cand3p(ncand3p,2) = currcand(2)
                        cand3p(ncand3p,3) = currcand(4)
                        cand3p(ncand3p,4) = currcand(5)
                     endif
                  else if(currcand(2).le.0.and.currcand(3).gt.0
     +                    .and.currcand(4).gt.0) then
                     if(itype.eq.0.or.itype.eq.3.or.itype.eq.32) then
                        ncand3p = ncand3p+1
                        cand3p(ncand3p,1) = currcand(1)
                        cand3p(ncand3p,2) = currcand(3)
                        cand3p(ncand3p,3) = currcand(4)
                        cand3p(ncand3p,4) = currcand(5)
                     endif
                  endif

               endif
            enddo
         endif
      enddo

* --- cycle over possible pairs of clusters in planes 2,5

      currcand(1) = 0

      do i=1,ntofclu
         if(ilayer_tof(i).eq.2) then

            currcand(2) = i

            do j=1,ntofclu
               if(ilayer_tof(j).eq.5) then

                  currcand(5) = j

* --------------- look for nearby clusters in planes 3,4

                  flagplane3 = 0
                  flagplane4 = 0

                  currcand(3) = -1
                  currcand(4) = -1

                  do k=1,ntofclu

                     if((ilayer_tof(k).eq.3)
     +                    .or.(ilayer_tof(k).eq.4)) then

*                        print*,'Testing',i,j,k

                        extrapx = coo_tof(1,i)
     +                       +(coo_tof(1,j)-coo_tof(1,i))
     +                       *(coo_tof(3,k)-coo_tof(3,i))
     +                       /(coo_tof(3,j)-coo_tof(3,i))
                        extrapy = coo_tof(2,i)
     +                       +(coo_tof(2,j)-coo_tof(2,i))
     +                       *(coo_tof(3,k)-coo_tof(3,i))
     +                       /(coo_tof(3,j)-coo_tof(3,i))

                        extrapdist2 = (extrapx-coo_tof(1,k))**2.
     +                               +(extrapy-coo_tof(2,k))**2.

*                        print*,'Extrap = ',extrapx,extrapy
*                        print*,'Dist = ',extrapdist2**0.5

                        if(extrapdist2.lt.tol**2.) then

                           if(ilayer_tof(k).eq.3) then
                              flagplane3 = 1
                              currcand(3) = k
                           endif

                           if(ilayer_tof(k).eq.4) then
                              flagplane4 = 1
                              currcand(4) = k
                           endif

                        endif

                     endif

                  enddo

* --------------- save sets of 3 aligned clusters
*                 (only one set is saved per 2-5 pair)

                  if(currcand(3).gt.0.and.currcand(4).gt.0) then
                     if(itype.eq.0.or.itype.eq.3.or.itype.eq.31) then
                        ncand3p = ncand3p+1
                        do n=1,4
                           cand3p(ncand3p,n) = currcand(n+1)
                        enddo
                     endif
                  endif

               endif
            enddo
         endif
      enddo

*      print*,'ncand4p = ',ncand4p
*      print*,'ncand3p = ',ncand3p

* --- Generate TOF track from sets of clusters

      if(ncand4p.eq.1) then

         iflag_tof = 1

         do i=1,5
            px(i) = coo_tof(1,cand4p(1,i))
            py(i) = coo_tof(2,cand4p(1,i))
            pz(i) = coo_tof(3,cand4p(1,i))
            perrx(i) = errcoo_tof(1,cand4p(1,i))
            perry(i) = errcoo_tof(2,cand4p(1,i))
            perrz(i) = errcoo_tof(3,cand4p(1,i))
            pwx(i) = 1./perrx(i)
            pwy(i) = 1./perry(i)
         enddo

*         print*,'LFITW data (ver2, 4p):'
*         print*,'x = ',px
*         print*,'y = ',py
*         print*,'z = ',pz
*         print*,'wx = ',pwx
*         print*,'wy = ',pwy

         call lfitw(px,pz,pwx,5,1,resax,resbx,resvarx)
         call lfitw(py,pz,pwy,5,1,resay,resby,resvary)

      else if(ncand4p.eq.0.and.ncand3p.eq.1) then

         iflag_tof = 1

         do i=1,4
            px(i) = coo_tof(1,cand3p(1,i))
            py(i) = coo_tof(2,cand3p(1,i))
            pz(i) = coo_tof(3,cand3p(1,i))
            perrx(i) = errcoo_tof(1,cand3p(1,i))
            perry(i) = errcoo_tof(2,cand3p(1,i))
            perrz(i) = errcoo_tof(3,cand3p(1,i))
            pwx(i) = 1./perrx(i)
            pwy(i) = 1./perry(i)
         enddo

         px(5) = -999.
         py(5) = -999.
         pz(5) = -999.
         perrx(5) = -999.
         perry(5) = -999.
         perrz(5) = -999.
         pwx(5) = -999.
         pwy(5) = -999.

*         print*,'LFITW data (ver2, 3p):'
*         print*,'x = ',px
*         print*,'y = ',py
*         print*,'z = ',pz
*         print*,'wx = ',pwx
*         print*,'wy = ',pwy

         call lfitw(px,pz,pwx,4,1,resax,resbx,resvarx)
         call lfitw(py,pz,pwy,4,1,resay,resby,resvary)

      endif

* --- Calculate final TOF track variables

      if(iflag_tof.eq.1) then

         pimp_tof(1) = -resbx/resax
         pimp_tof(2) = -resby/resay
         pimp_tof(3) = ZTARG

         pthe_tof = atan(((1./resax)**2.+(1./resay)**2.)**0.5)
         pphi_tof = atan2(1./resay,1./resax)

* ------ THE ERRORS BELOW SHOULD BE REPLACED BY REALISTIC VALUES
         epimp_tof(1) = 1.
         epimp_tof(2) = 1.
         epimp_tof(3) = 1.
         epthe_tof = 1.
         epphi_tof = 1.

*         print*,'===== TOF2 track successful ====='
*         print*,'pimp_tof    = ',pimp_tof
*         print*,'pthe_tof(d) = ',pthe_tof*RADDEG
*         print*,'pphi_tof(d) = ',pphi_tof*RADDEG

      else
*         print*,'===== TOF2 track failed ====='

      endif

      return

      end


*****************************************************************
*** RICHTOFTRACKINIT3: initialization of TOF track (type III) ***
*****************************************************************

      subroutine richtoftrackinit3(itype)

#include "../include/richrec_lipf.h"
#include "../include/richrec_lipc.h"

      integer itype

      real tol

      integer ncand2p,ncand1p
      real cand2p(100,3)
      real cand1p(100,2)
      real currcand(3)
      real extrapx,extrapy
      real extrapdist

      real px(3),perrx(3),pwx(3)
      real py(3),perry(3),pwy(3)
      real pz(3),perrz(3)
      real resax,resbx,resvarx
      real resay,resby,resvary

*      print*,'Entering RICHTOFTRACKINIT3'

*      print*,'ntofclu = ',ntofclu
*      do i=1,ntofclu
*         print*,'coo_tof(*,',i,') = ',
*     +        coo_tof(1,i),coo_tof(2,i),coo_tof(3,i)
*      enddo

      if(itype.ne.0
     +     .and.itype.ne.2
     +     .and.itype.ne.1
     +     .and.itype.ne.11
     +     .and.itype.ne.12) then
         print*,'RICHTOFTRACKINIT3: invalid TOF-based rec type'
      endif

* --- maximum distance for cluster search
      tol = 10. !5. ! cm

      ncand2p = 0
      ncand1p = 0

* --- reset reconstruction flag
      iflag_tof = 0

* --- cycle over possible pairs of clusters in planes 1,5

      do i=1,ntofclu
         if(ilayer_tof(i).eq.1) then

            currcand(1) = i

            do j=1,ntofclu
               if(ilayer_tof(j).eq.5) then

                  currcand(3) = j

* --------------- look for nearby cluster in plane 2

                  flagplane2 = 0

                  currcand(2) = -1

                  do k=1,ntofclu

                     if(ilayer_tof(k).eq.2) then

*                        print*,'Testing',i,j,k

                        extrapx = coo_tof(1,i)
     +                       +(coo_tof(1,j)-coo_tof(1,i))
     +                       *(coo_tof(3,k)-coo_tof(3,i))
     +                       /(coo_tof(3,j)-coo_tof(3,i))
                        extrapy = coo_tof(2,i)
     +                       +(coo_tof(2,j)-coo_tof(2,i))
     +                       *(coo_tof(3,k)-coo_tof(3,i))
     +                       /(coo_tof(3,j)-coo_tof(3,i))

                        extrapdist2 = (extrapx-coo_tof(1,k))**2.
     +                               +(extrapy-coo_tof(2,k))**2.

*                        print*,'Extrap = ',extrapx,extrapy
*                        print*,'Dist = ',extrapdist2**0.5

                        if(extrapdist2.lt.tol**2.) then

                           flagplane2 = 1
                           currcand(2) = k

                        endif

                     endif

                  enddo

* --------------- save sets of 1 and 2 aligned clusters
*                 (only one set is saved per 1-5 pair)

                  if(currcand(2).gt.0) then
                     if(itype.eq.0.or.itype.eq.2) then
                        ncand2p = ncand2p+1
                        do n=1,3
                           cand2p(ncand2p,n) = currcand(n)
                        enddo
                     endif
                  else
                     if(itype.eq.0.or.itype.eq.1.or.itype.eq.12) then
                        ncand1p = ncand1p+1
                        cand1p(ncand1p,1) = currcand(1)
                        cand1p(ncand1p,2) = currcand(3)
                     endif
                  endif
               endif
            enddo
         endif
      enddo

* --- cycle over possible pairs of clusters in planes 2,5

      currcand(1) = 0

      do i=1,ntofclu
         if(ilayer_tof(i).eq.2) then

            currcand(2) = i

            do j=1,ntofclu
               if(ilayer_tof(j).eq.5) then

                  currcand(3) = j

* --------------- save sets of 2 aligned clusters

                  if(itype.eq.0.or.itype.eq.1.or.itype.eq.11) then
                     ncand1p = ncand1p+1
                     do n=1,2
                        cand1p(ncand1p,n) = currcand(n+1)
                     enddo
                  endif

               endif
            enddo
         endif
      enddo

*      print*,'ncand2p = ',ncand2p
*      print*,'ncand1p = ',ncand1p

* --- Generate TOF track from sets of clusters

      if(ncand2p.eq.1) then

         iflag_tof = 1

         do i=1,3
            px(i) = coo_tof(1,cand2p(1,i))
            py(i) = coo_tof(2,cand2p(1,i))
            pz(i) = coo_tof(3,cand2p(1,i))
            perrx(i) = errcoo_tof(1,cand2p(1,i))
            perry(i) = errcoo_tof(2,cand2p(1,i))
            perrz(i) = errcoo_tof(3,cand2p(1,i))
            pwx(i) = 1./perrx(i)
            pwy(i) = 1./perry(i)
         enddo

*         print*,'LFITW data (ver3, 2p):'
*         print*,'x = ',px
*         print*,'y = ',py
*         print*,'z = ',pz
*         print*,'wx = ',pwx
*         print*,'wy = ',pwy

         call lfitw(px,pz,pwx,3,1,resax,resbx,resvarx)
         call lfitw(py,pz,pwy,3,1,resay,resby,resvary)

      else if(ncand2p.eq.0.and.ncand1p.eq.1) then

         iflag_tof = 1

         do i=1,2
            px(i) = coo_tof(1,cand1p(1,i))
            py(i) = coo_tof(2,cand1p(1,i))
            pz(i) = coo_tof(3,cand1p(1,i))
            perrx(i) = errcoo_tof(1,cand1p(1,i))
            perry(i) = errcoo_tof(2,cand1p(1,i))
            perrz(i) = errcoo_tof(3,cand1p(1,i))
            pwx(i) = 1./perrx(i)
            pwy(i) = 1./perry(i)
         enddo

         px(3) = -999.
         py(3) = -999.
         pz(3) = -999.
         perrx(3) = -999.
         perry(3) = -999.
         perrz(3) = -999.
         pwx(3) = -999.
         pwy(3) = -999.

*         print*,'LFITW data (ver3, 1p):'
*         print*,'x = ',px
*         print*,'y = ',py
*         print*,'z = ',pz
*         print*,'wx = ',pwx
*         print*,'wy = ',pwy

         call lfitw(px,pz,pwx,2,1,resax,resbx,resvarx)
         call lfitw(py,pz,pwy,2,1,resay,resby,resvary)

      endif

* --- Calculate final TOF track variables

      if(iflag_tof.eq.1) then

         pimp_tof(1) = -resbx/resax
         pimp_tof(2) = -resby/resay
         pimp_tof(3) = ZTARG

         pthe_tof = atan(((1./resax)**2.+(1./resay)**2.)**0.5)
         pphi_tof = atan2(1./resay,1./resax)

* ------ THE ERRORS BELOW SHOULD BE REPLACED BY REALISTIC VALUES
         epimp_tof(1) = 1.
         epimp_tof(2) = 1.
         epimp_tof(3) = 1.
         epthe_tof = 1.
         epphi_tof = 1.

*         print*,'===== TOF3 track successful ====='
*         print*,'pimp_tof    = ',pimp_tof
*         print*,'pthe_tof(d) = ',pthe_tof*RADDEG
*         print*,'pphi_tof(d) = ',pphi_tof*RADDEG

      else
*         print*,'===== TOF3 track failed ====='

      endif

      return

      end


****************************************************************
*** RICHTOFTRACKINIT4: initialization of TOF track (type IV) ***
****************************************************************

      subroutine richtoftrackinit4(itype)

#include "../include/richrec_lipf.h"
#include "../include/richrec_lipc.h"

      integer itype

      real tol

      integer ncand2p,ncand1p
      real cand2p(100,3)
      real cand1p(100,2)
      real currcand(3)
      real extrapx,extrapy
      real extrapdist

      real px(3),perrx(3),pwx(3)
      real py(3),perry(3),pwy(3)
      real pz(3),perrz(3)
      real resax,resbx,resvarx
      real resay,resby,resvary

*      print*,'Entering RICHTOFTRACKINIT4'

*      print*,'ntofclu = ',ntofclu
*      do i=1,ntofclu
*         print*,'coo_tof(*,',i,') = ',
*     +        coo_tof(1,i),coo_tof(2,i),coo_tof(3,i)
*      enddo

      if(itype.ne.0
     +     .and.itype.ne.2
     +     .and.itype.ne.1
     +     .and.itype.ne.13
     +     .and.itype.ne.14) then
         print*,'RICHTOFTRACKINIT4: invalid TOF-based rec type'
      endif

* --- maximum distance for cluster search
      tol = 10. !5. ! cm

      ncand2p = 0
      ncand1p = 0

* --- reset reconstruction flag
      iflag_tof = 0

* --- cycle over possible pairs of clusters in planes 3,5

      do i=1,ntofclu
         if(ilayer_tof(i).eq.3) then

            currcand(1) = i

            do j=1,ntofclu
               if(ilayer_tof(j).eq.5) then

                  currcand(3) = j

* --------------- look for nearby cluster in plane 4

                  flagplane4 = 0

                  currcand(2) = -1

                  do k=1,ntofclu

                     if(ilayer_tof(k).eq.4) then

*                        print*,'Testing',i,j,k

                        extrapx = coo_tof(1,i)
     +                       +(coo_tof(1,j)-coo_tof(1,i))
     +                       *(coo_tof(3,k)-coo_tof(3,i))
     +                       /(coo_tof(3,j)-coo_tof(3,i))
                        extrapy = coo_tof(2,i)
     +                       +(coo_tof(2,j)-coo_tof(2,i))
     +                       *(coo_tof(3,k)-coo_tof(3,i))
     +                       /(coo_tof(3,j)-coo_tof(3,i))

                        extrapdist2 = (extrapx-coo_tof(1,k))**2.
     +                               +(extrapy-coo_tof(2,k))**2.

*                        print*,'Extrap = ',extrapx,extrapy
*                        print*,'Dist = ',extrapdist2**0.5

                        if(extrapdist2.lt.tol**2.) then

                           flagplane2 = 1
                           currcand(2) = k

                        endif

                     endif

                  enddo

* --------------- save sets of 1 and 2 aligned clusters
*                 (only one set is saved per 1-5 pair)

                  if(currcand(2).gt.0) then
                     if(itype.eq.0.or.itype.eq.2) then
                        ncand2p = ncand2p+1
                        do n=1,3
                           cand2p(ncand2p,n) = currcand(n)
                        enddo
                     endif
                  else
                     if(itype.eq.0.or.itype.eq.1.or.itype.eq.14) then
                        ncand1p = ncand1p+1
                        cand1p(ncand1p,1) = currcand(1)
                        cand1p(ncand1p,2) = currcand(3)
                     endif
                  endif

               endif
            enddo
         endif
      enddo

* --- cycle over possible pairs of clusters in planes 4,5

      currcand(1) = 0

      do i=1,ntofclu
         if(ilayer_tof(i).eq.4) then

            currcand(2) = i

            do j=1,ntofclu
               if(ilayer_tof(j).eq.5) then

                  currcand(3) = j

* --------------- save sets of 2 aligned clusters

                  if(itype.eq.0.or.itype.eq.1.or.itype.eq.11) then
                     ncand1p = ncand1p+1
                     do n=1,2
                        cand1p(ncand1p,n) = currcand(n+1)
                     enddo
                  endif

               endif
            enddo
         endif
      enddo

*      print*,'ncand2p = ',ncand2p
*      print*,'ncand1p = ',ncand1p

* --- Generate TOF track from sets of clusters

      if(ncand2p.eq.1) then

         iflag_tof = 1

         do i=1,3
            px(i) = coo_tof(1,cand2p(1,i))
            py(i) = coo_tof(2,cand2p(1,i))
            pz(i) = coo_tof(3,cand2p(1,i))
            perrx(i) = errcoo_tof(1,cand2p(1,i))
            perry(i) = errcoo_tof(2,cand2p(1,i))
            perrz(i) = errcoo_tof(3,cand2p(1,i))
            pwx(i) = 1./perrx(i)
            pwy(i) = 1./perry(i)
         enddo

*         print*,'LFITW data (ver4, 2p):'
*         print*,'x = ',px
*         print*,'y = ',py
*         print*,'z = ',pz
*         print*,'wx = ',pwx
*         print*,'wy = ',pwy

         call lfitw(px,pz,pwx,3,1,resax,resbx,resvarx)
         call lfitw(py,pz,pwy,3,1,resay,resby,resvary)

      else if(ncand2p.eq.0.and.ncand1p.eq.1) then

         iflag_tof = 1

         do i=1,2
            px(i) = coo_tof(1,cand1p(1,i))
            py(i) = coo_tof(2,cand1p(1,i))
            pz(i) = coo_tof(3,cand1p(1,i))
            perrx(i) = errcoo_tof(1,cand1p(1,i))
            perry(i) = errcoo_tof(2,cand1p(1,i))
            perrz(i) = errcoo_tof(3,cand1p(1,i))
            pwx(i) = 1./perrx(i)
            pwy(i) = 1./perry(i)
         enddo

         px(3) = -999.
         py(3) = -999.
         pz(3) = -999.
         perrx(3) = -999.
         perry(3) = -999.
         perrz(3) = -999.
         pwx(3) = -999.
         pwy(3) = -999.

*         print*,'LFITW data (ver4, 1p):'
*         print*,'x = ',px
*         print*,'y = ',py
*         print*,'z = ',pz
*         print*,'wx = ',pwx
*         print*,'wy = ',pwy

         call lfitw(px,pz,pwx,2,1,resax,resbx,resvarx)
         call lfitw(py,pz,pwy,2,1,resay,resby,resvary)

      endif

* --- Calculate final TOF track variables

      if(iflag_tof.eq.1) then

         pimp_tof(1) = -resbx/resax
         pimp_tof(2) = -resby/resay
         pimp_tof(3) = ZTARG

         pthe_tof = atan(((1./resax)**2.+(1./resay)**2.)**0.5)
         pphi_tof = atan2(1./resay,1./resax)

* ------ THE ERRORS BELOW SHOULD BE REPLACED BY REALISTIC VALUES
         epimp_tof(1) = 1.
         epimp_tof(2) = 1.
         epimp_tof(3) = 1.
         epthe_tof = 1.
         epphi_tof = 1.

*         print*,'===== TOF4 track successful ====='
*         print*,'pimp_tof    = ',pimp_tof
*         print*,'pthe_tof(d) = ',pthe_tof*RADDEG
*         print*,'pphi_tof(d) = ',pphi_tof*RADDEG

      else
*         print*,'===== TOF4 track failed ====='

      endif

      return

      end


**************************************************************
*** RICHRECFLEX: reconstruction using uncertain track data ***
**************************************************************

      subroutine richrecflex

#include "../include/richrec_lipf.h"

      integer irshint
      real minsigquo
      integer ierris

      real x0m_in,y0m_in
      real x0v_in,y0v_in
      real theta_in,phi_in,thetac_in

      integer iflag_out
      real x0m_out,y0m_out
      real x0v_out,y0v_out
      real theta_out,phi_out,thetac_out
      real chi2_out
      integer nuhits_out
      integer phit_out(nhitmax),used_out(nhitmax)
      real hres_out(nhitmax)

      real errhint(5)
      real vlike
      integer ierr5p

      real ztest,xrad_in,yrad_in,refind_in

      integer amsrk
      double precision amsri
      integer amsrt

      real ZEMI

* --- set reconstruction options
cc    RECBET(1)  = 11   ! unused
      RECBET(4)  = 1    ! minimum search with brent method

* ... phi hint mechanism
      IPHIHINTKIND = 0
      IPHIHINTFLAG = 0

* ... Minimum quotient between strongest and average PMT signals
      minsigquo = 0. ! should be zero for TOF reconstruction

* ... Type of RICH hint to be used
      irshint = 1

* ... Locate particle signal in PMT matrix
      call richinitstand(irshint,minsigquo,ierris)

* ... get coordinates of track points and set Cerenkov angle hint
*     (x0v,y0v: top radiator - x0m,y0m: inside PMT matrix)

      x0v_in = pimp(1)
      y0v_in = pimp(2)
      x0m_in = pimp(1) +
     +         tan(pthe)*cos(pphi)*(ZPMTDET+ZLGSIGNAL-pimp(3))
      y0m_in = pimp(2) +
     +         tan(pthe)*sin(pphi)*(ZPMTDET+ZLGSIGNAL-pimp(3))

* ... determine radiator for track hint
      ZTEST = ZTARG+(HRAD-HRNAF)
      xrad_in = x0v_in+(x0m_in-x0v_in)*ZTEST/(ZPMTDET+ZLGSIGNAL)
      yrad_in = y0v_in+(y0m_in-y0v_in)*ZTEST/(ZPMTDET+ZLGSIGNAL)

#ifndef LIPRICHALONE
* ... New method using data from global simulation
      call lipgetrad(xrad_in,yrad_in,ZTEST,0.,0.,
     +               amsrk,amsri,amsrt)
*      print*,'(B) amsrk = ',amsrk

      if(amsrk.eq.0) then ! no radiator
         refind_in = -999.
         thetac_in = -999.
      endif

      if(amsrk.eq.1) then ! aerogel
*         refind_in = radix(1)
         refind_in = 1.05  ! temporary fix to have same thc hint in all events
         thetac_in = acos(1./refind_in)
      endif

      if(amsrk.eq.2) then ! NaF
         refind_in = radix(2)
         thetac_in = acos(1./refind_in)
      endif
#else
* ... Old method
      if(abs(xrad_in).lt.(0.5*3.*radtile_pitch).and.
     +     abs(yrad_in).lt.(0.5*3.*radtile_pitch)) then
         refind_in = radix(2) !NAF
      else
         refind_in = radix(1) !AGL
      endif

      thetac_in = acos(1./refind_in)
#endif


*      print*,'INGOING DATA FOR TOF RECONSTRUCTION'
*      print*,'-----------------------------------'
*      print*,'x0m_in (inside LG) = ',x0m_in
*      print*,'y0m_in (inside LG) = ',y0m_in
*      print*,'x0v_in (top radiator) = ',x0v_in
*      print*,'y0v_in (top radiator) = ',y0v_in
*      print*,'thetac_in (deg) = ',thetac_in*RADDEG

      x0m_out = x0m_in
      y0m_out = y0m_in
      x0v_out = x0v_in
      y0v_out = y0v_in
      thetac_out = thetac_in

* --- Errors in hints
*     SHOULD CORRESPOND TO DATA IN COMMON!
      errhint(1) = 1.*DEGRAD
      errhint(2) = 1. ! cm
      errhint(3) = 1. ! cm
      errhint(4) = 1. ! cm
      errhint(5) = 1. ! cm

* ... Call 5-parameter reconstruction

      call richlikemin5parxypart(thetac_out,x0m_out,y0m_out,
     +     x0v_out,y0v_out,errhint,vlike,ierr5p)

      if(ierr5p.eq.0) then ! successful 5-parameter reconstruction
         iflag_out = 1

#ifndef LIPRICHALONE
* ...... Get local ref index from global simulation
         call lipgetrad(x0v_out,y0v_out,0.,0.,0.,
     +                  amsrk,amsri,amsrt)
#else
* ...... Get local ref index from old method
         if(abs(x0v_out).lt.(0.5*3.*radtile_pitch).and.
     +        abs(y0v_out).lt.(0.5*3.*radtile_pitch)) then
            amsri = radix(2)    !NAF
         else
            amsri = radix(1)    !AGL
         endif
#endif
         beta_out = 1./(amsri*cos(thetac_out))
         x0_out = x0m_out-
     +            ZLGSIGNAL/(ZPMTDET+ZLGSIGNAL)*(x0m_out-x0v_out)
         y0_out = y0m_out-
     +            ZLGSIGNAL/(ZPMTDET+ZLGSIGNAL)*(y0m_out-y0v_out)
         theta_out = atan(sqrt((x0m_out-x0v_out)**2.+
     +               (y0m_out-y0v_out)**2.)/(ZPMTDET+ZLGSIGNAL))
         phi_out = atan2(y0m_out-y0v_out,x0m_out-x0v_out)
         if(phi_out.lt.0.) phi_out=phi_out+twopi

         chi2_out = chi2rec_fit/real(nbushits_fit)
         nuhits_out = nbushits_fit
         if(nuhits_out.gt.0) then
            do j=1,min(nbhits,nhitmax)
               hres_out(j) = chi2hit_fit(j)
               used_out(j) = -1
            enddo
            do k=1,nuhits_out
               phit_out(k) = ipushits_fit(k)
               used_out(ipushits_fit(k))
     +              = ireflechit_fit(ipushits_fit(k))
            enddo
         endif

         if(amsri<1.1) then
            ZEMI = HRAD*0.6  ! in the future replace 0.6 with formula
         else
            ZEMI = (HRAD-HRNAF)+HRNAF*0.5  ! in the future replace 0.5 with formula
         endif

         resvlike = vlike

      else
         iflag_out = 0
         beta_out = -999.
         thetac_out = -999.
         x0_out = -999.
         y0_out = -999.
         theta_out = -999.
         phi_out = -999.
         chi2_out = -999.
         nuhits_out = -999
         resvlike = -999.
      endif

* --- Filling of TOF common data

      iflag_rectof = iflag_out
      beta_rectof = beta_out
      thc_rectof = thetac_out

      if(iflag_rectof.eq.1) then
         pimp_rectof(1) = x0_out-ZPMTDET*tan(theta_out)*cos(phi_out)
         pimp_rectof(2) = y0_out-ZPMTDET*tan(theta_out)*sin(phi_out)
         pimp_rectof(3) = 0.
         epimp_rectof(1) = 0.
         epimp_rectof(2) = 0.
         epimp_rectof(3) = 0.
         pvtx_rectof(1) = pimp_rectof(1)+ZEMI*tan(theta_out)
     +        *cos(phi_out)
         pvtx_rectof(2) = pimp_rectof(2)+ZEMI*tan(theta_out)
     +        *sin(phi_out)
         pvtx_rectof(3) = ZEMI
         epvtx_rectof(1) = 0.
         epvtx_rectof(2) = 0.
         epvtx_rectof(3) = 0.
      else
         pimp_rectof(1) = -999.
         pimp_rectof(2) = -999.
         pimp_rectof(3) = -999.
         epimp_rectof(1) = -999.
         epimp_rectof(2) = -999.
         epimp_rectof(3) = -999.
         pvtx_rectof(1) = -999.
         pvtx_rectof(2) = -999.
         pvtx_rectof(3) = -999.
         epvtx_rectof(1) = -999.
         epvtx_rectof(2) = -999.
         epvtx_rectof(3) = -999.
      endif

      pthe_rectof = theta_out
      epthe_rectof = 0.
      pphi_rectof = phi_out
      if(pphi_rectof.gt.pi) then
         pphi_rectof = pphi_rectof-twopi
      endif
      if(pphi_rectof.lt.-pi) then
         pphi_rectof = pphi_rectof+twopi
      endif
      epphi_rectof = 0.

      chi2_rectof = chi2_out
      nuhits_rectof = nuhits_out
      if(nuhits_rectof.gt.0) then
         do k=1,nuhits_rectof
            phit_rectof(k) = phit_out(k)
         enddo
         do j=1,min(nbhits,nhitmax)
            used_rectof(j) = used_out(j)
            hres_rectof(j) = hres_out(j)
         enddo
         invchi2_rectof = 0.
         do k=1,nuhits_rectof
            if (hres_rectof(k).gt.CHI2HTCUT) then
               invchi2_rectof = invchi2_rectof+1./hres_rectof(k)
            endif
         enddo
      else
         invchi2_rectof = -999.
      endif


*      print*,'RESULT OF TOF RECONSTRUCTION'
*      print*,'----------------------------'
*      print*,'iflag_rectof = ',iflag_rectof
*      print*,'beta_rectof = ',beta_rectof
*      print*,'thc_rectof (deg) = ',thc_rectof*RADDEG
*      print*,'pimp_rectof = ',pimp_rectof
*      print*,'pvtx_rectof = ',pvtx_rectof
*      print*,'pthe_rectof (deg) = ',pthe_rectof*RADDEG
*      print*,'pphi_rectof (deg) = ',pphi_rectof*RADDEG
*      print*,'nuhits_rectof = ',nuhits_rectof
      return

      end


*********************************************************
*** RICHRECSTAND: reconstruction using RICH data only ***
*********************************************************

      subroutine richrecstand

#include "../include/richrec_lipf.h"

      real xmax,ymax,sigmax(5),fracmax,sigmed
      integer nhmax(5),imax(5)

      integer irshint
      real minsigquo
      integer ierris

      integer istrhit,istrhitpmt
      real strhitnpe,strhitpmtnpe
      real strpmtx,strpmty

* --- set data for minimization grid and hint selection

      dmtop = 50. ! 55. ! 17.  !  max x/y dist btw top and bottom
      ngridtop = 6         !  no. x/y grid points at top
      ngridbot = 3         !  no. x/y grid points at bottom
      steptop = 20.        ! 6.  ! (cm) x/y step at top
      stepbot = 1.         !  (cm) x/y step at bottom
      ngridthc = 5         !  no. points in thc
      fracminthc = 0.68    !  min. thc as fraction of thc(beta=1)
      herrthc = 1.*DEGRAD  ! 2.*DEGRAD !  initial perturbation in thc
      herrtop = 10.        ! 5. !  (cm) initial perturbation in top point
      herrbot = 1.         !  (cm) initial perturbation in bottom point

      ntestcand = 50       !  number of minimizations of candidate hints

* --- set reconstruction options
cc    RECBET(1)  = 11   ! unused
      RECBET(4)  = 1    ! minimum search with brent method

* ... phi hint mechanism
      IPHIHINTKIND = 0
      IPHIHINTFLAG = 0

* ... Minimum quotient between strongest and average PMT signals
      minsigquo = 3. !0. !3.

* ... Type of RICH hint to be used
      irshint = 1

* ... Locate particle signal in PMT matrix
      call richinitstand(irshint,minsigquo,ierris)

*      print*,'PCOOPMTOPTHINT = ',pcoopmtopthint

      if(ierris.eq.0) then ! good initialization
         call richgridstand(1)  ! generate preliminary hint grid
         call richgridstand(2)  ! perform minimization on selected hints
         call richgridstand(3)  ! get final result from minimization data
      else
         iflag_recstd = 0
         beta_recstd = -999.
         thc_recstd = -999.
         pimp_recstd(1) = -999.
         pimp_recstd(2) = -999.
         pimp_recstd(3) = -999.
         epimp_recstd(1) = -999.
         epimp_recstd(2) = -999.
         epimp_recstd(3) = -999.
         pvtx_recstd(1) = -999.
         pvtx_recstd(2) = -999.
         pvtx_recstd(3) = -999.
         epvtx_recstd(1) = -999.
         epvtx_recstd(2) = -999.
         epvtx_recstd(3) = -999.
         pthe_recstd = -999.
         epthe_recstd = -999.
         pphi_recstd = -999.
         epphi_recstd = -999.
         chi2_recstd = -999.
         nuhits_recstd = -999
         invchi2_recstd = -999.
      endif

*      print*,'RESULT OF 5-PAR RECONSTRUCTION'
*      print*,'------------------------------'
*      print*,'iflag_recstd = ',iflag_recstd
*      print*,'beta_recstd = ',beta_recstd
*      print*,'thc_recstd (deg) = ',thc_recstd*RADDEG
*      print*,'pimp_recstd = ',pimp_recstd
*      print*,'pvtx_recstd = ',pvtx_recstd
*      print*,'pthe_recstd (deg) = ',pthe_recstd*RADDEG
*      print*,'pphi_recstd (deg) = ',pphi_recstd*RADDEG
*      print*,'nuhits_recstd = ',nuhits_recstd

      return

      end


*************************************************************************************
*** RICHGRIDSTAND: new routine to process hint and minimization grid in 5-par rec ***
*************************************************************************************

      subroutine richgridstand(igridflag)

*=================================================
*   IGRIDFLAG (in) defines type of processing
*      1 - generate preliminary hint grid
*      2 - perform minimization on selected hints
*      3 - get final result from minimization data
*=================================================

#include "../include/richrec_lipf.h"

      integer currcand

      real x0m_cand,y0m_cand
      real x0v_cand,y0v_cand
      real thetac_cand

      real ztest,xrad_cand,yrad_cand,refind_cand

      integer amsrk
      double precision amsri
      integer amsrt

      integer auxcand(ncandmax)
      integer ibestcand
      integer irank

      real x0m_in,y0m_in
      real x0v_in,y0v_in
      real theta_in,phi_in,thetac_in

      real x0m_out,y0m_out
      real x0v_out,y0v_out
      real theta_out,phi_out,thetac_out
      real beta_out
      real chi2_out
      integer nuhits_out
      integer phit_out(nhitmax),used_out(nhitmax)
      real hres_out(nhitmax)

      real errhint(5)
      real vlike
      integer ierr5p

      integer nbestrec

      real vpar_cand(5)

      integer ierr_best
      real x0m_best,y0m_best
      real x0v_best,y0v_best
      real thetac_best
      real beta_best
      real vlike_best
      real chi2_best
      integer nuhits_best
      integer phit_best(nhitmax),used_best(nhitmax)
      real hres_best(nhitmax)

      real ZEMI

      if(igridflag.eq.1) then  ! generate preliminary hint grid

* ...... Reset candidate data
         do i=1,ncandmax
            candstatus(i) = 0

            candlike(i) = 1.e38
            candx0v(i) = -999.
            candy0v(i) = -999.
            candx0m(i) = -999.
            candy0m(i) = -999.
            candthc(i) = -999.
            canduhits(i) = -999
            candpkol(i) = -999.

            candreclike(i) = 1.e38
            candrecx0v(i) = -999.
            candrecy0v(i) = -999.
            candrecx0m(i) = -999.
            candrecy0m(i) = -999.
            candrecthc(i) = -999.
            candrecbeta(i) = -999.
            candrecuhits(i) = -999
            candrecpkol(i) = -999.

         enddo

         do i=1,ncandmax
            candrkhint(i) = 0
            candrkfit(i) = 0
         enddo

         currcand = 0

* ...... Cycle over candidate points in 5-dim space

*        print*,'HINT CYCLE'

         do ix=1,ngridtop
            do iy=1,ngridtop
               do ia=1,ngridbot
                  do ib=1,ngridbot
                     do iz=1,ngridthc

* ..................... Coordinates of candidate point

                        x0v_cand = pcoopmtopthint(1)
     +                       +steptop*(ix-0.5-ngridtop*0.5)
                        y0v_cand = pcoopmtopthint(2)
     +                       +steptop*(iy-0.5-ngridtop*0.5)

                        x0m_cand = pcoopmtopthint(1)
     +                       +stepbot*(ia-0.5-ngridbot*0.5)
                        y0m_cand = pcoopmtopthint(2)
     +                       +stepbot*(ib-0.5-ngridbot*0.5)

* ..................... determine radiator for candidate track
                        ZTEST = ZTARG+(HRAD-HRNAF)
                        xrad_cand = x0v_cand+(x0m_cand-x0v_cand)
     +                                       *ZTEST/(ZPMTDET+ZLGSIGNAL)
                        yrad_cand = y0v_cand+(y0m_cand-y0v_cand)
     +                                       *ZTEST/(ZPMTDET+ZLGSIGNAL)

#ifndef LIPRICHALONE
* ..................... New method using data from global simulation
                        call lipgetrad(xrad_cand,yrad_cand,ZTEST,0.,0.,
     +                                 amsrk,amsri,amsrt)
*                        print*,'(C) amsrk = ',amsrk

                        if(amsrk.eq.0) then ! no radiator
                           refind_cand = -999.
                           thetac_cand = -999.
                        endif

                        if(amsrk.eq.1) then ! aerogel
                           refind_cand = radix(1)
                           thetac_cand = acos(1./refind_cand)
     +                          *(fracminthc+(1.-fracminthc)*(iz-1.)
     +                          /(ngridthc-1.))
                        endif

                        if(amsrk.eq.2) then ! NaF
                           refind_cand = radix(2)
                           thetac_cand = acos(1./refind_cand)
     +                          *(fracminthc+(1.-fracminthc)*(iz-1.)
     +                          /(ngridthc-1.))
                        endif
#else
* ..................... Old method
                        if(abs(xrad_cand).lt.(0.5*3.*radtile_pitch)
     +                       .and.
     +                       abs(yrad_cand).lt.(0.5*3.*radtile_pitch))
     +                       then
                           refind_cand = radix(2) !NAF
                        else
                           refind_cand = radix(1) !AGL
                        endif

                        thetac_cand = acos(1./refind_cand)*(fracminthc
     +                       +(1.-fracminthc)*(iz-1.)/(ngridthc-1.))
#endif

* ..................... The following condition excludes combinations
*                       corresponding to very inclined tracks (x-y distance
*                       between RICH top and bottom greater than DMTOP)

                        if((x0v_cand-x0m_cand)**2.
     +                       +(y0v_cand-y0m_cand)**2..lt.dmtop**2.) then

* ------------------------ Call 5-parameter likelihood function

                           vpar_cand(1) = thetac_cand
                           vpar_cand(2) = x0m_cand
                           vpar_cand(3) = y0m_cand
                           vpar_cand(4) = x0v_cand
                           vpar_cand(5) = y0v_cand

* ........................ Tolerances set to infinity (this is not a minimization)
                           do i=1,5
                              vparin(i) = vpar_cand(i)
                              vparer(i) = 1.e37
                           enddo

                           vlike_cand = fliketot5parxypart(vpar_cand)
                           nuhits_cand = nbushits_fit

*                           print*,'Point',ix,iy,ia,ib,iz
*                           print*,'(',thetac_cand*RADDEG,x0m_cand,
*     +                          y0m_cand,x0v_cand,y0v_cand,
*     +                          ') ->',vlike_cand
*                           print*,' -----> HITS: ',nuhits_cand

                           if(vlike_cand.lt.1.e7) then ! good likelihood

* ........................... store hint data in array

                              currcand = currcand+1

                              candstatus(currcand) = 1
                              canduhits(currcand) = nuhits_cand
                              candlike(currcand) = vlike_cand
                              candx0v(currcand) = x0v_cand
                              candy0v(currcand) = y0v_cand
                              candx0m(currcand) = x0m_cand
                              candy0m(currcand) = y0m_cand
                              candthc(currcand) = thetac_cand

                           endif

                        endif

                     enddo
                  enddo
               enddo
            enddo
         enddo

* ...... sort candidate hints

         do i=1,ncandmax
            auxcand(i) = 0
         enddo

         irank = 0
         ibestcand = 100000  ! dummy, to bypass do-while

         do while(ibestcand.gt.0)  ! cycle while there are valid hints to find

            irank = irank+1
            ibestcand = 0

            do i=1,ncandmax
               if((candstatus(i).gt.0).and.(auxcand(i).eq.0)) then

                  if(ibestcand.eq.0) then ! first valid hint
                     ibestcand = i
                  else if((canduhits(i).gt.canduhits(ibestcand))
     +                    .or.((canduhits(i).eq.canduhits(ibestcand))
     +                    .and.(candlike(i).lt.candlike(ibestcand))))
     +                    then  ! ranking by hits then likelihood
*                  else if(candlike(i).lt.candlike(ibestcand)) then  ! ranking by likelihood

                     ibestcand = i

                  endif
               endif
            enddo

            candrkhint(irank) = ibestcand   ! add pointer to ranking list
            auxcand(ibestcand) = 1          ! exclude hint from next search

         enddo

* ...... histogram filling

         call hf1(-200001,canduhits(candrkhint(1))*1.,1.)
         call hf1(-200002,(canduhits(candrkhint(1))
     +        -canduhits(candrkhint(2)))*1.,1.)
         call hf2(-200003,canduhits(candrkhint(1))*1.,
     +        canduhits(candrkhint(2))*1.,1.)
         call hf1(-200004,candlike(candrkhint(2))
     +        -candlike(candrkhint(1)),1.)
         if(canduhits(candrkhint(1)).eq.canduhits(candrkhint(2))) then
            call hf1(-200005,candlike(candrkhint(2))
     +           -candlike(candrkhint(1)),1.)
         endif

         do i=2,ncandmax
            if(candlike(candrkhint(i)).lt.1.e30) then ! excludes failed reconstructions
               call hf1(-200006,(canduhits(candrkhint(1))
     +              -canduhits(candrkhint(i)))*1.,1.)
               call hf2(-200007,canduhits(candrkhint(1))*1.,
     +              canduhits(candrkhint(i))*1.,1.)
               call hf1(-200008,candlike(candrkhint(i))
     +              -candlike(candrkhint(1)),1.)
               if(canduhits(candrkhint(1))
     +              .eq.canduhits(candrkhint(i))) then
                  call hf1(-200009,candlike(candrkhint(i))
     +                 -candlike(candrkhint(1)),1.)
               endif
            endif
         enddo

      else if(igridflag.eq.2) then  ! perform minimization on selected hints

* ------ Minimization of selected hints

*         print*,'MINIMIZATION CYCLE'

c$$$         nbestrec = -999
c$$$
c$$$         ierr_best = 1
c$$$         beta_best = -999.
c$$$         thetac_best = -999.
c$$$         x0_best = -999.
c$$$         y0_best = -999.
c$$$         theta_best = -999.
c$$$         vlike_best = -999.
c$$$         phi_best = -999.
c$$$         chi2_best = -999.
c$$$         nuhits_best = -999

         do i=1,ntestcand

*            print*,'#',i,' -> ',candlike(candrkhint(i))

            if(candstatus(candrkhint(i)).gt.0) then ! excludes failed reconstructions

               x0v_in = candx0v(candrkhint(i))
               y0v_in = candy0v(candrkhint(i))
               x0m_in = candx0m(candrkhint(i))
               y0m_in = candy0m(candrkhint(i))
               thetac_in = candthc(candrkhint(i))

*               print*,' hint data: top(',x0v_in,',',y0v_in,
*     +              '),bot(',x0m_in,',',y0m_in,
*     +              '),thc(',thetac_in*RADDEG,'deg)'
*               print*,' ... HITS = ',canduhits(candrkhint(i))

               x0m_out = x0m_in
               y0m_out = y0m_in
               x0v_out = x0v_in
               y0v_out = y0v_in
               thetac_out = thetac_in

               errhint(1) = herrthc
               errhint(2) = herrbot
               errhint(3) = herrbot
               errhint(4) = herrtop
               errhint(5) = herrtop

* ............ Call 5-parameter reconstruction

               call richlikemin5parxypart(thetac_out,x0m_out,y0m_out,
     +              x0v_out,y0v_out,errhint,vlike,ierr5p)

               if(ierr5p.eq.0) then ! successful 5-parameter reconstruction

#ifndef LIPRICHALONE
* ............... Get local ref index from global simulation
                  call lipgetrad(x0v_out,y0v_out,0.,0.,0.,
     +                           amsrk,amsri,amsrt)
#else
* ............... Get local ref index from old method
                  if(abs(x0v_out).lt.(0.5*3.*radtile_pitch).and.
     +                 abs(y0v_out).lt.(0.5*3.*radtile_pitch)) then
                     amsri = radix(2) !NAF
                  else
                     amsri = radix(1) !AGL
                  endif
#endif
                  beta_out = 1./(amsri*cos(thetac_out))
                  x0_out = x0m_out-ZLGSIGNAL/(ZPMTDET+ZLGSIGNAL)
     +                 *(x0m_out-x0v_out)
                  y0_out = y0m_out-ZLGSIGNAL/(ZPMTDET+ZLGSIGNAL)
     +                 *(y0m_out-y0v_out)
                  theta_out = atan(sqrt((x0m_out-x0v_out)**2.
     +                 +(y0m_out-y0v_out)**2.)/(ZPMTDET+ZLGSIGNAL))
                  phi_out = atan2(y0m_out-y0v_out,x0m_out-x0v_out)
                  if(phi_out.lt.0.) phi_out=phi_out+twopi

                  chi2_out = chi2rec_fit/real(nbushits_fit)
                  nuhits_out = nbushits_fit
                  if(nuhits_out.gt.0) then
                     do j=1,min(nbhits,nhitmax)
                        hres_out(j) = chi2hit_fit(j)
                        used_out(j) = -1
                     enddo
                     do k=1,nuhits_out
                        phit_out(k) = ipushits_fit(k)
                        used_out(ipushits_fit(k))
     +                       = ireflechit_fit(ipushits_fit(k))
                     enddo
                  endif
                  dphistep=1.0
                  call richpkolm(dphistep,pk)  ! Kolmogorov probability

*                  print*,'  ... orig like = ',candlike(candrkhint(i)),
*     +                 ', final like = ',vlike,', used hits = ',
*     +                 nuhits_out,', x0,y0_out = ',x0_out,y0_out
*                  print*,'  --> PK = ',pk

                  candstatus(candrkhint(i)) = 2

                  candreclike(candrkhint(i)) = vlike
                  candrecx0v(candrkhint(i)) = x0v_out
                  candrecy0v(candrkhint(i)) = y0v_out
                  candrecx0m(candrkhint(i)) = x0m_out
                  candrecy0m(candrkhint(i)) = y0m_out
                  candrecthc(candrkhint(i)) = thetac_out
                  candrecbeta(candrkhint(i)) = beta_out
                  candrecuhits(candrkhint(i)) = nuhits_out
                  candrecpkol(candrkhint(i)) = pk

               else
                  beta_out = -999.
                  thetac_out = -999.
                  x0_out = -999.
                  y0_out = -999.
                  theta_out = -999.
                  phi_out = -999.
                  chi2_out = -999.
                  nuhits_out = -999
                  vlike = 1.e38
                  pk = -999.
*                  print*,'  ... orig like = ',candlike(candrkhint(i)),
*     +                 ', FAILED'
               endif

* ............ fill temporary arrays of standalone rec details

               if(i.le.50) then
                  creclike(i) = vlike
                  crecx0(i) = x0_out
                  crecy0(i) = y0_out
                  crectheta(i) = theta_out
                  crecphi(i) = phi_out
                  crecbeta(i) = beta_out
                  crecuhits(i) = nuhits_out
                  crecpkol(i) = pk
               endif

            endif

         enddo

* ...... sort candidate fits

         do i=1,ncandmax
            auxcand(i) = 0
         enddo

         irank = 0
         ibestcand = 100000  ! dummy, to bypass do-while

         do while(ibestcand.gt.0)  ! cycle while there are valid reconstructions to find

            irank = irank+1
            ibestcand = 0

            do i=1,ncandmax
               if((candstatus(i).eq.2).and.(auxcand(i).eq.0)) then

                  if(ibestcand.eq.0) then ! first valid fit
                     ibestcand = i
                  else if((candrecuhits(i).gt.candrecuhits(ibestcand))
     +                    .or.((candrecuhits(i)
     +                    .eq.candrecuhits(ibestcand))
     +                    .and.(candreclike(i)
     +                    .lt.candreclike(ibestcand))))
     +                    then  ! ranking by hits then likelihood
*                  else if(candreclike(i).lt.candreclike(ibestcand)) then  ! ranking by likelihood

                     ibestcand = i

                  endif
               endif
            enddo

            candrkfit(irank) = ibestcand   ! add pointer to ranking list
            auxcand(ibestcand) = 1         ! exclude fit from next search

         enddo

      else if(igridflag.eq.3) then  ! get final result from minimization data


* ...... Fill data for best reconstruction

         if(candrkfit(1).gt.0) then  ! at least one successful fit

            ierr_best = 0

* --------- Best minimization is repeated since this is the easiest way
*           to get hit data without storing hit data for all minimizations

            thetac_best = candthc(candrkfit(1))
            x0m_best = candx0m(candrkfit(1))
            y0m_best = candy0m(candrkfit(1))
            x0v_best = candx0v(candrkfit(1))
            y0v_best = candy0v(candrkfit(1))

            errhint(1) = herrthc
            errhint(2) = herrbot
            errhint(3) = herrbot
            errhint(4) = herrtop
            errhint(5) = herrtop

            call richlikemin5parxypart(thetac_best,x0m_best,y0m_best,
     +           x0v_best,y0v_best,errhint,vlike_best,ierr5p)

#ifndef LIPRICHALONE
* ......... Get local ref index from global simulation
            call lipgetrad(x0v_best,y0v_best,0.,0.,0.,
     +                     amsrk,amsri,amsrt)
#else
* ......... Get local ref index from old method
            if(abs(x0v_out).lt.(0.5*3.*radtile_pitch).and.
     +           abs(y0v_out).lt.(0.5*3.*radtile_pitch)) then
               amsri = radix(2) !NAF
            else
               amsri = radix(1) !AGL
            endif
#endif
            beta_best = 1./(amsri*cos(thetac_best))
            x0_best = x0m_best-ZLGSIGNAL/(ZPMTDET+ZLGSIGNAL)
     +           *(x0m_best-x0v_best)
            y0_best = y0m_best-ZLGSIGNAL/(ZPMTDET+ZLGSIGNAL)
     +           *(y0m_best-y0v_best)
            theta_best = atan(sqrt((x0m_best-x0v_best)**2.
     +           +(y0m_best-y0v_best)**2.)/(ZPMTDET+ZLGSIGNAL))
            phi_best = atan2(y0m_best-y0v_best,x0m_best-x0v_best)
            if(phi_best.lt.0.) phi_best=phi_best+twopi

            chi2_best = chi2rec_fit/real(nbushits_fit)
            nuhits_best = nbushits_fit
            if(nuhits_best.gt.0) then
               do j=1,min(nbhits,nhitmax)
                  hres_best(j) = chi2hit_fit(j)
                  used_best(j) = -1
               enddo
               do k=1,nuhits_best
                  phit_best(k) = ipushits_fit(k)
                  used_best(ipushits_fit(k))
     +                 = ireflechit_fit(ipushits_fit(k))
               enddo
            endif

            dphistep=1.0
            call richpkolm(dphistep,pk) ! Kolmogorov probability

            if(amsri<1.1) then
               ZEMI = HRAD*0.6  ! in the future replace 0.6 with formula
            else
               ZEMI = (HRAD-HRNAF)+HRNAF*0.5 ! in the future replace 0.5 with formula
            endif

         else

            ierr_best = 1
            beta_best = -999.
            thetac_best = -999.
            x0_best = -999.
            y0_best = -999.
            theta_best = -999.
            phi_best = -999.
            vlike_best = -999.
            chi2_best = -999.
            nuhits_best = -999

            ZEMI = -999.

         endif

* ...... histogram filling

         call hf1(-200011,nuhits_best*1.,1.)

* ...... find value of nbestrec
         do i=1,ncandmax
            if(candrkhint(i).eq.candrkfit(1)) then
               nbestrec = i
            endif
         enddo

         do i=1,ntestcand
            if(i.ne.nbestrec) then
               if(creclike(i).lt.1.e30) then ! excludes failed reconstructions
                  call hf1(-200016,(nuhits_best-crecuhits(i))*1.,1.)
                  call hf2(-200017,nuhits_best*1.,crecuhits(i)*1.,1.)
                  call hf1(-200018,creclike(i)-vlike_best,1.)
                  if(nuhits_best.eq.crecuhits(i)) then
                     call hf1(-200019,creclike(i)-vlike_best,1.)
                  endif

                  difphi = crecphi(i)-crecphi(nbestrec)
                  if(difphi.lt.-PI) difphi = difphi+TWOPI
                  if(difphi.gt.PI) difphi = difphi-TWOPI

                  crecxtopbest = crecx0(nbestrec)-ZPMTDET
     +                 *tan(crectheta(nbestrec))*cos(crecphi(nbestrec))
                  crecytopbest = crecy0(nbestrec)-ZPMTDET
     +                 *tan(crectheta(nbestrec))*sin(crecphi(nbestrec))

                  crecxtopi = crecx0(i)-ZPMTDET
     +                 *tan(crectheta(i))*cos(crecphi(i))
                  crecytopi = crecy0(i)-ZPMTDET
     +                 *tan(crectheta(i))*sin(crecphi(i))

                  call hf1(-200021,crecx0(i)-crecx0(nbestrec),1.)
                  call hf1(-200022,crecy0(i)-crecy0(nbestrec),1.)
                  call hf1(-200023,(crectheta(i)-crectheta(nbestrec))
     +                 *RADDEG,1.)
                  call hf1(-200024,difphi*RADDEG,1.)
                  call hf1(-200025,crecbeta(i)-crecbeta(nbestrec),1.)
                  call hf1(-200026,crecxtopi-crecxtopbest,1.)
                  call hf1(-200027,crecytopi-crecytopbest,1.)

                  if(nuhits_best.eq.crecuhits(i)) then
                     call hf1(-200031,crecx0(i)-crecx0(nbestrec),1.)
                     call hf1(-200032,crecy0(i)-crecy0(nbestrec),1.)
                     call hf1(-200033,(crectheta(i)-crectheta(nbestrec))
     +                    *RADDEG,1.)
                     call hf1(-200034,difphi*RADDEG,1.)
                     call hf1(-200035,crecbeta(i)-crecbeta(nbestrec),1.)
                     call hf1(-200036,crecxtopi-crecxtopbest,1.)
                     call hf1(-200037,crecytopi-crecytopbest,1.)
                  endif
               endif
            endif
         enddo

* ------ Filling of standalone common data

         iflag_recstd = 1-ierr_best
         beta_recstd = beta_best
         thc_recstd = thetac_best

         if(iflag_recstd.eq.1) then
            pimp_recstd(1) = x0_best-ZPMTDET*tan(theta_best)
     +           *cos(phi_best)
            pimp_recstd(2) = y0_best-ZPMTDET*tan(theta_best)
     +           *sin(phi_best)
            pimp_recstd(3) = 0.
            epimp_recstd(1) = 0.
            epimp_recstd(2) = 0.
            epimp_recstd(3) = 0.
            pvtx_recstd(1) = pimp_recstd(1)+ZEMI*tan(theta_best)
     +           *cos(phi_best)
            pvtx_recstd(2) = pimp_recstd(2)+ZEMI*tan(theta_best)
     +           *sin(phi_best)
            pvtx_recstd(3) = ZEMI
            epvtx_recstd(1) = 0.
            epvtx_recstd(2) = 0.
            epvtx_recstd(3) = 0.
         else
            pimp_recstd(1) = -999.
            pimp_recstd(2) = -999.
            pimp_recstd(3) = -999.
            epimp_recstd(1) = -999.
            epimp_recstd(2) = -999.
            epimp_recstd(3) = -999.
            pvtx_recstd(1) = -999.
            pvtx_recstd(2) = -999.
            pvtx_recstd(3) = -999.
            epvtx_recstd(1) = -999.
            epvtx_recstd(2) = -999.
            epvtx_recstd(3) = -999.
         endif

         pthe_recstd = theta_best
         epthe_recstd = 0.
         pphi_recstd = phi_best
         if(pphi_recstd.gt.pi) then
            pphi_recstd = pphi_recstd-twopi
         endif
         if(pphi_recstd.lt.-pi) then
            pphi_recstd = pphi_recstd+twopi
         endif
         epphi_recstd = 0.

         chi2_recstd = chi2_best
         nuhits_recstd = nuhits_best
         if(nuhits_recstd.gt.0) then
            do k=1,nuhits_recstd
               phit_recstd(k) = phit_best(k)
            enddo
            do j=1,min(nbhits,nhitmax)
               used_recstd(j) = used_best(j)
               hres_recstd(j) = hres_best(j)
            enddo
            invchi2_recstd = 0.
            do k=1,nuhits_recstd
               if (hres_recstd(k).gt.CHI2HTCUT) then
                  invchi2_recstd = invchi2_recstd+1./hres_recstd(k)
               endif
            enddo
         else
            invchi2_recstd = -999.
         endif

         resvlike = vlike_best

      else
         print*,'RICHGRIDSTAND: unknown option!'
      endif

      return

      end


*************************************************************
*** RICHINITSTAND: parameter initialization for 5-par rec ***
*************************************************************

      subroutine richinitstand(irshint,minsigquo,ierr)

*=================================================
*   IRSHINT (in) defines type of hint to be used:
*      1 - barycentre of strongest PMT
*      2 - strongest pixel in strongest PMT
*      3 - strongest pixel in matrix
*     99 - true impact point given by Tracker
*          (for debugging purposes)
*-------------------------------------------------
*   MINSIGQUO (in) is the minimum quotient between
*   strongest and average PMT signals needed
*   to proceed with reconstruction
*-------------------------------------------------
*   IERR (out) is the error flag:
*      0 - parameter initialization OK
*      1 - initialization failed
*          (MINSIGQUO limit not reached)
*=================================================

#include "../include/richrec_lipf.h"

      integer irshint
      real minsigquo
      integer ierr

      real xmax,ymax,sigmax(5),fracmax,sigmed
      integer nhmax(5),imax(5)

      integer istrhit,istrhitpmt
      real strhitnpe,strhitpmtnpe
      real strpmtx,strpmty

      real rectheta,recphi

      real recx0m,recy0m
      real recx0v,recy0v

* --- Find PMTs with strongest signals in detection matrix
      call getpmtmax(xmax,ymax,imax,sigmax,nhmax,fracmax,sigmed)

*      print*,'sigmax = ',sigmax(1),' ... sigmed = ',sigmed,
*     +     ' ... quotient = ',sigmax(1)/sigmed,' ... nhmax = ',nhmax(1)

      istrhit = -1
      strhitnpe = -1.e38

      istrhitpmt = -1
      strhitpmtnpe = -1.e38

      strpmtx = 0.
      strpmty = 0.
      strpmtnpe = 0.

* --- Filling of strongest hit data & strongest PMT barycentre
      do i=1,min(nbhits,nhitmax)
         if(hitsnpe(i).gt.strhitnpe) then
            istrhit = i
            strhitnpe = hitsnpe(i)
         endif
*         print*,'Hit ',i,', PMT ',1+hitspmt(i)/10/16
         if(1+hitspmt(i)/10/16.eq.imax(1)) then
            strpmtx = strpmtx+hitsnpe(i)*hitscoo(1,i)
            strpmty = strpmty+hitsnpe(i)*hitscoo(2,i)
            strpmtnpe = strpmtnpe+hitsnpe(i)
            if(hitsnpe(i).gt.strhitpmtnpe) then
               istrhitpmt = i
               strhitpmtnpe = hitsnpe(i)
            endif
         endif
      enddo
      strpmtx = strpmtx/strpmtnpe
      strpmty = strpmty/strpmtnpe

*      print*,' '
*      print*,'HINTS FOR IMPACT POINT'
*      print*,'----------------------'
*      print*,'PMT barycentre:    ',strpmtx,strpmty,' (PMT #',imax(1),
*     +     ', signal ',sigmax(1),')'
*      print*,'PMT strongest hit: ',xmax,ymax,
*     +     ' (signal ',strhitpmtnpe,')'
*      print*,'Strongest hit:     ',hitscoo(1,istrhit),
*     +     hitscoo(2,istrhit),' (signal ',strhitnpe,')'

      ierr = 0
      pcoopmtopthint(1) = -999.
      pcoopmtopthint(2) = -999.
      pcoopmtopthint(3) = -999.

      if(sigmax(1)/sigmed.gt.minsigquo) then
         if(irshint.eq.1) then  ! hint = barycentre of strongest PMT
            pcoopmtopthint(1) = strpmtx
            pcoopmtopthint(2) = strpmty
            pcoopmtopthint(3) = ZPMTDET+ZLGSIGNAL
         else if(irshint.eq.2) then  ! hint = strongest hit in strongest PMT
            pcoopmtopthint(1) = xmax
            pcoopmtopthint(2) = ymax
            pcoopmtopthint(3) = ZPMTDET+ZLGSIGNAL
         else if(irshint.eq.3) then  ! hint = strongest hit signal
            pcoopmtopthint(1) = hitscoo(1,istrhit)
            pcoopmtopthint(2) = hitscoo(2,istrhit)
            pcoopmtopthint(3) = ZPMTDET+ZLGSIGNAL
         else if(irshint.eq.99) then ! hint = impact point from Tracker
                                     !        (for debugging)
            pcoopmtopthint(1) = pimp(1)+(ZPMTDET+ZLGSIGNAL)
     +                                    *tan(pthe)*cos(pphi)
            pcoopmtopthint(2) = pimp(2)+(ZPMTDET+ZLGSIGNAL)
     +                                    *tan(pthe)*sin(pphi)
            pcoopmtopthint(3) = ZPMTDET+ZLGSIGNAL
         else
            ierr = 1
         endif
      else
         ierr = 1
      endif

      return

      end


*****************************************
*** CODE FROM richlikemin5parxypart.F ***
*****************************************

C -----------------------------------------------------------------
      subroutine richlikemin5parxypart(thetac,x0m,y0m,x0v,y0v,
     +                                 errhint,vlike,ierr)
C -----------------------------------------------------------------
*******************************************************************
* Steering routine for minimizing a likelihood function with
* 5 parameters free:
* - thetac
* - (x0m,y0m) of particle impact in PMT matrix
* - (x0v,y0v) of photon emission vertex
*
* IN/OUT :
*  thetac  = Cerenkov angle (rad)
*  x0m,y0m = matrix particle impact point at correct LG depth
*  x0v,y0v = top rad particle impact point
*
*  IN    :
*  errhint(5) = uncertainties for hint parameters
*               (1=thetac, 2=x0m, 3=y0m, 4=x0v, 5=y0v)
*
*  OUT   :
*   vlike = likelihood function value
*   ierr  = 0, normal
*         = 1, abnormal
*  -------------------------------------------------------------
*  by F. Barao, R. Pereira, June 2007
*******************************************************************

#include "../include/richrec_lipf.h"

      real thetac
      real x0m,y0m,x0v,y0v
      real errhint(5)
      real radius,theta,phi

      integer ndimval,mpval,npval
      parameter(ndimval=5,mpval=ndimval+1,npval=ndimval)
      real pmatrix(mpval,npval)
      real yvector(mpval)
      real ftol
      integer iters, ierr
      integer i

      real fliketot5parxypart, vpreal(5)
      external fliketot5parxypart

      character chradout*3

* ... exclude points with no radiator
      if(thetac.lt.0.) then
         vlike = 1.E10
         ierr = 1
*         print*,'EXCLUDED beg rlm5p'
         return
      endif

* ... init
      ierr = 0
*      print*,'(richlikemin5parxypart) Called'

* ... init common variables:
* ... fit parameters
      vparin(1) = thetac
      vparin(2) = x0m
      vparin(3) = y0m
      vparin(4) = x0v
      vparin(5) = y0v
* ... errors in parameters
      do i=1,5
         vparer(i) = errhint(i)
*         print*,'errhint(',i,') = ',errhint(i)
      enddo

* --> multidimensional points
      do i=1,mpval
         pmatrix(i,1) = thetac
         pmatrix(i,2) = x0m
         pmatrix(i,3) = y0m
         pmatrix(i,4) = x0v
         pmatrix(i,5) = y0v
      enddo
      if(vparer(1)<5.*DEGRAD) then
         pmatrix(2,1) = pmatrix(2,1)+vparer(1)  ! perturbation in thetac
      else
         pmatrix(2,1) = pmatrix(2,1)+5.*DEGRAD
      endif
      do i=2,5
         if(vparer(i)<10.) then
            pmatrix(i+1,i) = pmatrix(i+1,i)+vparer(i)  ! perturbation in point coords
         else
            pmatrix(i+1,i) = pmatrix(i+1,i)+10.
         endif
      enddo
* --> tolerance
      ftol = 1.e-3
* --> function values
      do i=1,mpval
         do j = 1,5 !parameters
           vpreal(j) = pmatrix(i,j)
         enddo
         yvector(i) = fliketot5parxypart(vpreal)
      enddo
* --> find minimum
      call amoeba(pmatrix,yvector,mpval,npval,ndimval,ftol,
     +            fliketot5parxypart,iters,ierr)
* --> converged?
*      print*,'nbushits_fit = ',nbushits_fit
      if (ierr.eq.0 .and. nbushits_fit.gt.0) then
* -----> return parameter values
         thetac = pmatrix(1,1)
         x0m    = pmatrix(1,2)
         y0m    = pmatrix(1,3)
         x0v    = pmatrix(1,4)
         y0v    = pmatrix(1,5)
         vlike  = yvector(1)
*         radius = sqrt((x0m-x0v)**2.+(y0m-y0v)**2.)
*         theta  = atan(radius/(ZPMTDET+ZLGSIGNAL))
*         phi    = atan2((y0m-y0v),(x0m-x0v))
*         if (phi.lt.0.) phi = phi + TWOPI
* -----> load rec commons
         ipthetac=1
         cangrec(ipthetac) = thetac
         do nn=1,nbushits_fit
            ipushits(ipthetac,nn) = ipushits_fit(nn)
            iflghit(ipushits_fit(nn)) = 2
         enddo
         nbushits(ipthetac) = nbushits_fit
         chi2rec(ipthetac)  = chi2rec_fit/real(nbushits_fit)
*         print*,'(rlm5pxyp) nbhits = ',nbhits
         do nn=1,min(nbhits,nhitmax)
            chi2hit(ipthetac,nn)    = chi2hit_fit(nn)
            phihit(ipthetac,nn)     = phihit_fit(nn)
            ireflechit(ipthetac,nn) = ireflechit_fit(nn)
            imsechit(ipthetac,nn)   = imsechit_fit(nn)
         enddo
*         print*,'(rlm5parxypart) Successful, yvector(1) = ',yvector(1)
      else
         vlike = 1.E10
         ierr = 1
*         print*,'(rlm5parxypart) Failed'
      endif

* ... end
      return
      end


**************************************
*** CODE FROM fliketot5parxypart.F ***
**************************************

**************************************************************************
      REAL FUNCTION FLIKETOT5PARXYPART(VPAR)
**************************************************************************
*
* Likelihood funtion to minimize;
* is function of the following parameters:
*
* Parameters: VPAR(1) = thetac
*             VPAR(2) = x coo of the particle impact point in the PMT matrix
*                       at the correct LG depth
*             VPAR(3) = y coo of the particle impact point in the PMT matrix
*                       at the correct LG depth
*             VPAR(4) = x coo of the particle impact point at radiator top
*             VPAR(5) = y coo of the particle impact point at radiator top
*
* NOTES:
*             VPAR(2,3) = PCOOPMTOPT(1,2) (see richtrk.inc)
*             VPAR(4,5) = PIMP(1,2) (see richtrk.inc)
*
**************************************************************************

#include "../include/richrec_lipf.h"

      real vpar(5), radius
      character chradout*3

      real distance
      real fact_weight

      real dinclmax
      PARAMETER(dinclmax=50.)

* ... exclude points with no radiator
      if(vpar(1).lt.0.) then
*         nbhits = 0
         nbushits_fit = 0
         fliketot5parxypart=1.E8
*         print*,'Excluded flt5p'
         return
      endif

* ... load track parameters
* --> get pmt matrix point depth
* --> set track direction
      radius = sqrt((vpar(2)-vpar(4))**2+(vpar(3)-vpar(5))**2)
      pthe   = atan2(radius,ZPMTDET+ZLGSIGNAL)
      pphi   = atan2((vpar(3)-vpar(5)),(vpar(2)-vpar(4)))
      if (pphi.lt.0.) pphi = pphi + twopi

* --> set impact point
      pimp(3) = 0.
      pimp(1) = real(vpar(4))
      pimp(2) = real(vpar(5))

      call richinitrec
      call richlikemin(-1)

* --> set pmt matrix impact point
      pcoopmt(3) = ZPMTDET
      pcoopmt(1) = real(vpar(2))-ZLGSIGNAL/(ZPMTDET+ZLGSIGNAL)
     +                          *(real(vpar(2))-real(vpar(4)))
      pcoopmt(2) = real(vpar(3))-ZLGSIGNAL/(ZPMTDET+ZLGSIGNAL)
     +                          *(real(vpar(3))-real(vpar(5)))
      pcoopmtradius = sqrt(pcoopmt(1)**2+pcoopmt(2)**2)

      pcoopmtopt(3) = ZPMTDET+ZLGSIGNAL
      pcoopmtopt(1) = real(vpar(2))
      pcoopmtopt(2) = real(vpar(3))
      pcoopmtoptradius = sqrt(pcoopmtopt(1)**2+pcoopmtopt(2)**2)

* ... update rotation matrix and load REC parameters
C      call patmatr

CCCCCC richsetradvtx appears unnecessary since richinitrec was called
C      call richsetradvtx(pimp,pthe,pphi,chradout) !pcervtx,...

* ... track/hit association
      call richtrkhitassoc

* ... likelihood
      thetac = vpar(1)
      fliketot5parxypart = fliketotall(thetac)
*      print*,'thetac = ',thetac
*      print*,'fliketot5parxypart (w/o extra) = ',fliketot5parxypart

* ... addition of terms for particle hits
*     (notice that using a subset of hits is not supported here)
      if(fliketot5parxypart.lt.1.E7) then
         do i=1,min(nbhits,nhitmax)
            if (iflghit(i).eq.1) then
               if (hitsnpe(i).lt.1.) then
                  fact_weight=1.
               else
                  fact_weight=hitsnpe(i)
               endif
               fliketot5parxypart = fliketot5parxypart
     +              -log(FBACKGR/FDISTAN)*fact_weight
            endif
         enddo
      endif
*      print*,'fliketot5parxypart (with extra) = ',fliketot5parxypart

*      print*,'pcoopmtopthint(1,2) = ',pcoopmtopthint(1),','
*     +     ,pcoopmtopthint(2)
*      print*,'vpar(1,2,3,4,5) = ',vpar(1)*RADDEG,'deg, ',vpar(2),', ',
*     +                            vpar(3),', ',vpar(4),', ',vpar(5)
*      print*,'fliketot5parxypart (bef dist) = ',fliketot5parxypart

* ... test on constraints for distance to hint (SHOULD NOT BE USED IN STANDALONE REC?)
      do i=1,5
         distance = abs(vpar(i)-vparin(i))
*         print*,'distance(',i,') = ',', vparer = ',vparer(i)
         if (distance.gt.3.*vparer(i)) fliketot5parxypart=1.E8
      enddo

* ... test on constraints for inclination
      dincl = ((vpar(2)-vpar(4))**2.+(vpar(3)-vpar(5))**2.)**0.5
      if(dincl.gt.DINCLMAX) fliketot5parxypart=1.E8

*      print*,'fliketot5parxypart (aft dist) = ',fliketot5parxypart

* ... end
      return
      end


**************************
*** CODE FROM amoeba.F ***
**************************

      SUBROUTINE amoeba(p,y,mp,np,ndim,ftol,funk,iter,ierr)
**************************************************************************
* Muldimensional minimum finding routine from numerical recipes
*
* IN : P(MP,NP)  = coordinates of 6 points in the muldimensional
*                  space (ndim)
*      Y(MP)     = function values in the coordinates
*      MP        = NP+1
*      NP        = NDIM
*      NDIM      = number of parameters of the function to minimize
*      FTOL      = tolerance (1.E-3 typically)
*      FUNK      = function to minimize
*
* OUT: ITER      = number of iteractions
*      IERR      = 0, normal end
*                = 1, number max of iterations reached
*
**************************************************************************
      INTEGER iter,mp,ndim,np,NMAX,ITMAX
      REAL ftol,p(mp,np),y(mp),funk,TINY
      PARAMETER (NMAX=20,ITMAX=5000,TINY=1.e-10)
      EXTERNAL funk
CU    USES amotry,funk
      INTEGER i,ihi,ilo,inhi,j,m,n
      REAL rtol,sum,swap,ysave,ytry,psum(NMAX),amotry
      integer ierr
      ierr=0
      iter=0
1     do 12 n=1,ndim
        sum=0.
        do 11 m=1,ndim+1
          sum=sum+p(m,n)
11      continue
        psum(n)=sum
12    continue
2     ilo=1
      if (y(1).gt.y(2)) then
        ihi=1
        inhi=2
      else
        ihi=2
        inhi=1
      endif
      do 13 i=1,ndim+1
        if(y(i).le.y(ilo)) ilo=i
        if(y(i).gt.y(ihi)) then
          inhi=ihi
          ihi=i
        else if(y(i).gt.y(inhi)) then
          if(i.ne.ihi) inhi=i
        endif
13    continue
      rtol=2.*abs(y(ihi)-y(ilo))/(abs(y(ihi))+abs(y(ilo))+TINY)
      if (rtol.lt.ftol) then
        swap=y(1)
        y(1)=y(ilo)
        y(ilo)=swap
        do 14 n=1,ndim
          swap=p(1,n)
          p(1,n)=p(ilo,n)
          p(ilo,n)=swap
14      continue
        return
      endif
      if (iter.ge.ITMAX) then
CC         pause 'ITMAX exceeded in amoeba'
*         print *,'(amoeba) ITMAX exceeded in amoeba'
         ierr = 1
         return
      endif
      iter=iter+2
      ytry=amotry(p,y,psum,mp,np,ndim,funk,ihi,-1.0)
      if (ytry.le.y(ilo)) then
        ytry=amotry(p,y,psum,mp,np,ndim,funk,ihi,2.0)
      else if (ytry.ge.y(inhi)) then
        ysave=y(ihi)
        ytry=amotry(p,y,psum,mp,np,ndim,funk,ihi,0.5)
        if (ytry.ge.ysave) then
          do 16 i=1,ndim+1
            if(i.ne.ilo)then
              do 15 j=1,ndim
                psum(j)=0.5*(p(i,j)+p(ilo,j))
                p(i,j)=psum(j)
15            continue
              y(i)=funk(psum)
            endif
16        continue
          iter=iter+ndim
          goto 1
        endif
      else
        iter=iter-1
      endif
      goto 2
      END


**************************
*** CODE FROM amotry.F ***
**************************

      FUNCTION amotry(p,y,psum,mp,np,ndim,funk,ihi,fac)
      INTEGER ihi,mp,ndim,np,NMAX
      REAL amotry,fac,p(mp,np),psum(np),y(mp),funk
      PARAMETER (NMAX=20)
      EXTERNAL funk
CU    USES funk
      INTEGER j
      REAL fac1,fac2,ytry,ptry(NMAX)
      fac1=(1.-fac)/ndim
      fac2=fac1-fac
      do 11 j=1,ndim
        ptry(j)=psum(j)*fac1-p(ihi,j)*fac2
11    continue
      ytry=funk(ptry)
      if (ytry.lt.y(ihi)) then
        y(ihi)=ytry
        do 12 j=1,ndim
          psum(j)=psum(j)-p(ihi,j)+ptry(j)
          p(ihi,j)=ptry(j)
12      continue
      endif
      amotry=ytry
      return
      END
