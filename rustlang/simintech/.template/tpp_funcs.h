#ifndef tpplink
#define tpplink

#include <direct.h>
#include <stdio.h>
#include <process.h>
#include <windows.h>

typedef struct
{
	//Массивы для передачи данных в код
	int            rowcount_to;	              //Размер массива под данные в код ТПП
	int*           file_data_shift_to;        //Массив смещений данных для передачи данных в код ТПП
	unsigned char* shm_datatype_to;           //Массив типов данных переменной в памяти исполняемой системы
	int*           shm_dim_to;                //Массив размерности данных переменной в памяти исполняемой системы
	void**         shm_ptr_to;                //Массив указателей данных переменной в памяти исполняемой системы
	FILE*          file_to;                   //Ссылка файла обмена с кодом 
	double*        to_buffer;                 //Буфер обмена данными для входных данных кода ТПП 
	//Массивы для передачи данных из кода
	int            rowcount_from;             //Размер массива под данные из кода ТПП
	int*           file_data_shift_from;      //Массив смещений данных для приёма данных из кода ТПП	
	unsigned char* shm_datatype_from;         //Массив типов данных переменной в памяти исполняемой системы
	int*           shm_dim_from;              //Массив размерности данных переменной в памяти исполняемой системы
	void**         shm_ptr_from;              //Массив указателей данных переменной в памяти исполняемой системы
	FILE*          file_from;	
	//Вспомогательные флаги
  char           f_havedata;                //Флаг наличия актуальных данных в коде
	int            fromdatacount;             //Количество данных считанных из кода
	int            to_buffer_size;            //Размер буфера для 
	//Переменные для синхронизации с кодом
	double         old_time;
	char           ffirstdata;
	//Данные необходимые для обеспечения внешнего управления кодом
	FILE*          file_shl;                  //Идентификатор файла, в который будет записывать данные оболочка в режиме наблюдения
} tpp_struct;


int find_shm_var(solver_struct* solver_data,char* var_name,unsigned char* datatype,int* linedim,void** shm_ptr)
{	
	//Выполнение поиска переменной по имени среди всех внешних переменных - в этой версии модуля, вызывается внешняя функция из стыковочного интерфейса	
    *datatype = solver_data->GetDataPtr(solver_data->TaskContext,var_name,shm_ptr,linedim);

	if(shm_ptr==NULL){
		return 1;
	}else{
	    return 0;
	};
}	


  /* Инициализация обмена данными с расчётным кодом ТПП
     tpp_context - адрес на структуру для обмена данными с расчётным кодом 
     tppprojname - имя проекта для расчётного кода 
     tppprojdir  - директория проекта расчётного кода ТПП
  */
