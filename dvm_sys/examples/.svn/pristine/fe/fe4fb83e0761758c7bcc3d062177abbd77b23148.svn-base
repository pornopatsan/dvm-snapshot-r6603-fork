        PROGRAM    JACH
        PARAMETER    (L=8,  ITMAX=20)
        REAL     A(L,L), EPS, MAXEPS, B(L,L)
CDVM$   DISTRIBUTE     ( BLOCK,   BLOCK)   ::   A
CDVM$   ALIGN  B(I,J)  WITH  A(I,J)
C		 arrays A and B  with block distribution 

        PRINT *,  '**********  TEST_JACOBI   **********'
                  MAXEPS  =  0.5E - 7
CDVM$   REGION
CDVM$   PARALLEL    (J,I)   ON   A(I, J)
C		nest of two parallel loops, iteration (i,j) will be executed on 
C		processor, which is owner of element A(i,j) 
            DO  1   J  =  1, L
            DO  1   I  =  1, L
                A(I,  J)  =  0.
                IF(I.EQ.1 .OR. J.EQ.1 .OR. I.EQ.L .OR. J.EQ.L) THEN
                      B(I,  J) = 0.
                ELSE
                      B(I,  J)  = ( 1. + I + J )
                ENDIF
    1       CONTINUE
CDVM$   END REGION
        DO  2   IT  =  1,  ITMAX
                  EPS  =  0.
CDVM$   ACTUAL(EPS)
CDVM$   REGION
CDVM$   PARALLEL  (J,  I)   ON  A(I,  J),  REDUCTION ( MAX( EPS ))
C		variable EPS is used for calculation of maximum value
                  DO  21  J  =  2, L-1
                  DO  21  I  =  2, L-1
                         EPS = MAX ( EPS,  ABS( B( I, J)  -  A( I, J)))
                         A(I, J)  =  B(I, J)
   21             CONTINUE
CDVM$   PARALLEL  (J,  I)   ON  B(I,  J),   SHADOW_RENEW   (A)
C		Copying shadow elements of array A from 
C		neighbouring processors before loop execution
                  DO  22  J = 2,  L-1
                  DO  22  I = 2,  L-1
        B(I, J) =  (A( I-1, J ) + A( I, J-1 ) + A( I+1, J)+
     *                        A( I, J+1 )) / 4
   22             CONTINUE
CDVM$   END REGION
CDVM$   GET_ACTUAL(EPS)
                  PRINT 200,  IT, EPS
200               FORMAT(' IT = ',I4, '   EPS = ', E14.7)
                  IF ( EPS . LT . MAXEPS )    GO TO   3
    2   CONTINUE
    3   CONTINUE
CDVM$   GET_ACTUAL(B)    
        OPEN (3, FILE='JAC.DAT', FORM='FORMATTED', STATUS='UNKNOWN')
        WRITE (3,*)   B
        CLOSE (3)
        END

