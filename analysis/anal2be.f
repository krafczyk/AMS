*  $Id: anal2be.f,v 1.4 2001/01/22 17:32:25 choutko Exp $
      REAL FUNCTION anal2be(imax,ibe)
*********************************************************
*                                                       *
* This file was generated by HUWFUN.                    *
*                                                       *
*********************************************************
*
*     Ntuple Id:      1    
*     Ntuple Title:   Converted Ntuple
*     Creation:       17/10/96 20.42.24
*
*********************************************************
*
      LOGICAL         CHAIN
      CHARACTER*128   CFILE
      INTEGER         IDNEVT,NCHEVT,ICHEVT
      REAL            OBS(13)
*
      COMMON /PAWIDN/ IDNEVT,OBS
      COMMON /PAWCHN/ CHAIN, NCHEVT, ICHEVT
      COMMON /PAWCHC/ CFILE
*
*
*--   Ntuple Variable Declarations
*

      REAL PolePhi,ThetaS,PhiS,beta(100),betaerror(100),betachi2(100)
     + ,betachi2s(100),probtof(7,10),probtracker(7,10),pmass(10)
     + ,perrmass(10),pmom(10),perrmom(10),pcharge(10),ptheta(10)
     + ,pphi(10),pcoo(3,10),signalctc(2,10),betactc(2,10)
     + ,errorbetactc(2,10),cooctc(3,2,10),cootof(3,4,10),TOFEdep(20)
     + ,TOFTime(20),TOFETime(20),TOFCoo(3,20),TOFErCoo(3,20)
     + ,TOFMCXcoo(3,200),TOFMCtof(200),TOFMCedep(200),Sumt(200)
     + ,Sigmat(200),Meant(200),RMSt(200),ErrorMeant(200)
     + ,Amplitude(5,200),ss(5,2,200),xca(3,200),xcb(3,200),xgl(3,200)
     + ,summc(200),hitr(3,500),ehitr(3,500),sumr(500),difosum(500)
     + ,Chi2StrLine(20),Chi2Circle(20),CircleRidgidity(20)
     + ,Chi2FastFit(20),Ridgidity(20),ErrRidgidity(20),Theta(20),phi(20)
     + ,p0(3,20),gchi2(20),gridgidity(20),gerrridgidity(20),gtheta(20)
     + ,gphi(20),gp0(3,20),hchi2(2,20),HRidgidity(2,20)
     + ,HErrRidgidity(2,20),htheta(2,20),hphi(2,20),hp0(3,2,20)
     + ,fchi2ms(20),gchi2ms(20),ridgidityms(20),gridgidityms(20)
     + ,coo(3,20),dir(3,20),momentum(20),mass(20),charge(20)
     + ,ctccoo(3,20),ctcercoo(3,20),ctcrawsignal(20),ctcsignal(20)
     + ,ctcesignal(20)
      INTEGER eventno,run,runtype,time(2),Particles,Tracks,Betas,Charges
     + ,TrRecHits,TrClusters,TrRawClusters,TrMCClusters,TOFClusters
     + ,TOFMCClusters,CTCClusters,CTCMCClusters,AntiMCClusters
     + ,AntiClusters,nbeta,betastatus(100),betapattern(100)
     + ,betantof(100),betaptof(4,100),betaptr(100),ncharge
     + ,chargestatus(10),chargebetap(10),chargetof(10),chargetracker(10)
     + ,npart,pctcp(2,10),pbetap(10),pchargep(10),ptrackp(10),pid(10)
     + ,ntof,TOFStatus(20),plane(20),bar(20),ntofmc,TOFMCIdsoft(200)
     + ,Ntrcl,Idsoft(200),Statust(200),NelemL(200),NelemR(200),ntrclmc
     + ,IdsoftMC(200),Itra(200),Left(2,200),Center(2,200),Right(2,200)
     + ,ntrrh,px(500),py(500),statusr(500),Layer(500),ntrtr,trstatus(20)
     + ,pattern(20),nhits(20),phits(6,20),FastFitDone(20)
     + ,GeaneFitDone(20),AdvancedFitDone(20),nmcg,nskip(20),Particle(20)
     + ,nctccl,CTCStatus(20),CTCLayer(20),nctcclmc,CTCMCIdsoft(200)
     + ,nanti,AntiStatus(16),AntiSector(16),nantimc,AntiMCIdsoft(200)
     + ,nlvl3,LVL3TOFTr(2),LVL3AntiTr(2),LVL3TrackerTr(2),LVL3NTrHits(2)
     + ,LVL3NPat(2),LVL3Pattern(2,2),nlvl1,LVL1Mode(2),LVL1Flag(2)
     + ,LVL1TOFPatt(4,2),LVL1TOFPatt1(4,2),LVL1AntiPatt(2),nctcht
     + ,CTChitStatus(50)
      REAL CTCMCXcoo(3,200),CTCMCXdir(3,200),CTCstep(200),ctccharge(200)
     + ,ctcbeta(200),ctcedep(200),AntiEdep(16),AntiCoo(3,16)
     + ,AntiErCoo(3,16),AntiMCXcoo(3,200),AntiMCtof(200),AntiMCedep(200)
     + ,LVL3Residual(2,2),LVL3Time(2),LVL3ELoss(2),ctchitsignal(50)
     + ,s2n(500),antirawsignal(32),tofrtovta(2,20),tofrtovtd(2,20)
     + ,tofrsdtm(2,20)
      INTEGER CTChitLayer(50),ctchitcolumn(50),ctchitrow(50),ntrraw
     + ,rawaddress(500),rawlength(500),nantiraw,antirawstatus(32)
     + ,antirawsector(32),antirawupdown(32),ntofraw,tofrstatus(20)
     + ,tofrplane(20),tofrbar(20)
