#include "statlist.h"
#include <iostream>

//---  Not relevant ‣ Use to_json  ---//
void CStatInter::to_string(std::string &result) {
	result += "@interval@ ";
	result += patch::to_string(id.nlev) + ' ';   //уровень вложенности
	result += patch::to_string(id.t) + ' ';      //тип интервала
	result += patch::to_string(id.expr) + ' ';   //значение выражения
	result += patch::to_string(id.nline) + ' ';  //номер строки начала
	result += patch::to_string(id.nline_end) + ' '; //номер строки конца
	result += patch::to_string(id.nenter) + ' '; //число вхождений в интервал
	result += std::string(id.pname) + ' ';
    result += "\n@times@ ";
	result += patch::to_string(prod_cpu)+' ';
	result += patch::to_string(prod_sys) + ' ';
	result += patch::to_string(prod_io) + ' ';
	result += patch::to_string(exec_time) + ' ';
	result += patch::to_string(sys_time) + ' ';
	result += patch::to_string(real_comm) + ' ';
	result += patch::to_string(efficiency) + ' ';
	result += patch::to_string(lost_time) + ' ';
	result += patch::to_string(insuf_user) + ' ';
	result += patch::to_string(insuf_sys) + ' ';
	result += patch::to_string(comm) + ' ';
	result += patch::to_string(real_comm) + ' ';
	result += patch::to_string(comm_start) + ' ';
	result += patch::to_string(idle) + ' ';
	result += patch::to_string(load_imb) + ' ';
	result += patch::to_string(synch) + ' ';
	result += patch::to_string(time_var) + ' ';
	result += patch::to_string(overlap) + ' ';
	result += patch::to_string(thr_user_time) + ' ';
	result += patch::to_string(thr_sys_time) + ' ';
	result += patch::to_string(gpu_time_prod) + ' ';
	result += patch::to_string(gpu_time_lost) + ' ';
	result += patch::to_string(nproc)+' ';
	result += patch::to_string(threadsOfAllProcs)+' ';
    result += "@end_times@\n";
    for (unsigned int i = 0; i < RED; i++) {
        result+= patch::to_string(col_op[i].ncall)+' ';
        result += patch::to_string(col_op[i].comm) + ' ';
        result += patch::to_string(col_op[i].synch) + ' ';
        result += patch::to_string(col_op[i].real_comm) + ' ';
        result += patch::to_string(col_op[i].time_var) + ' ';
        result += patch::to_string(col_op[i].overlap) + ' ';
    }
    if (!isjson){
        for (unsigned int j = 0; j < nproc; j++) {
            for (int i = 0; i <= StatGrpCount; i++) {
                result += patch::to_string(op_group[j][i].calls) + ' ';
                result += patch::to_string(op_group[j][i].prod) + ' ';
                result += patch::to_string(op_group[j][i].lost_time) + ' ';
            }
        }
        result += "\n@proc@\n";
        result += patch::to_string(id.proc) + '\n';   //кол-во процессов
        for (unsigned int i = 0; i < nproc; i++) {
            result += "@proc" + patch::to_string(i) + "@ ";
            result += patch::to_string(proc_times[i].prod_cpu) + ' ';
            result += patch::to_string(proc_times[i].prod_sys) + ' ';
            result += patch::to_string(proc_times[i].prod_io) + ' ';
            result += patch::to_string(proc_times[i].exec_time) + ' ';
            result += patch::to_string(proc_times[i].sys_time) + ' ';
            result += patch::to_string(proc_times[i].real_comm) + ' ';
            result += patch::to_string(proc_times[i].lost_time) + ' ';
            result += patch::to_string(proc_times[i].insuf_user) + ' ';
            result += patch::to_string(proc_times[i].insuf_sys) + ' ';
            result += patch::to_string(proc_times[i].comm) + ' ';
            result += patch::to_string(proc_times[i].idle) + ' ';
            result += patch::to_string(proc_times[i].load_imb) + ' ';
            result += patch::to_string(proc_times[i].synch) + ' ';
            result += patch::to_string(proc_times[i].time_var) + ' ';
            result += patch::to_string(proc_times[i].overlap) + ' ';
            result += patch::to_string(proc_times[i].thr_user_time) + ' ';
            result += patch::to_string(proc_times[i].thr_sys_time) + ' ';
            result += patch::to_string(proc_times[i].gpu_time_prod) + ' ';
            result += patch::to_string(proc_times[i].gpu_time_lost) + ' ';
            result += "\n@treads@\n";
            result += patch::to_string(proc_times[i].num_threads) + ' ';
            for (unsigned int j = 0; j < proc_times[i].num_threads;j++) {
                result += "@tread" + patch::to_string(j) + "@ ";
                result += patch::to_string(proc_times[i].th_times[j].sys_time) + ' ';
                result += patch::to_string(proc_times[i].th_times[j].user_time) + ' ';
                result += "@end_tread" + patch::to_string(j) + "@ ";
            }
            result += "\n@end_treads@\n";
            result += "@gpu@\n";
            result += patch::to_string(proc_times[i].num_gpu) + ' ';
            for (unsigned int j = 0; j < proc_times[i].num_gpu; j++) {
                result +="@gpu"+ patch::to_string(j) + "_"
                    + std::string(proc_times[i].gpu_times[j].gpu_name) + '@' + ' ';
                result += patch::to_string(proc_times[i].gpu_times[j].prod_time)+' ';
                result += patch::to_string(proc_times[i].gpu_times[j].lost_time) + ' ';
//                result += patch::to_string(proc_times[i].gpu_times[j].kernel_exec) + ' ';
//                result += patch::to_string(proc_times[i].gpu_times[j].loop_exec) + ' ';
//                result += patch::to_string(proc_times[i].gpu_times[j].get_actual) + ' ';
//                result += patch::to_string(proc_times[i].gpu_times[j].data_reorg) + ' ';
//                result += patch::to_string(proc_times[i].gpu_times[j].reduction) + ' ';
//                result += patch::to_string(proc_times[i].gpu_times[j].gpu_runtime_compilation) + ' ';
//                result += patch::to_string(proc_times[i].gpu_times[j].gpu_to_cpu) + ' ';
//                result += patch::to_string(proc_times[i].gpu_times[j].cpu_to_gpu) + ' ';
//                result += patch::to_string(proc_times[i].gpu_times[j].gpu_to_gpu)+ ' ';
//                for (unsigned int k = 0; k < GNUMOP; k++) {
//                    result += patch::to_string(proc_times[i].gpu_times[j].op_times[k].cpu_to_gpu) + ' ';
//                    result += patch::to_string(proc_times[i].gpu_times[j].op_times[k].gpu_to_gpu) + ' ';
//                    result += patch::to_string(proc_times[i].gpu_times[j].op_times[k].gpu_to_cpu) + ' ';
//                }
                result += "@end_gpu" + patch::to_string(j) + "@";
            }
            result += "\n@end_gpu@\n";

            // Col_op
            // for (int i = 0; i < 4; i++) {
            // 	result += patch::to_string(col_op[i].comm) + ' ';
            // 	result += patch::to_string(col_op[i].ncall) + ' ';
            // 	result += patch::to_string(col_op[i].overlap) + ' ';
            // 	result += patch::to_string(col_op[i].real_comm) + ' ';
            // 	result += patch::to_string(col_op[i].synch) + ' ';
            // 	result += patch::to_string(col_op[i].time_var) + ' ';
            // }
            result += "@end_proc" + patch::to_string(i) + "@\n";
        }
        result += "@end_proc@\n";
    }
    result += "@end_interval@\n";
}

