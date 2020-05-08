#include <string.h>

int is_initialized=0;

void init_buffer(){
	printf("init buffer\n");
	is_initialized=1;
}

int buffer_overflow(char *input){
	char buf[128];
	printf("buffer_overflow\n");
	if(is_initialized){
		strcpy(&buf[0], input);
	}
}

int keyword_buffer_overflow(char *keyword, char *input){
	printf("keyword_buffer_overflow\n");
	char buf[128];

	if(strlen(keyword) >= 2){
		if(keyword[0] == 'k' && keyword[1] == 'w'){
			strcpy(&buf[0], input);
		}
	}
}

int integer_overflow(int size, char *input){
	printf("integer_overflow\n");
	char buf[128];
	if(size < 128){
		strcpy(&buf[0], input);
	}
}
