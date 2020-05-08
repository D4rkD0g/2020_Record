#include <stdio.h>
#include <string.h>

int main(int argc, char *argv[]){
    char buf[10];
    char *src = "Hello world,Lambda,hhhhhhhhhalallalalalalallallalalallalala";
    memcpy(buf,src, strlen(src));
    for(int i = 1; i < 10000; i++){buf[i] = i;}
    return 0;
}
