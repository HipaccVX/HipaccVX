#include "config_reader.hpp"

using std::string;



enum class my_state
{
	invalid = -1,
	kernel_variables = 0,
	kernel,
	kernelcall_variables,
	mask,
	name,
	calls
};


std::vector<string> split(const string &text, char sep)
{
	std::vector<string> tokens;
	std::size_t start = 0, end = 0;
	while ((end = text.find(sep, start)) != string::npos)
	{
		string substring = text.substr(start, end - start);
		if (substring.length() != 0)
			tokens.push_back(substring);
		start = end + 1;
	}
	string substring = text.substr(start);
	if (substring.length() != 0)
		tokens.push_back(substring);
	return tokens;
}

string Kernel_General_Variable::generate_kerneldefinition_part()
{
	return datatype + ' ' + name;
}
std::vector<string> Kernel_General_Variable::generate_kernelcall_part()
{
	throw std::runtime_error("unimplemented");
}

string Kernel_Accessor::generate_kerneldefinition_part()
{
	return "Accessor<@@@INPUT_DATATYPE@@@> &" + name;
}
std::vector<string> Kernel_Accessor::generate_kernelcall_part()
{
	string line = "Accessor<@@@INPUT_DATATYPE@@@> @@@NAME@@@_@@@ID@@@(" + bc_name + ");";
	std::vector<string> to_return;
	to_return.emplace_back(line);
	return to_return;
}

string Kernel_Domain::generate_kerneldefinition_part()
{
	return "Domain &" + name;
}
std::vector<string> Kernel_Domain::generate_kernelcall_part()
{
	throw std::runtime_error("unimplemented");
}

string Kernel_Mask::generate_kerneldefinition_part()
{
	return "Mask<" + datatype + "> &" + name;
}
std::vector<string> Kernel_Mask::generate_kernelcall_part()
{
	throw std::runtime_error("unimplemented");
}


// TODO Get a name for this
std::string asd_type(std::string asd)
{
	if (asd == "TYPE_IN")
		return "@@@INPUT_DATATYPE@@@";
	if (asd == "TYPE_OUT")
		return "@@@OUTPUT_DATATYPE@@@";

	if (asd == "IMAGE_IN")
		return "@@@INPUT_IMAGE@@@";
	if (asd == "IMAGE_OUT")
		return "@@@OUTPUT_IMAGE@@@";

	if (asd == "BOUNDARY_CONDITION")
		return "@@@BOUNDARY_CONDITION@@@";

	return asd;
}



string Kernelcall_Mask::generate_kerneldefinition_part()
{
	throw std::runtime_error("unimplemented");
}
std::vector<string> Kernelcall_Mask::generate_kernelcall_part()
{
	std::vector<string> to_return;

	string coef_name = "coef_" + name + "_@@@ID@@@";
	string my_name = name + "_@@@ID@@@";
	to_return.emplace_back("const " + datatype + " " + coef_name +
						   "[" + std::to_string(len_dims[1]) + "][" + std::to_string(len_dims[0]) + "] = {");
	for (int i = 0; i < len_dims[1]; i++)
	{
		string to_add = "\t{";
		for (int j = 0; j < len_dims[0]; j++)
		{
			to_add += flat_mask[i*len_dims[1] + j];
			if (j != len_dims[0] - 1)
				to_add += ", ";
		}
		to_add += "}";
		if (i != len_dims[1] - 1)
			to_add += ",";
		else
			to_add += "};";

		to_return.emplace_back(to_add);
	}

	to_return.emplace_back("Mask<" + datatype + "> " + my_name + "(" + coef_name + ");");

	return to_return;
}
string Kernelcall_Domain::generate_kerneldefinition_part()
{
	throw std::runtime_error("unimplemented");
}
std::vector<string> Kernelcall_Domain::generate_kernelcall_part()
{
	std::vector<string> to_return;

	to_return.emplace_back("Domain " + get_name() + "(");
	to_return.emplace_back("\t" + argument->get_name() + ");");

	return to_return;
}
string Kernelcall_BoundaryCondition::generate_kerneldefinition_part()
{
	throw std::runtime_error("unimplemented");
}
std::vector<string> Kernelcall_BoundaryCondition::generate_kernelcall_part()
{
	std::vector<string> to_return;

	to_return.emplace_back("BoundaryCondition<" + asd_type(datatype) + "> " + get_name() + "(");
	to_return.emplace_back("\t" + asd_type(image) + ",");
	to_return.emplace_back("\t" + argument->get_name() + ",");
	to_return.emplace_back("\t" + asd_type(bc) + ");");

	return to_return;
}
string Kernelcall_Accessor::generate_kerneldefinition_part()
{
	throw std::runtime_error("unimplemented");
}
std::vector<string> Kernelcall_Accessor::generate_kernelcall_part()
{
	std::vector<string> to_return;

	to_return.emplace_back("Accessor<" + asd_type(datatype) + "> " + get_name() + "(");
	to_return.emplace_back("\t" + argument->get_name() + ");");

	return to_return;
}
string Kernelcall_IterationSpace::generate_kerneldefinition_part()
{
	throw std::runtime_error("unimplemented");
}
std::vector<string> Kernelcall_IterationSpace::generate_kernelcall_part()
{
	std::vector<string> to_return;

	string my_name = name + "_@@@ID@@@";

	to_return.emplace_back("IterationSpace<" + asd_type(datatype) + "> " + my_name + "(");
	to_return.emplace_back("\t" + asd_type(image) + ");");

	return to_return;
}

