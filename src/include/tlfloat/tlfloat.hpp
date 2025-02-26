#ifndef __TLFLOAT_HPP_INCLUDED__
#define __TLFLOAT_HPP_INCLUDED__

#include <cstdio>

#include <tlfloat/bigint.hpp>

#ifndef TLFLOAT_FP_NAN
#define TLFLOAT_FP_NAN 0
#define TLFLOAT_FP_INFINITE 1
#define TLFLOAT_FP_ZERO 2
#define TLFLOAT_FP_SUBNORMAL 3
#define TLFLOAT_FP_NORMAL 4
#define TLFLOAT_FP_ILOGB0 ((int)0x80000000)
#define TLFLOAT_FP_ILOGBNAN ((int)2147483647)
#endif

namespace tlfloat {
  namespace detail {
    static const double LOG10_2_ = 0.30102999566398119521; // log(2)/log(10)

    static constexpr bool xisspace(int c) { return c == ' ' || c == '\f' || c == '\n' || c == '\r' || c == '\t' || c == '\v'; }
    static constexpr bool xisdigit(int c) { return '0' <= c && c <= '9'; }
    static constexpr int xtolower(int c) { if ('A' <= c && c <= 'Z') return c - 'A' + 'a'; else return c; }

    static constexpr int xstrncasecmp(const char *p, const char *q, size_t n) {
      for(;n>0;n--) {
	if (*p == '\0' && *q == '\0') return 0;
	if (xtolower(*p) != xtolower(*q)) return *p > *q ? 1 : -1;
	p++; q++;
      }
      return 0;
    }

    static constexpr long long xstrtoll(const char *p, const char **endptr=nullptr) {
      bool negative = false;
      long long r = 0;
      while(xisspace(*p)) p++;
      if (*p == '-') {
	negative = true; p++;
      } else if (*p == '+') p++;
      while(xisdigit(*p)) {
	r = r * 10 + (*p - '0');
	p++;
      }
      if (endptr) *endptr = p;
      return negative ? -r : r;
    }

    static constexpr TLFLOAT_INLINE double xsqrt(double d) {
      if (std::is_constant_evaluated()) {
	if (d == 0) return d;
	if (d < 0) return NAN;
	if (d == INFINITY) return INFINITY;

	double q = 1;

	if (d < 8.636168555094445E-78) {
	  d *= 1.157920892373162E77;
	  q = 2.9387358770557188E-39;
	} else if (d > 1.3407807929942597e+154) {
	  d *= 7.4583407312002070e-155;
	  q = 1.1579208923731620e+77;
	}

	double x = std::bit_cast<double>(0x5fe6ec85e7de30da - (std::bit_cast<uint64_t>(d) >> 1));

	d *= 0.5;
	x *= 1.5 - d * x * x;
	x *= 1.5 - d * x * x;
	x *= 1.5 - d * x * x;

	return (x * d + 0.5 / x) * q;
      }
      return sqrt(d);
    }

    //

    static constexpr TLFLOAT_INLINE int icmp(uint16_t x, uint16_t y) {
      return x > y ? 1 : (x < y ? -1 : 0);
    }

    static constexpr TLFLOAT_INLINE int icmp(uint32_t x, uint32_t y) {
      return x > y ? 1 : (x < y ? -1 : 0);
    }

    static constexpr TLFLOAT_INLINE int icmp(uint64_t x, uint64_t y) {
      return x > y ? 1 : (x < y ? -1 : 0);
    }

    template<int N>
    static constexpr TLFLOAT_INLINE int icmp(const BigUInt<N>& x, const BigUInt<N>& y) {
      return x.compare(y);
    }

    //

    static constexpr TLFLOAT_INLINE uint32_t mul(uint16_t x, uint16_t y) {
      return uint32_t(x) * uint32_t(y);
    }

    static constexpr TLFLOAT_INLINE uint64_t mul(uint32_t x, uint32_t y) {
      return uint64_t(x) * uint64_t(y);
    }

    static constexpr TLFLOAT_INLINE BigUInt<7> mul(uint64_t x, uint64_t y) {
      return BigUInt<7>::mul(BigUInt<6>(x), BigUInt<6>(y));
    }

    template<int N>
    static constexpr TLFLOAT_INLINE BigUInt<N+1> mul(const BigUInt<N>& x, const BigUInt<N>& y) {
      return BigUInt<N+1>::mul(x, y);
    }

    //

    static constexpr TLFLOAT_INLINE xpair<uint16_t, uint16_t> divmod2(uint16_t x_, uint16_t y_) {
      uint32_t x = uint32_t(x_) << 15, y = y_ | (uint32_t(1) << 15);
      return xpair<uint16_t, uint16_t> { uint16_t(x / y), uint16_t(x % y) };
    }

    static constexpr TLFLOAT_INLINE xpair<uint32_t, uint32_t> divmod2(uint32_t x_, uint32_t y_) {
      uint64_t x = uint64_t(x_) << 31, y = y_ | (uint64_t(1) << 31);
      return xpair<uint32_t, uint32_t> { uint32_t(x / y), uint32_t(x % y) };
    }

    static constexpr TLFLOAT_INLINE xpair<uint64_t, uint64_t> divmod2(uint64_t x, uint64_t y) {
#if defined(_MSC_VER) && !defined(__clang__) // This is required to avoid ICE
      auto a = BigUInt<6>(x).divmod2(y, BigUInt<6>(y).reciprocal2());
#else
      auto a = BigUInt<6>(x).divmod2(y);
#endif
      return xpair<uint64_t, uint64_t> { a.first.u64, a.second.u64 };
    }

    template<int N>
    static constexpr TLFLOAT_INLINE xpair<BigUInt<N>, BigUInt<N>> divmod2(const BigUInt<N>& x, const BigUInt<N>& y) {
#if defined(_MSC_VER) && !defined(__clang__) // This is required to avoid ICE
      return x.divmod2(y, y.reciprocal2());
#else
      return x.divmod2(y);
#endif
    }

    //

#ifndef TLFLOAT_ENABLE_INTSQRT
    static constexpr TLFLOAT_INLINE uint32_t isqrt(uint32_t s) {
      uint64_t u = xsqrt(double(s) * (double)0x100000000ULL);
      return u - (u >> 32);
    }

    static constexpr TLFLOAT_INLINE uint64_t isqrt(uint64_t s) {
      double d = xsqrt((double)uint64_t(s) * double(1ULL << 32) * double(1ULL << 32));
      return d > 0x1.fffffffffffffp+63 ? UINT64_MAX : uint64_t(d); // nextafter((double)UINT64_MAX, 0)
    }

    static constexpr TLFLOAT_INLINE uint64_t isqrt(BigUInt<6> s) {
      double d = xsqrt((double)uint64_t(s) * double(1ULL << 32) * double(1ULL << 32));
      return d > 0x1.fffffffffffffp+63 ? UINT64_MAX : uint64_t(d);
    }

    template<int N>
    static constexpr TLFLOAT_INLINE BigUInt<N> isqrt(BigUInt<N> s) {
      if (!std::is_constant_evaluated()) assert(s >= (BigUInt<N>(1) << (sizeof(BigUInt<N>) * 8 - 2)));
      BigUInt<N> x = BigUInt<N>(isqrt(s.high) | 1, 0);
      x = (x >> 1) + s.mulhiAprx(x.reciprocalAprx());
      return x + ((~x >> (sizeof(x)*8 - 1)) & 1);
    }
#else // #ifndef TLFLOAT_ENABLE_INTSQRT
    static constexpr uint8_t rsqrttab[256] = {
      0x80, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff,
      0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff,
      0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff,
      0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff,
      0xff, 0xfe, 0xfc, 0xfa, 0xf8, 0xf7, 0xf5, 0xf3, 0xf1, 0xf0, 0xee, 0xec, 0xeb, 0xe9, 0xe8, 0xe6,
      0xe5, 0xe4, 0xe2, 0xe1, 0xdf, 0xde, 0xdd, 0xdc, 0xda, 0xd9, 0xd8, 0xd7, 0xd6, 0xd4, 0xd3, 0xd2,
      0xd1, 0xd0, 0xcf, 0xce, 0xcd, 0xcc, 0xcb, 0xca, 0xc9, 0xc8, 0xc7, 0xc6, 0xc5, 0xc4, 0xc3, 0xc2,
      0xc2, 0xc1, 0xc0, 0xbf, 0xbe, 0xbd, 0xbd, 0xbc, 0xbb, 0xba, 0xb9, 0xb9, 0xb8, 0xb7, 0xb6, 0xb6,
      0xb5, 0xb4, 0xb4, 0xb3, 0xb2, 0xb2, 0xb1, 0xb0, 0xb0, 0xaf, 0xae, 0xae, 0xad, 0xac, 0xac, 0xab,
      0xab, 0xaa, 0xa9, 0xa9, 0xa8, 0xa8, 0xa7, 0xa7, 0xa6, 0xa6, 0xa5, 0xa4, 0xa4, 0xa3, 0xa3, 0xa2,
      0xa2, 0xa1, 0xa1, 0xa0, 0xa0, 0x9f, 0x9f, 0x9e, 0x9e, 0x9e, 0x9d, 0x9d, 0x9c, 0x9c, 0x9b, 0x9b,
      0x9a, 0x9a, 0x9a, 0x99, 0x99, 0x98, 0x98, 0x97, 0x97, 0x97, 0x96, 0x96, 0x95, 0x95, 0x95, 0x94,
      0x94, 0x93, 0x93, 0x93, 0x92, 0x92, 0x92, 0x91, 0x91, 0x90, 0x90, 0x90, 0x8f, 0x8f, 0x8f, 0x8e,
      0x8e, 0x8e, 0x8d, 0x8d, 0x8d, 0x8c, 0x8c, 0x8c, 0x8b, 0x8b, 0x8b, 0x8a, 0x8a, 0x8a, 0x89, 0x89,
      0x89, 0x89, 0x88, 0x88, 0x88, 0x87, 0x87, 0x87, 0x86, 0x86, 0x86, 0x86, 0x85, 0x85, 0x85, 0x84,
      0x84, 0x84, 0x84, 0x83, 0x83, 0x83, 0x83, 0x82, 0x82, 0x82, 0x82, 0x81, 0x81, 0x81, 0x81, 0x80,
    };

    static constexpr TLFLOAT_INLINE uint8_t irsqrt(uint8_t s) {
      return rsqrttab[s];
    }

    static constexpr TLFLOAT_INLINE uint16_t irsqrt(uint16_t s) {
      if (!std::is_constant_evaluated()) assert(s >= (1U << 14));
      uint8_t t = (s + (1 << 7)) >> 8;
      t = irsqrt(uint8_t(t));
      uint32_t u = (t * (0xc000ULL - ((uint16_t(t) * t * uint32_t(s) + (1U << 15)) >> 16)) + (1U << 6)) >> 7;
      return u > 0xffffU ? 0xffffU : u;
    }

    static constexpr TLFLOAT_INLINE uint32_t irsqrt(uint32_t s) {
      if (!std::is_constant_evaluated()) assert(s >= (1ULL << 30));
      uint16_t t = (s + (1 << 15)) >> 16;
      t = irsqrt(uint16_t(t - (t == 0)));
      uint64_t u = (t * (0xc0000000ULL - ((uint32_t(t) * t * uint64_t(s) + (1U << 31)) >> 32)) + (1U << 14)) >> 15;
      return u > 0xffffffffU ? 0xffffffffU : u;
    }

    static constexpr TLFLOAT_INLINE uint64_t irsqrt(uint64_t s) {
      if (!std::is_constant_evaluated()) assert(s >= (1ULL << 62));
      uint32_t t = (s + (1U << 31)) >> 32;
      t = irsqrt(uint32_t(t - (t == 0)));
      BigUInt<7> u = (t * (0xc000000000000000ULL - ((uint64_t(t) * t * BigUInt<7>(s) + (1ULL << 63)) >> 64)) + (1U << 30)) >> 31;
      return u > 0xffffffffffffffffULL ? 0xffffffffffffffffULL : u.low.u64;
    }

    template<int N>
    static constexpr TLFLOAT_INLINE BigUInt<N> irsqrt(BigUInt<N> s) {
      if (!std::is_constant_evaluated()) assert(s >= (BigUInt<N>(1) << ((1 << N)-2)));
      BigUInt<N-1> t = (s + (BigUInt<N>(1) << ((1 << (N-1))-1))).high;
      t = irsqrt(BigUInt<N-1>(t - (t == 0)));
      BigUInt<N+1> u = (t * ((BigUInt<N>(0xc) << ((1 << N)-4)) - ((BigUInt<N+1>::mul(BigUInt<N>::mul(t, t), s) + (BigUInt<N>(1) << ((1 << N)-1))) >> (1 << N))) + (BigUInt<N>(1) << ((1 << (N-1))-2))) >> ((1 << (N-1))-1);
      return u.high.isZero() ? u.low : ~BigUInt<N>(0U);
    }

