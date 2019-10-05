/*********************************************************************/
/*                  pC++/Sage++  Copyright (C) 1993                  */
/*  Indiana University  University of Oregon  University of Rennes   */
/*********************************************************************/

/*
 * main.c --- Calls parser, dependence graph analyzer, subscript analyzer.
 * Opens and closes the files that are needed. 
 */


#include <stdio.h>

#include "compatible.h"
#ifdef SYS5
#include <string.h>
#else
#include <strings.h>
#endif

#ifdef WIN
#define SLASH_ "\\" 
#else
#define SLASH_ "/"
#endif

#include <stdlib.h>
#include <ctype.h>

#include <fcntl.h>
#include "db.h"
#include "defines.h"
#include "version.h"
#include "fdvm_version.h"
typedef FILE	*FILEP;

#ifdef __SPF
void addToCollection(const int line, const char *file, void *pointer, int type) { }
void removeFromCollection(void *pointer) { }
#endif

extern int	yylineno, yyleng, yylisting, yylonglines, yydebug;
extern int      parstate;   
extern char *   infname;             
extern int	errcnt,	prflag, errline, num_files;
extern PTR_BFND global_bfnd;
extern PTR_BLOB head_blob;
extern PTR_FNAME head_file,cur_thread_file;
extern int     num_bfnds;		/* total # of bif nodes */
extern int     num_llnds;		/* total # of low level nodes */
extern int     num_symbs;		/* total # of symbol nodes */
extern int     num_types;		/* total # of types nodes */
extern int     num_blobs;		/* total # of blob nodes */
extern int     num_sets;		/* total # of set nodes */
extern int     num_cmnt;
extern int     num_def;		/* total # of dependncy nodes */
extern int     num_dep;
extern int     num_label;		/* total # of label nodes */
extern PTR_BFND head_bfnd,	/* start of bfnd chain */
	        cur_bfnd;	/* poextern int to current bfnd */

extern PTR_LLND head_llnd, cur_llnd;
extern PTR_SYMB head_symb, cur_symb;
extern PTR_TYPE head_type, cur_type;
extern PTR_LABEL head_label, cur_label;
extern PTR_FNAME head_file,cur_thread_file;
extern PTR_BLOB head_blob, cur_blob;
extern PTR_SETS head_sets, cur_sets;
extern PTR_DEF head_def, cur_def;
extern PTR_DEFLST head_deflst, cur_deflst;
extern PTR_DEP head_dep, cur_dep, pre_dep;
extern PTR_CMNT head_cmnt, cur_cmnt;
extern PTR_CMNT cur_comment;
PTR_TYPE global_unknown = NULL;

int _filbuf();
PTR_SETS build_sets();
int relink();
int write_nodes();
int fclose();
int inilex();
int dbginilex();
int yyparse();

PTR_BFND get_bfnd();
void fatalstr(), initialize(), errstr(), release_nodes(), close_files();
void errstr_fatal(), err_fatal();
void make_file_list();
char *copys();
char *getenv();
int system();
void free();
void exit();
void err();
char* chkalloc();
int UnparseFDVMProgram();
void PrepareSourceFileForDebug (); /*OMP*/

extern int	language;
extern int warn_all;
/*int	language = ForSrc;*/
FILE	*outf;
static	char *defeditor = "/usr/ucb/vi +%d %s";
int show_deps = 0; /* set to 1 by -sd flag */
int garb_collect = 1; /* disabled by -c flag */
/*static int dep = 0;*/
int dvm_debug = 0;     /*set to 1 by -d1 ...-d4 flags */
/*int only_debug = 0; */   /*set to 1 by  -s flag */
/*int level_debug = 0; */  /*set to 1 by -d1 flag, to 2 by -d2 flag and so on */
/*int perf_analysis =0; */ /*set to 1 by -e1 flag, to 2 by -e2 flag and so on */
int v_print = 0; /*set to 1 by -v flag */
/*int warn_all = 0; */     /*set to 1 by -w flag */
int HPF;
extern int HPF_VERSION;
int HPF_program;
int OMP_program; /*OMP*/
int ACC_program; /*ACC*/
int SAPFOR;      /*SPF*/
int free_form;
int source_form_by_option;
int ftn_std;
int d_line;
int deb_mpi;
int extend_source;
int nchars_per_line;
char *outname;
PTR_FILE fi;
extern PTR_FILE cur_file;
struct include_dir {
        struct include_dir *next;
        char *dir_name;
}; /*list of directories for "include"*/
struct include_dir *incl_first = NULL; /*set by -I flag */
struct include_dir *incl_last;

