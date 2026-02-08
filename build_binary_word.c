#include <stdlib.h>
#include <ctype.h>
#include <string.h>
#include "first_second_pass.h"
#include "build_binary_word.h"


/* Table of register names and their corresponding binary codes */
static RegisterNumber register_name_table[] = {
        {"r0", "000"},
        {"r1", "001"},
        {"r2", "010"},
        {"r3", "011" },
        {"r4", "100"},
        {"r5", "101" },
        {"r6", "110" },
        {"r7", "111" },
};

/* Table of addressing modes and their corresponding binary codes */
static AddressingMode addresing_mode_table[] = {
        {0, "00"},   /* Immediate addressing */
        {1, "01"},   /* Direct addressing */
        {2, "10"},   /* Relative addressing */
        {3, "11"}    /* Register direct addressing */

};

/* Table of assembly commands, their binary opcode, and their corresponding binary funct code */
static Command commands_table[] = {
        {"mov","000000", "00000"},
        {"cmp","000001", "00000"},
        {"add","000010", "00001"},
        {"sub","000010", "00010"},
        {"lea","000100", "00000"},
        {"clr","000101", "00001"},
        {"not","000101", "00010"},
        {"inc","000101", "00011"},
        {"dec","000101", "00100"},
        {"jmp","001001", "00001"},
        {"bne","001001", "00010"},
        {"jsr","001001", "00011"},
        {"red","001100", "00000"},
        {"prn","001101", "00000"},
        {"rts","001110", "00000"},
        {"stop","001111", "00000"}
};

/**
 * Checks if a symbol is valid, meaning it starts with a letter and contains only alphanumeric characters.
 *
 * @param operand A string representing the symbol to check
 * @return int TRUE if the symbol is valid, FALSE otherwise
 */
static int is_valid_symbol(char* operand) {
    int i;
    if(!isalpha(operand[0])) {
        return FALSE;
    }
    for(i=1; operand[i] != '\0'; i++) {
        if(!isalnum(operand[i])) {
            return FALSE;
        }
    }
    return TRUE;
}

AddressingCase get_operand_addressing_mode(char* operand) {
    if(operand[0] == '#') {
        return IMMEDIATE_ADDRESSING;
    }
    else if(operand[0] == '&') {
        if(is_valid_symbol(operand + 1) == TRUE) {
            return RELATIVE_ADDRESSING;
        }
        return INVALID_ADDRESSING;
    }
    else if(is_register(operand) == TRUE) {
        return REGISTER_DIRECT_ADDRESSING;
    }

    else if(is_valid_symbol(operand) == TRUE) {
        return DIRECT_ADDRESSING;
    }
    else {
        return INVALID_ADDRESSING;
    }
}

int number_of_operands (char *command) {
    if(strcmp(command, "mov") == 0 || strcmp(command, "cmp") == 0
       || strcmp(command, "add") == 0 || strcmp(command, "sub") == 0
       || strcmp(command, "lea") == 0 ) {
        return 2;
    }
    else if(strcmp(command, "clr") == 0 || strcmp(command, "not") == 0
            || strcmp(command, "inc") == 0 || strcmp(command, "dec") == 0
            || strcmp(command, "jmp") == 0 || strcmp(command, "bne") == 0
            || strcmp(command, "jsr") == 0 || strcmp(command, "red") == 0
            || strcmp(command, "prn") == 0 ) {
        return 1;
    }
    else {
        return 0;
    }
}

int number_of_words(char* line, char* command) {
    char *first_operator;
    char *second_operator;

    /* Every instruction requires at least one word */
    int counter_of_words = 1;

    /* Process commands with either one or two operands */
    if(number_of_operands(command) == 2 || number_of_operands(command) == 1) {

        /* Extract the first operand from the line */
        first_operator = strtok(line, " ,");

        /* If the first (or the single) operand is not a register, it requires an extra word */
        if (get_operand_addressing_mode(first_operator) != REGISTER_DIRECT_ADDRESSING) {
            counter_of_words++;
        }

        /* If the command has two operands, process the second one */
        if (number_of_operands(command) == 2) {

            /* Extract the second operand from the line */
            second_operator = strtok(NULL, " \n\t");

            /* If the second operand is not a register, it requires an extra word */
            if (get_operand_addressing_mode(second_operator) != REGISTER_DIRECT_ADDRESSING) {
                counter_of_words++;
            }
        }
    }
    return counter_of_words;
}

void to_binary(char* array, int number, int number_of_bits) {
    int i;

    /* Process each bit position from most significant to least significant */
    for(i=number_of_bits; i>=0; i--) {

        /* Check if the bit at position i is 1 */
        if(((number>>i) & 1) == 1) {

            /* If the bit is 1, set the corresponding position in the array to '1' */
            array[number_of_bits-i] = '1';
        }
        /* If the bit is 0, the position remains '0' */
    }
}

