      subroutine timel(ss)
      data ss0/2.e6/
      call timex(ss1)
      ss=ss0-ss1
      end
      subroutine NPQ
      common /quest/iquest(100)
      iquest(10)=64000
      end
