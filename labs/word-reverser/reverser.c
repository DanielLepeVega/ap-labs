#include <stdio.h>
void displayString(char str[]);

int main(int argc, char *argv[])
{
    char str[50];
    printf("Input: ");
    char c = getchar();
    int i = 0;
    while(c != '\n'){
        c = getchar();
        str[i] = c;
        if(c == '\n'){
            break;
        }
    }             
    displayString(str);     // Passing string to a function.    
    return 0;
}
void displayString(char str[])
{
    /*for(int k = 0;k<sizeof(str);k++){
        printf("%c", str[k]);  
    }*/
    printf("Reversed Output: ");
    for(int i=50;i>0;i--){
        printf("%c", str[i]);
    }
}