#undef DEBUG
#undef LIP_HISTOS
#undef LIP_IODEBUG
*****************************************************************
* In this file
* ------------
*      subroutine richreclip(ievt,irec2do)
*      subroutine richinitrec
*      subroutine richtrkhitassoc
*
*****************************************************************

*****************************************************
*** RICHRECLIP REPLACES VERSION IN STANDALONE REC ***
*** (ROUTINE CALLING BETA, CHARGE RECS)           ***
*****************************************************

      subroutine richreclip(ievt,irec2do)
*****************************************************
* IN: ievt    = event number
*     irec2do = initialization or reconstruction to do
*                INITIALIZATIONS
*                   -1 = global (start of job)
*                   -2 = event data (start of event)
*                RECONSTRUCTIONS
*                 velocity only:
*                   1 = choose 2 or 3 from track error
*                   2 = beta rec with fixed track
*                   3 = beta rec with variable track
*                   4 = standalone beta rec
*                 velocity and charge:
*                  11 = choose 12 or 13 from track error
*                  12 = beta rec with fixed track
*                  13 = beta rec with variable track
*                  14 = standalone beta rec
*****************************************************

#include "../include/richrec_lipf.h"
#include "../include/richrec_lipc.h"

      integer ievt,irec2do

      real dphistep,probkol
      real flatsin,flatcos

      integer irecaux
      integer idobeta,idotof,idostd,idochg

      integer irecnumbini,irectot
      integer OMP_GET_THREAD_NUM,iThread


#ifdef LIP_HISTOS

* ... histogram booking

* LA 14Out10
      SAVE idummy,idumdisp

      data idummy/0/
      data idumdisp/0/
      !$OMP THREADPRIVATE (idummy)
      !$OMP THREADPRIVATE (idumdisp)

      if(idummy.eq.0)then

         call hbook1(-110001,'5-par rec, top x error (cm)',
     +        400,-100.,100.,0.)
         call hbook1(-110002,'5-par rec, top y error (cm)',
     +        400,-100.,100.,0.)
         call hbook1(-110003,'5-par rec, bottom x error (cm)',
     +        400,-10.,10.,0.)
         call hbook1(-110004,'5-par rec, bottom y error (cm)',
     +        400,-10.,10.,0.)
         call hbook1(-110005,'5-par rec, theta error (deg)',
     +        1000,-50.,50.,0.)
         call hbook1(-110006,'5-par rec, phi error (deg)',
     +        180,-180.,180.,0.)
         call hbook1(-110007,'5-par rec, thetac error (deg)',
     +        400,-20.,20.,0.)
         call hbook1(-110101,'5-par rec (6+ hits), top x error (cm)',
     +        400,-100.,100.,0.)
         call hbook1(-110102,'5-par rec (6+ hits), top y error (cm)',
     +        400,-100.,100.,0.)
         call hbook1(-110103,'5-par rec (6+ hits), bottom x error (cm)',
     +        400,-10.,10.,0.)
         call hbook1(-110104,'5-par rec (6+ hits), bottom y error (cm)',
     +        400,-10.,10.,0.)
         call hbook1(-110105,'5-par rec (6+ hits), theta error (deg)',
     +        1000,-50.,50.,0.)
         call hbook1(-110106,'5-par rec (6+ hits), phi error (deg)',
     +        180,-180.,180.,0.)
         call hbook1(-110107,'5-par rec (6+ hits), thetac error (deg)',
     +        400,-20.,20.,0.)

         call hbook1(-120001,'TOF rec, top x error (cm)',
     +        400,-100.,100.,0.)
         call hbook1(-120002,'TOF rec, top y error (cm)',
     +        400,-100.,100.,0.)
         call hbook1(-120003,'TOF rec, bottom x error (cm)',
     +        400,-10.,10.,0.)
         call hbook1(-120004,'TOF rec, bottom y error (cm)',
     +        400,-10.,10.,0.)
         call hbook1(-120005,'TOF rec, theta error (deg)',
     +        1000,-50.,50.,0.)
         call hbook1(-120006,'TOF rec, phi error (deg)',
     +        180,-180.,180.,0.)
         call hbook1(-120007,'TOF rec, thetac error (deg)',
     +        400,-20.,20.,0.)
         call hbook1(-120101,'TOF rec (6+ hits), top x error (cm)',
     +        400,-10.,10.,0.)
         call hbook1(-120102,'TOF rec (6+ hits), top y error (cm)',
     +        400,-10.,10.,0.)
         call hbook1(-120103,'TOF rec (6+ hits), bottom x error (cm)',
     +        400,-10.,10.,0.)
         call hbook1(-120104,'TOF rec (6+ hits), bottom y error (cm)',
     +        400,-10.,10.,0.)
         call hbook1(-120105,'TOF rec (6+ hits), theta error (deg)',
     +        1000,-50.,50.,0.)
         call hbook1(-120106,'TOF rec (6+ hits), phi error (deg)',
     +        180,-180.,180.,0.)
         call hbook1(-120107,'TOF rec (6+ hits), thetac error (deg)',
     +        400,-20.,20.,0.)

         call hbook1(-130010,'basic rec, beta error',
     +        400,-0.02,0.02,0.)
         call hbook1(-130110,'basic rec (6+ hits), beta error',
     +        400,-0.02,0.02,0.)

         call hbook2(-140000,'coords for point with no radiator',
     +        140,-70.,70.,140,-70.,70.,0.)
         call hbook2(-140001,'coords for point with aerogel',
     +        140,-70.,70.,140,-70.,70.,0.)
         call hbook2(-140002,'coords for point with NaF',
     +        140,-70.,70.,140,-70.,70.,0.)

         call hbook1(-150001,'entry Z, aerogel',40,-77.,-73.,0.)
         call hbook1(-150002,'entry Z, NaF',40,-77.,-73.,0.)

         call hbook1(-200001,'std cand hits, rank 1',31,-0.5,30.5,0.)
         call hbook1(-200002,'std cand hits, diff 1-2',31,-0.5,30.5,0.)
         call hbook2(-200003,'std cand hits, ranks 1-2',
     +        31,-0.5,30.5,31,-0.5,30.5,0.)
         call hbook1(-200004,'std cand like, diff 1-2 (all)',
     +        275,-5.,50.,0.)
         call hbook1(-200005,'std cand like, diff 1-2 (same hits)',
     +        100,0.,20.,0.)
         call hbook1(-200006,'std cand hits, diff 1-x',31,-0.5,30.5,0.)
         call hbook2(-200007,'std cand hits, ranks 1-x',
     +        31,-0.5,30.5,31,-0.5,30.5,0.)
         call hbook1(-200008,'std cand like, diff 1-x (all)',
     +        275,-5.,50.,0.)
         call hbook1(-200009,'std cand like, diff 1-x (same hits)',
     +        100,0.,20.,0.)

         call hbook1(-200011,'std rec hits, rank 1',31,-0.5,30.5,0.)
         call hbook1(-200016,'std rec hits, diff 1-x',31,-0.5,30.5,0.)
         call hbook2(-200017,'std rec hits, ranks 1-x',
     +        31,-0.5,30.5,31,-0.5,30.5,0.)
         call hbook1(-200018,'std rec like, diff 1-x (all)',
     +        275,-5.,50.,0.)
         call hbook1(-200019,'std rec like, diff 1-x (same hits)',
     +        100,0.,20.,0.)

         call hbook1(-200021,'std rec x0, diff 1-x (all)',
     +        200,-10.,10.,0.)
         call hbook1(-200022,'std rec y0, diff 1-x (all)',
     +        200,-10.,10.,0.)
         call hbook1(-200023,'std rec theta, diff 1-x(deg) (all)',
     +        160,-40.,40.,0.)
         call hbook1(-200024,'std rec phi, diff 1-x(deg) (all)',
     +        90,-180.,180.,0.)
         call hbook1(-200025,'std rec beta, diff 1-x (all)',
     +        120,-0.03,0.03,0.)
         call hbook1(-200026,'std rec xtop, diff 1-x (all)',
     +        200,-50.,50.,0.)
         call hbook1(-200027,'std rec ytop, diff 1-x (all)',
     +        200,-50.,50.,0.)

         call hbook1(-200031,'std rec x0, diff 1-x (same hits)',
     +        200,-10.,10.,0.)
         call hbook1(-200032,'std rec y0, diff 1-x (same hits)',
     +        200,-10.,10.,0.)
         call hbook1(-200033,'std rec theta, diff 1-x(deg) (same hits)',
     +        160,-40.,40.,0.)
         call hbook1(-200034,'std rec phi, diff 1-x(deg) (same hits)',
     +        90,-180.,180.,0.)
         call hbook1(-200035,'std rec beta, diff 1-x (same hits)',
     +        120,-0.03,0.03,0.)
         call hbook1(-200036,'std rec xtop, diff 1-x (same hits)',
     +        200,-50.,50.,0.)
         call hbook1(-200037,'std rec ytop, diff 1-x (same hits)',
     +        200,-50.,50.,0.)

