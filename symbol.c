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

char *st_type_list[TYPE_SIZE] = { "int", "char" };


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
    
    // Add types(int, char) to the symbol table
    // Update type_scope accordingly
    int i;
    for (i=0; i<TYPE_SIZE; i++) {
        id *type_id = get_id_from_name(st_type_list[i]);
        decl *type_decl = decl_type(type_id);
        type_scope->boundary = st_insert(type_id, type_decl); 
    }
}


/// @brief Insert a new symbol table entry
/// @param id* pointer to an id struct of the entry
/// @param  decl* pointer to the declaration of the entry
/// @retval ste* pointer to the created symbol table entry
ste *st_insert(id *id_ptr, decl *decl_ptr) {
    ste *prev_tail = st_tail->prev;
    
    // Allocate memory for a new symbol table entry
    ste *new_ste = malloc(sizeof(ste));
    if (new_ste == NULL) printf("malloc error in st_insert\n");
    
    new_ste->id_ptr = id_ptr;
    new_ste->decl_ptr = decl_ptr;
    new_ste->prev = prev_tail;
    st_tail->prev = new_ste;
    
    // Update scope boundary
    current_scope->boundary = new_ste;
    
    return new_ste;
}


/// @brief Insert a new var to symbol table
/// @brief Create a decl with declclass Var
/// @brief and set its type pointer to type_decl
/// @param id* pointer to an id struct of the entry
/// @param  decl* pointer to the declaration of the type
/// @retval ste* pointer to the created symbol table entry
ste *st_declare(id *var_id, decl *type_decl) {
    decl *var_decl  = decl_var(type_decl);
    return st_insert(var_id, var_decl);
}


/// @brief Print the current symbol table from tail to head
/// @param None
/// @retval None
void st_print() {
    ste *st_iter = st_tail;
    st_iter = st_iter->prev;
    
    printf("\nSymbol Table\n");
    while(st_iter != st_head) {
        printf("ADDR: %p\n", st_iter);
        printf("------------------------------\n");
        printf("ID  : %p, %s", st_iter->id_ptr, st_iter->id_ptr->name);
        printf("\n");
        printf("DECL: %p, %d", st_iter->decl_ptr, st_iter->decl_ptr->declclass);
        printf("\n");
        if (st_iter->decl_ptr->declclass == DECL_VAR) {
            printf("TYPE: %p", st_iter->decl_ptr->type);
            printf("\n");
        }
        printf("------------------------------\n");
        printf("               |              \n");
        printf("               V              \n");
        st_iter = st_iter->prev;
    }
    printf("End of symbol table\n");
}


/// @brief Get the decl with the id
/// @param id* pointer to an id struct of the entry
/// @retval decl* pointer
decl *st_decl_from_id(id *id_ptr) {
    ste *st_iter = st_tail;
    st_iter = st_iter->prev;
    
    while(st_iter != st_head) {
        if (st_iter->id_ptr == id_ptr) return st_iter->decl_ptr;
        st_iter = st_iter->prev;
    }
    
    // No such id found in the symbol table
    return NULL;
}
