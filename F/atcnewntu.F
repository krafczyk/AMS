      subroutine atcgeom(ierr)                                               
*************************************************************                
* SUB: atcgeom                                                               
*************************************************************                
                                                                             
      real pmcentx, pmcenty, pmcentz,                                        
     +     cellxdim, cellydim, cellzdim,                                     
     +     zpm1st, zpm2nd,                                                   
     +     zup1st, zlo1st, zup2nd, zlo2nd                                    
                                                                             
      common /atcuser/ pmcentx(168), pmcenty(168), pmcentz(168),             
     +                 cellxdim, cellydim, cellzdim,                         
     +                 zpm1st, zpm2nd,                                       
     +                 zup1st, zlo1st, zup2nd, zlo2nd                        
                                 
* from commons.h
      common /amsdatadir/amsdlength,amsdblength,amsdatadir,
     +                   amsdatabase
      integer amsdlength,amsdblength
      character *128 amsdatadir,amsdatabase
                   
* --- init                                                                   
      ierr = 0                                                               
                                                                             
* --- dimensions                                                             
      cellxdim =  11.5                                                       
      cellydim =  11.5                                                       
      cellzdim =   8.7                                                       
      cellofpm =   2.6                                                       
      zpm1st   = -91.7                                                       
      zpm2nd   = -111.1                                                      
      zup1st   = zpm1st + cellofpm + cellzdim                                
      zlo1st   = zpm1st + cellofpm                                           
      zup2nd   = zpm2nd + cellofpm + cellzdim                                
      zlo2nd   = zpm2nd + cellofpm                                           
      write(*,*) 'Opening ATC geometry file...'
      open(unit=90,file=amsdatadir(1:amsdlength)//'atcgeomet.dat',
     +     status='unknown',iostat=ierr)        
      if (ierr.ne.0) then                                                    
         print*,'atcgeomet.dat file...reading problems!!!'                   
         return                                                              
      endif     
      write(*,*) 'ATC geometry file found...',            
     +           amsdatadir(1:amsdlength)//'atcgeomet.dat'
      do i=1,168                                                             
         read(90,900,end=999) numb,pmcentx(i),pmcenty(i),pmcentz(i)          
         pmcentx(i) = pmcentx(i)/10.                                         
         pmcenty(i) = pmcenty(i)/10.                                         
         pmcentz(i) = pmcentz(i)/10.                                         
      enddo                                                                  
*      write(6,*) (pmcentx(i),pmcenty(i),pmcentz(i),i=1,168)                  
      close(unit=90)                                                         
 999  continue                                                               
 900  format(I3,7x,f6.1,1x,f4.0,1x,f5.0)                                     
                                                                             
* --- return                                                                 
      return                                                                 
      end                                                                    
********************************************************************         
      SUBROUTINE atcrec(run,nctcht,cooctc,ctchitlayer,ctchitcolumn,
     +                  ctchitrow,ctchitsignal,
     +                  atcnbcel,atcnbphe,atcidcel,atcdispm,
     +                  atcdaero,atcstatu,atcbeta) 
********************************************************************         
*   F. Barao (30/Nov/98)                                                     
*   Last modified: 7/Dec
*                                                                            
*   AIMS: get ATC detailed information per event                             
*     IN:                                                                    
*    OUT: atcnbcel(1:2) = nb cells acrossed                             
*         atcnbphe(1:2) = 
*         atcidcel(1:2) =
*         atcdispm(1:2) =
*         atcdaero(1:2) =
*         atcstatu(1:2) =
*         atcbeta       =
*                                                                            
********************************************************************   
                                                                             
* --- atc geometry                                                           
      real pmcentx, pmcenty, pmcentz,                                        
     +     cellxdim, cellydim, cellzdim,                                     
     +     zpm1st, zpm2nd,                                                   
     +     zup1st, zlo1st, zup2nd, zlo2nd                                    
      common /atcuser/ pmcentx(168), pmcenty(168), pmcentz(168),             
     +                 cellxdim, cellydim, cellzdim,                         
     +                 zpm1st, zpm2nd,                                       
     +                 zup1st, zlo1st, zup2nd, zlo2nd                        
                                                                             
* --- routine parameters declaration                                
* >>> output         
      integer atcnbcel(2),atcidcel(2),atcdispm(2),atcdaero(2), 
     +        atcstatu(2)                                      
      real    atcnbphe(2), atcbeta
