#ifndef FIRST_SECOND_PASS_H
#define FIRST_SECOND_PASS_H
#include "macro_data.h"
#include "first_second_pass_data.h"
#include "error_handler.h"

/**
 * First pass of the assembler's two-pass process.
 * The first pass processes the assembly code line by line, builds the symbol table,
 * and calculates the instruction counter (IC) and data counter (DC).
 * It handles directives (.data, .string, .extern) and identifies .entry directives
 * (without fully processing them). The function processes instruction commands,
 * identifies symbols (labels), and creates the initial machine code.
 *
 * @param fd File pointer to the assembly source code
 * @return FirstPassResult structure containing:
 *         - head_of_lines_list: The list of all lines processed
 *         - head_of_symbol_table: The symbol table with all symbols
 *         - ICF: Final Instruction Counter value after the first pass
 *         - DCF: Final Data Counter value after the first pass
 */
FirstPassResult first_pass(FILE *fd);

/**
 * Checks for errors in each line of an assembly file during the first pass.
 *
 * @param fd Pointer to the file for reading
 * @param head_of_macro_table Pointer to the head of the macro table
 * @return int ERROR_FOUND if an error found, ERROR_WAS_NOT_FOUND if no error is found.
 */
int check_errors_in_first_pass(FILE *fd, MacroNode * head_of_macro_table);

/**
 * Checks if the line contains a symbol (label).
 *
 * @param line Pointer to the string representing the line to check
 * @return int TRUE if the line contains a symbol, False otherwise
 **/
int is_symbol (char *line);

/**
 * Performs the second pass of a two-pass assembler.
 * During this pass, the assembler updates the machine code of operands that use symbols, according to the symbol table.
 * It also adds the `entry` attribute to symbols in the symbol table when they are used as operands for the entry directive.
 * By the end of the second pass, the program will be completely translated to machine code.
 *
 * @param head_of_symbol_table Pointer to the head of the symbol table
 * @param head_of_line_list Double pointer to the head of the assembly line list
 * @return  SecondPassResult structure containing:
 *         - head_of_lines_list: The list of all lines processed.
 *         - head_of_symbol_table: The symbol table with all symbols.
 */
SecondPassResult second_pass(SymbolNode *head_of_symbol_table, AssemblyLineList **head_of_line_list);

/**
 * Checks for errors in each line of an assembly file during the second pass.
 *
 * @param fd Pointer to the file for reading
 * @param head_of_symbol_table Pointer to the head of the symbol table
 * @return int ERROR_FOUND if errors are found, NO_ERROR if no errors are found.
 */
int check_errors_in_second_pass(FILE *fd, SymbolNode * head_of_symbol_table);

/**
* Replaces the first incomplete word in the binary machine code list with a given word.
*
* @param head_of_code_list Pointer to the head of the binary machine code list
* @param word A pointer to the string representing the word to replace the placeholder (?) with
* @return void
*/
void add_missing_word(BinaryMachineCode *head_of_code_list, char *word);

/**
 * Checks if a given symbol is marked as external in the symbol table.
 *
 * @param symbol A pointer to the string representing the symbol name to check
 * @param head_of_symbol_table Pointer to the head of the symbol table
 * @return int TRUE if the symbol is found and marked as `EXTERN`, FALSE otherwise
 */
int is_external (char *symbol, SymbolNode *head_of_symbol_table);

/**
 * Calculates the relative address of a symbol in relation to the address of the current line.
 *
 * @param symbol A pointer to the string representing the symbol whose relative address is to be calculated
 * @param address_of_line The address of the current line in the code
 * @param head_of_symbol_table A pointer to the head of the symbol table
 * @return int The relative address of the symbol
 */
int get_relative_address(char *symbol, int address_of_line, SymbolNode *head_of_symbol_table);

/**
 * Searches for a symbol in the symbol table and returns its associated address.
 *
 * @param symbol A pointer to the string representing the symbol whose address is being searched
 * @param head_of_symbol_table A pointer to the head of the symbol table
 * @return int The address associated with the symbol
 */
int get_symbol_address(char *symbol, SymbolNode *head_of_symbol_table);

#endif