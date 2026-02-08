#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <ctype.h>
#include "macro_data.h"

int check_file_open(FILE *file, char *file_name) {
    if (file == NULL) {
        printf("Error opening file: %s\n", file_name);
        return FALSE;
    }
    return TRUE;
}

/**
 * Checks if a given line is empty or a comment line.
 *
 * @param line Pointer to the string representing the line to check
 * @return int TRUE if the line is empty or a comment, otherwise FALSE
 */
static int is_empty_or_comment(char *line) {
    int i = 0;

    /* Skip leading spaces in the line */
    while (isspace(line[i])) i++;

    /* Check if the line is empty or starts with a comment character */
    if (line[i] == '\0' || line[i] == ';') {
        return TRUE;
    }
    return FALSE;
}

/**
 * Checks if a macro name is valid.
 * A valid macro name must not match an assembly command, a directive, or a register name.
 * Additionally, checks if the macro name is within the length limit and only contains valid characters.
 *
 * @param macro_name Pointer to the string representing the macro name
 * @param number_of_line The line number where the macro is defined
 * @return int ERROR_FOUND if the macro name is invalid, otherwise ERROR_WAS_NOT_FOUND.
 */
static int check_macro_name(char *macro_name, int number_of_line) {
    int error_flag = ERROR_WAS_NOT_FOUND;
    int i;

    /* List of assembly commands */
    char *commands[] = {"mov", "cmp", "add", "sub", "lea", "clr", "not",
                        "inc", "dec", "jmp", "bne", "jsr", "red", "prn",
                        "rts", "stop"};
    int number_of_commands = 16;


    /* Check if a macro name is longer than 31 characters */
    if(strlen(macro_name) > MAX_MACRO_NAME) {
        printf("Error in line %d: A macro name cannot be longer than 31 characters.\n", number_of_line);
        error_flag = ERROR_FOUND;
    }

    /* Check if a macro name contains only letters, digits, or underscores. */
    for(i=0; i < strlen(macro_name); i++) {
        if (!(isalnum(macro_name[i]) || macro_name[i] == '_')) {
            printf("Error in line %d: A macro name can only contain letters, digits, and underscores.\n", number_of_line);
            error_flag = ERROR_FOUND;
        }
    }

    /* Check if the macro name matches an assembly command */
    for(i=0; i < number_of_commands; i++) {
        if(strcmp(macro_name, commands[i]) == 0) {
            printf("Error in line %d: A macro name cannot be the same as a command name.\n", number_of_line);
            error_flag = ERROR_FOUND;
        }
    }

    /* Check if the macro name matches a directive name */
    if(strcmp(macro_name, "entry") == 0 || strcmp(macro_name, "entry") == 0 || strcmp(macro_name, "extern") == 0 || strcmp(macro_name, "data") == 0) {
        printf("Error in line %d: A macro name cannot be the same as a directive name.\n", number_of_line);
        error_flag = ERROR_FOUND;
    }

    /* Check if the macro name matches a register name */
    if(strcmp(macro_name, "r0") == 0 || strcmp(macro_name, "r1") == 0 || strcmp(macro_name, "r2") == 0 ||
       strcmp(macro_name, "r3") == 0 || strcmp(macro_name, "r4") == 0 || strcmp(macro_name, "r5") == 0 ||
       strcmp(macro_name, "r6") == 0 || strcmp(macro_name, "r7") == 0 ) {
        printf("Error in Line %d: A macro name cannot be a reserved assembly keyword.\n", number_of_line);
        error_flag = ERROR_FOUND;
    }
    return error_flag;
}

/**
 * Checks if a macro definition line is correctly formatted by ensuring
 * there are no extra characters and the line ends with only spaces or a newline.
 *
 * @param line Pointer to the string representing the macro definition line
 * @param number_of_line The line number where the macro definition is defined
 * @return int ERROR_FOUND if the line contains extra characters, otherwise ERROR_WAS_NOT_FOUND.
 */
static int check_macro_definition_line(const char *line, int number_of_line) {
    int error_flag = ERROR_WAS_NOT_FOUND;
    int i = 0;

    /* Skip spaces in the beginning of the line */
    while (isspace(line[i])) i++;

    /* Skip the word "mcro" */
    while (!isspace(line[i]) && line[i] != '\0') i++;

    /* Skip spaces */
    while (isspace(line[i])) i++;

    /* Skip macro name */
    while (!isspace(line[i]) && line[i] != '\0') i++;

    /* Check for extraneous text */
    while (line[i] != '\0') {
        if (!isspace(line[i])) {
            printf("Error in line %d: The macro definition and termination lines must not contain extra characters.\n", number_of_line);
            error_flag = ERROR_FOUND;
            return error_flag;
        }
        i++;
    }
    return error_flag;
}

/**
 * Checks if a macro termination line is correctly formatted by ensuring
 * there are no extra characters and the line ends with only spaces or a newline.
 *
 * @param line Pointer to the string representing the macro termination line
 * @param number_of_line The line number where the macro termination is defined
 * @return int ERROR_FOUND if the line contains extra characters, otherwise ERROR_WAS_NOT_FOUND.
 */