    template<> constexpr TLFLOAT_INLINE BigUInt<6> irsqrt(BigUInt<6> s) { return irsqrt(s.u64); }

    static constexpr TLFLOAT_INLINE uint32_t isqrt(uint32_t s) {
      if (s == 0) return 0;
      if (!std::is_constant_evaluated()) assert(s >= (1U << 30));
      uint64_t u = (uint64_t(s) * irsqrt(s)) >> 31;
      return u > 0xffffffffU ? 0xffffffffU : u;
    }

    static constexpr TLFLOAT_INLINE uint64_t isqrt(uint64_t s) {
      if (s == 0) return 0;
      if (!std::is_constant_evaluated()) assert(s >= (1ULL << 62));
      __uint128_t u = (__uint128_t(s) * irsqrt(s)) >> 63;
      return u > 0xffffffffffffffffULL ? 0xffffffffffffffffULL : u;
    }

    static constexpr TLFLOAT_INLINE uint64_t isqrt(BigUInt<6> s) { return isqrt(s.u64); }

    template<int N>
    static constexpr TLFLOAT_INLINE BigUInt<N> isqrt(BigUInt<N> s) {
      if (!std::is_constant_evaluated()) assert(s >= (BigUInt<N>(1) << (sizeof(BigUInt<N>) * 8 - 2)));
      BigUInt<N+1> u = s.mulhiAprx(irsqrt(s)) << 1;
      return u.high.isZero() ? u.low : ~BigUInt<N>(0U);
    }
#endif // #ifndef TLFLOAT_ENABLE_INTSQRT

    template<typename mant_t, typename longmant_t, int nbexp, int nbmant>
    class UnpackedFloat {
      template<typename, typename, int, int> friend class UnpackedFloat;

      static_assert(sizeof(longmant_t) == 2 * sizeof(mant_t));

      static constexpr size_t sizeof_mant_t() { return sizeof(mant_t); }

      template<typename floattype>
      static constexpr TLFLOAT_INLINE mant_t mantBits(const floattype& fl) {
	return toBits(fl) & ((mant_t(1) << nbmant) - 1);
      }

      template<typename floattype>
      static constexpr TLFLOAT_INLINE mant_t expBits(const floattype& fl) {
	return (toBits(fl) >> nbmant) & ((mant_t(1) << nbexp) - 1);
      }

      template<typename floattype>
      static constexpr TLFLOAT_INLINE mant_t toBits(const floattype& f) {
	static_assert(sizeof(floattype) == sizeof(mant_t));
	return std::bit_cast<mant_t>(f);
      }

      template<typename floattype>
      static constexpr TLFLOAT_INLINE floattype fromBits(const mant_t& u) {
	static_assert(sizeof(floattype) == sizeof(mant_t));
	return std::bit_cast<floattype>(u);
      }

      //

    public:
      int32_t exp;
      mant_t mant;
      bool sign, iszero, isinf, isnan;

      static constexpr TLFLOAT_INLINE int expoffset() {
	if constexpr (nbexp != 0) return (1 << (nbexp - 1)) - 1;
	return 0;
      }
      static constexpr TLFLOAT_INLINE int nbexp_() { return nbexp; }
      static constexpr TLFLOAT_INLINE int nbmant_() { return nbmant; }
      static constexpr TLFLOAT_INLINE longmant_t longmant_t_() { return 0; }
      static constexpr TLFLOAT_INLINE UnpackedFloat<mant_t, longmant_t, 0, sizeof(mant_t)*8-2> xUnpackedFloat() {
	return UnpackedFloat<mant_t, longmant_t, 0, sizeof(mant_t)*8-2>(0, 0, false, true, false, false);
      }

      constexpr TLFLOAT_INLINE int32_t ilogb() const {
	if (exp != 0) return exp - expoffset() + 1;
	return (sizeof(mant_t) * 8 - nbmant_()) - clz(mant) - expoffset();
      }

      friend constexpr TLFLOAT_INLINE UnpackedFloat fabs(UnpackedFloat x) { x.sign = 0; return x; }

      friend constexpr TLFLOAT_INLINE UnpackedFloat ldexp_(UnpackedFloat x, const int e) {
#if !(defined(__GNUC__) && !defined(__clang__))
	static_assert(x.nbexp_() == 0);
#endif
	if (!x.iszero) {
	  int64_t y = x.exp + (int64_t)e;
	  if (y < INT32_MIN) return zero(x.sign);
	  if (y > INT32_MAX) return infinity(x.sign);
	  x.exp = y;
	}
	return x;
      }

      friend constexpr TLFLOAT_INLINE UnpackedFloat ldexp(UnpackedFloat x, const int e) {
	return ldexp_(x.cast((decltype(UnpackedFloat::xUnpackedFloat())*) 0), e).cast((UnpackedFloat*) 0);
      }

      friend constexpr TLFLOAT_INLINE xpair<UnpackedFloat, int> frexp_(UnpackedFloat x) {
	static_assert(x.nbexp_() == 0);
	xpair<UnpackedFloat, int> ret = { x, x.exp - expoffset() + 2 };
	ret.first.exp = expoffset() - 2;
	return ret;
      }

      friend constexpr TLFLOAT_INLINE int cmp(const UnpackedFloat &lhs, const UnpackedFloat &rhs) {
	if (lhs.iszero && rhs.iszero) return 0;
	if (rhs.iszero) return lhs.sign ? -1 :  1;
	if (lhs.iszero) return rhs.sign ?  1 : -1;
	if (!lhs.sign &&  rhs.sign) return +1;
	if ( lhs.sign && !rhs.sign) return -1;
	if (lhs.exp  > rhs.exp) return lhs.sign ? -1 :  1;
	if (lhs.exp  < rhs.exp) return rhs.sign ?  1 : -1;
	if (lhs.mant > rhs.mant) return lhs.sign ? -1 :  1;
	if (lhs.mant < rhs.mant) return lhs.sign ?  1 : -1;
	return 0;
      }

      static constexpr TLFLOAT_INLINE unsigned clz(uint64_t u) {
	return u == 0 ? 64 : clz64(u);
      }

#ifdef TLFLOAT_ENABLE_GNUC_CLZ
      static constexpr TLFLOAT_INLINE unsigned clz(uint32_t u) {
	return u == 0 ? 32 : __builtin_clz(u);
      }
#else
      static constexpr TLFLOAT_INLINE unsigned clz(uint32_t u) {
	return clz(uint64_t(u)) - 32;
      }
#endif

      static constexpr TLFLOAT_INLINE unsigned clz(uint16_t u) {
	return clz(uint32_t(u)) - 16;
      }

      template<int N>
      static constexpr TLFLOAT_INLINE unsigned clz(const BigUInt<N>& u) {
	return u.clz();
      }

      //

      static constexpr TLFLOAT_INLINE bool bit(const uint64_t x, unsigned b) { return (x >> b) & 1; }

      template<int N>
      static constexpr TLFLOAT_INLINE bool bit(const BigUInt<N>& x, unsigned b) { return x.bit(b); }

      //

      friend constexpr double to_double(const UnpackedFloat &uf) {
	typedef UnpackedFloat<uint64_t, BigUInt<7>, 11, 52> udouble;
	return (double)uf.cast((udouble *)nullptr);
      }

      static constexpr TLFLOAT_INLINE UnpackedFloat nan() {
	return UnpackedFloat((mant_t(3) << (nbmant - 1)), (1 << nbexp) - 2, false, false, false, true);
      }

      static constexpr TLFLOAT_INLINE UnpackedFloat infinity(bool sign = false) {
	return UnpackedFloat(mant_t(1) << nbmant, (1 << nbexp) - 2, sign, false, true, false);
      }

      static constexpr TLFLOAT_INLINE UnpackedFloat flt_true_min(bool sign = false) {
	return UnpackedFloat(mant_t(1), 0, sign, false, false, false);
      }

      static constexpr TLFLOAT_INLINE UnpackedFloat flt_min(bool sign = false) {
	return UnpackedFloat(mant_t(1) << nbmant, 0, sign, false, false, false);
      }

      static constexpr TLFLOAT_INLINE UnpackedFloat flt_max(bool sign = false) {
	return UnpackedFloat((mant_t(2) << nbmant) - 1, (1 << nbexp) - 3, sign, false, false, false);
      }

      static constexpr TLFLOAT_INLINE UnpackedFloat zero(bool sign = false) {
	return UnpackedFloat(0, 0, sign, true, false, false);
      }

      static constexpr TLFLOAT_INLINE UnpackedFloat faddsub(const UnpackedFloat &lhs, const UnpackedFloat &rhs, bool negateRhs) {
	const bool rhssign = rhs.sign != negateRhs;
	const int ed = lhs.exp - rhs.exp;

	if (lhs.iszero | lhs.isinf | lhs.isnan | rhs.iszero | rhs.isinf | rhs.isnan | (ed > +nbmant+2) | (ed < -nbmant-2)) {
	  if (lhs.iszero && rhs.iszero)
	    return UnpackedFloat(0, 0, lhs.sign && rhssign, true, false, false);

	  if (lhs.isnan || rhs.iszero) return lhs;
	  if (rhs.isnan || lhs.iszero)
	    return UnpackedFloat(rhs.mant, rhs.exp, rhssign, rhs.iszero, rhs.isinf, rhs.isnan);

	  if (lhs.isinf || rhs.isinf) {
	    if (lhs.isinf && rhs.isinf && lhs.sign == rhssign) return lhs;
	    if (lhs.isinf && rhs.isinf && lhs.sign != rhssign) return nan();
	    if (lhs.isinf) return lhs;
	    return UnpackedFloat(rhs.mant, rhs.exp, rhssign, rhs.iszero, rhs.isinf, rhs.isnan);
	  }

	  if (ed > +nbmant+2) return lhs;
	  if (ed < -nbmant-2)
	    return UnpackedFloat(rhs.mant, rhs.exp, rhssign, rhs.iszero, rhs.isinf, rhs.isnan);
	}

	const bool subtract = lhs.sign != rhssign;

	longmant_t lm, rm;
	if (ed > 0) {
	  lm = longmant_t(lhs.mant) << (sizeof(mant_t) * 8     );
	  rm = longmant_t(rhs.mant) << (sizeof(mant_t) * 8 - ed);
	} else {
	  lm = longmant_t(lhs.mant) << (sizeof(mant_t) * 8 + ed);
	  rm = longmant_t(rhs.mant) << (sizeof(mant_t) * 8     );
	}

	int64_t exp = ed > 0 ? lhs.exp : rhs.exp;

	longmant_t am = 0;
	bool resultsign = lhs.sign;

	if (!subtract) {
	  am = lm + rm;
	  if (bit(am, (sizeof(mant_t) * 8) + nbmant + 1)) { am >>= 1; exp++; }
	  am += bit(am, sizeof(mant_t) * 8) + ((longmant_t(1) << (sizeof(mant_t) * 8 - 1)) - 1);
	  if (bit(am, (sizeof(mant_t) * 8) + nbmant + 1)) { am >>= 1; exp++; }
	  if constexpr (nbexp != 0) {
	    if (exp >= (1 << nbexp) - 2) {
	      exp = (1 << nbexp) - 2;
	      am = longmant_t(1) << (nbmant + sizeof(mant_t) * 8);
	    }
	  }
	} else {
	  am = lm < rm ? (rm - lm) : (lm - rm);
	  resultsign = lm < rm ? rhssign : lhs.sign;
	  if (am == 0) resultsign = false;

	  const int x = clz(am) - (sizeof(mant_t) * 8 - nbmant - 1);
	  am <<= x;
	  exp -= x;

	  int y = 0;
	  if constexpr (nbexp != 0) {
	    if (exp < 0) {
	      if (exp <= -(int)sizeof(longmant_t) * 8) am = 0;
	      y = -exp;
	      exp = 0;
	    }
	  }
	  am += bitmask<longmant_t>(sizeof(mant_t) * 8 + y - 1) + bit(am, sizeof(mant_t) * 8 + y);
	  am >>= y;

	  if (bit(am, (sizeof(mant_t) * 8) + nbmant + 1)) { am >>= 1; exp++; }
	}

	am >>= sizeof(mant_t) * 8;
	if (am == 0) exp = 0;
	if constexpr (nbexp == 0) {
	  if (exp < INT32_MIN) return zero(resultsign);
	  if (exp > INT32_MAX) return infinity(resultsign);
	}
	return UnpackedFloat(mant_t(am), exp, resultsign, am == 0, nbexp != 0 && exp == (1 << nbexp) - 2, false);
      }

