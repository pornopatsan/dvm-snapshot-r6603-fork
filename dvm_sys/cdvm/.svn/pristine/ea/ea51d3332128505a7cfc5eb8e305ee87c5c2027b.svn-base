/* CDVM_C.H ***************************** v 4.7 02.04.2007 **/
/*                                                          */
/*     Contents:                                            */
/*                                                          */
/* (1) MACROs, mapping DVM-directives onto RTL-calls        */
/* (2) stack for RTL-calls' parameters                      */
/* (3) auxiliary subroutines:                               */
/*     inssh_A, DVM_REMOTE_BUF                              */
/************************************************************/

#define DVMLINE(line) (DVM_FILE[0]=_SOURCE_,DVM_LINE[0]=line)

#include "dvmlib.h"

/*                                                          */
/* (1) MACROs, mapping DVM-directives onto RTL-calls        */
/*                                                          */



#define DVM_INIT(line,argn,args)  {DVMLINE(line);\
   rtl_init(0L,argn,args); \
   CDVM_BOTTOM=CDVM_ARG+100; CDVM_TOP=CDVM_BOTTOM;}

#define DVM_RETURN(line,r) {DVMLINE(line);\
       lexit_(DVM_A0(r));}

#define DVM_EXIT(r)  {DVMLINE(0); lexit_(DVM_A0(r));}



#undef NUMBER_OF_PROCESSORS
#define NUMBER_OF_PROCESSORS()\
   (DVM_PS=getps_(NULL),\
   getsiz_((ObjectRef*)&DVM_PS,DVM_0000))

#define DVM_PROCESSORS(line,proc,r,dims) {DVMLINE(line);\
   DVM_PS=getps_(NULL);\
   proc=psview_(&DVM_PS,\
             DVM1A(r), DVM##r##A dims, DVM_0000);\
   DVM_POP(r+1);}

#define DVM_TASK(id,n)\
   AMV_##id=crtamv_((AMRef*)DVM_A0((long)getam_()),\
           DVM_1111,DVM_A1(n), DVM_0000);

