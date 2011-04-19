***********************************************************************
* In this file
* ------------
*      subroutine richchgrec(IEF)
*      subroutine richinitrechg()
*
***********************************************************************


******************************
*** CODE FROM richchgrec.F ***
******************************

      subroutine richchgrec(IEF)
****************************************************************
* Charge reconstruction (LIP)
*
* IN: IEF - Flag to control the efficiency calculation
*           = 10*PMT_Gap + Ring_Width
*             PMT_Gap =0,1 (pmt gap taken or not into account)
*             Ring_Width =0,1 (Ring width)
*
* 03/Feb/2009 (R. Pereira) - Code now gives chgrec=-1.
*                            if reconstruction failed
*
* 14/May/2009 (R. Pereira) - mirror sectors included
*
****************************************************************

* reconstruction:

#include "../include/richrec_lipf.h"

* ... local variables:

       integer ich,i
       integer ipartpat,irefpoint,ibranch
       real pthdeg,thetac,pathrad,phipoint,d2point

       real sin2thc_rec, chgrec2

       real richacca(5),richeffa(8)

       real richaccmseca(nmaxmirsec,2),richeffmseca(nmaxmirsec,8)

       real accradwalldir,accradwallref
       real accradwallmsec(nmaxmirsec)

       integer idummy
       SAVE idummy
       data idummy /0/
       !$OMP THREADPRIVATE (idummy)

* **********< INIT
       real d2minip,FNORM
       SAVE d2minip,FNORM
       data d2minip /25./
*       data FNORM /146.1/
       data FNORM /124.84/
       !$OMP THREADPRIVATE (d2minip)
       !$OMP THREADPRIVATE (FNORM)

* **********>

       real accgap, accgapdir, accgapref
       real accgapmsec(nmaxmirsec)

* ... INIT **************************************

* --> init local variables

      call vzero(richacca,5)
      call vzero(richeffa,8)

      call vzero(richaccmseca,nmaxmirsec*2)
      call vzero(richeffmseca,nmaxmirsec*8)

      richeff_pni  = -1.
      richeff_avz  = -1.

* ... reconstruction criteria
      if (ipthetac.eq.0)             return ! beta rec required
      if (nbushits(ipthetac).le.2)   return ! beta rec with at least 3 hits

* ... set cerenkov theta value
      thetac = cangrec(ipthetac)

* ... flag patterns too close to particle matrix crossing point
      ipartpat =   0
* --> compute distance between particle matrix impact point and closest pattern point
      call dist2patt(thetac,pcoopmt(1),pcoopmt(2),d2point,phipoint,
     &     irefpoint)
      if (d2point.lt.d2minip) ipartpat = 1

* ... count nb photoelectrons close to pattern
      do i=1,min(nbhits,nhitmax)
*         print*,'(richchgrec) Hit ',i,' has flag ',iflghit(i)
         if ( iflghit(i).ne.1 .and.                !hits not associated to particle
     +        sqrt(chi2hit(ipthetac,i)).lt.chgwin  !in the window
     +      ) then
*            print*,' >>> hit ',i,' included in charge'
            chg_nphe = chg_nphe + hitsnpe(i) ! ADC npe
            if (ireflechit(1,i).eq.1)
     +         chg_nphe_ref = chg_nphe_ref + hitsnpe(i) ! ADC npe reflected
         endif
      enddo
      chg_nphe_dir = chg_nphe - chg_nphe_ref     ! ADC npe (direct)

*      print*,'... chg_nphe = ',chg_nphe
*      print*,'... chg_nphe_dir = ',chg_nphe_dir
*      print*,'... chg_nphe_ref = ',chg_nphe_ref

* ... compute particle path length
      pathrad = hrad/cos(pthe)

* ... compute additional quantities for charge calculation
      sin2thc_rec = (sin(cangrec(ipthetac)))**2


* ... CLASSICAL ACCEPTANCE CALCULATION
* ... compute acceptances and efficiencies
      if(LEVACC.eq.0)then
         call richacc(thetac,richacca,richeffa,
     +        richaccmseca,richeffmseca)
      elseif(LEVACC.eq.1)then
* ...... compute acceptance taking into account ring width
         call richaccsmear(thetac,1,richacca,richeffa,
     +        richaccmseca,richeffmseca)  !charge=1 applied
      endif

