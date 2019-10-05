#include <stdlib.h>
#include <string.h>

#include <cctype>
#include <fstream>

#include "Ps.h"
#include "ParseString.h"
//====
#include <stdio.h>
//=***

using namespace std;

extern ofstream prot;
extern int search_opt_mode; //====//

ClustInfo *CurrentCluster;

PS	*	ps = NULL;				// prosessor system object
long	currentPS_ID = 0;		// current PS ID

//==== grigory_struct
void ClustError (int num_error)
{
	printf("Error %d :", num_error);

	switch (num_error)
	{
		case 1 : printf("in ClusterInfo proc num cannot be less the 0 \n");
		break;

		case 2:  printf("Enable to add a cluster - cluster with this number already exists \n");
		break;

		case 4:  printf(" Wrong range for mapping processors \n");
		break;

		case 6:  printf ("wrong parameter for system topology:  size of mapped peocessors less then modelling proccessor's system (from command line) \n");
		break;

	
		default : printf("smthg wrong ");
		break;
	}
	exit(0);
}

//--------------------------------------------clustInfo -------------------------------------------


void ClustInfo::AddProc(int num,double power) // добавить информацию о процессоре в  Cluster 
{
  ProcInfo temp;
  temp.numClust=this->numClust;

  
  if(num<0)
  {
	ClustError(1);
  }

 if(num==Procs.size())
 {
	 temp.ProcPower= power;
	 temp.numInClust=num;
	 this->Procs.push_back(temp); 
 }
 else 
	 if (num< this->Procs.size())
	 {

		 this->Procs[num].numInClust=num;
		 this->Procs[num].ProcPower=power;	 
	 }
	 else
	 {
	 int i;
	 for(i=Procs.size();i<num ;i++)
	 {
		 temp.ProcPower=1.0;
		 temp.numInClust=i;
		 this->Procs.push_back(temp);	 
	 }
	 temp.ProcPower=power;
	 temp.numInClust=num;
	 this->Procs.push_back(temp);	  
 }

	 return;
}


ClustInfo::ClustInfo() 
{ 
	Procs=vector<strProcInfo>(0); 
	this->numClust=0;
} 


ClustInfo::ClustInfo(int num)
{
	Procs=vector<strProcInfo>(0); 
	this->numClust=num;
}

void ClustInfo::Set_all_fis_proc_id()
{
	static int id=0;
	int i;
	for(i=0; i<SubCluster.size(); i++)
		SubCluster[i].Set_all_fis_proc_id();

	if(SubCluster.size()==0)
		fis_proc_id=id++;
}

//calculate logic number of processors
long ClustInfo::GetProcCount() 
{
	int id=0;
	int i;
	for(i=0; i<SubCluster.size(); i++)
		id+=SubCluster[i].GetProcCount();

	if(SubCluster.size()==0)
		id++;

	return id;
}

// map logic id into phisical id
long ClustInfo::map(long proc_id)
{
	// now it is default variant
	// correct will have different pre-map fuctions and structs, where map information will store
	return proc_id;
}

bool ClustInfo::IsInCluster(long id)
{
	int i;

	if(SubCluster.size()==0)
		return id==fis_proc_id;
	else
		for(i=0; i<SubCluster.size(); i++)
			if(SubCluster[i].IsInCluster(id))
				return true;

	return false;
}

ClustInfo* ClustInfo::GetCommClust(long id1, long id2)
{ 
	int i;

	if(!IsInCluster(id1) || !IsInCluster(id2))
		return NULL;

	for(i=0; i<SubCluster.size(); i++)
		if(SubCluster[i].IsInCluster(id1) && SubCluster[i].IsInCluster(id2))
			return SubCluster[i].GetCommClust(id1,id2);

	return this;
}

double ClustInfo::GetProcPower(long fis_id)
{
	int i;

	if(SubCluster.size()==0)
		return ProcPower;
	else
		for(i=0; i<SubCluster.size(); i++)
			if(SubCluster[i].IsInCluster(fis_id))
				return SubCluster[i].GetProcPower(fis_id);

	return 0.0;
}

// ------------------------Complete PS  -------------------------------------------
CompletePS::CompletePS()
{
 this->Clusters=vector<classClustInfo>(0); 
 this->Clusters.resize(0);
}

void CompletePS::MakeNewCluster (int numClust)
{
	classClustInfo temp;
	
	if(numClust<this->Clusters.size())
		 this->Clusters[numClust].setNum(numClust);
	else 
		if(numClust>this->Clusters.size())
		{
			int i;
			 for(i=this->Clusters.size();i<numClust;i++)
			 {
			  temp.setNum(i);
			  this->Clusters.push_back(temp);
			 }
			 temp.setNum(numClust);
             this->Clusters.push_back(temp);
		} else
		{
	temp.setNum(numClust);
	this->Clusters.push_back(temp);
	//this->Clusters.resize(0);
	}
	return;
}

void CompletePS::AddProcToClust(int numc,int nump,strProcInfo procInfo)
{
	this->Clusters[numc].AddProc(nump,procInfo.ProcPower);
	return;
}

void CompletePS::MakeFullMapping(std::vector<double> & result)
{
	result.resize(0);
	int i,j;

	for(i=0;i<this->Clusters.size();i++)
	{
		for(j=0;j<this->Clusters[i].Procs.size();j++)
		{
		  result.push_back(Clusters[i].Procs[j].ProcPower);		
		}	
	}
}

void SortProcessors(std::vector<int> &array_of_productivity)
{

}



//----------------------------mapped procs ----------------------
MappedProcs::MappedProcs() 
{ 
	Processors.resize(0);
}


void MappedProcs::AddProc(strProcInfo &procInfo)
{
 this->Processors.push_back(procInfo);
 return;
}


void MappedProcs::AddProccessors(int start,int end,int step,ClustInfo & cPS)
{
   int i=0;
   if(start<0 || end>cPS.Procs.size())
   {
	   ClustError(4);   
   }

   for(i=start;(i<=end) && (i<cPS.Procs.size());i+=step)
   { 
	this->Processors.push_back(cPS.Procs[i]);   
   }
}
//=*** grigory_struct

