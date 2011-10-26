* $Id: TrFitF.F,v 1.2 2011/10/26 14:00:13 choutko Exp $
      SUBROUTINE DMM55(A,B,C)
C *** ---------------------------------------------                     05/07/81
C *** MULTIPLICATION OF 5X5 MATRICES: A*B -> C.
C *** FOR:I,J=1,5   C(I,J)=SUM:K=1,5(A(I,K)B(K,J))
C *** NB: B AND C MAY WELL BE THE SAME MATRIX
C
C        Author: A. Haas (Freiburg University)
C
C *** ---------------------------------------------
      DOUBLE PRECISION A,B,C,B1J,B2J,B3J,B4J,B5J
      DIMENSION A(25),B(25),C(25)
      DO 10 J5=5,25,5
      B5J=B(J5)
      J4=J5-1
      B4J=B(J4)
      J3=J4-1
      B3J=B(J3)
      J2=J3-1
      B2J=B(J2)
      J1=J2-1
      B1J=B(J1)
      C(J1)=A( 1)*B1J+A( 6)*B2J+A(11)*B3J+A(16)*B4J+A(21)*B5J
      C(J2)=A( 2)*B1J+A( 7)*B2J+A(12)*B3J+A(17)*B4J+A(22)*B5J
      C(J3)=A( 3)*B1J+A( 8)*B2J+A(13)*B3J+A(18)*B4J+A(23)*B5J
      C(J4)=A( 4)*B1J+A( 9)*B2J+A(14)*B3J+A(19)*B4J+A(24)*B5J
   10 C(J5)=A( 5)*B1J+A(10)*B2J+A(15)*B3J+A(20)*B4J+A(25)*B5J
*
      END
CDECK  ID>, DNORM.  
*CMZ :  3.21/02 29/03/94  15.41.50  by  S.Giani
*-- Author :
      SUBROUTINE DNORM(X,AX)
*     ======================
*
*     Double Precision Vector Normalization
*
*     Author: V.Innocente (Naples)
*
      IMPLICIT DOUBLE PRECISION ( A-H, O-Z )
      DIMENSION X(3)
*
      AX = SQRT(X(1)**2+X(2)**2+X(3)**2)
      AX1 = 1./AX
      X(1) = AX1*X(1)
      X(2) = AX1*X(2)
      X(3) = AX1*X(3)
*
      END
CDECK  ID>, DSMT5T. 
*CMZ :  3.21/02 29/03/94  15.41.50  by  S.Giani
*-- Author :
      SUBROUTINE DSMT5T(A,S,R)
C *** ------------------------------------------------------            05/07/81
C *** TRANSFORMATION OF SYMMETRIC 5X5 MATRIX S: A*S*AT -> R.
C *** A IS A GENERAL 5X5 MATRIX WHEREAS S,R ARE SYMMETRIC
C *** MATRICES STORED IN TRIANGULAR FORM. FOR:I,J=1,5
C *** R(I,J) = SUM:K=1,5(SUM:L=1,5(A(I,L)*S(L,K)*A(J,K))).
C *** NB: S AND R MAY WELL BE THE SAME MATRIX.
C
C        Author: A. Haas (Freiburg University)
C
C *** ------------------------------------------------------
      DOUBLE PRECISION A,S,R,Q,T1,T2,T3,T4,T5
      DIMENSION A(25),S(15),R(15),Q(15)
      DO 5 I=1,15
         Q(I)=S(I)
    5 CONTINUE
      K=1
      DO 15 J=1,5
         T1=A(J   )
         T2=A(J+ 5)
         T3=A(J+10)
         T4=A(J+15)
         T5=A(J+20)
         DO 10 I=J,5
            R(K)=A(I   )*(Q(1)*T1+Q(2)*T2+Q( 3)*T3+Q( 4)*T4+Q( 5)*T5)
     +          +A(I+ 5)*(Q(2)*T1+Q(6)*T2+Q( 7)*T3+Q( 8)*T4+Q( 9)*T5)
     +          +A(I+10)*(Q(3)*T1+Q(7)*T2+Q(10)*T3+Q(11)*T4+Q(12)*T5)
     +          +A(I+15)*(Q(4)*T1+Q(8)*T2+Q(11)*T3+Q(13)*T4+Q(14)*T5)
     +          +A(I+20)*(Q(5)*T1+Q(9)*T2+Q(12)*T3+Q(14)*T4+Q(15)*T5)
            K=K+1
   10    CONTINUE
   15 CONTINUE
*
      END
CDECK  ID>, RMM55.  
*CMZ :  3.21/02 29/03/94  15.41.50  by  S.Giani
*-- Author :
      SUBROUTINE RMM55(A,B,C)
C *** ---------------------------------------------                     05/07/81
C *** MULTIPLICATION OF 5X5 MATRICES: A*B -> C.
C *** FOR:I,J=1,5   C(I,J)=SUM:K=1,5(A(I,K)B(K,J))
C *** NB: B AND C MAY WELL BE THE SAME MATRIX
C
C        Author: A. Haas (Freiburg University)
C
C *** ---------------------------------------------
      DIMENSION A(25),B(25),C(25)
      DO 10 J5=5,25,5
         B5J=B(J5)
         J4=J5-1
         B4J=B(J4)
         J3=J4-1
         B3J=B(J3)
         J2=J3-1
         B2J=B(J2)
         J1=J2-1
         B1J=B(J1)
         C(J1)=A( 1)*B1J+A( 6)*B2J+A(11)*B3J+A(16)*B4J+A(21)*B5J
         C(J2)=A( 2)*B1J+A( 7)*B2J+A(12)*B3J+A(17)*B4J+A(22)*B5J
         C(J3)=A( 3)*B1J+A( 8)*B2J+A(13)*B3J+A(18)*B4J+A(23)*B5J
         C(J4)=A( 4)*B1J+A( 9)*B2J+A(14)*B3J+A(19)*B4J+A(24)*B5J
         C(J5)=A( 5)*B1J+A(10)*B2J+A(15)*B3J+A(20)*B4J+A(25)*B5J
   10 CONTINUE
*
      END
CDECK  ID>, RSMT5T. 
*CMZ :  3.21/02 29/03/94  15.41.50  by  S.Giani
*-- Author :
      SUBROUTINE RSMT5T(A,S,R)
C *** ------------------------------------------------------            05/07/81
C *** TRANSFORMATION OF SYMMETRIC 5X5 MATRIX S: A*S*AT -> R.
C *** A IS A GENERAL 5X5 MATRIX WHEREAS S,R ARE SYMMETRIC
C *** MATRICES STORED IN TRIANGULAR FORM. FOR:I,J=1,5
C *** R(I,J) = SUM:K=1,5(SUM:L=1,5(A(I,L)*S(L,K)*A(J,K))).
C *** NB: S AND R MAY WELL BE THE SAME MATRIX.
C
C        Author: A. Haas (Freiburg University)
C
C *** ------------------------------------------------------
      DIMENSION A(25),S(15),R(15),Q(15)
      DO 5 I=1,15
         Q(I)=S(I)
    5 CONTINUE
      K=1
      DO 15 J=1,5
         T1=A(J   )
         T2=A(J+ 5)
         T3=A(J+10)
         T4=A(J+15)
         T5=A(J+20)
         DO 10 I=J,5
            R(K)=A(I   )*(Q(1)*T1+Q(2)*T2+Q( 3)*T3+Q( 4)*T4+Q( 5)*T5)
     +          +A(I+ 5)*(Q(2)*T1+Q(6)*T2+Q( 7)*T3+Q( 8)*T4+Q( 9)*T5)
     +          +A(I+10)*(Q(3)*T1+Q(7)*T2+Q(10)*T3+Q(11)*T4+Q(12)*T5)
     +          +A(I+15)*(Q(4)*T1+Q(8)*T2+Q(11)*T3+Q(13)*T4+Q(14)*T5)
     +          +A(I+20)*(Q(5)*T1+Q(9)*T2+Q(12)*T3+Q(14)*T4+Q(15)*T5)
            K=K+1
   10    CONTINUE
   15 CONTINUE
*
      END
