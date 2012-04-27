************************************************************************
      SUBROUTINE DAPLCON(X,VX,F,IRET,CHSQ)
C
*     Apply constraints f(j) = function of x   j=1,nf
*     to data X(1)...X(nx) with covariance matrix VX.
*     VX(.) in symmetric storage mode
*        1,1  1,2  2,2  1,3  2,3  3,3 ...
*
*     Restriction because of limited dimensions of
*     internal arrays:
C        NX < 51
C        NF < 21
C
*     Usage
C     =====
C
C        CALL SIMDIM(NX,NF)
c        CALL SIMDEB(JDEBUG)        optional to change debug flag
c                    JDEBUG = 0   no printout
c                           > 0   more and more printout
C
*        Now the variables X(1) ... X(NX) have to be defined
*        and their covariance matrix V(1) ...
*        variables may be measured ones or unmeasured ones.
*        unmeasured variables are characterized by zero elements
*        of the covariance matrix (at least the corresponding
*        diagonal element has to be zero). For unmeasured variables
*        the value of x has to be some reasonable initial value.
*        in addition it is necessary to define some step size
*        for numerical differentiation for the unmeasured
*        variables (for measured values the necessary step size
*        is taken from the standard deviation in the covar.
*        matrix). The call to define step ST for variable X(i) is
*           CALL SIMSTP(I,ST)
*        The user may optionally define limits for physical regions
*        of variable x(i) by
*           CALL SIMLIM(I,XLOW,XHIG)
*        The following coding example represents the loop, which
*        performs the constrained fit. during the loop the
*        variables x(1)...x(nx) are modified, to perform the
*        numerical differentiation and to apply corrections
*        during the fit. The user has to supply the code to
*        calculate the constraint equations f(1)...f(nf).
*        Finally (at convergence) the covariance matrix is modified
*        to the matrix for the fitted values, which (for sufficient
*        constraints) has nonzero elements for the previously
*        unmeasured variables.
C
C     10 F(1)=function of X(1) ... X(NX)
C        ...
C        F(NF) = function of X(1) ... X(NX)
C        CALL APLCON(X,VX,F,IRET)
C        IF(IRET.LT.0) GOTO 10
C
*        IRET = 0   convergence reached
*        IRET = 1   convergence
C
*        Non-convergence is assumed for
*        chisquare above 4 standard deviations for first 10 iterations
*        chisquare above 3 standard deviations for next 10 iterations
*        more than 20 iterations
*
*        The user may scale up his covariance matrix to force more
*        cases with convergence.
*
*        Remark to precision of the constraints:
*        a necessary condition for convergence is the reduction of
*        the constraints to about 0.001 (absolute value). If a
*        different accuracy is required, the user should scale the
*        corresponding value f(j) accordingly. Convergence may be
*        difficult due to roundoff-errors, if the required
*        accuracy is too high.
*
*        Remark to differentiation:
*        By default numerical differentiation is done, to calculate
*        the nx*nf elements of the derivative matrix for the
*        constraits w.r.t the variables. Usually this works well.
*        The user may calculate the elements in his program, for
*        reasons of speed or in cases, where the num. diff. fails.
*        The derivative matrix is array A in
*        COMMON/MATCOM/A(.)
*        with the definition
*        df(j)/dx(i) = a(i+nx*(j-1))
*        and has to be defined by the user before the call of
*        APLCON. In the first iteration of the first case the
*        program will automatically compare the elements with
*        values calculated numerically and will printout elements
*        with disagreement.
*
*
*
      IMPLICIT DOUBLE PRECISION(A-H, O-Z)
      REAL*8 X(*),VX(*),F(*)
*
*     Parameter: absolute precision in f = epsf
*

C     DIMENSION NX
      REAL*8 XS(50),DX(50),DXP(50),R(70),W(2485)
      INTEGER NRD(50)

      COMMON/DSIMCOM/NX,MYF,NUM,IFLG,LUNSIM,IDEBUG,
     +      XD(2),XL(2,50),ST(50),FC(50),H(70)
*     DERIVATIVE MATRIX
      COMMON/DMATCOM/A(1000)
      REAL*8 DR(2,200)
      EQUIVALENCE (A(1),DR(1,1))
      CHARACTER*19 TEXT(3)
      DATA         TEXT   /'CHISQUARE TOO HIGH ',
     +                     'TOO MANY ITERATIONS',
     +                     'UNPHYSICAL REGION  '/
      DATA EPSF/0.001/
      DATA ISTAT/0/,ICNT/0/
      SAVE

*     CHISQUARE LIMIT FOR +K SIGMA AND ND DEGREES OF FREEDOM IS
*     APPROXIMATELY
      CHLIM(K,ND)=0.5*(FLOAT(K)+SQRT(FLOAT(2*ND+1)))**2
*     ...
      IF(ISTAT.NE.0) GOTO 40
      IFLG=ICNT
      ICNT=ICNT+1
      ISTAT=0

*     INITIALIZATION----------------------------------------------------

*     DEFINE LOOP PARAMETERS
   10 NXF=NX+MYF
      MXF=(NXF*NXF+NXF)/2
*
      CALL DSIMMAT(VX)
*     COUNT NR OF DEGREES OF FREEDOM
   20 ND=MYF
      II=0
      DO 24 I=1,NX
      II=II+I
*     SAVE INITIAL X VALUES AND RESET CORRECTION DX
      XS(I)=X(I)
      DX(I)=0.D0
      IF(VX(II).LE.0.0) THEN
*        UNMEASURED VARIABLE
         ND=ND-1
*        CLEAR REMAINING ELEMENTS
         IJ=II-I
         DO 22 J=1,NX
         IF(J.LE.I) IJ=IJ+1
         VX(IJ)=0.D0
         IF(J.GE.I) IJ=IJ+J
   22    CONTINUE
      END IF
   24 CONTINUE
      IF(IDEBUG.GE.1) THEN
         WRITE(LUNSIM,101) NX,MYF,ND
         IF(IDEBUG.GE.2) CALL DGMPRT(X,NX,1,'OF INITIAL X-VALUES')
         IF(IDEBUG.GE.3) CALL DGMPRT(ST,NX,1,'OF INITIAL STEPS')
         IF(IDEBUG.GE.3) CALL DSMPRTC(X,VX,NX)
      END IF
*     INITIAL VALUE OF FTEST
      FTESTP=0.0
      DO 26 J=1,MYF
   26 FTESTP=FTESTP+ABS(F(J))
      FTESTP=FTESTP/FLOAT(MYF)
      ITER=0
      NCST=0
      CHSQ=0.D0

*     PREPARE NEXT ITERATION--------------------------------------------

   30 ISTAT=1
      IRET=-1
*     DEFINE RIGHT HAND SIDE OF EQUATION R
      DO 32 I=1,NX
   32 R(I)=0.D0
      DO 34 J=1,MYF
*     FC IS USED IN SIMDER
      FC(J)=F(J)
   34 R(NX+J)=-F(J)
      IF(ITER.NE.0) THEN
*        DEFINE STEPS = 0.5 SIGMA FROM W
         II=0
         DO 36 I=1,NX
         II=II+I
         IF(ST(I).EQ.0.D0.OR.W(II).EQ.0.D0) GOTO 36
         ST(I)=0.5*SQRT(ABS(W(II)))
   36    CONTINUE
      END IF

*     LOOP FOR NUMERICAL CALCULATION OF DERIVATIVES---------------------

   40 IF(ISTAT.NE.1) GOTO 90
      CALL DSIMDER(X,F,JRET)
      IF(JRET.LT.0) GOTO 100
      IF(IDEBUG.GE.3.AND.ITER.EQ.0)
     +    CALL DGMPRT(A,NX,MYF,'DERIVATIVE MATRIX')

*     CONSTRUCT MATRIX W AND CORRECT VECTOR R---------------------------

*     INSERT -V AND A INTO W AND UPDATE R
   50 IJ=(NX*NX+NX)/2
      DO 52 I=1,IJ
   52 W(I)=-VX(I)
      IA=0
      DO 58 J=1,MYF
      DO 54 I=1,NX
      R(NX+J)=R(NX+J)+A(IA+I)*DX(I)
   54 W(IJ+I)=A(IA+I)
      IJ=IJ+NX
      DO 56 K=1,J
      IJ=IJ+1
   56 W(IJ)=0.D0
   58 IA=IA+NX

*     CALCULATE STEP DELX-----------------------------------------------

      ITER=ITER+1

