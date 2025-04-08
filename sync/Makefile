# Compiler & Flags
CXX := g++
CXXFLAGS := -std=c++17  # C++ version
CXXFLAGS += -Wall -Wextra -pedantic  # generate all warnings
CXXFLAGS += -g  # add GDB instrumentation
CXXFLAGS += -I include
CXXFLAGS += -MMD  # generate .d file with source and header dependencies
CXXFLAGS += -MP  # add phony targets to avoid errors if headers are deleted

# Build directories
BUILD_DIR := build

# Source files
MUTEX_TEST_SRC := src/thread_mutex.cc test/test_thread_mutex.cc

# Object and dependency files in build/
TEST_OBJS := $(addprefix $(BUILD_DIR)/, $(notdir $(MUTEX_TEST_SRC:.cc=.o)))

# Map .d dependency files to object files
DEPS := $(TEST_OBJS:.o=.d)

# Final executables
MUTEX_TEST_EXEC := thread-mutex

# Default target
all: $(MUTEX_TEST_EXEC)

# Build executables
$(MUTEX_TEST_EXEC): $(TEST_OBJS)
	$(CXX) $(TEST_OBJS) -o $@

# Build .o files inside build/
$(BUILD_DIR)/%.o: src/%.cc
	mkdir -p $(BUILD_DIR)
	$(CXX) $(CXXFLAGS) -c $< -o $@

$(BUILD_DIR)/%.o: test/%.cc
	mkdir -p $(BUILD_DIR)
	$(CXX) $(CXXFLAGS) -c $< -o $@

clean:
	rm -rf $(BUILD_DIR) $(MUTEX_TEST_EXEC)

# Include dependency files (.d). Only available in GNU Make.
-include $(DEPS)

.PHONY: all clean
