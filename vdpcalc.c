#include "vdpcalc.h"

int main(void) {
	uint16_t input;
	uint32_t baseReadVal;
	char in[0x20];
	
	// Get the VRAM address
	printf("Enter VRAM Address: $");
	scanf("%s", &in);
	input = (uint16_t) strtol(in, NULL, 16);
	printf("\nInputted value: $%X", input);
	
	baseReadVal = 0x00000000;
	baseReadVal |= (input & 0xC000) >> 14;
	baseReadVal |= ((input & 0xFF) << 16);
	baseReadVal |= ((input & 0x3F00) << 16);
	
	printf("\nBase read value: $%X", baseReadVal);
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