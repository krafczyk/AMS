      SUBROUTINE USRINIT
**************************************************************************
* Do user initialization such as histgram booking
**************************************************************************
      INCLUDE 'cutstat.inc'
*
*-- Reset the counters for cuts
      DO I=0,NCUT
        NPASS(I) = 0
      ENDDO
*
*-- Book histgrams
C      CALL HBOOK1(11,  'NPART', 12, -1, 5, 0.)
C      CALL HBOOK1(12,  'NTRTR', 12, -1, 5, 0.)
C      CALL HBOOK1(13,  'NBETA', 12, -1, 5, 0.)
C      CALL HBOOK1(103, '[b](v/c)', 120, -1.2, 1.2, 0.)
* 
       call HROPEN(44,'output','ahe.hbk','NP',1024,iostat)      
      x=ahe(0,1)
      RETURN
      END

C**------------- BLOCK DATA VARDATA ---------------------------------C
      BLOCK DATA VARDATA
*------------------------------------------------------------
* put the variable names below which are used in the function
*
* In case of NVAR=0, all variables will be read
*-----------------------------------------------------------
      INCLUDE 'usrcom.inc'
*-user may change following
      DATA NVAR/0/, VARLIS(1)/'EVENTSTATUS'/
C      DATA NVAR/5/,VARLIS/
C     &  'NBETA','BETA', 'NPART', 'NTRTR', 'NHITS', 217*' '/
*-end of user change
       END

      INTEGER FUNCTION USRCUT(IOPT)
*      REAL FUNCTION USRCUT()
**************************************************************************
* USER's CUT here
*
* Input:
*        IOPT  < 0, do initialization
*              = 0, event processing
*              > 0, do termination such as printing
*
* Return:
*        1, survives     
*        0, fails
*
* Usage:
*        1) In "BLOCK DATA VARDATA", change/add the list of variables in
*           VARS which are used in the function, NVAR is the total number
*           of variables used. Set NVAR=0 to enable reading all variables
*        2) set the value of this function, 1=survive, 0=fail
*
**************************************************************************
      
      INCLUDE 'cwncom.inc'
      INCLUDE 'usrcom.inc'
      INCLUDE 'cutstat.inc'
       integer ahe
      INTEGER IOPT
*
      USRCUT = 0
*
      IF (IOPT .LT. 0) THEN
         CALL USRINIT
         GOTO 990
      ELSE IF (IOPT .GT. 0) THEN
         CALL USREND
         GOTO 990
      ELSE If (IOPT .NE. 0) THEN
         GOTO 990
      ENDIF
*
*-impose cuts here
*
      NPASS(0) = NPASS(0) + 1
      IPASS    = 0


*
*-- Comment the following in case you do not want to write selectd events
*--  into new ntuples
      icut=ahe(1,-1)
      USRCUT =icut
      if(usrcut.ne.0)then
        notzero=notzero+1
      else
         izero=izero+1
      endif
       if(mod(izero,100000).eq.1)write(*,*)izero,notzero
 990  CONTINUE

      RETURN
      END


      SUBROUTINE USREND
**************************************************************************
* Do user termination such as printing.
**************************************************************************
      INCLUDE 'cutstat.inc'
*
      PRINT *,'Total events processed =',NPASS(0)
      DO I=1,NCUT
        WRITE(6,101) I, NPASS(I)
      ENDDO
      
      call  HCDIR ('//output',' ')
      ICYCL=0
      CALL HROUT (0, ICYCL, ' ')
      CALL HREND ('output')
      close(44)
