****************************************************************************************
* In this file
* ------------
*  subroutine getpmtmax(xmax,ymax,imax,sigmax,nhmax,fracmax,sigmed)
*  subroutine uradtile(x,y,nx,ny)
*  subroutine pmtnmb_flight(XPHOT,YPHOT,IPM,IFLG)
*  subroutine pmtnmb_protn(xp,yp,xc,yc,ipm,iflg)
*  subroutine gridcoo(x,y,igrid,x0,y0)
*  subroutine uradlim(nx,ny,xleft,xright,ydown,yup,iflag)
*  subroutine pmtpos_flight(IPM,XL,XR,YU,YD,XC,YC,IFLG)
*  subroutine pmtpos_protn(ipm,ix,iy,xl,xr,yu,yd,xc,yc)
*  subroutine flatness(iopt,residcut,flatsin,flatcos)
*  subroutine dist2patt(thetac,xpoint,ypoint,d2point,phipoint,irefpoint)
*  Subroutine probchg(chg0,sig0,chgm,prob0)
*  subroutine richpkolm(dphistep,probkol)
*  double precision function normg(x)
*
*  added cernlib numerical libs on 23 Oct 2010
*
*  rzero
*  dgmlt1
*  dgmlt2
*
*  Function betaresLIP(Z)
***************************************************************************************

*****************************
*** CODE FROM getpmtmax.F ***
*****************************

*------------------------------------------------------------------
      subroutine getpmtmax(xmax,ymax,imax,sigmax,nhmax,fracmax,sigmed)
*------------------------------------------------------------------
*******************************************************************
*
* Returns the information on max PMT
*
* OUT:
*      xmax,ymax   = coordinates of strongest pixel inside PMT imax
*      imax(5)     = PMT id sorted from signal
*      sigmax(5)   = PMT signal sorted
*      nhmax(5)    = NB HITS associated to max PMT
*      fracmax     = hit max fraction within pmt max
*      sigmed      = pmt signal averaged
*      io          = flag. = 0 ok (negative -> no answer)
*
*******************************************************************

#include "../include/richrec_lipf.h"

      integer npmtmax
      parameter(npmtmax=680)
      real sig_pmt(npmtmax),coo_pmt(2,npmtmax)
      integer nbhits_pmt(npmtmax),index_pmt(npmtmax)

      real xmax,ymax,sigtot,sigmax(5)
      integer imax(5),nhmax(5)
      real hit_pmtmax,fracmax

* --- init
      xmax  = -999.
      ymax  = -999.
      sigtot = 0.
      do i=1,5
         imax(i)   = -1
         sigmax(i) = -999.
         nhmax(i) = 0
      enddo

      do i=1,npmtmax
         sig_pmt(i)   = 0.
         nbhits_pmt(i)= 0
         coo_pmt(1,i) = 0.
         coo_pmt(2,i) = 0.
      enddo

* --- protection
      if (nbhits.eq.0) return

* --- get pmt signal
      do i=1,min(nbhits,nhitmax)
* ...... decode pmt number
         ipmt = hitspmt(i)/10/16 ! =0,...,98 (prototype,0...679 flight)
*         print*,'ipmt = ',ipmt
* ...... get signals per pmt
         sig_pmt(ipmt+1)=sig_pmt(ipmt+1)+hitsnpe(i)
*         print*,'signal = ',hitsnpe(i)
* ...... gravity center for each activated pmt
         coo_pmt(1,ipmt+1)=coo_pmt(1,ipmt+1)+hitscoo(1,i)*hitsnpe(i)
         coo_pmt(2,ipmt+1)=coo_pmt(2,ipmt+1)+hitscoo(2,i)*hitsnpe(i)
* ...... occupancy of pmt
         nbhits_pmt(ipmt+1)=nbhits_pmt(ipmt+1)+1
* ...... total signal
         sigtot = sigtot + hitsnpe(i)
      enddo

* --- sort signals (descendent order)
      call sortzv_alt(sig_pmt,index_pmt,npmtmax,+1,1,0)

* --- count nb of pmts with hits
      ipmtc = 0
      do i=1,npmtmax
         if (nbhits_pmt(i).gt.0) ipmtc = ipmtc + 1
      enddo
      sigmed = sigtot/float(ipmtc)

* --- output
      do i=1,5
         imax(i)    = index_pmt(i)
         sigmax(i)  = sig_pmt(index_pmt(i))
         nhmax(i)   = nbhits_pmt(index_pmt(i))
      enddo

* --- use highest pixel to define particle impact point
      hit_pmtmax=-999.
      do i=1,min(nbhits,nhitmax)
         ipmt = (hitspmt(i)/10)/16 ! goes from 0 to (98)
* ...... select pmt of maximum signal
         if ((ipmt+1).eq.imax(1))then
            if (hitsnpe(i).gt.hit_pmtmax)then
               hit_pmtmax=hitsnpe(i)
* ............ Take highest activated pixel inside highest activated PMT
*              to define alignment coordinates
               xmax=hitscoo(1,i)
               ymax=hitscoo(2,i)
               imaxx=i
            endif
         endif
      enddo

* ... max hit fraction
      fracmax = -999.
      if (sigmax(1).gt.0.) fracmax = hit_pmtmax/sigmax(1)


* --- end
 999  continue
      return
      end


****************************
*** CODE FROM uradtile.F ***
****************************

      subroutine uradtile(x,y,nx,ny)
*****************************************************************************
*
*  uradtile: get tile number on x and y for a set of coordinates (x,y)
*
*  |.
*  |(0,1)
*  |(0,0) (1,0) (2,0) ...
*  |--------
*
*  Tiles nb: -5...0...5
*
* last update: 25/05/2004 (F.Barao)
* corrected  : 27/11/2008 (R.Pereira)
*
*****************************************************************************

#include "../include/richrec_lipf.h"

      real x,y
      integer nx,ny
      real half_tile

* ... get numbers
/*      half_tile=(radtile_pitch)/2.
      if(abs(x).lt.half_tile)then
         nx = int(x/half_tile)
      else
         xx=x+sign(1.,x)*half_tile
         nx = int( xx/(radtile_pitch))
      endif
      if(abs(y).lt.half_tile)then
         ny = int( y/half_tile)
      else
         yy=y+sign(1.,y)*half_tile
         ny = int( yy/(radtile_pitch))
      endif
*/

C ----------------------------------------
      IF (RADTILE_PITCH.LT.12.) THEN
C ----------------------------------------

      half_tile = (radtile_pitch-radtile_supthk)/2.
      if (abs(x).lt.half_tile) then
         nx = int(x/half_tile)
      else
*         xx = x+sign(1.,x)*half_tile+sign(1.,x)*radtile_supthk
         xx = x+sign(1.,x)*half_tile+sign(1.,x)*radtile_supthk/2.
         nx = int( xx/(radtile_pitch))
      endif
      if (abs(y).lt.half_tile) then
         ny = int( y/half_tile)
      else
*         yy = y+sign(1.,y)*half_tile+sign(1.,y)*radtile_supthk
         yy = y+sign(1.,y)*half_tile+sign(1.,y)*radtile_supthk/2.
         ny = int( yy/(radtile_pitch))
      endif

C ----------------------------------------
      ELSE
C ----------------------------------------

      radtile_net = radtile_pitch - radtile_supthk
      nx = int(x/radtile_net) + int(sign(1.,x))
      ny = int(y/radtile_net) + int(sign(1.,y))
CC      print*,'(uradtile) tile=',nx,ny

C ----------------------------------------
      ENDIF
C ----------------------------------------

*      nx = sign(nx,x)
*      ny = sign(ny,y)

* ... end
      return
      end


*********************************
*** CODE FROM pmtnmb_flight.F ***
*********************************

      subroutine pmtnmb_flight(XPHOT,YPHOT,IPM,IFLG)
**************************************************************************
* AIMS  : get pmt nmb and flag (used by pattrace to flag path through matrix)
* PARAMS:
*        IN - XPHOT,YPHOT
*        OUT- IPM  PMT number
*             IFLG  0 - good PMT
*                   1 - no PMT at this position
*                   3 - bad PMT (dead areas)
*
*     Updated: April 2011 (R. Pereira) - IFLG now comes from external array
*                                        intermediate determinations of IFLAG
*                                        maintained since they are used mid-code,
*                                        but overriden by final change
**************************************************************************

#include "../include/richrec_lipf.h"

      integer IPM,IFLG
      real XPHOT,YPHOT,RPHOT,dxp,dyp

*      PITCH=3.7
      IP   = -1
      IPM  = -1
      IFLG =  3
*
      PITCHX=PMTWX+SHIELDW
      PITCHY=PMTWY+SHIELDW

      XLIMV=(17*PITCHX + JUMP)/2.
      XLIMH=XLIMV + DMECX
      YLIMH=(17*PITCHY)/2.
      YLIMV= YLIMH + DMECY
