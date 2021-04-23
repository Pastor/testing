#ifndef dsplib
#define dsplib

#define MAXFACTORS 32

typedef int factors_array[2 * MAXFACTORS];

#define C_MUL(m,a,b) \
    do{ (m).re = (a).re*(b).re - (a).im*(b).im;\
        (m).im = (a).re*(b).im + (a).im*(b).re; }while(0)

#   define C_MULBYSCALAR( c, s ) \
    do{ (c).re *= (s);\
        (c).im *= (s); }while(0)
#define  C_ADD( res, a,b)\
    do { \
	    (res).re=(a).re+(b).re;  (res).im=(a).im+(b).im; \
    }while(0)
#define  C_SUB( res, a,b)\
    do { \
	    (res).re=(a).re-(b).re;  (res).im=(a).im-(b).im; \
    }while(0)
#define C_ADDTO( res , a)\
    do { \
	    (res).re += (a).re;  (res).im += (a).im;\
    }while(0)

#define C_SUBFROM( res , a)\
    do {\
	    (res).re -= (a).re;  (res).im -= (a).im; \
    }while(0)

#  define HALF_OF(x) ((x)*.5)

void bfly2(g_complex_type * Fout, const int fstride, g_complex_type *twiddles, int m) {
	g_complex_type * Fout2;
	g_complex_type * tw1 = twiddles;
	g_complex_type t;
    Fout2 = Fout + m;
    do {
      C_MUL(t, *Fout2, *tw1);
      tw1 += fstride;
      C_SUB(*Fout2, *Fout, t);
      C_ADDTO(*Fout, t);
      ++Fout2;
      ++Fout;
    } while (--m);
}

void bfly4(g_complex_type * Fout, const int fstride, g_complex_type *twiddles, const int m, char inverse) {
	g_complex_type *tw1, *tw2, *tw3;
	g_complex_type scratch[6];
  int k = m;
  const int m2 = 2 * m;
  const int m3 = 3 * m;


  tw3 = tw2 = tw1 = twiddles;

  do {

    C_MUL(scratch[0], Fout[m], *tw1);
    C_MUL(scratch[1], Fout[m2], *tw2);
    C_MUL(scratch[2], Fout[m3], *tw3);

    C_SUB(scratch[5], *Fout, scratch[1]);
    C_ADDTO(*Fout, scratch[1]);
    C_ADD(scratch[3], scratch[0], scratch[2]);
    C_SUB(scratch[4], scratch[0], scratch[2]);
    C_SUB(Fout[m2], *Fout, scratch[3]);
    tw1 += fstride;
    tw2 += fstride * 2;
    tw3 += fstride * 3;
    C_ADDTO(*Fout, scratch[3]);

    if (inverse) {
      Fout[m].re = scratch[5].re - scratch[4].im;
      Fout[m].im = scratch[5].im + scratch[4].re;
      Fout[m3].re = scratch[5].re + scratch[4].im;
      Fout[m3].im = scratch[5].im - scratch[4].re;
    }
    else {
      Fout[m].re = scratch[5].re + scratch[4].im;
      Fout[m].im = scratch[5].im - scratch[4].re;
      Fout[m3].re = scratch[5].re - scratch[4].im;
      Fout[m3].im = scratch[5].im + scratch[4].re;
    }
    ++Fout;
  } while (--k);
}

void bfly3(g_complex_type * Fout, const int fstride, g_complex_type *twiddles, int m) {
  int k = m;
  const int m2 = 2 * m;
  g_complex_type *tw1, *tw2;
  g_complex_type scratch[5];
  g_complex_type epi3;
  epi3 = twiddles[fstride*m];

  tw1 = tw2 = twiddles;

  do {
    C_MUL(scratch[1], Fout[m], *tw1);
    C_MUL(scratch[2], Fout[m2], *tw2);

    C_ADD(scratch[3], scratch[1], scratch[2]);
    C_SUB(scratch[0], scratch[1], scratch[2]);
    tw1 += fstride;
    tw2 += fstride * 2;

    Fout[m].re = Fout->re - HALF_OF(scratch[3].re);
    Fout[m].im = Fout->im - HALF_OF(scratch[3].im);

    C_MULBYSCALAR(scratch[0], epi3.im);

    C_ADDTO(*Fout, scratch[3]);

    Fout[m2].re = Fout[m].re + scratch[0].im;
    Fout[m2].im = Fout[m].im - scratch[0].re;

    Fout[m].re -= scratch[0].im;
    Fout[m].im += scratch[0].re;

    ++Fout;
  } while (--k);
}

