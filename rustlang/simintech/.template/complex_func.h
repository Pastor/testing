
#ifndef complex_funcs_header
#define complex_funcs_header

const g_complex_type cpOne = { 1, 0 };

  /*Упаковка двух чисел в комплексное */
g_complex_type complex_pack(g_real_type re,g_real_type im)
{
	g_complex_type  res;	
	res.re = re;
	res.im = im;	
	return res;
};

  /*Аргумент комплексного числа */
g_real_type complex_arg(g_complex_type x)
{
	g_real_type  res;		
	res = atan2(x.im,x.re);	
	return res;
};

  /*Модуль комплексного числа */
g_real_type complex_abs(g_complex_type x)
{
	g_real_type  res;		
	res = sqrt(x.re*x.re + x.im*x.im);	
	return res;
} 

g_real_type complex_real(g_complex_type x)
{
	g_real_type  res;
	res = x.re;
	return res;
}

g_real_type complex_imag(g_complex_type x)
{
	g_real_type  res;
	res = x.im;
	return res;
}

  /*Инверсия числа */
g_complex_type complex_inv(g_complex_type x)
{
	g_complex_type res;
	res.re = -x.re;
	res.im = -x.im;
	return res;
};	
  
  /*Сложение чисел */
g_complex_type complex_add(g_complex_type x1,g_complex_type x2)
{
	g_complex_type res;
	res.re = x1.re + x2.re;
	res.im = x1.im + x2.im;
	return res;
};	

  /*Вычитание */
g_complex_type complex_sub(g_complex_type x1,g_complex_type x2)
{
	g_complex_type res;
	res.re = x1.re - x2.re;
	res.im = x1.im - x2.im;
	return res;
};	

  /*Перемножение */
g_complex_type complex_mul(g_complex_type x1,g_complex_type x2)
{
	g_complex_type res;	
	res.re = x1.re*x2.re - x1.im*x2.im;
	res.im = x1.re*x2.im + x1.im*x2.re;
	return res;
};	

  /*Деление */
g_complex_type complex_div(g_complex_type x1,g_complex_type x2)
{
	g_complex_type res;
	g_real_type det;	
  det = x2.re*x2.re + x2.im*x2.im;
  if(det!=0){  	
  	res.re = (x1.re*x2.re + x1.im*x2.im)/det;
   	res.im = (x1.im*x2.re - x1.re*x2.im)/det;
  }else{
  	res.re = 0;
  	res.im = 0;
  };
	return res;
};	

  /*Вычисление комплексной экспоненты */
g_complex_type complex_exp(g_complex_type x)
{
	g_complex_type res;
	g_real_type r1;		
	r1 = exp(x.re);	
	res.re = r1*cos(x.im);
	res.im = r1*sin(x.im);
	return res;
};	

g_complex_type complex_sqrt(g_complex_type a)
{
	g_complex_type res;
	g_real_type d;
	g_real_type b;

	d = complex_abs(a);
	if (a.re > 0) {
		res.re = sqrt((d + a.re) / 2);
		res.im = a.im / (2 * res.re);
	}
	else {
		b = a.im;
		res.im = sqrt((d - a.re) / 2);
		if (res.im == 0) {
			res.re = 0;
		}
		else {
			if (b < 0) res.im = -res.im;
			res.re = b / (2 * res.im);
		};
	};

	return res;
};

g_complex_type complex_int_pow(g_complex_type ABase,g_int_type APower)
{
	g_complex_type Res;
	
	int Y;
	g_complex_type LBase;
	
	if (ABase.im == 0.0) {
		Res.re = pow(ABase.re, APower);
		Res.im = 0;
	}
	else {
		Y = abs(APower);
		LBase = ABase;
		Res.re = 1.0;
		Res.im = 0.0;
		while (Y > 0) {
			while ((Y & 1) == 0) {
				Y = Y >> 1;
				LBase = complex_mul(LBase, LBase);
			};
			Y--;
			Res = complex_mul(Res, LBase);
		};
		if (APower < 0) Res = complex_div(cpOne, Res);
	};
	
	return Res;
};	

char  cpLn(g_complex_type  A,g_complex_type* Res)
{
  char ReturnValue;	 
  g_real_type R;

  R = complex_abs(A);
  
  if (R > 0) {
    Res->im = atan2(A.im,A.re);
    Res->re = log(R);
    ReturnValue = 1;
  }
  else {
    ReturnValue = 0;
  };	
  
  return ReturnValue;
};

/*Вычисление комплексного логарифма */
g_complex_type complex_ln(g_complex_type x)
{
	g_complex_type res;

	cpLn(x, &res);

	return res;
};

g_complex_type complex_real_pow(g_complex_type ABase,g_real_type APower)
{
	g_complex_type Res;	
	g_real_type    int_part;
	
    if (APower == 0.0) {
      Res.re = 1.0;               // n**0 = 1 
      Res.im = 0;
    }
    else
    if ((ABase.re == 0.0) && (ABase.im == 0.0) && (APower > 0.0)) {
      Res.re = 0.0;               // 0**n = 0, n > 0 
      Res.im = 0;
    }
    else
    if ((modf(APower,&int_part) == 0.0) && (abs(APower) <= 2147483647)) {
      Res = complex_int_pow(ABase,APower);
    }
    else {
      if ( cpLn(ABase,&Res) ) {
        Res.re = Res.re*APower;
        Res.im = Res.im*APower;
        Res = complex_exp(Res);
      }
      else {
        Res.re = 0;
        Res.im = 0;
      };
    };	
		
	return Res;
};	



#endif
