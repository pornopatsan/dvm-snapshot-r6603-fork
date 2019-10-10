#ifndef _MPI_STUBS_H_
#define _MPI_STUBS_H_

/* Rename interface functions to avoid clashes with a real MPI library */
#define MPI_Allgather NO_MPI_Allgather
#define MPI_Allgatherv NO_MPI_Allgatherv
#define MPI_Allreduce NO_MPI_Allreduce
#define MPI_Alltoall NO_MPI_Alltoall
#define MPI_Alltoallv NO_MPI_Alltoallv
#define MPI_Barrier NO_MPI_Barrier
#define MPI_Bcast NO_MPI_Bcast
#define MPI_Bsend NO_MPI_Bsend
#define MPI_Bsend_init NO_MPI_Bsend_init
#define MPI_Cart_create NO_MPI_Cart_create
#define MPI_Comm_compare NO_MPI_Comm_compare
#define MPI_Comm_create NO_MPI_Comm_create
#define MPI_Comm_dup NO_MPI_Comm_dup
#define MPI_Comm_free NO_MPI_Comm_free
#define MPI_Comm_group NO_MPI_Comm_group
#define MPI_Comm_rank NO_MPI_Comm_rank
#define MPI_Comm_size NO_MPI_Comm_size
#define MPI_Comm_split NO_MPI_Comm_split
#define MPI_Error_string NO_MPI_Error_string
#define MPI_Finalize NO_MPI_Finalize
#define MPI_Finalized NO_MPI_Finalized
#define MPI_Gather NO_MPI_Gather
#define MPI_Gatherv NO_MPI_Gatherv
#define MPI_Get_count NO_MPI_Get_count
#define MPI_Get_processor_name NO_MPI_Get_processor_name
#define MPI_Group_free NO_MPI_Group_free
#define MPI_Group_incl NO_MPI_Group_incl
#define MPI_Group_translate_ranks NO_MPI_Group_translate_ranks
#define MPI_Init NO_MPI_Init
#define MPI_Init_thread NO_MPI_Init_thread
#define MPI_Initialized NO_MPI_Initialized
#define MPI_Iprobe NO_MPI_Iprobe
#define MPI_Irecv NO_MPI_Irecv
#define MPI_Isend NO_MPI_Isend
#define MPI_Issend NO_MPI_Issend
#define MPI_Op_create NO_MPI_Op_create
#define MPI_Op_free NO_MPI_Op_free
#define MPI_Probe NO_MPI_Probe
#define MPI_Query_thread NO_MPI_Query_thread
#define MPI_Recv NO_MPI_Recv
#define MPI_Recv_init NO_MPI_Recv_init
#define MPI_Reduce NO_MPI_Reduce
#define MPI_Send NO_MPI_Send
#define MPI_Send_init NO_MPI_Send_init
#define MPI_Sendrecv NO_MPI_Sendrecv
#define MPI_Ssend NO_MPI_Ssend
#define MPI_Ssend_init NO_MPI_Ssend_init
#define MPI_Start NO_MPI_Start
#define MPI_Startall NO_MPI_Startall
#define MPI_Testall NO_MPI_Testall
#define MPI_Testany NO_MPI_Testany
#define MPI_Test NO_MPI_Test
#define MPI_Testsome NO_MPI_Testsome
#define MPI_Type_commit NO_MPI_Type_commit
#define MPI_Type_contiguous NO_MPI_Type_contiguous
#define MPI_Type_free NO_MPI_Type_free
#define MPI_Type_get_extent NO_MPI_Type_get_extent
#define MPI_Waitall NO_MPI_Waitall
#define MPI_Waitany NO_MPI_Waitany
#define MPI_Wait NO_MPI_Wait
#define MPI_Waitsome NO_MPI_Waitsome
#define MPI_Wtime NO_MPI_Wtime

