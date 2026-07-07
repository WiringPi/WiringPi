CXX      := g++
CXXFLAGS := -std=c++17 -Wall -Wextra -Werror -g \
	-Igoogletest-main/googletest/include -Igoogletest-main/googlemock/include
LDFLAGS  := -pthread
WIRING_PI_LIBS := -lwiringPi

# Where to download googletest
GTEST_URL := https://github.com/google/googletest/archive/refs/heads/main.zip
GTEST_ZIP := googletest.zip
GTEST_DIR := googletest-main

# Output directory
BIN_DIR := bin

# Helper program
RUN_ISR_SRC := run_isr.cpp
RUN_ISR_BIN := $(BIN_DIR)/run_isr

# Need BCM17 <-> BCM18 connection
TEST_SRCS := \
    test_isr_both.cpp \
    test_isr_conflict_between_processes.cpp \
    test_isr_falling.cpp \
    test_isr_restart.cpp \
    test_isr_restart2.cpp \
    test_isr_rising.cpp \
    test_isr_rising2.cpp

# Derive test binaries (in bin/)
TEST_BINS := $(addprefix $(BIN_DIR)/, $(TEST_SRCS:.cpp=))

# gtest/gmock sources
GTEST_SRCS := $(GTEST_DIR)/googletest/src/gtest-all.cc
GMOCK_SRCS := $(GTEST_DIR)/googlemock/src/gmock-all.cc
GTEST_MAIN := $(GTEST_DIR)/googletest/src/gtest_main.cc
GTEST_INCLUDES := -I$(GTEST_DIR)/googletest/include -I$(GTEST_DIR)/googlemock/include

GTEST_OBJS := $(GTEST_SRCS:.cc=.o)
GMOCK_OBJS := $(GMOCK_SRCS:.cc=.o)
GTEST_MAIN_OBJ := $(GTEST_MAIN:.cc=.o)

# Default: build everything
all: $(GTEST_DIR) $(BIN_DIR) $(TEST_BINS) $(RUN_ISR_BIN)

# Rule: build normal test binaries
$(filter-out $(BIN_DIR)/test_conflict_between_processes,$(TEST_BINS)): $(BIN_DIR)/%: %.cpp $(GTEST_OBJS) $(GMOCK_OBJS) $(GTEST_MAIN_OBJ)
	$(CXX) $(CXXFLAGS) $< $(GTEST_OBJS) $(GMOCK_OBJS) $(GTEST_MAIN_OBJ) -o $@ $(LDFLAGS) $(WIRING_PI_LIBS)

# Special case: test_conflict_between_processes depends on run_isr
$(BIN_DIR)/test_conflict_between_processes: test_conflict_between_processes.cpp $(GTEST_OBJS) $(GMOCK_OBJS) $(GTEST_MAIN_OBJ) $(RUN_ISR_BIN)
	$(CXX) $(CXXFLAGS) $< $(GTEST_OBJS) $(GMOCK_OBJS) $(GTEST_MAIN_OBJ) -o $@ $(LDFLAGS) $(WIRING_PI_LIBS)

# Build run_isr
$(RUN_ISR_BIN): $(RUN_ISR_SRC) | $(BIN_DIR)
	$(CXX) $(CXXFLAGS) $< -o $@ $(WIRING_PI_LIBS)

# Ensure bin directory exists
$(BIN_DIR):
	mkdir -p $(BIN_DIR)

# Download googletest if not already present
$(GTEST_DIR):
	@echo "Downloading GoogleTest..."
	curl -L $(GTEST_URL) -o $(GTEST_ZIP)
	unzip -q $(GTEST_ZIP)
	rm -f $(GTEST_ZIP)

# Generic rules for compiling gtest/gmock sources
%.o: %.cc
	$(CXX) $(CXXFLAGS) $(GTEST_INCLUDES) -I$(GTEST_DIR)/googletest -I$(GTEST_DIR)/googlemock -c $< -o $@

# Run all tests
test: all
	@failed=0; \
	for t in $(TEST_BINS); do \
	    echo "Running $$t..."; \
	    ./$$t || failed=1; \
	done; \
	if [ $$failed -eq 0 ]; then \
	    echo "✅ All tests passed."; \
	else \
	    echo "❌ Some tests failed."; \
	    exit 1; \
	fi

clean:
	rm -f $(TEST_BINS) $(RUN_ISR_BIN)

distclean: clean
	rm -rf $(GTEST_DIR) $(BIN_DIR)

.PHONY: all clean distclean test