*  $Id: trmc.f,v 1.5 2001/01/22 17:32:26 choutko Exp $
      REAL FUNCTION trmc(used)
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
     + ,ntrcl,Idsoft(200),Statust(200),NelemL(200),NelemR(200),ntrclmc
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
     + ,antirawsignal(32),tofrtovta(2,20),tofrtovtd(2,20),tofrsdtm(2,20)
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
     + ,TOFMCXcoo,TOFMCtof,TOFMCedep,ntrcl,Idsoft,Statust,NelemL,NelemR
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
     + ,ntrraw,rawaddress,rawlength,nantiraw,antirawstatus,antirawsector
     + ,antirawupdown,antirawsignal,ntofraw,tofrstatus,tofrplane,tofrbar
     + ,tofrtovta,tofrtovtd,tofrsdtm
       integer tridsoft(5,200)

*

*
*--   Enter user code here
*

       integer used      
      integer init,wide
      parameter (nchx=384)
      parameter(nchy=640)
      vector ax(nchx)
      vector ax2(nchx)
      vector ay2(nchy)
      vector ay(nchy)
      vector cx(nchx)
      vector cy(nchy)
c      real ax(nchx)
c      real ax2(nchx)
c      real ay2(nchy)
c      real ay(nchy)
c      real cx(nchx)
c      real cy(nchy)
      data ax/nchx*0/
      data ay/nchy*0/
      data ax2/nchx*0/
      data ay2/nchy*0/
      data cx/nchx*0/
      data cy/nchy*0/
      data init/0/
      integer nch(2)
      if(init.eq.0)then