*     FIRST PART OF MATRIX INVERSION, MAKING USE OF
*     THE FACT, THAT ALL ELEMENTS CORRESPONDING TO
*     MEASURED VARIABLES ARE ALREADY THE INVERSE ELEMENTS

   60 NM=0
      IF(IDEBUG.GE.4) CALL DSMPRT(W,NXF,'W BEFORE MODIFICATION ')
      II=0
      DO 61 I=1,NX
      II=II+I
      IF(W(II).LT.0.D0) THEN
         DR(1,I)=0.D0
         NM=NM+1
         NRD(NM)=I
      ELSE
         W(II)=0.D0
         DR(1,I)=1.D0
      END IF
   61 CONTINUE
      DO 67 I=NX+1,NXF
      DR(1,I)=1.0
      II=(I*I-I)/2
      DO 63 M=1,NM
      J=NRD(M)
      SUM=0.D0
      JK=(J*J-J)/2
      DO 62 K=1,NX
      IF(K.LE.J) JK=JK+1
      IF(DR(1,K).EQ.0.D0) SUM=SUM+W(JK)*W(II+K)
   62 IF(K.GE.J) JK=JK+K
   63 H(J)=SUM
      DO 65 K=I,NXF
      IK=(K*K-K)/2
      WJK=0.D0
      DO 64 M=1,NM
      J=NRD(M)
   64 WJK=WJK+W(IK+J)*H(J)
   65 W(IK+I)=W(IK+I)+WJK
      DO 66 M=1,NM
      J=NRD(M)
   66 W(II+J)=-H(J)
   67 CONTINUE
*     SAVE RIGHT HAND SIDE FOR CHI**2 CALCULATION
      DO 68 J=1,MYF
   68 H(J)=R(NX+J)

*     COMPLETE MATRIX INVERSION AND CALCULATE CHISQUARE

      IF(IDEBUG.GE.4) CALL DGMPRT(R,1,NXF,'R BEFORE SOLUTION')
   70 CALL DXMINV(W,R,NXF,1,NRANK)
*     CHI**2 CALCULATION
      CHSQP=CHSQ
      CHSQ=0.0
      DO 72 J=1,MYF
   72 CHSQ=CHSQ-H(J)*R(NX+J)
      IF(IDEBUG.GE.4) CALL DSMPRT(W,NXF,'W AFTER  XMINV')
      IF(IDEBUG.GE.4) CALL DGMPRT(R,1,NXF,'R AFTER SOLUTION')

      DO 74 I=1,NX
      DXP(I)=DX(I)
   74 DX(I)=R(I)
      GOTO 84
*     MAKE CUTSTEP
   80 DO 82 I=1,NX
   82 DX(I)=0.5D0*(DX(I)+DXP(I))
*     CORRECT X AND RETURN TO TEST CONSTRAINTS
   84 DO 86 I=1,NX
   86 X(I)=XS(I)+DX(I)
      DO 88 IJ=1,NXF
   88 A(IJ)=0.D0
      ISTAT=2
      IRET=-1
      GOTO 100

*     CONVERGENCE CHECK-------------------------------------------------

*     CALCULATE VALUE OF CONSTRAINTS AND COMPARE
   90 FTEST=0.D0
      DO 92 I=1,MYF
   92 FTEST=FTEST+ABS(F(I))
      FTEST=FTEST/FLOAT(MYF)
      FTESTP=MIN(FTEST,FTESTP)
      IF(IDEBUG.GE.2) WRITE(LUNSIM,102) ITER,NCST,CHSQ,FTEST

      IF(IDEBUG.GE.3.AND.NCST.EQ.0) THEN
         CALL DGMPRT(X,NX,1,'OF X-VALUES')
         CALL DGMPRT(F,MYF,1,' OF CONSTRAINT FUNCTION VALUES')
      END IF
*     DIVERGENCE/CONVERGENCE TESTS
      IF(FTEST.GT.1.1*FTESTP+0.01) THEN
*        DIVERGENCE, MAKE CUT STEPS
         NCST=NCST+1
         IF(NCST.LT.5) GOTO 80
      ELSE IF(NCST.EQ.0) THEN
         IF((ITER.GE.2.OR.CHSQ.LT.CHLIM(1,ND)).AND.
     1   (FTEST.LT.EPSF.AND.CHSQ-CHSQP.LT.0.1)) THEN
*           CONVERGENCE
            IF(IDEBUG.GE.1) WRITE(LUNSIM,104)ITER
            IF(IDEBUG.GE.2) CALL DGMPRT(X,NX,1,'OF FINAL X-VALUES')
*           PULLS
            II=0
            DO 94 I=1,NX
            II=II+I
            A(I)=0.D0
            IF(VX(II).GT.0.0) THEN
               IF(VX(II)-W(II).GT.0.0) A(I)=DX(I)/SQRT(VX(II)-W(II))
            END IF
   94       CONTINUE
            DO 96 I=1,(NX*NX+NX)/2
   96       VX(I)=W(I)
            IF(IDEBUG.GE.2) CALL DGMPRT(A,NX,1,'OF PULLS')
            IF(IDEBUG.GE.3) CALL DSMPRTC(X,VX,NX)
            IRET =0
            ISTAT=0
            GOTO 100
         END IF
      END IF
*     CONTINUE WITH ITERATION OR STOP
      NCST=0
      IF(ITER.GE. 2.AND.CHSQ.GT.CHLIM(4,ND))  IRET= 1
      IF(ITER.GT.10.AND.CHSQ.GT.CHLIM(3,ND))  IRET= 1
      IF(ITER.GT.20)                          IRET= 2
      IF(IRET.LT.0) GOTO 30
      IF(IDEBUG.GE.1) WRITE(LUNSIM,105) ITER,IRET,TEXT(IRET)
      ISTAT=0

  100 RETURN
  101 FORMAT('0-APLCON-   NX =',I3,'     MF =',I3,'     ND =',I3,
     +    '    -APLCON-')
  102 FORMAT(' ITERATION',I3,'.',I1,'   CHISQUARE =',
     + G12.5,'   FTEST =',G12.5)
  104 FORMAT(' CONVERGENCE AFTER',I3,' ITERATIONS')
  105 FORMAT(' NO CONVERGENCE (',I3,' ITER, IRET =',I2,')  ',A)
      END

************************************************************************
      SUBROUTINE DSMTOG(V,A,N)
C
C     SUBROUTINE SMTOG
C     ----------------
C     COPY SYMMETRIC N-BY-N MATRIX V TO GENERAL N-BY-N MATRIXA
C
C                   -   -
C        CALL SMTOG(V,A,N)        A := V
C                     -
C
C
      REAL*8 V(1),A(1)
      SAVE
   10 IJV=0
      DO 20 I=1,N
      IJA=I
      DO 20 J=1,I
      IJV=IJV+1
      A(IJA)=V(IJV)
      A(IJA+(N-1)*(I-J))=V(IJV)
   20 IJA=IJA+N
  100 RETURN
      END

************************************************************************
      SUBROUTINE DSMTOS(V,I,W,J,N)
C
C     SUBROUTINE SMTOS
C     ----------------
C     COPY SYMMETRIC N-BY-N MATRIX OR A N-BY-N SUBMATRIX OF A  SYMMETRIC
C     MATRIX TO ANOTHER SYMMETRIC MATRIX
C
C                     - -     -
C          CALL SMTOS(V,I,W,J,N)
C                         - -
C
C     N ROWS AND COLUMNS OF THE MATRIX V, STARTING FROM DIAGONAL ELEMENT
C     (I,I), ARE COPIED TO THE MATRIX W, STARTING  IN  DIAGONAL  ELEMENT
C     (J,J). THUS IF A COMPLETE SYMMETRIC MATRIX HAS TO BE COPIED, I=J=1
C     HAS TO BE USED.
C
      REAL*8 V(1),W(1)
      SAVE
      IM=(I*I+I)/2-1
      JM=(J*J+J)/2-1
      DO 20 K=1,N
         DO 10 L=1,K
            IM=IM+1
            JM=JM+1
            W(JM)=V(IM)
   10    CONTINUE
         IM=IM+I-1
         JM=JM+J-1
   20 CONTINUE
  100 RETURN
      END
      
************************************************************************
      SUBROUTINE DGMTOS(A,V,N)
C
C     SUBROUTINE GMTOS
C     ----------------
C     COPY GENERAL N-BY-N MATRIX A, ASSUMED TO BE A SYMMETRIC MATRIX, TO
C     A SYMMETRIC MATRIX V
C
C                   -   -
C        CALL GMTOS(A,V,N)        V := A
C                     -
C
C
      IMPLICIT DOUBLE PRECISION (A-H,O-Z)

      REAL*8 A(1),V(1)
      SAVE
      IJV=0
      DO 10 I=1,N
      IJA=I
      DO 10 J=1,I
      IJV=IJV+1
      V(IJV)=A(IJA)
   10 IJA=IJA+N
  100 RETURN
      END

************************************************************************
      SUBROUTINE DSMAVAT(V,A,W,N,M)
C
C     SUBROUTINE SMAVAT
C     -----------------
C
C     MULTIPLY SYMMETRIC N-BY-N MATRIX FROM THE LEFT WITH GENERAL M-BY-N
C     MATRIX AND FROM THE RIGHT WITH THE TRANSPOSED OF THE SAME  GENERAL
C     MATRIX  TO  FORM  SYMMETRIC  M-BY-M   MATRIX   (USED   FOR   ERROR
C     PROPAGATION).
C
C                    - -   - -
C        CALL SMAVAT(V,A,W,N,M)
C                        -
C                                  T
C         W   =   A   *   V   *   A
C        M*M     M*N     N*N     N*M
C
C
C        WHERE V = SYMMETRIC N-BY-N MATRIX
C              A = GENERAL N-BY-M MATRIX
C              W = SYMMETRIC M-BY-M MATRIX
C
      IMPLICIT DOUBLE PRECISION (A-H,O-Z)
      REAL*8 V(*),A(*),W(*)
      DOUBLE PRECISION CIK
      SAVE
