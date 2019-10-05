       subroutine rtl_nargs(pc)
!       USE MSFLIB

       integer*4 pc
       integer*4 rc
                       
       rc=nargs()
       pc=rc
       return
       end
       