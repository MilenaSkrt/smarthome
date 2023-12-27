#include "struct_func.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

Block if_block = NULL;
Block else_block = NULL;
bool last_condition_result = false;

Variable variables[MAX_VARIABLES];
int variableCount = 0;

#define MAX_COMMANDS 100
Command commandList[MAX_COMMANDS];
int commandCount = 0;
AST* objects[MAX_OBJECTS];
int objectCount = 0;

AST* getast(const char* name) {
    for (int i = 0; i < objectCount; i++) {
        if (strcmp(objects[i]->name, name) == 0) {
            return objects[i];
        }
    }
    if (objectCount < MAX_OBJECTS) {
        AST* newObj = create_ast(name);
        objects[objectCount++] = newObj;
        return newObj;
    }

    fprintf(stderr, "Maximum number of objects exceeded\n");
    return NULL;
}

void add_cmd_list(void (*execute)(AST*, int), AST* obj, int arg) {
    if (!in_false_if_block) {
        if (commandCount >= MAX_COMMANDS) {
            fprintf(stderr, "Maximum number of commands exceeded\n");
            return;
        }
        commandList[commandCount].execute = execute;
        commandList[commandCount].object = obj;
        commandList[commandCount].arg = arg;
        commandCount++;
    }
}

void execute_command_list() {
    for (int i = 0; i < commandCount; i++) {
        Command cmd = commandList[i];
        cmd.execute(cmd.object, cmd.arg);
    }
    commandCount = 0;
}

AST* create_ast(const char* name) {
    for (int i = 0; i < objectCount; i++) {
        if (strcmp(objects[i]->name, name) == 0) {
            return objects[i];
        }
    }

    AST* obj = (AST*)malloc(sizeof(AST));
    if (!obj) {
        perror("Memory allocation error");
        exit(EXIT_FAILURE);
    }
    strncpy(obj->name, name, sizeof(obj->name) - 1);
    obj->name[sizeof(obj->name) - 1] = '\0';
    obj->light_state = 0;
    obj->thermostat_temperature = 0;
    obj->conder = 0;
    if (objectCount < MAX_OBJECTS) {
        objects[objectCount++] = obj;
    } else {
        fprintf(stderr, "Maximum number of objects exceeded\n");
        free(obj);
        return NULL;
    }

    printf("Room %s successfully created with attributes:\n", obj->name);
    printf("Light state: %d\n", obj->light_state);
    printf("Temperature: %d degrees\n", obj->thermostat_temperature);
    printf("Conder state: %d\n", obj->conder);
    return obj;
}

void print_ast_state(AST* obj) {
    if (obj != NULL) {
        printf("Current state of room %s: Light=%d, Temperature=%d, Conder=%d\n", obj->name, obj->light_state, obj->thermostat_temperature, obj->conder);
    } else {
        printf("Error: Object is NULL\n");
    }
}

void light_on(AST* obj) {
    obj->light_state = 1;
    printf("%s: Light turned on!\n", obj->name);
}

void light_off(AST* obj) {
    obj->light_state = 0;
    printf("%s: Light turned off!\n", obj->name);
}

void set_temp(AST* obj, int temperature) {
    obj->thermostat_temperature = temperature;
    printf("%s: Temperature in the room set to %d degrees\n", obj->name, temperature);
}

void conder_on(AST* obj) {
    obj->conder = 1;
    printf("%s: Conder turned on\n", obj->name);
}

void conder_off(AST* obj) {
    obj->conder = 0;
    printf("%s: Conder turned off\n", obj->name);
}

void print_attribute(int attribute_value) {
    printf("%d\n", attribute_value);
}

int get_attribute_value(AST* obj, const char* attribute_name) {
    if (strcmp(attribute_name, "temperature") == 0) {
        return obj->thermostat_temperature;
    } else if (strcmp(attribute_name, "light_state") == 0) {
        return obj->light_state;
    }
}

void execute_method(AST* obj, const char* method_name, int argument) {
    if (strcmp(method_name, "light_on") == 0) {
        light_on(obj);
    } else if (strcmp(method_name, "light_off") == 0) {
        light_off(obj);
    } else if (strcmp(method_name, "conder_on") == 0) {
        conder_on(obj);
    } else if (strcmp(method_name, "conder_off") == 0) {
        conder_off(obj);
    } else if (strcmp(method_name, "status") == 0) {
        print_ast_state(obj);
    } else if (strcmp(method_name, "set_temperature") == 0) {
        set_temp(obj, argument);
    } else {
        fprintf(stderr, "Error: Unknown method '%s'\n", method_name);
        exit(EXIT_FAILURE);
    }
}

void if_else(Condition* condition, Block if_block, Block else_block) {
    if (if_block != NULL) {
        if_block();
    } else {
        fprintf(stderr, "Error: if_block is NULL\n");
        exit(EXIT_FAILURE);
    }

    if (else_block != NULL) {
        else_block();
    } else {
        fprintf(stderr, "Error: else_block is NULL\n");
        exit(EXIT_FAILURE);
    }
}

bool evaluate(const Condition* condition) {
    if (condition != NULL) {
        int left_value = condition->left_value;
        int right_value = condition->right_value;
        char* operator = condition->operator;
        if (operator == NULL) {
            printf("Operator is not specified\n");
            return false;
        }

        printf("Evaluating condition: %d %s %d\n", left_value, operator, right_value);

        if (strcmp(operator, ">") == 0) {
            return left_value > right_value;
        } else if (strcmp(operator, "<") == 0) {
            return left_value < right_value;
        } else if (strcmp(operator, "==") == 0) {
            return left_value == right_value;
        }
    }
    return false;
}

void free_condition(Condition* condition) {
    free(condition->attribute);
    free(condition->operator);
    free(condition);
}

Condition* create_condition(int left_value, char* operator, int right_value) {
    Condition* condition = (Condition*)malloc(sizeof(Condition));
    if (condition == NULL) {
        perror("Memory allocation error");
        exit(EXIT_FAILURE);
    }
    condition->left_value = left_value;
    condition->right_value = right_value;
    condition->operator = strdup(operator);
    return condition;
}

void execute_block_list(Block* block_list) {
    if (!block_list) {
        return;
    }

    while (*block_list != NULL) {
        (*block_list)();
        block_list++;
    }
}

CommandList* create_command_list() {
    CommandList* list = (CommandList*)malloc(sizeof(CommandList));
    if (list == NULL) {
        fprintf(stderr, "Error: Memory allocation failed for CommandList\n");
        exit(EXIT_FAILURE);
    }
    list->commands = NULL;
    list->count = 0;
    return list;
}

Condition* create_condition_attribute(AST* object, const char* attribute_name, char* operator, int right_value) {
    Condition* condition = (Condition*)malloc(sizeof(Condition));
    if (condition == NULL) {
        perror("Memory allocation error");
        exit(EXIT_FAILURE);
    }

    condition->object = object;
    condition->attribute = strdup(attribute_name);
    condition->operator = strdup(operator);
    condition->right_value = right_value;

    return condition;
}