* ...... Plots for Jun-2009 studies

         call hbook1(-300000,'acceptance (all agl)',
     +        200,-0.0025,1.0025,0.)

         call hbook1(-300001,'acc dir (all agl)',
     +        200,-0.0025,1.0025,0.)
         call hbook1(-300002,'acc 1st ref (all agl)',
     +        200,-0.0025,1.0025,0.)
         call hbook1(-300003,'acc 2nd ref (all agl)',
     +        200,-0.0025,1.0025,0.)

         call hbook1(-300011,'acc 1st ref (msec1) (all agl)',
     +        200,-0.0025,1.0025,0.)
         call hbook1(-300012,'acc 1st ref (msec2) (all agl)',
     +        200,-0.0025,1.0025,0.)
         call hbook1(-300013,'acc 1st ref (msec3) (all agl)',
     +        200,-0.0025,1.0025,0.)

         call hbook1(-300021,'acc 2nd ref (msec1) (all agl)',
     +        200,-0.0025,1.0025,0.)
         call hbook1(-300022,'acc 2nd ref (msec2) (all agl)',
     +        200,-0.0025,1.0025,0.)
         call hbook1(-300023,'acc 2nd ref (msec3) (all agl)',
     +        200,-0.0025,1.0025,0.)

         call hbook1(-300030,'rec beta (all agl)',
     +        1000,0.95,1.05,0.)

         call hbook1(-301000,'acceptance (sel agl)',
     +        200,-0.0025,1.0025,0.)

         call hbook1(-301001,'acc dir (sel agl)',
     +        200,-0.0025,1.0025,0.)
         call hbook1(-301002,'acc 1st ref (sel agl)',
     +        200,-0.0025,1.0025,0.)
         call hbook1(-301003,'acc 2nd ref (sel agl)',
     +        200,-0.0025,1.0025,0.)

         call hbook1(-301011,'acc 1st ref (msec1) (sel agl)',
     +        200,-0.0025,1.0025,0.)
         call hbook1(-301012,'acc 1st ref (msec2) (sel agl)',
     +        200,-0.0025,1.0025,0.)
         call hbook1(-301013,'acc 1st ref (msec3) (sel agl)',
     +        200,-0.0025,1.0025,0.)

         call hbook1(-301021,'acc 2nd ref (msec1) (sel agl)',
     +        200,-0.0025,1.0025,0.)
         call hbook1(-301022,'acc 2nd ref (msec2) (sel agl)',
     +        200,-0.0025,1.0025,0.)
         call hbook1(-301023,'acc 2nd ref (msec3) (sel agl)',
     +        200,-0.0025,1.0025,0.)

         call hbook1(-301030,'rec beta (sel agl)',
     +        1000,0.95,1.05,0.)

         call hbook1(-301050,'ring hits (sel agl)',
     +        51,-0.5,50.5,0.)
         call hbook1(-301051,'ring signal (sel agl)',
     +        501,-0.05,50.05,0.)

         call hbook2(-302000,'ly vs fref (sel agl)',
     +        501,-0.1,100.1,21,-0.025,1.025,0.)

         call hbook2(-302001,'ly vs fref (sel msec1 agl)',
     +        501,-0.1,100.1,21,-0.025,1.025,0.)
         call hbook2(-302002,'ly vs fref (sel msec2 agl)',
     +        501,-0.1,100.1,21,-0.025,1.025,0.)
         call hbook2(-302003,'ly vs fref (sel msec3 agl)',
     +        501,-0.1,100.1,21,-0.025,1.025,0.)

         idummy=1
      endif

#endif

C PROTECTION AGAINST EVENTS WITH TOO MANY HITS (OBSOLETE)
C
C      if(nbhits_ev.gt.nbhitsmax_ntup_ev) then
C         iThread = OMP_GET_THREAD_NUM()
C         print*,'MAX HITS EXCEEDED, RETURN: nbhits_ev,ievt,iThread = ',
C     +        nbhits_ev,ievt,iThread
C         return
C      endif

#ifdef LIP_IODEBUG

      iThread = OMP_GET_THREAD_NUM()
      print*,'iThread =',iThread
      print*,'INGOING VARIABLES (FULL) for event ',ievt
      print*,'jobc_cp_c2f        = ',jobc_cp_c2f
      print*,'ztoprad_ams_c2f    = ',ztoprad_ams_c2f
      print*,'rcgeom_c2f         = ',rcgeom_c2f
      print*,'levgeom_c2f        = ',levgeom_c2f
      print*,'levgrad_c2f        = ',levgrad_c2f
      print*,'levacc_c2f         = ',levacc_c2f
      print*,'levghit_c2f        = ',levghit_c2f
      print*,'ztarg_c2f          = ',ztarg_c2f
      print*,'nradts_c2f         = ',nradts_c2f
      print*,'hrad_c2f           = ',hrad_c2f
      print*,'hrnaf_c2f          = ',hrnaf_c2f
      print*,'radtile_pitch_c2f  = ',radtile_pitch_c2f
      print*,'radtile_supthk_c2f = ',radtile_supthk_c2f
      print*,'radix_c2f          = ',radix_c2f
      print*,'radclarity_c2f     = ',radclarity_c2f
      print*,'hpgl_c2f	         = ',hpgl_c2f	
      print*,'ztmirgap_c2f	 = ',ztmirgap_c2f	
      print*,'zbmirgap_c2f       = ',zbmirgap_c2f
      print*,'reflec_c2f	 = ',reflec_c2f	
      print*,'zlgsignal_c2f      = ',zlgsignal_c2f
      print*,'rtmir_c2f          = ',rtmir_c2f
      print*,'rbmir_c2f          = ',rbmir_c2f
      print*,'hmir_c2f           = ',hmir_c2f
      print*,'nmirsec_c2f        = ',nmirsec_c2f
      print*,'idmirs_c2f         = ',
     +     (idmirs_c2f(i),i=1,nmirsec_c2f)
      print*,'phimirs_c2f        = ',
     +     (phimirs_c2f(i),i=1,nmirsec_c2f)
      print*,'refmirs_c2f        = ',
     +     (refmirs_c2f(i),i=1,nmirsec_c2f)
      print*,'pmtwx_c2f          = ',pmtwx_c2f
      print*,'pmtwy_c2f          = ',pmtwy_c2f
      print*,'shieldw_c2f        = ',shieldw_c2f
      print*,'ipmtstat_c2f       = ',
     +     (ipmtstat(i),i=1,756)
      print*,'pglix_c2f          = ',pglix_c2f
      print*,'emcxlim_c2f        = ',emcxlim_c2f
      print*,'emcylim_c2f        = ',emcylim_c2f
      print*,'lg_top_width_c2f   = ',lg_top_width_c2f
      print*,'lg_bot_width_c2f   = ',lg_bot_width_c2f
      print*,'lg_pitch_c2f       = ',lg_pitch_c2f
      print*,'xbc_c2f            = ',xbc_c2f
      print*,'xbd_c2f            = ',xbd_c2f
      print*,'yef_c2f            = ',yef_c2f
      print*,'xpc_c2f            = ',xpc_c2f
      print*,'nbhits_ev          = ',nbhits_ev
      print*,'nbhitsmax_ntup_ev  = ',nbhitsmax_ntup_ev
      print*,'INGOING TRACK'
      print*,'pimp_main          = ',pimp_main
      print*,'pthe_main (deg)    = ',pthe_main*RADDEG
      print*,'pphi_main (deg)    = ',pphi_main*RADDEG

#endif

C     -------------------------------------------------------------------------
C     INIT geometry (once for all)
C     init liprec geom variables
C     -------------------------------------------------------------------------

      if (irec2do.eq.-1) then

#ifdef LIP_IODEBUG

         iThread = OMP_GET_THREAD_NUM()
         print*,'iThread =',iThread
         print*,'INGOING VARIABLES (global section) for event ',ievt
         print*,'jobc_cp_c2f        = ',jobc_cp_c2f
         print*,'ztoprad_ams_c2f    = ',ztoprad_ams_c2f
         print*,'rcgeom_c2f         = ',rcgeom_c2f
         print*,'levgeom_c2f        = ',levgeom_c2f
         print*,'levgrad_c2f        = ',levgrad_c2f
         print*,'levacc_c2f         = ',levacc_c2f
         print*,'levghit_c2f        = ',levghit_c2f
         print*,'ztarg_c2f          = ',ztarg_c2f
         print*,'nradts_c2f         = ',nradts_c2f
         print*,'hrad_c2f           = ',hrad_c2f
         print*,'hrnaf_c2f          = ',hrnaf_c2f
         print*,'radtile_pitch_c2f  = ',radtile_pitch_c2f
         print*,'radtile_supthk_c2f = ',radtile_supthk_c2f
         print*,'radix_c2f          = ',radix_c2f
         print*,'radclarity_c2f     = ',radclarity_c2f
         print*,'hpgl_c2f	         = ',hpgl_c2f	
         print*,'ztmirgap_c2f	 = ',ztmirgap_c2f	
         print*,'zbmirgap_c2f       = ',zbmirgap_c2f
         print*,'reflec_c2f	 = ',reflec_c2f	
         print*,'zlgsignal_c2f      = ',zlgsignal_c2f
         print*,'rtmir_c2f          = ',rtmir_c2f
         print*,'rbmir_c2f          = ',rbmir_c2f
         print*,'hmir_c2f           = ',hmir_c2f
         print*,'nmirsec_c2f        = ',nmirsec_c2f
         print*,'idmirs_c2f         = ',
     +       (idmirs_c2f(i),i=1,nmirsec_c2f)
         print*,'phimirs_c2f        = ',
     +       (phimirs_c2f(i),i=1,nmirsec_c2f)
         print*,'refmirs_c2f        = ',
     +       (refmirs_c2f(i),i=1,nmirsec_c2f)
         print*,'pmtwx_c2f          = ',pmtwx_c2f
         print*,'pmtwy_c2f          = ',pmtwy_c2f
         print*,'shieldw_c2f        = ',shieldw_c2f
         print*,'ipmtstat_c2f       = ',
     +       (ipmtstat(i),i=1,756)
         print*,'pglix_c2f          = ',pglix_c2f
         print*,'emcxlim_c2f        = ',emcxlim_c2f
         print*,'emcylim_c2f        = ',emcylim_c2f
         print*,'lg_top_width_c2f   = ',lg_top_width_c2f
         print*,'lg_bot_width_c2f   = ',lg_bot_width_c2f
         print*,'lg_pitch_c2f       = ',lg_pitch_c2f
         print*,'xbc_c2f            = ',xbc_c2f
         print*,'xbd_c2f            = ',xbd_c2f
         print*,'yef_c2f            = ',yef_c2f
         print*,'xpc_c2f            = ',xpc_c2f