*
      COMMON /PAWCR4/ eventno,run,runtype,time,PolePhi,ThetaS,PhiS
     + ,Particles,Tracks,Betas,Charges,TrRecHits,TrClusters
     + ,TrRawClusters,TrMCClusters,TOFClusters,TOFMCClusters,CTCClusters
     + ,CTCMCClusters,AntiMCClusters,AntiClusters,nbeta,betastatus
     + ,betapattern,beta,betaerror,betachi2,betachi2s,betantof,betaptof
     + ,betaptr,ncharge,chargestatus,chargebetap,chargetof,chargetracker
     + ,probtof,probtracker,npart,pctcp,pbetap,pchargep,ptrackp,pid
     + ,pmass,perrmass,pmom,perrmom,pcharge,ptheta,pphi,pcoo,signalctc
     + ,betactc,errorbetactc,cooctc,cootof,ntof,TOFStatus,plane,bar
     + ,TOFEdep,TOFTime,TOFETime,TOFCoo,TOFErCoo,ntofmc,TOFMCIdsoft
     + ,TOFMCXcoo,TOFMCtof,TOFMCedep,Ntrcl,Idsoft,Statust,NelemL,NelemR
     + ,Sumt,Sigmat,Meant,RMSt,ErrorMeant,Amplitude,ntrclmc,IdsoftMC
     + ,Itra,Left,Center,Right,ss,xca,xcb,xgl,summc,ntrrh,px,py,statusr
     + ,Layer,hitr,ehitr,sumr,difosum,ntrtr,trstatus,pattern,nhits,phits
     + ,FastFitDone,GeaneFitDone,AdvancedFitDone,Chi2StrLine,Chi2Circle
     + ,CircleRidgidity,Chi2FastFit,Ridgidity,ErrRidgidity,Theta,phi,p0
     + ,gchi2,gridgidity,gerrridgidity,gtheta,gphi,gp0,hchi2,HRidgidity
     + ,HErrRidgidity,htheta,hphi,hp0,fchi2ms,gchi2ms,ridgidityms
     + ,gridgidityms,nmcg,nskip,Particle,coo,dir,momentum,mass,charge
     + ,nctccl,CTCStatus,CTCLayer,ctccoo,ctcercoo,ctcrawsignal,ctcsignal
     + ,ctcesignal,nctcclmc,CTCMCIdsoft,CTCMCXcoo,CTCMCXdir,CTCstep
     + ,ctccharge,ctcbeta,ctcedep,nanti,AntiStatus,AntiSector,AntiEdep
     + ,AntiCoo,AntiErCoo,nantimc,AntiMCIdsoft,AntiMCXcoo,AntiMCtof
     + ,AntiMCedep,nlvl3,LVL3TOFTr,LVL3AntiTr,LVL3TrackerTr,LVL3NTrHits
     + ,LVL3NPat,LVL3Pattern,LVL3Residual,LVL3Time,LVL3ELoss,nlvl1
     + ,LVL1Mode,LVL1Flag,LVL1TOFPatt,LVL1TOFPatt1,LVL1AntiPatt,nctcht
     + ,CTChitStatus,CTChitLayer,ctchitcolumn,ctchitrow,ctchitsignal
     + ,ntrraw,rawaddress,rawlength,s2n,nantiraw,antirawstatus
     + ,antirawsector,antirawupdown,antirawsignal,ntofraw,tofrstatus
     + ,tofrplane,tofrbar,tofrtovta,tofrtovtd,tofrsdtm