void CStatInter::to_json(json & result){
    json col_op_json, proc_times_json;
    for (int i = 0; i < RED; i++) {
        col_op_json.push_back(
                {
                        {"ncall", col_op[i].ncall},
                        {"comm", col_op[i].comm},
                        {"real_comm", col_op[i].real_comm},
                        {"synch", col_op[i].synch},
                        {"time_var", col_op[i].time_var},
                        {"overlap", col_op[i].overlap}
                });
    }
    for (unsigned int i = 0; i < nproc; i++) {
        json th_times_json, gpu_times_json;
        // ---  treads  ---
//        std::cout << ">> th_times_json\n";
        for (unsigned int j = 0; j < proc_times[i].num_threads; ++j){
            th_times_json.push_back(
                    {
                        {"sys_time", proc_times[i].th_times[j].sys_time},
                        {"user_time", proc_times[i].th_times[j].user_time}
                    });
        }
        //  ---  GPU  ---
        for (unsigned int j = 0; j < proc_times[i].num_gpu; j++) {
            json metrics_json;
//            std::cout << ">> metrics_json\n";
            for (unsigned int k = 0; k < DVMH_STAT_METRIC_CNT; ++k) {
                metrics_json.push_back(
                        {
                            {"countMeasures", proc_times[i].gpu_times[j].metrics[k].countMeasures},
                            {"timeProductive", proc_times[i].gpu_times[j].metrics[k].timeProductive},
                            {"timeLost", proc_times[i].gpu_times[j].metrics[k].timeLost},
                            {"min", proc_times[i].gpu_times[j].metrics[k].min},
                            {"mean", proc_times[i].gpu_times[j].metrics[k].mean},
                            {"max", proc_times[i].gpu_times[j].metrics[k].max},
                            {"sum", proc_times[i].gpu_times[j].metrics[k].sum}
                        });
            }
//            std::cout << ">> gpu_times_json\n";
            gpu_times_json.push_back(
                    {
                        {"gpu_name", proc_times[i].gpu_times[j].gpu_name},
                        {"prod_time", proc_times[i].gpu_times[j].prod_time},
                        {"lost_time", proc_times[i].gpu_times[j].lost_time},
                        {"metrics", metrics_json}
                    });
        }
        //  ---  processors  ---
//        std::cout << ">> proc_times_json\n";
        proc_times_json.push_back(
                {
                        {"prod_cpu",      proc_times[i].prod_cpu},
                        {"prod_sys",      proc_times[i].prod_sys},
                        {"prod_io",       proc_times[i].prod_io},
                        {"exec_time",     proc_times[i].exec_time},
                        {"sys_time",      proc_times[i].sys_time},
                        {"real_comm",     proc_times[i].real_comm},
                        {"lost_time",     proc_times[i].lost_time},
                        {"insuf_user",    proc_times[i].insuf_user},
                        {"insuf_sys",     proc_times[i].insuf_sys},
                        {"comm",          proc_times[i].comm},
                        {"idle",          proc_times[i].idle},
                        {"load_imb",      proc_times[i].load_imb},
                        {"synch",         proc_times[i].synch},
                        {"time_var",      proc_times[i].time_var},
                        {"overlap",       proc_times[i].overlap},
                        {"thr_user_time", proc_times[i].thr_user_time},
                        {"thr_sys_time",  proc_times[i].thr_sys_time},
                        {"gpu_time_prod", proc_times[i].gpu_time_prod},
                        {"gpu_time_lost", proc_times[i].gpu_time_lost},
                        {"num_threads",   proc_times[i].num_threads},
                        {"num_gpu", proc_times[i].num_gpu},
                        {"th_times", th_times_json},
                        {"gpu_times", gpu_times_json}
                });
    }
//    std::cout << ">> New JSON OK\n";

    result = {
                {"id",
                    {
                        {"nlev", id.nlev},
                        {"t", id.t},
                        {"expr", id.expr},
                        {"nline", id.nline},
                        {"nline_end", id.nline_end},
                        {"nenter", id.nenter},
                        {"pname", id.pname}
                    }
                },
                {"times",
                    {
                        {"prod_cpu", prod_cpu},
                        {"prod_sys", prod_sys},
                        {"prod_io", prod_io},
                        {"exec_time", exec_time},
                        {"sys_time", sys_time},
                        {"efficiency", efficiency},
                        {"lost_time", lost_time},
                        {"insuf", insuf},
                        {"insuf_user", insuf_user},
                        {"insuf_sys", insuf_sys},
                        {"comm", comm},
                        {"real_comm", real_comm},
                        {"comm_start", comm_start},
                        {"idle", idle},
                        {"load_imb", load_imb},
                        {"synch", synch},
                        {"time_var", time_var},
                        {"overlap", overlap},
                        {"thr_user_time", thr_user_time},
                        {"thr_sys_time", thr_sys_time},
                        {"gpu_time_prod", gpu_time_prod},
                        {"gpu_time_lost", gpu_time_lost},
                        {"nproc", nproc},
                        {"threadsOfAllProcs", threadsOfAllProcs}
                    }
                },
                {"col_op", col_op_json},
                {"proc_times", proc_times_json}
             };
}