#endif

* ------ store kind of data
         jobc_cp = jobc_cp_c2f

* ------ store detector layout
         rcgeom(1) = rcgeom_c2f(1)
         rcgeom(2) = rcgeom_c2f(2)

* ------ store geometry levels
         levgeom = levgeom_c2f
         levgrad = levgrad_c2f
         levacc = levacc_c2f
         levghit = levghit_c2f

* ------ define debug level
         ldebug = 0

* ------ store detector and geometry data
         ztarg          = ztarg_c2f
         nradts         = nradts_c2f
         hrad           = hrad_c2f
         radtile_pitch  = radtile_pitch_c2f
         radtile_supthk = radtile_supthk_c2f
         radix(1)       = radix_c2f(1)
         radix(2)       = radix_c2f(2)
         radclarity     = radclarity_c2f
         hpgl           = hpgl_c2f
         ztmirgap       = ztmirgap_c2f
         zbmirgap       = zbmirgap_c2f
         reflec         = reflec_c2f
         zlgsignal      = zlgsignal_c2f

         hrnaf    = hrnaf_c2f
         hmir     = hmir_c2f
         rbmir    = rbmir_c2f
         rtmir    = rtmir_c2f

         zpmtdet = hrad+hpgl+ztmirgap+hmir+zbmirgap

         tgmir    = (rbmir-rtmir)/hmir
         d0mir    = rtmir/(rbmir-rtmir)*hmir-hrad-hpgl-ZTMIRGAP
         vtmir(1) = 0.
         vtmir(2) = 0.
         vtmir(3) = -d0mir

         nmirsec  = nmirsec_c2f
         do i=1,nmirsec_c2f
            idmirs(i) = idmirs_c2f(i)
            phimirs(i) = phimirs_c2f(i)
            refmirs(i) = refmirs_c2f(i)
         enddo

         pmtwx    = pmtwx_c2f
         pmtwy    = pmtwy_c2f
         shieldw  = shieldw_c2f

         do i=1,756
            ipmtstat(i) = ipmtstat_c2f(i)
         enddo

         pglix    = pglix_c2f
         emcxlim  = emcxlim_c2f
         emcylim  = emcylim_c2f

         lg_bot_width = lg_bot_width_c2f
         lg_top_width = lg_top_width_c2f
         lg_pitch     = lg_pitch_c2f

         xbc = xbc_c2f
         xbd = xbd_c2f
         yef = yef_c2f
         xpc = xpc_c2f

         pitchx   = pmtwx+shieldw
         pitchy   = pmtwy+shieldw

         JUMP  = XBC - 8*PITCHX
         DMECX = XBD - 17*PITCHX - JUMP
         DMECY = YEF - 16*PITCHY
         DX0   = PITCHX/2. - XPC

* ------ store parameters for alternative velocity algorithm
         facthcminscanagl = facthcminscanagl_c2f
         facthcmaxscanagl = facthcmaxscanagl_c2f
         facthcminscannaf = facthcminscannaf_c2f
         facthcmaxscannaf = facthcmaxscannaf_c2f
         thcminscanagl    = thcminscanagl_c2f  
         thcmaxscanagl    = thcmaxscanagl_c2f  
         thcstepscanagl   = thcstepscanagl_c2f 
         thcminscannaf    = thcminscannaf_c2f  
         thcmaxscannaf    = thcmaxscannaf_c2f  
         thcstepscannaf   = thcstepscannaf_c2f 
         dvmaxagl         = dvmaxagl_c2f       
         dvmaxnaf         = dvmaxnaf_c2f       
         tollinagl        = tollinagl_c2f      
         tollinnaf        = tollinnaf_c2f

         return

      endif

C     -------------------------------------------------------------------------
C     INIT event variables
C     -------------------------------------------------------------------------

      if (irec2do.eq.-2) then

#ifdef LIP_IODEBUG

         iThread = OMP_GET_THREAD_NUM()
         print*,'iThread =',iThread
         print*,'INGOING VARIABLES (event section) for event ',ievt
         print*,'nbhits_ev          = ',nbhits_ev
         print*,'nbhitsmax_ntup_ev  = ',nbhitsmax_ntup_ev

#endif

* ------ store event number
         ievnumb = ievt

* ------ store hit data

         nbhits = 0
         i = 0

         do while(i.lt.min(nbhits_ev,nhitmax))
            i = i+1
*            print*,'iThread,ievt,i,nbhits_ev = ',
*     +           iThread,ievt,i,nbhits_ev
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
               hitstat(nbhits)     = hitstat_ev(i) ! hit status
               if(hitstat(nbhits).eq.0) then
                  iflghit(nbhits) = 0
               else
                  iflghit(nbhits) = 4
               endif
            endif
         enddo

         nbhitsmax_ntup = nbhitsmax_ntup_ev !max nb hits in ntuple

         return

      endif

C ------------------------------------------------------------------
C     REC CODE
C ------------------------------------------------------------------

#ifdef LIP_IODEBUG

      iThread = OMP_GET_THREAD_NUM()
      print*,'iThread =',iThread
      print*,'INGOING VARIABLES (track section) for event ',ievt
      print*,'INGOING TRACK'
      print*,'pimp_main          = ',pimp_main
      print*,'pthe_main (deg)    = ',pthe_main*RADDEG
      print*,'pphi_main (deg)    = ',pphi_main*RADDEG

#endif

* --- decode rec tasks

      idobeta = 0
      idotof = 0
      idostd = 0
      idochg = 0
      irecaux = irec2do

      if(irecaux.gt.10) then
         idochg = 1
         irecaux = irecaux-10
      endif

      if(irecaux.eq.1) then
         if(epimp_main(1).lt.1.e-4
     +        .and.epimp_main(2).lt.1.e-4
     +        .and.epimp_main(3).lt.1.e-4
     +        .and.epthe_main.lt.1.e-4
     +        .and.epphi_main.lt.1.e-4) then
            irecaux = 2
         else
            irecaux = 3
         endif
      endif

      if(irecaux.eq.5.or.irecaux.eq.6.or.irecaux.eq.7) then
         irecaux = 3
      endif

      if(irecaux.eq.2) then  ! alternative beta rec, optimized (now standard)
         idobeta = 3
      endif

      if(irecaux.eq.3) then
         idotof = 1
      endif

      if(irecaux.eq.4) then
         idostd = 1
      endif

      if(irecaux.eq.8) then  ! alternative beta rec, non-optimized
         idobeta = 2
      endif

      if(irecaux.eq.9) then  ! original beta rec
         idobeta = 1
      endif

* --- re-initialize track dependent parameters: radiator thickness, n, clarity
      hrad           = hrad_c2f
      radix(1)       = radix_c2f(1)
      radix(2)       = radix_c2f(2)
      radclarity     = radclarity_c2f

* --- initialize simulated beta
      pbeta = pbeta_sim

* --- set track parameters
      call ucopy(pimp_main,pimp,3)
      call ucopy(epimp_main,epimp,3)
      pthe = pthe_main
      epthe = epthe_main
      pphi = pphi_main
      epphi = epphi_main
      pmom = pmom_main

* --- extrapolated point at PMT matrix
      pcoopmt(1) = pimp(1)+tan(pthe)*cos(pphi)*(ZPMTDET-pimp(3))
      pcoopmt(2) = pimp(2)+tan(pthe)*sin(pphi)*(ZPMTDET-pimp(3))
      pcoopmt(3) = ZPMTDET
      pcoopmtradius = sqrt(pcoopmt(1)**2+pcoopmt(2)**2)

* --- corrected particle extrapolated point at PMT matrix
      pcoopmtopt(1) = pimp(1) +
     +     tan(pthe)*cos(pphi)*(ZPMTDET+ZLGSIGNAL-pimp(3))
      pcoopmtopt(2) = pimp(2) +
     +     tan(pthe)*sin(pphi)*(ZPMTDET+ZLGSIGNAL-pimp(3))
      pcoopmtopt(3) = ZPMTDET+ZLGSIGNAL
      pcoopmtoptradius = sqrt(pcoopmtopt(1)**2+pcoopmtopt(2)**2)

* --- set radiator parameters, photon vertex, init hit flags,
*     transformation matrix (particle frame --> rich frame)
      call richinitrec

* --- onther inits: radiator ID, etc
* ... AEROGEL RADIATOR **************************************
      if (chradid.eq.'AGL') then
* ------> interaction length
         /* clari   = usrcla     ! clarity : radint = lambda^4(micm)/clarity */
         alambda = 377.E-3 ! mean wavelength (micm) no foil (higher with foil)
         if (RADCLARITY.gt.0) then
            alambda = (0.0327*RADCLARITY**(1.-0.867))**0.25
            radint  = (alambda)**4/RADCLARITY
         else
            print*,'[richreclip] clarity not defined...',
     +             ' assumed radint=3cm!'
            radint = 3.
         endif
