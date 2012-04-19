***********************************************************************
* In this file
* ------------
*      subroutine lipdisplay()
*      SUBROUTINE RICHDSP(IFLG,CHOPT)
*      subroutine rdisplay
*     +     (CHOPT,CHCOL,thc,parthe,parphi,pvertex,xcoopmt,ycoopmt)
*      subroutine color(itask,chcol,icol)
*      SUBROUTINE PHOTRACE(thetac,parthe,parphi,pvertex,ICODL)
*      SUBROUTINE PHOTRACE_PROT
*     ,     (thetac,parthe,parphi,pvertex,ICODL)
*      SUBROUTINE PHOTRACE_RAD(thetac,parthe,parphi,pvertex,ICODL)
*      subroutine patrace_rad
*     ,     (phig,cerang,pthe,pphi,pvertex,xdet,ydet,nmiref,nerr,verr)
*      subroutine rdisdrawlike(CHCOL,CHOPT)
*
***********************************************************************


********************************
*** CODE BASED ON recevent.F ***
********************************

* -------------------------------------------------------------
      subroutine lipdisplay()
* -------------------------------------------------------------
****************************************************************
* RICH event display (LIP)
****************************************************************

#include "../include/richrec_lipf.h"

      real betadisp,thetac, ptrkthe, ptrkphi,
     +     pcervtx_d(3), pimp_d(3)
      real pdum(3)


* --- CERENKOV REC

      if (ipthetac.gt.0) then
         betadisp = betarec
         thetac   = cangrec(ipthetac)
      else
         betadisp=-999.
         thetac = -999.
      endif

* --- particle

      ptrkthe = pthe
      ptrkphi = pphi
      pcervtx_d(1) = pcervtx(1)
      pcervtx_d(2) = pcervtx(2)
      pcervtx_d(3) = pcervtx(3)
      pimp_d(1) = pimp(1)
      pimp_d(2) = pimp(2)
      pimp_d(3) = pimp(3)

* --- draw detector
       call richdsp(0,'DET ')

* --- draw expected photon pattern
      call rdisplay('PATE','GREEN',18.*3.14159/180.,ptrkthe,
     +               ptrkphi,pcervtx_d,pimp_d(1),pimp_d(2))

* --- Draw chosen rec pattern
      if (betadisp.gt.0) then
         call rdisplay('PATE','ORANGE',thetac,ptrkthe,ptrkphi,
     +        pcervtx_d,pimp_d(1),pimp_d(2))
      endif

* --- draw particle crossing point at PMT matrix
      pdum(3) = ZPMTDET
      pdum(1) = pimp(1) + tan(pthe)*cos(pphi)*(ZPMTDET-pimp(3))
      pdum(2) = pimp(2) + tan(pthe)*sin(pphi)*(ZPMTDET-pimp(3))

      call rdisplay
     +        ('PPMT','ORANGE',0.,ptrkthe,ptrkphi,pcervtx_d,
     +     pdum(1),pdum(2))

* --- Draw event hits
      call rdisplay('HIT ','RED',0.,ptrkthe,ptrkphi,pcervtx_d,
     +                 pimp_d(1),pimp_d(2))

* --- display prints
      call rdisplay('PRIN',' ',thetac,0.,0.,pcervtx_d,
     +     pimp_d(1),pimp_d(2))

* --- menu
      call richdsp(0,'MENU')

* --- return
 99   continue

      return
      end


***************************
*** CODE FROM richdsp.F ***
***************************

      SUBROUTINE RICHDSP(IFLG,CHOPT)
*****************************************************************
* AIMS: Displays rich events by using the HIGZ package
*
* IN  :
*      IFLG :
*             flag_main = -1 init
*                       =  0 display
*                       = +1 end
*             flag_nwin = number of higz windows
*      CHOPT: option word
*                       ='DET ' - draw detector
*                       ='MENU' - draw menu
*      NWIN : number of higz windows (or window number)
*
*****************************************************************

#include "../include/richrec_lipf.h"


      real rquest(100)
      character*20 chtit,chmenu(10),chitem,chdef,chval
      character chopt*4, chdummy*10, chpsfile*80,chpname*10
      integer nwin

* --- init
      kwid1 = 1
      kwid2 = 2

*     ----------------------------------------------
      IF (IFLG.eq.-1) THEN
*     ----------------------------------------------

* ...... HIGZ init
CCCCC         call iginit(0)
CC        call igwkty(kwtype)
CC        call igsse(6,kwtype)
         CALL IOPKS(6)
         CALL IOPWK(KWID1,10,2)
CC         CALL IOPWK(KWID2,11,4)

         CALL IGZSET('GZ') ! initialize picture keeping in zebra and graphic mode

* ...... clear workstation
CC        call iclrwk(0,0) !clear workstation
         xsize = 20.
         ysize = 20.
         xmargcm = 1.5
         ymargcm = 1.5
         call igrng(xsize,ysize)
         CV = rquest(11)
         XL = rquest(12)
         YB = rquest(13)
         WD = rquest(14)
         call igset('LTYP',1.)
         call igset('LWID',3.)
         call igset('BORD',1.)
         call igset('CHHE',0.5)
         call igset('TXAL',3.)
         call istxal(2,2)
*
         x1_0 = 1.5*xmargcm
         x2_0 = xsize-1.5*xmargcm
         y1_0 = 1.5*ymargcm
         y2_0 = ysize-1.5*ymargcm
         call isvp(10,x1_0*CV,x2_0*CV,y1_0*CV,y2_0*CV)
*
         x1_1 = 1.5*xmargcm
         x2_1 = xsize-1.5*xmargcm
         y1_1 = 1.5*ymargcm
         y2_1 = ysize-1.5*ymargcm
*
         call isvp(11,x1_1*CV,x2_1*CV,y1_1*CV,y2_1*CV)

* ...... color definition
         call color(0,' ',icol)

*     ----------------------------------------------
      ELSEIF (IFLG.EQ.1) THEN
*     ----------------------------------------------

* ...... end graphics
         call igmeta(0,0)
         CALL IDAWK(KWID1)
         CALL ICLWK(KWID1)
         call igend

*     ----------------------------------------------
      ELSEIF (IFLG.EQ.0) THEN
*     ----------------------------------------------
         IF (CHOPT.eq.'DET ') THEN
* --------> activation
            call iclrwk(KWID1,0)
            call iacwk(KWID1)
            CALL IZPICT(' ','M')
* --------> detector display
            call rdisplay('DET ',' ',0.,0.,0.,pcervtx,0.,0.)

         ELSEIF (CHOPT.eq.'MENU') THEN
 1          continue
            call igset('CHHE',0.3)
* --------> update display
            call iuwk(KWID1,1)
* --------> menu
            call iselnt(0)
            chtit     = 'RICH Event Display'
            nbmenus   = 6
            chmenu(1) = 'next'
            chmenu(2) = 'pgl'
            chmenu(3) = 'F([q])'
            chmenu(4) = 'PS'
            chmenu(5) = 'stop'
            chmenu(6) = '5p'
            chitem=' '
            nbi = 0
            x1 = 0.1
            x2 = 0.9
            y1 = 0.9
            y2 = 1.0
            call igmenu(1,chtit,x1,x2,y1,y2,nbmenus,chmenu,nbi,
     +               chitem,chdef,chval,ichoice,'S C H D')

C            call igmenu(1,chtit,x1,x2,y1,y2,nbmenus,chmenu,nbi,
C     +               chitem,chdef,chval,ichoice,'S C H')

            print*,ichoice
            if (ichoice.lt.0) then
* ------------ nex event
               if (abs(ichoice).eq.1) then
                  print*,'next event...'
* ------------ LIP beta reconstruction for plexiglass
               elseif (abs(ichoice).eq.2) then
                  if (HPGL.eq.0.0) then
                     print*,'(richdsp) HPGL=0.0 !!!!'
                  else
* .................. photon emission point and radiator ID
                     ZTARG      = 0.0
                     chradid    = 'PGL'
                     ZTRIAL     = ZTARG+HRAD+HPGL*0.5
                     pcervtx(3) = ZTRIAL
                     pcervtx(2) = pimp(2) +
     +                            tan(pthe)*sin(pphi)*(ZTRIAL-pimp(3))
                     pcervtx(1) = pimp(1) +
     +                            tan(pthe)*cos(pphi)*(ZTRIAL-pimp(3))
                     refindex   = pglix
* .................. display expected pattern
                     cerang = acos(1./pbeta/pglix) !cerenkov angle
                     call iselnt(10)
                     thc=cerang
                     call rdisplay('PATE',' ',thc,pthe,pphi,pcervtx,
     +                             0.,0.)
* .................. do reconstruction and display reconstructed pattern
                     IF (JOBC_CP.EQ.0 .OR. JOBC_CP.EQ.1) THEN
