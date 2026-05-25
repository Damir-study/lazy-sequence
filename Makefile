CXX = g++
CXXSTD = c++17
CXXFLAGS = -std=$(CXXSTD) -Wall -Wextra -Wpedantic
DEBUG_FLAGS = -g -O0
RELEASE_FLAGS = -O3 -DNDEBUG
INCLUDE_FLAGS = -I. -Isequence

.PHONY: all debug release run run_release test clean help

DEBUG_TARGET = lab4_ui.exe
RELEASE_TARGET = lab4_release.exe
TEST_TARGET = lab4_tests.exe

SOURCES = lab4_main.cpp substring_counter.cpp ordinal.cpp
TEST_SOURCES = lab4_tests.cpp substring_counter.cpp ordinal.cpp

ROOT_HEADERS = generator.hpp lazy_sequence.hpp ordinal.hpp \
               read_only_stream.hpp substring_counter.hpp write_only_stream.hpp

ROOT_TEMPLATES = generator.tpp lazy_sequence.tpp \
                 read_only_stream.tpp write_only_stream.tpp

SEQUENCE_HEADERS = sequence/array_sequence.h sequence/dynamic_array.h \
                   sequence/IEnumerator.h sequence/immutable_array_sequence.h \
                   sequence/immutable_list_sequence.h sequence/linked_list.h \
                   sequence/list_sequence.h sequence/mutable_array_sequence.h \
                   sequence/mutable_list_sequence.h sequence/sequence.h \
                   sequence/sequence_functions.h

SEQUENCE_TEMPLATES = sequence/array_sequence.tpp sequence/dynamic_array.tpp \
                     sequence/immutable_array_sequence.tpp \
                     sequence/immutable_list_sequence.tpp \
                     sequence/linked_list.tpp sequence/list_sequence.tpp \
                     sequence/mutable_array_sequence.tpp \
                     sequence/mutable_list_sequence.tpp sequence/sequence.tpp \
                     sequence/sequence_functions.tpp

DEPS = $(SOURCES) $(ROOT_HEADERS) $(ROOT_TEMPLATES) \
       $(SEQUENCE_HEADERS) $(SEQUENCE_TEMPLATES)

TEST_DEPS = $(TEST_SOURCES) $(ROOT_HEADERS) $(ROOT_TEMPLATES) \
            $(SEQUENCE_HEADERS) $(SEQUENCE_TEMPLATES)

GTEST_DIR = sequence/googletest/googletest
GTEST_INC = -I$(GTEST_DIR)/include -I$(GTEST_DIR)
GTEST_ALL_OBJ = lab4_gtest_all.o
GTEST_MAIN_OBJ = lab4_gtest_main.o

all: debug release

debug: CXXFLAGS += $(DEBUG_FLAGS)
debug: $(DEBUG_TARGET)

release: CXXFLAGS += $(RELEASE_FLAGS)
release: $(RELEASE_TARGET)

$(DEBUG_TARGET): $(DEPS)
	$(CXX) $(CXXFLAGS) $(INCLUDE_FLAGS) $(SOURCES) -o $@

$(RELEASE_TARGET): $(DEPS)
	$(CXX) $(CXXFLAGS) $(INCLUDE_FLAGS) $(SOURCES) -o $@

$(GTEST_ALL_OBJ):
	$(CXX) -std=$(CXXSTD) $(GTEST_INC) -c $(GTEST_DIR)/src/gtest-all.cc -o $@

$(GTEST_MAIN_OBJ):
	$(CXX) -std=$(CXXSTD) $(GTEST_INC) -c $(GTEST_DIR)/src/gtest_main.cc -o $@

$(TEST_TARGET): $(TEST_DEPS) $(GTEST_ALL_OBJ) $(GTEST_MAIN_OBJ)
	$(CXX) -std=$(CXXSTD) $(INCLUDE_FLAGS) $(GTEST_INC) \
		$(TEST_SOURCES) $(GTEST_ALL_OBJ) $(GTEST_MAIN_OBJ) -o $@ -lpthread

run: $(DEBUG_TARGET)
	./$(DEBUG_TARGET)

run_release: $(RELEASE_TARGET)
	./$(RELEASE_TARGET)

test: $(TEST_TARGET)
	@echo "Running lab4 Google Tests..."
	./$(TEST_TARGET)

clean:
	rm -f $(DEBUG_TARGET) $(RELEASE_TARGET) $(TEST_TARGET) \
	      $(GTEST_ALL_OBJ) $(GTEST_MAIN_OBJ) *.o core

help:
	@echo "Makefile for lab4 lazy sequences, streams and substring counter"
	@echo ""
	@echo "Available targets:"
	@echo "  all          - build debug and release versions"
	@echo "  debug        - build lab4_ui.exe with debug flags"
	@echo "  release      - build optimized lab4_release.exe"
	@echo "  run          - build and run lab4_ui.exe"
	@echo "  run_release  - build and run lab4_release.exe"
	@echo "  test         - build and run lab4_tests.exe"
	@echo "  clean        - remove lab4 build files"
	@echo "  help         - show this help"
