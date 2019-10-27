#include <stdio.h>
#include <stdlib.h>
#include <fcntl.h>
#include <unistd.h>

int main()
{
    #pragma dvm array distribute[block][block][block]
    unsigned int A[4][4][4];

    #pragma dvm parallel([i][j][k] on A[i][j][k]) cuda_block(256)
    for(int i = 0; i < 4; ++i) {
        for(int j = 0; j < 4; ++j) {
            for(int k = 0; k < 4; ++k) {
                A[i][j][k] = 0x100 * i + 0x10 * j + 0x1 * k;
            }
        }
    }

    FILE *file = fopen("test_result.txt", "wb");
    dvmh_smart_void_write(A, file);

    return 0;
}
