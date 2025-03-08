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

  float tlfloat_sinpif(const float x) { return (float)sinpi(Float(x)); }
  double tlfloat_sinpi(const double x) { return (double)sinpi(Double(x)); }
  tlfloat_quad_ tlfloat_sinpiq(const tlfloat_quad_ x) { return (tlfloat_quad_)sinpi(Quad(x)); }
  tlfloat_octuple_ tlfloat_sinpio(const tlfloat_octuple_ x) { return (tlfloat_octuple_)sinpi(Octuple(x)); }

  float tlfloat_cospif(const float x) { return (float)cospi(Float(x)); }
  double tlfloat_cospi(const double x) { return (double)cospi(Double(x)); }
  tlfloat_quad_ tlfloat_cospiq(const tlfloat_quad_ x) { return (tlfloat_quad_)cospi(Quad(x)); }
  tlfloat_octuple_ tlfloat_cospio(const tlfloat_octuple_ x) { return (tlfloat_octuple_)cospi(Octuple(x)); }

  float tlfloat_tanpif(const float x) { return (float)tanpi(Float(x)); }
  double tlfloat_tanpi(const double x) { return (double)tanpi(Double(x)); }
  tlfloat_quad_ tlfloat_tanpiq(const tlfloat_quad_ x) { return (tlfloat_quad_)tanpi(Quad(x)); }
  tlfloat_octuple_ tlfloat_tanpio(const tlfloat_octuple_ x) { return (tlfloat_octuple_)tanpi(Octuple(x)); }

  void tlfloat_sincosf(const float x, float *s, float *c) {
    auto a = sincos(Float(x)); *s = (float)a.first; *c = (float)a.second;
  }
  void tlfloat_sincos(const double x, double *s, double *c) {
    auto a = sincos(Double(x)); *s = (double)a.first; *c = (double)a.second;
  }
  void tlfloat_sincosq(const tlfloat_quad_ x, tlfloat_quad_ *s, tlfloat_quad_ *c) {
    auto a = sincos(Quad(x)); *s = (tlfloat_quad_)a.first; *c = (tlfloat_quad_)a.second;
  }
  void tlfloat_sincoso(const tlfloat_octuple_ x, tlfloat_octuple_ *s, tlfloat_octuple_ *c) {
    auto a = sincos(Octuple(x)); *s = (tlfloat_octuple_)a.first; *c = (tlfloat_octuple_)a.second;
  }

  void tlfloat_sincospif(const float x, float *s, float *c) {
    auto a = sincospi(Float(x)); *s = (float)a.first; *c = (float)a.second;
  }
  void tlfloat_sincospi(const double x, double *s, double *c) {
    auto a = sincospi(Double(x)); *s = (double)a.first; *c = (double)a.second;
  }
  void tlfloat_sincospiq(const tlfloat_quad_ x, tlfloat_quad_ *s, tlfloat_quad_ *c) {
    auto a = sincospi(Quad(x)); *s = (tlfloat_quad_)a.first; *c = (tlfloat_quad_)a.second;
  }
  void tlfloat_sincospio(const tlfloat_octuple_ x, tlfloat_octuple_ *s, tlfloat_octuple_ *c) {
    auto a = sincospi(Octuple(x)); *s = (tlfloat_octuple_)a.first; *c = (tlfloat_octuple_)a.second;
  }
}
