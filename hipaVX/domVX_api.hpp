#pragma once
#include "abstractions.hpp"

typedef std::shared_ptr<DomVX::Map> domVX_point;
typedef std::shared_ptr<DomVX::LocalOperator> domVX_local;
typedef std::shared_ptr<DomVX::GlobalOperator> domVX_global;

typedef std::shared_ptr<DomVX::Domain> domVX_domain;
typedef std::shared_ptr<DomVX::Mask> domVX_mask;

typedef DomVX::Image* domVX_image;

//*********************************************************************
// OPERATORS
//*********************************************************************
/// @brief creates a point operator.
domVX_point create_point_op();

/// @brief creates a point operator.
domVX_local create_local_op();

/// @brief creates a point operator.
domVX_global create_global_op();

//*********************************************************************
// Image
//*********************************************************************
domVX_image create_image(unsigned int width, unsigned int height,
                         vx_df_image type);

//*********************************************************************
// DOMAIN  (TODO: Consider renaming as Window)
//*********************************************************************
/**
 * @brief Returns a new Domain and initializes it
 * @param x width of the domain
 * @param y height of the domain
 * @param dom row major flat matrix of the domain values. Has to have x*y
 * entries
 */
domVX_domain create_domain(unsigned int w, unsigned int h, std::vector<int> dom);

/**
 * @brief Returns a new Domain and initializes its values from the mask
 * @param mask The mask where the values should get read from
 */
domVX_domain create_domain(domVX_mask mask);

//*********************************************************************
// MASK
//*********************************************************************
/**
 * @brief Returns a new Mask and initializes it
 * @param x width of the mask
 * @param y height of the mask
 * @param mask row major flat matrix of the mask values. Has to have x*y entries
 */
domVX_mask create_mask(unsigned int w, unsigned int h,
                       std::initializer_list<int32_t> mask);

/**
 * @brief Returns a new Mask and initializes it
 * @param x width of the mask
 * @param y height of the mask
 * @param mask row major flat matrix of the mask values. Has to have x*y entries
 */
domVX_mask create_mask(unsigned int w, unsigned int h,
                       std::initializer_list<float> mask);