* move to matrix coordinates
*      XPHOT=XPHOT - XCPMM
*      YPHOT=YPHOT - YCPMM

      IF(ABS(YPHOT).LT.YLIMH) THEN  ! 1ST/6TH PLANES (153 PMTS EACH)
       IF(ABS(XPHOT).GT.XLIMH.AND.ABS(XPHOT).LT.XLIMH+9*PITCHX)THEN
          IFLG =  0
          DXP=ABS(ABS(XPHOT)-XLIMH)
          DYP=ABS(YPHOT+SIGN(YLIMH,XPHOT))

          IX=(INT(DXP/PITCHX)+1)
          IY=(INT(DYP/PITCHY)+1)

          IP= (IY-1)*9 + (IX-1)
          IF (XPHOT.GT.0) THEN
           IPM  = IP
          ELSE
           IPM  = IP + 2*(153+36)
          ENDIF

          IF(IP.EQ.  0.OR.IP.EQ.144.OR.
     &       IP.EQ.  7.OR.IP.EQ.  8.OR.IP.EQ. 17.OR.IP.EQ.26.OR.
     &       IP.EQ.134.OR.IP.EQ.143.OR.IP.EQ.151.OR.IP.EQ.152) IFLG=1
         ENDIF

      ELSEIF(
     & (ABS(XPHOT).LT.(XLIMV)).AND.
     &   ABS(YPHOT).GT.YLIMV.AND.
     &   ABS(YPHOT).LT.YLIMV+9*PITCHY) THEN ! TOP/BOTTOM PLANES : 3RD/4th/8TH/9th
             IF((XPHOT.LT.-DX0.AND.YPHOT.GT.0).OR.
     &          (XPHOT.GT. DX0.AND.YPHOT.LT.0))THEN
               DXP=ABS(XPHOT+SIGN(-JUMP+XLIMV,XPHOT))
               IFLG = 0
             ELSEIF((XPHOT.GT.-DX0+JUMP.AND.YPHOT.GT.0).OR.
     &         (XPHOT.LT.DX0-JUMP.AND.YPHOT.LT.0)) THEN
               DXP=ABS(ABS(XPHOT)-XLIMV)
               IFLG = 0
             ENDIF
             DYP=ABS(ABS(YPHOT)-YLIMV)
*
             IF (IFLG.EQ.0) THEN
               IX=(INT(DXP/PITCHX)+1)
               IY=(INT(DYP/PITCHY)+1)
               IP=(IX-1)*9 + (IY-1)

               IF(YPHOT.GT.0) THEN
                 IPM= IP+153+36
               ELSE
                 IPM= IP+3*(153+36)
               ENDIF

               IF(IP.EQ.  0.OR.IP.EQ.144.OR.
     &              IP.EQ.  7.OR.IP.EQ.  8.OR.IP.EQ. 17.OR.IP.EQ.26.OR.
     &              IP.EQ.134.OR.IP.EQ.143.OR.IP.EQ.151.OR.IP.EQ.152)
     &              IFLG=1

             ENDIF

      ELSEIF(ABS(XPHOT).gt.XLIMH.AND.
     &       ABS(YPHOT).gt.YLIMV) THEN  ! CORNER PLANES

        DXP=ABS(ABS(XPHOT)-XLIMH)
        DYP=ABS(ABS(YPHOT)-YLIMV)

        IX=(INT(DXP/PITCHX)+1)
        IY=(INT(DYP/PITCHY)+1)
        IP=0


        IF(XPHOT*YPHOT.GT.0) THEN ! 2nd/5th planes (36 each)
         DO I=1,IY-1
            IP=IP+(8-(I-1))
         ENDDO
         IP=IP+(IX-1)
          IF(XPHOT.GT.0) THEN
           IPM= IP+153
          ELSE
           IPM= IP+3*153+2*36
          ENDIF

        ELSEIF(XPHOT*YPHOT.LT.0)then    ! 7nd/10th planes (36 each)
         DO I=1,IX-1
            IP=IP+(8-(I-1))
         ENDDO
         IP=IP+(IY-1)
         IF(XPHOT.GT.0) THEN
           IPM= IP+4*153+3*36
          ELSE
           IPM= IP+2*153+36
          ENDIF
        ENDIF

       IF ((IY-1)+(IX-1).LE.6) THEN
         IFLG=0
       ELSEIF ((IY-1)+(IX-1).LE.6) THEN
         IFLG=1
       ELSE
         IFLG=3
         IPM=-1
       ENDIF

       IF(IP.EQ. 0.OR.IP.EQ. 7.OR.IP.EQ.14.OR.IP.EQ.20.OR.IP.EQ.25.OR.
     &     IP.EQ.29.OR.IP.EQ.32.OR.IP.EQ.34.OR.IP.EQ.35) IFLG=1

      ELSE
       IFLG=3
      ENDIF

* --- Final change overrides all previous changes to IFLG
      IFLG = IPMTSTAT(IPM+1)

* --- END
      END


********************************
*** CODE FROM pmtnmb_protn.F ***
********************************

      subroutine pmtnmb_protn(xp,yp,xc,yc,ipm,iflg)
**************************************************************************
* AIMS  : get x and y position of pmt
* PARAMS:
*        IN -  xp   =
*              yp   =
*        OUT-  xc   = pmt x center
*              yc   = pmt y center
*              ipm  = PMT nb
*              iflg = PMT FLAG
*
* PROTOTYPE ISN PMT COUNTING
*
*                65 |54 43 32 21  10
*       97 86 75    |   42 31 20   9
*                   |   41 30 19   8
*              .    |   40 29 18   7
*              .    ----39-28-17---6-> X
*              .    |   38 27 16   5
*                   |   37 26 15   4
*                   |   36 25 14   3
*                   |   35 24 13   2
*                   |   34 23 12   1
*       88 77 66 55 |44 33 22 11   0
*                  \/ Y
*
*
*              ----------------------------> x
*             |         Ydown
*             |      ----------
*             |     |          |
*             |     |          |
*             |     |Xleft     |Xright
*             |     |          |
*             |     |          |
*             |      ----------
*             |         Yup
*             |
*            \/ Y
*
**************************************************************************

#include "../include/richrec_lipf.h"

      integer ipm,iflg
      real xp,yp,xc,yc

* --- init
      IPM=-1
      IFLG=3
      XC=0.
      YC=0.
*
      PITCHX=PMTWX+SHIELDW
      PITCHY=PMTWY+SHIELDW
*
      if(xp.gt.-xlimprot+xcpmm.and.xp.lt.xlimprot+xcpmm.and.
     &   yp.gt.-ylimprot+ycpmm.and.yp.lt.ylimprot+ycpmm )then
         dxp=xlimprot-xp+xcpmm
         dyp=ylimprot-yp+ycpmm
         ix=int(dxp/pitchx)+1
         if(dxp/pitchx.ge.10.)ix=10
         iy=int(dyp/pitchy)+1
         if(dyp/pitchy.ge.11.)iy=11
*
         XC =XLIMPROT - (IX+0.5)*(PITCHX+XCPMM)
         YC =YLIMPROT - (IY+0.5)*(PITCHY+YCPMM)
*
         IPM  = (IX-1)*11 + (IY-1)
*         write(*,*) 'ix,iy,ipm,iflg'
*         write(*,*) ix,iy,ipm,iflg
*         print *,dxp,dxp/pitchx
*         print *,dyp,dyp/pitchy
         IFLG = IFLPMT(IX,IY)
      ENDIF
* --- end
      return
      end


***************************
*** CODE FROM gridcoo.F ***
***************************

      subroutine gridcoo(x,y,igrid,x0,y0)
**************************************************************************************
* Returns grid number according to J. Borges and corner (frame) coordinates
*
* IN :  x,y   - photon coordinates
* OUT:  igrid - grid number (J.Borges numbering)
*       x0    - corner coordinates
*       y0    -
*
**************************************************************************************

#include "../include/richrec_lipf.h"

      PITCHX=PMTWX+SHIELDW
      PITCHY=PMTWY+SHIELDW
      XLIMV=(17*PITCHX + JUMP)/2.
      XLIMH=XLIMV + DMECX
      YLIMH=(17*PITCHY)/2.
      YLIMV= YLIMH + DMECY


* --- module 1
      if (x.gt.XLIMH .and. abs(y).lt.YLIMH) then
         igrid = 1
         x0 = +XLIMH
         y0 = -YLIMH
      endif

* --- module 2
      if (x.gt.XLIMH .and. y.gt.YLIMV) then
         igrid = 2
         x0 = +XLIMH
         y0 = +YLIMV
      endif

* --- module 3
      if (x.gt.(XLIMV-9.*LG_PITCH) .and. x.lt. XLIMV
     +                             .and. y.gt. YLIMV) then
         igrid = 3
         x0 =  XLIMV-9.*LG_PITCH
         y0 = +YLIMV
      endif

* --- module 4
      if (x.lt.(-XLIMV+8.*LG_PITCH) .and. x.gt. -XLIMV
     +                              .and. y.gt.  YLIMV) then
         igrid = 4
         x0 = -XLIMV+8.*LG_PITCH
         y0 = +YLIMV
      endif


* --- module 5
      if (x.lt.-XLIMH .and. y.gt.YLIMV) then
         igrid = 5
         x0 = -XLIMH-7.*LG_PITCH
         y0 = +YLIMV
      endif

* --- module 6
      if (x.lt.-XLIMH .and. abs(y).lt.YLIMH) then
         igrid = 6
         x0 = -XLIMH-9.*LG_PITCH
         y0 = -YLIMH
      endif

* --- module 7
      if (x.lt.-XLIMH .and. y.lt.-YLIMV) then
         igrid = 7
         x0 = -XLIMH-7.*LG_PITCH
         y0 = -YLIMV
      endif

* --- module 8
      if (x.lt.(-XLIMV+9.*LG_PITCH) .and. x.gt. -XLIMV
     +                              .and. y.lt. -YLIMV) then
         igrid = 8
         x0 = -XLIMV+9.*LG_PITCH
         y0 = -YLIMV
      endif

* --- module 9
      if (x.gt.(XLIMV-8.*LG_PITCH) .and. x.lt. XLIMV
     +                             .and. y.lt.-YLIMV) then
         igrid = 9
         x0 =  XLIMV-8.*LG_PITCH
         y0 = -YLIMV
      endif

