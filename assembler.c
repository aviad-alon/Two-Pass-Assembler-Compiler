#include <stdio.h>
#include <string.h>
#include <ctype.h>
#include <stdlib.h>
#include "first_second_pass_data.h"
#include "macro_data.h"
#include "first_second_pass.h"


/**
 * Checks if there is any symbol in the symbol table with the entry flag set to 1.
 *
 * @param head_of_symbol_table Pointer to the head of the symbol table
 * @return int TRUE if there is a symbol with the entry flag set to 1, FALSE otherwise.
 */
static int is_entry_exist(SymbolNode *head_of_symbol_table) {
    SymbolNode *current = head_of_symbol_table;

    /* Traverse the symbol table */
    while (current != NULL) {

        /* Check if the current symbol has the entry flag set to 1 */
        if(current->entry_flag == 1) {
            return TRUE;
        }

        /* Move to the next symbol in the table */
        current = current->next;
    }
    return FALSE;
}

/**
 * Counts occurrences of an external symbol in an assembly line.
 *
 * @param line A string representing the assembly code line
 * @param symbol_name A string representing the external symbol to check
 * @return int Number of times the external symbol appears (0, 1, or 2)
 */
static int count_extern_symbol_in_line(char * line, char *symbol_name) {
    int external_symbol_count = 0;

    int i = 0, j = 0;

    char *input = malloc(strlen(line) + 1);
    char *original_input;
    char first_operand[MAX_LINE] = {0};
    char second_operand[MAX_LINE] = {0};
    char command[MAX_LINE] = {0};

    if(check_memory_allocation(input) == FALSE) {
        exit(1);
    }

    /* Stores the original address of input to ensure proper memory deallocation */
    original_input = input;
    strcpy(input, line);

    /* Check if the line starts with a symbol (label) */
    if (is_symbol(input) == TRUE) {

        /* Pass the label (until ':') */
        while (input[i] != ':') i++;

        /* Pass the ':' character */
        i++;
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

    /* Skip leading spaces in input */
    while (isspace(input[i])) i++;
    input = input + i;
    i = 0;

    /* Check if it's a command line and not a directive */
    if (is_directive(command) == FALSE) {

        /* Check if there are operands */
        if (number_of_operands(command) != 0) {

            /* Extract the first operand from the input */
            while (!isspace(input[i]) && input[i] != ',' && input[i] != '\0') {
                first_operand[j++] = input[i++];
            }
            first_operand[j] = '\0';
            input = input + i;
            i = 0;
            j = 0;

            /* Check if the first operand is the external symbol */
            if (strcmp(first_operand, symbol_name) == 0) {
                external_symbol_count++;
            }

            /* If there are two operands */
            if (number_of_operands(command) == 2) {

                /* Skip spaces between operands */
                while (isspace(input[i])) i++;

                /* +1 for skip the comma between operands */
                input = input + i + 1;
                i = 0;

                /* Skip spaces before the second operand */
                while (isspace(input[i])) i++;
                input = input + i;
                i = 0;

                /* Extract the second operand from the input */
                while (!isspace(input[i]) && input[i] != ',' && input[i] != '\0') {
                    second_operand[j++] = input[i++];
                }
                second_operand[j] = '\0';

                /* Check if the second operand is the external symbol */
                if (strcmp(second_operand, symbol_name) == 0) {
                    external_symbol_count++;
                }

            }
        }
    }
    free(original_input);
    return external_symbol_count;
}

/**
 * Checks if there is any symbol in the symbol table with the type EXTERN.
 * and whether it is used in any of the assembly lines.
 *
 * @param head_of_symbol_table Pointer to the head of the symbol table
 * @param head_of_lines_list Pointer to the head of the assembly lines list
 * @return int TRUE if there is a symbol with the type EXTERN that appears in at least one assembly line,
 *             FALSE otherwise
 */
static int is_extern_exist(SymbolNode *head_of_symbol_table, AssemblyLineList *head_of_lines_list) {
    SymbolNode *current_symbol = head_of_symbol_table;

    /* Traverse the symbol table */
    while (current_symbol != NULL) {

        /* Check if the current symbol is of type EXTERN */
        if(current_symbol->type == EXTERN) {
            AssemblyLineList *current_line = head_of_lines_list;

            /* Traverse through each line in the assembly code */
            while (current_line != NULL) {
                int number_of_extern_symbol = 0;
                char * line = current_line->line;

                /* Count how many times the external symbol appears in the line, if it exists */
                number_of_extern_symbol = count_extern_symbol_in_line(line, current_symbol->symbol_name);

                /* If the external symbol is found in the line, return TRUE */
                if(number_of_extern_symbol != 0) {
                    return TRUE;
                }

                /* Move to the next assembly line */
                current_line = current_line->next;
            }
        }
        /* Move to the next symbol in the table */
        current_symbol = current_symbol->next;
    }
    return FALSE;
}

/**
 * Creates an externals file containing all external symbols names
 * and their addresses in the machine code where they are used.
 *
 * @param head_of_symbol_table Pointer to the head of the symbol table
 * @param head_of_lines_list Pointer to the head of the assembly line list
 * @param file_name The file name without any extension
 * @return void
 */
static void create_externals_file (SymbolNode *head_of_symbol_table, AssemblyLineList *head_of_lines_list, char *file_name) {

    /* Check if there are any external symbols in the symbol table */
    if(is_extern_exist(head_of_symbol_table, head_of_lines_list) == TRUE) {
        char *externals_file_name;
        FILE *externals_output_file;
        SymbolNode *curren_symbol;

        /* create the externals file name */
        externals_file_name = malloc(strlen(file_name) + 5);

        /* Check if memory allocation was successful */
        if(check_memory_allocation(externals_file_name) == FALSE) {
            exit(1);
        }
        externals_file_name[0] = '\0';

        /* Concatenate the file name with ".ext" extension */
        strcat(externals_file_name, file_name);
        strcat(externals_file_name, ".ext");

        /* Open the externals file for writing */
        externals_output_file = fopen(externals_file_name, "w");

        if (check_file_open(externals_output_file, externals_file_name) == FALSE) {
            exit(1);
        }

        /* Iterate over the symbol table to find external symbols */
        curren_symbol = head_of_symbol_table;
        while (curren_symbol != NULL) {

            /* Check if the current symbol is external */
            if(curren_symbol->type == EXTERN) {

                /* Iterate over the assembly line list to find references to the external symbol */
                AssemblyLineList *current_line = head_of_lines_list;
                while (current_line != NULL) {
                    int number_of_extern_symbol = 0;
                    char * line = current_line->line;

                    /* Count how many times the external symbol appears in the line, if it exists */
                    number_of_extern_symbol = count_extern_symbol_in_line(line, curren_symbol->symbol_name);


                    /* If the external symbol appears in the line at least once */
                    if(number_of_extern_symbol != 0) {

                        /* Write to the externals file the address where the first external symbol appeared */
                        fprintf(externals_output_file, "%s %07d\n", curren_symbol->symbol_name, current_line->code->next->address);

                        /* If the external symbol appears in the line two times */
                        if (number_of_extern_symbol == 2) {

                            /* Write to the externals file the address where the second external symbol appeared */
                            fprintf(externals_output_file, "%s %07d\n", curren_symbol->symbol_name, current_line->code->next->next->address);
                        }
                    }

                    /* Move to the next assembly line */
                    current_line = current_line->next;
                }
            }

            /* Move to the next symbol in the symbol table */
            curren_symbol = curren_symbol->next;
        }

        /* Close the externals file */
        fclose(externals_output_file);

        /* Free the memory allocated for the externals file name */
        free(externals_file_name);
    }
}

/**
 * Creates an entries file containing the names and addresses of all entry symbols.
 *
 * @param head_of_symbol_tabl Pointer to the head of the symbol table
 * @param file_name The file name without any extension
 * @return void
 */
static void create_entries_file (SymbolNode *head_of_symbol_table, char *file_name) {

    /* Check if there are any entry symbols in the symbol table */
    if(is_entry_exist(head_of_symbol_table) == TRUE) {
        char *entries_file_name;
        FILE *entries_output_file;
        SymbolNode *current;

        /* create the entries file name */
        entries_file_name = malloc(strlen(file_name) + 5);

        /* Check if memory allocation was successful */
        if(check_memory_allocation(entries_file_name) == FALSE) {
            exit(1);
        }
        entries_file_name[0] = '\0';

        /* Concatenate the file name with ".ent" extension */
        strcat(entries_file_name, file_name);
        strcat(entries_file_name, ".ent");

        /* Open the entries file for writing */
        entries_output_file = fopen(entries_file_name, "w");
        if (check_file_open(entries_output_file, entries_file_name) == FALSE) {
            exit(1);
        }

        /* Iterate over the symbol table to find entry symbols */
        current = head_of_symbol_table;
        while (current != NULL) {

            /* Check if the symbol has entry flag set to 1 */
            if(current->entry_flag == 1) {

                /* Write to the entries file the name and address of the symbol */
                fprintf(entries_output_file, "%s %07d\n", current->symbol_name, current->address);
            }

            /* Move to the next symbol in the table */
            current = current->next;
        }

        /* Close the entries file */
        fclose(entries_output_file);

        /* Free the memory allocated for the entries file name */
        free(entries_file_name);
    }
}

/**
 * Compares the addresses of two BinaryMachineCode structures
 *
 * @param x Pointer to the first BinaryMachineCode structure
 * @param y Pointer to the second BinaryMachineCode structure
 * @return int the difference between the addresses.
 *             a positive number if the address of 'x' is greater,
 *             a negative number if 'x' is smaller, and zero if the addresses are equal.
 */
static int compare_addresses(const void *x, const void *y) {
    return ((BinaryMachineCode *)x)->address - ((BinaryMachineCode *)y)->address;
}

/**
 * Sorts a linked list of machine code words by their addressess.
 *
 * @param head_of_lines_list Pointer to the head of the assembly line list
 * @param ICF The instruction counter value
 * @param DCF The data counter value
 * @return BinaryMachineCode A pointer to the sorted array of BinaryMachineCode
 */
static BinaryMachineCode *sort_machine_code_list(AssemblyLineList *head_of_lines_list, int ICF, int DCF) {

    /* Calculate the total number of words in the list based on ICF and DCF */
    size_t word_count = (ICF - 100) + DCF;

    BinaryMachineCode *array;
    AssemblyLineList *current;

    int i;
    int array_index = 0;

    array = malloc(word_count * sizeof(BinaryMachineCode));
    if(check_memory_allocation(array) == FALSE) {
        exit(1);
    }

    current = head_of_lines_list;

    /* Traverse the assembly line list */
    while (current != NULL) {
        BinaryMachineCode *current_code = current->code;

        /* Traverse the machine code linked list for the current assembly line */
        while (current_code != NULL) {

            array[array_index].word = malloc(strlen(current_code->word) + 1);
            if(check_memory_allocation(array[array_index].word) == FALSE) {
                free(array);
                exit(1);
            }

            /* Copy the current machine code word into the array */
            strcpy(array[array_index].word, current_code->word);
            array[array_index].address = current_code->address;
            array[array_index].next = NULL;

            /* Link the current node in the array to the previous one (if it's not the first node) */
            if (array_index > 0) {
                array[array_index - 1].next = &array[array_index];
            }

            array_index++;

            /* Move to the next machine code word */
            current_code = current_code->next;
        }
        current = current->next;
    }

    /* Sort the array based on the address field of the BinaryMachineCode */
    qsort(array, word_count, sizeof(BinaryMachineCode), compare_addresses);

    /* After sorting, rebuild the linked list structure by linking the array elements */
    for (i = 0; i < word_count - 1; i++) {

        /* Link the current array node */
        array[i].next = &array[i + 1];
    }

    /* Set the last node to NULL */
    array[word_count - 1].next = NULL;

    return array;
}

/**
 * Creates an object file containing the assembled machine code and their matching addresses.
 *
 * @param head_of_lines_list Pointer to the head of the assembly line list
 * @param file_name The file name without any extension
 * @param ICF The instruction counter value
 * @param DCF The data counter value
 * @return void
 */
static void create_object_file(AssemblyLineList *head_of_lines_list, char *file_name, int ICF, int DCF) {
    char *object_file_name;
    FILE *object_output_file;

    BinaryMachineCode *sorted_code;
    BinaryMachineCode *current_code;

    /* create the object file name */
    object_file_name = malloc(strlen(file_name) + 4);

    if(check_memory_allocation(object_file_name) == FALSE) {
        exit(1);
    }
    object_file_name[0] = '\0';

    /* Concatenate the file name with ".ob" extension */
    strcat(object_file_name, file_name);
    strcat(object_file_name, ".ob");

    /* Open the object file for writing */
    object_output_file = fopen(object_file_name, "w");
    if (check_file_open(object_output_file, object_file_name) == FALSE) {
        exit(1);
    }

    /* Write the instruction and data counters (ICF - 100 and DCF) to the object file */
    fprintf(object_output_file, "    %d %d\n", ICF-100, DCF);

    /* Sort the machine code linked list */
    sorted_code = sort_machine_code_list(head_of_lines_list, ICF, DCF);
    current_code = sorted_code;

    while (current_code != NULL) {
        /* Convert the binary word to a decimal value */
        long decimal_value = strtol(current_code->word, NULL, 2);

        /* Write the machine code address and its corresponding decimal value to the object file */
        fprintf(object_output_file, "%07d %06lx\n", current_code->address, decimal_value);
        current_code = current_code->next;
    }

   /* Close the object file */
    fclose(object_output_file);

    /* Free the memory allocated for the object file name */
    free(object_file_name);

    /* Free the machine code list */
    free_machine_code_list(sorted_code, ICF, DCF);
}

/**
 * Main function that implements an assembler.
 * It processes a list of assembly files, performing the full assembly process,
 * which includes preprocessing, a first pass, and a second pass.
 * During the process, it checks for errors in the assembly code.
 * At the end of the process, the following files may be created:
 *   .am file: Contains the assembly code after macro expansion.
 *             This file is only created if no errors were found during preprocessing.
 *   .ob file: Contains the memory image of the machine code.
 *             This file is only created if no errors were found during the first or second pass.
 *   .ent file: Lists all symbols defined as entry along with their addresses.
 *              This file is only created if there are entry symbols.
 *   .ext file: Lists all symbols defined as external and the machine code addresses where these symbols are referenced.
 *              This file is only created if there are external symbols.
 *
 * @param argc The number of command-line arguments
 * @param argv An array of strings representing the command-line arguments
 *             Each argument after the program name is expected to be an assembly source file name (without extension)
 *
 * @return 0 (indicating successful execution)
 */
int main(int argc, char *argv[]) {
    int i;

    /* Iterate over all the files passed as arguments */
    for (i = 1; i < argc; i++) {
        FILE *original_source_file;
        char* expanded_to_as_file;
        MacroNode *head_of_macro_table = NULL;

        expanded_to_as_file = malloc(strlen(argv[i]) + 5);

        if(check_memory_allocation(expanded_to_as_file) == FALSE) {
            exit(1);
        }

        /* Concatenate the file name with ".as" extension */
        strcpy(expanded_to_as_file, argv[i]);
        strcat(expanded_to_as_file, ".as");

        /* Open the original source file for reading */
        original_source_file = fopen(expanded_to_as_file, "r");
        if (original_source_file == NULL) {
            free(expanded_to_as_file);
            printf("Error opening file: %s\n", argv[i]);
            return 1;
        }

        if(fgetc(original_source_file) == EOF) {
            printf("File %s is empty.\n", expanded_to_as_file);
            fclose(original_source_file);
            free(expanded_to_as_file);
            return 1;
        }

        rewind(original_source_file);

        /* Check if there are errors in the preprocessing, if no errors found continue with preprocessing */
        if(check_preprocessing_errors(original_source_file) == ERROR_WAS_NOT_FOUND) {
            char *am_file_name;
            FILE *am_output_file;

            am_file_name = malloc(strlen(argv[i]) + 5);
            if(check_memory_allocation(am_file_name) == FALSE) {
                exit(1);
            }

            /* Concatenate the file name with ".am" extension */
            strcpy(am_file_name, argv[i]);
            strcat(am_file_name, ".am");

            /* Rewind the original source file to start over */
            rewind(original_source_file);

            /* create the file preprocessing function and get the macro table */
            head_of_macro_table = file_preprocessing(original_source_file, am_file_name);

            /* Open the expanded source file for reading */
            am_output_file = fopen(am_file_name, "r");
            if (check_file_open(am_output_file, am_file_name) == FALSE) {
                exit(1);
            }

            /* Check if there are errors in the first pass, if no errors found continue with first pass */
            if(check_errors_in_first_pass(am_output_file, head_of_macro_table) == ERROR_WAS_NOT_FOUND) {
                FirstPassResult result_of_first_pass;
                AssemblyLineList *head_of_lines_list;
                SymbolNode *head_of_symbol_table;
                int ICF = 0, DCF = 0;

                /* Rewind the expanded source file to start over */
                rewind(am_output_file);

                /* Perform the first pass and get the result,
                 * which includes the assembly line list, symbol table,
                 * and the values for ICF (instruction count) and DCF (data count) */
                result_of_first_pass = first_pass(am_output_file);
                head_of_lines_list = result_of_first_pass.head_of_lines_list;
                head_of_symbol_table = result_of_first_pass.head_of_symbol_table;
                ICF = result_of_first_pass.ICF;
                DCF = result_of_first_pass.DCF;

                /* Rewind the expanded source file to start over */
                rewind(am_output_file);


                /* Check if there are errors in the second pass, if no errors found continue with second pass */
                if(check_errors_in_second_pass(am_output_file, head_of_symbol_table) == ERROR_WAS_NOT_FOUND) {
                    SecondPassResult result_of_second_pass;

                    /* Perform the second pass and get the result,
                     * which includes the updated assembly line list and symbol table */
                    result_of_second_pass = second_pass(head_of_symbol_table, &head_of_lines_list);

                    /* Update the line list and symbol table with the result of the second pass */
                    head_of_lines_list = result_of_second_pass.head_of_lines_list;
                    head_of_symbol_table = result_of_second_pass.head_of_symbol_table;

                    create_object_file(head_of_lines_list, argv[i], ICF, DCF);
                    create_entries_file(head_of_symbol_table, argv[i]);
                    create_externals_file(head_of_symbol_table, head_of_lines_list, argv[i]);

                }

                /* Free the assembly line list and symbol table after processing */
                free_line_list(head_of_lines_list);
                free_symbol_table(head_of_symbol_table);

            }

            /* Close the expanded source file */
            fclose(am_output_file);

            /* Free memory allocated for expanded file name */
            free(am_file_name);

            /* Free the macro table */
            free_macro_nodes(head_of_macro_table);
        }

        /* Close the original source file */
        fclose(original_source_file);

        /* Free memory allocated for expanded file name */
        free(expanded_to_as_file);

    }
    return 0;
}
