/* Jacobi-3 program */

#include <math.h>
#include <stdio.h>

#define Max(a, b) ((a) > (b) ? (a) : (b))

#define L 384
#define ITMAX 100

int i, j, k, it;
float eps;
float MAXEPS = 0.5f;

/* 3D arrays block distributed along 3 dimensions */
#pragma dvm array distribute[block][block][block]
float A[L][L][L];
#pragma dvm array align([i][j][k] with A[i][j][k])
float B[L][L][L];

int main(int an, char **as)
{
    double startt, endt;
    #pragma dvm region
    {
    /* 3D parallel loop with base array A */
    #pragma dvm parallel([i][j][k] on A[i][j][k]) cuda_block(32, 8)
    for (i = 0; i < L; i++)
        for (j = 0; j < L; j++)
            for (k = 0; k < L; k++)
            {
                A[i][j][k] = 0;
                if (i == 0 || j == 0 || k == 0 || i == L - 1 || j == L - 1 || k == L - 1)
                    B[i][j][k] = 0;
                else
                    B[i][j][k] = 4 + i + j + k;
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
        #pragma dvm parallel([i][j][k] on A[i][j][k]) reduction(max(eps)), cuda_block(32, 8)
        for (i = 1; i < L - 1; i++)
            for (j = 1; j < L - 1; j++)
                for (k = 1; k < L - 1; k++)
                {
                    float tmp = fabs(B[i][j][k] - A[i][j][k]);
                    eps = Max(tmp, eps);
                    A[i][j][k] = B[i][j][k];
                }

        /* Parallel loop with base array B and */
        /* with prior updating shadow elements of array A */
        #pragma dvm parallel([i][j][k] on B[i][j][k]) shadow_renew(A), cuda_block(32, 8)
        for (i = 1; i < L - 1; i++)
            for (j = 1; j < L - 1; j++)
                for (k = 1; k < L - 1; k++)
                    B[i][j][k] = (A[i - 1][j][k] + A[i][j - 1][k] + A[i][j][k - 1] + A[i][j][k + 1] + A[i][j + 1][k] + A[i + 1][j][k]) / 6.0f;
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

    printf(" Jacobi3D Benchmark Completed.\n");
    printf(" Size            = %4d x %4d x %4d\n", L, L, L);
    printf(" Iterations      =       %12d\n", ITMAX);
    printf(" Time in seconds =       %12.2lf\n", endt - startt);
    printf(" Operation type  =     floating point\n");
    printf(" Verification    =       %12s\n", (fabs(eps - 5.058044) < 1e-4 ? "SUCCESSFUL" : "UNSUCCESSFUL"));

    printf(" END OF Jacobi3D Benchmark\n");
    return 0;
}
