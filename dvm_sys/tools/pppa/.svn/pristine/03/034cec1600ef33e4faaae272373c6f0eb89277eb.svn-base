#if !defined( _STATREAD_H )
#define _STATREAD_H
#include "potensyn.h"
#include "inter.h"
#include "treeinter.h"
#include "statinter.h"
#define Fic_index 2000000000 //interval.h
#define PREC 4
#define DIGTIME 6+PREC
// sizeof(nameOper[i])=DIGTIME
#define DIGSTAT 5

enum typeprint {PRGEN,PRCOM,PRRCOM,PRSYN,PRVAR,PROVER,PRCALL,PRCALLS,PRLOST};
static const char *nameGen[ITER+1]={
"Lost time              ",
"User insufficient par. ",
"Sys.insufficient par.  ",
"Idle time              ",
"Communication          ",
"Real synchronization   ",
"Synchronization        ",
"Variation              ",
"Overlap                ",
"Load imbalance         ",
"Execution time         ",
"User CPU time          ",
"Sys. CPU time          ",
"I/O time               ",
"Start operation        ",
"Threads user time      ",
"Threads system time    ",
"Productive time GPU    ",
"Lost time GPU          ",
"Processors             "
};


static const char *nameGenMT[StatGrpCount]={
    "UserGrp                ",
    "MsgPasGrp              ",
    "StartRedGrp            ",
    "WaitRedGrp             ",
    "RedGrp                 ",
    "StartShdGrp            ",
    "WaitShdGrp             ",
    "ShdGrp                 ",
    "DistrGrp               ",
    "ReDistrGrp             ",
    "MapPLGrp               ", //10
    "DoPLGrp                ",
    "ProgBlockGrp           ",
    "IOGrp                  ",
    "RemAccessGrp           ",
    "UserDebGrp             ",
    "StatistGrp             ",
    "DvmhCpyHtoDTime", //17
    "DvmhCpyHtoDBytes",
    "DvmhCpyDtoHTime",
    "DvmhCpyDtoHBytes",
    "DvmhCpyDtoDTime",
    "DvmhCpyDtoDBytes",
    "DvmhCyclePortionTime",
    "DvmhArrayTransformTime",
    "DvmhArrayTransformBytes",
    "DvmhArrayReductionTime",
    "SystemGrp              "
};
static const char *nameCom[RED+1]={
"I/O             ",
"Reduction       ",
"Shadow          ",
"Remote access   ",
"Redistribution  "
};
static const char *nameOper[SUMOVERLAP-SUMCOM+1]={
"  Communic",
" Real_sync",
"   Synchro",
" Variation",
"   Overlap"
};
enum tmps {EMP,GNS,ROU,MPI,PVM};
struct vms_const {
    short  reverse,szsh,szl,
        szv,szd;
};
struct vms_short {
    short  rank,maxnlev,
             smallbuff,lvers;
};
struct vms_long{
    long proccount,mpstype,ioproc,
             qfrag,lbuf,linter,lsynchro;
};
struct vms_void{
    void *pbuffer;
};
struct vms_double{
    double proctime;
};
typedef struct tvms{
        vms_double d;
        vms_void v;
        vms_long l;
    vms_short sh;
    vms_const chc;
} pvms;
#define QV_CONST sizeof(vms_const)/SZSH
#define QV_SHORT sizeof(vms_short)/SZSH
#define QV_LONG sizeof(vms_long)/SZL
#define QV_VOID sizeof(vms_void)/SZV
#define QV_DOUBLE sizeof(vms_double)/SZD
class CStatRead {
public:
    CStatRead(const char * name,int i,int j,short sore);
    ~CStatRead(void);
    unsigned long QProc(void);
    unsigned long BeginTreeWalk(void);
    unsigned long TreeWalk(void);
    BOOL Valid(int *warn);
    void TextErr(char *t);
    short ReadTitle(char * p);
    void ReadIdent(ident *idp);
    void ReadProcS(struct ProcTimes * pt);
    BOOL ReadProc(typeprint t,unsigned long *pnumb,int qnumb,short fmt,double sum,char *str);
    void MinMaxSum(typeprint t,double *min,unsigned long *nprocmin,
                          double*max,unsigned long *nprocmax,
                          double *sum);
    void GrpTimes(double *arrprod,double *arrlost,double *arrcalls,int nproc);

    /**
     * Получить DVMH интервальную статистику по процессу
     *
     * @param nProc номер процесса
     *
     * @return интервальная DVMH статистики
     */
    dvmh_stat_interval * getDvmhStatIntervalByProcess(unsigned long nProc);


    /**
     * Получить DVMH описатель GPU по процессу
     *
     * @param nProc номер процесса
     * @param nGpu  номер GPU
     *
     * @return описатель GPU
     */
    dvmh_stat_header_gpu_info * getDvmhStatGpuInfoByProcess(unsigned long nProc, int nGpu);

    /**
     * Получить количество нитей по процессу    
     *
     * @param nProc номер процесса
     *
     */ 
    unsigned long getThreadsAmountByProcess(unsigned long nProc);
    /*
    *
    * Получить max min для нитей на процессоре
    * @param maxs массив максисумов
    * @param mins массив минимумов
    * @param meds массив средних
    * @param nProc номер процесса
    */
    void getMaxsAndMins(double* maxs, double* mins, double* meds, unsigned long* n, unsigned long nProc);

    void VMSSize(char *p);
    void WasErrAccum(char *p);
    long ReadCall(typecom t);
    char *ReadVers(void);
    char *ReadPlatform(void);
    void NameTimeProc(unsigned long n,char **name,double *time);

private:
    unsigned long proccount,curnproc;
    BOOL valid,valid_synchro;
    short rank;
    unsigned char *pch_vmssize,*pch_vms;
    short maxnlevel;
    char texterr[80];
    char textwarning[3][80];
    int valid_warning;
    CTreeInter **pclfrag;
    CSynchro **pclsyn;
    short smallbuff;
    CInter **pic;
    int nf,curntime;
    char *pvers;
    unsigned char *pbufcompr,*pbufuncompr;
    short gzplus; // sign of file gz/ gz+
    BOOL Synchro(void);
    };
#endif
