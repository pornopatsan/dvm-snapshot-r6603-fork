        PROGRAM JAC3D
        PARAMETER (L=384, ITMAX=100)
        REAL A(L, L, L), EPS, MAXEPS, B(L, L, L)
        DOUBLE PRECISION STARTT, ENDT, dvtime
!DVM$   DISTRIBUTE(BLOCK, BLOCK, BLOCK) :: A
!DVM$   ALIGN B(I, J, K) WITH A(I, J, K)
!        arrays A and B  with block distribution 

        MAXEPS = 0.5
!DVM$   REGION
!DVM$   PARALLEL(K, J, I) ON A(I, J, K), CUDA_BLOCK(32, 8)
!        nest of two parallel loops, iteration (i, j) will be executed on
!        processor, which is owner of element A(i, j)
        DO K = 1, L
          DO J = 1, L
            DO I = 1, L
              A(I, J, K) = 0.
              IF (I.EQ.1 .OR. J.EQ.1 .OR. K.EQ.1
     >.OR. I.EQ.L .OR. J.EQ.L .OR. K.EQ.L) THEN
                B(I, J, K) = 0.
              ELSE
                B(I, J, K) = (1. + I + J + K)
              ENDIF
            ENDDO
          ENDDO
        ENDDO
!DVM$   END REGION
!DVM$   BARRIER
        STARTT = dvtime()
        DO IT = 1, ITMAX
          EPS = 0.
!DVM$     ACTUAL(EPS)
!DVM$     REGION
!DVM$     PARALLEL(K, J, I) ON A(I, J, K), REDUCTION(MAX(EPS))
!DVM$>, CUDA_BLOCK(32, 8)
!          variable EPS is used for calculation of maximum value
          DO K = 2, L - 1
            DO J = 2, L - 1
              DO I = 2, L - 1
                EPS = MAX(EPS, ABS(B(I, J, K) - A(I, J, K)))
                A(I, J, K) = B(I, J, K)
              ENDDO
            ENDDO
          ENDDO
!DVM$     PARALLEL(K, J, I) ON B(I, J, K), SHADOW_RENEW(A)
!DVM$>, CUDA_BLOCK(32, 8)
!          Copying shadow elements of array A from
!          neighbouring processors before loop execution
          DO K = 2, L - 1
            DO J = 2, L - 1
              DO I = 2, L - 1
                B(I, J, K) = (A(I, J, K-1) + A(I, J-1, K) + A(I-1, J, K)
     >+ A(I+1, J, K) + A(I, J+1, K) + A(I, J, K+1)) / 6.
              ENDDO
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

        PRINT *, 'Jacobi3D Benchmark Completed.'
        PRINT 201, L, L, L
201     FORMAT (' Size            = ', I4, ' x ', I4, ' x ', I4)
        PRINT 202, ITMAX
202     FORMAT (' Iterations      =       ', I12)
        PRINT 203, ENDT - STARTT
203     FORMAT (' Time in seconds =       ', F12.2)
        PRINT *, 'Operation type  =     floating point'
        IF (ABS(EPS - 5.058044) .LT. 1.0E-4) THEN
          PRINT *, 'Verification    =         SUCCESSFUL'
        ELSE
          PRINT *, 'Verification    =       UNSUCCESSFUL'
        ENDIF

        PRINT *, 'END OF Jacobi3D Benchmark'
        END
