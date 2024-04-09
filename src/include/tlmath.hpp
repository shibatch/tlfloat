#ifndef __TLMATH_HPP_INCLUDED__
#define __TLMATH_HPP_INCLUDED__

#include "tlfloat.hpp"
#include "tlmathcoef.hpp"
#include "rpitab.hpp"

#if defined(__clang_major__) && !defined(_MSC_VER)
#if __clang_major__ < 17
#warning Unsupported clang version
#endif
#endif

namespace tlfloat {
  namespace detail {
    typedef UnpackedFloat<uint16_t, uint32_t, 0, 14> xhalf;
    typedef UnpackedFloat<uint32_t, uint64_t, 0, 30> xfloat;
    typedef UnpackedFloat<uint64_t, BigUInt<7>, 0, 62> xdouble;
    typedef UnpackedFloat<BigUInt<7>, BigUInt<8>, 0, 126> xquad;
    typedef UnpackedFloat<BigUInt<8>, BigUInt<9>, 0, 254> xoctuple;
    typedef UnpackedFloat<BigUInt<9>, BigUInt<10>, 0, 510> xsedecuple;

    //

    template<typename Unpacked_t>
    static consteval Unpacked_t constSqrt2O2() { return ldexp_(sqrt_(Unpacked_t::castFromInt(2)), -1); }

    template<typename Unpacked_t>
    static consteval Unpacked_t constOneOthree() { return Unpacked_t::castFromInt(1) / Unpacked_t::castFromInt(3); }

    template<typename T>
    static consteval T constPI() {
      return T("0x1.921fb54442d18469898cc51701b839a252049c1114cf98e804177d4c76273644ap+1");
    }

    template<typename T>
    static consteval T constLN2() {
      return T("0x1.62e42fefa39ef35793c7673007e5ed5e81e6864ce5316c5b141a2eb71755f457cf70ec40dbd75930ab2aa5f695f43621da5d5c6b827042884eae765222d37048p-1");
    }

    template<typename T>
    static consteval T upper(T v) {
      typedef decltype(T::mant) mant_t;
      v.mant &= ~((mant_t(1) << (sizeof(mant_t)*8/2)) - 1);
      return v;
    }

    template<typename T>
    static consteval T constLN2U() { return upper(constLN2<T>()); }

    template<typename T>
    static consteval T constLN2L() {
      return (constLN2<xsedecuple>() - constLN2U<T>().cast((const xsedecuple *)nullptr)).cast((const T *)nullptr);
    }

    template<typename T>
    static consteval T constLN10() {
      return T("0x1.26bb1bbb5551582dd4adac5705a61451c51fd9f3b4bbf21d078c3d0403e05ae52c5e05af635020b998e1978058be1ad058c448308217c5b5f71ef10805ca2dacp+1");
    }

    template<typename T>
    static consteval T constRLN2() {
      return T("0x1.71547652b82fe1777d0ffda0d23a7d11d6aef551bad2b4b1164a2cd9a342648fbc3887eeaa2ed9ac49b25eeb82d7c167d52173cc1895213f897f5e06a7be7368p+0");
    }

    template<typename T>
    static consteval T constRLN10() {
      return T("0x1.bcb7b1526e50e32a6ab7555f5a67b8647dc68c048b934404747e5a89ef1d4a78ea849df42805e7e52446ab258c8c942a319c77b3fa71b978df4571a5919b4f6p-2");
    }

    template<typename T>
    static consteval T constCbrt2() {
      return T("0x1.428a2f98d728ae223ddab715be250d0c288f10291631fbc061800cc36fa2dd3a6p+0");
    }

    template<typename T>
    static consteval T constCbrt4() {
      return T("0x1.965fea53d6e3c82b05999ab43dc4def1980762158a0a815f2291ac0cf93041e58p+0");
    }

    //

    template<typename Unpacked_t, unsigned N>
    static consteval xarray<Unpacked_t, N+1> genSinCoef() {
      xarray<Unpacked_t, N+1> a;
      for(unsigned k=0;k<N+1;k++) a.e[k] = Unpacked_t(sinCoefStr[k]);
      return a;
    }

    template<typename Unpacked_t, unsigned N>
    static consteval xarray<Unpacked_t, N+2> genCosCoef() {
      xarray<Unpacked_t, N+2> a;
      for(unsigned k=0;k<N+2;k++) a.e[k] = Unpacked_t(cosCoefStr[k]);
      return a;
    }

    template<typename Unpacked_t, unsigned N>
    static consteval xarray<Unpacked_t, N> genTanCoef() {
      xarray<Unpacked_t, N> a;
      for(unsigned k=0;k<N;k++) a.e[k] = Unpacked_t(tanCoefStr[k]);
      return a;
    }

    template<typename Unpacked_t, unsigned N>
    static consteval xarray<Unpacked_t, N+1> genAtanCoef() {
      xarray<Unpacked_t, N+1> a;
      for(unsigned k=0;k<N+1;k++) a.e[k] = Unpacked_t(atanCoefStr[k]);
      return a;
    }

    template<typename Unpacked_t, unsigned N>
    static consteval xarray<Unpacked_t, N+1> genExpCoef() {
      xarray<Unpacked_t, N+1> a;
      for(unsigned k=0;k<N+1;k++) a.e[k] = Unpacked_t(expCoefStr[k]);
      return a;
    }

    template<typename Unpacked_t, unsigned N>
    static consteval xarray<Unpacked_t, N+1> genLogCoef() {
      xarray<Unpacked_t, N+1> a;
      for(unsigned k=0;k<N+1;k++) a.e[k] = Unpacked_t(logCoefStr[k]);
      return a;
    }

    template<typename Unpacked_t, unsigned N>
    static consteval xarray<Unpacked_t, N+1> genCbrtCoef() {
      xarray<Unpacked_t, N+1> a;
      for(unsigned k=0;k<N+1;k++) a.e[k] = Unpacked_t(cbrtCoefStr[k]);
      return a;
    }

    //

