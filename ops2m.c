/*
 * ops2m.c - decrypt/encrypt config file of OPS2M demo discs
 *
 * Copyright (C) 2006, 2009 misfire <misfire@xploderfreax.de>
 *
 * ops2m is free software: you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation, either version 3 of the License, or
 * (at your option) any later version.
 *
 * ops2m is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with ops2m.  If not, see <http://www.gnu.org/licenses/>.
 */

#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#define APP_NAME	"ops2m"
#define APP_VERSION	"1.1"

enum {
	MODE_NEW,
	MODE_DEC_OLD,
	MODE_ENC_OLD
};

static void decrypt_old(unsigned char *buf, int size)
{
	char key[] = "SCEEDEMO2DISC"; /* Initial key */
	const int keylen = strlen(key);
	int i, j;

	for (i = 0, j = 0; i < size; i++, j++) {
		if (j == keylen) {
			/* New key = previous decrypted block */
			memcpy(key, &buf[i-keylen], keylen);
			j = 0;
		}
		buf[i] ^= key[j];
	}
}

static void encrypt_old(unsigned char *buf, int size)
{
	char key[] = "SCEEDEMO2DISC"; /* Initial key */
	const int keylen = strlen(key);
	unsigned char tmp[keylen];
	int i, j;

	for (i = 0, j = 0; i < size; i++, j++) {
		if (j == keylen) {
			/* New key = previous block */
			memcpy(key, tmp, keylen);
			j = 0;
		}
		tmp[j] = buf[i];
		buf[i] ^= key[j];
	}
}

static void crypt_new(unsigned char *buf, int size)
{
	static const char key[] = "Sir Robert McAlpine Fine Cranes";
	int i;

	for (i = 0; i < size; i++)
		buf[i] ^= key[i % strlen(key)];
}

int main(int argc, char *argv[])
{
	FILE *fp = NULL;
	unsigned char *buf = NULL;
	int filesize;
	int mode = MODE_NEW;

	if (argc < 3) {
		fprintf(stderr, "usage: %s <input file> <output file> [-d|-e]\n", argv[0]);
		return EXIT_FAILURE;
	}

	if ((argc > 3) && (argv[3][0] == '-')) {
		switch (argv[3][1]) {
		case 'd':
			mode = MODE_DEC_OLD;
			break;
		case 'e':
			mode = MODE_ENC_OLD;
			break;
		}
	}

	fp = fopen(argv[1], "rb");
	if (fp == NULL) {
		fprintf(stderr, "Error: could not open input file '%s'\n", argv[1]);
		return EXIT_FAILURE;
	}

	fseek(fp, 0, SEEK_END);
	filesize = ftell(fp);
	buf = (unsigned char*)malloc(filesize);
	if (buf == NULL) {
		fprintf(stderr, "Error: could not allocate %i bytes\n", filesize);
		fclose(fp);
		return EXIT_FAILURE;
	}

	fseek(fp, 0, SEEK_SET);
	if (fread(buf, filesize, 1, fp) != 1) {
		fprintf(stderr, "Error: could not read from input file\n");
		fclose(fp);
		return EXIT_FAILURE;
	}

	fclose(fp);
	fp = fopen(argv[2], "wb");
	if (fp == NULL) {
		fprintf(stderr, "Error: could not open output file '%s'\n", argv[2]);
		return EXIT_FAILURE;
	}

	switch (mode) {
	case MODE_NEW:
		crypt_new(buf, filesize);
		break;
	case MODE_DEC_OLD:
		decrypt_old(buf, filesize);
		break;
	case MODE_ENC_OLD:
		encrypt_old(buf, filesize);
		break;
	}

	fwrite(buf, filesize, 1, fp);
	fclose(fp);
	free(buf);

	return EXIT_SUCCESS;
}
