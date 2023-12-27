%{
#include "struct_func.h"
#include "parser.tab.h"

struct AST;
%}

struct AST;

%%
\"[^"]*\"          { yylval.strval = strdup(yytext); return STRING; }
[0-9]+             { yylval.intval = atoi(yytext); return INTEGER; }
"create_ast"    { yylval.strval = strdup(yytext); return CREATE_AST; }
"if"               { return IF; }
"int"              { return INT_TYPE; }
"string"           { return STRING_TYPE; }
"else"             { return ELSE; }
"light_on"         { return LIGHT_ON; }
"light_off"        { return LIGHT_OFF; }
"conder_on"        { return CONDER_ON; }
"conder_off"       { return CONDER_OFF; }
"status"           { return STATUS; }
"set_temperature"  { return SET_TEMPERATURE; }
"print"            { return PRINT; }
"("                { return LPAREN; }
")"                { return RPAREN; }
"{"                { return LBRACE; }
"}"                { return RBRACE; }
">"                { yylval.strval = strdup(yytext); return GREATER; }
"<"                { yylval.strval = strdup(yytext); return LESS; }
"=="               { yylval.strval = strdup(yytext); return EQUAL; }
";"                { return SEMICOLON; }
[a-zA-Z_][a-zA-Z0-9_]* { yylval.strval = strdup(yytext); return ID; }

[ \t\n]            ;
.                  { return DOT; }
%%

int yywrap() {
    return 1;
}