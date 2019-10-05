#include <stdio.h>
#include <string.h>

#include <exception>
#include <fstream>
#include <iomanip>
#include <vector>
#include <math.h>
//====
#ifdef _MSC_VER
/*Windows*/
#include <io.h>
#else
/*Unix*/
#include <sys/types.h>
#include <dirent.h>
#endif
//=***
 
#include "Ver.h"
#include "Ps.h"
#include "TraceLine.h"
#include "FuncCall.h"
#include "Interval.h"
#include "Vm.h"
#include "ModelStructs.h"
using namespace std;

root_Info *Get_Root_Info();

ofstream prot;
int search_opt_mode;

vector<long>	SizeArray;

//====
extern long StrToLong(char* str, int base);

struct tr_line
{ int func_id;
	double time;
	char *mask;
	long **extra_koef;
};
//=***

// External functions

extern void		SaveHTMLInterval(ofstream hfile);

extern void		TraceParsingCleanup();
extern			FuncType GetFuncType(Event func_id);
ofstream		hfile;
extern double grig_time_call;
extern long currentAM_ID;


//grig
typedef std::vector<long> long_vect;
typedef std::vector<long_vect> long2_vect;
// для автоматического поиска
long_vect MinSizesOfAM; 
struct conf	
{	long proc_num;		
	long_vect  proc_set;		
	long mark;	
};

struct searched
{	long proc_num;		
	int proc_id_diff;
	long_vect  proc_set;		
	double time;	
};
typedef std::vector<struct searched> searched_vect;
//\ для автоматического поиска
extern bool FirstTrace;
extern _PSInfo			*PSInfo	;
extern _AMInfo			*AMInfo	;
extern _AMViewInfo		*AMViews;

void resetInfos()
{
_PSInfo::count		= 0;
_AMInfo::count		= 0;
_AMViewInfo::count	= 0;
if(PSInfo!=NULL) {free(PSInfo); PSInfo=NULL;}
if(AMInfo!=NULL) {free(AMInfo); AMInfo=NULL;}
if(AMViews!=NULL) {free(AMViews); AMViews=NULL;}
}

//\grig
	
void ModelExec(VectorTraceLine * tl)
{ 
 	FuncCall * func_call;
	while (!tl->end()) {
		func_call = new FuncCall(tl);

		FuncType func_type = GetFuncType(func_call->func_id);

#ifdef P_DEBUG
			prot << " func_id = " << func_call->func_id
				 << " time = " << procElapsedTime[0]
				 << " file = " << func_call->source_file 
				 << " line = " << func_call->source_line
				 << endl;
#endif

		if (func_call->func_id == Event_dvm_exit)
			return;

		switch(func_type) {
			case __IntervalFunc :
			  func_call->IntervalTime();
			  break;
			case __IOFunc :
			  func_call->IOTime();
			  break;
			case __MPS_AMFunc :
			  func_call->MPS_AMTime();
			  break;
			case __DArrayFunc :
			  func_call->DArrayTime();
			  break;
			case __ShadowFunc :
			  func_call->ShadowTime();
			  break;
			case __ReductFunc :
			  func_call->ReductTime();
			  break;
			case __ParLoopFunc :
			  func_call->ParLoopTime();
			  break;
			case __RemAccessFunc :
			  func_call->RemAccessTime();
			  break;
			case __RegularFunc :
			  func_call->RegularTime();
			  break;
			case __UnknownFunc :
			  func_call->UnknownTime();
			  break;
		}
		delete func_call; 
	}
}

void CreateHTMLfile()
{
	//====// сколько цифр после запятой digits
    hfile << setiosflags(ios::fixed) << setprecision(4);
	
	// Write intervals in output file
	if (CurrInterval->count == 0) {
		// there is no nested intervals
		CurrInterval->SaveTree(hfile, 1, CurrInterval->ID, CurrInterval->ID);
	} else {
		CurrInterval->SaveTree(hfile, 1, 
			CurrInterval->nested_intervals[0]->ID, 
				CurrInterval->nested_intervals[CurrInterval->count - 1]->ID);
	}

	// close output file
	hfile.close();
}


static void message()
{
	std::cerr << "ERROR  : missing required command line parameter." << endl;
	std::cerr << "SYNTAX : predictor <param_file> <trc_file> <html_file> <processors>" << endl;
	std::cerr << "where  : <param_file> - parameter file name," << endl;
	std::cerr << "       : <trc_file> - trace file name" << endl;
	std::cerr << "       : <html_file> - resulting HTML file name," << endl;
	std::cerr << "       : <processors> - processors topology," << endl; 
	std::cerr << "                        i.e. extension on each dimension," << endl;
	std::cerr << "                        separated by the space." << endl;
	exit(EXIT_FAILURE);
}


void Getsimplefactors(std::vector<long> & result,int N)   // получить все простые множители!!!
{
	int i;
	int del1;
	int tempN;

	result.resize(0);
	tempN=N;
     
	i=1;
	while(true)
	{
		if(i>sqrt((float)N)) break;
		del1=tempN/i;
		if(i*del1==tempN) //- делитель
		{
//			printf("%d ",i);
			tempN=del1;
			result.push_back(i);		
			if(i==1)
				i++;
		}		
		else
			i++;
	}
	if(tempN!=1)
	{
	result.push_back(tempN);
//	printf("%d ",tempN);
	}

//	printf("\n");
}

void GetAllFactors(std::vector<long>& result,int N)   // получить все делители числа
{
	int i;
	int i1;
	result.resize(0);
	for(i=1;i<=N;i++)
	{
		i1=N/i;
		if(i1*i==N) result.push_back(i);
	}
}

void getRNK(std::vector<long> res,int N,int K,std::vector<long_vect>& glob_res) // получить разложение N на K множителей
{
	if(K==1)
	{
		std::vector<long> temp1;
		temp1=res;
		temp1.push_back(N);
	  //res.push_back(N);
	  glob_res.push_back(temp1);
	}
	else
	{
	  std::vector<long> temp_fact;
 	  GetAllFactors(temp_fact,N);
	  for(int i=0;i<temp_fact.size();i++)
	  {
		  std::vector<long> temp_2;
		  temp_2=res;
		  temp_2.push_back(temp_fact[i]);		  
		  getRNK(temp_2,N/temp_fact[i],K-1,glob_res);
	  }
	}
}

void getNK(std::vector<long_vect>& res,int N,int K)   // получить все разложения N на K множителей
{
	res.resize(0);
	std::vector<long> temp;
	temp.resize(0);
	getRNK(temp,N,K,res);
}

bool MakeAllConfigurations(int proc_number,int rank,std::vector<long_vect>& result)
{
	// получить разложение  proc_number на множители
	// проверить количество  этих множителей 
	// если их меньше чем ранк  то не судьба 	
	//==// а точнее, такая конфигурация уже встречается при меньшем ранге, 
	//==// поэтому если мы пробегаем по всем рангам, то не стоит этого делать еще раз, а если не пробегаем, то надо сделать
	//==// я выбрал путь не пробегать !
	std::vector<long> factors;

	Getsimplefactors(factors,proc_number);

//==//	if(factors.size()<rank)		return false;

	getNK(result,proc_number,rank);
	return true;
}

float GetEuristik(long j, long a_size /*, long p, long first_p, long last_p*/)
{
	long q;
	double ost;

	q=(long)ceil(((float)a_size)/j);
	ost=(double)((a_size%q)?(a_size%q):q);

	if(q*(j-1)+ost>a_size)// то есть на последнем проце нет элементов, потому что все разобрали раньше, 
		ost=0; //	что уступает всегда случаю с меньшим количеством процессоров

	ost/=q;

	return ost;

}

int CheckEuristik(std::vector<long> & who)
{ bool mode=0, flag=1;
	int i;
	double ost, min_ost=10001;

	for(i=0;i<who.size();i++)
	{
		if(who[i]>MinSizesOfAM[i])	return 0; // always bad
	}

	for(i=0;i<who.size();i++)
	{

		ost=GetEuristik(who[i],MinSizesOfAM[i]);
		
		if(ost<min_ost) min_ost=ost;
	}

	if(mode) for(int ii=0;ii<who.size();ii++) printf("%d ",who[ii]); 
		
	return (int)(min_ost*10000); 
} 

bool IsBestConfiguration(Interval* best,Interval * current)
{
//	prot<<"curr="<<current->GetEffectiveParameter() <<"; best="<<best->GetEffectiveParameter()<<"\n";
	if(best->GetEffectiveParameter() <= current->GetEffectiveParameter())
		return false;
	return true;
}

//====

bool match(char* name, char *mask)
{ char *s;
	int i,j,k, name_len=strlen(name), mask_len=strlen(mask);

//	printf("Matching... '%s' '%s'\n",name,mask);
	s=(char *)malloc((name_len+1)*sizeof(char));

	for(i=0;i<mask_len;i++)
	{ if(mask[i]!='*' && mask[i]!=name[i]) return false;
		if(mask[i]=='*')
		{
			for(j=i;j<name_len;j++)
			{ //printf("i=%d j=%d\n",i,j);
				for(k=i;k<=j;k++)
					s[k-i]=name[k];
				s[k-i]=0;
//				printf("'*'=='%s'\n",s);
				if(match(name+j+1,mask+i+1)) return true;
			}
			return false;
		}
	}
	return mask_len==name_len;
}


