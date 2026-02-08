#include <stdlib.h>
#include <string.h>
#include <stddef.h>
#include "first_second_pass_data.h"

void generic_insert_node(void **head, void *new_node, size_t next_offset) {
    void *current = *head;
    void **next_ptr;

    /* If the list is empty, the new node becomes the head */
    if (*head == NULL) {
        *head = new_node;
        return;
    }
    /* Traverse to the end of the list */
    while ((next_ptr = (void **)((char *)current + next_offset)) && *next_ptr != NULL) {
        current = *next_ptr;
    }

    /* Insert the new node at the end of the linked list */
    *next_ptr = new_node;
}


void insert_word(BinaryMachineCode **head, char* word, int address) {
    BinaryMachineCode *new_node;

    /* Allocate memory for the new node */
    new_node = (BinaryMachineCode *) malloc(sizeof(BinaryMachineCode));

    /* Check if memory allocation was successful */
    if (check_memory_allocation(new_node) == FALSE) {
        exit(1);
    }

    /* Allocate memory for the binary machine code and copy it */
    new_node->word =word;
    new_node->address = address; /* Copy the address of the word */
    new_node->next = NULL; /* Initialize next pointer to NULL */

    generic_insert_node((void **)head, (void *)new_node, offsetof(BinaryMachineCode, next));

}

void insert_line(AssemblyLineList **head, int number_of_words, char* line, AssemblyElementType type, BinaryMachineCode *code) {
    AssemblyLineList *new_node;
    AssemblyLineList *current;

    /* Allocate memory for the new node */
    new_node = (AssemblyLineList *) malloc(sizeof(AssemblyLineList));

    /* Check if memory allocation was successful */
    if (check_memory_allocation(new_node) == FALSE) {
        exit(1);
    }

    /* Allocate memory for the line and copy it */
    new_node->line = (char *) malloc(strlen(line) + 1);
    if (check_memory_allocation(new_node->line) == FALSE) {
        free(new_node);
        exit(1);
    }

    strcpy(new_node->line, line); /* Copy the line string */
    new_node->number_of_words = number_of_words; /* Set the number of words field */
    new_node->type = type; /* Set the type of the line (CODE, DATA, ENTRY, EXTERN) */
    new_node->code = code; /* Copy the binary machine code */
    new_node->next = NULL; /* Initialize next pointer to NULL */
    new_node->prev = NULL; /* Initialize prev pointer to NULL */

    /* If the list is empty, insert as the head */
    if (*head == NULL) {
        *head = new_node;
    } else {
        current = *head;

        /* Traverse to the end of the list */
        while (current->next != NULL) {
            current = current->next;
        }

        /* Link the new node at the end */
        current->next = new_node;
        new_node->prev = current;
    }
}

void insert_to_symbol_table(SymbolNode **head, char* symbol, AssemblyElementType type, int address) {
    SymbolNode *new_node;

    /* Allocate memory for the new node */
    new_node = (SymbolNode *) malloc(sizeof(SymbolNode));

    /* Check if memory allocation was successful */
    if (check_memory_allocation(new_node) == FALSE) {
        exit(1);
    }

    /* Allocate memory for the symbol name and copy it */
    new_node->symbol_name = (char *)malloc(strlen(symbol) + 1);
    if (check_memory_allocation(new_node->symbol_name) == FALSE) {
        free(new_node);
        exit(1);
    }
    strcpy(new_node->symbol_name, symbol); /* Copy the symbol name string */
    new_node->type = type; /* Set the type of the symbol */
    new_node->address = address; /* Copy the address of the symbol */
    new_node->entry_flag = 0; /* Initialize the entry flag to 0 */
    new_node->next = NULL;  /* Initialize next pointer to NULL */

    /* Insert the new node at the end of the list using the generic insert function */
    generic_insert_node((void**)head, (void*)new_node, offsetof(SymbolNode, next));
}

void update_data_symbol_addresses(SymbolNode *head_of_symbol_table, int ICF) {
    SymbolNode *current_line = head_of_symbol_table;

    /* Traverse the symbol table */
    while (current_line != NULL) {

        /* Update only DATA symbols */
        if(current_line->type == DATA) {
            current_line->address += ICF;
        }

        /* Move to the next node */
        current_line = current_line->next;
    }
}


void update_data_line_list_addresses(AssemblyLineList *head_of_line_list, int ICF) {
    AssemblyLineList *current_line = head_of_line_list;
    /* Traverse the line list */
    while (current_line != NULL) {

        /* Check if the current line is of type DATA */
        if (current_line->type == DATA) {

            /* Get the first binary word of the line */
            BinaryMachineCode *current_word = (current_line->code);

            /* Traverse through all binary words in the line and update addresses */
            while (current_word != NULL) {
                current_word->address += ICF;
                /* Move to the next binary word */
                current_word = current_word->next;
            }
        }

        /* Move to the next line */
        current_line = current_line->next;
    }
}

void free_symbol_table(SymbolNode *head) {
    SymbolNode *current = head;
    SymbolNode *next_node;

    while (current != NULL) {

        /* Frees the symbol's name */
        free(current->symbol_name);

        next_node = current->next;

        /* Frees the symbol node itself */
        free(current);

        /* Move to the next symbol node */
        current = next_node;
    }
}


void free_line_list(AssemblyLineList *head) {
    AssemblyLineList *current_line = head;
    AssemblyLineList *next_line;

    while (current_line != NULL) {
        BinaryMachineCode *current_code = current_line->code;
        BinaryMachineCode *next_code;

        next_line = current_line->next;

        if (current_line->line != NULL) {
            free(current_line->line);
        }

        while (current_code != NULL) {
            next_code = current_code->next;

            if (current_code->word != NULL) {
                free(current_code->word);
            }

            free(current_code);
            current_code = next_code;
        }

        free(current_line);
        current_line = next_line;
    }
}

void free_machine_code_list(BinaryMachineCode *array, int ICF, int DCF) {
    size_t num_elements = (ICF - 100) + DCF;
    size_t i;

    for (i = 0; i < num_elements; i++) {
        /* Free the string allocated for each element's 'word' */
        free(array[i].word);
    }

    /* Free the entire array of BinaryMachineCode structures */
    free(array);
}



