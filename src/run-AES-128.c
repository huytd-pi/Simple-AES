#include "AES-128.c"
#include <string.h>
#include "string_to_hex.h"	
#include "string_hex_to_uint8_t_hex.h"
#include "uint8_t_hex_to_string_hex.h"

int main(){
    uint8_t key[] = { 0x2b, 0x7e, 0x15, 0x16, 0x28, 0xae, 0xd2, 0xa6, 0xab, 0xf7, 0x15, 0x88, 0x09, 0xcf, 0x4f, 0x3c };

	uint8_t* ouputCipher = (uint8_t*) malloc (100* sizeof(uint8_t));
	uint8_t* ouputinvCipher = (uint8_t*)malloc(100* sizeof(uint8_t));
	uint8_t* hex = (unsigned char*)malloc(100* sizeof(unsigned char));
	uint8_t hex128bit[16];
	char* convertHexToString = (char*)malloc(100* sizeof(char));
	char* output = (char*)malloc(100* sizeof(char));
	int i,j, lengthHex = strlen(hex);
	int count = 0;
	char* text = (char*)malloc(100* sizeof(char));
	printf("nhap messenger: ");
	scanf("%[^\n]s", text);
	text = converStringToHex(text);
	printf("%s\n",text);
	hex = convertStringHexToUint8_t(text);
  	for(i = 0; i < strlen(text)/2; i++){
    	if(i % 16 == 0) printf("\n");
		printf("%.2X ",hex[i]);
  	}
	printf("\n--------------------------------\n");
	while (1)
	{
		memset(hex128bit,'00',strlen(hex128bit));
		memset(ouputCipher,'00',strlen(ouputCipher));
		memset(ouputinvCipher,'00',strlen(ouputinvCipher));
		for(i = 0; i < 16; i++){
			hex128bit[i] = hex[i+count*16];
		}
		count++;
		//AES
		Cipher(ouputCipher, hex128bit, key);
		printf("Cipher: ");
		for (i = 0; i <16; i++){
			printf("%.2X ", ouputCipher[i]);
		}
		invCipher(ouputinvCipher, ouputCipher, key);
		printf("| Decipher: ");
		for (i = 0; i < 16; i++) {
			printf("%.2X ", ouputinvCipher[i]);
		}
		convertUint8_tHexToString(ouputinvCipher,convertHexToString,16);
		strcat(output,convertHexToString);
		printf("| %s",convertHexToString);
		printf("\n");
		if (strlen(hex128bit) < 16) break;
	}
	printf("%s",output);
	printf("\n");
}