int multy_trace(char *mask, char *outfile)
{
	int tnum=0,i,j,k, trok[128], minline, activenum, printed, slen, anum;
	VectorTraceLine *traces[128];	
	TraceLine *tl, *tls[128];
	Event curfunc;
	double tm;
	char *txt, *pos, *infile=NULL, tbuf[1024], *p;
	FILE *outf, *inf;

	bool flag;
	int proc_num=1;
	int current_proc=0;


//	prot.open("mt.log");



	flag=0;
//	printf("mask='%s'\n",mask);
	for(i=0;i<strlen(mask);i++)
		if(mask[i]=='@') { mask[i]='*'; flag=1;}
	
	if(flag==0)
	{
		strncpy(tbuf,mask,strlen(mask)-4);
		tbuf[strlen(mask)-4]=0;
		strcat(tbuf,"0_0.ptr");

//		printf("tbuf=%s\n",tbuf);

		outf=fopen(tbuf,"r");
		if(outf==NULL)
		{
			printf("No trace file %s\n",tbuf);
			exit(1);
		}
		else
		{
			for(i=0;i<256;i++) //для верности чтобы найти слово ProcCount
			{
				fscanf(outf,"%s",tbuf);
				if(!strncmp(tbuf,"ProcCount",9))
				{
					proc_num=atoi(tbuf+10);
					break;
				}
			}

		}
		fclose(outf);

	}

//	printf("Trace proc num=%d\n",proc_num);

//	printf("Out File = '%s'\n",outfile);
	outf=fopen(outfile,"w");


	if(flag==0)
	{
		while(current_proc<proc_num)
		{
			strncpy(tbuf,outfile,strlen(outfile)-4);
			tbuf[strlen(outfile)-4]=0;
			sprintf(tbuf,"%s%d_%d.ptr",tbuf,current_proc,current_proc);
			current_proc++;

//			printf("mask='%s'\n",tbuf);

			prot<<"Reading "<<tbuf<<"\n";
			traces[tnum++] = new VectorTraceLine(tbuf);
			if (!infile) infile = strdup(tbuf);
		}
	}
	else
	{

#ifdef _MSC_VER
	struct _finddata_t fnd;
	long fh;


	fh = _findfirst(mask, &fnd);
	if (fh<0) {
		printf("Files not found.");
		return 0;
	}

	if (strcmp(fnd.name, outfile)) {
		prot<<"Reading "<<fnd.name<<"\n";
		traces[tnum++] = new VectorTraceLine(fnd.name);
		infile = _strdup(fnd.name);
	}

	while(!_findnext(fh, &fnd)) {
		if (!strcmp(fnd.name, outfile))
			continue;

		prot<<"Reading "<<fnd.name<<"\n";
		traces[tnum++] = new VectorTraceLine(fnd.name);
		if (!infile) infile = _strdup(fnd.name);
	}
	_findclose(fh);
#else
  DIR*                handle;
  char *name;
   struct dirent*      Dirstruct;

   handle = opendir(".");
   if (handle == NULL) return 1; 
 do                                                               
 {                                                                  
      Dirstruct = readdir(handle);   
     if (Dirstruct != NULL)                  
           name = Dirstruct->d_name; 
     if(match(name,mask) && strcmp(name, outfile))
	{
	    if(!infile) 
	    { infile=(char *)malloc((strlen(name)+1)*sizeof(char));
	      strcpy(infile,name);
	    }
//    	    printf("File open '%s'\n",name);
	    prot<<"Reading "<<name<<"\n";
	    traces[tnum++] = new VectorTraceLine(name);
	}
   } while (Dirstruct != NULL);
   closedir(handle);           

#endif

   }

	activenum = tnum;
//	printf("Generating new trace file %s..\n",outfile);

	printed = 0;
	for(i=0;i<tnum;i++) { //prepare
		tls[i] = traces[i]->current();
		while(tls[i] && !tls[i]->source_line) {
			if (!printed)
			{
				fprintf(outf,"%s\n",tls[i]->info_line);
//				printf("%s\n",tls[i]->info_line);
			}
			tls[i] = traces[i]->next();
		}
		printed=1;
		if (!tls[i]) {
			trok[i]=0; 
			activenum--;
			continue;
		}
		trok[i] = 1; 
	}
	
	for(j=0;j<tnum;j++)
		if (trok[j]) {
			for(i=0;i<traces[j]->p_size;i++)
				fprintf(outf,"%s\n",traces[j]->p_lines[i]);
			break;
		}

	while(activenum>0) {
			 
		minline = 1000000;	
		for(i=0;i<tnum;i++) // find minline
			if (trok[i] && tls[i]->source_line < minline) {
				minline = tls[i]->source_line;
				curfunc = tls[i]->func_id;
			}

		printed = 0; tm=0; anum = 0;
		for(i=0;i<tnum;i++) // work it
			if (trok[i] && tls[i]->source_line == minline 
				&& tls[i]->func_id == curfunc) {
				tl = tls[i];
//			printf("%d) %s\n", i, tls[i]->info_line);
				tm += tls[i]->func_time;
				anum++;
			}
			
		for(i=0;i<tnum;i++) // advance
			if (trok[i] && tls[i]->source_line == minline 
				&& tls[i]->func_id == curfunc) {

				if (!printed) {
//					txt = _strdup(tl->info_line);
					txt=(char *)malloc((strlen(tl->info_line)+1)*sizeof(char));
					strcpy(txt,tl->info_line);
					
					slen = strlen(txt);
					if (tls[i]->line_type == Ret_) {
						tm /= anum;
						switch(tls[i]->func_id) {
						case strtrd_:
						case strtsh_:
						case waitrd_:
						case waitsh_: 
						case redis_:
						case realn_:
						case arrcpy_: tm = 0.000001;
						}
					}
					if(pos = strstr(txt,"TIME")) {
						pos+=5;
						p = txt; k = 0;
						while(p!=pos) {
							tbuf[k++] = *p;
							p++;
						}
						sprintf(&(tbuf[k]),"%8.8lf",tm);
						while(tbuf[k]) k++;
						while((*p)!=' ') p++;
						while(*p) {
							tbuf[k++] = *p;
							p++;
						}
						tbuf[k]=0;
					}
					fprintf(outf,"\n%s\n",tbuf);
					free(txt);
				}

				tls[i] = traces[i]->next();
				while(tls[i] && !tls[i]->source_line) {
					if (!printed)
						fprintf(outf,"%s\n",tls[i]->info_line);
					tls[i] = traces[i]->next();
				}
				printed = 1;
				if (!tls[i]) {
					trok[i]=0; 
					activenum--;
					continue;
				}
				trok[i] = 1;
			}

	} // main loop

	if (infile) {
//		printf("infile=%s\n",infile);
		inf = fopen(infile,"rt");
		if (!inf) {
			printf("Can't open file %s\n",infile);
		} else {
			txt = (char*)calloc(1024,1);
			do {
				pos = fgets(txt,1024,inf);
				//if (pos) printf(",");
			} while(pos && !strstr(txt,"dvm_exit"));
			if (pos) {
				do
					fprintf(outf,"%s",txt);
				while(fgets(txt,1024,inf));
			} else {
				printf("dvm_exit not found!\n");
			}
			free(txt);
			fclose(inf);
		}
		free(infile);
	}
	fclose(outf);
//	printf("Done!\n");

	return 0;
}

//=***


