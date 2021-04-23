/* Стандартные библиотеки */
#include <stdint.h>
#include <string.h>
#include "stdlib.h"
#include "math.h"
#include "windows.h"

#ifdef WIN32
#define malloc_usable_size _msize
#endif

/* Декларация типов для генерации */
#include "c_types.h"

/* Сгенерированный заголовок */
#include "prog.h"

/* Для начала при вызове библиотеки надо сгенерировать структуры для хранения переменных
  n_ext_vars     - возвращает к-во необходимых внешних переменных (записей в списке)
  n_din_vars     - количество динамических переменных (записей в списке)
  n_alg_vars     - количество алгебраических переменных (записей в списке)
  n_state_vars   - к-во переменных состояния (записей в списке)
  n_consts       - к-во констант
  sizeof_state_vars - размер памяти по переменные состояния (память выделяется упраляющей программой)
  sizeof_consts  - размер памяти под константы
  sizeof_local_vars - размер памяти под локальные переменные
  din_vars_dim   - суммарная размерность массива переменных состояния
  alg_vars_dim   - ---//--- алгебраических переменных
  ext_vars_info - возвращает указатель на список имён внешних переменных
  din_vars_info - возвращает указатель на список имён динамических переменных
  alg_vars_info - возвращает указатель на список имён алгебраических переменных
  state_vars_info - возвращает указатель на список имён переменных состояния
  const_info - возвращает указатель на список имён констант */
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

   /* Внешние переменные (адреса) */
   #ifdef ext_vars_count
     *n_ext_vars    = ext_vars_count;
     *ext_vars_info = (void*)&ext_vars_names;
   #else
     *n_ext_vars    = 0;
	 *ext_vars_info = NULL;
   #endif

   /* Динамические переменные */
   #ifdef din_vars_count
     *n_din_vars    = din_vars_count;
     *din_vars_info = (void*)&din_vars_names;
	 *din_vars_dim  = din_vars_dimension;
   #else
     *n_din_vars    = 0;
     *din_vars_info = NULL;
	 *din_vars_dim  = 0;
   #endif

    /* Алгебраические переменные */
   #ifdef alg_vars_count
     *n_alg_vars    = alg_vars_count;
     *alg_vars_info = (void*)&alg_vars_names;
	 *alg_vars_dim  = alg_vars_dimension;
   #else
     *n_alg_vars    = 0;
     *alg_vars_info = NULL;	 
	 *alg_vars_dim  = 0;
   #endif

   /* Переменные состояния */
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

    /* Локальные (временные) переменные */
   #ifdef local_count
     *sizeof_local_vars = sizeof(t_local);
   #else
     *sizeof_local_vars = 0;
   #endif

   /* Контрольная сумма исходной схемы по которой был сгенерирован код */
   *sheme_hash32 = sp_sheme_hash_32; 

   return ret;
};

/* Функция установки начального состояния
 step           - шаг расчёта
 modeltime      - текущее модельное время (если не используется - то всегда 0)
 ext_vars_addr  - адрес массива адресов внешних переменных
 din_vars       - адрес массива динамических переменных 
 derivates      - адрес массива производных динамических переменных
 alg_vars       - адрес массива алгебраических переменных
 alg_funcs      - адрес массива значений алгебраических функций
 state_vars     - адрес структуры с внутренними переменными
 consts         - адрес структуры с константами */
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
   
   /* Тут мы делаем присвоение начальных значений */
   #include "init.inc"
      
   return ret;
};


/* Исполнительная функция  
action         - текущий тип вызова функции
step           - шаг расчёта
modeltime      - текущее модельное время (если не используется - то всегда 0)
ext_vars_addr  - адрес массива адресов внешних переменных
din_vars       - адрес массива динамических переменных 
derivates      - адрес массива производных динамических переменных
alg_vars       - адрес массива алгебраических переменных
alg_funcs      - адрес массива алгебраических функций
state_vars     - адрес структуры с внутренними переменными
consts         - адрес структуры с константами
locals         - адрес структуры с временными переменными */
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

