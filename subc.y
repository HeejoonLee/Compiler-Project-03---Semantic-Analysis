%{
/*
 * File Name   : subc.y
 * Description : a skeleton bison input
 */

#include "subc.h"
#include "subc.tab.h"

int    yylex ();
int    yyerror (char* s);
void REDUCE(char* s);
char *buffer;

%}

/* yylval types */
%union {
	int		intVal;
	char	*stringVal;
    char charVal;
    id *id_ptr;
    decl *decl_ptr;
    ste *ste_ptr;
}

/* Precedences and Associativities */
%left	','
%right '='
%left LOGICAL_OR
%left LOGICAL_AND
%left '&'
%left EQUOP
%left RELOP
%left '+' '-'
%left '*' '/' '%'
%right '!' INCOP DECOP
%left '[' ']' '(' ')' '.' STRUCTOP


/* Token and Types */
%token VOID NULLT STRUCT RETURN IF ELSE WHILE FOR BREAK CONTINUE
%token LOGICAL_OR LOGICAL_AND RELOP EQUOP INCOP DECOP STRUCTOP
%token<intVal> INTEGER_CONST
%token<stringVal> STRING
%token<charVal> CHAR_CONST
%token<id_ptr> TYPE ID

%type<stringVal> pointers
%type<decl_ptr> type_specifier unary binary and_list and_expr
%type<decl_ptr> or_expr or_list expr const_expr func_decl
%type<decl_ptr> param_list param_decl args struct_specifier def ext_def

%%
program
        : ext_def_list
        ;

ext_def_list
        : ext_def_list ext_def
        | /* empty */
        ;

ext_def
        : type_specifier pointers ID ';'
        { 
            if ($1 == NULL) $$ = NULL;
            else {
                if ($2 == NULL) {
                    // Not a pointer
                    if (st_check_redecl($3)) yyerror("redeclaration");
                    else {
                        st_insert($3, decl_var($1));
                    }
                }
                else {
                    // Pointer
                    if (st_check_redecl($3)) yyerror("redeclaration");
                    else {
                        st_insert($3, decl_pointer($1));
                    }
                }
            }
        }
        | type_specifier pointers ID '[' const_expr ']' ';'
        {
            if ($1 == NULL) $$ = NULL;
                else {
                if ($2 == NULL) {
                    // Not a pointer
                    if (st_check_redecl($3)) yyerror("redeclaration");
                    else st_insert($3, decl_array($1, $5));
                }
                else {
                    // Pointer
                    if (st_check_redecl($3)) yyerror("redeclaration");
                    else st_insert($3, decl_pointer_array($1, $5));
                }
            }
        }
        | func_decl ';'
        | type_specifier ';'
        | func_decl compound_stmt
        ;

type_specifier
        : TYPE { 
            $$ = st_decl_from_id($1);
         }
        | VOID { $$ = st_decl_from_id(get_id_from_name("void")); }
        | struct_specifier { $$ = $1; };
        ;

struct_specifier
        : STRUCT ID '{'
        {
            if (st_decl_from_id($2) != NULL) {
                yyerror("redeclaration");
                $<ste_ptr>$ = current_scope->boundary;
            }
            else {
                scope_push();
                $<ste_ptr>$ = NULL;
            }
        }
        def_list
        {
            if ($<ste_ptr>4 != NULL) {
                // Pop all the pushed symbols
                current_scope->boundary = $<ste_ptr>4;
                st_tail->prev = current_scope->boundary;
            }
            else {
                ste *prev_boundary = current_scope->prev->boundary;
                if (current_scope->boundary == prev_boundary) {
                    // 1. No member declared
                    decl *struct_decl = decl_struct_type(NULL);
                    st_insert_global($2, struct_decl);
                    $<decl_ptr>$ = struct_decl;
                }
                else {
                    ste *st_iter = current_scope->boundary;
                    while (st_iter->prev != prev_boundary) st_iter = st_iter->prev;
                    decl *struct_decl = decl_struct_type(current_scope->boundary);
                    st_insert_global($2, struct_decl);
                    st_iter->prev = NULL;
                    $<decl_ptr>$ = struct_decl;
                }
            }
        }
        '}'
        {
            if ($<ste_ptr>4 != NULL) $$ = NULL;
            else {
                scope_pop();
                $$ = $<decl_ptr>6;
            }
        }
        | STRUCT ID
        {
            decl *struct_decl = st_decl_from_id($2);
            if (struct_decl == NULL) {
                yyerror("incomplete type");
                $$ = NULL;
            }
            else $$ = struct_decl;
        }
        ;