* --- module 10
      if (x.gt.+XLIMH .and. y.lt.-YLIMV) then
         igrid = 10
         x0 = +XLIMH
         y0 = -YLIMV-7.*LG_PITCH
      endif

      end


***************************
*** CODE FROM uradlim.F ***
***************************

      subroutine uradlim(nx,ny,xleft,xright,ydown,yup,iflag)
*****************************************************************************
*
*  uradlim
*
*  get tile x and y limits
*
*  PARAMS:
*      IN - nx,ny (tile indices -5...0...5)
*     OUT - xleft, xright (tile x square limits)
*         - yup, ydown (tile y square limits)
*         - iflag
*                  =2 external tile;
*                  =1 boundary tile;
*                  =0 inner tile
*
* last update: 25/05/2004 (L.Arruda,F.Barao)
*
*****************************************************************************

#include "../include/richrec_lipf.h"

      integer nx,ny,iflag
      real xleft,xright,ydown,yup
      real radtile_radius(4)
      integer radtile_index(4)

      iflag = 0

      xright = -999.
      xleft  = -999.
      yup    = -999.
      ydown  = -999.


C ----------------------------------------
      IF (RADTILE_PITCH.LT.12.) THEN
C ----------------------------------------


      if (abs(nx).eq.5.and.abs(ny).eq.5 .or.
     +    abs(nx).eq.4.and.abs(ny).eq.5 .or.
     +    abs(nx).eq.3.and.abs(ny).eq.5 .or.
     +    abs(nx).eq.5.and.abs(ny).eq.4 .or.
     +    abs(nx).eq.5.and.abs(ny).eq.3
     +   ) then
         iflag = 2
         return
      endif

* ... get x limits
      xright =  radtile_pitch/2. + sign(1,nx)*abs(nx)*radtile_pitch
      xleft  = -radtile_pitch/2. + sign(1,nx)*abs(nx)*radtile_pitch
      xright =  xright - sign(1.,xright)*radtile_supthk !correct for wall thickness
      xleft  =  xleft  - sign(1.,xleft)*radtile_supthk !correct for wall thickness

* ... get y limits
      yup    =  radtile_pitch/2. + sign(1,ny)*abs(ny)*radtile_pitch
      ydown  = -radtile_pitch/2. + sign(1,ny)*abs(ny)*radtile_pitch
      yup    =  yup - sign(1.,yup)*radtile_supthk !correct for wall thickness
      ydown  =  ydown  - sign(1.,ydown)*radtile_supthk !correct for wall thickness

C ----------------------------------------
      ELSE
C ----------------------------------------

* ... get x limits
      if (nx.gt.0) then
         xright =  real(nx)*radtile_pitch
         xleft  =  real(nx-1)*radtile_pitch
      else
         xleft  =  real(nx)*radtile_pitch
         xright =  real(nx+1)*radtile_pitch
      endif
      xright =  xright - sign(1.,xright)*radtile_supthk !correct for wall thickness
      xleft  =  xleft  - sign(1.,xleft)*radtile_supthk !correct for wall thickness

* ... get y limits
      if (ny.gt.0) then
         yup    =  real(ny)*radtile_pitch
         ydown  =  real(ny-1)*radtile_pitch
      else
         ydown  =  real(ny)*radtile_pitch
         yup    =  real(ny+1)*radtile_pitch
      endif
      yup   =  yup    - sign(1.,yup)*radtile_supthk !correct for wall thickness
      ydown =  ydown  - sign(1.,ydown)*radtile_supthk !correct for wall thickness

CC      print*,'(uradlim) xleft,xright,ydown,yup=',xleft,xright,ydown,yup


C ----------------------------------------
      ENDIF
C ----------------------------------------


* ... flag boundary tiles
      radtile_radius(1) = sqrt(xleft**2 + ydown**2)
      radtile_radius(2) = sqrt(xleft**2 + yup**2)
      radtile_radius(3) = sqrt(xright**2 + ydown**2)
      radtile_radius(4) = sqrt(xright**2 + yup**2)

      call sortzv_alt(radtile_radius,radtile_index,4,1,1,0) !descending order

      if (radtile_radius(radtile_index(1)).gt.rtmir .and.
     +    radtile_radius(radtile_index(4)).lt.rtmir) iflag=1

      if (radtile_radius(radtile_index(1)).gt.rtmir .and.
     +    radtile_radius(radtile_index(4)).gt.rtmir) iflag=2



* ... end
      return
      end


*********************************
*** CODE FROM pmtpos_flight.F ***
*********************************

      subroutine pmtpos_flight(IPM,XL,XR,YU,YD,XC,YC,IFLG)
**************************************************************************
* AIMS  : get x and y position of pmt  (used by DISPLAY and by acceptance)
* PARAMS:
*        IN - pmtnb  = pmt number (0...755 - include virtual pmts)
*
*        OUT- xleft  = X left limit
*             xright = X right limit
*             yup    = Y up limit
*             ydown  = Y down limit
*             xcent  = X center
*             ycent  = Y center
*        IFLG 1 - external contour -first out
*             1 - internal non existing PMT
*             3 - external non existing PMT
*
* ****************************************************************************************
*
*         RICH PMT GRID SCHEME
*         --------------------
*         (using mechanical frame)
*
*         GRIDS                                  | #REAL PMTS | #REAL+VIRTUAL PMTS
*         ------------------------------------------------------------------------
*         module 1 (rectangular grid x>0)        | 143        | 153
*         module 2 (triangular grid x>0, y>0)    |  27        |  36
*         module 3 (rectangular grid x>0,y>0)    |  76        |  81
*         module 4 (rectangular grid x<0,y>0)    |  67        |  72
*         module 5 (triangular grid x<0, y>0)    |  27        |  36
*         module 6 (rectangular grid x<0)        | 143        | 153
*         module 7 (triangular grid x<0, y<0)    |  27        |  36
*         module 8 (rectangular grid x<0,y<0)    |  76        |  81
*         module 9 (rectangular grid x>0,y<0)    |  67        |  72
*         module 10 (triangular grid x>0, y<0)   |  27        |  36
*                                                  ---          ---
*                                                  680          756
*
********************************************************************************************

#include "../include/richrec_lipf.h"

      integer IPM,IFLG
      real XL,XR,YU,YD,XC,YC
*(153)
      PITCHX=PMTWX+SHIELDW
      PITCHY=PMTWY+SHIELDW
*
      IDUM=1
      IFLG=0
*      XLIMV=(16*PITCHX + PMTWX + DMODX)/2.
      XLIMV=(17*PITCHX + JUMP)/2.
      XLIMH=XLIMV + DMECX
*      YLIMH=(16*PITCHY + PMTWY + SHIELDW)/2.
      YLIMH=(17*PITCHY)/2.
      YLIMV= YLIMH + DMECY
*
      IF(IPM.LT.153) THEN
       IP=IPM

       IX = MOD(IP,9) +1
       IY = INT(IP/9.)+1

       XL = (IX-1)*PITCHX
       XR = (IX)*PITCHX
       YD = (IY-1)*PITCHY
       YU = (IY)*PITCHY

       XL=  XL + XLIMH
       XR=  XR + XLIMH
*
       YU= -YLIMH  +  YU
       YD= -YLIMH  +  YD

       IF(IP.EQ.  0.OR.IP.EQ.144.OR.
     &       IP.EQ.  7.OR.IP.EQ.  8.OR.IP.EQ. 17.OR.IP.EQ.26.OR.
     &       IP.EQ.134.OR.IP.EQ.143.OR.IP.EQ.151.OR.IP.EQ.152) IFLG=1
*
      ELSEIF(IPM.GE.153.AND.IPM.LT.189) THEN

       IP=IPM-153
       IDIF=IP
       DO I=1,8
        K=(8-(I-1))
        IF(IDIF.GE.K) THEN
          IDIF=IDIF-K
        ELSE
          IY=I
          IX=IDIF+1
          GOTO 11
        ENDIF
       ENDDO

 11    XL =  (IX-1)*PITCHX
       XR =  (IX)*PITCHX
       YD =  (IY-1)*PITCHY
       YU =  (IY)*PITCHY

       XL=XL+XLIMH
       XR=XR+XLIMH
       YD=YD+YLIMV
       YU=YU+YLIMV

        IF(IP.EQ. 0.OR.IP.EQ. 7.OR.IP.EQ.14.OR.IP.EQ.20.OR.IP.EQ.25.OR.
     &     IP.EQ.29.OR.IP.EQ.32.OR.IP.EQ.34.OR.IP.EQ.35) IFLG=1

      ELSEIF(IPM.GE.189.AND.IPM.LT.342) THEN
