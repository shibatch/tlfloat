#include <cstring>
#include <cctype>
#include <cstdarg>

#ifdef SUPPRESS_WARNINGS
#include "suppress.hpp"
#endif

#define NO_LIBSTDCXX
#include "tlfloat/tlmath.hpp"

#include "tlfloat/tlfloat.h"

using namespace tlfloat;

namespace {
  static int xvprintf(size_t (*consumer)(const char *ptr, size_t size, void *arg), void *arg, const char *fmt, va_list ap) {
    const int xbufsize = 5000;
    char *xbuf = (char *)malloc(xbufsize+10);
    int outlen = 0;
    bool errorflag = false;

    while(*fmt != '\0' && !errorflag) {
      // Copy the format string until a '%' is read

      if (*fmt != '%') {
	do {
	  outlen += (*consumer)(fmt++, 1, arg);
	} while(*fmt != '%' && *fmt != '\0');

	if (*fmt == '\0') break;
      }

      const char *subfmtstart = fmt;

      if ((*++fmt) == '\0') {
	errorflag = true;
	outlen += (*consumer)("%", 1, arg);
	break;
      }

      // Read flags

      bool flag_left = false, flag_sign = false, flag_blank = false;
      bool flag_alt = false, flag_zero = false, flag_upper = false, done = false;
      do {
	switch(*fmt) {
	case '-': flag_left  = true; break;
	case '+': flag_sign  = true; break;
	case ' ': flag_blank = true; break;
	case '#': flag_alt   = true; break;
	case '0': flag_zero  = true; break;
	default:  done       = true; break;
	}
      } while(!done && (*++fmt) != 0);

      // Read width

      int width = 0;
      bool readWidthFromArg = false;

      if (*fmt == '*') {
	readWidthFromArg = true;
	fmt++;
      } else {
	while(*fmt >= '0' && *fmt <= '9') {
	  width = width*10 + *fmt - '0';
	  fmt++;
	}
      }

      // Read precision

      int precision = -1;
      bool readPrecFromArg = false;

      if (*fmt == '.') {
	precision = 0;
	fmt++;
	if (*fmt == '*') {
	  readPrecFromArg = true;
	  fmt++;
	} else {
	  while(*fmt >= '0' && *fmt <= '9') {
	    precision = precision*10 + *fmt - '0';
	    fmt++;
	  }
	}
      }

      // Read size prefix

      bool flag_quad = false, subfmt_processed = false;
      int size_prefix = 0, nbits = 0;

      if (*fmt == '_') {
	char *eptr = NULL;
	nbits = strtol(fmt+1, &eptr, 10);
	if (eptr != fmt+1) {
	  fmt = eptr;
	} else {
	  nbits = 0;
	  errorflag = true;
	}
      } else if (*fmt == 'Q') {
	flag_quad = true;
	fmt++;
      } else {
	int pl = 0;
	if (*fmt == 'h' || *fmt == 'l' || *fmt == 'j' || *fmt == 'z' || *fmt == 't' || *fmt == 'L') {
	  size_prefix = *fmt;
	  pl = 1;
	}
	if ((*fmt == 'h' && *(fmt+1) == 'h') || (*fmt == 'l' && *(fmt+1) == 'l')) {
	  size_prefix = *fmt + 256 * *(fmt+1);
	  pl = 2;
	}
	fmt += pl;
      }

      // Call type-specific function

      va_list ap2;
      va_copy(ap2, ap);

      if (readWidthFromArg) width = va_arg(ap, int);
      if (width < 0) {
	width = -width;
	flag_left = true;
      }

      if (readPrecFromArg) precision = va_arg(ap, int);

      switch(*fmt) {
      case 'E': case 'F': case 'G': case 'A':
	flag_upper = true;
	// fall through
      case 'e': case 'f': case 'g': case 'a':
	{
	  if (nbits == 0) {
	    if (flag_quad) {
	      Quad value = std::bit_cast<Quad>(va_arg(ap, tlfloat_quad));
	      typedef decltype(decltype(value.getUnpacked())::xUnpackedFloat()) xUnpacked_t;
	      int ret = snprint(xbuf, xbufsize, value.getUnpacked().cast((xUnpacked_t *)0), *fmt, width, precision, 
				flag_sign, flag_blank, flag_alt, flag_left, flag_zero, flag_upper);
	      if (ret < 0) { errorflag = 1; break; }
	      outlen += (*consumer)(xbuf, strlen(xbuf), arg);
	      subfmt_processed = true;
	    } else if (size_prefix == 0) { // double and long double
	      va_arg(ap, double);
	    } else if (size_prefix == 'L') {
	      va_arg(ap, long double);
	    } else errorflag = 1;
	  } else {
	    switch(nbits) {
	    case 16: {
	      typedef struct { uint16_t e; } arg_t;
	      Half value = std::bit_cast<Half>(va_arg(ap, arg_t));
	      typedef decltype(decltype(value.getUnpacked())::xUnpackedFloat()) xUnpacked_t;
	      int ret = snprint(xbuf, xbufsize, value.getUnpacked().cast((xUnpacked_t *)0), *fmt, width, precision, 
				flag_sign, flag_blank, flag_alt, flag_left, flag_zero, flag_upper);
	      if (ret < 0) { errorflag = 1; break; }
	      outlen += (*consumer)(xbuf, strlen(xbuf), arg);
	      subfmt_processed = true;
	      break;
	    }
	    case 32: {
	      typedef struct { uint32_t e; } arg_t;
	      Float value = std::bit_cast<Float>(va_arg(ap, arg_t));
	      typedef decltype(decltype(value.getUnpacked())::xUnpackedFloat()) xUnpacked_t;
	      int ret = snprint(xbuf, xbufsize, value.getUnpacked().cast((xUnpacked_t *)0), *fmt, width, precision, 
				flag_sign, flag_blank, flag_alt, flag_left, flag_zero, flag_upper);
	      if (ret < 0) { errorflag = 1; break; }
	      outlen += (*consumer)(xbuf, strlen(xbuf), arg);
	      subfmt_processed = true;
	      break;
	    }
	    case 64: {
	      typedef struct { uint64_t e[1]; } arg_t;
	      Double value = std::bit_cast<Double>(va_arg(ap, arg_t));
	      typedef decltype(decltype(value.getUnpacked())::xUnpackedFloat()) xUnpacked_t;
	      int ret = snprint(xbuf, xbufsize, value.getUnpacked().cast((xUnpacked_t *)0), *fmt, width, precision, 
				flag_sign, flag_blank, flag_alt, flag_left, flag_zero, flag_upper);
	      if (ret < 0) { errorflag = 1; break; }
	      outlen += (*consumer)(xbuf, strlen(xbuf), arg);
	      subfmt_processed = true;
	      break;
	    }
	    case 128: {
	      typedef struct { uint64_t e[2]; } arg_t;
	      Quad value = std::bit_cast<Quad>(va_arg(ap, arg_t));
	      typedef decltype(decltype(value.getUnpacked())::xUnpackedFloat()) xUnpacked_t;
	      int ret = snprint(xbuf, xbufsize, value.getUnpacked().cast((xUnpacked_t *)0), *fmt, width, precision, 
				flag_sign, flag_blank, flag_alt, flag_left, flag_zero, flag_upper);
	      if (ret < 0) { errorflag = 1; break; }
	      outlen += (*consumer)(xbuf, strlen(xbuf), arg);
	      subfmt_processed = true;
	      break;
	    }
	    case 256: {
	      typedef struct { uint64_t e[4]; } arg_t;
	      Octuple value = std::bit_cast<Octuple>(va_arg(ap, arg_t));
	      typedef decltype(decltype(value.getUnpacked())::xUnpackedFloat()) xUnpacked_t;
	      int ret = snprint(xbuf, xbufsize, value.getUnpacked().cast((xUnpacked_t *)0), *fmt, width, precision, 
				flag_sign, flag_blank, flag_alt, flag_left, flag_zero, flag_upper);
	      if (ret < 0) { errorflag = 1; break; }
	      outlen += (*consumer)(xbuf, strlen(xbuf), arg);
	      subfmt_processed = true;
	      break;
	    }
	    default: errorflag = 1; break;
	    }
	  }
	}
	break;

      case 'd': case 'i': case 'u': case 'o': case 'x': case 'X':
	{
	  if (nbits == 0) {
	    switch(size_prefix) {
	    case 0: case 'h': case 'h' + 256*'h': va_arg(ap, int); break;
	    case 'l': va_arg(ap, long int); break;
	    case 'j': va_arg(ap, intmax_t); break;
	    case 'z': va_arg(ap, size_t); break;
	    case 't': va_arg(ap, std::ptrdiff_t); break;
	    case 'l' + 256*'l': va_arg(ap, long long int); break;
	    default: errorflag = 1; break;
	    }
	  } else {
	    bool flag_unsigned = (*fmt == 'u') || (*fmt == 'o') || (*fmt == 'x') || (*fmt == 'X');
	    bool flag_ptr = false;
	    if (*fmt == 'X') flag_upper = true;

	    char const *prefix = "";
	    int base = 10;
	    if (*fmt == 'o') { base = 8; prefix = "0"; }
	    if (*fmt == 'x' || *fmt == 'X') { base = 16; prefix = flag_upper ? "0X" : "0x"; }

	    switch(nbits) {
	    case 8: case 16: case 32: case 64: {
	      int64_t value = nbits == 64 ? va_arg(ap, int64_t) : va_arg(ap, int);
	      int ret = BigInt<7>::snprint(xbuf, xbufsize, BigInt<7>(value), tolower(*fmt), width, precision, base, nbits,
					   flag_sign, flag_blank, flag_alt, flag_left, flag_zero, flag_upper, flag_unsigned, flag_ptr, prefix);
	      if (ret < 0) { errorflag = 1; break; }
	      outlen += (*consumer)(xbuf, strlen(xbuf), arg);
	      subfmt_processed = true;
	      break;
	    }
	    case 128: {
	      BigInt<7> value = std::bit_cast<BigInt<7>>(va_arg(ap, tlfloat_bigint7));
	      int ret = BigInt<7>::snprint(xbuf, xbufsize, value, tolower(*fmt), width, precision, base, nbits,
					   flag_sign, flag_blank, flag_alt, flag_left, flag_zero, flag_upper, flag_unsigned, flag_ptr, prefix);
	      if (ret < 0) { errorflag = 1; break; }
	      outlen += (*consumer)(xbuf, strlen(xbuf), arg);
	      subfmt_processed = true;
	      break;
	    }
	    case 256: {
	      BigInt<8> value = std::bit_cast<BigInt<8>>(va_arg(ap, tlfloat_bigint8));
	      int ret = BigInt<8>::snprint(xbuf, xbufsize, value, tolower(*fmt), width, precision, base, nbits,
					   flag_sign, flag_blank, flag_alt, flag_left, flag_zero, flag_upper, flag_unsigned, flag_ptr, prefix);
	      if (ret < 0) { errorflag = 1; break; }
	      outlen += (*consumer)(xbuf, strlen(xbuf), arg);
	      subfmt_processed = true;
	      break;
	    }
	    case 512: {
	      BigInt<9> value = std::bit_cast<BigInt<9>>(va_arg(ap, tlfloat_bigint9));
	      int ret = BigInt<9>::snprint(xbuf, xbufsize, value, tolower(*fmt), width, precision, base, nbits,
					   flag_sign, flag_blank, flag_alt, flag_left, flag_zero, flag_upper, flag_unsigned, flag_ptr, prefix);
	      if (ret < 0) { errorflag = 1; break; }
	      outlen += (*consumer)(xbuf, strlen(xbuf), arg);
	      subfmt_processed = true;
	      break;
	    }
	    case 1024: {
	      BigInt<10> value = std::bit_cast<BigInt<10>>(va_arg(ap, tlfloat_bigint10));
	      int ret = BigInt<10>::snprint(xbuf, xbufsize, value, tolower(*fmt), width, precision, base, nbits,
					    flag_sign, flag_blank, flag_alt, flag_left, flag_zero, flag_upper, flag_unsigned, flag_ptr, prefix);
	      if (ret < 0) { errorflag = 1; break; }
	      outlen += (*consumer)(xbuf, strlen(xbuf), arg);
	      subfmt_processed = true;
	      break;
	    }
	    default: errorflag = 1; break;
	    }
	  }
	}
	break;

      case 'c':
	if (size_prefix == 0) {
	  va_arg(ap, int);
	} else
#if 0
	  if (size_prefix == 'l') {
	    va_arg(ap, wint_t); // wint_t is not defined
	  } else
#endif
	    errorflag = 1;
	break;

      case 's':
	if (size_prefix == 0 || size_prefix == 'l') {
	  va_arg(ap, void *);
	} else errorflag = 1;
	break;

      case 'p': case 'n':
	{
	  if ((*fmt == 'p' && size_prefix != 0) || size_prefix == 'L') { errorflag = 1; break; }
	  va_arg(ap, void *);
	}
	break;

      default:
	errorflag = 1;
      }

      if (!subfmt_processed) {
	char *subfmt = (char *)malloc(fmt - subfmtstart + 2);
	memcpy(subfmt, subfmtstart, fmt - subfmtstart + 1);
	subfmt[fmt - subfmtstart + 1] = 0;
	int ret = ::vsnprintf(xbuf, xbufsize, subfmt, ap2);
	free(subfmt);
	if (ret < 0) { errorflag = 1; break; }
	outlen += (*consumer)(xbuf, strlen(xbuf), arg);
      }

      fmt++;
    }

    free(xbuf);

    return errorflag ? -1 : outlen;
  }