CDECK  ID>, SSMT5T. 
*CMZ :  3.21/02 29/03/94  15.41.50  by  S.Giani
*-- Author :
      SUBROUTINE SSMT5T(A,S,R)
C *** ------------------------------------------------------            05/07/81
C *** TRANSFORMATION OF SYMMETRIC 5X5 MATRIX S: A*S*AT -> R.
C *** A IS A GENERAL 5X5 MATRIX WHEREAS S,R ARE SYMMETRIC
C *** MATRICES STORED IN TRIANGULAR FORM. FOR:I,J=1,5
C *** R(I,J) = SUM:K=1,5(SUM:L=1,5(A(I,L)*S(L,K)*A(J,K))).
C *** NB: S AND R MAY WELL BE THE SAME MATRIX.
C
C        Author: A. Haas (Freiburg University)
C
C *** ------------------------------------------------------
      DOUBLE PRECISION A,S,R,Q,T1,T2,T3,T4,T5
      DIMENSION A(25),S(15),R(15),Q(15)
      DO 5 I=1,15
         Q(I)=S(I)
    5 CONTINUE
      K=1
      DO 15 J=1,5
         T1=A(J   )
         T2=A(J+ 5)
         T3=A(J+10)
         T4=A(J+15)
         T5=A(J+20)
         DO 10 I=J,5
            R(K)=A(I   )*(Q(1)*T1+Q(2)*T2+Q( 3)*T3+Q( 4)*T4+Q( 5)*T5)
     +          +A(I+ 5)*(Q(2)*T1+Q(6)*T2+Q( 7)*T3+Q( 8)*T4+Q( 9)*T5)
     +          +A(I+10)*(Q(3)*T1+Q(7)*T2+Q(10)*T3+Q(11)*T4+Q(12)*T5)
     +          +A(I+15)*(Q(4)*T1+Q(8)*T2+Q(11)*T3+Q(13)*T4+Q(14)*T5)
     +          +A(I+20)*(Q(5)*T1+Q(9)*T2+Q(12)*T3+Q(14)*T4+Q(15)*T5)
            K=K+1
   10    CONTINUE
   15 CONTINUE
*
      END
CDECK  ID>, XMM55.  
*CMZ :  3.21/02 29/03/94  15.41.50  by  S.Giani
*-- Author :
      SUBROUTINE XMM55(A,B,C)
C *** ---------------------------------------------                     05/07/81
C *** MULTIPLICATION OF 5X5 MATRICES: A*B -> C.
C *** FOR:I,J=1,5   C(I,J)=SUM:K=1,5(A(I,K)B(K,J))
C *** NB: B AND C MAY WELL BE THE SAME MATRIX
C
C        Author: A. Haas (Freiburg University)
C
C *** ---------------------------------------------
      DOUBLE PRECISION A,B,C,B1J,B2J,B3J,B4J,B5J
      DIMENSION A(25),B(25),C(25)
      DO 10 J5=5,25,5
         B5J=B(J5)
         J4=J5-1
         B4J=B(J4)
         J3=J4-1
         B3J=B(J3)
         J2=J3-1
         B2J=B(J2)
         J1=J2-1
         B1J=B(J1)
         C(J1)=A( 1)*B1J+A( 6)*B2J+A(11)*B3J+A(16)*B4J+A(21)*B5J
         C(J2)=A( 2)*B1J+A( 7)*B2J+A(12)*B3J+A(17)*B4J+A(22)*B5J
         C(J3)=A( 3)*B1J+A( 8)*B2J+A(13)*B3J+A(18)*B4J+A(23)*B5J
         C(J4)=A( 4)*B1J+A( 9)*B2J+A(14)*B3J+A(19)*B4J+A(24)*B5J
         C(J5)=A( 5)*B1J+A(10)*B2J+A(15)*B3J+A(20)*B4J+A(25)*B5J
   10 CONTINUE
*
      END
CDECK  ID>, DSBEQN. 
*CMZ :  3.21/02 29/03/94  15.41.50  by  S.Giani
*-- Author :
          SUBROUTINE          DSBEQN(N,M,A,IDIM,IFAIL,K,B)
C
C     ******************************************************************
C
C     SOLVES A BANDED SYSTEM OF LINEAR EQUATIONS USING CHOLESKY
C     DECOMPOSITION
C
C     N       ORDER OF THE BAND MATRIX.
C
C     M       BAND PARAMETER. NON-ZERO COEFFICIENTS ARE CONFINED TO
C             IABS(I-J).LE.M.
C
C     A       (DOUBLE PRECISION) TWO-DIMENSIONAL ARRAY CONTAINING THE
C             SUCCESSIVE ROWS OF THE BAND MATRIX
C
C     IDIM    FIRST DIMENSION PARAMETER OF ARRAYS ABAND AND B.
C
C     IFAIL   OUTPUT PARAMETER.   IFAIL= 0 ... NORMAL EXIT.
C                                 IFAIL=-1 ... SINGULAR MATRIX.
C
C     K       NUMBER OF COLUMNS OF THE MATRIX IN ARRAY B.
C
C     B       (DOUBLE PRECISION) TWO-DIMENSIONAL ARRAY CONTAINING A
C             MATRIX OF RIGHT-HAND SIDES.
C
C     THIS SUBROUTINE REPLACES B BY THE SOLUTION MATRIX X OF A*X=B,
C     WHERE A IS THE BAND-MATRIX
C
C
C     Author. V.Innocente  -  November 1988
C
C     ORIGIN    KERNLIB F102
C
C     ref. J.H.Wilkinson & C.Reinsch
C          Linear Algebra - Springer-Verlag 1971
C          pag. 9 and 50
C
C
C     ******************************************************************
C
          DOUBLE PRECISION    A(IDIM,n), B(IDIM,1),  ONE,  X, Y
          REAL                PIVOTF
          DOUBLE PRECISION    S1, S21, S22,       DOTF
          CHARACTER*6         HNAME
          DATA      HNAME               /  'DSBEQN'  /
          PIVOTF(X)    =  SNGL(X)
          DOTF(X,Y,S1)  =  X * Y + S1
          DATA      ONE                 /  1.D0  /
          IF(IDIM .LT. N  .OR.  N .LE. 0  .OR.  K .LT. 0)  GOTO 900
          IFAIL  =  0
          DO 144    J  =  1, N
             IF(A(J,J) .LE. 0.)  GOTO 150
             A(J,J)  =  ONE / A(J,J)
             IF(J .EQ. N)  GOTO 199
 140         JP1  =  J+1
             JPM = MIN(J+M,N)
             JP1MM =MAX(1,JP1-M)
             DO 143   L  =  JP1, JPM
                A(J,L)  =  A(J,J)*A(L,J)
                S1      =  -A(L,JP1)
                IM = MAX(JP1MM,L-M)
                DO 141  I  =  IM, J
                   S1  =  DOTF(A(L,I),A(I,J+1),S1)
 141               CONTINUE
                A(L,JP1)  =  -S1
 143            CONTINUE
 144         CONTINUE
 150      IFAIL  =  -1
          RETURN
 199      CONTINUE
          IF(K .LE. 0)  GOTO 299
          DO 220    L  =  1, K
             B(1,L)  =  A(1,1)*B(1,L)
 220         CONTINUE
          IF(N .EQ. 1)  GOTO 299
          DO 243    L  =  1, K
             DO 232   I  =  2, N
                IM1  =  I-1
                IMM  =  MAX(1,I-M)
                S21  =  - B(I,L)
                DO 231   J  =  IMM, IM1
                   S21  =  DOTF(A(I,J),B(J,L),S21)
 231               CONTINUE
                B(I,L)  =  - A(I,I)*S21
 232            CONTINUE
             NM1  =  N-1
             DO 242   I  =  NM1,1,-1
                S22  =  - B(I,L)
                IP1 = I+1
                IPM = MIN(N,I+M)
                DO 241   J  =  IP1, IPM
                   S22    =  DOTF(A(I,J),B(J,L),S22)
 241               CONTINUE
                B(I,L)  =  - S22
 242            CONTINUE
 243         CONTINUE
 299      CONTINUE
          RETURN
 900      CALL TMPRNT(HNAME,N,IDIM,K)
          RETURN
          END
