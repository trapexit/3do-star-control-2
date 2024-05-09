/*
		DUCKCONVERT
	
	duckconvert.c11		--		use PADFILE to add frames (last frame
								is duplicated)
	duckconvert.c10		--		use PADFRAMES to create min framesize;
								player gets funky if frame<9k or so
	duckconvert.c9		--		workin' stereo (at least sans WHIDEO)
	duckconvert.c8		--		stereo comin' on line
	duckconvert.c7		--		for luck
	duckconvert.c6		--		skipAudio(frames),skipVideo(frames)
	duckconvert.c5		--		frame table generation
	duckconvert.c4		--		pad frames if necessary
	duckconvert.c3		--		whideo?   WHIDEO!
	duckconvert.c2		--		packets/frame is soft
	duckconvert.c1		--		adpcm frame-based + header

	This routine shall, upon its completion, convert a video file in
	.DKM format and an audio file in .SND format into a .DUK file.
	
	.DUK file format is a series of frames with subframes, 2 for now --
	first audio, then video (pronounced Whideo).  No frame header for
	now.  Frame format follows: (all values are stored in MAC format
	unless otherwise specified):
	
	audiobytecnt		u32
	videobytecnt		u32
	audio..
	video..
	
	audio format for now:
	
	magic				u16 = 0xF77F
	numsamples			u16
	firstsample			u16
	firstindex			u16
	..pack'd_samples..
	
	video format for now:
	
	AvlBsh				16 bytes (+4 is numbits, intel format)
	..videodata..		packed by 3d0pack
	
	.DKM files contain sequential video frames with AvlBsh headers
	
	.SND files contain 52 sample packets in the following 32 byte package:
	(note that since it comes from PC, byte order is screwy)
	
	index			u8		initial index for packet
	firstsamp		u8		yes, it's 8 bits; these are high-order 8 of 16-bit
							initial sample.  In future, 16 bit initial sample
							will be included (everything else may change too!).
	samples 2,3		u8		high nybble=sample 2, low = sample 3
	samples 0,1		u8		high nyb=0, low=1
	...
	samples 50,51	u8
	samples 48,49	u8
	pad				u32		this may contain full initial sample soon!

	For first pass, we will use 56 samples / frame =~~ 15 fps (really
	15.16666).  Then we will refine so we can customize samplecnt to
	frames per sec.
*/
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#define STEREO		1
#define AIFF		1
#define VIDEODUK	0
									// if VIDEODUK, ALIGN=PADFRAME=SKIP=0
#define AIFFSAMPLES	1144
#define AUDHEADBYTES (8+(STEREO*2))
#define MAXFRAMES 	0
									// set MAXFRAMES=0 for no limit
#define AUDSKIP 	0
#define VIDSKIP 	0

#define u8 unsigned char
#define u16 unsigned short
#define u32 unsigned long
#define s32 long

#define MAXPIX 100000
#define MAGICNUM 0xf77f
#define SAMPPERPACKET 52
#define MAXAUDBYTES 5000
#define MAXVIDBYTES 32000
									
#define WHIDEO 1
									// != 0 for normal operation

#define PADFRAME (1024*9)
//#define PADFRAME 0
									// if !=0, pad frames to this size

#define PADFILE 50
									// if !=0, add this many frames to file

#define ALIGNFRAME 4
//#define ALIGNFRAME 0
									// if !=0, align frames
long width,height;
Boolean byteswap;
FILE *audinfile,*vidinfile,*outfile,*outfrm;
char *audin;
char *audout;
char *vidin;
char *vector;
u16 index, indexR, maxindex=0, firstsamp;
u32 framecnt,audpackets=0,padframes=0;
unsigned char myhead[16];
unsigned char audhead[16];

//u32 audpacketsperfrm=56;			/* 15 fps */
//u32 audpacketsperfrm=50;			/* 16.96 fps */
//u32 audpacketsperfrm=47;			/* 18.04 fps */
//u32 audpacketsperfrm=56;			/* 15.14 fps */
//u32 audpacketsperfrm=42;			/* 20.19 fps */
//u32 audpacketsperfrm=44;			/* 19.27 fps */

//u32 audpacketsperfrm=21;			/* 20.19 fps, 22.050 */
u32 audpacketsperfrm=22;			/* 19.27 fps, 22.050 */
//u32 audpacketsperfrm=70;			/* don't ask */
//u32 audpacketsperfrm=36;			/* 23.55 fps */
//u32 audpacketsperfrm=28;			/* 30.29 fps */
//u32 audpacketsperfrm=18;			/* 23.55 fps, 22.050 KHZ */
//u32 audpacketsperfrm=27;			/* ~16 fps, 22.050 KHZ */
//u32 audpacketsperfrm=71;			/* ~12 fps, 44khz stereo */
//u32 audpacketsperfrm=1;			/* don't ask */

