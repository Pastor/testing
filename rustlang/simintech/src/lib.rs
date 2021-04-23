#![allow(unused_variables, unused_must_use, dead_code)]
extern crate libc;

use core::mem;
use libc::size_t;
use std::cell::RefCell;
use std::ffi::c_void;
use std::os::raw::{c_char, c_double, c_int};
use std::ptr::null;

const VT_DOUBLE: i8 = 0;
const VT_BOOL: i8 = 1;
const VT_INT: i8 = 2;
const VT_POINTER: i8 = 3;

/* Направление переменной */
const DIR_INPUT: i8 = 0;
const DIR_OUTPUT: i8 = 1;
const DIR_INOUT: i8 = 2;

thread_local! {
    static EXT_VARS_NAMES: RefCell<Vec<ExtVarInfoRecord>> = RefCell::new(Vec::new());
    static STATE_VARS_NAMES: RefCell<Vec<ExtVarInfoRecord>> = RefCell::new(Vec::new());

    static INPUT_0_DEFAULT: RefCell<c_double> = RefCell::new(0.);
    static INPUT_1_DEFAULT: RefCell<c_double> = RefCell::new(0.);
    static INPUT_2_DEFAULT: RefCell<c_double> = RefCell::new(0.);

    static OUTPUT_1_DEFAULT: RefCell<c_double> = RefCell::new(0.);
    static OUTPUT_2_DEFAULT: RefCell<c_double> = RefCell::new(0.);

    static SCHEMA_HASH: RefCell<u32> = RefCell::new(3931184425);

    static STATE_0_DEFAULT: RefCell<c_double> = RefCell::new(0.);
    static STATE_1_DEFAULT: RefCell<c_double> = RefCell::new(0.);
    static STATE_2_DEFAULT: RefCell<c_double> = RefCell::new(0.);
    static STATE_3_DEFAULT: RefCell<c_double> = RefCell::new(0.);
    static STATE_4_DEFAULT: RefCell<c_double> = RefCell::new(0.);
    static STATE_5_DEFAULT: RefCell<c_double> = RefCell::new(0.);

}

#[repr(u8)]
enum Action {
    InitState = 1,
    /* Запись начальных состояний */
    UpdateOuts = 2,
    /* Обновить выходы на предварительном шаге */
    GoodStep = 3,
    /* Обновить выходы на "хорошем" шаге */
    GetDeri = 4,
    /* Вычислить значения правых частей дифференциальных уравнений */
    GetAlgFun = 5,
    /* Вычислить значения правых частей алгебраических уравнений */
    SetState = 6,
    /* Вычислить значения дискретных переменных состояния (после шага интегрирования) */
    UpdateProps = 7,
    /* Обновить список параметров (с учётом флага изменяемости) */
    GetJacobyState = 8,
    /* Вычислить значения дискретных переменных состояния при расчете Якобиана */
    UpdateJacoby = 9,
    /* Обновить Якобиан блока */
    RestoreOuts = 10,
    /* Обновить выходы после рестарта (только если очень надо, т.к. выходы всё равно будут запоминаться) */
    SetAlgOut = 11,
    /* Выставить выходы блока, содержащих алгебраические переменные */
    InitAlgState = 12,
    /* Выставить начальное приближение для алгебраические переменных */
    Stop = 13,
    /* Вызывается при остановке расчёта (конец моделирования) */
    InitObjects = 14,
    /* Инициализация объектов, массивов и т.д. (сразу после сортировки) (начало моделирования) */
}

impl From<c_int> for Action {
    fn from(i: c_int) -> Self {
        match i {
            1 => Action::InitState,
            2 => Action::UpdateOuts,
            3 => Action::GoodStep,
            4 => Action::GetDeri,
            5 => Action::GetAlgFun,
            6 => Action::SetState,
            7 => Action::UpdateProps,
            8 => Action::GetJacobyState,
            9 => Action::UpdateJacoby,
            10 => Action::RestoreOuts,
            11 => Action::SetAlgOut,
            12 => Action::InitAlgState,
            13 => Action::Stop,
            14 => Action::InitObjects,
            _ => Action::Stop,
        }
    }
}

