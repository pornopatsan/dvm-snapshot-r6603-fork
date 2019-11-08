#include "../tools/pppa/src/statlist.h"
#include <iostream>
#include <string>

int main(int argv, char ** argc) {
  CStat s;
	try {
		printf("go to init\n");
		s.init((char*)("sts.gz+"));
        json j;
        s.to_json(j);
        std::cout << "STRING: " << j.dump() << std::endl;
	}
	catch(...){
		printf("error!");
	}
	printf("ok\n");
	return 0;
}
