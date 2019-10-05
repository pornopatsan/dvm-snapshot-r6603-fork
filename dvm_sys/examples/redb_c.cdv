/* RED-BLACK program */

#include <math.h>
#include <stdlib.h>
#include <stdio.h>

#define Max(a, b) ((a) > (b) ? (a) : (b))

#define N 8
#define ITMAX 10

int main(int an, char ** as)
{
    int i, j, it;
    float MAXEPS = 0.5E-5f;
    float w = 0.5f;

    #pragma dvm array distribute[block][block]
    float (*A)[N];

    /* Create array */
    A = malloc(N * N * sizeof(float));

    #pragma dvm region
    {
    /* Initialization parallel loop */
    #pragma dvm parallel([i][j] on A[i][j]) cuda_block(256)
    for (i = 0; i < N; i++)
        for (j = 0; j < N; j++)
            if (i == j)
                A[i][j] = N + 2;
            else
                A[i][j] = -1.0f;
    }

    /* iteration loop */
    for (it = 1; it <= ITMAX; it++)
    {
        float eps = 0.f;
        #pragma dvm actual(eps)

        #pragma dvm region
        {
        /* Parallel loop with reduction on RED points */
        #pragma dvm parallel([i][j] on A[i][j]) shadow_renew(A), reduction(max(eps)), cuda_block(256)
        for (i = 1; i < N - 1; i++)
            for (j = 1; j < N - 1; j++)
                if ((i + j) % 2 == 1)
                {
                    float s;
                    s = A[i][j];
                    A[i][j] = (w / 4) * (A[i - 1][j] + A[i + 1][j] + A[i][j - 1] + A[i][j + 1]) + (1 - w) * A[i][j];
                    eps = Max(fabs(s - A[i][j]), eps);
                }

        /* Parallel loop on BLACK points (without reduction) */
        #pragma dvm parallel([i][j] on A[i][j]) shadow_renew(A), cuda_block(256)
        for (i = 1; i < N - 1; i++)
            for (j = 1; j < N - 1; j++)
                if ((i + j) % 2 == 0)
                {
                    A[i][j] = (w / 4) * (A[i - 1][j] + A[i + 1][j] + A[i][j - 1] + A[i][j + 1]) + (1 - w) * A[i][j];
                }
        }

        #pragma dvm get_actual(eps)
        printf("it=%4i   eps=%e\n", it, eps);
        if (eps < MAXEPS)
            break;
    }

    free(A);
    return 0;
}
