#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <setjmp.h>
//---------------------------
#define MAX_PROGRAM_SIZE 1000
#define STACK_SIZE 256
#define MAX_LABELS 100
#define MAX_STACKS 10
#define MAX_STACK_NAME 20
//------------------------------------------------------------------------------------------------------------------------------------
jmp_buf error_jmp_buf;

typedef struct {
    char name[MAX_STACK_NAME];
    int buf[STACK_SIZE];
    int sp;
} Stack;

typedef struct {
    Stack stacks[MAX_STACKS];
    int stack_count;
    int current_stack;
} StackManager;


void log_error(const char* message) {
    fprintf(stderr, "Error: %s\n", message);
    longjmp(error_jmp_buf, 1);
}
//----------------------------------------------------------------------------------------------------------------------------------------
int create_stack(StackManager* sm, const char* name);
void init_stack_manager(StackManager* sm) {
    sm->stack_count = 0;
    sm->current_stack = -1;
    int default_stack = create_stack(sm, "main");
    sm->current_stack = default_stack;
}
//-----------------------------------------------------------------------------------------------------------------------------------------
int create_stack(StackManager* sm, const char* name) {
    if (sm->stack_count >= MAX_STACKS) {
        log_error("Maximum number of stacks reached");
        return -1;
    }
    int index = sm->stack_count++;
    strncpy(sm->stacks[index].name, name, MAX_STACK_NAME - 1);
    sm->stacks[index].name[MAX_STACK_NAME - 1] = '\0';
    sm->stacks[index].sp = -1;
    return index;
}

int find_stack_by_name(StackManager* sm, const char* name) {
    for (int i = 0; i < sm->stack_count; i++) {
        if (strcmp(sm->stacks[i].name, name) == 0) {
            return i;
        }
    }
    return -1;
}

void set_current_stack(StackManager* sm, int index) {
    if (index >= 0 && index < sm->stack_count) {
        sm->current_stack = index;
    } else {
        log_error("Invalid stack index");
    }
}

void push(StackManager* sm, int number) {
    Stack* stack = &sm->stacks[sm->current_stack];
    if (stack->sp < STACK_SIZE - 1) {
        stack->buf[++stack->sp] = number;
    } else {
        log_error("Stack overflow");
    }
}

int pop(StackManager* sm) {
    Stack* stack = &sm->stacks[sm->current_stack];
    if (stack->sp >= 0) {
        int value = stack->buf[stack->sp--];
        // printf("Popped %d from stack %s\n", value, stack->name);
        return value;
    } else {
        char error_msg[100];
        snprintf(error_msg, sizeof(error_msg), "Stack underflow on stack %s", stack->name);
        log_error(error_msg);
        return 0; 
    }
}

int top(StackManager* sm) {
    Stack* stack = &sm->stacks[sm->current_stack];
    if (stack->sp >= 0) {
        return stack->buf[stack->sp];
    } else {
        char error_msg[100];
        snprintf(error_msg, sizeof(error_msg), "Stack %s is empty", stack->name);
        log_error(error_msg);
        return 0; 
    }
}

void copy_stack(StackManager* sm, int from_index, int to_index) {
    if (from_index < 0 || from_index >= sm->stack_count || to_index < 0 || to_index >= sm->stack_count) {
        log_error("Invalid stack index for copy operation");
        return;
    }
    Stack* from_stack = &sm->stacks[from_index];
    
    if (from_stack->sp >= 0) {
        int current_stack = sm->current_stack;
        sm->current_stack = to_index;
        push(sm, from_stack->buf[from_stack->sp]);
        sm->current_stack = current_stack;
    } else {
        log_error("Cannot copy from empty stack");
    }
}

