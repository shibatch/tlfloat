#include <string>

#if defined(__x86_64__) && defined(__GNUC__) && !defined(__clang__)
#define ENABLE_QUAD
#include <quadmath.h>
typedef __float128 quad;
#define M_PIq_ M_PIq

std::ostream& operator<<(std::ostream &os, const __float128& f) {
  char buf[128];
  quadmath_snprintf(buf, sizeof(buf), "%.32Qg", f);
  return os << std::string(buf);
}
#endif

#if defined(__aarch64__) && defined(__GNUC__)
#define ENABLE_QUAD
typedef long double quad;
#define fabsq fabsl
#define fmaq fmal
#define sqrtq sqrtl
#define ilogbq ilogbl
#define finiteq finitel
#define isnanq isnanl
#define truncq truncl
#define floorq floorl
#define ceilq ceill
#define roundq roundl
#define rintq rintl
#define M_PIq_ M_PIl
#endif

typedef tlfloat::detail::UnpackedFloat<uint32_t, uint64_t, 8, 23> ufloat;
typedef tlfloat::detail::UnpackedFloat<uint64_t, tlfloat::BigUInt<7>, 11, 52> udouble;
typedef tlfloat::detail::UnpackedFloat<tlfloat::BigUInt<7>, tlfloat::BigUInt<8>, 15, 112> uquad;
typedef tlfloat::detail::UnpackedFloat<tlfloat::BigUInt<8>, tlfloat::BigUInt<9>, 19, 236> uoctuple;

typedef tlfloat::detail::UnpackedFloat<uint32_t, uint64_t, 0, 30> xfloat;
typedef tlfloat::detail::UnpackedFloat<uint64_t, tlfloat::BigUInt<7>, 0, 62> xdouble;
typedef tlfloat::detail::UnpackedFloat<tlfloat::BigUInt<7>, tlfloat::BigUInt<8>, 0, 126> xquad;
typedef tlfloat::detail::UnpackedFloat<tlfloat::BigUInt<8>, tlfloat::BigUInt<9>, 0, 254> xoctuple;
typedef tlfloat::detail::UnpackedFloat<tlfloat::BigUInt<9>, tlfloat::BigUInt<10>, 0, 510> xsedecuple;

xfloat   xfpi(0x6487ed51U, 0, false, false, false, false);
xdouble  xdpi(0x6487ed5110b4611aULL, 0, false, false, false, false);
uint64_t piqa[] = {0x62633145c06e0e69, 0x6487ed5110b4611aULL};
xquad xqpi(tlfloat::BigUInt<7>(piqa), 0, false, false, false, false);
uint64_t pioa[] = {0x0105DF531D89CD91ULL, 0x948127044533E63AULL, 0x62633145C06E0E68ULL, 0x6487ED5110B4611AULL};
xoctuple xopi(tlfloat::BigUInt<8>(pioa), 0, false, false, false, false);
#ifdef ENABLE_QUAD
static_assert(sizeof(quad) == 16, "quad precision FP not supported");
#else
typedef tlfloat::detail::UnpackedFloat<tlfloat::BigUInt<7>, tlfloat::BigUInt<8>, 0, 126> xquad;
const tlfloat::Quad M_PIq_ = (tlfloat::Quad)xqpi.cast((tlfloat::detail::UnpackedFloat<tlfloat::BigUInt<7>, tlfloat::BigUInt<8>, 15, 112> *)0);
#endif

std::string toHexString(unsigned long long u64) {
  char buf[17];
  snprintf(buf, 17, "%016llx", (unsigned long long)u64);
  return std::string(buf);
}

std::string toHexString(unsigned long u64) {
  char buf[17];
  snprintf(buf, 17, "%016llx", (unsigned long long)u64);
  return std::string(buf);
}

std::string toHexString(unsigned u32) {
  char buf[17];
  snprintf(buf, 17, "%08x", u32);
  return std::string(buf);
}

std::string toHexString(unsigned short u16) {
  char buf[17];
  snprintf(buf, 17, "%04x", (unsigned)u16);
  return std::string(buf);
}

template<typename T>
std::string toHexString(T x) { return x.toHexString(); }

#ifdef MPFR_VERSION_MAJOR
std::string to_string(const mpfr_t& fr, int digits = 51) {
  mpfr_t t;
  mpfr_inits(t, NULL);
  int sign = mpfr_signbit(fr) ? -1 : 1;
  char *s = (char *)malloc(digits + 20);
  if (mpfr_inf_p(fr)) {
    sprintf(s, "%cinf", sign < 0 ? '-' : '+');
  } else if (mpfr_nan_p(fr)) {
    sprintf(s, "nan");
  } else {
    mpfr_exp_t e;
    s[0] = sign < 0 ? '-' : '+';
    s[1] = '0';
    s[2] = '.';
    mpfr_abs(t, fr, GMP_RNDN);
    mpfr_get_str(s+3, &e, 10, digits, t, GMP_RNDN);
    int ie = e;
    char es[32];
    snprintf(es, 30, "e%c%d", ie >= 0 ? '+' : '-', ie >= 0 ? ie : -ie);
    strncat(s, es, digits+20);
  }

  mpfr_clears(t, NULL);
  std::string str = s;
  free(s);
  return str;
}

std::ostream& operator<<(std::ostream &os, const mpfr_t& fr) {
  return os << to_string(fr);
}

