/*
Assignment:
HW3 - Parser and Code Generator for PL/0
Author(s): <Rohaan Mansoor>, <Nathan Miriello>
Language: C (only)
To Compile:
Scanner:
gcc -O2 -std=c11 -o lex lex.c
Parser/Code Generator:
gcc -O2 -std=c11 -o parsercodegen parsercodegen.c
To Execute (on Eustis):
./lex <input_file.txt>
./parsercodegen
where:
<input_file.txt> is the path to the PL/0 source program
Notes:
- lex.c accepts ONE command-line argument (input PL/0 source file)
- parsercodegen.c accepts NO command-line arguments
- Input filename is hard-coded in parsercodegen.c
- Implements recursive-descent parser for PL/0 grammar
- Generates PM/0 assembly code (see Appendix A for ISA)
- All development and testing performed on Eustis
Class: COP3402 - System Software - Fall 2025
Instructor: Dr. Jie Lin
Due Date: Friday, October 31, 2025 at 11:59 PM ET
*/

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <ctype.h>
#define MAX_TOKEN_LENGTH 6
#define MAX_TOKENS 100

typedef enum {
    skipsym = 1 , // Skip / ignore token
    identsym , // Identifier
    numbersym , // Number
    plussym , // +
    minussym , // -
    multsym , // *
    slashsym , // /
    eqsym , // =
    neqsym , // <>
    lessym , // <
    leqsym , // <=
    gtrsym , // >
    geqsym , // >=
    lparentsym , // (
    rparentsym , // )
    commasym , // ,
    semicolonsym , // ;
    periodsym , // .
    becomessym , // :=
    beginsym , // begin
    endsym , // end
    ifsym , // if
    fisym , // fi
    thensym , // then
    whilesym , // while
    dosym , // do
    callsym , // call
    constsym , // const
    varsym , // var
    procsym , // procedure
    writesym , // write
    readsym , // read
    elsesym , // else
    evensym // even
} TokenType ;

typedef enum {
    TOKEN_INT,
    TOKEN_STRING
} TOKEN_TYPE;

typedef struct {
    TOKEN_TYPE type;
    union {
        int intVal;
        char strVal[MAX_TOKEN_LENGTH];
    } val;
} TOKEN;

struct Symbol {
    int kind; // 1 = const, 2 = var, 3 = proc
    char name[MAX_TOKEN_LENGTH];
    int val;
    int level;
    int addr;
    int mark; // 0 = active, 1 = inactive
};

struct Command {
    char op[4];
    int l;
    int m;
};

void insertCommand(char op[4], int l, int m, struct Command OPR[MAX_TOKENS], int index) {
    strcpy(op, OPR[index].op);
    OPR[index].l = l;
    OPR[index].m = m;
}

void modifySymbol(struct Symbol symbolTable[MAX_TOKENS], char name[], int val, int addr, int mark) {
    for (int s = 0; s < MAX_TOKENS; s++) {
        if (strcmp(symbolTable[s].name, name) == 0) {
            symbolTable[s].val = val;
            symbolTable[s].addr = addr;
            symbolTable[s].mark = mark;
            break;
        }
    }
}

void insertSymbol(struct Symbol symbolTable[MAX_TOKENS], int kind, char name[], int val, int level, int addr, int mark) {
    for (int s = 0; s < MAX_TOKENS; s++) {
        if (symbolTable[s].mark == 1 || symbolTable[s].name[0] == '\0') {
            symbolTable[s].kind = kind;
            strncpy(symbolTable[s].name, name, MAX_TOKEN_LENGTH);
            symbolTable[s].val = val;
            symbolTable[s].level = level;
            symbolTable[s].addr = addr;
            symbolTable[s].mark = mark;
            break;
        }
    }
}

int alreadyExists(char name[], struct Symbol symbolTable[MAX_TOKENS]) {
    for (int i=0; i < MAX_TOKENS; i++) {
        if (strcmp(symbolTable[i].name, name) == 0 && symbolTable[i].mark == 0) {
            return i;
        }
    }
    return -1;
}

