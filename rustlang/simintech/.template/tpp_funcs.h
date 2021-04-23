#ifndef tpplink
#define tpplink

#include <direct.h>
#include <stdio.h>
#include <process.h>
#include <windows.h>

typedef struct
{
	//������� ��� �������� ������ � ���
	int            rowcount_to;	              //������ ������� ��� ������ � ��� ���
	int*           file_data_shift_to;        //������ �������� ������ ��� �������� ������ � ��� ���
	unsigned char* shm_datatype_to;           //������ ����� ������ ���������� � ������ ����������� �������
	int*           shm_dim_to;                //������ ����������� ������ ���������� � ������ ����������� �������
	void**         shm_ptr_to;                //������ ���������� ������ ���������� � ������ ����������� �������
	FILE*          file_to;                   //������ ����� ������ � ����� 
	double*        to_buffer;                 //����� ������ ������� ��� ������� ������ ���� ��� 
	//������� ��� �������� ������ �� ����
	int            rowcount_from;             //������ ������� ��� ������ �� ���� ���
	int*           file_data_shift_from;      //������ �������� ������ ��� ����� ������ �� ���� ���	
	unsigned char* shm_datatype_from;         //������ ����� ������ ���������� � ������ ����������� �������
	int*           shm_dim_from;              //������ ����������� ������ ���������� � ������ ����������� �������
	void**         shm_ptr_from;              //������ ���������� ������ ���������� � ������ ����������� �������
	FILE*          file_from;	
	//��������������� �����
  char           f_havedata;                //���� ������� ���������� ������ � ����
	int            fromdatacount;             //���������� ������ ��������� �� ����
	int            to_buffer_size;            //������ ������ ��� 
	//���������� ��� ������������� � �����
	double         old_time;
	char           ffirstdata;
	//������ ����������� ��� ����������� �������� ���������� �����
	FILE*          file_shl;                  //������������� �����, � ������� ����� ���������� ������ �������� � ������ ����������
} tpp_struct;


