
 /* Специализированные функции: интерполяция, сортировка, генерация случайных чисел */
 
 /* Добавление новой точки в стек транспортного запаздывания (с использованием кольцевого буфера)
   x     - новое значение
   step  - шаг интегрирования
   tarr  - указатель на буфер времени сдвига
   uarr  - указатель на буфер значений
   count - указатель на текущий размер буфера
   max_buffer - максимально допустимое число точек буфера  */
void delay_addpoint(g_real_type x,g_real_type step,g_real_type tau,g_real_type* tarr,g_real_type* uarr,int* count,int max_buffer)
{
	int i,j;
	int del_count;
	/* Добавление новой точки */
	j=(*count)%max_buffer;
	tarr[j]=step;
	uarr[j]=x;	
	/* Инкремент текущего к-ва точек с ограничением по кольцу */
	*count=*count + 1;
    if(*count >= 2*max_buffer) *count=*count - max_buffer;
		
	return;
};	

 /* Вычисление значение транспортного запаздывания с кольцевым буфером
   tau        - время запаздывания
   tarr       - указатель на буфер времени сдвига
   uarr       - указатель на буфер значений
   count      - указатель на текущий размер буфера
   index      - указатель на интерполяционный индекс (индекс последней точки)
   max_buffer - максимально допустимое количество точек стека
   time_sum   - интеграл времени запаздывания  */
g_real_type delay_interp(g_real_type tau,g_real_type* tarr,g_real_type* uarr,int count,int* index,int max_buffer,g_real_type* time_sum)
{
	g_real_type res = 0;
	int i,j,k,iend;
	
	/* Для начала мы делаем сдвиг времени задержки  */
  if(count > max_buffer){
  	 iend=count-max_buffer;
  }else{
  	 iend=0;
  };
  *time_sum = 0;
  j=0;
	for (i=count-1;i>=iend;i--){	
     j=i%max_buffer;	
     *time_sum  = *time_sum  + tarr[j];
   	 if(*time_sum >= tau) {
   	 	 *index = i;
       k = i + 1;
       if((k >= count)||(tarr[k]==0)){
        	res = uarr[j];
       }else{
       	  k = k%max_buffer;  
       	  res = uarr[j] + (tau - *time_sum)*(uarr[k]-uarr[j])/tarr[k]; 
       };			 	
		 	 return res;
		 };	
	};		
	/* Если цикл не нашёл этого временного интервала в пределах буфера */
	if(j<count) res = uarr[j]; 			
	return res;
};	 
 
  /* Поиск интервала методом половинного деления    -  ok
    ax      -  значение аргумента
    aPx[nn] -  массив аргументов, должен быть упорядочен по возрастанию !!!
    nn      -  количество данных
    Index   -  индекс нижней границы интервала  */
void find1(g_real_type ax,g_real_type* apx,int nn,int* index)
{
 int ib;
 int ic;

 /* выбираем начальное приближение поиска с учётом ограничений длины массива
  индекс надо ограничивать в соответствии с размером массива аргументов !!!  */
 if (*index < 0){ 
   *index=0;
 }else{
	 if (*index >= nn) {*index=nn - 1;}};
 ib=*index + 2;
 if (ib >= nn) {ib=nn - 1;};
 if (ax > apx[ib]) {ib=nn - 1;} else {if (ax < apx[*index]) {*index=0;}};
 /* ищем участок интерполяции методом половинного деления */
 while (abs(ib - *index) > 1){
   ic=(*index + ib) / 2;  //div
   if (ax < apx[ic]) {ib=ic;} else {*index=ic;};
 };
 
 return;
};

  /* Одномерная сплайн-интерполяция значения X в соответсвии с матрицей a
    a[nn,mm]  - упорядоченная матрица коэффициентов интерполяции
                a = [X,Y,k1,k2 ...], где X,Y - упорядоченные векторы значений аргумента и функции
    nn        - степень интерполяции + 2 (a.CountX)
    mm        - количество участков интерполяции
    Index     - последний найденный интервал интерполяции */
