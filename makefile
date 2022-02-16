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
default: gcc-native

# Silent makefile
.SILENT:

# Install prefix
ifneq ($(strip $(INSTALL_PREFIX)),)
CMAKE_INSTALL_PREFIX:=-D CMAKE_INSTALL_PREFIX=$(INSTALL_PREFIX)
CMAKE_INSTALL_PREFIX_CMD:=--prefix $(INSTALL_PREFIX)
endif

# Build/clean all targets
all: gcc-native clang-native
clean: clean-gcc-native clean-clang-native
	@-rm -rf $(BIN_DIR)

# Targets for gcc-native build
GCC_NATIVE_BUILD_DIR:=$(ROOT_DIR)/build_gcc_native
GCC_NATIVE_BIN_DIR:=$(BIN_DIR)/gcc_native
gcc-native: $(GCC_NATIVE_BUILD_DIR)/Makefile
	@echo "Starting gcc-native build..."
	@mkdir -p $(GCC_NATIVE_BIN_DIR)
	@make --silent -C $(GCC_NATIVE_BUILD_DIR) $(VERBOSE) $(PARALLEL_BUILD)
	@echo "gcc-native build done!"

tests-gcc-native: $(GCC_NATIVE_BUILD_DIR)/Makefile
	@echo "Starting gcc-native tests..."
	@make --silent -C $(GCC_NATIVE_BUILD_DIR) test ARGS=--output-on-failure
	@echo "gcc-native tests done!"

clean-gcc-native:
	@-rm -rf $(GCC_NATIVE_BUILD_DIR)
	@-rm -rf $(GCC_NATIVE_BIN_DIR)
	@echo "gcc-native build cleaned!"

install-gcc-native: gcc-native
	@echo "Installing Open OCPP library compiled with gcc-native..."
	@cmake --build $(GCC_NATIVE_BUILD_DIR) --target install --config $(BUILD_TYPE)

tests-install-gcc-native: gcc-native install-gcc-native
	@echo "Testing Open OCPP library installation with gcc-native..."
	@mkdir -p $(GCC_NATIVE_BUILD_DIR)/tests/deploy
	@cd $(GCC_NATIVE_BUILD_DIR)/tests/deploy && export CC=gcc && export CXX=g++ && cmake -D CMAKE_BUILD_TYPE=$(BUILD_TYPE) -D TARGET=native $(CMAKE_INSTALL_PREFIX) $(ROOT_DIR)/tests/deploy
	@make --silent -C $(GCC_NATIVE_BUILD_DIR)/tests/deploy $(VERBOSE) $(PARALLEL_BUILD)
	@make --silent -C $(GCC_NATIVE_BUILD_DIR)/tests/deploy test ARGS=--output-on-failure
	@echo "gcc-native build installation checked!"

$(GCC_NATIVE_BUILD_DIR)/Makefile:
	@echo "Generating gcc-native makefiles..."
	@mkdir -p $(GCC_NATIVE_BUILD_DIR)
	@mkdir -p $(GCC_NATIVE_BIN_DIR)
	@cd $(GCC_NATIVE_BUILD_DIR) && export CC=gcc && export CXX=g++ && cmake -D CMAKE_BUILD_TYPE=$(BUILD_TYPE) -D TARGET=native -D BIN_DIR=$(GCC_NATIVE_BIN_DIR) $(CMAKE_INSTALL_PREFIX) $(ROOT_DIR)


# Targets for clang-native build
CLANG_NATIVE_BUILD_DIR:=$(ROOT_DIR)/build_clang_native
CLANG_NATIVE_BIN_DIR:=$(BIN_DIR)/clang_native
clang-native: $(CLANG_NATIVE_BUILD_DIR)/Makefile
	@echo "Starting clang native build..."
	@mkdir -p $(CLANG_NATIVE_BIN_DIR)
	@make --silent -C $(CLANG_NATIVE_BUILD_DIR) $(VERBOSE) $(PARALLEL_BUILD)
	@echo "clang native build done!"

tests-clang-native: $(CLANG_NATIVE_BUILD_DIR)/Makefile
	@echo "Starting clang native tests..."
	@make --silent -C $(CLANG_NATIVE_BUILD_DIR) test ARGS=--output-on-failure
	@echo "clang native tests done!"

clean-clang-native:
	@-rm -rf $(CLANG_NATIVE_BUILD_DIR)
	@-rm -rf $(CLANG_NATIVE_BIN_DIR)
	@echo "clang native build cleaned!"

install-clang-native: clang-native
	@echo "Installing Open OCPP library compiled with clang-native..."
	@cmake --build $(CLANG_NATIVE_BUILD_DIR) --target install --config $(BUILD_TYPE)

tests-install-clang-native: clang-native install-clang-native
	@echo "Testing Open OCPP library installation with clang-native..."
	@mkdir -p $(CLANG_NATIVE_BUILD_DIR)/tests/deploy
	@cd $(CLANG_NATIVE_BUILD_DIR)/tests/deploy && export CC=clang && export CXX=clang++ && cmake -D CMAKE_BUILD_TYPE=$(BUILD_TYPE) -D TARGET=native -D _CMAKE_TOOLCHAIN_PREFIX=llvm- -D BIN_DIR=$(CLANG_NATIVE_BIN_DIR) $(CMAKE_INSTALL_PREFIX) $(ROOT_DIR)/tests/deploy
	@make --silent -C $(CLANG_NATIVE_BUILD_DIR)/tests/deploy $(VERBOSE) $(PARALLEL_BUILD)
	@make --silent -C $(CLANG_NATIVE_BUILD_DIR)/tests/deploy test ARGS=--output-on-failure
	@echo "clang-native build installation checked!"

$(CLANG_NATIVE_BUILD_DIR)/Makefile:
	@echo "Generating clang-native makefiles..."
	@mkdir -p $(CLANG_NATIVE_BUILD_DIR)
	@mkdir -p $(CLANG_NATIVE_BIN_DIR)
	@cd $(CLANG_NATIVE_BUILD_DIR) && export CC=clang && export CXX=clang++ && cmake -D CMAKE_BUILD_TYPE=$(BUILD_TYPE) -D TARGET=native -D _CMAKE_TOOLCHAIN_PREFIX=llvm- -D BIN_DIR=$(CLANG_NATIVE_BIN_DIR) $(CMAKE_INSTALL_PREFIX) $(ROOT_DIR)