//------------------------------------------------------------------------------------
// read next line from "opt_file"
// skip blank lines and lines starting from "//" - comments
// compress '     ' -> ' '
// add line to "buffer"
//-------------------------------------------------------------------------------------

bool PS::NextOptionLine(std::istream& opt_file, std::string& buffer)
{
	string::size_type	spn;
	string::iterator	i;
	string				buf;
	bool				sem = false;

    do {
		if (opt_file.eof()) return false;
        getline(opt_file, buf, '\n');
        spn = buf.find_first_not_of(' ');
    } while (buf.size() == 0     || 
			 spn == string::npos || 
			 (buf[spn]=='/' && buf[spn+1]=='/'));
	buf += ' ';
	for (i = buf.begin(); i < buf.end(); i++) {
		if (*i == ' ' || *i == '\t' /* || *i == ',' || *i == ';' */) {
			if (sem) { 
				continue;
			} else {
				sem = true;
				buffer += ' ';
			}
		} else {
			sem = false;
			buffer += tolower((char) *i);
		}
	}

    return true;
}

#ifdef nodef
static void SkipSpaces(const std::string& buffer, std::string::size_type& pos)
{
		// skip spaces
	if ((pos = buffer.find_first_not_of(' ', pos)) == string::npos) {
		prot << "Syntax error." << endl;
		exit(EXIT_FAILURE);
	}
}
#endif

static void SkipSpaces(const char * buffer, size_t& pos)
{
	size_t i = strlen(buffer);

	while (buffer[pos] == ' ' && pos < i)
		pos++;
}

static void GetString(const char * buffer, size_t& pos, string *out)
{
	char buf[2];
	size_t i = strlen(buffer);

	*out="";
	while (pos < i && buffer[pos] != ' ' && buffer[pos] != '.' && buffer[pos] != ';' && buffer[pos] != ',' && buffer[pos] != '}' && buffer[pos] != '{')
	{
		buf[0]=buffer[pos++];
		buf[1]=0;
		out->append(buf);
	}
}

static void GetNumber(const char * buffer, size_t& pos, int *out)
{
	size_t i = strlen(buffer);

	*out=atoi(buffer+pos);
	for(	;pos<i; pos++)
	{	if(buffer[pos]<'0' || buffer[pos]>'9')
			break;
	}
}

static void GetNumber(const char * buffer, size_t& pos, float *out)
{
	size_t i = strlen(buffer);
	bool flag=0;

	*out=atof(buffer+pos);
	for(	;pos<i; pos++)
	{	
		if(buffer[pos]<'0' || buffer[pos]>'9')
		{
			if(!flag && buffer[pos]=='.')
				flag=1;
			else
				break;
		}
	}
}

static void GetNumber(const char * buffer, size_t& pos, double *out)
{
	size_t i = strlen(buffer);
	bool flag=0;

	*out=atof(buffer+pos);
	for(	;pos<i; pos++)
	{	
		if(buffer[pos]<'0' || buffer[pos]>'9')
		{
			if(!flag && buffer[pos]=='.')
				flag=1;
			else
				break;
		}
	}
}

static void SyntaxError(int err)
{
	prot << "Syntax error " << err << " in Predictor parameter file." << endl;
	cerr << "Syntax error " << err << " in Predictor parameter file." << endl;
	exit(EXIT_FAILURE);
}

void print_Cluster(string *pre_str, ClustInfo *Cluster)
{
	int i;
	string tmp_pre_str;
	tmp_pre_str=*pre_str+"  ";

	if(Cluster->SubCluster.size()==0)
		printf("%sClusterName = %s   ProcPower=%f fis_id=%d\n", pre_str->c_str(), Cluster->name.c_str(), Cluster->ProcPower, Cluster->fis_proc_id);
	else
		printf("%sClusterName = %s   Comm=%s num=%d TStart=%f TByte=%f\n", pre_str->c_str(), Cluster->name.c_str(), Cluster->CommType.c_str(), Cluster->num_channel, Cluster->TStart, Cluster->TByte);

	for(i=0;i<Cluster->SubCluster.size();i++)
		if(1||i==0) print_Cluster(&tmp_pre_str,&Cluster->SubCluster[i]);
}

void buildCluster(string *Name, ClustInfo *NewCluster, vector<ClustInfo> *Clusters)
{
	int i,j;

	for(j=0;j<Clusters->size();j++)
		if((*Clusters)[j].name == *Name)
			break;

	if(j>=Clusters->size())
		return;

	NewCluster->name=*Name;
	NewCluster->SubCluster.resize(0);
	NewCluster->num_channel=0;

	if((*Clusters)[j].SubCluster.size()==0)
		NewCluster->ProcPower=(*Clusters)[j].ProcPower;
	else
	{
		NewCluster->TStart=(*Clusters)[j].TStart;
		NewCluster->TByte=(*Clusters)[j].TByte;
		if((*Clusters)[j].num_channel!=-1)
		{
			NewCluster->CommType=(*Clusters)[j].CommType;
			NewCluster->num_channel=(*Clusters)[j].num_channel;
		}
		else
		{
			for(i=0;i<Clusters->size();i++)
				if((*Clusters)[i].name==(*Clusters)[j].CommType)
					break;

			if(i<Clusters->size())
			{
				NewCluster->CommType=(*Clusters)[i].CommType;
				NewCluster->num_channel=(*Clusters)[i].num_channel;
				NewCluster->TByte=(*Clusters)[i].TByte;
				NewCluster->TStart=(*Clusters)[i].TStart;
			}
		}
	}


	for(i=0;i<(*Clusters)[j].SubCluster.size();i++)
		NewCluster->SubCluster.push_back((*Clusters)[j].SubCluster[i]);

	for(i=0;i<(*Clusters)[j].SubCluster.size();i++)
		buildCluster(&((*Clusters)[j].SubCluster[i].name), &NewCluster->SubCluster[i], Clusters);


	if(NewCluster->num_channel<0) ClustError(1);
	NewCluster->channel_time.resize(NewCluster->num_channel);
	for(i=0;i<NewCluster->num_channel;i++)
		NewCluster->channel_time[i]=0.0;
}


