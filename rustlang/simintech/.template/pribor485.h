#ifndef pribor485
#define pribor485

  /* ������� ��������� � ��  
     comid    - ������������� �����, ���������� ��� ������ connect_port
     addr     - ����� �������
     command  - ��� �������
     buffer   - ������ �������  
     buffer_size - ������ ������ �������
  */
int write_rspack(int comid,int addr,int command,char* buffer,int buffer_size)
{
   return 0;
};

  /* ������ ������ �� ��������� �� ��  
     comid    - ������������� �����, ���������� ��� ������ connect_port
     addr     - ����� �������
     buffer   - ����� ������ ��� ������������ ������   
     buffer_size - ������ ������
     byte_readed - ���������� ����������� ����
  */
int read_rspack(int comid,int addr,char* buffer,int buffer_size,int* byte_readed)
{
	
	 byte_readed = 0;
	  
   return 0;
};

  /* ������������� ����� �������� ������
  */
int connect_port()
{
   return 0;	
};

#endif