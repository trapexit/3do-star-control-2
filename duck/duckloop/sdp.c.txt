/*
	File:		sdp.c

	Contains:	Simple (Stupid) Duck Player          

	Written by:	Dan Miller and Victor Yurkovsky
	Modified by: David Silver

	Copyright:	© 1993,4 by The Duck Corporation. All rights reserved.
				This material constitutes confidential and proprietary
				information of The Duck Corporation and shall not be used by
				any person or bozo for any purpose except as expressly
				authorized in writing by The Duck Corporation.

*/


#include "DuckDebug.h"
#include "DuckTypes.h"

#include "Portfolio.h"
#include "Init3DO.h"
#include "mem.h"
#include "SDPlayer.h"

#ifdef DEBUG
#define	PRD(s)	printf s
#else
#define PRD(s)
#endif

extern bool setupScreens(player *plr,u32 screencnt);
extern void setupPlayer(player *plr,char *paramname);

static void showframe(s32 frame);
static u32* loadftable(char *name);

extern ScreenContext TheScreen;

static player plr;
static u32 *frametab;
static ubyte *buf;
static u32 length;
static char gfilename[40];
static u32 frametab_size, buf_size;
static ubyte	first_time;

static int32	first_frame, last_frame, curframe;

static u32 longswp(u32 a) {
	return( ((a&0xFFL)<<24) | ((a&0xFF000000L)>>24) | ((a&0xFF00L)<<8) | ((a&0xFF0000L)>>8));
}

enum
{
    IO_FAILURE = -1,
    IO_CLOSE = 1,
    IO_BUF_FREE
};

static Item	io_task, parent_task;
static int32	io_sig, parent_sig;
static int32	io_cmd, io_offs, io_len;

#define BLOCK_SIZE	2048

static char	*bufs[1];
static int32	lastbuf;

static int	buf_flags;

static int32	max_frame_size;

//#define DEBUG
//#define FIRST_BUF

#if defined(FIRST_BUF) || defined(DEBUG)
static int32	load_frame;
#endif