#define DVM_ONTO(line,ps,k,ls,hs) DVMLINE(line);\
   DVM_PS=ps;\
   DVM_PS=crtps_(&DVM_PS,\
       DVM##k##A ls, DVM##k##A hs, DVM_0000);\
   DVM_POP(2*k);

#define DVM_PSSIZE(dim)\
       getsiz_((ObjectRef*)&DVM_PS,(CDVM_ARG[dim+1]\
        = dim+1,CDVM_ARG+dim+1))-1
/*       getsiz_((ObjectRef*)DVM_PS,DVM_A0(dim))-1*/

#define DVM_MAP(line,task,ind,ps) DVMLINE(line);\
   ps; task[ind]=DVM_PS;\
   mapam_((AMRef*)DVM_A0((long)\
       /**/getamr_(&AMV_##task,DVM_A1(ind))),\
       &DVM_PS);

#define DVM_RUN(line,task,ind)\
   runam_((AMRef*)DVM_A0((long)\
       /**/getamr_(&AMV_##task,DVM_A1(ind))))

#define DVM_STOP(line)    DVMLINE(line); stopam_()

/**************** Data distribution and allocation */

/* malloc -> ... -> distr_() */

#define DVM_DISTRIBUTE(line,amv,ps,k,axs) {DVMLINE(line);\
   DVM_PS=ps; DVM_AMV=amv;\
   distr_( &DVM_AMV, &DVM_PS,\
       DVM1A(k), DVM##k##A axs, DVM_0000);\
   DVM_POP(k+1); }

/* REDISTRIBUTE -> ... -> redis_() */

#define DVM_REDISTRIBUTE(line,amv,ps,k,axs,new)\
   {DVMLINE(line);\
   DVM_PS=ps; /* DVM_AMV=amv;  91120*/\
   redis_(/*&DVM_AMV   91120*/(AMViewRef*) amv, &DVM_PS,\
       DVM1A(k), DVM##k##A axs, DVM_0000, DVM1A(new));\
   DVM_POP(k+2); }


/* GENBLOCK format -> ... -> genbli_() */

#define DVM_GENBLOCK(line,am,ps,k,gbs) DVMLINE(line);\
   DVM_AM=am? /*am : getam_();*/ getamv_((long*)am):DVM_AMV;\
   DVM_PS=ps? ps : getps_(NULL);\
   genbli_(&DVM_PS, &DVM_AM,\
       (AddrType*)DVM##k##A gbs, DVM_A0(k));\
   DVM_POP(k);

/* WGTBLOCK format -> ... -> setelw_() */

#define DVM_WGTBLOCK(line,am,ps,k,gbs,ns) DVMLINE(line);\
   DVM_AM=am? /*am : getam_();*/ getamv_((long*)am):DVM_AMV;\
   DVM_PS=ps? ps : getps_(NULL);\
   setelw_(&DVM_PS, &DVM_AM,\
       (AddrType*)DVM##k##A gbs, DVM##k##A ns, DVM_A0(k));\
   DVM_POP(k*2);

/* MULTBLOCK format -> ... -> blkdiv_() */

#define DVM_MULTBLOCK(line,amv,k,mbs) DVMLINE(line);\
   if(DVM_AMV!=(amv)) DVM_AMV=getamv_((long*)amv);\
   blkdiv_(&DVM_AMV, DVM##k##A mbs, DVM_A0(k));\
   DVM_POP(k);

/* malloc -> ... -> align_() */

#define DVM_ALIGN(line,arr,base,k,i,a,b)  {DVMLINE(line);\
   align_( arr, (PatternRef*)/*??: */ &(base),\
       DVM##k##A i, DVM##k##A a, DVM##k##A b);\
   DVM_POP(k*3); }

/* REALIGN -> ... -> realn_() */

#define DVM_REALIGN(line,arr,base,k,i,a,b,new)\
   {DVMLINE(line);\
   realn_(arr, (PatternRef*)/*??: &*/(base),\
       DVM##k##A i, DVM##k##A a, DVM##k##A b, DVM1A(new));\
   DVM_POP(k*3+1); }

/* malloc or CREATE_TEMPLATE -> ... -> crtamv_() */

#define DVM_CREATE_TEMPLATE(line,am,t,r,di) {DVMLINE(line);\
   if(am==0) DVM_AM=getam_();\
   else DVM_AM=am;\
   DVM_AMV=crtamv_( (AMRef*)DVM1A((long)DVM_AM),\
       DVM1A(r), DVM##r##A di, DVM_0000);\
   t=DVM_AMV;  DVM_POP(r+2); }

/* malloc -> ... -> ... crtda_() ... */

#define DVM_MALLOC(line,arr,r,len,dim,lw,hw,redis)\
   {DVMLINE(line);\
   crtda_( arr, DVM_0000,\
       NULL, DVM1A(r), DVM1A(len), DVM##r##A dim,\
       DVM_0000, DVM1A(redis),\
       DVM##r##A lw, DVM##r##A hw );\
   DVM_POP(r*3+3); }

/* free -> ... -> delda_() */

#define DVM_FREE(line,arr) (DVMLINE(line), delda_(arr));

/* <own assignement> ->  */
/*    if(DVM_ISLOCAL)    ->  tstelm_() */
/*      <statement>; */
/*    DVM_ENDLOCAL       ->  dskpbl_() -- debugger */

#define DVM_ISLOCAL(line,a,r,ind)  DVMLINE(line),\
       CDVM_RC=tstelm_(a, DVM##r##A ind),\
       DVM_POP(r), CDVM_RC

#define DVM_ENDLOCAL(line) DVMLINE(line); dskpbl_()


/**************** PARALLEL loop */

#define DVM_PARALLEL(line,n,r)\
   {long DVM_LO##n[r], DVM_HI##n[r], DVM_ST##n[r];\
   LoopRef DVM_LP##n;\
   DVMLINE(line);\
   DVM_LP##n=crtpl_(DVM_A0(r))

#define DVM_DO_ON(line,n,r,vs,ls,hs,ss,base,rb,is,as,bs)\
   DVMLINE(line);\
   mappl_(&DVM_LP##n, (PatternRef*) /*&*/(base),\
       DVM##rb##A is, DVM##rb##A as, DVM##rb##A bs,\
       (AddrType*)DVM##r##A vs,\
       DVM_1111,/* TypeArray */\
       DVM##r##A ls, DVM##r##A hs, DVM##r##A ss,\
       DVM_LO##n, DVM_HI##n, DVM_ST##n);\
   DVM_POP(r*4+rb*3);

#define DVM_DOPL(n) while(dopl_(&DVM_LP##n))

#define DVM_FOR(line,n,v,k,lh)\
   for(v=DVM_LO##n[k]; ((DVM_HI##n[k]-v)*DVM_ST##n[k])>=0;\
        v+=DVM_ST##n[k])

/**************** "accelerated" ********************/
#define DVM_FOR_1(line,n,v,k,lh)\
    int /*v,*/ DVM_HI=DVM_HI##n[k];\
    for(v=DVM_LO##n[k]; v<=DVM_HI; v+=1)
/**************************************************/

#define DVM_REDBLACK(line,n,v,k,e,lh)\
   for(v=DVM_LO##n[k]+(DVM_LO##n[k]+e)%2;\
       v<=DVM_HI##n[k]; v+=2)

#define DVM_END_PARALLEL(line, n)\
   DVMLINE(line); endpl_(&DVM_LP##n);}

/**************** TASK_REGION */

#define DVM_TASKREGION(line,no,task) \
   AMViewRef AMV_0; /*dummy for -s */\
   AMViewRef DVM_LP##no=AMV_##task;


/**************** SHADOWS & ACROSS*/

#define DVM_CREATE_SHADOW_GROUP(line,sg,shads)\
   {ShadowGroupRef DVM_SG;\
   DVMLINE(line);\
   if(sg!=0) delshg_(&sg);\
   DVM_SG=crtshg_(DVM_0000);\
   shads;\
   sg=DVM_SG;\
   }

#define DVM_SHADOWS(a,k,ls,hs,corner)\
   (inssh_(&DVM_SG, a,\
       DVM##k##A ls, DVM##k##A hs, DVM1A(corner)),\
   DVM_POP(2*k+1))

#define DVM_SHADOWSa(a,k,ls,hs,corner, n )\
   (inssh_A( &DVM_SG, (long*)a, DVM1A(n),\
       DVM##k##A ls, DVM##k##A hs, DVM1A(corner),\
       sizeof(a), sizeof(a[0]) ),\
   DVM_POP(2*k+2))

#define DVM_SHADOW_START(line,sg) DVMLINE(line); strtsh_(&sg)
#define DVM_SHADOW_WAIT(line,sg)  DVMLINE(line); waitsh_(&sg)

#define DVM_PAR_SHADOW_START(n,sg) exfrst_(&DVM_LP##n,&sg)
#define DVM_PAR_SHADOW_WAIT(n,sg)  imlast_(&DVM_LP##n,&sg)

#define DVM_SHADOW_RENEW(n,das)\
   {ShadowGroupRef DVM_SG=crtshg_(DVM_0000);  das;\
   strtsh_(&DVM_SG); waitsh_(&DVM_SG); delshg_(&DVM_SG); }


/*20.11 -- --для цикла с отрицательным шагом */
#define DVM_ACROSS(line,loopid,shads)\
   {ShadowGroupRef DVM_SHG;\
   int DVM_LorH;\
    int DVM_NegStep=DVM_ST##loopid[0/**/]<0;\
   DVMLINE(line);\
   DVM_SHG=crtshg_(DVM_0000);\
   DVM_LorH=1;  shads;\
   strtsh_(&DVM_SHG); waitsh_(&DVM_SHG);\
   delshg_(&DVM_SHG);\
   DVM_SHG=crtshg_(DVM_0000);\
   DVM_LorH=0;  shads;\
   recvsh_(&DVM_SHG); waitsh_(&DVM_SHG);

#define DVM_END_ACROSS(line) DVMLINE(line);\
   sendsh_(&DVM_SHG); waitsh_(&DVM_SHG);\
   delshg_(&DVM_SHG);}

#define DVM_ACROSS_SH(a,k,ls,hs,corner)\
   (inssh_(&DVM_SHG,a,\
    DVM_LorH!=DVM_NegStep ? DVM_0000 : DVM##k##A ls,\
    DVM_LorH!=DVM_NegStep ? DVM##k##A hs : DVM_0000,\
    DVM1A(corner)), DVM_POP(k+1) )

#define DVM_ACROSS_SHa(a,k,ls,hs,corner, n )\
   (inssh_A(&DVM_SHG, (long*)a, DVM1A(n),\
    DVM_LorH!=DVM_NegStep ? DVM_0000 : DVM##k##A ls,\
    DVM_LorH!=DVM_NegStep ? DVM##k##A hs : DVM_0000,\
    DVM1A(corner), sizeof(a), sizeof(a[0]) ),\
    DVM_POP(k+2) )

#define DVM_ACROSS_IN(line,loopid,shads)\
   {ShadowGroupRef DVM_SHG, DVM_SHG1, DVM_SHG2;\
   int DVM_LorH;\
   int DVM_NegStep=DVM_ST##loopid[0/**/]<0;\
   double DVM_PipePar=0.;\
   DVMLINE(line);\
   DVM_SHG=crtshg_(DVM_0000);\
   DVM_LorH=1;  shads; DVM_SHG1=DVM_SHG;\
   DVM_SHG=crtshg_(DVM_0000);\
   DVM_LorH=0;  shads; DVM_SHG2=DVM_SHG;\
    across_(DVM_0000,&DVM_SHG1,&DVM_SHG2,&DVM_PipePar);\
    }

#define DVM_ACROSS_OUT(line,loopid,level,shads,slice)\
   {ShadowGroupRef DVM_SG, DVM_SHG1, DVM_SHG2;\
   int DVM_LorH;\
   int DVM_NegStep=DVM_ST##loopid[0/**/]<0;\
   DVMLINE(line);\
   DVM_SHG=crtshg_(DVM_0000);\
   DVM_LorH=1;  shads; DVM_SHG1=DVM_SHG;\
   DVM_SHG=crtshg_(DVM_0000);\
   DVM_LorH=0;  shads; DVM_SHG2=DVM_SHG;\
    across_(DVM_1111,&DVM_SHG1,&DVM_SHG2,DVM_0000);\
   delshg_(&DVM_SHG1);\
   delshg_(&DVM_SHG2);\
    }
    /********************************** old
#define DVM_ACROSS(line,loopid,shads)\
   {ShadowGroupRef DVM_LSG;\
   ShadowGroupRef DVM_HSG;\
   int DVM_LorH;\
   DVMLINE(line);\
   DVM_LSG=crtshg_(DVM_0000);\
   DVM_HSG=crtshg_(DVM_0000);\
   DVM_LorH=1;  shads;\
   strtsh_(&DVM_HSG); waitsh_(&DVM_HSG);\
   delshg_(&DVM_HSG);\
   DVM_LorH=0;  shads;\
   recvsh_(&DVM_LSG); waitsh_(&DVM_LSG);

#define DVM_END_ACROSS(line) DVMLINE(line);\
   sendsh_(&DVM_LSG); waitsh_(&DVM_LSG);\
   delshg_(&DVM_LSG);}

#define DVM_ACROSS_SH(a,k,ls,hs,corner)\
   ((DVM_LorH ?\
   inssh_(&DVM_HSG,a,DVM_0000,DVM##k##A hs,DVM1A(corner)):\
   inssh_(&DVM_LSG,a,DVM##k##A ls,DVM_0000,DVM1A(corner))\
   ), DVM_POP(k+1) )

#define DVM_ACROSS_SHa(a,k,ls,hs,corner, n )\
   (DVM_LorH? inssh_A( &DVM_LSG, (long*)a, DVM1A(n),\
           DVM##k##A ls, DVM_0000, DVM1A(corner),\
           sizeof(a), sizeof(a[0]) ):\
       inssh_A( &DVM_HSG, (long*)a, DVM1A(n),\
           DVM_0000, DVM##k##A hs, DVM1A(corner),\
           sizeof(a), sizeof(a[0]) ),\
   DVM_POP(k+2) )
    ***************************/
#define DVM_PIPE(line,loopid,a,t,n)\
    {LoopRef * DVM_l=&DVM_LP##loopid;\
    long DVM_a=(long)&(a[0]);\
    long DVM_t=t;\
    long DVM_n=n/*sizeof(a)/sizeof(*a)*/;\
    DVMLINE(line);\
    acrecv_(DVM_l,&DVM_a,&DVM_t,&DVM_n);\
    DVM_POP(2);

#define DVM_END_PIPE(line) DVMLINE(line);\
    acsend_(DVM_l,&DVM_a,&DVM_t, &DVM_n);\
    DVM_POP(2);\
    }

/**************** REDUCTION */


#define DVM_RVAR(f,v,t,l,s,d)\
   (DVM_OPT==0)? \
    ( CDVM_RC=(long)crtred_(DVM1A(f), /*(AddrType)*/&(v),\
       DVM1A(t), DVM1A(l),DVM_0000, DVM_0000, DVM_1111),\
   DVM_POP(3), DVM2A(CDVM_RC,-1) ): 0  , \
   (DVM_OPT==1) ? \
    ( dinsrd_((ObjectRef*) &DEB_DVM_RG,\
       DVM1A(f), /*(AddrType)*/&(v),\
       DVM1A(t), DVM1A(l),DVM_0000, DVM_0000, DVM_1111),\
    DVM_POP(3), 0 ) : 0

#define DVM_RLOC(f,v,t,l,loc,loctp,s,d)\
   (DVM_OPT==0) ? \
    ( CDVM_RC=(long)crtred_(DVM1A(f), /*(AddrType)*/&(v),\
        DVM1A(t),DVM1A(l),\
        &(loc)/*!*/,DVM1A(sizeof(loc)), DVM_1111),\
   lindtp_((RedRef*)&CDVM_RC,DVM1A(loctp)),\
   DVM_POP(5), DVM2A(CDVM_RC,-1) ) : 0 ,\
   DVM_OPT==1 ? \
    ( dinsrd_((ObjectRef*) &DEB_DVM_RG,\
       DVM1A(f), /*(AddrType)*/&(v),\
       DVM1A(t), DVM1A(l),\
        &(loc)/*!*/,DVM1A(sizeof(loc)), DVM1A(loctp)),\
    DVM_POP(5), 0 ) : 0

#define DVM_INSERT_RV()\
/* while(CDVM_TOP!=CDVM_BOTTOM) {*/  \
/*printf("[0]=%x [1]=%x \n",CDVM_TOP[0],CDVM_TOP[1]);*/ \
   while(CDVM_TOP[1]==-1) {\
   insred_( (RedGroupRef*) &DVM_RG,(RedRef*)CDVM_TOP,\
         &DVM_PSSpace, DVM_1111);\
     DVM_POP(2);}

#define DVM_REDUCTION(line,loopid,rvs,s,d)\
   { RedGroupRef DVM_RG;\
    ObjectRef DEB_DVM_RG;/**/\
    int DVM_OPT;\
   LoopRef DVM_LP0=0;/* dummy for -s */\
   PSSpaceRef DVM_PSSpace=DVM_LP##loopid;\
   DVMLINE(line);\
   if(!s /**/) { DVM_RG=crtrg_(DVM_1111, DVM_1111);\
        DVM_OPT=0; (void)rvs; \
        /**/DVM_INSERT_RV();}\
   if(d) { DEB_DVM_RG=dcrtrg_();\
        DVM_OPT=1; (void)rvs;\
        dsavrg_(&DEB_DVM_RG);}

#define DVM_END_REDUCTION(line,s,d)   {DVMLINE(line);\
    if(!s /**/)\
    {strtrd_(&DVM_RG); waitrd_(&DVM_RG); delrg_(&DVM_RG);}\
    if(d)\
    {dclcrg_(&DEB_DVM_RG); ddelrg_(&DEB_DVM_RG);}\
    }\
   }

#define DVM_CREATE_RG(line,rg,rvs,s,d) DVMLINE(line);\
   {ObjectRef DEB_DVM_RG=0;\
    int DVM_OPT;\
   if( (!s /**/)&& rg==0) rg=crtrg_(DVM_1111,DVM_1111);\
   if(d && DEB_##rg==0)\
     {DEB_##rg=dcrtrg_(); DEB_DVM_RG=DEB_##rg;}\
   if(!s) {DVM_OPT=0; (void)rvs;}\
   if(d) {DVM_OPT=1; (void)rvs;}\
    }

#define DVM_REDUCTION20(line, loopid, rg /*,rvs*/, s,d)\
   { RedGroupRef DVM_RG=rg;\
   LoopRef DVM_LP0=0;/* dummy for -s */\
   PSSpaceRef DVM_PSSpace=DVM_LP##loopid;\
   DVMLINE(line);\
    if(!s /**/) {/*(void)rvs;*/ DVM_INSERT_RV(); rg=DVM_RG;}\
    if(d) {dsavrg_( &(DEB_##rg) );}\
    }

#define DVM_REDUCTION_START(line,rg,s,d) DVMLINE(line);\
   if(!s /**/) {strtrd_((RedGroupRef*) &rg);}

#define DVM_REDUCTION_WAIT(line,rg,s,d)  DVMLINE(line);\
   if(!s /**/) {waitrd_((RedGroupRef*) &rg);\
        delrg_((RedGroupRef*) &rg); rg=0;}\
   if(d) {dclcrg_((ObjectRef*) &(DEB_##rg));\
       ddelrg_((ObjectRef*) &(DEB_##rg)); DEB_##rg=0;}


/*#define DVM_DEL_REDGROUP(line)   {DVMLINE(line);*/\
/*   delrg_(&DVM_RG);} }  */


/**************** REMOTE_ACCESS */

#define DVM_BLOCK_BEG(line) DVMLINE(line); begbl_()
#define DVM_BLOCK_END(line) DVMLINE(line); endbl_()

#define DVM_PREFETCH(line,rg) DVMLINE(line);\
   if(rg==0) {RMG_##rg=0;}\
   else {loadbg_(&rg, DVM_1111); RMG_##rg=1;}

#define DVM_RESET(line,rg) DVMLINE(line);\
   if(rg) delbg_(&rg); rg=0; RMG_##rg=0;

#define DVM_REMOTE20G(line,n,rg,arr,buf,k,is,as,bs)\
   DVMLINE(line);\
   if(RMG_##rg==1) {waitbg_(&rg); RMG_##rg=2;}\
   if(RMG_##rg==0)\
   {crtrbl_(arr, buf, NULL, DVM_1111/*static*/, &DVM_LP##n,\
           DVM##k##A is, DVM##k##A as, DVM##k##A bs);\
   DVM_POP(3*k);\
   loadrb_(buf, DVM_1111); waitrb_(buf);\
   if(rg==0) {rg=crtbg_(DVM_1111, DVM_1111);}\
   insrb_(&rg, buf);}

#define DVM_REMOTE20(line,n,arr,buf,k,is,as,bs)\
   DVMLINE(line);\
   {/* static long buf[2*k+2]; */  \
   crtrbl_(arr, buf, NULL, DVM_0000/*automatic*/,\
           &DVM_LP##n /* NULL */ ,\
           DVM##k##A is /* DVM_n111 */ ,\
           DVM##k##A as, DVM##k##A bs);\
   DVM_POP(3*k);\
   loadrb_(buf, DVM_1111); waitrb_(buf);}

/**************** INDIRECT_ACCESS */

/**************** PPPA's calls */

#define DVM_BPLOOP(line,n)  DVMLINE(line);\
       bploop_(DVM_A0(n));

#define DVM_ENLOOP(line,n)   DVMLINE(line);\
       enloop_(DVM_A0(n), DVM_A1(line));

#define DVM_BSLOOP(line,n)     DVMLINE(line);\
       bsloop_(DVM_A0(n));

#define DVM_BINTER(line,n,v)   DVMLINE(line);\
       binter_(DVM_A0(n),DVM_A1(v) )

#define DVM_EINTER(line,n)   DVMLINE(line);\
       einter_(DVM_A0(n), DVM_A1(line) )




/**************** DEBUGGER's calls */

#define DVM_PLOOP(line,n,r,ls,hs,ss)  DVMLINE(line);\
   dbegpl_(DVM1A(r), DVM1A(n),\
   DVM##r##A ls, DVM##r##A hs, DVM##r##A ss);\
   DVM_POP(3*r+2)

#define DVM_SLOOP(line,n)      DVMLINE(line);\
       dbegsl_(DVM_A0(n));

#define DVM_ENDLOOP(line,n) DVMLINE(line);\
       dendl_(DVM_A0(n), (unsigned long *)DVM_A1(line));

#define DVM_ITER(line,r,vars,tps)  DVMLINE(line);\
       diter_((AddrType*)DVM##r##A vars,\
           DVM##r##A tps\
/*0522      NULL */ /* TypeArray == now all are ints */\
        ); DVM_POP(r*2)

#define DVM_BTASK(line,n) DVMLINE(line);\
       dbegtr_(DVM_A0(n));

#define DVM_ETASK(line,n) DVMLINE(line);\
       dendl_(DVM_A0(n), (unsigned long *)DVM_A1(line));

#define DVM_NTASK(line,ind) DVMLINE(line);\
       diter_((AddrType*)DVM_A1((long)DVM_A0(ind)),\
       DVM_0000);



#define DVM_LDV(line,type,rt,var,base)\
   *(type*) ( DVMLINE(line),\
   ( DVM3A((long)&(var),rt, (long)(base)) && \
   dldv_(&CDVM_TOP[1], (AddrType *)&CDVM_TOP[0],\
  &CDVM_TOP[2] /* (long *) base */ , #var, -1) && 0) ? 0 :\
   DVM_POPr(3))

#define DVM_STV(line,type,rt,var,base)\
   (DVMLINE(line),\
    DVM3A((long)&(var), rt, (long)(base)),\
   dprstv_(&CDVM_TOP[1], (AddrType *)&CDVM_TOP[0],\
  &CDVM_TOP[2] /* (long *) base */ , #var, -1),\
   DVM_POP(3),  dstv_() )

#define DVM_STVN(line,type,rt,var,base)\
   {int I;\
   DVMLINE(line);\
   for(I=0; I<sizeof(var)/sizeof(type);I++)\
   {DVM3A((long)((type*)var+I), rt, (long)(base));\
   dprstv_(&CDVM_TOP[1], (AddrType *)&CDVM_TOP[0],\
  &CDVM_TOP[2] /* (long *) base */ , #var, -1),\
   DVM_POP(3);  dstv_();\
   }}


#define DVM_STVA(line,type,rt,var,base,rhs)\
   (DVMLINE(line),\
    DVM3A((long)&(var), rt, (long)(base)),\
   dprstv_(&CDVM_TOP[1], (AddrType *)&CDVM_TOP[0],\
  &CDVM_TOP[2] /* (long *) base */ , #var, -1),\
   (*(type *)(CDVM_TOP[0]) = rhs), \
   DVM_POP(3),  dstv_())

#define DVM_LD_ST(line,type,rt,var,base) var
/*    *((type *) ((DVM1A((long)&(var), rt) &&\*/
/*    dstv_(&CDVM_TOP[1], (AddrType *)&CDVM_TOP[0],\*/
/*                (long *) base) && 0) ? 0 : \*/
/*    DVM_POP(2) //1125// ))*/

#define DVM_REGARR(line,r,type,arr,size)\
    DVMLINE(line);\
    drarr_(DVM1A(r), DVM1A(type), DVM1A((long)arr), \
       DVM##r##A size, #arr /* #size */, -1);   \
    DVM_POP( (r) + 3 );


#define DVM_BARRIER(line) \
    DVMLINE(line); bsynch_();


#ifdef MAIN_IS_HERE


/************************************************************/
/* (2) stack for RTL-calls' parameters                      */
/*                                                          */
/*      CDVM_ARG[0] ==  undef                               */
/*      ...             ...                                 */
/*      CDVM_TOP[0] ==  x1    current call:                 */
/*                      ...      xxx(x1,...xn)              */
/*      CDVM_TOP[n-1]   xn                                  */
/*                      ...   outer calls:                  */
/*                      ym       yyy(..., xxx(),... ym)     */
/*                      ...   so on                         */
/*      CDVM_ARG[100]   zk    bottom                        */
/*                                                          */

long CDVM_ARG[128];
long *CDVM_BOTTOM = &CDVM_ARG[100];
long *CDVM_TOP = &CDVM_ARG[100];
long CDVM_RC;

void DVM_POP(int n)
   { CDVM_TOP +=n; }

void *DVM_POPr(int n)
   { CDVM_TOP+=(n); return (void*)CDVM_TOP[-(n)];}

long *DVM0A(void)   {return CDVM_TOP;}

long *DVM1A(long x1)
{   *--CDVM_TOP =x1;
   return CDVM_TOP;
}

long *DVM2A(long x1,long x2)
{   *--CDVM_TOP =x2;   /* TOP[1] */
   *--CDVM_TOP =x1;    /* TOP[0] */
   return CDVM_TOP;
}

long *DVM3A(long x1,long x2,long x3)
{   *--CDVM_TOP =x3;
   *--CDVM_TOP =x2;
   *--CDVM_TOP =x1;
   return CDVM_TOP;
}
long *DVM4A(long x1,long x2,long x3,long x4)
{   *--CDVM_TOP =x4;
   *--CDVM_TOP =x3;
   *--CDVM_TOP =x2;
   *--CDVM_TOP =x1;
   return CDVM_TOP;
}

long *DVM5A(long x1,long x2,long x3,long x4,long x5)
{   *--CDVM_TOP =x5;
   *--CDVM_TOP =x4;
   *--CDVM_TOP =x3;
   *--CDVM_TOP =x2;
   *--CDVM_TOP =x1;
   return CDVM_TOP;
}

long *DVM6A(long x1,long x2,long x3,long x4,long x5,long x6)
{   *--CDVM_TOP =x6;
   *--CDVM_TOP =x5;
   *--CDVM_TOP =x4;
   *--CDVM_TOP =x3;
   *--CDVM_TOP =x2;
   *--CDVM_TOP =x1;
   return CDVM_TOP;
}

long *DVM7A(long x1,long x2,long x3,
       long x4,long x5,long x6, long x7)
{   *--CDVM_TOP =x7;
   *--CDVM_TOP =x6;
   *--CDVM_TOP =x5;
   *--CDVM_TOP =x4;
   *--CDVM_TOP =x3;
   *--CDVM_TOP =x2;
   *--CDVM_TOP =x1;
   return CDVM_TOP;
}

void * DVM_ldv(int line, int rt,
    long vaddr, long * base, char * vname)
{
    DVMLINE(line);
    DVM2A(vaddr,rt);
    dldv_(&CDVM_TOP[1], (AddrType *)&CDVM_TOP[0],
         base, vname, -1);
   return (void*)DVM_POPr(2);
}


/************************************************************/
/* (3) auxiliary subroutines                                */
/*                                                          */
/* inssh_A                                                  */
/*  -- insert several (n) arrays into a SHADOW_GROUP        */
/*     by an array of pointers to handlers                  */

int inssh_A(ShadowGroupRef * SGRp, long * A,
   long * n,                                 /*1124*/
   long * LWa, long * HWa, long* C, int sa, int s0)
{   int i;
   long * carr;

   for(i=0; i< (*n!=0 ? *n : sa/s0) ; i++)   /*1124*/
       {carr= s0!=sizeof(long*)? &A[i*s0/sizeof(long*)]
                   : (long*)A[i];
       inssh_(SGRp,(long*)carr,LWa,HWa,C);
       }
   return 0;
}


long DVM_0000[10]={0,0,0,0,0,0,0,0,0,0};
long DVM_1111[10]={1,1,1,1,1,1,1,1,1,1};
long DVM_1234[10]={1,2,3,4,5,6,7,8,9,10};
long DVM_n111[10]={-1,-1,-1,-1,-1,-1,-1,-1,-1,-1};
PSRef     DVM_PS;
AMRef     DVM_AM;
AMViewRef DVM_AMV;

/*                                                          */
/* DVM_REMOTE_BUF                                           */
/*  -- create REMOTE_ACCESS buffer         (rank<=4)        */

void DVM_REMOTE_BUF(long * to, long rank, long * from,
               long i0, long i1, long i2, long i3)
   {
   int i;
   long brank=0;
   AddrType  RmBufAddr;                /*0918*/
   long DVM_2[10];
   long elem_len;
   long DVM_1[10];

/* Create array of extents. */
   DVM_1[0]=i0;
   DVM_1[1]=i1;
   DVM_1[2]=i2;
   DVM_1[3]=i3;

/* Get element size */
   for(i=0, brank=0; i<rank; i++ )
   {long iL=i+1;
   if(DVM_1[i]==-1)
           DVM_2[brank++]=getsiz_((ObjectRef*)from,&iL);
   }
   if(brank==0) {brank=1; DVM_2[0]=1;}
   elem_len=getlen_(from);

   DVM_AM=getam_();
   DVM_AMV=crtamv_((AMRef*)&DVM_AM, &brank, DVM_2, DVM_0000);

   distr_(&DVM_AMV, NULL, DVM_0000, DVM_0000, DVM_0000);

   crtda_(to, DVM_0000, NULL, &brank, &elem_len,
         DVM_2, DVM_0000, DVM_0000, DVM_0000, DVM_0000);

   align_(to, (PatternRef*)&DVM_AMV,
          DVM_1234, DVM_1111, DVM_0000);

  /* For every dimension:     */
  /*  source (-1,-1,1)   -> target (-1,0,1),   */
  /*  source (i,i,1)     -> target (-1,0,1)    */

   arrcpy_(from,DVM_1,DVM_1,DVM_1111,
           to, DVM_n111,DVM_0000,DVM_1111, DVM_0000);

   RmBufAddr=(AddrType)to;                  /*0918*/
   drmbuf_(from,&RmBufAddr, &rank, DVM_1);
   }

#else
/* It is not a main file.    */
/* Generate prototypes only. */

extern long CDVM_ARG[128], *CDVM_BOTTOM, *CDVM_TOP, CDVM_RC;
extern long DVM_0000[10];
extern long DVM_1111[10];
extern long DVM_1234[10];
extern long DVM_n111[10];
extern PSRef     DVM_PS;
extern AMRef     DVM_AM;
extern AMViewRef DVM_AMV;

void DVM_POP(int);
long *DVM0A(void);
long *DVM1A(long);
long *DVM2A(long,long);
long *DVM3A(long,long,long);
long *DVM4A(long,long,long,long);
long *DVM5A(long,long,long,long,long);
long *DVM6A(long,long,long,long,long,long);
long *DVM7A(long,long,long,long,long,long,long);

int inssh_A(ShadowGroupRef * SGRp, long * A,
   long * n,                               /*1124*/
   long * LWa, long * HWa, long* C, int sa, int s0);

void DVM_REMOTE_BUF(long * to, long rank, long * from,
               long i0, long i1, long i2, long i3);
#endif

#define DVM_A0(x) (CDVM_ARG[0]=x,CDVM_ARG)
#define DVM_A1(x) (CDVM_ARG[1]=x,CDVM_ARG+1)
#define DVM_A2(x) (CDVM_ARG[2]=x,CDVM_ARG+2)

/*1120*/
/******* "accelerated" access macros ***********************/


#define DVMda1(H, Type, I)\
        DAElm1(H, Type, I)
#define DVMda2(H, Type, I1, I2)\
        DAElm2(H, Type, I1, I2)
#define DVMda3(H, Type, I1, I2, I3)\
        DAElm3(H, Type, I1, I2, I3)
#define DVMda4(H, Type, I1, I2, I3, I4)\
        DAElm4(H, Type, I1, I2, I3, I4)
#define DVMda5(H, Type, I1, I2, I3, I4, I5)\
        DAElm5(H, Type, I1, I2, I3, I4, I5)
#define DVMda6(H, Type, I1, I2, I3, I4, I5, I6)\
        DAElm6(H, Type, I1, I2, I3, I4, I5, I6)

/* 02.2001 *************************************************/

#define DVM_COPY(line, v, f,l,s, fa, fe, ta, te)\
{long DVM_ff, DVM_fl, DVM_fs,\
    DVM_tf, DVM_tl, DVM_ts, DVM_reg;\
    DVMLINE(line);\
    v=f;  DVM_ff=fe;  DVM_tf=te;\
    v=l;  DVM_fl=fe;  DVM_tl=te;\
    v=l+s; DVM_fs=(fe)-DVM_fl; DVM_ts=(te)-DVM_tl;\
    DVM_reg=0;\
arrcpy_(fa, &DVM_ff, &DVM_fl, &DVM_fs,\
        ta, &DVM_tf, &DVM_tl, &DVM_ts, &DVM_reg);\
}

#define DVM_COPY_START(line, v, f,l,s, fa, fe, ta, te, flag)\
{long DVM_ff, DVM_fl, DVM_fs,\
    DVM_tf, DVM_tl, DVM_ts, DVM_reg;\
    DVMLINE(line);\
    v=f;  DVM_ff=fe;  DVM_tf=te;\
    v=l;  DVM_fl=fe;  DVM_tl=te;\
    v=l+s; DVM_fs=(fe)-DVM_fl; DVM_ts=(te)-DVM_tl;\
    DVM_reg=0;\
aarrcp_(fa, &DVM_ff, &DVM_fl, &DVM_fs,\
        ta, &DVM_tf, &DVM_tl, &DVM_ts,\
         &DVM_reg, (AddrType*)(flag));\
}

#define DVM_COPY_WAIT(line,flag) DVMLINE(line);\
    waitcp_((AddrType*)(flag));

#define DVM_COPYr(line,r,fs,ls,ss,fa,fr,fes,ta,tr,tes)\
{long *DVM_ff, *DVM_fl, *DVM_fs,\
    *DVM_tf, *DVM_tl, *DVM_ts, DVM_reg;\
    int DVM_i;\
    DVMLINE(line);\
    fs; /* (v1=first1, ...) */\
    DVM_ff = DVM##fr##A fes;\
    DVM_tf = DVM##tr##A tes;\
    ls; /* (v1=last1, ...) */\
    DVM_fl = DVM##fr##A fes;\
    DVM_tl = DVM##tr##A tes;\
    ss; /* (v1+=step1, ...) */\
    DVM_fs=DVM##fr##A fes ;\
    DVM_ts=DVM##tr##A tes ;\
    for(DVM_i=0; DVM_i<fr; DVM_i++)\
         DVM_fs[DVM_i]-=DVM_fl[DVM_i];\
    for(DVM_i=0; DVM_i<tr; DVM_i++)\
         DVM_ts[DVM_i]-=DVM_tl[DVM_i];\
    DVM_reg=0;\
    arrcpy_(fa, DVM_ff, DVM_fl, DVM_fs,\
        ta, DVM_tf, DVM_tl, DVM_ts, &DVM_reg);\
    DVM_POP(3*(fr+tr));\
}

#define DVM_COPY_STARTr(line,r,fs,ls,ss,\
        fa,fr,fes,ta,tr,tes,flag)\
{long *DVM_ff, *DVM_fl, *DVM_fs,\
    *DVM_tf, *DVM_tl, *DVM_ts, DVM_reg;\
    int DVM_i;\
    DVMLINE(line);\
    fs; /* (v1=first1, ...) */\
    DVM_ff = DVM##fr##A fes;\
    DVM_tf = DVM##tr##A tes;\
    ls; /* (v1=last1, ...) */\
    DVM_fl = DVM##fr##A fes;\
    DVM_tl = DVM##tr##A tes;\
    ss; /* (v1+=step1, ...) */\
    DVM_fs=DVM##fr##A fes ;\
    DVM_ts=DVM##tr##A tes ;\
    for(DVM_i=0; DVM_i<fr; DVM_i++)\
         DVM_fs[DVM_i]-=DVM_fl[DVM_i];\
    for(DVM_i=0; DVM_i<tr; DVM_i++)\
         DVM_ts[DVM_i]-=DVM_tl[DVM_i];\
    DVM_reg=0;\
    aarrcp_(fa, DVM_ff, DVM_fl, DVM_fs,\
        ta, DVM_tf, DVM_tl, DVM_ts, &DVM_reg,\
        (AddrType*) (flag) );\
    DVM_POP(3*(fr+tr));\
}
/* End of CDVM_C.H ******************************************/

