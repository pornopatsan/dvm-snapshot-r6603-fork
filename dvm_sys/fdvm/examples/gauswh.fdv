       PROGRAM GAUSWH
       PARAMETER   ( N = 10 )
       REAL   A( N, N+1 ),X( N )
       DOUBLE PRECISION WB(10)
C		section A(1:N,1:N)   - matrix of coefficients "A"
C		section A(1:N,N+1) - vector of free members "b"
CDVM$  DISTRIBUTE   A  ( WGT_BLOCK(WB,10),  *) 
CDVM$  ALIGN   X(I)   WITH   A(I,N+1)
       DATA WB/10.,9.,8.,7.,6.,5.,4.,3.,2.,1./      

CDVM$  PARALLEL (I) ON A(I,*)
       DO 100 I=1,N
       DO 100 J=1,N+1
       IF (I .EQ. J) THEN
         A(I,J)=2.0
       ELSE
         IF (J .EQ. N+1)   THEN
           A(I,J)=1.0
         ELSE
           A(I,J)=0.0
       ENDIF
       ENDIF   
  100  CONTINUE 
C
C       ELIMINATION
C
        DO  1  I = 1, N-1

C		the i-th row of array A will be buffered before
C		execution of i-th iteration, and reference A(I,K) 
C		will be replaced with corresponding reference to buffer 
CDVM$  PARALLEL ( J ) ON  A( J, * ), REMOTE_ACCESS (A( I, :))
                  DO  5  J = I+1, N
                  DO  5  K = I+1, N+1
        A( J, K ) = A( J, K ) - A( J, I ) * A( I, K ) / A( I, I )
    5             CONTINUE
    1   CONTINUE
        X( N ) = A( N, N+1 ) / A( N, N )
C       BACK SUBSTITUTION
C
        DO  6  J = N-1, 1, -1
C		the (j+1)-th elements of array X will be buffered before
C		execution of j-th iteration, and reference X(J+1) 
C		will be replaced with reference to temporal variable
CDVM$   PARALLEL  ( I )   ON   A( I , * ), REMOTE_ACCESS ( X( J+1 ))
                  DO  7  I = 1, J
                     A( I, N+1 )  = A( I, N+1 ) - A( I,J+1)*X(J+1)
    7             CONTINUE
        X( J ) = A( J,  N+1 ) / A( J, J)
    6   CONTINUE
        PRINT *, X
        END

