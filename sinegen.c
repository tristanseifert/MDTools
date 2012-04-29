/*
	Sine Wave Generator

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

#import "sinegen.h"

int main(void) {
	unsigned int numOfPts = 0x100;
	unsigned int amplitude = 0x7F;
	unsigned int Pitch = 0x00000000; // Distance of waves together (format is MMMMDDDD, where D is the decimal place)
	unsigned short outsize = 8;
	unsigned int Counter;
	unsigned int SinePos;

	printf("Tristan Seifert's Magical Sine Wave Generator of Death:\n\n\n");
	printf("Enter Number of Points: ");
	scanf("%i", &numOfPts);
	printf("Enter Amplitude (highest point): ");
	scanf("%i", &amplitude);
	printf("Enter Pitch (usually 0): ");
	scanf("%i", &Pitch);
	printf("Enter Output Size (8 or 16 bit): ");
	scanf("%i", &outsize);
	
	if(outsize != 8 && outsize != 16) {
		  fprintf(stderr, ANSI_COLOR_RED "ERROR: Output size must be either 8 or 16 (bits)\n" ANSI_COLOR_RESET);
		  exit(255);
	}
	
	if(outsize == 8 && amplitude > 256) {
		printf(ANSI_COLOR_YELLOW "WARNING: Amplitudes over 256 can not be represented in 8 bit output mode and will result in clipping." ANSI_COLOR_RESET);
	} else if(outsize == 16 && amplitude > 65535) {
		printf(ANSI_COLOR_YELLOW "WARNING: Amplitudes over 65535 can not be represented in 16 bit output mode and will result in clipping." ANSI_COLOR_RESET);
	}
	
	printf("\nGenerating %i samples with maximum amplitude of %i and outputting as %i bits.\n", numOfPts, amplitude, outsize);
	
	FILE *fp;
	fp = fopen ("sine.bin", "wb");

	if(outsize == 16) {
		uint16_t theValue = 0;
	
		for (Counter = 0x00, SinePos = 0x00; Counter < numOfPts; Counter += 2, SinePos += (0x00010000 + Pitch)) {
			theValue = amplitude * sin(((SinePos >> 0x10) & 0xFFFF) * 0x02 * 3.14159265358979323846 / numOfPts);
		
			uint8_t temp[2] = { theValue >> 8, theValue };
		
			fwrite(&temp, 1, 2, fp);
		}
	} else if(outsize == 8) {
		char SineWave[numOfPts];
		
		for (Counter = 0x00, SinePos = 0x00; Counter < numOfPts; Counter++, SinePos += (0x00010000 + Pitch)) {
			SineWave [Counter] = amplitude * sin (((SinePos >> 0x10) & 0xFFFF) * 0x02 * 3.14159265358979323846 / numOfPts);
		}
		
		fwrite (&SineWave, 0x01, numOfPts, fp);
	}
	
	//for(int i = 0; i < reps; i++) {
	//	fwrite (&SineWave, 0x02, numOfPts, fp);
	//}
	fclose (fp);
}