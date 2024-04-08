#include <iostream>
#include <bit>
#include <cstdlib>
#include <cstring>
#include <cstdarg>
#include <cctype>

#include "tlfloat.hpp"
#include "tlfloat.h"

using namespace tlfloat;

static const int test_widths[] = { -1, 0, 1, 2, 3, 4, 6, 8, 12, 16, 20, 32 };
static const int test_precs[] = { 0, 1, 2, 3, 4, 8, 12, 14 };

bool xisdigit(int c) { return '0' <= c && c <= '9'; }
bool xisxdigit(int c) { return ('0' <= c && c <= '9') || ('a' <= c && c <= 'f') || ('A' <= c && c <= 'F'); }
int xtolower(int c) { if ('A' <= c && c <= 'Z') return c - 'A' + 'a'; else return c; }

int xstrcmp(const char *p, const char *q, int prec) {
  for(;;) {
    if (*p == '\0' && *q == '\0') return 0;
    if (xtolower(*p) == 'e' || xtolower(*p) == 'p') prec = 10000;
    if (xisxdigit(*p) && xisxdigit(*q)) {
      if (prec > 0 && *p != *q) return *p > *q ? 1 : -1;
      prec--;
    } else if (*p != *q) {
      return *p > *q ? 1 : -1;
    }
    p++; q++;
  }
}

