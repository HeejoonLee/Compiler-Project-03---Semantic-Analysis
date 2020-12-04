#include "symbol.h"

/// @brief Initialize scope stack
/// @brief Two global vars: current_scope, type_scope
/// @brief Each boundary points to the last ste in the scope
/// @param
/// @retval
void scope_initialize() {
    // Allocate memory for type scope
    scope *type_sc = malloc(sizeof(scope));
    if (type_sc == NULL) printf("malloc error in stack_initialize\n");
    
    // Initialize scopes
    type_sc->boundary = NULL;
    type_sc->prev = NULL;
    
    // Set global current_scope and type_scope
    current_scope = type_sc;
    type_scope = type_sc;
}


/// @brief Push the address of the real tail of the symbol table
/// @param
/// @retval
void scope_push() {
    ste *tail = st_tail;
    tail = tail->prev;
    
    scope *new_scope = malloc(sizeof(scope));
    if (new_scope == NULL) printf("malloc error in scope_push()\n");
    
    new_scope->prev = current_scope;
    new_scope->boundary = tail;
    
    current_scope = new_scope;
}


/// @brief Pop scope
/// @param
/// @retval
void scope_pop() {
    scope *prev_scope = current_scope;
    current_scope = prev_scope->prev;
    free(prev_scope);
}


/// @brief Print scope list
/// @param
/// @retval
void scope_print() {
    scope *sc_iter = current_scope;
    printf("Variable scope:\n");
    while(sc_iter != type_scope) {
        printf("%p\n", sc_iter->boundary);
        sc_iter = sc_iter->prev;
    }
    printf("Type scope:\n");
    printf("%p\n", sc_iter->boundary);
    printf("Scope end\n");
}
