#pragma once

#include <algorithm>
#include <chrono>
#include <cmath>
#include <cstdlib>
#include <cstring>
#include <ctime>
#include <iostream>
#include <map>
#include <memory>
#include <string>
#include <vector>

#include "types.hpp"

namespace DomVX {

class CustomKernel : public Node{
// Can't we put the type and direction to Node definition?
public:
	virtual ~CustomKernel() override = default;

	std::vector<vx_direction_e> direction;
	std::vector<vx_type_e> type;
};

class HipaccKernel : public CustomKernel {
 public:
  virtual ~HipaccKernel() override = default;

  std::string filename;
  std::vector<HipaccParameterType> hipacc_type;
};

class CppKernel : public CustomKernel {
 public:
  virtual ~CppKernel() override = default;

  std::string filename;
};

class CustomNode : public Node {
 public:
  virtual ~CustomNode() override = default;

  DomVX::Graph *graph;
  CustomKernel *kernel;
  std::vector<vx_reference> parameters;
};

class MultiNode : public Node {
 public:
  MultiNode() = default;
  virtual ~MultiNode() override = default;

  std::vector<vx_node> nodes;
};

class FileinputImage : public Image {
 public:
  FileinputImage(vx_uint32 width, vx_uint32 height, vx_df_image color,
                 std::string filename)
      : Image(width, height, color), file(filename) {}
  virtual ~FileinputImage() = default;

  std::string file;
};

}  // namespace DomVX
