# Lexer LDP

A basic lexical analyzer (lexer) project implemented in C. This project provides a foundation for building lexical analyzers that can tokenize input text according to defined grammar rules.

## Project Structure

```
lexer-ldp/
├── src/
│   ├── main.c      # Main program entry point
│   ├── lexer.h     # Header file with type definitions and function declarations
│   └── lexer.c     # Lexer implementation
├── Makefile        # Build configuration
├── README.md       # This file
└── .gitignore      # Git ignore rules
```

## Features

- Basic token types (identifiers, numbers, strings, operators, keywords)
- Extensible token structure with line and column information
- Memory management for tokens
- Simple command-line interface

## Building

To build the project, use the provided Makefile:

```bash
# Build the release version
make all

# Build the debug version
make debug

# Clean build artifacts
make clean
```

## Usage

After building, run the lexer with an input file:

```bash
./bin/lexer-ldp <input_file>
```

## Development

### Available Make targets:

- `make all` - Build the project (default)
- `make debug` - Build debug version with debugging symbols
- `make clean` - Remove build artifacts
- `make run` - Build and run the program
- `make test` - Build and run with test input
- `make help` - Show available targets

### Requirements

- GCC compiler
- Make build system
- Standard C library

## Contributing

This is a basic project structure that can be extended with:
- More sophisticated token recognition
- Error handling and reporting
- Configuration files for grammar rules
- Unit tests
- Documentation

## License

This project is open source. Please check the repository for license information.