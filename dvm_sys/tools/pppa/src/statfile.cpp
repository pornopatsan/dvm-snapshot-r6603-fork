#include "bool.h"
#define _STATFILE_
#include "statread.h"
#include <stdlib.h>
#include <string.h>
#include <float.h>
#include <exception>
#include <new>

#ifdef __SPF_BUILT_IN_PPPA
#define VERS "406" 
#define PLATFORM "ntmpich" 
#else
#include "dvmvers.h"
#endif

#include "statprintf.h"

using namespace std;
/**
 * Человекопонятный формат размера
 *
 * @param bytes     размер в байтах
 * @param precision количество знаков после запятой
 *
 * @return отформатированная строка размера
 */
static char *humanizeSize(const unsigned long long bytes, const unsigned short precision);

/**
 * Подготовить значение dvmh статистики к печати
 *
 * @param value      сырое значение
 * @param isPositive значение строго положительно?
 * @param isSize     значение это размер?
 * @param dashOnZero выводить вместо нуля? "-"
 * @param isTime     значение это время (не работает, при установленном isSize)?
 *
 * @return отформатированное значение в виде строки
 */
static char *dvmhPrepareValue(
        const double value,
        const unsigned short isPositive,
        const unsigned short isSize,
        const unsigned short dashOnZero,
        const unsigned short isTime
);

/**
 * Маски определяющие уровень вывода информации
 */
enum VerbosityLevel {
        VERBOSITY_LVL_NONE = 0,
        VERBOSITY_LVL_CPU  = 1,
        VERBOSITY_LVL_GPU  = 2
    };

static void printHelp()
{
    printf("Performance analyser parameters: <file _name1> <file_name2> [additional_opt] \n");  
    printf("  <file_name1>       Statistics file name.\n");
    printf("  <file_name2>       Output file name.\n");
    printf("--------------------------------------------------------------------------\n");
    printf("Additional options:\n");
    printf("  -l <level>        Interval <level> number.\n");
    printf("  -n <list>         List of processor numbers,\n");
    printf("                    <list> = 0 - without processor characteristics.\n");
    printf("  -t <list>         List of threads numbers,\n");
    printf("                    <list> = 0 - all threads characteristics.\n");
    printf("  -v <verbosity>    Verbosity level, set that detailed information whould be\n");
    printf("                    displayed. <verbosity>:\n");
    printf("                       gpu     - display only GPU characteristics.\n");
    printf("                       cpu     - display only CPU characteristics.\n");
    printf("                       none    - do not display detailed characteristics.\n");
    printf("                       gpu,cpu - (default) display CPU and GPU characteristics.\n");
    printf("  -c <compr_level>  [0..9],-1: compression level of output file,-1-no compression\n");
    printf("  -m <matrix>       <i[:<groupname>]|j[:<groupname>]\n");
    printf("                    i|j - sum of row or column elements of matrix of\n");
    printf("                    interval characteristics.\n");
    printf("  <groupname>       Name of group, set to output row or column per elements.\n");
    printf("                    <groupname>:\n");
    printf("                       UserGrp|MsgPasGrp|StartRedGrp|WaitRedGrp|RedGrp|\n");
    printf("                       StartShdGrp|WaitShdGrp|ShdGrp|DistrGrp|ReDistrGrp|\n");
    printf("                       MapPLGrp|DoPLGrp|ProgBlockGrp|IOGrp|RemAccessGrp|\n");
    printf("                       UserDebGrp|StatistGrp|SystemGrp.\n");
}