int is_integer_value(str)
char *str;
{   
   if(!str)
     return 0;
   for( ; *str != '\0'; str++)
        if (! isdigit(*str))
           return 0;
   return 1;
}

void gen_out_name(const char *filename)
{
    register int i;
    char *q, *r;

    if (!filename)
        return;
    
    outname = (char *)malloc((unsigned)(strlen(filename) + 4));
    strcpy(outname, filename);

    for (i = strlen(filename) - 1; i >= 0; i--)
    {
        if (filename[i] == '.')
        {
            q = &(outname[i + 1]);
            break;
        }
    }
    if (i < 0) {
        q = &(outname[strlen(filename)]);
        *q++ = '.';
    }
    /*else if (!strcmp(q,"hpf"))*/

    else if ((q[0] == 'h' || q[0] == 'H') && (q[1] == 'p' || q[1] == 'P') && (q[2] == 'f' || q[2] == 'F') && (q[3] == '\0'))
        HPF_program = 1;
    /* else if ((q[0] == 'f' || q[0] == 'F') && (q[1] == 'd' || q[1] == 'D') && (q[2] == 'v' || q[2] == 'V') && (q[3] =='\0'))
     ;  */
     /* else if (deb_mpi && ( ((q[0] == 'f' || q[0] == 'F') && q[1] == '\0') || ((q[0] == 'f' || q[0] == 'F') && q[1] == '9' && q[2] == '0' && q[3] == '\0' ) )) */
     /* else if (deb_mpi && (!strcmp(q,"f") || !strcmp(q,"F") || !strcmp(q,"f90") || !strcmp(q,"F90") ) ) */
     /* else if (deb_mpi || ftn_std )
      ; */
      /* else
          errstr_fatal("Invalid source file %s ", filename, 300);
      */

#if 0 /* OBSOLETE */
    for (i = strlen(p), q = outname; i; i--)
        if ((*q++ = *p++) == '.') {
            dot = 1;
            break;
        }
#endif /* OBSOLETE */

    r = q;
    if (yylisting) 
    {
        *q++ = 'l';
        *q++ = 's';
        *q++ = 't';
        *q = '\0';
        if ((outf = fopen(outname, "w")) == NULL)
            errstr_fatal("Can't open file %s for write", outname, 6);
    }

    if (HPF) 
    {
        *r++ = 'h';
        *r++ = 'p';
        *r++ = 'f';
    }
    else 
    {
        *r++ = 'd';
        *r++ = 'e';
        *r++ = 'p';
    }
    *r = '\0';
}


int
SourceFormByName (filename)
char *filename;
{
   register int    i;
   char *q;
   
   for (i = strlen(filename)-1 ; i >= 0 ; i --)
   {
        if ( filename[i] == '.' )
        {
             q = &(filename[i+1]);
             break;
        }
   }
 if (i<0) 
   return(0); /* fix source form by default */ 
 
 else if (!strcmp(q,"f90") || !strcmp(q,"F90") )
   return(1);   /* free source form by default */
 else
   return(0); /* fix source form by default */  

}


void add_include_list(dirname)
char *dirname;
{
  struct include_dir *in;
  char  *namep = (char *) malloc((unsigned)(strlen(dirname)+1));
  (void)strcpy(namep, dirname);
  in = (struct include_dir *) chkalloc(sizeof(struct include_dir));
  in->next = NULL;
  in->dir_name = namep;
  if(!incl_first) {
    incl_first = in;
    incl_last = in;
  }
  else {
    incl_last->next = in;
    incl_last = in;
  } 
}

