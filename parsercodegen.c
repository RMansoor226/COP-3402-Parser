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
#define MAX_TOKEN_LENGTH 11
#define MAX_TOKENS 100

// Token Data Type

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



typedef struct {
    TokenType type;
    char lexeme[MAX_TOKEN_LENGTH];
    int val;
} Token;

Token tokens[MAX_TOKENS];
int currentToken = 0;
int tokenCount = 0;

// Symbol Table Data Type

typedef struct  {
    int kind; // 1 = const, 2 = var, 3 = proc
    char name[MAX_TOKEN_LENGTH];
    int val;
    int level;
    int addr;
    int mark; // 0 = active, 1 = inactive
} Symbol;

Symbol symbolTable[MAX_TOKENS];
int symbolCount = 0;

// Assembly Instruction Data Type

typedef struct {
    char op[4];
    int l;
    int m;
} Command;

Command OPR[MAX_TOKENS];
int codeIndex = 0;

// Primary Function Prototypes for parsing and code generation

void factor();
void term();
void expression();
void condition();
void statement();
int varDeclaration();
void constDeclaration();
void program();

// Helper Function Prototypes

void insertCommand(char op[4], int l, int m);
void printError(char message[]);
int searchSymbol(char name[]);
void insertSymbol( int kind, char name[], int val, int addr);
Token* current();
void nextToken();
int convertCommandToCode(char op[4]);

void insertCommand(char op[4], int l, int m) {
    strcpy(OPR[codeIndex].op, op);
    OPR[codeIndex].l = l;
    OPR[codeIndex].m = m;
    codeIndex++;
}

void printError(char message[]) {
    printf("Error: %s\n", message);
    FILE *elf = fopen("elf.txt", "w");
    if (elf) {
        fprintf(elf, "Error: %s\n", message);
        fclose(elf);
    }
    exit(1);
}

int searchSymbol(char name[]) {
    for (int i = 0; i < symbolCount; i++) {
        if (!strcmp(symbolTable[i].name, name) && !symbolTable[i].mark) {
            return i;
        }
    }
    return -1;
}

void insertSymbol(int kind, char name[], int val, int addr) {
    if (searchSymbol(name) != -1) {
        printf("Error: symbol name has already been declared\n");
        return;
    }
    symbolTable[symbolCount].kind = kind;
    strncpy(symbolTable[symbolCount].name, name, MAX_TOKEN_LENGTH);
    symbolTable[symbolCount].val = val;
    symbolTable[symbolCount].level = 0;
    symbolTable[symbolCount].addr = addr;
    symbolTable[symbolCount].mark = 0;
    symbolCount++;
}

Token* current() {
    return &tokens[currentToken];
}

void nextToken() {
    if (currentToken < tokenCount - 1)
        currentToken++;
}

int convertCommandToCode(char op[4]) {
    if (strcmp(op, "LIT") == 0) return 1;
    if (strcmp(op, "OPR") == 0) return 2;
    if (strcmp(op, "LOD") == 0) return 3;
    if (strcmp(op, "STO") == 0) return 4;
    if (strcmp(op, "CAL") == 0) return 5;
    if (strcmp(op, "INC") == 0) return 6;
    if (strcmp(op, "JMP") == 0) return 7;
    if (strcmp(op, "JPC") == 0) return 8;
    if (strcmp(op, "SYS") == 0) return 9;
    return -1; // Invalid operation
}

void factor() {
    if (current()->type == identsym) {
        int symbolIndex = searchSymbol(current()->lexeme);

        if (symbolIndex == -1)
            printError("undeclared identifier\n");

        if (symbolTable[symbolIndex].kind == 1)
            insertCommand("LIT", 0, symbolTable[symbolIndex].val);
        else
            insertCommand("LOD", 0, symbolTable[symbolIndex].addr);


        nextToken();
    }   else if (current()->type == numbersym) {
        insertCommand("LIT", 0, current()->val);
        nextToken();
    }   else if (current()->type == lparentsym) {
        nextToken();
        expression();
        if (current()->type != rparentsym)
            printError("right parenthesis must follow left parenthesis\n");
        nextToken();
    }   else {
        printError("arithmetic equations must contain operands, parentheses, numbers, or symbols\n");
    }
}