C      CALL HPRINT(0)
       

      RETURN
 101  FORMAT(2X,'Events survived after cut-',I2,' =',I8)
      END



      integer FUNCTION aprf(init,isig)
      INCLUDE 'cwncom.inc'
         integer patmiss(6,22)
         data patmiss/                0,0,0,0,0,0,   ! 123456  0
     +                                0,0,0,0,0,5,   ! 12346   1
     +                                0,0,0,0,0,4,   ! 12356   2
     +                                0,0,0,0,0,3,   ! 12456   3
     +                                0,0,0,0,0,2,   ! 13456   4
     +                                0,0,0,0,0,6,   ! 12345   5
     +                                0,0,0,0,0,1,   ! 23456   6
     +                                0,0,0,0,5,6,   ! 1234    7
     +                                0,0,0,0,4,6,   ! 1235    8
     +                                0,0,0,0,4,5,   ! 1236    9
     +                                0,0,0,0,3,6,   ! 1245   10
     +                                0,0,0,0,3,5,   ! 1246   11
     +                                0,0,0,0,3,4,   ! 1256   12
     +                                0,0,0,0,2,6,   ! 1345   13
     +                                0,0,0,0,2,5,   ! 1346   14
     +                                0,0,0,0,2,4,   ! 1356   15
     +                                0,0,0,0,2,3,   ! 1456   16
     +                                0,0,0,0,1,6,   ! 2345   17
     +                                0,0,0,0,1,5,   ! 2346   18
     +                                0,0,0,0,1,4,   ! 2356   19
     +                                0,0,0,0,1,3,   ! 2456   20
     +                                0,0,0,0,1,2/   ! 3456   21

      integer pm
      logical cut
      aprf=0
      
      if(init.eq.0)then
         call hbook1(11,'Trclusters',200,-0.5,199.5,0.)
         call hbook1(12,'TOFclusters',50,-0.5,49.5,0.)
         call hbook1(13,'mass',200,-5.,15.,0.)
         call hbook1(15,'gr/r',200,-1.,4.,0.)
         call hbook1(21,'ridgidity',200,-10.,10.,0.)
         call hbook1(1077,'beta',200,-5.,5.,0.)
         call hbook1(1078,'beta',200,0.,20.,0.)
         call hbook1(621,'ridgidity',200,-10.,10.,0.)
         call hbook1(721,'ridgidity',200,-10.,10.,0.)
         call hbook1(22,'charge',10,0.5,10.5,0.)
         call hbook1(23,'beta',200,-1.1,1.1,0.)
         call hbook1(24,'betapattern',23,-0.5,22.5,0.)
         call hbook1(25,'pattern',23,-0.5,22.5,0.)
         call hbook1(26,'cos(thetagl)',200,-1.,1.,0.)
         call hbook1(31,'r/r1',200,-1.,4.,0.)
         call hbook1(32,'r/r2',200,-1.,4.,0.)
         call hbook1(41,'probtr',200,0.,1.,0.)
         call hbook1(51,'chi2f',200,0.,50.,0.)
         call hbook1(52,'chi2c',200,0.,10.,0.)
         call hbook1(53,'chi2d',200,0.,51000.,0.)
         call hbook1(54,'chi2fms',200,0.,400.,0.)
         call hbook1(61,'phi',200,-0.002,0.002,0.)
         call hbook1(62,'the',200,-0.002,0.002,0.)
         call hbook1(63,'ridgidity rp',400,0,100.,0.)
         call hbook1(64,'ridgidity rt',400,0,100.,0.)
         call hbook1(65,'ridgidity 1',400,0,100.,0.)
         call hbook2(66,'ctc',100,-70.,70.,100,-50.,50.,0.)
         call hbook2(67,'ctc',100,-70.,70.,100,-50.,50.,0.)
         call hbook1(71,'pmass',200,-1.,1.,0.)
         call hbook1(72,'perrmass/pmass',200,0.,1.,0.)
         call hbook1(73,'prob',200,0.,1.,0.)
         call hbook1(82,'pid',200,-0.5,199.5,0.)
         call hbook1(91,'ridgidity',200,-40.,40.,0.)
         call hbook1(92,'pmass',200,-1.,1.,0.)
         call hbook1(93,'pmass',200,-1.,1.,0.)
         call hbook1(94,'locgl',10,-0.5,9.5,0.)
         call hbook1(95,'summis',200,-0.5,199.5,0.)
         call hbook1(96,'disty',200,-1.,1.,0.)
         call hbook1(97,'distx',200,-5.,5.,0.)
         call hbook1(1000+2,'Nparticles',20,-0.5,19.5,0.)
         call hbook1(1000+11,'Trclusters',200,-0.5,199.5,0.)
         call hbook1(1000+12,'TOFclusters',50,-0.5,49.5,0.)
         call hbook1(1000+13,'mass',200,-5.,15.,0.)
         call hbook1(1000+15,'gr/r',200,-1.,4.,0.)
         call hbook1(1000+21,'ridgidity',200,-10.,10.,0.)
         call hbook1(2021,'ridgidity',200,-10.,10.,0.)
         call hbook1(1000+22,'charge',10,0.5,10.5,0.)
         call hbook1(1000+23,'beta',200,-1.1,1.1,0.)
         call hbook1(1000+24,'betapattern',23,-0.5,22.5,0.)
         call hbook1(1000+25,'pattern',23,-0.5,22.5,0.)
         call hbook1(1000+26,'cos(thetagl)',200,-1.,1.,0.)
         call hbook1(1000+31,'r/r1',200,-1.,4.,0.)
         call hbook1(1000+32,'r/r2',200,-1.,4.,0.)
         call hbook1(1000+41,'probtr',200,0.,1.,0.)
         call hbook1(1000+51,'chi2f',200,0.,50.,0.)
         call hbook1(1000+52,'chi2c',200,0.,10.,0.)
         call hbook1(1000+53,'chi2d',200,0.,51000.,0.)
         call hbook1(1000+54,'chi2fms',200,0.,400.,0.)
         call hbook1(1000+61,'phi',200,-0.002,0.002,0.)
         call hbook1(1000+62,'the',200,-0.002,0.002,0.)
         call hbook1(1000+71,'pmass',200,-1.,1.,0.)
         call hbook1(1000+72,'perrmass/pmass',200,0.,1.,0.)
         call hbook1(1000+73,'prob',200,0.,1.,0.)
         call hbook1(1000+82,'pid',200,-0.5,199.5,0.)
         call hbook1(1000+91,'ridgidity',200,-20.,20.,0.)
         call hbook1(1000+92,'pmass',200,-2.,12.,0.)
         call hbook1(1000+98,'pmass2',200,-2.,12.,0.)
         call hbook1(1000+93,'pattern',25,-0.5,24.5,0.)
         call hbook1(1000+94,'locgl',2,-0.5,1.5,0.)
         call hbook1(1000+95,'summis',200,-0.5,199.5,0.)
         call hbook1(1000+96,'disty',200,-1.,1.,0.)
         return
      endif