u32 audinbytes;
u32	audoutbytes;
u32 vidinbytes;
u32 vidinduk;
u32 audinduk;
u32 vidoutbytes;
u32 frmTblCnt=0;

u32 longswp(u32 a) {
	return( ((a&0xFFL)<<24) | ((a&0xFF000000L)>>24) | ((a&0xFF00L)<<8) | ((a&0xFF0000L)>>8));
}
u16 wordswp(u16 a) {
	return(((a & 0xFFL)<<8) | ((a >> 8)& 0xFFL));
}
void QC(void *p) {
	int i,j;
	unsigned char *poin;
	
	poin = (unsigned char*)p;
	for(j=0;j<8;j++) {
		printf("%p: ",poin);
		for(i=0;i<16;i++) {
			printf("%2x ",*poin++);
		}
		printf("\n");
	}
}

Boolean loadaudHeader(FILE *f) {
	u32 cnt;
	
	cnt=fread(audhead,1,2,f);
	if(cnt!=2) {
#if !PADFILE
		return(false);
#else
		return(true);
#endif
	}
#if AIFF
	if(  *((int*)audhead)!=MAGICNUM  ) {
		printf("Oh no!!! incorrect #samples or filetype\n");
		printf("magic == %x\n",*((int*)audhead));
		return(false);
	}
#endif
	index=audhead[0];
	firstsamp=((int)audhead[1])<<8;				/* need full precision! */
	if (index>maxindex) {
		maxindex = index;
	}
	audpackets+=audpacketsperfrm;
	return(true);
}

Boolean loadvidHeader(FILE *f) {
	u32 cnt;
#if VIDEODUK
	cnt=fread(myhead,1,8,f);
	if(cnt!=8) {
		return(false);
	}
	audinduk=(*((u32*)myhead));
	vidinduk=*(((u32*)myhead)+1);
// printf("VIDEODUK: audinduk=%lx, vidinduk=%lx\n",audinduk,vidinduk);
	cnt=fread(vidin,1,audinduk,f);
#endif
	cnt=fread(myhead,1,16,f);
	if(cnt!=16) {
		if(padframes++<=PADFILE) {
			return(true);
		} else {
			return(false);
		}
	}
	return(true);
}

Boolean loadaudFrame(FILE *f) {
	u32 cnt;
	cnt=fread(audin,1,audinbytes,f);	
#if !PADFILE
	if(cnt!=audinbytes) return(false);
#endif
// printf("unprocessed: \n");
// QC(audin);
	indexR=audin[30];
	return(true);
}

Boolean skipAudio(long skip) {						/* skip frames */
	long i;
	for (i=0; i<skip; i++) {
		if( !loadaudHeader(audinfile)) {
			printf("Out of audio data\n");
			return(false);
		}
#if STEREO
	#if AIFF
  		audinbytes = AIFFSAMPLES+8;
	#else
  		audinbytes = (2*( ( ( (SAMPPERPACKET/2) +6) * audpacketsperfrm) ))-2;
	#endif
#else
		audinbytes = ( ( ( (SAMPPERPACKET/2) +6) * audpacketsperfrm) )-2;
#endif
		if( !loadaudFrame(audinfile)) {
			printf("Out of audio data\n");
			return(false);
		}
	}
	printf("%ld audio frames skipped\n",skip);
	return(true);
}

Boolean loadvidFrame(FILE *f) {
	u32 cnt;
	cnt=fread(vidin,1,vidinbytes,f);	
	if(cnt!=vidinbytes)  {
		if(padframes<=PADFILE) {
			return(true);
		} else {
			return(false);
		}
	}
// QC(vidin);
	return(true);
}

Boolean skipVideo(long skip) {						/* skip frames */
	long i;
	for (i=0; i<skip; i++) {
		if(! loadvidHeader(vidinfile)){
			printf("Out of video data\n");
			return(false);
		}
		vidinbytes = longswp(*((long*)(myhead+4))) >>3;
		vidoutbytes=vidinbytes+16;
		if( !loadvidFrame(vidinfile)) {
			printf("Out of whideo data\n");
			return(false);
		}
	}
	printf("%ld video frames skipped\n",skip);
	return(true);
}

Boolean saveaudFrame(FILE *f) {
	long cnt;
		
	cnt=fwrite(audout,1,audoutbytes,f);
	if(cnt!=audoutbytes) return(false);
	frmTblCnt+=cnt;
	return(true);
}