#ifdef FIRST_BUF
static void
io_task_func ()
{
    int32	blocks;
#define NUM_FILE_BUFS	2
#define MIN_BLOCKS	16
    int32	bytes;
    Item	fi, ireq;
    char	*buf, *work_buf[NUM_FILE_BUFS], *wbp;
    IOInfo	info;
    int32	cur_buf, load_buf;
    char	*first_buf;
    
    ChangeDirectory ("$boot");
    blocks = ((max_frame_size + BLOCK_SIZE - 1) / BLOCK_SIZE) * 2;
    if (blocks < MIN_BLOCKS)
	blocks = MIN_BLOCKS;
    bytes = blocks * BLOCK_SIZE;
    fi = ireq = -1;
    buf = 0;
    first_buf = 0;
    fi = OpenDiskFile (gfilename);
    ireq = CreateIOReq (NULL, 0, fi, 0);
    buf = (char *)ThreedoAlloc (bytes * NUM_FILE_BUFS);
    if (fi < 0 || ireq < 0 || !buf)
    {
	io_cmd = IO_FAILURE;
	goto Cleanup;
    }
	
    for (cur_buf = 0; cur_buf < NUM_FILE_BUFS; cur_buf++)
	work_buf[cur_buf] = cur_buf == 0 ? buf : work_buf[cur_buf - 1] + bytes;
    
    memset (&info, 0, sizeof (info));
    info.ioi_Command = CMD_READ;
    info.ioi_Recv.iob_Buffer = work_buf[0];
    info.ioi_Recv.iob_Len = bytes;
    info.ioi_Offset = 0;
    DoIO (ireq, &info);
    info.ioi_Offset += blocks;

    load_buf = 0; 
    cur_buf = NUM_FILE_BUFS - 1;
    
    if (first_buf = (char *)ThreedoAlloc (bytes))
	memcpy (first_buf, work_buf[0], bytes);

    io_sig = AllocSignal (0);
    SendSignal (parent_task, parent_sig);

    for (;;)
    {
	WaitSignal (io_sig);
	if (io_cmd != IO_BUF_FREE)
	    break;

	if (load_frame == first_frame)
	{
	    if (!first_buf)
		WaitIO (ireq);
	    
	    if (++cur_buf == NUM_FILE_BUFS)
		cur_buf = 0;
	    if (first_buf)
		memcpy (work_buf[cur_buf], first_buf, bytes);
	    wbp = work_buf[cur_buf];
	    memcpy (bufs[lastbuf], wbp, io_len);
	    buf_flags |= (1 << lastbuf);
		
	    wbp += io_len;
	    
	    if (++load_buf == NUM_FILE_BUFS)
		load_buf = 0;
	    info.ioi_Recv.iob_Buffer = work_buf[load_buf];
	    if (first_buf)
		WaitIO (ireq);
	    SendIO (ireq, &info);
	    info.ioi_Offset += blocks;
	}
	else
	{
	    int32	leftover;
    
	    leftover = bytes - (wbp - work_buf[cur_buf]);
	    if (io_len < leftover)
	    {
		memcpy (bufs[lastbuf], wbp, io_len);
		buf_flags |= (1 << lastbuf);
		
		wbp += io_len;
	    }
	    else
	    {
		memcpy (bufs[lastbuf], wbp, leftover);
		io_len -= leftover;
		if (io_len >= 0)
		{
		    WaitIO (ireq);

		    if (++cur_buf == NUM_FILE_BUFS)
			cur_buf = 0;
		    wbp = work_buf[cur_buf];
		    if (io_len > 0)
		    {
			memcpy (bufs[lastbuf] + leftover, wbp, io_len);
			wbp += io_len;
		    }
		    buf_flags |= (1 << lastbuf);
			
		    if (++load_buf == NUM_FILE_BUFS)
			load_buf = 0;
//printf ("reading %ld bytes at %ld into buffer %ld (last_frame is at %ld)\n",
//	bytes, info.ioi_Offset * BLOCK_SIZE, load_buf, frametab[last_frame]);
		    info.ioi_Recv.iob_Buffer = work_buf[load_buf];
		    SendIO (ireq, &info);
		    info.ioi_Offset += blocks;
		    
		    if (info.ioi_Offset * BLOCK_SIZE >= frametab[last_frame + 1])
			info.ioi_Offset = 0;
		}
	    }
	}
    }

Cleanup:
    if (first_buf)
	ThreedoFree (first_buf, bytes);
    if (buf)
	ThreedoFree (buf, bytes * NUM_FILE_BUFS);
    if (ireq >= 0)
	DeleteIOReq (ireq);
    if (fi >= 0)
	CloseDiskFile (fi);
    FreeSignal (io_sig);
    SendSignal (parent_task, parent_sig);
    WaitSignal (0); // wait forever
}
#else
static void
io_task_func ()
{
#define NUM_FILE_BUFS	2
#define MIN_BLOCKS	16
    int32	blocks;
    int32	bytes;
    Item	fi, ireq;
    char	*buf, *load_buf[NUM_FILE_BUFS], *wbp, *work_buf;
    IOInfo	info;
    int32	cur_idx, load_idx;
    int32	last_offs, last_block, last_bytes, last_alloc, alloc_tries;
    char	*last_buf;
    
    ChangeDirectory ("$boot");
    blocks = ((max_frame_size + BLOCK_SIZE - 1) / BLOCK_SIZE) * 2;
    if (blocks < MIN_BLOCKS)
	blocks = MIN_BLOCKS;
    bytes = blocks * BLOCK_SIZE;
    fi = ireq = -1;
    buf = 0;
    last_buf = 0;
    fi = OpenDiskFile (gfilename);
    ireq = CreateIOReq (NULL, 0, fi, 0);
    buf = (char *)ThreedoAlloc (bytes * NUM_FILE_BUFS);
    if (fi < 0 || ireq < 0 || !buf)
    {
	io_cmd = IO_FAILURE;
	goto Cleanup;
    }
	
    for (cur_idx = 0; cur_idx < NUM_FILE_BUFS; cur_idx++)
	load_buf[cur_idx] = cur_idx == 0 ? buf : load_buf[cur_idx - 1] + bytes;
    
    memset (&info, 0, sizeof (info));
    info.ioi_Command = CMD_READ;
    info.ioi_Recv.iob_Buffer = load_buf[0];
    info.ioi_Recv.iob_Len = bytes;
    info.ioi_Offset = 0;
    DoIO (ireq, &info);
    info.ioi_Offset += blocks;
    
    last_offs = frametab[last_frame + 1];
    last_block = (last_offs / (blocks * BLOCK_SIZE)) * blocks;
    last_bytes = last_offs - (last_block * BLOCK_SIZE);
    alloc_tries = 1;
    if (last_bytes < ((blocks * BLOCK_SIZE) >> 2))
    {
#ifdef DEBUG
printf ("pumping up to %ld bytes (from %ld)\n", last_bytes + (blocks * BLOCK_SIZE), last_bytes);
#endif
	last_bytes += (blocks * BLOCK_SIZE);
	last_block -= blocks;
	++alloc_tries;
    }
    
    do
    {
	last_alloc = ((last_bytes + BLOCK_SIZE - 1) / BLOCK_SIZE) * BLOCK_SIZE;
	if (last_buf = (char *)ThreedoAlloc (last_alloc))
	{
	    IOInfo	linfo;
	
	    linfo = info;
	    linfo.ioi_Recv.iob_Buffer = last_buf;
	    linfo.ioi_Recv.iob_Len = last_alloc;
	    linfo.ioi_Offset = last_block;
	    DoIO (ireq, &linfo);
	    break;
	}
	
	last_bytes -= (blocks * BLOCK_SIZE);
	last_block += blocks;
    } while (--alloc_tries);
    
    last_offs -= last_bytes;
    
    if (!last_buf)
	last_block = ((frametab[last_frame + 1] / (blocks * BLOCK_SIZE)) + 1) * blocks;
    
#ifdef DEBUG
printf ("frametab[%ld] is %ld ==> last_block %ld, last_offs %ld, last_bytes %ld\n",
	last_frame + 1, frametab[last_frame + 1], last_block, last_offs, last_bytes);
#endif

    info.ioi_Recv.iob_Buffer = load_buf[1];
    SendIO (ireq, &info);
    info.ioi_Offset += blocks;

    cur_idx = 0;
    load_idx = 1;
    wbp = work_buf = load_buf[0];

    io_sig = AllocSignal (0);
    SendSignal (parent_task, parent_sig);

    for (;;)
    {
	WaitSignal (io_sig);
	if (io_cmd != IO_BUF_FREE)
	    break;

#ifdef DEBUG
printf ("load_frame %ld:\n", load_frame);
#endif
	{
	    int32	leftover;

	    if (io_offs == 0)
		wbp = work_buf = load_buf[cur_idx]; // redundant if !last_buf?

	    if (work_buf == last_buf)
		leftover = last_bytes - (wbp - work_buf);
	    else
		leftover = bytes - (wbp - work_buf);
		
	    if (io_len < leftover)
	    {
#ifdef DEBUG
printf ("    0: %ld bytes from buffer %ld\n", io_len, work_buf == last_buf ? -1 : cur_idx);
#endif
		memcpy (bufs[lastbuf], wbp, io_len);
		buf_flags |= (1 << lastbuf);
		
		wbp += io_len;
	    }
	    else
	    {
#ifdef DEBUG
printf ("    1: %ld bytes from buffer %ld\n", leftover, work_buf == last_buf ? -1 : cur_idx);
#endif
		memcpy (bufs[lastbuf], wbp, leftover);
		io_len -= leftover;
		io_offs += leftover;
		if (io_len >= 0)
		{
		    int32	switch_to_last;
		    
		    switch_to_last = 0;
		    if (last_buf && io_offs >= last_offs)
		    {
			if (work_buf != last_buf)
			{
			    // this makes us WaitIO below
			    switch_to_last = 1;
			    wbp = work_buf = last_buf;
			}
		    }
		    else
		    {
			WaitIO (ireq);
			if (++cur_idx == NUM_FILE_BUFS)
			    cur_idx = 0;
			switch_to_last = 1;
			wbp = work_buf = load_buf[cur_idx];
		    }

		    if (io_len > 0)
		    {
#ifdef DEBUG
printf ("    2 : %ld bytes from buffer %ld\n", io_len, work_buf == last_buf ? -1 : cur_idx);
#endif
			memcpy (bufs[lastbuf] + leftover, wbp, io_len);
			io_offs += io_len;
			wbp += io_len;
		    }
		    
		    buf_flags |= (1 << lastbuf);
		    
		    if (work_buf == last_buf)
		    {
			if (switch_to_last)
			{
			    // I just switched -- WaitIO and start another IO request
			    WaitIO (ireq);
			    if (++cur_idx == NUM_FILE_BUFS)
				cur_idx = 0;
			}
			else // DON'T start an IO request!
			    continue;
		    }
		    
		    // load into next buf
		    if (++load_idx == NUM_FILE_BUFS)
			load_idx = 0;
#ifdef DEBUG
printf ("    0: reading %ld bytes at %ld (block %ld) into buffer %ld\n",
	bytes, info.ioi_Offset * BLOCK_SIZE, info.ioi_Offset, load_idx);
#endif
		    info.ioi_Recv.iob_Buffer = load_buf[load_idx];
		    SendIO (ireq, &info);
		    info.ioi_Offset += blocks;
		    if (info.ioi_Offset == last_block)
			info.ioi_Offset = 0;

		    // this is the case where I immediately use all of the next buffer			
		    if (!last_buf && switch_to_last && io_offs == frametab[last_frame + 1])
		    {
			WaitIO (ireq);
			if (++cur_idx == NUM_FILE_BUFS)
			    cur_idx = 0;
		    
			if (++load_idx == NUM_FILE_BUFS)
			    load_idx = 0;
#ifdef DEBUG
printf ("    1: reading %ld bytes at %ld (block %ld) into buffer %ld\n",
	bytes, info.ioi_Offset * BLOCK_SIZE, info.ioi_Offset, load_idx);
#endif
			info.ioi_Recv.iob_Buffer = load_buf[load_idx];
			SendIO (ireq, &info);
			info.ioi_Offset += blocks;
		    }
		}
	    }
	}
    }

Cleanup:
    if (last_buf)
	ThreedoFree (last_buf, last_alloc);

    // It's conceivable that we could break out without completing I/O (or is it?)
    // Since I/O request uses buf, we should WaitIO before freeing buf.
    if (ireq >= 0)
	WaitIO (ireq);
	
    if (buf)
	ThreedoFree (buf, bytes * NUM_FILE_BUFS);
    if (ireq >= 0)
	DeleteIOReq (ireq);
    if (fi >= 0)
	CloseDiskFile (fi);
    FreeSignal (io_sig);
    SendSignal (parent_task, parent_sig);
    WaitSignal (0); // wait forever
}
#endif

