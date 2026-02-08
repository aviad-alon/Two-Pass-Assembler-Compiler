#include <stdlib.h>
#include <string.h>
#include <ctype.h>
#include "error_handler.h"


/**
 * Checks if a symbol (label) exists in the symbol table.
 *
 * @param symbol_name A string representing the symbol name to search for
 * @param head_of_symbol_table Pointer to the head of the symbol table
 * @return int TRUE if the symbol exists, FALSE otherwise
 */
static int is_label_in_table(char *symbol_name, SymbolNode *head_of_symbol_table) {
    SymbolNode *current = head_of_symbol_table;

    /* Traverse through the symbol table */
    while (current != NULL) {

        /* If the symbol name was found */
        if (strcmp(symbol_name, current->symbol_name) == 0) {
            return TRUE;
        }

        /* Move to the next node in the list */
        current = current->next;
    }
    return FALSE;
}

ErrorCode check_line_errors_second_pass(char *line, SymbolNode *head_of_symbol_table) {
    char *input = malloc(strlen(line) + 1);
    char *command = malloc(strlen(line) + 1);
    char *original_input;
    char *original_command;
    int i = 0, j = 0;

    if(check_memory_allocation(input) == FALSE || check_memory_allocation(command) == FALSE) {
        exit(1);
    }

    /* Stores the original address of input and command to ensure proper memory deallocation */
    original_input = input;
    original_command = command;

    strcpy(input, line);

    /* Skip leading spaces in input */
    while (isspace(input[i])) i++;
    input = input + i;
    i = 0;

    /* Check if the line starts with a label (symbol), and skip it */
    if (is_symbol(line) == TRUE) {
        while (input[i] != ':') i++; /* Skip the symbol name */
        input = input + i + 1; /* +1 for ':' */
        i = 0;
    }

    /* Skip leading spaces in input */
    while (isspace(input[i])) i++;
    input = input + i;
    i = 0;

    /* Extract the command name from input */
    while (!isspace(input[i]) && input[i] != '\0' && input[i] != ',') {
        command[j++] = input[i++];
    }
    command[j] = '\0';
    input = input + i;
    i = 0;
    j = 0;

    /* Check if the command is a directive */
    if(is_directive(command) == TRUE) {
        /* Handle entry directive (only checks if the symbol exists, syntax was verified in first pass) */
        if(strcmp(command, ".entry") == 0) {
            char symbol_name[MAX_LINE] = {0};
            int j=0;

            /* Skip spaces after the ".entry" command */
            while (isspace(input[i])) i++;
            input = input + i;
            i=0;

            /* Extract the symbol name from input */
            while (!isspace(input[i]) && input[i] != ',' && input[i] != '\0') {
                symbol_name[j++] = input[i++];
            }
            symbol_name[j] = '\0';

            /* Check if the symbol exists in the symbol table */
            if(is_label_in_table(symbol_name, head_of_symbol_table) == FALSE) {
                free(original_input);
                free(original_command);
                return ERR_SYMBOL_NOT_FOUND;
            }

            free(original_input);
            free(original_command);
            return NO_ERROR;
        }

    } else { /* is a command line */
        char first_operand[MAX_LINE] = {0};
        char second_operand[MAX_LINE] = {0};

        /* Check if the command has one or two operands */
        if (number_of_operands(command) == 1 || number_of_operands(command) == 2) {
            /* Skip spaces after the command */
            while (isspace(input[i])) i++;
            input = input + i;
            i = 0;

            /* Extract the first operand */
            while (!isspace(input[i]) && input[i] != ',' && input[i] != '\0') {
                first_operand[j++] = input[i++];
            }
            first_operand[j] = '\0';
            input = input + i;
            i = 0;

            /* Check if the first operand uses direct addressing */
            if (get_operand_addressing_mode(first_operand) == DIRECT_ADDRESSING) {
                /* Check if the symbol exists in the symbol table */
                if(is_label_in_table(first_operand, head_of_symbol_table) == FALSE) {
                    free(original_input);
                    free(original_command);
                    return ERR_SYMBOL_NOT_FOUND;
                }

            }
            /* Check if the first operand uses relative addressing */
            else if (get_operand_addressing_mode(first_operand) == RELATIVE_ADDRESSING) {
                /* Skip the '&' character and check if the symbol exists in the symbol table */
                if(is_label_in_table(first_operand + 1, head_of_symbol_table) == FALSE) {
                    free(original_input);
                    free(original_command);
                    return ERR_SYMBOL_NOT_FOUND;
                }
            }
            /* Check if the command has two operands */
            if(number_of_operands(command) == 2) {
                /* Skip spaces before the comma */
                while (isspace(input[i])) i++;
                input = input + i;
                i = 0;
                j = 0;

                /* Pass ',' */
                input = input + 1;

                /* Skip spaces after the comma */
                while (isspace(input[i])) i++;
                input = input + i;
                i = 0;

                /* Extract the second operand */
                while ((!isspace(input[i]) && input[i] != ',' && input[i] != '\0') || input[i] == '.') {
                    second_operand[j++] = input[i++];
                }
                second_operand[j] = '\0';

                /* Check if the second operand uses direct addressing */
                if (get_operand_addressing_mode(second_operand) == DIRECT_ADDRESSING) {
                    /* Check if the symbol exists in the symbol table */
                    if(is_label_in_table(second_operand, head_of_symbol_table) == FALSE) {
                        free(original_input);
                        free(original_command);
                        return ERR_SYMBOL_NOT_FOUND;
                    }

                }
                /* Check if the second operand uses relative addressing */
                else if (get_operand_addressing_mode(second_operand) == RELATIVE_ADDRESSING) {
                    /* Skip the '&' character and check if the symbol exists in the symbol table */
                    if(is_label_in_table(second_operand + 1, head_of_symbol_table) == FALSE) {
                        free(original_input);
                        free(original_command);
                        return ERR_SYMBOL_NOT_FOUND;
                    }
                }
            }
        }
    }
    free(original_input);
    free(original_command);
    return NO_ERROR;
}