*                         call richlikebet(1)
                         call richlikemin(0)
                         call iuwk(KWID1,1)
	                 print*,' -> PLEXI REC'
	                 if(ipthetac.ne.0)then
                            print*,'  THETAC EXP ',cerang*RADDEG,
     +                           ' REC'  ,cangrec(ipthetac)*RADDEG

                            thc=cangrec(ipthetac)
                            call rdisplay('PATE',' ',thc,pthe,pphi,
     +                                    pcervtx,0.,0.)
                         else
                            print*,'  THETAC EXP ',cerang*RADDEG
	                 endif
                      ENDIF
                      call rdisplay('HIT ',' ',0.,0.,0.,pcertvx,0.,0.)
                   endif
                   goto 1
* ------------ Likelihood function
                elseif (abs(ichoice).eq.3) then
                   print*,'(richdsp) Likelihood function display'
                   CALL IOPWK(KWID2,11,10)
                   CALL ICLRWK(KWID2,0)
                   call iacwk(KWID2)
                   call idawk(KWID1) !deactivate event display window
                   call rdisdrawlike('BLUE',' ')
                   call iselnt(0)
                   chtit     = 'Likelihood function'
                   nbmenus   = 1
                   chmenu(1) = 'quit'
                   chitem=' '
                   nbi = 0
                   x1 = 0.1
                   x2 = 0.9
                   y1 = 0.9
                   y2 = 1.0
                   call igmenu(1,chtit,x1,x2,y1,y2,nbmenus,chmenu,nbi,
     +               chitem,chdef,chval,ichoice,'S C H D')
                   if (ichoice.eq.1) then
                      call idawk(KWID2)
                      call iclwk(KWID2)
                      call iacwk(KWID1) !reactivate event display window
                   endif
                   goto 1
* ------------ store event
                elseif (abs(ichoice).eq.4) then
*                  CALL IZPICT(' ','L')
                   CALL IZPICT(CHPNAME,'R')
                   print*,'current picture: ',chpname
                   call iclrwk(KWID1,0)
                   call izitoc(ievnumb,chdummy)
                   chpsfile = 'event-'//
     +                  chdummy(1:lenocc(chdummy))//'.eps'
                   open(unit=11,file=chpsfile,form='formatted',
     +                  status='unknown')
                   call igmeta(11,-113)
                   call igrng(18.,18.)
                   call izpict(CHPNAME,'D')
                   call igmeta(999,0)
                   print*,'saving event...'
                   call izpict(CHPNAME,'S')
                   goto 1
* ------------ stop
                elseif (abs(ichoice).eq.5) then
                   /* end graphics */
                   call igmeta(0,0)
                   CALL IDAWK(KWID1)
                   CALL ICLWK(KWID1)
                   if (nwin.eq.2) then
                      CALL IDAWK(KWID2)
                      CALL ICLWK(KWID2)
                   endif
                   call igend
                   stop
* ------------ 5 par fit
                elseif (abs(ichoice).eq.6) then
CCC                   call richbetarec5par
CCC                   if(iflgrec5par.eq.1) then
CCC                      thc=cangrec(ipthetac)
CCC                      print*,'DISPLAY 5-PAR REC DATA'
CCC                      print*,'----------------------'
CCC                      print*,'thc (deg)  = ',thc*RADDEG
CCC                      print*,'pthe (deg) = ',pthe*RADDEG
CCC                      print*,'pphi (deg) = ',pphi*RADDEG
CCC                      print*,'pcervtx    = ',(pcervtx(i),i=1,3)
CCC                      call rdisplay('PATE','GOLD',
CCC     +                     thc,pthe,pphi,pcervtx,0.,0.)
CCC                      call rdisplay('PPMT','GOLD',
CCC     +                     thc,pthe,pphi,pcervtx,pcoopmt(1),pcoopmt(2))
CCC                   endif
CCC                   goto 1
                endif
            else
               goto 1
            endif

         ENDIF

*     ----------------------------------------------
      ENDIF
*     ----------------------------------------------

* --- end
      return
      end


****************************
*** CODE FROM rdisplay.F ***
****************************

*==================================================================
      subroutine rdisplay
     +     (CHOPT,CHCOL,thc,parthe,parphi,pvertex,xcoopmt,ycoopmt)
*==================================================================
********************************************************************
* this subroutine calls HIGZ routine for:
*
* IN : CHOPT='DET '  detector drawing
*            'PATE'  pattern drawing
*            'HIT '  HIT drawing
*            'PPMT'  track drawing
*      CHCOL --> color ('RED','GREEN','BLUE',...)
*                                  see color subroutine in richdsp.F
*                                  for further choices
*      thc  ---> Cherenkov angle ('PATE' option)
*      parthe -> particle theta ('PPMT' option)
*      parphi -> particle phi   ('PPMT' option)
*      pvertex-> photons emission point
*      xcoopmt-> x coord of particle at PMT plane
*                                                   ('PPMT' option)
*      ycoopmt-> y coord of particle at PMT plane
*                                                   ('PPMT' option)
********************************************************************

#include "../include/richrec_lipf.h"

      INTEGER index_npe(nhitmax)

      real pvertex(3) ! other vertex than pcervtx in richrec.inc
      integer nassh
      real probability

      INTEGER IFLFL
      REAL X(41),Y(41),richacca(5),richeffa(8)

      character*4 chopt
      character*(*) chcol
      character*30 chdummy,chdummy2,chsignal
*
      integer index_hits(1600)
*
      integer ncolsignal
      parameter(icol0=10)
      parameter(ncolsignal=7)
      real signalncol(ncolsignal+1)

      real xzoom1,xzoom2,yzoom1,yzoom2

      parameter(antilog=2.718282)

      integer boxes(11,11)     ! the same definition as in g_flight.F
      data boxes/
     +     0, 0, 0, 1, 1, 1, 1, 1, 0, 0, 0,
     +     0, 1, 1, 1, 1, 1, 1, 1, 1, 1, 0,
     +     0, 1, 1, 1, 1, 1, 1, 1, 1, 1, 0,
     +     1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1,
     +     1, 1, 1, 1, 2, 2, 2, 1, 1, 1, 1,
     +     1, 1, 1, 1, 2, 2, 2, 1, 1, 1, 1,
     +     1, 1, 1, 1, 2, 2, 2, 1, 1, 1, 1,
     +     1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1,
     +     0, 1, 1, 1, 1, 1, 1, 1, 1, 1, 0,
     +     0, 1, 1, 1, 1, 1, 1, 1, 1, 1, 0,
     +     0, 0, 0, 1, 1, 1, 1, 1, 0, 0, 0
     + /
      SAVE boxes

      !$OMP THREADPRIVATE (boxes)
      integer boxesn(8,8)     ! the same definition as in g_flight.F
      data boxesn/
     +     0, 0, 1, 1, 1, 1, 0, 0,
     +     0, 1, 1, 1, 1, 1, 1, 0,
     +     1, 1, 1, 1, 1, 1, 1, 1,
     +     1, 1, 1, 2, 2, 1, 1, 1,
     +     1, 1, 1, 2, 2, 1, 1, 1,
     +     1, 1, 1, 1, 1, 1, 1, 1,
     +     0, 1, 1, 1, 1, 1, 1, 0,
     +     0, 0, 1, 1, 1, 1, 0, 0
     + /
      !$OMP THREADPRIVATE (boxesn)
      SAVE boxesn

* ... init (lazy???) variables
c      if (ipthatc.ne.0) then  <--- replaced, was a typo? (RP, 01-Mar-2007)
      if (ipthetac.ne.0) then
         nassh = nbushits(ipthetac)
         probability = pkolmog(ipthetac)
      else
         probability =  -999.
         nassh = -999
      endif


      call vfill(richacca,5,0.)
      idum=0
      pitchx=pmtwx+shieldw
      pitchy=pmtwy+shieldw
*
      XLIMV=(17*PITCHX + JUMP)/2.
      XLIMH=XLIMV + DMECX
      YLIMH=(17*PITCHY)/2.
      YLIMV= YLIMH + DMECY

*      call HPLSET('*fon',-60.)
*      call HPLSET('*siz',12.5)
**      call hplset('ksiz',0.4)
**      call hplset('vsiz',0.5)
*      call igset('LASI',0.05)
*      call hplset('asiz',0.4)
**      call ischh(0.5)
      call istxfp(-6,0)
*
*--------------------------------------------------------------------
      if (CHOPT.eq.'DET ') then
*--------------------------------------------------------------------

         call igset('LWID',1.)
         call igset('PLCI',1.)
         call ischh(0.2)

         if (rcgeom(1).ne.9) then !flight

* ...... draw detector

************************  change xzoom,yzoom to zoom  *******************
         xzoom1=-70.
         xzoom2=+70.
         yzoom1=-70.
         yzoom2=+70.
         call iswn(10,xzoom1,xzoom2,yzoom1,yzoom2)
         call iselnt(10)
         ndiv = 5*10000+2*100+7
         call igaxis(xzoom1,xzoom2,yzoom1,yzoom1,xzoom1,xzoom2,ndiv,' ')
         call igaxis(xzoom1,xzoom1,yzoom1,yzoom2,yzoom1,yzoom2,ndiv,' ')