int tpp_initialize(solver_struct* solver_data,tpp_struct** tpp_context,char* tppprojname,char* tppprojdir)
{ 
    char  buf[2048] = {0};
    FILE* fd = NULL;
    char  totpplist_filename[2048] = {0}; 
    char  fromtpplist_filename[2048] = {0};
    char  shellcontrol_filename[2048] = {0}; 
    char  signal_name[256] = {0};
    int   row_count = 0;
    int   pid = 0;
    int   new_buf_size;
    double* new_buffer;
	char  cur_dir[2048] = {0};
    int i;
      
	  /*Выделим память под структуру с которой будем работать в дальнейшем*/
    *tpp_context = malloc(sizeof(tpp_struct));    
    
    //Формируем имена файлов из которых будем читать конфигурацию обмена с кодом 
    strcpy(totpplist_filename,   tppprojname); 
    strcat(totpplist_filename,   ".totpcode");    
    strcpy(fromtpplist_filename, tppprojname); 
    strcat(fromtpplist_filename, ".fromtpcode"); 

	_getcwd(cur_dir,sizeof(cur_dir));
	_chdir(tppprojdir);
  
    //Тут мы загружаем файлы привязки параметров кода к переменным и формируем соотв-е служебные записи    
    //Файл "в код"    
  	if ((fd = fopen(totpplist_filename,"r")) == NULL)
	  {
	     //Возврат на предыдущую директорию
	     _chdir(cur_dir);
	   	 //perror("error open totpp code file");
		 return 1;  
	  };  
    //Считаем количество строк в файле  
    row_count = 0;
  	while (fgets(buf, 2048, fd) != NULL){row_count++;};   
    //Инициализируем память под данные из файла
    (*tpp_context)->rowcount_to = row_count;
    (*tpp_context)->file_data_shift_to = malloc(sizeof(int)*row_count);
    (*tpp_context)->shm_datatype_to = malloc(sizeof(unsigned char)*row_count);
    (*tpp_context)->shm_dim_to = malloc(sizeof(int)*row_count);
    (*tpp_context)->shm_ptr_to = malloc(sizeof(void*)*row_count);
    //Выполняем считывание данных из этого файла
    row_count = 0;
	fseek(fd, 0, SEEK_SET);
  	while (fgets(buf, 2048, fd) != NULL)
  	{
    	/*Получим информацию о привязке сигнала к номеру параметра в файле ТПП*/
      sscanf(buf, "%s %d", signal_name, &((*tpp_context)->file_data_shift_to[row_count]));
      //Выполним поиск сигнала по имени и его тип данных в памяти внешних переменных  
      (*tpp_context)->shm_datatype_to[row_count] = 255;
      (*tpp_context)->shm_ptr_to[row_count] = NULL;
      (*tpp_context)->shm_dim_to[row_count] = 0;
      if (find_shm_var(solver_data,&signal_name,
           &(*tpp_context)->shm_datatype_to[row_count],
           &(*tpp_context)->shm_dim_to[row_count],
           &(*tpp_context)->shm_ptr_to[row_count]) != 0) {
             	//Ошибка при поиске переменной - выдача диагностики если надо            	
           	};
      row_count++;
  	}; // while fgets  
  	fclose(fd);
     
  
    //Файл "из кода"       
  	if ((fd = fopen(fromtpplist_filename,"r")) == NULL)
	  {
	     //Возврат на предыдущую директорию
	     _chdir(cur_dir);
	   	 //perror("error open frommtpp code file");
		 return 1;  
	  };   
    //Считаем количество строк в файле  
    row_count = 0;
  	while (fgets(buf, 2048, fd) != NULL){row_count++;};   
    //Инициализируем память под данные из файла
    (*tpp_context)->rowcount_from = row_count;
    (*tpp_context)->file_data_shift_from = (int*)malloc(sizeof(int)*row_count);
    (*tpp_context)->shm_datatype_from = malloc(sizeof(unsigned char)*row_count);
    (*tpp_context)->shm_dim_from = malloc(sizeof(int)*row_count);
    (*tpp_context)->shm_ptr_from = malloc(sizeof(void*)*row_count);
    //Выполняем считывание данных из этого файла
    row_count = 0;
	fseek(fd, 0, SEEK_SET);
  	while (fgets(buf, 2048, fd) != NULL)
  	{
	   	/*Получим информацию о привязке сигнала к номеру параметра в файле ТПП*/
	  	sscanf(buf, "%s %d", signal_name, &((*tpp_context)->file_data_shift_from[row_count]));
      //Выполним поиск сигнала по имени и его тип данных в памяти внешних переменных   
      (*tpp_context)->shm_datatype_from[row_count] = 255;
	  (*tpp_context)->shm_ptr_from[row_count] = NULL;
      (*tpp_context)->shm_dim_from[row_count] = 0;
      if (find_shm_var(solver_data,&signal_name,
           &(*tpp_context)->shm_datatype_from[row_count],
           &(*tpp_context)->shm_dim_from[row_count],
           &(*tpp_context)->shm_ptr_from[row_count]) != 0) {         	
           	//Ошибка при поиске переменной - вывод сообщения в консоль если надо !!!

           	};
      row_count++;
  	}; // while fgets  
    fclose(fd);

	  (*tpp_context)->f_havedata = 0;

    //Инициализируем файлы обмена данными с расчётным кодом
    //В ТПП
    strcpy(totpplist_filename,   tppprojname); 
    strcat(totpplist_filename,   ".to");  
    //Из ТПП   
    strcpy(fromtpplist_filename, tppprojname); 
    strcat(fromtpplist_filename, ".frm");     
    
    //Пытаемся открыть файл записи в код ТПП если таковой есть, чтобы получить данные, которые там были записаны оболочкой  
  	if (((*tpp_context)->file_to = fopen(totpplist_filename,"rb+")) != NULL)
	  {
	  	 //Определяем размер файла ввода данных в тпп 
	     fseek((*tpp_context)->file_to,0,SEEK_END);
	     (*tpp_context)->to_buffer_size = (ftell((*tpp_context)->file_to) - sizeof(int))/sizeof(double);
	     (*tpp_context)->to_buffer = malloc(sizeof(double)*((*tpp_context)->to_buffer_size));
	     //Читаем начальное значение файла обмена, чтобы там были хоть какие-то проинициализированные данные по граничным условия от оболочки...      
	     fseek((*tpp_context)->file_to, sizeof(int), SEEK_SET);
	     fread((*tpp_context)->to_buffer,sizeof(double),((*tpp_context)->to_buffer_size),(*tpp_context)->file_to);  
	     fclose((*tpp_context)->file_to);   
	  }else{
	  	 (*tpp_context)->to_buffer_size = 0;
	  };	   
	  
	  	  	  	    
	//Запускаем расчётный код ТПП
    WinExec("go_tpp.bat",SW_SHOWNORMAL);
    	
	//Ждём пока не запуститься код 2 секунды 	
	Sleep(2000);  
    	     
    //После инициализации кода открываем файл обмена ещё раз, чтобы туда что-нибудь писать
  	if (((*tpp_context)->file_to = fopen(totpplist_filename,"rb+")) == NULL)
	  {
	     //Возврат на предыдущую директорию
	     _chdir(cur_dir);
	   	 //perror("error open .to data file");
		 return 1;  
	  };   
	  
	  //Определяем размер файла ввода данных в тпп 
	fseek((*tpp_context)->file_to,0,SEEK_END);
	new_buf_size = (ftell((*tpp_context)->file_to) - sizeof(int))/sizeof(double);
	if(new_buf_size > (*tpp_context)->to_buffer_size){
	  	new_buffer = malloc(sizeof(double)*new_buf_size);
		memset(new_buffer,0,sizeof(double)*new_buf_size);
	  	memcpy(new_buffer,(*tpp_context)->to_buffer,sizeof(double)*new_buf_size);
	  	(*tpp_context)->to_buffer=new_buffer;
	};		 
	(*tpp_context)->to_buffer_size = new_buf_size; 

    //Перевод ТПП на ожидание данных - чтобы не было шагов на неинициализированных значениях
    i = 0;
    fseek((*tpp_context)->file_to, 0, SEEK_SET);
    fwrite(&i, sizeof(i), 1, (*tpp_context)->file_to);
    //Пишем буфер в файл обмена целиком
    fwrite((*tpp_context)->to_buffer,sizeof(double)*(*tpp_context)->to_buffer_size,1,(*tpp_context)->file_to); 
    fflush((*tpp_context)->file_to);
        
	  //Создаём файл для обеспечения внешнего управления расчётным кодом to2  - в него будет писать оболочка, а этот модуль будет его перечитывать и выдавать данные непосредственно в код
    strcpy(shellcontrol_filename,   tppprojname); 
    strcat(shellcontrol_filename,   ".to2");  	  
    (*tpp_context)->file_shl = 0;
    //Инициализируем начальное состояние этого файла, в который будет писать свои данные оболочка через сервер обмена	  
  	if (((*tpp_context)->file_shl = fopen(shellcontrol_filename,"wb+")) != NULL)
	  {     
      i = 0;
      fwrite(&i, sizeof(i), 1, (*tpp_context)->file_shl);
      //Пишем буфер в файл обмена целиком
      fwrite((*tpp_context)->to_buffer,sizeof(double)*(*tpp_context)->to_buffer_size,1,(*tpp_context)->file_shl); 
      fflush((*tpp_context)->file_shl);         
	  }else{
	    (*tpp_context)->file_shl = 0;
	  };    
       
    //Инициализируем открытие файла чтения данных от кода ТПП
  	if (((*tpp_context)->file_from = fopen(fromtpplist_filename,"rb+")) == NULL)
	  {
	     //Возврат на предыдущую директорию
	     _chdir(cur_dir);
	   	 //perror("error open .frm data file");
		 return 1;  
	  }; 
	  
    //Перевод модуля обмена на ожидание данных - чтобы не было шагов на неинициализированных значениях
    i = 0;
    fseek((*tpp_context)->file_from, 0, SEEK_SET);
    fwrite(&i, sizeof(i), 1, (*tpp_context)->file_from);
    fflush((*tpp_context)->file_from);
    
    (*tpp_context)->ffirstdata = 1;

	//Возврат на предыдущую директорию
	_chdir(cur_dir);

    return 0;  
    
};

  /* Завершение обмена данными с расчётным кодом ТПП и выгрузка кода
     tpp_context - адрес на структуру для обмена данными с расчётным кодом 
  */
