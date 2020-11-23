/* Jacobi-1 program */

#include <math.h>
#include <stdio.h>

#define Max(a, b) ((a) > (b) ? (a) : (b))

#define L 8
#define ITMAX 20

int i, it;
double eps;
double MAXEPS = 0.5;

FILE *f;

/* 1D arrays block distributed along 1 dimension */
#pragma dvm array distribute[block]
double A[L];
#pragma dvm array align([i] with A[i])
double B[L];

int main(int an, char **as)
{
    #pragma dvm region
    {
    /* 1D parallel loop with base array A */
    #pragma dvm parallel([i] on A[i]) cuda_block(256)
    for (i = 0; i < L; i++)
    {
        A[i] = 0;
        if (i == 0 || i == L - 1)
            B[i] = 0;
        else
            B[i] = 2 + i;
    }
    }

    /* iteration loop */
    for (it = 1; it <= ITMAX; it++)
    {
        eps = 0;
        #pragma dvm actual(eps)

        #pragma dvm region
        {
        /* Parallel loop with base array A */
        /* calculating maximum in variable eps */
        #pragma dvm parallel([i] on A[i]) reduction(max(eps)), cuda_block(256)
        for (i = 1; i < L - 1; i++)
        {
            double tmp = fabs(B[i] - A[i]);
            eps = Max(tmp, eps);
            A[i] = B[i];
        }

        /* Parallel loop with base array B and */
        /* with prior updating shadow elements of array A */
        #pragma dvm parallel([i] on B[i]) shadow_renew(A), cuda_block(256)
        for (i = 1; i < L - 1; i++)
            B[i] = (A[i - 1] + A[i + 1]) / 2.0;
        }

        #pragma dvm get_actual(eps)
        printf("it=%4i   eps=%e\n", it, eps);
        if (eps < MAXEPS)
            break;
    }

    f = fopen("jacobi.dat", "wb");
    #pragma dvm get_actual(B)
    fwrite(B, sizeof(double), L, f);
    fclose(f);

    return 0;
}
