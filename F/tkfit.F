      SUBROUTINE TKFITA(NPOINTS, XM, EXM, IPLANEZ,
     + IMSCAT, xmass, OUTPAR, IERR)
*
*     Fitting NPOINTS (consecutive) (maximum 10 points)
*
*     Input Coordinates XM(i,j), i=1,3, j=1, NPOINTS
*     Error to Input Coordinates EXM(i,j), i=1,3, j=1, NPOINTS
*
*     If IMSCAT=0 no multiple scattering error is assumed
*     If IMSCAT<>0 the multiple scattering uncertainty is accounted for
*
*     BETA is the beta value if multiple scattering is required
*
*
*     Fitted parameters OUTPAR(k), k=1,5
*     OUTPAR(1) = x coordinate at first plane (cm)
*     OUTPAR(2) = y coordinate at first plane (cm)
*     OUTPAR(3) = u_x, component of direction at first plane
*     OUTPAR(4) = u_y, component of direction at first plane
*                 (u_z = -sqrt(1-u_x**2-u_y**2) )
*     OUTPAR(5) = 1/rigidity (1/GV)
*     OUTPAR(6) = sigma(1/rigidity)   
*
*     IERR=0 if everything was OK
*
********************************************
*
*     Things to play with (lengths are in cm):
*        ERRX = tracker resolution along X (K-side)
*        ERRY = tracker resolution along Y (S-side)
*        WLAD = Expected width of material in a tracker plane
*        RLEN = Radiation length (9.36 cm for silicon)
*        WLAD/RLEN = number of radiation lengths traversesed for one plane
*
*     The multiple scattering uses the formula:
*        (Deviation_rms_x)**2 = (Deviation_rms_y)**2 =
*          (0.0137/(rigidity(GV)*beta))**2 * (WLAD/RLEN) /3/cos(theta)
*
********************************************
*
*     Written by J. Alcaraz (1999)
*
*********************************************************************
*
      PARAMETER (NHITS=10, NITER=25)
      DIMENSION AXX(3,NHITS), AV(3,NHITS), XM(3,NHITS), XP(2,NHITS), 
     + AL(NHITS),exm(3,nhits),iplanez(nhits)
      DIMENSION D(2*NHITS,5), DX(5),  IDAUX(4*NHITS)
      DIMENSION OUTPAR(6)
      DIMENSION ABSUZ(NHITS) 
      real *8 ER2(2*NHITS,2*NHITS),DD(5,5)
 
      DATA WLAD /0.03/, RLEN /9.36/
      data tol/1.e-5/     

*
***---------------------------------------------------------------
*
c      write(*,*)imscat,xmass,iplanez,exm
      IERR = 1


      IF (NPOINTS.LT.3 .OR. NPOINTS.GT.nhits) THEN
        GOTO 999
      ENDIF

C Find out at which plane are they
c      CALL VZERO (Iplanez, NPOINTS)
c      do IP = 1, NPOINTS
c        distmin = 1.E30
c        do IZ = 1, nplanes
c          IF (ABS(XM(3,IP)-ZNOM(IZ)).LT.distmin) THEN
c            distmin = ABS(XM(3,IP)-ZNOM(IZ))
c            iplanez(IP) = IZ
c          ENDIF
c        end do
c      end do
c      print*,' IPLANEZ= ', iplanez
        
      do 20 ipl = 1, NPOINTS
        if (ipl.EQ.1) then
          DO i=1,3
            axx(i,ipl) = 0.
            av(i,ipl) = 0.
          END DO
        else
          CALL PATH_INTEGRAL (XM(1,ipl-1), XM(1,ipl)
     >               , axx(1,ipl), av(1,ipl) )
        endif
c        print*,'ipl, axx= ', ipl, (axx(i,ipl),i=1,3)
c        print*,'ipl, av= ', ipl, (av(i,ipl),i=1,3)
20    CONTINUE

      OUTPAR(1) = XM(1,1)
      OUTPAR(2) = XM(2,1)
      unorm = sqrt(  (XM(1,2)-XM(1,1))**2
     >             + (XM(2,2)-XM(2,1))**2
     >             + (XM(3,2)-XM(3,1))**2)
      OUTPAR(3) = (XM(1,2)-XM(1,1))/UNORM
      OUTPAR(4) = (XM(2,2)-XM(2,1))/UNORM
      OUTPAR(5) = 0.
c      print*,'First: OUTpar= ', OUTpar
      do 30 iter = 1, NITER
        outpar5=outpar(5)
        DO J = 1, NPOINTS
          IF (J.EQ.1) then
            ux = OUTPAR(3)
            uy = OUTPAR(4)
            uz = -sqrt(1.-ux**2-uy**2)
            AL(1) = 0.
            XP(1,1) = OUTPAR(1)
            XP(2,1) = OUTPAR(2)
          ELSE
            ux = ux + OUTPAR(5)*AL(J-1)*av(1,J-1)
            uy = uy + OUTPAR(5)*AL(J-1)*av(2,J-1)