CDECK  ID>, DSBFINV.
*CMZ :  3.21/02 29/03/94  15.41.50  by  S.Giani
*-- Author :
          SUBROUTINE          DSBFINV(N,M,A,IDIM,LB,B,LST)
C
C     ******************************************************************
C
C     INVERT A BANDED MATRIX USING CHOLESKY DECOMPOSITION
C     ROUTINE DSBEQN OR SIMILAR SHOULD BE CALLED FIRST
C     THE ELEMENT OF THE INVERTED MATRIX TO BE COMPIUTED ARE LISTED
C     IN LST AND STORED IN B
C
C     N       ORDER OF THE BAND MATRIX.
C
C     M       BAND PARAMETER. NON-ZERO COEFFICIENTS ARE CONFINED TO
C             IABS(I-J).LE.M.
C
C     A       (DOUBLE PRECISION) TWO-DIMENSIONAL ARRAY CONTAINING THE
C             SUCCESSIVE ROWS OF THE CHOLESKY DECOMPOSED BAND MATRIX
C
C     IDIM    FIRST DIMENSION PARAMETER OF ARRAYS ABAND AND B.
C
C     LB       NUMBER OF ELEMENTS TO BE INVERTED
C
C     B       (DOUBLE PRECISION) ARRAY CONTAINING IN EXIT THE INVERTED
C             ELEMENTS
C
C     LST     TWO-DIMENTIONAL ARRAY CONTEINING THE INDECES OF THE
C             ELEMENT OF THE MATRIX TO BE INVERTED
C
C                                                            -1
C     THIS SUBROUTINE REPLACES B(J)  BY  A(LST(1,J),LST(2,J))
C     WHERE A IS THE BAND-MATRIX
C
C
C     Author. V.Innocente  -  November 1988
C
C     ORIGIN    KERNLIB F102
C
C     ref. J.H.Wilkinson & C.Reinsch
C          Linear Algebra - Springer-Verlag 1971
C          pag. 9 and 50
C
C
C     ******************************************************************
C
          DOUBLE PRECISION    A(IDIM,N),ZERO,     X, Y
          DOUBLE PRECISION    B(*)
          DOUBLE PRECISION    S31, S32, S33, DOTF
          INTEGER             LST(2,1)
          CHARACTER*7         HNAME
          DOTF(X,Y,S31)  =  X*Y + S31
          DATA      HNAME               /  'DSBFINV'  /
          DATA      ZERO      /  0.D0  /
          IF(IDIM .LT. N  .OR.  N .LE. 0)  GOTO 900
          IF(N .EQ. 1)  GOTO 399
          A(1,2)  =  -A(1,2)
          A(2,1)  =   A(1,2)*A(2,2)
          IF(N .EQ. 2)  GOTO 320
          DO 314    J  =  3, N
             JM2  =  J - 2
             JMM = MAX(1,J-1-M)
             DO 312 K  =  1, JM2
                S31  =  A(K,J)
                IM = MAX(K,JMM)
                DO 311  I  =  IM, JM2
                   S31  =  DOTF(A(K,I+1),A(I+1,J),S31)
 311               CONTINUE
                A(K,J)  =  -S31
                A(J,K)  =  -S31*A(J,J)
 312            CONTINUE
             A(J-1,J)  =  -A(J-1,J)
             A(J,J-1)  =   A(J-1,J)*A(J,J)
 314         CONTINUE
*
 320         CONTINUE
             IF ( LB.LE.0 ) GO TO 999
             DO 329 L=1,LB
               K  =  LST(1,L)
               J  =  LST(2,L)
               IF (J.EQ.K)  THEN
                 S33  =  A(J,J)
                 IF(J .EQ. N)  GOTO 325
                 JP1  =  J + 1
                 DO 324 I  =  JP1, N
                   S33  =  DOTF(A(J,I),A(I,J),S33)
 324            CONTINUE
 325            B(L)  =  S33
               ELSE
                 IF ( K.GT.J ) THEN
                  K  =  LST(2,L)
                  J  =  LST(1,L)
                 ENDIF
                 S32  =  ZERO
                 DO 327  I  =  J, N
                   S32  =  DOTF(A(K,I),A(I,J),S32)
 327             CONTINUE
                 B(L)  =  S32
               ENDIF
 329         CONTINUE
 399      CONTINUE
          RETURN
 900      CALL TMPRNT(HNAME,N,IDIM,0)
 999      CONTINUE
          RETURN
          END
      function xsign(rr)
       if(rr.gt.0.)then
        xsign=1.
       else
        xsign=-1.
       endif
      end

CDECK  ID>, TKFINI. 
          subroutine tkfini
              common /hvsec/ihvse(2)
c              data ihvse/2,2/

              common /tkgdefC/tmaxf,smax,fieldmm,cconv,
     +        maxcal,nitm,xtol,iver,ialg,steps,npars,ims,
     +        bound,scale,cfld,sms,pii
              real *4 tmaxf,smax,fieldmm,bound(2,6),cfld,pii,cconv,
     +        scale(6)
              common/tftimeC/timer(100,2),xtime
              common /tkcorrC/
     +        xxn(2),vpar2(2,2),vpar(2,2),vxy(2)
              real *8 xxn,vpar2,vpar,vxy
              save ini
           data ini /0/
         
       !$OMP THREADPRIVATE (/tkgdefC/,ini)
           pii=3.1415927
           cfld=3333.*pii/180./(fieldmm+1.e-10)*tmaxf
           bound(1,1)=-100. ! x
           bound(2,1)= 100. ! x
           bound(1,2)=-1.   ! nx/nz
           bound(2,2)= 1.   ! nx/nz
           bound(1,3)=-100. ! y
           bound(2,3)= 100. ! y
           bound(1,4)=-1.   ! ny/nz
           bound(2,4)= 1.   ! ny/nz
           bound(1,5)=-10.  ! q/p
           bound(2,5)= 10.  ! q/p
           do i=1,5
            scale(i)=1.
           enddo
           if(ini.ne.0)goto 999
           pmini=0.5
           ims=1
           xls=300.e-4
           rls=9.36
           sms=13.6e-3*sqrt(xls/rls)
           steps=0.03
           ialg=1
           iver=0
           ini=1
           nitm=1000
           maxcal=20
           smax=50.
           fieldmm=20.
           npars=5
           tmaxf=0.2
           pii=3.1415927
           cfld=3333.*pii/180./(fieldmm+1.e-10)*tmaxf
           cconv=5.e-4
           xtol=1.e-2
999          end
CDECK  ID>, TKFITG. 
         subroutine tkgetres(out,nplan)
*
*          
*
       !$OMP THREADPRIVATE (/tkfrunC/)
              parameter (maxpl=20)
              common /tkfrunC/npoint,xp(3,maxpl),ys(3,maxpl),
     +        yn(3,maxpl),xmass,chrg,outa(7,maxpl)
             real *4 out(7,*)
             call ucopy(outa,out,7*nplan)
         end    
         subroutine tksetres(xh,nplan)
*
*          
*
       !$OMP THREADPRIVATE (/tkfrunC/)
              parameter (maxpl=20)
              common /tkfrunC/npoint,xp(3,maxpl),ys(3,maxpl),
     +        yn(3,maxpl),xmass,chrg,outa(7,maxpl)
             real *4 xh(3,*)
              
              do i=1,nplan
                 do k=1,3
                   outa(k,i)=xh(k,i)
                 enddo 
              enddo
         end    
         subroutine tksetresd(xh,nplan)
*
*          
*
       !$OMP THREADPRIVATE (/tkfrunC/)
              parameter (maxpl=20)
              common /tkfrunC/npoint,xp(3,maxpl),ys(3,maxpl),
     +        yn(3,maxpl),xmass,chrg,outa(7,maxpl)
             real *4 xh(3,*)
              
              do i=1,nplan
                 do k=4,6
                   outa(k,i)=xh(k,i)
                 enddo 
              enddo
         end    
         subroutine tkfitg(npt,xhits,ysigma,ynormal,ipart,
     +   ialgo,imsa,layer,out)