g_real_type  interpol(g_real_type x,g_real_type** a,int nn,int mm,int* index)
{
  int     i;
  g_real_type  d;
  g_real_type  res;

  /* сначала ищем участок интерполяции index = 0..mm - 1 */
  find1(x,a[0],mm,index);
  /* потом производим расчёт функции заданной степени на этом участке */
  res=a[1][*index];
  x=(x - a[0][*index]);
  d=x;
  for(i=2;i < nn;i++) {
    res=res + d*a[i][*index];
    d=d*x;
  };

  return res;
};

g_real_type simple_interp_1d(int nn, g_real_type* xdata, g_real_type* ydata, g_real_type x, int* index) 
{
  g_real_type res = 0; 	
  g_real_type k;
		
  /*  Поиск  */		
  find1(x, xdata, nn, index);

  /* Ограничение индекса */
  if (*index >= nn - 1) *index = nn - 2;
  
  /* Линейная интерполяция с экстраполяцией при выходе за пределы */
  k = xdata[*index + 1] - xdata[*index];
  if (k != 0) {
	  res = ydata[*index] + (x - xdata[*index])*(ydata[*index + 1] - ydata[*index]) / k;
  }
  else {
	  /* При совпадении границ участков по аргументу выдаём среднее значение */
	  res = 0.5*(ydata[*index + 1] + ydata[*index]);
  };
	
  return res;	
}

/* Сортировка по возрастанию X[nn] и копирование массивов X[nn],Y[nn] в a[0] и a[1] */
void sortarrays(g_real_type* x,g_real_type* y,g_real_type** a,int nn)
{
  int     i;
  int     j;
  g_real_type  tmp;

  for (i=0;i<nn;i++){
    a[0][i]=x[i];
    a[1][i]=y[i];
  };

  for (i=0;i<nn;i++)
	 for (j=i;j<nn;j++)
	   if (a[0][i] > a[0][j]){
        tmp=a[0][i];
        a[0][i]=a[0][j];
        a[0][j]=tmp;
        tmp=a[1][i];
        a[1][i]=a[1][j];
        a[1][j]=tmp;
	   };
	   
	return;   
};

  /* Расчёт матрицы интерполяции для линейной интерполяции
     X[nn]     - массив аргументов
     Y[nn]     - массив значений функции
     a[3,nn]   - выходная матрица интерполяции */
int  linterpcalc(g_real_type* x,g_real_type* y,g_real_type** a,int nn)
{
  int    i;
  g_real_type tmp;
  int    result=0;
  /* упорядочиваем исходные по возрастанию аргумента */
  sortarrays(x,y,a,nn);
  /* расчитываем коэффициенты линейной интерполяции */
  for (i=0;i < nn-1;i++){ 
    tmp =(a[0][i + 1] - a[0][i]);
	if (tmp != 0){
      a[2][i]=(a[1][i + 1] - a[1][i])/tmp;
	}else{ 
      result=1;
      return result;
	};
  };
  //последняя точка принимается равной предпоследней
  a[2][nn - 1]=a[2][nn - 2];
  return result;
};

  /* Вычисляет коэффициенты кубического сплайна y(x) = y[i] + b[i]*(x - x[i]) + c[i]*(x - x[i])^2 + d[i]*(x - x[i])^3
    x[nn],y[nn] - массивы аргументов и значений функции
    yp1,ypn     - значение первой производной в начале и конце сплайна
    IsNatural   - идентификатор натурального сплайна (y'[1] = 0 и y'[n] = 0)  */
