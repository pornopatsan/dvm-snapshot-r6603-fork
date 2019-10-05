#ifndef _STATEVNT_C_
#define _STATEVNT_C_
/******************/    /*E0000*/

#include "strall.h" 
 
void  statevnt(void)
{
  IsStat[ call_dvm_fopen          ] = INOUT;
  IsStat[ ret_dvm_fopen           ] = INOUT+QCOLLECT;
  IsStat[ call_dvm_fclose         ] = INOUT;
  IsStat[ ret_dvm_fclose          ] = INOUT+QCOLLECT;
  IsStat[ call_dvm_void_vfprintf  ] = INOUT;
  IsStat[ ret_dvm_void_vfprintf   ] = INOUT+QCOLLECT;
  IsStat[ call_dvm_vfprintf       ] = INOUT;
  IsStat[ ret_dvm_vfprintf        ] = INOUT+QCOLLECT;
  IsStat[ call_dvm_fwrite         ] = INOUT;
  IsStat[ ret_dvm_fwrite          ] = INOUT+QCOLLECT;
  IsStat[ call_dvm_fread          ] = INOUT;
  IsStat[ ret_dvm_fread           ] = INOUT+QCOLLECT;
  IsStat[ call_DisArrRead         ] = INOUT;
  IsStat[ ret_DisArrRead          ] = INOUT+QCOLLECT;
  IsStat[ call_DisArrWrite        ] = INOUT;
  IsStat[ ret_DisArrWrite         ] = INOUT+QCOLLECT;
  IsStat[ call_dvm_vscanf         ] = INOUT;
  IsStat[ ret_dvm_vscanf          ] = INOUT+QCOLLECT;
  IsStat[ call_dvm_fscanf         ] = INOUT;
  IsStat[ ret_dvm_fscanf          ] = INOUT+QCOLLECT;
  IsStat[ call_dvm_scanf          ] = INOUT;
  IsStat[ ret_dvm_scanf           ] = INOUT+QCOLLECT;
  IsStat[ call_dvm_vfscanf        ] = INOUT;
  IsStat[ ret_dvm_vfscanf         ] = INOUT+QCOLLECT;
  IsStat[ call_dvm_clearerr       ] = INOUT;
  IsStat[ ret_dvm_clearerr        ] = INOUT+QCOLLECT;
  IsStat[ call_dvm_feof           ] = INOUT;
  IsStat[ ret_dvm_feof            ] = INOUT+QCOLLECT;
  IsStat[ call_dvm_ferror         ] = INOUT;
  IsStat[ ret_dvm_ferror          ] = INOUT+QCOLLECT;
  IsStat[ call_dvm_fflush         ] = INOUT;
  IsStat[ ret_dvm_fflush          ] = INOUT+QCOLLECT;
  IsStat[ call_dvm_fgetc          ] = INOUT;
  IsStat[ ret_dvm_fgetc           ] = INOUT+QCOLLECT;
  IsStat[ call_dvm_fgetpos        ] = INOUT;
  IsStat[ ret_dvm_fgetpos         ] = INOUT+QCOLLECT;
  IsStat[ call_dvm_fgets          ] = INOUT;
  IsStat[ ret_dvm_fgets           ] = INOUT+QCOLLECT;
  IsStat[ call_dvm_fputc          ] = INOUT;
  IsStat[ ret_dvm_fputc           ] = INOUT+QCOLLECT;
  IsStat[ call_dvm_fputs          ] = INOUT;
  IsStat[ ret_dvm_fputs           ] = INOUT+QCOLLECT;
  IsStat[ call_dvm_freopen        ] = INOUT;
  IsStat[ ret_dvm_freopen         ] = INOUT+QCOLLECT;
  IsStat[ call_dvm_fseek          ] = INOUT;
  IsStat[ ret_dvm_fseek           ] = INOUT+QCOLLECT;
  IsStat[ call_dvm_fsetpos        ] = INOUT;
  IsStat[ ret_dvm_fsetpos         ] = INOUT+QCOLLECT;
  IsStat[ call_dvm_ftell          ] = INOUT;
  IsStat[ ret_dvm_ftell           ] = INOUT+QCOLLECT;
  IsStat[ call_dvm_getc           ] = INOUT;
  IsStat[ ret_dvm_getc            ] = INOUT+QCOLLECT;
  IsStat[ call_dvm_getchar        ] = INOUT;
  IsStat[ ret_dvm_getchar         ] = INOUT+QCOLLECT;
  IsStat[ call_dvm_gets           ] = INOUT;
  IsStat[ ret_dvm_gets            ] = INOUT+QCOLLECT;
  IsStat[ call_dvm_putc           ] = INOUT;
  IsStat[ ret_dvm_putc            ] = INOUT+QCOLLECT;
  IsStat[ call_dvm_putchar        ] = INOUT;
  IsStat[ ret_dvm_putchar         ] = INOUT+QCOLLECT;
  IsStat[ call_dvm_puts           ] = INOUT;
  IsStat[ ret_dvm_puts            ] = INOUT+QCOLLECT;
  IsStat[ call_dvm_rewind         ] = INOUT;
  IsStat[ ret_dvm_rewind          ] = INOUT+QCOLLECT;
  IsStat[ call_dvm_setbuf         ] = INOUT;
  IsStat[ ret_dvm_setbuf          ] = INOUT+QCOLLECT;
  IsStat[ call_dvm_setvbuf        ] = INOUT;
  IsStat[ ret_dvm_setvbuf         ] = INOUT+QCOLLECT;
  IsStat[ call_dvm_tmpfile        ] = INOUT;
  IsStat[ ret_dvm_tmpfile         ] = INOUT+QCOLLECT;
  IsStat[ call_dvm_ungetc         ] = INOUT;
  IsStat[ ret_dvm_ungetc          ] = INOUT+QCOLLECT;
  IsStat[ call_dvm_void_fprintf   ] = INOUT;
  IsStat[ ret_dvm_void_fprintf    ] = INOUT+QCOLLECT;
  IsStat[ call_dvm_fprintf        ] = INOUT;
  IsStat[ ret_dvm_fprintf         ] = INOUT+QCOLLECT;
  IsStat[ call_dvm_void_printf    ] = INOUT;
  IsStat[ ret_dvm_void_printf     ] = INOUT+QCOLLECT;
  IsStat[ call_dvm_printf         ] = INOUT;
  IsStat[ ret_dvm_printf          ] = INOUT+QCOLLECT;
  IsStat[ call_dvm_void_vprintf   ] = INOUT;
  IsStat[ ret_dvm_void_vprintf    ] = INOUT+QCOLLECT;
  IsStat[ call_dvm_vprintf        ] = INOUT;
  IsStat[ ret_dvm_vprintf         ] = INOUT+QCOLLECT;
  IsStat[ call_dvm_remove         ] = INOUT;
  IsStat[ ret_dvm_remove          ] = INOUT+QCOLLECT;
  IsStat[ call_dvm_rename         ] = INOUT;
  IsStat[ ret_dvm_rename          ] = INOUT+QCOLLECT;
  IsStat[ call_dvm_tmpnam         ] = INOUT;
  IsStat[ ret_dvm_tmpnam          ] = INOUT+QCOLLECT;
  IsStat[ call_dvm_close          ] = INOUT;
  IsStat[ ret_dvm_close           ] = INOUT+QCOLLECT;
  IsStat[ call_dvm_fstat          ] = INOUT;
  IsStat[ ret_dvm_fstat           ] = INOUT+QCOLLECT;
  IsStat[ call_dvm_lseek          ] = INOUT;
  IsStat[ ret_dvm_lseek           ] = INOUT+QCOLLECT;
  IsStat[ call_dvm_open           ] = INOUT;
  IsStat[ ret_dvm_open            ] = INOUT+QCOLLECT;
  IsStat[ call_dvm_read           ] = INOUT;
  IsStat[ ret_dvm_read            ] = INOUT+QCOLLECT;
  IsStat[ call_dvm_write          ] = INOUT;
  IsStat[ ret_dvm_write           ] = INOUT+QCOLLECT;
  IsStat[ call_dvm_access         ] = INOUT;
  IsStat[ ret_dvm_access          ] = INOUT+QCOLLECT;
  IsStat[ call_dvm_stat           ] = INOUT;
  IsStat[ ret_dvm_stat            ] = INOUT+QCOLLECT;

  IsStat[ call_biof_              ] = INOUT;
  IsStat[ ret_biof_               ] = INOUT+QCOLLECT;
  IsStat[ call_eiof_              ] = INOUT;
  IsStat[ ret_eiof_               ] = INOUT+QCOLLECT;


  IsStat[ call_strtrd_            ] = SREDUC;
  IsStat[ ret_strtrd_             ] = SREDUC+QCOLLECT;


  IsStat[ call_waitrd_            ] = WREDUC;
  IsStat[ ret_waitrd_             ] = WREDUC+QCOLLECT;


  IsStat[ call_strtsh_            ] = SSHAD;
  IsStat[ ret_strtsh_             ] = SSHAD+QCOLLECT;


  IsStat[ call_waitsh_            ] = WSHAD;
  IsStat[ ret_waitsh_             ] = WSHAD+QCOLLECT;


  IsStat[ call_arrcpy_            ] = RACC;
  IsStat[ ret_arrcpy_             ] = RACC+QCOLLECT;
  IsStat[ call_rwelm_             ] = RACC;
  IsStat[ ret_rwelm_              ] = RACC+QCOLLECT;
  IsStat[ call_copelm_            ] = RACC;
  IsStat[ ret_copelm_             ] = RACC+QCOLLECT;
  IsStat[ call_elmcpy_            ] = RACC;
  IsStat[ ret_elmcpy_             ] = RACC+QCOLLECT;


  IsStat[ call_realn_             ] = REDISTR;
  IsStat[ ret_realn_              ] = REDISTR+QCOLLECT;
  IsStat[ call_redis_             ] = REDISTR;
  IsStat[ ret_redis_              ] = REDISTR+QCOLLECT;
  IsStat[ call_mrealn_            ] = REDISTR;
  IsStat[ ret_mrealn_             ] = REDISTR+QCOLLECT;
  IsStat[ call_mredis_            ] = REDISTR;
  IsStat[ ret_mredis_             ] = REDISTR+QCOLLECT;

  IsStat[ Event0                  ] = 0;
  IsStat[ Event_MeasureStart      ] = 0;
  IsStat[ Event_MeasureFinish     ] = 0;
  IsStat[ DVM_Trace_Start         ] = 0;
  IsStat[ call_rtl_Send           ] = 0;
  IsStat[ ret_rtl_Send            ] = 0;
  IsStat[ call_rtl_Recv           ] = 0;
  IsStat[ ret_rtl_Recv            ] = 0;
  IsStat[ call_rtl_BroadCast      ] = 0;
  IsStat[ ret_rtl_BroadCast       ] = 0;
  IsStat[ call_rtl_Sendnowait     ] = 0;
  IsStat[ ret_rtl_Sendnowait      ] = 0;
  IsStat[ call_rtl_Recvnowait     ] = 0;
  IsStat[ ret_rtl_Recvnowait      ] = 0;
  IsStat[ call_rtl_Waitrequest    ] = 0;
  IsStat[ ret_rtl_Waitrequest     ] = 0;
  IsStat[ call_rtl_Testrequest    ] = 0;
  IsStat[ ret_rtl_Testrequest     ] = 0;
  IsStat[ call_rtl_SendA          ] = 0;
  IsStat[ ret_rtl_SendA           ] = 0;
  IsStat[ call_rtl_RecvA          ] = 0;
  IsStat[ ret_rtl_RecvA           ] = 0;
  IsStat[ call_delrg_             ] = 0;
  IsStat[ ret_delrg_              ] = 0;
  IsStat[ call_insred_            ] = 0;
  IsStat[ ret_insred_             ] = 0;
  IsStat[ call_bsynch_            ] = 0;
  IsStat[ ret_bsynch_             ] = 0;

  IsStat[ call_crtda_             ] = 0;
  IsStat[ ret_crtda_              ] = 0;
  IsStat[ call_getam_             ] = 0;
  IsStat[ ret_getam_              ] = 0;
  IsStat[ call_crtamv_            ] = 0;
  IsStat[ ret_crtamv_             ] = 0;
  IsStat[ call_align_             ] = 0;
  IsStat[ ret_align_              ] = 0;
  IsStat[ call_getps_             ] = 0;
  IsStat[ ret_getps_              ] = 0;
  IsStat[ call_saverg_            ] = 0;
  IsStat[ ret_saverg_             ] = 0;
  IsStat[ call_CreateVMS          ] = 0;
  IsStat[ ret_CreateVMS           ] = 0;
  IsStat[ call_tstelm_            ] = 0;
  IsStat[ ret_tstelm_             ] = 0;

  IsStat[ call_rlocel_            ] = 0;
  IsStat[ ret_rlocel_             ] = 0;
  IsStat[ call_delda_             ] = 0;
  IsStat[ ret_delda_              ] = 0;
  IsStat[ call_delobj_            ] = 0;
  IsStat[ ret_delobj_             ] = 0;

  IsStat[ call_wlocel_            ] = 0;
  IsStat[ ret_wlocel_             ] = 0;
  IsStat[ call_clocel_            ] = 0;
  IsStat[ ret_clocel_             ] = 0;
  IsStat[ call_GetLocElmAddr      ] = 0;
  IsStat[ ret_GetLocElmAddr       ] = 0;
  IsStat[ call_getlen_            ] = 0;
  IsStat[ ret_getlen_             ] = 0;

  IsStat[ call_tron_              ] = 0;
  IsStat[ call_delamv_            ] = 0;
  IsStat[ ret_delamv_             ] = 0;
  IsStat[ call_distr_             ] = 0;
  IsStat[ ret_distr_              ] = 0;
  IsStat[ call_crtred_            ] = 0;
  IsStat[ ret_crtred_             ] = 0;
  IsStat[ call_delred_            ] = 0;
  IsStat[ ret_delred_             ] = 0;
  IsStat[ call_RedVar_Done        ] = 0;
  IsStat[ ret_RedVar_Done         ] = 0;
  IsStat[ call_parloop_Done       ] = 0;
  IsStat[ ret_parloop_Done        ] = 0;
  IsStat[ call_env_Done           ] = 0;
  IsStat[ ret_env_Done            ] = 0;
  IsStat[ call_vms_Done           ] = 0;
  IsStat[ ret_vms_Done            ] = 0;
  IsStat[ call_begbl_             ] = 0;
  IsStat[ ret_begbl_              ] = 0;
  IsStat[ call_endbl_             ] = 0;
  IsStat[ ret_endbl_              ] = 0;
  IsStat[ call_crtpl_             ] = 0;
  IsStat[ ret_crtpl_              ] = 0;
  IsStat[ call_mappl_             ] = 0;
  IsStat[ ret_mappl_              ] = 0;
  IsStat[ call_endpl_             ] = 0;
  IsStat[ ret_endpl_              ] = 0;
  IsStat[ call_locind_            ] = 0;
  IsStat[ ret_locind_             ] = 0;
  IsStat[ call_tstda_             ] = 0;
  IsStat[ ret_tstda_              ] = 0;
  IsStat[ call_srmem_             ] = 0;
  IsStat[ ret_srmem_              ] = 0;
  IsStat[ call_tstio_             ] = 0;
  IsStat[ ret_tstio_              ] = 0;
  IsStat[ call_getrnk_            ] = 0;
  IsStat[ ret_getrnk_             ] = 0;
  IsStat[ call_getsiz_            ] = 0;
  IsStat[ ret_getsiz_             ] = 0;

  IsStat[ call_arrmap_            ] = 0;
  IsStat[ ret_arrmap_             ] = 0;
  IsStat[ call_red_Sendnowait     ] = 0;
  IsStat[ ret_red_Sendnowait      ] = 0;
  IsStat[ call_red_Recvnowait     ] = 0;
  IsStat[ ret_red_Recvnowait      ] = 0;
  IsStat[ call_red_Waitrequest    ] = 0;
  IsStat[ ret_red_Waitrequest     ] = 0;
  IsStat[ call_setpsw_            ] = 0;
  IsStat[ ret_setpsw_             ] = 0;
  IsStat[ call_red_Testrequest    ] = 0;
  IsStat[ ret_red_Testrequest     ] = 0;
  IsStat[ call_setind_            ] = 0;
  IsStat[ ret_setind_             ] = 0;
  IsStat[ call_locsiz_            ] = 0;
  IsStat[ ret_locsiz_             ] = 0;
  IsStat[ call_shd_Sendnowait     ] = 0;
  IsStat[ ret_shd_Sendnowait      ] = 0;
  IsStat[ call_shd_Recvnowait     ] = 0;
  IsStat[ ret_shd_Recvnowait      ] = 0;
  IsStat[ call_shd_Waitrequest    ] = 0;
  IsStat[ ret_shd_Waitrequest     ] = 0;
  IsStat[ call_imlast_            ] = 0;
  IsStat[ ret_imlast_             ] = 0;
  IsStat[ call_insshd_            ] = 0;
  IsStat[ ret_insshd_             ] = 0;
  IsStat[ call_malign_            ] = 0;
  IsStat[ ret_malign_             ] = 0;
  IsStat[ call_crtrg_             ] = 0;
  IsStat[ ret_crtrg_              ] = 0;

  IsStat[ call_amvmap_            ] = 0;
  IsStat[ ret_amvmap_             ] = 0;
  IsStat[ call_CreateBoundBuffer  ] = 0;
  IsStat[ ret_CreateBoundBuffer   ] = 0;
  IsStat[ call_getamv_            ] = 0;
  IsStat[ ret_getamv_             ] = 0;
  IsStat[ call_saverv_            ] = 0;
  IsStat[ ret_saverv_             ] = 0;
  IsStat[ call_rstrg_             ] = 0;
  IsStat[ ret_rstrg_              ] = 0;
  IsStat[ call_exfrst_            ] = 0;
  IsStat[ ret_exfrst_             ] = 0;
  IsStat[ call_dopl_              ] = 0;
  IsStat[ ret_dopl_               ] = 0;
  IsStat[ call_dvm_Init           ] = 0;
  IsStat[ ret_dvm_Init            ] = 0;
  IsStat[ call_dvm_Done           ] = 0;
  IsStat[ ret_dvm_Done            ] = 0;
  IsStat[ DVM_Trace_Finish        ] = 0;
  IsStat[ Event_rtl_GetMasterProc ] = 0;
  IsStat[ Event_rtl_GetCentralProc] = 0;
  IsStat[ Event_rtl_GetCurrentProc] = 0;
  IsStat[ Event_rtl_GetIOProc     ] = 0;
  IsStat[ call_mdistr_            ] = 0;
  IsStat[ ret_mdistr_             ] = 0;

  IsStat[ call_delarm_            ] = 0;
  IsStat[ ret_delarm_             ] = 0;
  IsStat[ call_delmvm_            ] = 0;
  IsStat[ ret_delmvm_             ] = 0;

  IsStat[ Event_dvm_exit          ] = 0;
  IsStat[ call_mps_Bcast          ] = 0;
  IsStat[ ret_mps_Bcast           ] = 0;
  IsStat[ call_mps_Barrier        ] = 0;
  IsStat[ ret_mps_Barrier         ] = 0;
  IsStat[ call_dvm_dfread         ] = 0;
  IsStat[ ret_dvm_dfread          ] = 0;
  IsStat[ call_dvm_dfwrite        ] = 0;
  IsStat[ ret_dvm_dfwrite         ] = 0;
  IsStat[ call_crtshg_            ] = 0;
  IsStat[ ret_crtshg_             ] = 0;
  IsStat[ call_inssh_             ] = 0;
  IsStat[ ret_inssh_              ] = 0;
 
  IsStat[ call_delshg_            ] = 0;
  IsStat[ ret_delshg_             ] = 0;
  IsStat[ call_bbuf_Done          ] = 0;
  IsStat[ ret_bbuf_Done           ] = 0;
  IsStat[ call_bgroup_Done        ] = 0;
  IsStat[ ret_bgroup_Done         ] = 0;
  IsStat[ call_amview_Done        ] = 0;
  IsStat[ ret_amview_Done         ] = 0;
  IsStat[ call_ArrMap_Done        ] = 0;
  IsStat[ ret_ArrMap_Done         ] = 0;
  IsStat[ call_disarr_Done        ] = 0;
  IsStat[ ret_disarr_Done         ] = 0;
  IsStat[ call_RedGroup_Done      ] = 0;
  IsStat[ ret_RedGroup_Done       ] = 0;
  IsStat[ call_AMVMap_Done        ] = 0;
  IsStat[ ret_AMVMap_Done         ] = 0;
  IsStat[ call_getind_            ] = 0;
  IsStat[ ret_getind_             ] = 0;
  IsStat[ call_addhdr_            ] = 0;
  IsStat[ ret_addhdr_             ] = 0;
  IsStat[ call_delhdr_            ] = 0;
  IsStat[ ret_delhdr_             ] = 0;
  IsStat[ call_troff_             ] = 0;
  IsStat[ call_tsynch_            ] = 0;
  IsStat[ ret_tsynch_             ] = 0;

  /* New functions */    /*E0001*/

  IsStat[ call_getamr_            ] = 0;
  IsStat[ ret_getamr_             ] = 0;
  IsStat[ call_crtps_             ] = 0;
  IsStat[ ret_crtps_              ] = 0;
  IsStat[ call_psview_            ] = 0;
  IsStat[ ret_psview_             ] = 0;
  IsStat[ call_delps_             ] = 0;
  IsStat[ ret_delps_              ] = 0;
  IsStat[ call_setelw_            ] = 0;
  IsStat[ ret_setelw_             ] = 0;

  IsStat[ call_recvsh_            ] = SSHAD;
  IsStat[ ret_recvsh_             ] = SSHAD+QCOLLECT;
  IsStat[ call_sendsh_            ] = SSHAD;
  IsStat[ ret_sendsh_             ] = SSHAD+QCOLLECT;

  IsStat[ call_mapam_             ] = 0;
  IsStat[ ret_mapam_              ] = 0;
  IsStat[ call_runam_             ] = 0;
  IsStat[ ret_runam_              ] = 0;
  IsStat[ call_stopam_            ] = 0;
  IsStat[ ret_stopam_             ] = 0;

  IsStat[ call_arwelm_            ] = SRACC;
  IsStat[ ret_arwelm_             ] = SRACC+QCOLLECT;
  IsStat[ call_arwelf_            ] = SRACC;
  IsStat[ ret_arwelf_             ] = SRACC+QCOLLECT;
  IsStat[ call_acopel_            ] = SRACC;
  IsStat[ ret_acopel_             ] = SRACC+QCOLLECT;
  IsStat[ call_aelmcp_            ] = SRACC;
  IsStat[ ret_aelmcp_             ] = SRACC+QCOLLECT;
  IsStat[ call_aarrcp_            ] = SRACC;
  IsStat[ ret_aarrcp_             ] = SRACC+QCOLLECT;
  IsStat[ call_waitcp_            ] = WRACC;
  IsStat[ ret_waitcp_             ] = WRACC+QCOLLECT;

  IsStat[ call_crtrbl_            ] = 0;
  IsStat[ ret_crtrbl_             ] = 0;
  IsStat[ call_loadrb_            ] = SRACC;
  IsStat[ ret_loadrb_             ] = SRACC+QCOLLECT;
  IsStat[ call_waitrb_            ] = WRACC;
  IsStat[ ret_waitrb_             ] = WRACC+QCOLLECT;
  IsStat[ call_delrb_             ] = 0;
  IsStat[ ret_delrb_              ] = 0;
  IsStat[ call_crtbg_             ] = 0;
  IsStat[ ret_crtbg_              ] = 0;
  IsStat[ call_insrb_             ] = 0;
  IsStat[ ret_insrb_              ] = 0;
  IsStat[ call_loadbg_            ] = SRACC;
  IsStat[ ret_loadbg_             ] = SRACC+QCOLLECT;
  IsStat[ call_waitbg_            ] = WRACC;
  IsStat[ ret_waitbg_             ] = WRACC+QCOLLECT;
  IsStat[ call_delbg_             ] = 0;
  IsStat[ ret_delbg_              ] = 0;

  IsStat[ call_crtibl_            ] = 0;
  IsStat[ ret_crtibl_             ] = 0;
  IsStat[ call_loadib_            ] = SRACC;
  IsStat[ ret_loadib_             ] = SRACC+QCOLLECT;
  IsStat[ call_waitib_            ] = WRACC;
  IsStat[ ret_waitib_             ] = WRACC+QCOLLECT;
  IsStat[ call_delib_             ] = 0;
  IsStat[ ret_delib_              ] = 0;
  IsStat[ call_crtig_             ] = 0;
  IsStat[ ret_crtig_              ] = 0;
  IsStat[ call_insib_             ] = 0;
  IsStat[ ret_insib_              ] = 0;
  IsStat[ call_loadig_            ] = SRACC;
  IsStat[ ret_loadig_             ] = SRACC+QCOLLECT;
  IsStat[ call_waitig_            ] = WRACC;
  IsStat[ ret_waitig_             ] = WRACC+QCOLLECT;
  IsStat[ call_delig_             ] = 0;
  IsStat[ ret_delig_              ] = 0;

  IsStat[ call_DelDA              ] = 0;
  IsStat[ ret_DelDA               ] = 0;

  IsStat[ call_plmap_             ] = 0;
  IsStat[ ret_plmap_              ] = 0;

  IsStat[ call_RegBufGroup_Done   ] = 0;
  IsStat[ ret_RegBufGroup_Done    ] = 0;

  IsStat[ call_crtrba_            ] = 0;
  IsStat[ ret_crtrba_             ] = 0;
  IsStat[ call_crtrbp_            ] = 0;
  IsStat[ ret_crtrbp_             ] = 0;

  IsStat[ call_genblk_            ] = 0;
  IsStat[ ret_genblk_             ] = 0;

  IsStat[ call_dyn_GetLocalBlock  ] = 0;
  IsStat[ ret_dyn_GetLocalBlock   ] = 0;

  IsStat[ call_rmkind_            ] = 0;
  IsStat[ ret_rmkind_             ] = 0;

  IsStat[ call_lindtp_            ] = 0;
  IsStat[ ret_lindtp_             ] = 0;

  IsStat[ call_setgrn_            ] = 0;
  IsStat[ ret_setgrn_             ] = 0;
  IsStat[ call_gettar_            ] = REDISTR;
  IsStat[ ret_gettar_             ] = REDISTR+QCOLLECT;
  IsStat[ call_rsttar_            ] = 0;
  IsStat[ ret_rsttar_             ] = 0;

  IsStat[ call_IdBufGroup_Done    ] = 0;
  IsStat[ ret_IdBufGroup_Done     ] = 0;
  IsStat[ call_StartLoadBuffer    ] = 0;
  IsStat[ ret_StartLoadBuffer     ] = 0;
  IsStat[ call_WaitLoadBuffer     ] = 0;
  IsStat[ ret_WaitLoadBuffer      ] = 0;

  IsStat[ call_crtib_             ] = 0;
  IsStat[ ret_crtib_              ] = 0;

  IsStat[ call_acsend_            ] = 0;
  IsStat[ ret_acsend_             ] = 0;
  IsStat[ call_acrecv_            ] = 0;
  IsStat[ ret_acrecv_             ] = 0;

  IsStat[ call_recvla_            ] = SSHAD;
  IsStat[ ret_recvla_             ] = SSHAD+QCOLLECT;
  IsStat[ call_sendsa_            ] = SSHAD;
  IsStat[ ret_sendsa_             ] = SSHAD+QCOLLECT;
  IsStat[ call_across_            ] = 0;
  IsStat[ ret_across_             ] = 0;

  IsStat[ call_addbnd_            ] = 0;
  IsStat[ ret_addbnd_             ] = 0;

  IsStat[ call_incsh_             ] = 0;
  IsStat[ ret_incsh_              ] = 0;
  IsStat[ call_incshd_            ] = 0;
  IsStat[ ret_incshd_             ] = 0;

  IsStat[ call_MPI_Allreduce      ] = 0;
  IsStat[ ret_MPI_Allreduce       ] = 0;
  IsStat[ call_MPI_Bcast          ] = 0;
  IsStat[ ret_MPI_Bcast           ] = 0;
  IsStat[ call_MPI_Barrier        ] = 0;
  IsStat[ ret_MPI_Barrier         ] = 0;

  IsStat[ call_dvm_gzsetparams    ] = 0;
  IsStat[ ret_dvm_gzsetparams     ] = 0;
  IsStat[ call_dvm_gzflush        ] = 0;
  IsStat[ ret_dvm_gzflush         ] = 0;

  IsStat[ call_doacr_             ] = 0;
  IsStat[ ret_doacr_              ] = 0;

  IsStat[ call_rstshg_            ] = 0;
  IsStat[ ret_rstshg_             ] = 0;

  IsStat[ call_crtrda_            ] = 0;
  IsStat[ ret_crtrda_             ] = 0;

  IsStat[ call_strtac_            ] = SRACC;
  IsStat[ ret_strtac_             ] = SRACC+QCOLLECT;
  IsStat[ call_waitac_            ] = WRACC;
  IsStat[ ret_waitac_             ] = WRACC+QCOLLECT;
  IsStat[ call_crtcg_             ] = 0;
  IsStat[ ret_crtcg_              ] = 0;
  IsStat[ call_inscg_             ] = 0;
  IsStat[ ret_inscg_              ] = 0;
  IsStat[ call_strtcg_            ] = SRACC;
  IsStat[ ret_strtcg_             ] = SRACC+QCOLLECT;
  IsStat[ call_waitcg_            ] = WRACC;
  IsStat[ ret_waitcg_             ] = WRACC+QCOLLECT;
  IsStat[ call_delcg_             ] = 0;
  IsStat[ ret_delcg_              ] = 0;
  IsStat[ call_DAConsistGroup_Done] = 0;
  IsStat[ ret_DAConsistGroup_Done ] = 0;
  IsStat[ call_consda_            ] = SRACC;
  IsStat[ ret_consda_             ] = SRACC+QCOLLECT;
  IsStat[ call_inclcg_            ] = 0;
  IsStat[ ret_inclcg_             ] = 0;
  IsStat[ call_rstcg_             ] = 0;
  IsStat[ ret_rstcg_              ] = 0;
  IsStat[ call_rstrda_            ] = 0;
  IsStat[ ret_rstrda_             ] = 0;

  IsStat[ call_rstbg_             ] = 0;
  IsStat[ ret_rstbg_              ] = 0;

  IsStat[ call_addshd_            ] = 0;
  IsStat[ ret_addshd_             ] = 0;

  IsStat[ call_setba_             ] = 0;
  IsStat[ ret_setba_              ] = 0;
  IsStat[ call_rstba_             ] = 0;
  IsStat[ ret_rstba_              ] = 0;

  IsStat[ call_dvmcom_            ] = 0;
  IsStat[ ret_dvmcom_             ] = 0;

  IsStat[ call_exlsiz_            ] = 0;
  IsStat[ ret_exlsiz_             ] = 0;
  IsStat[ call_exlind_            ] = 0;
  IsStat[ ret_exlind_             ] = 0;

  IsStat[ call_blkdiv_            ] = 0;
  IsStat[ ret_blkdiv_             ] = 0;

  IsStat[ call_io_Sendnowait      ] = 0;
  IsStat[ ret_io_Sendnowait       ] = 0;
  IsStat[ call_io_Recvnowait      ] = 0;
  IsStat[ ret_io_Recvnowait       ] = 0;
  IsStat[ call_io_Waitrequest     ] = 0;
  IsStat[ ret_io_Waitrequest      ] = 0;
  IsStat[ call_io_Testrequest     ] = 0;
  IsStat[ ret_io_Testrequest      ] = 0;

  IsStat[ call_MPI_Alltoallv      ] = 0;
  IsStat[ ret_MPI_Alltoallv       ] = 0;

  IsStat[ call_dvmsend_           ] = 0;
  IsStat[ ret_dvmsend_            ] = 0;
  IsStat[ call_dvmrecv_           ] = 0;
  IsStat[ ret_dvmrecv_            ] = 0;
  IsStat[ call_dvmsendrecv_       ] = 0;
  IsStat[ ret_dvmsendrecv_        ] = 0;
  IsStat[ call_dvmreduce_         ] = 0;
  IsStat[ ret_dvmreduce_          ] = 0;
  IsStat[ call_dvmiprobe_         ] = 0;
  IsStat[ ret_dvmiprobe_          ] = 0;

  IsStat[ call_dvmsecread_        ] = INOUT;
  IsStat[ ret_dvmsecread_         ] = INOUT+QCOLLECT;
  IsStat[ call_dvmsecwait_        ] = INOUT;
  IsStat[ ret_dvmsecwait_         ] = INOUT+QCOLLECT;
  IsStat[ call_dvmsecwrite_       ] = INOUT;
  IsStat[ ret_dvmsecwrite_        ] = INOUT+QCOLLECT;

  IsStat[ call_pllind_            ] = 0;
  IsStat[ ret_pllind_             ] = 0;

  IsStat[ call_dacsum_            ] = 0;
  IsStat[ ret_dacsum_             ] = 0;
  IsStat[ call_getdas_            ] = 0;
  IsStat[ ret_getdas_             ] = 0;
  IsStat[ call_arcsum_            ] = 0;
  IsStat[ ret_arcsum_             ] = 0;

  IsStat[ call_dvmsync_           ] = INOUT;
  IsStat[ ret_dvmsync_            ] = INOUT+QCOLLECT;

  IsStat[ Event536                ] = 0;
  IsStat[ Event537                ] = 0;
  IsStat[ Event538                ] = 0;
  IsStat[ Event539                ] = 0;
  IsStat[ Event540                ] = 0;
  IsStat[ Event541                ] = 0;
  IsStat[ Event542                ] = 0;
  IsStat[ Event543                ] = 0;
  IsStat[ Event544                ] = 0;
  IsStat[ Event545                ] = 0;
  IsStat[ Event546                ] = 0;
  IsStat[ Event547                ] = 0;
  IsStat[ Event548                ] = 0;
  IsStat[ Event549                ] = 0;

  /* Dynamic control events */    /*E0002*/

  IsStat[ call_dprstv_            ] = 0;
  IsStat[ ret_dprstv_             ] = 0;
  IsStat[ call_dstv_              ] = 0;
  IsStat[ ret_dstv_               ] = 0;
  IsStat[ call_dldv_              ] = 0;
  IsStat[ ret_dldv_               ] = 0;
  IsStat[ call_dbegpl_            ] = 0;
  IsStat[ ret_dbegpl_             ] = 0;
  IsStat[ call_dbegsl_            ] = 0;
  IsStat[ ret_dbegsl_             ] = 0;
  IsStat[ call_dendl_             ] = 0;
  IsStat[ ret_dendl_              ] = 0;
  IsStat[ call_diter_             ] = 0;
  IsStat[ ret_diter_              ] = 0;
  IsStat[ call_drmbuf_            ] = 0;
  IsStat[ ret_drmbuf_             ] = 0;
  IsStat[ call_dskpbl_            ] = 0;
  IsStat[ ret_dskpbl_             ] = 0;
  IsStat[ call_dbegtr_            ] = 0;
  IsStat[ ret_dbegtr_             ] = 0;
  IsStat[ call_dread_             ] = 0;
  IsStat[ ret_dread_              ] = 0;
  IsStat[ call_drarr_             ] = 0;
  IsStat[ ret_drarr_              ] = 0;
  IsStat[ call_dcrtrg_            ] = 0;
  IsStat[ ret_dcrtrg_             ] = 0;
  IsStat[ call_dinsrd_            ] = 0;
  IsStat[ ret_dinsrd_             ] = 0;
  IsStat[ call_ddelrg_            ] = 0;
  IsStat[ ret_ddelrg_             ] = 0;
  IsStat[ call_dsavrg_            ] = 0;
  IsStat[ ret_dsavrg_             ] = 0;
  IsStat[ call_dclcrg_            ] = 0;
  IsStat[ ret_dclcrg_             ] = 0;
  IsStat[ Event584                ] = 0;
  IsStat[ Event585                ] = 0;
  IsStat[ Event586                ] = 0;
  IsStat[ Event587                ] = 0;
  IsStat[ Event588                ] = 0;
  IsStat[ Event589                ] = 0;
  IsStat[ Event590                ] = 0;
  IsStat[ Event591                ] = 0;
  IsStat[ Event592                ] = 0;
  IsStat[ Event593                ] = 0;
  IsStat[ Event594                ] = 0;
  IsStat[ Event595                ] = 0;
  IsStat[ Event596                ] = 0;
  IsStat[ Event597                ] = 0;
  IsStat[ Event598                ] = 0;
  IsStat[ Event599                ] = 0;

  /* Statistics events */    /*E0003*/
                                      
  IsStat[ call_binter_            ] = 0;
  IsStat[ ret_binter_             ] = 0;
  IsStat[ call_einter_            ] = 0;
  IsStat[ ret_einter_             ] = 0;
  IsStat[ call_bsloop_            ] = 0;
  IsStat[ ret_bsloop_             ] = 0;
  IsStat[ call_bploop_            ] = 0;
  IsStat[ ret_bploop_             ] = 0;
  IsStat[ call_enloop_            ] = 0;
  IsStat[ ret_enloop_             ] = 0;
  IsStat[ Event610                ] = 0;
  IsStat[ Event611                ] = 0;
  IsStat[ Event612                ] = 0;
  IsStat[ Event613                ] = 0;
  IsStat[ Event614                ] = 0;
  IsStat[ Event615                ] = 0;
  IsStat[ Event616                ] = 0;
  IsStat[ Event617                ] = 0;
  IsStat[ Event618                ] = 0;
  IsStat[ Event619                ] = 0;

  return;
}


#endif    /*  _STATEVNT_C_  */    /*E0004*/