      static constexpr TLFLOAT_INLINE UnpackedFloat fmul(const UnpackedFloat &lhs, const UnpackedFloat &rhs) {
	if (int(lhs.isnan) | int(rhs.isnan) | (int(lhs.iszero) & int(rhs.isinf)) |
	    (int(lhs.isinf) & int(rhs.iszero)) | int(lhs.isinf) | int(rhs.isinf)) {
	  if (lhs.isnan) return lhs;
	  if (rhs.isnan) return rhs;
	  if ((lhs.iszero && rhs.isinf) || (lhs.isinf && rhs.iszero)) return nan();
	  if (lhs.isinf || rhs.isinf) return infinity(lhs.sign != rhs.sign);
	}

	longmant_t am = mul(lhs.mant, rhs.mant);

	const int x = (int)clz(am) - ((int)sizeof(mant_t)*8 - nbmant) * 2;
	am = am << (x + sizeof(mant_t)*8 - nbmant + 1);

	int64_t exp = (int64_t)lhs.exp + rhs.exp - expoffset() - x;

	int y = 0;
	if constexpr (nbexp != 0) {
	  if (exp < 0) {
	    if (exp <= -(int)sizeof(longmant_t) * 8) am = 0;
	    y = -exp;
	    exp = 0;
	  }
	}
	am += bitmask<longmant_t>(sizeof(mant_t) * 8 + y - 1) + bit(am, sizeof(mant_t) * 8 + y);
	am >>= y;

	if (bit(am, (sizeof(mant_t) * 8) + nbmant + 1)) { am >>= 1; exp++; }
	if constexpr (nbexp != 0) {
	  if (exp >= (1 << nbexp) - 2) {
	    exp = (1 << nbexp) - 2;
	    am = longmant_t(1) << (nbmant + sizeof(mant_t) * 8);
	  }
	}
	am >>= sizeof(mant_t) * 8;
	if (am == 0) exp = 0;
	if constexpr (nbexp == 0) {
	  if (exp < INT32_MIN) return zero(lhs.sign != rhs.sign);
	  if (exp > INT32_MAX) return infinity(lhs.sign != rhs.sign);
	}
	return UnpackedFloat(mant_t(am), exp, lhs.sign != rhs.sign, am == 0, nbexp != 0 && exp == (1 << nbexp) - 2, false);
      }

      static constexpr TLFLOAT_INLINE UnpackedFloat fdiv(const UnpackedFloat &lhs, const UnpackedFloat &rhs) {
	if (lhs.isnan | lhs.isinf | rhs.isnan | rhs.iszero | rhs.isinf) {
	  if (lhs.isnan) return lhs;
	  if (rhs.isnan) return rhs;

	  if (rhs.iszero) {
	    if (lhs.iszero) return nan();
	    return infinity(lhs.sign != rhs.sign);
	  }
	  if (rhs.isinf) {
	    if (lhs.isinf) return nan();
	    return zero(lhs.sign != rhs.sign);
	  }
	  if (lhs.isinf) return infinity(lhs.sign != rhs.sign);
	}

	int64_t exp = (int64_t)lhs.exp - rhs.exp + expoffset() - 1;
	int sl = clz(lhs.mant);
	int sr = clz(rhs.mant);
	exp -= sl + 1 - sr;

	auto p = divmod2(mant_t(lhs.mant << sl), mant_t(rhs.mant << sr));
	longmant_t am = longmant_t(p.first) << (nbmant + 2);

	int c = icmp(p.second, rhs.mant << (sr - 1));
	c = c >= 0 ? c + 2 : p.second != 0 ? 1 : 0;
	am += longmant_t(c) << nbmant;

	const int x = clz(am) - nbexp;
	am = x >= 0 ? (am << (x & (sizeof(am)*8-1))) : (am >> -x);
	exp -= x;

	int y = 0;
	if constexpr (nbexp != 0) {
	  if (exp < 0) {
	    if (exp <= -(int)sizeof(longmant_t) * 8) am = 0;
	    y = -exp;
	    exp = 0;
	  }
	}
	am += bitmask<longmant_t>(sizeof(mant_t) * 8 + y - 1) + bit(am, sizeof(mant_t) * 8 + y);
	am >>= y;

	if (bit(am, (sizeof(mant_t) * 8) + nbmant + 1)) { am >>= 1; exp++; }
	if constexpr (nbexp != 0) {
	  if (exp >= (1 << nbexp) - 2) {
	    exp = (1 << nbexp) - 2;
	    am = longmant_t(1) << (nbmant + sizeof(mant_t) * 8);
	  }
	}
	am >>= sizeof(mant_t) * 8;
	if (am == 0) exp = 0;
	if constexpr (nbexp == 0) {
	  if (exp < INT32_MIN) return zero(lhs.sign != rhs.sign);
	  if (exp > INT32_MAX) return infinity(lhs.sign != rhs.sign);
	}
	return UnpackedFloat(mant_t(am), exp, lhs.sign != rhs.sign, am == 0, nbexp != 0 && exp == (1 << nbexp) - 2, false);
      }

      static constexpr UnpackedFloat fma(const UnpackedFloat &lhs, const UnpackedFloat &rhs, const UnpackedFloat &zhs) {
	if (lhs.isnan | lhs.isinf | rhs.isnan | rhs.isinf | zhs.isnan | zhs.isinf) {
	  if (lhs.isnan) return lhs;
	  if (rhs.isnan) return rhs;
	  if (zhs.isnan) return zhs;

	  if (lhs.isinf || rhs.isinf || zhs.isinf) {
	    if ((lhs.iszero && rhs.isinf) || (lhs.isinf && rhs.iszero)) return nan();
	    if (!zhs.isinf) return infinity(lhs.sign != rhs.sign);
	    if (!(lhs.isinf || rhs.isinf) || ((lhs.sign != rhs.sign) == zhs.sign)) return infinity(zhs.sign);
	    return nan();
	  }
	}

	int64_t exp = (int64_t)lhs.exp + rhs.exp - expoffset();
	longmant_t am = mul(lhs.mant, rhs.mant);
	const int x = (int)clz(am) - ((int)sizeof(mant_t)*8 - nbmant) * 2;
	am = am << (x + sizeof(mant_t)*8 - nbmant + 1);
	exp -= x;
	if (am == 0) exp = -expoffset();

	int ed = exp - zhs.exp;
	if constexpr (nbexp == 0) {
	  if (am == 0 && zhs.iszero) {
	    ed = 0;
	  } else if (am == 0) {
	    ed = -(int)sizeof(longmant_t) * 8;
	  } else if (zhs.iszero) {
	    ed = +(int)sizeof(longmant_t) * 8;
	  }
	}
	const bool subtract = (lhs.sign != rhs.sign) != zhs.sign;
	bool resultsign = lhs.sign != rhs.sign;

	const int NGB = 4;
	bool amsb = false, zmsb = false;
	longmant_t zm = longmant_t(zhs.mant) << (sizeof(mant_t) * 8);
	if (ed < 0) {
	  if (ed <= -(int)sizeof(longmant_t) * 8 + NGB) {
	    amsb = am != 0;
	    am = 0;
	  } else {
	    amsb = (bitmask<longmant_t>(NGB - ed) & am) != 0;
	    am = (am >> -ed) & ~((longmant_t(1) << NGB) - 1);
	  }
	} else {
	  if (ed >= (int)sizeof(longmant_t) * 8 - NGB) {
	    zmsb = zm != 0;
	    zm = 0;
	  } else {
	    zmsb = (bitmask<longmant_t>(NGB + ed) & zm) != 0;
	    zm = (zm >> ed) & ~((longmant_t(1) << NGB) - 1);
	  }
	}
	exp = ed > 0 ? exp : zhs.exp;

	if (!subtract) {
	  am += zm;
	  if (bit(am, (sizeof(mant_t) * 8) + nbmant + 1)) { am >>= 1; exp++; }
	  am += ((longmant_t(1) << (sizeof(mant_t) * 8 - 1)) - 1) + (amsb || zmsb || bit(am, sizeof(mant_t) * 8));
	} else {
	  bool psb = am >= zm ? amsb : zmsb;
	  bool nsb = am >= zm ? zmsb : amsb;
	  resultsign = am >= zm ? resultsign : zhs.sign;
	  am = am >= zm ? (am - zm) : (zm - am);
	  if (am == 0 && !psb) resultsign = 0;
	  const int x = clz(am) - (sizeof(mant_t) * 8 - nbmant - 1);
	  am <<= x;
	  exp -= x;
	  const int y = (nbexp != 0 && exp < 0) ? -exp : 0;
	  am += bitmask<longmant_t>(sizeof(mant_t) * 8 + y - 1) + (!nsb && (psb || bit(am, sizeof(mant_t) * 8 + y)));
	  am >>= y;
	}

	if (bit(am, (sizeof(mant_t) * 8) + nbmant + 1)) { am >>= 1; exp++; }
	if constexpr (nbexp != 0) {
	  if (exp < 0) exp = 0;
	  if (exp >= (1 << nbexp) - 2) {
	    exp = (1 << nbexp) - 2;
	    am = longmant_t(1) << (nbmant + sizeof(mant_t) * 8);
	  }
	}
	am >>= sizeof(mant_t) * 8;
	if (am == 0) exp = 0;
	if constexpr (nbexp == 0) {
	  if (exp < INT32_MIN) return zero(resultsign);
	  if (exp > INT32_MAX) return infinity(resultsign);
	}
	return UnpackedFloat(mant_t(am), exp, resultsign, am == 0, nbexp != 0 && exp == (1 << nbexp) - 2, false);
      }

      //

      constexpr TLFLOAT_INLINE UnpackedFloat() = default;
      constexpr TLFLOAT_INLINE UnpackedFloat(const UnpackedFloat&) = default;
      constexpr TLFLOAT_INLINE UnpackedFloat& operator=(const UnpackedFloat&) = default;

      constexpr TLFLOAT_INLINE UnpackedFloat(const mant_t mant_, int32_t exp_, bool sign_, bool iszero_, bool isinf_, bool isnan_) :
	exp(exp_), mant(mant_), sign(sign_), iszero(iszero_), isinf(isinf_), isnan(isnan_) {
      }

      explicit constexpr TLFLOAT_INLINE UnpackedFloat(const int i) { *this = castFromInt(i); }

      static constexpr uint8_t hexchartab[256] = {
	0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff,
	0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff,
	0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff,
	0x00, 0x01, 0x02, 0x03, 0x04, 0x05, 0x06, 0x07, 0x08, 0x09, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff,
	0xff, 0x0a, 0x0b, 0x0c, 0x0d, 0x0e, 0x0f, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff,
	0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff,
	0xff, 0x0a, 0x0b, 0x0c, 0x0d, 0x0e, 0x0f, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff,
	0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff,
	0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff,
	0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff,
	0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff,
	0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff,
	0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff,
	0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff,
	0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff,
	0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff,
      };

      constexpr UnpackedFloat(const char *ptr, const char **endptr=nullptr) {
	while(xisspace(*ptr)) ptr++;

	bool positive = true;
	if (*ptr == '-') {
	  positive = false; ptr++;
	} else if (*ptr == '+') {
	  ptr++;
	}

	if (xstrncasecmp(ptr, "nan", 3) == 0) { *this = nan();               if (endptr) { *endptr = ptr + 3; } return; }
	if (xstrncasecmp(ptr, "inf", 3) == 0) { *this = infinity(!positive); if (endptr) { *endptr = ptr + 3; } return; }

	if (*ptr == '0' && (*(ptr+1) == 'x' || *(ptr+1) == 'X')) {
	  ptr += 2;
	  longmant_t m = 0;
	  exp = 0;
	  bool bp = false;
	  int dr = sizeof(mant_t)*2, wd = 0;
	  uint64_t w = 0;

	  while(*ptr != '\0' && dr >= 0) {
	    int d = hexchartab[(uint8_t)*ptr];
	    if (d != 0xff) {
	      ptr++;
	    } else if (*ptr == '.') {
	      if (bp) break;
	      bp = true;
	      ptr++;
	      continue;
	    } else break;
	    w = (w << 4) + d;
	    if (w != 0 || m != 0) dr--;
	    wd++;
	    if (w >= (1ULL << 60)) {
	      if (m != 0) m <<= wd * 4;
	      m |= w;
	      w = 0;
	      wd = 0;
	    }
	    if (bp) exp -= 4;
	  }

	  if (m != 0) m <<= wd * 4;
	  m |= w;

	  while(hexchartab[(uint8_t)*ptr] != 0xff) { ptr++; if (!bp) exp += 4; }
	  if (*ptr == '.' && !bp) ptr++;
	  while(hexchartab[(uint8_t)*ptr] != 0xff) ptr++;

	  if (*ptr == 'p' || *ptr == 'P') {
	    const char *q;
	    long long z = xstrtoll(ptr+1, &q);
	    if (!(ptr+1 == q || xisspace((int)*(ptr+1)))) {
	      exp += z;
	      ptr = q;
	    }
	  }

	  const int x = (int)clz(m) - ((int)sizeof(m) * 8 - nbmant - 1);
	  exp += nbmant - x - 1;
	  if (x >= 0) {
	    mant = x < (int)sizeof(mant_t)*8 ? mant_t(m << x) : 0;
	  } else {
	    mant = mant_t((m + (longmant_t(1) << (-x-1))) >> -x);
	    if (bit(mant, nbmant + 1)) { mant >>= 1; exp++; }
	  }

	  sign = !positive;
	  isinf = isnan = iszero = false;

	  if (mant == 0) {
	    iszero = true;
	    exp = 0;
	  }

	  if (endptr) *endptr = ptr;

	  return;
	} 

	bool bp = false;
	UnpackedFloat n = castFromInt(0);
	int e = 0;

	for(unsigned i=0;;i++) {
	  char c = ptr[i];
	  if (c == '\0') {
	    ptr += i;
	    break;
	  }
	  if (c == '.' && !bp) { bp = true; continue; }
	  if ('0' <= c && c <= '9') {
	    if (n.exp > expoffset() + nbmant) {
	      if (!bp) e++;
	      continue;
	    }
	    n = fma(n, castFromInt(10), castFromInt(c - '0'));
	    if (bp) e--;
	    continue;
	  }
	  if (c == 'e' || c == 'E') {
	    e += xstrtoll(&ptr[i+1], &ptr);
	    break;
	  }
	}

	n *= UnpackedFloat::exp10i(e);
	if (!positive) n = -n;
	*this = n;

	if (endptr) *endptr = ptr;
      }