void spline(g_real_type* x,g_real_type* y,int nn, g_real_type yp1,g_real_type ypn,char isnatural,g_real_type* b,g_real_type* c,g_real_type* d)
{
   int     i,k,n;
   g_real_type  p,qn,sig,un,h;
   g_real_type* y2;

   n=nn - 1;
   y2=b;
   h=0;
   if (isnatural){
      y2[0] = 0.0;
      c[0]  = 0.0;
   }else {
      y2[0] = -0.5;
      c[0] = (3.0/(x[1]-x[0]))*((y[1]-y[0])/(x[1]-x[0])-yp1);
   };
   for (i=1;i<n;i++){
      sig   = (x[i]-x[i-1])/(x[i+1]-x[i-1]);
      p     = sig*y2[i-1]+2.0;
      y2[i] = (sig-1.0)/p;
      c[i]  = (y[i+1]-y[i])/(x[i+1]-x[i])-(y[i]-y[i-1])/(x[i]-x[i-1]);
      c[i]  = (6.0*c[i]/(x[i+1]-x[i-1])-sig*c[i-1])/p;
   };
   if (isnatural){
      qn = 0.0;
      un = 0.0;
   }else {
      qn = 0.5;
      un = (3.0/(x[n]-x[n-1]))*(ypn-(y[n]-y[n-1])/(x[n]-x[n-1]));
   };
   y2[n] = (un-qn*c[n-1])/(qn*y2[n-1]+1.0);
   for (k = n - 1;k >= 0;k--) {
      y2[k] = y2[k]*y2[k+1]+c[k];
   };
   for (i=0;i<n;i++){
     h=x[i + 1] - x[i];
     c[i]=0.5*y2[i];
     d[i]=(y2[i + 1] - y2[i])/(6*h);
     b[i]=(y[i + 1] - y[i])/h - h*(y2[i + 1] + 2*y2[i])/6;
   };
   c[n]=c[n - 1];
   b[i]=h*c[n];
   
   return;
};


  /* Расчёт матрицы интерполяции для натурального кубического сплайна
     X[nn]     - массив аргументов
     Y[nn]     - массив значений функции
     a[3,nn]   - выходная матрица интерполяции */
int  naturalsplinecalc(g_real_type* x,g_real_type* y,g_real_type** a,int nn)
{
  int i;
  int result = 0;
  /* упорядочиваем исходные по возрастанию аргумента */
  sortarrays(x,y,a,nn);
  /* проверяем на неоднозначность по x */
  for (i=0;i < nn-1;i++) if (a[0][i + 1] == a[0][i]) {
    result=1;
    return result;
  };
  
  /* вычисляем коэффициенты натурального кубического сплайна */
  spline(a[0],a[1],nn,0,0,1,a[2],a[3],a[4]);
  
  return result;
};

  /* Двумерная линейная интерполяция
     arow, acol - значения аргумента по строкам и столбцам
     irow, icol - номер интервала в массивах px1(row),px2(col)
     px1,  px2  - векторы значений аргументов по строкам и столбцам матрицы
     py         - матрица значений функции */
g_real_type  interpol2d(g_real_type arow,g_real_type acol,int irow,int icol,g_real_type* px1,g_real_type* px2,int x1count,int x2count,g_real_type** py)
{
 g_real_type a,b,c,d,y1,y0,x1,x0;
 int    ib1,ib2;
 g_real_type result=0;

 ib1=irow + 1;
 ib2=icol + 1;
 if (ib1 >= x1count){ib1=x1count - 1;};
 if (ib2 >= x2count){ib2=x2count - 1;};
 x0=px1[irow];
 x1=px1[ib1];
 y0=px2[icol];
 y1=px2[ib2];
 a=py[irow][icol];
 if (icol == ib2){ 
   c=0; 
 }else {
   c=(py[irow][ib2]-py[irow][icol])/(y1-y0);
 };
 if (irow == ib1){ 
   d=0;
 }else{
   d=(py[ib1][ib2]-py[ib1][icol]-c*(y1-y0))/((x1-x0)*(y1-y0));
 };
 if (irow == ib1){ 
   b=0;
 }else{ 
   b=(py[ib1][icol]-a)/(x1-x0);
 };
 result=a+b*(arow-x0)+c*(acol-y0)+d*(arow-x0)*(acol-y0);
 
 return result;
};