c            print*,' ux, uy= ', ux, uy
            uz = -sqrt(1.-ux**2-uy**2)
            IF (OUTPAR(5).eq.0.) then
              AL(J) = (XM(3,J)-XM(3,J-1))/uz
            ELSE
              AL(J) = (-uz
     >          +sqrt(uz**2+4.*OUTPAR(5)*(xm(3,j)-xm(3,j-1))*axx(3,j)))
     >                /2./OUTPAR(5)/axx(3,j)
            ENDIF
            AL(J) = (XM(3,J)-XM(3,J-1))/uz
     >            + (XM(3,J)-XM(3,J-1))**2/uz**3/4.*OUTPAR(5)*axx(3,j)
            XP(1,J) = XP(1,J-1) + ux*AL(J) + OUTPAR(5)*AL(J)**2*axx(1,j)
            XP(2,J) = XP(2,J-1) + uy*AL(J) + OUTPAR(5)*AL(J)**2*axx(2,j)
          ENDIF
c          PRINT*,' JX,DX, DY (um)= ', J,(XM(1,J)-XP(1,J))*1.E4
c     >                    ,(XM(2,J)-XP(2,J))*1.E4
          ABSUZ(J) = abs(uz)
        END DO
c        print*,' AL= ', AL

        CALL VZERO (D, 2*NHITS*5)
        CALL VZERO (ER2, 2*NHITS*2*NHITS*2)
        DO J = 1, NPOINTS
          jx = j
          jy = j+NPOINTS
          D(JX,1) = 1.
          D(JY,2) = 1.
          DO K = 1, J
            D(JX,3) = D(JX,3) + AL(K)
            D(JY,4) = D(JY,4) + AL(K)
            DO L = 1, K
              IF (L.EQ.K) THEN
                D(JX,5) = D(JX,5) + AL(K)*AL(L)*axx(1,k)
                D(JY,5) = D(JY,5) + AL(K)*AL(L)*axx(2,k)
              ELSE
                D(JX,5) = D(JX,5) + AL(K)*AL(L)*av(1,l)
                D(JY,5) = D(JY,5) + AL(K)*AL(L)*av(2,l)
              ENDIF
            END DO
          END DO
c          PRINT*,'JX, D= ', J, (D(JX,I),i=1,5)
c          PRINT*,'JY, D= ', J, (D(JY,I),i=1,5)

          DO L = 1, JX
            M = L + NPOINTS
            IF (L.EQ.JX) THEN
              ER2(L,JX) = ER2(L,JX) + Exm(1,jx)**2
              ER2(M,JY) = ER2(M,JY) + Exm(2,jx)**2
            ENDIF
            IF (IMSCAT.NE.0) THEN
              IF (OUTPAR(5).NE.0.) THEN
c                PBETAI = (OUTPAR(5)/2.997E-4)/BETA
                xinv=OUTPAR(5)/2.997E-4
                PBETAI = xinv*sqrt(1+xmass**2*xinv**2)
              ELSE
                PBETAI = 0.
              ENDIF
              SCAREF = (0.0137*PBETAI)**2*(WLAD/RLEN)/3.
              ZINVLI2 = 0.
              DO K = 1, L-1
                AL_K = 0.
                DO KK = K+1, JX
                  IZMAX = (IPLANEZ(KK)-IPLANEZ(K))/(KK-K)
                  DO IZ = 1, IZMAX
                    AL_K = AL_K + (AL(KK)*IZ)/IZMAX
                  END DO
                END DO
                AL_L = 0.
                DO LL = K+1, L
                  IZMAX = (IPLANEZ(LL)-IPLANEZ(K))/(LL-K)
                  DO IZ = 1, IZMAX
                    AL_L = AL_L + (AL(LL)*IZ)/IZMAX
                  END DO
                END DO
                ER2(L,JX) = ER2(L,JX) + AL_K*AL_L*SCAREF/ABSUZ(K)
                ER2(M,JY) = ER2(M,JY) + AL_K*AL_L*SCAREF/ABSUZ(K)
              END DO
            ENDIF
            ER2(JX,L) = ER2(L,JX)
            ER2(JY,M) = ER2(M,JY)
          END DO
        END DO

c        DO JJ = 1, NPOINTS
c          PRINT*,'JJ, ER2X= ', JJ, (ER2(JJ,KK),KK=1,NPOINTS)
c        END DO
c        DO JJ = 1, NPOINTS
c          PRINT*,'JJ, ER2Y= ', JJ
c     >       , (ER2(JJ+NPOINTS,KK+NPOINTS),KK=1,NPOINTS)
c        END DO

        CALL DINV (2*NPOINTS, ER2, 2*NHITS, IDAUX, IFAIL)
        IF (IFAIL.LT.0) THEN
c          PRINT*,'**************************************'
          PRINT*,' ER2 Covariance Matrix is Singular !!!'
