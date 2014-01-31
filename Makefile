.PHONY: run analyze cleanreport wireshark

PROJECT_NAME := static-regexp

SRC_PREFIX := src

TYPE := deploy

include mk/config.mk

APPLICATION_TARGETS := $(wildcard src/benchmark/*.cpp) $(wildcard src/mains/*.cpp)

SOURCE_FILES := $(shell find . -name *.cpp | cut -c 3-) 
IGNORE := 
INCLUDE_HEADS := -I. 
LIBS += 

override CXXFLAGS += -Wall -Wextra -pedantic

include mk/compile.mk

cleanreport:
	@rm -rf report
