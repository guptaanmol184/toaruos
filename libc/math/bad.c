/* STUB MATH LIBRARY */
#include <stdio.h>
#include <stdlib.h>
#include <math.h>

extern char * _argv_0;
#define BAD do { if (getenv("LIBM_DEBUG")) { fprintf(stderr, "%s called bad math function %s\n", _argv_0, __func__); } } while (0)

double acos(double x) {
	BAD;
	return 0.0;
}

double asin(double x) {
	BAD;
	return 0.0;
}

double ldexp(double a, int exp) {
	double out = a;
	while (exp) {
		out *= 2.0;
		exp--;
	}
	return out;
}

double log(double x) {
	BAD;
	return 0.0;
}

double log10(double x) {
	BAD;
	return 0.0;
}

double log2(double x) {
	BAD;
	return 0.0;
}

double log1p(double x) {
	BAD;
	return log(x+1.0);
}

double expm1(double x) {
	BAD;
	return exp(x) - 1.0;
}