  //

  struct stream_consumer_t { FILE *fp; };

  static size_t stream_consumer(const char *ptr, size_t size, void *varg) {
    stream_consumer_t *arg = (stream_consumer_t *)varg;
    return fwrite(ptr, size, 1, arg->fp);
  }

  struct buf_consumer_t {
    char *buf;
    size_t pos, size;
  };

  static size_t buf_consumer(const char *ptr, size_t size, void *varg) {
    buf_consumer_t *arg = (buf_consumer_t *)varg;

    size_t p = 0;
    while(p < size) {
      if (arg->pos >= arg->size - 1) break;
      arg->buf[arg->pos++] = ptr[p++];
    }

    arg->buf[arg->pos] = '\0';

    return size;
  }
} // namespace

extern "C" {
  int tlfloat_vfprintf(FILE *fp, const char *fmt, va_list ap) {
    stream_consumer_t arg = { fp };
    return xvprintf(stream_consumer, &arg, fmt, ap);
  }

  int tlfloat_vprintf(const char *fmt, va_list ap) {
    return tlfloat_vfprintf(stdout, fmt, ap);
  }

  int tlfloat_fprintf(FILE *fp, const char *fmt, ...) {
    va_list ap;
    va_start(ap, fmt);
    int ret = tlfloat_vfprintf(fp, fmt, ap);
    va_end(ap);
    return ret;
  }

  int tlfloat_printf(const char *fmt, ...) {
    va_list ap;
    va_start(ap, fmt);
    int ret = tlfloat_vfprintf(stdout, fmt, ap);
    va_end(ap);
    return ret;
  }

  int tlfloat_vsnprintf(char *str, size_t size, const char *fmt, va_list ap) {
    buf_consumer_t arg = { str, 0, size };
    return xvprintf(buf_consumer, &arg, fmt, ap);
  }

  int tlfloat_snprintf(char *str, size_t size, const char *fmt, ...) {
    va_list ap;
    va_start(ap, fmt);
    int ret = tlfloat_vsnprintf(str, size, fmt, ap);
    va_end(ap);
    return ret;
  }

  float tlfloat_strtof(const char *nptr, const char **endptr) { return (float)Float(nptr, endptr); }
  double tlfloat_strtod(const char *nptr, const char **endptr) { return (double)Double(nptr, endptr); }
  tlfloat_quad tlfloat_strtoq(const char *nptr, const char **endptr) { return (tlfloat_quad)Quad(nptr, endptr); }
  tlfloat_octuple tlfloat_strtoo(const char *nptr, const char **endptr) { return (tlfloat_octuple)Octuple(nptr, endptr); }

  //

  float tlfloat_sinf(const float x) { return (float)sin(Float(x)); }
  double tlfloat_sin(const double x) { return (double)sin(Double(x)); }
  tlfloat_quad tlfloat_sinq(const tlfloat_quad x) { return (tlfloat_quad)sin(Quad(x)); }
  tlfloat_octuple tlfloat_sino(const tlfloat_octuple x) { return (tlfloat_octuple)sin(Octuple(x)); }

  float tlfloat_cosf(const float x) { return (float)cos(Float(x)); }
  double tlfloat_cos(const double x) { return (double)cos(Double(x)); }
  tlfloat_quad tlfloat_cosq(const tlfloat_quad x) { return (tlfloat_quad)cos(Quad(x)); }
  tlfloat_octuple tlfloat_coso(const tlfloat_octuple x) { return (tlfloat_octuple)cos(Octuple(x)); }

  float tlfloat_tanf(const float x) { return (float)tan(Float(x)); }
  double tlfloat_tan(const double x) { return (double)tan(Double(x)); }
  tlfloat_quad tlfloat_tanq(const tlfloat_quad x) { return (tlfloat_quad)tan(Quad(x)); }
  tlfloat_octuple tlfloat_tano(const tlfloat_octuple x) { return (tlfloat_octuple)tan(Octuple(x)); }

  float tlfloat_asinf(const float x) { return (float)asin(Float(x)); }
  double tlfloat_asin(const double x) { return (double)asin(Double(x)); }
  tlfloat_quad tlfloat_asinq(const tlfloat_quad x) { return (tlfloat_quad)asin(Quad(x)); }
  tlfloat_octuple tlfloat_asino(const tlfloat_octuple x) { return (tlfloat_octuple)asin(Octuple(x)); }

  float tlfloat_acosf(const float x) { return (float)acos(Float(x)); }
  double tlfloat_acos(const double x) { return (double)acos(Double(x)); }
  tlfloat_quad tlfloat_acosq(const tlfloat_quad x) { return (tlfloat_quad)acos(Quad(x)); }
  tlfloat_octuple tlfloat_acoso(const tlfloat_octuple x) { return (tlfloat_octuple)acos(Octuple(x)); }

  float tlfloat_atanf(const float x) { return (float)atan(Float(x)); }
  double tlfloat_atan(const double x) { return (double)atan(Double(x)); }
  tlfloat_quad tlfloat_atanq(const tlfloat_quad x) { return (tlfloat_quad)atan(Quad(x)); }
  tlfloat_octuple tlfloat_atano(const tlfloat_octuple x) { return (tlfloat_octuple)atan(Octuple(x)); }

  float tlfloat_atan2f(const float y, const float x) { return (float)atan2(Float(y), Float(x)); }
  double tlfloat_atan2(const double y, const double x) { return (double)atan2(Double(y), Double(x)); }
  tlfloat_quad tlfloat_atan2q(const tlfloat_quad y, const tlfloat_quad x) { return (tlfloat_quad)atan2(Quad(y), Quad(x)); }
  tlfloat_octuple tlfloat_atan2o(const tlfloat_octuple y, const tlfloat_octuple x) { return (tlfloat_octuple)atan2(Octuple(y), Octuple(x)); }

  float tlfloat_expf(const float a) { return (float)exp(Float(a)); }
  double tlfloat_exp(const double a) { return (double)exp(Double(a)); }
  tlfloat_quad tlfloat_expq(const tlfloat_quad a) { return (tlfloat_quad)exp(Quad(a)); }
  tlfloat_octuple tlfloat_expo(const tlfloat_octuple a) { return (tlfloat_octuple)exp(Octuple(a)); }

  float tlfloat_expm1f(const float a) { return (float)expm1(Float(a)); }
  double tlfloat_expm1(const double a) { return (double)expm1(Double(a)); }
  tlfloat_quad tlfloat_expm1q(const tlfloat_quad a) { return (tlfloat_quad)expm1(Quad(a)); }
  tlfloat_octuple tlfloat_expm1o(const tlfloat_octuple a) { return (tlfloat_octuple)expm1(Octuple(a)); }

  float tlfloat_exp2f(const float a) { return (float)exp2(Float(a)); }
  double tlfloat_exp2(const double a) { return (double)exp2(Double(a)); }
  tlfloat_quad tlfloat_exp2q(const tlfloat_quad a) { return (tlfloat_quad)exp2(Quad(a)); }
  tlfloat_octuple tlfloat_exp2o(const tlfloat_octuple a) { return (tlfloat_octuple)exp2(Octuple(a)); }

  float tlfloat_exp10f(const float a) { return (float)exp10(Float(a)); }
  double tlfloat_exp10(const double a) { return (double)exp10(Double(a)); }
  tlfloat_quad tlfloat_exp10q(const tlfloat_quad a) { return (tlfloat_quad)exp10(Quad(a)); }
  tlfloat_octuple tlfloat_exp10o(const tlfloat_octuple a) { return (tlfloat_octuple)exp10(Octuple(a)); }

  float tlfloat_logf(const float a) { return (float)log(Float(a)); }
  double tlfloat_log(const double a) { return (double)log(Double(a)); }
  tlfloat_quad tlfloat_logq(const tlfloat_quad a) { return (tlfloat_quad)log(Quad(a)); }
  tlfloat_octuple tlfloat_logo(const tlfloat_octuple a) { return (tlfloat_octuple)log(Octuple(a)); }

  float tlfloat_log1pf(const float a) { return (float)log1p(Float(a)); }
  double tlfloat_log1p(const double a) { return (double)log1p(Double(a)); }
  tlfloat_quad tlfloat_log1pq(const tlfloat_quad a) { return (tlfloat_quad)log1p(Quad(a)); }
  tlfloat_octuple tlfloat_log1po(const tlfloat_octuple a) { return (tlfloat_octuple)log1p(Octuple(a)); }

  float tlfloat_log2f(const float a) { return (float)log2(Float(a)); }
  double tlfloat_log2(const double a) { return (double)log2(Double(a)); }
  tlfloat_quad tlfloat_log2q(const tlfloat_quad a) { return (tlfloat_quad)log2(Quad(a)); }
  tlfloat_octuple tlfloat_log2o(const tlfloat_octuple a) { return (tlfloat_octuple)log2(Octuple(a)); }

  float tlfloat_log10f(const float a) { return (float)log10(Float(a)); }
  double tlfloat_log10(const double a) { return (double)log10(Double(a)); }
  tlfloat_quad tlfloat_log10q(const tlfloat_quad a) { return (tlfloat_quad)log10(Quad(a)); }
  tlfloat_octuple tlfloat_log10o(const tlfloat_octuple a) { return (tlfloat_octuple)log10(Octuple(a)); }

  float tlfloat_powf(const float x, const float y) { return (float)pow(Float(x), Float(y)); }
  double tlfloat_pow(const double x, const double y) { return (double)pow(Double(x), Double(y)); }
  tlfloat_quad tlfloat_powq(const tlfloat_quad x, const tlfloat_quad y) { return (tlfloat_quad)pow(Quad(x), Quad(y)); }
  tlfloat_octuple tlfloat_powo(const tlfloat_octuple x, const tlfloat_octuple y) { return (tlfloat_octuple)pow(Octuple(x), Octuple(y)); }

  float tlfloat_cbrtf(const float a) { return (float)cbrt(Float(a)); }
  double tlfloat_cbrt(const double a) { return (double)cbrt(Double(a)); }
  tlfloat_quad tlfloat_cbrtq(const tlfloat_quad a) { return (tlfloat_quad)cbrt(Quad(a)); }
  tlfloat_octuple tlfloat_cbrto(const tlfloat_octuple a) { return (tlfloat_octuple)cbrt(Octuple(a)); }

  float tlfloat_sinhf(const float x) { return (float)sinh(Float(x)); }
  double tlfloat_sinh(const double x) { return (double)sinh(Double(x)); }
  tlfloat_quad tlfloat_sinhq(const tlfloat_quad x) { return (tlfloat_quad)sinh(Quad(x)); }
  tlfloat_octuple tlfloat_sinho(const tlfloat_octuple x) { return (tlfloat_octuple)sinh(Octuple(x)); }

  float tlfloat_coshf(const float x) { return (float)cosh(Float(x)); }
  double tlfloat_cosh(const double x) { return (double)cosh(Double(x)); }
  tlfloat_quad tlfloat_coshq(const tlfloat_quad x) { return (tlfloat_quad)cosh(Quad(x)); }
  tlfloat_octuple tlfloat_cosho(const tlfloat_octuple x) { return (tlfloat_octuple)cosh(Octuple(x)); }

  float tlfloat_tanhf(const float x) { return (float)tanh(Float(x)); }
  double tlfloat_tanh(const double x) { return (double)tanh(Double(x)); }
  tlfloat_quad tlfloat_tanhq(const tlfloat_quad x) { return (tlfloat_quad)tanh(Quad(x)); }
  tlfloat_octuple tlfloat_tanho(const tlfloat_octuple x) { return (tlfloat_octuple)tanh(Octuple(x)); }

  float tlfloat_asinhf(const float x) { return (float)asinh(Float(x)); }
  double tlfloat_asinh(const double x) { return (double)asinh(Double(x)); }
  tlfloat_quad tlfloat_asinhq(const tlfloat_quad x) { return (tlfloat_quad)asinh(Quad(x)); }
  tlfloat_octuple tlfloat_asinho(const tlfloat_octuple x) { return (tlfloat_octuple)asinh(Octuple(x)); }

  float tlfloat_acoshf(const float x) { return (float)acosh(Float(x)); }
  double tlfloat_acosh(const double x) { return (double)acosh(Double(x)); }
  tlfloat_quad tlfloat_acoshq(const tlfloat_quad x) { return (tlfloat_quad)acosh(Quad(x)); }
  tlfloat_octuple tlfloat_acosho(const tlfloat_octuple x) { return (tlfloat_octuple)acosh(Octuple(x)); }

  float tlfloat_atanhf(const float x) { return (float)atanh(Float(x)); }
  double tlfloat_atanh(const double x) { return (double)atanh(Double(x)); }
  tlfloat_quad tlfloat_atanhq(const tlfloat_quad x) { return (tlfloat_quad)atanh(Quad(x)); }
  tlfloat_octuple tlfloat_atanho(const tlfloat_octuple x) { return (tlfloat_octuple)atanh(Octuple(x)); }

  float tlfloat_fmodf(const float x, const float y) { return (float)fmod(Float(x), Float(y)); }
  double tlfloat_fmod(const double x, const double y) { return (double)fmod(Double(x), Double(y)); }
  tlfloat_quad tlfloat_fmodq(const tlfloat_quad x, const tlfloat_quad y) { return (tlfloat_quad)fmod(Quad(x), Quad(y)); }
  tlfloat_octuple tlfloat_fmodo(const tlfloat_octuple x, const tlfloat_octuple y) { return (tlfloat_octuple)fmod(Octuple(x), Octuple(y)); }

  float tlfloat_remainderf(const float x, const float y) { return (float)remainder(Float(x), Float(y)); }
  double tlfloat_remainder(const double x, const double y) { return (double)remainder(Double(x), Double(y)); }
  tlfloat_quad tlfloat_remainderq(const tlfloat_quad x, const tlfloat_quad y) { return (tlfloat_quad)remainder(Quad(x), Quad(y)); }
  tlfloat_octuple tlfloat_remaindero(const tlfloat_octuple x, const tlfloat_octuple y) { return (tlfloat_octuple)remainder(Octuple(x), Octuple(y)); }
}
