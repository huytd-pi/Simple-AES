#include "../include/AES-128.h"

// Multiplication by two in GF(2^8). Multiplication by three is xtime(a) ^ a
#define xtime(x) ((x<<1) ^ (((x>>7) & 1) * 0x1b))
#define getSBoxValue(num) (sbox[num])
#define getSBoxInvert(num) (rsbox[(num)])

// This method performs the Key expansion
static void key_expansion(uint8_t* cipher_key)
{
	uint8_t temp[4];
	// length is 16 because the first cipher key provided
	uint8_t length = 16;
	uint8_t i, k = 0;
	// We need 11 sets of sixteen bytes each for AES-128 
	while (length < 176)
	{
		// Copy the temporary variable over from the last 4-byte block
		for (i = 0; i < 4; i++)
		{
			temp[i] = cipher_key[i + length - 4];
			// printf("temp[%d] %x ",i,temp[i]);
		}
		// Every four blocks (of four bytes) perform the operation		  
		if (length % 16 == 0)
		{
			// Rotate the vector and get the sboxvalue of the associated values.
			shiftRow(temp, 1);
			for (i = 0; i < 4; i++)
				temp[i] = getSBoxValue(temp[i]);
			// The first value is XORed with the rcon value
			temp[0] = temp[0] ^ rcon[k];
			k++;
		}
		//Finally add the calculated value back to the cipher_key
		for (i = 0; i < 4; i++)
		{
			cipher_key[length] = cipher_key[length - 16] ^ temp[i];
			//printf("cy[i] %x+%x ",cipher_key[length], temp[i]);
			length++;
		}
	}
}

// Performs a matrix multiplication of the column of the state with the given matrix in the galois field
static void mixColumn(uint8_t* column)
{
	uint8_t temp[4];
	uint8_t i;
	for (i = 0; i < 4; i++)
	{
		temp[i] = column[i];
	}
	// column of matrix state with  
	column[0] = xtime(temp[0]) ^ temp[3] ^ temp[2] ^ xtime(temp[1]) ^ temp[1]; // xtime(temp[0]) <=> (temp[0] * 2)
	column[1] = xtime(temp[1]) ^ temp[0] ^ temp[3] ^ xtime(temp[2]) ^ temp[2]; // xtime(temp[2]) ^ temp[2] <=> (temp[2] * 3) = (temp[2] * 2) xor temp[2]
	column[2] = xtime(temp[2]) ^ temp[1] ^ temp[0] ^ xtime(temp[3]) ^ temp[3];
	column[3] = xtime(temp[3]) ^ temp[2] ^ temp[1] ^ xtime(temp[0]) ^ temp[0];
}

// Perform the matrix multiplication for each column in the state
static void mixColumns(uint8_t* state)
{
	uint8_t i, j;
	uint8_t column[4];
	for (i = 0; i < 4; i++)
	{
		for (j = 0; j < 4; j++)
		{
			column[j] = state[(i * 4) + j];
		}
		mixColumn(column);
		for (j = 0; j < 4; j++)
		{
			state[(i * 4) + j] = column[j];
		}
	}
}

// In this method, the rows are rotated by the number of times specified.
static void shiftRow(uint8_t* row, uint8_t num)
{
	uint8_t i;
	uint8_t temp;
	// shift left n time
	for (i = 0; i < num; i++)
	{
		// shift right once
		temp = row[0];
		row[0] = row[1];
		row[1] = row[2];
		row[2] = row[3];
		row[3] = temp;
	}
}

static void shiftRows(uint8_t* state)
{
	uint8_t i, j;
	uint8_t mat[4][4];
	//First create a matrix of 4 rows and 4 columns from the state which is stored in 1 D
	for (i = 0; i < 4; i++)
		for (j = 0; j < 4; j++)
			mat[j][i] = state[i * 4 + j];
	// Shift the rows based on the row number
	for (i = 0; i < 4; i++)
	{
		shiftRow(mat[i], i);
	}
	//The matrix is converted back to the 1 D array and stored in state
	for (i = 0; i < 4; i++)
		for (j = 0; j < 4; j++)
			state[i * 4 + j] = mat[j][i];
}

