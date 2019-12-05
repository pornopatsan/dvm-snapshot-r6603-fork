#include <stdio.h>
#include <stdlib.h>
#include <fcntl.h>
#include <unistd.h>

typedef long DvmType;
const int N = 8;
int ok = 0;

int main()
{
    // Save before creating. Should Fail
    dvmh_save_control_point("test_cp_create_fail_local");
    return 0;
}
