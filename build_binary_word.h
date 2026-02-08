#ifndef BUILD_BINARY_WORD_H
#define BUILD_BINARY_WORD_H

#include "first_second_pass_data.h"

#define WORD_SIZE 24
#define BINARY_BITS 24

/**
 * Represents a command with its name and his binary code and binary funct code.
 */
typedef struct Command {
    char *command_name;
    char* command_binary_code;
    char* funct_binary_code;
} Command;

/**
 * Represents an addressing mode with its addressing code and binary code.
 */
typedef struct AddressingMode {
    int addresing_code;
    char* addresing_binary_code;
} AddressingMode;

/**
 * Represents a register with its name and his binary code.
 */
typedef struct RegisterNumber {
    char *register_name;
    char *register_binary_code;
} RegisterNumber;


/**
 * Determines the addressing mode of a given operand.
 *
 * @param operand A string representing the operand whose addressing mode is to be determined.
 * @return The addressing mode of the operand as an `AddressingCase` enum value:
 *         - `IMMEDIATE_ADDRESSING`
 *         - `RELATIVE_ADDRESSING`
 *         - `REGISTER_DIRECT_ADDRESSING`
 *         - `DIRECT_ADDRESSING`
 */
AddressingCase get_operand_addressing_mode(char* operand);

/**
 * Determines the number of operands for a given command.
 *
 * @param command Pointer to the string representing the command.
 * @return The number of operands:
 *         - 2 for commands that require two operands.
 *         - 1 for commands that require one operand.
 *         - 0 for commands that require no operands.
 */
int number_of_operands (char *command);

/**
 * Calculates the total number of machine words needed for an instruction,
 * This depends on the command type and addressing modes of operands.
 *
 * @param line The remaining line after the command
 * @param command The instruction command name
 * @return Total number of machine words needed for this instruction
 */
int number_of_words(char* line, char* command);

/**
 * Converts a decimal number to its binary representation.
 *
 * @param array The character array where the binary representation will be stored
 * @param number The decimal number to convert to binary
 * @param number_of_bits The number of bits to use in the binary representation
 * @return void
 */
void to_binary(char* array, int number, int number_of_bits);

/**
 * Creates the binary representation of the first word of an assembly instruction.
 *
 * @param line Pointer to the string representing the assembly instruction line to be processed
 * @param is_symbol Flag indicating if the line contains a symbol (label)
 * @return Pointer to a string containing the binary representation of the first word of the assembly instruction
 */
char* create_first_word(char* line, int is_symbol);

/**
 * Creates a binary representation of an extra word for assembly elements (instructions, data values, and strings)
 *
 * @param operator The operand string to be converted to binary representation
 * @param type The type of assembly element (STRING, DATA, or INSTRUCTION)
 * @return A string containing the binary representation of the extra word, or "?" for words to be resolved later
 */
char* create_extra_word(char* operator, AssemblyElementType type);

/**
 * Builds a missing binary word for the second pass of the assembler.
 *
 * @param address The address value to encode
 * @param addressing_mode The addressing mode (DIRECT or RELATIVE)
 * @param external_flag Indicates if the symbol is external (TRUE or FALSE)
 * @return A string representing the binary word of the missing word
 */
char* build_word_second_pass(int address, AddressingCase addressing_mode, int external_flag);

/**
 * Creates additional words during the second pass for operands that reference symbols
 *
 * @line Pointer to the current assembly line list node
 * @head_of_symbol_table Pointer to the head of the symbol table
 * @return void
 */
void create_missing_word_second_pass(AssemblyLineList *line, SymbolNode *head_of_symbol_table);

#endif