CStatInter::CStatInter(json source){
    isjson = true;
    // -----  id  -----
    json j_id = source["id"];
    id.nenter = j_id["nenter"];
    id.nline_end = j_id["nline_end"];
    id.nline = j_id["nline"];
    id.expr = j_id["expr"];
    id.nlev = j_id["nlev"];
    id.t = j_id["t"];
    std::string tmp = std::string(j_id["pname"]);
    id.pname = new char[tmp.length() + 1];
    for (int i = 0; i < tmp.length(); ++i)
        id.pname[i] = tmp[i];
    id.pname[tmp.length()] = '\0';

    // -----  times  -----
    json j_times = source["times"];
    prod_cpu = j_times["prod_cpu"];
    prod_sys = j_times["prod_sys"];
    prod_io = j_times["prod_io"];
    exec_time = j_times["exec_time"];
    sys_time = j_times["sys_time"];
    efficiency = j_times["efficiency"];
    lost_time = j_times["lost_time"];
    insuf = j_times["insuf"];
    insuf_user = j_times["insuf_user"];
    insuf_sys = j_times["insuf_sys"];
    comm = j_times["comm"];
    real_comm = j_times["real_comm"];
    comm_start = j_times["comm_start"];
    idle = j_times["idle"];
    load_imb = j_times["load_imb"];
    synch = j_times["synch"];
    time_var = j_times["time_var"];
    overlap = j_times["overlap"];
    thr_user_time = j_times["thr_user_time"];
    thr_sys_time = j_times["thr_sys_time"];
    gpu_time_prod = j_times["gpu_time_prod"];
    gpu_time_lost = j_times["gpu_time_lost"];
    nproc = j_times["nproc"];
    threadsOfAllProcs = j_times["threadsOfAllProcs"];

    // -----  col_op  -----
    if (source.contains("col_op")) {
        json j_col_op = source["col_op"];
        for (int i = 0; i < RED; ++i) {
            col_op[i].ncall = j_col_op[i]["ncall"];
            col_op[i].comm = j_col_op[i]["comm"];
            col_op[i].real_comm = j_col_op[i]["real_comm"];
            col_op[i].synch = j_col_op[i]["synch"];
            col_op[i].time_var = j_col_op[i]["time_var"];
            col_op[i].overlap = j_col_op[i]["overlap"];
        }
    }
//    std::cout << ">>  Going to proc_times\n";
    // -----  proc_times  -----
    if (source.contains("proc_times")){
        proc_times = new struct ProcTimes[nproc];
        json proc_times_json = source["proc_times"];
        for (int i = 0; i < nproc; ++i){
            json th_times_json = proc_times_json[i]["th_times"],
                 gpu_times_json = proc_times_json[i]["gpu_times"];
            proc_times[i].prod_cpu = proc_times_json[i]["prod_cpu"];
            proc_times[i].prod_sys = proc_times_json[i]["prod_sys"];
            proc_times[i].prod_io = proc_times_json[i]["prod_io"];
            proc_times[i].exec_time = proc_times_json[i]["exec_time"];
            proc_times[i].sys_time = proc_times_json[i]["sys_time"];
            proc_times[i].real_comm = proc_times_json[i]["real_comm"];
            proc_times[i].lost_time = proc_times_json[i]["lost_time"];
            proc_times[i].insuf_user = proc_times_json[i]["insuf_user"];
            proc_times[i].insuf_sys = proc_times_json[i]["insuf_sys"];
            proc_times[i].comm = proc_times_json[i]["comm"];
            proc_times[i].idle = proc_times_json[i]["idle"];
            proc_times[i].load_imb = proc_times_json[i]["load_imb"];
            proc_times[i].synch = proc_times_json[i]["synch"];
            proc_times[i].time_var = proc_times_json[i]["time_var"];
            proc_times[i].overlap = proc_times_json[i]["overlap"];
            proc_times[i].thr_user_time = proc_times_json[i]["thr_user_time"];
            proc_times[i].thr_sys_time = proc_times_json[i]["thr_sys_time"];
            proc_times[i].gpu_time_prod = proc_times_json[i]["gpu_time_prod"];
            proc_times[i].gpu_time_lost = proc_times_json[i]["gpu_time_lost"];
            proc_times[i].num_threads = proc_times_json[i]["num_threads"];
            proc_times[i].num_gpu = proc_times_json[i]["num_gpu"];
//            std::cout << ">>  NumGPU  -  " << proc_times[i].num_gpu << std::endl;
            proc_times[i].th_times = new struct ThreadTime[proc_times[i].num_threads];
            for (int j = 0; j < proc_times[i].num_threads; ++j){
                proc_times[i].th_times[j].user_time = th_times_json[j]["user_time"];
                proc_times[i].th_times[j].sys_time = th_times_json[j]["sys_time"];
            }
            proc_times[i].gpu_times = new struct GpuTime[proc_times[i].num_gpu];
            for (int j = 0; j < proc_times[i].num_gpu; ++j){
                tmp = std::string(gpu_times_json[j]["gpu_name"]);
                proc_times[i].gpu_times[j].gpu_name = new char[tmp.length() + 1];
                for (int k = 0; k < tmp.length(); ++k)
                    proc_times[i].gpu_times[j].gpu_name[k] = tmp[k];
                proc_times[i].gpu_times[j].gpu_name[tmp.length()] = '\0';
                proc_times[i].gpu_times[j].prod_time = gpu_times_json[j]["prod_time"];
                proc_times[i].gpu_times[j].lost_time = gpu_times_json[j]["lost_time"];

//                std::cout << ">>  Odl times OK\n";

                json metrics_json = gpu_times_json[j]["metrics"];

//                std::cout << ">>  metrics_json = " << metrics_json << std::endl;

                for (int m = 0; m < DVMH_STAT_METRIC_CNT; ++m){
                    proc_times[i].gpu_times[j].metrics[m].countMeasures = metrics_json[m]["countMeasures"];
                    proc_times[i].gpu_times[j].metrics[m].timeProductive = metrics_json[m]["timeProductive"];
                    proc_times[i].gpu_times[j].metrics[m].timeLost = metrics_json[m]["timeLost"];
                    proc_times[i].gpu_times[j].metrics[m].min = metrics_json[m]["min"];
                    proc_times[i].gpu_times[j].metrics[m].mean = metrics_json[m]["mean"];
                    proc_times[i].gpu_times[j].metrics[m].max = metrics_json[m]["max"];
                    proc_times[i].gpu_times[j].metrics[m].sum = metrics_json[m]["sum"];
                }
            }
        }
    }

    next = NULL;
}

