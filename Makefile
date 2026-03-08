PROJECT_NAME ?= gpu-display
CMAKE ?= cmake
CLANG_FORMAT ?= clang-format

DEBUG_DIR := build/debug
RELEASE_DIR := build/release

.PHONY: all cmake build run debug release cmake-debug build-debug run-debug cmake-release build-release run-release format clean clean-debug clean-release

all: build-debug

cmake: cmake-debug

build: build-debug

run: run-debug

debug: build-debug

release: build-release

cmake-debug:
	$(CMAKE) -S . -B $(DEBUG_DIR) -DCMAKE_BUILD_TYPE=Debug

build-debug: cmake-debug
	$(CMAKE) --build $(DEBUG_DIR)

run-debug: build-debug
	./$(DEBUG_DIR)/$(PROJECT_NAME)

cmake-release:
	$(CMAKE) -S . -B $(RELEASE_DIR) -DCMAKE_BUILD_TYPE=Release

build-release: cmake-release
	$(CMAKE) --build $(RELEASE_DIR)

run-release: build-release
	./$(RELEASE_DIR)/$(PROJECT_NAME)

format:
	@files=$$(find src include tests -type f \( -name "*.c" -o -name "*.cc" -o -name "*.cpp" -o -name "*.cxx" -o -name "*.h" -o -name "*.hh" -o -name "*.hpp" -o -name "*.hxx" \) 2>/dev/null); \
	if [ -z "$$files" ]; then \
		echo "No C/C++ files found in src, include, tests."; \
	else \
		$(CLANG_FORMAT) -i $$files; \
		echo "Formatted C/C++ files with clang-format."; \
	fi

clean: clean-debug clean-release

clean-debug:
	rm -rf $(DEBUG_DIR)

clean-release:
	rm -rf $(RELEASE_DIR)