#ifdef STANDALONE
int32	framecount, frameticks;

#define ONE_SECOND	239
#define FRAME_INFO(f) \
    do \
    { \
	if (frameticks > ONE_SECOND) \
	{ \
	    int32	numer; \
	    GrafCon	GCon; \
	    char	frmstr[32]; \
	    \
	    numer = framecount * ONE_SECOND; \
	    sprintf (frmstr,"%02ld (%04ld) -- %02ld.%02ld/sec", \
		    (f), \
		    framecount, \
		    numer / frameticks, \
		    (numer % frameticks) * 100 / frameticks); \
	    MoveTo (&GCon, 30, 50); \
	    DrawText8 (&GCon, TheScreen.sc_BitmapItems[curscreen], frmstr); \
	} \
	else \
	{ \
	    GrafCon	GCon; \
	    char	frmstr[32]; \
	    \
	    sprintf (frmstr,"%02ld (%04ld) -- --/sec", \
		    (f), \
		    framecount); \
	    MoveTo (&GCon, 30, 50); \
	    DrawText8 (&GCon, TheScreen.sc_BitmapItems[curscreen], frmstr); \
	} \
    } while (0)
#else
#define FRAME_INFO(f)
#endif

static void
showframe (s32 frame)
{
    ubyte	*screen;
    static int	curscreen = 0;

    screen = (ubyte*)GetPixelAddress (TheScreen.sc_Screens[curscreen], 0, 0);

    while (!(buf_flags & (1 << lastbuf)))
	Yield ();
    dxframe (bufs[lastbuf], screen, &plr);
    buf_flags &= ~(1 << lastbuf);
    
    FRAME_INFO (frame);

    DisplayScreen (TheScreen.sc_Screens[curscreen], 0);

    if (++frame > last_frame)
	frame = first_frame;

#if defined(FIRST_BUF) || defined(DEBUG)	
    load_frame = frame;
#endif
    io_offs = frametab[frame];
    io_len = frametab[frame + 1] - frametab[frame];

    SendSignal (io_task, io_sig);
	
    if (++curscreen == plr.numScreens)
	curscreen = 0;
}

