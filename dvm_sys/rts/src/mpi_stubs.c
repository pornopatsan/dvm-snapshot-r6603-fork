#include "mpi_stubs.h"

#include <stdlib.h>
#include <assert.h>
#include <stdio.h>
#include <string.h>
#ifdef _UNIX_
#include <time.h>
#include <sys/time.h>
#else
#include <windows.h>
#endif

static int initialized = 0;
static int finalized = 0;

static void fillStatus(MPI_Status *status)
{
    status->MPI_ERROR = MPI_SUCCESS;
    status->MPI_SOURCE = 0;
    status->MPI_TAG = 0;
}

static int sizeofmpi(MPI_Datatype dt)
{
    switch (dt) {
        case MPI_CHAR:
        case MPI_UNSIGNED_CHAR:
        case MPI_SIGNED_CHAR:
            return sizeof(char);
        case MPI_SHORT:
        case MPI_UNSIGNED_SHORT:
            return sizeof(short);
        case MPI_INT:
        case MPI_UNSIGNED:
            return sizeof(int);
        case MPI_LONG:
        case MPI_UNSIGNED_LONG:
            return sizeof(long);
        case MPI_LONG_LONG:
        case MPI_UNSIGNED_LONG_LONG:
            return sizeof(long long);
        case MPI_FLOAT:
            return sizeof(float);
        case MPI_DOUBLE:
            return sizeof(double);
        case MPI_LONG_DOUBLE:
            return sizeof(long double);
        case MPI_2INT:
            return 2 * sizeof(int);
        case MPI_C_FLOAT_COMPLEX:
            return 2 * sizeof(float);
        case MPI_C_DOUBLE_COMPLEX:
            return 2 * sizeof(double);
        case MPI_C_LONG_DOUBLE_COMPLEX:
            return 2 * sizeof(long double);
    }
    if (dt > MPI_DATATYPE_CUSTOM)
        return dt - MPI_DATATYPE_CUSTOM;
    assert(0);
    return 1;
}

MY_DECLSPEC int MPI_Allgather(void *sendbuf, int sendcount, MPI_Datatype sendtype,
                                 void *recvbuf, int recvcount,
                                 MPI_Datatype recvtype, MPI_Comm comm)
{
    int count = sendcount * sizeofmpi(sendtype);
    if (count > recvcount * sizeofmpi(recvtype))
        count = recvcount * sizeofmpi(recvtype);
    if (sendbuf != MPI_IN_PLACE)
        memcpy(recvbuf, sendbuf, count);
    return MPI_SUCCESS;
}

MY_DECLSPEC int MPI_Allgatherv(void *sendbuf, int sendcount, MPI_Datatype sendtype,
                                  void *recvbuf, int *recvcounts,
                                  int *displs, MPI_Datatype recvtype, MPI_Comm comm)
{
    int count = sendcount * sizeofmpi(sendtype);
    if (count > recvcounts[0] * sizeofmpi(recvtype))
        count = recvcounts[0] * sizeofmpi(recvtype);
    if (sendbuf != MPI_IN_PLACE)
        memcpy(recvbuf, sendbuf, count);
    return MPI_SUCCESS;
}

MY_DECLSPEC int MPI_Allreduce(void *sendbuf, void *recvbuf, int count,
                                 MPI_Datatype datatype, MPI_Op op, MPI_Comm comm)
{
    if (sendbuf != MPI_IN_PLACE)
        memcpy(recvbuf, sendbuf, count * sizeofmpi(datatype));
    return MPI_SUCCESS;
}

MY_DECLSPEC int MPI_Alltoall(void *sendbuf, int sendcount, MPI_Datatype sendtype,
                                void *recvbuf, int recvcount,
                                MPI_Datatype recvtype, MPI_Comm comm)
{
    int count = sendcount * sizeofmpi(sendtype);
    if (count > recvcount * sizeofmpi(recvtype))
        count = recvcount * sizeofmpi(recvtype);
    if (sendbuf != MPI_IN_PLACE)
        memcpy(recvbuf, sendbuf, count);
    return MPI_SUCCESS;
}

MY_DECLSPEC int MPI_Alltoallv(void *sendbuf, int *sendcounts, int *sdispls,
                                 MPI_Datatype sendtype, void *recvbuf, int *recvcounts,
                                 int *rdispls, MPI_Datatype recvtype, MPI_Comm comm)
{
    int count = sendcounts[0] * sizeofmpi(sendtype);
    if (count > recvcounts[0] * sizeofmpi(recvtype))
        count = recvcounts[0] * sizeofmpi(recvtype);
    if (sendbuf != MPI_IN_PLACE)
        memcpy(recvbuf, sendbuf, count);
    return MPI_SUCCESS;
}