void term() {
    factor();
    while (current()->type == multsym || current()->type == slashsym) {
        int op = current()->type;
        nextToken();
        factor();

        if (op == multsym)
            insertCommand("OPR", 0, 3);
        else
            insertCommand("OPR", 0, 4);
    }
}

void expression() {
    int sign = 0;

    if (current()->type == minussym || current()->type == plussym) {
        sign = (current()->type == minussym) ? 1: 0;
        nextToken();
    }

    term();
    if (sign)
        insertCommand("OPR", 0, 2);

    while (current()->type == plussym || current()->type == minussym) {
        int op = current()->type;
        nextToken();
        term();

        if (op == plussym)
            insertCommand("OPR", 0, 1);
        else
            insertCommand("OPR", 0, 2);
    }
}

void condition() {
    if (current()->type == evensym) {
        nextToken();
        expression();
        insertCommand("OPR", 0, 11);
    } else {
        expression();
        int code = current()->type;

        if (!(code >= eqsym && code <= geqsym))
            printError("condition must contain comparison operator\n");

        nextToken();
        expression();
        insertCommand("OPR", 0, code - eqsym + 5);
    }
}

void statement() {
    if (current()->type == identsym) {
        int symbolIndex = searchSymbol(current()->lexeme);

        if (symbolIndex == -1)
            printError("undeclared identifier\n");

        if (symbolTable[symbolIndex].kind != 2)
            printError("only variable values may be altered\n");

        nextToken();
        if (current()->type != becomessym)
            printError("assignment statements must use ':='\n");

        nextToken();
        expression();

        insertCommand("STO", 0, symbolTable[symbolIndex].addr);
    }   else if (current()->type == beginsym) {
        do {
            nextToken();
            statement();
        }
        while (current()->type == semicolonsym);

        if (current()->type != endsym)
            printError("begin must be followed by end\n");
        nextToken();
    }   else if (current()->type == ifsym) {
        nextToken();
        condition();

        if (current()->type != thensym)
            printError("if must be followed by then\n");
        nextToken();

        int jpcIndex = codeIndex;
        insertCommand("JPC", 0, 0);

        // Parse body of if-statement
        statement();

        // Check if the else keyword is present
        if (current()->type == elsesym)    {
            int jmpIndex = codeIndex;
            insertCommand("JMP", 0, 0);
            OPR[jpcIndex].m = 3*codeIndex;
            nextToken();
            statement(); // Parse else body
            OPR[jmpIndex].m = 3*codeIndex;
        }   else {
            // Check if if-statement is terminated by fi keyword
            if (current()->type != fisym)
                printError("if must end in fi\n");
            OPR[jpcIndex].m = 3*codeIndex;
        }

        nextToken();

    }   else if (current()->type == whilesym) {
        int loopIndex = codeIndex;
        nextToken();
        condition();

        if (current()->type != dosym)
            printError("while must be followed by do\n");
        nextToken();

        int jpcIndex = codeIndex;
        insertCommand("JPC", 0, 0);

        statement();

        insertCommand("JMP", 0, loopIndex);

        OPR[jpcIndex].m = 3*codeIndex;
    }   else if (current()->type == readsym) {
        nextToken();
        if (current()->type != identsym)
            printf("const, var, and read keywords must be followed by identifier\n");

        int i = searchSymbol(current()->lexeme);

        if (i == -1)
            printError("undeclared identifier\n");

        if (symbolTable[i].kind != 2)
            printError("only variable values may be altered\n");

        insertCommand("SYS", 0, 2);
        insertCommand("STO", 0, symbolTable[i].addr);
        nextToken();
    }   else if (current()->type == writesym) {
        nextToken();
        expression();
        insertCommand("SYS", 0, 1);
    }
}