* 3RD AND 4TH PLANE
       IP=IPM-189
       IY = MOD(IP,9) + 1
       IX = INT(IP/9.)+ 1

       XR =  -(IX-1)*PITCHX
       XL =  -(IX)*PITCHX
       YD =   (IY-1)*PITCHY
       YU =   (IY)*PITCHY

       XL=XL + XLIMV
       XR=XR + XLIMV
       IF(IX.GE.10) THEN
        XL = XL - JUMP
        XR = XR - JUMP
       ENDIF
       YD=YD+YLIMV
       YU=YU+YLIMV

       IF(IP.EQ.  0.OR.IP.EQ.144.OR.
     &       IP.EQ.  7.OR.IP.EQ.  8.OR.IP.EQ. 17.OR.IP.EQ.26.OR.
     &       IP.EQ.134.OR.IP.EQ.143.OR.IP.EQ.151.OR.IP.EQ.152) IFLG=1

      ELSEIF(IPM.GE.342.AND.IPM.LT.378) THEN ! 5TH PLANE

       IP=IPM-342
       IDIF=IP
       DO I=1,8
        K=(8-(I-1))
        IF(IDIF.GE.K) THEN
         IDIF=IDIF-K
        ELSE
         IX=I
         IY=IDIF+1
         goto 12
        endif
       enddo

 12    XR =  -(IX-1)*PITCHX
       XL =  -(IX)*PITCHX
       YD =   (IY-1)*PITCHY
       YU =   (IY)*PITCHY

       XL=XL- XLIMH
       XR=XR- XLIMH
       YD=YD+ YLIMV
       YU=YU+ YLIMV

        IF(IP.EQ. 0.OR.IP.EQ. 7.OR.IP.EQ.14.OR.IP.EQ.20.OR.IP.EQ.25.OR.
     &     IP.EQ.29.OR.IP.EQ.32.OR.IP.EQ.34.OR.IP.EQ.35) IFLG=1

      ELSEIF(IPM.GE.378.AND.IPM.LT.531) THEN !  6TH PLANE

       IP=IPM-378

       IX = MOD(IP,9) +1
       IY = INT(IP/9.)+1

       XR = -(IX-1)*PITCHX
       XL = -(IX)*PITCHX
       YU = -(IY-1)*PITCHY
       YD = -(IY)*PITCHY

       XL= XL - XLIMH
       XR= XR - XLIMH
       YU= YU + YLIMH
       YD= YD + YLIMH

          IF(IP.EQ.  0.OR.IP.EQ.144.OR.
     &       IP.EQ.  7.OR.IP.EQ.  8.OR.IP.EQ. 17.OR.IP.EQ.26.OR.
     &       IP.EQ.134.OR.IP.EQ.143.OR.IP.EQ.151.OR.IP.EQ.152) IFLG=1

      ELSEIF(IPM.GE.531.AND.IPM.LT.567) THEN ! 7TH PLANE(LOWER LEFT CORNER)

       IP=IPM-531
       IDIF=IP
       DO I=1,8
        K=(8-(I-1))
        IF(IDIF.GE.K) THEN
          IDIF=IDIF-K
        ELSE
          IY=I
          IX=IDIF+1
          GOTO 13
        ENDIF
       ENDDO

 13    XR =  -(IX-1)*PITCHX
       XL =  -(IX)*PITCHX
       YU =  -(IY-1)*PITCHY
       YD =  -(IY)*PITCHY

       XL=XL-XLIMH
       XR=XR-XLIMH
       YD=YD-YLIMV
       YU=YU-YLIMV

        IF(IP.EQ. 0.OR.IP.EQ. 7.OR.IP.EQ.14.OR.IP.EQ.20.OR.IP.EQ.25.OR.
     &     IP.EQ.29.OR.IP.EQ.32.OR.IP.EQ.34.OR.IP.EQ.35) IFLG=1

      ELSEIF(IPM.GE.567.AND.IPM.LT.720) THEN !  8TH PLANE(LOWER LEFT)

       IP=IPM-567

       IX = INT(IP/9.)+ 1
       IY = MOD(IP,9) + 1

       XL =  (IX-1)*PITCHX
       XR =  (IX)*PITCHX
       YU = -(IY-1)*PITCHY
       YD = -(IY)*PITCHY

       XL=XL - XLIMV
       XR=XR - XLIMV
       IF(IX.GE.10) THEN
        XL = XL + JUMP
        XR = XR + JUMP
       ENDIF
       YD=YD - YLIMV
       YU=YU - YLIMV

          IF(IP.EQ.  0.OR.IP.EQ.144.OR.
     &       IP.EQ.  7.OR.IP.EQ.  8.OR.IP.EQ. 17.OR.IP.EQ.26.OR.
     &       IP.EQ.134.OR.IP.EQ.143.OR.IP.EQ.151.OR.IP.EQ.152) IFLG=1
      ELSEif(ipm.ge.720.and.ipm.le.755) then
* 10th plane(lower right corner)
       IP=IPM-720
       IDIF=IP
       DO I=1,8
        K=(8-(I-1))
        IF(IDIF.GE.K) THEN
          IDIF=IDIF-K
        ELSE
          IX=I
          IY=IDIF+1
          GOTO 14
        ENDIF
       ENDDO

 14    XL =  (IX-1)*PITCHX
       XR =  (IX)*PITCHX
       YU =  -(IY-1)*PITCHY
       YD =  -(IY)*PITCHY

       XL=XL + XLIMH
       XR=XR + XLIMH
       YD=YD - YLIMV
       YU=YU - YLIMV

        IF(IP.EQ. 0.OR.IP.EQ. 7.OR.IP.EQ.14.OR.IP.EQ.20.OR.
     & IP.EQ.25.OR.IP.EQ.29.OR.IP.EQ.32.OR.IP.EQ.34.OR.IP.EQ.35)IFLG=1
      ELSE
       IFLG=3
      ENDIF

      XR=XR+XCPMM
      YU=YU+YCPMM
      XL=XL+XCPMM
      YD=YD+YCPMM

      XC =(XL+XR)/2.
      YC =(YU+YD)/2.

* --- end
      return
      end


********************************
*** CODE FROM pmtpos_protn.F ***
********************************

      subroutine pmtpos_protn(ipm,ix,iy,xl,xr,yu,yd,xc,yc)
**************************************************************************
* AIMS  : get x and y position of pmt
* PARAMS:
*        IN - pmtnb  = pmt number
*        OUT- xleft  = X left limit
*             xright = X right limit
*             yup    = Y up limit
*             ydown  = Y down limit
*             xcent  = X center
*             ycent  = Y center
*
* PROTOTYPE ISN PMT COUNTING
*
*                          32 21  10
*                       42 31 20   9
*                       41 30 19   8
*                       40 29 18   7
*                   ----39-28-17---6-> X
*                   |   38 27 16   5
*                   |   37 26 15   4
*                   |   36 25 14   3
*                   |   35 24 13   2
*                   |   34 23 12   1
*              ...  |   33 22 11   0
*                  \/ Y
*
*
*              ----------------------------> x
*             |         Ydown
*             |      ----------
*             |     |          |
*             |     |          |
*             |     |Xleft     |Xright
*             |     |          |
*             |     |          |
*             |      ----------
*             |         Yup
*             |
*            \/ Y
*
**************************************************************************

#include "../include/richrec_lipf.h"

      integer ipm
      real xl,xr,
     +     yu,yd,
     +     xc,yc
* --- init
* --- pointers
      IX = INT(IPM/11)
      IY = MOD(IPM,11)
* --- coordinates
      XR = XLIMPROT-(IX)  *(PMTWX+SHIELDW)+XCPMM
      XL = XLIMPROT-(IX+1)*(PMTWX+SHIELDW)+XCPMM
      YU = YLIMPROT-(IY)  *(PMTWY+SHIELDW)+YCPMM
      YD = YLIMPROT-(IY+1)*(PMTWY+SHIELDW)+YCPMM
      XC = (XR+XL)/2.
      YC = (YU+YD)/2.
* --- output pointers
      IX = IX+1
      IY = IY+1
* --- end
      return
      end


****************************
*** CODE FROM flatness.F ***
****************************

*-------------------------------------------------------
      subroutine flatness(iopt,residcut,flatsin,flatcos)
*-------------------------------------------------------
*     IN: iopt      = 0  (use signal to make a weighted average)
*                   = 1  (simple average - Compare to Kolmogorov test)
*         residcut
*
*     OUT: flatsin
*          flatcos
*-------------------------------------------------------

#include "../include/richrec_lipf.h"

* ... init
      flatsin = -999.
      flatcos = -999.

* ... loop on hits
      if (ipthetac.ne.0) then

         nb_ring=0
         ringsignal= 0.0
         flatsin   = 0.0
         flatcos   = 0.0
         do i=1,min(nbhits,nhitmax)
C            if (iflghit(i).ne.1) then
            if (iflghit(i).gt.0) then
               if (chi2hit(ipthetac,i).gt.0.) then
                  hits_res = sqrt(chi2hit(ipthetac,i))
               else
                  hits_res = 0.0
               endif
*               print*,'hits_res = ',hits_res
               phi_hit  = phihit(ipthetac,i)
               if (hits_res.lt.residcut) then
                  if (iopt.eq.1) then
                     flatsin=flatsin+hitsnpe(i)*sin(phi_hit)
                     flatcos=flatcos+hitsnpe(i)*cos(phi_hit)
                  elseif (iopt.eq.0) then
                     flatsin=flatsin+sin(phi_hit)
                     flatcos=flatcos+cos(phi_hit)
                  endif
                  ringsignal=ringsignal+hitsnpe(i)
                  nb_ring=nb_ring+1
               endif
            endif
         enddo

         if (nb_ring.gt.0) then
            if (iopt.eq.1) then
               flatsin=flatsin/ringsignal
               flatcos=flatcos/ringsignal
            elseif (iopt.eq.0) then
               flatsin=flatsin/real(nb_ring)
               flatcos=flatcos/real(nb_ring)
            endif
         else
            flatsin = -999.
            flatcos = -999.
         endif

      endif


* ... end
      return
      end


*****************************
*** CODE FROM dist2patt.F ***
*****************************

* ========================================================================
      subroutine dist2patt(thetac,xpoint,ypoint,
     +                     d2point,phipoint,irefpoint)
* ========================================================================
***************************************************************************
*  get point from photon pattern closer to the given point (x,y)
*
*  PARAMETERS:
*         IN : thetac = cerenkov angle
*                       xpoint = x coord
*                       ypoint = y coord
*         OUT: d2point  = distance squared
*              phipoint = phi
*              irefpoint= reflection flag
*
***************************************************************************

#include "../include/richrec_lipf.h"

      integer nstmax, irefpoint
      real d2point, xpoint,ypoint,phipoint
      real funcbuf, phibuf, phphin1, phphin2

      real fchi2hit
      external fchi2hit