* -----> signal window integration
*         chgwin = 2.1  ! value now initialized in richinitrec
* ... NAF RADIATOR
       elseif(chradid.eq.'NAF')then
         radint = 1.E2
*         chgwin = 2.76  ! value now initialized in richinitrec
       endif

* --> init charge variables
      call richinitrechg

      if (jobc_cp.eq.0) then    ! (0=simulation, 1=real data)
         cerang = acos(1./refindex/pbeta)
      else
         cerang = -999.
      endif

* --- store current number of reconstructions
      irecnumbini = irecnumb

*******************************************
* --- conventional velocity reconstruction
*******************************************

      if(idobeta.eq.1 .or. idobeta.eq.2 .or. idobeta.eq.3) then

* ------ count one more reconstruction
         irecnumb = irecnumb+1

* ------ set track parameters
         call ucopy(pimp_main,pimp,3)
         pthe = pthe_main
         pphi = pphi_main
         pmom = pmom_main

* ------ extrapolated point at PMT matrix
         pcoopmt(1) = pimp(1)+tan(pthe)*cos(pphi)*(ZPMTDET-pimp(3))
         pcoopmt(2) = pimp(2)+tan(pthe)*sin(pphi)*(ZPMTDET-pimp(3))
         pcoopmt(3) = ZPMTDET
         pcoopmtradius = sqrt(pcoopmt(1)**2+pcoopmt(2)**2)

* ------ corrected particle extrapolated point at PMT matrix
         pcoopmtopt(1) = pimp(1) +
     +        tan(pthe)*cos(pphi)*(ZPMTDET+ZLGSIGNAL-pimp(3))
         pcoopmtopt(2) = pimp(2) +
     +        tan(pthe)*sin(pphi)*(ZPMTDET+ZLGSIGNAL-pimp(3))
         pcoopmtopt(3) = ZPMTDET+ZLGSIGNAL
         pcoopmtoptradius = sqrt(pcoopmtopt(1)**2+pcoopmtopt(2)**2)

* ------ set radiator parameters, photon vertex, init hit flags,
*        transformation matrix (particle frame --> rich frame)
         call richinitrec

* ------ track/hit association
         call richtrkhitassoc

* ------ beta recs
         if(idobeta.eq.1) then
            call richbetarec
         endif
         if(idobeta.eq.2) then
            call richbetarec_alt(1)
         endif
         if(idobeta.eq.3) then
            call richbetarec_alt(2)
         endif

* ------ Kolmogorov probability
         if (ipthetac.ne.0) then
            dphistep=1.0
            call richpkolm(dphistep,probkol)
            pkolmog(ipthetac) = probkol
         endif

* ------ flatness
*         chgwin = sqrt(chi2htcut)  ! value now initialized in richinitrec
         call flatness(1,chgwin,flatsin,flatcos)
         flatevt(1) = flatsin
         flatevt(2) = flatcos

* ------ filling of beta rec data

         if (ipthetac.eq.0) then
            resb_iflag(irecnumb+1) = 0
            resc_iflag(irecnumb+1) = 0
         endif

         if (ipthetac.gt.0) then

            if (irecnumb+1.le.NMAXLIPREC) then

               if(betarec.gt.0.5) then ! here cut at 0 is not good since failed recs give zero
                  resb_iflag(irecnumb+1) = 1
               else
                  resb_iflag(irecnumb+1) = 0
               endif

               resb_itype(irecnumb+1) = 2 ! number 2 = standard LIP rec
               resb_itrk(irecnumb+1) = itrknumb
               resb_beta(irecnumb+1) = betarec
               resb_thc(irecnumb+1) = cangrec(ipthetac)
               if(d2loglike.gt.1.e-5) then
                  resb_thcerr(irecnumb+1) = 1./sqrt(d2loglike)
               else
                  resb_thcerr(irecnumb+1) = -999.
               endif
               resb_chi2(irecnumb+1) = chi2rec(ipthetac)
               resb_like(irecnumb+1) = resvlike
               resb_d2like(irecnumb+1) = d2loglike
               resb_nhit(irecnumb+1) = nbushits(ipthetac)

               do j=1,min(nbhits,nhitmax)
                  resb_hres(j,irecnumb+1) = chi2hit(ipthetac,j)
                  resb_used(j,irecnumb+1) = -1
               enddo

               do k=1,nbushits(ipthetac)
                  resb_phit(k,irecnumb+1) = ipushits(ipthetac,k)
                  resb_used(ipushits(ipthetac,k),irecnumb+1) =
     +                 ireflechit(ipthetac,ipushits(ipthetac,k))
               enddo

               do j=1,min(nbhits,nhitmax)
                  if(iflghit(j).eq.4) then
                     resb_hres(j,irecnumb+1) = 1.e20
                     resb_used(j,irecnumb+1) = -2
                  endif
               enddo

               resb_invchi2(irecnumb+1) = 0.
               do k=1,nbushits(ipthetac)
                  if (chi2hit(ipthetac,k).gt.CHI2HTCUT) then
                     resb_invchi2(irecnumb+1) = resb_invchi2(irecnumb+1)
     +                    +1./chi2hit(ipthetac,k)
                  endif
               enddo

               resb_flatsin(irecnumb+1) = flatevt(1)
               resb_flatcos(irecnumb+1) = flatevt(2)
               resb_probkl(irecnumb+1) = pkolmog(ipthetac)

               do n=1,3
                  resb_pimp(n,irecnumb+1) = pimp(n)
                  resb_epimp(n,irecnumb+1) = 0.
                  resb_pvtx(n,irecnumb+1) = pcervtx(n)
                  resb_epvtx(n,irecnumb+1) = 0.
               enddo

               resb_pthe(irecnumb+1) = pthe
               resb_epthe(irecnumb+1) = 0.
               resb_pphi(irecnumb+1) = pphi
               resb_epphi(irecnumb+1) = 0.

               do i=1,50
                  rstd_creclike(i,irecnumb+1) = -999.
                  rstd_crecx0(i,irecnumb+1) = -999.
                  rstd_crecy0(i,irecnumb+1) = -999.
                  rstd_crectheta(i,irecnumb+1) = -999.
                  rstd_crecphi(i,irecnumb+1) = -999.
                  rstd_crecbeta(i,irecnumb+1) = -999.
                  rstd_crecuhits(i,irecnumb+1) = -999
                  rstd_crecpkol(i,irecnumb+1) = -999.
               enddo

            endif

**************************************
* --- charge reconstruction (beta rec)
**************************************

            if (idochg.eq.1) then

*     --------- call charge reconstruction
               call richchgrec

*     --------- charge probability
               if (chgrec.gt.0.) then

*     print*,'*** Charge rec'
*     print*,'    chgrec = ',chgrec

                  chgtest1 = nint(chgrec)-1.
                  chgtest2 = nint(chgrec)+0.
                  chgtest3 = nint(chgrec)+1.

*     print*,'chgtest1 = ',chgtest1
*     print*,'chgtest2 = ',chgtest2
*     print*,'chgtest3 = ',chgtest3

                  call probchg(chgtest1,0.2,chgrec,chgprob1)
                  call probchg(chgtest2,0.2,chgrec,chgprob2)
                  call probchg(chgtest3,0.2,chgrec,chgprob3)

*     print*,'chgprob1 = ',chgprob1
*               print*,'chgprob2 = ',chgprob2
*     print*,'chgprob3 = ',chgprob3

               endif

* --------- filling of charge rec data

               if (irecnumb+1.le.NMAXLIPREC) then

                  if(chgrec.gt.0.) then
                     resc_iflag(irecnumb+1) = 1
                  else
                     resc_iflag(irecnumb+1) = 0
                  endif

                  resc_cnpe(irecnumb+1) = chg_nphe
                  resc_cnpedir(irecnumb+1) = chg_nphe_dir
                  resc_cnperef(irecnumb+1) = chg_nphe_ref
                  resc_chg(irecnumb+1) = chgrec
                  resc_chgdir(irecnumb+1) = chgrec_dir
                  resc_chgmir(irecnumb+1) = chgrec_ref
                  resc_accgeom(1,irecnumb+1) = richacc_vis
                  resc_accgeom(2,irecnumb+1) = richacc_dir
                  resc_accgeom(3,irecnumb+1) = richacc_mir0
                  resc_eff(1,irecnumb+1) = richeff_tot
                  resc_eff(2,irecnumb+1) = richeff_dir
                  resc_eff(3,irecnumb+1) = richeff_1rf
                  resc_eff(4,irecnumb+1) = richeff_2rf
                  resc_eff(5,irecnumb+1) = richeff_rad
                  resc_eff(6,irecnumb+1) = richeff_lg
                  resc_arw(1,irecnumb+1) = 1.-richarwdir ! inverted convention
                  resc_arw(2,irecnumb+1) = 1.-richarwref ! inverted convention

*                  print*,'IRECNUMB = ',irecnumb

                  resc_nmirsec(irecnumb+1) = nmirsec
                  do i=1,nmaxmirsec
                     resc_accmsec(i,1,irecnumb+1)
     +                    = richacc_msec0(i)
                     resc_accmsec(i,2,irecnumb+1)
     +                    = richacc_msec1(i)
                     resc_effmsec(i,1,irecnumb+1)
     +                    = richeff_msec0(i)
                     resc_effmsec(i,2,irecnumb+1)
     +                    = richeff_msec1(i)
                     resc_arwmsec(i,irecnumb+1) = 1.-richarwmsec(i) ! inverted convention
                     if(i.le.nmirsec) then
