/* ADI program */

#include <math.h>
#include <stdlib.h>
#include <stdio.h>

#define Max(a, b) ((a) > (b) ? (a) : (b))

#define nx 384
#define ny 384
#define nz 384

#pragma dvm inherit(a)
void init(double (*a)[ny][nz]);

int main(int argc, char *argv[])
{
    double maxeps, eps;
    #pragma dvm array distribute[block][block][block]
    double (*a)[ny][nz];
    int it, itmax, i, j, k;
    double startt, endt;
    maxeps = 0.01;
    itmax = 100;
    a = (double (*)[ny][nz])malloc(nx * ny * nz * sizeof(double));
    init(a);

#ifdef _DVMH
    dvmh_barrier();
    startt = dvmh_wtime();
#else
    startt = 0;
#endif
    for (it = 1; it <= itmax; it++)
    {
        eps = 0;
        #pragma dvm actual(eps)
        #pragma dvm region
        {
        #pragma dvm parallel([i][j][k] on a[i][j][k]) across(a[1:1][0:0][0:0])
        for (i = 1; i < nx - 1; i++)
            for (j = 1; j < ny - 1; j++)
                for (k = 1; k < nz - 1; k++)
                    a[i][j][k] = (a[i-1][j][k] + a[i+1][j][k]) / 2;
        #pragma dvm parallel([i][j][k] on a[i][j][k]) across(a[0:0][1:1][0:0])
        for (i = 1; i < nx - 1; i++)
            for (j = 1; j < ny - 1; j++)
                for (k = 1; k < nz - 1; k++)
                    a[i][j][k] = (a[i][j-1][k] + a[i][j+1][k]) / 2;
        #pragma dvm parallel([i][j][k] on a[i][j][k]) across(a[0:0][0:0][1:1]), reduction(max(eps))
        for (i = 1; i < nx - 1; i++)
            for (j = 1; j < ny - 1; j++)
                for (k = 1; k < nz - 1; k++)
                {
                    double tmp1 = (a[i][j][k-1] + a[i][j][k+1]) / 2;
                    double tmp2 = fabs(a[i][j][k] - tmp1);
                    eps = Max(eps, tmp2);
                    a[i][j][k] = tmp1;
                }
        }
        #pragma dvm get_actual(eps)
        printf(" IT = %4i   EPS = %14.7E\n", it, eps);
        if (eps < maxeps)
            break;
    }
#ifdef _DVMH
    dvmh_barrier();
    endt = dvmh_wtime();
#else
    endt = 0;
#endif
    free(a);

    printf(" ADI Benchmark Completed.\n");
    printf(" Size            = %4d x %4d x %4d\n", nx, ny, nz);
    printf(" Iterations      =       %12d\n", itmax);
    printf(" Time in seconds =       %12.2lf\n", endt - startt);
    printf(" Operation type  =   double precision\n");
    printf(" Verification    =       %12s\n", (fabs(eps - 0.07249074) < 1e-6 ? "SUCCESSFUL" : "UNSUCCESSFUL"));

    printf(" END OF ADI Benchmark\n");
    return 0;
}

#pragma dvm inherit(a)
void init(double (*a)[ny][nz])
{
    int i, j, k;
    #pragma dvm region out(a)
    {
    #pragma dvm parallel([i][j][k] on a[i][j][k])
    for (i = 0; i < nx; i++)
        for (j = 0; j < ny; j++)
            for (k = 0; k < nz; k++)
                if (k == 0 || k == nz - 1 || j == 0 || j == ny - 1 || i == 0 || i == nx - 1)
                    a[i][j][k] = 10.0 * i / (nx - 1) + 10.0 * j / (ny - 1) + 10.0 * k / (nz - 1);
                else
                    a[i][j][k] = 0;
    }
}
