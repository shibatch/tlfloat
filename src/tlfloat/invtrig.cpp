#include <cstring>
#include <cctype>
#include <cstdarg>

#include "suppress.hpp"

#define TLFLOAT_NO_LIBSTDCXX
#include "tlfloat/tlmath.hpp"

#include "tlfloat/tlfloat.h"

using namespace tlfloat;

extern "C" {
  float tlfloat_asinf(const float x) { return (float)asin(Float(x)); }
  double tlfloat_asin(const double x) { return (double)asin(Double(x)); }
  tlfloat_quad_ tlfloat_asinq(const tlfloat_quad_ x) { return (tlfloat_quad_)asin(Quad(x)); }
  tlfloat_octuple_ tlfloat_asino(const tlfloat_octuple_ x) { return (tlfloat_octuple_)asin(Octuple(x)); }

  float tlfloat_acosf(const float x) { return (float)acos(Float(x)); }
  double tlfloat_acos(const double x) { return (double)acos(Double(x)); }
  tlfloat_quad_ tlfloat_acosq(const tlfloat_quad_ x) { return (tlfloat_quad_)acos(Quad(x)); }
  tlfloat_octuple_ tlfloat_acoso(const tlfloat_octuple_ x) { return (tlfloat_octuple_)acos(Octuple(x)); }

  float tlfloat_atanf(const float x) { return (float)atan(Float(x)); }
  double tlfloat_atan(const double x) { return (double)atan(Double(x)); }
  tlfloat_quad_ tlfloat_atanq(const tlfloat_quad_ x) { return (tlfloat_quad_)atan(Quad(x)); }
  tlfloat_octuple_ tlfloat_atano(const tlfloat_octuple_ x) { return (tlfloat_octuple_)atan(Octuple(x)); }

  float tlfloat_atan2f(const float y, const float x) { return (float)atan2(Float(y), Float(x)); }
  double tlfloat_atan2(const double y, const double x) { return (double)atan2(Double(y), Double(x)); }
  tlfloat_quad_ tlfloat_atan2q(const tlfloat_quad_ y, const tlfloat_quad_ x) { return (tlfloat_quad_)atan2(Quad(y), Quad(x)); }
  tlfloat_octuple_ tlfloat_atan2o(const tlfloat_octuple_ y, const tlfloat_octuple_ x) { return (tlfloat_octuple_)atan2(Octuple(y), Octuple(x)); }
}
