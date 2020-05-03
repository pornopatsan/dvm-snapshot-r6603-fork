#include <stdio.h>
#include <stdlib.h>
#include <fcntl.h>
#include <unistd.h>
#include <assert.h>
#include "all_header.h"

const char *name = "test_checkpoint_parallel_async";
enum Mode mode = PARALLEL_ASYNC;

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

    DvmType **dvmDesc = (DvmType **) malloc(1 * sizeof(DvmType *));
    dvmDesc[0] = A;
    dvmh_create_or_bind_control_point(name, dvmDesc, 1, nfiles, mode);
    dvmh_load_control_point(name);
    dvmh_save_control_point(name);

    #pragma dvm parallel([i][j][k] on A[i][j][k]) cuda_block(256)
    for(int i = 0; i < N; ++i) {
        for(int j = 0; j < N; ++j) {
            for(int k = 0; k < N; ++k) {
                A[i][j][k] = 0;
            }
        }
    }

    dvmh_wait_control_point(name);
    dvmh_load_control_point(name);

    #pragma dvm parallel([i][j][k] on A[i][j][k]) cuda_block(256)
    for(int i = 0; i < N; ++i) {
        for(int j = 0; j < N; ++j) {
            for(int k = 0; k < N; ++k) {
                assert(A[i][j][k] == 0x10000 * i + 0x100 * j + 0x1 * k);
            }
        }
    }

    return 0;
}