*
*
**
* Input
*          npt            ! number of planes
*          xhits(3,npt)   ! measured coordinates
*          ysigma(3,npt)  ! errors
*          ynormal(3,npt) ! normales to planes  (0,0,-1. for AMS)
*          ialgo
*               =1     V. Choutko  fit (5X5 matrix inv, mscat included)
*               =2     GEANE banded matrix inv
*               =3     GEANE Kalman filter
*               =4     Juan algo
*               =5     A. Chikanian algo 
*
* Output   out(9) ! x0,y0,z0,theta,phi,rig,chi2,ier, err(1/rig)
*
*
* Multiple scattering (default on) can be switched off in all 3 algorithmes
* by datacard TKFIT 12=0
*
              integer layer(1)
       !$OMP THREADPRIVATE (/tkgdefC/,/tftimeC/,/tkcorrC/,/tkfrunC/)
              common /tkgdefC/tmaxf,smax,fieldmm,cconv,
     +        maxcal,nitm,xtol,iver,ialg,steps,npars,ims,
     +        bound,scale,cfld,sms,pii
              real *4 tmaxf,smax,fieldmm,bound(2,6),cfld,pii,cconv,
     +        scale(6)
              common/tftimeC/timer(100,2),xtime
              common /tkcorrC/
     +        xxn(2),vpar2(2,2),vpar(2,2),vxy(2)
              real *8 xxn,vpar2,vpar,vxy
              parameter (maxpl=20)
              common /tkfrunC/npoint,xp(3,maxpl),ys(3,maxpl),
     +        yn(3,maxpl),xmass,chrg,outa(7,maxpl)
         integer npt
         real xhits(3,*),ysigma(3,*),ynormal(3,*),pinit
         real out(*)
           real *8 x(6),f,w1(6),w2(6),w3(6),w4(6),w5(7),fdum
           real *8 tol
           real *4 nx,ny,nz
           character *20 cnn
*
*          if initial ims ==0 keep it switched off
*
           integer imsinit/0/,imskeep/0/,ialgkeep/0/
           if(imsinit.eq.0)then
               imsinit=1
               imskeep=ims
               ialgkeep=ialg
           endif
c            call tgfpart(ipart,cnn,itrt7,xmass,chrg,tlt7,uwb,nwb)
             xmass=0.938
             chrg=1
*
*            new 2 lines 
*
             truecharge=chrg
             xmz=xmass/abs(chrg)
             if(chrg.lt.0)chrg=-chrg
             if(imskeep.ne.0)then
              ims=imsa
             endif
             if(ialgkeep/10.ne.0)then
               if(ialgo.eq.1)ialgo=11
             endif
             call tkfini
             ialg=mod(ialgo,10)
             npoint=npt
             do i=1,npt
              do k=1,3
               xp(k,i)=xhits(k,i)
               ys(k,i)=ysigma(k,i)
               yn(k,i)=ynormal(k,i)
              enddo
             enddo
             r=(xp(1,2)-xp(1,1))**2+
     +         (xp(2,2)-xp(2,1))**2+
     +         (xp(3,2)-xp(3,1))**2
             r2=(xp(1,npt)-xp(1,npt-1))**2+
     +         (xp(2,npt)-xp(2,npt-1))**2+
     +         (xp(3,npt)-xp(3,npt-1))**2
             r=sqrt(r)
             r2=sqrt(r2)
             nx=(xp(1,2)-xp(1,1))/r
             ny=(xp(2,2)-xp(2,1))/r
             nz=(xp(3,2)-xp(3,1))/r
             cx=nx*(xp(1,npt)-xp(1,npt-1))/r2+
     +       ny*(xp(2,npt)-xp(2,npt-1))/r2+
     +       nz*(xp(3,npt)-xp(3,npt-1))/r2
             dt=aycos(cx)
             if(dt.ne.0)then
                 call getfield(bb)
             pinit=-(xhits(3,npt)-xhits(3,1))*chrg*abs(bb)*10
     +       *2.99e-4/dt
c              pinit=abs(pinit)
             else
             pinit=1000
             endif
             pinit=10000
             if(ialgo/10.eq.1)pinit=1/out(1)*chrg
             x(1)=xp(1,1)
             x(2)=nx/nz
             x(3)=xp(2,1)
             x(4)=ny/nz
             if(pinit.ne.0)then
             x(5)=chrg/pinit
             else
             x(5)=1.e6
             endif
             if(ialg.eq.1)then
              n=npars
              tol=xtol
              ifail=0
c              write(*,*)' calling tkhmd'
              call tkhmd(1+(ialgo/10)*10,n,x,f,tol,maxcal,ifail,er)
c              write(*,*)ims,f,ifail
              if(ifail.eq.0)then
*
* Smoothing
*
c               call tkhmd(2,n,x,f,tol,2,ifail1,er1)

                x(5)=x(5)*chrg/abs(chrg)
              endif
             else if(ialg.eq.2.or.ialg.eq.3)then
c             call tkfitge(npt,xhits,ysigma,ynormal,truecharge,
c     +       ipart,out,ialg-1)
             goto 999
             else if(ialg.eq.4)then
              call TKFITA(NPt, Xhits, ysigma, layer,
     +        IMSa, xmz, OUT, chi2, ndof, IERR)
              call tksetres(xhits,npt)
              do k=1,npt
              ysigma(1,k)=out(3)
              ysigma(2,k)=out(4)
              ysigma(3,k)=-sqrt(1-out(3)**2-out(4)**2)
              enddo
              call tksetresd(ysigma,npt) 
              theta=aycos(-sqrt(1-out(3)**2-out(4)**2))
              phi=atan2(out(4),out(3))
              z0=xhits(3,1)
              rig=10000000
              if(out(5).ne.0)rig=1/out(5)
              erig=out(6)
              out(3)=z0
              out(4)=theta
              out(5)=phi
              out(6)=rig
              out(7)=chi2
              out(8)=ierr
              out(9)=erig
              goto 999
             else if(ialg.eq.5)then
              call rkms_rig(npt,layer,xhits,ysigma,ipart,pinit,out)
              call tksetres(xhits,npt)
              call tksetresd(ysigma,npt)
              out(8)=0
              goto 999
           else
            ifail=1
           endif
              out(1)=x(1)
              out(2)=x(3)
              out(3)=xp(3,1)
              out(5)=atan2(-x(4),-x(2))
              cx=-1.d0/sqrt(1.d0+x(2)**2+x(4)**2)
              out(4)=aycos(cx)
              if(x(5).ne.0)then
              out(6)=1./x(5)
              else
              out(6)=1.e10
              endif
              out(7)=f
              out(8)=ifail
              out(9)=er
999             continue
         end
          subroutine tkhmd(icase,n,xc,fc,tol,mxcal,ifail,er)
#      include "mfield.h"
       !$OMP THREADPRIVATE (/tkgdefC/,/tftimeC/,/tkcorrC/,/tkfrunC/)
       !$OMP THREADPRIVATE (/tkclearC/)
          common /tkclearC/clear,der
          integer clear

              common /tkgdefC/tmaxf,smax,fieldmm,cconv,
     +        maxcal,nitm,xtol,iver,ialg,steps,npars,ims,
     +        bound,scale,cfld,sms,pii
              real *4 tmaxf,smax,fieldmm,bound(2,6),cfld,pii,cconv,
     +        scale(6)
              common/tftimeC/timer(100,2),xtime
              common /tkcorrC/
     +        xxn(2),vpar2(2,2),vpar(2,2),vxy(2)
              real *8 xxn,vpar2,vpar,vxy
              parameter (maxpl=20)
              common /tkfrunC/npoint,xp(3,maxpl),ys(3,maxpl),
     +        yn(3,maxpl),xmass,chrg,outa(7,maxpl)
          real *8 xc(6),fc,tol,dlt
          real *8 xcinit
          real *4 point(6),outa
          real *4 init(7),out(7)
          real* 8 m55(5,5,maxpl),sol(5),g(5),gg(5,5),
     +    icc(2,2,maxpl) ,aa(maxpl),bb(maxpl),fcb
          real *4 fact(maxpl),xmsr(maxpl,maxpl),xms(maxpl),fck(maxpl)
          ifail=0
          xcinit=xc(5)

