      SUBROUTINE USRINIT
**************************************************************************
* Do user initialization such as histgram booking
**************************************************************************
      PRINT *, 'USRINIT called'
      CALL HBOOK1(10, 'npart', 10, 0., 10., 0.)
      CALL HBOOK1(11, 'ntrtr', 20, 0., 20., 0.)
 
      RETURN
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
*        1) In "BLOCK DATA VARDATA", change/Add the list of variables in
*           VARS which are used in the function, NVAR is the total number
*           of variables used.
*        2) set the value of this function, 1=survive, 0=fail
*
**************************************************************************
      
      INCLUDE 'cwncom.inc'
      INCLUDE 'usrcom.inc'
      INTEGER IOPT
*
      USRCUT = 0
*
      IF (IOPT .NE. 0) PRINT *,'Calling USRCUT, IOPT=',IOPT

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
c      CALL HF1(10, float(npart), 1.0)
c      CALL HF1(11, float(ntrtr), 1.0)
         if(mod(eventstatus/2097152,4).gt.0)then
          ic=mod(eventstatus/32,8)
          im=mod(eventstatus/256,2)
          if(ic.gt.0.or.im.eq.0)USRCUT =1
         endif
990      END

      SUBROUTINE USREND
**************************************************************************
* Do user termination such as printing.
**************************************************************************
      CALL HPRINT(0)

      RETURN
      END


      
C**------------- BLOCK DATA VARDATA ---------------------------------C
      BLOCK DATA VARDATA
*------------------------------------------------------------
* put the variable names below which are used in the function
*-----------------------------------------------------------
      INCLUDE 'usrcom.inc'
*-user may change following
      DATA NVAR/3/, VARLIS(1)/'NPART'/, VARLIS(2)/'NTRTR'/,
     &  VARLIS(3)/'NHITS'/
C     &  VARLIS(3)/'NHITS'/,
C     &  VARLIS(4)/'PHITS'/, VARLIS(5)/'LAYER'/, VARLIS(6)/'PX'/,
C     &  VARLIS(7)/'PY'/
*-end of user change
       END
