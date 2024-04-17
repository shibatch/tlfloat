#include <cstring>
#include <cctype>
#include <cstdarg>

#ifdef TLFLOAT_SUPPRESS_WARNINGS
#include "suppress.hpp"
#endif

#define TLFLOAT_NO_LIBSTDCXX
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

  double tlfloat_cast_d_q(const tlfloat_quad x         ) { return (double)Double(Quad(x)); }
  double tlfloat_cast_d_o(const tlfloat_octuple x      ) { return (double)Double(Octuple(x)); }

  tlfloat_quad tlfloat_cast_q_d(const double x         ) { return (tlfloat_quad)Quad(Double(x)); }
  tlfloat_quad tlfloat_cast_q_o(const tlfloat_octuple x) { return (tlfloat_quad)Quad(Octuple(x)); }

  tlfloat_octuple tlfloat_cast_o_d(const double x      ) { return (tlfloat_octuple)Octuple(Double(x)); }
  tlfloat_octuple tlfloat_cast_o_q(const tlfloat_quad x) { return (tlfloat_octuple)Octuple(Quad(x)); }

  int64_t tlfloat_cast_i64_q(const tlfloat_quad x) { return (int64_t)Quad(x); }
  tlfloat_quad tlfloat_cast_q_i64(const int64_t x) { return (tlfloat_quad)Quad(x); }

  int64_t tlfloat_cast_i64_o(const tlfloat_octuple x) { return (int64_t)Octuple(x); }
  tlfloat_octuple tlfloat_cast_o_i64(const int64_t x) { return (tlfloat_octuple)Octuple(x); }

  uint64_t tlfloat_cast_u64_q(const tlfloat_quad x) { return (uint64_t)Quad(x); }
  tlfloat_quad tlfloat_cast_q_u64(const uint64_t x) { return (tlfloat_quad)Quad(x); }

  uint64_t tlfloat_cast_u64_o(const tlfloat_octuple x) { return (uint64_t)Octuple(x); }
  tlfloat_octuple tlfloat_cast_o_u64(const uint64_t x) { return (tlfloat_octuple)Octuple(x); }

  //

  int tlfloat_eq_q_q (const tlfloat_quad x, const tlfloat_quad y) { return Quad(x) == Quad(y); }
  int tlfloat_neq_q_q(const tlfloat_quad x, const tlfloat_quad y) { return Quad(x) != Quad(y); }
  int tlfloat_lt_q_q (const tlfloat_quad x, const tlfloat_quad y) { return Quad(x) <  Quad(y); }
  int tlfloat_le_q_q (const tlfloat_quad x, const tlfloat_quad y) { return Quad(x) <= Quad(y); }
  int tlfloat_gt_q_q (const tlfloat_quad x, const tlfloat_quad y) { return Quad(x) >  Quad(y); }
  int tlfloat_ge_q_q (const tlfloat_quad x, const tlfloat_quad y) { return Quad(x) >= Quad(y); }

  int tlfloat_eq_o_o (const tlfloat_octuple x, const tlfloat_octuple y) { return Octuple(x) == Octuple(y); }
  int tlfloat_neq_o_o(const tlfloat_octuple x, const tlfloat_octuple y) { return Octuple(x) != Octuple(y); }
  int tlfloat_lt_o_o (const tlfloat_octuple x, const tlfloat_octuple y) { return Octuple(x) <  Octuple(y); }
  int tlfloat_le_o_o (const tlfloat_octuple x, const tlfloat_octuple y) { return Octuple(x) <= Octuple(y); }
  int tlfloat_gt_o_o (const tlfloat_octuple x, const tlfloat_octuple y) { return Octuple(x) >  Octuple(y); }
  int tlfloat_ge_o_o (const tlfloat_octuple x, const tlfloat_octuple y) { return Octuple(x) >= Octuple(y); }

  //

  tlfloat_quad tlfloat_addq(const tlfloat_quad x, const tlfloat_quad y) { return (tlfloat_quad)(Quad(x) + Quad(y)); }
  tlfloat_octuple tlfloat_addo(const tlfloat_octuple x, const tlfloat_octuple y) { return (tlfloat_octuple)(Octuple(x) + Octuple(y)); }

  tlfloat_quad tlfloat_subq(const tlfloat_quad x, const tlfloat_quad y) { return (tlfloat_quad)(Quad(x) - Quad(y)); }
  tlfloat_octuple tlfloat_subo(const tlfloat_octuple x, const tlfloat_octuple y) { return (tlfloat_octuple)(Octuple(x) - Octuple(y)); }

  tlfloat_quad tlfloat_mulq(const tlfloat_quad x, const tlfloat_quad y) { return (tlfloat_quad)(Quad(x) * Quad(y)); }
  tlfloat_octuple tlfloat_mulo(const tlfloat_octuple x, const tlfloat_octuple y) { return (tlfloat_octuple)(Octuple(x) * Octuple(y)); }

  tlfloat_quad tlfloat_divq(const tlfloat_quad x, const tlfloat_quad y) { return (tlfloat_quad)(Quad(x) / Quad(y)); }
  tlfloat_octuple tlfloat_divo(const tlfloat_octuple x, const tlfloat_octuple y) { return (tlfloat_octuple)(Octuple(x) / Octuple(y)); }

  float tlfloat_fmaf(const float x, const float y, const float z) { return (float)fma(Float(x), Float(y), Float(z)); }
  double tlfloat_fma(const double x, const double y, const double z) { return (double)fma(Double(x), Double(y), Double(z)); }
  tlfloat_quad tlfloat_fmaq(const tlfloat_quad x, const tlfloat_quad y, const tlfloat_quad z) { return (tlfloat_quad)fma(Quad(x), Quad(y), Quad(z)); }
  tlfloat_octuple tlfloat_fmao(const tlfloat_octuple x, const tlfloat_octuple y, const tlfloat_octuple z) { return (tlfloat_octuple)fma(Octuple(x), Octuple(y), Octuple(z)); }

  float tlfloat_sqrtf(const float x) { return (float)sqrt(Float(x)); }
  double tlfloat_sqrt(const double x) { return (double)sqrt(Double(x)); }
  tlfloat_quad tlfloat_sqrtq(const tlfloat_quad x) { return (tlfloat_quad)sqrt(Quad(x)); }
  tlfloat_octuple tlfloat_sqrto(const tlfloat_octuple x) { return (tlfloat_octuple)sqrt(Octuple(x)); }

  //

  float tlfloat_fabsf(const float x) { return (float)fabs(Float(x)); }
  double tlfloat_fabs(const double x) { return (double)fabs(Double(x)); }
  tlfloat_quad tlfloat_fabsq(const tlfloat_quad x) { return (tlfloat_quad)fabs(Quad(x)); }
  tlfloat_octuple tlfloat_fabso(const tlfloat_octuple x) { return (tlfloat_octuple)fabs(Octuple(x)); }

  float tlfloat_copysignf(const float x, const float y) { return (float)copysign(Float(x), Float(y)); }
  double tlfloat_copysign(const double x, const double y) { return (double)copysign(Double(x), Double(y)); }
  tlfloat_quad tlfloat_copysignq(const tlfloat_quad x, const tlfloat_quad y) { return (tlfloat_quad)copysign(Quad(x), Quad(y)); }
  tlfloat_octuple tlfloat_copysigno(const tlfloat_octuple x, const tlfloat_octuple y) { return (tlfloat_octuple)copysign(Octuple(x), Octuple(y)); }

  float tlfloat_fmaxf(const float x, const float y) { return (float)fmax(Float(x), Float(y)); }
  double tlfloat_fmax(const double x, const double y) { return (double)fmax(Double(x), Double(y)); }
  tlfloat_quad tlfloat_fmaxq(const tlfloat_quad x, const tlfloat_quad y) { return (tlfloat_quad)fmax(Quad(x), Quad(y)); }
  tlfloat_octuple tlfloat_fmaxo(const tlfloat_octuple x, const tlfloat_octuple y) { return (tlfloat_octuple)fmax(Octuple(x), Octuple(y)); }

  float tlfloat_fminf(const float x, const float y) { return (float)fmin(Float(x), Float(y)); }
  double tlfloat_fmin(const double x, const double y) { return (double)fmin(Double(x), Double(y)); }
  tlfloat_quad tlfloat_fminq(const tlfloat_quad x, const tlfloat_quad y) { return (tlfloat_quad)fmin(Quad(x), Quad(y)); }
  tlfloat_octuple tlfloat_fmino(const tlfloat_octuple x, const tlfloat_octuple y) { return (tlfloat_octuple)fmin(Octuple(x), Octuple(y)); }

  float tlfloat_fdimf(const float x, const float y) { return (float)fdim(Float(x), Float(y)); }
  double tlfloat_fdim(const double x, const double y) { return (double)fdim(Double(x), Double(y)); }
  tlfloat_quad tlfloat_fdimq(const tlfloat_quad x, const tlfloat_quad y) { return (tlfloat_quad)fdim(Quad(x), Quad(y)); }
  tlfloat_octuple tlfloat_fdimo(const tlfloat_octuple x, const tlfloat_octuple y) { return (tlfloat_octuple)fdim(Octuple(x), Octuple(y)); }

  float tlfloat_ldexpf(const float x, const int y) { return (float)ldexp(Float(x), y); }
  double tlfloat_ldexp(const double x, const int y) { return (double)ldexp(Double(x), y); }
  tlfloat_quad tlfloat_ldexpq(const tlfloat_quad x, const int y) { return (tlfloat_quad)ldexp(Quad(x), y); }
  tlfloat_octuple tlfloat_ldexpo(const tlfloat_octuple x, const int y) { return (tlfloat_octuple)ldexp(Octuple(x), y); }

  float tlfloat_frexpf(const float x, int *y) { return (float)frexp(Float(x), y); }
  double tlfloat_frexp(const double x, int *y) { return (double)frexp(Double(x), y); }
  tlfloat_quad tlfloat_frexpq(const tlfloat_quad x, int *y) { return (tlfloat_quad)frexp(Quad(x), y); }
  tlfloat_octuple tlfloat_frexpo(const tlfloat_octuple x, int *y) { return (tlfloat_octuple)frexp(Octuple(x), y); }

  float tlfloat_modff(const float x, float *y) {
    Float z;
    float r = (float)modf(Float(x), &z);
    *y = (float)z;
    return r;
  }
  double tlfloat_modf(const double x, double *y) {
    Double z;
    double r = (double)modf(Double(x), &z);
    *y = (double)z;
    return r;
  }
  tlfloat_quad tlfloat_modfq(const tlfloat_quad x, tlfloat_quad *y) {
    Quad z;
    tlfloat_quad r = (tlfloat_quad)modf(Quad(x), &z);
    *y = (tlfloat_quad)z;
    return r;
  }
  tlfloat_octuple tlfloat_modfo(const tlfloat_octuple x, tlfloat_octuple *y) {
    Octuple z;
    tlfloat_octuple r = (tlfloat_octuple)modf(Octuple(x), &z);
    *y = (tlfloat_octuple)z;
    return r;
  }

  float tlfloat_nextafterf(const float x, const float y) { return (float)nextafter(Float(x), Float(y)); }
  double tlfloat_nextafter(const double x, const double y) { return (double)nextafter(Double(x), Double(y)); }
  tlfloat_quad tlfloat_nextafterq(const tlfloat_quad x, const tlfloat_quad y) { return (tlfloat_quad)nextafter(Quad(x), Quad(y)); }
  tlfloat_octuple tlfloat_nextaftero(const tlfloat_octuple x, const tlfloat_octuple y) { return (tlfloat_octuple)nextafter(Octuple(x), Octuple(y)); }

  int tlfloat_ilogbf(const float x) { return ilogb(Float(x)); }
  int tlfloat_ilogb(const double x) { return ilogb(Double(x)); }
  int tlfloat_ilogbq(const tlfloat_quad x) { return ilogb(Quad(x)); }
  int tlfloat_ilogbo(const tlfloat_octuple x) { return ilogb(Octuple(x)); }

  int tlfloat_isnanf(const float x) { return isnan(Float(x)); }
  int tlfloat_isnan(const double x) { return isnan(Double(x)); }
  int tlfloat_isnanq(const tlfloat_quad x) { return isnan(Quad(x)); }
  int tlfloat_isnano(const tlfloat_octuple x) { return isnan(Octuple(x)); }

  int tlfloat_isinff(const float x) { return isinf(Float(x)); }
  int tlfloat_isinf(const double x) { return isinf(Double(x)); }
  int tlfloat_isinfq(const tlfloat_quad x) { return isinf(Quad(x)); }
  int tlfloat_isinfo(const tlfloat_octuple x) { return isinf(Octuple(x)); }

  int tlfloat_finitef(const float x) { return finite(Float(x)); }
  int tlfloat_finite(const double x) { return finite(Double(x)); }
  int tlfloat_finiteq(const tlfloat_quad x) { return finite(Quad(x)); }
  int tlfloat_finiteo(const tlfloat_octuple x) { return finite(Octuple(x)); }

  int tlfloat_signbitf(const float x) { return signbit(Float(x)); }
  int tlfloat_signbit(const double x) { return signbit(Double(x)); }
  int tlfloat_signbitq(const tlfloat_quad x) { return signbit(Quad(x)); }
  int tlfloat_signbito(const tlfloat_octuple x) { return signbit(Octuple(x)); }

  //

  float tlfloat_hypotf(const float x, const float y) { return (float)hypot(Float(x), Float(y)); }
  double tlfloat_hypot(const double x, const double y) { return (double)hypot(Double(x), Double(y)); }
  tlfloat_quad tlfloat_hypotq(const tlfloat_quad x, const tlfloat_quad y) { return (tlfloat_quad)hypot(Quad(x), Quad(y)); }
  tlfloat_octuple tlfloat_hypoto(const tlfloat_octuple x, const tlfloat_octuple y) { return (tlfloat_octuple)hypot(Octuple(x), Octuple(y)); }

  float tlfloat_truncf(const float x) { return (float)trunc(Float(x)); }
  double tlfloat_trunc(const double x) { return (double)trunc(Double(x)); }
  tlfloat_quad tlfloat_truncq(const tlfloat_quad x) { return (tlfloat_quad)trunc(Quad(x)); }
  tlfloat_octuple tlfloat_trunco(const tlfloat_octuple x) { return (tlfloat_octuple)trunc(Octuple(x)); }

  float tlfloat_floorf(const float x) { return (float)floor(Float(x)); }
  double tlfloat_floor(const double x) { return (double)floor(Double(x)); }
  tlfloat_quad tlfloat_floorq(const tlfloat_quad x) { return (tlfloat_quad)floor(Quad(x)); }
  tlfloat_octuple tlfloat_flooro(const tlfloat_octuple x) { return (tlfloat_octuple)floor(Octuple(x)); }

  float tlfloat_ceilf(const float x) { return (float)ceil(Float(x)); }
  double tlfloat_ceil(const double x) { return (double)ceil(Double(x)); }
  tlfloat_quad tlfloat_ceilq(const tlfloat_quad x) { return (tlfloat_quad)ceil(Quad(x)); }
  tlfloat_octuple tlfloat_ceilo(const tlfloat_octuple x) { return (tlfloat_octuple)ceil(Octuple(x)); }

  float tlfloat_roundf(const float x) { return (float)round(Float(x)); }
  double tlfloat_round(const double x) { return (double)round(Double(x)); }
  tlfloat_quad tlfloat_roundq(const tlfloat_quad x) { return (tlfloat_quad)round(Quad(x)); }
  tlfloat_octuple tlfloat_roundo(const tlfloat_octuple x) { return (tlfloat_octuple)round(Octuple(x)); }

  float tlfloat_rintf(const float x) { return (float)rint(Float(x)); }
  double tlfloat_rint(const double x) { return (double)rint(Double(x)); }
  tlfloat_quad tlfloat_rintq(const tlfloat_quad x) { return (tlfloat_quad)rint(Quad(x)); }
  tlfloat_octuple tlfloat_rinto(const tlfloat_octuple x) { return (tlfloat_octuple)rint(Octuple(x)); }

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