static int check_macro_termination_line(char *line, int number_of_line) {
    int error_flag = ERROR_WAS_NOT_FOUND;
    int i=0;

    /* Skip spaces in the beginning of the line */
    while (isspace(line[i])) i++;

    /* Skip the word "mcroend" */
    while (!isspace(line[i]) && line[i] != '\0') i++;

    /* Check for extraneous text */
    while (line[i] != '\0') {
        if (!isspace(line[i])) {
            printf("Error in line %d: The macro definition and termination lines must not contain extra characters.\n", number_of_line);
            error_flag = ERROR_FOUND;
            return error_flag;
        }
        i++;
    }
    return error_flag;
}

/**
 * Checks if a macro with a given name exists in the macro linked list.
 *
 * @param head Pointer to the head of the macro linked list
 * @param macro_name The name of the macro to search for
 * @return int TRUE if the macro exists, otherwise FALSE.
 */
static int is_macro_exist(MacroNode *head, char *macro_name) {
    /* Traverse the macro linked list */
    while (head != NULL) {

        /* Compare the current macro name with the target macro name */
        if (strcmp(head->data, macro_name) == 0) {

            /* The macro is found */
            return TRUE;
        }

        /* Move to the next macro node */
        head = head->next;
    }

    /* The macro is not found */
    return FALSE;
}

int check_preprocessing_errors(FILE *fd) {
    char line[MAX_LINE] = {0};
    char copy_line[MAX_LINE] = {0};
    char macro_name[MAX_LINE] = {0};
    char command[MAX_LINE] = {0};

    int number_of_line = 0;

    int error_flag = ERROR_WAS_NOT_FOUND;

    /* Read the file line by line */
    while (fgets(line, sizeof(line), fd)) {
        number_of_line++;
        /* If the current line is not a comment line or empty line */
        if(!is_empty_or_comment(line)) {
            strcpy(copy_line, line);
            strcpy(command,strtok(copy_line, " :,\n\t"));

            /* Check if the line defines a macro */
            if (strcmp(command, "mcro") == 0) {

                /* Extract the macro name */
                strcpy(macro_name, strtok(NULL, "\n"));

                /* Validate the macro name and definition format */
                if ((check_macro_name(macro_name, number_of_line) == 1) ||
                    check_macro_definition_line(line, number_of_line) == 1) {
                    error_flag = ERROR_FOUND;
                }
            }

            /* Check if the line marks the end of a macro */
            if (strcmp(command, "mcroend") == 0) {

                /* Validate the termination line format */
                if (check_macro_termination_line(line, number_of_line) == 1) {
                    error_flag = ERROR_FOUND;
                }
            }
        }
    }
    /* Return whether an error was found */
    return error_flag;
}

MacroNode * file_preprocessing(FILE *fd, char *output_filename) {
    MacroNode *head_of_macro_table = NULL;
    MacroState macro_state = MACRO_OUTSIDE; /* Flag to indicate if we are inside a macro definition */

    char line[MAX_LINE] = {0};
    char copy_line[MAX_LINE] = {0};
    char macro_name[MAX_LINE] = {0};
    char command[MAX_LINE] = {0};

    /* Open the output file for writing */
    FILE *output_file = fopen(output_filename, "w");
    if (check_file_open(output_file, output_filename) == FALSE) {
        exit(1);
    }

    /* Read the file line by line */
    while (fgets(line, sizeof(line), fd)) {
        strcpy(copy_line, line);

        /* If the current line is not a comment line or empty line */
        if(!is_empty_or_comment(line)) {

            /* If not currently inside a macro definition */
            if (macro_state == MACRO_OUTSIDE)  {

                /* Extract the first word of the line */
                strcpy(command, strtok(copy_line, " \n\t"));

                /* Check if the line contains a previously defined macro */
                if (is_macro_exist(head_of_macro_table, command) == 1) {
                    char *combined;

                    /* Get the macro's content */
                    combined = str_combined(head_of_macro_table, command);

                    /* Write the macro's content to the output file */
                    fputs(combined, output_file);
                    free(combined);

                } else {
                    /* Check if the current line starts a macro definition */
                    if (strcmp(command, "mcro") == 0) {

                        /* Change state to inside macro */
                        macro_state = MACRO_INSIDE;

                        /* Extract the macro name */
                        strcpy(macro_name, strtok(NULL, "\n"));

                        /* Add the macro name to the macro table */
                        add_macro_name(&head_of_macro_table, macro_name);
                    } else {

                        /* Write non-macro lines to output file */
                        fputs(line, output_file);
                    }
                }
            } else {  /* If currently inside a macro definition */
                strcpy(command, strtok(copy_line, " \n\t"));

                /* Check if the current line ends a macro definition */
                if (strcmp(command, "mcroend") == 0) {

                    /* Change state to outside macro */
                    macro_state = MACRO_OUTSIDE;
                } else {

                    /* Add the current line to the macro definition */
                    add_macro_content(head_of_macro_table, macro_name, line);
                }
            }
        }
    }

    /* Close the output file */
    fclose(output_file);
    return head_of_macro_table;
}