void factor(TOKEN tokens[], struct Command OPR[MAX_TOKENS], struct Symbol symbolTable[MAX_TOKENS]) {
    int i = 0;
    if (tokens[i].val.intVal == identsym) {
        int symbolIndex = alreadyExists(tokens[i].val.strVal, symbolTable);
        if (symbolIndex == -1) {
            printf("Error: undeclared identifier\n");
            return;
        }
        if (symbolTable[symbolIndex].kind == 1) {
            insertCommand("LIT", 0, symbolTable[symbolIndex].val, OPR, i);
        } else if (symbolTable[symbolIndex].kind == 2) {
            insertCommand("LOD", 0, symbolTable[symbolIndex].addr, OPR, i);
        } else {
            printf("Custom ! Error: cannot use procedure identifier in expression\n");
            return;
        }
        i++;
        return;
    }   else if (tokens[i].val.intVal == numbersym) {
        insertCommand("LIT", 0, tokens[i].val.intVal, OPR, i);
        i++;
        return;
    }   else if (tokens[i].val.intVal == lparentsym) {
        i++;
        // expression();
        if (tokens[i].val.intVal != rparentsym) {
            printf("Error: right parenthesis must follow left parenthesis\n");
            return;
        }
        i++;
        return;
    }   else {
        printf("Maybe !! Error: arithmetic equations must contain operands, parentheses, numbers, or symbols\n");
        return;
    }
}

void term(TOKEN tokens[], struct Command OPR[MAX_TOKENS], struct Symbol symbolTable[MAX_TOKENS]) {
    int i = 0;
    // factor();
    while (tokens[i].val.intVal == multsym || tokens[i].val.intVal == slashsym || tokens[i].val.intVal == evensym) {
        int op = tokens[i].val.intVal;
        i++;
        // factor();
        if (op == multsym) {
            insertCommand("OPR", 0, 3, OPR, i);
        } else if (op == slashsym) {
            insertCommand("OPR", 0, 4, OPR, i);
        } else {
            insertCommand("OPR", 0, 11, OPR, i);
        }
    }
}

void expression(TOKEN tokens[], struct Command OPR[MAX_TOKENS], struct Symbol symbolTable[MAX_TOKENS]) {
    int i = 0;
    if (tokens[i].val.intVal == minussym) {
        i++;
        // term();
        // NEG operation
        while (tokens[i].val.intVal == plussym || tokens[i].val.intVal == minussym) {
            int op = tokens[i].val.intVal;
            i++;
            // term();
            if (op == plussym) {
                insertCommand("OPR", 0, 1, OPR, i);
            } else {
                insertCommand("OPR", 0, 2, OPR, i);
            }
        }
    } else {
        if (tokens[i].val.intVal == plussym) {
            i++;
        }
        // term();
        while (tokens[i].val.intVal == plussym || tokens[i].val.intVal == minussym) {
            int op = tokens[i].val.intVal;
            i++;
            // term();
            if (op == plussym) {
                insertCommand("OPR", 0, 1, OPR, i);
            } else {
                insertCommand("OPR", 0, 2, OPR, i);
            }
        }
        insertCommand("OPR", 0, 1, OPR, i);
    }
}

void condition(TOKEN tokens[], struct Command OPR[MAX_TOKENS], struct Symbol symbolTable[MAX_TOKENS]) {
    int i = 0;
    if (tokens[i].val.intVal == evensym) {
        i++;
        // expression();
        insertCommand("OPR", 0, 11, OPR, i);
    } else {
        // expression();
        int code = tokens[i].val.intVal;
        i++;
        // expression();
        switch (code) {
            case eqsym:
                insertCommand("OPR", 0, 5, OPR, i);
                break;
            case neqsym:
                insertCommand("OPR", 0, 6, OPR, i);
                break;
            case lessym:
                insertCommand("OPR", 0, 7, OPR, i);
                break;
            case leqsym:
                insertCommand("OPR", 0, 8, OPR, i);
                break;
            case gtrsym:
                insertCommand("OPR", 0, 9, OPR, i);
                break;
            case geqsym:
                insertCommand("OPR", 0, 10, OPR, i);
                break;
            default:
                printf("Error: condition must contain comparison operator\n");
                return;
        }
    }
}