/*  Интерполяция полиномом лагранжа
    x - аргумент
    x1 - массив значений аргумента
    y1 - массив значений функции
    n - порядок полинома
    m - homep элемента, c kotopoгo необходимо начать интерполяцию */
g_real_type lagrange(g_real_type* x1,g_real_type* y1,g_real_type x,int n,int m)
{
    int    i,j;
    g_real_type p1,p2;
    g_real_type result = 0.0;

	  for (j =0;j<n;j++){
       p1=1.0;
       p2=1.0;
	   for(i=0;i<n;i++){
         if (i == j) continue;
         p1=p1*(x-x1[i+m-1]);
         p2=p2*(x1[j+m-1]-x1[i+m-1]);
	   };
       result=result+p1/p2*y1[j+m-1];
	  };
	  
	  return result;
};

 /* Быстрая сортировка для целых и вещественных массивов */
void iquicksort(int* a,int ilo,int ihi)
{
    int lo, hi, mid, t;

    lo = ilo;
    hi = ihi;
    mid = a[(lo + hi) / 2];
    do {
      while(a[lo] < mid) lo++;
      while (a[hi] > mid) hi--;
      if (lo <= hi) {
        t = a[lo];
        a[lo] = a[hi];
        a[hi] = t;
        lo++;
        hi--;
      }
    }  
    while (lo < hi);
    if (hi > ilo) iquicksort(a, ilo, hi);
    if (lo < ihi) iquicksort(a, lo, ihi);
    	
    return;	
};

void rquicksort(g_real_type* a,int ilo,int ihi)
{
    int lo, hi;
    g_real_type mid, t;

    lo = ilo;
    hi = ihi;
    mid = a[(lo + hi) / 2];
    do {
      while(a[lo] < mid) lo++;
      while (a[hi] > mid) hi--;
      if (lo <= hi) {
        t = a[lo];
        a[lo] = a[hi];
        a[hi] = t;
        lo++;
        hi--;
      }
    }  
    while (lo < hi);
    if (hi > ilo) rquicksort(a, ilo, hi);
    if (lo < ihi) rquicksort(a, lo, ihi);
    	
    return;	
};

void bquicksort(char* a,int ilo,int ihi)
{
    int lo, hi;
    char mid, t;

    lo = ilo;
    hi = ihi;
    mid = a[(lo + hi) / 2];
    do {
      while(a[lo] < mid) lo++;
      while (a[hi] > mid) hi--;
      if (lo <= hi) {
        t = a[lo];
        a[lo] = a[hi];
        a[hi] = t;
        lo++;
        hi--;
      }
    }  
    while (lo < hi);
    if (hi > ilo) bquicksort(a, ilo, hi);
    if (lo < ihi) bquicksort(a, lo, ihi);
    	
    return;	
};

  /* Генерация случайного равномерно распределённого g_real_type числа в даиапазоне 0..1 - тупой вариант */
g_real_type my_random(g_real_type* ainitvalue)
{
	g_real_type res;
	 	 	 
	res = (g_real_type)rand()/(g_real_type)RAND_MAX;
	
	return res;
};	
  
  /* Случайная инициализация состояния ГСЧ */
g_real_type my_randomize()
{
	g_real_type res;
	 
  res = (g_real_type)rand()/(g_real_type)RAND_MAX;
	
	return res;
};

  /* Генерация случайного числа с нормальным распределением, математическим ожиданием Mean и девиацией StdDev */
g_real_type my_randg(g_real_type* ainitvalue,g_real_type Mean,g_real_type StdDev)
{
  /* Marsaglia-Bray algorithm  */ 
  g_real_type U1,S2,U2;  
  do {
    U1 = 2*my_random(ainitvalue) - 1;
    U2 = 2*my_random(ainitvalue) - 1;    	
    S2 = U1*U1 + U2*U2;
  } while (S2 >= 1);
  U2 = sqrt(-2*log(S2)/S2)*U1*StdDev + Mean;
  return U2;
};

  /* Многомерная линейная интерполяция */