void bfly5(g_complex_type * Fout, const int fstride, g_complex_type *atwiddles, int m) {
	g_complex_type *Fout0, *Fout1, *Fout2, *Fout3, *Fout4;
  int u;
  g_complex_type scratch[13];
  g_complex_type * twiddles = atwiddles;
  g_complex_type *tw;
  g_complex_type ya, yb;
  ya = twiddles[fstride*m];
  yb = twiddles[fstride * 2 * m];

  Fout0 = Fout;
  Fout1 = Fout0 + m;
  Fout2 = Fout0 + 2 * m;
  Fout3 = Fout0 + 3 * m;
  Fout4 = Fout0 + 4 * m;

  tw = atwiddles;
  for (u = 0; u<m; ++u) {
    scratch[0] = *Fout0;

    C_MUL(scratch[1], *Fout1, tw[u*fstride]);
    C_MUL(scratch[2], *Fout2, tw[2 * u*fstride]);
    C_MUL(scratch[3], *Fout3, tw[3 * u*fstride]);
    C_MUL(scratch[4], *Fout4, tw[4 * u*fstride]);

    C_ADD(scratch[7], scratch[1], scratch[4]);
    C_SUB(scratch[10], scratch[1], scratch[4]);
    C_ADD(scratch[8], scratch[2], scratch[3]);
    C_SUB(scratch[9], scratch[2], scratch[3]);

    Fout0->re += scratch[7].re + scratch[8].re;
    Fout0->im += scratch[7].im + scratch[8].im;

    scratch[5].re = scratch[0].re + scratch[7].re * ya.re + scratch[8].re * yb.re;
    scratch[5].im = scratch[0].im + scratch[7].im * ya.re + scratch[8].im * yb.re;

    scratch[6].re = scratch[10].im * ya.im + scratch[9].im * yb.im;
    scratch[6].im = -scratch[10].re * ya.im - scratch[9].re * yb.im;

    C_SUB(*Fout1, scratch[5], scratch[6]);
    C_ADD(*Fout4, scratch[5], scratch[6]);

    scratch[11].re= scratch[0].re + scratch[7].re * yb.re + scratch[8].re * ya.re;
    scratch[11].im = scratch[0].im + scratch[7].im * yb.re + scratch[8].im * ya.re;
    scratch[12].re = -scratch[10].im * yb.im + scratch[9].im * ya.im;
    scratch[12].im = scratch[10].re * yb.im - scratch[9].re * ya.im;

    C_ADD(*Fout2, scratch[11], scratch[12]);
    C_SUB(*Fout3, scratch[11], scratch[12]);

    ++Fout0; ++Fout1; ++Fout2; ++Fout3; ++Fout4;
  }
}

/* perform the butterfly for one stage of a mixed radix FFT */
void bfly_generic(g_complex_type * Fout, const int fstride, g_complex_type *atwiddles, int m, int p, g_complex_type *scratch,int nfft) {
  int u, k, q1, q;
  g_complex_type * twiddles = atwiddles;
  g_complex_type t;
  int Norig = nfft;

  for (u = 0; u<m; ++u) {
    k = u;
    for (q1 = 0; q1<p; ++q1) {
      scratch[q1] = Fout[k];
      k += m;
    }

    k = u;
    for (q1 = 0; q1<p; ++q1) {
      int twidx = 0;
      Fout[k] = scratch[0];
      for (q = 1; q<p; ++q) {
        twidx += fstride * k;
        if (twidx >= Norig) twidx -= Norig;
        C_MUL(t, scratch[q], twiddles[twidx]);
        C_ADDTO(Fout[k], t);
      }
      k += m;
    }
  }
}


void work(g_complex_type * Fout, const g_complex_type * f, const int fstride, int in_stride, int * factors, g_complex_type *atwiddles, g_complex_type *scratch, char inverse, int nfft) {
  g_complex_type * Fout_beg = Fout;
  const int p = *factors++; /* the radix  */
  const int m = *factors++; /* stage's fft length/p */
  const g_complex_type * Fout_end = Fout + p*m;

  if (m == 1) {
    do {
      *Fout = *f;
      f += fstride*in_stride;
    } while (++Fout != Fout_end);
  }
  else {
    do {
      // recursive call:
      // DFT of size m*p performed by doing
      // p instances of smaller DFTs of size m, 
      // each one takes a decimated version of the input
      work(Fout, f, fstride*p, in_stride, factors, atwiddles, scratch, inverse, nfft);
      f += fstride*in_stride;
    } while ((Fout += m) != Fout_end);
  }

  Fout = Fout_beg;

  // recombine the p smaller DFTs 
  switch (p) {
  case 2: bfly2(Fout, fstride, atwiddles, m); break;
  case 3: bfly3(Fout, fstride, atwiddles, m); break;
  case 4: bfly4(Fout, fstride, atwiddles, m, inverse); break;
  case 5: bfly5(Fout, fstride, atwiddles, m); break;
  default:
	  bfly_generic(Fout, fstride, atwiddles, m, p, scratch, nfft);
	  break;
  }
}

