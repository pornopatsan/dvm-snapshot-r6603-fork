#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#ifndef _MPI_STUBS_
#include <mpi.h>
#else
#include "../../include/mpi_stubs.h"
#endif
#include <signal.h>
#ifdef _UNIX_
#include <unistd.h>
#endif

#define MAX(x,y) (((x) > (y))? (x): (y))
#define MIN(x,y) (((x) < (y))? (x): (y))

#define LOC(n,x,p) \
	if ((x) > (max##n)) pmax##n=p;\
	max##n=(((max##n) > (x))? (max##n): (x));\
	if ((x) < (min##n)) pmin##n=p;\
	min##n=(((min##n) < (x))? (min##n): (x));\
	mid##n += (x)
	
static	long MAXLOOP=1000;
static	long newloop;
static	double tloop;
static	int pweight=0;
static	double tlast=0.0;
static	double tlost=0.0;
static	int tnull=0;
static	int ip;
static	double temp;

static char ProcName[128];
static long minSendBytes=0;
static long maxSendBytes=0;
static int pminSendBytes=0;
static int pmaxSendBytes=0;
static long midSendBytes=0;

static double ar_time[4]={0.0,0.0,0.0,0.0};
static double *ar_times;

static double mintaskTime=0.0;
static double maxtaskTime=0.0;
static int pmintaskTime=0;
static int pmaxtaskTime=0;
static double midtaskTime=0.0;

static double mintaskMPITime=0.0;
static double maxtaskMPITime=0.0;
static int pmintaskMPITime=0;
static int pmaxtaskMPITime=0;
static double midtaskMPITime=0.0;

static double minintervalTime=0.0;
static double maxintervalTime=0.0;
static int pminintervalTime=0;
static int pmaxintervalTime=0;
static double midintervalTime=0.0;

static double minintervalMPITime=0.0;
static double maxintervalMPITime=0.0;
static int pminintervalMPITime=0;
static int pmaxintervalMPITime=0;
static double midintervalMPITime=0.0;

static long *othersBytes;

static long totalSendBytes=0;
static long otherSendBytes=0;
static double taskTime=0.0;
static double taskMPITime=0.0;
static double intervalTime=0.0;
static double intervalMPITime=0.0;
static double startIntervalTime=0.0;

static int cur_func;


struct stat_op {
	char name[80];
	int count;
	double totalTime;
	double maxTime;
	double minTime;
	long totalSendBytes;
	long maxSendBytes;
	long minSendBytes;
	long totalRecvBytes;
	long maxRecvBytes;
	long minRecvBytes;
	int other;
};
int TEST=0;
int WTIME=0;
#define num_Send 1
#define num_Isend 2
#define num_Recv 3
#define num_Irecv 4
#define num_Wait 5
#define num_Waitall 6
#define num_Barrier 7
#define num_Comm_dup 8
#define num_Comm_split 9
#define num_Bcast 10
#define num_Alltoall 11
#define num_Reduce 12
#define num_Allreduce 13
#define num_Alltoallv 14
#define num_Test 15
#define num_Wtime 16

#define NUM_FUNC 17
#define fout stderr
static FILE *ftrc;
struct stat_op * prof_info[NUM_FUNC];
static int myid;
static int level_func=0;
#define my_struct prof_info[cur_func]
#define DELAY\
	if ((tlast!=0.0)&&(pweight!=0)) {\
		tstart=PMPI_Wtime();\
		if(tstart-tlast) {\
			newloop=(long)(MAXLOOP*(tstart-tlast)/tloop);\
			for (ip=0; ip < newloop; ip++) temp = temp+(double)1/(ip+1);\
        		tlost+=PMPI_Wtime()-tstart;\
		} else tnull++;\
	}

#define PROF_FINISH	tlast=PMPI_Wtime()

#define PROF_START(x) \
	level_func++;\
 	if (cur_func) my_struct->other++;\
	else {\
	DELAY;\
	tstart=PMPI_Wtime();\
	if (plevel==1) {\
	if (prof_info[num_##x]==NULL) {\
	   prof_info[num_##x]=(struct stat_op *)calloc(1,sizeof(struct stat_op));\
	   strcat(prof_info[num_##x]->name,"MPI_"#x);\
	   prof_info[num_##x]->count=0;\
	   prof_info[num_##x]->totalTime=0.0;\
	   prof_info[num_##x]->maxTime=0.0;\
	   prof_info[num_##x]->totalSendBytes=-1;\
	   prof_info[num_##x]->maxSendBytes=0;\
	   prof_info[num_##x]->totalRecvBytes=-1;\
	   prof_info[num_##x]->maxRecvBytes=0;\
PMPI_Comm_rank(MPI_COMM_WORLD,&myid);\
/*if((myid==0) && (fout==NULL)) fout=fopen("mpi_stat.out","w");*/\
	}\
	prof_info[num_##x]->count++;\
	cur_func=num_##x;\
	}\
	}

#define PROF_TIME \
        level_func--;\
        if (!level_func) {\
	toper=PMPI_Wtime() - tstart;\
	taskMPITime+=toper;\
	if (plevel==1) {\
	  intervalMPITime+=toper;\
	  my_struct->totalTime += toper;\
	  my_struct->maxTime=MAX(my_struct->maxTime,toper);\
	  cur_func=0;\
	 }\
	}
/*
#define TRC_SEND(x) if (plevel==1) {\
	if(ftrc) fprintf(ftrc,"%s to %d %d bytes for %lf time\n",#x,dest,count*extent,toper);\
	}
*/
#define PROF_SBYTES if (plevel==1) {\
	if (level_func==1) {\
	PMPI_Type_size(datatype, &extent);\
	if (my_struct->totalSendBytes == -1) my_struct->totalSendBytes=0;\
	my_struct->totalSendBytes += count * extent;\
	totalSendBytes += count * extent;\
	my_struct->maxSendBytes = MAX(my_struct->maxSendBytes,count * extent);\
	}\
	}

#define PROF_RBYTES if (plevel==1) {\
	if (level_func==1) {\
	if (my_struct->totalRecvBytes == -1) my_struct->totalRecvBytes=0;\
	PMPI_Get_count(status,MPI_CHAR,&extent);\
	my_struct->totalRecvBytes += extent;\
	my_struct->maxRecvBytes = MAX(my_struct->maxRecvBytes,extent);\
	}\
	}

struct tail_req {
	struct tail_req *next;
	MPI_Request *req;
	int func;
};

struct tail_req *head_req=NULL;

static struct tail_req *add_req(MPI_Request *rrr) {
	struct tail_req **p;
	p=&head_req;
	for(;*p;p=&((*p)->next));
	*p=(struct tail_req *)malloc(sizeof(struct tail_req));
	(*p)->req=rrr;
	(*p)->next=NULL;
	(*p)->func=cur_func;
	return *p;
}
static struct tail_req *del_req(MPI_Request *rrr) {
	struct tail_req **p;
	struct tail_req *pst;
	p=&head_req;
	for(;*p;p=&((*p)->next)) if ((*p)->req==rrr) {pst=*p;*p=(*p)->next;free(pst);break;}
	return NULL;
}

static struct tail_req *find_req(MPI_Request *rrr) {
	struct tail_req **p;
	p=&head_req;
	for(;*p;p=&((*p)->next)) if ((*p)->req==rrr) {;return *p;}
	return NULL;
}
	

static	double tstart;
static	double toper;
static	int extent;
static	int res;
static	int plevel=1;
char fname[128];

int MPI_Init(int *argc,char ***argv) {
int result_len,nsize,i;
	res=PMPI_Init(argc,argv);
	PMPI_Get_processor_name(ProcName, &result_len);
	ProcName[result_len]=0;
	taskTime=PMPI_Wtime();
	if (startIntervalTime==0.0) startIntervalTime=PMPI_Wtime();
	PMPI_Comm_rank(MPI_COMM_WORLD,&myid);
	PMPI_Comm_size(MPI_COMM_WORLD,&nsize);
	sprintf(fname,"mpi_trc.%d",myid);
	ftrc=fopen(fname,"r");
	if (!ftrc) ftrc=fopen("mpi_trc.all","r");
	if (ftrc) {fclose(ftrc);ftrc=fopen(fname,"w");}

/*pause*/
{
double tt;
while(1) {
    tt=PMPI_Wtime();
    for (ip=0; ip < MAXLOOP; ip++)
         {temp = temp+(double)1/(ip+1); }
    tloop=PMPI_Wtime()-tt;
    
    if(tloop >= 0.1) break;
    else MAXLOOP*=2;
}

}
#ifdef _UNIX_
{
char str[256],str1[256];
int nproc,pid,count_proc,r,i,step, begn, endn;
FILE *fh;
char *p;
double temp;
	fh=fopen("weight_ranks","r");
	if (fh!=NULL) {
		i=nsize;
		while (feof(fh)==0) {
			p=str;
			if (fgets(p,256,fh)==NULL) continue;
			if((p[strlen(p)-1]=='\n') || (p[strlen(p)-1]=='\r')) p[strlen(p)-1]=0;
			if((p[strlen(p)-1]=='\n') || (p[strlen(p)-1]=='\r')) p[strlen(p)-1]=0;
			if (p[0]==0) continue;
			nproc=0;
			r=sscanf(str,"%s %d",str1,&nproc);
			if (strcmp(str1,"#numproc")==0) {if (r>=2)count_proc=nproc;continue;}
			r=sscanf(str,"%d %d %d %d",&begn,&step,&endn,&nproc);
/*printf("r=%d, b=%d,s=%d,e=%d\n",r,begn,step,endn);*/
			if (r<3) continue;
			for(i=begn;i<=endn;i=i+step) if (i==myid) break;
			if (i==endn+1) {i=nsize;continue;}
			if (i==myid) {if (r==4) count_proc=nproc;break;}
		}
		if (i==myid) {
/*
printf("%d: fork %d processes\n",myid,count_proc); 
fflush(NULL);
		 for(i=0;i<count_proc;i++) {
		   if ((pid=vfork())==0) {
			if(execl("fon",(*argv)[0],(*argv)[1])==-1) printf("Error EXECV fon process\n");
			fflush(NULL);
		   }		
		   if(pid==-1) {printf("process %d not forked\n",i); exit(1);}
		 }
*/
printf("%d: delay %d%%\n",myid,count_proc); 
fflush(NULL);
		pweight=count_proc;
		}
		fclose(fh);
	} else {	
		fh=fopen("weight_hosts","r");
	   if (fh!=NULL) {
		while (feof(fh)==0) {
			p=str;
			if (fgets(p,256,fh)==NULL) continue;
			if((p[strlen(p)-1]=='\n') || (p[strlen(p)-1]=='\r')) p[strlen(p)-1]=0;
			if((p[strlen(p)-1]=='\n') || (p[strlen(p)-1]=='\r')) p[strlen(p)-1]=0;
			if (p[0]==0) continue;
			nproc=0;
			r=sscanf(str,"%s %d",str1,&nproc);
			if (strcmp(str1,"#numproc")==0) {if (r>=2)count_proc=nproc;continue;}

			if (strcmp(str1,ProcName)==0) {if(r>=2)count_proc=nproc;break;}
		}
		if (strcmp(str1,ProcName)==0) {
printf("%d: fork %d processes\n",myid,count_proc); 
fflush(NULL);
		 for(i=0;i<count_proc;i++) {
		   if ((pid=fork())==0) {
			while(1)
				for (i=0; i < 1000000; i++)
					{temp = temp+(double)1/(i+1); }
			exit(1);      
		   }		
		   if(pid==-1) {printf("process %d not forked\n",i); exit(1);}
		 }
		}
		fclose(fh);
	   }
	}	
}
#endif
	MAXLOOP=MAXLOOP*pweight/100;
/*printf("%d:: MAXLOOP=%d\n",myid,MAXLOOP);*/
	PROF_FINISH;
	return res;
}


int MPI_Pcontrol(int level, ...)
{
    int i;
	if (level<3) plevel=level; else return -1;
	if (level==1) {
		if (startIntervalTime==0.0) startIntervalTime=PMPI_Wtime();
	}
	if (level==0) {
		if (startIntervalTime!=0.0) intervalTime +=(PMPI_Wtime()-startIntervalTime);
		startIntervalTime=0.0;
	}
	if (level==2) {
		startIntervalTime=PMPI_Wtime();
		plevel=1;
		for(i=0;i<NUM_FUNC;i++) {
		    if (prof_info[i]!=NULL) {
			free(prof_info[i]); prof_info[i]=NULL;
		    }
		}
		intervalMPITime=0.0;
		totalSendBytes=0;
		if (ftrc) {fclose(ftrc);ftrc=fopen(fname,"w");}
	}
	return 0;
}
int MPI_Finalize(void)
{
int i,j;
int myid;
int nsize;
char pstr[256];
int count_op=0;
MPI_Status sts;
char ProcName[128];
char PP_Name[128];
int result_len;
	if(ftrc!=NULL) {fclose(ftrc);}
	if(startIntervalTime) intervalTime += (PMPI_Wtime()-startIntervalTime);
	taskTime=PMPI_Wtime()-taskTime;
	PMPI_Comm_rank(MPI_COMM_WORLD,&myid);
	PMPI_Comm_size(MPI_COMM_WORLD,&nsize);
	PMPI_Get_processor_name(ProcName, &result_len);
	ProcName[result_len]=0;
fflush(NULL);
printf("%d: MPI_Test=%d, MPI_Wtime=%d\n",myid,TEST,WTIME);

printf("%d: tlost=%lf, tnull=%d, Loop_count(0.1)=%ld \n",myid,tlost,tnull,MAXLOOP);

PMPI_Barrier(MPI_COMM_WORLD);
	for(i=0;i<NUM_FUNC;i++) if(prof_info[i]!=NULL) count_op++;
	if (myid!=0) {
/*		PMPI_Send(&count_op,1,MPI_INT,0,010152,MPI_COMM_WORLD);*/
		ar_time[0]=taskTime;
		ar_time[1]=taskMPITime;
		ar_time[2]=intervalTime;
		ar_time[3]=intervalMPITime;		
		PMPI_Send(ar_time,4,MPI_DOUBLE,0,010154,MPI_COMM_WORLD);
		PMPI_Send(&totalSendBytes,1,MPI_LONG,0,010153,MPI_COMM_WORLD);
	} else {
		printf("\n==========MPI_FUNCTIONS statistic%s==========\n",count_op?(char *)&myid:" is empty");

	}
	if(myid==0) {
minSendBytes=totalSendBytes;
maxSendBytes=totalSendBytes;
midSendBytes=totalSendBytes;

mintaskTime=taskTime;
maxtaskTime=taskTime;
midtaskTime=taskTime;

mintaskMPITime=taskMPITime;
maxtaskMPITime=taskMPITime;
midtaskMPITime=taskMPITime;

minintervalTime=intervalTime;
maxintervalTime=intervalTime;
midintervalTime=intervalTime;

minintervalMPITime=intervalMPITime;
maxintervalMPITime=intervalMPITime;
midintervalMPITime=intervalMPITime;
	   ar_times=(double *)calloc(nsize-1,sizeof(double)*4);
	   othersBytes=(long *)calloc(nsize-1,sizeof(long));
	   for(i=1;i<nsize;i++) {
/*		PMPI_Recv(&count_op,1,MPI_INT,i,010152,MPI_COMM_WORLD,&sts);*/
		PMPI_Recv(&ar_times[(i-1)*4],4,MPI_DOUBLE,i,010154,MPI_COMM_WORLD,&sts);
		PMPI_Recv(&othersBytes[i-1],1,MPI_LONG,i,010153,MPI_COMM_WORLD,&sts);
		LOC(SendBytes,othersBytes[i-1],i);
		LOC(taskTime,ar_times[(i-1)*4+0],i);
		LOC(taskMPITime,ar_times[(i-1)*4+1],i);
		LOC(intervalTime,ar_times[(i-1)*4+2],i);
		LOC(intervalMPITime,ar_times[(i-1)*4+3],i);
	   }
printf("-------------------------------------------\n\
              	   Tmin Np	   Tmax Np	   Tmid\n\
TaskTime	%lf %d	%lf %d	%lf\n\
TaskMPITime	%lf %d	%lf %d	%lf\n\
IntervalTime	%lf %d	%lf %d	%lf\n\
IntervalMPITime	%lf %d	%lf %d	%lf\n\
SendBytes	%ld %d	%ld %d	%ld\n\
-------------------------------------------\n",
mintaskTime,pmintaskTime,maxtaskTime,pmaxtaskTime,midtaskTime/nsize,
mintaskMPITime,pmintaskMPITime,maxtaskMPITime,pmaxtaskMPITime,midtaskMPITime/nsize,
minintervalTime,pminintervalTime,maxintervalTime,pmaxintervalTime,midintervalTime/nsize,
minintervalMPITime,pminintervalMPITime,maxintervalMPITime,pmaxintervalMPITime,midintervalMPITime/nsize,
minSendBytes,pminSendBytes,maxSendBytes,pmaxSendBytes,midSendBytes/nsize
);
	   printf("%d: TaskTime=%lf, TaskMPITime=%lf, IntervalTime=%lf, IntervalMPITime=%lf, TotalSendBytes=%ld\n",\
		myid,\
		taskTime,taskMPITime,intervalTime,intervalMPITime,totalSendBytes\
	   );
	   for(i=0;i<nsize-1;i++) {
		printf("%d: TaskTime=%lf, TaskMPITime=%lf, IntervalTime=%lf, IntervalMPITime=%lf, TotalSendBytes=%ld\n",\
			i+1,\
			ar_times[i*4+0],ar_times[i*4+1],ar_times[i*4+2],ar_times[i*4+3],othersBytes[i]\
		);
	   }
	}
	if (myid==0) printf("---------------on 0 (%s) ----------------------------\n",ProcName);
	else {
/**/
    	    PMPI_Send(&count_op,1,MPI_INT,0,010152,MPI_COMM_WORLD);
	    PMPI_Send(ProcName,128,MPI_CHAR,0,261051,MPI_COMM_WORLD);
	}
	for(i=0;i<NUM_FUNC;i++) {
	   if (prof_info[i]==NULL) continue;
	   sprintf(pstr,"%d: %s:\tcount=%d, totalTime=%lf, maxTime=%lf",\
		myid,\
		prof_info[i]->name,\
		prof_info[i]->count,\
		prof_info[i]->totalTime,\
		prof_info[i]->maxTime\
	   );
	   if (prof_info[i]->totalSendBytes!=-1)
		sprintf(pstr+(int)(strlen(pstr)),", totalSendBytes=%ld, maxSendBytes=%ld",\
			prof_info[i]->totalSendBytes,\
			prof_info[i]->maxSendBytes\
		);
	   if (prof_info[i]->totalRecvBytes!=-1)
		sprintf(pstr+strlen(pstr),", totalRecvBytes=%ld, maxRecvBytes=%ld",\
			prof_info[i]->totalRecvBytes,\
			prof_info[i]->maxRecvBytes\
		);
	   if (prof_info[i]->other)
		sprintf(pstr+strlen(pstr),", otherFunc=%d",\
			prof_info[i]->other\
		);
	   if (myid==0) {
		printf("%s\n",pstr);
	   } else {
		PMPI_Send(pstr,strlen(pstr)+1,MPI_CHAR,0,261051,MPI_COMM_WORLD);
	   }
	}
	if (myid==0) {
	   printf("Receive from others processors...\n");
	   for(i=1;i<nsize;i++) {
		PMPI_Recv(&count_op,1,MPI_INT,i,010152,MPI_COMM_WORLD,&sts);
		PMPI_Recv(PP_Name,128,MPI_CHAR,i,261051,MPI_COMM_WORLD,&sts);
		
		printf("Receive from %d (%s) - %d message...\n",i,PP_Name,count_op);
		for(j=0;j<count_op;j++) {
			PMPI_Recv(pstr,256,MPI_CHAR,i,261051,MPI_COMM_WORLD,&sts);
			printf("%s\n",pstr);
		}
	   }
	}
	i=PMPI_Finalize();
	fflush(NULL);

/*	kill(-1,SIGKILL);*/
	return i;
}

int MPI_Send(void* buf, int count, MPI_Datatype datatype, int dest, int tag, 
	     MPI_Comm comm)
{

	PROF_START(Send);
	PROF_SBYTES;
	res = PMPI_Send(buf, count, datatype, dest, tag, comm);
	PROF_TIME;
	if(ftrc && plevel==1 && level_func==0) fprintf(ftrc,"Send to %d %d bytes for %lf time\n",dest,count*extent,toper);\
/*	TRC_SEND(Send);*/
	PROF_FINISH;
	return res;
}

int MPI_Isend(void* buf, int count, MPI_Datatype datatype, int dest, int tag, 
	     MPI_Comm comm, MPI_Request *request)
{
	PROF_START(Isend);
/*if(myid==0)
fprintf(fout,"%d: Isend - buf=%p,cnt=%d,dst=%d,tag=%d,request=%p-%d\n",myid,buf,count,dest,tag,request,(int)*request);
fprintf(fout,"%d: Send\n",myid);
*/

	PROF_SBYTES;
	res = PMPI_Isend(buf, count, datatype, dest, tag, comm, request);
	PROF_TIME;
	if(ftrc && plevel==1 && level_func==0) fprintf(ftrc,"Isend to %d %d bytes for %lf time, request=%p\n",dest,count*extent,toper,request);\
	PROF_FINISH;
	return res;

}

int MPI_Recv(void* buf, int count, MPI_Datatype datatype, int source, 
	     int tag, MPI_Comm comm, MPI_Status *status)
{
	PROF_START(Recv);
	res = PMPI_Recv(buf, count, datatype, source, tag, comm, status);
/*	PROF_RBYTES;*/
	PROF_TIME;
	if(ftrc && plevel==1 && level_func==0) {
		PMPI_Type_size(datatype, &extent);
		fprintf(ftrc,"Recv from %d %d bytes for %lf time\n",source,count*extent,toper);\
	}
	PROF_FINISH;
	return res;
}

int MPI_Irecv(void* buf, int count, MPI_Datatype datatype, int source, 
	     int tag, MPI_Comm comm, MPI_Request *request)
{
	PROF_START(Irecv);

	res = PMPI_Irecv(buf, count, datatype, source, tag, comm, request);
/*
if(level_func==1) {
 	add_req(request);
}
*/
        PROF_TIME;
	if(ftrc && plevel==1 && level_func==0) {
		PMPI_Type_size(datatype, &extent);
		fprintf(ftrc,"Irecv from %d %d bytes for %lf time, request=%p\n",source,count*extent,toper,request);\
	}
	PROF_FINISH;
	return res;
}

int MPI_Wait(MPI_Request *request, MPI_Status *status)
{
struct tail_req *pp;
	PROF_START(Wait);
	res = PMPI_Wait(request, status);
/*
if(level_func==1) {
	if((pp=find_req(request))!=NULL) {cur_func=pp->func;PROF_RBYTES; del_req(request);}
}
*/
        PROF_TIME;
	if(ftrc && plevel==1 && level_func==0) fprintf(ftrc,"Wait %lf time request=%p\n",toper,request);\
	PROF_FINISH;
	return res;
}


int MPI_Waitall(int count, MPI_Request *array_of_requests, 
		MPI_Status *array_of_statuses)
{
MPI_Status *status;
MPI_Request *request;
int i;
struct tail_req *pp;
	PROF_START(Waitall);
	res = PMPI_Waitall(count,array_of_requests, array_of_statuses);
/*
if(level_func==1) {
   for(i=0;i<count;i++) {
	status=&array_of_statuses[i];
	request=&array_of_requests[i];
	if((pp=find_req(request))!=NULL) {cur_func=pp->func;PROF_RBYTES; del_req(request);}
   }
}
*/        PROF_TIME;
	if(ftrc && plevel==1 && level_func==0) fprintf(ftrc,"Waitall %lf time %d requests\n",toper,count);\
	PROF_FINISH;
	return res;
}


int MPI_Barrier(MPI_Comm comm )
{
	PROF_START(Barrier);
	res = PMPI_Barrier( comm );
	PROF_TIME;
	PROF_FINISH;
	return res;
}
int MPI_Comm_dup(MPI_Comm comm, MPI_Comm *newcomm)
{
	PROF_START(Comm_dup);
	res = PMPI_Comm_dup(comm,newcomm);
	PROF_TIME;
	PROF_FINISH;
	return res;
}
int MPI_Comm_split(MPI_Comm comm, int color, int key, MPI_Comm *newcomm)
{
	PROF_START(Comm_split);
	res = PMPI_Comm_split(comm,color,key,newcomm);
	PROF_TIME;
	PROF_FINISH;
	return res;
}
int MPI_Bcast(void* buffer, int count, MPI_Datatype datatype, int root, 
	      MPI_Comm comm )
{
	PROF_START(Bcast);
	PROF_SBYTES;
	res = PMPI_Bcast(buffer, count, datatype, root, comm);
	PROF_TIME;
	PROF_FINISH;
	return res;
}
int MPI_Alltoall(void* sendbuf, int sendcount, MPI_Datatype sendtype, 
		 void* recvbuf, int recvcount, MPI_Datatype recvtype, 
		 MPI_Comm comm)
{
int np;
MPI_Datatype datatype=sendtype;
int count;
	PMPI_Comm_size(comm,&np);
	count=sendcount*np;

	PROF_START(Alltoall);
	PROF_SBYTES;
	res = PMPI_Alltoall(sendbuf,sendcount,sendtype,recvbuf,recvcount,recvtype,comm);
	PROF_TIME;
	PROF_FINISH;
	return res;
}
int MPI_Alltoallv(void* sendbuf, int *sendcounts, int *sdispls, 
		  MPI_Datatype sendtype, void* recvbuf, int *recvcounts, 
		  int *rdispls, MPI_Datatype recvtype, MPI_Comm comm)
{
MPI_Datatype datatype=sendtype;
int count=0;
int np;
int i;
PMPI_Comm_size(comm,&np);
for(i=0;i<np;i++) count+=sendcounts[i];
	PROF_START(Alltoallv);
	PROF_SBYTES;
	res= PMPI_Alltoallv(sendbuf,sendcounts,sdispls,
		  sendtype,recvbuf,recvcounts,
		  rdispls,recvtype,comm);
	PROF_TIME;
	PROF_FINISH;
	return res;

}
int MPI_Reduce(void* sendbuf, void* recvbuf, int count, 
	       MPI_Datatype datatype, MPI_Op op, int root, MPI_Comm comm)
{
	PROF_START(Reduce);
	PROF_SBYTES;
	res = PMPI_Reduce(sendbuf,recvbuf,count,datatype,op,root,comm);
	PROF_TIME;
	PROF_FINISH;
	return res;
}
int MPI_Allreduce(void* sendbuf, void* recvbuf, int count, 
		  MPI_Datatype datatype, MPI_Op op, MPI_Comm comm)
{
/*
if(myid==0)
printf("%d: Allreduce(%d) sbuf=%p rbuf=%p count=%d\n",myid, level_func,sendbuf,recvbuf,count);
*/
	PROF_START(Allreduce);
	PROF_SBYTES;
	res = PMPI_Allreduce(sendbuf,recvbuf,count,datatype,op,comm);
/*
if(myid==0)
printf("%d: Allreduce end\n",myid);
*/

	PROF_TIME;
	PROF_FINISH;
	return res;
}
double MPI_Wtime(void) {
double res;
    res=0;
/*    DELAY;*/
    PROF_START(Wtime);
    WTIME++;
    res=PMPI_Wtime();
    PROF_TIME;
    PROF_FINISH;
    return res;
}
int MPI_Test(MPI_Request *request, int *flag, MPI_Status *status) {
    DELAY;
    TEST++;
    res=  PMPI_Test(request, flag, status);
    PROF_FINISH;
    return res;
}


/*
int MPI_Get_count(MPI_Status *status, MPI_Datatype datatype, int *count);
int MPI_Bsend(void* buf, int count, MPI_Datatype datatype, int dest, int tag, 
	      MPI_Comm comm);
int MPI_Ssend(void* buf, int count, MPI_Datatype datatype, int dest, int tag, 
	      MPI_Comm comm);
int MPI_Rsend(void* buf, int count, MPI_Datatype datatype, int dest, int tag, 
	      MPI_Comm comm);
int MPI_Buffer_attach( void* buffer, int size);
int MPI_Buffer_detach( void* buffer, int* size);
int MPI_Ibsend(void* buf, int count, MPI_Datatype datatype, int dest, 
	       int tag, MPI_Comm comm, MPI_Request *request);
int MPI_Issend(void* buf, int count, MPI_Datatype datatype, int dest, 
	       int tag, MPI_Comm comm, MPI_Request *request);
int MPI_Irsend(void* buf, int count, MPI_Datatype datatype, int dest, 
	       int tag, MPI_Comm comm, MPI_Request *request);
int MPI_Test(MPI_Request *request, int *flag, MPI_Status *status);
int MPI_Request_free(MPI_Request *request);
int MPI_Waitany(int, MPI_Request *, int *, MPI_Status *);
int MPI_Testany(int, MPI_Request *, int *, int *, MPI_Status *);
int MPI_Testall(int count, MPI_Request *array_of_requests, int *flag, 
		MPI_Status *array_of_statuses);
int MPI_Waitsome(int incount, MPI_Request *array_of_requests, int *outcount, 
		 int *array_of_indices, MPI_Status *array_of_statuses);
int MPI_Testsome(int incount, MPI_Request *array_of_requests, int *outcount, 
		 int *array_of_indices, MPI_Status *array_of_statuses);
int MPI_Iprobe(int source, int tag, MPI_Comm comm, int *flag, 
	       MPI_Status *status);
int MPI_Probe(int source, int tag, MPI_Comm comm, MPI_Status *status);
int MPI_Cancel(MPI_Request *request);
int MPI_Test_cancelled(MPI_Status *status, int *flag);
int MPI_Send_init(void* buf, int count, MPI_Datatype datatype, int dest, 
		  int tag, MPI_Comm comm, MPI_Request *request);
int MPI_Bsend_init(void* buf, int count, MPI_Datatype datatype, int dest, 
		   int tag, MPI_Comm comm, MPI_Request *request);
int MPI_Ssend_init(void* buf, int count, MPI_Datatype datatype, int dest, 
		   int tag, MPI_Comm comm, MPI_Request *request);
int MPI_Rsend_init(void* buf, int count, MPI_Datatype datatype, int dest, 
		   int tag, MPI_Comm comm, MPI_Request *request);
int MPI_Recv_init(void* buf, int count, MPI_Datatype datatype, int source, 
		  int tag, MPI_Comm comm, MPI_Request *request);
int MPI_Start(MPI_Request *request);
int MPI_Startall(int count, MPI_Request *array_of_requests);
int MPI_Sendrecv(void *sendbuf, int sendcount, MPI_Datatype sendtype, 
		 int dest, int sendtag, void *recvbuf, int recvcount, 
		 MPI_Datatype recvtype, int source, int recvtag, 
		 MPI_Comm comm, MPI_Status *status);
int MPI_Sendrecv_replace(void* buf, int count, MPI_Datatype datatype, 
			 int dest, int sendtag, int source, int recvtag, 
			 MPI_Comm comm, MPI_Status *status);
int MPI_Type_contiguous(int count, MPI_Datatype oldtype, 
			MPI_Datatype *newtype);
int MPI_Type_vector(int count, int blocklength, int stride, 
		    MPI_Datatype oldtype, MPI_Datatype *newtype);
int MPI_Type_hvector(int count, int blocklength, MPI_Aint stride, 
		     MPI_Datatype oldtype, MPI_Datatype *newtype);
int MPI_Type_indexed(int count, int *array_of_blocklengths, 
		     int *array_of_displacements, MPI_Datatype oldtype, 
		     MPI_Datatype *newtype);
int MPI_Type_hindexed(int count, int *array_of_blocklengths, 
		      MPI_Aint *array_of_displacements, MPI_Datatype oldtype, 
		      MPI_Datatype *newtype);
int MPI_Type_struct(int count, int *array_of_blocklengths, 
		    MPI_Aint *array_of_displacements, 
		    MPI_Datatype *array_of_types, MPI_Datatype *newtype);
int MPI_Address(void* location, MPI_Aint *address);
int MPI_Type_extent(MPI_Datatype datatype, MPI_Aint *extent);

int MPI_Type_size(MPI_Datatype datatype, int *size);
int MPI_Type_count(MPI_Datatype datatype, int *count);
int MPI_Type_lb(MPI_Datatype datatype, MPI_Aint* displacement);
int MPI_Type_ub(MPI_Datatype datatype, MPI_Aint* displacement);
int MPI_Type_commit(MPI_Datatype *datatype);
int MPI_Type_free(MPI_Datatype *datatype);
int MPI_Get_elements(MPI_Status *status, MPI_Datatype datatype, int *count);
int MPI_Pack(void* inbuf, int incount, MPI_Datatype datatype, void *outbuf, 
	     int outsize, int *position,  MPI_Comm comm);
int MPI_Unpack(void* inbuf, int insize, int *position, void *outbuf, 
	       int outcount, MPI_Datatype datatype, MPI_Comm comm);
int MPI_Pack_size(int incount, MPI_Datatype datatype, MPI_Comm comm, 
		  int *size);
int MPI_Gather(void* sendbuf, int sendcount, MPI_Datatype sendtype, 
	       void* recvbuf, int recvcount, MPI_Datatype recvtype, 
	       int root, MPI_Comm comm); 
int MPI_Gatherv(void* sendbuf, int sendcount, MPI_Datatype sendtype, 
		void* recvbuf, int *recvcounts, int *displs, 
		MPI_Datatype recvtype, int root, MPI_Comm comm); 
int MPI_Scatter(void* sendbuf, int sendcount, MPI_Datatype sendtype, 
		void* recvbuf, int recvcount, MPI_Datatype recvtype, 
		int root, MPI_Comm comm);
int MPI_Scatterv(void* sendbuf, int *sendcounts, int *displs, 
		 MPI_Datatype sendtype, void* recvbuf, int recvcount, 
		 MPI_Datatype recvtype, int root, MPI_Comm comm);
int MPI_Allgather(void* sendbuf, int sendcount, MPI_Datatype sendtype, 
		  void* recvbuf, int recvcount, MPI_Datatype recvtype, 
		  MPI_Comm comm);
int MPI_Allgatherv(void* sendbuf, int sendcount, MPI_Datatype sendtype, 
		   void* recvbuf, int *recvcounts, int *displs, 
		   MPI_Datatype recvtype, MPI_Comm comm);
int MPI_Op_create(MPI_User_function *, int, MPI_Op *);
int MPI_Op_free( MPI_Op *);
int MPI_Reduce_scatter(void* sendbuf, void* recvbuf, int *recvcounts, 
		       MPI_Datatype datatype, MPI_Op op, MPI_Comm comm);
int MPI_Scan(void* sendbuf, void* recvbuf, int count, MPI_Datatype datatype, 
	     MPI_Op op, MPI_Comm comm );
int MPI_Group_size(MPI_Group group, int *size);
int MPI_Group_rank(MPI_Group group, int *rank);
int MPI_Group_translate_ranks (MPI_Group group1, int n, int *ranks1, 
			       MPI_Group group2, int *ranks2);
int MPI_Group_compare(MPI_Group group1, MPI_Group group2, int *result);
int MPI_Comm_group(MPI_Comm comm, MPI_Group *group);
int MPI_Group_union(MPI_Group group1, MPI_Group group2, MPI_Group *newgroup);
int MPI_Group_intersection(MPI_Group group1, MPI_Group group2, 
			   MPI_Group *newgroup);
int MPI_Group_difference(MPI_Group group1, MPI_Group group2, 
			 MPI_Group *newgroup);
int MPI_Group_incl(MPI_Group group, int n, int *ranks, MPI_Group *newgroup);
int MPI_Group_excl(MPI_Group group, int n, int *ranks, MPI_Group *newgroup);
int MPI_Group_range_incl(MPI_Group group, int n, int ranges[][3], 
			 MPI_Group *newgroup);
int MPI_Group_range_excl(MPI_Group group, int n, int ranges[][3], 
			 MPI_Group *newgroup);
int MPI_Group_free(MPI_Group *group);
int MPI_Comm_size(MPI_Comm comm, int *size);
int MPI_Comm_rank(MPI_Comm comm, int *rank);
int MPI_Comm_compare(MPI_Comm comm1, MPI_Comm comm2, int *result);
int MPI_Comm_create(MPI_Comm comm, MPI_Group group, MPI_Comm *newcomm);
int MPI_Comm_free(MPI_Comm *comm);
int MPI_Comm_test_inter(MPI_Comm comm, int *flag);
int MPI_Comm_remote_size(MPI_Comm comm, int *size);
int MPI_Comm_remote_group(MPI_Comm comm, MPI_Group *group);
int MPI_Intercomm_create(MPI_Comm local_comm, int local_leader, 
			 MPI_Comm peer_comm, int remote_leader, 
			 int tag, MPI_Comm *newintercomm);
int MPI_Intercomm_merge(MPI_Comm intercomm, int high, MPI_Comm *newintracomm);
int MPI_Keyval_create(MPI_Copy_function *copy_fn, 
		      MPI_Delete_function *delete_fn, 
		      int *keyval, void* extra_state);
int MPI_Keyval_free(int *keyval);
int MPI_Attr_put(MPI_Comm comm, int keyval, void* attribute_val);
int MPI_Attr_get(MPI_Comm comm, int keyval, void *attribute_val, int *flag);
int MPI_Attr_delete(MPI_Comm comm, int keyval);
int MPI_Topo_test(MPI_Comm comm, int *status);
int MPI_Cart_create(MPI_Comm comm_old, int ndims, int *dims, int *periods,
		    int reorder, MPI_Comm *comm_cart);
int MPI_Dims_create(int nnodes, int ndims, int *dims);
int MPI_Graph_create(MPI_Comm, int, int *, int *, int, MPI_Comm *);
int MPI_Graphdims_get(MPI_Comm comm, int *nnodes, int *nedges);
int MPI_Graph_get(MPI_Comm, int, int, int *, int *);
int MPI_Cartdim_get(MPI_Comm comm, int *ndims);
int MPI_Cart_get(MPI_Comm comm, int maxdims, int *dims, int *periods,
		 int *coords);
int MPI_Cart_rank(MPI_Comm comm, int *coords, int *rank);
int MPI_Cart_coords(MPI_Comm comm, int rank, int maxdims, int *coords);
int MPI_Graph_neighbors_count(MPI_Comm comm, int rank, int *nneighbors);
int MPI_Graph_neighbors(MPI_Comm comm, int rank, int maxneighbors,
			int *neighbors);
int MPI_Cart_shift(MPI_Comm comm, int direction, int disp, 
		   int *rank_source, int *rank_dest);
int MPI_Cart_sub(MPI_Comm comm, int *remain_dims, MPI_Comm *newcomm);
int MPI_Cart_map(MPI_Comm comm, int ndims, int *dims, int *periods, 
		 int *newrank);
int MPI_Graph_map(MPI_Comm, int, int *, int *, int *);
int MPI_Get_processor_name(char *name, int *result_len);
int MPI_Errhandler_create(MPI_Handler_function *function, 
			  MPI_Errhandler *errhandler);
int MPI_Errhandler_set(MPI_Comm comm, MPI_Errhandler errhandler);
int MPI_Errhandler_get(MPI_Comm comm, MPI_Errhandler *errhandler);
int MPI_Errhandler_free(MPI_Errhandler *errhandler);
int MPI_Error_string(int errorcode, char *string, int *result_len);
int MPI_Error_class(int errorcode, int *errorclass);
double MPI_Wtick(void);
#ifndef MPI_Wtime
double PMPI_Wtime(void);
double PMPI_Wtick(void);
*/
/*
    MPI_: 
* MPI_allreduce;
* MPI_alltoall;
* MPI_barrier;
* MPI_bcast;
* MPI_comm_split;
* MPI_comm_dup;
? MPI_error:
* MPI_irecv;
* MPI_recv;
* MPI_isend;
* MPI_send;
MPI_ireduce;
* MPI_reduce;
* MPI_wait;
* MPI_waitall;
---------------
MPI_init;
MPI_comm_rank;
MPI_comm_size;
MPI_finalize;
MPI_abort;

*/