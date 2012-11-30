#include "vdpcalc.h"

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
	
	printf("\nVRAM Write, no DMA:\t$%X\n", baseReadVal | VDP_ADDR_WRITE);
	printf("VRAM Write, Fill DMA:\t$%X\n", baseReadVal | (VDP_ADDR_WRITE | VDP_ADDR_WRITE_DMA));
	printf("VRAM Write, Copy DMA:\t$%X\n", baseReadVal | (VDP_ADDR_WRITE | VDP_ADDR_WRITE_COPY));
	printf("VRAM Read:\t\t$%X\n\n", baseReadVal);
	
	printf("VSCR Write, no DMA:\t$%X\n", baseReadVal | VDP_ADDR_WRITE_VSCR);
	printf("VSCR Write, DMA:\t$%X\n", baseReadVal | (VDP_ADDR_WRITE_VSCR | VDP_ADDR_WRITE_DMA));
	printf("VSCR Read:\t\t$%X\n\n", baseReadVal | VDP_ADDR_READ_VSCR);
	
	printf("CRAM Write, no DMA:\t$%X\n", baseReadVal | VDP_ADDR_WRITE_CRAM);
	printf("CRAM Write, DMA:\t$%X\n", baseReadVal | (VDP_ADDR_WRITE_CRAM | VDP_ADDR_WRITE_DMA));
	printf("CRAM Read:\t\t$%X\n", baseReadVal | VDP_ADDR_READ_CRAM);
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