int find_shm_var(solver_struct* solver_data,char* var_name,unsigned char* datatype,int* linedim,void** shm_ptr)
{	
	//���������� ������ ���������� �� ����� ����� ���� ������� ���������� - � ���� ������ ������, ���������� ������� ������� �� ������������ ����������	
    *datatype = solver_data->GetDataPtr(solver_data->TaskContext,var_name,shm_ptr,linedim);

	if(shm_ptr==NULL){
		return 1;
	}else{
	    return 0;
	};
}	


  /* ������������� ������ ������� � ��������� ����� ���
     tpp_context - ����� �� ��������� ��� ������ ������� � ��������� ����� 
     tppprojname - ��� ������� ��� ���������� ���� 
     tppprojdir  - ���������� ������� ���������� ���� ���
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
      
	  /*������� ������ ��� ��������� � ������� ����� �������� � ����������*/
    *tpp_context = malloc(sizeof(tpp_struct));    
    
    //��������� ����� ������ �� ������� ����� ������ ������������ ������ � ����� 
    strcpy(totpplist_filename,   tppprojname); 
    strcat(totpplist_filename,   ".totpcode");    
    strcpy(fromtpplist_filename, tppprojname); 
    strcat(fromtpplist_filename, ".fromtpcode"); 

	_getcwd(cur_dir,sizeof(cur_dir));
	_chdir(tppprojdir);
  
    //��� �� ��������� ����� �������� ���������� ���� � ���������� � ��������� �����-� ��������� ������    
    //���� "� ���"    
  	if ((fd = fopen(totpplist_filename,"r")) == NULL)
	  {
	     //������� �� ���������� ����������
	     _chdir(cur_dir);
	   	 //perror("error open totpp code file");
		 return 1;  
	  };  
    //������� ���������� ����� � �����  
    row_count = 0;
  	while (fgets(buf, 2048, fd) != NULL){row_count++;};   
    //�������������� ������ ��� ������ �� �����
    (*tpp_context)->rowcount_to = row_count;
    (*tpp_context)->file_data_shift_to = malloc(sizeof(int)*row_count);
    (*tpp_context)->shm_datatype_to = malloc(sizeof(unsigned char)*row_count);
    (*tpp_context)->shm_dim_to = malloc(sizeof(int)*row_count);
    (*tpp_context)->shm_ptr_to = malloc(sizeof(void*)*row_count);
    //��������� ���������� ������ �� ����� �����
    row_count = 0;
	fseek(fd, 0, SEEK_SET);
  	while (fgets(buf, 2048, fd) != NULL)
  	{
    	/*������� ���������� � �������� ������� � ������ ��������� � ����� ���*/
      sscanf(buf, "%s %d", signal_name, &((*tpp_context)->file_data_shift_to[row_count]));
      //�������� ����� ������� �� ����� � ��� ��� ������ � ������ ������� ����������  
      (*tpp_context)->shm_datatype_to[row_count] = 255;
      (*tpp_context)->shm_ptr_to[row_count] = NULL;
      (*tpp_context)->shm_dim_to[row_count] = 0;
      if (find_shm_var(solver_data,&signal_name,
           &(*tpp_context)->shm_datatype_to[row_count],
           &(*tpp_context)->shm_dim_to[row_count],
           &(*tpp_context)->shm_ptr_to[row_count]) != 0) {
             	//������ ��� ������ ���������� - ������ ����������� ���� ����            	
           	};
      row_count++;
  	}; // while fgets  
  	fclose(fd);
     
  
    //���� "�� ����"       
  	if ((fd = fopen(fromtpplist_filename,"r")) == NULL)
	  {
	     //������� �� ���������� ����������
	     _chdir(cur_dir);
	   	 //perror("error open frommtpp code file");
		 return 1;  
	  };   
    //������� ���������� ����� � �����  
    row_count = 0;
  	while (fgets(buf, 2048, fd) != NULL){row_count++;};   
    //�������������� ������ ��� ������ �� �����
    (*tpp_context)->rowcount_from = row_count;
    (*tpp_context)->file_data_shift_from = (int*)malloc(sizeof(int)*row_count);
    (*tpp_context)->shm_datatype_from = malloc(sizeof(unsigned char)*row_count);
    (*tpp_context)->shm_dim_from = malloc(sizeof(int)*row_count);
    (*tpp_context)->shm_ptr_from = malloc(sizeof(void*)*row_count);
    //��������� ���������� ������ �� ����� �����
    row_count = 0;
	fseek(fd, 0, SEEK_SET);
  	while (fgets(buf, 2048, fd) != NULL)
  	{
	   	/*������� ���������� � �������� ������� � ������ ��������� � ����� ���*/
	  	sscanf(buf, "%s %d", signal_name, &((*tpp_context)->file_data_shift_from[row_count]));
      //�������� ����� ������� �� ����� � ��� ��� ������ � ������ ������� ����������   
      (*tpp_context)->shm_datatype_from[row_count] = 255;
	  (*tpp_context)->shm_ptr_from[row_count] = NULL;
      (*tpp_context)->shm_dim_from[row_count] = 0;
      if (find_shm_var(solver_data,&signal_name,
           &(*tpp_context)->shm_datatype_from[row_count],
           &(*tpp_context)->shm_dim_from[row_count],
           &(*tpp_context)->shm_ptr_from[row_count]) != 0) {         	
           	//������ ��� ������ ���������� - ����� ��������� � ������� ���� ���� !!!

           	};
      row_count++;
  	}; // while fgets  
    fclose(fd);

	  (*tpp_context)->f_havedata = 0;

    //�������������� ����� ������ ������� � ��������� �����
    //� ���
    strcpy(totpplist_filename,   tppprojname); 
    strcat(totpplist_filename,   ".to");  
    //�� ���   
    strcpy(fromtpplist_filename, tppprojname); 
    strcat(fromtpplist_filename, ".frm");     
    
    //�������� ������� ���� ������ � ��� ��� ���� ������� ����, ����� �������� ������, ������� ��� ���� �������� ���������  
  	if (((*tpp_context)->file_to = fopen(totpplist_filename,"rb+")) != NULL)
	  {
	  	 //���������� ������ ����� ����� ������ � ��� 
	     fseek((*tpp_context)->file_to,0,SEEK_END);
	     (*tpp_context)->to_buffer_size = (ftell((*tpp_context)->file_to) - sizeof(int))/sizeof(double);
	     (*tpp_context)->to_buffer = malloc(sizeof(double)*((*tpp_context)->to_buffer_size));
	     //������ ��������� �������� ����� ������, ����� ��� ���� ���� �����-�� ��������������������� ������ �� ��������� ������� �� ��������...      
	     fseek((*tpp_context)->file_to, sizeof(int), SEEK_SET);
	     fread((*tpp_context)->to_buffer,sizeof(double),((*tpp_context)->to_buffer_size),(*tpp_context)->file_to);  
	     fclose((*tpp_context)->file_to);   
	  }else{
	  	 (*tpp_context)->to_buffer_size = 0;
	  };	   
	  
	  	  	  	    
	//��������� ��������� ��� ���
    WinExec("go_tpp.bat",SW_SHOWNORMAL);
    	
	//��� ���� �� ����������� ��� 2 ������� 	
	Sleep(2000);  
    	     
    //����� ������������� ���� ��������� ���� ������ ��� ���, ����� ���� ���-������ ������
  	if (((*tpp_context)->file_to = fopen(totpplist_filename,"rb+")) == NULL)
	  {
	     //������� �� ���������� ����������
	     _chdir(cur_dir);
	   	 //perror("error open .to data file");
		 return 1;  
	  };   
	  
	  //���������� ������ ����� ����� ������ � ��� 
	fseek((*tpp_context)->file_to,0,SEEK_END);
	new_buf_size = (ftell((*tpp_context)->file_to) - sizeof(int))/sizeof(double);
	if(new_buf_size > (*tpp_context)->to_buffer_size){
	  	new_buffer = malloc(sizeof(double)*new_buf_size);
		memset(new_buffer,0,sizeof(double)*new_buf_size);
	  	memcpy(new_buffer,(*tpp_context)->to_buffer,sizeof(double)*new_buf_size);
	  	(*tpp_context)->to_buffer=new_buffer;
	};		 
	(*tpp_context)->to_buffer_size = new_buf_size; 

    //������� ��� �� �������� ������ - ����� �� ���� ����� �� �������������������� ���������
    i = 0;
    fseek((*tpp_context)->file_to, 0, SEEK_SET);
    fwrite(&i, sizeof(i), 1, (*tpp_context)->file_to);
    //����� ����� � ���� ������ �������
    fwrite((*tpp_context)->to_buffer,sizeof(double)*(*tpp_context)->to_buffer_size,1,(*tpp_context)->file_to); 
    fflush((*tpp_context)->file_to);
        
	  //������ ���� ��� ����������� �������� ���������� ��������� ����� to2  - � ���� ����� ������ ��������, � ���� ������ ����� ��� ������������ � �������� ������ ��������������� � ���
    strcpy(shellcontrol_filename,   tppprojname); 
    strcat(shellcontrol_filename,   ".to2");  	  
    (*tpp_context)->file_shl = 0;
    //�������������� ��������� ��������� ����� �����, � ������� ����� ������ ���� ������ �������� ����� ������ ������	  
  	if (((*tpp_context)->file_shl = fopen(shellcontrol_filename,"wb+")) != NULL)
	  {     
      i = 0;
      fwrite(&i, sizeof(i), 1, (*tpp_context)->file_shl);
      //����� ����� � ���� ������ �������
      fwrite((*tpp_context)->to_buffer,sizeof(double)*(*tpp_context)->to_buffer_size,1,(*tpp_context)->file_shl); 
      fflush((*tpp_context)->file_shl);         
	  }else{
	    (*tpp_context)->file_shl = 0;
	  };    
       
    //�������������� �������� ����� ������ ������ �� ���� ���
  	if (((*tpp_context)->file_from = fopen(fromtpplist_filename,"rb+")) == NULL)
	  {
	     //������� �� ���������� ����������
	     _chdir(cur_dir);
	   	 //perror("error open .frm data file");
		 return 1;  
	  }; 
	  
    //������� ������ ������ �� �������� ������ - ����� �� ���� ����� �� �������������������� ���������
    i = 0;
    fseek((*tpp_context)->file_from, 0, SEEK_SET);
    fwrite(&i, sizeof(i), 1, (*tpp_context)->file_from);
    fflush((*tpp_context)->file_from);
    
    (*tpp_context)->ffirstdata = 1;

	//������� �� ���������� ����������
	_chdir(cur_dir);

    return 0;  
    
};

  /* ���������� ������ ������� � ��������� ����� ��� � �������� ����
     tpp_context - ����� �� ��������� ��� ������ ������� � ��������� ����� 
  */