*
*
* Variable Declarations
*



*

*
*--   Enter user code here
*
      real x(2),y(2),z(2),tm(2)
      logical cuts(10)      
      integer init,wide,ipass
      data init /0/
      data ipass/0/
      if(init.eq.0)then
         id=ibe*10000
         call hbook1(id+11,'Trclusters',200,-0.5,199.5,0.)
         call hbook1(id+12,'TOFclusters',20,-0.5,19.5,0.)
         call hbook1(id+21,'Tof 1',100,0.,1.,0.)
         call hbook1(id+22,'Tof 2',100,0.,1.,0.)
         call hbook1(id+23,'Tof 3',100,0.,1.,0.)
         call hbook1(id+24,'Tof 4',100,0.,1.,0.)
         call hbook1(id+25,'Tof 4',100,0.,1.,0.)
         call hbook1(id+31,'Tof 1',100,0.,1.,0.)
         call hbook1(id+32,'Tof 2',100,0.,1.,0.)
         call hbook1(id+33,'Tof 3',100,0.,1.,0.)
         call hbook1(id+34,'Tof 4',100,0.,1.,0.)
         call hbook1(id+35,'Tof 4',100,0.,1.,0.)
         call hbook1(id+41,'Tof 1',200,0.,2.,0.)
         call hbook1(id+142,'Tof 2*beta^2',200,0.,.2,0.)
         call hbook1(id+42,'Tof 2',200,0.,2.,0.)
         call hbook1(id+43,'Tof 3',200,0.,2.,0.)
         call hbook1(id+44,'Tof 4',200,0.,2.,0.)
         call hbook1(id+45,'Tof 4',200,0.,2.,0.)
         call hbook2(id+101,'Tof2 vs Tof3',100,0.,1.,100,0.,1.,0.)
         call hbook2(id+102,'Tof2 vs Tof3',100,0.,1.,100,0.,1.,0.)
         call hbook2(id+103,'Tof2 vs Tof3',100,0.,1.,100,0.,1.,0.)
         call hbook1(id+51,'beta',200,0.05,1.05,0.)
         call hbook1(id+52,'beta',200,0.05,1.05,0.)
         call hbook1(id+152,'beta',200,0.05,1.05,0.)
         call hbook1(id+252,'beta',200,0.05,1.05,0.)
         call hbook1(id+54,'beta',200,0.05,2.05,0.)
         call hbook1(id+154,'beta',200,0.8,1.2,0.)
         call hbook2(53,'beta',100,0.05,0.55,100,0.05,0.55,0.)
         call hbook2(153,'beta',100,0.05,0.55,100,0.05,0.55,0.)
         call hbook1(id+61,'mass',200,3.,13.,0.)
         call hbook1(id+62,'mass',200,3.,13.,0.)
         call hbook1(id+63,'mass',200,3.,13.,0.)
         call hbook1(id+162,'mass',200,3.,13.,0.)
         call hbook1(id+262,'mass',200,3.,13.,0.)
         call hbook1(id+163,'mass',200,3.,13.,0.)
         call hbook1(id+263,'mass',200,3.,13.,0.)
         call hbook1(id+64,'mass',200,3.,13.,0.)
         call hbook1(id+65,'mmom',200,0.,2.,0.)
         call hbook1(id+71,'Tr 1',200,0.,.1,0.)
         call hbook1(id+72,'Tr 2',200,0.,.1,0.)
         call hbook1(id+73,'Tr 3',200,0.,.1,0.)
         call hbook1(id+74,'Tr 4',200,0.,.1,0.)
         call hbook1(id+75,'Tr s',200,0.,.1,0.)
         call hbook1(id+76,'Tr s',200,0.,0.1,0.)
         call hbook1(id+79,'Tr s',200,0.,0.3,0.)
         call hbook1(id+81,'betachi2',100,0.,10.,0.)
         call hbook1(id+1011,'Trclusters',200,-0.5,199.5,0.)
         call hbook1(id+1012,'TOFclusters',10,-0.5,19.5,0.)
         call hbook1(id+1021,'Tof 1',100,0.,1.,0.)
         call hbook1(id+1022,'Tof 2',100,0.,1.,0.)
         call hbook1(id+1023,'Tof 3',100,0.,1.,0.)
         call hbook1(id+1024,'Tof 4',100,0.,1.,0.)
         call hbook1(id+1025,'Tof 4',100,0.,1.,0.)
        init=1
      endif
      ipass=ipass+1
      if(ipass.gt.imax)return
      do i=1,10
         cuts(i)=.false.
      enddo
      s1=0
      s2=0
      s3=0
      s4=0
      i2=0
      i3=0
      do i=1,ntof
       
       if(plane(i).eq.1)then
          s1=s1+tofedep(i)/1000.
       endif
       if(plane(i).eq.2)s2=s1+tofedep(i)/1000.
       if(plane(i).eq.3)s3=s1+tofedep(i)/1000.
       if(plane(i).eq.4)s4=s1+tofedep(i)/1000.
       if(plane(i).eq.2)i2=1
       if(plane(i).eq.3)i3=1
       if(plane(i).eq.2)tm(1)=toftime(i)
       if(plane(i).eq.2)x(1)=tofcoo(1,i)
       if(plane(i).eq.2)y(1)=tofcoo(2,i)
       if(plane(i).eq.2)z(1)=tofcoo(3,i)
       if(plane(i).eq.3)tm(2)=toftime(i)
       if(plane(i).eq.3)x(2)=tofcoo(1,i)
       if(plane(i).eq.3)y(2)=tofcoo(2,i)
       if(plane(i).eq.3)z(2)=tofcoo(3,i)
      enddo
      if(i2.eq.0.or.i3.eq.0)return
      call hf1(id+12,float(ntof),1.)
      call hf1(id+21,s1,1.)
      call hf1(id+22,s2,1.)
      call hf1(id+23,s3,1.)
      call hf1(id+24,s4,1.)
      call hf1(id+25,s1+s2+s3,1.)
      if(npart.gt.0)then
       call hf1(id+1012,float(ntof),1.)
       call hf1(id+1021,s1,1.)
       call hf1(id+1022,s2,1.)
       call hf1(id+1023,s3,1.)
       call hf1(id+1024,s4,1.)
       call hf1(id+1025,s1+s2+s3+s4,1.)
      endif
      anal2be=0
      cuts(1)=s4.lt.0.01.or.s4.gt.0.25e10
      if(cuts(1))then
       call hf1(id+31,s1,1.)
       call hf1(id+32,s2,1.)
       call hf1(id+33,s3,1.)
       call hf1(id+34,s4,1.)
       call hf1(id+35,s1+s2+s3,1.)
        rd=(x(2)-x(1))**2+(y(2)-y(1))**2+(z(2)-z(1))**2
        rd=sqrt(rd)
       cuts(2)=s1+s2+s3.gt.0.45.and.s3.gt.0.2
       if(cuts(2))then
        if(tm(2).gt.tm(1))then
          b3=rd/(tm(2)-tm(1))/2.998e10
        else 
          br=1.1
        endif
