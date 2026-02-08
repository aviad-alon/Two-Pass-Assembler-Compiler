#include <ctype.h>
#include <string.h>
#include <stdlib.h>
#include <stdio.h>
#include "error_handler.h"

void print_error(ErrorCode code, int num_of_line) {
    switch (code) {
        case ERR_UNDEFINED_COMMAND:
            printf("Error in line %d: Undefined command name.\n", num_of_line);
            break;
        case ERR_INVALID_PARA:
            printf("Error in line %d: Invalid parameter.\n", num_of_line);
            break;
        case ERR_INVALID_REG:
            printf("Error in Line %d: Invalid register.\n", num_of_line);
            break;
        case ERR_MISSING_PARA:
            printf("Error in Line %d: Missing parameter.\n", num_of_line);
            break;
        case ERR_MISSING_COMMA:
            printf("Error in Line %d: Missing comma.\n", num_of_line);
            break;
        case ERR_CONS_COMMAS:
            printf("Error in Line %d: Multiple consecutive commas.\n", num_of_line);
            break;
        case ERR_ILLEGAL_COMMA:
            printf("Error in Line %d: Illegal comma.\n", num_of_line);
            break;
        case ERR_EXTRANEOUS_TEXT:
            printf("Error in Line %d: Extraneous text after end of command.\n", num_of_line);
            break;
        case ERR_SRC_ADDRESSING:
            printf("Error in Line %d: Illegal addressing mode for source operand.\n",num_of_line);
            break;
        case ERR_DEST_ADDRESSING:
            printf("Error in Line %d: Illegal addressing mode for destination operand.\n",num_of_line);
            break;
        case ERR_INVALID_LABEL_NAME:
            printf("Error in Line %d: A label name cannot be a reserved assembly keyword.\n", num_of_line);
            break;
        case ERR_LABEL_MACRO:
            printf("Error in Line %d: A label name cannot be the same as a macro name.\n", num_of_line);
            break;
        case ERR_MISSING_QUOTATION:
            printf("Error in Line %d: Missing quotation mark.\n", num_of_line);
            break;
        case ERR_SYMBOL_NOT_FOUND:
            printf("Error in Line %d: Used symbol not found.\n", num_of_line);
            break;
        case ERR_LINE_TOO_LONG:
            printf("Error in Line %d: The length of a line is limited to 80 characters.\n", num_of_line);
            break;
        case ERR_SYMBOL_TOO_LONG:
            printf("Error in Line %d: The length of a label name is limited to 31 characters.\n", num_of_line);
            break;
        case ERR_INVALID_SYMBOL_CHAR:
            printf("Error in Line %d: Symbol contains an invalid character. Only letters and digits are allowed.\n", num_of_line);
            break;
        case ERR_INVALID_SYMBOL_START:
            printf("Error in Line %d: Symbol's name must start with a letter.\n", num_of_line);
            break;
        case ERR_EMPTY_LABEL_LINE:
            printf("Error in Line %d: No command or directive detected after the label name.\n", num_of_line);
            break;
        case NO_ERROR:
            break;
    }
}

/**
 * Checks if a given string is a valid assembly command.
 *
 * @param command Pointer to the string representing the command to check
 * @return int TRUE if the string matches a known assembly command, otherwise FALSE
 */
static int is_command(char* command) {
    if(strcmp(command, "mov") == 0 || strcmp(command, "cmp") == 0 || strcmp(command, "add") == 0 ||
       strcmp(command, "sub") == 0 || strcmp(command, "lea") == 0 || strcmp(command, "clr") == 0 ||
       strcmp(command, "not") == 0 || strcmp(command, "inc") == 0 || strcmp(command, "dec") == 0 ||
       strcmp(command, "jmp") == 0 || strcmp(command, "bne") == 0 || strcmp(command, "jsr") == 0 ||
       strcmp(command, "red") == 0 || strcmp(command, "prn") == 0 || strcmp(command, "rts") == 0 ||
       strcmp(command, "stop") == 0) {
        return TRUE;
    }
    return FALSE;
}

int is_register(char* operand) {
    if(strcmp(operand, "r0") == 0 || strcmp(operand, "r1") == 0 || strcmp(operand, "r2") == 0 ||
       strcmp(operand, "r3") == 0 || strcmp(operand, "r4") == 0 || strcmp(operand, "r5") == 0 ||
       strcmp(operand, "r6") == 0 || strcmp(operand, "r7") == 0 ) {
        return TRUE;
    }
    return FALSE;
}