/* Define data types */
typedef int MPI_Comm;
typedef struct {
    int MPI_SOURCE;
    int MPI_TAG;
    int MPI_ERROR;
} MPI_Status;
typedef int MPI_Group;
typedef int MPI_Request;
typedef int MPI_Datatype;
typedef int MPI_Op;
typedef int MPI_Fint;
typedef long MPI_Aint;
typedef void (MPI_User_function)(void *, void *, int *, MPI_Datatype *);

/* Define constants */
#define MPI_COMM_NULL 0
#define MPI_COMM_WORLD 1
#define MPI_COMM_SELF 1

#define MPI_DATATYPE_NULL 0
#define MPI_CHAR 1
#define MPI_BYTE 1
#define MPI_SHORT 2
#define MPI_INT 3
#define MPI_LONG 4
#define MPI_FLOAT 5
#define MPI_DOUBLE 6
#define MPI_LONG_DOUBLE 7
#define MPI_UNSIGNED_CHAR 8
#define MPI_SIGNED_CHAR 9
#define MPI_UNSIGNED_SHORT 10
#define MPI_UNSIGNED 11
#define MPI_UNSIGNED_LONG 12
#define MPI_LONG_LONG 13
#define MPI_UNSIGNED_LONG_LONG 14
#define MPI_2INT 15
#define MPI_C_FLOAT_COMPLEX 16
#define MPI_C_DOUBLE_COMPLEX 17
#define MPI_C_LONG_DOUBLE_COMPLEX 18
#define MPI_DATATYPE_CUSTOM 100

#define MPI_OP_NULL 0
#define MPI_MAX 1
#define MPI_MIN 2
#define MPI_SUM 3
#define MPI_PROD 4
#define MPI_BAND 5
#define MPI_BOR 6
#define MPI_BXOR 7
#define MPI_MAXLOC 8
#define MPI_MINLOC 9
#define MPI_OP_CUSTOM 100

#define MPI_ANY_SOURCE 1

#define MPI_SUCCESS 0
#define MPI_ERR_COMM 1
#define MPI_ERR_UNKNOWN 2

#define MPI_UNEQUAL 1
#define MPI_IDENT 2
#define MPI_CONGRUENT 3

#define MPI_THREAD_SINGLE 1
#define MPI_THREAD_FUNNELED 2
#define MPI_THREAD_SERIALIZED 3
#define MPI_THREAD_MULTIPLE 4

#define MPI_IN_PLACE ((void *)1)
#define MPI_STATUS_IGNORE ((MPI_Status *)0)
#define MPI_STATUSES_IGNORE ((MPI_Status *)0)

#define MPI_MAX_PROCESSOR_NAME 256
#define MPI_MAX_ERROR_STRING 32

/* Declare interface functions */
#ifdef __cplusplus
#define MY_DECLSPEC extern "C"
#else
#define MY_DECLSPEC
#endif

MY_DECLSPEC int MPI_Allgather(void *sendbuf, int sendcount, MPI_Datatype sendtype,
                                 void *recvbuf, int recvcount,
                                 MPI_Datatype recvtype, MPI_Comm comm);
MY_DECLSPEC int MPI_Allgatherv(void *sendbuf, int sendcount, MPI_Datatype sendtype,
                                  void *recvbuf, int *recvcounts,
                                  int *displs, MPI_Datatype recvtype, MPI_Comm comm);
MY_DECLSPEC int MPI_Allreduce(void *sendbuf, void *recvbuf, int count,
                                 MPI_Datatype datatype, MPI_Op op, MPI_Comm comm);
MY_DECLSPEC int MPI_Alltoall(void *sendbuf, int sendcount, MPI_Datatype sendtype,
                                void *recvbuf, int recvcount,
                                MPI_Datatype recvtype, MPI_Comm comm);
MY_DECLSPEC int MPI_Alltoallv(void *sendbuf, int *sendcounts, int *sdispls,
                                 MPI_Datatype sendtype, void *recvbuf, int *recvcounts,
                                 int *rdispls, MPI_Datatype recvtype, MPI_Comm comm);
