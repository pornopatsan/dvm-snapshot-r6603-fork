/* Jacobi-2 program */

#include <math.h>
#include <stdio.h>

#define Max(a, b) ((a) > (b) ? (a) : (b))

#define L 8
#define ITMAX 20

int i, j, it;
double eps;
double MAXEPS = 0.5;

FILE *f;

/* 2D arrays block distributed along 2 dimensions */
#pragma dvm array distribute[block][block]
double A[L][L];
#pragma dvm array align([i][j] with A[i][j])
double B[L][L];

int main(int an, char **as)
{
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
                double tmp = fabs(B[i][j] - A[i][j]);
                eps = Max(tmp, eps);
                A[i][j] = B[i][j];
            }

        /* Parallel loop with base array B and */
        /* with prior updating shadow elements of array A */
        #pragma dvm parallel([i][j] on B[i][j]) shadow_renew(A), cuda_block(256)
        for (i = 1; i < L - 1; i++)
            for (j = 1; j < L - 1; j++)
                B[i][j] = (A[i - 1][j] + A[i][j - 1] + A[i][j + 1] + A[i + 1][j]) / 4.0;
        }

        #pragma dvm get_actual(eps)
        printf("it=%4i   eps=%e\n", it, eps);
        if (eps < MAXEPS)
            break;
    }

    f = fopen("jacobi.dat", "wb");
    #pragma dvm get_actual(B)
    fwrite(B, sizeof(double), L * L, f);
    fclose(f);

    return 0;
}
