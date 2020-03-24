#include <string.h>
#include <stdlib.h>
int myStringLen(char *str){
	int i;
	for(i=0;str[i]!='\0';i++);
    return i;
}

char *myStringAdd(char *origin, char *addition){
	int size1 = myStringLen(origin);
	int size2 = myStringLen(addition);
	char *newString = calloc(size1+size2+1, sizeof(char));
	strcpy(newString,origin);
	strcpy(newString+size1,addition);
    return newString;
}

int myStringFind(char *origin, char *substr){
	int i;
    int j; 
    int found;
	int size1 = myStringLen(origin);
	int size2 = myStringLen(substr);
	for(i=0;i<size1;i++){
		found = 1;
		for(j=0;i+j<size1 && j<size2;j++){
			if(origin[i+j] != substr[j]){
				found = 0;
				break;
			}
		}
		if(found && j==size2)
			return i;
	}
    return -1;
}