      template<typename floattype, std::enable_if_t<!std::is_pointer_v<floattype>, int> = 0>
      constexpr TLFLOAT_INLINE UnpackedFloat(const floattype &fl) :
	exp(int32_t(expBits(fl) - (expBits(fl) != 0))),
	mant(mantBits(fl) | (mant_t(expBits(fl) != 0) << nbmant)),
	sign(toBits(fl) >> (sizeof(fl) * 8 - 1)),
	iszero((toBits(fl) & ((mant_t(1) << (sizeof(mant_t)*8-1))-1)) == 0),
	isinf((exp == ((1 << nbexp) - 2)) && mant == (mant_t(1) << nbmant)),
	isnan((exp == ((1 << nbexp) - 2)) && mant != (mant_t(1) << nbmant)) {
	static_assert(sizeof(floattype) == sizeof(mant_t));
      }

      template<typename floattype>
      explicit constexpr TLFLOAT_INLINE operator floattype() const {
	static_assert(sizeof(floattype) == sizeof(mant_t));
	mant_t m = mant & ((mant_t(1) << nbmant) - 1);
	m |= mant_t(sign) << (sizeof(mant_t) * 8 - 1);
	m |= mant_t(exp + ((mant >> nbmant) != 0)) << nbmant;
	return fromBits<floattype>(m);
      }

      template<typename dsttype>
      constexpr TLFLOAT_INLINE dsttype cast(const dsttype *ptr) const {
	typedef decltype(dsttype::mant) dmant_t;
	typedef decltype(dsttype::longmant_t_()) dlongmant_t;
	if (isnan) return dsttype::nan();
	if constexpr (dsttype::nbmant_() > nbmant && (dsttype::sizeof_mant_t() > sizeof_mant_t() ||
						      nbexp == 0 || dsttype::nbexp_() == 0)) {
	  int64_t e = (int64_t)exp - expoffset() + dsttype::expoffset();
	  const int x = clz(mant) - (sizeof(mant_t) * 8 - nbmant - 1);
	  e -= x;
	  int s = dsttype::nbmant_() - nbmant + x;
	  if constexpr (dsttype::nbexp_() != 0) { if (e < 0) { s += e; e = 0; } }
	  dmant_t m = s < int(sizeof(dmant_t)*8) ? dmant_t(mant) << s : 0;
	  if (isinf || isnan) e = (1 << dsttype::nbexp_()) - 2;
	  if (iszero) e = 0;
	  return dsttype(m, (int)e, sign, iszero, isinf, isnan);
	} else if constexpr (dsttype::nbmant_() < nbmant && (dsttype::sizeof_mant_t() < sizeof_mant_t() ||
							     nbexp == 0 || dsttype::nbexp_() == 0)) {
	  bool isinf_ = isinf, sb = false;
	  int64_t e = (int64_t)exp - expoffset() + dsttype::expoffset();
	  mant_t m = mant;
	  if constexpr (dsttype::nbexp_() != 0) {
	    if (e >= (1 << dsttype::nbexp_()) - 2) isinf_ = true;
	    if (e < 0) {
	      sb = (m & bitmask<mant_t>(-e)) != 0;
	      if (unsigned(-e) >= sizeof(mant_t)*8) { sb = m != 0; m = 0; }
	      m >>= -e;
	      e = 0;
	    }
	  }
	  m += bit(m, nbmant - dsttype::nbmant_()) || sb;
	  m += (mant_t(1) << ((nbmant - dsttype::nbmant_()) - 1)) - 1;
	  if (bit(m, nbmant + 1)) { m >>= 1; e++; }
	  m >>= nbmant - dsttype::nbmant_();
	  if (isinf_ || isnan) e = (1 << dsttype::nbexp_()) - 2;
	  if (isinf_) m = mant_t(1) << dsttype::nbmant_();
	  if (isnan) m = mant_t(3) << (dsttype::nbmant_() - 1);
	  bool iszero_ = iszero;
	  if (dmant_t(m) == 0) iszero_ = true;
	  if (iszero_) e = 0;
	  return dsttype(dmant_t(m), (int)e, sign, iszero_, isinf_, isnan);
	} else if constexpr (dsttype::nbmant_() < nbmant && dsttype::sizeof_mant_t() == sizeof_mant_t()) {
	  // BFloat16 <- Half
	  int64_t e = (int64_t)exp - expoffset() + dsttype::expoffset();
	  const int x = clz(mant) - (sizeof(mant_t) * 8 - nbmant - 1);
	  e -= x;
	  dlongmant_t m = dlongmant_t(mant) << (dsttype::nbmant_() - nbmant + x + (sizeof(dlongmant_t)-sizeof(dmant_t))*8);
	  if (isinf || isnan) e = (1 << dsttype::nbexp_()) - 2;
	  if (iszero) e = 0;
	  m += bitmask<dlongmant_t>(sizeof(dmant_t) * 8 - 1) + bit(m, sizeof(dmant_t) * 8);
	  if (bit(m, (sizeof(dmant_t) * 8) + dsttype::nbmant_() + 1)) { m >>= 1; e++; }
	  m >>= sizeof(dmant_t) * 8;
	  return dsttype(dmant_t(m), m == 0 ? 0 : e, sign, m == 0, isinf, isnan);
	} else if constexpr (dsttype::nbmant_() > nbmant && dsttype::sizeof_mant_t() == sizeof_mant_t()) {
	  // Half <- BFloat16
	  bool isinf_ = isinf, sb = false;
	  int64_t e = (int64_t)exp - expoffset() + dsttype::expoffset();
	  longmant_t m = longmant_t(mant) << (sizeof(mant_t) * 8);
	  if constexpr (dsttype::nbexp_() != 0) {
	    if (e >= (1 << dsttype::nbexp_()) - 2) isinf_ = true;
	    if (e < 0) {
	      sb = (m & bitmask<longmant_t>(-e)) != 0;
	      if (unsigned(-e) >= sizeof(longmant_t)*8) { sb = m != 0; m = 0; }
	      m >>= -e;
	      e = 0;
	    }
	  }
	  m += bit(m, (nbmant + sizeof(mant_t)*8) - dsttype::nbmant_()) || sb;
	  m += (longmant_t(1) << ((nbmant + sizeof(mant_t)*8 - dsttype::nbmant_()) - 1)) - 1;
	  if (bit(m, nbmant + sizeof(mant_t)*8 + 1)) { m >>= 1; e++; }
	  m >>= nbmant + sizeof(mant_t)*8 - dsttype::nbmant_();
	  if (isinf_ || isnan) e = (1 << dsttype::nbexp_()) - 2;
	  if (isinf_) m = longmant_t(1) << dsttype::nbmant_();
	  if (isnan) m = longmant_t(3) << (dsttype::nbmant_() - 1);
	  bool iszero_ = iszero;
	  if (dmant_t(m) == 0) iszero_ = true;
	  if (iszero_) e = 0;
	  return dsttype(dmant_t(m), (int)e, sign, iszero_, isinf_, isnan);
	} else {
	  static_assert(dsttype::nbmant_() == nbmant);
	  static_assert(dsttype::sizeof_mant_t() == sizeof_mant_t());
	  return dsttype(mant, exp, sign, iszero, isinf, isnan);
	}
      }

      template<typename dsttype, std::enable_if_t<(std::is_integral_v<dsttype> && sizeof(dsttype) <= 8), int> = 0>
      constexpr TLFLOAT_INLINE dsttype castToInt(const dsttype *ptr) const {
	if (exp - expoffset() < -1) return 0;
	if constexpr (std::is_unsigned_v<dsttype>) {
	  if (sign && !iszero) return ~dsttype(0);
	  if (exp - expoffset() >= int(sizeof(dsttype)*8-1)) return ~dsttype(0);
	  int s = nbmant - (exp - expoffset() + 1);
	  return dsttype(s > 0 ? (mant >> s) : (dsttype(mant) << -s));
	} else {
	  if (exp - expoffset() >= int(sizeof(dsttype)*8-2))
	    return dsttype(dsttype(1) << (sizeof(dsttype)*8-1));
	  int s = nbmant - (exp - expoffset() + 1);
	  dsttype r = s > 0 ? dsttype(mant >> s) : (dsttype(mant) << -s);
	  return sign ? -r : r;
	}
      }

      template<int N>
      constexpr TLFLOAT_INLINE BigInt<N> castToInt(const BigInt<N> *ptr) const {
	if (exp - expoffset() < -1) return 0;
	if (exp - expoffset() >= int(sizeof(BigInt<N>)*8-2))
	  return BigInt<N>(BigInt<N>(1) << (sizeof(BigInt<N>)*8-1));
	int s = nbmant - (exp - expoffset() + 1);
	BigInt<N> r = BigInt<N>(s > 0 ? BigUInt<N>(mant >> s) : (BigUInt<N>(mant) << -s));
	return sign ? -r : r;
      }

      template<int N>
      constexpr TLFLOAT_INLINE BigUInt<N> castToInt(const BigUInt<N> *ptr) const {
	if (exp - expoffset() < -1) return 0;
	if (sign && !iszero) return ~BigUInt<N>(0);
	if (exp - expoffset() >= int(sizeof(BigUInt<N>)*8-1)) return ~BigUInt<N>(0);
	int s = nbmant - (exp - expoffset() + 1);
	return s > 0 ? BigUInt<N>(mant >> s) : (BigUInt<N>(mant) << -s);
      }

      constexpr TLFLOAT_INLINE uint64_t castToInt2() const {
	int s = nbmant - (exp - expoffset() + 1);
	if (s <= -int(sizeof(uint64_t))*8 || s >= int(sizeof(mant_t))*8) return 0;
	return uint64_t(s > 0 ? (mant >> s) : (uint64_t(mant) << -s));
      }

      template<typename srctype, std::enable_if_t<(std::is_integral_v<srctype> && sizeof(srctype) <= 8), int> = 0>
      static constexpr TLFLOAT_INLINE UnpackedFloat castFromInt(srctype src) {
	if (src == 0) return zero();
	bool sign = false;
	uint64_t v = src;
	if constexpr (!std::is_unsigned_v<srctype>) {
	  sign = src < 0;
	  v = src < 0 ? -int64_t(src) : src;
	}
	int x = sizeof(v) * 8 - clz(v);
	int s = nbmant - (x - 1) + int(sizeof(mant_t)) * 8;
	bool sb = false;
	longmant_t u = 0;
	if (s >= 0) {
	  u = longmant_t(v) << s;
	} else {
	  sb = (bitmask<longmant_t>(-s) & v) != 0;
	  u = v >> -s;
	}
	u += (((u >> (sizeof(mant_t) * 8)) & 1) || sb) + (longmant_t(1) << (sizeof(mant_t) * 8 - 1)) - 1;
	if (bit(u, nbmant + sizeof(mant_t) * 8 + 1)) { u >>= 1; x++; }
	u >>= sizeof(mant_t) * 8;
	return UnpackedFloat(mant_t(u), x + expoffset() - 2, sign, u == 0, false, false);
      }

