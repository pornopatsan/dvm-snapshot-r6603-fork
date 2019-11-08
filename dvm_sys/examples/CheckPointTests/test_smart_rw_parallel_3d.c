#include <stdio.h>
#include <stdlib.h>
#include <fcntl.h>
#include <unistd.h>

const char *filename = "data/test_smart_rw_parallel.txt";
const int N = 4;
int ok = 0;

int main()
{
    #pragma dvm array distribute[block][block][block]
    unsigned int A[4][4][4];

    #pragma dvm array align([i][j][k] with A[i][j][k])
    unsigned int B[4][4][4];

    #pragma dvm parallel([i][j][k] on A[i][j][k]) cuda_block(256)
    for(int i = 0; i < N; ++i) {
        for(int j = 0; j < N; ++j) {
            for(int k = 0; k < N; ++k) {
                A[i][j][k] = 0x10000 * i + 0x100 * j + 0x1 * k;
            }
        }
    }


    FILE *file = fopen(filename, "wbp");
    dvmh_smart_void_write(A, file);
    fclose(file);

    file = fopen(filename, "rbp");
    dvmh_smart_void_read(B, file);
    fclose(file);

    #pragma dvm parallel([i][j][k] on A[i][j][k]) cuda_block(256) reduction(sum(ok))
    for(int i = 0; i < N; ++i) {
        for(int j = 0; j < N; ++j) {
            for(int k = 0; k < N; ++k) {
                if (A[i][j][k] != B[i][j][k]) {
                    ok += 1;
                }
            }
        }
    }

    #pragma dvm actual(ok)
    return ok;
}