MY_DECLSPEC int MPI_Barrier(MPI_Comm comm);
MY_DECLSPEC int MPI_Bcast(void *buffer, int count, MPI_Datatype datatype,
                             int root, MPI_Comm comm);
MY_DECLSPEC int MPI_Bsend(void *buf, int count, MPI_Datatype datatype,
                             int dest, int tag, MPI_Comm comm);
MY_DECLSPEC int MPI_Bsend_init(void *buf, int count, MPI_Datatype datatype,
                                  int dest, int tag, MPI_Comm comm, MPI_Request *request);
MY_DECLSPEC int MPI_Cart_create(MPI_Comm oldcomm, int ndims, int *dims, int *periods, 
                                                int reorder, MPI_Comm *cartcomm);
MY_DECLSPEC int MPI_Comm_compare(MPI_Comm comm1, MPI_Comm comm2, int *result);
MY_DECLSPEC int MPI_Comm_create(MPI_Comm comm, MPI_Group group, MPI_Comm *newcomm);
MY_DECLSPEC int MPI_Comm_dup(MPI_Comm comm, MPI_Comm *newcomm);
MY_DECLSPEC int MPI_Comm_free(MPI_Comm *comm);
MY_DECLSPEC int MPI_Comm_group(MPI_Comm comm, MPI_Group *group);
MY_DECLSPEC int MPI_Comm_rank(MPI_Comm comm, int *rank);
MY_DECLSPEC int MPI_Comm_size(MPI_Comm comm, int *size);
MY_DECLSPEC int MPI_Comm_split(MPI_Comm comm, int color, int key, MPI_Comm *newcomm);
MY_DECLSPEC int MPI_Error_string(int errorcode, char *string, int *resultlen);
MY_DECLSPEC int MPI_Finalize(void);
MY_DECLSPEC int MPI_Finalized(int *flag);
MY_DECLSPEC int MPI_Gather(void *sendbuf, int sendcount, MPI_Datatype sendtype,
                              void *recvbuf, int recvcount, MPI_Datatype recvtype,
                              int root, MPI_Comm comm);
MY_DECLSPEC int MPI_Gatherv(void *sendbuf, int sendcount, MPI_Datatype sendtype,
                               void *recvbuf, int *recvcounts, int *displs,
                               MPI_Datatype recvtype, int root, MPI_Comm comm);
MY_DECLSPEC int MPI_Get_count(MPI_Status *status, MPI_Datatype datatype, int *count);
MY_DECLSPEC int MPI_Get_processor_name(char *name, int *resultlen);
MY_DECLSPEC int MPI_Group_free(MPI_Group *group);
MY_DECLSPEC int MPI_Group_incl(MPI_Group group, int n, int *ranks,
                                  MPI_Group *newgroup);
MY_DECLSPEC int MPI_Group_translate_ranks(MPI_Group group1, int n, int *ranks1,
                                             MPI_Group group2, int *ranks2);
MY_DECLSPEC int MPI_Init(int *argc, char ***argv);
MY_DECLSPEC int MPI_Init_thread(int *argc, char ***argv, int required,
                                   int *provided);
MY_DECLSPEC int MPI_Initialized(int *flag);
MY_DECLSPEC int MPI_Iprobe(int source, int tag, MPI_Comm comm, int *flag,
                              MPI_Status *status);
MY_DECLSPEC int MPI_Irecv(void *buf, int count, MPI_Datatype datatype, int source,
                             int tag, MPI_Comm comm, MPI_Request *request);
MY_DECLSPEC int MPI_Isend(void *buf, int count, MPI_Datatype datatype, int dest,
                             int tag, MPI_Comm comm, MPI_Request *request);
