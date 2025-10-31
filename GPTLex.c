// lexer_debug.c
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <ctype.h>

typedef enum {
    skipsym = 1, identsym, numbersym, plussym, minussym, multsym, slashsym,
    eqsym, neqsym, lessym, leqsym, gtrsym, geqsym, lparentsym, rparentsym,
    commasym, semicolonsym, periodsym, becomessym, beginsym, endsym, ifsym,
    fisym, thensym, whilesym, dosym, callsym, constsym, varsym, procsym,
    writesym, readsym, elsesym, evensym
} TokenType;

const char* token_name(int tok) {
    switch (tok) {
        case skipsym: return "skipsym";
        case identsym: return "identsym";
        case numbersym: return "numbersym";
        case plussym: return "plussym";
        case minussym: return "minussym";
        case multsym: return "multsym";
        case slashsym: return "slashsym";
        case eqsym: return "eqsym";
        case neqsym: return "neqsym";
        case lessym: return "lessym";
        case leqsym: return "leqsym";
        case gtrsym: return "gtrsym";
        case geqsym: return "geqsym";
        case lparentsym: return "lparentsym";
        case rparentsym: return "rparentsym";
        case commasym: return "commasym";
        case semicolonsym: return "semicolonsym";
        case periodsym: return "periodsym";
        case becomessym: return "becomessym";
        case beginsym: return "beginsym";
        case endsym: return "endsym";
        case ifsym: return "ifsym";
        case fisym: return "fisym";
        case thensym: return "thensym";
        case whilesym: return "whilesym";
        case dosym: return "dosym";
        case callsym: return "callsym";
        case constsym: return "constsym";
        case varsym: return "varsym";
        case procsym: return "procsym";
        case writesym: return "writesym";
        case readsym: return "readsym";
        case elsesym: return "elsesym";
        case evensym: return "evensym";
        default: return "UNKNOWN";
    }
}