*
************************************************************************
*
         call igbox(xzoom1,xzoom2,yzoom1,yzoom2)
         call igset('FAIS',0.)
         call igset('FACI',42.)
         call igset('BORD',0.)
         call igset('FACI',41.)
         call igset('BORD',1.)
         call igset('FAIS',0.)
*         xnafsizedum=1.5*radtile_pitch
*         call igbox(-xnafsizedum,xnafsizedum,-xnafsizedum,xnafsizedum) ! naf square

* ...... DRAW RADIATOR INNER WALLS
         if (LEVGRAD.eq.1) then

C           -------------------------------
            IF (RADTILE_PITCH.LT.12.) THEN
C           -------------------------------

            do i=1,11
               do j=1,11
                  xc=(i-6)*(RADTILE_PITCH) - RADTILE_PITCH/2.
                  yc=(j-6)*(RADTILE_PITCH) - RADTILE_PITCH/2.
                  X1=XC
                  X2=XC+(RADTILE_PITCH-RADTILE_SUPTHK)
                  Y1=YC
                  Y2=YC+(RADTILE_PITCH-RADTILE_SUPTHK)
                  if (boxes(i,j).eq.2) then  !NaF
                     call igset('FAIS',1.)
                     call igset('PLCI',49.)
                  elseif (boxes(i,j).eq.1) then !AGL
                     call igset('FAIS',0.)
                  elseif (boxes(i,j).eq.0) then !non-existent
                     call igset('FAIS',0.)
                     call igset('PLCI',0.)
                  endif
                  call igbox(x1,x2,y1,y2)
                  call igset('PLCI',1.)
               enddo
            enddo

C           -------------------------------
            ELSE
C           -------------------------------

            do i=1,8
               do j=1,8
                  xleft  = - 4.*radtile_pitch + real(i-1)*radtile_pitch
                  xright =  xleft + (radtile_pitch-radtile_supthk)
                  ydown  = - 4.*radtile_pitch + real(j-1)*radtile_pitch
                  yup    =  ydown + (radtile_pitch-radtile_supthk)

                  if (boxesn(i,j).eq.2) then  !NaF
                     call igset('FAIS',1.)
                     call igset('PLCI',49.)
                  elseif (boxesn(i,j).eq.1) then !AGL
                     call igset('FAIS',0.)
                  elseif (boxesn(i,j).eq.0) then !non-existent
                     call igset('FAIS',0.)
                     call igset('PLCI',0.)
                  endif
                  call igbox(xleft,xright,ydown,yup)
                  call igset('PLCI',1.)
               enddo
            enddo


C           -------------------------------
            ENDIF
C           -------------------------------


         endif

* ...... MATRIX
         call igset('PLCI',1.)
         call igset('BORD',1.)
         call igset('FAIS',0.)
         call igset('FACI',1.)
         call igbox(-EMCXLIM,+EMCXLIM,-EMCYLIM,+EMCYLIM)
         call igset('LWID',5.)
         call igarc(vtmir(1),vtmir(2),0.,RBMIR,0.,0.)
         call igset('LWID',1.)
         call igset('LTYP',2.)
****>>>           call igarc(0.,0.,0.,RTMIR,0.,0.)
         call igset('LTYP',1.)
* ...... draw pmt matrix
*-----> DRAW PMT BOXES (loop on pmt numbers)
         call igset('LWID',1.)
         do i=0,755
           call pmtpos_flight(I,XL,XR,YU,YD,XC,YC,IFLFL)
           x(1)=xr
           x(2)=xl
           x(3)=xl
           x(4)=xr
           x(5)=x(1)
           y(1)=yd
           y(2)=yd
           y(3)=yu
           y(4)=yu
           y(5)=y(1)
           RC=SQRT(XC**2+YC**2)
*           IF (RC.LT.65.) then
            IF(IFLFL.EQ.0) THEN
              CALL ISLN(3)
              CALL IPL(5,X,Y)
            ELSEIF(IFLFL.EQ.2) THEN
             call igset('FAIS',1.)
             call igset('FACI',41.)
             call igset('BORD',1.)
             call IFA(5,X,Y)
             call igset('FAIS',0.)
            ENDIF
*           ENDIF
         ENDDO
* -----> frame
         call igset('LWID',3.)
         x(1) = 0
         x(2) = 0
         y(1) =  ylimv+9*pitchy
         y(2) = -y(1)
         call isln(2) !dashed line
c         call ipl(2,X,Y)
         x(1) = xlimh + 9*pitchx
         x(2) = -x(1)
         y(1) = 0
         y(2) = 0
         call isln(2) !dashed line
c         call ipl(2,X,Y)
 /* MODULO 1 and 6 */
         x(1)  =  xlimh + 0*(pitchx)+XCPMM
         x(2)  =  xlimh + 1*(pitchx)+XCPMM
         x(3)  =  xlimh + 1*(pitchx)+XCPMM
         x(4)  =  xlimh + 7*(pitchx)+XCPMM
         x(5)  =  xlimh + 7*(pitchx)+XCPMM
         x(6)  =  xlimh + 8*(pitchx)+XCPMM
         x(7)  =  xlimh + 8*(pitchx)+XCPMM
         x(8)  =  xlimh + 9*(pitchx)+XCPMM
         x(9)  =  xlimh + 9*(pitchx)+XCPMM
         x(10) =  xlimh + 8*(pitchx)+XCPMM
         x(11) =  xlimh + 8*(pitchx)+XCPMM
         x(12) =  xlimh + 7*(pitchx)+XCPMM
         x(13) =  xlimh + 7*(pitchx)+XCPMM
         x(14) =  xlimh + 1*(pitchx)+XCPMM
         x(15) =  xlimh + 1*(pitchx)+XCPMM
         x(16) =  xlimh + 0*(pitchx)+XCPMM
         x(17) = x(1)
         y(1)  =-  ylimh + 1*(pitchy)+YCPMM
         y(2)  =-  ylimh + 1*(pitchy)+YCPMM
         y(3)  =-  ylimh + 0*(pitchy)+YCPMM
         y(4)  =-  ylimh + 0*(pitchy)+YCPMM
         y(5)  =-  ylimh + 1*(pitchy)+YCPMM
         y(6)  =-  ylimh + 1*(pitchy)+YCPMM
         y(7)  =-  ylimh + 3*(pitchy)+YCPMM
         y(8)  =-  ylimh + 3*(pitchy)+YCPMM
         y(9)  =-  ylimh + 14*(pitchy)+YCPMM
         y(10) =-  ylimh + 14*(pitchy)+YCPMM
         y(11) =-  ylimh + 16*(pitchy)+YCPMM
         y(12) =-  ylimh + 16*(pitchy)+YCPMM
         y(13) =-  ylimh + 17*(pitchy)+YCPMM
         y(14) =-  ylimh + 17*(pitchy)+YCPMM
         y(15) =-  ylimh + 16*(pitchy)+YCPMM
         y(16) =-  ylimh + 16*(pitchy)+YCPMM
         y(17) =  y(1)
