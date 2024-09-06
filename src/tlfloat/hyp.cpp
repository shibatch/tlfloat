#include <cstring>
#include <cctype>
#include <cstdarg>

#include "suppress.hpp"

#define TLFLOAT_NO_LIBSTDCXX
#include "tlfloat/tlmath.hpp"

#include "tlfloat/tlfloat.h"

using namespace tlfloat;

extern "C" {
  float tlfloat_sinhf(const float x) { return (float)sinh(Float(x)); }
  double tlfloat_sinh(const double x) { return (double)sinh(Double(x)); }
  tlfloat_quad_ tlfloat_sinhq(const tlfloat_quad_ x) { return (tlfloat_quad_)sinh(Quad(x)); }
  tlfloat_octuple_ tlfloat_sinho(const tlfloat_octuple_ x) { return (tlfloat_octuple_)sinh(Octuple(x)); }

  float tlfloat_coshf(const float x) { return (float)cosh(Float(x)); }
  double tlfloat_cosh(const double x) { return (double)cosh(Double(x)); }
  tlfloat_quad_ tlfloat_coshq(const tlfloat_quad_ x) { return (tlfloat_quad_)cosh(Quad(x)); }
  tlfloat_octuple_ tlfloat_cosho(const tlfloat_octuple_ x) { return (tlfloat_octuple_)cosh(Octuple(x)); }

  float tlfloat_tanhf(const float x) { return (float)tanh(Float(x)); }
  double tlfloat_tanh(const double x) { return (double)tanh(Double(x)); }
  tlfloat_quad_ tlfloat_tanhq(const tlfloat_quad_ x) { return (tlfloat_quad_)tanh(Quad(x)); }
  tlfloat_octuple_ tlfloat_tanho(const tlfloat_octuple_ x) { return (tlfloat_octuple_)tanh(Octuple(x)); }

  float tlfloat_asinhf(const float x) { return (float)asinh(Float(x)); }
  double tlfloat_asinh(const double x) { return (double)asinh(Double(x)); }
  tlfloat_quad_ tlfloat_asinhq(const tlfloat_quad_ x) { return (tlfloat_quad_)asinh(Quad(x)); }
  tlfloat_octuple_ tlfloat_asinho(const tlfloat_octuple_ x) { return (tlfloat_octuple_)asinh(Octuple(x)); }

  float tlfloat_acoshf(const float x) { return (float)acosh(Float(x)); }
  double tlfloat_acosh(const double x) { return (double)acosh(Double(x)); }
  tlfloat_quad_ tlfloat_acoshq(const tlfloat_quad_ x) { return (tlfloat_quad_)acosh(Quad(x)); }
  tlfloat_octuple_ tlfloat_acosho(const tlfloat_octuple_ x) { return (tlfloat_octuple_)acosh(Octuple(x)); }

  float tlfloat_atanhf(const float x) { return (float)atanh(Float(x)); }
  double tlfloat_atanh(const double x) { return (double)atanh(Double(x)); }
  tlfloat_quad_ tlfloat_atanhq(const tlfloat_quad_ x) { return (tlfloat_quad_)atanh(Quad(x)); }
  tlfloat_octuple_ tlfloat_atanho(const tlfloat_octuple_ x) { return (tlfloat_octuple_)atanh(Octuple(x)); }
}