      template<int N>
      static constexpr TLFLOAT_INLINE UnpackedFloat castFromInt(BigInt<N> src) {
	if (src == 0) return zero();
	bool sign = src < 0;
	BigUInt<N> v = sign ? -src : src;
	int x = sizeof(v) * 8 - clz(v);
	int s = nbmant - (x - 1) + int(sizeof(mant_t)) * 8;
	bool sb = false;
	longmant_t u = 0;
	if (s >= 0) {
	  u = longmant_t(v) << s;
	} else {
	  sb = (bitmask<longmant_t>(-s) & v) != 0;
	  u = longmant_t(v >> -s);
	}
	u += (((u >> (sizeof(mant_t) * 8)) & 1) || sb) + (longmant_t(1) << (sizeof(mant_t) * 8 - 1)) - 1;
	if (bit(u, nbmant + sizeof(mant_t) * 8 + 1)) { u >>= 1; x++; }
	u >>= sizeof(mant_t) * 8;
	return UnpackedFloat(mant_t(u), x + expoffset() - 2, sign, u == 0, false, false);
      }

      template<int N>
      static constexpr TLFLOAT_INLINE UnpackedFloat castFromInt(BigUInt<N> src) {
	if (src == 0) return zero();
	BigUInt<N> v = src;
	int x = sizeof(v) * 8 - clz(v);
	int s = nbmant - (x - 1) + int(sizeof(mant_t)) * 8;
	bool sb = false;
	longmant_t u = 0;
	if (s >= 0) {
	  u = longmant_t(v) << s;
	} else {
	  sb = (bitmask<longmant_t>(-s) & v) != 0;
	  u = longmant_t(v >> -s);
	}
	u += (((u >> (sizeof(mant_t) * 8)) & 1) || sb) + (longmant_t(1) << (sizeof(mant_t) * 8 - 1)) - 1;
	if (bit(u, nbmant + sizeof(mant_t) * 8 + 1)) { u >>= 1; x++; }
	u >>= sizeof(mant_t) * 8;
	return UnpackedFloat(mant_t(u), x + expoffset() - 2, false, u == 0, false, false);
      }

#if defined(TLFLOAT_COMPILER_SUPPORTS_INT128) && !defined(__CUDA_ARCH__)
      constexpr TLFLOAT_INLINE __int128_t  castToInt(const __int128_t  *ptr) const { return __int128_t (castToInt((BigInt<7>  *)0)); }
      constexpr TLFLOAT_INLINE __uint128_t castToInt(const __uint128_t *ptr) const { return __uint128_t(castToInt((BigUInt<7> *)0)); }
      static constexpr TLFLOAT_INLINE UnpackedFloat castFromInt(__int128_t  src) { return castFromInt(BigInt <7>(src)); }
      static constexpr TLFLOAT_INLINE UnpackedFloat castFromInt(__uint128_t src) { return castFromInt(BigUInt<7>(src)); }
#endif

      friend constexpr UnpackedFloat trunc(const UnpackedFloat &f) {
	if (f.iszero || f.isinf || f.isnan) return f;
	if (f.exp >= expoffset() + nbmant) return f;
	if (f.exp < expoffset() - 1) return zero(f.sign);
	int s = nbmant - (f.exp - expoffset() + 1);
	UnpackedFloat r = f;
	r.mant &= ~bitmask<mant_t>(s);
	return r;
      }

      friend constexpr UnpackedFloat floor(const UnpackedFloat &f) {
	if (f.iszero || f.isinf || f.isnan) return f;
	if (f.exp >= expoffset() + nbmant) return f;
	if (f.exp < expoffset() - 1) {
	  if (f.iszero || !f.sign) return zero(f.sign);
	  return castFromInt(-1);
	}
	int s = nbmant - (f.exp - expoffset() + 1);
	UnpackedFloat r = f;
	if (r.sign) r.mant += bitmask<mant_t>(s);
	r.mant &= ~bitmask<mant_t>(s);
	if (bit(r.mant, nbmant + 1)) { r.mant >>= 1; r.exp++; }
	return r;
      }

      friend constexpr UnpackedFloat ceil(const UnpackedFloat &f) {
	if (f.iszero || f.isinf || f.isnan) return f;
	if (f.exp >= expoffset() + nbmant) return f;
	if (f.exp < expoffset() - 1) {
	  if (f.iszero || f.sign) return zero(f.sign);
	  return castFromInt(1);
	}
	int s = nbmant - (f.exp - expoffset() + 1);
	UnpackedFloat r = f;
	if (!r.sign) r.mant += bitmask<mant_t>(s);
	r.mant &= ~bitmask<mant_t>(s);
	if (bit(r.mant, nbmant + 1)) { r.mant >>= 1; r.exp++; }
	return r;
      }

      friend constexpr UnpackedFloat round(const UnpackedFloat &f) {
	if (f.iszero || f.isinf || f.isnan) return f;
	if (f.exp >= expoffset() + nbmant - 1) return f;
	if (f.exp < expoffset() - 2) return zero(f.sign);
	int s = nbmant - (f.exp - expoffset() + 1);
	UnpackedFloat r = f;
	r.mant += mant_t(1) << (s - 1);
	r.mant &= ~((mant_t(1) << s) - 1);
	if (bit(r.mant, nbmant + 1)) { r.mant >>= 1; r.exp++; }
	if (r.mant == 0) return zero(f.sign);
	return r;
      }

      friend constexpr UnpackedFloat rint(const UnpackedFloat &f) {
	if (f.iszero || f.isinf || f.isnan) return f;
	if (f.exp >= expoffset() + nbmant - 1) return f;
	if (f.exp < expoffset() - 2) return zero(f.sign);
	int s = nbmant - (f.exp - expoffset() + 1);
	UnpackedFloat r = f;
	r.mant += bit(r.mant, s) + (mant_t(1) << (s - 1)) - 1;
	r.mant &= ~((mant_t(1) << s) - 1);
	if (bit(r.mant, nbmant + 1)) { r.mant >>= 1; r.exp++; }
	if (r.mant == 0) return zero(f.sign);
	return r;
      }

      friend constexpr UnpackedFloat sqrt_(const UnpackedFloat &f) {
	const int offset = 100000000;
	if (f.iszero) return f;
	int y = clz(f.mant) - (sizeof(mant_t) * 8 - nbmant - 1);
	int x = f.exp - y;
	int s0 = sizeof(longmant_t) * 8 - nbmant - 2 + (1 & ~(x + f.expoffset() + offset)) + y;
	longmant_t l = longmant_t(f.mant) << s0;
	l = isqrt(l);
	int s1 = sizeof(longmant_t) * 8 - nbmant - 1 - clz(l);
	l += bit(l, s1) + bitmask<longmant_t>(s1 - 1);
	l -= ~l >> (sizeof(l) * 8 - 1);
	mant_t m = mant_t(l >> s1);
	if (bit(m, nbmant + 1)) { m >>= 1; x++; }
	return UnpackedFloat(mant_t(m), (x + f.expoffset() + offset * 2 + 1) / 2 - offset - 1, false, m == 0, false, false);
      }

      friend constexpr TLFLOAT_INLINE bool isint(const UnpackedFloat &f) {
	if (f.isinf || f.isnan) return false;
	return trunc(f) == f;
      }

      friend constexpr TLFLOAT_INLINE bool iseven(const UnpackedFloat &f) {
	return isint(ldexp(f, -1));
      }

      friend constexpr TLFLOAT_INLINE UnpackedFloat toward0(UnpackedFloat f) {
	static_assert(f.nbexp_() == 0);
	if (f.iszero || f.isinf || f.isnan) return f;
	f.mant--;
	if (!((f.mant >> nbmant) & 1)) {
	  f.exp--;
	  f.mant <<= 1;
	}
	return f;
      }

      friend constexpr int32_t intlsb(const UnpackedFloat &f) {
	if (f.iszero || f.isinf || f.isnan) return 0;
	int s = nbmant - (f.exp - expoffset() + 1);
	if (s >= int(sizeof(f.mant)*8) || s <= -int(sizeof(uint16_t)*8)) return 0;
	int m = s >= 0 ? uint16_t(f.mant >> s) : (uint16_t(f.mant) << -s);
	return f.sign ? -m-1 : m;
      }

      //

      constexpr TLFLOAT_INLINE UnpackedFloat operator+() const { return UnpackedFloat(*this); }
      constexpr TLFLOAT_INLINE UnpackedFloat operator-() const { return UnpackedFloat(mant, exp, !sign, iszero, isinf, isnan); }
      constexpr UnpackedFloat operator+(const UnpackedFloat& rhs) const { return faddsub(*this, rhs, false); }
      constexpr UnpackedFloat operator-(const UnpackedFloat& rhs) const { return faddsub(*this, rhs, true); }
      constexpr UnpackedFloat operator*(const UnpackedFloat& rhs) const { return fmul(*this, rhs); }
      constexpr UnpackedFloat operator/(const UnpackedFloat& rhs) const { return fdiv(*this, rhs); }

      constexpr TLFLOAT_INLINE bool operator==(const UnpackedFloat& rhs) const {
	if (isnan || rhs.isnan) return false;
	return cmp(*this, rhs) == 0;
      }

      constexpr TLFLOAT_INLINE bool operator!=(const UnpackedFloat& rhs) const {
	return !((*this) == rhs);
      }

      constexpr TLFLOAT_INLINE bool operator>(const UnpackedFloat& rhs) const {
	if (isnan || rhs.isnan) return false;
	return cmp(*this, rhs) == 1;
      }

      constexpr TLFLOAT_INLINE bool operator<(const UnpackedFloat& rhs) const {
	if (isnan || rhs.isnan) return false;
	return cmp(*this, rhs) == -1;
      }

      constexpr TLFLOAT_INLINE bool operator<=(const UnpackedFloat& rhs) const {
	if (isnan || rhs.isnan) return false;
	return cmp(*this, rhs) != 1;
      }

      constexpr TLFLOAT_INLINE bool operator>=(const UnpackedFloat& rhs) const {
	if (isnan || rhs.isnan) return false;
	return cmp(*this, rhs) != -1;
      }

      //

      template<typename rhstype>
      constexpr TLFLOAT_INLINE UnpackedFloat operator+(const rhstype& rhs) const { return *this + UnpackedFloat(rhs); }
      template<typename rhstype>
      constexpr TLFLOAT_INLINE UnpackedFloat operator-(const rhstype& rhs) const { return *this - UnpackedFloat(rhs); }
      template<typename rhstype>
      constexpr TLFLOAT_INLINE UnpackedFloat operator*(const rhstype& rhs) const { return *this * UnpackedFloat(rhs); }
      template<typename rhstype>
      constexpr UnpackedFloat operator/(const rhstype& rhs) const { return *this / UnpackedFloat(rhs); }

      template<typename rhstype>
      constexpr TLFLOAT_INLINE bool operator==(const rhstype& rhs) const { return *this == UnpackedFloat(rhs); }
      template<typename rhstype>
      constexpr TLFLOAT_INLINE bool operator!=(const rhstype& rhs) const { return *this != UnpackedFloat(rhs); }
      template<typename rhstype>
      constexpr TLFLOAT_INLINE bool operator> (const rhstype& rhs) const { return *this >  UnpackedFloat(rhs); }
      template<typename rhstype>
      constexpr TLFLOAT_INLINE bool operator< (const rhstype& rhs) const { return *this <  UnpackedFloat(rhs); }
      template<typename rhstype>
      constexpr TLFLOAT_INLINE bool operator>=(const rhstype& rhs) const { return *this >= UnpackedFloat(rhs); }
      template<typename rhstype>
      constexpr TLFLOAT_INLINE bool operator<=(const rhstype& rhs) const { return *this <= UnpackedFloat(rhs); }

      template<typename srctype>
      constexpr TLFLOAT_INLINE UnpackedFloat& operator=(const srctype& s) {
	UnpackedFloat n(s);
	*this = n;
	return *this;
      }

      template<typename rhstype>
      constexpr TLFLOAT_INLINE UnpackedFloat& operator+=(const rhstype& rhs) { *this = *this + UnpackedFloat(rhs); return *this; }
      template<typename rhstype>
      constexpr TLFLOAT_INLINE UnpackedFloat& operator-=(const rhstype& rhs) { *this = *this - UnpackedFloat(rhs); return *this; }
      template<typename rhstype>
      constexpr TLFLOAT_INLINE UnpackedFloat& operator*=(const rhstype& rhs) { *this = *this * UnpackedFloat(rhs); return *this; }
      template<typename rhstype>
      constexpr UnpackedFloat& operator/=(const rhstype& rhs) { *this = *this / UnpackedFloat(rhs); return *this; }

      //