int is_directive(char *command) {
    if(strcmp(command, ".entry") == 0 || strcmp(command, ".extern") == 0 ||
       strcmp(command, ".string") == 0 || strcmp(command, ".data") == 0) {
        return TRUE;
    }
    return FALSE;
}

/**
 * Checks if a symbol is a valid label name by verifying it is not a command, register, or directive.
x * @param symbol Pointer to the string representing the symbol name to check
 *
 * @return ErrorCode -  ERR_SYMBOL_TOO_LONG if the symbol name is too long
 *                      ERR_INVALID_SYMBOL_CHAR if the symbol name contains invalid characters
 *                      ERR_INVALID_LABEL_NAME if the symbol name is a command, register, or directive
 *                      NO_ERROR if the symbol is valid
 */
static ErrorCode check_symbol_name(char *symbol) {
    int i;

    /* Check if the symbol length is too long */
    if(strlen(symbol) > MAX_SYMBOL_NAME) {
        return ERR_SYMBOL_TOO_LONG;
    }

    if(!isalpha(symbol[0])) {
        return ERR_INVALID_SYMBOL_START;
    }

    /* Check if the symbol contains only letters or digits */
    for (i = 0; i < strlen(symbol); i++) {
        if (!(isalpha(symbol[i]) || isdigit(symbol[i]))) {
            return ERR_INVALID_SYMBOL_CHAR;
        }
    }

    /* Check if the symbol matches any command, register, or directive */
    if(is_command(symbol) == TRUE || is_register(symbol) == TRUE || is_directive(symbol) == TRUE) {
        return ERR_INVALID_LABEL_NAME;
    }
    return NO_ERROR;
}

/**
 * Validates an operand based on its addressing mode.
 * - For immediate addressing (#number), it verifies that the operand is a valid number.
 * - For register direct addressing, it checks if the operand is a valid register name.
 * - For other addressing modes, validation will be performed during the second pass.
 *
 * @param operand The operand to validate
 * @return ErrorCode - ERR_INVALID_PARA for an invalid immediate value,
 *                     ERR_INVALID_REG for an invalid register,
 *                     NO_ERROR if the operand is valid.
 */
static ErrorCode check_operand(char *operand) {
    char *endptr = NULL;
    AddressingCase addressing_mode;

    /* Determine the addressing mode of the operand */
    addressing_mode = get_operand_addressing_mode(operand);

    /* Handle immediate addressing mode */
    if(addressing_mode == IMMEDIATE_ADDRESSING) {

        /* Pass the '#' */
        operand = operand + 1;
        strtod(operand, &endptr);

        /* Check if the parameter is not a valid number */
        if (endptr == operand || *endptr != '\0') {
            return ERR_INVALID_PARA;
        }
    }

    /* Handle register addressing mode */
    if(addressing_mode == REGISTER_DIRECT_ADDRESSING) {

        /* Check if operand is a valid register */
        if(is_register(operand) == FALSE) {
            return ERR_INVALID_REG;
        }
    }

    return NO_ERROR;
}

/**
 * Checks if a given label name matches any macro name in the macro table.
 *
 * @param symbol The label name to check.
 * @param head_of_macro_table Pointer to the head of the macro table.
 * @return ErrorCode - ERR_LABEL_MACRO if the label is a macro name, otherwise NO_ERROR
 */
static ErrorCode search_label_macro (char *symbol,MacroNode *head_of_macro_table) {
    MacroNode *current = head_of_macro_table;

    /* Traverse through the macro table */
    while (current != NULL) {

        /* Compare the symbol (label) name with the current macro name */
        if (strcmp(symbol, current->data) == 0) {

            /* If a match is found, return error - labels cannot have the same name as macros */
            return ERR_LABEL_MACRO;
        }

        /* Move to the next macro in the linked list */
        current = current->next;
    }
    return NO_ERROR;
}

ErrorCode is_label_macro(char *line, MacroNode *head_of_macro_table) {
    char *input = malloc(strlen(line) + 1);
    char *original_input;
    int i = 0, j = 0;
    ErrorCode error_check = NO_ERROR;

    if(check_memory_allocation(input) == FALSE) {
        exit(1);
    }

    /* Stores the original address of input and command to ensure proper memory deallocation */
    original_input = input;

    strcpy(input, line);

    /* Skip leading spaces in input */
    while (isspace(input[i])) i++;
    input = input + i;
    i = 0;

    /* Check if the line starts with a symbol (label) */
    if(is_symbol(line) == TRUE) {
        char symbol_name[MAX_LINE] = {0};

        /* Extract the symbol name from input */
        while (input[i] != ':') {
            symbol_name[j++] = input[i++];
        }
        symbol_name[j] = '\0';

        /* Check if the symbol name is already a macro name */
        error_check = search_label_macro(symbol_name, head_of_macro_table);

        free(original_input);
        return error_check;
    }
    free(original_input);
    return error_check;
}

