#include <stdio.h>
#include <string.h>
#include <stdlib.h>

#include "first_second_pass.h"

int is_symbol (char *line) {
    int i=0;
    int find_space = 0;

    /* Traverse through the characters in the line */
    while (line[i] != '\n' && line[i] != '\0') {

        /* If a space character is found, set the space flag */
        if(line[i] == ' ') {
            find_space = 1;
        }

        /* If ':' is found and no space before it, return TRUE */
        if(line[i] == ':' && find_space == 0) {
            return TRUE;
        }
        i++;
    }
    return FALSE;
}

int check_errors_in_first_pass(FILE *fd, MacroNode *head_of_macro_table) {
    char line[MAX_LINE] = {0};
    int line_number = 0; /* Tracks the current line number in the file */
    int error_flag = ERROR_WAS_NOT_FOUND; /* Flag to indicate if errors are found */

    /* Read line by line from the file until the end */
    while (fgets(line, sizeof(line), fd)) {
        ErrorCode error_check = NO_ERROR;
        line_number++;

        if (strlen(line) > MAX_LINE - 1) {
            error_check = ERR_LINE_TOO_LONG;
            print_error(error_check, line_number);
        }

        /* If a macro table exists, check if the line contains a macro label */
        else if (head_of_macro_table != NULL) {
            error_check = is_label_macro(line, head_of_macro_table);
        }

        /* If an error is found, print it and update the error flag */
        if (error_check != NO_ERROR) {
            print_error(error_check, line_number);
            error_flag = ERROR_FOUND;
        }
        else {
            /* Check for error in the current line */
            error_check = check_line_errors_first_pass(line);

            /* If an error is found, print it and update the error flag */
            if (error_check != NO_ERROR) {
                print_error(error_check, line_number);
                error_flag = ERROR_FOUND;
            }
        }
    }
    return error_flag;
}