void parse_kernel_variables(std::vector<Kernel_Variable *> &variables, string line)
{
	auto splitted = split(line, ' ');
	if (splitted.size() == 0)
		return;
	if (splitted.size() == 1)
		throw std::runtime_error("Expected at least two tokens in line \"" + line + "\"");

	string type_of_variable = splitted[0];

	if (type_of_variable == "General")
	{
		if (splitted.size() < 3)
			throw std::runtime_error("Expected at least three tokens in line \"" + line + "\"");
		Kernel_General_Variable *gv = new Kernel_General_Variable();
		gv->datatype = splitted[1];
		gv->name = splitted[2];
		variables.push_back(gv);
	}
	else if (type_of_variable == "Accessor")
	{
		Kernel_Accessor *a = new Kernel_Accessor();
		a->name = splitted[1];
		variables.push_back(a);
	}
	else if (type_of_variable == "Domain")
	{
		Kernel_Domain *d = new Kernel_Domain();
		d->name = splitted[1];
		variables.push_back(d);
	}
	else if (type_of_variable == "Mask")
	{
		if (splitted.size() < 3)
			throw std::runtime_error("Expected at least three tokens in line \"" + line + "\"");
		Kernel_Mask *m = new Kernel_Mask();
		m->datatype = splitted[1];
		m->name = splitted[2];
		variables.push_back(m);
	}
	else
	{
		throw std::runtime_error("Unsupported line \"" + line + "\"");
	}
}

Kernelcall_Variable* find_kcv(std::vector<Kernelcall_Variable *> &variables, string name)
{
	for (auto kcv: variables)
	{
		if (kcv->get_real_name() == name)
			return kcv;
	}
	return nullptr;
}

[[noreturn]] void throw_exception(std::string msg, std::string line)
{
	throw std::runtime_error(msg + " - Line: \"" + line + "\"");
}

void parse_kernelcall_variables(std::vector<Kernelcall_Variable *> &variables, string line)
{
	auto splitted = split(line, ' ');
	if (splitted.size() == 0)
		return;
	if (splitted.size() < 3)
		throw_exception("Expected at least three tokens", line);

	string type_of_variable = splitted[0];

	if (type_of_variable == "Mask")
	{
		Kernelcall_Mask *m = new Kernelcall_Mask();
		m->datatype = splitted[1];
		m->set_real_name(splitted[2]);
		variables.push_back(m);
	}
	else if (type_of_variable == "Domain")
	{
		Kernelcall_Domain *d = new Kernelcall_Domain();
		d->set_real_name(splitted[1]);
		d->argument = dynamic_cast<Kernelcall_Mask*> (find_kcv(variables, splitted[2]));
		if (d->argument == nullptr)
			throw_exception("Couldn't find mask variable for domain (mistyped name?)", line);
		variables.push_back(d);
	}
	else if (type_of_variable == "BoundaryCondition")
	{
		if (splitted.size() < 6)
			throw_exception("Expected at least six tokens", line);

		Kernelcall_BoundaryCondition *bc = new Kernelcall_BoundaryCondition();
		bc->datatype = splitted[1];
		bc->set_real_name(splitted[2]);
		bc->image = splitted[3];
		bc->argument = dynamic_cast<Kernelcall_Domain*> (find_kcv(variables, splitted[4]));
		if (bc->argument == nullptr)
			throw_exception("Couldn't find Domain variable for BoundaryCondition (mistyped name?)", line);
		bc->bc = splitted[5];
		variables.push_back(bc);
	}
	else if (type_of_variable == "Accessor")
	{
		Kernelcall_Accessor *a = new Kernelcall_Accessor();
		a->datatype = splitted[1];
		a->set_real_name(splitted[2]);
		a->argument = dynamic_cast<Kernelcall_BoundaryCondition*> (find_kcv(variables, splitted[3]));
		if (a->argument == nullptr)
			throw_exception("Couldn't find BoundaryCondition variable for Accessor (mistyped name?)", line);
		variables.push_back(a);
	}
	else if (type_of_variable == "IterationSpace")
	{
		if (splitted.size() < 4)
			throw_exception("Expected at least four tokens", line);
		Kernelcall_IterationSpace *is = new Kernelcall_IterationSpace();
		is->datatype = splitted[1];
		is->set_real_name(splitted[2]);
		is->image = splitted[3];
		variables.push_back(is);
	}
	else
	{
		throw std::runtime_error("Unsupported line \"" + line + "\"");
	}
}