template<typename T>
void testem(double val, int cmpprec=10) {
  const char *types[] = { "e", "f", "g", "a" };
  int nbits = sizeof(T) * 8;
  for(int i=0;i<4;i++) {
    for(int alt=0;alt<2;alt++) {
      for(int zero=0;zero<2;zero++) {
	for(int left=0;left<2;left++) {
	  for(int blank=0;blank<2;blank++) {
	    for(int sign=0;sign<2;sign++) {
	      char fmt[120], corr0[120], corr1[120], test[120];
	      int lc0, lc1, lt;

	      snprintf(fmt, 100, "%%%s%s%s%s%s%s",
		       alt ? "#" : "", 
		       zero ? "0" : "", 
		       left ? "-" : "", 
		       blank ? " " : "", 
		       sign ? "+" : "",
		       types[i]);
	      lc0 = snprintf(corr0, 100, fmt, val);
	      lc1 = snprintf(corr1, 100, fmt, strtod(corr0, nullptr));

	      snprintf(fmt, 100, "%%%d_%s%s%s%s%s%s",
		       nbits,
		       alt ? "#" : "", 
		       zero ? "0" : "", 
		       left ? "-" : "", 
		       blank ? " " : "", 
		       sign ? "+" : "",
		       types[i]);
	      lt = tlfloat_snprintf(test, 100, fmt, T(val));

	      if( (lt != lc0 && lt != lc1) ||
		  (xstrcmp(test, corr0, cmpprec) != 0 && xstrcmp(test, corr1, cmpprec) != 0) ||
		  (strtod(corr0, NULL) != tlfloat_strtod(corr0, NULL) && strstr(corr0, "nan") == NULL) ) {
		printf("%s : c0=[%s] c1=[%s] t=[%s] lc0=%d lc1=%d lt=%d %g %g <%016llx>\n", fmt, corr0, corr1, test, lc0, lc1, lt, 
		       strtod(corr0, NULL), (double)tlfloat_strtod(corr0, NULL), std::bit_cast<unsigned long long>(val));
		exit(-1);
	      }

	      for(unsigned w=0;w<sizeof(test_widths)/sizeof(int);w++) {
		int width = test_widths[w];
		snprintf(fmt, 100, "%%%s%s%s%s%s%d.%s",
			 alt ? "#" : "", 
			 zero ? "0" : "", 
			 left ? "-" : "", 
			 blank ? " " : "", 
			 sign ? "+" : "",
			 width, types[i]);
		lc0 = snprintf(corr0, 100, fmt, val);
		lc1 = snprintf(corr1, 100, fmt, strtod(corr0, nullptr));

		snprintf(fmt, 100, "%%%d_%s%s%s%s%s%d.%s",
			 nbits,
			 alt ? "#" : "", 
			 zero ? "0" : "", 
			 left ? "-" : "", 
			 blank ? " " : "", 
			 sign ? "+" : "",
			 width, types[i]);
		lt = tlfloat_snprintf(test, 100, fmt, T(val));

		if( (lt != lc0 && lt != lc1) ||
		    (xstrcmp(test, corr0, cmpprec) != 0 && xstrcmp(test, corr1, cmpprec) != 0) ||
		    (strtod(corr0, NULL) != tlfloat_strtod(corr0, NULL) && strstr(corr0, "nan") == NULL) ) {
		  printf("%s : c0=[%s] c1=[%s] t=[%s] lc0=%d lc1=%d lt=%d %g %g <%016llx>\n", fmt, corr0, corr1, test, lc0, lc1, lt, 
			 strtod(corr0, NULL), (double)tlfloat_strtod(corr0, NULL), std::bit_cast<unsigned long long>(val));
		  exit(-1);
		}
	      }

	      for(unsigned p=0;p<sizeof(test_precs)/sizeof(int);p++) {
		int prec = test_precs[p];
		for(unsigned w=0;w<sizeof(test_widths)/sizeof(int);w++) {
		  int width = test_widths[w];
		  snprintf(fmt, 100, "%%%s%s%s%s%s%d.%d%s",
			   alt ? "#" : "", 
			   zero ? "0" : "", 
			   left ? "-" : "", 
			   blank ? " " : "", 
			   sign ? "+" : "",
			   width, prec, types[i]);
		  lc0 = snprintf(corr0, 100, fmt, val);
		  lc1 = snprintf(corr1, 100, fmt, strtod(corr0, nullptr));

		  snprintf(fmt, 100, "%%%d_%s%s%s%s%s%d.%d%s",
			   nbits,
			   alt ? "#" : "", 
			   zero ? "0" : "", 
			   left ? "-" : "", 
			   blank ? " " : "", 
			   sign ? "+" : "",
			   width, prec, types[i]);
		  lt = tlfloat_snprintf(test, 100, fmt, T(val));

		  if( (lt != lc0 && lt != lc1) ||
		      (xstrcmp(test, corr0, cmpprec) != 0 && xstrcmp(test, corr1, cmpprec) != 0) ||
		      (strtod(corr0, NULL) != tlfloat_strtod(corr0, NULL) && strstr(corr0, "nan") == NULL) ) {
		    printf("%s : c0=[%s] c1=[%s] t=[%s] lc0=%d lc1=%d lt=%d %g %g <%016llx>\n", fmt, corr0, corr1, test, lc0, lc1, lt, 
			   strtod(corr0, NULL), (double)tlfloat_strtod(corr0, NULL), std::bit_cast<unsigned long long>(val));
		    exit(-1);
		  }
		}

		snprintf(fmt, 100, "%%%s%s%s%s%s.%d%s",
			 alt ? "#" : "", 
			 zero ? "0" : "", 
			 left ? "-" : "", 
			 blank ? " " : "", 
			 sign ? "+" : "",
			 prec, types[i]);
		lc0 = snprintf(corr0, 100, fmt, val);
		lc1 = snprintf(corr1, 100, fmt, strtod(corr0, nullptr));

		snprintf(fmt, 100, "%%%d_%s%s%s%s%s.%d%s",
			 nbits,
			 alt ? "#" : "", 
			 zero ? "0" : "", 
			 left ? "-" : "", 
			 blank ? " " : "", 
			 sign ? "+" : "",
			 prec, types[i]);
		lt = tlfloat_snprintf(test, 100, fmt, T(val));

		if( (lt != lc0 && lt != lc1) ||
		    (xstrcmp(test, corr0, cmpprec) != 0 && xstrcmp(test, corr1, cmpprec) != 0) ||
		    (strtod(corr0, NULL) != tlfloat_strtod(corr0, NULL) && strstr(corr0, "nan") == NULL) ) {
		  printf("%s : c0=[%s] c1=[%s] t=[%s] lc0=%d lc1=%d lt=%d %g %g <%016llx>\n", fmt, corr0, corr1, test, lc0, lc1, lt, 
			 strtod(corr0, NULL), (double)tlfloat_strtod(corr0, NULL), std::bit_cast<unsigned long long>(val));
		  exit(-1);
		}
	      }
	    }
	  }
	}
      }
    }
  }
}

