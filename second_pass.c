#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include "first_second_pass.h"
#include "first_second_pass_data.h"
#include "error_handler.h"


/**
 * Marks a symbol as an entry in the symbol table by setting its entry flag.
 * If the symbol is not found, no changes are made.
 *
 * @param head_of_symbol_table A pointer to the head of the symbol table
 * @param symbol A pointer to the string representing the symbol to be marked as an entry
 * @return void
 */
static void mark_as_entry(SymbolNode *head_of_symbol_table, char *symbol) {
    SymbolNode *current_line = head_of_symbol_table;

    /* Traverse through the symbol table linked list */
    while (current_line != NULL) {

        /* Check if the current symbol matches the given symbol name */
        if(strcmp(current_line->symbol_name, symbol) == 0) {

            /* Mark the symbol as an entry */
            current_line->entry_flag = 1;
            return;
        }
        /* Move to the next node in the linked list */
        current_line = current_line->next;
    }
    return;
}

/**
 * Checks if there are incomplete binary words (marked as "?" in the first pass) in the binary machine code list.
 *
 * @param head_of_code_list A pointer to the head of the binary machine code linked list
 * @return int TRUE if there is incomplete word, FALSE otherwise
 */
static int needs_extra(BinaryMachineCode *head_of_code_list) {
    BinaryMachineCode *current_code = head_of_code_list;

    /* Traverse through the list of binary machine code */
    while (current_code != NULL) {

        /* Checks if the current word in the binary machine code list is marked as incomplete ("?") */
        if (strcmp(current_code->word, "?") == 0) {
            return TRUE;
        }

        /* Moves to the next node in the linked list */
        current_code = current_code->next;
    }
    return FALSE;
}

int get_symbol_address(char *symbol, SymbolNode *head_of_symbol_table) {
    SymbolNode *current_symbol = head_of_symbol_table;

    /* Traverse through the symbol table linked list */
    while (current_symbol != NULL) {

        /* Checks if the current symbol matches the given symbol name */
        if (strcmp(current_symbol->symbol_name, symbol) == 0) {

            /* Returns the symbol address */
            return current_symbol->address;
        }

        /* Moves to the next node in the linked list */
        current_symbol = current_symbol->next;
    }

    return 0;

}

int get_relative_address(char *symbol, int address_of_line, SymbolNode *head_of_symbol_table) {
    return get_symbol_address(symbol, head_of_symbol_table) - address_of_line;
}

int is_external (char *symbol, SymbolNode *head_of_symbol_table) {
    SymbolNode *current_symbol = head_of_symbol_table;

    /* Traverse through the symbol table linked list */
    while (current_symbol != NULL) {

        /* Check if the current symbol matches the given symbol name */
        if (strcmp(current_symbol->symbol_name, symbol) == 0) {

            /* Check if the symbol is of type EXTERN */
            if(current_symbol->type == EXTERN) {
                return TRUE;
            }
        }

        /* Move to the next node in the linked list */
        current_symbol = current_symbol->next;
    }
    return FALSE;
}

void add_missing_word(BinaryMachineCode *head_of_code_list, char *word) {
    BinaryMachineCode *current_code = head_of_code_list;

    /* Traverse through the list of binary machine code */
    while (current_code != NULL) {

        /* Checks if the current word in the binary machine code list is marked as incomplete ("?") */
        if (strcmp(current_code->word, "?") == 0) {
            free(current_code->word);
            /* replace the placeholder with the given word */
            current_code->word = word;
            return;
        }

        /* Moves to the next node in the linked list */
        current_code = current_code->next;
    }
}

int check_errors_in_second_pass(FILE *fd, SymbolNode * head_of_symbol_table) {
    char line[MAX_LINE] = {0};
    int line_number = 1;
    int error_flag = ERROR_WAS_NOT_FOUND;

    /* Read line by line from the file until the end */
    while (fgets(line, sizeof(line), fd)) {
        ErrorCode error_check = NO_ERROR;
        error_check = check_line_errors_second_pass(line, head_of_symbol_table);

        /* If an error is found, print it and update the error flag */
        if (error_check != NO_ERROR) {
            print_error(error_check, line_number);
            error_flag = ERROR_FOUND;
        }
        line_number++;
    }
    return error_flag;
}

SecondPassResult second_pass(SymbolNode *head_of_symbol_table, AssemblyLineList **head_of_line_list) {
    SecondPassResult result;
    AssemblyLineList *current_line_list = *head_of_line_list;

    /* Traverse through the assembly line list */
    while (current_line_list != NULL) {
        int next_flag = 0;
        char *current_line = current_line_list->line;
        char *command;

        char line_copy[MAX_LINE] = {0};

        strcpy(line_copy, current_line);

        /* Check if line starts with a symbol (label) */
        if (is_symbol(current_line) == TRUE) {

            /* Extract the command after the label */
            strtok(line_copy, ":");
            command = strtok(NULL, " \n\t");
        } else {

            /* Extract the command from the beginning of the line */
            command = strtok(line_copy, " \n\t");

        }

        /* Process only if not a data, string, or extern directive */
        if (!(strcmp(command, ".data") == 0 || strcmp(command, ".string") == 0 || strcmp(command, ".extern") == 0)) {

            /* Handle entry directive */
            if (strcmp(command, ".entry") == 0) {
                AssemblyLineList *temp = current_line_list;
                char *symbol;

                /* Extract the symbol name */
                symbol = strtok(NULL, " \n\t");

                /* Mark the symbol as an entry in the symbol table */
                mark_as_entry(head_of_symbol_table, symbol);

                next_flag = 1;
                current_line_list = current_line_list->next;


                /* Remove the entry line from the linked list */
                if (temp->prev != NULL) {
                    temp->prev->next = temp->next;
                }
                if (temp->next != NULL) {
                    temp->next->prev = temp->prev;
                }

                /* Update head of the list if needed */
                if (temp == *head_of_line_list) {
                    *head_of_line_list = temp->next;
                }

                free(temp->code->word);
                free(temp->code);
                free(temp->line);
                free(temp);

                /* Handle command line */
            } else {

                /* Check if extra word is needed */
                if(needs_extra(current_line_list->code) == TRUE) {
                    /* Create the missing word */
                    create_missing_word_second_pass(current_line_list, head_of_symbol_table);
                }
            }
        }

        if(next_flag == 0) {
            /* Move to the next node in the line list */
            current_line_list = current_line_list->next;
        }
    }
    result.head_of_lines_list = *head_of_line_list;
    result.head_of_symbol_table = head_of_symbol_table;

    /*print_assembly_lines(result.head_of_lines_list); */
    return result;
}