*
*    Basic cuts
*
c      call hf1(2,float(npart),1.)
      if(mod(eventstatus/1048576/2,4).gt.0.and.
     +  mod(eventstatus/32,8).eq.0)then
        r=gridgidity(ptrackp(1))/ridgidity(ptrackp(1))
        if(beta(pbetap(1)).lt.0)r=-r
        call hf1(15,r,1.)        
        call hf1(21,pmom(1)/pcharge(1),1.)        
        call hf1(22,pcharge(1),1.)
        call hf1(23,beta(pbetap(1)),1.)
        call hf1(24,float(betapattern(pbetap(1))),1.)
        call hf1(25,float(pattern(ptrackp(1))),1.)
        call hf1(26,cos(thetagl(1)),1.)
        if(geanefitdone(ptrackp(1)).eq.0.or.
c     +    (r.gt.0.4.and.r.lt.2.5))then
     +    (r.gt.0.4.and.r.lt.2.5e4))then
          if(xsign(pmom(1)).eq.isig.and.
     +       betapattern(pbetap(1)).lt.5.and.
     +       advancedfitdone(ptrackp(1)).ne.0)then
             r1=ridgidity(ptrackp(1))/hridgidity(1,ptrackp(1))
             r2=ridgidity(ptrackp(1))/hridgidity(2,ptrackp(1))
             call hf1(31,r1,1.)
             call hf1(32,r2,1.)
