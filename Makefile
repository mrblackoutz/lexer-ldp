# Makefile for Lexer LDP project

# Compiler and flags
CC = gcc
CFLAGS = -Wall -Wextra -std=c99 -O2
DEBUGFLAGS = -g -DDEBUG

# Directories
SRCDIR = src
OBJDIR = obj
BINDIR = bin

# Target executable
TARGET = $(BINDIR)/lexer-ldp
DEBUG_TARGET = $(BINDIR)/lexer-ldp-debug

# Source files
SOURCES = $(wildcard $(SRCDIR)/*.c)
OBJECTS = $(SOURCES:$(SRCDIR)/%.c=$(OBJDIR)/%.o)
DEBUG_OBJECTS = $(SOURCES:$(SRCDIR)/%.c=$(OBJDIR)/%-debug.o)

# Default target
all: $(TARGET)

# Debug target
debug: $(DEBUG_TARGET)

# Create directories if they don't exist
$(OBJDIR):
	mkdir -p $(OBJDIR)

$(BINDIR):
	mkdir -p $(BINDIR)

# Compile source files to object files
$(OBJDIR)/%.o: $(SRCDIR)/%.c | $(OBJDIR)
	$(CC) $(CFLAGS) -c $< -o $@

$(OBJDIR)/%-debug.o: $(SRCDIR)/%.c | $(OBJDIR)
	$(CC) $(CFLAGS) $(DEBUGFLAGS) -c $< -o $@

# Link object files to create executable
$(TARGET): $(OBJECTS) | $(BINDIR)
	$(CC) $(OBJECTS) -o $@

$(DEBUG_TARGET): $(DEBUG_OBJECTS) | $(BINDIR)
	$(CC) $(DEBUG_OBJECTS) -o $@

# Clean build artifacts
clean:
	rm -rf $(OBJDIR) $(BINDIR)

# Install (copy to /usr/local/bin)
install: $(TARGET)
	cp $(TARGET) /usr/local/bin/

# Uninstall
uninstall:
	rm -f /usr/local/bin/lexer-ldp

# Run the program
run: $(TARGET)
	./$(TARGET)

# Run with a test file
test: $(TARGET)
	echo "test input" | ./$(TARGET) /dev/stdin

# Show help
help:
	@echo "Available targets:"
	@echo "  all     - Build the project (default)"
	@echo "  debug   - Build debug version"
	@echo "  clean   - Remove build artifacts"
	@echo "  run     - Build and run the program"
	@echo "  test    - Build and run with test input"
	@echo "  install - Install to /usr/local/bin"
	@echo "  uninstall - Remove from /usr/local/bin"
	@echo "  help    - Show this help message"

.PHONY: all debug clean install uninstall run test help