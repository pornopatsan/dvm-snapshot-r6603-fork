#include <stdio.h>
#include <stdlib.h>
#include <fcntl.h>
#include <unistd.h>
#include <assert.h>
#include "all_header.h"

const char *name = "test_checkpoint_exit_and_load_local_async";
enum Mode mode = LOCAL_ASYNC;

void run(int val) {
    #pragma dvm array distribute[block][block][block]
    unsigned int A[N][N][N];

    #pragma dvm parallel([i][j][k] on A[i][j][k]) cuda_block(256)
    for(int i = 0; i < N; ++i) {
        for(int j = 0; j < N; ++j) {
            for(int k = 0; k < N; ++k) {
                A[i][j][k] = 0x10000 * i + 0x100 * j + 0x1 * k + val;
            }
        }
    }

    DvmType **dvmDesc = (DvmType **) malloc(1 * sizeof(DvmType *));
    dvmDesc[0] = A;

    char B[5] = "12345"; B[val % 5] = '1';
    double C = 3.14 + val;

    #pragma dvm checkpoint test_checkpoint_exit_and_load_local_async LOCAL_ASYNC 3 [A] [B C]

    #pragma dvm parallel([i][j][k] on A[i][j][k]) cuda_block(256)
    for(int i = 0; i < N; ++i) {
        for(int j = 0; j < N; ++j) {
            for(int k = 0; k < N; ++k) {
                assert(A[i][j][k] == 0x10000 * i + 0x100 * j + 0x1 * k);
            }
        }
    }

    for (int i = 0; i < 5; ++i) {
        assert(B[i] == i + '1');
    }
    assert(C == 3.14);

    # pragma dvm checkpoint save test_checkpoint_exit_and_load_local_async
    # pragma dvm checkpoint wait test_checkpoint_exit_and_load_local_async
    dvmh_deactivate_control_point(name);
}

int main()
{
    run(0);
    run(1);
    run(2);
    return 0;
}