*
* Load starting Values
*
            if(npoint.gt.maxpl)then
              write(*,*)'TKHMD  - too many planes ',npoint,maxpl
              stop 'abort'
            else if(2*npoint-5.le.0)then
              write(*,*)'TKHMD  - too small number of planes ',
     +       npoint
            ifail=1
            goto 999
            endif
           if(chrg.eq.0)then
            ifail=777
            write(*,*)' Charge 0!!!!'
            goto 999
           endif
           call vzero(m55(1,1,1),50)
           do i=1,5
            m55(i,i,1)=1
           enddo
           fcb=0
           do kiter=1,mxcal
            init(1)=xc(1)
            init(2)=xc(3)
            init(3)=xp(3,1)+1.d-4
            dnorm=1./sqrt(1.d0+xc(2)**2+xc(4)**2)
            init(4)=-xc(2)*dnorm
            init(5)=-xc(4)*dnorm
            init(6)=-dnorm
            if(xc(5).ne.0)then
             if(icase/10.eq.0)then
                init(7)=1./xc(5)*chrg
             else
                init(7)=1./xcinit*chrg
                xc(5)=xcinit
             endif
            else
             init(7)=1.e10
            endif
*
*
*       plane loop
*
            do i=1,npoint
             if(i.le.2)then
              clear=1
             else
              clear=2
             endif
             do k=1,3
              point(k)=xp(k,i)
              point(k+3)=yn(k,i)
             enddo
             if(i.gt.1)then
              call tkfitpar(init(1),chrg,point,out,m55(1,1,i),
     +        stept)
             else
              call ucopy(init(1),out(1),7)
             endif
             if(i.ne.npoint)
     +       call ucopy(m55(1,1,i),m55(1,1,i+1),50)
             if(ims.eq.0.or.i.le.2)then
              fact(i)=0
             else
              beta=max(abs(init(7)/sqrt(init(7)**2+xmass**2)),0.1)
              fact(i)=(sms*chrg/init(7)/beta)**2/abs(init(6))
             endif
               if(mod(icase,10).eq.1)then
               do im=i,1,-1
                xmsr(im,i)=0
                do il=2,min(i,im)-1
                 xmsr(im,i)=xmsr(im,i)+(xp(3,i)-xp(3,il))*
     +           (xp(3,im)-xp(3,il))*fact(il+1)
                enddo
                if(im.eq.i)then
                 xms(i)=xmsr(i,i)
                else
                xmsr(im,i)=2*xmsr(im,i)/
     +          sqrt(2*xms(i)+(ys(1,i)**2+ys(2,i)**2)*init(6)**2+
     +          (1-init(6)**2)*ys(3,i)**2+1.e-10)/
     +          sqrt(2*xms(im)+(ys(1,im)**2+ys(2,im)**2)*init(6)**2+
     +          (1-init(6)**2)*ys(3,im)**2+1.e-10)
                endif
               enddo
               else
               xms(i)=0
               if(i.gt.2)then
                xms(i)=fact(i)*(xp(3,i)-xp(3,i-1))**2*(i-2)
c                xms(i)=fact(i)*(xp(3,i)-xp(3,i-1))**2*(i-2)*2  ! last 2 - tubes
               endif
               endif
*
*              out points load
*
               call ucopy(out(1),outa(1,i),7)
               fck(i)=tkermt(out(1),point(1),ys(1,i),ims,xms(i),
     +          icc(1,1,i),aa(i),bb(i))
              if(mod(icase,10).eq.1)then
                call ucopy(out(1),init(1),7)
               else
                call ucopy(out(4),init(4),4)
                call ucopy(xp(1,i),init(1),3)
               endif
            enddo
            fc=0
            do i=1,npoint
             fc=fc+fck(i)
            do k=i+1,npoint
             fc=fc+sqrt(abs(fck(i)*fck(k)))*xmsr(i,k)
            enddo
            enddo
            call vzero(gg(1,1),50)
            call vzero(g(1),10)
            do i=1,npoint
             do j=1,5
              g(j)=g(j)+
     +        2*(m55(1,j,i)*(icc(1,1,i)*aa(i)+icc(1,2,i)*bb(i))+
     +        m55(3,j,i)*(icc(2,2,i)*bb(i)+icc(1,2,i)*aa(i)))
              do k=1,5
               gg(k,j)=gg(k,j)+2*(icc(1,1,i)*m55(1,k,i)*m55(1,j,i)+
     +         icc(2,2,i)*m55(3,k,i)*m55(3,j,i)+
     +         icc(1,2,i)*(m55(1,k,i)*m55(3,j,i)+
     +         m55(3,k,i)*m55(1,j,i)))
              enddo
             enddo
            enddo
           if(icase/10.ne.0.or.magstat2.le.0.or.point(3).gt.800)then
            do i=1,4
             gg(i,5)=0
             gg(5,i)=0
            enddo
            if(magstat2.le.0.or.point(3).gt.800)then
              gg(5,5)=0.000001
            endif
           endif
           call dsinv(5,gg(1,1),5,ifail)
           if(ifail.ne.0)then
c             write(*,*) 'sinv gg ',ifail
             goto 999
           endif
           er=sqrt(2*gg(5,5))
           dlt=(fc-fcb)/(fcb+2*npoint-5+1.e-3)
c           write(*,*)' iter',er,fc,fcb,xc
           fcb=fc
           if(abs(dlt).le.tol)then
            ifail=0
            goto 999
           endif
           do i=1,5
            sol(i)=(gg(i,1)*g(1)+gg(i,2)*g(2)+
     +      gg(i,3)*g(3)+gg(i,4)*g(4)+gg(i,5)*g(5))
            if(i.le.4)then
             xc(i)=xc(i)-sol(i)
            else
             xc(i)=xc(i)+sol(i)
            endif
           enddo
           if(xc(5).le.bound(1,5))xc(5)=bound(1,5)
           if(xc(5).ge.bound(2,5))xc(5)=bound(2,5)
           enddo

           ifail=2
c           if(fc/(2*npoint-5+1.e-3).le.2.)ifail=0
999       continue
          end
CDECK  ID>, TKFITPAR.   
         subroutine tkfitpar(init,chrg,point,out,m55,stept)
       !$OMP THREADPRIVATE (/tkgdefC/,/tftimeC/,/tkcorrC/)

*
* Input init(7) x0,y0,z0,nx,ny,nz,p
*       point(6)
*       yn(3)
*
* Output out(7)
*        m55(5,5)
*         stept
        parameter (maxmat=100)
        real *8  m55(5,5),dum(5,5),xmat(5,5,maxmat)
         real *4 init(7),point(6),out(7)
         real *4 vin(7),der(10)

              common /tkgdefC/tmaxf,smax,fieldmm,cconv,
     +        maxcal,nitm,xtol,iver,ialg,steps,npars,ims,
     +        bound,scale,cfld,sms,pii
              real *4 tmaxf,smax,fieldmm,bound(2,6),cfld,pii,cconv,
     +        scale(6)
              common/tftimeC/timer(100,2),xtime
              common /tkcorrC/
     +        xxn(2),vpar2(2,2),vpar(2,2),vxy(2)
              real *8 xxn,vpar2,vpar,vxy
          stept=0
          idir=1
          stepa=0
          imat=0
          sfield=cfld*abs(init(7)/chrg)
          call ucopy(init(1),vin(1),7)
          call ucopy(init(1),out(1),7)
          sdist=(point(1)-out(1))*point(4)+
     +         (point(2)-out(2))*point(5)+
     +         (point(3)-out(3))*point(6)
          if(sdist.lt.0)then
           idir=-1
           vin(4)=-init(4)
           vin(5)=-init(5)
           vin(6)=-init(6)
           out(4)=-init(4)
           out(5)=-init(5)
           out(6)=-init(6)
c           goto 200
          endif
         step=0
         ich=0
         nit=0
         za=out(3)
100       continue
          nit=nit+1
          if(nit.gt.nitm)goto 200
          if(imat.gt.maxmat)goto 200
          sdist=(point(1)-out(1))*point(4)+
     +         (point(2)-out(2))*point(5)+
     +         (point(3)-out(3))*point(6)
          sd2=out(4)*point(4)+out(5)*point(5)+out(6)*point(6)
          sdist=sdist/(sd2+1.e-10)
