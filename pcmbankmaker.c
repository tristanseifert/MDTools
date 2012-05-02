/*
	PCM Bank Generator

	Copyright (c) 2012, Tristan Seifert
	All rights reserved.

	Redistribution and use in source and binary forms, with or without modification, are permitted provided that the following conditions are met:

	Redistributions of source code must retain the above copyright notice, this list of conditions and the following disclaimer.
	Redistributions in binary form must reproduce the above copyright notice, this list of conditions and the following disclaimer in the documentation and/or other materials provided with the distribution.
	THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS "AS IS" AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE ARE DISCLAIMED. IN NO EVENT SHALL THE COPYRIGHT HOLDER OR CONTRIBUTORS BE LIABLE FOR ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES; LOSS OF USE, DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND ON ANY THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE OF THIS SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.
*/

#import "stdio.h"
#import "stdint.h"
#import "stdlib.h"
#import "math.h"
#import <sys/types.h>
#import <sys/stat.h>
#import <unistd.h>
#import <dirent.h>
#import <string.h>

#import "pcmbankmaker.h"

int main(void) {
	char *sampleDir = "samples";
	DIR *dp;
    struct dirent *ep;
    struct dirent *samples[1024];
    int sampleIndex = 0;
    int numberOfSamples = 0;
	char *fileBuffer;
   	char filename[1025+256];
	uint32_t sampleHeader[1025];
	uint32_t sampleHeadOffset;

	printf(ANSI_COLOR_GREEN ANSI_BOLD "Tristan Seifert's Magical PCM Bank Generator™\n\n" ANSI_RESET);
	
	//printf(ANSI_BOLD "Which directory contains the samples you wish to add to the bank? " ANSI_RESET);
	//scanf("%s", &sampleDir);
	printf(ANSI_RESET "\n" ANSI_BOLD "Searching directory '%s' for samples...\n" ANSI_RESET, sampleDir);

    dp = opendir(sampleDir);
    
	if (dp != NULL) {
		while ((ep = readdir(dp))) {
			printf(ANSI_BOLD "Found sample: " ANSI_RESET);
			
			if(strcmp(".", ep->d_name) == 0 || strcmp(ep->d_name, "..") == 0) {
				printf("Skipping '..' or '.'\n");
			} else {
        		puts(ep->d_name);
        		samples[sampleIndex] = ep;
        		sampleIndex++;
        		numberOfSamples++;
        	}
		}
        
        closedir (dp);
	} else {
		perror(ANSI_COLOR_RED "Couldn't open the samples directory");
	}
	
	printf(ANSI_BOLD "\n\nProcessing all samples into file... (will be output as PCM_____.BNK)\n" ANSI_RESET);
	
	FILE *pcmBankFile = fopen("PCM_____.BNK", "w+b");
	FILE *sampleFile;
	
	if(pcmBankFile == NULL) {
		printf(ANSI_COLOR_RED ANSI_BOLD "Could not create file handle for bank. Exiting..." ANSI_RESET);
		return 255;
	}
		
	sampleHeadOffset = numberOfSamples * 4 + 4;
		
	for(int i = 0; i < sampleIndex; i++) {	
   		char filename[1025+256];
		sprintf(filename, "./%s/%s", sampleDir, samples[i]->d_name);
		printf(ANSI_BOLD "\tProcessing %s... " ANSI_RESET, filename);

		sampleHeader[i] = swap_uint32(sampleHeadOffset);
		printf("Placed sample at $%x.\n", swap_uint32(sampleHeader[i]));
		
		sampleHeadOffset += getFileSize(filename);
	}
	
	printf(ANSI_BOLD "\n\tWriting Header to file...\n\n" ANSI_RESET);
	
	fwrite(&sampleHeader, sizeof(int), numberOfSamples, pcmBankFile);
	
	printf(ANSI_BOLD "\tWriting samples to file...\n" ANSI_RESET);
		
	for(int j = 0; j < sampleIndex; j++) {	
		sprintf(filename, "./%s/%s", sampleDir, samples[j]->d_name);
		
		printf("\t\tWriting sample '%s'...\n", filename);
		
		int fileSize = getFileSize(filename);
		fileBuffer = (char *) malloc(sizeof(char)  * fileSize);
		
		FILE *pcmSampleFile = fopen(filename, "r");	
		
		if(pcmSampleFile != NULL) {
			printf("\t\t\tOpened File...\n");
		
			printf("\t\t\tReading File...\n");
		
			fread(fileBuffer, sizeof(char), fileSize-2, pcmSampleFile);
		} else {
			printf(ANSI_COLOR_RED "\nCould not read file %s.\n" ANSI_RESET, filename);
		}
		
		fclose(pcmSampleFile);
		
		fwrite(&fileBuffer, 1, fileSize, pcmBankFile);
	}
	
	fclose(pcmBankFile);
	printf(ANSI_RESET);
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