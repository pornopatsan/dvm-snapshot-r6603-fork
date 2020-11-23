
enum {
	GSHADOW,
	GREMOTE,
	GREDISTRIBUTION,
	GREGIONIN,
	GNUMOP
};

struct GpuOpTime {             //������� �������� �������� ������ � gpu
	double gpu_to_cpu;
	double cpu_to_gpu;
	double gpu_to_gpu;
};

struct ThreadTime {            //������� ������ ����� ����
	double user_time;
	double sys_time;
};

struct GpuMetric {
    unsigned int countMeasures;  /**< количество измерений характеристики */
    double  timeProductive; /**< полезное время */
    double  timeLost;       /**< потерянное время */

    // -- Агрегированные значения (для box-диаграммы)
    double min;    /**< минимальное значение */
    double mean;   /**< среднее */
    double max;    /**< максимальное значение */
    double sum;    /**< сумма значений */
};

struct GpuTime {               //������� ������ gpu
	char * gpu_name;
	double prod_time;
    double lost_time;
//    double kernel_exec;
//	double loop_exec;
//	double get_actual;
//	double data_reorg;
//	double reduction;
//	double gpu_runtime_compilation;
//	double gpu_to_cpu;
//	double cpu_to_gpu;
//	double gpu_to_gpu;
//	struct GpuOpTime op_times[GNUMOP];
    struct GpuMetric metrics[DVMH_STAT_METRIC_FORCE_INT];
};

struct ColOp {                 //������� ���������� ������������ ��������
	long ncall;
	double comm;
	double synch;
	double real_comm;
	double time_var;
	double overlap;
};

struct OpGrp {
	double calls;
	double lost_time;
	double prod;
};

struct ProcTimes {
	double prod_cpu;
	double prod_sys;
	double prod_io;
	double exec_time;
	double sys_time;
	double real_comm;
	double lost_time;
	double insuf_user;
	double insuf_sys;
	double comm;
	double idle;
	double load_imb;
	double synch;
	double time_var;
	double overlap;
	double thr_user_time;
	double thr_sys_time;
	double gpu_time_prod;
	double gpu_time_lost;
	unsigned long num_threads;
	struct ThreadTime * th_times;
	unsigned long num_gpu;
	struct GpuTime *gpu_times;
	struct ColOp col_op[4];//4~RED
};

typedef struct OpGrp OpGrp;
typedef struct ProcTimes ProcTimes;