      static constexpr TLFLOAT_INLINE UnpackedFloat exp10i(int a) {
	bool negative = a < 0;
	if (negative) a = -a;

	UnpackedFloat r = castFromInt(1), d = castFromInt(10);
	for(;a;a>>=1) {
	  if (a & 1) r *= d;
	  d *= d;
	}

	return negative ? castFromInt(1) / r : r;
      }

      friend constexpr TLFLOAT_INLINE int ilog10(const UnpackedFloat& a) { // returns floor(log10(a))
	int ret = rint(a.ilogb() * LOG10_2_);
	UnpackedFloat e = UnpackedFloat::exp10i(ret);
	if (e > a) {
	  ret--;
	} else if (e * castFromInt(10) < a) {
	  ret++;
	}
	return ret;
      }

      TLFLOAT_NOINLINE friend int snprint(char *cbuf, const size_t bufsize,
					  UnpackedFloat arg, char typespec, int width = 0, int precision = 6,
					  bool flag_sign = false, bool flag_blank = false, bool flag_alt = false,
					  bool flag_left = false, bool flag_zero = false, bool flag_upper = false) {
	if (bufsize == 0) return 0;
	if (bufsize == 1) { *cbuf = '\0'; return 0; }
	if (typespec != 'e' && typespec != 'f' && typespec != 'g' && typespec != 'a') return 0;

	detail::SafeArray<char> buf(cbuf, bufsize);

	if (width > (long)bufsize) width = bufsize;

	int64_t idx = 0;
	char prefix = 0;
	int length = 0, flag_rtz = 0, mainpos = 0;

	if (arg.sign) {
	  arg.sign = 0;
	  prefix = '-';
	} else if (flag_sign) {
	  prefix = '+';
	} else if (flag_blank) {
	  prefix = ' ';
	}

	UnpackedFloat value = arg;

	if (value.isnan) {
	  if (prefix) buf[idx++] = prefix;
	  buf.strcpyFrom(idx, flag_upper ? "NAN" : "nan");
	  idx += 3;
	  flag_zero = false;
	} else if (value.isinf) {
	  if (prefix) buf[idx++] = prefix;
	  buf.strcpyFrom(idx, flag_upper ? "INF" : "inf");
	  idx += 3;
	  flag_zero = false;
	} else if (typespec != 'a') {
	  if (precision < 0) precision = 6;
	  if (precision > (long)bufsize/2 - 10) precision = bufsize/2 - 10;
	  if (typespec == 'g' && precision > 0) precision--;

	  UnpackedFloat rounder = value.iszero ? UnpackedFloat::zero() : ldexp(UnpackedFloat::exp10i(-precision), -1);

	  if (typespec == 'f') {
	    UnpackedFloat v = (value + rounder) * UnpackedFloat::exp10i(precision);
	    if (iseven(v) || !isint(v)) value += rounder;
	  }

	  int exp = 0, e2 = 0;
	  if (!arg.iszero) {
	    exp = ilog10(value);
	    value *= UnpackedFloat::exp10i(-exp);
	  }

	  bool flag_exp = typespec == 'e';

	  if (typespec != 'f') {
	    UnpackedFloat v = value + rounder;
	    if (iseven(v) || !isint(v)) value = v;
	  }

	  if (value >= castFromInt(10)) {
	    value /= castFromInt(10);
	    exp++;
	  } else if (!value.iszero && value < castFromInt(1)) {
	    value *= castFromInt(10);
	    exp--;
	  }

	  if (typespec == 'g') {
	    flag_rtz = !flag_alt;
	    if (exp < -4 || exp > precision) {
	      typespec = 'e';
	    } else {
	      precision = precision - exp;
	      typespec = 'f';
	    }
	  }

	  if (typespec != 'e') e2 = exp;

	  bool flag_dp = precision > 0 || flag_alt;

	  if (prefix) buf[idx++] = prefix;

	  if (e2 < 0) {
	    buf[idx++] = '0';
	  } else {
	    for(;e2>=0;e2--) {
	      int digit = value.castToInt((int *)nullptr);
	      if (value < castFromInt(digit)) digit--;
	      buf[idx++] = digit + '0';
	      value = (value - castFromInt(digit)) * castFromInt(10);
	    }
	  }

	  if (flag_dp) buf[idx++] = '.';

	  for(e2++;e2 < 0 && precision > 0;precision--, e2++) buf[idx++] = '0';

	  while (precision-- > 0) {
	    int digit = value.castToInt((int *)nullptr);
	    if (value < castFromInt(digit)) digit--;
	    buf[idx++] = digit + '0';
	    value = (value - castFromInt(digit)) * castFromInt(10);
	  }

	  if (flag_rtz && flag_dp) {
	    while(buf[idx-1] == '0') buf[--idx] = '\0';
	    if (buf[idx-1] == '.') buf[--idx] = '\0';
	  }

	  if (flag_exp || (typespec == 'e' && exp != 0)) {
	    buf[idx++] = flag_upper ? 'E' : 'e';
	    if (arg.iszero) exp = 0;
	    if (exp < 0){
	      buf[idx++] = '-'; exp = -exp;
	    } else {
	      buf[idx++] = '+';
	    }

	    char str[32];
	    snprintf(str, sizeof(buf), "%02d", exp);
	    buf.strcpyFrom(idx, str);
	    idx += strlen(str);
	  }
	} else {
	  if (!value.iszero && precision >= 0 && precision < nbmant/4+1) {
	    int s = nbmant - precision*4 - 1;
	    if (s < nbmant && s >= 0) value.mant += ((mant_t)1) << s;
	    if (bit(value.mant, nbmant + 1)) { value.mant >>= 1; value.exp++; }
	  }

	  if (prefix) buf[idx++] = prefix;

	  buf.strcpyFrom(idx, flag_upper ? "0X" :"0x");
	  idx += 2;
	  mainpos = idx;
	  buf.strcpyFrom(idx, value.iszero ? "0" : "1");
	  idx += 1;

	  mant_t m = value.mant << (sizeof(mant_t)*8 - nbmant);

	  if ((!(m == 0 && precision < 0) && precision != 0) || flag_alt) buf.strcpyFrom(idx++, ".");

	  const char *digits = flag_upper ? "0123456789ABCDEF" : "0123456789abcdef";

	  const int niter = precision >= 0 ? precision : -1;
	  for(int i=0;(niter < 0 && m != 0) || i < niter;i++) {
	    buf[idx++] = digits[(unsigned)((m >> (sizeof(mant_t)*8 - 4)) & 0xf)];
	    m <<= 4;
	  }

	  int exp = value.ilogb();
	  if (value.iszero) exp = 0;

	  char str[32];
	  snprintf(str, sizeof(buf), "%c%+d", flag_upper ? 'P' : 'p', exp);
	  buf.strcpyFrom(idx, str);
	  idx += strlen(str);
	}

	buf[idx] = '\0';
	length = idx;

	if (!flag_zero) mainpos = 0;

	if (!flag_left && length < width) {
	  int nPad = width - length;
	  for(int i=width; i-nPad >= mainpos; i--) buf[i] = buf[i-nPad];

	  int i = mainpos;
	  if (prefix != 0 && flag_zero && typespec != 'a') i++;
	  while (nPad-- > 0) buf[i++] = flag_zero ? '0' : ' ';
	  length = width;
	}

	if (flag_left) {
	  while (length < width) buf[length++] = ' ';
	  buf[length] = '\0';
	}

	return length;
      }
    }; // class UnpackedFloat

    template<typename T>
    static consteval T const_M_PI_(int exp = 0) {
      return ldexp_(
	T("0x1.921fb54442d18469898cc51701b839a252049c1114cf98e804177d4c76273644a29410f31c6809bbdf2a33679a748636605614dbe4be286e9fc26adadaa38488p+1"), exp);
    }

    template<typename T>
    static consteval T const_M_E_(int exp = 0) {
      return ldexp_(
        T("0x2.b7e151628aed2a6abf7158809cf4f3c762e7160f38b4da56a784d9045190cfef324e7738926cfbe5f4bf8d8d8c31d763da06c80abb1185eb4f7c7b5757f59584p+0"), exp);
    }
  } // namespace detail

  /**
   * This is a trivially copyable generic template class that
   * represents an IEEE 754 floating-point number. By adjusting the
   * template parameters, it can represent an IEEE 754 floating-point
   * number of any precision. The data size and data structure of the
   * object are the same as the corresponding floating-point number
   * represented in the IEEE 754 format.
   */
  template<typename Unpacked_t>
  class TLFloat {
    template<typename> friend class TLFloat;

    typedef detail::UnpackedFloat<uint16_t, uint32_t, 5, 10> uhalf;
    typedef detail::UnpackedFloat<uint16_t, uint32_t, 8, 7> ubf16;
    typedef detail::UnpackedFloat<uint32_t, uint64_t, 8, 23> ufloat;
    typedef detail::UnpackedFloat<uint64_t, BigUInt<7>, 11, 52> udouble;
    typedef detail::UnpackedFloat<BigUInt<7>, BigUInt<8>, 15, 112> uquad;
    typedef detail::UnpackedFloat<BigUInt<8>, BigUInt<9>, 19, 236> uoctuple;

    typedef decltype(Unpacked_t::mant) mant_t;
    typedef decltype(Unpacked_t::xUnpackedFloat()) xUnpacked_t;

  public:
    mant_t m = 0;

    /** Returns NaN */
    static constexpr TLFLOAT_INLINE TLFloat nan() { return Unpacked_t::nan(); }

    /** Returns infinity of the given sign */
    static constexpr TLFLOAT_INLINE TLFloat infinity(bool sign=false) { return Unpacked_t::infinity(sign); }

    /** Returns the minimum representative number of the given sign */
    static constexpr TLFLOAT_INLINE TLFloat flt_true_min(bool sign=false) { return Unpacked_t::flt_true_min(sign); }

    /** Returns the minimum normalized number of the given sign */
    static constexpr TLFLOAT_INLINE TLFloat flt_min(bool sign=false) { return Unpacked_t::flt_min(sign); }

    /** Returns the maximum representative number of the given sign */
    static constexpr TLFLOAT_INLINE TLFloat flt_max(bool sign=false) { return Unpacked_t::flt_max(sign); }

    /** Returns the absolute value difference between 1.0 and the next representable value */
    static constexpr TLFLOAT_INLINE TLFloat flt_epsilon() { return nextafter(TLFloat(1), 2) - 1; }

    /** Returns zero of the given sign */
    static constexpr TLFLOAT_INLINE TLFloat zero(bool sign=false) { return Unpacked_t::zero(sign); }

    constexpr TLFLOAT_INLINE TLFloat() = default;
    constexpr TLFLOAT_INLINE TLFloat(const TLFloat&) = default;
    constexpr TLFLOAT_INLINE TLFloat& operator=(const TLFloat&) = default;

    constexpr TLFLOAT_INLINE Unpacked_t getUnpacked() const { return Unpacked_t(m); }

    static constexpr TLFLOAT_INLINE Unpacked_t to_Unpacked_t(TLFloat f = zero()) { return f.getUnpacked(); }

    template<typename mant_t_, typename longmant_t_, int nbexp_, int nbmant_>
    constexpr TLFLOAT_INLINE TLFloat(const detail::UnpackedFloat<mant_t_, longmant_t_, nbexp_, nbmant_> &tf) :
      m(mant_t(tf.cast((Unpacked_t *)0))) {}

    /** Cast from an instance of any TLFloat class */
    template<typename srctftype>
    constexpr TLFLOAT_INLINE TLFloat(const TLFloat<srctftype>& s) :
      TLFloat(s.getUnpacked().cast((const Unpacked_t *)nullptr)) {}

    /** Cast from any primitive integer type */
    template<typename inttype, std::enable_if_t<(std::is_integral_v<inttype>), int> = 0>
    constexpr TLFLOAT_INLINE TLFloat(const inttype x) :
      TLFloat(Unpacked_t::castFromInt(x)) {}

    /** Cast to any primitive integer type */
    template<typename inttype, std::enable_if_t<(std::is_integral_v<inttype>), int> = 0>
    constexpr TLFLOAT_INLINE operator inttype() const {
      return getUnpacked().castToInt((const inttype *)nullptr);
    }

    template<int N> constexpr TLFLOAT_INLINE TLFloat(const BigInt <N> x) : TLFloat(Unpacked_t::castFromInt(x)) {}
    template<int N> constexpr TLFLOAT_INLINE TLFloat(const BigUInt<N> x) : TLFloat(Unpacked_t::castFromInt(x)) {}

    template<int N> constexpr TLFLOAT_INLINE operator BigInt <N>() const { return getUnpacked().castToInt((const BigInt <N> *)nullptr); }
    template<int N> constexpr TLFLOAT_INLINE operator BigUInt<N>() const { return getUnpacked().castToInt((const BigUInt<N> *)nullptr); }