func_decl
        : type_specifier pointers ID '(' ')'
        {
            if ($1 == NULL) $$ = NULL;
            else {
                if (!st_check_redecl($3)) {
                    decl *decl_ptr = decl_func($1);
                    st_insert($3, decl_ptr);
                    decl_ptr->formals = st_get_ste_from_decl(decl_ptr);
                    scope_push();
                    $$ = decl_ptr;
                }
                else {
                    yyerror("redeclaration");
                    $$ = NULL;
                }
            }
        }
        | type_specifier pointers ID '(' VOID ')'
        {
            if ($1 == NULL) $$ = NULL;
            else {
                if (!st_check_redecl($3)) {
                    decl *decl_ptr = decl_func($1);
                    st_insert($3, decl_ptr);
                    decl_ptr->formals = st_get_ste_from_decl(decl_ptr);
                    scope_push();
                    $$ = decl_ptr;
                }
                else {
                    yyerror("redeclaration");
                    $$ = NULL;
                }
            }
        }
        | type_specifier pointers ID '('
        {
            if ($1 == NULL) $<decl_ptr>$ = NULL;
            else {
                if (!st_check_redecl($3)) {
                    decl *func = decl_func($1);
                    st_insert($3, func);
                    scope_push(); // A new scope for param_list
                    $<decl_ptr>$ = func;
                }
                else {
                    yyerror("redeclaration");
                    $<decl_ptr>$ = NULL;
                }
            }
        }
        param_list ')'
        {
           if ($<decl_ptr>5 == NULL) $$ = NULL;
           else {
               decl *func = $<decl_ptr>5;
               func->formals = st_get_ste_from_decl($6);
               $$ = func;
           }
        }
               

pointers
        : '*' { $$ = "*"; }
        | /* empty */ { $$ = NULL; }

param_list  /* list of formal parameter declaration */
        : param_decl { $$ = $1; }
        | param_list ',' param_decl
        {
            $3->next = $1;
            $$ = $3;
        }

param_decl  /* formal parameter declaration */
        : type_specifier pointers ID
        { 
            if ($1 == NULL) $$ = NULL;
            else {
                if ($2 == NULL) {
                    // Not a pointer
                    if (st_check_redecl($3)) yyerror("redeclaration");
                    else {
                        decl *var_decl = decl_var($1);
                        st_insert($3, var_decl);
                        $$ = var_decl;
                    }
                }
                else {
                    // Pointer
                    if (st_check_redecl($3)) yyerror("redeclaration");
                    else {
                        decl *pointer_decl = decl_pointer($1);
                        st_insert($3, pointer_decl);
                        $$ = pointer_decl;
                    }
                }
            }
        }
        | type_specifier pointers ID '[' const_expr ']'
        {
            if ($1 == NULL) $$ = NULL;
                else {
                if ($2 == NULL) {
                    // Not a pointer
                    if (st_check_redecl($3)) yyerror("redeclaration");
                    else {
                        decl *decl_ptr = decl_array($1, $5);
                        st_insert($3, decl_ptr);
                        $$ = decl_ptr;
                    }
                }
                else {
                    // Pointer
                    if (st_check_redecl($3)) yyerror("redeclaration");
                    else {
                        decl *decl_ptr = decl_pointer_array($1, $5);
                        st_insert($3, decl_ptr);
                        $$ = decl_ptr;
                    }
                }
            }
        }

def_list    /* list of definitions, definition can be type(struct), variable, function */
        : def_list def
        | /* empty */

def
        : type_specifier pointers ID ';' 
        { 
            if ($1 == NULL) $$ = NULL;
            else {
                if ($2 == NULL) {
                    // Not a pointer
                    if (st_check_redecl($3)) yyerror("redeclaration");
                    else {
                        st_insert($3, decl_var($1));
                    }
                }
                else {
                    // Pointer
                    if (st_check_redecl($3)) yyerror("redeclaration");
                    else {
                        st_insert($3, decl_pointer($1));
                    }
                }
            }
        }
        | type_specifier pointers ID '[' const_expr ']' ';'
        {
            if ($1 == NULL) $$ = NULL;
                else {
                if ($2 == NULL) {
                    // Not a pointer
                    if (st_check_redecl($3)) yyerror("redeclaration");
                    else st_insert($3, decl_array($1, $5));
                }
                else {
                    // Pointer
                    if (st_check_redecl($3)) yyerror("redeclaration");
                    else st_insert($3, decl_pointer_array($1, $5));
                }
            }
        }
        | type_specifier ';' { ; }
        | func_decl ';' { ; }