    template<typename Unpacked_t>
    static constexpr xpair<Unpacked_t, int> ph(const Unpacked_t &ux, int p) {
      typedef decltype(ux.mant) mant_t;
      typedef decltype(Unpacked_t::longmant_t_()) longmant_t;

      if (ux.iszero || ux.exp < Unpacked_t::expoffset()-5) return xpair<Unpacked_t, int>(ux, 0);

      const int mantexp = ux.exp - Unpacked_t::expoffset() - Unpacked_t::nbmant_() + 1;
      const int m = (mantexp - 1) < 0 ? 0 : (mantexp - 1);

      int rpiexp = p - (m & ~7) - sizeof(mant_t) * 8;
      longmant_t rpih = 0, rpil = 0;
      for(unsigned i=0;i<sizeof(mant_t);i++) rpih = (rpih << 8) + tlfloat_rpitab[i                  + m/8];
      for(unsigned i=0;i<sizeof(mant_t);i++) rpil = (rpil << 8) + tlfloat_rpitab[i + sizeof(mant_t) + m/8];

      longmant_t rm = ux.mant * rpih + ((ux.mant * rpil) >> (sizeof(mant_t) * 8));
      int q = 0;
      if (-(mantexp + rpiexp) < (int)sizeof(longmant_t)*8) {
	longmant_t qm = (rm + (longmant_t(1) << (-(mantexp + rpiexp) - 1))) & (~longmant_t(0) << -(mantexp + rpiexp));
	rm -= qm;
	q = int(qm >> -(mantexp + rpiexp));
      }

      int exp = mantexp + rpiexp - p;
      for(unsigned j=sizeof(mant_t) * 2;Unpacked_t::clz(rm) > sizeof(mant_t) * 8 || Unpacked_t::clz(~rm) > sizeof(mant_t) * 8;j += sizeof(mant_t)) {
	rm = (rm << (sizeof(mant_t) * 8)) + ((ux.mant * rpil) & ((longmant_t(1) << (sizeof(mant_t) * 8)) - 1));
	rpil = 0;
	for(unsigned i=0;i<sizeof(mant_t);i++) rpil = (rpil << 8) + tlfloat_rpitab[i + j + m/8];
	rm += ((ux.mant * rpil) >> (sizeof(mant_t) * 8));
	exp -= sizeof(mant_t) * 8;
      }

      bool sign = ux.sign;
      if (rm >> (sizeof(rm) * 8 - 1)) { rm = -rm; q = -q; sign = !sign; }

      int x = sizeof(rm) * 8 - Unpacked_t::clz(rm);
      int s = Unpacked_t::nbmant_() - (x - 1);
      mant_t u = mant_t(s >= 0 ? (rm << s) : ((rm + (mant_t(1) << (-s - 1))) >> -s));
      if (Unpacked_t::bit(u, Unpacked_t::nbmant_() + 1)) { u >>= 1; x++; }
      Unpacked_t r = Unpacked_t(u, x + Unpacked_t::expoffset() - 2 + exp, false, false, false, false) * constPI<Unpacked_t>();

      return xpair<Unpacked_t, int>(sign ? -r : r, sign ? -q & 0xff : q & 0xff);
    }

    //

    template<typename Unpacked_t, unsigned N>
    static constexpr Unpacked_t sin_(const Unpacked_t& a) {
      const xpair<Unpacked_t, int> p = ph<Unpacked_t>(a, 1);

      Unpacked_t d = Unpacked_t::castFromInt(0);

      if (p.second & 1) {
	constexpr xarray<Unpacked_t, N+2> cosCoef = genCosCoef<Unpacked_t, N>();
	d = cosCoef.e[N+1];
	for(int i=N-0;i>=0;i--) d = Unpacked_t::fma(d, -p.first * p.first, cosCoef.e[i]);
      } else {
	constexpr xarray<Unpacked_t, N+1> sinCoef = genSinCoef<Unpacked_t, N>();
	d = sinCoef.e[N];
	for(int i=N-1;i>=0;i--) d = Unpacked_t::fma(d, -p.first * p.first, sinCoef.e[i]);
	d *= p.first;
      }

      return (p.second & 2) ? -d : d;
    }

    template<typename tlfloat_t, typename Unpacked_t, unsigned N>
    static constexpr tlfloat_t sin(const tlfloat_t& a) {
      if (isnan(a)) return a;
      if (isinf(a)) return tlfloat_t::nan();
      auto y = sin_<Unpacked_t, N>(a.getUnpacked().cast((const Unpacked_t *)nullptr));
      return tlfloat_t(y.cast((const decltype(a.getUnpacked()) *)nullptr));
    }

    template<typename Unpacked_t, unsigned N>
    static constexpr Unpacked_t cos_(const Unpacked_t& a) {
      const xpair<Unpacked_t, int> p = ph<Unpacked_t>(a, 1);

      Unpacked_t d = Unpacked_t::castFromInt(0);

      if (p.second & 1) {
	constexpr xarray<Unpacked_t, N+1> sinCoef = genSinCoef<Unpacked_t, N>();
	d = sinCoef.e[N];
	for(int i=N-1;i>=0;i--) d = Unpacked_t::fma(d, -p.first * p.first, sinCoef.e[i]);
	d *= -p.first;
      } else {
	constexpr xarray<Unpacked_t, N+2> cosCoef = genCosCoef<Unpacked_t, N>();
	d = cosCoef.e[N+1];
	for(int i=N-0;i>=0;i--) d = Unpacked_t::fma(d, -p.first * p.first, cosCoef.e[i]);
      }

      return (p.second & 2) ? -d : d;
    }

    template<typename tlfloat_t, typename Unpacked_t, unsigned N>
    static constexpr tlfloat_t cos(const tlfloat_t& a) {
      if (isnan(a)) return a;
      if (isinf(a)) return tlfloat_t::nan();
      auto y = cos_<Unpacked_t, N>(a.getUnpacked().cast((const Unpacked_t *)nullptr));
      return tlfloat_t(y.cast((const decltype(a.getUnpacked()) *)nullptr));
    }

    //

    template<typename Unpacked_t, unsigned N, unsigned M>
    static constexpr Unpacked_t tan_(const Unpacked_t& a) {
      const xpair<Unpacked_t, int> p = ph<Unpacked_t>(a, 1);

      Unpacked_t x = ldexp_(p.first, -1-int(M)), y;

      constexpr xarray<Unpacked_t, N> tanCoef = genTanCoef<Unpacked_t, N>();
      Unpacked_t d = tanCoef.e[N-1];
      for(int i=N-1;i>0;i--) d = Unpacked_t::fma(d, x * x, tanCoef.e[i-1]);
      d *= x;

      if constexpr (M > 0) {
	for(unsigned i=0;i<M;i++) {
	  y = Unpacked_t::fma(d, d, Unpacked_t::castFromInt(-1));
	  x = -ldexp_(d, 1);
	  d = x / y;
	}
      }

      y = Unpacked_t::fma(d, d, Unpacked_t::castFromInt(-1));
      x = -ldexp_(d, 1);

      if (p.second & 1) { d = x; x = y; y = -d; }

      return x / y;
    }

    template<typename tlfloat_t, typename Unpacked_t, unsigned N, unsigned M>
    static constexpr tlfloat_t tan(const tlfloat_t& a) {
      if (isnan(a)) return a;
      if (isinf(a)) return tlfloat_t::nan();
      auto y = tan_<Unpacked_t, N, M>(a.getUnpacked().cast((const Unpacked_t *)nullptr));
      return tlfloat_t(y.cast((const decltype(a.getUnpacked()) *)nullptr));
    }

    //

