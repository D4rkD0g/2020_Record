#include <stdio.h>

void print_hello(void){
        printf("Hello\n");
}

void print_good(void){
        printf("Goodbye\n");
}

void print_bad(void){
        printf("Oh no!\n");
}
void darkerthandark(void){
        printf("DARK\n");
}

int main(){
        int i;
        int a = 10;

        if (a < 10)
                print_hello();
        if (0)
                print_good();
        if (1)
                print_bad();

        for ( i = 0 , a = 3 ; i < 99 ; i++){
                darkerthandark();
        }
        return 0;

}