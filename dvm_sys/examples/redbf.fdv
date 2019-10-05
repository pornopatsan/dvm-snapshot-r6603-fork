      PROGRAM REDBF
      PARAMETER (N=10)
      REAL A(N,N), EPS, MAXEXP, W
      INTEGER ITMAX
CDVM$ DISTRIBUTE A(BLOCK, BLOCK)
      PRINT *,  '**********  TEST_REDBLACK   **********'
      ITMAX = 20
      MAXEXP = 0.5E - 5
      W = 0.5
CDVM$ PARALLEL (J,I) ON A(I, J)
      DO 1  J = 1,N
      DO 1  I = 1,N
         IF (I.EQ.J) THEN
           A(I,J) = N+2
         ELSE
          A(I,J) = -1. 
         ENDIF
1     CONTINUE
      DO 2  IT = 1, ITMAX
      EPS = 0.
C	loop for red and black variables 
      DO 3 IRB = 0,1
CDVM$ PARALLEL (J,I) ON A(I, J), NEW (S), REDUCTION (MAX(EPS)),
CDVM$*                      SHADOW_RENEW  (A)
C	variable S - private variable in loop iterations
C	variable EPS is used for calculation of maximum value 

C	Exception : iteration space is not rectangular

      DO 21  J = 2,N-1
      DO 21  I = 2 + MOD(J+IRB,2), N-1, 2
         S = A(I,J)
         A(I,J) = (W/4) * (A(I-1,J) + A(I+1,J) + A(I,J-1) +
     *   A(I,J+1)) + (1-W) * A(I,J)
         EPS = MAX (EPS, ABS(S - A(I,J)))
21    CONTINUE
3     CONTINUE
      PRINT 200, IT, EPS
200   FORMAT(' IT = ',I4, '   EPS = ', E14.7)
      IF (EPS.LT.MAXEXP) GO TO 4
2     CONTINUE
4     OPEN (3, FILE='REDBF.DAT', FORM='FORMATTED',STATUS='UNKNOWN')
      WRITE (3,*)   A
      CLOSE (3)
      END