int extra_trace(char *outfile)
{ char *mask, s[256], *buf;
	char tnum,parnum,flag;
	int i, j, d[10], k, m, best[10], price[10], best_var[10], best_price;
	long n, num_c, *last_SizeArray, last_SizeArray_mid;
	VectorTraceLine *traces[128];	
	TraceLine *tls[128];
	char *infile=NULL;
	bool mask_flag=false;
	int *par[128];
	FILE *outf, *inf;
	char print_mode=0; //0-no print   1-only times (OUT CMP)	2-all times		3-time+param (OUT CMP) 4-all params and times


	printf("Collection of extra trace into %s\n",outfile);
	mask=(char*)malloc(sizeof(char)*strlen(outfile));

	// получаем параметры из целевой трассы (которую будем строить)
	for(i=0,j=0,parnum=0; ;i++)
	{
		if(outfile[i]=='@' && outfile[i+1]=='@')
		{
			if(outfile[i+2]>='0' && outfile[i+2]<='9') 
			{
				mask[j++]='@';
				mask[j++]='@';
				i+=2;
				
				while(1)
				{
					d[parnum++]=atoi(outfile+i);
					mask[j++]='*';

					while(outfile[i]>='0' && outfile[i]<='9') 
						i++;

					if(outfile[i]=='_') { mask[j++]='_'; i++; continue;}
					break;
				}
			}
			else return 1; //ошибка вызова эктраполятора
		}
		mask[j++]=outfile[i];

		if(outfile[i]==0)  break;
	}

	if(parnum==1) printf("parnum=%d param(%d)\n",parnum,d[0]);
	if(parnum==2) printf("parnum=%d param(%d,%d)\n",parnum,d[0],d[1]);
	printf("mask=%s\n",mask);
	
	tnum=0;
//получаем параметры из тех трасс, которые уже есть и находятся в текущей директории, чтобы их использовать для экстраполяции
#ifdef _MSC_VER
	struct _finddata_t fnd;
	long fh;

//	printf("mask='%s'\n",mask);

	fh = _findfirst(mask, &fnd);
	if (fh<0) {
		printf("Files not found.");
		return 0;
	}

	do
	{	if (strcmp(fnd.name, outfile))
		{
			par[tnum]=(int *)malloc(sizeof(int)*parnum);

			// заполняем par[tnum] параметрами из fnd.name
			for(i=0,j=0,k=1; fnd.name[i]!=0 && k;i++)
			{	if(fnd.name[i]=='@' && fnd.name[i+1]=='@' && fnd.name[i+2]>='0' && fnd.name[i+2]<='9') 
				{	i+=2;
					while(1)
					{ if(j>=parnum) { k=0; break;} //не соответствует число параметров
						par[tnum][j++]=atoi(fnd.name+i);
					
						while(fnd.name[i]>='0' && fnd.name[i]<='9') 
							i++;

						if(fnd.name[i]=='_') { i++; continue;}
						break;
					}
				}
			}

			if(k==0) continue;

			if(parnum==1) printf("par[%d]=%d\n",tnum,par[tnum][0]);
			if(parnum==2) printf("par[%d]=%d %d\n",tnum,par[tnum][0],par[tnum][1]);

			tnum++;
		}
	} while (!_findnext(fh, &fnd));

	_findclose(fh);
#else
  DIR*                handle;
  char *name;
	struct dirent*      Dirstruct;

	handle = opendir(".");
	if (handle == NULL) return 1; 
	do                                                               
	{                                                                  
		Dirstruct = readdir(handle);   
		if (Dirstruct != NULL)                  
			name = Dirstruct->d_name; 
		if(match(name,mask) && strcmp(name, outfile))
		{
			if(!infile) 
			{ infile=(char *)malloc((strlen(name)+1)*sizeof(char));
				strcpy(infile,name);
			}

			par[tnum]=(int *)malloc(sizeof(int)*parnum);

			// заполняем par[tnum] параметрами из name
			for(i=0,j=0,k=1; name[i]!=0 && k;i++)
			{	if(name[i]=='@' && name[i+1]=='@' && name[i+2]>='0' && name[i+2]<='9') 
				{	i+=2;
					while(1)
					{ if(j>=parnum) { k=0; break;} //не соответствует число параметров
						par[tnum][j++]=atoi(name+i);
						
						while(name[i]>='0' && name[i]<='9') 
							i++;

						if(name[i]=='_') { i++; continue;}
						break;
					}
				}
			}

			if(k==0) continue;
			if(parnum==1) printf("par[%d]=%d\n",tnum,par[tnum][0]);
			if(parnum==2) printf("par[%d]=%d %d\n",tnum,par[tnum][0],par[tnum][1]);

			tnum++;
		}
	} while (Dirstruct != NULL);
	closedir(handle);           

#endif

	//находим наилучшее дерево трасс чтобы минимальное расстояние между соответствующими параметрами было максимальным
	for(i=0,best_price=0;i<tnum;i++)
	{ 
		for(k=0,flag=0;k<parnum;k++)
		{ 
//			printf("Try %d variant by dim %d\n",i,k);
			for(j=0,price[k]=0;j<tnum;j++)
			{ if(i==j) continue;
//				printf("par[i=%d]=%d %d   par[j=%d]=%d %d",i,par[i][0],par[i][1],j,par[j][0],par[j][1]);
				for(m=0;m<parnum;m++)
				{
					if(m==k) continue;
					if(par[j][m]!=par[i][m]) break; //этот вариант не устраивает по этому измерению	
				}

				if(flag==k && m==parnum) flag=k+1;

				if(flag==k+1) 
				{ if(price[k] < abs(par[j][k]-par[i][k])) 
					{ price[k]=abs(par[j][k]-par[i][k]); 
						best[k]=j;
					}
				}

//				if(m==parnum) printf("   ok price=%d best=%d\n",price[k],best[k]);
//				else printf("\n");
			}
			if(flag!=k+1) break;

//				if(flag==parnum) printf("We find !!!\n");
		}

		//m=min(price[j])
		for(j=1,m=price[0];j<parnum;j++)
			if(price[j]<m) m=price[j];

		if(m>best_price)
		{ best_price=m;
			best_var[0]=i;
			for(j=0;j<parnum;j++)
				best_var[j+1]=best[j];
		}
	}

	if(best_price==0) return 1; //ошибка того что не нашлось необходимых файлов для экстраполяции

	printf("The best price=%d   Best var=",best_price);
	for(j=0;j<parnum+1;j++)
		printf(" %d",best_var[j]);
	printf("\n");



	//получить из списка параметров + маска = файлы трасс, необходимых для экстраполяции
	buf=(char *)malloc(64*sizeof(char));

	for(j=0;j<parnum+1;j++)
	{ for(i=0,m=0,k=0;   ;i++)
		{ if(mask[i]=='*')
			{ sprintf(buf, "%d", par[best_var[j]][m++]);
				for(   ;buf[0]!=0; buf++)
					s[k++]=buf[0];
			}
			else
				s[k++]=mask[i];			

			if(mask[i]==0) break;
		}
//		printf("s='%s'\n",s);
		prot<<"Reading "<<s<<"\n";
		if(j==0) 
		{	inf=fopen(s,"rt"); // чтобы позже переписать из него концовку в outf
			if(print_mode>=1) traces[127]=new VectorTraceLine("f2_cmp_ideal.txt"); //эталон  (потом убрать!!!)
		}
		traces[j] = new VectorTraceLine(s);
	}


// собственно экстраполяция
//	outf=fopen(outfile,"w"); //пока не в него
	outf=fopen("f2_test.txt","w"); 

	for(i=0;i<traces[0]->p_size;i++)
		fprintf(outf,"%s\n",traces[0]->p_lines[i]);


	double res,tmp,fmin,fmax;
	long min,max,mid, last_call, last_call_type, par_type=-1, my_event=0;
	char *p,*pos,store;
	long **num_arr, *num_arr_mid;

	struct circle
	{
//		vector<tr_line> line;
		long id;
		long2_vect *nums; // nums[trace][appearance_id][par_id]
		long num_cnt;
		char *mask;
		bool mask_done;
	} *circles=NULL;
	int circles_count=0;


	long cur_line=1, circle_line=0;
	long count_lines=0;
	TraceLine *end = new TraceLine ("Event_dvm_exit             TIME=0.00000100 LINE=10000000     FILE=f2.cdv");

	last_SizeArray=(long *)malloc(10*sizeof(long));
	last_SizeArray_mid=1;

	num_arr=(long **)malloc((parnum+1)*sizeof(long*));
	for(i=0;i<parnum+1;i++)
		num_arr[i]=(long *)malloc(100*sizeof(long));
	num_arr_mid=(long *)malloc(100*sizeof(long));


	for(i=0;i<parnum+1;i++)
	{
		tls[i]=traces[i]->current();
		last_SizeArray[i]=1;
	}

	if(print_mode>=1) tls[127]=traces[127]->current(); //потом убрать!!!

	store=0;

	while(1) //main loop
	{
/*		for(i=0;i<parnum;i++)
		{ while(tls[i+1]->func_id != tls[0]->func_id)
				tls[i+1] = traces[i+1]->next();
		}
*/
/*
		switch(tls[0]->func_id)
		{ case dopl_:  break;
			case dvm_void_printf:  break;
			default: ;
		}
*/


		for(i=0;i<parnum+1;i++)
			if(tls[i]==0) 
			{	tls[i]=end;
				printf("dvm exit [%d]\n",i);
			}


		for(i=0;i<parnum+1;i++)
			if(tls[i]!=end) break;

		if(i==parnum+1) break; // break from main loop = end of all traces

/*		for(i=0;i<parnum+1;i++)
			if(tls[i]==end) break;
		if(i!=parnum+1) break; // break from main loop = end of one trace
*/
//printf("beg\n");
		for(i=0,j=-1;i<parnum+1;i++)
		{
//			printf("ti=%d %d\n",tls[i],end);
//			if(tls[i]!=end) printf("tis=%d\n",tls[i]->source_line);
//			printf("i=%d ok\n",i);
			
			if(tls[i]!=end && tls[i]->source_line!=cur_line) { j=i; break;}
		}
//printf("end\n");

		for(i=0;i<parnum+1;i++)
			if(tls[i]!=end && tls[i]->source_line==cur_line) break;
		

		if(i==parnum+1 && tls[j]->source_line!=0)  cur_line=tls[j]->source_line;

//		printf("curline=%d\n",cur_line);

		
		for(i=0,m=0;i<parnum+1;i++)
		{	//if(tls[i]->source_line) printf("time=%8.8f\n",tls[i]->func_time);
			if(tls[i]==end || tls[i]->source_line!=0 && tls[i]->source_line != cur_line) continue; //значит какая-то трасса дошла до события которое еще не встретилось в другой трассе,

			if(print_mode==2 && tls[i]->func_id!=-1 || print_mode==4) printf("%d - %s\n",i,tls[i]->info_line);

			if(i>=1) 
			{	
				flag=(par[best_var[0]][i-1] < par[best_var[i]][i-1]);
			
				if(	tls[0]->func_id==align_ && tls[0]->line_type==1 ||	//ret_align_
						tls[0]->func_id==across_ && tls[0]->line_type==1 ||	//ret_across_
						tls[0]->func_id==dopl_ && tls[0]->line_type==0 )		//call_dopl_
				{	flag=last_SizeArray[0] < last_SizeArray[i];
					min=last_SizeArray[flag?0:i];
					max=last_SizeArray[flag?i:0];
					mid=last_SizeArray_mid;					
//				printf("min=%d max=%d mid=%d\n",min,max,mid);
				}
				else
				{
					min= par[best_var[flag?0:i]][i-1];
					max= par[best_var[flag?i:0]][i-1];
					mid=d[i-1];
				}

				fmin=tls[flag?0:i]->func_time;
				fmax=tls[flag?i:0]->func_time;

//					printf("flag=%d min=%d fmin=%f max=%d fmax=%f\n",flag,min,fmin,max,fmax);

				if(mid <= min) tmp=fmin;
				else if(mid >= max) tmp=fmax;
				else tmp=(fmax-fmin)/(max-min)*mid + fmax-(fmax-fmin)/(max-min)*max;

				if(m==0) res=tmp;
				else res=(res+tmp)/2; //влияние многих параметров на один считаем пока !почти! средним арифметическим
			}
		}

//		res=1.0;

		for(i=0;i<parnum+1;i++)
		{	if(!strstr(tls[i]->info_line,"TIME")) continue;
			int h;
//			printf("i=%d\n",i);

			last_call=tls[i]->func_id; last_call_type=tls[i]->line_type;

			if(last_call==crtpl_ && last_call_type==0) //call crtpl_
			{
				printf("CL = %d\n",circle_line);
				if(circle_line==0) 
				{	for(h=0; h<circles_count; h++)
						if(circles[h].id==cur_line) break;
								
					if(h==circles_count) //первое появление этого цикла в k-той трассе
					{	
						printf("First appearance in trace No %d\n",i);
						circles_count++;
						circles=(circle*)realloc(circles, circles_count*sizeof(struct circle));
						circles[h].id=cur_line;
						circles[h].mask=(char *)malloc(strlen(tls[i]->info_line) + 2);
						circles[h].mask[0]=0;
						circles[h].nums=new long2_vect[parnum+1];
						circles[h].nums[i].resize(1); //it is only the first appearance
						circles[h].num_cnt=0;
						circles[h].mask_done=false;
					}
					else
					{ printf("Appearance in trace No %d\n",i);
						
						circles[h].nums[i].resize(circles[h].nums[i].size()+1); 
					}
					mask_flag=true;
				}
				else //либо второе и более появление в той трассе, где впервые или любой появление в другой
				{
					for(h=0; h<circles_count; h++)
						if(circles[h].id==cur_line) break;
					
					if(h<circles_count) 
					{ printf("appearance in trace No %d\n",i);
						circles[h].nums[i].resize(circles[h].nums[i].size()+1); 
					}
				}

				if(circle_line==0) circle_line=cur_line;
			}


			if(cur_line==circle_line && mask_flag)
			{	
				for(h=0; h<circles_count; h++)
					if(circles[h].id==cur_line) break;

				if(i==0  && !circles[h].mask_done) // в общем случае первый у кого она есть
				{
					circles[h].mask=(char *)realloc(circles[h].mask, strlen(circles[h].mask) + strlen(tls[i]->info_line) + 2);
								
					strcat(circles[h].mask, tls[i]->info_line);
					strcat(circles[h].mask, "\n");
				}

			}
			else 
				mask_flag=false;


			if(last_call==endpl_ && last_call_type==1)	
			{	
				for(h=0; h<circles_count; h++)
					if(circles[h].id==cur_line) break;
				
				if(1)
				{
					printf("-------- circle=%d -------\n",cur_line);
					printf("%s",circles[h].mask);

					for(n=0;n<circles[h].num_cnt;n++)
					{
						for(k=0;k<parnum+1;k++)
						{
							for(m=0;m<circles[h].nums[k].size();m++)
								printf("ptr[%d]var[%d]=%d  \t",k,m,circles[h].nums[k][m][n]);
							printf("    ");
						}
						printf("\n");
					}
					printf("--------------------------\n\n");
				}

				circle_line=0;
				circles[h].mask_done=true;
			}


		}
/*
		if(pos = strstr(tls[0]->info_line,"TIME")) 
		{
			char *tmp_buf; 
			
//			printf("result=%8.8f\n\n", res);

			tmp_buf=(char *)malloc(sizeof(char)*(strlen(tls[0]->info_line)+2));
			pos+=5;
			p=tls[0]->info_line;
			k=0;
			while(p!=pos) 
			{
				tmp_buf[k++] = *p;
				p++;
			}
			sprintf(&(tmp_buf[k]),"%8.8lf",res);
			
			while(tmp_buf[k]) k++;

			while((*p)!=' ') p++;
			while(*p) {
				tmp_buf[k++] = *p;
				p++;
			}
			tmp_buf[k]=0;

			fprintf(outf,"\n%s\n",tmp_buf);

			if(print_mode>=1)
			{
				printf("OUT %s\n",tmp_buf);
				printf("CMP %s\n",tls[127]->info_line);
			}
		}
*/
	
		{ char *tmp_buf, *tmp_mask=0;
			long num_tmp;

			num_c=0; //количество чисел которые надо будет потом вставить в маску

			for(k=0;k<parnum+1;k++)
			{ num_c=0;
				if(tls[k]==end) continue;
				if(strstr(tls[k]->info_line,"TIME"))  continue;

				tmp_mask=(char *)realloc(tmp_mask, sizeof(char)*strlen(tls[k]->info_line));

				for(i=0,j=0,flag=0;	;i++)
				{	
					if(!strncmp(tls[k]->info_line + i,"SizeArray",9))
					{	flag=true; //будет действовать до конца строки
						for(int id=0;id<parnum+1;id++)
							last_SizeArray[id]=1; 
						last_SizeArray_mid=1;
					}

					if(!strncmp(tls[k]->info_line + i,"Size=",5) && last_call==align_ && last_call_type==0)
					{	par_type=num_c;
					}

					if(!strncmp(tls[k]->info_line + i,"InInitIndexArray",16) && last_call==mappl_ && last_call_type==0)
					{	my_event=1;
					}

					if(!strncmp(tls[k]->info_line + i,"InLastIndexArray",16) && last_call==mappl_ && last_call_type==0)
					{	my_event=2;
					}
					
					if(tls[k]->info_line[i]=='=')
					{	tmp_mask[j++]=tls[k]->info_line[i++];
						
						while(tls[k]->info_line[i]==' ') tmp_mask[j++]=tls[k]->info_line[i++];

						if(tls[k]->info_line[i]!='-' && (tls[k]->info_line[i]<'0' || tls[k]->info_line[i]>'9')) { tmp_mask[j++]=tls[k]->info_line[i]; continue;}
						num_tmp=atoi(tls[k]->info_line + i);

						m=i;
						if(tls[k]->info_line[i]=='-') i++; //знак '-' если есть 
						// проверяем является ли оно десятичным
						while(tls[k]->info_line[i]>='0' && tls[k]->info_line[i]<='9')	i++;
						
						if(tls[k]->info_line[i]!=' ' && tls[k]->info_line[i]!=';' && tls[k]->info_line[i]!='\n'  && tls[k]->info_line[i]!=0) //то есть число не закончилось, а значит оно недесятичное
						{ i=m-1; // возвращаемся чтобы занести число как есть в трассе
							continue;
						}

						num_arr[k][num_c++]=num_tmp;
						tmp_mask[j++]='*';
						tmp_mask[j++]=tls[k]->info_line[i];

					}
					else
						tmp_mask[j++]=tls[k]->info_line[i];

					if(tls[k]->info_line[i]==0) break;
				}

				if(circle_line==cur_line && mask_flag)
				{	int h,g,e,f;
					for(h=0; h<circles_count; h++)
						if(circles[h].id==cur_line) break;

	
					if(k==0 && !circles[h].mask_done)
					{	
//						printf("mask='%s'",circles[h].mask);
						circles[h].mask=(char *)realloc(circles[h].mask, strlen(circles[h].mask) + strlen(tmp_mask) + 2);
						
						strcat(circles[h].mask, tmp_mask);
						strcat(circles[h].mask, "\n");
					}
					
					if(num_c>0)
					{
						f=circles[h].nums[k].size()-1;
//if(f>1) exit(0);
						e=circles[h].nums[k][f].size();

						circles[h].nums[k][f].resize(e+num_c); 

//						circles[h].nums[k][f].resize(500); 

						for(g=0; g<num_c; g++)
						{
//							printf("var %d of %d\n",g,num_c-1);
								circles[h].nums[k][f][e+g]=	num_arr[k][g];
//								printf("var %d copy[%d] %d\n",f,k,num_arr[k][g]);
						}


						if(circles[h].nums[k][f].size() > circles[h].num_cnt) circles[h].num_cnt=circles[h].nums[k][f].size();
					}

				}

//			printf("mask[%d]='%s'\n\n",k,tmp_mask); //маски по каждой из трасс
			}
//			printf("mask='%s'\n",tmp_mask);
				


		
			if(my_event==2) 
			{	for(k=0;k<parnum+1;k++) last_SizeArray[k]=1;
				last_SizeArray_mid=1;
			}
/*
			for(i=0;i<num_c;i++)
			{	//f(x,y)=Ax+By+C;   C=f(x0,y0)-Ax0-By0; => f(x,y)=f(x0,y0)+ Ax-Ax0 + By-By0;
				
				for(k=1,num_tmp=num_arr[0][i];k<parnum+1;k++)
				{
					if(i!=par_type) num_tmp+=(num_arr[k][i]-num_arr[0][i]) / (par[best_var[k]][k-1] - par[best_var[0]][k-1]) * ( d[k-1] - par[best_var[0]][k-1]); // += Ax - Ax0
					else	
					{ if(last_SizeArray[0]!=last_SizeArray[1])
							num_tmp=num_arr[0][i]+(num_arr[1][i]-num_arr[0][i]) / (last_SizeArray[1] - last_SizeArray[0]) * ( last_SizeArray_mid - last_SizeArray[0]); // += Ax - Ax0
						//иначе он так и останется num_arr[0][i], то есть не понадобилась экстраполяция
						par_type=-1;
					}

				}
				
				if(my_event==1) //будем использовать конец num_arr чтобы хранить индексы начала цикла
				{ for(k=0; k<parnum+1; k++)
						num_arr[k][99-i]=num_arr[k][i];
					num_arr_mid[i]=num_tmp;
				}
				if(my_event==2) //будем использовать конец num_arr чтобы хранить разницу между параметрами = размерности цикла
				{ 
					for(k=0; k<parnum+1; k++)
						last_SizeArray[k]*=abs(num_arr[k][i]-num_arr[k][99-i])+1;
					last_SizeArray_mid*=abs(num_tmp-num_arr_mid[i])+1;
				}

				if(flag) 
				{ for(k=0;k<parnum+1;k++)
						last_SizeArray[k]*=num_arr[k][i]; 
					
					last_SizeArray_mid*=num_tmp;
				}
				num_arr[0][i]=num_tmp; // нам больше не нужен [0][i] поэтому мы можем в него занести результат
			}

			my_event=0; //закончили обработку my_event

			tmp_buf=(char *)malloc(sizeof(char)*(strlen(tmp_mask)+15*num_c)); //по не более 16 цифр на число

			for(i=0,j=0,k=0;	;i++)
			{ if(tmp_mask[i]=='*') 
				{	sprintf(&(tmp_buf[j]),"%d",num_arr[0][k++]);
					while(tmp_buf[j]!=0) j++;
				}
				else
					tmp_buf[j++]=tmp_mask[i];
				
				if(tmp_mask[i]==0) break;
			}

			fprintf(outf,"%s\n",tmp_buf);

			if(print_mode>=3)
			{
				printf("OUT %s\n",tmp_buf);
				printf("CMP %s\n",tls[127]->info_line);				
			}
*/
		}


//		break; // чтобы обрабатывать только одно первое событие
		//только при условии что они обрабатываются одновременно!!!
		for(i=0;i<parnum+1;i++)
			if(tls[i]->source_line == 0 || tls[i]->source_line == cur_line) 
			{
//				printf("--i=%d tls=%d\n",i,tls[i]);
				tls[i] = traces[i]->next();
//				printf("i=%d tls=%d\n",i,tls[i]);
				
			}
//		printf("TTT tls=%d tls=%d\n",tls[1],tls[127]);
//		printf("TTT tlssss=%d\n",tls[127]->source_line);

//		if(print_mode>=1) 
//			if(tls[127]->source_line==0 || tls[127]->source_line==cur_line) tls[127]=traces[127]->next(); //потом убрать!!!

		if(print_mode>=1) printf("\n");
	}


	if (inf) 
	{ char txt[1024], *pos;
		do {
			pos = fgets(txt,1024,inf);
			//if (pos) printf(",");
		} while(pos && !strstr(txt,"dvm_exit"));

		if (pos) 
		{	fprintf(outf,"\n");
			do
				fprintf(outf,"%s",txt);
			while(fgets(txt,1024,inf));
		} 
		else printf("dvm_exit not found!\n");
		
		fclose(inf);
	}

	fclose(outf);


	return 0;
}




