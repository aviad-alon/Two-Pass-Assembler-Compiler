#ifndef FIRST_SECOND_PASS_DATA_H
#define FIRST_SECOND_PASS_DATA_H

#include "macro_data.h"

/**
 * Enum representing the type of assembly elements
 */
typedef enum {
    DATA,         /* .data directive */
    STRING,       /* .string directive */
    ENTRY,        /* .entry directive */
    EXTERN,       /* .extern directive */
    INSTRUCTION,  /* instruction command */
    CODE          /* machine code instruction */
} AssemblyElementType;

/**
 * Enum representing different addressing modes.
 */
typedef enum {
    IMMEDIATE_ADDRESSING,
    DIRECT_ADDRESSING,
    RELATIVE_ADDRESSING,
    REGISTER_DIRECT_ADDRESSING,
    INVALID_ADDRESSING
} AddressingCase;

/**
 * Struct representing a symbol (label) in the symbol table.
 */
typedef struct SymbolNode {
    char *symbol_name; /* Name of the symbol */
    int address; /* the address of the symbol */
    AssemblyElementType type; /* The type of the symbol */
    int entry_flag; /* Flag indicating if the symbol is an entry */
    struct SymbolNode *next; /* Pointer to the next symbol in the table */
} SymbolNode ;

/**
 * Struct representing a binary machine code word.
 */
typedef struct BinaryMachineCode {
    struct BinaryMachineCode *next; /* Pointer to the next machine code in the list */
    char* word; /* The binary word representing the machine code */
    int address; /* The address of the machine word */
} BinaryMachineCode;

/**
 * Struct representing a line of assembly code and its associated machine code.
 */
typedef struct AssemblyLineList{
    char *line; /*The assembly line */
    int number_of_words; /* The number of machine code words created from this assembly line */
    AssemblyElementType type; /* The type ot the assembly line */
    BinaryMachineCode *code; /* The binary machine codes of the line */
    struct AssemblyLineList *next; /* Pointer to the next assembly line */
    struct AssemblyLineList *prev; /* Pointer to the previous assembly line */
} AssemblyLineList;

/**
 * Struct holding the results from the first pass of the assembler.
 */
typedef struct {
    AssemblyLineList *head_of_lines_list;
    SymbolNode *head_of_symbol_table;
    int ICF;
    int DCF;
} FirstPassResult;

/**
 * Struct holding the results from the second pass of the assembler.
 */
typedef struct {
    AssemblyLineList *head_of_lines_list;
    SymbolNode *head_of_symbol_table;
} SecondPassResult;


/**
 * Inserts a new node at the end of a generic linked list,
 * Uses memory offset to find the next pointer field in different node structures.
 *
 * @param head Double pointer to the head of the linked list
 * @param new_node Pointer to the new node to be inserted
 * @param next_offset Byte offset of the "next" pointer within the node structure
 * @return void
 */
void generic_insert_node(void **head, void *new_node, size_t node_offset);

/**
 * Inserts a new binary word node at the end of a machine code linked list.
 *
 * @param head Double pointer to the head of the binary machine code linked list
 * @param word A string representing the binary word to insert
 * @param address The address associated with this word
 * @return NULL
 */
void insert_word(BinaryMachineCode **head, char* word, int address);

/**
 * Inserts a new assembly line node at the end of a doubly-linked list.
 *
 * @param head Double pointer to the head of the assembly line list
 * @param number_of_words Number of machine words this assembly line generates
 * @param line A string representing the line
 * @param type The type of the line (CODE, DATA, ENTRY, EXTERN)
 * @param code Pointer to binary machine code representation of this line
 * @return void
 */
void insert_line(AssemblyLineList **head, int number_of_words, char* line, AssemblyElementType type, BinaryMachineCode *code);

/**
 * Inserts a new symbol (label) node at the end of the linked-list symbol table.
 *
 * @param head Double pointer to the head of the symbol table
 * @param symbol The name of the symbol (label) to be added
 * @param type The type of the assembly element
 * @param address The address associated with the symbol.
 * @return void
 */
void insert_to_symbol_table(SymbolNode **head, char* symbol, AssemblyElementType type, int address);

/**
 * Updates the addresses of DATA symbols in the symbol table after the first pass.
 * Since DATA symbols are stored after the instructions in memory, their addresses
 * must be updated by adding the final instruction count (ICF).
 *
 * @param head_of_symbol_table Pointer to the head of the symbol table
 * @param ICF The final instruction counter value after the first pass
 * @return void
 */
void update_data_symbol_addresses(SymbolNode *head_of_symbol_table, int ICF);

/**
 * Updates the addresses of all data-type lines in the assembly line list after the first pass.
 * Since DATA lines are stored after the instructions in memory, their addresses
 * must be updated by adding the final instruction count (ICF).
 *
 * @param head_of_line_list Pointer to the head of the assembly line list
 * @param ICF The final instruction counter value after the first pass
 * @return void
 */
void update_data_line_list_addresses(AssemblyLineList *head_of_line_list, int ICF);

/**
 * Frees all memory allocated for the symbol table, including its content.
 *
 * @param head Pointer to head of the symbol table
 * @return void
 */
void free_symbol_table(SymbolNode *head);

/**
 * Frees all memory allocated for the line list, including its content.
 *
 * @param head Pointer to head of the assembly line list
 * @return void
 */
void free_line_list(AssemblyLineList *head);

void free_machine_code_list(BinaryMachineCode *array, int ICF, int DCF);
#endif
