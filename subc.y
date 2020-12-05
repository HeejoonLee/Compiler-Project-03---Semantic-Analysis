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

%}

/* yylval types */
%union {
	int		intVal;
	char	*stringVal;
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
%token<stringVal> CHAR_CONST STRING
%token<id_ptr> TYPE ID

%type<stringVal> pointers
%type<decl_ptr> type_specifier unary binary and_list and_expr
%type<decl_ptr> or_expr or_list expr const_expr

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
            if (st_check_redecl($3)) yyerror("redeclaration");
            else {
                st_insert($3, decl_var($1));
            }
        }
        | type_specifier pointers ID '[' const_expr ']' ';'
        | func_decl ';'
        | type_specifier ';'
        | func_decl compound_stmt

type_specifier
        : TYPE { 
            $$ = st_decl_from_id($1);
         }
        | VOID
        | struct_specifier

struct_specifier
        : STRUCT ID '{' def_list '}'
        | STRUCT ID

func_decl
        : type_specifier pointers ID '(' ')'
        {
            st_insert($3, decl_func($1));
        }
        | type_specifier pointers ID '(' VOID ')'
        | type_specifier pointers ID '('
        {
            decl *func = decl_func($1);
            st_insert($3, func);
            scope_push(); // A new scope for param_list
            
        }
        param_list ')'

pointers
        : '*' { $$ = "*"; }
        | /* empty */ { $$ = NULL; }

param_list  /* list of formal parameter declaration */
        : param_decl
        | param_list ',' param_decl

param_decl  /* formal parameter declaration */
        : type_specifier pointers ID
        | type_specifier pointers ID '[' const_expr ']'

def_list    /* list of definitions, definition can be type(struct), variable, function */
        : def_list def
        | /* empty */

def
        : type_specifier pointers ID ';' 
        { 
            // REDUCE("def->type_specifier pointers ID ;");
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
        | type_specifier pointers ID '[' const_expr ']' ';'
        {
            if ($2 == NULL) {
                // Not a pointer
                if (st_check_redecl($3)) yyerror("redeclaration");
                else st_insert($3, decl_array($1, $5));
            }
            else {
                // Pointer
                // TODO
            }
        }
        | type_specifier ';'
        | func_decl ';'

compound_stmt
        : '{'
        {
            scope_push();
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
        | RETURN expr ';'
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
                        printf("LHS and RHS are compatible\n");
                        $$ = lhs_type;
                    }
                    else {
                        yyerror("LHS and RHS are not same type");
                        $$ = NULL;
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
                    yyerror("not computable");
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
                    yyerror("not computable");
                    $$ = NULL;
                }
            }
        }
        | binary '+' binary
        {
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
        | binary '-' binary
        {
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
        | unary { $$ = $1; } %prec '='

unary
        : '(' expr ')' { $$ = $2; }
        | '(' unary ')' { $$ = $2; }
        | INTEGER_CONST { $$ = decl_int_const($1); }
        | CHAR_CONST { $$ = decl_char_const($1); }
        | STRING
        | NULLT
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
            // Type checking: int
            decl *type_decl = $2;
            if (!st_check_iftype(type_decl)) type_decl = type_decl->type;
            if (st_check_ifint(type_decl)) $$ = type_decl;
            else {
                yyerror("not computable");
                $$ = NULL;
            }
        } %prec '!'
        | '!' unary
        {
            // Type checking: int
            decl *type_decl = $2;
            if (!st_check_iftype(type_decl)) type_decl = type_decl->type;
            if (st_check_ifint(type_decl)) $$ = type_decl;
            else {
                yyerror("not computable");
                $$ = NULL;
            }
        }
        | unary INCOP
        {
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
        | unary DECOP
        {
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
        | INCOP unary
        {
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
        | DECOP unary
        {
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
        | '&' unary %prec '!'
        {
            // Type checking: variable
            decl *type_decl = $2;
            if (!st_check_iftype(type_decl)) type_decl = type_decl->type;
            if (st_check_ifint(type_decl) ||
                st_check_ifchar(type_decl)) $$ = decl_pointer(type_decl)->type;
            else {
                yyerror("not a variable");
                $$ = NULL;
            }
        }
        | '*' unary %prec '!'
        {
            // Type checking: pointer
            decl *type_decl = $2;
            if (!st_check_iftype(type_decl)) type_decl = type_decl->type;
            if (st_check_ifpointer(type_decl)) $$ = type_decl->ptrto->type;
            else {
                yyerror("not a pointer");
                $$ = NULL;
            }
        }
        | unary '[' expr ']'
        | unary '.' ID
        | unary STRUCTOP ID
        | unary '(' args ')'
        | unary '(' ')'

args    /* actual parameters(function arguments) transferred to function */
        : expr
        | args ',' expr
    
%%

/*  Additional C Codes here */

int    yyerror (char* s)
{
    printf("%3d: error: %s\n", read_line(), s);
}


void REDUCE(char *s) {
    printf("%s\n", s);
}