compound_stmt
        : '{'
        {
            if (($<decl_ptr>0 != NULL) && ($<decl_ptr>0->declclass == DECL_FUNC)) ;
            else {
                scope_push();
            }
        }
        local_defs stmt_list 
        {
            scope_pop();
        }
        '}'

local_defs  /* local definitions, of which scope is only inside of compound statement */
        :   def_list

stmt_list
        : stmt_list stmt
        | /* empty */

stmt
        : expr ';'
        | compound_stmt
        | RETURN ';'
        {
           // Type checking: func rettype must be void
           // Find the closest function
           // Move out of scope until we reach a function
           scope *scope_iter = current_scope;
           scope_iter = scope_iter->prev;
           int match = 0;
           while (scope_iter != type_scope) {
                if (scope_iter->boundary->decl_ptr->declclass == DECL_FUNC) {
                    // Found a function
                    if (st_check_rettype_void(scope_iter->boundary->decl_ptr)) {
                        // printf("RETURN match!\n");
                        match = 1;
                        break;
                    }
                    else {
                        yyerror("incompatible return types");
                        match = 1;
                        break;
                    }
                }
                scope_iter = scope_iter->prev;
            }
            if (match == 0) yyerror("cannot return from non-function");
        }
        | RETURN expr ';'
        {
            if ($2 == NULL) ;
            else {
                scope *scope_iter = current_scope;
                scope_iter = scope_iter->prev;
                int match = 0;
                while (scope_iter != type_scope) {
                    if (scope_iter->boundary->decl_ptr->declclass == DECL_FUNC) {
                        // Found a function
                        decl *expr_decl = $2;
                        if (!st_check_iftype(expr_decl)) expr_decl = expr_decl->type;
                        if (st_check_rettype_match(scope_iter->boundary->decl_ptr, expr_decl)) {
                            // printf("RETURN match!\n");
                            match = 1;
                            break;
                        }
                        else {
                            yyerror("incompatible return types");
                            match = 1;
                            break;
                        }
                    }
                    scope_iter = scope_iter->prev;
                }
                if (match == 0) yyerror("cannot return from non-function");
            }
        }
        | ';'
        | IF '(' expr ')' stmt
        | IF '(' expr ')' stmt ELSE stmt
        | WHILE '(' expr ')' stmt
        | FOR '(' expr_e ';' expr_e ';' expr_e ')' stmt
        | BREAK ';'
        | CONTINUE ';'

expr_e
        : expr
        | /* empty */

const_expr
        : expr { $$ = $1; }

expr
        : unary '=' expr
        {
            // Assignment type checking
            // 1. Check if LHS is VAR
            // 2. Check if RHS and LHS are compatible types
            // 3. Check if RHS is NULL
            if (($1 == NULL) || ($3 == NULL)) $$ = NULL;
            else {
                if (st_check_ifvar($1)) {
                    // LHS is var
                    decl *lhs_type = $1;
                    decl *rhs_type = $3;
                    if (!st_check_iftype(rhs_type)) rhs_type = rhs_type->type;
                    if (!st_check_iftype(lhs_type)) lhs_type = lhs_type->type;
                    if (st_check_type_compat(lhs_type, rhs_type)) {
                        // LHS and RHS are compatible
                        // printf("LHS and RHS are compatible\n");
                        $$ = lhs_type;
                    }
                    else {
                        if (rhs_type->typeclass == 6) {
                            yyerror("RHS is not a const or variable");
                            $$ = NULL;
                        }
                        else {
                            yyerror("LHS and RHS are not same type");
                            $$ = NULL;
                        }
                    }
                }
                else {
                    yyerror("LHS is not a variable");
                    $$ = NULL;
                }
            }
        }
        | or_expr { $$ = $1; }

or_expr
        : or_list { $$ = $1; }