c        write(*,*)tm(2)-tm(1),rd,b3
c        write (*,*)'ok 2'
        ct=abs(z(2)-z(1))/rd
        if(s4.lt.0.02.and.npart.eq.0)then
         call hf2(id+101,s2,s3,1.)
         call hf2(id+103,s2,s3,1.)
        endif
        call hf1(id+41,s1,1.)
        call hf1(id+42,s2,1.)
        call hf1(id+43,s3,1.)
        call hf1(id+44,s4,1.)
        call hf1(id+45,s1+s2+s3+s4,1.)
        b2=sqrt(0.028/s2/ct)
            fac=1.23
        if(b2.lt.0.65)then
          delta2=(1.-b2**2)/b2**2
         gamma2=delta2*(1+sqrt((delta2-1.)/delta2))
         call hf1(id+163,fac*(s3)*gamma2,1.)
         call hf1(id+162,fac*(s3+s4)*gamma2,1.)
        endif
c        write (*,*)'ok 2a'
       if(b3.lt.0.5.and.npart.eq.0)then
         delta3=(1.-b3**2)/b3**2
         gamma3=delta3*(1+sqrt((delta3-1.)/delta3))
         if((s3+s4+0.018)*gamma3.gt.10.)then
c          write(*,*)'1',s1,s2,s3,s4,b3
         endif        
         call hf1(id+263,fac*(s3+0.018)*gamma3,1.)
         call hf1(id+262,fac*(s3+s4+0.018)*gamma3,1.)
        endif