* --- INIT
* ... tolerances
      phitol = 0.01*degrad
* ... photon phi
      phphi1 = 180.*degrad
      phphi2 = 181.*degrad
      phphi3 = 0.
      funcphi1 = 0.
      funcphi2 = 0.
      funcphi3 = 0.
* ... hit coordinates
      hypthc   = thetac
      curhit(1)= xpoint
      curhit(2)= ypoint

* --- sampling the distance function
      funcbuf = 1.E9
      nstmax = int(360./PHISTEP) + 1 ! nbr intervals for phi
      do iv = 1,nstmax
         xphi  = 0. + PHISTEP*(iv-1)
         funcval = fchi2hit(degrad*xphi)
         if (funcval.lt.funcbuf) then
            funcbuf = funcval
            phibuf  = degrad*xphi
         endif
      enddo

* --- minimal bracketing
      phphin1 = phibuf
      phphin2 = phphin1 + 0.5*degrad
      call minbrack(phphin1,phphin2,phphin3,
     +              funcphi1,funcphi2,funcphi3,
     +              fchi2hit,ierr)
* ... find the minimal phi of the hit
      if (ierr.eq.0) then
         phphi1 = phphin1
         phphi3 = phphin2
         phphi2 = phphin3
         if (int(recbet(4)).eq.1) then
            chi2_hit = brent2(phphi1,phphi3,phphi2,
     +                        fchi2hit,PHITOL,phphimin)
         else
            chi2_hit = golden2(phphi1,phphi3,phphi2,
     +                         fchi2hit,PHITOL,phphimin)
         endif
         if (phphimin.gt.twopi) phphimin = phphimin - twopi
         if (phphimin.lt.0.)    phphimin = phphimin + twopi
         d2point   = chi2_hit
         phipoint  = phphimin
         irefpoint = nmirefhit
        endif

* ... end
      return
      end


***************************
*** CODE FROM probchg.F ***
***************************

      Subroutine probchg(chg0,sig0,chgm,prob0)

      double precision avg,SIG
      common/NORMGC/avg,SIG

      !$OMP THREADPRIVATE(/NORMGC/)

      real chg0,sig0,prob0,chgm

      double precision xlimlow,xlimup

      External normg
      double precision normg

cc      External DGAUSS
cc      Double precision DGAUSS

      sig = dble(sig0)
      avg = dble(chg0)
      chgmd = dble(chgm)

      if ( (chgmd - avg).lt.0.D0) then

          xlimlow = avg - 50.D0
          xlimup  = chgmd

      else

          xlimlow = chgmd
          xlimup  = avg + 50.D0

      endif

cc DGAUSS is a routine not thread safe
cc      prob0 = real(DGAUSS(normg,xlimlow,xlimup,
cc     +             1.D-6))

      prob0 = -999.


      return
      end


*************************************************************************************


c NORMALIZED GAUSSIAN

      double precision function normg(x)

      double precision avg,SIG
      common/NORMGC/avg,SIG

      !$OMP THREADPRIVATE(/NORMGC/)

      double precision x
      double precision xlimlow,xlimup,pi
      double precision K,dif
      PARAMETER (PI=3.14159265358979324D0)

      k=2.D0/(sig*dsqrt(2.D0*pi))
      dif=(x-avg)/sig
      normg=k*dexp(-0.5D0*(dif**2))
      return
      end


*****************************
*** CODE FROM richpkolm.F ***
*****************************

* ======================================================================
      subroutine richpkolm(dphistep,probkol)
* ======================================================================
*
*  Kolmogorov test to the uniformity/nonuniformity
*  of the photons phi distribution
*
*  INP:
*      -> dphistep: delta phi step in degrees for generating
*                   the reference distribution (azimuthaly uniform)
*  OUT:
*      -> probkol:  probability
*
* ======================================================================

#include "../include/richrec_lipf.h"

*      integer nassh
*      common /probbklc/ probability,nassh
*
      integer nphmax
      parameter(nphmax=NPHIRMAX)
      real phimin(nphmax),phimax(nphmax)
*
      integer n_exp_max
      parameter(n_exp_max=1000)
      real phiexp(n_exp_max)
      real phiexp_test(n_exp_max)
      integer iflag_phiexp(n_exp_max) !=1 enter the test
                                      !=0 doesn't enter the test
*
      integer nbhits_ring_max
*      parameter(nbhits_ring_max=1000)
      parameter(nbhits_ring_max=nhitmax) ! R. Pereira 13-Jan-2011
      integer indexphi_meas(nbhits_ring_max)
      integer iflag_phimeas(nbhits_ring_max)!=1 enter the test
                                            !=0 doesn't enter the test
      real phimeas(nbhits_ring_max)
      real phimeas_test(nbhits_ring_max)

* ======================================================================


* --- init ...

      probkol=0.
      visacc=0.

* --- cerenkov angle
      if (ipthetac.eq.0) return
      thetac=cangrec(ipthetac)

* --- visible paths: get phi limits
      call getphivis(thetac,npvis,phimin,phimax)

* --- store phi values for ring data hits
      nbhits_ring=0
      do i=1,min(nbhits,nhitmax)
C         if (iflghit(i).ne.1) then
         if (iflghit(i).gt.0) then
            if(chi2hit(ipthetac,i).lt.CHI2HTCUT)then
               nbhits_ring=nbhits_ring+1
               if (nbhits_ring.le.nbhits_ring_max)
     +          phimeas(nbhits_ring)=phihit(ipthetac,i)
            endif
         endif
      enddo

* --- Generate expected uniform distribution for phi
      npoints_exp=int(twopi/(dphistep*DEGRAD))+1
      if (npoints_exp.gt.n_exp_max) npoints_exp = n_exp_max
      nbins_exp=npoints_exp-1
*
      phiexp(1)=0.0
* ... fill phiexp(2:npoints_exp)
      do ibin=1,nbins_exp
         ipoint=ibin+1
         phiexp(ipoint)=ibin*dphistep*DEGRAD
      enddo
      call vzero(iflag_phimeas,nbhits_ring_max) ! init flag
      call vzero(iflag_phiexp,n_exp_max)        ! init flag

* --- flagging loop for the expected phi points inside visible branches greater than dphistep
      do i=1,npoints_exp
*        Does phiexp(i) belong to any visible branch greater than dphistep?
         do j=1,npvis
            dphi_vis   = phimax(j) - phimin(j)
c            print*,' dphi_vis',dphi_vis
            if(phiexp(i).gt.phimin(j).and.phiexp(i).lt.phimax(j)) then
               if(dphi_vis.ge.dphistep*DEGRAD)then
                  iflag_phiexp(i)=1 ! accept this point
               endif
            endif
         enddo
      enddo

* --- flagging loop for the measured phi points inside visible branches greater than dphistep
      do i=1,nbhits_ring
*        Does phimeas(i) belong to any visible branch greater than dphistep?
         do j=1,npvis
            dphi_vis   = phimax(j) - phimin(j)
            if(phimin(j).lt.phimeas(i).and.phimeas(i).lt.phimax(j))then
               if(dphi_vis.ge.dphistep*DEGRAD)then
                  iflag_phimeas(i)=1 ! accept this point
               endif
            endif
         enddo
      enddo

* --- Sort measured phi array in ascending order
      call sortzv_alt(phimeas,indexphi_meas,nbhits_ring,1,0,0) ! ascending order
*     copy relevant values for kolmogorov test in ascending order
      nbmeas_test=0
      do i=1,nbhits_ring
         if(iflag_phimeas(indexphi_meas(i)).eq.1)then
            nbmeas_test=nbmeas_test+1
            phimeas_test(nbmeas_test) = phimeas(indexphi_meas(i))
         endif
      enddo

*     copy relevant values from expected array for kolmogorov test
      nbexp_test=0
      do i=1,npoints_exp
         if(iflag_phiexp(i).eq.1)then
            nbexp_test=nbexp_test+1
            phiexp_test(nbexp_test) = phiexp(i)
         endif
      enddo

c      print*,' '
c      print*,' ------ TESTKOL subroutine -------'
c      print*,' ID EVT',ievnumb
c      print*,' visacc ',visacc,npvis
c      print*,' npoints_exp,nbhits_ring ',npoints_exp,nbhits_ring
c      print*,'nbexp_test,nbmeas_test ',nbexp_test,nbmeas_test
c      do i=1,nbexp_test,nbmeas_test)
c         print*,' phiexp phimeas',phiexp_test(i)*RADDEG,phimeas_test(i)
c     .        *RADDEG
c      enddo

* --- kolmogorov test (CernLib p.217)
      call TKOLMO(phiexp_test,nbexp_test,phimeas_test,nbmeas_test,PROB)
      probkol=prob

c      print*,' prob ',prob

* ... end
      return
      end



********************************************************************
*
* $Id: richutil_lip.F,v 1.7.14.1 2011/10/04 17:28:06 barao Exp $
*
* $Log: richutil_lip.F,v $
* Revision 1.7.14.1  2011/10/04 17:28:06  barao
* array hitstat_ev added to RICH LIP code, corresponding adjustments in LIP code
*
* Revision 1.7  2011/04/19 15:45:34  barao
* Update of LIP RICH rec to support bad PMT/pixel flagging
*
* Revision 1.6  2011/01/17 17:57:23  barao
* bug fix in RICH LIP reconstruction
*
* Revision 1.5  2011/01/14 18:58:44  barao
* bug fig in LIP RICH reconstruction
*
* Revision 1.4  2010/11/03 18:25:22  barao
* LIP reconstrunction algorithms for RICH detector ada pted to parallelised scheme
*
* Revision 1.1.1.1  1996/04/01 15:02:15  mclareni
* Mathlib gen
*
*