int tpp_stop(solver_struct* solver_data,tpp_struct** tpp_context)
{
		  
   int i;
   
   //Тут мы подаём коду через файлы команду на завершение счёта
   if ((*tpp_context)->file_to != NULL){
     //Посылаем сигнал останова расчётному коду вместе с буфером, т.к. иначе код испортит файл обмена !!!
     fseek((*tpp_context)->file_to, sizeof(int), SEEK_SET);
     fwrite((*tpp_context)->to_buffer,sizeof(double)*(*tpp_context)->to_buffer_size,1,(*tpp_context)->file_to);       
     fseek((*tpp_context)->file_to, 0, SEEK_SET); 
     i = -1;
     fwrite(&i, sizeof(i), 1, (*tpp_context)->file_to);
     fflush((*tpp_context)->file_to);   
     //Закрываем файл обмена данными
     fclose((*tpp_context)->file_to);   
     (*tpp_context)->file_to = NULL; 
     //И сообщаем что код мы прибиваем аккуратно
     printf("write stop signal to tpp \n"); 	
   };

   if ((*tpp_context)->file_from != NULL){
     fclose((*tpp_context)->file_from);
   	(*tpp_context)->file_from = NULL;
   };
   
   if ((*tpp_context)->file_shl != NULL){
     fclose((*tpp_context)->file_shl);
   	(*tpp_context)->file_shl = NULL;
   };   
   
   //Уничтожаем память выделенную для обработки соединения
   free((*tpp_context)->file_data_shift_from); 
   free((*tpp_context)->shm_datatype_from);
   free((*tpp_context)->shm_dim_from);
   free((*tpp_context)->shm_ptr_from);   
   free((*tpp_context)->file_data_shift_to);
   free((*tpp_context)->shm_datatype_to);
   free((*tpp_context)->shm_dim_to);
   free((*tpp_context)->shm_ptr_to);
   free((*tpp_context));

   return 0;
};

  /* Обмен данными с расчётным кодом ТПП
     tpp_context - адрес на структуру для обмена данными с расчётным кодом 
     step        - текущий шаг интегрирования
     tpptime     - локальное время самого расчётного кода
	 sleeptime   - задержка ожидания выполнения шага расчётного кода
  */
