*  $Id: rndm.F,v 1.10 2003/04/23 09:41:27 choutko Exp $
             function rndm(dummy)
             
             data i/0/
             real *4 r(2)
             call grndm(r,1)
             rndm=r(1)
c             call grndmq(i1,i2,0,'G')
c              write(*,*)i1,i2
c             i=i+1
c             write(*,*)r(1)
              end
