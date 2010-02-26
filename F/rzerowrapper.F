              subroutine rzerowrapper(za,zb,x0,zmin,ierr)  
                common /zminacommon/zmina
!$OMP threadprivate(/zminacommon/)
                real *8 zmina,zmin
                real *8 za,zb,x0,xfun,r,eps
                external xfun
                eps=0.01
                zmina=zmin
                maxfu=1000 
                call dzero(za,zb,x0,r,eps,maxfu,xfun)
                ierr=0
                if(r.lt.0)ierr=1 
              end
              real *8 function xfun(x,i)
!$OMP  threadprivate(/zminacommon/)
                common /zminacommon/zmina
                real *8 zmina
                real *8 x
                xfun=derfc(x)-zmina
              end
