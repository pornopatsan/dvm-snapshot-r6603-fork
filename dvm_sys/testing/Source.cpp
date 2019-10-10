#include "../tools/pppa/src/statlist.h"

int main(int argv, char ** argc) {

  CStat s;
	printf("\nOHAIMARK\n>>>%d", s.nproc);
	try {
		printf("go to init\n");
		s.init((char*)("sts.gz+"));
	}
	catch(...){
		printf("error!");
	}
	printf("\n>>>%d", s.nproc);
	printf("ok\n");
	return 0;
}