#[repr(C)]
#[derive(Debug, Copy, Clone)]
pub struct TypeState {
    v_: c_double,
    v1: c_double,
    v2: c_double,
    v3: c_double,
    o1: c_double,
    o2: c_double,
}

type TypeConst = c_char;
type TypeLocal = c_char;

#[repr(C)]
#[derive(Debug, Copy, Clone)]
pub struct ExtVarInfoRecord {
    name: *const u8,
    data_type: c_int,
    dim: [c_int; 3],
    index: c_int,
    direction: c_int,
    description: *const u8,
    default_ptr: *mut c_void,
    data_size: c_int,
}

/* Описание структуры для доступа к специальным переменным и методам решателя */
#[repr(C)]
#[derive(Debug, Copy, Clone)]
pub struct SolverStruct {
    layer_context: *const c_void,
    /* Указатель на контекст решателя */
    /* Параметры интегрирования - передаются извне */
    int_met: c_char,
    /* Метод интегрирования */
    loop_met: c_char,
    /* Метод решения системы НАУ */
    is_loop: c_char,
    /* Флаг метода интегрирования (явный или неявный) - для явных - True */
    max_loop_it: c_int,
    /* Максимальное число итераций при решении системы НАУ */
    abs_err: c_double,
    /* Абсолютная ошибка */
    rel_err: c_double,
    /* Относительная ошибка */
    h_min: c_double,
    /* Минимальный шаг интегрирования */
    h_max: c_double,
    /* Максимальный шаг интегрирования */
    /*Параметры, задающие тип синхронизации расчета */
    f_precition: c_char,
    /* Флаг точной синхронизации */
    f_one_step: c_char,
    /* Флаг выполнения одного шага интегрирования */
    f_first_step: c_char,
    /* Флаг первого шага расчёта */
    /*Переменные управления решателем */
    new_step: c_double,
    /* Новый прогнозный шаг интегрирования */
    f_set_step: c_char,
    /* Флаг - установить новый шаг интегрирования */
    /* Эти методы используются для того чтобы зарегситрировать и удалить
    специализированные объекты схемы (например распределённый решатель лин. уравнений)
    Найти глобальный объект по имени    */
    find_global_object:
    extern "C" fn(layer_context: *const c_void, global_object_name: *const u8) -> *const c_void,
    /* Зарегистрировать новый глобальный объект */
    register_global_object: extern "C" fn(
        layer_context: *const c_void,
        global_object_name: *const u8,
        new_object_name: *const c_void,
        destructor: extern "C" fn(*const c_void) -> c_int,
    ) -> *const c_void,
    /* Регистрация нужной библиотеки и получение от неё функций */
    do_load_need_plugin: extern "C" fn(plugin_name: *const u8) -> *const c_int,
    /* Флаг необходимости повторного шага  */
    f_need_iter: c_char,
    /* Контрольная сумма структуры текущей схемы (для проверки совпадения со сгенерированным кодом) */
    scheme_hash: c_int,
    /* Ссылка на глобальные списки именованных зависимостей и соотв-й хеш-лист */
    global_dep_list: *const c_void,
    global_dep_hash: *const c_void,
    /* Флаг учёта расширенных зависимостей сортировки для блоков чтения и записи сигналов */
    use_signal_extended_sort: c_char,
    /* Флаг - ошибка при наличии алгебраических петель образованных блоками чтения и записи сигналов */
    error_on_signal_loop: c_char,
    /* Учёт зависимостей сортировки блоков типа "Условие выполенния субмодели" */
    use_conditions_extended_sort: c_char,
    /* Текущий список зависимостей для записи дополнительных данных */
    current_dep_list: *const c_void,
    /* Ссылка на список замены портов для оптимизации передачи данных */
    global_where_with_list: *const c_void,
    global_where_with_hash: *const c_void,
    /* Использовать замену портов при оптимизации связок "чтение сигналов"-"запись сигналов" */
    use_signals_port_reconnection: c_char,
    /* Флаг режима проверки констант */
    f_constant_check_mode: c_char,
    /* Флаг режима генерации кода - для блокировки работы некоторых блоков */
    f_code_gen_mode: c_char,
    /* Указатель контекста задачи, необходимый для некоторых функций */
    task_context: *const c_void,
    /* Функция поиска указателя на данные по имени объекта, возврат = тип данных, и указатель */
    get_data_ptr: extern "C" fn(
        task_context: *const c_void,
        signal_name: *const u8,
        data_ptr: *mut *mut c_void,
        dimension: *mut c_int,
    ) -> c_char,
    /* Проверка необходимости принудительной остановки  */
    stop_check: extern "C" fn(task_context: *const c_void) -> c_char,
    /* Флаг - блоки записи выполнять только на шаге синхронизации */
    write_signals_on_sync_step: c_char,
    /* Флаг - транслировать сигналы из\в внешней исполнительной системы */
    common_translation_flag: c_char,
    /* Указатель на глобальную переменную текущего времени */
    f_current_time: *const c_double,
    /* Указатель на глобальную переменную - текущий шаг решателя */
    f_current_step: *const c_double,
    /* Флаг - необходимо перезапустить расчёт с нулевой точки заново */
    f_start_again: c_char,
    /* Флаг - необходимо запомнить стартовое состояние модели */
    f_save_model_state: c_char,
    /* Флаг - нужно перезаписывать значения сигналов на выходах при вызове f_InitState */
    f_write_signals_on_init_state: c_char,
    /* Флаг - использовать контроль точности для алгебраических переменных для DIRK и явных методов */
    use_alg_vars_step_control: c_char,
    /* Количество текущих локальных итераций (для распределённого анализа точности) */
    n_local_iter: c_int,
    /* Флаг - делать дополнительные пробные шаги для внутренних итераций модели
    При наличии блоков анализа точности и повтора расчёта надо выставить этот флаг, чтобы
    перед выполнением f_GoodStep делался f_UpdateOuts с тем же шагом интегрирования  */
    f_need_update_outs_before_good_step: c_char,
    /* Флаг - использовать уточнение шага для разрывных источников сигнала */
    f_precise_src_step: c_char,
    /* Имя библиотеки решения разреженной СЛАУ  */
    default_lae_solver_library_name: *const u8,
}

