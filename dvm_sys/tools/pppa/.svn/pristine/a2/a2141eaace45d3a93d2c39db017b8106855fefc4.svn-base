#ifndef _SYSSTAT_H
#define _SYSSTAT_H
typedef struct {
	double  CallCount; 
	double  ProductTime;
	double  LostTime;
}s_GRPTIMES;

#define StatGrpCount 28  /* number of operation groups */
#define UserGrp       0  /* execution of user program */
#define MsgPasGrp     1  /* operation group of message exchange */
#define StartRedGrp   2  /* start reduction */
#define WaitRedGrp    3  /* waiting for the end of reduction */   		 
#define RedGrp        4  /* other reduction operations */  		  		
#define StartShdGrp   5  /* start edges exchange */  				 	 
#define WaitShdGrp    6  /* waiting for the end of edges exchange */    
#define ShdGrp        7  /* other operations of edges exchange */   	 
#define DistrGrp      8  /* data distribution */   						 
#define ReDistrGrp    9  /* data redistribution */   					 
#define MapPLGrp     10  /* parallel loop distribution operations */    
#define DoPLGrp      11  /* function dopl_ */    								
#define ProgBlockGrp 12  /* program block operations */ 			   
#define IOGrp        13  /* input/output operations */  				  
#define RemAccessGrp 14  /* remote access operations */  				  
#define UserDebGrp   15  /* dynamic control operations
                            and operations for user program trace */    
#define StatistGrp   16  /* user program interval operations 
                            for performance analysis */   					 
#define SystemGrp    27  /* system work */    	
						
#endif