void parse_kernelcall_mask(std::vector<Kernelcall_Variable *> &variables, string line, bool& switched_category)
{
	auto splitted = split(line, ' ');
	if (splitted.size() == 0)
		return;

	static Kernelcall_Mask *current = nullptr;
	if (switched_category)
	{
		current = dynamic_cast<Kernelcall_Mask*> (find_kcv(variables, splitted[0]));
		if (current == nullptr)
			throw_exception("Couldn't find Mask variable (mistyped name?)", line);
		switched_category = false;
		current->len_dims[0] = -1;
		return;
	}

	if (current->len_dims[0] == -1)
	{
		current->len_dims[0] = static_cast<int>(splitted.size());
		current->len_dims[1] = 0;
	}
	else if (current->len_dims[0] != static_cast<int>(splitted.size()))
	{
		throw_exception("X Dimension of Mask should always have the same length", line);
	}
	current->flat_mask.insert(current->flat_mask.end(), splitted.begin(), splitted.end());
	current->len_dims[1]++;
}

config_struct___ read_config(string file)
{
	std::ifstream in(file);

	string line;
	my_state state = my_state::invalid;

	std::vector<Kernel_Variable*> kernel_variables;
	std::vector<Kernelcall_Variable*> kernelcall_variables;
	std::vector<string> kernel;
	std::vector<std::vector<Kernelcall_Variable *>> kernel_calls;

	std::string kernel_name;

	bool switched_category = true;
	while (std::getline(in, line))
	{
		if (line == "[KERNEL_VARIABLES]")
		{
			state = my_state::kernel_variables;
			switched_category = true;
		}
		else if (line == "[KERNEL]")
		{
			state = my_state::kernel;
			switched_category = true;
		}
		else if (line == "[KERNEL_END]")
		{
			state = my_state::invalid;
			switched_category = true;
		}
		else if (line == "[KERNELCALL_VARIABLES]")
		{
			state = my_state::kernelcall_variables;
			switched_category = true;
		}
		else if (line == "[MASK]")
		{
			state = my_state::mask;
			switched_category = true;
		}
		else if (line == "[NAME]")
		{
			state = my_state::name;
			switched_category = true;
		}
		else if (line == "[KERNEL_CALLS]")
		{
			state = my_state::calls;
			switched_category = true;
		}

		else
		{
			switch (state)
			{
			case my_state::kernel_variables:
				parse_kernel_variables(kernel_variables, line);
				break;
			case my_state::kernel:
				kernel.push_back(line);
				break;
			case my_state::kernelcall_variables:
				parse_kernelcall_variables(kernelcall_variables, line);
				break;
			case my_state::mask:
				parse_kernelcall_mask(kernelcall_variables, line, switched_category);
				break;
			case my_state::name:
			{
				auto splitted = split(line, ' ');
				if (splitted.size() != 0)
					kernel_name = splitted[0];
			}	break;
			case my_state::calls:
			{
				auto splitted = split(line, ' ');
				if (splitted.size() != 0)
				{
					std::vector<Kernelcall_Variable*> kernel_call_parameters;
					for(const auto& variable_name: splitted)
					{
						auto kcv = find_kcv(kernelcall_variables, variable_name);
						if (kcv == nullptr)
							throw_exception("Couldn't find variable \"" + variable_name + "\"", line);
						kernel_call_parameters.emplace_back(kcv);
					}
					kernel_calls.emplace_back(kernel_call_parameters);
				}
			}	break;
			case my_state::invalid:
			default:
				break;
			}
		}
	}

	config_struct___ cs;
	cs.kv = kernel_variables;
	cs.k = kernel;
	cs.kcv = kernelcall_variables;
	cs.kc = kernel_calls;
	return cs;
}
