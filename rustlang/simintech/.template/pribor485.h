#ifndef pribor485
#define pribor485

  /* Посылка сообщения в ФБ  
     comid    - идентификатор порта, полученный при помощи connect_port
     addr     - адрес прибора
     command  - тип команды
     buffer   - данные команды  
     buffer_size - размер данных команды
  */
int write_rspack(int comid,int addr,int command,char* buffer,int buffer_size)
{
   return 0;
};

  /* Чтение ответа на сообщение из ФБ  
     comid    - идентификатор порта, полученный при помощи connect_port
     addr     - адрес прибора
     buffer   - адрес буфера под возвращаемые данные   
     buffer_size - размер буфера
     byte_readed - количество прочитанных байт
  */
int read_rspack(int comid,int addr,char* buffer,int buffer_size,int* byte_readed)
{
	
	 byte_readed = 0;
	  
   return 0;
};

  /* Инициализация порта передачи данных
  */
int connect_port()
{
   return 0;	
};

#endif