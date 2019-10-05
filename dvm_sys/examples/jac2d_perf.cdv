/* Jacobi-2 program */

#include <math.h>
#include <stdio.h>

#define Max(a, b) ((a) > (b) ? (a) : (b))

#define L 8000
#define ITMAX 100

int i, j, it;
float eps;
float MAXEPS = 0.5f;

/* 2D arrays block distributed along 2 dimensions */
#pragma dvm array distribute[block][block]
float A[L][L];
#pragma dvm array align([i][j] with A[i][j])
float B[L][L];

int main(int an, char **as)
{
    double startt, endt;
    #pragma dvm region
    {
    /* 2D parallel loop with base array A */
    #pragma dvm parallel([i][j] on A[i][j]) cuda_block(256)
    for (i = 0; i < L; i++)
        for (j = 0; j < L; j++)
        {
            A[i][j] = 0;
            if (i == 0 || j == 0 || i == L - 1 || j == L - 1)
                B[i][j] = 0;
            else
                B[i][j] = 3 + i + j;
        }
    }

#ifdef _DVMH
    dvmh_barrier();
    startt = dvmh_wtime();
#else
    startt = 0;
#endif
    /* iteration loop */
    for (it = 1; it <= ITMAX; it++)
    {
        eps = 0;
        #pragma dvm actual(eps)

        #pragma dvm region
        {
        /* Parallel loop with base array A */
        /* calculating maximum in variable eps */
        #pragma dvm parallel([i][j] on A[i][j]) reduction(max(eps)), cuda_block(256)
        for (i = 1; i < L - 1; i++)
            for (j = 1; j < L - 1; j++)
            {
                float tmp = fabs(B[i][j] - A[i][j]);
                eps = Max(tmp, eps);
                A[i][j] = B[i][j];
            }

        /* Parallel loop with base array B and */
        /* with prior updating shadow elements of array A */
        #pragma dvm parallel([i][j] on B[i][j]) shadow_renew(A), cuda_block(256)
        for (i = 1; i < L - 1; i++)
            for (j = 1; j < L - 1; j++)
                B[i][j] = (A[i - 1][j] + A[i][j - 1] + A[i][j + 1] + A[i + 1][j]) / 4.0f;
        }

        #pragma dvm get_actual(eps)
        printf(" IT = %4i   EPS = %14.7E\n", it, eps);
        if (eps < MAXEPS)
            break;
    }
#ifdef _DVMH
    dvmh_barrier();
    endt = dvmh_wtime();
#else
    endt = 0;
#endif

    printf(" Jacobi2D Benchmark Completed.\n");
    printf(" Size            =    %6d x %6d\n", L, L);
    printf(" Iterations      =       %12d\n", ITMAX);
    printf(" Time in seconds =       %12.2lf\n", endt - startt);
    printf(" Operation type  =     floating point\n");
    printf(" Verification    =       %12s\n", (fabs(eps - 58.37598) < 1e-3 ? "SUCCESSFUL" : "UNSUCCESSFUL"));

    printf(" END OF Jacobi2D Benchmark\n");
    return 0;
}
