#include <cstring>
#include <cctype>
#include <cstdarg>

#include "suppress.hpp"

#define TLFLOAT_NO_LIBSTDCXX
#include "tlfloat/tlmath.hpp"

#include "tlfloat/tlfloat.h"

using namespace tlfloat;

extern "C" {
  float tlfloat_addf(const float x, const float y) { return (float)(Float(x) + Float(y)); }
  double tlfloat_add(const double x, const double y) { return (double)(Double(x) + Double(y)); }
  tlfloat_quad_ tlfloat_addq(const tlfloat_quad_ x, const tlfloat_quad_ y) { return (tlfloat_quad_)(Quad(x) + Quad(y)); }
  tlfloat_octuple_ tlfloat_addo(const tlfloat_octuple_ x, const tlfloat_octuple_ y) { return (tlfloat_octuple_)(Octuple(x) + Octuple(y)); }

  float tlfloat_subf(const float x, const float y) { return (float)(Float(x) - Float(y)); }
  double tlfloat_sub(const double x, const double y) { return (double)(Double(x) - Double(y)); }
  tlfloat_quad_ tlfloat_subq(const tlfloat_quad_ x, const tlfloat_quad_ y) { return (tlfloat_quad_)(Quad(x) - Quad(y)); }
  tlfloat_octuple_ tlfloat_subo(const tlfloat_octuple_ x, const tlfloat_octuple_ y) { return (tlfloat_octuple_)(Octuple(x) - Octuple(y)); }

  float tlfloat_mulf(const float x, const float y) { return (float)(Float(x) * Float(y)); }
  double tlfloat_mul(const double x, const double y) { return (double)(Double(x) * Double(y)); }
  tlfloat_quad_ tlfloat_mulq(const tlfloat_quad_ x, const tlfloat_quad_ y) { return (tlfloat_quad_)(Quad(x) * Quad(y)); }
  tlfloat_octuple_ tlfloat_mulo(const tlfloat_octuple_ x, const tlfloat_octuple_ y) { return (tlfloat_octuple_)(Octuple(x) * Octuple(y)); }

  float tlfloat_divf(const float x, const float y) { return (float)(Float(x) / Float(y)); }
  double tlfloat_div(const double x, const double y) { return (double)(Double(x) / Double(y)); }
  tlfloat_quad_ tlfloat_divq(const tlfloat_quad_ x, const tlfloat_quad_ y) { return (tlfloat_quad_)(Quad(x) / Quad(y)); }
  tlfloat_octuple_ tlfloat_divo(const tlfloat_octuple_ x, const tlfloat_octuple_ y) { return (tlfloat_octuple_)(Octuple(x) / Octuple(y)); }

  float tlfloat_fmaf(const float x, const float y, const float z) { return (float)fma(Float(x), Float(y), Float(z)); }
  double tlfloat_fma(const double x, const double y, const double z) { return (double)fma(Double(x), Double(y), Double(z)); }
  tlfloat_quad_ tlfloat_fmaq(const tlfloat_quad_ x, const tlfloat_quad_ y, const tlfloat_quad_ z) { return (tlfloat_quad_)fma(Quad(x), Quad(y), Quad(z)); }
  tlfloat_octuple_ tlfloat_fmao(const tlfloat_octuple_ x, const tlfloat_octuple_ y, const tlfloat_octuple_ z) { return (tlfloat_octuple_)fma(Octuple(x), Octuple(y), Octuple(z)); }

  float tlfloat_sqrtf(const float x) { return (float)sqrt(Float(x)); }
  double tlfloat_sqrt(const double x) { return (double)sqrt(Double(x)); }
  tlfloat_quad_ tlfloat_sqrtq(const tlfloat_quad_ x) { return (tlfloat_quad_)sqrt(Quad(x)); }
  tlfloat_octuple_ tlfloat_sqrto(const tlfloat_octuple_ x) { return (tlfloat_octuple_)sqrt(Octuple(x)); }

  //

  float tlfloat_negf(const float x) { return (float)-Float(x); }
  double tlfloat_neg(const double x) { return (double)-Double(x); }
  tlfloat_quad_ tlfloat_negq(const tlfloat_quad_ x) { return (tlfloat_quad_)-Quad(x); }
  tlfloat_octuple_ tlfloat_nego(const tlfloat_octuple_ x) { return (tlfloat_octuple_)-Octuple(x); }

  float tlfloat_fabsf(const float x) { return (float)fabs(Float(x)); }
  double tlfloat_fabs(const double x) { return (double)fabs(Double(x)); }
  tlfloat_quad_ tlfloat_fabsq(const tlfloat_quad_ x) { return (tlfloat_quad_)fabs(Quad(x)); }
  tlfloat_octuple_ tlfloat_fabso(const tlfloat_octuple_ x) { return (tlfloat_octuple_)fabs(Octuple(x)); }

  float tlfloat_copysignf(const float x, const float y) { return (float)copysign(Float(x), Float(y)); }
  double tlfloat_copysign(const double x, const double y) { return (double)copysign(Double(x), Double(y)); }
  tlfloat_quad_ tlfloat_copysignq(const tlfloat_quad_ x, const tlfloat_quad_ y) { return (tlfloat_quad_)copysign(Quad(x), Quad(y)); }
  tlfloat_octuple_ tlfloat_copysigno(const tlfloat_octuple_ x, const tlfloat_octuple_ y) { return (tlfloat_octuple_)copysign(Octuple(x), Octuple(y)); }

  float tlfloat_fmaxf(const float x, const float y) { return (float)fmax(Float(x), Float(y)); }
  double tlfloat_fmax(const double x, const double y) { return (double)fmax(Double(x), Double(y)); }
  tlfloat_quad_ tlfloat_fmaxq(const tlfloat_quad_ x, const tlfloat_quad_ y) { return (tlfloat_quad_)fmax(Quad(x), Quad(y)); }
  tlfloat_octuple_ tlfloat_fmaxo(const tlfloat_octuple_ x, const tlfloat_octuple_ y) { return (tlfloat_octuple_)fmax(Octuple(x), Octuple(y)); }

  float tlfloat_fminf(const float x, const float y) { return (float)fmin(Float(x), Float(y)); }
  double tlfloat_fmin(const double x, const double y) { return (double)fmin(Double(x), Double(y)); }
  tlfloat_quad_ tlfloat_fminq(const tlfloat_quad_ x, const tlfloat_quad_ y) { return (tlfloat_quad_)fmin(Quad(x), Quad(y)); }
  tlfloat_octuple_ tlfloat_fmino(const tlfloat_octuple_ x, const tlfloat_octuple_ y) { return (tlfloat_octuple_)fmin(Octuple(x), Octuple(y)); }

  float tlfloat_fdimf(const float x, const float y) { return (float)fdim(Float(x), Float(y)); }
  double tlfloat_fdim(const double x, const double y) { return (double)fdim(Double(x), Double(y)); }
  tlfloat_quad_ tlfloat_fdimq(const tlfloat_quad_ x, const tlfloat_quad_ y) { return (tlfloat_quad_)fdim(Quad(x), Quad(y)); }
  tlfloat_octuple_ tlfloat_fdimo(const tlfloat_octuple_ x, const tlfloat_octuple_ y) { return (tlfloat_octuple_)fdim(Octuple(x), Octuple(y)); }

  float tlfloat_ldexpf(const float x, const int y) { return (float)ldexp(Float(x), y); }
  double tlfloat_ldexp(const double x, const int y) { return (double)ldexp(Double(x), y); }
  tlfloat_quad_ tlfloat_ldexpq(const tlfloat_quad_ x, const int y) { return (tlfloat_quad_)ldexp(Quad(x), y); }
  tlfloat_octuple_ tlfloat_ldexpo(const tlfloat_octuple_ x, const int y) { return (tlfloat_octuple_)ldexp(Octuple(x), y); }

  float tlfloat_frexpf(const float x, int *y) { return (float)frexp(Float(x), y); }
  double tlfloat_frexp(const double x, int *y) { return (double)frexp(Double(x), y); }
  tlfloat_quad_ tlfloat_frexpq(const tlfloat_quad_ x, int *y) { return (tlfloat_quad_)frexp(Quad(x), y); }
  tlfloat_octuple_ tlfloat_frexpo(const tlfloat_octuple_ x, int *y) { return (tlfloat_octuple_)frexp(Octuple(x), y); }

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
  tlfloat_quad_ tlfloat_modfq(const tlfloat_quad_ x, tlfloat_quad_ *y) {
    Quad z;
    tlfloat_quad_ r = (tlfloat_quad_)modf(Quad(x), &z);
    *y = (tlfloat_quad_)z;
    return r;
  }
  tlfloat_octuple_ tlfloat_modfo(const tlfloat_octuple_ x, tlfloat_octuple_ *y) {
    Octuple z;
    tlfloat_octuple_ r = (tlfloat_octuple_)modf(Octuple(x), &z);
    *y = (tlfloat_octuple_)z;
    return r;
  }

  float tlfloat_nextafterf(const float x, const float y) { return (float)nextafter(Float(x), Float(y)); }
  double tlfloat_nextafter(const double x, const double y) { return (double)nextafter(Double(x), Double(y)); }
  tlfloat_quad_ tlfloat_nextafterq(const tlfloat_quad_ x, const tlfloat_quad_ y) { return (tlfloat_quad_)nextafter(Quad(x), Quad(y)); }
  tlfloat_octuple_ tlfloat_nextaftero(const tlfloat_octuple_ x, const tlfloat_octuple_ y) { return (tlfloat_octuple_)nextafter(Octuple(x), Octuple(y)); }

  int tlfloat_ilogbf(const float x) { return ilogb(Float(x)); }
  int tlfloat_ilogb(const double x) { return ilogb(Double(x)); }
  int tlfloat_ilogbq(const tlfloat_quad_ x) { return ilogb(Quad(x)); }
  int tlfloat_ilogbo(const tlfloat_octuple_ x) { return ilogb(Octuple(x)); }

  int tlfloat_isnanf(const float x) { return isnan(Float(x)); }
  int tlfloat_isnan(const double x) { return isnan(Double(x)); }
  int tlfloat_isnanq(const tlfloat_quad_ x) { return isnan(Quad(x)); }
  int tlfloat_isnano(const tlfloat_octuple_ x) { return isnan(Octuple(x)); }

  int tlfloat_isinff(const float x) { return isinf(Float(x)); }
  int tlfloat_isinf(const double x) { return isinf(Double(x)); }
  int tlfloat_isinfq(const tlfloat_quad_ x) { return isinf(Quad(x)); }
  int tlfloat_isinfo(const tlfloat_octuple_ x) { return isinf(Octuple(x)); }

  int tlfloat_finitef(const float x) { return finite(Float(x)); }
  int tlfloat_finite(const double x) { return finite(Double(x)); }
  int tlfloat_finiteq(const tlfloat_quad_ x) { return finite(Quad(x)); }
  int tlfloat_finiteo(const tlfloat_octuple_ x) { return finite(Octuple(x)); }

  int tlfloat_fpclassifyf(const float x) { return fpclassify(Float(x)); }
  int tlfloat_fpclassify(const double x) { return fpclassify(Double(x)); }
  int tlfloat_fpclassifyq(const tlfloat_quad_ x) { return fpclassify(Quad(x)); }
  int tlfloat_fpclassifyo(const tlfloat_octuple_ x) { return fpclassify(Octuple(x)); }

  int tlfloat_signbitf(const float x) { return signbit(Float(x)); }
  int tlfloat_signbit(const double x) { return signbit(Double(x)); }
  int tlfloat_signbitq(const tlfloat_quad_ x) { return signbit(Quad(x)); }
  int tlfloat_signbito(const tlfloat_octuple_ x) { return signbit(Octuple(x)); }

  //

  float tlfloat_hypotf(const float x, const float y) { return (float)hypot(Float(x), Float(y)); }
  double tlfloat_hypot(const double x, const double y) { return (double)hypot(Double(x), Double(y)); }
  tlfloat_quad_ tlfloat_hypotq(const tlfloat_quad_ x, const tlfloat_quad_ y) { return (tlfloat_quad_)hypot(Quad(x), Quad(y)); }
  tlfloat_octuple_ tlfloat_hypoto(const tlfloat_octuple_ x, const tlfloat_octuple_ y) { return (tlfloat_octuple_)hypot(Octuple(x), Octuple(y)); }

  float tlfloat_truncf(const float x) { return (float)trunc(Float(x)); }
  double tlfloat_trunc(const double x) { return (double)trunc(Double(x)); }
  tlfloat_quad_ tlfloat_truncq(const tlfloat_quad_ x) { return (tlfloat_quad_)trunc(Quad(x)); }
  tlfloat_octuple_ tlfloat_trunco(const tlfloat_octuple_ x) { return (tlfloat_octuple_)trunc(Octuple(x)); }

  float tlfloat_floorf(const float x) { return (float)floor(Float(x)); }
  double tlfloat_floor(const double x) { return (double)floor(Double(x)); }
  tlfloat_quad_ tlfloat_floorq(const tlfloat_quad_ x) { return (tlfloat_quad_)floor(Quad(x)); }
  tlfloat_octuple_ tlfloat_flooro(const tlfloat_octuple_ x) { return (tlfloat_octuple_)floor(Octuple(x)); }

  float tlfloat_ceilf(const float x) { return (float)ceil(Float(x)); }
  double tlfloat_ceil(const double x) { return (double)ceil(Double(x)); }
  tlfloat_quad_ tlfloat_ceilq(const tlfloat_quad_ x) { return (tlfloat_quad_)ceil(Quad(x)); }
  tlfloat_octuple_ tlfloat_ceilo(const tlfloat_octuple_ x) { return (tlfloat_octuple_)ceil(Octuple(x)); }

  float tlfloat_roundf(const float x) { return (float)round(Float(x)); }
  double tlfloat_round(const double x) { return (double)round(Double(x)); }
  tlfloat_quad_ tlfloat_roundq(const tlfloat_quad_ x) { return (tlfloat_quad_)round(Quad(x)); }
  tlfloat_octuple_ tlfloat_roundo(const tlfloat_octuple_ x) { return (tlfloat_octuple_)round(Octuple(x)); }

  float tlfloat_rintf(const float x) { return (float)rint(Float(x)); }
  double tlfloat_rint(const double x) { return (double)rint(Double(x)); }
  tlfloat_quad_ tlfloat_rintq(const tlfloat_quad_ x) { return (tlfloat_quad_)rint(Quad(x)); }
  tlfloat_octuple_ tlfloat_rinto(const tlfloat_octuple_ x) { return (tlfloat_octuple_)rint(Octuple(x)); }
}
