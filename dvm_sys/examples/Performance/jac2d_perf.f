        PROGRAM JAC2D
        PARAMETER (L=8000, ITMAX=100)
        REAL A(L, L), EPS, MAXEPS, B(L, L)
        DOUBLE PRECISION STARTT, ENDT, dvtime
!DVM$   DISTRIBUTE(BLOCK, BLOCK) :: A
!DVM$   ALIGN B(I, J) WITH A(I, J)
!        arrays A and B  with block distribution 

        MAXEPS = 0.5
!DVM$   REGION
!DVM$   PARALLEL(J, I) ON A(I, J), CUDA_BLOCK(256)
!        nest of two parallel loops, iteration (i, j) will be executed on
!        processor, which is owner of element A(i, j)
        DO J = 1, L
          DO I = 1, L
            A(I, J) = 0.
            IF (I.EQ.1 .OR. J.EQ.1 .OR. I.EQ.L .OR. J.EQ.L) THEN
              B(I, J) = 0.
            ELSE
              B(I, J) = (1. + I + J)
            ENDIF
          ENDDO
        ENDDO
!DVM$   END REGION
!DVM$   BARRIER
        STARTT = dvtime()
        DO IT = 1, ITMAX
          EPS = 0.
!DVM$     ACTUAL(EPS)
!DVM$     REGION
!DVM$    PARALLEL(J, I) ON A(I, J), REDUCTION(MAX(EPS)), CUDA_BLOCK(256)
!          variable EPS is used for calculation of maximum value
          DO J = 2, L - 1
            DO I = 2, L - 1
              EPS = MAX(EPS, ABS(B(I, J) - A(I, J)))
              A(I, J) = B(I, J)
            ENDDO
          ENDDO
!DVM$     PARALLEL(J, I) ON B(I, J), SHADOW_RENEW(A), CUDA_BLOCK(256)
!          Copying shadow elements of array A from
!          neighbouring processors before loop execution
          DO J = 2, L - 1
            DO I = 2, L - 1
          B(I, J) = (A(I, J-1) + A(I-1, J) + A(I+1, J) + A(I, J+1)) / 4.
            ENDDO
          ENDDO
!DVM$     END REGION
!DVM$     GET_ACTUAL(EPS)
          PRINT 200, IT, EPS
200       FORMAT (' IT = ', I4, '   EPS = ', E14.7)
          IF (EPS .LT. MAXEPS) EXIT
        ENDDO
!DVM$   BARRIER
        ENDT = dvtime()

        PRINT *, 'Jacobi2D Benchmark Completed.'
        PRINT 201, L, L
201     FORMAT (' Size            =    ', I6, ' x ', I6)
        PRINT 202, ITMAX
202     FORMAT (' Iterations      =       ', I12)
        PRINT 203, ENDT - STARTT
203     FORMAT (' Time in seconds =       ', F12.2)
        PRINT *, 'Operation type  =     floating point'
        IF (ABS(EPS - 58.37598) .LT. 1.0E-3) THEN
          PRINT *, 'Verification    =         SUCCESSFUL'
        ELSE
          PRINT *, 'Verification    =       UNSUCCESSFUL'
        ENDIF

        PRINT *, 'END OF Jacobi2D Benchmark'
        END
