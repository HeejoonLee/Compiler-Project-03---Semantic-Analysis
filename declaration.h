//--------------------------------------------------------------------------------------------------
// Compiler                      Lab03 Semantic Analysis                             Fall 2020
//
/// @file
/// @brief Declaration header file
/// @author Heejoon Lee <hjl951130@snu.ac.kr>
/// @section changelog Change Log
/// 2020/12/03 File created
///
//--------------------------------------------------------------------------------------------------

#ifndef __DECLARATION_H__
#define __DECLARATION_H__

#include <stdio.h>
#include <string.h>
#include <stdlib.h>

typedef struct declaration decl;
#include "subc.h"
#include "symbol.h"

#define DECL_VAR 0
#define DECL_CONST 1
#define DECL_FUNC 2
#define DECL_TYPE 3


/// Struct definition
typedef struct declaration{
    int declclass;
    struct declaration *type;
    int value;
    float real_value;
    struct symbol_table_entry *formals;
    struct declaration *returntype;
    int typeclass;
    struct declaration *elementvar;
    int num_index;
    struct symbol_table_entry *fieldlist;
    struct declaration *ptrto;
    int size;
    struct symbol_table_entry **scope;
    struct declaration *next;
} decl;



/// Global variables



/// Function declarations
decl *decl_type(id *);
decl *decl_var(decl *type_decl);
decl *decl_func(decl *type_decl);
decl *decl_pointer(decl *type_decl);
decl *decl_pointer_type(decl *type_decl);
decl *decl_char_const(char char_value);
decl *decl_int_const(int int_value);
decl *decl_array(decl *type_decl, decl *const_decl);
decl *decl_array_type(decl *type_decl, decl *const_decl);
decl *decl_type_from_type(decl *decl_type);
decl *decl_struct_type(ste *fields);
decl *decl_null_type();
decl *decl_pointer_array_type(decl *type_decl, decl *const_decl);
decl *decl_pointer_array(decl *type_decl, decl *const_decl);

int _get_size(id *);
int decl_is_type(decl *);

#endif