c          stepa=stepa+step
          if(min(sfield,smax)
     +     -stepa.le.0.or.abs(sdist).lt.cconv)then
           h=out(3)-za
           imat=imat+1
           if(abs(h).gt.steps)then
           call tkfunxy(vin(1),out(1),der(1))
           dfx=der(1)*h*chrg/init(7)
           dfy=der(2)*h*chrg/init(7)
           dfxp=der(3)*h*chrg/init(7)
           dfyp=der(4)*h*chrg/init(7)
           dgx=der(5)*h*chrg/init(7)
           dgy=der(6)*h*chrg/init(7)
           dgxp=der(7)*h*chrg/init(7)
           dgyp=der(8)*h*chrg/init(7)
           fm=der(9)
           gm=der(10)
           xmat(1,1,imat)=1+0.5*h*dfx
           xmat(1,2,imat)=h+0.5*h*dfxp
           xmat(1,3,imat)=0.5*h*dfy
           xmat(1,4,imat)=0.5*h*dfyp
           xmat(1,5,imat)=fm*h**2/2.
           xmat(2,1,imat)=dfx
           xmat(2,2,imat)=1+dfxp
           xmat(2,3,imat)=dfy
           xmat(2,4,imat)=dfyp
           xmat(2,5,imat)=h*fm
           xmat(3,1,imat)=0.5*h*dgx
           xmat(3,2,imat)=0.5*h*dgxp
           xmat(3,3,imat)=1+0.5*h*dgy
           xmat(3,4,imat)=h+0.5*h*dgyp
           xmat(3,5,imat)= 0.5*h**2*gm
           xmat(4,1,imat)= dgx
           xmat(4,2,imat)= dfxp
           xmat(4,3,imat)= dgy
           xmat(4,4,imat)= 1+dgyp
           xmat(4,5,imat)= h*gm
           xmat(5,1,imat)= 0
           xmat(5,2,imat)= 0
           xmat(5,3,imat)= 0
           xmat(5,4,imat)= 0
           xmat(5,5,imat)= 1
           else
            call vzero(xmat(1,1,imat),50)
            do ix=1,5
             xmat(ix,ix,imat)=1
            enddo
           endif
c          stepa=0
          za=out(3)

          endif
          call ucopy(out(1),vin(1),7)
          if(abs(sdist).lt.cconv)goto 200
          if(sdist.lt.0)then
           idir=-idir
           sdist=-sdist
           ich=ich+1
           do k=4,6
            vin(k)=-vin(k)
           enddo
          endif
          step=min(smax,sfield,sdist,sfield-stepa,smax-stepa)
          stept=stept+step*idir
          if(step.gt.steps )then
           call grkuta(chrg*idir,step,vin,out)
          else
            out(1)=vin(1)+step*vin(4)
            out(2)=vin(2)+step*vin(5)
            out(3)=vin(3)+step*vin(6)
            call ucopy(vin(4),out(4),3)
          endif
          goto 100
200       continue
          if(mod(ich,2).ne.0)then
           do k=4,6
            out(k)=-out(k)
           enddo
          endif
c         call vzero(beg(1,1),50)
c          do i=1,5
c            beg(i,i)=1
c          enddo
c          do i=imat,1,-1
c           call dmm55(beg,xmat(1,1,i),dum)
c           call ucopy(dum(1,1),beg(1,1),50)
c          enddo

c          call dmm55(beg(1,1),m55,dum)
          call dmm55(xmat(1,1,1),m55,dum)
          call ucopy(dum,m55,50)
          end

CDECK  ID>, TKFITPARCYL.   
         subroutine tkfitparcyl(init,chrg,point,out,m55,stept)
       !$OMP THREADPRIVATE (/tkgdefC/,/tftimeC/,/tkcorrC/)

*
* Input init(7) x0,y0,z0,nx,ny,nz,p
*       point(7) = coo_cyl, axis_cyl, radius_cyl
*       yn(3)
*
* Output out(7)
*        m55(5,5)
*         stept
        parameter (maxmat=100)
        real *8  m55(5,5),dum(5,5),xmat(5,5,maxmat)
         real *4 init(7),point(7),out(7)
         real *4 vin(7),der(10)

              common /tkgdefC/tmaxf,smax,fieldmm,cconv,
     +        maxcal,nitm,xtol,iver,ialg,steps,npars,ims,
     +        bound,scale,cfld,sms,pii
              real *4 tmaxf,smax,fieldmm,bound(2,6),cfld,pii,cconv,
     +        scale(6)
              common/tftimeC/timer(100,2),xtime
              common /tkcorrC/
     +        xxn(2),vpar2(2,2),vpar(2,2),vxy(2)
              real *8 xxn,vpar2,vpar,vxy,s1,s2
          stept=0
          idir=1
          stepa=0
          imat=0
          sfield=cfld*abs(init(7)/chrg)
          call ucopy(init(1),vin(1),7)
          call ucopy(init(1),out(1),7)
           s1=(point(1)-out(1))**2+
     +       (point(2)-out(2))**2+
     +       (point(3)-out(3))**2
          s2=(point(1)-out(1))*point(4)+
     +       (point(2)-out(2))*point(5)+
     +       (point(3)-out(3))*point(6)
          if(s1-s2**2.lt.0)then
           sdist=point(7)
          else
          sdist=point(7)-sqrt(s1-s2**2)
          endif
          if(sdist.lt.0)then
           stept=1.e10
#if defined(__AMSDEBUG__)
           write(*,*)'tkfitparcyl-E-outside of cyl',sdist,point(7)
#endif
           return
          endif
         step=0
         ich=0
         nit=0
         za=out(3)
100       continue
          nit=nit+1
          if(nit.gt.nitm)goto 200
          if(imat.gt.maxmat)goto 200
          s1=(point(1)-out(1))**2+
     +       (point(2)-out(2))**2+
     +       (point(3)-out(3))**2
          s2=(point(1)-out(1))*point(4)+
     +       (point(2)-out(2))*point(5)+
     +       (point(3)-out(3))*point(6)
          if(s1-s2**2.lt.0)then
           sdist=point(7)
          else
          sdist=point(7)-sqrt(s1-s2**2)
          endif
          sd2=out(4)*point(4)+out(5)*point(5)+out(6)*point(6)
          sd1=sqrt(abs(1-sd2**2))
          
          sdist=sdist/(sd1+1.e-10)
c          stepa=stepa+step
          if(min(sfield,smax)
     +     -stepa.le.0.or.abs(sdist).lt.cconv)then
           h=out(3)-za
           imat=imat+1
           if(abs(h).gt.steps)then
           call tkfunxy(vin(1),out(1),der(1))
           dfx=der(1)*h*chrg/init(7)
           dfy=der(2)*h*chrg/init(7)
           dfxp=der(3)*h*chrg/init(7)
           dfyp=der(4)*h*chrg/init(7)
           dgx=der(5)*h*chrg/init(7)
           dgy=der(6)*h*chrg/init(7)
           dgxp=der(7)*h*chrg/init(7)
           dgyp=der(8)*h*chrg/init(7)
           fm=der(9)
           gm=der(10)
           xmat(1,1,imat)=1+0.5*h*dfx
           xmat(1,2,imat)=h+0.5*h*dfxp
           xmat(1,3,imat)=0.5*h*dfy
           xmat(1,4,imat)=0.5*h*dfyp
           xmat(1,5,imat)=fm*h**2/2.
           xmat(2,1,imat)=dfx
           xmat(2,2,imat)=1+dfxp
           xmat(2,3,imat)=dfy
           xmat(2,4,imat)=dfyp
           xmat(2,5,imat)=h*fm
           xmat(3,1,imat)=0.5*h*dgx
           xmat(3,2,imat)=0.5*h*dgxp
           xmat(3,3,imat)=1+0.5*h*dgy
           xmat(3,4,imat)=h+0.5*h*dgyp
           xmat(3,5,imat)= 0.5*h**2*gm
           xmat(4,1,imat)= dgx
           xmat(4,2,imat)= dfxp
           xmat(4,3,imat)= dgy
           xmat(4,4,imat)= 1+dgyp
           xmat(4,5,imat)= h*gm
           xmat(5,1,imat)= 0
           xmat(5,2,imat)= 0
           xmat(5,3,imat)= 0
           xmat(5,4,imat)= 0
           xmat(5,5,imat)= 1
           else
            call vzero(xmat(1,1,imat),50)
            do ix=1,5
             xmat(ix,ix,imat)=1
            enddo
           endif