char* create_first_word(char* line, int is_symbol) {
    char *src_operand;
    char *dest_operand;
    int addressing_mode_src;
    int adressing_mode_dest;

    char *command;

    char *opcode;
    char *src_addresing = "00";
    char *src_register = "000";
    char *dest_adrresing = "00";
    char *dest_register = "000";
    char *funct;
    char *A = "1";
    char *R = "0";
    char *E = "0";
    int i;

    char* result = (char*) malloc((WORD_SIZE + 1) * sizeof(char));
    if(check_memory_allocation(result) == FALSE) {
        exit(1);
    }


    /* If the line contains a symbol, skip it and process the command */
    if (is_symbol == TRUE) {
        line = strtok(line, ":");
        line = strtok(NULL, "");
    }

    /* Extract the command from the line */
    command =strtok(line, " \n\t");

    /* Iterate over the commands table */
    for(i = 0; i<= 15; i++) {

        /* Find the command's opcode and function code in the table */
        if(strcmp(command, commands_table[i].command_name) == 0) {
            opcode = commands_table[i].command_binary_code;
            funct = commands_table[i].funct_binary_code;
        }
    }

    /* If the command has two operands, process the source operand */
    if (number_of_operands(command) == 2) {

        /* Extract the source operand */
        src_operand = strtok(NULL, " ,\t");

        /* Determine its addressing mode */
        addressing_mode_src = get_operand_addressing_mode(src_operand);

        /* Iterate over the addresing mode table */
        for(i = 0; i<= 3; i++) {

            /* Find the binary representation of the addressing mode */
            if(addressing_mode_src == addresing_mode_table[i].addresing_code) {
                src_addresing = addresing_mode_table[i].addresing_binary_code;
            }
        }

        /* If the operand is a register, get its binary code */
        if(is_register(src_operand) == TRUE) {

            /* Iterate over the register name table */
            for(i=0; i<=7; i++) {

                /* Find the binary representation of the register name */
                if(strcmp(src_operand, register_name_table[i].register_name) == 0) {
                    src_register = register_name_table[i].register_binary_code;
                }
            }
        }

        /* Default to "000" if not a register */
        else {
            src_register = "000";
        }
    }

    if(number_of_operands(command) == 2 || number_of_operands(command) == 1) {

        /* Extract the destination operand */
        dest_operand = strtok(NULL, " ,\n\t");

        /* Determine its addressing mode */
        adressing_mode_dest = get_operand_addressing_mode(dest_operand);

        /* Iterate over the addresing mode table */
        for(i = 0; i<= 3; i++) {

            /* Find the binary representation of the addressing mode */
            if(adressing_mode_dest == addresing_mode_table[i].addresing_code) {
                dest_adrresing = addresing_mode_table[i].addresing_binary_code;
            }
        }

        /* If the operand is a register, get its binary code */
        if(is_register(dest_operand) == TRUE) {

            /* Iterate over the register name table */
            for(i=0; i<=7; i++) {

                /* Find the binary representation of the register name */
                if(strcmp(dest_operand, register_name_table[i].register_name) == 0) {
                    dest_register = register_name_table[i].register_binary_code;
                }
            }
        }

        /* Default to "000" if not a register */
        else {
            dest_register = "000";
        }
    }

    /* Concatenates all the binary components to the result string. */
    result[0] = '\0';
    strcat(result, opcode);
    strcat(result, src_addresing);
    strcat(result, src_register);
    strcat(result, dest_adrresing);
    strcat(result, dest_register);
    strcat(result, funct);
    strcat(result, A);
    strcat(result, R);
    strcat(result, E);

    return result;
}

char* create_extra_word(char* operator, AssemblyElementType type) {
    char* result = (char*) malloc((WORD_SIZE + 1) * sizeof(char));
    int addressing_mode;
    int number;
    int i;

    if(check_memory_allocation(result) == FALSE) {
        exit(1);
    }

    /* Initialize the binary extra word with all zeros */
    for(i = 0; i <= WORD_SIZE; i++) {
        result[i] = '0';
    }
    result[WORD_SIZE] = '\0';

    /* Handle STRING and DATA types */
    if(type == STRING || type == DATA) {
        if(type == STRING) {

            /* Convert character to its ASCII value */
            number = (int)operator[0];
        }
        else {

            /* Convert string to integer */
            number = atoi(operator);
        }

        /* Convert the number to binary */
        to_binary(result, number, BINARY_BITS - 1);
        return result;

    }
    else {
        addressing_mode = get_operand_addressing_mode(operator);

        /* Handle immediate addressing mode */
        if(addressing_mode == IMMEDIATE_ADDRESSING) {
            result[BINARY_BITS - 1] = '0';  /* Set E bit to 0 */
            result[BINARY_BITS - 2] = '0';  /* Set R bit to 0 */
            result[BINARY_BITS - 3] = '1';  /* Set A bit to 1 */

            /* Skip the '#' and convert to integer */
            number = atoi(operator + 1);

            /* Convert the number to binary */
            to_binary(result, number, BINARY_BITS - 4);
            return result;
        }

        /* Handle direct addressing or relative addressing modes */
        if(addressing_mode == DIRECT_ADDRESSING || addressing_mode == RELATIVE_ADDRESSING) {

            /* Free the allocated memory */
            free(result);
            result = (char*) malloc(2 * sizeof(char));
            if(check_memory_allocation(result) == FALSE) {
                exit(1);
            }
            /* Placeholder, the word will be completed in second pass */
            result[0] = '?';
            result[1] = '\0';
            return result;
        }
    }
    return result;
}

