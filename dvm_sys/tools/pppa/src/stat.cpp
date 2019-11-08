#include "statlist.h"

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
//    json temp;
            //{{"iscomp", iscomp}, {"nproc", nproc}};
//    result.push_back(temp);
//    result += patch::to_string(iscomp) + ' ' + patch::to_string(nproc) + '\n';
    json proc, inter, temp;
    for (unsigned int i = 0; i < nproc; i++) {
        proc.push_back({{"node_name", std::string(proc_info[i].node_name)}, {"test_time", proc_info[i].test_time}});
//        result += '@'+std::string(proc_info[i].node_name)+'@' + ' ';
//        result += patch::to_string(proc_info[i].test_time)+'\n';
    }

    while (cur != NULL)
    {
        cur->to_json(temp);
        inter.push_back(temp);
        cur = cur->next;
//        if (cur) {
//            result += ' ';
//        }
    }
    result = {{"iscomp", iscomp}, {"nproc", nproc}, {"proc", proc}, {"inter", inter}};
}

CStat::CStat(const CStat &s) {
    stat = NULL;
	isinitialized = s.isinitialized;
	nproc = s.nproc;
	inter_tree = NULL;
	CStatInter * cur_s=s.inter_tree,** cur_this=&inter_tree;
	if (cur_s) {
		*cur_this = new CStatInter(*(cur_s));
		cur_this = &((*cur_this)->next);
		cur_s = cur_s->next;
	}
	iscomp = s.iscomp;
	proc_info = NULL;
	if (s.spath) {
		spath = new char[strlen(spath) + 1];
		strcpy(spath, s.spath);
	}
	else {
		spath = NULL;
	}
}

CStat::~CStat() {
	if (inter_tree) {
		inter_tree->delete_tail();
		inter_tree->clear();
		delete inter_tree;
	}
	//for (unsigned long i = 0; i < nproc; i++)
	//	delete [] proc_info[i].node_name;
	if (proc_info)
	    delete [] proc_info;
    if (stat)
        delete stat;
}

void CStat::clear() {
	if (inter_tree) {
		inter_tree->delete_tail();
		inter_tree->clear();
		delete inter_tree;
	}
	// for (unsigned long i = 0; i < nproc; i++)
	// 	delete[] proc_info[i].node_name;
	delete[] proc_info;
	if (spath) {
		delete[] spath;
	}
    if (stat)
        delete stat;
}

CStat::CStat() {
    stat = NULL;
	isinitialized = false;
	nproc = 0;
	inter_tree = NULL;
	iscomp = false;
	proc_info = NULL;
	spath = NULL;
};

void CStat::init(char * path) {
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

CStatInter * find_inter(long expr, short nlev, CStatInter * cur) {
	while (cur && cur->id.nlev == nlev) {
		if (cur->id.expr == expr) {
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
	while (cur && cur->id.nlev != nlev)
	{
		cur = cur->next;
	}
	return cur;
};

void stat_intersect(const CStat &s1, const CStat &s2, CStat & r1, CStat & r2) {
	if (!s1.isinitialized || !s2.isinitialized) {
		return;
	}
	r1.spath = new char[(strlen(s1.spath)) + 1];
	strcpy(r1.spath, s1.spath);
	r1.iscomp = 1;
	r1.isinitialized = 1;
	r2.spath = new char[(strlen(s2.spath)) + 1];
	strcpy(r2.spath, s2.spath);
	r2.iscomp = 1;
	r2.isinitialized = 1;
	inter_tree_intersect(s1.inter_tree, s2.inter_tree, &r1.inter_tree, &r2.inter_tree);
	return;
};

void inter_tree_intersect(CStatInter *i1, CStatInter *i2, CStatInter **r1, CStatInter **r2) {
	CStatInter *cur;
	short cur_lev = i1->id.nlev;
	while (i1 != NULL) {
		if (cur = find_inter(i1->id.expr, cur_lev, i2)) {
			*r1 = new CStatInter(*i1);
			*r2 = new CStatInter(*i2);
			r1 = &(*r1)->next;
			r2 = &(*r2)->next;
			if (i1->next->id.nlev > cur_lev && cur->next->id.nlev > cur_lev) {
				inter_tree_intersect(i1->next, cur->next, r1, r2);
			}
		}
		i1 = next_inter(cur_lev, i1);
	}
 }