int tpp_stop(solver_struct* solver_data,tpp_struct** tpp_context)
{
		  
   int i;
   
   //��� �� ����� ���� ����� ����� ������� �� ���������� �����
   if ((*tpp_context)->file_to != NULL){
     //�������� ������ �������� ���������� ���� ������ � �������, �.�. ����� ��� �������� ���� ������ !!!
     fseek((*tpp_context)->file_to, sizeof(int), SEEK_SET);
     fwrite((*tpp_context)->to_buffer,sizeof(double)*(*tpp_context)->to_buffer_size,1,(*tpp_context)->file_to);       
     fseek((*tpp_context)->file_to, 0, SEEK_SET); 
     i = -1;
     fwrite(&i, sizeof(i), 1, (*tpp_context)->file_to);
     fflush((*tpp_context)->file_to);   
     //��������� ���� ������ �������
     fclose((*tpp_context)->file_to);   
     (*tpp_context)->file_to = NULL; 
     //� �������� ��� ��� �� ��������� ���������
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
   
   //���������� ������ ���������� ��� ��������� ����������
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

  /* ����� ������� � ��������� ����� ���
     tpp_context - ����� �� ��������� ��� ������ ������� � ��������� ����� 
     step        - ������� ��� ��������������
     tpptime     - ��������� ����� ������ ���������� ����
	 sleeptime   - �������� �������� ���������� ���� ���������� ����
  */
int tpp_step(solver_struct* solver_data,tpp_struct** tpp_context,double* step,double* tpptime,int sleeptime)
{
	 
   int i,j,tmp_int;
   double tmp_var;
   char   tmp_char;
   char*  temp_ptr;
   char*  wr_ptr;
   double new_time;
   	
   //��� �� ��������� ��� ������ ������� � ������������ � ���������������������� �����������   
   if ((*tpp_context)->f_havedata) {

      //��� ����������� ���������� ������ �� ���������� ����
      (*tpp_context)->fromdatacount = 0;
      
	  next_cycle:
	      
		   fseek((*tpp_context)->file_from, 0, SEEK_SET);
		   //������ ������
       fread(&(*tpp_context)->fromdatacount,sizeof(int),1,(*tpp_context)->file_from);  
       //������ ������ ������� �� ���������� ���� 
       fread(&new_time,sizeof(new_time),1,(*tpp_context)->file_from);          
       //��������� ���������� ��������
	     if((*tpp_context)->fromdatacount == -1){                 
         return 1;
	     };  
	     //����� �� ��������� �� ����������� ��������� ������
	     if(solver_data->StopCheck(solver_data->TaskContext)){
	     	 return 0;
	     };	
	                
       //�������� ��������� ���������� ������� - �� ����� �������� ���������� �������� �� ��� ��������� �����
       if((*tpp_context)->old_time == new_time){
        	Sleep(sleeptime); 
          if(!((*tpp_context)->ffirstdata)) goto next_cycle;
       }else{
         (*tpp_context)->ffirstdata = 0; 	          	
       };		 
         
      //������ � ��������� ������ �� ���� ���
	  for(i=0;i<(*tpp_context)->rowcount_from;i++){
		  if (((*tpp_context)->shm_datatype_from[i] != 255)&&((*tpp_context)->shm_ptr_from[i] != NULL))  {
			//������������� ������ ������ �� �����   
            fseek((*tpp_context)->file_from, sizeof(int) + (*tpp_context)->file_data_shift_from[i]*sizeof(double), SEEK_SET);
            //������ �� � ����� ���� double
			if((*tpp_context)->shm_datatype_from[i] == 0) {
		      //������ ��� ��������������		 
              fread((*tpp_context)->shm_ptr_from[i],sizeof(double)*(*tpp_context)->shm_dim_from[i],1,(*tpp_context)->file_from); 
			}else{  
              //������ � ��������������� �������� 
                temp_ptr = (*tpp_context)->shm_ptr_from[i];
				for(j=0;j<(*tpp_context)->shm_dim_from[i];j++){
                  //������ � ������������� ����������
                  fread(&tmp_var,sizeof(double),1,(*tpp_context)->file_from); 
				  //�������������� � ���������� ����������
				  if((*tpp_context)->shm_datatype_from[i] == 1){ 
                     tmp_char = tmp_var > 0.5;
                     memcpy(temp_ptr,&tmp_char,sizeof(char));
                     temp_ptr = temp_ptr + sizeof(char);
				  };
				  //�������������� � ����� ����������
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
     
   //������ ������� �� ���������� ���� 
   fseek((*tpp_context)->file_from, sizeof(int), SEEK_SET);   
   fread(&(*tpp_context)->old_time,sizeof(double),1,(*tpp_context)->file_from);   
   
    //������ ������ ������ ����� ��������� �� ����� ���������� �������� - ����� ��, ��� ���� ����������� ���� �������  
   if ((*tpp_context)->file_shl != NULL)
	 {          
     fseek((*tpp_context)->file_shl, 4, SEEK_SET);
     fread((*tpp_context)->to_buffer,sizeof(double)*(*tpp_context)->to_buffer_size,1,(*tpp_context)->file_shl);          
	 };     
            
   //������� ��� �� �������� ������
   i = 0;
   fseek((*tpp_context)->file_to, 0, SEEK_SET);
   fwrite(&i, sizeof(i), 1, (*tpp_context)->file_to);
   fflush((*tpp_context)->file_to);
   
   //�� ���� ��� ����� �������� � ������ ����� ����� � ���, �� ���� ��� �������� �� �����, �.�. �� �� ����� ����������

   //����� ������ � ��� ���  
   for(i=0;i<(*tpp_context)->rowcount_to;i++){
   	
   	  wr_ptr = (char*)(*tpp_context)->to_buffer + (*tpp_context)->file_data_shift_to[i]*sizeof(double);
   	  
		  if (((*tpp_context)->shm_datatype_to[i] != 255)&&((*tpp_context)->shm_ptr_to[i] != NULL))  {
		  			  	
		   	if ((*tpp_context)->shm_datatype_to[i] == 0) {

		    	//������ ��� �������������� ������
		     	memcpy(wr_ptr,(*tpp_context)->shm_ptr_to[i],sizeof(double)*(*tpp_context)->shm_dim_to[i]);
		  	   	 
			  }else{  
                //������ � ��������������� �������� 
			  	tmp_var = 0;
          temp_ptr = (*tpp_context)->shm_ptr_to[i];
        
			  	for(j=0;j<(*tpp_context)->shm_dim_to[i];j++){
				    //�������������� �� ���������� ����������
				    if((*tpp_context)->shm_datatype_to[i] == 1){ 
                     memcpy(&tmp_char,temp_ptr,sizeof(char));
                     tmp_var = tmp_char;
                     temp_ptr = temp_ptr + sizeof(char);
				    };
				    //�������������� �� ����� ����������
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
 
   //������������� ����, ��� ��� ��� ���� ����� ����� ���������� ������ �� ���������� ����
   (*tpp_context)->f_havedata=1;

   return 0;
};

#endif