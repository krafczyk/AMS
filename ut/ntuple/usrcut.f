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
      DATA NVAR/1/, VARLIS(1)/'EVENTSTATUS'/
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
       integer aprf
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
*
* Requires events with pmom<0 or pcharge>1
*
      USRCUT =0
      if(mod(eventstatus/1048576/2,4).gt.0.and.
     +(mod(eventstatus/32,8).gt.0.or.mod(eventstatus/256,2).eq.0))
     +usrcut=1
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