// AddRoundKey method, the key in that round is XORed with the state
static void addRoundKey(uint8_t* state, uint8_t* key)
{
	uint8_t i;
	for (i = 0; i < 16; i++)
	{
		state[i] = state[i] ^ key[i];
	}
}

//Performs the first operation in AES-128, where every value in the state is replaced by its associated sBoxValue
static void subsitute_Bytes(uint8_t* state)
{
	uint8_t i;
	for (i = 0; i < 16; i++)
	{
		state[i] = getSBoxValue(state[i]);
	}
}
//The cipher key is a 176 byte length array. We need 16 bytes for each round. 
// getRoundKey function returns a 16 byte key for each round
static void getRoundKey(uint8_t* expanded_key, uint8_t* round_key)
{
	uint8_t i;
	for (i = 0; i < 16; i++)
	{
		round_key[i] = expanded_key[i];
	}
}
/* Under the 16-byte key at k, encrypt the 16-byte plaintext at p and store it at c. */
static void Cipher(uint8_t* c, uint8_t* p, uint8_t* k) {
	uint8_t expanded_key[176];
	uint8_t round_key[16];
	uint8_t state[16];
	uint8_t i, j;
	for (i = 0; i < 16; i++)
	{
		expanded_key[i] = k[i];
		state[i] = p[i];
	}
	//First generate the keys required for all 10 rounds
	key_expansion(expanded_key);
	//Get the key for the inital round
	getRoundKey(expanded_key, round_key);
	//Perform the add round key first
	addRoundKey(state, round_key);
	// Perform four functions 9 times.
	for (j = 1; j < 10; j++)
	{
		getRoundKey(expanded_key + 16 * j, round_key); //  Gets the key required for that particular round
		subsitute_Bytes(state);
		shiftRows(state);
		mixColumns(state);
		addRoundKey(state, round_key);
	}
	// For the last round mix columns is not performed
	getRoundKey(expanded_key + 16 * 10, round_key);
	subsitute_Bytes(state);
	shiftRows(state);
	addRoundKey(state, round_key);
	// Finally the values are stored in array c (pointer)
	for (i = 0; i < 16; i++)
	{
		c[i] = state[i];
	}
}

/*--------------------------------------------------------------*/
/*-------------------DECRYPTION FUNCTION------------------------*/
/*--------------------------------------------------------------*/

static void invSubByte(uint8_t* state)
{
	uint8_t i;
	for (i = 0; i < 16; i++)
	{
		state[i] = getSBoxInvert(state[i]);
	}
}

static void invShiftRow(uint8_t* row, uint8_t num)
{
	uint8_t i;
	uint8_t temp;
	// shift right n time 
	for (i = 0; i < num; i++)
	{
		// shift right once
		temp = row[0];
		row[0] = row[3];
		row[3] = row[2];
		row[2] = row[1];
		row[1] = temp;
	}
}

static void invShiftRowS(uint8_t* state)
{
	uint8_t i, j;
	uint8_t mat[4][4];
	for (i = 0; i < 4; i++)
		for (j = 0; j < 4; j++)
			mat[j][i] = state[i * 4 + j];
	for (i = 0; i < 4; i++)
	{
		invShiftRow(mat[i], i);
	}
	for (i = 0; i < 4; i++)
		for (j = 0; j < 4; j++)
			state[i * 4 + j] = mat[j][i];
}