int tpp_step(solver_struct* solver_data,tpp_struct** tpp_context,double* step,double* tpptime,int sleeptime)
{
	 
   int i,j,tmp_int;
   double tmp_var;
   char   tmp_char;
   char*  temp_ptr;
   char*  wr_ptr;
   double new_time;
   	
   //Тут мы выполняем шаг обмена данными в соотсвествии с проинициализированными структурами   
   if ((*tpp_context)->f_havedata) {

      //Ждём поступления актуальных данных от расчётного кода
      (*tpp_context)->fromdatacount = 0;
      
	  next_cycle:
	      
		   fseek((*tpp_context)->file_from, 0, SEEK_SET);
		   //Чтение данных
       fread(&(*tpp_context)->fromdatacount,sizeof(int),1,(*tpp_context)->file_from);  
       //Чтение нового времени от расчётного кода 
       fread(&new_time,sizeof(new_time),1,(*tpp_context)->file_from);          
       //Обработка завершения процесса
	     if((*tpp_context)->fromdatacount == -1){                 
         return 1;
	     };  
	     //Выход из обработки по асинхронной остановке сверху
	     if(solver_data->StopCheck(solver_data->TaskContext)){
	     	 return 0;
	     };	
	                
       //Проверка изменения модельного времени - по этому критерию определяем выполнен ли шаг расчётным кодом
       if((*tpp_context)->old_time == new_time){
        	Sleep(sleeptime); 
          if(!((*tpp_context)->ffirstdata)) goto next_cycle;
       }else{
         (*tpp_context)->ffirstdata = 0; 	          	
       };		 
         
      //Читаем и разбираем данные от кода ТПП
	  for(i=0;i<(*tpp_context)->rowcount_from;i++){
		  if (((*tpp_context)->shm_datatype_from[i] != 255)&&((*tpp_context)->shm_ptr_from[i] != NULL))  {
			//Позиционируем объект данных на файле   
            fseek((*tpp_context)->file_from, sizeof(int) + (*tpp_context)->file_data_shift_from[i]*sizeof(double), SEEK_SET);
            //Читаем всё в буфер типа double
			if((*tpp_context)->shm_datatype_from[i] == 0) {
		      //Чтение без преобразования		 
              fread((*tpp_context)->shm_ptr_from[i],sizeof(double)*(*tpp_context)->shm_dim_from[i],1,(*tpp_context)->file_from); 
			}else{  
              //Чтение с преобразованием величины 
                temp_ptr = (*tpp_context)->shm_ptr_from[i];
				for(j=0;j<(*tpp_context)->shm_dim_from[i];j++){
                  //Читаем в промежуточную переменную
                  fread(&tmp_var,sizeof(double),1,(*tpp_context)->file_from); 
				  //Преобразование к дискретной переменной
				  if((*tpp_context)->shm_datatype_from[i] == 1){ 
                     tmp_char = tmp_var > 0.5;
                     memcpy(temp_ptr,&tmp_char,sizeof(char));
                     temp_ptr = temp_ptr + sizeof(char);
				  };
				  //Преобразование к целой переменной
				  if((*tpp_context)->shm_datatype_from[i] == 2){ 
                     tmp_int = tmp_var;
                     memcpy(temp_ptr,&tmp_int,sizeof(int));
                     temp_ptr = temp_ptr + sizeof(int);
				  };
				};
			};
        };
	  };
   };  //end if f_havedata
     
   //Чтение времени от расчётного кода 
   fseek((*tpp_context)->file_from, sizeof(int), SEEK_SET);   
   fread(&(*tpp_context)->old_time,sizeof(double),1,(*tpp_context)->file_from);   
   
    //Читаем данные буфера входа программы из файла управления оболочки - потом то, что надо перепишется этим модулем  
   if ((*tpp_context)->file_shl != NULL)
	 {          
     fseek((*tpp_context)->file_shl, 4, SEEK_SET);
     fread((*tpp_context)->to_buffer,sizeof(double)*(*tpp_context)->to_buffer_size,1,(*tpp_context)->file_shl);          
	 };     
            
   //Перевод ТПП на ожидание данных
   i = 0;
   fseek((*tpp_context)->file_to, 0, SEEK_SET);
   fwrite(&i, sizeof(i), 1, (*tpp_context)->file_to);
   fflush((*tpp_context)->file_to);
   
   //По идее тут можно вставить и чтение файла входа в ТПП, но пока это работать не будет, т.к. он всё время обнуляется

   //Пишем данные в код ТПП  
   for(i=0;i<(*tpp_context)->rowcount_to;i++){
   	
   	  wr_ptr = (char*)(*tpp_context)->to_buffer + (*tpp_context)->file_data_shift_to[i]*sizeof(double);
   	  
		  if (((*tpp_context)->shm_datatype_to[i] != 255)&&((*tpp_context)->shm_ptr_to[i] != NULL))  {
		  			  	
		   	if ((*tpp_context)->shm_datatype_to[i] == 0) {

		    	//Запись без преобразования данных
		     	memcpy(wr_ptr,(*tpp_context)->shm_ptr_to[i],sizeof(double)*(*tpp_context)->shm_dim_to[i]);
		  	   	 
			  }else{  
                //Запись с преобразованием величины 
			  	tmp_var = 0;
          temp_ptr = (*tpp_context)->shm_ptr_to[i];
        
			  	for(j=0;j<(*tpp_context)->shm_dim_to[i];j++){
				    //Преобразование из дискретной переменной
				    if((*tpp_context)->shm_datatype_to[i] == 1){ 
                     memcpy(&tmp_char,temp_ptr,sizeof(char));
                     tmp_var = tmp_char;
                     temp_ptr = temp_ptr + sizeof(char);
				    };
				    //Преобразование из целой переменной
				    if((*tpp_context)->shm_datatype_to[i] == 2){ 
                     memcpy(&tmp_int,temp_ptr,sizeof(int));
                     tmp_var = tmp_int;                    
                     temp_ptr = temp_ptr + sizeof(int);
				    };

            memcpy(wr_ptr,&tmp_var,sizeof(double));                 
            wr_ptr = wr_ptr + sizeof(double);          
				  };  //  for j
		  	};						
     };
   }; 

   fseek((*tpp_context)->file_to, sizeof(int), SEEK_SET);
   fwrite((*tpp_context)->to_buffer,sizeof(double)*(*tpp_context)->to_buffer_size,1,(*tpp_context)->file_to);       
   fseek((*tpp_context)->file_to, 0, SEEK_SET); 
   i = (*tpp_context)->to_buffer_size;
   fwrite(&i, sizeof(i), 1, (*tpp_context)->file_to);
   fflush((*tpp_context)->file_to);
 
   //Устанавливаем флаг, что нам уже таки имеет смысл дожидаться данных от расчётного кода
   (*tpp_context)->f_havedata=1;

   return 0;
};

#endif