c             if(r1.gt.0.4.and.r1.lt.1.6.and.r2.gt.0.4.and.r2.lt.1.6)then
             if(r1.gt.0..and.r1.lt.1000.and.r2.gt.0..and.r2.lt.1000)then
               call hf1(41,proballtracker(pchargep(1)),1.)
               if(proballtracker(pchargep(1)).gt.0.05)then
                call hf1(51,chi2fastfit(ptrackp(1)),1.)
                call hf1(52,chi2circle(ptrackp(1)),1.)
                iptr=ptrackp(1)
                d=chi2fastfit(iptr)-hchi2(1,iptr)-hchi2(2,iptr)
                call hf1(53,d,1.)
                call hf1(54,fchi2ms(ptrackp(1)),1.)
                if(d.lt.50)then
                   rp= asin(sin(hphi(2,iptr)-hphi(1,iptr)))
                   rt=htheta(2,iptr)-htheta(1,iptr)
                   call hf1(61,rp,1.)
                   call hf1(62 ,rt,1.)
                   if(rp.lt.0.002)
     +             call hf1(63,abs(pmom(1))/pcharge(1),rp*rp)
                   call hf1(64,abs(pmom(1))/pcharge(1),rt*rt)
                   call hf1(65,abs(pmom(1))/pcharge(1),1.)
                   if(abs(rt).lt.frt(abs(pmom(1))/pcharge(1)).and.
     +                abs(rp).lt.frp(abs(pmom(1))/pcharge(1)))then
                    call hf1(71,pmass(1),1.)
                    call hf1(72,perrmass(1)/abs(pmass(1)),1.)
                    call hf1(73,probpid(1,1),1.)
                    if(probpid(1,1).gt.-0.02)then
                     call hf1(82,float(pid(1)),1.)
                     xpid=pid(1)
                    xll=-58.
                    xlr=58.
                    yll=-43.
                    ylr=43.
                    cut= 
     +              cooctc(1,1,1).gt.xll.and.
     +              cooctc(1,1,1).lt.xlr.and.
     +              cooctc(2,1,1).gt.yll.and.
     +              cooctc(2,1,1).lt.ylr
                    call hf2(66,cooctc(1,1,1),cooctc(2,1,1),1.)
                    xcut=5
                    if(isig.eq.-1)xcut=100
                    if(abs(pmom(1)).lt.xcut)then
                       call hf1(1021,pmom(1),1.)
                    zg=(abs(1/beta(pbetap(1)))-1.)
     +              /betaerror(pbetap(1))
                       call hf1(1077,zg,1.)
                      if(abs(1/beta(pbetap(1)))
     +                 .lt.1+1.*betaerror(pbetap(1)))then
                       call hf1(621,pmom(1),1.)
                       call hf1(1078,atcnbphe(1,1)+atcnbphe(2,1),1.)
                     if(atcnbphe(1,1)+atcnbphe(2,1).gt.1.5)then
                       call hf1(2021,pmom(1),1.)
                     xmc=0.65
                     if(isig.eq.-1)xmc=200.
                     if(pmass(1).lt.xmc)then
                      summis=0
                      
                      goto 777
                      disty=-1.e6
                      do i=1,6
                       ipm=patmiss(i,pattern(ptrackp(1))+1)
                       if(ipm.gt.1.and.ipm.lt.6)then
                        do j=1,ntrrh
                         if(layer(j).eq.ipm)then
                          dist=0
                          disty=cootr(2,ipm,1)-hitr(2,j)
                          distx=cootr(2,ipm,1)-hitr(2,j)
                          do k=1,3
                           dist=dist+(cootr(k,ipm,1)-hitr(k,j))**2
                          enddo
                          dist=sqrt(dist)
                          if(abs(distx).lt.3)call hf1(96,disty,1.)
                          call hf1(97,distx,1.)
                          if(dist.lt.1.or.
     +                     (abs(disty).lt.0.2.and.abs(distx).lt.3.))then
                           summis=summis+sumr(j)
                          endif 
                         endif
                        enddo
                       endif
                      enddo
                      call hf1(95,summis,1.)
 777                  continue
                       call hf1(92,pmass(1),1.)
                     if(1.eq.1)then
                       call hf1(93,pmass(1),1.)
                       if(isig.gt.0)then
                        aprf=1
                        write(86,*)run,eventno,pmass(1),pmom(1),
     +                  address(ptrackp(1))
                       else if(rndm(d).lt.1.1)then
                        aprf=1
                        write(87,*)run,eventno,pmass(1),pmom(1),
     +                  address(ptrackp(1))
                       endif
                       istat=trstatus(ptrackp(1))
                       ktofonly=mod(istat/16384,2)
                       kfalsex=mod(istat/8192,2)
                       if(mod(istat/32768/2,2).eq.1)then
                            call hf1(94,0.,1.)
                       else if(mod(istat/32768/2/2,2).eq.0)then
                          call hf1(94,0.,2.)
                       endif
                       if(mod(istat/32768/2/2,2).eq.1)then
                            call hf1(94,1.,1.)
                       endif
                       kpat=pattern(ptrackp(1))
                       call hf1(94,3.+ktofonly,1.)                 
                       call hf1(91,pmom(1)/pcharge(1),1.)
                       if(aprf.eq.1)then             
                       call hf1(1015,r,1.)
                       call hf1(1031,r1,1.)
                       call hf1(1032,r2,1.)
                       call hf1(1041,proballtracker(pchargep(1)),1.)
                       call hf1(1051,chi2fastfit(ptrackp(1)),1.)
                       call hf1(1052,chi2circle(ptrackp(1)),1.)
                       call hf1(1053,d,1.)
                       call hf1(1054,fchi2ms(ptrackp(1)),1.)
                       call hf1(1061,rp,1.)
                       call hf1(1062 ,rt,1.)
                       call hf1(1073,probpid(1,1),1.)
                       call hf1(1082,float(pid(1)),1.)
                       call hf1(1091,pmom(1)/pcharge(1),1.)
                       call hf1(1092,pmass(1),1.)
                       xpm=pmom(1)**2/beta(pbetap(1))**2*
     +                 (1-beta(pbetap(1))**2)
                       if(xpm.gt.0)then
                        xpm=sqrt(xpm)
                       else
                        xpm=-sqrt(-xpm)
                       endif
                       call hf1(1098,xpm,1.)
                       call hf1(1093,float(pattern(ptrackp(1))),1.)
                       call hf1(1095,summis,1.)
                       call hf1(1096,disty,1.)
                       call hf1(1022,pcharge(1),1.)
                       call hf1(1023,beta(pbetap(1)),1.)
                       call hf1(1024,float(betapattern(pbetap(1))),1.)
                       call hf1(1025,float(pattern(ptrackp(1))),1.)
                       call hf1(1026,cos(thetagl(1)),1.)
                      endif
                      endif                      
                     endif                      
                    endif
                    endif
                    endif
                    endif
                   endif
                endif
               endif
             endif
          endif          
        endif
      endif     