* >>> input                                      
      real cooctc(3,2,1)                                            
      real ctchitsignal(*)                                          
      integer run,nctcht,ctchitcolumn(*),ctchitrow(*),ctchitlayer(*)
                                                                             
* --- variables declaration
      parameter (maxcells=10)
      integer idcel1(maxcells),idcel2(maxcells)
      real    daero1(maxcells),daero2(maxcells), 
     +        dispm1(maxcells),dispm2(maxcells)
      real a(2),b(2),                                                        
     +     xentr1(maxcells), yentr1(maxcells), zentr1(maxcells),             
     +     xexit1(maxcells), yexit1(maxcells), zexit1(maxcells),             
     +     xentr2(maxcells), yentr2(maxcells), zentr2(maxcells),             
     +     xexit2(maxcells), yexit2(maxcells), zexit2(maxcells),             
     +     uc(3), uv(3)                                                     
      
                                                                             
* --- data                                                                   
      data idummy /0/                                                        
      data ifirstrun /896851950/
                                                                             
                                                                             
***************< code >********************* 
                                
* --- init                                                                   
      call vzero(idcel1,maxcells)                                            
      call vzero(idcel2,maxcells)                                            
      call vzero(daero1,maxcells)                                            
      call vzero(daero2,maxcells)                                            
      call vzero(dispm1,maxcells)                                            
      call vzero(dispm2,maxcells)                                            
* --- atc geometry                                                           
      if (idummy.eq.0) then                                                  
         call atcgeom(ierr)                                                  
         if (ierr.ne.0) then                                                 
            print*,' problems with reading atc geom...'                      
            stop                                                             
         endif                                                               
         idummy = 1                                                          
      endif                                                                  
* --- extrapolation                                                          
      a(1) = (cooctc(1,1,1)-cooctc(1,2,1))/                                  
     +       (cooctc(3,1,1)-cooctc(3,2,1))                                   
      b(1) =  cooctc(1,1,1) - a(1)*cooctc(3,1,1)                             
      a(2) = (cooctc(2,1,1)-cooctc(2,2,1))/                                  
     +       (cooctc(3,1,1)-cooctc(3,2,1))                                   
      b(2) =  cooctc(2,1,1) - a(2)*cooctc(3,1,1)                             
* --- cells acrossed                                                         
      nbcel1 = 0                                                             
      icelldum = 0                                                           
      do zuser=zup1st+0.05,zlo1st-0.05,-0.1                                  
         if (zuser.lt.zlo1st .or. zuser.gt.zup1st) then                      
            icellnb = -1                                                     
            goto 10                                                          
         endif                                                               
         x = a(1)*zuser + b(1)                                               
         y = a(2)*zuser + b(2)                                               
         dxycent = 999.                                                      
         do i=1,80                                                           
             xdif = pmcentx(i) - x                                           
             ydif = pmcenty(i) - y                                           
             ddif = sqrt(xdif**2+ydif**2)                                    
             if (ddif.lt.dxycent) then                                       
                dxycent = ddif                                               
                icellnb = i                                                  
                xdifdum = xdif                                               
                ydifdum = ydif                                               
             endif                                                           
         enddo                                                               
         if (abs(xdifdum).gt.cellxdim/2.) icellnb = 0.                       
         if (abs(ydifdum).gt.cellydim/2.) icellnb = 0.                       
 10      if (icellnb.gt.0 .and. icellnb.ne.icelldum) then                    
            if (icelldum.gt.0) then                                          
               xexit1(nbcel1) = x                                            
               yexit1(nbcel1) = y                                            
               zexit1(nbcel1) = zuser                                        
            endif                                                            
            icelldum = icellnb                                               
            nbcel1 = nbcel1 + 1                                              
            idcel1(nbcel1) = icellnb                                         
            xentr1(nbcel1) = x                                               
            yentr1(nbcel1) = y                                               
            zentr1(nbcel1) = zuser                                           
         endif                                                               
         if ((icellnb.eq.0 .or. icellnb.eq.-1) .and.                         
     +       icelldum.gt.0) THEN                                             
             icelldum = 0                                                    
             xexit1(nbcel1) = x                                              
             yexit1(nbcel1) = y                                              
             zexit1(nbcel1) = zuser                                          
         ENDIF                                                               
      enddo                                                                  