#if defined(CERNLIB_IBMRT)
#ifndef CERNLIB_IBMRS
#define CERNLIB_IBMRS
#endif
#endif
#if defined(CERNLIB_VAXVMS)||defined(CERNLIB_VAXULTRIX)
#ifndef CERNLIB_VAX
#define CERNLIB_VAX
#endif
#endif
#if defined(CERNLIB_VAX)
#ifndef CERNLIB_DOUBLE
#define CERNLIB_DOUBLE
#endif
#endif
#if (defined(CERNLIB_UNIX))&&(!defined(CERNLIB_SINGLE))
#ifndef CERNLIB_DOUBLE
#define CERNLIB_DOUBLE
#endif
#endif

#if defined(CERNLIB_LINUX)||defined(CERNLIB_MSDOS) && !defined(CERNLIB_WINNT)
#ifndef CERNLIB_NUMIB2
#define CERNLIB_NUMIB2
#endif
#ifdef CERNLIB_NUMD38
#undef CERNLIB_NUMD38
#endif
#ifndef CERNLIB_NUMD279
#define CERNLIB_NUMD279
#endif
#endif

#if defined(CERNLIB_VAXVMS)
#ifndef CERNLIB_NUMDE
#define CERNLIB_NUMDE
#endif
#ifndef CERNLIB_NUMD38
#define CERNLIB_NUMD38
#endif
#ifdef CERNLIB_NUMD279
#undef CERNLIB_NUMD279
#endif
#endif
#if (defined(CERNLIB_UNIX)) || defined(CERNLIB_WINNT) && (!defined(CERNLIB_QF2C))
#ifndef CERNLIB_NUMAP
#define CERNLIB_NUMAP
#endif
#ifndef CERNLIB_NUMD38
#define CERNLIB_NUMD38
#endif
#ifdef CERNLIB_NUMD279
#undef CERNLIB_NUMD279
#endif
#endif
#ifndef CERNLIB_NUMLOPRE
#define CERNLIB_NUMLOPRE
#endif
#ifdef CERNLIB_NUMHIPRE
#undef CERNLIB_NUMHIPRE
#endif
#ifndef CERNLIB_NUMRDBLE
#define CERNLIB_NUMRDBLE
#endif
#ifndef CERNLIB_NUMCDBLE
#define CERNLIB_NUMCDBLE
#endif
#ifndef CERNLIB_NUME38
#define CERNLIB_NUME38
#endif
#ifdef CERNLIB_NUME75
#undef CERNLIB_NUME75
#endif
#ifdef CERNLIB_NUME293
#undef CERNLIB_NUME293
#endif
#ifdef CERNLIB_NUME2465
#undef CERNLIB_NUME2465
#endif
#ifdef CERNLIB_NUMD75
#undef CERNLIB_NUMD75
#endif
#ifdef CERNLIB_NUMD2465
#undef CERNLIB_NUMD2465
#endif

C - history:
C   cernlib routines were copied from source to overcome problem
C   arising on parallelization (LIP)



      FUNCTION DGMLT1_ALT(FSUB1,A,B,NI,NG,X)

      IMPLICIT DOUBLE PRECISION (A-H,O-Z)

      CHARACTER NAME*(*)
      CHARACTER*80 ERRTXT
      PARAMETER (NAME = 'DGMLT1')
      PARAMETER (Z1 = 1, HALF = Z1/2)

      DOUBLE PRECISION X(6),W(14),T(14),V(64),U(64),F(64)

      DATA (T(I),W(I),I=1,14)
     1/-0.93246 95142 03152 028D0, 0.17132 44923 79170 345D0,
     2 -0.66120 93864 66264 514D0, 0.36076 15730 48138 608D0,
     3 -0.23861 91860 83196 909D0, 0.46791 39345 72691 047D0,
     4     0.23861 91860 83196 909D0, 0.46791 39345 72691 047D0,
     5     0.66120 93864 66264 514D0, 0.36076 15730 48138 608D0,
     6     0.93246 95142 03152 028D0, 0.17132 44923 79170 345D0,
     7 -0.96028 98564 97536 232D0, 0.10122 85362 90376 259D0,
     8 -0.79666 64774 13626 740D0, 0.22238 10344 53374 471D0,
     9 -0.52553 24099 16328 986D0, 0.31370 66458 77887 287D0,
     A -0.18343 46424 95649 805D0, 0.36268 37833 78361 983D0,
     B  0.18343 46424 95649 805D0, 0.36268 37833 78361 983D0,
     C  0.52553 24099 16328 986D0, 0.31370 66458 77887 287D0,
     D  0.79666 64774 13626 740D0, 0.22238 10344 53374 471D0,
     E  0.96028 98564 97536 232D0, 0.10122 85362 90376 259D0/
      SAVE T,W
      !$OMP THREADPRIVATE (T,W)

      IF(NI .LE. 0) THEN
       WRITE(ERRTXT,101) NI
       CALL MTLPRT(NAME,'D110.1',ERRTXT)
       STOP
      END IF
      M0=NG
      IF(M0 .NE. 8) M0=6
      I0=0
      IF(M0 .EQ. 8) I0=6
      D=(B-A)/NI
      R=HALF*D
      RA=R+A
      MV=MOD(M0*NI-1,64)+1
      S=0
      J=0
      DO 1 I = 1+I0,M0+I0
      RTA=R*T(I)+RA
      DO 2 K = 1,NI
      J=J+1
      V(J)=W(I)
      U(J)=RTA+(K-1)*D
      IF(J .EQ. MV) THEN
       CALL FSUB1(MV,U,F,X)
       DO 3 J = 1,MV
 3          S=S+V(J)*F(J)
       MV=64
       J=0
      END IF
 2     CONTINUE
 1      CONTINUE
      DGMLT1_ALT=R*S
      RETURN
 101   FORMAT('N1 = ',I4,' <= 0')
      END

******************************************************

      FUNCTION DGMLT2_ALT(FSUB2,A,B,NI,NG,X)

      IMPLICIT DOUBLE PRECISION (A-H,O-Z)

      CHARACTER NAME*(*)
      CHARACTER*80 ERRTXT
      PARAMETER (NAME = 'DGMLT2')

      PARAMETER (Z1 = 1, HALF = Z1/2)

       DIMENSION X(6),W(14),T(14),V(64),U(64),F(64)

      DATA (T(I),W(I),I=1,14)
     1/-0.93246 95142 03152 028D0, 0.17132 44923 79170 345D0,
     2 -0.66120 93864 66264 514D0, 0.36076 15730 48138 608D0,
     3 -0.23861 91860 83196 909D0, 0.46791 39345 72691 047D0,
     4      0.23861 91860 83196 909D0, 0.46791 39345 72691 047D0,
     5      0.66120 93864 66264 514D0, 0.36076 15730 48138 608D0,
     6      0.93246 95142 03152 028D0, 0.17132 44923 79170 345D0,
     7 -0.96028 98564 97536 232D0, 0.10122 85362 90376 259D0,
     8 -0.79666 64774 13626 740D0, 0.22238 10344 53374 471D0,
     9 -0.52553 24099 16328 986D0, 0.31370 66458 77887 287D0,
     A -0.18343 46424 95649 805D0, 0.36268 37833 78361 983D0,
     B  0.18343 46424 95649 805D0, 0.36268 37833 78361 983D0,
     C  0.52553 24099 16328 986D0, 0.31370 66458 77887 287D0,
     D  0.79666 64774 13626 740D0, 0.22238 10344 53374 471D0,
     E  0.96028 98564 97536 232D0, 0.10122 85362 90376 259D0/
       SAVE T,W
      !$OMP THREADPRIVATE (T,W)

      IF(NI .LE. 0) THEN
       WRITE(ERRTXT,101) NI
       CALL MTLPRT(NAME,'D110.1',ERRTXT)
      END IF
      M0=NG
      IF(M0 .NE. 8) M0=6
      I0=0
      IF(M0 .EQ. 8) I0=6
      D=(B-A)/NI
      R=HALF*D
      RA=R+A
      MV=MOD(M0*NI-1,64)+1
      S=0
      J=0
      DO 1 I = 1+I0,M0+I0
      RTA=R*T(I)+RA
      DO 2 K = 1,NI
      J=J+1
      V(J)=W(I)
      U(J)=RTA+(K-1)*D
      IF(J .EQ. MV) THEN
       CALL FSUB2(MV,U,F,X)
       DO 3 J = 1,MV
 3          S=S+V(J)*F(J)
       MV=64
       J=0
      END IF
 2     CONTINUE
 1      CONTINUE

      DGMLT2_ALT=R*S

      RETURN
 101   FORMAT('N2 = ',I4,' <= 0')
      END

*********************************************************************+

*
* $Id: richutil_lip.F,v 1.7.14.1 2011/10/04 17:28:06 barao Exp $
*
* $Log: richutil_lip.F,v $
* Revision 1.7.14.1  2011/10/04 17:28:06  barao
* array hitstat_ev added to RICH LIP code, corresponding adjustments in LIP code
*
* Revision 1.7  2011/04/19 15:45:34  barao
* Update of LIP RICH rec to support bad PMT/pixel flagging
*
* Revision 1.6  2011/01/17 17:57:23  barao
* bug fix in RICH LIP reconstruction
*
* Revision 1.5  2011/01/14 18:58:44  barao
* bug fig in LIP RICH reconstruction
*
* Revision 1.4  2010/11/03 18:25:22  barao
* LIP reconstrunction algorithms for RICH detector ada pted to parallelised scheme
*
* Revision 1.1.1.1  1996/02/15 17:49:07  mclareni
* Kernlib
*
*
#if defined(CERNLIB_IBMRT)
#ifndef CERNLIB_IBMRS
#define CERNLIB_IBMRS
#endif
#endif
#if defined(CERNLIB_VAXVMS)||defined(CERNLIB_VAXULTRIX)
#ifndef CERNLIB_VAX
#define CERNLIB_VAX
#endif
#endif
#if defined(CERNLIB_VAX)
#ifndef CERNLIB_DOUBLE
#define CERNLIB_DOUBLE
#endif
#endif
#if (defined(CERNLIB_UNIX))&&(!defined(CERNLIB_SINGLE))
#ifndef CERNLIB_DOUBLE
#define CERNLIB_DOUBLE
#endif
#endif