    template<typename Unpacked_t, int N, unsigned M>
    static constexpr Unpacked_t atan2_(Unpacked_t y, Unpacked_t x) {
      constexpr xarray<Unpacked_t, N+1> atanCoef = genAtanCoef<Unpacked_t, N>();
      int q = 0;
      if (fabs(y) > fabs(x)) {
	Unpacked_t t = x; x = y; y = -t;
	q = x.sign ? -1 : 1;
      } else if (x.sign) {
	q = y.sign? -2 : 2;
      }

      Unpacked_t d = y / x, r = atanCoef.e[N];

      for(unsigned i=0;i<M;i++) {
	d = d / (sqrt_(Unpacked_t::fma(d, d, Unpacked_t::castFromInt(1))) + Unpacked_t::castFromInt(1));
      }

      for(int i=N-1;i>=0;i--) r = Unpacked_t::fma(r, d * d, atanCoef.e[i]);
      r *= d;

      Unpacked_t qu = Unpacked_t::castFromInt(q);
      if (q == 0) qu.sign = r.sign;
      if (x.iszero) { r = Unpacked_t::castFromInt(0); qu.sign = r.sign = y.sign; }

      return Unpacked_t::fma(qu, ldexp_(constPI<Unpacked_t>(), -1),  ldexp_(r, M));
    }

    template<typename tlfloat_t, typename Unpacked_t, unsigned N, unsigned M>
    static constexpr tlfloat_t atan2(tlfloat_t y, tlfloat_t x) {
      if (isnan(y)) return y;
      if (isnan(x)) return x;
      if (int(isinf(y)) | isinf(x)) {
	y = isinf(y) ? (signbit(y) ? -1 : 1) : (signbit(y) ? -0.0 : +0.0);
	x = isinf(x) ? (signbit(x) ? -1 : 1) : (signbit(x) ? -0.0 : +0.0);
      }

      Unpacked_t z = atan2_<Unpacked_t, N, M>(y.getUnpacked().cast((const Unpacked_t *)nullptr), x.getUnpacked().cast((const Unpacked_t *)nullptr));
      return tlfloat_t(z.cast((const decltype(x.getUnpacked()) *)nullptr));
    }

    template<typename tlfloat_t, typename Unpacked_t, unsigned N, unsigned M>
    static constexpr tlfloat_t atan(const tlfloat_t &a) {
      if (isnan(a)) return a;
      Unpacked_t y = isinf(a) ? Unpacked_t::castFromInt(signbit(a) ? -1 : 1) : a.getUnpacked().cast((const Unpacked_t *)nullptr);
      Unpacked_t x = Unpacked_t::castFromInt(isinf(a) ? 0 : 1);
      Unpacked_t z = atan2_<Unpacked_t, N, M>(y, x);
      return tlfloat_t(z.cast((const decltype(a.getUnpacked()) *)nullptr));
    }

    template<typename tlfloat_t, typename Unpacked_t, unsigned N, unsigned M>
    static constexpr tlfloat_t asin(const tlfloat_t &a) {
      if (isnan(a)) return a;
      if (fabs(a) > 1) return tlfloat_t::nan();
      Unpacked_t y = a.getUnpacked().cast((const Unpacked_t *)nullptr);
      Unpacked_t x = sqrt_((Unpacked_t::castFromInt(1) + y) * (Unpacked_t::castFromInt(1) - y));
      Unpacked_t z = atan2_<Unpacked_t, N, M>(y, x);
      return tlfloat_t(z.cast((const decltype(a.getUnpacked()) *)nullptr));
    }

    template<typename tlfloat_t, typename Unpacked_t, unsigned N, unsigned M>
    static constexpr tlfloat_t acos(const tlfloat_t &a) {
      if (isnan(a)) return a;
      if (fabs(a) > 1) return tlfloat_t::nan();
      Unpacked_t x = a.getUnpacked().cast((const Unpacked_t *)nullptr);
      Unpacked_t y = sqrt_((Unpacked_t::castFromInt(1) + x) * (Unpacked_t::castFromInt(1) - x));
      Unpacked_t z = atan2_<Unpacked_t, N, M>(y, x);
      return tlfloat_t(z.cast((const decltype(a.getUnpacked()) *)nullptr));
    }

    //

    template<typename Unpacked_t, unsigned N, unsigned M>
    static constexpr Unpacked_t exp_(Unpacked_t d) {
      constexpr xarray<Unpacked_t, N+2> expCoef = genExpCoef<Unpacked_t, N+1>();

      if (d.exp > 28) return d.sign ? Unpacked_t::castFromInt(0) : Unpacked_t::inf();

      int64_t q = rint(d * constRLN2<Unpacked_t>()).castToInt((int64_t *)nullptr);

      d = Unpacked_t::fma(Unpacked_t::castFromInt(-q), constLN2U<Unpacked_t>(), d);
      d = Unpacked_t::fma(Unpacked_t::castFromInt(-q), constLN2L<Unpacked_t>(), d);

      d = ldexp_(d, -M);

      Unpacked_t s = expCoef.e[N+1];
      for(int i=N;i>=0;i--) s = Unpacked_t::fma(s, d, expCoef.e[i]);

      if constexpr (M > 0) for(unsigned i=0;i<M;i++) s *= s;

      return ldexp_(s, q);
    }

    template<typename Unpacked_t, unsigned N, unsigned M>
    static constexpr Unpacked_t expm1_(Unpacked_t d) {
      constexpr xarray<Unpacked_t, N+2> expCoef = genExpCoef<Unpacked_t, N+1>();

      if (d.iszero) return d;
      if (d.exp > 28) return d.sign ? Unpacked_t::castFromInt(-1) : Unpacked_t::inf();

      int64_t q = rint(d * constRLN2<Unpacked_t>()).castToInt((int64_t *)nullptr);

      d = Unpacked_t::fma(Unpacked_t::castFromInt(-q), constLN2U<Unpacked_t>(), d);
      d = Unpacked_t::fma(Unpacked_t::castFromInt(-q), constLN2L<Unpacked_t>(), d);

      d = ldexp_(d, -M);

      Unpacked_t s = expCoef.e[N+1];
      for(int i=N;i>=1;i--) s = Unpacked_t::fma(s, d, expCoef.e[i]);
      s *= d;

      if constexpr (M > 0) for(unsigned i=0;i<M;i++) s = Unpacked_t::fma(s, s, ldexp_(s, 1));

      return ldexp_(s, q) + (ldexp_(Unpacked_t::castFromInt(1), q) - Unpacked_t::castFromInt(1));
    }

    template<typename tlfloat_t, typename Unpacked_t, unsigned N, unsigned M>
    static constexpr tlfloat_t exp(const tlfloat_t &a) {
      if (isnan(a)) return a;
      if (isinf(a) && signbit(a)) return 0;
      Unpacked_t z = exp_<Unpacked_t, N, M>(a.getUnpacked().cast((const Unpacked_t *)nullptr));
      return tlfloat_t(z.cast((const decltype(a.getUnpacked()) *)nullptr));
    }

    template<typename tlfloat_t, typename Unpacked_t, unsigned N, unsigned M>
    static constexpr tlfloat_t exp(const tlfloat_t &a, const Unpacked_t &SCALE) {
      if (isnan(a)) return a;
      if (isinf(a) && signbit(a)) return 0;
      Unpacked_t z = exp_<Unpacked_t, N, M>(a.getUnpacked().cast((const Unpacked_t *)nullptr) * SCALE);
      return tlfloat_t(z.cast((const decltype(a.getUnpacked()) *)nullptr));
    }

