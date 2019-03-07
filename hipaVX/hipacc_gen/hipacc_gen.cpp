#include "hipacc_gen.hpp"
#include <string>

// TODO: use tuples
string generate_image_name(HipaVX::Image *image)
{
    return string("Image_") + std::to_string(image->my_id);
}

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

    string s = read_file(hipaVX_folder + "/templates/hipacc_kernel.temp");
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

    Kernelcall_Mask *mask = new Kernelcall_Mask();
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

namespace function_ast
{

// !!! get rid of this use tuples
std::string generate_image_name(Image *i)
{
    //std::string hipavx_part = string("Image_") + std::to_string(i->my_id); // same with hipacc_gen.hpp
    std::string hipavx_part = ::generate_image_name(i->image);
    return hipavx_part;
}

std::string to_string(Datatype d)
{
    std::string datatype = "Unsupported Datatype";
    switch(d)
    {
    case Datatype::FLOAT:
        datatype = "float";
        break;
    case Datatype::UINT8:
        datatype = "unsigned char";
        break;
    case Datatype::INT16:
        datatype = "short";
        break;
    case Datatype::INT32:
        datatype = "int";
        break;
    }
    return datatype;
}

std::tuple<std::vector<Kernelcall_Variable*>, std::vector<Kernelcall_Variable*>> generate_accessor(HipaVX::Image *image, function_ast::Stencil *stencil)
{
    std::vector<Kernelcall_Variable*> to_return_call_parameters;
    std::vector<Kernelcall_Variable*> to_return;

    Kernelcall_Mask *mask = new Kernelcall_Mask;
	string mask_name = "mask_" + ::generate_image_name(image);
    mask->set_real_name(mask_name);
    mask->datatype = to_string(stencil->datatype);
    mask->len_dims[0] = stencil->dim[0];
    mask->len_dims[1] = stencil->dim[1];
    mask->flat_mask = stencil->mask;

    Kernelcall_Domain *domain = new Kernelcall_Domain();
	string domain_name = "domain_" + ::generate_image_name(image);
    domain->set_real_name(domain_name);
    domain->argument = mask;

    Kernelcall_BoundaryCondition_from_Dom *boundary = new Kernelcall_BoundaryCondition_from_Dom();
	string boundary_name = "boundary_" + ::generate_image_name(image);
    boundary->set_real_name(boundary_name);
    boundary->datatype = VX_DF_IMAGE_to_hipacc[image->col];
	boundary->image = ::generate_image_name(image);
    boundary->bc = "@@@BOUNDARY_CONDITION@@@";
    boundary->argument = domain;

    Kernelcall_Accessor *accessor = new Kernelcall_Accessor();
	string accessor_name = "accessor_" + ::generate_image_name(image);
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

std::tuple<std::vector<Kernelcall_Variable*>, std::vector<Kernelcall_Variable*>> generate_accessor(Image *image)
{
	std::vector<Kernelcall_Variable*> to_return_call_parameters;
	std::vector<Kernelcall_Variable*> to_return;

	Kernelcall_BoundaryCondition_from_WH *boundary = new Kernelcall_BoundaryCondition_from_WH();
	string boundary_name = "boundary_" + generate_image_name(image);
	boundary->set_real_name(boundary_name);
	boundary->datatype = VX_DF_IMAGE_to_hipacc[image->image->col];
	boundary->image = ::generate_image_name(image->image);
	boundary->bc = "@@@BOUNDARY_CONDITION@@@";
	boundary->width = std::to_string(image->image->w);
	boundary->height = std::to_string(image->image->h);

	Kernelcall_Accessor *accessor = new Kernelcall_Accessor();
	string accessor_name = "accessor_" + generate_image_name(image);
	accessor->set_real_name(accessor_name);
	accessor->datatype = VX_DF_IMAGE_to_hipacc[image->image->col];
	accessor->argument = boundary;

	to_return_call_parameters.push_back(accessor);

	to_return.push_back(boundary);
	to_return.push_back(accessor);

	return {to_return_call_parameters,to_return};
}

std::string generate_call(ForEveryPixel *fep)
{
    config_struct_call___ hipacc_call;
    std::vector<Kernelcall_Variable*> kernel_parameters;

    std::tuple<std::vector<Kernelcall_Variable*>, std::vector<Kernelcall_Variable*>> tuple;

    HipaVX::Image *image = std::dynamic_pointer_cast<Image>(fep->output)->image;
    tuple = generator::generate_iterationspace(image);
    kernel_parameters.insert(kernel_parameters.end(), std::get<0>(tuple).begin(), std::get<0>(tuple).end());
    hipacc_call.kcv.insert(hipacc_call.kcv.end(), std::get<1>(tuple).begin(), std::get<1>(tuple).end());

    for(unsigned int i = 0; i < fep->inputs.size(); i++)
    {
        auto node = fep->inputs[i];
        switch(node->type)
        {
        /*case secret::NodeType::Variable:
            tuple = generator::generate_scalar((HipaVX::Scalar*) n->parameters[i]);
            break;*/
        case function_ast::NodeType::Image:
        {
            auto image = std::dynamic_pointer_cast<Image>(node);
            HipaVX::Image *HVX_image = image->image;
            if (i+1 < fep->inputs.size() && fep->inputs[i+1]->type == function_ast::NodeType::Stencil)
            {
                auto stencil = std::dynamic_pointer_cast<Stencil>(fep->inputs[i+1]);
                tuple = generate_accessor(HVX_image, stencil.get());
                i++;
            }
			else
			{
				tuple = generate_accessor(image.get());
			}
            break;
        }
        default:
            throw std::runtime_error("std::string generate_call(ForEveryPixel *fep):\n\tdefault in switchcase");
        }

        kernel_parameters.insert(kernel_parameters.end(), std::get<0>(tuple).begin(), std::get<0>(tuple).end());
        hipacc_call.kcv.insert(hipacc_call.kcv.end(), std::get<1>(tuple).begin(), std::get<1>(tuple).end());
    }

    hipacc_call.kcv.push_back(generator::generate_kernelcall("Kernel", kernel_parameters));
    string s = generator::kernelcall_builder(hipacc_call.kcv);

    s = use_template(s, "ID", fep->id);
    s = use_template(s, "BOUNDARY_CONDITION", "Boundary::UNDEFINED"); // TODO

    return s;
}
}

// move this to domVX_main.cpp
std::vector<HipaVX::Image*> get_all_images(HipaVX::Graph *g)
{
    std::vector<HipaVX::Image*> images;
    std::vector<HipaVX::Node*> nodes = g->graph;

    while(!nodes.empty())
    {
        auto node = nodes[nodes.size() - 1];
        nodes.pop_back();

        for(auto ref: node->inputs)
        {
            if (ref->type == VX_TYPE_IMAGE)
                images.push_back((HipaVX::Image*) ref);
        }
        for(auto ref: node->outputs)
        {
            if (ref->type == VX_TYPE_IMAGE)
                images.push_back((HipaVX::Image*) ref);
        }

        auto subnodes = node->subnodes;
        nodes.insert(nodes.end(), subnodes.begin(), subnodes.end());
    }

    std::sort(images.begin(), images.end());
    images.erase(std::unique(images.begin(), images.end()), images.end());

    return images;
}

std::string generate_source_recursive(std::vector<HipaVX::Node*> nodes, const generator::Type t)
{
    string sources = "";
    for (auto node: nodes)
    {
        if (dynamic_cast<HipaVX::WriteImageNode*>(node) != nullptr)
        {
            if (t == generator::Type::Call)
                sources += generator::node_generator(dynamic_cast<HipaVX::WriteImageNode*>(node), t);
            continue;
        }
        auto subnodes = node->subnodes;
        if (subnodes.empty()) //we are at a fundamental node
        {
            switch (t)
            {
            case generator::Type::Definition:
            {
                HipaccVisitor v;
                sources += v.visit(node->kernel, 0) + '\n';

            } break;
            case generator::Type::Call:
                sources += function_ast::generate_call(node->kernel.get()) + '\n';
                break;
            }
        }
        else
        {
            sources += generate_source_recursive(subnodes, t) + '\n';
        }
    }
    return sources;
}

void process_graph(HipaVX::Graph *graph)
{
    string main = read_file(hipaVX_folder + "/templates/hipacc_main.templ");

    string images;
    const string image_decl_template = "\tImage<@@@DATATYPE@@@> @@@IMAGE_NAME@@@(@@@IMAGE_WIDTH@@@, @@@IMAGE_HEIGHT@@@);\n";

    auto used_images = get_all_images(graph);
    for(const auto image: used_images)
    {
        string image_name = generate_image_name(image);

        string temp = image_decl_template;

        // BAD HACKs
        const HipaVX::FileinputImage* fim = dynamic_cast<const HipaVX::FileinputImage*>(image);
        const HipaVX::Array* arr = dynamic_cast<const HipaVX::Array*>(image);
        if(fim)
        {
            string fim_decl_template;
            if (fim->col == VX_DF_IMAGE_RGBX)
            {
                fim_decl_template = "\t@@@DATATYPE@@@ *@@@IMAGE_NAME@@@_input = (@@@DATATYPE@@@*) load_data<uchar>(@@@IMAGE_WIDTH@@@, @@@IMAGE_HEIGHT@@@, 4, \"@@@FILENAME@@@\");\n"
                                    "\tImage<@@@DATATYPE@@@> @@@IMAGE_NAME@@@(@@@IMAGE_WIDTH@@@, @@@IMAGE_HEIGHT@@@, @@@IMAGE_NAME@@@_input);\n";
            }
            else
            {
                fim_decl_template = "\t@@@DATATYPE@@@ *@@@IMAGE_NAME@@@_input = load_data<@@@DATATYPE@@@>(@@@IMAGE_WIDTH@@@, @@@IMAGE_HEIGHT@@@, 1, \"@@@FILENAME@@@\");\n"
                                    "\tImage<@@@DATATYPE@@@> @@@IMAGE_NAME@@@(@@@IMAGE_WIDTH@@@, @@@IMAGE_HEIGHT@@@, @@@IMAGE_NAME@@@_input);\n";
            }

            temp = fim_decl_template;
            temp = use_template(temp, "FILENAME", fim->file);
        }
        else if(arr)
        {
            continue;//Currently arr is only used in Harris corner
            // There is a bug where reads of images after writes are not transpiled correctly
            // Therefore i write to this array when constructing -> constructor has to be called later
        }

        temp = use_template(temp, "DATATYPE", VX_DF_IMAGE_to_hipacc[image->col]);
        temp = use_template(temp, "IMAGE_NAME", image_name);
        temp = use_template(temp, "IMAGE_WIDTH", image->w);
        temp = use_template(temp, "IMAGE_HEIGHT", image->h);

        images += temp;
    }

    main = use_template(main, "IMAGE_DEFINITIONS", images);

    string node_definitions = generate_source_recursive(graph->graph, generator::Type::Definition);

    main = use_template(main, "KERNEL_DEFINITIONS", node_definitions);

    string node_calls = generate_source_recursive(graph->graph, generator::Type::Call);

    main = use_template(main, "KERNEL_CALLS", node_calls);

    _write_to_file("main.hipaVX.cpp", main);
}

std::string HipaccVisitor::visit(std::shared_ptr<function_ast::Node> n, int i)
{
    switch(n->type)
    {
    case function_ast::NodeType::None:
        throw std::runtime_error("HipaccVisitor visited None");
    case function_ast::NodeType::Add:
    case function_ast::NodeType::Sub:
    case function_ast::NodeType::Mul:
    case function_ast::NodeType::Div:
    case function_ast::NodeType::ShiftLeft:
    case function_ast::NodeType::ShiftRight:
    case function_ast::NodeType::Less:
    case function_ast::NodeType::LessEquals:
    case function_ast::NodeType::Equals:
    case function_ast::NodeType::GreaterEquals:
    case function_ast::NodeType::Greater:
    case function_ast::NodeType::Unequals:
    case function_ast::NodeType::And:
    case function_ast::NodeType::Or:
    case function_ast::NodeType::Xor:
    case function_ast::NodeType::BitwiseAnd:
    case function_ast::NodeType::BitwiseOr:
    case function_ast::NodeType::BitwiseXor:
    {
        auto s = std::dynamic_pointer_cast<function_ast::SimpleBinaryNode>(n);
        std::string op;

        switch(s->type)
        {
        case function_ast::NodeType::Add:
            op = "+";
            break;
        case function_ast::NodeType::Sub:
            op = "-";
            break;
        case function_ast::NodeType::Mul:
            op = "*";
            break;
        case function_ast::NodeType::Div:
            op = "/";
            break;
        case function_ast::NodeType::ShiftLeft:
            op = "<<";
            break;
        case function_ast::NodeType::ShiftRight:
            op = ">>";
            break;
        case function_ast::NodeType::And:
            op = "&&";
            break;
        case function_ast::NodeType::Or:
            op = "||";
            break;
        case function_ast::NodeType::Xor:
            throw std::runtime_error("std::string generate(SimpleBinaryNode *s): No logical XOR available");
        case function_ast::NodeType::BitwiseAnd:
            op = "&";
            break;
        case function_ast::NodeType::BitwiseOr:
            op = "|";
            break;
        case function_ast::NodeType::BitwiseXor:
            op = "^";
            break;
        case function_ast::NodeType::Less:
            op = "<";
            break;
        case function_ast::NodeType::LessEquals:
            op = "<=";
            break;
        case function_ast::NodeType::Equals:
            op = "==";
            break;
        case function_ast::NodeType::GreaterEquals:
            op = ">=";
            break;
        case function_ast::NodeType::Greater:
            op = ">";
            break;
        case function_ast::NodeType::Unequals:
            op = "!=";
            break;
        }

        auto left = this->visit(s->subnodes[0], 0);
        auto right = this->visit(s->subnodes[1], 0);

        return "(" + left + " " + op + " " + right + ")";
    }

    case function_ast::NodeType::BitwiseNot:
    case function_ast::NodeType::Not:
    case function_ast::NodeType::Sqrt:
    case function_ast::NodeType::Exp:
    case function_ast::NodeType::Conversion:
    case function_ast::NodeType::Abs:
    case function_ast::NodeType::Atan2:
    {
        auto s = std::dynamic_pointer_cast<function_ast::SimpleUnaryFunctionNode>(n);
        std::string func;

        switch(s->type)
        {
        case function_ast::NodeType::Sqrt:
            func = "sqrt";
            break;
        case function_ast::NodeType::Exp:
            func = "exp";
            break;
        case function_ast::NodeType::Atan2:
            func = "atan2";
            break;
        case function_ast::NodeType::Abs:
            func = "abs";
            break;
        case function_ast::NodeType::Not:
            func = "!";
            break;
        case function_ast::NodeType::BitwiseNot:
            func = "~";
            break;
        case function_ast::NodeType::Conversion:
            func = "(" + to_string(std::dynamic_pointer_cast<function_ast::Conversion>(s)->to) + ")";
            break;
        }

        auto argument = this->visit(s->subnodes[0], 0);

        return func + "(" + argument + ")";
    }

    case function_ast::NodeType::Constant:
    {
        if (auto c = dynamic_cast<function_ast::Constant<float>*>(n.get()))
        {
            return std::to_string(c->value);
        }
        else if (auto c = dynamic_cast<function_ast::Constant<unsigned char>*>(n.get()))
        {
            return std::to_string(c->value);
        }
        else if (auto c = dynamic_cast<function_ast::Constant<unsigned int>*>(n.get()))
        {
            return std::to_string(c->value);
        }
        else if (auto c = dynamic_cast<function_ast::Constant<int>*>(n.get()))
        {
            return std::to_string(c->value);
        }
        return "CPP Generate: Constant type fail";
    }

    case function_ast::NodeType::Vect4:
    {
        auto s = std::dynamic_pointer_cast<function_ast::Vect4>(n);
        std::string func = "";

        switch(s->to_dtype)
        {
        case function_ast::Datatype::UCHAR4:
            func = "make_uchar4";
            break;
        case function_ast::Datatype::UINT4:
            func = "make_uint4";
            break;
        default:
            std::cerr << "AST: Vect4 is called with an unsupported datatype: " << std::endl;
            exit(1);
            break;
        }

        auto argument = this->visit(s->subnodes[0], 0) + ", "+
                this->visit(s->subnodes[1], 0) + ", "+
                this->visit(s->subnodes[2], 0) + ", "+
                this->visit(s->subnodes[3], 0);
        return func + "("+ argument + ")";
    }

    case function_ast::NodeType::Extract4:
    {
        auto s = std::dynamic_pointer_cast<function_ast::Extract4>(n);
        string channel;
        switch(s->channel)
        {
        case function_ast::VectChannelType::CHANNEL0:
            channel = "x";
            break;
        case function_ast::VectChannelType::CHANNEL1:
            channel = "y";
            break;
        case function_ast::VectChannelType::CHANNEL2:
            channel = "z";
            break;
        case function_ast::VectChannelType::CHANNEL3:
            channel = "w";
            break;
        default:
            throw std::runtime_error("AST: Vect4 is called with an unsupported datatype");
        }

        std::string func = "";
        switch(s->from_dtype)
        {
        case function_ast::Datatype::UCHAR4:
            func = "convert_uchar4";
            break;
        case function_ast::Datatype::UINT4:
            func = "convert_uint4";
            break;
        default:
            std::cerr << "AST: Extract4 is called with an unsupported datatype: " << std::endl;
            exit(1);
            break;
        }

        auto argument = this->visit(s->subnodes[0], 0);
        return func + "("+ argument + ")." + channel;
    }

    case function_ast::NodeType::Variable:
    {
        return std::dynamic_pointer_cast<function_ast::Variable>(n)->name;
    }

    case function_ast::NodeType::VariableDefinition:
    {
        auto s = std::dynamic_pointer_cast<function_ast::VariableDefinition>(n);
        std::string datatype = to_string(std::dynamic_pointer_cast<function_ast::Variable>(s->subnodes[0])->datatype);
        return datatype + " " + this->visit(s->subnodes[0], 0);
    }

    case function_ast::NodeType::Assignment:
    {
        auto s = std::dynamic_pointer_cast<function_ast::Assignment>(n);
        if (s->subnodes[0]->type == function_ast::NodeType::ReductionOutput)
        {
            auto right = this->visit(s->subnodes[1], 0);
            return "return " + right + ";";
        }
        else
        {
            auto left = this->visit(s->subnodes[0], 0);
            auto right = this->visit(s->subnodes[1], 0);

            return left + "=" + right;
        }
    }

    case function_ast::NodeType::TargetPixel:
    {
        return "output()";
    }

    case function_ast::NodeType::If:
    {
        auto s = std::dynamic_pointer_cast<function_ast::If>(n);
        std::string to_return;

        to_return += "if (" + this->visit(s->condition, 0) + ")\n";
        to_return += "{\n";
        to_return += this->visit(s->body, 0);
        to_return += "}\n";

        return to_return;
    }

    case function_ast::NodeType::Else:
    {
        auto s = std::dynamic_pointer_cast<function_ast::Else>(n);
        std::string to_return;

        to_return += "else\n";
        to_return += "{\n";
        to_return += this->visit(s->body, 0);
        to_return += "}\n";

        return to_return;
    }

    case function_ast::NodeType::Image:
    {
        return function_ast::generate_image_name(std::dynamic_pointer_cast<function_ast::Image>(n).get());
    }

    case function_ast::NodeType::ForEveryPixel:
    {
        auto s = std::dynamic_pointer_cast<function_ast::ForEveryPixel>(n);

        string member_variables;
        string constructor_parameters;
        string constructor_init_list;
        string add_accessor;

        string tabs = "\t";

        std::vector<function_ast::Variable*> variables;

        // Dirty hack
        for(auto node: s->inputs) {
            string name="";
            if (node->type == function_ast::NodeType::Variable)
            {
                name = visit(node, i);
            }
            if (node->type == function_ast::NodeType::Image)
            {
                auto image = std::dynamic_pointer_cast<function_ast::Image>(node);
                name = visit(image, i);
            }
            if (node->type == function_ast::NodeType::Stencil)
            {
                auto s = std::dynamic_pointer_cast<function_ast::Stencil>(node);
                name = s->name;
            }

            s->input_params.emplace(name, node);
        }

        for(auto node_it: s->input_params)
        {
            auto node = node_it.second;
            string name = node_it.first;
            string type;
            if (node->type == function_ast::NodeType::Variable)
            {
                type = to_string(std::dynamic_pointer_cast<function_ast::Variable>(node)->datatype);
            }
            if (node->type == function_ast::NodeType::Image)
            {
                auto image = std::dynamic_pointer_cast<function_ast::Image>(node);
                type = "Accessor<" + VX_DF_IMAGE_to_hipacc[image->image->col] + ">&";
            }
            if (node->type == function_ast::NodeType::Stencil)
            {
                auto s = std::dynamic_pointer_cast<function_ast::Stencil>(node);
                type = "Domain&";

                member_variables += tabs + type + " " + name + ";\n";
                constructor_parameters += ", " + type + " " + name;
                constructor_init_list += ", " + name + "(" + name + ")";

                name = name + "_mask";
                type = "Mask<" + to_string(s->datatype) + ">&";
            }

            member_variables += tabs + type + " " + name + ";\n";
            constructor_parameters += ", " + type + " " + name;
            constructor_init_list += ", " + name + "(" + name + ")";


            if (node->type == function_ast::NodeType::Image)
            {
                add_accessor += tabs + '\t' + "add_accessor(&" + name + ");\n";
            }
        }

        if (s->output->type != function_ast::NodeType::Image)
            throw std::runtime_error("std::string generate(ForEveryPixel *s)");

        HipaccVisitor v;
        string kernel = v.visit(s->function, 0);

        string def = read_file(hipaVX_folder + "/templates/hipacc_kernel.templ");
        def = use_template(def, "KERNEL_NAME", "Kernel");
        def = use_template(def, "VARIABLES", member_variables);
        def = use_template(def, "VARIABLES_CONSTRUCTOR_PARAMS", constructor_parameters);
        def = use_template(def, "VARIABLES_CONSTRUCTOR_INIT_LIST", constructor_init_list);
        def = use_template(def, "ADD_ACCESSOR", add_accessor);
        def = use_template(def, "MISC_CONSTRUCTOR", "");
        def = use_template(def, "KERNEL", kernel);
        def = use_template(def, "OUTPUT_DATATYPE", VX_DF_IMAGE_to_hipacc[std::dynamic_pointer_cast<function_ast::Image>(s->output)->image->col]);
        def = use_template(def, "ID", std::to_string(s->id));

        return def;
    }

    case function_ast::NodeType::CurrentPixelvalue:
    {
        auto s = std::dynamic_pointer_cast<function_ast::CurrentPixelvalue>(n);
        return generate_image_name(std::dynamic_pointer_cast<function_ast::Image>(s->subnodes[0]).get()) + "()";
    }

    case function_ast::NodeType::Stencil:
    {
        return "Stencil generate todo";
    }

    case function_ast::NodeType::IterateAroundPixel:
    {
        auto s = std::dynamic_pointer_cast<function_ast::IterateAroundPixel>(n);
        auto stencil = std::dynamic_pointer_cast<function_ast::Stencil>(s->subnodes[1]);
        std::string t = "iterate(@@@DOM_NAME@@@, [&] () -> void {\n"
                        "@@@BODY@@@\n"
                        "})";
        t = use_template(t, "DOM_NAME", stencil->name);
        t = use_template(t, "BODY", this->visit(s->subnodes[2], i));
        return t;
    }

    case function_ast::NodeType::ReductionOutput:
    {
        return "ReductionOutput should not generate";
    }

    case function_ast::NodeType::ReduceAroundPixel:
    {
        auto s = std::dynamic_pointer_cast<function_ast::ReduceAroundPixel>(n);

        auto stencil = std::dynamic_pointer_cast<function_ast::Stencil>(s->subnodes[1]);
        std::string reduction = "";
        switch(s->reduction_type)
        {
        case function_ast::ReduceAroundPixel::Type::SUM:
            reduction = "SUM";
            break;
        case function_ast::ReduceAroundPixel::Type::MIN:
            reduction = "MIN";
            break;
        case function_ast::ReduceAroundPixel::Type::MAX:
            reduction = "MAX";
            break;
        }

        std::string t = "reduce(@@@DOM_NAME@@@, Reduce::@@@REDUCTION@@@, [&] () -> @@@REDUCE_DATATYPE@@@ {\n"
                        "@@@BODY@@@\n"
                        "})";
        t = use_template(t, "REDUCE_DATATYPE", to_string(s->datatype));
        t = use_template(t, "DOM_NAME", stencil->name);
        t = use_template(t, "REDUCTION", reduction);
        t = use_template(t, "BODY", this->visit(s->subnodes[2], i));

        return t;
    }

    case function_ast::NodeType::PixelvalueAtCurrentStencilPos:
    {
        auto s = std::dynamic_pointer_cast<function_ast::PixelvalueAtCurrentStencilPos>(n);
        std::shared_ptr<function_ast::Stencil>stencil;
        std::shared_ptr<function_ast::Image> image;
        function_ast::IterateAroundPixel* iterate;
        function_ast::ReduceAroundPixel* reduce;
        if ((iterate = dynamic_cast<function_ast::IterateAroundPixel*>(s->parent)))
        {
            stencil = std::dynamic_pointer_cast<function_ast::Stencil>(iterate->subnodes[1]);
            image = std::dynamic_pointer_cast<function_ast::Image>(iterate->subnodes[0]);
        }
        else if ((reduce = dynamic_cast<function_ast::ReduceAroundPixel*>(s->parent)))
        {
            stencil = std::dynamic_pointer_cast<function_ast::Stencil>(reduce->subnodes[1]);
            image = std::dynamic_pointer_cast<function_ast::Image>(reduce->subnodes[0]);
        }

        return this->visit(image, i) + "(" + stencil->name + ")";
    }

    case function_ast::NodeType::StencilvalueAtCurrentStencilPos:
    {
        auto s = std::dynamic_pointer_cast<function_ast::StencilvalueAtCurrentStencilPos>(n);
        std::shared_ptr<function_ast::Stencil> stencil;
        function_ast::IterateAroundPixel* iterate;
        function_ast::ReduceAroundPixel* reduce;
        if ((iterate = dynamic_cast<function_ast::IterateAroundPixel*>(s->parent)))
        {
            stencil = std::dynamic_pointer_cast<function_ast::Stencil>(iterate->subnodes[1]);
        }
        else if ((reduce = dynamic_cast<function_ast::ReduceAroundPixel*>(s->parent)))
        {
            stencil = std::dynamic_pointer_cast<function_ast::Stencil>(reduce->subnodes[1]);
        }

        return stencil->name + "_mask" + "(" + stencil->name + ")";
    }

    case function_ast::NodeType::Statements:
    {
        auto s = std::dynamic_pointer_cast<function_ast::Statements>(n);
        std::string to_return;

        for(auto statement: s->statements)
        {
            to_return += this->visit(statement, i);
            if (statement->type != function_ast::NodeType::If && statement->type != function_ast::NodeType::Else)
                to_return += ";\n";
        }

        return to_return;
    }

    case function_ast::NodeType::Pregenerated:
        break;
    case function_ast::NodeType::PixelAccessor:
        break;
    case function_ast::NodeType::WindowAccessor:
        break;
    }
    throw std::runtime_error("Hipacc Generate: reached end of switch case");
}