*                        print*,'Sector ',i,':',
*     +                       ' accs = ',
*     +                       richacc_msec0(i),richacc_msec1(i),
*     +                       ', effs = ',
*     +                       richeff_msec0(i),richeff_msec1(i)
                     endif
                  enddo

                  resc_chgprob(1,irecnumb+1) = chgprob1
                  resc_chgprob(2,irecnumb+1) = chgprob2
                  resc_chgprob(3,irecnumb+1) = chgprob3

                  resc_nrseg(irecnumb+1) = nringseg_smear
                  do i=1,nringseg_smear
                     resc_pmtrseg(i,irecnumb+1) = ipmtringseg_smear(i)
                     resc_refrseg(i,irecnumb+1) = irefringseg_smear(i)
                     do j=1,3
                        resc_effrseg(j,i,irecnumb+1)
     +                       = effringseg_smear(i,j)
                     enddo
                  enddo

               endif

            else

               if(irecnumb+1.le.NMAXLIPREC) then
                  resc_iflag(irecnumb+1) = -1
               endif

            endif

         endif

      endif

*************************
* --- TOF reconstruction
*************************

      if(idotof.eq.1) then

* ------ count one more reconstruction
         irecnumb = irecnumb+1

* ------ set track parameters
         call ucopy(pimp_main,pimp,3)
         pthe = pthe_main
         pphi = pphi_main

* ------ call TOF reconstruction
         call richrecflex

* ------ Kolmogorov probability
         if (ipthetac.ne.0) then
            dphistep=1.0
            call richpkolm(dphistep,probkol)
            pkolmog(ipthetac) = probkol
         endif

* ------ flatness
*         chgwin = sqrt(chi2htcut)  ! value now initialized in richinitrec
         call flatness(1,chgwin,flatsin,flatcos)
         flatevt(1) = flatsin
         flatevt(2) = flatcos

* ------ filling of TOF rec data

         if(irecnumb+1.le.NMAXLIPREC) then

            resb_iflag(irecnumb+1) = iflag_rectof
            resb_itype(irecnumb+1) = 3  ! number 3 = TOF rec
            resb_itrk(irecnumb+1) = itrknumb
            resb_beta(irecnumb+1) = beta_rectof
            resb_thc(irecnumb+1) = thc_rectof
            if(d2loglike.gt.1.e-5) then
               resb_thcerr(irecnumb+1) = 1./sqrt(d2loglike)
            else
               resb_thcerr(irecnumb+1) = -999.
            endif
            resb_chi2(irecnumb+1) = chi2_rectof
            resb_like(irecnumb+1) = resvlike
            resb_d2like(irecnumb+1) = d2loglike
            resb_nhit(irecnumb+1) = nuhits_rectof
            do i=1,nhitmax
               resb_phit(i,irecnumb+1) = phit_rectof(i)
               resb_used(i,irecnumb+1) = used_rectof(i)
               resb_hres(i,irecnumb+1) = hres_rectof(i)
            enddo
            resb_invchi2(irecnumb+1) = invchi2_rectof
            resb_flatsin(irecnumb+1) = flatevt(1)
            resb_flatcos(irecnumb+1) = flatevt(2)
            resb_probkl(irecnumb+1) = pkolmog(ipthetac)
            do i=1,3
               resb_pimp(i,irecnumb+1) = pimp_rectof(i)
               resb_epimp(i,irecnumb+1) = epimp_rectof(i)
               resb_pvtx(i,irecnumb+1) = pvtx_rectof(i)
               resb_epvtx(i,irecnumb+1) = epvtx_rectof(i)
            enddo
            resb_pthe(irecnumb+1) = pthe_rectof
            resb_epthe(irecnumb+1) = epthe_rectof
            resb_pphi(irecnumb+1) = pphi_rectof
            resb_epphi(irecnumb+1) = epphi_rectof

            do i=1,50
               rstd_creclike(i,irecnumb+1) = -999.
               rstd_crecx0(i,irecnumb+1) = -999.
               rstd_crecy0(i,irecnumb+1) = -999.
               rstd_crectheta(i,irecnumb+1) = -999.
               rstd_crecphi(i,irecnumb+1) = -999.
               rstd_crecbeta(i,irecnumb+1) = -999.
               rstd_crecuhits(i,irecnumb+1) = -999
               rstd_crecpkol(i,irecnumb+1) = -999.
            enddo

         endif

*************************************
* --- charge reconstruction (TOF rec)
*************************************

         if(idochg.eq.1) then

* --------- call charge reconstruction
            call richchgrec

* --------- charge probability
            if(chgrec.gt.0.) then

*               print*,'*** Charge rec'
*               print*,'    chgrec = ',chgrec

               chgtest1 = nint(chgrec)-1.
               chgtest2 = nint(chgrec)+0.
               chgtest3 = nint(chgrec)+1.

*               print*,'chgtest1 = ',chgtest1
*               print*,'chgtest2 = ',chgtest2
*               print*,'chgtest3 = ',chgtest3

               call probchg(chgtest1,0.2,chgrec,chgprob1)
               call probchg(chgtest2,0.2,chgrec,chgprob2)
               call probchg(chgtest3,0.2,chgrec,chgprob3)

*               print*,'chgprob1 = ',chgprob1
*               print*,'chgprob2 = ',chgprob2
*               print*,'chgprob3 = ',chgprob3

            endif

* --------- filling of charge rec data

            if(irecnumb+1.le.NMAXLIPREC) then

               if(chgrec.gt.0.) then
                  resc_iflag(irecnumb+1) = 1
               else
                  resc_iflag(irecnumb+1) = 0
               endif

               resc_cnpe(irecnumb+1) = chg_nphe
               resc_cnpedir(irecnumb+1) = chg_nphe_dir
               resc_cnperef(irecnumb+1) = chg_nphe_ref
               resc_chg(irecnumb+1) = chgrec
               resc_chgdir(irecnumb+1) = chgrec_dir
               resc_chgmir(irecnumb+1) = chgrec_ref
               resc_accgeom(1,irecnumb+1) = richacc_vis
               resc_accgeom(2,irecnumb+1) = richacc_dir
               resc_accgeom(3,irecnumb+1) = richacc_mir0
               resc_eff(1,irecnumb+1) = richeff_tot
               resc_eff(2,irecnumb+1) = richeff_dir
               resc_eff(3,irecnumb+1) = richeff_1rf
               resc_eff(4,irecnumb+1) = richeff_2rf
               resc_eff(5,irecnumb+1) = richeff_rad
               resc_eff(6,irecnumb+1) = richeff_lg

*               print*,'IRECNUMB = ',irecnumb

               resc_nmirsec(irecnumb+1) = nmirsec
               do i=1,nmaxmirsec
                  resc_accmsec(i,1,irecnumb+1)
     +                 = richacc_msec0(i)
                  resc_accmsec(i,2,irecnumb+1)
     +                 = richacc_msec1(i)
                  resc_effmsec(i,1,irecnumb+1)
     +                 = richeff_msec0(i)
                  resc_effmsec(i,2,irecnumb+1)
     +                 = richeff_msec1(i)
                  if(i.le.nmirsec) then
*                     print*,'Sector ',i,':',
*     +                    ' accs = ',
*     +                    richacc_msec0(i),richacc_msec1(i),
*     +                    ', effs = ',
*     +                    richeff_msec0(i),richeff_msec1(i)
                  endif
               enddo

               resc_chgprob(1,irecnumb+1) = chgprob1
               resc_chgprob(2,irecnumb+1) = chgprob2
               resc_chgprob(3,irecnumb+1) = chgprob3

            endif

         else

            if(irecnumb+1.le.NMAXLIPREC) then
               resc_iflag(irecnumb+1) = -1
            endif

         endif

      endif

********************************
* --- standalone reconstruction
********************************

      if(idostd.eq.1) then

* ------ count one more reconstruction
         irecnumb = irecnumb+1

* ------ call standalone reconstruction
         call richrecstand

* ------ Kolmogorov probability
         if (ipthetac.ne.0) then
            dphistep=1.0
            call richpkolm(dphistep,probkol)
            pkolmog(ipthetac) = probkol
         endif

* ------ flatness
*         chgwin = sqrt(chi2htcut)  ! value now initialized in richinitrec
         call flatness(1,chgwin,flatsin,flatcos)
         flatevt(1) = flatsin
         flatevt(2) = flatcos

