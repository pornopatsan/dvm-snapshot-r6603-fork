#pragma once

#ifdef _WIN32
#ifdef _DEBUG

#define _CRTDBG_MAP_ALLOC 
#include <stdlib.h> 
#include <crtdbg.h>  

#ifdef _DEBUG 
	#ifndef DBG_NEW 
		#define DBG_NEW new ( _NORMAL_BLOCK , __FILE__ , __LINE__ )
		#define new DBG_NEW 
	#endif 
#endif 

#endif
#endif