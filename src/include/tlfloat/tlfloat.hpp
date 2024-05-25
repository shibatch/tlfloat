#ifndef __TLFLOAT_HPP_INCLUDED__
#define __TLFLOAT_HPP_INCLUDED__

#include <cstdio>

#include "tlfloat/bigint.hpp"

namespace tlfloat {
  namespace detail {
    static const double LOG10_2 = 0.30102999566398119521; // log(2)/log(10)

    static constexpr bool xisspace(int c) { return c == ' ' || c == '\f' || c == '\n' || c == '\r' || c == '\t' || c == '\v'; }
    static constexpr bool xisdigit(int c) { return '0' <= c && c <= '9'; }
    static constexpr int xtolower(int c) { if ('A' <= c && c <= 'Z') return c - 'A' + 'a'; else return c; }

    static constexpr int xstrcasecmp(const char *p, const char *q) {
      for(;;) {
	if (*p == '\0' && *q == '\0') return 0;
	if (xtolower(*p) != xtolower(*q)) return *p > *q ? 1 : -1;
	p++; q++;
      }
    }

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

    static constexpr double xsqrt(double d) {
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

    template<typename mant_t, typename longmant_t, int nbexp, int nbmant>
    class UnpackedFloat {
      static_assert(sizeof(longmant_t) == 2 * sizeof(mant_t));

      template<typename floattype>
      static constexpr mant_t mantBits(const floattype& fl) {
	return toBits(fl) & ((mant_t(1) << nbmant) - 1);
      }

      template<typename floattype>
      static constexpr mant_t expBits(const floattype& fl) {
	return (toBits(fl) >> nbmant) & ((mant_t(1) << nbexp) - 1);
      }

      template<typename floattype>
      static constexpr mant_t toBits(const floattype& f) {
	static_assert(sizeof(floattype) == sizeof(mant_t));
	return std::bit_cast<mant_t>(f);
      }

      template<typename floattype>
      static constexpr floattype fromBits(const mant_t& u) {
	static_assert(sizeof(floattype) == sizeof(mant_t));
	return std::bit_cast<floattype>(u);
      }

      //

      static constexpr uint32_t mul(uint16_t x, uint16_t y) {
	return uint32_t(x) * uint32_t(y);
      }

      static constexpr uint64_t mul(uint32_t x, uint32_t y) {
	return uint64_t(x) * uint64_t(y);
      }

      static constexpr BigUInt<7> mul(uint64_t x, uint64_t y) {
	return BigUInt<7>::mul(BigUInt<6>(x), BigUInt<6>(y));
      }

      template<int N>
      static constexpr BigUInt<N+1> mul(const BigUInt<N>& x, const BigUInt<N>& y) {
	return BigUInt<N+1>::mul(x, y);
      }

      //

      static constexpr detail::xpair<uint16_t, uint16_t> divmod(uint32_t x, uint16_t y) {
	return detail::xpair<uint16_t, uint16_t> { uint16_t(x / y), uint16_t(x % y) };
      }

      static constexpr detail::xpair<uint32_t, uint32_t> divmod(uint64_t x, uint32_t y) {
	return detail::xpair<uint32_t, uint32_t> { uint32_t(x / y), uint32_t(x % y) };
      }

      template<int N>
      static constexpr detail::xpair<BigUInt<N-1>, BigUInt<N-1>> divmod(const BigUInt<N>& x, const BigUInt<N-1>& y) {
	auto p = x.divmod(y, BigUInt<N>(y).reciprocal());
	return detail::xpair<BigUInt<N-1>, BigUInt<N-1>> { (BigUInt<N-1>)p.first, (BigUInt<N-1>)p.second };
      }

      //

      static constexpr uint32_t isqrt(uint32_t s) {
	uint64_t u = xsqrt(double(s) * (double)0x100000000ULL);
	return u - (u >> 32);
      }

      static constexpr unsigned long long isqrt(uint64_t s) {
	double d = xsqrt((double)uint64_t(s) * double(1ULL << 32) * double(1ULL << 32));
	return d > 0x1.fffffffffffffp+63 ? UINT64_MAX : uint64_t(d); // nextafter((double)UINT64_MAX, 0)
      }

      static constexpr unsigned long long isqrt(BigUInt<6> s) {
	double d = xsqrt((double)uint64_t(s) * double(1ULL << 32) * double(1ULL << 32));
	return d > 0x1.fffffffffffffp+63 ? UINT64_MAX : uint64_t(d);
      }

      template<int N>
      static constexpr BigUInt<N> isqrt(BigUInt<N> s) {
	if (!std::is_constant_evaluated()) assert(s >= (BigUInt<N>(1) << (sizeof(BigUInt<N>) * 8 - 2)));
	BigUInt<N> x = BigUInt<N>(isqrt(s.high) | 1, 0);
	x = (x >> 1) + s.mulhiAprx(x.reciprocalAprx());
	return x + ((~x >> (sizeof(x)*8 - 1)) & 1);
      }

      //

    public:
      int32_t exp;
      mant_t mant;
      bool sign, iszero, isinf, isnan;

      static constexpr int expoffset() {
	if constexpr (nbexp != 0) return (1 << (nbexp - 1)) - 1;
	return 0;
      }
      static constexpr int nbexp_() { return nbexp; }
      static constexpr int nbmant_() { return nbmant; }
      static constexpr longmant_t longmant_t_() { return 0; }
      static constexpr UnpackedFloat<mant_t, longmant_t, 0, sizeof(mant_t)*8-2> xUnpackedFloat() {
	return UnpackedFloat<mant_t, longmant_t, 0, sizeof(mant_t)*8-2>(0, 0, false, true, false, false);
      }

      constexpr int32_t ilogb() const {
	if (exp != 0) return exp - expoffset() + 1;
	return (sizeof(mant_t) * 8 - nbmant_()) - clz(mant) - expoffset();
      }

      friend constexpr UnpackedFloat fabs(UnpackedFloat x) { x.sign = 0; return x; }

      friend constexpr UnpackedFloat ldexp_(UnpackedFloat x, const int e) { x.exp += e; return x; }

      friend constexpr detail::xpair<UnpackedFloat, int> frexp_(UnpackedFloat x) {
	detail::xpair<UnpackedFloat, int> ret = { x, x.exp - expoffset() + 2 };
	ret.first.exp = expoffset() - 2;
	return ret;
      }

      friend constexpr int cmp(const UnpackedFloat &lhs, const UnpackedFloat &rhs) {
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

      static constexpr unsigned clz(uint64_t u) {
	return u == 0 ? 64 : clz64(u);
      }

#ifdef TLFLOAT_ENABLE_GNUC_CLZ
      static constexpr unsigned clz(uint32_t u) {
	return u == 0 ? 32 : __builtin_clz(u);
      }
#else
      static constexpr unsigned clz(uint32_t u) {
	return clz(uint64_t(u)) - 32;
      }
#endif

      static constexpr unsigned clz(uint16_t u) {
	return clz(uint32_t(u)) - 16;
      }

      template<int N>
      static constexpr unsigned clz(const BigUInt<N>& u) {
	return u.clz();
      }

      //

      static constexpr bool bit(const uint64_t x, unsigned b) { return (x >> b) & 1; }

      template<int N>
      static constexpr bool bit(const BigUInt<N>& x, unsigned b) { return x.bit(b); }

      //

      friend constexpr double to_double(const UnpackedFloat &uf) {
	typedef UnpackedFloat<uint64_t, BigUInt<7>, 11, 52> udouble;
	return (double)uf.cast((udouble *)nullptr);
      }

      static constexpr UnpackedFloat nan() {
	return UnpackedFloat((mant_t(3) << (nbmant - 1)), (1 << nbexp) - 2, false, false, false, true);
      }

      static constexpr UnpackedFloat inf(bool sign = false) {
	return UnpackedFloat(mant_t(1) << nbmant, (1 << nbexp) - 2, sign, false, true, false);
      }

      static constexpr UnpackedFloat floatdenormmin(bool sign = false) {
	return UnpackedFloat(mant_t(1), 0, sign, false, false, false);
      }

      static constexpr UnpackedFloat floatmax(bool sign = false) {
	return UnpackedFloat((mant_t(2) << nbmant) - 1, (1 << nbexp) - 3, sign, false, false, false);
      }

      static constexpr UnpackedFloat zero(bool sign = false) {
	return UnpackedFloat(0, 0, sign, true, false, false);
      }

      static constexpr UnpackedFloat addsub(const UnpackedFloat &lhs, const UnpackedFloat &rhs, bool negateRhs) {
	const bool rhssign = rhs.sign != negateRhs;

	if (lhs.iszero | lhs.isinf | lhs.isnan | rhs.iszero | rhs.isinf | rhs.isnan) {
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
	}

	const int ed = lhs.exp - rhs.exp;
	const bool subtract = lhs.sign != rhssign;

	if (ed > +nbmant+2) return lhs;
	if (ed < -nbmant-2)
	  return UnpackedFloat(rhs.mant, rhs.exp, rhssign, rhs.iszero, rhs.isinf, rhs.isnan);

	longmant_t lm = longmant_t(lhs.mant) << (ed > 0 ? sizeof(mant_t) * 8      : sizeof(mant_t) * 8 + ed);
	longmant_t rm = longmant_t(rhs.mant) << (ed > 0 ? sizeof(mant_t) * 8 - ed : sizeof(mant_t) * 8     );
	int exp = ed > 0 ? lhs.exp : rhs.exp;

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
	  am += ((longmant_t(1) << (sizeof(mant_t) * 8 + y - 1)) - 1) + bit(am, sizeof(mant_t) * 8 + y);
	  am >>= y;

	  if (bit(am, (sizeof(mant_t) * 8) + nbmant + 1)) { am >>= 1; exp++; }
	}

	am >>= sizeof(mant_t) * 8;
	if (am == 0) exp = 0;
	return UnpackedFloat(mant_t(am), exp, resultsign, am == 0, nbexp != 0 && exp == (1 << nbexp) - 2, false);
      }

      static constexpr UnpackedFloat mul(const UnpackedFloat &lhs, const UnpackedFloat &rhs) {
	if (int(lhs.isnan) | int(rhs.isnan) | (int(lhs.iszero) & int(rhs.isinf)) |
	    (int(lhs.isinf) & int(rhs.iszero)) | int(lhs.isinf) | int(rhs.isinf)) {
	  if (lhs.isnan) return lhs;
	  if (rhs.isnan) return rhs;
	  if ((lhs.iszero && rhs.isinf) || (lhs.isinf && rhs.iszero)) return nan();
	  if (lhs.isinf || rhs.isinf) return inf(lhs.sign != rhs.sign);
	}

	int exp = lhs.exp + rhs.exp - expoffset();
	longmant_t am = mul(lhs.mant, rhs.mant);

	const int x = (int)clz(am) - ((int)sizeof(mant_t)*8 - nbmant) * 2;
	am = am << (x + sizeof(mant_t)*8 - nbmant + 1);

	exp -= x;

	int y = 0;
	if constexpr (nbexp != 0) {
	  if (exp < 0) {
	    if (exp <= -(int)sizeof(longmant_t) * 8) am = 0;
	    y = -exp;
	    exp = 0;
	  }
	}
	am += ((longmant_t(1) << (sizeof(mant_t) * 8 + y - 1)) - 1) + bit(am, sizeof(mant_t) * 8 + y);
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
	return UnpackedFloat(mant_t(am), exp, lhs.sign != rhs.sign, am == 0, nbexp != 0 && exp == (1 << nbexp) - 2, false);
      }

      static constexpr UnpackedFloat div(const UnpackedFloat &lhs, const UnpackedFloat &rhs) {
	if (lhs.isnan | lhs.isinf | rhs.isnan | rhs.iszero | rhs.isinf) {
	  if (lhs.isnan) return lhs;
	  if (rhs.isnan) return rhs;

	  if (rhs.iszero) {
	    if (lhs.iszero) return nan();
	    return inf(lhs.sign != rhs.sign);
	  }
	  if (rhs.isinf) {
	    if (lhs.isinf) return nan();
	    return zero(lhs.sign != rhs.sign);
	  }
	  if (lhs.isinf) return inf(lhs.sign != rhs.sign);
	}

	int exp = lhs.exp - rhs.exp + expoffset() - 1;
	int sl = clz(lhs.mant) + nbmant + 1 - sizeof(mant_t) * 8 + 1;
	int sr = clz(rhs.mant) + nbmant + 1 - sizeof(mant_t) * 8;
	exp -= sl - sr;

	auto p = divmod(longmant_t(lhs.mant) << (sl + nbmant), rhs.mant << sr);
	longmant_t am = longmant_t(p.first) << (sizeof(mant_t)*8);

	if (mant_t(p.second) * 2 > (rhs.mant << sr)) { 
	  am += longmant_t(3) << (sizeof(mant_t)*8 - 2);
	} else if (mant_t(p.second) * 2 == (rhs.mant << sr)) { 
	  am += longmant_t(2) << (sizeof(mant_t)*8 - 2);
	} else if (mant_t(p.second) != 0) {
	  am += longmant_t(1) << (sizeof(mant_t)*8 - 2);
	}

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
	am += ((longmant_t(1) << (sizeof(mant_t) * 8 + y - 1)) - 1) + bit(am, sizeof(mant_t) * 8 + y);
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
	return UnpackedFloat(mant_t(am), exp, lhs.sign != rhs.sign, am == 0, nbexp != 0 && exp == (1 << nbexp) - 2, false);
      }

      static constexpr UnpackedFloat fma(const UnpackedFloat &lhs, const UnpackedFloat &rhs, const UnpackedFloat &zhs) {
	if (lhs.isnan | lhs.isinf | rhs.isnan | rhs.isinf | zhs.isnan | zhs.isinf) {
	  if (lhs.isnan) return lhs;
	  if (rhs.isnan) return rhs;
	  if (zhs.isnan) return zhs;

	  if (lhs.isinf || rhs.isinf || zhs.isinf) {
	    if ((lhs.iszero && rhs.isinf) || (lhs.isinf && rhs.iszero)) return nan();
	    if (!zhs.isinf) return inf(lhs.sign != rhs.sign);
	    if (!(lhs.isinf || rhs.isinf) || ((lhs.sign != rhs.sign) == zhs.sign)) return inf(zhs.sign);
	    return nan();
	  }
	}

	int exp = lhs.exp + rhs.exp - expoffset();
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
	    amsb = (((longmant_t(1) << (NGB - ed)) - 1) & am) != 0;
	    am = (am >> -ed) & ~((longmant_t(1) << NGB) - 1);
	  }
	} else {
	  if (ed >= (int)sizeof(longmant_t) * 8 - NGB) {
	    zmsb = zm != 0;
	    zm = 0;
	  } else {
	    zmsb = (((longmant_t(1) << (NGB + ed)) - 1) & zm) != 0;
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
	  am += ((longmant_t(1) << (sizeof(mant_t) * 8 + y - 1)) - 1) + ((bit(am, sizeof(mant_t) * 8 + y) || psb) && !nsb);
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
	return UnpackedFloat(mant_t(am), exp, resultsign, am == 0, nbexp != 0 && exp == (1 << nbexp) - 2, false);
      }

      //

      constexpr UnpackedFloat() = default;
      constexpr UnpackedFloat(const UnpackedFloat&) = default;
      constexpr UnpackedFloat& operator=(const UnpackedFloat&) = default;

      constexpr UnpackedFloat(const mant_t mant_, int32_t exp_, bool sign_, bool iszero_, bool isinf_, bool isnan_) :
	exp(exp_), mant(mant_), sign(sign_), iszero(iszero_), isinf(isinf_), isnan(isnan_) {
      }

      constexpr UnpackedFloat(const char *ptr, const char **endptr=nullptr) {
	while(xisspace(*ptr)) ptr++;

	bool positive = true;
	if (*ptr == '-') {
	  positive = false; ptr++;
	} else if (*ptr == '+') {
	  ptr++;
	}

	if (xstrncasecmp(ptr, "nan", 3) == 0) { *this = nan();          if (endptr) { *endptr = ptr + 3; } return; }
	if (xstrncasecmp(ptr, "inf", 3) == 0) { *this = inf(!positive); if (endptr) { *endptr = ptr + 3; } return; }

	if (*ptr == '0' && (*(ptr+1) == 'x' || *(ptr+1) == 'X')) {
	  ptr += 2;
	  longmant_t m = 0;
	  exp = 0;
	  bool bp = false;
	  int dr = sizeof(mant_t)*2 + 1, wd = 0;
	  uint64_t w = 0;

	  while(*ptr != '\0' && dr > 0) {
	    int d = 0;
	    if (('0' <= *ptr && *ptr <= '9')) {
	      d = *ptr++ - '0';
	      if (d != 0) dr--;
	    } else if ('a' <= *ptr && *ptr <= 'f') {
	      d = *ptr++ - 'a' + 10;
	      dr--;
	    } else if ('A' <= *ptr && *ptr <= 'F') {
	      d = *ptr++ - 'A' + 10;
	      dr--;
	    } else if (*ptr == '.') {
	      if (bp) break;
	      bp = 1;
	      ptr++;
	      continue;
	    } else break;

	    w = (w << 4) + d;
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

	  while(('0' <= *ptr && *ptr <= '9') || ('a' <= *ptr && *ptr <= 'f') || ('A' <= *ptr && *ptr <= 'F')) ptr++;
	  if (*ptr == '.' && !bp) ptr++;
	  while(('0' <= *ptr && *ptr <= '9') || ('a' <= *ptr && *ptr <= 'f') || ('A' <= *ptr && *ptr <= 'F')) ptr++;

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
      constexpr UnpackedFloat(const floattype &fl) :
	exp(int32_t(expBits(fl) - (expBits(fl) != 0))),
	mant(mantBits(fl) | (mant_t(expBits(fl) != 0) << nbmant)),
	sign(toBits(fl) >> (sizeof(fl) * 8 - 1)),
	iszero((toBits(fl) & ((mant_t(1) << (sizeof(mant_t)*8-1))-1)) == 0),
	isinf((exp == ((1 << nbexp) - 2)) && mant == (mant_t(1) << nbmant)),
	isnan((exp == ((1 << nbexp) - 2)) && mant != (mant_t(1) << nbmant)) {
	static_assert(sizeof(floattype) == sizeof(mant_t));
      }

      template<typename floattype>
      explicit constexpr operator floattype() const {
	static_assert(sizeof(floattype) == sizeof(mant_t));
	mant_t m = mant & ((mant_t(1) << nbmant) - 1);
	m |= mant_t(sign) << (sizeof(mant_t) * 8 - 1);
	m |= mant_t(exp + ((mant >> nbmant) != 0)) << nbmant;
	return fromBits<floattype>(m);
      }

      template<typename dsttype>
      constexpr dsttype cast(const dsttype *ptr) const {
	typedef decltype(dsttype::mant) dmant_t;
	if constexpr (dsttype::nbmant_() > nbmant) {
	  int e = exp - expoffset() + dsttype::expoffset();
	  const int x = clz(mant) - (sizeof(mant_t) * 8 - nbmant - 1);
	  e -= x;
	  dmant_t m = dmant_t(mant) << (dsttype::nbmant_() - nbmant + x);
	  if (isinf || isnan) e = (1 << dsttype::nbexp_()) - 2;
	  if (iszero) e = 0;
	  return dsttype(m, e, sign, iszero, isinf, isnan);
	} else if constexpr (dsttype::nbmant_() < nbmant) {
	  bool isinf_ = isinf, sb = false;
	  int e = exp - expoffset() + dsttype::expoffset();
	  mant_t m = mant;
	  if constexpr (dsttype::nbexp_() != 0) {
	    if (e >= (1 << dsttype::nbexp_()) - 2) isinf_ = true;
	    if (e < 0) {
	      sb = (m & ((mant_t(1) << -e) - 1)) != 0;
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
	  return dsttype(dmant_t(m), e, sign, iszero_, isinf_, isnan);
	} else {
	  return dsttype(mant, exp, sign, iszero, isinf, isnan);
	}
      }

      template<typename dsttype, std::enable_if_t<(std::is_integral_v<dsttype> && sizeof(dsttype) <= 8), int> = 0>
      constexpr dsttype castToInt(const dsttype *ptr) const {
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
      constexpr BigInt<N> castToInt(const BigInt<N> *ptr) const {
	if (exp - expoffset() < -1) return 0;
	if (exp - expoffset() >= int(sizeof(BigInt<N>)*8-2))
	  return BigInt<N>(BigInt<N>(1) << (sizeof(BigInt<N>)*8-1));
	int s = nbmant - (exp - expoffset() + 1);
	BigInt<N> r = BigInt<N>(s > 0 ? BigUInt<N>(mant >> s) : (BigUInt<N>(mant) << -s));
	return sign ? -r : r;
      }

      template<int N>
      constexpr BigUInt<N> castToInt(const BigUInt<N> *ptr) const {
	if (exp - expoffset() < -1) return 0;
	if (sign && !iszero) return ~BigUInt<N>(0);
	if (exp - expoffset() >= int(sizeof(BigUInt<N>)*8-1)) return ~BigUInt<N>(0);
	int s = nbmant - (exp - expoffset() + 1);
	return s > 0 ? BigUInt<N>(mant >> s) : (BigUInt<N>(mant) << -s);
      }

      template<typename srctype, std::enable_if_t<(std::is_integral_v<srctype> && sizeof(srctype) <= 8), int> = 0>
      static constexpr UnpackedFloat castFromInt(srctype src) {
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
	  sb = (((longmant_t(1) << (-s)) - 1) & v) != 0;
	  u = v >> -s;
	}
	u += (((u >> (sizeof(mant_t) * 8)) & 1) || sb) + (longmant_t(1) << (sizeof(mant_t) * 8 - 1)) - 1;
	if (bit(u, nbmant + sizeof(mant_t) * 8 + 1)) { u >>= 1; x++; }
	u >>= sizeof(mant_t) * 8;
	return UnpackedFloat(mant_t(u), x + expoffset() - 2, sign, u == 0, false, false);
      }

      template<int N>
      static constexpr UnpackedFloat castFromInt(BigInt<N> src) {
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
	  sb = (((longmant_t(1) << (-s)) - 1) & v) != 0;
	  u = longmant_t(v >> -s);
	}
	u += (((u >> (sizeof(mant_t) * 8)) & 1) || sb) + (longmant_t(1) << (sizeof(mant_t) * 8 - 1)) - 1;
	if (bit(u, nbmant + sizeof(mant_t) * 8 + 1)) { u >>= 1; x++; }
	u >>= sizeof(mant_t) * 8;
	return UnpackedFloat(mant_t(u), x + expoffset() - 2, sign, u == 0, false, false);
      }

      template<int N>
      static constexpr UnpackedFloat castFromInt(BigUInt<N> src) {
	if (src == 0) return zero();
	BigUInt<N> v = src;
	int x = sizeof(v) * 8 - clz(v);
	int s = nbmant - (x - 1) + int(sizeof(mant_t)) * 8;
	bool sb = false;
	longmant_t u = 0;
	if (s >= 0) {
	  u = longmant_t(v) << s;
	} else {
	  sb = (((longmant_t(1) << (-s)) - 1) & v) != 0;
	  u = longmant_t(v >> -s);
	}
	u += (((u >> (sizeof(mant_t) * 8)) & 1) || sb) + (longmant_t(1) << (sizeof(mant_t) * 8 - 1)) - 1;
	if (bit(u, nbmant + sizeof(mant_t) * 8 + 1)) { u >>= 1; x++; }
	u >>= sizeof(mant_t) * 8;
	return UnpackedFloat(mant_t(u), x + expoffset() - 2, false, u == 0, false, false);
      }

#if defined(TLFLOAT_COMPILER_SUPPORTS_INT128) && !defined(__CUDA_ARCH__)
      constexpr __int128_t  castToInt(const __int128_t  *ptr) const { return __int128_t (castToInt((BigInt<7>  *)0)); }
      constexpr __uint128_t castToInt(const __uint128_t *ptr) const { return __uint128_t(castToInt((BigUInt<7> *)0)); }
      static constexpr UnpackedFloat castFromInt(__int128_t  src) { return castFromInt(BigInt <7>(src)); }
      static constexpr UnpackedFloat castFromInt(__uint128_t src) { return castFromInt(BigUInt<7>(src)); }
#endif

      friend constexpr UnpackedFloat trunc(const UnpackedFloat &f) {
	if (f.iszero || f.isinf || f.isnan) return f;
	if (f.exp >= expoffset() + nbmant) return f;
	if (f.exp < expoffset() - 1) return zero(f.sign);
	int s = nbmant - (f.exp - expoffset() + 1);
	UnpackedFloat r = f;
	r.mant &= ~((mant_t(1) << s) - 1);
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
	if (r.sign) r.mant += (mant_t(1) << s) - 1;
	r.mant &= ~((mant_t(1) << s) - 1);
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
	if (!r.sign) r.mant += (mant_t(1) << s) - 1;
	r.mant &= ~((mant_t(1) << s) - 1);
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
	l += bit(l, s1) + (longmant_t(1) << (s1 - 1)) - 1;
	l -= ~l >> (sizeof(l) * 8 - 1);
	mant_t m = mant_t(l >> s1);
	if (bit(m, nbmant + 1)) { m >>= 1; x++; }
	return UnpackedFloat(mant_t(m), (x + f.expoffset() + offset * 2 + 1) / 2 - offset - 1, false, m == 0, false, false);
      }

      friend constexpr bool isint(const UnpackedFloat &f) {
	if (f.isinf || f.isnan) return false;
	return rint(f) == f;
      }

      friend constexpr bool iseven(const UnpackedFloat &f) {
	return isint(ldexp_(f, -1));
      }

      friend constexpr UnpackedFloat toward0(UnpackedFloat f) {
	if (f.iszero || f.isinf || f.isnan) return f;
	f.mant--;
	if (!((f.mant >> nbmant) & 1)) {
	  f.exp--;
	  f.mant <<= 1;
	}
	return f;
      }

      //

      constexpr UnpackedFloat operator+() const { return UnpackedFloat(*this); }
      constexpr UnpackedFloat operator-() const { return UnpackedFloat(mant, exp, !sign, iszero, isinf, isnan); }
      constexpr UnpackedFloat operator+(const UnpackedFloat& rhs) const { return addsub(*this, rhs, false); }
      constexpr UnpackedFloat operator-(const UnpackedFloat& rhs) const { return addsub(*this, rhs, true); }
      constexpr UnpackedFloat operator*(const UnpackedFloat& rhs) const { return mul(*this, rhs); }
      constexpr UnpackedFloat operator/(const UnpackedFloat& rhs) const { return div(*this, rhs); }

      constexpr bool operator==(const UnpackedFloat& rhs) const {
	if (isnan || rhs.isnan) return false;
	return cmp(*this, rhs) == 0;
      }

      constexpr bool operator!=(const UnpackedFloat& rhs) const {
	return !((*this) == rhs);
      }

      constexpr bool operator>(const UnpackedFloat& rhs) const {
	if (isnan || rhs.isnan) return false;
	return cmp(*this, rhs) == 1;
      }

      constexpr bool operator<(const UnpackedFloat& rhs) const {
	if (isnan || rhs.isnan) return false;
	return cmp(*this, rhs) == -1;
      }

      constexpr bool operator<=(const UnpackedFloat& rhs) const {
	if (isnan || rhs.isnan) return false;
	return cmp(*this, rhs) != 1;
      }

      constexpr bool operator>=(const UnpackedFloat& rhs) const {
	if (isnan || rhs.isnan) return false;
	return cmp(*this, rhs) != -1;
      }

      //

      template<typename rhstype>
      constexpr UnpackedFloat operator+(const rhstype& rhs) const { return *this + UnpackedFloat(rhs); }
      template<typename rhstype>
      constexpr UnpackedFloat operator-(const rhstype& rhs) const { return *this - UnpackedFloat(rhs); }
      template<typename rhstype>
      constexpr UnpackedFloat operator*(const rhstype& rhs) const { return *this * UnpackedFloat(rhs); }
      template<typename rhstype>
      constexpr UnpackedFloat operator/(const rhstype& rhs) const { return *this / UnpackedFloat(rhs); }

      template<typename rhstype>
      constexpr bool operator==(const rhstype& rhs) const { return *this == UnpackedFloat(rhs); }
      template<typename rhstype>
      constexpr bool operator!=(const rhstype& rhs) const { return *this != UnpackedFloat(rhs); }
      template<typename rhstype>
      constexpr bool operator> (const rhstype& rhs) const { return *this >  UnpackedFloat(rhs); }
      template<typename rhstype>
      constexpr bool operator< (const rhstype& rhs) const { return *this <  UnpackedFloat(rhs); }
      template<typename rhstype>
      constexpr bool operator>=(const rhstype& rhs) const { return *this >= UnpackedFloat(rhs); }
      template<typename rhstype>
      constexpr bool operator<=(const rhstype& rhs) const { return *this <= UnpackedFloat(rhs); }

      template<typename srctype>
      constexpr UnpackedFloat& operator=(const srctype& s) {
	UnpackedFloat n(s);
	*this = n;
	return *this;
      }

      template<typename rhstype>
      constexpr UnpackedFloat& operator+=(const rhstype& rhs) { *this = *this + UnpackedFloat(rhs); return *this; }
      template<typename rhstype>
      constexpr UnpackedFloat& operator-=(const rhstype& rhs) { *this = *this - UnpackedFloat(rhs); return *this; }
      template<typename rhstype>
      constexpr UnpackedFloat& operator*=(const rhstype& rhs) { *this = *this * UnpackedFloat(rhs); return *this; }
      template<typename rhstype>
      constexpr UnpackedFloat& operator/=(const rhstype& rhs) { *this = *this / UnpackedFloat(rhs); return *this; }

      //

      static constexpr UnpackedFloat exp10i(int a) {
	bool negative = a < 0;
	if (negative) a = -a;

	UnpackedFloat r = castFromInt(1), d = castFromInt(10);
	for(;a;a>>=1) {
	  if (a & 1) r *= d;
	  d *= d;
	}

	return negative ? castFromInt(1) / r : r;
      }

      friend constexpr int ilog10(const UnpackedFloat& a) { // returns floor(log10(a))
	int ret = rint(a.ilogb() * LOG10_2);
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

	  UnpackedFloat rounder = value.iszero ? UnpackedFloat::zero() : ldexp_(UnpackedFloat::exp10i(-precision), -1);

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
    typedef detail::UnpackedFloat<uint32_t, uint64_t, 8, 23> ufloat;
    typedef detail::UnpackedFloat<uint64_t, BigUInt<7>, 11, 52> udouble;
    typedef detail::UnpackedFloat<BigUInt<7>, BigUInt<8>, 15, 112> uquad;
    typedef detail::UnpackedFloat<BigUInt<8>, BigUInt<9>, 19, 236> uoctuple;

    typedef decltype(Unpacked_t::mant) mant_t;
    typedef decltype(Unpacked_t::xUnpackedFloat()) xUnpacked_t;

  public:
    mant_t m = 0;

    /** Returns NaN */
    static constexpr TLFloat nan() { return Unpacked_t::nan(); }

    /** Returns infinity of the given sign */
    static constexpr TLFloat inf(bool sign=false) { return Unpacked_t::inf(sign); }

    /** Returns the minimum representative number of the given sign */
    static constexpr TLFloat floatdenormmin(bool sign=false) { return Unpacked_t::floatdenormmin(sign); }

    /** Returns the maximum representative number of the given sign */
    static constexpr TLFloat floatmax(bool sign=false) { return Unpacked_t::floatmax(sign); }

    /** Returns zero of the given sign */
    static constexpr TLFloat zero(bool sign=false) { return Unpacked_t::zero(sign); }

    constexpr TLFloat() = default;
    constexpr TLFloat(const TLFloat&) = default;
    constexpr TLFloat& operator=(const TLFloat&) = default;

    constexpr Unpacked_t getUnpacked() const { return Unpacked_t(m); }

    template<typename mant_t_, typename longmant_t_, int nbexp_, int nbmant_>
    constexpr TLFloat(const detail::UnpackedFloat<mant_t_, longmant_t_, nbexp_, nbmant_> &tf) :
      m(mant_t(tf.cast((Unpacked_t *)0))) {}

    /** Cast from an instance of any TLFloat class */
    template<typename srctftype>
    constexpr TLFloat(const TLFloat<srctftype>& s) :
      TLFloat(s.getUnpacked().cast((const Unpacked_t *)nullptr)) {}

    /** Cast from any primitive integer type */
    template<typename inttype, std::enable_if_t<(std::is_integral_v<inttype>), int> = 0>
    constexpr TLFloat(const inttype x) :
      TLFloat(Unpacked_t::castFromInt(x)) {}

    /** Cast to any primitive integer type */
    template<typename inttype, std::enable_if_t<(std::is_integral_v<inttype>), int> = 0>
    constexpr operator inttype() const {
      return getUnpacked().castToInt((const inttype *)nullptr);
    }

    template<int N> constexpr TLFloat(const BigInt <N> x) : TLFloat(Unpacked_t::castFromInt(x)) {}
    template<int N> constexpr TLFloat(const BigUInt<N> x) : TLFloat(Unpacked_t::castFromInt(x)) {}

    template<int N> constexpr operator BigInt <N>() const { return getUnpacked().castToInt((const BigInt <N> *)nullptr); }
    template<int N> constexpr operator BigUInt<N>() const { return getUnpacked().castToInt((const BigUInt<N> *)nullptr); }

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
      std::enable_if_t<(sizeof(fptype)==sizeof(mant_t) && (std::is_floating_point_v<fptype> || (!std::is_pointer_v<fptype> && !std::is_integral_v<fptype>))), int> = 0>
    constexpr TLFloat(const fptype& s) {
      m = std::bit_cast<mant_t>(s);
    }

    template<typename fptype,
      std::enable_if_t<(sizeof(fptype)==sizeof(mant_t) && (std::is_floating_point_v<fptype> || (!std::is_pointer_v<fptype> && !std::is_integral_v<fptype>))), int> = 0>
    explicit constexpr operator fptype() const {
      static_assert(sizeof(mant_t) == sizeof(fptype));
      return std::bit_cast<fptype>(m);
    }

    /** This works like strtod */
    constexpr TLFloat(const char *ptr, const char **endptr=nullptr) {
      *this = TLFloat(xUnpacked_t(ptr, endptr).cast((Unpacked_t *)nullptr));
    }

    explicit constexpr operator Unpacked_t() const { return Unpacked_t(m); }

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

    constexpr TLFloat operator-() const {
      TLFloat n(*this);
      n.m ^= mant_t(1) << (sizeof(mant_t)*8-1);
      return n;
    }

    constexpr TLFloat operator+() const { return TLFloat(*this); }

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
    friend constexpr bool signbit(const TLFloat& u) {
      TLFloat n(u);
      return Unpacked_t::bit(n.m, sizeof(mant_t) * 8 - 1);
    }
    friend constexpr bool iszero(const TLFloat& u) { return u.getUnpacked().iszero; }
    friend constexpr bool isint(const TLFloat& u) { return isint(u.getUnpacked()); }
    friend constexpr bool iseven(const TLFloat& u) { return iseven(u.getUnpacked()); }

    /** This function performs the fused multiply-add operation of floating point numbers. This function returns correctly rounded results. */
    friend constexpr TLFloat fma(const TLFloat& x, const TLFloat& y, const TLFloat& z) {
      return Unpacked_t::fma(x.getUnpacked(), y.getUnpacked(), z.getUnpacked());
    }

    /** This function performs ordered comparison of two floating point numbers. */
    constexpr bool operator==(const TLFloat& rhs) const { return getUnpacked() == rhs.getUnpacked(); }

    /** This function performs ordered comparison of two floating point numbers. */
    constexpr bool operator!=(const TLFloat& rhs) const { return getUnpacked() != rhs.getUnpacked(); }

    /** This function performs ordered comparison of two floating point numbers. */
    constexpr bool operator> (const TLFloat& rhs) const { return getUnpacked() >  rhs.getUnpacked(); }

    /** This function performs ordered comparison of two floating point numbers. */
    constexpr bool operator< (const TLFloat& rhs) const { return getUnpacked() <  rhs.getUnpacked(); }

    /** This function performs ordered comparison of two floating point numbers. */
    constexpr bool operator>=(const TLFloat& rhs) const { return getUnpacked() >= rhs.getUnpacked(); }

    /** This function performs ordered comparison of two floating point numbers. */
    constexpr bool operator<=(const TLFloat& rhs) const { return getUnpacked() <= rhs.getUnpacked(); }

    //

    template<typename srctype>
    constexpr TLFloat& operator=(const srctype& s) {
      TLFloat n(s);
      *this = n;
      return *this;
    }

    template<typename rhstype>
    constexpr TLFloat& operator+=(const rhstype& rhs) { *this = *this + TLFloat(rhs); return *this; }
    template<typename rhstype>
    constexpr TLFloat& operator-=(const rhstype& rhs) { *this = *this - TLFloat(rhs); return *this; }
    template<typename rhstype>
    constexpr TLFloat& operator*=(const rhstype& rhs) { *this = *this * TLFloat(rhs); return *this; }
    template<typename rhstype>
    constexpr TLFloat& operator/=(const rhstype& rhs) { *this = *this / TLFloat(rhs); return *this; }

    //

    // Double + int, Double + float, Double + Float
    template<typename rhstype, std::enable_if_t<(std::is_integral_v<rhstype> || sizeof(rhstype) <= sizeof(mant_t)), int> = 0>
    constexpr TLFloat operator+(const rhstype& rhs) const { return *this + TLFloat(rhs); }
    template<typename rhstype, std::enable_if_t<(std::is_integral_v<rhstype> || sizeof(rhstype) <= sizeof(mant_t)), int> = 0>
    constexpr TLFloat operator-(const rhstype& rhs) const { return *this - TLFloat(rhs); }
    template<typename rhstype, std::enable_if_t<(std::is_integral_v<rhstype> || sizeof(rhstype) <= sizeof(mant_t)), int> = 0>
    constexpr TLFloat operator*(const rhstype& rhs) const { return *this * TLFloat(rhs); }
    template<typename rhstype, std::enable_if_t<(std::is_integral_v<rhstype> || sizeof(rhstype) <= sizeof(mant_t)), int> = 0>
    constexpr TLFloat operator/(const rhstype& rhs) const { return *this / TLFloat(rhs); }

    // int + Double, float + Double, Float + Double
    template<typename lhstype, std::enable_if_t<(std::is_integral_v<lhstype> || sizeof(lhstype) <= sizeof(mant_t)), int> = 0>
    friend constexpr TLFloat operator+(const lhstype& lhs, const TLFloat &rhs) { return TLFloat(lhs) + rhs; }
    template<typename lhstype, std::enable_if_t<(std::is_integral_v<lhstype> || sizeof(lhstype) <= sizeof(mant_t)), int> = 0>
    friend constexpr TLFloat operator-(const lhstype& lhs, const TLFloat &rhs) { return TLFloat(lhs) - rhs; }
    template<typename lhstype, std::enable_if_t<(std::is_integral_v<lhstype> || sizeof(lhstype) <= sizeof(mant_t)), int> = 0>
    friend constexpr TLFloat operator*(const lhstype& lhs, const TLFloat &rhs) { return TLFloat(lhs) * rhs; }
    template<typename lhstype, std::enable_if_t<(std::is_integral_v<lhstype> || sizeof(lhstype) <= sizeof(mant_t)), int> = 0>
    friend constexpr TLFloat operator/(const lhstype& lhs, const TLFloat &rhs) { return TLFloat(lhs) / rhs; }

    // Double == int, Double == float, Double == Float
    template<typename rhstype, std::enable_if_t<(std::is_integral_v<rhstype> || sizeof(rhstype) <= sizeof(mant_t)), int> = 0>
    constexpr bool operator==(const rhstype& rhs) const { return *this == TLFloat(rhs); }
    template<typename rhstype, std::enable_if_t<(std::is_integral_v<rhstype> || sizeof(rhstype) <= sizeof(mant_t)), int> = 0>
    constexpr bool operator!=(const rhstype& rhs) const { return *this != TLFloat(rhs); }
    template<typename rhstype, std::enable_if_t<(std::is_integral_v<rhstype> || sizeof(rhstype) <= sizeof(mant_t)), int> = 0>
    constexpr bool operator> (const rhstype& rhs) const { return *this >  TLFloat(rhs); }
    template<typename rhstype, std::enable_if_t<(std::is_integral_v<rhstype> || sizeof(rhstype) <= sizeof(mant_t)), int> = 0>
    constexpr bool operator< (const rhstype& rhs) const { return *this <  TLFloat(rhs); }
    template<typename rhstype, std::enable_if_t<(std::is_integral_v<rhstype> || sizeof(rhstype) <= sizeof(mant_t)), int> = 0>
    constexpr bool operator>=(const rhstype& rhs) const { return *this >= TLFloat(rhs); }
    template<typename rhstype, std::enable_if_t<(std::is_integral_v<rhstype> || sizeof(rhstype) <= sizeof(mant_t)), int> = 0>
    constexpr bool operator<=(const rhstype& rhs) const { return *this <= TLFloat(rhs); }

    // int == Double, float == Double, Float == Double
    template<typename lhstype, std::enable_if_t<(std::is_integral_v<lhstype> || sizeof(lhstype) <= sizeof(mant_t)), int> = 0>
    friend constexpr bool operator==(const lhstype& lhs, const TLFloat &rhs) { return TLFloat(lhs) == rhs; }
    template<typename lhstype, std::enable_if_t<(std::is_integral_v<lhstype> || sizeof(lhstype) <= sizeof(mant_t)), int> = 0>
    friend constexpr bool operator!=(const lhstype& lhs, const TLFloat &rhs) { return TLFloat(lhs) != rhs; }
    template<typename lhstype, std::enable_if_t<(std::is_integral_v<lhstype> || sizeof(lhstype) <= sizeof(mant_t)), int> = 0>
    friend constexpr bool operator> (const lhstype& lhs, const TLFloat &rhs) { return TLFloat(lhs) >  rhs; }
    template<typename lhstype, std::enable_if_t<(std::is_integral_v<lhstype> || sizeof(lhstype) <= sizeof(mant_t)), int> = 0>
    friend constexpr bool operator< (const lhstype& lhs, const TLFloat &rhs) { return TLFloat(lhs) <  rhs; }
    template<typename lhstype, std::enable_if_t<(std::is_integral_v<lhstype> || sizeof(lhstype) <= sizeof(mant_t)), int> = 0>
    friend constexpr bool operator>=(const lhstype& lhs, const TLFloat &rhs) { return TLFloat(lhs) >= rhs; }
    template<typename lhstype, std::enable_if_t<(std::is_integral_v<lhstype> || sizeof(lhstype) <= sizeof(mant_t)), int> = 0>
    friend constexpr bool operator<=(const lhstype& lhs, const TLFloat &rhs) { return TLFloat(lhs) <= rhs; }

    //

    /** This function has the same functionality as the corresponding function in math.h. */
    friend constexpr int32_t ilogb(const TLFloat& f) {
      auto u = f.getUnpacked();
      if (u.isinf) return 2147483647;
      if (u.isnan) return FP_ILOGBNAN;
      if (u.exp != 0) return u.exp - Unpacked_t::expoffset() + 1;
      if (u.mant == 0) return FP_ILOGB0;
      return (sizeof(mant_t) * 8 - Unpacked_t::nbmant_()) - Unpacked_t::clz(u.mant) - Unpacked_t::expoffset();
    }

    friend constexpr TLFloat ldexp_(const TLFloat& x, const int e) {
      auto f = x.getUnpacked();
      f.exp += e;
      return f;
    }

    friend constexpr detail::xpair<TLFloat, int> frexp(const TLFloat& f) {
      auto p = frexp_(f.getUnpacked().cast((xUnpacked_t *)0));
      return detail::xpair<TLFloat, int> { (TLFloat)p.first.cast((Unpacked_t *)0), p.second };
    }

    /** This function has the same functionality as the corresponding function in math.h. */
    friend constexpr TLFloat frexp(const TLFloat& f, int *exp) {
      auto p = frexp_(f.getUnpacked().cast((xUnpacked_t *)0));
      if (exp) *exp = p.second;
      return (TLFloat)p.first.cast((Unpacked_t *)0);
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
      if (xx.isinf || yy.isinf) return inf(false);
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
    std::vector<char> buf(1000);
    snprint(buf.data(), buf.size(), u, 'g', 0, d, false, false, false, false, false, false);
    return std::string(buf.data());
  }
#endif

  /** This works like sprintf(str, "%.6g", *this) where the precision can be specified with d */
  template<typename Unpacked_t>
  static std::string to_string(TLFloat<Unpacked_t> a, int d=6) {
    std::vector<char> buf(1000);
    snprint(buf.data(), buf.size(), a.getUnpacked(), 'g', 0, d, false, false, false, false, false, false);
    return std::string(buf.data());
  }

  template<typename Unpacked_t>
  static std::ostream& operator<<(std::ostream &os, const TLFloat<Unpacked_t>& u) { return os << to_string(u); }
#endif // #if defined(TLFLOAT_DOXYGEN) || !defined(TLFLOAT_NO_LIBSTDCXX)

  //

  typedef TLFloat<detail::UnpackedFloat<uint16_t, uint32_t, 5, 10>> Half; ///< This class represents a half-precision IEEE 754 floating-point number. The data size and data structure of the objects are the same as the corresponding floating-point number.
  typedef TLFloat<detail::UnpackedFloat<uint32_t, uint64_t, 8, 23>> Float; ///< This class represents a single-precision IEEE 754 floating-point number. The data size and data structure of the objects are the same as the corresponding floating-point number.
  typedef TLFloat<detail::UnpackedFloat<uint64_t, BigUInt<7>, 11, 52>> Double; ///< This class represents a double-precision IEEE 754 floating-point number. The data size and data structure of the objects are the same as the corresponding floating-point number.
  typedef TLFloat<detail::UnpackedFloat<BigUInt<7>, BigUInt<8>, 15, 112>> Quad; ///< This class represents a quadruple-precision IEEE 754 floating-point number. The data size and data structure of the objects are the same as the corresponding floating-point number.
  typedef TLFloat<detail::UnpackedFloat<BigUInt<8>, BigUInt<9>, 19, 236>> Octuple; ///< This class represents a octuple-precision IEEE 754 floating-point number. The data size and data structure of the objects are the same as the corresponding floating-point number.
} // namespace tlfloat
#endif // #ifndef __TLFLOAT_HPP_INCLUDED__
