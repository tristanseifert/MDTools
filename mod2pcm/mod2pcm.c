/*
	MOD2PCM, converts MOD files to the format needed by the PCM driver
	
	Copyright (c) 2013, Tristan Seifert
	All rights reserved.

	Redistribution and use in source and binary forms, with or without modification, are permitted provided that the following conditions are met:

	Redistributions of source code must retain the above copyright notice, this list of conditions and the following disclaimer.
	Redistributions in binary form must reproduce the above copyright notice, this list of conditions and the following disclaimer in the documentation and/or other materials provided with the distribution.
	THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS "AS IS" AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE ARE DISCLAIMED. IN NO EVENT SHALL THE COPYRIGHT HOLDER OR CONTRIBUTORS BE LIABLE FOR ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES; LOSS OF USE, DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND ON ANY THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE OF THIS SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.
*/

#include <stdio.h>
#include <stdlib.h>
#include <math.h>
#include <string.h>

#include "mod2pcm.h"

int main(int argc, char *argv[]) {	
    if ( argc != 2 )  {
        printf("usage: %s filename\n", argv[0]);
        return 255;
    }
    
	printf(ANSI_BOLD "MOD2PCM Version 1.0\n" ANSI_RESET);
	
	FILE *modFile = fopen(argv[1], "rw");
	
	if(modFile == NULL) {
		perror(ANSI_COLOR_RED "Couldn't open module file" ANSI_RESET);
		return 254;
	}
	
	modfile_header fileInformation;
	populateModInfoStruct(modFile, &fileInformation);
	
	// this is kinda bad but I'm too lazy to figure it out other ways
	fileInformation.numChannels = 4;
	
	printf("Reading file \"%s\"...\n", fileInformation.title);
	printf("\t> File has %i instruments.\n", fileInformation.num_instruments);
	
	for(int i = 0; i < fileInformation.num_instruments; i++) {
		mod_sample *currentSample = fileInformation.samples[i];
		printf("\t> Instrument \"%s\", length 0x%X, volume 0x%X, finetume 0x%X, loop start 0x%X, loop length 0x%X\n", 
			   currentSample->title, currentSample->sample_length, currentSample->sample_volume, currentSample->sample_finetune,
			   currentSample->sample_loopstart, currentSample->sample_looplen);
	}

	printf(ANSI_BOLD "\nPattern table:\n" ANSI_RESET);

	fileInformation.highestPattern = 0x00;

	int w = 0;
	for(int i = 0; i < 128; i++) {
		uint8_t x = fileInformation.patternSequences[i];
		printf("0x%.02X ", x);
		
		if(x > fileInformation.highestPattern) {
			fileInformation.highestPattern = x;
		}
		
		w++;
		if(w == 0x10) {
			printf("\n");
			w = 0;
		}
	}
		
	printf("\nHighest pattern value is 0x%.02X.\n\n", fileInformation.highestPattern);
	
	fileInformation.sampleOffset = ((fileInformation.numChannels * 4 * 64) * (fileInformation.highestPattern + 1)) + 0x43C;
	
	printf(ANSI_BOLD "Reading instrument data...\n" ANSI_RESET);
	readSampleData(modFile, &fileInformation);
	
	printf(ANSI_BOLD "Generating PCM sample bank...\n" ANSI_RESET);
	uint8_t *sampleBankData = createPCMSampleBank(&fileInformation);
}

