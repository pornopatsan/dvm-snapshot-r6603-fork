      subroutine clfdvm
      integer  tstio
      logical  op
      if(tstio() .ne. 0) then
         do 1 i = 0,99         
            inquire (unit = i,opened = op)
C            if (op)   close (unit = i)
1        continue
      endif
      end