void delete_stack(StackManager* sm, int index) {
    if (index < 0 || index >= sm->stack_count) {
        log_error("Invalid stack index for deletion");
        return;
    }
    for (int i = index; i < sm->stack_count - 1; i++) {
        sm->stacks[i] = sm->stacks[i + 1];
    }
    sm->stack_count--;
    if (sm->current_stack >= index) {
        sm->current_stack--;
    }
    if (sm->stack_count == 0) {
        sm->current_stack = create_stack(sm, "default");
    } else if (sm->current_stack < 0) {
        sm->current_stack = 0;
    }
}
//---------------------------------------------------------------------------------------------------------------------------
int get_label_address(char* labels[], int label_addresses[], int label_count, char* label) {
    for (int i = 0; i < label_count; i++) {
        if (strcmp(labels[i], label) == 0) {
            return label_addresses[i];
        }
    }
    return -1;
}
//---------------------------------------------------------------------------------------------------------------------------
void interpret(char* program[], int program_size, char* labels[], int label_addresses[], int label_count, StackManager* sm) {
    int pc = 0;

    while (pc < program_size && strcmp(program[pc], "HALT") != 0) {
        char* opcode = program[pc++];

        if (strcmp(opcode, "INITSTACK") == 0) {
            char* stack_name = program[pc++];
            create_stack(sm, stack_name);
        } else if (strcmp(opcode, "CURRENTSTACK") == 0) {
            char* stack_name = program[pc++];
            int index = find_stack_by_name(sm, stack_name);
            if (index >= 0) {
                set_current_stack(sm, index);
            } else {
                log_error("Stack not found");
                continue;
            }
        } else if (strcmp(opcode, "PUSH") == 0) {
            int number = atoi(program[pc++]);
            push(sm, number);
        } else if (strcmp(opcode, "POP") == 0) {
            pop(sm);
        } else if (strcmp(opcode, "SWAP") == 0) {
            int a = pop(sm);
            int b = pop(sm);
            push(sm, a);
            push(sm, b);
        } else if (strcmp(opcode, "ADD") == 0) {
            int a = pop(sm);
            int b = pop(sm);
            push(sm, a + b);
        } else if (strcmp(opcode, "MUL") == 0) {
            int a = pop(sm);
            int b = pop(sm);
            push(sm, a * b);
        } else if (strcmp(opcode, "SUB") == 0) {
            int a = pop(sm);
            int b = pop(sm);
            push(sm, b - a);
        } else if (strcmp(opcode, "DIV") == 0) {
            int a = pop(sm);
            int b = pop(sm);
            if (a != 0) {
                push(sm, b / a);
            } else {
                log_error("Divide by 0 error");
                continue;
            }
        } else if (strcmp(opcode, "MOD") == 0) {
            int a = pop(sm);
            int b = pop(sm);
            if (a != 0) {
                push(sm, b % a);
            } else {
                log_error("Divide by 0 error");
                continue;
            }
        } else if (strcmp(opcode, "DUP") == 0) {
            push(sm, top(sm));
        } else if (strcmp(opcode, "DUP2") == 0) {
            int a = pop(sm);
            int b = pop(sm);
            push(sm, b);
            push(sm, a);
            push(sm, b);
            push(sm, a);
        } else if (strcmp(opcode, "PRINT") == 0) {
            char* str = program[pc++];
            for (char* p = str; *p != '\0'; p++) {
                if (*p == '\\' && *(p + 1) == 'n') {
                    putchar('\n');
                    p++;
                } else if (*p == '\\' && *(p + 1) == 't') {
                    putchar('\t');
                    p++;
                } else {
                    putchar(*p);
                }
            }
        } else if (strcmp(opcode, "TOP") == 0) {
            printf("%d\n", top(sm));

        } else if (strcmp(opcode, "READ") == 0) {
            int number;
            if (scanf("%d", &number) == 1) {
                push(sm, number);
            } else {
                log_error("Invalid input");
                int c;
                while ((c = getchar()) != '\n' && c != EOF) { }
                continue;
            }
        } else if (strcmp(opcode, "JUMP.=0") == 0) {
            char* label = program[pc++];
            int number = top(sm);
            if (number == 0) {
                int new_pc = get_label_address(labels, label_addresses, label_count, label);
                if (new_pc != -1) {
                    pc = new_pc;
                } else {
                    log_error("Label not found");
                    continue;
                }
            }
        } else if (strcmp(opcode, "JUMP.>0") == 0) {
            char* label = program[pc++];
            int number = top(sm);
            if (number > 0) {
                int new_pc = get_label_address(labels, label_addresses, label_count, label);
                if (new_pc != -1) {
                    pc = new_pc;
                } else {
                    log_error("Label not found");
                    continue;
                }
            }
        } else if (strcmp(opcode, "JUMP.<0") == 0) {
            char* label = program[pc++];
            int number = top(sm);
            if (number < 0) {
                int new_pc = get_label_address(labels, label_addresses, label_count, label);
                if (new_pc != -1) {
                    pc = new_pc;
                } else {
                    log_error("Label not found");
                    continue;
                }
            }
        } else if (strcmp(opcode, "JUMP") == 0) {
            char* label = program[pc++];
            int new_pc = get_label_address(labels, label_addresses, label_count, label);
            if (new_pc != -1) {
                pc = new_pc;
            } else {
                log_error("Label not found");
                continue;
            }
        } else if (strcmp(opcode, "COPY") == 0) {
            char* from_stack_name = program[pc++];
            int from_index = find_stack_by_name(sm, from_stack_name);
            if (from_index >= 0) {
                copy_stack(sm, from_index, sm->current_stack);
            } else {
                log_error("Stack not found");
                continue;
            }
        } else if (strcmp(opcode, "DELETESTACK") == 0) {
            char* stack_name = program[pc++];
            int index = find_stack_by_name(sm, stack_name);
            if (index >= 0) {
                delete_stack(sm, index);
            } else {
                log_error("Stack not found");
                continue;
            }
        } else if (strcmp(opcode, "FLUSH") == 0) {
            Stack* current_stack = &sm->stacks[sm->current_stack];
            current_stack->sp = -1;
        } else if (strcmp(opcode, "8=D") == 0) {
            for (int i = 0; i < sm->stack_count; i++) {
                printf("Stack %s: \n", sm->stacks[i].name);
                for (int j = 0; j <= sm->stacks[i].sp; j++) {
                    printf("[_%d_]\n", sm->stacks[i].buf[(sm->stacks[i].sp)-j]);
                }
                printf("\n");
            }
        } else {
            char error_msg[100];
            snprintf(error_msg, sizeof(error_msg), "Unexpected opcode received: %s", opcode);
            log_error(error_msg);
            continue;
        }
    }
}
//------------------------------------------------------------------------------------------------------------------------------
void RUN(char* file_name) {
    FILE* file = fopen(file_name, "r");
    if (!file) {
        log_error("Could not open file");
        return;
    }

    char* program[MAX_PROGRAM_SIZE];
    char* labels[MAX_LABELS];
    int label_addresses[MAX_LABELS];
    int program_size = 0;
    int label_count = 0;
    char line[256];
    int token_counter = 0;

    while (fgets(line, sizeof(line), file)) {
        char* opcode = strtok(line, " \t\n");

        if (opcode == NULL) {
            continue;
        }

        if (opcode[strlen(opcode) - 1] == ':') {
            opcode[strlen(opcode) - 1] = '\0';
            labels[label_count] = strdup(opcode);
            label_addresses[label_count] = token_counter;
            label_count++;
            continue;
        }

        program[token_counter++] = strdup(opcode);

        if (strcmp(opcode, "PUSH") == 0 || strcmp(opcode, "CURRENTSTACK") == 0 || 
            strcmp(opcode, "COPY") == 0 || strcmp(opcode, "DELETESTACK") == 0 ||
            strcmp(opcode, "INITSTACK") == 0) {
            char* arg = strtok(NULL, " \t\n");
            if (arg) {
                program[token_counter++] = strdup(arg);
            }
        } else if (strcmp(opcode, "PRINT") == 0) {
            char* string_literal = strtok(NULL, "\"");
            if (string_literal) {
                program[token_counter++] = strdup(string_literal);
            }
        } else if (strcmp(opcode, "JUMP.=0") == 0 || strcmp(opcode, "JUMP.>0") == 0 || 
                   strcmp(opcode, "JUMP.<0") == 0 || strcmp(opcode, "JUMP") == 0) {
            char* label = strtok(NULL, " \t\n");
            if (label) {
                program[token_counter++] = strdup(label);
            }
        }
    }
    fclose(file);

    StackManager sm;
    init_stack_manager(&sm);
    interpret(program, token_counter, labels, label_addresses, label_count, &sm);

    for (int i = 0; i < token_counter; i++) {
        free(program[i]);
    }
    for (int i = 0; i < label_count; i++) {
        free(labels[i]);
    }

    int c;
    while ((c = getchar()) != '\n' && c != EOF) { }
}
//------------------------------------------------------------------------------------------------------------------------------
int main() {
    char input[256];
    char* program[MAX_PROGRAM_SIZE];
    int token_counter = 0;

    char* labels[MAX_LABELS];
    int label_addresses[MAX_LABELS];
    int label_count = 0;

    StackManager sm;
    init_stack_manager(&sm);
    system("cls");
    printf(" ___ _   _   _ ___ ___ _____ _   ___ _  __\n");
    printf("| _ ) | | | | | __/ __|_   _/_\\ / __| |/ /\n");
    printf("| _ \\ |_| |_| | _|\\__ \\ | |/ _ \\ (__| ' < \n");
    printf("|___/____\\___/|___|___/ |_/_/ \\_\\___|_|\\ \\\n");
    printf("version-0.0.1                           \\_\\ \n\n");

    while (1) {
        printf("\033[34;1m:->\033[0m");
        if (fgets(input, sizeof(input), stdin) == NULL) {
            break;
        }

        input[strcspn(input, "\n")] = 0;

        if (strcmp(input, "EXIT") == 0) {
            break;
        }
        if (strcmp(input, "HELP") == 0) {
            FILE *file = fopen("help.txt", "r");
            if (file == NULL) {
                perror("Failed to open help file");
                continue;
            }

            char line[256];
            printf("   _____________________________________\n");
            while (fgets(line, sizeof(line), file) != NULL) {
            printf("  |  %s", line);
            }
            fclose(file);
            printf("\n   _____________________________________\n");
            continue;
        }

        if (setjmp(error_jmp_buf) == 0) {
            if (strncmp(input, "RUN ", 4) == 0) {
                char* file_name = input + 4;
                if (strlen(file_name) > 0) {
                    RUN(file_name);
                } else {
                    log_error("No file name provided for RUN command");
                }
            } else if (strncmp(input, "PRINT ", 6) == 0) {
                char* str = input + 6;
                for (char* p = str; *p != '\0'; p++) {
                    if (*p == '\\' && *(p + 1) == 'n') {
                        putchar('\n');
                        p++;
                    } else if (*p == '\\' && *(p + 1) == 't') {
                        putchar('\t');
                        p++;
                    } else {
                        putchar(*p);
                    }
                }
                printf("\n");
            } else if (strncmp(input, "CLS", 3) == 0) {
                system("cls");
            } else {
                token_counter = 0;
                char* token = strtok(input, " ");
                while (token != NULL && token_counter < MAX_PROGRAM_SIZE) {
                    program[token_counter++] = strdup(token);
                    token = strtok(NULL, " ");
                }

                if (token_counter > 0) {
                    interpret(program, token_counter, labels, label_addresses, label_count, &sm);

                    for (int i = 0; i < token_counter; i++) {
                        free(program[i]);
                    }
                }
            }
        } else {

            continue;
        }
    }
    return 0;
}