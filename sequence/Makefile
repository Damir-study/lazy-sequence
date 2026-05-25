# Компилятор
CXX = g++
# Стандарт C++ 
CXXSTD = c++17
# Флаги компиляции
CXXFLAGS = -std=$(CXXSTD) -Wall -Wextra -Wpedantic
# Флаги отладки
DEBUG_FLAGS = -g -O0
# Флаги релиза
RELEASE_FLAGS = -O3 -DNDEBUG
# Флаги инклюдов
INCLUDE_FLAGS = -I.

# Цели
.PHONY: all debug release clean run test help

# Имена исполняемых файлов
DEBUG_TARGET = sequence_debug
RELEASE_TARGET = seq_release

SOURCES = main.cpp

# Вспомогательные файлы шаблонов 
TEMPLATE_FILES = immutable_array_sequence.tpp mutable_array_sequence.tpp \
                 immutable_list_sequence.tpp mutable_list_sequence.tpp \
                 linked_list.tpp dynamic_array.tpp sequence.tpp \
                 array_sequence.tpp list_sequence.tpp \
                 sequence_functions.tpp

HEADERS = immutable_array_sequence.h mutable_array_sequence.h \
          immutable_list_sequence.h mutable_list_sequence.h \
          linked_list.h dynamic_array.h sequence.h \
          array_sequence.h list_sequence.h IEnumerator.h \
          sequence_functions.h

# Полный список зависимостей для сборки
DEPS = $(SOURCES) $(HEADERS) $(TEMPLATE_FILES)

GTEST_DIR = googletest/googletest
GTEST_INC = -I$(GTEST_DIR)/include -I$(GTEST_DIR)
TEST_TARGET = test.exe

gtest-all.o:
	g++ -std=$(CXXSTD) $(GTEST_INC) -c $(GTEST_DIR)/src/gtest-all.cc

gtest_main.o:
	g++ -std=$(CXXSTD) $(GTEST_INC) -c $(GTEST_DIR)/src/gtest_main.cc

tests.o: tests.cpp $(HEADERS) $(TEMPLATE_FILES)
	g++ -std=$(CXXSTD) -I$(GTEST_DIR)/include -I. -c tests.cpp

$(TEST_TARGET): tests.o gtest-all.o gtest_main.o
	g++ tests.o gtest-all.o gtest_main.o -o $(TEST_TARGET) -lpthread

# Цель по умолчанию
all: debug release

# Сборка debug версии
debug: CXXFLAGS += $(DEBUG_FLAGS)
debug: $(DEBUG_TARGET)

# Сборка release версии
release: CXXFLAGS += $(RELEASE_FLAGS)
release: $(RELEASE_TARGET)

# Компиляция debug версии
$(DEBUG_TARGET): $(DEPS)
	$(CXX) $(CXXFLAGS) $(INCLUDE_FLAGS) $(SOURCES) -o $@

# Компиляция release версии
$(RELEASE_TARGET): $(DEPS)
	$(CXX) $(CXXFLAGS) $(INCLUDE_FLAGS) $(SOURCES) -o $@

# Запуск debug версии
run: $(DEBUG_TARGET)
	./$(DEBUG_TARGET)

# Запуск release версии
run_release: $(RELEASE_TARGET)
	./$(RELEASE_TARGET)

# Тестирование (Теперь запускает gtest)
test: $(TEST_TARGET)
	@echo "Запуск Google Tests..."
	./$(TEST_TARGET)

# Очистка
clean:
	rm -f $(DEBUG_TARGET) $(RELEASE_TARGET) $(TEST_TARGET) *.o core

# Справка
help:
	@echo "Makefile for the extended template sequence class"
	@echo ""
	@echo "Available targets:"
	@echo "  all          - build both debug and release versions"
	@echo "  debug        - build with debugging information"
	@echo "  release      - build optimized version"
	@echo "  run          - compile debug version and run"
	@echo "  run_release  - run release version"
	@echo "  test         - compile and run Google Tests"
	@echo "  clean        - remove compiled files"
	@echo "  help         - display this help message"