Boolean savevidFrame(FILE *f) {
	long cnt;

	cnt=fwrite(myhead,1,16,f);				// for now, dx wants this
	if(cnt!=16) return(false);
	frmTblCnt+=cnt;
	
	cnt=fwrite(vidin,1,vidoutbytes,f);
	if(cnt!=vidoutbytes) return(false);
//	printf("%ld video bytes\n",cnt);
	frmTblCnt+=cnt;
	return(true);
}

Boolean saveCounts(FILE *f) {
	long cnt;

	cnt=fwrite(&audoutbytes,1,4,f);
	if(cnt!=4) return(false);
	frmTblCnt+=cnt;
	cnt=fwrite(&vidoutbytes,1,4,f);
	if(cnt!=4) return(false);
	frmTblCnt+=cnt;
	return(true);
}

Boolean saveFrmTbl(FILE *f) {
	long cnt;

	cnt=fwrite(&frmTblCnt,1,4,f);
	if(cnt!=4) return(false);
// printf("Frame Table offset = %ld\n",frmTblCnt);
	return(true);
}

Boolean savePad(FILE *f) {
	long cnt,pad;
	
									/* 8 for counts, 16 for AvlBsh: */
	pad = PADFRAME-(AUDHEADBYTES+audoutbytes+16+vidoutbytes);
	if(pad<=0) {
//		printf("Cannot pad file\n");
		return(true);				/* allow this to happen */
	}
	printf("padding frame %ld\n",framecnt);
	cnt=fwrite(&vidoutbytes,1,pad,f);
	if(cnt!=pad) return(false);
	frmTblCnt+=cnt;
	return(true);
}

Boolean saveAlign(FILE *f) {
	long cnt,pad,align;
	
	align = (frmTblCnt+ALIGNFRAME-1) & (0-ALIGNFRAME);
	pad = align-frmTblCnt;
// printf("aligning file with %ld extra bytes \n",pad);

	cnt=fwrite(&vidoutbytes,1,pad,f);
	if(cnt!=pad) return(false);
	frmTblCnt+=cnt;
	return(true);
}

int FixAudFrm() {					/* make header, add packets while
										swapping bytes	*/
	unsigned int i,j;
	unsigned char *src,*dest;

	src=(unsigned char *)audin;
	dest=(unsigned char *)audout;
	
	if(AIFF) {
		*((int*)dest)=MAGICNUM;
		dest++; dest++;
		for(i=0;i<AIFFSAMPLES+8;i++) {
			*dest++=*src++;
		}
		return;
	}
 
 /* else old DVI type file: */
 	
	((int *)dest)[0] = MAGICNUM;
	((int *)dest)[1] = (SAMPPERPACKET*audpacketsperfrm);
	((int *)dest)[2] = firstsamp;
	((int *)dest)[3] = index;
#if STEREO
	((int *)dest)[4] = indexR;
#endif
	dest+=AUDHEADBYTES;
	
	for(j=0;j<audpacketsperfrm;j++) {
		for(i=0;i<(SAMPPERPACKET/4);i++) {
#if STEREO
			*dest++=(src[1] & 0xF0) | (src[33] >> 4);
			*dest++=(src[1] << 4) | (src[33] & 0x0F);
			*dest++=(src[0] & 0xF0) | (src[32] >> 4);
			*dest++=(src[0] << 4) | (src[32] & 0x0F);

//			*dest++=src[1] >> 4;	/* one nybble per byte for now */
//			*dest++=src[1] & 0x0F;
//			*dest++=src[0] >> 4;
//			*dest++=src[0] & 0x0F;
#else
			*dest++=src[1];			/* plain old byteswap */
			*dest++=src[0];
#endif
			src++;
			src++;
			if(*((u16 *)(dest-2))==MAGICNUM) printf("MAGIC nuber in data!\n");
			if(*((u16 *)(dest-4))==MAGICNUM) printf("MAGIC nuber in data!\n");
		}
		src+=6;						/* skip pad & next header */
#if STEREO
		src+=(SAMPPERPACKET/2)+6;
#endif
	}
// printf("processed frame:\n");
// QC(audout);
}

