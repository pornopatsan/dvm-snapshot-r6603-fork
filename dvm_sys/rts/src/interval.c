#ifndef _INTERVAL_C_
#define _INTERVAL_C_

#include "statist.h"
#include "interval.h"

short UserInter = 0;
DvmType NLine_coll;


/**
 * Создать пользовательский интервал
 *
 *
 * @param nfrag	порядковый номер цикла для идентификации парных команд
 * @param index	значение выражения в пользовательском интервале
 */
void __callstd binter_(DvmType *nfrag, DvmType *index) {
    // RTL_STAT - general flag to turn statistics gathering on
    // STAT_TRACE - enable tracing calls

    dvmh_stat_log(DVMH_STAT_LOG_LVL_DOWN, "binter_ (start)");

    if(RTL_STAT) {
        if (UserInter > 0) { /* intermediate interval */
            UserInter++;
            dvmh_stat_log(DVMH_STAT_LOG_LVL_UP, "binter_ (stop:1)");
            return;
        }

        DVMFTimeStart(call_binter_);

        if(STAT_TRACE)
           dvm_trace(call_binter_,"nfrag=%ld; index=%ld UserInter=%i;\n", *nfrag, *index, UserInter);

        if (FindInter(USER, *index) == 0)
            CreateInter(USER, *index);

        if(STAT_TRACE)
            dvm_trace(ret_binter_," \n"); fflush(stdout);

        (DVM_RET);

        DVMFTimeFinish(ret_binter_);
            if (UserInter==1) {
                biinter_(PREFIX);
        }
    }

    dvmh_stat_log(DVMH_STAT_LOG_LVL_UP, "binter_ (stop)");

    return;
}

/**
 * Завершить пользовательский интервал
 *
 *
 * @param nfrag	порядковый номер цикла для идентификации парных команд
 * @param nline	номер строки в исходном файле
 */
void __callstd einter_(DvmType  *nfrag, DvmType  *nline) {
    dvmh_stat_log(DVMH_STAT_LOG_LVL_DOWN, "einter_ (start)");

    // RTL_STAT - general flag to turn statistics gathering on
    if(RTL_STAT) {
        if (UserInter>1) {
            UserInter--;
            dvmh_stat_log(DVMH_STAT_LOG_LVL_UP, "einter_ (stop:1)");
            return;
        } /* intermediate interval */

        if (UserInter==1) {
            eiinter_(); /* PREFIX or collective interval */
            UserInter=0;
            NLine_coll=0;
        }

        DVMFTimeStart(call_einter_);

        if(STAT_TRACE)
            dvm_trace(call_einter_,"nfrag=%ld; nline=%ld\n", *nfrag, *nline);

        EndInter(*nline);

        if(STAT_TRACE)
            dvm_trace(ret_einter_," \n"); fflush(stdout);

        (DVM_RET);

        DVMFTimeFinish(ret_einter_);
    }

    dvmh_stat_log(DVMH_STAT_LOG_LVL_UP, "einter_ (stop)");

    return;
}

/**
 * Создать коллективный интервал @todo
 *
 *
 * @param nitem тип интервала коллективной опперации
 */
void __callstd biinter_(short nitem) {
    dvmh_stat_log(DVMH_STAT_LOG_LVL_DOWN, "biinter_ (start)");

	// RTL_STAT - general flag to turn statistics gathering on
    if(RTL_STAT) {
        DVMFTimeStart(call_binter_);

        if(STAT_TRACE)
            dvm_trace(call_binter_,"call_biinter line=%ld; nitem=%i;\n",DVM_LINE[0], nitem);

        /* create collective interval */
        if (FindInter(nitem,Fic_index)==0) 
            CreateInter(nitem,Fic_index);
        NLine_coll=DVM_LINE[0];
        if(STAT_TRACE)
            dvm_trace(ret_binter_,"ret_biinter\n"); fflush(stdout);

        (DVM_RET);

        DVMFTimeFinish(ret_binter_);
    }

    dvmh_stat_log(DVMH_STAT_LOG_LVL_UP, "biinter_ (stop)");

    return;
}

/**
 * Закрыть текущий коллективный интервал
 */