char* build_word_second_pass(int address, AddressingCase addressing_mode, int external_flag) {
    char* result = (char*) malloc((WORD_SIZE + 1) * sizeof(char));
    int i;

    if(check_memory_allocation(result) == FALSE) {
        exit(1);
    }

    /* Initialize the binary word with all zeros */
    for(i = 0; i <= WORD_SIZE; i++) {
        result[i] = '0';
    }
    result[WORD_SIZE] = '\0';

    /* Handle direct addressing mode */
    if(addressing_mode == DIRECT_ADDRESSING) {
        if(external_flag == TRUE) {
            result[23] = '1';  /* Set E bit to 1 */
            result[22] = '0';  /* Set R bit to 0 */
            result[21] = '0';  /* Set A bit to 0 */
        }
        else {
            result[23] = '0';  /* Set E bit to 0 */
            result[22] = '1';  /* Set R bit to 1 */
            result[21] = '0';  /* Set A bit to 0 */
        }
        to_binary(result, address, BINARY_BITS - 4);
    }

    /* Handle relative addressing mode */
    if(addressing_mode == RELATIVE_ADDRESSING) {
        result[23] = '0';  /* Set E bit to 0 */
        result[22] = '0';  /* Set R bit to 0 */
        result[21] = '1';  /* Set A bit to 1 */

        to_binary(result, address, BINARY_BITS - 4);
    }

    return result;
}

/**
* Processes a single operand to create its missing machine code word and add it to the line's machine code.
* This function is a helper for `create_missing_word_second_pass`
*
* @line Pointer to the assembly line list node
* @operand Pointer to the string representing the operand to process
* @head_of_symbol_table Pointer to the head of the symbol table
* @return void
*/
static void process_operand(AssemblyLineList *line, char *operand, SymbolNode *head_of_symbol_table) {
    AddressingCase addressing_mode;
    int address;
    int external_flag;
    char *missing_word;

    /* Get the addressing mode of the first operand */
    addressing_mode = get_operand_addressing_mode(operand);

    /* Handle direct addressing mode for first operand */
    if(addressing_mode == DIRECT_ADDRESSING) {

        /* Get the address of the symbol from the symbol table */
        address = get_symbol_address(operand, head_of_symbol_table);

        /* Check if the symbol is external */
        external_flag = is_external(operand, head_of_symbol_table);

        /* Build the additional word with the symbol's address */
        missing_word = build_word_second_pass(address, addressing_mode, external_flag);

        /* Add the missing word to the line's machine code */
        add_missing_word(line->code, missing_word);
    }

    /* Handle relative addressing mode for first operand */
    if(addressing_mode == RELATIVE_ADDRESSING) {

        /* Get the address of the current line */
        int addresss_of_line = line->code->address;

        /* Calculate the relative address (+1 for pass the '@' character) */
        address = get_relative_address(operand + 1, addresss_of_line, head_of_symbol_table);

        /* Build the additional word with the relative address */
        missing_word = build_word_second_pass(address, addressing_mode, FALSE);

        /* Add the missing word to the line's machine code */
        add_missing_word(line->code, missing_word);
    }
}

void create_missing_word_second_pass(AssemblyLineList *line, SymbolNode *head_of_symbol_table) {
    char *string_line = line->line;
    char *command;
    char *first_operand;
    char *second_operand;

    char *line_copy = (char *)malloc(strlen(string_line) + 1);
    if(check_memory_allocation(line_copy) == FALSE) {
        exit(1);
    }

    strcpy(line_copy, string_line);

    /* Check if the line starts with a symbol (label) */
    if (is_symbol(line_copy) == TRUE) {

        /* Extract the command after the label */
        strtok(line_copy, ":");
        command = strtok(NULL, " \n\t");
    } else {

        /* Extract the command from the beginning of the line */
        command = strtok(line_copy, " \n\t");

    }

    /* Process commands that have 1 or 2 operands */
    if(number_of_operands(command) == 1 || number_of_operands(command) == 2) {

        /* Extract the first operand */
        first_operand = strtok(NULL, " ,\n\t");
        process_operand(line, first_operand, head_of_symbol_table);
    }

    /* Process the second operand if the command has 2 operands */
    if(number_of_operands(command) == 2) {

        /* Extract the second operand */
        second_operand = strtok(NULL, " \n\t");
        process_operand(line, second_operand, head_of_symbol_table);
    }
    free(line_copy);
}
