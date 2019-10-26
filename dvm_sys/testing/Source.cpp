#include "../tools/pppa/src/statlist.h"
#include <iostream>
#include <string>

int main(int argv, char ** argc) {
  CStat s;
	try {
		printf("go to init\n");
		s.init((char*)("sts.gz+"));
        std::string str;
        s.to_string(str);
        std::cout << "STRING: " << str;
	}
	catch(...){
		printf("error!");
	}
	printf(">>>%d", s.nproc);
	printf("ok\n");
	return 0;
}