C     ...
      MM=(M*M+M)/2
      DO 1 I=1,MM
    1 W(I)=0.D0
C
      IL=-N
      IJS=0
      DO 50 I=1,M
      IJS=IJS+I-1
      IL=IL+N
      LKL=0
      DO 50 K=1,N
      CIK=0.D0
      LKL=LKL+K-1
      LK=LKL
      DO 10 L=1,K
         LK=LK+1
         CIK=CIK+A(IL+L)*V(LK)
   10 CONTINUE
      IF(K.EQ.N)GOTO 30
      KP=K+1
      DO 20 L=KP,N
         LK=LK+L-1
         CIK=CIK+A(IL+L)*V(LK)
   20 CONTINUE
   30 JK=K
      IJ=IJS
      DO 40 J=1,I
         IJ=IJ+1
         W(IJ)=W(IJ)+CIK*A(JK)
         JK=JK+N
   40 CONTINUE
   50 CONTINUE
C
  100 RETURN
      END

*******************************************************************************
      SUBROUTINE DSMINV(V,B,N,M,NRANK)
C
C     SUBROUTINE SMINV
C     ----------------
C     OBTAIN SOLUTION OF A SYSTEM OF LINEAR EQUATIONS V *  X  =  B  WITH
C     SYMMETRIC MATRIX V AND INVERSE (FOR M =  1)  OR  MATRIX  INVERSION
C     ONLY (FOR M = 0)
C
C                   - - - -
C        CALL SMINV(V,B,N,M,NRANK)
C                   - -     -----
C
C           V = SYMMETRIC N-BY-N MATRIX IN SYMMETRIC STORAGE MODE
C               V(1) = V11, V(2) = V12, V(3) = V22, V(4) = V13, . . .
C               REPLACED BY INVERSE MATRIX
C           B = N-VECTOR   (FOR M = 0 USE A DUMMY ARGUMENT)
C               REPLACED BY SOLUTION VECTOR
C           M = SEE ABOVE
C
C
C     METHOD OF SOLUTION IS BY ELIMINATION SELECTING THE  PIVOT  ON  THE
C     DIAGONAL EACH STAGE. THE RANK OF THE MATRIX IS RETURNED IN  NRANK.
C     FOR NRANK NE N, ALL REMAINING  ROWS  AND  COLS  OF  THE  RESULTING
C     MATRIX V AND THE CORRESPONDING ELEMENTS OF  B  ARE  SET  TO  ZERO.
C     SMINV USES A WORK ARRAY OF 2*N WORDS IN COMMON/MATCOM/. FOR N> 200
C     THE USER HAS TO DEFINE COMMON/MATCOM/ WITH 2*N WORDS.
C
C
      IMPLICIT DOUBLE PRECISION (A-H,O-Z)

      REAL*8 V(1),B(1),VKK,C,D,E
      COMMON/DMATCOM/DR(2,200)
      DATA EPS/1.E-6/
      SAVE
C
C     CONSTRUCT TABLE
C
      DO 10 I=1,N
   10 DR(1,I)=1.D0
      NI=N
      GOTO 14

      ENTRY DXMINV(V,B,N,M,NRANK)
      NI=0
      DO 12 I=1,N
      IF(DR(1,I).EQ.0.D0) GOTO 12
      NI=NI+1
   12 CONTINUE
   14 II=0
      DO 16 I=1,N
      II=II+I
   16 DR(2,I)=ABS(V(II))
C
C     LOOP BEGIN
C
      NRANK=N-NI
      DO 60 I=1,NI
C
C     SEARCH FOR PIVOT AND TEST FOR LINEARITY AND ZERO MATRIX
C
      K=0
      JJ=0
      VKK=0.D0
      DO 20 J=1,N
      JJ=JJ+J
      IF(DR(1,J).EQ.0.D0) GOTO 20
      IF(ABS(V(JJ)).LE.VKK) GOTO 20
      IF(ABS(V(JJ)).LT.EPS*DR(2,J)) GOTO 20
      VKK=ABS(V(JJ))
      K=J
      KK=JJ
   20 CONTINUE
      IF(K.EQ.0) GOTO 80
C
C     PREPARATION FOR ELIMINATION
C
      NRANK=NRANK+1
      DR(1,K)=0.D0
      D=1.D0/V(KK)
      V(KK)=-D
      IF(M.EQ.1) B(K)=B(K)*D
      JK=KK-K
      JL=0
C
C          ELIMINATION
C
      DO 50 J=1,N
      IF(J-K) 24,22,26
   22 JK=KK
      JL=JL+J
      GOTO 50
   24 JK=JK+1
      GOTO 28
   26 JK=JK+J-1
   28 E=V(JK)
      V(JK)=D*E
      IF(M.EQ.1) B(J)=B(J)-B(K)*E
      LK=KK-K
      DO 40 L=1,J
      JL=JL+1
      IF(L-K) 34,32,36
   32 LK=KK
      GOTO 40
   34 LK=LK+1
      GOTO 38
   36 LK=LK+L-1
   38 V(JL)=V(JL)-V(LK)*E
   40 CONTINUE
   50 CONTINUE
   60 CONTINUE
C
C          CHANGE SIGN
C
      IJ=0
      DO 70 I=1,N
      DO 70 J=1,I
      IJ=IJ+1
   70 V(IJ)=-V(IJ)
      GOTO 100
C
C          CLEAR REST OF MATRIX
C
   80 IJ=0
      DO 90 I=1,N
      IF(M.EQ.1.AND.DR(1,I).NE.0.D0) B(I)=0.D0
      DO 90 J=1,I
      IJ=IJ+1
      IF(DR(1,I)+DR(1,J).NE.0.D0) V(IJ)=0.D0
   90 V(IJ)=-V(IJ)
  100 RETURN
      END

*******************************************************************************
      SUBROUTINE DGMAB(A,B,C,N,M,L)
C
C     SUBROUTINE GMAB
C     ---------------
C     MULTIPLY GENERAL N-BY-M MATRIX AND GENERAL M-BY-L MATRIX  TO  FORM
C     GENERAL N-BY-L MATRIX
C
C                  - -   - - -
C        CALL GMAB(A,B,C,N,M,L)
C                      -
C
C         C  :=    A   *    B
C        N*L      N*M      M*L
C
C
      IMPLICIT DOUBLE PRECISION (A-H,O-Z)
      REAL*8 A(1),B(1),C(1)
      DOUBLE PRECISION SUM
      SAVE
      
      IJ=0
      IK=0
      DO 30 K=1,N
         DO 20 I=1,L
            JK=I
            SUM=0.D0
            DO 10 J=1,M
               SUM=SUM+A(IJ+J)*B(JK)
               JK=JK+L
   10       CONTINUE
            IK=IK+1
            C(IK)=SUM
   20    CONTINUE
         IJ=IJ+M
   30 CONTINUE
      RETURN
      END
      
*******************************************************************************
      SUBROUTINE DGMINV(A,B,N,MV,DET)
      IMPLICIT DOUBLE PRECISION (A-H,O-Z)
      REAL*8 A(*),B(*)
      COMMON/DMATCOM/LRC(2,200)
C
C     SUBROUTINE GMINV
C     ----------------
C     OBTAIN SOLUTION OF A SYSTEM  OF  N  LINEAR  EQUATIONS  A*X=B  WITH
C     GENERAL N*N MATRIX A AND INVERSE OF A (FOR MV=1) OR INVERSION ONLY
C     (FOR MV=0)
C
C                   - - - --
C        CALL GMINV(A,B,N,MV,DET)
C                   - -      ---                               -1
C        WHERE    A = GENERAL N*N MATRIX, REPLACED BY INVERSE A
C                 B = N VECTOR, REPLACED BY SOLUTION VECTOR X
C                     (FOR MV=0 USE A DUMMY ARGUMENT)
C                MV = SEE ABOVE
C               DET = DETERMINANT OF MATRIX A
C
C     A DETERMINANT OF ZERO  INDICATES  A  SINGULAR  MATRIX.  METHOD  OF
C     SOLUTION IS BY ELIMINATION OF THE LARGEST PIVOTAL DIVISOR AT  EACH
C     STAGE (STANDARD GAUSS-JORDAN METHOD)  WITH  INTERMEDIATE  ROW  AND
C     COLUMN INTERCHANGE. GMINV USES  A  WORK  ARRAY  OF  2*N  WORDS  IN
C     COMMON/MATCOM/. FOR N> 200 THE USER HAS TO  DEFINE  COMMON/MATCOM/
C     WITH 2*N WORDS.
C
      SAVE
      
      DET=1.D0
      DO 10 K=1,N
      IF(MV.EQ.1) B(K)=-B(K)
      LRC(1,K)=K
   10 LRC(2,K)=K
C
      DO 60 IJK=1,N
C
C     SEARCH FOR PIVOT
      PIV=0.D0
      LK=0
      DO 20 L=1,N
      IF(LRC(1,L).EQ.0) GOTO 20
      DO 15 K=1,N
      IF(LRC(2,K).EQ.0) GOTO 15
      IF(ABS(A(LK+K)).LE.ABS(PIV)) GOTO 15
      PIV=A(LK+K)
      IJ=LK+K
      PIV=A(IJ)
      I=L
      J=K
   15 CONTINUE
   20 LK=LK+N
