*  $Id: timel.F,v 1.4 2001/01/22 17:32:25 choutko Exp $
      subroutine timel(ss)
      data ss0/2.e6/
      call timex(ss1)
      ss=ss0-ss1
      end
