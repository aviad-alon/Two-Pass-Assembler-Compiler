#include <stdlib.h>
#include <string.h>
#include <stddef.h>
#include "macro_data.h"
#include "first_second_pass_data.h"

int check_memory_allocation(void *ptr) {
    if (ptr == NULL) {
        printf("Memory allocation failed!\n");
        return FALSE;
    }
    return TRUE;
}

void add_macro_name(MacroNode **mcro_list_head, char *macro_name) {
    MacroNode *new_node;

    new_node = (MacroNode *)malloc(sizeof(MacroNode));

    /* Check if memory allocation was successful */
    if(check_memory_allocation(new_node) == FALSE) {
        exit(1);
    }

    /* Allocate memory for the macro name and copy the content manually */
    new_node->data = (char *)malloc(strlen(macro_name) + 1);
    if(check_memory_allocation(new_node->data) == FALSE) {
        free(new_node);
        exit(1);
    }

    /* Initialize the new macro node */
    strcpy(new_node->data, macro_name);
    new_node->content_of_macro = NULL;
    new_node->next = NULL;

    /* Insert the new node into the linked list */
    generic_insert_node((void**)mcro_list_head, (void*)new_node, offsetof(MacroNode, next));
}

void add_macro_content(MacroNode *macro_list_head, char *macro_name, char *new_line_to_add) {
    LineNode *new_node;
    MacroNode *temp = macro_list_head;

    new_node = (LineNode *)malloc(sizeof(LineNode));

    /* Check if memory allocation was successful */
    if(check_memory_allocation(new_node) == FALSE) {
        exit(1);
    }


    /* Allocate memory for the new line and copy the content manually */
    new_node->line = (char *)malloc(strlen(new_line_to_add) + 1);
    if(check_memory_allocation(new_node->line) == FALSE) {
        free(new_node);
        exit(1);
    }

    /* Initialize the new macro node */
    strcpy(new_node->line, new_line_to_add);
    new_node->next = NULL;

    /* Finds the macro by name to add content to it */
    while (temp != NULL && strcmp(temp->data, macro_name) != 0) {
        temp = temp->next;
    }

    /* Insert the new line node into the macro's content list */
    generic_insert_node((void**)&temp->content_of_macro, (void*)new_node, offsetof(LineNode, next));

}

char *str_combined(MacroNode *macro_list_head, char* macro_name) {
    LineNode *line;
    LineNode *temp;
    size_t total_length = 0;
    char *result;


    /* Searches for the macro in the linked list by name */
    while (strcmp(macro_list_head->data, macro_name) != 0) {
        macro_list_head = macro_list_head->next;
    }

    /*Points to the start of the macro's content (lines) */
    line = macro_list_head->content_of_macro;
    temp = line;

    /* Calculates the total length of all the macro lines (including the newline character for each line) */
    while (temp != NULL) {
        total_length += strlen(temp->line) +1; /* +1 for the newline character '\n' */
        temp = temp->next;
    }

    result = (char *)malloc(total_length + 1);
    /* Check if memory allocation was successful */
    if(check_memory_allocation(result) == FALSE) {
        exit(1);
    }
    result[0] = '\0';

    /* Concatenates each line of the macro content into the result string */
    temp = macro_list_head->content_of_macro;
    while (temp != NULL) {
        strcat(result, temp->line);
        temp = temp->next;
    }
    return result;
}

void free_macro_nodes(MacroNode *macro_list_head) {
    while (macro_list_head != NULL) {
        MacroNode *next_macro;
        LineNode *current_line_node = macro_list_head->content_of_macro;
        /* Frees the macro's name */
        free(macro_list_head->data);

        /* Free all lines in the macro's content */
        while (current_line_node != NULL)
        {
            LineNode *next_line = current_line_node->next;

            /* Frees the content of the line */
            free(current_line_node->line);

            /* Frees the line node itself */
            free(current_line_node);

            /* Move to the next line */
            current_line_node = next_line;

        }
        next_macro = macro_list_head->next;

        /* Frees the macro node itself */
        free(macro_list_head);
        /* Move to the next macro */
        macro_list_head = next_macro;
    }
}
