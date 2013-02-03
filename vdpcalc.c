#include "vdpcalc.h"
#include "stdio.h"
#include "stdint.h"
#include "stdlib.h"

int main(void) {
	uint16_t input;
	uint32_t baseReadVal;
	char in[0x20];
	
	// Get the VRAM address
	printf("Enter VRAM Address: $");
	scanf("%s", &in);
	input = (uint16_t) strtol(in, NULL, 16);
	
	baseReadVal = 0x00000000;
	baseReadVal |= (input & 0xC000) >> 14;
	baseReadVal |= ((input & 0xFF) << 16);
	baseReadVal |= ((input & 0x3F00) << 16);
	
	printf("\nVRAM Write, no DMA:\t$%0*X\n", 8, baseReadVal | VDP_ADDR_WRITE);
	printf("VRAM Write, Fill DMA:\t$%0*X\n", 8, baseReadVal | (VDP_ADDR_WRITE | VDP_ADDR_WRITE_DMA));
	printf("VRAM Write, Copy DMA:\t$%0*X\n", 8, baseReadVal | (VDP_ADDR_WRITE | VDP_ADDR_WRITE_COPY));
	printf("VRAM Read:\t\t$%0*X\n\n", 8, baseReadVal);
	
	printf("VSCR Write, no DMA:\t$%0*X\n", 8, baseReadVal | VDP_ADDR_WRITE_VSCR);
	printf("VSCR Write, DMA:\t$%0*X\n", 8, baseReadVal | (VDP_ADDR_WRITE_VSCR | VDP_ADDR_WRITE_DMA));
	printf("VSCR Read:\t\t$%0*X\n\n", 8, baseReadVal | VDP_ADDR_READ_VSCR);
	
	printf("CRAM Write, no DMA:\t$%0*X\n", 8, baseReadVal | VDP_ADDR_WRITE_CRAM);
	printf("CRAM Write, DMA:\t$%0*X\n", 8, baseReadVal | (VDP_ADDR_WRITE_CRAM | VDP_ADDR_WRITE_DMA));
	printf("CRAM Read:\t\t$%0*X\n", 8, baseReadVal | VDP_ADDR_READ_CRAM);
}