*
         call isln(1) !full line
         call ipl(17,X,Y)
         do i=1,17
          x(i)= -x(i)+2*xcpmm
          y(i)= -y(i)+2*ycpmm
         enddo
         call ipl(17,X,Y)
    /*planes 2 and 7 */
         x(1) =  xlimh + 1*(pitchx)+XCPMM
         x(2) =  xlimh + 7*(pitchx)+XCPMM
         x(3) =  xlimh + 7*(pitchx)+XCPMM
         x(4) =  xlimh + 6*(pitchx)+XCPMM
         x(5) =  xlimh + 6*(pitchx)+XCPMM
         x(6) =  xlimh + 5*(pitchx)+XCPMM
         x(7) =  xlimh + 5*(pitchx)+XCPMM
         x(8) =  xlimh + 4*(pitchx)+XCPMM
         x(9) =  xlimh + 4*(pitchx)+XCPMM
         x(10) =  xlimh + 3*(pitchx)+XCPMM
         x(11) =  xlimh + 3*(pitchx)+XCPMM
         x(12) =  xlimh + 2*(pitchx)+XCPMM
         x(13) =  xlimh + 2*(pitchx)+XCPMM
         x(14) =  xlimh + 1*(pitchx)+XCPMM
         x(15) =  xlimh + 1*(pitchx)+XCPMM
         x(16) =  xlimh + 0*(pitchx)+XCPMM
         x(17) =  xlimh + 0*(pitchx)+XCPMM
         x(18) =  xlimh + 1*(pitchx)+XCPMM
         x(19) =  x(1)
         y(1) =  ylimv + 0*(pitchy)+YCPMM
         y(2) =  ylimv + 0*(pitchy)+YCPMM
         y(3) =  ylimv + 1*(pitchy)+YCPMM
         y(4) =  ylimv + 1*(pitchy)+YCPMM
         y(5) =  ylimv + 2*(pitchy)+YCPMM
         y(6) =  ylimv + 2*(pitchy)+YCPMM
         y(7) =  ylimv + 3*(pitchy)+YCPMM
         y(8) =  ylimv + 3*(pitchy)+YCPMM
         y(9) =  ylimv + 4*(pitchy)+YCPMM
         y(10) =  ylimv + 4*(pitchy)+YCPMM
         y(11) =  ylimv + 5*(pitchy)+YCPMM
         y(12) =  ylimv + 5*(pitchy)+YCPMM
         y(13) =  ylimv + 6*(pitchy)+YCPMM
         y(14) =  ylimv + 6*(pitchy)+YCPMM
         y(15) =  ylimv + 7*(pitchy)+YCPMM
         y(16) =  ylimv + 7*(pitchy)+YCPMM
         y(17) =  ylimv + 1*(pitchy)+YCPMM
         y(18) =  ylimv + 1*(pitchy)+YCPMM
         y(19) =  y(1)
         call isln(1) !full line
         call ipl(19,X,Y)
         do i=1,19
          x(i)= -x(i)+2*xcpmm
          y(i)= -y(i)+2*ycpmm
         enddo
         call ipl(19,X,Y)
     /* planes 3 and 8*/
         x(1)  = 0*(pitchx)+XCPMM
         X(2)  = 8*(PITCHX)+XCPMM
         X(3)  = 8*(PITCHX)+XCPMM
         X(4)  = 9*(PITCHX)+XCPMM
         X(5)  = 9*(PITCHX)+XCPMM
         X(6)  = 8*(PITCHX)+XCPMM
         X(7)  = 8*(PITCHX)+XCPMM
         X(8)  = 6*(PITCHX)+XCPMM
         X(9)  = 6*(PITCHX)+XCPMM
         X(10) = 0*(PITCHX)+XCPMM
         X(11) = 0*(PITCHX)+XCPMM
         do i=1,11
          x(i)= x(i)-(DX0)
         enddo
         y(1)  =  ylimv + 0*(pitchy)+YCPMM
         y(2)  =  ylimv + 0*(pitchy)+YCPMM
         y(3)  =  ylimv + 1*(pitchy)+YCPMM
         y(4)  =  ylimv + 1*(pitchy)+YCPMM
         y(5)  =  ylimv + 7*(pitchy)+YCPMM
         y(6)  =  ylimv + 7*(pitchy)+YCPMM
         y(7)  =  ylimv + 8*(pitchy)+YCPMM
         y(8)  =  ylimv + 8*(pitchy)+YCPMM
         y(9)  =  ylimv + 9*(pitchy)+YCPMM
         y(10) =  ylimv + 9*(pitchy)+YCPMM
         y(11) =  ylimv + 0*(pitchy)+YCPMM
         call ipl(11,X,Y)
         do i=1,11
          x(i)=-x(i)+2*xcpmm
          y(i)=-y(i)+2*ycpmm
         enddo
         call ipl(11,X,Y)
   /* planes 4 and 9*/
         x(1)  =  0*(pitchx)+XCPMM
         x(2)  =  0*(pitchx)+XCPMM
         x(3)  =  5*(pitchx)+XCPMM
         x(4)  =  5*(pitchx)+XCPMM
         x(5)  =  7*(pitchx)+XCPMM
         x(6)  =  7*(pitchx)+XCPMM
         x(7)  =  8*(pitchx)+XCPMM
         x(8)  =  8*(pitchx)+XCPMM
         x(9)  =  7*(pitchx)+XCPMM
         x(10) =  7*(pitchx)+XCPMM
         x(11) =  0*(pitchx)+XCPMM
         y(1)  =  ylimv +0*(pitchy)+YCPMM
         y(2)  =  ylimv +9*(pitchy)+YCPMM
         y(3)  =  ylimv +9*(pitchy)+YCPMM
         y(4)  =  ylimv +8*(pitchy)+YCPMM
         y(5)  =  ylimv +8*(pitchy)+YCPMM
         y(6)  =  ylimv +7*(pitchy)+YCPMM
         y(7)  =  ylimv +7*(pitchy)+YCPMM
         y(8)  =  ylimv +1*(pitchy)+YCPMM
         y(9)  =  ylimv +1*(pitchy)+YCPMM
         y(10) =  ylimv +0*(pitchy)+YCPMM
         y(11) =  ylimv +0*(pitchy)+YCPMM

         do i=1,11
          x(i)= -dx0-jump-x(i)+2*xcpmm
         enddo
         call ipl(11,X,Y)
         do i=1,11
          x(i)=-x(i)+2*xcpmm
          y(i)=-y(i)+2*ycpmm
         enddo
         call ipl(11,X,Y)
  /* planes 5 and 10 */
         x(1) =  xlimh + 0*(pitchx)+XCPMM
         x(2) =  xlimh + 0*(pitchx)+XCPMM
         x(3) =  xlimh + 1*(pitchx)+XCPMM
         x(4) =  xlimh + 1*(pitchx)+XCPMM
         x(5) =  xlimh + 2*(pitchx)+XCPMM
         x(6) =  xlimh + 2*(pitchx)+XCPMM
         x(7) =  xlimh + 3*(pitchx)+XCPMM
         x(8) =  xlimh + 3*(pitchx)+XCPMM
         x(9) =  xlimh + 4*(pitchx)+XCPMM
         x(10) =  xlimh + 4*(pitchx)+XCPMM
         x(11) =  xlimh + 5*(pitchx)+XCPMM
         x(12) =  xlimh + 5*(pitchx)+XCPMM
         x(13) =  xlimh + 6*(pitchx)+XCPMM
         x(14) =  xlimh + 6*(pitchx)+XCPMM
         x(15) =  xlimh + 7*(pitchx)+XCPMM
         x(16) =  xlimh + 7*(pitchx)+XCPMM
         x(17) =  xlimh + 1*(pitchx)+XCPMM
         x(18) =  xlimh + 1*(pitchx)+XCPMM
         x(19) =  x(1)
         do i=1,19
          x(i)=-x(i)+2*xcpmm
         enddo
         y(1) =  ylimv + 1*(pitchy)+YCPMM
         y(2) =  ylimv + 7*(pitchy)+YCPMM
         y(3) =  ylimv + 7*(pitchy)+YCPMM
         y(4) =  ylimv + 6*(pitchy)+YCPMM
         y(5) =  ylimv + 6*(pitchy)+YCPMM
         y(6) =  ylimv + 5*(pitchy)+YCPMM
         y(7) =  ylimv + 5*(pitchy)+YCPMM
         y(8) =  ylimv + 4*(pitchy)+YCPMM
         y(9) =  ylimv + 4*(pitchy)+YCPMM
         y(10) =  ylimv + 3*(pitchy)+YCPMM
         y(11) =  ylimv + 3*(pitchy)+YCPMM
         y(12) =  ylimv + 2*(pitchy)+YCPMM
         y(13) =  ylimv + 2*(pitchy)+YCPMM
         y(14) =  ylimv + 1*(pitchy)+YCPMM
         y(15) =  ylimv + 1*(pitchy)+YCPMM
         y(16) =  ylimv + 0*(pitchy)+YCPMM
         y(17) =  ylimv + 0*(pitchy)+YCPMM
         y(18) =  ylimv + 1*(pitchy)+YCPMM
         y(19) =  y(1)
         call ipl(19,X,Y)
         do i=1,19
          x(i)=-x(i)+2*xcpmm
          y(i)=-y(i)+2*ycpmm
         enddo
         call ipl(19,X,Y)
*       -----------------
        else ! prototype
*       -----------------

* ...... draw/basics
         a=20.
         xzoom1=-a
         xzoom2=+a
         yzoom1=-a
         yzoom2=+a
         call iswn(10,xzoom1,xzoom2,yzoom1,yzoom2)
         call iselnt(10)
         ndiv = 5*100+10
         call igaxis(xzoom1,xzoom2,yzoom1,yzoom1,xzoom1,xzoom2,ndiv,' ')
         call igaxis(xzoom1,xzoom1,yzoom1,yzoom2,yzoom1,yzoom2,ndiv,' ')
         xtxt=xzoom2
         ytxt=yzoom1-4.0
         call igtext(xtxt,ytxt,'Y (cm)',.80,0.,'R')
         xtxt=xzoom1-0.75
         ytxt=yzoom2-3.0
         call igtext(xtxt,ytxt,'X (cm)',.80,0.,'R')


c         call igbox(xzoom1,xzoom2,yzoom1,yzoom2)  ! why do I need this ?

         call igset('LTYP',2.)
         call igset('LWID',3.)
*
         call igset('LTYP',1.)
         call igset('LWID',1.)
