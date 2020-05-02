#include "statlist.h"
#include <iostream>

void CStat::to_string(std::string & result) {
	CStatInter * cur=inter_tree;
	if (cur == NULL||!isinitialized) return;
    result = "";
	result += patch::to_string(iscomp) + ' ' + patch::to_string(nproc) + '\n';
	for (unsigned int i = 0; i < nproc; i++) {
		result += '@'+std::string(proc_info[i].node_name)+'@' + ' ';
		result += patch::to_string(proc_info[i].test_time)+'\n';
	}
	while (cur != NULL)
	{
		cur->to_string(result);
		cur = cur->next;
		if (cur) {
			result += ' ';
		}
	}
}

void CStat::to_json(json &result){
    CStatInter * cur=inter_tree;
    if (cur == NULL||!isinitialized) return;
    json proc, inter, temp;
    for (unsigned int i = 0; i < nproc; i++) {
        proc.push_back({{"node_name", std::string(proc_info[i].node_name)},
                        {"test_time", (proc_info[i].test_time) ? proc_info[i].test_time : 0.0}});
    }
    while (cur != NULL)
    {
        cur->to_json(temp);
        inter.push_back(temp);
        cur = cur->next;
    }
    result = {{"iscomp", iscomp}, {"nproc", nproc}, {"p_heading", std::string(p_heading)}, {"proc", proc}, {"inter", inter}};
}

CStat::CStat(json source){
    isjson = true;
    stat = NULL;
    iscomp = source["iscomp"];
    nproc = source["nproc"];
    const char *tmp = ((std::string)source["p_heading"]).c_str();
    std::cout << ">>  p_heading = " << source["p_heading"] << "  " << strlen(tmp) << "  " << tmp << std::endl;
    for (int i = 0; i < strlen(tmp); ++i)
        p_heading[i] = tmp[i];
    proc_info = new CProcInfo[nproc];
    int i = 0;
    for (json::iterator it = source["proc"].begin(); it != source["proc"].end() && i < nproc; ++it, ++i){
        const char * str = ((std::string)(*it)["node_name"]).c_str();
        proc_info[i].node_name = new char[strlen(str)];
        strcpy(proc_info[i].node_name, str);
        proc_info[i].test_time = (*it)["test_time"];
    }
    CStatInter *inter_temp = NULL;
    inter_tree = NULL;
    for (json::reverse_iterator it = source["inter"].rbegin(); it != source["inter"].rend(); ++it){
        inter_temp = inter_tree;
        inter_tree = new CStatInter((json)(*it));
//        if(inter_temp)
        inter_tree->next = inter_temp;
    }
    isinitialized = true;
}

CStat::~CStat() {
//    printf("Destructor: ~Stat()\n");
//	if (inter_tree) {
//        inter_tree->delete_tail();
//		inter_tree->clear();
//		delete inter_tree;
//	}
	//for (unsigned long i = 0; i < nproc; i++)
	//	delete [] proc_info[i].node_name;
//	if (proc_info){
//        delete [] proc_info;
//    }
//    if (stat){
//        delete stat;
//    }
}

void CStat::clear() {
	if (inter_tree) {
        inter_tree->delete_tail();
		inter_tree->clear();
		delete inter_tree;
	}
	// for (unsigned long i = 0; i < nproc; i++)
	// 	delete[] proc_info[i].node_name;
	if (proc_info)
	    delete[] proc_info;
	if (spath) {
        delete[] spath;
	}
    if (stat)
        delete stat;
}

CStat::CStat() {
    isjson = false;
    stat = NULL;
	isinitialized = false;
	nproc = 0;
	inter_tree = NULL;
	iscomp = false;
	proc_info = NULL;
	spath = NULL;
};

void CStat::init(char * path) {
    isjson = false;
	if (isinitialized) {
		err = true;
		return;
	}
	stat = new CStatRead(path, 0, 0, 0);
	int warn;
	if (stat->Valid(&warn) != TRUE) {
		err = true;
		return;
	}
	nproc = stat->QProc();
	if (nproc == 0) {
		err = true;
		return;
	}
    stat->VMSSize(p_heading);
	unsigned long n = stat->BeginTreeWalk();
	if (n != 0) inter_tree = new CStatInter(stat, n);
	proc_info = new struct CProcInfo[nproc];
	for (unsigned long i = 0; i<nproc; i++) {
		stat->NameTimeProc(i, &proc_info[i].node_name, &proc_info[i].test_time);
	}
	isinitialized = true;
	spath = new char[strlen(path) + 1];
	strcpy(spath, path);
}

