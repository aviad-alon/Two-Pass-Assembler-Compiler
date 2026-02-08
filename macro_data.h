#ifndef MACRO_DATA_H
#define MACRO_DATA_H

#include <stdio.h>

#define TRUE 1 /* Boolean representation of TRUE */
#define FALSE 0 /* Boolean representation of FALSE */
#define ERROR_FOUND 1 /* Indicates that an error was found */
#define ERROR_WAS_NOT_FOUND 0 /* Indicates that no errors were found */
#define MAX_LINE 81 /* Maximum line length */
#define MAX_MACRO_NAME 31 /* Maximum macro name length */

/**
 * Represents the state of macro processing.
 */
typedef enum {
    MACRO_OUTSIDE = 0, /* Not inside a macro definition */
    MACRO_INSIDE = 1 /* Inside a macro definition */
} MacroState;

/**
 * Represents a single line of macro content.
 */
typedef struct LineNode {
    char *line; /* A line of macro content */
    struct LineNode *next; /* Pointer to the next line in the list */
}LineNode ;

/**
 * Represents a macro definition in the macro table.
 */
typedef struct MacroNode {
    char *data; /* Name of the macro */
    struct MacroNode *next; /* Pointer to the next macro in the list */
    LineNode *content_of_macro; /* Linked list containing the macro content */
} MacroNode;

/**
 * Adds a new macro name to the macro table (linked list).
 *
 * @param head Pointer to the head of the macro table linked list
 * @param macro_name The name of the macro to be added
 * @return void
 *
 */
void add_macro_name(MacroNode **head, char *macro_name);

/**
 * Adds a new line of content to a macro definition in the macro table.
 *
 * @param head Pointer to the head of the macro table linked list
 * @param macro_name The name of the macro to which the content will be added
 * @param new_line_to_add The line of text to append to the macro definition
 * @return void
 */
void add_macro_content(MacroNode *head, char *macro_name, char *new_line_to_add);

/**
 * Combines all lines of a macro (from a linked list) into a single string.
 * @param head Pointer to the head of the macro list.
 * @param macro_name The name of the macro whose content to combine.
 * @return A dynamically allocated string containing all the lines of the macro.
 */
char *str_combined(MacroNode *head, char* command);

/**
 * Frees all memory allocated for the macro linked list, including its content.
 *
 * @param head Pointer to the first node of the macro linked list
 * @return void
 */
void free_macro_nodes(MacroNode *head);

/**
 * Checks if memory allocation was successful
 *
 * @param ptr Pointer to the allocated memory
 * @return int TRUE if memory allocation succeeded, FALSE otherwise
 */
int check_memory_allocation(void *ptr);

/**
 * Checks if a file was opened successfully.
 *
 * @param file Pointer to the file to check
 * @param file_name: Name of the file
 * @return int TRUE if the file was opened successfully, FALSE otherwise
 */
int check_file_open(FILE *file, char *file_name);

/**
 * Checks for preprocessing errors in a file by validating macro definitions
 * and terminations.
 *
 * @param fd Pointer to the file being checked
 * @return int ERROR_FOUND if any preprocessing errors are found, otherwise ERROR_WAS_NOT_FOUND.
 */
int check_preprocessing_errors(FILE *fd);

/**
 * This function reads an assembly file line by line, expands macros,
 * and writes the expanded content to an output assembly macro (am) file.
 *
 * @param fd Pointer to the assembly file
 * @param output_filename Name of the output file to store the expanded content
 * @return Pointer to the head of the macro table linked list
 */
MacroNode * file_preprocessing(FILE *fd, char *output_filename);
#endif