void mpfr_set_inttype(mpfr_t &rop, uint16_t op, const mpfr_rnd_t rnd) {
  mpfr_set_d(rop, op, GMP_RNDN);
}

template<typename inttype>
static void mpfr_set_inttype(mpfr_t &rop, inttype op, const mpfr_rnd_t rnd) {
  mpfr_set_d(rop, 0, GMP_RNDN);
  for(unsigned u=0;u<sizeof(inttype)/sizeof(uint32_t);u++) {
    double d = double(op & 0xffffffffULL);
    if (sizeof(op) > 4) op >>= 32;
    mpfr_add_d(rop, rop, ldexp(d, 32 * u), GMP_RNDN);
  }
}

template<typename Unpacked_t>
static void mpfr_set_unpacked(mpfr_t &rop, const Unpacked_t& ux, const mpfr_rnd_t rnd) {
  if (ux.isnan) {
    mpfr_set_nan(rop);
    return;
  }
  if (ux.isinf) {
    mpfr_set_inf(rop, ux.sign);
    mpfr_setsign(rop, rop, ux.sign, GMP_RNDN);
    return;
  }
  if (ux.iszero) {
    mpfr_set_zero(rop, ux.sign);
    mpfr_setsign(rop, rop, ux.sign, GMP_RNDN);
    return;
  }

  mpfr_set_inttype(rop, ux.mant, rnd);
  mpfr_set_exp(rop, ux.ilogb() + 1);
  mpfr_setsign(rop, rop, ux.sign, GMP_RNDN);
}

int mpfr_ph(mpfr_t &rop, const mpfr_t &op, int e) {
  mpfr_t pi;
  mpfr_inits(pi, NULL);

  mpfr_const_pi(pi, GMP_RNDN);
  mpfr_mul_d(pi, pi, ldexp(1, -e), GMP_RNDN);

  //

  long int q;
  mpfr_remquo(rop, &q, op, pi, GMP_RNDN);

  mpfr_clears(pi, NULL);

  return q;
}

template<typename Unpacked_t>
static int mpfr_ph(mpfr_t &rop, const Unpacked_t& ux, int e) {
  mpfr_t x;
  mpfr_inits(x, NULL);

  mpfr_set_unpacked(x, ux, GMP_RNDN);
  int q = mpfr_ph(rop, x, e);

  mpfr_clears(x, NULL);

  return q & 0xff;
}

#ifdef ENABLE_QUAD
quad mpfr_sqrt(quad a) {
  mpfr_t x;
  mpfr_inits(x, NULL);

  mpfr_set_float128(x, a, GMP_RNDN);
  mpfr_sqrt(x, x, GMP_RNDN);
  quad d = mpfr_get_float128(x, GMP_RNDN);

  mpfr_clears(x, NULL);

  return d;
}
#endif

template<typename Unpacked_t, typename Unpacked2_t>
static double countULP(const Unpacked_t& ux, mpfr_t c, const Unpacked2_t& umin, const Unpacked2_t& umax, bool checkSignedZero=false) {
  const int nbmant = umin.nbmant_();
  mpfr_t fra, frb, frc, frd, frmin, frmino2, frmax;
  mpfr_inits(fra, frb, frc, frd, frmin, frmino2, frmax, NULL);

  mpfr_set_unpacked(frmin, umin, GMP_RNDN);
  mpfr_mul_d(frmino2, frmin, 0.5, GMP_RNDN);
  mpfr_set_unpacked(frmax, umax, GMP_RNDN);

  mpfr_abs(fra, c, GMP_RNDN);
  bool ciszero = mpfr_cmp(fra, frmino2) < 0;
  bool cisinf = mpfr_cmp(fra, frmax) > 0;

  if (ciszero && !ux.iszero) {
    mpfr_clears(fra, frb, frc, frd, frmin, frmino2, frmax, NULL);
    return 10000;
  }
  if (checkSignedZero && ciszero && ux.iszero) {
    if (mpfr_signbit(c) != ux.sign) {
      mpfr_clears(fra, frb, frc, frd, frmin, frmino2, frmax, NULL);
      return 10002;
    }
  }
  if (cisinf && ux.isinf && mpfr_signbit(c) == ux.sign) {
    mpfr_clears(fra, frb, frc, frd, frmin, frmino2, frmax, NULL);
    return 0;
  }
  if (mpfr_nan_p(c) && ux.isnan) {
    mpfr_clears(fra, frb, frc, frd, frmin, frmino2, frmax, NULL);
    return 0;
  }
  if (mpfr_nan_p(c) || ux.isnan) {
    mpfr_clears(fra, frb, frc, frd, frmin, frmino2, frmax, NULL);
    return 10001;
  }

  int e = mpfr_get_exp(c);

  mpfr_set_d(frb, 1, GMP_RNDN);
  mpfr_set_exp(frb, e - nbmant);
  mpfr_max(frb, frb, frmin, GMP_RNDN);

  mpfr_set_unpacked(frd, ux, GMP_RNDN);
  mpfr_sub(fra, frd, c, GMP_RNDN);
  mpfr_div(fra, fra, frb, GMP_RNDN);
  double u = fabs(mpfr_get_d(fra, GMP_RNDN));

  mpfr_clears(fra, frb, frc, frd, frmin, frmino2, frmax, NULL);

  return u;
}
#endif