void clearScrn(u32 *screen) {

	int x,y;
	for(y=0;y<120;y++) {
		for(x=0;x<320;x++) {
			*screen++=0;
		}
	}
}

void
makename(char *name,char *body,char *ext)
{
	strcpy(name,"duckart/");
	strcat(name,body);
	strcat(name,".");
	strcat(name,ext);
}

int
fix_frametable(u32 *frametab)
{
	long	cnt,offset;

	Stream	*sp;
	
	sp = OpenDiskStream (gfilename, 0);
	
	SeekDiskStream (sp, frametab[length], SEEK_SET);
	ReadDiskStream (sp, (char *)&cnt, sizeof (cnt));
	offset = cnt+8;

	SeekDiskStream (sp, frametab[length] + offset + 4, SEEK_SET);
	ReadDiskStream (sp, (char *)&cnt, sizeof (cnt));
	offset += longswp(cnt)/8 + 16;

	CloseDiskStream (sp);
	
	frametab[length+1] = frametab[length] + offset;

	length++;
	
	return(0);
}

static u32 *
loadftable(char *name)
{	
	Stream *sp;
	long len,res;
	u32 *p;
	char filename[40];
	
	makename(filename,name,"frm");

	sp = OpenDiskStream(filename,0);
	if(!sp) {
		PRD (("error opening %s\n",filename));
		return((u32 *)0L);
	}
	len = SeekDiskStream(sp,0L,SEEK_END);
	length = len/4 - 1;
	SeekDiskStream(sp,0L,SEEK_SET);

	p = (u32 *)ThreedoAlloc(frametab_size = len+4);
	if(!p) {
		PRD (("couldn't malloc %ld\n",len));
		return((u32*)0L);
	}

	res = (long)ReadDiskStream(sp,(char *)p,len);
	
	if(res != len) {
		PRD (("couldn't read frmtbl %ld\n",res));
		return((u32*)0L);
	}

	CloseDiskStream(sp);

	return(p);	
}

