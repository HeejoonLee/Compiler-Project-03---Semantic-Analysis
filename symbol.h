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
#include "subc.h"
#include "declaration.h"

#define TYPE_SIZE 2

/// Struct definition
typedef struct symbol_table_entry{
	struct id *id_ptr;
	struct declaration *decl_ptr;
	struct symbol_table_entry *prev;
} ste;


/// Global variables
ste *st_head;
ste *st_tail;


/// Function declarations
void st_initialize();
void st_print();
ste *st_insert(id *, decl *);
ste *st_declare(id *var_id, decl *type_decl);
decl *st_decl_from_id(id *id_ptr);

#endif