    template<typename tlfloat_t, typename Unpacked_t, unsigned N, unsigned M>
    static constexpr tlfloat_t expm1(const tlfloat_t &a) {
      if (isnan(a)) return a;
      if (isinf(a) && signbit(a)) return -1;
      Unpacked_t z = expm1_<Unpacked_t, N, M>(a.getUnpacked().cast((const Unpacked_t *)nullptr));
      return tlfloat_t(z.cast((const decltype(a.getUnpacked()) *)nullptr));
    }

    //

    template<typename Unpacked_t, unsigned N, unsigned M>
    static constexpr Unpacked_t log_(Unpacked_t d) {
      constexpr xarray<Unpacked_t, N+2> logCoef = genLogCoef<Unpacked_t, N+1>();

      xpair<Unpacked_t, int> p = frexp_(d);
      Unpacked_t fr = p.first;
      int e = p.second;

      if (fr < constSqrt2O2<Unpacked_t>()) { fr.exp++; e--; }

      if constexpr (M > 0) for(unsigned i=0;i<M;i++) fr = sqrt_(fr);

      Unpacked_t x = (fr - Unpacked_t::castFromInt(1)) / (fr + Unpacked_t::castFromInt(1));

      Unpacked_t y = logCoef.e[N+1];
      for(int i=N;i>=0;i--) y = Unpacked_t::fma(y, x*x, logCoef.e[i]);
      y *= x;

      y = Unpacked_t::fma(Unpacked_t::castFromInt(e), constLN2L<Unpacked_t>(), ldexp_(y, M));
      return Unpacked_t::fma(Unpacked_t::castFromInt(e), constLN2U<Unpacked_t>(), y);
    }

    template<typename tlfloat_t, typename Unpacked_t, unsigned N, unsigned M>
    static constexpr tlfloat_t log(const tlfloat_t &a) {
      if (isnan(a)) return a;
      if (a == 0) return tlfloat_t::inf(true);
      if (signbit(a)) return tlfloat_t::nan();
      if (isinf(a)) return tlfloat_t::inf();
      Unpacked_t z = log_<Unpacked_t, N, M>(a.getUnpacked().cast((const Unpacked_t *)nullptr));
      return tlfloat_t(z.cast((const decltype(a.getUnpacked()) *)nullptr));
    }

    template<typename tlfloat_t, typename Unpacked_t, unsigned N, unsigned M>
    static constexpr tlfloat_t log(const tlfloat_t &a, const Unpacked_t &SCALE) {
      if (isnan(a)) return a;
      if (a == 0) return tlfloat_t::inf(true);
      if (signbit(a)) return tlfloat_t::nan();
      if (isinf(a)) return tlfloat_t::inf();
      Unpacked_t z = log_<Unpacked_t, N, M>(a.getUnpacked().cast((const Unpacked_t *)nullptr)) * SCALE;
      return tlfloat_t(z.cast((const decltype(a.getUnpacked()) *)nullptr));
    }

    template<typename Unpacked_t, unsigned N, unsigned M>
    static constexpr Unpacked_t log1p_(Unpacked_t d) {
      constexpr xarray<Unpacked_t, N+2> logCoef = genLogCoef<Unpacked_t, N+1>();

      xpair<Unpacked_t, int> p = frexp_(d + Unpacked_t::castFromInt(1));
      Unpacked_t fr = p.first;
      int e = p.second;

      if (fr < constSqrt2O2<Unpacked_t>()) { fr.exp++; e--; }

      fr = e == 0 ? d : (fr - Unpacked_t::castFromInt(1));

      if constexpr (M > 0) for(unsigned i=0;i<M;i++) fr = fr / (sqrt_(fr + Unpacked_t::castFromInt(1)) + Unpacked_t::castFromInt(1));

      Unpacked_t x = fr / (fr + Unpacked_t::castFromInt(2));

      Unpacked_t y = logCoef.e[N+1];
      for(int i=N;i>=0;i--) y = Unpacked_t::fma(y, x*x, logCoef.e[i]);
      y *= x;

      y = Unpacked_t::fma(Unpacked_t::castFromInt(e), constLN2L<Unpacked_t>(), ldexp_(y, M));
      return Unpacked_t::fma(Unpacked_t::castFromInt(e), constLN2U<Unpacked_t>(), y);
    }

    template<typename tlfloat_t, typename Unpacked_t, unsigned N, unsigned M>
    static constexpr tlfloat_t log1p(const tlfloat_t &a) {
      if (isnan(a) || a == 0) return a;
      if (a == -1) return tlfloat_t::inf(true);
      if (a < -1) return tlfloat_t::nan();
      if (isinf(a)) return tlfloat_t::inf();
      Unpacked_t z = log1p_<Unpacked_t, N, M>(a.getUnpacked().cast((const Unpacked_t *)nullptr));
      return tlfloat_t(z.cast((const decltype(a.getUnpacked()) *)nullptr));
    }

    //

    template<typename tlfloat_t, typename Unpacked_t, unsigned LN, unsigned LM, unsigned EN, unsigned EM>
    static constexpr tlfloat_t pow(const tlfloat_t &x, const tlfloat_t &y) {
      if (signbit(x) && finite(x) && !iszero(x) && finite(y) && !isint(y)) return tlfloat_t::nan();
      if (x == 1 || iszero(y)) return 1;
      if (isnan(x)) return x;
      if (isnan(y)) return y;
      bool yisoddint = isint(y) && !iseven(y);
      if (!signbit(y) && !iszero(y) && iszero(x) && yisoddint) return tlfloat_t::zero(signbit(x));
      if (iszero(x) && !signbit(y) && !iszero(y) && !yisoddint) return 0;
      if (isinf(y) && x == -1) return 1;
      bool absxlt1 = -1 < x && x < 1;
      if (isinf(y) && signbit(y) && absxlt1) return tlfloat_t::inf();
      bool absxgt1 = x < -1 || 1 < x;
      if (isinf(y) && signbit(y) && absxgt1) return 0;
      if (isinf(y) && !signbit(y) && absxlt1) return 0;
      if (isinf(y) && !signbit(y) && absxgt1) return tlfloat_t::inf();
      if (isinf(x) && signbit(x) && yisoddint && signbit(y)) return tlfloat_t::zero(true);
      if (isinf(x) && signbit(x) && signbit(y) && !iszero(y) && !yisoddint) return 0;
      if (isinf(x) && signbit(x) && !signbit(y) && yisoddint) return tlfloat_t::inf(true);
      if (isinf(x) && signbit(x) && !signbit(y) && !iszero(y) && !yisoddint) return tlfloat_t::inf(false);
      if (isinf(x) && !signbit(x) && signbit(y) && !iszero(y)) return 0;
      if (isinf(x) && !signbit(x) && !signbit(y) && !iszero(y)) return tlfloat_t::inf(false);
      if (iszero(x) && yisoddint && signbit(y)) return tlfloat_t::inf(signbit(x));
      if (iszero(x) && signbit(y) && !iszero(y) && !yisoddint) return tlfloat_t::inf(false);
      
      Unpacked_t z = log_<Unpacked_t, LN, LM>(fabs(x).getUnpacked().cast((const Unpacked_t *)nullptr));
      z = exp_<Unpacked_t, EN, EM>(z * y.getUnpacked().cast((const Unpacked_t *)nullptr));
      if (signbit(x) && yisoddint) z.sign = !z.sign;
      return tlfloat_t(z.cast((const decltype(x.getUnpacked()) *)nullptr));
    }

