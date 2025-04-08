# Compiler & Flags
CXX := g++
CXXFLAGS := -std=c++17  # C++ version
CXXFLAGS += -Wall -Wextra -pedantic  # generate all warnings
CXXFLAGS += -g  # add GDB instrumentation
CXXFLAGS += -I include -I ../sync/include 
CXXFLAGS += -MMD  # generate .d file with source and header dependencies
CXXFLAGS += -MP  # add phony targets to avoid errors if headers are deleted

# Build directories
BUILD_DIR := build

# Source files
SYNC_SRC := ../sync/src/thread_mutex.cc
BANKERS_SRC := src/bankers_resource_manager.cc
THREAD_SRC := src/bankers_thread.cc

# Object and dependency files in build
THREAD_OBJS := $(addprefix $(BUILD_DIR)/, $(notdir $(THREAD_SRC:.cc=.o))) \
							 $(addprefix $(BUILD_DIR)/, $(notdir $(BANKERS_SRC:.cc=.o))) \
               $(addprefix $(BUILD_DIR)/, $(notdir $(SYNC_SRC:.cc=.o)))

# Map .d dependency files to object files
DEPS := $(THREAD_OBJS:.o=.d)

# Final executables
THREAD_EXEC := bankers-threads

# Default target
all: $(THREAD_EXEC)

# Build executables
$(THREAD_EXEC): $(THREAD_OBJS)
	$(CXX) $(THREAD_OBJS) -o $@

# Build .o files inside build/
$(BUILD_DIR)/%.o: ../sync/src/%.cc
	mkdir -p $(BUILD_DIR)
	$(CXX) $(CXXFLAGS) -c $< -o $@

$(BUILD_DIR)/%.o: src/%.cc
	mkdir -p $(BUILD_DIR)
	$(CXX) $(CXXFLAGS) -c $< -o $@

clean:
	rm -rf $(BUILD_DIR) $(BANKERS_EXEC) $(THREAD_EXEC)

# Include dependency files (.d). Only available in GNU Make. The '-' makes this
# fail silently. Works just like #include from C/C++ in that it "copies" the
# dependency files' contents here in the makefile.
-include $(DEPS)

.PHONY: all clean
