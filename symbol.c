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

char *st_type_list[TYPE_SIZE] = { "int", "char", "void" };


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


/// @brief Insert a new symbol table entry right after the head
/// @param id*, decl*
/// @retval ste* pointer to the newly created ste
ste *st_insert_global(id *id_ptr, decl *decl_ptr) {
    // search for the ste right after the head
    ste *st_iter = st_tail;
    
    while(st_iter->prev != st_head) {
        st_iter = st_iter->prev;
    }
    
    
    ste *new_ste = malloc(sizeof(ste));
    if (new_ste == NULL) printf("malloc error in st_insert_global\n");
    
    new_ste->id_ptr = id_ptr;
    new_ste->decl_ptr = decl_ptr;
    new_ste->prev = st_head;
    st_iter->prev = new_ste;
    
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
        
        decl *decl_ptr = st_iter->decl_ptr;
        if (decl_ptr->declclass == DECL_VAR) {
            // variable
            printf("DECL: %p, VAR\n", decl_ptr);
            printf("TYPE: %p, %d\n", decl_ptr->type,
            decl_ptr->type->typeclass);
            if (decl_ptr->type->typeclass == 0) {
                // pointer
                printf("PTRT: %p, VAR\n", decl_ptr->type->ptrto);
                printf("TYPE: %p, %d\n", decl_ptr->type->ptrto->type,
                decl_ptr->type->ptrto->type->typeclass);
            }
            else if (decl_ptr->type->typeclass == 5) {
                // struct
                ste *st_iter = decl_ptr->type->fieldlist;
                while (st_iter != NULL) {
                    printf("MEMB: %p, %s\n", st_iter->decl_ptr, st_iter->id_ptr->name);
                    printf("TYPE: %p, %d\n", st_iter->decl_ptr->type,
                           st_iter->decl_ptr->type->typeclass);
                    st_iter = st_iter->prev;
                }
            }
        }
        else if (decl_ptr->declclass == DECL_CONST) {
            // constant
            printf("DECL: %p, CONST\n", decl_ptr);
            printf("TYPE: %p, %d\n", decl_ptr->type, 
            decl_ptr->type->typeclass);
            if (decl_ptr->type->typeclass == 1) {
                // array
                printf("ELEM: %p, VAR\n", decl_ptr->type->elementvar);
                printf("TYPE: %p, %d\n", decl_ptr->type->elementvar->type,
                decl_ptr->type->elementvar->type->typeclass);
                printf("INDX: %d\n", decl_ptr->type->num_index);
            }
        }
        else if (decl_ptr->declclass == DECL_FUNC) {
            // function
            printf("DECL: %p, FUNC\n", decl_ptr);
            printf("FORM: %p\n", decl_ptr->formals);
            printf("RETT: %p, %s\n", decl_ptr->returntype,
            st_get_ste_from_decl(decl_ptr->returntype)->id_ptr->name);
        }
        else {
            // type
            printf("TYPE: %p, %d\n", decl_ptr, decl_ptr->typeclass);
            if (decl_ptr->typeclass == 5) {
                // struct
                ste *st_iter = decl_ptr->fieldlist;
                while (st_iter != NULL) {
                    printf("MEMB: %p, %s\n", st_iter->decl_ptr, st_iter->id_ptr->name);
                    printf("TYPE: %p, %d\n", st_iter->decl_ptr->type,
                           st_iter->decl_ptr->type->typeclass);
                    st_iter = st_iter->prev;
                }
            }
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
    //printf("ERROR: No such id in the symbol table\n");
    return NULL;
}


/// @brief Get ste with the decl
/// @param decl* pointer to search for
/// @return ste* with the decl
ste *st_get_ste_from_decl(decl *decl_ptr) {
    ste *st_iter = st_tail;
    st_iter = st_tail->prev;
    
    while(st_iter!= st_head) {
        if (st_iter->decl_ptr == decl_ptr) return st_iter;
        st_iter = st_iter->prev;
    }
    
    //printf("Such decl doesn't exist in symbol table\n");
    return NULL;
}


// TODO: merge st_check_redcle and st_check_ifdecl
/// @brief Check if there already exists a ste with the same id_ptr
/// @param id_ptr to check for
/// @retval 1 if exists, 0 if not
int st_check_redecl(id *id_ptr) {
    ste *st_iter = current_scope->boundary;
    ste *end = current_scope->prev->boundary;
    
    while(st_iter != end) {
        if (st_iter->id_ptr == id_ptr) return 1;
        st_iter = st_iter->prev;
    }
    
    // No duplicates
    return 0;
}


/// @brief Check if the variable with id_ptr has been declared
/// @param id_ptr to check for
/// @retval 1 if declared, 0 if not
int st_check_ifdecl(id *id_ptr) {
    ste *st_iter = current_scope->boundary;
    ste *end = current_scope->prev->boundary;
    
    while(st_iter != end) {
        if (st_iter->id_ptr == id_ptr) return 1;
        st_iter = st_iter->prev;
    }
    
    // No declaration
    return 0;
}


/// @brief Check if given decl is VAR
/// @param decl* to check
/// @retval 1 if VAR, 0 if not VAR
int st_check_ifvar(decl *decl_ptr) {
    return decl_ptr->declclass == DECL_VAR;
}


/// @brief Check if given decl is TYPE
/// @param decl* to check
/// @retval 1 if TYPE, 0 if not TYPE
int st_check_iftype(decl *decl_ptr) {
    return decl_ptr->declclass == DECL_TYPE;
}


/// @brief Check if given decl is FUNC
/// @param decl* to check
/// @retval 1 if FUNC, 0 if not FUNC
int st_check_iffunc(decl *decl_ptr) {
    return decl_ptr->declclass == DECL_FUNC;
}


/// @brief Check if two decl have compatible types
/// @param decl* decls to check
/// @retval 1 if compatible, 0 if not compatible
int st_check_type_compat(decl *decl_ptr1, decl *decl_ptr2) {
    // 1. Both int
    // 2. Both char
    // 3. Both pointer of same type
    // 4. Both struct of same type
    
    // When LHS is pointer of any type and RHS is NULL
    if ((decl_ptr1->typeclass == 0) && (decl_ptr2->typeclass == 6)) return 1;
    
    // Both int
    if ((decl_ptr1->typeclass == 2) && (decl_ptr2->typeclass == 2)) return 1;
    
    // Both char
    else if ((decl_ptr1->typeclass == 3) && (decl_ptr2->typeclass == 3)) return 1;
    
    // Both pointers
    else if ((decl_ptr1->typeclass == 0) && (decl_ptr2->typeclass == 0)) {
        if ((decl_ptr1->ptrto->type->typeclass == 5) && (decl_ptr2->ptrto->type->typeclass == 5)) {
            // Both struct pointers
            ste *struct_lhs = st_get_ste_from_decl(decl_ptr1->ptrto->type);
            ste *struct_rhs = st_get_ste_from_decl(decl_ptr2->ptrto->type);
            return struct_lhs->id_ptr == struct_rhs->id_ptr;
        }
        else return decl_ptr1->ptrto->type->typeclass == decl_ptr2->ptrto->type->typeclass;
    }
    
    // Both struct of the same type
    else if ((decl_ptr1->typeclass == 5) && (decl_ptr2->typeclass == 5)) {
        ste *struct_lhs = st_get_ste_from_decl(decl_ptr1);
        ste *struct_rhs = st_get_ste_from_decl(decl_ptr2);
        return struct_lhs->id_ptr == struct_rhs->id_ptr;
    }
    
    else return 0;
}


/// @brief Check if two decl are both int
/// @param decl* decls to check
/// @retval 1 if both are int, 0 if not
int st_check_bothint(decl *decl_ptr1, decl *decl_ptr2) {
    return (decl_ptr1->typeclass == 2) && (decl_ptr2->typeclass == 2);
}


/// @brief Check if two decls are both char
/// @param decl* decls to check
/// @retval 1 if both are char, 0 if not
int st_check_bothchar(decl *decl_ptr1, decl *decl_ptr2) {
    return (decl_ptr1->typeclass == 3) && (decl_ptr2->typeclass == 3);
}


/// @brief Check if decl is int
/// @param decl* to check
/// @retval 1 if int, 0 if not int
int st_check_ifint(decl *decl_ptr) {
    return decl_ptr->typeclass == 2;
}


/// @brief Check if decl is char
/// @param decl* to check
/// @retval 1 if char, 0 if not char
int st_check_ifchar(decl *decl_ptr) {
    return decl_ptr->typeclass == 3;
}


/// @brief Check if decl is pointer
/// @param decl* to check
/// @retval 1 if pointer, 0 if not pointer
int st_check_ifpointer(decl *decl_ptr) {
    return decl_ptr->typeclass == 0;
}


/// @brief Check if decl is array
/// @param decl* to check
/// @retval 1 if array, 0 if not array
int st_check_ifarray(decl *decl_ptr) {
    return decl_ptr->typeclass == 1;
}


/// @brief Check if decl is struct
/// @param decl* to check
/// @retval 1 if struct, 0 if not
int st_check_ifstruct(decl *decl_ptr) {
    return decl_ptr->typeclass == 5;
}


/// @brief Check if decl is struct pointer
/// @param decl* to check
/// @retval 1 if struct pointer, 0 if not
int st_check_ifstructpointer(decl *decl_ptr) {
    if (decl_ptr->typeclass != 0) return 0;
    else return decl_ptr->ptrto->type->typeclass == 5;
}


/// @brief Check if both decls are the same pointer of same type
/// @param decl*s to check
/// @retval 1 if pointer of the same type, 0 if not
int st_check_both_same_pointers(decl *decl_ptr1, decl *decl_ptr2) {
    if ((decl_ptr1->typeclass == 0) && (decl_ptr2->typeclass == 0))
        return decl_ptr1->ptrto->type->typeclass == decl_ptr2->ptrto->type->typeclass;
    else return 0;
}


/// @brief Check if array index is in range
/// @param decl* array_type, decl* const_decl
/// @retval 1 if in range, 0 if not
int st_check_array_index_range(decl *type, decl *const_decl) {
    return const_decl->value < type->num_index;
}


/// @brief Check if function return type is void
/// @param decl* of func decl
/// @retval 1 if rettype is void, 0 if not
int st_check_rettype_void(decl *func_decl) {
    return func_decl->returntype->typeclass == 4;
}


/// @brief Check if function return type matches the type of expr
/// @param decl* of func decl, expr type
/// @retval 1 if match, 0 if not
int st_check_rettype_match(decl *func_decl, decl *expr_type) {
    if (func_decl->returntype->typeclass == 4) {
        // return type: void
        return 0;
    }
    else if (func_decl->returntype->typeclass == 3) {
        // return type: char
        return expr_type->typeclass == 3;
    }
    else if (func_decl->returntype->typeclass == 2) {
        // return type: int
        return expr_type->typeclass == 2;
    }
    else if (func_decl->returntype->typeclass == 0) {
        // return type pointer
        return func_decl->returntype->ptrto->type->typeclass ==
               expr_type->ptrto->type->typeclass;
    }
    else return 0;
}