MY_DECLSPEC int MPI_Issend(void *buf, int count, MPI_Datatype datatype, int dest,
                              int tag, MPI_Comm comm, MPI_Request *request);
MY_DECLSPEC int MPI_Op_create(MPI_User_function *function, int commute, MPI_Op *op);
MY_DECLSPEC int MPI_Op_free(MPI_Op *op);
MY_DECLSPEC int MPI_Probe(int source, int tag, MPI_Comm comm, MPI_Status *status);
MY_DECLSPEC int MPI_Query_thread(int *provided);
MY_DECLSPEC int MPI_Recv(void *buf, int count, MPI_Datatype datatype, int source,
                            int tag, MPI_Comm comm, MPI_Status *status);
MY_DECLSPEC int MPI_Recv_init(void *buf, int count, MPI_Datatype datatype, int source,
                                 int tag, MPI_Comm comm, MPI_Request *request);
MY_DECLSPEC int MPI_Reduce(void *sendbuf, void *recvbuf, int count,
                              MPI_Datatype datatype, MPI_Op op, int root, MPI_Comm comm);
MY_DECLSPEC int MPI_Send(void *buf, int count, MPI_Datatype datatype, int dest,
                            int tag, MPI_Comm comm);
MY_DECLSPEC int MPI_Send_init(void *buf, int count, MPI_Datatype datatype,
                                 int dest, int tag, MPI_Comm comm,
                                 MPI_Request *request);
MY_DECLSPEC int MPI_Sendrecv(void *sendbuf, int sendcount, MPI_Datatype sendtype,
                                int dest, int sendtag, void *recvbuf, int recvcount,
                                MPI_Datatype recvtype, int source, int recvtag,
                                MPI_Comm comm,  MPI_Status *status);
MY_DECLSPEC int MPI_Ssend(void *buf, int count, MPI_Datatype datatype, int dest,
                             int tag, MPI_Comm comm);
MY_DECLSPEC int MPI_Ssend_init(void *buf, int count, MPI_Datatype datatype,
                                  int dest, int tag, MPI_Comm comm,
                                  MPI_Request *request);
MY_DECLSPEC int MPI_Start(MPI_Request *request);
MY_DECLSPEC int MPI_Startall(int count, MPI_Request *array_of_requests);
MY_DECLSPEC int MPI_Testall(int count, MPI_Request array_of_requests[], int *flag,
                               MPI_Status array_of_statuses[]);
MY_DECLSPEC int MPI_Testany(int count, MPI_Request array_of_requests[], int *index,
                               int *flag, MPI_Status *status);
MY_DECLSPEC int MPI_Test(MPI_Request *request, int *flag, MPI_Status *status);
MY_DECLSPEC int MPI_Testsome(int incount, MPI_Request array_of_requests[],
                                int *outcount, int array_of_indices[],
                                MPI_Status array_of_statuses[]);
MY_DECLSPEC int MPI_Type_commit(MPI_Datatype *type);
MY_DECLSPEC int MPI_Type_contiguous(int count, MPI_Datatype oldtype,
                                       MPI_Datatype *newtype);
MY_DECLSPEC int MPI_Type_free(MPI_Datatype *type);
MY_DECLSPEC int MPI_Type_get_extent(MPI_Datatype type, MPI_Aint *lb,
                                       MPI_Aint *extent);
MY_DECLSPEC int MPI_Waitall(int count, MPI_Request *array_of_requests,
                               MPI_Status *array_of_statuses);
MY_DECLSPEC int MPI_Waitany(int count, MPI_Request *array_of_requests,
                               int *index, MPI_Status *status);
MY_DECLSPEC int MPI_Wait(MPI_Request *request, MPI_Status *status);
MY_DECLSPEC int MPI_Waitsome(int incount, MPI_Request *array_of_requests,
                                int *outcount, int *array_of_indices,
                                MPI_Status *array_of_statuses);
MY_DECLSPEC double MPI_Wtime(void);

#endif
