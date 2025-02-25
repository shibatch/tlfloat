#include <cstring>
#include <cctype>
#include <cstdarg>

#include "suppress.hpp"

#define TLFLOAT_NO_LIBSTDCXX
#include "tlfloat/tlmath.hpp"

#include "tlfloat/tlfloat.h"

using namespace tlfloat;

extern "C" {
  uint64_t tlfloat_version() {
    return TLFLOAT_VERSION_MAJOR * 1000000ULL + TLFLOAT_VERSION_MINOR * 1000ULL + TLFLOAT_VERSION_PATCH;
  }

  //

  double tlfloat_cast_d_q(const tlfloat_quad_    x) { return (double)Double(Quad(x)); }
  double tlfloat_cast_d_o(const tlfloat_octuple_ x) { return (double)Double(Octuple(x)); }

  tlfloat_quad_ tlfloat_cast_q_d_(const double           x) { return (tlfloat_quad_)Quad(Double(x)); }
  tlfloat_quad_ tlfloat_cast_q_o (const tlfloat_octuple_ x) { return (tlfloat_quad_)Quad(Octuple(x)); }

  tlfloat_octuple_ tlfloat_cast_o_d_(const double        x) { return (tlfloat_octuple_)Octuple(Double(x)); }
  tlfloat_octuple_ tlfloat_cast_o_q (const tlfloat_quad_ x) { return (tlfloat_octuple_)Octuple(Quad(x)); }

  int64_t tlfloat_cast_i64_q(const tlfloat_quad_  x) { return (int64_t)Quad(x); }
  tlfloat_quad_ tlfloat_cast_q_i64_(const int64_t x) { return (tlfloat_quad_)Quad(x); }

  int64_t tlfloat_cast_i64_o(const tlfloat_octuple_  x) { return (int64_t)Octuple(x); }
  tlfloat_octuple_ tlfloat_cast_o_i64_(const int64_t x) { return (tlfloat_octuple_)Octuple(x); }

  uint64_t tlfloat_cast_u64_q(const tlfloat_quad_  x) { return (uint64_t)Quad(x); }
  tlfloat_quad_ tlfloat_cast_q_u64_(const uint64_t x) { return (tlfloat_quad_)Quad(x); }

  uint64_t tlfloat_cast_u64_o(const tlfloat_octuple_  x) { return (uint64_t)Octuple(x); }
  tlfloat_octuple_ tlfloat_cast_o_u64_(const uint64_t x) { return (tlfloat_octuple_)Octuple(x); }

  //

  int tlfloat_eq_q_q(const tlfloat_quad_ x, const tlfloat_quad_ y) { return Quad(x) == Quad(y); }
  int tlfloat_ne_q_q(const tlfloat_quad_ x, const tlfloat_quad_ y) { return Quad(x) != Quad(y); }
  int tlfloat_lt_q_q(const tlfloat_quad_ x, const tlfloat_quad_ y) { return Quad(x) <  Quad(y); }
  int tlfloat_le_q_q(const tlfloat_quad_ x, const tlfloat_quad_ y) { return Quad(x) <= Quad(y); }
  int tlfloat_gt_q_q(const tlfloat_quad_ x, const tlfloat_quad_ y) { return Quad(x) >  Quad(y); }
  int tlfloat_ge_q_q(const tlfloat_quad_ x, const tlfloat_quad_ y) { return Quad(x) >= Quad(y); }

  int tlfloat_eq_o_o(const tlfloat_octuple_ x, const tlfloat_octuple_ y) { return Octuple(x) == Octuple(y); }
  int tlfloat_ne_o_o(const tlfloat_octuple_ x, const tlfloat_octuple_ y) { return Octuple(x) != Octuple(y); }
  int tlfloat_lt_o_o(const tlfloat_octuple_ x, const tlfloat_octuple_ y) { return Octuple(x) <  Octuple(y); }
  int tlfloat_le_o_o(const tlfloat_octuple_ x, const tlfloat_octuple_ y) { return Octuple(x) <= Octuple(y); }
  int tlfloat_gt_o_o(const tlfloat_octuple_ x, const tlfloat_octuple_ y) { return Octuple(x) >  Octuple(y); }
  int tlfloat_ge_o_o(const tlfloat_octuple_ x, const tlfloat_octuple_ y) { return Octuple(x) >= Octuple(y); }

  //

  tlfloat_int128_t_ tlfloat_cast_i128_i64_(const int64_t x) { return std::bit_cast<tlfloat_int128_t_>(BigInt<7>(x)); }
  int64_t tlfloat_cast_i64_i128(const tlfloat_int128_t_ x) { return (int64_t)std::bit_cast<BigInt<7>>(x); }

  tlfloat_uint128_t_ tlfloat_cast_u128_u64_(const uint64_t x) { return std::bit_cast<tlfloat_uint128_t_>(BigUInt<7>(x)); }
  uint64_t tlfloat_cast_u64_u128(const tlfloat_uint128_t_ x) { return (uint64_t)std::bit_cast<BigUInt<7>>(x); }

  tlfloat_int128_t_ tlfloat_cast_i128_d_(const double x) { return std::bit_cast<tlfloat_int128_t_>(BigInt<7>(x)); }
  double tlfloat_cast_d_i128(const tlfloat_int128_t_ x) { return (double)std::bit_cast<BigInt<7>>(x); }

  tlfloat_uint128_t_ tlfloat_cast_u128_d_(const double x) { return std::bit_cast<tlfloat_uint128_t_>(BigUInt<7>(x)); }
  double tlfloat_cast_d_u128(const tlfloat_uint128_t_ x) { return (double)std::bit_cast<BigUInt<7>>(x); }

  tlfloat_int128_t_ tlfloat_cast_i128_q(const tlfloat_quad_ x) { return std::bit_cast<tlfloat_int128_t_>(BigInt<7>((Quad)x)); }
  tlfloat_quad_ tlfloat_cast_q_i128(const tlfloat_int128_t_ x) { return (tlfloat_quad_)(Quad)std::bit_cast<BigInt<7>>(x); }

  tlfloat_uint128_t_ tlfloat_cast_u128_q(const tlfloat_quad_ x) { return std::bit_cast<tlfloat_uint128_t_>(BigUInt<7>((Quad)x)); }
  tlfloat_quad_ tlfloat_cast_q_u128(const tlfloat_uint128_t_ x) { return (tlfloat_quad_)(Quad)std::bit_cast<BigUInt<7>>(x); }

  tlfloat_int128_t_ tlfloat_cast_i128_o(const tlfloat_octuple_ x) { return std::bit_cast<tlfloat_int128_t_>(BigInt<7>((Octuple)x)); }
  tlfloat_octuple_ tlfloat_cast_o_i128(const tlfloat_int128_t_ x) { return (tlfloat_octuple_)(Octuple)std::bit_cast<BigInt<7>>(x); }

  tlfloat_uint128_t_ tlfloat_cast_u128_o(const tlfloat_octuple_ x) { return std::bit_cast<tlfloat_uint128_t_>(BigUInt<7>((Octuple)x)); }
  tlfloat_octuple_ tlfloat_cast_o_u128(const tlfloat_uint128_t_ x) { return (tlfloat_octuple_)(Octuple)std::bit_cast<BigUInt<7>>(x); }

  int tlfloat_eq_i128_i128(const tlfloat_int128_t_ x, const tlfloat_int128_t_ y) { return std::bit_cast<BigInt<7>>(x) == std::bit_cast<BigInt<7>>(y); }
  int tlfloat_ne_i128_i128(const tlfloat_int128_t_ x, const tlfloat_int128_t_ y) { return std::bit_cast<BigInt<7>>(x) != std::bit_cast<BigInt<7>>(y); }
  int tlfloat_lt_i128_i128(const tlfloat_int128_t_ x, const tlfloat_int128_t_ y) { return std::bit_cast<BigInt<7>>(x) <  std::bit_cast<BigInt<7>>(y); }
  int tlfloat_le_i128_i128(const tlfloat_int128_t_ x, const tlfloat_int128_t_ y) { return std::bit_cast<BigInt<7>>(x) <= std::bit_cast<BigInt<7>>(y); }
  int tlfloat_gt_i128_i128(const tlfloat_int128_t_ x, const tlfloat_int128_t_ y) { return std::bit_cast<BigInt<7>>(x) >  std::bit_cast<BigInt<7>>(y); }
  int tlfloat_ge_i128_i128(const tlfloat_int128_t_ x, const tlfloat_int128_t_ y) { return std::bit_cast<BigInt<7>>(x) >= std::bit_cast<BigInt<7>>(y); }

  tlfloat_int128_t_ tlfloat_add_i128_i128(const tlfloat_int128_t_ x, const tlfloat_int128_t_ y) {
    return std::bit_cast<tlfloat_int128_t_>(std::bit_cast<BigInt<7>>(x) + std::bit_cast<BigInt<7>>(y));
  }
  tlfloat_int128_t_ tlfloat_sub_i128_i128(const tlfloat_int128_t_ x, const tlfloat_int128_t_ y) {
    return std::bit_cast<tlfloat_int128_t_>(std::bit_cast<BigInt<7>>(x) - std::bit_cast<BigInt<7>>(y));
  }
  tlfloat_int128_t_ tlfloat_mul_i128_i128(const tlfloat_int128_t_ x, const tlfloat_int128_t_ y) {
    return std::bit_cast<tlfloat_int128_t_>(std::bit_cast<BigInt<7>>(x) * std::bit_cast<BigInt<7>>(y));
  }
  tlfloat_int128_t_ tlfloat_div_i128_i128(const tlfloat_int128_t_ x, const tlfloat_int128_t_ y) {
    return std::bit_cast<tlfloat_int128_t_>(std::bit_cast<BigInt<7>>(x) / std::bit_cast<BigInt<7>>(y));
  }
  tlfloat_int128_t_ tlfloat_mod_i128_i128(const tlfloat_int128_t_ x, const tlfloat_int128_t_ y) {
    return std::bit_cast<tlfloat_int128_t_>(std::bit_cast<BigInt<7>>(x) % std::bit_cast<BigInt<7>>(y));
  }
  tlfloat_int128_t_ tlfloat_and_i128_i128(const tlfloat_int128_t_ x, const tlfloat_int128_t_ y) {
    return std::bit_cast<tlfloat_int128_t_>(std::bit_cast<BigInt<7>>(x) & std::bit_cast<BigInt<7>>(y));
  }
  tlfloat_int128_t_ tlfloat_or_i128_i128(const tlfloat_int128_t_ x, const tlfloat_int128_t_ y) {
    return std::bit_cast<tlfloat_int128_t_>(std::bit_cast<BigInt<7>>(x) | std::bit_cast<BigInt<7>>(y));
  }
  tlfloat_int128_t_ tlfloat_xor_i128_i128(const tlfloat_int128_t_ x, const tlfloat_int128_t_ y) {
    return std::bit_cast<tlfloat_int128_t_>(std::bit_cast<BigInt<7>>(x) ^ std::bit_cast<BigInt<7>>(y));
  }
  tlfloat_int128_t_ tlfloat_not_i128(const tlfloat_int128_t_ x) {
    return std::bit_cast<tlfloat_int128_t_>(~std::bit_cast<BigInt<7>>(x));
  }
  tlfloat_int128_t_ tlfloat_shl_i128_i(const tlfloat_int128_t_ x, const int y) {
    return std::bit_cast<tlfloat_int128_t_>(std::bit_cast<BigInt<7>>(x) << y);
  }
  tlfloat_int128_t_ tlfloat_shr_i128_i(const tlfloat_int128_t_ x, const int y) {
    return std::bit_cast<tlfloat_int128_t_>(std::bit_cast<BigInt<7>>(x) >> y);
  }
  tlfloat_int128_t_ tlfloat_neg_i128(const tlfloat_int128_t_ x) {
    return std::bit_cast<tlfloat_int128_t_>(-std::bit_cast<BigInt<7>>(x));
  }

  int tlfloat_eq_u128_u128(const tlfloat_uint128_t_ x, const tlfloat_uint128_t_ y) { return std::bit_cast<BigUInt<7>>(x) == std::bit_cast<BigUInt<7>>(y); }
  int tlfloat_ne_u128_u128(const tlfloat_uint128_t_ x, const tlfloat_uint128_t_ y) { return std::bit_cast<BigUInt<7>>(x) != std::bit_cast<BigUInt<7>>(y); }
  int tlfloat_lt_u128_u128(const tlfloat_uint128_t_ x, const tlfloat_uint128_t_ y) { return std::bit_cast<BigUInt<7>>(x) <  std::bit_cast<BigUInt<7>>(y); }
  int tlfloat_le_u128_u128(const tlfloat_uint128_t_ x, const tlfloat_uint128_t_ y) { return std::bit_cast<BigUInt<7>>(x) <= std::bit_cast<BigUInt<7>>(y); }
  int tlfloat_gt_u128_u128(const tlfloat_uint128_t_ x, const tlfloat_uint128_t_ y) { return std::bit_cast<BigUInt<7>>(x) >  std::bit_cast<BigUInt<7>>(y); }
  int tlfloat_ge_u128_u128(const tlfloat_uint128_t_ x, const tlfloat_uint128_t_ y) { return std::bit_cast<BigUInt<7>>(x) >= std::bit_cast<BigUInt<7>>(y); }

  tlfloat_uint128_t_ tlfloat_add_u128_u128(const tlfloat_uint128_t_ x, const tlfloat_uint128_t_ y) {
    return std::bit_cast<tlfloat_uint128_t_>(std::bit_cast<BigUInt<7>>(x) + std::bit_cast<BigUInt<7>>(y));
  }
  tlfloat_uint128_t_ tlfloat_sub_u128_u128(const tlfloat_uint128_t_ x, const tlfloat_uint128_t_ y) {
    return std::bit_cast<tlfloat_uint128_t_>(std::bit_cast<BigUInt<7>>(x) - std::bit_cast<BigUInt<7>>(y));
  }
  tlfloat_uint128_t_ tlfloat_mul_u128_u128(const tlfloat_uint128_t_ x, const tlfloat_uint128_t_ y) {
    return std::bit_cast<tlfloat_uint128_t_>(std::bit_cast<BigUInt<7>>(x) * std::bit_cast<BigUInt<7>>(y));
  }
  tlfloat_uint128_t_ tlfloat_div_u128_u128(const tlfloat_uint128_t_ x, const tlfloat_uint128_t_ y) {
    return std::bit_cast<tlfloat_uint128_t_>(std::bit_cast<BigUInt<7>>(x) / std::bit_cast<BigUInt<7>>(y));
  }
  tlfloat_uint128_t_ tlfloat_mod_u128_u128(const tlfloat_uint128_t_ x, const tlfloat_uint128_t_ y) {
    return std::bit_cast<tlfloat_uint128_t_>(std::bit_cast<BigUInt<7>>(x) % std::bit_cast<BigUInt<7>>(y));
  }
  tlfloat_uint128_t_ tlfloat_and_u128_u128(const tlfloat_uint128_t_ x, const tlfloat_uint128_t_ y) {
    return std::bit_cast<tlfloat_uint128_t_>(std::bit_cast<BigUInt<7>>(x) & std::bit_cast<BigUInt<7>>(y));
  }
  tlfloat_uint128_t_ tlfloat_or_u128_u128(const tlfloat_uint128_t_ x, const tlfloat_uint128_t_ y) {
    return std::bit_cast<tlfloat_uint128_t_>(std::bit_cast<BigUInt<7>>(x) | std::bit_cast<BigUInt<7>>(y));
  }
  tlfloat_uint128_t_ tlfloat_xor_u128_u128(const tlfloat_uint128_t_ x, const tlfloat_uint128_t_ y) {
    return std::bit_cast<tlfloat_uint128_t_>(std::bit_cast<BigUInt<7>>(x) ^ std::bit_cast<BigUInt<7>>(y));
  }
  tlfloat_uint128_t_ tlfloat_not_u128(const tlfloat_uint128_t_ x) {
    return std::bit_cast<tlfloat_uint128_t_>(~std::bit_cast<BigUInt<7>>(x));
  }
  tlfloat_uint128_t_ tlfloat_shl_u128_i(const tlfloat_uint128_t_ x, const int y) {
    return std::bit_cast<tlfloat_uint128_t_>(std::bit_cast<BigUInt<7>>(x) << y);
  }
  tlfloat_uint128_t_ tlfloat_shr_u128_i(const tlfloat_uint128_t_ x, const int y) {
    return std::bit_cast<tlfloat_uint128_t_>(std::bit_cast<BigUInt<7>>(x) >> y);
  }

  //

  float tlfloat_cbrtf(const float a) { return (float)cbrt(Float(a)); }
  double tlfloat_cbrt(const double a) { return (double)cbrt(Double(a)); }
  tlfloat_quad_ tlfloat_cbrtq(const tlfloat_quad_ a) { return (tlfloat_quad_)cbrt(Quad(a)); }
  tlfloat_octuple_ tlfloat_cbrto(const tlfloat_octuple_ a) { return (tlfloat_octuple_)cbrt(Octuple(a)); }

  float tlfloat_fmodf(const float x, const float y) { return (float)fmod(Float(x), Float(y)); }
  double tlfloat_fmod(const double x, const double y) { return (double)fmod(Double(x), Double(y)); }
  tlfloat_quad_ tlfloat_fmodq(const tlfloat_quad_ x, const tlfloat_quad_ y) { return (tlfloat_quad_)fmod(Quad(x), Quad(y)); }
  tlfloat_octuple_ tlfloat_fmodo(const tlfloat_octuple_ x, const tlfloat_octuple_ y) { return (tlfloat_octuple_)fmod(Octuple(x), Octuple(y)); }

  float tlfloat_remainderf(const float x, const float y) { return (float)remainder(Float(x), Float(y)); }
  double tlfloat_remainder(const double x, const double y) { return (double)remainder(Double(x), Double(y)); }
  tlfloat_quad_ tlfloat_remainderq(const tlfloat_quad_ x, const tlfloat_quad_ y) { return (tlfloat_quad_)remainder(Quad(x), Quad(y)); }
  tlfloat_octuple_ tlfloat_remaindero(const tlfloat_octuple_ x, const tlfloat_octuple_ y) { return (tlfloat_octuple_)remainder(Octuple(x), Octuple(y)); }

  float tlfloat_remquof(const float x, const float y, int *quo) {
    auto p = remquo(Float(x), Float(y));
    if (quo) *quo = p.second & 0xffffffff;
    return (float)p.first;
  }
  double tlfloat_remquo(const double x, const double y, int *quo) {
    auto p = remquo(Double(x), Double(y));
    if (quo) *quo = p.second & 0xffffffff;
    return (double)p.first;
  }
  tlfloat_quad_ tlfloat_remquoq(const tlfloat_quad_ x, const tlfloat_quad_ y, int *quo) {
    auto p = remquo(Quad(x), Quad(y));
    if (quo) *quo = p.second & 0xffffffff;
    return (tlfloat_quad_)p.first;
  }
  tlfloat_octuple_ tlfloat_remquoo(const tlfloat_octuple_ x, const tlfloat_octuple_ y, int *quo) {
    auto p = remquo(Octuple(x), Octuple(y));
    if (quo) *quo = p.second & 0xffffffff;
    return (tlfloat_octuple_)p.first;
  }
}
