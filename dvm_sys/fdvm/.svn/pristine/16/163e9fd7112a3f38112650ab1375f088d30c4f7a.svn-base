include_directories("${CMAKE_CURRENT_BINARY_DIR}")
if(MSVC_IDE)
   foreach(DIR ${SAGE_DIRS})
    file(GLOB_RECURSE FILES RELATIVE ${CMAKE_CURRENT_SOURCE_DIR}
      "${DIR}/*.h" "${DIR}/*.def" "${DIR}/head" "${DIR}/tag")
    set(HEADERS ${HEADERS} ${FILES})
  endforeach()
  source_group("Header Files" FILES ${HEADERS})
endif()
add_library(libDB
  anal_ind.c db.c db_unp.c db_unp_comm.c db_unp_vpc.c dbutils.c garb_coll.c
  glob_anal.c ker_fun.c list.c make_nodes.c mod_ref.c ndeps.c readnodes.c sets.c
  setutils.c symb_alg.c writenodes.c ${HEADERS})