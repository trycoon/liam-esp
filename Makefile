SHELL := /usr/bin/env bash
.DEFAULT_GOAL := help

WEB_DIR := web
SRC_DIR := src

PLATFORMIO_BIN ?= platformio
CPPCHECK_BIN ?= cppcheck
CLANG_FORMAT_BIN ?= $(shell command -v clang-format 2>/dev/null || command -v clang-format-18 2>/dev/null || command -v clang-format-17 2>/dev/null || echo clang-format)
CLANG_TIDY_BIN ?= $(shell command -v clang-tidy 2>/dev/null || command -v clang-tidy-18 2>/dev/null || command -v clang-tidy-17 2>/dev/null || echo clang-tidy)

CPP_FILES := $(shell find $(SRC_DIR) -type f \( -name "*.h" -o -name "*.cpp" \))
CPP_IMPL_FILES := $(shell find $(SRC_DIR) -type f -name "*.cpp")

.PHONY: help prepush ci firmware-ci firmware-build firmware-compiledb cppcheck clang-format-check clang-format-fix clang-tidy web-ci web-install web-lint web-build clean

help:
	@echo "Available targets:"
	@echo "  make prepush            - Run full local pre-push flow (firmware + web)"
	@echo "  make ci                 - Alias of prepush"
	@echo "  make firmware-ci        - Firmware checks from GitHub Actions"
	@echo "  make firmware-build     - Build ESP32 firmware"
	@echo "  make firmware-compiledb - Generate compile_commands.json for clang-tidy"
	@echo "  make cppcheck           - Run cppcheck with embedded-friendly settings"
	@echo "  make clang-format-check - Check C/C++ formatting"
	@echo "  make clang-format-fix   - Auto-format C/C++ source files"
	@echo "  make clang-tidy         - Run clang-tidy over all .cpp files"
	@echo "  make web-ci             - Web lint + build:ci"
	@echo "  make web-install        - npm ci in web/"
	@echo "  make web-lint           - npm run lint in web/"
	@echo "  make web-build          - npm run build:ci in web/"

prepush: firmware-ci web-ci

ci: prepush

firmware-ci: firmware-build firmware-compiledb cppcheck clang-format-check clang-tidy

firmware-build:
	$(PLATFORMIO_BIN) run

firmware-compiledb:
	$(PLATFORMIO_BIN) run -t compiledb

cppcheck:
	@if ! command -v $(CPPCHECK_BIN) >/dev/null 2>&1; then \
		echo "Error: cppcheck not found. Install with: sudo apt install cppcheck"; \
		exit 1; \
	fi
	$(CPPCHECK_BIN) --std=c++11 --language=c++ --enable=warning,performance,portability \
		--suppress=missingInclude --suppress=missingIncludeSystem \
		-I$(SRC_DIR) --quiet --error-exitcode=1 $(SRC_DIR)/

clang-format-check:
	@if ! command -v $(CLANG_FORMAT_BIN) >/dev/null 2>&1; then \
		echo "Error: clang-format not found. Install with: sudo apt install clang-format"; \
		exit 1; \
	fi
	find $(SRC_DIR) -type f \( -name "*.h" -o -name "*.cpp" \) -print0 \
		| xargs -0 $(CLANG_FORMAT_BIN) --dry-run --Werror -style=file

clang-format-fix:
	@if ! command -v $(CLANG_FORMAT_BIN) >/dev/null 2>&1; then \
		echo "Error: clang-format not found. Install with: sudo apt install clang-format"; \
		exit 1; \
	fi
	find $(SRC_DIR) -type f \( -name "*.h" -o -name "*.cpp" \) -print0 \
		| xargs -0 $(CLANG_FORMAT_BIN) -i -style=file

clang-tidy: firmware-compiledb
	@if ! command -v $(CLANG_TIDY_BIN) >/dev/null 2>&1; then \
		echo "Error: clang-tidy not found. Install with: sudo apt install clang-tidy"; \
		exit 1; \
	fi
	find $(SRC_DIR) -type f -name "*.cpp" -print0 \
		| xargs -0 -n 1 $(CLANG_TIDY_BIN) -p . --quiet

web-ci: web-lint web-build

web-install:
	cd $(WEB_DIR) && npm ci

web-lint:
	cd $(WEB_DIR) && npm run lint

web-build:
	cd $(WEB_DIR) && npm run build:ci

clean:
	$(PLATFORMIO_BIN) run -t clean
