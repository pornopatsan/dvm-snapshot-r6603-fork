#include <stdio.h>
#include <stdlib.h>
#include <fcntl.h>
#include <unistd.h>

int main()
{
    char *filename = "data/test_smart_rw_local_%d.txt";
    int ok = 0;

    #pragma dvm array distribute[block][block][block]
    unsigned int A[4][4][4];

    #pragma dvm array align([i][j][k] with A[i][j][k])
    unsigned int B[4][4][4];

    #pragma dvm parallel([i][j][k] on A[i][j][k]) cuda_block(256)
    for(int i = 0; i < 4; ++i) {
        for(int j = 0; j < 4; ++j) {
            for(int k = 0; k < 4; ++k) {
                A[i][j][k] = 0x100 * i + 0x10 * j + 0x1 * k;
            }
        }
    }

    FILE *file = fopen(filename, "wbl");
    dvmh_smart_void_write(A, file);
    fclose(file);

    file = fopen(filename, "rbl");
    dvmh_smart_void_read(B, file);
    fclose(file);

    #pragma dvm parallel([i][j][k] on A[i][j][k]) cuda_block(256)
    for(int i = 0; i < 4; ++i) {
        for(int j = 0; j < 4; ++j) {
            for(int k = 0; k < 4; ++k) {
                if (A[i][j][k] != B[i][j][k]) {
                    ok += 1;
                }
            }
        }
    }

    return ok;
}
