#include <stdio.h>

int myStringLen(char *str);
char *myStringAdd(char *origin, char *addition);
int myStringFind(char *origin, char *substr);

int main(int argc, char **argv) {
	if(argc!=4){
		printf("Only insert 3 arguments\n");
		return -1;
	}
	printf("Initial Lenght: %d\n", myStringLen(argv[1]));
	char *newStr = myStringAdd(argv[1],argv[2]);
	printf("New String: %s\n", newStr);
	int found = myStringFind(newStr,argv[3]);
	printf("Substring was found: ");
	if(found<0){
		printf("No\n");
	}
	else{
		printf("Yes, at pos %d\n", found);
	}
	return 0;
}