void statement(TOKEN tokens[], struct Command OPR[MAX_TOKENS], struct Symbol symbolTable[MAX_TOKENS]) {
    int i = 0;
    if (tokens[i].val.intVal == identsym) {
        int symbolIndex = alreadyExists(tokens[i].val.strVal, symbolTable);
        if (symbolIndex == -1) {
            printf("Error: undeclared identifier\n");
            return;
        }
        if (symbolTable[symbolIndex].kind != 2) {
            printf("Error: only variable values may be altered\n");
            return;
        }
        i++;
        // expression();
        insertCommand("STO", 0, symbolTable[symbolIndex].addr, OPR, i);
        return;
    }
    if (tokens[i].val.intVal == beginsym) {
        do {
            i++;
            // statement(tokens);
        }
        while (tokens[i].val.intVal == semicolonsym);
        if (tokens[i].val.intVal != endsym) {
            printf("Error: begin must be followed by end\n");
            return;
        }
        i++;
        return;
    }
    if (tokens[i].val.intVal == ifsym) {
        i++;
        // condition();
        int jpcIndex = i;
        insertCommand("JPC", 0, jpcIndex, OPR, i);
        if (tokens[i].val.intVal != thensym) {
            printf("Error: if must be followed by then\n");
            return;
        }
        i++;
        // statement();
        // code[jpcIndex].M = i;
        return;
    }
    if (tokens[i].val.intVal == whilesym) {
        i++;
        int loopIndex = i;
        // condition();
        if (tokens[i].val.intVal != dosym) {
            printf("Error: while must be followed by do\n");
            return;
        }
        i++;
        int jpcIndex = i;
        insertCommand("JPC", 0, jpcIndex, OPR, i);
        // statement();
        insertCommand("JMP", 0, loopIndex, OPR, i);
        // code[jpcIndex].M = i;
        return;
    }
    if (tokens[i].val.intVal == readsym) {
        i++;
        if (tokens[i].val.intVal != identsym) {
            printf("Error: const, var, and read keywords must be followed by identifier\n");
            return;
        }
        i++;
        insertCommand("SYS", 0, 3, OPR, i);
        insertCommand("STO", 0, symbolTable[i].addr, OPR, i);
        return;
    }
    if (tokens[i].val.intVal == writesym) {
        i++;
        //expression();
        insertCommand("SYS", 0, 1, OPR, i);
        return;
    }
}

int varDeclaration(TOKEN tokens[], struct Symbol symbolTable[MAX_TOKENS]) {
    int i = 0;
    char ident[MAX_TOKEN_LENGTH];
    int varCount = 0;
    // Retrieve token and verify if it's a var keyword
    if (tokens[i].val.intVal == varsym) {
        do {
            // Retrieve next token and verify if it's an identifier
            i++;
            if (tokens[i].val.intVal != identsym) {
                printf("Error: const, var, and read keywords must be followed by identifier\n");
                return -1;
            }
            // Check if identifier already exists in symbol table
            if (alreadyExists(tokens[i].val.strVal, symbolTable) != -1) {
                printf("Error: symbol name has already been declared\n");
                return -1;
            }
            // Store unique identifier
            strncpy(tokens[i].val.strVal, ident, MAX_TOKEN_LENGTH);
            // Add variable to symbol table
            insertSymbol(symbolTable, 2, ident, 0, 0, varCount + 2, 0);
            varCount++;
            // Retrieve next token
            i++;
        }

        // Check if it's a comma or semicolon
        while (tokens[i].val.intVal == commasym);

        if (tokens[i].val.intVal != semicolonsym) {
            printf("Error: constant and variable declarations must be followed by a semicolon\n");
            return -1;
        }

        i++;
    }
    return varCount;
}