    //

    template<typename Unpacked_t, unsigned N, unsigned M>
    static constexpr Unpacked_t cbrt_(Unpacked_t d) {
      const int eoffset = 1 << 20;
      constexpr xarray<Unpacked_t, N+2> cbrtCoef = genCbrtCoef<Unpacked_t, N+1>();

      int e = (d * constOneOthree<Unpacked_t>()).ilogb() + 2;
      d.exp -= e;
      int r = (e + eoffset * 3) % 3;
      Unpacked_t q = Unpacked_t::castFromInt(1);
      if (r == 1) q = constCbrt2<Unpacked_t>();
      if (r == 2) q = constCbrt4<Unpacked_t>();

      q.exp += (e + eoffset * 3) / 3 - eoffset;

      if (d.sign) q.sign = !q.sign;
      d.sign = false;

      Unpacked_t x = cbrtCoef.e[N+1], y;
      for(int i=N;i>=0;i--) x = Unpacked_t::fma(x, d - Unpacked_t::castFromInt(1), cbrtCoef.e[i]);

      if constexpr (M > 0) {
	for(unsigned i=0;i<M;i++) {
	  y = x * x;
	  y = y * y;
	  x = Unpacked_t::fma(Unpacked_t::fma(d, y, -x), -constOneOthree<Unpacked_t>(), x);
	}
      }
      y = x * x * d;

      return Unpacked_t::fma(Unpacked_t::fma(y, x, Unpacked_t::castFromInt(-1)), -ldexp_(constOneOthree<Unpacked_t>(), 1) * y, y) * q;
    }

    template<typename tlfloat_t, typename Unpacked_t, unsigned N, unsigned M>
    static constexpr tlfloat_t cbrt(const tlfloat_t &a) {
      if (iszero(a) || isinf(a) || isnan(a)) return a;
      Unpacked_t z = cbrt_<Unpacked_t, N, M>(a.getUnpacked().cast((const Unpacked_t *)nullptr));
      return tlfloat_t(z.cast((const decltype(a.getUnpacked()) *)nullptr));
    }

    //

    template<typename tlfloat_t, typename Unpacked_t, unsigned N, unsigned M>
    static constexpr tlfloat_t sinh(const tlfloat_t &a) {
      Unpacked_t x = a.getUnpacked().cast((const Unpacked_t *)nullptr), y = fabs(x);
      if (x.isnan || x.isinf) return a;
      if (y > Unpacked_t::castFromInt(1ULL << 20)) return tlfloat_t::inf(x.sign);
      Unpacked_t d = expm1_<Unpacked_t, N, M>(fabs(x));
      d = ldexp_(d * (d + Unpacked_t::castFromInt(2)) / (d + Unpacked_t::castFromInt(1)), -1);
      if (x.sign) d.sign = !d.sign;
      return tlfloat_t(d.cast((decltype(a.getUnpacked()) *)nullptr));
    }

    template<typename tlfloat_t, typename Unpacked_t, unsigned N, unsigned M>
    static constexpr tlfloat_t cosh(const tlfloat_t &a) {
      Unpacked_t x = a.getUnpacked().cast((const Unpacked_t *)nullptr), y = fabs(x);
      if (x.isnan) return a;
      if (x.iszero) return 1;
      if (y > Unpacked_t::castFromInt(1ULL << 20)) return tlfloat_t::inf();
      Unpacked_t d = exp_<Unpacked_t, N, M>(fabs(x));
      d = ldexp_(d + Unpacked_t::castFromInt(1) / d, -1);
      return tlfloat_t(d.cast((decltype(a.getUnpacked()) *)nullptr));
    }

    template<typename tlfloat_t, typename Unpacked_t, unsigned N, unsigned M>
    static constexpr tlfloat_t tanh(const tlfloat_t &a) {
      Unpacked_t x = a.getUnpacked().cast((const Unpacked_t *)nullptr), y = fabs(x);
      if (x.isnan) return a;
      if (y.isinf || y > Unpacked_t::castFromInt(1ULL << 20)) return x.sign ? -1 : 1;
      Unpacked_t d = expm1_<Unpacked_t, N, M>(ldexp_(y, 1));
      d = d / (d + Unpacked_t::castFromInt(2));
      if (x.sign) d.sign = !d.sign;
      return tlfloat_t(d.cast((decltype(a.getUnpacked()) *)nullptr));
    }

    template<typename tlfloat_t, typename Unpacked_t, unsigned N, unsigned M>
    static constexpr tlfloat_t asinh(const tlfloat_t &a) {
      Unpacked_t x = a.getUnpacked().cast((const Unpacked_t *)nullptr), y = fabs(x);
      if (x.isnan || y.isinf) return a;
      bool ygt1 = y > Unpacked_t::castFromInt(1);
      Unpacked_t d = ygt1 ? Unpacked_t::castFromInt(1) / y : y;
      d = sqrt_(Unpacked_t::fma(d, d, Unpacked_t::castFromInt(1)));
      if (ygt1) {
	d = log_<Unpacked_t, N, M>(Unpacked_t::fma(d, y, y));
      } else {
	d = log1p_<Unpacked_t, N, M>(d - Unpacked_t::castFromInt(1) + y);
      }
      if (x.sign) d.sign = !d.sign;
      return tlfloat_t(d.cast((decltype(a.getUnpacked()) *)nullptr));
    }

    template<typename tlfloat_t, typename Unpacked_t, unsigned N, unsigned M>
    static constexpr tlfloat_t acosh(const tlfloat_t &a) {
      Unpacked_t x = a.getUnpacked().cast((const Unpacked_t *)nullptr);
      if (x.isnan) return a;
      if (x < Unpacked_t::castFromInt(1)) return tlfloat_t::nan();
      if (x.isinf) return tlfloat_t::inf();
      Unpacked_t d = log_<Unpacked_t, N, M>(sqrt_((x + Unpacked_t::castFromInt(1))*(x - Unpacked_t::castFromInt(1))) + x);
      return tlfloat_t(d.cast((decltype(a.getUnpacked()) *)nullptr));
    }

