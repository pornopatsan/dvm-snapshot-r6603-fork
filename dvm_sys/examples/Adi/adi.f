       program adi
       integer nx, ny, nz, itmax
       double precision eps, relax, maxeps
       double precision startt, endt, dvtime
       parameter(nx=384, ny=384, nz=384, maxeps=0.01, itmax=100)
       double precision a(nx, ny, nz)
!DVM$  DISTRIBUTE(BLOCK, BLOCK, BLOCK) :: a
       call init(a, nx, ny, nz)
!DVM$  BARRIER
       startt = dvtime()
       do it = 1, itmax
         eps=0.D0
!DVM$    ACTUAL(eps)
!DVM$    REGION
!DVM$    PARALLEL(k, j, i) ON a(i, j, k), ACROSS(a(1:1, 0:0, 0:0))
         do k = 2, nz - 1
           do j = 2, ny - 1
             do i = 2, nx - 1
               a(i, j, k) = (a(i-1, j, k) + a(i+1, j, k)) / 2
             enddo
           enddo
         enddo
!DVM$    PARALLEL(k, j, i) ON a(i, j, k), ACROSS(a(0:0, 1:1, 0:0))
         do k = 2, nz - 1
           do j = 2, ny - 1
             do i = 2, nx - 1
               a(i, j, k) = (a(i, j-1, k) + a(i, j+1, k)) / 2
             enddo
           enddo
         enddo
!DVM$    PARALLEL(k, j, i) ON a(i, j, k), ACROSS(a(0:0, 0:0, 1:1))
!DVM$>, REDUCTION(MAX(eps))
         do k = 2, nz - 1
           do j = 2, ny - 1
             do i = 2, nx - 1
               eps = max(eps, abs(a(i, j, k) -
     >                  (a(i,j,k-1) + a(i,j,k+1)) / 2))
               a(i, j, k) = (a(i, j, k-1) + a(i, j, k+1)) / 2
             enddo
           enddo
         enddo
!DVM$    END REGION
!DVM$    GET_ACTUAL(eps)
         print 200, it, eps
200      format (' IT = ', i4, '   EPS = ', e14.7)
         if (eps .lt. maxeps) exit
       enddo
!DVM$  BARRIER
       endt = dvtime()

       print *, 'ADI Benchmark Completed.'
       print 201, nx, ny, nz
201    format (' Size            = ', i4, ' x ', i4, ' x ', i4)
       print 202, itmax
202    format (' Iterations      =       ', i12)
       print 203, endt - startt
203    format (' Time in seconds =       ', f12.2)
       print *, 'Operation type  =   double precision'
       if (abs(eps - 0.07249074) .lt. 1.0e-6) then
         print *, 'Verification    =         SUCCESSFUL'
       else
         print *, 'Verification    =       UNSUCCESSFUL'
       endif

       print *, 'END OF ADI Benchmark'
       end

       subroutine init(a, nx, ny, nz)
       double precision a(nx, ny, nz)
!DVM$  INHERIT a
       integer nx, ny, nz
!DVM$  REGION OUT(a)
!DVM$  PARALLEL(k, j, i) ON a(i, j, k)
       do k = 1, nz
         do j = 1, ny
           do i = 1, nx
             if(k.eq.1 .or. k.eq.nz .or. j.eq.1 .or. j.eq.ny .or.
     >          i.eq.1 .or. i.eq.nx) then
               a(i, j, k) = 10.*(i-1)/(nx-1) + 10.*(j-1)/(ny-1)
     >                      + 10.*(k-1)/(nz-1)
             else
               a(i, j, k) = 0.D0
             endif
           enddo
         enddo
       enddo
!DVM$  END REGION
       end