int varDeclaration() {
    int varCount = 0;

    // Retrieve token and verify if it's a var keyword
    if (current()->type == varsym) {
        do {
            // Retrieve next token and verify if it's an identifier
            nextToken();
            if (current()->type != identsym)
                printError("const, var, and read keywords must be followed by identifier\n");

            // Check if identifier already exists in symbol table
            if (searchSymbol(current()->lexeme) != -1) {
                printError("symbol name has already been declared\n");
            }

            // Add variable to symbol table
            varCount++;
            insertSymbol(2, current()->lexeme, 0, varCount + 2);

            // Retrieve next token
            nextToken();
        }

        // Check if it's a comma or semicolon
        while (current()->type == commasym);

        if (current()->type != semicolonsym)
            printError("constant and variable declarations must be followed by a semicolon\n");
        nextToken();
    }
    return varCount;
}

void constDeclaration() {
    // Retrieve token and verify if it's a const keyword
    if (current()->type == constsym) {
        do {
            // Retrieve next token and verify if it's an identifier
            nextToken();
            if (current()->type != identsym)
                printError("const, var, and read keywords must be followed by identifier\n");

            // Check if identifier already exists in symbol table
            if (searchSymbol(current()->lexeme) != -1)
                printError("symbol name has already been declared\n");

            char ident[MAX_TOKEN_LENGTH];
            strcpy(ident, current()->lexeme);

            // Retrieve next token and verify if it's =
            nextToken();
            if (current()->type != eqsym)
                printError("constants must be assigned with =\n");

            // Retrieve next token and verify if it's a number
            nextToken();
            if (current()->type != numbersym)
                printError("constants must be assigned an integer value\n");

            // Add constant to symbol table
            insertSymbol( 1, ident, current()->val, 0);

            // Retrieve next token
            nextToken();
        }
        // Check if it's a comma or semicolon
        while (current()->type == commasym);

        if (current()->type != semicolonsym)
            printError("constant and variable declarations must be followed by a semicolon\n");
        nextToken();
    }

}

void block() {
    int blockStart = symbolCount;

    constDeclaration();
    int nums = varDeclaration();
    insertCommand("INC", 0, nums + 3);\

    statement();

    for (int i = blockStart; i < symbolCount; i++) {
        symbolTable[i].mark = 1;
    }
}

void program() {
    block();
    if (current()->type != periodsym) {
        printError("program must end with period\n");
    }
    insertCommand("SYS", 0, 3);
}

int main(void) {
    // Step 1: Read token list from lex.c
    FILE* inputFile = fopen("tokenlist.txt", "r");

    if (!inputFile) {
        printf(stderr, "Error: failed opening token list file.\n");
        return 1;
    }

    memset(tokens, 0, sizeof(tokens));
    memset(symbolTable, 0, sizeof(symbolTable));
    memset(OPR, 0, sizeof(OPR));

    while (fscanf(inputFile, "%u", &tokens[tokenCount].type) != EOF && tokenCount < MAX_TOKENS) {
        if (tokens[tokenCount].type == skipsym)
            printError("Scanning error detected by lexer (skipsym present)");
        if (tokens[tokenCount].type == identsym || tokens[tokenCount].type == numbersym) {
            fscanf(inputFile, "%s", tokens[tokenCount].lexeme);
            if (tokens[tokenCount].type == numbersym)
                tokens[tokenCount].val = atoi(tokens[tokenCount].lexeme);
        }
        tokenCount++;
    }

    fclose(inputFile);

    currentToken = 0;

    // Step 2: Validate grammar

    insertCommand("JMP", 0, 3);
    program();

    // Step 3: Generate PM/0 assembly code

    printf("Assembly Code:\n\n");
    printf("Line\t OP   L   M\n");

    FILE* elf = fopen("elf.txt", "w");

    for (int i = 0; i < codeIndex; i++) {
        if (strcmp(OPR[i].op, "") != 0) {
            printf("%3d%8s%4d%4d\n", i, OPR[i].op, OPR[i].l, OPR[i].m);
            fprintf(elf, "%d %d %d\n", convertCommandToCode(OPR[i].op), OPR[i].l, OPR[i].m);
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

    for (int i = 0; i < MAX_TOKENS; i++) {
        if (symbolTable[i].kind == 0)
            break;
        printf("%4d | \t\t%s | \t%d | \t%d | \t  %d | \t%d\n", symbolTable[i].kind, symbolTable[i].name, symbolTable[i].val, symbolTable[i].level, symbolTable[i].addr, symbolTable[i].mark);
    }

    return 0;
}