* ...... draw pmt matrix
* -----> frame
         x(1)  = xlimprot - 10*(pmtwx+shieldw)+XCPMM
         x(2)  = xlimprot - 9*(pmtwx+shieldw)+XCPMM
         x(3)  = xlimprot - 9*(pmtwx+shieldw)+XCPMM
         x(4)  = xlimprot - 0*(pmtwx+shieldw)+XCPMM
         x(5)  = xlimprot - 0*(pmtwx+shieldw)+XCPMM
         x(6)  = xlimprot - 6*(pmtwx+shieldw)+XCPMM
         x(7)  = xlimprot - 6*(pmtwx+shieldw)+XCPMM
         x(8)  = xlimprot - 9*(pmtwx+shieldw)+XCPMM
         x(9)  = xlimprot - 9*(pmtwx+shieldw)+XCPMM
         x(10) = xlimprot - 10*(pmtwx+shieldw)+XCPMM
         x(11) = X(1)
*
         y(1)  = ylimprot - 1*(pmtwy+shieldw)+YCPMM
         y(2)  = ylimprot - 1*(pmtwy+shieldw)+YCPMM
         y(3)  = ylimprot - 0*(pmtwy+shieldw)+YCPMM
         y(4)  = ylimprot - 0*(pmtwy+shieldw)+YCPMM
         y(5)  = ylimprot - 11*(pmtwy+shieldw)+YCPMM
         y(6)  = ylimprot - 11*(pmtwy+shieldw)+YCPMM
         y(7)  = ylimprot - 10*(pmtwy+shieldw)+YCPMM
         y(8)  = ylimprot - 10*(pmtwy+shieldw)+YCPMM
         y(9)  = ylimprot - 9*(pmtwy+shieldw)+YCPMM
         y(10) = ylimprot - 9*(pmtwy+shieldw)+YCPMM
         y(11) = Y(1)
         call isln(1) !full line
         call ipl(11,Y,X)

*-----> DRAW PMT BOXES (loop on pmt numbers)
         do i=0,nbpmtx*nbpmty-1
           call PMTPOS_PROTN(I,ix,iy,XL,XR,YU,YD,XC,YC)
           x(1)=xr
           x(2)=xl
           x(3)=xl
           x(4)=xr
           x(5)=x(1)
           y(1)=yd
           y(2)=yd
           y(3)=yu
           y(4)=yu
           y(5)=y(1)
           IF (IFLPMT(IX,IY).EQ.0) THEN
              CALL ISLN(3)
              CALL IPL(5,Y,X)
           ELSEIF (IFLPMT(IX,IY).EQ.2) THEN
             call igset('FAIS',1.)
             call igset('FACI',41.)
             call igset('BORD',0.)
             call IFA(5,Y,X)
             call igset('FAIS',0.)
*             CALL ISFAIS(3)
*             CALL IFA(5,Y,X)
*             CALL ISFAIS(0)
           ENDIF
         ENDDO
* -----> DRAW MIRROR
         if (RCGEOM(1).EQ.9 .and. RCGEOM(2).EQ.1) then
            call igset('PLCI',1.)
            call igset('BORD',1.)
            call igset('VAL ',1.)
            phi1_deg = 90.-MIRPHILD(1)*RADDEG
            phi2_deg = 90.-MIRPHILD(2)*RADDEG
            call igarc(vtmir(2),vtmir(1),RBMIR,RBMIR,phi2_deg,phi1_deg)
            call igarc(vtmir(2),vtmir(1),RTMIR,RTMIR,phi2_deg,phi1_deg)
         endif

         endif
*--------------------------------------------------------------
      elseif (chopt.eq.'PATE') then
* ... draw full photon pattern
*--------------------------------------------------------------
         call color(1,chcol,icol)
         ICODL = 10000*ICOL+100*1+7 !1000*PLCI+LTYP*100+LWID
*         print*,' '
*         print*,'(rdisplay) PARAMETERS FOR PHOTRACE'
*         print*,'----------------------------------'
*         print*,'thc (deg)    = ',thc*raddeg
*         print*,'parthe (deg) = ',parthe*raddeg
*         print*,'parphi (deg) = ',parphi*raddeg
*         print*,'pvertex = ',pvertex(1),pvertex(2),pvertex(3)
*         print*,'icodl = ',icodl
         if (rcgeom(1).ne.9) then
            xzoom1=-70.
            xzoom2=+70.
            yzoom1=-70.
            yzoom2=+70.
            call iswn(10,xzoom1,xzoom2,yzoom1,yzoom2)
            call iselnt(10)
            call photrace(thc,parthe,parphi,pvertex,ICODL)
         else
            xzoom1=-20.
            xzoom2=+20.
            yzoom1=-20.
            yzoom2=+20.
            call iswn(10,xzoom1,xzoom2,yzoom1,yzoom2)
            call iselnt(10)
            call photrace_prot(thc,parthe,parphi,pvertex,ICODL)
         endif
         call igset('LWID',1.)
*--------------------------------------------------------------
      elseif (chopt.eq.'PATR') then
* ... draw photon pattern at radiator bottom
*--------------------------------------------------------------
         call color(1,chcol,icol)
         ICODL = 10000*ICOL+100*1+7 !1000*PLCI+LTYP*100+LWID
         call photrace_rad(thc,parthe,parphi,pvertex,ICODL)
         call igset('LWID',1.)
*--------------------------------------------------------------
      elseif (chopt.eq.'HIT ') then
* ... draw hits
*--------------------------------------------------------------
*         print*,'nbhits = ',nbhits
*         print*,'hit coords:'
*         do i=1,min(nbhits,nhitmax)
*            print*,hitscoo(1,i),hitscoo(2,i),hitscoo(3,i)
*         enddo

         if (rcgeom(1).eq.9) then
            a=20.
            xzoom1=-a
            xzoom2=+a
            yzoom1=-a
            yzoom2=+a
         else
            a=70.
            xzoom1=-a
            xzoom2=+a
            yzoom1=-a
            yzoom2=+a
         endif
         call igset('LWID',1.)
         call igset('PLCI',1.)
         CALL ISLN(1)
         call igset('MTYP',21.)
         call iselnt(10)
         if(rcgeom(1).eq.9)then
*========================================================
            if(chcol.eq.'ADJUST')then  ! Adjusted scale
*========================================================
            if(nbhits.ge.2)then
               call sortzv_alt(hitsnpe,index_npe,
     +              min(nbhits,nhitmax),+1,1,0)
*
               sigmax=hitsnpe(index_npe(1))
               sigmin=hitsnpe(index_npe(min(nbhits,nhitmax)))
               amplitudelin=sigmax-sigmin
*
               sigmaxlog=log(sigmax)
               sigminlog=log(sigmin)
               amplitudelog=sigmax/sigmin
            endif
            do i=1,min(nbhits,nhitmax)
               if(nbhits.gt.1)then
                  if (amplitudelog.gt.10.)then
                     step=(sigmaxlog-sigminlog)/7.0
                     ncol=int((log(hitsnpe(i))-sigminlog)/step)
                  else
                     step=amplitudelin/7.0
                     ncol=int((hitsnpe(i)-sigmin)/step)
                  endif
               else
                  ncol=0
               endif
               if(ncol.lt.7)ncol=ncol+1
               col=real(ncol)+real(icol0)
               xl=hitscoo(2,i)-.3875
               xr=hitscoo(2,i)+.3875
               yd=hitscoo(1,i)-.3875
               yu=hitscoo(1,i)+.3875
               call igset('FAIS',1.)
               call igset('BORD',1.)
               call igset('FACI',col)
               call igbox(xl,xr,yd,yu)
*
               call igset('MTYP',5.)
               call igset('MSCF',1.8)
               call igset('PLCI',1.0)
               if(iflghit(i).eq.2)
     &         call igraph(1,hitscoo(2,i),hitscoo(1,i),'P')
               call igset('MSCF',1.)
            enddo
*  -----------------------------------------------------------------
*  ---      print vertical COLOUR scale  (if defined-at least 2 hits)
*  -----------------------------------------------------------------
            if(nbhits.ge.2)then
               signalncol(1)=sigmin
               signalncol(ncolsignal+1)=sigmax
               if (amplitudelog.gt.10.)then
                  step=(sigmaxlog-sigminlog)/7.0
                  do i=2,ncolsignal
                     signalncol(i)=ANTILOG**(sigminlog+(i-1)*step)
                  enddo
               else
                  step=(sigmax-sigmin)/7.0
                  do i=2,ncolsignal
                     signalncol(i)=sigmin+(i-1)*step
                  enddo
               endif
               call igset('PLCI',1.)
               call igset('FAIS',1.)
               call igset('BORD',1.)
               yd=-10.
               do j=1,ncolsignal
                  yu=yd+2.5
                  call igset('FACI',float(j+icol0))
                  call igbox(xzoom2-2.,xzoom2,yd,yu) ! color scale box
                  ytxt=(yu+yd)/2.-0.5
                  call izitoc(nint(signalncol(j)),chdummy)
                  call izitoc(nint(signalncol(j+1)),chdummy2)
                  chdummy = chdummy(1:min(5,lenocc(chdummy)))//'-'
     .                 //chdummy2(1:min(5,lenocc(chdummy2)))
                  call igtext(xzoom2+0.25,ytxt,chdummy,0.7,0.,'L')
                  yd=yu
               enddo
               xtxt=xzoom2-2.
               ytxt=yd+2
               call igtext(xtxt,ytxt,'(p.e.)',0.9,0.,'L')
               ytxt=ytxt+2
               call igtext(xtxt,ytxt,'signal',0.9,0.,'L')
               call igset('FACI',1.)
               call igset('FAIS',0.)
            endif
