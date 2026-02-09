# Two-Pass-Assembler-Compiler

A comprehensive assembler compiler written in C that translates Assembly language into machine code. This project demonstrates advanced compiler design principles including lexical analysis, syntax validation, and symbol table management.

## Overview

This two-pass assembler processes Assembly source code and generates machine code output, along with supplementary files for linking and debugging. It implements a complete pipeline from tokenization to code generation with robust error handling and validation.

## Features

- **Lexical Analysis**: Tokenizes and parses Assembly instructions and directives
- **Syntax Validation**: Comprehensive checking of instruction syntax and operand validity
- **Macro Expansion**: Preprocesses and expands macro definitions
- **Symbol Management**: Maintains symbol tables for entry points and external symbols
- **Error Handling**: Detailed error reporting with line numbers and context
- **Multi-Pass Processing**: Two-pass compilation for forward reference resolution
- **File Output**: Generates object files, entry symbol files, and external symbol files

## Technical Implementation

- **Language**: C
- **Memory Management**: 
  - Dynamic memory allocation and deallocation
  - Careful pointer manipulation and null-termination handling
  - Prevention of memory leaks through proper resource cleanup
- **Custom Data Structures**:
  - Hand-built dynamic data structures (linked lists, hash tables, symbol tables)
  - Dynamic array implementations for flexible sizing
  - Efficient memory utilization patterns
- **Advanced Concepts**:
  - Compiler design patterns
  - Low-level data structure manipulation
  - File I/O operations and format specifications

## Project Structure
