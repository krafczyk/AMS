*  $Id: rndm.F,v 1.7 2001/01/22 17:32:25 choutko Exp $
             function rndm(dummy)
             data i/0/
             real *4 r(2)
             call grndm(r,1)
             rndm=r(1)
c             i=i+1
c             write(*,*)i,r(1)
              end