// Helper functions
void populateModInfoStruct(FILE* fp, modfile_header *header) {
	rewind(fp); // rewind to start of file

	fread(header->title, sizeof(char), 20, fp); // Read title of file.
	
	// check how many instruments we have.
	fseek(fp, 0x438, SEEK_SET);
	fread(header->identification, sizeof(char), 4, fp);
	
	if(strcmp("M.K.", header->identification) == 0 || strcmp("8CHN", header->identification) == 0 ||
	   strcmp("4CHN", header->identification) == 0 || strcmp("6CHN", header->identification) == 0 ||
	   strcmp("FLT4", header->identification) == 0 || strcmp("FLT8", header->identification) == 0) {
	// We've got 31 instruments.
		header->num_instruments = 31;
	} else {
		header->num_instruments = 15;	
	}
	
	// Load instruments now.
	fseek(fp, 0x14, SEEK_SET);
	mod_sample* currentSample;
	
	for(int i = 0; i < header->num_instruments; i++) {
		header->samples[i] = (mod_sample *) malloc(sizeof(mod_sample));
		currentSample = header->samples[i];
		
		fread(currentSample->title, sizeof(char), 22, fp); // Read title of sample.
		
		currentSample->sample_length = read_word(fp) << 0x01; // Read sample length
		
		currentSample->sample_finetune = read_byte(fp); // Read finetune.
		currentSample->sample_volume = read_byte(fp); // Read volume.
		
		currentSample->sample_loopstart = read_word(fp) << 0x01; // Read loop start.
		currentSample->sample_looplen = read_word(fp) << 0x01; // Read loop end.
	}
	
	fseek(fp, 0x3B6, SEEK_SET); // Pattern number
	header->num_patterns = read_byte(fp);
	fseek(fp, 0x3B8, SEEK_SET); // First byte of pattern sequence
	
	fread(header->patternSequences, sizeof(char), 128, fp); // Read pattern sequences.
	
//	printf("0x%X 0x%X 0x%X 0x%X\n", header->patternSequences[0], header->patternSequences[1], header->patternSequences[2], header->patternSequences[3]);
}

// This function reads the sample data from the file.
void readSampleData(FILE* fp, modfile_header *header) {
	mod_sample *currentSample;
	
	uint32_t offset = header->sampleOffset;
	
	for(int i = 0; i < header->num_instruments; i++) {
		currentSample = header->samples[i];
		fseek(fp, offset, SEEK_SET);
		
		// Allocate some memory
		currentSample->sampleData = calloc(currentSample->sample_length, 1);
		
		printf("Allocated 0x%.04X bytes for instrument %.02i...", currentSample->sample_length, i);
		printf(" Got memory at 0x%X.\n", (unsigned int) currentSample->sampleData);
	
		fread(currentSample->sampleData, sizeof(currentSample->sampleData), currentSample->sample_length, fp); // Read the sample.
		
		offset += currentSample->sample_length;
	}
}

// Create PCM sample bank
uint8_t* createPCMSampleBank(modfile_header *header) {
	uint8_t *buffer;
	
	uint8_t usableInstruments = 0x00;
	uint32_t instrumentSize = 0x00000000;
	
	// Gather total number of actual instruments and their total size.
	mod_sample* currentSample;
	for(int i = 0; i < header->num_instruments; i++) {
		currentSample = header->samples[i];
		
		if(currentSample->sample_length != 0) {
			usableInstruments++;
			instrumentSize += currentSample->sample_length;
		}
	}
	
	uint32_t neededMemory = instrumentSize + (usableInstruments * 8);
	printf("Allocating 0x%X bytes of memory for PCM bank...", neededMemory);
	
	buffer = calloc(neededMemory, 1);
	printf(" Got memory at 0x%X.\n", buffer);
	
	return buffer;
}

// Inline tools functions
inline uint16_t word_swap(uint16_t in) {
	return ((in & 0xFF) << 0x08) | ((in & 0xFF00) >> 0x08);
}

// The functions below read big endian
inline uint8_t read_byte(FILE *fp) {
	unsigned char buffer[1];
	fread(buffer, sizeof(char), 1, fp); // Read a byte.
	
	return buffer[0]; 
}

inline uint16_t read_word(FILE *fp) {
	unsigned char buffer[2];
	fread(buffer, sizeof(char), 2, fp); // Read a word.
	
	return buffer[1] | buffer[0] << 0x08; 
}

inline uint32_t read_long(FILE *fp) {
	unsigned char buffer[4];
	fread(buffer, sizeof(char), 4, fp); // Read a longword.
	
	return buffer[3] | buffer[2] << 0x08 | buffer[1] << 0x10 | buffer[0] << 0x18; 
}