/* ------------------------------------------------------------------------- */
/*        The Keldish Institute of Applied Mathematics of RAS            (c) */

/*                 The GNS programming system                                */

/* ------------------------------------------------------------------------- */
#ifndef        EXTERN
#define        EXTERN   extern
#endif

typedef        int      TASKID;

/* External variables */


/* Named constants */

#define GNS_VER             3.2
#define TRUE                1
#define FALSE               0
#define NOTASKID            0
#define SEND_ALL           "*"

/* Error Codes */

#define ENOLENGTH        -20
#define ENOHEAP          -30
/*
#ifdef        __STDC__
*/

/* ------------------------------------------------------------------------- */
/*                  The LIBGNS external function prototypes                  */
/* ------------------------------------------------------------------------- */

int gns_newtask(char* task_name, int n_task, int vproc[], TASKID vt[]);
TASKID  gns_mytaskid(void);
void    gns_init(int mode, char* task_name);
TASKID  gns_master(void);
TASKID  gns_parent(void);
void    gns_exit(int gns_status);
void    gns_finish(int gns_status);
int     gns_ntasks(void);
char*   gns_mytaskname(void);

#if 0
void    gns_error(char*,int arg1,int arg2,int arg3,int arg4,int arg5,int arg6,int arg7,int arg8,int arg9);
#endif

void    gns_abort(int gns_status);

/*--------------------Send-Receive function headers ------------------------*/
int  gns_send    (TASKID destid, void* data, int length);
int  gns_senda   (TASKID destid, int tag, void* data, int length);
int  gns_sendnw  (TASKID destid, long* flag , void* data, int length);
int  gns_send2   (char* destid_name, void* data, int length);
int  gns_senda2  (char* destid_name, int tag, void* data, int length);
int  gns_sendnw2 (char* destid_name, int* flag, void* data, int length);
int  gns_send3   (TASKID destid[], int l_of_destid, void* data, int length);
int  gns_senda3  (TASKID destid[], int l_of_destid, int tag, void* data, int length);
int  gns_sendnw3 (TASKID destid[], int l_of_destid, int* flag, void* data, int length);

int  gns_receive   (TASKID sourid, TASKID* source, void* data, int length);
int  gns_receivea  (TASKID sourid, TASKID* source, int tag, void* data, int length);
int  gns_receivenw (TASKID sourid, TASKID* source, long* flag, int* nb, void* data, int length);

void gns_msgdone (long* flag);
void gns_alldone (void);
int  gns_testflag (long* flag);
int  gns_testtag (int tag);
int  gns_testmsg (int tag, TASKID sourid);
int  gns_probe   (void);
void gns_waitmsg (void);

/* NEW  FUNCTIONS  */
TASKID  gns_gettaskid (int virt_proc);
int     gns_myvp (void);
int     gns_ntasksbyname (char* task_name);
void    gns_nmessage (int* message_count);
int     gns_nvirpr (char* task_name,int* array_virpr,int l_array_virpr);
void    gns_terminate (int gns_status);

/* Inline functions */
/*
#endif
*/
int gns_argc(void);
char **gns_argv(void);