    constexpr TLFloat(const double d) {
      if constexpr (sizeof(mant_t) == sizeof(double)) {
	m = std::bit_cast<mant_t>(d);
      } else {
	udouble td(d);
	m = mant_t(td.cast((const Unpacked_t *)nullptr));
      }
    }

    constexpr TLFloat(const float f) {
      if constexpr (sizeof(mant_t) == sizeof(float)) {
	m = std::bit_cast<mant_t>(f);
      } else {
	ufloat tf(f);
	m = mant_t(tf.cast((const Unpacked_t *)nullptr));
      }
    }

    /** Any non-integer object is bitcast to a TLFloat type of the same size with this constructor */
    template<typename fptype,
      std::enable_if_t<(sizeof(fptype) == sizeof(mant_t) && std::is_trivially_copyable_v<fptype> &&
			(std::is_floating_point_v<fptype> || (!std::is_pointer_v<fptype> && !std::is_integral_v<fptype>))), int> = 0>
    constexpr TLFLOAT_INLINE TLFloat(const fptype& s) {
      m = std::bit_cast<mant_t>(s);
    }

    template<typename fptype,
      std::enable_if_t<(sizeof(fptype) == sizeof(mant_t) && std::is_trivially_copyable_v<fptype> &&
			(std::is_floating_point_v<fptype> || (!std::is_pointer_v<fptype> && !std::is_integral_v<fptype>))), int> = 0>
    explicit constexpr TLFLOAT_INLINE operator fptype() const {
      return std::bit_cast<fptype>(m);
    }

    /** This works like strtod */
    constexpr TLFloat(const char *ptr, const char **endptr=nullptr) {
      *this = TLFloat(xUnpacked_t(ptr, endptr).cast((Unpacked_t *)nullptr));
    }

    explicit constexpr TLFLOAT_INLINE operator Unpacked_t() const { return Unpacked_t(m); }

    explicit constexpr operator double() const {
      if constexpr (sizeof(mant_t) == sizeof(double)) {
	return std::bit_cast<double>(m);
      } else {
	Unpacked_t tf(m);
	return double(tf.cast((const udouble *)nullptr));
      }
    }

    explicit constexpr operator float() const {
      if constexpr (sizeof(mant_t) == sizeof(float)) {
	return std::bit_cast<float>(m);
      } else {
	Unpacked_t tf(m);
	return float(tf.cast((const ufloat *)nullptr));
      }
    }

    constexpr TLFLOAT_INLINE TLFloat operator-() const {
      TLFloat n(*this);
      n.m ^= mant_t(1) << (sizeof(mant_t)*8-1);
      return n;
    }

    constexpr TLFLOAT_INLINE TLFloat operator+() const { return TLFloat(*this); }

    /** This function performs addition of two floating point numbers. This function returns correctly rounded results. */
    constexpr TLFloat operator+(const TLFloat& rhs) const { return getUnpacked() + rhs.getUnpacked(); }

    /** This function performs subtraction of floating point numbers. This function returns correctly rounded results. */
    constexpr TLFloat operator-(const TLFloat& rhs) const { return getUnpacked() - rhs.getUnpacked(); }

    /** This function performs multiplication of two floating point numbers. This function returns correctly rounded results. */
    constexpr TLFloat operator*(const TLFloat& rhs) const { return getUnpacked() * rhs.getUnpacked(); }

    /** This function performs division of floating point numbers. This function returns correctly rounded results. */
    constexpr TLFloat operator/(const TLFloat& rhs) const { return getUnpacked() / rhs.getUnpacked(); }

    /** This function has the same functionality as the corresponding function in math.h. This function returns correctly rounded results. */
    friend constexpr TLFloat fabs(const TLFloat& u) {
      TLFloat n(u);
      n.m &= ~(mant_t(1) << (sizeof(mant_t)*8-1));
      return n;
    }

    /** This function has the same functionality as the corresponding function in math.h. This function returns correctly rounded results. */
    friend constexpr TLFloat copysign(const TLFloat& x, const TLFloat& y) {
      TLFloat n(x);
      n.m &= ~(mant_t(1) << (sizeof(mant_t)*8-1));
      n.m |= y.m & (mant_t(1) << (sizeof(mant_t)*8-1));
      return n;
    }

    /** This function has the same functionality as the corresponding function in math.h. This function returns correctly rounded results. */
    friend constexpr TLFloat fmax(const TLFloat& x, const TLFloat& y) {
      return isnan(y) ? x : (x > y ? x : y);
    }

    /** This function has the same functionality as the corresponding function in math.h. This function returns correctly rounded results. */
    friend constexpr TLFloat fmin(const TLFloat& x, const TLFloat& y) {
      return isnan(y) ? x : (x < y ? x : y);
    }

    /** This function has the same functionality as the corresponding function in math.h. This function returns correctly rounded results. */
    friend constexpr TLFloat fdim(const TLFloat& x, const TLFloat& y) {
      TLFloat ret = x - y;
      if (ret < 0 || x == y) ret = 0;
      return ret;
    }

    /** This function has the same functionality as the corresponding function in math.h. */
    friend constexpr bool isnan(const TLFloat& u) { return u.getUnpacked().isnan; }

    /** This function has the same functionality as the corresponding function in math.h. */
    friend constexpr bool isinf(const TLFloat& u) { return u.getUnpacked().isinf; }

    /** This function has the same functionality as the corresponding function in math.h. */
    friend constexpr bool finite(const TLFloat& u) { return !(isnan(u) || isinf(u)); }

    /** This function has the same functionality as the corresponding function in math.h. */
    friend constexpr int fpclassify(const TLFloat& u) {
      if (isnan(u)) return TLFLOAT_FP_NAN;
      if (isinf(u)) return TLFLOAT_FP_INFINITE;
      if (u == 0) return TLFLOAT_FP_ZERO;
      if (fabs(u) < u.getUnpacked().flt_min()) return TLFLOAT_FP_SUBNORMAL;
      return TLFLOAT_FP_NORMAL;
    }

    /** This function has the same functionality as the corresponding function in math.h. */
    friend constexpr TLFLOAT_INLINE bool signbit(const TLFloat& u) {
      TLFloat n(u);
      return Unpacked_t::bit(n.m, sizeof(mant_t) * 8 - 1);
    }
    friend constexpr TLFLOAT_INLINE bool iszero(const TLFloat& u) { return u.getUnpacked().iszero; }
    friend constexpr TLFLOAT_INLINE bool isint(const TLFloat& u) { return isint(u.getUnpacked()); }
    friend constexpr TLFLOAT_INLINE bool iseven(const TLFloat& u) { return iseven(u.getUnpacked()); }

    /** This function performs the fused multiply-add operation of floating point numbers. This function returns correctly rounded results. */
    friend constexpr TLFloat fma(const TLFloat& x, const TLFloat& y, const TLFloat& z) {
      return Unpacked_t::fma(x.getUnpacked(), y.getUnpacked(), z.getUnpacked());
    }

    /** This function performs ordered comparison of two floating point numbers. */
    constexpr TLFLOAT_INLINE bool operator==(const TLFloat& rhs) const { return getUnpacked() == rhs.getUnpacked(); }

    /** This function performs ordered comparison of two floating point numbers. */
    constexpr TLFLOAT_INLINE bool operator!=(const TLFloat& rhs) const { return getUnpacked() != rhs.getUnpacked(); }

    /** This function performs ordered comparison of two floating point numbers. */
    constexpr TLFLOAT_INLINE bool operator> (const TLFloat& rhs) const { return getUnpacked() >  rhs.getUnpacked(); }

    /** This function performs ordered comparison of two floating point numbers. */
    constexpr TLFLOAT_INLINE bool operator< (const TLFloat& rhs) const { return getUnpacked() <  rhs.getUnpacked(); }

    /** This function performs ordered comparison of two floating point numbers. */
    constexpr TLFLOAT_INLINE bool operator>=(const TLFloat& rhs) const { return getUnpacked() >= rhs.getUnpacked(); }

    /** This function performs ordered comparison of two floating point numbers. */
    constexpr TLFLOAT_INLINE bool operator<=(const TLFloat& rhs) const { return getUnpacked() <= rhs.getUnpacked(); }

    //

    template<typename srctype>
    constexpr TLFLOAT_INLINE TLFloat& operator=(const srctype& s) {
      TLFloat n(s);
      *this = n;
      return *this;
    }

    template<typename rhstype>
    constexpr TLFLOAT_INLINE TLFloat& operator+=(const rhstype& rhs) { *this = *this + TLFloat(rhs); return *this; }
    template<typename rhstype>
    constexpr TLFLOAT_INLINE TLFloat& operator-=(const rhstype& rhs) { *this = *this - TLFloat(rhs); return *this; }
    template<typename rhstype>
    constexpr TLFLOAT_INLINE TLFloat& operator*=(const rhstype& rhs) { *this = *this * TLFloat(rhs); return *this; }
    template<typename rhstype>
    constexpr TLFloat& operator/=(const rhstype& rhs) { *this = *this / TLFloat(rhs); return *this; }

    constexpr TLFLOAT_INLINE TLFloat& operator++()    { *this += 1; return *this; }
    constexpr TLFLOAT_INLINE TLFloat& operator--()    { *this -= 1; return *this; }
    constexpr TLFLOAT_INLINE TLFloat  operator++(int) { TLFloat t = *this; *this += 1; return t; }
    constexpr TLFLOAT_INLINE TLFloat  operator--(int) { TLFloat t = *this; *this -= 1; return t; }

    //

    // Double + int, Double + float, Double + Float
    template<typename rhstype, std::enable_if_t<(std::is_integral_v<rhstype> || sizeof(rhstype) <= sizeof(mant_t)), int> = 0>
    constexpr TLFLOAT_INLINE TLFloat operator+(const rhstype& rhs) const { return *this + TLFloat(rhs); }
    template<typename rhstype, std::enable_if_t<(std::is_integral_v<rhstype> || sizeof(rhstype) <= sizeof(mant_t)), int> = 0>
    constexpr TLFLOAT_INLINE TLFloat operator-(const rhstype& rhs) const { return *this - TLFloat(rhs); }
    template<typename rhstype, std::enable_if_t<(std::is_integral_v<rhstype> || sizeof(rhstype) <= sizeof(mant_t)), int> = 0>
    constexpr TLFLOAT_INLINE TLFloat operator*(const rhstype& rhs) const { return *this * TLFloat(rhs); }
    template<typename rhstype, std::enable_if_t<(std::is_integral_v<rhstype> || sizeof(rhstype) <= sizeof(mant_t)), int> = 0>
    constexpr TLFloat operator/(const rhstype& rhs) const { return *this / TLFloat(rhs); }

    // int + Double, float + Double, Float + Double
    template<typename lhstype, std::enable_if_t<(std::is_integral_v<lhstype> || sizeof(lhstype) <= sizeof(mant_t)), int> = 0>
    friend constexpr TLFLOAT_INLINE TLFloat operator+(const lhstype& lhs, const TLFloat &rhs) { return TLFloat(lhs) + rhs; }
    template<typename lhstype, std::enable_if_t<(std::is_integral_v<lhstype> || sizeof(lhstype) <= sizeof(mant_t)), int> = 0>
    friend constexpr TLFLOAT_INLINE TLFloat operator-(const lhstype& lhs, const TLFloat &rhs) { return TLFloat(lhs) - rhs; }
    template<typename lhstype, std::enable_if_t<(std::is_integral_v<lhstype> || sizeof(lhstype) <= sizeof(mant_t)), int> = 0>
    friend constexpr TLFLOAT_INLINE TLFloat operator*(const lhstype& lhs, const TLFloat &rhs) { return TLFloat(lhs) * rhs; }
    template<typename lhstype, std::enable_if_t<(std::is_integral_v<lhstype> || sizeof(lhstype) <= sizeof(mant_t)), int> = 0>
    friend constexpr TLFloat operator/(const lhstype& lhs, const TLFloat &rhs) { return TLFloat(lhs) / rhs; }