*                                                                            
      nbcel2 = 0                                                             
      icelldum = 0                                                           
      do zuser=zup2nd+0.05,zlo2nd-0.05,-0.1                                  
         if (zuser.lt.zlo2nd .or. zuser.gt.zup2nd) then                      
            icellnb = -1                                                     
            goto 11                                                          
         endif                                                               
         x = a(1)*zuser + b(1)                                               
         y = a(2)*zuser + b(2)                                               
         dxycent = 999.                                                      
         do i=81,168                                                         
             xdif = pmcentx(i) - x                                           
             ydif = pmcenty(i) - y                                           
             ddif = sqrt(xdif**2+ydif**2)                                    
             if (ddif.lt.dxycent) then                                       
                dxycent = ddif                                               
                icellnb = i                                                  
                xdifdum = xdif                                               
                ydifdum = ydif                                               
             endif                                                           
         enddo                                                               
         if (abs(xdifdum).gt.cellxdim/2.) icellnb = 0.                       
         if (abs(ydifdum).gt.cellydim/2.) icellnb = 0.                       
 11      if (icellnb.gt.0 .and. icellnb.ne.icelldum) then                    
            if (icelldum.gt.0) then                                          
               xexit2(nbcel2) = x                                            
               yexit2(nbcel2) = y                                            
               zexit2(nbcel2) = zuser                                        
            endif                                                            
            icelldum = icellnb                                               
            nbcel2 = nbcel2 + 1                                              
            idcel2(nbcel2) = icellnb                                         
            xentr2(nbcel2) = x                                               
            yentr2(nbcel2) = y                                               
            zentr2(nbcel2) = zuser                                           
         endif                                                               
         if ((icellnb.eq.0 .or. icellnb.eq.-1) .and.                         
     +       icelldum.gt.0) THEN                                             
             icelldum = 0                                                    
             xexit2(nbcel2) = x                                              
             yexit2(nbcel2) = y                                              
             zexit2(nbcel2) = zuser                                          
         ENDIF                                                               
      enddo                                                                  
* coding
      atcnbcel(1) = nbcel1
      atcnbcel(2) = nbcel2
*
      call vzero(atcidcel,2)
      do i=1,min(5,nbcel1)
       atcidcel(1) = atcidcel(1) + idcel1(i)*10**(2*i-2)        
      enddo
      do i=1,min(5,nbcel2)
       atcidcel(2) = atcidcel(2) + (idcel2(i)-80)*10**(2*i-2)        
      enddo
       
* --- aerogel distance acrossed                                              
      do i=1,min(maxcells,nbcel1)                                            
         daero1(i) = sqrt(                                                   
     +                   (xentr1(i)-xexit1(i))**2 +                          
     +                   (yentr1(i)-yexit1(i))**2 +                          
     +                   (zentr1(i)-zexit1(i))**2                            
     +                  )                                                    
      enddo                                                                  
      do i=1,min(maxcells,nbcel2)                                            
         daero2(i) = sqrt(                                                   
     +                   (xentr2(i)-xexit2(i))**2 +                          
     +                   (yentr2(i)-yexit2(i))**2 +                          
     +                   (zentr2(i)-zexit2(i))**2                            
     +                  )                                                    
      enddo                                      
* coding                                                           
      call vzero(atcdaero,2)                                       
      do i=1,min(5,nbcel1)                                         
         atcdaero(1) = atcdaero(1) + 
     +                 min(int(daero1(i)*10.),99)*10**(2*i-2)
      enddo                                                        
      do i=1,min(5,nbcel2)                                         
         atcdaero(2) = atcdaero(2) + 
     +                 min(int(daero2(i)*10.),99)*10**(2*i-2)
      enddo                                                        
               
* --- closest distance to pm                                                 
      uv(1) = cooctc(1,2,1) - cooctc(1,1,1)                                  
      uv(2) = cooctc(2,2,1) - cooctc(2,1,1)                                  
      uv(3) = cooctc(3,2,1) - cooctc(3,1,1)                                  
      uvm   = sqrt(uv(1)**2+uv(2)**2+uv(3)**2)                               