c          stepa=0
          za=out(3)

          endif
          call ucopy(out(1),vin(1),7)
          if(abs(sdist).lt.cconv)goto 200
          if(sdist.lt.0)then
           idir=-idir
           sdist=-sdist
           ich=ich+1
           do k=4,6
            vin(k)=-vin(k)
           enddo
          endif
          step=min(smax,sfield,sdist,sfield-stepa,smax-stepa)
          stept=stept+step*idir
          if(step.gt.steps)then
           call grkuta(chrg*idir,step,vin,out)
          else
            out(1)=vin(1)+step*vin(4)
            out(2)=vin(2)+step*vin(5)
            out(3)=vin(3)+step*vin(6)
            call ucopy(vin(4),out(4),3)
          endif
          goto 100
200       continue
          if(mod(ich,2).ne.0)then
           do k=4,6
            out(k)=-out(k)
           enddo
          endif
c         call vzero(beg(1,1),50)
c          do i=1,5
c            beg(i,i)=1
c          enddo
c          do i=imat,1,-1
c           call dmm55(beg,xmat(1,1,i),dum)
c           call ucopy(dum(1,1),beg(1,1),50)
c          enddo

c          call dmm55(beg(1,1),m55,dum)
          call dmm55(xmat(1,1,1),m55,dum)
          call ucopy(dum,m55,50)
          end



CDECK  ID>, TKFUNXY.
          subroutine tkfunxy(in,out,derl)
       !$OMP THREADPRIVATE (/tkclearC/)
          common /tkclearC/clear,der
          integer clear
          data clear/1/
          real der(10,2)
c          save der
          real derl(10)
           real *4 in(*),out(*),xx(3),h(3),hxy(3,3)
           if(clear.le.0)clear=1
c           clear=1
           if(clear.eq.2)then
            call ucopy( der(1,2),der(1,1),10)
          endif
          do k=clear,2
          if(k.eq.1)then
             call ucopy(in(1),xx(1),3)
             dx=in(4)/in(6)
             dy=in(5)/in(6)
          else
           call ucopy(out(1),xx(1),3)
             dx=out(4)/out(6)
             dy=out(5)/out(6)
          endif
           s=sqrt(1+dx**2+dy**2)
           call gufld(xx,h)
           call tkfld(xx,hxy)
           der(10,k)=s*((1+dy**2)*h(1)-dx*dy*h(2)-dx*h(3))
           der(9,k)=s*(dx*dy*h(1)-(1+dx**2)*h(2)+dy*h(3))
           der(8,k)=dy/s*((1+dy**2)*h(1)-dx*dy*h(2)-dx*h(3))+
     +     s*(2*dy*h(1)-dx*h(2))
           der(7,k)=dx/s*((1+dy**2)*h(1)-dx*dy*h(2)-dx*h(3))-
     +     s*(dy*h(2)+h(3))
           der(6,k)=
     +     s*((1+dy**2)*hxy(1,2)-dx*dy*hxy(2,2)-dx*hxy(3,2))
           der(5,k)=
     +       s*((1+dy**2)*hxy(1,1)-dx*dy*hxy(2,1)-dx*hxy(3,1))
           der(4,k)=dy/s*(dx*dy*h(1)-(1+dx**2)*h(2)+dy*h(3))+
     +    s*(dx*h(1)+h(3))
           der(3,k)=dx/s*(dx*dy*h(1)-(1+dx**2)*h(2)+dy*h(3))+
     +    s*(dy*h(1)-2*dx*h(2))
           der(2,k)=s*(dx*dy*hxy(1,2)-
     +     (1+dx**2)*hxy(2,2)+dy*hxy(3,2))
           der(1,k)=s*(dx*dy*hxy(1,1)-
     +(1+dx**2)*hxy(2,1)+dy*hxy(3,1))
          enddo
          do i=1,10
           derl(i)=(der(i,1)+der(i,2))*2.997e-4*0.5
          enddo
               end
           function aycos(y)
            x=y
            if(x.gt.1.)x=1.
            if(x.lt.-1.)x=-1.
            aycos=acos(x)
           end
           function aysin(y)
            x=y
            if(x.gt.1.)x=1.
            if(x.lt.-1.)x=-1.
            aysin=asin(x)
           end
CDECK  ID>, TKERMT. 
             function tkermt(pred,meas,sig,ims,xms,cc,aa,bb)
       !$OMP THREADPRIVATE (/tkfrunC/)
              parameter (maxpl=20)
              common /tkfrunC/npoint,xp(3,maxpl),ys(3,maxpl),
     +        yn(3,maxpl),xmass,chrg

             real *4 pred(*),meas(*),sig(*)
             real *8 cc(2,2),xperp(2,2),aa,bb
             cc(1,1)=sig(1)**2+pred(4)**2*sig(3)**2/pred(6)**2
             cc(2,2)=sig(2)**2+pred(5)**2*sig(3)**2/pred(6)**2
             cc(1,2)=0
             cc(2,1)=0
             xperp(1,1)=pred(1)-pred(4)/pred(6)*pred(3)
             xperp(2,1)=pred(2)-pred(5)/pred(6)*pred(3)
             xperp(1,2)=meas(1)-pred(4)/pred(6)*meas(3)
             xperp(2,2)=meas(2)-pred(5)/pred(6)*meas(3)
             if(ims.ne.0)then
               vv=xms/pred(6)**4
               cc(1,1)=cc(1,1)+vv*(1-pred(5)**2)
               cc(2,2)=cc(2,2)+vv*(1-pred(4)**2)
               cc(1,2)=cc(1,2)+vv*pred(4)*pred(5)
               cc(2,1)=cc(2,1)+vv*pred(4)*pred(5)
             endif
             call dsinv(2,cc(1,1),2,ifail)
             if(ifail.ne.0)then
c               write(*,*)'tkermt - ifail ',cc
                call vzero(cc(1,1),4)
             endif
             tkermt=0
             aa=xperp(1,1)-xperp(1,2)
             bb=xperp(2,1)-xperp(2,2)
             do i=1,2
              do j=1,2
               tkermt=tkermt+(xperp(i,1)-xperp(i,2))*
     +         cc(i,j)*(xperp(j,1)-xperp(j,2))
              enddo
             enddo
             end

      SUBROUTINE GRKUTA (CHARGE,STEP,VECT,VOUT)
C.
C.    ******************************************************************
C.    *                                                                *
C.    *  Runge-Kutta method for tracking a particle through a magnetic *
C.    *  field. Uses Nystroem algorithm (See Handbook Nat. Bur. of     *
C.    *  Standards, procedure 25.5.20)                                 *
C.    *                                                                *
C.    *  Input parameters                                              *
C.    *       CHARGE    Particle charge                                *
C.    *       STEP      Step size                                      *
C.    *       VECT      Initial co-ords,direction cosines,momentum     *
C.    *  Output parameters                                             *
C.    *       VOUT      Output co-ords,direction cosines,momentum      *
C.    *  User routine called                                           *
C.    *       CALL GUFLD(X,F)                                          *
C.    *                                                                *
C.    *    ==>Called by : <USER>, GUSWIM                               *
C.    *       Authors    R.Brun, M.Hansroul  *********                 *
C.    *                  V.Perevoztchikov (CUT STEP implementation)    *
C.    *                                                                *
C.    *                                                                *
C.    ******************************************************************
C.
#if !defined(CERNLIB_SINGLE)
      IMPLICIT DOUBLE PRECISION(A-H,O-Z)
#endif
      REAL CHARGE, STEP, VECT(*), VOUT(*), F(4)
      REAL XYZT(3), XYZ(3), X, Y, Z, XT, YT, ZT
      DIMENSION SECXS(4),SECYS(4),SECZS(4),HXP(3)
      EQUIVALENCE (X,XYZ(1)),(Y,XYZ(2)),(Z,XYZ(3)),
     +            (XT,XYZT(1)),(YT,XYZT(2)),(ZT,XYZT(3))
