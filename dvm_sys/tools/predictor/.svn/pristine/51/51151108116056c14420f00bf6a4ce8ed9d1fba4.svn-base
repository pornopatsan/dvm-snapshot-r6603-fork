#ifndef __PS_H
#define __PS_H

//	#pragma warning(disable: 4786)

#include <iostream>
#include <vector>
#include <queue>
#include <string>
#include "Vm.h"

//grigory add-on 
using namespace std;

void ClustError (int num_error);

struct ProcInfo
{
 int numClust;
 int numInClust;
 double ProcPower;
};

typedef  struct ProcInfo strProcInfo;

class ClustInfo
{
public :
	//новое
	string name; 
	vector<ClustInfo> SubCluster;
	string CommType; //between SubClusters
	int num_channel;
	vector<double> channel_time;
	double TStart;
	double TByte;
	long fis_proc_id;
	double ProcPower; //if have no SubClusters, it means one processor with no communication inside itself

	long GetProcCount();
	double GetProcPower(long fis_id);
	void Set_all_fis_proc_id();
	long map(long proc_id);
	bool IsInCluster(long id);
	ClustInfo *GetCommClust(long id1, long id2);
	//новое

	//должнo отмереть
	int numClust; 
	vector <strProcInfo> Procs;
	ClustInfo();
	ClustInfo(int num);
	void AddProc(int num,double power);
	void setNum(int num) {this->numClust=num;}
	//доделать
	void setTStart(double tstart);
	void setTByte(double tbyte);
	//доделать
	//должно отмереть

};

typedef class ClustInfo  classClustInfo;

class CompletePS
{
public :
	vector<classClustInfo> Clusters;
	CompletePS();
	void MakeNewCluster(int numClust);
	void AddProcToClust(int numClust,int numProc,strProcInfo procInfo);	
	void MakeFullMapping(std::vector<double>& result);
	void SortProcessors(std::vector<double> &array_of_productivity);
	
};

class MappedProcs
{
public:
	vector<strProcInfo> Processors;
	void AddProc(strProcInfo& procInfo);
	MappedProcs() ;
	void AddProccessors(int start,int end,int step , ClustInfo &cPS);
};
	//\grigory add-on


typedef std::vector<long> LongVector;
typedef std::vector<double> DoubleVector;

class PS {

	static bool NextOptionLine(std::istream& opt_file, std::string& buffer);

	std::queue<LongVector>		ps_lb_list;		// low bounderies on each dim	 
	std::queue<LongVector>		SizeArray_list;	// extentions  on each dim
	std::queue<DoubleVector>	weight_list;	// list of vectors - PS weights

    mach_Type	Type;
	int			numChanels;		// numbers of parallel chanels in Myrinet
    double		TStart,                                     
				TByte,                                    
				ProcPower,
				scale;		// 

	//grigory add-on 
public :
	CompletePS completePS;    // процессорная система без учета первого отображения 
	MappedProcs mappedProcs;  // отображенные пользователем процессоры(будут использоваться в сисиетме)
	//std::vector<LongVector> vWeights;
	vector<double> vProcPower;// процессорные мощности процессоров , на которых должна быть выполнена программа
public : 
	int getProcCount();
	void CorrectMappedProcs();
void  PrepareForAutoSearch(std::vector<long>& perstanovki);
    void reset();
	
	//\grigory add-on


public:

	// read PS's configurations from the file
	PS(const char* file_name);
	PS(const char *option_file, int i1);//old variant of parameter file
	PS(mach_Type AType, int AnumChanels, double TStart, double TByte);

	// returns next processor's charactiristics
	void	nextPS(std::vector<long>& lb, std::vector<long>& ASizeArray, 
				   mach_Type& AMType, int& AnumChanels, double& Ascale, 
				   double&	ATStart, double&  ATByte, double& AProcPower, vector<double>& AvProcPower);
	void	setTopology(std::vector<long>& ASizeArray);
	void	hardwarePS(int& AMType, double&	ATStart, double&  ATByte, double& AProcPower,vector<double> & AvProcPower);


#ifdef P_DEBUG
	friend std::ostream& operator << (std::ostream& os, const PS& ps);
#endif

};

extern	PS *	ps;					// prosessor system object
extern	long	currentPS_ID;		// current PS ID

extern ClustInfo *CurrentCluster;

#endif 
