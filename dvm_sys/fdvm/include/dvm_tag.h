#define HPF_TEMPLATE_STAT            296
#define HPF_ALIGN_STAT               297
#define HPF_PROCESSORS_STAT	     298
#define DVM_DISTRIBUTE_DIR           277
#define DVM_REDISTRIBUTE_DIR         299
#define DVM_PARALLEL_ON_DIR          211
#define DVM_SHADOW_START_DIR         212
#define DVM_SHADOW_GROUP_DIR         213
#define DVM_SHADOW_WAIT_DIR          214
#define DVM_REDUCTION_START_DIR      215
#define DVM_REDUCTION_GROUP_DIR      216
#define DVM_REDUCTION_WAIT_DIR       217
#define DVM_DYNAMIC_DIR              218
#define DVM_ALIGN_DIR                219 
#define DVM_REALIGN_DIR              220 
#define DVM_REALIGN_NEW_DIR          221  
#define DVM_REMOTE_ACCESS_DIR        222  
#define HPF_INDEPENDENT_DIR          223 
#define DVM_SHADOW_DIR               224 
#define DVM_NEW_VALUE_DIR            247
#define DVM_VAR_DECL           	     248
#define DVM_POINTER_DIR              249
#define DVM_DEBUG_DIR	      	     146  
#define DVM_ENDDEBUG_DIR	     147  
#define DVM_TRACEON_DIR		     148  
#define DVM_TRACEOFF_DIR	     149       
#define DVM_INTERVAL_DIR	     128  
#define DVM_ENDINTERVAL_DIR	     129
#define DVM_TASK_REGION_DIR 	     605	
#define DVM_END_TASK_REGION_DIR      606	
#define DVM_ON_DIR  		     607	
#define DVM_END_ON_DIR		     608	
#define DVM_TASK_DIR                 609	
#define DVM_MAP_DIR 		     610	
#define DVM_PARALLEL_TASK_DIR	     611	
#define DVM_INHERIT_DIR		     612
#define DVM_INDIRECT_GROUP_DIR	     613	
#define DVM_INDIRECT_ACCESS_DIR      614	
#define DVM_REMOTE_GROUP_DIR         615	
#define DVM_RESET_DIR		     616	
#define DVM_PREFETCH_DIR	     617	
#define DVM_OWN_DIR		     618	
#define DVM_HEAP_DIR		     619	
#define DVM_ASYNCID_DIR		     620	
#define DVM_ASYNCHRONOUS_DIR	     621	
#define DVM_ENDASYNCHRONOUS_DIR	     622	
#define DVM_ASYNCWAIT_DIR	     623	
#define DVM_F90_DIR		     624 
#define DVM_BARRIER_DIR		     625
#define FORALL_STAT                  626
#define DVM_CONSISTENT_GROUP_DIR     627	
#define DVM_CONSISTENT_START_DIR     628	
#define DVM_CONSISTENT_WAIT_DIR      629	
#define DVM_CONSISTENT_DIR           630
#define DVM_CHECK_DIR                631
#define DVM_IO_MODE_DIR              632
#define DVM_LOCALIZE_DIR             633    
#define DVM_SHADOW_ADD_DIR           634    
#define DVM_CP_CREATE_DIR            635
#define DVM_CP_LOAD_DIR              636
#define DVM_CP_SAVE_DIR              637	
#define DVM_CP_WAIT_DIR              638
#define DVM_EXIT_INTERVAL_DIR	     639
#define DVM_TEMPLATE_CREATE_DIR	     640 
#define DVM_TEMPLATE_DELETE_DIR	     641       		   
#define BLOCK_OP           705
#define NEW_SPEC_OP        706
#define REDUCTION_OP       707
#define SHADOW_RENEW_OP    708
#define SHADOW_START_OP    709
#define SHADOW_WAIT_OP     710
#define DIAG_OP            711
#define REMOTE_ACCESS_OP   712
#define TEMPLATE_OP        713     
#define PROCESSORS_OP      714     
#define DYNAMIC_OP         715     
#define ALIGN_OP           716     
#define DISTRIBUTE_OP      717     
#define SHADOW_OP          718 
#define INDIRECT_ACCESS_OP 719
#define ACROSS_OP          720  
#define NEW_VALUE_OP       721 
#define SHADOW_COMP_OP     722  
#define STAGE_OP           723  
#define FORALL_OP          724
#define CONSISTENT_OP      725
#define PARALLEL_OP        737      
#define INDIRECT_OP        738     
#define DERIVED_OP         739     
#define DUMMY_REF          740     
#define COMMON_OP          741
#define SHADOW_NAMES_OP    742
          
#define SHADOW_GROUP_NAME     523
#define REDUCTION_GROUP_NAME  524
#define REF_GROUP_NAME        525      
#define ASYNC_ID	      526
#define CONSISTENT_GROUP_NAME 527 

#define ACC_REGION_DIR        	         900     /* ACC Fortran */
#define ACC_END_REGION_DIR               901     /* ACC Fortran */
#define ACC_CALL_STMT                    907     /* ACC Fortran */
#define ACC_KERNEL_HEDR                  908     /* ACC Fortran */
#define ACC_GET_ACTUAL_DIR               909      /* ACC Fortran */
#define ACC_ACTUAL_DIR                   910      /* ACC Fortran */
#define ACC_CHECKSECTION_DIR             911      /* ACC Fortran */
#define ACC_END_CHECKSECTION_DIR         912      /* ACC Fortran */
                            
#define ACC_INLOCAL_OP                   931     /* ACC Fortran */
#define ACC_INOUT_OP                     932     /* ACC Fortran */
#define ACC_IN_OP                        933     /* ACC Fortran */
#define ACC_OUT_OP                       934     /* ACC Fortran */
#define ACC_LOCAL_OP                     935     /* ACC Fortran */
#define ACC_PRIVATE_OP                   936     /* ACC Fortran */
#define ACC_DEVICE_OP                    937     /* ACC Fortran */
#define ACC_CUDA_OP                      938     /* ACC Fortran */
#define ACC_HOST_OP                      939     /* ACC Fortran */

#define ACC_GLOBAL_OP                    940     /* ACC Fortran */ 
#define ACC_ATTRIBUTES_OP                941     /* ACC Fortran */
#define ACC_VALUE_OP                     942     /* ACC Fortran */
#define ACC_SHARED_OP                    943     /* ACC Fortran */
#define ACC_CONSTANT_OP                  944     /* ACC Fortran */
#define ACC_USES_OP                      945     /* ACC Fortran */
#define ACC_CALL_OP                      946     /* ACC Fortran */
#define ACC_CUDA_BLOCK_OP                947     /* ACC Fortran */

#define ACC_TARGETS_OP                   948     /* ACC Fortran */
#define ACC_ASYNC_OP                     949     /* ACC Fortran */

#define SPF_ANALYSIS_DIR                 950     /* SAPFOR */
#define SPF_PARALLEL_DIR                 951     /* SAPFOR */
#define SPF_TRANSFORM_DIR                952     /* SAPFOR */
#define SPF_NOINLINE_OP                  953     /* SAPFOR */
#define SPF_PARALLEL_REG_DIR             954     /* SAPFOR */
#define SPF_END_PARALLEL_REG_DIR         955     /* SAPFOR */
#define SPF_REGION_NAME                  956     /* SAPFOR */
#define SPF_PRIVATES_EXPANSION_OP        957     /* SAPFOR */
#define SPF_FISSION_OP                   958     /* SAPFOR */ 