#if defined(CERNLIB_LINUX)||defined(CERNLIB_MSDOS) && !defined(CERNLIB_WINNT)
#ifndef CERNLIB_NUMIB2
#define CERNLIB_NUMIB2
#endif
#ifdef CERNLIB_NUMD38
#undef CERNLIB_NUMD38
#endif
#ifndef CERNLIB_NUMD279
#define CERNLIB_NUMD279
#endif
#endif

#if defined(CERNLIB_VAXVMS)
#ifndef CERNLIB_NUMDE
#define CERNLIB_NUMDE
#endif
#ifndef CERNLIB_NUMD38
#define CERNLIB_NUMD38
#endif
#ifdef CERNLIB_NUMD279
#undef CERNLIB_NUMD279
#endif
#endif
#if (defined(CERNLIB_UNIX)) || defined(CERNLIB_WINNT) && (!defined(CERNLIB_QF2C))
#ifndef CERNLIB_NUMAP
#define CERNLIB_NUMAP
#endif
#ifndef CERNLIB_NUMD38
#define CERNLIB_NUMD38
#endif
#ifdef CERNLIB_NUMD279
#undef CERNLIB_NUMD279
#endif
#endif
#ifndef CERNLIB_NUMLOPRE
#define CERNLIB_NUMLOPRE
#endif
#ifdef CERNLIB_NUMHIPRE
#undef CERNLIB_NUMHIPRE
#endif
#ifndef CERNLIB_NUMRDBLE
#define CERNLIB_NUMRDBLE
#endif
#ifndef CERNLIB_NUMCDBLE
#define CERNLIB_NUMCDBLE
#endif
#ifndef CERNLIB_NUME38
#define CERNLIB_NUME38
#endif
#ifdef CERNLIB_NUME75
#undef CERNLIB_NUME75
#endif
#ifdef CERNLIB_NUME293
#undef CERNLIB_NUME293
#endif
#ifdef CERNLIB_NUME2465
#undef CERNLIB_NUME2465
#endif
#ifdef CERNLIB_NUMD75
#undef CERNLIB_NUMD75
#endif
#ifdef CERNLIB_NUMD2465
#undef CERNLIB_NUMD2465
#endif

      SUBROUTINE RZERO_ALT(A,B,X0,R,EPS,MXF,F)
      IMPLICIT REAL (A-H,O-Z)



* $Id: richutil_lip.F,v 1.7.14.1 2011/10/04 17:28:06 barao Exp $
*
* $Log: richutil_lip.F,v $
* Revision 1.7.14.1  2011/10/04 17:28:06  barao
* array hitstat_ev added to RICH LIP code, corresponding adjustments in LIP code
*
* Revision 1.7  2011/04/19 15:45:34  barao
* Update of LIP RICH rec to support bad PMT/pixel flagging
*
* Revision 1.6  2011/01/17 17:57:23  barao
* bug fix in RICH LIP reconstruction
*
* Revision 1.5  2011/01/14 18:58:44  barao
* bug fig in LIP RICH reconstruction
*
* Revision 1.4  2010/11/03 18:25:22  barao
* LIP reconstrunction algorithms for RICH detector ada pted to parallelised scheme
*
* Revision 1.1.1.1  1996/02/15 17:49:07  mclareni
* Kernlib
*
*
#ifndef CERNLIB_KERNNUM_C205BODY_INC
#define CERNLIB_KERNNUM_C205BODY_INC
*
*
* c205body.inc
*
      LOGICAL MFLAG,RFLAG
      EXTERNAL F

      PARAMETER (ONE = 1, HALF = ONE/2)

      XA=MIN(A,B)
      XB=MAX(A,B)
      FA=F(XA,1)
      FB=F(XB,2)
      IF(FA*FB .GT. 0) GO TO 5
      MC=0

 1     X0=HALF*(XA+XB)
      R=X0-XA
      EE=EPS*(ABS(X0)+1)
      IF(R .LE. EE) GO TO 4
      F1=FA
      X1=XA
      F2=FB
      X2=XB

 2     FX=F(X0,2)
      MC=MC+1
      IF(MC .GT. MXF) GO TO 6
      IF(FX*FA .GT. 0) THEN
       XA=X0
       FA=FX
      ELSE
       XB=X0
       FB=FX
      END IF

 3     U1=F1-F2
      U2=X1-X2
      U3=F2-FX
      U4=X2-X0
      IF(U2 .EQ. 0 .OR. U4 .EQ. 0) GO TO 1
      F3=FX
      X3=X0
      U1=U1/U2
      U2=U3/U4
      CA=U1-U2
      CB=(X1+X2)*U2-(X2+X0)*U1
      CC=(X1-X0)*F1-X1*(CA*X1+CB)
      IF(CA .EQ. 0) THEN
       IF(CB .EQ. 0) GO TO 1
       X0=-CC/CB
      ELSE
       U3=CB/(2*CA)
       U4=U3*U3-CC/CA
       IF(U4 .LT. 0) GO TO 1
       X0=-U3+SIGN(SQRT(U4),X0+U3)
      END IF
      IF(X0 .LT. XA .OR. X0 .GT. XB) GO TO 1

      R=MIN(ABS(X0-X3),ABS(X0-X2))
      EE=EPS*(ABS(X0)+1)
      IF(R .GT. EE) THEN
       F1=F2
       X1=X2
       F2=F3
       X2=X3
       GO TO 2
      END IF

      FX=F(X0,2)
      IF(FX .EQ. 0) GO TO 4
      IF(FX*FA .LT. 0) THEN
       XX=X0-EE
       IF(XX .LE. XA) GO TO 4
       FF=F(XX,2)
       FB=FF
       XB=XX
      ELSE
       XX=X0+EE
       IF(XX .GE. XB) GO TO 4
       FF=F(XX,2)
       FA=FF
       XA=XX
      END IF
      IF(FX*FF .GT. 0) THEN
       MC=MC+2
       IF(MC .GT. MXF) GO TO 6
       F1=F3
       X1=X3
       F2=FX
       X2=X0
       X0=XX
       FX=FF
       GO TO 3
      END IF

 4     R=EE
      FF=F(X0,3)
      RETURN
 5     CALL KERMTR('C205.1',LGFILE,MFLAG,RFLAG)
      IF(MFLAG) THEN
       IF(LGFILE .EQ. 0) WRITE(*,100)
       IF(LGFILE .NE. 0) WRITE(LGFILE,100)
      END IF
      IF(.NOT.RFLAG) CALL ABEND
      R=-2*(XB-XA)
      X0=0
      RETURN
 6     CALL KERMTR('C205.2',LGFILE,MFLAG,RFLAG)
      IF(MFLAG) THEN
       IF(LGFILE .EQ. 0) WRITE(*,101)
       IF(LGFILE .NE. 0) WRITE(LGFILE,101)
      END IF
      IF(.NOT.RFLAG) CALL ABEND
      R=-HALF*ABS(XB-XA)
      X0=0
      RETURN

#endif

 100  FORMAT(1X,'***** CERN C205 RZERO ... F(A) AND F(B)',
     1     ' HAVE THE SAME SIGN')
 101  FORMAT(1X,'***** CERN C205 RZERO ... TOO MANY FUNCTION CALLS')
      END

***********************************************************************
      real function betaresLIP(zch)
#include "../include/richrec_lipf.h"
      real zch

      betaresLIP = 1.E-2*sqrt( (a_beta/zch)**2 + b_beta**2 )

      return
      end

***********************************************************************
*
* $Id: richutil_lip.F,v 1.7.14.1 2011/10/04 17:28:06 barao Exp $
*
* $Log: richutil_lip.F,v $
* Revision 1.7.14.1  2011/10/04 17:28:06  barao
* array hitstat_ev added to RICH LIP code, corresponding adjustments in LIP code
*
* Revision 1.7  2011/04/19 15:45:34  barao
* Update of LIP RICH rec to support bad PMT/pixel flagging
*
* Revision 1.6  2011/01/17 17:57:23  barao
* bug fix in RICH LIP reconstruction
*
* Revision 1.5  2011/01/14 18:58:44  barao
* bug fig in LIP RICH reconstruction
*
* Revision 1.4  2010/11/03 18:25:22  barao
* LIP reconstrunction algorithms for RICH detector ada pted to parallelised scheme
*
* Revision 1.1.1.1  1996/02/15 17:48:36  mclareni
* Kernlib
*
*

#if defined(CERNLIB_IBMRT)
#ifndef CERNLIB_IBMRS
#define CERNLIB_IBMRS
#endif
#endif
#if defined(CERNLIB_VAXVMS)||defined(CERNLIB_VAXULTRIX)
#ifndef CERNLIB_VAX
#define CERNLIB_VAX
#endif
#endif
#if defined(CERNLIB_VAX)
#ifndef CERNLIB_DOUBLE
#define CERNLIB_DOUBLE
#endif
#endif
#if (defined(CERNLIB_UNIX))&&(!defined(CERNLIB_SINGLE))
#ifndef CERNLIB_DOUBLE
#define CERNLIB_DOUBLE
#endif
#endif

