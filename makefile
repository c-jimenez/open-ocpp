######################################################
#         OCPP library helper makefile               #
######################################################

# Root directory containing the top level CMakeLists.txt file
ROOT_DIR:=$(PWD)

# Generated binary directory
BIN_DIR:=$(ROOT_DIR)/bin

# Make options
#VERBOSE="VERBOSE=1"
PARALLEL_BUILD?=-j 4

# Build type can be either Debug or Release
BUILD_TYPE?=Release

# Default target
default: gcc

# Silent makefile
.SILENT:

# Install prefix
ifneq ($(strip $(INSTALL_PREFIX)),)
CMAKE_INSTALL_PREFIX:=-D CMAKE_INSTALL_PREFIX=$(INSTALL_PREFIX)
CMAKE_INSTALL_PREFIX_CMD:=--prefix $(INSTALL_PREFIX)
endif

# Format code
format:
	@echo "Formatting source code..."
	@find ./src -name '*.h' -or -name '*.cpp' | xargs clang-format -i
	@find ./examples -name '*.h' -or -name '*.cpp' | xargs clang-format -i
	@find ./tests -name '*.h' -or -name '*.cpp' | xargs clang-format -i
	@echo "Formatting done!"

# Build/clean all targets
all: gcc clang
clean: clean-gcc clean-clang
	@-rm -rf $(BIN_DIR)

# Targets for gcc build
GCC_NATIVE_BUILD_DIR:=$(ROOT_DIR)/build_gcc_native
GCC_NATIVE_BIN_DIR:=$(BIN_DIR)/gcc_native
gcc: $(GCC_NATIVE_BUILD_DIR)/Makefile
	@echo "Starting gcc build..."
	@mkdir -p $(GCC_NATIVE_BIN_DIR)
	@make --silent -C $(GCC_NATIVE_BUILD_DIR) $(VERBOSE) $(PARALLEL_BUILD)
	@echo "gcc build done!"

tests-gcc: $(GCC_NATIVE_BUILD_DIR)/Makefile
	@echo "Starting gcc tests..."
	@make --silent -C $(GCC_NATIVE_BUILD_DIR) test ARGS=--output-on-failure
	@echo "gcc tests done!"

clean-gcc:
	@-rm -rf $(GCC_NATIVE_BUILD_DIR)
	@-rm -rf $(GCC_NATIVE_BIN_DIR)
	@echo "gcc build cleaned!"

install-gcc: gcc
	@echo "Installing Open OCPP library compiled with gcc..."
	@cmake --build $(GCC_NATIVE_BUILD_DIR) --target install --config $(BUILD_TYPE)

tests-install-gcc: gcc install-gcc
	@echo "Testing Open OCPP library installation with gcc..."
	@mkdir -p $(GCC_NATIVE_BUILD_DIR)/tests/deploy
	@cd $(GCC_NATIVE_BUILD_DIR)/tests/deploy && export CC=gcc && export CXX=g++ && cmake -D CMAKE_BUILD_TYPE=$(BUILD_TYPE) $(CMAKE_INSTALL_PREFIX) $(ROOT_DIR)/tests/deploy
	@make --silent -C $(GCC_NATIVE_BUILD_DIR)/tests/deploy $(VERBOSE) $(PARALLEL_BUILD)
	@make --silent -C $(GCC_NATIVE_BUILD_DIR)/tests/deploy test ARGS=--output-on-failure
	@echo "gcc build installation checked!"

$(GCC_NATIVE_BUILD_DIR)/Makefile:
	@echo "Generating gcc makefiles..."
	@mkdir -p $(GCC_NATIVE_BUILD_DIR)
	@mkdir -p $(GCC_NATIVE_BIN_DIR)
	@cd $(GCC_NATIVE_BUILD_DIR) && export CC=gcc && export CXX=g++ && cmake -D CMAKE_BUILD_TYPE=$(BUILD_TYPE) -D BIN_DIR=$(GCC_NATIVE_BIN_DIR) $(CMAKE_INSTALL_PREFIX) $(ROOT_DIR)


# Targets for clang build
CLANG_NATIVE_BUILD_DIR:=$(ROOT_DIR)/build_clang_native
CLANG_NATIVE_BIN_DIR:=$(BIN_DIR)/clang_native
clang: $(CLANG_NATIVE_BUILD_DIR)/Makefile
	@echo "Starting clang build..."
	@mkdir -p $(CLANG_NATIVE_BIN_DIR)
	@make --silent -C $(CLANG_NATIVE_BUILD_DIR) $(VERBOSE) $(PARALLEL_BUILD)
	@echo "clang build done!"

tests-clang: $(CLANG_NATIVE_BUILD_DIR)/Makefile
	@echo "Starting clang tests..."
	@make --silent -C $(CLANG_NATIVE_BUILD_DIR) test ARGS=--output-on-failure
	@echo "clang tests done!"

clean-clang:
	@-rm -rf $(CLANG_NATIVE_BUILD_DIR)
	@-rm -rf $(CLANG_NATIVE_BIN_DIR)
	@echo "clang build cleaned!"

install-clang: clang
	@echo "Installing Open OCPP library compiled with clang..."
	@cmake --build $(CLANG_NATIVE_BUILD_DIR) --target install --config $(BUILD_TYPE)

tests-install-clang: clang install-clang
	@echo "Testing Open OCPP library installation with clang..."
	@mkdir -p $(CLANG_NATIVE_BUILD_DIR)/tests/deploy
	@cd $(CLANG_NATIVE_BUILD_DIR)/tests/deploy && export CC=clang && export CXX=clang++ && cmake -D CMAKE_BUILD_TYPE=$(BUILD_TYPE) -D _CMAKE_TOOLCHAIN_PREFIX=llvm- -D BIN_DIR=$(CLANG_NATIVE_BIN_DIR) $(CMAKE_INSTALL_PREFIX) $(ROOT_DIR)/tests/deploy
	@make --silent -C $(CLANG_NATIVE_BUILD_DIR)/tests/deploy $(VERBOSE) $(PARALLEL_BUILD)
	@make --silent -C $(CLANG_NATIVE_BUILD_DIR)/tests/deploy test ARGS=--output-on-failure
	@echo "clang build installation checked!"

$(CLANG_NATIVE_BUILD_DIR)/Makefile:
	@echo "Generating clang makefiles..."
	@mkdir -p $(CLANG_NATIVE_BUILD_DIR)
	@mkdir -p $(CLANG_NATIVE_BIN_DIR)
	@cd $(CLANG_NATIVE_BUILD_DIR) && export CC=clang && export CXX=clang++ && cmake -D CMAKE_BUILD_TYPE=$(BUILD_TYPE) -D _CMAKE_TOOLCHAIN_PREFIX=llvm- -D BIN_DIR=$(CLANG_NATIVE_BIN_DIR) $(CMAKE_INSTALL_PREFIX) $(ROOT_DIR)
