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

    #pragma dvm array align([i][j][k] with A[i][j][k])
    unsigned int B[N][N][N];

    #pragma dvm parallel([i][j][k] on A[i][j][k]) cuda_block(256)
    for(int i = 0; i < N; ++i) {
        for(int j = 0; j < N; ++j) {
            for(int k = 0; k < N; ++k) {
                A[i][j][k] = 0x10000 * i + 0x100 * j + 0x1 * k;
            }
        }
    }

    DvmType *dvmDesc = (DvmType *) malloc(2 * sizeof(DvmType *));
    dvmDesc[0] = A; dvmDesc[1] = B;
    dvmh_create_or_bind_control_point("test_cp_create_or_bind_parallel", dvmDesc, 2, 5, 1);
    dvmh_save_control_point("test_cp_create_or_bind_parallel");
    return 0;
}