*
      PARAMETER (MAXIT = 1992, MAXCUT = 11)
      PARAMETER (EC=2.9979251D-4,DLT=1D-4,DLT32=DLT/32)
      PARAMETER (ZERO=0, ONE=1, TWO=2, THREE=3)
      PARAMETER (THIRD=ONE/THREE, HALF=ONE/TWO)
      PARAMETER (PISQUA=.986960440109D+01)
      PARAMETER      (IX=1,IY=2,IZ=3,IPX=4,IPY=5,IPZ=6)
*.
*.    ------------------------------------------------------------------
*.
*             This constant is for units CM,GEV/C and KGAUSS
*
      ITER = 0
      NCUT = 0
      DO 10 J=1,7
         VOUT(J)=VECT(J)
   10 CONTINUE
      PINV   = EC * CHARGE / VECT(7)
      TL = 0.
      H      = STEP
*
*
   20 REST  = STEP-TL
      IF (ABS(H).GT.ABS(REST)) H = REST
      CALL GUFLD(VOUT,F)
*
*             Start of integration
*
      X      = VOUT(1)
      Y      = VOUT(2)
      Z      = VOUT(3)
      A      = VOUT(4)
      B      = VOUT(5)
      C      = VOUT(6)
*
      H2     = HALF * H
      H4     = HALF * H2
      PH     = PINV * H
      PH2    = HALF * PH
      SECXS(1) = (B * F(3) - C * F(2)) * PH2
      SECYS(1) = (C * F(1) - A * F(3)) * PH2
      SECZS(1) = (A * F(2) - B * F(1)) * PH2
      ANG2 = (SECXS(1)**2 + SECYS(1)**2 + SECZS(1)**2)
      IF (ANG2.GT.PISQUA) GO TO 40
      DXT    = H2 * A + H4 * SECXS(1)
      DYT    = H2 * B + H4 * SECYS(1)
      DZT    = H2 * C + H4 * SECZS(1)
      XT     = X + DXT
      YT     = Y + DYT
      ZT     = Z + DZT
*
*              Second intermediate point
*
      EST = ABS(DXT)+ABS(DYT)+ABS(DZT)
      IF (EST.GT.H) GO TO 30
 
      CALL GUFLD(XYZT,F)
      AT     = A + SECXS(1)
      BT     = B + SECYS(1)
      CT     = C + SECZS(1)
*
      SECXS(2) = (BT * F(3) - CT * F(2)) * PH2
      SECYS(2) = (CT * F(1) - AT * F(3)) * PH2
      SECZS(2) = (AT * F(2) - BT * F(1)) * PH2
      AT     = A + SECXS(2)
      BT     = B + SECYS(2)
      CT     = C + SECZS(2)
      SECXS(3) = (BT * F(3) - CT * F(2)) * PH2
      SECYS(3) = (CT * F(1) - AT * F(3)) * PH2
      SECZS(3) = (AT * F(2) - BT * F(1)) * PH2
      DXT    = H * (A + SECXS(3))
      DYT    = H * (B + SECYS(3))
      DZT    = H * (C + SECZS(3))
      XT     = X + DXT
      YT     = Y + DYT
      ZT     = Z + DZT
      AT     = A + TWO*SECXS(3)
      BT     = B + TWO*SECYS(3)
      CT     = C + TWO*SECZS(3)
*
      EST = ABS(DXT)+ABS(DYT)+ABS(DZT)
      IF (EST.GT.2.*ABS(H)) GO TO 30
 
      CALL GUFLD(XYZT,F)
*
      Z      = Z + (C + (SECZS(1) + SECZS(2) + SECZS(3)) * THIRD) * H
      Y      = Y + (B + (SECYS(1) + SECYS(2) + SECYS(3)) * THIRD) * H
      X      = X + (A + (SECXS(1) + SECXS(2) + SECXS(3)) * THIRD) * H
*
      SECXS(4) = (BT*F(3) - CT*F(2))* PH2
      SECYS(4) = (CT*F(1) - AT*F(3))* PH2
      SECZS(4) = (AT*F(2) - BT*F(1))* PH2
      A      = A+(SECXS(1)+SECXS(4)+TWO * (SECXS(2)+SECXS(3))) * THIRD
      B      = B+(SECYS(1)+SECYS(4)+TWO * (SECYS(2)+SECYS(3))) * THIRD
      C      = C+(SECZS(1)+SECZS(4)+TWO * (SECZS(2)+SECZS(3))) * THIRD
*
      EST    = ABS(SECXS(1)+SECXS(4) - (SECXS(2)+SECXS(3)))
     ++        ABS(SECYS(1)+SECYS(4) - (SECYS(2)+SECYS(3)))
     ++        ABS(SECZS(1)+SECZS(4) - (SECZS(2)+SECZS(3)))
*
      IF (EST.GT.DLT .AND. ABS(H).GT.1.E-4) GO TO 30
      ITER = ITER + 1
      NCUT = 0
*               If too many iterations, go to HELIX
      IF (ITER.GT.MAXIT) GO TO 40
*
      TL = TL + H
      IF (EST.LT.(DLT32)) THEN
         H = H*TWO
      ENDIF
      CBA    = ONE/ SQRT(A*A + B*B + C*C)
      VOUT(1) = X
      VOUT(2) = Y
      VOUT(3) = Z
      VOUT(4) = CBA*A
      VOUT(5) = CBA*B
      VOUT(6) = CBA*C
      REST = STEP - TL
      IF (STEP.LT.0.) REST = -REST
      IF (REST .GT. 1.E-5*ABS(STEP)) GO TO 20
*
      GO TO 999
*
**              CUT STEP
   30 NCUT = NCUT + 1
*               If too many cuts , go to HELIX
      IF (NCUT.GT.MAXCUT)       GO TO 40
      H = H*HALF
      GO TO 20
*
**              ANGLE TOO BIG, USE HELIX
   40 F1  = F(1)
      F2  = F(2)
      F3  = F(3)
      F4  = SQRT(F1**2+F2**2+F3**2)
      RHO = -F4*PINV
      TET = RHO * STEP
      IF(TET.NE.0.) THEN
         HNORM = ONE/F4
         F1 = F1*HNORM
         F2 = F2*HNORM
         F3 = F3*HNORM
*
         HXP(1) = F2*VECT(IPZ) - F3*VECT(IPY)
         HXP(2) = F3*VECT(IPX) - F1*VECT(IPZ)
         HXP(3) = F1*VECT(IPY) - F2*VECT(IPX)
 
         HP = F1*VECT(IPX) + F2*VECT(IPY) + F3*VECT(IPZ)
*
         RHO1 = ONE/RHO
         SINT = SIN(TET)
         COST = TWO*SIN(HALF*TET)**2
*
         G1 = SINT*RHO1
         G2 = COST*RHO1
         G3 = (TET-SINT) * HP*RHO1
         G4 = -COST
         G5 = SINT
         G6 = COST * HP
 
         VOUT(IX) = VECT(IX) + (G1*VECT(IPX) + G2*HXP(1) + G3*F1)
         VOUT(IY) = VECT(IY) + (G1*VECT(IPY) + G2*HXP(2) + G3*F2)
         VOUT(IZ) = VECT(IZ) + (G1*VECT(IPZ) + G2*HXP(3) + G3*F3)
 
         VOUT(IPX) = VECT(IPX) + (G4*VECT(IPX) + G5*HXP(1) + G6*F1)
         VOUT(IPY) = VECT(IPY) + (G4*VECT(IPY) + G5*HXP(2) + G6*F2)
         VOUT(IPZ) = VECT(IPZ) + (G4*VECT(IPZ) + G5*HXP(3) + G6*F3)
*
      ELSE
         VOUT(IX) = VECT(IX) + STEP*VECT(IPX)
         VOUT(IY) = VECT(IY) + STEP*VECT(IPY)
         VOUT(IZ) = VECT(IZ) + STEP*VECT(IPZ)
*
      ENDIF
*
  999 END
       subroutine resetmagstat()
#include "mfield.h"
       magstat2=magstat
      end
       subroutine setmagstat()
#include "mfield.h"
       magstat=1
       fscale=1
      end
      subroutine zeromagstat()
#include "mfield.h"
       magstat2=0
      end 