    template<typename tlfloat_t, typename Unpacked_t, unsigned N, unsigned M>
    static constexpr tlfloat_t atanh(const tlfloat_t &a) {
      Unpacked_t x = a.getUnpacked().cast((const Unpacked_t *)nullptr), y = fabs(x);
      if (x.isnan) return a;
      if (x.isinf) return tlfloat_t::nan();
      if (y > Unpacked_t::castFromInt(1)) return tlfloat_t::nan();
      if (y == Unpacked_t::castFromInt(1)) return tlfloat_t::inf(x.sign);
      Unpacked_t d = ldexp_(log1p_<Unpacked_t, N, M>(ldexp_(y, 1) / (Unpacked_t::castFromInt(1) - y)), -1);
      if (x.sign) d.sign = !d.sign;
      return tlfloat_t(d.cast((decltype(a.getUnpacked()) *)nullptr));
    }

    //

    template<typename Unpacked_t>
    static constexpr Unpacked_t fmod_(const Unpacked_t &n, const Unpacked_t &d, const Unpacked_t &rd) {
      Unpacked_t r = n, d3 = d * Unpacked_t::castFromInt(3), d2 = ldexp_(d, 1);
      for(int i=0;i<(1 << 20);i++) {
	Unpacked_t q = trunc(toward0(r * rd));
	if (d3 > r && r >  d) q = Unpacked_t::castFromInt(2);
	if (d2 > r && r >= d) q = Unpacked_t::castFromInt(1);
	r = Unpacked_t::fma(q, -d, r);
	if (r < d) break;
      }
      return r;
    }

    template<typename tlfloat_t, typename Unpacked_t>
    static constexpr tlfloat_t fmod(const tlfloat_t &n, const tlfloat_t &d) {
      Unpacked_t un = n.getUnpacked().cast((Unpacked_t *)0);
      Unpacked_t ud = d.getUnpacked().cast((Unpacked_t *)0);
      if (un.isinf || ud.iszero) return tlfloat_t::nan();
      if (un.isnan || ud.isinf) return n;
      if (ud.isnan) return d;
      Unpacked_t ur = fmod_<Unpacked_t>(fabs(un), fabs(ud), toward0(Unpacked_t::castFromInt(1) / fabs(ud)));
      if (un.sign) ur.sign = !ur.sign;
      return tlfloat_t(ur.cast((decltype(n.getUnpacked()) *)nullptr));
    }

    template<typename Unpacked_t>
    static constexpr Unpacked_t remainder_(const Unpacked_t &n, const Unpacked_t &d, const Unpacked_t &rd) {
      Unpacked_t r = n, d15 = d * ldexp_(Unpacked_t::castFromInt(3), -1), d05 = ldexp_(d, -1);
      bool qisodd = false;
      for(int i=0;i<(1 << 20);i++) {
	Unpacked_t q = rint(r * rd);
	if (fabs(r) < d15) q = Unpacked_t::castFromInt(r.sign ? -1 : 1);
	if (fabs(r) < d05 || (!qisodd && fabs(r) == d05)) q = Unpacked_t::castFromInt(0);
	if (q.iszero) break;
	qisodd = qisodd != (isint(q) && !iseven(q));
	r = Unpacked_t::fma(q, -d, r);
      }
      return r;
    }

    template<typename tlfloat_t, typename Unpacked_t>
    static constexpr tlfloat_t remainder(const tlfloat_t &n, const tlfloat_t &d) {
      Unpacked_t un = n.getUnpacked().cast((Unpacked_t *)0);
      Unpacked_t ud = d.getUnpacked().cast((Unpacked_t *)0);
      if (un.isinf || ud.iszero) return tlfloat_t::nan();
      if (un.isnan || ud.isinf) return n;
      if (ud.isnan) return d;
      Unpacked_t ur = remainder_<Unpacked_t>(fabs(un), fabs(ud), toward0(Unpacked_t::castFromInt(1) / fabs(ud)));
      if (un.sign) ur.sign = !ur.sign;
      return tlfloat_t(ur.cast((decltype(n.getUnpacked()) *)nullptr));
    }
  }

  static inline constexpr Half sin(const Half& a) { return detail::sin<Half, detail::xhalf, 2>(a); }
  static inline constexpr Float sin(const Float& a) { return detail::sin<Float, detail::xfloat, 4>(a); }
  static inline constexpr Double sin(const Double& a) { return detail::sin<Double, detail::xdouble, 9>(a); }
  static inline constexpr Quad sin(const Quad& a) { return detail::sin<Quad, detail::xquad, 14>(a); }
  static inline constexpr Octuple sin(const Octuple& a) { return detail::sin<Octuple, detail::xoctuple, 25>(a); }

  static inline constexpr Half cos(const Half& a) { return detail::cos<Half, detail::xhalf, 2>(a); }
  static inline constexpr Float cos(const Float& a) { return detail::cos<Float, detail::xfloat, 4>(a); }
  static inline constexpr Double cos(const Double& a) { return detail::cos<Double, detail::xdouble, 9>(a); }
  static inline constexpr Quad cos(const Quad& a) { return detail::cos<Quad, detail::xquad, 14>(a); }
  static inline constexpr Octuple cos(const Octuple& a) { return detail::cos<Octuple, detail::xoctuple, 25>(a); }

  static inline constexpr Half tan(const Half& a) { return detail::tan<Half, detail::xhalf, 5, 0>(a); }
  static inline constexpr Float tan(const Float& a) { return detail::tan<Float, detail::xfloat, 9, 0>(a); }
  static inline constexpr Double tan(const Double& a) { return detail::tan<Double, detail::xdouble, 11, 1>(a); }
  static inline constexpr Quad tan(const Quad& a) { return detail::tan<Quad, detail::xquad, 16, 2>(a); }
  static inline constexpr Octuple tan(const Octuple& a) { return detail::tan<Octuple, detail::xoctuple, 25, 3>(a); }

  static inline constexpr Half atan2(const Half& y, const Half& x) { return detail::atan2<Half, detail::xhalf, 4, 1>(y, x); }
  static inline constexpr Float atan2(const Float& y, const Float& x) { return detail::atan2<Float, detail::xfloat, 9, 1>(y, x); }
  static inline constexpr Double atan2(const Double& y, const Double& x) { return detail::atan2<Double, detail::xdouble, 14, 2>(y, x); }
  static inline constexpr Quad atan2(const Quad& y, const Quad& x) { return detail::atan2<Quad, detail::xquad, 17, 3>(y, x); }
  static inline constexpr Octuple atan2(const Octuple& y, const Octuple& x) { return detail::atan2<Octuple, detail::xoctuple, 21, 6>(y, x); }

  static inline constexpr Half asin(const Half& a) { return detail::asin<Half, detail::xhalf, 4, 1>(a); }
  static inline constexpr Float asin(const Float& a) { return detail::asin<Float, detail::xfloat, 9, 1>(a); }
  static inline constexpr Double asin(const Double& a) { return detail::asin<Double, detail::xdouble, 14, 2>(a); }
  static inline constexpr Quad asin(const Quad& a) { return detail::asin<Quad, detail::xquad, 17, 3>(a); }
  static inline constexpr Octuple asin(const Octuple& a) { return detail::asin<Octuple, detail::xoctuple, 21, 6>(a); }