FirstPassResult first_pass(FILE *fd) {

    /* Initialize instruction counter (IC) to 100 and data counter (DC) to 0 */
    int IC = 100, DC = 0;

    /* Buffers for line manipulation */
    char line[MAX_LINE] = {0}, first_copy_line[MAX_LINE] = {0}, second_copy_line[MAX_LINE] = {0};

    /* Initialize the result structure */
    FirstPassResult result = {NULL, NULL};

    char *symbol = NULL, *command = NULL, *remaining_line = NULL;
    char *first_word = NULL, *extra_word = NULL;

    int symbol_flag=0;
    int L;

    /* Read the file line by line */
    while (fgets(line, sizeof(line), fd)) {
        strcpy(first_copy_line, line);
        strcpy(second_copy_line, line);

        /* Check if line starts with a symbol (label) */
        if (is_symbol(line) == TRUE) {

            /* Set the symbol flag */
            symbol_flag = 1;

            /* Extract symbol name */
            symbol = strtok(first_copy_line, ":");

            /* Extract command after symbol */
            command = strtok(NULL, " \n\t");
        } else {

            /* Extract command when no symbol */
            command = strtok(first_copy_line, " \nֿ");
        }

        /* Get the rest of the line after the command */
        if ((strcmp(command, "stop") != 0 ) && (strcmp(command, "rts") != 0) ) {

            /* For "stop" and "rts" command, no operands */
            remaining_line = strtok(NULL, "");
        } else {
            remaining_line = command;
        }

        /* Process data and string directives */
        if (strcmp(command, ".data") == 0 || strcmp(command, ".string") == 0) {

            /* If there is a symbol, add it to symbol table as DATA type */
            if (symbol_flag == 1) {
                insert_to_symbol_table(&result.head_of_symbol_table, symbol, DATA, DC);
            }

            /* Process .data directive - create machine code for numbers */
            if (strcmp(command, ".data") == 0) {
                BinaryMachineCode *head_of_binary_code = NULL;
                char *operator = strtok(remaining_line, " ,\n");

                /* Iterate through all numbers in the directive */
                while (operator != NULL) {

                    /* Create a binary machine code word for the current number */
                    extra_word = create_extra_word(operator, DATA);

                    /* Insert the word into the binary machine code linked list */
                    insert_word(&head_of_binary_code, extra_word, DC);

                    /* Advance the data counter */
                    DC++;

                    /* Get next number */
                    operator = strtok(NULL, " ,\n");

                }

                /* Insert the processed .data line into the assembly lines list */
                insert_line(&result.head_of_lines_list, 0, line, DATA, head_of_binary_code);
            }

            /* Process .string directive - create machine code for string characters */
            if (strcmp(command, ".string") == 0) {
                BinaryMachineCode *head_of_binary_code = NULL;
                char *binary_word;

                /* Find the first quotation mark */
                char *start = strchr(line, '"');

                /* Find the last quotation mark */
                char *end = strrchr(line, '"');

                int i;


                if (start != NULL && end != NULL && start != end) {
                    /* Move past the opening quotation mark */
                    start++;

                    /* Terminate the string before the closing quotation mark */
                    *end = '\0';
                }

                /* Iterate over each character in the extracted string */
                for (i = 0; start[i] != '\0'; i++) {
                    char *binary_word;

                    /* Convert the character to a string format */
                    char operator_char[2];
                    operator_char[0] = start[i];
                    operator_char[1] = '\0';

                    /* Create binary word for the character */
                    binary_word = create_extra_word(operator_char, STRING);

                    /* Insert the character into the binary machine code list */
                    insert_word(&head_of_binary_code, binary_word, DC);

                    /* Advance the data counter */
                    DC++;
                }

                /* Create binary word for '\0' end-of-string character */
                binary_word = create_extra_word("", STRING);

                /* Insert the binary word into the binary code linked list */
                insert_word(&head_of_binary_code, binary_word, DC);

                /* Advance the data counter */
                DC++;

                /* Insert the processed .string directive into the assembly lines list */
                insert_line(&result.head_of_lines_list, 0, line, DATA, head_of_binary_code);
            }

        /* Process extern directive - add external symbol to table */
        } else if (strcmp(command, ".extern") == 0) {

            /* Remove newline character from the remaining line */
            remaining_line[strcspn(remaining_line, "\n")] = '\0';
            symbol = remaining_line;

            /* Insert the external symbol into the symbol table with a value of 0 */
            insert_to_symbol_table(&result.head_of_symbol_table, symbol, EXTERN, 0);

        /* Process entry directive */
        } else if (strcmp(command, ".entry") == 0) {
            BinaryMachineCode *head_of_binary_code = NULL;
            char *empty_word = malloc(1);
            /* Check if memory allocation was successful */
            if (check_memory_allocation(empty_word) == FALSE) {
                exit(1);
            }
            empty_word[0] = '\0';


            /* Insert a placeholder word into the binary machine code list (empty string, since it's an entry) */
            insert_word(&head_of_binary_code, empty_word, DC);

            /*  Insert the processed .entry line into the assembly lines list */
            insert_line(&result.head_of_lines_list, 0, line, ENTRY, head_of_binary_code);

        /* Process instruction commands */
        } else {
            BinaryMachineCode *head_of_binary_code;

            /* If there is a symbol, add it to symbol table as CODE type */
            if (symbol_flag == 1) {
                insert_to_symbol_table(&result.head_of_symbol_table, symbol, CODE, IC);
            }

            /* Create the first word of the instruction */
            first_word = create_first_word(second_copy_line, symbol_flag);
            head_of_binary_code = NULL;

            /* Add the first word to the binary machine code list at the current IC address */
            insert_word(&head_of_binary_code, first_word, IC);
            IC++;

            /* Calculate how many words are needed */
            if (remaining_line != NULL) {
                char temp_copy[MAX_LINE] = {0};
                strcpy(temp_copy, remaining_line);
                L = number_of_words(temp_copy, command);
            }

            /* If there are more than one word that are nedded */
            if (L > 1) {
                char *operator = NULL;
                operator = strtok(remaining_line, " ,\n");

                /* Loop through each operator */
                while (operator != NULL) {
                    /* If the operator is not a register */
                    if (is_register(operator) == FALSE) {

                        /* Create the binary word for the operator */
                        char *binary_word = create_extra_word(operator, INSTRUCTION);

                        /* Insert the binary word to the binary machine code list at the current IC address */
                        insert_word(&head_of_binary_code, binary_word, IC);
                        IC++;

                    }

                    /* Move to the next operator */
                    operator = strtok(NULL, " ,\n");
                }

            }

            /* Insert the processed instruction line into the lines list */
            insert_line(&result.head_of_lines_list, L, line, CODE, head_of_binary_code);
        }

        /* Reset the symbol flag */
        symbol_flag = 0;
    }

    /* Store the final instruction and data counter values in the result structure */
    result.ICF = IC;
    result.DCF = DC;

    /* Update addresses of data lines, shifting them after the code section */
    update_data_line_list_addresses(result.head_of_lines_list, result.ICF);

    /* Update addresses of data symbols in the symbol table */
    update_data_symbol_addresses(result.head_of_symbol_table,result.ICF);

    return result;

}