void __callstd eiinter_() {
    dvmh_stat_log(DVMH_STAT_LOG_LVL_DOWN, "eiinter_ (start)");

	// RTL_STAT - general flag to turn statistics gathering on
    if(RTL_STAT) {
        DVMFTimeStart(call_einter_);

        if(STAT_TRACE)
            dvm_trace(call_einter_,"call_eiinter nline=%ld\n",NLine_coll);

        EndInter(NLine_coll); /* PREFIX or collective interval */

        if(STAT_TRACE)
            dvm_trace(ret_einter_,"ret_eiinter\n"); fflush(stdout);

        (DVM_RET);

        DVMFTimeFinish(ret_einter_);
    }

    dvmh_stat_log(DVMH_STAT_LOG_LVL_UP, "eiinter_ (stop)");

    return;
}

/**
 * Создать последовательный интервал для цикла
 *
 *
 * @param nfrag	порядковый номер цикла для идентификации парных команд
 */
void __callstd bsloop_(DvmType  *nfrag) {
    dvmh_stat_log(DVMH_STAT_LOG_LVL_DOWN, "bsloop_ (start)");

    // RTL_STAT - general flag to turn statistics gathering on
    if(RTL_STAT) {
        if (UserInter!=0) {
            dvmh_stat_log(DVMH_STAT_LOG_LVL_UP, "bsloop_ (stop:1)");
            return;
        } /* intermediate interval */

        DVMFTimeStart(call_bsloop_);

        if(STAT_TRACE)
            dvm_trace(call_bsloop_,"nfrag=%ld;\n", *nfrag);


        if (FindInter(SEQ,Fic_index)==0)
            CreateInter(SEQ,Fic_index);

        if(STAT_TRACE)
           dvm_trace(ret_bsloop_," \n"); fflush(stdout);

        (DVM_RET);

        DVMFTimeFinish(ret_bsloop_);
    }

    dvmh_stat_log(DVMH_STAT_LOG_LVL_UP, "bsloop_ (stop)");

    return;
}

/**
 * Создать праллельный интервал для цикла
 *
 *
 * @param nfrag	порядковый номер цикла для идентификации парных команд
 */
void __callstd bploop_(DvmType  *nfrag) {
    dvmh_stat_log(DVMH_STAT_LOG_LVL_DOWN, "bploop_ (start)");

    // RTL_STAT - general flag to turn statistics gathering on
    if(RTL_STAT) {
        if (UserInter!=0) {
            dvmh_stat_log(DVMH_STAT_LOG_LVL_UP, "bploop_ (stop:1)");
            return;
        } /* intermediate interval */
        DVMFTimeStart(call_bploop_);

        if(STAT_TRACE)
            dvm_trace(call_bploop_,"nfrag=%ld\n", *nfrag);

        if (FindInter(PAR,Fic_index)==0)
            CreateInter(PAR,Fic_index);

        if(STAT_TRACE)
            dvm_trace(ret_bploop_," \n"); fflush(stdout);

        (DVM_RET);

        DVMFTimeFinish(ret_bploop_);
    }

    dvmh_stat_log(DVMH_STAT_LOG_LVL_UP, "bploop_ (stop)");

    return;
}

/**
 * Закрыть текущий последовательный и праллельный интервал для цикла
 *
 *
 * @param nfrag	порядковый номер цикла для идентификации парных команд
 * @param nline	номер строки в исходном файле
 */
void __callstd enloop_(DvmType  *nfrag, DvmType  *nline) {
    dvmh_stat_log(DVMH_STAT_LOG_LVL_DOWN, "enloop_ (start)");

  	// RTL_STAT - general flag to turn statistics gathering on
    if(RTL_STAT) {
        if (UserInter!=0) {
            dvmh_stat_log(DVMH_STAT_LOG_LVL_UP, "enloop_ (stop:1)");
            return;
        } /* intermediate interval */
        DVMFTimeStart(call_enloop_);

        if(STAT_TRACE)
            dvm_trace(call_enloop_,"nfrag=%ld; nline=%ld\n", *nfrag, *nline);

        EndInter(*nline);

        if(STAT_TRACE)
            dvm_trace(ret_enloop_," \n"); fflush(stdout);

        (DVM_RET);

        DVMFTimeFinish(ret_enloop_);
    }

    dvmh_stat_log(DVMH_STAT_LOG_LVL_UP, "enloop_ (stop)");

    return;
}

#endif  /* _INTERVAL_C_ */