ErrorCode check_line_errors_first_pass(char *line) {
    char *input = malloc(strlen(line) + 1);
    char *command = malloc(strlen(line) + 1);
    char *original_input;
    char *original_command;
    int i=0, j=0;

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

    /* Check if the line starts with a symbol (label) */
    if(is_symbol(input) == TRUE) {
        char symbol_name[MAX_LINE] = {0};
        ErrorCode error_check = NO_ERROR;

        /* Extract the symbol name from input */
        while (input[i] != ':') {
            symbol_name[j++] = input[i++];
        }
        symbol_name[j] = '\0';
        input = input + i + 1; /* Skip past ':' */
        i=0;
        j=0;

        /* Check if the symbol name is valid */
        error_check = check_symbol_name(symbol_name);
        if(error_check != NO_ERROR) {
            free(original_input);
            free(original_command);
            return error_check;

        }

        /* Skip leading spaces in input */
        while (isspace(input[i])) i++;
        input = input + i;
        i = 0;

        /* Check if the input ends right after the symbol_name */
        if (input[0] == '\0') {
            free(original_input);
            free(original_command);
            return ERR_EMPTY_LABEL_LINE;
        }

    }

    /* Skip leading spaces in input */
    while (isspace(input[i])) i++;
    input = input + i;
    i = 0;

    /* Check if input starts with a comma (illegal input) */
    if (input[i] == ',') {
        free(original_input);
        free(original_command);
        return ERR_ILLEGAL_COMMA;
    }

    /* Extract the command name from input */
    while (!isspace(input[i]) && input[i] != '\0' && input[i] != ',') {
        command[j++] = input[i++];
    }
    command[j] = '\0';
    input = input + i;
    i = 0;
    j = 0;

    /* If it is a directive */
    if(is_directive(command) == TRUE) {

        /* Handle string directive */
        if(strcmp(command, ".string") == 0) {
            int last_quote_index;

            /* Skip spaces after the ".string" command */
            while (isspace(input[i])) i++;

            /* Check for missing parameter after the command */
            if (input[i] == '\0') {
                free(original_input);
                free(original_command);
                return ERR_MISSING_PARA;
            }

            /* Check for illegal comma immediately following the command */
            if (input[i] == ',') {
                free(original_input);
                free(original_command);
                return ERR_ILLEGAL_COMMA;
            }
            input = input + i;
            i=0;

            /* Check for opening quotation mark */
            if (input[i] != '"') {
                free(original_input);
                free(original_command);
                return ERR_MISSING_QUOTATION;
            }

            i++;
            input = input + i;
            i = 0;


            /* Look for the closing quotation mark */
            last_quote_index = -1;
            while (input[i] != '\0') {
                if (input[i] == '"') {

                    /* Update the position of the last found quote */
                    last_quote_index = i;
                }
                i++;
            }

            /* If no closing quote was found, return an error */
            if (last_quote_index == -1) {
                free(original_input);
                free(original_command);
                return ERR_MISSING_QUOTATION;
            }


            input = input + last_quote_index + 1;
            i = 0;

            /* Check for extraneous text after parameters the directive */
            while (input[i] != '\0') {
                if (!isspace(input[i])) {
                    free(original_input);
                    free(original_command);
                    return ERR_EXTRANEOUS_TEXT;
                }
                i++;
            }

            free(original_input);
            free(original_command);
            return NO_ERROR;
        }

        /* Handle data directive */
        if(strcmp(command, ".data") == 0) {
            int number_of_comma = 0;
            int temp_i;
            int last_number_flag = 0;

            /* Skip spaces after the ".data" command */
            while (isspace(input[i])) i++;

            /* Check for missing parameter after the command */
            if (input[i] == '\0') {
                free(original_input);
                free(original_command);
                return ERR_MISSING_PARA;
            }

            /* Check for illegal comma immediately following the command */
            if (input[i] == ',') {
                free(original_input);
                free(original_command);
                return ERR_ILLEGAL_COMMA;
            }
            input = input + i;
            i=0;

            /* Loop until the last number */
            while (last_number_flag != 1) {

                if (input[i] == '+' || input[i] == '-') {
                    i++;
                }

                while (isdigit(input[i])) {
                    i++;
                }

                /* Check if it's the last number */
                temp_i = i;
                while (isspace(input[temp_i])) temp_i++;
                if (input[temp_i] == '\0') {
                    last_number_flag = 1;
                }

                /* Check for illegal character after the number */
                if((input[i] != ',') && (input[i] !=  ' ') && input[i] != '\0' && input[i] != '\n') {
                    free(original_input);
                    free(original_command);
                    return ERR_INVALID_PARA;
                }

                /* If it's not the last number */
                if(last_number_flag == 0) {
                    while (isspace(input[i])) i++;
                    input = input + i;
                    i = 0;

                    /* Count commas until the first alphanumeric characters */
                    while (input[i] != '\0' && (input[i] == ' ' || input[i] == ',' || input[i] == '\t')) i++;

                    for (j = 0; j < i; j++) {
                        if (input[j] == ',') {
                            number_of_comma++;
                        }
                    }

                    /* Check for consecutive or missing commas */
                    if (number_of_comma > 1 || number_of_comma == 0) {
                        if (number_of_comma > 1) {
                            free(original_input);
                            free(original_command);
                            return ERR_CONS_COMMAS;
                        }
                        if (number_of_comma == 0) {
                            free(original_input);
                            free(original_command);
                            return ERR_MISSING_COMMA;
                        }
                    }
                    number_of_comma = 0;

                    input = input + i;
                    i = 0;
                }
            }
            free(original_input);
            free(original_command);
            return NO_ERROR;
        }

        /* Handle extern directive */
        if(strcmp(command, ".extern") == 0 || strcmp(command, ".entry") == 0) {
            char symbol_name[MAX_LINE] = {0};
            int j=0;

            /* Skip spaces after the ".extern"/ ".entry" command */
            while (isspace(input[i])) i++;

            /* Check for missing parameter after the command */
            if (input[i] == '\0') {
                free(original_input);
                free(original_command);
                return ERR_MISSING_PARA;
            }

            /* Check for illegal comma immediately following the command */
            if (input[i] == ',') {
                free(original_input);
                free(original_command);
                return ERR_ILLEGAL_COMMA;
            }
            input = input + i;
            i=0;

            /* Extract the symbol name from input */
            while (!isspace(input[i]) && input[i] != ',' && input[i] != '\0') {
                symbol_name[j++] = input[i++];
            }
            symbol_name[j] = '\0';

            /* Check if the symbol is a command, register, or directive */
            if(is_command(symbol_name) == TRUE || is_register(symbol_name) == TRUE || is_directive(symbol_name) == TRUE) {
                free(original_input);
                free(original_command);
                return ERR_INVALID_LABEL_NAME;
            }

            /* Check if the symbol is a valid symbol */
            if (get_operand_addressing_mode(symbol_name) == INVALID_ADDRESSING) {
                free(original_input);
                free(original_command);
                return ERR_INVALID_SYMBOL_CHAR;
            }

            input = input + i;
            i=0;

            /* Check for extraneous text after directive */
            while (input[i] != '\0') {
                if (!isspace(input[i])) {
                    free(original_input);
                    free(original_command);
                    return ERR_EXTRANEOUS_TEXT;
                }
                i++;
            }
            free(original_input);
            free(original_command);
            return NO_ERROR;
        }

        free(original_input);
        free(original_command);
        return NO_ERROR;
    }

    /* is a command line */
    else {
        int i = 0, j = 0;
        int number_of_comma = 0;
        char first_operand[MAX_LINE] = {0};
        char second_operand[MAX_LINE] = {0};

        /* Check if the command is defined */
        if (is_command(command) == FALSE) {
            free(original_input);
            free(original_command);
            return ERR_UNDEFINED_COMMAND;
        }

        /* Handle commands with 2 operands */
        if (number_of_operands(command) == 2) {

            /* Skip spaces after the command */
            while (isspace(input[i])) i++;

            /* Check for illegal comma immediately following the command */
            if (input[i] == ',') {
                free(original_input);
                free(original_command);
                return ERR_ILLEGAL_COMMA;
            }
            input = input + i;
            i = 0;

            /* Check for missing parameter after the command */
            if (input[i] == '\0') {
                free(original_input);
                free(original_command);
                return ERR_MISSING_PARA;
            }

            /* Extract the first operand from the input */
            while (!isspace(input[i]) && input[i] != ',' && input[i] != '\0') {
                first_operand[j++] = input[i++];
            }
            first_operand[j] = '\0';

            /* Validate the addressing mode of the first operand based on the command */
            if (strcmp(command, "mov") == 0 || strcmp(command, "add") == 0 || strcmp(command, "sub") == 0
                || strcmp(command, "cmp") == 0) {

                /* Check if the first operand has a relative addressing mode,
                 * which is not allowed for the current commands */
                if (get_operand_addressing_mode(first_operand) == RELATIVE_ADDRESSING) {
                    free(original_input);
                    free(original_command);
                    return ERR_SRC_ADDRESSING;
                }
                if (get_operand_addressing_mode(first_operand) == INVALID_ADDRESSING) {
                    free(original_input);
                    free(original_command);
                    return ERR_INVALID_PARA;
                }

            } else if (strcmp(command, "lea") == 0) {

                if (get_operand_addressing_mode(first_operand) == INVALID_ADDRESSING) {
                    free(original_input);
                    free(original_command);
                    return ERR_INVALID_PARA;
                }
                /* Check if the first operand is not using direct addressing mode, which is required for "lea" */
                if (get_operand_addressing_mode(first_operand) != DIRECT_ADDRESSING) {
                    free(original_input);
                    free(original_command);
                    return ERR_SRC_ADDRESSING;
                }
            }

            /* Check if the first operand is valid */
            if (check_operand(first_operand) == ERR_INVALID_PARA) {
                free(original_input);
                free(original_command);
                return ERR_INVALID_PARA; /* Return error for invalid parameter */
            }
            if (check_operand(first_operand) == ERR_INVALID_REG) {
                free(original_input);
                free(original_command);
                return ERR_INVALID_REG; /* Return error for invalid register */
            }

            input = input + i;
            i = 0;

            /* Count commas until the first alphanumeric characters */
            while (input[i] != '\0' && (input[i] == ' ' || input[i] == ',' || input[i] == '\t')) i++;

            for (j = 0; j < i; j++) {
                if (input[j] == ',') {
                    number_of_comma++;
                }
            }

            /* Check for consecutive or missing commas */
            if (number_of_comma > 1 || number_of_comma == 0) {
                if (number_of_comma > 1) {
                    free(original_input);
                    free(original_command);
                    return ERR_CONS_COMMAS;

                }
                if (number_of_comma == 0) {
                    free(original_input);
                    free(original_command);
                    return ERR_MISSING_COMMA;
                }
            }

            input = input + i;
            i = 0;
            j = 0;

            /* Skip spaces before the second operand */
            while (isspace(input[i])) i++;

            /* Check if the second operand is missing */
            if (input[i] == '\0') {
                free(original_input);
                free(original_command);
                return ERR_MISSING_PARA;
            }

            /* Extract the second operand from input */
            while ((!isspace(input[i]) && input[i] != ',' && input[i] != '\0') || input[i] == '.') {
                second_operand[j++] = input[i++];
            }
            second_operand[j] = '\0';

            /* Validate the addressing mode of the second operand based on the command */
            if (strcmp(command, "mov") == 0 || strcmp(command, "add") == 0 || strcmp(command, "sub") == 0
                || strcmp(command, "lea") == 0) {

                /* Check if the second operand uses relative or immediate addressing mode,
                 * which is not allowed for destination operands for the current commands*/
                if (get_operand_addressing_mode(second_operand) == RELATIVE_ADDRESSING || get_operand_addressing_mode(second_operand) == IMMEDIATE_ADDRESSING) {
                    free(original_input);
                    free(original_command);
                    return ERR_DEST_ADDRESSING;
                }
                if (get_operand_addressing_mode(second_operand) == INVALID_ADDRESSING) {
                    free(original_input);
                    free(original_command);
                    return ERR_INVALID_PARA;
                }
            } else if (strcmp(command, "cmp") == 0) {

                /* Check if the second operand uses relative addressing,
                 * which is not allowed for destination operand in "cmp" */
                if (get_operand_addressing_mode(second_operand) == RELATIVE_ADDRESSING) {
                    free(original_input);
                    free(original_command);
                    return ERR_DEST_ADDRESSING;
                }
                if (get_operand_addressing_mode(second_operand) == INVALID_ADDRESSING) {
                    free(original_input);
                    free(original_command);
                    return ERR_INVALID_PARA;
                }
            }

            /* Check if the second operand is valid */
            if (check_operand(second_operand) == ERR_INVALID_PARA) {
                free(original_input);
                free(original_command);
                return ERR_INVALID_PARA; /* Return error for invalid parameter */
            }
            if (check_operand(second_operand) == ERR_INVALID_REG) {
                free(original_input);
                free(original_command);
                return ERR_INVALID_REG; /* Return error for invalid register */
            }

            input = input + i;
            i = 0;

            /* Check for extraneous text after the operands */
            while (input[i] != '\0') {
                if (!isspace(input[i])) {
                    free(original_input);
                    free(original_command);
                    return ERR_EXTRANEOUS_TEXT;
                }
                i++;
            }
        }

        /* Handle commands with 1 operand */
        else if(number_of_operands(command) == 1) {

            /* Skip spaces after the command */
            while (isspace(input[i])) i++;

            /* Check for illegal comma immediately following the command */
            if (input[i] == ',') {
                free(original_input);
                free(original_command);
                return ERR_ILLEGAL_COMMA;
            }

            input = input + i;
            i=0;
            j=0;

            /* Check if the operand is missing */
            if (input[i] == '\0') {
                free(original_input);
                free(original_command);
                return ERR_MISSING_PARA;
            }

            /* Extract the operand from the input */
            while (!isspace(input[i]) && input[i] != ',' && input[i] != '\0') {
                first_operand[j++] = input[i++];
            }
            first_operand[j] = '\0';

            /* Validate the addressing mode of the operand based on the command */
            if(strcmp(command, "clr") == 0 || strcmp(command, "not") == 0 || strcmp(command, "inc") == 0
               || strcmp(command, "dec") == 0 || strcmp(command, "red") == 0) {

                /* Check if the operand uses relative or immediate addressing mode,
                 * which is not allowed for destination operands for the current commands*/
                if(get_operand_addressing_mode(first_operand) == IMMEDIATE_ADDRESSING || get_operand_addressing_mode(first_operand) == RELATIVE_ADDRESSING) {
                    free(original_input);
                    free(original_command);
                    return ERR_DEST_ADDRESSING;
                }
                if (get_operand_addressing_mode(first_operand) == INVALID_ADDRESSING) {
                    free(original_input);
                    free(original_command);
                    return ERR_INVALID_PARA;
                }
            }
            else if(strcmp(command, "jmp") == 0 || strcmp(command, "bne") == 0 || strcmp(command, "jsr") == 0) {

                /* Check if the operand uses register-direct addressing or immediate addressing mode,
                 * which is not allowed for destination operands for the current commands*/
                if(get_operand_addressing_mode(first_operand) == IMMEDIATE_ADDRESSING || get_operand_addressing_mode(first_operand) == REGISTER_DIRECT_ADDRESSING) {
                    free(original_input);
                    free(original_command);
                    return ERR_DEST_ADDRESSING;
                }
                if (get_operand_addressing_mode(first_operand) == INVALID_ADDRESSING) {
                    free(original_input);
                    free(original_command);
                    return ERR_INVALID_PARA;
                }
            }

            else if(strcmp(command, "prn") == 0) {

                /* Check if the operand uses relative addressing,
                 * which is not allowed for destination operand in "prn" */
                if(get_operand_addressing_mode(first_operand) == RELATIVE_ADDRESSING) {
                    free(original_input);
                    free(original_command);
                    return ERR_DEST_ADDRESSING;
                }
                if (get_operand_addressing_mode(first_operand) == INVALID_ADDRESSING) {
                    free(original_input);
                    free(original_command);
                    return ERR_INVALID_PARA;
                }
            }
            /* Check if the operand is valid */
            if(check_operand(first_operand) == ERR_INVALID_PARA) {
                free(original_input);
                free(original_command);
                return ERR_INVALID_PARA; /* Return error for invalid parameter */
            }
            if(check_operand(first_operand) == ERR_INVALID_REG) {
                free(original_input);
                free(original_command);
                return ERR_INVALID_REG; /* Return error for invalid register */
            }

            input = input + i;
            i = 0;

            /* Check for extraneous text after operand */
            while (input[i] != '\0') {
                if (!isspace(input[i])) {
                    free(original_input);
                    free(original_command);
                    return ERR_EXTRANEOUS_TEXT;
                }
                i++;
            }
        }

        /* Handle commands with 0 operands */
        else {
            /* Check for extraneous text after the command */
            while (input[i] != '\0') {
                if (!isspace(input[i])) {
                    free(original_input);
                    free(original_command);
                    return ERR_EXTRANEOUS_TEXT;
                }
                i++;
            }
        }
        free(original_input);
        free(original_command);
        return NO_ERROR;
    }
}