C     INVERT PIVOT
      DET=DET*PIV
C     SINGULAR MATRIX FOR ZERO PIVOT
      IF(PIV.EQ.0.D0) GOTO 100
      A(IJ)=1.D0/PIV
C     EXCHANGE ROWS I AND LC(J)
      IF(I.EQ.LRC(2,J)) GOTO 26
      IB=(I-1)*N
      MB=(LRC(2,J)-I)*N
      DO 25 K=1,N
      IB=IB+1
      H=A(IB)
      A(IB)=A(IB+MB)
   25 A(IB+MB)=H
      IF(MV.EQ.0) GOTO 26
      H=B(I)
      B(I)=B(LRC(2,J))
      B(LRC(2,J))=H
C     EXCHANGE COLS LRC(1,I) AND J
   26 IF(LRC(1,I).EQ.J) GOTO 35
      IB=LRC(1,I)
      MB=J-LRC(1,I)
      DO 30 K=1,N
      H=A(IB)
      A(IB)=A(IB+MB)
      A(IB+MB)=H
   30 IB=IB+N
C     UPDATE POINTER
   35 IS=LRC(1,I)
      JS=LRC(2,J)
      LRC(1,I)=LRC(1,JS)
      LRC(2,J)=LRC(2,IS)
      I=IS
      J=JS
      LRC(1,J)=0
      LRC(2,I)=0
C     DIVIDE ROW BY -PIV
      JK=(J-1)*N
      DO 40 K=1,N
      JK=JK+1
      IF(K.EQ.I) GOTO 40
      A(JK)=-A(JK)/PIV
   40 CONTINUE
      IF(MV.EQ.1) B(J)=-B(J)/PIV
C     REDUCE REST OF MATRIX AND DIVIDE COL I BY PIV
      JK=(J-1)*N
      IK=I
      LK=0
      DO 50 L=1,N
      IF(L.EQ.J) GOTO 49
      IF(MV.EQ.1) B(L)=B(L)+B(J)*A(IK)
      DO 45 K=1,N
      IF(K.EQ.I) GOTO 45
      A(LK+K)=A(LK+K)+A(JK+K)*A(IK)
   45 CONTINUE
      A(IK)=A(IK)/PIV
   49 IK=IK+N
   50 LK=LK+N
C
   60 CONTINUE
  100 RETURN
      END
      
*******************************************************************************
      SUBROUTINE DGMPRT(A,N,M,TEXT)
C
C     SUBROUTINE GMPRT
C     ----------------
C     GMPRT PRINTS THE N-BY-M GENERAL MATRIX A
C
C                   - - - ----
C        CALL GMPRT(A,N,M,TEXT)
C
C
C
      IMPLICIT DOUBLE PRECISION (A-H,O-Z)
      REAL*8 A(1)
      CHARACTER*(*) TEXT
      SAVE
C
C     GENERAL MATRIX OR VECTOR
C
   10 IF(N.NE.1.AND.M.NE.1) GOTO 20
C     VECTOR
      NM=N*M
      WRITE(6,101) N,M,TEXT
      WRITE(6,102) (A(L),L=1,NM)
      GOTO 90
C     MATRIX
   20 WRITE(6,103) N,M,TEXT
      IJ=0
      DO 25 I=1,N
      WRITE(6,104) I,(A(IJ+L),L=1,M)
   25 IJ=IJ+M
   90 WRITE(6,102)
  100 RETURN
  101 FORMAT('0',I4,' *',I3,'  VECTOR ',A/)
  102 FORMAT(10X,10G12.5)
  103 FORMAT('0',I4,' *',I3,'  MATRIX ',A/)
  104 FORMAT(1X,I4,5X,12(1PE12.4)/(10X,12(1PE12.4)/))
      END

*******************************************************************************
      SUBROUTINE DSMPRT(V,N,TEXT)
C
C     SUBROUTINE SMPRT
C     ----------------
C     GMPRT PRINTS THE SYMMETRIC N-BY-N MATRIX V
C
C                   - - ----
C        CALL SMPRT(V,N,TEXT)
C
C
C
      IMPLICIT DOUBLE PRECISION (A-H,O-Z)
      REAL*8 V(1)
      CHARACTER*(*) TEXT
      SAVE
      WRITE(6,101) N,N,TEXT
      IF(N.LE.0) GOTO 100
      II=0
      DO 10 I=1,N
      WRITE(6,102) I,(V(II+J),J=1,I)
   10 II=II+I
      WRITE(6,102)
  100 RETURN
  101 FORMAT('0',I4,' BY',I3,' SYMMETRIC MATRIX ',A/)
  102 FORMAT(1X,I4,5X,10G12.5/(10X,10G12.5/))
      END

************************************************************************
      SUBROUTINE DERRPRP(X,VX,Y,VY,IRET)
C
C     ERROR PROPAGATION FOR Y = FUNCTION OF X
C        TO DATA X,VX,NX       NX+MYF<51
C
C        CALL SIMDIM(NX,NY)
C     10 Y(1)=FUNCTION OF X
C        CALL ERRPRP(X,VX,Y,VY,IRET)
C        IF(IRET.LT.0) GOTO 10
C
C
      IMPLICIT REAL*8 (A-H,O-Z)
      REAL*8 X(*),VX(*),Y(*),VY(*)

      COMMON/DSIMCOM/NX,MYF,NUM,IFLG,LUNSIM,IDEBUG,
     +      XD(2),XL(2,50),ST(50),FC(50),H(70)

*     DERIVATIVE MATRIX
      COMMON/DMATCOM/A(1000)

      DATA ISTAT/0/,ICNT/0/
      SAVE
*     ...
      IF(ISTAT.NE.0) GOTO 20
      IF(IDEBUG.GE.1) WRITE(LUNSIM,101) NX,MYF
      IF(IDEBUG.GE.2) CALL DGMPRT(X,NX,1,'OF X-VALUES')
      IF(IDEBUG.GE.3) CALL DSMPRTC(X,VX,NX)
      IFLG=ICNT
      ICNT=ICNT+1
      ISTAT=1

      CALL DSIMMAT(VX)

      IRET=-1
      DO 10 J=1,MYF
*     FC IS USED IN SIMDER
   10 FC(J)=Y(J)

*     LOOP FOR NUMERICAL CALCULATION OF DERIVATIVES---------------------

   20 CALL DSIMDER(X,Y,JRET)
      IF(JRET.LT.0) GOTO 100
      IF(IDEBUG.GE.3) CALL DGMPRT(A,NX,MYF,'DERIVATIVE MATRIX')
      IRET=0
      ISTAT=0
      CALL DSMAVAT(VX,A,VY,NX,MYF)
      DO 30 I=1,MYF
   30 Y(I)=FC(I)
      IF(IDEBUG.GE.2) CALL DGMPRT(Y,MYF,1,'OF TRANSFORMED PARAMETERS')
      IF(IDEBUG.GE.3) CALL DSMPRTC(Y,VY,MYF)

  100 RETURN
  101 FORMAT('0-ERRPRP-   NX =',I3,'     MY =',I3,'   -ERRPRP-')
      END


************************************************************************
      SUBROUTINE DSIMDIM(NI,NJ)
*     DIMENSIONS OF X AND F
      IMPLICIT REAL*8 (A-H,O-Z)
      PARAMETER (LUNP=6)

      COMMON/DSIMCOM/NX,MYF,NUM,IFLG,LUNSIM,IDEBUG,
     +      XD(2),XL(2,50),ST(50),FC(50),H(70)

*     NX       = number of parameters
*     NYF      = number of transformed parameters/constraint equations
*     NUM      = flag for numerical differentiation
*     IFLG     = flag for first case (check derivative)
*     LUNSIM   = printout unit (see parameter statement)
*     ST(50)   = step sizes for numerical differentiation
*     XL(2,50) = lower and upper values of parameters
*     XD(2)    = for current parameter displaced values for differ.
*     H(70)    =
*     FC(50)   = central values of parameters
*     IDEBUG   = debug flag value

*     A        = derivative matrix a/flags during matrix inversion/pulls
      COMMON/DMATCOM/A(1000)
      REAL*8 X(*),F(*),VX(*)
      LOGICAL LIMDEF,INIT
      DATA KDEBUG/1/
*     debug flag = 0   no printout
*                = 1   only warnings are printed (default)
*                = 2,3 more and more printout
      SAVE
*     ...
      NX=NI
      MYF=NJ
      LUNSIM=LUNP
      IDEBUG=KDEBUG
      INIT=.TRUE.
*     CLEAR DERIVATIVE MATRIX A AND STEPS ST
      DO 10 IJ=1,NX*MYF
   10 A(IJ)=0.D0
      DO 12 I=1,NX
      XL(1,I)=0.D0
      XL(2,I)=0.D0
   12 ST(I)=0.D0
      GOTO 100

      ENTRY DSIMDEB(JDEBUG)
      KDEBUG=JDEBUG
      GOTO 100

      ENTRY DSIMSTP(IA,STEP)
      IF(IA.LT.1.OR.IA.GT.NX) GOTO 100