int main(int argc, char **argv) {
    if (argc < 2) {
        fprintf(stderr, "Usage: %s sourcefile\n", argv[0]);
        return 1;
    }

    // --- Read entire file into memory safely ---
    FILE *f = fopen(argv[1], "rb");
    if (!f) { perror("fopen"); return 1; }
    if (fseek(f, 0, SEEK_END) != 0) { perror("fseek"); fclose(f); return 1; }
    long flen = ftell(f);
    if (flen < 0) { perror("ftell"); fclose(f); return 1; }
    rewind(f);

    char *info = malloc(flen + 2); // +1 for '\0', +1 safety
    if (!info) { fprintf(stderr, "malloc failed\n"); fclose(f); return 1; }
    size_t read = fread(info, 1, flen, f);
    info[read] = '\0';
    fclose(f);

    // --- Token storage ---
    const int MAXTOK = 1000;
    char lexemes[MAXTOK][64];
    int tokens[MAXTOK];
    int t = 0;

    // --- Lexer loop with full control of index k ---
    int k = 0;
    int i = (int)read; // length
    while (k < i && t < MAXTOK - 1) {
        char c = info[k];

        // Skip whitespace
        if (isspace((unsigned char)c)) { k++; continue; }

        // Comments: support // single-line and /* ... */
        if (c == '/' && k + 1 < i && info[k+1] == '/') {
            // skip to end of line
            k += 2;
            while (k < i && info[k] != '\n') k++;
            continue;
        }
        if (c == '/' && k + 1 < i && info[k+1] == '*') {
            k += 2;
            while (k + 1 < i && !(info[k] == '*' && info[k+1] == '/')) k++;
            if (k + 1 < i) k += 2; // skip closing */
            continue;
        }

        // Identifier or keyword
        if (isalpha((unsigned char)c)) {
            int len = 0;
            while (k < i && (isalnum((unsigned char)info[k]) || info[k] == '_') && len < (int)sizeof(lexemes[0]) - 1) {
                lexemes[t][len++] = info[k++];
            }
            lexemes[t][len] = '\0';

            // keyword checks
            if (strcmp(lexemes[t], "const") == 0) tokens[t] = constsym;
            else if (strcmp(lexemes[t], "var") == 0) tokens[t] = varsym;
            else if (strcmp(lexemes[t], "procedure") == 0) tokens[t] = procsym;
            else if (strcmp(lexemes[t], "begin") == 0) tokens[t] = beginsym;
            else if (strcmp(lexemes[t], "end") == 0) tokens[t] = endsym;
            else if (strcmp(lexemes[t], "if") == 0) tokens[t] = ifsym;
            else if (strcmp(lexemes[t], "fi") == 0) tokens[t] = fisym;
            else if (strcmp(lexemes[t], "then") == 0) tokens[t] = thensym;
            else if (strcmp(lexemes[t], "while") == 0) tokens[t] = whilesym;
            else if (strcmp(lexemes[t], "do") == 0) tokens[t] = dosym;
            else if (strcmp(lexemes[t], "call") == 0) tokens[t] = callsym;
            else if (strcmp(lexemes[t], "read") == 0) tokens[t] = readsym;
            else if (strcmp(lexemes[t], "write") == 0) tokens[t] = writesym;
            else if (strcmp(lexemes[t], "else") == 0) tokens[t] = elsesym;
            else if (strcmp(lexemes[t], "even") == 0) tokens[t] = evensym;
            else tokens[t] = identsym;

            // debug print:
            printf("[DBG] pos=%3d lexeme=\"%s\" token=%s\n", k - (int)strlen(lexemes[t]), lexemes[t], token_name(tokens[t]));
            t++;
            continue;
        }

        // Number literal
        if (isdigit((unsigned char)c)) {
            int len = 0;
            while (k < i && isdigit((unsigned char)info[k]) && len < (int)sizeof(lexemes[0]) - 1) {
                lexemes[t][len++] = info[k++];
            }
            lexemes[t][len] = '\0';
            tokens[t] = numbersym;
            printf("[DBG] pos=%3d lexeme=\"%s\" token=%s\n", k - len, lexemes[t], token_name(tokens[t]));
            t++;
            continue;
        }

        // Special symbols (multi-char first)
        if (c == ':' && k + 1 < i && info[k+1] == '=') {
            strcpy(lexemes[t], ":=");
            tokens[t] = becomessym;
            printf("[DBG] pos=%3d lexeme=\":=\" token=%s\n", k, token_name(tokens[t]));
            k += 2; t++; continue;
        }
        if (c == '<') {
            if (k + 1 < i && info[k+1] == '=') { strcpy(lexemes[t], "<="); tokens[t] = leqsym; printf("[DBG] pos=%3d lexeme=\"<=\" token=%s\n", k, token_name(tokens[t])); k+=2; t++; continue; }
            if (k + 1 < i && info[k+1] == '>') { strcpy(lexemes[t], "<>"); tokens[t] = neqsym; printf("[DBG] pos=%3d lexeme=\"<>\" token=%s\n", k, token_name(tokens[t])); k+=2; t++; continue; }
            strcpy(lexemes[t], "<"); tokens[t] = lessym; printf("[DBG] pos=%3d lexeme=\"<\" token=%s\n", k, token_name(tokens[t])); k++; t++; continue;
        }
        if (c == '>' && k + 1 < i && info[k+1] == '=') {
            strcpy(lexemes[t], ">="); tokens[t] = geqsym; printf("[DBG] pos=%3d lexeme=\">=\" token=%s\n", k, token_name(tokens[t])); k+=2; t++; continue;
        }

        // Single-char symbols
        switch (c) {
            case '+': strcpy(lexemes[t], "+"); tokens[t] = plussym; break;
            case '-': strcpy(lexemes[t], "-"); tokens[t] = minussym; break;
            case '*': strcpy(lexemes[t], "*"); tokens[t] = multsym; break;
            case '/': strcpy(lexemes[t], "/"); tokens[t] = slashsym; break;
            case '(': strcpy(lexemes[t], "("); tokens[t] = lparentsym; break;
            case ')': strcpy(lexemes[t], ")"); tokens[t] = rparentsym; break;
            case ',' : strcpy(lexemes[t], ","); tokens[t] = commasym; break;
            case ';' : strcpy(lexemes[t], ";"); tokens[t] = semicolonsym; break;
            case '.' : strcpy(lexemes[t], "."); tokens[t] = periodsym; break;
            case '=' : strcpy(lexemes[t], "="); tokens[t] = eqsym; break;
            default:
                // unknown / invalid symbol
                lexemes[t][0] = c; lexemes[t][1] = '\0';
                tokens[t] = skipsym;
                printf("[DBG] pos=%3d lexeme=\"%s\" token=skipsym (invalid symbol)\n", k, lexemes[t]);
                k++; t++; continue;
        }
        // for single-char handled above:
        printf("[DBG] pos=%3d lexeme=\"%s\" token=%s\n", k, lexemes[t], token_name(tokens[t]));
        k++; t++;
    }

    // --- Final print: Lexeme Table + Token List ---
    printf("\n\nLexeme Table:\n");
    printf("%-12s %s\n", "lexeme", "token");
    for (int x = 0; x < t; x++) {
        printf("%-12s %s\n", lexemes[x], token_name(tokens[x]));
    }

    printf("\nToken List (numbers are token ids):\n");
    for (int x = 0; x < t; x++) {
        printf("%d ", tokens[x]);
        if (tokens[x] == identsym || tokens[x] == numbersym) {
            printf("%s ", lexemes[x]);
        }
    }
    printf("\n");

    free(info);
    return 0;
}
