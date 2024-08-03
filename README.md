![image](https://github.com/user-attachments/assets/937876a1-698d-4dbb-8664-a8590b3b7f66)
# Overview
This programming language is based on a stack-based push-down automaton. It includes commands for stack manipulation, arithmetic operations, control flow, and more.
## HOW to RUN 
compile interpreter.c
``cd "c:\Users\path\to\sourcecode" ; if ($?) { gcc interpreter.c -o interpreter } ; if ($?) { .\interpreter }``
then use the shell
try
``:->RUN example.blu``
## Commands

### Stack Operations

- **INITSTACK `<stack_name>`**
  - Initializes a new stack with the given name.
- **CURRENTSTACK `<stack_name>`**
  - Sets the current stack to the specified stack.
- **PUSH `<number>`**
  - Pushes a number onto the current stack.
- **POP**
  - Pops the top value from the current stack.
- **TOP**
  - Prints the top value of the current stack without popping it.
- **SWAP**
  - Swaps the two top values of the current stack.
- **DUP**
  - Duplicates the top value of the current stack.
- **DUP2**
  - Duplicates the top two values of the current stack.
- **COPY `<from_stack_name>`**
  - Copies the top value from the specified stack to the current stack.
- **DELETESTACK `<stack_name>`**
  - Deletes the specified stack.
- **FLUSH**
  - Clears all values from the current stack.
- **8=D**
  - Prints all stacks and their contents.

### Arithmetic Operations

- **ADD**
  - Pops the top two values, adds them, and pushes the result.
- **SUB**
  - Pops the top two values, subtracts the first popped value from the second, and pushes the result.
- **MUL**
  - Pops the top two values, multiplies them, and pushes the result.
- **DIV**
  - Pops the top two values, divides the second popped value by the first, and pushes the result. Checks for division by zero.
- **MOD**
  - Pops the top two values, calculates the modulus of the second popped value by the first, and pushes the result. Checks for division by zero.

### Control Flow

- **JUMP `<label>`**
  - Jumps to the specified label.
- **JUMP.=0 `<label>`**
  - Jumps to the specified label if the top value of the stack is zero.
- **JUMP.>0 `<label>`**
  - Jumps to the specified label if the top value of the stack is greater than zero.
- **JUMP.<0 `<label>`**
  - Jumps to the specified label if the top value of the stack is less than zero.
- **HALT**
  - Stops the execution of the program.

### I/O Operations

- **PRINT `<message>`**
  - Prints the specified message.
- **READ**
  - Reads an integer from the input and pushes it onto the current stack.

## Example Programs

### Hello World

```plaintext
PRINT "Hello, World!"
HALT
```

# Error Handling

- **Stack Overflow**: Error when trying to push onto a full stack.
- **Stack Underflow**: Error when trying to pop from an empty stack.
- **Invalid Stack Index**: Error when accessing a non-existent stack.
- **Divide by Zero**: Error when performing a division or modulus operation with zero.

# Built-in Functions

- **create_stack**: Creates a new stack.
- **find_stack_by_name**: Finds a stack by name.
- **set_current_stack**: Sets the current stack.
- **push**: Pushes a value onto the current stack.
- **pop**: Pops a value from the current stack.
- **top**: Retrieves the top value of the current stack.
- **copy_stack**: Copies a value from one stack to another.
- **delete_stack**: Deletes a specified stack.
- **get_label_address**: Gets the address of a label in the program.
- **interpret**: Interprets and executes the program.



# License
no license ..uhh.. do whatever u want
