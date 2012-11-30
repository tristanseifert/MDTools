#include "stdio.h"
#include "stdint.h"
#include "stdlib.h"
#include "math.h"
#include <sys/types.h>
#include <sys/stat.h>
#include <unistd.h>
#include <dirent.h>
#include <string.h>

#define VDP_ADDR_WRITE 0x40000000
#define VDP_ADDR_WRITE_DMA 0x80
#define VDP_ADDR_WRITE_COPY 0xC0
#define VDP_ADDR_READ_VSCR 0x10
#define VDP_ADDR_WRITE_VSCR 0x40000010
#define VDP_ADDR_READ_CRAM 0x20
#define VDP_ADDR_WRITE_CRAM 0xC0000000

#define HI_NIBBLE(b) (((b) >> 4) & 0x0F)
#define LO_NIBBLE(b) ((b) & 0x0F)

#define ANSI_COLOR_RED     "\x1b[31m"
#define ANSI_COLOR_GREEN   "\x1b[32m"
#define ANSI_COLOR_YELLOW  "\x1b[33m"
#define ANSI_COLOR_BLUE    "\x1b[34m"
#define ANSI_COLOR_MAGENTA "\x1b[35m"
#define ANSI_COLOR_CYAN    "\x1b[36m"
#define ANSI_COLOR_RESET   "\x1b[0m"

#define ANSI_BGCOLOR_WHITE   "\x1b[47m"
#define ANSI_BGCOLOR_RED     "\x1b[41m"
#define ANSI_BGCOLOR_GREEN   "\x1b[42m"
#define ANSI_BGCOLOR_YELLOW  "\x1b[43m"
#define ANSI_BGCOLOR_BLUE    "\x1b[44m"
#define ANSI_BGCOLOR_MAGENTA "\x1b[45m"
#define ANSI_BGCOLOR_CYAN    "\x1b[46m"
#define ANSI_BGCOLOR_RESET   "\x1b[49m"

#define ANSI_BOLD			 "\x1b[1m"
#define ANSI_ITALIC		     "\x1b[3m"
#define ANSI_UNDERLINE	   	 "\x1b[4m"
#define ANSI_STRIKETHRU		 "\x1b[9m"
#define ANSI_BOLDOFF    	 "\x1b[22m"
#define ANSI_ITALICOFF		 "\x1b[23m"
#define ANSI_UNDERLINEOFF    "\x1b[24m"
#define ANSI_STRIKEOFF    	 "\x1b[29m"
#define ANSI_RESET   		 "\x1b[0m"

int getFileSize(char filename[]);
uint32_t swap_uint32(uint32_t val);