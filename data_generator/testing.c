#include <stdio.h>
#include <stdlib.h>
#include <sys/stat.h>

#define SIZE 1000000 //defining size for all array operations (for the file size)

int main(){
    FILE* ptr = fopen("init.csv", "r");

    int* init_data = (int*) malloc(sizeof(int)*SIZE);
    for(int i = 0; i <= SIZE; i++){
        if(fscanf(ptr, " %d", (init_data + i)) == EOF){
            break;
        }
    }

    for(int i = 0; i < 53; i++){
        printf("%d\n", init_data[i]);
    }

    printf("%d\n", init_data[SIZE-1]);

    
}