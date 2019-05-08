#pragma once
#include "abstractions.hpp"

//typedef domVX_image

typedef std::shared_ptr<DomVX::Map> domVX_point;
typedef std::shared_ptr<DomVX::LocalOperation> domVX_local;
typedef std::shared_ptr<DomVX::GlobalOperation> domVX_global;

typedef std::shared_ptr<DomVX::Domain> domVX_domain;
typedef std::shared_ptr<DomVX::Mask> domVX_mask;

typedef HipaVX::Image* domVX_image;

//******************************************************************************
// OPERATORS
//******************************************************************************

/**
 * @brief Returns a new Map and initializes it
 */
domVX_point create_point_op();

/**
 * @brief Returns a new LocalOperation and initializes it
 */
domVX_local create_local_op();

/**
 * @brief Returns a new GlobalOperation and initializes it
 */
domVX_global create_global_op();


//******************************************************************************
// Image
//******************************************************************************
domVX_image create_image(unsigned int width, unsigned int height, vx_df_image type);


//******************************************************************************
// DOMAIN  (TODO: Consider renaming as Window)
//******************************************************************************

/**
 * @brief Returns a new Domain and initializes it
 * @param x width of the domain
 * @param y height of the domain
 * @param dom row major flat matrix of the domain values. Has to have x*y entries
 */
domVX_domain create_dom(unsigned int x, unsigned int y, std::vector<int> dom);

/**
 * @brief Returns a new Domain and initializes its values from the mask
 * @param mask The mask where the values should get read from
 */
domVX_domain create_dom(std::shared_ptr<DomVX::Mask> mask);


//******************************************************************************
// MASK
//******************************************************************************

/**
 * @brief Returns a new Mask and initializes it
 * @param x width of the mask
 * @param y height of the mask
 * @param mask row major flat matrix of the mask values. Has to have x*y entries
 */
domVX_mask create_mask(unsigned int x, unsigned int y, std::initializer_list<int32_t> mask);

/**
 * @brief Returns a new Mask and initializes it
 * @param x width of the mask
 * @param y height of the mask
 * @param mask row major flat matrix of the mask values. Has to have x*y entries
 */
domVX_mask create_mask(unsigned int x, unsigned int y, std::initializer_list<float> mask);