//------------------------------------------------------------------------------
//
//                     CONSTRUCTOR
//
//-------------------------------------------------------------------------------
//
PS::PS(mach_Type AType, int AnumChanels, double ATStart, double ATByte):
Type(AType),
numChanels(AnumChanels),
TStart(ATStart),
TByte(ATByte),
scale(1.0),
ProcPower(1.0)
{ int i;		
	ClustInfo tempClustInfo;
	tempClustInfo.Procs.resize(0);
	int clustNum, mystartint,myendint;
	double mypower;

	search_opt_mode=0; //no grig search optimal way of processor grid

			clustNum = 1;
			tempClustInfo.setNum(clustNum);
			mystartint=0;
			myendint=99999; //максимум 100000 процов в кластере
			if(ProcPower > 0.000000001) mypower=ProcPower;
			else mypower=1;

//			printf("Mypower=%f\n",mypower);

			for(i=mystartint;i<=myendint;i++)
			{ tempClustInfo.AddProc(i,mypower);
			}
				
			completePS.MakeNewCluster(clustNum);

			for(i=0;i<tempClustInfo.Procs.size();i++)
			{
				ProcInfo tempPI;
				tempPI.numClust=tempClustInfo.numClust;
				tempPI.numInClust=tempClustInfo.Procs[i].numInClust;
				tempPI.ProcPower=tempClustInfo.Procs[i].ProcPower;
				completePS.AddProcToClust(tempClustInfo.numClust,i,tempPI);
			}			
			
      mappedProcs.AddProccessors(mystartint,myendint,1,completePS.Clusters[clustNum]);

//      double min_weight=1.e7;

			for(i=0;i<mappedProcs.Processors.size();i++)
			{
				vProcPower.push_back(mappedProcs.Processors[i].ProcPower);
				//min_weight = min_weight > mappedProcs.Processors[i].ProcPower ? mappedProcs.Processors[i].ProcPower :  min_weight;
			}

}


// построить информацию о процессорной системе 
//
// ввод параметров для новой схемы коммуникаций
PS::PS(const char *option_file)
{
	vector<ClustInfo> Clusters;
	ClustInfo NewCluster;
	ClustInfo TmpCluster;
    string buffer1;
    string CurrentClusterName, TmpClusterName, tmp_str;
	const char * buffer;
	size_t pos=0;
	int i,j;

	search_opt_mode=0; //initialization
	Clusters.resize(0);


    ifstream opt_file(option_file);

	if (!opt_file.is_open()) {
		cerr << "Can't open parameter file '" << option_file << '\'' << endl;
		opt_file.exceptions(ifstream::badbit | ifstream::failbit | ifstream::eofbit);
	}

    prot << "Reading predictor options..." << endl;
	
	// read configuration file into string, all letters - small letters

    while (NextOptionLine(opt_file, buffer1)) 
		;
	buffer = buffer1.c_str();

//	printf("Buffer=%s\n",buffer);

	while (pos < strlen(buffer)) 
	{
		SkipSpaces(buffer, pos);
		// find key words

		if (strncmp(buffer + pos, "cluster", strlen("cluster")) == 0) 
		{	// CLUSTER ----------------------------------------------------------------------
			pos += strlen("cluster");
			SkipSpaces(buffer, pos);
			if (buffer[pos] != '=') SyntaxError(1);
			SkipSpaces(buffer, ++pos);

			GetString(buffer,pos,&CurrentClusterName);

			SkipSpaces(buffer, pos);
			if(buffer[pos++]!=';') SyntaxError(2);
		}
		else
		if (strncmp(buffer + pos, "search", strlen("search")) == 0) 
		{	// SEARCH ----------------------------------------------------------------------
			pos += strlen("search");
			SkipSpaces(buffer, pos);
			if (buffer[pos] != '=') SyntaxError(1);
			SkipSpaces(buffer, ++pos);

			GetNumber(buffer,pos,&search_opt_mode);

			SkipSpaces(buffer, pos);
			if(buffer[pos++]!=';') SyntaxError(2);
		}
		else
		{
			SkipSpaces(buffer, pos);
			GetString(buffer,pos,&TmpClusterName);

			if(TmpClusterName[0]>='0' && TmpClusterName[0]<='9') SyntaxError(3);

			NewCluster.SubCluster.resize(0);
			NewCluster.name=TmpClusterName;

			NewCluster.ProcPower=1;
			NewCluster.TStart=0;
			NewCluster.TByte=0;
			NewCluster.num_channel=1;

//			printf("Current '%c'\n",buffer[pos]);
			if(buffer[pos]=='.') 
			{
				GetString(buffer,++pos,&tmp_str);
				SkipSpaces(buffer, pos);
				if (buffer[pos] != '=') SyntaxError(1);
				SkipSpaces(buffer, ++pos);

				if(tmp_str=="tstart")
				{
					GetNumber(buffer,pos,&NewCluster.TStart);
					NewCluster.TStart/=1000000;
				}
				if(tmp_str=="tbyte")
				{
					GetNumber(buffer,pos,&NewCluster.TByte);
					NewCluster.TByte/=1000000;
				}
				if(tmp_str=="commtype")
				{
					GetString(buffer,pos,&TmpClusterName);
					SkipSpaces(buffer, pos);

					if(buffer[pos]=='.')
					{
						SkipSpaces(buffer, ++pos);
						GetString(buffer,pos,&tmp_str);
						if(tmp_str!="commtype") SyntaxError(6);

						NewCluster.CommType=TmpClusterName;
						NewCluster.num_channel=-1; //sign of commtype of other cluster

					}
					else
					{
						if(TmpClusterName=="myrinet")
						{
							SkipSpaces(buffer, pos);
							if(buffer[pos++]!='(') SyntaxError(7);
							SkipSpaces(buffer, pos);
							GetNumber(buffer,pos,&NewCluster.num_channel);

							SkipSpaces(buffer, pos);
							if(buffer[pos++]!=')') SyntaxError(7);

							NewCluster.CommType=TmpClusterName;
						}
						if(TmpClusterName=="ethernet" || TmpClusterName=="transputer")
							NewCluster.CommType=TmpClusterName;
					}
					
				}



				for(i=0;i<Clusters.size();i++)
					if(Clusters[i].name==NewCluster.name)
						break;

				if(i<Clusters.size())
				{
					if(NewCluster.TStart!=0) 
						Clusters[i].TStart=NewCluster.TStart;

					if(NewCluster.TByte!=0) 
						Clusters[i].TByte=NewCluster.TByte;
					
					if(tmp_str=="commtype")
					{
						Clusters[i].CommType=NewCluster.CommType;
						Clusters[i].num_channel=NewCluster.num_channel;
					}
				}

				SkipSpaces(buffer, pos);
				if(buffer[pos++]!=';') SyntaxError(2);

//				while(buffer[pos]!=';')
//					pos++;
//				pos++;
			}
			else
			{
				SkipSpaces(buffer, pos);
				
				if(pos>=strlen(buffer))
					break;


				if (buffer[pos] != '=') SyntaxError(1);
				SkipSpaces(buffer, ++pos);

				if (buffer[pos] != '{') 
				{
					if(buffer[pos] <'0' || buffer[pos] >'9')  
						SyntaxError(3);

					GetNumber(buffer,pos, &NewCluster.ProcPower);
					SkipSpaces(buffer, pos);
					if(buffer[pos++]!=';') SyntaxError(2);
					Clusters.push_back(NewCluster);
					continue;
				}
				SkipSpaces(buffer, ++pos);

				while(1)
				{
					i=1;
					if(buffer[pos]>='0' && buffer[pos]<='9')
					{
						GetNumber(buffer,pos,&i);
						SkipSpaces(buffer, pos);
						if(buffer[pos]!='x') SyntaxError(4);
						SkipSpaces(buffer, ++pos);
					}

					GetString(buffer,pos,&tmp_str);

					TmpCluster.name=tmp_str;
					TmpCluster.SubCluster.resize(0);
					TmpCluster.ProcPower=1;
					TmpCluster.TStart=0;
					TmpCluster.TByte=0;

					for(	;i>0;i--)
					{
						NewCluster.SubCluster.push_back(TmpCluster);
					}
					SkipSpaces(buffer, pos);
					if(buffer[pos]=='}')
						break;

					if(buffer[pos]!=',') SyntaxError(5);
					SkipSpaces(buffer, ++pos);
				}

				SkipSpaces(buffer, ++pos);
				if(buffer[pos++]!=';') SyntaxError(2);


				for(i=0;i<Clusters.size();i++)
					if(Clusters[i].name==NewCluster.name)
						break;

				if(i>=Clusters.size())
					Clusters.push_back(NewCluster);
				else
				{
					if(NewCluster.ProcPower) 
						Clusters[i].ProcPower=NewCluster.ProcPower;
					else
						for(j=0;j<NewCluster.SubCluster.size();j++)	
							Clusters[i].SubCluster.push_back(NewCluster.SubCluster[j]);
				}
			}
		}
	}

	CurrentCluster=new ClustInfo;
	buildCluster(&CurrentClusterName, CurrentCluster, &Clusters);
	Clusters.resize(0);

	CurrentCluster->Set_all_fis_proc_id();


	tmp_str="";
//	print_Cluster(&tmp_str,CurrentCluster);
}