#[no_mangle]
pub extern "C" fn tick(c: size_t) -> size_t {
    c + 1
}

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
#[allow(non_snake_case)]
#[no_mangle]
pub extern "C" fn INFO_FUNC(
    n_ext_vars: *mut c_int,
    n_din_vars: *mut c_int,
    n_alg_vars: *mut c_int,
    n_state_vars: *mut c_int,
    n_consts: *mut c_int,
    sizeof_state_vars: *mut c_int,
    sizeof_consts: *mut c_int,
    sizeof_local_vars: *mut c_int,
    din_vars_dim: *mut c_int,
    alg_vars_dim: *mut c_int,
    ext_vars_info: *mut *mut c_void,
    din_vars_info: *mut *mut c_void,
    alg_vars_info: *mut *mut c_void,
    state_vars_info: *mut *mut TypeState,
    const_info: *mut *mut c_void,
    solver_data: *mut SolverStruct,
    scheme_hash32: *mut c_int,
    algo_name: *const u8,
    algo_object_id: *mut *mut c_void,
) -> c_int {
    EXT_VARS_NAMES.try_with(|e| unsafe {
        let mut record = e.borrow_mut();
        record.clear();

        INPUT_0_DEFAULT.try_with(|input| {
            record.push(ExtVarInfoRecord {
                name: "input:0".as_ptr(),
                data_type: VT_DOUBLE as c_int,
                dim: [1, 1, 1],
                index: 0,
                direction: DIR_INPUT as c_int,
                description: null(),
                default_ptr: input.as_ptr().cast(),
                data_size: mem::size_of::<c_double>() as c_int,
            });
        });

        INPUT_1_DEFAULT.try_with(|input| {
            record.push(ExtVarInfoRecord {
                name: "input:1".as_ptr(),
                data_type: VT_DOUBLE as c_int,
                dim: [1, 1, 1],
                index: 1,
                direction: DIR_INPUT as c_int,
                description: null(),
                default_ptr: input.as_ptr().cast(),
                data_size: mem::size_of::<c_double>() as c_int,
            });
        });

        INPUT_2_DEFAULT.try_with(|input| {
            record.push(ExtVarInfoRecord {
                name: "input:2".as_ptr(),
                data_type: VT_DOUBLE as c_int,
                dim: [1, 1, 1],
                index: 2,
                direction: DIR_INPUT as c_int,
                description: null(),
                default_ptr: input.as_ptr().cast(),
                data_size: mem::size_of::<c_double>() as c_int,
            });
        });

        OUTPUT_1_DEFAULT.try_with(|input| {
            record.push(ExtVarInfoRecord {
                name: "out:0".as_ptr(),
                data_type: VT_DOUBLE as c_int,
                dim: [1, 1, 1],
                index: 3,
                direction: DIR_INPUT as c_int,
                description: null(),
                default_ptr: input.as_ptr().cast(),
                data_size: mem::size_of::<c_double>() as c_int,
            });
        });

        OUTPUT_2_DEFAULT.try_with(|input| {
            record.push(ExtVarInfoRecord {
                name: "out:1".as_ptr(),
                data_type: VT_DOUBLE as c_int,
                dim: [1, 1, 1],
                index: 4,
                direction: DIR_INPUT as c_int,
                description: null(),
                default_ptr: input.as_ptr().cast(),
                data_size: mem::size_of::<c_double>() as c_int,
            });
        });

        *n_ext_vars = record.len() as c_int;
        *ext_vars_info = record.as_mut_ptr().cast();

        SCHEMA_HASH.try_with(|hash| *scheme_hash32 = *hash.borrow_mut() as i32)
    });

    STATE_VARS_NAMES.try_with(|s| unsafe {
        let mut record = s.borrow_mut();
        record.clear();

        STATE_0_DEFAULT.try_with(|input| {
            record.push(ExtVarInfoRecord {
                name: "my_diagramv0_out_0".as_ptr(),
                data_type: VT_DOUBLE as c_int,
                dim: [1, 1, 1],
                index: 0,
                direction: DIR_INPUT as c_int,
                description: null(),
                default_ptr: input.as_ptr().cast(),
                data_size: mem::size_of::<c_double>() as c_int,
            });
        });
        STATE_1_DEFAULT.try_with(|input| {
            record.push(ExtVarInfoRecord {
                name: "my_diagramv1_out_0".as_ptr(),
                data_type: VT_DOUBLE as c_int,
                dim: [1, 1, 1],
                index: 8,
                direction: DIR_INPUT as c_int,
                description: null(),
                default_ptr: input.as_ptr().cast(),
                data_size: mem::size_of::<c_double>() as c_int,
            });
        });
        STATE_2_DEFAULT.try_with(|input| {
            record.push(ExtVarInfoRecord {
                name: "my_diagramv2_out_0".as_ptr(),
                data_type: VT_DOUBLE as c_int,
                dim: [1, 1, 1],
                index: 16,
                direction: DIR_INPUT as c_int,
                description: null(),
                default_ptr: input.as_ptr().cast(),
                data_size: mem::size_of::<c_double>() as c_int,
            });
        });
        STATE_3_DEFAULT.try_with(|input| {
            record.push(ExtVarInfoRecord {
                name: "my_diagramv3_out_0".as_ptr(),
                data_type: VT_DOUBLE as c_int,
                dim: [1, 1, 1],
                index: 24,
                direction: DIR_INPUT as c_int,
                description: null(),
                default_ptr: input.as_ptr().cast(),
                data_size: mem::size_of::<c_double>() as c_int,
            });
        });
        STATE_4_DEFAULT.try_with(|input| {
            record.push(ExtVarInfoRecord {
                name: "my_diagramv4_out_0".as_ptr(),
                data_type: VT_DOUBLE as c_int,
                dim: [1, 1, 1],
                index: 32,
                direction: DIR_INPUT as c_int,
                description: null(),
                default_ptr: input.as_ptr().cast(),
                data_size: mem::size_of::<c_double>() as c_int,
            });
        });
        STATE_5_DEFAULT.try_with(|input| {
            record.push(ExtVarInfoRecord {
                name: "my_diagramv5_out_0".as_ptr(),
                data_type: VT_DOUBLE as c_int,
                dim: [1, 1, 1],
                index: 40,
                direction: DIR_INPUT as c_int,
                description: null(),
                default_ptr: input.as_ptr().cast(),
                data_size: mem::size_of::<c_double>() as c_int,
            });
        });

        *n_state_vars = record.len() as c_int;
        *state_vars_info = record.as_mut_ptr().cast();
        *sizeof_state_vars = mem::size_of::<TypeState>() as c_int
    });

    0
}

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
#[allow(non_snake_case)]
#[no_mangle]
pub extern "C" fn INIT_FUNC(
    step: c_double,
    modeltime: c_double,
    ext_vars_addr: *mut *mut c_void,
    din_vars: *mut c_double,
    derivates: *mut c_double,
    alg_vars: *mut c_double,
    alg_funcs: *mut c_double,
    state_vars: *mut TypeState,
    consts: *mut TypeConst,
    locals: *mut TypeLocal,
    solver_data: *mut SolverStruct,
    algo_object_id: *mut c_void,
) -> c_int {
    STATE_0_DEFAULT.try_with(|s| {
        let mut state = s.borrow_mut();
        *state = 0 as c_double;
    });
    STATE_1_DEFAULT.try_with(|s| {
        let mut state = s.borrow_mut();
        *state = 0 as c_double;
    });
    STATE_2_DEFAULT.try_with(|s| {
        let mut state = s.borrow_mut();
        *state = 0 as c_double;
    });
    STATE_3_DEFAULT.try_with(|s| {
        let mut state = s.borrow_mut();
        *state = 0 as c_double;
    });
    STATE_4_DEFAULT.try_with(|s| {
        let mut state = s.borrow_mut();
        *state = 0 as c_double;
    });
    STATE_5_DEFAULT.try_with(|s| {
        let mut state = s.borrow_mut();
        *state = 0 as c_double;
    });
    0
}

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
///
/// # Safety
///
#[allow(non_snake_case)]
#[no_mangle]
pub unsafe extern "C" fn RUN_FUNC(
    action: c_int,
    step: c_double,
    modeltime: c_double,
    ext_vars_addr: *mut *mut c_void,
    din_vars: *mut c_double,
    derivates: *mut c_double,
    alg_vars: *mut c_double,
    alg_funcs: *mut c_double,
    state_vars: *mut TypeState,
    consts: *mut TypeConst,
    locals: *mut TypeLocal,
    solver_data: *mut SolverStruct,
    algo_object_id: *mut c_void,
) -> c_int {
    tracing::warn!("{:#?}", *state_vars);
    match Action::from(action) {
        Action::Stop | Action::GetDeri | Action::GetAlgFun => 0,
        _ => {
            (*state_vars).o1 = (*state_vars).v1 + (*state_vars).v2 + (*state_vars).v3;
            (*state_vars).o2 = (*state_vars).o1 + 1f64;
            0
        }
    }
}

#[allow(non_snake_case)]
#[no_mangle]
pub extern "C" fn STATE_FUNC(
    action: c_int,
    step: c_double,
    modeltime: c_double,
    ext_vars_addr: *mut *mut c_void,
    din_vars: *mut c_double,
    derivates: *mut c_double,
    alg_vars: *mut c_double,
    alg_funcs: *mut c_double,
    state_vars: *mut TypeState,
    consts: *mut TypeConst,
    locals: *mut TypeLocal,
    solver_data: *mut SolverStruct,
    algo_object_id: *mut c_void,
) -> c_int {
    0
}