#ifdef __SPF_BUILT_IN_PPPA
int pppa_analyzer(int argv, char** argc)
#else
int main(int argv, char **argc)
#endif
{
    //sfn outfn -l d>=0 -c d=-1..9 -n d-d>=0 -m text
    if (argv < 3 || strcmp(argc[1], "help") == 0)
    {
        printHelp();
#ifdef __SPF_BUILT_IN_PPPA
        return 1;
#else
        return 0;
#endif
    }

    if (argv > 14) {
        printf("Incorrect number of parameters\n");
        printHelp();
#ifdef __SPF_BUILT_IN_PPPA
        return 1;
#else
        exit(1);
#endif
    }

    BOOL proc = TRUE, comp = TRUE, gen = TRUE;
    BOOL dvmh_gpu = TRUE, dvmh_threads = FALSE, dvmh_threads_full = FALSE;
    int verbosity = VERBOSITY_LVL_CPU | VERBOSITY_LVL_GPU;
    char compr[3], mode[5];
    strcpy(mode, "wb0");
    int nlevel = 9999, qnumb = 1, qnumthreads = 1;
    unsigned long *pnumb = NULL;
    unsigned long *pnumthreads = NULL;
    char * token;
    int iIM = 0, jIM = 0, nparin = 0, nparout = 0;
    short sore = 0;
    size_t st = 0;
    //new_handler set_new_handler(NULL);
    nparin = 1;// statistics file
    nparout = 2;// output file
    for (int npar = 3; npar < argv; npar++) { // key parameters
        int i, cc;
        char let;
        char arrs[24] = "                       "; // strlen nameGen[i]
        if (argc[npar][2] != 0) {
            printf("Incorrect parameter %s\n", argc[npar]);
#ifdef __SPF_BUILT_IN_PPPA
            return 1;
#else
            exit(1);
#endif
        }
        if (argv == npar + 1) {
            printf("Parameter for %s not set\n", argc[npar]);
#ifdef __SPF_BUILT_IN_PPPA
            return 1;
#else
            exit(1);
#endif
        }
        switch (argc[npar][1]) {
        case 'l':
            //interval level
            npar++;
            nlevel = atoi(argc[npar]);
            if (nlevel < 0 || (nlevel == 0 && strcmp(argc[npar], "0") != 0)) {
                printf("Incorrect number of level %s \n", argc[npar]);
#ifdef __SPF_BUILT_IN_PPPA
                return 1;
#else
                exit(1);
#endif
            }
            break;
        case 'c':
            // compression level
            npar++;
            mode[0] = 0; compr[0] = 0;
            cc = atoi(argc[npar]);
            if (cc < -1 || cc>9) {
                printf("Incorrect compression level of file %s \n", argc[3]);
#ifdef __SPF_BUILT_IN_PPPA
                return 1;
#else
                exit(1);
#endif
            }
            if (cc == -1) {
                strcpy(compr, "0");
            }
            else {
                int comprd;
                if (cc == 0) strcpy(compr, "-1");
                else {
                    comprd = cc;
                    sprintf(compr, "%d", comprd);
                }
            }
            strcpy(mode, "wb");
            strcat(mode, compr);
            break;
        case 'n':
            // list of processor numbers
            npar++;
            qnumb = 1;
            i = 0;
            while (argc[npar][i] != 0) {
                if (argc[npar][i] == ',') qnumb++;
                i++;
            }
            pnumb = new unsigned long(qnumb * 2);
            if (pnumb == NULL) throw("Out of memory\n");
            token = strtok(argc[npar], ",");
            i = 0;
            while (token != NULL) {
                char *tdiap = strchr(token, '-');
                if (tdiap == NULL) {
                    pnumb[i] = atoi(token);
                    pnumb[i + 1] = pnumb[i];
                }
                else {
                    pnumb[i + 1] = atoi(tdiap + 1);
                    tdiap[0] = '\0';
                    pnumb[i] = atoi(token);
                }
                token = strtok(NULL, ",\n");
                i = i + 2;
            } // end while
            // -n 0 - not print processor characteristics
            if (qnumb == 1 && pnumb[0] == 0 && pnumb[1] == 0)
            {
                proc = FALSE;
                dvmh_threads = FALSE;
                dvmh_gpu = FALSE;
            }
            break;
        case 't':
            //list of threads to show
            npar++;
            qnumthreads = 1;
            i = 0;
            while (argc[npar][i] != 0) {
                if (argc[npar][i] == ',') qnumthreads++;
                i++;
            }
            pnumthreads = new unsigned long(qnumthreads * 2);
            if (pnumthreads == NULL) throw("Out of memory\n");
            token = strtok(argc[npar], ",");
            i = 0;
            while (token != NULL) {
                char *tdiap = strchr(token, '-');
                if (tdiap == NULL) {
                    pnumthreads[i] = atoi(token);
                    pnumthreads[i + 1] = pnumthreads[i];
                }
                else {
                    pnumthreads[i + 1] = atoi(tdiap + 1);
                    tdiap[0] = '\0';
                    pnumthreads[i] = atoi(token);
                }
                token = strtok(NULL, ",\n");
                i = i + 2;
            } // end while
            // -t 0 - print all threads characteristics
            if (qnumthreads == 1 && pnumthreads[0] == 0 && pnumthreads[1] == 0)
            {
                dvmh_threads = TRUE;
                dvmh_threads_full = TRUE;
            }
            break;
        case 'm':
            // interval matrix
            npar++;
            let = argc[npar][0];
            if (let != 'i' && let != 'j') {
                printf("Incorrect %d parameter %c, must be i/j\n", npar, let);
#ifdef __SPF_BUILT_IN_PPPA
                return 1;
#else
                exit(1);
#endif
            }
            sore = 1;
            if (let == 'i') iIM = 1; else jIM = 1;
            st = strlen(argc[npar]);
            if (st == 1) break;
            sore = 0;  // element
            if (argc[npar][1] != ':') {
                printf("Incorrect %d parameter %s, must be i=<groupname>/j=<groupname>\n", npar, argc[npar]);
#ifdef __SPF_BUILT_IN_PPPA
                return 1;
#else
                exit(1);
#endif
            }
            strncpy(arrs, &(argc[npar][2]), st - 2);
            iIM = 0; jIM = 0;
            for (i = 1; i <= StatGrpCount; i++) {
                if (strcmp(nameGenMT[i - 1], arrs) == 0) {
                    if (let == 'i') iIM = i;
                    else jIM = i;
                    break;
                }
            } // end for
            if (iIM == 0 && jIM == 0) {
                printf("Incorrect group name %s \n", argc[npar]);
#ifdef __SPF_BUILT_IN_PPPA
                return 1;
#else
                exit(1);
#endif
            }
            break;
        case 'v':
            // verbosity level
            npar++;
            verbosity = VERBOSITY_LVL_NONE;

            {
                char *buf = argc[npar];
                char *flag = buf;
                char cond = 1;

                do {
                    if (*buf == '\0') cond = 0;
                    if (*buf == '\0' || *buf == ',') {
                        *buf = '\0';
                        if (strcmp(flag, "cpu") == 0) {
                            verbosity = verbosity | VERBOSITY_LVL_CPU;
                        }
                        else
                            if (strcmp(flag, "gpu") == 0) {
                                verbosity = verbosity | VERBOSITY_LVL_GPU;
                            }
                            else
                                if (strcmp(flag, "none") != 0) {
                                    printf("Incorrect verbosity level %s \n", argc[npar]);
#ifdef __SPF_BUILT_IN_PPPA
                                    return 1;
#else
                                    exit(1);
#endif
                                }
                        flag = ++buf;
                    }
                    else ++buf;
                } while (cond);
            }

            break;
        default:
            printf("Incorrect parameter %s\n", argc[npar]);
#ifdef __SPF_BUILT_IN_PPPA
            return 1;
#else
            exit(1);
#endif
            break;
        } // end switch
    } // end for key parameters
    // read time characteristics and syn times
    try {
        CStatRead stat(argc[nparin], iIM, jIM, sore);
        int warn;
        if (stat.Valid(&warn) != TRUE) {
            char t[80];
            stat.TextErr(t);
            printf("%s", t);
            printHelp();
#ifdef __SPF_BUILT_IN_PPPA
            return 1;
#else
            exit(1);
#endif
        }

        // Возвращает количество процессоров, на которых считалась задача.
        unsigned long qproc = stat.QProc();

        if (qproc == 0) 
        {
#ifdef __SPF_BUILT_IN_PPPA
            return 1;
#else
            exit(1);
#endif
        }
        // string for processor characteristics - max
        // printf for compressed and not compressed out file
        CStatPrintf statpr(argc[nparout], 1024, mode);
        if (statpr.Valid() != TRUE) {
            char t[80];
            statpr.TextErr(t);
            printf("%s", t);
#ifdef __SPF_BUILT_IN_PPPA
            return 1;
#else
            exit(1);
#endif
        }
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
        const char *namecomp[3] = { "Tmin","Tmax","Tmid" };
        int ltxt = strlen(nameCom[0]) + 1;
        char p_heading[80];
        int lenstr = 0;
        char *poutstr = NULL;
        int i;
        stat.VMSSize(p_heading);
        statpr.StatPrintf("Processor system=%s\n", p_heading);
        char * pvers = stat.ReadVers();
        char *pplat = stat.ReadPlatform();
        statpr.StatPrintf("Statistics has been accumulated on DVM-system version %s, platform %s\n", pvers, pplat);
        statpr.StatPrintf("Analyzer is executing on DVM-system version %s, platform %s\n", VERS, PLATFORM);
        for (i = 0; i < warn; i++) { // warning message
            stat.WasErrAccum(p_heading);
            statpr.StatPrintf("!! %s", p_heading);
        }
        short dig_time = 0;
        unsigned long n = stat.BeginTreeWalk();
        while (n != 0) {
            short nlev = stat.ReadTitle(p_heading); //  информация о заголовке, идентифицирующем интервал (видимо уровень)
            if (nlev <= nlevel) {
                statpr.StatPrintf("%s", "-------------------------------------------------------------------------\n");
                statpr.StatPrintf("%s", p_heading);
                // calculate min,max,sum values for all times
                stat.MinMaxSum(PRGEN, min, nprocmin, max, nprocmax, sum);
                stat.MinMaxSum(PRCOM, minc, nprocminc, maxc, nprocmaxc, sumc);
                stat.MinMaxSum(PRRCOM, minrc, nprocminrc, maxrc, nprocmaxrc, sumrc);
                stat.MinMaxSum(PRSYN, mins, nprocmins, maxs, nprocmaxs, sums);
                stat.MinMaxSum(PRVAR, minv, nprocminv, maxv, nprocmaxv, sumv);
                stat.MinMaxSum(PROVER, minov, nprocminov, maxov, nprocmaxov, sumov);
                if (dig_time == 0) { // format for print
                    double max_val = 0.0;
                    for (i = 0; i <= RED; i++) {
                        if (max_val < sumc[i]) max_val = sumc[i];
                        if (max_val < sumrc[i]) max_val = sumrc[i];
                        if (max_val < sums[i]) max_val = sums[i];
                        if (max_val < sumv[i]) max_val = sumv[i];
                        if (max_val < sumov[i]) max_val = sumov[i];
                    } // end for
                    char tval[80];
                    sprintf(tval, "%*.*lf", DIGTIME, PREC, max_val);
                    dig_time = (short)strlen(tval);
                    lenstr = (dig_time + 1)*qproc + strlen(nameGen[0]) + 1;
                    if (lenstr <= 1024) lenstr = 1024; else statpr.ChangeLenStr(lenstr);
                    poutstr = new char[lenstr];
                    if (poutstr == NULL) throw("Out of memory\n");
                }
                if (gen == TRUE) {
                    statpr.StatPrintf("--- The main characteristics --- \n");
                    // double time1,prodcpu,timef,prod;

                    double prod_cpu, prod_sys, prod_io, prod;
                    prod_cpu = sum[CPUUSR];
                    prod_sys = sum[CPU];
                    prod_io = sum[IOTIME];
                    prod = prod_cpu + prod_sys + prod_io;

                    double exec_time, sys_time, efficiency;
                    exec_time = max[EXEC];
                    sys_time = n * max[EXEC];
                    efficiency = sys_time ? prod / sys_time : 0.0;
                    unsigned long threadsOfAllProcs = 0;
                    for (unsigned int not_i = 1; not_i <= n; ++not_i)
                        threadsOfAllProcs += stat.getThreadsAmountByProcess(not_i);

                    statpr.StatPrintf("%s %*.*lf \n", "Parallelization efficiency ", dig_time, PREC, efficiency);
                    statpr.StatPrintf("%s %*.*lf \n", "Execution time             ", dig_time, PREC, exec_time);
                    statpr.StatPrintf("%s %*d\n", "Processors                 ", dig_time, n);
                    statpr.StatPrintf("%s %*d\n", "Threads amount             ", dig_time, threadsOfAllProcs);
                    statpr.StatPrintf("%s %*.*lf\n", "Total time                 ", dig_time, PREC, sys_time);

                    if (prod > 0.0) {
                        statpr.StatPrintf("%s %*.*lf %s %.*lf %s %.*lf %s %.*lf %c\n",
                            "Productive time            ", dig_time, PREC, prod,
                            "( CPU=", PREC, prod_cpu,
                            "Sys=", PREC, prod_sys,
                            "I/O=", PREC, prod_io,
                            ')'
                        );
                    }

                    if (sum[LOST] > 0.0)statpr.StatPrintf("%s %*.*lf \n", "Lost  time                 ", dig_time, PREC, sum[LOST]);
                    if (sum[INSUFUSR] + sum[INSUF] != 0.0)
                        statpr.StatPrintf("%s %*.*lf %s %.*lf %s %.*lf %c\n", "   Insufficient parallelism", dig_time, PREC, sum[INSUFUSR] + sum[INSUF],
                            "( User=", PREC, sum[INSUFUSR], "Sys=", PREC, sum[INSUF], ')');
                    if (sum[SUMCOM] != 0.0)
                        statpr.StatPrintf("%s %*.*lf %s %.*lf %s %.*lf %c\n", "   Communication           ", dig_time, PREC, sum[SUMCOM],
                            "( Real_sync=", PREC, sum[SUMRCOM], "Starts=", PREC, sum[START], ')');
                    if (sum[IDLE] != 0.0)   statpr.StatPrintf("%s %*.*lf\n", "   Idle time               ", dig_time, PREC, sum[IDLE]);
                    if (sum[IMB] != 0.0)statpr.StatPrintf("%s %*.*lf\n", "Load imbalance             ",
                        dig_time, PREC, sum[IMB]);
                    if (sum[SUMSYN] != 0.0) statpr.StatPrintf("%s %*.*lf\n", "Synchronization            ", dig_time, PREC, sum[SUMSYN]);
                    if (sum[SUMVAR] != 0.0) statpr.StatPrintf("%s %*.*lf\n", "Time variation             ", dig_time, PREC, sum[SUMVAR]);
                    if (sum[SUMOVERLAP] > 0.0)statpr.StatPrintf("%s %*.*lf\n", "Overlap                    ", dig_time, PREC,
                        sum[SUMOVERLAP]);

                    if (sum[DVMH_THREADS_USER_TIME] > 0.0)
                        statpr.StatPrintf("%s %*.*lf\n", "Threads user time          ", dig_time, PREC, sum[DVMH_THREADS_USER_TIME]);
                    if (sum[DVMH_THREADS_SYSTEM_TIME] > 0.0)
                        statpr.StatPrintf("%s %*.*lf\n", "Threads system time        ", dig_time, PREC, sum[DVMH_THREADS_SYSTEM_TIME]);

                    if (sum[DVMH_GPU_TIME_PRODUCTIVE] > 0.0)
                        statpr.StatPrintf("%s %*.*lf\n", "Productive time GPU        ", dig_time, PREC, sum[DVMH_GPU_TIME_PRODUCTIVE]);
                    if (sum[DVMH_GPU_TIME_LOST] > 0.0)
                        statpr.StatPrintf("%s %*.*lf\n", "Lost time GPU              ", dig_time, PREC, sum[DVMH_GPU_TIME_LOST]);


                    long ncall = 0;
                    int dig_stat = DIGSTAT;
                    for (i = 0; i <= RED; i++) ncall = ncall + stat.ReadCall(typecom(i));
                    if (ncall > 0) {
                        statpr.StatPrintf("%*c", ltxt, ' ');
                        char tval[20];
                        sprintf(tval, "%ld", ncall);
                        int l = strlen(tval);
                        if (l > DIGSTAT)dig_stat = l;
                        statpr.StatPrintf("%*s", dig_stat, " Nop ");
                        for (int j = SUMCOM; j <= SUMOVERLAP; j++) {
                            if (sum[j] > 0.0)
                                statpr.StatPrintf("%*s ", dig_time, nameOper[j - SUMCOM]);
                        } // end for
                        statpr.StatPrintf("\n");
                    }// end if
                    for (i = 0; i <= RED; i++) {
                        ncall = stat.ReadCall(typecom(i));
                        if (ncall > 0) {
                            statpr.StatPrintf("%s", nameCom[i]);
                            statpr.StatPrintf("%*d ", dig_stat, ncall);
                            if (sum[SUMCOM] > 0.0)statpr.StatPrintf("%*.*lf ", dig_time, PREC, sumc[i]);
                            if (sum[SUMRCOM] > 0.0)statpr.StatPrintf("%*.*lf ", dig_time, PREC, sumrc[i]);
                            if (sum[SUMSYN] > 0.0)statpr.StatPrintf("%*.*lf ", dig_time, PREC, sums[i]);
                            if (sum[SUMVAR] > 0.0)statpr.StatPrintf("%*.*lf ", dig_time, PREC, sumv[i]);
                            if (sum[SUMOVERLAP] > 0.0)statpr.StatPrintf("%*.*lf ", dig_time, PREC, sumov[i]);
                            statpr.StatPrintf("\n");
                        } // end if ncall>0
                    } // end for
                    if (iIM > 0 || jIM > 0) { // statistics matrix
                        for (unsigned long np = 1; np <= qproc; np++) {
                            double prod[StatGrpCount], lost[StatGrpCount], sumprod = 0.0, sumlost = 0.0;
                            double calls[StatGrpCount], sumcalls = 0.0;
                            stat.GrpTimes(prod, lost, calls, np);
                            sprintf(p_heading, "%ld", np);
                            int ll = strlen(nameGen[0]) - strlen(p_heading) - strlen(" Nproc=") - 2;
                            statpr.StatPrintf("%s %d %*c %*s %*s %*s\n", " Nproc=", np, ll, ' ',
                                dig_time, "CALL COUNT", dig_time, "PRODUCT TIME", dig_time,
                                "LOST TIME");
                            for (i = 0; i < StatGrpCount; i++) {
                                sumprod = sumprod + prod[i];
                                sumlost = sumlost + lost[i];
                                sumcalls = sumcalls + calls[i];
                                //if (calls[i]>0 || prod[i]!=0.0 || lost[i]!=0.0 ) {
                                statpr.StatPrintf("%s %*.*lf %*.*lf %*.*lf \n", nameGenMT[i],
                                    dig_time, PREC, calls[i],
                                    dig_time, PREC, prod[i],
                                    dig_time, PREC, lost[i]);
                                //}
                            } // end for
                            statpr.StatPrintf("%s %*.*lf %*.*lf %*.*lf \n", "      Total:           ",
                                dig_time, PREC, sumcalls, dig_time, PREC, sumprod, dig_time, PREC, sumlost);
                        } // end for qproc
                    } // end statistics matrix
                } // end main characteristics
                comp = comp && ((verbosity & VERBOSITY_LVL_CPU) > 0);
                if (comp == TRUE) {
                    // comparative characteristics
                    statpr.StatPrintf("--- The comparative characteristics --- \n");
                    poutstr[0] = 0;
                    statpr.StatPrintf("%*c", strlen(nameGen[0]) + 1, ' ');
                    int i;
                    for (i = 0; i < 3; i++) {
                        if (i == 2)statpr.StatPrintf("%*s\n", dig_time, namecomp[i]);
                        else statpr.StatPrintf("%*s %*s", dig_time, namecomp[i],
                            DIGSTAT, "N proc");
                    }
                    // general characteristics
                    for (i = 0; i <= ITER; i++) {
                        if (sum[i] > 0.0) {
                            int prec;
                            double tt = sum[i] / n;
                            if ((typetime)(i) == PROC || (typetime)(i) == ITER)
                                prec = 0; else prec = PREC;
                            statpr.StatPrintf("%s %*.*lf %*d %*.*lf %*d %*.*lf \n",
                                nameGen[i], dig_time, prec,
                                min[i], DIGSTAT, nprocmin[i], dig_time, prec, max[i], DIGSTAT,
                                nprocmax[i], dig_time, prec, tt);
                        }
                    }
                    long ncall = 0;
                    // characteristics of collective operations
                    for (i = 0; i <= RED; i++) ncall = ncall + stat.ReadCall(typecom(i));
                    if (ncall > 0) {
                        statpr.StatPrintf("%*c", ltxt - 2, ' ');
                        for (int j = SUMCOM; j <= SUMOVERLAP; j++) {
                            if (sum[j] > 0.0)
                                statpr.StatPrintf("%*s  ", dig_time + DIGSTAT, nameOper[j - SUMCOM]);
                        }
                        statpr.StatPrintf("\n");
                        for (i = 0; i <= RED; i++) {
                            for (int k = 0; k < 3; k++) {
                                if (sumc[i] == 0.0 && sumrc[i] == 0.0 && sums[i] == 0.0 &&
                                    sumv[i] == 0.0 &&sumov[i] == 0.0) break;
                                double t[CALL];//com,realcom,syn,var,overlap
                                unsigned long pnp[CALL];
                                // 0 - min; 1 - max; 2 - sum
                                switch (k) {
                                case 0:
                                    t[0] = minc[i]; t[1] = minrc[i];
                                    t[2] = mins[i]; t[3] = minv[i];
                                    t[4] = minov[i];
                                    pnp[0] = nprocminc[i];
                                    pnp[1] = nprocminrc[i];
                                    pnp[2] = nprocmins[i];
                                    pnp[3] = nprocminv[i];
                                    pnp[4] = nprocminov[i];
                                    break;
                                case 1:
                                    t[0] = maxc[i]; t[1] = maxrc[i];
                                    t[2] = maxs[i]; t[3] = maxv[i];
                                    t[4] = maxov[i];
                                    pnp[0] = nprocmaxc[i];
                                    pnp[1] = nprocmaxrc[i];
                                    pnp[2] = nprocmaxs[i];
                                    pnp[3] = nprocmaxv[i];
                                    pnp[4] = nprocmaxov[i];
                                    break;
                                case 2:
                                    t[0] = sumc[i] / n; t[1] = sumrc[i] / n;
                                    t[2] = sums[i] / n; t[3] = sumv[i] / n;
                                    t[4] = sumov[i] / n;
                                    pnp[0] = 0;
                                    pnp[1] = 0;
                                    pnp[2] = 0;
                                    pnp[3] = 0;
                                    pnp[4] = 0;
                                    break;
                                default:
                                    statpr.StatPrintf("Unknown type=%d\n", k);
#ifdef __SPF_BUILT_IN_PPPA
                                    return 1;
#else
                                    exit(1);
#endif
                                }// end switch
                                statpr.StatPrintf("%s%s", nameCom[i], namecomp[k]);
                                for (int j = SUMCOM; j <= SUMOVERLAP; j++) {
                                    if (sum[j] > 0.0) {
                                        if (pnp[0] == 0) {
                                            statpr.StatPrintf("%*.*lf ",
                                                dig_time, PREC, t[j - SUMCOM]);
                                            statpr.StatPrintf("%*c", DIGSTAT + 1, ' ');
                                        }
                                        else
                                            statpr.StatPrintf("%*.*lf %*d ",
                                                dig_time, PREC, t[j - SUMCOM],
                                                DIGSTAT, pnp[j - SUMCOM]);
                                    }
                                }
                                statpr.StatPrintf("\n");
                            } //end for
                        }//end for
                    }
                }
                proc = proc && ((verbosity & VERBOSITY_LVL_CPU) > 0);
                if (proc == TRUE) {
                    // execution characteristics
                    statpr.StatPrintf("--- The execution characteristics --- \n");
                    statpr.StatPrintf("%s", "                       ");
                    unsigned long i;
                    // print numbers of processor
                    for (i = 0; i < n; i++) {//!!! qproc
                        int pr = FALSE;
                        if (pnumb == NULL) pr = TRUE;
                        else {
                            for (int j = 0; j < qnumb; j++) {
                                if (i + 1 >= pnumb[j] && i + 1 <= pnumb[j + 1]) pr = TRUE;
                            }
                        }
                        if (pr == TRUE) {
                            statpr.StatPrintf("%*d ", dig_time, i + 1); // probel
                        }
                    }
                    statpr.StatPrintf("\n");
                    // general characteristics
                    for (i = 0; i <= ITER; i++) {
                        stat.ReadProc(PRGEN, pnumb, qnumb, dig_time, sum[i], poutstr);
                        if (poutstr[0] != '\0')statpr.StatPrintf("%s\n", poutstr);
                        //statpr.StatPrintf("\n");
                    }
                    //statpr.StatPrintf("\n");
                    // characteristics of collective operatios
                    long ncall = 0;
                    for (int k = 0; k <= RED; k++) ncall = ncall + stat.ReadCall(typecom(k));
                    for (int j = SUMCOM; j <= SUMOVERLAP; j++) {
                        double *ps;
                        switch (j) {
                        case SUMCOM: ps = sumc; break;
                        case SUMRCOM: ps = sumrc; break;
                        case SUMSYN: ps = sums; break;
                        case SUMVAR: ps = sumv; break;
                        case SUMOVERLAP: ps = sumov; break;
                        default:statpr.StatPrintf("Unknown type=%d\n", j);
#ifdef __SPF_BUILT_IN_PPPA
                            return 1;
#else
                            exit(1);
#endif
                        }// end for j
                        int i = 0;
                        //if (j==SUMOVERLAP) ncall=0; //for pc sum[j]=0.0
                        if (sum[j] > 0.0 && ncall > 0) {
                            statpr.StatPrintf("           %s\n", nameGen[j]);
                            for (i = 0; i <= RED; i++) {
                                stat.ReadProc((typeprint)(j - SUMCOM + 1), pnumb, qnumb, dig_time, ps[i], poutstr);
                                if (poutstr[0] != '\0')statpr.StatPrintf("%s\n", poutstr);
                            }
                        }
                    } // end for k
                }//exec characteristics
                //dvmh_threads = dvmh_threads && ((verbosity & VERBOSITY_LVL_CPU) > 0);
        //if(dvmh_threads == TRUE)
                if ((verbosity & VERBOSITY_LVL_CPU) > 0);
                {
                    short isThreadsHeaderPrinted = 0;
                    // Выводим информацию по процессам (нити)
                    for (unsigned long np = 1; np <= qproc; ++np)
                    {
                        int show_this_proc = FALSE;
                        if (pnumb == NULL) show_this_proc = TRUE;
                        else {
                            for (int j = 0; j < qnumb * 2; j += 2) {
                                if (np >= pnumb[j] && np <= pnumb[j + 1]) show_this_proc = TRUE;
                            }
                        }
                        if (show_this_proc)
                        {
                            dvmh_stat_interval        *dvmhStatInterval = stat.getDvmhStatIntervalByProcess(np);
                            dvmh_stat_header_gpu_info *dvmhStatGpuInfo;
                            if (!dvmhStatInterval) {
                                continue;
                            }
                            if (!dvmhStatInterval->threadsUsed)
                                continue;
                            if (!isThreadsHeaderPrinted)
                            {
                                statpr.StatPrintf("\n--- Threads characteristics --- \n");
                                isThreadsHeaderPrinted = 1;
                            }

                            double maxs_t[2];
                            double mins_t[2];
                            double meds_t[2];
                            unsigned long n_t[4];
                            double max_val = 0;
                            stat.getMaxsAndMins(maxs_t, mins_t, meds_t, n_t, np);
                            for (int i = 0; i < 2; ++i)
                            {
                                if (maxs_t[i] > max_val) max_val = maxs_t[i];
                                if (mins_t[i] > max_val) max_val = mins_t[i];
                                if (meds_t[i] > max_val) max_val = meds_t[i];
                            }
                            char tval[40];
                            sprintf(tval, "%*.*lf", DIGTIME, PREC, max_val);
                            short tvlen = strlen(tval);
                            short tnlen = 0;
                            unsigned long tnumval = stat.getThreadsAmountByProcess(np);
                            while (tnumval > 0) { tnumval /= 10; tnlen++; }

                            statpr.StatPrintf("\nProcessor# %ld:\n", np);
                            statpr.StatPrintf("            ");
                            for (int i = 0; i < 3; i++) {
                                if (i == 2)statpr.StatPrintf("%*s\n", dig_time, namecomp[i]);
                                else statpr.StatPrintf("%*s %*s ", dig_time, namecomp[i], tnlen, "N");
                            }
                            statpr.StatPrintf("User time   %*.*f %.*ld %*.*f %.*ld %*.*f\n", tvlen, PREC, mins_t[0], tnlen, n_t[1]
                                , tvlen, PREC, maxs_t[0], tnlen, n_t[0]
                                , tvlen, PREC, meds_t[0]);
                            statpr.StatPrintf("System time %*.*f %.*ld %*.*f %.*ld %*.*f\n", tvlen, PREC, mins_t[1], tnlen, n_t[3]
                                , tvlen, PREC, maxs_t[1], tnlen, n_t[2]
                                , tvlen, PREC, meds_t[1]);
                            if (dvmh_threads)
                                for (unsigned long i = 0; i < stat.getThreadsAmountByProcess(np)/*dvmhStatInterval->max_threads*/; ++i)
                                {
                                    int show_this_thread = FALSE;
                                    if (pnumthreads == NULL) show_this_thread = TRUE;
                                    else {
                                        for (int j = 0; j < qnumthreads * 2; j += 2) {
                                            if (i + 1 >= pnumthreads[j] && i + 1 <= pnumthreads[j + 1]) show_this_thread = TRUE;
                                        }
                                    }
                                    if (show_this_thread || dvmh_threads_full)
                                    {
                                        statpr.StatPrintf("    Thread[%.4ld]\n", i + 1);
                                        statpr.StatPrintf("        User time   %10.*f\n", PREC, dvmhStatInterval->threads[i].user_time);
                                        statpr.StatPrintf("        System time %10.*f\n", PREC, dvmhStatInterval->threads[i].system_time);
                                    }
                                }
                        }
                    }
                    statpr.StatPrintf("\n");
                }
                dvmh_gpu = dvmh_gpu && ((verbosity & VERBOSITY_LVL_GPU) > 0);
                if (dvmh_gpu == TRUE) {
                    short isGPUHeaderPrinted = 0;
                    // Выводим информацию по процессам (GPU)
                    for (unsigned long np = 1; np <= qproc; ++np)
                    {
                        short isProcHeaderPrinted = 0;
                        dvmh_stat_interval        *dvmhStatInterval = stat.getDvmhStatIntervalByProcess(np);
                        dvmh_stat_header_gpu_info *dvmhStatGpuInfo;
                        if (!dvmhStatInterval) {
                            continue;
                        }
                        short havePrevGpu = 0;
                        // Выводи информацию по каждому GPU на процессе
                        for (int gpu = 0; gpu < DVMH_STAT_MAX_GPU_CNT; ++gpu) {
                            if (((dvmhStatInterval->mask >> gpu) & 1) == 0) continue;
                            dvmhStatGpuInfo = stat.getDvmhStatGpuInfoByProcess(np, gpu);
                            dvmh_stat_interval_gpu *dvmhStatGpu = &dvmhStatInterval->gpu[gpu];
                            havePrevGpu = 1;
                            if (!isGPUHeaderPrinted) {
                                statpr.StatPrintf("\n--- The GPU characteristics --- \n");
                                isGPUHeaderPrinted = 1;
                            }

                            if (!isProcHeaderPrinted) {
                                statpr.StatPrintf(" ┌───────────────┐\n");
                                statpr.StatPrintf(" │ Proc: #%-6d │\n", np);
                                statpr.StatPrintf(" └─────┬─────────┘\n");
                                statpr.StatPrintf("       │\n");
                            }

                            int lenName = strlen((char *)dvmhStatGpuInfo->name);
                            if (lenName == 0) {
                                strcpy((char *)dvmhStatGpuInfo->name, DVMH_STAT_GPU_UNKNOWN);
                                lenName = strlen(DVMH_STAT_GPU_UNKNOWN);
                            }

                            int nameSpace = 9;
                            if (dvmhStatGpuInfo->id == -1) nameSpace = -1;

                            statpr.StatPrintf("   ┌");
                            for (int i = 0; i < lenName + 12 + nameSpace; ++i) {
                                if (i == 3 && (havePrevGpu || !isProcHeaderPrinted)) statpr.StatPrintf("┴");
                                else statpr.StatPrintf("─");
                            }
                            isProcHeaderPrinted = 1;
                            statpr.StatPrintf("┐\n");

                            if (dvmhStatGpuInfo->id != -1)
                                statpr.StatPrintf("   │ GPU #%-d[%08X] (%*s) │\n",
                                    gpu + 1, dvmhStatGpuInfo->id, lenName, dvmhStatGpuInfo->name);
                            else
                                statpr.StatPrintf("   │ GPU #%-d (%*s) │\n",
                                    gpu + 1, lenName, dvmhStatGpuInfo->name);

                            statpr.StatPrintf("   ├");

#if DVMH_EXTENDED_STAT == 1
                            int hr_len = 170;
#else
                            int hr_len = 128;
#endif

                            for (int i = 0; i < hr_len; ++i) {
                                if (lenName + 12 + nameSpace == i) statpr.StatPrintf("┴");
                                else statpr.StatPrintf("─");
                            }
                            statpr.StatPrintf("┐\n");

#if DVMH_EXTENDED_STAT == 1
                            statpr.StatPrintf("   │ %35s %6s %13s %13s %13s %13s %13s %13s %13s %13s %13s │\n",
                                "", "#", "Min", "Max", "Sum", "Average",
                                "Q1", "Median", "Q3", "Productive", "Lost");
#else
                            statpr.StatPrintf("   │ %35s %6s %13s %13s %13s %13s %13s %13s │\n",
                                "", "#", "Min", "Max", "Sum", "Average",
                                "Productive", "Lost");
#endif
                            statpr.StatPrintf("   ├");
                            for (int i = 0; i < hr_len; ++i) {
                                statpr.StatPrintf("─");
                            }
                            statpr.StatPrintf("┤\n");
                            // Выводим информацию по метрикам
                            for (int metric = 0; metric < DVMH_STAT_METRIC_CNT; ++metric) {
                                dvmh_stat_interval_gpu_metric *dvmhStatMetric = &dvmhStatGpu->metrics[metric];
                                if (dvmhStatMetric->countMeasures <= 0) continue;

                                short isSize = metric >= DVMH_STAT_METRIC_CPY_DTOH &&
                                    metric <= DVMH_STAT_METRIC_CPY_GET_ACTUAL ||
                                    metric == DVMH_STAT_METRIC_UTIL_ARRAY_TRANSFORMATION;

                                short isPositive = isSize;
                                short isZeroDash = 0;
                                short isZeroDashEx = dvmhStatMetric->hasOwnMeasures <= 0;

                                char *sMin = dvmhPrepareValue(dvmhStatMetric->min, isPositive, isSize, isZeroDash, 0);
                                char *sMax = dvmhPrepareValue(dvmhStatMetric->max, isPositive, isSize, isZeroDash, 0);
                                char *sSum = dvmhPrepareValue(dvmhStatMetric->sum, isPositive, isSize, isZeroDash, 0);
                                char *sMean = dvmhPrepareValue(dvmhStatMetric->mean, isPositive, isSize, isZeroDash, 0);
#if DVMH_EXTENDED_STAT == 1
                                char *sQ1 = dvmhPrepareValue(dvmhStatMetric->q1, isPositive, isSize, isZeroDashEx, 0);
                                char *sMedian = dvmhPrepareValue(dvmhStatMetric->median, isPositive, isSize, isZeroDashEx, 0);
                                char *sQ3 = dvmhPrepareValue(dvmhStatMetric->q3, isPositive, isSize, isZeroDashEx, 0);
#endif
                                char *timeProductive = dvmhPrepareValue(dvmhStatMetric->timeProductive, 1, 0, 1, 1);
                                char *timeLost = dvmhPrepareValue(dvmhStatMetric->timeLost, 1, 0, 1, 1);

#if DVMH_EXTENDED_STAT == 1
                                statpr.StatPrintf("   │ %-35s %6d %13s %13s %13s %13s %13s %13s %13s %13s %13s │\n",
                                    dvmhStatMetricsTitles[metric],
                                    dvmhStatMetric->countMeasures,
                                    sMin,
                                    sMax,
                                    sSum,
                                    sMean,
                                    sQ1,
                                    sMedian,
                                    sQ3,
                                    timeProductive,
                                    timeLost);
#else
                                statpr.StatPrintf("   │ %-35s %6d %13s %13s %13s %13s %13s %13s │\n",
                                    dvmhStatMetricsTitles[metric],
                                    dvmhStatMetric->countMeasures,
                                    sMin,
                                    sMax,
                                    sSum,
                                    sMean,
                                    timeProductive,
                                    timeLost);
#endif

                                free(sMin);
                                free(sMax);
                                free(sSum);
                                free(sMean);
#if DVMH_EXTENDED_STAT == 1
                                free(sQ1);
                                free(sMedian);
                                free(sQ3);
#endif
                                free(timeProductive);
                                free(timeLost);

                            }

                            statpr.StatPrintf("   ├");
                            for (int i = 0; i < hr_len; ++i) {
                                statpr.StatPrintf("─");
                            }
                            statpr.StatPrintf("┤\n");
#if DVMH_EXTENDED_STAT == 1
                            statpr.StatPrintf("   │ Productive time: %10.4fs%141s│\n", dvmhStatGpu->timeProductive, "");
                            statpr.StatPrintf("   │ Lost time      : %10.4fs%141s│\n", dvmhStatGpu->timeLost, "");
#else
                            statpr.StatPrintf("   │ Productive time: %10.4fs%99s│\n", dvmhStatGpu->timeProductive, "");
                            statpr.StatPrintf("   │ Lost time      : %10.4fs%99s│\n", dvmhStatGpu->timeLost, "");
#endif
                            statpr.StatPrintf("   └");
                            for (int i = 0; i < hr_len; ++i) {
                                if ((dvmhStatInterval->mask >> (gpu + 1)) > 0 && i == 3)
                                    statpr.StatPrintf("┬");
                                else
                                    statpr.StatPrintf("─");
                            }
                            statpr.StatPrintf("┘\n");
                        }
                        if (isProcHeaderPrinted && np < qproc)
                            statpr.StatPrintf("\n");
                    }
                    statpr.StatPrintf("\n");
                }
            } // if nlev<=nlevel
            n = stat.TreeWalk();
        }
        //names and times of processors
        char *pname = NULL, *pnamemin = NULL, *pnamemax = NULL;
        double time, mintime = DBL_MAX, maxtime = 0.0, sumtime = 0.0;
        stat.NameTimeProc(0, &pname, &time);
        if (pname == NULL)
        {
            //not MPI
#ifdef __SPF_BUILT_IN_PPPA
            return 1;
#else
            exit(1);
#endif
        }
        unsigned long minn = 0, maxn = 0;
        statpr.StatPrintf("%s", "-------------------------------------------------------------------------\n");
        statpr.StatPrintf("Name (number) and performance time of processors\n");
        for (unsigned long i1 = 0; i1 < qproc; i1++) {
            stat.NameTimeProc(i1, &pname, &time);
            sumtime = sumtime + time;
            if (time < mintime) { mintime = time; minn = i1 + 1; pnamemin = pname; }
            if (time > maxtime) { maxtime = time; maxn = i1 + 1; pnamemax = pname; }
            statpr.StatPrintf("%s(%d) %lf\n", pname, i1 + 1, time);
        }
        statpr.StatPrintf("min - %s(%d) %lf; max - %s(%d) %lf; mid - %lf\n",
            pnamemin, minn, mintime, pnamemax, maxn, maxtime, sumtime / qproc);
    } // end try
    catch (bad_alloc ex) {
        printf("Out of memory\n");
#ifdef __SPF_BUILT_IN_PPPA
        return 1;
#else
        exit(1);
#endif
    }
    catch (exception ex) {
        printf("Exception in standart library %s\n", ex.what());
#ifdef __SPF_BUILT_IN_PPPA
        return 1;
#else
        exit(1);
#endif
    }
    catch (char *str) {
        printf("%s\n", str);
#ifdef __SPF_BUILT_IN_PPPA
        return 1;
#else
        exit(1);
#endif
    }
    return 0;
}

