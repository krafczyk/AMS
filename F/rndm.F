*  $Id: rndm.F,v 1.9 2001/11/30 16:47:11 choutko Exp $
             function rndm(dummy)
             
             data i/0/
             real *4 r(2)
             call grndm(r,1)
             rndm=r(1)
c             call grndmq(i1,i2,0,'G')
c              write(*,*)i1,i2
c             i=i+1
c             write(*,*)i,r(1)
              end
