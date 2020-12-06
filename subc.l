%{
/*
 * File Name   : subc.l
 * Description : a skeleton flex input
 */

#include "subc.h"
#include "subc.tab.h"
static int comment_depth = 0;
static int lineno = 1;

%}

/* regular definitions */
letter		[A-Za-z_]
digit		[0-9]
id		{letter}({letter}|{digit})*
integer_const	([1-9]{digit}*)|"0"
white-space		[ \t]+
special-char	\\[nt]
operator       [-.,;\[\]\*()\{\}=\|&\/%\+!]


%x COMMENT
%%
{id}	{
		   struct id *entered  = enter(ID, yytext, yyleng);
		   yylval.id_ptr = entered;
		   return entered->tokenType;
		}
		
"\|\|" { return LOGICAL_OR; }
"&&" { return LOGICAL_AND; }
"<"|"<="|">"|">=" { return RELOP; }
"=="|"!=" { return EQUOP; }
"\+\+" { return INCOP; }
"--" { return DECOP; }
"->" { return STRUCTOP; }

{integer_const}	{
		   yylval.intVal = atoi(yytext);
		   return INTEGER_CONST;
		}
\"({special-char}|[^\\\"\n])*\"	{
			/* String Variable */
		   yylval.stringVal = yytext;
		   return STRING;
		}
'({special-char}|[^\\'\n])'	{
			/* Character Variable */
		   yylval.stringVal = yytext;
		   return CHAR_CONST;
		}
{operator} { return yytext[0]; }

{white-space}
<*>\n		   {
    ++lineno;

    if (lineno == 27 ||
        lineno == 30 ||
        lineno == 33 ) {
        printf("LINE: %3d\n", lineno);
        scope_print();
        st_print();
    }

}
<INITIAL,COMMENT>"/*"	{
		   ++comment_depth;
		   BEGIN (COMMENT);
		}
<COMMENT>"*/"	   if(--comment_depth == 0) BEGIN (INITIAL);
<COMMENT>.		 {}


%%

int read_line()
{
   return lineno;
}

int main(int argc, char* argv[])
{
   static char *keyword[] = { "int", "char", "void", "NULL", "struct",
                              "return", "if", "else", "while", "for",
                              "break", "continue", NULL };
   static int tokentype[] = { TYPE, TYPE, VOID, NULLT, STRUCT,
                              RETURN, IF, ELSE, WHILE, FOR,
                              BREAK, CONTINUE, 0 };

   int i;
   for(i = 0; keyword[i] != NULL; i++)
      enter(tokentype[i], keyword[i], strlen(keyword[i]));
      
   // ht_print();

   if(argc >= 2) yyin = fopen(argv[1], "r");
   else yyin = stdin;
   if(!yyin) {
      printf("Can't open input stream!\n");
      exit(1);
   }
   
   scope_initialize();
   st_initialize();
   
   st_print();
   scope_print();

   yyparse();
   fclose(yyin);
   
   st_print();
   scope_print();
   // ht_print();
   return 0;
}
