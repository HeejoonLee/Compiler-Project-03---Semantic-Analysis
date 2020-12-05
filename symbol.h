//--------------------------------------------------------------------------------------------------
// Compiler                      Lab03 Semantic Analysis                             Fall 2020
//
/// @file
/// @brief Symbol table entry global vars and functions
/// @author Heejoon Lee <hjl951130@snu.ac.kr>
/// @section changelog Change Log
/// 2020/12/03 File created
///
//--------------------------------------------------------------------------------------------------

#ifndef __SYMBOL_H__
#define __SYMBOL_H__

#include <stdio.h>
#include <string.h>
#include <stdlib.h>  // malloc()

typedef struct symbol_table_entry ste;
typedef struct scope scope;
#include "subc.h"
#include "declaration.h"

#define TYPE_SIZE 2

/// Struct definition
// Symbol table 
typedef struct symbol_table_entry{
	struct id *id_ptr;
	struct declaration *decl_ptr;
	struct symbol_table_entry *prev;
} ste;

// Scope stack
typedef struct scope{
    ste *boundary;
    struct scope *prev;
} scope;


/// Global variables
ste *st_head;
ste *st_tail;

scope *current_scope;
scope *type_scope;


/// Function declarations
// Symbol functions
void st_initialize();
void st_print();
ste *st_insert(id *, decl *);
ste *st_declare(id *var_id, decl *type_decl);
decl *st_decl_from_id(id *id_ptr);
ste *st_get_ste_from_decl(decl *decl_ptr);

int st_check_ifdecl(id *id_ptr);
int st_check_redecl(id *id_ptr);
int st_check_ifvar(decl *);
int st_check_iftype(decl *);
int st_check_type_compat(decl *, decl *);
int st_check_bothint(decl *decl_ptr1, decl *decl_ptr2);
int st_check_bothchar(decl *decl_ptr1, decl *decl_ptr2);
int st_check_ifint(decl *decl_ptr);
int st_check_ifchar(decl *decl_ptr);

// Scope functions
void scope_initialize();
void scope_push();
void scope_pop();
void scope_print();

#endif