PS::PS(const char *option_file, int i1)
{ 
    string				buffer1;
	const char *		buffer;
	const char *		p;
	size_t				pos = 0;

	LongVector			int1,
						int2;
	DoubleVector		dbl;
	int					i,
						k,
						lb,
						ub;

	bool				loop;
	double				d;

	search_opt_mode=0; //initialization

	int1.resize(0);
	int2.resize(0);

	//grig
	vProcPower.resize(0);
	bool wasmapped=false;
	//\grig
//====
	bool mapflag=true;
	bool procflag=true;
//=***
	scale =1.0;

    ifstream opt_file(option_file);

	if (!opt_file.is_open()) {
		cerr << "Can't open parameter file '" << option_file << '\'' << endl;
		opt_file.exceptions(ifstream::badbit | ifstream::failbit | ifstream::eofbit);
	}

    prot << "Reading predictor options..." << endl;
	
	// read configuration file into string

    while (NextOptionLine(opt_file, buffer1))
		;
	buffer = buffer1.c_str();

//	prot << buffer << endl;

	while (pos < strlen(buffer)) {
		SkipSpaces(buffer, pos);
		// find key words

		if (strncmp(buffer + pos, "type", strlen("type")) == 0) {
			// TYPE ----------------------------------------------------------------------
			pos += strlen("type");
			SkipSpaces(buffer, pos);
			if (buffer[pos] != '=') SyntaxError(1);
			SkipSpaces(buffer, ++pos);

			if (strncmp(buffer + pos, "ethernet", strlen("ethernet")) == 0) {
				// TYPE = ETHERNET
				Type = mach_ETHERNET;
				pos += strlen("ethernet");
				SkipSpaces(buffer, pos);
			} else if (strncmp(buffer + pos, "transputer", strlen("transputer")) == 0) {
				// TYPE = TRANSPUTER
				Type = mach_TRANSPUTER;
				pos += strlen("transputer");
				SkipSpaces(buffer, pos);
			} else if (strncmp(buffer + pos, "myrinet", strlen("myrinet")) == 0) {
				// TYPE = MYRINET
				Type = mach_MYRINET;
				pos += strlen("myrinet");
				SkipSpaces(buffer, pos);
				if (buffer[pos] != '(') SyntaxError(1);
				SkipSpaces(buffer, ++pos);
				p = strpbrk(buffer + pos, " )");
				if (p == NULL)
					SyntaxError(4);
				numChanels = atoi(buffer + pos);
				pos = p - buffer;
				SkipSpaces(buffer, pos);
				if (buffer[pos] != ')') SyntaxError(1);
				SkipSpaces(buffer, ++pos);
			} else {
				SyntaxError(2);
			}

//			prot << "Type = " << Type << endl;

		} else if (strncmp(buffer + pos, "power", strlen("power")) == 0) {
			// POWER ---------------------------------------------------------------------
			pos += strlen("power");
			SkipSpaces(buffer, pos);
			if (buffer[pos] != '=')	
				SyntaxError(3);
			SkipSpaces(buffer, ++pos);
			p = strpbrk(buffer + pos, " ;");
			if (p == NULL)
				SyntaxError(4);
			ProcPower = atof(buffer + pos);
			pos = p - buffer;
			SkipSpaces(buffer, pos);

//			prot << "ProcPower = " << ProcPower << endl;
		} else if (strncmp(buffer + pos, "topology", strlen("topology")) == 0) {
			// TOPOLOGY = { int, int : int, ...} ------------------------------------------
//			prot << "topology = { ";
			pos += strlen("topology");
			SkipSpaces(buffer, pos);
			if (buffer[pos] != '=') 
				SyntaxError(5);
			SkipSpaces(buffer, ++pos);
			if (buffer[pos] != '{') 
				SyntaxError(6);
			++pos;
			SkipSpaces(buffer, pos);
			loop = true;
			while (loop) {

				SkipSpaces(buffer, pos);
				p = strpbrk(buffer + pos, " :,}");

				if (p == NULL)
					SyntaxError(7);
				lb = 0;
				ub = atoi(buffer + pos) - 1;
				pos = p - buffer;
				SkipSpaces(buffer, pos);

				if (buffer[pos] == ':') {
					++pos;
					p = strpbrk(buffer + pos, " ,}");
					if (p == NULL)
						SyntaxError(8);
					lb = ub + 1;
					ub = atoi(buffer + pos);
					pos = p - buffer;
					SkipSpaces(buffer, pos);
				}
				int1.push_back(lb);
				int2.push_back(ub - lb + 1);

//				prot << ' ' << lb << ':' << (ub - lb + 1) << ' ';

				if (buffer[pos] == '}') {
					++pos;
					loop = false;
					SkipSpaces(buffer, pos);
					if (buffer[pos] != ';') SyntaxError(9);
				} else if (buffer[pos] == ',') {
					++pos;
				} else 
					SyntaxError(10);
				
			}	// end while (loop)
//			prot << '}' << endl;
			ps_lb_list.push(int1);
			SizeArray_list.push(int2);

		} else if (strncmp(buffer + pos, "scale", strlen("scale")) == 0) {
			pos += strlen("scale");
			SkipSpaces(buffer, pos);
			if (buffer[pos] != '=')	
				SyntaxError(3);
			SkipSpaces(buffer, ++pos);
			p = strpbrk(buffer + pos, " ;");
			if (p == NULL)
				SyntaxError(4);
			scale = atof(buffer + pos);
			pos = p - buffer;
			SkipSpaces(buffer, pos);

		} else if (strncmp(buffer + pos, "weight", strlen("weight")) == 0) {
			// WEIGHT -------------------------------------------------------------
			dbl.resize(0);
//			prot << "weight = { ";
			pos += strlen("weight");

			SkipSpaces(buffer, pos);
			if (buffer[pos] != '=') 
				SyntaxError(11);

			SkipSpaces(buffer, ++pos);
			if (buffer[pos] != '{') 
				SyntaxError(12);
			++pos;

			loop = true;
			while (loop) {

				SkipSpaces(buffer, pos);
				p = strpbrk(buffer + pos, " ,(}");
				if (p == NULL)
					SyntaxError(13);
				d = atof(buffer + pos);
				//prot << d << ' ';
				dbl.push_back(d);
				pos = p - buffer;
				SkipSpaces(buffer, pos);

				if (buffer[pos] == '(') {
					++pos;
					SkipSpaces(buffer, pos);
					p = strpbrk(buffer + pos, " )");
					if (p == NULL)
						SyntaxError(14);
					k = atoi(buffer + pos) - 1;
					if (k < 0) 
						SyntaxError(15);
					for (i = 0; i < k; i++) {
						dbl.push_back(d);
					//	prot << d << ' ';
					}
					pos = p - buffer;
					SkipSpaces(buffer, pos);
					if (buffer[pos] != ')')
						SyntaxError(16);
					++pos;
					SkipSpaces(buffer, pos);
				}
				if (buffer[pos] == '}') {
					loop = false;
					++pos;
					SkipSpaces(buffer, pos);
					if (buffer[pos] != ';') 
						SyntaxError(17);
				} else if (buffer[pos] == ',') {
					++pos;
				} else
					SyntaxError(18);
			}		// end while(loop)

			weight_list.push(dbl);
//			prot << '}' << endl;

//====
		} else if (strncmp(buffer + pos, "search", strlen("search")) == 0) {
//			pos += strlen("search optimal configuration");
			pos += strlen("search");
			
			//проверить знак '='
			SkipSpaces(buffer, pos);
			if (buffer[pos] != '=') 
				SyntaxError(11);

			SkipSpaces(buffer, ++pos);
			p = strpbrk(buffer + pos, " ;");
			search_opt_mode = atoi(buffer + pos);
			pos=p-buffer;
			SkipSpaces(buffer, pos);

			if (buffer[pos] != ';') 
				SyntaxError(26);
			SkipSpaces(buffer, pos);

//=***

//grig add-on

		} else if (strncmp(buffer + pos, "procpowers", strlen("procpowers")) == 0) {
			// прочитать строчку о производительностях  -  добавить в структуру кластеров
//procpowers={ 1   { 0 0 : 1.5 , 1 2 : 2.5 , 3 4 : 3.5 }};
			procflag=false; //====//
			ClustInfo tempClustInfo;
			tempClustInfo.Procs.resize(0);
			int clustNum;
			int mystartint,myendint;
			double mypower;

			pos += strlen("procpowers");
            //проверить знак '='
			SkipSpaces(buffer, pos);
			if (buffer[pos] != '=') 
				SyntaxError(11);
			//проверить '{'
			SkipSpaces(buffer, ++pos);
			if (buffer[pos] != '{') 
				SyntaxError(12);
			++pos;


			//get cluster num
			SkipSpaces(buffer, pos);
			p = strpbrk(buffer + pos, " ,(}");
			if (p == NULL)
				SyntaxError(13);
			clustNum = atoi(buffer + pos);
		//	prot << d << ' ';
			pos = p - buffer;
			SkipSpaces(buffer, pos);

			tempClustInfo.setNum(clustNum);

			//проверить вторую '{'
	
			if (buffer[pos] != '{') 
				SyntaxError(13);
			++pos;

            loop = true;
			while (loop) {
				SkipSpaces(buffer, pos);
				p = strpbrk(buffer + pos, " ,(}:");
				if (p == NULL)
					SyntaxError(13);
				d = atof(buffer + pos);
				//prot << d << ' ';
				mystartint=d;
				
				pos = p - buffer;
				SkipSpaces(buffer, pos);

				SkipSpaces(buffer, pos);
				p = strpbrk(buffer + pos, " ,(}:");
				if (p == NULL)
					SyntaxError(13);
				d = atof(buffer + pos);
				//prot << d << ' ';
				myendint=d;
				
				pos = p - buffer;
				SkipSpaces(buffer, pos);

				SkipSpaces(buffer, pos);
				if(buffer[pos]!=':')
					SyntaxError(99); // 99 - ':' absent
				pos++;

				SkipSpaces(buffer, pos);
				p = strpbrk(buffer + pos, " ,(}:");
				if (p == NULL)
					SyntaxError(13);
				d = atof(buffer + pos);
				//prot << d << ' ';
				mypower=d;
				
				int i;
				if (myendint< mystartint)
					SyntaxError(100);//  end of interval is less then begin

				for(i=mystartint;i<=myendint;i++)
					tempClustInfo.AddProc(i,mypower);

				SkipSpaces(buffer, pos);
				p = strpbrk(buffer + pos, " ,(}:");
				pos=p-buffer;
				SkipSpaces(buffer, pos);
								
				if (buffer[pos] == '}') {
					loop = false;
					++pos;

					SkipSpaces(buffer, pos);
					if (buffer[pos] != '}') 
						SyntaxError(24);
					++pos;
					SkipSpaces(buffer, pos);
					if (buffer[pos] != ';') 
						SyntaxError(26);

				} else if (buffer[pos] == ',') {
					++pos;
				} else
					SyntaxError(18);
			}		// end while(loop)
		
			completePS.MakeNewCluster(clustNum);

			for(i=0;i<tempClustInfo.Procs.size();i++)
			{
				ProcInfo tempPI;
				tempPI.numClust=tempClustInfo.numClust;
				tempPI.numInClust=tempClustInfo.Procs[i].numInClust;
				tempPI.ProcPower=tempClustInfo.Procs[i].ProcPower;
				completePS.AddProcToClust(tempClustInfo.numClust,i,tempPI);
			}			
		

		} else if (strncmp(buffer + pos, "mapping", strlen("mapping")) == 0) {

			int clustNum;
			int start, end, step;
			mapflag=false; //====//
			
			if(procflag) 
			{ cout<<"ERROR> Cannot map vector of processors without procpower vector\n";
				prot<<"ERROR> Cannot map vector of processors without procpower vector\n";
				cout<<"ERROR> Set procpower vector before mapping OR use default values\n";
				prot<<"ERROR> Set procpower vector before mapping OR use default values\n";
				SyntaxError(25);
			}



			pos += strlen("mapping");
            //проверить знак '='
			SkipSpaces(buffer, pos);
			if (buffer[pos] != '=') 
				SyntaxError(11);
			wasmapped=true;

			//проверить '{'
			SkipSpaces(buffer, ++pos);
			if (buffer[pos] != '{') 
				SyntaxError(12);
			++pos;

			loop=true;
			while(loop)
			{

			//get cluster num
			SkipSpaces(buffer, pos);
			p = strpbrk(buffer + pos, " ,(}");
			if (p == NULL)
				SyntaxError(13);
			clustNum = atoi(buffer + pos);
			//prot << d << ' ';
			pos = p - buffer;
			SkipSpaces(buffer, pos);

			//проверить ':'
			if (buffer[pos] != ':') 
				SyntaxError(76);
			++pos;

			//get start
			SkipSpaces(buffer, pos);
			p = strpbrk(buffer + pos, " ,(}");
			if (p == NULL)
				SyntaxError(13);
			start = atoi(buffer + pos);
			//prot << d << ' ';
			pos = p - buffer;
			SkipSpaces(buffer, pos);

			//get end
			SkipSpaces(buffer, pos);
			p = strpbrk(buffer + pos, " ,(}");
			if (p == NULL)
				SyntaxError(13);
			end = atoi(buffer + pos);
			//prot << d << ' ';
			pos = p - buffer;
			SkipSpaces(buffer, pos);

			//проверить ':'
			if (buffer[pos] != ':') 
				SyntaxError(76);
			++pos;

			//get step
			SkipSpaces(buffer, pos);
			p = strpbrk(buffer + pos, " ,(}");
			if (p == NULL)
				SyntaxError(13);
			step = atoi(buffer + pos);
			//prot << d << ' ';
			pos = p - buffer;
			SkipSpaces(buffer, pos);
			
            mappedProcs.AddProccessors(start,end,step,completePS.Clusters[clustNum]);
			
			if (buffer[pos] == '}') {
					loop = false;
					++pos;

				
					SkipSpaces(buffer, pos);
					if (buffer[pos] != ';') 
						SyntaxError(26);

				} else if (buffer[pos] == ',') {
					++pos;
				} else
					SyntaxError(18);
			}		// end while(loop)


            double min_weight=1.e7;
			int i;
			for(i=0;i<mappedProcs.Processors.size();i++)
			{
				vProcPower.push_back(mappedProcs.Processors[i].ProcPower);
				//min_weight = min_weight > mappedProcs.Processors[i].ProcPower ? mappedProcs.Processors[i].ProcPower :  min_weight;
			}

		/*	for(i=0;i<mappedProcs.Processors.size();i++)
			{
				
				vProcPower[i]=vProcPower[i];//!!!/min_weight;
				mappedProcs.Processors[i].ProcPower=mappedProcs.Processors[i].ProcPower;//!!!/min_weight;
        	}*/
	
// grig add-on		
		}
		else if (strncmp(buffer + pos, "start", strlen("start")) == 0) {
			// START [ TIME ] ------------------------------------------------------------
			pos += strlen("start");
			SkipSpaces(buffer, pos);

			if (strncmp(buffer + pos, "time", strlen("time")) == 0) {
				pos += strlen("time");
				SkipSpaces(buffer, pos);
			}

			if (buffer[pos] != '=') 
				SyntaxError(19);

			SkipSpaces(buffer, ++pos);

			p = strpbrk(buffer + pos, " ;");
			if (p == NULL) 
				SyntaxError(20);
			TStart = atof(buffer + pos);
			pos = p - buffer;
			SkipSpaces(buffer, pos);

//			prot << "TStart = " << TStart << endl;

		} else if (strncmp(buffer + pos, "send", strlen("send")) == 0) {
			// SEND [ BYTE ] [ TIME ] ------------------------------------------------------
			pos += strlen("send");
			SkipSpaces(buffer, pos);
			
			if (strncmp(buffer + pos, "byte", strlen("byte")) == 0) {
				pos += strlen("byte");
				SkipSpaces(buffer, pos);
			}
			
			if (strncmp(buffer + pos, "time", strlen("time")) == 0) {
				pos += strlen("time");
				SkipSpaces(buffer, pos);
			}

			if (buffer[pos] != '=') 
				SyntaxError(21);

			SkipSpaces(buffer, ++pos);

			p = strpbrk(buffer + pos, " ;");
			if (p == NULL) 
				SyntaxError(22);

			TByte = atof(buffer + pos);
			pos = p - buffer;
			SkipSpaces(buffer, pos);

//			prot << "TByte = " << TByte << endl;

		} else {
		}
		if (buffer[pos] != ';') {
			SyntaxError(23);
		}
		++pos;
		SkipSpaces(buffer, pos);
	} 
#ifdef nodef
    // while ((buffer[pos] == ';') || (buffer.npos != pos)); 
	if (weight_list.size() == 0) {
		// set default weights for root topology
		dbl.resize(0);
		k = 1;
		for (i = 0; i < SizeArray_list.front().size(); i++)
			k *= SizeArray_list.front()[i];

		for (i = 0; i < k; i++)
			dbl.push_back(1.0);
		weight_list.push(dbl);
	}
#endif

	/* //====//
//grig add-on
	this->ProcPower=1.0;
	if(!wasmapped)
	{
		for(i=0;i<this->completePS.Clusters.size();i++)
			this->mappedProcs.AddProccessors(0,this->completePS.Clusters[i].Procs.size() -1,1,this->completePS.Clusters[i]);
	}

	// correct  weights function


	//\grig add-on
	*/ //====//
	
//==== значения по умолчанию
//printf("Flags %d %d\n",procflag,mapflag);

		if(procflag && mapflag)
		{
			ClustInfo tempClustInfo;
			tempClustInfo.Procs.resize(0);
			int clustNum, mystartint,myendint;
			double mypower;

			clustNum = 1;
			tempClustInfo.setNum(clustNum);
			mystartint=0;
			myendint=99999; //максимум 100000 процов в кластере
			if(ProcPower > 0.000000001) mypower=ProcPower;
			else mypower=1;

//			printf("Mypower=%f\n",mypower);

			for(i=mystartint;i<=myendint;i++)
			{ tempClustInfo.AddProc(i,mypower);
			}
				
			completePS.MakeNewCluster(clustNum);

			for(i=0;i<tempClustInfo.Procs.size();i++)
			{
				ProcInfo tempPI;
				tempPI.numClust=tempClustInfo.numClust;
				tempPI.numInClust=tempClustInfo.Procs[i].numInClust;
				tempPI.ProcPower=tempClustInfo.Procs[i].ProcPower;
				completePS.AddProcToClust(tempClustInfo.numClust,i,tempPI);
			}			
			
      mappedProcs.AddProccessors(mystartint,myendint,1,completePS.Clusters[clustNum]);

//      double min_weight=1.e7;
			int i;
			for(i=0;i<mappedProcs.Processors.size();i++)
			{
				vProcPower.push_back(mappedProcs.Processors[i].ProcPower);
				//min_weight = min_weight > mappedProcs.Processors[i].ProcPower ? mappedProcs.Processors[i].ProcPower :  min_weight;
			}

		}

		if(!procflag && mapflag)
		{ prot<<" ERROR> Use procpower vector without mapping\n";
			cout<<" ERROR> Use procpower vector without mapping\n";
			SyntaxError(25);
		}
//=***

	prot << "Reading predictor options done." << endl;
}