  static inline constexpr Half acos(const Half& a) { return detail::acos<Half, detail::xhalf, 4, 1>(a); }
  static inline constexpr Float acos(const Float& a) { return detail::acos<Float, detail::xfloat, 9, 1>(a); }
  static inline constexpr Double acos(const Double& a) { return detail::acos<Double, detail::xdouble, 14, 2>(a); }
  static inline constexpr Quad acos(const Quad& a) { return detail::acos<Quad, detail::xquad, 17, 3>(a); }
  static inline constexpr Octuple acos(const Octuple& a) { return detail::acos<Octuple, detail::xoctuple, 21, 6>(a); }

  static inline constexpr Half atan(const Half& a) { return detail::atan<Half, detail::xhalf, 4, 1>(a); }
  static inline constexpr Float atan(const Float& a) { return detail::atan<Float, detail::xfloat, 9, 1>(a); }
  static inline constexpr Double atan(const Double& a) { return detail::atan<Double, detail::xdouble, 14, 2>(a); }
  static inline constexpr Quad atan(const Quad& a) { return detail::atan<Quad, detail::xquad, 17, 3>(a); }
  static inline constexpr Octuple atan(const Octuple& a) { return detail::atan<Octuple, detail::xoctuple, 21, 6>(a); }

  static inline constexpr Half exp(const Half& a) { return detail::exp<Half, detail::xhalf, 3, 0>(a); }
  static inline constexpr Float exp(const Float& a) { return detail::exp<Float, detail::xfloat, 7, 0>(a); }
  static inline constexpr Double exp(const Double& a) { return detail::exp<Double, detail::xdouble, 13, 0>(a); }
  static inline constexpr Quad exp(const Quad& a) { return detail::exp<Quad, detail::xquad, 20, 1>(a); }
  static inline constexpr Octuple exp(const Octuple& a) { return detail::exp<Octuple, detail::xoctuple, 33, 2>(a); }

  static inline constexpr Half expm1(const Half& a) { return detail::expm1<Half, detail::xhalf, 3, 1>(a); }
  static inline constexpr Float expm1(const Float& a) { return detail::expm1<Float, detail::xfloat, 6, 1>(a); }
  static inline constexpr Double expm1(const Double& a) { return detail::expm1<Double, detail::xdouble, 12, 1>(a); }
  static inline constexpr Quad expm1(const Quad& a) { return detail::expm1<Quad, detail::xquad, 21, 1>(a); }
  static inline constexpr Octuple expm1(const Octuple& a) { return detail::expm1<Octuple, detail::xoctuple, 33, 2>(a); }

  static inline constexpr Half exp2(const Half& a) { return detail::exp<Half, detail::xfloat, 3, 0>(a, detail::constLN2<detail::xfloat>()); }
  static inline constexpr Float exp2(const Float& a) { return detail::exp<Float, detail::xdouble, 7, 0>(a, detail::constLN2<detail::xdouble>()); }
  static inline constexpr Double exp2(const Double& a) { return detail::exp<Double, detail::xquad, 13, 0>(a, detail::constLN2<detail::xquad>()); }
  static inline constexpr Quad exp2(const Quad& a) { return detail::exp<Quad, detail::xoctuple, 20, 1>(a, detail::constLN2<detail::xoctuple>()); }
  static inline constexpr Octuple exp2(const Octuple& a) { return detail::exp<Octuple, detail::xsedecuple, 33, 2>(a, detail::constLN2<detail::xsedecuple>()); }

  static inline constexpr Half exp10(const Half& a) { return detail::exp<Half, detail::xfloat, 3, 0>(a, detail::constLN10<detail::xfloat>()); }
  static inline constexpr Float exp10(const Float& a) { return detail::exp<Float, detail::xdouble, 7, 0>(a, detail::constLN10<detail::xdouble>()); }
  static inline constexpr Double exp10(const Double& a) { return detail::exp<Double, detail::xquad, 13, 0>(a, detail::constLN10<detail::xquad>()); }
  static inline constexpr Quad exp10(const Quad& a) { return detail::exp<Quad, detail::xoctuple, 20, 1>(a, detail::constLN10<detail::xoctuple>()); }
  static inline constexpr Octuple exp10(const Octuple& a) { return detail::exp<Octuple, detail::xsedecuple, 33, 2>(a, detail::constLN10<detail::xsedecuple>()); }

  static inline constexpr Half log(const Half& a) { return detail::log<Half, detail::xhalf, 3, 0>(a); }
  static inline constexpr Float log(const Float& a) { return detail::log<Float, detail::xfloat, 4, 0>(a); }
  static inline constexpr Double log(const Double& a) { return detail::log<Double, detail::xdouble, 10, 0>(a); }
  static inline constexpr Quad log(const Quad& a) { return detail::log<Quad, detail::xquad, 21, 0>(a); }
  static inline constexpr Octuple log(const Octuple& a) { return detail::log<Octuple, detail::xoctuple, 25, 2>(a); }

  static inline constexpr Half log1p(const Half& a) { return detail::log1p<Half, detail::xhalf, 3, 0>(a); }
  static inline constexpr Float log1p(const Float& a) { return detail::log1p<Float, detail::xfloat, 4, 0>(a); }
  static inline constexpr Double log1p(const Double& a) { return detail::log1p<Double, detail::xdouble, 10, 0>(a); }
  static inline constexpr Quad log1p(const Quad& a) { return detail::log1p<Quad, detail::xquad, 21, 0>(a); }
  static inline constexpr Octuple log1p(const Octuple& a) { return detail::log1p<Octuple, detail::xoctuple, 25, 2>(a); }

  static inline constexpr Half log2(const Half& a) { return detail::log<Half, detail::xhalf, 3, 0>(a, detail::constRLN2<detail::xhalf>()); }
  static inline constexpr Float log2(const Float& a) { return detail::log<Float, detail::xfloat, 4, 0>(a, detail::constRLN2<detail::xfloat>()); }
  static inline constexpr Double log2(const Double& a) { return detail::log<Double, detail::xdouble, 10, 0>(a, detail::constRLN2<detail::xdouble>()); }
  static inline constexpr Quad log2(const Quad& a) { return detail::log<Quad, detail::xquad, 22, 0>(a, detail::constRLN2<detail::xquad>()); }
  static inline constexpr Octuple log2(const Octuple& a) { return detail::log<Octuple, detail::xoctuple, 25, 2>(a, detail::constRLN2<detail::xoctuple>()); }

  static inline constexpr Half log10(const Half& a) { return detail::log<Half, detail::xhalf, 3, 0>(a, detail::constRLN10<detail::xhalf>()); }
  static inline constexpr Float log10(const Float& a) { return detail::log<Float, detail::xfloat, 4, 0>(a, detail::constRLN10<detail::xfloat>()); }
  static inline constexpr Double log10(const Double& a) { return detail::log<Double, detail::xdouble, 10, 0>(a, detail::constRLN10<detail::xdouble>()); }
  static inline constexpr Quad log10(const Quad& a) { return detail::log<Quad, detail::xquad, 22, 0>(a, detail::constRLN10<detail::xquad>()); }
  static inline constexpr Octuple log10(const Octuple& a) { return detail::log<Octuple, detail::xoctuple, 25, 2>(a, detail::constRLN10<detail::xoctuple>()); }