*     STEP FOR NUMERICAL DIFFERENTIATION OF X(IA)
      ST(IA)=ABS(STEP)
      GOTO 100

      ENTRY DSIMLIM(IA,XLOW,XHIG)
      IF(IA.LT.1.OR.IA.GT.NX) GOTO 100
*     LOWER OR UPPER LIMIT OF X(IA)
      XL(1,IA)=MIN(XLOW,XHIG)
      XL(2,IA)=MAX(XLOW,XHIG)
      GOTO 100

      ENTRY DSIMMAT(VX)
*     NUM =0 ANALYTICAL DERS        NUM = 1 OR 2 NUMERICAL DERS
   20 NUM=0
      DO 22 IJ=1,NX*MYF
      IF(A(IJ).NE.0.D0) GOTO 24
   22 CONTINUE
      NUM=1
*     FORCE NUMERICAL DERIVATIVES FOR FIRST CALL
   24 IF(NUM.EQ.0.AND.IFLG.EQ.0.AND.IDEBUG.GE.0) NUM=2
      IF(NUM.EQ.0) GOTO 100
*     DEFINE STEPS FROM COVARIANCE MATRIX
      II=0
      DO 26 I=1,NX
      II=II+I
      VII=ABS(VX(II))
      IF(VII.NE.0.D0) ST(I)=0.5D0*SQRT(VII)
   26 CONTINUE
      GOTO 100

      ENTRY DSIMDER(X,F,JRET)

*     INITIALIZE DERIVATIVE CALCULATION---------------------------------

      IF(INIT) THEN
*        START WITH FIRST VARIBALE
         I=0
         INIT=.FALSE.
         GOTO 50
      END IF
      JRET=-1
*     DERIVATIVE CALCULATION -------------------------------------------
      IF(I.LT.0) GOTO 70
      X(I)=XSAVE
      IJ=I
      DO 30 J=1,MYF
*     CALCULATION OF NUMERICAL DERIVATIVE
      IF(ILR.EQ.0) THEN
*        SYMMETRIC FORMULA
         DER=0.5*(H(J)-F(J))/ST(I)
      ELSE
*        ASYMMETRIC FORMULA
         DER=0.5*(3.0*FC(J)+F(J)-4.0*H(J))/ST(I)
         IF(ILR.EQ.2) DER=-DER
      END IF
*     COMPARE DERIVATIVES FOR NUM=2
      IF(NUM.EQ.2) THEN
         IF(ABS(A(IJ)-DER).GT.0.005*(ABS(A(IJ))+ABS(DER))) THEN
            WRITE(LUNSIM,101) J,I,A(IJ),DER
         END IF
      END IF
*     INSERT INTO A
      A(IJ)=DER
   30 IJ=IJ+NX
*     TEST END CONDITION
   40 IF(I.EQ.NX) THEN
         JRET=0
         INIT=.TRUE.
         IF(NUMN.EQ.2) NUM=0
         GOTO 100
      END IF
*     NEXT VARIABLE
   50 JRET=-1
      I=I+1
      IF(ST(I).EQ.0.0) GOTO 40
      IREDUC=0
      XSAVE=X(I)
*     CENTRAL DIFFERENTIATION
   60 ILR=0
      LIMDEF=XL(1,I).NE.XL(2,I)
      XD(1)=XSAVE+ST(I)
      IF(LIMDEF.AND.XD(1).GT.XL(2,I)) THEN
*        ABOVE UPPER LIMIT
         XD(1)=XSAVE-ST(I)
         IF(LIMDEF.AND.XD(1).LT.XL(1,I)) GOTO 80
         XD(2)=XSAVE-ST(I)-ST(I)
         IF(LIMDEF.AND.XD(2).LT.XL(1,I)) GOTO 80
*        LEFT DIFFERENTIATION
         ILR=1
      ELSE
         XD(2)=XSAVE-ST(I)
         IF(LIMDEF.AND.XD(2).LT.XL(1,I)) THEN
*           BELOW LOWER LIMIT
            XD(2)=XSAVE+ST(I)+ST(I)
            IF(LIMDEF.AND.XD(2).GT.XL(2,I)) GOTO 80
*           RIGHT DIFFERENTIATION
            ILR=2
         END IF
      END IF
C     FIRST STEP
      X(I)=XD(1)
      I=-I
      GOTO 100
   70 DO 72 J=1,MYF
   72 H(J)=F(J)
*     SECOND STEP
      I=-I
      X(I)=XD(2)
      GOTO 100
*     REDUCE STEP SIZE
   80 IF(IREDUC.GE.4) GOTO 40
      ST(I)=ST(I)/3.0
      IREDUC=IREDUC+1
      GOTO 60

  100 RETURN
  101 FORMAT('0DERIVATIVE DF(',I2,')/DX(',I2,') = ',G15.5,' VERSUS ',
     + G15.5,' (NUMERICAL) '/)
      END

************************************************************************
      SUBROUTINE DSMPRTC(X,V,N)
C
C     SUBROUTINE SMPRTC
C     -----------------
C     SMPRTC PRINTS THE N-VECTOR X AND THE SYMMETRIC  N-BY-N  COVARIANCE
C     MATRIX V, THE LATTER AS A CORRELATION MATRIX.
C
C                    - -
C        CALL SMPRTV(V,N)       OPTIONAL, IF PRINTOUT OF GLOBAL
C                               CORRELATION IS WANTED IN ADDITION,
C                               V = COVARIANCE MATRIX BEFORE INVERSION
C
C                    - - -
C        CALL SMPRTC(X,V,N)     X = VECTOR OF PARAMETERS
C                               V = COVARIANCE MATRIX
C
      IMPLICIT REAL*8 (A-H,O-Z)
      REAL*8 X(1),V(1),C(15),D(50)
      CHARACTER*8 PTEXT(2)
      DATA    ISW/0/
      DATA    PTEXT/'        ','GLOBAL  '/
      SAVE
      WRITE(6,101) PTEXT(ISW+1)
      II=0
      DO 40 I=1,N
      IJ=II
      II=II+I
      IF(I.GT.50) ISW=0
      IF(ISW.EQ.0) GOTO 5
      GLB=0.0
      PD=V(II)*D(I)
      IF(PD.GT.1.0) GLB=SQRT(1.D0-1.D0/PD)
    5 ERR=0.0
      IF(V(II).GT.0.0) ERR=SQRT(V(II))
C
      L=0
      JJ=0
      DO 30 J=1,I
      JJ=JJ+J
      IJ=IJ+1
      RHO=0.0
      PD=V(II)*V(JJ)
      IF(PD.GT.0.D0) RHO=V(IJ)/SQRT(PD)
C
      L=L+1
      C(L)=RHO
      IF(J.NE.I.AND.L.NE.15) GOTO 30
      IF(J.GT.15) GOTO 10
      IF(ISW.EQ.0) WRITE(6,102) I,X(I),ERR,    (C(M),M=1,L)
      IF(ISW.EQ.1) WRITE(6,104) I,X(I),ERR,GLB,(C(M),M=1,L)
      GOTO 20
   10 WRITE(6,103) (C(M),M=1,L)
   20 L=0
   30 CONTINUE
   40 CONTINUE
      ISW=0
      GOTO 100
C
C
      ENTRY DSMPRTV(V,N)
C
C
C     SMPRTV CAN BE CALLED BEFORE SMPRTC, TO TRANSMIT THE COVARIANCE
C     MATRIX BEFORE INVERSION.
C
      ISW=1
      II=0
      NMOD=MIN(N,50)
      DO 50 I=1,NMOD
         II=II+I
   50 D(I)=V(II)
  100 RETURN
  101 FORMAT('0',5X,'PARAM',7X,'ERROR',7X,'CORRELATION COEFFICIENTS'/
     1       30X,A6,'   TO PARAM . . .')
  102 FORMAT(1X,I2,2G12.4,10X,15F6.2)
  103 FORMAT(37X,15F6.2)
  104 FORMAT(1X,I2,2G12.4,3X,F6.2,1X,15F6.2)
      END

************************************************************************
      SUBROUTINE DVALLEY(F,A,NC)
      IMPLICIT DOUBLE PRECISION (A-H,O-Z)