//grig
void PS::reset()
	{
	if(ps_lb_list.size()!=0) ps_lb_list.pop();		// low bounderies on each dim	 
	if(SizeArray_list.size()!=0)SizeArray_list.pop();	// extentions  on each dim
	if(weight_list.size()!=0)weight_list.pop();	// list of vectors - PS weights
	currentPS_ID = 0;
	}


//\grig

//---------------------------------------------------------------------------------------
//
//                               MEMBERS
//
//----------------------------------------------------------------------------------------
// Установить размеры решетки процесоров используя параметры , полученные из командной строки
//
//
void	PS::setTopology(vector<long>& ASizeArray)
{
		LongVector			int1;
		int1.resize(ASizeArray.size(), 0);
		ps_lb_list.push(int1);
		SizeArray_list.push(ASizeArray);
		
}


//grig
// Служебная функция 
//
//
int PS::getProcCount()
{
     	long s=1;

		if(SizeArray_list.size()!=0)
		{
		LongVector					sz = this->SizeArray_list.front();
		LongVector::const_iterator	sz_vec_iter;
		for (sz_vec_iter = sz.begin();sz_vec_iter<sz.end();sz_vec_iter++)		
		{
		s=s* (*sz_vec_iter);          
		}
		
		//printf("size of ps is %d\n",s);

	    return s;
		}
		else 
			return 0;
}