* ------ filling of standalone rec data

         if(irecnumb+1.le.NMAXLIPREC) then

            resb_iflag(irecnumb+1) = iflag_recstd
            resb_itype(irecnumb+1) = 4  ! number 4 = standalone rec
            resb_itrk(irecnumb+1) = itrknumb
            resb_beta(irecnumb+1) = beta_recstd
            resb_thc(irecnumb+1) = thc_recstd
            if(d2loglike.gt.1.e-5) then
               resb_thcerr(irecnumb+1) = 1./sqrt(d2loglike)
            else
               resb_thcerr(irecnumb+1) = -999.
            endif
            resb_chi2(irecnumb+1) = chi2_recstd
            resb_like(irecnumb+1) = resvlike
            resb_d2like(irecnumb+1) = d2loglike
            resb_nhit(irecnumb+1) = nuhits_recstd
            do i=1,nhitmax
               resb_phit(i,irecnumb+1) = phit_recstd(i)
               resb_used(i,irecnumb+1) = used_recstd(i)
               resb_hres(i,irecnumb+1) = hres_recstd(i)
            enddo
            resb_invchi2(irecnumb+1) = invchi2_recstd
            resb_flatsin(irecnumb+1) = flatevt(1)
            resb_flatcos(irecnumb+1) = flatevt(2)
            resb_probkl(irecnumb+1) = pkolmog(ipthetac)
            do i=1,3
               resb_pimp(i,irecnumb+1) = pimp_recstd(i)
               resb_epimp(i,irecnumb+1) = epimp_recstd(i)
               resb_pvtx(i,irecnumb+1) = pvtx_recstd(i)
               resb_epvtx(i,irecnumb+1) = epvtx_recstd(i)
            enddo
            resb_pthe(irecnumb+1) = pthe_recstd
            resb_epthe(irecnumb+1) = epthe_recstd
            resb_pphi(irecnumb+1) = pphi_recstd
            resb_epphi(irecnumb+1) = epphi_recstd

            do i=1,50
               rstd_creclike(i,irecnumb+1) = creclike(i)
               rstd_crecx0(i,irecnumb+1) = crecx0(i)
               rstd_crecy0(i,irecnumb+1) = crecy0(i)
               rstd_crectheta(i,irecnumb+1) = crectheta(i)
               rstd_crecphi(i,irecnumb+1) = crecphi(i)
               rstd_crecbeta(i,irecnumb+1) = crecbeta(i)
               rstd_crecuhits(i,irecnumb+1) = crecuhits(i)
               rstd_crecpkol(i,irecnumb+1) = crecpkol(i)
            enddo

         endif

********************************************
* --- charge reconstruction (standalone rec)
********************************************

         if(idochg.eq.1) then

*********** INITIALIZATION WITH STD DATA...

* --------- call charge reconstruction
            call richchgrec

* --------- charge probability
            if(chgrec.gt.0.) then

*               print*,'*** Charge rec'
*               print*,'    chgrec = ',chgrec

               chgtest1 = nint(chgrec)-1.
               chgtest2 = nint(chgrec)+0.
               chgtest3 = nint(chgrec)+1.

*               print*,'chgtest1 = ',chgtest1
*               print*,'chgtest2 = ',chgtest2
*               print*,'chgtest3 = ',chgtest3

               call probchg(chgtest1,0.2,chgrec,chgprob1)
               call probchg(chgtest2,0.2,chgrec,chgprob2)
               call probchg(chgtest3,0.2,chgrec,chgprob3)

*               print*,'chgprob1 = ',chgprob1
*               print*,'chgprob2 = ',chgprob2
*               print*,'chgprob3 = ',chgprob3

            endif

* --------- filling of charge rec data

            if(irecnumb+1.le.NMAXLIPREC) then

               if(chgrec.gt.0.) then
                  resc_iflag(irecnumb+1) = 1
               else
                  resc_iflag(irecnumb+1) = 0
               endif

               resc_cnpe(irecnumb+1) = chg_nphe
               resc_cnpedir(irecnumb+1) = chg_nphe_dir
               resc_cnperef(irecnumb+1) = chg_nphe_ref
               resc_chg(irecnumb+1) = chgrec
               resc_chgdir(irecnumb+1) = chgrec_dir
               resc_chgmir(irecnumb+1) = chgrec_ref
               resc_accgeom(1,irecnumb+1) = richacc_vis
               resc_accgeom(2,irecnumb+1) = richacc_dir
               resc_accgeom(3,irecnumb+1) = richacc_mir0
               resc_eff(1,irecnumb+1) = richeff_tot
               resc_eff(2,irecnumb+1) = richeff_dir
               resc_eff(3,irecnumb+1) = richeff_1rf
               resc_eff(4,irecnumb+1) = richeff_2rf
               resc_eff(5,irecnumb+1) = richeff_rad
               resc_eff(6,irecnumb+1) = richeff_lg

*               print*,'IRECNUMB = ',irecnumb

               resc_nmirsec(irecnumb+1) = nmirsec
               do i=1,nmaxmirsec
                  resc_accmsec(i,1,irecnumb+1)
     +                 = richacc_msec0(i)
                  resc_accmsec(i,2,irecnumb+1)
     +                 = richacc_msec1(i)
                  resc_effmsec(i,1,irecnumb+1)
     +                 = richeff_msec0(i)
                  resc_effmsec(i,2,irecnumb+1)
     +                 = richeff_msec1(i)
                  if(i.le.nmirsec) then
*                     print*,'Sector ',i,':',
*     +                    ' accs = ',
*     +                    richacc_msec0(i),richacc_msec1(i),
*     +                    ', effs = ',
*     +                    richeff_msec0(i),richeff_msec1(i)
                  endif
               enddo

               resc_chgprob(1,irecnumb+1) = chgprob1
               resc_chgprob(2,irecnumb+1) = chgprob2
               resc_chgprob(3,irecnumb+1) = chgprob3

            endif

         else

            if(irecnumb+1.le.NMAXLIPREC) then
               resc_iflag(irecnumb+1) = -1
            endif

         endif

      endif

* --- Total number of reconstructions performed
      irectot = irecnumb-irecnumbini

#ifdef LIP_IODEBUG

************
* --- OUTPUT
************

      print*,' '
      print*,'iThread =',iThread
      print*,'OUTGOING REC COUNT FOR EVENT ',ievt
      print*,'irecnumb    = ',irecnumb
      print*,'irecnumbini = ',irecnumbini
      print*,'irectot     = ',irectot

      do k=irecnumbini+2,irecnumb+1
         print*,' '
         print*,'iThread =',iThread
         print*,'OUTGOING VARIABLES FOR EVENT/RECONSTRUCTION ',ievt,k-1
         print*,'resb_iflag    = ',resb_iflag(k)
         print*,'resb_itype    = ',resb_itype(k)
         print*,'resb_itrk     = ',resb_itrk(k)
         print*,'resb_beta     = ',resb_beta(k)
         print*,'resb_thc (d)  = ',resb_thc(k)*RADDEG
         print*,'resb_thcerr(d)= ',resb_thcerr(k)*RADDEG
         print*,'resb_chi2     = ',resb_chi2(k)
         print*,'resb_like     = ',resb_like(k)
         print*,'resb_d2like   = ',resb_d2like(k)
         print*,'resb_nhit     = ',resb_nhit(k)
         print*,'resb_phit     = ',(resb_phit(i,k),i=1,resb_nhit(k))
         print*,'resb_used     = ',(resb_used(i,k),
     +        i=1,min(min(nbhits_ev,nbhitsmax_ntup),nhitmax))
         print*,'resb_hres     = ',(resb_hres(i,k),
     +        i=1,min(min(nbhits_ev,nbhitsmax_ntup),nhitmax))
         print*,'resb_invchi2  = ',resb_invchi2(k)
         print*,'resb_flatsin  = ',resb_flatsin(k)
         print*,'resb_flatcos  = ',resb_flatcos(k)
         print*,'resb_probkl   = ',resb_probkl(k)
         print*,'resc_iflag    = ',resc_iflag(k)
         print*,'resc_cnpe     = ',resc_cnpe(k)
         print*,'resc_cnpedir  = ',resc_cnpedir(k)
         print*,'resc_cnperef  = ',resc_cnperef(k)
         print*,'resc_chg      = ',resc_chg(k)
         print*,'resc_chgdir   = ',resc_chgdir(k)
         print*,'resc_chgmir   = ',resc_chgmir(k)
         print*,'resc_accgeom  = ',(resc_accgeom(i,k),i=1,3)
         print*,'resc_eff      = ',(resc_eff(i,k),i=1,6)
         print*,'resc_arw      = ',(resc_arw(i,k),i=1,2)
         print*,'resc_nmirsec  = ',resc_nmirsec(k)
         print*,'resc_accmsec  = ',
     +        ((resc_accmsec(i,j,k),i=1,resc_nmirsec(k)),j=1,2)
         print*,'resc_effmsec  = ',((resc_effmsec(i,j,k),i=1,3),j=1,2)
         print*,'resc_arwmsec  = ',(resc_arwmsec(i,k),i=1,3)
         print*,'resc_chgprob  = ',(resc_chgprob(i,k),i=1,3)
         print*,'resc_nrseg    = ',resc_nrseg(k)
         print*,'resc_pmtrseg  = ',(resc_pmtrseg(i,k),i=1,resc_nrseg(k))
         print*,'resc_refrseg  = ',(resc_refrseg(i,k),i=1,resc_nrseg(k))
         print*,'resc_effrseg  = ',
     +        ((resc_effrseg(i,j,k),i=1,3),j=1,resc_nrseg(k))
         print*,'resb_pimp     = ',(resb_pimp(i,k),i=1,3)
         print*,'resb_epimp    = ',(resb_epimp(i,k),i=1,3)
         print*,'resb_pvtx     = ',(resb_pvtx(i,k),i=1,3)
         print*,'resb_epvtx    = ',(resb_epvtx(i,k),i=1,3)
         print*,'resb_pthe(d)  = ',resb_pthe(k)*RADDEG
         print*,'resb_epthe(d) = ',resb_epthe(k)*RADDEG
         print*,'resb_pphi(d)  = ',resb_pphi(k)*RADDEG
         print*,'resb_epphi(d) = ',resb_epphi(k)*RADDEG
         print*,' '
      enddo

#endif


