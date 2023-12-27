%{
#include <stdio.h>
#include <stdlib.h>
#include "struct_func.h"
#include <string.h>
#include "parser.tab.h"

bool in_false_if_block = false;
extern FILE* yyin;
extern int yylex();
extern char* yytext;

struct AST;
AST* root = NULL;
CommandList* current_command_list = NULL;
void yyerror(const char* s);
%}

%lex-param { YYSTYPE yylval; }

%union {
    int intval;
    char* strval;
    AST* objectval;
    Condition* conditionval;
    Block* blockval;
}

%token <intval> INTEGER 
%token <objectval> CREATE_AST IF ELSE TURN_ON TURN_OFF  
%token COLON SEMICOLON LPAREN RPAREN LBRACE RBRACE COMMA DOT SET_TEMPERATURE PRINT
%token <strval> STRING_VALUE 
%token INT_TYPE STRING_TYPE 
%token <strval> LIGHT_ON LIGHT_OFF CONDER_ON CONDER_OFF STATUS GREATER LESS EQUAL STRING ID 

%type <intval> expression SET_TEMPERATURE  attribute argument_list
%type <strval> attribute_name relation_operator
%type <objectval> create_ast_statement light_command conder_command status_command object expression_statement
%type <conditionval> condition
%type <blockval> statement_list
%%

program: statement_list
        ;

statement_list: statement
              | statement_list statement
              ;


statement: create_ast_statement SEMICOLON 
         | expression_statement SEMICOLON
         | light_command SEMICOLON
         | conder_command SEMICOLON  
         | status_command SEMICOLON 
         | set_temperature_statement SEMICOLON    
         | print_statement SEMICOLON 
         | if_else_statement 
         ;

create_ast_statement: CREATE_AST STRING { $$ = create_ast($2); root = $$; }
;

light_command: object DOT LIGHT_ON LPAREN RPAREN { 
                add_cmd_list(light_on, getast($1), 0); 
             }
             | object DOT LIGHT_OFF LPAREN RPAREN { 
                add_cmd_list(light_off, getast($1), 0); 
             }
             ;

conder_command: object DOT CONDER_ON LPAREN RPAREN { add_cmd_list(conder_on, getast($1), 0); }
             | object DOT CONDER_OFF LPAREN RPAREN { add_cmd_list(conder_off, getast($1), 0); }
             ;


set_temperature_statement: object DOT SET_TEMPERATURE LPAREN INTEGER RPAREN { add_cmd_list(set_temp, getast($1), $5) ;};


             
status_command: object DOT STATUS LPAREN RPAREN {add_cmd_list(print_ast_state, getast($1), 0) ; };

expression_statement: object DOT attribute_name LPAREN argument_list RPAREN SEMICOLON
{
    execute_method($1, $3, $5);
}


print_statement: PRINT LPAREN attribute RPAREN {add_cmd_list(print_attribute, $3 , 0) ;}          
                        
;

condition: LPAREN expression relation_operator expression RPAREN
  {
    $$ = create_condition($2, $3, $4);
  }
;



relation_operator: GREATER
                | LESS 
                | EQUAL 
                ;

if_else_statement:
{
    execute_command_list();
}
IF condition LBRACE
{
    bool condition_result = evaluate($3);
    in_false_if_block = !condition_result;
}
statement_list RBRACE ELSE LBRACE
{
    in_false_if_block = !in_false_if_block;
}
statement_list RBRACE
{
    in_false_if_block = false;
}
|
{
    execute_command_list();
}
IF condition LBRACE
{
    bool condition_result = evaluate($3);
    in_false_if_block = !condition_result;
}
statement_list RBRACE
{
    in_false_if_block = false;
}
;


argument_list: expression
            | argument_list COMMA expression
            ;

expression: INTEGER
          | ID
          | object DOT attribute_name LPAREN argument_list RPAREN { $$ = (AST*)$1; }
          | attribute
          | condition
          ;

object: STRING { $$ = getast($1); }
      ;
      
attribute_name: ID
              ;

attribute: object DOT ID { $$ = get_attribute_value(root, $3); }
;
%%

void yyerror(const char* s) {
    fprintf(stderr, "Error: %s near '%s'\n", s, yytext);
    exit(EXIT_FAILURE);
}

int main(int argc, char* argv[]) {
    if (argc != 2) {
        fprintf(stderr, "Usage: %s input_file\n", argv[0]);
        exit(EXIT_FAILURE);
    }
    FILE* input_file = fopen(argv[1], "r");
    if (!input_file) {
        perror("Error opening input file");
        exit(EXIT_FAILURE);
    }
    yyin = input_file;
    AST* obj = NULL;
    yyparse();
    execute_command_list();
    fclose(input_file);
    return 0;
}