or_list
        : or_list LOGICAL_OR and_expr
        {
            // Type checking: Both operands must be int
            if (($1 == NULL) || ($3 == NULL)) $$ = NULL;
            else {
                decl *lhs_type = $1;
                decl *rhs_type = $3;
                if (!st_check_iftype(lhs_type)) lhs_type = lhs_type->type;
                if (!st_check_iftype(rhs_type)) rhs_type = rhs_type->type;
                if (st_check_bothint(lhs_type, rhs_type)) $$ = lhs_type;
                else {
                    yyerror("not computable");
                    $$ = NULL;
                }
            }
        }
        | and_expr { $$ = $1; }

and_expr
        : and_list { $$ = $1; }

and_list
        : and_list LOGICAL_AND binary
        {
            // Type checking: Both operands must be int
            if (($1 == NULL) || ($3 == NULL)) $$ = NULL;
            else {
                decl *lhs_type = $1;
                decl *rhs_type = $3;
                if (!st_check_iftype(lhs_type)) lhs_type = lhs_type->type;
                if (!st_check_iftype(rhs_type)) rhs_type = rhs_type->type;
                if (st_check_bothint(lhs_type, rhs_type)) $$ = lhs_type;
                else {
                    yyerror("not computable");
                    $$ = NULL;
                }
            }
        }
        | binary { $$ = $1; }

binary
        : binary RELOP binary
        {
            // Type checking: Both int or both char
            // Return int type
            if (($1 == NULL) || ($3 == NULL)) $$ = NULL;
            else {
                decl *lhs_type = $1;
                decl *rhs_type = $3;
                if (!st_check_iftype(lhs_type)) lhs_type = lhs_type->type;
                if (!st_check_iftype(rhs_type)) rhs_type = rhs_type->type;
                if (st_check_bothint(lhs_type, rhs_type) ||
                    st_check_bothchar(lhs_type, rhs_type)) {
                    $$ = st_decl_from_id(get_id_from_name("int"));
                }
                else {
                    yyerror("not comparable");
                    $$ = NULL;
                }
            }
        }
        | binary EQUOP binary
        {
            // Type checking: Both int or both char or both same type of pointers
            // Return int type
            if (($1 == NULL) || ($3 == NULL)) $$ = NULL;
            else {
                decl *lhs_type = $1;
                decl *rhs_type = $3;
                if (!st_check_iftype(lhs_type)) lhs_type = lhs_type->type;
                if (!st_check_iftype(rhs_type)) rhs_type = rhs_type->type;
                if (st_check_bothint(lhs_type, rhs_type) ||
                    st_check_bothchar(lhs_type, rhs_type) ||
                    st_check_both_same_pointers(lhs_type, rhs_type)) {
                    $$ = st_decl_from_id(get_id_from_name("int"));
                }
                else {
                    yyerror("not comparable");
                    $$ = NULL;
                }
            }
        }
        | binary '+' binary
        {
            if (($1 == NULL) || ($3 == NULL)) $$ = NULL;
            else {
                // Type checking: Both operands must be int
                decl *lhs_type = $1;
                decl *rhs_type = $3;
                if (!st_check_iftype(lhs_type)) lhs_type = lhs_type->type;
                if (!st_check_iftype(rhs_type)) rhs_type = rhs_type->type;
                if (st_check_bothint(lhs_type, rhs_type)) $$ = lhs_type;
                else {
                    yyerror("not computable");
                    $$ = NULL;
                }
            }
        }
        | binary '-' binary
        {
            if (($1 == NULL) || ($3 == NULL)) $$ = NULL;
            else {
                // Type checking: Both operands must be int
                decl *lhs_type = $1;
                decl *rhs_type = $3;
                if (!st_check_iftype(lhs_type)) lhs_type = lhs_type->type;
                if (!st_check_iftype(rhs_type)) rhs_type = rhs_type->type;
                if (st_check_bothint(lhs_type, rhs_type)) $$ = lhs_type;
                else {
                    yyerror("not computable");
                    $$ = NULL;
                }
            }
        }
        | unary { $$ = $1; } %prec '='

