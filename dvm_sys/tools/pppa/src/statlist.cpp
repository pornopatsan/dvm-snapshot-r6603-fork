#include "statlist.h"

CStatList::CStatList(char * path) {
	err = false;
	length = 0;
	lname = NULL;				//???
	stat_list = NULL;
	last = NULL;
	comp_stat1 = NULL;
	comp_stat2 = NULL;
	char buf[100];
	std::ifstream fin;
	fin.open(path);
	fin.getline(buf, 100);
	if (!fin.eof()) {
		stat_list = new struct CStatNode;
	}
	else {
		stat_list = NULL;
		last = NULL;
	}
	CStatNode * cur = stat_list, * prev = NULL;
	while (!fin.eof()) {
		cur->stat.init(buf);
		if (cur->stat.err) {
			err = true;
			clear_stat_list(stat_list);
			delete stat_list;
			return;
		}
		fin.getline(buf, 100);
		if (!fin.eof()) {
			cur->next = new struct CStatNode;
			prev = cur;
			cur = cur->next;
		}
		else {
			cur->next = NULL;
		}
	}
	if (prev) {
		last = &prev->next;
	}
	else {
		last = NULL;
	}

};

CStatList::CStatList(int n, char** paths) {
	length = n;
	lname = NULL;				//???
	if (n > 0) {
		stat_list = new struct CStatNode;
		CStatNode * cur = stat_list, * prev = NULL;
		for (; n > 0; n--) {
			cur->stat.init(paths[n - 1]);
			if (cur->stat.err) {
				err = true;
				clear_stat_list(stat_list);
				delete stat_list;
				return;
			}
			if (n - 1 > 0) {
				cur->next = new struct CStatNode;
				prev = cur;
				cur = cur->next;
			}
			else
				cur->next = NULL;
		}
		if (prev) {
			last = &prev->next;
		}
		else {
			last = NULL;
		}
	}
	else {
		stat_list = NULL;
		last = NULL;
	}
	comp_stat1 = NULL;
	comp_stat2 = NULL;
};

CStatList::CStatList() {
	err = false;
	length = 0;
	lname = NULL;				//???
	stat_list = NULL;
	last = NULL;
	comp_stat1 = NULL;
	comp_stat2 = NULL;
};

CStatList::CStatList(const CStatList & sl) {
	length = sl.length;
	lname = NULL;				//???
	int n = length;
	if (n > 0) {
		stat_list = new struct CStatNode;
		CStatNode * cur_new = stat_list, *prev = NULL, *cur_old = sl.stat_list;
		for (; n > 0; n--) {
			cur_new->stat.init(sl.stat_list->stat.spath);
			if (cur_new->stat.err) {
				err = true;
				clear_stat_list(stat_list);
				delete stat_list;
				return;
			}
			if (n - 1 > 0) {
				cur_new->next = new struct CStatNode;
				prev = cur_new;
				cur_new = cur_new->next;
				cur_old = cur_old->next;
			}
			else
				cur_new->next = NULL;
		}
		if (prev) {
			last = &prev->next;
		}
		else {
			last = NULL;
		}
	}
	else {
		stat_list = NULL;
		last = NULL;
	}
	comp_stat1 = NULL;
	comp_stat2 = NULL;
};

CStatList::~CStatList() {
	clear_stat_list(stat_list);
	delete stat_list;
};

CStatList & CStatList::operator= (const CStatList & sl) {
	clear_list();
	length = sl.length;
	lname = NULL;				//???
	int n = length;
	if (n > 0) {
		stat_list = new struct CStatNode;
		CStatNode * cur_new = stat_list, *prev = NULL, *cur_old = sl.stat_list;
		for (; n > 0; n--) {
			cur_new->stat.init(sl.stat_list->stat.spath);
			if (n - 1 > 0) {
				cur_new->next = new struct CStatNode;
				prev = cur_new;
				cur_new = cur_new->next;
				cur_old = cur_old->next;
			}
			else
				cur_new->next = NULL;
		}
		if (prev) {
			last = &prev->next;
		}
		else {
			last = NULL;
		}
	}
	else {
		stat_list = NULL;
		last = NULL;
	}
	comp_stat1 = NULL;
	comp_stat2 = NULL;
	return *this;
};