* --- FILL OBSOLETE VARIABLES FROM COMMON LIPVAR
* ... In IF statement, itype 2 = standard LIP rec

      do k=1,irectot
         if((resb_iflag(k).eq.1).and.(resb_itype(k).eq.2)) then
            nbhits_used = resb_nhit(k)
            thcrec = resb_thc(k)
            vbetarec = resb_beta(k)
            vebetarec = 0.
            likep = resb_like(k)
            chi2beta = resb_chi2(k)
            rprob = 0.
         endif
      enddo

*===============
* EVENT DISPLAY
*===============

* --- Event display
      if(idispflag.eq.1) then
         if(idumdisp.eq.0) then
            print*,'initializing display...'
            call richdsp(-1,' ')
            idumdisp=1
         endif
         call lipdisplay
      endif

*====================
* CONTROL HISTOGRAMS
*====================

#ifdef LIP_HISTOS

* --- filling of control histograms (standalone)
* ... In IF statement, itype 4 = standalone rec

      do k=1,irectot
         if((resb_iflag(k).eq.1).and.(resb_itype(k).eq.4)) then

            err_x0top = resb_pimp(i,k)-pimp_main(1)
            err_y0top = resb_pimp(i,k)-pimp_main(2)

            err_x0bot = (resb_pimp(1,k)
     +           +ZPMTDET*tan(resb_pthe(k))*cos(resb_pphi(k)))
     +           -(pimp_main(1)+ZPMTDET*tan(pthe_main)*cos(pphi_main))
            err_y0bot = (resb_pimp(2,k)
     +           +ZPMTDET*tan(resb_pthe(k))*sin(resb_pphi(k)))
     +           -(pimp_main(2)+ZPMTDET*tan(pthe_main)*sin(pphi_main))

            err_theta = resb_pthe(k)-pthe_main
            err_phi = resb_pphi(k)-pphi_main
            if(err_phi.lt.-pi) then
               err_phi = err_phi+twopi
            endif
            if(err_phi.gt.pi) then
               err_phi = err_phi-twopi
            endif

            err_thetac = resb_thc(k)-cerang

            call hf1(-110001,err_x0top,1.)
            call hf1(-110002,err_y0top,1.)
            call hf1(-110003,err_x0bot,1.)
            call hf1(-110004,err_y0bot,1.)
            call hf1(-110005,err_theta*RADDEG,1.)
            call hf1(-110006,err_phi*RADDEG,1.)
            call hf1(-110007,err_thetac*RADDEG,1.)

            if(resb_nhit(k).ge.6) then
               call hf1(-110101,err_x0top,1.)
               call hf1(-110102,err_y0top,1.)
               call hf1(-110103,err_x0bot,1.)
               call hf1(-110104,err_y0bot,1.)
               call hf1(-110105,err_theta*RADDEG,1.)
               call hf1(-110106,err_phi*RADDEG,1.)
               call hf1(-110107,err_thetac*RADDEG,1.)
            endif

         endif
      enddo

* --- filling of control histograms (TOF)
* ... In IF statement, itype 3 = standalone rec

      do k=1,irectot
         if((resb_iflag(k).eq.1).and.(resb_itype(k).eq.3)) then

            err_x0top = resb_pimp(1,k)-pimp_main(1)
            err_y0top = resb_pimp(2,k)-pimp_main(2)

            err_x0bot = (resb_pimp(1,k)
     +           +ZPMTDET*tan(resb_pthe(k))*cos(resb_pphi(k)))
     +           -(pimp_main(1)+ZPMTDET*tan(pthe_main)*cos(pphi_main))
            err_y0bot = (resb_pimp(2,k)
     +           +ZPMTDET*tan(resb_pthe(k))*sin(resb_pphi(k)))
     +           -(pimp_main(2)+ZPMTDET*tan(pthe_main)*sin(pphi_main))

            err_theta = resb_pthe(k)-pthe_main
            err_phi = resb_pphi(k)-pphi_main
            if(err_phi.lt.-pi) then
               err_phi = err_phi+twopi
            endif
            if(err_phi.gt.pi) then
               err_phi = err_phi-twopi
            endif

            err_thetac = resb_thc(k)-cerang

            call hf1(-120001,err_x0top,1.)
            call hf1(-120002,err_y0top,1.)
            call hf1(-120003,err_x0bot,1.)
            call hf1(-120004,err_y0bot,1.)
            call hf1(-120005,err_theta*RADDEG,1.)
            call hf1(-120006,err_phi*RADDEG,1.)
            call hf1(-120007,err_thetac*RADDEG,1.)

            if(resb_nhit(k).ge.6) then
               call hf1(-120101,err_x0top,1.)
               call hf1(-120102,err_y0top,1.)
               call hf1(-120103,err_x0bot,1.)
               call hf1(-120104,err_y0bot,1.)
               call hf1(-120105,err_theta*RADDEG,1.)
               call hf1(-120106,err_phi*RADDEG,1.)
               call hf1(-120107,err_thetac*RADDEG,1.)
            endif

         endif
      enddo

* --- filling of control histograms (beta)
* ... In IF statement, itype 2 = standard LIP rec

      do k=1,irectot
         if((resb_iflag(k).eq.1).and.(resb_itype(k).eq.2)) then

            err_beta = (resb_beta(k)-pbeta)/pbeta
*            print*,'resb_beta = ',resb_beta
*            print*,'pbeta = ',pbeta
*            print*,'err_beta = ',err_beta

            call hf1(-130010,err_beta,1.)

            if(resb_nhit(k).ge.6) then
               call hf1(-130110,err_beta,1.)
            endif

* ......... Plots for Jun-09 tests

            if(abs(resb_pimp(1,k)).gt.17.5
     +           .and.abs(resb_pimp(2,k)).gt.17.5
     +           .and.resc_accgeom(1,k).gt.0.) then ! exclude non-reconstructed evs and NaF

               call hf1(-300000,resc_accgeom(1,k),1.)

               call hf1(-300001,resc_accgeom(2,k),1.)
               call hf1(-300002,resc_accgeom(3,k),1.)
               call hf1(-300003,resc_accmsec(1,2,k)
     +              +resc_accmsec(2,2,k)+resc_accmsec(3,2,k),1.)

               call hf1(-300011,resc_accmsec(1,1,k),1.)
               call hf1(-300012,resc_accmsec(2,1,k),1.)
               call hf1(-300013,resc_accmsec(3,1,k),1.)

               call hf1(-300021,resc_accmsec(1,2,k),1.)
               call hf1(-300022,resc_accmsec(2,2,k),1.)
               call hf1(-300023,resc_accmsec(3,2,k),1.)

               call hf1(-300030,resb_beta(k),1.)

               if(resc_accgeom(1,k).gt.0.4
     +              .and.resb_nhit(k).ge.3
     +              .and.resb_beta(k).ge.0.99) then ! quality cut

                  call hf1(-301000,resc_accgeom(1,k),1.)

                  call hf1(-301001,resc_accgeom(2,k),1.)
                  call hf1(-301002,resc_accgeom(3,k),1.)
                  call hf1(-301003,resc_accmsec(1,2,k)
     +                 +resc_accmsec(2,2,k)+resc_accmsec(3,2,k),1.)

                  call hf1(-301011,resc_accmsec(1,1,k),1.)
                  call hf1(-301012,resc_accmsec(2,1,k),1.)
                  call hf1(-301013,resc_accmsec(3,1,k),1.)

                  call hf1(-301021,resc_accmsec(1,2,k),1.)
                  call hf1(-301022,resc_accmsec(2,2,k),1.)
                  call hf1(-301023,resc_accmsec(3,2,k),1.)

                  call hf1(-301030,resb_beta(k),1.)

                  call hf1(-301050,resb_nhit(k)*1.,1.)
                  call hf1(-301051,resc_cnpe(k),1.)

*                  print*,'DATA FOR HISTO -302000'
*                  print*,'resb_nhit(k)        = ',resb_nhit(k)
*                  print*,'resc_eff(1,k)       = ',resc_eff(1,k)
*                  print*,'resb_beta(k)        = ',resb_beta(k)
*                  print*,'resb_pthe(k)        = ',resb_pthe(k)
*                  print*,'resc_accgeom(2,k)   = ',resc_accgeom(2,k)
*                  print*,'resc_accgeom(3,k)   = ',resc_accgeom(3,k)
*                  print*,'resc_accmsec(1,1,k) = ',resc_accmsec(1,1,k)
*                  print*,'resc_accmsec(2,1,k) = ',resc_accmsec(2,1,k)
*                  print*,'resc_accmsec(3,1,k) = ',resc_accmsec(3,1,k)

                  call hf2(-302000,resb_nhit(k)*1.
     +                 /(resc_eff(1,k)/cos(resb_pthe(k))),
     +                 resc_accgeom(3,k)
     +                 /(resc_accgeom(2,k)+resc_accgeom(3,k)),1.)

                  do j=1,3
                     if((resc_accmsec(j,1,k).gt.0.0001)
     +                    .and.((resc_accgeom(3,k)
     +                    -resc_accmsec(j,1,k)).lt.0.0001)) then
                        call hf2(-302000-j,resb_nhit(k)*1.
     +                       /(resc_eff(1,k)/cos(resb_pthe(k))),
     +                       resc_accgeom(3,k)
     +                       /(resc_accgeom(2,k)+resc_accgeom(3,k)),1.)

                     endif
                  enddo

               endif

            endif

         endif
      enddo

#endif

* --- end of main LIP routine

      return

      end


*******************************
*** CODE FROM richinitrec.F ***
*******************************

*-------------------------------------------------------------------
      subroutine richinitrec
