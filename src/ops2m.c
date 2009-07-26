#include <ctype.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "mytypes.h"

// Application's name and current version
#define APP_NAME	"ops2m"
#define APP_VERSION	"1.1"

enum {
	MODE_NEW,
	MODE_DEC_OLD,
	MODE_ENC_OLD
};

void ShowBanner(void)
{
	printf("OPS2M Config File Cryptor v%s\n", APP_VERSION);
	printf("Copyright (C) 2006 misfire\n\n");
}

void ShowUsage(void)
{
	printf("Usage: %s <input file> <output file> [options]\n", APP_NAME);
	printf(" Decrypts/encrypts the config file of OPS2M demo discs (\\SCEE_DD\\CONFIG.TXT)\n");
	printf(" Options are:\n");
	printf("  -d\tDecrypt using old scheme (\\SCEE_DD\\CONFIG.DD2)\n");
	printf("  -e\tEncrypt using old scheme\n");
}

void decrypt_old(u8 *buf, int size)
{
	char key[] = "SCEEDEMO2DISC"; // Initial key
	const int keylen = strlen(key);
	int i, j;

	for (i = 0, j = 0; i < size; i++, j++) {
		if (j == keylen) {
			// New key = previous decrypted block
			memcpy(key, &buf[i-keylen], keylen);
			j = 0;
		}
		buf[i] ^= key[j];
	}
}

void encrypt_old(u8 *buf, int size)
{
	char key[] = "SCEEDEMO2DISC"; // Initial key
	const int keylen = strlen(key);
	u8 tmp[keylen];
	int i, j;

	for (i = 0, j = 0; i < size; i++, j++) {
		if (j == keylen) {
			// New key = previous block
			memcpy(key, tmp, keylen);
			j = 0;
		}
		tmp[j] = buf[i];
		buf[i] ^= key[j];
	}
}

void crypt_new(u8 *buf, int size)
{
	const char key[] = "Sir Robert McAlpine Fine Cranes";
	int i;

	for (i = 0; i < size; i++)
		buf[i] ^= key[i % strlen(key)];
}

int main(int argc, char *argv[])
{
	FILE *fp;
	u8 *buf;
	int filesize;
	int mode = MODE_NEW;

	ShowBanner();

	// Check arguments
	if (argc < 3) {
		ShowUsage();
		return -1;
	}

	if ((argc == 4) && (argv[3][0] == '-')) {
		switch (tolower(argv[3][1])) {
			case 'd':
				mode = MODE_DEC_OLD;
				break;
			case 'e':
				mode = MODE_ENC_OLD;
				break;
		}
	}

	// Open input file
	fp = fopen(argv[1], "rb");
	if (fp == NULL) {
		fprintf(stderr, "Error: Can't open file '%s' for reading\n", argv[1]);
		return -2;
	}

	// Get size of input file
	fseek(fp, 0, SEEK_END);
	filesize = ftell(fp);

	// Allocate memory for file contents
	buf = (u8*)malloc(filesize);
	if (buf == NULL) {
		fprintf(stderr, "Error: Unable to allocate %i bytes\n", filesize);
		fclose(fp);
		return -3;
	}

	// Read from input file into buffer
	fseek(fp, 0, SEEK_SET);
	fread(buf, filesize, 1, fp);
	fclose(fp);

	// Open output file
	fp = fopen(argv[2], "wb");
	if (fp == NULL) {
		fprintf(stderr, "Error: Can't open file '%s' for writing\n", argv[2]);
		return -4;
	}

	// Decrypt/encrypt
	switch (mode) {
		case MODE_NEW:
			printf("Using new scheme.\nDecrypting/encrypting file... ");
			crypt_new(buf, filesize);
			break;

		case MODE_DEC_OLD:
			printf("Using old scheme.\nDecrypting file... ");
			decrypt_old(buf, filesize);
			break;

		case MODE_ENC_OLD:
			printf("Using old scheme.\nEncrypting file... ");
			encrypt_old(buf, filesize);
			break;
	}

	// Write result to output file
	fwrite(buf, filesize, 1, fp);
	fclose(fp);
	free(buf);

	printf("Done!\n");
	printf("File size = %i bytes\n", filesize);

	return 0;
}