void testem32(int32_t val) {
  const char *types[] = { "d", "u", "x", "o" };
  for(int i=0;i<4;i++) {
    for(int alt=0;alt<2;alt++) {
      for(int zero=0;zero<2;zero++) {
	for(int left=0;left<2;left++) {
	  for(int blank=0;blank<2;blank++) {
	    for(int sign=0;sign<2;sign++) {
	      char fmt[120], corr[120], test[120];

	      snprintf(fmt, 100, "%%%s%s%s%s%s%s",
		       alt ? "#" : "", 
		       zero ? "0" : "", 
		       left ? "-" : "", 
		       blank ? " " : "", 
		       sign ? "+" : "",
		       types[i]);
	      snprintf(corr, 100, fmt, val);

	      snprintf(fmt, 100, "%%32_%s%s%s%s%s%s",
		       alt ? "#" : "", 
		       zero ? "0" : "", 
		       left ? "-" : "", 
		       blank ? " " : "", 
		       sign ? "+" : "",
		       types[i]);
	      tlfloat_snprintf(test, 100, fmt, val);

	      if(strcmp(test,corr) != 0) {
		printf("%s : c=[%s] t=[%s]\n", fmt, corr, test);
		exit(-1);
	      }

	      for(unsigned w=0;w<sizeof(test_widths)/sizeof(int);w++) {
		int width = test_widths[w];
		snprintf(fmt, 100, "%%%s%s%s%s%s%d.%s",
			 alt ? "#" : "", 
			 zero ? "0" : "", 
			 left ? "-" : "", 
			 blank ? " " : "", 
			 sign ? "+" : "",
			 width, types[i]);
		snprintf(corr, 100, fmt, val);

		snprintf(fmt, 100, "%%32_%s%s%s%s%s%d.%s",
			 alt ? "#" : "", 
			 zero ? "0" : "", 
			 left ? "-" : "", 
			 blank ? " " : "", 
			 sign ? "+" : "",
			 width, types[i]);
		tlfloat_snprintf(test, 100, fmt, val);

		if(strcmp(test,corr) != 0) {
		  printf("%s : c=[%s] t=[%s]\n", fmt, corr, test);
		  exit(-1);
		}
	      }

	      for(int prec=4;prec<=12;prec += 1) {
		for(unsigned w=0;w<sizeof(test_widths)/sizeof(int);w++) {
		  int width = test_widths[w];
		  snprintf(fmt, 100, "%%%s%s%s%s%s%d.%d%s",
			   alt ? "#" : "", 
			   zero ? "0" : "", 
			   left ? "-" : "", 
			   blank ? " " : "", 
			   sign ? "+" : "",
			   width, prec, types[i]);
		  snprintf(corr, 100, fmt, val);

		  snprintf(fmt, 100, "%%32_%s%s%s%s%s%d.%d%s",
			    alt ? "#" : "", 
			    zero ? "0" : "", 
			    left ? "-" : "", 
			    blank ? " " : "", 
			    sign ? "+" : "",
			    width, prec, types[i]);
		  tlfloat_snprintf(test, 100, fmt, val);

		  if(strcmp(test,corr) != 0) {
		    printf("%s : c=[%s] t=[%s]\n", fmt, corr, test);
		    exit(-1);
		  }
		}

		snprintf(fmt, 100, "%%%s%s%s%s%s.%d%s",
			 alt ? "#" : "", 
			 zero ? "0" : "", 
			 left ? "-" : "", 
			 blank ? " " : "", 
			 sign ? "+" : "",
			 prec, types[i]);
		snprintf(corr, 100, fmt, val);

		snprintf(fmt, 100, "%%32_%s%s%s%s%s.%d%s",
			 alt ? "#" : "", 
			 zero ? "0" : "", 
			 left ? "-" : "", 
			 blank ? " " : "", 
			 sign ? "+" : "",
			 prec, types[i]);
		tlfloat_snprintf(test, 100, fmt, val);

		if(strcmp(test,corr) != 0) {
		  printf("%s : c=[%s] t=[%s]\n", fmt, corr, test);
		  exit(-1);
		}
	      }
	    }
	  }
	}
      }
    }
  }
}

