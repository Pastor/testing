
/*  ------------------------------------------------------
     Routine name:  my_diagram
     Description:   
     Project file:  2.prt

------------------------------------------------------  */

/*  --- Base generator data types --- */
/* Real data type */
typedef double g_real_type;
/* Integer data type */
typedef int g_int_type;
/* Boolean data type */
typedef char g_boolean_type;
/* Complex data type */
typedef complex_64 g_complex_type;


/* Default initialization values */

/* Project signal database hash */
const unsigned int sp_database_hash_32=0;
/* Project sheme structure hash */
const unsigned int sp_sheme_hash_32=3931184425;

const double input_0_default=0;
const double input_1_default=0;
const double input_2_default=0;
const double out_0_default=0;
const double out_2_default=1;
const double my_diagramv0_out_0_default=0;
const double my_diagramv1_out_0_default=0;
const double my_diagramv2_out_0_default=0;
const double my_diagramv3_out_0_default=0;
const double my_diagramv4_out_0_default=0;
const double my_diagramv4_out_1_default=1;

/* Main structures defines */
/* External variables count */
#define ext_vars_count 5
/* Internal state variables count */
#define state_vars_count 6
/*  --- Source model preferences --- */
/* Minimum integration step */
#define INTEGRATION_MIN_STEP 1E-5
/* Maximum integration step */
#define INTEGRATION_MAX_STEP 1
/* Integration synchronization step */
#define INTEGRATION_SYNC_STEP 0
/* Model integration method */
#define INTEGRATION_METHOD 5
/* Model relative error */
#define INTEGRATION_RELATIVE_ERROR 0.0001
/* Model absolute error */
#define INTEGRATION_ABSOLUTE_ERROR 1E-6
/* Model end time */
#define INTEGRATION_END_TIME 10
/* Model maximum iteration count */
#define INTEGRATION_MAX_LOOP_ITER_COUNT 10


const ext_var_info_record ext_vars_names[ext_vars_count] = {
{"input:0",   vt_double,   {1}, 0,dir_input,"", (void*)&input_0_default, sizeof(double)} ,
{"input:1",   vt_double,   {1}, 1,dir_input,"", (void*)&input_1_default, sizeof(double)} ,
{"input:2",   vt_double,   {1}, 2,dir_input,"", (void*)&input_2_default, sizeof(double)} ,
{"out:0",   vt_double,   {1}, 3,dir_out,"", (void*)&out_0_default, sizeof(double)} ,
{"out:2",   vt_double,   {1}, 4,dir_out,"", (void*)&out_2_default, sizeof(double)} 
};
#define input_0 (*(double*)(ext_vars_addr[0]))
#define input_1 (*(double*)(ext_vars_addr[1]))
#define input_2 (*(double*)(ext_vars_addr[2]))
#define out_0 (*(double*)(ext_vars_addr[3]))
#define out_2 (*(double*)(ext_vars_addr[4]))

const ext_var_info_record state_vars_names[state_vars_count] = {
{"my_diagramv0_out_0",   vt_double,   {1}, 0, dir_inout,"Input pin state variable", (void*)&my_diagramv0_out_0_default, sizeof(double)} ,
{"my_diagramv1_out_0",   vt_double,   {1}, 8, dir_inout,"Input pin state variable", (void*)&my_diagramv1_out_0_default, sizeof(double)} ,
{"my_diagramv2_out_0",   vt_double,   {1}, 16, dir_inout,"Input pin state variable", (void*)&my_diagramv2_out_0_default, sizeof(double)} ,
{"my_diagramv3_out_0",   vt_double,   {1}, 24, dir_inout,"Input pin state variable", (void*)&my_diagramv3_out_0_default, sizeof(double)} ,
{"my_diagramv4_out_0",   vt_double,   {1}, 32, dir_inout,"Language out", (void*)&my_diagramv4_out_0_default, sizeof(double)} ,
{"my_diagramv4_out_1",   vt_double,   {1}, 40, dir_inout,"Language out", (void*)&my_diagramv4_out_1_default, sizeof(double)} 
};
   typedef struct {
double my_diagramv0_out_0_;
double my_diagramv1_out_0_;
double my_diagramv2_out_0_;
double my_diagramv3_out_0_;
double my_diagramv4_out_0_;
double my_diagramv4_out_1_;
   } t_state_vars;
  typedef char t_consts;
  typedef char t_local;