int fft_stride(int * factors, g_complex_type *atwiddles, g_complex_type *fin, g_complex_type *fout, int in_stride, g_complex_type *scratch, char inverse, int nfft) {
  if (fin == fout) {
    return 1;
  }
  work(fout, fin, 1, in_stride, factors, atwiddles, scratch, inverse, nfft);
  return 0;
}

int fft(int size, void *fin, void *fout, char isInverse, void *atwiddles, void *factors, void *scratch) {
  int i;
  g_real_type scalingFactor = 1. / (g_real_type) size;
 

  if (fft_stride((int*)factors, (g_complex_type*)atwiddles, (g_complex_type*)fin, (g_complex_type*)fout, 1, (g_complex_type*)scratch, isInverse, size)) {
    return 1;
  }
  if (isInverse) {
    for (i = 0; i < size; ++i) {
      C_MULBYSCALAR(((g_complex_type*)fout)[i], scalingFactor);
    }
  }

  return 0;
}

void BufferPushRight(int bufferSize, g_real_type *bufferArray, g_real_type newValue) {
  int outIndex;
  for (outIndex = bufferSize - 1; outIndex > 0; --outIndex) {
    bufferArray[outIndex] = bufferArray[outIndex - 1];
  }
  bufferArray[0] = newValue;
  return;
}

//фильтрация сигнала во временной области по рекурсивному соотношению
int RecursiveFilterProc(
  char reset,
  int delay,
  void*    nominatorCoefficients,
  void*    denominatorCoefficients,
  void*    inputBuffer,
  void*    outputBuffer,
  g_real_type inputValue,
  g_real_type *outputValue) {
  
  int i = 0;

  if (reset) {
    int cellIndex;
    for (cellIndex = 0; cellIndex < delay; ++cellIndex) {
	  ((g_real_type*)inputBuffer)[cellIndex] = 0;
	  ((g_real_type*)outputBuffer)[cellIndex] = 0;
    }
    return 0;
  }

  BufferPushRight(delay, (g_real_type*)inputBuffer, inputValue);
  BufferPushRight(delay, (g_real_type*)outputBuffer, 0);

  for (i = 0; i < delay; ++i) {
	((g_real_type*)outputBuffer)[0] += ((g_real_type*)inputBuffer)[i] * ((g_real_type*)nominatorCoefficients)[i];
  }
  for (i = 1; i < delay; ++i) {
	((g_real_type*)outputBuffer)[0] -= ((g_real_type*)outputBuffer)[i] * ((g_real_type*)denominatorCoefficients)[i];
  }

  *outputValue = ((g_real_type*)outputBuffer)[0];

  return 0;
}

g_real_type SosFilterStep(g_real_type* KSos, g_real_type* Buf, g_real_type X) {

  g_real_type Y;

  g_real_type v0 = Buf[0];
  g_real_type v1 = Buf[1];
  g_real_type v2 = Buf[2];

  g_real_type a0 = KSos[3];
  g_real_type a1 = KSos[4] / a0;
  g_real_type a2 = KSos[5] / a0;
  g_real_type b0 = KSos[0] / a0;
  g_real_type b1 = KSos[1] / a0;
  g_real_type b2 = KSos[2] / a0;

  v0 = X - a1 * v1 - a2 * v2;
  Y = b0 * v0 + b1 * v1 + b2 * v2;
  v2 = v1;
  v1 = v0;

  Buf[0] = v0;
  Buf[1] = v1;
  Buf[2] = v2;

  return Y;
}

g_real_type SosFilt(int SOSCount, g_real_type* Sos, g_real_type* Buf, g_real_type X) {
  g_real_type Xj;
  int j;

  for (j = 0; j < SOSCount; j++) {
	Xj = SosFilterStep(&Sos[j*6], &Buf[j*3], X);
	X = Xj;
  }

  return Xj;
}

int SOSFilterProc(
                  char reset,
                  int SOSCount,
				  void* Sos,
				  void* Buf,
				  g_real_type inputValue,
				  g_real_type *outputValue) {

  int i;

  if (reset) {
	for (i = 0; i < SOSCount; ++i) {
	  ((g_real_type*)Buf)[i*3]   = 0;
	  ((g_real_type*)Buf)[i*3+1] = 0;
	  ((g_real_type*)Buf)[i*3+2] = 0;
	}
    return 0;
  }

  *outputValue = SosFilt(SOSCount, (g_real_type*)Sos, (g_real_type*)Buf, inputValue);

  return 0;
}

#endif