*
* Sigmas
*
            nch(1)=nchx-1
            nch(2)=nchy-1
            xlim=200
            do i=1,nchx
             ax(i)=0
             ax2(i)=0
             cx(i)=0
            enddo
            do i=1,nchy
             ay(i)=0
             ay2(i)=0
             cy(i)=0
            enddo
            call hbook1(100+1,'difference x',200,-xlim,xlim,0.)
            call hbook1(100+2,'difference y',200,-xlim,xlim,0.)
            call hbook1(110+1,'difference x edge l',
     +      200,-xlim,xlim,0.)
            call hbook1(110+2,'difference y edge l',
     +      200,-xlim,xlim,0.)
            call hbook1(120+1,'difference x edge r',
     +      200,-xlim,xlim,0.)
            call hbook1(120+2,'difference y edge r',
     +      200,-xlim,xlim,0.)
            call hbook2(130+1,'difference x edge l',
     +      200,-xlim,xlim,100,0.,1.,0.)
            call hbook2(130+2,'difference y edge l',
     +      200,-xlim,xlim,100,0.,1.,0.)
            call hbook2(140+1,'difference x edge r',
     +      200,-xlim,xlim,100,0.,1.,0.)
            call hbook2(140+2,'difference y edge r',
     +      200,-xlim,xlim,100,0.,1.,0.)
            call hbook1(150+1,'bad x',nchx,0.5,nchx+0.5,0.)
            call hbook1(150+2,'bad y',nchy,0.5,nchy+0.5,0.)
         call hbook1(201,'Signal x',200,0.,200.,0.)   
         call hbook1(202,'Signal y',200,0.,200.,0.)   
         call hbook1(1201,'Signal x',200,0.,200.,0.)   
         call hbook1(1202,'Signal y',200,0.,200.,0.)   
         call hbook1(211,'Noise x',200,0.,100.,0.)   
         call hbook1(212,'Noise y',200,0.,100.,0.)   
         call hbook1(221,'S/Noise x',200,0.,100.,0.)   
         call hbook1(222,'S/Noise y',200,0.,100.,0.)   
         call hbook1(1221,'S/Noise x',200,0.,100.,0.)   
         call hbook1(1222,'S/Noise y',200,0.,100.,0.)   
         call hbook1(231,'NStrips x',10,-0.5,9.5,0.)   
         call hbook1(232,'NStrips y',10,-0.5,9.5,0.)   
         call hbook1(241,'S1/Stot x',200,0.,100.,0.)   
         call hbook1(242,'S1/Stot',200,0.,100.,0.)

         call hbook2(921,'u vs diff ',
     +   100,0.,0.7,100,-200.,200.,0.)
         call hbook2(922,'v vs diff ',
     +   100,0.,0.7,100,-100.,100.,0.)
   
         call hbook1(931,'layer 1&6 x ', 90,0.,45.,0.)
         call hbook1(932,'layer 1&6 x ', 90,0.,45.,0.)
         call hbook1(933,' y', 90,0.,45.,0.)
         call hbook1(934,' y', 90,0.,45.,0.)
         call hbook1(935,'other x ', 90,0.,45.,0.)
         call hbook1(936,'other x ', 90,0.,45.,0.)
         
         call hbook1(7007,'part',50,0.5,50.5,0.)         
        init=1
      endif

      do i=1,ntrcl
       tridsoft(1,i)=mod(idsoft(i),10) ! layer
       tridsoft(2,i)=mod(idsoft(i)/10,100) ! ladder
       ir=mod(idsoft(i)/1000,10)
       if(ir.eq.0)then
        tridsoft(3,i)=0   !side
        tridsoft(4,i)=0   !half
       else if(ir.eq.1)then
        tridsoft(3,i)=0
        tridsoft(4,i)=1
       else if(ir.eq.2)then
        tridsoft(3,i)=1
        tridsoft(4,i)=0
       else 
        tridsoft(3,i)=1
        tridsoft(4,i)=1
       endif
       tridsoft(5,i)=mod(idsoft(i)/10000,1000)    ! strip
      enddo
        nnoise=0 
        rmsm=0
       do i=1,ntrclmc
        call hf1(7007,float(itra(i)),1.)
       enddo
                            
       do i=1,ntrclmc
        if(itra(i).ne.555)then
         mclay=mod(idsoftmc(i),10)
         mclad=mod(idsoftmc(i)/10,100)
         mcsen=idsoftmc(i)/1000
         do j=1,ntrrh
          if(mclay.eq.layer(j).and.tridsoft(2,px(j)).eq.mclad
     +    .and.(statusr(j).ne.0.or.used.eq.0))then
           do k=1,2
            r=10000*(-xgl(k,i)+hitr(k,j))
            if(abs(r).lt.200)then
              if(k.eq.1)l=px(j)
              if(k.eq.2)l=py(j)             
              if((k.eq.1.and.abs(r).gt.100.) .or.
                 (k.eq.2.and.abs(r).gt.50.) )then
                call hf1(150+k,float(tridsoft(5,l)+1),1.)
              endif
              call hf1(100+k,r,1.)
              call hf1(200+k,sumt(l),1.)
              if(abs(r).lt.30)call hf1(1200+k,sumt(l),1.)
              call hf1(210+k,sigmat(l),1.)
              if(sigmat(l).ne.0)
     +        call hf1(220+k,sumt(l)/sigmat(l),1.)
              if(sigmat(l).ne.0.and.abs(r).lt.30)
     +        call hf1(1220+k,sumt(l)/sigmat(l),1.)
              call hf1(230+k,float(nelemr(l)-neleml(l)+1),1.)
              if(sumt(l).gt.0)then
                 s1t=amplitude(1-neleml(l),l)/sumt(l)
              else 
                 s1t=2.
              endif
              call hf1(240+k,s1t,1.)
              
              if(tridsoft(5,l).eq.0)then
                  call hf1(110+k,r,1.)
                  call hf2(130+k,r,s1t,1.)
              endif
              if(tridsoft(5,l).eq.nch(k))then
                 call hf1(120+k,r,1.)              
                  call hf2(140+k,r,s1t,1.)
              endif
              if(npart.eq.1)then
               u=sin(ptheta(1))*cos(pphi(1))
               v=sin(ptheta(1))*sin(pphi(1))
               w=cos(ptheta(1))
               if(abs(r).lt.200)then
                if(k.eq.1)then
                 il=tridsoft(5,px(j))+1
                 ilay=tridsoft(1,px(j))
                 if(il.le.0.or.il.gt.nchx)write(*,*)'x',il
                  cx(il)=cx(il)+1
                  ax(il)=ax(il)+r
                  ax2(il)=ax2(il)+r**2
                  pi=3.1415926
                  t=(pi/2-acos(u))/pi*180.
                  call hf2(921,u,r,1.)
                  if(ilay.eq.1.or.ilay.eq.6)then
                    call hf1(931,t,1.)
                    call hf1(932,t,r*r)
                  else
                    call hf1(935,t,1.)
                    call hf1(936,t,r*r)
                  endif
                else if(k.eq.2)then
                 il=tridsoft(5,py(j))+1
                 if(il.le.0.or.il.gt.nchy)write(*,*)'y',il
                 cy(il)=cy(il)+1
                 ay(il)=ay(il)+r
                 ay2(il)=ay2(il)+r**2
                 pi=3.1415926
                 t=(pi/2-acos(v))/pi*180.
                 call hf2(922,v,r,1.)
                 call hf1(933,t,1.)
                 call hf1(934,t,r*r)
                endif              
               endif
              endif
            endif
           enddo
          endif
         enddo 
        endif
       enddo
       trmc=0.        
 999  END
