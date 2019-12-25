#include <math.h>

#define NVCONST 1.7155277699141

double spc_rand_normalvariate(double mu, double sigma) {
  double myr1, myr2, t1, t2;

  do {
    myr1 = spc_rand_real();
    myr2 = spc_rand_real();
    t1 = NVCONST * (myr1 - 0.5) / myr2;
    t2 = t1 * t1 / 4.0;
  } while (t2 > -log(myr2));
  return mu + t1 * sigma;
}

double spc_rand_lognormalvariate(double mu, double sigma) {
  return exp(spc_rand_normalvariate(mu, sigma));
}

double spc_rand_paretovariate(double alpha) {
  return 1.0 / pow(spc_rand_real(), 1.0 / alpha);
}

double spc_rand_weibullvariate(double alpha, double beta) {
  return alpha * pow(-log(spc_rand_real()), 1.0 / beta);
}

double spc_rand_expovariate(double lambda) {
  double myr = spc_rand_real();

  while (myr <= 1e-7)
    myr = spc_rand_real();
  return -log(myr) / lambda;
}

double spc_rand_betavariate(double alpha, double beta) {
  double myr1, myr2;

  myr1 = spc_rand_expovariate(alpha);
  myr2 = spc_rand_expovariate(1.0 / beta);
  return myr2 / (myr1 + myr2);
}

#define SPC_PI 3.1415926535

double spc_rand_cunifvariate(double mean, double arc) {
  return (mean + arc * (spc_rand_real() - 0.5)) / SPC_PI;
}