FILE * open_include_file(name)
char   *name;	
{
  char *whole_name,*p;
  FILE *fincl;
  struct include_dir *incld; 
  for(p=name; (*p != '\0'); p++)
   if(*p == *SLASH_){
        fincl = fopen(name, "r"); 
        return(fincl);
   }

  for(incld=incl_first; incld; incld=incld->next) {
     whole_name =(char *) malloc((unsigned)(strlen(incld->dir_name)+strlen(name)+2));
     strcpy(whole_name,incld->dir_name);
     strcat(whole_name,SLASH_);
     strcat(whole_name,name);
     fincl = fopen(whole_name, "r");
     if(fincl)
        return(fincl);
  }
  fincl = fopen(name, "r"); 
  return(fincl);
}

void FragmentList(char *str)
{
cur_num:
 if(!isdigit(*str)) {
     (void)fprintf (stderr, "Warning 002: invalid option argument %s is ignored\n", str);
     return;
 }
 for(str++; (*str != '\0' &&  *str != ',' &&  *str != '-'); str++) 
   if(!isdigit(*str)) {
     (void)fprintf (stderr, "Warning 002: invalid option argument %s is ignored\n", str);
     return;
   }
 if(*str == '\0')
     return;
 str = str+1;
 goto cur_num; 
 
return;
}

