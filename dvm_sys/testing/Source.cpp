#include "../tools/pppa/src/statlist.h"
#include <iostream>
#include <string>

int main(int argv, char ** argc) {
  CStat s;
	try {
	    CStat * test;
		printf("go to init\n");
		s.init((char*)("sts.gz+"));
        json j;
        s.to_json(j);
        std::string str;
        s.to_string(str);
        std::cout << std::endl << "STRING s: " << str << std::endl;
        test = new CStat(j);
        test->to_string(str);
        std::cout << std::endl << "STRING test: " << str << std::endl;
	}
	catch(...){
		printf("error!");
	}
	printf("ok\n");
	return 0;
}