*====================================
            else  ! FIXED scale
*====================================
            do i=1,min(nbhits,nhitmax)
               if(hitsnpe(i).lt.1.)then
                  ncol=1
               elseif(hitsnpe(i).lt.3.)then
                  ncol=2
               elseif(hitsnpe(i).lt.7.)then
                  ncol=3
               elseif(hitsnpe(i).lt.15.)then
                  ncol=4
               elseif(hitsnpe(i).lt.30.)then
                  ncol=5
               elseif(hitsnpe(i).lt.60.)then
                  ncol=6
               else
                  ncol=7
               endif
               col=real(ncol)+real(icol0)
               xl=hitscoo(2,i)-.3875
               xr=hitscoo(2,i)+.3875
               yd=hitscoo(1,i)-.3875
               yu=hitscoo(1,i)+.3875
               call igset('FAIS',1.)
               call igset('BORD',1.)
               call igset('FACI',col)
               call igbox(xl,xr,yd,yu)
*
               call igset('MTYP',5.)
               call igset('MSCF',1.8)
               call igset('PLCI',1.0)
               if(iflghit(i).eq.2)
     &         call igraph(1,hitscoo(2,i),hitscoo(1,i),'P')
               call igset('MSCF',1.)
            enddo
*  -----------------------------------------------------------------
*  ---      print vertical COLOUR scale  (if defined-at least 2 hits)
*  -----------------------------------------------------------------
            call igset('PLCI',1.)
            call igset('FAIS',1.)
            call igset('BORD',1.)
            yd=-10.
            do j=1,ncolsignal
               if(j.eq.1)then
                  chsignal = '0-1'
               elseif(j.eq.2)then
                  chsignal = '1-3'
               elseif(j.eq.3)then
                  chsignal = '3-7'
               elseif(j.eq.4)then
                  chsignal = '7-15'
               elseif(j.eq.5)then
                  chsignal = '15-30'
               elseif(j.eq.6)then
                  chsignal = '30-60'
               elseif(j.eq.7)then
                  chsignal = '"G#60'
               endif
               yu=yd+2.5
               call igset('FACI',float(j+icol0))
               call igbox(xzoom2-2.,xzoom2,yd,yu) ! color scale box
               ytxt=(yu+yd)/2.-0.5
               call igtext
     ,(xzoom2+0.25,ytxt,chsignal(1:lenocc(chsignal)),0.7,0.,'L')
               yd=yu
            enddo
            xtxt=xzoom2-2.
            ytxt=yd+2
            call igtext(xtxt,ytxt,'(p.e.)',0.9,0.,'L')
            ytxt=ytxt+2
            call igtext(xtxt,ytxt,'signal',0.9,0.,'L')
            call igset('FACI',1.)
            call igset('FAIS',0.)
*====================================
            endif ! matches if (chcol.eq.'ADJUST')
*====================================
*=======================================================
         else                   ! DRAW HITS IN THE FLIGHT MATRIX
*=======================================================
            call color(1,chcol,icol)
            call igset('PMCI',real(icol))
            do i=1,min(nbhits,nhitmax)
*               call igset('PMCI',46.)
               call igset('MSCF',0.95)
               call igraph(1,hitscoo(1,i),hitscoo(2,i),'P')
               col=4.
               call igset('FAIS',1.)
               call igset('BORD',1.)
               call igset('FACI',col)
               xl=hitscoo(1,i)-.3875
               xr=hitscoo(1,i)+.3875
               yd=hitscoo(2,i)-.3875
               yu=hitscoo(2,i)+.3875
               call igbox(xl,xr,yd,yu)
               call igset('FAIS',0.)
            enddo
         endif
*--------------------------------------------------------------
      elseif (chopt.eq.'PPMT') then
* ... draw particle track projection
*--------------------------------------------------------------
         if(abs(xcoopmt).lt.1.E3.and.abs(ycoopmt).lt.1.E3)then
            call color(1,chcol,icol)
            call igset('PMCI',real(icol))
            call igset('MTYP',5.)
            call igset('MSCF',5.0)
            call igset('LWID',5.0)
            call ismksc(5.0)
            if (rcgeom(1).ne.9) then
               call igraph(1,xcoopmt,ycoopmt,'P')
            else
               call igraph(1,ycoopmt,xcoopmt,'P')
            endif

*-----> DRAW DIRECTION OF INCOMING PARTICLE
            call igset('PLCI',real(icol))
            call igset('LWID',5.)
            x(1) = xcoopmt-abs(zpmtdet-pcervtx(3))
     ,           *tan(parthe)*cos(parphi)
            x(2) = xcoopmt
            y(1) = ycoopmt-abs(zpmtdet-pcervtx(3))
     ,           *tan(parthe)*sin(parphi)
            y(2) = ycoopmt
            CALL ISLN(2)
            if(rcgeom(1).eq.9)then
               call ipl(2,Y,X)
            else
               call ipl(2,X,Y)
            endif
            CALL ISLN(1)
            x(2)=x(1)
            y(2)=y(1)
            x(1)=x(2)-100.*tan(parthe)*cos(parphi)
            y(1)=y(2)-100.*tan(parthe)*sin(parphi)
            if(rcgeom(1).eq.9)then
               call ipl(2,Y,X)
            else
               call ipl(2,X,Y)
            endif
            call igset('LWID',1.)
            call igset('MSCF',1.)
         endif
* ----------------------------------------------------
      elseif (chopt.eq.'PRIN') then
* ----------------------------------------------------

*     printout
*
      call iselnt(10)
*
      call izitoc(irun,chdummy)
      chdummy = 'RUN '//
     +     chdummy(1:lenocc(chdummy))//' /EVENT'
      call izitoc(ievnumb,chdummy2)
      chdummy = chdummy(1:lenocc(chdummy))//' '//
     +     chdummy2(1:lenocc(chdummy2))
      if(rcgeom(1).eq.9)then
         a=20.
         xzoom1=-a
         xzoom2=+a
         yzoom1=-a
         yzoom2=+a
         xtxt= xzoom1+2.5
         ytxt= yzoom2-2.5
         call igtext(xtxt,ytxt,chdummy,1.0,0.,'L')
      else
         a=70.
         xzoom1=-a
         xzoom2=+a
         yzoom1=-a
         yzoom2=+a
         xtxt=-65.
         ytxt=+65.
         call igtext(xtxt,ytxt,chdummy,2.5,0.,'L')
      endif

      if (rcgeom(1).eq.9)then
         if(ipthetac.ne.0)then
*            call izrtoc(cangrec(ipthetac)*RADDEG,chdummy)
            call izrtoc(thc*RADDEG,chdummy)
            chdummy='[Q]^rec!= '//chdummy(1:5)//' Deg'
            xtxt=xzoom2-2.5
            ytxt=yzoom2-1.5
            call igtext(xtxt,ytxt,chdummy,0.80,0.,'R')
*
            call izrtoc(probability,chdummy)
            chdummy='Prob^Kol!= '//chdummy(1:lenocc(chdummy))
            ytxt=yzoom2
            call igtext(xtxt,ytxt,chdummy,0.80,0.,'R')
*
*            call izitoc(nbushits(ipthetac),chdummy)
            call izitoc(nassh,chdummy)
            chdummy='Nb us hits= '//chdummy(1:lenocc(chdummy))
            ytxt=yzoom2-3.5
            call igtext(xtxt,ytxt,chdummy,0.80,0.,'R')
         else
            if(jobc_cp.ne.-9)then
               call izitoc(min(nbhits,nhitmax),chdummy)
               chdummy='Nb hits= '//chdummy(1:lenocc(chdummy))
               xtxt=xzoom2-2.5
               ytxt=yzoom2-3.5
               call igtext(xtxt,ytxt,chdummy,0.80,0.,'R')
            endif
         endif
      else
         if(ipthetac.ne.0)then
*            call izrtoc(cangrec(ipthetac)*RADDEG,chdummy)
            call izrtoc(thc*RADDEG,chdummy)
            chdummy='[Q]^rec!= '//chdummy(1:5)//' Deg'
            xtxt=xzoom2-2.5
            ytxt=yzoom2-5.5
            call igtext(xtxt,ytxt,chdummy,2.80,0.,'R')
*            call izitoc(nbushits(ipthetac),chdummy)
            call izitoc(nassh,chdummy)
            chdummy='Nb us hits= '//chdummy(1:lenocc(chdummy))
            ytxt=yzoom2-12.5
            call igtext(xtxt,ytxt,chdummy,2.80,0.,'R')
