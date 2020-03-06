#include <stdio.h>
#include <stdlib.h>
#include <fcntl.h>
#include <unistd.h>

typedef long DvmType;
const int N = 8;

int main()
{
    #pragma dvm array distribute[block][block][block]
    unsigned int A[N][N][N];

    #pragma dvm parallel([i][j][k] on A[i][j][k]) cuda_block(256)
    for(int i = 0; i < N; ++i) {
        for(int j = 0; j < N; ++j) {
            for(int k = 0; k < N; ++k) {
                A[i][j][k] = 0x10000 * i + 0x100 * j + 0x1 * k;
            }
        }
    }

    DvmType *dvmDesc = (DvmType *) malloc(1 * sizeof(DvmType *));
    dvmDesc[0] = A;
    dvmh_create_control_point("test_cp_create_local", dvmDesc, 1, 2, 0);
    dvmh_save_control_point("test_cp_create_local");
    return 0;
}
