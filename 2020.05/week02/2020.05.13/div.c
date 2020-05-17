#include <stdio.h>
#include <stdlib.h>
int main(int argc, char* argv[]) {
	int a = 100;
	int b;
	if(argc == 2){
		b = a / atoi(argv[1]);
	}else{
		b = 10;
	}
	printf("%d\n", b);
	return 0;
}
