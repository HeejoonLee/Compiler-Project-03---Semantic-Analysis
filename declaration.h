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
#include <strings.h>
#include "subc.h"
#include "symbol.h"


/// Struct definition
typedef struct declaration{
    int declclass;
    struct declaration *type;
    int value;
    float real_value;
    struct symbol_table_entry *formals;
    struct symbol_table_entry *returntype;
    int typeclass;
    struct declaration *elementvar;
    int num_index;
    struct symbol_table_entry *fieldlist;
    struct symbol_table_entry *ptrto;
    int size;
    struct symbol_table_entry **scope;
    struct declaration *next;
} decl;



/// Global variables


/// Function declarations
decl *decl_type(id *);
