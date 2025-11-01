//
// Created by Rohaan Mansoor on 10/29/2025.
//

/*
Assignment:
lex - Lexical Analyzer for PL/0
Author: <Rohaan Mansoor, Nathan Miriello>
Language: C (only)
To Compile:
gcc -O2 -std=c11 -o lex lex.c
To Execute (on Eustis):
./lex <input file>
where:
<input file> is the path to the PL/0 source program
Notes:
- Implement a lexical analyser for the PL/0 language.
- The program must detect errors such as
- numbers longer than five digits
- identifiers longer than eleven characters
- invalid characters.
- The output format must exactly match the specification.
- Tested on Eustis.
Class: COP3402 - System Software - Fall 2025
Instructor: Dr. Jie Lin
Due Date: Friday, October 3, 2025 at 11:59 PM ET
*/

#include <stdio.h>
#include <string.h>
#include <ctype.h>

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

int main(int argc, char* argv[]) {
    FILE *inputFile = fopen(argv[1], "r");

    // Check if input file opened successfully
    if (!inputFile) {
        printf("Error opening file.\n");
        return 1;
    }

    fseek(inputFile, 0, SEEK_SET);

    char lexemes[1000][20];
    int tokens[1000];
    int t = 0;

    int c;
    while ((c = fgetc(inputFile)) != EOF) {
        // Skip whitespace
        if (isspace(c)) {
            continue;
        }

        // Handle comments
        if (c == '/') {
            int next = fgetc(inputFile);
            if (next == '/') {
                // Single-line comment - skip until newline
                while ((c = fgetc(inputFile)) != EOF && c != '\n');
                continue;
            } else if (next == '*') {
                // Multi-line comment - skip until */
                int prev = 0;
                while ((c = fgetc(inputFile)) != EOF) {
                    if (prev == '*' && c == '/') {
                        break;
                    }
                    prev = c;
                }
                continue;
            } else {
                // Classify as division symbol if not a comment
                ungetc(next, inputFile);
                tokens[t] = slashsym;
                strcpy(lexemes[t], "/");
                t++;
                continue;
            }
        }

        // Handle identifiers and keywords
        if (isalpha(c)) {
            int len = 0;
            char buffer[20];
            memset(buffer, 0, sizeof(buffer));
            
            while (isalnum(c)) {
                if (len < 11) {
                    buffer[len++] = c;
                } else {
                    // Continue reading but mark as an error via skipsym
                    len++;
                }
                c = fgetc(inputFile);
            }
            ungetc(c, inputFile); // Put back the non-alphanumeric character
            buffer[len < 11 ? len : 11] = '\0';

            if (len > 11) {
                strcpy(lexemes[t], buffer);
                tokens[t] = skipsym;
                t++;
                continue;
            }

            // Check for keywords
            if (strcmp(buffer, "begin") == 0) {
                tokens[t] = beginsym;
            } else if (strcmp(buffer, "end") == 0) {
                tokens[t] = endsym;
            } else if (strcmp(buffer, "if") == 0) {
                tokens[t] = ifsym;
            } else if (strcmp(buffer, "fi") == 0) {
                tokens[t] = fisym;
            } else if (strcmp(buffer, "then") == 0) {
                tokens[t] = thensym;
            } else if (strcmp(buffer, "else") == 0) {
                tokens[t] = elsesym;
            } else if (strcmp(buffer, "while") == 0) {
                tokens[t] = whilesym;
            } else if (strcmp(buffer, "do") == 0) {
                tokens[t] = dosym;
            } else if (strcmp(buffer, "call") == 0) {
                tokens[t] = callsym;
            } else if (strcmp(buffer, "const") == 0) {
                tokens[t] = constsym;
            } else if (strcmp(buffer, "var") == 0) {
                tokens[t] = varsym;
            } else if (strcmp(buffer, "procedure") == 0) {
                tokens[t] = procsym;
            } else if (strcmp(buffer, "write") == 0) {
                tokens[t] = writesym;
            } else if (strcmp(buffer, "read") == 0) {
                tokens[t] = readsym;
            } else if (strcmp(buffer, "even") == 0) {
                tokens[t] = evensym;
            } else {
                // It's an identifier
                tokens[t] = identsym;
            }
            strcpy(lexemes[t], buffer);
            t++;
            continue;
        }

        // Handle numbers
        if (isdigit(c)) {
            int len = 0;
            char buffer[20];
            memset(buffer, 0, sizeof(buffer));
            
            while (isdigit(c)) {
                if (len < 5) {
                    buffer[len++] = c;
                } else {
                    len++;
                }
                c = fgetc(inputFile);
            }
            ungetc(c, inputFile);
            buffer[len < 5 ? len : 5] = '\0';

            if (len > 5) {
                strcpy(lexemes[t], buffer);
                tokens[t] = skipsym;
                t++;
                continue;
            }

            tokens[t] = numbersym;
            strcpy(lexemes[t], buffer);
            t++;
            continue;
        }

        // Handle operators and delimiters
        switch (c) {
            case '+':
                tokens[t] = plussym;
                strcpy(lexemes[t], "+");
                break;
            case '-':
                tokens[t] = minussym;
                strcpy(lexemes[t], "-");
                break;
            case '*':
                tokens[t] = multsym;
                strcpy(lexemes[t], "*");
                break;
            case '(':
                tokens[t] = lparentsym;
                strcpy(lexemes[t], "(");
                break;
            case ')':
                tokens[t] = rparentsym;
                strcpy(lexemes[t], ")");
                break;
            case '=':
                tokens[t] = eqsym;
                strcpy(lexemes[t], "=");
                break;
            case ',':
                tokens[t] = commasym;
                strcpy(lexemes[t], ",");
                break;
            case '.':
                tokens[t] = periodsym;
                strcpy(lexemes[t], ".");
                break;
            case ';':
                tokens[t] = semicolonsym;
                strcpy(lexemes[t], ";");
                break;
            case ':':
                if ((c = fgetc(inputFile)) == '=') {
                    tokens[t] = becomessym;
                    strcpy(lexemes[t], ":=");
                } else {
                    ungetc(c, inputFile);
                    tokens[t] = skipsym;
                    strcpy(lexemes[t], ":");
                    t++;
                    continue;
                }
                break;
            case '<':
                c = fgetc(inputFile);
                if (c == '=') {
                    tokens[t] = leqsym;
                    strcpy(lexemes[t], "<=");
                } else if (c == '>') {
                    tokens[t] = neqsym;
                    strcpy(lexemes[t], "<>");
                } else {
                    ungetc(c, inputFile);
                    tokens[t] = lessym;
                    strcpy(lexemes[t], "<");
                }
                break;
            case '>':
                c = fgetc(inputFile);
                if (c == '=') {
                    tokens[t] = geqsym;
                    strcpy(lexemes[t], ">=");
                } else {
                    ungetc(c, inputFile);
                    tokens[t] = gtrsym;
                    strcpy(lexemes[t], ">");
                }
                break;
            default:
                tokens[t] = skipsym;
                lexemes[t][0] = c;
                lexemes[t][1] = '\0';
                t++;
                continue;
        }
        t++;
    }


    FILE *tokenFile = fopen("tokenlist.txt", "w");

    // Print token list to tokenlist.txt

    for (int l = 0; l < t; l++) {
        if (tokenFile) {
            fprintf(tokenFile, "%d ", tokens[l]);
            if (tokens[l] == 2 || tokens[l] == 3) {
                if (tokenFile) {
                    fprintf(tokenFile, "%s \n", lexemes[l]);
                    continue;
                }
            }
            fprintf(tokenFile, "\n");

        }

    }

    // Close all files

    if (tokenFile) {
        fclose(tokenFile);
    }
    fclose(inputFile);

    return 0;
}