*-------------------------------------------------------------------
********************************************************************
*
* This routine shall be called before the cerenkov angle rec and it
* is used to set on an event by event basis:
*
* - the rotation matrix -> track data should be already loaded
* - the radiator parameters (CHRADID, REFINDEX)
* - the photon emission vertex (PCERVTX) -> track data should be already loaded
* - initialization of the hit flags
* - initialization of cerenkov angle rec boundaries
*
* Notes:
* - the track parameters (PIMP, PTHE, PPHI) are initialised on RECEVENT
* - the geometry is initialized in lipinit (once)
*
* Last Revision: 12/Fev/2007 (F.Barao)
********************************************************************

#include "../include/richrec_lipf.h"

      logical LEVTFAST

      integer rad_kind !1/0 = NaF/otherwise; -1 = none
      real ztest, pdum(3)

      integer amsrk
      double precision amsri
      integer amsrt

*      integer idummy
*     LA 14Out10
*      SAVE idummy
*      data idummy /0/
*      !$OMP THREADPRIVATE (idummy)

* --- init
      ipthetac = 0 !reset thetac rec pointer
C      call vzero(iflghit,nhitmax)

* --- photons Tracing Variables init  /Begin/
*     rotation matrix for loaded track was forced
      irotflg = 0              !reset rotation matrix flag
      call patmatr
      irotflg = 1

* --- set radiator parameters
      IF (RCGEOM(1).EQ.0) THEN !FLIGHT

* ...... in which radiator is the particle?
         ZTEST   = ZTARG+(HRAD-HRNAF)
         pdum(3) = ZTEST
         pdum(2) = pimp(2) + tan(pthe)*sin(pphi)*(ZTEST-pimp(3))
         pdum(1) = pimp(1) + tan(pthe)*cos(pphi)*(ZTEST-pimp(3))

* ------ Determination of radiator

* ...... Old method
         if (abs(pdum(1)).lt.(0.5*3.*radtile_pitch) .and.
     +       abs(pdum(2)).lt.(0.5*3.*radtile_pitch)) then
            rad_kind = 1        !NAF
         else
            rad_kind = 0        !AGL
         endif

      ELSEIF (RCGEOM(1).EQ.9) THEN !PROTOTYPE

         rad_kind = 0
         if (radix(1).gt.1.15) rad_kind = 1

      ENDIF

* --- set rad parameters
      if (rad_kind.eq.0) then !particle inciding on aerogel
         refindex = radix(1)
         chradid  = 'AGL'
         ZPHEMI   = 0.60
cc         call lipfzphemi(amsrt,ZPHEMI)
         ZTRIAL   = ZTARG+HRAD*ZPHEMI
      elseif (rad_kind.eq.1) then !particle inciding on NaF
         refindex = radix(2)
         chradid  = 'NAF'
         ZPHEMI   = 0.5
cc         call lipfzphemi(amsrt,ZPHEMI)
         ZTRIAL   = ZTARG+(HRAD-HRNAF)+HRNAF*ZPHEMI
      endif

* --- set photon vertex    
      pcervtx(3) = ZTRIAL    
      pcervtx(2) = pimp(2) + tan(pthe)*sin(pphi)*(ZTRIAL-pimp(3))
      pcervtx(1) = pimp(1) + tan(pthe)*cos(pphi)*(ZTRIAL-pimp(3))

* --- set thetac rec boundaries
      cangrecup  = acos(1./refindex)*1.10
#ifdef NVAR
      cangrecup  = acos(1./refindex)*1.50
#endif
      cangreclow = 2.0*DEGRAD

* --- set track hit distance for association
      trhitass2 = 5.**2 !cm

* --- set hit residue cut
      hitresmax = 20. !cm

* --- set event kind
      LEVTFAST = .FALSE.
      if (rcgeom(1).eq.0) then !flight
         if ( (pcoopmtradius.lt.50. .and. abs(refindex-1.03).lt.0.005)
     +        .or.
     +        (pcoopmtradius.lt.40. .and. abs(refindex-1.05).lt.0.005)
     +      ) LEVTFAST = .TRUE.
      elseif (rcgeom(1).eq.9) then !prototype
         if (rcgeom(2).ne.1) LEVTFAST = .TRUE. !no mirror
      endif

* --- set reconstruction parameters
      NSIGCUT = 4. ! nb sigmas to count fitted hits (currently unused)
      if (rcgeom(1).ne.9) then !-------------------------[ FLIGHT
         FDISTAN    =  134.      ! detector matrix dimension
         if (chradid.eq.'AGL') then
            PHISTEP   = 10.0      ! phi step for hit chi2 scanning
            if (LEVTFAST) then    ! thetac step for Likelihood F. scanning
               THCSTEP   =  2.5
            else
               THCSTEP   =  1.2
            endif
            F1SIGMA2  =  0.374**2 ! likelihood function width
            F2SIGMA2  =  1.348**2 ! likelihood function width
            F1NORMG   =  0.76     ! likelihood function width
            FBACKGR   =  0.776    ! background fraction
            CHI2HTCUT =  2.1**2   ! signal hits distance cut
* --------- single gaussian
C           FSIGMA2   =  0.45**2  ! likelihood function width
C           FBACKGR   =  0.949    ! background fraction
C           CHI2HTCUT =  0.598**2   ! signal hits distance cut
         elseif (chradid.eq.'NAF' .or. chradid.eq.'PGL') then
            PHISTEP   =  5.        ! phi step for hit chi2 scanning
            THCSTEP   =  1.5       ! thetac step for Likelihood F. scanning
            F1SIGMA2  =  0.5424**2 ! likelihood function width
            F2SIGMA2  =  1.3500**2 ! likelihood function width
            F1NORMG   =  0.4723    ! likelihood function width
            FBACKGR   =  0.2059    ! background fraction
*            CHI2HTCUT =  4.0**2    !signal hits distance cut
            CHI2HTCUT =  2.76**2    !signal hits distance cut
* --------- single gaussian
C            FSIGMA2   =  0.7**2 ! likelihood function width
         endif
      else !--------------------------------------[ PROTOTYPE
         FDISTAN    =  20.      ! detector matrix dimension
         if(rcgeom(2).eq.0) then !no mirror
            if (chradid.eq.'AGL') then
               PHISTEP   = 50.0     ! phi step for hit chi2 scanning
               THCSTEP   =  2.0     ! thetac step for Likelihood F. scanning
               F1NORMG   =  1.0     ! signal gaussian normalization
               F1SIGMA2  =  0.33**2 ! likelihood function width
               F2SIGMA2  =  0.33**2 ! dummy if F1NORMG=1
               FBACKGR   =  0.1943  ! background fraction
               CHI2HTCUT =  0.873**2
            elseif (chradid.eq.'NAF' .or. chradid.eq.'PGL') then
               PHISTEP   = 50.0     ! phi step for hit chi2 scanning
               THCSTEP   =  1.5     ! thetac step for Likelihood F. scanning
               F1NORMG   =  1.0     ! signal gaussian normalization
               F1SIGMA2  =  0.75**2 !
               F2SIGMA2  =  0.75**2 ! dummy if F1NORMG=1
               FBACKGR   =  0.10     ! background fraction
               CHI2HTCUT =  2.18**2
            endif
         else                   ! PROTOTYPE TESTBEAM 2003 (MIRROR)
            IF (chradid.eq.'AGL') then
               PHISTEP   = 10.0      ! phi step for hit chi2 scanning
               THCSTEP   =  1.0      ! thetac step for Likelihood F. scanning
               F1NORMG   =  1.0      ! signal gaussian normalization
               F1SIGMA2  =  0.25**2  ! likelihood function width
               F2SIGMA2  =  0.25**2  ! dummy if F1NORMG=1
               FBACKGR   =  0.689    ! background fraction
               CHI2HTCUT =  0.873**2
            elseif (chradid.eq.'NAF' .or. chradid.eq.'PGL') then
               PHISTEP   =  5.0      ! phi step for hit chi2 scanning
               THCSTEP   =  1.5      ! thetac step for Likelihood F. scanning
               F1NORMG   =  1.0      ! signal gaussian normalization
               F1SIGMA2  =  0.75**2  ! lorentz function width
               F2SIGMA2  =  0.75**2  ! dummy if F1NORMG=1
               FBACKGR   =  0.10      ! background fraction
               CHI2HTCUT =  2.18**2
            endif
         endif
      endif

      chgwin = sqrt(chi2htcut)  ! now done exclusively here

* --- end
      return
      end





***********************************
*** CODE FROM richtrkhitassoc.F ***
***********************************

      subroutine richtrkhitassoc
************************************************************
*
* Performs track-hit association
* Hits associated to track have iflghit(i)=1
*
*************************************************************

#include "../include/richrec_lipf.h"

      real difextrap2

* --- init
      nbhits_nass = 0

      do i=1,min(min(nbhits,nbhitsmax_ntup),nhitmax)
         difextrap2 = (hitscoo(1,i)-(pcoopmt(1)
     +        +ZLGSIGNAL*atan(pthe)*cos(pphi)))**2 +
     +        (hitscoo(2,i)-(pcoopmt(2)
     +        +ZLGSIGNAL*atan(pthe)*sin(pphi)))**2
*         print*,'... hit ',i,' - difextrap2 = ',difextrap2
         if (difextrap2.lt.TRHITASS2 .and. iflghit(i).eq.0) then
            iflghit(i) = 1
         else
C            iflghit(i) = 0
            nbhits_nass = nbhits_nass + 1
            if (nbhits_nass.le.nhitmax) ip_nass(nbhits_nass) = i
         endif
      enddo

* --- end
      return
      end