static int isearch(g_real_type t, g_real_type* x, int n)
{
    int i1, i2, i;
    if ( x[0] <= t  &&  t <= x[n - 1] )
    {
        i1 = 0;
        i2 = n - 1;
        while ( i2 - i1 > 1 )
        {
            i = (i1 + i2) / 2;
            if ( t <= x[i] )
            {
                i2 = i;
            }
            else
            {
                i1 = i;
            }
        }
        return (i1);
    }
    else
    {
        return (-1);
    }
}

static void fast_int_search(g_real_type xx, g_real_type* x, int nx, int *i)
{
    if ( *i == -1 )
    {
        *i = isearch(xx, x, nx);
    }
    else if ( !  (x[*i] <= xx && xx <= x[*i + 1]) )
    {
        *i = isearch(xx, x, nx);
    }
}

static void coord_by_periodicity(g_real_type *t, g_real_type x[], int n, int *i)
{
    g_real_type r, L;
    L = x[n - 1] - x[0];
    r = (*t - x[0]) / L;
    if (r >= 0.0)
    {
        *t = x[0] + (r - floor(r)) * L;
    }
    else
    {
        *t = x[n - 1] + (r - ceil(r)) * L;
    }

    if (*t < x[0])
    {
        *t = x[0];
        *i = 0;
    }
    else if (*t > x[n - 1])
    {
        *t = x[n - 1];
        *i  = n - 2;
    }
    else
    {
        *i = isearch(*t, x, n);
    }
}

#define IPOL_NATURAL 0
#define IPOL_C0 1
#define IPOL_BY_ZERO 2
#define IPOL_PERIODIC 3

void nlinear_interp(g_real_type *x , g_real_type* val, int* dim, int n,
                    g_real_type *xp, g_real_type* yp, int np, int outmode,
                    g_real_type* u, g_real_type* v, int* ad, int* k)
{
    int i, j, l, p, temp, b, two_p_n, jpos;
    g_real_type xx;

    ad[0] = 0;
    ad[1] = 1;
    temp = 1 ;
    p = 1;
    for ( j = 0; j < n - 1; j++)
    {
        temp = temp * dim[j];
        p = 2 * p;
        for ( i = 0; i < p; i++ )
        {
            ad[p + i] = ad[i] + temp;
        }
    };

    two_p_n = 2 * p;

    for ( j = 0; j < n; j++ )
    {
        k[j] = -1;
    }

    for ( i = 0; i < np; i++ )
    {
		jpos = 0;

        for ( j = 0; j < n; j++ )
        {
            xx = xp[j + i*n];
            if ( !isfinite(xx) )
            {
                v[0] = 0.0;
                goto fin;
            }
            fast_int_search(xx, &x[jpos], dim[j], &(k[j]));
            if ( k[j] == -1 )  
                switch (outmode)
                {
                    case IPOL_BY_ZERO :
                        v[0] = 0.0;
                        goto fin;

                    case IPOL_NATURAL :
                        if (xx < x[jpos])
                        {
                            k[j] = 0;
                        }
                        else
                        {
                            k[j] = dim[j] - 2;
                        }
                        break;

                    case IPOL_C0 :
                        if (xx < x[jpos])
                        {
                            u[j] = 0.0;
                            k[j] = 0;
                        }
                        else
                        {
                            u[j] = 1.0;
                            k[j] = dim[j] - 2;
                        }
                        continue;

                    case IPOL_PERIODIC :
                        coord_by_periodicity(&xx, &x[jpos], dim[j], &(k[j]));
                        break;

                }
            u[j] = (xx - x[jpos+k[j]]) / ( x[jpos+k[j] + 1] -  x[jpos+k[j]]); 
			
			jpos = jpos + dim[j];
        }

        b = k[n - 1];
        for ( j = n - 2; j >= 0; j-- )
        {
            b = k[j] + dim[j] * b;
        }

        for ( j = 0; j < two_p_n; j++ )
        {
            v[j] = val[b + ad[j]];
        }

        temp = 1;
        p = two_p_n;
        for ( j = 0; j < n ; j++ )
        {
            for ( l = 0; l < two_p_n; l += 2 * temp)
            {
                v[l] = v[l] * (1.0 - u[j]) + v[l + temp] * u[j];
            }
            p = p / 2;
            temp = 2 * temp;
        }

fin:
        yp[i] = v[0];

    }
}

  /* Многомерная ступенчатая интерполяция */
