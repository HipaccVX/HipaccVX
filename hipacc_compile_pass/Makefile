# Search binaries
CXX        := $(shell which c++)
PKG_CONFIG := $(shell which pkg-config)

################################################################################

# Search Hipacc (prefer relative path over package path over PATH env var)
#HIPACC_DIR ?= "/home/burak/Documents/Uni/hipacc/installation/hipacc-0.8.2"
HIPACC_DIR          ?= "/home/burak/Documents/Uni/hipacc/installation/hipacc-0.8.2"
CLANG_INCLUDE_PATH   ?= "/home/burak/Documents/Uni/hipacc/installation/hipacc-0.8.2/include/clang"
LIBCXX_INCLUDE_PATH ?= "/home/burak/Documents/Uni/hipacc/installation/hipacc-0.8.2/include/c++/v1"
#HIPACC_DIR          ?= "/scratch-local2/projects/hipacc/hipacc-install"
#LLVM_INCLUDE_PATH   ?= "/scratch-local2/opt/llvm/llvm_install-5.0.1/lib/clang/5.0.1/include/"
#LIBCXX_INCLUDE_PATH ?= "/scratch-local2/opt/libcxx/libcxx-50_/install/include/c++/v1"

HIPACC := "$(HIPACC_DIR)/bin/hipacc"
ifndef HIPACC
  $(error "Error: Could not find binary 'hipacc' in PATH")
endif

################################################################################

# Set compiler options
CXX_FLAGS     = -std=c++11 -O2
CXX_INCLUDE   = -I $(HIPACC_DIR)/include
CXX_LIB_DIR   = -L $(HIPACC_DIR)/lib
CXX_LINK      = -lhipaccRuntime

OCL_INCLUDE   = $(CXX_INCLUDE)
OCL_LIB_DIR   = $(CXX_LIB_DIR)
OCL_LINK      = $(CXX_LINK) -lOpenCL

HIPACC_FLAGS   = -std=c++11  -nostdinc++
HIPACC_INCLUDE = -I $(LIBCXX_INCLUDE_PATH)  \
                 -I $(CLANG_INCLUDE_PATH)  \
				 -I $(HIPACC_DIR)/include/dsl  \
                 -I ./hipaVX

ifdef PKG_CONFIG
# Check for OpenCV
ifeq ($(shell if $(PKG_CONFIG) --exists opencv; then echo true; fi),true)
CXX_FLAGS   += $(shell $(PKG_CONFIG) --cflags-only-other opencv) -DUSE_OPENCV
CXX_INCLUDE += $(shell $(PKG_CONFIG) --cflags-only-I opencv)
CXX_LIB_DIR += $(shell $(PKG_CONFIG) --libs-only-L opencv)
CXX_LINK    += $(shell $(PKG_CONFIG) --libs-only-l opencv)
endif
endif

################################################################################

# Turn on GNU Make feature for using automatic variables in dependencies
.SECONDEXPANSION:
.PHONY: cpu cuda opencl-cpu opencl-gpu renderscript filterscript
.PRECIOUS: main_%.cc

################################################################################

# Target rules
all: cpu

# Run CPU/CUDA/OpenCL binary
cpu cuda opencl-cpu opencl-gpu: main_$$@
	./main_$@

# Run Hipacc
main_%.cc: ../main.hipaVX.cpp
	$(HIPACC) -emit-$* $(HIPACC_FLAGS) $(HIPACC_INCLUDE) $(CURDIR)/$< -o $@

# Build CPU
main_cpu: $$@.cc
	$(CXX) $(CXX_FLAGS) $< $(CXX_INCLUDE) $(CXX_LIB_DIR) $(CXX_LINK) -o $@

# Build OpenCL-CPU or OpenCL-GPU
main_opencl-%: $$@.cc
	$(CXX) $(CXX_FLAGS) $< $(OCL_INCLUDE) $(OCL_LIB_DIR) $(OCL_LINK) -o $@

clean:
	rm -rf jni libs obj
	rm -f *cc *.cu *.cubin *.rs *.fs *.jpg akif-200x300_bw_*.png

distclean: clean
	rm -f main_* *.cl