void PS::CorrectMappedProcs()
{
/*	int cp=this->getProcCount();
	int i;

	this->vProcPower.resize(0);

	if (cp<this->mappedProcs.Processors.size())
	{
		for(i=0;i<cp;i++)
			this->vProcPower.push_back(this->mappedProcs.Processors[i].ProcPower);
	}
	else
	{
		int k=this->mappedProcs.Processors.size();
		for(i=0;i<k;i++)
			this->vProcPower.push_back(this->mappedProcs.Processors[i].ProcPower);

		ClustError(6);
		
	}



*/

}

void  PS::PrepareForAutoSearch(std::vector<long>& perstanovki) // установить полное отображение 
// возвращает  массив содержащий параметр номера перестановок ()
{
	std::vector<double> procs_powers;
	std::vector<long>   procs_num;
	

	mappedProcs.Processors.resize(0);
	MappedProcs before_sort;
	

	//отобразить все процессоры
	int i,j;
	strProcInfo strProcOne;
	for(i=0;i<CurrentCluster->GetProcCount();i++)
	{
	    strProcOne.numClust=0;
	    strProcOne.numInClust=i;
	    strProcOne.ProcPower=CurrentCluster->GetProcPower(i);
	    
	    before_sort.AddProc(strProcOne);
	}
/*	for(i=0;i<completePS.Clusters.size();i++)
		//for(j=0;j<completePS.Clusters[i].Procs.size();j++)
			before_sort.AddProccessors(0,completePS.Clusters[i].Procs.size()-1,1,completePS.Clusters[i]);
*/	
	// отсортировать их по убыванию производительности
    for(i=0;i<before_sort.Processors.size();i++)
		for(j=i;j<before_sort.Processors.size();j++)
		{
			if(before_sort.Processors[i].ProcPower<before_sort.Processors[j].ProcPower)
			{
				strProcInfo temp;
				temp=before_sort.Processors[j];
				before_sort.Processors[j]=before_sort.Processors[i];
				before_sort.Processors[i]=temp;
			}
		}
// на самом деле ищем минимум
	double max_w=before_sort.Processors[0].ProcPower;

	for(i=0;i<before_sort.Processors.size();i++)
	{
		if(before_sort.Processors[i].ProcPower<max_w)
		{
		 max_w=before_sort.Processors[i].ProcPower	;
		}
	} // хотя чисто теоретически мах - первый элемент

    
	for(i=0;i<before_sort.Processors.size();i++)
	{
		 before_sort.Processors[i].ProcPower=before_sort.Processors[i].ProcPower;// !!!!!/max_w;
//		 prot<<"ProcPower["<<i<<"] = "<<before_sort.Processors[i].ProcPower<<"\n";
		 mappedProcs.AddProc(before_sort.Processors[i]);
	} 
}
//\grig