#if defined(CERNLIB_LINUX)||defined(CERNLIB_MSDOS) && !defined(CERNLIB_WINNT)
#ifndef CERNLIB_NUMIB2
#define CERNLIB_NUMIB2
#endif
#ifdef CERNLIB_NUMD38
#undef CERNLIB_NUMD38
#endif
#ifndef CERNLIB_NUMD279
#define CERNLIB_NUMD279
#endif
#endif

#if defined(CERNLIB_VAXVMS)
#ifndef CERNLIB_NUMDE
#define CERNLIB_NUMDE
#endif
#ifndef CERNLIB_NUMD38
#define CERNLIB_NUMD38
#endif
#ifdef CERNLIB_NUMD279
#undef CERNLIB_NUMD279
#endif
#endif
#if (defined(CERNLIB_UNIX)) || defined(CERNLIB_WINNT) && (!defined(CERNLIB_QF2C))
#ifndef CERNLIB_NUMAP
#define CERNLIB_NUMAP
#endif
#ifndef CERNLIB_NUMD38
#define CERNLIB_NUMD38
#endif
#ifdef CERNLIB_NUMD279
#undef CERNLIB_NUMD279
#endif
#endif
#ifndef CERNLIB_NUMLOPRE
#define CERNLIB_NUMLOPRE
#endif
#ifdef CERNLIB_NUMHIPRE
#undef CERNLIB_NUMHIPRE
#endif
#ifndef CERNLIB_NUMRDBLE
#define CERNLIB_NUMRDBLE
#endif
#ifndef CERNLIB_NUMCDBLE
#define CERNLIB_NUMCDBLE
#endif
#ifndef CERNLIB_NUME38
#define CERNLIB_NUME38
#endif
#ifdef CERNLIB_NUME75
#undef CERNLIB_NUME75
#endif
#ifdef CERNLIB_NUME293
#undef CERNLIB_NUME293
#endif
#ifdef CERNLIB_NUME2465
#undef CERNLIB_NUME2465
#endif
#ifdef CERNLIB_NUMD75
#undef CERNLIB_NUMD75
#endif
#ifdef CERNLIB_NUMD2465
#undef CERNLIB_NUMD2465
#endif


      FUNCTION DIVDIF_ALT(F,A,NN,X,MM)
      DIMENSION A(NN),F(NN),T(20),D(20)
      LOGICAL EXTRA
      LOGICAL MFLAG,RFLAG
      SAVE MMAX
      DATA MMAX/10/
      !$OMP THREADPRIVATE(MMAX)
C
C  TABULAR INTERPOLATION USING SYMMETRICALLY PLACED ARGUMENT POINTS.
C
C  START.  FIND SUBSCRIPT IX OF X IN ARRAY A.
      IF( (NN.LT.2) .OR. (MM.LT.1) ) GO TO 20
      N=NN
      M=MIN0(MM,MMAX,N-1)
      MPLUS=M+1
      IX=0
      IY=N+1
      IF(A(1).GT.A(N)) GO TO 4
C     (SEARCH INCREASING ARGUMENTS.)
 1        MID=(IX+IY)/2
         IF(X.GE.A(MID)) GO TO 2
            IY=MID
            GO TO 3
C        (IF TRUE.)
 2                 IX=MID
 3                     IF(IY-IX.GT.1) GO TO 1
         GO TO 7
C     (SEARCH DECREASING ARGUMENTS.)
 4           MID=(IX+IY)/2
         IF(X.LE.A(MID)) GO TO 5
            IY=MID
            GO TO 6
C        (IF TRUE.)
 5                 IX=MID
 6                     IF(IY-IX.GT.1) GO TO 4
C
C  COPY REORDERED INTERPOLATION POINTS INTO (T(I),D(I)), SETTING
C  *EXTRA* TO TRUE IF M+2 POINTS TO BE USED.
 7                      NPTS=M+2-MOD(M,2)
      IP=0
      L=0
      GO TO 9
 8        L=-L
         IF(L.GE.0) L=L+1
 9           ISUB=IX+L
         IF((1.LE.ISUB).AND.(ISUB.LE.N)) GO TO 10
C        (SKIP POINT.)
            NPTS=MPLUS
            GO TO 11
C        (INSERT POINT.)
 10                IP=IP+1
            T(IP)=A(ISUB)
            D(IP)=F(ISUB)
 11             IF(IP.LT.NPTS) GO TO 8
      EXTRA=NPTS.NE.MPLUS
C
C  REPLACE D BY THE LEADING DIAGONAL OF A DIVIDED-DIFFERENCE TABLE, SUP-
C  PLEMENTED BY AN EXTRA LINE IF *EXTRA* IS TRUE.
      DO 14 L=1,M
         IF(.NOT.EXTRA) GO TO 12
            ISUB=MPLUS-L
            D(M+2)=(D(M+2)-D(M))/(T(M+2)-T(ISUB))
 12             I=MPLUS
         DO 13 J=L,M
            ISUB=I-L
            D(I)=(D(I)-D(I-1))/(T(I)-T(ISUB))
            I=I-1
 13             CONTINUE
 14              CONTINUE
C
C  EVALUATE THE NEWTON INTERPOLATION FORMULA AT X, AVERAGING TWO VALUES
C  OF LAST DIFFERENCE IF *EXTRA* IS TRUE.
      SUM=D(MPLUS)
      IF(EXTRA) SUM=0.5*(SUM+D(M+2))
      J=M
      DO 15 L=1,M
         SUM=D(J)+(X-T(J))*SUM
         J=J-1
 15       CONTINUE
      DIVDIF_ALT=SUM
      RETURN
C
 20    CALL KERMTR('E105.1',LGFILE,MFLAG,RFLAG)
      DIVDIF_ALT=0
      IF(MFLAG) THEN
         IF(LGFILE.EQ.0) THEN
            IF(MM.LT.1) WRITE(*,101) MM
            IF(NN.LT.2) WRITE(*,102) NN
         ELSE
            IF(MM.LT.1) WRITE(LGFILE,101) MM
            IF(NN.LT.2) WRITE(LGFILE,102) NN
         ENDIF
      ENDIF
      IF(.NOT.RFLAG) CALL ABEND
      RETURN
 101  FORMAT( 7X, 'FUNCTION DIVDIF_ALT ... M =',I6,' IS LESS THAN 1')
 102  FORMAT( 7X, 'FUNCTION DIVDIF_ALT ... N =',I6,' IS LESS THAN 2')
      END


***********************************************************************

* $Id: richutil_lip.F,v 1.7.14.1 2011/10/04 17:28:06 barao Exp $
*
* $Log: richutil_lip.F,v $
* Revision 1.7.14.1  2011/10/04 17:28:06  barao
* array hitstat_ev added to RICH LIP code, corresponding adjustments in LIP code
*
* Revision 1.7  2011/04/19 15:45:34  barao
* Update of LIP RICH rec to support bad PMT/pixel flagging
*
* Revision 1.6  2011/01/17 17:57:23  barao
* bug fix in RICH LIP reconstruction
*
* Revision 1.1.1.1  1996/02/15 17:49:50  mclareni
* Kernlib
*
*
#include "kerngen/pilot.h"
      SUBROUTINE SORTZV_ALT (A,INDEX,N1,MODE,NWAY,NSORT)
C
C CERN PROGLIB# M101    SORTZV          .VERSION KERNFOR  3.15  820113
C ORIG. 02/10/75
C
      DIMENSION A(N1),INDEX(N1)
C
C
      N = N1
      IF (N.LE.0)            RETURN
      IF (NSORT.NE.0) GO TO 2
      DO 1 I=1,N
    1 INDEX(I)=I
C
    2 IF (N.EQ.1)            RETURN
      IF (MODE)    10,20,30
   10 CALL SORTTI (A,INDEX,N)
      GO TO 40
C
   20 CALL SORTTC(A,INDEX,N)
      GO TO 40
C
   30 CALL SORTTF_ALT (A,INDEX,N)
C
   40 IF (NWAY.EQ.0) GO TO 50
      N2 = N/2
      DO 41 I=1,N2
      ISWAP = INDEX(I)
      K = N+1-I
      INDEX(I) = INDEX(K)
   41 INDEX(K) = ISWAP
   50 RETURN
      END
*     ========================================
      SUBROUTINE SORTTF_ALT (A,INDEX,N1)
C
      DIMENSION A(N1),INDEX(N1)
C
      N = N1
      DO 3 I1=2,N
      I3 = I1
      I33 = INDEX(I3)
      AI = A(I33)
    1 I2 = I3/2
      IF (I2) 3,3,2
    2 I22 = INDEX(I2)
      IF (AI.LE.A (I22)) GO TO 3
      INDEX (I3) = I22
      I3 = I2
      GO TO 1
    3 INDEX (I3) = I33
    4 I3 = INDEX (N)
      INDEX (N) = INDEX (1)
      AI = A(I3)
      N = N-1
      IF (N-1) 12,12,5
    5 I1 = 1
    6 I2 = I1 + I1
      IF (I2.LE.N) I22= INDEX(I2)
      IF (I2-N) 7,9,11
    7 I222 = INDEX (I2+1)
      IF (A(I22)-A(I222)) 8,9,9
    8 I2 = I2+1
      I22 = I222
    9 IF (AI-A(I22)) 10,11,11
   10 INDEX(I1) = I22
      I1 = I2
      GO TO 6
   11 INDEX (I1) = I3
      GO TO 4
   12 INDEX (1) = I3
      RETURN
      END