void
changedir(char *name)
{
	if (ChangeDirectory(name) < 0)
	{
		PRD(("Cannot change directory to '%s'!\n", name));
	}
}

s32
InitVidLoop (char *name, u32 startframe, u32 numframes)
{
	int32	i;
	char	filename[40];

	first_time = 1;
	
	plr.numScreens = 2;//kNumScreens;

	if(!setupScreens(&plr,plr.numScreens)) {
		PRD (("unable to initialize screens\n"));
		return(0);
	}

	FadeToBlack (&TheScreen, 0);
	
	for(i=0;i<TheScreen.sc_nScreens;i++)
		clearScrn((u32*)GetPixelAddress(TheScreen.sc_Screens[i],0,0));

	makename(gfilename,name,"duk");

	frametab = loadftable(name);
	if(!frametab) {
		PRD (("Error loading frame table\n"));
		return(0);
	}

	fix_frametable(frametab);

	makename(filename,name,"hdr");
	if(!loadParams(&plr,filename)) {
		PRD (("Error loading params\n"));
		ThreedoFree (frametab);
		return(0);
	}
	
	if(!numframes)
	    numframes = length;

	first_frame = startframe;
	last_frame = startframe + numframes - 1;

	max_frame_size = 0;
	for (i = first_frame; i <= last_frame; i++)
	{
	    int32	s;

	    s = frametab[i + 1] - frametab[i];
	    if (s > max_frame_size)
		max_frame_size = s;
	}

	if (!(buf = (ubyte *)ThreedoAlloc (buf_size = max_frame_size)))
	{
	    ThreedoFree (frametab, frametab_size); 
	    return (0);
	}
	
	bufs[0] = buf;

	makename(filename,name,"tbl");
	setupPlayer(&plr,filename);

	parent_sig = AllocSignal (0);
	parent_task = KernelBase->kb_CurrentTask->t.n_Item;
	if ((io_task = CreateThread ("io_task",
		KernelBase->kb_CurrentTask->t.n_Priority + 10, io_task_func, 2048)) < 0)
	{
	    ThreedoFree (buf, buf_size);
	    ThreedoFree (frametab, frametab_size); 
	    return (0);
	}
	
	WaitSignal (parent_sig);
	if (io_cmd == IO_FAILURE)
	{
	    ThreedoFree (buf, buf_size);
	    ThreedoFree (frametab, frametab_size); 
	    return (0);
	}

	buf_flags = 0;
#if defined(FIRST_BUF) || defined(DEBUG)	
	load_frame = first_frame;
#endif
	lastbuf = 0;
	io_cmd = IO_BUF_FREE;
	io_offs = frametab[first_frame];
	io_len = frametab[first_frame + 1] - frametab[first_frame];
	SendSignal (io_task, io_sig);

#ifdef STANDALONE
framecount = frameticks = 0;
#endif

	curframe = first_frame;
	
	return (1);
}

void
UninitVidLoop ()
{
	ThreedoFree (buf, buf_size);
	ThreedoFree (frametab, frametab_size);

	io_cmd = IO_CLOSE;
	SendSignal (io_task, io_sig);
	WaitSignal (parent_sig);
	DeleteThread (io_task);
	FreeSignal (parent_sig);

	FadeToBlack (&TheScreen, 15);

	uninitScreens ();
}

int32
DoVidLoop ()
{
    showframe (curframe);
    if (++curframe > last_frame)
	curframe = first_frame;

    if (first_time)
    {
	first_time = 0;
	FadeFromBlack (&TheScreen, 15);
    }
    
    return (curframe);
}