void testem64(int64_t val) {
  const char *types[] = { "lld", "llu", "llx", "llo" };
  for(int i=0;i<4;i++) {
    for(int alt=0;alt<2;alt++) {
      for(int zero=0;zero<2;zero++) {
	for(int left=0;left<2;left++) {
	  for(int blank=0;blank<2;blank++) {
	    for(int sign=0;sign<2;sign++) {
	      char fmt[120], corr[120], test[120];

	      snprintf(fmt, 90, "%%%s%s%s%s%s%s",
		       alt ? "#" : "", 
		       zero ? "0" : "", 
		       left ? "-" : "", 
		       blank ? " " : "", 
		       sign ? "+" : "",
		       types[i]);
	      snprintf(corr, 98, fmt, val);

	      snprintf(fmt, 90, "%%64_%s%s%s%s%s%s",
		       alt ? "#" : "", 
		       zero ? "0" : "", 
		       left ? "-" : "", 
		       blank ? " " : "", 
		       sign ? "+" : "",
		       types[i]);
	      tlfloat_snprintf(test, 98, fmt, val);

	      if(strcmp(test,corr) != 0) {
		printf("%s : c=[%s] t=[%s]\n", fmt, corr, test);
	      }

	      for(unsigned w=0;w<sizeof(test_widths)/sizeof(int);w++) {
		int width = test_widths[w];
		snprintf(fmt, 90, "%%%s%s%s%s%s%d.%s",
			 alt ? "#" : "", 
			 zero ? "0" : "", 
			 left ? "-" : "", 
			 blank ? " " : "", 
			 sign ? "+" : "",
			 width, types[i]);
		snprintf(corr, 98, fmt, val);

		snprintf(fmt, 90, "%%64_%s%s%s%s%s%d.%s",
			 alt ? "#" : "", 
			 zero ? "0" : "", 
			 left ? "-" : "", 
			 blank ? " " : "", 
			 sign ? "+" : "",
			 width, types[i]);
		tlfloat_snprintf(test, 98, fmt, val);

		if(strcmp(test,corr) != 0) {
		  printf("%s : c=[%s] t=[%s]\n", fmt, corr, test);
		  exit(-1);
		}
	      }

	      for(int prec=0;prec<=12;prec += 2) {
		for(unsigned w=0;w<sizeof(test_widths)/sizeof(int);w++) {
		  int width = test_widths[w];
		  snprintf(fmt, 90, "%%%s%s%s%s%s%d.%d%s",
			   alt ? "#" : "", 
			   zero ? "0" : "", 
			   left ? "-" : "", 
			   blank ? " " : "", 
			   sign ? "+" : "",
			   width, prec, types[i]);
		  snprintf(corr, 98, fmt, val);

		  snprintf(fmt, 90, "%%64_%s%s%s%s%s%d.%d%s",
			    alt ? "#" : "", 
			    zero ? "0" : "", 
			    left ? "-" : "", 
			    blank ? " " : "", 
			    sign ? "+" : "",
			    width, prec, types[i]);
		  tlfloat_snprintf(test, 98, fmt, val);

		  if(strcmp(test,corr) != 0) {
		    printf("%s : c=[%s] t=[%s]\n", fmt, corr, test);
		    exit(-1);
		  }
		}

		snprintf(fmt, 90, "%%%s%s%s%s%s.%d%s",
			 alt ? "#" : "", 
			 zero ? "0" : "", 
			 left ? "-" : "", 
			 blank ? " " : "", 
			 sign ? "+" : "",
			 prec, types[i]);
		snprintf(corr, 98, fmt, val);

		snprintf(fmt, 90, "%%64_%s%s%s%s%s.%d%s",
			 alt ? "#" : "", 
			 zero ? "0" : "", 
			 left ? "-" : "", 
			 blank ? " " : "", 
			 sign ? "+" : "",
			 prec, types[i]);
		tlfloat_snprintf(test, 98, fmt, val);

		if(strcmp(test,corr) != 0) {
		  printf("%s : c=[%s] t=[%s]\n", fmt, corr, test);
		  exit(-1);
		}
	      }
	    }
	  }
	}
      }
    }
  }
}

using namespace std;

