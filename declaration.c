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

// Typeclass
// 0: pointer, 1: array, 2: int, 3: char


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
    
    int type;
    if (strcmp(type_id->name, "pointer") == 0) type = 0;
    else if (strcmp(type_id->name, "array") == 0) type = 1;
    else if (strcmp(type_id->name, "int") == 0) type = 2;
    else if (strcmp(type_id->name, "char") == 0) type = 3;
    else type = 4;
    
    new_decl->typeclass = type;
    new_decl->elementvar = NULL;
    new_decl->num_index = 0;
    new_decl->fieldlist = NULL;
    new_decl->ptrto = NULL;
    new_decl->size = 0;  // TODO: Fix
    new_decl->scope = NULL;
    new_decl->next = NULL;
    
    return new_decl;
}


/// @brief Make a pointer decl
/// @param decl* of type
/// @retval decl* of pointer
decl *decl_pointer(decl *type_decl) {
    decl *new_decl = malloc(sizeof(decl));
    if (new_decl == NULL) printf("malloc error in decl_var\n");
    
    new_decl->declclass = DECL_VAR;
    new_decl->type = decl_pointer_type(type_decl);
    new_decl->value = 0;
    new_decl->real_value = 0.0;
    new_decl->formals = NULL;
    new_decl->returntype = NULL;
    new_decl->typeclass = 0;
    new_decl->elementvar = NULL;
    new_decl->num_index = 0;
    new_decl->fieldlist = NULL;
    new_decl->ptrto = NULL;
    new_decl->size = 0;
    new_decl->scope = NULL;
    new_decl->next = NULL;

    return new_decl;
}


/// @brief New decl of pointer type
/// @param
/// @retval decl* of pointer type
decl *decl_pointer_type(decl *type_decl) {
    decl *new_decl = malloc(sizeof(decl));
    if (new_decl == NULL) printf("malloc error in decl_var\n");
    
    new_decl->declclass = DECL_TYPE;
    new_decl->type = NULL;
    new_decl->value = 0;
    new_decl->real_value = 0.0;
    new_decl->formals = NULL;
    new_decl->returntype = NULL;
    new_decl->typeclass = 0; // pointer
    new_decl->elementvar = NULL;
    new_decl->num_index = 0;
    new_decl->fieldlist = NULL;
    new_decl->ptrto = decl_var(type_decl);
    new_decl->size = 0;
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


/// @brief Make a new declaration of type func
/// @param decl* pointer to return type of the function
/// @retval decl* pointer to the new declaration
decl *decl_func(decl *type_decl) {
    decl *new_decl = malloc(sizeof(decl));
    if (new_decl == NULL) printf("malloc error in decl_func\n");
    
    new_decl->declclass = DECL_FUNC;
    new_decl->type = NULL;
    new_decl->value = 0;
    new_decl->real_value = 0.0;
    new_decl->formals = NULL;
    new_decl->returntype = type_decl;
    new_decl->typeclass = 0;
    new_decl->elementvar = NULL;
    new_decl->num_index = 0;
    new_decl->fieldlist = NULL;
    new_decl->ptrto = NULL;
    new_decl->size = 0;
    new_decl->scope = NULL;
    new_decl->next = NULL;
    
    return new_decl;
}


/// @brief Make a new declaration of integer constant
/// @param int value of the constant
/// @retval decl* to the new decl
decl *decl_int_const(int int_value) {
    decl *new_decl = malloc(sizeof(decl));
    if (new_decl == NULL) printf("malloc error in decl_func\n");
    
    new_decl->declclass = DECL_CONST;
    
    id *int_id = get_id_from_name("int");
    new_decl->type = st_decl_from_id(int_id);
    
    new_decl->value = int_value;
    new_decl->real_value = 0.0;
    new_decl->formals = NULL;
    new_decl->returntype = NULL;
    new_decl->typeclass = 0;
    new_decl->elementvar = NULL;
    new_decl->num_index = 0;
    new_decl->fieldlist = NULL;
    new_decl->ptrto = NULL;
    new_decl->size = 0;
    new_decl->scope = NULL;
    new_decl->next = NULL;
    
    return new_decl;
}


/// @brief Make a new declaration of character constant
/// @param char value of the constant
/// @retval decl* of new decl
decl *decl_char_const(char char_value) {
    decl *new_decl = malloc(sizeof(decl));
    if (new_decl == NULL) printf("malloc error in decl_func\n");
    
    new_decl->declclass = DECL_CONST;
    
    id *char_id = get_id_from_name("char");
    new_decl->type = st_decl_from_id(char_id);
    
    new_decl->value = char_value;
    new_decl->real_value = 0.0;
    new_decl->formals = NULL;
    new_decl->returntype = NULL;
    new_decl->typeclass = 0;
    new_decl->elementvar = NULL;
    new_decl->num_index = 0;
    new_decl->fieldlist = NULL;
    new_decl->ptrto = NULL;
    new_decl->size = 0;
    new_decl->scope = NULL;
    new_decl->next = NULL;
    
    return new_decl;
}


/// @brief Make a new declaration of array
/// @param decl* of type_decl
/// @param decl* of constant_decl
/// @retval decl* of new declaration
decl *decl_array(decl *type_decl, decl *const_decl) {
    decl *new_decl = malloc(sizeof(decl));
    if (new_decl == NULL) printf("malloc error in decl_array\n");
    
    new_decl->declclass = DECL_CONST;
    new_decl->type = decl_array_type(type_decl, const_decl);
    new_decl->value = 0;
    new_decl->real_value = 0.0;
    new_decl->formals = NULL;
    new_decl->returntype = NULL;
    new_decl->typeclass = 0;
    new_decl->elementvar = NULL;
    new_decl->num_index = 0;
    new_decl->fieldlist = NULL;
    new_decl->ptrto = NULL;
    new_decl->size = 0;
    new_decl->scope = NULL;
    new_decl->next = NULL;
    
    return new_decl;
}


/// @brief Make a new array type
/// @param decl* of type_decl
/// @param decl* of constant_decl
/// @retval decl* of new declaration
decl *decl_array_type(decl *type_decl, decl *const_decl) {
    decl *new_decl = malloc(sizeof(decl));
    if (new_decl == NULL) printf("malloc error in decl_array_type\n");
    
    new_decl->declclass = DECL_TYPE;
    new_decl->type = NULL;
    new_decl->value = 0;
    new_decl->real_value = 0.0;
    new_decl->formals = NULL;
    new_decl->returntype = NULL;
    new_decl->typeclass = 1; // array
    new_decl->elementvar = decl_var(type_decl);
    new_decl->num_index = const_decl->value;
    new_decl->fieldlist = NULL;
    new_decl->ptrto = NULL;
    new_decl->size = 0;
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