CStatInter * find_inter(short type, long expr, short nlev, CStatInter * cur) {
    while (cur != NULL) {
		if (cur->id.t == type && cur->id.nlev == nlev)
		    switch (type){
		        case USER:
		            if (cur->id.expr == expr)
                        return cur;
		            break;
                default:
                    return cur;
		    }
        cur = cur->next;
	}
	return NULL;
};

CStatInter * next_inter(short nlev, CStatInter * cur) {
	if (cur == NULL) {
		return NULL;
	}
	cur = cur->next;
	while (cur != NULL && cur->id.nlev != nlev)
	{
	    if (cur->id.nlev < nlev)
	        return NULL;
		cur = cur->next;
	}
	return cur;
};

int copy_for_compare(const CStat &s, CStat &r){
    if (!s.isinitialized)
        return 1;
    if (!s.isjson) {
        r.spath = new char[(strlen(s.spath)) + 1];
        strcpy(r.spath, s.spath);
    }
    r.nproc = s.nproc;
    strcpy(r.p_heading, s.p_heading);
    r.proc_info = new CProcInfo[r.nproc];
    for (int i = 0; i < r.nproc; ++i){
        r.proc_info[i].node_name = new char[strlen(s.proc_info[i].node_name)];
        strcpy(r.proc_info[i].node_name, s.proc_info[i].node_name);
        r.proc_info->test_time = s.proc_info->test_time;
    }
    r.isinitialized = true;
    r.iscomp = true;
    return 0;
}

void stat_intersect(const CStat &s1, const CStat &s2, CStat & r1, CStat & r2) {
	if (copy_for_compare(s1, r1) || copy_for_compare(s2, r2))
	    return;
	inter_tree_intersect(s1.inter_tree, s2.inter_tree, &r1.inter_tree, &r2.inter_tree);
};

void skip_to_end(CStatInter ***i){
    while ((**i)->next != NULL)
        *i = &(**i)->next;
}

void inter_tree_intersect(CStatInter *i1, CStatInter *i2, CStatInter **r1, CStatInter **r2) {
    std::cout << "In inter_tree_intersect\n";
	CStatInter *cur;
	if (!i1 || !i2)
	    return;
	short cur_lev = i1->id.nlev;
	while (i1 != NULL && i2 != NULL) {
	    std::cout << "Going to find_inter: " << i1->id.expr << "  " << cur_lev  << " " << i1->id.t << " " << i1->id.nline << std::endl;
		if (cur = find_inter(i1->id.t, i1->id.expr, cur_lev, i2)) {
            std::cout << "Find_inter: " << cur->id.expr << "  " << cur->id.nlev << " " << cur->id.nline << std::endl;
            *r1 = new CStatInter(*i1);
			*r2 = new CStatInter(*cur);
			r1 = &(*r1)->next;
			r2 = &(*r2)->next;
            if (i1->next != NULL && cur->next != NULL && i1->next->id.nlev > cur_lev && cur->next->id.nlev > cur_lev) {
                inter_tree_intersect(i1->next, cur->next, r1, r2);
                if (*r1 != NULL && *r2 != NULL){
                    skip_to_end(&r1);
                    skip_to_end(&r2);
                    r1 = &(*r1)->next;
                    r2 = &(*r2)->next;
                }
            }
            i2 = next_inter(cur_lev, cur);
        }
		i1 = next_inter(cur_lev, i1);
//		json j;
//
//		if (i1 != NULL){
//            i1->to_json(j);
//		    std::cout << ">> next_inter:  " << i1->id.nlev << "\n\n" << j << "\n\n";
//        }
//		else
//            std::cout << ">> next_inter:  " << i1 << "\n\n" << j << "\n\n";
	}
//	std::cout << "inter_tree_intersect OK " << cur_lev << std::endl;
 }
