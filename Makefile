# GMRedis Makefile
# Convenience targets for common development tasks

.PHONY: all setup build test test-unit test-acceptance clean rebuild \
        format lint check install help debug release

# Default target
all: build

# Build directory
BUILD_DIR := build
BUILD_TYPE ?= Release

# Detect if running in devcontainer
ifdef REMOTE_CONTAINERS
    IN_CONTAINER := 1
endif

# Setup: Install Conan dependencies and configure CMake
setup:
	@echo "==> Setting up Conan dependencies..."
	conan install . --output-folder=$(BUILD_DIR) --build=missing -s build_type=$(BUILD_TYPE)
	@echo "==> Configuring CMake..."
	cmake -S . -B $(BUILD_DIR) \
		-DCMAKE_TOOLCHAIN_FILE=$(BUILD_DIR)/conan_toolchain.cmake \
		-DCMAKE_BUILD_TYPE=$(BUILD_TYPE) \
		-G Ninja
	@echo "==> Setup complete!"

# Build the project
build: $(BUILD_DIR)/CMakeCache.txt
	@echo "==> Building project..."
	cmake --build $(BUILD_DIR) --config $(BUILD_TYPE) -j $$(nproc 2>/dev/null || sysctl -n hw.ncpu 2>/dev/null || echo 4)

# Ensure CMake is configured
$(BUILD_DIR)/CMakeCache.txt:
	@echo "==> Build not configured. Running setup..."
	$(MAKE) setup

# Run all tests
test: build
	@echo "==> Running all tests..."
	cd $(BUILD_DIR) && ctest -C $(BUILD_TYPE) --output-on-failure

# Run unit tests only
ut: build
	@echo "==> Running unit tests..."
	cd $(BUILD_DIR) && ctest -C $(BUILD_TYPE) --output-on-failure -L unit

# Run acceptance tests only
uat: build
	@echo "==> Running acceptance tests..."
	cd $(BUILD_DIR) && ctest -C $(BUILD_TYPE) --output-on-failure -L acceptance

# Debug build
debug:
	$(MAKE) BUILD_TYPE=Debug setup build

# Release build
release:
	$(MAKE) BUILD_TYPE=Release setup build

# Clean build artifacts
clean:
	@echo "==> Cleaning build directory..."
	rm -rf $(BUILD_DIR)
	@echo "==> Clean complete!"

# Clean and rebuild
rebuild: clean all

# Format source code
format:
	@echo "==> Formatting source code..."
	find lib src tests -name '*.cpp' -o -name '*.hpp' | xargs clang-format -i
	@echo "==> Formatting complete!"

# Check formatting without modifying files
format-check:
	@echo "==> Checking code format..."
	find lib src tests -name '*.cpp' -o -name '*.hpp' | xargs clang-format --dry-run --Werror

# Run static analysis
lint:
	@echo "==> Running static analysis..."
	cppcheck --enable=all --error-exitcode=1 \
		--suppress=missingIncludeSystem \
		--suppress=unusedFunction \
		-I lib/gmredis/include \
		lib src

# Run all checks (format + lint)
check: format-check lint
	@echo "==> All checks passed!"

# Install the project
install: build
	@echo "==> Installing..."
	cmake --install $(BUILD_DIR)

# Run the server
run-server: build
	@echo "==> Starting GMRedis server..."
	./$(BUILD_DIR)/src/server/gmredis-server

# Run the client
run-client: build
	@echo "==> Starting GMRedis client..."
	./$(BUILD_DIR)/src/client/gmredis-cli

# Help
help:
	@echo "GMRedis Build System"
	@echo ""
	@echo "Usage: make [target] [BUILD_TYPE=Debug|Release]"
	@echo ""
	@echo "Targets:"
	@echo "  all             Build the project (default)"
	@echo "  setup           Install dependencies and configure CMake"
	@echo "  build           Build the project"
	@echo "  debug           Build with Debug configuration"
	@echo "  release         Build with Release configuration"
	@echo "  test            Run all tests"
	@echo "  ut       	     Run unit tests only"
	@echo "  uat Run acceptance tests only"
	@echo "  clean           Remove build directory"
	@echo "  rebuild         Clean and rebuild"
	@echo "  format          Format source code with clang-format"
	@echo "  format-check    Check code formatting"
	@echo "  lint            Run static analysis (cppcheck)"
	@echo "  check           Run all checks (format + lint)"
	@echo "  install         Install the project"
	@echo "  run-server      Run the GMRedis server"
	@echo "  run-client      Run the GMRedis CLI client"
	@echo "  help            Show this help message"
	@echo ""
	@echo "Examples:"
	@echo "  make setup              # First-time setup"
	@echo "  make                    # Build with Release (default)"
	@echo "  make BUILD_TYPE=Debug   # Build with Debug configuration"
	@echo "  make test               # Run all tests"
