//#include "config_reader.hpp"
#include "../gen_template.hpp"
//#include "../../VX/vx.h"
//#include "../domVX_types.hpp"
//#include "../domVX_kernels.hpp"
#include "hipacc_gen.hpp"

#pragma once

using std::string;

namespace generator
{

Kernelcall_Variable* generate_kernelcall(std::string kernel_name, std::vector<Kernelcall_Variable*> parameters);

std::tuple<std::vector<Kernelcall_Variable*>, std::vector<Kernelcall_Variable*>> generate_iterationspace(HipaVX::Image *image);

string kernelcall_builder(const std::vector<Kernelcall_Variable*>& call_variables);

}
