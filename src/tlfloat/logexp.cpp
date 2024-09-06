#include <cstring>
#include <cctype>
#include <cstdarg>

#include "suppress.hpp"

#define TLFLOAT_NO_LIBSTDCXX
#include "tlfloat/tlmath.hpp"

#include "tlfloat/tlfloat.h"

using namespace tlfloat;

extern "C" {
  float tlfloat_expf(const float a) { return (float)exp(Float(a)); }
  double tlfloat_exp(const double a) { return (double)exp(Double(a)); }
  tlfloat_quad_ tlfloat_expq(const tlfloat_quad_ a) { return (tlfloat_quad_)exp(Quad(a)); }
  tlfloat_octuple_ tlfloat_expo(const tlfloat_octuple_ a) { return (tlfloat_octuple_)exp(Octuple(a)); }

  float tlfloat_expm1f(const float a) { return (float)expm1(Float(a)); }
  double tlfloat_expm1(const double a) { return (double)expm1(Double(a)); }
  tlfloat_quad_ tlfloat_expm1q(const tlfloat_quad_ a) { return (tlfloat_quad_)expm1(Quad(a)); }
  tlfloat_octuple_ tlfloat_expm1o(const tlfloat_octuple_ a) { return (tlfloat_octuple_)expm1(Octuple(a)); }

  float tlfloat_exp2f(const float a) { return (float)exp2(Float(a)); }
  double tlfloat_exp2(const double a) { return (double)exp2(Double(a)); }
  tlfloat_quad_ tlfloat_exp2q(const tlfloat_quad_ a) { return (tlfloat_quad_)exp2(Quad(a)); }
  tlfloat_octuple_ tlfloat_exp2o(const tlfloat_octuple_ a) { return (tlfloat_octuple_)exp2(Octuple(a)); }

  float tlfloat_exp10f(const float a) { return (float)exp10(Float(a)); }
  double tlfloat_exp10(const double a) { return (double)exp10(Double(a)); }
  tlfloat_quad_ tlfloat_exp10q(const tlfloat_quad_ a) { return (tlfloat_quad_)exp10(Quad(a)); }
  tlfloat_octuple_ tlfloat_exp10o(const tlfloat_octuple_ a) { return (tlfloat_octuple_)exp10(Octuple(a)); }

  float tlfloat_logf(const float a) { return (float)log(Float(a)); }
  double tlfloat_log(const double a) { return (double)log(Double(a)); }
  tlfloat_quad_ tlfloat_logq(const tlfloat_quad_ a) { return (tlfloat_quad_)log(Quad(a)); }
  tlfloat_octuple_ tlfloat_logo(const tlfloat_octuple_ a) { return (tlfloat_octuple_)log(Octuple(a)); }

  float tlfloat_log1pf(const float a) { return (float)log1p(Float(a)); }
  double tlfloat_log1p(const double a) { return (double)log1p(Double(a)); }
  tlfloat_quad_ tlfloat_log1pq(const tlfloat_quad_ a) { return (tlfloat_quad_)log1p(Quad(a)); }
  tlfloat_octuple_ tlfloat_log1po(const tlfloat_octuple_ a) { return (tlfloat_octuple_)log1p(Octuple(a)); }

  float tlfloat_log2f(const float a) { return (float)log2(Float(a)); }
  double tlfloat_log2(const double a) { return (double)log2(Double(a)); }
  tlfloat_quad_ tlfloat_log2q(const tlfloat_quad_ a) { return (tlfloat_quad_)log2(Quad(a)); }
  tlfloat_octuple_ tlfloat_log2o(const tlfloat_octuple_ a) { return (tlfloat_octuple_)log2(Octuple(a)); }

  float tlfloat_log10f(const float a) { return (float)log10(Float(a)); }
  double tlfloat_log10(const double a) { return (double)log10(Double(a)); }
  tlfloat_quad_ tlfloat_log10q(const tlfloat_quad_ a) { return (tlfloat_quad_)log10(Quad(a)); }
  tlfloat_octuple_ tlfloat_log10o(const tlfloat_octuple_ a) { return (tlfloat_octuple_)log10(Octuple(a)); }

  float tlfloat_powf(const float x, const float y) { return (float)pow(Float(x), Float(y)); }
  double tlfloat_pow(const double x, const double y) { return (double)pow(Double(x), Double(y)); }
  tlfloat_quad_ tlfloat_powq(const tlfloat_quad_ x, const tlfloat_quad_ y) { return (tlfloat_quad_)pow(Quad(x), Quad(y)); }
  tlfloat_octuple_ tlfloat_powo(const tlfloat_octuple_ x, const tlfloat_octuple_ y) { return (tlfloat_octuple_)pow(Octuple(x), Octuple(y)); }
}