CStatInter::CStatInter(const CStatInter & si) {
    std::cout << ">> In CStatInter(si)\n";
    isjson = si.isjson;
	id.t = si.id.t;
	id.nlev = si.id.nlev;
	id.expr = si.id.expr;
	id.nline = si.id.nline;
	id.nline_end = si.id.nline_end;
	id.proc = si.id.proc;
	id.nenter = si.id.nenter;
	if (si.id.pname) {
		id.pname = new char[strlen(si.id.pname) + 1];
		strcpy(id.pname, si.id.pname);
	}
	else {
		id.pname = new char [2];
		id.pname[0] = '0';
		id.pname[1] = '\0';
	}
	prod_cpu=si.prod_cpu;
	prod_sys = si.prod_sys;
	prod_io= si.prod_io;
	prod = si.prod;
	exec_time=si.exec_time;
	sys_time=si.sys_time;
	efficiency=si.efficiency;
	lost_time=si.lost_time;
	insuf=si.insuf;
	insuf_user=si.insuf_user;
	insuf_sys=si.insuf_sys;
	comm=si.comm;
	real_comm=si.real_comm;
	comm_start=si.comm_start;
	idle=si.idle;
	load_imb=si.load_imb;
	synch=si.synch;
	time_var=si.time_var;
	overlap=si.overlap;
	thr_user_time=si.thr_user_time;
	thr_sys_time=si.thr_sys_time;
	gpu_time_prod=si.gpu_time_prod;
	gpu_time_lost=si.gpu_time_lost;
	nproc=si.nproc;
	threadsOfAllProcs=si.threadsOfAllProcs;
    for (int i = 0; i < RED; i++) {
        col_op[i] = si.col_op[i];
    }
//    std::cout << ">> Prev OK\n";
    if (!isjson) {
        op_group = new OpGrp[nproc][StatGrpCount];
        //	for (int i = 0; i < nproc; ++i)
        //        op_group[i] = new OpGrp[StatGrpCount];

        for (unsigned long j = 0; j < nproc; j++) {
            for (int i = 0; i < StatGrpCount; i++) {
                op_group[j][i].calls = si.op_group[j][i].calls;
                op_group[j][i].prod = si.op_group[j][i].prod;
                op_group[j][i].lost_time = op_group[j][i].lost_time;
            }
        }
    }
//    std::cout << ">> Going to ProcTimes  -  nproc = " << nproc << std::endl;

    proc_times = new struct ProcTimes[nproc];
    for (unsigned long i = 0; i < nproc; ++i) {
        proc_times[i].prod_cpu = si.proc_times[i].prod_cpu;
        proc_times[i].prod_sys = si.proc_times[i].prod_sys;
        proc_times[i].prod_io = si.proc_times[i].prod_io;
        proc_times[i].exec_time = si.proc_times[i].exec_time;
        proc_times[i].sys_time = si.proc_times[i].sys_time;
        proc_times[i].real_comm = si.proc_times[i].real_comm;
        proc_times[i].lost_time = si.proc_times[i].lost_time;
        proc_times[i].insuf_user = si.proc_times[i].insuf_user;
        proc_times[i].insuf_sys = si.proc_times[i].insuf_sys;
        proc_times[i].comm = si.proc_times[i].comm;
        proc_times[i].idle = si.proc_times[i].idle;
        proc_times[i].load_imb = si.proc_times[i].load_imb;
        proc_times[i].synch = si.proc_times[i].synch;
        proc_times[i].time_var = si.proc_times[i].time_var;
        proc_times[i].overlap = si.proc_times[i].overlap;
        proc_times[i].thr_user_time = si.proc_times[i].thr_user_time;
        proc_times[i].thr_sys_time = si.proc_times[i].thr_sys_time;
        proc_times[i].gpu_time_prod = si.proc_times[i].gpu_time_prod;
        proc_times[i].gpu_time_lost = si.proc_times[i].gpu_time_lost;
        proc_times[i].num_threads = si.proc_times[i].num_threads;
        std::cout << ">> Going to ThreadTime\n";
        proc_times[i].th_times = new struct ThreadTime[proc_times[i].num_threads];
        for (unsigned long j = 0; j < proc_times[i].num_threads; j++) {
            proc_times[i].th_times[j].user_time = si.proc_times[i].th_times[j].user_time;
            proc_times[i].th_times[j].sys_time = si.proc_times[i].th_times[j].sys_time;
        }
        proc_times[i].num_gpu = si.proc_times[i].num_gpu;
//        std::cout << ">> Going to GpuTime  -  num_gpu = " << proc_times[i].num_gpu << std::endl;
        proc_times[i].gpu_times = new struct GpuTime[proc_times[i].num_gpu];
        for (unsigned int j = 0; j < proc_times[i].num_gpu; j++) {
//            std::cout << ">> Name = " << si.proc_times[i].gpu_times[j].gpu_name << std::endl;
            if (si.proc_times[i].gpu_times[j].gpu_name) {
                proc_times[i].gpu_times[j].gpu_name = new char[strlen(si.proc_times[i].gpu_times[j].gpu_name) + 1];
                strcpy(proc_times[i].gpu_times[j].gpu_name, si.proc_times[i].gpu_times[j].gpu_name);
            } else {
                proc_times[i].gpu_times[j].gpu_name = new char[2];
                proc_times[i].gpu_times[j].gpu_name[0] = '0';
                proc_times[i].gpu_times[j].gpu_name[1] = '\0';
            }
//            std::cout << ">> Name OK\n";
            proc_times[i].gpu_times[j].prod_time = si.proc_times[i].gpu_times[j].prod_time;
            proc_times[i].gpu_times[j].lost_time = si.proc_times[i].gpu_times[j].lost_time;
            for (int m = 0; m < DVMH_STAT_METRIC_CNT; ++m) {
                proc_times[i].gpu_times[j].metrics[m] = si.proc_times[i].gpu_times[j].metrics[m];
            }
        }
    }
	next = NULL;
    std::cout << ">>  CStatInter(si)  OK\n";
}

