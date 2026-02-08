#ifndef ERROR_HANDLER_H
#define ERROR_HANDLER_H


#include "macro_data.h"
#include "first_second_pass_data.h"
#include "build_binary_word.h"
#include "first_second_pass.h"

#define MAX_SYMBOL_NAME 31 /* Maximum symbol name length */

/**
 * Enum representing various error codes.
 */
typedef enum {
    ERR_UNDEFINED_COMMAND,
    ERR_INVALID_PARA,
    ERR_INVALID_REG,
    ERR_MISSING_PARA,
    ERR_MISSING_COMMA,
    ERR_CONS_COMMAS,
    ERR_EXTRANEOUS_TEXT,
    ERR_ILLEGAL_COMMA,
    ERR_SRC_ADDRESSING,
    ERR_DEST_ADDRESSING,
    ERR_INVALID_LABEL_NAME,
    ERR_LABEL_MACRO,
    ERR_MISSING_QUOTATION,
    ERR_SYMBOL_NOT_FOUND,
    ERR_LINE_TOO_LONG,
    ERR_SYMBOL_TOO_LONG,
    ERR_INVALID_SYMBOL_CHAR,
    ERR_INVALID_SYMBOL_START,
    ERR_EMPTY_LABEL_LINE,
    NO_ERROR
} ErrorCode;

/**
 * Validates assembly code lines during the first pass of the assembler.
 * Performs syntax and semantic validation on assembly code lines, checking command
 * structure, operands, addressing modes, label definitions, and directive formatting
 * to ensure they conform to the assembly language specifications.
 *
 * @param line A pointer to the line string to be checked
 * @param head_of_macro_table Pointer to the head of the macro table
 * @return ErrorCode corresponding to the validation result:
 *         - ERR_UNDEFINED_COMMAND: If the command is not defined.
 *         - ERR_INVALID_PARA: If a parameter is invalid (e.g., not a valid number).
 *         - ERR_INVALID_REG: If an invalid register name is used.
 *         - ERR_MISSING_PARA: If a required parameter is missing.
 *         - ERR_MISSING_COMMA: If a comma is missing where expected.
 *         - ERR_CONS_COMMAS: If there are multiple consecutive commas.
 *         - ERR_ILLEGAL_COMMA: If a comma appears in an illegal position.
 *         - ERR_EXTRANEOUS_TEXT: If there is extra text after end of command.
 *         - ERR_SRC_ADDRESSING: If an illegal addressing mode is used for a source operand.
 *         - ERR_DEST_ADDRESSING: If an illegal addressing mode is used for a destination operand.
 *         - ERR_INVALID_LABEL_NAME: If a label name is a reserved word (e.g., command, register, or directive name).
 *         - ERR_LABEL_MACRO: If a label name matches an existing macro name.
 *         - ERR_MISSING_QUOTATION: If a string directive is missing opening or closing quotation marks.
 *         - ERR_SYMBOL_NOT_FOUND: If a symbol is not found in the symbol table.
 *         - ERR_LINE_TOO_LONG: If the length of a line exceeds 80 characters.
 *         - ERR_SYMBOL_TOO_LONG: If the length of a symbol name exceeds 31 characters.
 *         - ERR_INVALID_SYMBOL_CHAR: If a symbol name contains an invalid character.
 *         - ERR_INVALID_SYMBOL_START: If symbol name not start with a letter.
 *         - ERR_EMPTY_LABEL_LINE: If after a label name there is an empty line.
 *         - NO_ERROR: If no errors were found and the line is valid.
 */
ErrorCode check_line_errors_first_pass(char *line);

/**
 * Checks if a symbol name in a given line is already a macro name in the macro table.
 *
 * @param line Pointer to the string representing the line to check
 * @param head_of_macro_table A pointer to the head of the macro linked list
 * @return ErrorCode - ERR_LABEL_MACRO if the label is a macro name, otherwise NO_ERROR
 */
ErrorCode is_label_macro(char *line, MacroNode *head_of_macro_table);

/**
 * Checks if a given operand is a register name (r0 to r7).
 *
 * @param operand A string representing the operand to check
 * @return TRUE if the operand is a register (r0 to r7), FALSE otherwise.
 */
int is_register(char* operand);

/**
 * Checks if a given string is a directive.
 *
 * @param command Pointer to the string representing the command to check
 * @return int TRUE if the string matches a known directive, otherwise FALSE.
 */
int is_directive(char *command);

/**
 * Prints an error message based on the given ErrorCode.
 * Each error code represents a specific type of error.
 *
 * @param code The error code of type `ErrorCode` that specifies the error to be printed.
 * @return void
 */
void print_error(ErrorCode code, int num_of_line);

/**
 * This function Performs error validation checks on code lines during the second pass of the assembler.
 * 1. Entry directive operands - verifies symbols referenced in .entry directives exist in the symbol table,
 *    also validate that the format of the line is correct.
 * 2. Operand symbols - confirms all labels used as operands exist in the symbol table
 *
 * @param line A string representing the line to be checked
 * @param head_of_symbol_table Pointer to the head of the symbol table
 * @return ErrorCode corresponding to the validation result:
 *         - ERR_ILLEGAL_COMMA: If a comma appears in an illegal position.
 *         - ERR_SYMBOL_NOT_FOUND: If a used symbol does not exist in the symbol table.
 *         - ERR_EXTRANEOUS_TEXT: If there is extra text after end of command.
 *         - NO_ERROR: If no errors were found and the line is valid.
 */
ErrorCode check_line_errors_second_pass(char *line, SymbolNode *head_of_symbol_table);

#endif