*
      END



      function xsign(x)
       xsign=1.
       if(x.lt.0)xsign=-1.
      end      
      function frp(x)
       frp=9*sqrt(0.42e-6*exp(-x*0.037)+0.46e-7/x/x)
      end
      function frt(x)
       frt=9*sqrt(0.2e-7*exp(-x*0.037)+0.46e-7/x/x)
      end

      integer FUNCTION ahe(init,isig)
      INCLUDE 'cwncom.inc'
         integer patmiss(6,22)
         data patmiss/                0,0,0,0,0,0,   ! 123456  0
     +                                0,0,0,0,0,5,   ! 12346   1
     +                                0,0,0,0,0,4,   ! 12356   2
     +                                0,0,0,0,0,3,   ! 12456   3
     +                                0,0,0,0,0,2,   ! 13456   4
     +                                0,0,0,0,0,6,   ! 12345   5
     +                                0,0,0,0,0,1,   ! 23456   6
     +                                0,0,0,0,5,6,   ! 1234    7
     +                                0,0,0,0,4,6,   ! 1235    8
     +                                0,0,0,0,4,5,   ! 1236    9
     +                                0,0,0,0,3,6,   ! 1245   10
     +                                0,0,0,0,3,5,   ! 1246   11
     +                                0,0,0,0,3,4,   ! 1256   12
     +                                0,0,0,0,2,6,   ! 1345   13
     +                                0,0,0,0,2,5,   ! 1346   14
     +                                0,0,0,0,2,4,   ! 1356   15
     +                                0,0,0,0,2,3,   ! 1456   16
     +                                0,0,0,0,1,6,   ! 2345   17
     +                                0,0,0,0,1,5,   ! 2346   18
     +                                0,0,0,0,1,4,   ! 2356   19
     +                                0,0,0,0,1,3,   ! 2456   20
     +                                0,0,0,0,1,2/   ! 3456   21

      integer pm
      ahe=0
      
      if(init.eq.0)then
         call hbook1(11,'Trclusters',200,-0.5,199.5,0.)
         call hbook1(12,'TOFclusters',50,-0.5,49.5,0.)
         call hbook1(13,'mass',200,-5.,15.,0.)
         call hbook1(15,'gr/r',200,-1.,4.,0.)
         call hbook1(21,'ridgidity',200,-40.,40.,0.)
         call hbook1(22,'charge',10,0.5,10.5,0.)
         call hbook1(23,'beta',200,-1.1,1.1,0.)
         call hbook1(24,'betapattern',23,-0.5,22.5,0.)
         call hbook1(25,'pattern',23,-0.5,22.5,0.)
         call hbook1(26,'cos(thetagl)',200,-1.,1.,0.)
         call hbook1(31,'r/r1',200,-1.,4.,0.)
         call hbook1(32,'r/r2',200,-1.,4.,0.)
         call hbook1(41,'probtr',200,0.,1.,0.)
         call hbook1(51,'chi2f',200,0.,50.,0.)
         call hbook1(52,'chi2c',200,0.,10.,0.)
         call hbook1(53,'chi2d',200,0.,50.,0.)
         call hbook1(54,'chi2fms',200,0.,400.,0.)
         call hbook1(61,'phi',200,-0.002,0.002,0.)
         call hbook1(62,'the',200,-0.002,0.002,0.)
         call hbook1(63,'ridgidity rp',400,0,100.,0.)
         call hbook1(64,'ridgidity rt',400,0,100.,0.)
         call hbook1(65,'ridgidity 1',400,0,100.,0.)
         call hbook1(71,'pmass',200,-2.,12.,0.)
         call hbook1(72,'perrmass/pmass',200,0.,1.,0.)
         call hbook1(73,'prob',200,0.,1.,0.)
         call hbook1(82,'pid',200,-0.5,199.5,0.)
         call hbook1(91,'ridgidity',200,-40.,40.,0.)
         call hbook1(92,'pmass',200,-2.,12.,0.)
         call hbook1(93,'pattern',25,-0.5,24.5,0.)
         call hbook1(94,'locgl',10,-0.5,9.5,0.)
         call hbook1(95,'summis',200,-0.5,199.5,0.)
         call hbook1(96,'disty',200,-1.,1.,0.)
         call hbook1(97,'distx',200,-5.,5.,0.)
         call hbook1(1000+2,'Nparticles',20,-0.5,19.5,0.)
         call hbook1(1000+11,'Trclusters',200,-0.5,199.5,0.)
         call hbook1(1000+12,'TOFclusters',50,-0.5,49.5,0.)
         call hbook1(1000+13,'mass',200,-5.,15.,0.)
         call hbook1(1000+15,'gr/r',200,-1.,4.,0.)
         call hbook1(1000+21,'ridgidity',200,-40.,40.,0.)
         call hbook1(1000+22,'charge',10,0.5,10.5,0.)
         call hbook1(1000+23,'beta',200,-1.1,1.1,0.)
         call hbook1(1000+24,'betapattern',23,-0.5,22.5,0.)
         call hbook1(1000+25,'pattern',23,-0.5,22.5,0.)
         call hbook1(1000+26,'cos(thetagl)',200,-1.,1.,0.)
         call hbook1(1000+31,'r/r1',200,-1.,4.,0.)
         call hbook1(1000+32,'r/r2',200,-1.,4.,0.)
         call hbook1(1000+41,'probtr',200,0.,1.,0.)
         call hbook1(1000+51,'chi2f',200,0.,50.,0.)
         call hbook1(1000+52,'chi2c',200,0.,10.,0.)
         call hbook1(1000+53,'chi2d',200,0.,50.,0.)
         call hbook1(1000+54,'chi2fms',200,0.,400.,0.)
         call hbook1(1000+61,'phi',200,-0.002,0.002,0.)
         call hbook1(1000+62,'the',200,-0.002,0.002,0.)
         call hbook1(1000+71,'pmass',200,-2.,12.,0.)
         call hbook1(1000+72,'perrmass/pmass',200,0.,1.,0.)
         call hbook1(1000+73,'prob',200,0.,1.,0.)
         call hbook1(1000+82,'pid',200,-0.5,199.5,0.)
         call hbook1(1000+91,'ridgidity',200,-20.,20.,0.)
         call hbook1(1000+92,'pmass',200,-2.,12.,0.)
         call hbook1(1000+98,'pmass2',200,-2.,12.,0.)
         call hbook1(1000+93,'pattern',25,-0.5,24.5,0.)
         call hbook1(1000+94,'locgl',2,-0.5,1.5,0.)
         call hbook1(1000+95,'summis',200,-0.5,199.5,0.)
         call hbook1(1000+96,'disty',200,-1.,1.,0.)
         return
      endif
