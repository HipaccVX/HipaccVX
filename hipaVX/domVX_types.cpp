#include "../VX/vx.h"
#include "domVX_types.hpp"

#include <iostream>

namespace HipaVX
{

// Object
int Object::next_id = 0;

Object::Object()
    :my_id(next_id++)
{
}

// Scalar
Scalar::Scalar(vx_type_e t, const void *ptr)
    :data_type(t)
{
    type = VX_TYPE_SCALAR;
    switch(data_type)
    {
    case VX_TYPE_INT32:
        i32 = *((vx_int32*) ptr);
        break;
    case VX_TYPE_FLOAT32:
        f32 = *((vx_float32*) ptr);
        break;
    default:
        throw std::runtime_error("Unsupported vx_type_e in Scalar constructor");
    }
}

// Image
Image::Image(vx_uint32 width, vx_uint32 height, vx_df_image color)
    :w(width), h(height), col(color)
{
    type = VX_TYPE_IMAGE;
}

// Array
Array::Array(vx_enum item_type, vx_size cap, vx_size rows)
    :Image(rows, cap, VX_DF_IMAGE_S32), type(item_type), capacity(cap)
{
    type = VX_TYPE_ARRAY;
}

// Graph
void Graph::build()
{
    for (auto& node: graph)
    {
        node->build();
    }

    built = true;
}

}
