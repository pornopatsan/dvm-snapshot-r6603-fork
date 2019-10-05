	PROGRAM SOR
	PARAMETER (N = 10)
	REAL A(N, N), EPS, MAXEPS, W
	INTEGER ITMAX
!DVM$	DISTRIBUTE A(BLOCK, BLOCK)
	PRINT *, '**********  TEST_SOR   **********'
	ITMAX = 20
	MAXEPS = 0.5E-5
	W = 0.5
!DVM$	REGION
!DVM$	PARALLEL(J, I) ON A(I, J)
	DO J = 1, N
	DO I = 1, N
		IF (I .EQ. J) THEN
		     A(I, J) = N + 2
		ELSE
		     A(I, J) = -1.0
		ENDIF
	END DO
	END DO
!DVM$	END REGION
	DO IT = 1, ITMAX
	EPS = 0.0
!DVM$	ACTUAL(EPS)
!DVM$	REGION
!DVM$	PARALLEL(J, I) ON A(I, J), PRIVATE(S),
!DVM$*	REDUCTION(MAX(EPS)), ACROSS(A(1:1,1:1))
	DO J = 2, N-1
	DO I = 2, N-1
		S = A(I, J)
		A(I, J) = (W / 4) * (A(I-1, J) + A(I+1, J) + A(I, J-1) +
     *		A(I, J+1)) + (1 - W) * A(I, J)
	EPS = MAX(EPS, ABS(S - A(I, J)))
	END DO
	END DO
!DVM$	END REGION
!DVM$	GET_ACTUAL(EPS)
	PRINT 200, IT, EPS
200     FORMAT(' IT = ', I4, '   EPS = ', E14.7)
	IF (EPS .LT. MAXEPS) GOTO 4
	END DO
4	CONTINUE
!DVM$	GET_ACTUAL(A)
	OPEN(3, FILE='SOR.DAT', FORM='FORMATTED', STATUS='UNKNOWN')
	WRITE(3, *) A
	CLOSE(3)
	END