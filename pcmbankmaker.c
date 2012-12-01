/*
	PCM Bank Generator

	Copyright (c) 2012, Tristan Seifert
	All rights reserved.

	Redistribution and use in source and binary forms, with or without modification, are permitted provided that the following conditions are met:

	Redistributions of source code must retain the above copyright notice, this list of conditions and the following disclaimer.
	Redistributions in binary form must reproduce the above copyright notice, this list of conditions and the following disclaimer in the documentation and/or other materials provided with the distribution.
	THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS "AS IS" AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE ARE DISCLAIMED. IN NO EVENT SHALL THE COPYRIGHT HOLDER OR CONTRIBUTORS BE LIABLE FOR ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES; LOSS OF USE, DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND ON ANY THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE OF THIS SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.

	Special Thanks go out to GerbilSoft for helping me fix a fucking memory bug I didn't notice.
*/

#include "stdio.h"
#include "stdint.h"
#include "stdlib.h"
#include "math.h"
#include <sys/types.h>
#include <sys/stat.h>
#include <unistd.h>
#include <dirent.h>
#include <string.h>

#include "pcmbankmaker.h"

int main(void) {
	char sampleDir[256];
	DIR *dp;
    struct dirent *ep;
    struct dirent *samples[1024];
    int sampleIndex = 0;
    int numberOfSamples = 0;
	char *fileBuffer;
   	char filename[1025+256];
	uint32_t sampleHeader[2];	// used as a temporary buffer for fwrite
	uint32_t sampleHeadOffset;

	printf(ANSI_COLOR_GREEN ANSI_BOLD "Tristan Seifert's Magical PCM Bank Generator™\n\n" ANSI_RESET);
	
	printf(ANSI_BOLD "Which directory contains the samples you wish to add to the bank? " ANSI_RESET);
	scanf("%s", &sampleDir);
	printf(ANSI_RESET "\n" ANSI_BOLD "Searching directory '%s' for samples...\n" ANSI_RESET, sampleDir);

	memset(samples, 0x00, sizeof(samples));
    dp = opendir(sampleDir);
    
	if (dp != NULL) {
		while ((ep = readdir(dp))) {
			printf(ANSI_BOLD ANSI_COLOR_GREEN "Found sample: " ANSI_RESET);
			
			if(strcmp(".", ep->d_name) == 0 || strcmp(ep->d_name, "..") == 0 || strcmp(ep->d_name, ".DS_Store") == 0) {
				printf("Skipping '..' '.DS_Store' or '.'\n");
			} else {
        		puts(ep->d_name);
        		samples[sampleIndex] = (struct dirent*)malloc(sizeof(*samples[sampleIndex]));
				memcpy(samples[sampleIndex], ep, sizeof(*samples[sampleIndex]));
        		sampleIndex++;
        		numberOfSamples++;
        	}
		}
        
        closedir (dp);
	} else {
		perror(ANSI_COLOR_RED "Couldn't open the samples directory");
		return 250;
	}
	
	printf(ANSI_BOLD "\n\nProcessing all samples into file... (will be output as PCM_____.BNK)\n" ANSI_RESET);
	
	FILE *pcmBankFile = fopen("PCM_____.BNK", "w+b");
	FILE *sampleFile;
	
	if(pcmBankFile == NULL) {
		printf(ANSI_COLOR_RED ANSI_BOLD "Could not create file handle for bank. Exiting..." ANSI_RESET);
		return 255;
	}
		
	// Each header entry is 8 bytes long (4 byte offset, 4 byte length)
	sampleHeadOffset = numberOfSamples * 8; 
		
	for(int i = 0; i < sampleIndex; i++) {	
   		char filename[1025+256];
		snprintf(filename, sizeof(filename), "%s/%s", sampleDir, samples[i]->d_name);
		printf(ANSI_BOLD "\tProcessing %s... " ANSI_RESET, filename);

		sampleHeader[0] = swap_uint32(sampleHeadOffset);
		sampleHeader[1] = swap_uint32(getFileSize(filename));
		fwrite(&sampleHeader, sizeof(uint32_t), 2, pcmBankFile);
	
	
		printf("Placed sample at $%X (Size $%X)\n", swap_uint32(sampleHeader[0]), getFileSize(filename));
		
		sampleHeadOffset += getFileSize(filename);
	}
	
	printf(ANSI_BOLD "\n\tWriting Header to file...\n\n" ANSI_RESET);
	
	printf(ANSI_BOLD "\tWriting samples to file...\n" ANSI_RESET);
		
	for(int j = 0; j < sampleIndex; j++) {	
		printf("\t\tWriting sample '%s'...\n", samples[j]->d_name);
		snprintf(filename, sizeof(filename), "%s/%s", sampleDir, samples[j]->d_name);
		
		int fileSize = getFileSize(filename);
		fileBuffer = (char *) malloc(sizeof(char)  * fileSize);
		
		FILE *pcmSampleFile = fopen(filename, "r");	
		
		if(pcmSampleFile != NULL) {
			printf("\t\t\tOpened File...\n");
			rewind(pcmSampleFile);
		
			printf("\t\t\tReading File...\n");
		
			fread(fileBuffer, sizeof(char), fileSize, pcmSampleFile);
			fwrite(fileBuffer, 1, fileSize, pcmBankFile);
		} else {
			printf(ANSI_COLOR_RED "\n\t\t\tCould not read file %s.\n" ANSI_RESET, filename);
			return 253;
		}
		
		printf(ANSI_COLOR_GREEN ANSI_BOLD "\t\t\tDone.\n" ANSI_RESET);
		fclose(pcmSampleFile);
		
		free(fileBuffer);
	}
	
	fclose(pcmBankFile);
	printf(ANSI_RESET);
	
	for (size_t i = 0; i < sizeof(samples)/sizeof(samples[0]); i++) {
		free(samples[i]);
		samples[i] = NULL;
	}
}

int getFileSize(char filename[]) {	
	struct stat st;
	stat(filename, &st);
	int size = st.st_size;
	
	return size;
}

uint32_t swap_uint32(uint32_t val) {
    val = ((val << 8) & 0xFF00FF00 ) | ((val >> 8) & 0xFF00FF); 
    return (val << 16) | (val >> 16);
}