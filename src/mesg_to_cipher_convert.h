#include "AES-128.c"
#include "string_to_hex.h"	
#include "string_hex_to_uint8_t_hex.h"
#include "uint8_t_hex_to_string_hex.h"
void cipherConvertMegs(uint8_t *key,char* text,char* output){
    uint8_t* hex = (unsigned char*)malloc(100* sizeof(unsigned char));
	uint8_t hex128bit[16];
	uint8_t* outputCipher = (uint8_t*) malloc (100* sizeof(uint8_t));
	char* convertHexToString = (unsigned char*)malloc(100* sizeof(unsigned char));
    
    int i,count = 0;

    text = converStringToHex(text);
	// printf("%s\n",text);
    hex = convertStringHexToUint8_t(text);
    while (1)
	{
		memset(hex128bit,'00',strlen(hex128bit));
		memset(outputCipher,'00',strlen(outputCipher));
		for(i = 0; i < 16; i++){
			hex128bit[i] = hex[i+count*16];
		}
		count++;
		//AES
		Cipher(outputCipher, hex128bit, key);
		convertUint8_tHexToString(outputCipher,convertHexToString,16);
		strcat(output,convertHexToString);
		if (strlen(hex128bit) < 16) break;
	}
}