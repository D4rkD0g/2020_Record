#include <stdio.h>
#include <stdlib.h>
#include <stdint.h>
#include <string.h>
#include <dlfcn.h>
#include <unistd.h> 

int isLoaded = 0;
void *vuln = NULL;

typedef void(*_init_0_t)();
typedef long long int(*init_buffer_0_t)();
typedef long long int(*buffer_overflow_0_t)(long long int);
typedef long long int(*keyword_buffer_overflow_0_t)(char*,long long int);
typedef long long int(*integer_overflow_0_t)(int,long long int);
typedef long long int(*_fini_0_t)();

_init_0_t _init_0 = NULL;
init_buffer_0_t init_buffer_0 = NULL;
buffer_overflow_0_t buffer_overflow_0 = NULL;
keyword_buffer_overflow_0_t keyword_buffer_overflow_0 = NULL;
integer_overflow_0_t integer_overflow_0 = NULL;
_fini_0_t _fini_0 = NULL;

void CloseLibrary()
{
    if(vuln){
        dlclose(vuln);
        vuln = NULL;
    }
    return;
}

int LoadLibrary()
{
    vuln = dlopen("vuln.so", RTLD_NOW|RTLD_GLOBAL);
    fprintf(stderr, "%s\n", dlerror());
    printf("loaded vuln at %p\n", vuln);
    atexit(CloseLibrary);
    return vuln != NULL;
}

void ResolveSymbols()
{
    _init_0 = (_init_0_t)dlsym(vuln, "_init_0"); printf("loaded _init_0 at %p\n", _init_0);
    init_buffer_0 = (init_buffer_0_t)dlsym(vuln, "init_buffer_0"); printf("loaded init_buffer_0 at %p\n", init_buffer_0);
    buffer_overflow_0 = (buffer_overflow_0_t)dlsym(vuln, "buffer_overflow_0"); printf("loaded buffer_overflow_0 at %p\n", buffer_overflow_0);
    keyword_buffer_overflow_0 = (keyword_buffer_overflow_0_t)dlsym(vuln, "keyword_buffer_overflow_0"); printf("loaded keyword_buffer_overflow_0 at %p\n", keyword_buffer_overflow_0);
    integer_overflow_0 = (integer_overflow_0_t)dlsym(vuln, "integer_overflow_0"); printf("loaded integer_overflow_0 at %p\n", integer_overflow_0);
    _fini_0 = (_fini_0_t)dlsym(vuln, "_fini_0"); printf("loaded _fini_0 at %p\n", _fini_0); 
}

int main(int argc, char *argv[])
{
    
    int BufSize = 512;
    char Buf[BufSize];
    char* Data = NULL;
    int Size = BufSize;


    if (!isLoaded)
    {
        if(!LoadLibrary()) 
        {
            printf("could not load vuln.so\n");
            return -1;
        }
        ResolveSymbols();
        isLoaded = 1;
    }

    if (Size==0)
    {
        return 0;
    }

    uint8_t choice = 0;
    
    /* Reset state. */
    memset(Buf, 0, BufSize);

    /* Read input data. */
    Size = read(0, Buf, BufSize);
    Data = &Buf[0];


    do
    {
        if (Size < sizeof(choice)) 
        {
            break;
        }

        choice = Data[0];
        Data += sizeof(choice);
        Size -= sizeof(choice);

        switch(choice%6) 
        {
            
            case 0:
                
                _init_0();
                Data += (0);
                Size -= (0);
                
                break;

            case 1:
                
                init_buffer_0();
                Data += (0);
                Size -= (0);
                
                break;

            case 2:
                
                if(Size < 4){
                    // not enough bytes in buffer
                    return 0;
                }
                long long int l_2000; memcpy(&l_2000, Data+(0), sizeof(long long int));
                buffer_overflow_0(l_2000);
                Data += (0 + 8);
                Size -= (0 + 8);
                
                break;

            case 3:
                
                if(Size < 4){
                    //not enough bytes in buffer
                    return 0;
                }
                unsigned int strlen_3000; memcpy(&strlen_3000, Data+(0), sizeof(int));
                if(strlen_3000 > Size){
                    //not enough bytes in buffer
                    return 0;
                }
                char *tmpbuf_3000 = malloc(strlen_3000+1);
                if(tmpbuf_3000 == NULL){
                    //could not allocate tmpstring
                    return 0;
                }
                strncpy(tmpbuf_3000, Data+(0)+4, strlen_3000);
                tmpbuf_3000[strlen_3000] = 0;
                 
                if(Size < 4){
                    // not enough bytes in buffer
                    return 0;
                }
                long long int l_3001; memcpy(&l_3001, Data+(0 + 4 + strlen_3000), sizeof(long long int));
                keyword_buffer_overflow_0(tmpbuf_3000, l_3001);
                Data += (0 + 4 + strlen_3000 + 8);
                Size -= (0 + 4 + strlen_3000 + 8);
                free(tmpbuf_3000);
                break;

            case 4:
                
                if(Size < 4){
                    // not enough bytes in buffer
                    return 0;
                }
                int l_4000; memcpy(&l_4000, Data+(0), sizeof(int)); 
                if(Size < 4){
                    // not enough bytes in buffer
                    return 0;
                }
                long long int l_4001; memcpy(&l_4001, Data+(0 + 4), sizeof(long long int));
                integer_overflow_0(l_4000, l_4001);
                Data += (0 + 4 + 8);
                Size -= (0 + 4 + 8);
                
                break;

            case 5:
                
                _fini_0();
                Data += (0);
                Size -= (0);
                
                break;
        }
    }while(Size > 0);
    return 0;
}
