#pragma once
#include "abstractions.hpp"

//******************************************************************************
// OPERATORS
//******************************************************************************

/**
 * @brief Returns a new Map and initializes it
 */
std::shared_ptr<DomVX::Map> create_point_op();

/**
 * @brief Returns a new LocalOperation and initializes it
 */
std::shared_ptr<DomVX::LocalOperation> create_local_op();

/**
 * @brief Returns a new GlobalOperation and initializes it
 */
std::shared_ptr<DomVX::GlobalOperation> create_global_op();

//******************************************************************************
// DOMAIN  (TODO: Consider renaming as Window)
//******************************************************************************

/**
 * @brief Returns a new Domain and initializes it
 * @param x width of the domain
 * @param y height of the domain
 * @param dom row major flat matrix of the domain values. Has to have x*y entries
 */
std::shared_ptr<DomVX::Domain> create_dom(unsigned int x, unsigned int y, std::vector<int> dom);

/**
 * @brief Returns a new Domain and initializes its values from the mask
 * @param mask The mask where the values should get read from
 */
std::shared_ptr<DomVX::Domain> create_dom(std::shared_ptr<DomVX::Mask> mask);

//******************************************************************************
// MASK
//******************************************************************************

/**
 * @brief Returns a new Mask and initializes it
 * @param x width of the mask
 * @param y height of the mask
 * @param mask row major flat matrix of the mask values. Has to have x*y entries
 */
std::shared_ptr<DomVX::Mask> create_mask(unsigned int x, unsigned int y, std::initializer_list<int32_t> mask);

/**
 * @brief Returns a new Mask and initializes it
 * @param x width of the mask
 * @param y height of the mask
 * @param mask row major flat matrix of the mask values. Has to have x*y entries
 */
std::shared_ptr<DomVX::Mask> create_mask(unsigned int x, unsigned int y, std::initializer_list<float> mask);