MY_DECLSPEC int MPI_Barrier(MPI_Comm comm)
{
    return (comm != MPI_COMM_NULL ? MPI_SUCCESS : MPI_ERR_COMM);
}

MY_DECLSPEC int MPI_Bcast(void *buffer, int count, MPI_Datatype datatype,
                             int root, MPI_Comm comm)
{
    return (comm != MPI_COMM_NULL ? MPI_SUCCESS : MPI_ERR_COMM);
}

MY_DECLSPEC int MPI_Bsend(void *buf, int count, MPI_Datatype datatype,
                             int dest, int tag, MPI_Comm comm)
{
    return MPI_ERR_UNKNOWN;
}

MY_DECLSPEC int MPI_Bsend_init(void *buf, int count, MPI_Datatype datatype,
                                  int dest, int tag, MPI_Comm comm, MPI_Request *request)
{
    return MPI_ERR_UNKNOWN;
}

MY_DECLSPEC int MPI_Cart_create(MPI_Comm oldcomm, int ndims, int *dims, int *periods, int reorder, MPI_Comm *cartcomm) 
{
    *cartcomm = oldcomm;
    return MPI_SUCCESS;
}

MY_DECLSPEC int MPI_Comm_compare(MPI_Comm comm1, MPI_Comm comm2, int *result) {
    if (comm1 == comm2)
        *result = MPI_IDENT;
    else
        *result = MPI_UNEQUAL;
    return MPI_SUCCESS;
}

MY_DECLSPEC int MPI_Comm_create(MPI_Comm comm, MPI_Group group, MPI_Comm *newcomm)
{
    *newcomm = comm;
    return MPI_SUCCESS;
}

MY_DECLSPEC int MPI_Comm_dup(MPI_Comm comm, MPI_Comm *newcomm)
{
    *newcomm = comm;
    return MPI_SUCCESS;
}

MY_DECLSPEC int MPI_Comm_free(MPI_Comm *comm)
{
    return MPI_SUCCESS;
}

MY_DECLSPEC int MPI_Comm_group(MPI_Comm comm, MPI_Group *group)
{
    *group = 1;
    return MPI_SUCCESS;
}

MY_DECLSPEC int MPI_Comm_rank(MPI_Comm comm, int *rank)
{
    *rank = 0;
    return MPI_SUCCESS;
}

MY_DECLSPEC int MPI_Comm_size(MPI_Comm comm, int *size)
{
    if (comm != MPI_COMM_NULL) {
        *size = 1;
        return MPI_SUCCESS;
    } else {
        return MPI_ERR_COMM;
    }
}

MY_DECLSPEC int MPI_Comm_split(MPI_Comm comm, int color, int key, MPI_Comm *newcomm)
{
    *newcomm = comm;
    return MPI_SUCCESS;
}

MY_DECLSPEC int MPI_Error_string(int errorcode, char *string, int *resultlen)
{
    if (errorcode == MPI_SUCCESS)
        strcpy(string, "SUCCESS");
    else
        sprintf(string, "ERROR #%d", errorcode);
    *resultlen = strlen(string);
    return MPI_SUCCESS;
}

MY_DECLSPEC int MPI_Finalize(void)
{
    finalized = 1;
    return MPI_SUCCESS;
}

MY_DECLSPEC int MPI_Finalized(int *flag)
{
    *flag = finalized;
    return MPI_SUCCESS;
}

MY_DECLSPEC int MPI_Gather(void *sendbuf, int sendcount, MPI_Datatype sendtype,
                              void *recvbuf, int recvcount, MPI_Datatype recvtype,
                              int root, MPI_Comm comm)
{
    int count = sendcount * sizeofmpi(sendtype);
    if (count > recvcount * sizeofmpi(recvtype))
        count = recvcount * sizeofmpi(recvtype);
    if (sendbuf != MPI_IN_PLACE)
        memcpy(recvbuf, sendbuf, count);
    return MPI_SUCCESS;
}

MY_DECLSPEC int MPI_Gatherv(void *sendbuf, int sendcount, MPI_Datatype sendtype,
                               void *recvbuf, int *recvcounts, int *displs,
                               MPI_Datatype recvtype, int root, MPI_Comm comm)
{
    int count = sendcount * sizeofmpi(sendtype);
    if (count > recvcounts[0] * sizeofmpi(recvtype))
        count = recvcounts[0] * sizeofmpi(recvtype);
    if (sendbuf != MPI_IN_PLACE)
        memcpy(recvbuf, sendbuf, count);
    return MPI_SUCCESS;
}

