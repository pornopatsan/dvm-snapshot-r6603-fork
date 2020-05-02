#include <ctime>
#define _STATFILE_
#include "statread.h"
#include <string.h>
#include <stdlib.h>
#include <fstream>
#include <float.h>
#include <exception>
#include <new>
#include <stdbool.h>
#include <string>
#include <sstream>
#include "json.hpp"

using json = nlohmann::json;

namespace patch
{
	template < typename T > std::string to_string(const T& n)
	{
		std::ostringstream stm;
		stm << n;
		return stm.str();
	}
}

class CStatInter {
public:
    CStatInter( const CStatInter& si);
	CStatInter(CStatRead * stat_read, int n);
	CStatInter(json source);
	~CStatInter();
	void delete_tail();
	void clear();
	void to_string(std::string & result);
	void to_json(json & result);

	ident id;
	//main characteristics
	double prod_cpu;
	double prod_sys;
	double prod_io;
	double prod;
	double exec_time;
	double sys_time;
	double efficiency;
	double lost_time;
	double insuf;
	double insuf_user;
	double insuf_sys;
	double comm;
	double real_comm;
	double comm_start;
	double idle;
	double load_imb;
	double synch;
	double time_var;
	double overlap;
	double thr_user_time;
	double thr_sys_time;
	double gpu_time_prod;
	double gpu_time_lost;
	unsigned long nproc;
	unsigned long threadsOfAllProcs;
	struct ColOp col_op[RED];
    OpGrp (* op_group)[StatGrpCount];
    unsigned long qproc;
    bool isjson;
    //comparative characteristics
	//struct ProcTimes comp_proc_times[3];
	//characteristics by processes
	struct ProcTimes *  proc_times;
	CStatInter * next;
};

struct CProcInfo {
	char * node_name;
	double test_time;
};

class CStat {   //копирование и присваивание запрещены
private:
    CStat operator=(const CStat&);
    CStat( const CStat& );
public:
	CStat();
	CStat(json source);
	void init(char * path) ;
	void clear();
	~CStat() ;
	CStatInter * inter_tree;                                //"�������" ������ ����������
	unsigned long nproc;									//���������� ���������
	char p_heading[80];
	CProcInfo  * proc_info;                                 //���������� �� ����������� (��� ����, �������� �����)
    CStatRead * stat;
    char * spath;
	bool iscomp;
	bool isinitialized;
	void to_string(std::string & result);
	void to_json(json &result);
	bool err;
	bool isjson;
};

struct CStatParam {
	//?????????????       								    //��������� �������
};

struct CStatNode {                                          //��������� ���������� � ������
	CStat stat;
	struct CStatNode * next;
	struct CStatParam param;
};

class CStatList {
public:
	CStatList(int n, char** paths) ;                      //������� ������ �� ������ n ���������
	CStatList(char * path) ;                              //������� ����������� ������
	CStatList();                                          //������� ������ ������
	CStatList(const CStatList & stat_list);
	~CStatList();
	CStatList & operator= (const CStatList & stat_list);
	CStatList & operator+ (const CStatList & stat_list);
	void add_nodes(int n, char ** paths) ;  //�������� �������� � ������
	void clear_list() ;                                   //������� ��� �������� �� ������
	void rename_list(char * new_lname);                     //������������� ������
	void del_node(char * path) ;                          //������� ���� ������� ������
	struct CStatNode * get_stat_node(char * path) ;       //�������� ��������� �� ��������� ����� (����������)
	CStat * get_stat(char * path) ;
	void save_list(char * path);	                        //��������� ������ � ��������� ����
	void compare(struct CStatNode * n1, struct CStatNode * n2);
	void clear_stat_list(CStatNode * sl) ;
	struct CStatNode* stat_list, **last, *comp_stat1, *comp_stat2;
	int length;
	char * lname;
	bool err;
};

void inter_tree_intersect(CStatInter *i1, CStatInter *i2, CStatInter **r1, CStatInter **r2);
void stat_intersect(const CStat &s1, const CStat &s2, CStat & r1, CStat & r2);
CStatInter * find_inter(long expr, short nlev, CStatInter * cur);
CStatInter * next_inter(short nlev, CStatInter * cur);
/*
struct CStatListNode {
	char * nname;
	CStatList sl;
	CStatListNode * next;
};

CStatListNode * stat_set, **last; //stat_set - list of CStatList

void create_stat_list(char * name) {
	if (stat_set) {
		(*last)->next = new CStatListNode;
		last = &(*last)->next;
		(*last)->next = NULL;
		(*last)->nname = new char[strlen(name) + 1];
		strcpy((*last)->nname, name);
	}
	else {
		stat_set = new CStatListNode;
		stat_set->next = NULL;
		stat_set->nname = new char[strlen(name) + 1];
		strcpy(stat_set->nname, name);
		last = &stat_set;
	}
}*/
