//--------------------------------------------------------------------------------------------------
// Compiler                      Lab03 Semantic Analysis                             Fall 2020
//
/// @file
/// @brief Symbol table entry definitions
/// @author Heejoon Lee <hjl951130@snu.ac.kr>
/// @section changelog Change Log
/// 2020/12/03 File created
///
//--------------------------------------------------------------------------------------------------


#include "symbol.h"


/// @brief Initialize symbol table
/// @brief Create dummy symbol table entry and insert primitives
/// @param None
/// @retval None
void st_initialize() {
    // Allocate memory for dummy head, tail
    ste *dummy_head = malloc(sizeof(ste));
    if (dummy_head == NULL) printf("malloc error in st_initialize\n");
    
    ste *dummy_tail = malloc(sizeof(ste));
    if (dummy_tail == NULL) printf("malloc error in st_initialize\n");
    
    // Initialize dummies
    dummy_head->id_ptr = NULL;
    dummy_head->decl_ptr = NULL;
    dummy_head->prev = NULL;
    
    dummy_tail->id_ptr = NULL;
    dummy_tail->decl_ptr = NULL;
    dummy_tail->prev = dummy_head;
    
    // Set global st_head and st_tail
    st_head = dummy_head;
    st_tail = dummy_tail;
    
    // Add int and char to the symbol table
    int i;
    for (i=0; i<TYPE_SIZE; i++) {
        id *type_id = get_id_from_name(type_list[i]);
        decl *type_decl = decl_type(type_id); // TODO
        st_insert(type_id, type_decl);        
    }
}


/// @brief Insert a new symbol table entry
/// @param id* pointer to an id struct of the entry
/// @param  decl* pointer to the declaration of the entry
/// @retval None
void st_insert(id *id_ptr, decl *decl_ptr) {
    ste *prev_tail = st_tail->prev;
    
    // Allocate memory for a new symbol table entry
    ste *new_ste = malloc(sizeof(ste));
    if (new_ste == NULL) printf("malloc error in st_insert\n");
    
    new_ste->id_ptr = id_ptr;
    new_ste->decl_ptr = decl_ptr;
    new_ste->prev = prev_tail;
    st_tail->prev = new_ste;
}


/// @brief Print the current symbol table from tail to head
/// @param None
/// @retval None
void st_print() {
    ste *st_iter = st_tail;
    st_iter = st_iter->prev;
    
    printf("Symbol Table\n");
    while(st_iter != st_head) {
        printf("ID: %p, ", st_iter->id_ptr);
        printf("\n");
        printf("DECL: %p, ", st_iter->decl_ptr);
        printf("\n");
        st_iter = st_iter->prev;
    }
    printf("End of symbol table\n");
}