static uint8_t Multiply(uint8_t x, uint8_t y)
{
	return (
			((y & 1) * x) ^
			((y >> 1 & 1) * xtime(x)) ^
			((y >> 2 & 1) * xtime(xtime(x))) ^
			((y >> 3 & 1) * xtime(xtime(xtime(x))))
			// ^ ((y >> 4 & 1) * xtime(xtime(xtime(xtime(x)))))
			);
}
/* 	9 have bit 1001
	(y & 1) * x) = X										1001 & 1 = 1
	(y >> 1 & 1) * xtime(x) = 0								shift right once 100 & 1 = 0
	(y >> 2 & 1) * xtime(xtime(x)) = 0						shift right 2 times 10 & 1 = 0
	(y >> 3 & 1) * xtime(xtime(xtime(x))) = ((X.2).2).2		shift right 3 times 1 & 1 = 1
	(y >> 4 & 1) * xtime(xtime(xtime(xtime(x)))) = 0		shift right 4 times	0 & 1 = 0
	((X.2).2).2 xor X
*/

/* 	14 have bit 1110
	(y & 1) * x) = 0										1110 & 1 = 0
	(y >> 1 & 1) * xtime(x) = X.2							shift right once 111 & 1 = 1
	(y >> 2 & 1) * xtime(xtime(x)) = (X.2).2				shift right 2 times 11 & 1 = 1
	(y >> 3 & 1) * xtime(xtime(xtime(x))) = ((X.2).2).2		shift right 3 times 1 & 1 = 1
	(y >> 4 & 1) * xtime(xtime(xtime(xtime(x)))) = 0		shift right 4 times	0 & 1 = 0
	X.2 xor (X.2).2 xor ((X.2).2).2 
*/

/*
  	X . 9  = ((( X . 2) . 2) . 2) xor X 
	X . 11 = ((((X . 2) . 2) + X) . 2) xor X
	X . 13 = ((((X . 2) xor X) . 2) . 2) xor X
	X . 14 = ((((X . 2) xor X) . 2) xor x) . 2	*/

static void invMixColumn(uint8_t* state)
{
	uint8_t i, j;
	uint8_t matrix[4][4];
	for (i = 0; i < 4; i++)
		for (j = 0; j < 4; j++)
			matrix[j][i] = state[i * 4 + j];
	
	uint8_t a, b, c, d;
	for (i = 0; i < 4; i++) {
		a = matrix[0][i];
		b = matrix[1][i];
		c = matrix[2][i];
		d = matrix[3][i];

		matrix[0][i] = Multiply(a, 0x0e) ^ Multiply(b, 0x0b) ^ Multiply(c, 0x0d) ^ Multiply(d, 0x09);
		matrix[1][i] = Multiply(a, 0x09) ^ Multiply(b, 0x0e) ^ Multiply(c, 0x0b) ^ Multiply(d, 0x0d);
		matrix[2][i] = Multiply(a, 0x0d) ^ Multiply(b, 0x09) ^ Multiply(c, 0x0e) ^ Multiply(d, 0x0b);
		matrix[3][i] = Multiply(a, 0x0b) ^ Multiply(b, 0x0d) ^ Multiply(c, 0x09) ^ Multiply(d, 0x0e);
	}

	for (i = 0; i < 4; i++)
		for (j = 0; j < 4; j++)
			state[i * 4 + j] = matrix[j][i];
}

static void invCipher(uint8_t* c, uint8_t* p, uint8_t* k)
{
	uint8_t expanded_key[176];
	uint8_t round_key[16];
	uint8_t state[16];
	uint8_t i, round;
	for (i = 0; i < 16; i++)
	{
		expanded_key[i] = k[i];
		state[i] = p[i];
	}
	key_expansion(expanded_key);
	getRoundKey(expanded_key + 160, round_key);
	addRoundKey(state, round_key);
	for (round = 9; ; round--)
	{
		invShiftRowS(state);
		invSubByte(state);
		getRoundKey(expanded_key + 16 * round, round_key);
		addRoundKey(state, round_key);
		if (round == 0) {
			break;
		}
		invMixColumn(state);
	}
	for (i = 0; i < 16; i++)
	{
		c[i] = state[i];
	}
}