CStatList & CStatList::operator+ (const CStatList & sl) {
	length += sl.length;
	int n = sl.length;
	if (n > 0) {
		CStatNode * cur_new, *prev = NULL, *cur_old = sl.stat_list;
		if (last) {
			(*last)->next = new struct CStatNode;
			prev = *last;
			cur_new = (*last)->next;
		}
		else {
			if (stat_list) {
				(stat_list)->next = new struct CStatNode;
				cur_new = (stat_list)->next;
				prev = stat_list;
			}
			else {
				stat_list = new struct CStatNode;
				cur_new = stat_list;
			}
		}
		for (; n > 0; n--) {
			cur_new->stat.init(sl.stat_list->stat.spath);
			if (n - 1 > 0) {
				cur_new->next = new struct CStatNode;
				prev = cur_new;
				cur_new = cur_new->next;
				cur_old = cur_old->next;
			}
			else
				cur_new->next = NULL;
		}
		if (prev) {
			last = &prev->next;
		}
		else {
			last = NULL;
		}
	}
	return *this;
};

void CStatList::add_nodes(int n, char ** paths) {
	if (n > 0) {
		CStatNode * cur, *prev = NULL;
		length += n;
		if (last) {
			(*last)->next = new struct CStatNode;
			prev = *last;
			cur = (*last)->next;
		}
		else {
			if (stat_list) {
				(stat_list)->next = new struct CStatNode;
				cur = (stat_list)->next;
				prev = stat_list;
			}
			else {
				stat_list = new struct CStatNode;
				cur = stat_list;
			}
		}
		for (int i=0; i<n; i++) {
			cur->stat.init(paths[i]);
			if (i+1<n) {
				cur->next = new struct CStatNode;
				prev = cur;
				cur = cur->next;
			}
			else
				cur->next = NULL;
		}
		if (prev) {
			last = &prev->next;
		}
		else {
			last = NULL;
		}
	}
};

void CStatList::clear_list() {
	clear_stat_list(stat_list);
	clear_stat_list(comp_stat1);
	clear_stat_list(comp_stat2);
	stat_list = NULL;
	comp_stat1 = NULL;
	comp_stat2 = NULL;
	last = NULL;
	delete [] lname;
	lname = NULL;
	length = 0;
}

void CStatList::rename_list(char * new_lname) {
	if (lname) {
		delete[] lname;
	}
	lname = new char[strlen(new_lname) + 1];
	strcpy(lname, new_lname);
};

void CStatList::del_node(char * path) {
	struct CStatNode * cur = stat_list, *prev = NULL;
	while (cur != NULL) {
		if (!strcmp(path, cur->stat.spath)) {
			if (prev) {
				prev->next = cur->next;
			}
			else {
				stat_list = cur->next;
			}
			cur->stat.clear();
		}
		cur = cur->next;
		prev = cur;
	}
};

//void change_nparam (char * path, ???);


struct CStatNode * CStatList::get_stat_node(char * path) {
	struct CStatNode * cur = stat_list;
	while (cur != NULL) {
		if (!strcmp(path, cur->stat.spath)) {
			return cur;
		}
		cur = cur->next;
	}
	return NULL;
};

CStat * CStatList::get_stat(char * path) {
	struct CStatNode * np = get_stat_node(path);
	return &np->stat;
};

void CStatList::save_list(char * path) {
	std::ofstream fout;
	fout.open(path);
	struct CStatNode * cur = stat_list;
	while (cur) {
		fout << (cur->stat.spath) << std::endl;
		cur = cur->next;
	}
	fout.close();
};


void CStatList::compare(struct CStatNode * n1, struct CStatNode * n2) {
	if (comp_stat1) {
		comp_stat1->stat.clear();
	}
	else {
		comp_stat1 = new CStatNode;
	}
	if (comp_stat2) {
		comp_stat2->stat.clear();
	}
	else {
		comp_stat2 = new CStatNode;
	}
	comp_stat2->next = NULL;
	comp_stat1->next = NULL;
	comp_stat1->param = n1->param;
	comp_stat2->param = n2->param;
	stat_intersect(n1->stat, n2->stat, comp_stat1->stat, comp_stat2->stat);
};


void CStatList::clear_stat_list(CStatNode * sl) {
	if (sl->next) {
		clear_stat_list(sl->next);
		delete sl->next;
	}
	else {
		sl->stat.clear();
	}
};