int main(int argc, char *argv[])
{
    char *proj_name = "dvm.proj";
    FILE *fproj;
    int k;
    int fromfile = (argv != 0);	/* Flag to see if read from a file */
    void procinit();
    char *input_file = NULL;
    char *debug_file;/*OMP*/
    char *output_file = NULL;
    int gendeps();
    int collect_garbage();
    int no = 0;
    int ad = 0;
    HPF = 0;
    warn_all = 0;
    HPF_program = 0;
    OMP_program = 0; /*OMP*/
    ACC_program = 0; /*ACC*/
    SAPFOR = 0;      /*SPF*/
    /*  free_form = 0; */
    source_form_by_option = 0;
    ftn_std = 0;
    d_line = 0;
    deb_mpi = 0;
    extend_source = 0;
    nchars_per_line = 72 - 6;
#ifdef __SPF
    int noProject = 0;
#endif
    argv++;

    while ((argc > 1) && (*argv)[0] == '-')
    {
        if ((*argv)[1] == 'D')
            yydebug = 1;
        else if ((*argv)[1] == 'I')
            add_include_list((*argv) + 2);
        else if (!strcmp(argv[0], "-o")) {
            output_file = argv[1]; no++;
            argv++;
            argc--;
        }
        else if (!strcmp(argv[0], "-a")) {
            proj_name = argv[1]; ad++;
            argv++;
            argc--;
        }
        else if (!strcmp(argv[0], "-d_line"))
            d_line = 1;
        else if (!strcmp(argv[0], "-dc"))
            ;
        else if (!strcmp(argv[0], "-dbif1"))
            ;
        else if (!strcmp(argv[0], "-dbif2"))
            ;
        else if (!strcmp(argv[0], "-dnoind"))
            ;
        else if (!strcmp(argv[0], "-dvmLoopAnalysisEC"))       /*ACC*/
            ;
        else if (!strcmp(argv[0], "-dvmLoopAnalysis"))   /*ACC*/
            ;
        else if (!strcmp(argv[0], "-dvmPrivateAnalysis"))   /*ACC*/
            ;
        else if (!strcmp(argv[0], "-speedL0")) /*ACC*/
            ;
        else if (!strcmp(argv[0], "-speedL1")) /*ACC*/
            ;
        else if (!strcmp(argv[0], "-uf"))
            ;
        else if (!strcmp(argv[0], "-dmpi"))
            deb_mpi = 1;
        else if (!strcmp(argv[0], "-dperf"))
            dvm_debug = 5;
        else if (!strcmp(argv[0], "-emp"))
            ;
        else if (!strcmp(argv[0], "-extend_source")) {
            extend_source = 132;
            nchars_per_line = extend_source - 6;
        }
        else if (((*argv)[1] == 'd') || ((*argv)[1] == 'e')) {
            if ((*argv)[1] == 'd') {
                switch ((*argv)[2]) {
                case '0': dvm_debug = 0; break; /*OMP*/
                case '1': dvm_debug = 1; break; /*OMP*/
                case '2': dvm_debug = 2; break; /*OMP*/
                case '3': dvm_debug = 3; break; /*OMP*/
                case '4': dvm_debug = 4; break; /*OMP*/
                    /*case '5':*/
                    break;
                default:
                    goto ERR;
                }
            }
            else if ((*argv)[1] == 'e') { /*OMP*/
                switch ((*argv)[2]) { /*OMP*/
                case '0': /*OMP*/
                case '1': /*OMP*/
                case '2': /*OMP*/
                case '3': /*OMP*/
                case '4': /*OMP*/
                    /*case '5':*/
                    break;/*OMP*/
                default: /*OMP*/
                    goto ERR; /*OMP*/
                } /*OMP*/
            }
            if ((*argv)[3] == ':')
                FragmentList(*argv + 4);
            else  if ((*argv)[3] != '\0')
                goto ERR;
        }
        else if (!strcmp(argv[0], "-spf"))
            SAPFOR = 1;
        else if (!strcmp(argv[0], "-p"))
            HPF = 0;
        else if (!strcmp(argv[0], "-s"))
            HPF = 0;
        else if (!strcmp(argv[0], "-v"))
            v_print = 1;
        else if (!strcmp(argv[0], "-w"))
            warn_all = 1;
        else if (!strcmp(argv[0], "-t8"))
            ;
        else if (!strcmp(argv[0], "-t4"))
            ;
        else if (!strcmp(argv[0], "-r8"))
            ;
        else if (!strcmp(argv[0], "-i8"))
            ;
        else if (!strcmp(argv[0], "-bind0"))
            ;
        else if (!strcmp(argv[0], "-bind1"))
            ;
        else if (!strcmp(argv[0], "-mp"))  /*OMP*/
            OMP_program = 1;
        else if (!strncmp(argv[0], "-bufio", 6)) {
            if ((*argv)[6] == '\0' || (!is_integer_value(*argv + 6)))
                goto ERR;
        }
        else if (!strncmp(argv[0], "-ubuf", 5)) {
            if ((*argv)[5] == '\0' || (!is_integer_value(*argv + 5)))
                goto ERR;
        }
        else if (!strcmp(argv[0], "-ioRTS"))
            ;
        else if (!strncmp(argv[0], "-collapse", 9)) {
            if ((*argv)[9] == '\0' || (!is_integer_value(*argv + 9)))
                goto ERR;
        }
        else if (!strcmp(argv[0], "-Obase"))
            ;
        else if (!strcmp(argv[0], "-Oloop_range"))
            ;
        else if (!strcmp(argv[0], "-hpf")) {
            HPF = 1;
            HPF_VERSION = 2;
        }
        else if (!strcmp(argv[0], "-hpf1")) {
            HPF = 1;
            HPF_VERSION = 1;
        }
        else if (!strcmp(argv[0], "-hpf2")) {
            HPF = 1;
            HPF_VERSION = 2;
        }
        else if (!strcmp(argv[0], "-f90")) {
            free_form = 1;
            source_form_by_option = 1;
        }
        else if (!strcmp(argv[0], "-FR")) {
            free_form = 1;
            source_form_by_option = 1;
        }
        else if (!strcmp(argv[0], "-FI")) {
            free_form = 0;
            source_form_by_option = 1;
        }
        else if (!strcmp(argv[0], "-ffo"))
            ;
        else if (!strcmp(argv[0], "-upcase"))
            ;
        else if (!strcmp(argv[0], "-lgstd"))
            ftn_std = 1;
        //else if (!strcmp(argv[0],"-ta"))
        //  ACC_program= 1;
        else if (!strcmp(argv[0], "-noH"))
            ;
        else if (!strcmp(argv[0], "-C_Cuda"))     /*ACC*/
            ;
        else if (!strcmp(argv[0], "-FTN_Cuda") || !strcmp(argv[0], "-F_Cuda"))    /*ACC*/
            ;
        else if (!strcmp(argv[0], "-noCuda"))         /*ACC*/
            ;
        else if (!strcmp(argv[0], "-undef_cuda"))     /*ACC*/
            ;
        else if (!strcmp(argv[0], "-no_blocks_info")) /*ACC*/
            ;
        else if (!strcmp(argv[0], "-noBI"))           /*ACC*/
            ;
        else if (!strcmp(argv[0], "-cacheIdx"))       /*ACC*/
            ;
        else if (!strcmp(argv[0], "-Ohost"))          /*ACC*/
            ;
        else if (!strcmp(argv[0], "-Opl2"))           /*ACC*/
            ;
        else if (!strcmp(argv[0], "-Opl"))           /*ACC*/
            ;
        else if (!strcmp(argv[0], "-oneThread"))     /*ACC*/
            ;
        else if (!strcmp(argv[0], "-noTfm"))         /*ACC*/
            ;
        else if (!strcmp(argv[0], "-autoTfm"))       /*ACC*/
            ;
        else if (!strcmp(argv[0], "-gpuO0"))        /*ACC*/
            ;
        else if (!strcmp(argv[0], "-gpuO1"))        /*ACC*/
            ;
        else if (!strcmp(argv[0], "-rtc"))         /*ACC*/
            ;
        else if ((*argv)[1] == 'H')
        {
            if ((*argv)[2] == 's' && (*argv)[3] == 'h' && (*argv)[4] == 'w')
            {
                if (!is_integer_value(*argv + 5))
                    goto ERR;
            }
            else if (!strcmp(*argv + 2, "nora"))
                ;
            else if (!strcmp(*argv + 2, "oneq"))
                ;
            else if (!strcmp(*argv + 2, "onlyl"))
                ;
            else
                goto ERR;
        }
        else if (!strcmp(argv[0], "-ver"))
        {
            (void)fprintf(stderr, "parser version is \"%s\"\n", VERSION_NUMBER_INT);
            exit(0);
        }
#ifdef __SPF
        else if (!strcmp(argv[0], "-noProject"))
            noProject = 1;        
#endif
        /*   else if (!strcmp(argv[0],"-l"))
         *	 yylisting = 1;
        */
        else
ERR:       (void)fprintf(stderr, "Warning 001: unknown option %s is ignored\n", argv[0]);
        argc--;
        argv++;
    }

retry:
    if (*argv)
    {
        input_file = *argv;
        if (output_file)
            gen_out_name(output_file);
        else
            gen_out_name(input_file);
    }
    else
    {
        /*input_file = "stdin";*/
        /*(void)fprintf(stderr,"fdvm: Error: no source file specified\n");
          exit (1);
         */
        err_fatal("no source file specified", 3);
    }

    if (!source_form_by_option)
        free_form = SourceFormByName(input_file);

    if (output_file && !strcmp(input_file, output_file))
        err_fatal("Output file has the same name as source file", 334);

    if (argc > 2) {
        /*
          (void)fprintf(stderr,"fdvm: Error: illegal command line format\n");
          exit (1);
         */
        err_fatal("illegal command line format", 4);
    }
    /*  if(perf_analysis && dvm_debug)
        err_fatal("conflicting options -e and -d");
    */
    if (HPF_program && HPF) {
        (void)fprintf(stderr, "Warning: option -hpf%d is ignored\n", HPF_VERSION);
        HPF = 0;
    }
    if (free_form && extend_source) {
        (void)fprintf(stderr, "Warning: option -extend_source is ignored\n");
        extend_source= 0;
    } 
    language = ForSrc;
    yylonglines = 1;
    prflag = 1;
    cur_file = fi = (PTR_FILE)calloc(1, (unsigned)sizeof(struct file_obj));
    fi->lang = language;
    initialize();
    if ((OMP_program == 1) && (dvm_debug > 0)) {/*OMP*/
        debug_file = (char *)malloc((unsigned)(strlen(input_file) + 5));/*OMP*/
        sprintf(debug_file, "dbg_%s", input_file);/*OMP*/
        PrepareSourceFileForDebug(input_file, debug_file); /*OMP*/
        make_file_list(input_file);/*OMP*/
        if (dbginilex(copys(input_file), copys(debug_file))) { /*OMP*/
            err_fatal("Compiler bug (Error in inilex)", 0); /*OMP*/
        } /*OMP*/
    }/*OMP*/ else {
        make_file_list(input_file);

        if (inilex(copys(*argv ? *argv : ""))) {
            /*  (void)printf("Error in inilex\n");
             exit(1); */
            err_fatal("Compiler bug (Error in inilex)", 0);
            /*goto finish;*/
        }
    }/*OMP*/
    procinit();

    if (v_print) {
        (void)fprintf(stderr, "*****  Fortran DVM  %s  *****\n", COMPILER_VERSION);
        (void)fprintf(stderr, "\n<<<<<  Parsing  %s  >>>>>\n", input_file);
    }
    /* parsing */
    if ((k = yyparse())) {
        /*(void)printf("Bad parse, return code %d\n", k);*/
        (void)err("Compiler bug", 0);
        exit(1);
        /*goto finish;*/
    }
    if (parstate != OUTSIDE) {
        infname = input_file;
        err("Missing final end statement or unclosed construct", 8);
        exit(1);
        /*goto finish;*/
    }
    global_bfnd->filename = head_file; /*podd 18.04.99*/
    /*global_bfnd->filename = cur_thread_file;*/ /*podd 18.04.99*/
    if (errcnt) {
        (void)fprintf(stderr, "%d error(s)\n", errcnt);
        exit(1);

        if (fromfile) {
            int ans;

            (void)printf("Do you want to invoke editor? [y/n] ");
            while ((ans = getchar()) == '\n');
            if (ans == 'y' || ans == 'Y') {
                char *edtcmd;
                char cmd[50];

                edtcmd = getenv("FOREDIT");
                if (!edtcmd) edtcmd = defeditor;
                (void)sprintf(cmd, edtcmd, errline, *argv);
                (void)system(cmd);
                release_nodes();
                close_files();
                goto retry;
            }
            exit(1);
        }

    }
    /*
       if (dep) {
          (void)build_sets(0, global_bfnd,NULL,NULL,1);
          (void)build_sets(0, global_bfnd,NULL,NULL,2);
          (void)gendeps(global_bfnd);
          (void)relink(fi);
          printf("garbage collecting\n");

          if (garb_collect) (void)collect_garbage(fi);
       }
    */
    if (yylisting)
        (void)fprintf(outf, "%s\n", input_file);
    if (!global_bfnd->thread) /*null program (only global_bfnd is)*/
    {
        err_fatal("null program", 7);
    }

    if (HPF)
    {
        FILE *hpfout;
        if (no <= 1)
            output_file = outname;
        if ((hpfout = fopen(output_file, "w")) == NULL) {
            /* (void) fprintf(stderr,"Can't open file %s for write\n",output_file);*/
                /*UnparseFDVMProgram(stdout,cur_file);*/
            /* exit(1);*/
            errstr_fatal("Can't open file %s for write", output_file, 6);
        }
        else
        {
            if (v_print)
                (void)fprintf(stderr, "\n<<<<<  Generating HPF program  %s  >>>>>\n", output_file);
            errcnt = UnparseFDVMProgram(hpfout, cur_file);
            (void)fclose(hpfout);
            if (errcnt) {
                (void)fprintf(stderr, "%d error(s)\n", errcnt);
                exit(1);
            }
            else
                if (v_print)
                    (void)fprintf(stderr, "\n*****  Done  *****\n");
            exit(0);
        }
    }

    write_nodes(cur_file, outname);

#ifdef __SPF
    if (noProject == 0)
#endif
    {
        fproj = ad ? fopen(proj_name, "a") : fopen(proj_name, "w");
        if (fproj == NULL) {
            /* (void) fprintf(stderr,"Can't open file %s for write\n",proj_name);
                exit(1); */
            errstr_fatal("Can't open file %s for write", proj_name, 6);
        }
        (void)fprintf(fproj, "%s\n", outname);
        (void)fclose(fproj);
    }
    /* finish:*/
    exit(0);
}


