//--------------------------------------------------------------------------------------------------
// Compiler                      Lab03 Semantic Analysis                             Fall 2020
//
/// @file
/// @brief Declaration c file
/// @author Heejoon Lee <hjl951130@snu.ac.kr>
/// @section changelog Change Log
/// 2020/12/03 File created
///
//--------------------------------------------------------------------------------------------------

#include "declaration.h"


/// @brief Make a new declaration of type type_id
/// @param id* pointer to an id struct of the entry
/// @retval decl* pointer to the declaration
decl *decl_type(id *type_id) {
    decl *new_decl = malloc(sizeof(decl));
    if (new_decl == NULL) printf("malloc error in decl_type\n");
    
    new_decl->declclass = DECL_TYPE;
    new_decl->type = NULL;
    new_decl->value = 0;
    new_decl->real_value = 0.0;
    new_decl->formals = NULL;
    new_decl->returntype = NULL;
    new_decl->typeclass = type_id->tokenType;
    new_decl->elementvar = NULL;
    new_decl->num_index = 0;
    new_decl->fieldlist = NULL;
    new_decl->ptrto = NULL;
    new_decl->size = _get_size(type_id);
    new_decl->scope = NULL;
    new_decl->next = NULL;
    
    return new_decl;
}


/// @brief Make a new declaration of type var
/// @param decl* pointer to decl of var type
/// @retval decl* pointer to the new declaration
decl *decl_var(decl *type_decl) {
    decl *new_decl = malloc(sizeof(decl));
    if (new_decl == NULL) printf("malloc error in decl_var\n");
    
    new_decl->declclass = DECL_VAR;
    new_decl->type = type_decl;
    new_decl->value = 0;
    new_decl->real_value = 0.0;
    new_decl->formals = NULL;
    new_decl->returntype = NULL;
    new_decl->typeclass = 0;
    new_decl->elementvar = NULL;
    new_decl->num_index = 0;
    new_decl->fieldlist = NULL;
    new_decl->ptrto = NULL;
    new_decl->size = type_decl->size;
    new_decl->scope = NULL;
    new_decl->next = NULL;
    
    return new_decl;
}


/// @brief Get the size of the type
/// @param id* pointer to an id struct of the entry
/// @retval int size of the type
int _get_size(id *type_id) {
    int return_value = 0;
    
    if (strcmp(type_id->name, "int") == 0) return_value = sizeof(int);
    else if (strcmp(type_id->name, "char") == 0) return_value = sizeof(char);
    
    return return_value;
}


/// @brief Return whether the decl is type or not
/// @param decl* decl to check for
/// @retval 0: false, 1: true
int decl_is_type(decl *decl_ptr) {
    return decl_ptr->declclass == DECL_TYPE;
}