  static inline constexpr Half pow(const Half& x, const Half& y) { return detail::pow<Half, detail::xfloat, 3, 0, 3, 0>(x, y); }
  static inline constexpr Float pow(const Float& x, const Float& y) { return detail::pow<Float, detail::xdouble, 5, 0, 7, 0>(x, y); }
  static inline constexpr Double pow(const Double& x, const Double& y) { return detail::pow<Double, detail::xquad, 11, 0, 13, 0>(x, y); }
  static inline constexpr Quad pow(const Quad& x, const Quad& y) { return detail::pow<Quad, detail::xoctuple, 23, 0, 20, 1>(x, y); }
  static inline constexpr Octuple pow(const Octuple& x, const Octuple& y) { return detail::pow<Octuple, detail::xsedecuple, 26, 2, 33, 2>(x, y); }

  static inline constexpr Half cbrt(const Half& a) { return detail::cbrt<Half, detail::xhalf, 3, 0>(a); }
  static inline constexpr Float cbrt(const Float& a) { return detail::cbrt<Float, detail::xfloat, 4, 1>(a); }
  static inline constexpr Double cbrt(const Double& a) { return detail::cbrt<Double, detail::xdouble, 4, 2>(a); }
  static inline constexpr Quad cbrt(const Quad& a) { return detail::cbrt<Quad, detail::xquad, 4, 3>(a); }
  static inline constexpr Octuple cbrt(const Octuple& a) { return detail::cbrt<Octuple, detail::xoctuple, 4, 4>(a); }

  static inline constexpr Half sinh(const Half& a) { return detail::sinh<Half, detail::xhalf, 4, 0>(a); }
  static inline constexpr Float sinh(const Float& a) { return detail::sinh<Float, detail::xfloat, 7, 0>(a); }
  static inline constexpr Double sinh(const Double& a) { return detail::sinh<Double, detail::xdouble, 13, 0>(a); }
  static inline constexpr Quad sinh(const Quad& a) { return detail::sinh<Quad, detail::xquad, 20, 1>(a); }
  static inline constexpr Octuple sinh(const Octuple& a) { return detail::sinh<Octuple, detail::xoctuple, 33, 2>(a); }

  static inline constexpr Half cosh(const Half& a) { return detail::cosh<Half, detail::xhalf, 3, 0>(a); }
  static inline constexpr Float cosh(const Float& a) { return detail::cosh<Float, detail::xfloat, 7, 0>(a); }
  static inline constexpr Double cosh(const Double& a) { return detail::cosh<Double, detail::xdouble, 13, 0>(a); }
  static inline constexpr Quad cosh(const Quad& a) { return detail::cosh<Quad, detail::xquad, 20, 1>(a); }
  static inline constexpr Octuple cosh(const Octuple& a) { return detail::cosh<Octuple, detail::xoctuple, 33, 2>(a); }

  static inline constexpr Half tanh(const Half& a) { return detail::tanh<Half, detail::xhalf, 3, 0>(a); }
  static inline constexpr Float tanh(const Float& a) { return detail::tanh<Float, detail::xfloat, 7, 0>(a); }
  static inline constexpr Double tanh(const Double& a) { return detail::tanh<Double, detail::xdouble, 13, 0>(a); }
  static inline constexpr Quad tanh(const Quad& a) { return detail::tanh<Quad, detail::xquad, 20, 1>(a); }
  static inline constexpr Octuple tanh(const Octuple& a) { return detail::tanh<Octuple, detail::xoctuple, 33, 2>(a); }

  static inline constexpr Half asinh(const Half& a) { return detail::asinh<Half, detail::xfloat, 3, 0>(a); }
  static inline constexpr Float asinh(const Float& a) { return detail::asinh<Float, detail::xdouble, 7, 0>(a); }
  static inline constexpr Double asinh(const Double& a) { return detail::asinh<Double, detail::xquad, 13, 0>(a); }
  static inline constexpr Quad asinh(const Quad& a) { return detail::asinh<Quad, detail::xoctuple, 20, 1>(a); }
  static inline constexpr Octuple asinh(const Octuple& a) { return detail::asinh<Octuple, detail::xsedecuple, 33, 2>(a); }

  static inline constexpr Half acosh(const Half& a) { return detail::acosh<Half, detail::xfloat, 3, 0>(a); }
  static inline constexpr Float acosh(const Float& a) { return detail::acosh<Float, detail::xdouble, 7, 0>(a); }
  static inline constexpr Double acosh(const Double& a) { return detail::acosh<Double, detail::xquad, 13, 0>(a); }
  static inline constexpr Quad acosh(const Quad& a) { return detail::acosh<Quad, detail::xoctuple, 20, 1>(a); }
  static inline constexpr Octuple acosh(const Octuple& a) { return detail::acosh<Octuple, detail::xsedecuple, 33, 2>(a); }

  static inline constexpr Half atanh(const Half& a) { return detail::atanh<Half, detail::xhalf, 3, 0>(a); }
  static inline constexpr Float atanh(const Float& a) { return detail::atanh<Float, detail::xfloat, 4, 0>(a); }
  static inline constexpr Double atanh(const Double& a) { return detail::atanh<Double, detail::xdouble, 10, 0>(a); }
  static inline constexpr Quad atanh(const Quad& a) { return detail::atanh<Quad, detail::xquad, 21, 0>(a); }
  static inline constexpr Octuple atanh(const Octuple& a) { return detail::atanh<Octuple, detail::xoctuple, 25, 2>(a); }

  static inline constexpr Half fmod(const Half& y, const Half& x) { return detail::fmod<Half, detail::xhalf>(y, x); }
  static inline constexpr Float fmod(const Float& y, const Float& x) { return detail::fmod<Float, detail::xfloat>(y, x); }
  static inline constexpr Double fmod(const Double& y, const Double& x) { return detail::fmod<Double, detail::xdouble>(y, x); }
  static inline constexpr Quad fmod(const Quad& y, const Quad& x) { return detail::fmod<Quad, detail::xquad>(y, x); }
  static inline constexpr Octuple fmod(const Octuple& y, const Octuple& x) { return detail::fmod<Octuple, detail::xoctuple>(y, x); }

  static inline constexpr Half remainder(const Half& y, const Half& x) { return detail::remainder<Half, detail::xhalf>(y, x); }
  static inline constexpr Float remainder(const Float& y, const Float& x) { return detail::remainder<Float, detail::xfloat>(y, x); }
  static inline constexpr Double remainder(const Double& y, const Double& x) { return detail::remainder<Double, detail::xdouble>(y, x); }
  static inline constexpr Quad remainder(const Quad& y, const Quad& x) { return detail::remainder<Quad, detail::xquad>(y, x); }
  static inline constexpr Octuple remainder(const Octuple& y, const Octuple& x) { return detail::remainder<Octuple, detail::xoctuple>(y, x); }
}
#endif // #ifndef __TLMATH_HPP_INCLUDED__
