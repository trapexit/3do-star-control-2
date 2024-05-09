#define u16 unsigned short int
#define u32 unsigned long int

/* the following header contains little-endian data!!! */

typedef struct DuckFrameHeader{
	u16	AlgNum;
	u16 Flags;
	u32 NumBits;
	u32 AlgSpec;
	u16	Height;
	u16 Width;
} DuckFrameHeader;

void initDX(s16 *protoy, s16 *protoc,char *filename);
void duckDX(void *bits, void *screen,u32 wh, int pitch);

extern u32 forceheight;
