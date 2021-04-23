/* ����������� ���������� */
#include <stdint.h>
#include <string.h>
#include "stdlib.h"
#include "math.h"
#include "windows.h"

#ifdef WIN32
#define malloc_usable_size _msize
#endif

/* ���������� ����� ��� ��������� */
#include "c_types.h"

/* ��������������� ��������� */
#include "prog.h"

/* ��� ������ ��� ������ ���������� ���� ������������� ��������� ��� �������� ����������
  n_ext_vars     - ���������� �-�� ����������� ������� ���������� (������� � ������)
  n_din_vars     - ���������� ������������ ���������� (������� � ������)
  n_alg_vars     - ���������� �������������� ���������� (������� � ������)
  n_state_vars   - �-�� ���������� ��������� (������� � ������)
  n_consts       - �-�� ��������
  sizeof_state_vars - ������ ������ �� ���������� ��������� (������ ���������� ���������� ����������)
  sizeof_consts  - ������ ������ ��� ���������
  sizeof_local_vars - ������ ������ ��� ��������� ����������
  din_vars_dim   - ��������� ����������� ������� ���������� ���������
  alg_vars_dim   - ---//--- �������������� ����������
  ext_vars_info - ���������� ��������� �� ������ ��� ������� ����������
  din_vars_info - ���������� ��������� �� ������ ��� ������������ ����������
  alg_vars_info - ���������� ��������� �� ������ ��� �������������� ����������
  state_vars_info - ���������� ��������� �� ������ ��� ���������� ���������
  const_info - ���������� ��������� �� ������ ��� �������� */
EXPORTED_FUNC INFO_FUNC(
							 int*    n_ext_vars, 
							 int*    n_din_vars, 
							 int*    n_alg_vars,
							 int*    n_state_vars,
							 int*    n_consts,
                                                         int*    sizeof_state_vars,
							 int*    sizeof_consts,
							 int*    sizeof_local_vars,
							 int*    din_vars_dim,
							 int*    alg_vars_dim,
							 void**  ext_vars_info, 
							 void**  din_vars_info, 
							 void**  alg_vars_info, 
							 void**  state_vars_info,
							 void**  const_info,
							 solver_struct* solver_data,
                                                         unsigned int* sheme_hash32,
                                                         char*   algo_name,
                                                         void**  algo_object_id
							 )
{  int ret; 
   ret=0;

   /* ������� ���������� (������) */
   #ifdef ext_vars_count
     *n_ext_vars    = ext_vars_count;
     *ext_vars_info = (void*)&ext_vars_names;
   #else
     *n_ext_vars    = 0;
	 *ext_vars_info = NULL;
   #endif

   /* ������������ ���������� */
   #ifdef din_vars_count
     *n_din_vars    = din_vars_count;
     *din_vars_info = (void*)&din_vars_names;
	 *din_vars_dim  = din_vars_dimension;
   #else
     *n_din_vars    = 0;
     *din_vars_info = NULL;
	 *din_vars_dim  = 0;
   #endif

    /* �������������� ���������� */
   #ifdef alg_vars_count
     *n_alg_vars    = alg_vars_count;
     *alg_vars_info = (void*)&alg_vars_names;
	 *alg_vars_dim  = alg_vars_dimension;
   #else
     *n_alg_vars    = 0;
     *alg_vars_info = NULL;	 
	 *alg_vars_dim  = 0;
   #endif

   /* ���������� ��������� */
   #ifdef state_vars_count
	 *n_state_vars = state_vars_count;
     *sizeof_state_vars = sizeof(t_state_vars);
	 *state_vars_info = (void*)&state_vars_names;
   #else
	 *n_state_vars = 0;
     *sizeof_state_vars = 0;
	 *state_vars_info = NULL;
   #endif

   #ifdef const_count
	 *n_consts = const_count;
     *sizeof_consts = sizeof(t_consts);
	 *const_info = (void*)&const_names;
   #else
	 *n_consts = 0;
     *sizeof_consts = 0;
	 *const_info = NULL;     
   #endif

    /* ��������� (���������) ���������� */
   #ifdef local_count
     *sizeof_local_vars = sizeof(t_local);
   #else
     *sizeof_local_vars = 0;
   #endif

   /* ����������� ����� �������� ����� �� ������� ��� ������������ ��� */
   *sheme_hash32 = sp_sheme_hash_32; 

   return ret;
};

/* ������� ��������� ���������� ���������
 step           - ��� �������
 modeltime      - ������� ��������� ����� (���� �� ������������ - �� ������ 0)
 ext_vars_addr  - ����� ������� ������� ������� ����������
 din_vars       - ����� ������� ������������ ���������� 
 derivates      - ����� ������� ����������� ������������ ����������
 alg_vars       - ����� ������� �������������� ����������
 alg_funcs      - ����� ������� �������� �������������� �������
 state_vars     - ����� ��������� � ����������� �����������
 consts         - ����� ��������� � ����������� */
EXPORTED_FUNC INIT_FUNC( double step,
						 double            modeltime,
						 void**            ext_vars_addr,
						 double*           din_vars,
						 double*           derivates,
						 double*           alg_vars,
						 double*           alg_funcs,
						 t_state_vars*     state_vars, 
						 t_consts*         consts,
             t_local*          locals,
						 solver_struct*    solver_data,
             void*             algo_object_id
					   )
{  int ret; 
   
   /* ��� �� ������ ���������� ��������� �������� */
   #include "init.inc"
      
   return ret;
};


/* �������������� �������  
action         - ������� ��� ������ �������
step           - ��� �������
modeltime      - ������� ��������� ����� (���� �� ������������ - �� ������ 0)
ext_vars_addr  - ����� ������� ������� ������� ����������
din_vars       - ����� ������� ������������ ���������� 
derivates      - ����� ������� ����������� ������������ ����������
alg_vars       - ����� ������� �������������� ����������
alg_funcs      - ����� ������� �������������� �������
state_vars     - ����� ��������� � ����������� �����������
consts         - ����� ��������� � �����������
locals         - ����� ��������� � ���������� ����������� */
EXPORTED_FUNC RUN_FUNC(
						 int               action,	
						 double            step,
						 double            modeltime,
						 void**            ext_vars_addr,
						 double*           din_vars,
						 double*           derivates,
						 double*           alg_vars,
						 double*           alg_funcs,
						 t_state_vars*     state_vars, 
						 t_consts*         consts,
						 t_local*          locals,
						 solver_struct*    solver_data,
                                                 void*             algo_object_id
					   )
{  int ret; 
  
   #include "prog.inc"
   
   return ret;
};

EXPORTED_FUNC STATE_FUNC(
						 int               action,	
						 double            step,
						 double            modeltime,
						 void**            ext_vars_addr,
						 double*           din_vars,
						 double*           derivates,
						 double*           alg_vars,
						 double*           alg_funcs,
						 t_state_vars*     state_vars, 
						 t_consts*         consts,
						 t_local*          locals,
						 solver_struct*    solver_data,
                                                 void*             algo_object_id
					   )
{  int ret; 
   
   #include "state.inc"
   
   return ret;
};

