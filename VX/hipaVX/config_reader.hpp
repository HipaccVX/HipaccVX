#pragma once

#include <fstream>
#include <string>
#include <sstream>
#include <map>
#include <vector>



class Kernel_Variable
{
public:
	std::string name;

	virtual ~Kernel_Variable() = default;
	virtual std::string generate_kerneldefinition() = 0;
	virtual bool is_accessor() {return false;}
};

class Kernel_General_Variable: public Kernel_Variable
{
public:
	std::string datatype;

	virtual ~Kernel_General_Variable() override = default;
	virtual std::string generate_kerneldefinition() override;
};

class Kernel_Accessor: public Kernel_Variable
{
public:
	std::string bc_name;
	virtual ~Kernel_Accessor() override = default;
	virtual std::string generate_kerneldefinition() override;
	virtual bool is_accessor() override {return true;}
};

class Kernel_Domain: public Kernel_Variable
{
public:
	std::string mask;
	virtual ~Kernel_Domain() override = default;
	virtual std::string generate_kerneldefinition() override;
};

class Kernel_Mask: public Kernel_Variable
{
public:
	std::string datatype;
	virtual ~Kernel_Mask() override = default;
	virtual std::string generate_kerneldefinition() override;
};






class Kernelcall_Variable
{
protected:
	std::string name;

public:
	virtual std::string get_name() {return name + "_@@@ID@@@";}
	virtual std::string get_real_name() {return name;}
	virtual void set_real_name(std::string new_name) {name = new_name;}

	virtual ~Kernelcall_Variable() = default;
	virtual std::vector<std::string> generate_kernelcall() = 0;
};

class Kernelcall_General_Variable: public Kernelcall_Variable
{
public:
	std::string datatype;

	virtual ~Kernelcall_General_Variable() override = default;
	virtual std::vector<std::string> generate_kernelcall() override;
};

class Kernelcall_Mask: public Kernelcall_Variable
{
public:
	int len_dims[2];
	std::vector<std::string> flat_mask;
	std::string datatype;
	virtual ~Kernelcall_Mask() override = default;
	virtual std::vector<std::string> generate_kernelcall() override;
};

class Kernelcall_Domain: public Kernelcall_Variable
{
public:
	Kernelcall_Mask* argument;

	std::string width;
	std::string height;
	virtual ~Kernelcall_Domain() override = default;
	virtual std::vector<std::string> generate_kernelcall() override;
};

class Kernelcall_BoundaryCondition: public Kernelcall_Variable
{
public:
	std::string datatype;
	std::string image;
	std::string bc;
	virtual ~Kernelcall_BoundaryCondition() override = default;
};

class Kernelcall_BoundaryCondition_from_Dom: public Kernelcall_BoundaryCondition
{
public:
	Kernelcall_Domain* argument;
	virtual ~Kernelcall_BoundaryCondition_from_Dom() override = default;
	virtual std::vector<std::string> generate_kernelcall() override;
};
// From Width/Height
class Kernelcall_BoundaryCondition_from_WH: public Kernelcall_BoundaryCondition
{
public:
	std::string width;
	std::string height;
	virtual ~Kernelcall_BoundaryCondition_from_WH() override = default;
	virtual std::vector<std::string> generate_kernelcall() override;
};

class Kernelcall_Accessor: public Kernelcall_Variable
{
public:
	std::string datatype;
	Kernelcall_BoundaryCondition* argument;
	virtual ~Kernelcall_Accessor() override = default;
	virtual std::vector<std::string> generate_kernelcall() override;
};

class Kernelcall_IterationSpace: public Kernelcall_Variable
{
public:
	std::string datatype;
	std::string image;
	virtual ~Kernelcall_IterationSpace() override = default;
	virtual std::vector<std::string> generate_kernelcall() override;
};

class Kernelcall: public Kernelcall_Variable
{
public:
	std::string kernel_name;
	std::vector<Kernelcall_Variable*> arguments;
	virtual ~Kernelcall() override = default;
	virtual std::vector<std::string> generate_kernelcall() override;
};




// TODO good name?
struct config_struct_def___
{
	std::string name;
	std::vector<Kernel_Variable*> kv;					// Kernelmembervariables
	std::vector<std::string> k;							// The actual kernel code
};

// TODO good name?
struct config_struct_call___
{
	std::vector<Kernelcall_Variable*> kcv;				// Variables needed to call the kernel
	std::vector<std::vector<Kernelcall_Variable *>> kc;	// The kernel calls
};


config_struct_def___ read_config_def(std::string file);
config_struct_call___ read_config_call(std::string file);




