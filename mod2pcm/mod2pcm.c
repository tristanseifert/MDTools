/*
	MOD2PCM, converts MOD files to the format needed by the PCM driver
	
	Copyright (c) 2013, Tristan Seifert
	All rights reserved.

	Redistribution and use in source and binary forms, with or without modification, are permitted provided that the following conditions are met:

	Redistributions of source code must retain the above copyright notice, this list of conditions and the following disclaimer.
	Redistributions in binary form must reproduce the above copyright notice, this list of conditions and the following disclaimer in the documentation and/or other materials provided with the distribution.
	THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS "AS IS" AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE ARE DISCLAIMED. IN NO EVENT SHALL THE COPYRIGHT HOLDER OR CONTRIBUTORS BE LIABLE FOR ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES; LOSS OF USE, DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND ON ANY THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE OF THIS SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.

	Notes:
	http://www.aes.id.au/modformat.html has useful information about the MOD file format.
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
	// fileInformation.numChannels = 4;
	
	printf("Reading file \"%s\"...\n", fileInformation.title);
	printf("\t> File has %i sampless and %i channels.\n", fileInformation.num_instruments, fileInformation.numChannels);
	
	if(fileInformation.numChannels > 8) {
		printf(ANSI_COLOR_RED "Files with more than 8 channels are not supported at this time.\n" ANSI_RESET);
		return 255;
	} else if(fileInformation.numChannels == 0) {
		printf(ANSI_COLOR_RED "The number of channels in this file could not be determined. Check that the file isn't corrupted and try again. (File ID is %.04s)\n" ANSI_RESET,
			   fileInformation.identification);
		printf(ANSI_COLOR_RED "Enter number of channels or ^C: ");
		
		scanf("%i", &fileInformation.numChannels);

	}
	
	for(int i = 0; i < fileInformation.num_instruments; i++) {
		mod_sample *currentSample = fileInformation.samples[i];
		printf("\t> Sample \"%s\", length 0x%X, volume 0x%X, finetume 0x%X, loop start 0x%X, loop length 0x%X\n", 
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
	
	printf(ANSI_BOLD "Reading sample data...\n" ANSI_RESET);
	readSampleData(modFile, &fileInformation);
	
	printf(ANSI_BOLD "\nGenerating PCM sample bank...\n" ANSI_RESET);
	uint8_t *sampleBankData = createPCMSampleBank(&fileInformation);

	printf("\nSample bank is %i bytes.\n", fileInformation.sampleBankSize);

	FILE *pcmBank = fopen("PCM_____.BNK", "w");
	fwrite(sampleBankData, 1, fileInformation.sampleBankSize, pcmBank); // Write sample bank.
	fclose(pcmBank);
	
	printf(ANSI_BOLD "\nConverting module pattern data...\n" ANSI_RESET);
	readPatternData(modFile, &fileInformation);

//	while(1);
}

// Helper functions
void populateModInfoStruct(FILE* fp, modfile_header *header) {
	rewind(fp); // rewind to start of file

	fread(header->title, sizeof(char), 20, fp); // Read title of file.
	
	// check how many samples we have.
	fseek(fp, 0x438, SEEK_SET);
	fread(header->identification, sizeof(char), 4, fp);
	
	header->identification[4] = 0x00; // I derped =V
	
	if(strcmp("M.K.", header->identification) == 0 || strcmp("8CHN", header->identification) == 0 ||
	   strcmp("4CHN", header->identification) == 0 || strcmp("6CHN", header->identification) == 0 ||
	   strcmp("FLT4", header->identification) == 0 || strcmp("FLT8", header->identification) == 0) {
	// We've got 31 samples.
		header->num_instruments = 31;
	} else {
		header->num_instruments = 15;	
	}
	
	header->numChannels = 0; // Assume we can't determine the number of channels
	
	if(strcmp("M.K.", header->identification) == 0 || strcmp("4CHN", header->identification) == 0 || 
	   strcmp("FLT4", header->identification) == 0) { // 4 channels
		header->numChannels = 4;
	} else if(strcmp("6CHN", header->identification) == 0) { // 6 channels
		header->numChannels = 6;	
	} else if(strcmp("8CHN", header->identification) == 0 || strcmp("FLT8", header->identification) == 0) { // 8 channels
		header->numChannels = 8;	
	}
	
	// Load samples now.
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
		
		printf("Allocated 0x%.04X bytes for sample %.02i...", currentSample->sample_length, i);
		printf(" Got memory at 0x%X.\n", (unsigned int) currentSample->sampleData);
	
		fread(currentSample->sampleData, sizeof(currentSample->sampleData), currentSample->sample_length, fp); // Read the sample.
		
		offset += currentSample->sample_length;
	}
}

// Create PCM sample bank
uint8_t* createPCMSampleBank(modfile_header *header) {
	uint8_t *buffer;
	uint8_t *outputPointer;
	
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
	printf(" Got memory at 0x%X.\n", (unsigned int) buffer);
	outputPointer = buffer; // this way we can shit all over buffer
	
	header->sampleBankSize = neededMemory;
	
	uint32_t sampleOffset = (usableInstruments * 8) + 8;
	
	uint32_t *bufferPtr = (uint32_t *) buffer;
	
	// Write out sample header.
	for(int i = 0; i < header->num_instruments; i++) {
		currentSample = header->samples[i];
		
		if(currentSample->sample_length != 0) {
			printf("Sample %.02i: SOff: 0x%.05X, SLen: 0x%.05X\n", i, sampleOffset, currentSample->sample_length);
		
			*bufferPtr = longword_swap(sampleOffset);
			bufferPtr++;
			*bufferPtr = longword_swap(currentSample->sample_length);
			bufferPtr++;
			
			sampleOffset += currentSample->sample_length;
		}
	}
	
	buffer =  (uint8_t *) bufferPtr;
	
	// Write actual sample data.
	for(int i = 0; i < header->num_instruments; i++) {
		currentSample = header->samples[i];
		
		if(currentSample->sample_length != 0) {		
			uint8_t *sampleArea = currentSample->sampleData;
				
			for(int w = 0; w < currentSample->sample_length; w++) {
				*buffer = *sampleArea;
				
				sampleArea++;
				buffer++;
			}
		}
	}
	
	return outputPointer;
}

void readPatternData(FILE *fp, modfile_header *header) {
	// First, we need to allocate all the memory we need.
	
	for(int i = 0; i < header->highestPattern; i++) {
		header->patterns[i] = calloc(sizeof(mod_pattern), 1);
		printf("Allocated %i bytes of memory for pattern data at 0x%X.\n", (int) sizeof(mod_pattern), (unsigned int) header->patterns[i]);
	}
	
	fseek(fp, 0x43C, SEEK_SET); // Seek to place in file that's the start of pattern data
	
	/*
	 * Format of note entry in a pattern: 
	 * (MSB) wwww xxxxxxxxxxxxxx yyyy zzzzzzzzzzzzzz (LSB)
	 * 
     * wwwwyyyy (8 bits) is the sample for this channel/division
	 * xxxxxxxxxxxx (12 bits) is the sample's period (or effect parameter)
	 * zzzzzzzzzzzz (12 bits) is the effect for this channel/division
	 */
	
	for(int p = 0; p < header->highestPattern; p++) {
		for(int r = 0; r < 64; r++) {
			for(int c = 0; c < header->numChannels; c++) {			
				uint32_t buffer = read_long(fp);
			
				header->patterns[p]->noteData[c][r] = malloc(sizeof(mod_note));
			
				header->patterns[p]->noteData[c][r]->period = (buffer & 0xFFF0000) >> 0x10;
				header->patterns[p]->noteData[c][r]->instrument = ((buffer & 0xF0000000) >> 0x1C) | ((buffer & 0xF000) >> 0x0C);
				header->patterns[p]->noteData[c][r]->fxCommand = (buffer & 0xF00) >> 0x08;
				header->patterns[p]->noteData[c][r]->fxData = (buffer & 0xFF);
			}
		}
	}
}

// Inline tools functions
inline uint16_t word_swap(uint16_t in) {
	return ((in & 0xFF) << 0x08) | ((in & 0xFF00) >> 0x08);
}

inline uint32_t longword_swap(uint32_t val) {
    return ((val >> 24) & 0xFF) | ((val << 8) & 0xFF0000) | ((val >> 8) & 0xFF00) | ((val << 24) & 0xFF000000);
}

inline void pointer_Write32To8(uint32_t val, uint8_t *ptr) {
	*ptr = (val >> 24) & 0xFF;
	ptr++;
	*ptr = (val >> 16) & 0xFF;
	ptr++;
	*ptr = (val >> 8) & 0xFF;
	ptr++;
	*ptr = (val & 0xFF);
	ptr++;
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