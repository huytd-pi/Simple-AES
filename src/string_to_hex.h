char* converStringToHex(char* str){
    char *hex = (char *)malloc(1000* sizeof(char));
    int len = strlen(str)-1;
    int i, j;
    for (i = 0, j = 0; i < len; ++i, j += 2){
        sprintf(hex + j, "%02x", str[i] & 0xff);
        // printf("%s ",hex);
    }

    return hex;
}