void constDeclaration(TOKEN tokens[], struct Symbol symbolTable[MAX_TOKENS]) {
    int i = 0;
    char ident[MAX_TOKEN_LENGTH];

    do {
        // Retrieve token and verify if it's a const keyword
        if (tokens[i].val.intVal == constsym) {
            // Retrieve next token and verify if it's an identifier
            i++;
            if (tokens[i].val.intVal != identsym) {
                printf("Error: const, var, and read keywords must be followed by identifier\n");
                return;
            }
            // Check if identifier already exists in symbol table
            if (alreadyExists(tokens[i].val.strVal, symbolTable) != -1) {
                printf("Error: symbol name has already been declared\n");
                return;
            }
            // Store unique identifier
            strncpy(tokens[i].val.strVal, ident, MAX_TOKEN_LENGTH);
            // Retrieve next token and verify if it's =
            i++;
            if (tokens[i].val.intVal != eqsym) {
                printf("Error: constants must be assigned with =\n");
                return;
            }
            // Retrieve next token and verify if it's a number
            i++;
            if (tokens[i].val.intVal != numbersym) {
                printf("Error: constants must be assigned an integer value\n");
                return;
            }
            // Add constant to symbol table
            insertSymbol(symbolTable, 1, ident, tokens[i].val.intVal, 0, 0, 0);
            // Retrieve next token
            i++;
        }
    }
    // Check if it's a comma or semicolon
    while (tokens[i].val.intVal == commasym);

    if (tokens[i].val.intVal != semicolonsym) {
        printf("Error: constant and variable declarations must be followed by a semicolon\n");
        return;
    }
    i++;
}

void block(TOKEN tokens[], struct Command OPR[MAX_TOKENS], struct Symbol symbolTable[MAX_TOKENS]) {
    constDeclaration(tokens, symbolTable);
    int nums = varDeclaration(tokens, symbolTable);
    insertCommand("INC", 0, nums + 3, OPR, 0);
    statement(tokens, OPR, symbolTable);
}

void program(TOKEN tokens[], int size, struct Command OPR[MAX_TOKENS], struct Symbol symbolTable[MAX_TOKENS]) {
    block(tokens, OPR, symbolTable);
    if (tokens[size-2].val.intVal == numbersym || tokens[size-1].val.intVal != periodsym) {
        printf("Error: program must end with period\n");
        return;
    }
    int oprSize = sizeof(OPR) / sizeof(OPR[0]);
    insertCommand("SYS", 0, 3, OPR, oprSize - 1);
}

int main(void) {
    // Step 1: Read token list from lex.c
    FILE* inputFile = fopen("tokenlist.txt", "r");

    if (!inputFile) {
        printf(stderr, "Error opening token list file.\n");
        return 1;
    }

    TOKEN tokens[MAX_TOKENS];
    char buffer[MAX_TOKEN_LENGTH];
    int size = 0;

    while (fscanf(inputFile, "%s", buffer) != EOF && size < MAX_TOKENS) {
        if (isdigit((int) buffer[0])) {
            if (buffer[0] == skipsym) {
                printf("Error: Scanning error detected by lexer (skipsym present)");
                return 1;
            }
            tokens[size].type = TOKEN_INT;
            tokens[size].val.intVal = atoi(buffer);
        }   else {
            tokens[size].type = TOKEN_STRING;
            strncpy(tokens[size].val.strVal, buffer, MAX_TOKEN_LENGTH);
            tokens[size].val.strVal[MAX_TOKEN_LENGTH - 1] = '\0';
        }
        size++;
    }

    fclose(inputFile);

    // Step 2: Validate grammar

    struct Command OPR[MAX_TOKENS];

    struct Symbol symbolTable[MAX_TOKENS];

    program(tokens, size, OPR, symbolTable);

    // Step 3: Generate PM/0 assembly code

    printf("Assembly Code:\n\n");
    printf("Line\t OP   L   M\n");

    printf("  0\tJMP   0   3\n");

    for (int k = 0; k < MAX_TOKENS; k++) {
        if (strcmp(OPR[k].op, "") != 0) {
            printf("%3d\t%s\t%d\t%d\n", k + 1, OPR[k].op, OPR[k].l, OPR[k].m);
        }   else {
            break;
        }
    }

    // Step 4: Generate Symbol Table

    printf("\nSymbol Table:\n\n");
    printf("Kind | Name       | Value | Level | Address | Mark\n");
    for (int z = 0; z < 50; z++) {
        printf("-");
    }
    printf("\n");
    // Placeholder for symbol table entries

    for (int l = 0; l < MAX_TOKENS; l++) {
        if (strcmp(symbolTable[l].name, "") != 0) {
            printf("%4d | \t\t%s | \t%d | \t%d | \t%d | \t%d\n", symbolTable[l].kind, symbolTable[l].name, symbolTable[l].val, symbolTable[l].level, symbolTable[l].addr, symbolTable[l].mark);
        }   else {
            break;
        }
    }

    return 0;
}