void	PS::nextPS(vector<long>& lb, vector<long>& ASizeArray, mach_Type& AMType, 
				   int& AnumChanels, double& Ascale, double&	ATStart, 
				   double&  ATByte, double& AProcPower,vector<double>& AvProcPower)
{
#ifdef P_DEBUG
//	prot << *this << endl;
#endif

	lb = ps_lb_list.front();
	ps_lb_list.pop();
	ASizeArray = SizeArray_list.front();
	//SizeArray_list.pop();//grig!!!
	AMType = Type;
	AnumChanels = numChanels;
	Ascale = scale;
	ATStart = TStart;
	ATByte = TByte;
	AProcPower = 1.0;
		//ProcPower;
	//grig

	AvProcPower.resize(0);

	int i;
	int k=this->getProcCount();
	for(i =0;i<k;i++)
	{
//		AvProcPower.push_back(mappedProcs.Processors[i].ProcPower);
		AvProcPower.push_back(CurrentCluster->GetProcPower(CurrentCluster->map(i)));
	}
	//\grig
}

void	PS::hardwarePS(int& AMType, double&	ATStart, double&  ATByte, double& AProcPower , vector <double> &AvProcPower)
{
#ifdef P_DEBUG
	prot << *this << endl;
#endif
	AMType = Type;
	ATStart = TStart;
	ATByte = TByte;
	AProcPower = ProcPower;

	//grig
	AvProcPower.resize(0);
	for(int i =0;i<this->mappedProcs.Processors.size();i++)
	{
		AvProcPower.push_back(mappedProcs.Processors[i].ProcPower);
	}
	//\grig
}