C
C
C     SUBROUTINE VALLEY SEARCHES FOR THE MINIMUM OF A FUNCTION OF
C     N PARAMETERS A(1) . . . A(N) AND, ASSUMING THAT THE FUNCTION
C     IS THE SUM OF THE SQUARES OF THE DIFFERENCES BETWEEN THEORY
C     AND EXPERIMENTAL DATA (CHI SQUARE), IT CALCULATES THE
C     COVARIANCE MATRIX V OF THE PARAMETER. IN A MAXIMUM LIKELIHOOD
C     FIT, THE FUNCTION HAS TO BE -2 TIMES THE LOG OF THE LIKELIHOOD
C     FUNCTION.
C
C
C     USAGE FOR N PARAMETER MINIMIZATION OF A (CHISQUARE) FUNCTION
C     ===== WITH A MAXIMUM OF (ABOUT) NFLIM FUNCTION EVALUATIONS
C
C                ST(1) = STEP SIZE FOR FIRST PARAMETER
C                ...     ...
C                ST(N) = STEP SIZE FOR LAST PARAMETER
C
C                CALL VALLIN(N,ST,NFLIM)
C
C                A(1)  = START VALUE OF FIRST PARAMETER
C                ...     ...
C                A(N)  = START VALUE OF LAST PARAMETER
C
C             10 F=VALUE OF (CHISQUARE) FUNCTION FOR CURRENT VALUES
C                  OF PARAMATERS A(1) . . . A(N)
C
C                CALL VALLEY(F,A,NC)
C                IF(NC.LE.0) GOTO 10
C
C     FURTHER DETAILS
C
C     NUMBER OF PARAMETERS N = 1 . . . 20
C     REASONABLE VALUE FOR NFLIM IS 3*N*(N+10) (IN MOST CASES)
C     ST(I)=0.0 MEANS PARAMETER A(I) IS KEPT FIXED
C     FINAL COVARIANCE MATRIX IS IN ARRAY V (SYMMETRIC STORAGE MODE) IN
C        COMMON/VALCOM/V(210)
C     INFORMATION ON STEPS AND THE FINAL RESULT IS PRINTED. PRINTOUT OF
C        STEP INFORMATION IS SUPPRESSED BY SETTING N OR NFLIM NEGATIVE.
C        ALL PRINTOUT IS SUPPRESSED BY SETTING N AND NFLIM NEGATIVE.
C     NC = -1 FOR LAST FUNCTION EVALUATION
C        =  1 IF CONVERGENCE IS REACHED
C        =  2 IF ENDED WITHOUT CONVERGENCE
C
C     IN ANY CASE THE LAST FUNCTION EVALUATION WILL BE AT THE MINIMUM
C         OBTAINED SO FAR.
C
C
C
C
      REAL*8 A(*)
      REAL*8 STEP(20),AD(20),DA(20),DG(20),FD(20),FDL(20),AS(20)
      REAL*8 FS(3),DF,U(400)
      REAL*8 AOPT(20)
      EQUIVALENCE (N,FN)
      COMMON/VALCOM/V(210)
      CHARACTER*4 TS,TA,TB,TN,TT
      DATA TS/'-   '/,TA/'*   '/,TB/'    '/,TN/' NO '/
      DATA EPS/1.0E-4/,REDC/0.1/,RELAX/0.9/
      SAVE
C
C
      NC=0
      IF(J.GT.N) GOTO 98
      NFUN=NFUN+1
      IF(J) 82,70,70
C
C
C
      ENTRY DVALLIN(F,A,NC)
      FN=F
      IPR=0
      IF(N.LT.0) IPR=1
      IF(NC.LT.0) IPR=IPR+1
      N=IABS(N)
      NLIM=IABS(NC)
      IJ=0
      M=0
      DO 4 I=1,N
      STEP(I)=0.D0
      FD(I)=0.D0
      FDL(I)=0.D0
      DO 2 J=1,N
    2 U(IJ+J)=0.D0
      IF(A(I).EQ.0.D0) GOTO 4
      STEP(I)=1.D0
      M=M+1
      U(IJ+I)=A(I)
    4 IJ=IJ+N
      NN=(N*N+N)/2
      N2=N*N
      DO 6 I=1,NN
    6 V(I)=0.D0
      NP=3
      NFUN=0
      NEDEF=0
      IT=1
      FSTEP=1.D0
      GOTO 12
C
C
C
C
C     (1)   SINGLE PARAMETER/EIGENVECTOR SEARCH - NEXT INDEX
C
   10 NP=8
   12 J =0
      JP=N
   14 L =J
   15 J =J+1
      IF(J.GT.N) GOTO 18
      IF(STEP(J).EQ.0.D0) GOTO 15
      IF(NEDEF.NE.0.AND.V((J*J+J)/2).GE.0.D0) GOTO 15
      IF(NEDEF.NE.0) L=0
      CALL DESTDEF(DUMMY,FSTEP*STEP(J),NP)
      IREP=0
      IJ=(J-1)*N
      DO 16 I=1,N
      AS(I)=A(I)
   16 AD(I)=U(IJ+I)
      FLAST=F
      IF(NFUN.EQ.0) GOTO 100
      GOTO 70
   18 IT=IT+1
      IF(IPR.EQ.0) WRITE(6,102) IT,TS,NFUN,F,(A(I),I=1,N)
      IF(IT.EQ.2) GOTO 10
C
C     (2)   CALCULATION OF HESSIAN - NEXT INDEX
C
   20 JP=0
   22 J =0
   24 J =J+1
      L =J+JP
      IF(L.GT.N) GOTO 28
      IF(STEP(J).EQ.0.0.OR.STEP(L).EQ.0.0) GOTO 24
   25 IJ=(J-1)*N
      IL=(L-1)*N
      DO 26 I=1,N
      AD(I)=STEP(J)*U(IJ+I)
      IF(J.NE.L) AD(I)=AD(I)+STEP(L)*U(IL+I)
   26 CONTINUE
      GOTO 80
   28 JP=JP+1
      IF(JP.LT.N) GOTO 22
   29 FCV=0.0
      IOPT=0
      RELAX=0.75
C
C     (3)   DIAGONALIZATION
C
   30 ND=N*N
      DO 34 ID=1,ND
      VM=0.0
      JL=0
      DO 32 JQ=1,N
      DO 32 LQ=1,JQ
      JL=JL+1
      IF(JQ.EQ.LQ) GOTO 32
      IF(ABS(V(JL)).LE.VM) GOTO 32
      VM=ABS(V(JL))
      J=JQ
      L=LQ
   32 CONTINUE
      IF(VM.LT.1.0E-10) GOTO 36
      IF(VM.LE.1.0E-4.AND.ID.GT.N) GOTO 36
      CALL DJACROT(V,U,N,J,L,CS,SN)
      FDJ  = CS*FD(J)+SN*FD(L)
      FD(L)=-SN*FD(J)+CS*FD(L)
      FD(J)= FDJ
   34 CONTINUE
   36 NEDEF=0
      FSTEP=MAX(0.2*FSTEP,REDC)
      II=0
      DO 38 I=1,N
      II=II+I
      IF(V(II).EQ.0.0) GOTO 38
      IF(V(II).LT.0.0) NEDEF=NEDEF+1
      DELTA=150.D0*EPS*MAX(ABS(F),EPS)/(ABS(V(II))*FSTEP*FSTEP)
      C=SQRT(DELTA)/ABS(STEP(I))
      IF(C.GT.10.D0) C=10.D0
      IF(C.LT.0.30D0) C=0.30D0
      STEP(I)=STEP(I)*C
   38 CONTINUE
      IF(NEDEF.EQ.0) GOTO 40
      NCN=2
      IF(IOPT.EQ.0) GOTO 39
      DO 139 I=1,N
  139 A(I)=AOPT(I)
      F=FOPT
      IOPT=0
   39 CONTINUE
      IF(NFUN.GT.NLIM) GOTO 90
      GOTO 10
C
C     (4)   NEWTON STEP
C
   40 IT=IT+1
      FCE=0.0
      BE=0.0
      BB=0.0
      II=0
      DO 42 I=1,N
      II=II+I
      DA(I)=0.0
      AD(I)=0.0
      IF(V(II).EQ.0.0) GOTO 42
      FCE=FCE+FD(I)*FD(I)/V(II)
      DA(I)=-0.381966*FD(I)/V(II)
      BE=BE+FD(I)*FD(I)
      BB=BB+FD(I)*V(II)*FD(I)
   42 CONTINUE
      BB=BE/BB
      IJ=-N
      DO 43 I=1,N
      DG(I)=-0.381966*BB*FD(I)+BE*BB*DA(I)/FCE
      IJ=IJ+N
      DO 43 J=1,N
   43 AD(J)=AD(J)+DA(I)*U(IJ+J)
      IREP=0
   44 J=0
      CALL DESTDEF(DUMMY,1.D0,12)
      IREP=0
      FLAST=F
      DO 41 I=1,N
   41 AS(I)=A(I)
      GOTO 70
   45 DO 46 I=1,N
   46 DA(I)=DL*DA(I)
      FCT=FLAST-F
      IF(IPR.EQ.0) WRITE(6,102) IT,TA,NFUN,F,(A(I),I=1,N)
C     WRITE(6,107) FCT,FCE,FCV
C 107 FORMAT(60X,3G10.3)
      IF(IOPT.EQ.0) GOTO 47
      IF(F.LE.FOPT) GOTO 47
      DO 147 I=1,N
  147 A(I)=AOPT(I)
      F=FOPT
      GOTO 10
   47 NCN=1
      IF(FCT+FCE.LT.0.0001)     GOTO 90
      IF(FCT.NE.0.0.AND.FCT.LT.0.0001.AND.FCE.LT.0.1) GOTO 90
      IF(FCT.EQ.0.0.AND.FCE.LT.0.10) GOTO 90
      NCN=2
   48 IF(NFUN.GT.NLIM) GOTO 90
      DO 49 I=1,N
      AOPT(I)=A(I)
      DA(I)=RELAX*DA(I)
      A(I)=A(I)-DL*(1.D0-RELAX)*AD(I)
   49 CONTINUE
      FOPT=F
      IOPT=1
      IF(RELAX.EQ.1.D0) GOTO 149
      RELAX=1.D0
      IREP=1
      GOTO 100
  149 IREP=0
      RELAX=0.75D0