void nstep_interp(g_real_type *x , g_real_type* val, int* dim, int n,
                  g_real_type *xp, g_real_type* yp, int np, int outmode,
                  int* k)
{
    int i, j, b, jpos;
    g_real_type xx;

    for ( j = 0; j < n; j++ )
    {
        k[j] = -1;
    }

    for ( i = 0; i < np; i++ )
    {
		jpos = 0;

        for ( j = 0; j < n; j++ )
        {
            xx = xp[j + i*n];
            if ( !isfinite(xx) )
            {
                yp[i] = 0.0;
                goto fin;
            }
            fast_int_search(xx, &x[jpos], dim[j], &(k[j]));
            if ( k[j] == -1 )  
                switch (outmode)
                {
                    case IPOL_BY_ZERO :
                        yp[i] = 0.0;
                        goto fin;

                    case IPOL_NATURAL :
                        if (xx < x[jpos])
                        {
                            k[j] = 0;
                        }
                        else
                        {
                            k[j] = dim[j] - 1;
                        }
                        break;

                    case IPOL_C0 :
                        if (xx < x[jpos])
                        {                            
                            k[j] = 0;
                        }
                        else
                        {               
                            k[j] = dim[j] - 1;
                        }
                        continue;

                    case IPOL_PERIODIC :
                        coord_by_periodicity(&xx, &x[jpos], dim[j], &(k[j]));
                        break;

                }
			
			jpos = jpos + dim[j];
        }

        b = k[n - 1];
        for ( j = n - 2; j >= 0; j-- )
        {
            b = k[j] + dim[j] * b;
        }
		
		yp[i] = val[b];

fin:   ;

    }
}

  //Поиск интервала в массиве аргументов методом половинного деления
void tablefind(g_real_type ax,g_real_type *Xa, g_real_type *Xb, int *Ia, int *Ib,g_real_type *aPx)
{
  int Ic;
  int k;  

  k = 1;
  if (aPx[*Ia] > aPx[*Ib] ){
    Ic  = *Ia;
    *Ia = *Ib;
    *Ib = Ic;
    k   = -1;
  };

 if (ax <= aPx[*Ia]) { 
   *Ib = *Ia+k;
 }else{
	if (ax >= aPx[*Ib]) *Ia = *Ib - k;
 };	 

 while (abs(*Ib - *Ia) > 1) {
  Ic = (*Ia + *Ib) / 2;
  if (ax < aPx[Ic]) {
	*Ib = Ic; 
  }else{ 
    *Ia = Ic;
  };	
 }; 
 
 *Xa = aPx[*Ia];
 *Xb = aPx[*Ib];
};

  //Двумерная линейная интерполяция с экстраполяцией
g_real_type table2dgetfunvalue(g_real_type arow,g_real_type acol,g_real_type *Px1, g_real_type *Px2, int rowcount, int colcount, g_real_type *Py)
{
 int Ia1,Ib1,Ia2,Ib2;
 g_real_type a,b,c,d,x0,x1,y0,y1,res;

 Ia1 = 0;
 Ib1 = rowcount - 1;
 tablefind(arow,&x0,&x1,&Ia1,&Ib1,Px1);

 Ia2 = 0;
 Ib2 = colcount - 1;
 tablefind(acol,&y0,&y1,&Ia2,&Ib2,Px2);

 a = Py[Ia1*colcount + Ia2];

 if (Ia2 == Ib2) {
   c = 0;   
 }else{
   c = (Py[Ia1*colcount + Ib2]-Py[Ia1*colcount + Ia2])/(y1-y0);
 };  

 if (Ia1 == Ib1) {
   d = 0;
 }else{
   d = (Py[Ib1*colcount + Ib2]-Py[Ib1*colcount + Ia2]-c*(y1-y0))/((x1-x0)*(y1-y0));
 };  

 if (Ia1 == Ib1) {
   b = 0;
 }else{
   b = (Py[Ib1*colcount + Ia2]-a)/(x1-x0);
 };  

 res = a+b*(arow-x0)+c*(acol-y0)+d*(arow-x0)*(acol-y0);
 
 return res;
};  

  //Двумерная интерполяция без экстраполяции при выходе за диапазон
