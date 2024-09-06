#include <cstring>
#include <cctype>
#include <cstdarg>

#include "suppress.hpp"

#define TLFLOAT_NO_LIBSTDCXX
#include "tlfloat/tlmath.hpp"

#include "tlfloat/tlfloat.h"

using namespace tlfloat;

extern "C" {
  float tlfloat_sinf(const float x) { return (float)sin(Float(x)); }
  double tlfloat_sin(const double x) { return (double)sin(Double(x)); }
  tlfloat_quad_ tlfloat_sinq(const tlfloat_quad_ x) { return (tlfloat_quad_)sin(Quad(x)); }
  tlfloat_octuple_ tlfloat_sino(const tlfloat_octuple_ x) { return (tlfloat_octuple_)sin(Octuple(x)); }

  float tlfloat_cosf(const float x) { return (float)cos(Float(x)); }
  double tlfloat_cos(const double x) { return (double)cos(Double(x)); }
  tlfloat_quad_ tlfloat_cosq(const tlfloat_quad_ x) { return (tlfloat_quad_)cos(Quad(x)); }
  tlfloat_octuple_ tlfloat_coso(const tlfloat_octuple_ x) { return (tlfloat_octuple_)cos(Octuple(x)); }

  float tlfloat_tanf(const float x) { return (float)tan(Float(x)); }
  double tlfloat_tan(const double x) { return (double)tan(Double(x)); }
  tlfloat_quad_ tlfloat_tanq(const tlfloat_quad_ x) { return (tlfloat_quad_)tan(Quad(x)); }
  tlfloat_octuple_ tlfloat_tano(const tlfloat_octuple_ x) { return (tlfloat_octuple_)tan(Octuple(x)); }
}