void CStatInter::clear() {
    if (!isjson){
        delete [] op_group;
        for (unsigned int i = 0; i < nproc; i++) {
            if (proc_times[i].num_threads)
                delete [] proc_times[i].th_times;
            for (unsigned int j = 0; j < proc_times[i].num_gpu; j++)
                if (proc_times[i].gpu_times[j].gpu_name)
                    delete [] proc_times[i].gpu_times[j].gpu_name;
            if (proc_times[i].num_gpu)
                delete [] proc_times[i].gpu_times;
        }
    }
}

void CStatInter::delete_tail() {
	if (next != NULL) {
		next->delete_tail();
        clear();
        delete next;
	}
}

CStatInter::~CStatInter() {
    printf("Destructor: ~CStatInter()\n");
	//delete_tail();
	//clear();
}

//---  Copy dvmhStatMetric to metric in CStatInter  ---//
void init_gpu_metric(GpuMetric &metric, dvmh_stat_interval_gpu_metric *dvmhStatMetric){
    metric.countMeasures = dvmhStatMetric->countMeasures;
    metric.max = dvmhStatMetric->max;
    metric.mean = dvmhStatMetric->mean;
    metric.min = dvmhStatMetric->min;
    metric.sum = dvmhStatMetric->sum;
    metric.timeLost = dvmhStatMetric->timeLost;
    metric.timeProductive = dvmhStatMetric->timeProductive;
}

