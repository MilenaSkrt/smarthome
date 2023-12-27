#include <stdbool.h>
#ifndef STRUCT_FUNC_H
#define STRUCT_FUNC_H
#define MAX_OBJECTS 100 
#define MAX_VARIABLES 50

typedef struct {
    char name[50];
    int light_state;
    int thermostat_temperature;
    int conder;
} AST;

typedef void (*Block)();
extern bool last_condition_result;
extern AST* objects[MAX_OBJECTS];
extern int objectCount;
typedef void (*CommandFunction)(AST* obj);

typedef enum { INT, VAR_STRING_TYPE } VarType;


typedef struct {
    char name[50];
    VarType type;
    union {
        int int_value;
        char* str_value;
    };
} Variable;

extern Variable variables[MAX_VARIABLES];
extern int variableCount;

typedef struct Command {
    void (*execute)(AST*, int);  // Обновите указатель на функцию, чтобы он мог принимать аргумент
    AST* object;                // Указатель на объект
    int arg;                            // Аргумент для команды
} Command;

typedef struct CommandList {
    Command* commands; // Массив команд
    int count; // Количество команд в списке
} CommandList;


typedef struct {
    AST* object;      // Указатель на объект (если требуется)
    char* attribute;          // Название атрибута объекта (если требуется)
    int left_value;           // Левый операнд для сравнения
    int right_value;          // Правый операнд для сравнения
    char* operator;           // Оператор сравнения
} Condition;

extern AST* current_object;
extern Block if_block;
extern Block else_block;
extern bool in_false_if_block;

void add_cmd(CommandFunction func, AST* obj);
void execute_commands();
void clear_commands();
void execute_block_list(Block* block_list);
AST* getast(const char* name);
void execute_if_else(Condition* condition, Block if_block, Block else_block);
void execute_block(const char* command, AST* obj);
void add_cmd_list(void (*execute)(AST*, int), AST* obj, int arg);
void print_ast_state(AST* obj);
AST* create_ast(const char* name);
void print_attribute(int attribute_value);
void light_on(AST* obj);
bool evaluate(const Condition* condition);
void execute_method(AST* obj, const char* method_name, int argument) ;
int get_attribute_value(AST* obj, const char* attribute_name);
void light_off(AST* obj);
void set_temp(AST* obj, int temperature);
void conder_on(AST* obj);
void conder_off(AST* obj);
Condition* create_condition(int left_value, char* operator, int right_value);
Condition* create_condition_attribute(AST* object, const char* attribute_name, char* operator, int right_value);
Condition* create_condition_object(const char* object_name);
Condition* create_condition_time(const char* time_expression);



#endif 