MY_DECLSPEC int MPI_Get_count(MPI_Status *status, MPI_Datatype datatype, int *count)
{
    return MPI_ERR_UNKNOWN;
}

MY_DECLSPEC int MPI_Get_processor_name(char *name, int *resultlen)
{
    const char *actualName = "Unknown";
    strcpy(name, actualName);
    *resultlen = strlen(actualName);
    return MPI_SUCCESS;
}

MY_DECLSPEC int MPI_Group_free(MPI_Group *group)
{
    *group = 0;
    return MPI_SUCCESS;
}

MY_DECLSPEC int MPI_Group_incl(MPI_Group group, int n, int *ranks,
                                  MPI_Group *newgroup)
{
    *newgroup = group + 1;
    return MPI_SUCCESS;
}

MY_DECLSPEC int MPI_Group_translate_ranks(MPI_Group group1, int n, int *ranks1,
                                             MPI_Group group2, int *ranks2)
{
    int i;
    for (i = 0; i < n; i++)
        ranks2[i] = ranks1[i];
    return MPI_SUCCESS;
}

MY_DECLSPEC int MPI_Init(int *argc, char ***argv)
{
    initialized = 1;
    return MPI_SUCCESS;
}

MY_DECLSPEC int MPI_Init_thread(int *argc, char ***argv, int required,
                                   int *provided)
{
    *provided = required;
    return MPI_SUCCESS;
}

MY_DECLSPEC int MPI_Initialized(int *flag)
{
    *flag = initialized;
    return MPI_SUCCESS;
}

MY_DECLSPEC int MPI_Iprobe(int source, int tag, MPI_Comm comm, int *flag,
                              MPI_Status *status)
{
    return MPI_ERR_UNKNOWN;
}

MY_DECLSPEC int MPI_Irecv(void *buf, int count, MPI_Datatype datatype, int source,
                             int tag, MPI_Comm comm, MPI_Request *request)
{
    return MPI_ERR_UNKNOWN;
}

MY_DECLSPEC int MPI_Isend(void *buf, int count, MPI_Datatype datatype, int dest,
                             int tag, MPI_Comm comm, MPI_Request *request)
{
    return MPI_ERR_UNKNOWN;
}

MY_DECLSPEC int MPI_Issend(void *buf, int count, MPI_Datatype datatype, int dest,
                              int tag, MPI_Comm comm, MPI_Request *request)
{
    return MPI_ERR_UNKNOWN;
}

MY_DECLSPEC int MPI_Op_create(MPI_User_function *function, int commute, MPI_Op *op)
{
    (void)function;
    (void)commute;
    *op = MPI_OP_CUSTOM;
    return MPI_SUCCESS;
}

MY_DECLSPEC int MPI_Op_free(MPI_Op *op)
{
    *op = MPI_OP_NULL;
    return MPI_SUCCESS;
}

MY_DECLSPEC int MPI_Probe(int source, int tag, MPI_Comm comm, MPI_Status *status)
{
    return MPI_ERR_UNKNOWN;
}

MY_DECLSPEC int MPI_Query_thread(int *provided)
{
    *provided = MPI_THREAD_MULTIPLE;
    return MPI_SUCCESS;
}

MY_DECLSPEC int MPI_Recv(void *buf, int count, MPI_Datatype datatype, int source,
                            int tag, MPI_Comm comm, MPI_Status *status)
{
    return MPI_ERR_UNKNOWN;
}

MY_DECLSPEC int MPI_Recv_init(void *buf, int count, MPI_Datatype datatype, int source,
                                 int tag, MPI_Comm comm, MPI_Request *request)
{
    return MPI_ERR_UNKNOWN;
}

MY_DECLSPEC int MPI_Reduce(void *sendbuf, void *recvbuf, int count,
                              MPI_Datatype datatype, MPI_Op op, int root, MPI_Comm comm)
{
    if (sendbuf != MPI_IN_PLACE)
        memcpy(recvbuf, sendbuf, count * sizeofmpi(datatype));
    return MPI_SUCCESS;
}

MY_DECLSPEC int MPI_Send(void *buf, int count, MPI_Datatype datatype, int dest,
                            int tag, MPI_Comm comm)
{
    return MPI_ERR_UNKNOWN;
}

MY_DECLSPEC int MPI_Send_init(void *buf, int count, MPI_Datatype datatype,
                                 int dest, int tag, MPI_Comm comm,
                                 MPI_Request *request)
{
    return MPI_ERR_UNKNOWN;
}

