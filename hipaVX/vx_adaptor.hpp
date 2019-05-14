#pragma once

#include "../VX/vx.h"
#include "../VX/vx_types.h"
#include "dsl/types.hpp"

struct _vx_reference {
  DomVX::Object *o;
};

struct _vx_node {
  DomVX::Object *o;
};

struct _vx_convolution {
  DomVX::Object *o;
};

struct _vx_kernel {
  DomVX::Object *o;
};

struct _vx_scalar {
  DomVX::Object *o;
};

struct _vx_threshold {
  DomVX::Object *o;
};

struct _vx_image {
  DomVX::Object *o;
};

struct _vx_array {
  DomVX::Object *o;
};

struct _vx_matrix {
  DomVX::Object *o;
};

struct _vx_graph {
  DomVX::Object *o;
};

struct _vx_context {
  DomVX::Object *o;
};