*
            call izrtoc(probability,chdummy)
            chdummy='Pkol= '//chdummy(1:lenocc(chdummy))
            call igtext(70.,77.,chdummy,2.80,0.,'R')
         else
            if(jobc_cp.ne.-9)then
               call izitoc(min(nbhits,nhitmax),chdummy)
               chdummy='Nb hits= '//chdummy(1:lenocc(chdummy))
               xtxt=xzoom2-2.5
               ytxt=yzoom2-12.5
               call igtext(xtxt,ytxt,chdummy,0.80,0.,'R')
            endif
         endif
      endif
 11   continue

* ----------------------------------------------------
      endif
* ----------------------------------------------------

* ... return
      return
      end


*************************
*** CODE FROM color.F ***
*************************

*========================================================
      subroutine color(itask,chcol,icol)
*========================================================
* color definition and coding
      character*(*) chcol

      kwid1=1
      if(itask.eq.0)then
         call iscr(kwid1, 1,0.00,0.00,0.00) !black
         call iscr(kwid1, 2,0.90,0.00,0.00) !red
         call iscr(kwid1, 3,0.00,0.90,0.00) !green
         call iscr(kwid1, 4,0.00,0.00,1.00) !blue
         call iscr(kwid1, 5,1.00,0.84,0.00) !gold
         call iscr(kwid1, 6,0.80,0.36,0.36) !indian

         call iscr(kwid1, 8,1.00,1.00,1.00) !white
         call iscr(kwid1, 9,1.00,1.00,0.90) !light pale yellow
         call iscr(kwid1,10,1.00,1.00,0.80) !light lemmon yellow

         call iscr(kwid1,11,1.00,1.00,0.70) !lemmon yellow
         call iscr(kwid1,12,1.00,1.00,0.00) !yellow
         call iscr(kwid1,13,1.00,0.84,0.00) !pale orange
         call iscr(kwid1,14,1.00,0.64,0.31) !orange
         call iscr(kwid1,15,1.00,0.00,0.00) !lighter red
         call iscr(kwid1,16,0.54,0.27,0.07) !dark brown
         call iscr(kwid1,17,0.00,0.00,0.00) !black
*
         call iscr(kwid1,41,0.90,0.90,0.90) !grey
         call iscr(kwid1,42,0.95,0.95,0.95) !lighter grey
         call iscr(kwid1,43,1.00,0.24,0.55) !red-pink
         call iscr(kwid1,44,0.60,0.80,0.19) !yellow-green
         call iscr(kwid1,45,0.18,0.54,0.34) !seagreen
         call iscr(kwid1,46,0.00,0.00,0.60) !navyblue
         call iscr(kwid1,47,0.60,0.19,0.80) !dark orchid
         call iscr(kwid1,48,0.85,0.64,0.13) !goldenrod
         call iscr(kwid1,49,0.70,0.13,0.13) !firebrick
      else
         icol=1 ! default color
         if(chcol.eq.'BLACK')then
            icol=1
         elseif(chcol.eq.'RED')then
            icol=2
         elseif(chcol.eq.'GREEN')then
            icol=3
         elseif(chcol.eq.'BLUE')then
            icol=4
         elseif(chcol.eq.'GOLD')then
            icol=5
         elseif(chcol.eq.'INDIAN')then
            icol=6
         elseif(chcol.eq.'WHITE')then
            icol=8
         elseif(chcol.eq.'LIGHT YEL')then
            icol=9
         elseif(chcol.eq.'LIGHT YEL2')then
            icol=10
         elseif(chcol.eq.'LIGHT YEL')then
            icol=11
         elseif(chcol.eq.'YELLOW')then
            icol=12
         elseif(chcol.eq.'ORANGE')then
            icol=14
         elseif(chcol.eq.'NAVYBLUE')then
            icol=46
         elseif(chcol.eq.'FIREBRICK')then
            icol=49
         endif
      endif

      return
      end


****************************
*** CODE FROM photrace.F ***
****************************

***************************************************************
      SUBROUTINE PHOTRACE(thetac,parthe,parphi,pvertex,ICODL)
***************************************************************
* TRACE PHOTON PATTERN
* (BOTTOM VIEW OF THE PMT MATRIX
* x and y axes in the usual disposition)
***************************************************************

#include "../include/richrec_lipf.h"

      integer nphmax
      parameter(nphmax=10)
      parameter(nphisteps=500)
      parameter(nhist_bin=2000)
      real xhist(nhist_bin),yhist(nhist_bin)
      real xhistnr(nhist_bin),yhistnr(nhist_bin)
      real xhistr(nhist_bin),yhistr(nhist_bin)

      integer verr(nphmax),icodl
      real vdet(3),vdir(3)
      real pvertex(3)

* ----init
      iuc   = 0
      iucnr = 0
      iucr  = 0

      ierrt = 0
      phibuf1 = 0.
      phibuf2 = 0.

* --- graphics definitions (ICODL = 10000*LCOL+100*LTYP+LWID)
      LCOL = icodl/10000
      LTYP = mod(icodl/100,100)
      LWID = mod(icodl,100)
      call igset('LWID',float(LWID))
      call igset('PLCI',float(LCOL))

* ----loop on photon phi
      do xphi=0.,twopi,twopi/float(nphisteps)
         /* find pattern at detector plane */
         xdet = -999.
         ydet = -999.
         call patrace_fulln(xphi,thetac,parthe,parphi,pvertex,
     +                      xdet,ydet,nmiref,nerr,verr) ! by arguments
cc         call patrace_full(xphi,thetac,xdet,ydet,nmiref,nerr,verr) ! by common
*         print*,'xdet,ydet,nmiref,nerr = ',xdet,ydet,nmiref,nerr
         if (nerr.eq.0) then
            if (nmiref.eq.0) then !no reflected point
               phidif  = abs(xphi - phibuf1)
               phibuf1 = xphi
               if (phidif.lt.1.1*twopi/float(nphisteps)) then
                  iucnr = iucnr + 1
                  xhistnr(iucnr) = xdet
                  yhistnr(iucnr) = ydet
               else
                  if (iucnr.ne.0) then
CC                     call igset('LTYP',1.)
CC                     print*,ltyp
                     call igset('LTYP',float(LTYP))
                     call igraph(iucnr,xhistnr,yhistnr,'L')
                     iucnr = 0
                  endif
               endif
            elseif (nmiref.ne.0) then
               phidif = abs(xphi - phibuf2)
               phibuf2 = xphi
               if (phidif.lt.1.1*twopi/float(nphisteps)) then
                  iucr = iucr + 1
                  xhistr(iucr) = xdet
                  yhistr(iucr) = ydet
               else
                  if (iucr.ne.0) then
                     call igset('LTYP',float(LTYP))
CC                     call igset('LTYP',2.)
                     call igraph(iucr,xhistr,yhistr,'L')
                     iucr = 0
                  endif
               endif
            endif
         endif
      enddo

* ----Draw pattern
      if (iucnr.ne.0) then
         call igset('LTYP',float(LTYP))
CC         call igset('LTYP',1.)
         call igraph(iucnr,xhistnr,yhistnr,'L')
      endif
      if (iucr.ne.0) then
         call igset('LTYP',float(LTYP))
CC           call igset('LTYP',2.)
         call igraph(iucr,xhistr,yhistr,'L')
      endif

* --- RETURN
      RETURN
      END


*********************************
*** CODE FROM photrace_prot.F ***
*********************************

      SUBROUTINE PHOTRACE_PROT
     ,     (thetac,parthe,parphi,pvertex,ICODL)
***************************************************************
* TRACE PHOTON PATTERN
* (TOP VIEW OF THE PMT MATRIX - x and y axes interchanged)
***************************************************************

#include "../include/richrec_lipf.h"

      integer nphmax
      parameter(nphmax=10)
      parameter(nphisteps=500)
      parameter(nhist_bin=2000)
      real xhist(nhist_bin),yhist(nhist_bin)
      real xhistnr(nhist_bin),yhistnr(nhist_bin)
      real xhistr(nhist_bin),yhistr(nhist_bin)

      integer verr(nphmax),icodl
      real vdet(3),vdir(3)
      real pvertex(3)

* ----init
      iuc   = 0
      iucnr = 0
      iucr  = 0

      ierrt = 0
      phibuf1 = 0.
      phibuf2 = 0.

* --- graphics definitions (ICODL = 10000*LCOL+100*LTYP+LWID)
      LCOL = icodl/10000
      LTYP = mod(icodl/100,100)
      LWID = mod(icodl,100)
      call igset('LWID',float(LWID))
      call igset('PLCI',float(LCOL))

* ----loop on photon phi
      do xphi=0.,twopi,twopi/float(nphisteps)
         /* find pattern at detector plane */
         xdet = -999.
         ydet = -999.
         call patrace_fulln
     ,        (xphi,thetac,parthe,parphi,pvertex,xdet,ydet,nmiref,nerr
     ,        ,verr)                                                ! by arguments
