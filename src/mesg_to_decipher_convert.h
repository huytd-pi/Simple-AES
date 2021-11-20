#include "AES-128.c"
#include "string_hex_to_uint8_t_hex.h"
#include "uint8_t_hex_to_string_hex.h"
char* decipherConvertMessenger(uint8_t *key,char* text){
	char* output = (char*)malloc(100* sizeof(char));
    uint8_t* hex = (unsigned char*)malloc(100* sizeof(unsigned char));
	uint8_t hex128bit[16];
	uint8_t* ouputinvCipher = (uint8_t*) malloc (100* sizeof(uint8_t));
	char* convertHexToString = (unsigned char*)malloc(100* sizeof(unsigned char));
    
    int i,count = 0;
    // text = converStringToHex(text);
    hex = convertStringHexToUint8_t(text);
    while (1)
	{
		memset(hex128bit,'00',strlen(hex128bit));
		memset(ouputinvCipher,'00',strlen(ouputinvCipher));
		for(i = 0; i < 16; i++){
			hex128bit[i] = hex[i+count*16];
		}
		count++;
		//AES
		invCipher(ouputinvCipher, hex128bit, key);
		// printf("debug ouputinvCipher %s\n",ouputinvCipher);
		convertUint8_tHexToString(ouputinvCipher,convertHexToString,16);
		strcat(output,convertHexToString);
		// printf("debug convertHexToString %s\n",convertHexToString);
		// printf("debug hex128bit %ld\n",strlen(hex128bit));
		if ((strlen(hex)/16) == count) break;
	}
	return output;
}