C
C     (5)   CALCULATION OF GRADIENT - NEXT INDEX
C
   50 J =0
      JP=N
   52 J =J+1
      L =J
      IF(J.GT.N) GOTO 60
      IF(STEP(J).EQ.0.D0) GOTO 52
      IJ=(J-1)*N
      DO 54 I=1,N
   54 AD(I)=STEP(J)*U(IJ+I)
      GOTO 80
C
C     (6)   UPDATE MATRIX V
C
   60 SUMA=0.0
      SUMB=0.0
      II=0
      DO 62 I=1,N
      II=II+I
      SUMA=SUMA+DA(I)*(FD(I)-FDL(I))
      SUMB=SUMB+DA(I)*V(II)*DA(I)
   62 DA(I)=DA(I)*V(II)
      IF(SUMA*SUMB.NE.0.D0) GOTO 65
      IF(IOPT.EQ.0) GOTO 10

      DO 63 I=1,N
   63 A(I)=AOPT(I)
      F=FOPT
      IOPT=0
      GOTO 10

   65 CONTINUE
      FAC=MAX(1.D0,2.D0-FCT/FCE)
      FCV=0.0
      IJ=0
      DO 64 I=1,N
      DO 64 J=1,I
      IJ=IJ+1
      IF(I.EQ.J) VIJ=V(IJ)
      V(IJ)=V(IJ)+(FD(I)-FDL(I))*(FD(J)-FDL(J))/SUMA*FAC
     1     -DA(I)*DA(J)/SUMB*FAC
      IF(I.NE.J) GOTO 64
      FCV=FCV+ABS(V(IJ)-VIJ)/(EPS+ABS(V(IJ))+ABS(VIJ))/FLOAT(M)
   64 CONTINUE
      FCV=100.D0*FCV
      GOTO 30
C
C     MINIMUM SEARCH FOR (1) AND (4)
C
   70 IF(NFUN.EQ.1.AND.IPR.LE.1) WRITE(6,101) N,M,NLIM
      IF(NFUN.EQ.1.AND.IPR.LE.1) WRITE(6,102) IT,TB,NFUN,F,(A(I),I=1,N)
      IF(NFUN.EQ.1) FLAST=F
      IF(IREP.NE.0) GOTO 50
      CALL DESTMIN(F,D,IC)
      DO 72 I=1,N
      IF(NFUN.EQ.1) AS(I)=A(I)
   72 A(I)=A(I)+D*AD(I)
      IF(IC.EQ.0) GOTO 100
      CALL DESTEPL(SEC,DL,IB)
      IF(FLAST.NE.F) GOTO 75
      DO 73 I=1,N
   73 A(I)=AS(I)
   75 CONTINUE
      IF(J.EQ.0) GOTO 45
      IF(DL.LT.0.0) STEP(J)=-STEP(J)
      V((J*J+J)/2)=0.50*SEC
      C=10.0
      IF(SEC.LE.0.0) GOTO 74
      DELTA=300.0*EPS*MAX(ABS(F),EPS)/(ABS(SEC)*FSTEP*FSTEP)
      C=SQRT(DELTA)/ABS(STEP(J))
      IF(C.GT.10.0) C=10.0
      IF(C.LT.0.30) C=0.30
   74 STEP(J)=C*STEP(J)
      IF(L) 25,14,25
C
C     CALCULATION OF DERIVATIVE FOR (1), (2) AND (5)
C
   80 NF=0
      J=-J
   82 NF=NF+1
      FS(NF)=F
      D=FSTEP
      IF(J.NE.L) D=0.7071*D
      IF(NF.EQ.2) D=-D-D
      DO I = 1,N
         IF(NF.EQ.1) AS(I)=A(I)
         A(I)=A(I)+D*AD(I)
         IF(NF.EQ.3) A(I)=AS(I)
      ENDDO
      IF(NF.NE.3) GOTO 100
      F=FS(1)
      J=-J
      DF=FS(2)-FS(1)+FS(3)-FS(1)
      SDF=DF
      DM=150.0*EPS*MAX(F,EPS)
      SEC=DF/(D*D)
      IF(J.NE.L) GOTO 86
      IF(SEC.GT.0.0) V((J*J+J)/2)=0.50*SEC/(STEP(J)*STEP(J))
      FDL(J)=FD(J)
      DF=FS(2)-FS(3)
      FD (J)=0.25*DF/(D*STEP(J))
      C=SQRT(ABS(SDF)/DM)
      IF(C.LT.0.1) C=0.1
      IF(C.GT.3.0) C=3.0
      STEP(J)=STEP(J)/C
      IF(JP-N) 24,52,24
   86 IF(J.LE.L) JL=J+(L*L-L)/2
      IF(J.GT.L) JL=L+(J*J-J)/2
      V(JL)=0.5*(0.5*SEC/(STEP(J)*STEP(L))
     1     -V((J*J+J)/2)*STEP(J)/STEP(L)
     2     -V((L*L+L)/2)*STEP(L)/STEP(J))
      IF(JP.LT.N) GOTO 24
      CALL DJACROT(V,U,N,J,L,CS,SN)
      GOTO 14
C
C     CALCULATION OF COVARIANCE MATRIX
C
   90 NC=-1
      IF(IPR.LE.1) WRITE(6,102) IT,TB,NFUN,F
      IF(IPR.LE.1) WRITE(6,103) FCT,FCE,FCV
      TT=TB
      IF(NCN.EQ.2) TT=TN
      IF(IPR.LE.1) WRITE(6,104) TT
      II=0
      DO 92 I=1,N
      II=II+I
      AD(I)=0.0
      IF(V(II).NE.0.0) AD(I)=1.0/V(II)
   92 CONTINUE
      IJ=0
      DO 95 I=1,N
      DO 95 J=1,I
      IJ=IJ+1
      IK=I
      JK=J
      SUM=0.0
      DO 94 K=1,N
      SUM=SUM+U(IK)*AD(K)*U(JK)
      IK=IK+N
   94 JK=JK+N
      V(IJ)=SUM
   95 CONTINUE
      J=N+1
      IF(IPR.GE.2) GOTO 100
      WRITE(6,105)
      II=0
      DO 97 I=1,N
      IJ=II
      II=II+I
      ERR=0.D0
      IF(V(II).GT.0.0) ERR=SQRT(V(II))
      JJ=0
      DO 96 J=1,I
      JJ=JJ+J
      IJ=IJ+1
      DG(J)=0.D0
      PD   =V(II)*V(JJ)
      IF(PD.GT.0.D0) DG(J)=V(IJ)/SQRT(PD)
   96 CONTINUE
      WRITE(6,106) I,A(I),ERR,(DG(J),J=1,I)
   97 CONTINUE
      WRITE(6,106)
      J=N+1
      GOTO 100
C
C     LAST RETURN
C
   98 NC=NCN
C
C
  100 CONTINUE
      RETURN

  101 FORMAT('0-VALLEY-  STARTS ',I2,'(',I2,') PARAMETER FIT WITH MAX.',
     1       I4,' F-EVALS'/
     2       '  IT',8X,'EVALS',4X,'F-VALUE',10X,'PARAMETER-VALUES'/)
  102 FORMAT(1X,I3,2X,A4,I7,G15.5,5X,7G13.5/(36X,7G13.5))
  103 FORMAT('0FUNCTION CHANGE IN LAST IT WAS',G10.3,
     1       ' ,WHILE',G10.3,' WAS EXPECTED.'/
     2       ' LAST PC CHANGE OF SEC.DER. WAS',G10.3/)
  104 FORMAT('0  ',A4,'CONVERGENCE'/)
  105 FORMAT('0',6X,'PARAM',7X,'ERROR',7X,'CORRELATION COEFFICIENTS'/)
  106 FORMAT(1X,I2,G13.5,G12.4,10X,15F6.2/(38X,15F6.2))
      END

************************************************************************
      SUBROUTINE DESTMIN(F,E,NC)
      IMPLICIT DOUBLE PRECISION (A-H,O-Z)
