.PHONY: run analyze cleanreport wireshark

PROJECT_NAME := static-regexp

SRC_PREFIX := src

TYPE := deploy

include mk/config.mk

APPLICATION_TARGETS := $(wildcard src/benchmark/*.cpp) $(wildcard src/mains/*.cpp) $(wildcard tests/*.test.cpp)

SOURCE_FILES := $(shell find . -name *.cpp | cut -c 3-) 
IGNORE := 
INCLUDE_HEADS := -I. -I/usr/local/include
LIBS +=

override CXXFLAGS += -Wall -Wextra -pedantic -ftemplate-backtrace-limit=0 

include mk/compile.mk

cleanreport:
	@rm -rf report

test: all
	@mk/run_all_tests.sh