    // Double == int, Double == float, Double == Float
    template<typename rhstype, std::enable_if_t<(std::is_integral_v<rhstype> || sizeof(rhstype) <= sizeof(mant_t)), int> = 0>
    constexpr TLFLOAT_INLINE bool operator==(const rhstype& rhs) const { return *this == TLFloat(rhs); }
    template<typename rhstype, std::enable_if_t<(std::is_integral_v<rhstype> || sizeof(rhstype) <= sizeof(mant_t)), int> = 0>
    constexpr TLFLOAT_INLINE bool operator!=(const rhstype& rhs) const { return *this != TLFloat(rhs); }
    template<typename rhstype, std::enable_if_t<(std::is_integral_v<rhstype> || sizeof(rhstype) <= sizeof(mant_t)), int> = 0>
    constexpr TLFLOAT_INLINE bool operator> (const rhstype& rhs) const { return *this >  TLFloat(rhs); }
    template<typename rhstype, std::enable_if_t<(std::is_integral_v<rhstype> || sizeof(rhstype) <= sizeof(mant_t)), int> = 0>
    constexpr TLFLOAT_INLINE bool operator< (const rhstype& rhs) const { return *this <  TLFloat(rhs); }
    template<typename rhstype, std::enable_if_t<(std::is_integral_v<rhstype> || sizeof(rhstype) <= sizeof(mant_t)), int> = 0>
    constexpr TLFLOAT_INLINE bool operator>=(const rhstype& rhs) const { return *this >= TLFloat(rhs); }
    template<typename rhstype, std::enable_if_t<(std::is_integral_v<rhstype> || sizeof(rhstype) <= sizeof(mant_t)), int> = 0>
    constexpr TLFLOAT_INLINE bool operator<=(const rhstype& rhs) const { return *this <= TLFloat(rhs); }

    // int == Double, float == Double, Float == Double
    template<typename lhstype, std::enable_if_t<(std::is_integral_v<lhstype> || sizeof(lhstype) <= sizeof(mant_t)), int> = 0>
    friend constexpr TLFLOAT_INLINE bool operator==(const lhstype& lhs, const TLFloat &rhs) { return TLFloat(lhs) == rhs; }
    template<typename lhstype, std::enable_if_t<(std::is_integral_v<lhstype> || sizeof(lhstype) <= sizeof(mant_t)), int> = 0>
    friend constexpr TLFLOAT_INLINE bool operator!=(const lhstype& lhs, const TLFloat &rhs) { return TLFloat(lhs) != rhs; }
    template<typename lhstype, std::enable_if_t<(std::is_integral_v<lhstype> || sizeof(lhstype) <= sizeof(mant_t)), int> = 0>
    friend constexpr TLFLOAT_INLINE bool operator> (const lhstype& lhs, const TLFloat &rhs) { return TLFloat(lhs) >  rhs; }
    template<typename lhstype, std::enable_if_t<(std::is_integral_v<lhstype> || sizeof(lhstype) <= sizeof(mant_t)), int> = 0>
    friend constexpr TLFLOAT_INLINE bool operator< (const lhstype& lhs, const TLFloat &rhs) { return TLFloat(lhs) <  rhs; }
    template<typename lhstype, std::enable_if_t<(std::is_integral_v<lhstype> || sizeof(lhstype) <= sizeof(mant_t)), int> = 0>
    friend constexpr TLFLOAT_INLINE bool operator>=(const lhstype& lhs, const TLFloat &rhs) { return TLFloat(lhs) >= rhs; }
    template<typename lhstype, std::enable_if_t<(std::is_integral_v<lhstype> || sizeof(lhstype) <= sizeof(mant_t)), int> = 0>
    friend constexpr TLFLOAT_INLINE bool operator<=(const lhstype& lhs, const TLFloat &rhs) { return TLFloat(lhs) <= rhs; }

    //

    /** This function has the same functionality as the corresponding function in math.h. */
    friend constexpr int32_t ilogb(const TLFloat& f) {
      auto u = f.getUnpacked();
      if (u.isinf) return 2147483647;
      if (u.isnan) return TLFLOAT_FP_ILOGBNAN;
      if (u.exp != 0) return u.exp - Unpacked_t::expoffset() + 1;
      if (u.mant == 0) return TLFLOAT_FP_ILOGB0;
      return (sizeof(mant_t) * 8 - Unpacked_t::nbmant_()) - Unpacked_t::clz(u.mant) - Unpacked_t::expoffset();
    }

    friend constexpr xpair<TLFloat, int> frexp(const TLFloat& f) {
      if (f == 0 || isnan(f) || isinf(f)) return xpair<TLFloat, int> { f, 0 };
      auto p = frexp_(f.getUnpacked().cast((xUnpacked_t *)0));
      return xpair<TLFloat, int> { (TLFloat)p.first.cast((Unpacked_t *)0), p.second };
    }

    /** This function has the same functionality as the corresponding function in math.h. */
    friend constexpr TLFloat frexp(const TLFloat& f, int *exp) {
      auto p = frexp(f);
      if (exp) *exp = p.second;
      return p.first;
    }

    /** This function has the same functionality as the corresponding function in math.h. */
    friend constexpr TLFloat ldexp(const TLFloat& f, int exp) {
      return (TLFloat)ldexp_(f.getUnpacked().cast((xUnpacked_t *)0), exp).cast((Unpacked_t *)0);
    }

    /** Returns 10^a */
    static constexpr TLFloat exp10i(int a) { return TLFloat(Unpacked_t::exp10i(a)); }

    /** This function finds the square root of a floating point number. This function returns correctly rounded results. */
    friend constexpr TLFloat sqrt(const TLFloat& d) {
      if (d < 0) return nan();
      if (isinf(d) || isnan(d)) return d;
      if (d.getUnpacked().iszero) return d;

      return TLFloat(sqrt_(d.getUnpacked()));
    }

    /** This function has the same functionality as the corresponding function in math.h. The accuracy of the return value is 1ULP. */
    friend constexpr TLFloat hypot(const TLFloat& x, const TLFloat& y) {
      auto xx = x.getUnpacked().cast((xUnpacked_t *)0);
      auto yy = y.getUnpacked().cast((xUnpacked_t *)0);
      if (xx.isinf || yy.isinf) return infinity(false);
      if (xx.isnan || yy.isnan) return nan();
      return sqrt_(xx * xx + yy * yy).cast((Unpacked_t *)0);
    }

    /** This function has the same functionality as the corresponding function in math.h. This function returns correctly rounded results. */
    friend constexpr TLFloat trunc(const TLFloat& d) { return (TLFloat)trunc(d.getUnpacked()); }

    /** This function has the same functionality as the corresponding function in math.h. This function returns correctly rounded results. */
    friend constexpr TLFloat floor(const TLFloat& d) { return (TLFloat)floor(d.getUnpacked()); }

    /** This function has the same functionality as the corresponding function in math.h. This function returns correctly rounded results. */
    friend constexpr TLFloat ceil (const TLFloat& d) { return (TLFloat)ceil (d.getUnpacked()); }

    /** This function has the same functionality as the corresponding function in math.h. This function returns correctly rounded results. */
    friend constexpr TLFloat round(const TLFloat& d) { return (TLFloat)round(d.getUnpacked()); }

    /** This function has the same functionality as the corresponding function in math.h. This function returns correctly rounded results. */
    friend constexpr TLFloat rint (const TLFloat& d) { return (TLFloat)rint (d.getUnpacked()); }

    /** This function has the same functionality as the corresponding function in math.h. This function returns correctly rounded results. */
    friend constexpr TLFloat modf(const TLFloat& x, TLFloat* iptr) {
      if (isnan(x) || iszero(x)) { *iptr = x; return x; }
      if (isinf(x)) { *iptr = x; return zero(signbit(x)); }
      TLFloat i = trunc(x);
      if (iptr) *iptr = i;
      return x - i;
    }

    /** This function has the same functionality as the corresponding function in math.h. This function returns correctly rounded results. */
    friend constexpr TLFloat nextafter(const TLFloat& x, const TLFloat& y) {
      if (isnan(x)) return x;
      if (isnan(y)) return y;
      int c = cmp(x.getUnpacked(), y.getUnpacked()), i = signbit(x) ? 1 : -1;
      TLFloat r = x;
      if (iszero(x)) {
	if (c > 0) {
	  r.m = (mant_t(1) << (sizeof(mant_t)*8-1)) + 1;
	} else if (c < 0) {
	  r.m = 1;
	} else {
	  r = y;
	}
      } else {
	r.m += c * i;
      }
      return r;
    }
  }; // class TLFloat

  //

#if defined(TLFLOAT_DOXYGEN) || !defined(TLFLOAT_NO_LIBSTDCXX)
#ifndef TLFLOAT_DOXYGEN
  template<typename mant_t, typename longmant_t, int nbexp, int nbmant>
  static std::string to_string_d(const detail::UnpackedFloat<mant_t, longmant_t, nbexp, nbmant> &uf) {
    std::string s;
    char buf[32];
    snprintf(buf, sizeof(buf), "%.18lg", to_double(uf));
    s += std::string(buf) + " : ";
    s += "sign = " + std::to_string(uf.sign);
    s += ", iszero = " + std::to_string(uf.iszero);
    s += ", isinf = " + std::to_string(uf.isinf);
    s += ", isnan = " + std::to_string(uf.isnan);
    s += ", exp = " + std::to_string(uf.exp - uf.expoffset()) + "(" + std::to_string(uf.exp) + ")";
    s += std::string(", mant = ") + toHexString(uf.mant);
    return s;
  }

  template<typename Unpacked_t>
  static std::string to_string(Unpacked_t u, int d=6) {
    typedef decltype(Unpacked_t::xUnpackedFloat()) xUnpacked_t;
    std::vector<char> buf(1000);
    snprint(buf.data(), buf.size(), u.cast((xUnpacked_t *)0), 'g', 0, d, false, false, false, false, false, false);
    return std::string(buf.data());
  }
#endif

  /** This works like sprintf(str, "%.6g", *this) where the precision can be specified with d */
  template<typename Unpacked_t>
  static std::string to_string(TLFloat<Unpacked_t> a, int d=6) {
    typedef decltype(Unpacked_t::xUnpackedFloat()) xUnpacked_t;
    std::vector<char> buf(1000);
    snprint(buf.data(), buf.size(), a.getUnpacked().cast((xUnpacked_t *)0), 'g', 0, d, false, false, false, false, false, false);
    return std::string(buf.data());
  }

  template<typename Unpacked_t>
  static std::ostream& operator<<(std::ostream &os, const TLFloat<Unpacked_t>& a) {
    typedef decltype(Unpacked_t::xUnpackedFloat()) xUnpacked_t;
    std::vector<char> buf(1000);
    std::ios_base::fmtflags f = os.flags();
    char typespec = 'g';
    if ((f & os.fixed) != 0) typespec = 'f';
    if ((f & os.hex) != 0) typespec = 'a';
    snprint(buf.data(), buf.size(), a.getUnpacked().cast((xUnpacked_t *)0), typespec, os.width(), os.precision(),
	    false, false, false, (f & std::ios::left) != 0, false, (f & std::ios::uppercase) != 0);
    return os << buf.data();
  }
#endif // #if defined(TLFLOAT_DOXYGEN) || !defined(TLFLOAT_NO_LIBSTDCXX)

  //

  typedef TLFloat<detail::UnpackedFloat<uint16_t, uint32_t, 8, 7>> BFloat16; ///< This class represents a BFloat16 floating-point number. The data size and data structure of the objects are the same as the corresponding floating-point number.
  typedef TLFloat<detail::UnpackedFloat<uint16_t, uint32_t, 5, 10>> Half; ///< This class represents a half-precision IEEE 754 floating-point number. The data size and data structure of the objects are the same as the corresponding floating-point number.
  typedef TLFloat<detail::UnpackedFloat<uint32_t, uint64_t, 8, 23>> Float; ///< This class represents a single-precision IEEE 754 floating-point number. The data size and data structure of the objects are the same as the corresponding floating-point number.
  typedef TLFloat<detail::UnpackedFloat<uint64_t, BigUInt<7>, 11, 52>> Double; ///< This class represents a double-precision IEEE 754 floating-point number. The data size and data structure of the objects are the same as the corresponding floating-point number.
  typedef TLFloat<detail::UnpackedFloat<BigUInt<7>, BigUInt<8>, 15, 112>> Quad; ///< This class represents a quadruple-precision IEEE 754 floating-point number. The data size and data structure of the objects are the same as the corresponding floating-point number.
  typedef TLFloat<detail::UnpackedFloat<BigUInt<8>, BigUInt<9>, 19, 236>> Octuple; ///< This class represents a octuple-precision IEEE 754 floating-point number. The data size and data structure of the objects are the same as the corresponding floating-point number.

  static_assert(sizeof(BFloat16) == 2);
  static_assert(sizeof(Half) == 2);
  static_assert(sizeof(Float) == 4);
  static_assert(sizeof(Double) == 8);
  static_assert(sizeof(Quad) == 16);
  static_assert(sizeof(Octuple) == 32);

  static_assert(std::is_trivially_copyable_v<BFloat16>);
  static_assert(std::is_trivially_copyable_v<Half>);
  static_assert(std::is_trivially_copyable_v<Float>);
  static_assert(std::is_trivially_copyable_v<Double>);
  static_assert(std::is_trivially_copyable_v<Quad>);
  static_assert(std::is_trivially_copyable_v<Octuple>);
} // namespace tlfloat
#endif // #ifndef __TLFLOAT_HPP_INCLUDED__
