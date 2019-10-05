       subroutine rtl_getarg(np)
!       USE MSFLIB

       integer*4 np
       character*256 s
       common /dvmstr/ s
       integer*2 n
       integer*2 status
       integer*4 lstr
       common /length/ lstr

       n=np
       call getarg(n,s,status)
       lstr=status
       return
       end
       