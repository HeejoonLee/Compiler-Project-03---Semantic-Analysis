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
void st_insert(id *, decl *);


#endif