static char *humanizeSize(const unsigned long long bytes, const unsigned short precision) {
    char *buf;

    buf = (char *)malloc(16);
    if (!buf)  printf("-- humanizeSize No memeory! \n"); fflush(stdout);

    if (bytes < 0) strcpy(buf, "-");
    else if (bytes >= (1ll << 30)) sprintf(buf, "%.*lfG", precision, (double)bytes / (1 << 30));
    else if (bytes >= (1ll << 20)) sprintf(buf, "%.*lfM", precision, (double)bytes / (1 << 20));
    else if (bytes >= (1ll << 10)) sprintf(buf, "%.*lfK", precision, (double)bytes / (1 << 10));
    else if (bytes >= 0)         sprintf(buf, "%lluB", bytes);
    else strcpy(buf, "?");

    return buf;
}

static char *dvmhPrepareValue(
        const double value,
        const unsigned short isPositive,
        const unsigned short isSize,
        const unsigned short dashOnZero,
        const unsigned short isTime )
{
    char *buf;

    //printf("dvmhPrepareValue: %.4lf isPositive=%d isSize=%d dashOnZero=%d\n", value, isPositive, isSize, dashOnZero);

    buf = (char *) malloc(16);
    if (!buf)  printf("-- dvmhPrepareValue No memeory! \n"); fflush(stdout);

    if (isPositive && value < 0) {
        strcpy(buf, "-");
    } else if (value == 0 && dashOnZero) {
        strcpy(buf, "-");
    } else if (isSize) {
        free(buf);
        buf = humanizeSize((unsigned long long) value, 3);
    } else if (isTime) {
        sprintf(buf, "%12.4lfs", value);
    } else {
        sprintf(buf, "%13.4lf", value);
    }

    return buf;
}