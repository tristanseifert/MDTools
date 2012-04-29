/*
	Bitmap to Sega CD ASIC Data converter
	
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

#import "bmp2scdasic.h"

typedef struct bmpfile_header {
  uint32_t filesz;
  uint16_t creator1;
  uint16_t creator2;
  uint32_t bmp_offset;
} bmpfile_header;

typedef struct bitmap_info {
  uint32_t header_sz;
  int32_t width;
  int32_t height;
  uint16_t nplanes;
  uint16_t bitspp;
  uint32_t compress_type;
  uint32_t bmp_bytesz;
  int32_t hres;
  int32_t vres;
  uint32_t ncolors;
  uint32_t nimpcolors;
} bitmap_info;

typedef struct stampMapEntry {
	unsigned int stamp_idx:11;
	unsigned int undefined:2;
	unsigned int rotation:2;
	unsigned int hflip:1;
} stampMapEntry;

typedef struct md_tile {
	uint8_t tileData[32];
} md_tile;

typedef struct asic_tile {
	md_tile tiles[4];
} asic_tile;

int main(void) {	
	FILE *palPtr, *bitmapPtr, *a1, *a0;
	int d0, d1, d2, d3;
	
	printf (ANSI_BOLD "Status: Palette Data\n" ANSI_BOLDOFF);
	
	printf ("\t" ANSI_UNDERLINE ">> Opening Palette File\n" ANSI_UNDERLINEOFF);
	if ((palPtr = fopen ("Palette.bmp", "rb")) == NULL) {
		printf ("\t\t" ANSI_COLOR_RED ">> Error: Could not open 'Palette.bmp'\n");
		printf (ANSI_COLOR_MAGENTA  "\nPress enter key to exit...\n");
		exitProgrammeWithCode(254);
	}
	
	printf (ANSI_COLOR_BLUE "\t\t>> Loading & Dumping Palette Data\n" ANSI_RESET);
	if ((a1 = fopen ("pal.bin", "wb")) == NULL) {
		fclose (palPtr);
		printf (ANSI_COLOR_RED "\t\t>> Error: Could not open 'pal.bin'\n");
		printf (ANSI_COLOR_MAGENTA "\nPress enter key to exit...\n");
		exitProgrammeWithCode(253);
	}
	
	fseek (palPtr, 0x36, SEEK_SET);
	int PalRam[0x30], PalPoint = 0x00;
	d3 = 0x10;
	while (d3 != 0x00) {
		d0 = fgetc (palPtr);
		PalRam[PalPoint] = d0;
		PalPoint += 0x01;
		d0 >>= 0x04;
		d0 &= 0x0E;
		d1 = fgetc (palPtr);
		PalRam[PalPoint] = d1;
		PalPoint += 0x01;
		d1 &= 0xE0;
		d2 = fgetc (palPtr);
		PalRam[PalPoint] = d2;
		PalPoint += 0x01;
		d2 >>= 0x04;
		d2 &= 0x0E;
		d1 += d2;
		fputc (d0, a1);
		fputc (d1, a1);
		fseek (palPtr, 0x15, SEEK_CUR);
		d3 -= 0x01;
	}
	
	fclose (palPtr);
	fclose (a1);
	printf (ANSI_COLOR_GREEN ANSI_BOLD "\t>> Complete\n" ANSI_RESET);
	
	
	printf (ANSI_BOLD "\n Status: Art Data\n" ANSI_BOLDOFF);
	
	printf ("\t" ANSI_UNDERLINE ">> Opening Bitmap File\n" ANSI_UNDERLINEOFF);
	if ((a0 = fopen ("Bitmap.bmp", "rb")) == NULL)
	{
		printf (ANSI_COLOR_RED "\t\t>> Error: Could not open 'Bitmap.bmp'\n");
		printf (ANSI_COLOR_MAGENTA "\nPress enter key to exit...\n");
		exitProgrammeWithCode(252);
		return 0;
	}
	
	printf (ANSI_COLOR_BLUE "\t\t>> Obtaining Bitmap Information\n" ANSI_RESET);
	fseek (a0, 0x12, SEEK_SET);
	int XSIZE = GetPointer (a0);
	int YSIZE = GetPointer (a0);
	printf (ANSI_COLOR_GREEN "\t\t\t~Info:\n\t\t\t\tX Size of bitmap: $%0.8X\n\t\t\t\tY Size of bitmap: $%0.8X\n" ANSI_RESET, XSIZE, YSIZE);

	printf (ANSI_COLOR_BLUE "\t\t>> Padding Bitmap Art\n" ANSI_RESET);
	if ((a1 = fopen ("Pad.bmp", "wb")) == NULL) {
		fclose (a0);
		printf (ANSI_COLOR_RED "\t\t>> Error: Could not open 'Pad.bmp'\n");
		printf (ANSI_COLOR_MAGENTA "\nPress enter key to exit...\n");
		exitProgrammeWithCode(251);
	}
	fseek (a0, 0x00, SEEK_SET);
	d1 = 0x36;
	while (d1 != 0x00) {
		d0 = fgetc (a0);
		fputc (d0, a1);
		d1 -= 0x01;
	}
	fseek (a1, 0x12, SEEK_SET);
	d1 = XSIZE;
	int XORI = XSIZE;
	d1 &= 0x07;
	if (d1 != 0x00) {
		XSIZE += 0x08;
	}
	
	XSIZE &= 0xFFFFFFF0;
	int XPAD = XSIZE;
	XPAD -= XORI;
	SavePointer (XSIZE, a1);

	d1 = YSIZE;
	int YORI = d1;
	d1 &= 0x07;
	if (d1 != 0x00) {
		YSIZE += 0x08;
	}
	YSIZE &= 0xFFFFFFF0;
	int YPAD = YSIZE;
	YPAD -= YORI;
	SavePointer (YSIZE, a1);
	printf (ANSI_COLOR_GREEN "\t\t\t~Info:\n\t\t\t\tX Pixels Padded:  $%0.8X\n\t\t\t\tY Pixels Padded:  $%0.8X\n" ANSI_RESET, XPAD, YPAD);
	fseek (a0, 0x36, SEEK_SET);
	fseek (a1, 0x36, SEEK_SET);
	while (YPAD != 0x00) {
		d0 = XSIZE;
		while (d0 != 0x00) {
			fputc (PalRam[0x00], a1);
			fputc (PalRam[0x01], a1);
			fputc (PalRam[0x02], a1);
			d0 -= 0x01;
		}
		YPAD -= 0x01;
	}
	
	d2 = YORI;
	while (d2 != 0x00) {
		d1 = XORI;
		while (d1 != 0x00) {
			d0 = fgetc (a0);
			fputc (d0, a1);
			d0 = fgetc (a0);
			fputc (d0, a1);
			d0 = fgetc (a0);
			fputc (d0, a1);
			d1 -= 0x01;
		}
	// Padding...
		d0 = ftell (a0);
		d0 -= 0x36;
		d0 &= 0x03;
		if (d0 != 0x00) {
			d1 = 0x04;
			d1 -= d0;
			d0 = d1;
		}
		fseek (a0, d0, SEEK_CUR);
	// ...ughh
		d1 = XPAD;
		while (d1 != 0x00) {
			fputc (PalRam[0x00], a1);
			fputc (PalRam[0x01], a1);
			fputc (PalRam[0x02], a1);
			d1 -= 0x01;
		}
		d2 -= 0x01;
	}
	
	fclose (a0);
	fclose (a1);
	
	if ((a0 = fopen ("Pad.bmp", "rb")) == NULL) {
		fclose (a0);
		printf (ANSI_COLOR_RED "\t\t" ">> Error: Could not open 'Pad.bmp'\n");
		printf (ANSI_COLOR_MAGENTA "\nPress enter key to exit...\n");
		exitProgrammeWithCode(250);
	}
	
	printf (ANSI_COLOR_GREEN ANSI_BOLD "\t>> Complete\n" ANSI_RESET);
	printf ("\t" ANSI_UNDERLINE ">> Converting Art Data\n" ANSI_RESET);
	
	asic_tile tiles[512];
	
	uint32_t currentPixel;
	uint8_t currentPixelMatch;
	
	fseek (palPtr, 0x36, SEEK_SET);
	
	int currentTile = 0;
	uint8_t currentMDTile = 0;
	uint8_t currentMDTilePixel = 0;
	uint8_t theCurrentPixel;
	uint8_t pixel1, pixel2;
	uint8_t currentRow = 0;
	int rowPtr = 0;
	int columnPtr = 0;
	
	while(!feof(a0)) {	
		currentRow = 0;
		pixel2 = 0;
		pixel1 = 0;
		theCurrentPixel = 0;
		currentMDTilePixel = 0;
		currentMDTile = 0;
		
		for(currentRow = 0; currentRow < 8; currentRow++) {
			rowPtr = ftell(a0);
			
			fread(&currentPixel, 3, 0, a0);
			currentPixelMatch = GetColourMatch(a0, PalRam);

			if(currentPixelMatch == 0xFF) {
				d0 = ftell (a0);
				d0 -= 0x03;
						
				fclose (a0);
				printf (ANSI_COLOR_RED "\t\t" ">> Error: Could not find palette entry for pixel at $%0.8X\n", d0);
				printf (ANSI_COLOR_MAGENTA "\nPress enter key to exit...\n");
				exitProgrammeWithCode(249);		
			} else {
				pixel1 = currentPixelMatch;
			}
		
			if(feof(a0)) {
				pixel2 = 0x00;
			} else {
				fread(&currentPixel, 3, 0, a0);
				currentPixelMatch = GetColourMatch(a0, PalRam);
			
				if(currentPixelMatch == 0xFF) {
					d0 = ftell (a0);
					d0 -= 0x03;
						
					fclose (a0);
					printf (ANSI_COLOR_RED "\t\t" ">> Error: Could not find palette entry for pixel at $%0.8X\n", d0);
					printf (ANSI_COLOR_MAGENTA "\nPress enter key to exit...\n");
					exitProgrammeWithCode(249);		
				} else {
					pixel2 = currentPixelMatch;
				}
			}
		
			theCurrentPixel = ((theCurrentPixel & 0xF) | (pixel1 << 4)); // set upper nybble of 'variable' to 'value'
			theCurrentPixel = ((theCurrentPixel & 0xF0) | (pixel2 & 0xF)); // set lower nybble of 'variable' to 'value'
		
			if(currentMDTilePixel >= 2) {
				tiles[currentTile].tiles[currentMDTile].tileData[currentMDTilePixel + (currentRow * 2)] = theCurrentPixel;
				currentMDTilePixel += 1;
			} else {
				tiles[currentTile].tiles[currentMDTile+1].tileData[(currentMDTilePixel - 2)  + (currentRow * 2)] = theCurrentPixel;
				currentMDTilePixel += 1;		
			}
			
			if(currentRow == 0) {
				columnPtr = ftell(a0);
			}
			
			currentRow++;
			rowPtr += XSIZE;
			rowPtr -= (16 * 3);
			fseek(a0, rowPtr, SEEK_SET);
			
			if(currentRow == 9) {
				currentMDTile = 2;
				currentMDTilePixel = 0;
				
			}
		}
	}
	
}

void exitProgrammeWithCode(int code) {
	printf(ANSI_COLOR_RESET);
	printf(ANSI_BGCOLOR_RESET);
	printf(ANSI_RESET);

	if(code != 0) {
		getc (stdin);
	}
	
	exit(code);
}

int GetPointer (FILE *a0) {
	int d0, d1, d2, d3;
	d3 = fgetc (a0);
	d2 = fgetc (a0);
	d1 = fgetc (a0);
	d0 = fgetc (a0);
	d0 <<= 0x08;
	d0 += d1;
	d0 <<= 0x08;
	d0 += d2;
	d0 <<= 0x08;
	d0 += d3;
	return (d0);
}

void SavePointer (int d0, FILE *a1) {
	fputc (d0, a1);
	d0 >>= 0x08;
	fputc (d0, a1);
	d0 >>= 0x08;
	fputc (d0, a1);
	d0 >>= 0x08;
	fputc (d0, a1);
}

int GetColourMatch (FILE *a0, int PalRam[]) {
	int d0, d1, d2, d3, d4, PalPoint;
	d0 = fgetc (a0);
	d1 = fgetc (a0);
	d2 = fgetc (a0);
	d4 = 0x00;
	PalPoint = 0x00;
	while (PalPoint != 0x30&&d4 != 0x03) {
		d4 = 0x00;
		d3 = PalRam[PalPoint];
		PalPoint += 0x01;
		if (d3 == d0) {
			d4 += 0x01;
		}
		d3 = PalRam[PalPoint];
		PalPoint += 0x01;
		if (d3 == d1) {
			d4 += 0x01;
		}
		d3 = PalRam[PalPoint];
		PalPoint += 0x01;
		if (d3 == d2) {
			d4 += 0x01;
		}
	}
	PalPoint /= 0x03;
	d0 = PalPoint;
	d0 -= 0x01;
	if (d4 != 0x03) {
		d0 = 0xFF;
	}
	return (d0);
}