*      print*,'In RICHCHGREC - pre-correction values:'
*      print*,'- richacca = ',(richacca(i),i=1,5)
*      print*,'- richeffa = ',(richeffa(i),i=1,8)
*      print*,'- richaccmseca = ',
*     +     ((richaccmseca(j,i),j=1,nmirsec),i=1,2)
*      print*,'- richeffmseca = ...'
*      do j=1,nmirsec
*         print*,(richeffmseca(j,i),i=1,8)
*      enddo

      valma1 = richacca(3)*reflec
      valma1a = richacca(3)*reflec+richacca(4)*reflec**2.
      valma2 = 0.
      do j=1,nmirsec
         valma2 = valma2+richaccmseca(j,1)*reflec
     +        +richaccmseca(j,2)*reflec**2.
      enddo

      valme1 = richeffa(7)
      valme1a = richeffa(7)+richeffa(8)
      valme2 = 0.
      valme2a = 0.
      do j=1,nmirsec
         valme2 = valme2+richeffmseca(j,7)
         valme2a = valme2a+richeffmseca(j,7)+richeffmseca(j,8)
      enddo

*      print*,'MIRROR COMPONENT - pre-correction:'
*      print*,'accep mir1, mir1+2 (global):',valma1,valma1a
*      print*,'accep mir1         (msecs) :',valma2
*      print*,'eff   mir1, mir1+2 (global):',valme1,valme1a
*      print*,'eff   mir1, mir1+2 (msecs) :',valme2,valme2a

* --- Take into account photon loss in radiator walls
      if (LEVGRAD.eq.2) then
         call richaccradwall(thetac,accradwalldir,accradwallref,
     +        accradwallmsec)

*         print*,'In RICHCHGREC - pre-correction values:'
*         print*,'- richacca = ',(richacca(i),i=1,5)
*         print*,'- richeffa = ',(richeffa(i),i=1,8)
*         print*,'- richaccmseca = ',
*     +        ((richaccmseca(j,i),j=1,nmirsec),i=1,2)
*         print*,'- richeffmseca = ...'
*         do j=1,nmirsec
*            print*,(richeffmseca(j,i),i=1,8)
*         enddo

         richeffa(6) = richeffa(6)*(1.-accradwalldir)
         richeffa(7) = 0.
         richeffa(8) = 0.
         richacca(2) = richacca(2)*(1.-accradwalldir)
         richacca(3) = 0.
         richacca(4) = 0.
         richacca(1) = 1.-richacca(2)
         do i=1,nmirsec
* ......... correct efficiencies using sector's reflectivity (REFMIRS)
*           since richacc always uses global value (REFLEC)
*           (minor issue: double reflections use value from same sector twice)
            richeffmseca(i,6) = 0.
            richeffmseca(i,7) = richeffmseca(i,7)*refmirs(i)/reflec
            richeffmseca(i,8) = richeffmseca(i,8)
     +           *(refmirs(i)/reflec)**2.
            richeffmseca(i,5) = richeffmseca(i,7)+richeffmseca(i,8)
* ......... conventional corrections, similar to global eff
            richeffmseca(i,5) = richeffmseca(i,5)
     +           -(richeffmseca(i,7)+richeffmseca(i,8))
     +           *accradwallmsec(i)
            richeffmseca(i,7) = richeffmseca(i,7)*(1.-accradwallmsec(i))
            richeffmseca(i,8) = richeffmseca(i,8)*(1.-accradwallmsec(i))
            richaccmseca(i,1) = richaccmseca(i,1)*(1.-accradwallmsec(i))
            richaccmseca(i,2) = richaccmseca(i,2)*(1.-accradwallmsec(i))
* ......... add values to global mirror acceptances and efficiencies
            richeffa(7) = richeffa(7)+richeffmseca(i,7)
     +           *reflec/refmirs(i)
            richeffa(8) = richeffa(8)+richeffmseca(i,8)
     +           *(reflec/refmirs(i))**2.
            richacca(3) = richacca(3)+richaccmseca(i,1)
            richacca(4) = richacca(4)+richaccmseca(i,2)
            richacca(1) = richacca(1)
     +           -richaccmseca(i,1)-richaccmseca(i,2)
         enddo
         richeffa(5) = richeffa(6)+richeffa(7)+richeffa(8)

      endif