c        write (*,*)'ok 3'
        call hf1(id+52,b2,1.)
        call hf1(id+152,b3,1.)
        bn=(b2+4*b3)/5.
        if(bn.lt.0.65)then
         delta2=(1.-bn**2)/bn**2
         gamma2=delta2*(1+sqrt((delta2-1.)/delta2))
        endif
        call hf1(id+252,bn,1.)
        cuts(3)=npart.gt.0
         if(cuts(3))then
           ipt=ptrackp(npart)
           s=0
           do j=1,nhits(ipt)
            r=sumr(phits(j,ipt))/1.e6*
     +      abs(hitr(3,phits(j,ipt))-z(1))/abs(z(2)-z(1))
            r=sumr(phits(j,ipt))/1.e6/2
            s=s+r
            call hf1(id+70+j,r,1.)
           enddo
           call hf1(id+79,s,1.)
           ipb=pbetap(npart)
c           ipb=1
           call hf1(id+51,beta(ipb),1.)
c           write (*,*)'ok 4'
           
           call hf2(id+53,beta(ipb),b2,1.)
           call hf2(id+153,beta(ipb),b3,1.)
           call hf1(id+54,b2/beta(ipb),1.)
           call hf1(id+154,b3/beta(ipb),1.)
           if(s4.lt.0.02)then
            call hf1(id+142,s2*beta(ipb)**2*ct,1.)
            call hf2(id+102,s2,s3+s/2.,1.)
            call hf2(id+103,s2,s3+s/2.,1.)
           endif
           cuts(4)=beta(ipb).lt.0.5
           if(cuts(4))then
            call hf1(id+81,betachi2(1),1.)
            xm0=9.328
            delta=(1.-beta(pbetap(npart))**2)/beta(pbetap(npart))**2
            gamma=delta*(1+sqrt((delta-1.)/delta))
           if((s3+s4+0.018)*gamma3.gt.10.)then
            xm=(s4+s3+s/2)*gamma
            xm3=(s4+s3+s/2)*gamma3
            xm33=(s4+s3+0.018)*gamma3
c            write(*,*)'2',b3,beta(ipb),xm,xm3,xm33,
c     +      beta(1),betachi2(1)
           endif        
            call hf1(id+61,fac*(s3)*gamma,1.)
            call hf1(id+62,fac*(s4+s3+s/2)*gamma,1.)
            call hf1(id+63,fac*(s3+s/2.)*gamma,
1.)
            call hf1(id+262,fac*(s4+s3+s/2)*gamma,1.)
            call hf1(id+263,fac*(s3+s/2.)*gamma,1.)
            call hf1(id+163,fac*(s3+s/2.)*gamma2,1.)
            call hf1(id+64,pmass(npart),1.)
            call hf1(id+65,pmom(npart)/momentum(1),1.)
          endif
         endif
       endif 
      endif