g_real_type table2dgetfunvaluenotextra(g_real_type arow,g_real_type acol,g_real_type *Px1, g_real_type *Px2, int rowcount, int colcount, g_real_type *Py)
{
  int Ia1, Ib1, Ia2, Ib2;
  g_real_type a, b, c, d, x0, x1, y0, y1, res;

  Ia1 = 0;
  Ib1 = rowcount - 1;
  tablefind(arow, &x0, &x1, &Ia1, &Ib1, Px1);

  // вставка для случая когда аргумент ax не между двух соседних узлов а слева
  if (arow < x0) {
    Ib1 = Ia1;                // меньше наименьшего
  }else{ 
    if (arow > x1) Ia1 = Ib1; // больше наибольшего
  };	

  Ia2 = 0;
  Ib2 = colcount - 1;
  tablefind(acol, &y0, &y1, &Ia2, &Ib2, Px2);

  // вставка для случая когда аргумент ax не между двух соседних узлов а слева
  if (acol < y0) {
    Ib2 = Ia2;                // меньше наименьшего
  }else{ 
    if (acol > y1) Ia2 = Ib2; // больше наибольшего
  };	

 a = Py[Ia1*colcount + Ia2];

 if (Ia2 == Ib2) { 
   c = 0; 
 }else{ 
   c = (Py[Ia1*colcount + Ib2]-Py[Ia1*colcount + Ia2])/(y1-y0);
 };

 if (Ia1 == Ib1) { 
   d = 0;   
 }else{ 
   d = (Py[Ib1*colcount + Ib2]-Py[Ib1*colcount + Ia2]-c*(y1-y0))/((x1-x0)*(y1-y0));
 };  

 if (Ia1 == Ib1) { 
   b = 0;
 }else{ 
   b = (Py[Ib1*colcount + Ia2]-a)/(x1-x0);
 };  

 res = a+b*(arow-x0)+c*(acol-y0)+d*(arow-x0)*(acol-y0);

 return res;
};

  //Двумерная ступенчатая интерполяция
g_real_type table2dgetfunvaluenotinterp(g_real_type arow,g_real_type acol,g_real_type *Px1, g_real_type *Px2, int rowcount, int colcount, g_real_type *Py)
{
  int Ia1, Ib1, Ia2, Ib2;
  g_real_type a, b, c, d, x0, x1, y0, y1, res;

  Ia1 = 0;
  Ib1 = rowcount - 1;
  tablefind(arow, &x0, &x1, &Ia1, &Ib1, Px1);

  // вставка для случая когда аргумент ax не между двух соседних узлов а слева
  if (arow < x0) {
    Ib1 = Ia1;                // меньше наименьшего
  }else{ 
    if (arow > x1) Ia1 = Ib1; // больше наибольшего
  };	

  Ia2 = 0;
  Ib2 = colcount - 1;
  tablefind(acol, &y0, &y1, &Ia2, &Ib2, Px2);

  // вставка для случая когда аргумент ax не между двух соседних узлов а слева
  if (acol < y0) {
    Ib2 = Ia2;                // меньше наименьшего
  }else{ 
    if (acol > y1) Ia2 = Ib2; // больше наибольшего
  };	

  res = Py[Ib1*colcount + Ib2];
  
  return res;
};