unary
        : '(' expr ')' { $$ = $2; }
        | '(' unary ')' { $$ = $2; }
        | INTEGER_CONST { $$ = decl_int_const($1); }
        | CHAR_CONST { $$ = decl_char_const($1); }
        | STRING { ; }
        | NULLT { $$ = decl_null_type(); }
        | ID 
        { 
            decl *decl_ptr = st_decl_from_id($1);
            if (decl_ptr != NULL) $$ = decl_ptr;
            else {
                yyerror("not declared");
                $$ = NULL;
            }
        }
        | '-' unary 
        {
            if ($2 == NULL) $$ = NULL;
            else {
                // Type checking: int
                decl *type_decl = $2;
                if (!st_check_iftype(type_decl)) type_decl = type_decl->type;
                if (st_check_ifint(type_decl)) $$ = type_decl;
                else {
                    yyerror("not computable");
                    $$ = NULL;
                }
            }
        } %prec '!'
        | '!' unary
        {
            if ($2 == NULL) $$ = NULL;
            else {
                // Type checking: int
                decl *type_decl = $2;
                if (!st_check_iftype(type_decl)) type_decl = type_decl->type;
                if (st_check_ifint(type_decl)) $$ = type_decl;
                else {
                    yyerror("not computable");
                    $$ = NULL;
                }
            }
        }
        | unary INCOP
        {
            if ($1 == NULL) $$ = NULL;
            else {
                // Type checking: int, char
                decl *type_decl = $1;
                if (!st_check_iftype(type_decl)) type_decl = type_decl->type;
                if (st_check_ifint(type_decl) || st_check_ifchar(type_decl))
                    $$ = type_decl;
                else {
                    yyerror("not computable");
                    $$ = NULL;
                }
            }
        }
        | unary DECOP
        {
            if ($1 == NULL) $$ = NULL;
            else {
                // Type checking: int, char
                decl *type_decl = $1;
                if (!st_check_iftype(type_decl)) type_decl = type_decl->type;
                if (st_check_ifint(type_decl) || st_check_ifchar(type_decl))
                    $$ = type_decl;
                else {
                    yyerror("not computable");
                    $$ = NULL;
                }
            }
        }
        | INCOP unary
        {
            if ($2 == NULL) $$ = NULL;
            else {
                // Type checking: int, char
                decl *type_decl = $2;
                if (!st_check_iftype(type_decl)) type_decl = type_decl->type;
                if (st_check_ifint(type_decl) || st_check_ifchar(type_decl))
                    $$ = type_decl;
                else {
                    yyerror("not computable");
                    $$ = NULL;
                }
            }
        }
        | DECOP unary
        {
            if ($2 == NULL) $$ = NULL;
            else {
                // Type checking: int, char
                decl *type_decl = $2;
                if (!st_check_iftype(type_decl)) type_decl = type_decl->type;
                if (st_check_ifint(type_decl) || st_check_ifchar(type_decl))
                    $$ = type_decl;
                else {
                    yyerror("not computable");
                    $$ = NULL;
                }
            }
        }
        | '&' unary %prec '!'
        {
            if ($2 == NULL) $$ = NULL;
            else {
                // Type checking: variable
                decl *type_decl = $2;
                if (!st_check_iftype(type_decl)) type_decl = type_decl->type;
                if (st_check_ifint(type_decl) ||
                    st_check_ifchar(type_decl) ||
                    st_check_ifstruct(type_decl)) $$ = decl_pointer(type_decl)->type;
                else {
                    yyerror("not a variable");
                    $$ = NULL;
                }
            }
        }
        | '*' unary %prec '!'
        {
            if ($2 == NULL) $$ = NULL;
            else {
                // Type checking: pointer
                decl *type_decl = $2;
                if (!st_check_iftype(type_decl)) type_decl = type_decl->type;
                if (st_check_ifpointer(type_decl)) $$ = type_decl->ptrto->type;
                else {
                    yyerror("not a pointer");
                    $$ = NULL;
                }
            }
        }
        | unary '[' expr ']'
        {
            if (($1 == NULL) || ($3 == NULL)) $$ = NULL;
            else {
                // Type checking: array
                decl *type_decl = $1;
                if (!st_check_iftype(type_decl)) type_decl = type_decl->type;
                if (st_check_ifarray(type_decl)) {
                    // array
                    if(st_check_array_index_range(type_decl, $3)) 
                        $$ = type_decl->elementvar;
                    else {
                        yyerror("array index out of range");
                        $$ = NULL;
                    }
                }
                else {
                    yyerror("not an array type");
                    $$ = NULL;
                }
            }
        }
        | unary '.' ID
        {
            if ($1 == NULL) $$ = NULL;
            else {
                // Type checking: (1) unary must be a struct
                // (2) ID must be a valid member
                decl *type_decl = $1;
                if (!st_check_iftype(type_decl)) type_decl = type_decl->type;
                if (st_check_ifstruct(type_decl)) {
                    ste *st_iter = type_decl->fieldlist;
                    int flag = 0;
                    while (st_iter != NULL) {
                        if ($3 == st_iter->id_ptr) {
                            flag = 1;
                            break;
                        }
                        st_iter = st_iter->prev;
                    }
                    if (flag == 0) {
                        yyerror("struct not have same name field");
                        $$ = NULL;
                    }
                    else $$ = st_iter->decl_ptr->type;
                }
                else {
                    yyerror("not a struct");
                    $$ = NULL;
                }
            }
        }
        | unary STRUCTOP ID
        {
            if ($1 == NULL) $$ = NULL;
            else {
                // Type checking: (1) unary must be a struct pointer
                // (2) ID must be a valid member
                decl *type_decl = $1;
                if (!st_check_iftype(type_decl)) type_decl = type_decl->type;
                if (st_check_ifstructpointer(type_decl)) {
                    ste *st_iter = type_decl->ptrto->type->fieldlist;
                    int flag = 0;
                    while (st_iter != NULL) {
                        if ($3 == st_iter->id_ptr) {
                            flag = 1;
                            break;
                        }
                        st_iter = st_iter->prev;
                    }
                    if (flag == 0) {
                        yyerror("struct not have same name field");
                        $$ = NULL;
                    }
                    else $$ = st_iter->decl_ptr->type;
                }
                else {
                    yyerror("not a struct pointer");
                    $$ = NULL;
                }
            }
        }
        | unary '(' args ')'
        {
            if ($1 == NULL) $$ = NULL;
            else {
                // Type checking: (1) unary must be a function
                // (2) type of args should match
                if (st_check_iffunc($1)) {
                    // Function
                    ste *func_ste = st_get_ste_from_decl($1);
                    ste *ste_iter = $1->formals;
                    decl *decl_iter = $3;
                    int match = 1;
                    while (ste_iter != func_ste) {
                        if (decl_iter == NULL) {
                                match = 0;
                                break;
                        }
                        if (!st_check_type_compat(decl_iter, ste_iter->decl_ptr->type)) {
                            match = 0;
                            break;
                        }
                        ste_iter = ste_iter->prev;
                        decl_iter = decl_iter->next;
                    }
                    if (decl_iter != NULL) match = 0;
                    if (match == 0) {
                        yyerror("actual args are not equal to formal args");
                        $$ = NULL;
                    }
                    else {
                        $$ = func_ste->decl_ptr->returntype;
                        // printf("Args match!\n");
                    }
                }
                else {
                    yyerror("not a function");
                    $$ = NULL;
                }
            }
        }
        | unary '(' ')'
        {
            if ($1 == NULL) $$ = NULL;
            else {
                // Type checking: (1) unary must be a function
                // (2) type of args should match
                if (st_check_iffunc($1)) {
                    ste *func_ste = st_get_ste_from_decl($1);
                    ste *ste_iter = $1->formals;
                    if (func_ste == ste_iter) {
                        // printf("Args match!\n");
                        $$ = func_ste->decl_ptr->returntype;
                    }
                    else {
                        yyerror("actual args are not equal to formal args");
                        $$ = NULL;
                    }
                }
                else  {
                    yyerror("not a function");
                    $$ = NULL;
                }
            }
        }

args    /* actual parameters(function arguments) transferred to function */
        : expr 
        { 
            // Make a list of types
            if ($1 == NULL) $$ = NULL;
            else {
                decl *type_decl = $1;
                if (!st_check_iftype(type_decl)) type_decl = type_decl->type;
                $$ = decl_type_from_type(type_decl);
            }
        }
        | args ',' expr 
        {
            if (($1 == NULL) || ($3 == NULL)) $$ = NULL;
            else {
                decl *type_decl = $3;
                if (!st_check_iftype(type_decl)) type_decl = type_decl->type;
                decl *new_type = decl_type_from_type(type_decl);
                new_type->next = $1;
                $$ = new_type;
            }
        }
            
    
%%

/*  Additional C Codes here */

int    yyerror (char* s)
{
    printf("%s:%3d: error: %s\n", buffer, read_line(), s);
}


void REDUCE(char *s) {
    printf("%s\n", s);
}


