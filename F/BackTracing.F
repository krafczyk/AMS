      subroutine MIB_ascoor

!GLV        

******************************************************************************
C VERSION  WITH  SUPPLEMENT SPHERICAL HARMONIC COEFFICIENTS OF THE 1995-2020
C REVISION OF THE INTERNATIONAL GEOMAGNETIC REFERENCE  FIELD 
C                                 IEP SAS Kosice
c				  Watsonova 47
c				  043 53 Kosice
c				  Slovakia
C
c    Calculation trajectory of particle by program T96_01 or T05
c    for years from 1945 to 2020 e.g.<1945,2020> 
c*****************************************************************************
c
C				REFERENCES:
C
C	J. Kassovicova, K. Kudela: On the Compuations of Cosmic Ray 
C	Trajectories in the Geomagnetic Field, Preprint DSP IEP SAS, 
C	Kosice, May 1998, pp.24.
C
C	P. Bobik, G. Boella, M. J. Boschini, D. Grandi, M. Gervasi, K. Kudela
C	S. Pensotti and P. G. Rancoita : Magnetospheric Transmission Function	
C	Approach to disantangle Primary from Secondary Cosmic Rays in the Penumbra
C	Region. 2006, J. Geophys. Res., 111, A05205
C
C
c*****************************************************************************
c
c	Modified March 2015 by 
C
c	Davide Grandi.
c	INFN Milano Bicocca
C	P.zza delle Scienze 3-20126 Milano
c	ITALY
C
c	GEOPACK 2008 dp
C	
C------------------------------------------------------------------------------

      IMPLICIT REAL*8 (A-H,O-Z)
c	setting all quantities in double precision...	    

      REAL*8 rig,rigidity,amen,zn
      REAL*8 hm0,hm1,c,q,hm,pi,rad,Re
      REAL*8 time,time1,alength,a1length
      REAL*8 frig,deltarig,rcut,er,gs
      REAL*8 diff,reldiff,r0,the0,fi0
      REAL*8 the1,fi1,pax,paz,paus
      REAL*8 ast,asf,r,a,td,fei,gu
      REAL*8 gmlong,distd,gmlat,tt,h
      REAL*8 el,al,alp,elp,alc,elc
      REAL*8 vc,vv,b,d,t,v,vva,tu,gc
      REAL*8 alf,kap,rmin
      REAL*8 hmn,hmp,hme,amass
      REAL*8 w1,w2,w3,w4,w5,w6,ext
      REAL*8 pdyn,dst,byimf,bzimf
      REAL*8 shue,mpr0,alfa,paxz,cteta,bzeta
      REAL*8 astar,bstar,cstar   
      
      REAL*8 a1,b1,c1,a2,b2,c2,a3,b3,c3
      REAL*8 a4,b4,c4,a5,b5,c5,a6,b6,c6
      REAL*8 aa1,bb1,cc1
      REAL*8 aa2,bb2,cc2,aa3,bb3,cc3,aa4,bb4,cc4
      REAL*8 vx,vy,vz,bx,by,bz,x,y,z  
      
      REAL*8 VGSEX,VGSEY,VGSEZ
       
      !integer i,jj,j,kk,MM,NN
      !integer ios,nr,nl,il
      !integer maxrecs,np1,np2,i1,i2
      
c  define function for code RUNGE -KUTTA 
      REAL*8 FNX,FNY,FNZ
      FNX(y,z)=d*(y*bz-z*by)			
      FNY(x,z)=d*(z*bx-x*bz)
      FNZ(x,y)=d*(x*by-y*bx)
       
!---COMMONS for AMS-----------------------------------------------
c     solar variables
      REAL*8 PAR(11)
      !integer iy,mes,ide,id,ih,min,is,nk1,iopt,ist
      !real*8 r0,the0,fi0,the1,fi1,rig,zn,amass,dflag,rkflag
      common/backtracingpar/PAR,r0,the0,fi0,the1,fi1,
     $     rig,zn,amass,dflag,rkflag,
     $     res,r,td,fei,ast,asf,time,alast,
     $     iy,mes,ide,id,ih,min,is
!$OMP THREADPRIVATE (/backtracingpar/)
!-------------------------------------------------------------------
       
!---COMMONS------------------------------------------------------
c     solar variables
c      REAL*8 PAR(11)
c      !integer iy,mes,ide,id,ih,min,is,nk1,iopt,ist
c      !real*8 r0,the0,fi0,the1,fi1
c      common/input/PAR,r0,the0,fi0,the1,fi1,
c     $     iy,mes,ide,id,ih,min,is
c
c      !REAL*8 res,r,td,fei,ast,asf,time,alast
c      common/output/res,r,td,fei,ast,asf,time,alast
!-------------------------------------------------------------------

      !dflag = 0
      !rkflag = 6

      nk1=100
      iopt=1
      ist=1

      pdyn  = PAR(1)
      dst   = PAR(2)
      byimf = PAR(3)
      bzimf = PAR(4)
      
      bzeta = PAR(4)
      
      w1 = PAR(5)
      w2 = PAR(6)
      w3 = PAR(7)
      w4 = PAR(8)
      w5 = PAR(9)
      w6 = PAR(10)
      
      ext = PAR(11)
		
c   Using constants
      pi=3.141592654
      rad=pi/180.
      q=1.6021e-19		!electron charge in coul.
      c=2.99725e+08		!speed of light in m/sec
      hmp=1.6725e-27		!proton mass in kg
      hme=9.10938188e-31	!electron mass in kg
      hmn=1.65998e-27		!nucleon mass in kg       
      Re=6.3712e+06		!earth radius in m
      nkrat=1  	! only for optimalization when we calculate EXT field
                ! only every n steps
			
      VGSEX = -400.0            !default value for SW
      VGSEY = 0.0
      VGSEZ = 0.0	
       
c  Input data: spherical coordinates of starting point
c	       year,month,day,day(in year),time
c	       starting step,iopt,ist=1
c              read point r0,the0,fi0
c              read asymptotic direction the1,fi1            
c              read time, year etc. iy,mes,ide,id,ih,min,is
c	       read ext model
c	       0 is only INT, 1 is T96, 2 is T05
c	       dflag is set to 0 for default exit when new particle is traced..
c	       rkflag is for RK version 4 or 6

c     kind of part as function of amass			
      if(amass.eq.1.) then
	 hm0 = hmp
c	 write(6,*) rig,' PROTON'			!protons
      elseif (amass.eq.0) then
	 hm0 = hme		
c	 write(6,*) rig,' ELECTRON'			!electrons
      elseif (amass.gt.1) then
	 hm0 = amass*hmn		
c	 write(6,*) rig,' ION'				!ions	
      endif		
			
c     defining initial value of variables	 
      time=0.
      time1=0.
      alength=0.
      a1length=0.
      tu=0.001                  !angular precision
        		
c     defining the output first output is set at 2 

      res=6.      ! Putting 6 as random number differnt from 0 or 1
      cicle=6.    ! Putting 6 as random number differnt from 0 or 1
      changeback=0. 
       
      asf = 0
      ast = 0
      td = 0
      fei = 0
       
      awrite = 0

     
       if(ext.ne.0) then
 
c	check missing data both on T96 par and T05 par!

c	write(6,*) ext,pdyn,byimf,bzimf,dst,w1,w2,w3,w4,w5,w6
c        write(6,*) r0,the0,fi0,the1,fi1,
c     $     rig,zn,amass,dflag,rkflag,
c     $     iy,mes,ide,id,ih,min,is

	if(pdyn.gt.99.98.or.byimf.gt.999.98.or.bzimf.gt.999.98) then
c	 write(6,*) 'NO PARAMETERS FOR T96'
	 cicle = 1
         awrite = 1
         res = 5
	elseif(ext.eq.2.and.(w1.gt.99.98.or.w2.gt.99.98.or.w3.gt.99.98.
     +	       or.w4.gt.99.98.or.w5.gt.99.98.or.w6.gt.99.98)) then
c	 write(6,*) 'NO PARAMETERS FOR T05'
	 cicle = 1
         awrite = 1
         res = 5
	endif
      

      if(dflag.eq.0.and.res.ne.5) then		!to avoid check of NON EXISTING PAR
	
	 if(pdyn.lt.0.5.or.pdyn.gt.10.) then
c	  write(6,*) 'PDYN OUT OF LIMIT'
            cicle = 1
            awrite = 1
            res = 4
	 else if(dst.lt.-100.or.dst.gt.20.) then
c	   write(6,*) 'DST OUT OF LIMIT'
            cicle = 1
            awrite = 1
            res = 4
	 else if(byimf.lt.-10.or.byimf.gt.10.) then
c	   write(6,*) 'BYIMF OUT OF LIMIT'
            cicle = 1
            awrite = 1
            res = 4
	 else if(bzimf.lt.-10.or.bzimf.gt.10.) then
c	  write(6,*) 'BZIMF OUT OF LIMIT'
            cicle = 1
            awrite = 1
            res = 4
	 endif
	
      endif	 			        

      endif

       
!RECALC_08 call SUN that calculate 
!parameters depending on time
      CALL RECALC_08(iy,id,ih,min,is,VGSEX,VGSEY,VGSEZ)	

      a=rig*1.0e+09*zn*q/(hm0*c*c) !to calculate velocity
      v=c*dsqrt(1.-1./(1.+a*a))    !beta=momentum/energy

c     here change for a faster program and better precision
c     decreasing lenght of step
        	
      nk1 = nk1*85              !BEST AT 10/2002 for backtracing

      nk0=nk1
       
      r=r0
      td=90.-the0               !calculate colatitude
      td1=90.-the1              !calculate asymptotic colatitude
      fei=fi0
      fei1=fi1
      th=td*rad
      th1=td1*rad
      f=fei*rad
      f1=fei1*rad  
      rr=r*Re  			!radius in km
      jj=0                      !first step set to zero
      rmin=r0-0.00001  

!     convert spherical to cartesian coord.
      CALL SPHCAR_08(rr,th,f,x,y,z,1)
!     from terrestrial GEO to solar GSW coord.
      CALL GEOGSW_08(x,y,z,xs,ys,zs,1)
	
c     solar coordinates in unit of earth radii
      xx=xs/Re
      yy=ys/Re
      zz=zs/Re
       
      CALL BTOT(par,xx,yy,zz,iopt,bxs,bys,bzs,ist)
!     from solar GSW to terrestrial GEO 
      CALL GEOGSW_08(bx,by,bz,bxs,bys,bzs,-1) 
        
      d=1.e-09*zn*q/hm0         !for runge-kutta

      b=1.e-09*dsqrt(bx*bx+by*by+bz*bz) !magnetic field

c     calculate velocity components

      vx=v*dsin(th1)*dcos(f1)
      vy=v*dsin(th1)*dsin(f1)
      vz=v*dcos(th1)
      vv=v

      DO WHILE (cicle.NE.1.)    ! START OF SECOND cicle

c     res=5.     ! Putting 5 as  number differnt from 0 or 1
c     cicle=5.   ! Putting 5 as  number differnt from 0 or 1 

         nk=nk0                 !first number of steps
        
c     Summation for total number of steps of a trajetory
         jj=jj+1                !increase num of steps
        
c     Remembering preceded data of coordinates and velocity
c     to be used to compute angle between two close velocity

         xp=x
         yp=y
         zp=z

         vxp=vx
         vyp=vy
         vzp=vz
       
         rp=r

         vc=vv/c    
                
         IF(vc.ge.1.) THEN
c         vc=0.9999991
            res=3.
            awrite=1.
            cicle=1.
         ENDIF
        	 
         hm=hm0/dsqrt(1.-vc*vc)	!relativistic proton mass
         t=2.*pi*hm/(ABS(zn)*q*b) !Larmor period (1 circle)
         d=1.e-09*zn*q/hm 	!for code of RUNGE -KUTTA
         v=vv
       
         h=t/nk

         if(rkflag.eq.4) then
c Code of RUNGE-KUTTA of 4-th order
            a1=h*FNX(vy,vz)
            b1=h*FNY(vx,vz)
            c1=h*FNZ(vx,vy)
            a2=h*FNX(vy+b1/2,vz+c1/2)
            b2=h*FNY(vx+a1/2,vz+c1/2)
            c2=h*FNZ(vx+a1/2,vy+b1/2)
            a3=h*FNX(vy+b2/2,vz+c2/2)	
            b3=h*FNY(vx+a2/2,vz+c2/2)
            c3=h*FNZ(vx+a2/2,vy+b2/2)
            a4=h*FNX(vy+b3,vz+c3)
            b4=h*FNY(vx+a3,vz+c3)
            c4=h*FNZ(vx+a3,vy+b3)
            
            aa1=h*vx
            bb1=h*vy
            cc1=h*vz
            aa2=h*(vx+aa1/2)
            bb2=h*(vy+bb1/2)
            cc2=h*(vz+cc1/2)
            aa3=h*(vx+aa2/2)
            bb3=h*(vy+bb2/2)
            cc3=h*(vz+cc2/2)
            aa4=h*(vx+aa3)
            bb4=h*(vy+bb3)
            cc4=h*(vz+cc3)

            vx=vx+((1/6.)*(a1+2*(a2+a3)+a4))
            vy=vy+((1/6.)*(b1+2*(b2+b3)+b4))
            vz=vz+((1/6.)*(c1+2*(c2+c3)+c4))
            
            x=x+((1/6.)*(aa1+2*(aa2+aa3)+aa4))
            y=y+((1/6.)*(bb1+2*(bb2+bb3)+bb4))
            z=z+((1/6.)*(cc1+2*(cc2+cc3)+cc4))

            vv=dsqrt(vx*vx+vy*vy+vz*vz)
	else 

c     Code of RUNGE -KUTTA of 6-th order
           a1=h*FNX(vy,vz)
           b1=h*FNY(vx,vz)
           c1=h*FNZ(vx,vy)
           a2=h*FNX(vy+b1/3.,vz+c1/3.)
           b2=h*FNY(vx+a1/3.,vz+c1/3.)
           c2=h*FNZ(vx+a1/3.,vy+b1/3.)
           a3=h*FNX(vy+(4.*b1+6.*b2)/25.,vz+(4.*c1+6.*c2)/25.)
           b3=h*FNY(vx+(4.*a1+6.*a2)/25.,vz+(4.*c1+6.*c2)/25.)
           c3=h*FNZ(vx+(4.*a1+6.*a2)/25.,vy+(4.*b1+6.*b2)/25.)
           a4=h*FNX(vy+(b1-12.*b2+15.*b3)/4.,vz+(c1-12.*c2+15.*c3)/4.)
           b4=h*FNY(vx+(a1-12.*a2+15.*a3)/4.,vz+(c1-12.*c2+15.*c3)/4.)
           c4=h*FNZ(vx+(a1-12.*a2+15.*a3)/4.,vy+(b1-12.*b2+15.*b3)/4.)
           a5=h*FNX(vy+(6.*b1+90.*b2-50.*b3+8.*b4)/81.,
     /          vz+(6.*c1+90.*c2-50.*c3+8.*c4)/81.)
           b5=h*FNY(vx+(6.*a1+90.*a2-50.*a3+8.*a4)/81.,
     /          vz+(6.*c1+90.*c2-50.*c3+8.*c4)/81.)
           c5=h*FNZ(vx+(6.*a1+90.*a2-50.*a3+8.*a4)/81.,
     /          vy+(6.*b1+90.*b2-50.*b3+8.*b4)/81.)
           a6=h*FNX(vy+(6.*b1+36.*b2+10.*b3+8.*b4)/75.,
     /          vz+(6.*c1+36.*c2+10.*c3+8.*c4)/75.)
           b6=h*FNY(vx+(6.*a1+36.*a2+10.*a3+8.*a4)/75.,
     /          vz+(6.*c1+36.*c2+10.*c3+8.*c4)/75.)
           c6=h*FNZ(vx+(6.*a1+36.*a2+10.*a3+8.*a4)/75.,
     /          vy+(6.*b1+36.*b2+10.*b3+8.*b4)/75.)
           
           aa1=h*vx
           bb1=h*vy
           cc1=h*vz
           aa2=h*(vx+aa1/3.)
           bb2=h*(vy+bb1/3.)
           cc2=h*(vz+cc1/3.)
           aa3=h*(vx+(4.*aa1+6.*aa2)/25.)
           bb3=h*(vy+(4.*bb1+6.*bb2)/25.)
           cc3=h*(vz+(4.*cc1+6.*cc2)/25.)
           aa4=h*(vx+(aa1-12.*aa2+15.*aa3)/4.)
           bb4=h*(vy+(bb1-12.*bb2+15.*bb3)/4.)
           cc4=h*(vz+(cc1-12.*cc2+15.*cc3)/4.)
           aa5=h*(vx+(6.*aa1+90.*aa2-50.*aa3+8.*aa4)/81.)
           bb5=h*(vy+(6.*bb1+90.*bb2-50.*bb3+8.*bb4)/81.)
           cc5=h*(vz+(6.*cc1+90.*cc2-50.*cc3+8.*cc4)/81.)
           aa6=h*(vx+(6.*aa1+36.*aa2+10.*aa3+8.*aa4)/75.)
           bb6=h*(vy+(6.*bb1+36.*bb2+10.*bb3+8.*bb4)/75.)
           cc6=h*(vz+(6.*cc1+36.*cc2+10.*cc3+8.*cc4)/75.)
           vx=vx+(23.*a1+125.*a3-81.*a5+125.*a6)/192.
           vy=vy+(23.*b1+125.*b3-81.*b5+125.*b6)/192.
           vz=vz+(23.*c1+125.*c3-81.*c5+125.*c6)/192.
           x=x+(23.*aa1+125.*aa3-81.*aa5+125.*aa6)/192.
           y=y+(23.*bb1+125.*bb3-81.*bb5+125.*bb6)/192.
           z=z+(23.*cc1+125.*cc3-81.*cc5+125.*cc6)/192.
       
           vv=dsqrt(vx*vx+vy*vy+vz*vz)
	endif

c     Testing of angle between a velocity and next velocity
        gc=(vx*vxp+vy*vyp+vz*vzp)/(vv*v)
       
        IF(gc.le.1.0) THEN
           gs=dsqrt((1.-gc)*(1.+gc))
           gu=datan2(gs,gc)
       
c     Regulation of length of step (by testing of angle of velocities)

           IF(gu.lt.tu .OR. nk0.gt.500000) THEN !limit of num steps 
              gu=gu                             !to get next case
           ELSE 	
              nk0=nk0*2
              changeback=1.
           ENDIF
        ENDIF  
       
        IF(nk0.le.0) THEN       !limit of num steps 
           cicle = 1            !to get next case
        ENDIF 	 
 
!     convert cartesian to spherical
        CALL SPHCAR_08(rr,th,f,x,y,z,-1) 

c     Equation of magnetosphere
!from terrestrial GEO to solar GSW here same as GSM for magpause
        CALL GEOGSW_08(x,y,z,xsp,ysp,zsp,1)	
	
c     Old one for IGRF (fixed)

	if(ext.eq.0) then
           pax=(xsp/Re+25.3)/36.08       
           paz=(ysp*ysp+zsp*zsp)/(Re*Re)
           paus=pax*pax+paz/459.6736

c	Here insert equation of magnetopause from Siebeck & al.(1991)
c	with Tsyganenko modification  

	else if(ext.eq.1) then
	        
	 alf=0.14
         kap=(pdyn/2)**alf
	 
	 
         astar = 65./kap
         bstar = 76./kap
         cstar = 808.7/kap
 
         pax=(xsp/Re+astar)/bstar
         paz=(ysp*ysp+zsp*zsp)/(Re*Re)
         paus = dsqrt(pax*pax+paz/cstar)
       
c	here insert equation of Shue (1997) for magpause

	else if (ext.eq.2) then
	 
           alf = -1/(6.6)
           alfa = (0.58 -0.010*bzeta)*(1+0.010*pdyn)
           if(bzeta.ge.0) then
              mpr0 = (11.4 + 0.013*bzeta)*(pdyn**alf) 
           else if(bzeta.lt.0) then
              mpr0 = (11.4 + 0.14*bzeta)*(pdyn**alf)
           endif

           pax = xsp*xsp/Re/Re
           paz = (ysp*ysp/Re/Re) + (zsp*zsp/Re/Re) 

           paxz = dsqrt(pax + paz)
           cteta = xsp/Re/paxz  !cos theta
		
           shue = mpr0*(2/(1+cteta))**alfa
	
           paus = paxz/shue
	
	endif
	
c
          
        r=rr/Re
        teh=th/rad
        fei=f/rad
        td=90.-teh              !from colatitude to latitude
        	
c     here you can select the constraints and uncomment those one needed

        IF(paus.GE.1.) THEN

           IF(abs(paus-1.).lt.0.002) THEN
              res=1.
              cicle=1.
           ELSE   
              nk0=nk0*2
              changeback=1.
           ENDIF                !(25 Earth radii)
        ELSEIF(r.GE.25.) THEN		
           IF(abs(r-25.).lt.0.002) THEN
              res=1.
              cicle=1.
           ELSE

              nk0=nk0*2

              changeback=1.
           ENDIF
        ENDIF                   !particles reaches the magnetosphere
        
        IF(r.lt.rmin) THEN      !radius lower than AMS orbit (here 400 km) 
           res=0.				
           cicle=1.
        ENDIF				

        IF(changeback.eq.0)THEN 	
!     from GEO terrestrial to GSW solar
           CALL GEOGSW_08(x,y,z,xs,ys,zs,1)
	
           xx=xs/Re
           yy=ys/Re
           zz=zs/Re

           CALL BTOT(par,xx,yy,zz,iopt,bxs,bys,bzs,ist)
!     from GSW solar to GEO terrestrial
           CALL GEOGSW_08(bx,by,bz,bxs,bys,bzs,-1) 

           b=1.e-09*dsqrt(bx*bx+by*by+bz*bz)

c	Here calculate the time spent by the particle in the trajectory
c	and the lenght of the path (by Pavol) in sec. and m.
       
           IF (cicle.ne.1) THEN  
              time1=(t/nk)
              time=time+time1

              a1length=dsqrt((x-xp)*(x-xp)+(y-yp)*(y-yp)+(z-zp)*(z-zp))
              alength=alength+a1length
	 
           ENDIF ! added to calculate time and lenght in REAL last point
       
C~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~	       
       
c	Calculate quantities for 'TRAJECTORY' retransform to colatitude 
c	Uncomment if needed
        
c	ro=r
c	tc=90.-td
c	to=tc*rad
c	fo=fei*rad
        
c	 CALL SPHCAR_08(ro,to,fo,xo,yo,zo,1)
        
c	Here write the output file 'cartesio', all trajectory points in 
c	cartesian coordinates. Uncomment if needed
        
c	 write(6,*) xo,yo,zo
c
C~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~


c	time limit

           IF(time.gt.10.0) THEN !set to 10 sec
              res=2.
              cicle=1.
           ENDIF


           nk0=nk1		!return to previous value
        ENDIF                   !changeback 0

c  arrive here from regulation of lenght of step

        IF(changeback.EQ.1.) THEN        
       
           x=xp
           y=yp
           z=zp
        
           vx=vxp
           vy=vyp
           vz=vzp
       
           CALL SPHCAR_08(rr,th,f,x,y,z,-1) !from cartesian to spherical
        
        
           CALL GEOGSW_08(x,y,z,xs,ys,zs,1) !from GEO terrestrial to GSW solar
        
           xx=xs/Re
           yy=ys/Re
           zz=zs/Re
       
           jj=jj-1              !decrease number of steps


           CALL BTOT(par,xx,yy,zz,iopt,bxs,bys,bzs,ist)
 
           CALL GEOGSW_08(bx,by,bz,bxs,bys,bzs,-1) !from GSW solar to terrestrial GEO
	 
           b=1.e-09*dsqrt(bx*bx+by*by+bz*bz)

           changeback=0. 


        ENDIF



      END DO                    ! END OF SECOND cicle
              
c      print*,jj


c Calculation of asymptotic coordinates

c First transform cartesian velocity to spherical velocity

      CALL CARBSP(th,f,vx,vy,vz,vr,vt,vf)
         
c Then obtain asymptotic theta and phi an get them in radiant
c th is the colatitude in rad (th=90.-the0, f=fi0*rad)

      CALL ASYMP(th,f,vr,vt,vf,at,af)
         
      ast=at/rad
      asf=af/rad
         
      IF(asf.gt.360.) asf=asf-360.

c Writing asymptotic coordinates of last point    
          !if(awrite.ne.1) then 
          ! write(2,*)i, res,r,td,fei,ast,asf,time,alength/Re
          ! else
	  !  write(2,*)i, res,0,0,0,0,0,0,0
	  ! endif 
c          write (6,*) r
           alast=alength/Re
           if(awrite.eq.1)then
              r=0
              td=0
              fei=0
              ast=0
              asf=0
              time=0
              alast=0
           end if

c Writing to the screen asymptotic coordinates of last point,ast,asf        	
c	Here calculate rigidity of the last point in trajectory 
c	to trace back particles (check of validity)

!          hm1=(hm0**2)*(c**4)
!          vva=(1-(vv/c)*(vv/c))
!          amen=(hm1/(vva*q*q))-(hm1/(q*q))
!          rigidity=dsqrt(abs(amen))   
!          frig=(rigidity)/(1e+9)
!          deltarig = (abs(rig-frig))/rig

c	here write all the quantities calculated by the program and the 
c	selection automatic or not made by the program. Time in sec. and length
c	in unit of Earth radii, velocity in unit of c, rigidity in GV.
c	time,alength/Re        
          
c	Once used to check...	
          
c	  write(6,*) 'time in sec = ' , time
c	  write(6,*) 'length in Re = ' , alength/Re
c	  write(6,*) 'number cross = '  , across
c	  write(6,*) 'final rigidity = '  , frig
c	  write(6,*) 'error rigidity = '  , deltarig
c	  write(6,*) 'velocity = '  , vv/c

        
!         END DO 	      ! END OF FIRST cicle NEW
       

! 16    continue
       
          
c        write(6,*)rmin,r0  
!       close(1)
!       close(2)
!       close(3)
!       close(4)
       
       
!       stop
       return
! 101   stop 'no Inpfil file'
! 102   stop 'no "parmod_new.dat" file'
! 103   stop 'no "OMNI_W1_W6.dat" file'
       end

c************************************************************************

       SUBROUTINE CARBSP(th,f,bx,by,bz,br,bt,bf)
       
c Transfer for magnetic field from cartesian to spherical coordinates
c Input parameters are theta, phi, bx, by, bz
C Output parameters are bradial, btheta and bphi

        IMPLICIT REAL*8 (A-H,O-Z)

         s=dsin(th)
         c=dcos(th)
         sf=dsin(f)
         cf=dcos(f)
         be=bx*cf+by*sf
         br=be*s+bz*c
         bf=-bx*sf+by*cf
         bt=be*c-bz*s

         return
         end
C	 
C	DOUBLE PREC D. GRANDI 12-2012
c************************************************************************

       SUBROUTINE ASYMP(th,f,vr,vt,vf,at,af)

c Calculation of asymptotic coordinates
c Input parameters are theta, phi,and velocity vradial, vtheta and vphi
c Output parameters are asymptotic theta and asymptotic phi
        
        IMPLICIT REAL*8 (A-H,O-Z)

        ct=dcos(th)
        st=dsin(th)
        va=vt*ct+vr*st
        dp=-vt*st+vr*ct
        dm=dsqrt(vt*vt+va*va)
        at=datan2(dp,dm)
        af=f+datan2(vf,va)

        return
        end

C
C	DOUBLE PREC D. GRANDI 12-2012
c************************************************************************

       SUBROUTINE BTOT(PAR,X,Y,Z,IOPT,BX,BY,BZ,count)

C	Written by:	Mauricio Peredo
C			Hughes STX at NASA/GSFC, Code 695
C			September 1992
C	
C	Modified by:	Davide Grandi
C			INFN Milano
C			Via Celoria 16-20123 Milano-ITALY
C			November 2012
C
Cc
c	CHECK COMMENTSS	
c
C	This routine computes the total magnetic field at each
C	(input) point (Xi,Yi,Zi). It is assumed that subroutine
C	RECALC has already been called to set quantities in
C	the common block (in particular, the call to RECALC
C	sets the dipole tilt angle, and all angles needed for
C	conversions between various coordinate systems).
C
C
C------ INPUT:  X,Y,Z = Arrays of Cartesian GSM coordinates with the points
C			where the field is to be computed (in Re)
C		IOPT = Flag indication which "version" of the model to
C		       use, the options correspond to different levels of
C		       magnetospheric activity.  EXT87L, EX89KP and EX89AE
C		       each allow IOPT = 1 to 6, EXT87T has IOPT = 1 to 8,
c		       EXT87W has IOPT = 101 to 112 for models binned by solar
c		       wind dynamic pressure and IMF-Bz, and IOPT = 1 to 6
c		       for models binned by Kp level.
c		count =  Dimension of X,Y,Z arrays
C		par   = vector of four quantities needed from T96 and T05			
C
C------ OUTPUT: BX,BY,BZ = Cartesian GSM components of the total (internal
C			   plus external) magnetic field vector at (X,Y,Z)
C       
c Peredo's program for total geomagnetic field

cD
      IMPLICIT REAL*8 (A-H,O-Z)
cD

cP
      INTEGER IY,K,count,i,iopt,IHARM

      REAL*8 X,Y,Z,BX,BY,
     1	BZ,BXEXT,BYEXT,BZEXT,XGEO,YGEO,ZGEO,R,T,F,FX,FY,
     2	FZ,BXINT,BYINT,BZINT,BR,BF,BT,HX,HY,HZ

     
      REAL*8 PDYNI,DST,BYIM,BZIM,W1,W2,W3,W4,W5,W6,EXT      
      REAL*8 PARMOD(10),PAR(11),PS,PSI
      
c      COMMON /GEOPACK2/ G(105),H(105),REC(105)
        
c       COMMON /GEOPACK1/ ST0,CT0,SL0,CL0,CTCL,STCL,CTSL,STSL,SFI,CFI,
c     * SPS,CPS,DS3,CGST,SGST,PSI,A11,A21,A31,A12,A22,A32,A13,A23,A33,
c     * E11,E21,E31,E12,E22,E32,E13,E23,E33,AA,BB,
c     * AAA,AB,A,B,CC,DD
     
	 
         DATA PARMOD/0.0,0.0,0.0,0.0,0.0,0.0,0.0,0.0,0.0,0.0/ 
                           
        nkrat=1
	
        PARMOD(1)=PAR(1)
        PARMOD(2)=PAR(2)
        PARMOD(3)=PAR(3)
        PARMOD(4)=PAR(4)
        PARMOD(5)=PAR(5)
	PARMOD(6)=PAR(6)
	PARMOD(7)=PAR(7)
	PARMOD(8)=PAR(8)
	PARMOD(9)=PAR(9)
	PARMOD(10)=PAR(10)
	
	EXT = PAR(11)      

cD
 
        CALL GEOGSW_08(XGEO,YGEO,ZGEO,X,Y,Z,-1) 	!from solar GSW to terrestrial GEO

        CALL SPHCAR_08(R,T,F,XGEO,YGEO,ZGEO,-1)
           
c        CALL IGRF_GSM (X,Y,Z,HX,HY,HZ)      
	CALL IGRF_GSW_08 (X,Y,Z,HX,HY,HZ) 	!GEOCENTRIC SOLAR-WIND (GSW) COORDINATE SYSTEM
c
C	Compute external field

	IF(EXT.NE.0)THEN
	
c	WRITE(6,*) 'EXT MODEL'
	
         IF (R.LT.2)THEN
             BXEXT=0.
             BYEXT=0.
             BZEXT=0.
         ENDIF


         IF ((R.GE.2).AND.(R.LT.3)) THEN
           nkrat=nkrat+1
           IF (nkrat.EQ.2) THEN
	    IF(EXT.EQ.1) THEN
	     CALL T96_01(IOPT,PARMOD,PSI,X,Y,Z,BXEXT,BYEXT,BZEXT)
            ELSE IF(EXT.EQ.2) THEN
	     CALL T04_s(IOPT,PARMOD,PSI,X,Y,Z,BXEXT,BYEXT,BZEXT)
	    ENDIF 
	   ENDIF
           IF (nkrat.EQ.11) THEN
           nkrat=1
           ENDIF
         ENDIF
        
        
        
         IF ((R.GE.3).AND.(R.LT.4)) THEN
           nkrat=nkrat+1
           IF (nkrat.EQ.2) THEN
	    IF(EXT.EQ.1) THEN
	     CALL T96_01(IOPT,PARMOD,PSI,X,Y,Z,BXEXT,BYEXT,BZEXT)
c	     WRITE(6,*) 'EXT 96'
            ELSE IF(EXT.EQ.2) THEN
c             WRITE(6,*) 'EXT 05'
	     CALL T04_s(IOPT,PARMOD,PSI,X,Y,Z,BXEXT,BYEXT,BZEXT)
	    ENDIF 	   
	   ENDIF
           IF (nkrat.EQ.6) THEN
           nkrat=1
           ENDIF
         ENDIF
        
        
         IF (R.GE.4) THEN
	  IF(EXT.EQ.1) THEN
	     CALL T96_01(IOPT,PARMOD,PSI,X,Y,Z,BXEXT,BYEXT,BZEXT)
            ELSE IF(EXT.EQ.2) THEN
	     CALL T04_s(IOPT,PARMOD,PSI,X,Y,Z,BXEXT,BYEXT,BZEXT)
	    ENDIF          	 
         ENDIF
	 
c	do not compute external field

	ELSE
c	 WRITE(6,*) 'NO EXT'
	     BXEXT=0.
             BYEXT=0.
             BZEXT=0.
	ENDIF

c	sum up

           BX = HX + BXEXT
           BY = HY + BYEXT
           BZ = HZ + BZEXT


 100    continue
c         write(*,*)HX,HY,HZ
c         write(*,*)BX,BY,BZ

        return
        END

c*******************************************************************************************

c@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@ c

c
c          ##########################################################################
c          #                                                                        #
c          #                          GEOPACK-2008_dp                               #
c          #                     (MAIN SET OF FORTRAN CODES)                        #
c          #               (double-precision version of 03/21/08)                   #
C          #                (IGRF coefficients updated 11/30/10)                    #
c          ##########################################################################
C
c
c  This collection of subroutines is a result of several upgrades of the original package
c  written by N. A. Tsyganenko in 1978-1979.
c
c  PREFATORY NOTE TO THE VERSION OF FEBRUARY 4, 2008:
c
c  To avoid inappropriate use of obsolete subroutines from earlier versions, a suffix 08 was
c  added to the name of each subroutine in this release.
c
c  A possibility has been added in this version to calculate vector components in the
c  "Geocentric Solar Wind" (GSW) coordinate system, which, to our knowledge, was first
c  introduced by Hones et al., Planet. Space Sci., v.34, p.889, 1986 (aka GSWM, see Appendix,
c  Tsyganenko et al., JGRA, v.103(A4), p.6827, 1998). The GSW system is analogous to the
c  standard GSM, except that its X-axis is antiparallel to the currently observed solar wind
c  flow vector, rather than aligned with the Earth-Sun line. The orientation of axes in the
c  GSW system can be uniquely defined by specifying three components (VGSEX,VGSEY,VGSEZ) of
c  the solar wind velocity, and in the case of a strictly radial anti-sunward flow (VGSEY=
c  VGSEZ=0) the GSW system becomes identical to the standard GSM, which fact was used here
c  to minimize the number of subroutines in the package. To that end, instead of the special
c  case of the GSM coordinates, this version uses a more general GSW system, and three more
c  input parameters are added in the subroutine RECALC_08, the observed values (VGSEX,VGSEY,
c  VGSEZ) of the solar wind velocity. Invoking RECALC_08 with VGSEY=VGSEZ=0 restores the
c  standard (sunward) orientation of the X axis, which allows one to easily convert vectors
c  between GSW and GSM, as well as to/from other standard and commonly used systems. For more
c  details, see the documentation file GEOPACK-2008.DOC.
c
c  Another modification allows users to have more control over the procedure of field line
c  mapping using the subroutine TRACE_08. To that end, three new input parameters were added
c  in that subroutine, allowing one to set (i) an upper limit, DSMAX, on the automatically
c  adjusted step size, (ii) a permissible step error, ERR, and (iii) maximal length, LMAX,
c  of arrays where field line point coordinates are stored. Minor changes were also made in
c  the tracing subroutine, to make it more compact and easier for understanding, and to
c  prevent the algorithm from making uncontrollable large number of multiple loops in some
c  cases with plasmoid-like field structures.
c
C  One more subroutine, named GEODGEO_08, was added to the package, allowing one to convert
c  geodetic coordinates of a point in space (altitude above the Earth's WGS84 ellipsoid and
c  geodetic latitude) to geocentric radial distance and colatitude, and vice versa.
c
C  For a complete list of modifications made earlier in previous versions, see the
c  documentation file GEOPACK-2008.DOC.
c
c----------------------------------------------------------------------------------
c
      SUBROUTINE IGRF_GSW_08 (XGSW,YGSW,ZGSW,HXGSW,HYGSW,HZGSW)
c
C  CALCULATES COMPONENTS OF THE MAIN (INTERNAL) GEOMAGNETIC FIELD IN THE GEOCENTRIC SOLAR-WIND
C  (GSW) COORDINATE SYSTEM, USING IAGA INTERNATIONAL GEOMAGNETIC REFERENCE MODEL COEFFICIENTS
C  (e.g., http://www.ngdc.noaa.gov/IAGA/vmod/igrf.html, revised 22 March, 2005)
c
C  THE GSW SYSTEM IS ESSENTIALLY SIMILAR TO THE STANDARD GSM (THE TWO SYSTEMS BECOME IDENTICAL
C  TO EACH OTHER IN THE CASE OF STRICTLY ANTI-SUNWARD SOLAR WIND FLOW). FOR A DETAILED
C  DEFINITION, SEE INTRODUCTORY COMMENTS FOR THE SUBROUTINE GSWGSE_08 .
C
C  BEFORE THE FIRST CALL OF THIS SUBROUTINE, OR, IF THE DATE/TIME (IYEAR,IDAY,IHOUR,MIN,ISEC),
C  OR THE SOLAR WIND VELOCITY COMPONENTS (VGSEX,VGSEY,VGSEZ) HAVE CHANGED, THE MODEL COEFFICIENTS
c  AND GEO-GSW ROTATION MATRIX ELEMENTS SHOULD BE UPDATED BY CALLING THE SUBROUTINE RECALC_08.
C
C-----INPUT PARAMETERS:
C
C     XGSW,YGSW,ZGSW - CARTESIAN GEOCENTRIC SOLAR-WIND COORDINATES (IN UNITS RE=6371.2 KM)
C
C-----OUTPUT PARAMETERS:
C
C     HXGSW,HYGSW,HZGSW - CARTESIAN GEOCENTRIC SOLAR-WIND COMPONENTS OF THE MAIN GEOMAGNETIC
C                           FIELD IN NANOTESLA
C
C     LAST MODIFICATION:  MARCH 21, 2008 (DOUBLE-PRECISION VERSION)
C     THIS VERSION OF THE CODE ACCEPTS DATES FROM 1965 THROUGH 2020.
c
C     AUTHOR: N. A. TSYGANENKO
C
C
      IMPLICIT REAL*8 (A-H,O-Z)
      COMMON /GEOPACK2/ G(105),H(105),REC(105)

      DIMENSION A(14),B(14)

      CALL GEOGSW_08 (XGEO,YGEO,ZGEO,XGSW,YGSW,ZGSW,-1)
      RHO2=XGEO**2+YGEO**2
      R=DSQRT(RHO2+ZGEO**2)
      C=ZGEO/R
      RHO=DSQRT(RHO2)
      S=RHO/R
      IF (S.LT.1.D-10) THEN
        CF=1.D0
        SF=0.D0
      ELSE
        CF=XGEO/RHO
        SF=YGEO/RHO
      ENDIF
C
      PP=1.D0/R
      P=PP
C
C  IN THIS VERSION, THE OPTIMAL VALUE OF THE PARAMETER NM (MAXIMAL ORDER OF THE SPHERICAL
C    HARMONIC EXPANSION) IS NOT USER-PRESCRIBED, BUT CALCULATED INSIDE THE SUBROUTINE, BASED
C      ON THE VALUE OF THE RADIAL DISTANCE R:
C
      IRP3=R+2
      NM=3+30/IRP3
      IF (NM.GT.13) NM=13

      K=NM+1
      DO 150 N=1,K
         P=P*PP
         A(N)=P
150      B(N)=P*N

      P=1.D0
      D=0.D0
      BBR=0.D0
      BBT=0.D0
      BBF=0.D0

      DO 200 M=1,K
         IF(M.EQ.1) GOTO 160
         MM=M-1
         W=X
         X=W*CF+Y*SF
         Y=Y*CF-W*SF
         GOTO 170
160      X=0.D0
         Y=1.D0
170      Q=P
         Z=D
         BI=0.D0
         P2=0.D0
         D2=0.D0
         DO 190 N=M,K
            AN=A(N)
            MN=N*(N-1)/2+M
            E=G(MN)
            HH=H(MN)
            W=E*Y+HH*X
            BBR=BBR+B(N)*W*Q
            BBT=BBT-AN*W*Z
            IF(M.EQ.1) GOTO 180
            QQ=Q
            IF(S.LT.1.D-10) QQ=Z
            BI=BI+AN*(E*X-HH*Y)*QQ
180         XK=REC(MN)
            DP=C*Z-S*Q-XK*D2
            PM=C*Q-XK*P2
            D2=Z
            P2=Q
            Z=DP
190        Q=PM
         D=S*D+C*P
         P=S*P
         IF(M.EQ.1) GOTO 200
         BI=BI*MM
         BBF=BBF+BI
200   CONTINUE
C
      BR=BBR
      BT=BBT
      IF(S.LT.1.D-10) GOTO 210
      BF=BBF/S
      GOTO 211
210   IF(C.LT.0.) BBF=-BBF
      BF=BBF

211   HE=BR*S+BT*C
      HXGEO=HE*CF-BF*SF
      HYGEO=HE*SF+BF*CF
      HZGEO=BR*C-BT*S
C
      CALL GEOGSW_08 (HXGEO,HYGEO,HZGEO,HXGSW,HYGSW,HZGSW,1)
C
      RETURN
      END
C
c==========================================================================================
C
c
      SUBROUTINE IGRF_GEO_08 (R,THETA,PHI,BR,BTHETA,BPHI)
c
C  CALCULATES COMPONENTS OF THE MAIN (INTERNAL) GEOMAGNETIC FIELD IN THE SPHERICAL GEOGRAPHIC
C  (GEOCENTRIC) COORDINATE SYSTEM, USING IAGA INTERNATIONAL GEOMAGNETIC REFERENCE MODEL
C  COEFFICIENTS  (e.g., http://www.ngdc.noaa.gov/IAGA/vmod/igrf.html, revised: 22 March, 2005)
C
C  BEFORE THE FIRST CALL OF THIS SUBROUTINE, OR IF THE DATE (IYEAR AND IDAY) WAS CHANGED,
C  THE MODEL COEFFICIENTS SHOULD BE UPDATED BY CALLING THE SUBROUTINE RECALC_08
C
C-----INPUT PARAMETERS:
C
C   R, THETA, PHI - SPHERICAL GEOGRAPHIC (GEOCENTRIC) COORDINATES:
C   RADIAL DISTANCE R IN UNITS RE=6371.2 KM, COLATITUDE THETA AND LONGITUDE PHI IN RADIANS
C
C-----OUTPUT PARAMETERS:
C
C     BR, BTHETA, BPHI - SPHERICAL COMPONENTS OF THE MAIN GEOMAGNETIC FIELD IN NANOTESLA
C      (POSITIVE BR OUTWARD, BTHETA SOUTHWARD, BPHI EASTWARD)
C
C     LAST MODIFICATION:  MARCH 21, 2008 (DOUBLE-PRECISION VERSION)
C     THIS VERSION OF THE  CODE ACCEPTS DATES FROM 1965 THROUGH 2020.
c
C     AUTHOR: N. A. TSYGANENKO
C
C
      IMPLICIT REAL*8 (A-H,O-Z)
      COMMON /GEOPACK2/ G(105),H(105),REC(105)

      DIMENSION A(14),B(14)

      C=DCOS(THETA)
      S=DSIN(THETA)
      CF=DCOS(PHI)
      SF=DSIN(PHI)
C
      PP=1.D0/R
      P=PP
C
C  IN THIS NEW VERSION, THE OPTIMAL VALUE OF THE PARAMETER NM (MAXIMAL ORDER OF THE SPHERICAL
C    HARMONIC EXPANSION) IS NOT USER-PRESCRIBED, BUT CALCULATED INSIDE THE SUBROUTINE, BASED
C      ON THE VALUE OF THE RADIAL DISTANCE R:
C
      IRP3=R+2
      NM=3+30/IRP3
      IF (NM.GT.13) NM=13

      K=NM+1
      DO 150 N=1,K
         P=P*PP
         A(N)=P
150      B(N)=P*N

      P=1.D0
      D=0.D0
      BBR=0.D0
      BBT=0.D0
      BBF=0.D0

      DO 200 M=1,K
         IF(M.EQ.1) GOTO 160
         MM=M-1
         W=X
         X=W*CF+Y*SF
         Y=Y*CF-W*SF
         GOTO 170
160      X=0.D0
         Y=1.D0
170      Q=P
         Z=D
         BI=0.D0
         P2=0.D0
         D2=0.D0
         DO 190 N=M,K
            AN=A(N)
            MN=N*(N-1)/2+M
            E=G(MN)
            HH=H(MN)
            W=E*Y+HH*X
            BBR=BBR+B(N)*W*Q
            BBT=BBT-AN*W*Z
            IF(M.EQ.1) GOTO 180
            QQ=Q
            IF(S.LT.1.D-5) QQ=Z
            BI=BI+AN*(E*X-HH*Y)*QQ
180         XK=REC(MN)
            DP=C*Z-S*Q-XK*D2
            PM=C*Q-XK*P2
            D2=Z
            P2=Q
            Z=DP
190        Q=PM
         D=S*D+C*P
         P=S*P
         IF(M.EQ.1) GOTO 200
         BI=BI*MM
         BBF=BBF+BI
200   CONTINUE
C
      BR=BBR
      BTHETA=BBT
      IF(S.LT.1.D-10) GOTO 210
      BPHI=BBF/S
      RETURN
210   IF(C.LT.0.D0) BBF=-BBF
      BPHI=BBF

      RETURN
      END
C
c==========================================================================================
c
       SUBROUTINE DIP_08 (XGSW,YGSW,ZGSW,BXGSW,BYGSW,BZGSW)
C
C  CALCULATES GSW (GEOCENTRIC SOLAR-WIND) COMPONENTS OF GEODIPOLE FIELD WITH THE DIPOLE MOMENT
C  CORRESPONDING TO THE EPOCH, SPECIFIED BY CALLING SUBROUTINE RECALC_08 (SHOULD BE
C  INVOKED BEFORE THE FIRST USE OF THIS ONE, OR IF THE DATE/TIME, AND/OR THE OBSERVED
C  SOLAR WIND DIRECTION, HAVE CHANGED.
C
C  THE GSW COORDINATE SYSTEM IS ESSENTIALLY SIMILAR TO THE STANDARD GSM (THE TWO SYSTEMS BECOME
C  IDENTICAL TO EACH OTHER IN THE CASE OF STRICTLY RADIAL ANTI-SUNWARD SOLAR WIND FLOW). ITS
C  DETAILED DEFINITION IS GIVEN IN INTRODUCTORY COMMENTS FOR THE SUBROUTINE GSWGSE_08 .

C--INPUT PARAMETERS: XGSW,YGSW,ZGSW - GSW COORDINATES IN RE (1 RE = 6371.2 km)
C
C--OUTPUT PARAMETERS: BXGSW,BYGSW,BZGSW - FIELD COMPONENTS IN GSW SYSTEM, IN NANOTESLA.
C
C  LAST MODIFICATION:   MARCH 21, 2008 (DOUBLE-PRECISION VERSION).
C
C  AUTHOR: N. A. TSYGANENKO
C
      IMPLICIT REAL*8 (A-H,O-Z)
      COMMON /GEOPACK1/ AA(10),SPS,CPS,BB(22)
      COMMON /GEOPACK2/ G(105),H(105),REC(105)
C
      DIPMOM=DSQRT(G(2)**2+G(3)**2+H(3)**2)
C
      P=XGSW**2
      U=ZGSW**2
      V=3.D0*ZGSW*XGSW
      T=YGSW**2
      Q=DIPMOM/DSQRT(P+T+U)**5
      BXGSW=Q*((T+U-2.D0*P)*SPS-V*CPS)
      BYGSW=-3.D0*YGSW*Q*(XGSW*SPS+ZGSW*CPS)
      BZGSW=Q*((P+T-2.D0*U)*CPS-V*SPS)
      RETURN
      END

C*******************************************************************
c
      SUBROUTINE SUN_08 (IYEAR,IDAY,IHOUR,MIN,ISEC,GST,SLONG,SRASN,SDEC)
C
C  CALCULATES FOUR QUANTITIES NECESSARY FOR COORDINATE TRANSFORMATIONS
C  WHICH DEPEND ON SUN POSITION (AND, HENCE, ON UNIVERSAL TIME AND SEASON)
C
C-------  INPUT PARAMETERS:
C  IYR,IDAY,IHOUR,MIN,ISEC -  YEAR, DAY, AND UNIVERSAL TIME IN HOURS, MINUTES,
C    AND SECONDS  (IDAY=1 CORRESPONDS TO JANUARY 1).
C
C-------  OUTPUT PARAMETERS:
C  GST - GREENWICH MEAN SIDEREAL TIME, SLONG - LONGITUDE ALONG ECLIPTIC
C  SRASN - RIGHT ASCENSION,  SDEC - DECLINATION  OF THE SUN (RADIANS)
C  ORIGINAL VERSION OF THIS SUBROUTINE HAS BEEN COMPILED FROM:
C  RUSSELL, C.T., COSMIC ELECTRODYNAMICS, 1971, V.2, PP.184-196.
C
C  LAST MODIFICATION:    MARCH 21, 2008 (DOUBLE-PRECISION VERSION)
C
C     ORIGINAL VERSION WRITTEN BY:    Gilbert D. Mead
C
      IMPLICIT REAL*8 (A-H,O-Z)
      DATA RAD/57.295779513D0/
C
      IF(IYEAR.LT.1901.OR.IYEAR.GT.2099) RETURN
      FDAY=DFLOAT(IHOUR*3600+MIN*60+ISEC)/86400.D0
      DJ=365*(IYEAR-1900)+(IYEAR-1901)/4+IDAY-0.5D0+FDAY
      T=DJ/36525.D0
      VL=DMOD(279.696678D0+0.9856473354D0*DJ,360.D0)
      GST=DMOD(279.690983D0+.9856473354D0*DJ+360.D0*FDAY+180.D0,360.D0)/
     * RAD
      G=DMOD(358.475845D0+0.985600267D0*DJ,360.D0)/RAD
      SLONG=(VL+(1.91946D0-0.004789D0*T)*DSIN(G)+0.020094D0
     * *DSIN(2.D0*G))/RAD
      IF(SLONG.GT.6.2831853D0) SLONG=SLONG-6.283185307D0
      IF (SLONG.LT.0.D0) SLONG=SLONG+6.283185307D0
      OBLIQ=(23.45229D0-0.0130125D0*T)/RAD
      SOB=DSIN(OBLIQ)
      SLP=SLONG-9.924D-5
C
C   THE LAST CONSTANT IS A CORRECTION FOR THE ANGULAR ABERRATION DUE TO
C   EARTH'S ORBITAL MOTION
C
      SIND=SOB*DSIN(SLP)
      COSD=DSQRT(1.D0-SIND**2)
      SC=SIND/COSD
      SDEC=DATAN(SC)
      SRASN=3.141592654D0-DATAN2(DCOS(OBLIQ)/SOB*SC,-DCOS(SLP)/COSD)
      RETURN
      END
C
C================================================================================
c
      SUBROUTINE SPHCAR_08 (R,THETA,PHI,X,Y,Z,J)
C
C   CONVERTS SPHERICAL COORDS INTO CARTESIAN ONES AND VICE VERSA
C    (THETA AND PHI IN RADIANS).
C
C                  J>0            J<0
C-----INPUT:   J,R,THETA,PHI     J,X,Y,Z
C----OUTPUT:      X,Y,Z        R,THETA,PHI
C
C  NOTE: AT THE POLES (X=0 AND Y=0) WE ASSUME PHI=0 WHEN CONVERTING
C        FROM CARTESIAN TO SPHERICAL COORDS (I.E., FOR J<0)
C
C   LAST MOFIFICATION:    MARCH 21, 2008 (DOUBLE-PRECISION VERSION)
C
C   AUTHOR:  N. A. TSYGANENKO
C
      IMPLICIT REAL*8 (A-H,O-Z)

      IF(J.GT.0) GOTO 3
      SQ=X**2+Y**2
      R=DSQRT(SQ+Z**2)
      IF (SQ.NE.0.D0) GOTO 2
      PHI=0.D0
      IF (Z.LT.0.D0) GOTO 1
      THETA=0.D0
      RETURN
  1   THETA=3.141592654D0
      RETURN
  2   SQ=DSQRT(SQ)
      PHI=DATAN2(Y,X)
      THETA=DATAN2(SQ,Z)
      IF (PHI.LT.0.D0) PHI=PHI+6.283185307D0
      RETURN
  3   SQ=R*DSIN(THETA)
      X=SQ*DCOS(PHI)
      Y=SQ*DSIN(PHI)
      Z=R*DCOS(THETA)
      RETURN
      END
C
C===========================================================================
c
      SUBROUTINE BSPCAR_08 (THETA,PHI,BR,BTHETA,BPHI,BX,BY,BZ)
C
C   CALCULATES CARTESIAN FIELD COMPONENTS FROM LOCAL SPHERICAL ONES
C
C-----INPUT:   THETA,PHI - SPHERICAL ANGLES OF THE POINT IN RADIANS
C              BR,BTHETA,BPHI -  LOCAL SPHERICAL COMPONENTS OF THE FIELD
C-----OUTPUT:  BX,BY,BZ - CARTESIAN COMPONENTS OF THE FIELD
C
C   LAST MOFIFICATION:    MARCH 21, 2008 (DOUBLE-PRECISION VERSION)
C
C   WRITTEN BY:  N. A. TSYGANENKO
C
      IMPLICIT REAL*8 (A-H,O-Z)

      S=DSIN(THETA)
      C=DCOS(THETA)
      SF=DSIN(PHI)
      CF=DCOS(PHI)
      BE=BR*S+BTHETA*C
      BX=BE*CF-BPHI*SF
      BY=BE*SF+BPHI*CF
      BZ=BR*C-BTHETA*S
      RETURN
      END
c
C==============================================================================
C
      SUBROUTINE BCARSP_08 (X,Y,Z,BX,BY,BZ,BR,BTHETA,BPHI)
C
CALCULATES LOCAL SPHERICAL FIELD COMPONENTS FROM THOSE IN CARTESIAN SYSTEM
C
C-----INPUT:   X,Y,Z  - CARTESIAN COMPONENTS OF THE POSITION VECTOR
C              BX,BY,BZ - CARTESIAN COMPONENTS OF THE FIELD VECTOR
C-----OUTPUT:  BR,BTHETA,BPHI - LOCAL SPHERICAL COMPONENTS OF THE FIELD VECTOR
C
C  NOTE: AT THE POLES (THETA=0 OR THETA=PI) WE ASSUME PHI=0,
C        AND HENCE BTHETA=BX, BPHI=BY
C
C   WRITTEN AND ADDED TO THIS PACKAGE:  APRIL 1, 2003
c   LAST MODIFICATION:    MARCH 21, 2008 (DOUBLE-PRECISION VERSION)
C   AUTHOR:   N. A. TSYGANENKO
C
      IMPLICIT REAL*8 (A-H,O-Z)

      RHO2=X**2+Y**2
      R=DSQRT(RHO2+Z**2)
      RHO=DSQRT(RHO2)

      IF (RHO.NE.0.D0) THEN
        CPHI=X/RHO
        SPHI=Y/RHO
       ELSE
        CPHI=1.D0
        SPHI=0.D0
      ENDIF

      CT=Z/R
      ST=RHO/R

      BR=(X*BX+Y*BY+Z*BZ)/R
      BTHETA=(BX*CPHI+BY*SPHI)*CT-BZ*ST
      BPHI=BY*CPHI-BX*SPHI

      RETURN
      END
C
c=====================================================================================
C
      SUBROUTINE RECALC_08 (IYEAR,IDAY,IHOUR,MIN,ISEC,VGSEX,VGSEY,VGSEZ)
C
C  1. PREPARES ELEMENTS OF ROTATION MATRICES FOR TRANSFORMATIONS OF VECTORS BETWEEN
C     SEVERAL COORDINATE SYSTEMS, MOST FREQUENTLY USED IN SPACE PHYSICS.
C
C  2. PREPARES COEFFICIENTS USED IN THE CALCULATION OF THE MAIN GEOMAGNETIC FIELD
C      (IGRF MODEL)
C
C  THIS SUBROUTINE SHOULD BE INVOKED BEFORE USING THE FOLLOWING SUBROUTINES:
C  IGRF_GEO_08, IGRF_GSW_08, DIP_08, GEOMAG_08, GEOGSW_08, MAGSW_08, SMGSW_08, GSWGSE_08,
c  GEIGEO_08, TRACE_08, STEP_08, RHAND_08.
C
C  THERE IS NO NEED TO REPEATEDLY INVOKE RECALC_08, IF MULTIPLE CALCULATIONS ARE MADE
C    FOR THE SAME DATE/TIME AND SOLAR WIND FLOW DIRECTION.
C
C-----INPUT PARAMETERS:
C
C     IYEAR   -  YEAR NUMBER (FOUR DIGITS)
C     IDAY  -  DAY OF YEAR (DAY 1 = JAN 1)
C     IHOUR -  HOUR OF DAY (00 TO 23)
C     MIN   -  MINUTE OF HOUR (00 TO 59)
C     ISEC  -  SECONDS OF MINUTE (00 TO 59)
C     VGSEX,VGSEY,VGSEZ - GSE (GEOCENTRIC SOLAR-ECLIPTIC) COMPONENTS OF THE OBSERVED
C                              SOLAR WIND FLOW VELOCITY (IN KM/S)
C
C  IMPORTANT: IF ONLY QUESTIONABLE INFORMATION (OR NO INFORMATION AT ALL) IS AVAILABLE
C             ON THE SOLAR WIND SPEED, OR, IF THE STANDARD GSM AND/OR SM COORDINATES ARE
C             INTENDED TO BE USED, THEN SET VGSEX=-400.0 AND VGSEY=VGSEZ=0. IN THIS CASE,
C             THE GSW COORDINATE SYSTEM BECOMES IDENTICAL TO THE STANDARD GSM.
C
C             IF ONLY SCALAR SPEED V OF THE SOLAR WIND IS KNOWN, THEN SETTING
C             VGSEX=-V, VGSEY=29.78, VGSEZ=0.0 WILL TAKE INTO ACCOUNT THE ~4 degs
C             ABERRATION OF THE MAGNETOSPHERE DUE TO EARTH'S ORBITAL MOTION
C
C             IF ALL THREE GSE COMPONENTS OF THE SOLAR WIND VELOCITY ARE AVAILABLE,
C             PLEASE NOTE THAT IN SOME SOLAR WIND DATABASES THE ABERRATION EFFECT
C             HAS ALREADY BEEN TAKEN INTO ACCOUNT BY SUBTRACTING 29.78 KM/S FROM VYGSE;
C             IN THAT CASE, THE UNABERRATED (OBSERVED) VYGSE VALUES SHOULD BE RESTORED
C             BY ADDING BACK THE 29.78 KM/S CORRECTION. WHETHER OR NOT TO DO THAT, MUST
C             BE EITHER VERIFIED WITH THE DATA ORIGINATOR OR DETERMINED BY AVERAGING
C             VGSEY OVER A SUFFICIENTLY LONG TIME INTERVAL.
C
C-----OUTPUT PARAMETERS:  NONE (ALL OUTPUT QUANTITIES ARE PLACED
C                         INTO THE COMMON BLOCKS /GEOPACK1/ AND /GEOPACK2/)
C
C    OTHER SUBROUTINES CALLED BY THIS ONE: SUN_08
C
C    AUTHOR:  N.A. TSYGANENKO
C    DATE:    DEC.1, 1991
C
C    REVISION OF JANUARY 31, 2015:
C
C     The table of IGRF coefficients was extended to include those for the epoch 2015 (igrf-12)
c     (for details, see http://www.ngdc.noaa.gov/IAGA/vmod/igrf.html)
C
C    REVISION OF NOVEMBER 30, 2010:
C
C     The table of IGRF coefficients was extended to include those for the epoch 2010
c         (for details, see http://www.ngdc.noaa.gov/IAGA/vmod/igrf.html)
C
C    REVISION OF NOVEMBER 15, 2007: ADDED THE POSSIBILITY TO TAKE INTO ACCOUNT THE OBSERVED
C     DEFLECTION OF THE SOLAR WIND FLOW FROM STRICTLY RADIAL DIRECTION. TO THAT END, THREE
C     GSE COMPONENTS OF THE SOLAR WIND VELOCITY WERE ADDED TO THE INPUT PARAMETERS.
C
c    CORRECTION OF MAY 9, 2006:  INTERPOLATION OF THE COEFFICIENTS (BETWEEN
C     LABELS 50 AND 105) IS NOW MADE THROUGH THE LAST ELEMENT OF THE ARRAYS
C     G(105)  AND H(105) (PREVIOUSLY MADE ONLY THROUGH N=66, WHICH IN SOME
C     CASES CAUSED RUNTIME ERRORS)
c
C    REVISION OF MAY 3, 2005:
C     The table of IGRF coefficients was extended to include those for the epoch 2005
c       the maximal order of spherical harmonics was also increased up to 13
c         (for details, see http://www.ngdc.noaa.gov/IAGA/vmod/igrf.html)
c
C    REVISION OF APRIL 3, 2003:
c    The code now includes preparation of the model coefficients for the subroutines
c    IGRF_08 and GEOMAG_08. This eliminates the need for the SAVE statements, used
c    in the old versions, making the codes easier and more compiler-independent.
C
      IMPLICIT REAL*8 (A-H,O-Z)
      SAVE ISW
C
      COMMON /GEOPACK1/ ST0,CT0,SL0,CL0,CTCL,STCL,CTSL,STSL,SFI,CFI,
     * SPS,CPS,DS3,CGST,SGST,PSI,A11,A21,A31,A12,A22,A32,A13,A23,A33,
     * E11,E21,E31,E12,E22,E32,E13,E23,E33
C
C  THE COMMON BLOCK /GEOPACK1/ CONTAINS ELEMENTS OF THE ROTATION MATRICES AND OTHER
C   PARAMETERS RELATED TO THE COORDINATE TRANSFORMATIONS PERFORMED BY THIS PACKAGE
C
      COMMON /GEOPACK2/ G(105),H(105),REC(105)
C
C  THE COMMON BLOCK /GEOPACK2/ CONTAINS COEFFICIENTS OF THE IGRF FIELD MODEL, CALCULATED
C    FOR A GIVEN YEAR AND DAY FROM THEIR STANDARD EPOCH VALUES. THE ARRAY REC CONTAINS
C    COEFFICIENTS USED IN THE RECURSION RELATIONS FOR LEGENDRE ASSOCIATE POLYNOMIALS.
C
      DIMENSION G65(105),H65(105),G70(105),H70(105),G75(105),H75(105),
     + G80(105),H80(105),G85(105),H85(105),G90(105),H90(105),G95(105),
     + H95(105),G00(105),H00(105),G05(105),H05(105),G10(105),H10(105),
     + G15(105),H15(105),DG15(45),DH15(45)
C
      DATA ISW /0/
c
      DATA G65/0.D0,-30334.D0,-2119.D0,-1662.D0,2997.D0,1594.D0,1297.D0,
     *-2038.D0,1292.D0,856.D0,957.D0,804.D0,479.D0,-390.D0,252.D0,
     *-219.D0,358.D0,254.D0,-31.D0,-157.D0,-62.D0,45.D0,61.D0,8.D0,
     *-228.D0,4.D0,1.D0,-111.D0,75.D0,-57.D0,4.D0,13.D0,-26.D0,-6.D0,
     *13.D0,1.D0,13.D0,5.D0,-4.D0,-14.D0,0.D0,8.D0,-1.D0,11.D0,4.D0,
     *8.D0,10.D0,2.D0,-13.D0,10.D0,-1.D0,-1.D0,5.D0,1.D0,-2.D0,-2.D0,
     *-3.D0,2.D0,-5.D0,-2.D0,4.D0,4.D0,0.D0,2.D0,2.D0,0.D0,39*0.D0/
      DATA H65/0.D0,0.D0,5776.D0,0.D0,-2016.D0,114.D0,0.D0,-404.D0,
     *240.D0,-165.D0,0.D0,148.D0,-269.D0,13.D0,-269.D0,0.D0,19.D0,
     *128.D0,-126.D0,-97.D0,81.D0,0.D0,-11.D0,100.D0,68.D0,-32.D0,-8.D0,
     *-7.D0,0.D0,-61.D0,-27.D0,-2.D0,6.D0,26.D0,-23.D0,-12.D0,0.D0,7.D0,
     *-12.D0,9.D0,-16.D0,4.D0,24.D0,-3.D0,-17.D0,0.D0,-22.D0,15.D0,7.D0,
     *-4.D0,-5.D0,10.D0,10.D0,-4.D0,1.D0,0.D0,2.D0,1.D0,2.D0,6.D0,-4.D0,
     *0.D0,-2.D0,3.D0,0.D0,-6.D0,39*0.D0/
c
      DATA G70/0.D0,-30220.D0,-2068.D0,-1781.D0,3000.D0,1611.D0,1287.D0,
     *-2091.D0,1278.D0,838.D0,952.D0,800.D0,461.D0,-395.D0,234.D0,
     *-216.D0,359.D0,262.D0,-42.D0,-160.D0,-56.D0,43.D0,64.D0,15.D0,
     *-212.D0,2.D0,3.D0,-112.D0,72.D0,-57.D0,1.D0,14.D0,-22.D0,-2.D0,
     *13.D0,-2.D0,14.D0,6.D0,-2.D0,-13.D0,-3.D0,5.D0,0.D0,11.D0,3.D0,
     *8.D0,10.D0,2.D0,-12.D0,10.D0,-1.D0,0.D0,3.D0,1.D0,-1.D0,-3.D0,
     *-3.D0,2.D0,-5.D0,-1.D0,6.D0,4.D0,1.D0,0.D0,3.D0,-1.D0,39*0.D0/
      DATA H70/0.D0,0.D0,5737.D0,0.D0,-2047.D0,25.D0,0.D0,-366.D0,
     *251.D0,-196.D0,0.D0,167.D0,-266.D0,26.D0,-279.D0,0.D0,26.D0,
     *139.D0,-139.D0,-91.D0,83.D0,0.D0,-12.D0,100.D0,72.D0,-37.D0,-6.D0,
     *1.D0,0.D0,-70.D0,-27.D0,-4.D0,8.D0,23.D0,-23.D0,-11.D0,0.D0,7.D0,
     *-15.D0,6.D0,-17.D0,6.D0,21.D0,-6.D0,-16.D0,0.D0,-21.D0,16.D0,6.D0,
     *-4.D0,-5.D0,10.D0,11.D0,-2.D0,1.D0,0.D0,1.D0,1.D0,3.D0,4.D0,-4.D0,
     *0.D0,-1.D0,3.D0,1.D0,-4.D0,39*0.D0/
c
      DATA G75/0.D0,-30100.D0,-2013.D0,-1902.D0,3010.D0,1632.D0,1276.D0,
     *-2144.D0,1260.D0,830.D0,946.D0,791.D0,438.D0,-405.D0,216.D0,
     *-218.D0,356.D0,264.D0,-59.D0,-159.D0,-49.D0,45.D0,66.D0,28.D0,
     *-198.D0,1.D0,6.D0,-111.D0,71.D0,-56.D0,1.D0,16.D0,-14.D0,0.D0,
     *12.D0,-5.D0,14.D0,6.D0,-1.D0,-12.D0,-8.D0,4.D0,0.D0,10.D0,1.D0,
     *7.D0,10.D0,2.D0,-12.D0,10.D0,-1.D0,-1.D0,4.D0,1.D0,-2.D0,-3.D0,
     *-3.D0,2.D0,-5.D0,-2.D0,5.D0,4.D0,1.D0,0.D0,3.D0,-1.D0,39*0.D0/
C
      DATA H75/0.D0,0.D0,5675.D0,0.D0,-2067.D0,-68.D0,0.D0,-333.D0,
     *262.D0,-223.D0,0.D0,191.D0,-265.D0,39.D0,-288.D0,0.D0,31.D0,
     *148.D0,-152.D0,-83.D0,88.D0,0.D0,-13.D0,99.D0,75.D0,-41.D0,-4.D0,
     *11.D0,0.D0,-77.D0,-26.D0,-5.D0,10.D0,22.D0,-23.D0,-12.D0,0.D0,
     *6.D0,-16.D0,4.D0,-19.D0,6.D0,18.D0,-10.D0,-17.D0,0.D0,-21.D0,
     *16.D0,7.D0,-4.D0,-5.D0,10.D0,11.D0,-3.D0,1.D0,0.D0,1.D0,1.D0,3.D0,
     *4.D0,-4.D0,-1.D0,-1.D0,3.D0,1.D0,-5.D0,39*0.D0/
c
      DATA G80/0.D0,-29992.D0,-1956.D0,-1997.D0,3027.D0,1663.D0,1281.D0,
     *-2180.D0,1251.D0,833.D0,938.D0,782.D0,398.D0,-419.D0,199.D0,
     *-218.D0,357.D0,261.D0,-74.D0,-162.D0,-48.D0,48.D0,66.D0,42.D0,
     *-192.D0,4.D0,14.D0,-108.D0,72.D0,-59.D0,2.D0,21.D0,-12.D0,1.D0,
     *11.D0,-2.D0,18.D0,6.D0,0.D0,-11.D0,-7.D0,4.D0,3.D0,6.D0,-1.D0,
     *5.D0,10.D0,1.D0,-12.D0,9.D0,-3.D0,-1.D0,7.D0,2.D0,-5.D0,-4.D0,
     *-4.D0,2.D0,-5.D0,-2.D0,5.D0,3.D0,1.D0,2.D0,3.D0,0.D0,39*0.D0/
C
      DATA H80/0.D0,0.D0,5604.D0,0.D0,-2129.D0,-200.D0,0.D0,-336.D0,
     *271.D0,-252.D0,0.D0,212.D0,-257.D0,53.D0,-297.D0,0.D0,46.D0,
     *150.D0,-151.D0,-78.D0,92.D0,0.D0,-15.D0,93.D0,71.D0,-43.D0,-2.D0,
     *17.D0,0.D0,-82.D0,-27.D0,-5.D0,16.D0,18.D0,-23.D0,-10.D0,0.D0,
     *7.D0,-18.D0,4.D0,-22.D0,9.D0,16.D0,-13.D0,-15.D0,0.D0,-21.D0,
     *16.D0,9.D0,-5.D0,-6.D0,9.D0,10.D0,-6.D0,2.D0,0.D0,1.D0,0.D0,3.D0,
     *6.D0,-4.D0,0.D0,-1.D0,4.D0,0.D0,-6.D0,39*0.D0/
c
      DATA G85/0.D0,-29873.D0,-1905.D0,-2072.D0,3044.D0,1687.D0,1296.D0,
     *-2208.D0,1247.D0,829.D0,936.D0,780.D0,361.D0,-424.D0,170.D0,
     *-214.D0,355.D0,253.D0,-93.D0,-164.D0,-46.D0,53.D0,65.D0,51.D0,
     *-185.D0,4.D0,16.D0,-102.D0,74.D0,-62.D0,3.D0,24.D0,-6.D0,4.D0,
     *10.D0,0.D0,21.D0,6.D0,0.D0,-11.D0,-9.D0,4.D0,4.D0,4.D0,-4.D0,5.D0,
     *10.D0,1.D0,-12.D0,9.D0,-3.D0,-1.D0,7.D0,1.D0,-5.D0,-4.D0,-4.D0,
     *3.D0,-5.D0,-2.D0,5.D0,3.D0,1.D0,2.D0,3.D0,0.D0,39*0.D0/
C
      DATA H85/0.D0,0.D0,5500.D0,0.D0,-2197.D0,-306.D0,0.D0,-310.D0,
     *284.D0,-297.D0,0.D0,232.D0,-249.D0,69.D0,-297.D0,0.D0,47.D0,
     *150.D0,-154.D0,-75.D0,95.D0,0.D0,-16.D0,88.D0,69.D0,-48.D0,-1.D0,
     *21.D0,0.D0,-83.D0,-27.D0,-2.D0,20.D0,17.D0,-23.D0,-7.D0,0.D0,8.D0,
     *-19.D0,5.D0,-23.D0,11.D0,14.D0,-15.D0,-11.D0,0.D0,-21.D0,15.D0,
     *9.D0,-6.D0,-6.D0,9.D0,9.D0,-7.D0,2.D0,0.D0,1.D0,0.D0,3.D0,6.D0,
     *-4.D0,0.D0,-1.D0,4.D0,0.D0,-6.D0,39*0.D0/
c
      DATA G90/0.D0,-29775.D0,-1848.D0,-2131.D0,3059.D0,1686.D0,1314.D0,
     *     -2239.D0,  1248.D0,  802.D0,  939.D0, 780.D0, 325.D0,-423.D0,
     *       141.D0,  -214.D0,  353.D0,  245.D0,-109.D0,-165.D0, -36.D0,
     *        61.D0,    65.D0,   59.D0, -178.D0,   3.D0,  18.D0, -96.D0,
     *        77.D0,   -64.D0,    2.D0,   26.D0,  -1.D0,   5.D0,   9.D0,
     *         0.D0,    23.D0,    5.D0,   -1.D0, -10.D0, -12.D0,   3.D0,
     *         4.D0,     2.D0,   -6.D0,    4.D0,   9.D0,   1.D0, -12.D0,
     *         9.D0,    -4.D0,   -2.D0,    7.D0,   1.D0,  -6.D0,  -3.D0,
     *        -4.D0,     2.D0,   -5.D0,   -2.D0,   4.D0,   3.D0,   1.D0,
     *         3.D0,     3.D0,    0.D0,39*0.D0/

      DATA H90/0.D0,  0.D0,5406.D0,   0.D0,-2279.D0,-373.D0,  0.D0,
     *      -284.D0,293.D0,-352.D0,   0.D0,  247.D0,-240.D0, 84.D0,
     *      -299.D0,  0.D0,  46.D0, 154.D0, -153.D0, -69.D0, 97.D0,
     *         0.D0,-16.D0,  82.D0,  69.D0,  -52.D0,   1.D0, 24.D0,
     *         0.D0,-80.D0, -26.D0,   0.D0,   21.D0,  17.D0,-23.D0,
     *        -4.D0,  0.D0,  10.D0, -19.D0,    6.D0, -22.D0, 12.D0,
     *        12.D0,-16.D0, -10.D0,   0.D0,  -20.D0,  15.D0, 11.D0,
     *        -7.D0, -7.D0,   9.D0,   8.D0,   -7.D0,   2.D0,  0.D0,
     *         2.D0,  1.D0,   3.D0,   6.D0,   -4.D0,   0.D0, -2.D0,
     *         3.D0, -1.D0,  -6.D0,39*0.D0/

      DATA G95/0.D0,-29692.D0,-1784.D0,-2200.D0,3070.D0,1681.D0,1335.D0,
     *     -2267.D0,  1249.D0,  759.D0,  940.D0, 780.D0, 290.D0,-418.D0,
     *       122.D0,  -214.D0,  352.D0,  235.D0,-118.D0,-166.D0, -17.D0,
     *        68.D0,    67.D0,   68.D0, -170.D0,  -1.D0,  19.D0, -93.D0,
     *        77.D0,   -72.D0,    1.D0,   28.D0,   5.D0,   4.D0,   8.D0,
     *        -2.D0,    25.D0,    6.D0,   -6.D0,  -9.D0, -14.D0,   9.D0,
     *         6.D0,    -5.D0,   -7.D0,    4.D0,   9.D0,   3.D0, -10.D0,
     *         8.D0,    -8.D0,   -1.D0,   10.D0,  -2.D0,  -8.D0,  -3.D0,
     *        -6.D0,     2.D0,   -4.D0,   -1.D0,   4.D0,   2.D0,   2.D0,
     *         5.D0,     1.D0,    0.D0,  39*0.D0/

      DATA H95/0.D0,  0.D0,5306.D0,  0.D0,-2366.D0,-413.D0,  0.D0,
     *      -262.D0,302.D0,-427.D0,  0.D0,  262.D0,-236.D0, 97.D0,
     *      -306.D0,  0.D0,  46.D0,165.D0, -143.D0, -55.D0,107.D0,
     *         0.D0,-17.D0,  72.D0, 67.D0,  -58.D0,   1.D0, 36.D0,
     *         0.D0,-69.D0, -25.D0,  4.D0,   24.D0,  17.D0,-24.D0,
     *        -6.D0,  0.D0,  11.D0,-21.D0,    8.D0, -23.D0, 15.D0,
     *        11.D0,-16.D0,  -4.D0,  0.D0,  -20.D0,  15.D0, 12.D0,
     *        -6.D0, -8.D0,   8.D0,  5.D0,   -8.D0,   3.D0,  0.D0,
     *         1.D0,  0.D0,   4.D0,  5.D0,   -5.D0,  -1.D0, -2.D0,
     *         1.D0, -2.D0,  -7.D0,39*0.D0/

      DATA G00/0.D0,-29619.4D0,-1728.2D0,-2267.7D0,3068.4D0,1670.9D0,
     *     1339.6D0,  -2288.D0, 1252.1D0,  714.5D0, 932.3D0, 786.8D0,
     *       250.D0,   -403.D0,  111.3D0, -218.8D0, 351.4D0, 222.3D0,
     *     -130.4D0,  -168.6D0,  -12.9D0,   72.3D0,  68.2D0,  74.2D0,
     *     -160.9D0,    -5.9D0,   16.9D0,  -90.4D0,  79.0D0, -74.0D0,
     *         0.D0,    33.3D0,    9.1D0,    6.9D0,   7.3D0,  -1.2D0,
     *       24.4D0,     6.6D0,   -9.2D0,   -7.9D0, -16.6D0,   9.1D0,
     *        7.0D0,    -7.9D0,    -7.D0,     5.D0,   9.4D0,    3.D0,
     *      - 8.4D0,     6.3D0,   -8.9D0,   -1.5D0,   9.3D0,  -4.3D0,
     *       -8.2D0,    -2.6D0,    -6.D0,    1.7D0,  -3.1D0,  -0.5D0,
     *        3.7D0,      1.D0,     2.D0,    4.2D0,   0.3D0,  -1.1D0,
     *        2.7D0,    -1.7D0,   -1.9D0,    1.5D0,  -0.1D0,   0.1D0,
     *       -0.7D0,     0.7D0,    1.7D0,    0.1D0,   1.2D0,   4.0D0,
     *       -2.2D0,    -0.3D0,    0.2D0,    0.9D0,  -0.2D0,   0.9D0,
     *       -0.5D0,     0.3D0,   -0.3D0,   -0.4D0,  -0.1D0,  -0.2D0,
     *       -0.4D0,    -0.2D0,   -0.9D0,    0.3D0,   0.1D0,  -0.4D0,
     *        1.3D0,    -0.4D0,    0.7D0,   -0.4D0,   0.3D0,  -0.1D0,
     *        0.4D0,      0.D0,    0.1D0/

      DATA H00/0.D0,   0.D0,5186.1D0,   0.D0,-2481.6D0,-458.0D0,   0.D0,
     *     -227.6D0,293.4D0,-491.1D0,   0.D0,  272.6D0,-231.9D0,119.8D0,
     *     -303.8D0,   0.D0,  43.8D0,171.9D0, -133.1D0, -39.3D0,106.3D0,
     *         0.D0,-17.4D0,  63.7D0, 65.1D0,  -61.2D0,   0.7D0, 43.8D0,
     *         0.D0,-64.6D0, -24.2D0,  6.2D0,    24.D0,  14.8D0,-25.4D0,
     *       -5.8D0,  0.0D0,  11.9D0,-21.5D0,    8.5D0, -21.5D0, 15.5D0,
     *        8.9D0,-14.9D0,  -2.1D0,  0.0D0,  -19.7D0,  13.4D0, 12.5D0,
     *       -6.2D0, -8.4D0,   8.4D0,  3.8D0,   -8.2D0,   4.8D0,  0.0D0,
     *        1.7D0,  0.0D0,   4.0D0,  4.9D0,   -5.9D0,  -1.2D0, -2.9D0,
     *        0.2D0, -2.2D0,  -7.4D0,  0.0D0,    0.1D0,   1.3D0, -0.9D0,
     *       -2.6D0,  0.9D0,  -0.7D0, -2.8D0,   -0.9D0,  -1.2D0, -1.9D0,
     *       -0.9D0,  0.0D0,  -0.4D0,  0.3D0,    2.5D0,  -2.6D0,  0.7D0,
     *        0.3D0,  0.0D0,   0.0D0,  0.3D0,   -0.9D0,  -0.4D0,  0.8D0,
     *        0.0D0, -0.9D0,   0.2D0,  1.8D0,   -0.4D0,  -1.0D0, -0.1D0,
     *        0.7D0,  0.3D0,   0.6D0,  0.3D0,   -0.2D0,  -0.5D0, -0.9D0/

      DATA G05/0.D0,  -29554.6D0,-1669.0D0,-2337.2D0,3047.7D0,1657.8D0,
     *     1336.3D0,   -2305.8D0, 1246.4D0,  672.5D0, 920.6D0, 798.0D0,
     *      210.7D0,    -379.9D0,  100.0D0, -227.0D0, 354.4D0, 208.9D0,
     *     -136.5D0,    -168.1D0,  -13.6D0,   73.6D0,  69.6D0,  76.7D0,
     *     -151.3D0,     -14.6D0,   14.6D0,  -86.4D0,  79.9D0, -74.5D0,
     *       -1.7D0,      38.7D0,   12.3D0,    9.4D0,   5.4D0,   1.9D0,
     *       24.8D0,       7.6D0,  -11.7D0,   -6.9D0, -18.1D0,  10.2D0,
     *        9.4D0,     -11.3D0,   -4.9D0,    5.6D0,   9.8D0,   3.6D0,
     *       -6.9D0,       5.0D0,  -10.8D0,   -1.3D0,   8.8D0,  -6.7D0,
     *       -9.2D0,      -2.2D0,   -6.1D0,    1.4D0,  -2.4D0,  -0.2D0,
     *        3.1D0,       0.3D0,    2.1D0,    3.8D0,  -0.2D0,  -2.1D0,
     *        2.9D0,      -1.6D0,   -1.9D0,    1.4D0,  -0.3D0,   0.3D0,
     *       -0.8D0,       0.5D0,    1.8D0,    0.2D0,   1.0D0,   4.0D0,
     *       -2.2D0,      -0.3D0,    0.2D0,    0.9D0,  -0.4D0,   1.0D0,
     *       -0.3D0,       0.5D0,   -0.4D0,   -0.4D0,   0.1D0,  -0.5D0,
     *       -0.1D0,      -0.2D0,   -0.9D0,    0.3D0,   0.3D0,  -0.4D0,
     *        1.2D0,      -0.4D0,    0.8D0,   -0.3D0,   0.4D0,  -0.1D0,
     *        0.4D0,      -0.1D0,   -0.2D0/
C
      DATA H05/0.D0,  0.0D0,5078.0D0,  0.0D0,-2594.5D0,-515.4D0,  0.0D0,
     *     -198.9D0,269.7D0,-524.7D0,  0.0D0,  282.1D0,-225.2D0,145.2D0,
     *     -305.4D0,  0.0D0,  42.7D0,180.3D0, -123.5D0, -19.6D0,103.9D0,
     *        0.0D0,-20.3D0,  54.8D0, 63.6D0,  -63.5D0,   0.2D0, 50.9D0,
     *        0.0D0,-61.1D0, -22.6D0,  6.8D0,   25.4D0,  10.9D0,-26.3D0,
     *       -4.6D0,  0.0D0,  11.2D0,-20.9D0,    9.8D0, -19.7D0, 16.2D0,
     *        7.6D0,-12.8D0,  -0.1D0,  0.0D0,  -20.1D0,  12.7D0, 12.7D0,
     *       -6.7D0, -8.2D0,   8.1D0,  2.9D0,   -7.7D0,   6.0D0,  0.0D0,
     *        2.2D0,  0.1D0,   4.5D0,  4.8D0,   -6.7D0,  -1.0D0, -3.5D0,
     *       -0.9D0, -2.3D0,  -7.9D0,  0.0D0,    0.3D0,   1.4D0, -0.8D0,
     *       -2.3D0,  0.9D0,  -0.6D0, -2.7D0,   -1.1D0,  -1.6D0, -1.9D0,
     *       -1.4D0,  0.0D0,  -0.6D0,  0.2D0,    2.4D0,  -2.6D0,  0.6D0,
     *        0.4D0,  0.0D0,   0.0D0,  0.3D0,   -0.9D0,  -0.3D0,  0.9D0,
     *        0.0D0, -0.8D0,   0.3D0,  1.7D0,   -0.5D0,  -1.1D0,  0.0D0,
     *        0.6D0,  0.2D0,   0.5D0,  0.4D0,   -0.2D0,  -0.6D0, -0.9D0/
C
      DATA G10/0.00D0,-29496.57D0,-1586.42D0,-2396.06D0,3026.34D0,
     *      1668.17D0,  1339.85D0,-2326.54D0, 1232.10D0, 633.73D0,
     *       912.66D0,   808.97D0,  166.58D0, -356.83D0,  89.40D0,
     *      -230.87D0,   357.29D0,  200.26D0, -141.05D0,-163.17D0,
     *        -8.03D0,    72.78D0,   68.69D0,   75.92D0,-141.40D0,
     *       -22.83D0,    13.10D0,  -78.09D0,   80.44D0, -75.00D0,
     *        -4.55D0,    45.24D0,   14.00D0,   10.46D0,   1.64D0,
     *         4.92D0,    24.41D0,    8.21D0,  -14.50D0,  -5.59D0,
     *       -19.34D0,    11.61D0,   10.85D0,  -14.05D0,  -3.54D0,
     *         5.50D0,     9.45D0,    3.45D0,   -5.27D0,   3.13D0,
     *       -12.38D0,    -0.76D0,    8.43D0,   -8.42D0, -10.08D0,
     *        -1.94D0,    -6.24D0,    0.89D0,   -1.07D0,  -0.16D0,
     *         2.45D0,    -0.33D0,    2.13D0,    3.09D0,  -1.03D0,
     *        -2.80D0,     3.05D0,   -1.48D0,   -2.03D0,   1.65D0,
     *        -0.51D0,     0.54D0,   -0.79D0,    0.37D0,   1.79D0,
     *         0.12D0,     0.75D0,    3.75D0,   -2.12D0,  -0.21D0,
     *         0.30D0,     1.04D0,   -0.63D0,    0.95D0,  -0.11D0,
     *         0.52D0,    -0.39D0,   -0.37D0,    0.21D0,  -0.77D0,
     *         0.04D0,    -0.09D0,   -0.89D0,    0.31D0,   0.42D0,
     *        -0.45D0,     1.08D0,   -0.31D0,    0.78D0,  -0.18D0,
     *         0.38D0,     0.02D0,    0.42D0,   -0.26D0,  -0.26D0/
C
      DATA H10/0.00D0,  0.00D0,4944.26D0,   0.00D0,-2708.54D0,
     *     -575.73D0,   0.00D0,-160.40D0, 251.75D0, -537.03D0, 0.00D0,
     *      286.48D0,-211.03D0, 164.46D0,-309.72D0,    0.00D0,44.58D0,
     *      189.01D0,-118.06D0,  -0.01D0, 101.04D0,    0.00D0,-20.90D0,
     *       44.18D0,  61.54D0, -66.26D0,   3.02D0,   55.40D0,  0.00D0,
     *      -57.80D0, -21.20D0,   6.54D0,  24.96D0,    7.03D0,-27.61D0,
     *       -3.28D0,   0.00D0,  10.84D0, -20.03D0,   11.83D0,-17.41D0,
     *       16.71D0,   6.96D0, -10.74D0,   1.64D0,    0.00D0,-20.54D0,
     *       11.51D0,  12.75D0,  -7.14D0,  -7.42D0,    7.97D0,  2.14D0,
     *       -6.08D0,   7.01D0,   0.00D0,   2.73D0,   -0.10D0,  4.71D0,
     *        4.44D0,  -7.22D0,  -0.96D0,  -3.95D0,   -1.99D0, -1.97D0,
     *       -8.31D0,   0.00D0,   0.13D0,   1.67D0,   -0.66D0, -1.76D0,
     *        0.85D0,  -0.39D0,  -2.51D0,  -1.27D0,   -2.11D0, -1.94D0,
     *       -1.86D0,   0.00D0,  -0.87D0,   0.27D0,    2.13D0, -2.49D0,
     *        0.49D0,   0.59D0,   0.00D0,   0.13D0,    0.27D0, -0.86D0,
     *       -0.23D0,   0.87D0,   0.00D0,  -0.87D0,    0.30D0,  1.66D0,
     *       -0.59D0,  -1.14D0,  -0.07D0,   0.54D0,    0.10D0,  0.49D0,
     *        0.44D0,  -0.25D0,  -0.53D0,  -0.79D0/
C
      DATA G15/0.D0,-29442.0D0, -1501.0D0, -2445.1D0,  3012.9D0,
     *     1676.7D0,  1350.7D0, -2352.3D0,  1225.6D0,   582.0D0,907.6D0,
     *      813.7D0,   120.4D0,  -334.9D0,    70.4D0,  -232.6D0,360.1D0,
     *      192.4D0,  -140.9D0,  -157.5D0,     4.1D0,    70.0D0, 67.7D0,
     *       72.7D0,  -129.9D0,   -28.9D0,    13.2D0,   -70.9D0, 81.6D0,
     *      -76.1D0,    -6.8D0,    51.8D0,    15.0D0,     9.4D0, -2.8D0,
     *        6.8D0,    24.2D0,     8.8D0,   -16.9D0,    -3.2D0,-20.6D0,
     *       13.4D0,    11.7D0,   -15.9D0,    -2.0D0,     5.4D0,  8.8D0,
     *        3.1D0,    -3.3D0,     0.7D0,   -13.3D0,    -0.1D0,  8.7D0,
     *       -9.1D0,   -10.5D0,    -1.9D0,    -6.3D0,     0.1D0,  0.5D0,
     *       -0.5D0,     1.8D0,    -0.7D0,     2.1D0,     2.4D0, -1.8D0,
     *       -3.6D0,     3.1D0,    -1.5D0,    -2.3D0,     2.0D0, -0.8D0,
     *        0.6D0,    -0.7D0,     0.2D0,     1.7D0,    -0.2D0,  0.4D0,
     *        3.5D0,    -1.9D0,    -0.2D0,     0.4D0,     1.2D0, -0.8D0,
     *        0.9D0,     0.1D0,     0.5D0,    -0.3D0,    -0.4D0,  0.2D0,
     *       -0.9D0,     0.0D0,     0.0D0,    -0.9D0,     0.4D0,  0.5D0,
     *       -0.5D0,     1.0D0,    -0.2D0,     0.8D0,    -0.1D0,  0.3D0,
     *        0.1D0,     0.5D0,    -0.4D0,    -0.3D0/
c
      DATA H15/0.D0,     0.0D0, 4797.1D0,     0.0D0, -2845.6D0,-641.9D0,
     *        0.0D0,  -115.3D0,  244.9D0,  -538.4D0,     0.0D0, 283.3D0,
     *     -188.7D0,   180.9D0, -329.5D0,     0.0D0,    47.3D0, 197.0D0,
     *     -119.3D0,    16.0D0,  100.2D0,     0.0D0,   -20.8D0,  33.2D0,
     *       58.9D0,   -66.7D0,    7.3D0,    62.6D0,     0.0D0, -54.1D0,
     *      -19.5D0,     5.7D0,   24.4D0,     3.4D0,   -27.4D0,  -2.2D0,
     *        0.0D0,    10.1D0,  -18.3D0,    13.3D0,   -14.6D0,  16.2D0,
     *        5.7D0,    -9.1D0,    2.1D0,     0.0D0,   -21.6D0,  10.8D0,
     *       11.8D0,    -6.8D0,   -6.9D0,     7.8D0,     1.0D0,  -4.0D0,
     *        8.4D0,     0.0D0,    3.2D0,    -0.4D0,     4.6D0,   4.4D0,
     *       -7.9D0,    -0.6D0,   -4.2D0,    -2.8D0,    -1.2D0,  -8.7D0,
     *        0.0D0,    -0.1D0,    2.0D0,    -0.7D0,    -1.1D0,   0.8D0,
     *       -0.2D0,    -2.2D0,   -1.4D0,    -2.5D0,    -2.0D0,  -2.4D0,
     *        0.0D0,    -1.1D0,    0.4D0,     1.9D0,    -2.2D0,   0.3D0,
     *        0.7D0,    -0.1D0,    0.3D0,     0.2D0,    -0.9D0,  -0.1D0,
     *        0.7D0,     0.0D0,   -0.9D0,     0.4D0,     1.6D0,  -0.5D0,
     *       -1.2D0,    -0.1D0,    0.4D0,    -0.1D0,     0.4D0,   0.5D0,
     *       -0.3D0,    -0.4D0,   -0.8D0/
c
      DATA DG15/0.0D0, 10.3D0,  18.1D0,  -8.7D0,  -3.3D0,  2.1D0, 3.4D0,
     *         -5.5D0, -0.7D0, -10.1D0,  -0.7D0,   0.2D0, -9.1D0, 4.1D0,
     *         -4.3D0, -0.2D0,   0.5D0,  -1.3D0,  -0.1D0,  1.4D0, 3.9D0,
     *         -0.3D0, -0.1D0,  -0.7D0,   2.1D0,  -1.2D0,  0.3D0, 1.6D0,
     *          0.3D0, -0.2D0,  -0.5D0,   1.3D0,   0.1D0, -0.6D0,-0.8D0,
     *          0.2D0,  0.2D0,   0.0D0,  -0.6D0,   0.5D0, -0.2D0, 0.4D0,
     *          0.1D0, -0.4D0,   0.3D0/
c
      DATA DH15/0.0D0,  0.0D0, -26.6D0,   0.0D0, -27.4D0,-14.1D0, 0.0D0,
     *          8.2D0, -0.4D0,   1.8D0,   0.0D0,  -1.3D0,  5.3D0, 2.9D0,
     *         -5.2D0,  0.0D0,   0.6D0,   1.7D0,  -1.2D0,  3.4D0, 0.0D0,
     *          0.0D0,  0.0D0,  -2.1D0,  -0.7D0,   0.2D0,  0.9D0, 1.0D0,
     *          0.0D0,  0.8D0,   0.4D0,  -0.2D0,  -0.3D0, -0.6D0, 0.1D0,
     *         -0.2D0,  0.0D0,  -0.3D0,   0.3D0,   0.1D0,  0.5D0,-0.2D0,
     *         -0.3D0,  0.3D0,   0.0D0/
C
      IF (VGSEY.EQ.0..AND.VGSEZ.EQ.0..AND.ISW.NE.1) THEN
c      PRINT *, ''
c      PRINT *,
c     *' RECALC_08: RADIAL SOLAR WIND --> GSW SYSTEM IDENTICAL HERE'
c      PRINT *,
c     *' TO STANDARD GSM (I.E., XGSW AXIS COINCIDES WITH EARTH-SUN LINE)'
c      PRINT *, ''
      ISW=1
      ENDIF

      IF ((VGSEY.NE.0.D0.OR.VGSEZ.NE.0.D0).AND.ISW.NE.2) THEN       !  CORRECTED DEC.01, 2010
c      PRINT *, ''
c      PRINT *,
c     *' WARNING: NON-RADIAL SOLAR WIND FLOW SPECIFIED IN RECALC_08;'
c      PRINT *,
c     *' HENCE XGSW AXIS IS ASSUMED ORIENTED ANTIPARALLEL TO V_SW VECTOR'
c      PRINT *, ''
      ISW=2
      ENDIF
C
      IY=IYEAR
C
C  WE ARE RESTRICTED BY THE INTERVAL 1965-2020, FOR WHICH THE IGRF COEFFICIENTS
c    ARE KNOWN; IF IYEAR IS OUTSIDE THIS INTERVAL, THEN THE SUBROUTINE USES THE
C      NEAREST LIMITING VALUE AND PRINTS A WARNING:
C
      IF(IY.LT.1965) THEN
       IY=1965
c       WRITE (*,10) IYEAR,IY
      ENDIF

      IF(IY.GT.2020) THEN
       IY=2020
c       WRITE (*,10) IYEAR,IY
      ENDIF

C
C  CALCULATE THE ARRAY REC, CONTAINING COEFFICIENTS FOR THE RECURSION RELATIONS,
C  USED IN THE IGRF SUBROUTINE FOR CALCULATING THE ASSOCIATE LEGENDRE POLYNOMIALS
C  AND THEIR DERIVATIVES:
c
      DO 20 N=1,14
         N2=2*N-1
         N2=N2*(N2-2)
         DO 20 M=1,N
            MN=N*(N-1)/2+M
20    REC(MN)=DFLOAT((N-M)*(N+M-2))/DFLOAT(N2)
C
      IF (IY.LT.1970) GOTO 50          !INTERPOLATE BETWEEN 1965 - 1970
      IF (IY.LT.1975) GOTO 60          !INTERPOLATE BETWEEN 1970 - 1975
      IF (IY.LT.1980) GOTO 70          !INTERPOLATE BETWEEN 1975 - 1980
      IF (IY.LT.1985) GOTO 80          !INTERPOLATE BETWEEN 1980 - 1985
      IF (IY.LT.1990) GOTO 90          !INTERPOLATE BETWEEN 1985 - 1990
      IF (IY.LT.1995) GOTO 100         !INTERPOLATE BETWEEN 1990 - 1995
      IF (IY.LT.2000) GOTO 110         !INTERPOLATE BETWEEN 1995 - 2000
      IF (IY.LT.2005) GOTO 120         !INTERPOLATE BETWEEN 2000 - 2005
      IF (IY.LT.2010) GOTO 130         !INTERPOLATE BETWEEN 2005 - 2010
      IF (IY.LT.2015) GOTO 140         !INTERPOLATE BETWEEN 2010 - 2015
C
C       EXTRAPOLATE BEYOND 2015:
C
      DT=DFLOAT(IY)+DFLOAT(IDAY-1)/365.25D0-2015.D0
      DO 40 N=1,105
         G(N)=G15(N)
         H(N)=H15(N)
         IF (N.GT.45) GOTO 40
         G(N)=G(N)+DG15(N)*DT
         H(N)=H(N)+DH15(N)*DT
40    CONTINUE
      GOTO 300
C
C       INTERPOLATE BETWEEEN 1965 - 1970:
C
50    F2=(DFLOAT(IY)+DFLOAT(IDAY-1)/365.25D0-1965)/5.D0
      F1=1.D0-F2
      DO 55 N=1,105
         G(N)=G65(N)*F1+G70(N)*F2
55       H(N)=H65(N)*F1+H70(N)*F2
      GOTO 300
C
C       INTERPOLATE BETWEEN 1970 - 1975:
C
60    F2=(DFLOAT(IY)+DFLOAT(IDAY-1)/365.25D0-1970)/5.D0
      F1=1.D0-F2
      DO 65 N=1,105
         G(N)=G70(N)*F1+G75(N)*F2
65       H(N)=H70(N)*F1+H75(N)*F2
      GOTO 300
C
C       INTERPOLATE BETWEEN 1975 - 1980:
C
70    F2=(DFLOAT(IY)+DFLOAT(IDAY-1)/365.25D0-1975)/5.D0
      F1=1.D0-F2
      DO 75 N=1,105
         G(N)=G75(N)*F1+G80(N)*F2
75       H(N)=H75(N)*F1+H80(N)*F2
      GOTO 300
C
C       INTERPOLATE BETWEEN 1980 - 1985:
C
80    F2=(DFLOAT(IY)+DFLOAT(IDAY-1)/365.25D0-1980)/5.D0
      F1=1.D0-F2
      DO 85 N=1,105
         G(N)=G80(N)*F1+G85(N)*F2
85       H(N)=H80(N)*F1+H85(N)*F2
      GOTO 300
C
C       INTERPOLATE BETWEEN 1985 - 1990:
C
90    F2=(DFLOAT(IY)+DFLOAT(IDAY-1)/365.25D0-1985)/5.D0
      F1=1.D0-F2
      DO 95 N=1,105
         G(N)=G85(N)*F1+G90(N)*F2
95       H(N)=H85(N)*F1+H90(N)*F2
      GOTO 300
C
C       INTERPOLATE BETWEEN 1990 - 1995:
C
100   F2=(DFLOAT(IY)+DFLOAT(IDAY-1)/365.25D0-1990)/5.D0
      F1=1.D0-F2
      DO 105 N=1,105
         G(N)=G90(N)*F1+G95(N)*F2
105      H(N)=H90(N)*F1+H95(N)*F2
      GOTO 300
C
C       INTERPOLATE BETWEEN 1995 - 2000:
C
110   F2=(DFLOAT(IY)+DFLOAT(IDAY-1)/365.25D0-1995)/5.D0
      F1=1.D0-F2
      DO 115 N=1,105   !  THE 2000 COEFFICIENTS (G00) GO THROUGH THE ORDER 13, NOT 10
         G(N)=G95(N)*F1+G00(N)*F2
115      H(N)=H95(N)*F1+H00(N)*F2
      GOTO 300
C
C       INTERPOLATE BETWEEN 2000 - 2005:
C
120   F2=(DFLOAT(IY)+DFLOAT(IDAY-1)/365.25D0-2000)/5.D0
      F1=1.D0-F2
      DO 125 N=1,105
         G(N)=G00(N)*F1+G05(N)*F2
125      H(N)=H00(N)*F1+H05(N)*F2
      GOTO 300
C
C       INTERPOLATE BETWEEN 2005 - 2010:
C
130   F2=(FLOAT(IY)+FLOAT(IDAY-1)/365.25-2005)/5.
      F1=1.-F2
      DO 135 N=1,105
         G(N)=G05(N)*F1+G10(N)*F2
135      H(N)=H05(N)*F1+H10(N)*F2
      GOTO 300
C
C       INTERPOLATE BETWEEN 2010 - 2015:
C
140   F2=(DFLOAT(IY)+DFLOAT(IDAY-1)/365.25-2010)/5.
      F1=1.-F2
      DO 145 N=1,105
         G(N)=G10(N)*F1+G15(N)*F2
145      H(N)=H10(N)*F1+H15(N)*F2
      GOTO 300

C
C   COEFFICIENTS FOR A GIVEN YEAR HAVE BEEN CALCULATED; NOW MULTIPLY
C   THEM BY SCHMIDT NORMALIZATION FACTORS:
C
300   S=1.D0
      DO 150 N=2,14
         MN=N*(N-1)/2+1
         S=S*DFLOAT(2*N-3)/DFLOAT(N-1)
         G(MN)=G(MN)*S
         H(MN)=H(MN)*S
         P=S
         DO 150 M=2,N
            AA=1.D0
            IF (M.EQ.2) AA=2.D0
            P=P*DSQRT(AA*DFLOAT(N-M+1)/DFLOAT(N+M-2))
            MNN=MN+M-1
            G(MNN)=G(MNN)*P
150         H(MNN)=H(MNN)*P

           G_10=-G(2)
           G_11= G(3)
           H_11= H(3)
C
C  NOW CALCULATE GEO COMPONENTS OF THE UNIT VECTOR EzMAG, PARALLEL TO GEODIPOLE AXIS:
C   SIN(TETA0)*COS(LAMBDA0), SIN(TETA0)*SIN(LAMBDA0), AND COS(TETA0)
C         ST0 * CL0                ST0 * SL0                CT0
C
      SQ=G_11**2+H_11**2
      SQQ=DSQRT(SQ)
      SQR=DSQRT(G_10**2+SQ)
      SL0=-H_11/SQQ
      CL0=-G_11/SQQ
      ST0=SQQ/SQR
      CT0=G_10/SQR
      STCL=ST0*CL0
      STSL=ST0*SL0
      CTSL=CT0*SL0
      CTCL=CT0*CL0
C
C  NOW CALCULATE GEI COMPONENTS (S1,S2,S3) OF THE UNIT VECTOR S = EX_GSE
C    POINTING FROM THE EARTH'S CENTER TO SUN
C
      CALL SUN_08 (IY,IDAY,IHOUR,MIN,ISEC,GST,SLONG,SRASN,SDEC)
C
      S1=DCOS(SRASN)*DCOS(SDEC)
      S2=DSIN(SRASN)*DCOS(SDEC)
      S3=DSIN(SDEC)
C
C  NOW CALCULATE GEI COMPONENTS (DZ1,DZ2,DZ3) OF THE UNIT VECTOR EZGSE
C  POINTING NORTHWARD AND ORTHOGONAL TO THE ECLIPTIC PLANE, AS
C  (0,-SIN(OBLIQ),COS(OBLIQ)). FOR THE EPOCH 1978, OBLIQ = 23.44214 DEGS.
C  HERE WE USE A MORE ACCURATE TIME-DEPENDENT VALUE, DETERMINED AS:
C
      DJ=DFLOAT(365*(IY-1900)+(IY-1901)/4 +IDAY)
     * -0.5+DFLOAT(IHOUR*3600+MIN*60+ISEC)/86400.D0
      T=DJ/36525.D0
      OBLIQ=(23.45229D0-0.0130125D0*T)/57.2957795D0
      DZ1=0.D0
      DZ2=-DSIN(OBLIQ)
      DZ3=DCOS(OBLIQ)
C
C  NOW WE OBTAIN GEI COMPONENTS OF THE UNIT VECTOR EYGSE=(DY1,DY2,DY3),
C  COMPLETING THE RIGHT-HANDED SYSTEM. THEY CAN BE FOUND FROM THE VECTOR
C  PRODUCT EZGSE x EXGSE = (DZ1,DZ2,DZ3) x (S1,S2,S3):
C
      DY1=DZ2*S3-DZ3*S2
      DY2=DZ3*S1-DZ1*S3
      DY3=DZ1*S2-DZ2*S1
C
C  NOW LET'S CALCULATE GEI COMPONENTS OF THE UNIT VECTOR X = EXGSW, DIRECTED ANTIPARALLEL
C  TO THE OBSERVED SOLAR WIND FLOW. FIRST, CALCULATE ITS COMPONENTS IN GSE:
C
      V=DSQRT(VGSEX**2+VGSEY**2+VGSEZ**2)
      DX1=-VGSEX/V
      DX2=-VGSEY/V
      DX3=-VGSEZ/V
C
C  THEN IN GEI:
C
      X1=DX1*S1+DX2*DY1+DX3*DZ1
      X2=DX1*S2+DX2*DY2+DX3*DZ2
      X3=DX1*S3+DX2*DY3+DX3*DZ3
C
C  NOW CALCULATE GEI COMPONENTS (DIP1,DIP2,DIP3) OF THE UNIT VECTOR DIP = EZ_SM = EZ_MAG,
C   ALIGNED WITH THE GEODIPOLE AND POINTING NORTHWARD FROM ECLIPTIC PLANE:
C
      CGST=DCOS(GST)
      SGST=DSIN(GST)
C
      DIP1=STCL*CGST-STSL*SGST
      DIP2=STCL*SGST+STSL*CGST
      DIP3=CT0
C
C  THIS ALLOWS US TO CALCULATE GEI COMPONENTS OF THE UNIT VECTOR Y = EYGSW
C   BY TAKING THE VECTOR PRODUCT DIP x X AND NORMALIZING IT TO UNIT LENGTH:
C
      Y1=DIP2*X3-DIP3*X2
      Y2=DIP3*X1-DIP1*X3
      Y3=DIP1*X2-DIP2*X1
      Y=DSQRT(Y1*Y1+Y2*Y2+Y3*Y3)
      Y1=Y1/Y
      Y2=Y2/Y
      Y3=Y3/Y
C
C   AND GEI COMPONENTS OF THE UNIT VECTOR Z = EZGSW = EXGSW x EYGSW = X x Y:
C
      Z1=X2*Y3-X3*Y2
      Z2=X3*Y1-X1*Y3
      Z3=X1*Y2-X2*Y1
C
C   ELEMENTS OF THE MATRIX GSE TO GSW ARE THE SCALAR PRODUCTS:
C
C  E11=(EXGSE,EXGSW)  E12=(EXGSE,EYGSW)  E13=(EXGSE,EZGSW)
C  E21=(EYGSE,EXGSW)  E22=(EYGSE,EYGSW)  E23=(EYGSE,EZGSW)
C  E31=(EZGSE,EXGSW)  E32=(EZGSE,EYGSW)  E33=(EZGSE,EZGSW)
C
      E11= S1*X1 +S2*X2 +S3*X3
      E12= S1*Y1 +S2*Y2 +S3*Y3
      E13= S1*Z1 +S2*Z2 +S3*Z3
      E21=DY1*X1+DY2*X2+DY3*X3
      E22=DY1*Y1+DY2*Y2+DY3*Y3
      E23=DY1*Z1+DY2*Z2+DY3*Z3
      E31=DZ1*X1+DZ2*X2+DZ3*X3
      E32=DZ1*Y1+DZ2*Y2+DZ3*Y3
      E33=DZ1*Z1+DZ2*Z2+DZ3*Z3
C
C   GEODIPOLE TILT ANGLE IN THE GSW SYSTEM: PSI=ARCSIN(DIP,EXGSW)
C
      SPS=DIP1*X1+DIP2*X2+DIP3*X3
      CPS=DSQRT(1.D0-SPS**2)
      PSI=DASIN(SPS)
C
C   ELEMENTS OF THE MATRIX GEO TO GSW ARE THE SCALAR PRODUCTS:
C
C   A11=(EXGEO,EXGSW), A12=(EYGEO,EXGSW), A13=(EZGEO,EXGSW),
C   A21=(EXGEO,EYGSW), A22=(EYGEO,EYGSW), A23=(EZGEO,EYGSW),
C   A31=(EXGEO,EZGSW), A32=(EYGEO,EZGSW), A33=(EZGEO,EZGSW),
C
C   ALL THE UNIT VECTORS IN BRACKETS ARE ALREADY DEFINED IN GEI:
C
C  EXGEO=(CGST,SGST,0), EYGEO=(-SGST,CGST,0), EZGEO=(0,0,1)
C  EXGSW=(X1,X2,X3),  EYGSW=(Y1,Y2,Y3),   EZGSW=(Z1,Z2,Z3)
C                                                           AND  THEREFORE:
C
      A11=X1*CGST+X2*SGST
      A12=-X1*SGST+X2*CGST
      A13=X3
      A21=Y1*CGST+Y2*SGST
      A22=-Y1*SGST+Y2*CGST
      A23=Y3
      A31=Z1*CGST+Z2*SGST
      A32=-Z1*SGST+Z2*CGST
      A33=Z3
C
C  NOW CALCULATE ELEMENTS OF THE MATRIX MAG TO SM (ONE ROTATION ABOUT THE GEODIPOLE AXIS);
C   THEY ARE FOUND AS THE SCALAR PRODUCTS: CFI=GM22=(EYSM,EYMAG)=(EYGSW,EYMAG),
C                                          SFI=GM23=(EYSM,EXMAG)=(EYGSW,EXMAG),
C    DERIVED AS FOLLOWS:
C
C IN GEO, THE VECTORS EXMAG AND EYMAG HAVE THE COMPONENTS (CT0*CL0,CT0*SL0,-ST0)
C  AND (-SL0,CL0,0), RESPECTIVELY.    HENCE, IN GEI THEIR COMPONENTS ARE:
C  EXMAG:    CT0*CL0*COS(GST)-CT0*SL0*SIN(GST)
C            CT0*CL0*SIN(GST)+CT0*SL0*COS(GST)
C            -ST0
C  EYMAG:    -SL0*COS(GST)-CL0*SIN(GST)
C            -SL0*SIN(GST)+CL0*COS(GST)
C             0
C  NOW, NOTE THAT GEI COMPONENTS OF EYSM=EYGSW WERE FOUND ABOVE AS Y1, Y2, AND Y3,
C  AND WE ONLY HAVE TO COMBINE THESE QUANTITIES INTO SCALAR PRODUCTS:
C
      EXMAGX=CT0*(CL0*CGST-SL0*SGST)
      EXMAGY=CT0*(CL0*SGST+SL0*CGST)
      EXMAGZ=-ST0
      EYMAGX=-(SL0*CGST+CL0*SGST)
      EYMAGY=-(SL0*SGST-CL0*CGST)
      CFI=Y1*EYMAGX+Y2*EYMAGY
      SFI=Y1*EXMAGX+Y2*EXMAGY+Y3*EXMAGZ
C
 10   FORMAT(//1X,
     *'**** RECALC_08 WARNS: YEAR IS OUT OF INTERVAL 1965-2020: IYEAR=',
     *I4,/,6X,'CALCULATIONS WILL BE DONE FOR IYEAR=',I4,/)
      RETURN
      END
c
c==================================================================================

      SUBROUTINE GSWGSE_08 (XGSW,YGSW,ZGSW,XGSE,YGSE,ZGSE,J)
C
C  THIS SUBROUTINE TRANSFORMS COMPONENTS OF ANY VECTOR BETWEEN THE STANDARD GSE
C  COORDINATE SYSTEM AND THE GEOCENTRIC SOLAR-WIND (GSW, aka GSWM), DEFINED AS FOLLOWS
C  (HONES ET AL., PLANET.SPACE SCI., V.34, P.889, 1986; TSYGANENKO ET AL., JGRA,
C  V.103(A4), P.6827, 1998):
C
C  IN THE GSW SYSTEM, X AXIS IS ANTIPARALLEL TO THE OBSERVED DIRECTION OF THE SOLAR WIND FLOW.
C  TWO OTHER AXES, Y AND Z, ARE DEFINED IN THE SAME WAY AS FOR THE STANDARD GSM, THAT IS,
C  Z AXIS ORTHOGONAL TO X AXIS, POINTS NORTHWARD, AND LIES IN THE PLANE DEFINED BY THE X-
C  AND GEODIPOLE AXIS. THE Y AXIS COMPLETES THE RIGHT-HANDED SYSTEM.
C
C  THE GSW SYSTEM BECOMES IDENTICAL TO THE STANDARD GSM IN THE CASE OF
C   A STRICTLY RADIAL SOLAR WIND FLOW.
C
C  AUTHOR:  N. A. TSYGANENKO
C  ADDED TO 2008 VERSION OF GEOPACK: JAN 27, 2008.
C  LAST MODIFICATION:    MARCH 21, 2008 (DOUBLE-PRECISION VERSION)
C                    J>0                       J<0
C-----INPUT:   J,XGSW,YGSW,ZGSW          J,XGSE,YGSE,ZGSE
C-----OUTPUT:    XGSE,YGSE,ZGSE            XGSW,YGSW,ZGSW
C
C  IMPORTANT THINGS TO REMEMBER:
C
C   (1) BEFORE CALLING GSWGSE_08, BE SURE TO INVOKE SUBROUTINE RECALC_08, IN ORDER
C       TO DEFINE ALL NECESSARY ELEMENTS OF TRANSFORMATION MATRICES
C
C   (2) IN THE ABSENCE OF INFORMATION ON THE SOLAR WIND DIRECTION, E.G., WITH ONLY SCALAR
C       SPEED V KNOWN, THIS SUBROUTINE CAN BE USED TO CONVERT VECTORS TO ABERRATED
C       COORDINATE SYSTEM, TAKING INTO ACCOUNT EARTH'S ORBITAL SPEED OF 29 KM/S.
C       TO DO THAT, SPECIFY THE LAST 3 PARAMETERS IN RECALC_08 AS FOLLOWS:
C       VGSEX=-V, VGSEY=29.0, VGSEZ=0.0.
C
C       IT SHOULD ALSO BE KEPT IN MIND THAT IN SOME SOLAR WIND DATABASES THE ABERRATION
C       EFFECT HAS ALREADY BEEN TAKEN INTO ACCOUNT BY SUBTRACTING 29 KM/S FROM VYGSE;
C       IN THAT CASE, THE ORIGINAL VYGSE VALUES SHOULD BE RESTORED BY ADDING BACK THE
C       29 KM/S CORRECTION. WHETHER OR NOT TO DO THAT, MUST BE VERIFIED WITH THE DATA
C       ORIGINATOR (OR CAN BE DETERMINED BY CALCULATING THE AVERAGE VGSEY OVER
C       A SUFFICIENTLY LONG TIME INTERVAL)
C
C   (3) IF NO INFORMATION IS AVAILABLE ON THE SOLAR WIND SPEED, THEN SET VGSEX=-400.0
c       AND  VGSEY=VGSEZ=0. IN THAT CASE, THE GSW COORDINATE SYSTEM BECOMES
c       IDENTICAL TO THE STANDARD ONE.
C
      IMPLICIT REAL*8 (A-H,O-Z)
      COMMON /GEOPACK1/ AAA(25),E11,E21,E31,E12,E22,E32,E13,E23,E33
C
C  DIRECT TRANSFORMATION:
C
      IF (J.GT.0) THEN
        XGSE=XGSW*E11+YGSW*E12+ZGSW*E13
        YGSE=XGSW*E21+YGSW*E22+ZGSW*E23
        ZGSE=XGSW*E31+YGSW*E32+ZGSW*E33
      ENDIF
C
C   INVERSE TRANSFORMATION: CARRIED OUT USING THE TRANSPOSED MATRIX:
C
      IF (J.LT.0) THEN
        XGSW=XGSE*E11+YGSE*E21+ZGSE*E31
        YGSW=XGSE*E12+YGSE*E22+ZGSE*E32
        ZGSW=XGSE*E13+YGSE*E23+ZGSE*E33
      ENDIF
C
      RETURN
      END
C
C========================================================================================
C
      SUBROUTINE GEOMAG_08 (XGEO,YGEO,ZGEO,XMAG,YMAG,ZMAG,J)
C
C    CONVERTS GEOGRAPHIC (GEO) TO DIPOLE (MAG) COORDINATES OR VICE VERSA.
C
C                    J>0                       J<0
C-----INPUT:  J,XGEO,YGEO,ZGEO           J,XMAG,YMAG,ZMAG
C-----OUTPUT:    XMAG,YMAG,ZMAG           XGEO,YGEO,ZGEO
C
C  ATTENTION:  SUBROUTINE  RECALC_08  MUST BE INVOKED BEFORE GEOMAG_08 IN TWO CASES:
C     /A/  BEFORE THE FIRST TRANSFORMATION OF COORDINATES
C     /B/  IF THE VALUES OF IYEAR AND/OR IDAY HAVE BEEN CHANGED
C
C  NO INFORMATION IS REQUIRED HERE ON THE SOLAR WIND VELOCITY, SO ONE
C  CAN SET VGSEX=-400.0, VGSEY=0.0, VGSEZ=0.0 IN RECALC_08.
C
C   LAST MOFIFICATION:    MARCH 21, 2008 (DOUBLE-PRECISION VERSION)
C
C   AUTHOR:  N. A. TSYGANENKO
C
      IMPLICIT REAL*8 (A-H,O-Z)
      COMMON /GEOPACK1/ ST0,CT0,SL0,CL0,CTCL,STCL,CTSL,STSL,AB(26)

      IF(J.GT.0) THEN
       XMAG=XGEO*CTCL+YGEO*CTSL-ZGEO*ST0
       YMAG=YGEO*CL0-XGEO*SL0
       ZMAG=XGEO*STCL+YGEO*STSL+ZGEO*CT0
      ELSE
       XGEO=XMAG*CTCL-YMAG*SL0+ZMAG*STCL
       YGEO=XMAG*CTSL+YMAG*CL0+ZMAG*STSL
       ZGEO=ZMAG*CT0-XMAG*ST0
      ENDIF

      RETURN
      END
c
c=========================================================================================
c
      SUBROUTINE GEIGEO_08 (XGEI,YGEI,ZGEI,XGEO,YGEO,ZGEO,J)
C
C   CONVERTS EQUATORIAL INERTIAL (GEI) TO GEOGRAPHICAL (GEO) COORDS
C   OR VICE VERSA.
C                    J>0                J<0
C----INPUT:  J,XGEI,YGEI,ZGEI    J,XGEO,YGEO,ZGEO
C----OUTPUT:   XGEO,YGEO,ZGEO      XGEI,YGEI,ZGEI
C
C  ATTENTION:  SUBROUTINE  RECALC_08  MUST BE INVOKED BEFORE GEIGEO_08 IN TWO CASES:
C     /A/  BEFORE THE FIRST TRANSFORMATION OF COORDINATES
C     /B/  IF THE CURRENT VALUES OF IYEAR,IDAY,IHOUR,MIN,ISEC HAVE BEEN CHANGED
C
C  NO INFORMATION IS REQUIRED HERE ON THE SOLAR WIND VELOCITY, SO ONE
C  CAN SET VGSEX=-400.0, VGSEY=0.0, VGSEZ=0.0 IN RECALC_08.
C
C     LAST MODIFICATION:    MARCH 21, 2008 (DOUBLE-PRECISION VERSION)

C     AUTHOR:  N. A. TSYGANENKO
C
      IMPLICIT REAL*8 (A-H,O-Z)
      COMMON /GEOPACK1/ A(13),CGST,SGST,B(19)
C
      IF(J.GT.0) THEN
       XGEO=XGEI*CGST+YGEI*SGST
       YGEO=YGEI*CGST-XGEI*SGST
       ZGEO=ZGEI
      ELSE
       XGEI=XGEO*CGST-YGEO*SGST
       YGEI=YGEO*CGST+XGEO*SGST
       ZGEI=ZGEO
      ENDIF

      RETURN
      END
C
C=======================================================================================
C
      SUBROUTINE MAGSM_08 (XMAG,YMAG,ZMAG,XSM,YSM,ZSM,J)
C
C  CONVERTS DIPOLE (MAG) TO SOLAR MAGNETIC (SM) COORDINATES OR VICE VERSA
C
C                    J>0              J<0
C----INPUT: J,XMAG,YMAG,ZMAG     J,XSM,YSM,ZSM
C----OUTPUT:    XSM,YSM,ZSM       XMAG,YMAG,ZMAG
C
C  ATTENTION:  SUBROUTINE  RECALC_08  MUST BE INVOKED BEFORE MAGSM_08 IN THREE CASES:
C     /A/  BEFORE THE FIRST TRANSFORMATION OF COORDINATES, OR
C     /B/  IF THE VALUES OF IYEAR,IDAY,IHOUR,MIN,ISEC HAVE CHANGED, AND/OR
C     /C/  IF THE VALUES OF COMPONENTS OF THE SOLAR WIND FLOW VELOCITY HAVE CHANGED
C
C    IMPORTANT NOTE:
C
C        A NON-STANDARD DEFINITION IS IMPLIED HERE FOR THE SOLAR MAGNETIC COORDINATE
C        SYSTEM:  IT IS ASSUMED THAT THE XSM AXIS LIES IN THE PLANE DEFINED BY THE
C        GEODIPOLE AXIS AND THE OBSERVED VECTOR OF THE SOLAR WIND FLOW (RATHER THAN
C        THE EARTH-SUN LINE).  IN ORDER TO CONVERT MAG COORDINATES TO AND FROM THE
C        STANDARD SM COORDINATES, INVOKE RECALC_08 WITH VGSEX=-400.0, VGSEY=0.0, VGSEZ=0.0
C
C     LAST MODIFICATION:    MARCH 21, 2008 (DOUBLE-PRECISION VERSION)
C
C     AUTHOR:  N. A. TSYGANENKO
C
      IMPLICIT REAL*8 (A-H,O-Z)
      COMMON /GEOPACK1/ A(8),SFI,CFI,B(24)
C
      IF (J.GT.0) THEN
       XSM=XMAG*CFI-YMAG*SFI
       YSM=XMAG*SFI+YMAG*CFI
       ZSM=ZMAG
      ELSE
       XMAG=XSM*CFI+YSM*SFI
       YMAG=YSM*CFI-XSM*SFI
       ZMAG=ZSM
      ENDIF

      RETURN
      END
C
C=====================================================================================
C
       SUBROUTINE SMGSW_08 (XSM,YSM,ZSM,XGSW,YGSW,ZGSW,J)
C
C  CONVERTS SOLAR MAGNETIC (SM) TO GEOCENTRIC SOLAR-WIND (GSW) COORDINATES OR VICE VERSA.
C
C                  J>0                 J<0
C-----INPUT: J,XSM,YSM,ZSM        J,XGSW,YGSW,ZGSW
C----OUTPUT:  XGSW,YGSW,ZGSW       XSM,YSM,ZSM
C
C  ATTENTION:  SUBROUTINE RECALC_08 MUST BE INVOKED BEFORE SMGSW_08 IN THREE CASES:
C     /A/  BEFORE THE FIRST TRANSFORMATION OF COORDINATES
C     /B/  IF THE VALUES OF IYEAR,IDAY,IHOUR,MIN,ISEC HAVE BEEN CHANGED
C     /C/  IF THE VALUES OF COMPONENTS OF THE SOLAR WIND FLOW VELOCITY HAVE CHANGED
C
C    IMPORTANT NOTE:
C
C        A NON-STANDARD DEFINITION IS IMPLIED HERE FOR THE SOLAR MAGNETIC (SM) COORDINATE
C        SYSTEM:  IT IS ASSUMED THAT THE XSM AXIS LIES IN THE PLANE DEFINED BY THE
C        GEODIPOLE AXIS AND THE OBSERVED VECTOR OF THE SOLAR WIND FLOW (RATHER THAN
C        THE EARTH-SUN LINE).  IN ORDER TO CONVERT MAG COORDINATES TO AND FROM THE
C        STANDARD SM COORDINATES, INVOKE RECALC_08 WITH VGSEX=-400.0, VGSEY=0.0, VGSEZ=0.0
C
C     LAST MODIFICATION:    MARCH 21, 2008 (DOUBLE-PRECISION VERSION)
C
C     AUTHOR:  N. A. TSYGANENKO
C
      IMPLICIT REAL*8 (A-H,O-Z)
      COMMON /GEOPACK1/ A(10),SPS,CPS,B(22)

      IF (J.GT.0) THEN
       XGSW=XSM*CPS+ZSM*SPS
       YGSW=YSM
       ZGSW=ZSM*CPS-XSM*SPS
      ELSE
       XSM=XGSW*CPS-ZGSW*SPS
       YSM=YGSW
       ZSM=XGSW*SPS+ZGSW*CPS
      ENDIF

      RETURN
      END
C
C==========================================================================================
C
      SUBROUTINE GEOGSW_08 (XGEO,YGEO,ZGEO,XGSW,YGSW,ZGSW,J)
C
C CONVERTS GEOGRAPHIC (GEO) TO GEOCENTRIC SOLAR-WIND (GSW) COORDINATES OR VICE VERSA.
C
C                   J>0                   J<0
C----- INPUT:  J,XGEO,YGEO,ZGEO    J,XGSW,YGSW,ZGSW
C---- OUTPUT:    XGSW,YGSW,ZGSW      XGEO,YGEO,ZGEO
C
C  ATTENTION:  SUBROUTINE  RECALC_08  MUST BE INVOKED BEFORE GEOGSW_08 IN THREE CASES:
C     /A/  BEFORE THE FIRST TRANSFORMATION OF COORDINATES, OR
C     /B/  IF THE VALUES OF IYEAR,IDAY,IHOUR,MIN,ISEC  HAVE CHANGED, AND/OR
C     /C/  IF THE VALUES OF COMPONENTS OF THE SOLAR WIND FLOW VELOCITY HAVE CHANGED
C
C  NOTE: THIS SUBROUTINE CONVERTS GEO VECTORS TO AND FROM THE SOLAR-WIND GSW COORDINATE
C        SYSTEM, TAKING INTO ACCOUNT POSSIBLE DEFLECTIONS OF THE SOLAR WIND DIRECTION FROM
C        STRICTLY RADIAL.  BEFORE CONVERTING TO/FROM STANDARD GSM COORDINATES, INVOKE RECALC_08
C        WITH VGSEX=-400.0 and VGSEY=0.0, VGSEZ=0.0
C
C     LAST MODIFICATION:    MARCH 21, 2008 (DOUBLE-PRECISION VERSION)
C
C     AUTHOR:  N. A. TSYGANENKO
C
      IMPLICIT REAL*8 (A-H,O-Z)
      COMMON /GEOPACK1/ AA(16),A11,A21,A31,A12,A22,A32,A13,A23,A33,B(9)
C
      IF (J.GT.0) THEN
       XGSW=A11*XGEO+A12*YGEO+A13*ZGEO
       YGSW=A21*XGEO+A22*YGEO+A23*ZGEO
       ZGSW=A31*XGEO+A32*YGEO+A33*ZGEO
      ELSE
       XGEO=A11*XGSW+A21*YGSW+A31*ZGSW
       YGEO=A12*XGSW+A22*YGSW+A32*ZGSW
       ZGEO=A13*XGSW+A23*YGSW+A33*ZGSW
      ENDIF

      RETURN
      END
C
C=====================================================================================
C
      SUBROUTINE GEODGEO_08 (H,XMU,R,THETA,J)
C
C  THIS SUBROUTINE (1) CONVERTS VERTICAL LOCAL HEIGHT (ALTITUDE) H AND GEODETIC
C  LATITUDE XMU INTO GEOCENTRIC COORDINATES R AND THETA (GEOCENTRIC RADIAL
C  DISTANCE AND COLATITUDE, RESPECTIVELY; ALSO KNOWN AS ECEF COORDINATES),
C  AS WELL AS (2) PERFORMS THE INVERSE TRANSFORMATION FROM {R,THETA} TO {H,XMU}.
C
C  THE SUBROUTINE USES WORLD GEODETIC SYSTEM WGS84 PARAMETERS FOR THE EARTH'S
C  ELLIPSOID. THE ANGULAR QUANTITIES (GEO COLATITUDE THETA AND GEODETIC LATITUDE
C  XMU) ARE IN RADIANS, AND THE DISTANCES (GEOCENTRIC RADIUS R AND ALTITUDE H
C  ABOVE THE EARTH'S ELLIPSOID) ARE IN KILOMETERS.
C
C  IF J>0, THE TRANSFORMATION IS MADE FROM GEODETIC TO GEOCENTRIC COORDINATES
C   USING SIMPLE DIRECT EQUATIONS.
C  IF J<0, THE INVERSE TRANSFORMATION FROM GEOCENTRIC TO GEODETIC COORDINATES
C   IS MADE BY MEANS OF A FAST ITERATIVE ALGORITHM.
C
c-------------------------------------------------------------------------------
C                   J>0                     |            J<0
c-------------------------------------------|-----------------------------------
C--INPUT:   J        H          XMU         |    J         R          THETA
c         flag  altitude (km)  geodetic     |   flag   geocentric    spherical
c                              latitude     |         distance (km) colatitude
c                              (radians)    |                        (radians)
c-------------------------------------------|-----------------------------------
c                                           |
C----OUTPUT:         R           THETA      |          H              XMU
C                geocentric    spherical    |      altitude (km)    geodetic
C                distance (km) colatitude   |                       latitude
C                              (radians)    |                       (radians)
C-------------------------------------------------------------------------------
C
C   AUTHOR:  N. A. TSYGANENKO
c   DATE:    DEC 5, 2007
C   LAST MODIFICATION:    MARCH 21, 2008 (DOUBLE-PRECISION VERSION)
C
      IMPLICIT REAL*8 (A-H,O-Z)
      DATA R_EQ, BETA /6378.137D0, 6.73949674228D-3/
c
c  R_EQ is the semi-major axis of the Earth's ellipsoid, and BETA is its
c  second eccentricity squared
c
      DATA TOL /1.D-6/
c
c   Direct transformation (GEOD=>GEO):
c
      IF (J.GT.0) THEN
       COSXMU=DCOS(XMU)
       SINXMU=DSIN(XMU)
       DEN=DSQRT(COSXMU**2+(SINXMU/(1.0D0+BETA))**2)
       COSLAM=COSXMU/DEN
       SINLAM=SINXMU/(DEN*(1.0D0+BETA))
       RS=R_EQ/DSQRT(1.0D0+BETA*SINLAM**2)
       X=RS*COSLAM+H*COSXMU
       Z=RS*SINLAM+H*SINXMU
       R=DSQRT(X**2+Z**2)
       THETA=DACOS(Z/R)
      ENDIF

c
c   Inverse transformation (GEO=>GEOD):
c
      IF (J.LT.0) THEN
       N=0
       PHI=1.570796327D0-THETA
       PHI1=PHI
  1    SP=DSIN(PHI1)
       ARG=SP*(1.0D0+BETA)/DSQRT(1.0D0+BETA*(2.0D0+BETA)*SP**2)
       XMUS=DASIN(ARG)
       RS=R_EQ/DSQRT(1.0D0+BETA*DSIN(PHI1)**2)
       COSFIMS=DCOS(PHI1-XMUS)
       H=DSQRT((RS*COSFIMS)**2+R**2-RS**2)-RS*COSFIMS
       Z=RS*DSIN(PHI1)+H*DSIN(XMUS)
       X=RS*DCOS(PHI1)+H*DCOS(XMUS)
       RR=DSQRT(X**2+Z**2)
       DPHI=DASIN(Z/RR)-PHI
       PHI1=PHI1-DPHI
       N=N+1
       IF (DABS(DPHI).GT.TOL.AND.N.LT.100) GOTO 1
       XMU=XMUS
      ENDIF

      RETURN
      END
C
C=====================================================================================
C
      SUBROUTINE RHAND_08 (X,Y,Z,R1,R2,R3,IOPT,PARMOD,EXNAME,INNAME)
C
C  CALCULATES THE COMPONENTS OF THE RIGHT HAND SIDE VECTOR IN THE GEOMAGNETIC FIELD
C    LINE EQUATION  (a subsidiary subroutine for the subroutine STEP_08)
C
C     LAST MODIFICATION:    MARCH 21, 2008 (DOUBLE-PRECISION VERSION)
C
C     AUTHOR:  N. A. TSYGANENKO
C
      IMPLICIT REAL*8 (A-H,O-Z)
      DIMENSION PARMOD(10)
C
C     EXNAME AND INNAME ARE NAMES OF SUBROUTINES FOR THE EXTERNAL AND INTERNAL
C     PARTS OF THE TOTAL FIELD, E.G., T96_01 AND IGRF_GSW_08
C
      COMMON /GEOPACK1/ A(12),DS3,BB(2),PSI,CC(18)

      CALL EXNAME (IOPT,PARMOD,PSI,X,Y,Z,BXGSW,BYGSW,BZGSW)
      CALL INNAME (X,Y,Z,HXGSW,HYGSW,HZGSW)

      BX=BXGSW+HXGSW
      BY=BYGSW+HYGSW
      BZ=BZGSW+HZGSW
      B=DS3/DSQRT(BX**2+BY**2+BZ**2)
      R1=BX*B
      R2=BY*B
      R3=BZ*B
      RETURN
      END
C
C===================================================================================
C
      SUBROUTINE STEP_08(X,Y,Z,DS,DSMAX,ERRIN,IOPT,PARMOD,EXNAME,INNAME)
C
C   RE-CALCULATES THE INPUT VALUES {X,Y,Z} (IN GSW COORDINATES) FOR ANY POINT ON A FIELD LINE,
C     BY MAKING A STEP ALONG THAT LINE USING RUNGE-KUTTA-MERSON ALGORITHM (G.N. Lance, Numerical
C      methods for high-speed computers, Iliffe & Sons, London 1960.)
C   DS IS A PRESCRIBED VALUE OF THE CURRENT STEP SIZE, DSMAX IS ITS UPPER LIMIT.
C   ERRIN IS A PERMISSIBLE ERROR (ITS OPTIMAL VALUE SPECIFIED IN THE S/R TRACE_08)
C     IF THE ACTUAL ERROR (ERRCUR) AT THE CURRENT STEP IS LARGER THAN ERRIN, THE STEP IS REJECTED,
C       AND THE CALCULATION IS REPEATED ANEW WITH HALVED STEPSIZE DS.
C     IF ERRCUR IS SMALLER THAN ERRIN, THE STEP IS ACCEPTED, AND THE CURRENT VALUE OF DS IS RETAINED
C       FOR THE NEXT STEP.
C     IF ERRCUR IS SMALLER THAN 0.04*ERRIN, THE STEP IS ACCEPTED, AND THE VALUE OF DS FOR THE NEXT STEP
C       IS INCREASED BY THE FACTOR 1.5, BUT NOT LARGER THAN DSMAX.
C   IOPT IS A FLAG, RESERVED FOR SPECIFYNG A VERSION OF THE EXTERNAL FIELD MODEL EXNAME.
C   ARRAY PARMOD(10) CONTAINS INPUT PARAMETERS FOR THE MODEL EXNAME.
C   EXNAME IS THE NAME OF THE SUBROUTINE FOR THE EXTERNAL FIELD MODEL.
C   INNAME IS THE NAME OF THE SUBROUTINE FOR THE INTERNAL FIELD MODEL (EITHER DIP_08 OR IGRF_GSW_08)
C
C   ALL THE ABOVE PARAMETERS ARE INPUT ONES; OUTPUT IS THE RECALCULATED VALUES OF X,Y,Z
C
C     LAST MODIFICATION:    APRIL 21, 2008 (SEE ERRATA AS OF THAT DATE)
C
C     AUTHOR:  N. A. TSYGANENKO
C
      IMPLICIT REAL*8 (A-H,O-Z)
      DIMENSION PARMOD(10)
      COMMON /GEOPACK1/ A(12),DS3,B(21)
      EXTERNAL EXNAME,INNAME

  1   DS3=-DS/3.D0
      CALL RHAND_08 (X,Y,Z,R11,R12,R13,IOPT,PARMOD,EXNAME,INNAME)
      CALL RHAND_08 (X+R11,Y+R12,Z+R13,R21,R22,R23,IOPT,PARMOD,EXNAME,
     * INNAME)
      CALL RHAND_08 (X+.5D0*(R11+R21),Y+.5D0*(R12+R22),Z+.5D0*
     *(R13+R23),R31,R32,R33,IOPT,PARMOD,EXNAME,INNAME)
      CALL RHAND_08 (X+.375D0*(R11+3.D0*R31),Y+.375D0*(R12+3.D0*R32
     *),Z+.375D0*(R13+3.D0*R33),R41,R42,R43,IOPT,PARMOD,EXNAME,INNAME)
      CALL RHAND_08 (X+1.5D0*(R11-3.D0*R31+4.D0*R41),Y+1.5D0*(R12-
     *3.D0*R32+4.D0*R42),Z+1.5D0*(R13-3.D0*R33+4.D0*R43),
     *R51,R52,R53,IOPT,PARMOD,EXNAME,INNAME)
      ERRCUR=DABS(R11-4.5D0*R31+4.D0*R41-.5D0*R51)+DABS(R12-4.5D0*R32
     *+4.D0*R42-.5D0*R52)+DABS(R13-4.5D0*R33+4.D0*R43-.5D0*R53)
C
C  READY FOR MAKING THE STEP, BUT CHECK THE ACCURACY; IF INSUFFICIENT,
C   REPEAT THE STEP WITH HALVED STEPSIZE:
C
      IF (ERRCUR.GT.ERRIN) THEN
      DS=DS*.5D0
      GOTO 1
      ENDIF
C
C  ACCURACY IS ACCEPTABLE, BUT CHECK IF THE STEPSIZE IS NOT TOO LARGE;
C    OTHERWISE REPEAT THE STEP WITH DS=DSMAX
C
      IF (DABS(DS).GT.DSMAX) THEN
      DS=DSIGN(DSMAX,DS)
      GOTO 1
      ENDIF
C
C  MAKING THE STEP:
C
  2   X=X+.5D0*(R11+4.D0*R41+R51)
      Y=Y+.5D0*(R12+4.D0*R42+R52)
      Z=Z+.5D0*(R13+4.D0*R43+R53)
C
C  IF THE ACTUAL ERROR IS TOO SMALL (LESS THAN 4% OF ERRIN) AND DS SMALLER
C   THAN DSMAX/1.5, THEN WE INCREASE THE STEPSIZE FOR THE NEXT STEP BY 50%
C
      IF(ERRCUR.LT.ERRIN*.04D0.AND.DS.LT.DSMAX/1.5D0) DS=DS*1.5D0
      RETURN
      END
C
C==============================================================================
C
      SUBROUTINE TRACE_08 (XI,YI,ZI,DIR,DSMAX,ERR,RLIM,R0,IOPT,PARMOD,
     * EXNAME,INNAME,XF,YF,ZF,XX,YY,ZZ,L,LMAX)
C
C  TRACES A FIELD LINE FROM AN ARBITRARY POINT OF SPACE TO THE EARTH'S
C  SURFACE OR TO A MODEL LIMITING BOUNDARY.
C
C  THIS SUBROUTINE ALLOWS TWO OPTIONS:
C
C  (1) IF INNAME=IGRF_GSW_08, THEN THE IGRF MODEL WILL BE USED FOR CALCULATING
C      CONTRIBUTION FROM EARTH'S INTERNAL SOURCES. IN THIS CASE, SUBROUTINE
C      RECALC_08 MUST BE CALLED BEFORE USING TRACE_08, WITH PROPERLY SPECIFIED DATE,
C      UNIVERSAL TIME, AND SOLAR WIND VELOCITY COMPONENTS, TO CALCULATE IN ADVANCE
C      ALL QUANTITIES NEEDED FOR THE MAIN FIELD MODEL AND FOR TRANSFORMATIONS
C      BETWEEN INVOLVED COORDINATE SYSTEMS.
C
C  (2) IF INNAME=DIP_08, THEN A PURE DIPOLE FIELD WILL BE USED INSTEAD OF THE IGRF MODEL.
C      IN THIS CASE, THE SUBROUTINE RECALC_08 MUST ALSO BE CALLED BEFORE TRACE_08.
C      HERE ONE CAN CHOOSE EITHER TO
C      (a) CALCULATE DIPOLE TILT ANGLE BASED ON DATE, TIME, AND SOLAR WIND DIRECTION,
C   OR (b) EXPLICITLY SPECIFY THAT ANGLE, WITHOUT ANY REFERENCE TO DATE/UT/SOLAR WIND.
C      IN THE LAST CASE (b), THE SINE (SPS) AND COSINE (CPS) OF THE DIPOLE TILT
C      ANGLE MUST BE SPECIFIED IN ADVANCE (BUT AFTER HAVING CALLED RECALC_08) AND FORWARDED
C      IN THE COMMON BLOCK /GEOPACK1/ (IN ITS 11th AND 12th ELEMENTS, RESPECTIVELY).
C      IN THIS CASE THE ROLE OF THE SUBROUTINE RECALC_08 IS REDUCED TO ONLY CALCULATING
C      THE COMPONENTS OF THE EARTH'S DIPOLE MOMENT.
C
C------------- INPUT PARAMETERS:
C
C   XI,YI,ZI - GSW COORDS OF THE FIELD LINE STARTING POINT (IN EARTH RADII, 1 RE = 6371.2 km),
C
C   DIR - SIGN OF THE TRACING DIRECTION: IF DIR=1.0 THEN THE TRACING IS MADE ANTIPARALLEL
C     TO THE TOTAL FIELD VECTOR (E.G., FROM NORTHERN TO SOUTHERN CONJUGATE POINT);
C     IF DIR=-1.0 THEN THE TRACING PROCEEDS IN THE OPPOSITE DIRECTION, THAT IS, PARALLEL TO
C     THE TOTAL FIELD VECTOR.
C
C   DSMAX - UPPER LIMIT ON THE STEPSIZE (SETS A DESIRED MAXIMAL SPACING BETWEEN
C                 THE FIELD LINE POINTS)
C
C   ERR - PERMISSIBLE STEP ERROR. A REASONABLE ESTIMATE PROVIDING A SUFFICIENT ACCURACY FOR MOST
C         APPLICATIONS IS ERR=0.0001. SMALLER/LARGER VALUES WILL RESULT IN LARGER/SMALLER NUMBER
C         OF STEPS AND, HENCE, OF OUTPUT FIELD LINE POINTS. NOTE THAT USING MUCH SMALLER VALUES
C         OF ERR MAY REQUIRE USING A DOUBLE PRECISION VERSION OF THE ENTIRE PACKAGE.
C
C   R0 -  RADIUS OF A SPHERE (IN RE), DEFINING THE INNER BOUNDARY OF THE TRACING REGION
C         (USUALLY, EARTH'S SURFACE OR THE IONOSPHERE, WHERE R0~1.0)
C         IF THE FIELD LINE REACHES THAT SPHERE FROM OUTSIDE, ITS INBOUND TRACING IS
C         TERMINATED AND THE CROSSING POINT COORDINATES XF,YF,ZF  ARE CALCULATED.
C
C   RLIM - RADIUS OF A SPHERE (IN RE), DEFINING THE OUTER BOUNDARY OF THE TRACING REGION;
C         IF THE FIELD LINE REACHES THAT BOUNDARY FROM INSIDE, ITS OUTBOUND TRACING IS
C         TERMINATED AND THE CROSSING POINT COORDINATES XF,YF,ZF ARE CALCULATED.
C
C   IOPT - A MODEL INDEX; CAN BE USED FOR SPECIFYING A VERSION OF THE EXTERNAL FIELD
C       MODEL (E.G., A NUMBER OF THE KP-INDEX INTERVAL). ALTERNATIVELY, ONE CAN USE THE ARRAY
C       PARMOD FOR THAT PURPOSE (SEE BELOW); IN THAT CASE IOPT IS JUST A DUMMY PARAMETER.
C
C   PARMOD -  A 10-ELEMENT ARRAY CONTAINING INPUT PARAMETERS NEEDED FOR A UNIQUE
C      SPECIFICATION OF THE EXTERNAL FIELD MODEL. THE CONCRETE MEANING OF THE COMPONENTS
C      OF PARMOD DEPENDS ON A SPECIFIC VERSION OF THAT MODEL.
C
C   EXNAME - NAME OF A SUBROUTINE PROVIDING COMPONENTS OF THE EXTERNAL MAGNETIC FIELD
C    (E.G., T89, OR T96_01, ETC.).
C   INNAME - NAME OF A SUBROUTINE PROVIDING COMPONENTS OF THE INTERNAL MAGNETIC FIELD
C    (EITHER DIP_08 OR IGRF_GSW_08).
C
C   LMAX - MAXIMAL LENGTH OF THE ARRAYS XX,YY,ZZ, IN WHICH COORDINATES OF THE FIELD
C          LINE POINTS ARE STORED. LMAX SHOULD BE SET EQUAL TO THE ACTUAL LENGTH OF
C          THE ARRAYS, DEFINED IN THE MAIN PROGRAM AS ACTUAL ARGUMENTS OF THIS SUBROUTINE.
C
C-------------- OUTPUT PARAMETERS:
C
C   XF,YF,ZF - GSW COORDINATES OF THE ENDPOINT OF THE TRACED FIELD LINE.
C   XX,YY,ZZ - ARRAYS OF LENGTH LMAX, CONTAINING COORDINATES OF THE FIELD LINE POINTS.
C   L - ACTUAL NUMBER OF FIELD LINE POINTS, GENERATED BY THIS SUBROUTINE.
C
C ----------------------------------------------------------
C
C     LAST MODIFICATION:    MARCH 21, 2008 (DOUBLE-PRECISION VERSION)
C
C     AUTHOR:  N. A. TSYGANENKO
C
      IMPLICIT REAL*8 (A-H,O-Z)
      DIMENSION XX(LMAX),YY(LMAX),ZZ(LMAX), PARMOD(10)
      COMMON /GEOPACK1/ AA(12),DD,BB(21)
      EXTERNAL EXNAME,INNAME
C
      L=0
      NREV=0
      DD=DIR
C
C  INITIALIZE THE STEP SIZE AND STARTING PONT:
C
      DS=0.5D0*DIR
      X=XI
      Y=YI
      Z=ZI
c
c  here we call RHAND_08 just to find out the sign of the radial component of the field
c   vector, and to determine the initial direction of the tracing (i.e., either away
c   or towards Earth):
c
      CALL RHAND_08 (X,Y,Z,R1,R2,R3,IOPT,PARMOD,EXNAME,INNAME)
      AD=0.01D0
      IF (X*R1+Y*R2+Z*R3.LT.0.D0) AD=-0.01D0
C
c     |AD|=0.01 and its sign follows the rule:
c (1) if DIR=1 (tracing antiparallel to B vector) then the sign of AD is the same as of Br
c (2) if DIR=-1 (tracing parallel to B vector) then the sign of AD is opposite to that of Br
c     AD is defined in order to initialize the value of RR (radial distance at previous step):

      RR=DSQRT(X**2+Y**2+Z**2)+AD
c
  1   L=L+1
      IF(L.GT.LMAX) GOTO 7
      XX(L)=X
      YY(L)=Y
      ZZ(L)=Z
      RYZ=Y**2+Z**2
      R2=X**2+RYZ
      R=DSQRT(R2)
C
c  check if the line hit the outer tracing boundary; if yes, then terminate
c   the tracing (label 8). The outer boundary is assumed reached, when the line
c   crosses any of the 3 surfaces: (1) a sphere R=RLIM, (2) a cylinder of radius 40Re,
c   coaxial with the XGSW axis, (3) the plane X=20Re:

      IF (R.GT.RLIM.OR.RYZ.GT.1600.D0.OR.X.GT.20.D0) GOTO 8
c
c  check whether or not the inner tracing boundary was crossed from outside,
c  if yes, then calculate the footpoint position by interpolation (go to label 6):
c
      IF (R.LT.R0.AND.RR.GT.R) GOTO 6

c  check if we are moving outward, or R is still larger than 3Re; if yes, proceed further:
c
      IF (R.GE.RR.OR.R.GE.3.D0) GOTO 4
c
c  now we entered inside the sphere R=3: to avoid too large steps (and hence
c  inaccurate interpolated position of the footpoint), enforce the progressively
c  smaller stepsize values as we approach the inner boundary R=R0:
c
      FC=0.2D0
      IF(R-R0.LT.0.05D0) FC=0.05D0
      AL=FC*(R-R0+0.2D0)
      DS=DIR*AL
c
  4   XR=X
      YR=Y
      ZR=Z
c
      DRP=R-RR
      RR=R
c
      CALL STEP_08 (X,Y,Z,DS,DSMAX,ERR,IOPT,PARMOD,EXNAME,INNAME)
c
C  check the total number NREV of changes in the tracing radial direction; (NREV.GT.2) means
c   that the line started making multiple loops, in which case we stop the process:
C
      R=DSQRT(X**2+Y**2+Z**2)
      DR=R-RR
      IF (DRP*DR.LT.0.D0) NREV=NREV+1
      IF (NREV.GT.2) GOTO 8
C
      GOTO 1
c
c  find the footpoint position by interpolating between the current and previous
c   field line points:
c
  6   R1=(R0-R)/(RR-R)
      X=X-(X-XR)*R1
      Y=Y-(Y-YR)*R1
      Z=Z-(Z-ZR)*R1
      GOTO 8
  7   continue
c      WRITE (*,10)
      L=LMAX
  8   XF=X
      YF=Y
      ZF=Z
C
C  replace the coordinates of the last (L-th) point in the XX,YY,ZZ arrays
C   so that they correspond to the estimated footpoint position {XF,YF,ZF},
c   satisfying:  sqrt(XF**2+YF**2+ZF**2}=R0
C
      XX(L)=XF
      YY(L)=YF
      ZZ(L)=ZF
C
      RETURN
 10   FORMAT(//,1X,'**** COMPUTATIONS IN THE SUBROUTINE TRACE_08 ARE',
     *' TERMINATED: THE NUMBER OF POINTS EXCEEDED LMAX ****'//)
      END
c
C====================================================================================
C
      SUBROUTINE SHUETAL_MGNP_08(XN_PD,VEL,BZIMF,XGSW,YGSW,ZGSW,
     *  XMGNP,YMGNP,ZMGNP,DIST,ID)
C
C  FOR ANY POINT OF SPACE WITH COORDINATES (XGSW,YGSW,ZGSW) AND SPECIFIED CONDITIONS
C  IN THE INCOMING SOLAR WIND, THIS SUBROUTINE:
C
C (1) DETERMINES IF THE POINT (XGSW,YGSW,ZGSW) LIES INSIDE OR OUTSIDE THE
C      MODEL MAGNETOPAUSE OF SHUE ET AL. (JGR-A, V.103, P. 17691, 1998).
C
C (2) CALCULATES THE GSW POSITION OF A POINT {XMGNP,YMGNP,ZMGNP}, LYING AT THE MODEL
C      MAGNETOPAUSE AND ASYMPTOTICALLY TENDING TO THE NEAREST BOUNDARY POINT WITH
C      RESPECT TO THE OBSERVATION POINT {XGSW,YGSW,ZGSW}, AS IT APPROACHES THE MAGNETO-
C      PAUSE.
C
C  INPUT: XN_PD - EITHER SOLAR WIND PROTON NUMBER DENSITY (PER C.C.) (IF VEL>0)
C                    OR THE SOLAR WIND RAM PRESSURE IN NANOPASCALS   (IF VEL<0)
C         BZIMF - IMF BZ IN NANOTESLAS
C
C         VEL - EITHER SOLAR WIND VELOCITY (KM/SEC)
C                  OR ANY NEGATIVE NUMBER, WHICH INDICATES THAT XN_PD STANDS
C                     FOR THE SOLAR WIND PRESSURE, RATHER THAN FOR THE DENSITY
C
C         XGSW,YGSW,ZGSW - GSW POSITION OF THE OBSERVATION POINT IN EARTH RADII
C
C  OUTPUT: XMGNP,YMGNP,ZMGNP - GSW POSITION OF THE BOUNDARY POINT
C          DIST - DISTANCE (IN RE) BETWEEN THE OBSERVATION POINT (XGSW,YGSW,ZGSW)
C                 AND THE MODEL NAGNETOPAUSE
C          ID -  POSITION FLAG:  ID=+1 (-1) MEANS THAT THE OBSERVATION POINT
C          LIES INSIDE (OUTSIDE) OF THE MODEL MAGNETOPAUSE, RESPECTIVELY.
C
C  OTHER SUBROUTINES USED: T96_MGNP_08
C
c          AUTHOR:  N.A. TSYGANENKO,
C          DATE:    APRIL 4, 2003.
C          LAST MODIFICATION:    MARCH 21, 2008 (DOUBLE-PRECISION VERSION)
C
      IMPLICIT REAL*8 (A-H,O-Z)
C
      IF (VEL.LT.0.) THEN
        P=XN_PD
      ELSE
        P=1.94D-6*XN_PD*VEL**2  ! P IS THE SOLAR WIND DYNAMIC PRESSURE (IN nPa)
      ENDIF

c
c  DEFINE THE ANGLE PHI, MEASURED DUSKWARD FROM THE NOON-MIDNIGHT MERIDIAN PLANE;
C  IF THE OBSERVATION POINT LIES ON THE X AXIS, THE ANGLE PHI CANNOT BE UNIQUELY
C  DEFINED, AND WE SET IT AT ZERO:
c
      IF (YGSW.NE.0.D0.OR.ZGSW.NE.0.D0) THEN
         PHI=DATAN2(YGSW,ZGSW)
      ELSE
         PHI=0.D0
      ENDIF
C
C  FIRST, FIND OUT IF THE OBSERVATION POINT LIES INSIDE THE SHUE ET AL BDRY
C  AND SET THE VALUE OF THE ID FLAG:
C
      ID=-1
      R0=(10.22D0+1.29D0*DTANH(.184D0*(BZIMF+8.14D0)))*P**(-.15151515D0)
      ALPHA=(0.58D0-0.007D0*BZIMF)*(1.D0+0.024*DLOG(P))
      R=DSQRT(XGSW**2+YGSW**2+ZGSW**2)
      RM=R0*(2.D0/(1.D0+XGSW/R))**ALPHA
      IF (R.LE.RM) ID=+1
C
C  NOW, FIND THE CORRESPONDING T96 MAGNETOPAUSE POSITION, TO BE USED AS
C  A STARTING APPROXIMATION IN THE SEARCH OF A CORRESPONDING SHUE ET AL.
C  BOUNDARY POINT:
C
      CALL T96_MGNP_08(P,-1.D0,XGSW,YGSW,ZGSW,XMT96,YMT96,ZMT96,DIST,
     *  ID96)
C
      RHO2=YMT96**2+ZMT96**2
      R=DSQRT(RHO2+XMT96**2)
      ST=DSQRT(RHO2)/R
      CT=XMT96/R
C
C  NOW, USE NEWTON'S ITERATIVE METHOD TO FIND THE NEAREST POINT AT THE
C   SHUE ET AL.'S BOUNDARY:
C
      NIT=0

  1   T=DATAN2(ST,CT)
      RM=R0*(2.D0/(1.D0+CT))**ALPHA

      F=R-RM
      GRADF_R=1.D0
      GRADF_T=-ALPHA/R*RM*ST/(1.D0+CT)
      GRADF=DSQRT(GRADF_R**2+GRADF_T**2)

      DR=-F/GRADF**2
      DT= DR/R*GRADF_T

      R=R+DR
      T=T+DT
      ST=DSIN(T)
      CT=DCOS(T)

      DS=DSQRT(DR**2+(R*DT)**2)

      NIT=NIT+1

c      IF (NIT.GT.1000) THEN
c         PRINT *,
c     *' BOUNDARY POINT COULD NOT BE FOUND; ITERATIONS DO NOT CONVERGE'
c      ENDIF

      IF (DS.GT.1.D-4) GOTO 1

      XMGNP=R*DCOS(T)
      RHO=  R*DSIN(T)

      YMGNP=RHO*DSIN(PHI)
      ZMGNP=RHO*DCOS(PHI)

      DIST=DSQRT((XGSW-XMGNP)**2+(YGSW-YMGNP)**2+(ZGSW-ZMGNP)**2)

      RETURN
      END
C
C=======================================================================================
C
      SUBROUTINE T96_MGNP_08(XN_PD,VEL,XGSW,YGSW,ZGSW,XMGNP,YMGNP,ZMGNP,
     * DIST,ID)
C
C  FOR ANY POINT OF SPACE WITH GIVEN COORDINATES (XGSW,YGSW,ZGSW), THIS SUBROUTINE DEFINES
C  THE POSITION OF A POINT (XMGNP,YMGNP,ZMGNP) AT THE T96 MODEL MAGNETOPAUSE WITH THE
C  SAME VALUE OF THE ELLIPSOIDAL TAU-COORDINATE, AND THE DISTANCE BETWEEN THEM.  THIS IS
C  NOT THE SHORTEST DISTANCE D_MIN TO THE BOUNDARY, BUT DIST ASYMPTOTICALLY TENDS TO D_MIN,
C  AS THE OBSERVATION POINT GETS CLOSER TO THE MAGNETOPAUSE.
C
C  INPUT: XN_PD - EITHER SOLAR WIND PROTON NUMBER DENSITY (PER C.C.) (IF VEL>0)
C                    OR THE SOLAR WIND RAM PRESSURE IN NANOPASCALS   (IF VEL<0)
C         VEL - EITHER SOLAR WIND VELOCITY (KM/SEC)
C                  OR ANY NEGATIVE NUMBER, WHICH INDICATES THAT XN_PD STANDS
C                     FOR THE SOLAR WIND PRESSURE, RATHER THAN FOR THE DENSITY
C
C         XGSW,YGSW,ZGSW - COORDINATES OF THE OBSERVATION POINT IN EARTH RADII
C
C  OUTPUT: XMGNP,YMGNP,ZMGNP - GSW POSITION OF THE BOUNDARY POINT, HAVING THE SAME
C          VALUE OF TAU-COORDINATE AS THE OBSERVATION POINT (XGSW,YGSW,ZGSW)
C          DIST -  THE DISTANCE BETWEEN THE TWO POINTS, IN RE,
C          ID -    POSITION FLAG; ID=+1 (-1) MEANS THAT THE POINT (XGSW,YGSW,ZGSW)
C          LIES INSIDE (OUTSIDE) THE MODEL MAGNETOPAUSE, RESPECTIVELY.
C
C  THE PRESSURE-DEPENDENT MAGNETOPAUSE IS THAT USED IN THE T96_01 MODEL
C  (TSYGANENKO, JGR, V.100, P.5599, 1995; ESA SP-389, P.181, OCT. 1996)
C
c   AUTHOR:  N.A. TSYGANENKO
C   DATE:    AUG.1, 1995, REVISED APRIL 3, 2003.
C   LAST MODIFICATION:    MARCH 21, 2008 (DOUBLE-PRECISION VERSION)
C
C  DEFINE SOLAR WIND DYNAMIC PRESSURE (NANOPASCALS, ASSUMING 4% OF ALPHA-PARTICLES),
C   IF NOT EXPLICITLY SPECIFIED IN THE INPUT:
C
      IMPLICIT REAL*8 (A-H,O-Z)
C
      IF (VEL.LT.0.D0) THEN
       PD=XN_PD
      ELSE
       PD=1.94D-6*XN_PD*VEL**2
C
      ENDIF
C
C  RATIO OF PD TO THE AVERAGE PRESSURE, ASSUMED EQUAL TO 2 nPa:

      RAT=PD/2.0D0
      RAT16=RAT**0.14D0

C (THE POWER INDEX 0.14 IN THE SCALING FACTOR IS THE BEST-FIT VALUE OBTAINED FROM DATA
C    AND USED IN THE T96_01 VERSION)
C
C  VALUES OF THE MAGNETOPAUSE PARAMETERS FOR  PD = 2 nPa:
C
      A0=70.D0
      S00=1.08D0
      X00=5.48D0
C
C   VALUES OF THE MAGNETOPAUSE PARAMETERS, SCALED BY THE ACTUAL PRESSURE:
C
      A=A0/RAT16
      S0=S00
      X0=X00/RAT16
      XM=X0-A
C
C  (XM IS THE X-COORDINATE OF THE "SEAM" BETWEEN THE ELLIPSOID AND THE CYLINDER)
C
C     (FOR DETAILS OF THE ELLIPSOIDAL COORDINATES, SEE THE PAPER:
C      N.A.TSYGANENKO, SOLUTION OF CHAPMAN-FERRARO PROBLEM FOR AN
C      ELLIPSOIDAL MAGNETOPAUSE, PLANET.SPACE SCI., V.37, P.1037, 1989).
C
       IF (YGSW.NE.0.D0.OR.ZGSW.NE.0.D0) THEN
          PHI=DATAN2(YGSW,ZGSW)
       ELSE
          PHI=0.D0
       ENDIF
C
       RHO=DSQRT(YGSW**2+ZGSW**2)
C
       IF (XGSW.LT.XM) THEN
           XMGNP=XGSW
           RHOMGNP=A*DSQRT(S0**2-1.D0)
           YMGNP=RHOMGNP*DSIN(PHI)
           ZMGNP=RHOMGNP*DCOS(PHI)
           DIST=DSQRT((XGSW-XMGNP)**2+(YGSW-YMGNP)**2+(ZGSW-ZMGNP)**2)
           IF (RHOMGNP.GT.RHO) ID=+1
           IF (RHOMGNP.LE.RHO) ID=-1
           RETURN
       ENDIF
C
          XKSI=(XGSW-X0)/A+1.D0
          XDZT=RHO/A
          SQ1=DSQRT((1.D0+XKSI)**2+XDZT**2)
          SQ2=DSQRT((1.D0-XKSI)**2+XDZT**2)
          SIGMA=0.5D0*(SQ1+SQ2)
          TAU=0.5D0*(SQ1-SQ2)
C
C  NOW CALCULATE (X,Y,Z) FOR THE CLOSEST POINT AT THE MAGNETOPAUSE
C
          XMGNP=X0-A*(1.D0-S0*TAU)
          ARG=(S0**2-1.D0)*(1.D0-TAU**2)
          IF (ARG.LT.0.D0) ARG=0.D0
          RHOMGNP=A*DSQRT(ARG)
          YMGNP=RHOMGNP*DSIN(PHI)
          ZMGNP=RHOMGNP*DCOS(PHI)
C
C  NOW CALCULATE THE DISTANCE BETWEEN THE POINTS {XGSW,YGSW,ZGSW} AND {XMGNP,YMGNP,ZMGNP}:
C   (IN GENERAL, THIS IS NOT THE SHORTEST DISTANCE D_MIN, BUT DIST ASYMPTOTICALLY TENDS
C    TO D_MIN, AS WE ARE GETTING CLOSER TO THE MAGNETOPAUSE):
C
      DIST=DSQRT((XGSW-XMGNP)**2+(YGSW-YMGNP)**2+(ZGSW-ZMGNP)**2)
C
      IF (SIGMA.GT.S0) ID=-1   !  ID=-1 MEANS THAT THE POINT LIES OUTSIDE
      IF (SIGMA.LE.S0) ID=+1   !  ID=+1 MEANS THAT THE POINT LIES INSIDE
C                                           THE MAGNETOSPHERE
      RETURN
      END
C
C===================================================================================
C
c




C*******************************************************************************************
C              TSYGANENKO 96



c
      SUBROUTINE T96_01(IOPT,PARMOD,PS,X,Y,Z,BX,BY,BZ)
C
c     RELEASE DATE OF THIS VERSION:   JUNE 22, 1996.

C----------------------------------------------------------------------
C
C  WITH TWO CORRECTIONS, SUGGESTED BY T.SOTIRELIS' COMMENTS (APR.7, 1997)
C
C  (1) A "STRAY "  CLOSING PARENTHESIS WAS REMOVED IN THE S/R	R2_BIRK
C  (2) A 0/0 PROBLEM ON THE Z-AXIS WAS SIDESTEPPED (LINES 44-46 OF THE
c	DOUBLE PRECISION FUNCTION XKSI)
c--------------------------------------------------------------------
C DATA-BASED MODEL CALIBRATED BY (1) SOLAR WIND PRESSURE PDYN (NANOPASCALS),
C	    (2) DST (NANOTESLA),  
C	    (3) BYIMF, AND (4) BZIMF (NANOTESLA) in GSM system.
c THESE INPUT PARAMETERS SHOULD BE PLACED IN THE FIRST 4 ELEMENTS
c OF THE ARRAY PARMOD(10).
C
C   THE REST OF THE INPUT VARIABLES ARE: THE GEODIPOLE TILT ANGLE PS (RADIANS),
C AND	X,Y,Z -  GSM POSITION (RE)
C
c   IOPT  IS JUST A DUMMY INPUT PARAMETER, NECESSARY TO MAKE THIS SUBROUTINE
C COMPATIBLE WITH THE NEW RELEASE (APRIL 1996) OF THE TRACING SOFTWARE
C PACKAGE (GEOPACK). IOPT VALUE DOES NOT AFFECT THE OUTPUT FIELD.
c
C
c OUTPUT:  GSM COMPONENTS OF THE EXTERNAL MAGNETIC FIELD (BX,BY,BZ, nanotesla)
C	     COMPUTED AS A SUM OF CONTRIBUTIONS FROM PRINCIPAL FIELD SOURCES
C
c  (C) Copr. 1995, 1996, Nikolai A. Tsyganenko, Raytheon STX, Code 695, NASA GSFC
c      Greenbelt, MD 20771, USA
c
C			     REFERENCES:
C
C		(1) N.A. TSYGANENKO AND D.P. STERN, A NEW-GENERATION GLOBAL
C	    MAGNETOSPHERE FIELD MODEL  , BASED ON SPACECRAFT MAGNETOMETER DATA,
C	    ISTP NEWSLETTER, V.6, NO.1, P.21, FEB.1996.
C
c	       (2) N.A.TSYGANENKO,  MODELING THE EARTH'S MAGNETOSPHERIC
C	    MAGNETIC FIELD CONFINED WITHIN A REALISTIC MAGNETOPAUSE,
C	    J.GEOPHYS.RES., V.100, P. 5599, 1995.
C
C	       (3) N.A. TSYGANENKO AND M.PEREDO, ANALYTICAL MODELS OF THE
C	    MAGNETIC FIELD OF DISK-SHAPED CURRENT SHEETS, J.GEOPHYS.RES.,
C	    V.99, P. 199, 1994.
C
c----------------------------------------------------------------------

      IMPLICIT REAL*8 (A-H,O-Z)
      REAL*8 PDYN,DST,BYIMF,BZIMF,PS,X,Y,Z,BX,BY,BZ,QX,QY,QZ,PARMOD(10),
     *   A(9)
c
      DATA PDYN0,EPS10 /2.,3630.7/
C
      DATA A/1.162,22.344,18.50,2.602,6.903,5.287,0.5790,0.4462,0.7850/
C
      DATA  AM0,S0,X00,DSIG04/70.,1.08,5.48,0.005/
      DATA  DELIMFX,DELIMFY /20.,10./
C
       PDYN=PARMOD(1)
       DST=PARMOD(2)
       BYIMF=PARMOD(3)
       BZIMF=PARMOD(4)
C
       SPS=DSIN(PS)		
       PPS=PS
C
       DEPR=0.8*DST-13.*DSQRT(PDYN)  !  DEPR is an estimate of total near-Earth
c					  depression, based on DST and Pdyn
c					      (usually, DEPR < 0 )
C
C  CALCULATE THE IMF-RELATED QUANTITIES:
C
       Bt=DSQRT(BYIMF**2+BZIMF**2)

       IF (BYIMF.EQ.0..AND.BZIMF.EQ.0.) THEN
          THETA=0.
          GOTO 1
       ENDIF
C
       THETA=DATAN2(BYIMF,BZIMF)
       IF (THETA.LE.0.D0) THETA=THETA+6.2831853
  1    CT=DCOS(THETA)
       ST=DSIN(THETA)
       EPS=718.5*DSQRT(Pdyn)*Bt*DSIN(THETA/2.)
C
       FACTEPS=EPS/EPS10-1.
       FACTPD=DSQRT(PDYN/PDYN0)-1.
C
       RCAMPL=-A(1)*DEPR     !   RCAMPL is the amplitude of the ring current
c		   (positive and equal to abs.value of RC depression at origin)
C
       TAMPL2=A(2)+A(3)*FACTPD+A(4)*FACTEPS
       TAMPL3=A(5)+A(6)*FACTPD
       B1AMPL=A(7)+A(8)*FACTEPS
       B2AMPL=20.*B1AMPL  ! IT IS EQUIVALENT TO ASSUMING THAT THE TOTAL CURRENT
C			    IN THE REGION 2 SYSTEM IS 40% OF THAT IN REGION 1
       RECONN=A(9)
C
       XAPPA=(PDYN/PDYN0)**0.14
       XAPPA3=XAPPA**3
       YS=Y*CT-Z*ST
       ZS=Z*CT+Y*ST
C
       FACTIMF=EXP(X/DELIMFX-(YS/DELIMFY)**2)
C
C  CALCULATE THE "IMF" COMPONENTS OUTSIDE THE LAYER  (HENCE BEGIN WITH "O")
C
       OIMFX=0.
       OIMFY=RECONN*BYIMF*FACTIMF
       OIMFZ=RECONN*BZIMF*FACTIMF
C
       RIMFAMPL=RECONN*Bt
C
       PPS=PS
       XX=X*XAPPA
       YY=Y*XAPPA
       ZZ=Z*XAPPA
C
C  SCALE AND CALCULATE THE MAGNETOPAUSE PARAMETERS FOR THE INTERPOLATION ACROSS
C   THE BOUNDARY LAYER (THE COORDINATES XX,YY,ZZ  ARE ALREADY SCALED)
C
       X0=X00/XAPPA
       AM=AM0/XAPPA
       RHO2=Y**2+Z**2
       ASQ=AM**2
       XMXM=AM+X-X0
       IF (XMXM.LT.0.) XMXM=0. ! THE BOUNDARY IS A CYLINDER TAILWARD OF X=X0-AM
       AXX0=XMXM**2
       ARO=ASQ+RHO2
       SIGMA=DSQRT((ARO+AXX0+DSQRT((ARO+AXX0)**2-4.*ASQ*AXX0))/(2.*ASQ))
C
C   NOW, THERE ARE THREE POSSIBLE CASES:
C    (1) INSIDE THE MAGNETOSPHERE
C    (2) IN THE BOUNDARY LAYER
C    (3) OUTSIDE THE MAGNETOSPHERE AND B.LAYER
C	FIRST OF ALL, CONSIDER THE CASES (1) AND (2):
C
      IF (SIGMA.LT.S0+DSIG04) THEN  !  CALCULATE THE T95_06 FIELD (WITH THE
C				 POTENTIAL "PENETRATED" INTERCONNECTION FIELD):

       CALL DIPSHLD(PPS,XX,YY,ZZ,CFX,CFY,CFZ)
       CALL TAILRC96(SPS,XX,YY,ZZ,BXRC,BYRC,BZRC,BXT2,BYT2,BZT2,
     *   BXT3,BYT3,BZT3)
       CALL BIRK1TOT_02(PPS,XX,YY,ZZ,R1X,R1Y,R1Z)
       CALL BIRK2TOT_02(PPS,XX,YY,ZZ,R2X,R2Y,R2Z)
       CALL INTERCON(XX,YS*XAPPA,ZS*XAPPA,RIMFX,RIMFYS,RIMFZS)
       RIMFY=RIMFYS*CT+RIMFZS*ST
       RIMFZ=RIMFZS*CT-RIMFYS*ST
C
      FX=CFX*XAPPA3+RCAMPL*BXRC +TAMPL2*BXT2+TAMPL3*BXT3
     *  +B1AMPL*R1X +B2AMPL*R2X +RIMFAMPL*RIMFX
      FY=CFY*XAPPA3+RCAMPL*BYRC +TAMPL2*BYT2+TAMPL3*BYT3
     *  +B1AMPL*R1Y +B2AMPL*R2Y +RIMFAMPL*RIMFY
      FZ=CFZ*XAPPA3+RCAMPL*BZRC +TAMPL2*BZT2+TAMPL3*BZT3
     *  +B1AMPL*R1Z +B2AMPL*R2Z +RIMFAMPL*RIMFZ
C
C  NOW, LET US CHECK WHETHER WE HAVE THE CASE (1). IF YES - WE ARE DONE:
C
       IF (SIGMA.LT.S0-DSIG04) THEN
         BX=FX
         BY=FY
         BZ=FZ
        		ELSE  !  THIS IS THE MOST COMPLEX CASE: WE ARE INSIDE
C					  THE INTERPOLATION REGION
       FINT=0.5*(1.-(SIGMA-S0)/DSIG04)
       FEXT=0.5*(1.+(SIGMA-S0)/DSIG04)
C
       CALL DIPOLE(PS,X,Y,Z,QX,QY,QZ)
       BX=(FX+QX)*FINT+OIMFX*FEXT -QX
       BY=(FY+QY)*FINT+OIMFY*FEXT -QY
       BZ=(FZ+QZ)*FINT+OIMFZ*FEXT -QZ
c
        ENDIF  !   THE CASES (1) AND (2) ARE EXHAUSTED; THE ONLY REMAINING
C		       POSSIBILITY IS NOW THE CASE (3):
         ELSE
        	CALL DIPOLE(PS,X,Y,Z,QX,QY,QZ)
        	BX=OIMFX-QX
        	BY=OIMFY-QY
        	BZ=OIMFZ-QZ
         ENDIF
C
       RETURN
       END
C=====================================================================

      SUBROUTINE DIPSHLD(PS,X,Y,Z,BX,BY,BZ)
C
C   CALCULATES GSM COMPONENTS OF THE EXTERNAL MAGNETIC FIELD DUE TO
C    SHIELDING OF THE EARTH'S DIPOLE ONLY
C
       IMPLICIT REAL*8 (A-H,O-Z)
       DIMENSION A1(12),A2(12)
      DATA A1 /.24777,-27.003,-.46815,7.0637,-1.5918,-.90317E-01,57.522,
     * 13.757,2.0100,10.458,4.5798,2.1695/
      DATA A2/-.65385,-18.061,-.40457,-5.0995,1.2846,.78231E-01,39.592,
     * 13.291,1.9970,10.062,4.5140,2.1558/
C
          CPS=DCOS(PS)
          SPS=DSIN(PS)
          CALL CYLHARM(A1,X,Y,Z,HX,HY,HZ)
          CALL CYLHAR1(A2,X,Y,Z,FX,FY,FZ)
C
          BX=HX*CPS+FX*SPS
          BY=HY*CPS+FY*SPS
          BZ=HZ*CPS+FZ*SPS
        RETURN
       END
C
C+++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
C
C  THIS CODE YIELDS THE SHIELDING FIELD FOR THE PERPENDICULAR DIPOLE
C
         SUBROUTINE  CYLHARM( A, X,Y,Z, BX,BY,BZ)
C
C
C   ***  N.A. Tsyganenko ***  Sept. 14-18, 1993; revised March 16, 1994 ***
C
C   An approximation for the Chapman-Ferraro field by a sum of 6 cylin-
c   drical harmonics (see pp. 97-113 in the brown GSFC notebook #1)
c
C      Description of parameters:
C
C  A   - input vector containing model parameters;
C  X,Y,Z   -  input GSM coordinates
C  BX,BY,BZ - output GSM components of the shielding field
C - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
C  The 6 linear parameters A(1)-A(6) are amplitudes of the cylindrical harmonic
c	terms.
c  The 6 nonlinear parameters A(7)-A(12) are the corresponding scale lengths
C	for each term (see GSFC brown notebook).
c
C - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
C
         IMPLICIT  REAL*8  (A - H, O - Z)
C
         DIMENSION  A(12)
C
           RHO=DSQRT(Y**2+Z**2)
            IF (RHO.LT.1.D-8) THEN
               SINFI=1.D0
               COSFI=0.D0
               RHO=1.D-8
               GOTO 1
            ENDIF
C
           SINFI=Z/RHO
           COSFI=Y/RHO
  1	   SINFI2=SINFI**2
           SI2CO2=SINFI2-COSFI**2
C
             BX=0.D0
             BY=0.D0
             BZ=0.D0
C
           DO 11 I=1,3
             DZETA=RHO/A(I+6)
             XJ0=BES(DZETA,0)
             XJ1=BES(DZETA,1)
             XEXP=DEXP(X/A(I+6))
             BX=BX-A(I)*XJ1*XEXP*SINFI
             BY=BY+A(I)*(2.D0*XJ1/DZETA-XJ0)*XEXP*SINFI*COSFI
             BZ=BZ+A(I)*(XJ1/DZETA*SI2CO2-XJ0*SINFI2)*XEXP
   11	     CONTINUE
c
           DO 12 I=4,6
             DZETA=RHO/A(I+6)
             XKSI=X/A(I+6)
             XJ0=BES(DZETA,0)
             XJ1=BES(DZETA,1)
             XEXP=DEXP(XKSI)
             BRHO=(XKSI*XJ0-(DZETA**2+XKSI-1.D0)*XJ1/DZETA)*XEXP*SINFI
             BPHI=(XJ0+XJ1/DZETA*(XKSI-1.D0))*XEXP*COSFI
             BX=BX+A(I)*(DZETA*XJ0+XKSI*XJ1)*XEXP*SINFI
             BY=BY+A(I)*(BRHO*COSFI-BPHI*SINFI)
             BZ=BZ+A(I)*(BRHO*SINFI+BPHI*COSFI)
   12	     CONTINUE
C
c
         RETURN
         END
C
c%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
C
C  THIS CODE YIELDS THE SHIELDING FIELD FOR THE PARALLEL DIPOLE
C
         SUBROUTINE  CYLHAR1(A, X,Y,Z, BX,BY,BZ)
C
C
C   ***  N.A. Tsyganenko ***  Sept. 14-18, 1993; revised March 16, 1994 ***
C
C   An approximation of the Chapman-Ferraro field by a sum of 6 cylin-
c   drical harmonics (see pages 97-113 in the brown GSFC notebook #1)
c
C      Description of parameters:
C
C  A   - input vector containing model parameters;
C  X,Y,Z - input GSM coordinates,
C  BX,BY,BZ - output GSM components of the shielding field
C
C - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
C
C      The 6 linear parameters A(1)-A(6) are amplitudes of the cylindrical
c  harmonic terms.
c      The 6 nonlinear parameters A(7)-A(12) are the corresponding scale
c  lengths for each term (see GSFC brown notebook).
c
C - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
C
         IMPLICIT  REAL*8  (A - H, O - Z)
C
         DIMENSION  A(12)
C
           RHO=DSQRT(Y**2+Z**2)
            IF (RHO.LT.1.D-10) THEN
               SINFI=1.D0
               COSFI=0.D0
               GOTO 1
            ENDIF
C
           SINFI=Z/RHO
           COSFI=Y/RHO
C
   1	  BX=0.D0
          BY=0.D0
          BZ=0.D0
C
             DO 11 I=1,3
             DZETA=RHO/A(I+6)
             XKSI=X/A(I+6)
             XJ0=BES(DZETA,0)
             XJ1=BES(DZETA,1)
             XEXP=DEXP(XKSI)
             BRHO=XJ1*XEXP
             BX=BX-A(I)*XJ0*XEXP
             BY=BY+A(I)*BRHO*COSFI
             BZ=BZ+A(I)*BRHO*SINFI
   11	     CONTINUE
c
           DO 12 I=4,6
             DZETA=RHO/A(I+6)
             XKSI=X/A(I+6)
             XJ0=BES(DZETA,0)
             XJ1=BES(DZETA,1)
             XEXP=DEXP(XKSI)
             BRHO=(DZETA*XJ0+XKSI*XJ1)*XEXP
             BX=BX+A(I)*(DZETA*XJ1-XJ0*(XKSI+1.D0))*XEXP
             BY=BY+A(I)*BRHO*COSFI
             BZ=BZ+A(I)*BRHO*SINFI
   12	     CONTINUE
C
         RETURN
         END

c%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
C
      DOUBLE PRECISION FUNCTION BES(X,K)
      IMPLICIT REAL*8 (A-H,O-Z)
C
      IF (K.EQ.0) THEN
        BES=BES0(X)
        RETURN
      ENDIF
C
      IF (K.EQ.1) THEN
        BES=BES1(X)
        RETURN
      ENDIF
C
      IF (X.EQ.0.D0) THEN
        BES=0.D0
        RETURN
      ENDIF
C
      G=2.D0/X
      IF (X.LE.DFLOAT(K)) GOTO 10
C
      N=1
      XJN=BES1(X)
      XJNM1=BES0(X)
C
  1   XJNP1=G*N*XJN-XJNM1
      N=N+1
      IF (N.LT.K) GOTO 2
      BES=XJNP1
      RETURN
C
 2    XJNM1=XJN
      XJN=XJNP1
      GOTO 1
C
 10   N=24
      XJN=1.D0
      XJNP1=0.D0
      SUM=0.D0
C
  3   IF (MOD(N,2).EQ.0) SUM=SUM+XJN
      XJNM1=G*N*XJN-XJNP1
      N=N-1
C
      XJNP1=XJN
      XJN=XJNM1
      IF (N.EQ.K) BES=XJN
C
      IF (DABS(XJN).GT.1.D5) THEN
        XJNP1=XJNP1*1.D-5
        XJN=XJN*1.D-5
        SUM=SUM*1.D-5
        IF (N.LE.K) BES=BES*1.D-5
      ENDIF
C
      IF (N.EQ.0) GOTO 4
      GOTO 3
C
  4   SUM=XJN+2.D0*SUM
      BES=BES/SUM
      RETURN
      END
c-------------------------------------------------------------------
c
       DOUBLE PRECISION FUNCTION BES0(X)
C
        IMPLICIT REAL*8 (A-H,O-Z)
C
        IF (DABS(X).LT.3.D0) THEN
          X32=(X/3.D0)**2
          BES0=1.D0-X32*(2.2499997D0-X32*(1.2656208D0-X32*
     *    (0.3163866D0-X32*(0.0444479D0-X32*(0.0039444D0
     *     -X32*0.00021D0)))))
        ELSE
        XD3=3.D0/X
        F0=0.79788456D0-XD3*(0.00000077D0+XD3*(0.00552740D0+XD3*
     *   (0.00009512D0-XD3*(0.00137237D0-XD3*(0.00072805D0
     *   -XD3*0.00014476D0)))))
        T0=X-0.78539816D0-XD3*(0.04166397D0+XD3*(0.00003954D0-XD3*
     *   (0.00262573D0-XD3*(0.00054125D0+XD3*(0.00029333D0
     *   -XD3*0.00013558D0)))))
        BES0=F0/DSQRT(X)*DCOS(T0)
       ENDIF
       RETURN
       END
c
c--------------------------------------------------------------------------
c
       DOUBLE PRECISION FUNCTION BES1(X)
C
        IMPLICIT REAL*8 (A-H,O-Z)
C
       IF (DABS(X).LT.3.D0) THEN
        X32=(X/3.D0)**2
        BES1XM1=0.5D0-X32*(0.56249985D0-X32*(0.21093573D0-X32*
     *  (0.03954289D0-X32*(0.00443319D0-X32*(0.00031761D0
     *  -X32*0.00001109D0)))))
         BES1=BES1XM1*X
       ELSE
        XD3=3.D0/X
        F1=0.79788456D0+XD3*(0.00000156D0+XD3*(0.01659667D0+XD3*
     *   (0.00017105D0-XD3*(0.00249511D0-XD3*(0.00113653D0
     *   -XD3*0.00020033D0)))))
        T1=X-2.35619449D0+XD3*(0.12499612D0+XD3*(0.0000565D0-XD3*
     *   (0.00637879D0-XD3*(0.00074348D0+XD3*(0.00079824D0
     *   -XD3*0.00029166D0)))))
        BES1=F1/DSQRT(X)*DCOS(T1)
       ENDIF
       RETURN
       END
C------------------------------------------------------------
C
         SUBROUTINE INTERCON(X,Y,Z,BX,BY,BZ)
C
C      Calculates the potential interconnection field inside the magnetosphere,
c  corresponding to  DELTA_X = 20Re and DELTA_Y = 10Re (NB#3, p.90, 6/6/1996).
C  The position (X,Y,Z) and field components BX,BY,BZ are given in the rotated
c   coordinate system, in which the Z-axis is always directed along the BzIMF
c   (i.e. rotated by the IMF clock angle Theta)
C   It is also assumed that the IMF Bt=1, so that the components should be
c     (i) multiplied by the actual Bt, and
c     (ii) transformed to standard GSM coords by rotating back around X axis
c	       by the angle -Theta.
c
C      Description of parameters:
C
C     X,Y,Z -	GSM POSITION
C      BX,BY,BZ - INTERCONNECTION FIELD COMPONENTS INSIDE THE MAGNETOSPHERE
C	 OF A STANDARD SIZE (TO TAKE INTO ACCOUNT EFFECTS OF PRESSURE CHANGES,
C	  APPLY THE SCALING TRANSFORMATION)
C
C - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
C
C     The 9 linear parameters are amplitudes of the "cartesian" harmonics
c     The 6 nonlinear parameters are the scales Pi and Ri entering
c    the arguments of exponents, sines, and cosines in the 9 "Cartesian"
c	harmonics (3+3)
C - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
C
         IMPLICIT  REAL*8  (A - H, O - Z)
C
        DIMENSION A(15),RP(3),RR(3),P(3),R(3)
C
      DATA A/-8.411078731,5932254.951,-9073284.93,-11.68794634,
     * 6027598.824,-9218378.368,-6.508798398,-11824.42793,18015.66212,
     * 7.99754043,13.9669886,90.24475036,16.75728834,1015.645781,
     * 1553.493216/
C
        DATA M/0/
C
        IF (M.NE.0) GOTO 111
        M=1
C
         P(1)=A(10)
         P(2)=A(11)
         P(3)=A(12)
         R(1)=A(13)
         R(2)=A(14)
         R(3)=A(15)
C
C
           DO 11 I=1,3
            RP(I)=1.D0/P(I)
  11	    RR(I)=1.D0/R(I)
C
  111	CONTINUE
C
            L=0
C
               BX=0.
               BY=0.
               BZ=0.
C
c	 "PERPENDICULAR" KIND OF SYMMETRY ONLY
C
               DO 2 I=1,3
        	  CYPI=DCOS(Y*RP(I))
        	  SYPI=DSIN(Y*RP(I))
C
        	DO 2 K=1,3
        	   SZRK=DSIN(Z*RR(K))
        	   CZRK=DCOS(Z*RR(K))
        	     SQPR=DSQRT(RP(I)**2+RR(K)**2)
        	      EPR=DEXP(X*SQPR)
C
        	     HX=-SQPR*EPR*CYPI*SZRK
        	     HY=RP(I)*EPR*SYPI*SZRK
        	     HZ=-RR(K)*EPR*CYPI*CZRK
             L=L+1
c
          BX=BX+A(L)*HX
          BY=BY+A(L)*HY
          BZ=BZ+A(L)*HZ
  2   CONTINUE
C
      RETURN
      END

C~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
      SUBROUTINE TAILRC96(SPS,X,Y,Z,BXRC,BYRC,BZRC,BXT2,BYT2,BZT2,
     *   BXT3,BYT3,BZT3)
c
c  COMPUTES THE COMPONENTS OF THE FIELD OF THE MODEL RING CURRENT AND THREE
c		    TAIL MODES WITH UNIT AMPLITUDES
C      (FOR THE RING CURRENT, IT MEANS THE DISTURBANCE OF Bz=-1nT AT ORIGIN,
C   AND FOR THE TAIL MODES IT MEANS MAXIMAL BX JUST ABOVE THE SHEET EQUAL 1 nT.
C
         IMPLICIT REAL*8 (A-H,O-Z)
         DIMENSION ARC(48),ATAIL2(48),ATAIL3(48)
         COMMON /WARP/ CPSS,SPSS,DPSRR,RPS,WARP,D,XS,ZS,DXSX,DXSY,DXSZ,
     *   DZSX,DZSY,DZSZ,DZETAS,DDZETADX,DDZETADY,DDZETADZ,ZSWW
C
         DATA ARC/-3.087699646,3.516259114,18.81380577,-13.95772338,
     *  -5.497076303,0.1712890838,2.392629189,-2.728020808,-14.79349936,
     *  11.08738083,4.388174084,0.2492163197E-01,0.7030375685,
     *-.7966023165,-3.835041334,2.642228681,-0.2405352424,-0.7297705678,
     * -0.3680255045,0.1333685557,2.795140897,-1.078379954,0.8014028630,
     * 0.1245825565,0.6149982835,-0.2207267314,-4.424578723,1.730471572,
     * -1.716313926,-0.2306302941,-0.2450342688,0.8617173961E-01,
     *  1.54697858,-0.6569391113,-0.6537525353,0.2079417515,12.75434981,
     *  11.37659788,636.4346279,1.752483754,3.604231143,12.83078674,
     * 7.412066636,9.434625736,676.7557193,1.701162737,3.580307144,
     *  14.64298662/
C
        DATA ATAIL2/.8747515218,-.9116821411,2.209365387,-2.159059518,
     * -7.059828867,5.924671028,-1.916935691,1.996707344,-3.877101873,
     * 3.947666061,11.38715899,-8.343210833,1.194109867,-1.244316975,
     * 3.73895491,-4.406522465,-20.66884863,3.020952989,.2189908481,
     * -.09942543549,-.927225562,.1555224669,.6994137909,-.08111721003,
     * -.7565493881,.4686588792,4.266058082,-.3717470262,-3.920787807,
     * .02298569870,.7039506341,-.5498352719,-6.675140817,.8279283559,
     * -2.234773608,-1.622656137,5.187666221,6.802472048,39.13543412,
     *  2.784722096,6.979576616,25.71716760,4.495005873,8.068408272,
     * 93.47887103,4.158030104,9.313492566,57.18240483/
C
        DATA ATAIL3/-19091.95061,-3011.613928,20582.16203,4242.918430,
     * -2377.091102,-1504.820043,19884.04650,2725.150544,-21389.04845,
     * -3990.475093,2401.610097,1548.171792,-946.5493963,490.1528941,
     * 986.9156625,-489.3265930,-67.99278499,8.711175710,-45.15734260,
     * -10.76106500,210.7927312,11.41764141,-178.0262808,.7558830028,
     *  339.3806753,9.904695974,69.50583193,-118.0271581,22.85935896,
     * 45.91014857,-425.6607164,15.47250738,118.2988915,65.58594397,
     * -201.4478068,-14.57062940,19.69877970,20.30095680,86.45407420,
     * 22.50403727,23.41617329,48.48140573,24.61031329,123.5395974,
     * 223.5367692,39.50824342,65.83385762,266.2948657/
C
       DATA RH,DR,G,D0,DELTADY/9.,4.,10.,2.,10./
C
C   TO ECONOMIZE THE CODE, WE FIRST CALCULATE COMMON VARIABLES, WHICH ARE
C      THE SAME FOR ALL MODES, AND PUT THEM IN THE COMMON-BLOCK /WARP/
C
       DR2=DR*DR
       C11=DSQRT((1.D0+RH)**2+DR2)
       C12=DSQRT((1.D0-RH)**2+DR2)
       C1=C11-C12
       SPSC1=SPS/C1
       RPS=0.5*(C11+C12)*SPS  !  THIS IS THE SHIFT OF OF THE SHEET WITH RESPECT
C			     TO GSM EQ.PLANE FOR THE 3RD (ASYMPTOTIC) TAIL MODE
C
        R=DSQRT(X*X+Y*Y+Z*Z)
        SQ1=DSQRT((R+RH)**2+DR2)
        SQ2=DSQRT((R-RH)**2+DR2)
        C=SQ1-SQ2
        CS=(R+RH)/SQ1-(R-RH)/SQ2
        SPSS=SPSC1/R*C
        CPSS=DSQRT(1.D0-SPSS**2)
        DPSRR=SPS/(R*R)*(CS*R-C)/DSQRT((R*C1)**2-(C*SPS)**2)
C
        WFAC=Y/(Y**4+1.D4)   !   WARPING
        W=WFAC*Y**3
        WS=4.D4*Y*WFAC**2
        WARP=G*SPS*W
        XS=X*CPSS-Z*SPSS
        ZSWW=Z*CPSS+X*SPSS  ! "WW" MEANS "WITHOUT Y-Z WARPING" (IN X-Z ONLY)
        ZS=ZSWW +WARP

        DXSX=CPSS-X*ZSWW*DPSRR
        DXSY=-Y*ZSWW*DPSRR
        DXSZ=-SPSS-Z*ZSWW*DPSRR
        DZSX=SPSS+X*XS*DPSRR
        DZSY=XS*Y*DPSRR  +G*SPS*WS  !  THE LAST TERM IS FOR THE Y-Z WARP
        DZSZ=CPSS+XS*Z*DPSRR	    !	   (TAIL MODES ONLY)

        D=D0+DELTADY*(Y/20.D0)**2   !  SHEET HALF-THICKNESS FOR THE TAIL MODES
        DDDY=DELTADY*Y*0.005D0      !  (THICKENS TO FLANKS, BUT NO VARIATION
C					  ALONG X, IN CONTRAST TO RING CURRENT)
C
        DZETAS=DSQRT(ZS**2+D**2)  !  THIS IS THE SAME SIMPLE WAY TO SPREAD
C					 OUT THE SHEET, AS THAT USED IN T89
        DDZETADX=ZS*DZSX/DZETAS
        DDZETADY=(ZS*DZSY+D*DDDY)/DZETAS
        DDZETADZ=ZS*DZSZ/DZETAS
C
        CALL SHLCAR3X3(ARC,X,Y,Z,SPS,WX,WY,WZ)
        CALL RINGCURR96(X,Y,Z,HX,HY,HZ)
        BXRC=WX+HX
        BYRC=WY+HY
        BZRC=WZ+HZ
C
        CALL SHLCAR3X3(ATAIL2,X,Y,Z,SPS,WX,WY,WZ)
        CALL TAILDISK(X,Y,Z,HX,HY,HZ)
        BXT2=WX+HX
        BYT2=WY+HY
        BZT2=WZ+HZ
C
        CALL SHLCAR3X3(ATAIL3,X,Y,Z,SPS,WX,WY,WZ)
        CALL TAIL87(X,Z,HX,HZ)
        BXT3=WX+HX
        BYT3=WY
        BZT3=WZ+HZ
C
        RETURN
        END
C
c********************************************************************
C
        SUBROUTINE RINGCURR96(X,Y,Z,BX,BY,BZ)
c
c	THIS SUBROUTINE COMPUTES THE COMPONENTS OF THE RING CURRENT FIELD,
C	 SIMILAR TO THAT DESCRIBED BY TSYGANENKO AND PEREDO (1994).  THE
C	   DIFFERENCE IS THAT NOW WE USE SPACEWARPING, AS DESCRIBED IN THE
C	    PAPER ON MODELING BIRKELAND CURRENTS (TSYGANENKO AND STERN, 1996),
C	     INSTEAD OF SHEARING IT IN THE SPIRIT OF THE T89 TAIL MODEL.
C
C	   IN  ADDITION, INSTEAD OF 7 TERMS FOR THE RING CURRENT MODEL, WE USE
C	      NOW ONLY 2 TERMS;  THIS SIMPLIFICATION ALSO GIVES RISE TO AN
C		 EASTWARD RING CURRENT LOCATED EARTHWARD FROM THE MAIN ONE,
C		   IN LINE WITH WHAT IS ACTUALLY OBSERVED
C
C	      FOR DETAILS, SEE NB #3, PAGES 70-73
C
        IMPLICIT REAL*8 (A-H,O-Z)
        DIMENSION F(2),BETA(2)
        COMMON /WARP/ CPSS,SPSS,DPSRR, XNEXT(3),XS,ZSWARPED,DXSX,DXSY,
     *   DXSZ,DZSX,DZSYWARPED,DZSZ,OTHER(4),ZS  !  ZS HERE IS WITHOUT Y-Z WARP
C

      DATA D0,DELTADX,XD,XLDX /2.,0.,0.,4./  !  ACHTUNG !!  THE RC IS NOW
C					     COMPLETELY SYMMETRIC (DELTADX=0)

C
        DATA F,BETA /569.895366D0,-1603.386993D0,2.722188D0,3.766875D0/
C
C  THE ORIGINAL VALUES OF F(I) WERE MULTIPLIED BY BETA(I) (TO REDUCE THE
C     NUMBER OF MULTIPLICATIONS BELOW)  AND BY THE FACTOR -0.43, NORMALIZING
C      THE DISTURBANCE AT ORIGIN  TO  B=-1nT
C
           DZSY=XS*Y*DPSRR  ! NO WARPING IN THE Y-Z PLANE (ALONG X ONLY), AND
C			  THIS IS WHY WE DO NOT USE  DZSY FROM THE COMMON-BLOCK
           XXD=X-XD
           FDX=0.5D0*(1.D0+XXD/DSQRT(XXD**2+XLDX**2))
           DDDX=DELTADX*0.5D0*XLDX**2/DSQRT(XXD**2+XLDX**2)**3
           D=D0+DELTADX*FDX

           DZETAS=DSQRT(ZS**2+D**2)  !  THIS IS THE SAME SIMPLE WAY TO SPREAD
C					 OUT THE SHEET, AS THAT USED IN T89
           RHOS=DSQRT(XS**2+Y**2)
           DDZETADX=(ZS*DZSX+D*DDDX)/DZETAS
           DDZETADY=ZS*DZSY/DZETAS
           DDZETADZ=ZS*DZSZ/DZETAS
         IF (RHOS.LT.1.D-5) THEN
            DRHOSDX=0.D0
            DRHOSDY=DSIGN(1.D0,Y)
            DRHOSDZ=0.D0
          ELSE
           DRHOSDX=XS*DXSX/RHOS
           DRHOSDY=(XS*DXSY+Y)/RHOS
           DRHOSDZ=XS*DXSZ/RHOS
         ENDIF
C
           BX=0.D0
           BY=0.D0
           BZ=0.D0
C
           DO 1 I=1,2
C
           BI=BETA(I)
C
           S1=DSQRT((DZETAS+BI)**2+(RHOS+BI)**2)
           S2=DSQRT((DZETAS+BI)**2+(RHOS-BI)**2)
           DS1DDZ=(DZETAS+BI)/S1
           DS2DDZ=(DZETAS+BI)/S2
           DS1DRHOS=(RHOS+BI)/S1
           DS2DRHOS=(RHOS-BI)/S2
C
           DS1DX=DS1DDZ*DDZETADX+DS1DRHOS*DRHOSDX
           DS1DY=DS1DDZ*DDZETADY+DS1DRHOS*DRHOSDY
           DS1DZ=DS1DDZ*DDZETADZ+DS1DRHOS*DRHOSDZ
C
           DS2DX=DS2DDZ*DDZETADX+DS2DRHOS*DRHOSDX
           DS2DY=DS2DDZ*DDZETADY+DS2DRHOS*DRHOSDY
           DS2DZ=DS2DDZ*DDZETADZ+DS2DRHOS*DRHOSDZ
C
           S1TS2=S1*S2
           S1PS2=S1+S2
           S1PS2SQ=S1PS2**2
           FAC1=DSQRT(S1PS2SQ-(2.D0*BI)**2)
           AS=FAC1/(S1TS2*S1PS2SQ)
           TERM1=1.D0/(S1TS2*S1PS2*FAC1)
           FAC2=AS/S1PS2SQ
           DASDS1=TERM1-FAC2/S1*(S2*S2+S1*(3.D0*S1+4.D0*S2))
           DASDS2=TERM1-FAC2/S2*(S1*S1+S2*(3.D0*S2+4.D0*S1))
C
           DASDX=DASDS1*DS1DX+DASDS2*DS2DX
           DASDY=DASDS1*DS1DY+DASDS2*DS2DY
           DASDZ=DASDS1*DS1DZ+DASDS2*DS2DZ
C
      BX=BX+F(I)*((2.D0*AS+Y*DASDY)*SPSS-XS*DASDZ
     *   +AS*DPSRR*(Y**2*CPSS+Z*ZS))
      BY=BY-F(I)*Y*(AS*DPSRR*XS+DASDZ*CPSS+DASDX*SPSS)
  1   BZ=BZ+F(I)*((2.D0*AS+Y*DASDY)*CPSS+XS*DASDX
     *   -AS*DPSRR*(X*ZS+Y**2*SPSS))
C
       RETURN
       END
C
C$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$
C
         SUBROUTINE TAILDISK(X,Y,Z,BX,BY,BZ)
C
c
c	THIS SUBROUTINE COMPUTES THE COMPONENTS OF THE TAIL CURRENT FIELD,
C	 SIMILAR TO THAT DESCRIBED BY TSYGANENKO AND PEREDO (1994).  THE
C	   DIFFERENCE IS THAT NOW WE USE SPACEWARPING, AS DESCRIBED IN OUR
C	    PAPER ON MODELING BIRKELAND CURRENTS (TSYGANENKO AND STERN, 1996)
C	     INSTEAD OF SHEARING IT IN THE SPIRIT OF T89 TAIL MODEL.
C
C	   IN  ADDITION, INSTEAD OF 8 TERMS FOR THE TAIL CURRENT MODEL, WE USE
C	    NOW ONLY 4 TERMS
C
C	      FOR DETAILS, SEE NB #3, PAGES 74-
C
         IMPLICIT REAL*8 (A-H,O-Z)
         DIMENSION F(4),BETA(4)
         COMMON /WARP/ CPSS,SPSS,DPSRR,XNEXT(3),XS,ZS,DXSX,DXSY,DXSZ,
     *    OTHER(3),DZETAS,DDZETADX,DDZETADY,DDZETADZ,ZSWW
C
         DATA XSHIFT /4.5/
C
         DATA F,BETA
     * / -745796.7338D0,1176470.141D0,-444610.529D0,-57508.01028D0,
     *   7.9250000D0,8.0850000D0,8.4712500D0,27.89500D0/
c
c  here original F(I) are multiplied by BETA(I), to economize
c    calculations
C
           RHOS=DSQRT((XS-XSHIFT)**2+Y**2)
         IF (RHOS.LT.1.D-5) THEN
            DRHOSDX=0.D0
            DRHOSDY=DSIGN(1.D0,Y)
            DRHOSDZ=0.D0
         ELSE
           DRHOSDX=(XS-XSHIFT)*DXSX/RHOS
           DRHOSDY=((XS-XSHIFT)*DXSY+Y)/RHOS
           DRHOSDZ=(XS-XSHIFT)*DXSZ/RHOS
         ENDIF
C
           BX=0.D0
           BY=0.D0
           BZ=0.D0
C
           DO 1 I=1,4
C
           BI=BETA(I)
C
           S1=DSQRT((DZETAS+BI)**2+(RHOS+BI)**2)
           S2=DSQRT((DZETAS+BI)**2+(RHOS-BI)**2)
           DS1DDZ=(DZETAS+BI)/S1
           DS2DDZ=(DZETAS+BI)/S2
           DS1DRHOS=(RHOS+BI)/S1
           DS2DRHOS=(RHOS-BI)/S2
C
           DS1DX=DS1DDZ*DDZETADX+DS1DRHOS*DRHOSDX
           DS1DY=DS1DDZ*DDZETADY+DS1DRHOS*DRHOSDY
           DS1DZ=DS1DDZ*DDZETADZ+DS1DRHOS*DRHOSDZ
C
           DS2DX=DS2DDZ*DDZETADX+DS2DRHOS*DRHOSDX
           DS2DY=DS2DDZ*DDZETADY+DS2DRHOS*DRHOSDY
           DS2DZ=DS2DDZ*DDZETADZ+DS2DRHOS*DRHOSDZ
C
           S1TS2=S1*S2
           S1PS2=S1+S2
           S1PS2SQ=S1PS2**2
           FAC1=DSQRT(S1PS2SQ-(2.D0*BI)**2)
           AS=FAC1/(S1TS2*S1PS2SQ)
           TERM1=1.D0/(S1TS2*S1PS2*FAC1)
           FAC2=AS/S1PS2SQ
           DASDS1=TERM1-FAC2/S1*(S2*S2+S1*(3.D0*S1+4.D0*S2))
           DASDS2=TERM1-FAC2/S2*(S1*S1+S2*(3.D0*S2+4.D0*S1))
C
           DASDX=DASDS1*DS1DX+DASDS2*DS2DX
           DASDY=DASDS1*DS1DY+DASDS2*DS2DY
           DASDZ=DASDS1*DS1DZ+DASDS2*DS2DZ
C
      BX=BX+F(I)*((2.D0*AS+Y*DASDY)*SPSS-(XS-XSHIFT)*DASDZ
     *   +AS*DPSRR*(Y**2*CPSS+Z*ZSWW))
C
      BY=BY-F(I)*Y*(AS*DPSRR*XS+DASDZ*CPSS+DASDX*SPSS)
  1   BZ=BZ+F(I)*((2.D0*AS+Y*DASDY)*CPSS+(XS-XSHIFT)*DASDX
     *   -AS*DPSRR*(X*ZSWW+Y**2*SPSS))

       RETURN
       END

C-------------------------------------------------------------------------
C
      SUBROUTINE TAIL87(X,Z,BX,BZ)

      IMPLICIT REAL*8 (A-H,O-Z)

      COMMON /WARP/ FIRST(3), RPS,WARP,D, OTHER(13)
C
C      'LONG' VERSION OF THE 1987 TAIL MAGNETIC FIELD MODEL
C	       (N.A.TSYGANENKO, PLANET. SPACE SCI., V.35, P.1347, 1987)
C
C      D   IS THE Y-DEPENDENT SHEET HALF-THICKNESS (INCREASING TOWARDS FLANKS)
C      RPS  IS THE TILT-DEPENDENT SHIFT OF THE SHEET IN THE Z-DIRECTION,
C	    CORRESPONDING TO THE ASYMPTOTIC HINGING DISTANCE, DEFINED IN THE
C	    MAIN SUBROUTINE (TAILRC96) FROM THE PARAMETERS RH AND DR OF THE
C	    T96-TYPE MODULE, AND
C      WARP  IS THE BENDING OF THE SHEET FLANKS IN THE Z-DIRECTION, DIRECTED
C	    OPPOSITE TO RPS, AND INCREASING WITH DIPOLE TILT AND |Y|
C

        DATA DD/3./
C
      DATA HPI,RT,XN,X1,X2,B0,B1,B2,XN21,XNR,ADLN
     * /1.5707963,40.,-10.,
     * -1.261,-0.663,0.391734,5.89715,24.6833,76.37,-0.1071,0.13238005/
C		 !!!   THESE ARE NEW VALUES OF  X1, X2, B0, B1, B2,
C			CORRESPONDING TO TSCALE=1, INSTEAD OF TSCALE=0.6
C
C  THE ABOVE QUANTITIES WERE DEFINED AS FOLLOWS:------------------------
C	HPI=PI/2
C	RT=40.      !  Z-POSITION OF UPPER AND LOWER ADDITIONAL SHEETS
C	XN=-10.     !  INNER EDGE POSITION
C
C	TSCALE=1  !  SCALING FACTOR, DEFINING THE RATE OF INCREASE OF THE
C			CURRENT DENSITY TAILWARDS
C
c  ATTENTION !  NOW I HAVE CHANGED TSCALE TO:  TSCALE=1.0, INSTEAD OF 0.6
c		   OF THE PREVIOUS VERSION
c
C	B0=0.391734
C	B1=5.89715 *TSCALE
C	B2=24.6833 *TSCALE**2
C
C    HERE ORIGINAL VALUES OF THE MODE AMPLITUDES (P.77, NB#3) WERE NORMALIZED
C      SO THAT ASYMPTOTIC  BX=1  AT X=-200RE
C
C      X1=(4.589  -5.85) *TSCALE -(TSCALE-1.)*XN ! NONLINEAR PARAMETERS OF THE
C							  CURRENT FUNCTION
C      X2=(5.187  -5.85) *TSCALE -(TSCALE-1.)*XN
c
c
C      XN21=(XN-X1)**2
C      XNR=1./(XN-X2)
C      ADLN=-DLOG(XNR**2*XN21)
C
C---------------------------------------------------------------
C
      ZS=Z -RPS +WARP
      ZP=Z-RT
      ZM=Z+RT
C
      XNX=XN-X
      XNX2=XNX**2
      XC1=X-X1
      XC2=X-X2
      XC22=XC2**2
      XR2=XC2*XNR
      XC12=XC1**2
      D2=DD**2    !  SQUARE OF THE TOTAL HALFTHICKNESS (DD=3Re for this mode)
      B20=ZS**2+D2
      B2P=ZP**2+D2
      B2M=ZM**2+D2
      B=DSQRT(B20)
      BP=DSQRT(B2P)
      BM=DSQRT(B2M)
      XA1=XC12+B20
      XAP1=XC12+B2P
      XAM1=XC12+B2M
      XA2=1./(XC22+B20)
      XAP2=1./(XC22+B2P)
      XAM2=1./(XC22+B2M)
      XNA=XNX2+B20
      XNAP=XNX2+B2P
      XNAM=XNX2+B2M
      F=B20-XC22
      FP=B2P-XC22
      FM=B2M-XC22
      XLN1=DLOG(XN21/XNA)
      XLNP1=DLOG(XN21/XNAP)
      XLNM1=DLOG(XN21/XNAM)
      XLN2=XLN1+ADLN
      XLNP2=XLNP1+ADLN
      XLNM2=XLNM1+ADLN
      ALN=0.25*(XLNP1+XLNM1-2.*XLN1)
      S0=(DATAN(XNX/B)+HPI)/B
      S0P=(DATAN(XNX/BP)+HPI)/BP
      S0M=(DATAN(XNX/BM)+HPI)/BM
      S1=(XLN1*.5+XC1*S0)/XA1
      S1P=(XLNP1*.5+XC1*S0P)/XAP1
      S1M=(XLNM1*.5+XC1*S0M)/XAM1
      S2=(XC2*XA2*XLN2-XNR-F*XA2*S0)*XA2
      S2P=(XC2*XAP2*XLNP2-XNR-FP*XAP2*S0P)*XAP2
      S2M=(XC2*XAM2*XLNM2-XNR-FM*XAM2*S0M)*XAM2
      G1=(B20*S0-0.5*XC1*XLN1)/XA1
      G1P=(B2P*S0P-0.5*XC1*XLNP1)/XAP1
      G1M=(B2M*S0M-0.5*XC1*XLNM1)/XAM1
      G2=((0.5*F*XLN2+2.*S0*B20*XC2)*XA2+XR2)*XA2
      G2P=((0.5*FP*XLNP2+2.*S0P*B2P*XC2)*XAP2+XR2)*XAP2
      G2M=((0.5*FM*XLNM2+2.*S0M*B2M*XC2)*XAM2+XR2)*XAM2
      BX=B0*(ZS*S0-0.5*(ZP*S0P+ZM*S0M))
     * +B1*(ZS*S1-0.5*(ZP*S1P+ZM*S1M))+B2*(ZS*S2-0.5*(ZP*S2P+ZM*S2M))
      BZ=B0*ALN+B1*(G1-0.5*(G1P+G1M))+B2*(G2-0.5*(G2P+G2M))
C
C    CALCULATION OF THE MAGNETOTAIL CURRENT CONTRIBUTION IS FINISHED
C
      RETURN
      END

C$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$
C
C THIS CODE RETURNS THE SHIELDING FIELD REPRESENTED BY  2x3x3=18 "CARTESIAN"
C    HARMONICS
C
         SUBROUTINE  SHLCAR3X3(A,X,Y,Z,SPS,HX,HY,HZ)
C
C - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
C  The 36 coefficients enter in pairs in the amplitudes of the "cartesian"
c    harmonics (A(1)-A(36).
c  The 12 nonlinear parameters (A(37)-A(48) are the scales Pi,Ri,Qi,and Si
C   entering the arguments of exponents, sines, and cosines in each of the
C   18 "Cartesian" harmonics
C - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
C
         IMPLICIT  REAL*8  (A - H, O - Z)
C
         DIMENSION A(48)
C
          CPS=DSQRT(1.D0-SPS**2)
          S3PS=4.D0*CPS**2-1.D0   !  THIS IS SIN(3*PS)/SIN(PS)
C
           HX=0.D0
           HY=0.D0
           HZ=0.D0
           L=0
C
           DO 1 M=1,2	  !    M=1 IS FOR THE 1ST SUM ("PERP." SYMMETRY)
C			    AND M=2 IS FOR THE SECOND SUM ("PARALL." SYMMETRY)
             DO 2 I=1,3
        	  P=A(36+I)
        	  Q=A(42+I)
        	  CYPI=DCOS(Y/P)
        	  CYQI=DCOS(Y/Q)
        	  SYPI=DSIN(Y/P)
        	  SYQI=DSIN(Y/Q)
C
              DO 3 K=1,3
        	   R=A(39+K)
        	   S=A(45+K)
        	   SZRK=DSIN(Z/R)
        	   CZSK=DCOS(Z/S)
        	   CZRK=DCOS(Z/R)
        	   SZSK=DSIN(Z/S)
        	     SQPR=DSQRT(1.D0/P**2+1.D0/R**2)
        	     SQQS=DSQRT(1.D0/Q**2+1.D0/S**2)
        		EPR=DEXP(X*SQPR)
        		EQS=DEXP(X*SQQS)
C
        	   DO 4 N=1,2  ! N=1 IS FOR THE FIRST PART OF EACH COEFFICIENT
C				   AND N=2 IS FOR THE SECOND ONE
C
        	    L=L+1
        	     IF (M.EQ.1) THEN
        	       IF (N.EQ.1) THEN
        		 DX=-SQPR*EPR*CYPI*SZRK
        		 DY=EPR/P*SYPI*SZRK
        		 DZ=-EPR/R*CYPI*CZRK
        		 HX=HX+A(L)*DX
        		 HY=HY+A(L)*DY
        		 HZ=HZ+A(L)*DZ
        			   ELSE
        		 DX=DX*CPS
        		 DY=DY*CPS
        		 DZ=DZ*CPS
        		 HX=HX+A(L)*DX
        		 HY=HY+A(L)*DY
        		 HZ=HZ+A(L)*DZ
        			   ENDIF
        	     ELSE
        	       IF (N.EQ.1) THEN
        		 DX=-SPS*SQQS*EQS*CYQI*CZSK
        		 DY=SPS*EQS/Q*SYQI*CZSK
        		 DZ=SPS*EQS/S*CYQI*SZSK
        		 HX=HX+A(L)*DX
        		 HY=HY+A(L)*DY
        		 HZ=HZ+A(L)*DZ
        			   ELSE
        		 DX=DX*S3PS
        		 DY=DY*S3PS
        		 DZ=DZ*S3PS
        		 HX=HX+A(L)*DX
        		 HY=HY+A(L)*DY
        		 HZ=HZ+A(L)*DZ
        	       ENDIF
        	 ENDIF
c
  4   CONTINUE
  3   CONTINUE
  2   CONTINUE
  1   CONTINUE
C
         RETURN
         END

C
C@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@
C
       SUBROUTINE BIRK1TOT_02(PS,X,Y,Z,BX,BY,BZ)
C
C  THIS IS THE SECOND VERSION OF THE ANALYTICAL MODEL OF THE REGION 1 FIELD
C   BASED ON A SEPARATE REPRESENTATION OF THE POTENTIAL FIELD IN THE INNER AND
C   OUTER SPACE, MAPPED BY MEANS OF A SPHERO-DIPOLAR COORDINATE SYSTEM (NB #3,
C   P.91).   THE DIFFERENCE FROM THE FIRST ONE IS THAT INSTEAD OF OCTAGONAL
C   CURRENT LOOPS, CIRCULAR ONES ARE USED IN THIS VERSION FOR APPROXIMATING THE
C   FIELD IN THE OUTER REGION, WHICH IS FASTER.
C
      IMPLICIT REAL*8 (A-H,O-Z)
C
      DIMENSION D1(3,26),D2(3,79),XI(4),C1(26),C2(79)

         COMMON /COORD11/ XX1(12),YY1(12)
         COMMON /RHDR/ RH,DR
         COMMON /LOOPDIP1/ TILT,XCENTRE(2),RADIUS(2), DIPX,DIPY
C
         COMMON /COORD21/ XX2(14),YY2(14),ZZ2(14)
         COMMON /DX1/ DX,SCALEIN,SCALEOUT
C
      DATA C1/-0.911582E-03,-0.376654E-02,-0.727423E-02,-0.270084E-02,
     * -0.123899E-02,-0.154387E-02,-0.340040E-02,-0.191858E-01,
     * -0.518979E-01,0.635061E-01,0.440680,-0.396570,0.561238E-02,
     *  0.160938E-02,-0.451229E-02,-0.251810E-02,-0.151599E-02,
     * -0.133665E-02,-0.962089E-03,-0.272085E-01,-0.524319E-01,
     *  0.717024E-01,0.523439,-0.405015,-89.5587,23.2806/

C
      DATA C2/6.04133,.305415,.606066E-02,.128379E-03,-.179406E-04,
     * 1.41714,-27.2586,-4.28833,-1.30675,35.5607,8.95792,.961617E-03,
     * -.801477E-03,-.782795E-03,-1.65242,-16.5242,-5.33798,.424878E-03,
     * .331787E-03,-.704305E-03,.844342E-03,.953682E-04,.886271E-03,
     * 25.1120,20.9299,5.14569,-44.1670,-51.0672,-1.87725,20.2998,
     * 48.7505,-2.97415,3.35184,-54.2921,-.838712,-10.5123,70.7594,
     * -4.94104,.106166E-03,.465791E-03,-.193719E-03,10.8439,-29.7968,
     *  8.08068,.463507E-03,-.224475E-04,.177035E-03,-.317581E-03,
     * -.264487E-03,.102075E-03,7.71390,10.1915,-4.99797,-23.1114,
     *-29.2043,12.2928,10.9542,33.6671,-9.3851,.174615E-03,-.789777E-06,
     * .686047E-03,.460104E-04,-.345216E-02,.221871E-02,.110078E-01,
     * -.661373E-02,.249201E-02,.343978E-01,-.193145E-05,.493963E-05,
     * -.535748E-04,.191833E-04,-.100496E-03,-.210103E-03,-.232195E-02,
     * .315335E-02,-.134320E-01,-.263222E-01/
c
      DATA TILT,XCENTRE,RADIUS,DIPX,DIPY /1.00891,2.28397,-5.60831,
     * 1.86106,7.83281,1.12541,0.945719/

      DATA DX,SCALEIN,SCALEOUT /-0.16D0,0.08D0,0.4D0/
      DATA XX1/-11.D0,2*-7.D0,2*-3.D0,3*1.D0,2*5.D0,2*9.D0/
      DATA YY1/2.D0,0.D0,4.D0,2.D0,6.D0,0.D0,4.D0,8.D0,2.D0,6.D0,0.D0,
     *  4.D0/
      DATA XX2/-10.D0,-7.D0,2*-4.D0,0.D0,2*4.D0,7.D0,10.D0,5*0.D0/
      DATA YY2/3.D0,6.D0,3.D0,9.D0,6.D0,3.D0,9.D0,6.D0,3.D0,5*0.D0/
      DATA ZZ2/2*20.D0,4.D0,20.D0,2*4.D0,3*20.D0,2.D0,3.D0,4.5D0,
     *  7.D0,10.D0/
C
      DATA RH,DR /9.D0,4.D0/   !  RH IS THE "HINGING DISTANCE" AND DR IS THE
C				 TRANSITION SCALE LENGTH, DEFINING THE
C				 CURVATURE  OF THE WARPING (SEE P.89, NB #2)
C
      DATA XLTDAY,XLTNGHT /78.D0,70.D0/  !  THESE ARE LATITUDES OF THE R-1 OVAL
C					      AT NOON AND AT MIDNIGHT
      DATA DTET0 /0.034906/   !   THIS IS THE LATITUDINAL HALF-THICKNESS OF THE
C				   R-1 OVAL (THE INTERPOLATION REGION BETWEEN
C				     THE HIGH-LAT. AND THE PLASMA SHEET)
C
        TNOONN=(90.D0-XLTDAY)*0.01745329D0
        TNOONS=3.141592654D0-TNOONN	! HERE WE ASSUME THAT THE POSITIONS OF
C					   THE NORTHERN AND SOUTHERN R-1 OVALS
C					   ARE SYMMETRIC IN THE SM-COORDINATES
        DTETDN=(XLTDAY-XLTNGHT)*0.01745329D0
        DR2=DR**2
C
      SPS=DSIN(PS)
      R2=X**2+Y**2+Z**2
      R=DSQRT(R2)
      R3=R*R2
C
      RMRH=R-RH
      RPRH=R+RH
      SQM=DSQRT(RMRH**2+DR2)
      SQP=DSQRT(RPRH**2+DR2)
      C=SQP-SQM
      Q=DSQRT((RH+1.D0)**2+DR2)-DSQRT((RH-1.D0)**2+DR2)
      SPSAS=SPS/R*C/Q
      CPSAS=DSQRT(1.D0-SPSAS**2)
       XAS = X*CPSAS-Z*SPSAS
       ZAS = X*SPSAS+Z*CPSAS
        IF (XAS.NE.0.D0.OR.Y.NE.0.D0) THEN
          PAS = DATAN2(Y,XAS)
        			      ELSE
          PAS=0.D0
        ENDIF
C
      TAS=DATAN2(DSQRT(XAS**2+Y**2),ZAS)
      STAS=DSIN(TAS)
      F=STAS/(STAS**6*(1.D0-R3)+R3)**0.1666666667D0
C
      TET0=DASIN(F)
      IF (TAS.GT.1.5707963D0) TET0=3.141592654D0-TET0
      DTET=DTETDN*DSIN(PAS*0.5D0)**2
      TETR1N=TNOONN+DTET
      TETR1S=TNOONS-DTET
C
C NOW LET'S DEFINE WHICH OF THE FOUR REGIONS (HIGH-LAT., NORTHERN PSBL,
C   PLASMA SHEET, SOUTHERN PSBL) DOES THE POINT (X,Y,Z) BELONG TO:
C
       IF (TET0.LT.TETR1N-DTET0.OR.TET0.GT.TETR1S+DTET0)  LOC=1 ! HIGH-LAT.
       IF (TET0.GT.TETR1N+DTET0.AND.TET0.LT.TETR1S-DTET0) LOC=2 ! PL.SHEET
       IF (TET0.GE.TETR1N-DTET0.AND.TET0.LE.TETR1N+DTET0) LOC=3 ! NORTH PSBL
       IF (TET0.GE.TETR1S-DTET0.AND.TET0.LE.TETR1S+DTET0) LOC=4 ! SOUTH PSBL
C
       IF (LOC.EQ.1) THEN   ! IN THE HIGH-LAT. REGION USE THE SUBROUTINE DIPOCT
C
C      print *, '  LOC=1 (HIGH-LAT)'	!  (test printout; disabled now)
         XI(1)=X
         XI(2)=Y
         XI(3)=Z
         XI(4)=PS
         CALL  DIPLOOP1(XI,D1)
          BX=0.D0
          BY=0.D0
          BZ=0.D0
            DO 1 I=1,26
              BX=BX+C1(I)*D1(1,I)
              BY=BY+C1(I)*D1(2,I)
  1	      BZ=BZ+C1(I)*D1(3,I)
       ENDIF					       !  END OF THE CASE 1
C
       IF (LOC.EQ.2) THEN
C	    print *, '  LOC=2 (PLASMA SHEET)'  !  (test printout; disabled now)
C
         XI(1)=X
         XI(2)=Y
         XI(3)=Z
         XI(4)=PS
         CALL  CONDIP1(XI,D2)
          BX=0.D0
          BY=0.D0
          BZ=0.D0
            DO 2 I=1,79
              BX=BX+C2(I)*D2(1,I)
              BY=BY+C2(I)*D2(2,I)
  2	      BZ=BZ+C2(I)*D2(3,I)
       ENDIF					       !   END OF THE CASE 2
C
       IF (LOC.EQ.3) THEN
C	print *, '  LOC=3 (north PSBL)'  !  (test printout; disabled now)
C
         T01=TETR1N-DTET0
         T02=TETR1N+DTET0
          SQR=DSQRT(R)
          ST01AS=SQR/(R3+1.D0/DSIN(T01)**6-1.D0)**0.1666666667
          ST02AS=SQR/(R3+1.D0/DSIN(T02)**6-1.D0)**0.1666666667
          CT01AS=DSQRT(1.D0-ST01AS**2)
          CT02AS=DSQRT(1.D0-ST02AS**2)
         XAS1=R*ST01AS*DCOS(PAS)
         Y1=  R*ST01AS*DSIN(PAS)
         ZAS1=R*CT01AS
         X1=XAS1*CPSAS+ZAS1*SPSAS
         Z1=-XAS1*SPSAS+ZAS1*CPSAS ! X1,Y1,Z1 ARE COORDS OF THE NORTHERN
c						       BOUNDARY POINT
         XI(1)=X1
         XI(2)=Y1
         XI(3)=Z1
         XI(4)=PS
         CALL  DIPLOOP1(XI,D1)
          BX1=0.D0
          BY1=0.D0
          BZ1=0.D0
            DO 11 I=1,26
              BX1=BX1+C1(I)*D1(1,I) !	BX1,BY1,BZ1  ARE FIELD COMPONENTS
              BY1=BY1+C1(I)*D1(2,I)  !  IN THE NORTHERN BOUNDARY POINT
 11	      BZ1=BZ1+C1(I)*D1(3,I)  !
C
         XAS2=R*ST02AS*DCOS(PAS)
         Y2=  R*ST02AS*DSIN(PAS)
         ZAS2=R*CT02AS
         X2=XAS2*CPSAS+ZAS2*SPSAS
         Z2=-XAS2*SPSAS+ZAS2*CPSAS ! X2,Y2,Z2 ARE COORDS OF THE SOUTHERN
C					 BOUNDARY POINT
         XI(1)=X2
         XI(2)=Y2
         XI(3)=Z2
         XI(4)=PS
         CALL  CONDIP1(XI,D2)
          BX2=0.D0
          BY2=0.D0
          BZ2=0.D0
            DO 12 I=1,79
              BX2=BX2+C2(I)*D2(1,I)!  BX2,BY2,BZ2  ARE FIELD COMPONENTS
              BY2=BY2+C2(I)*D2(2,I) !  IN THE SOUTHERN BOUNDARY POINT
  12	      BZ2=BZ2+C2(I)*D2(3,I)
C
C  NOW INTERPOLATE:
C
         SS=DSQRT((X2-X1)**2+(Y2-Y1)**2+(Z2-Z1)**2)
         DS=DSQRT((X-X1)**2+(Y-Y1)**2+(Z-Z1)**2)
         FRAC=DS/SS
         BX=BX1*(1.D0-FRAC)+BX2*FRAC
         BY=BY1*(1.D0-FRAC)+BY2*FRAC
         BZ=BZ1*(1.D0-FRAC)+BZ2*FRAC
C
        ENDIF						   ! END OF THE CASE 3
C
        IF (LOC.EQ.4) THEN
C	print *, '  LOC=4 (south PSBL)'  !  (test printout; disabled now)
C
         T01=TETR1S-DTET0
         T02=TETR1S+DTET0
          SQR=DSQRT(R)
          ST01AS=SQR/(R3+1.D0/DSIN(T01)**6-1.D0)**0.1666666667
          ST02AS=SQR/(R3+1.D0/DSIN(T02)**6-1.D0)**0.1666666667
          CT01AS=-DSQRT(1.D0-ST01AS**2)
          CT02AS=-DSQRT(1.D0-ST02AS**2)
         XAS1=R*ST01AS*DCOS(PAS)
         Y1=  R*ST01AS*DSIN(PAS)
         ZAS1=R*CT01AS
         X1=XAS1*CPSAS+ZAS1*SPSAS
         Z1=-XAS1*SPSAS+ZAS1*CPSAS ! X1,Y1,Z1 ARE COORDS OF THE NORTHERN
C						BOUNDARY POINT
         XI(1)=X1
         XI(2)=Y1
         XI(3)=Z1
         XI(4)=PS
         CALL  CONDIP1(XI,D2)
          BX1=0.D0
          BY1=0.D0
          BZ1=0.D0
            DO 21 I=1,79
              BX1=BX1+C2(I)*D2(1,I) !  BX1,BY1,BZ1  ARE FIELD COMPONENTS
              BY1=BY1+C2(I)*D2(2,I)  !  IN THE NORTHERN BOUNDARY POINT
 21	      BZ1=BZ1+C2(I)*D2(3,I)  !
C
         XAS2=R*ST02AS*DCOS(PAS)
         Y2=  R*ST02AS*DSIN(PAS)
         ZAS2=R*CT02AS
         X2=XAS2*CPSAS+ZAS2*SPSAS
         Z2=-XAS2*SPSAS+ZAS2*CPSAS ! X2,Y2,Z2 ARE COORDS OF THE SOUTHERN
C					   BOUNDARY POINT
         XI(1)=X2
         XI(2)=Y2
         XI(3)=Z2
         XI(4)=PS
         CALL  DIPLOOP1(XI,D1)
          BX2=0.D0
          BY2=0.D0
          BZ2=0.D0
            DO 22 I=1,26
              BX2=BX2+C1(I)*D1(1,I) !  BX2,BY2,BZ2  ARE FIELD COMPONENTS
              BY2=BY2+C1(I)*D1(2,I) !	  IN THE SOUTHERN BOUNDARY POINT
  22	      BZ2=BZ2+C1(I)*D1(3,I)
C
C  NOW INTERPOLATE:
C
         SS=DSQRT((X2-X1)**2+(Y2-Y1)**2+(Z2-Z1)**2)
         DS=DSQRT((X-X1)**2+(Y-Y1)**2+(Z-Z1)**2)
         FRAC=DS/SS
         BX=BX1*(1.D0-FRAC)+BX2*FRAC
         BY=BY1*(1.D0-FRAC)+BY2*FRAC
         BZ=BZ1*(1.D0-FRAC)+BZ2*FRAC
C
        ENDIF					     ! END OF THE CASE 4
C
C   NOW, LET US ADD THE SHIELDING FIELD
C
        CALL  BIRK1SHLD(PS,X,Y,Z,BSX,BSY,BSZ)
        BX=BX+BSX
        BY=BY+BSY
        BZ=BZ+BSZ
         RETURN
         END
C
C------------------------------------------------------------------------------
C
C
         SUBROUTINE  DIPLOOP1(XI,D)
C
C
C      Calculates dependent model variables and their deriva-
C  tives for given independent variables and model parame-
C  ters.  Specifies model functions with free parameters which
C  must be determined by means of least squares fits (RMS
C  minimization procedure).
C
C      Description of parameters:
C
C  XI  - input vector containing independent variables;
C  D   - output double precision vector containing
C	 calculated values for derivatives of dependent
C	 variables with respect to LINEAR model parameters;
C
C - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
C
c  The  26 coefficients are moments (Z- and X-components) of 12 dipoles placed
C    inside the  R1-shell,  PLUS amplitudes of two octagonal double loops.
C     The dipoles with nonzero  Yi appear in pairs with equal moments.
c		   (see the notebook #2, pp.102-103, for details)
C - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
c
         IMPLICIT  REAL*8  (A - H, O - Z)
C
         COMMON /COORD11/ XX(12),YY(12)
         COMMON /LOOPDIP1/ TILT,XCENTRE(2),RADIUS(2),  DIPX,DIPY
         COMMON /RHDR/RH,DR
         DIMENSION XI(4),D(3,26)
C
           X = XI(1)
           Y = XI(2)
           Z = XI(3)
           PS= XI(4)
           SPS=DSIN(PS)
C
         DO 1 I=1,12
           R2=(XX(I)*DIPX)**2+(YY(I)*DIPY)**2
           R=DSQRT(R2)
             RMRH=R-RH
             RPRH=R+RH
             DR2=DR**2
             SQM=DSQRT(RMRH**2+DR2)
             SQP=DSQRT(RPRH**2+DR2)
             C=SQP-SQM
             Q=DSQRT((RH+1.D0)**2+DR2)-DSQRT((RH-1.D0)**2+DR2)
             SPSAS=SPS/R*C/Q
             CPSAS=DSQRT(1.D0-SPSAS**2)
         XD= (XX(I)*DIPX)*CPSAS
         YD= (YY(I)*DIPY)
         ZD=-(XX(I)*DIPX)*SPSAS
      CALL DIPXYZ(X-XD,Y-YD,Z-ZD,BX1X,BY1X,BZ1X,BX1Y,BY1Y,BZ1Y,
     *  BX1Z,BY1Z,BZ1Z)
        IF (DABS(YD).GT.1.D-10) THEN
      CALL DIPXYZ(X-XD,Y+YD,Z-ZD,BX2X,BY2X,BZ2X,BX2Y,BY2Y,BZ2Y,
     *  BX2Z,BY2Z,BZ2Z)
        			   ELSE
        BX2X=0.D0
        BY2X=0.D0
        BZ2X=0.D0
C
        BX2Z=0.D0
        BY2Z=0.D0
        BZ2Z=0.D0
        			   ENDIF
C
            D(1,I)=BX1Z+BX2Z
            D(2,I)=BY1Z+BY2Z
            D(3,I)=BZ1Z+BZ2Z
            D(1,I+12)=(BX1X+BX2X)*SPS
            D(2,I+12)=(BY1X+BY2X)*SPS
            D(3,I+12)=(BZ1X+BZ2X)*SPS
  1   CONTINUE
c
           R2=(XCENTRE(1)+RADIUS(1))**2
           R=DSQRT(R2)
             RMRH=R-RH
             RPRH=R+RH
             DR2=DR**2
             SQM=DSQRT(RMRH**2+DR2)
             SQP=DSQRT(RPRH**2+DR2)
             C=SQP-SQM
             Q=DSQRT((RH+1.D0)**2+DR2)-DSQRT((RH-1.D0)**2+DR2)
             SPSAS=SPS/R*C/Q
             CPSAS=DSQRT(1.D0-SPSAS**2)
         XOCT1= X*CPSAS-Z*SPSAS
         YOCT1= Y
         ZOCT1= X*SPSAS+Z*CPSAS
C
      CALL CROSSLP(XOCT1,YOCT1,ZOCT1,BXOCT1,BYOCT1,BZOCT1,XCENTRE(1),
     *        RADIUS(1),TILT)
            D(1,25)=BXOCT1*CPSAS+BZOCT1*SPSAS
            D(2,25)=BYOCT1
            D(3,25)=-BXOCT1*SPSAS+BZOCT1*CPSAS
C
           R2=(RADIUS(2)-XCENTRE(2))**2
           R=DSQRT(R2)
             RMRH=R-RH
             RPRH=R+RH
             DR2=DR**2
             SQM=DSQRT(RMRH**2+DR2)
             SQP=DSQRT(RPRH**2+DR2)
             C=SQP-SQM
             Q=DSQRT((RH+1.D0)**2+DR2)-DSQRT((RH-1.D0)**2+DR2)
             SPSAS=SPS/R*C/Q
             CPSAS=DSQRT(1.D0-SPSAS**2)
         XOCT2= X*CPSAS-Z*SPSAS -XCENTRE(2)
         YOCT2= Y
         ZOCT2= X*SPSAS+Z*CPSAS
            CALL CIRCLE(XOCT2,YOCT2,ZOCT2,RADIUS(2),BX,BY,BZ)
            D(1,26) =  BX*CPSAS+BZ*SPSAS
            D(2,26) =  BY
            D(3,26) = -BX*SPSAS+BZ*CPSAS
C
            RETURN
            END
c-------------------------------------------------------------------------
C
        SUBROUTINE CIRCLE(X,Y,Z,RL,BX,BY,BZ)
C
C  RETURNS COMPONENTS OF THE FIELD FROM A CIRCULAR CURRENT LOOP OF RADIUS RL
C  USES THE SECOND (MORE ACCURATE) APPROXIMATION GIVEN IN ABRAMOWITZ AND STEGUN

        IMPLICIT REAL*8 (A-H,O-Z)
        REAL*8 K
        DATA PI/3.141592654D0/
C
        RHO2=X*X+Y*Y
        RHO=DSQRT(RHO2)
        R22=Z*Z+(RHO+RL)**2
        R2=DSQRT(R22)
        R12=R22-4.D0*RHO*RL
        R32=0.5D0*(R12+R22)
        XK2=1.D0-R12/R22
        XK2S=1.D0-XK2
        DL=DLOG(1.D0/XK2S)
        K=1.38629436112d0+XK2S*(0.09666344259D0+XK2S*(0.03590092383+
     *     XK2S*(0.03742563713+XK2S*0.01451196212))) +DL*
     *     (0.5D0+XK2S*(0.12498593597D0+XK2S*(0.06880248576D0+
     *     XK2S*(0.03328355346D0+XK2S*0.00441787012D0))))
        E=1.D0+XK2S*(0.44325141463D0+XK2S*(0.0626060122D0+XK2S*
     *      (0.04757383546D0+XK2S*0.01736506451D0))) +DL*
     *     XK2S*(0.2499836831D0+XK2S*(0.09200180037D0+XK2S*
     *       (0.04069697526D0+XK2S*0.00526449639D0)))

        IF (RHO.GT.1.D-6) THEN
           BRHO=Z/(RHO2*R2)*(R32/R12*E-K) !  THIS IS NOT EXACTLY THE B-RHO COM-
        		   ELSE 	  !   PONENT - NOTE THE ADDITIONAL
           BRHO=PI*RL/R2*(RL-RHO)/R12*Z/(R32-RHO2)  !	   DIVISION BY RHO
        ENDIF

        BX=BRHO*X
        BY=BRHO*Y
        BZ=(K-E*(R32-2.D0*RL*RL)/R12)/R2
        RETURN
        END
C-------------------------------------------------------------
C
        SUBROUTINE CROSSLP(X,Y,Z,BX,BY,BZ,XC,RL,AL)
C
c   RETURNS FIELD COMPONENTS OF A PAIR OF LOOPS WITH A COMMON CENTER AND
C    DIAMETER,  COINCIDING WITH THE X AXIS. THE LOOPS ARE INCLINED TO THE
C    EQUATORIAL PLANE BY THE ANGLE AL (RADIANS) AND SHIFTED IN THE POSITIVE
C     X-DIRECTION BY THE DISTANCE  XC.
c
        IMPLICIT REAL*8 (A-H,O-Z)
C
            CAL=DCOS(AL)
            SAL=DSIN(AL)
C
        Y1=Y*CAL-Z*SAL
        Z1=Y*SAL+Z*CAL
        Y2=Y*CAL+Z*SAL
        Z2=-Y*SAL+Z*CAL
        CALL CIRCLE(X-XC,Y1,Z1,RL,BX1,BY1,BZ1)
        CALL CIRCLE(X-XC,Y2,Z2,RL,BX2,BY2,BZ2)
        BX=BX1+BX2
        BY= (BY1+BY2)*CAL+(BZ1-BZ2)*SAL
        BZ=-(BY1-BY2)*SAL+(BZ1+BZ2)*CAL
C
        RETURN
        END

C*******************************************************************

       SUBROUTINE DIPXYZ(X,Y,Z,BXX,BYX,BZX,BXY,BYY,BZY,BXZ,BYZ,BZZ)
C
C	RETURNS THE FIELD COMPONENTS PRODUCED BY THREE DIPOLES, EACH
C	 HAVING M=Me AND ORIENTED PARALLEL TO X,Y, and Z AXIS, RESP.
C
      IMPLICIT REAL*8 (A-H,O-Z)
C
      X2=X**2
      Y2=Y**2
      Z2=Z**2
      R2=X2+Y2+Z2

      XMR5=30574.D0/(R2*R2*DSQRT(R2))
      XMR53=3.D0*XMR5
      BXX=XMR5*(3.D0*X2-R2)
      BYX=XMR53*X*Y
      BZX=XMR53*X*Z
C
      BXY=BYX
      BYY=XMR5*(3.D0*Y2-R2)
      BZY=XMR53*Y*Z
C
      BXZ=BZX
      BYZ=BZY
      BZZ=XMR5*(3.D0*Z2-R2)
C
      RETURN
      END
C
C------------------------------------------------------------------------------
         SUBROUTINE  CONDIP1(XI,D)
C
C      Calculates dependent model variables and their derivatives for given
C  independent variables and model parameters.  Specifies model functions with
C  free parameters which must be determined by means of least squares fits
C  (RMS minimization procedure).
C
C      Description of parameters:
C
C  XI  - input vector containing independent variables;
C  D   - output double precision vector containing
C	 calculated values for derivatives of dependent
C	 variables with respect to LINEAR model parameters;
C
C - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
C
c  The  79 coefficients are (1) 5 amplitudes of the conical harmonics, plus
c			    (2) (9x3+5x2)x2=74 components of the dipole moments
c	       (see the notebook #2, pp.113-..., for details)
C - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
c
         IMPLICIT  REAL*8  (A - H, O - Z)
C
      COMMON /DX1/ DX,SCALEIN,SCALEOUT
      COMMON /COORD21/ XX(14),YY(14),ZZ(14)
c
         DIMENSION XI(4),D(3,79),CF(5),SF(5)
C
           X = XI(1)
           Y = XI(2)
           Z = XI(3)
           PS= XI(4)
           SPS=DSIN(PS)
           CPS=DCOS(PS)
C
      XSM=X*CPS-Z*SPS  - DX
      ZSM=Z*CPS+X*SPS
      RO2=XSM**2+Y**2
      RO=DSQRT(RO2)
C
      CF(1)=XSM/RO
      SF(1)=Y/RO
C
      CF(2)=CF(1)**2-SF(1)**2
      SF(2)=2.*SF(1)*CF(1)
      CF(3)=CF(2)*CF(1)-SF(2)*SF(1)
      SF(3)=SF(2)*CF(1)+CF(2)*SF(1)
      CF(4)=CF(3)*CF(1)-SF(3)*SF(1)
      SF(4)=SF(3)*CF(1)+CF(3)*SF(1)
      CF(5)=CF(4)*CF(1)-SF(4)*SF(1)
      SF(5)=SF(4)*CF(1)+CF(4)*SF(1)
C
      R2=RO2+ZSM**2
      R=DSQRT(R2)
      C=ZSM/R
      S=RO/R
      CH=DSQRT(0.5D0*(1.D0+C))
      SH=DSQRT(0.5D0*(1.D0-C))
      TNH=SH/CH
      CNH=1.D0/TNH
C
      DO 1 M=1,5
       BT=M*CF(M)/(R*S)*(TNH**M+CNH**M)
       BF=-0.5D0*M*SF(M)/R*(TNH**(M-1)/CH**2-CNH**(M-1)/SH**2)
       BXSM=BT*C*CF(1)-BF*SF(1)
       BY=BT*C*SF(1)+BF*CF(1)
       BZSM=-BT*S
C
       D(1,M)=BXSM*CPS+BZSM*SPS
       D(2,M)=BY
  1    D(3,M)=-BXSM*SPS+BZSM*CPS
C
      XSM = X*CPS-Z*SPS
      ZSM = Z*CPS+X*SPS
C
        DO 2 I=1,9
C
        IF (I.EQ.3.OR.I.EQ.5.OR.I.EQ.6) THEN
        	XD =  XX(I)*SCALEIN
        	YD =  YY(I)*SCALEIN
        				 ELSE
        	XD =  XX(I)*SCALEOUT
        	YD =  YY(I)*SCALEOUT
        ENDIF
C
         ZD =  ZZ(I)
C
      CALL DIPXYZ(XSM-XD,Y-YD,ZSM-ZD,BX1X,BY1X,BZ1X,BX1Y,BY1Y,BZ1Y,
     *  BX1Z,BY1Z,BZ1Z)
      CALL DIPXYZ(XSM-XD,Y+YD,ZSM-ZD,BX2X,BY2X,BZ2X,BX2Y,BY2Y,BZ2Y,
     *  BX2Z,BY2Z,BZ2Z)
      CALL DIPXYZ(XSM-XD,Y-YD,ZSM+ZD,BX3X,BY3X,BZ3X,BX3Y,BY3Y,BZ3Y,
     *  BX3Z,BY3Z,BZ3Z)
      CALL DIPXYZ(XSM-XD,Y+YD,ZSM+ZD,BX4X,BY4X,BZ4X,BX4Y,BY4Y,BZ4Y,
     *  BX4Z,BY4Z,BZ4Z)
C
      IX=I*3+3
      IY=IX+1
      IZ=IY+1
C
      D(1,IX)=(BX1X+BX2X-BX3X-BX4X)*CPS+(BZ1X+BZ2X-BZ3X-BZ4X)*SPS
      D(2,IX)= BY1X+BY2X-BY3X-BY4X
      D(3,IX)=(BZ1X+BZ2X-BZ3X-BZ4X)*CPS-(BX1X+BX2X-BX3X-BX4X)*SPS
C
      D(1,IY)=(BX1Y-BX2Y-BX3Y+BX4Y)*CPS+(BZ1Y-BZ2Y-BZ3Y+BZ4Y)*SPS
      D(2,IY)= BY1Y-BY2Y-BY3Y+BY4Y
      D(3,IY)=(BZ1Y-BZ2Y-BZ3Y+BZ4Y)*CPS-(BX1Y-BX2Y-BX3Y+BX4Y)*SPS
C
      D(1,IZ)=(BX1Z+BX2Z+BX3Z+BX4Z)*CPS+(BZ1Z+BZ2Z+BZ3Z+BZ4Z)*SPS
      D(2,IZ)= BY1Z+BY2Z+BY3Z+BY4Z
      D(3,IZ)=(BZ1Z+BZ2Z+BZ3Z+BZ4Z)*CPS-(BX1Z+BX2Z+BX3Z+BX4Z)*SPS
C
      IX=IX+27
      IY=IY+27
      IZ=IZ+27
C
      D(1,IX)=SPS*((BX1X+BX2X+BX3X+BX4X)*CPS+(BZ1X+BZ2X+BZ3X+BZ4X)*SPS)
      D(2,IX)=SPS*(BY1X+BY2X+BY3X+BY4X)
      D(3,IX)=SPS*((BZ1X+BZ2X+BZ3X+BZ4X)*CPS-(BX1X+BX2X+BX3X+BX4X)*SPS)
C
      D(1,IY)=SPS*((BX1Y-BX2Y+BX3Y-BX4Y)*CPS+(BZ1Y-BZ2Y+BZ3Y-BZ4Y)*SPS)
      D(2,IY)=SPS*(BY1Y-BY2Y+BY3Y-BY4Y)
      D(3,IY)=SPS*((BZ1Y-BZ2Y+BZ3Y-BZ4Y)*CPS-(BX1Y-BX2Y+BX3Y-BX4Y)*SPS)
C
      D(1,IZ)=SPS*((BX1Z+BX2Z-BX3Z-BX4Z)*CPS+(BZ1Z+BZ2Z-BZ3Z-BZ4Z)*SPS)
      D(2,IZ)=SPS*(BY1Z+BY2Z-BY3Z-BY4Z)
      D(3,IZ)=SPS*((BZ1Z+BZ2Z-BZ3Z-BZ4Z)*CPS-(BX1Z+BX2Z-BX3Z-BX4Z)*SPS)
  2   CONTINUE
C
      DO 3 I=1,5
      ZD=ZZ(I+9)
      CALL DIPXYZ(XSM,Y,ZSM-ZD,BX1X,BY1X,BZ1X,BX1Y,BY1Y,BZ1Y,BX1Z,BY1Z,
     *  BZ1Z)
      CALL DIPXYZ(XSM,Y,ZSM+ZD,BX2X,BY2X,BZ2X,BX2Y,BY2Y,BZ2Y,BX2Z,BY2Z,
     *  BZ2Z)
       IX=58+I*2
       IZ=IX+1
      D(1,IX)=(BX1X-BX2X)*CPS+(BZ1X-BZ2X)*SPS
      D(2,IX)=BY1X-BY2X
      D(3,IX)=(BZ1X-BZ2X)*CPS-(BX1X-BX2X)*SPS
C
      D(1,IZ)=(BX1Z+BX2Z)*CPS+(BZ1Z+BZ2Z)*SPS
      D(2,IZ)=BY1Z+BY2Z
      D(3,IZ)=(BZ1Z+BZ2Z)*CPS-(BX1Z+BX2Z)*SPS
C
      IX=IX+10
      IZ=IZ+10
      D(1,IX)=SPS*((BX1X+BX2X)*CPS+(BZ1X+BZ2X)*SPS)
      D(2,IX)=SPS*(BY1X+BY2X)
      D(3,IX)=SPS*((BZ1X+BZ2X)*CPS-(BX1X+BX2X)*SPS)
C
      D(1,IZ)=SPS*((BX1Z-BX2Z)*CPS+(BZ1Z-BZ2Z)*SPS)
      D(2,IZ)=SPS*(BY1Z-BY2Z)
  3   D(3,IZ)=SPS*((BZ1Z-BZ2Z)*CPS-(BX1Z-BX2Z)*SPS)
C
            RETURN
            END
C
C$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$
C
         SUBROUTINE  BIRK1SHLD(PS,X,Y,Z,BX,BY,BZ)
C
C - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
C
C  The 64 linear parameters are amplitudes of the "box" harmonics.
c The 16 nonlinear parameters are the scales Pi, and Qk entering the arguments
C  of sines/cosines and exponents in each of  32 cartesian harmonics
c  N.A. Tsyganenko, Spring 1994, adjusted for the Birkeland field Aug.22, 1995
c    Revised  June 12, 1996.
C - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
C
      IMPLICIT  REAL*8  (A - H, O - Z)
C
      DIMENSION A(80)
      DIMENSION P1(4),R1(4),Q1(4),S1(4),RP(4),RR(4),RQ(4),RS(4)
C
      EQUIVALENCE (P1(1),A(65)),(R1(1),A(69)),(Q1(1),A(73)),
     * (S1(1),A(77))
C
      DATA A/1.174198045,-1.463820502,4.840161537,-3.674506864,
     * 82.18368896,-94.94071588,-4122.331796,4670.278676,-21.54975037,
     * 26.72661293,-72.81365728,44.09887902,40.08073706,-51.23563510,
     * 1955.348537,-1940.971550,794.0496433,-982.2441344,1889.837171,
     * -558.9779727,-1260.543238,1260.063802,-293.5942373,344.7250789,
     * -773.7002492,957.0094135,-1824.143669,520.7994379,1192.484774,
     * -1192.184565,89.15537624,-98.52042999,-0.8168777675E-01,
     * 0.4255969908E-01,0.3155237661,-0.3841755213,2.494553332,
     * -0.6571440817E-01,-2.765661310,0.4331001908,0.1099181537,
     * -0.6154126980E-01,-0.3258649260,0.6698439193,-5.542735524,
     * 0.1604203535,5.854456934,-0.8323632049,3.732608869,-3.130002153,
     * 107.0972607,-32.28483411,-115.2389298,54.45064360,-0.5826853320,
     * -3.582482231,-4.046544561,3.311978102,-104.0839563,30.26401293,
     * 97.29109008,-50.62370872,-296.3734955,127.7872523,5.303648988,
     * 10.40368955,69.65230348,466.5099509,1.645049286,3.825838190,
     * 11.66675599,558.9781177,1.826531343,2.066018073,25.40971369,
     * 990.2795225,2.319489258,4.555148484,9.691185703,591.8280358/
C
         BX=0.D0
         BY=0.D0
         BZ=0.D0
         CPS=DCOS(PS)
         SPS=DSIN(PS)
         S3PS=4.D0*CPS**2-1.D0
C
         DO 11 I=1,4
          RP(I)=1.D0/P1(I)
          RR(I)=1.D0/R1(I)
          RQ(I)=1.D0/Q1(I)
 11	  RS(I)=1.D0/S1(I)
C
          L=0
C
           DO 1 M=1,2	  !    M=1 IS FOR THE 1ST SUM ("PERP." SYMMETRY)
C			    AND M=2 IS FOR THE SECOND SUM ("PARALL." SYMMETRY)
             DO 2 I=1,4
        	  CYPI=DCOS(Y*RP(I))
        	  CYQI=DCOS(Y*RQ(I))
        	  SYPI=DSIN(Y*RP(I))
        	  SYQI=DSIN(Y*RQ(I))
C
        	DO 3 K=1,4
        	   SZRK=DSIN(Z*RR(K))
        	   CZSK=DCOS(Z*RS(K))
        	   CZRK=DCOS(Z*RR(K))
        	   SZSK=DSIN(Z*RS(K))
        	     SQPR=DSQRT(RP(I)**2+RR(K)**2)
        	     SQQS=DSQRT(RQ(I)**2+RS(K)**2)
        		EPR=DEXP(X*SQPR)
        		EQS=DEXP(X*SQQS)
C
        	    DO 4 N=1,2  ! N=1 IS FOR THE FIRST PART OF EACH COEFFICIENT
C				   AND N=2 IS FOR THE SECOND ONE
        	     IF (M.EQ.1) THEN
        	       IF (N.EQ.1) THEN
        		 HX=-SQPR*EPR*CYPI*SZRK
        		 HY=RP(I)*EPR*SYPI*SZRK
        		 HZ=-RR(K)*EPR*CYPI*CZRK
        			   ELSE
        		 HX=HX*CPS
        		 HY=HY*CPS
        		 HZ=HZ*CPS
        			   ENDIF
        	     ELSE
        	       IF (N.EQ.1) THEN
        		 HX=-SPS*SQQS*EQS*CYQI*CZSK
        		 HY=SPS*RQ(I)*EQS*SYQI*CZSK
        		 HZ=SPS*RS(K)*EQS*CYQI*SZSK
        			   ELSE
        		 HX=HX*S3PS
        		 HY=HY*S3PS
        		 HZ=HZ*S3PS
        	       ENDIF
        	 ENDIF
       L=L+1
c
       BX=BX+A(L)*HX
       BY=BY+A(L)*HY
  4    BZ=BZ+A(L)*HZ
  3   CONTINUE
  2   CONTINUE
  1   CONTINUE
C
         RETURN
         END
C
C##########################################################################
C
         SUBROUTINE BIRK2TOT_02(PS,X,Y,Z,BX,BY,BZ)
C
          IMPLICIT REAL*8 (A-H,O-Z)
C
          CALL BIRK2SHL(X,Y,Z,PS,WX,WY,WZ)
          CALL R2_BIRK(X,Y,Z,PS,HX,HY,HZ)
         BX=WX+HX
         BY=WY+HY
         BZ=WZ+HZ
         RETURN
         END
C
C$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$
C
C THIS CODE IS FOR THE FIELD FROM  2x2x2=8 "CARTESIAN" HARMONICS
C
         SUBROUTINE  BIRK2SHL(X,Y,Z,PS,HX,HY,HZ)
C
C - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
C    The model parameters are provided to this module via common-block /A/.
C  The 16 linear parameters enter in pairs in the amplitudes of the
c	"cartesian" harmonics.
c    The 8 nonlinear parameters are the scales Pi,Ri,Qi,and Si entering the
c  arguments of exponents, sines, and cosines in each of the 8 "Cartesian"
c   harmonics
C - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
C
         IMPLICIT  REAL*8  (A - H, O - Z)
C
         DIMENSION P(2),R(2),Q(2),S(2)
         DIMENSION A(24)
C
         EQUIVALENCE(P(1),A(17)),(R(1),A(19)),(Q(1),A(21)),(S(1),A(23))
         DATA A/-111.6371348,124.5402702,110.3735178,-122.0095905,
     * 111.9448247,-129.1957743,-110.7586562,126.5649012,-0.7865034384,
     * -0.2483462721,0.8026023894,0.2531397188,10.72890902,0.8483902118,
     * -10.96884315,-0.8583297219,13.85650567,14.90554500,10.21914434,
     * 10.09021632,6.340382460,14.40432686,12.71023437,12.83966657/
C
            CPS=DCOS(PS)
            SPS=DSIN(PS)
            S3PS=4.D0*CPS**2-1.D0   !  THIS IS SIN(3*PS)/SIN(PS)
C
           HX=0.D0
           HY=0.D0
           HZ=0.D0
           L=0
C
           DO 1 M=1,2	  !    M=1 IS FOR THE 1ST SUM ("PERP." SYMMETRY)
C			    AND M=2 IS FOR THE SECOND SUM ("PARALL." SYMMETRY)
             DO 2 I=1,2
        	  CYPI=DCOS(Y/P(I))
        	  CYQI=DCOS(Y/Q(I))
        	  SYPI=DSIN(Y/P(I))
        	  SYQI=DSIN(Y/Q(I))
C
               DO 3 K=1,2
        	   SZRK=DSIN(Z/R(K))
        	   CZSK=DCOS(Z/S(K))
        	   CZRK=DCOS(Z/R(K))
        	   SZSK=DSIN(Z/S(K))
        	     SQPR=DSQRT(1.D0/P(I)**2+1.D0/R(K)**2)
        	     SQQS=DSQRT(1.D0/Q(I)**2+1.D0/S(K)**2)
        		EPR=DEXP(X*SQPR)
        		EQS=DEXP(X*SQQS)
C
        	   DO 4 N=1,2  ! N=1 IS FOR THE FIRST PART OF EACH COEFFICIENT
C				   AND N=2 IS FOR THE SECOND ONE
C
        	    L=L+1
        	     IF (M.EQ.1) THEN
        	       IF (N.EQ.1) THEN
        		 DX=-SQPR*EPR*CYPI*SZRK
        		 DY=EPR/P(I)*SYPI*SZRK
        		 DZ=-EPR/R(K)*CYPI*CZRK
        		 HX=HX+A(L)*DX
        		 HY=HY+A(L)*DY
        		 HZ=HZ+A(L)*DZ
        			   ELSE
        		 DX=DX*CPS
        		 DY=DY*CPS
        		 DZ=DZ*CPS
        		 HX=HX+A(L)*DX
        		 HY=HY+A(L)*DY
        		 HZ=HZ+A(L)*DZ
        			   ENDIF
        	     ELSE
        	       IF (N.EQ.1) THEN
        		 DX=-SPS*SQQS*EQS*CYQI*CZSK
        		 DY=SPS*EQS/Q(I)*SYQI*CZSK
        		 DZ=SPS*EQS/S(K)*CYQI*SZSK
        		 HX=HX+A(L)*DX
        		 HY=HY+A(L)*DY
        		 HZ=HZ+A(L)*DZ
        			   ELSE
        		 DX=DX*S3PS
        		 DY=DY*S3PS
        		 DZ=DZ*S3PS
        		 HX=HX+A(L)*DX
        		 HY=HY+A(L)*DY
        		 HZ=HZ+A(L)*DZ
        	       ENDIF
        	 ENDIF
c
  4   CONTINUE
  3   CONTINUE
  2   CONTINUE
  1   CONTINUE
C
         RETURN
         END

c********************************************************************
C
       SUBROUTINE R2_BIRK(X,Y,Z,PS,BX,BY,BZ)
C
C  RETURNS THE MODEL FIELD FOR THE REGION 2 BIRKELAND CURRENT/PARTIAL RC
C    (WITHOUT SHIELDING FIELD)
C
       IMPLICIT REAL*8 (A-H,O-Z)
       SAVE PSI,CPS,SPS
       DATA DELARG/0.030D0/,DELARG1/0.015D0/,PSI/10.D0/
C
       IF (DABS(PSI-PS).GT.1.D-10) THEN
         PSI=PS
         CPS=DCOS(PS)
         SPS=DSIN(PS)
       ENDIF
C
       XSM=X*CPS-Z*SPS
       ZSM=Z*CPS+X*SPS
C
       XKS=XKSI(XSM,Y,ZSM)
      IF (XKS.LT.-(DELARG+DELARG1)) THEN
        CALL R2OUTER(XSM,Y,ZSM,BXSM,BY,BZSM)
         BXSM=-BXSM*0.02      !  ALL COMPONENTS ARE MULTIPLIED BY THE
         BY=-BY*0.02	      !  FACTOR -0.02, IN ORDER TO NORMALIZE THE
         BZSM=-BZSM*0.02      !  FIELD (SO THAT Bz=-1 nT at X=-5.3 RE, Y=Z=0)
      ENDIF
      IF (XKS.GE.-(DELARG+DELARG1).AND.XKS.LT.-DELARG+DELARG1) THEN
        CALL R2OUTER(XSM,Y,ZSM,BXSM1,BY1,BZSM1)
        CALL R2SHEET(XSM,Y,ZSM,BXSM2,BY2,BZSM2)
        F2=-0.02*TKSI(XKS,-DELARG,DELARG1)
        F1=-0.02-F2
        BXSM=BXSM1*F1+BXSM2*F2
        BY=BY1*F1+BY2*F2
        BZSM=BZSM1*F1+BZSM2*F2
      ENDIF

      IF (XKS.GE.-DELARG+DELARG1.AND.XKS.LT.DELARG-DELARG1) THEN
       CALL R2SHEET(XSM,Y,ZSM,BXSM,BY,BZSM)
         BXSM=-BXSM*0.02
         BY=-BY*0.02
         BZSM=-BZSM*0.02
      ENDIF
      IF (XKS.GE.DELARG-DELARG1.AND.XKS.LT.DELARG+DELARG1) THEN
        CALL R2INNER(XSM,Y,ZSM,BXSM1,BY1,BZSM1)
        CALL R2SHEET(XSM,Y,ZSM,BXSM2,BY2,BZSM2)
        F1=-0.02*TKSI(XKS,DELARG,DELARG1)
        F2=-0.02-F1
        BXSM=BXSM1*F1+BXSM2*F2
        BY=BY1*F1+BY2*F2
        BZSM=BZSM1*F1+BZSM2*F2
      ENDIF
      IF (XKS.GE.DELARG+DELARG1) THEN
         CALL R2INNER(XSM,Y,ZSM,BXSM,BY,BZSM)
         BXSM=-BXSM*0.02
         BY=-BY*0.02
         BZSM=-BZSM*0.02
      ENDIF
C
        BX=BXSM*CPS+BZSM*SPS
        BZ=BZSM*CPS-BXSM*SPS
C
        RETURN
        END
C
C****************************************************************

c
      SUBROUTINE R2INNER (X,Y,Z,BX,BY,BZ)
C
C
      IMPLICIT REAL*8 (A-H,O-Z)
      DIMENSION CBX(5),CBY(5),CBZ(5)
C
      DATA PL1,PL2,PL3,PL4,PL5,PL6,PL7,PL8/154.185,-2.12446,.601735E-01,
     * -.153954E-02,.355077E-04,29.9996,262.886,99.9132/
      DATA PN1,PN2,PN3,PN4,PN5,PN6,PN7,PN8/-8.1902,6.5239,5.504,7.7815,
     * .8573,3.0986,.0774,-.038/
C
      CALL BCONIC(X,Y,Z,CBX,CBY,CBZ,5)
C
C   NOW INTRODUCE  ONE  4-LOOP SYSTEM:
C
       CALL LOOPS4(X,Y,Z,DBX8,DBY8,DBZ8,PN1,PN2,PN3,PN4,PN5,PN6)
C
       CALL DIPDISTR(X-PN7,Y,Z,DBX6,DBY6,DBZ6,0)
       CALL DIPDISTR(X-PN8,Y,Z,DBX7,DBY7,DBZ7,1)

C			    NOW COMPUTE THE FIELD COMPONENTS:

      BX=PL1*CBX(1)+PL2*CBX(2)+PL3*CBX(3)+PL4*CBX(4)+PL5*CBX(5)
     * +PL6*DBX6+PL7*DBX7+PL8*DBX8
      BY=PL1*CBY(1)+PL2*CBY(2)+PL3*CBY(3)+PL4*CBY(4)+PL5*CBY(5)
     * +PL6*DBY6+PL7*DBY7+PL8*DBY8
      BZ=PL1*CBZ(1)+PL2*CBZ(2)+PL3*CBZ(3)+PL4*CBZ(4)+PL5*CBZ(5)
     * +PL6*DBZ6+PL7*DBZ7+PL8*DBZ8
C
      RETURN
      END
C-----------------------------------------------------------------------

      SUBROUTINE BCONIC(X,Y,Z,CBX,CBY,CBZ,NMAX)
C
c   "CONICAL" HARMONICS
c
       IMPLICIT REAL*8 (A-H,O-Z)
C
       DIMENSION CBX(NMAX),CBY(NMAX),CBZ(NMAX)

       RO2=X**2+Y**2
       RO=DSQRT(RO2)		
C
       CF=X/RO
       SF=Y/RO
       CFM1=1.D0
       SFM1=0.D0
C
      R2=RO2+Z**2
      R=DSQRT(R2)
      C=Z/R
      S=RO/R
      CH=DSQRT(0.5D0*(1.D0+C))
      SH=DSQRT(0.5D0*(1.D0-C))
      TNHM1=1.D0
      CNHM1=1.D0
      TNH=SH/CH
      CNH=1.D0/TNH
C
      DO 1 M=1,NMAX
        CFM=CFM1*CF-SFM1*SF
        SFM=CFM1*SF+SFM1*CF
        CFM1=CFM
        SFM1=SFM
        TNHM=TNHM1*TNH
        CNHM=CNHM1*CNH
       BT=M*CFM/(R*S)*(TNHM+CNHM)
       BF=-0.5D0*M*SFM/R*(TNHM1/CH**2-CNHM1/SH**2)
         TNHM1=TNHM
         CNHM1=CNHM
       CBX(M)=BT*C*CF-BF*SF
       CBY(M)=BT*C*SF+BF*CF
  1    CBZ(M)=-BT*S
C
       RETURN
       END

C-------------------------------------------------------------------
C
       SUBROUTINE DIPDISTR(X,Y,Z,BX,BY,BZ,MODE)
C
C   RETURNS FIELD COMPONENTS FROM A LINEAR DISTRIBUTION OF DIPOLAR SOURCES
C     ON THE Z-AXIS.  THE PARAMETER MODE DEFINES HOW THE DIPOLE STRENGTH
C     VARIES ALONG THE Z-AXIS:  MODE=0 IS FOR A STEP-FUNCTION (Mx=const > 0
c	  FOR Z > 0, AND Mx=-const < 0 FOR Z < 0)
C      WHILE MODE=1 IS FOR A LINEAR VARIATION OF THE DIPOLE MOMENT DENSITY
C	SEE NB#3, PAGE 53 FOR DETAILS.
C
C
C INPUT: X,Y,Z OF A POINT OF SPACE, AND MODE
C
        IMPLICIT REAL*8 (A-H,O-Z)
        X2=X*X
        RHO2=X2+Y*Y
        R2=RHO2+Z*Z
        R3=R2*DSQRT(R2)

        IF (MODE.EQ.0) THEN
         BX=Z/RHO2**2*(R2*(Y*Y-X2)-RHO2*X2)/R3
         BY=-X*Y*Z/RHO2**2*(2.D0*R2+RHO2)/R3
         BZ=X/R3
        ELSE
         BX=Z/RHO2**2*(Y*Y-X2)
         BY=-2.D0*X*Y*Z/RHO2**2
         BZ=X/RHO2
        ENDIF
         RETURN
         END

C+++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
C
      SUBROUTINE R2OUTER (X,Y,Z,BX,BY,BZ)
C
      IMPLICIT REAL*8 (A-H,O-Z)
C
      DATA PL1,PL2,PL3,PL4,PL5/-34.105,-2.00019,628.639,73.4847,12.5162/
      DATA PN1,PN2,PN3,PN4,PN5,PN6,PN7,PN8,PN9,PN10,PN11,PN12,PN13,PN14,
     *  PN15,PN16,PN17 /.55,.694,.0031,1.55,2.8,.1375,-.7,.2,.9625,
     * -2.994,2.925,-1.775,4.3,-.275,2.7,.4312,1.55/
c
C    THREE PAIRS OF CROSSED LOOPS:
C
      CALL CROSSLP(X,Y,Z,DBX1,DBY1,DBZ1,PN1,PN2,PN3)
      CALL CROSSLP(X,Y,Z,DBX2,DBY2,DBZ2,PN4,PN5,PN6)
      CALL CROSSLP(X,Y,Z,DBX3,DBY3,DBZ3,PN7,PN8,PN9)
C
C    NOW AN EQUATORIAL LOOP ON THE NIGHTSIDE
C
      CALL CIRCLE(X-PN10,Y,Z,PN11,DBX4,DBY4,DBZ4)
c
c   NOW A 4-LOOP SYSTEM ON THE NIGHTSIDE
c

      CALL LOOPS4(X,Y,Z,DBX5,DBY5,DBZ5,PN12,PN13,PN14,PN15,PN16,PN17)

C---------------------------------------------------------------------

C			    NOW COMPUTE THE FIELD COMPONENTS:

      BX=PL1*DBX1+PL2*DBX2+PL3*DBX3+PL4*DBX4+PL5*DBX5
      BY=PL1*DBY1+PL2*DBY2+PL3*DBY3+PL4*DBY4+PL5*DBY5
      BZ=PL1*DBZ1+PL2*DBZ2+PL3*DBZ3+PL4*DBZ4+PL5*DBZ5

       RETURN
       END
C
C--------------------------------------------------------------------
C
       SUBROUTINE LOOPS4(X,Y,Z,BX,BY,BZ,XC,YC,ZC,R,THETA,PHI)
C
C   RETURNS FIELD COMPONENTS FROM A SYSTEM OF 4 CURRENT LOOPS, POSITIONED
C     SYMMETRICALLY WITH RESPECT TO NOON-MIDNIGHT MERIDIAN AND EQUATORIAL
C      PLANES.
C  INPUT: X,Y,Z OF A POINT OF SPACE
C	 XC,YC,ZC (YC > 0 AND ZC > 0) - POSITION OF THE CENTER OF THE
C					  1ST-QUADRANT LOOP
C	 R - LOOP RADIUS (THE SAME FOR ALL FOUR)
C	 THETA, PHI  -  SPECIFY THE ORIENTATION OF THE NORMAL OF THE 1ST LOOP
c      -----------------------------------------------------------

        IMPLICIT REAL*8 (A-H,O-Z)
C
          CT=DCOS(THETA)
          ST=DSIN(THETA)
          CP=DCOS(PHI)
          SP=DSIN(PHI)
C------------------------------------1ST QUADRANT:
        XS=(X-XC)*CP+(Y-YC)*SP
        YSS=(Y-YC)*CP-(X-XC)*SP
        ZS=Z-ZC
        XSS=XS*CT-ZS*ST
        ZSS=ZS*CT+XS*ST

        CALL CIRCLE(XSS,YSS,ZSS,R,BXSS,BYS,BZSS)
          BXS=BXSS*CT+BZSS*ST
          BZ1=BZSS*CT-BXSS*ST
          BX1=BXS*CP-BYS*SP
          BY1=BXS*SP+BYS*CP
C-------------------------------------2nd QUADRANT:
        XS=(X-XC)*CP-(Y+YC)*SP
        YSS=(Y+YC)*CP+(X-XC)*SP
        ZS=Z-ZC
        XSS=XS*CT-ZS*ST
        ZSS=ZS*CT+XS*ST

        CALL CIRCLE(XSS,YSS,ZSS,R,BXSS,BYS,BZSS)
          BXS=BXSS*CT+BZSS*ST
          BZ2=BZSS*CT-BXSS*ST
          BX2=BXS*CP+BYS*SP
          BY2=-BXS*SP+BYS*CP
C-------------------------------------3RD QUADRANT:
        XS=-(X-XC)*CP+(Y+YC)*SP
        YSS=-(Y+YC)*CP-(X-XC)*SP
        ZS=Z+ZC
        XSS=XS*CT-ZS*ST
        ZSS=ZS*CT+XS*ST

        CALL CIRCLE(XSS,YSS,ZSS,R,BXSS,BYS,BZSS)
          BXS=BXSS*CT+BZSS*ST
          BZ3=BZSS*CT-BXSS*ST
          BX3=-BXS*CP-BYS*SP
          BY3=BXS*SP-BYS*CP
C-------------------------------------4TH QUADRANT:
        XS=-(X-XC)*CP-(Y-YC)*SP
        YSS=-(Y-YC)*CP+(X-XC)*SP
        ZS=Z+ZC
        XSS=XS*CT-ZS*ST
        ZSS=ZS*CT+XS*ST

        CALL CIRCLE(XSS,YSS,ZSS,R,BXSS,BYS,BZSS)
          BXS=BXSS*CT+BZSS*ST
          BZ4=BZSS*CT-BXSS*ST
          BX4=-BXS*CP+BYS*SP
          BY4=-BXS*SP-BYS*CP

        BX=BX1+BX2+BX3+BX4
        BY=BY1+BY2+BY3+BY4
        BZ=BZ1+BZ2+BZ3+BZ4

         RETURN
         END
C
C@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@
C
      SUBROUTINE R2SHEET(X,Y,Z,BX,BY,BZ)
C
      IMPLICIT REAL*8 (A-H,O-Z)
C
      DATA PNONX1,PNONX2,PNONX3,PNONX4,PNONX5,PNONX6,PNONX7,PNONX8,
     *     PNONY1,PNONY2,PNONY3,PNONY4,PNONY5,PNONY6,PNONY7,PNONY8,
     *     PNONZ1,PNONZ2,PNONZ3,PNONZ4,PNONZ5,PNONZ6,PNONZ7,PNONZ8
     */-19.0969D0,-9.28828D0,-0.129687D0,5.58594D0,22.5055D0,
     *  0.483750D-01,0.396953D-01,0.579023D-01,-13.6750D0,-6.70625D0,
     *  2.31875D0,11.4062D0,20.4562D0,0.478750D-01,0.363750D-01,
     * 0.567500D-01,-16.7125D0,-16.4625D0,-0.1625D0,5.1D0,23.7125D0,
     * 0.355625D-01,0.318750D-01,0.538750D-01/
C
C
      DATA A1,A2,A3,A4,A5,A6,A7,A8,A9,A10,A11,A12,A13,A14,A15,A16,A17,
     *  A18,A19,A20,A21,A22,A23,A24,A25,A26,A27,A28,A29,A30,A31,A32,A33,
     *  A34,A35,A36,A37,A38,A39,A40,A41,A42,A43,A44,A45,A46,A47,A48,A49,
     *  A50,A51,A52,A53,A54,A55,A56,A57,A58,A59,A60,A61,A62,A63,A64,A65,
     *  A66,A67,A68,A69,A70,A71,A72,A73,A74,A75,A76,A77,A78,A79,A80
     * /8.07190D0,-7.39582D0,-7.62341D0,0.684671D0,-13.5672D0,11.6681D0,
     * 13.1154,-0.890217D0,7.78726D0,-5.38346D0,-8.08738D0,0.609385D0,
     * -2.70410D0, 3.53741D0,3.15549D0,-1.11069D0,-8.47555D0,0.278122D0,
     *  2.73514D0,4.55625D0,13.1134D0,1.15848D0,-3.52648D0,-8.24698D0,
     * -6.85710D0,-2.81369D0, 2.03795D0, 4.64383D0,2.49309D0,-1.22041D0,
     * -1.67432D0,-0.422526D0,-5.39796D0,7.10326D0,5.53730D0,-13.1918D0,
     *  4.67853D0,-7.60329D0,-2.53066D0, 7.76338D0, 5.60165D0,5.34816D0,
     * -4.56441D0,7.05976D0,-2.62723D0,-0.529078D0,1.42019D0,-2.93919D0,
     *  55.6338D0,-1.55181D0,39.8311D0,-80.6561D0,-46.9655D0,32.8925D0,
     * -6.32296D0,19.7841D0,124.731D0,10.4347D0,-30.7581D0,102.680D0,
     * -47.4037D0,-3.31278D0,9.37141D0,-50.0268D0,-533.319D0,110.426D0,
     *  1000.20D0,-1051.40D0, 1619.48D0,589.855D0,-1462.73D0,1087.10D0,
     *  -1994.73D0,-1654.12D0,1263.33D0,-260.210D0,1424.84D0,1255.71D0,
     *  -956.733D0, 219.946D0/
C
C
      DATA B1,B2,B3,B4,B5,B6,B7,B8,B9,B10,B11,B12,B13,B14,B15,B16,B17,
     *  B18,B19,B20,B21,B22,B23,B24,B25,B26,B27,B28,B29,B30,B31,B32,B33,
     *  B34,B35,B36,B37,B38,B39,B40,B41,B42,B43,B44,B45,B46,B47,B48,B49,
     *  B50,B51,B52,B53,B54,B55,B56,B57,B58,B59,B60,B61,B62,B63,B64,B65,
     *  B66,B67,B68,B69,B70,B71,B72,B73,B74,B75,B76,B77,B78,B79,B80
     */-9.08427D0,10.6777D0,10.3288D0,-0.969987D0,6.45257D0,-8.42508D0,
     * -7.97464D0,1.41996D0,-1.92490D0,3.93575D0,2.83283D0,-1.48621D0,
     *0.244033D0,-0.757941D0,-0.386557D0,0.344566D0,9.56674D0,-2.5365D0,
     * -3.32916D0,-5.86712D0,-6.19625D0,1.83879D0,2.52772D0,4.34417D0,
     * 1.87268D0,-2.13213D0,-1.69134D0,-.176379D0,-.261359D0,.566419D0,
     * 0.3138D0,-0.134699D0,-3.83086D0,-8.4154D0,4.77005D0,-9.31479D0,
     * 37.5715D0,19.3992D0,-17.9582D0,36.4604D0,-14.9993D0,-3.1442D0,
     * 6.17409D0,-15.5519D0,2.28621D0,-0.891549D-2,-.462912D0,2.47314D0,
     * 41.7555D0,208.614D0,-45.7861D0,-77.8687D0,239.357D0,-67.9226D0,
     * 66.8743D0,238.534D0,-112.136D0,16.2069D0,-40.4706D0,-134.328D0,
     * 21.56D0,-0.201725D0,2.21D0,32.5855D0,-108.217D0,-1005.98D0,
     * 585.753D0,323.668D0,-817.056D0,235.750D0,-560.965D0,-576.892D0,
     * 684.193D0,85.0275D0,168.394D0,477.776D0,-289.253D0,-123.216D0,
     * 75.6501D0,-178.605D0/
C
      DATA C1,C2,C3,C4,C5,C6,C7,C8,C9,C10,C11,C12,C13,C14,C15,C16,C17,
     *  C18,C19,C20,C21,C22,C23,C24,C25,C26,C27,C28,C29,C30,C31,C32,C33,
     *  C34,C35,C36,C37,C38,C39,C40,C41,C42,C43,C44,C45,C46,C47,C48,C49,
     *  C50,C51,C52,C53,C54,C55,C56,C57,C58,C59,C60,C61,C62,C63,C64,C65,
     *  C66,C67,C68,C69,C70,C71,C72,C73,C74,C75,C76,C77,C78,C79,C80
     * / 1167.61D0,-917.782D0,-1253.2D0,-274.128D0,-1538.75D0,1257.62D0,
     * 1745.07D0,113.479D0,393.326D0,-426.858D0,-641.1D0,190.833D0,
     * -29.9435D0,-1.04881D0,117.125D0,-25.7663D0,-1168.16D0,910.247D0,
     * 1239.31D0,289.515D0,1540.56D0,-1248.29D0,-1727.61D0,-131.785D0,
     * -394.577D0,426.163D0,637.422D0,-187.965D0,30.0348D0,0.221898D0,
     * -116.68D0,26.0291D0,12.6804D0,4.84091D0,1.18166D0,-2.75946D0,
     * -17.9822D0,-6.80357D0,-1.47134D0,3.02266D0,4.79648D0,0.665255D0,
     * -0.256229D0,-0.857282D-1,-0.588997D0,0.634812D-1,0.164303D0,
     * -0.15285D0,22.2524D0,-22.4376D0,-3.85595D0,6.07625D0,-105.959D0,
     * -41.6698D0,0.378615D0,1.55958D0,44.3981D0,18.8521D0,3.19466D0,
     *  5.89142D0,-8.63227D0,-2.36418D0,-1.027D0,-2.31515D0,1035.38D0,
     *  2040.66D0,-131.881D0,-744.533D0,-3274.93D0,-4845.61D0,482.438D0,
     * 1567.43D0,1354.02D0,2040.47D0,-151.653D0,-845.012D0,-111.723D0,
     * -265.343D0,-26.1171D0,216.632D0/
C
c------------------------------------------------------------------
C
       XKS=XKSI(X,Y,Z)    !  variation across the current sheet
       T1X=XKS/DSQRT(XKS**2+PNONX6**2)
       T2X=PNONX7**3/DSQRT(XKS**2+PNONX7**2)**3
       T3X=XKS/DSQRT(XKS**2+PNONX8**2)**5 *3.493856D0*PNONX8**4
C
       T1Y=XKS/DSQRT(XKS**2+PNONY6**2)
       T2Y=PNONY7**3/DSQRT(XKS**2+PNONY7**2)**3
       T3Y=XKS/DSQRT(XKS**2+PNONY8**2)**5 *3.493856D0*PNONY8**4
C
       T1Z=XKS/DSQRT(XKS**2+PNONZ6**2)
       T2Z=PNONZ7**3/DSQRT(XKS**2+PNONZ7**2)**3
       T3Z=XKS/DSQRT(XKS**2+PNONZ8**2)**5 *3.493856D0*PNONZ8**4
C
      RHO2=X*X+Y*Y
      R=DSQRT(RHO2+Z*Z)
      RHO=DSQRT(RHO2)
C
      C1P=X/RHO
      S1P=Y/RHO
      S2P=2.D0*S1P*C1P
      C2P=C1P*C1P-S1P*S1P
      S3P=S2P*C1P+C2P*S1P
      C3P=C2P*C1P-S2P*S1P
      S4P=S3P*C1P+C3P*S1P
      CT=Z/R
      ST=RHO/R
C
      S1=FEXP(CT,PNONX1)
      S2=FEXP(CT,PNONX2)
      S3=FEXP(CT,PNONX3)
      S4=FEXP(CT,PNONX4)
      S5=FEXP(CT,PNONX5)
C
C		    NOW COMPUTE THE GSM FIELD COMPONENTS:
C
C
      BX=S1*((A1+A2*T1X+A3*T2X+A4*T3X)
     *        +C1P*(A5+A6*T1X+A7*T2X+A8*T3X)
     *        +C2P*(A9+A10*T1X+A11*T2X+A12*T3X)
     *        +C3P*(A13+A14*T1X+A15*T2X+A16*T3X))
     *    +S2*((A17+A18*T1X+A19*T2X+A20*T3X)
     *        +C1P*(A21+A22*T1X+A23*T2X+A24*T3X)
     *        +C2P*(A25+A26*T1X+A27*T2X+A28*T3X)
     *        +C3P*(A29+A30*T1X+A31*T2X+A32*T3X))
     *    +S3*((A33+A34*T1X+A35*T2X+A36*T3X)
     *        +C1P*(A37+A38*T1X+A39*T2X+A40*T3X)
     *        +C2P*(A41+A42*T1X+A43*T2X+A44*T3X)
     *        +C3P*(A45+A46*T1X+A47*T2X+A48*T3X))
     *    +S4*((A49+A50*T1X+A51*T2X+A52*T3X)
     *        +C1P*(A53+A54*T1X+A55*T2X+A56*T3X)
     *        +C2P*(A57+A58*T1X+A59*T2X+A60*T3X)
     *        +C3P*(A61+A62*T1X+A63*T2X+A64*T3X))
     *    +S5*((A65+A66*T1X+A67*T2X+A68*T3X)
     *        +C1P*(A69+A70*T1X+A71*T2X+A72*T3X)
     *        +C2P*(A73+A74*T1X+A75*T2X+A76*T3X)
     *        +C3P*(A77+A78*T1X+A79*T2X+A80*T3X))
C
C
      S1=FEXP(CT,PNONY1)
      S2=FEXP(CT,PNONY2)
      S3=FEXP(CT,PNONY3)
      S4=FEXP(CT,PNONY4)
      S5=FEXP(CT,PNONY5)
C
      BY=S1*(S1P*(B1+B2*T1Y+B3*T2Y+B4*T3Y)
     *      +S2P*(B5+B6*T1Y+B7*T2Y+B8*T3Y)
     *      +S3P*(B9+B10*T1Y+B11*T2Y+B12*T3Y)
     *      +S4P*(B13+B14*T1Y+B15*T2Y+B16*T3Y))
     *  +S2*(S1P*(B17+B18*T1Y+B19*T2Y+B20*T3Y)
     *      +S2P*(B21+B22*T1Y+B23*T2Y+B24*T3Y)
     *      +S3P*(B25+B26*T1Y+B27*T2Y+B28*T3Y)
     *      +S4P*(B29+B30*T1Y+B31*T2Y+B32*T3Y))
     *  +S3*(S1P*(B33+B34*T1Y+B35*T2Y+B36*T3Y)
     *      +S2P*(B37+B38*T1Y+B39*T2Y+B40*T3Y)
     *      +S3P*(B41+B42*T1Y+B43*T2Y+B44*T3Y)
     *      +S4P*(B45+B46*T1Y+B47*T2Y+B48*T3Y))
     *  +S4*(S1P*(B49+B50*T1Y+B51*T2Y+B52*T3Y)
     *      +S2P*(B53+B54*T1Y+B55*T2Y+B56*T3Y)
     *      +S3P*(B57+B58*T1Y+B59*T2Y+B60*T3Y)
     *      +S4P*(B61+B62*T1Y+B63*T2Y+B64*T3Y))
     *  +S5*(S1P*(B65+B66*T1Y+B67*T2Y+B68*T3Y)
     *      +S2P*(B69+B70*T1Y+B71*T2Y+B72*T3Y)
     *      +S3P*(B73+B74*T1Y+B75*T2Y+B76*T3Y)
     *      +S4P*(B77+B78*T1Y+B79*T2Y+B80*T3Y))
C
      S1=FEXP1(CT,PNONZ1)
      S2=FEXP1(CT,PNONZ2)
      S3=FEXP1(CT,PNONZ3)
      S4=FEXP1(CT,PNONZ4)
      S5=FEXP1(CT,PNONZ5)
C
      BZ=S1*((C1+C2*T1Z+C3*T2Z+C4*T3Z)
     *      +C1P*(C5+C6*T1Z+C7*T2Z+C8*T3Z)
     *      +C2P*(C9+C10*T1Z+C11*T2Z+C12*T3Z)
     *      +C3P*(C13+C14*T1Z+C15*T2Z+C16*T3Z))
     *   +S2*((C17+C18*T1Z+C19*T2Z+C20*T3Z)
     *      +C1P*(C21+C22*T1Z+C23*T2Z+C24*T3Z)
     *      +C2P*(C25+C26*T1Z+C27*T2Z+C28*T3Z)
     *      +C3P*(C29+C30*T1Z+C31*T2Z+C32*T3Z))
     *   +S3*((C33+C34*T1Z+C35*T2Z+C36*T3Z)
     *      +C1P*(C37+C38*T1Z+C39*T2Z+C40*T3Z)
     *      +C2P*(C41+C42*T1Z+C43*T2Z+C44*T3Z)
     *      +C3P*(C45+C46*T1Z+C47*T2Z+C48*T3Z))
     *   +S4*((C49+C50*T1Z+C51*T2Z+C52*T3Z)
     *      +C1P*(C53+C54*T1Z+C55*T2Z+C56*T3Z)
     *      +C2P*(C57+C58*T1Z+C59*T2Z+C60*T3Z)
     *      +C3P*(C61+C62*T1Z+C63*T2Z+C64*T3Z))
     *   +S5*((C65+C66*T1Z+C67*T2Z+C68*T3Z)
     *      +C1P*(C69+C70*T1Z+C71*T2Z+C72*T3Z)
     *      +C2P*(C73+C74*T1Z+C75*T2Z+C76*T3Z)
     *      +C3P*(C77+C78*T1Z+C79*T2Z+C80*T3Z))
C
       RETURN
       END
C
C^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^

      DOUBLE PRECISION FUNCTION XKSI(X,Y,Z)
C
      IMPLICIT REAL*8 (A-H,O-Z)
C
C   A11 - C72, R0, and DR below  ARE STRETCH PARAMETERS (P.26-27, NB# 3),
C
      DATA A11A12,A21A22,A41A42,A51A52,A61A62,B11B12,B21B22,C61C62,
     *  C71C72,R0,DR /0.305662,-0.383593,0.2677733,-0.097656,-0.636034,
     *  -0.359862,0.424706,-0.126366,0.292578,1.21563,7.50937/

      DATA TNOON,DTETA/0.3665191,0.09599309/ ! Correspond to noon and midnight
C					  latitudes 69 and 63.5 degs, resp.
       DR2=DR*DR
C
       X2=X*X
       Y2=Y*Y
       Z2=Z*Z
       XY=X*Y
       XYZ=XY*Z
       R2=X2+Y2+Z2
       R=DSQRT(R2)
       R3=R2*R
       R4=R2*R2
       XR=X/R
       YR=Y/R
       ZR=Z/R
C
       IF (R.LT.R0) THEN
         PR=0.D0
       ELSE
         PR=DSQRT((R-R0)**2+DR2)-DR
       ENDIF
C
      F=X+PR*(A11A12+A21A22*XR+A41A42*XR*XR+A51A52*YR*YR+
     *        A61A62*ZR*ZR)
      G=Y+PR*(B11B12*YR+B21B22*XR*YR)
      H=Z+PR*(C61C62*ZR+C71C72*XR*ZR)
      G2=G*G
C
      FGH=F**2+G2+H**2
      FGH32=DSQRT(FGH)**3
      FCHSG2=F**2+G2

      IF (FCHSG2.LT.1.D-5) THEN
         XKSI=-1.D0		  !  THIS IS JUST FOR ELIMINATING PROBLEMS
         RETURN 		   !  ON THE Z-AXIS
      ENDIF

      SQFCHSG2=DSQRT(FCHSG2)
      ALPHA=FCHSG2/FGH32
      THETA=TNOON+0.5D0*DTETA*(1.D0-F/SQFCHSG2)
      PHI=DSIN(THETA)**2
C
      XKSI=ALPHA-PHI
C
      RETURN
      END
C
C--------------------------------------------------------------------
C
        FUNCTION FEXP(S,A)
         IMPLICIT REAL*8 (A-H,O-Z)
          DATA E/2.718281828459D0/
          IF (A.LT.0.D0) FEXP=DSQRT(-2.D0*A*E)*S*DEXP(A*S*S)
          IF (A.GE.0.D0) FEXP=S*DEXP(A*(S*S-1.D0))
         RETURN
         END
C
C-----------------------------------------------------------------------
        FUNCTION FEXP1(S,A)
         IMPLICIT REAL*8 (A-H,O-Z)
         IF (A.LE.0.D0) FEXP1=DEXP(A*S*S)
         IF (A.GT.0.D0) FEXP1=DEXP(A*(S*S-1.D0))
         RETURN
         END
C
C************************************************************************
C
         DOUBLE PRECISION FUNCTION TKSI(XKSI,XKS0,DXKSI)
         IMPLICIT REAL*8 (A-H,O-Z)
         SAVE M,TDZ3
         DATA M/0/
C
         IF (M.EQ.0) THEN
         TDZ3=2.*DXKSI**3
         M=1
         ENDIF
C
         IF (XKSI-XKS0.LT.-DXKSI) TKSII=0.
         IF (XKSI-XKS0.GE.DXKSI)  TKSII=1.
C
         IF (XKSI.GE.XKS0-DXKSI.AND.XKSI.LT.XKS0) THEN
           BR3=(XKSI-XKS0+DXKSI)**3
           TKSII=1.5*BR3/(TDZ3+BR3)
         ENDIF
C
         IF (XKSI.GE.XKS0.AND.XKSI.LT.XKS0+DXKSI) THEN
           BR3=(XKSI-XKS0-DXKSI)**3
           TKSII=1.+1.5*BR3/(TDZ3-BR3)
         ENDIF
           TKSI=TKSII
         END
C
C~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
c
       SUBROUTINE DIPOLE(PS,X,Y,Z,BX,BY,BZ)
C
C  CALCULATES GSM COMPONENTS OF GEODIPOLE FIELD WITH THE DIPOLE MOMENT
C  CORRESPONDING TO THE EPOCH OF 1980.
C------------INPUT PARAMETERS:
C   PS - GEODIPOLE TILT ANGLE IN RADIANS, X,Y,Z - GSM COORDINATES IN RE
C------------OUTPUT PARAMETERS:
C   BX,BY,BZ - FIELD COMPONENTS IN GSM SYSTEM, IN NANOTESLA.
C
C
C		    AUTHOR: NIKOLAI A. TSYGANENKO
C			    INSTITUTE OF PHYSICS
C			    ST.-PETERSBURG STATE UNIVERSITY
C			    STARY PETERGOF 198904
C			    ST.-PETERSBURG
C			    RUSSIA
C
      IMPLICIT REAL*8 (A-H,O-Z)
C
      REAL*8 PS,X,Y,Z,BX,BY,BZ,PSI,SPS,CPS,P,U,V,T,Q
      INTEGER M

      DATA M,PSI/0,5./
      IF(M.EQ.1.AND.ABS(PS-PSI).LT.1.E-5) GOTO 1
      SPS=DSIN(PS)
      CPS=DCOS(PS)
      PSI=PS
      M=1
  1   P=X**2
      U=Z**2
      V=3.*Z*X
      T=Y**2
      Q=30574./DSQRT(P+T+U)**5
      BX=Q*((T+U-2.*P)*SPS-V*CPS)
      BY=-3.*Y*Q*(X*SPS+Z*CPS)
      BZ=Q*((P+T-2.*U)*CPS-V*SPS)
      RETURN
      END
C~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~


c**************************************************************************************
c                 TSYGANENKO 05



      SUBROUTINE T04_s (IOPT,PARMOD,PS,X,Y,Z,BX,BY,BZ)
c
c     ASSEMBLED:  MARCH 25, 2004; UPDATED:  AUGUST 2 & 31, DECEMBER 27, 2004.
C
c--------------------------------------------------------------------
C   A DATA-BASED MODEL OF THE EXTERNAL (I.E., WITHOUT EARTH'S CONTRIBUTION) PART OF THE
C   MAGNETOSPHERIC MAGNETIC FIELD, CALIBRATED BY
C    (1) SOLAR WIND PRESSURE PDYN (NANOPASCALS),
C    (2) DST (NANOTESLA),
C    (3) BYIMF,
C    (4) BZIMF (NANOTESLA)
C    (5-10)   INDICES W1 - W6, CALCULATED AS TIME INTEGRALS FROM THE BEGINNING OF A STORM
c               SEE THE REFERENCE (3) BELOW, FOR A DETAILED DEFINITION OF THOSE VARIABLES
C
c   THE ABOVE 10 INPUT PARAMETERS SHOULD BE PLACED IN THE ELEMENTS
c   OF THE ARRAY PARMOD(10).
C
C   THE REST OF THE INPUT VARIABLES ARE: THE GEODIPOLE TILT ANGLE PS (RADIANS),
C        X,Y,Z -  GSM POSITION (RE)
C
c   IOPT IS A DUMMY INPUT PARAMETER, INCLUDED TO MAKE THIS SUBROUTINE
C   COMPATIBLE WITH THE TRACING SOFTWARE PACKAGE (GEOPACK). IN THIS MODEL,
C   THE PARAMETER IOPT DOES NOT AFFECT THE OUTPUT FIELD.
c
C*******************************************************************************************
c** ATTENTION:  THE MODEL IS BASED ON DATA TAKEN SUNWARD FROM X=-15Re, AND HENCE BECOMES   *
C**              INVALID AT LARGER TAILWARD DISTANCES !!!                                  *
C*******************************************************************************************
C
c   OUTPUT:  GSM COMPONENTS OF THE EXTERNAL MAGNETIC FIELD (BX,BY,BZ, nanotesla)
C            COMPUTED AS A SUM OF CONTRIBUTIONS FROM PRINCIPAL FIELD SOURCES
C
c  (C) Copr. 2004, Nikolai A. Tsyganenko, USRA/Code 695.1, NASA GSFC
c      Greenbelt, MD 20771, USA
c
C                            REFERENCES:
C
C  (1)   N. A. Tsyganenko, A new data-based model of the near magnetosphere magnetic field:
c       1. Mathematical structure.
c       2. Parameterization and fitting to observations.  JGR v. 107(A8), 1176/1179, doi:10.1029/2001JA000219/220, 2002.
c
c  (2)  N. A. Tsyganenko, H. J. Singer, J. C. Kasper, Storm-time distortion of the
c           inner magnetosphere: How severe can it get ?  JGR v. 108(A5), 1209, doi:10.1029/2002JA009808, 2003.

c           magnetosphere during strong geomagnetic storms,  JGR v. 110, 2005, in press.
c----------------------------------------------------------------------
c
      REAL*8 PARMOD(10),PS,X,Y,Z,BX,BY,BZ
      REAL*8 A(69),PDYN,DST_AST,BXIMF,BYIMF,BZIMF,W1,W2,W3,W4,W5,W6,
     *  PSS,XX,YY,ZZ,BXCF,BYCF,BZCF,BXT1,BYT1,BZT1,BXT2,BYT2,BZT2,
     *  BXSRC,BYSRC,BZSRC,BXPRC,BYPRC,BZPRC, BXR11,BYR11,BZR11,
     *  BXR12,BYR12,BZR12,BXR21,BYR21,BZR21,BXR22,BYR22,BZR22,HXIMF,
     *  HYIMF,HZIMF,BBX,BBY,BBZ
C
      DATA A/1.00000,5.19884,0.923524,8.68111,0.00000,-6.44922,11.3109,
     * -3.84555,0.00000,0.558081,0.937044,0.00000,0.772433,0.687241,
     * 0.00000,0.320369,1.22531,-0.432246E-01,-0.382436,0.457468,
     * 0.741917,0.227194,0.154269,5.75196,22.3113,10.3526,64.3312,
     * 1.01977,-0.200859E-01,0.971643,0.295525E-01,1.01032,0.215561,
     * 1.50059,0.730898E-01,1.93625,1.74545,1.29533,0.714744,0.391687,
     * 3.31283,75.0127,6.36283,4.43561,0.387801,0.699661,0.305352E-01,
     * 0.581002,1.14671,0.876060,0.386060,0.801831,0.874315,0.463634,
     * 0.175077,0.673053,0.388341,2.32074,1.32373,0.419800,1.24968,
     * 1.28903,.409286,1.57622,.690036,1.28836,2.4054,.528557,.564247/

      DATA IOPGEN,IOPTT,IOPB,IOPR/0,0,0,0/
C
      PDYN=PARMOD(1)
      DST_AST=PARMOD(2)*0.8-13.*DSQRT(PDYN)
      BYIMF=PARMOD(3)
      BZIMF=PARMOD(4)
C     
      W1=0            !da controllare per ora azzerati
      W2=0
      W3=0
      W4=0
      W5=0
      W6=0
             
      
      W1=PARMOD (5)
      W2=PARMOD (6)
      W3=PARMOD (7)
      W4=PARMOD (8)
      W5=PARMOD (9)
      W6=PARMOD(10)

      PSS=PS
      XX=X
      YY=Y
      ZZ=Z
C
      CALL EXTERN04 (IOPGEN,IOPTT,IOPB,IOPR,A,79,PDYN,DST_AST,BXIMF,
     * BYIMF,
     + BZIMF,W1,W2,W3,W4,W5,W6,PSS,XX,YY,ZZ,BXCF,BYCF,BZCF,BXT1,BYT1,
     + BZT1,BXT2,BYT2,BZT2,BXSRC,BYSRC,BZSRC,BXPRC,BYPRC,BZPRC, BXR11,
     + BYR11,BZR11,BXR12,BYR12,BZR12,BXR21,BYR21,BZR21,BXR22,BYR22,
     + BZR22,HXIMF,HYIMF,HZIMF,BBX,BBY,BBZ)
C
      BX=BBX
      BY=BBY
      BZ=BBZ
C
      RETURN
      END

c%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
c
      SUBROUTINE EXTERN04 (IOPGEN,IOPT,IOPB,IOPR,A,NTOT,
     *  PDYN,DST,BXIMF,BYIMF,BZIMF,W1,W2,W3,W4,W5,W6,PS,X,Y,Z,
     *  BXCF,BYCF,BZCF,BXT1,BYT1,BZT1,BXT2,BYT2,BZT2,
     *  BXSRC,BYSRC,BZSRC,BXPRC,BYPRC,BZPRC, BXR11,BYR11,BZR11,
     *  BXR12,BYR12,BZR12,BXR21,BYR21,BZR21,BXR22,BYR22,BZR22,HXIMF,
     *  HYIMF,HZIMF,BX,BY,BZ)
C
C   IOPGEN - GENERAL OPTION FLAG:  IOPGEN=0 - CALCULATE TOTAL FIELD
C                                  IOPGEN=1 - DIPOLE SHIELDING ONLY
C                                  IOPGEN=2 - TAIL FIELD ONLY
C                                  IOPGEN=3 - BIRKELAND FIELD ONLY
C                                  IOPGEN=4 - RING CURRENT FIELD ONLY
C                                  IOPGEN=5 - INTERCONNECTION FIELD ONLY
C
C   IOPT -  TAIL FIELD FLAG:       IOPT=0  -  BOTH MODES
C                                  IOPT=1  -  MODE 1 ONLY
C                                  IOPT=2  -  MODE 2 ONLY
C
C   IOPB -  BIRKELAND FIELD FLAG:  IOPB=0  -  ALL 4 TERMS
C                                  IOPB=1  -  REGION 1, MODES 1 AND 2
C                                  IOPB=2  -  REGION 2, MODES 1 AND 2
C
C   IOPR -  RING CURRENT FLAG:     IOPR=0  -  BOTH SRC AND PRC
C                                  IOPR=1  -  SRC ONLY
C                                  IOPR=2  -  PRC ONLY
C
      IMPLICIT  REAL * 8  (A - H, O - Z)
C
      DIMENSION A(NTOT)
C
      COMMON /TAIL/ DXSHIFT1,DXSHIFT2,D,DELTADY  ! THE COMMON BLOCKS FORWARD NONLINEAR PARAMETERS
      COMMON /BIRKPAR/ XKAPPA1,XKAPPA2
      COMMON /RCPAR/ SC_SY,SC_AS,PHI
      COMMON /G/ G
      COMMON /RH004/ RH004
C
C
      DATA A0_A,A0_S0,A0_X0 /34.586D0,1.1960D0,3.4397D0/   !   SHUE ET AL. PARAMETERS
      DATA DSIG /0.005D0/, RH004,RH2 /7.5D0,-5.2D0/
c
      XAPPA=(PDYN/2.)**A(23)   !  OVERALL SCALING PARAMETER
      RH004=7.5                  !  TAIL HINGING DISTANCE
c
      G=  35.0                 !  TAIL WARPING PARAMETER

      XAPPA3=XAPPA**3

      XX=X*XAPPA
      YY=Y*XAPPA
      ZZ=Z*XAPPA
C
      SPS=DSIN(PS)
c
      X0=A0_X0/XAPPA
      AM=A0_A/XAPPA
      S0=A0_S0
c
C  CALCULATE "IMF" COMPONENTS OUTSIDE THE MAGNETOPAUSE LAYER (HENCE BEGIN WITH "O")
C  THEY ARE NEEDED ONLY IF THE POINT (X,Y,Z) IS WITHIN THE TRANSITION MAGNETOPAUSE LAYER
C  OR OUTSIDE THE MAGNETOSPHERE:
C
      FACTIMF=A(20)
c
      OIMFX=0.D0
      OIMFY=BYIMF*FACTIMF
      OIMFZ=BZIMF*FACTIMF
c
      R=DSQRT(X**2+Y**2+Z**2)
      XSS=X
      ZSS=Z

  1   XSOLD=XSS      !   BEGIN ITERATIVE SEARCH OF UNWARPED COORDS (TO FIND SIGMA)
      ZSOLD=ZSS

      RH=RH004+RH2*(ZSS/R)**2
      SINPSAS=SPS/(1.D0+(R/RH)**3)**0.33333333D0
      COSPSAS=DSQRT(1.D0-SINPSAS**2)
      ZSS=X*SINPSAS+Z*COSPSAS
      XSS=X*COSPSAS-Z*SINPSAS
      DD=DABS(XSS-XSOLD)+DABS(ZSS-ZSOLD)
      IF (DD.GT.1.D-6) GOTO 1
C                                END OF ITERATIVE SEARCH
      RHO2=Y**2+ZSS**2
      ASQ=AM**2
      XMXM=AM+XSS-X0
      IF (XMXM.LT.0.) XMXM=0. ! THE BOUNDARY IS A CYLINDER TAILWARD OF X=X0-AM
      AXX0=XMXM**2
      ARO=ASQ+RHO2
      SIGMA=DSQRT((ARO+AXX0+DSQRT((ARO+AXX0)**2-4.*ASQ*AXX0))/(2.*ASQ))
C
C   NOW, THERE ARE THREE POSSIBLE CASES:
C    (1) INSIDE THE MAGNETOSPHERE   (SIGMA
C    (2) IN THE BOUNDARY LAYER
C    (3) OUTSIDE THE MAGNETOSPHERE AND B.LAYER
C       FIRST OF ALL, CONSIDER THE CASES (1) AND (2):
C
C+++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
      IF (SIGMA.LT.S0+DSIG04) THEN  !  CASES (1) OR (2); CALCULATE THE MODEL FIELD
C                                   (WITH THE POTENTIAL "PENETRATED" INTERCONNECTION FIELD):
C+++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
C
      IF (IOPGEN.LE.1) THEN
         CALL SHLCAR3X304(XX,YY,ZZ,PS,CFX,CFY,CFZ)         !  DIPOLE SHIELDING FIELD
         BXCF=CFX*XAPPA3
         BYCF=CFY*XAPPA3
         BZCF=CFZ*XAPPA3
      ELSE
         BXCF=0.D0
         BYCF=0.D0
         BZCF=0.D0
      ENDIF                                              !  DONE

      IF (IOPGEN.EQ.0.OR.IOPGEN.EQ.2) THEN
          DSTT=-20.
          IF (DST.LT.DSTT) DSTT=DST
          ZNAM=DABS(DSTT)**0.37
         DXSHIFT1=A(24)-A(25)/ZNAM
         DXSHIFT2=A(26)-A(27)/ZNAM
         D=A(36)*DEXP(-W1/A(37))  +A(69)
         DELTADY=4.7

         CALL DEFORMED04 (IOPT,PS,XX,YY,ZZ,                !  TAIL FIELD (THREE MODES)
     *    BXT1,BYT1,BZT1,BXT2,BYT2,BZT2)
      ELSE
         BXT1=0.D0
         BYT1=0.D0
         BZT1=0.D0
         BXT2=0.D0
         BYT2=0.D0
         BZT2=0.D0
      ENDIF

      IF (IOPGEN.EQ.0.OR.IOPGEN.EQ.3) THEN

          ZNAM=DABS(DST)
          IF (DST.GE.-20.D0) ZNAM=20.D0
          XKAPPA1=A(32)*(ZNAM/20.D0)**A(33)
          XKAPPA2=A(34)*(ZNAM/20.D0)**A(35)

         CALL BIRK_TOT04 (IOPB,PS,XX,YY,ZZ,BXR11,BYR11,BZR11,BXR12,BYR12,
     *   BZR12,BXR21,BYR21,BZR21,BXR22,BYR22,BZR22)    !   BIRKELAND FIELD (TWO MODES FOR R1 AND TWO MODES FOR R2)
      ELSE
         BXR11=0.D0
         BYR11=0.D0
         BZR11=0.D0
         BXR21=0.D0
         BYR21=0.D0
         BZR21=0.D0
      ENDIF

      IF (IOPGEN.EQ.0.OR.IOPGEN.EQ.4) THEN
         PHI=A(38)

          ZNAM=DABS(DST)
          IF (DST.GE.-20.D0) ZNAM=20.D0
          SC_SY=A(28)*(20.D0/ZNAM)**A(29) *XAPPA    !
          SC_AS=A(30)*(20.D0/ZNAM)**A(31) *XAPPA    !  MULTIPLICATION  BY XAPPA IS MADE IN ORDER TO MAKE THE SRC AND PRC
                                                    !     SCALING COMPLETELY INDEPENDENT OF THE GENERAL SCALING DUE TO THE
C                                                         MAGNETOPAUSE COMPRESSION/EXPANSION                             !
C
         CALL FULL_RC04(IOPR,PS,XX,YY,ZZ,BXSRC,BYSRC,BZSRC,BXPRC,BYPRC,
     *                                        BZPRC)  !  SHIELDED RING CURRENT (SRC AND PRC)
      ELSE
         BXSRC=0.D0
         BYSRC=0.D0
         BZSRC=0.D0
         BXPRC=0.D0
         BYPRC=0.D0
         BZPRC=0.D0
      ENDIF
C
      IF (IOPGEN.EQ.0.OR.IOPGEN.EQ.5) THEN
         HXIMF=0.D0
         HYIMF=BYIMF
         HZIMF=BZIMF   ! THESE ARE COMPONENTS OF THE PENETRATED FIELD PER UNIT OF THE PENETRATION COEFFICIENT.
C                             IN OTHER WORDS, THESE ARE DERIVATIVES OF THE PENETRATION FIELD COMPONENTS WITH RESPECT
C                             TO THE PENETRATION COEFFICIENT.   WE ASSUME THAT ONLY TRANSVERSE COMPONENT OF THE
C                             FIELD PENETRATES INSIDE.
       ELSE
         HXIMF=0.D0
         HYIMF=0.D0
         HZIMF=0.D0
       ENDIF
C
C-----------------------------------------------------------
C
C    NOW, ADD UP ALL THE COMPONENTS:
c
      DLP1=(PDYN/2.D0)**A(21)
      DLP2=(PDYN/2.D0)**A(22)

      TAMP1=A(2)+A(3)*DLP1+A(4)*A(39)*W1/DSQRT(W1**2+A(39)**2)+A(5)*DST
      TAMP2=A(6)+A(7)*DLP2+A(8)*A(40)*W2/DSQRT(W2**2+A(40)**2)+A(9)*DST
      A_SRC=A(10)+A(11)*A(41)*W3/DSQRT(W3**2+A(41)**2)
     *  +A(12)*DST
      A_PRC=A(13)+A(14)*A(42)*W4/DSQRT(W4**2+A(42)**2)
     *  +A(15)*DST
      A_R11=A(16)+A(17)*A(43)*W5/DSQRT(W5**2+A(43)**2)
      A_R21=A(18)+A(19)*A(44)*W6/DSQRT(W6**2+A(44)**2)

      BBX=A(1)*BXCF+TAMP1*BXT1+TAMP2*BXT2+A_SRC*BXSRC+A_PRC*BXPRC
     * +A_R11*BXR11+A_R21*BXR21+A(20)*HXIMF

      BBY=A(1)*BYCF+TAMP1*BYT1+TAMP2*BYT2+A_SRC*BYSRC+A_PRC*BYPRC
     * +A_R11*BYR11+A_R21*BYR21+A(20)*HYIMF

      BBZ=A(1)*BZCF+TAMP1*BZT1+TAMP2*BZT2+A_SRC*BZSRC+A_PRC*BZPRC
     * +A_R11*BZR11+A_R21*BZR21+A(20)*HZIMF
C
C   AND WE HAVE THE TOTAL EXTERNAL FIELD.
C
C
C  NOW, LET US CHECK WHETHER WE HAVE THE CASE (1). IF YES - ALL DONE:
C
      IF (SIGMA.LT.S0-DSIG04) THEN    !  (X,Y,Z) IS INSIDE THE MAGNETOSPHERE

       BX=BBX
       BY=BBY
       BZ=BBZ
                     ELSE           !  THIS IS THE MOST COMPLEX CASE: WE ARE INSIDE
C                                             THE INTERPOLATION REGION
       FINT=0.5*(1.-(SIGMA-S0)/DSIG04)
       FEXT=0.5*(1.+(SIGMA-S0)/DSIG04)
C
       CALL DIPOLE04 (PS,X,Y,Z,QX,QY,QZ)
       BX=(BBX+QX)*FINT+OIMFX*FEXT -QX
       BY=(BBY+QY)*FINT+OIMFY*FEXT -QY
       BZ=(BBZ+QZ)*FINT+OIMFZ*FEXT -QZ
c
        ENDIF  !   THE CASES (1) AND (2) ARE EXHAUSTED; THE ONLY REMAINING
C                      POSSIBILITY IS NOW THE CASE (3):
C++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
        ELSE
                CALL DIPOLE (PS,X,Y,Z,QX,QY,QZ)
                BX=OIMFX-QX
                BY=OIMFY-QY
                BZ=OIMFZ-QZ
        ENDIF
C
      END
c

C$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$
C
         SUBROUTINE  SHLCAR3X304(X,Y,Z,PS,BX,BY,BZ)
C
C   THIS S/R RETURNS THE SHIELDING FIELD FOR THE EARTH'S DIPOLE,
C   REPRESENTED BY  2x3x3=18 "CARTESIAN" HARMONICS, tilted with respect
C   to the z=0 plane
C
C - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
C  The 36 coefficients enter in pairs in the amplitudes of the "cartesian"
c    harmonics (A(1)-A(36).
c  The 14 nonlinear parameters (A(37)-A(50) are the scales Pi,Ri,Qi,and Si
C   entering the arguments of exponents, sines, and cosines in each of the
C   18 "Cartesian" harmonics  PLUS TWO TILT ANGLES FOR THE CARTESIAN HARMONICS
C       (ONE FOR THE PSI=0 MODE AND ANOTHER FOR THE PSI=90 MODE)
C - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
C
      IMPLICIT  REAL * 8  (A - H, O - Z)
C
      DIMENSION A(50)
      DATA A/-901.2327248,895.8011176,817.6208321,-845.5880889,
     *-83.73539535,86.58542841,336.8781402,-329.3619944,-311.2947120,
     *308.6011161,31.94469304,-31.30824526,125.8739681,-372.3384278,
     *-235.4720434,286.7594095,21.86305585,-27.42344605,-150.4874688,
     *2.669338538,1.395023949,-.5540427503,-56.85224007,3.681827033,
     *-43.48705106,5.103131905,1.073551279,-.6673083508,12.21404266,
     *4.177465543,5.799964188,-.3977802319,-1.044652977,.5703560010,
     *3.536082962,-3.222069852,9.620648151,6.082014949,27.75216226,
     *12.44199571,5.122226936,6.982039615,20.12149582,6.150973118,
     *4.663639687,15.73319647,2.303504968,5.840511214,.8385953499E-01,
     *.3477844929/
C
         P1=A(37)
         P2=A(38)
         P3=A(39)
         R1=A(40)
         R2=A(41)
         R3=A(42)
         Q1=A(43)
         Q2=A(44)
         Q3=A(45)
         S1=A(46)
         S2=A(47)
         S3=A(48)

         T1  =A(49)
         T2  =A(50)
C
         CPS=DCOS(PS)
         SPS=DSIN(PS)
         S2PS=2.D0*CPS

C
           ST1=DSIN(PS*T1)
           CT1=DCOS(PS*T1)
           ST2=DSIN(PS*T2)
           CT2=DCOS(PS*T2)

            X1=X*CT1-Z*ST1
            Z1=X*ST1+Z*CT1
            X2=X*CT2-Z*ST2
            Z2=X*ST2+Z*CT2
C
C
c  MAKE THE TERMS IN THE 1ST SUM ("PERPENDICULAR" SYMMETRY):
C
C       I=1
C
        SQPR= DSQRT(1.D0/P1**2+1.D0/R1**2)
        CYP = DCOS(Y/P1)
        SYP = DSIN(Y/P1)
        CZR = DCOS(Z1/R1)
        SZR = DSIN(Z1/R1)
        EXPR= DEXP(SQPR*X1)
        FX1 =-SQPR*EXPR*CYP*SZR
        HY1 = EXPR/P1*SYP*SZR
        FZ1 =-EXPR*CYP/R1*CZR
        HX1 = FX1*CT1+FZ1*ST1
        HZ1 =-FX1*ST1+FZ1*CT1

        SQPR= DSQRT(1.D0/P1**2+1.D0/R2**2)
        CYP = DCOS(Y/P1)
        SYP = DSIN(Y/P1)
        CZR = DCOS(Z1/R2)
        SZR = DSIN(Z1/R2)
        EXPR= DEXP(SQPR*X1)
        FX2 =-SQPR*EXPR*CYP*SZR
        HY2 = EXPR/P1*SYP*SZR
        FZ2 =-EXPR*CYP/R2*CZR
        HX2 = FX2*CT1+FZ2*ST1
        HZ2 =-FX2*ST1+FZ2*CT1

        SQPR= DSQRT(1.D0/P1**2+1.D0/R3**2)
        CYP = DCOS(Y/P1)
        SYP = DSIN(Y/P1)
        CZR = DCOS(Z1/R3)
        SZR = DSIN(Z1/R3)
        EXPR= DEXP(SQPR*X1)
        FX3 =-EXPR*CYP*(SQPR*Z1*CZR+SZR/R3*(X1+1.D0/SQPR))
        HY3 = EXPR/P1*SYP*(Z1*CZR+X1/R3*SZR/SQPR)
        FZ3 =-EXPR*CYP*(CZR*(1.D0+X1/R3**2/SQPR)-Z1/R3*SZR)
        HX3 = FX3*CT1+FZ3*ST1
        HZ3 =-FX3*ST1+FZ3*CT1
C
C       I=2:
C
        SQPR= DSQRT(1.D0/P2**2+1.D0/R1**2)
        CYP = DCOS(Y/P2)
        SYP = DSIN(Y/P2)
        CZR = DCOS(Z1/R1)
        SZR = DSIN(Z1/R1)
        EXPR= DEXP(SQPR*X1)
        FX4 =-SQPR*EXPR*CYP*SZR
        HY4 = EXPR/P2*SYP*SZR
        FZ4 =-EXPR*CYP/R1*CZR
        HX4 = FX4*CT1+FZ4*ST1
        HZ4 =-FX4*ST1+FZ4*CT1

        SQPR= DSQRT(1.D0/P2**2+1.D0/R2**2)
        CYP = DCOS(Y/P2)
        SYP = DSIN(Y/P2)
        CZR = DCOS(Z1/R2)
        SZR = DSIN(Z1/R2)
        EXPR= DEXP(SQPR*X1)
        FX5 =-SQPR*EXPR*CYP*SZR
        HY5 = EXPR/P2*SYP*SZR
        FZ5 =-EXPR*CYP/R2*CZR
        HX5 = FX5*CT1+FZ5*ST1
        HZ5 =-FX5*ST1+FZ5*CT1

        SQPR= DSQRT(1.D0/P2**2+1.D0/R3**2)
        CYP = DCOS(Y/P2)
        SYP = DSIN(Y/P2)
        CZR = DCOS(Z1/R3)
        SZR = DSIN(Z1/R3)
        EXPR= DEXP(SQPR*X1)
        FX6 =-EXPR*CYP*(SQPR*Z1*CZR+SZR/R3*(X1+1.D0/SQPR))
        HY6 = EXPR/P2*SYP*(Z1*CZR+X1/R3*SZR/SQPR)
        FZ6 =-EXPR*CYP*(CZR*(1.D0+X1/R3**2/SQPR)-Z1/R3*SZR)
        HX6 = FX6*CT1+FZ6*ST1
        HZ6 =-FX6*ST1+FZ6*CT1
C
C       I=3:
C
        SQPR= DSQRT(1.D0/P3**2+1.D0/R1**2)
        CYP = DCOS(Y/P3)
        SYP = DSIN(Y/P3)
        CZR = DCOS(Z1/R1)
        SZR = DSIN(Z1/R1)
        EXPR= DEXP(SQPR*X1)
        FX7 =-SQPR*EXPR*CYP*SZR
        HY7 = EXPR/P3*SYP*SZR
        FZ7 =-EXPR*CYP/R1*CZR
        HX7 = FX7*CT1+FZ7*ST1
        HZ7 =-FX7*ST1+FZ7*CT1

        SQPR= DSQRT(1.D0/P3**2+1.D0/R2**2)
        CYP = DCOS(Y/P3)
        SYP = DSIN(Y/P3)
        CZR = DCOS(Z1/R2)
        SZR = DSIN(Z1/R2)
        EXPR= DEXP(SQPR*X1)
        FX8 =-SQPR*EXPR*CYP*SZR
        HY8 = EXPR/P3*SYP*SZR
        FZ8 =-EXPR*CYP/R2*CZR
        HX8 = FX8*CT1+FZ8*ST1
        HZ8 =-FX8*ST1+FZ8*CT1

        SQPR= DSQRT(1.D0/P3**2+1.D0/R3**2)
        CYP = DCOS(Y/P3)
        SYP = DSIN(Y/P3)
        CZR = DCOS(Z1/R3)
        SZR = DSIN(Z1/R3)
        EXPR= DEXP(SQPR*X1)
        FX9 =-EXPR*CYP*(SQPR*Z1*CZR+SZR/R3*(X1+1.D0/SQPR))
        HY9 = EXPR/P3*SYP*(Z1*CZR+X1/R3*SZR/SQPR)
        FZ9 =-EXPR*CYP*(CZR*(1.D0+X1/R3**2/SQPR)-Z1/R3*SZR)
        HX9 = FX9*CT1+FZ9*ST1
        HZ9 =-FX9*ST1+FZ9*CT1


       A1=A(1)+A(2)*CPS
       A2=A(3)+A(4)*CPS
       A3=A(5)+A(6)*CPS
       A4=A(7)+A(8)*CPS
       A5=A(9)+A(10)*CPS
       A6=A(11)+A(12)*CPS
       A7=A(13)+A(14)*CPS
       A8=A(15)+A(16)*CPS
       A9=A(17)+A(18)*CPS
       BX=A1*HX1+A2*HX2+A3*HX3+A4*HX4+A5*HX5+A6*HX6+A7*HX7+A8*HX8+A9*HX9
       BY=A1*HY1+A2*HY2+A3*HY3+A4*HY4+A5*HY5+A6*HY6+A7*HY7+A8*HY8+A9*HY9
       BZ=A1*HZ1+A2*HZ2+A3*HZ3+A4*HZ4+A5*HZ5+A6*HZ6+A7*HZ7+A8*HZ8+A9*HZ9


c  MAKE THE TERMS IN THE 2ND SUM ("PARALLEL" SYMMETRY):
C
C       I=1
C
       SQQS= DSQRT(1.D0/Q1**2+1.D0/S1**2)
       CYQ = DCOS(Y/Q1)
       SYQ = DSIN(Y/Q1)
       CZS = DCOS(Z2/S1)
       SZS = DSIN(Z2/S1)
       EXQS= DEXP(SQQS*X2)
       FX1 =-SQQS*EXQS*CYQ*CZS *SPS
       HY1 = EXQS/Q1*SYQ*CZS   *SPS
       FZ1 = EXQS*CYQ/S1*SZS   *SPS
       HX1 = FX1*CT2+FZ1*ST2
       HZ1 =-FX1*ST2+FZ1*CT2

       SQQS= DSQRT(1.D0/Q1**2+1.D0/S2**2)
       CYQ = DCOS(Y/Q1)
       SYQ = DSIN(Y/Q1)
       CZS = DCOS(Z2/S2)
       SZS = DSIN(Z2/S2)
       EXQS= DEXP(SQQS*X2)
       FX2 =-SQQS*EXQS*CYQ*CZS *SPS
       HY2 = EXQS/Q1*SYQ*CZS   *SPS
       FZ2 = EXQS*CYQ/S2*SZS   *SPS
       HX2 = FX2*CT2+FZ2*ST2
       HZ2 =-FX2*ST2+FZ2*CT2

       SQQS= DSQRT(1.D0/Q1**2+1.D0/S3**2)
       CYQ = DCOS(Y/Q1)
       SYQ = DSIN(Y/Q1)
       CZS = DCOS(Z2/S3)
       SZS = DSIN(Z2/S3)
       EXQS= DEXP(SQQS*X2)
       FX3 =-SQQS*EXQS*CYQ*CZS *SPS
       HY3 = EXQS/Q1*SYQ*CZS   *SPS
       FZ3 = EXQS*CYQ/S3*SZS   *SPS
       HX3 = FX3*CT2+FZ3*ST2
       HZ3 =-FX3*ST2+FZ3*CT2
C
C       I=2
C
       SQQS= DSQRT(1.D0/Q2**2+1.D0/S1**2)
       CYQ = DCOS(Y/Q2)
       SYQ = DSIN(Y/Q2)
       CZS = DCOS(Z2/S1)
       SZS = DSIN(Z2/S1)
       EXQS= DEXP(SQQS*X2)
       FX4 =-SQQS*EXQS*CYQ*CZS *SPS
       HY4 = EXQS/Q2*SYQ*CZS   *SPS
       FZ4 = EXQS*CYQ/S1*SZS   *SPS
       HX4 = FX4*CT2+FZ4*ST2
       HZ4 =-FX4*ST2+FZ4*CT2

       SQQS= DSQRT(1.D0/Q2**2+1.D0/S2**2)
       CYQ = DCOS(Y/Q2)
       SYQ = DSIN(Y/Q2)
       CZS = DCOS(Z2/S2)
       SZS = DSIN(Z2/S2)
       EXQS= DEXP(SQQS*X2)
       FX5 =-SQQS*EXQS*CYQ*CZS *SPS
       HY5 = EXQS/Q2*SYQ*CZS   *SPS
       FZ5 = EXQS*CYQ/S2*SZS   *SPS
       HX5 = FX5*CT2+FZ5*ST2
       HZ5 =-FX5*ST2+FZ5*CT2

       SQQS= DSQRT(1.D0/Q2**2+1.D0/S3**2)
       CYQ = DCOS(Y/Q2)
       SYQ = DSIN(Y/Q2)
       CZS = DCOS(Z2/S3)
       SZS = DSIN(Z2/S3)
       EXQS= DEXP(SQQS*X2)
       FX6 =-SQQS*EXQS*CYQ*CZS *SPS
       HY6 = EXQS/Q2*SYQ*CZS   *SPS
       FZ6 = EXQS*CYQ/S3*SZS   *SPS
       HX6 = FX6*CT2+FZ6*ST2
       HZ6 =-FX6*ST2+FZ6*CT2
C
C       I=3
C
       SQQS= DSQRT(1.D0/Q3**2+1.D0/S1**2)
       CYQ = DCOS(Y/Q3)
       SYQ = DSIN(Y/Q3)
       CZS = DCOS(Z2/S1)
       SZS = DSIN(Z2/S1)
       EXQS= DEXP(SQQS*X2)
       FX7 =-SQQS*EXQS*CYQ*CZS *SPS
       HY7 = EXQS/Q3*SYQ*CZS   *SPS
       FZ7 = EXQS*CYQ/S1*SZS   *SPS
       HX7 = FX7*CT2+FZ7*ST2
       HZ7 =-FX7*ST2+FZ7*CT2

       SQQS= DSQRT(1.D0/Q3**2+1.D0/S2**2)
       CYQ = DCOS(Y/Q3)
       SYQ = DSIN(Y/Q3)
       CZS = DCOS(Z2/S2)
       SZS = DSIN(Z2/S2)
       EXQS= DEXP(SQQS*X2)
       FX8 =-SQQS*EXQS*CYQ*CZS *SPS
       HY8 = EXQS/Q3*SYQ*CZS   *SPS
       FZ8 = EXQS*CYQ/S2*SZS   *SPS
       HX8 = FX8*CT2+FZ8*ST2
       HZ8 =-FX8*ST2+FZ8*CT2

       SQQS= DSQRT(1.D0/Q3**2+1.D0/S3**2)
       CYQ = DCOS(Y/Q3)
       SYQ = DSIN(Y/Q3)
       CZS = DCOS(Z2/S3)
       SZS = DSIN(Z2/S3)
       EXQS= DEXP(SQQS*X2)
       FX9 =-SQQS*EXQS*CYQ*CZS *SPS
       HY9 = EXQS/Q3*SYQ*CZS   *SPS
       FZ9 = EXQS*CYQ/S3*SZS   *SPS
       HX9 = FX9*CT2+FZ9*ST2
       HZ9 =-FX9*ST2+FZ9*CT2

       A1=A(19)+A(20)*S2PS
       A2=A(21)+A(22)*S2PS
       A3=A(23)+A(24)*S2PS
       A4=A(25)+A(26)*S2PS
       A5=A(27)+A(28)*S2PS
       A6=A(29)+A(30)*S2PS
       A7=A(31)+A(32)*S2PS
       A8=A(33)+A(34)*S2PS
       A9=A(35)+A(36)*S2PS

       BX=BX+A1*HX1+A2*HX2+A3*HX3+A4*HX4+A5*HX5+A6*HX6+A7*HX7+A8*HX8
     *   +A9*HX9
       BY=BY+A1*HY1+A2*HY2+A3*HY3+A4*HY4+A5*HY5+A6*HY6+A7*HY7+A8*HY8
     *   +A9*HY9
       BZ=BZ+A1*HZ1+A2*HZ2+A3*HZ3+A4*HZ4+A5*HZ5+A6*HZ6+A7*HZ7+A8*HZ8
     *   +A9*HZ9
C
       RETURN
       END
c
c############################################################################
c
C
      SUBROUTINE DEFORMED04 (IOPT,PS,X,Y,Z,BX1,BY1,BZ1,BX2,BY2,BZ2)
C
C   IOPT - TAIL FIELD MODE FLAG:   IOPT=0 - THE TWO TAIL MODES ARE ADDED UP
C                                  IOPT=1 - MODE 1 ONLY
C                                  IOPT=2 - MODE 2 ONLY
C
C   CALCULATES GSM COMPONENTS OF TWO UNIT-AMPLITUDE TAIL FIELD MODES,
C    TAKING INTO ACCOUNT BOTH EFFECTS OF DIPOLE TILT:
C    WARPING IN Y-Z (DONE BY THE S/R WARPED) AND BENDING IN X-Z (DONE BY THIS SUBROUTINE)
C
      IMPLICIT REAL*8 (A-H,O-Z)
      COMMON /RH004/ RH004
      DATA RH2,IEPS /-5.2D0,3/
C
C  RH0,RH1,RH2, AND IEPS CONTROL THE TILT-RELATED DEFORMATION OF THE TAIL FIELD
C
      SPS=DSIN(PS)
      CPS=DSQRT(1.D0-SPS**2)
      R2=X**2+Y**2+Z**2
      R=DSQRT(R2)
      ZR=Z/R
      RH=RH004+RH2*ZR**2
      DRHDR=-ZR/R*2.D0*RH2*ZR
      DRHDZ= 2.D0*RH2*ZR/R
C
      RRH=R/RH

      F=1.D0/(1.D0+RRH**IEPS)**(1.D0/IEPS)
      DFDR=-RRH**(IEPS-1)*F**(IEPS+1)/RH
      DFDRH=-RRH*DFDR
c
      SPSAS=SPS*F
      CPSAS=DSQRT(1.D0-SPSAS**2)
C
      XAS=X*CPSAS-Z*SPSAS
      ZAS=X*SPSAS+Z*CPSAS
C
      FACPS=SPS/CPSAS*(DFDR+DFDRH*DRHDR)/R
      PSASX=FACPS*X
      PSASY=FACPS*Y
      PSASZ=FACPS*Z+SPS/CPSAS*DFDRH*DRHDZ
C
      DXASDX=CPSAS-ZAS*PSASX
      DXASDY=-ZAS*PSASY
      DXASDZ=-SPSAS-ZAS*PSASZ
      DZASDX=SPSAS+XAS*PSASX
      DZASDY=XAS*PSASY
      DZASDZ=CPSAS+XAS*PSASZ
      FAC1=DXASDZ*DZASDY-DXASDY*DZASDZ
      FAC2=DXASDX*DZASDZ-DXASDZ*DZASDX
      FAC3=DZASDX*DXASDY-DXASDX*DZASDY
C
C     DEFORM:
C
      CALL WARPED04(IOPT,PS,XAS,Y,ZAS,BXAS1,BYAS1,
     +BZAS1,BXAS2,BYAS2,BZAS2)
C
      BX1=BXAS1*DZASDZ-BZAS1*DXASDZ +BYAS1*FAC1
      BY1=BYAS1*FAC2
      BZ1=BZAS1*DXASDX-BXAS1*DZASDX +BYAS1*FAC3

      BX2=BXAS2*DZASDZ-BZAS2*DXASDZ +BYAS2*FAC1
      BY2=BYAS2*FAC2
      BZ2=BZAS2*DXASDX-BXAS2*DZASDX +BYAS2*FAC3

      RETURN
      END
C
C------------------------------------------------------------------
c
C
      SUBROUTINE WARPED04 (IOPT,PS,X,Y,Z,BX1,BY1,BZ1,BX2,BY2,BZ2)
C
C   CALCULATES GSM COMPONENTS OF THE WARPED FIELD FOR TWO TAIL UNIT MODES.
C   THE WARPING DEFORMATION IS IMPOSED ON THE UNWARPED FIELD, COMPUTED
C   BY THE S/R "UNWARPED".  THE WARPING PARAMETERS WERE TAKEN FROM THE
C   RESULTS OF GEOTAIL OBSERVATIONS (TSYGANENKO ET AL. [1998]).
C   NB # 6, P.106, OCT 12, 2000.
C
C   IOPT - TAIL FIELD MODE FLAG:   IOPT=0 - THE TWO TAIL MODES ARE ADDED UP
C                                  IOPT=1 - MODE 1 ONLY
C                                  IOPT=2 - MODE 2 ONLY
C
      IMPLICIT REAL*8 (A-H,O-Z)
C
      COMMON /G/ G
      DGDX=0.D0
      XL=20.D0
      DXLDX=0.D0

      SPS=DSIN(PS)
      RHO2=Y**2+Z**2
      RHO=DSQRT(RHO2)

      IF (Y.EQ.0.D0.AND.Z.EQ.0.D0) THEN
       PHI=0.D0
       CPHI=1.D0
       SPHI=0.D0
      ELSE
       PHI=DATAN2(Z,Y)
       CPHI=Y/RHO
       SPHI=Z/RHO
      ENDIF

      RR4L4=RHO/(RHO2**2+XL**4)

      F=PHI+G*RHO2*RR4L4*CPHI*SPS
      DFDPHI=1.D0-G*RHO2*RR4L4*SPHI*SPS
      DFDRHO=G*RR4L4**2*(3.D0*XL**4-RHO2**2)*CPHI*SPS
      DFDX=RR4L4*CPHI*SPS*(DGDX*RHO2-G*RHO*RR4L4*4.D0*XL**3*DXLDX)

      CF=DCOS(F)
      SF=DSIN(F)
      YAS=RHO*CF
      ZAS=RHO*SF

      CALL UNWARPED04 (IOPT,X,YAS,ZAS,BX_AS1,BY_AS1,BZ_AS1,
     *  BX_AS2,BY_AS2,BZ_AS2)

      BRHO_AS =  BY_AS1*CF+BZ_AS1*SF      !   DEFORM THE 1ST MODE
      BPHI_AS = -BY_AS1*SF+BZ_AS1*CF

      BRHO_S = BRHO_AS*DFDPHI
      BPHI_S = BPHI_AS-RHO*(BX_AS1*DFDX+BRHO_AS*DFDRHO)
      BX1    = BX_AS1*DFDPHI

      BY1    = BRHO_S*CPHI-BPHI_S*SPHI
      BZ1    = BRHO_S*SPHI+BPHI_S*CPHI    !   DONE

      BRHO_AS =  BY_AS2*CF+BZ_AS2*SF      !   DEFORM THE 2ND MODE
      BPHI_AS = -BY_AS2*SF+BZ_AS2*CF

      BRHO_S = BRHO_AS*DFDPHI
      BPHI_S = BPHI_AS-RHO*(BX_AS2*DFDX+BRHO_AS*DFDRHO)
      BX2    = BX_AS2*DFDPHI

      BY2    = BRHO_S*CPHI-BPHI_S*SPHI
      BZ2    = BRHO_S*SPHI+BPHI_S*CPHI    !   DONE

      RETURN
      END
C
C%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
C
       SUBROUTINE UNWARPED04 (IOPT,X,Y,Z,BX1,BY1,BZ1,BX2,BY2,BZ2)

C   IOPT - TAIL FIELD MODE FLAG:   IOPT=0 - THE TWO TAIL MODES ARE ADDED UP
C                                  IOPT=1 - MODE 1 ONLY
C                                  IOPT=2 - MODE 2 ONLY
C
C    CALCULATES GSM COMPONENTS OF THE SHIELDED FIELD OF TWO TAIL MODES WITH UNIT
C    AMPLITUDES,  WITHOUT ANY WARPING OR BENDING.  NONLINEAR PARAMETERS OF THE MODES
C    ARE FORWARDED HERE VIA A COMMON BLOCK /TAIL/.
C
      IMPLICIT REAL*8 (A-H,O-Z)
C
      DIMENSION A1(60),A2(60)  !   TAIL SHIELDING FIELD PARAMETERS FOR THE MODES #1 & #2

      COMMON /TAIL/ DXSHIFT1,DXSHIFT2,D0,DELTADY  ! ATTENTION:  HERE D0 & DELTADY ARE INCLUDED IN /TAIL/
C                                                                  AND EXCLUDED FROM DATA
      DATA DELTADX1,ALPHA1,XSHIFT1
     *  /1.D0,1.1D0,6.D0/
      DATA DELTADX2,ALPHA2,XSHIFT2
     *  /0.D0,.25D0,4.D0/

      DATA A1/-25.45869857,57.35899080,317.5501869,-2.626756717,
     *-93.38053698,-199.6467926,-858.8129729,34.09192395,845.4214929,
     *-29.07463068,47.10678547,-128.9797943,-781.7512093,6.165038619,
     *167.8905046,492.0680410,1654.724031,-46.77337920,-1635.922669,
     *40.86186772,-.1349775602,-.9661991179E-01,-.1662302354,
     *.002810467517,.2487355077,.1025565237,-14.41750229,-.8185333989,
     *11.07693629,.7569503173,-9.655264745,112.2446542,777.5948964,
     *-5.745008536,-83.03921993,-490.2278695,-1155.004209,39.08023320,
     *1172.780574,-39.44349797,-14.07211198,-40.41201127,-313.2277343,
     *2.203920979,8.232835341,197.7065115,391.2733948,-18.57424451,
     *-437.2779053,23.04976898,11.75673963,13.60497313,4.691927060,
     *18.20923547,27.59044809,6.677425469,1.398283308,2.839005878,
     *31.24817706,24.53577264/

      DATA A2/-287187.1962,4970.499233,410490.1952,-1347.839052,
     *-386370.3240,3317.983750,-143462.3895,5706.513767,171176.2904,
     *250.8882750,-506570.8891,5733.592632,397975.5842,9771.762168,
     *-941834.2436,7990.975260,54313.10318,447.5388060,528046.3449,
     *12751.04453,-21920.98301,-21.05075617,31971.07875,3012.641612,
     *-301822.9103,-3601.107387,1797.577552,-6.315855803,142578.8406,
     *13161.93640,804184.8410,-14168.99698,-851926.6360,-1890.885671,
     *972475.6869,-8571.862853,26432.49197,-2554.752298,-482308.3431,
     *-4391.473324,105155.9160,-1134.622050,-74353.53091,-5382.670711,
     *695055.0788,-916.3365144,-12111.06667,67.20923358,-367200.9285,
     *-21414.14421,14.75567902,20.75638190,59.78601609,16.86431444,
     *32.58482365,23.69472951,17.24977936,13.64902647,68.40989058,
     *11.67828167/

      DATA XM1,XM2/2*-12.D0/

      IF (IOPT.EQ.2) GOTO 1

      XSC1=(X-XSHIFT1-DXSHIFT1)*ALPHA1-XM1*(ALPHA1-1.D0)
      YSC1=Y*ALPHA1
      ZSC1=Z*ALPHA1
      D0SC1=D0*ALPHA1   ! HERE WE USE A SINGLE VALUE D0 OF THE THICKNESS FOR BOTH MODES

      CALL TAILDISK04(D0SC1,DELTADX1,DELTADY,XSC1,YSC1,ZSC1,FX1,FY1,FZ1)
      CALL SHLCAR5X504(A1,X,Y,Z,DXSHIFT1,HX1,HY1,HZ1)

      BX1=FX1+HX1
      BY1=FY1+HY1
      BZ1=FZ1+HZ1

      IF (IOPT.EQ.1) THEN
        BX2=0.D0
        BY2=0.D0
        BZ2=0.D0
        RETURN
      ENDIF

 1    XSC2=(X-XSHIFT2-DXSHIFT2)*ALPHA2-XM2*(ALPHA2-1.D0)
      YSC2=Y*ALPHA2
      ZSC2=Z*ALPHA2
      D0SC2=D0*ALPHA2   ! HERE WE USE A SINGLE VALUE D0 OF THE THICKNESS FOR BOTH MODES

      CALL TAILDISK04(D0SC2,DELTADX2,DELTADY,XSC2,YSC2,ZSC2,FX2,FY2,FZ2)
      CALL SHLCAR5X504(A2,X,Y,Z,DXSHIFT2,HX2,HY2,HZ2)

      BX2=FX2+HX2
      BY2=FY2+HY2
      BZ2=FZ2+HZ2

      IF (IOPT.EQ.2) THEN
        BX1=0.D0
        BY1=0.D0
        BZ1=0.D0
        RETURN
      ENDIF

      RETURN
      END
C
C$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$
C
      SUBROUTINE TAILDISK04(D0,DELTADX,DELTADY,X,Y,Z,BX,BY,BZ)
c
c      THIS SUBROUTINE COMPUTES THE COMPONENTS OF THE TAIL CURRENT FIELD,
C       SIMILAR TO THAT DESCRIBED BY TSYGANENKO AND PEREDO (1994).  THE
C        DIFFERENCE IS THAT NOW WE USE SPACEWARPING, AS DESCRIBED IN OUR
C         PAPER ON MODELING BIRKELAND CURRENTS (TSYGANENKO AND STERN, 1996)
C          INSTEAD OF SHEARING IT IN THE SPIRIT OF T89 TAIL MODEL.
C
      IMPLICIT REAL*8 (A-H,O-Z)
c
      DIMENSION F(5),B(5),C(5)
C
      DATA F /-71.09346626D0,-1014.308601D0,-1272.939359D0,
     *        -3224.935936D0,-44546.86232D0/
      DATA B /10.90101242D0,12.68393898D0,13.51791954D0,14.86775017D0,
     *          15.12306404D0/
      DATA C /.7954069972D0,.6716601849D0,1.174866319D0,2.565249920D0,
     *          10.01986790D0/
C
      RHO=DSQRT(X**2+Y**2)
      DRHODX=X/RHO
      DRHODY=Y/RHO

      DEX=DEXP(X/7.D0)
      D=D0+DELTADY*(Y/20.D0)**2  +DELTADX*DEX !   THE LAST TERM (INTRODUCED 10/11/2000) MAKES THE SHEET
      DDDY=DELTADY*Y*0.005D0                  !   THICKEN SUNWARD, TO AVOID PROBLEMS IN THE SUBSOLAR REGION
      DDDX=DELTADX/7.D0*DEX
C
      DZETA=DSQRT(Z**2+D**2)  !  THIS IS THE SAME SIMPLE WAY TO SPREAD
C                                        OUT THE SHEET, AS THAT USED IN T89
      DDZETADX=D*DDDX/DZETA
      DDZETADY=D*DDDY/DZETA
      DDZETADZ=Z/DZETA

C
      DBX=0.D0
      DBY=0.D0
      DBZ=0.D0
C
      DO 1 I=1,5
C
      BI=B(I)
      CI=C(I)
C
      S1=DSQRT((RHO+BI)**2+(DZETA+CI)**2)
      S2=DSQRT((RHO-BI)**2+(DZETA+CI)**2)

      DS1DRHO=(RHO+BI)/S1
      DS2DRHO=(RHO-BI)/S2
      DS1DDZ=(DZETA+CI)/S1
      DS2DDZ=(DZETA+CI)/S2
C
      DS1DX=DS1DRHO*DRHODX  +DS1DDZ*DDZETADX
      DS1DY=DS1DRHO*DRHODY  +   DS1DDZ*DDZETADY
      DS1DZ=                      DS1DDZ*DDZETADZ
C
      DS2DX=DS2DRHO*DRHODX  +DS2DDZ*DDZETADX
      DS2DY=DS2DRHO*DRHODY  +   DS2DDZ*DDZETADY
      DS2DZ=                    DS2DDZ*DDZETADZ
C
      S1TS2=S1*S2
      S1PS2=S1+S2
      S1PS2SQ=S1PS2**2

      FAC1=DSQRT(S1PS2SQ-(2.D0*BI)**2)
      AS=FAC1/(S1TS2*S1PS2SQ)
      DASDS1=(1.D0/(FAC1*S2)-AS/S1PS2*(S2*S2+S1*(3.D0*S1+4.D0*S2)))
     *          /(S1*S1PS2)
      DASDS2=(1.D0/(FAC1*S1)-AS/S1PS2*(S1*S1+S2*(3.D0*S2+4.D0*S1)))
     *          /(S2*S1PS2)
C
      DASDX=DASDS1*DS1DX+DASDS2*DS2DX
      DASDY=DASDS1*DS1DY+DASDS2*DS2DY
      DASDZ=DASDS1*DS1DZ+DASDS2*DS2DZ
C
      DBX=DBX-F(I)*X*DASDZ
      DBY=DBY-F(I)*Y*DASDZ
  1   DBZ=DBZ+F(I)*(2.D0*AS+X*DASDX+Y*DASDY)

      BX=DBX
      BY=DBY
      BZ=DBZ

      RETURN
      END
C
C$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$$
C
C THIS CODE RETURNS THE SHIELDING FIELD REPRESENTED BY  5x5=25 "CARTESIAN"
C    HARMONICS
C
         SUBROUTINE  SHLCAR5X504(A,X,Y,Z,DSHIFT,HX,HY,HZ)
C
C - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
C  The NLIN coefficients are the amplitudes of the "cartesian"
c    harmonics (A(1)-A(NLIN).
c  The NNP nonlinear parameters (A(NLIN+1)-A(NTOT) are the scales Pi and Ri
C   entering the arguments of exponents, sines, and cosines in each of the
C   NLIN "Cartesian" harmonics
C - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
C
         IMPLICIT  REAL * 8  (A - H, O - Z)
C
         DIMENSION A(60)
C
         DHX=0.D0
         DHY=0.D0
         DHZ=0.D0

         L=0
C
         DO 2 I=1,5
         RP=1.D0/A(50+I)
         CYPI=DCOS(Y*RP)
         SYPI=DSIN(Y*RP)
C
         DO 2 K=1,5
         RR=1.D0/A(55+K)
         SZRK=DSIN(Z*RR)
         CZRK=DCOS(Z*RR)
         SQPR=DSQRT(RP**2+RR**2)
         EPR=DEXP(X*SQPR)
C
         DBX=-SQPR*EPR*CYPI*SZRK
         DBY= RP*EPR*SYPI*SZRK
         DBZ=-RR*EPR*CYPI*CZRK

         L=L+2
         COEF=A(L-1)+A(L)*DSHIFT

         DHX=DHX+COEF*DBX
         DHY=DHY+COEF*DBY
         DHZ=DHZ+COEF*DBZ
c
  2      CONTINUE

         HX=DHX
         HY=DHY
         HZ=DHZ
C
      RETURN
      END
c
c %%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
C
      SUBROUTINE BIRK_TOT04 (IOPB,PS,X,Y,Z,BX11,BY11,BZ11,BX12,BY12,BZ12,
     *                          BX21,BY21,BZ21,BX22,BY22,BZ22)
C
C      IOPB -  BIRKELAND FIELD MODE FLAG:
C         IOPB=0 - ALL COMPONENTS
C         IOPB=1 - REGION 1, MODES 1 & 2
C         IOPB=2 - REGION 2, MODES 1 & 2
C
      IMPLICIT REAL*8 (A-H,O-Z)
      DIMENSION SH11(86),SH12(86),SH21(86),SH22(86)
      COMMON /BIRKPAR/ XKAPPA1,XKAPPA2   !  INPUT PARAMETERS, SPECIFIED FROM A MAIN PROGRAM
      COMMON /DPHI_B_RHO0/ DPHI,B,RHO_0,XKAPPA ! PARAMETERS, CONTROL DAY-NIGHT ASYMMETRY OF F.A.C.

      DATA SH11/46488.84663,-15541.95244,-23210.09824,-32625.03856,
     *-109894.4551,-71415.32808,58168.94612,55564.87578,-22890.60626,
     *-6056.763968,5091.368100,239.7001538,-13899.49253,4648.016991,
     *6971.310672,9699.351891,32633.34599,21028.48811,-17395.96190,
     *-16461.11037,7447.621471,2528.844345,-1934.094784,-588.3108359,
     *-32588.88216,10894.11453,16238.25044,22925.60557,77251.11274,
     *50375.97787,-40763.78048,-39088.60660,15546.53559,3559.617561,
     *-3187.730438,309.1487975,88.22153914,-243.0721938,-63.63543051,
     *191.1109142,69.94451996,-187.9539415,-49.89923833,104.0902848,
     *-120.2459738,253.5572433,89.25456949,-205.6516252,-44.93654156,
     *124.7026309,32.53005523,-98.85321751,-36.51904756,98.88241690,
     *24.88493459,-55.04058524,61.14493565,-128.4224895,-45.35023460,
     *105.0548704,-43.66748755,119.3284161,31.38442798,-92.87946767,
     *-33.52716686,89.98992001,25.87341323,-48.86305045,59.69362881,
     *-126.5353789,-44.39474251,101.5196856,59.41537992,41.18892281,
     *80.86101200,3.066809418,7.893523804,30.56212082,10.36861082,
     *8.222335945,19.97575641,2.050148531,4.992657093,2.300564232,
     *.2256245602,-.05841594319/

      DATA SH12/210260.4816,-1443587.401,-1468919.281,281939.2993,
     *-1131124.839,729331.7943,2573541.307,304616.7457,468887.5847,
     *181554.7517,-1300722.650,-257012.8601,645888.8041,-2048126.412,
     *-2529093.041,571093.7972,-2115508.353,1122035.951,4489168.802,
     *75234.22743,823905.6909,147926.6121,-2276322.876,-155528.5992,
     *-858076.2979,3474422.388,3986279.931,-834613.9747,3250625.781,
     *-1818680.377,-7040468.986,-414359.6073,-1295117.666,-346320.6487,
     *3565527.409,430091.9496,-.1565573462,7.377619826,.4115646037,
     *-6.146078880,3.808028815,-.5232034932,1.454841807,-12.32274869,
     *-4.466974237,-2.941184626,-.6172620658,12.64613490,1.494922012,
     *-21.35489898,-1.652256960,16.81799898,-1.404079922,-24.09369677,
     *-10.99900839,45.94237820,2.248579894,31.91234041,7.575026816,
     *-45.80833339,-1.507664976,14.60016998,1.348516288,-11.05980247,
     *-5.402866968,31.69094514,12.28261196,-37.55354174,4.155626879,
     *-33.70159657,-8.437907434,36.22672602,145.0262164,70.73187036,
     *85.51110098,21.47490989,24.34554406,31.34405345,4.655207476,
     *5.747889264,7.802304187,1.844169801,4.867254550,2.941393119,
     *.1379899178,.06607020029/

      DATA SH21/162294.6224,503885.1125,-27057.67122,-531450.1339,
     *84747.05678,-237142.1712,84133.61490,259530.0402,69196.05160,
     *-189093.5264,-19278.55134,195724.5034,-263082.6367,-818899.6923,
     *43061.10073,863506.6932,-139707.9428,389984.8850,-135167.5555,
     *-426286.9206,-109504.0387,295258.3531,30415.07087,-305502.9405,
     *100785.3400,315010.9567,-15999.50673,-332052.2548,54964.34639,
     *-152808.3750,51024.67566,166720.0603,40389.67945,-106257.7272,
     *-11126.14442,109876.2047,2.978695024,558.6019011,2.685592939,
     *-338.0004730,-81.99724090,-444.1102659,89.44617716,212.0849592,
     *-32.58562625,-982.7336105,-35.10860935,567.8931751,-1.917212423,
     *-260.2023543,-1.023821735,157.5533477,23.00200055,232.0603673,
     *-36.79100036,-111.9110936,18.05429984,447.0481000,15.10187415,
     *-258.7297813,-1.032340149,-298.6402478,-1.676201415,180.5856487,
     *64.52313024,209.0160857,-53.85574010,-98.52164290,14.35891214,
     *536.7666279,20.09318806,-309.7349530,58.54144539,67.45226850,
     *97.92374406,4.752449760,10.46824379,32.91856110,12.05124381,
     *9.962933904,15.91258637,1.804233877,6.578149088,2.515223491,
     *.1930034238,-.02261109942/

      DATA SH22/-131287.8986,-631927.6885,-318797.4173,616785.8782,
     *-50027.36189,863099.9833,47680.20240,-1053367.944,-501120.3811,
     *-174400.9476,222328.6873,333551.7374,-389338.7841,-1995527.467,
     *-982971.3024,1960434.268,297239.7137,2676525.168,-147113.4775,
     *-3358059.979,-2106979.191,-462827.1322,1017607.960,1039018.475,
     *520266.9296,2627427.473,1301981.763,-2577171.706,-238071.9956,
     *-3539781.111,94628.16420,4411304.724,2598205.733,637504.9351,
     *-1234794.298,-1372562.403,-2.646186796,-31.10055575,2.295799273,
     *19.20203279,30.01931202,-302.1028550,-14.78310655,162.1561899,
     *.4943938056,176.8089129,-.2444921680,-100.6148929,9.172262228,
     *137.4303440,-8.451613443,-84.20684224,-167.3354083,1321.830393,
     *76.89928813,-705.7586223,18.28186732,-770.1665162,-9.084224422,
     *436.3368157,-6.374255638,-107.2730177,6.080451222,65.53843753,
     *143.2872994,-1028.009017,-64.22739330,547.8536586,-20.58928632,
     *597.3893669,10.17964133,-337.7800252,159.3532209,76.34445954,
     *84.74398828,12.76722651,27.63870691,32.69873634,5.145153451,
     *6.310949163,6.996159733,1.971629939,4.436299219,2.904964304,
     *.1486276863,.06859991529/

      XKAPPA=XKAPPA1        !  FORWARDED IN BIRK_1N2
      X_SC=XKAPPA1-1.1D0    !  FORWARDED IN BIRK_SHL

      IF (IOPB.EQ.0.OR.IOPB.EQ.1) THEN

      CALL BIRK_1N204 (1,1,PS,X,Y,Z,FX11,FY11,FZ11)           !  REGION 1, MODE 1
      CALL BIRK_SHL04 (SH11,PS,X_SC,X,Y,Z,HX11,HY11,HZ11)
      BX11=FX11+HX11
      BY11=FY11+HY11
      BZ11=FZ11+HZ11

      CALL BIRK_1N204 (1,2,PS,X,Y,Z,FX12,FY12,FZ12)           !  REGION 1, MODE 2
      CALL BIRK_SHL04 (SH12,PS,X_SC,X,Y,Z,HX12,HY12,HZ12)
      BX12=FX12+HX12
      BY12=FY12+HY12
      BZ12=FZ12+HZ12

      ENDIF

      XKAPPA=XKAPPA2        !  FORWARDED IN BIRK_1N2
      X_SC=XKAPPA2-1.0D0    !  FORWARDED IN BIRK_SHL

      IF (IOPB.EQ.0.OR.IOPB.EQ.2) THEN

      CALL BIRK_1N204 (2,1,PS,X,Y,Z,FX21,FY21,FZ21)           !  REGION 2, MODE 1
      CALL BIRK_SHL04 (SH21,PS,X_SC,X,Y,Z,HX21,HY21,HZ21)
      BX21=FX21+HX21
      BY21=FY21+HY21
      BZ21=FZ21+HZ21

      CALL BIRK_1N204 (2,2,PS,X,Y,Z,FX22,FY22,FZ22)           !  REGION 2, MODE 2
      CALL BIRK_SHL04 (SH22,PS,X_SC,X,Y,Z,HX22,HY22,HZ22)
      BX22=FX22+HX22
      BY22=FY22+HY22
      BZ22=FZ22+HZ22

      ENDIF

      RETURN
      END
C
c %%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
c
c
      SUBROUTINE BIRK_1N204 (NUMB,MODE,PS,X,Y,Z,BX,BY,BZ)
C
C  CALCULATES COMPONENTS  OF REGION 1/2 FIELD IN SPHERICAL COORDS.  DERIVED FROM THE S/R DIPDEF2C (WHICH
C    DOES THE SAME JOB, BUT INPUT/OUTPUT THERE WAS IN SPHERICAL COORDS, WHILE HERE WE USE CARTESIAN ONES)
C
C   INPUT:  NUMB=1 (2) FOR REGION 1 (2) CURRENTS
C           MODE=1 YIELDS SIMPLE SINUSOIDAL MLT VARIATION, WITH MAXIMUM CURRENT AT DAWN/DUSK MERIDIAN
C     WHILE MODE=2 YIELDS THE SECOND HARMONIC.
C
C
      IMPLICIT REAL*8 (A-H,O-Z)
      DIMENSION A11(31),A12(31),A21(31),A22(31)
      COMMON /MODENUM/ M
      COMMON /DTHETA/ DTHETA

      COMMON /DPHI_B_RHO0/ DPHI,B,RHO_0,XKAPPA ! THESE PARAMETERS CONTROL DAY-NIGHT ASYMMETRY OF F.A.C., AS FOLLOWS:

C  (1) DPHI:   HALF-DIFFERENCE (IN RADIANS) BETWEEN DAY AND NIGHT LATITUDE OF FAC OVAL AT IONOSPHERIC ALTITUDE;
C              TYPICAL VALUE: 0.06
C  (2) B:      AN ASYMMETRY FACTOR AT HIGH-ALTITUDES;  FOR B=0, THE ONLY ASYMMETRY IS THAT FROM DPHI
C              TYPICAL VALUES: 0.35-0.70
C  (3) RHO_0:  A FIXED PARAMETER, DEFINING THE DISTANCE RHO, AT WHICH THE LATITUDE SHIFT GRADUALLY SATURATES AND
C              STOPS INCREASING
C              ITS VALUE WAS ASSUMED FIXED, EQUAL TO 7.0.
C  (4) XKAPPA: AN OVERALL SCALING FACTOR, WHICH CAN BE USED FOR CHANGING THE SIZE OF THE F.A.C. OVAL
C
      DATA BETA,RH,EPS/0.9D0,10.D0,3.D0/ ! parameters of the tilt-dependent deformation of the untilted F.A.C. field

      DATA A11/.1618068350,-.1797957553,2.999642482,-.9322708978,
     *-.6811059760,.2099057262,-8.358815746,-14.86033550,.3838362986,
     *-16.30945494,4.537022847,2.685836007,27.97833029,6.330871059,
     *1.876532361,18.95619213,.9651528100,.4217195118,-.08957770020,
     *-1.823555887,.7457045438,-.5785916524,-1.010200918,.01112389357,
     *.09572927448,-.3599292276,8.713700514,.9763932955,3.834602998,
     *2.492118385,.7113544659/
      DATA A12/.7058026940,-.2845938535,5.715471266,-2.472820880,
     *-.7738802408,.3478293930,-11.37653694,-38.64768867,.6932927651,
     *-212.4017288,4.944204937,3.071270411,33.05882281,7.387533799,
     *2.366769108,79.22572682,.6154290178,.5592050551,-.1796585105,
     *-1.654932210,.7309108776,-.4926292779,-1.130266095,-.009613974555,
     *.1484586169,-.2215347198,7.883592948,.02768251655,2.950280953,
     *1.212634762,.5567714182/
      DATA A21/.1278764024,-.2320034273,1.805623266,-32.37241440,
     *-.9931490648,.3175085630,-2.492465814,-16.21600096,.2695393416,
     *-6.752691265,3.971794901,14.54477563,41.10158386,7.912889730,
     *1.258297372,9.583547721,1.014141963,.5104134759,-.1790430468,
     *-1.756358428,.7561986717,-.6775248254,-.04014016420,.01446794851,
     *.1200521731,-.2203584559,4.508963850,.8221623576,1.779933730,
     *1.102649543,.8867880020/
      DATA A22/.4036015198,-.3302974212,2.827730930,-45.44405830,
     *-1.611103927,.4927112073,-.003258457559,-49.59014949,.3796217108,
     *-233.7884098,4.312666980,18.05051709,28.95320323,11.09948019,
     *.7471649558,67.10246193,.5667096597,.6468519751,-.1560665317,
     *-1.460805289,.7719653528,-.6658988668,.2515179349E-05,
     *.02426021891,.1195003324,-.2625739255,4.377172556,.2421190547,
     *2.503482679,1.071587299,.7247997430/

      B=0.5
      RHO_0=7.0

      M=MODE
      IF (NUMB.EQ.1) THEN
          DPHI=0.055D0
          DTHETA=0.06D0
      ENDIF

      IF (NUMB.EQ.2) THEN
          DPHI=0.030D0
          DTHETA=0.09D0
      ENDIF

      Xsc=X*XKAPPA
      Ysc=Y*XKAPPA
      Zsc=Z*XKAPPA
      RHO=DSQRT(Xsc**2+Zsc**2)

      Rsc=DSQRT(Xsc**2+Ysc**2+Zsc**2)                                 !  SCALED
      RHO2=RHO_0**2

      IF (Xsc.EQ.0.D0.AND.Zsc.EQ.0.D0) THEN
         PHI=0.D0
      ELSE
         PHI=DATAN2(-Zsc,Xsc)  !  FROM CARTESIAN TO CYLINDRICAL (RHO,PHI,Y)
      ENDIF

      SPHIC=DSIN(PHI)
      CPHIC=DCOS(PHI)  !  "C" means "CYLINDRICAL", TO DISTINGUISH FROM SPHERICAL PHI

      BRACK=DPHI+B*RHO2/(RHO2+1.D0)*(RHO**2-1.D0)/(RHO2+RHO**2)
      R1RH=(Rsc-1.D0)/RH
      PSIAS=BETA*PS/(1.D0+R1RH**EPS)**(1.D0/EPS)

      PHIS=PHI-BRACK*DSIN(PHI) -PSIAS
      DPHISPHI=1.D0-BRACK*DCOS(PHI)
      DPHISRHO=-2.D0*B*RHO2*RHO/(RHO2+RHO**2)**2 *DSIN(PHI)
     *   +BETA*PS*R1RH**(EPS-1.D0)*RHO/(RH*Rsc*
     *   (1.D0+R1RH**EPS)**(1.D0/EPS+1.D0))
      DPHISDY= BETA*PS*R1RH**(EPS-1.D0)*Ysc/(RH*Rsc*
     *   (1.D0+R1RH**EPS)**(1.D0/EPS+1.D0))

      SPHICS=DSIN(PHIS)
      CPHICS=DCOS(PHIS)

      XS= RHO*CPHICS
      ZS=-RHO*SPHICS

      IF (NUMB.EQ.1) THEN
        IF (MODE.EQ.1) CALL TWOCONES04 (A11,XS,Ysc,ZS,BXS,BYAS,BZS)
        IF (MODE.EQ.2) CALL TWOCONES04 (A12,XS,Ysc,ZS,BXS,BYAS,BZS)
      ELSE
        IF (MODE.EQ.1) CALL TWOCONES04 (A21,XS,Ysc,ZS,BXS,BYAS,BZS)
        IF (MODE.EQ.2) CALL TWOCONES04 (A22,XS,Ysc,ZS,BXS,BYAS,BZS)
      ENDIF

      BRHOAS=BXS*CPHICS-BZS*SPHICS
      BPHIAS=-BXS*SPHICS-BZS*CPHICS

      BRHO_S=BRHOAS*DPHISPHI                             *XKAPPA        ! SCALING
      BPHI_S=(BPHIAS-RHO*(BYAS*DPHISDY+BRHOAS*DPHISRHO)) *XKAPPA
      BY_S=BYAS*DPHISPHI                                 *XKAPPA

      BX=BRHO_S*CPHIC-BPHI_S*SPHIC
      BY=BY_S
      BZ=-BRHO_S*SPHIC-BPHI_S*CPHIC

      RETURN
      END
c
C=========================================================================
c
      SUBROUTINE TWOCONES04 (A,X,Y,Z,BX,BY,BZ)
C
C   ADDS FIELDS FROM TWO CONES (NORTHERN AND SOUTHERN), WITH A PROPER SYMMETRY OF THE CURRENT AND FIELD,
C     CORRESPONDING TO THE REGION 1 BIRKELAND CURRENTS.
C

      IMPLICIT REAL*8 (A-H,O-Z)
      DIMENSION A(31)

      CALL ONE_CONE04 (A,X,Y,Z,BXN,BYN,BZN)
      CALL ONE_CONE04 (A,X,-Y,-Z,BXS,BYS,BZS)
      BX=BXN-BXS
      BY=BYN+BYS
      BZ=BZN+BZS

      RETURN
      END
c
C-------------------------------------------------------------------------
C
      SUBROUTINE ONE_CONE04(A,X,Y,Z,BX,BY,BZ)
c
c  RETURNS FIELD COMPONENTS FOR A DEFORMED CONICAL CURRENT SYSTEM, FITTED TO A BIOSAVART FIELD
c    BY SIM_14.FOR.  HERE ONLY THE NORTHERN CONE IS TAKEN INTO ACCOUNT.
c

      IMPLICIT REAL*8 (A-H,O-Z)
      DIMENSION A(31)

      COMMON /DTHETA/ DTHETA
      COMMON /MODENUM/ M

      DATA DR,DT/1.D-6,1.D-6/  !   JUST FOR NUMERICAL DIFFERENTIATION

      THETA0=A(31)

      RHO2=X**2+Y**2
      RHO=DSQRT(RHO2)
      R=DSQRT(RHO2+Z**2)
      THETA=DATAN2(RHO,Z)
      PHI=DATAN2(Y,X)
C
C   MAKE THE DEFORMATION OF COORDINATES:
C
       RS=R_S04(A,R,THETA)
       THETAS=THETA_S04(A,R,THETA)
       PHIS=PHI
C
C   CALCULATE FIELD COMPONENTS AT THE NEW POSITION (ASTERISKED):
C
       CALL FIALCOS04 (RS,THETAS,PHIS,BTAST,BFAST,M,THETA0,DTHETA)    !   MODE #M
C
C   NOW TRANSFORM B{R,T,F}_AST BY THE DEFORMATION TENSOR:
C
C      FIRST OF ALL, FIND THE DERIVATIVES:
C
       DRSDR=(R_S04(A,R+DR,THETA)-R_S04(A,R-DR,THETA))/(2.D0*DR)
       DRSDT=(R_S04(A,R,THETA+DT)-R_S04(A,R,THETA-DT))/(2.D0*DT)
       DTSDR=(THETA_S04(A,R+DR,THETA)-THETA_S04(A,R-DR,THETA))/(2.D0*DR)
       DTSDT=(THETA_S04(A,R,THETA+DT)-THETA_S04(A,R,THETA-DT))/(2.D0*DT)

       STSST=DSIN(THETAS)/DSIN(THETA)
       RSR=RS/R

       BR     =-RSR/R*STSST*BTAST*DRSDT
       BTHETA = RSR*STSST*BTAST*DRSDR
       BPHI   = RSR*BFAST*(DRSDR*DTSDT-DRSDT*DTSDR)

       S=RHO/R
       C=Z/R
       SF=Y/RHO
       CF=X/RHO

       BE=BR*S+BTHETA*C

       BX=A(1)*(BE*CF-BPHI*SF)
       BY=A(1)*(BE*SF+BPHI*CF)
       BZ=A(1)*(BR*C-BTHETA*S)

       RETURN
       END
C
C=====================================================================================
      DOUBLE PRECISION FUNCTION R_S04(A,R,THETA)
      IMPLICIT REAL*8 (A-H,O-Z)
      DIMENSION A(31)
C
      R_S04=R+A(2)/R+A(3)*R/DSQRT(R**2+A(11)**2)+A(4)*R/(R**2+A(12)**2)
     *+(A(5)+A(6)/R+A(7)*R/DSQRT(R**2+A(13)**2)+A(8)*R/(R**2+A(14)**2))*
     * DCOS(THETA)
     *+(A(9)*R/DSQRT(R**2+A(15)**2)+A(10)*R/(R**2+A(16)**2)**2)
     * *DCOS(2.D0*THETA)
C
      RETURN
      END
C
C-----------------------------------------------------------------------------
C
      DOUBLE PRECISION FUNCTION THETA_S04(A,R,THETA)
      IMPLICIT REAL*8 (A-H,O-Z)
      DIMENSION A(31)
c
      THETA_S04=THETA+(A(17)+A(18)/R+A(19)/R**2
     *                +A(20)*R/DSQRT(R**2+A(27)**2))*DSIN(THETA)
     * +(A(21)+A(22)*R/DSQRT(R**2+A(28)**2)
     *                +A(23)*R/(R**2+A(29)**2))*DSIN(2.D0*THETA)
     * +(A(24)+A(25)/R+A(26)*R/(R**2+A(30)**2))*DSIN(3.D0*THETA)
C
      RETURN
      END
C
c!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!
c
      SUBROUTINE FIALCOS04(R,THETA,PHI,BTHETA,BPHI,N,THETA0,DT)
C
C  CONICAL MODEL OF BIRKELAND CURRENT FIELD; BASED ON THE OLD S/R FIALCO (OF 1990-91)

C  BTN, AND BPN ARE THE ARRAYS OF BTHETA AND BPHI (BTN(i), BPN(i) CORRESPOND TO i-th MODE).
C   ONLY FIRST  N  MODE AMPLITUDES ARE COMPUTED (N<=10).
C    THETA0 IS THE ANGULAR HALF-WIDTH OF THE CONE, DT IS THE ANGULAR H.-W. OF THE CURRENT LAYER

C   NOTE:  BR=0  (BECAUSE ONLY RADIAL CURRENTS ARE PRESENT IN THIS MODEL)
C
      IMPLICIT REAL*8 (A-H,O-Z)
      DIMENSION  BTN(10),BPN(10),CCOS(10),SSIN(10)

      SINTE=DSIN(THETA)
      RO=R*SINTE
      COSTE=DCOS(THETA)
      SINFI=DSIN(PHI)
      COSFI=DCOS(PHI)
      TG=SINTE/(1.D0+COSTE)   !        TAN(THETA/2)
      CTG=SINTE/(1.D0-COSTE)  !        CTG(THETA/2)
C
C
      TETANP=THETA0+DT
      TETANM=THETA0-DT
      IF(THETA.LT.TETANM) GOTO 1
      TGP=DTAN(TETANP*0.5D0)
      TGM=DTAN(TETANM*0.5D0)
      TGM2=TGM*TGM
      TGP2=TGP*TGP
  1   CONTINUE

      COSM1=1.D0
      SINM1=0.D0
      TM=1.D0
      TGM2M=1.D0
      TGP2M=1.D0

      DO 2 M=1,N
      TM=TM*TG
      CCOS(M)=COSM1*COSFI-SINM1*SINFI
      SSIN(M)=SINM1*COSFI+COSM1*SINFI
      COSM1=CCOS(M)
      SINM1=SSIN(M)
      IF(THETA.LT.TETANM) THEN
      T=TM
      DTT=0.5D0*M*TM*(TG+CTG)
      DTT0=0.D0
      ELSE IF(THETA.LT.TETANP) THEN
      TGM2M=TGM2M*TGM2
      FC=1.D0/(TGP-TGM)
      FC1=1.D0/(2*M+1)
      TGM2M1=TGM2M*TGM
      TG21=1.D0+TG*TG
      T=FC*(TM*(TGP-TG)+FC1*(TM*TG-TGM2M1/TM))
      DTT=0.5D0*M*FC*TG21*(TM/TG*(TGP-TG)-FC1*(TM-TGM2M1/(TM*TG)))
      DTT0=0.5D0*FC*((TGP+TGM)*(TM*TG-FC1*(TM*TG-TGM2M1/TM))+
     * TM*(1.D0-TGP*TGM)-(1.D0+TGM2)*TGM2M/TM)
      ELSE
      TGP2M=TGP2M*TGP2
      TGM2M=TGM2M*TGM2
      FC=1.D0/(TGP-TGM)
      FC1=1.D0/(2*M+1)
      T=FC*FC1*(TGP2M*TGP-TGM2M*TGM)/TM
      DTT=-T*M*0.5D0*(TG+CTG)
      ENDIF

      BTN(M)=M*T*CCOS(M)/RO
  2   BPN(M)=-DTT*SSIN(M)/R

      BTHETA=BTN(N) *800.
      BPHI  =BPN(N) *800.

      RETURN
      END
C
C-------------------------------------------------------------------------
C
C
         SUBROUTINE BIRK_SHL04 (A,PS,X_SC,X,Y,Z,BX,BY,BZ)
C
         IMPLICIT  REAL * 8  (A - H, O - Z)
         DIMENSION A(86)
C
         CPS=DCOS(PS)
         SPS=DSIN(PS)

         S3PS=2.D0*CPS
C
         PST1=PS*A(85)
         PST2=PS*A(86)

         ST1=DSIN(PST1)
         CT1=DCOS(PST1)
         ST2=DSIN(PST2)
         CT2=DCOS(PST2)

         X1=X*CT1-Z*ST1
         Z1=X*ST1+Z*CT1
         X2=X*CT2-Z*ST2
         Z2=X*ST2+Z*CT2
C
         L=0
         GX=0.D0
         GY=0.D0
         GZ=0.D0
C
         DO 1 M=1,2     !    M=1 IS FOR THE 1ST SUM ("PERP." SYMMETRY)
C                          AND M=2 IS FOR THE SECOND SUM ("PARALL." SYMMETRY)
             DO 2 I=1,3
                  P=A(72+I)
                  Q=A(78+I)
                  CYPI=DCOS(Y/P)
                  CYQI=DCOS(Y/Q)
                  SYPI=DSIN(Y/P)
                  SYQI=DSIN(Y/Q)
C
                DO 3 K=1,3
                   R=A(75+K)
                   S=A(81+K)
                   SZRK=DSIN(Z1/R)
                   CZSK=DCOS(Z2/S)
                   CZRK=DCOS(Z1/R)
                   SZSK=DSIN(Z2/S)
                     SQPR=DSQRT(1.D0/P**2+1.D0/R**2)
                     SQQS=DSQRT(1.D0/Q**2+1.D0/S**2)
                        EPR=DEXP(X1*SQPR)
                        EQS=DEXP(X2*SQQS)
C
                  DO 4 N=1,2  ! N=1 IS FOR THE FIRST PART OF EACH COEFFICIENT
C                                AND N=2 IS FOR THE SECOND ONE

                    DO 5 NN=1,2 !   NN = 1,2 FURTHER SPLITS THE COEFFICIENTS INTO 2 PARTS,
C                                         TO TAKE INTO ACCOUNT THE SCALE FACTOR DEPENDENCE

                    IF (M.EQ.1) THEN
                         FX=-SQPR*EPR*CYPI*SZRK
                         FY=EPR*SYPI*SZRK/P
                         FZ=-EPR*CYPI*CZRK/R
                       IF (N.EQ.1) THEN
                         IF (NN.EQ.1) THEN
                          HX=FX
                          HY=FY
                          HZ=FZ
                         ELSE
                          HX=FX*X_SC
                          HY=FY*X_SC
                          HZ=FZ*X_SC
                         ENDIF
                       ELSE
                         IF (NN.EQ.1) THEN
                          HX=FX*CPS
                          HY=FY*CPS
                          HZ=FZ*CPS
                         ELSE
                          HX=FX*CPS*X_SC
                          HY=FY*CPS*X_SC
                          HZ=FZ*CPS*X_SC
                         ENDIF
                       ENDIF

                     ELSE                            !   M.EQ.2
                         FX=-SPS*SQQS*EQS*CYQI*CZSK
                         FY=SPS/Q*EQS*SYQI*CZSK
                         FZ=SPS/S*EQS*CYQI*SZSK
                       IF (N.EQ.1) THEN
                        IF (NN.EQ.1) THEN
                          HX=FX
                          HY=FY
                          HZ=FZ
                        ELSE
                          HX=FX*X_SC
                          HY=FY*X_SC
                          HZ=FZ*X_SC
                        ENDIF
                       ELSE
                        IF (NN.EQ.1) THEN
                         HX=FX*S3PS
                         HY=FY*S3PS
                         HZ=FZ*S3PS
                        ELSE
                         HX=FX*S3PS*X_SC
                         HY=FY*S3PS*X_SC
                         HZ=FZ*S3PS*X_SC
                        ENDIF
                       ENDIF
                  ENDIF
       L=L+1

       IF (M.EQ.1) THEN
       HXR=HX*CT1+HZ*ST1
       HZR=-HX*ST1+HZ*CT1
       ELSE
       HXR=HX*CT2+HZ*ST2
       HZR=-HX*ST2+HZ*CT2
       ENDIF

       GX=GX+HXR*A(L)
       GY=GY+HY *A(L)
  5    GZ=GZ+HZR*A(L)

  4   CONTINUE
  3   CONTINUE
  2   CONTINUE
  1   CONTINUE

      BX=GX
      BY=GY
      BZ=GZ

      RETURN
      END

C
C************************************************************************************
C
      SUBROUTINE FULL_RC04 (IOPR,PS,X,Y,Z,BXSRC,BYSRC,BZSRC,BXPRC,BYPRC,
     *  BZPRC)
C
C   CALCULATES GSM FIELD COMPONENTS OF THE SYMMETRIC (SRC) AND PARTIAL (PRC) COMPONENTS OF THE RING CURRENT
C   SRC  PROVIDES A DEPRESSION OF -28 nT AT EARTH
C   PRC  CORRESPONDS TO THE PRESSURE DIFFERENCE OF 2 nPa BETWEEN MIDNIGHT AND NOON RING CURRENT
C             PARTICLE PRESSURE AND YIELDS A DEPRESSION OF -17 nT AT X=-6Re
C
C   SC_SY AND SC_PR ARE SCALING FACTORS FOR THE SYMMETRIC AND PARTIAL COMPONENTS:
C          VALUES LARGER THAN 1 RESULT IN SPATIALLY LARGER CURRENTS
C
C   PHI IS THE ROTATION ANGLE IN RADIANS OF THE PARTIAL RING CURRENT (MEASURED FROM MIDNIGHT TOWARD DUSK)
C
C     IOPR -  A RING CURRENT CALCULATION FLAG (FOR LEAST-SQUARES FITTING ONLY):
C             IOPR=0 - BOTH SRC AND PRC FIELDS ARE CALCULATED
C             IOPR=1 - SRC ONLY
C             IOPR=2 - PRC ONLY
C
        IMPLICIT REAL*8 (A-H,O-Z)
        DIMENSION C_SY(86),C_PR(86)
        COMMON /RCPAR/ SC_SY,SC_PR,PHI
C
        DATA C_SY/1675.694858,1780.006388,-961.6082149,-1668.914259,
     *-27.40437029,-107.4169670,27.76189943,92.89740503,-43.92949274,
     *-403.6444072,6.167161865,298.2779761,-1680.779044,-1780.933039,
     *964.1861088,1670.988659,27.48864650,107.7809519,-27.84600972,
     *-93.20691865,44.28496784,404.4537249,-6.281958730,-298.6050952,
     *-7.971914848,2.017383761,-1.492230168,-1.957411655,-.08525523181,
     *-.3811813235,.08446716725,.3215044399,-.7141912767,-.9086294596,
     *.2966677742,-.04736679933,-11.38731325,.1719795189,1.356233066,
     *.8613438429,-.09143823092,-.2593979098,.04244838338,.06318383319,
     *-.5861372726,-.03368780733,-.07104470269,-.06909052953,
     *-60.18659631,-32.87563877,11.76450433,5.891673644,2.562360333,
     *6.215377232,-1.273945165,-1.864704763,-5.394837143,-8.799382627,
     *3.743066561,-.7649164511,57.09210569,32.61236511,-11.28688017,
     *-5.849523392,-2.470635922,-5.961417272,1.230031099,1.793192595,
     *5.383736074,8.369895153,-3.611544412,.7898988697,7.970609948,
     *7.981216562,35.16822497,12.45651654,1.689755359,3.678712366,
     *23.66117284,6.987136092,6.886678677,20.91245928,1.650064156,
     *3.474068566,.3474715765,.6564043111/

        DATA C_PR/-64820.58481,-63965.62048,66267.93413,135049.7504,
     *-36.56316878,124.6614669,56.75637955,-87.56841077,5848.631425,
     *4981.097722,-6233.712207,-10986.40188,68716.52057,65682.69473,
     *-69673.32198,-138829.3568,43.45817708,-117.9565488,-62.14836263,
     *79.83651604,-6211.451069,-5151.633113,6544.481271,11353.03491,
     *23.72352603,-256.4846331,25.77629189,145.2377187,-4.472639098,
     *-3.554312754,2.936973114,2.682302576,2.728979958,26.43396781,
     *-9.312348296,-29.65427726,-247.5855336,-206.9111326,74.25277664,
     *106.4069993,15.45391072,16.35943569,-5.965177750,-6.079451700,
     *115.6748385,-35.27377307,-32.28763497,-32.53122151,93.74409310,
     *84.25677504,-29.23010465,-43.79485175,-6.434679514,-6.620247951,
     *2.443524317,2.266538956,-43.82903825,6.904117876,12.24289401,
     *17.62014361,152.3078796,124.5505289,-44.58690290,-63.02382410,
     *-8.999368955,-9.693774119,3.510930306,3.770949738,-77.96705716,
     *22.07730961,20.46491655,18.67728847,9.451290614,9.313661792,
     *644.7620970,418.2515954,7.183754387,35.62128817,19.43180682,
     *39.57218411,15.69384715,7.123215241,2.300635346,21.90881131,
     *-.01775839370,.3996346710/

        CALL SRC_PRC04 (IOPR,SC_SY,SC_PR,PHI,PS,X,Y,Z,HXSRC,HYSRC,HZSRC,
     *      HXPRC,HYPRC,HZPRC)

        X_SC=SC_SY-1.D0
        IF (IOPR.EQ.0.OR.IOPR.EQ.1) THEN
          CALL RC_SHIELD04 (C_SY,PS,X_SC,X,Y,Z,FSX,FSY,FSZ)
        ELSE
           FSX=0.D0
           FSY=0.D0
           FSZ=0.D0
        ENDIF

        X_SC=SC_PR-1.D0
        IF (IOPR.EQ.0.OR.IOPR.EQ.2) THEN
          CALL RC_SHIELD04 (C_PR,PS,X_SC,X,Y,Z,FPX,FPY,FPZ)
        ELSE
           FPX=0.D0
           FPY=0.D0
           FPZ=0.D0
        ENDIF

        BXSRC=HXSRC+FSX
        BYSRC=HYSRC+FSY
        BZSRC=HZSRC+FSZ

        BXPRC=HXPRC+FPX
        BYPRC=HYPRC+FPY
        BZPRC=HZPRC+FPZ

        RETURN
        END
C---------------------------------------------------------------------------------------
C
       SUBROUTINE SRC_PRC04 (IOPR,SC_SY,SC_PR,PHI,PS,X,Y,Z,BXSRC,BYSRC,
     *    BZSRC,BXPRC,BYPRC,BZPRC)
C
C   RETURNS FIELD COMPONENTS FROM A MODEL RING CURRENT, INCLUDING ITS SYMMETRIC PART
C     AND A PARTIAL RING CURRENT, CLOSED VIA BIRKELAND CURRENTS. BASED ON RESULTS, DESCRIBED
C     IN A PAPER "MODELING THE INNER MAGNETOSPHERE: ASYMMETRIC RING CURRENT AND REGION 2
C     BIRKELAND CURRENTS REVISITED" (JGR, DEC.2000).
C
C     IOPR -  A RING CURRENT CALCULATION FLAG (FOR LEAST-SQUARES FITTING ONLY):
C             IOPR=0 - BOTH SRC AND PRC FIELDS ARE CALCULATED
C             IOPR=1 - SRC ONLY
C             IOPR=2 - PRC ONLY
C
C     SC_SY &  SC_PR ARE SCALE FACTORS FOR THE ABOVE COMPONENTS;  TAKING SC<1 OR SC>1 MAKES THE CURRENTS
C                      SHRINK OR EXPAND, RESPECTIVELY.
C
C   PHI IS THE ROTATION ANGLE (RADIANS) OF THE PARTIAL RING CURRENT (MEASURED FROM MIDNIGHT TOWARD DUSK)
C
        IMPLICIT REAL*8 (A-H,O-Z)
c
c   1.  TRANSFORM TO TILTED COORDINATES (i.e., SM coordinates):
C
        CPS=DCOS(PS)
        SPS=DSIN(PS)

        XT=X*CPS-Z*SPS
        ZT=Z*CPS+X*SPS
C
C   2.  SCALE THE COORDINATES FOR THE SYMMETRIC AND PARTIAL RC COMPONENTS:
C
        XTS=XT/SC_SY    !  SYMMETRIC
        YTS=Y /SC_SY
        ZTS=ZT/SC_SY

        XTA=XT/SC_PR    !  PARTIAL
        YTA=Y /SC_PR
        ZTA=ZT/SC_PR
C
C   3.  CALCULATE COMPONENTS OF THE TOTAL FIELD IN THE TILTED (SOLAR-MAGNETIC) COORDINATE SYSTEM:
C
C
C    3a. SYMMETRIC FIELD:
C
        IF (IOPR.LE.1) CALL RC_SYMM04(XTS,YTS,ZTS,BXS,BYS,BZS)
        IF (IOPR.EQ.0.OR.IOPR.EQ.2)
     *                 CALL PRC_SYMM04(XTA,YTA,ZTA,BXA_S,BYA_S,BZA_S)

C    3b. ROTATE THE SCALED SM COORDINATES BY PHI AROUND ZSM AXIS AND CALCULATE QUADRUPOLE PRC FIELD
C         IN THOSE COORDS:

        CP=DCOS(PHI)
        SP=DSIN(PHI)
        XR=XTA*CP-YTA*SP
        YR=XTA*SP+YTA*CP

        IF (IOPR.EQ.0.OR.IOPR.EQ.2)
     *                 CALL PRC_QUAD04(XR,YR,ZTA,BXA_QR,BYA_QR,BZA_Q)

C    3c. TRANSFORM THE QUADRUPOLE FIELD COMPONENTS BACK TO THE SM COORDS:
C
        BXA_Q= BXA_QR*CP+BYA_QR*SP
        BYA_Q=-BXA_QR*SP+BYA_QR*CP

C    3d. FIND THE TOTAL FIELD OF PRC (SYMM.+QUADR.) IN THE SM COORDS:
C
        BXP=BXA_S+BXA_Q
        BYP=BYA_S+BYA_Q
        BZP=BZA_S+BZA_Q
C
C   4.  TRANSFORM THE FIELDS OF BOTH PARTS OF THE RING CURRENT BACK TO THE GSM SYSTEM:
C
        BXSRC=BXS*CPS+BZS*SPS   !    SYMMETRIC RC
        BYSRC=BYS
        BZSRC=BZS*CPS-BXS*SPS
C
        BXPRC=BXP*CPS+BZP*SPS   !    PARTIAL RC
        BYPRC=BYP
        BZPRC=BZP*CPS-BXP*SPS
C
        RETURN
        END
C
C&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&
C
      SUBROUTINE RC_SYMM04 (X,Y,Z,BX,BY,BZ)
      IMPLICIT  REAL * 8  (A - H, O - Z)
      DATA DS,DC/1.D-2,0.99994999875D0/, D/1.D-4/,DRD/5.D3/  ! DS=SIN(THETA) AT THE BOUNDARY OF THE LINEARITY
                                                                        REGION; DC=DSQRT(1-DS**2);  DRD=1/(2*D)
      RHO2=X**2+Y**2
      R2=RHO2+Z**2
      R=DSQRT(R2)
      RP=R+D
      RM=R-D
      SINT=DSQRT(RHO2)/R
      COST=Z/R

      IF (SINT.LT.DS) THEN  !  TOO CLOSE TO THE Z-AXIS; USING A LINEAR APPROXIMATION A_PHI~SINT,
C                                    TO AVOID THE SINGULARITY PROBLEM
        A=AP(R,DS,DC)/DS
        DARDR=(RP*AP(RP,DS,DC)-RM*AP(RM,DS,DC))*DRD
        FXY=Z*(2.D0*A-DARDR)/(R*R2)
        BX=FXY*X
        BY=FXY*Y
        BZ=(2.D0*A*COST**2+DARDR*SINT**2)/R

       ELSE

        THETA=DATAN2(SINT,COST)
        TP=THETA+D
        TM=THETA-D
        SINTP=DSIN(TP)
        SINTM=DSIN(TM)
        COSTP=DCOS(TP)
        COSTM=DCOS(TM)
        BR=(SINTP*AP(R,SINTP,COSTP)-SINTM*AP(R,SINTM,COSTM))
     *       /(R*SINT)*DRD
        BT=(RM*AP(RM,SINT,COST)-RP*AP(RP,SINT,COST))/R*DRD
        FXY=(BR+BT*COST/SINT)/R
        BX=FXY*X
        BY=FXY*Y
        BZ=BR*COST-BT*SINT

      ENDIF

      RETURN
      END
c
c&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&
C
      DOUBLE PRECISION FUNCTION AP(R,SINT,COST)
C
C      Calculates azimuthal component of the vector potential of the symmetric
c  part of the model ring current.
C
      IMPLICIT  REAL * 8  (A - H, O - Z)
      LOGICAL PROX   !  INDICATES WHETHER WE ARE TOO CLOSE TO THE AXIS OF SYMMETRY, WHERE THE INVERSION
C                                                             OF DIPOLAR COORDINATES BECOMES INACCURATE
      DATA A1,A2,RRC1,DD1,RRC2,DD2,P1,R1,DR1,DLA1,P2,R2,DR2,DLA2,P3,
     *R3,DR3/-563.3722359,425.0891691,4.150588549,2.266150226,
     * 3.334503403,3.079071195,.02602428295,8.937790598,3.327934895,
     *.4487061833,.09125832351,6.243029867,1.750145910,.4181957162,
     *.06106691992,2.079908581,.6828548533/

      PROX=.FALSE.
      SINT1=SINT
      COST1=COST
      IF (SINT1.LT.1.D-2) THEN  !  TOO CLOSE TO Z-AXIS;  USE LINEAR INTERPOLATION BETWEEN SINT=0 & SINT=0.01
        SINT1=1.D-2
        COST1=.99994999875
        PROX=.TRUE.
      ENDIF

         ALPHA=SINT1**2/R         !  R,THETA -> ALPHA,GAMMA
         GAMMA=COST1/R**2

         ARG1=-((R-R1)/DR1)**2-(COST1/DLA1)**2
         ARG2=-((R-R2)/DR2)**2-(COST1/DLA2)**2
         ARG3=-((R-R3)/DR3)**2

         IF (ARG1.LT.-500.D0) THEN        !   TO PREVENT "FLOATING UNDERFLOW" CRASHES
           DEXP1=0.D0
         ELSE
           DEXP1=DEXP(ARG1)
         ENDIF

         IF (ARG2.LT.-500.D0) THEN
           DEXP2=0.D0
         ELSE
           DEXP2=DEXP(ARG2)
         ENDIF

         IF (ARG3.LT.-500.D0) THEN
           DEXP3=0.D0
         ELSE
           DEXP3=DEXP(ARG3)
         ENDIF


         ALPHA_S=ALPHA*(1.D0+P1*DEXP1+P2*DEXP2+P3*DEXP3)     !  ALPHA -> ALPHA_S  (DEFORMED)

         GAMMA_S=GAMMA
         GAMMAS2=GAMMA_S**2


         ALSQH=ALPHA_S**2/2.D0            !  ALPHA_S,GAMMA_S -> RS,SINTS,COSTS
         F=64.D0/27.D0*GAMMAS2+ALSQH**2
         Q=(DSQRT(F)+ALSQH)**(1.D0/3.D0)
         C=Q-4.D0*GAMMAS2**(1.D0/3.D0)/(3.D0*Q)
         IF (C.LT.0.D0) C=0.D0
         G=DSQRT(C**2+4.D0*GAMMAS2**(1.D0/3.D0))
         RS=4.D0/((DSQRT(2.D0*G-C)+DSQRT(C))*(G+C))
         COSTS=GAMMA_S*RS**2
         SINTS=DSQRT(1.D0-COSTS**2)
         RHOS=RS*SINTS
         RHOS2=RHOS**2
         ZS=RS*COSTS
C
c  1st loop:

         P=(RRC1+RHOS)**2+ZS**2+DD1**2
         XK2=4.D0*RRC1*RHOS/P
         XK=DSQRT(XK2)
         XKRHO12=XK*DSQRT(RHOS)
C
      XK2S=1.D0-XK2
      DL=DLOG(1.D0/XK2S)
      ELK=1.38629436112d0+XK2S*(0.09666344259D0+XK2S*(0.03590092383+
     *     XK2S*(0.03742563713+XK2S*0.01451196212))) +DL*
     *     (0.5D0+XK2S*(0.12498593597D0+XK2S*(0.06880248576D0+
     *     XK2S*(0.03328355346D0+XK2S*0.00441787012D0))))
      ELE=1.D0+XK2S*(0.44325141463D0+XK2S*(0.0626060122D0+XK2S*
     *      (0.04757383546D0+XK2S*0.01736506451D0))) +DL*
     *     XK2S*(0.2499836831D0+XK2S*(0.09200180037D0+XK2S*
     *       (0.04069697526D0+XK2S*0.00526449639D0)))
C
      APHI1=((1.D0-XK2*0.5D0)*ELK-ELE)/XKRHO12
c
c  2nd loop:

         P=(RRC2+RHOS)**2+ZS**2+DD2**2
         XK2=4.D0*RRC2*RHOS/P
         XK=DSQRT(XK2)
         XKRHO12=XK*DSQRT(RHOS)
C
      XK2S=1.D0-XK2
      DL=DLOG(1.D0/XK2S)
      ELK=1.38629436112d0+XK2S*(0.09666344259D0+XK2S*(0.03590092383+
     *     XK2S*(0.03742563713+XK2S*0.01451196212))) +DL*
     *     (0.5D0+XK2S*(0.12498593597D0+XK2S*(0.06880248576D0+
     *     XK2S*(0.03328355346D0+XK2S*0.00441787012D0))))
      ELE=1.D0+XK2S*(0.44325141463D0+XK2S*(0.0626060122D0+XK2S*
     *      (0.04757383546D0+XK2S*0.01736506451D0))) +DL*
     *     XK2S*(0.2499836831D0+XK2S*(0.09200180037D0+XK2S*
     *       (0.04069697526D0+XK2S*0.00526449639D0)))
C
       APHI2=((1.D0-XK2*0.5D0)*ELK-ELE)/XKRHO12

       AP=A1*APHI1+A2*APHI2
       IF (PROX) AP=AP*SINT/SINT1   !   LINEAR INTERPOLATION, IF TOO CLOSE TO THE Z-AXIS
C
       RETURN
       END
c
c@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@
C
      SUBROUTINE PRC_SYMM04 (X,Y,Z,BX,BY,BZ)
      IMPLICIT  REAL * 8  (A - H, O - Z)
      DATA DS,DC/1.D-2,0.99994999875D0/, D/1.D-4/,DRD/5.D3/  ! DS=SIN(THETA) AT THE BOUNDARY OF THE LINEARITY
                                                                        REGION; DC=DSQRT(1-DS**2);  DRD=1/(2*D)
      RHO2=X**2+Y**2
      R2=RHO2+Z**2
      R=DSQRT(R2)
      RP=R+D
      RM=R-D
      SINT=DSQRT(RHO2)/R
      COST=Z/R

      IF (SINT.LT.DS) THEN  !  TOO CLOSE TO THE Z-AXIS; USING A LINEAR APPROXIMATION A_PHI~SINT,
C                                    TO AVOID THE SINGULARITY PROBLEM
        A=APPRC04(R,DS,DC)/DS
        DARDR=(RP*APPRC04(RP,DS,DC)-RM*APPRC04(RM,DS,DC))*DRD
        FXY=Z*(2.D0*A-DARDR)/(R*R2)
        BX=FXY*X
        BY=FXY*Y
        BZ=(2.D0*A*COST**2+DARDR*SINT**2)/R

       ELSE

        THETA=DATAN2(SINT,COST)
        TP=THETA+D
        TM=THETA-D
        SINTP=DSIN(TP)
        SINTM=DSIN(TM)
        COSTP=DCOS(TP)
        COSTM=DCOS(TM)
        BR=(SINTP*APPRC04(R,SINTP,COSTP)-SINTM*APPRC04(R,SINTM,COSTM))
     *       /(R*SINT)*DRD
        BT=(RM*APPRC04(RM,SINT,COST)-RP*APPRC04(RP,SINT,COST))/R*DRD
        FXY=(BR+BT*COST/SINT)/R
        BX=FXY*X
        BY=FXY*Y
        BZ=BR*COST-BT*SINT

      ENDIF

      RETURN
      END
c
c&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&
C
C
      DOUBLE PRECISION FUNCTION APPRC04(R,SINT,COST)
C
C      Calculates azimuthal component of the vector potential of the symmetric
c  part of the model PARTIAL ring current.
C
      IMPLICIT  REAL * 8  (A - H, O - Z)
      LOGICAL PROX
      DATA A1,A2,RRC1,DD1,RRC2,DD2,P1,ALPHA1,DAL1,BETA1,DG1,P2,ALPHA2,
     * DAL2,BETA2,DG2,BETA3,P3,ALPHA3,DAL3,BETA4,DG3,BETA5,Q0,Q1,ALPHA4,
     * DAL4,DG4,Q2,ALPHA5,DAL5,DG5,BETA6,BETA7
     * /-80.11202281,12.58246758,6.560486035,1.930711037,3.827208119,
     *.7789990504,.3058309043,.1817139853,.1257532909,3.422509402,
     *.04742939676,-4.800458958,-.02845643596,.2188114228,2.545944574,
     *.00813272793,.35868244,103.1601001,-.00764731187,.1046487459,
     *2.958863546,.01172314188,.4382872938,.01134908150,14.51339943,
     *.2647095287,.07091230197,.01512963586,6.861329631,.1677400816,
     *.04433648846,.05553741389,.7665599464,.7277854652/

      PROX=.FALSE.
      SINT1=SINT
      COST1=COST
      IF (SINT1.LT.1.D-2) THEN  !  TOO CLOSE TO Z-AXIS;  USE LINEAR INTERPOLATION BETWEEN SINT=0 & SINT=0.01
        SINT1=1.D-2
        COST1=.99994999875
        PROX=.TRUE.
      ENDIF

         ALPHA=SINT1**2/R         !  R,THETA -> ALPHA,GAMMA
         GAMMA=COST1/R**2

         ARG1=-(GAMMA/DG1)**2
         ARG2=-((ALPHA-ALPHA4)/DAL4)**2-(GAMMA/DG4)**2

         IF (ARG1.LT.-500.D0) THEN        !   TO PREVENT "FLOATING UNDERFLOW" CRASHES
           DEXP1=0.D0
         ELSE
           DEXP1=DEXP(ARG1)
         ENDIF

         IF (ARG2.LT.-500.D0) THEN        !   TO PREVENT "FLOATING UNDERFLOW" CRASHES
           DEXP2=0.D0
         ELSE
           DEXP2=DEXP(ARG2)
         ENDIF

         ALPHA_S=ALPHA*(1.D0+P1/(1.D0+((ALPHA-ALPHA1)/DAL1)**2)**BETA1
     * *DEXP1+P2*(ALPHA-ALPHA2)/(1.D0+((ALPHA-ALPHA2)/DAL2)**2)**BETA2
     */(1.D0+(GAMMA/DG2)**2)**BETA3
     *+P3*(ALPHA-ALPHA3)**2/(1.D0+((ALPHA-ALPHA3)/DAL3)**2)**BETA4
     */(1.D0+(GAMMA/DG3)**2)**BETA5)     !  ALPHA -> ALPHA_S  (DEFORMED)

         GAMMA_S=GAMMA*(1.D0+Q0+Q1*(ALPHA-ALPHA4)*DEXP2              !  GAMMA -> GAMMA_  (DEFORMED)
     * +Q2*(ALPHA-ALPHA5)/(1.D0+((ALPHA-ALPHA5)/DAL5)**2)**BETA6
     * /(1.D0+(GAMMA/DG5)**2)**BETA7)

         GAMMAS2=GAMMA_S**2

         ALSQH=ALPHA_S**2/2.D0                            !  ALPHA_S,GAMMA_S -> RS,SINTS,COSTS
         F=64.D0/27.D0*GAMMAS2+ALSQH**2
         Q=(DSQRT(F)+ALSQH)**(1.D0/3.D0)
         C=Q-4.D0*GAMMAS2**(1.D0/3.D0)/(3.D0*Q)
         IF (C.LT.0.D0) C=0.D0
         G=DSQRT(C**2+4.D0*GAMMAS2**(1.D0/3.D0))
         RS=4.D0/((DSQRT(2.D0*G-C)+DSQRT(C))*(G+C))
         COSTS=GAMMA_S*RS**2
         SINTS=DSQRT(1.D0-COSTS**2)
         RHOS=RS*SINTS
         RHOS2=RHOS**2
         ZS=RS*COSTS
C
c  1st loop:

         P=(RRC1+RHOS)**2+ZS**2+DD1**2
         XK2=4.D0*RRC1*RHOS/P
         XK=DSQRT(XK2)
         XKRHO12=XK*DSQRT(RHOS)
C
      XK2S=1.D0-XK2
      DL=DLOG(1.D0/XK2S)
      ELK=1.38629436112d0+XK2S*(0.09666344259D0+XK2S*(0.03590092383+
     *     XK2S*(0.03742563713+XK2S*0.01451196212))) +DL*
     *     (0.5D0+XK2S*(0.12498593597D0+XK2S*(0.06880248576D0+
     *     XK2S*(0.03328355346D0+XK2S*0.00441787012D0))))
      ELE=1.D0+XK2S*(0.44325141463D0+XK2S*(0.0626060122D0+XK2S*
     *      (0.04757383546D0+XK2S*0.01736506451D0))) +DL*
     *     XK2S*(0.2499836831D0+XK2S*(0.09200180037D0+XK2S*
     *       (0.04069697526D0+XK2S*0.00526449639D0)))
C
      APHI1=((1.D0-XK2*0.5D0)*ELK-ELE)/XKRHO12
c
c  2nd loop:

         P=(RRC2+RHOS)**2+ZS**2+DD2**2
         XK2=4.D0*RRC2*RHOS/P
         XK=DSQRT(XK2)
         XKRHO12=XK*DSQRT(RHOS)
C
      XK2S=1.D0-XK2
      DL=DLOG(1.D0/XK2S)
      ELK=1.38629436112d0+XK2S*(0.09666344259D0+XK2S*(0.03590092383+
     *     XK2S*(0.03742563713+XK2S*0.01451196212))) +DL*
     *     (0.5D0+XK2S*(0.12498593597D0+XK2S*(0.06880248576D0+
     *     XK2S*(0.03328355346D0+XK2S*0.00441787012D0))))
      ELE=1.D0+XK2S*(0.44325141463D0+XK2S*(0.0626060122D0+XK2S*
     *      (0.04757383546D0+XK2S*0.01736506451D0))) +DL*
     *     XK2S*(0.2499836831D0+XK2S*(0.09200180037D0+XK2S*
     *       (0.04069697526D0+XK2S*0.00526449639D0)))
C
      APHI2=((1.D0-XK2*0.5D0)*ELK-ELE)/XKRHO12

      APPRC04=A1*APHI1+A2*APHI2
      IF (PROX) APPRC04=APPRC04*SINT/SINT1   !   LINEAR INTERPOLATION, IF TOO CLOSE TO THE Z-AXIS
C
      RETURN
      END
C
C@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@
C
C
         SUBROUTINE PRC_QUAD04 (X,Y,Z,BX,BY,BZ)
C
         IMPLICIT  REAL * 8  (A - H, O - Z)

         DATA D,DD/1.D-4,2.D-4/, DS/1.D-2/,DC/0.99994999875D0/

         RHO2=X**2+Y**2
         R=DSQRT(RHO2+Z**2)
         RHO=DSQRT(RHO2)
         SINT=RHO/R
         COST=Z/R
         RP=R+D
         RM=R-D

         IF (SINT.GT.DS) THEN
           CPHI=X/RHO
           SPHI=Y/RHO
           BR=BR_PRC_Q04(R,SINT,COST)
           BT=BT_PRC_Q04(R,SINT,COST)
           DBRR=(BR_PRC_Q04(RP,SINT,COST)-BR_PRC_Q04(RM,SINT,COST))/DD
           THETA=DATAN2(SINT,COST)
           TP=THETA+D
           TM=THETA-D
           SINTP=DSIN(TP)
           COSTP=DCOS(TP)
           SINTM=DSIN(TM)
           COSTM=DCOS(TM)
           DBTT=(BT_PRC_Q04(R,SINTP,COSTP)-BT_PRC_Q04(R,SINTM,COSTM))/DD
           BX=SINT*(BR+(BR+R*DBRR+DBTT)*SPHI**2)+COST*BT
           BY=-SINT*SPHI*CPHI*(BR+R*DBRR+DBTT)
           BZ=(BR*COST-BT*SINT)*CPHI
         ELSE
           ST=DS
           CT=DC
           IF (Z.LT.0.D0) CT=-DC
           THETA=DATAN2(ST,CT)
           TP=THETA+D
           TM=THETA-D
           SINTP=DSIN(TP)
           COSTP=DCOS(TP)
           SINTM=DSIN(TM)
           COSTM=DCOS(TM)
           BR=BR_PRC_Q04(R,ST,CT)
           BT=BT_PRC_Q04(R,ST,CT)
           DBRR=(BR_PRC_Q04(RP,ST,CT)-BR_PRC_Q04(RM,ST,CT))/DD
           DBTT=(BT_PRC_Q04(R,SINTP,COSTP)-BT_PRC_Q04(R,SINTM,COSTM))/DD
           FCXY=R*DBRR+DBTT
           BX=(BR*(X**2+2.D0*Y**2)+FCXY*Y**2)/(R*ST)**2+BT*COST
           BY=-(BR+FCXY)*X*Y/(R*ST)**2
           BZ=(BR*COST/ST-BT)*X/R
         ENDIF

         RETURN
         END
c
c&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&&
C
      DOUBLE PRECISION FUNCTION BR_PRC_Q04 (R,SINT,COST)
C
Calculates the radial component of the "quadrupole" part of the model partial ring current.
C
      IMPLICIT  REAL * 8  (A - H, O - Z)

      DATA A1,A2,A3,A4,A5,A6,A7,A8,A9,A10,A11,A12,A13,A14,A15,A16,A17,   ! ALL LINEAR PARAMETERS HERE
     * A18,XK1,AL1,DAL1,B1,BE1,XK2,AL2,DAL2,B2,BE2,XK3,XK4,AL3,DAL3,B3,  ! WERE MULTIPLIED BY 0.1,
     * BE3,AL4,DAL4,DG1,AL5,DAL5,DG2,C1,C2,C3,AL6,DAL6,DRM/-21.2666329,  ! SO THAT THEY CORRESPOND TO P_0=1 nPa,
     *32.24527521,-6.062894078,7.515660734,233.7341288,-227.1195714,     ! RATHER THAN THE ORIGINAL VALUE OF 10 nPa
     *8.483233889,16.80642754,-24.63534184,9.067120578,-1.052686913,     ! ASSUMED IN THE BIOT-SAVART INTEGRAL
     *-12.08384538,18.61969572,-12.71686069,47017.35679,-50646.71204,
     *7746.058231,1.531069371,2.318824273,.1417519429,.6388013110E-02,
     *5.303934488,4.213397467,.7955534018,.1401142771,.2306094179E-01,
     *3.462235072,2.568743010,3.477425908,1.922155110,.1485233485,
     *.2319676273E-01,7.830223587,8.492933868,.1295221828,.01753008801,
     *.01125504083,.1811846095,.04841237481,.01981805097,6.557801891,
     *6.348576071,5.744436687,.2265212965,.1301957209,.5654023158/

        SINT2=SINT**2
        COST2=COST**2
        SC=SINT*COST
        ALPHA=SINT2/R
        GAMMA=COST/R**2

        CALL FFS04(ALPHA,AL1,DAL1,F,FA,FS)
        D1=SC*F**XK1/((R/B1)**BE1+1.D0)
        D2=D1*COST2

        CALL FFS04(ALPHA,AL2,DAL2,F,FA,FS)
        D3=SC*FS**XK2/((R/B2)**BE2+1.D0)
        D4=D3*COST2

        CALL FFS04(ALPHA,AL3,DAL3,F,FA,FS)
        D5=SC*(ALPHA**XK3)*(FS**XK4)/((R/B3)**BE3+1.D0)
        D6=D5*COST2

        ARGA=((ALPHA-AL4)/DAL4)**2+1.D0
        ARGG=1.D0+(GAMMA/DG1)**2

        D7=SC/ARGA/ARGG
        D8=D7/ARGA
        D9=D8/ARGA
        D10=D9/ARGA

        ARGA=((ALPHA-AL5)/DAL5)**2+1.D0
        ARGG=1.D0+(GAMMA/DG2)**2

        D11=SC/ARGA/ARGG
        D12=D11/ARGA
        D13=D12/ARGA
        D14=D13/ARGA


        D15=SC/(R**4+C1**4)
        D16=SC/(R**4+C2**4)*COST2
        D17=SC/(R**4+C3**4)*COST2**2

        CALL FFS04(ALPHA,AL6,DAL6,F,FA,FS)
        D18=SC*FS/(1.D0+((R-1.2D0)/DRM)**2)

        BR_PRC_Q04=A1*D1+A2*D2+A3*D3+A4*D4+A5*D5+A6*D6+A7*D7+A8*D8+A9*D9+
     *  A10*D10+A11*D11+A12*D12+A13*D13+A14*D14+A15*D15+A16*D16+A17*D17+
     *   A18*D18
C
        RETURN
        END
c
C%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
C
        DOUBLE PRECISION FUNCTION BT_PRC_Q04 (R,SINT,COST)
C
Calculates the Theta component of the "quadrupole" part of the model partial ring current.
C
        IMPLICIT  REAL * 8  (A - H, O - Z)

      DATA A1,A2,A3,A4,A5,A6,A7,A8,A9,A10,A11,A12,A13,A14,A15,A16,A17,  ! ALL LINEAR PARAMETERS HERE
     *XK1,AL1,DAL1,B1,BE1,XK2,AL2,DAL2,BE2,XK3,XK4,AL3,DAL3,B3,BE3,AL4, ! WERE MULTIPLIED BY 0.1,
     *DAL4,DG1,AL5,DAL5,DG2,C1,C2,C3/12.74640393,-7.516393516,          ! SO THAT THEY CORRESPOND TO P_0=1 nPa,
     *-5.476233865,3.212704645,-59.10926169,46.62198189,-.01644280062,  ! RATHER THAN THE ORIGINAL VALUE OF 10 nPa
     *.1234229112,-.08579198697,.01321366966,.8970494003,9.136186247,   ! ASSUMED IN THE BIOT-SAVART INTEGRAL
     *-38.19301215,21.73775846,-410.0783424,-69.90832690,-848.8543440,
     *1.243288286,.2071721360,.05030555417,7.471332374,3.180533613,
     *1.376743507,.1568504222,.02092910682,1.985148197,.3157139940,
     *1.056309517,.1701395257,.1019870070,6.293740981,5.671824276,
     *.1280772299,.02189060799,.01040696080,.1648265607,.04701592613,
     *.01526400086,12.88384229,3.361775101,23.44173897/

        SINT2=SINT**2
        COST2=COST**2
        SC=SINT*COST
        ALPHA=SINT2/R
        GAMMA=COST/R**2

        CALL FFS04(ALPHA,AL1,DAL1,F,FA,FS)
        D1=F**XK1/((R/B1)**BE1+1.D0)
        D2=D1*COST2

        CALL FFS04(ALPHA,AL2,DAL2,F,FA,FS)
        D3=FA**XK2/R**BE2
        D4=D3*COST2

        CALL FFS04(ALPHA,AL3,DAL3,F,FA,FS)
        D5=FS**XK3*ALPHA**XK4/((R/B3)**BE3+1.D0)
        D6=D5*COST2

        CALL FFS04(GAMMA,0.D0,DG1,F,FA,FS)
        FCC=(1.D0+((ALPHA-AL4)/DAL4)**2)
        D7 =1.D0/FCC*FS
        D8 =D7/FCC
        D9 =D8/FCC
        D10=D9/FCC

        ARG=1.D0+((ALPHA-AL5)/DAL5)**2
        D11=1.D0/ARG/(1.D0+(GAMMA/DG2)**2)
        D12=D11/ARG
        D13=D12/ARG
        D14=D13/ARG

        D15=1.D0/(R**4+C1**2)
        D16=COST2/(R**4+C2**2)
        D17=COST2**2/(R**4+C3**2)
C
        BT_PRC_Q04=A1*D1+A2*D2+A3*D3+A4*D4+A5*D5+A6*D6+A7*D7+A8*D8+A9*D9+
     *   A10*D10+A11*D11+A12*D12+A13*D13+A14*D14+A15*D15+A16*D16+A17*D17
C
       RETURN
       END
c
c@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@

       SUBROUTINE FFS04(A,A0,DA,F,FA,FS)
       IMPLICIT  REAL * 8  (A - H, O - Z)
       SQ1=DSQRT((A+A0)**2+DA**2)
       SQ2=DSQRT((A-A0)**2+DA**2)
       FA=2.D0/(SQ1+SQ2)
       F=FA*A
       FS=0.5D0*(SQ1+SQ2)/(SQ1*SQ2)*(1.D0-F*F)
       RETURN
       END
C
C||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||
C
C-------------------------------------------------------------------------
C
C
         SUBROUTINE RC_SHIELD04 (A,PS,X_SC,X,Y,Z,BX,BY,BZ)
C
       IMPLICIT  REAL * 8  (A - H, O - Z)
         DIMENSION A(86)
C
         FAC_SC=(X_SC+1.D0)**3
C
         CPS=DCOS(PS)
         SPS=DSIN(PS)

         S3PS=2.D0*CPS
C
         PST1=PS*A(85)
         PST2=PS*A(86)

         ST1=DSIN(PST1)
         CT1=DCOS(PST1)
         ST2=DSIN(PST2)
         CT2=DCOS(PST2)

         X1=X*CT1-Z*ST1
         Z1=X*ST1+Z*CT1
         X2=X*CT2-Z*ST2
         Z2=X*ST2+Z*CT2
C
         L=0
         GX=0.D0
         GY=0.D0
         GZ=0.D0
C
         DO 1 M=1,2     !    M=1 IS FOR THE 1ST SUM ("PERP." SYMMETRY)
C                          AND M=2 IS FOR THE SECOND SUM ("PARALL." SYMMETRY)
             DO 2 I=1,3
                  P=A(72+I)
                  Q=A(78+I)
                  CYPI=DCOS(Y/P)
                  CYQI=DCOS(Y/Q)
                  SYPI=DSIN(Y/P)
                  SYQI=DSIN(Y/Q)
C
                DO 3 K=1,3
                   R=A(75+K)
                   S=A(81+K)
                   SZRK=DSIN(Z1/R)
                   CZSK=DCOS(Z2/S)
                   CZRK=DCOS(Z1/R)
                   SZSK=DSIN(Z2/S)
                     SQPR=DSQRT(1.D0/P**2+1.D0/R**2)
                     SQQS=DSQRT(1.D0/Q**2+1.D0/S**2)
                        EPR=DEXP(X1*SQPR)
                        EQS=DEXP(X2*SQQS)
C
                  DO 4 N=1,2  ! N=1 IS FOR THE FIRST PART OF EACH COEFFICIENT
C                                AND N=2 IS FOR THE SECOND ONE

                    DO 5 NN=1,2 !   NN = 1,2 FURTHER SPLITS THE COEFFICIENTS INTO 2 PARTS,
C                                         TO TAKE INTO ACCOUNT THE SCALE FACTOR DEPENDENCE

                    IF (M.EQ.1) THEN
                         FX=-SQPR*EPR*CYPI*SZRK  *FAC_SC
                         FY=EPR*SYPI*SZRK/P   *FAC_SC
                         FZ=-EPR*CYPI*CZRK/R  *FAC_SC
                       IF (N.EQ.1) THEN
                         IF (NN.EQ.1) THEN
                          HX=FX
                          HY=FY
                          HZ=FZ
                         ELSE
                          HX=FX*X_SC
                          HY=FY*X_SC
                          HZ=FZ*X_SC
                         ENDIF
                       ELSE
                         IF (NN.EQ.1) THEN
                          HX=FX*CPS
                          HY=FY*CPS
                          HZ=FZ*CPS
                         ELSE
                          HX=FX*CPS*X_SC
                          HY=FY*CPS*X_SC
                          HZ=FZ*CPS*X_SC
                         ENDIF
                       ENDIF

                     ELSE                            !   M.EQ.2
                         FX=-SPS*SQQS*EQS*CYQI*CZSK  *FAC_SC
                         FY=SPS/Q*EQS*SYQI*CZSK   *FAC_SC
                         FZ=SPS/S*EQS*CYQI*SZSK   *FAC_SC
                       IF (N.EQ.1) THEN
                        IF (NN.EQ.1) THEN
                          HX=FX
                          HY=FY
                          HZ=FZ
                        ELSE
                          HX=FX*X_SC
                          HY=FY*X_SC
                          HZ=FZ*X_SC
                        ENDIF
                       ELSE
                        IF (NN.EQ.1) THEN
                         HX=FX*S3PS
                         HY=FY*S3PS
                         HZ=FZ*S3PS
                        ELSE
                         HX=FX*S3PS*X_SC
                         HY=FY*S3PS*X_SC
                         HZ=FZ*S3PS*X_SC
                        ENDIF
                       ENDIF
                  ENDIF
       L=L+1

       IF (M.EQ.1) THEN
       HXR=HX*CT1+HZ*ST1
       HZR=-HX*ST1+HZ*CT1
       ELSE
       HXR=HX*CT2+HZ*ST2
       HZR=-HX*ST2+HZ*CT2
       ENDIF

       GX=GX+HXR*A(L)
       GY=GY+HY *A(L)
  5    GZ=GZ+HZR*A(L)

  4   CONTINUE
  3   CONTINUE
  2   CONTINUE
  1   CONTINUE

      BX=GX
      BY=GY
      BZ=GZ

      RETURN
      END
C
c===========================================================================
c
       SUBROUTINE DIPOLE04 (PS,X,Y,Z,BX,BY,BZ)
C
C      A DOUBLE PRECISION ROUTINE
C
C  CALCULATES GSM COMPONENTS OF A GEODIPOLE FIELD WITH THE DIPOLE MOMENT
C  CORRESPONDING TO THE EPOCH OF 2000.
C
C----INPUT PARAMETERS:
C     PS - GEODIPOLE TILT ANGLE IN RADIANS,
C     X,Y,Z - GSM COORDINATES IN RE (1 RE = 6371.2 km)
C
C----OUTPUT PARAMETERS:
C     BX,BY,BZ - FIELD COMPONENTS IN GSM SYSTEM, IN NANOTESLA.
C
      IMPLICIT REAL*8 (A-H,O-Z)
      SPS=DSIN(PS)
      CPS=DCOS(PS)
      P=X**2
      U=Z**2
      V=3.D0*Z*X
      T=Y**2
      Q=30115.D0/DSQRT(P+T+U)**5
      BX=Q*((T+U-2.D0*P)*SPS-V*CPS)
      BY=-3.D0*Y*Q*(X*SPS+Z*CPS)
      BZ=Q*((P+T-2.D0*U)*CPS-V*SPS)
      RETURN
      END



C
C(((((((((((((((((((((((((((((((((((((((((((((((((((((((((((((((((((((((((



