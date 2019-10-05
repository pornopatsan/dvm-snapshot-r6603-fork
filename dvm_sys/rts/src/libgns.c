#ifndef  _LIBGNS_C_
#define  _LIBGNS_C_
/*****************/    /*E0000*/

/* #include <sysLib.h> */    /*E0001*/

#define LINIT 4
#define LBUF 256

/* #define idtask TASKID */    /*e0002*/

#define idtask int

int my_number;

/*
int  init_number;
*/    /*e0003*/

int  num_proc;
int  num_proc_run = 1;  /* running processes */    /*e0004*/
char buf[LBUF];
int  err = 0;


#define s_read(x,y,z)	err = r_read( (x), (y), (z));\
			if(err != 0)\
                          eprintf(__FILE__,__LINE__,\
                          "*** RTL err 214.000: r_read rc = %d\n",err);\
			w_read( (x) )

#define s_write(x,y,z)	err = r_write( (x), (y), (z));\
			if(err != 0)\
                           eprintf(__FILE__,__LINE__,\
                           "*** RTL err 214.001: r_write rc = %d, "\
                           "addr=%d, buf=%p, len=%d\n",\
                           err, (x), (y), (z));\
			w_write( (x) )


void  sleep(float  a)
{
      return;
}


extern int rf_create( void );
extern int sysProcNumGet( void );
extern sysProcTotalGet( void );


void  gns_init(int c, char *a)
{
        /* init_number = 0; */    /*e0005*/

#ifndef _i860_ROU_
	err = rf_create();

        if(!err)
        {  fprintf( stderr, "rf_create error\n" );
           exit(1);
	}

	my_number  = sysProcNumGet();
	num_proc = sysProcTotalGet();
#else
	my_number = atoi(dvm_argv[1]);
	num_proc = atoi(dvm_argv[2]);
#endif

	if(my_number != 0)
        {  /* Slaves */    /*e0006*/

           s_read(0, buf, LINIT);

           if(buf[0] == 0)
              exit(1);

           s_write(0, buf, LINIT);
	}
}



int  gns_newtask(char *name, int nproc, int *vproc, idtask *vtaskid)
{
        int  i;

	for(i=0; i < nproc; i++)
        {  if(RouterNewTaskPrint && SysInfoPrint)
              pprintf(0, "*** RTL: router newtask %d\n", vproc[i]);

           s_write(vproc[i], name, LINIT);
           s_read(vproc[i], buf, LINIT);
           vtaskid[i] = (idtask)vproc[i];
	}

/*	for(i=nproc+1; i < num_proc; i++)
        {  buf[0] = 0;
           s_write(i, buf, LINIT);
	}
*/    /*e0007*/

        num_proc_run = nproc+1;

        return  nproc;
}



idtask  gns_mytaskid(void)
{
        return  my_number;
}



idtask  gns_master(void)
{
        return  0;
}



void  gns_exit(int  st)
{
        int  i;

        /* printf("libgns: beg exit, num_proc_run=%d, num_proc=%d\n",
                  num_proc_run, num_proc); */    /*E0008*/

	if(my_number != 0)
        {  s_write(0, &st, sizeof(int));
           exit(st);
	}

        for(i=1; i < num_proc_run; i++)
        {  s_read(i, buf, sizeof(int));
	}

        for(i=num_proc_run; i < num_proc; i++)
        {  if(RouterKillPrint && SysInfoPrint)
              pprintf(0, "*** RTL: router kill %d\n", i);

           buf[0] = 0;
           s_write(i, buf, LINIT);
	}

        /* printf("libgns: end exit\n"); */    /*E0009*/

        /* Terminate_i860(); */    /*e0010*/
        /*  exit(st); */    /*e0011*/
}



void  gns_abort(int st)
{
;
}



int  gns_send(idtask did, void *buf, int l)
{
	s_write((int)did, buf, l);
        return  l;
}



int  gns_senda(idtask did, int tag, void *buf, int l)
{
   eprintf(__FILE__,__LINE__,
     "*** RTL fatal err 214.002: function gns_senda does not exist\n");
   return  0;
}



int  gns_sendnw(idtask did, long *flag, void *buf, int l)
{
        int  err;

	*flag = (int)did;
	err = r_write((int)did, buf, l);

	if(err)
           err = -err;

	return  err;
}



int  gns_receive(idtask wid, idtask *sid, void *buf, int l)
{
	s_read((int)wid, buf, l);
	*sid = wid;
        return  l;
}



int  gns_receivea(idtask wid, idtask *sid, int tag, void *buf, int l)
{
   eprintf(__FILE__,__LINE__,
   "*** RTL fatal err 214.003: function gns_receivea does not exist\n");
   return  0;
}



int  gns_receivenw(idtask wid, idtask *sid, long *flag, int *nb,
                   void *buf, int l)
{
        int  err;

	*flag = (int)wid | 0x1000;
	*sid = wid;
	*nb = l;
	err = r_read((int)wid, buf, l);

        if(err)
           err = -err;

        return  err;
}



/*
int  gns_testtag(int  tag)
{
}



int  gns_testmsg(int tag, idtask id)
{
}
*/    /*e0012*/



int  gns_testflag(long  *flag)
{
        int  err;

        if(*flag & 0x1000)
           err = t_read(*flag & 0xfff);
        else
           err = t_write(*flag);

        if(err == 1)
           *flag = 0;

/*      if(err == 0)
           eprintf(__FILE__,__LINE__,
                   "*** RTL fatal err 214.004: t_read/t_write:  "
                   "proc %d does not exist\n",
                   (*flag & 0xfff));
*/    /*e0013*/

	return  err;
}



void  gns_msgdone(long  *flag)
{
	int err;

	if(*flag & 0x1000)
           err = w_read(*flag & 0xfff);
        else
           err = w_write(*flag);

/*      if(err == 0)
           eprintf(__FILE__,__LINE__,
                   "*** RTL fatal err 214.004: t_read/t_write: "
                   "proc %d does not exist\n",
                   (*flag & 0xfff));
*/    /*e0014*/

	*flag = err;              	
}


#endif   /*  _LIBGNS_C_  */    /*e0015*/