c          PRINT*,'**************************************'
          outpar(5)=0
          GOTO 999
        ENDIF

        DO J = 1, 5
          DX(J) = 0.
          DO L = 1, NPOINTS
           DO M = 1, NPOINTS
            DX(J) = DX(J) + D(L,J)*ER2(L,M)*XM(1,M)
            DX(J) = DX(J) + D(L+NPOINTS,J)*ER2(L+NPOINTS,M+NPOINTS)
     >               *XM(2,M)
           END DO
          END DO
          DO K = 1, 5
            DD(J,K) = 0.
            DO L = 1, NPOINTS
             DO M = 1, NPOINTS
              DD(J,K) = DD(J,K) + D(L,J)*ER2(L,M)*D(M,K)
              DD(J,K) = DD(J,K) + D(L+NPOINTS,J)
     >                 *ER2(L+NPOINTS,M+NPOINTS)*D(M+NPOINTS,K)
             END DO
            END DO
          END DO
c          PRINT*,' J, DD= ', J, (DD(J,i),i=1,5)
        END DO
        
        CALL DINV (5, DD, 5, IDAUX, IFAIL)
        IF (IFAIL.LT.0) THEN
c          PRINT*,'**************************************'
          PRINT*,' DD Covariance Matrix is Singular !!!'
c          PRINT*,'**************************************'
c          PRINT*,' NPOINTS= ', NPOINTs
c          DO IV = 1, NPOINTS
c            PRINT*,(XM(IU,IV),IU=1,3)
c          END DO
c          PRINT*,' '
c          DO J = 1, 5
c            PRINT*,' J, DD= ', J, (DD(J,i),i=1,5)
c          END DO
c        DO JJ = 1, NPOINTS
c          PRINT*,'JJ, ER2X= ', JJ, (ER2(JJ,KK),KK=1,NPOINTS)
c        END DO
c        DO JJ = 1, NPOINTS
c          PRINT*,'JJ, ER2Y= ', JJ
c     >       , (ER2(JJ+NPOINTS,KK+NPOINTS),KK=1,NPOINTS)
c        END DO
          outpar(5)=0
          GOTO 999
        ELSE
c          DO J = 1, 5
c            PRINT*,' J, DD= ', J, (DD(J,i),i=1,5)
c          END DO
        ENDIF

        DO K = 1, 5
          OUTPAR(K) = 0.
          DO I = 1, 5
            OUTPAR(K) = OUTPAR(K) + DD(K,I)*DX(I)
          END DO
        END DO
c        PRINT*,' OUTpar= ', OUTpar
         if(abs(outpar5-outpar(5))/2.997e-4.lt.tol)then
          goto 998
         endif
30    continue
      ierr=2
      OUTPAR(5)=OUTPAR(5)/2.997E-4
      OUTPAR(6)=SQRT(DD(5,5))/2.997E-4
      write(*,*)'tkfita-Error-TooManyIterations'
      goto 999
 998  continue
c      write(*,*)'qq ',iter
      OUTPAR(5)=OUTPAR(5)/2.997E-4
      OUTPAR(6)=SQRT(DD(5,5))/2.997E-4
      IERR = 0

999   CONTINUE
*
      END
      SUBROUTINE PATH_INTEGRAL (P1, P2, axx, av)
*
      REAL P1(3), P2(3), alpha, axx(3), av(3)
      REAL X(3), U(3), B(3)
*
      PARAMETER (NTOT=20)
*
***---------------------------------------------------------------
*
      CALL VZERO (axx, 3)
      CALL VZERO (av, 3)

      UNORM = (P2(1)-P1(1))**2+(P2(2)-P1(2))**2+(P2(3)-P1(3))**2
      IF (UNORM.LE.0.) GOTO 999
      UNORM = sqrt(UNORM)

c      print*,' P1= ', P1
c      print*,' P2= ', P2

      do l = 0, NTOT
        alpha = float(l)/ntot

        if (l.eq.0.or.l.eq.ntot) then
           fact = 1.
        else if (mod(l,2).eq.1) then
           fact = 4.
        else
           fact = 2.
        endif

        do i = 1, 3
          X(i) = P1(i) + alpha*(P2(i)-P1(i))
          U(i) = (P2(i)-P1(i))/UNORM
        end do
c        print*, ' l, X, U= ', l, X, U

        CALL GUFLD (X, B)
c        print*, ' l, b= ', l, b
        axx(1) = axx(1) + fact*
     >           (1.-alpha)*(u(2)*b(3)-u(3)*b(2))
        axx(2) = axx(2) + fact*
     >           (1.-alpha)*(u(3)*b(1)-u(1)*b(3))
        axx(3) = axx(3) + fact*
     >           (1.-alpha)*(u(1)*b(2)-u(2)*b(1))
        av(1) = av(1) + fact*(u(2)*b(3)-u(3)*b(2))
        av(2) = av(2) + fact*(u(3)*b(1)-u(1)*b(3))
        av(3) = av(3) + fact*(u(1)*b(2)-u(2)*b(1))
      end do

      axx(1) = axx(1)/3./NTOT
      axx(2) = axx(2)/3./NTOT
      axx(3) = axx(3)/3./NTOT
      av(1) = av(1)/3./NTOT
      av(2) = av(2)/3./NTOT
      av(3) = av(3)/3./NTOT

999   CONTINUE
      
*                                  END PATH_INTEGRAL
      END