int main(int argc, char **argv) {
  testem32(0);
  for(int i=0;i<31;i++) {
    testem32(+1 << i);
    testem32(-1 << i);
  }

  cout << "int32_t OK" << endl;

  testem64(0);
  for(int i=0;i<63;i++) {
    testem64(+1LL << i);
    testem64(-1LL << i);
  }

  cout << "int64_t OK" << endl;

  testem<Float>(NAN, 5);
  testem<Float>(+0.0, 5);
  testem<Float>(-0.0, 5);
  testem<Float>(+INFINITY, 5);
  testem<Float>(-INFINITY, 5);
  testem<Float>(+1.0, 5);
  testem<Float>(-1.0, 5);

  cout << "Float OK" << endl;

  testem<Double>(NAN);
  testem<Double>(+0.0);
  testem<Double>(-0.0);
  testem<Double>(+INFINITY);
  testem<Double>(-INFINITY);
  testem<Double>(+1.0);
  testem<Double>(-1.0);
  testem<Double>(+0.1);
  testem<Double>(-0.1);
  testem<Double>(+1e+12);
  testem<Double>(-1e+12);
  testem<Double>(+1.234567890123456789e+12);
  testem<Double>(-1.234567890123456789e+12);
  testem<Double>(+1e-12);
  testem<Double>(-1e-12);
  testem<Double>(+1.234567890123456789e-12);
  testem<Double>(-1.234567890123456789e-12);
  testem<Double>(+1e+120);
  testem<Double>(-1e+120);
  testem<Double>(+1.234567890123456789e+120);
  testem<Double>(-1.234567890123456789e+120);
  testem<Double>(+1e-120);
  testem<Double>(-1e-120);
  testem<Double>(+1.234567890123456789e-120);
  testem<Double>(-1.234567890123456789e-120);

  cout << "Double OK" << endl;

  testem<Quad>(NAN);
  testem<Quad>(+0.0);
  testem<Quad>(-0.0);
  testem<Quad>(+INFINITY);
  testem<Quad>(-INFINITY);
  testem<Quad>(+1.0);
  testem<Quad>(-1.0);
  testem<Quad>(+0.1);
  testem<Quad>(-0.1);
  testem<Quad>(+1e+12);
  testem<Quad>(-1e+12);
  testem<Quad>(+1.234567890123456789e+12);
  testem<Quad>(-1.234567890123456789e+12);
  testem<Quad>(+1e-12);
  testem<Quad>(-1e-12);
  testem<Quad>(+1.234567890123456789e-12);
  testem<Quad>(-1.234567890123456789e-12);
  testem<Quad>(+1e+120);
  testem<Quad>(-1e+120);
  testem<Quad>(+1.234567890123456789e+120);
  testem<Quad>(-1.234567890123456789e+120);
  testem<Quad>(+1e-120);
  testem<Quad>(-1e-120);
  testem<Quad>(+1.234567890123456789e-120);
  testem<Quad>(-1.234567890123456789e-120);

  cout << "Quad OK" << endl;

  testem<Octuple>(NAN);
  testem<Octuple>(+0.0);
  testem<Octuple>(-0.0);
  testem<Octuple>(+INFINITY);
  testem<Octuple>(-INFINITY);
  testem<Octuple>(+1.0);
  testem<Octuple>(-1.0);
  testem<Octuple>(+0.1);
  testem<Octuple>(-0.1);
  testem<Octuple>(+1e+12);
  testem<Octuple>(-1e+12);
  testem<Octuple>(+1.234567890123456789e+12);
  testem<Octuple>(-1.234567890123456789e+12);
  testem<Octuple>(+1e-12);
  testem<Octuple>(-1e-12);
  testem<Octuple>(+1.234567890123456789e-12);
  testem<Octuple>(-1.234567890123456789e-12);
  testem<Octuple>(+1e+120);
  testem<Octuple>(-1e+120);
  testem<Octuple>(+1.234567890123456789e+120);
  testem<Octuple>(-1.234567890123456789e+120);
  testem<Octuple>(+1e-120);
  testem<Octuple>(-1e-120);
  testem<Octuple>(+1.234567890123456789e-120);
  testem<Octuple>(-1.234567890123456789e-120);

  cout << "OK" << endl;

  exit(0);
}