*
 999  END
      function estrate(inter,phim,theta,phi)
      integer init
      real modul
           real aa(4),zz(4)
           data aa/1.,4.,12.,1./
           data zz/1.,2.,6.,1./
           double precision t,p,sth,cl

      data init/0/  
      if(init.eq.0)then
        init=1
        modul=350.
        call hbook1(7101 ,'interstellar p',1000,50.,100050.,0.)
        call hbook1(7201 ,'interstellar He',1000,50.,100050.,0.)
        call hbook1(7102 ,'modulated p',1000,50.,100050.,0.)
        call hbook1(7202 ,'modulated He',1000,50.,100050.,0.)
        call hbook1(7103 ,'modulated p',1000,50.,100050.,0.)
        call hbook1(7203 ,'modulated He',1000,50.,100050.,0.)
          call hbook1(7106,'Rate H',100,0.,5000.,0.)
          call hbook1(7206,'Rate He',100,0.,600.,0.)

          fguz=1.
          bw=100
          al=50.
          bl=100050.
          do i=1,1000
*
*  Proton
*
           xkin=(i-1)*bw+al+bw/2
           xt=xkin/1000+0.938
           beta=sqrt(1-0.938**2/xt**2)
           xmom=beta*xt
           yy=1.5e4/beta/xmom**2.74/fguz
           call hf1(7101,xkin,yy)
           xt=(xkin+modul)/1000+0.938
           beta=sqrt(1-0.938**2/xt**2)
           xmom=beta*xt
           yy=1.5e4/beta/xmom**2.74/fguz
           yy=yy*(xkin**2+2*938*xkin)/
     +     ((xkin+modul)**2+2*938*(xkin+modul))
           call hf1(7102,xkin,yy)
           call hf1(7103,xkin,yy)
*
*  He
*
           xkin=(i-1)*bw+al+bw/2
           a=4
           am=0.938*a
           amm=am*1000
           z=2
           xk=xkin*a
           xt=xk/1000+am
           beta=sqrt(1-am**2/xt**2)
           xmom=beta*xt/z
           yy=.5e4/beta/xmom**2.68/fguz
           call hf1(7201,xkin,yy)
           xkm=xk+z*modul
           xt=xkm/1000+am
           beta=sqrt(1-am**2/xt**2)
           xmom=beta*xt/z
           yy=.5e4/beta/xmom**2.68/fguz
           yy=yy*(xk**2+2*amm*xk)/
     +     (xkm**2+2*amm*xkm)
           call hf1(7202,xkin,yy)
           call hf1(7203,xkin,yy)
          enddo

      endif
             estrate=0
             thetam=78.6/180.*3.1415926;
             do l=1,2
              xsum=hsum(7000+l*100+2)/10.
              um=sin(pi/2-thetam)*cos(phim)
              vm=sin(pi/2-thetam)*sin(phim)
              wm=cos(pi/2-thetam)
              up=sin(pi/2-theta)*cos(phi)
              vp=sin(pi/2-theta)*sin(phi)                                          
              wp=cos(pi/2-theta)
              cts=um*up+vm*vp+wm*wp
              xl=acos(cts)
              cl=abs(sin(xl))
              rate=0
              ntry=100 
              do k=1,ntry
               the=acos(rndm(d))
               phe=twopi*rndm(d)
               ue=sin(the)*cos(phe)
               ve=sin(the)*sin(phe)
               we=cos(the)
               uv=vm*wp-wm*vp
               vv=wm*up-wp*um                                                    
               wv=um*vp-vm*up
               cth=ue*uv+ve*vv+we*wv
               p=52.5*cl**4/(sqrt(1.d0-cth*cl**3)+1)**2
               pt=p
               ee=hrndm1(7000+l*100+3)/1000.
               xmom=sqrt(ee*(ee+2*0.938))*aa(l)/zz(l)
              if(xmom.gt.pt)then                                                
               rate=rate+1+inter*(www(xmom)-1)
              endif
             enddo
             rate=rate/ntry*xsum
             call hf1(7000+l*100+6,rate*xsum,1.)
             estrate=estrate+rate 
             enddo


      end



             function www(xmom)
              if(xmom.lt.20)then
               www=0.77+0.35*xmom-0.66e-2*xmom**2
              else
               www=5.13
              endif
              www=www*0.88
             end