C
C     SUBROUTINE ESTMIN SEARCHES FOR THE MINIMUM OF A FUNCTION OF ONE
C     PARAMETER X. THE METHOD OF GOLDEN SECTION IS USED WITH INCREASING
C     STEP SIZE, UNTIL THE LOWEST FUNCTION VALUE IS BETWEEN HIGHER ONES.
C     THEN AFTER ONE ADDITIONAL GOLDEN SECTION STEP A THIRD ORDER
C     POLYNOMIAL IS CONSTRUCTED THROUGH THE LAST FOUR POINTS, AND
C     A FINAL STEP IS TAKEN TO THE (APPROXIMATE) MINIMUM OF THE
C     THIRD ORDER POLYNOMIAL. FOR NFLIM = 3  A SIMPLIFIED METHOD IS
C     USED.
C
C     USAGE
C
C            X = START VALUE FOR PARAMETER
C            CALL DESTDEF(DUMMY,STEP,NFLIM)
C         10 F = FUNCTION OF X
C            CALL DESTMIN(F,D,NC)
C            X = X + D
C            IF(NC.LE.0) GOTO 10
C            CALL DESTEPL(SDER,DX,IB)
C
C     EXPLANATION
C     IN ESTDEF THE INITIAL STEP SIZE (STEP) AND THE MAXIMUM NUMBER
C     OF FUNCTION EVALUATIONS (NFLIM) HAS TO BE SPECIFIED, WITH
C     NFLIM = 3 . . . 20. THE ACTUAL NUMBER OF FUNCTION EVALUATIONS
C     MAY BE NFLIM+1.
C     THE ACTUAL FUNCTION VALUE FOR THE CUURENT VALUE OF X IS
C     TRANSMITTED TO ESTMIN, IN WHICH A NEW STEP D IS DETERMINED.
C     THIS STEP HAS TO BE ADDED TO X. IF NC IS LESS OR EQUAL TO ZERO,
C     THE FUNCTION HAS TO BE EVALUATED AGAIN. IF NC GREATER THAN ZERO,
C     F IS THE MINIMUM FUNCTION VALUE FOUND AND X WILL BE THE
C     CORRESPONDING PARAMETER VALUE. FINALLY THE FOLLOWING VALUES MAY BE
C     OBTAINED BY A CALL TO ESTEPL.
C
C         SDER = SEC. DERIVATIVE OF FUNCTION W.R.T. X
C         DX   = TOTAL STEP TAKEN FROM INITIAL TO FINAL X VALUE
C         IB   = 1 IF LOWEST FUNCTION VALUE BETWEEN HIGHER ONES
C                2 OTHERWISE
C
C
      REAL*8 FT(22),DFL,DFS
      INTEGER K,KLIM
      DATA    K/0/,KLIM/8/
      SAVE
      
      NC    = 0
      S     = 0.0
      K     = K+1
      FT(K) = F
      IF (K .NE. 1) GOTO 10

C     INITIALIZATION
      KMIN  = 1
      X     = 0.D0
      XN    = 1.D0
      D     = 1.D0
      T     = 0.5D0*(SQRT(5.D0)-1.D0)
      R     = 1.D0/T
      GOTO 90
C
   10 IF(R .LT. 1.D0) GOTO 50
C
      IF(FT(K) .LT. FT(KMIN)) GOTO 30

C     FAILURE
      D = -D
      IF(K.NE.2) GOTO 20
C     REVERSE ORDER AND ASSUME SUCCESS
      FT(2) = FT(1)
      FT(1) = F
      KMIN  = K
      XMIN  = X+D
      D     = R*D
      XN    = XMIN+D
      GOTO 90
C
   20 KFAIL=K
      R=T
      GOTO 40
C     SUCCESS
   30 KMIN=K
      XMIN=X
      IF(K.LT.KLIM) GOTO 40
      KBRACK=2
      S=-1.0
      IF(K.GE.4) GOTO 60
      S=2.0*(T*(FT(3)-FT(2))+FT(1)-FT(2))/(D*STEP)**2
      E=0.0
      NC=KBRACK
      GOTO 100
   40 D=R*D
      XN=X+D
      GOTO 90
C
   50 KBRACK=1
      IF(FT(K).GT.FT(KMIN)) GOTO 60
      KMIN=K
      XMIN=X
   60 IF(K.EQ.KFAIL+2) GOTO 80
C     INTERPOLATION
      FA=T*T*T
      FB=-0.5*(3.0*FA*FA-1.0)
      FC=-0.5*FA*(5.0*FA*FA-3.0)
      DFL=FT(K-1)+FT(K-3)
      DFS=FT(K)+FT(K-2)
      A0=0.5*(DFL*FB+DFS)/(1.0+FB)
      A2=0.5*(DFL-DFS)/(1.0+FB)
      IF(S.LT.0.0) GOTO 80
      DFL=FT(K-1)-FT(K-3)
      DFS=FT(K)-FT(K-2)
      A1=0.5*(DFL*FC+DFS)/(FA+FC)
      A3=0.5*(DFL*FA-DFS)/(FA+FC)
      XZ=0.0
      IF(ABS(A2).GT.1.E-20) XZ=-A1/(3.0*A2)
      DN=A3*(1.0-5.0*XZ*XZ)
      DD=2.0*(A2+5.0*XZ*A3)
      IF(ABS(DD).GT.1.E-20) XZ=XZ+DN/DD
      XN=X+0.5*T*D-XZ*(1.0+0.5*T)*D
      GOTO 90
C
C     FINAL RETURN
   80 F=FT(KMIN)
      XN=XMIN
      S=3.0*A2/((1.0+0.5*T)*D)**2
      S=S/(STEP*STEP)
      NC=KBRACK
      K=0
C
C     ADD CORRECTION D TO X
   90 E=(XN-X)*STEP
      X=XN
  100 RETURN
C
      ENTRY DESTDEF(F,E,NC)
      STEP=E
      KLIM=NC
      IF(KLIM.LT. 3) KLIM= 3
      IF(KLIM.GT.20) KLIM=20
      KBRACK=1
      K=0
      GOTO 100
C
      ENTRY DESTEPL(F,E,NC)
      F=S
      E=X
      NC=KBRACK
      GOTO 100
      END

************************************************************************
      SUBROUTINE DJACROT(V,U,N,JP,JQ,CS,SN)
      IMPLICIT DOUBLE PRECISION (A-H,O-Z)
C
C     SUBROUTINE JACROT PERFORMS ONE JACOBI ROTATION ON THE N*N
C     SYMMETRIC MATRIX V (STORED IN SYMMETRIC STORAGE MODE), TO ROTATE
C     THE JP,JQ ELEMENT TO ZERO. THE TRANSFORMATION MATRIX U (INITIALLY
C     A N*N UNIT MATRIX) IS CHANGED ACCORDINGLY. C AND S ARE THE
C     COSINE AND SINE OF THE ROTATION ANGLE.
C
C
C     USAGE
C                    - - - -- --
C        CALL DJACROT(V,U,N,JP,JQ,C,S)
C                    - -         - -
C
C     IF APPLIED TO ALWAYS THE LARGEST REMAINING OFFDIAGONAL ELEMENT
C     OF MATRIX V UNTIL THIS IS BELOW A CERTAIN LIMIT, V WILL CONTAIN
C     THE EIGENVALUES OF THE (ORIGINAL MATRIX) V IN THE DIAGONAL,
C     WHILE U WILL CONTAIN THE N (APPROXIMATE) EIGENVECTORS, THE FIRST
C     ONE BEING IN U(1) . . . U(N).
C
C                       T
C     FINAL MATRIX V = U  * (INITIAL MATRIX V) * U
C
      REAL*8 V(1)
      REAL*8 U(1),T,C,S,H,G
      SAVE
      
      C=1.D0
      S=0.D0
      IF(JQ.LT.JP) GOTO 10
      IP=JP
      IQ=JQ
      GOTO 20
   10 IP=JQ
      IQ=JP
   20 IPP=(IP*IP+IP)/2
      IQQ=(IQ*IQ+IQ)/2
      IPQ=IP+(IQ*IQ-IQ)/2
C     DETERMINE COS AND SIN FOR ROTATION
      IF(V(IPQ).EQ.0.0) GOTO 100
      TH=0.5*(V(IQQ)-V(IPP))/V(IPQ)
      T=1.0
      IF(TH.EQ.0.D0) GOTO 30
      T=1.D0/(TH+SIGN(SQRT(1.D0+TH*TH),TH))
   30 C=1.D0/DSQRT(1.0D0+T*T)
      S=C*T
C     TRANSFORMATION OF U
      LP=N*(IP-1)
      LQ=N*(IQ-1)
      DO 40 I=1,N
      LP=LP+1
      LQ=LQ+1
      H=C*U(LP)-S*U(LQ)
      U(LQ)=S*U(LP)+C*U(LQ)
   40 U(LP)=H
C     TRANSFORMATION OF V
      H=C*C*V(IPP)-2.0*C*S*V(IPQ)+S*S*V(IQQ)
      G=S*S*V(IPP)+2.0*C*S*V(IPQ)+C*C*V(IQQ)
C     V(IPQ)=C*S*(V(IPP)-V(IQQ))+(C*C-S*S)*V(IPQ)
      V(IPQ)=0.0
      V(IPP)=H
      V(IQQ)=G
      IJP=IPP-IP
      IJQ=IQQ-IQ
      DO 80 I=1,N
      IF(I.GT.IP) GOTO 50
      IJP=IJP+1
      IJQ=IJQ+1
      IF(I.EQ.IP) GOTO 80
      GOTO 70
   50 IJP=IJP+I-1
      IF(I.GT.IQ) GOTO 60
      IJQ=IJQ+1
      IF(I.EQ.IQ) GOTO 80
      GOTO 70
   60 IJQ=IJQ+I-1
   70 H=C*V(IJP)-S*V(IJQ)
      V(IJQ)=S*V(IJP)+C*V(IJQ)
      V(IJP)=H
   80 CONTINUE
  100 CS=C
      SN=S
      RETURN
      END