cc      print*,'In RICHCHGREC - post-correction values:'
cc      print*,'- richacca = ',(richacca(i),i=1,5)
cc      print*,'- richeffa = ',(richeffa(i),i=1,8)
cc      print*,'- richaccmseca = ',
cc     +     ((richaccmseca(j,i),j=1,nmirsec),i=1,2)
cc      print*,'- richeffmseca = ...'
cc      do j=1,nmirsec
cc         print*,(richeffmseca(j,i),i=1,8)
cc      enddo

      valma1 = richacca(3)*reflec
      valma1a = richacca(3)*reflec+richacca(4)*reflec**2.
      valma2 = 0.
      do j=1,nmirsec
         valma2 = valma2+richaccmseca(j,1)*reflec
     +        +richaccmseca(j,2)*reflec**2.
      enddo

      valme1 = richeffa(7)
      valme1a = richeffa(7)+richeffa(8)
      valme2 = 0.
      valme2a = 0.
      do j=1,nmirsec
         valme2 = valme2+richeffmseca(j,7)
         valme2a = valme2a+richeffmseca(j,7)+richeffmseca(j,8)
      enddo

*      print*,'MIRROR COMPONENT - post-correction:'
*      print*,'accep mir1, mir1+2 (global):',valma1,valma1a
*      print*,'accep mir1         (msecs) :',valma2
*      print*,'eff   mir1, mir1+2 (global):',valme1,valme1a
*      print*,'eff   mir1, mir1+2 (msecs) :',valme2,valme2a

      if(ldebug.ge.2)then
         do i=1,8
            if(richeffa(i).lt.-0.00001) then
               print*,'STRANGE richeffa(',i,') = ',richeffa(i)
            endif
         enddo
         do j=1,nmirsec
            do i=1,8
               if(richeffmseca(j,i).lt.-0.00001) then
                  print*,'STRANGE richeffmseca(',j,',',i,') = ',
     +                 richeffmseca(j,i)
               endif
            enddo
         enddo
      endif

      richacc_inv  = richacca(1)   ! invisible acceptance
      richacc_dir  = richacca(2)   ! direct acceptance
      richacc_mir0 = richacca(3)   ! mirror acceptance 1ref
      richacc_mir1 = richacca(4)   ! mirror acceptance 2ref
      richacc_hol  = richacca(5)   ! hole acceptance
*
      richacc_vis = richacc_dir +
     +              reflec*richacc_mir0 +
     +              reflec**2*richacc_mir1
*
      richacc_ref = reflec*richacc_mir0 + reflec**2*richacc_mir1

      do i=1,nmaxmirsec
         richacc_msec0(i) = richaccmseca(i,1)  ! mirror sector acceptances
         richacc_msec1(i) = richaccmseca(i,2)  ! (1st, 2nd reflection)
      enddo

      richeff_pni = richeffa(1)       ! radiator eff
      richeff_rad = richeffa(2)       ! radiator eff * geom acceptance
      richeff_lg  = richeffa(3)       ! light guide efficiency
      richeff_avz = richeffa(4)       ! mean photon path
      richeff_tot = richeffa(5)       ! total efficiency
      richeff_dir = richeffa(6)       ! total efficiency direct
      richeff_1rf = richeffa(7)       ! total efficiency 1 reflection
      richeff_2rf = richeffa(8)       ! total efficiency 2 reflections

      do i=1,nmaxmirsec
         richeff_msec0(i) = richeffmseca(i,7)  ! mirror sector efficiency
         richeff_msec1(i) = richeffmseca(i,8)  ! (1st, 2nd reflection)
      enddo

* ... is a tiny acceptance event? if so no charge is reconstructed...
      if(richacc_vis.lt.0.01) then
         chgrec = -1.
         return
      endif

      iflagchg = 1
      iflagacc = 1

* ... acceptance asymetry
      assym_acc =(richacc_dir - richacc_mir0)/
     +           (richacc_dir + richacc_mir0)

*------------------------
* ... FLIGHT GEOMETRY
*------------------------
      if (rcgeom(1).eq.0) then
         call richaccpmtgap(accgap,accgapdir,accgapref,accgapmsec)
         richacc_gap    = accgap
         richacc_gapdir = accgapdir
         richacc_gapref = accgapref
         do i=1,nmaxmirsec
            richacc_gapmsec(i) = accgapmsec(i)
         enddo
      endif

*      print*,'pathrad     = ',pathrad
*      print*,'richeff_tot = ',richeff_tot
*      print*,'sin2thc_rec = ',sin2thc_rec

