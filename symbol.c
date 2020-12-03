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

#include <stdio.h>
#include <strings.h>
#include "symbol.h"


/// @brief Initialize symbol table
/// @brief Create dummy symbol table entry 
/// @param None
/// @retval None
void st_initialize() {
    // Allocate memory for dummy head, tail
    ste *dummy_head = malloc(sizeof(ste));
    if (dummy_head == NULL) printf("malloc error in st_initialize\n");
    
    ste *dummy_tail = malloc(sizeof(ste));
    if (dummy_tail == NULL) printf("malloc error in st_initialize\n");
    
    // Initialize
    dummy_head->id_ptr = NULL;
    dummy_head->decl_ptr = NULL;
    dummy_head->prev = NULL;
    
    dummy_tail->id_ptr = NULL;
    dummy_tail->decl_ptr = NULL;
    dummy_tail->prev = dummy_head;
    
    // Set global st_head and st_tail
    st_head = dummy_head;
    st_tail = dummy_tail;
    
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