void clf(p) 
FILEP *p;
{
   void fatal();
   
   if (p && *p && *p != stdout) {
      if (ferror(*p))
	 fatal("writing error", 329);
      (void)fclose(*p);
   }
   *p = (FILEP)NULL;
}


void
make_file_list(filename)
char *filename;
{
   char  *namep = (char *) malloc((unsigned)(strlen(filename)+1));
   PTR_FNAME this = (PTR_FNAME) malloc((unsigned)sizeof(struct file_name));
   PTR_FNAME p;
   
   (void)strcpy(namep, filename);
   this->id = fi->num_files = ++num_files;
   this->name = namep;
   this->next = (PTR_FNAME)NULL;
   if (head_file == (PTR_FNAME)NULL)
      fi->head_file = head_file = this;
   else {
      for (p = head_file; p->next != NULL; p = p->next)
	 ;
      p->next = this;
   }
   cur_thread_file = this;
}

void PrepareSourceFileForDebug (input_file, debug_file) /*OMP*/
char *input_file; /*OMP*/
char *debug_file; /*OMP*/
{ /*OMP*/
  FILE * pFile;
  long lSize;
  char sInclude[] = "\r\n      include 'dbg_init.h'";
  /*int count = 0;*/
  char * buffer;
  size_t result;
  pFile = fopen ("dbg_init.h", "r");
  if (pFile==NULL) {
    pFile = fopen ("dbg_init.h", "w");
    fprintf (pFile, "      subroutine DBG_Init_Handles ()\n");
    fprintf (pFile, "      include 'dbg_vars.h'\n");
    fprintf (pFile, "      end subroutine DBG_Init_Handles");

  }
  fclose (pFile);
  pFile = fopen ("dbg_vars.h", "r");
  if (pFile==NULL) {
    pFile = fopen ("dbg_vars.h", "w");
    fprintf (pFile, "      dimension idyn_mp(1)\n");
    fprintf (pFile, "      dimension istat_mp(1)\n");
    fprintf (pFile, "      common /DBG_STAT/ istat_mp\n");
    fprintf (pFile, "      common /DBG_DYN/ idyn_mp\n");
    fprintf (pFile, "C$OMP THREADPRIVATE (/DBG_DYN/)\n");
    fprintf (pFile, "      common /DBG_THREAD/ ithreadid\n");
    fprintf (pFile, "C$OMP THREADPRIVATE (/DBG_THREAD/)");
  }
  fclose (pFile);
  pFile = fopen (input_file, "rb" );
  if (pFile==NULL) {return;}
  /* obtain file size:*/
  fseek (pFile , 0 , SEEK_END);
  lSize = ftell (pFile);
  rewind (pFile);
  /* allocate memory to contain the whole file:*/
  buffer = (char*) malloc (sizeof(char)*(lSize + sizeof(sInclude)));
  if (buffer == NULL) {fprintf (stderr, "PrepareSourceFileForDebug: not engough memory!\n"); exit (1);}
  /* copy the file into the buffer:*/
  result = fread (buffer,1,lSize,pFile);
  /* the whole file is now loaded in the memory buffer. */ 
  fclose (pFile);
  pFile = fopen (debug_file, "wb" );
  if (pFile==NULL) {fprintf (stderr, "PrepareSourceFileForDebug: can not open %s file!\n", debug_file); exit (1);}
  strcpy (buffer+lSize, sInclude);
  result = fwrite (buffer,1,lSize + sizeof(sInclude),pFile);
  fclose (pFile);
  free (buffer);
} /*OMP*/

int DoDebugInclude (char *fname) {/*OMP*/
  FILE * pFile;
  long lSize;
  int count = 0;
  char * buffer;
  char * beg = NULL;
  size_t result;
  pFile = fopen (fname, "rb" );
  if (pFile==NULL) {return 0;}
  /* obtain file size:*/
  fseek (pFile , 0 , SEEK_END);
  lSize = ftell (pFile);
  rewind (pFile);
  /* allocate memory to contain the whole file:*/
  buffer = (char*) malloc (sizeof(char)*lSize);
  if (buffer == NULL) {fprintf (stderr, "DoDebugInclude: not engough memory!\n"); exit (1);}
  /* copy the file into the buffer:*/
  result = fread (buffer,1,lSize,pFile);
  /* the whole file is now loaded in the memory buffer. */ 
  fclose (pFile);
  beg = buffer;
  while (beg != NULL) {
     beg = strstr (beg, "call DBG_Get_Handle");
     if (beg != NULL) {
        beg++;     
        count++;
     }
  }  
  free (buffer);
  fprintf (stderr, "%d",count);
  return count;
}

