 /* Матричные вычисления над действительными числами */
 
 /* Умножение двух неразреженных матриц */
void matrix_mul(g_real_type* result,g_real_type* a, int a_rows, int a_cols, g_real_type* b, int b_rows, int b_cols)
{
	int i;
	int j;
	int k;
	g_real_type tmp;
			
    for(i=0; i < a_rows; i++){
      for(j=0; j < b_cols;j++){
        tmp = 0.0;		
        for(k=0; k < a_cols;k++){			
          tmp = tmp +  a[k + i*a_cols]*b[j + k*b_cols];		  
		};  		  
		result[j + i*b_cols] = tmp; 	  
      };
	};  
		
};
 
 
 /* транспонирование неразреженной матрицы */
void matrix_transp(g_real_type* result,g_real_type* a, int a_rows, int a_cols)
{
	int i;
	int j;
	
    for (i = 0; i < a_cols; i++){
      for(j = 0; j < a_rows; j++){
	     result[j + i*a_rows] = a[i + j*a_cols];	
	  }; 
	};		
}; 
 
 /* LU-декомпозиция квадратной матрицы*/
int  ludcmp(g_real_type* a, int nn,
            int* indx, int* swap, g_real_type* d, 
			g_real_type* vv, g_real_type tiny)
{			
   int k;
   int j;
   int imax;
   int i;
   int n;
   int Result;
   int pdum;
   g_real_type sum;
   g_real_type dum;
   g_real_type big;

   Result = 0;
   *d = 1.0;
   n = nn - 1;
   imax = 0;
   for(i = 0; i <= n;i++){
	  swap[i] = i; 
      big = 0.0;	  
      for(j = 0; j <= n; j++){ 
	    if (abs(a[i*nn + j]) > big) big = abs(a[i*nn + j]);
	  };	
      if (big == 0.0){
         Result = 1;
         return Result;
      };
      vv[i] = 1.0/big;
   };
   for (j = 0; j <= n; j++){
      if (j > 0) {
         for (i = 0; i <= j - 1;i++){
            sum = a[swap[i]*nn+j];
            if (i > 0) {
               for (k = 0; k <= i - 1;k++){
                  sum = sum - a[swap[i]*nn+k]*a[swap[k]*nn+j];
               };
               a[swap[i]*nn+j] = sum;
            };
         };
      };
      big = 0.0;
      for (i = j; i <= n;i++){
         sum = a[swap[i]*nn+j];
         if (j > 0) {
            for (k = 0;k <= j - 1;k++){
               sum = sum - a[swap[i]*nn+k]*a[swap[k]*nn+j];
            };
            a[swap[i]*nn+j] = sum;
         };
         dum = vv[i]*abs(sum);
         if (dum > big) {
            big = dum;
            imax = i;
         };
      };
      if (j != imax) {
		  
         /* Используем переключение индексов вместо переноса данных соотвественно потом надо
		  при вызове lubksb использовать этот массив для определения номера строки */
		 
		 pdum = swap[imax];
		 swap[imax] = swap[j];
		 swap[j] = pdum;
		 
         *d = -*d;
         vv[imax] = vv[j];
      };
      indx[j] = imax;
      if (j != n) {
         if (a[swap[j]*nn+j] == 0.0) {a[swap[j]*nn+j] = tiny;};
         dum = 1.0/a[swap[j]*nn+j];
         for(i = j+1; i <= n; i++){
            a[swap[i]*nn+j] = a[swap[i]*nn+j]*dum;
         };
      };	  
   };
   if (a[swap[n]*nn+n] == 0.0) a[swap[n]*nn+n] = tiny;
   
   return Result;
};
  
   /* Решение системы линейных алгебраических уравнений */ 
void lubksb(g_real_type* a, int nn, int* indx, int* swap, g_real_type* b)
{
   int j;
   int ip;
   int ii;
   int i;
   int n;
   g_real_type sum;

   ii = -1;
   n = nn - 1;
   for( i = 0; i <= n;i++){
      ip = indx[i];
      sum = b[ip];
      b[ip] = b[i];
      if  (ii != -1) {
         for (j = ii; j <= i-1;j++){
            sum = sum - a[i*nn + j]*b[j];
         };
      } else{ 
	    if (sum != 0.0) {
          ii = i;
        };
	  };
      b[i] = sum;
   };
   for (i = n; i >= 0 ;i--){
      sum = b[i];
      if (i < n) {
	    for (j = i + 1; j <= n; j++){
            sum = sum - a[swap[i]*nn+j]*b[j];
	    };
      };
      b[i] = sum/a[swap[i]*nn+i];
   };
};

  /* решение системы линейных алгебраических уравнений  A*x = b */
int  solvelsys(g_real_type* a, g_real_type* b, int nn, int* indx, int* swap, g_real_type* vv)
{
  g_real_type d;
  int Result;
  Result = ludcmp(a,nn,indx,swap,&d,vv,1.0e-20);
  if (Result == 0) lubksb(a,nn,indx,swap,b);
  return Result;
};
  
  /* решение системы линейных алгебраических уравнений  A*x = b с резервированием массивов */
void lsolve(g_real_type* x, g_real_type* a, int a_row, int a_col, g_real_type* b, int b_row, int b_col, 
            g_real_type* a_temp, int* indx, int* swap, g_real_type* vv)
{
	
  g_real_type d;
  int Result;
  int i;
  
  /* Копируем матрицу и вектор правых частей в буферы (т.к. матрица меняется после вызова ludcmp) */
  /* b -> x */
  for(i=0;i < b_col;i++) x[i] = b[i];
    
  /* a -> a_temp */
  for(i=0;i < a_row*a_col;i++) a_temp[i] = a[i];
  
  /* Производим вычисления */
  Result = ludcmp(a_temp,a_row,indx,swap,&d,vv,1.0e-20);
  if (Result == 0) lubksb(a_temp,a_row,indx,swap,x);	
		
};

void det(double* result, g_real_type* a, int a_row, int a_col,
         g_real_type* a_temp, int* indx, int* swap, g_real_type* vv)
{
 int i;
 
  /* a -> a_temp */
 for(i=0;i < a_row*a_col;i++) a_temp[i] = a[i];
 
 /* Вычисляем определитель матрицы */
 if (ludcmp(a_temp,a_row,indx,swap,result,vv,1.0e-20) == 0) {
   for(i = 0; i < a_row; i++)
       *result = *result*a_temp[swap[i]*a_col+i];
 }else{
   *result = 0.0;
 };  
};


