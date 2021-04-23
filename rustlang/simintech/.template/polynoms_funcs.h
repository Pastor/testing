
#include "complex_func.h"

void Horner(g_real_type* A, int n, g_complex_type z, g_real_type* B, g_complex_type* f)
{
  int k;
  g_real_type q;
  g_real_type r;
  g_real_type bm1;

  if (n == 0) {
       B[0] = 0; 
	   f->re = A[0]; 
	   f->im = 0; 
  } else {
     if (z.im == 0) 
	     { 
           B[n-1] = A[n];
           for (k=n-2;k >= 0;k--) B[k] = A[k+1]+B[k+1]*z.re;
           f->re = A[0]+B[0]*z.re; 
		   f->im = 0;
         } 
		 else {
           if (n == 1) { 
	          B[0] = 0; 
			  f->re = A[0]+A[1]*z.re; 
			  f->im = A[1]*z.im; 
		   }
           else {
             r = 2*z.re; 
			 q =-(z.re*z.re)-(z.im*z.im);
             B[n-1] = 0; 
			 B[n-2] = A[n];
		     for (k=n-3;k >= 0;k--) B[k]=A[k+2]+r*B[k+1]+q*B[k+2];
             bm1 = A[1]+r*B[0]+q*B[1];
             f->re = A[0]+z.re*bm1+q*B[0]; 
			 f->im = bm1*z.im;
           };
		 };  
	 };	 	 
};

void PolyRoots(g_real_type* A, int n,
	int* nr, g_complex_type* Roots,
	int sort,
	g_real_type* B, g_real_type* B1, g_real_type* B2, g_real_type* B3)
{

	const g_real_type EPS = 1e-12;
	const g_real_type EPS_Im = 1.0e-6;
	const g_real_type imax = 10;
	const g_real_type iimax = 10;

	int  i;
	int  ii;
	int  j;
	int  k;
	int  m;
	g_real_type  tmpsin;
	g_real_type  tmpcos;
	g_real_type  g;
	g_real_type g1;
	g_real_type g2;
	g_real_type ep;
	g_complex_type f;
	g_complex_type f1;
	g_complex_type f2;
	g_complex_type c1;
	g_complex_type c2;
	g_complex_type z;
	g_complex_type dz;
	char condition;

	*nr = 0;
	m = n;
	for (k = 0; k <= n; k++) B[k] = A[k];
	z.re = 0.0;

lbl1:

	z.im = 0.0;
	if (m == 0) goto lbl3;
	if (B[m] == 0.0) {
		m = m - 1;
		goto lbl1;
	};
	if (B[0] == 0.0) {
		for (k = 0; k < m; k++) B[k] = B[k + 1];
		Roots[*nr].re = 0;
		Roots[*nr].im = 0;
		*nr = *nr + 1;
		m = m - 1;
		goto lbl1;
	};
	if (m == 1) {
		Roots[*nr].re = -B[0] / B[1];
		Roots[*nr].im = 0;
		*nr = *nr + 1;
		m = m - 1;
		goto lbl1;
	};

	if ((z.re == 0) && (m > 2)) {
		g = 0.0;
		for (k = 1; k <= m; k++) {
			g1 = B[k] / B[0];
			g2 = fabs(g1);
			if (g2 > 0.0) g2 = exp(log(g2) / k);
			if (g2 > fabs(g)) {
				g = g2;
				if (g1 > 0) g = -g;
			};
		};
		if (fabs(g) > 0) z.re = 1 / g;
	};

	for (k = 1; k <= m; k++) B1[k - 1] = k*B[k];
	for (k = 1; k < m; k++) B2[k - 1] = k*B1[k];

	ep = EPS;
	for (ii = 1; ii <= iimax; ii++) {
		for (i = 1; i <= imax; i++) {
			Horner(B, m, z, B3, &f);
			Horner(B1, m - 1, z, B3, &f1);
			Horner(B2, m - 2, z, B3, &f2);
			if (complex_abs(f) == 0) {
				goto lbl2;
			}
			else
				if (complex_abs(f1) > 0) {
					c1 = complex_div(f, f1);
					c1.re = -2 * c1.re;
					c1.im = -2 * c1.im;
					c2 = complex_div(f2, f1);
					c2 = complex_mul(c1, c2);
					c2.re = c2.re + 1;
					c2 = complex_sqrt(c2);
					c2.re = c2.re + 1;
					dz = complex_div(c1, c2);
				}
				else
					if (complex_abs(f2) > 0) {
						c1 = complex_div(f, f2);
						c2.re = -2 * c1.re;
						c2.im = -2 * c1.im;
						dz = complex_sqrt(c2);
					}
					else
					{
						sincos(i, &tmpsin, &tmpcos);
						dz.re = z.re*tmpsin;
						dz.im = z.im*tmpsin;
						dz.re = dz.re + tmpcos;
					};
			z = complex_add(z, dz);
			if (complex_abs(dz) <= (ep*complex_abs(z))) goto lbl2;
		};
		ep = 10 * ep;
	};

lbl2:

	if (fabs(z.im) <= (EPS_Im*fabs(z.re))) z.im = 0;
	if (fabs(z.re) <= (ep*fabs(z.im))) z.re = 0;
	Horner(B, m, z, B3, &f);
	Roots[*nr].re = z.re;
	Roots[*nr].im = z.im;
	*nr = *nr + 1;
	m = m - 1;
	if (z.im != 0) {
		Roots[*nr].re = z.re;
		Roots[*nr].im = -z.im;
		*nr = *nr + 1;
		m = m - 1;
	};
	for (k = 0; k <= m; k++) B[k] = B3[k];
	goto lbl1;

lbl3:

	for (i = 0; i < (*nr - 1); i++)
		for (j = i + 1; j < *nr; j++) {

			if (sort == 1) { 
				 condition = (Roots[j].re > Roots[i].re); 
			}
			else {
				if (sort == 2) {
					c1 = complex_pack(Roots[i].re, Roots[i].im);
					c2 = complex_pack(Roots[j].re, Roots[j].im);
					condition = (complex_abs(c2) > complex_abs(c1));
				}
				else {
					condition = 0;
				};
			};

		if (condition) {
		   g = Roots[i].re; 
		   Roots[i].re = Roots[j].re; 
		   Roots[j].re = g;
	       g = Roots[i].im; 
		   Roots[i].im = Roots[j].im; 
		   Roots[j].im = g;
		};
	};

};
