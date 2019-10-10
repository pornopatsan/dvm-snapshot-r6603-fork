/* GAUSS program with WGTBLOCK distribution */

#include <math.h>
#include <stdlib.h>
#include <stdio.h>

#define N 10

double wb[10] = {1., 2., 3., 4., 5., 6., 7., 8., 9., 10.};

int main(int an, char **as)
{
    int i, j, k;

    /* declaration of dynamic distributed arrays */
    #pragma dvm array
    float (*A)[N + 1];
    #pragma dvm array
    float (*X);

    /* create arrays */
    A = malloc(N * (N + 1) * sizeof(float));
    #pragma dvm redistribute(A[wgtblock(wb, 10)][*])
    X = malloc(N * sizeof(float));
    #pragma dvm realign(X[i] with A[i][])

    /* initialize array A */
    #pragma dvm region
    {
    #pragma dvm parallel([i][j] on A[i][j])
    for (i = 0; i < N; i++)
        for (j = 0; j < N + 1; j++)
            if (i == j || j == N)
                A[i][j] = 1.f;
            else
                A[i][j] = 0.f;
    }

    /* elimination */
    for (i = 0; i < N - 1; i++)
    {
        #pragma dvm region
        {
        #pragma dvm parallel([k][j] on A[k][j]) remote_access(A[i][])
        for (k = i + 1; k < N; k++)
            for (j = i + 1; j < N + 1; j++)
                A[k][j] = A[k][j] - A[k][i] * A[i][j] / A[i][i];
        }
    }

    /* reverse substitution */
    #pragma dvm region in(A[N - 1][N - 1 : N]), out(X[N - 1])
    {
    X[N - 1] = A[N - 1][N] / A[N - 1][N - 1];
    }

    for (j = N - 2; j >= 0; j--)
    {
        #pragma dvm region
        {
        #pragma dvm parallel([k] on A[k][]) remote_access(X[j + 1])
        for (k = 0; k <= j; k++)
            A[k][N] = A[k][N] - A[k][j + 1] * X[j + 1];
        X[j] = A[j][N] / A[j][j];
        }

        #pragma dvm remote_access(X[j])
        {
        printf("j=%4i   X[j]=%e\n", j, X[j]);
        }
    }

    free(A);
    free(X);
    return 0;
}
