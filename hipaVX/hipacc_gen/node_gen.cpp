#include "node_gen.hpp"

#include <string>
#include <map>
#include <type_traits>

using std::string;


// TODO: Why do we need an explicit node generator for all the functions??

namespace generator
{

string kernel_builder(const std::vector<Kernel_Variable*>& variables, const std::vector<string>& kernel_code, const string& kernel_name)
{
    string member_variables;
    string constructor_parameters;
    string constructor_init_list;
    string add_accessor;

    string tabs = "\t";

    for(const auto& kv: variables)
    {
        string s = kv->generate_kerneldefinition();

        member_variables += tabs + s + ";\n";
        constructor_parameters += ", " + s;
        constructor_init_list += ", " + kv->name + "(" + kv->name + ")";
        if (kv->is_accessor())
        {
            add_accessor += tabs + '\t' + "add_accessor(&" + kv->name + ");\n";
        }
    }

    string kernel;
    for(const auto& line: kernel_code)
    {
        kernel += tabs + '\t' + line + '\n';
    }

    string s = read_file(hipaVX_folder + "/kernels/general_kernel_definition");
    s = use_template(s, "KERNEL_NAME", kernel_name);
    s = use_template(s, "VARIABLES", member_variables);
    s = use_template(s, "VARIABLES_CONSTRUCTOR_PARAMS", constructor_parameters);
    s = use_template(s, "VARIABLES_CONSTRUCTOR_INIT_LIST", constructor_init_list);
    s = use_template(s, "ADD_ACCESSOR", add_accessor);
    s = use_template(s, "MISC_CONSTRUCTOR", "");
    s = use_template(s, "KERNEL", kernel);

    return s;
}

string kernelcall_builder(const std::vector<Kernelcall_Variable*>& call_variables)
{
    string to_return;
    for(const auto& kv: call_variables)
    {
        auto strings = kv->generate_kernelcall();
        for (const auto& s: strings)
        {
            to_return += '\t' + s + '\n';
        }
        to_return += '\n';
    }

    to_return += '\n';

    return to_return;
}

std::tuple<std::vector<Kernelcall_Variable*>, std::vector<Kernelcall_Variable*>> generate_iterationspace(HipaVX::Image *image)
{
    std::vector<Kernelcall_Variable*> to_return_call_parameters;
    std::vector<Kernelcall_Variable*> to_return;

    Kernelcall_IterationSpace *is = new Kernelcall_IterationSpace();
    string iter_name = "iterspace_" + generate_image_name(image);
    is->set_real_name(iter_name);
    is->datatype = VX_DF_IMAGE_to_hipacc[image->col];
    is->image = generate_image_name(image);

    to_return_call_parameters.push_back(is);
    to_return.push_back(is);

    return {to_return_call_parameters,to_return};
}

std::tuple<std::vector<Kernelcall_Variable*>, std::vector<Kernelcall_Variable*>> generate_accessor(HipaVX::Image *image, HipaVX::VX_Matrix *mat)
{
    std::vector<Kernelcall_Variable*> to_return_call_parameters;
    std::vector<Kernelcall_Variable*> to_return;

    Kernelcall_Mask *mask = new Kernelcall_Mask;
    string mask_name = "mask_" + generate_image_name(image);
    mask->set_real_name(mask_name);
    mask->datatype = VX_DF_IMAGE_to_hipacc[mat->data_type];
    mask->len_dims[0] = mat->rows;
    mask->len_dims[1] = mat->columns;
    switch(mat->data_type)
    {
    case VX_TYPE_UINT8:
        for(size_t i = 0; i < mat->rows * mat->columns; i++)
            mask->flat_mask.emplace_back(std::to_string(((uint8_t*) mat->mat.data())[i]));
        break;
    case VX_TYPE_INT32:
        for(size_t i = 0; i < mat->rows * mat->columns; i++)
            mask->flat_mask.emplace_back(std::to_string(((int32_t*) mat->mat.data())[i]));
        break;
    case VX_TYPE_FLOAT32:
        for(size_t i = 0; i < mat->rows * mat->columns; i++)
            mask->flat_mask.emplace_back(std::to_string(((float_t*) mat->mat.data())[i]));
        break;
    }

    Kernelcall_Domain *domain = new Kernelcall_Domain();
    string domain_name = "domain_" + generate_image_name(image);
    domain->set_real_name(domain_name);
    domain->argument = mask;

    Kernelcall_BoundaryCondition_from_Dom *boundary = new Kernelcall_BoundaryCondition_from_Dom();
    string boundary_name = "boundary_" + generate_image_name(image);
    boundary->set_real_name(boundary_name);
    boundary->datatype = VX_DF_IMAGE_to_hipacc[image->col];
    boundary->image = generate_image_name(image);
    boundary->bc = "@@@BOUNDARY_CONDITION@@@";
    boundary->argument = domain;

    Kernelcall_Accessor *accessor = new Kernelcall_Accessor();
    string accessor_name = "accessor_" + generate_image_name(image);
    accessor->set_real_name(accessor_name);
    accessor->datatype = VX_DF_IMAGE_to_hipacc[image->col];
    accessor->argument = boundary;


    to_return_call_parameters.push_back(accessor);
    to_return_call_parameters.push_back(domain);
    to_return_call_parameters.push_back(mask);

    to_return.push_back(mask);
    to_return.push_back(domain);
    to_return.push_back(boundary);
    to_return.push_back(accessor);

    return {to_return_call_parameters,to_return};
}
std::tuple<std::vector<Kernelcall_Variable*>, std::vector<Kernelcall_Variable*>> generate_scalar(HipaVX::Scalar *scalar)
{
    static int id = 0;

    std::vector<Kernelcall_Variable*> to_return_call_parameters;
    std::vector<Kernelcall_Variable*> to_return;

    Kernelcall_General_Variable *gv = new Kernelcall_General_Variable();
    gv->set_real_name("general_variable_" + std::to_string(id++));
    gv->datatype = VX_DF_IMAGE_to_hipacc[scalar->data_type];
    switch(scalar->data_type)
    {
    case VX_TYPE_FLOAT32:
        gv->value = std::to_string(scalar->f32);
        break;
    case VX_TYPE_INT32:
        gv->value = std::to_string(scalar->i32);
        break;
    case VX_TYPE_INT8:
        gv->value = std::to_string(scalar->i8);
        break;
    default:
        throw std::runtime_error("std::tuple<std::vector<Kernelcall_Variable*>, std::vector<Kernelcall_Variable*>> generate_scalar(HipaVX::Scalar *scalar):\n\tNot supported scalar type");
    }

    to_return_call_parameters.push_back(gv);
    to_return.push_back(gv);

    return {to_return_call_parameters,to_return};
}
Kernelcall_Variable* generate_kernelcall(std::string kernel_name, std::vector<Kernelcall_Variable*> parameters)
{
    Kernelcall *call = new Kernelcall();
    call->set_real_name("hipacc_kernel_call");
    call->kernel_name = kernel_name;
    call->arguments = parameters;

    return call;
}

string node_generator(HipaVX::WriteImageNode* n, Type t)
{
    if (t == Type::Definition)
    {
        return "";
    }
    else if (t == Type::Call)
    {
        string s = "\t@@@IMAGE_DATATYPE@@@ *data_@@@IMAGE@@@_@@@ID@@@ = @@@IMAGE@@@.data();\n";
        s       += "\tsave_data(@@@IMAGE_WIDTH@@@, @@@IMAGE_HEIGHT@@@, @@@IMAGE_CHANNELS@@@, @@@CONVERSION@@@ data_@@@IMAGE@@@_@@@ID@@@, \"@@@FILENAME@@@\");\n";
        s       += "\n";


        s = use_template(s, "ID", n->my_id);
        s = use_template(s, "IMAGE", generate_image_name(n->in));
        s = use_template(s, "IMAGE_DATATYPE", VX_DF_IMAGE_to_hipacc[n->in->col]);

        s = use_template(s, "IMAGE_WIDTH", n->in->w);
        s = use_template(s, "IMAGE_HEIGHT", n->in->h);
        if (n->in->col == VX_DF_IMAGE_RGBX)
        {
            s = use_template(s, "IMAGE_CHANNELS", 4);
            s = use_template(s, "CONVERSION", "(uchar*)");
        }
        else
        {
            s = use_template(s, "IMAGE_CHANNELS", 1); //TODO
            s = use_template(s, "CONVERSION", "");
        }
        s = use_template(s, "FILENAME", n->out_file);

        return s;
    }
    return "SOMETHING IS WRONG";
}

//string node_generator(HipaVX::HarrisCorners* n, Type t)
//{
//    //return "";
//
//    if (t == Type::Definition)
//    {
//        auto abstract_1_acc_cs = read_config_def(hipaVX_folder + "/kernels/point/abstract_1_acc.def");
//
//        // V_c Kernel
//        auto v_c = kernel_builder(abstract_1_acc_cs.kv, abstract_1_acc_cs.k, abstract_1_acc_cs.name + "_V_c");
//        v_c = use_template(v_c, "KERNEL", "float out = input();\n"
//                                          "\t\tif (out <= @@@THRESHOLD@@@)\n"
//                                          "\t\t\tout = 0;\n"
//                                          "\t\toutput() = out;");
//        v_c = use_template(v_c, "THRESHOLD", std::to_string(n->strength_thresh->f32));
//        v_c = use_template(v_c, "INPUT_DATATYPE", VX_DF_IMAGE_to_hipacc[VX_TYPE_FLOAT32]);
//        v_c = use_template(v_c, "OUTPUT_DATATYPE", VX_DF_IMAGE_to_hipacc[VX_TYPE_FLOAT32]);
//
//        string s;
//        s += v_c;
//
//        s = use_template(s, "ID", n->my_id);
//
//        return s;
//    }
//    else if (t == Type::Call)
//    {
//        auto cs = read_config_call(hipaVX_folder + "/kernels/harris_corners.call");
//        string s = kernelcall_builder(cs.kcv);
//
//
//        s += "\t@@@V_C_TYPE_OUT@@@ *v_c_@@@ID@@@_data = @@@IMAGE_V_C@@@.data();\n";
//        s += "\tstd::vector<@@@V_C_TYPE_OUT@@@> v_c_suppressed_@@@ID@@@(@@@IMAGE_IN_WIDTH@@@ * @@@IMAGE_IN_HEIGHT@@@);\n";
//        s += "\tnon_max_supression(@@@IMAGE_IN_WIDTH@@@, @@@IMAGE_IN_HEIGHT@@@, v_c_@@@ID@@@_data, v_c_suppressed_@@@ID@@@.data());\n";
//        s += "\tauto features_@@@ID@@@ = euclidian_single_feature(@@@IMAGE_IN_WIDTH@@@, @@@IMAGE_IN_HEIGHT@@@, v_c_suppressed_@@@ID@@@.data(), @@@EUCLIDIAN_DISTANCE@@@);\n";
//        s += "\t@@@NUM_KEYPOINTS@@@ = features_@@@ID@@@.size();\n";
//        s += "\tfeatures_@@@ID@@@.resize(@@@MAX_KEYPOINTS@@@);\n";
//        s += "\tstd::vector<int> cont_features_@@@ID@@@ = create_contiguous_array_from_keypoints(features_@@@ID@@@);\n";
//        //s += "\t@@@CORNERS_ARRAY@@@ = cont_features_@@@ID@@@.data();\n"; //Not working due to a current bug
//
//        s += "\tImage<int> @@@CORNERS_ARRAY@@@(7, @@@MAX_KEYPOINTS@@@, cont_features_@@@ID@@@.data());\n";
//
//        s += "\n";
//
//        s += "\t@@@SOBEL_TYPE_IN@@@ *input_image_@@@ID@@@_data = @@@INPUT_IMAGE@@@.data();\n";
//        s += "\tstd::vector<uchar> out_data_@@@ID@@@(@@@IMAGE_IN_WIDTH@@@ * @@@IMAGE_IN_HEIGHT@@@);\n";
//        s += "\tdraw_cross(@@@IMAGE_IN_WIDTH@@@, @@@IMAGE_IN_HEIGHT@@@, input_image_@@@ID@@@_data, features_@@@ID@@@, out_data_@@@ID@@@.data(), (uchar) 255);\n";
//        s += "\tsave_data(@@@IMAGE_IN_WIDTH@@@, @@@IMAGE_IN_HEIGHT@@@, 1, out_data_@@@ID@@@.data(), \"akif-200x300_bw_harris.png\");\n";
//
//        s = use_template(s, "ID", n->my_id);
//        s = use_template(s, "BOUNDARY_CONDITION", "Boundary::UNDEFINED"); // TODO
//        s = use_template(s, "INPUT_IMAGE", generate_image_name(n->in));
//        s = use_template(s, "EUCLIDIAN_DISTANCE", n->min_distance->f32);
//
//        s = use_template(s, "IMAGE_IN_WIDTH", n->in->w);
//        s = use_template(s, "IMAGE_IN_HEIGHT", n->in->h);
//
//
//        s = use_template(s, "V_C_TYPE_IN", VX_DF_IMAGE_to_hipacc[VX_TYPE_FLOAT32]);
//        s = use_template(s, "V_C_TYPE_OUT", VX_DF_IMAGE_to_hipacc[VX_TYPE_FLOAT32]);
//        s = use_template(s, "SOBEL_TYPE_IN", VX_DF_IMAGE_to_hipacc[VX_DF_IMAGE_U8]);
//
//
//        s = use_template(s, "IMAGE_M_C", generate_image_name(&n->Mc));
//        s = use_template(s, "IMAGE_V_C", generate_image_name(&n->Vc));
//
//
//        //s = use_template(s, "NUM_KEYPOINTS", n->num_corners); // TODO
//        s = use_template(s, "NUM_KEYPOINTS", "int a");
//        s = use_template(s, "MAX_KEYPOINTS", n->corners->h);
//        s = use_template(s, "CORNERS_ARRAY", generate_image_name(n->corners));
//
//
//        return s;
//    }
//    return "SOMETHING IS WRONG";
//}

string node_generator(HipaVX::HipaccNode* n, Type t)
{
    if (t == Type::Definition)
    {
        string kernel = read_file(n->filename);

        size_t class_index = kernel.find("class");
        size_t kernelname_index = kernel.find(" ", class_index) + 1;
        size_t kernelname_end_index = kernel.find(" ", kernelname_index);

        n->kernel_name = kernel.substr(kernelname_index, kernelname_end_index - kernelname_index);
        const size_t kernel_name_length = n->kernel_name.length();
        while(true)
        {
            kernelname_index = kernel.find(n->kernel_name, kernelname_index);
            if (kernelname_index == string::npos)
                break;
            kernel.replace(kernelname_index, kernel_name_length, n->kernel_name + "_" + std::to_string(n->my_id));
            kernelname_index += kernel_name_length;
        }
        return kernel;
    }
    else if (t == Type::Call)
    {
        config_struct_call___ hipacc_call;
        std::vector<Kernelcall_Variable*> kernel_parameters;

        std::tuple<std::vector<Kernelcall_Variable*>, std::vector<Kernelcall_Variable*>> tuple;

        tuple = generate_iterationspace(n->out);
        kernel_parameters.insert(kernel_parameters.end(), std::get<0>(tuple).begin(), std::get<0>(tuple).end());
        hipacc_call.kcv.insert(hipacc_call.kcv.end(), std::get<1>(tuple).begin(), std::get<1>(tuple).end());

        for(unsigned int i = 0; i < n->parameters.size(); i++)
        {
            switch(n->parameters[i]->type)
            {
            case VX_TYPE_SCALAR:
                tuple = generate_scalar((HipaVX::Scalar*) n->parameters[i]);
                break;
            case VX_TYPE_IMAGE:
                if (i+1 < n->parameters.size() && n->parameters[i+1]->type == VX_TYPE_MATRIX)
                {
                    tuple = generate_accessor((HipaVX::Image*) n->parameters[i], (HipaVX::VX_Matrix*) n->parameters[i+1]);
                    i++;
                }
                break;
            default:
                throw std::runtime_error("string node_generator(HipaVX::HipaccNode* n, Type t):\n\tUnsupported OpenVX Parameter");
            }

            kernel_parameters.insert(kernel_parameters.end(), std::get<0>(tuple).begin(), std::get<0>(tuple).end());
            hipacc_call.kcv.insert(hipacc_call.kcv.end(), std::get<1>(tuple).begin(), std::get<1>(tuple).end());
        }

        hipacc_call.kcv.push_back(generate_kernelcall(n->kernel_name, kernel_parameters));
        string s = kernelcall_builder(hipacc_call.kcv);

        s = use_template(s, "ID", n->my_id);
        s = use_template(s, "BOUNDARY_CONDITION", "Boundary::UNDEFINED"); // TODO

        return s;
    }
    return "SOMETHING IS WRONG";
}

}
