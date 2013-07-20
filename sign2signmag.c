/*
	PCM Format Converter

	Copyright (c) 2013, Tristan Seifert
	All rights reserved.

	Redistribution and use in source and binary forms, with or without modification, are permitted provided that the following conditions are met:

	Redistributions of source code must retain the above copyright notice, this list of conditions and the following disclaimer.
	Redistributions in binary form must reproduce the above copyright notice, this list of conditions and the following disclaimer in the documentation and/or other materials provided with the distribution.
	THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS "AS IS" AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE ARE DISCLAIMED. IN NO EVENT SHALL THE COPYRIGHT HOLDER OR CONTRIBUTORS BE LIABLE FOR ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES; LOSS OF USE, DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND ON ANY THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE OF THIS SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.
*/

#include "stdio.h"
#include "stdint.h"
#include "stdlib.h"
#include <string.h>

int main(int argc, char* argv[]) {
	if(argc != 3) {
		printf("Usage: sign2signmag [in] [out]\n");
		return 0;
	}

	FILE *in = fopen(argv[1], "rb");
	if(!in) {
		perror("Could not open input file");
		return 1;
	}

	// get file size
	fseek(in, 0L, SEEK_END);
	int sz = ftell(in);
	fseek(in, 0L, SEEK_SET);

	// allocate memory and read
	uint8_t *buf = malloc(sizeof(uint8_t) * sz + 1);
	fread(buf, sizeof(uint8_t), sz, in);

	// open output file
	FILE *out = fopen(argv[2], "wb");
	if(!out) {
		perror("Could not open output file");
		return 1;
	}	

	// flip sign bits
	uint8_t *buf_write = buf;
	for(int i = 0; i < sz; i++) {
		*buf_write ^= 0x80;
		buf_write++;
	}

	int bytesWritten = fwrite(buf, sizeof(uint8_t), sz, out);

	printf("Converted $%X bytes of data, wrote file with $%X bytes of data.\n", sz, bytesWritten);

	// clean up
	fclose(in);
	fclose(out);

	free(buf);
}