Boolean processFrame() {
	if(! loadaudHeader(audinfile)){
		printf("loadaudheader fails\n");
		return(false);
	}

	if(MAXFRAMES) if(MAXFRAMES<=framecnt) return(false);

#if STEREO
	#if AIFF
  		audinbytes = AIFFSAMPLES+8;
  		audoutbytes = AIFFSAMPLES+10;
	#else
		audinbytes = (2*( ( ( (SAMPPERPACKET/2) +6) * audpacketsperfrm) ))-2;
		audoutbytes = ( ((SAMPPERPACKET) * audpacketsperfrm)+AUDHEADBYTES );
	#endif
#else
	audinbytes = ( ( ( (SAMPPERPACKET/2) +6) * audpacketsperfrm) )-2;
	audoutbytes = ( ((SAMPPERPACKET/2) * audpacketsperfrm)+AUDHEADBYTES );
#endif
	if(WHIDEO) {
		if(! loadvidHeader(vidinfile)){
			return(false);
		}
#if VIDEODUK
		vidinbytes = (((vidinduk+audinduk)+3)&0xFFFFFFFC)-audinduk;
		/* early versions of duckconvert did not align this numner */
#else
		vidinbytes = longswp(*((long*)(myhead+4))) >>3;
#endif
//		printf("vidinbytes = %lx \n",vidinbytes);
		if(vidinbytes>MAXVIDBYTES) {
			printf("Danger! Too many vidinbytes\n");
			while(!Button());
			ExitToShell();
		}
#if VIDEODUK
		vidoutbytes=vidinbytes;
#else
		vidoutbytes=vidinbytes+16;
#endif
	}

	if(! loadaudFrame(audinfile)){
		printf("Audio data finished\n");
		return(false);
	}

	if(WHIDEO) {
		if(! loadvidFrame(vidinfile)){
			return(false);
		}
	}

	if(! saveFrmTbl(outfrm)) {
		return(false);
	}
	
    FixAudFrm();
    
    if(WHIDEO) {
    	if(! saveCounts(outfile)) {
    		return(false);
    	}
   	}
    
	if(! saveaudFrame(outfile)){
		printf("Unable to save audio frame\n");
		while(!Button());
		ExitToShell();
	}

    if(WHIDEO) {
		if(! savevidFrame(outfile)){
			printf("Unable to save whideo frame\n");
			while(!Button());
			ExitToShell();
		}
	}
	
	if(PADFRAME) {
		if(! savePad(outfile)){
			printf("Unable to save pad bytes\n");
			while(!Button());
			ExitToShell();
		}
	}
	
	if(ALIGNFRAME) {
		if(! saveAlign(outfile)){
			printf("Unable to save alignment bytes\n");
			while(!Button());
			ExitToShell();
		}
	}
	
	framecnt++;
	return(true);
}

void main() {
	char fname[64],infname[64],outfname[64];

	printf("adpcm packer\n");
	printf("===============\n");
	printf("© 1993 The Duck Corporation\nAll rights reserved\n\n");
	printf("This program merges .SND + .DKM ==> .DUK files.\n");
	printf("Please enter the filename without the extension.\n");
	scanf("%s",fname);
printf("processing %s\n",fname);

	audin = malloc(MAXAUDBYTES);
	audout = malloc(MAXAUDBYTES);
	vidin = malloc(MAXVIDBYTES);
	if((audin == NULL) || (audout == NULL)) {
		printf("Unable to malloc\n");
		while(!Button());
		ExitToShell();
	}
	strcpy(infname,fname);
#if AIFF
	strcat(infname,".dpm");
#else
	strcat(infname,".snd");
#endif
	audinfile = fopen(infname,"rb");
	if(audinfile == NULL) {
		printf("Unable to open audio .SND file\n");
		while(!Button());
		ExitToShell();
	}

	skipAudio(AUDSKIP);

	if(WHIDEO) {	
		strcpy(infname,fname);
#if VIDEODUK
		strcat(infname,".DUK");
#else
		strcat(infname,".DKM");
#endif
		printf("opening %s for input\n",infname);
		vidinfile = fopen(infname,"rb");
		if(vidinfile == NULL) {
			printf("Unable to open video .DKM file\n");
			while(!Button());
			ExitToShell();
		}
		skipVideo(VIDSKIP);
	}
	
	strcpy(outfname,fname);
#if VIDEODUK
	strcat(outfname,".dk2");
#else
	strcat(outfname,".duk");
#endif
	outfile = fopen(outfname,"wb");
	printf("opening %s for output\n",outfname);
	if(outfile == NULL) {
		printf("Unable to open output file\n");
		while(!Button());
		ExitToShell();
	}
	
	strcpy(outfname,fname);
	strcat(outfname,".frm");
	outfrm = fopen(outfname,"wb");
	if(outfrm == NULL) {
		printf("Unable to open frame table file\n");
		while(!Button());
		ExitToShell();
	}
	
	framecnt = 0;
	while(processFrame()) {
		if(!(framecnt&0x1f)) printf("finished frame #%ld\n",framecnt);
	}
	fclose(audinfile);
	fclose(outfile);
	printf("finished processing %s\n",outfname);
	printf("FINISHED! confused %ld frames, %ld audio packets \n",
			framecnt,audpackets);
	printf("maxindex = %d \n",maxindex);
}
