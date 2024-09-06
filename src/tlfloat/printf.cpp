#include <cstring>
#include <cctype>
#include <cstdarg>

#include "suppress.hpp"

#define TLFLOAT_NO_LIBSTDCXX
#include "tlfloat/tlmath.hpp"

#include "tlfloat/tlfloat.h"

using namespace tlfloat;

namespace {
  static int xvprintf(size_t (*consumer)(const char *ptr, size_t size, void *arg), void *arg, const char *fmt, va_list ap) {
    const int xbufsize = 5000;
    char *xbuf = (char *)calloc(xbufsize+10, 1);
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

      if (*fmt == '%') {
	outlen += (*consumer)("%", 1, arg);
	fmt++;
	continue;
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

      bool subfmt_processed = false;
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
      } else {
	int pl = 0;
	if (*fmt == 'h' || *fmt == 'l' || *fmt == 'j' || *fmt == 'z' || *fmt == 't' || *fmt == 'L' || *fmt == 'Q' || *fmt == 'O') {
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
	    if (size_prefix == 'Q') {
	      Quad value = std::bit_cast<Quad>(va_arg(ap, tlfloat_quad_));
	      typedef decltype(decltype(value.getUnpacked())::xUnpackedFloat()) xUnpacked_t;
	      int ret = snprint(xbuf, xbufsize, value.getUnpacked().cast((xUnpacked_t *)0), *fmt, width, precision, 
				flag_sign, flag_blank, flag_alt, flag_left, flag_zero, flag_upper);
	      if (ret < 0) { errorflag = 1; break; }
	      outlen += (*consumer)(xbuf, strlen(xbuf), arg);
	      subfmt_processed = true;
	    } else if (size_prefix == 'O') {
	      Octuple value = std::bit_cast<Octuple>(va_arg(ap, tlfloat_octuple_));
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
	  bool flag_unsigned = (*fmt == 'u') || (*fmt == 'o') || (*fmt == 'x') || (*fmt == 'X');
	  bool flag_ptr = false;
	  if (*fmt == 'X') flag_upper = true;

	  char const *prefix = "";
	  int base = 10;
	  if (*fmt == 'o') { base = 8; prefix = "0"; }
	  if (*fmt == 'x' || *fmt == 'X') { base = 16; prefix = flag_upper ? "0X" : "0x"; }

	  if (nbits == 0) {
	    if (size_prefix == 'Q') {
#ifdef TLFLOAT_COMPILER_SUPPORTS_INT128
	      BigInt<7> value = std::bit_cast<BigInt<7>>(va_arg(ap, __int128_t));
#else
	      BigInt<7> value = std::bit_cast<BigInt<7>>(va_arg(ap, tlfloat_int128_t_));
#endif
	      int ret = BigInt<7>::snprint(xbuf, xbufsize, value, tolower(*fmt), width, precision, base, 1 << 7,
					   flag_sign, flag_blank, flag_alt, flag_left, flag_zero, flag_upper, flag_unsigned, flag_ptr, prefix);
	      if (ret < 0) { errorflag = 1; break; }
	      outlen += (*consumer)(xbuf, strlen(xbuf), arg);
	      subfmt_processed = true;
	    } else {
	      switch(size_prefix) {
	      case 0: case 'h': case 'h' + 256*'h': va_arg(ap, int); break;
	      case 'l': va_arg(ap, long int); break;
	      case 'j': va_arg(ap, intmax_t); break;
	      case 'z': va_arg(ap, size_t); break;
	      case 't': va_arg(ap, std::ptrdiff_t); break;
	      case 'l' + 256*'l': va_arg(ap, long long int); break;
	      default: errorflag = 1; break;
	      }
	    }
	  } else {
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
	      typedef struct { uint64_t e[2]; } arg_t;
	      BigInt<7> value = std::bit_cast<BigInt<7>>(va_arg(ap, arg_t));
	      int ret = BigInt<7>::snprint(xbuf, xbufsize, value, tolower(*fmt), width, precision, base, nbits,
					   flag_sign, flag_blank, flag_alt, flag_left, flag_zero, flag_upper, flag_unsigned, flag_ptr, prefix);
	      if (ret < 0) { errorflag = 1; break; }
	      outlen += (*consumer)(xbuf, strlen(xbuf), arg);
	      subfmt_processed = true;
	      break;
	    }
	    case 256: {
	      typedef struct { uint64_t e[4]; } arg_t;
	      BigInt<8> value = std::bit_cast<BigInt<8>>(va_arg(ap, arg_t));
	      int ret = BigInt<8>::snprint(xbuf, xbufsize, value, tolower(*fmt), width, precision, base, nbits,
					   flag_sign, flag_blank, flag_alt, flag_left, flag_zero, flag_upper, flag_unsigned, flag_ptr, prefix);
	      if (ret < 0) { errorflag = 1; break; }
	      outlen += (*consumer)(xbuf, strlen(xbuf), arg);
	      subfmt_processed = true;
	      break;
	    }
	    case 512: {
	      typedef struct { uint64_t e[8]; } arg_t;
	      BigInt<9> value = std::bit_cast<BigInt<9>>(va_arg(ap, arg_t));
	      int ret = BigInt<9>::snprint(xbuf, xbufsize, value, tolower(*fmt), width, precision, base, nbits,
					   flag_sign, flag_blank, flag_alt, flag_left, flag_zero, flag_upper, flag_unsigned, flag_ptr, prefix);
	      if (ret < 0) { errorflag = 1; break; }
	      outlen += (*consumer)(xbuf, strlen(xbuf), arg);
	      subfmt_processed = true;
	      break;
	    }
	    case 1024: {
	      typedef struct { uint64_t e[16]; } arg_t;
	      BigInt<10> value = std::bit_cast<BigInt<10>>(va_arg(ap, arg_t));
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
	char *subfmt = (char *)calloc(fmt - subfmtstart + 2, 1);
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

  //

  float tlfloat_strtof(const char *nptr, const char **endptr) { return (float)Float(nptr, endptr); }
  double tlfloat_strtod(const char *nptr, const char **endptr) { return (double)Double(nptr, endptr); }
  tlfloat_quad_ tlfloat_strtoq(const char *nptr, const char **endptr) { return (tlfloat_quad_)Quad(nptr, endptr); }
  tlfloat_octuple_ tlfloat_strtoo(const char *nptr, const char **endptr) { return (tlfloat_octuple_)Octuple(nptr, endptr); }

  //

  tlfloat_int128_t_ tlfloat_strtoi128(const char *nptr, const char **endptr, const int base) {
    BigInt<7> b = BigInt<7>(nptr, endptr, base);
    tlfloat_int128_t_ r;
    memcpy(&r, &b, sizeof(r));
    return r;
  }

  tlfloat_uint128_t_ tlfloat_strtou128(const char *nptr, const char **endptr, const int base) {
    BigUInt<7> b = BigUInt<7>(nptr, endptr, base);
    tlfloat_uint128_t_ r;
    memcpy(&r, &b, sizeof(r));
    return r;
  }
}
