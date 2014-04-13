# Description: unittest Makefile
#
# Copyright (c) 2008 by Baidu.com, Incorporation, P.R.C
# All rights reserved.
# Author: zhangzhibiao01@baidu.com
#

BTEST_DIR=../../btest/

CXXFLAGS=-g -O0

INCPATH= -I$(BTEST_DIR)/gtest/output/include \
		 -I../output/

DEP_LDFLAGS= -L$(BTEST_DIR)/gtest/output/lib \
			 -L../output/

DEP_LDLIBS= -lgtest -lgtest_main -lparser -lpthread

CXX=g++

PATTERN=test*.cpp
TESTFILES=$(wildcard $(PATTERN))
EXE=$(basename $(TESTFILES))
all : $(EXE) output
$(EXE) : % : %.cpp
	$(CXX) $^ -o $@ $(CXXFLAGS) $(INCPATH) $(DEP_LDFLAGS) $(DEP_LDLIBS)
.PHONY: list clean output
list:
	@echo $(EXE)

output:
	rm -rf ./output
	mkdir ./output
	mv $(EXE) ./output/

clean:
	rm -rf ./output
	rm -f $(EXE)
