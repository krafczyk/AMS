CDECK  ID>, BLANKDEK.   
*CMZ :  3.21/02 29/03/94  15.41.18  by  S.Giani
*-- Author :
CDECK  ID>, ASTDES. 

CDECK  ID>, LSCOMS. 

*-- Author :
CDECK  ID>, ASTDES. 

CDECK  ID>, LSCOMS. 

*-- Author :
CDECK  ID>, ZPINIT. 
*CMZ :          05/06/92  09.26.29  by
*-- Author :
      SUBROUTINE ZPINIT
      COMMON/GCKINE/IKINE,PKINE(10),ITRA,ISTAK,IVERT,IPART,ITRTYP
     +      ,NAPART(5),AMASS,CHARGE,TLIFE,VERT(3),PVERT(4),IPAOLD
C
      INTEGER       IKINE,ITRA,ISTAK,IVERT,IPART,ITRTYP,NAPART,IPAOLD
      REAL          PKINE,AMASS,CHARGE,TLIFE,VERT,PVERT
C
       common /seedC/seed
       integer seed

*-- Random number seed
*
      CALL DATIME(IDD,ITT)
      SEED = ITT+IDD*341
      WRITE(*,1045)SEED
1045  FORMAT(/
     +   20X,'RANDOM NUMBER SEED =',I20,/)
      END
CDECK  ID>, GUTREV. 
*CMZ :          05/06/92  09.26.29  by
*-- Author :
      SUBROUTINE GUTREV
      CALL GTREVE
      END


CDECK  ID>, USTUFF. 
*CMZ :          05/06/92  09.26.57  by
CDECK  ID>, SPECTRA.
c      SUBROUTINE SPECTRA
c***************************************************************
c*
c* read in momentum spectra of H, He, Li, Be, B and C
c* seen at 51 degree by AMS in 2002
c*
c*                 files from G. Guzik
c*
c*                                         Y.F. Wang
c*     Bug fixed V. Shoutko
c**************************************************************
c      real *4 yap(500)
c      data yap/
c     +   1.0000      ,50.000      ,587.00      ,1705.0      ,2841.0
c     +  ,4329.0      ,6163.0      ,8420.0      ,11036.      ,14104.
c     +  ,16461.      ,18793.      ,21135.      ,23615.      ,25994.
c     +  ,28222.      ,30239.      ,32038.      ,33728.      ,35953.
c     +  ,37071.      ,38644.      ,39417.      ,40628.      ,41875.
c     +  ,43109.      ,43891.      ,45431.      ,46059.      ,47021.
c     +  ,47522.      ,48765.      ,49606.      ,50031.      ,50586.
c     +  ,49328.      ,47786.      ,46891.      ,45876.      ,44465.
c     +  ,44208.      ,43229.      ,42566.      ,42312.      ,41732.
c     +  ,41047.      ,40183.      ,39570.      ,38942.      ,38192.
c     +  ,37014.      ,36300.      ,35540.      ,34913.      ,33767.
c     +  ,32913.      ,32231.      ,31837.      ,30946.      ,29737.
c     +  ,28978.      ,28566.      ,27468.      ,27394.      ,26360.
c     +  ,25588.      ,24938.      ,23986.      ,23810.      ,22810.
c     +  ,22046.      ,21684.      ,20946.      ,20522.      ,20061.
c     +  ,19388.      ,18556.      ,18493.      ,17741.      ,17155.
c     +  ,16693.      ,16221.      ,15827.      ,15320.      ,15043.
c     +  ,14496.      ,13985.      ,13647.      ,13109.      ,12875.
c     +  ,12466.      ,12087.      ,11911.      ,11291.      ,10986.
c     +  ,10799.      ,10329.      ,10114.      ,9925.0      ,9591.0
c     +  ,9311.0      ,9198.0      ,9085.0      ,9053.0      ,8950.0
c     +  ,8644.0      ,8771.0      ,8281.0      ,8171.0      ,8150.0
c     +  ,8048.0      ,7864.0      ,7777.0      ,7458.0      ,7544.0
c     +  ,7494.0      ,7135.0      ,6915.0      ,6911.0      ,6819.0
c     +  ,6531.0      ,6370.0      ,6439.0      ,6223.0      ,6116.0
c     +  ,6112.0      ,5948.0      ,5741.0      ,5781.0      ,5508.0
c     +  ,5496.0      ,5243.0      ,5153.0      ,5131.0      ,4980.0
c     +  ,4952.0      ,4763.0      ,4675.0      ,4688.0      ,4367.0
c     +  ,4397.0      ,4196.0      ,4239.0      ,4059.0      ,3917.0
c     +  ,3847.0      ,3920.0      ,3748.0      ,3709.0      ,3643.0
c     +  ,3525.0      ,3382.0      ,3337.0      ,3251.0      ,3231.0
c     +  ,3179.0      ,3001.0      ,3024.0      ,2951.0      ,2858.0
c     +  ,2786.0      ,2795.0      ,2685.0      ,2706.0      ,2621.0
c     +  ,2478.0      ,2390.0      ,2369.0      ,2387.0      ,2289.0
c     +  ,2242.0      ,2164.0      ,2048.0      ,2073.0      ,1993.0
c     +  ,1940.0      ,2011.0      ,1890.0      ,1793.0      ,1736.0
c     +  ,1710.0      ,1720.0      ,1670.0      ,1724.0      ,1589.0
c     +  ,1568.0      ,1529.0      ,1551.0      ,1496.0      ,1430.0
c     +  ,1490.0      ,1350.0      ,1383.0      ,1313.0      ,1308.0
c     +  ,1195.0      ,1233.0      ,1193.0      ,1168.0      ,1127.0
c     +  ,1159.0      ,1060.0      ,1022.0      ,987.00      ,995.00
c     +  ,955.00      ,958.00      ,928.00      ,896.00      ,913.00
c     +  ,876.00      ,817.00      ,793.00      ,796.00      ,818.00
c     +  ,762.00      ,703.00      ,723.00      ,712.00      ,676.00
c     +  ,698.00      ,727.00      ,658.00      ,625.00      ,631.00
c     +  ,593.00      ,569.00      ,553.00      ,541.00      ,544.00
c     +  ,568.00      ,497.00      ,537.00      ,470.00      ,465.00
c     +  ,493.00      ,439.00      ,423.00      ,429.00      ,383.00
c     +  ,392.00      ,405.00      ,411.00      ,372.00      ,358.00
c     +  ,378.00      ,323.00      ,352.00      ,334.00      ,343.00
c     +  ,317.00      ,331.00      ,280.00      ,329.00      ,310.00
c     +  ,291.00      ,302.00      ,280.00      ,252.00      ,268.00
c     +  ,251.00      ,245.00      ,242.00      ,252.00      ,227.00
c     +  ,223.00      ,227.00      ,219.00      ,207.00      ,198.00
c     +  ,194.00      ,192.00      ,152.00      ,174.00      ,167.00
c     +  ,179.00      ,169.00      ,168.00      ,165.00      ,168.00
c     +  ,151.00      ,162.00      ,149.00      ,137.00      ,143.00
c     +  ,138.00      ,142.00      ,122.00      ,128.00      ,111.00
c     +  ,111.00      ,131.00      ,104.00      ,116.00      ,103.00
c     +  ,105.00      ,117.00      ,95.000      ,100.00      ,80.000
c     +  ,105.00      ,91.000      ,78.000      ,97.000      ,89.000
c     +  ,84.000      ,87.000      ,85.000      ,90.000      ,96.000
c     +  ,55.000      ,64.000      ,88.000      ,57.000      ,67.000
c     +  ,76.000      ,72.000      ,54.000      ,42.000      ,62.000
c     +  ,59.000      ,63.000      ,56.000      ,36.000      ,67.000
c     +  ,50.000      ,44.000      ,47.000      ,40.000      ,49.000
c     +  ,39.000      ,43.000      ,47.000      ,32.000      ,38.000
c     +  ,37.000      ,36.000      ,44.000      ,28.000      ,34.000
c     +  ,31.000      ,39.000      ,31.000      ,26.000      ,30.000
c     +  ,34.000      ,36.000      ,28.000      ,32.000      ,24.000
c     +  ,28.000      ,32.000      ,27.000      ,18.000      ,24.000
c     +  ,24.000      ,18.000      ,16.000      ,24.000      ,17.000
c     +  ,14.000      ,21.000      ,16.000      ,23.000      ,21.000
c     +  ,19.000      ,12.000      ,18.000      ,22.000      ,18.000
c     +  ,12.000      ,18.000      ,18.000      ,19.000      ,17.000
c     +  ,12.000      ,13.000      ,10.000      ,16.000      ,12.000
c     +  ,13.000      ,15.000      ,12.000      ,13.000      ,12.000
c     +  ,7.0000      ,17.000      ,11.000      ,12.000      ,10.000
c     +  ,11.000      ,12.000      ,12.000      ,6.0000      ,5.0000
c     +  ,7.0000      ,7.0000      ,5.0000      ,15.000      ,9.0000
c     +  ,7.0000      ,9.0000      ,10.000      ,9.0000      ,10.000
c     +  ,8.0000      ,4.0000      ,13.000      ,5.0000      ,8.0000
c     +  ,9.0000      ,5.0000      ,10.000      ,9.0000      ,4.0000
c     +  ,3.0000      ,4.0000      ,2.0000      ,4.0000      ,2.0000
c     +  ,8.0000      ,5.0000      ,4.0000      ,4.0000      ,7.0000
c     +  ,3.0000      ,7.0000      ,3.0000      ,2.0000      ,2.0000
c     +  ,2.0000      ,2.0000      ,5.0000      ,4.0000      ,1.0000
c     +  ,1.0000      ,3.0000      ,8.0000      ,1.0000      ,4.0000
c     +  ,6.0000      ,2.0000      ,1.0000      ,5.0000      ,2.0000
c     +  ,2.0000      ,7.0000      ,3.0000      ,1.0000      ,1.0000
c     +  ,3.0000      ,2.0000      ,2.0000      ,1.0000      ,2.0000
c     +  ,1.0000      ,4.0000      ,1.0000      ,1.0000      ,3.0000
c     +  ,1.0000      ,1.0000      ,1.0000      ,1.0000      ,1.0000
c     +  ,1.0000      ,1.0000      ,1.0000      ,1.0000      ,2.0000
c     +  ,1.0000      ,6.0000      ,1.0000      ,1.0000      ,3.0000
c     +  ,1.0000      ,4.0000      ,3.0000      ,1.0000      ,1.0000
c     +  ,2.0000      ,3.0000      ,1.0000      ,2.0000      ,1.0000
c     +  ,1.0000      ,2.0000      ,1.0000      ,3.0000      ,1.0000
c     +  ,1.0000      ,2.0000      ,2.0000      ,1.0000      ,1.0000
c     +  ,1.0000      ,1.0000      ,1.0000      ,1.0000      ,1.0000/
c+CDE, FIELD.
c      DIMENSION AREAD1(28),AREAD2(28)
c      CHARACTER*24 FILE1,FILE2
c+SELF, IF=-VAX.
c      DATA FILE1
c     &/'AMS_SS_2002_AC_GEO.LIN'/
c      DATA FILE2
c     &/'AMS_SS_2002_GCR_GEO.LIN'/
c+SELF, IF=VAX.
c      DATA FILE1
c     &/'gener:AMS_SS_2002_AC_GEO.LIN'/
c      DATA FILE2
c     &/'gener:AMS_SS_2002_GCR_GEO.LIN'/
c+SELF.
c      DO I = 1, 28
c       CALL HBOOK1(20000+I,'Spectra',120,1.005,5.005,0.)
c      ENDDO
c      OPEN(10, FILE=
c     +amsdatadir(1:amsdlength)//FILE1, FORM='FORMATTED',STATUS='OLD')
c      OPEN(11, FILE=
c     +amsdatadir(1:amsdlength)//FILE2, FORM='FORMATTED',STATUS='OLD')
c      DO I = 1, 8
c      READ(10,*)
c      READ(11,*)
c      ENDDO
c
c      eold=0
c      DO I = 1,115
c       READ(10,*) E, AREAD1
c       READ(11,*) E, AREAD2
c       AE = ALOG10(E)
c       de=e-eold
c       DO J = 1, 28
c        CALL HFILL(20000+J,AE,0.,(AREAD1(J)+AREAD2(J))*de)
c       ENDDO
c       eold=e
c      ENDDO
c      do j=1,6
cc       ee=hrndm1(20000+j)
c      enddo
c      CLOSE(10)
c      CLOSE(11)
c      call hbook1(30001,'qq',12,0.,6.,0.)
cC     20500. 7600. 3850. 1750. 880. 390. 157. 55. 19. 5. 1.8 1.3
c      call hf1(30001,0.75,12900.)
c      call hf1(30001,1.25,4550.)
c      call hf1(30001,1.75,1810.)
c      call hf1(30001,2.25,846.)
c      call hf1(30001,2.75,376.)
c      call hf1(30001,3.25,177.)
c      call hf1(30001,3.75,67.)
c      call hf1(30001,4.25,29.)
c      call hf1(30001,4.75,7.3)
c      call hf1(30001,5.25,2.7)
c      call hf1(30001,5.75,.4)
c* book & fill ap
c      call hbook1(120001,'ap',500,50.,50050.,0.)
c          bw=100.
c          al=50.
c          bl=50050.
c          do i=1,500
c           r=yap(i)
c           xx=(i-1)*bw+al+bw/2.
c           call hf1(120001,xx,r)
c          enddo
c
c      RETURN
c      END
c
CDECK  ID>, ABCR2.  
        subroutine abcr2(i,a_p,z_p,a_t,z_t,p_p,ipart)
       parameter (inmat=1000)
       common /abcrossC/sigma_t(inmat),sigma_el(3,inmat),
     +  sigma_q(5,inmat),abinx(inmat),abelx(inmat),abqex(inmat)
        common /abcr2C/qexnor,anxnor,elxnor,sine,sela,sqel,fsig
*
**** G.L. 2/2/00 *********
 
       common /hvsec/ihvse(2)
*
       real *4 st,se(3),sq(5)
*
       REAL *4 TRIPA_T
       integer IFLAG_T

        TRIPA_T=0.0
        IFLAG_T=0
*
*************************

        if(i.gt.inmat)then
         write(*,*)'ABCR2 - too many media'
           i=inmat
        endif

        sigma_t(i)=0
        call vzero(sigma_el(1,i),3)
        call vzero(sigma_q(1,i),5)
        do k=1,2
         call abcross(k,a_p,a_t,z_p,z_t,p_p,
     +   st,se,sq)
         sigma_t(i)=sigma_t(i)+st/2
         do j=1,3
          sigma_el(j,i)=sigma_el(j,i)+se(j)/2
         enddo
         if(k.eq.1)then
         do j=1,5
          sigma_q(j,i)=sq(j)
         enddo
         endif
        enddo
**** G.L. 2/2/00 *********
*         
          if (ihvse(1).eq.2) then       

            CALL TRIPA(ipart,P_P,Z_P,A_P,Z_T,A_T,TRIPA_T,IFLAG_T)

            IF (IFLAG_T.EQ.1) THEN  
             sigma_q(1,i)=0
             sigma_t(i)=TRIPA_T
            ENDIF

          endif ! ihvse(1).eq.2
*
****************************
        summ=sigma_q(1,i)+sigma_el(1,i)
        if(summ.gt.sigma_t(i))then
c         write(*,*)'ABCR2 - E - wrong sigma_tot',i,sigma_t(i),
c     +  sigma_q(1,i),sigma_el(1,i)
         sigma_q(1,i)=0
        endif
        end

CDECK  ID>, TRIPA. 
cccccccccccccccccccccccccccccccccccccccccccccccccccccccccccc
c                                                   2/2/2000
c Deuteron total cross section
c                                               by G.LAMANNA  
cccccccccccccccccccccccccccccccccccccccccccccccccccccccccccc
c
      SUBROUTINE TRIPA(ipart,P_P,Z_P,A_P,Z_T,A_T,SIGMA_T,IFLAG)
c

        real A_P,A_T
        integer IFLAG
        real M_P,P_P,M_T,Z_T,Z_P,r_t,r_p
        real E_lab,E_cm,T1,D,C_E,S,dE,R,B,X1,S_L,X_m
        real R_C,tem_p,tem_t        
        real *4 sigma_t
        DATA PI / 3.1415927 /
        DATA r0 / 1.1 /
*        DATA M_P / 1876. /  
        real P_PM
*
*    Input:
*     a_p,a_t,z_p,z_t,p_p [atom number projectile & target,Z p & t
*      and momentom of projectile [MeV/c]]
*     Transform p from GeV/c to Mev/c is mandatory
*
*    Output:
*     sigma_T=total inelastic cr_sec [mbarn]


        sigma_T=0.

        IFLAG=0
        IF (IPART.NE.45) GOTO 66
*       
        IFLAG=1 
*
*       ASSUMING TARGET MASS M_T=A_T * 938.0 MeV/c2 (PROTON MASS)

        M_T=A_T*938.0 
        M_P=A_P*938.0

*
*    P_P  is    DEUTERON'S momentum in                           "MeV/c"           

       P_PM=P_P*1000.

* 
*    E_k  is    DEUTERON'S KINETIC ENERGY 
*               per NUMBER OF NUCLEONS (dividing by 2=A_P) in    "MeV/A"    
*

      E_k=((sqrt((P_PM**2)+(M_P**2))-M_P)/2.)  
*

* ***************     Parameter T1  and R_C  for D+X ....

      T1=23.
      R_C=1.
* ***************     Parameter D for D+X ....

      D=1.65+(0.1/(1.+(exp((500.-E_k)/200.))))


       if (Z_T.eq.1.0) THEN
         if (A_T.lt.2.0.and.A_T.gt.0) then ! d+p becomes p+d

         tem_p=0.0
         tem_t=0.0
         tem_p=A_P
         tem_t=A_T
         A_P=tem_t
         A_T=tem_p

         tem_p=0.0
         tem_t=0.0
         tem_p=M_P
         tem_t=M_T
         M_P=tem_t
         M_T=tem_p

         T1=18.
         R_C=13.5
         D=1.85+(0.16/(1.+(exp((500.-E_k)/200.))))
       endif
      endif
*************************************
* Center of mass energy : E_cm
*
      E_lab=(E_k*A_P)+M_P
*
      
      E_cm=sqrt((M_P**2)+(M_T**2)+(2.*M_T*E_lab))
*
*
***
*

* ************        Parameter C_E      for D+X systems:

      C_E=(D*(1-exp(-E_k/T1)))-((0.292*exp(-E_k/792.))*
     +       cos(0.229*(E_k**0.453)))

*
*
* ************        Parameter S        for D+X systems:

        S=((A_P**(1./3.))*(A_T**(1./3.)))/
     +     ((A_P**(1./3.))+(A_T**(1./3.)))  

*
*
* ************        Parameter dE       for D+X systems:

        dE=(1.85*S)+(0.16*(S/(E_cm**(1./3.))))-C_E+(0.91*
     +   ((A_T-2*Z_T)*Z_P/(A_T*A_P)))  

*
*
* ************ radii r_p and r_t         for D+X systems:

              r_p=RADIUS(A_P)
              r_t=RADIUS(A_T)

*
*
* ************         Parameter R       for D+X systems:
        
        R=r_p+r_t+(1.2*((A_P**(1./3.)) +
     +      (A_T**(1./3.)))/(E_cm**(1./3.)))

*
*
* ***********         Parameter B       for D+X systems:

        B=(1.44*Z_P*Z_T)/R

*
* 
************    Optical model multiplier for D+X systems:

        X1=2.83-(3.1*A_T/100.)+(1.7*(A_T**2)/10000.)

        S_L=1.2+1.6*(1-exp(-E_k/15.))
*
*
************************* Parameter X_m :
*
        X_m=1-(X1*(exp(-E_k/(X1*S_L)))) 
*
*
* ***********

       if (A_T.eq.2.0.and.Z_T.eq.1.) THEN     ! d+d 
        R_C=13.5
       endif

       if (A_T.eq.4.0.and.Z_T.eq.2.) THEN     ! d+He4 
        R_C=13.5
       endif

       if (A_T.eq.12.0.and.Z_T.eq.6.) THEN     ! d+C 
        R_C=6.0
       endif



*
*
* ****** FINALLY THE TOTAL CROSS SECTION sigma_T in  "mbarn" : 


         sigma_T=10.*PI*(r0**2)*(((A_P**(1./3.))+(A_T**(1./3.)
     +               )+dE)**2)*(1-(R_C*B/E_cm))*X_m

          if (sigma_T.lt.0.) sigma_T=0.
          if (sigma_T.eq.0.) IFLAG=0


 66       CONTINUE
          RETURN
          END
********************************
*** density of a nucleus in the hard sphere model: rho_av=(rho_Ap + rho_At)/2 
*   where the density of nucleus projectile rho_Ap and nucleus target rho_At 
*   are : 
*
*    rho_Ai=Ai/((4*PI/3)*r_i**3)
*
*   where the radius of the nucleus r_i is
*
*    r_i=1.29*(r_i)_rms
*
*   It is provided by FUNCTION RADIUS
*
*
******************************************************************
*
        FUNCTION RADIUS(A)
        DIMENSION NA(23),RMS(23)
        DATA NA/1,2,3,4,6,7,9,10,11,12,13,14,15,16,17,18,
     +         19,20,22,23,24,25,26/
        DATA RMS/0.85,2.095,1.976,1.671,2.57,2.41,2.519,
     +          2.45,2.42,
     +          2.471,2.440,2.58,2.611,2.730,2.662,
     +          2.727,2.900,
     +          3.040,2.969,2.94,3.075,3.11,3.06/
        FACT=SQRT(5./3.)
        IA=A+0.4
        RADIUS=FACT*(0.84*A**(1./3.)+0.55)
        DO 1 I=1,23
        IF (IA.EQ.NA(I)) GO TO 2
        GO TO 1
 2      RADIUS=FACT*RMS(I)
 1      CONTINUE
        RETURN
        END




cccccccccccccccccccccccccccccccccccccccccccccccccccccccccccc

CDECK  ID>, ABCROSS.
       subroutine abcross(icas,a_p,a_t,z_pb,z_t,p_p,
     +  sigma_t,sigma_el,sigma_q)
        real *4 sigma_t,sigma_el(3),sigma_q(5)
CDE, ABC.
*
*     Input:
*     icase (normally should be 1)
*     a_p,a_t,z_p,z_t,p_p [atom number projectile & target,Z p & t
*      and momentom of projectile [Gev/c]]
*
*     Output
*
*     sigma_t=total cr_sec [mbarn]
*     sigma_el(3) = sigma elastic [mbarn]
*                   slope [gev/c]^-2
*                   reserved
*     sigma_q(5)  = sigma quasi elastic [mbarn]
*                   sigma q -el (1)
*                   slope    [gev/c]^-2   (1)
*                   slope    [gev/c]^-2   (2)
*                   reserved
*
       real al_d(6)
       data al_d/1.91,1.66,3*0.,1.4/
       data al_par /0.225/
       p_pn=p_p/a_p
       z_p=abs(z_pb)
       if(z_pb.gt.0)then
       snn=sig_pn(1,p_pn)*(z_p*z_t+(a_p-z_p)*(a_t-z_t))
     + +sig_pn(2,p_pn)*((a_p-z_p)*z_t+z_p*(a_t-z_t))
       else
       snn=sig_pn(3,p_pn)*(z_p*z_t+(a_p-z_p)*(a_t-z_t))
     + +sig_pn(4,p_pn)*((a_p-z_p)*z_t+z_p*(a_t-z_t))
       snnel=sig_pn(5,p_pn)*(z_p*z_t+(a_p-z_p)*(a_t-z_t))
     + +sig_pn(6,p_pn)*((a_p-z_p)*z_t+z_p*(a_t-z_t))
       snnel=snnel/a_p/a_t
       endif
       snn=snn/a_p/a_t
       icase=1

       call vzero(sigma_el(1),3)
       call vzero(sigma_q(1),5)
       sigma_t=0
       if(icase.eq.1)then
*
* Factorization
*
        iap=a_p+0.5
        iat=a_t+0.5
       if(iap.ne.2.and.iap.ne.4.and.iap.ne.12)then
        alpha_p=2./3+4./3/a_p**al_par
       else
        inum=iap/2
        alpha_p=al_d(inum)
       endif
       if(iat.ne.2.and.iat.ne.4.and.iat.ne.12)then
        alpha_t=2./3+4./3/a_t**al_par
       else
        inum=iat/2
        alpha_t=al_d(inum)
       endif
       if(a_p.gt.a_t)then
        a_l=a_t
        a_h=a_p
        alpha_l=alpha_t
        alpha_h=alpha_p
       else
        a_l=a_p
        a_h=a_t
        alpha_l=alpha_p
        alpha_h=alpha_t
       endif
        alpha_l=0.5*alpha_l
        alpha_h=0.5*alpha_h
       rr=a_l/a_h
       alpha_hh=rr*alpha_h+(1-rr)*alpha_h**0.5
       sigma_t=snn*a_h**alpha_hh*a_l**alpha_l
       sigma_t_2=144*(a_p**0.33334+a_t**0.33334-1.48)**2*snn/44
       if(sigma_t_2.gt.sigma_t.and.a_p+a_t.gt.10)then
        sigma_t=(sigma_t+sigma_t_2)/2.
       endif
*
* bnn
*
100    continue
       bnn=fb_ab(p_pn)
       ab=0.53
       b_ab=2.2*bnn*(a_t*a_p)**0.53
       cts=0.389*16*3.14159
       dsdt=sigma_t**2/cts
       sigma_el(1)=dsdt/b_ab
       sigma_el(2)=b_ab
       if(sigma_el(1).ge.sigma_t)then
*
* unitarity viol
*
        sigma_el(1)=sigma_t
       endif
       if(z_pb.le.0.and.p_pn.le.20.)then
        iap=a_p+0.5
        iat=a_t+0.5
       if(iap.ne.2.and.iap.ne.4.and.iap.ne.12)then
        alpha_p=2./3+4./3/a_p**al_par
       else
        inum=iap/2
        alpha_p=al_d(inum)
       endif
       if(iat.ne.2.and.iat.ne.4.and.iat.ne.12)then
        alpha_t=2./3+4./3/a_t**al_par
       else
        inum=iat/2
        alpha_t=al_d(inum)
       endif
       if(a_p.gt.a_t)then
        a_l=a_t
        a_h=a_p
        alpha_l=alpha_t
        alpha_h=alpha_p
       else
        a_l=a_p
        a_h=a_t
        alpha_l=alpha_p
        alpha_h=alpha_t
       endif
        alpha_l=0.5*alpha_l
        alpha_h=0.5*alpha_h
       rr=a_l/a_h
       alpha_hh=rr*alpha_h+(1-rr)*alpha_h**0.5
       sigma_el(1)=snnel*a_h**alpha_hh*a_l**alpha_l
       endif
*
* quasi -elastic
* we will assume it follows the total one
*
        bq=14.3+12/a_h
        bq2=9.
        if(a_h.gt.4)then
          dsqdt=11.5*bnn*a_h**0.3333*a_l**alpha_l
          sigma_q(1)=dsqdt/bq+dsqdt/4.5/bq2
          sigma_q(2)=dsqdt/bq
          sigma_q(3)=bq
          sigma_q(4)=bq2
        else
          dsqdt=6.5*bnn*(a_h*a_l)**0.5
          sigma_q(1)=dsqdt/bq
          sigma_q(2)=dsqdt/bq
          sigma_q(3)=bq
          sigma_q(4)=bq2
        endif

       else if(icase.eq.2)then
*
* Geometrical picture
*
       sigma_t=144*(a_p**0.33334+a_t**0.33334-1.48)**2
       if(p_pn.lt.1.5)goto 100
       sigma_i=78*(a_p**0.33334+a_t**0.33334-1.25)**2
       sigma_e=sigma_t-sigma_i
*
* bnn
*

       bnn=fb_ab(p_pn)
       ab=0.53
       b_ab=2.2*bnn*(a_t*a_p)**0.53
       cts=0.389*16*3.14159
       dsdt=sigma_t**2/cts
       sigma_el(1)=sigma_e
       sigma_el(2)=dsdt/sigma_e
       endif
       end
        function fb_ab(p)
         parameter (thr=0.)
        if(p.gt.thr)then
         fb_ab=6.3+1.03*log(p)
        else
         fb_ab=0.
        endif
       end
CDECK  ID>, SIG_PN. 
       function sig_pn(icase,pmom)
*
*  return proton-proton (icase=1) or proton-neutron (icase=2)
*  p-ap icase=3 p-an icase=4
*  total cross section in mbarn vs pmom in Gev/c
*

        if(icase.eq.1)then
* proton
         if(pmom.ge.1.5)then
          sig_pn=48+0.522*(log(pmom))**2-4.51*log(pmom)
         else if(pmom.ge.1.)then
           sig_pn=25*exp(1.23*(pmom-1))
         else if(pmom.ge.0.34)then
           sig_pn=25
         else if (pmom.ge.0.1)then
           sig_pn=25*exp(12.5*(0.34-pmom))
         else
           sig_pn=500.
         endif
        else if(icase.eq.2)then
* neutron
         if(pmom.ge.3.)then
          sig_pn=47.3+0.51*(log(pmom))**2-4.27*log(pmom)
         else if(pmom.ge.1.)then
          sig_pn=40.2+1.5*(pmom-1)
         else if(pmom.ge.0.34)then
          sig_pn=40.2*exp(0.85*(1.-pmom))
         else if(pmom.ge.0.1)then
           sig_pn=70*exp(12.5*(0.34-pmom))
         else
           sig_pn=1400
         endif
        else if(icase.eq.3)then
        sig_pn=38.4+77.6*pmom**(-0.64)+0.26*(log(pmom))**2-
     +  1.2*log(pmom)
        else if(icase.eq.4)then
         sig_pn=133.6*pmom**(-0.7)-1.22*(log(pmom))**2+
     +   13.7*log(pmom)
        else if(icase.eq.5)then
        sig_pn=10.2+52.7*pmom**(-1.16)+0.125*(log(pmom))**2-
     +  1.28*log(pmom)
        else if(icase.eq.6)then
         sig_pn=36.5-11.9*log(pmom)
        else
         write(*,*)'wrong icase'
         stop
        endif
       end
CDECK  ID>, ABSIEL. 
       subroutine absiel(pmom_p,pmom_t,iuu)
*
* To generate elastic scattering
*
       parameter (inmat=1000)
       common /abcrossC/sigma_t(inmat),sigma_el(3,inmat),
     +  sigma_q(5,inmat),abinx(inmat),abelx(inmat),abqex(inmat)
        common /abcr2C/qexnor,anxnor,elxnor,sine,sela,sqel,fsig
       real *4 pmom_p(5),u(3),pmom_t(6)
       xp=pmom_p(4)
       phi=2*3.14159*rndm(d)
10       q2=-log(rndm(d))/sigma_el(2,iuu)  !q^2

       xm1=pmom_p(5)
       xm2=pmom_t(5)
       call abkin(xm1,xm2,q2,xp,p1,p1_per,ierr)
       if(ierr.ne.0)goto 10
       sint=p1_per/p1         ! sin(theta)
       pmom_p(4)=p1
       sint=min(sint,1.)
       sint=max(sint,-1.)
       t=asin(sint)
       q=sqrt(q2)
       pmom_t(4)=q
       pmom_t(6)=sqrt(q2+pmom_t(5)**2)-pmom_t(5)

       call conv_r_p(pmom_p(1),pmom_p(2),pmom_p(3),th,ph,r)
       u(3)=1.
       u(2)=0.
       u(1)=0.
       call rotate(t,2,u(1),u(2),u(3),tst)
       call rotate(phi,3,u(1),u(2),u(3),tst)
       call rotate(th,2,u(1),u(2),u(3),tst)
       call rotate(ph,3,u(1),u(2),u(3),tst)
       pmom_t(1)=pmom_p(1)*xp-u(1)*p1
       pmom_t(2)=pmom_p(2)*xp-u(2)*p1
       pmom_t(3)=pmom_p(3)*xp-u(3)*p1
       ds=pmom_t(1)**2+pmom_t(2)**2+pmom_t(3)**2
       ds=sqrt(ds)
       if(ds.eq.0)ds=1.
       do kk=1,3
        pmom_t(kk)=pmom_t(kk)/ds
       enddo

       call ucopy(u(1),pmom_p(1),3)
       end
CDECK  ID>, ABSIIN. 
       subroutine absiin(a_p,z_p,a_t,z_t,pmom_p,nsec,p_sec,ierr)
*        input
*        pmom_p(5)   u,v,w,etotal,mass
*        a_p,t atomic weight
*        z_p,t 
*        output p_sec(6,1000)   // as in gkin
         real *4 p_sec(6,*)
         real *4 u(3),pmom_p(5)
         a_p=int(a_p+0.5)
         ekin=(pmom_p(4)-pmom_p(5))/a_p
*
*   call to rqmd
*
         call call_rqmd(a_p,z_p,a_t,z_t,ekin,nsec,p_sec,ierr)
         if(ierr.ne.0)then
             nsec=0
             goto 999
         endif
*
* Rotate back
*

c         px=0
c         py=0
c         pz=0
c         ptot=sqrt((pmom_p(4)-pmom_p(5))*(pmom_p(4)+pmom_p(5)))
         call conv_r_p(pmom_p(1),pmom_p(2),pmom_p(3),th,ph,r)
c         u(1)=0
c         u(2)=0
c         u(3)=ptot
c          call rotate(th,2,u(1),u(2),u(3),tst)
c          call rotate(ph,3,u(1),u(2),u(3),tst)
c           write(*,*)u
         do i=1,nsec
          call rotate(th,2,p_sec(1,i),p_sec(2,i),p_sec(3,i),tst)
          call rotate(ph,3,p_sec(1,i),p_sec(2,i),p_sec(3,i),tst)
*
* Check conserv
*      
c          px=px+p_sec(1,i)
c          py=py+p_sec(2,i)
c          pz=pz+p_sec(3,i)
         enddo
c         write(*,*)'*** secondary *****'
c         write(*,*)px,py,pz,pmom_p(1)*ptot,
c     +    pmom_p(2)*ptot,pmom_p(3)*ptot

*
 999     end

CDECK  ID>, ABSIQE. 
       subroutine absiqe(pmom_p,pmom_t,iuu)
*
* Here quasi elastic cross
*
       parameter (inmat=1000)
       common /abcrossC/sigma_t(inmat),sigma_el(3,inmat),
     +  sigma_q(5,inmat),abinx(inmat),abelx(inmat),abqex(inmat)
        common /abcr2C/qexnor,anxnor,elxnor,sine,sela,sqel,fsig
       real *4 pmom_p(5),u(3),pmom_t(6)
*
*     choose between two exponents.
*
       x1=sigma_q(2,iuu)/sigma_q(1,iuu)
       if(rndm(d).lt.x1)then
        bt=sigma_q(3,iuu)
       else
        bt=sigma_q(4,iuu)
       endif
       xp=pmom_p(4)
       phi=2*3.14159*rndm(d)
10       q2=-log(rndm(d))/bt  !q^2
       xm1=pmom_p(5)
       xm2=pmom_t(5)
       call abkin(xm1,xm2,q2,xp,p1,p1_per,ierr)
       if(ierr.ne.0)goto 10
       sint=p1_per/p1         ! sin(theta)
       pmom_p(4)=p1
       sint=min(sint,1.)
       sint=max(sint,-1.)
       t=asin(sint)
       pmom_t(4)=sqrt(q2)
       q=sqrt(q2)
       pmom_t(6)=sqrt(q2+pmom_t(5)**2)-pmom_t(5)
       call conv_r_p(pmom_p(1),pmom_p(2),pmom_p(3),th,ph,r)
       u(3)=1.
       u(2)=0.
       u(1)=0.
       call rotate(t,2,u(1),u(2),u(3),tst)
       call rotate(phi,3,u(1),u(2),u(3),tst)
       call rotate(th,2,u(1),u(2),u(3),tst)
       call rotate(ph,3,u(1),u(2),u(3),tst)
       pmom_t(1)=pmom_p(1)*xp-u(1)*p1
       pmom_t(2)=pmom_p(2)*xp-u(2)*p1
       pmom_t(3)=pmom_p(3)*xp-u(3)*p1
       ds=pmom_t(1)**2+pmom_t(2)**2+pmom_t(3)**2
       ds=sqrt(ds)
       if(ds.eq.0)ds=1.
       do kk=1,3
        pmom_t(kk)=pmom_t(kk)/ds
       enddo
       call ucopy(u(1),pmom_p(1),3)
       end
CDECK  ID>, ABKIN.  
            subroutine abkin(m1,m2,q2,p,p1,p1_per,ierr)
            real *4 m1,m2,p,p1,p1_per
            real *8 g,e,p1d,p1_par,pd,q2d,m1d,m2d
*
*   input:
* m1,m2  - masses of nuclea
* q2 -  q^2
* p -proj momentun
*   output:
* p1 -proj momentum after scatt
* p1_per proj mom perp
*
            q2d=q2
            pd=p
            ierr=0
            m1d=m1
            m2d=m2
            e=sqrt(pd**2+m1d**2)
            g=e+m2
            p1d=g**2+q2d+m2d**2-m1d**2-2*g*sqrt(q2d+m2d**2)
             if(p1d.lt.0)p1d=0.
             p1d=sqrt(p1d)
            p1=p1d
            p1_par=pd/2+p1d/pd/2*p1d-q2d/2/pd
            if(abs(p1d).lt.abs(p1_par))then
             write(*,*)' ABKIN - too big q^2 requested',q2
             ierr=1
            else
             p1_per=sqrt((p1d-p1_par)*(p1d+p1_par))
            endif
            end
CDECK  ID>, ABLORZ. 
      SUBROUTINE ROTATE (A,IAXE,PX,PY,PZ,TEST)

      IMPLICIT NONE

      INTEGER IAXE
      REAL    V(3) , A , PX , PY , PZ
      LOGICAL TEST

      TEST = .FALSE.

      IF (IAXE.EQ.1)THEN
         V(1) = PX
         V(2) = PY*COS(A) - PZ*SIN(A)
         V(3) = PY*SIN(A) + PZ*COS(A)
         TEST = .TRUE.
         GOTO 100
      ENDIF
      IF (IAXE.EQ.2)THEN
         V(1) = PZ*SIN(A) + PX*COS(A)
         V(2) = PY
         V(3) = PZ*COS(A) - PX*SIN(A)
         TEST = .TRUE.
         GOTO 100
      ENDIF
      IF (IAXE.EQ.3)THEN
         V(1) = PX*COS(A) - PY*SIN(A)
         V(2) = PX*SIN(A) + PY*COS(A)
         V(3) = PZ
         TEST = .TRUE.
         GOTO 100
      ENDIF

100   PX = V(1)
      PY = V(2)
      PZ = V(3)

      RETURN
      END
      SUBROUTINE BOOST_ALONG_Z (B , E , PZ , Q)

      IMPLICIT NONE
      REAL    B,G,E,PZ,E_DUM
      LOGICAL Q

      Q = .TRUE.
      IF (ABS(B).GT.1.0.OR.E.LT.ABS(PZ)) THEN
         Q = .FALSE.
         RETURN
      ENDIF

      G = SQRT(1/(1-B**2))
      E_DUM = E
      E     = G * (   E_DUM - B*PZ)
      PZ    = G * (-B*E_DUM + PZ)

      RETURN
      END
      SUBROUTINE CONV_R_P (X,Y,Z,T,P,R)

      IMPLICIT NONE

      LOGICAL  Q
      REAL     X,Y,Z,T,P,R,PI
      PARAMETER (PI = 3.14159)

      Q = .TRUE.
      R = SQRT (X**2 + Y**2 + Z**2)
      IF (ABS(R).LT.1E-30) THEN
         T = 0.0
         P = 0.0
         RETURN
      ENDIF
      T = ACOS (Z/R)
      IF (ABS(X).LT.1E-30) THEN
         P = SIGN (PI/2 ,Y)
      ELSE
         P = ATAN (Y/X)
         IF (X.LT.0.0) P = P + 3.14159
      ENDIF

      RETURN
      END
      SUBROUTINE POL_REC ( P,THE,PHI,PX,PY,PZ )

      IMPLICIT NONE
      REAL     P,THE,PHI , PX,PY,PZ

      PX = P * SIN(THE) * COS(PHI)
      PY = P * SIN(THE) * SIN(PHI)
      PZ = P * COS(THE)

      RETURN
      END
CDECK  ID>, ABDIST. 
       subroutine abdist
      COMMON/GCFLAG/IDEBUG,IDEMIN,IDEMAX,ITEST,IDRUN,IDEVT,IEORUN
     +        ,IEOTRI,IEVENT,ISWIT(10),IFINIT(20),NEVENT,NRNDM(2)
      COMMON/GCFLAX/BATCH, NOLOG
      LOGICAL BATCH, NOLOG
C
      INTEGER       IDEBUG,IDEMIN,IDEMAX,ITEST,IDRUN,IDEVT,IEORUN
     +        ,IEOTRI,IEVENT,ISWIT,IFINIT,NEVENT,NRNDM
C
      INTEGER IQ,LQ,NZEBRA,IXSTOR,IXDIV,IXCONS,LMAIN,LR1,JCG
      INTEGER KWBANK,KWWORK,IWS
      REAL GVERSN,ZVERSN,FENDQ,WS,Q
C
      PARAMETER (KWBANK=69000,KWWORK=5200)
      COMMON/GCBANK/NZEBRA,GVERSN,ZVERSN,IXSTOR,IXDIV,IXCONS,FENDQ(16)
     +             ,LMAIN,LR1,WS(KWBANK)
      DIMENSION IQ(2),Q(2),LQ(8000),IWS(2)
      EQUIVALENCE (Q(1),IQ(1),LQ(9)),(LQ(1),LMAIN),(IWS(1),WS(1))
      EQUIVALENCE (JCG,JGSTAT)
      INTEGER       JDIGI ,JDRAW ,JHEAD ,JHITS ,JKINE ,JMATE ,JPART
     +      ,JROTM ,JRUNG ,JSET  ,JSTAK ,JGSTAT,JTMED ,JTRACK,JVERTX
     +      ,JVOLUM,JXYZ  ,JGPAR ,JGPAR2,JSKLT
C
      COMMON/GCLINK/JDIGI ,JDRAW ,JHEAD ,JHITS ,JKINE ,JMATE ,JPART
     +      ,JROTM ,JRUNG ,JSET  ,JSTAK ,JGSTAT,JTMED ,JTRACK,JVERTX
     +      ,JVOLUM,JXYZ  ,JGPAR ,JGPAR2,JSKLT
C
      COMMON/GCKINE/IKINE,PKINE(10),ITRA,ISTAK,IVERT,IPART,ITRTYP
     +      ,NAPART(5),AMASS,CHARGE,TLIFE,VERT(3),PVERT(4),IPAOLD
C
      INTEGER       IKINE,ITRA,ISTAK,IVERT,IPART,ITRTYP,NAPART,IPAOLD
      REAL          PKINE,AMASS,CHARGE,TLIFE,VERT,PVERT
C
      INTEGER NMEC,LMEC,NAMEC,NSTEP ,MAXNST,IGNEXT,INWVOL,ISTOP,MAXMEC
     + ,IGAUTO,IEKBIN,ILOSL, IMULL,INGOTO,NLDOWN,NLEVIN,NLVSAV,ISTORY
     + ,MAXME1,NAMEC1
      REAL  VECT,GETOT,GEKIN,VOUT,DESTEP,DESTEL,SAFETY,SLENG ,STEP
     + ,SNEXT,SFIELD,TOFG  ,GEKRAT,UPWGHT
      REAL POLAR
      PARAMETER (MAXMEC=30)
      COMMON/GCTRAK/VECT(7),GETOT,GEKIN,VOUT(7),NMEC,LMEC(MAXMEC)
     + ,NAMEC(MAXMEC),NSTEP ,MAXNST,DESTEP,DESTEL,SAFETY,SLENG
     + ,STEP  ,SNEXT ,SFIELD,TOFG  ,GEKRAT,UPWGHT,IGNEXT,INWVOL
     + ,ISTOP ,IGAUTO,IEKBIN, ILOSL, IMULL,INGOTO,NLDOWN,NLEVIN
     + ,NLVSAV,ISTORY
      PARAMETER (MAXME1=30)
      COMMON/GCTPOL/POLAR(3), NAMEC1(MAXME1)
C
      COMMON/GCMATE/NMAT,NAMATE(5),A,Z,DENS,RADL,ABSL
C
      INTEGER NMAT,NAMATE
      REAL A,Z,DENS,RADL,ABSL
C
      DOUBLE PRECISION PI,TWOPI,PIBY2,DEGRAD,RADDEG,CLIGHT,BIG,EMASS
      DOUBLE PRECISION EMMU,PMASS,AVO
*
      PARAMETER (PI=3.14159265358979324D0)
      PARAMETER (TWOPI=6.28318530717958648D0)
      PARAMETER (PIBY2=1.57079632679489662D0)
      PARAMETER (DEGRAD=0.0174532925199432958D0)
      PARAMETER (RADDEG=57.2957795130823209D0)
      PARAMETER (CLIGHT=29979245800.D0)
      PARAMETER (BIG=10000000000.D0)
      PARAMETER (EMASS=0.0005109990615D0)
      PARAMETER (EMMU=0.105658387D0)
      PARAMETER (PMASS=0.9382723128D0)
      PARAMETER (AVO=0.60221367D0)
*
      COMMON/GCPHYS/IPAIR,SPAIR,SLPAIR,ZINTPA,STEPPA
     +             ,ICOMP,SCOMP,SLCOMP,ZINTCO,STEPCO
     +             ,IPHOT,SPHOT,SLPHOT,ZINTPH,STEPPH
     +             ,IPFIS,SPFIS,SLPFIS,ZINTPF,STEPPF
     +             ,IDRAY,SDRAY,SLDRAY,ZINTDR,STEPDR
     +             ,IANNI,SANNI,SLANNI,ZINTAN,STEPAN
     +             ,IBREM,SBREM,SLBREM,ZINTBR,STEPBR
     +             ,IHADR,SHADR,SLHADR,ZINTHA,STEPHA
     +             ,IMUNU,SMUNU,SLMUNU,ZINTMU,STEPMU
     +             ,IDCAY,SDCAY,SLIFE ,SUMLIF,DPHYS1
     +             ,ILOSS,SLOSS,SOLOSS,STLOSS,DPHYS2
     +             ,IMULS,SMULS,SOMULS,STMULS,DPHYS3
     +             ,IRAYL,SRAYL,SLRAYL,ZINTRA,STEPRA
      COMMON/GCPHLT/ILABS,SLABS,SLLABS,ZINTLA,STEPLA
     +             ,ISYNC
     +             ,ISTRA
*
      INTEGER IPAIR,ICOMP,IPHOT,IPFIS,IDRAY,IANNI,IBREM,IHADR,IMUNU
     +       ,IDCAY,ILOSS,IMULS,IRAYL,ILABS,ISYNC,ISTRA
      REAL    SPAIR,SLPAIR,ZINTPA,STEPPA,SCOMP,SLCOMP,ZINTCO,STEPCO
     +       ,SPHOT,SLPHOT,ZINTPH,STEPPH,SPFIS,SLPFIS,ZINTPF,STEPPF
     +       ,SDRAY,SLDRAY,ZINTDR,STEPDR,SANNI,SLANNI,ZINTAN,STEPAN
     +       ,SBREM,SLBREM,ZINTBR,STEPBR,SHADR,SLHADR,ZINTHA,STEPHA
     +       ,SMUNU,SLMUNU,ZINTMU,STEPMU,SDCAY,SLIFE ,SUMLIF,DPHYS1
     +       ,SLOSS,SOLOSS,STLOSS,DPHYS2,SMULS,SOMULS,STMULS,DPHYS3
     +       ,SRAYL,SLRAYL,ZINTRA,STEPRA,SLABS,SLLABS,ZINTLA,STEPLA
C
      COMMON/GCJLOC/NJLOC(2),JTM,JMA,JLOSS,JPROB,JMIXT,JPHOT,JANNI
     +                  ,JCOMP,JBREM,JPAIR,JDRAY,JPFIS,JMUNU,JRAYL
     +                  ,JMULOF,JCOEF,JRANG
C
      INTEGER       NJLOC   ,JTM,JMA,JLOSS,JPROB,JMIXT,JPHOT,JANNI
     +                  ,JCOMP,JBREM,JPAIR,JDRAY,JPFIS,JMUNU,JRAYL
     +                  ,JMULOF,JCOEF,JRANG
C
      COMMON/GCJLCK/NJLCK(2),JTCKOV,JABSCO,JEFFIC,JINDEX,JCURIN
     +                      ,JPOLAR,JTSTRA,JTSTCO,JTSTEN,JTASHO
C
      EQUIVALENCE (JLASTV,JTSTEN)
C
      INTEGER       NJLCK,JTCKOV,JABSCO,JEFFIC,JINDEX,JCURIN
     +                   ,JPOLAR,JLASTV,JTSTRA,JTSTCO,JTSTEN
     +                   ,JTASHO
C
       parameter (inmat=1000)
       common /abcrossC/sigma_t(inmat),sigma_el(3,inmat),
     +  sigma_q(5,inmat),abinx(inmat),abelx(inmat),abqex(inmat)
        common /abcr2C/qexnor,anxnor,elxnor,sine,sela,sqel,fsig
      parameter (avogad=6.022e23)
      parameter   (avogmb=1.e27/avogad)
      parameter (xm_proton=0.938)

         ZELA  = BIG
         Zine  = BIG
         zqel=big
         a_p=amass/xm_proton
         z_p=abs(charge)
         P_p = SQRT(GEKIN*(GEKIN+2*amass))
         JMA = LQ(JMATE-NMAT)
         NCOMP = ABS(Q(JMA+11))
         DENS = Q(JMA+8)
         JMIXT = LQ(JMA-5)
         IF ( NCOMP .LE. 1) THEN
            a_t = a
            Z_t = z
            CALL abcr2(1,a_p,z_p,a_t,z_t,p_p,ipart)
            if(sigma_t(1)-sigma_el(1,1)-sigma_q(1,1).gt.0)then
             zin=a*avogmb/(sigma_t(1)-sigma_el(1,1)-sigma_q(1,1))
            else
             zin=big
            endif
            zel=a*avogmb/sigma_el(1,1)
            if(sigma_q(1,1).gt.0)then
             zqe=a*avogmb/sigma_q(1,1)
            else
             zqe=big
            endif
            IF (ZEL.LT.BIG) THEN
               ZELA = ZEL/DENS
            END IF
            IF (Zin.LT.BIG) THEN
               ZinA = Zin/DENS
            END IF
            IF (Zqe.LT.BIG) THEN
               Zqel = Zqe/DENS
            END IF

         ELSE
            zin1 = 0.
            zel1=0
            zqe1=0
            DO 10 K=1,NCOMP
               CALL abcr2(k,a_p,z_p,Q(JMIXT+K),
     +         Q(JMIXT+NCOMP+K),p_p,ipart)
            if(sigma_t(k)-sigma_el(1,k)-sigma_q(1,k).gt.0)then
            zin=q(jmixt+k)*avogmb/(sigma_t(k)-sigma_el(1,k)-
     +      sigma_q(1,k))
            else
            zin=big
            endif
            zel=q(jmixt+k)*avogmb/sigma_el(1,k)
            if(sigma_q(1,k).gt.0)then
            zqe=q(jmixt+k)*avogmb/sigma_q(1,k)
            else
            zqe=big
            endif
               IF (zin.Ge.BIG) THEN
                  ZIN1 = 0.0 + ZIN1
               ELSE
                  ZIN1 = DENS*Q(JMIXT+2*NCOMP+K)/Zin + ZIN1
               END IF
               ABINX(K) = ZIN1
               IF (zqe.Ge.BIG) THEN
                  Zqe1 = 0.0 + Zqe1
               ELSE
                  Zqe1 = DENS*Q(JMIXT+2*NCOMP+K)/Zqe + Zqe1
               END IF
               ABQEX(K) = Zqe1
               IF (zel.Ge.BIG) THEN
                  Zel1 = 0.0 + Zel1
               ELSE
                  Zel1 = DENS*Q(JMIXT+2*NCOMP+K)/Zel + Zel1
               END IF
               ABelX(K) = Zel1
   10       CONTINUE
            ANXNOR = ZIN1
            ELXNOR = ZEL1
            qexnor=zqe1
            IF (ZIN1.GT.0.0) THEN
               ZINE = 1./ZIN1
            END IF
            IF (ZEL1.GT.0.0) THEN
               ZELA = 1./ZEL1
            END IF
            IF (Zqe1.GT.0.0) THEN
               Zqel = 1./Zqe1
            END IF
         END IF
      IF (ZINE.EQ.BIG) THEN
         SINE = 0.0
      ELSE
         SINE = 1./ZINE
      END IF
      IF (ZELA.EQ.BIG) THEN
         SELA = 0.0
      ELSE
         SELA = 1./ZELA
      END IF
      IF (Zqel.EQ.BIG) THEN
         Sqel = 0.0
      ELSE
         Sqel = 1./zqel
      END IF
      FSIG = SINE + SELA +sqel
      IF (FSIG .LE. 0) THEN
         SHADR = BIG
      ELSE
         SHADR = ZINTHA/FSIG
      END IF
  999 CONTINUE
      END

CDECK  ID>, ABUFIN. 
      SUBROUTINE ABUFIN
      INTEGER IQ,LQ,NZEBRA,IXSTOR,IXDIV,IXCONS,LMAIN,LR1,JCG
      INTEGER KWBANK,KWWORK,IWS
      REAL GVERSN,ZVERSN,FENDQ,WS,Q
C
      PARAMETER (KWBANK=69000,KWWORK=5200)
      COMMON/GCBANK/NZEBRA,GVERSN,ZVERSN,IXSTOR,IXDIV,IXCONS,FENDQ(16)
     +             ,LMAIN,LR1,WS(KWBANK)
      DIMENSION IQ(2),Q(2),LQ(8000),IWS(2)
      EQUIVALENCE (Q(1),IQ(1),LQ(9)),(LQ(1),LMAIN),(IWS(1),WS(1))
      EQUIVALENCE (JCG,JGSTAT)
      INTEGER       JDIGI ,JDRAW ,JHEAD ,JHITS ,JKINE ,JMATE ,JPART
     +      ,JROTM ,JRUNG ,JSET  ,JSTAK ,JGSTAT,JTMED ,JTRACK,JVERTX
     +      ,JVOLUM,JXYZ  ,JGPAR ,JGPAR2,JSKLT
C
      COMMON/GCLINK/JDIGI ,JDRAW ,JHEAD ,JHITS ,JKINE ,JMATE ,JPART
     +      ,JROTM ,JRUNG ,JSET  ,JSTAK ,JGSTAT,JTMED ,JTRACK,JVERTX
     +      ,JVOLUM,JXYZ  ,JGPAR ,JGPAR2,JSKLT
C
      COMMON/GCCUTS/CUTGAM,CUTELE,CUTNEU,CUTHAD,CUTMUO,BCUTE,BCUTM
     +             ,DCUTE ,DCUTM ,PPCUTM,TOFMAX,GCUTS(5)
C
      REAL          CUTGAM,CUTELE,CUTNEU,CUTHAD,CUTMUO,BCUTE,BCUTM
     +             ,DCUTE ,DCUTM ,PPCUTM,TOFMAX,GCUTS
C
      COMMON/GCJLOC/NJLOC(2),JTM,JMA,JLOSS,JPROB,JMIXT,JPHOT,JANNI
     +                  ,JCOMP,JBREM,JPAIR,JDRAY,JPFIS,JMUNU,JRAYL
     +                  ,JMULOF,JCOEF,JRANG
C
      INTEGER       NJLOC   ,JTM,JMA,JLOSS,JPROB,JMIXT,JPHOT,JANNI
     +                  ,JCOMP,JBREM,JPAIR,JDRAY,JPFIS,JMUNU,JRAYL
     +                  ,JMULOF,JCOEF,JRANG
C
      COMMON/GCJLCK/NJLCK(2),JTCKOV,JABSCO,JEFFIC,JINDEX,JCURIN
     +                      ,JPOLAR,JTSTRA,JTSTCO,JTSTEN,JTASHO
C
      EQUIVALENCE (JLASTV,JTSTEN)
C
      INTEGER       NJLCK,JTCKOV,JABSCO,JEFFIC,JINDEX,JCURIN
     +                   ,JPOLAR,JLASTV,JTSTRA,JTSTCO,JTSTEN
     +                   ,JTASHO
C
      COMMON/GCFLAG/IDEBUG,IDEMIN,IDEMAX,ITEST,IDRUN,IDEVT,IEORUN
     +        ,IEOTRI,IEVENT,ISWIT(10),IFINIT(20),NEVENT,NRNDM(2)
      COMMON/GCFLAX/BATCH, NOLOG
      LOGICAL BATCH, NOLOG
C
      INTEGER       IDEBUG,IDEMIN,IDEMAX,ITEST,IDRUN,IDEVT,IEORUN
     +        ,IEOTRI,IEVENT,ISWIT,IFINIT,NEVENT,NRNDM
C
      COMMON/GCKINE/IKINE,PKINE(10),ITRA,ISTAK,IVERT,IPART,ITRTYP
     +      ,NAPART(5),AMASS,CHARGE,TLIFE,VERT(3),PVERT(4),IPAOLD
C
      INTEGER       IKINE,ITRA,ISTAK,IVERT,IPART,ITRTYP,NAPART,IPAOLD
      REAL          PKINE,AMASS,CHARGE,TLIFE,VERT,PVERT
C
      INTEGER MXGKIN
      PARAMETER (MXGKIN=100)
      COMMON/GCKING/KCASE,NGKINE,GKIN(5,MXGKIN),
     +                           TOFD(MXGKIN),IFLGK(MXGKIN)
      INTEGER       KCASE,NGKINE ,IFLGK,MXPHOT,NGPHOT
      REAL          GKIN,TOFD,XPHOT
C
      PARAMETER (MXPHOT=800)
      COMMON/GCKIN2/NGPHOT,XPHOT(11,MXPHOT)
C
      COMMON/GCKIN3/GPOS(3,MXGKIN)
      REAL          GPOS
C
      COMMON/GCMATE/NMAT,NAMATE(5),A,Z,DENS,RADL,ABSL
C
      INTEGER NMAT,NAMATE
      REAL A,Z,DENS,RADL,ABSL
C
      COMMON/GCPHYS/IPAIR,SPAIR,SLPAIR,ZINTPA,STEPPA
     +             ,ICOMP,SCOMP,SLCOMP,ZINTCO,STEPCO
     +             ,IPHOT,SPHOT,SLPHOT,ZINTPH,STEPPH
     +             ,IPFIS,SPFIS,SLPFIS,ZINTPF,STEPPF
     +             ,IDRAY,SDRAY,SLDRAY,ZINTDR,STEPDR
     +             ,IANNI,SANNI,SLANNI,ZINTAN,STEPAN
     +             ,IBREM,SBREM,SLBREM,ZINTBR,STEPBR
     +             ,IHADR,SHADR,SLHADR,ZINTHA,STEPHA
     +             ,IMUNU,SMUNU,SLMUNU,ZINTMU,STEPMU
     +             ,IDCAY,SDCAY,SLIFE ,SUMLIF,DPHYS1
     +             ,ILOSS,SLOSS,SOLOSS,STLOSS,DPHYS2
     +             ,IMULS,SMULS,SOMULS,STMULS,DPHYS3
     +             ,IRAYL,SRAYL,SLRAYL,ZINTRA,STEPRA
      COMMON/GCPHLT/ILABS,SLABS,SLLABS,ZINTLA,STEPLA
     +             ,ISYNC
     +             ,ISTRA
*
      INTEGER IPAIR,ICOMP,IPHOT,IPFIS,IDRAY,IANNI,IBREM,IHADR,IMUNU
     +       ,IDCAY,ILOSS,IMULS,IRAYL,ILABS,ISYNC,ISTRA
      REAL    SPAIR,SLPAIR,ZINTPA,STEPPA,SCOMP,SLCOMP,ZINTCO,STEPCO
     +       ,SPHOT,SLPHOT,ZINTPH,STEPPH,SPFIS,SLPFIS,ZINTPF,STEPPF
     +       ,SDRAY,SLDRAY,ZINTDR,STEPDR,SANNI,SLANNI,ZINTAN,STEPAN
     +       ,SBREM,SLBREM,ZINTBR,STEPBR,SHADR,SLHADR,ZINTHA,STEPHA
     +       ,SMUNU,SLMUNU,ZINTMU,STEPMU,SDCAY,SLIFE ,SUMLIF,DPHYS1
     +       ,SLOSS,SOLOSS,STLOSS,DPHYS2,SMULS,SOMULS,STMULS,DPHYS3
     +       ,SRAYL,SLRAYL,ZINTRA,STEPRA,SLABS,SLLABS,ZINTLA,STEPLA
C
      INTEGER NMEC,LMEC,NAMEC,NSTEP ,MAXNST,IGNEXT,INWVOL,ISTOP,MAXMEC
     + ,IGAUTO,IEKBIN,ILOSL, IMULL,INGOTO,NLDOWN,NLEVIN,NLVSAV,ISTORY
     + ,MAXME1,NAMEC1
      REAL  VECT,GETOT,GEKIN,VOUT,DESTEP,DESTEL,SAFETY,SLENG ,STEP
     + ,SNEXT,SFIELD,TOFG  ,GEKRAT,UPWGHT
      REAL POLAR
      PARAMETER (MAXMEC=30)
      COMMON/GCTRAK/VECT(7),GETOT,GEKIN,VOUT(7),NMEC,LMEC(MAXMEC)
     + ,NAMEC(MAXMEC),NSTEP ,MAXNST,DESTEP,DESTEL,SAFETY,SLENG
     + ,STEP  ,SNEXT ,SFIELD,TOFG  ,GEKRAT,UPWGHT,IGNEXT,INWVOL
     + ,ISTOP ,IGAUTO,IEKBIN, ILOSL, IMULL,INGOTO,NLDOWN,NLEVIN
     + ,NLVSAV,ISTORY
      PARAMETER (MAXME1=30)
      COMMON/GCTPOL/POLAR(3), NAMEC1(MAXME1)
C
      COMMON/GSECTI/ AIEL(20),AIIN(20),AIFI(20),AICA(20),ALAM,K0FLAG
      INTEGER K0FLAG
      REAL AIEL,AIIN,AIFI,AICA,ALAM
C
      COMMON/GCTMED/NUMED,NATMED(5),ISVOL,IFIELD,FIELDM,TMAXFD,STEMAX
     +      ,DEEMAX,EPSIL,STMIN,CFIELD,PREC,IUPD,ISTPAR,NUMOLD
      COMMON/GCTLIT/THRIND,PMIN,DP,DNDL,JMIN,ITCKOV,IMCKOV,NPCKOV
C
      INTEGER       NUMED,NATMED,ISVOL,IFIELD,IUPD,ISTPAR,NUMOLD
      REAL          FIELDM,TMAXFD,STEMAX,DEEMAX,EPSIL,STMIN,CFIELD,PREC
      INTEGER       JMIN,NPCKOV,IMCKOV,ITCKOV
      REAL          THRIND,PMIN,DP,DNDL
C
      COMMON/GCUNIT/LIN,LOUT,NUNITS,LUNITS(5)
      INTEGER LIN,LOUT,NUNITS,LUNITS
      COMMON/GCMAIL/CHMAIL
      CHARACTER*132 CHMAIL
C
       parameter (inmat=1000)
       common /abcrossC/sigma_t(inmat),sigma_el(3,inmat),
     +  sigma_q(5,inmat),abinx(inmat),abelx(inmat),abqex(inmat)
        common /abcr2C/qexnor,anxnor,elxnor,sine,sela,sqel,fsig
       parameter (maxs=200)
       common /aboutC/iseco,e_r(maxs),a_r(maxs)
CDE, FCONS.

      parameter (xm_proton=0.938)
              common /hvsec/ihvse(2)
      REAL RNDMm(1)
      real *4 pmom_p(5),pmom_t(6)
      parameter (nsecm=1000)
      real *4 p_sec(6,nsecm)
*
      NP = 0
*
      INT=0
      EKE = GEKIN
      TXI = VECT(4)
      TYI = VECT(5)
      TZI = VECT(6)
      DMOD = 1./SQRT(TXI**2+TYI**2+TZI**2)
      TXI = TXI*DMOD
      TYI = TYI*DMOD
      TZI = TZI*DMOD
      WE  = 1.
      JMA = LQ(JMATE-NMAT)
      NCOMP = ABS (Q(JMA+11))
      AMM = Q(JMA+6)
      JMIXT = LQ(JMA-5)
       POO=SQRT(EKE*(EKE+2*AMass))
c      IF (GEKIN.LE.CUTHAD.or.poo/amass.le.ddcutt)then
      IF (GEKIN.LE.CUTHAD)then
         DESTEP = DESTEP + GEKIN
         GEKIN  = 0.
         GETOT  = AMASS
         VECT(7) = 0.
         ISTOP = 1
         GO TO 110
      ENDIF
*
         pmom_p(1)=txi
         pmom_p(2)=tyi
         pmom_p(3)=tzi
         pmom_p(4)=poo
         pmom_p(5)=amass
888      CALL GRNDM(RNDmM,1)
c      RNDEVT=RNDmM(1)
      RNDEVT=RNDm(dd)
      IF ( RNDEVT .le. Sela/FSIG) THEN

         INT=1
c         iiel=iiel+1
c         write(*,*)'el',iiel,sela/fsig
         IF(NCOMP.LE.1) THEN
            pmom_t(5)=amm*xm_proton
            zmed=z
            CALL absiel(pmom_p,pmom_t,1)
*
* ! here for secondaries --- later
*

         ELSE
c            CALL GRNDM(RNDmM,1)
            RCONT=ELXNOR*RNDm(dd)
            DO 40  I=1,NCOMP
               IF(RCONT.LE.ABELX(I)) GO TO 50
   40       CONTINUE
            I=NCOMP
   50       CONTINUE
            pmom_t(5)=q(jmixt+i)*xm_proton
            zmed=Q(JMIXT+NCOMP+i)
            call absiel(pmom_p,pmom_t,i)
         END IF
        else IF ( RNDEVT .le. (Sela+sqel)/FSIG) THEN
         INT=2
c         iqi=iqi+1
c         write(*,*)iqi,(sela+sqel)/fsig
         IF(NCOMP.LE.1) THEN
            pmom_t(5)=amm*xm_proton
            zmed=z
            CALL absiqe(pmom_p,pmom_t,1)
*
* ! here for secondaries --- later
*

         ELSE
            CALL GRNDM(RNDMm,1)
            RCONT=qeXNOR*RNDM(dd)
            DO 400  I=1,NCOMP
               IF(RCONT.LE.ABqeX(I)) GO TO 500
  400       CONTINUE
            I=NCOMP
  500       CONTINUE
            zmed=Q(JMIXT+NCOMP+i)
            pmom_t(5)=q(jmixt+i)*xm_proton
            call absiqe(pmom_p,pmom_t,i)
         END IF
      ELSE
c           goto 888
            int=3
            if(ihvse(2).lt.2)call abinelset(vect,ipart)
            if(ihvse(2).eq.1)then
             ISTOP = 1
             DESTEP = DESTEP + GEKIN
            else if(ihvse(2).eq.0)then
             GO TO 110
            else
*
* here inelastic kinematics will play
*
             ISTOP = 1
             IF(NCOMP.LE.1) THEN
               a_t = a
               Z_t = z
               pmom_t(5)=amm*xm_proton
               zmed=z
             ELSE
              CALL GRNDM(RNDMm,1)
              RCONT=qeXNOR*RNDM(dd)
              DO   I=1,NCOMP
               IF(RCONT.LE.ABqeX(I)) GO TO 501
              enddo
              I=NCOMP
 501          CONTINUE
              a_t=Q(JMIXT+I)
              z_t=Q(JMIXT+NCOMP+I)
              pmom_t(5)=q(jmixt+i)*xm_proton
             END IF
             a_p=amass/xm_proton
             z_p=abs(charge)
             nsec=-nsecm
             call absiin(a_p,z_p,a_t,z_t,pmom_p,nsec,p_sec,ierr)
              if(ierr.ne.0)then
                DESTEP=DESTEP+GEKIN
                write(*,*)'rqmd error ',gekin,'gev energy released' 
                nsec=0
              endif 
               if(ngkine.ge.mxgkin)then
                write(*,*)'absiin-E-Stack overflowExiting' 
                nsec=0
               endif
             do i=1,nsec
              if(p_sec(4,i)-p_sec(6,i).gt.cuthad
     +          .and.ngkine.lt.mxgkin)then
               ngkine=ngkine+1
               do j=1,5
                gkin(j,ngkine)=p_sec(j,i)
               enddo
               call gsking(ngkine)
               ngkine=ngkine-1
              else
               DESTEP = DESTEP + p_sec(4,i)-p_sec(6,i)
              endif
             enddo
            endif
      endif
   80 IF(INT.Eq.1.or.int.eq.2)then
         VECT(4)=pmom_p(1)
         VECT(5)=pmom_p(2)
         VECT(6)=pmom_p(3)
         VECT(7)=pmom_p(4)
         GETOT= (vect(7)**2+pmom_p(5)**2)**0.5
         gekin=getot-pmom_p(5)
c         if(pmom_t(4)/pmom_t(5)*xm_proton.gt.ddcutt)then
          iseco=min(iseco+1,maxs)
          e_r(iseco)=pmom_t(6)
          a_r(iseco)=pmom_t(5)
         if(pmom_t(6).gt.cuthad)then

*
*        Secondary!
*
c         write(*,*)'ABUFIN- secondary!!!',pmom_t(4),pmom_t(6)
            intt=1
            NGKINE = min(NGKINE + 1,mxgkin)
            IGEPAR = IAFPAR(zmed)
            JPA = LQ(JPART-IGEPAR)
            AGEMAS = Q(JPA+7)
            PGEANT = pmom_t(4)
            GKIN(1,NGKINE)=pmom_t(1)*PGEANT
            GKIN(2,NGKINE)=pmom_t(2)*PGEANT
            GKIN(3,NGKINE)=pmom_t(3)*PGEANT
            GKIN(4,NGKINE)=pmom_t(6)+AGEMAS
            GKIN(5,NGKINE)=IGEPAR
            TOFD(NGKINE)=0.0
            GPOS(1,NGKINE) = VECT(1)
            GPOS(2,NGKINE) = VECT(2)
            GPOS(3,NGKINE) = VECT(3)
*
       endif
      END IF
  110 CONTINUE
      ZINTHA = GARNDM(DUMMY)
      SLHADR = SLENG
      STEPHA = 1.0E10
10000 FORMAT(' **** FLUFIN: Stack overflow, ',I6,' particles lost')
10100 FORMAT(' **** FLUFIN: Stack overflow, ',I6,
     +' heavy particles lost')
  999 END
           function iafpar(zm)
           real *4 ion(54),uw(1)
           character *50 cht
           integer ipart(54)
           data ij /0/
           if(ij.eq.0)then
            ipart(1)=14
            ipart(2)=47
            do i=1,52
             ipart(i+2)=60+i
            enddo
            do i=1,54
             nwb=1
             call gfpart(ipart(i),cht,itr,aa,ion(i),tl,uw,nwb)
            enddo
               ij=1
            endif
            do i=1,54
             if(zm.le.ion(i))then
              iafpar=ipart(i)
              goto 999
             endif
            enddo
            i=54
              iafpar=ipart(i)
999           end
CDECK  ID>, GPIONS. 

      SUBROUTINE GPIONS(itrt)
C.
C.    *******************************************************************
C.    *                                                                 *
C.    *  Define a subset of the 'stable' most common elements           *
C.    *    in the Nature                                                *
C.    *                                                                 *
C.    *    ==>Called by : <USER>, UGINIT                                *
C.    *       Author    B.Grosdidier (Strasbourg) *****                 *
C.    *                                                                 *
C.    *******************************************************************
C.
      DIMENSION UBUF(1)
      DATA UBUF/0./
*
*     The Geant particle identification begin to IPART = 61
      CALL GSPART( 61,'LI6  ',itrt,   5.60305,  3., 1000., UBUF,0)
      CALL GSPART( 62,'LI7  ',itrt,   6.53536,  3., 1000., UBUF,0)
      CALL GSPART( 63,'BE7  ',itrt,   6.53622,  4., 1000., UBUF,0)
      CALL GSPART( 64,'BE9  ',itrt,   8.39479,  4., 1000., UBUF,0)
      CALL GSPART( 65,'B10  ',itrt,   9.32699,  4., 1000., UBUF,0)
      CALL GSPART( 66,'B11  ',itrt,  10.25510,  5., 1000., UBUF,0)
      CALL GSPART( 67,'C12  ',itrt,  11.17793,  6., 1000., UBUF,0)
      CALL GSPART( 68,'N14  ',itrt,  13.04378,  7., 1000., UBUF,0)
      CALL GSPART( 69,'O16  ',itrt,  14.89917,  8., 1000., UBUF,0)
      CALL GSPART( 70,'F19  ',itrt,  17.69690,  9., 1000., UBUF,0)
      CALL GSPART( 71,'NE20 ',itrt,  18.62284, 10., 1000., UBUF,0)
      CALL GSPART( 72,'NA23 ',itrt,  21.41483, 11., 1000., UBUF,0)
      CALL GSPART( 73,'MG24 ',itrt,  22.34193, 12., 1000., UBUF,0)
      CALL GSPART( 74,'AL27 ',itrt,  25.13314, 13., 1000., UBUF,0)
      CALL GSPART( 75,'SI28 ',itrt,  26.06034, 14., 1000., UBUF,0)
      CALL GSPART( 76,'P31  ',itrt,  28.85188, 15., 1000., UBUF,0)
      CALL GSPART( 77,'S32  ',itrt,  29.78180, 16., 1000., UBUF,0)
      CALL GSPART( 78,'CL35 ',itrt,  32.57328, 17., 1000., UBUF,0)
      CALL GSPART( 79,'AR36 ',itrt,  33.50356, 18., 1000., UBUF,0)
      CALL GSPART( 80,'K39  ',itrt,  36.29447, 19., 1000., UBUF,0)
      CALL GSPART( 81,'CA40 ',itrt,  37.22492, 20., 1000., UBUF,0)
      CALL GSPART( 82,'SC45 ',itrt,  41.87617, 21., 1000., UBUF,0)
      CALL GSPART( 83,'TI48 ',itrt,  44.66324, 22., 1000., UBUF,0)
      CALL GSPART( 84,'V51  ',itrt,  47.45401, 23., 1000., UBUF,0)
      CALL GSPART( 85,'CR52 ',itrt,  48.38228, 24., 1000., UBUF,0)
      CALL GSPART( 86,'MN55 ',itrt,  51.17447, 25., 1000., UBUF,0)
      CALL GSPART( 87,'FE56 ',itrt,  52.10307, 26., 1000., UBUF,0)
      CALL GSPART( 88,'CO59 ',itrt,  54.89593, 27., 1000., UBUF,0)
      CALL GSPART( 89,'NI58 ',itrt,  53.96644, 28., 1000., UBUF,0)
      CALL GSPART( 90,'CU63 ',itrt,  58.61856, 29., 1000., UBUF,0)
      CALL GSPART( 91,'ZN64 ',itrt,  59.54963, 30., 1000., UBUF,0)
      CALL GSPART( 92,'GE74 ',itrt,  68.85715, 32., 1000., UBUF,0)
      CALL GSPART( 93,'SE80 ',itrt,  74.44178, 34., 1000., UBUF,0)
      CALL GSPART( 94,'KR84 ',itrt,  78.16309, 36., 1000., UBUF,0)
      CALL GSPART( 95,'SR88 ',itrt,  81.88358, 38., 1000., UBUF,0)
      CALL GSPART( 96,'ZR90 ',itrt,  83.74571, 40., 1000., UBUF,0)
      CALL GSPART( 97,'MO98 ',itrt,  91.19832, 42., 1000., UBUF,0)
      CALL GSPART( 98,'PD106',itrt,  98.64997, 46., 1000., UBUF,0)
      CALL GSPART( 99,'CD114',itrt, 106.10997, 48., 1000., UBUF,0)
      CALL GSPART(100,'SN120',itrt, 111.68821, 50., 1000., UBUF,0)
      CALL GSPART(101,'XE132',itrt, 122.86796, 54., 1000., UBUF,0)
      CALL GSPART(102,'BA138',itrt, 128.45793, 56., 1000., UBUF,0)
      CALL GSPART(103,'CE140',itrt, 130.32111, 58., 1000., UBUF,0)
      CALL GSPART(104,'SM152',itrt, 141.51236, 62., 1000., UBUF,0)
      CALL GSPART(105,'DY164',itrt, 152.69909, 66., 1000., UBUF,0)
      CALL GSPART(106,'YB174',itrt, 162.02245, 70., 1000., UBUF,0)
      CALL GSPART(107,'W184 ',itrt, 171.34924, 74., 1000., UBUF,0)
      CALL GSPART(108,'PT194',itrt, 180.67513, 78., 1000., UBUF,0)
      CALL GSPART(109,'AU197',itrt, 183.47324, 79., 1000., UBUF,0)
      CALL GSPART(110,'HG202',itrt, 188.13451, 80., 1000., UBUF,0)
      CALL GSPART(111,'PB208',itrt, 193.72907, 82., 1000., UBUF,0)
      CALL GSPART(112,'U238 ',itrt, 221.74295, 92., 1000., UBUF,0)
      CALL GSPART(45,'DEUTERONC   ',itrt,1.875613,+1.,1.000000E+15,
     +UBUF,0)
      CALL GSPART(46,'TRITONC     ',itrt,2.80925 ,+1.,1.000000E+15,
     +UBUF,0)
      CALL GSPART(47,'ALPHAC      ',itrt,3.727417,+2.,1.000000E+15,
     +UBUF,0)
      CALL GSPART(49,'HE3C        ',itrt,2.80923 ,+2.,1.000000E+15,
     +UBUF,0)
      CALL GAPIONS(ITRT)
*
      END
      SUBROUTINE GAPIONS(ITRT)
C.
C.    *******************************************************************
C.    *                                                                 *
C.    *  Define a subset of the 'stable' most common elements           *
C.    *    in the Nature                                                *
C.    *                                                                 *
C.    *    ==>Called by : <USER>, UGINIT                                *
C.    *       Author    B.Grosdidier (Strasbourg) *****                 *
C.    *                                                                 *
C.    *******************************************************************
C.
      DIMENSION UBUF(1)
      DATA UBUF/0./
*
*     The Geant particle identification begin to IPART = 61
      CALL GSPART( 161,'ALI6  ',ITRT,   5.60305,  -3., 1000., UBUF,0)
      CALL GSPART( 162,'ALI7  ',ITRT,   6.53536,  -3., 1000., UBUF,0)
      CALL GSPART( 163,'ABE7  ',ITRT,   6.53622,  -4., 1000., UBUF,0)
      CALL GSPART( 164,'ABE9  ',ITRT,   8.39479,  -4., 1000., UBUF,0)
      CALL GSPART( 165,'AB10  ',ITRT,   9.32699,  -5., 1000., UBUF,0)
      CALL GSPART( 166,'AB11  ',ITRT,  10.25510,  -5., 1000., UBUF,0)
      CALL GSPART( 167,'AC12  ',ITRT,  11.17793,  -6., 1000., UBUF,0)
      CALL GSPART( 168,'AN14  ',ITRT,  13.04378,  -7., 1000., UBUF,0)
      CALL GSPART( 169,'AO16  ',ITRT,  14.89917,  -8., 1000., UBUF,0)
      CALL GSPART( 170,'AF19  ',ITRT,  17.69690,  -9., 1000., UBUF,0)
      CALL GSPART( 171,'ANE20 ',ITRT,  18.62284, -10., 1000., UBUF,0)
      CALL GSPART( 172,'ANA23 ',ITRT,  21.41483, -11., 1000., UBUF,0)
      CALL GSPART( 173,'AMG24 ',ITRT,  22.34193, -12., 1000., UBUF,0)
      CALL GSPART( 174,'AAL27 ',ITRT,  25.13314, -13., 1000., UBUF,0)
      CALL GSPART( 175,'ASI28 ',ITRT,  26.06034, -14., 1000., UBUF,0)
      CALL GSPART( 176,'AP31  ',ITRT,  28.85188, -15., 1000., UBUF,0)
      CALL GSPART( 177,'AS32  ',ITRT,  29.78180, -16., 1000., UBUF,0)
      CALL GSPART( 178,'ACL35 ',ITRT,  32.57328, -17., 1000., UBUF,0)
      CALL GSPART( 179,'AAR36 ',ITRT,  33.50356, -18., 1000., UBUF,0)
      CALL GSPART( 180,'AK39  ',ITRT,  36.29447, -19., 1000., UBUF,0)
      CALL GSPART( 181,'ACA40 ',ITRT,  37.22492, -20., 1000., UBUF,0)
      CALL GSPART( 182,'ASC45 ',ITRT,  41.87617, -21., 1000., UBUF,0)
      CALL GSPART( 183,'ATI48 ',ITRT,  44.66324, -22., 1000., UBUF,0)
      CALL GSPART( 184,'AV51  ',ITRT,  47.45401, -23., 1000., UBUF,0)
      CALL GSPART( 185,'ACR52 ',ITRT,  48.38228, -24., 1000., UBUF,0)
      CALL GSPART( 186,'AMN55 ',ITRT,  51.17447, -25., 1000., UBUF,0)
      CALL GSPART( 187,'AFE56 ',ITRT,  52.10307, -26., 1000., UBUF,0)
      CALL GSPART( 188,'ACO59 ',ITRT,  54.89593, -27., 1000., UBUF,0)
      CALL GSPART( 189,'ANI58 ',ITRT,  53.96644, -28., 1000., UBUF,0)
      CALL GSPART( 190,'ACU63 ',ITRT,  58.61856, -29., 1000., UBUF,0)
      CALL GSPART( 191,'AZN64 ',ITRT,  59.54963, -30., 1000., UBUF,0)
      CALL GSPART( 192,'AGE74 ',ITRT,  68.85715, -32., 1000., UBUF,0)
      CALL GSPART( 193,'ASE80 ',ITRT,  74.44178, -34., 1000., UBUF,0)
      CALL GSPART( 194,'AKR84 ',ITRT,  78.16309, -36., 1000., UBUF,0)
      CALL GSPART( 195,'ASR88 ',ITRT,  81.88358, -38., 1000., UBUF,0)
      CALL GSPART( 196,'AZR90 ',ITRT,  83.74571, -40., 1000., UBUF,0)
      CALL GSPART( 197,'AMO98 ',ITRT,  91.19832, -42., 1000., UBUF,0)
      CALL GSPART( 198,'APD106',ITRT,  98.64997, -46., 1000., UBUF,0)
      CALL GSPART( 199,'ACD114',ITRT, 106.10997, -48., 1000., UBUF,0)
      CALL GSPART(200,'ASN120',ITRT, 111.68821, -50., 1000., UBUF,0)
      CALL GSPART(201,'AXE132',ITRT, 122.86796, -54., 1000., UBUF,0)
      CALL GSPART(202,'ABA138',ITRT, 128.45793, -56., 1000., UBUF,0)
      CALL GSPART(203,'ACE140',ITRT, 130.32111, -58., 1000., UBUF,0)
      CALL GSPART(204,'ASM152',ITRT, 141.51236, -62., 1000., UBUF,0)
      CALL GSPART(205,'ADY164',ITRT, 152.69909, -66., 1000., UBUF,0)
      CALL GSPART(206,'AYB174',ITRT, 162.02245, -70., 1000., UBUF,0)
      CALL GSPART(207,'AW184 ',ITRT, 171.34924, -74., 1000., UBUF,0)
      CALL GSPART(208,'APT194',ITRT, 180.67513, -78., 1000., UBUF,0)
      CALL GSPART(209,'AAU197',ITRT, 183.47324, -79., 1000., UBUF,0)
      CALL GSPART(210,'AHG202',ITRT, 188.13451, -80., 1000., UBUF,0)
      CALL GSPART(211,'APB208',ITRT, 193.72907, -82., 1000., UBUF,0)
      CALL GSPART(212,'AU238 ',ITRT, 221.74295, -92., 1000., UBUF,0)
      CALL GSPART(145,'ADEUTERONC   ',itrt,1.875613,-1.,1.000000E+15,
     +UBUF,0)
      CALL GSPART(146,'ATRITONC     ',itrt,2.80925 ,-1.,1.000000E+15,
     +UBUF,0)
      CALL GSPART(147,'AALPHAC      ',itrt,3.727417,-2.,1.000000E+15,
     +UBUF,0)
      CALL GSPART(149,'AHE3C        ',itrt,2.80923 ,-2.,1.000000E+15,
     +UBUF,0)
*
      END

CDECK  ID>, GUPHAD. 
*CMZ :          05/06/92  09.26.29  by
*-- Author :
      SUBROUTINE GUPHAD
      COMMON/GCPHYS/IPAIR,SPAIR,SLPAIR,ZINTPA,STEPPA
     +             ,ICOMP,SCOMP,SLCOMP,ZINTCO,STEPCO
     +             ,IPHOT,SPHOT,SLPHOT,ZINTPH,STEPPH
     +             ,IPFIS,SPFIS,SLPFIS,ZINTPF,STEPPF
     +             ,IDRAY,SDRAY,SLDRAY,ZINTDR,STEPDR
     +             ,IANNI,SANNI,SLANNI,ZINTAN,STEPAN
     +             ,IBREM,SBREM,SLBREM,ZINTBR,STEPBR
     +             ,IHADR,SHADR,SLHADR,ZINTHA,STEPHA
     +             ,IMUNU,SMUNU,SLMUNU,ZINTMU,STEPMU
     +             ,IDCAY,SDCAY,SLIFE ,SUMLIF,DPHYS1
     +             ,ILOSS,SLOSS,SOLOSS,STLOSS,DPHYS2
     +             ,IMULS,SMULS,SOMULS,STMULS,DPHYS3
     +             ,IRAYL,SRAYL,SLRAYL,ZINTRA,STEPRA
      COMMON/GCPHLT/ILABS,SLABS,SLLABS,ZINTLA,STEPLA
     +             ,ISYNC
     +             ,ISTRA
*
      INTEGER IPAIR,ICOMP,IPHOT,IPFIS,IDRAY,IANNI,IBREM,IHADR,IMUNU
     +       ,IDCAY,ILOSS,IMULS,IRAYL,ILABS,ISYNC,ISTRA
      REAL    SPAIR,SLPAIR,ZINTPA,STEPPA,SCOMP,SLCOMP,ZINTCO,STEPCO
     +       ,SPHOT,SLPHOT,ZINTPH,STEPPH,SPFIS,SLPFIS,ZINTPF,STEPPF
     +       ,SDRAY,SLDRAY,ZINTDR,STEPDR,SANNI,SLANNI,ZINTAN,STEPAN
     +       ,SBREM,SLBREM,ZINTBR,STEPBR,SHADR,SLHADR,ZINTHA,STEPHA
     +       ,SMUNU,SLMUNU,ZINTMU,STEPMU,SDCAY,SLIFE ,SUMLIF,DPHYS1
     +       ,SLOSS,SOLOSS,STLOSS,DPHYS2,SMULS,SOMULS,STMULS,DPHYS3
     +       ,SRAYL,SLRAYL,ZINTRA,STEPRA,SLABS,SLLABS,ZINTLA,STEPLA
C
      COMMON/GCKINE/IKINE,PKINE(10),ITRA,ISTAK,IVERT,IPART,ITRTYP
     +      ,NAPART(5),AMASS,CHARGE,TLIFE,VERT(3),PVERT(4),IPAOLD
C
      INTEGER       IKINE,ITRA,ISTAK,IVERT,IPART,ITRTYP,NAPART,IPAOLD
      REAL          PKINE,AMASS,CHARGE,TLIFE,VERT,PVERT
C
CDE, FCONS.
CDE, XCSIZE.
CDE, XCMATM.
       parameter (inmat=1000)
       common /abcrossC/sigma_t(inmat),sigma_el(3,inmat),
     +  sigma_q(5,inmat),abinx(inmat),abelx(inmat),abqex(inmat)
        common /abcr2C/qexnor,anxnor,elxnor,sine,sela,sqel,fsig
C.
C.    ******************************************************************
C.    *                                                                *
C.    *                                                                *
C.    *       GEANT3 user routine called at each step                  *
C.    *       to evaluate the next hadronic interaction point          *
C.    *                                                                *
C.    *                (TATINA) uses R.Barlow cross-sections tables    *
C.    *                (GHEISHA)uses the GHEISHA cross-sections        *
C.    *                                                                *
C.    ******************************************************************
C.
C.
C.    ------------------------------------------------------------------
C.
              common /hvsec/ihvse(2)
      if(ipart.ge.32)then
          if(ihvse(1).eq.1.or.ihvse(1).eq.2)call abdist
      else
       if(ihadr.le.3)then
        CALL GPGHEI
       else
        call fldist
       endif
      endif
C
      END
CDECK  ID>, GUHADR. 
*CMZ :          05/06/92  09.26.29  by
*-- Author :
      SUBROUTINE GUHADR
      COMMON/GCPHYS/IPAIR,SPAIR,SLPAIR,ZINTPA,STEPPA
     +             ,ICOMP,SCOMP,SLCOMP,ZINTCO,STEPCO
     +             ,IPHOT,SPHOT,SLPHOT,ZINTPH,STEPPH
     +             ,IPFIS,SPFIS,SLPFIS,ZINTPF,STEPPF
     +             ,IDRAY,SDRAY,SLDRAY,ZINTDR,STEPDR
     +             ,IANNI,SANNI,SLANNI,ZINTAN,STEPAN
     +             ,IBREM,SBREM,SLBREM,ZINTBR,STEPBR
     +             ,IHADR,SHADR,SLHADR,ZINTHA,STEPHA
     +             ,IMUNU,SMUNU,SLMUNU,ZINTMU,STEPMU
     +             ,IDCAY,SDCAY,SLIFE ,SUMLIF,DPHYS1
     +             ,ILOSS,SLOSS,SOLOSS,STLOSS,DPHYS2
     +             ,IMULS,SMULS,SOMULS,STMULS,DPHYS3
     +             ,IRAYL,SRAYL,SLRAYL,ZINTRA,STEPRA
      COMMON/GCPHLT/ILABS,SLABS,SLLABS,ZINTLA,STEPLA
     +             ,ISYNC
     +             ,ISTRA
*
      INTEGER IPAIR,ICOMP,IPHOT,IPFIS,IDRAY,IANNI,IBREM,IHADR,IMUNU
     +       ,IDCAY,ILOSS,IMULS,IRAYL,ILABS,ISYNC,ISTRA
      REAL    SPAIR,SLPAIR,ZINTPA,STEPPA,SCOMP,SLCOMP,ZINTCO,STEPCO
     +       ,SPHOT,SLPHOT,ZINTPH,STEPPH,SPFIS,SLPFIS,ZINTPF,STEPPF
     +       ,SDRAY,SLDRAY,ZINTDR,STEPDR,SANNI,SLANNI,ZINTAN,STEPAN
     +       ,SBREM,SLBREM,ZINTBR,STEPBR,SHADR,SLHADR,ZINTHA,STEPHA
     +       ,SMUNU,SLMUNU,ZINTMU,STEPMU,SDCAY,SLIFE ,SUMLIF,DPHYS1
     +       ,SLOSS,SOLOSS,STLOSS,DPHYS2,SMULS,SOMULS,STMULS,DPHYS3
     +       ,SRAYL,SLRAYL,ZINTRA,STEPRA,SLABS,SLLABS,ZINTLA,STEPLA
C
      COMMON/GCKINE/IKINE,PKINE(10),ITRA,ISTAK,IVERT,IPART,ITRTYP
     +      ,NAPART(5),AMASS,CHARGE,TLIFE,VERT(3),PVERT(4),IPAOLD
C
      INTEGER       IKINE,ITRA,ISTAK,IVERT,IPART,ITRTYP,NAPART,IPAOLD
      REAL          PKINE,AMASS,CHARGE,TLIFE,VERT,PVERT
C
CDE, FCONS.
C.
C.    ******************************************************************
C.    *                                                                *
C.    *                                                                *
C.    *       GEANT3 user routine called when a hadronic process       *
C.    *       has been selected in the current step.                   *
C.    ******************************************************************
C.
C.
C.    ------------------------------------------------------------------
C.
              common /hvsec/ihvse(2)
      if(ipart.ge.32)then
       if(ihvse(1).eq.1.or.ihvse(1).eq.2)call abufin
      else
      if(ihadr.le.3)then
       CALL GHEISH
      else
       call flufin
      endif
      endif
      RETURN
      END
CDECK  ID>, BLANKDEK.   
*CMZ :  3.21/02 29/03/94  15.41.49  by  S.Giani
*-- Author :

CDECK  ID>, BLANKDEK.   
*CMZ :  3.21/02 29/03/94  15.41.49  by  S.Giani
*-- Author :
CDECK  ID>, ERTRAK. 
*CMZ :  3.21/02 29/03/94  15.41.49  by  S.Giani
*-- Author :
      SUBROUTINE ERTRAK (X1, P1, X2, P2, IPA, CHOPT)
*
************************************************************************
*                                                                      *
*          Perform the tracking of the track from point X1 to          *
*                    point X2                                          *
*          (Before calling this routine the user should also provide   *
*                    the input informations in /EROPTS/ and /ERTRIO/   *
*                    using subroutine EUFIL(L/P/V)                     *
*                 X1       - Starting coordinates (Cartesian)          *
*                 P1       - Starting 3-momentum  (Cartesian)          *
*                 X2       - Final coordinates    (Cartesian)          *
*                 P2       - Final 3-momentum     (Cartesian)          *
*                 IPA      - Particle code (a la GEANT) of the track   *
*                                                                      *
*                 CHOPT                                                *
*                     'B'   'Backward tracking' - i.e. energy loss     *
*                                        added to the current energy   *
*                     'E'   'Exact' calculation of errors assuming     *
*                                        helix (i.e. pathlength not    *
*                                        assumed as infinitesimal)     *
*                     'L'   Tracking upto prescribed Lengths reached   *
*                     'M'   'Mixed' prediction (not yet coded)         *
*                     'O'   Tracking 'Only' without calculating errors *
*                     'P'   Tracking upto prescribed Planes reached    *
*                     'V'   Tracking upto prescribed Volumes reached   *
*                     'X'   Tracking upto prescribed Point approached  *
*                                                                      *
*                Interface with GEANT :                                *
*             Track parameters are in /CGKINE/ and /GCTRAK/            *
*                                                                      *
*          ==>Called by : USER                                         *
*             Authors   M.Maire, E.Nagy  *********                     *
*                                                                      *
************************************************************************
*
      INTEGER IQ,LQ,NZEBRA,IXSTOR,IXDIV,IXCONS,LMAIN,LR1,JCG
      INTEGER KWBANK,KWWORK,IWS
      REAL GVERSN,ZVERSN,FENDQ,WS,Q
C
      PARAMETER (KWBANK=69000,KWWORK=5200)
      COMMON/GCBANK/NZEBRA,GVERSN,ZVERSN,IXSTOR,IXDIV,IXCONS,FENDQ(16)
     +             ,LMAIN,LR1,WS(KWBANK)
      DIMENSION IQ(2),Q(2),LQ(8000),IWS(2)
      EQUIVALENCE (Q(1),IQ(1),LQ(9)),(LQ(1),LMAIN),(IWS(1),WS(1))
      EQUIVALENCE (JCG,JGSTAT)
      INTEGER       JDIGI ,JDRAW ,JHEAD ,JHITS ,JKINE ,JMATE ,JPART
     +      ,JROTM ,JRUNG ,JSET  ,JSTAK ,JGSTAT,JTMED ,JTRACK,JVERTX
     +      ,JVOLUM,JXYZ  ,JGPAR ,JGPAR2,JSKLT
C
      COMMON/GCLINK/JDIGI ,JDRAW ,JHEAD ,JHITS ,JKINE ,JMATE ,JPART
     +      ,JROTM ,JRUNG ,JSET  ,JSTAK ,JGSTAT,JTMED ,JTRACK,JVERTX
     +      ,JVOLUM,JXYZ  ,JGPAR ,JGPAR2,JSKLT
C
      COMMON/GCNUM/NMATE ,NVOLUM,NROTM,NTMED,NTMULT,NTRACK,NPART
     +            ,NSTMAX,NVERTX,NHEAD,NBIT
      COMMON /GCNUMX/ NALIVE,NTMSTO
C
      INTEGER      NMATE ,NVOLUM,NROTM,NTMED,NTMULT,NTRACK,NPART
     +            ,NSTMAX,NVERTX,NHEAD,NBIT ,NALIVE,NTMSTO
C
      COMMON/GCKINE/IKINE,PKINE(10),ITRA,ISTAK,IVERT,IPART,ITRTYP
     +      ,NAPART(5),AMASS,CHARGE,TLIFE,VERT(3),PVERT(4),IPAOLD
C
      INTEGER       IKINE,ITRA,ISTAK,IVERT,IPART,ITRTYP,NAPART,IPAOLD
      REAL          PKINE,AMASS,CHARGE,TLIFE,VERT,PVERT
C
      COMMON/GCTMED/NUMED,NATMED(5),ISVOL,IFIELD,FIELDM,TMAXFD,STEMAX
     +      ,DEEMAX,EPSIL,STMIN,CFIELD,PREC,IUPD,ISTPAR,NUMOLD
      COMMON/GCTLIT/THRIND,PMIN,DP,DNDL,JMIN,ITCKOV,IMCKOV,NPCKOV
C
      INTEGER       NUMED,NATMED,ISVOL,IFIELD,IUPD,ISTPAR,NUMOLD
      REAL          FIELDM,TMAXFD,STEMAX,DEEMAX,EPSIL,STMIN,CFIELD,PREC
      INTEGER       JMIN,NPCKOV,IMCKOV,ITCKOV
      REAL          THRIND,PMIN,DP,DNDL
C
      INTEGER NMEC,LMEC,NAMEC,NSTEP ,MAXNST,IGNEXT,INWVOL,ISTOP,MAXMEC
     + ,IGAUTO,IEKBIN,ILOSL, IMULL,INGOTO,NLDOWN,NLEVIN,NLVSAV,ISTORY
     + ,MAXME1,NAMEC1
      REAL  VECT,GETOT,GEKIN,VOUT,DESTEP,DESTEL,SAFETY,SLENG ,STEP
     + ,SNEXT,SFIELD,TOFG  ,GEKRAT,UPWGHT
      REAL POLAR
      PARAMETER (MAXMEC=30)
      COMMON/GCTRAK/VECT(7),GETOT,GEKIN,VOUT(7),NMEC,LMEC(MAXMEC)
     + ,NAMEC(MAXMEC),NSTEP ,MAXNST,DESTEP,DESTEL,SAFETY,SLENG
     + ,STEP  ,SNEXT ,SFIELD,TOFG  ,GEKRAT,UPWGHT,IGNEXT,INWVOL
     + ,ISTOP ,IGAUTO,IEKBIN, ILOSL, IMULL,INGOTO,NLDOWN,NLEVIN
     + ,NLVSAV,ISTORY
      PARAMETER (MAXME1=30)
      COMMON/GCTPOL/POLAR(3), NAMEC1(MAXME1)
C
      COMMON/GCUNIT/LIN,LOUT,NUNITS,LUNITS(5)
      INTEGER LIN,LOUT,NUNITS,LUNITS
      COMMON/GCMAIL/CHMAIL
      CHARACTER*132 CHMAIL
C
      INTEGER          MXPRED
      PARAMETER (MXPRED = 150)
      DOUBLE PRECISION ERDTRP
      REAL             ERRIN, ERROUT, ERTRSP, ERXIN, ERXOUT, ERPIN,
     +                 ERPOUT
      INTEGER          NEPRED, INLIST, ILPRED, IEPRED
      COMMON /ERTRIO/  ERDTRP(5,5,MXPRED), ERRIN(15), ERROUT(15,MXPRED),
     +                 ERTRSP(5,5,MXPRED), ERXIN( 3), ERXOUT( 3,MXPRED),
     +                 ERPIN(3), ERPOUT(3,MXPRED), NEPRED,INLIST,ILPRED,
     +                 IEPRED(MXPRED)
      CHARACTER*8     CHOPTI
      LOGICAL         LEEXAC, LELENG, LEONLY, LEPLAN, LEPOIN, LEVOLU
      REAL            ERPLI, ERPLO, ERLENG
      INTEGER         NAMEER, NUMVER, IOVLER
      COMMON /EROPTS/ ERPLI(3,2), ERPLO(3,4,MXPRED), ERLENG(MXPRED),
     +                NAMEER(MXPRED), NUMVER(MXPRED), IOVLER(MXPRED),
     +                LEEXAC, LELENG, LEONLY, LEPLAN, LEPOIN, LEVOLU
      COMMON /EROPTC/CHOPTI


      DOUBLE PRECISION EI, EF, ASDSC
      COMMON /ERWORK/ EI(15), EF(15), ASDSC(5,5),
     +                   XI(3), PPI(3), HI(9),
     +                   XF(3), PF(3),  HF(9),
     +                   CHTR, DEDX2, BACKTR, CUTEK, TLGCM2, TLRAD
      DOUBLE PRECISION A, B, S, TN, T, COSL, SINL, COSP, SINP, COSL1
      INTEGER          NEW
      COMMON /TRCOM3/ A(5,5), B(5,5), S(15),
     +                TN(3), T(5), COSL, SINL, COSP, SINP, COSL1, NEW
*
      DIMENSION      P1(3), P2(3), X1(3), X2(3), DUM(15), IOPT(30)
      EQUIVALENCE    (IOPT(1),IOPTB), (IOPT(2),IOPTE), (IOPT(3),IOPTL),
     ,               (IOPT(4),IOPTM), (IOPT(5),IOPTO), (IOPT(6),IOPTP),
     ,               (IOPT(7),IOPTV), (IOPT(8),IOPTX)
      CHARACTER      CHOPT*(*)
*
*
* *** Decode character option
*
      CHOPTI = CHOPT
      CALL UOPTC (CHOPT, 'BELMOPVX', IOPT)
*
      IF (IOPTB.EQ.0) THEN
         BACKTR = 1.
      ELSE
         BACKTR = -1.
      ENDIF
*
      LEEXAC = IOPTE.NE.0
      LELENG = IOPTL.NE.0
      LEONLY = IOPTO.NE.0
      LEPLAN = IOPTP.NE.0
      LEVOLU = IOPTV.NE.0
      LEPOIN = IOPTX.NE.0
*
* *** Check consistency of the Ch-options
*
         IF ((LELENG .AND. LEVOLU) .OR. (LELENG .AND. LEPLAN) .OR.
     +       (LEVOLU .AND. LEPLAN)) THEN
            WRITE (LOUT, 779)
            GO TO 99
         ENDIF
*
* *** Initialization
*
      IF (NEPRED.LE.0) THEN
         WRITE (LOUT, 780)
         GO TO 99
      ENDIF
      ILPRED = 0
      TLGCM2 = 0.
      TLRAD  = 0.
      CALL VZERO (IEPRED ,   MXPRED)
      CALL VZERO (ERXOUT , 3*MXPRED)
      CALL VZERO (ERPOUT , 3*MXPRED)
      CALL VZERO (ERROUT ,15*MXPRED)
*
      DO 10 I = 1, 3
         ERXIN(I) = X1(I)
   10 CONTINUE
*
      PMOM2 = P1(1)**2 + P1(2)**2 + P1(3)**2
      IF(PMOM2.LE.1.E-20) THEN
         WRITE (LOUT, 778)
         GO TO 99
      ENDIF
      PABS = SQRT (PMOM2)
      ERPIN(1) = 1./PABS
      ERPIN(2) = ASIN (P1(3)*ERPIN(1))
      IF (ABS (P1(1)) .LT. 1.E-30) P1(1) = 1.E-30
      ERPIN(3) = ATAN2 (P1(2), P1(1))
*
* *** Initialize GCKINE common
*
      IF((IPA.LE.0).OR.(IPA.GT.NPART)) THEN
          WRITE (LOUT, 777) IPA
          GO TO 99
      ENDIF
*
      ITRA = 1
      ISTAK = 0
      IPART = IPA
      JPA = LQ(JPART-IPART)
      DO 26 I=1,5
         NAPART(I) = IQ(JPA+I)
   26 CONTINUE
      ITRTYP = Q(JPA+6)
      AMASS  = Q(JPA+7)
      CHARGE = Q(JPA+8)
      CHTR   = CHARGE*BACKTR
      TLIFE  = Q(JPA+9)
*
* *** Starting field
*
      CALL VZERO (HI, 9)
      CALL VZERO (HF, 9)
*
* *** Error matrix into SC System
*
      IF (LEPLAN) THEN
         IF (IFIELD.EQ.3) THEN
            HI(3) = FIELDM
         ELSEIF (IFIELD.NE.0) THEN
            CALL GUFLD (X1, HI)
         ENDIF
         CALL VZERO (DUM,15)
         CALL TRSCSD (ERPIN(1), DUM(1),   ERPIN(1), DUM(1),   HI(1),
     +                CHARGE, IERR, SPU, ERPLI(1,1), ERPLI(1,2))
         IF (LEONLY) GOTO 35
         CALL TRSDSC (ERPIN(1), ERRIN(1), DUM(1),   ERRIN(1), HI(1),
     +                CHARGE, IERR, SPU, ERPLI(1,1), ERPLI(1,2))
         DO 29 I = 1, 5
            DO 28 J = 1, 5
               ASDSC(I,J) = A(I,J)
   28       CONTINUE
   29    CONTINUE
      ENDIF
      IF (LEONLY) GOTO 35
*
* *** Error matrix into direction of tracking
*
      IF (BACKTR .LT. 0.) CALL ERBCER (ERRIN(1))
*
* *** Error matrix into double precision
*
      DO 30 I = 1,15
         EI(I) = ERRIN(I)
   30 CONTINUE
*
   35 CONTINUE
      DO 41 I = 1, 3
         VERT(I)  = X1(I)
         PVERT(I) = P1(I)*BACKTR
   41 CONTINUE
*
* *** Initialize GCTRAK common
*
      PVERT(4) = SQRT (PMOM2 + AMASS**2)
      VECT(7) = PABS
      DO 51 I=1,3
         VECT(I) = VERT(I)
         VECT(I+3) = PVERT(I)/VECT(7)
   51 CONTINUE
      GETOT = PVERT(4)
      GEKIN = GETOT - AMASS
      IF(GEKIN.LT.0.) GEKIN = 0.
      CALL GEKBIN
*
* *** Additional EMC initialization
*
      IF (.NOT.LEONLY) CALL ERPINI
*
* *** Ready for tracking
*
      CALL ERTRGO
*
* *** Copy the final point and momentum into the output buffer
*
      DO 60 I = 1, 3
         X2(I) = VECT(I)
         P2(I) = VECT(7)*BACKTR*VECT(I+3)
   60 CONTINUE
*
  777 FORMAT(/,4X,'Error in ERTRAK : particle type ', I4,
     *        '  unknown in GEANT' )
  778 FORMAT(/,4X,'Error in ERTRAK : Nul Momentum. Tracking stops now')
  779 FORMAT(/,4X,'Error in ERTRAK : Inconsistent character options',
     +                               '. Tracking stops now')
  780 FORMAT(/,4X,'Error in ERTRAK : No prediction. Tracking stops now')
*                                                           END ERTRAK
  99  END
CDECK  ID>, ERBCER. 
*CMZ :  3.21/02 29/03/94  15.41.49  by  S.Giani
*-- Author :
      SUBROUTINE ERBCER (E)
*
C.    ******************************************************************
C.    *                                                                *
C.    *       After the back-tracking transforms the error matrix      *
C.    *                  back to the original                          *
C.    *                  particle direction                            *
C.    *                                                                *
C.    *       Arguments                                                *
C.    *           E        - Error Matrix (Triangular)                 *
C.    *                                                                *
C.    *    ==>Called by : ERPEND                                       *
C.    *       Author    E.Nagy  *********                              *
C.    *                                                                *
C.    ******************************************************************
*
         DIMENSION E(15)
*
      J = 0
      DO 10 I = 1, 5
         DO 5 K = I, 5
            J = J + 1
            IF (I.EQ.2) E(J) = -E(J)
            IF (K.EQ.2) E(J) = -E(J)
            IF (I.EQ.4) E(J) = -E(J)
            IF (K.EQ.4) E(J) = -E(J)
    5    CONTINUE
   10 CONTINUE
*                                                           END ERBCER
      END
CDECK  ID>, ERBCTR. 
*CMZ :  3.21/02 29/03/94  15.41.49  by  S.Giani
*-- Author :
      SUBROUTINE ERBCTR (X)
*
C.    ******************************************************************
C.    *                                                                *
C.    *       After the back-tracking transforms                       *
C.    *                  the transport matrix back to the original     *
C.    *                  particle direction                            *
C.    *                                                                *
C.    *       Arguments                                                *
C.    *           X        - Transport Matrix                          *
C.    *                                                                *
C.    *    ==>Called by : ERPEND                                       *
C.    *       Author    E.Nagy  *********                              *
C.    *                                                                *
C.    ******************************************************************
*
         DOUBLE PRECISION X(5,5)
*
      DO 10 I = 1, 5
         DO 5 K = 1, 5
            IF (I.EQ.2) X(I,K) = -X(I,K)
            IF (K.EQ.2) X(I,K) = -X(I,K)
            IF (I.EQ.4) X(I,K) = -X(I,K)
            IF (K.EQ.4) X(I,K) = -X(I,K)
    5    CONTINUE
   10 CONTINUE
*                                                           END ERBCTR
      END
CDECK  ID>, ERLAND. 
*CMZ :  3.21/02 29/03/94  15.41.49  by  S.Giani
*-- Author :
      SUBROUTINE ERLAND (STEP, Z, A, RHO, P, E, XMASS, DEDX2)
C.
C.    ******************************************************************
C.    *                                                                *
C.    *  Calculates energy straggling using Gaussian theory in a step  *
C.    *                                                                *
C.    *  Input  STEP   =  current step-length (cm)                     *
C.    *         Z      =  Z of the material                            *
C.    *         A      =  A of the material                            *
C.    *         RHO    =  density of the material                      *
C.    *         P      =  momentum of the particle                     *
C.    *         E      =  energy   of the particle                     *
C.    *         XMASS  =  mass     of the particle                     *
C.    *                                                                *
C.    *  Output DEDX2  =  mean square of the straggling in G           *
C.    *                                                                *
C.    *    ==>Called by : ERTRCH                                       *
C.    *       Author    E.Nagy  *********                              *
C.    *       Original routine : GLANDO                                *
C.    *                                                                *
C.    ******************************************************************
C.
      REAL          PI,TWOPI,PIBY2,DEGRAD,RADDEG,CLIGHT,BIG,EMASS
      REAL          EMMU,PMASS,AVO
C
      COMMON/GCONST/PI,TWOPI,PIBY2,DEGRAD,RADDEG,CLIGHT,BIG,EMASS
      COMMON/GCONSX/EMMU,PMASS,AVO
C
*
      IF (STEP.LT.1.E-7) THEN
         DEDX2=0.
         RETURN
      ENDIF
*
*     Calculate xi factor (KeV).
      BETA   = P/E
      GAMMA  = E/XMASS
      XI     = (153.5*Z*STEP*RHO)/(A*BETA*BETA)
*
*     Maximum energy transfer to atomic electron (KeV).
      ETA    = BETA*GAMMA
      ETASQ  = ETA*ETA
      RATIO  = EMASS/XMASS
      F1     = 2.*EMASS*ETASQ
      F2     = 1.+2.*RATIO*GAMMA+RATIO*RATIO
      EMAX   = 1.E+6*F1/F2
*
*
* *** and now sigma**2  in GeV
*        gaussian
      DEDX2  = XI*EMAX*(1.-(BETA*BETA/2.))*1.E-12
*
      END
CDECK  ID>, ERMCSC. 
*CMZ :  3.21/02 29/03/94  15.41.49  by  S.Giani
*-- Author :
      SUBROUTINE ERMCSC (E)
*
C.    ******************************************************************
C.    *                                                                *
C.    *       Calculates Multiple Scattering Matrix in a step          *
C.    *                                                                *
C.    *           E        - Error Matrix (Triangular)                 *
C.    *           STEP     - Step length in cm                         *
C.    *           RADL     - Radiation length in cm                    *
C.    *           VECT(7)  - Momentum in GeV                           *
C.    *           CLA      - Cosine of the deep angle                  *
C.    *                                                                *
C.    *    ==>Called by : ERPROP                                       *
C.    *       Author    E.Nagy  *********                              *
C.    *                                                                *
C.    ******************************************************************
*
      COMMON/GCMATE/NMAT,NAMATE(5),A,Z,DENS,RADL,ABSL
C
      INTEGER NMAT,NAMATE
      REAL A,Z,DENS,RADL,ABSL
C
      COMMON/GCKINE/IKINE,PKINE(10),ITRA,ISTAK,IVERT,IPART,ITRTYP
     +      ,NAPART(5),AMASS,CHARGE,TLIFE,VERT(3),PVERT(4),IPAOLD
C
      INTEGER       IKINE,ITRA,ISTAK,IVERT,IPART,ITRTYP,NAPART,IPAOLD
      REAL          PKINE,AMASS,CHARGE,TLIFE,VERT,PVERT
C
      INTEGER NMEC,LMEC,NAMEC,NSTEP ,MAXNST,IGNEXT,INWVOL,ISTOP,MAXMEC
     + ,IGAUTO,IEKBIN,ILOSL, IMULL,INGOTO,NLDOWN,NLEVIN,NLVSAV,ISTORY
     + ,MAXME1,NAMEC1
      REAL  VECT,GETOT,GEKIN,VOUT,DESTEP,DESTEL,SAFETY,SLENG ,STEP
     + ,SNEXT,SFIELD,TOFG  ,GEKRAT,UPWGHT
      REAL POLAR
      PARAMETER (MAXMEC=30)
      COMMON/GCTRAK/VECT(7),GETOT,GEKIN,VOUT(7),NMEC,LMEC(MAXMEC)
     + ,NAMEC(MAXMEC),NSTEP ,MAXNST,DESTEP,DESTEL,SAFETY,SLENG
     + ,STEP  ,SNEXT ,SFIELD,TOFG  ,GEKRAT,UPWGHT,IGNEXT,INWVOL
     + ,ISTOP ,IGAUTO,IEKBIN, ILOSL, IMULL,INGOTO,NLDOWN,NLEVIN
     + ,NLVSAV,ISTORY
      PARAMETER (MAXME1=30)
      COMMON/GCTPOL/POLAR(3), NAMEC1(MAXME1)
C
*
      DOUBLE PRECISION E(15)
*
      PBETA = VECT(7)*VECT(7)/GETOT
      RI    = STEP/RADL
*
      CRLOG = 1.
CC      CRLOG = 1. + 0.201*LOG(RI)
CC      IF (CRLOG.LE.0.) CRLOG = 0.
      D     = 1.8496E-4*RI*((CHARGE*CRLOG/PBETA)**2)
      S1    = D*STEP**2/3.
      S2    = D
      S3    = D*STEP/2.
      CLA = SQRT (VECT(4)**2+VECT(5)**2)
      E(6)  = E(6)+S2
      E(9)  = E(9)-S3
      E(10) = E(10)+S2/CLA**2
      E(11) = E(11)+S3/CLA
      E(13) = E(13)+S1
      E(15) = E(15)+S1
*
      END
CDECK  ID>, ERPINI. 
*CMZ :  3.21/02 29/03/94  15.41.49  by  S.Giani
*-- Author :
      SUBROUTINE ERPINI
*
C.    ******************************************************************
C.    *                                                                *
C.    *       Initializes the error propagation                        *
C.    *                                                                *
C.    *    ==>Called by : ERTRAK                                       *
C.    *       Author    E.Nagy  *********                              *
C.    *                                                                *
C.    ******************************************************************
C.
      COMMON/GCKINE/IKINE,PKINE(10),ITRA,ISTAK,IVERT,IPART,ITRTYP
     +      ,NAPART(5),AMASS,CHARGE,TLIFE,VERT(3),PVERT(4),IPAOLD
C
      INTEGER       IKINE,ITRA,ISTAK,IVERT,IPART,ITRTYP,NAPART,IPAOLD
      REAL          PKINE,AMASS,CHARGE,TLIFE,VERT,PVERT
C
      INTEGER NMEC,LMEC,NAMEC,NSTEP ,MAXNST,IGNEXT,INWVOL,ISTOP,MAXMEC
     + ,IGAUTO,IEKBIN,ILOSL, IMULL,INGOTO,NLDOWN,NLEVIN,NLVSAV,ISTORY
     + ,MAXME1,NAMEC1
      REAL  VECT,GETOT,GEKIN,VOUT,DESTEP,DESTEL,SAFETY,SLENG ,STEP
     + ,SNEXT,SFIELD,TOFG  ,GEKRAT,UPWGHT
      REAL POLAR
      PARAMETER (MAXMEC=30)
      COMMON/GCTRAK/VECT(7),GETOT,GEKIN,VOUT(7),NMEC,LMEC(MAXMEC)
     + ,NAMEC(MAXMEC),NSTEP ,MAXNST,DESTEP,DESTEL,SAFETY,SLENG
     + ,STEP  ,SNEXT ,SFIELD,TOFG  ,GEKRAT,UPWGHT,IGNEXT,INWVOL
     + ,ISTOP ,IGAUTO,IEKBIN, ILOSL, IMULL,INGOTO,NLDOWN,NLEVIN
     + ,NLVSAV,ISTORY
      PARAMETER (MAXME1=30)
      COMMON/GCTPOL/POLAR(3), NAMEC1(MAXME1)
C
      COMMON/GCUNIT/LIN,LOUT,NUNITS,LUNITS(5)
      INTEGER LIN,LOUT,NUNITS,LUNITS
      COMMON/GCMAIL/CHMAIL
      CHARACTER*132 CHMAIL
C
      DOUBLE PRECISION EI, EF, ASDSC
      COMMON /ERWORK/ EI(15), EF(15), ASDSC(5,5),
     +                   XI(3), PPI(3), HI(9),
     +                   XF(3), PF(3),  HF(9),
     +                   CHTR, DEDX2, BACKTR, CUTEK, TLGCM2, TLRAD
      INTEGER          MXPRED
      PARAMETER (MXPRED = 150)
      DOUBLE PRECISION ERDTRP
      REAL             ERRIN, ERROUT, ERTRSP, ERXIN, ERXOUT, ERPIN,
     +                 ERPOUT
      INTEGER          NEPRED, INLIST, ILPRED, IEPRED
      COMMON /ERTRIO/  ERDTRP(5,5,MXPRED), ERRIN(15), ERROUT(15,MXPRED),
     +                 ERTRSP(5,5,MXPRED), ERXIN( 3), ERXOUT( 3,MXPRED),
     +                 ERPIN(3), ERPOUT(3,MXPRED), NEPRED,INLIST,ILPRED,
     +                 IEPRED(MXPRED)
      CHARACTER*8     CHOPTI
      LOGICAL         LEEXAC, LELENG, LEONLY, LEPLAN, LEPOIN, LEVOLU
      REAL            ERPLI, ERPLO, ERLENG
      INTEGER         NAMEER, NUMVER, IOVLER
      COMMON /EROPTS/ ERPLI(3,2), ERPLO(3,4,MXPRED), ERLENG(MXPRED),
     +                NAMEER(MXPRED), NUMVER(MXPRED), IOVLER(MXPRED),
     +                LEEXAC, LELENG, LEONLY, LEPLAN, LEPOIN, LEVOLU
      COMMON /EROPTC/CHOPTI


*
      DIMENSION DUMM(15)
*
* *** Starting point
*
      DO 11 I = 1,3
         XI(I) = VECT(I)
         PPI(I) = VECT(7)*VECT(3+I)
   11 CONTINUE
*
* *** Initialize tracking
*
      MVAR   = 0
      IFLAG  =-1
      ITRAN  = 0
      CALL TRPROP (XI, PPI, HI, DUM, DUM, HI, CHTR, DUM, DUMM,
     &            MVAR, IFLAG, ITRAN, IERR)
      IF (IERR.NE.0) THEN
         WRITE (LOUT, 777) IERR
         RETURN
      ENDIF
      DO 13 I = 1,15
         EF(I) = EI(I)
   13 CONTINUE
*
  777 FORMAT(//4X,' **** Error in subr. TRPROP',I4,'  called by'
     F     ,' subr. ERPINI'//)
*
       END
CDECK  ID>, ERSTOR. 
*CMZ :  3.21/02 29/03/94  15.41.49  by  S.Giani
*-- Author :
      SUBROUTINE ERSTOR
*
C.    ******************************************************************
C.    *                                                                *
C.    *       Store error matrix informations                          *
C.    *                                                                *
C.    *    ==>Called by : ERTRGO , ERTRCH , ERTRNT                     *
C.    *       Author    M.Maire, E.Nagy  *********                     *
C.    *                                                                *
C.    ******************************************************************
*
      COMMON/GCKINE/IKINE,PKINE(10),ITRA,ISTAK,IVERT,IPART,ITRTYP
     +      ,NAPART(5),AMASS,CHARGE,TLIFE,VERT(3),PVERT(4),IPAOLD
C
      INTEGER       IKINE,ITRA,ISTAK,IVERT,IPART,ITRTYP,NAPART,IPAOLD
      REAL          PKINE,AMASS,CHARGE,TLIFE,VERT,PVERT
C
      INTEGER NMEC,LMEC,NAMEC,NSTEP ,MAXNST,IGNEXT,INWVOL,ISTOP,MAXMEC
     + ,IGAUTO,IEKBIN,ILOSL, IMULL,INGOTO,NLDOWN,NLEVIN,NLVSAV,ISTORY
     + ,MAXME1,NAMEC1
      REAL  VECT,GETOT,GEKIN,VOUT,DESTEP,DESTEL,SAFETY,SLENG ,STEP
     + ,SNEXT,SFIELD,TOFG  ,GEKRAT,UPWGHT
      REAL POLAR
      PARAMETER (MAXMEC=30)
      COMMON/GCTRAK/VECT(7),GETOT,GEKIN,VOUT(7),NMEC,LMEC(MAXMEC)
     + ,NAMEC(MAXMEC),NSTEP ,MAXNST,DESTEP,DESTEL,SAFETY,SLENG
     + ,STEP  ,SNEXT ,SFIELD,TOFG  ,GEKRAT,UPWGHT,IGNEXT,INWVOL
     + ,ISTOP ,IGAUTO,IEKBIN, ILOSL, IMULL,INGOTO,NLDOWN,NLEVIN
     + ,NLVSAV,ISTORY
      PARAMETER (MAXME1=30)
      COMMON/GCTPOL/POLAR(3), NAMEC1(MAXME1)
C
      DOUBLE PRECISION EI, EF, ASDSC
      COMMON /ERWORK/ EI(15), EF(15), ASDSC(5,5),
     +                   XI(3), PPI(3), HI(9),
     +                   XF(3), PF(3),  HF(9),
     +                   CHTR, DEDX2, BACKTR, CUTEK, TLGCM2, TLRAD
      INTEGER          MXPRED
      PARAMETER (MXPRED = 150)
      DOUBLE PRECISION ERDTRP
      REAL             ERRIN, ERROUT, ERTRSP, ERXIN, ERXOUT, ERPIN,
     +                 ERPOUT
      INTEGER          NEPRED, INLIST, ILPRED, IEPRED
      COMMON /ERTRIO/  ERDTRP(5,5,MXPRED), ERRIN(15), ERROUT(15,MXPRED),
     +                 ERTRSP(5,5,MXPRED), ERXIN( 3), ERXOUT( 3,MXPRED),
     +                 ERPIN(3), ERPOUT(3,MXPRED), NEPRED,INLIST,ILPRED,
     +                 IEPRED(MXPRED)
      CHARACTER*8     CHOPTI
      LOGICAL         LEEXAC, LELENG, LEONLY, LEPLAN, LEPOIN, LEVOLU
      REAL            ERPLI, ERPLO, ERLENG
      INTEGER         NAMEER, NUMVER, IOVLER
      COMMON /EROPTS/ ERPLI(3,2), ERPLO(3,4,MXPRED), ERLENG(MXPRED),
     +                NAMEER(MXPRED), NUMVER(MXPRED), IOVLER(MXPRED),
     +                LEEXAC, LELENG, LEONLY, LEPLAN, LEPOIN, LEVOLU
      COMMON /EROPTC/CHOPTI


      DOUBLE PRECISION A, B, S, TN, T, COSL, SINL, COSP, SINP, COSL1
      INTEGER          NEW
      COMMON /TRCOM3/ A(5,5), B(5,5), S(15),
     +                TN(3), T(5), COSL, SINL, COSP, SINP, COSL1, NEW
*
      DOUBLE PRECISION  C(5,5), DUM(5,5)
*
* *** Write out results at intermediate points
*
      ILPRED = ILPRED + 1
      IF (ILPRED.GE.NEPRED) ISTOP = 1
      IEPRED(ILPRED) = INLIST
*
* *** Leave B-matrix intact for eventual further tracking
*
      IF (.NOT. LEONLY) THEN
         DO 6 I = 1, 5
            DO 5 J = 1, 5
               C(I,J) = B(I,J)
    5       CONTINUE
    6    CONTINUE
*
* ***    Error matrix into single precision
*
         DO 10 I = 1, 15
            ERROUT(I,ILPRED) = EF(I)
   10    CONTINUE
*
* ***    If backtracking - vector, charge, error- and transport-matrix
*        in original direction
*
         IF (BACKTR.LT.0.) THEN
            CALL ERBCER (ERROUT(1,ILPRED))
            CALL ERBCTR (C)
         ENDIF
      ENDIF
*
* *** Back into SC variables
*
      IF (VECT(7) .LT. 1.E-30) VECT(7) = 1.E-30
      PF(1) = 1./VECT(7)
      PF(2) = BACKTR*ASIN (VECT(6))
      IF (ABS (VECT(4)) .LT. 1.E-30) VECT(4) = 1.E-30
      PF(3) = ATAN2 (BACKTR*VECT(5), BACKTR*VECT(4))
*
* *** Transform error matrix in the variables of the plane where
*         tracking terminates
*
      IF (LEPLAN) THEN
         CALL TRSCSD (PF, ERROUT(1,ILPRED), PF ,ERROUT(1,ILPRED), HF,
     +                CHARGE, IER ,SPU, ERPLO(1,1,INLIST),
     +                ERPLO(1,2,INLIST))
         IF (LEONLY) GOTO 25
         CALL DMM55 (A, C, DUM)
         CALL DMM55 (DUM, ASDSC, C)
      ENDIF
*
* *** Transport matrix in single precision
*
      DO 20 I = 1,5
         DO 15 J = 1,5
            ERDTRP(I,J,ILPRED) = C(I,J)
            ERTRSP(I,J,ILPRED) = C(I,J)
   15    CONTINUE
   20 CONTINUE
*
   25 CONTINUE
      DO 30 I = 1, 3
         ERXOUT(I,ILPRED) = VECT(I)
         ERPOUT(I,ILPRED) = PF(I)
   30 CONTINUE
*
      END
CDECK  ID>, ERTRGO. 
*CMZ :  3.21/02 29/03/94  15.41.49  by  S.Giani
*-- Author :
      SUBROUTINE ERTRGO
*
C.    ******************************************************************
C.    *                                                                *
C.    *       Perform the tracking of the track                        *
C.    *       Track parameters are in VECT                             *
C.    *                                                                *
C.    *    ==>Called by : ERTRAK                                       *
C.    *       Original routines : GTRACK + GTVOL                       *
C.    *       Authors   M.Maire, E.Nagy  *********                     *
C.    *                                                                *
C.    ******************************************************************
C.
      INTEGER IQ,LQ,NZEBRA,IXSTOR,IXDIV,IXCONS,LMAIN,LR1,JCG
      INTEGER KWBANK,KWWORK,IWS
      REAL GVERSN,ZVERSN,FENDQ,WS,Q
C
      PARAMETER (KWBANK=69000,KWWORK=5200)
      COMMON/GCBANK/NZEBRA,GVERSN,ZVERSN,IXSTOR,IXDIV,IXCONS,FENDQ(16)
     +             ,LMAIN,LR1,WS(KWBANK)
      DIMENSION IQ(2),Q(2),LQ(8000),IWS(2)
      EQUIVALENCE (Q(1),IQ(1),LQ(9)),(LQ(1),LMAIN),(IWS(1),WS(1))
      EQUIVALENCE (JCG,JGSTAT)
      INTEGER       JDIGI ,JDRAW ,JHEAD ,JHITS ,JKINE ,JMATE ,JPART
     +      ,JROTM ,JRUNG ,JSET  ,JSTAK ,JGSTAT,JTMED ,JTRACK,JVERTX
     +      ,JVOLUM,JXYZ  ,JGPAR ,JGPAR2,JSKLT
C
      COMMON/GCLINK/JDIGI ,JDRAW ,JHEAD ,JHITS ,JKINE ,JMATE ,JPART
     +      ,JROTM ,JRUNG ,JSET  ,JSTAK ,JGSTAT,JTMED ,JTRACK,JVERTX
     +      ,JVOLUM,JXYZ  ,JGPAR ,JGPAR2,JSKLT
C
      COMMON/GCJLOC/NJLOC(2),JTM,JMA,JLOSS,JPROB,JMIXT,JPHOT,JANNI
     +                  ,JCOMP,JBREM,JPAIR,JDRAY,JPFIS,JMUNU,JRAYL
     +                  ,JMULOF,JCOEF,JRANG
C
      INTEGER       NJLOC   ,JTM,JMA,JLOSS,JPROB,JMIXT,JPHOT,JANNI
     +                  ,JCOMP,JBREM,JPAIR,JDRAY,JPFIS,JMUNU,JRAYL
     +                  ,JMULOF,JCOEF,JRANG
C
      COMMON/GCJLCK/NJLCK(2),JTCKOV,JABSCO,JEFFIC,JINDEX,JCURIN
     +                      ,JPOLAR,JTSTRA,JTSTCO,JTSTEN,JTASHO
C
      EQUIVALENCE (JLASTV,JTSTEN)
C
      INTEGER       NJLCK,JTCKOV,JABSCO,JEFFIC,JINDEX,JCURIN
     +                   ,JPOLAR,JLASTV,JTSTRA,JTSTCO,JTSTEN
     +                   ,JTASHO
C
      COMMON/GCCUTS/CUTGAM,CUTELE,CUTNEU,CUTHAD,CUTMUO,BCUTE,BCUTM
     +             ,DCUTE ,DCUTM ,PPCUTM,TOFMAX,GCUTS(5)
C
      REAL          CUTGAM,CUTELE,CUTNEU,CUTHAD,CUTMUO,BCUTE,BCUTM
     +             ,DCUTE ,DCUTM ,PPCUTM,TOFMAX,GCUTS
C
      REAL          PI,TWOPI,PIBY2,DEGRAD,RADDEG,CLIGHT,BIG,EMASS
      REAL          EMMU,PMASS,AVO
C
      COMMON/GCONST/PI,TWOPI,PIBY2,DEGRAD,RADDEG,CLIGHT,BIG,EMASS
      COMMON/GCONSX/EMMU,PMASS,AVO
C
      COMMON/GCPHYS/IPAIR,SPAIR,SLPAIR,ZINTPA,STEPPA
     +             ,ICOMP,SCOMP,SLCOMP,ZINTCO,STEPCO
     +             ,IPHOT,SPHOT,SLPHOT,ZINTPH,STEPPH
     +             ,IPFIS,SPFIS,SLPFIS,ZINTPF,STEPPF
     +             ,IDRAY,SDRAY,SLDRAY,ZINTDR,STEPDR
     +             ,IANNI,SANNI,SLANNI,ZINTAN,STEPAN
     +             ,IBREM,SBREM,SLBREM,ZINTBR,STEPBR
     +             ,IHADR,SHADR,SLHADR,ZINTHA,STEPHA
     +             ,IMUNU,SMUNU,SLMUNU,ZINTMU,STEPMU
     +             ,IDCAY,SDCAY,SLIFE ,SUMLIF,DPHYS1
     +             ,ILOSS,SLOSS,SOLOSS,STLOSS,DPHYS2
     +             ,IMULS,SMULS,SOMULS,STMULS,DPHYS3
     +             ,IRAYL,SRAYL,SLRAYL,ZINTRA,STEPRA
      COMMON/GCPHLT/ILABS,SLABS,SLLABS,ZINTLA,STEPLA
     +             ,ISYNC
     +             ,ISTRA
*
      INTEGER IPAIR,ICOMP,IPHOT,IPFIS,IDRAY,IANNI,IBREM,IHADR,IMUNU
     +       ,IDCAY,ILOSS,IMULS,IRAYL,ILABS,ISYNC,ISTRA
      REAL    SPAIR,SLPAIR,ZINTPA,STEPPA,SCOMP,SLCOMP,ZINTCO,STEPCO
     +       ,SPHOT,SLPHOT,ZINTPH,STEPPH,SPFIS,SLPFIS,ZINTPF,STEPPF
     +       ,SDRAY,SLDRAY,ZINTDR,STEPDR,SANNI,SLANNI,ZINTAN,STEPAN
     +       ,SBREM,SLBREM,ZINTBR,STEPBR,SHADR,SLHADR,ZINTHA,STEPHA
     +       ,SMUNU,SLMUNU,ZINTMU,STEPMU,SDCAY,SLIFE ,SUMLIF,DPHYS1
     +       ,SLOSS,SOLOSS,STLOSS,DPHYS2,SMULS,SOMULS,STMULS,DPHYS3
     +       ,SRAYL,SLRAYL,ZINTRA,STEPRA,SLABS,SLLABS,ZINTLA,STEPLA
C
      COMMON/GCKINE/IKINE,PKINE(10),ITRA,ISTAK,IVERT,IPART,ITRTYP
     +      ,NAPART(5),AMASS,CHARGE,TLIFE,VERT(3),PVERT(4),IPAOLD
C
      INTEGER       IKINE,ITRA,ISTAK,IVERT,IPART,ITRTYP,NAPART,IPAOLD
      REAL          PKINE,AMASS,CHARGE,TLIFE,VERT,PVERT
C
      COMMON/GCFLAG/IDEBUG,IDEMIN,IDEMAX,ITEST,IDRUN,IDEVT,IEORUN
     +        ,IEOTRI,IEVENT,ISWIT(10),IFINIT(20),NEVENT,NRNDM(2)
      COMMON/GCFLAX/BATCH, NOLOG
      LOGICAL BATCH, NOLOG
C
      INTEGER       IDEBUG,IDEMIN,IDEMAX,ITEST,IDRUN,IDEVT,IEORUN
     +        ,IEOTRI,IEVENT,ISWIT,IFINIT,NEVENT,NRNDM
C
      COMMON/GCTMED/NUMED,NATMED(5),ISVOL,IFIELD,FIELDM,TMAXFD,STEMAX
     +      ,DEEMAX,EPSIL,STMIN,CFIELD,PREC,IUPD,ISTPAR,NUMOLD
      COMMON/GCTLIT/THRIND,PMIN,DP,DNDL,JMIN,ITCKOV,IMCKOV,NPCKOV
C
      INTEGER       NUMED,NATMED,ISVOL,IFIELD,IUPD,ISTPAR,NUMOLD
      REAL          FIELDM,TMAXFD,STEMAX,DEEMAX,EPSIL,STMIN,CFIELD,PREC
      INTEGER       JMIN,NPCKOV,IMCKOV,ITCKOV
      REAL          THRIND,PMIN,DP,DNDL
C
      COMMON/GCMATE/NMAT,NAMATE(5),A,Z,DENS,RADL,ABSL
C
      INTEGER NMAT,NAMATE
      REAL A,Z,DENS,RADL,ABSL
C
      INTEGER NMEC,LMEC,NAMEC,NSTEP ,MAXNST,IGNEXT,INWVOL,ISTOP,MAXMEC
     + ,IGAUTO,IEKBIN,ILOSL, IMULL,INGOTO,NLDOWN,NLEVIN,NLVSAV,ISTORY
     + ,MAXME1,NAMEC1
      REAL  VECT,GETOT,GEKIN,VOUT,DESTEP,DESTEL,SAFETY,SLENG ,STEP
     + ,SNEXT,SFIELD,TOFG  ,GEKRAT,UPWGHT
      REAL POLAR
      PARAMETER (MAXMEC=30)
      COMMON/GCTRAK/VECT(7),GETOT,GEKIN,VOUT(7),NMEC,LMEC(MAXMEC)
     + ,NAMEC(MAXMEC),NSTEP ,MAXNST,DESTEP,DESTEL,SAFETY,SLENG
     + ,STEP  ,SNEXT ,SFIELD,TOFG  ,GEKRAT,UPWGHT,IGNEXT,INWVOL
     + ,ISTOP ,IGAUTO,IEKBIN, ILOSL, IMULL,INGOTO,NLDOWN,NLEVIN
     + ,NLVSAV,ISTORY
      PARAMETER (MAXME1=30)
      COMMON/GCTPOL/POLAR(3), NAMEC1(MAXME1)
C
      COMMON/GCVOLU/NLEVEL,NAMES(15),NUMBER(15),
     +LVOLUM(15),LINDEX(15),INFROM,NLEVMX,NLDEV(15),LINMX(15),
     +GTRAN(3,15),GRMAT(10,15),GONLY(15),GLX(3)
C
      INTEGER NLEVEL,NAMES,NUMBER,LVOLUM,LINDEX,INFROM,NLEVMX,
     +        NLDEV,LINMX
      REAL GTRAN,GRMAT,GONLY,GLX
      COMMON/GCUNIT/LIN,LOUT,NUNITS,LUNITS(5)
      INTEGER LIN,LOUT,NUNITS,LUNITS
      COMMON/GCMAIL/CHMAIL
      CHARACTER*132 CHMAIL
C
      COMMON/GCNUM/NMATE ,NVOLUM,NROTM,NTMED,NTMULT,NTRACK,NPART
     +            ,NSTMAX,NVERTX,NHEAD,NBIT
      COMMON /GCNUMX/ NALIVE,NTMSTO
C
      INTEGER      NMATE ,NVOLUM,NROTM,NTMED,NTMULT,NTRACK,NPART
     +            ,NSTMAX,NVERTX,NHEAD,NBIT ,NALIVE,NTMSTO
C
      INTEGER          MXPRED
      PARAMETER (MXPRED = 150)
      DOUBLE PRECISION ERDTRP
      REAL             ERRIN, ERROUT, ERTRSP, ERXIN, ERXOUT, ERPIN,
     +                 ERPOUT
      INTEGER          NEPRED, INLIST, ILPRED, IEPRED
      COMMON /ERTRIO/  ERDTRP(5,5,MXPRED), ERRIN(15), ERROUT(15,MXPRED),
     +                 ERTRSP(5,5,MXPRED), ERXIN( 3), ERXOUT( 3,MXPRED),
     +                 ERPIN(3), ERPOUT(3,MXPRED), NEPRED,INLIST,ILPRED,
     +                 IEPRED(MXPRED)
      CHARACTER*8     CHOPTI
      LOGICAL         LEEXAC, LELENG, LEONLY, LEPLAN, LEPOIN, LEVOLU
      REAL            ERPLI, ERPLO, ERLENG
      INTEGER         NAMEER, NUMVER, IOVLER
      COMMON /EROPTS/ ERPLI(3,2), ERPLO(3,4,MXPRED), ERLENG(MXPRED),
     +                NAMEER(MXPRED), NUMVER(MXPRED), IOVLER(MXPRED),
     +                LEEXAC, LELENG, LEONLY, LEPLAN, LEPOIN, LEVOLU
      COMMON /EROPTC/CHOPTI


      DOUBLE PRECISION EI, EF, ASDSC
      COMMON /ERWORK/ EI(15), EF(15), ASDSC(5,5),
     +                   XI(3), PPI(3), HI(9),
     +                   XF(3), PF(3),  HF(9),
     +                   CHTR, DEDX2, BACKTR, CUTEK, TLGCM2, TLRAD
      COMMON/GCCHAN/LSAMVL
      LOGICAL LSAMVL
*
*
      DIMENSION CUTS(10),MECA(5,12)
      EQUIVALENCE (CUTS(1),CUTGAM),(MECA(1,1),IPAIR)
      DIMENSION NAMIN(15),NUMIN(15),NAMOUT(15),NUMOUT(15)
*
      SAVE PRECOR,NSTOUT
      PARAMETER (EPSMAC=5.E-6)
C.
C.    ------------------------------------------------------------------
*
      NSTOUT = 0
      EPSCUR = EPSMAC
      LSAMVL = .FALSE.
      SLENG  = 0.
      ISTOP  = 0
      NUMED  = 0
      NUMOLD = 0
      IUPD   = 0
      NMEC   = 0
      INGOTO = 0
      INFROM = 0
      SAFETY = 0.
      MXNSTP = 1000
      NSTEP  = 0
*
      CALL GMEDIA(VECT,NUMED)
      IF (NUMED.EQ.0) GO TO 200
*
* *** Come back here each time we enter into a new volume
*
   10 CONTINUE
*
* *** Get tracking medium and material parameters
      IF (NUMED.NE.NUMOLD) THEN
         NUMOLD = NUMED
         IUPD   = 0
         JTM = LQ(JTMED- NUMED)
         DO 20 I=1,5
            NATMED(I)=IQ(JTM+I)
  20     CONTINUE
         NMAT   = Q(JTM + 6)
         ISVOL  = Q(JTM + 7)
         IFIELD = Q(JTM + 8)
         FIELDM = Q(JTM + 9)
         TMAXFD = Q(JTM + 10)
         DMAXMS = Q(JTM + 11)
         DEEMAX = Q(JTM + 12)
         EPSIL  = Q(JTM + 13)
         STMIN  = Q(JTM + 14)
         PRECOR   = MIN(0.1*EPSIL, 0.0010)
*
         IF(LQ(JTM).EQ.0)THEN
            IF(ISTPAR.NE.0)THEN
               DO 30 I=1,10
                  CUTS(I)=Q(JTMED+I)
  30           CONTINUE
               DO 40 I=1,12
                  MECA(1,I)=Q(JTMED+10+I)
  40           CONTINUE
               ISTPAR=0
            ENDIF
         ELSE
            JTMN=LQ(JTM)
            DO 50 I=1,10
               CUTS(I)=Q(JTMN+I)
  50        CONTINUE
            DO 60 I=1,12
               MECA(1,I)=Q(JTMN+10+I)
  60        CONTINUE
            ILABS = Q(JTMN+10+21)
            ISYNC = Q(JTMN+10+22)
            ISTRA = Q(JTMN+10+23)
            ISTPAR=1
         ENDIF
*
         JMA  = LQ(JMATE- NMAT)
         JPROB=LQ(JMA-4)
         JMIXT=LQ(JMA-5)
         DO 70 I=1,5
  70     NAMATE(I)=IQ(JMA+I)
         A    = Q(JMA + 6)
         Z    = Q(JMA + 7)
         DENS = Q(JMA + 8)
         RADL = Q(JMA + 9)
         ABSL = Q(JMA + 10)
      ENDIF
*
      IF(LSAMVL) THEN
*
*       If now the particle is entering in the same volume where
*       it was exiting from last step, and if it has done this for
*       more than 5 times, we decrease the precision of tracking
         NSTOUT=NSTOUT+1
         IF(MOD(NSTOUT,5).EQ.0) THEN
            EPSCUR=NSTOUT*EPSMAC
c            WRITE(CHMAIL,10000)ITRA,ISTAK,NTMULT,NAPART
c10000          FORMAT(' *** ERTRGO *** Boundary loop: track ',
c     +         I4,' stack ',I4,' NTMULT ',I5,1X,5A4)
c            CALL GMAIL(1,0)
c            WRITE(CHMAIL,10100) EPSCUR
c10100          FORMAT('                Precision now set to ',G10.3)
c            CALL GMAIL(0,1)
         ENDIF
      ELSE
         NSTOUT = 0
         EPSCUR = EPSMAC
      ENDIF
*
* *** Initialize magnetic field for EMC package
      HI(1) = 0.
      HI(2) = 0.
      HI(3) = 0.
      IF (IFIELD.EQ.3) THEN
         HI(3) = FIELDM
      ELSEIF (IFIELD.NE.0) THEN
         CALL GUFLD (VECT, HI)
      ENDIF
*
* *** Control given to user at entrance of volume (INWVOL=1)
      INWVOL = 1
      NMEC   = 1
      LMEC(1) = 29
      STEP   = 0.
      DESTEP = 0.
*
      IF((LEVOLU).AND.(SLENG.GT.0.)) THEN
         IMEC = 0
         CALL EVOLIO(NVLIN,NAMIN,NUMIN,NVLOUT,NAMOUT,NUMOUT)
         DO 80 IPR =1,NEPRED
            NAMPR = NAMEER(IPR)
            NUMPR = NUMVER(IPR)
            IOVPR = IOVLER(IPR)
            IF (IOVPR.EQ.1) THEN
               IV = IUCOMP(NAMPR,NAMIN ,NVLIN )
               IF (IV.NE.0) THEN
                  IF (NUMPR.EQ.0) NUMPR = NUMIN (IV)
                  IF (NUMPR.EQ.NUMIN (IV)) THEN
                     NMEC = NMEC + 1
                     LMEC(NMEC) = 27
                     INLIST = IPR
                     CALL ERSTOR
                  ENDIF
               ENDIF
            ELSE IF (IOVPR.EQ.2) THEN
               IV = IUCOMP(NAMPR,NAMOUT,NVLOUT)
               IF (IV.NE.0) THEN
                  IF (NUMPR.EQ.0) NUMPR = NUMOUT(IV)
                  IF (NUMPR.EQ.NUMOUT(IV)) THEN
                     NMEC = NMEC + 1
                     LMEC(NMEC) = 27
                     INLIST = IPR
                     CALL ERSTOR
                  ENDIF
               ENDIF
            ENDIF
*
   80    CONTINUE
*
      ENDIF
*
      CALL EUSTEP
      IF (ISTOP.NE.0) GO TO 999
*
* *** Particle is propagated up to the next volume boundary
*
      INWVOL=0
*
* *** Come back here after each step in the same volume
 100  IGNEXT = 0
      INGOTO = 0
      NLEVIN = NLEVEL
      NMEC   = 0
      STEP   = 0.
      DESTEP = 0.
      DEDX2  = 0.
      PREC   = MAX(PRECOR,MAX(ABS(VECT(1)),ABS(VECT(2)),
     +                        ABS(VECT(3)),SLENG)*EPSCUR)
*
      IF(CHARGE.NE.0.) THEN
         CALL ERTRCH
      ELSE
         CALL ERTRNT
      ENDIF
*
      NSTEP  = NSTEP + 1
      IF (NSTEP.GT.MXNSTP) THEN
         ISTOP = 99
         NMEC  = NMEC + 1
         LMEC(NMEC) = 30
      ENDIF
*
      SAFETY = SAFETY - STEP
      TLRAD  = TLRAD  + STEP/RADL
      TLGCM2 = TLGCM2 + STEP*DENS
*
* *** Give control to user after each tracking step
      CALL EUSTEP
*
      IF(ISTOP.NE.0) GO TO 999
*
* *** Renormalize direction cosines
      CMOD = 1./SQRT(VECT(4)**2 + VECT(5)**2 + VECT(6)**2)
      VECT(4) = VECT(4)*CMOD
      VECT(5) = VECT(5)*CMOD
      VECT(6) = VECT(6)*CMOD
*
      IF (INWVOL.EQ.0) GO TO 100
*
* *** Particle is leaving the volume (INWVOL=2) :
*
*     Save the current volume's tree before leaving the volume
      IF(LEVOLU) CALL EVOLIO(NVLIN,NAMIN,NUMIN,NVLOUT,NAMOUT,NUMOUT)
*
*     find the new volume
      IF (NLEVIN.GE.NLEVEL) THEN
         INFROM = 0
      ELSE
         IF (NLEVIN.EQ.0) GO TO 200
         INFROM = LINDEX (NLEVIN+1)
      ENDIF
      IF (NLEVIN.NE.NLEVEL) INGOTO = 0
      NLEVEL = NLEVIN
*
      CALL GTMEDI (VECT,NUMED)
      IF (NUMED.NE.0) THEN
         SAFETY = 0.
         GO TO 10
      ENDIF
*
* *** Track outside setup, give control to user (INWVOL=3)
  200 INWVOL= 3
      ISTOP = 1
      NMEC  = NMEC + 1
      LMEC(NMEC) = 30
      CALL EUSTEP
 999  CONTINUE
      ILOSL = 0
*
      END
CDECK  ID>, ERTRNT. 
*CMZ :  3.21/02 29/03/94  15.41.49  by  S.Giani
*-- Author :
      SUBROUTINE ERTRNT
C.
C.    ******************************************************************
C.    *                                                                *
C.    *    Average neutral track is extrapolated by one step           *
C.    *                                                                *
C.    *    ==>Called by : ERTRGO                                       *
C.    *       Original routine : GTNINO                                *
C.    *       Authors   M.Maire, E.Nagy  *********                     *
C.    *                                                                *
C.    ******************************************************************
C.
      INTEGER          MXPRED
      PARAMETER (MXPRED = 150)
      DOUBLE PRECISION ERDTRP
      REAL             ERRIN, ERROUT, ERTRSP, ERXIN, ERXOUT, ERPIN,
     +                 ERPOUT
      INTEGER          NEPRED, INLIST, ILPRED, IEPRED
      COMMON /ERTRIO/  ERDTRP(5,5,MXPRED), ERRIN(15), ERROUT(15,MXPRED),
     +                 ERTRSP(5,5,MXPRED), ERXIN( 3), ERXOUT( 3,MXPRED),
     +                 ERPIN(3), ERPOUT(3,MXPRED), NEPRED,INLIST,ILPRED,
     +                 IEPRED(MXPRED)
      CHARACTER*8     CHOPTI
      LOGICAL         LEEXAC, LELENG, LEONLY, LEPLAN, LEPOIN, LEVOLU
      REAL            ERPLI, ERPLO, ERLENG
      INTEGER         NAMEER, NUMVER, IOVLER
      COMMON /EROPTS/ ERPLI(3,2), ERPLO(3,4,MXPRED), ERLENG(MXPRED),
     +                NAMEER(MXPRED), NUMVER(MXPRED), IOVLER(MXPRED),
     +                LEEXAC, LELENG, LEONLY, LEPLAN, LEPOIN, LEVOLU
      COMMON /EROPTC/CHOPTI


      REAL          PI,TWOPI,PIBY2,DEGRAD,RADDEG,CLIGHT,BIG,EMASS
      REAL          EMMU,PMASS,AVO
C
      COMMON/GCONST/PI,TWOPI,PIBY2,DEGRAD,RADDEG,CLIGHT,BIG,EMASS
      COMMON/GCONSX/EMMU,PMASS,AVO
C
      COMMON/GCTMED/NUMED,NATMED(5),ISVOL,IFIELD,FIELDM,TMAXFD,STEMAX
     +      ,DEEMAX,EPSIL,STMIN,CFIELD,PREC,IUPD,ISTPAR,NUMOLD
      COMMON/GCTLIT/THRIND,PMIN,DP,DNDL,JMIN,ITCKOV,IMCKOV,NPCKOV
C
      INTEGER       NUMED,NATMED,ISVOL,IFIELD,IUPD,ISTPAR,NUMOLD
      REAL          FIELDM,TMAXFD,STEMAX,DEEMAX,EPSIL,STMIN,CFIELD,PREC
      INTEGER       JMIN,NPCKOV,IMCKOV,ITCKOV
      REAL          THRIND,PMIN,DP,DNDL
C
      INTEGER NMEC,LMEC,NAMEC,NSTEP ,MAXNST,IGNEXT,INWVOL,ISTOP,MAXMEC
     + ,IGAUTO,IEKBIN,ILOSL, IMULL,INGOTO,NLDOWN,NLEVIN,NLVSAV,ISTORY
     + ,MAXME1,NAMEC1
      REAL  VECT,GETOT,GEKIN,VOUT,DESTEP,DESTEL,SAFETY,SLENG ,STEP
     + ,SNEXT,SFIELD,TOFG  ,GEKRAT,UPWGHT
      REAL POLAR
      PARAMETER (MAXMEC=30)
      COMMON/GCTRAK/VECT(7),GETOT,GEKIN,VOUT(7),NMEC,LMEC(MAXMEC)
     + ,NAMEC(MAXMEC),NSTEP ,MAXNST,DESTEP,DESTEL,SAFETY,SLENG
     + ,STEP  ,SNEXT ,SFIELD,TOFG  ,GEKRAT,UPWGHT,IGNEXT,INWVOL
     + ,ISTOP ,IGAUTO,IEKBIN, ILOSL, IMULL,INGOTO,NLDOWN,NLEVIN
     + ,NLVSAV,ISTORY
      PARAMETER (MAXME1=30)
      COMMON/GCTPOL/POLAR(3), NAMEC1(MAXME1)
C
*
* *** Compute point where to store error matrix
*
      STEPER = BIG
      DO 20 IPR = 1,NEPRED
         STEPE  = BIG
         IF (LELENG) STEPE = ERLENG(IPR) - SLENG
         IF (LEPLAN) THEN
            SCAL1 = 0.
            SCAL2 = 0.
            DO 18 I=1,3
               SCAL1 = SCAL1 + ERPLO(I,4,IPR)*(ERPLO(I,3,IPR)-VECT(I))
               SCAL2 = SCAL2 + ERPLO(I,4,IPR)*VECT(I+3)
   18       CONTINUE
            STEPE = SCAL1/SCAL2
         ENDIF
         IF (STEPE.LE.PREC) STEPE = BIG
         IF (STEPE.LT.STEPER) THEN
            STEPER = STEPE
            INLIST = IPR
         ENDIF
   20 CONTINUE
*
      STEP = BIG
      CALL GTNEXT
*
      IF(STEPER.LT.SNEXT) THEN
         STEP = STEPER
         INWVOL = 0
         IMEC   = 27
         NMEC   = NMEC + 1
         LMEC(NMEC) = IMEC
      ELSE
         STEP = SNEXT + PREC
         INWVOL = 2
         IMEC   = 1
         NMEC   = NMEC + 1
         LMEC(NMEC) = IMEC
         IF ((STEPER-SNEXT).LE.(2*PREC)) THEN
            IMEC   = 27
            NMEC   = NMEC + 1
            LMEC(NMEC) = IMEC
         ENDIF
      ENDIF
*
* *** Linear transport
*
      DO 25 I=1,3
         VECT(I) = VECT(I) + STEP*VECT(I+3)
   25 CONTINUE
*
      SLENG = SLENG + STEP
*
* *** Propagate error matrix
*
      IF (.NOT. LEONLY) CALL ERPROP
*
* *** Store informations
*
      IF(IMEC.EQ.27) CALL ERSTOR
*
      END
CDECK  ID>, ERXYZC. 
*CMZ :  3.21/02 29/03/94  15.41.49  by  S.Giani
*-- Author :
      SUBROUTINE ERXYZC
C.
C.    ******************************************************************
C.    *                                                                *
C.    *        Print track and volume parameters at current point      *
C.    *                                                                *
C.    *    ==>Called by : <USER,EUSTEP>                                *
C.    *       Author    R.Brun  *********                              *
C.    *                                                                *
C.    ******************************************************************
C.
      COMMON/GCKINE/IKINE,PKINE(10),ITRA,ISTAK,IVERT,IPART,ITRTYP
     +      ,NAPART(5),AMASS,CHARGE,TLIFE,VERT(3),PVERT(4),IPAOLD
C
      INTEGER       IKINE,ITRA,ISTAK,IVERT,IPART,ITRTYP,NAPART,IPAOLD
      REAL          PKINE,AMASS,CHARGE,TLIFE,VERT,PVERT
C
      INTEGER NMEC,LMEC,NAMEC,NSTEP ,MAXNST,IGNEXT,INWVOL,ISTOP,MAXMEC
     + ,IGAUTO,IEKBIN,ILOSL, IMULL,INGOTO,NLDOWN,NLEVIN,NLVSAV,ISTORY
     + ,MAXME1,NAMEC1
      REAL  VECT,GETOT,GEKIN,VOUT,DESTEP,DESTEL,SAFETY,SLENG ,STEP
     + ,SNEXT,SFIELD,TOFG  ,GEKRAT,UPWGHT
      REAL POLAR
      PARAMETER (MAXMEC=30)
      COMMON/GCTRAK/VECT(7),GETOT,GEKIN,VOUT(7),NMEC,LMEC(MAXMEC)
     + ,NAMEC(MAXMEC),NSTEP ,MAXNST,DESTEP,DESTEL,SAFETY,SLENG
     + ,STEP  ,SNEXT ,SFIELD,TOFG  ,GEKRAT,UPWGHT,IGNEXT,INWVOL
     + ,ISTOP ,IGAUTO,IEKBIN, ILOSL, IMULL,INGOTO,NLDOWN,NLEVIN
     + ,NLVSAV,ISTORY
      PARAMETER (MAXME1=30)
      COMMON/GCTPOL/POLAR(3), NAMEC1(MAXME1)
C
      COMMON/GCVOLU/NLEVEL,NAMES(15),NUMBER(15),
     +LVOLUM(15),LINDEX(15),INFROM,NLEVMX,NLDEV(15),LINMX(15),
     +GTRAN(3,15),GRMAT(10,15),GONLY(15),GLX(3)
C
      INTEGER NLEVEL,NAMES,NUMBER,LVOLUM,LINDEX,INFROM,NLEVMX,
     +        NLDEV,LINMX
      REAL GTRAN,GRMAT,GONLY,GLX
      COMMON/GCUNIT/LIN,LOUT,NUNITS,LUNITS(5)
      INTEGER LIN,LOUT,NUNITS,LUNITS
      COMMON/GCMAIL/CHMAIL
      CHARACTER*132 CHMAIL
C
      INTEGER          MXPRED
      PARAMETER (MXPRED = 150)
      DOUBLE PRECISION ERDTRP
      REAL             ERRIN, ERROUT, ERTRSP, ERXIN, ERXOUT, ERPIN,
     +                 ERPOUT
      INTEGER          NEPRED, INLIST, ILPRED, IEPRED
      COMMON /ERTRIO/  ERDTRP(5,5,MXPRED), ERRIN(15), ERROUT(15,MXPRED),
     +                 ERTRSP(5,5,MXPRED), ERXIN( 3), ERXOUT( 3,MXPRED),
     +                 ERPIN(3), ERPOUT(3,MXPRED), NEPRED,INLIST,ILPRED,
     +                 IEPRED(MXPRED)
      CHARACTER*8     CHOPTI
      LOGICAL         LEEXAC, LELENG, LEONLY, LEPLAN, LEPOIN, LEVOLU
      REAL            ERPLI, ERPLO, ERLENG
      INTEGER         NAMEER, NUMVER, IOVLER
      COMMON /EROPTS/ ERPLI(3,2), ERPLO(3,4,MXPRED), ERLENG(MXPRED),
     +                NAMEER(MXPRED), NUMVER(MXPRED), IOVLER(MXPRED),
     +                LEEXAC, LELENG, LEONLY, LEPLAN, LEPOIN, LEVOLU
      COMMON /EROPTC/CHOPTI


*
      DIMENSION MECNAM(20)
      CHARACTER*4 KUN1,KUN2
*
      DO 10 I=1,NMEC
         MEC=LMEC(I)
         MECNAM(I)=NAMEC(MEC)
   10 CONTINUE
*
      IF (SLENG.LE.0.) THEN
         WRITE(CHMAIL,1000) (NAPART(I),I=1,5),NEPRED,CHOPTI
         CALL GMAIL(0,0)
         WRITE(CHMAIL,1100)
         CALL GMAIL(0,0)
      ENDIF
*
      R = SQRT(VECT(1)**2+VECT(2)**2)
      CALL GEVKEV (DESTEP,DESU,KUN1)
      CALL GEVKEV (GEKIN ,GEKU,KUN2)
      WRITE(CHMAIL,2000)(VECT(I),I=1,3),R,NAMES(NLEVEL),NUMBER(NLEVEL)
     +      ,SLENG,STEP,DESU,KUN1,GEKU,KUN2,(MECNAM(I),I=1,NMEC)
      CALL GMAIL(0,0)
C
 1000 FORMAT(' =====> starting GEANE tracking for  ',5A4,' NEPRED =',I3,
     +       '  with options : ',A8)
 1100 FORMAT('      X         Y         Z         R      NAME  NUMBER',
     +'   SLENG      STEP      DESTEP     GEKIN    MECHANISMS')
 2000 FORMAT(1X,4F10.4,2X,A4,2X,I4,2X,2F10.4,F7.1,A4,F9.3,A4,2X,
     +          6(A4,1X))
*
      END
CDECK  ID>, EUFILL. 
*CMZ :  3.21/02 29/03/94  15.41.49  by  S.Giani
*-- Author :
      SUBROUTINE EUFILL (N, EIN, XLF)
*
C.    ******************************************************************
C.    *                                                                *
C.    *    User routine to fill the input values of the commons :      *
C.    *               /EROPTS/, /EROPTC/ and /ERTRIO/ for CHOPT = 'L'  *
C.    *         N     Number of predictions where to store results     *
C.    *         EIN   Input error matrix                               *
C.    *         XLF   Defines the tracklengths which if passed the     *
C.    *                      result should be stored                   *
C.    *                                                                *
C.    *                                                                *
C.    *    ==>Called by : USER (before calling ERTRAK)                 *
C.    *       Author    M.Maire, E.Nagy  *********                     *
C.    *                                                                *
C.    ******************************************************************
C.
      INTEGER          MXPRED
      PARAMETER (MXPRED = 150)
      DOUBLE PRECISION ERDTRP
      REAL             ERRIN, ERROUT, ERTRSP, ERXIN, ERXOUT, ERPIN,
     +                 ERPOUT
      INTEGER          NEPRED, INLIST, ILPRED, IEPRED
      COMMON /ERTRIO/  ERDTRP(5,5,MXPRED), ERRIN(15), ERROUT(15,MXPRED),
     +                 ERTRSP(5,5,MXPRED), ERXIN( 3), ERXOUT( 3,MXPRED),
     +                 ERPIN(3), ERPOUT(3,MXPRED), NEPRED,INLIST,ILPRED,
     +                 IEPRED(MXPRED)
      CHARACTER*8     CHOPTI
      LOGICAL         LEEXAC, LELENG, LEONLY, LEPLAN, LEPOIN, LEVOLU
      REAL            ERPLI, ERPLO, ERLENG
      INTEGER         NAMEER, NUMVER, IOVLER
      COMMON /EROPTS/ ERPLI(3,2), ERPLO(3,4,MXPRED), ERLENG(MXPRED),
     +                NAMEER(MXPRED), NUMVER(MXPRED), IOVLER(MXPRED),
     +                LEEXAC, LELENG, LEONLY, LEPLAN, LEPOIN, LEVOLU
      COMMON /EROPTC/CHOPTI


      DIMENSION EIN(15), XLF(N)
*
      NEPRED = MIN0 (N, MXPRED)
*
      CALL UCOPY (EIN(1), ERRIN(1),  15)
      CALL UCOPY (XLF(1), ERLENG(1), NEPRED)
*
*                                                           END EUFILL
  99  END
CDECK  ID>, EUFILP. 
*CMZ :  3.21/02 29/03/94  15.41.49  by  S.Giani
*-- Author :
      SUBROUTINE EUFILP (N, EIN, PLI, PLF)
*
C.    ******************************************************************
C.    *                                                                *
C.    *    User routine to fill the input values of the commons :      *
C.    *               /EROPTS/, /EROPTC/ and /ERTRIO/ for CHOPT = 'P'  *
C.    *         N     Number of predictions where to store results     *
C.    *         EIN   Input error matrix (in the 'Plane' system )      *
C.    *         PLI   Defines the start plane                          *
C.    *                      PLI(3,1) - and                            *
C.    *                      PLI(3,2) - 2 unit vectors in the plane    *
C.    *         PLF   Defines the end plane                            *
C.    *                      PLF(3,1,I) - and                          *
C.    *                      PLF(3,2,I) - 2 unit vectors in the plane  *
C.    *                      PLF(3,3,I) - point on the plane           *
C.    *                                   at intermediate point I      *
C.    *                                                                *
C.    *    ==>Called by : USER (before calling ERTRAK)                 *
C.    *       Author    M.Maire, E.Nagy  *********                     *
C.    *                                                                *
C.    ******************************************************************
C.
      INTEGER          MXPRED
      PARAMETER (MXPRED = 150)
      DOUBLE PRECISION ERDTRP
      REAL             ERRIN, ERROUT, ERTRSP, ERXIN, ERXOUT, ERPIN,
     +                 ERPOUT
      INTEGER          NEPRED, INLIST, ILPRED, IEPRED
      COMMON /ERTRIO/  ERDTRP(5,5,MXPRED), ERRIN(15), ERROUT(15,MXPRED),
     +                 ERTRSP(5,5,MXPRED), ERXIN( 3), ERXOUT( 3,MXPRED),
     +                 ERPIN(3), ERPOUT(3,MXPRED), NEPRED,INLIST,ILPRED,
     +                 IEPRED(MXPRED)
      CHARACTER*8     CHOPTI
      LOGICAL         LEEXAC, LELENG, LEONLY, LEPLAN, LEPOIN, LEVOLU
      REAL            ERPLI, ERPLO, ERLENG
      INTEGER         NAMEER, NUMVER, IOVLER
      COMMON /EROPTS/ ERPLI(3,2), ERPLO(3,4,MXPRED), ERLENG(MXPRED),
     +                NAMEER(MXPRED), NUMVER(MXPRED), IOVLER(MXPRED),
     +                LEEXAC, LELENG, LEONLY, LEPLAN, LEPOIN, LEVOLU
      COMMON /EROPTC/CHOPTI


      DIMENSION EIN(15), PLF(3,3,N), PLI(3,2),PWORK(3)
*
      NEPRED = MIN0 (N, MXPRED)
*
      CALL UCOPY (EIN(1), ERRIN(1), 15)
      CALL UCOPY (PLI(1,1), ERPLI(1,1), 6)
      DO 5 I = 1, NEPRED
         CALL UCOPY (PLF(1,1,I), ERPLO(1,1,I), 9)
         CALL CROSS (PLF(1,1,I), PLF(1,2,I), PWORK(1))
         PNORM = VMOD(PWORK,3)
         CALL VSCALE(PWORK,PNORM,ERPLO(1,4,I),3)
    5 CONTINUE
*
*                                                           END EUFILP
  99  END
CDECK  ID>, EUFILV. 
*CMZ :  3.21/02 29/03/94  15.41.49  by  S.Giani
*-- Author :
      SUBROUTINE EUFILV (N, EIN, CNAMV, NUMV, IOVL)
*
C.    ******************************************************************
C.    *                                                                *
C.    *    User routine to fill the input values of the commons :      *
C.    *               /EROPTS/, /EROPTC/ and /ERTRIO/ for CHOPT = 'V'  *
C.    *         N     Number of predictions where to store results     *
C.    *         EIN   Input error matrix                               *
C.    *        CNAMV  Volume name of the prediction                    *
C.    *        NUMV   Volume number (if 0 = all volumes)               *
C.    *        IOVL   = 1  prediction when entering in the volume      *
C.    *               = 2  prediction when leaving the volume          *
C.    *                                                                *
C.    *    ==>Called by : USER (before calling ERTRAK)                 *
C.    *       Author    M.Maire, E.Nagy  *********                     *
C.    *                                                                *
C.    ******************************************************************
C.
      INTEGER          MXPRED
      PARAMETER (MXPRED = 150)
      DOUBLE PRECISION ERDTRP
      REAL             ERRIN, ERROUT, ERTRSP, ERXIN, ERXOUT, ERPIN,
     +                 ERPOUT
      INTEGER          NEPRED, INLIST, ILPRED, IEPRED
      COMMON /ERTRIO/  ERDTRP(5,5,MXPRED), ERRIN(15), ERROUT(15,MXPRED),
     +                 ERTRSP(5,5,MXPRED), ERXIN( 3), ERXOUT( 3,MXPRED),
     +                 ERPIN(3), ERPOUT(3,MXPRED), NEPRED,INLIST,ILPRED,
     +                 IEPRED(MXPRED)
      CHARACTER*8     CHOPTI
      LOGICAL         LEEXAC, LELENG, LEONLY, LEPLAN, LEPOIN, LEVOLU
      REAL            ERPLI, ERPLO, ERLENG
      INTEGER         NAMEER, NUMVER, IOVLER
      COMMON /EROPTS/ ERPLI(3,2), ERPLO(3,4,MXPRED), ERLENG(MXPRED),
     +                NAMEER(MXPRED), NUMVER(MXPRED), IOVLER(MXPRED),
     +                LEEXAC, LELENG, LEONLY, LEPLAN, LEPOIN, LEVOLU
      COMMON /EROPTC/CHOPTI


      DIMENSION EIN(15), NUMV(N), IOVL(N)
      CHARACTER * 4 CNAMV(N)
*
      NEPRED = MIN0 (N, MXPRED)
*
      CALL UCOPY (EIN(1), ERRIN(1),  15)
      DO I=1,NEPRED
        CALL UCTOH (CNAMV(I),NAMEER(I),4,4)
      ENDDO
      CALL UCOPY (NUMV(1),NUMVER(1),NEPRED)
      CALL UCOPY (IOVL(1),IOVLER(1),NEPRED)
*
*                                                           END EUFILV
  99  END
CDECK  ID>, EUSTEP. 
*CMZ :  3.21/02 29/03/94  15.41.49  by  S.Giani
*-- Author :
      SUBROUTINE EUSTEP
*
      COMMON/GCFLAG/IDEBUG,IDEMIN,IDEMAX,ITEST,IDRUN,IDEVT,IEORUN
     +        ,IEOTRI,IEVENT,ISWIT(10),IFINIT(20),NEVENT,NRNDM(2)
      COMMON/GCFLAX/BATCH, NOLOG
      LOGICAL BATCH, NOLOG
C
      INTEGER       IDEBUG,IDEMIN,IDEMAX,ITEST,IDRUN,IDEVT,IEORUN
     +        ,IEOTRI,IEVENT,ISWIT,IFINIT,NEVENT,NRNDM
C
*
*
      IF (IDEBUG*ISWIT(2).NE.0) CALL ERXYZC
*
      END
CDECK  ID>, EVOLIO. 
*CMZ :  3.21/02 29/03/94  15.41.49  by  S.Giani
*-- Author :
      SUBROUTINE EVOLIO (NVLIN,NAMIN,NUMIN,NVLOUT,NAMOUT,NUMOUT)
*
C.    ******************************************************************
C.    *                                                                *
C.    *    Called with INWVOL=2 this routine save the volume's tree    *
C.    *                                                                *
C.    *    Called with INWVOL=1 this routine returns                   *
C.    *             - the list of volumes (and number) left            *
C.    *             - the list of volumes (and number) entered         *
C.    *                                                                *
C.    *    ==>Called by : ERTRGO                                       *
C.    *       Author    M.Maire  *********                             *
C.    *                                                                *
C.    ******************************************************************
C.
      COMMON/GCVOLU/NLEVEL,NAMES(15),NUMBER(15),
     +LVOLUM(15),LINDEX(15),INFROM,NLEVMX,NLDEV(15),LINMX(15),
     +GTRAN(3,15),GRMAT(10,15),GONLY(15),GLX(3)
C
      INTEGER NLEVEL,NAMES,NUMBER,LVOLUM,LINDEX,INFROM,NLEVMX,
     +        NLDEV,LINMX
      REAL GTRAN,GRMAT,GONLY,GLX
      INTEGER NMEC,LMEC,NAMEC,NSTEP ,MAXNST,IGNEXT,INWVOL,ISTOP,MAXMEC
     + ,IGAUTO,IEKBIN,ILOSL, IMULL,INGOTO,NLDOWN,NLEVIN,NLVSAV,ISTORY
     + ,MAXME1,NAMEC1
      REAL  VECT,GETOT,GEKIN,VOUT,DESTEP,DESTEL,SAFETY,SLENG ,STEP
     + ,SNEXT,SFIELD,TOFG  ,GEKRAT,UPWGHT
      REAL POLAR
      PARAMETER (MAXMEC=30)
      COMMON/GCTRAK/VECT(7),GETOT,GEKIN,VOUT(7),NMEC,LMEC(MAXMEC)
     + ,NAMEC(MAXMEC),NSTEP ,MAXNST,DESTEP,DESTEL,SAFETY,SLENG
     + ,STEP  ,SNEXT ,SFIELD,TOFG  ,GEKRAT,UPWGHT,IGNEXT,INWVOL
     + ,ISTOP ,IGAUTO,IEKBIN, ILOSL, IMULL,INGOTO,NLDOWN,NLEVIN
     + ,NLVSAV,ISTORY
      PARAMETER (MAXME1=30)
      COMMON/GCTPOL/POLAR(3), NAMEC1(MAXME1)
C
*
      PARAMETER (NVMX=15)
      COMMON/OLDTRE/NLVOLD,NAMOLD(NVMX),NUMOLD(NVMX),IOFLAG(NVMX),KONLY
*
      DIMENSION NAMIN(NVMX),NUMIN(NVMX),NAMOUT(NVMX),NUMOUT(NVMX)
*
*
      NVLOUT = 0
      NVLIN  = 0
      IF(INWVOL.EQ.0) RETURN
      IF(NLEVEL.EQ.0) RETURN
*
* *** Save the tree when leaving a volume
*
      IF(INWVOL.EQ.2) THEN
         NLVOLD = NLEVEL
         KONLY  = 1
         DO 1 ILV =1,NLEVEL
           NAMOLD(ILV) = NAMES (ILV)
           NUMOLD(ILV) = NUMBER(ILV)
           IOFLAG(ILV) = 2
           IF(GONLY(ILV).NE.1.) KONLY = 0
    1    CONTINUE
         RETURN
      ENDIF
*
* *** Entering in a new volume : compare the trees
*
      IF(INWVOL.NE.1) RETURN
*
* *** Simplest case : old tree is 'ONLY'
*
      IF(KONLY.EQ.1) THEN
        NLMIN = MIN(NLEVEL,NLVOLD)
        NL    = 0
   10   NL    = NL + 1
        IF(NL.GT.NLMIN) GO TO 12
        IF((NAMES (NL).EQ.NAMOLD(NL)).AND.
     *     (NUMBER(NL).EQ.NUMOLD(NL))) GO TO 10
*
   12   CONTINUE
        IF(NL.LE.NLVOLD) THEN
          DO 14 IL = NL,NLVOLD
            NVLOUT = NVLOUT + 1
            NAMOUT(NVLOUT) = NAMOLD(IL)
            NUMOUT(NVLOUT) = NUMOLD(IL)
   14     CONTINUE
        ENDIF
*
        IF(NL.LE.NLEVEL) THEN
          DO 16 IL = NL,NLEVEL
            NVLIN  = NVLIN  + 1
            NAMIN (NVLIN ) = NAMES (IL)
            NUMIN (NVLIN ) = NUMBER(IL)
   16     CONTINUE
        ENDIF
*
        RETURN
      ENDIF
*
* *** Special case for 'NOT ONLY' volumes
*
      IF(KONLY.EQ.0) THEN
         DO 24 IL1 = 1,NLEVEL
           DO 22 IL2 = 1,NLVOLD
              IF((NAMES (IL1).EQ.NAMOLD(IL2)).AND.
     *           (NUMBER(IL1).EQ.NUMOLD(IL2))) THEN
                   IOFLAG(IL2) = 0
                   GO TO 24
              ENDIF
   22      CONTINUE
           NVLIN  = NVLIN  + 1
           NAMIN (NVLIN ) = NAMES (IL1)
           NUMIN (NVLIN ) = NUMBER(IL1)
   24    CONTINUE
*
         DO 26 IL2 = 1,NLVOLD
           IF(IOFLAG(IL2).EQ.0) GO TO 26
           NVLOUT = NVLOUT + 1
           NAMOUT(NVLOUT) = NAMOLD(IL2)
           NUMOUT(NVLOUT) = NUMOLD(IL2)
   26    CONTINUE
*
      ENDIF
*
      END
CDECK  ID>, TRPRFN. 
*CMZ :  3.21/02 29/03/94  15.41.49  by  S.Giani
*-- Author :
      SUBROUTINE TRPRFN(X1,P1,H1,X2,P2,H2,CH,XL,R,MVAR,IFLAG,ITRAN,IERR)
************************************************************************
*
*
*     SUBR. TRPRFN(X1,P1,H1,X2,P2,H2,CH,XL,*R*,MVAR,IFLAG,ITRAN,IERR*)
*
*     Origin W.Wittek    EMCSW/81/18
*
*     Finite step length case coded by V.Innocente ( Feb. 88 )
*
*     code improved:                   V.Innocente ( April. 90 )
*                   inline code replaces external function
*     code improved:                   V.Innocente ( January 91 )
*                   effect of energy loss added
*
*_______________________________________________________________________
*
* *** ERROR PROPAGATION ALONG A PARTICLE TRAJECTORY IN A MAGNETIC FIELD
*     ROUTINE ASSUMES THAT IN THE INTERVAL (X1,X2) THE QUANTITIES 1/P
*     AND (HX,HY,HZ) ARE CONSTANT.
*
* *** IFLAG  =  -1   INITIALIZATION, TRANSFORMATION OF ERROR MATRIX FROM
*                    EXTERNAL TO SC VARIABLES
*            =   0   ERROR PROPAGATION FROM X1 TO X2
*            =   1   TRANSFORMATION OF ERROR MATRIX FROM SC TO
*                    EXTERNAL VARIABLES
*
*     ITRAN          USED FOR IFLAG = 0 OR 1 ONLY
*            =   0   TRANSFORMATION MATRIX IS UPDATED ,BUT ERROR MATRIX IS NOT
*                    TRANSFORMED
*           =    1   TRANSF. MATRIX IS UPDATED  AND ERROR MATRIX IS TRANSFORMED
*
*     MVAR           SPECIFIES TYPE OF EXTERNAL VARIABLES
*            =   0   ( 1/P,LAMBDA,PHI,YT, ZT ;   SC   )
*            =   1   ( 1/P,  Y',  Z',  Y,  Z ; SPLINE )
*
* *** X1, P1, H1     X,Y,Z COMPONENTS OF POSITION, MOMENTUM AND MAGNETIC   INPUT
*                    FIELD VECTOR/GRADIENT AT STARTING POINT OF INTERVAL
*     X2, P2, H2     ......  AT END POINT OF INTERVAL                      INPUT
*     CH             CHARGE OF PARTICLE                                    INPUT
*     XL             PATHLENGTH FROM X1 TO X2   ( NEGATIVE IF OPPOSITE
*                    TO ACTUAL MOVEMENT OF PARTICLE )                      INPUT
*     R              ERROR MATRIX  (TRIANGLE)                       INPUT/OUTPUT
*     B              5 * 5 TRANSFORMATION MATRIX FOR ERRORS IN
*                    SC VARIABLES                                         OUTPUT
*
* *** IERR   =  1    ILLEGAL VALUE OF MVAR                                OUTPUT
*               2    MOMENTUM IS ZERO
*               3    H*ALFA/P AT X1 AND X2 DIFFER TOO MUCH
*               4    PARTICLE MOVES IN Z - DIRECTION
*
************************************************************************
*
      IMPLICIT DOUBLE PRECISION (A-H,O-Z)
      REAL  X1,P1,H1,X2,P2,H2,R,CH,PS,PC,XL,SPX
      DOUBLE PRECISION A, B, S, TN, T, COSL, SINL, COSP, SINP, COSL1
      INTEGER          NEW
      COMMON /TRCOM3/ A(5,5), B(5,5), S(15),
     +                TN(3), T(5), COSL, SINL, COSP, SINP, COSL1, NEW
      COMMON/GCUNIT/LIN,LOUT,NUNITS,LUNITS(5)
      INTEGER LIN,LOUT,NUNITS,LUNITS
      COMMON/GCMAIL/CHMAIL
      CHARACTER*132 CHMAIL
C
      DIMENSION X1(3),P1(3),H1(9),X2(3),P2(3),H2(9)
      DIMENSION R(15),PS(3),PC(3)
*
      DIMENSION T1(3),T2(3),U1(3),U2(3),V1(3),V2(3),HN(9)
      DIMENSION AN1(3),AN2(3),DX(3)
      DIMENSION HV1(3),HU1(3)
*
      SAVE INIT,DELHP6,CFACT8
*
      DATA INIT/0/
      DATA DELHP6/300.D0/
*
      DATA CFACT8 / 2.997925 D-4 /
*
*____________________________________________________________________
*
      IERR=0
      IF(IFLAG) 10, 20, 80
*
* *** TRANSFORM ERROR MATRIX FROM EXTERNAL TO INTERNAL VARIABLES;
*
   10 NEW=1
      IF(MVAR.NE.1) GO TO 11
      PA1=SQRT(P1(1)**2+P1(2)**2+P1(3)**2)
      IF(PA1.EQ.0.) GO TO 902
      PS(1)=1./PA1
      IF(P1(1).EQ.0.) GO TO 904
      PS(2)=P1(2)/P1(1)
      PS(3)=P1(3)/P1(1)
      SPX=1.
      IF(P1(1).LT.0.) SPX=-1.
      CALL TRSPSC(PS,R,PC,R,H1,CH,IERR,SPX)
      GO TO 19
*
   11 IF(MVAR.NE.0) GO TO 901
   19 GO TO 900
*
* *** ERROR PROPAGATION ON A HELIX ASSUMING SC VARIABLES

*
   20 PA1=SQRT(P1(1)**2+P1(2)**2+P1(3)**2)
      PA2=SQRT(P2(1)**2+P2(2)**2+P2(3)**2)
      IF(PA1*PA2.EQ.0.) GO TO 902
C      DPA = PA2 - PA1
      PM1=1./PA1
      PM2=1./PA2
      DPM = PM2 - PM1
*
      DO 201 I=1,3
        T1(I) = P1(I)*PM1
        T2(I) = P2(I)*PM2
201   CONTINUE
*
      SINL=T2(3)
      SINL0=T1(3)
*
      COSL=SQRT(ABS(1.-SINL**2))
      IF(COSL.EQ.0.) GO TO 904
      COSL1=1./COSL
      COSL0=SQRT(ABS(1.-SINL0**2))
*
* *** DEFINE TRANSFORMATION MATRIX BETWEEN X1 AND X2 FOR
* *** NEUTRAL PARTICLE OR FIELDFREE REGION
*
      DO 26 I=1,5
         DO 15 K=1,5
            A(I,K)=0.
   15    CONTINUE
         A(I,I)=1.
   26 CONTINUE
      A(4,3)=XL*COSL
      A(5,2)=XL
*
      IF(CH.EQ.0.) GO TO 45
      HA1=SQRT(H1(1)**2+H1(2)**2+H1(3)**2)
      HA2=SQRT(H2(1)**2+H2(2)**2+H2(3)**2)
      HAM1=HA1*PM1
      HAM2=HA2*PM2
      HAMX=MAX(HAM1,HAM2)
      IF(HAMX.EQ.0.) GO TO 45
*
*
*
* *** CHECK WHETHER H*ALFA/P IS TOO DIFFERENT AT X1 AND X2
*
*
      IF(HA2.NE.0.) THEN
         GAM=(H2(1)*T2(1)+H2(2)*T2(2)+H2(3)*T2(3))/HA2
      ELSE
         GAM=(H1(1)*T1(1)+H1(2)*T1(2)+H1(3)*T1(3))/HA1
      ENDIF
*
      ALFA2=1.-GAM**2
*
      DH2=(H1(1)*PM1-H2(1)*PM2)**2+
     1    (H1(2)*PM1-H2(2)*PM2)**2+
     1    (H1(3)*PM1-H2(3)*PM2)**2
      IF(DH2*ALFA2.GT.DELHP6**2) GO TO 903
*
* *** DEFINE AVERAGE MAGNETIC FIELD AND GRADIENT
*
      PM12=(PM1+PM2)*0.5
      P12=1./(2.*PM12)
      HN(1)=(H1(1)*PM1+H2(1)*PM2)*P12*CH*CFACT8
      HN(2)=(H1(2)*PM1+H2(2)*PM2)*P12*CH*CFACT8
      HN(3)=(H1(3)*PM1+H2(3)*PM2)*P12*CH*CFACT8
CC    HN(4)=(H1(4)*PM1+H2(4)*PM2)*P12*CH*CFACT8
CC    HN(5)=(H1(5)*PM1+H2(5)*PM2)*P12*CH*CFACT8
CC    HN(6)=(H1(6)*PM1+H2(6)*PM2)*P12*CH*CFACT8
CC    HN(7)=(H1(7)*PM1+H2(7)*PM2)*P12*CH*CFACT8
CC    HN(8)=(H1(8)*PM1+H2(8)*PM2)*P12*CH*CFACT8
CC    HN(9)=(H1(9)*PM1+H2(9)*PM2)*P12*CH*CFACT8
*
      HM = SQRT(HN(1)**2+HN(2)**2+HN(3)**2)
      OVER = 1./HM
      HN(1) = OVER*HN(1)
      HN(2) = OVER*HN(2)
      HN(3) = OVER*HN(3)
      PAV = .5*(PA1+PA2)
      Q = - HM/PAV
      THETA = Q*XL
      SINT = SIN(THETA)
      COST = COS(THETA)
      GAMMA=HN(1)*T2(1)+HN(2)*T2(2)+HN(3)*T2(3)
      AN2(1) = HN(2)*T2(3)-HN(3)*T2(2)
      AN2(2) = HN(3)*T2(1)-HN(1)*T2(3)
      AN2(3) = HN(1)*T2(2)-HN(2)*T2(1)
*
      AU = 1./SQRT(T1(1)**2+T1(2)**2)
      U1(1) = -AU*T1(2)
      U1(2) =  AU*T1(1)
      U1(3) =  0.D0
      V1(1) = -T1(3)*U1(2)
      V1(2) =  T1(3)*U1(1)
      V1(3) =  T1(1)*U1(2)-T1(2)*U1(1)
*
      AU = 1./SQRT(T2(1)**2+T2(2)**2)
      U2(1) = -AU*T2(2)
      U2(2) =  AU*T2(1)
      U2(3) =  0.D0
      V2(1) = -T2(3)*U2(2)
      V2(2) =  T2(3)*U2(1)
      V2(3) =  T2(1)*U2(2)-T2(2)*U2(1)
*
      DX(1) = X1(1) - X2(1)
      DX(2) = X1(2) - X2(2)
      DX(3) = X1(3) - X2(3)
*
*
* *** COMPLETE TRANSFORMATION MATRIX BETWEEN ERRORS AT X1 AND X2
* *** FIELD GRADIENT PERPENDICULAR TO TRACK IS PRESENTLY NOT
* *** TAKEN INTO ACCOUNT
*
   30 CONTINUE
      QP  = Q*PAV
      ANV = -(HN(1)*U2(1)+HN(2)*U2(2)            )
      ANU =  (HN(1)*V2(1)+HN(2)*V2(2)+HN(3)*V2(3))
      OMCOST = 1.-COST
      TMSINT = THETA-SINT
*
      HU1(1) =            -HN(3)*U1(2)
      HU1(2) = HN(3)*U1(1)
      HU1(3) = HN(1)*U1(2)-HN(2)*U1(1)
*
      HV1(1) = HN(2)*V1(3)-HN(3)*V1(2)
      HV1(2) = HN(3)*V1(1)-HN(1)*V1(3)
      HV1(3) = HN(1)*V1(2)-HN(2)*V1(1)
*
***   1/P
*
      A(1,1) = 1.-DPM*PAV*(1.+(T2(1)*DX(1)+T2(2)*DX(2)+T2(3)*DX(3))/XL)
     +           +2.*DPM*PAV
*
      A(1,2) =  -DPM/THETA*
     1           ( TMSINT*GAMMA*(HN(1)*V1(1)+HN(2)*V1(2)+HN(3)*V1(3)) +
     2             SINT*(V1(1)*T2(1)+V1(2)*T2(2)+V1(3)*T2(3)) +
     3             OMCOST*(HV1(1)*T2(1)+HV1(2)*T2(2)+HV1(3)*T2(3)) )
*
      A(1,3) =  -COSL0*DPM/THETA*
     1           ( TMSINT*GAMMA*(HN(1)*U1(1)+HN(2)*U1(2)            ) +
     2             SINT*(U1(1)*T2(1)+U1(2)*T2(2)            ) +
     3             OMCOST*(HU1(1)*T2(1)+HU1(2)*T2(2)+HU1(3)*T2(3)) )
*
      A(1,4) =  -DPM/XL*(U1(1)*T2(1)+U1(2)*T2(2)            )
*
      A(1,5) =  -DPM/XL*(V1(1)*T2(1)+V1(2)*T2(2)+V1(3)*T2(3))
*
***   Lambda
*
      A(2,1) = -QP*ANV*(T2(1)*DX(1)+T2(2)*DX(2)+T2(3)*DX(3))
     +         *(1.+DPM*PAV)
*
      A(2,2) = COST*(V1(1)*V2(1)+V1(2)*V2(2)+V1(3)*V2(3)) +
     +         SINT*(HV1(1)*V2(1)+HV1(2)*V2(2)+HV1(3)*V2(3)) +
     1         OMCOST*(HN(1)*V1(1)+HN(2)*V1(2)+HN(3)*V1(3))*
     A                (HN(1)*V2(1)+HN(2)*V2(2)+HN(3)*V2(3)) +
     2         ANV*( -SINT*(V1(1)*T2(1)+V1(2)*T2(2)+V1(3)*T2(3)) +
     3         OMCOST*(V1(1)*AN2(1)+V1(2)*AN2(2)+V1(3)*AN2(3)) -
     4         TMSINT*GAMMA*(HN(1)*V1(1)+HN(2)*V1(2)+HN(3)*V1(3)) )
*
      A(2,3) = COST*(U1(1)*V2(1)+U1(2)*V2(2)            ) +
     +         SINT*(HU1(1)*V2(1)+HU1(2)*V2(2)+HU1(3)*V2(3)) +
     1         OMCOST*(HN(1)*U1(1)+HN(2)*U1(2)            )*
     A                (HN(1)*V2(1)+HN(2)*V2(2)+HN(3)*V2(3)) +
     2         ANV*( -SINT*(U1(1)*T2(1)+U1(2)*T2(2)            ) +
     3         OMCOST*(U1(1)*AN2(1)+U1(2)*AN2(2)             ) -
     4         TMSINT*GAMMA*(HN(1)*U1(1)+HN(2)*U1(2)            ) )
      A(2,3) = COSL0*A(2,3)
*
      A(2,4) = -Q*ANV*(U1(1)*T2(1)+U1(2)*T2(2)            )
*
      A(2,5) = -Q*ANV*(V1(1)*T2(1)+V1(2)*T2(2)+V1(3)*T2(3))
*
***   Phi
*
      A(3,1) = -QP*ANU*(T2(1)*DX(1)+T2(2)*DX(2)+T2(3)*DX(3))*COSL1
     +         *(1.+DPM*PAV)
*
      A(3,2) = COST*(V1(1)*U2(1)+V1(2)*U2(2)            ) +
     +         SINT*(HV1(1)*U2(1)+HV1(2)*U2(2)             ) +
     1         OMCOST*(HN(1)*V1(1)+HN(2)*V1(2)+HN(3)*V1(3))*
     A                (HN(1)*U2(1)+HN(2)*U2(2)            ) +
     2         ANU*( -SINT*(V1(1)*T2(1)+V1(2)*T2(2)+V1(3)*T2(3)) +
     3         OMCOST*(V1(1)*AN2(1)+V1(2)*AN2(2)+V1(3)*AN2(3)) -
     4         TMSINT*GAMMA*(HN(1)*V1(1)+HN(2)*V1(2)+HN(3)*V1(3)) )
      A(3,2) = COSL1*A(3,2)
*
      A(3,3) = COST*(U1(1)*U2(1)+U1(2)*U2(2)            ) +
     +         SINT*(HU1(1)*U2(1)+HU1(2)*U2(2)             ) +
     1         OMCOST*(HN(1)*U1(1)+HN(2)*U1(2)            )*
     A                (HN(1)*U2(1)+HN(2)*U2(2)            ) +
     2         ANU*( -SINT*(U1(1)*T2(1)+U1(2)*T2(2)            ) +
     3         OMCOST*(U1(1)*AN2(1)+U1(2)*AN2(2)             ) -
     4         TMSINT*GAMMA*(HN(1)*U1(1)+HN(2)*U1(2)            ) )
      A(3,3) = COSL1*COSL0*A(3,3)
*
      A(3,4) = -Q*ANU*(U1(1)*T2(1)+U1(2)*T2(2)            )*COSL1
*
      A(3,5) = -Q*ANU*(V1(1)*T2(1)+V1(2)*T2(2)+V1(3)*T2(3))*COSL1
*
***   Yt
*
      A(4,1) = PAV*(U2(1)*DX(1)+U2(2)*DX(2)            )
     +         *(1.+DPM*PAV)
*
      A(4,2) = (   SINT*(V1(1)*U2(1)+V1(2)*U2(2)            ) +
     1           OMCOST*(HV1(1)*U2(1)+HV1(2)*U2(2)             ) +
     2           TMSINT*(HN(1)*U2(1)+HN(2)*U2(2)            )*
     3                  (HN(1)*V1(1)+HN(2)*V1(2)+HN(3)*V1(3)) )/Q
*
      A(4,3) = (   SINT*(U1(1)*U2(1)+U1(2)*U2(2)            ) +
     1           OMCOST*(HU1(1)*U2(1)+HU1(2)*U2(2)             ) +
     2           TMSINT*(HN(1)*U2(1)+HN(2)*U2(2)            )*
     3                  (HN(1)*U1(1)+HN(2)*U1(2)            ) )*COSL0/Q
*
      A(4,4) = (U1(1)*U2(1)+U1(2)*U2(2)            )
*
      A(4,5) = (V1(1)*U2(1)+V1(2)*U2(2)            )
*
***   Zt
*
      A(5,1) = PAV*(V2(1)*DX(1)+V2(2)*DX(2)+V2(3)*DX(3))
     +         *(1.+DPM*PAV)
*
      A(5,2) = (   SINT*(V1(1)*V2(1)+V1(2)*V2(2)+V1(3)*V2(3)) +
     1           OMCOST*(HV1(1)*V2(1)+HV1(2)*V2(2)+HV1(3)*V2(3)) +
     2           TMSINT*(HN(1)*V2(1)+HN(2)*V2(2)+HN(3)*V2(3))*
     3                  (HN(1)*V1(1)+HN(2)*V1(2)+HN(3)*V1(3)) )/Q
*
      A(5,3) = (   SINT*(U1(1)*V2(1)+U1(2)*V2(2)            ) +
     1           OMCOST*(HU1(1)*V2(1)+HU1(2)*V2(2)+HU1(3)*V2(3)) +
     2           TMSINT*(HN(1)*V2(1)+HN(2)*V2(2)+HN(3)*V2(3))*
     3                  (HN(1)*U1(1)+HN(2)*U1(2)            ) )*COSL0/Q
*
      A(5,4) = (U1(1)*V2(1)+U1(2)*V2(2)            )
*
      A(5,5) = (V1(1)*V2(1)+V1(2)*V2(2)+V1(3)*V2(3))
   45 CONTINUE
*
* *** NEW = 0  TRANSFORMATION MATRIX IS UPDATED
*           1  TRANSFORMATION MATRIX IS INITIALIZED
*
      IF(NEW.EQ.0) GO TO 23
      NEW=0
      DO 25 I=1,5
         DO 24 K=1,5
            B(I,K)=A(I,K)
   24    CONTINUE
   25 CONTINUE
      GO TO 27
   23 CONTINUE
*
      CALL XMM55(A,B,B)
*
   27 CONTINUE
   80 IF(ITRAN.EQ.0) GO TO 90
*
*
      J=0
      DO 22 I=1,5
         DO 21 K=I,5
            J=J+1
            S(J)=R(J)
   21    CONTINUE
   22 CONTINUE
*
*
* *** TRANSFORM ERROR MATRIX
*
      CALL SSMT5T(B,S,S)
*
      NEW=1

      J=0
      DO 41 I=1,5
         DO 40 K=I,5
            J=J+1
            R(J)=S(J)
   40    CONTINUE
   41 CONTINUE
*
   90 IF(IFLAG.LE.0) GO TO 900
*
*
* *** TRANSFORM ERROR MATRIX FROM INTERNAL TO EXTERNAL VARIABLES;
*
*
      NEW=1
      IF(MVAR.NE.1) GO TO 91
      PC(1)=PM2
      PC(2)=ASIN(P2(3)*PC(1))
      IF (ABS (P2(1)) .LT. 1.E-30) P2(1) = 1.E-30
      PC(3)=ATAN2(P2(2),P2(1))
      CALL TRSCSP(PC,R,PS,R,H2,CH,IERR,SPX)
      GO TO 900
*
   91 IF(MVAR.NE.0) GO TO 901
      GO TO 900
*
* *** ERROR EXITS
*
  901 IERR=1
      GO TO 999
  902 IERR=2
      GO TO 999
  903 IERR=3
C     IF(INIT.NE.0) GO TO 30
*     WRITE (LOUT, 998) DH2,ALFA2,XL
  998 FORMAT('0',' *** S/R TRPROP   DELTA(H*ALFA/P)',5X
     1,'EXCEEDS TOLERANCE    '/'0',3E12.5//' **********    ',///)
      INIT=1
      GO TO 30
  904 IERR=4
  999 WRITE (LOUT, 1000) IERR
 1000 FORMAT(1H ,' *** S/R ERPROP   IERR =',I5)
*
  900 CONTINUE
      END

CDECK  ID>, TRPROP. 
*CMZ :  3.21/02 29/03/94  15.41.49  by  S.Giani
*-- Author :
C
      SUBROUTINE TRPROP(X1,P1,H1,X2,P2,H2,CH,XL,R,MVAR,IFLAG,ITRAN,IERR)
C
C *** ERROR PROPAGATION ALONG A PARTICLE TRAJECTORY IN A MAGNETIC FIELD
C     ROUTINE ASSUMES THAT IN THE INTERVAL (X1,X2) THE QUANTITIES 1/P
C     AND (HX,HY,HZ) ARE RATHER CONSTANT. DELTA(PHI) MUST NOT BE TOO LARGE
C
C     Authors: A. Haas and W. Wittek
C
C *** IFLAG  =  -1   INITIALIZATION, TRANSFORMATION OF ERROR MATRIX FROM
C                    EXTERNAL TO SC VARIABLES
C            =   0   ERROR PROPAGATION FROM X1 TO X2
C            =   1   TRANSFORMATION OF ERROR MATRIX FROM SC TO
C                    EXTERNAL VARIABLES
C
C     ITRAN          USED FOR IFLAG = 0 OR 1 ONLY
C            =   0   TRANSFORMATION MATRIX IS UPDATED ,BUT ERROR MATRIX IS NOT
C                    TRANSFORMED
C           =    1   TRANSF. MATRIX IS UPDATED  AND ERROR MATRIX IS TRANSFORMED
C
C     MVAR           SPECIFIES TYPE OF EXTERNAL VARIABLES
C            =   0   ( 1/P,LAMBDA,PHI,YT, ZT ;   SC   )
C            =   1   ( 1/P,  Y',  Z',  Y,  Z ; SPLINE )
C
C *** X1, P1, H1     X,Y,Z COMPONENTS OF POSITION, MOMENTUM AND MAGNETIC   INPUT
C                    FIELD VECTOR/GRADIENT AT STARTING POINT OF INTERVAL
C     X2, P2, H2     ......  AT END POINT OF INTERVAL                      INPUT
C     CH             CHARGE OF PARTICLE                                    INPUT
C     XL             PATHLENGTH FROM X1 TO X2   ( NEGATIVE IF OPPOSITE
C                    TO ACTUAL MOVEMENT OF PARTICLE )                      INPUT
C     R              ERROR MATRIX  (TRIANGLE)                       INPUT/OUTPUT
C     B              5 * 5 TRANSFORMATION MATRIX FOR ERRORS IN
C                    SC VARIABLES                                         OUTPUT
C
C *** IERR   =  1    ILLEGAL VALUE OF MVAR                                OUTPUT
C               2    MOMENTUM IS ZERO
C               3    H*ALFA/P AT X1 AND X2 DIFFER TOO MUCH
C                    OR DELTA PHI IS TOO LARGE
C               4    PARTICLE MOVES IN Z - DIRECTION
C
      IMPLICIT DOUBLE PRECISION (A-H,O-Z)
      REAL  X1,P1,H1,X2,P2,H2,R,CH,PS,PC,XL,SPX
      DOUBLE PRECISION A, B, S, TN, T, COSL, SINL, COSP, SINP, COSL1
      INTEGER          NEW
      COMMON /TRCOM3/ A(5,5), B(5,5), S(15),
     +                TN(3), T(5), COSL, SINL, COSP, SINP, COSL1, NEW
      COMMON/GCUNIT/LIN,LOUT,NUNITS,LUNITS(5)
      INTEGER LIN,LOUT,NUNITS,LUNITS
      COMMON/GCMAIL/CHMAIL
      CHARACTER*132 CHMAIL
C
      DIMENSION X1(3),P1(3),H1(9),X2(3),P2(3),H2(9)
      DIMENSION R(15),PS(3),PC(3),HN(9)
C
      SAVE INIT,DELHP6,CFACT8
*
      DATA INIT/0/
      DATA DELHP6/300.D0/,DELFI6/0.1D0/
      DATA CFACT8 / 2.997925 D-4 /
C
      IERR=0
      IF(IFLAG) 10, 20, 80
C
C *** TRANSFORM ERROR MATRIX FROM EXTERNAL TO INTERNAL VARIABLES;
C
   10 NEW=1
      IF(MVAR.NE.1) GO TO 11
      PA1=SQRT(P1(1)**2+P1(2)**2+P1(3)**2)
      IF(PA1.EQ.0.) GO TO 902
      PS(1)=1./PA1
      IF(P1(1).EQ.0.) GO TO 904
      PS(2)=P1(2)/P1(1)
      PS(3)=P1(3)/P1(1)
      SPX=1.
      IF(P1(1).LT.0.) SPX=-1.
      CALL TRSPSC(PS,R,PC,R,H1,CH,IERR,SPX)
      GO TO 19
C
   11 IF(MVAR.NE.0) GO TO 901
   19 GO TO 900
C
C *** ERROR PROPAGATION ON A HELIX ASSUMING SC VARIABLES

C
   20 PA1=SQRT(P1(1)**2+P1(2)**2+P1(3)**2)
      PA2=SQRT(P2(1)**2+P2(2)**2+P2(3)**2)
      IF(PA1*PA2.EQ.0.) GO TO 902
      PM1=1./PA1
      PM2=1./PA2
C
      TN(1)=P1(1)+P2(1)
      TN(2)=P1(2)+P2(2)
      TN(3)=P1(3)+P2(3)
      PM12=1./SQRT(TN(1)**2+TN(2)**2+TN(3)**2)
      TN(1)=TN(1)*PM12
      TN(2)=TN(2)*PM12
      TN(3)=TN(3)*PM12
C
      SINL=TN(3)
      COSL=SQRT(ABS(1.-SINL**2))
      IF(COSL.EQ.0.) GO TO 904
      COSL1=1./COSL
      SINP=TN(2)*COSL1
      COSP=TN(1)*COSL1
C
C *** DEFINE TRANSFORMATION MATRIX BETWEEN X1 AND X2 FOR
C *** NEUTRAL PARTICLE OR FIELDFREE REGION
C
      DO 26 I=1,5
         DO 15 K=1,5
            A(I,K)=0.
   15    CONTINUE
         A(I,I)=1.
   26 CONTINUE
      A(4,3)=XL*COSL
      A(5,2)=XL
C
      IF(CH.EQ.0.) GO TO 45
      HA1=SQRT(H1(1)**2+H1(2)**2+H1(3)**2)
      HA2=SQRT(H2(1)**2+H2(2)**2+H2(3)**2)
      HAM1=HA1*PM1
      HAM2=HA2*PM2
      HAMX = MAX(HAM1,HAM2)
      IF(HAMX.EQ.0.) GO TO 45
C
C *** DEFINE AVERAGE MAGNETIC FIELD AND GRADIENT
C
      PM12=(PM1+PM2)*0.5
      P12=1./(2.*PM12)
      HN(1)=(H1(1)*PM1+H2(1)*PM2)*P12*CH*CFACT8
      HN(2)=(H1(2)*PM1+H2(2)*PM2)*P12*CH*CFACT8
      HN(3)=(H1(3)*PM1+H2(3)*PM2)*P12*CH*CFACT8
      HN(4)=(H1(4)*PM1+H2(4)*PM2)*P12*CH*CFACT8
      HN(5)=(H1(5)*PM1+H2(5)*PM2)*P12*CH*CFACT8
      HN(6)=(H1(6)*PM1+H2(6)*PM2)*P12*CH*CFACT8
      HN(7)=(H1(7)*PM1+H2(7)*PM2)*P12*CH*CFACT8
      HN(8)=(H1(8)*PM1+H2(8)*PM2)*P12*CH*CFACT8
      HN(9)=(H1(9)*PM1+H2(9)*PM2)*P12*CH*CFACT8
C
      B0=HN(1)*COSP+HN(2)*SINP
      B2=-HN(1)*SINP+HN(2)*COSP
      B3=-B0*SINL+HN(3)*COSL
      TGL=SINL*COSL1
C
C
C *** CHECK WHETHER H*ALFA/P IS TOO DIFFERENT AT X1 AND X2
C     AND WHETHER CHANGE OF TRACK DIRECTION DUE TO MAG.FIELD IS TOO LARGE
C
C
      IF(HA2.EQ.0.) GO TO 29

      GAM=(H2(1)*TN(1)+H2(2)*TN(2)+H2(3)*TN(3))/HA2
      GO TO 28
   29 GAM=(H1(1)*TN(1)+H1(2)*TN(2)+H1(3)*TN(3))/HA1
   28 CONTINUE
      ALFA=SQRT(ABS(1.-GAM**2))
C
      DH2=(H1(1)*PM1-H2(1)*PM2)**2+
     1    (H1(2)*PM1-H2(2)*PM2)**2+
     1    (H1(3)*PM1-H2(3)*PM2)**2
      IF(DH2*ALFA**2.GT.DELHP6**2) GO TO 903
      ALFAQ=-ALFA*CFACT8*(HAM1+HAM2)*0.5
      DFI=ABS(XL*ALFAQ)
      IF(DFI.GT.DELFI6) GO TO 903
C
C *** COMPLETE TRANSFORMATION MATRIX BETWEEN ERRORS AT X1 AND X2
C *** TAKING INTO ACCOUNT  FIELD GRADIENT PERPENDICULAR TO TRACK
C
   30 COSP2=COSP*COSP
      SINP2=SINP*SINP
      COSIP=COSP*SINP
C
      G22=SINP2*HN(9)+COSP2*HN(8)-2.0*COSIP*HN(7)
      G33=SINL*SINL*(COSP2*HN(9)+SINP2*HN(8)+2.0*COSIP*HN(7))
     ++COSL*(COSL*HN(6)-2.0*SINL*(COSP*HN(4)+SINP*HN(5)))
      G23=SINL*(COSIP*(HN(9)-HN(8))+(SINP2-COSP2)*HN(7))
     ++COSL*(COSP*HN(5)-SINP*HN(4))
C
      A(2,1)=XL*B2
      A(2,3)=-B0*XL*PM12
      A(2,4)=(B2*B3*PM12+G22)*XL*PM12
      A(2,5)=(-B2*B2*PM12+G23)*XL*PM12
C
      A(3,1)=-XL*B3*COSL1
      A(3,2)=B0*XL*PM12*COSL1**2
      A(3,3)=1.+TGL*B2*XL*PM12
      A(3,4)=(-B3*B3*PM12-G23)*XL*PM12*COSL1
      A(3,5)=(B3*B2*PM12-G33)*XL*PM12*COSL1
C
      A(4,5)=-B3*TGL*XL*PM12
      A(5,4)=B3*TGL*XL*PM12
C
   45 CONTINUE
C
C *** NEW = 0  TRANSFORMATION MATRIX IS UPDATED
C           1  TRANSFORMATION MATRIX IS INITIALIZED
C
      IF(NEW.EQ.0) GO TO 23
      NEW=0
      DO 25 I=1,5
         DO 24 K=1,5
            B(I,K)=A(I,K)
   24    CONTINUE
   25 CONTINUE
      GO TO 27
   23 CONTINUE
C
      CALL XMM55(A,B,B)
C
   27 CONTINUE
   80 IF(ITRAN.EQ.0) GO TO 90
C
C
      J=0
      DO 22 I=1,5
         DO 21 K=I,5
            J=J+1
            S(J)=R(J)
   21    CONTINUE
   22 CONTINUE
C
C
C *** TRANSFORM ERROR MATRIX
C
      CALL SSMT5T(B,S,S)
C
      NEW=1

      J=0
      DO 41 I=1,5
         DO 40 K=I,5
            J=J+1
            R(J)=S(J)
   40    CONTINUE
   41 CONTINUE
C
   90 IF(IFLAG.LE.0) GO TO 900
C
C
C *** TRANSFORM ERROR MATRIX FROM INTERNAL TO EXTERNAL VARIABLES;
C
C
      NEW=1
      IF(MVAR.NE.1) GO TO 91
      PC(1)=PM2
      PC(2)=ASIN(P2(3)*PC(1))
      IF (ABS (P2(1)) .LT. 1.E-30) P2(1) = 1.E-30
      PC(3) = ATAN2 (P2(2),P2(1))
      CALL TRSCSP(PC,R,PS,R,H2,CH,IERR,SPX)
      GO TO 900
C
   91 IF(MVAR.NE.0) GO TO 901
      GO TO 900
C
C *** ERROR EXITS
C
  901 IERR=1
      GO TO 999
  902 IERR=2
      GO TO 999
  903 IERR=3
      IF(INIT.NE.0) GO TO 30
C     WRITE (LOUT, 998) DH2,DFI,ALFA,XL
C 998 FORMAT(1H0,48H *** S/R TRPROP   DELTA(H*ALFA/P)  OR DELTA(PHI),5X
C    1,22HEXCEEDS TOLERANCE     /1H0,4E12.5//16H **********    ,
C    251HATTENTION !   NO FURTHER WARNINGS WILL BE GIVEN    ///)
      INIT=1
      GO TO 30
  904 IERR=4
  999 WRITE (LOUT, 1000) IERR
 1000 FORMAT(1H ,' *** S/R ERPROP   IERR =',I5)
C
  900 RETURN
      END
CDECK  ID>, TRSCSD. 
*CMZ :  3.21/02 29/03/94  15.41.49  by  S.Giani
*-- Author :
C
      SUBROUTINE TRSCSD(PC,RC,PD,RD,H,CH,IERR,SPU,DJ,DK)
C
C *** TRANSFORMS ERROR MATRIX
C     FROM   SC   VARIABLES (1/P,LAMBDA,PHI,YT,ZT)
C      TO         VARIABLES (1/P,V',W',V,W)
C
C     Authors: A. Haas and W. Wittek
C
C
C *** PC(3)     1/P,LAMBDA,PHI                          INPUT
C     PD(3)     1/P,V',W'                              OUTPUT
C     H(3)      MAGNETIC FIELD                          INPUT
C     RC(15)    ERROR MATRIX IN   SC   VARIABLES        INPUT     (TRIANGLE)
C     RD(15)    ERROR MATRIX IN 1/P,V',W',V,W          OUTPUT     (TRIANGLE)
C     CH        CHARGE OF PARTICLE                      INPUT
C               CHARGE AND MAGNETIC FIELD ARE NEEDED
C               FOR CORRELATION TERMS (V',YT),(V',ZT),(W',YT),(W',ZT)
C               THESE CORRELATION TERMS APPEAR BECAUSE RC IS ASSUMED
C               TO BE THE ERROR MATRIX FOR FIXED S (PATH LENGTH)
C               AND RD FOR FIXED U
C     DJ(3)     UNIT VECTOR IN V-DIRECTION
C     DK(3)     UNIT VECTOR IN W-DIRECTION    OF DETECTOR SYSTEM
C
C     IERR  =   1       PARTICLE MOVES PERPENDICULAR TO U-AXIS
C                      ( V',W' ARE NOT DEFINED )
C     SPU       SIGN OF U-COMPONENT OF PARTICLE MOMENTUM   OUTPUT
C
      IMPLICIT DOUBLE PRECISION (A-H,O-Z)
      REAL  PC,PD,H,RC,RD,CH,DJ,DK,SPU
      DOUBLE PRECISION A, B, S, TN, T, COSL, SINL, COSP, SINP, COSL1
      INTEGER          NEW
      COMMON /TRCOM3/ A(5,5), B(5,5), S(15),
     +                TN(3), T(5), COSL, SINL, COSP, SINP, COSL1, NEW
      DIMENSION PC(3),PD(3),H(3),RC(15),RD(15),DJ(3),DK(3)
      DIMENSION UN(3),VN(3),DI(3),TVW(3)
C
      DATA CFACT8 / 2.997925 D-4 /
C
      IERR=0
      PM=PC(1)
      COSL=COS(PC(2))
      SINP=SIN(PC(3))
      COSP=COS(PC(3))
C
      TN(1)=COSL*COSP
      TN(2)=COSL*SINP
      TN(3)=SIN(PC(2))
C
      DI(1)=DJ(2)*DK(3)-DJ(3)*DK(2)
      DI(2)=DJ(3)*DK(1)-DJ(1)*DK(3)
      DI(3)=DJ(1)*DK(2)-DJ(2)*DK(1)
C
      TVW(1)=TN(1)*DI(1)+TN(2)*DI(2)+TN(3)*DI(3)
      SPU=1.
      IF(TVW(1).LT.0.) SPU=-1.
      TVW(2)=TN(1)*DJ(1)+TN(2)*DJ(2)+TN(3)*DJ(3)
      TVW(3)=TN(1)*DK(1)+TN(2)*DK(2)+TN(3)*DK(3)
      IF(TVW(1).EQ.0.) GO TO 901
C
      T1R=1./TVW(1)
      PD(1)=PC(1)
      PD(2)=TVW(2)*T1R
      PD(3)=TVW(3)*T1R
C
      UN(1)=-SINP
      UN(2)=COSP
      UN(3)=0.
C
      VN(1)=-TN(3)*UN(2)
      VN(2)=TN(3)*UN(1)
      VN(3)=COSL
C
      UJ=UN(1)*DJ(1)+UN(2)*DJ(2)+UN(3)*DJ(3)
      UK=UN(1)*DK(1)+UN(2)*DK(2)+UN(3)*DK(3)
      VJ=VN(1)*DJ(1)+VN(2)*DJ(2)+VN(3)*DJ(3)
      VK=VN(1)*DK(1)+VN(2)*DK(2)+VN(3)*DK(3)
C
C
      J=0

      DO 10 I=1,5
         DO 5 K=I,5
            J=J+1
            A(I,K)=0.
            A(K,I)=0.
            S(J)=RC(J)
    5    CONTINUE
   10 CONTINUE
C
      IF(CH.EQ.0.) GO TO 6
      HA=SQRT(H(1)**2+H(2)**2+H(3)**2)
      HAM=HA*PM
      IF(HAM.EQ.0.) GO TO 6
      HM=CH/HA
      Q=-HAM*CFACT8
C
C
      SINZ=-(H(1)*UN(1)+H(2)*UN(2)+H(3)*UN(3))*HM
      COSZ= (H(1)*VN(1)+H(2)*VN(2)+H(3)*VN(3))*HM
      T3R=Q*T1R**3
      UI=UN(1)*DI(1)+UN(2)*DI(2)+UN(3)*DI(3)
      VI=VN(1)*DI(1)+VN(2)*DI(2)+VN(3)*DI(3)
      A(2,4)=-UI*(VK*COSZ-UK*SINZ)*T3R
      A(2,5)=-VI*(VK*COSZ-UK*SINZ)*T3R
      A(3,4)= UI*(VJ*COSZ-UJ*SINZ)*T3R
      A(3,5)= VI*(VJ*COSZ-UJ*SINZ)*T3R
C
    6 T2R=T1R**2
C
      A(1,1)=1.
      A(2,2)=-UK*T2R
      A(2,3)=VK*COSL*T2R
      A(3,2)=UJ*T2R
      A(3,3)=-VJ*COSL*T2R
      A(4,4)=VK*T1R
      A(4,5)=-UK*T1R
      A(5,4)=-VJ*T1R
      A(5,5)=UJ*T1R
C
      CALL SSMT5T(A,S,S)
C
      J=0
      DO 25 I=1,5
         DO 20 K=I,5
            J=J+1
            RD(J)=S(J)
   20    CONTINUE
   25 CONTINUE
C
      RETURN
C
C *** ERROR EXITS
C
  901 IERR=1
  910 CONTINUE
C
      RETURN
      END
*
CDECK  ID>, TRSCSP. 
*CMZ :  3.21/02 29/03/94  15.41.49  by  S.Giani
*-- Author :
C
      SUBROUTINE TRSCSP(PC,RC,PS,RS,H,CH,IERR,SPX)
C
C *** TRANSFORMS ERROR MATRIX
C     FROM   SC   VARIABLES (1/P,LAMBDA,PHI,YT,ZT)
C      TO  SPLINE VARIABLES (1/P,Y',Z',Y,Z)
C
C     Authors: A. Haas and W. Wittek
C
C
C *** PC(3)     1/P,LAMBDA,PHI                          INPUT
C     PS(3)     1/P,Y',Z'                              OUTPUT
C     H(3)      MAGNETIC FIELD                          INPUT
C     RC(15)    ERROR MATRIX IN   SC   VARIABLES        INPUT     (TRIANGLE)
C     RS(15)    ERROR MATRIX IN SPLINE VARIABLES       OUTPUT     (TRIANGLE)
C     CH        CHARGE OF PARTICLE                      INPUT
C               CHARGE AND MAGNETIC FIELD ARE NEEDED
C               FOR CORRELATION TERMS (Y',YT),(Y',ZT),(Z',YT),(Z',ZT)
C               THESE CORRELATION TERMS APPEAR BECAUSE RC IS ASSUMED
C               TO BE THE ERROR MATRIX FOR FIXED S (PATH LENGTH)
C               AND RS FOR FIXED X
C
C     IERR  =   1       PARTICLE MOVES PERPENDICULAR TO X-AXIS
C                      ( Y',Z' ARE NOT DEFINED )
C     SPX       SIGN OF X-COMPONENT OF PARTICLE MOMENTUM   OUTPUT
C
      IMPLICIT DOUBLE PRECISION (A-H,O-Z)
      REAL   PC,PS,H,RC,RS,CH,SPX
      DOUBLE PRECISION A, B, S, TN, T, COSL, SINL, COSP, SINP, COSL1
      INTEGER          NEW
      COMMON /TRCOM3/ A(5,5), B(5,5), S(15),
     +                TN(3), T(5), COSL, SINL, COSP, SINP, COSL1, NEW
      DIMENSION PC(3),PS(3),H(3),RC(15),RS(15)
      DIMENSION UN(3),VN(3)
C
      DATA CFACT8 / 2.997925 D-4 /
C
      IERR=0
      PM=PC(1)
      COSL=COS(PC(2))
      SINP=SIN(PC(3))
      COSP=COS(PC(3))
C
      TN(1)=COSL*COSP
      SPX=1.
      IF(TN(1).LT.0.) SPX=-1.
      IF(TN(1).EQ.0.) GO TO 901
      TN(2)=COSL*SINP
      TN(3)=SIN(PC(2))
C
      T1R=1./TN(1)
      PS(1)=PC(1)
      PS(2)=SINP/COSP
      PS(3)=TN(3)*T1R
C
      UN(1)=-SINP
      UN(2)=COSP
C
      VN(1)=-TN(3)*UN(2)
      VN(2)=TN(3)*UN(1)
      VN(3)=COSL
C
      J=0
      DO 10 I=1,5
         DO 5 K=I,5
            J=J+1
            A(I,K)=0.
            A(K,I)=0.
            S(J)=RC(J)
    5    CONTINUE
   10 CONTINUE
C
      IF(CH.EQ.0.) GO TO 6
      HA=SQRT(H(1)**2+H(2)**2+H(3)**2)
      HAM=HA*PM
      IF(HAM.EQ.0.) GO TO 6
      HM=CH/HA
      Q=-HAM*CFACT8
C
C
      SINZ=-(H(1)*UN(1)+H(2)*UN(2)           )*HM
      COSZ= (H(1)*VN(1)+H(2)*VN(2)+H(3)*VN(3))*HM

      T3R=Q*T1R**3
      A(2,4)=-UN(1)*(VN(3)*COSZ           )*T3R
      A(2,5)=-VN(1)*(VN(3)*COSZ           )*T3R
      A(3,4)=UN(1)*(VN(2)*COSZ-UN(2)*SINZ)*T3R
      A(3,5)=VN(1)*(VN(2)*COSZ-UN(2)*SINZ)*T3R
C
    6 T2R=T1R**2
C
      A(1,1)=1.
      A(2,3)=VN(3)*COSL*T2R
      A(3,2)=UN(2)*T2R
      A(3,3)=-VN(2)*COSL*T2R
      A(4,4)=VN(3)*T1R
      A(5,4)=-VN(2)*T1R
      A(5,5)=UN(2)*T1R
C
      CALL SSMT5T(A,S,S)
C
      J=0
      DO 25 I=1,5
         DO 20 K=I,5
            J=J+1
            RS(J)=S(J)
   20    CONTINUE
   25 CONTINUE
C
      RETURN
C
C *** ERROR EXITS
C
  901 IERR=1
  910 CONTINUE
C
      RETURN
      END
*
CDECK  ID>, TRSDSC. 
*CMZ :  3.21/02 29/03/94  15.41.49  by  S.Giani
*-- Author :
C
      SUBROUTINE TRSDSC(PD,RD,PC,RC,H,CH,IERR,SPU,DJ,DK)
C
C *** TRANSFORMS ERROR MATRIX
C     FROM        VARIABLES (1/P,V',W',V,W)
C      TO    SC   VARIABLES (1/P,LAMBDA,PHI,YT,ZT)
C
C     Authors: A. Haas and W. Wittek
C
C
C *** PD(3)     1/P,V',W'                               INPUT
C     PC(3)     1/P,LAMBDA,PHI                         OUTPUT
C     H(3)      MAGNETIC FIELD                          INPUT
C     RD(15)    ERROR MATRIX IN 1/P,V',W',V,W           INPUT      (TRIANGLE)
C     RC(15)    ERROR MATRIX IN   SC   VARIABLES       OUTPUT      (TRIANGLE)
C     CH        CHARGE OF PARTICLE                      INPUT
C               CHARGE AND MAGNETIC FIELD ARE NEEDED
C               FOR CORRELATION TERMS (LAMBDA,V),(LAMBDA,W),(PHI,V),(PHI,W)
C               THESE CORRELATION TERMS APPEAR BECAUSE RC IS ASSUMED
C               TO BE THE ERROR MATRIX FOR FIXED S (PATH LENGTH)
C               AND RD FOR FIXED U
C     DJ(3)     UNIT VECTOR IN V-DIRECTION
C     DK(3)     UNIT VECTOR IN W-DIRECTION    OF DETECTOR SYSTEM
C
C     IERR              NOT USED
C     SPU       SIGN OF U-COMPONENT OF PARTICLE MOMENTUM    INPUT
C
      IMPLICIT DOUBLE PRECISION (A-H,O-Z)
      REAL   PD,PC,H,RC,RD,CH,DJ,DK,SPU
      DOUBLE PRECISION A, B, S, TN, T, COSL, SINL, COSP, SINP, COSL1
      INTEGER          NEW
      COMMON /TRCOM3/ A(5,5), B(5,5), S(15),
     +                TN(3), T(5), COSL, SINL, COSP, SINP, COSL1, NEW
      DIMENSION PD(3),PC(3),H(3),RC(15),RD(15),DJ(3),DK(3)
      DIMENSION UN(3),VN(3),DI(3),TVW(3)
C
      DATA CFACT8 / 2.997925 D-4 /
C
      IERR=0
      PM=PD(1)
      TVW(1)=1./SQRT(1.+PD(2)**2+PD(3)**2)
      IF(SPU.LT.0.) TVW(1)=-TVW(1)
      TVW(2)=PD(2)*TVW(1)
      TVW(3)=PD(3)*TVW(1)
C
      DI(1)=DJ(2)*DK(3)-DJ(3)*DK(2)
      DI(2)=DJ(3)*DK(1)-DJ(1)*DK(3)
      DI(3)=DJ(1)*DK(2)-DJ(2)*DK(1)
C
      DO 5 I=1,3
         TN(I)=TVW(1)*DI(I)+TVW(2)*DJ(I)+TVW(3)*DK(I)
    5 CONTINUE
C
      PC(1)=PD(1)
      PC(2)=ASIN(TN(3))
      IF (ABS (TN(1)) .LT. 1.E-30) TN(1) = 1.E-30
      PC(3) = ATAN2 (TN(2),TN(1))
C
      cosl=sqrt(tn(1)**2+tn(2)**2)/sqrt(tn(1)**2+tn(2)**2+tn(3)**2)
c      COSL=SQRT(ABS(1.-TN(3)**2))
      IF (COSL .LT. 1.E-30) COSL = 1.E-30
      COSL1=1./COSL
      UN(1)=-TN(2)*COSL1
      UN(2)=TN(1)*COSL1
      UN(3)=0.
C
      VN(1)=-TN(3)*UN(2)
      VN(2)=TN(3)*UN(1)
      VN(3)=COSL

C
      UJ=UN(1)*DJ(1)+UN(2)*DJ(2)+UN(3)*DJ(3)
      UK=UN(1)*DK(1)+UN(2)*DK(2)+UN(3)*DK(3)
      VJ=VN(1)*DJ(1)+VN(2)*DJ(2)+VN(3)*DJ(3)
      VK=VN(1)*DK(1)+VN(2)*DK(2)+VN(3)*DK(3)
C
      J=0
      DO 10 I=1,5
         DO 4 K=I,5
            J=J+1
            A(I,K)=0.
            A(K,I)=0.
            S(J)=RD(J)
    4    CONTINUE
   10 CONTINUE
C
      IF(CH.EQ.0.) GO TO 6
      HA=SQRT(H(1)**2+H(2)**2+H(3)**2)
      HAM=HA*PM
      IF(HAM.EQ.0.) GO TO 6
      HM=CH/HA
C
      Q=-HAM*CFACT8
C
      SINZ=-(H(1)*UN(1)+H(2)*UN(2)+H(3)*UN(3))*HM
      COSZ= (H(1)*VN(1)+H(2)*VN(2)+H(3)*VN(3))*HM
      A(2,4)=-Q*TVW(2)*SINZ
      A(2,5)=-Q*TVW(3)*SINZ
      A(3,4)=-Q*TVW(2)*COSZ*COSL1
      A(3,5)=-Q*TVW(3)*COSZ*COSL1
C
    6 A(1,1)=1.
      A(2,2)=TVW(1)*VJ
      A(2,3)=TVW(1)*VK
      A(3,2)=TVW(1)*UJ*COSL1
      A(3,3)=TVW(1)*UK*COSL1
      A(4,4)=UJ
      A(4,5)=UK
      A(5,4)=VJ
      A(5,5)=VK
C
      CALL SSMT5T(A,S,S)
C
      J=0
      DO 25 I=1,5
         DO 20 K=I,5
            J=J+1
            RC(J)=S(J)
   20    CONTINUE
   25 CONTINUE
C
      RETURN
C
C *** ERROR EXITS
C
      END
*
CDECK  ID>, TRSPSC. 
*CMZ :  3.21/02 29/03/94  15.41.49  by  S.Giani
*-- Author :
C
      SUBROUTINE TRSPSC(PS,RS,PC,RC,H,CH,IERR,SPX)
C
C *** TRANSFORMS ERROR MATRIX
C     FROM SPLINE VARIABLES (1/P,Y',Z',Y,Z)
C      TO    SC   VARIABLES (1/P,LAMBDA,PHI,YT,ZT)
C
C     Authors: A. Haas and W. Wittek
C
C
C *** PS(3)     1/P,Y',Z'                               INPUT
C     PC(3)     1/P,LAMBDA,PHI                         OUTPUT
C     H(3)      MAGNETIC FIELD                          INPUT
C     RS(15)    ERROR MATRIX IN SPLINE VARIABLES        INPUT      (TRIANGLE)
C     RC(15)    ERROR MATRIX IN   SC   VARIABLES       OUTPUT      (TRIANGLE)
C     CH        CHARGE OF PARTICLE                      INPUT
C               CHARGE AND MAGNETIC FIELD ARE NEEDED
C               FOR CORRELATION TERMS (LAMBDA,Y),(LAMBDA,Z),(PHI,Y),(PHI,Z)
C               THESE CORRELATION TERMS APPEAR BECAUSE RC IS ASSUMED
C               TO BE THE ERROR MATRIX FOR FIXED S (PATH LENGTH)
C               AND RS FOR FIXED X
C
C     IERR              NOT USED
C     SPX       SIGN OF X-COMPONENT OF PARTICLE MOMENTUM    INPUT
C
      IMPLICIT DOUBLE PRECISION (A-H,O-Z)
      REAL   PS,PC,H,RC,RS,CH,SPX
      DOUBLE PRECISION A, B, S, TN, T, COSL, SINL, COSP, SINP, COSL1
      INTEGER          NEW
      COMMON /TRCOM3/ A(5,5), B(5,5), S(15),
     +                TN(3), T(5), COSL, SINL, COSP, SINP, COSL1, NEW
      DIMENSION PS(3),PC(3),H(3),RC(15),RS(15)
      DIMENSION UN(3),VN(3)
C
      DATA CFACT8 / 2.997925 D-4 /
C
      IERR=0
      PM=PS(1)
      TN(1)=1./SQRT(1.+PS(2)**2+PS(3)**2)
      IF(SPX.LT.0.) TN(1)=-TN(1)
      TN(2)=PS(2)*TN(1)
      TN(3)=PS(3)*TN(1)
C
      PC(1)=PS(1)
      PC(2)=ASIN(TN(3))
      IF (ABS (TN(1)) .LT. 1.E-30) TN(1) = 1.E-30
      PC(3) = ATAN2 (TN(2),TN(1))
C
c      COSL=SQRT(ABS(1.-TN(3)**2))
      cosl=sqrt(tn(1)**2+tn(2)**2)/sqrt(tn(1)**2+tn(2)**2+tn(3)**2)
      IF (COSL .LT. 1.E-30) COSL = 1.E-30
      COSL1=1./COSL
      UN(1)=-TN(2)*COSL1
      UN(2)=TN(1)*COSL1
C
      VN(1)=-TN(3)*UN(2)
      VN(2)=TN(3)*UN(1)
      VN(3)=COSL
C
      J=0
      DO 10 I=1,5
         DO 4 K=I,5
            J=J+1
            A(I,K)=0.
            A(K,I)=0.
            S(J)=RS(J)
    4    CONTINUE
   10 CONTINUE
C
      IF(CH.EQ.0.) GO TO 6

      HA=SQRT(H(1)**2+H(2)**2+H(3)**2)
      HAM=HA*PM
      IF(HAM.EQ.0.) GO TO 6
      HM=CH/HA
C
      Q=-HAM*CFACT8
C
      SINZ=-(H(1)*UN(1)+H(2)*UN(2)           )*HM
      COSZ= (H(1)*VN(1)+H(2)*VN(2)+H(3)*VN(3))*HM
      A(2,4)=-Q*TN(2)*SINZ
      A(2,5)=-Q*TN(3)*SINZ
      A(3,4)=-Q*TN(2)*COSZ*COSL1
      A(3,5)=-Q*TN(3)*COSZ*COSL1
C
    6 A(1,1)=1.
      A(2,2)=TN(1)*VN(2)
      A(2,3)=TN(1)*VN(3)
      A(3,2)=TN(1)*UN(2)*COSL1
      A(4,4)=UN(2)
      A(5,4)=VN(2)
      A(5,5)=VN(3)
C
      CALL SSMT5T(A,S,S)
C
      J=0
      DO 25 I=1,5
         DO 20 K=I,5
            J=J+1
            RC(J)=S(J)
   20    CONTINUE
   25 CONTINUE
*
* *** ERROR EXITS
*
      END
CDECK  ID>, TRS1S2. 
*CMZ :  3.21/02 29/03/94  15.41.49  by  S.Giani
*-- Author :
      SUBROUTINE TRS1S2 (PD1,RD1,PD2,RD2,H,CH,IERR,SP1,SP2
     1,                  DJ1,DK1,DJ2,DK2)
C
C *** TRANSFORMS ERROR MATRIX
C     FROM        VARIABLES (1/P,V1',W1',V1,W1)
C      TO         VARIABLES (1/P,V2',W2',V2,W2)
C
C     Authors: A. Haas and W. Wittek
C
C
C *** PD1(3)    1/P,V1',W1'                             INPUT
C     PD2(3)    1/P,V2',W2'                            OUTPUT
C     H(3)      MAGNETIC FIELD                          INPUT
C     RD1(15)   ERROR MATRIX IN 1/P,V1',W1',V1,W1       INPUT      (TRIANGLE)
C     RD2(15)   ERROR MATRIX IN 1/P,V2',W2',V2,W2      OUTPUT      (TRIANGLE)
C     CH        CHARGE OF PARTICLE                      INPUT
C               CHARGE AND MAGNETIC FIELD ARE NEEDED
C               FOR CORRELATION TERMS (V2',V1),(V2',W1),(W2',V1),(W2',W1)
C               THESE CORRELATION TERMS APPEAR BECAUSE RD1 IS ASSUMED
C               TO BE THE ERROR MATRIX FOR FIXED U1
C               AND RD2 FOR FIXED U2
C     SP1       SIGN OF U1-COMPONENT OF PARTICLE MOMENTUM     INPUT
C     SP2       SIGN OF U2-COMPONENT OF PARTICLE MOMENTUM    OUTPUT
C     DJ1(3)    UNIT VECTOR IN V1-DIRECTION
C     DK1(3)    UNIT VECTOR IN W1-DIRECTION    OF SYSTEM 1
C     DJ2(3)    UNIT VECTOR IN V2-DIRECTION
C     DK2(3)    UNIT VECTOR IN W2-DIRECTION    OF SYSTEM 2
C
C     IERR      = 0    TRANSFORMATION OK
C               = 1    MOMENTUM PERPENDICULAR TO U2-DIRECTION (V2',W2' NOT DEFIN
C               = 2    MOMENTUM PERPENDICULAR TO X-AXIS
C
C
      IMPLICIT DOUBLE PRECISION (A-H,O-Z)
      REAL   PD1,PD2,RD1,RD2,H,CH,SP1,SP2,DJ1,DK1,DJ2,DK2
      DOUBLE PRECISION A, B, S, TN, T, COSL, SINL, COSP, SINP, COSL1
      INTEGER          NEW
      COMMON /TRCOM3/ A(5,5), B(5,5), S(15),
     +                TN(3), T(5), COSL, SINL, COSP, SINP, COSL1, NEW
      DIMENSION PD1(3),PD2(3),RD1(15),RD2(15),H(3),DJ1(3),DK1(3)
     +,DJ2(3),DK2(3),UN(3),VN(3),DI1(3),DI2(3),TVW1(3),TVW2(3)
C
      DATA CFACT8 / 2.997925 D-4 /
C
      IERR=0
      PM=PD1(1)
      TVW1(1)=1./SQRT(1.+PD1(2)**2+PD1(3)**2)
      IF(SP1.LT.0.) TVW1(1)=-TVW1(1)
      TVW1(2)=PD1(2)*TVW1(1)
      TVW1(3)=PD1(3)*TVW1(1)
C
      DI1(1)=DJ1(2)*DK1(3)-DJ1(3)*DK1(2)
      DI1(2)=DJ1(3)*DK1(1)-DJ1(1)*DK1(3)
      DI1(3)=DJ1(1)*DK1(2)-DJ1(2)*DK1(1)
C
      DO 5 I=1,3
         TN(I)=TVW1(1)*DI1(I)+TVW1(2)*DJ1(I)+TVW1(3)*DK1(I)
    5 CONTINUE
C
      DI2(1)=DJ2(2)*DK2(3)-DJ2(3)*DK2(2)
      DI2(2)=DJ2(3)*DK2(1)-DJ2(1)*DK2(3)
      DI2(3)=DJ2(1)*DK2(2)-DJ2(2)*DK2(1)
C
      TVW2(1)=TN(1)*DI2(1)+TN(2)*DI2(2)+TN(3)*DI2(3)
      TVW2(2)=TN(1)*DJ2(1)+TN(2)*DJ2(2)+TN(3)*DJ2(3)
      TVW2(3)=TN(1)*DK2(1)+TN(2)*DK2(2)+TN(3)*DK2(3)
C
      IF(TVW2(1).EQ.0.) GO TO 901
      TR=1./TVW2(1)
      SP2=1.
      IF(TVW2(1).LT.0.) SP2=-1.
      PD2(1)=PD1(1)
      PD2(2)=TVW2(2)*TR
      PD2(3)=TVW2(3)*TR
C
c      COSL=SQRT(ABS(1.-TN(3)**2))
      cosl=sqrt(tn(1)**2+tn(2)**2)/sqrt(tn(1)**2+tn(2)**2+tn(3)**2)
      IF(COSL.EQ.0.) GO TO 902
      COSL1=1./COSL
      UN(1)=-TN(2)*COSL1
      UN(2)=TN(1)*COSL1
      UN(3)=0.
C
      VN(1)=-TN(3)*UN(2)
      VN(2)=TN(3)*UN(1)
      VN(3)=COSL
C
      UJ1=UN(1)*DJ1(1)+UN(2)*DJ1(2)+UN(3)*DJ1(3)
      UK1=UN(1)*DK1(1)+UN(2)*DK1(2)+UN(3)*DK1(3)
      VJ1=VN(1)*DJ1(1)+VN(2)*DJ1(2)+VN(3)*DJ1(3)
      VK1=VN(1)*DK1(1)+VN(2)*DK1(2)+VN(3)*DK1(3)
C
      UJ2=UN(1)*DJ2(1)+UN(2)*DJ2(2)+UN(3)*DJ2(3)
      UK2=UN(1)*DK2(1)+UN(2)*DK2(2)+UN(3)*DK2(3)
      VJ2=VN(1)*DJ2(1)+VN(2)*DJ2(2)+VN(3)*DJ2(3)
      VK2=VN(1)*DK2(1)+VN(2)*DK2(2)+VN(3)*DK2(3)
C
      J=0
      DO 10 I=1,5
         DO 4 K=I,5
            J=J+1
            A(I,K)=0.
            A(K,I)=0.
            S(J)=RD1(J)
    4    CONTINUE
   10 CONTINUE
C
      IF(CH.EQ.0.) GO TO 6
      HA=SQRT(H(1)**2+H(2)**2+H(3)**2)
      HAM=HA*PM
      IF(HAM.EQ.0.) GO TO 6
      HM=CH/HA
C
      Q=-HAM*CFACT8
      TT=-Q*TR**3
      SJ1I2=DJ1(1)*DI2(1)+DJ1(2)*DI2(2)+DJ1(3)*DI2(3)
      SK1I2=DK1(1)*DI2(1)+DK1(2)*DI2(2)+DK1(3)*DI2(3)
      SK2U=DK2(1)*UN(1)+DK2(2)*UN(2)+DK2(3)*UN(3)
      SK2V=DK2(1)*VN(1)+DK2(2)*VN(2)+DK2(3)*VN(3)
      SJ2U=DJ2(1)*UN(1)+DJ2(2)*UN(2)+DJ2(3)*UN(3)
      SJ2V=DJ2(1)*VN(1)+DJ2(2)*VN(2)+DJ2(3)*VN(3)
C
      SINZ=-(H(1)*UN(1)+H(2)*UN(2)+H(3)*UN(3))*HM
      COSZ= (H(1)*VN(1)+H(2)*VN(2)+H(3)*VN(3))*HM
      A(2,4)=-TT*SJ1I2*(SK2U*SINZ-SK2V*COSZ)
      A(2,5)=-TT*SK1I2*(SK2U*SINZ-SK2V*COSZ)
      A(3,4)= TT*SJ1I2*(SJ2U*SINZ-SJ2V*COSZ)
      A(3,5)= TT*SK1I2*(SJ2U*SINZ-SJ2V*COSZ)
C
    6 A(1,1)=1.
      A(4,4)=TR*(UJ1*VK2-VJ1*UK2)
      A(4,5)=TR*(UK1*VK2-VK1*UK2)
      A(5,4)=TR*(VJ1*UJ2-UJ1*VJ2)
      A(5,5)=TR*(VK1*UJ2-UK1*VJ2)
C
      TS=TR*TVW1(1)
      A(2,2)=A(4,4)*TS
      A(2,3)=A(4,5)*TS
      A(3,2)=A(5,4)*TS
      A(3,3)=A(5,5)*TS
C
      CALL SSMT5T(A,S,S)
C
      J=0
      DO 25 I=1,5
         DO 20 K=I,5
            J=J+1
            RD2(J)=S(J)
   20    CONTINUE
   25 CONTINUE
C
      RETURN
C
C *** ERROR EXITS
C
  901 IERR=1
      GO TO 910
  902 IERR=2
  910 RETURN
      END
*
CDECK  ID>, TRPTSC. 
*CMZ :  3.21/02 29/03/94  15.41.49  by  S.Giani
*-- Author :
      SUBROUTINE TRPTSC(PC,RC,PD,RD,IERR)
C
C *** TRANSFORMS ERROR MATRIX
C     FROM   SC   VARIABLES (1/Pt,LAMBDA,PHI,YT,ZT)
C     FROM   SC   VARIABLES (1/P,LAMBDA,PHI,YT,ZT)


      IMPLICIT DOUBLE PRECISION (A-H,O-Z)
      REAL  PC,PD,RC,RD
      DOUBLE PRECISION A, B, S, TN, T, COSL, SINL, COSP, SINP, COSL1
      INTEGER          NEW
      COMMON /TRCOM3/ A(5,5), B(5,5), S(15),
     +                TN(3), T(5), COSL, SINL, COSP, SINP, COSL1, NEW
      DIMENSION PC(3),PD(3),RC(15),RD(15)
*
*______________________________________________________________________
*
      IERR = 0
      COSL  = COS(PC(2))
      IF (ABS(COSL).EQ.0) GO TO 901
      SINL  =    SIN(PC(2))
*
      PD(1) = PC(1)*COSL
      PD(2) = PC(2)
      PD(3) = PC(3)
*
      J=0
*
      DO 10 I=1,5
         DO 5 K=I,5
            J=J+1
            A(I,K)=0.
            A(K,I)=0.
            S(J)=RC(J)
    5    CONTINUE
   10 CONTINUE
*
      A(1,1) = COSL
      A(2,2) = 1.D0
      A(3,3) = 1.D0
      A(4,4) = 1.D0
      A(5,5) = 1.D0
*
      A(1,2) = -PC(1)*SINL
C
      CALL SSMT5T(A,S,S)
C
      DO 25 J=1,15
        RD(J)=S(J)
   25 CONTINUE
C
      RETURN
C
C *** ERROR EXITS
C
  901 IERR=1
  910 CONTINUE
C
      END
CDECK  ID>, TRPTSD. 
*CMZ :  3.21/02 29/03/94  15.41.49  by  S.Giani
*-- Author :
      SUBROUTINE TRPTSD(PD,RD,PC,RC,H,CH,IERR,SPU,DJ,DK)
*
***************************************************************************
C
C *** TRANSFORMS ERROR MATRIX
C     FROM        VARIABLES (1/Pt,V',W',V,W)
C     FROM        VARIABLES (1/P, V',W',V,W)
C
C
      IMPLICIT DOUBLE PRECISION (A-H,O-Z)
      REAL   PD,PC,H,RC,RD,CH,DJ,DK,SPU
      DOUBLE PRECISION A, B, S, TN, T, COSL, SINL, COSP, SINP, COSL1
      INTEGER          NEW
      COMMON /TRCOM3/ A(5,5), B(5,5), S(15),
     +                TN(3), T(5), COSL, SINL, COSP, SINP, COSL1, NEW
      DIMENSION PD(3),PC(3),H(3),RC(15),RD(15),DJ(3),DK(3)
      DIMENSION UN(3),VN(3),DI(3),TVW(3)
C
      DATA CFACT8 / 2.997925 D-4 /
*
**_____________________________________________________________________
*
      IERR=0
      TVW(1)=1./SQRT(1.+PD(2)**2+PD(3)**2)
      IF(SPU.LT.0.) TVW(1)=-TVW(1)
      TVW(2)=PD(2)*TVW(1)
      TVW(3)=PD(3)*TVW(1)
C
      DI(1)=DJ(2)*DK(3)-DJ(3)*DK(2)
      DI(2)=DJ(3)*DK(1)-DJ(1)*DK(3)
      DI(3)=DJ(1)*DK(2)-DJ(2)*DK(1)
C
      DO 5 I=1,3
         TN(I)=TVW(1)*DI(I)+TVW(2)*DJ(I)+TVW(3)*DK(I)
    5 CONTINUE
C
c      COSL=SQRT(ABS(1.-TN(3)**2))
      cosl=sqrt(tn(1)**2+tn(2)**2)/sqrt(tn(1)**2+tn(2)**2+tn(3)**2)
      IF (COSL .LT. 1.E-30) COSL = 1.E-30
      COSL1=1./COSL
      SINL  = TN(3)
*
      PC(1)=PD(1)*COSL
      PC(2)=PD(2)
      PC(3)=PD(3)
      PM=PC(1)
*
      IF (ABS (TN(1)) .LT. 1.E-30) TN(1) = 1.E-30
C
      UN(1)=-TN(2)*COSL1
      UN(2)=TN(1)*COSL1
      UN(3)=0.
C
      VN(1)=-TN(3)*UN(2)
      VN(2)=TN(3)*UN(1)
      VN(3)=COSL

C
      UJ=UN(1)*DJ(1)+UN(2)*DJ(2)+UN(3)*DJ(3)
      UK=UN(1)*DK(1)+UN(2)*DK(2)+UN(3)*DK(3)
      VJ=VN(1)*DJ(1)+VN(2)*DJ(2)+VN(3)*DJ(3)
      VK=VN(1)*DK(1)+VN(2)*DK(2)+VN(3)*DK(3)
C
      J=0
      DO 10 I=1,5
         DO 4 K=I,5
            J=J+1
            A(I,K)=0.
            A(K,I)=0.
            S(J)=RD(J)
    4    CONTINUE
   10 CONTINUE
C
      IF(CH.EQ.0.) GO TO 6
      HA=SQRT(H(1)**2+H(2)**2+H(3)**2)
      HAM=HA*PM
      IF(HAM.EQ.0.) GO TO 6
      HM=CH/HA
C
      Q=-HAM*CFACT8
C
      SINZ=-(H(1)*UN(1)+H(2)*UN(2)+H(3)*UN(3))*HM
      COSZ= (H(1)*VN(1)+H(2)*VN(2)+H(3)*VN(3))*HM
      A(1,4)=Q*TVW(2)*SINZ*(SINL*PD(1))
      A(1,5)=Q*TVW(3)*SINZ*(SINL*PD(1))
C
6     continue
      A(1,1) = COSL
      A(2,2) = 1.
      A(3,3) = 1.
      A(4,4) = 1.
      A(5,5) = 1.
*
      A(1,2)=-TVW(1)*VJ*(SINL*PD(1))
      A(1,3)=-TVW(1)*VK*(SINL*PD(1))
C
      CALL SSMT5T(A,S,S)
C
      DO J=1,15
        RC(J)=S(J)
      ENDDO
*
      END
CDECK  ID>, TRSCPT. 
*CMZ :  3.21/02 29/03/94  15.41.49  by  S.Giani
*-- Author :
      SUBROUTINE TRSCPT(PC,RC,PD,RD,IERR)
C
C *** TRANSFORMS ERROR MATRIX
C     FROM   SC   VARIABLES (1/P,LAMBDA,PHI,YT,ZT)
C     FROM   SC   VARIABLES (1/Pt,LAMBDA,PHI,YT,ZT)


      IMPLICIT DOUBLE PRECISION (A-H,O-Z)
      REAL  PC,PD,RC,RD
      DOUBLE PRECISION A, B, S, TN, T, COSL, SINL, COSP, SINP, COSL1
      INTEGER          NEW
      COMMON /TRCOM3/ A(5,5), B(5,5), S(15),
     +                TN(3), T(5), COSL, SINL, COSP, SINP, COSL1, NEW
      DIMENSION PC(3),PD(3),RC(15),RD(15)
*
*______________________________________________________________________
*
      IERR = 0
      COSL  = COS(PC(2))
      IF (ABS(COSL).EQ.0) GO TO 901
      COSL1 = 1./COSL
      TANL  =    TAN(PC(2))
*
      PD(1) = PC(1)*COSL1
      PD(2) = PC(2)
      PD(3) = PC(3)
*
      J=0
*
      DO 10 I=1,5
         DO 5 K=I,5
            J=J+1
            A(I,K)=0.
            A(K,I)=0.
            S(J)=RC(J)
    5    CONTINUE
   10 CONTINUE
*
      A(1,1) = COSL1
      A(2,2) = 1.D0
      A(3,3) = 1.D0
      A(4,4) = 1.D0
      A(5,5) = 1.D0
*
      A(1,2) = PD(1)*TANL
C
      CALL SSMT5T(A,S,S)
C
      DO 25 J=1,15
        RD(J)=S(J)
   25 CONTINUE
C
      RETURN
C
C *** ERROR EXITS
C
  901 IERR=1
  910 CONTINUE
C
      END
CDECK  ID>, TRSDPT. 
*CMZ :  3.21/02 29/03/94  15.41.49  by  S.Giani
*-- Author :
      SUBROUTINE TRSDPT(PD,RD,PC,RC,H,CH,IERR,SPU,DJ,DK)
*
***************************************************************************
C
C *** TRANSFORMS ERROR MATRIX
C     FROM        VARIABLES (1/P,V',W',V,W)
C     FROM        VARIABLES (1/Pt,V',W',V,W)
C
C
      IMPLICIT DOUBLE PRECISION (A-H,O-Z)
      REAL   PD,PC,H,RC,RD,CH,DJ,DK,SPU
      DOUBLE PRECISION A, B, S, TN, T, COSL, SINL, COSP, SINP, COSL1
      INTEGER          NEW
      COMMON /TRCOM3/ A(5,5), B(5,5), S(15),
     +                TN(3), T(5), COSL, SINL, COSP, SINP, COSL1, NEW
      DIMENSION PD(3),PC(3),H(3),RC(15),RD(15),DJ(3),DK(3)
      DIMENSION UN(3),VN(3),DI(3),TVW(3)
C
      DATA CFACT8 / 2.997925 D-4 /
*
**_____________________________________________________________________
*
      IERR=0
      PM=PD(1)
      TVW(1)=1./SQRT(1.+PD(2)**2+PD(3)**2)
      IF(SPU.LT.0.) TVW(1)=-TVW(1)
      TVW(2)=PD(2)*TVW(1)
      TVW(3)=PD(3)*TVW(1)
C
      DI(1)=DJ(2)*DK(3)-DJ(3)*DK(2)
      DI(2)=DJ(3)*DK(1)-DJ(1)*DK(3)
      DI(3)=DJ(1)*DK(2)-DJ(2)*DK(1)
C
      DO 5 I=1,3
         TN(I)=TVW(1)*DI(I)+TVW(2)*DJ(I)+TVW(3)*DK(I)
    5 CONTINUE
C
c      COSL=SQRT(ABS(1.-TN(3)**2))
      cosl=sqrt(tn(1)**2+tn(2)**2)/sqrt(tn(1)**2+tn(2)**2+tn(3)**2)
      IF (COSL .LT. 1.E-30) COSL = 1.E-30
      COSL1=1./COSL
      TANL  = TN(3)*COSL1
*
      PC(1)=PD(1)*COSL1
      PC(2)=PD(2)
      PC(3)=PD(3)
*
      IF (ABS (TN(1)) .LT. 1.E-30) TN(1) = 1.E-30
C
      UN(1)=-TN(2)*COSL1
      UN(2)=TN(1)*COSL1
      UN(3)=0.
C
      VN(1)=-TN(3)*UN(2)
      VN(2)=TN(3)*UN(1)
      VN(3)=COSL

C
      UJ=UN(1)*DJ(1)+UN(2)*DJ(2)+UN(3)*DJ(3)
      UK=UN(1)*DK(1)+UN(2)*DK(2)+UN(3)*DK(3)
      VJ=VN(1)*DJ(1)+VN(2)*DJ(2)+VN(3)*DJ(3)
      VK=VN(1)*DK(1)+VN(2)*DK(2)+VN(3)*DK(3)
C
      J=0
      DO 10 I=1,5
         DO 4 K=I,5
            J=J+1
            A(I,K)=0.
            A(K,I)=0.
            S(J)=RD(J)
    4    CONTINUE
   10 CONTINUE
C
      IF(CH.EQ.0.) GO TO 6
      HA=SQRT(H(1)**2+H(2)**2+H(3)**2)
      HAM=HA*PM
      IF(HAM.EQ.0.) GO TO 6
      HM=CH/HA
C
      Q=-HAM*CFACT8
C
      SINZ=-(H(1)*UN(1)+H(2)*UN(2)+H(3)*UN(3))*HM
      COSZ= (H(1)*VN(1)+H(2)*VN(2)+H(3)*VN(3))*HM
      A(1,4)=-Q*TVW(2)*SINZ*(TANL*PC(1))
      A(1,5)=-Q*TVW(3)*SINZ*(TANL*PC(1))
C
6     continue
      A(1,1) = COSL1
      A(2,2) = 1.
      A(3,3) = 1.
      A(4,4) = 1.
      A(5,5) = 1.
*
      A(1,2)=TVW(1)*VJ*(TANL*PC(1))
      A(1,3)=TVW(1)*VK*(TANL*PC(1))
C
      CALL SSMT5T(A,S,S)
C
      DO J=1,15
        RC(J)=S(J)
      ENDDO
*
      END
CDECK  ID>, BLANKDEK.   
*CMZ :  3.21/02 29/03/94  15.41.50  by  S.Giani
*-- Author :
*
CDECK  ID>, DCROSS. 
*CMZ :  3.21/02 29/03/94  15.41.50  by  S.Giani
*-- Author :
      SUBROUTINE DCROSS(X,Y,Z)
*     ========================
*
*     Double Precision Vector Product
*
*     Author: V.Innocente (Naples)
*
      IMPLICIT DOUBLE PRECISION ( A-H, O-Z )
      DIMENSION X(3),Y(3),Z(3)
*
      Z(1) = X(2)*Y(3)-X(3)*Y(2)
      Z(2) = X(3)*Y(1)-X(1)*Y(3)
      Z(3) = X(1)*Y(2)-X(2)*Y(1)
*
      END
CDECK  ID>, DMM55.  
*CMZ :  3.21/02 29/03/94  15.41.50  by  S.Giani
*-- Author :
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

CDECK  ID>, UPCARD. 
      subroutine upcard
      COMMON/GCUNIT/LIN,LOUT,NUNITS,LUNITS(5)
      INTEGER LIN,LOUT,NUNITS,LUNITS
      COMMON/GCMAIL/CHMAIL
      CHARACTER*132 CHMAIL
C
      COMMON/GCKINE/IKINE,PKINE(10),ITRA,ISTAK,IVERT,IPART,ITRTYP
     +      ,NAPART(5),AMASS,CHARGE,TLIFE,VERT(3),PVERT(4),IPAOLD
C
      INTEGER       IKINE,ITRA,ISTAK,IVERT,IPART,ITRTYP,NAPART,IPAOLD
      REAL          PKINE,AMASS,CHARGE,TLIFE,VERT,PVERT
C
*
*     xnois
*   1 -switch on/off
*   2 - average number of falses
*   3 - distance(cm) from true points
      parameter (nitagm=200)
      common /zeroC/izero,rt
      common /muon_idC/muon_id
      common /ioptC/iopt,iopt2
      common /myfitC/ikal,epsmy,nitermy,iextmy,iprmy,ilh
      COMMON/CDHSC/xmuen,as1,as2,as3,
     +     rden,ccosr,car,dmax,srk,car2,
     +     a_el,b_el,c_el,i_el,i_wi,a_d,b_d,chi_th,span,
     +     scut,inarrow,ispeed,iphot,
     +     iel,iphix,noal,nofit,lsthm,no3,cosoff,
     +       iminb,nminb,icminb,bangle,bangle1,ben,ibb,ioddeven,
*     +      ntrmi,imux,imfy,esec,xsec,sdwig1,sdwig2,pinit_n,
     +     xnois(4),histl(2),imixt,xmphi,sign,ems(3),idir,
     +   pxini,nmeas,iymeas(10),sigma(27),sigmaz,sigmazz,cuton(2),
     +        iiee,nio,xhiggs,tkin,nsub,isubb(30),tcutoff
     +            ,ANG,SC(5),PB(5),ANCU(2),
     +            BF2(2),PBSC(2),SLICE(13),ISLICE(13),KB(3),ZI,COEF(2),
     +            coeff1,coeff2,ISPEC,nnout,ENL,IGEOM,ILUNT,IWRITE
     +           ,LUNOU1,LUNOU2,LFNAM1(20),LFNAM2(20),
     +            rrnd,RRND1,imed(5),ixmf,XMF(30),AI,xii,yii,vvrt(3),
     +            enarr(1000,3),tharr(1000,3),inc(10,3),
     +            cone(5,50),dctmin(5),dctmax(5),dcpmin(5),
     +            dcpmax(5),dtP7(5),dtT7(5),itag(15),
     +            xarray(8,15,nitagm)
* 315 -eta ; 315 -phi

      REAL ISLICE,KB
      common /xmuon2C/xmuon(10,2),imuon,NIMUON,ixini
      common /xmuonC/xxmuon(7),vvect(7)
        real xxmuon,vvect
        integer icomb(11,5)

      CHARACTER*80 FNAM
      call vzero(sigma(1),10)
      sig1=0.0125
      sig2=0.04
      epsmy=2.e-3
      ems(1)=0.25
      ems(2)=0.09
      ems(3)=0.09
      ikal=1
      nitermy=20
      iextmy=3
      idir=0
      iprmy=2
      imixt=3
      pxini=5.
      sigma(1)=0.001
      sigma(2)=0.0035
      call ffkey('IKAL',ikal,1,'MIXED')
      call FFKEY('IMIXT',imixt,1,'MIXED')
      call FFKEY('EPS',epsmy,5,'MIXED')
      call FFKEY('EMS',ems,3,'REAL')
      call FFKEY('IDIR',idir,1,'MIXED')
      call FFKEY('PXINI',pxini,1,'REAL')
      call FFKEY('SIGMA',sigma,29,'REAL')
      call tkfini
      END
CDECK  ID>, TKFINI. 
          subroutine tkfini
              common /hvsec/ihvse(2)
              data ihvse/1,0/

              common /tkgdefC/tmaxf,smax,fieldmm,cconv,
     +        maxcal,nitm,xtol,iver,ialg,steps,npars,ims,
     +        bound,scale,cfld,sms,pii
              real *4 tmaxf,smax,fieldmm,bound(2,6),cfld,pii,cconv,
     +        scale(6)
              common/tftimeC/timer(100,2),xtime
              common /tkcorrC/
     +        xxn(2),vpar2(2,2),vpar(2,2),vxy(2)
              real *8 xxn,vpar2,vpar,vxy
           data ini /0/
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
           maxcal=8
           smax=50.
           fieldmm=20.
           npars=5
           tmaxf=0.2
           pii=3.1415927
           cfld=3333.*pii/180./(fieldmm+1.e-10)*tmaxf
           cconv=5.e-4
           xtol=1.e-2
           call ffkey('TKFIT',tmaxf,20,'MIXED')
           call ffkey('HISE',ihvse,2,'MIXED' )
999          end
CDECK  ID>, TKFITG. 
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
*
* Output   out(9) ! x0,y0,z0,theta,phi,rig,chi2,ier, err(1/rig)
*
*
* Multiple scattering (default on) can be switched off in all 3 algorithmes
* by datacard TKFIT 12=0
*
              integer layer(1)
              common /tkgdefC/tmaxf,smax,fieldmm,cconv,
     +        maxcal,nitm,xtol,iver,ialg,steps,npars,ims,
     +        bound,scale,cfld,sms,pii
              real *4 tmaxf,smax,fieldmm,bound(2,6),cfld,pii,cconv,
     +        scale(6)
              common/tftimeC/timer(100,2),xtime
              common /tkcorrC/
     +        xxn(2),vpar2(2,2),vpar(2,2),vxy(2)
              real *8 xxn,vpar2,vpar,vxy
              parameter (maxpl=10)
              common /tkfrunC/npoint,xp(3,maxpl),ys(3,maxpl),
     +        yn(3,maxpl),xmass,chrg
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
            call gfpart(ipart,cnn,itrt7,xmass,chrg,tlt7,uwb,nwb)
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
              call tkhmd(1+(ialgo/10)*10,n,x,f,tol,maxcal,ifail,er)
              if(ifail.eq.0)then
*
* Smoothing
*
c               call tkhmd(2,n,x,f,tol,2,ifail1,er1)

                x(5)=x(5)*chrg/abs(chrg)
              endif
             else if(ialg.eq.2.or.ialg.eq.3)then
             call tkfitge(npt,xhits,ysigma,ynormal,truecharge,
     +       ipart,out,ialg-1)
             goto 999
             else if(ialg.eq.4)then
              call TKFITA(NPt, Xhits, ysigma, layer,
     +        IMSa, xmz, OUT,IERR)
              theta=aycos(-sqrt(1-out(3)**2-out(4)**2))
              phi=atan2(out(4),out(3))
              z0=xhits(3,1)
              rig=1/out(5)
              chi2=0
              erig=out(6)
              out(3)=z0
              out(4)=theta
              out(5)=phi
              out(6)=rig
              out(7)=chi2
              out(8)=ierr
              out(9)=erig
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
CDECK  ID>, TKFITGE.
         subroutine tkfitge(npt,xhits,ysigma,ynormal,chrg,iprt,out,
     +    ikal)
*
* Input
*          npt
*          xhits(3,npt)
*          ysigma(3,npt)
*          ynormal(3,npt)
*          pinit
*          chrg
*          iprt
* Output   out(9) ! x0,y0,z0,theta,phi,p,chi2,ier, err(1/p)
*
              common/tftimeC/timer(100,2),xtime
      COMMON/GCFLAG/IDEBUG,IDEMIN,IDEMAX,ITEST,IDRUN,IDEVT,IEORUN
     +        ,IEOTRI,IEVENT,ISWIT(10),IFINIT(20),NEVENT,NRNDM(2)
      COMMON/GCFLAX/BATCH, NOLOG
      LOGICAL BATCH, NOLOG
C
      INTEGER       IDEBUG,IDEMIN,IDEMAX,ITEST,IDRUN,IDEVT,IEORUN
     +        ,IEOTRI,IEVENT,ISWIT,IFINIT,NEVENT,NRNDM
C
*
*
          common /gcfit2/npln,nstp,nstp5
       PARAMETER (NPLAN = 6, NSTOP = NPLAN-1, NSTOP5 = 5*NPLAN )
      COMMON /GCFIT/
     +          CSIX(6,0:NSTOP),CSIY(6,0:NSTOP),CSIZ(6,0:NSTOP)
     +         ,WW(5,5,0:NSTOP),DDT(5,5,0:NSTOP)
     +         ,VV(5,5,0:NSTOP),SS(5,5,0:NSTOP),CC(5,5,0:NSTOP)
     +         ,CHI2,CHI2N,CHI2T,CHI2M
     +         ,PLANI(3,4,0:NSTOP),PLANJ(3,3,0:NSTOP)
      DOUBLE PRECISION
     +          CSIX           ,CSIY           ,CSIZ
     +         ,WW             ,DDT
     +         ,VV             ,SS,              cc
     +         ,CHI2,CHI2N,CHI2T,CHI2M
*
*     CSIZ            measured values
*     CSIY            predicted values
*     CSIX            fitted  values
*
*     VV              Weight on CSIZ
*     WW              tracking Weight
*     SS              fit covariance
*
*     DDT             trasport matrix (transpose)
*
*     CHI2            first chi2
*     CHI2N           estimated new chi2
*     CHI2T           estimated new chi2 due to tracking
*     CHI2M           estimated new chi2 due to measurements
*

      COMMON/GCKINE/IKINE,PKINE(10),ITRA,ISTAK,IVERT,IPART,ITRTYP
     +      ,NAPART(5),AMASS,CHARGE,TLIFE,VERT(3),PVERT(4),IPAOLD
C
      INTEGER       IKINE,ITRA,ISTAK,IVERT,IPART,ITRTYP,NAPART,IPAOLD
      REAL          PKINE,AMASS,CHARGE,TLIFE,VERT,PVERT
C
       real xhits(3,*),ysigma(3,*),ynormal(3,*),pinit,chrg
       real out(*),pnt(3)
      DIMENSION WRK(5),ERROR(3,3),ERRIN(3,3),dcs(3)
      DATA E1S/0.9/    ! % error in 1/p
      DATA E2S/.09/    ! error in py/px
      DATA E3S/.09/    ! error in pz/px
*
*
*
      save ifail
      data ifail/0/
*
      real nx,ny,nz
*
*_____________________________________________________________________
*
*
*
**    tracking versus   0 = FORWARD 1 = BACKWARD
*
      ipart=iprt
      if(chrg.eq.0)then
        out(8)=777
        return
      endif
      npln=npt
      nstp=npln-1
      nstp5=npln*5
      IVER = 0
*
* stupid geane doesn' want to track if z1 < Z2 
*

      if(xhits(3,1).lt.xhits(3,npt))then
c         do i=1,npt
c          do j=1,3
c            ynormal(j,i)=-ynormal(j,i)
c          enddo
c         enddo
c        iver=1
c        do i=1,npt/2
c          do j=1,3
c           tmp=xhits(j,i)
c           xhits(j,i)=xhits(j,npt+1-i)
c           xhits(j,npt+1-i)=tmp
c           tmp=ysigma(j,i)
c           ysigma(j,i)=ysigma(j,npt+1-i)
c           ysigma(j,npt+1-i)=tmp
c           tmp=ynormal(j,i)
c           ynormal(j,i)=ynormal(j,npt+1-i)
c           ynormal(j,npt+1-i)=tmp
c          enddo
c        enddo
      endif   

*
**     compute a starting point trajectory parameters
*
*             get helix crossing three points
*     circle is parameterized as:
*    C*[(X-Xp)**2+(Y-Yp)**2] - 2*alpha*(X-Xp) - 2*beta*(Y-Yp) = 0
*    Xp,Yp is a point on the track;
*    C = 1/r0 is the curvature  ( sign of C is charge of particle );
*   alpha & beta are the direction cosines of the radial vector at Xp,Yp
*    i.e.  alpha = C*(X0-Xp),
*          beta  = C*(Y0-Yp),
*    where center of circle is at X0,Y0.
*    Slope dy/dx of tangent at Xp,Yp is -alpha/beta.
*
*
             r=(ysigma(1,2)-xhits(1,1))**2+
     +         (xhits(2,2)-xhits(2,1))**2+
     +         (xhits(3,2)-xhits(3,1))**2
             r2=(xhits(1,npt)-xhits(1,npt-1))**2+
     +         (xhits(2,npt)-xhits(2,npt-1))**2+
     +         (xhits(3,npt)-xhits(3,npt-1))**2
             r=sqrt(r)
             r2=sqrt(r2)
             nx=(xhits(1,2)-xhits(1,1))/r
             ny=(xhits(2,2)-xhits(2,1))/r
             nz=(xhits(3,2)-xhits(3,1))/r
             cx=nx*(xhits(1,npt)-xhits(1,npt-1))/r2+
     +       ny*(xhits(2,npt)-xhits(2,npt-1))/r2+
     +       nz*(xhits(3,npt)-xhits(3,npt-1))/r2
             dt=aycos(cx)
             if(dt.ne.0)then
              call getfield(bb)
              pinit=-(xhits(3,npt)-xhits(3,1))*abs(chrg)*abs(bb)*
     +        10*2.99e-4/dt
c             pinit=abs(pinit)
             else
             pinit=1000
             endif
             pnt(1)=abs(pinit)*nx
             pnt(2)=abs(pinit)*ny
             pnt(3)=abs(pinit)*nz
*
* Here assumed that exp errors are x,y,z as declared thatis ynormal has
* the diagonal form
* otherwise should be err(x') = err(x)*inv(ynormal)
*
c             call vunit(pnt(1),wrk(1),3)
c             call ucopy(wrk(1), dcs(1),3)
*
*
**    start to load parameters
*
      IF (IVER.EQ.0) THEN
        I1 = 1
        INC = 1
      ELSE
        I1 = NPln
        INC = -1
      ENDIF
      CALL VZERO(VV,50*NPln)
      CALL VZERO(CSIZ,12*NPln)
      CALL VZERO(PLANI,6)
C    was redkaia hujnia
c      PLANI(1,1,0) =  1.
c      PLANI(2,2,0) =  1.
c      CALL CROSS(PLANI(1,1,0),PLANI(1,2,0),PLANI(1,4,0))

      IC = I1
      DO IP=0,Npln-1
         call vzero(plani(1,1,ip),3)
         call ucopy(ynormal(1,ip+1),plani(1,4,ip),3)
c         call ucopy(dcs(1),plani(1,4,ip),3)
         im = 1
         DO K=1,3
            IF (ABS(plani(K,4,ip)).GT.ABS(plani(IM,4,ip))) IM=K
         ENDDO
         km = mod(im,3) + 1
         plani(km,1,ip) = 1.0
         call cross(plani(1,4,ip),plani(1,1,ip),wrk)
         call vunit(wrk,plani(1,2,ip),3)
         call cross(plani(1,2,ip),plani(1,4,ip),plani(1,1,ip))
         call ucopy(plani(1,1,ip),planj(1,1,ip),6)
         call ucopy(plani(1,4,ip),planj(1,3,ip),3)
        PLANI(1,3,IP) = Xhits(1,IC)
        PLANI(2,3,IP) = Xhits(2,IC)
        PLANI(3,3,IP) = Xhits(3,IC)
        CSIZ(4,IP) = VDOT(PLANI(1,1,IP),Xhits(1,IC),3)
        CSIZ(5,IP) = VDOT(PLANI(1,2,IP),Xhits(1,IC),3)
        CSIZ(6,IP) = VDOT(PLANI(1,4,IP),Xhits(1,IC),3)
         call vzero(errIN,9)
         do i=1,3
            errIN(i,i)=ysigma(i,ip+1)**2
         enddo
         call mxmlrt(planj(1,1,ip),errIN,error,3,3)
         IF (IP.EQ.0) THEN
*
**     compute initial covariance matrix
*
             CALL VZERO(CC,50)
             CC(1,1,0) = (E1S/pinit)**2
             CC(2,2,0) = E2S**2
             CC(3,3,0) = E3S**2
             CC(4,4,0) = ERROR(1,1)
             CC(4,5,0) = ERROR(1,2)
             CC(5,4,0) = ERROR(2,1)
             CC(5,5,0) = ERROR(2,2)
          ENDIF
         call rinv(3,error,3,wrk,ifail1)
         vv(4,4,ip) = error(1,1)
         vv(4,5,ip) = error(1,2)
         vv(5,4,ip) = error(2,1)
         vv(5,5,ip) = error(2,2)
        IC = IC + INC
      ENDDO
*         initial parameters
      Pxx = VDOT(PLANI(1,1,0 ),PnT(1),3)
      Pyy = VDOT(PLANI(1,2,0 ),PnT(1),3)
      Pzz = VDOT(PLANI(1,4,0 ),PnT(1),3)
      CSIX(1,0) = 1./pinit
      CSIX(2,0) = pxx/pzz
      CSIX(3,0) = pyy/pzz
      if(iver.ne.0)then
       CSIX(2,0) = -pxx/pzz
       CSIX(3,0) = -pyy/pzz
      endif
      CSIX(4,0) = csiz(4,0)
      CSIX(5,0) = csiz(5,0)
      CSIX(6,0) = csiz(6,0)
      CALL VZERO(DDT(1,1,0),50)
      CALL VZERO(WW(1,1,0),50)
      DO I=1,5
        DDT(I,I,0) = 1.D0
      ENDDO
c      print *,'fitting starting point ',csix(6,0),csix(4,0),csix(5,0)
c     +                                 ,csix(1,0),csix(2,0),csix(3,0)
c      print *,'starting momentum ',1./csix(1,0)
*
**     call the chosen fitting algorithm
*
      if (ikal.eq.1) THEN
        CALL TKGFITMI(iver,ifail)
      elseif (ikal.eq.2) THEN
        CALL TKGFITKF(iver,chrg,ifail)
      endif

c      print *,'fitpoint ',csix(6,0),csix(4,0),csix(5,0)
c     +                                 ,csix(1,0),csix(2,0),csix(3,0)
c      print *,'fit momentum ',1./csix(1,0)
      ier=ifail
      if (ifail.ne.0) then
c        print *,' Fit failed ',ifail
      else
       do k=1,3
        out(k)=csix(4,0)*planj(k,1,0)+csix(5,0)*planj(k,2,0)+
     +        csix(6,0)*planj(k,3,0)
       enddo
       dcs(1)=csix(2,0)
       dcs(2)=csix(3,0)
       dcs(3)=1
       call vunit(dcs(1),wrk(1),3)
       nx=wrk(1)*planj(1,1,0)+wrk(2)*planj(1,2,0)+wrk(3)*planj(1,3,0)
       ny=wrk(1)*planj(2,1,0)+wrk(2)*planj(2,2,0)+wrk(3)*planj(2,3,0)
       nz=wrk(1)*planj(3,1,0)+wrk(2)*planj(3,2,0)+wrk(3)*planj(3,3,0)
       out(5)=atan2(ny,nx)
       out(4)=aycos(nz)
       if(csix(1,0).ne.0)then
         out(6)=1./csix(1,0)/abs(chrg)
       else
         out(6)=1.e10
       endif
       out(7)=chi2n
      endif
       out(8)=ifail
       out(9)=sqrt(ss(1,1,0))*abs(chrg)
999         end
          subroutine tkhmd(icase,n,xc,fc,tol,mxcal,ifail,er)
          common /tkclearC/clear
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
              parameter (maxpl=10)
              common /tkfrunC/npoint,xp(3,maxpl),ys(3,maxpl),
     +        yn(3,maxpl),xmass,chrg
          real *8 xc(6),fc,tol,dlt
          real *8 xcinit
          real *4 point(6)
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
           if(icase/10.ne.0)then
            do i=1,4
             gg(i,5)=0
             gg(5,i)=0
            enddo
            
           endif
           call dsinv(5,gg(1,1),5,ifail)
           if(ifail.ne.0)then
             write(*,*) 'sinv gg ',ifail
             goto 999
           endif
           er=sqrt(2*gg(5,5))
           dlt=(fc-fcb)/(fcb+2*npoint-5+1.e-3)
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
          if(step.gt.steps)then
           call grkuta(chrg,step,vin,out)
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
              real *8 xxn,vpar2,vpar,vxy
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
          sdist=point(7)-sqrt(s1-s2**2)
          if(sdist.lt.0)then
           stept=1.e10
           write(*,*)'tkfitparcyl-E-outside of cyl',sdist,point(7)
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
          sdist=point(7)-sqrt(s1-s2**2)
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
           call grkuta(chrg,step,vin,out)
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
          common /tkclearC/clear
          integer clear
          data clear/1/
          real der(10,2)
          save der
          real derl(*)
           real *4 in(*),out(*),xx(3),h(3),hxy(3,3)
           if(clear.le.0)clear=1
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
CDECK  ID>, TKFLDC. 
          subroutine tkfldc(xx,hxy)
          common /tkigufldC/i,j,xxy(3)
          real xx(3),hxy(3,3)
          real *8 xval,hbase,der(14),erest(14),tkigufld
          external tkigufld
          do i=1,2
            do j=1,3
          ifail=1
          nder=1
          hbase=0.5
          call ucopy(xx(1),xxy(1),3)
          xval=xx(i)
          call d04aaf(XVAL, NDER, HBASE, DER, EREST,
     +    tkigufld,ifail)
            hxy(j,i)=der(1)
            enddo
          enddo
          end
          real *8 function tkigufld(x)
          common /tkigufldC/i,j,xx(3)
          real h(3)
          real *8 x
          xx(i)=x
          call gufld(xx,h)
          tkigufld=h(j)
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
CDECK  ID>, TKGFITMI.   
*CMZ :  3.21/02 29/03/94  15.41.50  by  S.Giani
*-- Author :
      SUBROUTINE TKGFITMI(IVER,IFAFT)
*
*    trak fitting using GEANE and matrix inversion
*
*    IVER = 0  forward  tracking
*           1  backward tracking
*
*    IFAFT = 0  Fit successful
*    IFAFT = 1  Error matrix singular
*    IFAFT = 9  Fit not successful
*
*    Ask V.Innocente for documentation
*
      COMMON/GCKINE/IKINE,PKINE(10),ITRA,ISTAK,IVERT,IPART,ITRTYP
     +      ,NAPART(5),AMASS,CHARGE,TLIFE,VERT(3),PVERT(4),IPAOLD
C
      INTEGER       IKINE,ITRA,ISTAK,IVERT,IPART,ITRTYP,NAPART,IPAOLD
      REAL          PKINE,AMASS,CHARGE,TLIFE,VERT,PVERT
C
      COMMON/GCFLAG/IDEBUG,IDEMIN,IDEMAX,ITEST,IDRUN,IDEVT,IEORUN
     +        ,IEOTRI,IEVENT,ISWIT(10),IFINIT(20),NEVENT,NRNDM(2)
      COMMON/GCFLAX/BATCH, NOLOG
      LOGICAL BATCH, NOLOG
C
      INTEGER       IDEBUG,IDEMIN,IDEMAX,ITEST,IDRUN,IDEVT,IEORUN
     +        ,IEOTRI,IEVENT,ISWIT,IFINIT,NEVENT,NRNDM
C
      INTEGER          MXPRED
      PARAMETER (MXPRED = 150)
      DOUBLE PRECISION ERDTRP
      REAL             ERRIN, ERROUT, ERTRSP, ERXIN, ERXOUT, ERPIN,
     +                 ERPOUT
      INTEGER          NEPRED, INLIST, ILPRED, IEPRED
      COMMON /ERTRIO/  ERDTRP(5,5,MXPRED), ERRIN(15), ERROUT(15,MXPRED),
     +                 ERTRSP(5,5,MXPRED), ERXIN( 3), ERXOUT( 3,MXPRED),
     +                 ERPIN(3), ERPOUT(3,MXPRED), NEPRED,INLIST,ILPRED,
     +                 IEPRED(MXPRED)
      CHARACTER*8     CHOPTI
      LOGICAL         LEEXAC, LELENG, LEONLY, LEPLAN, LEPOIN, LEVOLU
      REAL            ERPLI, ERPLO, ERLENG
      INTEGER         NAMEER, NUMVER, IOVLER
      COMMON /EROPTS/ ERPLI(3,2), ERPLO(3,4,MXPRED), ERLENG(MXPRED),
     +                NAMEER(MXPRED), NUMVER(MXPRED), IOVLER(MXPRED),
     +                LEEXAC, LELENG, LEONLY, LEPLAN, LEPOIN, LEVOLU
      COMMON /EROPTC/CHOPTI


          common /gcfit2/npln,nstp,nstp5
       PARAMETER (NPLAN = 6, NSTOP = NPLAN-1, NSTOP5 = 5*NPLAN )
      COMMON /GCFIT/
     +          CSIX(6,0:NSTOP),CSIY(6,0:NSTOP),CSIZ(6,0:NSTOP)
     +         ,WW(5,5,0:NSTOP),DDT(5,5,0:NSTOP)
     +         ,VV(5,5,0:NSTOP),SS(5,5,0:NSTOP),CC(5,5,0:NSTOP)
     +         ,CHI2,CHI2N,CHI2T,CHI2M
     +         ,PLANI(3,4,0:NSTOP),PLANJ(3,3,0:NSTOP)
      DOUBLE PRECISION
     +          CSIX           ,CSIY           ,CSIZ
     +         ,WW             ,DDT
     +         ,VV             ,SS,              cc
     +         ,CHI2,CHI2N,CHI2T,CHI2M
*
*     CSIZ            measured values
*     CSIY            predicted values
*     CSIX            fitted  values
*
*     VV              Weight on CSIZ
*     WW              tracking Weight
*     SS              fit covariance
*
*     DDT             trasport matrix (transpose)
*
*     CHI2            first chi2
*     CHI2N           estimated new chi2
*     CHI2T           estimated new chi2 due to tracking
*     CHI2M           estimated new chi2 due to measurements
*

*
*
*
      REAL PD(3),RD(15),PC(3),RC(15),H(3),XXI(3),XXO(3),PII(3),POO(3)
      REAL DIF(3)
      INTEGER LSTER(2,100)
      DOUBLE PRECISION AMA(NSTOP5,NSTOP5),SOL(NSTOP5)
      DOUBLE PRECISION XDU55(5,5),WDT(5,5)
     +                ,VDUM(5),VDUM0(5),YDUM(5)
     +                ,ERRA(150),FF,DDUM(5)
      DOUBLE PRECISION DZERO, DSMALL, DBIG
*
      CHARACTER * 4 CHOPT
*
c      PARAMETER (NITER = 2)
*
      DATA H/0.,0.,20./
*
      DATA DZERO,DSMALL,DBIG/0.D0,.1D-8,1.D8/
*_____________________________________________________________________
*
*
         return
          niter=100
         pold=0

      IFAFT = 0
*
*         Define coordinate for which we want the errors
        K = 1
        DO 1 I=1,5
        DO 1 J=I,5
          LSTER(1,K) = I
          LSTER(2,K) = J
          K = K+1
1       CONTINUE
        DO 2 I=6,NPLN*5
        DO 2 J=I,5*((I-1)/5)+5
          LSTER(1,K) = I
          LSTER(2,K) = J
          K = K+1
2       CONTINUE
        NERRA = K-1
*
*
      CALL VZERO(RD,15)
*
      CHOPT = 'EP'
      IF (IVER.NE.0) CHOPT = 'BEP'
*
**     NITER  iterations
*
      DO 101 ITER=1,NITER
*
        CALL UCOPY(CSIX(1,0),CSIY(1,0),12)
*            loop over inner planes
        DO JSTOP = 1,NSTP
*            load starting point
           PD(1) = ABS(CSIX(1,JSTOP-1))
           PD(2) = CSIX(2,JSTOP-1)
           PD(3) = CSIX(3,JSTOP-1)
           DO I=1,3
              XXI(I)=0.
              DO J=1,3
                 XXI(I)=XXI(I)+PLANJ(I,J,JSTOP-1)*CSIX(3+J,JSTOP-1)
              ENDDO
           ENDDO
*
           CH = SIGN(1.D0,CSIX(1,JSTOP-1))
            ityp=ipart
            ipaold=ipart
c            if(ipart.le.32)then
c             ITYP = 5
c             IF ( CH.LT.0)  ITYP = 6
c            else if(ipart.le.120)then
c             ityp=ipart
c             if(ch.lt.0)ityp=ipart+100
c            else
c             ityp=ipart-100
c             if(ch.lt.0)ityp=ipart
c            endif
           CALL GUFLD(XXI,H(1))
           SPU = 1.
           CALL TRSDSC(PD,RD,PC,RC,H,CH,IERR,SPU,
     +                 PLANI(1,1,JSTOP-1),PLANI(1,2,JSTOP-1))
*
           CALL DIRCOS(PC(2),DIF)
           if(pd(1).ne.0)then
           P0 = 1.D0/PD(1)
           else
           p0=1.e5
           endif
           PII(1) = P0*DIF(1)
           PII(2) = P0*DIF(2)
           PII(3) = P0*DIF(3)
           k=0
           do i=1,5
           do j=i,5
             k = k+1
             FF = 1.D0
             IF (K.NE.1.AND.K.LE.5) FF = CH
             RC(K) = FF*CC(I,J,JSTOP-1)
           enddo
           enddo



           CALL EUFILP(1,RC,PLANI(1,1,JSTOP-1),PLANI(1,1,JSTOP))
*                   track
           CALL ERTRAK(XXI,PII,XXO,POO,ITYP,CHOPT)
           IF (IEPRED(1).NE.1) THEN
             print *, 'plane ',JSTOP,' not reached '
             IFAFT = 9
             GO TO 999
           ENDIF
*
*
**            load arriving point
*
           CSIY(1,JSTOP) = CH*ERPOUT(1,1)
           CSIY(2,JSTOP) = ERPOUT(2,1)
           CSIY(3,JSTOP) = ERPOUT(3,1)
           CSIY(4,JSTOP) = VDOT(PLANI(1,1,JSTOP),XXO,3)
           CSIY(5,JSTOP) = VDOT(PLANI(1,2,JSTOP),XXO,3)
           CSIY(6,JSTOP) = VDOT(PLANI(1,4,JSTOP),XXO,3)
           IF (ITER.EQ.1) CALL UCOPY(CSIY(1,JSTOP),CSIX(1,JSTOP),12)
           do i=1,15
               if(errout(i,1).eq.0)errout(i,1)=1.e-34
           enddo
           K=0
           DO I=1,5
           DO J=I,5
             K=K+1
             FF = 1.D0
             IF (K.NE.1.AND.K.LE.5) FF = CH
             WW(I,J,JSTOP) = FF*ERROUT(K,1)
             WW(J,I,JSTOP) = WW(I,J,JSTOP)
           ENDDO
           ENDDO
C            DO I=1,5
C              IF( WW(I,I,JSTOP).LE.DZERO )  THEN
C                 WW(I,I,JSTOP) = SMALL
C              ENDIF
C            ENDDO
            CALL DSINV(5,WW(1,1,JSTOP),5,IFAIL)
c            ifail=1
            IF ( IFAIL.NE.0 )  THEN
c                PRINT *,'DSINV IFAIL',IFAIL,' AT PLANE ',JSTOP
                CALL VZERO(WW(1,1,JSTOP),50)
            ENDIF
*
           DO I=1,5
           DO J=1,5
             FF = 1.D0
             IF (I.EQ.1 .AND. J.NE.1 )  FF = CH
             IF (J.EQ.1 .AND. I.NE.1 )  FF = CH
             DDT(I,J,JSTOP) = FF*ERDTRP(J,I,1)
           ENDDO
           ENDDO
          CALL DVADD(25,WW(1,1,JSTOP),WW(2,1,JSTOP),
     +                  VV(1,1,JSTOP),VV(2,1,JSTOP),
     +                  CC(1,1,JSTOP),CC(2,1,JSTOP))
          CALL DSINV(5,CC(1,1,JSTOP),5,IFAIL)
          IF ( IFAIL.NE.0 )  THEN
c              PRINT *,'CC DSINV IFAIL',IFAIL,' AT PLANE ',JSTOP
              CALL VZERO(CC(1,1,JSTOP),50)
c              CALL ucopy(cc(1,1,0),CC(1,1,JSTOP),50)
              IFAFT = 1
              GOTO 999
          ENDIF
        ENDDO               !  End plane loop
c        IF ( IDEBUG.GE.1 )  THEN
c          WRITE(6,'(I6,''     CSIZ '',/(3X,6E15.5))')ITER,CSIZ
c          WRITE(6,'(I6,''     CSIX '',/(3X,6E15.5))')ITER,CSIX
c          WRITE(6,'(I6,''     CSIY '',/(3X,6E15.5))')ITER,CSIY
c        ENDIF
*
**     Solve now
*
        ISOL = 0
150     CONTINUE
        ISOL = ISOL + 1
*
c        CALL VZERO(AMA,2*NSTOP5*NSTOP5)
c        CALL VZERO(SOL,2*NSTOP5)
        CHI2 = 0.D0
        DO 151 J=1,5
151      VDUM(J) = CSIY(J,0) - CSIX(J,0)
        DO 120 I=0,NSTP
          CALL UCOPY(VDUM,VDUM0,10)
          IF ( I.NE.NSTP ) THEN
            CALL DMMLT(5,5,5,WW(1,1,I+1),WW(1,2,I+1),WW(2,1,I+1),
     1                   DDT(1,1,I+1),DDT(2,1,I+1),DDT(1,2,I+1),
     2                   WDT(1,1),WDT(1,2),WDT(2,1), DDUM )
            CALL DMMLT(5,5,5,DDT(1,1,I+1),DDT(1,2,I+1),DDT(2,1,I+1),
     1                   WDT(1,1),WDT(1,2),WDT(2,1),
     2                   XDU55(1,1),XDU55(1,2),XDU55(2,1), DDUM )
*
            DO 152 J=1,5
              VDUM(J) = CSIY(J,I+1) - CSIX(J,I+1)
152         CONTINUE
          ELSE
            CALL VZERO(XDU55,50)
            CALL VZERO(VDUM,10)
          ENDIF
          DO 153 J=1,5
            YDUM(J) = CSIZ(J,I) - CSIX(J,I)
153       CONTINUE
          II = 5*I
          DO 120 J=1,5
            IJ = II+J
            DO 121 K=1,5
              IK = II+K
              AMA(IJ,IK) = WW(K,J,I) + VV(K,J,I)  + XDU55(K,J)
              IF ( I.NE.NSTP) THEN
                AMA(IJ,IK+5) = -WW(K,1,I+1)*DDT(J,1,I+1)
     2                         -WW(K,2,I+1)*DDT(J,2,I+1)
     3                         -WW(K,3,I+1)*DDT(J,3,I+1)
     4                         -WW(K,4,I+1)*DDT(J,4,I+1)
     5                         -WW(K,5,I+1)*DDT(J,5,I+1)
              ENDIF
              IF ( I.NE.0 ) THEN
                AMA(IJ,IK-5) = -WW(J,1,I)*DDT(K,1,I)
     2                         -WW(J,2,I)*DDT(K,2,I)
     3                         -WW(J,3,I)*DDT(K,3,I)
     4                         -WW(J,4,I)*DDT(K,4,I)
     5                         -WW(J,5,I)*DDT(K,5,I)
              ENDIF
              CHI2 = CHI2
     1         + VDUM0(J)*WW(J,K,I)*VDUM0(K)
     2         +  YDUM(J)*VV(J,K,I)*YDUM(K)
121         CONTINUE
*
            SOL(IJ)=
     1          VDUM0(1)*WW(1,J,I)+VDUM0(2)*WW(2,J,I)
     2         +VDUM0(3)*WW(3,J,I)+VDUM0(4)*WW(4,J,I)+VDUM0(5)*WW(5,J,I)
     3         +YDUM(1)*VV(1,J,I)+YDUM(2)*VV(2,J,I)
     4         +YDUM(3)*VV(3,J,I)+YDUM(4)*VV(4,J,I)+YDUM(5)*VV(5,J,I)
             IF (I.NE.NSTP) SOL(IJ) = SOL(IJ)
     5         - VDUM(1)*WDT(1,J)-VDUM(2)*WDT(2,J)
     6         - VDUM(3)*WDT(3,J)-VDUM(4)*WDT(4,J)-VDUM(5)*WDT(5,J)
*
120     CONTINUE
*            "home made" band code
*           ( can be optimized )

        MBAN = 9
           call timex(st1)
        CALL DSBEQN(NSTP5,MBAN,AMA,NSToP5,IFAIL,1,SOL)
        IF ( IFAIL.NE.0 ) THEN
*
**      inversion failed!!
*
          PRINT *,' DSBEQN IFAIL ',IFAIL
          IFAFT = 9
          GO TO 999
        ENDIF
*
        CALL DSBFINV(NSTP5,MBAN,AMA,NSToP5,NERRA,ERRA,LSTER)
*
*      compute new fit values  and chi2s
        CHI2T = 0.D0
        CHI2M = 0.D0
        DO I = 0,NSTP
          II = 5*I
          DO J=1,5
            IJ= II+J
            CSIX(J,I) = CSIX(J,I) + SOL(IJ)
          ENDDO
          IF (I.NE.0) THEN
             CALL UCOPY(CSIY(1,I),VDUM,10)
             DO J=1,5
               IJ= II+J -5
               DO K=1,5
                 VDUM(K) = VDUM(K) + DDT(J,K,I)*SOL(IJ)
               ENDDO
             ENDDO
          ENDIF
          DO J = 1,5
            DO K=1,5
              CHI2M = CHI2M +
     +         (CSIX(K,I)-CSIZ(K,I))*VV(K,J,I)*(CSIX(J,I)-CSIZ(J,I))
            ENDDO
            IF (I.NE.0) THEN
              DO K=1,5
                CHI2T = CHI2T +
     +           (CSIX(K,I)-VDUM(K))*WW(K,J,I)*(CSIX(J,I)-VDUM(J))
              ENDDO
            ENDIF
          ENDDO
        ENDDO
        CHI2N = CHI2T + CHI2M
c          print *,' chi2s ',iter,chi2,chi2n,chi2t,chi2m,1/csix(1,0)
*
          NDOF = 2*npln-5
*
*

          dlt1=(pold-chi2)/(pold+ndof+1.e-10)
          dlt2=(chi2-chi2n)/(chi2+ndof+1.e-10)
          epsmy=1.e-2
          if(abs(dlt1).le.3*epsmy.and.abs(dlt2).le.epsmy)then
           ifaft=0
           goto 999
          endif
          nitermy=20
          if(iter.ge.nitermy)then
           ifail=2
           goto 999
          endif
          pold=chi2
101   CONTINUE
*
**      done
*
*       load final coovariance
*
*
*
*
*       load final coovariance
*
      call vzero(ss,50*npln)
      do ie=1,nerra
       iplan = (lster(1,ie)-1)/5
       jplan = (lster(2,ie)-1)/5
       if (iplan.eq.jplan) then
         i = lster(1,ie) - 5*iplan
         j = lster(2,ie) - 5*jplan
         ss(i,j,iplan) = erra(ie)
         if (i.eq.j.and.erra(ie).le.DZERO) then
           ss(i,j,iplan) = DBIG
           IFAFT = 1
         endif
       endif
      enddo
999   CONTINUE
*
      END
CDECK  ID>, TKGFITKF.   
*CMZ :  3.21/02 29/03/94  15.41.50  by  S.Giani
*-- Author :
      SUBROUTINE TKGFITKF(IVER,cho,IFAFT)
*
*    trak fitting using GEANE and KALMAN FILTER
*
*    IVER = 0  forward  tracking
*           1  backward tracking
*
*    IFAFT = 0  Fit successful
*    IFAFT = 1  Error matrix singular
*    IFAFT = 9  Fit not successful
*
*    Ask V.Innocente for documentation
*
      COMMON/GCKINE/IKINE,PKINE(10),ITRA,ISTAK,IVERT,IPART,ITRTYP
     +      ,NAPART(5),AMASS,CHARGE,TLIFE,VERT(3),PVERT(4),IPAOLD
C
      INTEGER       IKINE,ITRA,ISTAK,IVERT,IPART,ITRTYP,NAPART,IPAOLD
      REAL          PKINE,AMASS,CHARGE,TLIFE,VERT,PVERT
C
      COMMON/GCFLAG/IDEBUG,IDEMIN,IDEMAX,ITEST,IDRUN,IDEVT,IEORUN
     +        ,IEOTRI,IEVENT,ISWIT(10),IFINIT(20),NEVENT,NRNDM(2)
      COMMON/GCFLAX/BATCH, NOLOG
      LOGICAL BATCH, NOLOG
C
      INTEGER       IDEBUG,IDEMIN,IDEMAX,ITEST,IDRUN,IDEVT,IEORUN
     +        ,IEOTRI,IEVENT,ISWIT,IFINIT,NEVENT,NRNDM
C
      INTEGER          MXPRED
      PARAMETER (MXPRED = 150)
      DOUBLE PRECISION ERDTRP
      REAL             ERRIN, ERROUT, ERTRSP, ERXIN, ERXOUT, ERPIN,
     +                 ERPOUT
      INTEGER          NEPRED, INLIST, ILPRED, IEPRED
      COMMON /ERTRIO/  ERDTRP(5,5,MXPRED), ERRIN(15), ERROUT(15,MXPRED),
     +                 ERTRSP(5,5,MXPRED), ERXIN( 3), ERXOUT( 3,MXPRED),
     +                 ERPIN(3), ERPOUT(3,MXPRED), NEPRED,INLIST,ILPRED,
     +                 IEPRED(MXPRED)
      CHARACTER*8     CHOPTI
      LOGICAL         LEEXAC, LELENG, LEONLY, LEPLAN, LEPOIN, LEVOLU
      REAL            ERPLI, ERPLO, ERLENG
      INTEGER         NAMEER, NUMVER, IOVLER
      COMMON /EROPTS/ ERPLI(3,2), ERPLO(3,4,MXPRED), ERLENG(MXPRED),
     +                NAMEER(MXPRED), NUMVER(MXPRED), IOVLER(MXPRED),
     +                LEEXAC, LELENG, LEONLY, LEPLAN, LEPOIN, LEVOLU
      COMMON /EROPTC/CHOPTI


          common /gcfit2/npln,nstp,nstp5
       PARAMETER (NPLAN = 6, NSTOP = NPLAN-1, NSTOP5 = 5*NPLAN )
      COMMON /GCFIT/
     +          CSIX(6,0:NSTOP),CSIY(6,0:NSTOP),CSIZ(6,0:NSTOP)
     +         ,WW(5,5,0:NSTOP),DDT(5,5,0:NSTOP)
     +         ,VV(5,5,0:NSTOP),SS(5,5,0:NSTOP),CC(5,5,0:NSTOP)
     +         ,CHI2,CHI2N,CHI2T,CHI2M
     +         ,PLANI(3,4,0:NSTOP),PLANJ(3,3,0:NSTOP)
      DOUBLE PRECISION
     +          CSIX           ,CSIY           ,CSIZ
     +         ,WW             ,DDT
     +         ,VV             ,SS,              cc
     +         ,CHI2,CHI2N,CHI2T,CHI2M
*
*     CSIZ            measured values
*     CSIY            predicted values
*     CSIX            fitted  values
*
*     VV              Weight on CSIZ
*     WW              tracking Weight
*     SS              fit covariance
*
*     DDT             trasport matrix (transpose)
*
*     CHI2            first chi2
*     CHI2N           estimated new chi2
*     CHI2T           estimated new chi2 due to tracking
*     CHI2M           estimated new chi2 due to measurements
*

*
*
*
*
      REAL PD(3),RD(15),PC(3),RC(15),H(3),XXI(3),XXO(3),PII(3),POO(3)
      REAL DIF(3)
*
      DOUBLE PRECISION VDUM(5),YDUM(5),AA(5,5),XDU55(5,5)
      DOUBLE PRECISION WI(5,5,0:NSTOP)
      DOUBLE PRECISION DDUM(5)
      DOUBLE PRECISION DZERO, DSMALL, DBIG
*
      CHARACTER * 4 CHOPT
*
c      PARAMETER (NITER = 2)
*
      DATA H/0.,0.,20./   ! magnetc field (not really needed)
*
*
      DATA DZERO,DSMALL,DBIG/0.D0,.1D-8,1.D8/
*_____________________________________________________________________
*
          ipo=ipart
          niter=100
         pold=0
      IFAFT = 1
c      return
*
*
      CALL VZERO(RD,15)
*
      CHOPT = 'EP'
      IF (IVER.NE.0) CHOPT = 'BEP'
*
**     compute initial coovariance matrix (to be moved in GEAFIT?)
*
c      CALL VZERO(CC,50)
      CALL VZERO(WI,50)
c      CC(1,1,0) = (E1S*CSIX(1,0))**2
c      CC(2,2,0) = E2S**2
c      CC(3,3,0) = E3S**2
c      CC(4,4,0) = 1.D0/VV(4,4,0)
c      CC(5,5,0) = 1.D0/VV(5,5,0)
*
**     NITER  iterations
*
      DO 101 ITER=1,NITER
*
ccc        IF (ITER.NE.1) CALL UCOPY(SS(1,1,0),CC(1,1,0),50)
        CALL UCOPY(CSIX(1,0),CSIY(1,0),12)
*                    chi2
        DO J = 1,5
          DO K=1,5
            CHI2 =
     +       (CSIX(K,0)-CSIZ(K,0))*VV(K,J,0)*
     +       (CSIX(J,0)-CSIZ(J,0))
          ENDDO
        ENDDO
*            loop over inner planes
        DO JSTOP = 1,NSTP
*            load starting point
           PD(1) = ABS(CSIX(1,JSTOP-1))
           PD(2) = CSIX(2,JSTOP-1)
           PD(3) = CSIX(3,JSTOP-1)
           DO I=1,3
              XXI(I)=0.
              DO J=1,3
                 XXI(I)=XXI(I)+PLANJ(I,J,JSTOP-1)*CSIX(3+J,JSTOP-1)
              ENDDO
           ENDDO
*
           CH = SIGN(1.D0,CSIX(1,JSTOP-1))
           if(ipo.le.32)then
             idf=1
           else
             idf=100
           endif
            if (ch*cho.gt.0)then
             ityp=ipo
            else if (ch.lt.0)then
             ityp=ipo+idf
            else
             ityp=ipo-idf
            endif
           CALL GUFLD(XXI,H)
           SPU = 1.
           CALL TRSDSC(PD,RD,PC,RC,H,CH,IERR,SPU,
     +                 PLANI(1,1,JSTOP-1),PLANI(1,2,JSTOP-1))
*
**         Load coov matrix at plane jstop-1
*
           k=0
           do i=1,5
           do j=i,5
             k = k+1
             FF = 1.D0
             IF (K.NE.1.AND.K.LE.5) FF = CH
             RC(K) = FF*CC(I,J,JSTOP-1)
           enddo
           enddo
*
           CALL DIRCOS(PC(2),DIF)
           if(pd(1).ne.0)then
           P0 = 1.D0/PD(1)
           else
           p0=1.e5
           endif
           PII(1) = P0*DIF(1)
           PII(2) = P0*DIF(2)
           PII(3) = P0*DIF(3)
           CALL EUFILP(1,RC,PLANI(1,1,JSTOP-1),PLANI(1,1,JSTOP))
*                   track
           CALL ERTRAK(XXI,PII,XXO,POO,ITYP,CHOPT)
           IF (IEPRED(1).NE.1) THEN
             print *, 'plane ',JSTOP,' not reached '
             IFAFT = 9
             GO TO 999
           ENDIF
*
*
**            load arriving point
*
           CSIY(1,JSTOP) = CH*ERPOUT(1,1)
           CSIY(2,JSTOP) = ERPOUT(2,1)
           CSIY(3,JSTOP) = ERPOUT(3,1)
           CSIY(4,JSTOP) =
     +         PLANI(1,1,JSTOP)*XXO(1)+PLANI(2,1,JSTOP)*XXO(2)+
     +         PLANI(3,1,JSTOP)*XXO(3)
           CSIY(5,JSTOP) =
     +         PLANI(1,2,JSTOP)*XXO(1)+PLANI(2,2,JSTOP)*XXO(2)+
     +         PLANI(3,2,JSTOP)*XXO(3)
           CSIY(6,JSTOP) =
     +         PLANI(1,4,JSTOP)*XXO(1)+PLANI(2,4,JSTOP)*XXO(2)+
     +         PLANI(3,4,JSTOP)*XXO(3)
           K=0
           DO I=1,5
           DO J=I,5
             K=K+1
             FF = 1.D0
             IF (K.NE.1.AND.K.LE.5) FF = CH
             WW(I,J,JSTOP) = FF*ERROUT(K,1)
             WW(J,I,JSTOP) = WW(I,J,JSTOP)
           ENDDO
           ENDDO
C            DO I=1,5
C              IF( WW(I,I,JSTOP).LE.DZERO )  THEN
C                 WW(I,I,JSTOP) = DSMALL
C              ENDIF
C            ENDDO
            CALL UCOPY(WW(1,1,JSTOP),WI(1,1,JSTOP),50)
            CALL DSINV(5,WW(1,1,JSTOP),5,IFAIL)
            IF ( IFAIL.NE.0 )  THEN
c                PRINT *,'DSINV IFAIL',IFAIL,' AT PLANE ',JSTOP
                CALL VZERO(WW(1,1,JSTOP),50)
                IFAFT = 1
                GOTO 999
             endif
*

           DO I=1,5
           DO J=1,5
             FF = 1.D0
             IF (I.EQ.1 .AND. J.NE.1 )  FF = CH
             IF (J.EQ.1 .AND. I.NE.1 )  FF = CH
             DDT(I,J,JSTOP) = FF*ERDTRP(J,I,1)
           ENDDO
           ENDDO
*
**        filter
*
          CALL DVADD(25,WW(1,1,JSTOP),WW(2,1,JSTOP),
     +                  VV(1,1,JSTOP),VV(2,1,JSTOP),
     +                  CC(1,1,JSTOP),CC(2,1,JSTOP))
          CALL DSINV(5,CC(1,1,JSTOP),5,IFAIL)
          IF ( IFAIL.NE.0 )  THEN
c              PRINT *,'CC DSINV IFAIL',IFAIL,' AT PLANE ',JSTOP
              CALL VZERO(CC(1,1,JSTOP),50)
c              CALL ucopy(cc(1,1,0),CC(1,1,JSTOP),50)
              IFAFT = 1
              GOTO 999
          ENDIF
          CALL DMMPY(5,5,VV(1,1,JSTOP),VV(1,2,JSTOP),VV(2,1,JSTOP),
     +                   CSIZ(1,JSTOP),CSIZ(2,JSTOP),
     +                   YDUM(1),      YDUM(2))
          CALL DMMPA(5,5,WW(1,1,JSTOP),WW(1,2,JSTOP),WW(2,1,JSTOP),
     +                   CSIY(1,JSTOP),CSIY(2,JSTOP),
     +                   YDUM(1),      YDUM(2))
          CALL DMMPY(5,5,CC(1,1,JSTOP),CC(1,2,JSTOP),CC(2,1,JSTOP),
     +                   YDUM(1),      YDUM(2),
     +                   CSIX(1,JSTOP),CSIX(2,JSTOP))
          CSIX(6,JSTOP) = CSIY(6,JSTOP)
*                CHI2
          DO J = 1,5
            DO K=1,5
              CHI2 = CHI2 +
     +         (CSIX(K,JSTOP)-CSIZ(K,JSTOP))*VV(K,J,JSTOP)*
     +         (CSIX(J,JSTOP)-CSIZ(J,JSTOP))                 +
     +         (CSIX(K,JSTOP)-CSIY(K,JSTOP))*WW(K,J,JSTOP)*
     +         (CSIX(J,JSTOP)-CSIY(J,JSTOP))
            ENDDO
          ENDDO
*
        ENDDO               !  End plane loop
*
**               smoothing
*
        CALL UCOPY(CC(1,1,NSTP),SS(1,1,NSTP),50)
*                chi2
        DO J = 1,5
          DO K=1,5
            CHI2M =
     +         (CSIX(K,NSTP)-CSIZ(K,NSTP))*VV(K,J,NSTP)*
     +         (CSIX(J,NSTP)-CSIZ(J,NSTP))
            CHI2T =
     +         (CSIX(K,NSTP)-CSIY(K,NSTP))*WW(K,J,NSTP)*
     +         (CSIX(J,NSTP)-CSIY(J,NSTP))
          ENDDO
        ENDDO
*
        DO JS=NSTP-1,0,-1
          CALL DMMLT(5,5,5,DDT(1,1,JS+1),DDT(1,2,JS+1),DDT(2,1,JS+1),
     1                     WW(1,1,JS+1),WW(1,2,JS+1),WW(2,1,JS+1),
     2                     XDU55(1,1),XDU55(1,2),XDU55(2,1), DDUM )
          CALL DMMLT(5,5,5,CC(1,1,JS),CC(1,2,JS),CC(2,1,JS),
     1                     XDU55(1,1),XDU55(1,2),XDU55(2,1),
     2                     AA(1,1),AA(1,2),AA(2,1), DDUM  )
          CALL DVSUB(25,SS(1,1,JS+1),SS(2,1,JS+1),
     1                  WI(1,1,JS+1),WI(2,1,JS+1),
     2                  XDU55(1,1),XDU55(2,1) )
          CALL DMMLT(5,5,5,XDU55(1,1),XDU55(1,2),XDU55(2,1),
     1                     AA(1,1),AA(2,1),AA(1,2),
     2                     XDU55(1,1),XDU55(1,2),XDU55(2,1), DDUM  )
          CALL UCOPY(CC(1,1,JS),SS(1,1,JS),50)
          CALL DMMLA(5,5,5, AA(1,1),AA(1,2),AA(2,1),
     1                     XDU55(1,1),XDU55(1,2),XDU55(2,1),
     2                     SS(1,1,JS),SS(1,2,JS),SS(2,1,JS) )
          CALL DVSUB(5,CSIX(1,JS+1),CSIX(2,JS+1),
     1                 CSIY(1,JS+1),CSIY(2,JS+1),
     2                 YDUM(1),YDUM(2))
          CALL DMMPA(5,5,AA(1,1),AA(1,2),AA(2,1),
     1                  YDUM(1),YDUM(2),
     2                  CSIX(1,JS),CSIX(2,JS) )
*
          DO I=1,5
            IF (SS(I,I,JS).LE.DZERO) THEN
              IFAFT = 1
              SS(I,I,JS) = DBIG
            ENDIF
          ENDDO
*                    chi2
          DO J = 1,5
            DO K=1,5
              CHI2M = CHI2M +
     +           (CSIX(K,JS)-CSIZ(K,JS))*VV(K,J,JS)*
     +           (CSIX(J,JS)-CSIZ(J,JS))
              CHI2T = CHI2T +
     +           (CSIX(K,JS)-CSIY(K,JS))*WW(K,J,JS)*
     +           (CSIX(J,JS)-CSIY(J,JS))
            ENDDO
          ENDDO
*
        ENDDO
        CHI2N = CHI2T + CHI2M
c        print *,' chi2s ',iter,chi2,chi2n,chi2t,chi2m
c        IF ( IDEBUG.GE.1 )  THEN
c          WRITE(6,'(I6,''     CSIZ '',/(3X,6E15.5))')ITER,CSIZ
c          WRITE(6,'(I6,''     CSIX '',/(3X,6E15.5))')ITER,CSIX
c          WRITE(6,'(I6,''     CSIY '',/(3X,6E15.5))')ITER,CSIY
c        ENDIF
*
          NDOF = 2*npln-5
          pmom=1./csix(1,0)
          delta=abs(pmom-pold)/abs(pmom)
          pold=pmom
          epsmy=1.e-2
          if(delta.le.epsmy)then
           ifaft=0
           goto 999
          endif
          nitermy=20
          if(iter.ge.nitermy)then
           ifail=2
           goto 999
          endif
*
*
101   CONTINUE
*
**      done
*
999   CONTINUE
*
       ipaold=ityp
       ipart=ipo
      END
CDECK  ID>, TKERMT. 
             function tkermt(pred,meas,sig,ims,xms,cc,aa,bb)
              parameter (maxpl=10)
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

CDECK  ID>, ERPROP. 
*CMZ :  3.21/02 29/03/94  15.41.49  by  S.Giani
*-- Author :
      SUBROUTINE ERPROP
C.
C.    ******************************************************************
C.    *                                                                *
C.    *       Performs the error propagation in a step                 *
C.    *                                                                *
C.    *    ==>Called by : ERTRCH / ERTRNT                              *
C.    *       Author    E.Nagy  *********                              *
C.    *                                                                *
C.    ******************************************************************
C.
      COMMON/GCTMED/NUMED,NATMED(5),ISVOL,IFIELD,FIELDM,TMAXFD,STEMAX
     +      ,DEEMAX,EPSIL,STMIN,CFIELD,PREC,IUPD,ISTPAR,NUMOLD
      COMMON/GCTLIT/THRIND,PMIN,DP,DNDL,JMIN,ITCKOV,IMCKOV,NPCKOV
C
      INTEGER       NUMED,NATMED,ISVOL,IFIELD,IUPD,ISTPAR,NUMOLD
      REAL          FIELDM,TMAXFD,STEMAX,DEEMAX,EPSIL,STMIN,CFIELD,PREC
      INTEGER       JMIN,NPCKOV,IMCKOV,ITCKOV
      REAL          THRIND,PMIN,DP,DNDL
C
      COMMON/GCKINE/IKINE,PKINE(10),ITRA,ISTAK,IVERT,IPART,ITRTYP
     +      ,NAPART(5),AMASS,CHARGE,TLIFE,VERT(3),PVERT(4),IPAOLD
C
      INTEGER       IKINE,ITRA,ISTAK,IVERT,IPART,ITRTYP,NAPART,IPAOLD
      REAL          PKINE,AMASS,CHARGE,TLIFE,VERT,PVERT
C
      INTEGER NMEC,LMEC,NAMEC,NSTEP ,MAXNST,IGNEXT,INWVOL,ISTOP,MAXMEC
     + ,IGAUTO,IEKBIN,ILOSL, IMULL,INGOTO,NLDOWN,NLEVIN,NLVSAV,ISTORY
     + ,MAXME1,NAMEC1
      REAL  VECT,GETOT,GEKIN,VOUT,DESTEP,DESTEL,SAFETY,SLENG ,STEP
     + ,SNEXT,SFIELD,TOFG  ,GEKRAT,UPWGHT
      REAL POLAR
      PARAMETER (MAXMEC=30)
      COMMON/GCTRAK/VECT(7),GETOT,GEKIN,VOUT(7),NMEC,LMEC(MAXMEC)
     + ,NAMEC(MAXMEC),NSTEP ,MAXNST,DESTEP,DESTEL,SAFETY,SLENG
     + ,STEP  ,SNEXT ,SFIELD,TOFG  ,GEKRAT,UPWGHT,IGNEXT,INWVOL
     + ,ISTOP ,IGAUTO,IEKBIN, ILOSL, IMULL,INGOTO,NLDOWN,NLEVIN
     + ,NLVSAV,ISTORY
      PARAMETER (MAXME1=30)
      COMMON/GCTPOL/POLAR(3), NAMEC1(MAXME1)
C
      COMMON/GCUNIT/LIN,LOUT,NUNITS,LUNITS(5)
      INTEGER LIN,LOUT,NUNITS,LUNITS
      COMMON/GCMAIL/CHMAIL
      CHARACTER*132 CHMAIL
C
      DOUBLE PRECISION EI, EF, ASDSC
      COMMON /ERWORK/ EI(15), EF(15), ASDSC(5,5),
     +                   XI(3), PPI(3), HI(9),
     +                   XF(3), PF(3),  HF(9),
     +                   CHTR, DEDX2, BACKTR, CUTEK, TLGCM2, TLRAD
      INTEGER          MXPRED
      PARAMETER (MXPRED = 150)
      DOUBLE PRECISION ERDTRP
      REAL             ERRIN, ERROUT, ERTRSP, ERXIN, ERXOUT, ERPIN,
     +                 ERPOUT
      INTEGER          NEPRED, INLIST, ILPRED, IEPRED
      COMMON /ERTRIO/  ERDTRP(5,5,MXPRED), ERRIN(15), ERROUT(15,MXPRED),
     +                 ERTRSP(5,5,MXPRED), ERXIN( 3), ERXOUT( 3,MXPRED),
     +                 ERPIN(3), ERPOUT(3,MXPRED), NEPRED,INLIST,ILPRED,
     +                 IEPRED(MXPRED)
      CHARACTER*8     CHOPTI
      LOGICAL         LEEXAC, LELENG, LEONLY, LEPLAN, LEPOIN, LEVOLU
      REAL            ERPLI, ERPLO, ERLENG
      INTEGER         NAMEER, NUMVER, IOVLER
      COMMON /EROPTS/ ERPLI(3,2), ERPLO(3,4,MXPRED), ERLENG(MXPRED),
     +                NAMEER(MXPRED), NUMVER(MXPRED), IOVLER(MXPRED),
     +                LEEXAC, LELENG, LEONLY, LEPLAN, LEPOIN, LEVOLU
      COMMON /EROPTC/CHOPTI


      DOUBLE PRECISION A, B, S, TN, T, COSL, SINL, COSP, SINP, COSL1
      INTEGER          NEW
      COMMON /TRCOM3/ A(5,5), B(5,5), S(15),
     +                TN(3), T(5), COSL, SINL, COSP, SINP, COSL1, NEW

              common /tkgdefC/tmaxf,smax,fieldmm,cconv,
     +        maxcal,nitm,xtol,iver,ialg,steps,npars,ims,
     +        bound,scale,cfld,sms,pii
              real *4 tmaxf,smax,fieldmm,bound(2,6),cfld,pii,cconv,
     +        scale(6)
              common/tftimeC/timer(100,2),xtime
              common /tkcorrC/
     +        xxn(2),vpar2(2,2),vpar(2,2),vxy(2)
              real *8 xxn,vpar2,vpar,vxy
*

      DIMENSION DUM(15)
*
* *** Prepares the end-point
*
      DO 11 I = 1,3
         XF(I) = VECT(I)
         PF(I) = VECT(7)*VECT(3+I)
         HF(I) = 0.
   11 CONTINUE
*
      IF (IFIELD.EQ.3) THEN
         HF(3) = FIELDM
      ELSEIF (IFIELD.NE.0) THEN
         CALL GUFLD (VECT, HF)
      ENDIF
*
* *** Propagates the error (in SC-variables)
*
      MVAR  = 0
      IFLAG = 0
      ITRAN = 0
      IF (.NOT.LEEXAC) THEN
         CALL TRPROP (XI ,PPI, HI, XF, PF, HF, CHTR, STEP, DUM,
     &                MVAR, IFLAG, ITRAN, IERR)
      ELSE
         CALL TRPRFN (XI ,PPI, HI, XF, PF, HF, CHTR, STEP, DUM,
     &                MVAR, IFLAG, ITRAN, IERR)
      ENDIF
*
      IF(IERR.NE.0) THEN
         WRITE (LOUT, 778) IERR
         RETURN
      ENDIF
*
* *** Transport the matrix
*
      CALL SSMT5T (A, EF, EF)
*
      IF (CHARGE.NE.0.) THEN
*
* ***    Add multiple scattering
*
        if(ims.ne.0) CALL ERMCSC (EF)
*
* ***    Add fluctuation due to ionization to the error matrix
*          (and (later) due to bremsstrahlung and pair-production)
*
          aaa=1/vect(7)

          EF(1) = EF(1) + (GETOT/vect(7)*GETOT/vect(7)*DEDX2)*
     +     aaa**4
*
      ENDIF
*
* *** Copy variables for the next step
*
      CALL UCOPY (XF, XI, 3)
      CALL UCOPY (PF, PPI, 3)
      CALL UCOPY (HF, HI, 9)
*
*
  778 FORMAT(//4X,' *** Error in subr. TRPROP',I4,'  called by',
     &                'subr. ERPROP'//)
*
      END
CDECK  ID>, ERTRCH. 
*CMZ :  3.21/02 29/03/94  15.41.49  by  S.Giani
*-- Author :
      SUBROUTINE ERTRCH
C.
C.    ******************************************************************
C.    *                                                                *
C.    *    Average charged track is extrapolated by one step           *
C.    *                                                                *
C.    *    ==>Called by : ERTRGO                                       *
C.    *       Original routine : GTHADR                                *
C.    *       Authors   M.Maire, E.Nagy  *********                     *
C.    *                                                                *
C.    ******************************************************************
C.
      INTEGER IQ,LQ,NZEBRA,IXSTOR,IXDIV,IXCONS,LMAIN,LR1,JCG
      INTEGER KWBANK,KWWORK,IWS
      REAL GVERSN,ZVERSN,FENDQ,WS,Q
C
      PARAMETER (KWBANK=69000,KWWORK=5200)
      COMMON/GCBANK/NZEBRA,GVERSN,ZVERSN,IXSTOR,IXDIV,IXCONS,FENDQ(16)
     +             ,LMAIN,LR1,WS(KWBANK)
      DIMENSION IQ(2),Q(2),LQ(8000),IWS(2)
      EQUIVALENCE (Q(1),IQ(1),LQ(9)),(LQ(1),LMAIN),(IWS(1),WS(1))
      EQUIVALENCE (JCG,JGSTAT)
      INTEGER       JDIGI ,JDRAW ,JHEAD ,JHITS ,JKINE ,JMATE ,JPART
     +      ,JROTM ,JRUNG ,JSET  ,JSTAK ,JGSTAT,JTMED ,JTRACK,JVERTX
     +      ,JVOLUM,JXYZ  ,JGPAR ,JGPAR2,JSKLT
C
      COMMON/GCLINK/JDIGI ,JDRAW ,JHEAD ,JHITS ,JKINE ,JMATE ,JPART
     +      ,JROTM ,JRUNG ,JSET  ,JSTAK ,JGSTAT,JTMED ,JTRACK,JVERTX
     +      ,JVOLUM,JXYZ  ,JGPAR ,JGPAR2,JSKLT
C
      REAL          PI,TWOPI,PIBY2,DEGRAD,RADDEG,CLIGHT,BIG,EMASS
      REAL          EMMU,PMASS,AVO
C
      COMMON/GCONST/PI,TWOPI,PIBY2,DEGRAD,RADDEG,CLIGHT,BIG,EMASS
      COMMON/GCONSX/EMMU,PMASS,AVO
C
      COMMON/GCCUTS/CUTGAM,CUTELE,CUTNEU,CUTHAD,CUTMUO,BCUTE,BCUTM
     +             ,DCUTE ,DCUTM ,PPCUTM,TOFMAX,GCUTS(5)
C
      REAL          CUTGAM,CUTELE,CUTNEU,CUTHAD,CUTMUO,BCUTE,BCUTM
     +             ,DCUTE ,DCUTM ,PPCUTM,TOFMAX,GCUTS
C
      COMMON/GCPHYS/IPAIR,SPAIR,SLPAIR,ZINTPA,STEPPA
     +             ,ICOMP,SCOMP,SLCOMP,ZINTCO,STEPCO
     +             ,IPHOT,SPHOT,SLPHOT,ZINTPH,STEPPH
     +             ,IPFIS,SPFIS,SLPFIS,ZINTPF,STEPPF
     +             ,IDRAY,SDRAY,SLDRAY,ZINTDR,STEPDR
     +             ,IANNI,SANNI,SLANNI,ZINTAN,STEPAN
     +             ,IBREM,SBREM,SLBREM,ZINTBR,STEPBR
     +             ,IHADR,SHADR,SLHADR,ZINTHA,STEPHA
     +             ,IMUNU,SMUNU,SLMUNU,ZINTMU,STEPMU
     +             ,IDCAY,SDCAY,SLIFE ,SUMLIF,DPHYS1
     +             ,ILOSS,SLOSS,SOLOSS,STLOSS,DPHYS2
     +             ,IMULS,SMULS,SOMULS,STMULS,DPHYS3
     +             ,IRAYL,SRAYL,SLRAYL,ZINTRA,STEPRA
      COMMON/GCPHLT/ILABS,SLABS,SLLABS,ZINTLA,STEPLA
     +             ,ISYNC
     +             ,ISTRA
*
      INTEGER IPAIR,ICOMP,IPHOT,IPFIS,IDRAY,IANNI,IBREM,IHADR,IMUNU
     +       ,IDCAY,ILOSS,IMULS,IRAYL,ILABS,ISYNC,ISTRA
      REAL    SPAIR,SLPAIR,ZINTPA,STEPPA,SCOMP,SLCOMP,ZINTCO,STEPCO
     +       ,SPHOT,SLPHOT,ZINTPH,STEPPH,SPFIS,SLPFIS,ZINTPF,STEPPF
     +       ,SDRAY,SLDRAY,ZINTDR,STEPDR,SANNI,SLANNI,ZINTAN,STEPAN
     +       ,SBREM,SLBREM,ZINTBR,STEPBR,SHADR,SLHADR,ZINTHA,STEPHA
     +       ,SMUNU,SLMUNU,ZINTMU,STEPMU,SDCAY,SLIFE ,SUMLIF,DPHYS1
     +       ,SLOSS,SOLOSS,STLOSS,DPHYS2,SMULS,SOMULS,STMULS,DPHYS3
     +       ,SRAYL,SLRAYL,ZINTRA,STEPRA,SLABS,SLLABS,ZINTLA,STEPLA
C
      COMMON/GCJLOC/NJLOC(2),JTM,JMA,JLOSS,JPROB,JMIXT,JPHOT,JANNI
     +                  ,JCOMP,JBREM,JPAIR,JDRAY,JPFIS,JMUNU,JRAYL
     +                  ,JMULOF,JCOEF,JRANG
C
      INTEGER       NJLOC   ,JTM,JMA,JLOSS,JPROB,JMIXT,JPHOT,JANNI
     +                  ,JCOMP,JBREM,JPAIR,JDRAY,JPFIS,JMUNU,JRAYL
     +                  ,JMULOF,JCOEF,JRANG
C
      COMMON/GCJLCK/NJLCK(2),JTCKOV,JABSCO,JEFFIC,JINDEX,JCURIN
     +                      ,JPOLAR,JTSTRA,JTSTCO,JTSTEN,JTASHO
C
      EQUIVALENCE (JLASTV,JTSTEN)
C
      INTEGER       NJLCK,JTCKOV,JABSCO,JEFFIC,JINDEX,JCURIN
     +                   ,JPOLAR,JLASTV,JTSTRA,JTSTCO,JTSTEN
     +                   ,JTASHO
C
      COMMON/GCKINE/IKINE,PKINE(10),ITRA,ISTAK,IVERT,IPART,ITRTYP
     +      ,NAPART(5),AMASS,CHARGE,TLIFE,VERT(3),PVERT(4),IPAOLD
C
      INTEGER       IKINE,ITRA,ISTAK,IVERT,IPART,ITRTYP,NAPART,IPAOLD
      REAL          PKINE,AMASS,CHARGE,TLIFE,VERT,PVERT
C
      COMMON/GCMATE/NMAT,NAMATE(5),A,Z,DENS,RADL,ABSL
C
      INTEGER NMAT,NAMATE
      REAL A,Z,DENS,RADL,ABSL
C
      COMMON/GCMULO/SINMUL(101),COSMUL(101),SQRMUL(101),OMCMOL,CHCMOL
     +  ,EKMIN,EKMAX,NEKBIN,NEK1,EKINV,GEKA,GEKB,EKBIN(200),ELOW(200)
C
      REAL SINMUL,COSMUL,SQRMUL,OMCMOL,CHCMOL,EKMIN,EKMAX,ELOW,EKINV
      REAL GEKA,GEKB,EKBIN
      INTEGER NEKBIN,NEK1
C
      COMMON/GCTMED/NUMED,NATMED(5),ISVOL,IFIELD,FIELDM,TMAXFD,STEMAX
     +      ,DEEMAX,EPSIL,STMIN,CFIELD,PREC,IUPD,ISTPAR,NUMOLD
      COMMON/GCTLIT/THRIND,PMIN,DP,DNDL,JMIN,ITCKOV,IMCKOV,NPCKOV
C
      INTEGER       NUMED,NATMED,ISVOL,IFIELD,IUPD,ISTPAR,NUMOLD
      REAL          FIELDM,TMAXFD,STEMAX,DEEMAX,EPSIL,STMIN,CFIELD,PREC
      INTEGER       JMIN,NPCKOV,IMCKOV,ITCKOV
      REAL          THRIND,PMIN,DP,DNDL
C
      INTEGER NMEC,LMEC,NAMEC,NSTEP ,MAXNST,IGNEXT,INWVOL,ISTOP,MAXMEC
     + ,IGAUTO,IEKBIN,ILOSL, IMULL,INGOTO,NLDOWN,NLEVIN,NLVSAV,ISTORY
     + ,MAXME1,NAMEC1
      REAL  VECT,GETOT,GEKIN,VOUT,DESTEP,DESTEL,SAFETY,SLENG ,STEP
     + ,SNEXT,SFIELD,TOFG  ,GEKRAT,UPWGHT
      REAL POLAR
      PARAMETER (MAXMEC=30)
      COMMON/GCTRAK/VECT(7),GETOT,GEKIN,VOUT(7),NMEC,LMEC(MAXMEC)
     + ,NAMEC(MAXMEC),NSTEP ,MAXNST,DESTEP,DESTEL,SAFETY,SLENG
     + ,STEP  ,SNEXT ,SFIELD,TOFG  ,GEKRAT,UPWGHT,IGNEXT,INWVOL
     + ,ISTOP ,IGAUTO,IEKBIN, ILOSL, IMULL,INGOTO,NLDOWN,NLEVIN
     + ,NLVSAV,ISTORY
      PARAMETER (MAXME1=30)
      COMMON/GCTPOL/POLAR(3), NAMEC1(MAXME1)
C
      COMMON/GCUNIT/LIN,LOUT,NUNITS,LUNITS(5)
      INTEGER LIN,LOUT,NUNITS,LUNITS
      COMMON/GCMAIL/CHMAIL
      CHARACTER*132 CHMAIL
C
      INTEGER          MXPRED
      PARAMETER (MXPRED = 150)
      DOUBLE PRECISION ERDTRP
      REAL             ERRIN, ERROUT, ERTRSP, ERXIN, ERXOUT, ERPIN,
     +                 ERPOUT
      INTEGER          NEPRED, INLIST, ILPRED, IEPRED
      COMMON /ERTRIO/  ERDTRP(5,5,MXPRED), ERRIN(15), ERROUT(15,MXPRED),
     +                 ERTRSP(5,5,MXPRED), ERXIN( 3), ERXOUT( 3,MXPRED),
     +                 ERPIN(3), ERPOUT(3,MXPRED), NEPRED,INLIST,ILPRED,
     +                 IEPRED(MXPRED)
      CHARACTER*8     CHOPTI
      LOGICAL         LEEXAC, LELENG, LEONLY, LEPLAN, LEPOIN, LEVOLU
      REAL            ERPLI, ERPLO, ERLENG
      INTEGER         NAMEER, NUMVER, IOVLER
      COMMON /EROPTS/ ERPLI(3,2), ERPLO(3,4,MXPRED), ERLENG(MXPRED),
     +                NAMEER(MXPRED), NUMVER(MXPRED), IOVLER(MXPRED),
     +                LEEXAC, LELENG, LEONLY, LEPLAN, LEPOIN, LEVOLU
      COMMON /EROPTC/CHOPTI


      DOUBLE PRECISION EI, EF, ASDSC
      COMMON /ERWORK/ EI(15), EF(15), ASDSC(5,5),
     +                   XI(3), PPI(3), HI(9),
     +                   XF(3), PF(3),  HF(9),
     +                   CHTR, DEDX2, BACKTR, CUTEK, TLGCM2, TLRAD
*
      PARAMETER (EPSMAC=5.E-6)
      DOUBLE PRECISION GKR,DEMEAN,STOPP1,STOPP2,STOPMX,STOPRG,STOPC
      DOUBLE PRECISION EKIPR
      REAL VNEXT(6)
      SAVE CFLD,CHARG2,RMASS,CUTPRO,IKCUT,STOPC
C.
C.    ------------------------------------------------------------------
*
*
* *** Update local pointers if medium has changed
*
      IF (IUPD.EQ.0) THEN
         IUPD  = 1
         CHARG2 = CHARGE*CHARGE
         IF (IPART.LE.3) THEN
            CUTEK  = CUTELE
            RMASS  = 1.
            JRANG  = LQ(JMA-15)
         ELSE IF (IPART.LE.6) THEN
            CUTEK  = CUTMUO
            RMASS  = 1.
            JRANG  = LQ(JMA-16)
         ELSE
            CUTEK  = CUTHAD
            RMASS  = PMASS/AMASS
            JRANG  = LQ(JMA-16) + NEK1
         ENDIF
         CUTPRO = MAX(CUTEK*RMASS,ELOW(1))
         IKCUT = GEKA*LOG10(CUTPRO) + GEKB
         GKR   = (CUTPRO - ELOW(IKCUT))/(ELOW(IKCUT+1) - ELOW(IKCUT))
         STOPC = (1.-GKR)*Q(JRANG+IKCUT) + GKR*Q(JRANG+IKCUT+1)
         CFLD  = 0.
         IF (FIELDM.NE.0.) CFLD = 3333.*DEGRAD*TMAXFD/ABS(FIELDM*CHARGE)
      ENDIF
*
* *** Compute current step size
*
      STEP   = BIG
      GEKRT1 = 1. - GEKRAT
*
* *** Step limitation due to energy loss (stopping range) ?
*
      IF (ILOSS*DEEMAX.GT.0.) THEN
c         STOPP1 = GEKRT1*Q(JRANG+IEKBIN) + GEKRAT*Q(JRANG+IEKBIN+1)
c         STOPMX = (STOPP1 - STOPC)/(RMASS*CHARG2)
c         EKF  = (1. - BACKTR*DEEMAX)*GEKIN*RMASS
c         IF (EKF.LT.ELOW(1))    EKF = ELOW(1)
c         IF (EKF.GE.ELOW(NEK1)) EKF = ELOW(NEK1)*0.99
c         IKF=GEKA*LOG10(EKF)+GEKB
c         GKR=(EKF-ELOW(IKF))/(ELOW(IKF+1)-ELOW(IKF))
c         STOPP2 = (1.-GKR)*Q(JRANG+IKF) + GKR*Q(JRANG+IKF+1)
c         SLOSP  = ABS (STOPP1 - STOPP2)
c         STEP   = SLOSP/(RMASS*CHARG2)
      ENDIF
*
* *** Step limitation due to energy loss in magnetic field ?
*
      IF (IFIELD*FIELDM.NE.0.) THEN
         SFIELD = CFLD*VECT(7)
         IF (SFIELD.LT.STEP) STEP = SFIELD
      ENDIF
*
* *** Compute point where to store error matrix
*
      LERST  = 0
      STEPER = BIG
      ASCL1  = BIG
      DO 20 IPR = 1,NEPRED
         STEPE  = BIG
         IF (LELENG) STEPE = ERLENG(IPR) - SLENG
         IF (LEPLAN) THEN
            SCAL1 = 0.
            SCAL2 = 0.
            DO 18 I=1,3
               SCAL1 = SCAL1 + ERPLO(I,4,IPR)*(ERPLO(I,3,IPR)-VECT(I))
               SCAL2 = SCAL2 + ERPLO(I,4,IPR)*VECT(I+3)
   18       CONTINUE
            STEPE = SCAL1/SCAL2
         ENDIF
         IF (STEPE.LE.PREC) STEPE = BIG
         IF (STEPE.LT.STEPER) THEN
            STEPER = STEPE
            INLIST = IPR
            IF (LEPLAN) ASCL1  = ABS (SCAL1)
         ENDIF
   20 CONTINUE
      IF (STEPER.LE.STEP)  THEN
         STEP  = STEPER
         LERST = 1
      ENDIF
*
* *** Step limitation due to geometry ?
*
      LNEXT  = 0
      IF (STEP.GE.0.95*SAFETY) THEN
         CALL GTNEXT
         IF (IGNEXT.NE.0) THEN
            STEP   = SNEXT + PREC
            LNEXT = 1
            IF ((STEPER-SNEXT).GT.(2*PREC)) LERST = 0
         ENDIF
      ENDIF
*
* *** Linear transport when no field or very short step
*
      IF (IFIELD.EQ.0.OR.STEP.LE.2*PREC) THEN
        IF (IGNEXT.NE.0) THEN
          DO 25 I = 1,3
            VECTMP = VECT(I) +STEP*VECT(I+3)
            IF(VECTMP.EQ.VECT(I)) THEN
*
* *** Correct for machine precision
*
                  IF(VECT(I+3).NE.0.) THEN
                     VECTMP =
     +               VECT(I)+ABS(VECT(I))*SIGN(1.,VECT(I+3))*EPSMAC
                  ENDIF
            ENDIF
            VOUT(I) = VECTMP
   25     CONTINUE
            INWVOL = 2
            NMEC = NMEC +1
            LMEC(NMEC) = 1
        ELSE
            DO 30 I = 1,3
               VOUT(I)  = VECT(I) +STEP*VECT(I+3)
   30       CONTINUE
        ENDIF
        DO 35 I = 4,6
          VOUT(I)  = VECT(I)
   35   CONTINUE
      ELSE
*
* *** otherwise, swim particle in magnetic field
*
        NMEC = NMEC +1
        LMEC(NMEC) = 4
*
   50   LERST = 0
        LNEXT = 0
        CALL GUSWIM (CHTR , STEP, VECT, VOUT)
*
*     When near to boundary, take proper action (cut-step,crossing...)
        IF (STEP.GE.SAFETY) THEN
           INEAR = 0
           IF (IGNEXT.NE.0) THEN
             DO 51 I = 1,3
                 VNEXT(I+3) = VECT(I+3)
                 VNEXT(I) = VECT(I) +SNEXT*VECT(I+3)
   51        CONTINUE
             DO 52 I = 1,3
               IF (ABS(VOUT(I)-VNEXT(I)).GT.EPSIL) GO TO 55
   52        CONTINUE
             INEAR = 1
           ENDIF
*
   55      CALL GINVOL (VOUT,ISAME)
           IF (ISAME.EQ.0) THEN
             IF ((INEAR.NE.0).OR.(STEP.LT.EPSIL)) THEN
               INWVOL = 2
               NMEC = NMEC +1
               LMEC(NMEC) = 1
             ELSE
*              Cut step
               STEP = 0.5*STEP
               IF (LMEC(NMEC).NE.24) THEN
                 NMEC = NMEC +1
                 LMEC(NMEC) = 24
               ENDIF
               GO TO 50
             ENDIF
             LNEXT = 1
           ENDIF
        ENDIF
*
      ENDIF
*
*
*     preset plane reached  ?
      IF ((LEPLAN).AND.(STEP.GE.ASCL1)) THEN
         SCAL3 = 0.
         DO 28 I=1,3
            SCAL3=SCAL3+ERPLO(I,4,INLIST)*(ERPLO(I,3,INLIST)-VOUT(I))
   28    CONTINUE
         ASCL3 = ABS(SCAL3)
         SSCL1 = ASCL1/SCAL1
         IF (SCAL3*SSCL1.LT. -PREC) THEN
*            Cut step
             STEP  = STEP*(ASCL1/(ASCL1+ASCL3))
             NMEC  = NMEC +1
             LMEC(NMEC) = 24
             GO TO 50
         ELSE
           IF(ASCL3.LE.PREC) LERST = 1
         ENDIF
      ENDIF
*
        DO 75 I=1,6
           VECT(I) = VOUT(I)
   75   CONTINUE
*
      IF (LELENG.AND.(STEP.GE.STEPER)) LERST = 1
*
      SLENG = SLENG + STEP
*
* *** Now apply selected mechanisms
*
      IF (LNEXT.EQ.1) THEN
          INWVOL = 2
          NMEC = NMEC + 1
          LMEC(NMEC) = 1
      ENDIF
*
* *** apply energy loss : find the kinetic energy corresponding
*      to the new stopping range = stopmx -/+ step
*      (take care of the back tracking !)
*
      IF (ILOSS*DEEMAX.GT.0) THEN
c         NMEC = NMEC +1
c         LMEC(NMEC) = 3
c         CALL ERLAND (STEP,Z,A,DENS,VECT(7),GETOT,AMASS,DEDX2)
c         DEDX2  = DEDX2*CHARG2*CHARG2
c         STOPRG = STOPP1 - BACKTR*STEP*RMASS*CHARG2
c         IKF = IEKBIN
c         IF (BACKTR.LE.0.) THEN
c   95       IF (STOPRG.LT.Q(JRANG+IKF)) THEN
c               IKF = IKF - 1
c               IF (IKF.GT.1) GO TO 95
c            ENDIF
c         ELSE
c   96       IF (STOPRG.GE.Q(JRANG+IKF+1)) THEN
c               IKF = IKF + 1
c               IF (IKF.LT.NEK1) GO TO 96
c            ENDIF
c         ENDIF
c         GKR = (STOPRG - Q(JRANG+IKF)) / (Q(JRANG+IKF+1) - Q(JRANG+IKF))
c         EKIPR = (1. -GKR)*ELOW(IKF) + GKR*ELOW(IKF+1)
c         GEKINT =  EKIPR/RMASS
c         IF (GEKINT.GT.CUTEK) THEN
c            DESTEP = ABS (GEKIN - GEKINT)
c            GEKIN  = GEKINT
c            GETOT  = GEKIN + AMASS
c            VECT(7)= SQRT((GETOT+AMASS)*GEKIN)
c            CALL GEKBIN
c         ELSE
c            DESTEP = GEKINT
c            GEKIN  = 0.
c            GETOT  = AMASS
c            VECT(7)= 0.
c            INWVOL = 0
c            ISTOP  = 2
c            NMEC = NMEC + 1
c            LMEC(NMEC) = 30
c         ENDIF
      ENDIF
*
* *** Propagate error matrix
*
      IF (.NOT. LEONLY) CALL ERPROP
*
* *** Store informations
*
      IF(LERST.EQ.1) THEN
         NMEC = NMEC + 1
         LMEC(NMEC) = 27
         CALL ERSTOR
      ENDIF
*
      END

CDECK  ID>, DUMMY.  
      SUBROUTINE QNEXT
      END
      subroutine npq
      common /quest/iquest(100)
      iquest(10)=64000
      end

      subroutine abinelset(acoo,ipart)
         implicit none
         real acoo(7),coo(7)
         common /abinelseti/iset,coo
         integer iset,i,ipart
         data iset/0/
         if(iset.ne.0)then
c           write(*,*)'abinelsetlogicerroralreadyset'
         else
         do i=1,7
          coo(i)=acoo(i)
         enddo
         endif
         iset=ipart
c         iset=iset-1
       end
       subroutine abinelget(isett,acoo)
         implicit none
         integer iset,isett,i
         real coo(7),acoo(7)
         common /abinelseti/iset,coo
         isett=iset
         do i=1,7
          acoo(i)=coo(i)
         enddo
       end         
       subroutine abinelclear
         implicit none
         integer iset,isett,i
         real coo(7),acoo(7)
         common /abinelseti/iset,coo
         do i=1,7
          coo(i)=0
         enddo
         iset=0
       end         
