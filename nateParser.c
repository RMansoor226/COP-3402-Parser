#include <stdio.h>
#include <string.h>
#include <ctype.h>
int main(){
    FILE *inputFile = fopen("input.txt", "r");

    // Check if input file opened successfully
    if (!inputFile) {
        printf("Error opening file.\n");
        return 1;
    }
    else {
        printf("file opened successfully\n");
    }
    //read through every line in the token input, and check for a 1
    char ch1, ch2;
    while ((ch1 = fgetc(inputFile)) != EOF && (ch2 = fgetc(inputFile)) != EOF) {
        // Process each character here
        if(ch1 && ch2 != EOF){
            printf("%c %c\n", ch1, ch2);   
        } 
        else {
            printf("%c\n", ch1);
        }//if(ch == '1'){
            //printf("Error: Scanning error detected by lexer (skipsym present)");
            //break;
        }
    }
