# Copyright (c) 2020, Lawrence Livermore National Security, LLC and other
# bindee developers. See the top-level LICENSE file for details.
# 
# SPDX-License-Identifier: MIT
CXX=clang++
CXXFLAGS=$(shell llvm-config --cxxflags)
LDFLAGS=-Wl,--start-group \
	-lclangAST \
	-lclangASTMatchers \
	-lclangAnalysis \
	-lclangBasic \
	-lclangDriver \
	-lclangFrontend \
	-lclangLex \
	-lclangParse \
	-lclangSema \
	-lclangEdit \
	-lclangSerialization \
	-lclangTooling \
	-Wl,--end-group \
	$(shell llvm-config --ldflags --libs --system-libs)

#######################
#    DEFAULT FLAGS    #
#######################
STDCPP=-std=c++11
EXTRA_FLAGS=

# clang builtin header: $(dirname /path/to/clang)/../lib/clang/${CLANG_VER}/include
CLANG_INCLUDE=-I$(strip $(shell $(CXX) -xc++ -E -v - </dev/null 2>&1 | grep '^ /.*/clang/[^/]*/include'))

DEFAULT_FLAGS=$(STDCPP) $(EXTRA_FLAGS) $(CLANG_INCLUDE)

##############
#    MAKE    #
##############
SRC_DIR=src
INC_DIR=-I$(SRC_DIR)
SRCS=main.cc callbacks.cc database.cc bindee.cc diagnostics.cc property.cc
SRCS:=$(foreach SRC, $(SRCS), $(SRC_DIR)/$(SRC))
OBJS=$(SRCS:.cc=.o)
DEPS=$(OBJS:.o=.d)

all: bindee

bindee: $(OBJS)
	$(CXX) $^ $(LDFLAGS) -o $@

$(SRC_DIR)/main.o: $(SRC_DIR)/main.cc
	$(CXX) $(CXXFLAGS) $(INC_DIR) -DDEFAULT_FLAGS='$(DEFAULT_FLAGS)' -MMD -MP -o $@ -c $<

%.o: %.cc
	$(CXX) $(CXXFLAGS) $(INC_DIR) -MMD -MP -o $@ -c $<

-include $(DEPS)

test: bindee
	$(CURDIR)/tests/test.sh bindee

clean:
	rm -f bindee $(OBJS) $(DEPS)

.PHONY: all clean