MY_DECLSPEC int MPI_Sendrecv(void *sendbuf, int sendcount, MPI_Datatype sendtype,
                                int dest, int sendtag, void *recvbuf, int recvcount,
                                MPI_Datatype recvtype, int source, int recvtag,
                                MPI_Comm comm,  MPI_Status *status)
{
    return MPI_ERR_UNKNOWN;
}

MY_DECLSPEC int MPI_Ssend(void *buf, int count, MPI_Datatype datatype, int dest,
                             int tag, MPI_Comm comm)
{
    return MPI_ERR_UNKNOWN;
}

MY_DECLSPEC int MPI_Ssend_init(void *buf, int count, MPI_Datatype datatype,
                                  int dest, int tag, MPI_Comm comm,
                                  MPI_Request *request)
{
    return MPI_ERR_UNKNOWN;
}

MY_DECLSPEC int MPI_Start(MPI_Request *request)
{
    return MPI_SUCCESS;
}

MY_DECLSPEC int MPI_Startall(int count, MPI_Request *array_of_requests)
{
    return MPI_SUCCESS;
}

MY_DECLSPEC int MPI_Testall(int count, MPI_Request array_of_requests[], int *flag,
                               MPI_Status array_of_statuses[])
{
    *flag = 1;
    return MPI_Waitall(count, array_of_requests, array_of_statuses);
}

MY_DECLSPEC int MPI_Testany(int count, MPI_Request array_of_requests[], int *index,
                               int *flag, MPI_Status *status)
{
    *flag = 1;
    return MPI_Waitany(count, array_of_requests, index, status);
}

MY_DECLSPEC int MPI_Test(MPI_Request *request, int *flag, MPI_Status *status)
{
    *flag = 1;
    return MPI_Wait(request, status);
}

MY_DECLSPEC int MPI_Testsome(int incount, MPI_Request array_of_requests[],
                                int *outcount, int array_of_indices[],
                                MPI_Status array_of_statuses[])
{
    return MPI_Waitsome(incount, array_of_requests, outcount, array_of_indices, array_of_statuses);
}

MY_DECLSPEC int MPI_Type_commit(MPI_Datatype *type)
{
    (void)type;
    return MPI_SUCCESS;
}

MY_DECLSPEC int MPI_Type_contiguous(int count, MPI_Datatype oldtype,
                                       MPI_Datatype *newtype)
{
    *newtype = MPI_DATATYPE_CUSTOM + count * sizeofmpi(oldtype);
    return MPI_SUCCESS;
}

MY_DECLSPEC int MPI_Type_free(MPI_Datatype *type)
{
    *type = MPI_DATATYPE_NULL;
    return MPI_SUCCESS;
}

MY_DECLSPEC int MPI_Type_get_extent(MPI_Datatype type, MPI_Aint *lb,
                                       MPI_Aint *extent)
{
    *lb = 0;
    *extent = sizeofmpi(type);
    return MPI_SUCCESS;
}

MY_DECLSPEC int MPI_Waitall(int count, MPI_Request *array_of_requests,
                               MPI_Status *array_of_statuses)
{
    if (array_of_statuses != MPI_STATUSES_IGNORE) {
        int i;
        for (i = 0; i < count; i++)
            fillStatus(&array_of_statuses[i]);
    }
    return MPI_SUCCESS;
}

MY_DECLSPEC int MPI_Waitany(int count, MPI_Request *array_of_requests,
                               int *index, MPI_Status *status)
{
    *index = 0;
    if (status != MPI_STATUS_IGNORE)
        fillStatus(status);
    return MPI_SUCCESS;
}

MY_DECLSPEC int MPI_Wait(MPI_Request *request, MPI_Status *status)
{
    if (status != MPI_STATUS_IGNORE)
        fillStatus(status);
    return MPI_SUCCESS;
}

MY_DECLSPEC int MPI_Waitsome(int incount, MPI_Request *array_of_requests,
                                int *outcount, int *array_of_indices,
                                MPI_Status *array_of_statuses)
{
    int i;
    *outcount = incount;
    for (i = 0; i < incount; i++) {
        array_of_indices[i] = i;
        if (array_of_statuses != MPI_STATUSES_IGNORE)
            fillStatus(&array_of_statuses[i]);
    }
    return MPI_SUCCESS;
}

MY_DECLSPEC double MPI_Wtime(void)
{
#ifdef _UNIX_
    struct timeval tv;
    gettimeofday(&tv, 0);
    return tv.tv_sec + 1e-6 * tv.tv_usec;
#else
    LARGE_INTEGER frequency, measuredTime;
    QueryPerformanceFrequency(&frequency); 
    QueryPerformanceCounter(&measuredTime);
    return double(measuredTime.QuadPart) / frequency.QuadPart;
#endif
}
