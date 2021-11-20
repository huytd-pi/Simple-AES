#include<stdio.h>
#include <stdlib.h>
int main(){
    char *hex = (char*)malloc(100* sizeof(char));
    scanf("%[^\n]s",&hex);
    printf("%x",hex);
}