cc        call patrace_full(xphi,thetac,xdet,ydet,nmiref,nerr,verr) ! by common
         if (nerr.eq.0) then
            if (nmiref.eq.0) then !no reflected point
               phidif  = abs(xphi - phibuf1)
               phibuf1 = xphi
               if (phidif.lt.1.1*twopi/float(nphisteps)) then
                  iucnr = iucnr + 1
                  xhistnr(iucnr) = xdet
                  yhistnr(iucnr) = ydet
               else
                  if (iucnr.ne.0) then
                     call igset('LTYP',1.)
                     call igset('LTYP',float(LTYP))
                     call igraph(iucnr,yhistnr,xhistnr,'L')
                     iucnr = 0
                  endif
               endif
            elseif (nmiref.ne.0) then
              phidif = abs(xphi - phibuf2)
              phibuf2 = xphi
              if (phidif.lt.1.1*twopi/float(nphisteps)) then
                iucr = iucr + 1
                xhistr(iucr) = xdet
                yhistr(iucr) = ydet
              else
                if (iucr.ne.0) then
                   call igset('LTYP',float(LTYP))
                   call igset('LTYP',2.)
                  call igraph(iucr,yhistr,xhistr,'L')
                  iucr = 0
                endif
              endif
            endif
         endif
      enddo

* ----Draw pattern
      if (iucnr.ne.0) then
         call igset('LTYP',float(LTYP))
         call igset('LTYP',1.)
         call igraph(iucnr,yhistnr,xhistnr,'L')
      endif
      if (iucr.ne.0) then
           call igset('LTYP',float(LTYP))
           call igset('LTYP',2.)
         call igraph(iucr,yhistr,xhistr,'L')
      endif

* --- RETURN
      RETURN
      END


********************************
*** CODE FROM photrace_rad.F ***
********************************

      SUBROUTINE PHOTRACE_RAD(thetac,parthe,parphi,pvertex,ICODL)
***************************************************************
* TRACE PHOTON PATTERN
* (BOTTOM VIEW OF THE PMT MATRIX
* x and y axes in the usual disposition)
***************************************************************

#include "../include/richrec_lipf.h"

      integer nphmax
      parameter(nphmax=10)
      parameter(nphisteps=500)
      parameter(nhist_bin=2000)
      real xhist(nhist_bin),yhist(nhist_bin)
      real xhistnr(nhist_bin),yhistnr(nhist_bin)
      real xhistr(nhist_bin),yhistr(nhist_bin)

      integer verr(nphmax),icodl
      real vdet(3),vdir(3)
      real pvertex(3)

* ----init
      iuc   = 0
      iucnr = 0
      iucr  = 0

      ierrt = 0
      phibuf1 = 0.
      phibuf2 = 0.

* --- graphics definitions (ICODL = 10000*LCOL+100*LTYP+LWID)
      LCOL = icodl/10000
      LTYP = mod(icodl/100,100)
      LWID = mod(icodl,100)
      call igset('LWID',float(LWID))
      call igset('PLCI',float(LCOL))

* ----loop on photon phi
      do xphi=0.,twopi,twopi/float(nphisteps)
         /* find pattern at detector plane */
         xdet = -999.
         ydet = -999.
         call patrace_rad
     ,        (xphi,thetac,parthe,parphi,pvertex,xdet,ydet,nmiref,nerr
     ,        ,verr)                                                ! by arguments
         if (nerr.eq.0) then
            if (nmiref.eq.0) then !no reflected point
               phidif  = abs(xphi - phibuf1)
               phibuf1 = xphi
               if (phidif.lt.1.1*twopi/float(nphisteps)) then
                  iucnr = iucnr + 1
                  xhistnr(iucnr) = xdet
                  yhistnr(iucnr) = ydet
                  if(rcgeom(1).eq.9)then
                    xhistnr(iucnr) = ydet
                    yhistnr(iucnr) = xdet
                 endif
               else
                  if (iucnr.ne.0) then
CC                     call igset('LTYP',1.)
CC                     print*,ltyp
                     call igset('LTYP',float(LTYP))
                     call igraph(iucnr,xhistnr,yhistnr,'L')
                     iucnr = 0
                  endif
               endif
            elseif (nmiref.ne.0) then
              phidif = abs(xphi - phibuf2)
              phibuf2 = xphi
              if (phidif.lt.1.1*twopi/float(nphisteps)) then
                iucr = iucr + 1
                xhistr(iucr) = xdet
                yhistr(iucr) = ydet
                if(rcgeom(1).eq.9)then
                    xhistnr(iucnr) = ydet
                    yhistnr(iucnr) = xdet
                 endif
              else
                if (iucr.ne.0) then
                   call igset('LTYP',float(LTYP))
CC                  call igset('LTYP',2.)
                  call igraph(iucr,xhistr,yhistr,'L')
                  iucr = 0
                endif
              endif
            endif
         endif
      enddo

* ----Draw pattern
      if (iucnr.ne.0) then
         call igset('LTYP',float(LTYP))
CC         call igset('LTYP',1.)
         call igraph(iucnr,xhistnr,yhistnr,'L')
      endif
      if (iucr.ne.0) then
           call igset('LTYP',float(LTYP))
CC           call igset('LTYP',2.)
         call igraph(iucr,xhistr,yhistr,'L')
      endif


* --- RETURN
      RETURN
      END




* ================================================================
      subroutine patrace_rad
     ,     (phig,cerang,pthe,pphi,pvertex,xdet,ydet,nmiref,nerr,verr)
* ================================================================
******************************************************************
*       AIMS: Steering routine for pattern cerenkov drawing at the
*             radiator level
*
* PARAMETERS: IN - phig   = photon phi angle
*                  cerang = cerenkov angle
*            OUT - xdet   = x-coord of the photon at the radiator bottom
*                  ydet   = y-coord of the photon at the radiator bottom
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

******************************************************************

      integer nphmax
      parameter(nphmax=10)
      integer nerr,verr(nphmax)
      real vdet(3),vdir(3)
      real pthe,pphi,pvertex(3)
      real phdir(3),phpto(3)

* --- init
      nerr   = 0
      call vfill(verr,nphmax,0)
      nmiref = 0 ! number of mirror reflections or out of prototype PMT matrix
*
      vdet(1) = -999.
      vdet(2) = -999.

* --- set particle rotation matrix if not already done
      if (irotflg.eq.0) then
         call patmatrn(pthe,pphi)
         irotflg = 1
      endif

* --- function
* ... photon emission point
      call ucopy(pvertex,phpto,3)
* ... generate photon on detector frame
      call patphot(phig,cerang,phdir)
* ... test photon direction: backward?
      if (phdir(3).lt.0.) then
         nerr = nerr + 1
         verr(nerr) = 10
         return
      endif
      call patints(phpto,phdir,'RADB',ierr) !intersection with rad bottom

      if (ierr.ne.0) then
         nerr = nerr + 1
         verr(nerr) = 11        !photon going out of radiator
         return
      endif


* ... output coordinates
      call ucopy(phpto,vdet,3)

* ... output coordinates
      xdet = vdet(1)
      ydet = vdet(2)

CC      print*,'(photrace_rad) xdet,ydet=',xdet,ydet

* --- end
      return
      end


********************************
*** CODE FROM rdisdrawlike.F ***
********************************

*==================================================================
      subroutine rdisdrawlike(CHCOL,CHOPT)
*==================================================================
********************************************************************
* It draws the likelihood function L(thetac)
*
* IN :
*      CHCOL =  color ('RED','GREEN','BLUE',...)
*               see color subroutine in richdsp.F for further choices
*      CHOPT = options (not yet implemented...)
********************************************************************

#include "../include/richrec_lipf.h"

      integer npmax,np
      parameter (npmax=1000)
      real vx(npmax),vf(npmax)
      integer ind1(npmax)
      character CHCOL*4, CHOPT*4

      real fliketot_phiscan
      external fliketot_phiscan

* ... init
      call igset('LWID',5.)
      call color(1,CHCOL,icol)
      call igset('PLCI',float(icol))

* ... reset associated hit flags set at richlikemin
      do i=1,min(nbhits,nhitmax)
         if (iflghit(i).ne.1) iflghit(i)=0
      enddo

* ... draw function
      IPHIHINTFLAG = 0
      np = 0
      do x=cangreclow,cangrecup,DEGRAD*0.1
         np       = np + 1
         vx(np)   = raddeg*x
         vf(np)   = fliketotall(x)
         IPHIHINTFLAG = 1
      enddo
      call sortzv_alt(vf,ind1,np,1,1,0)
*
      xlow = RADDEG*CANGRECLOW
      xup  = RADDEG*CANGRECUP
      ylow = vf(ind1(np))
      yup  = vf(ind1(1))
C      print*,xlow,xup,ylow,yup
C      print*,(vf(ind1(j)),j=1,10)
      call iswn(10,xlow,xup,ylow,yup)
      call iselnt(10)

      call igaxis(xlow,xlow,ylow,yup,ylow,yup,10*100+7,'GA-')
      call igaxis(xlow,xup,ylow,ylow,xlow,xup,10*100+7,'A-')
      call igraph(np,vx,vf,'L')

* ... end
      return
      end
