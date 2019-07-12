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

// TODO: This header should be independent from the vx_types - sure?
//        move Image definition to here, and rename to Acc2D

using AbstractionNode = DomVX::Object;

/**
 * @brief Represents an actual Domain for the virtual ast4vx::WindowDescriptor
 */
class Domain : public AbstractionNode {
 public:
  unsigned int width, height;
  /**
   * @brief domain[height][width] with values 0 and 1
   */
  std::vector<std::vector<unsigned char>> domain;

  /**
   * @brief Constructs a new Domain with initialized values
   * @param x width of the domain
   * @param y height of the domain
   * @param dom row major flat matrix of the domain values. Has to have x*y
   * entries
   */
  // TODO: why is the dom int, just holds 1 and 0
  Domain(unsigned int x, unsigned int y, std::vector<int> dom)
      : width(x), height(y) {
    set_domain(dom);
    set_name("dom");
  }

  /**
   * @brief Updates the domain values
   * @param dom row major flat matrix of the domain values. Has to have x*y
   * entries
   */
  void set_domain(std::vector<int> dom) {
    if (dom.size() != height * width)
      throw std::runtime_error(
          "Domain::set_domain: dom needs to have x * y elements");

    domain.clear();
    auto dom_it = dom.begin();
    for (unsigned int y = 0; y < height; y++) {
      domain.push_back(std::vector<unsigned char>());
      for (unsigned x = 0; x < width; x++) {
        domain[y].push_back((*dom_it != 0) ? 1 : 0);
        dom_it++;
      }
    }
  }
};

/**
 * @brief Represents an actual Mask for the virtual ast4vx::MaskPixelToPixel
 * statements
 */
class Mask : public AbstractionNode {
 public:
  union mask_type {
    int32_t i;
    float f;
    mask_type(int32_t i) : i(i) {}
    mask_type(float f) : f(f) {}
  };

  unsigned int width, height;
  /**
   * @brief mask[height][width] with either integer or float values
   */
  std::vector<std::vector<mask_type>> mask;
  bool mask_is_int;

  /**
   * @brief Constructs a new Mask with initialized values
   * @param x width of the mask
   * @param y height of the mask
   * @param m row major flat matrix of the int32_t mask values. Has to have x*y
   * entries
   */
  Mask(unsigned int x, unsigned int y, std::vector<int32_t> m)
      : width(x), height(y) {
    set_mask(m);
    set_name("mask");
  }
  /**
   * @brief Constructs a new Mask with initialized values
   * @param x width of the mask
   * @param y height of the mask
   * @param m row major flat matrix of the float mask values. Has to have x*y
   * entries
   */
  Mask(unsigned int x, unsigned int y, std::vector<float> m)
      : width(x), height(y) {
    set_mask(m);
    set_name("mask");
  }

  /**
   * @brief Updates the mask values
   * @param m row major flat matrix of the int32_t mask values. Has to have x*y
   * entries
   */
  void set_mask(std::vector<int32_t> m) {
    if (m.size() != height * width)
      throw std::runtime_error(
          "Mask::set_mask: m needs to have x * y elements");

    mask_is_int = true;
    mask.clear();
    auto m_it = m.begin();
    for (unsigned int y = 0; y < height; y++) {
      mask.push_back(std::vector<mask_type>());
      for (unsigned x = 0; x < width; x++) {
        mask[y].push_back(*m_it);
        m_it++;
      }
    }
  }

  /**
   * @brief Updates the mask values
   * @param m row major flat matrix of the float mask values. Has to have x*y
   * entries
   */
  void set_mask(std::vector<float> m) {
    if (m.size() != height * width)
      throw std::runtime_error(
          "Mask::set_mask: m needs to have x * y elements");

    mask_is_int = false;
    mask.clear();
    auto m_it = m.begin();
    for (unsigned int y = 0; y < height; y++) {
      mask.push_back(std::vector<mask_type>());
      for (unsigned x = 0; x < width; x++) {
        mask[y].push_back(*m_it);
        m_it++;
      }
    }
  }
};

class HipaccKernel : public Node {
 public:
  HipaccKernel() = default;
  virtual ~HipaccKernel() override = default;

  std::vector<vx_direction_e> direction;
  std::vector<vx_type_e> type;
  std::string filename;
};

class HipaccNode : public Node {
 public:
  HipaccNode() = default;
  virtual ~HipaccNode() override = default;

  DomVX::Graph *graph;
  HipaccKernel *kernel;
  std::vector<vx_reference> parameters;
};

class MultiNode : public Node {
 public:
  MultiNode() = default;
  virtual ~MultiNode() override = default;

  std::vector<vx_node> nodes;
};

}  // namespace DomVX