*
*    Basic cuts
*
c      call hf1(2,float(npart),1.)
      if(mod(eventstatus/1048576/2,4).gt.0.and.
     +  mod(eventstatus/32,8).gt.0)then
        r=gridgidity(ptrackp(1))/ridgidity(ptrackp(1))
        if(beta(pbetap(1)).lt.0)r=-r
        call hf1(15,r,1.)        
        call hf1(21,pmom(1)/pcharge(1),1.)        
        call hf1(22,pcharge(1),1.)
        call hf1(23,beta(pbetap(1)),1.)
        call hf1(24,float(betapattern(pbetap(1))),1.)
        call hf1(25,float(pattern(ptrackp(1))),1.)
        call hf1(26,cos(thetagl(1)),1.)
        if(geanefitdone(ptrackp(1)).eq.0.or.
     +    (r.gt.0.4.and.r.lt.2.5))then
          if(xsign(pmom(1)).eq.isig.and.
     +       betapattern(pbetap(1)).lt.5.and.
     +       advancedfitdone(ptrackp(1)).ne.0)then
             r1=ridgidity(ptrackp(1))/hridgidity(1,ptrackp(1))
             r2=ridgidity(ptrackp(1))/hridgidity(2,ptrackp(1))
             call hf1(31,r1,1.)
             call hf1(32,r2,1.)
             if(r1.gt.0.4.and.r1.lt.1.6.and.r2.gt.0..and.r2.lt.160.)then
               call hf1(41,proballtracker(pchargep(1)),1.)
               if(proballtracker(pchargep(1)).gt.0.05)then
                call hf1(51,chi2fastfit(ptrackp(1)),1.)
                call hf1(52,chi2circle(ptrackp(1)),1.)
                iptr=ptrackp(1)
                d=chi2fastfit(iptr)-hchi2(1,iptr)-hchi2(2,iptr)
                call hf1(53,d,1.)
                call hf1(54,fchi2ms(ptrackp(1)),1.)
                if(d.lt.50.and.fchi2ms(ptrackp(1)).lt.200.)then
                   rp= asin(sin(hphi(2,iptr)-hphi(1,iptr)))
                   rt=htheta(2,iptr)-htheta(1,iptr)
                   call hf1(61,rp,1.)
                   call hf1(62 ,rt,1.)
                   if(rp.lt.0.01)then
                     call hf1(63,abs(pmom(1))/pcharge(1),rp*rp)
                     call hf1(64,abs(pmom(1))/pcharge(1),rt*rt)
                     call hf1(65,abs(pmom(1))/pcharge(1),1.)
                   endif
                   if(abs(rt).lt.frt(abs(pmom(1))/pcharge(1)).and.
     +                abs(rp).lt.frp(abs(pmom(1))/pcharge(1)))then
                    call hf1(71,pmass(1),1.)
                    call hf1(72,perrmass(1)/abs(pmass(1)),1.)
                    call hf1(73,probpid(1,1),1.)
                    if(probpid(1,1).gt.-0.02)then
                     call hf1(82,float(pid(1)),1.)
                     xpid=pid(1)
                     if(isig.lt.0)xpid=pid(1)-100
                     if(xpid.gt.20)then
                      summis=0
                      goto 777
                      disty=-1.e6
                      do i=1,6
                       ipm=patmiss(i,pattern(ptrackp(1))+1)
                       if(ipm.gt.1.and.ipm.lt.6)then
                        do j=1,ntrrh
                         if(layer(j).eq.ipm)then
                          dist=0
                          disty=cootr(2,ipm,1)-hitr(2,j)
                          distx=cootr(2,ipm,1)-hitr(2,j)
                          do k=1,3
                           dist=dist+(cootr(k,ipm,1)-hitr(k,j))**2
                          enddo
                          dist=sqrt(dist)
                          if(abs(distx).lt.3)call hf1(96,disty,1.)
                          call hf1(97,distx,1.)
                          if(dist.lt.1.or.
     +                     (abs(disty).lt.0.2.and.abs(distx).lt.3.))then
                           summis=summis+sumr(j)
                          endif 
                         endif
                        enddo
                       endif
                      enddo
                      call hf1(95,summis,1.)
 777                  continue
                       summis=0
                      if(summis.lt.40)then
                       if(isig.lt.0)then
                        ahe=1
                        write(26,*)run,eventno,pmass(1),pmom(1),
     +                  address(ptrackp(1))
                       else if(rndm(d).lt.0.01)then
                        ahe=1
                        write(27,*)run,eventno,pmass(1),pmom(1),
     +                  address(ptrackp(1))
                       endif
                       istat=trstatus(ptrackp(1))
                       ktofonly=mod(istat/16384,2)
                       kfalsex=mod(istat/8192,2)
                       if(mod(istat/32768/2,2).eq.1)then
                            call hf1(94,0.,1.)
                       else if(mod(istat/32768/2/2,2).eq.0)then
                          call hf1(94,0.,2.)
                       endif
                       if(mod(istat/32768/2/2,2).eq.1)then
                            call hf1(94,1.,1.)
                       endif
                       kpat=pattern(ptrackp(1))
                       call hf1(94,3.+ktofonly,1.)                 
                       call hf1(91,pmom(1)/pcharge(1),1.)
                       if(ahe.eq.1)then             
                       call hf1(1015,r,1.)
                       call hf1(1031,r1,1.)
                       call hf1(1032,r2,1.)
                       call hf1(1041,proballtracker(pchargep(1)),1.)
                       call hf1(1051,chi2fastfit(ptrackp(1)),1.)
                       call hf1(1052,chi2circle(ptrackp(1)),1.)
                       call hf1(1053,d,1.)
                       call hf1(1054,fchi2ms(ptrackp(1)),1.)
                       call hf1(1061,rp,1.)
                       call hf1(1062 ,rt,1.)
                       call hf1(1073,probpid(1,1),1.)
                       call hf1(1082,float(pid(1)),1.)
                       call hf1(1091,pmom(1)/pcharge(1),1.)
                       call hf1(1092,pmass(1),1.)
                       xpm=pmom(1)**2/beta(pbetap(1))**2*
     +                 (1-beta(pbetap(1))**2)
                       if(xpm.gt.0)then
                        xpm=sqrt(xpm)
                       else
                        xpm=-sqrt(-xpm)
                       endif
                       call hf1(1098,xpm,1.)
                       call hf1(1093,float(pattern(ptrackp(1))),1.)
                       call hf1(1095,summis,1.)
                       call hf1(1096,disty,1.)
                       call hf1(1022,pcharge(1),1.)
                       call hf1(1023,beta(pbetap(1)),1.)
                       call hf1(1024,float(betapattern(pbetap(1))),1.)
                       call hf1(1025,float(pattern(ptrackp(1))),1.)
                       call hf1(1026,cos(thetagl(1)),1.)
                      endif
                      endif                      
                     endif                      
                    endif
                   endif
                endif
               endif
             endif
          endif          
        endif
      endif     
*
      END