* ... compute reconstructed charge
      if ( ipartpat.eq.0 ) then         !is particle near the pattern?


       if (sin2thc_rec.eq.0.) return         !check thetac
* =====> overall charge
         if (richeff_tot.gt.0.) then
           chgrec2 = chg_nphe/pathrad/richeff_tot/sin2thc_rec ! from simulation ntuple
           chgrec  = sqrt(chgrec2/FNORM)
* ========> Gap correction for flight
           if (rcgeom(1).eq.0 .and.   ! Flight geometry
     +            accgap.gt.0) then   ! No null gap acceptance
              chgrec2 = chg_nphe/pathrad/richeff_tot/accgap/sin2thc_rec
              chgrec_gap = sqrt(chgrec2/FNORM)
           else
              chgrec_gap = -999.
           endif
        endif
*        print*,'after richeff_tot.gt.0.--> chgrec_cor',chgrec_cor

* =====> Direct charge
        if (richeff_dir.gt.0.) then
           chgrec2 = chg_nphe_dir/pathrad/richeff_dir/sin2thc_rec ! from simulation ntuple
           chgrec_dir = sqrt(chgrec2/FNORM)
        endif
* =====> Reflected charge
        if (richeff_1rf.gt.0.) then
           chgrec2 = chg_nphe_ref/pathrad/richeff_1rf/sin2thc_rec ! from simulation ntuple
           chgrec_ref = sqrt(chgrec2/FNORM)
        endif

      else   ! FOR PARTICLES CLOSE TO RING


        if (sin2thc_rec.eq.0.) return

        if (irefpoint.ne.0) then ! nearest point is reflected

           if (richeff_dir.gt.0.) then
              chgrec2 = chg_nphe_dir/pathrad/richeff_dir/sin2thc_rec ! from simulation ntuple
              chgrec_dir = sqrt(chgrec2/FNORM)
              chgrec     = chgrec_dir
           endif

        else

           if (richeff_1rf.gt.0.) then
              chgrec2 = chg_nphe_ref/pathrad/richeff_1rf/sin2thc_rec ! from simulation ntuple
              chgrec_ref = sqrt(chgrec2/FNORM)
              chgrec     = chgrec_ref
           endif

        endif

      endif

* ... charge determinatiomn with gap correction
      if ( rcgeom(1).eq.0) then ! Flight geometry

* ----->  direct
         if (accgapdir.gt.0. ) then ! No null gap direct acceptance
            if (chgrec_dir.gt.0.)
     +           chgrec_gapdir = chgrec_dir/sqrt(accgapdir)
         else
            chgrec_gapdir = -999.
         endif

*     ----->  reflected
         if (accgapref.gt.0. ) then ! No null gap reflected acceptance
             if (chgrec_ref.gt.0.)
     +           chgrec_gapref = chgrec_ref/sqrt(accgapref)
          else
             chgrec_gapref = -999.
          endif
       endif

* ... end
      return
      end


*********************************
*** CODE FROM richinitrechg.F ***
*********************************

      subroutine richinitrechg()

#include "../include/richrec_lipf.h"

* Inits charge variables

* ... init charge common variables (in richrec)

      iflagchg     = 0
      iflagacc     = 0

      chg_nphe     = 0.  !total npe
      chg_nphe_dir = 0.  !direct npe
      chg_nphe_ref = 0.  !reflected npe

      chgrec       = -999.
      chgrec_dir   = -999.
      chgrec_ref   = -999.

*
      chgrec_gap       = -999.
      chgrec_gapdir   = -999.
      chgrec_gapref   = -999.

      richacc_inv  = -1.
      richacc_dir  = -1.
      richacc_mir0 = -1.
      richacc_mir1 = -1.
      richacc_hol  = -1.
      richacc_vis  = -1.

      richeff_rad  = -1.
      richeff_lg   = -1.
      richeff_tot  = -1.
      richeff_dir  = -1.
      richeff_1rf  = -1.
      richeff_2rf  = -1.

      richeff_tot_gap  = -1.
      richeff_dir_gap  = -1.
      richeff_ref_gap  = -1.

      do i=1,nmaxmirsecc
         richacc_msec0(i) = -1.
         richacc_msec1(i) = -1.
         richacc_gapmsec(i) = -1.
         richeff_msec0(i) = -1.
         richeff_msec1(i) = -1.
         richeff_msec_gap(i) = -1.
      enddo

      a_beta = 0.126 !1E-2
      b_beta = 0.02  !1E-2
      return
      end