*                                                                            
      do i=1,min(maxcells,nbcel1)                                            
         uc(1) = pmcentx(idcel1(i))-cooctc(1,1,1)                            
         uc(2) = pmcenty(idcel1(i))-cooctc(2,1,1)                            
         uc(3) = pmcentz(idcel1(i))-cooctc(3,1,1)                            
         ucm   = sqrt(uc(1)**2+uc(2)**2+uc(3)**2)                            
         utotsum = 0.                                                        
         do 12 j=1,3                                                         
 12         utotsum = utotsum + uv(j)*uc(j)                                  
         cosalf = utotsum/ucm/uvm                                            
         if (abs(cosalf).gt.1.) cosalf=cosalf/abs(cosalf)                    
         dispm1(i) = ucm*sqrt(1.-cosalf**2)                                  
      enddo                                                                  
*                                                                            
      do i=1,min(maxcells,nbcel2)                                            
         uc(1) = pmcentx(idcel2(i))-cooctc(1,1,1)                            
         uc(2) = pmcenty(idcel2(i))-cooctc(2,1,1)                            
         uc(3) = pmcentz(idcel2(i))-cooctc(3,1,1)                            
         ucm   = sqrt(uc(1)**2+uc(2)**2+uc(3)**2)                            
         utotsum = 0.                                                        
         do 13 j=1,3                                                         
 13         utotsum = utotsum + uv(j)*uc(j)                                  
         cosalf = utotsum/ucm/uvm                                            
         if (abs(cosalf).gt.1.) cosalf=cosalf/abs(cosalf)                    
         dispm2(i) = ucm*sqrt(1.-cosalf**2)                                  
      enddo                                                        
* coding                                                           
      call vzero(atcdispm,2)                                       
      do i=1,min(5,nbcel1)                                         
         atcdispm(1) = atcdispm(1) + 
     +                 min(int(dispm1(i)*10.),99)*10**(2*i-2)
      enddo                                                        
      do i=1,min(5,nbcel2)                                         
         atcdispm(2) = atcdispm(2) + 
     +                 min(int(dispm2(i)*10.),99)*10**(2*i-2)
      enddo                                                        
          
* --- nb photoelectrons                                                      
      call vzero(atcnbphe,2)                                                   
      do i=1,nctcht                                                          
         itcell = (ctchitlayer(i)-1)*80 +                                    
     +            (ctchitcolumn(i)-1)*8 +                                    
     +            ctchitrow(i)                                               
         if (nbcel1.ne.0 .and. ctchitlayer(i).eq.1) then                     
            distup = sqrt(                                                   
     +                    (xentr1(1)-pmcentx(itcell))**2 +                   
     +                    (yentr1(1)-pmcenty(itcell))**2                     
     +                   )                                                   
            distdo = sqrt(                                                   
     +                    (xexit1(nbcel1)-pmcentx(itcell))**2 +              
     +                    (yexit1(nbcel1)-pmcenty(itcell))**2                
     +                   )                                                   
            distan = min(distup,distdo)                                      
            if (distan.lt.20.) atcnbphe(1)=atcnbphe(1)+ctchitsignal(i) 
         endif         
*                                                      
         if (nbcel2.ne.0 .and. ctchitlayer(i).eq.2) then                     
            distup = sqrt(                                                   
     +                    (xentr2(1)-pmcentx(itcell))**2 +                   
     +                    (yentr2(1)-pmcenty(itcell))**2                     
     +                   )                                                   
            distdo = sqrt(                                                   
     +                    (xexit2(nbcel2)-pmcentx(itcell))**2 +              
     +                    (yexit2(nbcel2)-pmcenty(itcell))**2                
     +                   )                                                   
            distan = min(distup,distdo)                                      
            if (distan.lt.30.) atcnbphe(2)=atcnbphe(2)+ctchitsignal(i)
         endif                                                               
      enddo                                                                  
* coding inefficient cells (10h period)
      atcstatu(1) = 2 + 11*10 + 68*10**3
      if (abs(run-ifirstrun)/36000.le.1) then
       atcstatu(2) = 3 + (103-80)*10 + (133-80)*10**3 + 
     +                   (175-80)*10**5
      else 
       if (abs(run-ifirstrun)/36000.le.8) then
        atcstatu(2) = 4 + (93-80)*10 + (103-80)*10**3 + 
     +                    (133-80)*10**5 + (175-80)*10**7  
       else
        atcstatu(2) = 3 + (93-80)*10 + (103-80)*10**3 + (133-80)*10**5 
       endif       
      endif      

* beta measurement
      atcbeta = 0.0
      
* --- return                                                                 
      return                                                                 
      end                                                                    