int main(int argc, char *argv[])
{
	bool	top = false;
	char	protName[256];
	int		i,j;
	char outfile[100]; //====//
	FILE *is_multy; //====//
//	char htmlfile[100]; //====//

	//grig
		vector<long>	lb;
		vector<long>	ASizeArray;
		mach_Type		AMType;
		int				AnumChanels = 1;
		double			Ascale = 1.0;
		double			ATStart;
		double			ATByte;
		double			AProcPower;

	Interval* BesInterval;
	MappedProcs BestConfigurationsProcs;
	std::vector<long> BestConfigurationSize;

	BestConfigurationsProcs.Processors.resize(0);
	BestConfigurationSize.resize(0);
	
    MinSizesOfAM.resize(4); // для автоматического поиска
	for(i=0;i<MinSizesOfAM.size();i++)
		MinSizesOfAM[i]=0.0;

//====
//printf("Predictor grig + max\n");
//=***

	CommCost cc_empty;	

	//\grig
	try {

		// Checking input parameters
		if (argc < 4) {
			message();
		}	else if (argc < 4) {
			// get topology from config file
			top = false;
		} else {
			// get topology from command line
			top = true;
			for (int i = 4; i < argc; i++) {
				if (isdigit(*argv[i])) {
					int ext = atoi(argv[i]);
					SizeArray.push_back(ext);
				} else
					message();
			}

			if(argc==4) SizeArray.push_back(1); //====// если процессорная решетка не задана то считаем ее равной одному процу
		}
		char * last = strrchr(argv[3], '.');
		if (last == NULL) {
			strcpy(protName, argv[3]);
		} else {
			char * p1,  * p2;
			for (p1 = argv[3], p2 = &protName[0]; p1 < last; p1++, p2++)
				*p2 = *p1;
			*p2 = 0;
		}
		strcat(protName, ".log");
		prot.open(protName);
		// Predictor logo
		cout << "*** DVM performance " << VER_PRED << " ***" << endl;

		
		prot << "*****************************************************" << endl;
		prot << "*** DVM performance " << VER_PRED << " ***" << endl;
		prot << "*****************************************************" << endl;
		prot << endl;

		prot << "Protocol file name: " << protName << endl; 
		prot << "Options file name: " << argv[1] << endl; 
		prot << "Trace file name: "  << argv[2] << endl; 
//====
//printf("Start '%s'\n",argv[2]);

		//!!! немного неправильно что он доходит до собачки или двух собачек и останавливается
		for(i=0,j=0;i<=strlen(argv[2]);i++)
			if(argv[2][i]==0 || argv[2][i]=='@' || argv[2][i]=='.') 
			{	
				if(argv[2][i]=='@') { outfile[i]='@'; j++;}
				else if(j==1)	{ //одна собачка
						outfile[i]=0; 
						break;
				}
				if(j==2) //две собачки
				{ outfile[i+1]=0; 
					break;
				}
				
				if(j==0 && argv[2][i]!='@')
				{
					outfile[i]=0;
					break;
				}
			}
			else 
			{	if(j==1)
				{	outfile[i]=0; 
					break;
				}
				outfile[i]=argv[2][i];
			}


	
		strcpy(outfile,strcat(outfile,".ptr"));

		is_multy=fopen(outfile,"r");


		if (j==1 && is_multy==NULL)
		{ //используется несколько трасс с помощью '*'
//			strcpy(outfile,strcat("multy_",outfile));
//			strcpy(htmlfile,strcat(outfile,".html"));
//printf("Out '%s'\n",outfile);
			multy_trace(argv[2],outfile);
			prot << "Resulting multy trace name: " << outfile << endl;
		}

		if (j==2 && is_multy==NULL)
		{ //используется несколько трасс с помощью экстраполяции
//printf("Out '%s'\n",outfile);
			extra_trace(argv[2]);
			prot << "Resulting Extra trace name: " << argv[2] << endl;
		}

		if(j==0 && is_multy==NULL)
		{	for(i=0;i<=strlen(argv[2]);i++)
				outfile[i]=argv[2][i];
			
			outfile[i]=0;
			
			multy_trace(argv[2],outfile);

		}

		//		exit(0);

//printf("Html '%s'\n",htmlfile);

//=***
		prot << "Resulting HTML file name: " << argv[3] << endl; //====//

		// ==================== Reading configuration file ====================

		ps = new PS(argv[1]);




		cc_empty.SaveChannels();
		
//		cc.CommSend(0.0, 5, 6, 100);
//		cc.CommSend(0.0, 5, 6, 100);
//		cc.CommSend(0.0, 0, 12, 200);

		// ==================== Create root VM ====================

//		for(i=0;i<ps->mappedProcs.Processors.size();i++)
//			printf("procpower[%d] = %f \n",i,ps->mappedProcs.Processors[i].ProcPower);




/*		vector<long>	lb;
		vector<long>	ASizeArray;
		mach_Type		AMType;
		int				AnumChanels = 1;
		double			Ascale = 1.0;
		double			ATStart;
		double			ATByte;
		double			AProcPower;*/
		
		//grig
		vector<double>  AvProcPower;
		//\grig

		if (top) {
			// correct 'VM' from command line parameter
			long sss=1;
			for(i=0;i<SizeArray.size();i++)
				sss*=SizeArray[i];

//		if(sss>ps->mappedProcs.Processors.size())
		if(sss>CurrentCluster->GetProcCount())
			{
				printf("error!!! not enough processors\n");
					exit(0);
			
			}

			ps->setTopology(SizeArray);
		}

#ifdef P_DEBUG
		prot << *ps << endl;
#endif
		ps->nextPS(lb, ASizeArray, AMType, AnumChanels, Ascale, ATStart, ATByte, AProcPower,AvProcPower);
	

//		printf("\n\n%d \n",ps->vProcPower.size());
		rootVM = new VM(ASizeArray, AMType, AnumChanels, Ascale, ATStart/1000000.0, ATByte/1000000.0, AvProcPower);
//		printf("size1=%d\n",ASizeArray.size());
		currentVM = rootVM;
		rootProcCount = rootVM->getProcCount();

	//	printf("Rank of root vm is %d : ",rootVM->Rank());
	//	for(i=0;i<rootVM->Rank();i++)
	//		printf("%d ",rootVM->GetSize(i+1));
	//	printf("\n");


#ifdef P_DEBUG
		prot << *rootVM << endl;
#endif

		// ==================== Reading/parsing trace file ====================
//		prot << "Reading/parsing trace file..." << endl;

//		prot << "  Convert 'trace file' -> ...'VectorTraceLine::traceLines'" << endl;

		
		traceLines = new VectorTraceLine(outfile); //====// edited

		root_Info* tmp = Get_Root_Info();
//		prot << "  Completed." << endl;

//		prot << "Reading/parsing trace file successful." << endl;

		// ==================== Modelling execution ====================

//		printf("Model begin\n");
#ifdef P_DEBUG
		prot << "Modelling application execution..." << endl;
#endif

		procElapsedTime = new double[rootProcCount];
		for (i = 0; i < rootProcCount; i++)
			procElapsedTime[i] = 0.0;
		
		CurrInterval = new Interval();

		ModelExec(traceLines);

//		printf("Model Done\n");
//		delete funcCalls;


		CurrInterval->CalcIdleAndImbalance();//====//

		CurrInterval->Integrate();        
#ifdef P_DEBUG
		prot << "Modelling application execution successful." << endl;
#endif

		// ==================== Writing results ====================

#ifdef P_DEBUG
		prot << "Creating output HTML file." << endl;
#endif
		// open output HTML file
		hfile.open(argv[3]);
		if (!hfile.is_open()) {
			cerr << "Can't open resulting HTML file '" << argv[3] << '\'' << endl;
			hfile.exceptions(ostream::badbit | ostream::failbit | ostream::eofbit);
		}

		//====
		CurrInterval->html_title=(char *)malloc(strlen(argv[3])*sizeof(char)); 
		strcpy(CurrInterval->html_title, argv[3]);
		//=***
		CreateHTMLfile();

#ifdef P_DEBUG
		prot << "Creating output HTML file done." << endl;
#endif

		// Successful 


		/*printf("Processing done.\n");

		for(i=0;i<MinSizesOfAM.size();i++)
			printf("%d ",MinSizesOfAM[i]);
		printf("\n");
		printf("all calls take %f seconds\n",grig_time_call);
*/

		//printf("Pred done OK\n");
//		if(!search_opt_mode) return 0;
	}

//#ifdef nodef

#ifdef _MSC_VER
	catch (fstream::failure e) {
		// any exeptions in input/output stream
		cout << "Exeptions in input/output stream: " << e.what() << endl;
		abort();
	}
#endif

	catch (bad_alloc e) {
		// exception in memory allocator
		prot << "Exeption in memory allocator: " << e.what() << endl;
		abort();
	}

	catch (exception e) {
		// any exceptions in standard library
		prot << "Exeptions in standard library: " << e.what() << endl;
		abort();
	}
//	catch (...) {
//		// any other exeptions
//		prot << "Can't recognise exeption" << endl;
//		abort();
//	}
//#endif

//printf("continue search optimal configuration? (1/0)\n");
//cin >> i;
	
	if(!search_opt_mode) return 0;

i=search_opt_mode;
prot << "\nStart searching optimal configuration of processor set\n";

if(i!=0)
{
	//prepare for search
	std::vector<long> indexes;
	ps->PrepareForAutoSearch(indexes);
	
//PrepareForAutoSearch(std::vector<long>& perstanovki)
int k,t,n,proc_count, variant_count=0, not_bad=0, count_iter=0, min_best_num, max_best_num;
int NUMPROC=100;
int rank, last_best_proc=0;
int num_conf;
bool was_best=false, is_best=false, is_min_best, is_max_best;
float first_time;
vector<long> first_SizeArray, min_best, max_best;

BesInterval=CurrInterval;
BestConfigurationSize=ASizeArray;
FirstTrace=false;
first_time=CurrInterval->Execution_time;
first_SizeArray=ASizeArray;

NUMPROC=ps->mappedProcs.Processors.size();
//NUMPROC=256;

while(1)
{
	struct searched searched_tmp;
	searched_vect searched_vtmp; //класс уже рассмотренных вариантов
	vector<searched_vect> searched_class; //классы уже рассмотренных вариантов
	searched_class.resize(0);

	struct conf tmp;
	long_vect vec;
	vector<long_vect>  Configurations;
	vector<conf> config;
	config.resize(0);

	for(i=0, rank=0; i<4; i++)
		if(MinSizesOfAM[i]!=0) rank=i+1;

	// Make all into config
	for(proc_count=1; proc_count<=NUMPROC; proc_count++) 
	{
		MakeAllConfigurations(proc_count,rank,Configurations);

		for(num_conf=0;num_conf< Configurations.size();num_conf++)
		{
			j=CheckEuristik(Configurations[num_conf]); 
			variant_count++;

			if(j>0 || search_opt_mode==3)
			{	
				long proc_num=1;

				if(j>0) not_bad++;
/*				printf("Try %d ",config.size());
				for(i=0; i<Configurations[num_conf].size(); i++ ) printf(" %d",Configurations[num_conf][i]);
				printf(" - %d\n",j);
*/				
				tmp.proc_set=Configurations[num_conf];
				for(i=0; i<tmp.proc_set.size(); i++)
					proc_num*=tmp.proc_set[i];
				tmp.proc_num=proc_num;
				tmp.mark=j;
				config.push_back(tmp);
			}
		}
	}

	if(search_opt_mode==1 || search_opt_mode==5)
	{
		// sort by marks
		for(i=0; i<config.size(); i++)
		{
			for(j=i+1, k=i; j<config.size(); j++)
			{ if(config[j].mark > config[k].mark || config[j].mark==config[k].mark && config[j].proc_num<config[k].proc_num) k=j;
			}

			if(config[i].mark < config[k].mark || config[i].mark==config[k].mark && config[i].proc_num>config[k].proc_num ) 
			{
				tmp.proc_set=config[i].proc_set;
				tmp.proc_num=config[i].proc_num;
				tmp.mark=config[i].mark;

				config[i].proc_set=config[k].proc_set;
				config[i].proc_num=config[k].proc_num;
				config[i].mark=config[k].mark;

				config[k].proc_set=tmp.proc_set;
				config[k].proc_num=tmp.proc_num;
				config[k].mark=tmp.mark;
			}
		}

		if(0)
		for(i=0; i<config.size(); i++)
		{
			printf("Sort %d ",i);
			for(j=0; j<config[i].proc_set.size(); j++ ) printf(" %d",config[i].proc_set[j]);
			printf(" - %d\n",config[i].mark);
		}
	}

	while(config.size()>0)
	{
		if(search_opt_mode==1 || search_opt_mode==5)
		{
			if(0)
			for(i=0; i<config.size(); i++)
			{
				printf("Sort %d ",i);
				for(j=0; j<config[i].proc_set.size(); j++ ) printf(" %d",config[i].proc_set[j]);
				printf(" - %d\n",config[i].mark);
			}

			//построить нижнюю и верхнюю грань BestConfigurationSize
			min_best.resize(config[0].proc_set.size());
			max_best.resize(config[0].proc_set.size());
			for(i=0; i<min_best.size(); i++)
			{	min_best[i]=1;
				max_best[i]=MinSizesOfAM[i];
			}

			for(i=0; i<config.size() && config[i].mark==config[0].mark; i++)
			{	
				for(j=0; j<config[i].proc_set.size(); j++)
				{
					if(j>=BestConfigurationSize.size()) break;
					if(config[i].proc_set[j]<BestConfigurationSize[j] && config[i].proc_set[j]>min_best[j]) min_best[j]=config[i].proc_set[j];
					if(config[i].proc_set[j]>BestConfigurationSize[j] && config[i].proc_set[j]<max_best[j]) max_best[j]=config[i].proc_set[j];
				}
			}

			for(i=0, min_best_num=1, max_best_num=1; i<min_best.size(); i++)
			{	min_best_num*=min_best[i];
				max_best_num*=max_best[i];
			}


			if(0)
			{	for(i=0; i<min_best.size(); i++)
					printf(" (%d - %d)", min_best[i], max_best[i]);
			}


			// постоить массив различных процессоров с наилучшими оценками
			vec.resize(0); is_min_best=false; is_max_best=false;
			for(i=0; i<config.size() && config[i].mark==config[0].mark; i++)
			{
				if(!is_min_best && min_best_num==config[i].proc_num) is_min_best=true;
				if(!is_max_best && max_best_num==config[i].proc_num) is_max_best=true;

				for(j=0; j<vec.size(); j++)
					if(config[i].proc_num==vec[j]) break;
				
				if(j==vec.size())
					vec.push_back(config[i].proc_num);
			}

			// вычислять следуюший цикл необязательно - только для выдачи
			for(i=1,j=0,k=0; i<config.size(); i++)
			{ if(config[i].proc_num < config[j].proc_num) j=i;
				if(config[i].proc_num > config[k].proc_num) k=i;
			}
			printf("Search %d (%d - %d) ",config.size(),config[j].proc_num,config[k].proc_num);




//			printf("Search %d (%d - %d) ",vec.size(),vec[0],vec[vec.size()-1]);
//			for(j=0; j<vec.size(); j++ ) printf(" %d",vec[j]);
//printf("was_best=%d   is_last_best=%d   so ",was_best,is_best);

			k=vec[(vec.size()-1)/2]; //пока нет улучшений ищем делением пополам
			if(is_best==1 && is_min_best)  k=min_best_num; // первое улучшение ищем внизу от лучшего
			if(was_best==1 && is_best==0 && is_max_best)  k=max_best_num; // если провалилось внизу, то ищем сверху

//			printf(" best_min_max (%d - %d) ",is_min_best?min_best_num:0, is_max_best?max_best_num:0);
			printf("   mark=%.2f%%\n",((float)config[0].mark)/100);

		}

		for(num_conf=0;  true ; num_conf++)
		{
			if(search_opt_mode==1 || search_opt_mode==5)
				for(	; num_conf<config.size(); num_conf++) 
					if(config[num_conf].proc_num==k && config[num_conf].mark==config[0].mark) break;

			if(num_conf >= config.size()) break;



			currentAM_ID=0; //reinitialize AM_ID

			printf("iter=%d best: ",count_iter+1);
			for(i=0;i<BestConfigurationSize.size();i++)
				printf("%d ",BestConfigurationSize[i]);		
			printf("- %f; ",BesInterval->GetEffectiveParameter());

			printf("check proc_set(%d): ",config[num_conf].proc_num);
			for(i=0;i<config[num_conf].proc_set.size();i++)
				printf("%d ",config[num_conf].proc_set[i]);		

			if(rootVM!=NULL) { delete rootVM; rootVM=NULL;}
			//====
						
			if(Ascale<10.0)
			{ gzclose(traceLines->trace_file);
				traceLines->restore();
				if(traceLines!=NULL) { delete traceLines; traceLines=NULL;}
				resetInfos();
			}
		
			//=***
			//delete CurrInterval; 

					ps->reset();

					//grig
					vector<double>  AvProcPower;
					//\grig
						
					AvProcPower.resize(0);

					//set configuration of PS
					ps->setTopology(config[num_conf].proc_set);
					
					ps->nextPS(lb, ASizeArray, AMType, AnumChanels, Ascale, ATStart, ATByte, AProcPower,AvProcPower);
					//ps->CorrectMappedProcs();

					rootVM = new VM(ASizeArray, AMType, AnumChanels, Ascale, ATStart/1000000.0, ATByte/1000000.0, AProcPower,AvProcPower);
					currentVM = rootVM;
					rootProcCount = rootVM->getProcCount();


						// ==================== Reading/parsing trace file ====================

if(Ascale<10.0)					traceLines = new VectorTraceLine(outfile);

					root_Info* tmp = Get_Root_Info();
					// ==================== Modelling execution ====================
			//printf("Model done\n");

					procElapsedTime = new double[rootProcCount];
					for (i = 0; i < rootProcCount; i++)
						procElapsedTime[i] = 0.0;
					
					CurrInterval = new Interval();

					cc_empty.RestoreChannels();
					cc_empty.SaveChannels();


			//printf("Model execing...\n");
if(Ascale<10.0)					ModelExec(traceLines);
			//printf("Model exec done\n");
					count_iter++; //count excellent variants

					CurrInterval->CalcIdleAndImbalance(); //====//

					CurrInterval->Integrate();        
					delete tmp;
					delete []procElapsedTime;

					prot<<"proc_set: ";
					for(i=0;i<config[num_conf].proc_set.size();i++)
						prot<<config[num_conf].proc_set[i]<<" ";			 


					printf("- %f\n",CurrInterval->GetEffectiveParameter());
					prot<<" - "<<CurrInterval->GetEffectiveParameter()<<"\n"; //==//

					//добавить информацию о рассмотренном варианте в классификацию всех вариантов, чтобы потом отсечь ненужные варианты
					searched_tmp.proc_num=config[num_conf].proc_num;
					searched_tmp.proc_set=config[num_conf].proc_set;
					searched_tmp.time=CurrInterval->GetEffectiveParameter();
					for(i=0;i<rank;i++)
					{	
						searched_tmp.proc_id_diff=i;
						for(j=0;j<searched_class.size();j++)
						{	
							if(searched_class[j].size()>0 && searched_class[j][0].proc_id_diff==i)
							{	for(n=0; n<rank; n++)
									if(n!=i && searched_class[j][0].proc_set[n]!=searched_tmp.proc_set[n]) break;
								
								if(n==rank) //нашли подходящий класс для i-того измерения
								{	searched_class[j].push_back(searched_tmp);
									break;
								}
							}
						}

						if(j==searched_class.size()) //не нашли подходящий класс для i-того измерения
						{	searched_vtmp.resize(0);
							searched_vtmp.push_back(searched_tmp);
							searched_class.push_back(searched_vtmp);
						}
					}

					
					if(IsBestConfiguration(BesInterval,CurrInterval))
					{
					//	delete BesInterval;
//						printf("proc_c=%d tmp=%d min=%d\n",proc_count,tmp_tmp,tmp_min);
//						if(tmp_tmp==proc_count) min_proc=tmp_min;
//						else min_proc=tmp_tmp;
//						tmp_min=tmp_tmp;
//						tmp_tmp=proc_count;
//						max_proc_mem=-proc_count;


						BesInterval=CurrInterval;
						BestConfigurationsProcs.Processors.resize(0);
						for(i=0;i<currentVM->getProcCount();i++)
						{
							BestConfigurationsProcs.AddProc(ps->mappedProcs.Processors[i]);
						}

						BestConfigurationSize=config[num_conf].proc_set;
					}
					else
					{
/*						for(i=0,j=1;i<BestConfigurationSize.size();i++)
							j*=BestConfigurationSize[i];

//						printf("proc_count=%d   best_count=%d   tmp_tmp=%d   tmp_min=%d   max_proc_mem=%d \n",proc_count,j,tmp_tmp,tmp_min,max_proc_mem);
						if(proc_count<j && proc_count>min_proc) min_proc=proc_count;
						if(proc_count>j && proc_count<max_proc_mem) max_proc_mem=proc_count;

						//запоминаем последние два числа процессоров, для которых перебирали вариант
						if(proc_count>tmp_tmp && tmp_tmp>tmp_min) { tmp_min=tmp_tmp; tmp_tmp=proc_count;}
						else if(proc_count>tmp_tmp) {tmp_tmp=proc_count;}

						if(max_proc_mem<0 && proc_count>-max_proc_mem) max_proc_mem=proc_count;
*/
						delete CurrInterval;		
					}
		}


		if(search_opt_mode==1 || search_opt_mode==5)
		{
					if(1) // отсечение по классам
					{	for(i=0; i<searched_class.size(); i++)
						{
							if(searched_class[i].size()>1)
							{
								int xmin=0, xmax=0, xtmin=0, id=searched_class[i][0].proc_id_diff;

								for(j=0; j<searched_class[i].size(); j++)
								{ 
									if(0) //печать
									{	printf("Class %d [%d] - ",i,j);

										for(n=0; n<searched_class[i][j].proc_set.size(); n++)
											printf("%d ",searched_class[i][j].proc_set[n]);
										printf("(%d) - %f sec\n",searched_class[i][j].proc_num, searched_class[i][j].time);
									}

									if(searched_class[i][j].proc_set[id] < searched_class[i][xmin].proc_set[id]) xmin=j;
									if(searched_class[i][j].proc_set[id] > searched_class[i][xmax].proc_set[id]) xmax=j;
									if(searched_class[i][j].time < searched_class[i][xtmin].time) xtmin=j;
								}

/*								//ищем xmin & xmax - верхние и нижние грани по времени и индексу
								for(j=0; j<searched_class[i].size(); j++)
								{	if(searched_class[i][j].time < searched_class[i][xmin].time && searched_class[i][j].proc_set[id] < searched_class[i][xtmin].proc_set[id]) 
										xmin=j;
									if(searched_class[i][j].time < searched_class[i][xmax].time && searched_class[i][j].proc_set[id] > searched_class[i][xtmin].proc_set[id]) 
										xmax=j;
								}
*/
								//ищем xmin & xmax - верхние и нижние грани по индексу
								for(j=0; j<searched_class[i].size(); j++)
								{	

									//не удалять несколько соседей по времени проигрывающих лидеру не более 0.25 % 
									if(searched_class[i][j].time/(BesInterval->GetEffectiveParameter()+0.0000001)<1.0025) continue;
									
									if(searched_class[i][j].time < searched_class[i][xmin].time && searched_class[i][j].proc_set[id] < searched_class[i][xtmin].proc_set[id]) 
										xmin=j;
									if(searched_class[i][j].time < searched_class[i][xmax].time && searched_class[i][j].proc_set[id] > searched_class[i][xtmin].proc_set[id]) 
										xmax=j;
								}


								if(0) //печать
								{	printf("Xmin=%d xmax=%d xtmin=%d\n",xmin,xmax,xtmin);
									if(searched_class[i][xmin].proc_set[id] < searched_class[i][xtmin].proc_set[id] || searched_class[i][xmax].proc_set[id] > searched_class[i][xtmin].proc_set[id]) 
									{
										printf("class [ ");
										for(j=0; j<searched_class[i][0].proc_set.size(); j++)
											if(j!=id) printf("%d ",searched_class[i][0].proc_set[j]);
											else printf("* ");
									}

									printf("] - mid = %d  ",searched_class[i][xtmin].proc_set[id]);
									if(searched_class[i][xmin].proc_set[id] < searched_class[i][xtmin].proc_set[id]) printf(" - erase < %d",searched_class[i][xmin].proc_set[id]);
									if(searched_class[i][xmax].proc_set[id] > searched_class[i][xtmin].proc_set[id]) printf(" - erase > %d",searched_class[i][xmax].proc_set[id]);
									printf("\n");
								}
								
								if(searched_class[i][xmin].proc_set[id] < searched_class[i][xtmin].proc_set[id] || searched_class[i][xmax].proc_set[id] > searched_class[i][xtmin].proc_set[id])
								{	
									// удалить конфигурации с ненужным количеством процессоров 
									for(j=0; j<config.size(); j++)
									{
										for(n=0; n<config[j].proc_set.size(); n++)
											if(n!=id && config[j].proc_set[n]!=searched_class[i][0].proc_set[n]) break;

										if(n==config[j].proc_set.size() && searched_class[i][xmin].proc_set[id] < searched_class[i][xtmin].proc_set[id] && config[j].proc_set[id] < searched_class[i][xmin].proc_set[id] ||
										   n==config[j].proc_set.size() && searched_class[i][xmax].proc_set[id] > searched_class[i][xtmin].proc_set[id] && config[j].proc_set[id] > searched_class[i][xmax].proc_set[id])
										{	
//											printf("erase = ");	for(int x=0; x<config[j].proc_set.size(); x++)	printf("%d ",config[j].proc_set[x]);	printf("\n");
//											config.erase(&config[j]);
											config.erase(config.begin()+j);
											j--;
										}
									}
								}
									
								//printf("\n");

							}
						}
					}


			for(i=0, j=1; i<BestConfigurationSize.size(); i++)
				j*=BestConfigurationSize[i];

			was_best=is_best;
			is_best=(j==k);

			if(j<k || is_best && was_best && j<last_best_proc)
			{ // ищем минимальную цифру
				for(i=1,n=0; i<BestConfigurationSize.size(); i++)
					if(BestConfigurationSize[i]<BestConfigurationSize[n]) n=i;

				// находим максимального соседа по одной цифре
				for(i=0, t=BestConfigurationSize[n]+1; i<BestConfigurationSize.size(); i++)
					if(i!=n) t*=BestConfigurationSize[i];

				if(k>t) t=k;
				if(last_best_proc>t) t=last_best_proc;
//				printf("Erase > %d\n",t);
			}
			if(j>k || is_best && was_best && j>last_best_proc)
			{  // ищем минимальную цифру
				for(i=1,n=0; i<BestConfigurationSize.size(); i++)
					if(BestConfigurationSize[i]<BestConfigurationSize[n]) n=i;

				// находим минимального соседа по одной цифре
				for(i=0, t=BestConfigurationSize[n]-1; i<BestConfigurationSize.size(); i++)
					if(i!=n) t*=BestConfigurationSize[i];

				if(k<t) t=k;

				if(last_best_proc<t) t=last_best_proc;
//				printf("Erase < %d\n",t);
			}

//			printf("Best=%d ,cur=%d   t=%d   history(%d %d) last_best=%d\n", j,k,t,was_best, is_best, last_best_proc);

			// удалить конфигурации с ненужным количеством процессоров 
			for(i=0; i<config.size(); i++)
			{
				if(j<k && config[i].proc_num>t || 
					 j>k && config[i].proc_num<t || 
					 config[i].proc_num==k && config[i].mark==config[0].mark ||
					 is_best && was_best && j>last_best_proc && config[i].proc_num<t ||
					 is_best && was_best && j<last_best_proc && config[i].proc_num>t)
				{	
//					config.erase(&config[i]);
					config.erase(config.begin()+i);
					i--;
				}
			}
			last_best_proc=j;
		}
		else // search_opt_mode!=1 && search_opt_mode!=5
		{
			config.resize(0);
		}
 
	}
					if(0) // печать перебранных вариантов в виде классификации
					{	printf("Searched Classification\n");
						for(i=0; i<searched_class.size(); i++)
						{
							if(1)//sort
							for(j=0; j<searched_class[i].size(); j++)
							{
								for(n=j+1; n<searched_class[i].size(); n++)
									if(searched_class[i][j].proc_set[searched_class[i][0].proc_id_diff] > searched_class[i][n].proc_set[searched_class[i][0].proc_id_diff])
									{ searched_tmp.proc_num=searched_class[i][j].proc_num;
										searched_tmp.proc_set=searched_class[i][j].proc_set;
										searched_tmp.time=searched_class[i][j].time;
										searched_class[i][j].proc_num=searched_class[i][n].proc_num;
										searched_class[i][j].proc_set=searched_class[i][n].proc_set;
										searched_class[i][j].time=searched_class[i][n].time;
										searched_class[i][n].proc_num=searched_tmp.proc_num;
										searched_class[i][n].proc_set=searched_tmp.proc_set;
										searched_class[i][n].time=searched_tmp.time;
										n=j;
									}
							}

							if(searched_class[i].size()>0)
							{
								int xmin=0, xmax=0, xtmin=0, id=searched_class[i][0].proc_id_diff;

								for(j=0; j<searched_class[i].size(); j++)
								{ if(searched_class[i][j].proc_set[id] < searched_class[i][xmin].proc_set[id]) xmin=j;
									if(searched_class[i][j].proc_set[id] > searched_class[i][xmax].proc_set[id]) xmax=j;
									if(searched_class[i][j].time < searched_class[i][xtmin].time) xtmin=j;
								}

/*								//ищем xmin & xmax - верхние и нижние грани по времени и индексу
								for(j=0; j<searched_class[i].size(); j++)
								{	if(searched_class[i][j].time < searched_class[i][xmin].time && searched_class[i][j].proc_set[id] < searched_class[i][xtmin].proc_set[id]) 
										xmin=j;
									if(searched_class[i][j].time < searched_class[i][xmax].time && searched_class[i][j].proc_set[id] > searched_class[i][xtmin].proc_set[id]) 
										xmax=j;
								}
*/
								//ищем xmin & xmax - верхние и нижние грани по индексу
								for(j=0; j<searched_class[i].size(); j++)
								{	
									printf("Class %d [%d] - ",i,j);

									for(n=0; n<searched_class[i][j].proc_set.size(); n++)
										printf("%d ",searched_class[i][j].proc_set[n]);
									printf("(%d) - %f sec",searched_class[i][j].proc_num, searched_class[i][j].time);

									if(j==xtmin) printf(" *");
									else if(searched_class[i][j].time/(BesInterval->GetEffectiveParameter()+0.0000001)<1.0025) printf(" +");
									printf("\n");


									if(searched_class[i][j].time < searched_class[i][xmin].time && searched_class[i][j].proc_set[id] < searched_class[i][xtmin].proc_set[id]) 
										xmin=j;
									if(searched_class[i][j].time < searched_class[i][xmax].time && searched_class[i][j].proc_set[id] > searched_class[i][xtmin].proc_set[id]) 
										xmax=j;
								}


								if(0) //печать
								{
									printf("Xmin=%d xmax=%d xtmin=%d\n",xmin,xmax,xtmin);
									if(searched_class[i][xmin].proc_set[id] < searched_class[i][xtmin].proc_set[id] || searched_class[i][xmax].proc_set[id] > searched_class[i][xtmin].proc_set[id]) 
									{
										printf("class [ ");
										for(j=0; j<searched_class[i][0].proc_set.size(); j++)
											if(j!=id) printf("%d ",searched_class[i][0].proc_set[j]);
											else printf("* ");
									}

									printf("] - mid = %d  ",searched_class[i][xtmin].proc_set[id]);
								
									if(searched_class[i][xmin].proc_set[id] < searched_class[i][xtmin].proc_set[id]) printf(" - erase < %d",searched_class[i][xmin].proc_set[id]);
									if(searched_class[i][xmax].proc_set[id] > searched_class[i][xtmin].proc_set[id]) printf(" - erase > %d",searched_class[i][xmax].proc_set[id]);
									printf("\n");
									
								}
								printf("\n");

							}
						}
					}

	prot<<"\n\n\n";
	prot<<"Best set of processors (for cluster with "<<NUMPROC<<" processors) = ";
	for(i=0;i<BestConfigurationSize.size();i++)
		prot<<BestConfigurationSize[i]<<" ";
	prot<<"\n";
	prot<<"Checked variants : "<<count_iter<<" of "<<variant_count<<" (not-bad "<<not_bad<<")" << endl;
	prot<<"\n\n";
	

	if(search_opt_mode==5) 	
	{ search_opt_mode=2; variant_count=0; not_bad=0; count_iter=0;	
//		for(i=0; i<BestConfigurationSize.size(); i++)
//			BestConfigurationSize[i]=1;
		BestConfigurationSize=first_SizeArray;
		BesInterval->Execution_time=first_time;

	}
	else
		break;
}


		// open output HTML file
		hfile.open("best.html");
		if (!hfile.is_open()) {
			cerr << "Can't open resulting HTML file '" << "best.html" << '\'' << endl;
			hfile.exceptions(ostream::badbit | ostream::failbit | ostream::eofbit);
		}


     // for show best interval
		if(rootVM!=NULL) { delete rootVM; rootVM=NULL;}
		//gzclose(traceLines->trace_file);
		//====//	traceLines->restore();
		if(traceLines!=NULL) { delete traceLines; traceLines=NULL;}
		resetInfos();
		//delete CurrInterval;
		ps->reset();
		//grig
		vector<double>  AvProcPower;
		//\grig
		AvProcPower.resize(0);
		//set configuration of PS
		ps->setTopology(BestConfigurationSize);
		ps->nextPS(lb, ASizeArray, AMType, AnumChanels, Ascale, ATStart, ATByte, AProcPower,AvProcPower);
		//ps->CorrectMappedProcs();
 	    rootVM = new VM(ASizeArray, AMType, AnumChanels, Ascale, ATStart/1000000.0, ATByte/1000000.0, AProcPower,AvProcPower);
		currentVM = rootVM;
		rootProcCount = rootVM->getProcCount();

		CurrInterval=BesInterval;
		CreateHTMLfile();

	}
	return 0;
}

