#include <stdint.h>

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

// Various structs we can use to store MOD file info

typedef struct __attribute__((__packed__)) mod_sample {
	char title[23]; // MOD file's title is 22, but we need the 0x00 at end
	uint32_t sample_length; // divided by 2 in file
	uint8_t sample_finetune;
	uint8_t sample_volume;
	uint16_t sample_loopstart; // Divided by 2 in file
	uint16_t sample_looplen; // Divided by 2 in file
  
	uint8_t *sampleData;
} mod_sample;

typedef struct __attribute__((__packed__)) mod_note {
	uint8_t instrument;
	uint16_t period;
	uint8_t fxCommand;
	uint8_t fxData;
} mod_note;

typedef struct __attribute__((__packed__)) mod_pattern {
	mod_note *noteData[8][64];
} mod_pattern;

typedef struct __attribute__((__packed__)) modfile_header {
	char title[21]; // MOD file's title is 20, but we need the 0x00 at end
	mod_sample *samples[31];
	uint8_t num_patterns;
	uint8_t highestPattern;
	uint8_t numChannels;
	uint32_t sampleOffset;
	unsigned char patternSequences[128];
	char identification[5];
	uint8_t num_instruments; 
	
	uint32_t sampleBankSize;
	
	mod_pattern *patterns[128];
} modfile_header;

// Function declarations
extern inline uint16_t word_swap(uint16_t in);
extern inline uint32_t longword_swap(uint32_t val);

extern inline void pointer_Write32To8(uint32_t val, uint8_t *ptr);

extern inline uint8_t read_byte(FILE *fp);
extern inline uint16_t read_word(FILE *fp);
extern inline uint32_t read_long(FILE *fp);

void populateModInfoStruct(FILE* fp, modfile_header *header);
void readSampleData(FILE* fp, modfile_header *header);
uint8_t* createPCMSampleBank(modfile_header *header);
void readPatternData(FILE *fp, modfile_header *header);