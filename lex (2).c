#include <stdio.h>
#include <string.h>
#include <ctype.h>

typedef enum {
    skipsym = 1,  // Skip / ignore token
    identsym,     // Identifier
    numbersym,    // Number
    plussym,      // +
    minussym,     // -
    multsym,      // *
    slashsym,     // /
    eqsym,        // =
    neqsym,       // <>
    lessym,       // <
    leqsym,       // <=
    gtrsym,       // >
    geqsym,       // >=
    lparentsym,   // (
    rparentsym,   // )
    commasym,     // ,
    semicolonsym, // ;
    periodsym,    // .
    becomessym,   // :=
    beginsym,     // begin
    endsym,       // end
    ifsym,        // if
    fisym,        // fi
    thensym,      // then
    whilesym,     // while
    dosym,        // do
    callsym,      // call
    constsym,     // const
    varsym,       // var
    procsym,      // procedure
    writesym,     // write
    readsym,      // read
    elsesym,      // else
    evensym       // even
} TokenType;

int main(int argc, char* argv[]) {

    FILE *inputFile = fopen(argv[1], "r");

    // Check if input file opened successfully
    if (!inputFile) {
        printf("Error opening file.\n");
        return 1;
    }

    char info[100];
    int i = 0;
    int j = 0;

    // Print header
    printf("Source Program:\n\n");

    // Print input file contents
    while (fgets(info, sizeof(info), inputFile)) {
        printf("%s", info);
    }

    fseek(inputFile, 0, SEEK_SET);

    // Print Lexeme Table header
    printf("\n\nLexeme Table:\n");
    printf("\nlexeme\ttoken type\n");

    // Scan input file character by character excluding whitespace
    i = 0;
    while ((j = fgetc(inputFile)) != EOF && i < sizeof(info) - 1) {
        info[i++] = (char) j;
    }
    info[i] = '\0';

    info[i] = '\0'; // Null-terminate the string

    char lexemes[100][20]; // Assuming max 100 lexemes, each up to 19 chars + null terminator
    char tokens[100];
    char message[20];

    for (int k = 0; k < i; k++) {
        if (info[k] == ' ' || info[k] == '\t' || info[k] == '\n' || info[k] == '\r') {
            continue;
        }

        // Tokenize single-line comment delimiter
        if (info[k] == '/' && info[k + 1] == '/') {
            tokens[t] = skipsym;
            strcpy(lexemes[t], "//");
            t++;
            k++; // Skip the next '/'
            printf("%-7s %-10d\n", lexemes[t - 1], tokens[t - 1]);
            continue;
        }

        // Tokenize multi-line comment start delimiter
        if (info[k] == '/' && info[k + 1] == '*') {
            tokens[t] = skipsym;
            strcpy(lexemes[t], "/*");
            t++;
            k++;
            printf("%-7s %-10d\n", lexemes[t - 1], tokens[t - 1]);
            continue;
        }

        // Tokenize multi-line comment end delimiter
        if (info[k] == '*' && info[k + 1] == '/') {
            tokens[t] = skipsym;
            strcpy(lexemes[t], "*/");
            t++;
            k++;
            printf("%-7s %-10d\n", lexemes[t - 1], tokens[t - 1]);
            continue;
        }

        switch (info[k]) {
            case '+':
                tokens[t] = plussym;
                strcpy(lexemes[t], "+");
                t++;
                break;

            case '-':
                tokens[t] = minussym;
                strcpy(lexemes[t], "-");
                t++;
                break;

            case '*':
                tokens[t] = multsym;
                strcpy(lexemes[t], "*");
                t++;
                break;

            case '/':
                tokens[t] = slashsym;
                strcpy(lexemes[t], "/");
                t++;
                break;

            case '<':
                switch (info[k + 1]) {
                    case '=':
                        tokens[t] = leqsym;
                        strcpy(lexemes[t], "<=");
                        t++;
                        k++;
                        break;
                    case '>':
                        tokens[t] = neqsym;
                        strcpy(lexemes[t], "<>");
                        t++;
                        k++;
                        break;
                    default:
                        tokens[t] = lessym;
                        strcpy(lexemes[t], "<");
                        t++;
                        break;
                }
                break;

            case '>':
                if (info[k + 1] == '=') {
                    tokens[t] = geqsym;
                    strcpy(lexemes[t], ">=");
                    t++;
                    k++;
                } else {
                    tokens[t] = gtrsym;
                    strcpy(lexemes[t], ">");
                    t++;
                }
                break;

            case '(':
                tokens[t] = lparentsym;
                strcpy(lexemes[t], "(");
                t++;
                break;

            case ')':
                tokens[t] = rparentsym;
                strcpy(lexemes[t], ")");
                t++;
                break;

            case ',':
                tokens[t] = commasym;
                strcpy(lexemes[t], ",");
                t++;
                break;

            case ';':
                tokens[t] = semicolonsym;
                strcpy(lexemes[t], ";");
                t++;
                break;

            case '.':
                tokens[t] = periodsym;
                strcpy(lexemes[t], ".");
                t++;
                break;

            case ':':
                if (info[k + 1] == '=') {
                    tokens[t] = becomessym;
                    strcpy(lexemes[t], ":=");
                    t++;
                    k++;
                } else {
                    tokens[t] = skipsym;
                    strcpy(lexemes[t], ":");
                    strcpy(message, "Invalid symbol");
                    t++;
                }
                break;

            case '=':
                tokens[t] = eqsym;
                strcpy(lexemes[t], "=");
                t++;
                break;

            // Keywords and identifiers
            case 'b':
                if (strncmp(&info[k], "begin", 5) == 0) {
                    tokens[t] = beginsym;
                    strcpy(lexemes[t], "begin");
                    t++;
                    k += 4;
                } else {
                    tokens[t] = identsym;
                    strcpy(lexemes[t], "b");
                    t++;
                }
                break;

            case 'e':
                if (strncmp(&info[k], "end", 3) == 0) {
                    tokens[t] = endsym;
                    strcpy(lexemes[t], "end");
                    t++;
                    k += 2;
                } else if (strncmp(&info[k], "else", 4) == 0) {
                    tokens[t] = elsesym;
                    strcpy(lexemes[t], "else");
                    t++;
                    k += 3;
                } else if (strncmp(&info[k], "even", 4) == 0) {
                    tokens[t] = evensym;
                    strcpy(lexemes[t], "even");
                    t++;
                    k += 3;
                } else {
                    tokens[t] = identsym;
                    strcpy(lexemes[t], "e");
                    t++;
                }
                break;

            case 'i':
                if (info[k + 1] == 'f') {
                    tokens[t] = ifsym;
                    strcpy(lexemes[t], "if");
                    t++;
                    k++;
                } else {
                    tokens[t] = identsym;
                    strcpy(lexemes[t], "i");
                    t++;
                }
                break;

            case 'f':
                if (info[k + 1] == 'i') {
                    tokens[t] = fisym;
                    strcpy(lexemes[t], "fi");
                    t++;
                    k++;
                } else {
                    tokens[t] = identsym;
                    strcpy(lexemes[t], "f");
                    t++;
                }
                break;

            case 't':
                if (strncmp(&info[k], "then", 4) == 0) {
                    tokens[t] = thensym;
                    strcpy(lexemes[t], "then");
                    t++;
                    k += 3;
                } else {
                    tokens[t] = identsym;
                    strcpy(lexemes[t], "t");
                    t++;
                }
                break;

            case 'w':
                if (strncmp(&info[k], "while", 5) == 0) {
                    tokens[t] = whilesym;
                    strcpy(lexemes[t], "while");
                    t++;
                    k += 4;
                } else if (strncmp(&info[k], "write", 5) == 0) {
                    tokens[t] = writesym;
                    strcpy(lexemes[t], "write");
                    t++;
                    k += 4;
                } else {
                    tokens[t] = identsym;
                    strcpy(lexemes[t], "w");
                    t++;
                }
                break;

            case 'd':
                if (info[k + 1] == 'o') {
                    tokens[t] = dosym;
                    strcpy(lexemes[t], "do");
                    t++;
                    k++;
                } else {
                    tokens[t] = identsym;
                    strcpy(lexemes[t], "d");
                    t++;
                }
                break;

            case 'c':
                if (strncmp(&info[k], "call", 4) == 0) {
                    tokens[t] = callsym;
                    strcpy(lexemes[t], "call");
                    t++;
                    k += 3;
                } else if (strncmp(&info[k], "const", 5) == 0) {
                    tokens[t] = constsym;
                    strcpy(lexemes[t], "const");
                    t++;
                    k += 4;
                } else {
                    tokens[t] = identsym;
                    strcpy(lexemes[t], "c");
                    t++;
                }
                break;

            case 'v':
                if (strncmp(&info[k], "var", 3) == 0) {
                    tokens[t] = varsym;
                    strcpy(lexemes[t], "var");
                    t++;
                    k += 2;
                } else {
                    tokens[t] = identsym;
                    strcpy(lexemes[t], "v");
                    t++;
                }
                break;

            case 'p':
                if (strncmp(&info[k], "procedure", 9) == 0) {
                    tokens[t] = procsym;
                    strcpy(lexemes[t], "procedure");
                    t++;
                    k += 8;
                } else {
                    tokens[t] = identsym;
                    strcpy(lexemes[t], "p");
                    t++;
                }
                break;

            case 'r':
                if (strncmp(&info[k], "read", 4) == 0) {
                    tokens[t] = readsym;
                    strcpy(lexemes[t], "read");
                    t++;
                    k += 3;
                } else {
                    tokens[t] = identsym;
                    strcpy(lexemes[t], "r");
                    t++;
                }
                break;

            default:
                if (isdigit(info[k])) {
                    int len = 0;
                    memset(lexemes[t], 0, sizeof(lexemes[t]));

                    while (isdigit(info[k])) {
                        lexemes[t][len++] = info[k++];
                    }

                    if (len > 5) {
                        strcpy(message, "Number too long");
                        tokens[t] = skipsym;
                    } else {
                        tokens[t] = numbersym;
                    }

                    lexemes[t][len] = '\0';
                    t++;
                    k--;
                }
                else if (isalpha(info[k])) {
                    int len = 0;
                    memset(lexemes[t], 0, sizeof(lexemes[t]));

                    while (isalnum(info[k]) && len < 11) {
                        lexemes[t][len++] = info[k++];
                    }

                    lexemes[t][len] = '\0';

                    if (len > 11) {
                        strcpy(message, "Identifier too long");
                        tokens[t] = skipsym;
                    } else {
                        tokens[t] = identsym;
                    }

                    t++;
                    k--;
                } 
                else {
                    lexemes[t][0] = info[k];
                    lexemes[t][1] = '\0';
                    strcpy(message, "Invalid symbol");
                    tokens[t] = skipsym;
                    t++;
                }
                break;
        }

        if (tokens[t - 1] == skipsym) {
            printf("%-7s %-10s\n", lexemes[t - 1], message);
        } else {
            printf("%-7s %-10d\n", lexemes[t - 1], tokens[t - 1]);
        }
    }

    printf("\nToken List:\n\n");

    for (int l = 0; l < t; l++) {
        printf("%d ", tokens[l]);
        if (tokens[l] == 2 || tokens[l] == 3) {
            printf("%s ", lexemes[l]);
        }
    }

    fclose(inputFile);
    return 0;
}
