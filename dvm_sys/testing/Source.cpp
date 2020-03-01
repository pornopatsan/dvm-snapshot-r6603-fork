#include "../tools/pppa/src/statlist.h"
//#include "../tools/pppa/src/integration.h"
#include <iostream>
#include <string>

int main(int argv, char ** argc) {
  CStat s;
	try {
		printf("go to init\n");
		s.init((char*)("sts.gz+"));
        json j;
        s.to_json(j);
        char *res1, *res2;

        const char * s = "{\"inter\":[{\"id\":{\"expr\":2000000000,\"nenter\":1.0,\"nlev\":0,\"nline\":20,\"nline_end\":83,\"pname\":\"jac2d.c\",\"t\":21},\"times\":{\"comm\":0.008511781692504883,\"comm_start\":0.0,\"efficiency\":0.20376201484280726,\"exec_time\":0.2566530704498291,\"gpu_time_lost\":0.0,\"gpu_time_prod\":0.0,\"idle\":0.19022417068481445,\"insuf_sys\":0.007939696311950684,\"insuf_user\":0.6107520461082458,\"load_imb\":0.04692608118057251,\"lost_time\":0.8174276947975159,\"nproc\":4,\"overlap\":1.9550323486328125e-05,\"prod_cpu\":0.20653802156448364,\"prod_io\":0.0,\"prod_sys\":0.0026465654373168945,\"real_comm\":0.0,\"synch\":0.0051233768463134766,\"sys_time\":1.0266122817993164,\"thr_sys_time\":0.0,\"thr_user_time\":0.0,\"threadsOfAllProcs\":4,\"time_var\":0.004382610321044922}},{\"id\":{\"expr\":1,\"nenter\":13.0,\"nlev\":1,\"nline\":48,\"nline_end\":61,\"pname\":\"jac2d.c\",\"t\":23},\"times\":{\"comm\":0.005877494812011719,\"comm_start\":0.0,\"efficiency\":0.18105467470353143,\"exec_time\":0.0036389827728271484,\"gpu_time_lost\":0.0,\"gpu_time_prod\":0.0,\"idle\":0.0015106201171875,\"insuf_sys\":0.0015538930892944336,\"insuf_user\":0.002978503704071045,\"load_imb\":0.00030857324600219727,\"lost_time\":0.011920511722564697,\"nproc\":4,\"overlap\":1.239776611328125e-05,\"prod_cpu\":0.002117455005645752,\"prod_io\":0.0,\"prod_sys\":0.0005179643630981445,\"real_comm\":0.0,\"synch\":0.0026230812072753906,\"sys_time\":0.014555931091308594,\"thr_sys_time\":0.0,\"thr_user_time\":0.0,\"threadsOfAllProcs\":4,\"time_var\":0.0026237964630126953}},{\"id\":{\"expr\":11,\"nenter\":13.0,\"nlev\":2,\"nline\":49,\"nline_end\":58,\"pname\":\"jac2d.c\",\"t\":23},\"times\":{\"comm\":0.005877494812011719,\"comm_start\":0.0,\"efficiency\":0.1784580787508486,\"exec_time\":0.0035119056701660156,\"gpu_time_lost\":0.0,\"gpu_time_prod\":0.0,\"idle\":0.0015163421630859375,\"insuf_sys\":0.0012211203575134277,\"insuf_user\":0.0029257535934448242,\"load_imb\":0.00031000375747680664,\"lost_time\":0.011540710926055908,\"nproc\":4,\"overlap\":1.239776611328125e-05,\"prod_cpu\":0.0020998716354370117,\"prod_io\":0.0,\"prod_sys\":0.0004070401191711426,\"real_comm\":0.0,\"synch\":0.0026230812072753906,\"sys_time\":0.014047622680664063,\"thr_sys_time\":0.0,\"thr_user_time\":0.0,\"threadsOfAllProcs\":4,\"time_var\":0.0026237964630126953}},{\"id\":{\"expr\":12,\"nenter\":13.0,\"nlev\":2,\"nline\":59,\"nline_end\":60,\"pname\":\"jac2d.c\",\"t\":23},\"times\":{\"comm\":0.0,\"comm_start\":0.0,\"efficiency\":0.23588709677419356,\"exec_time\":5.173683166503906e-05,\"gpu_time_lost\":0.0,\"gpu_time_prod\":0.0,\"idle\":1.1682510375976563e-05,\"insuf_sys\":0.00013697147369384766,\"insuf_user\":9.47713851928711e-06,\"load_imb\":2.9206275939941406e-06,\"lost_time\":0.00015813112258911133,\"nproc\":4,\"overlap\":0.0,\"prod_cpu\":3.159046173095703e-06,\"prod_io\":0.0,\"prod_sys\":4.565715789794922e-05,\"real_comm\":0.0,\"synch\":0.0,\"sys_time\":0.00020694732666015625,\"thr_sys_time\":0.0,\"thr_user_time\":0.0,\"threadsOfAllProcs\":4,\"time_var\":0.0}},{\"id\":{\"expr\":2,\"nenter\":13.0,\"nlev\":1,\"nline\":65,\"nline_end\":70,\"pname\":\"jac2d.c\",\"t\":23},\"times\":{\"comm\":0.002634286880493164,\"comm_start\":0.0,\"efficiency\":0.2507883836957545,\"exec_time\":0.0025327205657958984,\"gpu_time_lost\":0.0,\"gpu_time_prod\":0.0,\"idle\":0.0008120536804199219,\"insuf_sys\":0.0015192031860351563,\"insuf_user\":0.0026246309280395508,\"load_imb\":0.0006643533706665039,\"lost_time\":0.007590174674987793,\"nproc\":4,\"overlap\":7.152557373046875e-06,\"prod_cpu\":0.002034306526184082,\"prod_io\":0.0,\"prod_sys\":0.0005064010620117188,\"real_comm\":0.0,\"synch\":0.002500295639038086,\"sys_time\":0.010130882263183594,\"thr_sys_time\":0.0,\"thr_user_time\":0.0,\"threadsOfAllProcs\":4,\"time_var\":0.0017588138580322266}}],\"iscomp\":false,\"nproc\":4,\"proc\":[{\"node_name\":\"MacBook-Penek.local\",\"test_time\":1.67373},{\"node_name\":\"MacBook-Penek.local\",\"test_time\":1.7022669999999998},{\"node_name\":\"MacBook-Penek.local\",\"test_time\":1.7026919999999999},{\"node_name\":\"MacBook-Penek.local\",\"test_time\":1.7122719999999998}]}";
//        std::cout << j << std::endl << std::endl;

//        if (stat_intersect_((char*) s, (char*) s, res1, res2)){
//            printf("not working ((((9 \n\n");
//            return 1;
//        }
//
//        std::cout << res1 << std::endl << std::endl << res2 << std::endl << std::endl;

	}
	catch(...){
		printf("error!");
	}
	printf("ok\n");
	return 0;
}