#ifdef P_DEBUG

ostream& operator << (ostream& os, const PS& ps)
{
	os << "PROCESSOR:" << endl;
	os << "          TYPE = ";
	if (ps.Type == mach_ETHERNET)
		os << "ETHERNET";
	else if (ps.Type == mach_TRANSPUTER)
		os << "TRANSPUTER";
	else if (ps.Type == mach_MYRINET) 
		os << "MYRINET(" << ps.numChanels <<')';
	os << endl;

	os << "          SCALE = " << ps.scale << endl;
	os << "          POWER = " << ps.ProcPower << endl;
	os << "          START TIME = " << ps.TStart << " (MKS)" <<endl;
	os << "          SEND BYTE TIME = " << ps.TByte << " (MKS)" << endl;

	if (ps.ps_lb_list.size() != 0) {
		LongVector					lb = ps.ps_lb_list.front();	 
		LongVector					sz = ps.SizeArray_list.front();
		LongVector::const_iterator	lb_vec_iter;
		LongVector::const_iterator	sz_vec_iter;
		
		os << "          TOPOLOGY = { ";
		for (lb_vec_iter = lb.begin(), sz_vec_iter = sz.begin();
			lb_vec_iter < lb.end();
			lb_vec_iter++, sz_vec_iter++ ) {
				os << *lb_vec_iter << ':' << *lb_vec_iter + *sz_vec_iter - 1;
			if (lb_vec_iter < lb.end() - 1)
				os << ',';

		}
		os << " };" << endl;
	}

	if (ps.weight_list.size() > 0) {
		// list of vectors - PS weights
		DoubleVector	weight_vec = ps.weight_list.front();
		DoubleVector::const_iterator	weight_vec_iter;

		os << "          WEIGHT = { ";
		for (weight_vec_iter = weight_vec.begin(); 
			weight_vec_iter < weight_vec.end(); 
				weight_vec_iter++) {
			os << *weight_vec_iter;
			if (weight_vec_iter < weight_vec.end() -1)
				os << ',';
		}
		os << " };";
	}
	return os;
}


#endif

