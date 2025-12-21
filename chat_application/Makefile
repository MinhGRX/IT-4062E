# Compiler and flags
CC = gcc
CFLAGS = -Wall -Wextra -pthread -g
LDFLAGS = -pthread

# Directories
CLIENT_DIR = client
SERVER_DIR = server

# Source files
CLIENT_SRC = $(CLIENT_DIR)/client.c
SERVER_SRC = $(SERVER_DIR)/server.c

# Output binaries
CLIENT_BIN = $(CLIENT_DIR)/client
SERVER_BIN = $(SERVER_DIR)/server

# Default target: build both client and server
.PHONY: all
all: client server

# Build client
.PHONY: client
client: $(CLIENT_BIN)

$(CLIENT_BIN): $(CLIENT_SRC)
	@echo "Building client..."
	$(CC) $(CFLAGS) -o $(CLIENT_BIN) $(CLIENT_SRC) $(LDFLAGS)
	@echo "Client built successfully: $(CLIENT_BIN)"

# Build server
.PHONY: server
server: $(SERVER_BIN)

$(SERVER_BIN): $(SERVER_SRC)
	@echo "Building server..."
	$(CC) $(CFLAGS) -o $(SERVER_BIN) $(SERVER_SRC) $(LDFLAGS)
	@echo "Server built successfully: $(SERVER_BIN)"

# Clean build artifacts
.PHONY: clean
clean:
	@echo "Cleaning build artifacts..."
	rm -f $(CLIENT_BIN) $(SERVER_BIN)
	rm -rf $(CLIENT_DIR)/*.o $(SERVER_DIR)/*.o
	rm -rf $(CLIENT_DIR)/*.dSYM $(SERVER_DIR)/*.dSYM
	@echo "Clean complete."

# Clean data and logs (use with caution!)
.PHONY: clean-data
clean-data:
	@echo "Cleaning data and logs..."
	rm -rf data logs
	@echo "Data and logs removed."

# Full clean: artifacts + data
.PHONY: clean-all
clean-all: clean clean-data
	@echo "Full clean complete."

# Run server (convenience target)
.PHONY: run-server
run-server: server
	@echo "Starting server on localhost:8023..."
	cd $(SERVER_DIR) && ./server localhost 8023

# Run client (convenience target)
.PHONY: run-client
run-client: client
	@echo "Connecting client to localhost:8023..."
	cd $(CLIENT_DIR) && ./client localhost 8023

# Rebuild everything
.PHONY: rebuild
rebuild: clean all

# Display help
.PHONY: help
help:
	@echo "Chat Application - Makefile Help"
	@echo "================================="
	@echo ""
	@echo "Available targets:"
	@echo "  make all          - Build both client and server (default)"
	@echo "  make client       - Build client only"
	@echo "  make server       - Build server only"
	@echo "  make clean        - Remove compiled binaries"
	@echo "  make clean-data   - Remove data and log directories"
	@echo "  make clean-all    - Remove binaries, data, and logs"
	@echo "  make rebuild      - Clean and rebuild everything"
	@echo "  make run-server   - Build and run server on localhost:8023"
	@echo "  make run-client   - Build and run client connecting to localhost:8023"
	@echo "  make help         - Display this help message"
	@echo ""
	@echo "Usage examples:"
	@echo "  make              # Build everything"
	@echo "  make clean all    # Clean and rebuild"
	@echo "  make run-server   # Start server"
	@echo ""

# Check code (optional: requires cppcheck)
.PHONY: check
check:
	@echo "Running static analysis..."
	@which cppcheck > /dev/null 2>&1 && \
		cppcheck --enable=all --suppress=missingIncludeSystem $(CLIENT_SRC) $(SERVER_SRC) || \
		echo "cppcheck not installed, skipping static analysis"

# Format code (optional: requires clang-format)
.PHONY: format
format:
	@echo "Formatting code..."
	@which clang-format > /dev/null 2>&1 && \
		clang-format -i $(CLIENT_SRC) $(SERVER_SRC) || \
		echo "clang-format not installed, skipping formatting"

# Show compiler and system info
.PHONY: info
info:
	@echo "Build Information"
	@echo "================="
	@echo "Compiler: $(CC)"
	@echo "Compiler version:"
	@$(CC) --version | head -n 1
	@echo "Compile flags: $(CFLAGS)"
	@echo "Linker flags: $(LDFLAGS)"
	@echo "System: $$(uname -s) $$(uname -m)"
	@echo ""