CStatInter::CStatInter(CStatRead * stat, int n)
{
    isjson = false;
	printf("begin init %d\n", n);
	qproc = stat->QProc();
	//������ ��������� �������� ���������
	stat->ReadIdent(&id);
	// string for processor characteristics - max
	double min[ITER + 1];
	double max[ITER + 1];
	double sum[ITER + 1];
	// communication
	double minc[RED + 1];
	double maxc[RED + 1];
	double sumc[RED + 1];
	// real communication
	double minrc[RED + 1];
	double maxrc[RED + 1];
	double sumrc[RED + 1];
	// synchronization
	double mins[RED + 1];
	double maxs[RED + 1];
	double sums[RED + 1];
	// variation
	double minv[RED + 1];
	double maxv[RED + 1];
	double sumv[RED + 1];
	// overlap
	double minov[RED + 1];
	double maxov[RED + 1];
	double sumov[RED + 1];
	// number of processor
	unsigned long nprocmin[ITER + 1], nprocmax[ITER + 1];
	unsigned long nprocminc[RED + 1], nprocmaxc[RED + 1];
	unsigned long nprocminrc[RED + 1], nprocmaxrc[RED + 1];
	unsigned long nprocmins[RED + 1], nprocmaxs[RED + 1];
	unsigned long nprocminv[RED + 1], nprocmaxv[RED + 1];
	unsigned long nprocminov[RED + 1], nprocmaxov[RED + 1];
	stat->MinMaxSum(PRGEN, min, nprocmin, max, nprocmax, sum);
	stat->MinMaxSum(PRCOM, minc, nprocminc, maxc, nprocmaxc, sumc);
	stat->MinMaxSum(PRRCOM, minrc, nprocminrc, maxrc, nprocmaxrc, sumrc);
	stat->MinMaxSum(PRSYN, mins, nprocmins, maxs, nprocmaxs, sums);
	stat->MinMaxSum(PRVAR, minv, nprocminv, maxv, nprocmaxv, sumv);
	stat->MinMaxSum(PROVER, minov, nprocminov, maxov, nprocmaxov, sumov);
	printf("main charecteristics\n");
	//������ �������� ��������������
	nproc = n;
	prod_cpu = sum[CPUUSR];
	prod_sys = sum[CPU];
	prod_io = sum[IOTIME];
	prod = prod_cpu + prod_sys + prod_io;
	exec_time = max[EXEC];
	sys_time = nproc * max[EXEC];
	efficiency = sys_time ? prod / sys_time : 0.0;
	threadsOfAllProcs = 0;
	for (unsigned int not_i = 1; not_i <= nproc; ++not_i)
		threadsOfAllProcs += stat->getThreadsAmountByProcess(not_i);
	lost_time = sum[LOST];
	insuf = sum[INSUFUSR] + sum[INSUF];
	insuf_user = sum[INSUFUSR];
	insuf_sys = sum[INSUF];
	comm = sum[SUMCOM];
	real_comm = sum[SUMRCOM];
	comm_start = sum[START];
	idle = sum[IDLE];
	load_imb = sum[IMB];
	synch = sum[SUMSYN];
	time_var = sum[SUMVAR];
	overlap = sum[SUMOVERLAP];
	thr_user_time = sum[DVMH_THREADS_USER_TIME];
	thr_sys_time = sum[DVMH_THREADS_SYSTEM_TIME];
	gpu_time_prod = sum[DVMH_GPU_TIME_PRODUCTIVE];
	gpu_time_lost = sum[DVMH_GPU_TIME_LOST];
	for (int i = 0; i < RED; i++) {
		col_op[i].ncall = stat->ReadCall(typecom(i));
		col_op[i].comm = sumc[i];
		col_op[i].real_comm = sumrc[i];
		col_op[i].synch = sums[i];
		col_op[i].time_var = sumv[i];
		col_op[i].overlap = sumov[i];
	}
	//return;

    op_group = new OpGrp[qproc][StatGrpCount];
//    for (int i = 0; i < qproc; ++i)
//        op_group[i] = new OpGrp[StatGrpCount];
	printf("group\n");
	for (unsigned long j = 0; j < qproc; j++) {
		double prod[StatGrpCount], lost[StatGrpCount], sumprod = 0.0, sumlost = 0.0;
		double calls[StatGrpCount], sumcalls = 0.0;
		printf("stat call\n");
		stat->GrpTimes(prod, lost, calls, j + 1);
		printf("begin loop\n");
		for (int i = 0; i < StatGrpCount - 1; i++) {
			sumprod = sumprod + prod[i];
			sumlost = sumlost + lost[i];
			sumcalls = sumcalls + calls[i];
			op_group[j][i].calls = calls[i];
			op_group[j][i].prod = prod[i];
			op_group[j][i].lost_time = lost[i];
		}
		op_group[j][StatGrpCount - 1].calls = sumcalls;
		op_group[j][StatGrpCount - 1].prod = sumprod;
		op_group[j][StatGrpCount - 1].lost_time = sumlost;
	}
	printf("proc %d\n", qproc);
	proc_times = new struct ProcTimes[qproc];
	printf("struct\n");
	stat->ReadProcS(proc_times);
	printf("procs call\n");
	for (unsigned long np = 1; np <= qproc; ++np)
	{
		proc_times[np-1].num_threads = 0;
		proc_times[np - 1].th_times = NULL;
		dvmh_stat_interval        *dvmhStatInterval = stat->getDvmhStatIntervalByProcess(np);
		if (!dvmhStatInterval) {
			continue;
		}
		if (!dvmhStatInterval->threadsUsed)
			continue;
		proc_times[np - 1].num_threads = stat->getThreadsAmountByProcess(np);
		proc_times[np - 1].th_times = new struct ThreadTime[proc_times[np - 1].num_threads];
		for (unsigned long i = 0; i < proc_times[np - 1].num_threads; ++i)
		{
			proc_times[np - 1].th_times[i].user_time = dvmhStatInterval->threads[i].user_time;
			proc_times[np - 1].th_times[i].sys_time = dvmhStatInterval->threads[i].system_time;
		}
	}
	printf("gpu\n");
	for (unsigned long np = 1; np <= qproc; ++np)
	{
		dvmh_stat_interval        *dvmhStatInterval = stat->getDvmhStatIntervalByProcess(np);
		dvmh_stat_header_gpu_info *dvmhStatGpuInfo;
		if (!dvmhStatInterval) {
			continue;
		}
		proc_times[np - 1].gpu_times = new struct GpuTime[DVMH_STAT_MAX_GPU_CNT];
		proc_times[np - 1].num_gpu = 0;
		for (int gpu = 0; gpu < DVMH_STAT_MAX_GPU_CNT; ++gpu) {
			if (((dvmhStatInterval->mask >> gpu) & 1) == 0) continue;
			proc_times[np - 1].num_gpu++;
			dvmhStatGpuInfo = stat->getDvmhStatGpuInfoByProcess(np, gpu);
			dvmh_stat_interval_gpu *dvmhStatGpu = &dvmhStatInterval->gpu[gpu];
			if ((char *)dvmhStatGpuInfo->name) {
				int lenName = strlen((char *)dvmhStatGpuInfo->name);
				proc_times[np - 1].gpu_times[proc_times[np - 1].num_gpu - 1].gpu_name = new char[lenName + 1];
				strcpy(proc_times[np - 1].gpu_times[proc_times[np - 1].num_gpu - 1].gpu_name, (char *)dvmhStatGpuInfo->name);
			}
			else {
				proc_times[np - 1].gpu_times[proc_times[np - 1].num_gpu - 1].gpu_name = new char[2];
				proc_times[np - 1].gpu_times[proc_times[np - 1].num_gpu - 1].gpu_name[0]='0';
				proc_times[np - 1].gpu_times[proc_times[np - 1].num_gpu - 1].gpu_name[1] = '\0';
			}
			//--- Init times  ---//
            dvmh_stat_interval_gpu_metric *dvmhStatMetric;
			for (int m = 0; m < DVMH_STAT_METRIC_CNT; ++m)
                init_gpu_metric(proc_times[np - 1].gpu_times[proc_times[np - 1].num_gpu - 1].metrics[m], &dvmhStatGpu->metrics[m]);
			proc_times[np - 1].gpu_times[proc_times[np - 1].num_gpu - 1].prod_time = dvmhStatGpu->timeProductive;
			proc_times[np - 1].gpu_times[proc_times[np - 1].num_gpu - 1].lost_time = dvmhStatGpu->timeLost;
			//if (dvmhStatMetric->countMeasures <= 0) ????;
		}
	}
	printf("next\n");
	n = stat->TreeWalk();
	printf("next %d\n", n);
	if (n != 0)
	{
		next = new CStatInter(stat, n);
	}
	else
		next = NULL;
	printf("end inter\n");
}
