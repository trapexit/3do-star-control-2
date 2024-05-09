#include "Portfolio.h"
#include "filefunctions.h"
#include "filestream.h"
#include "directory.h"
#include "directoryfunctions.h"
#include "BlockFile.h"
#include "stdio.h"

//#define NVRAM_DEBUG

#define NVRAM_FILE_SIZE		(3 * 1024)
#define NVRAM_AVAILABLE		(32 * 1024)
#define NUMBER_OF_SAVES		(NVRAM_AVAILABLE / NVRAM_FILE_SIZE)

#define NVRAM_BUF_SIZE		(2 * 1024)

#define WRITE_CHARS(fp,s) \
	do \
	{ \
	    int	size; \
	    \
	    size = NVRAMp - NVRAMbuf; \
	    if (size > 0) \
	    { \
		(*s) = do_write (NVRAMbuf, size, 1, (fp)); \
		NVRAMp = NVRAMbuf; \
	    } \
	    else \
		(*s) = 1; \
	} while (0)

#define	NVRAM_WRITING		(1<<0)
#define NVRAM_READING		(1<<1)
#define NVRAM_USER_BUF		(1<<2)

typedef struct
{
    int		pos;	// current position in file
    int		len;	// file length
    int		flags;	// reading or writing
    char	*p;	// current position in buffer
    char	*buf;	// buffer to read/write data
    int		bufsize;// size of buffer
    Item	ioreq;
} NVRAM_INFO;

static NVRAM_INFO	NVRAMinfo;

#define	NVRAMpos	(NVRAMinfo.pos)
#define	NVRAMlen	(NVRAMinfo.len)
#define	NVRAMflags	(NVRAMinfo.flags)
#define	NVRAMbuf	(NVRAMinfo.buf)
#define	NVRAMbufsize	(NVRAMinfo.bufsize)
#define	NVRAMp		(NVRAMinfo.p)
#define	NVRAMioreq	(NVRAMinfo.ioreq)

#if 0//def HORDE
#define IS_OUR_FILE(f)	(strlen (f) == 11 \
	&& (f[0] == 's' || f[0] == 'S') \
	&& (f[1] == 'g' || f[1] == 'G') \
	&& f[8] == '.' && f[2] < '4')
#else
#define IS_OUR_FILE(f)	(strncmp (f, "starcon2", 8) == 0)
#endif
	
static int
GetFileInfo (Item ireq, DeviceStatus *ds)
{
    IOInfo	info;
    ubyte	buf[sizeof (DeviceStatus) + 1]; // need extra byte? or just a bug?

    /* get blockSize */
//    memset (ds, 0, sizeof (DeviceStatus));
    memset (buf, 0, sizeof (buf));
    memset (&info, 0, sizeof (IOInfo));
    info.ioi_Command = CMD_STATUS;
//    info.ioi_Recv.iob_Buffer = ds;
    info.ioi_Recv.iob_Buffer = buf;
    info.ioi_Recv.iob_Len = sizeof (DeviceStatus);
    if (DoIO (ireq, &info) < 0)
	return (0);

//    *ds = *(DeviceStatus *)buf;
    memcpy (ds, buf, sizeof (DeviceStatus));
    
    return (1);
}

void *
open_directory ()
{
    return ((void *)OpenDirectoryPath ("/nvram"));
}

void
close_directory (Directory *dir)
{
    CloseDirectory (dir);
}

int
read_directory (Directory *dir, char *pattern, char *name)
{
    DirectoryEntry	dirent;
	
    while (!ReadDirectory (dir, &dirent))
    {
	char	*pS;

	if (strcasecmp (pattern, dirent.de_FileName) == 0
		|| (pattern[0] == '*' && pattern[1] == '.'
		&& (pS = strchr (dirent.de_FileName, '.'))
		&& strcasecmp (&pattern[1], pS) == 0))
//		&& IS_OUR_FILE (dirent.de_FileName)))
	{
	    strcpy (name, dirent.de_FileName);
	    return (0);
	}
    }
    
    return (1);
}

static int
fill_read_buf (FILE *fp)
{
    Item	ireq;
    int		retval;
    
    retval = 0;
    if (ireq = NVRAMioreq)
    {
	DeviceStatus	ds;
	
	if (GetFileInfo (ireq, &ds))
	{
	    int		bytes_to_read;
	    int		bytes_needed;
	    void	*locbuf, *p;

	    if (NVRAMlen)
	    {
		if (NVRAMpos + NVRAMbufsize > NVRAMlen)
		    bytes_needed = NVRAMlen - NVRAMpos;
		else
		    bytes_needed = NVRAMbufsize;
	    }
	    else // first read
		bytes_needed = NVRAMbufsize;
		
	    if (ds.ds_DeviceBlockSize != 1) // seems to be 1
		bytes_to_read = ((bytes_needed + ds.ds_DeviceBlockSize - 1)
			/ ds.ds_DeviceBlockSize) * ds.ds_DeviceBlockSize;
	    else
		bytes_to_read = bytes_needed;

	    if (bytes_needed != bytes_to_read)
		p = locbuf = (void *)ThreedoAlloc (bytes_to_read);
	    else
	    {
		p = NVRAMbuf;
		locbuf = 0;
	    }
		
	    if (p)
	    {
		IOInfo	info;
		
		memset (&info, 0, sizeof (IOInfo));
		info.ioi_Command = CMD_READ;
		info.ioi_Recv.iob_Buffer = p;
		info.ioi_Recv.iob_Len = bytes_to_read;
		info.ioi_Offset = NVRAMpos;
		
		if (DoIO (ireq, &info) >= 0)
		{
		    retval = 1;
		    NVRAMp = NVRAMbuf;
		    NVRAMpos += bytes_needed;
		}
	    
		if (bytes_needed != bytes_to_read)
		{
		    if (retval)
			memcpy (NVRAMbuf, p, bytes_needed);
		    ThreedoFree (p, bytes_to_read);
		}
	    }
	}
#ifdef NVRAM_DEBUG
else printf ("Can't GetFileInfo!\n");
#endif /* NVRAM_DEBUG */
    }
    
    return (retval);
}

int
ReadNVRAMFile (void *data, int size, int count, FILE *fp)
{
    int	bytes_needed;
 
    bytes_needed = size * count;

    // asking for nothing, or starting after end of file
    if (!bytes_needed)// || NVRAMpos >= NVRAMlen)
	return (0);
	
    while (bytes_needed > 0)
    {
	int	bytes_left;
	
	bytes_left = NVRAMbufsize - (NVRAMp - NVRAMbuf);
	if (bytes_needed < bytes_left)
	{
	    // what we need is available in buffer
	    memcpy (data, NVRAMp, bytes_needed);
	    NVRAMp += bytes_needed;
	    
	    return (count);
	}
	else
	{
	    // need more than in buffer; grab what's left
	    // (if any) and refill buffer
	    if (bytes_left > 0)
	    {
		memcpy (data, NVRAMp, bytes_left);
		data = (void *)((char *)data + bytes_left);
		bytes_needed -= bytes_left;
	    }
	    
	    if (!fill_read_buf (fp))
		return (((size * count) - bytes_needed) / size); 
	}
    }
}

static int
do_write (void *data, int size, int count, FILE *fp)
{
    Item	ireq;
    int		retcount;
    int32	bytes_needed;
    
    bytes_needed = size * count;

    retcount = 0;
    if (ireq = NVRAMioreq)
    {
	DeviceStatus	ds;
	
	if (GetFileInfo (ireq, &ds))
	{
	    IOInfo	info;
	    int32	bytes_needed, bytes_to_write;
	    
	    bytes_needed = size * count;
	    if (ds.ds_DeviceBlockSize != 1) // seems to be 1
		bytes_to_write = ((size * count + ds.ds_DeviceBlockSize - 1)
			/ ds.ds_DeviceBlockSize) * ds.ds_DeviceBlockSize;
	    else
		bytes_to_write = bytes_needed;
	
	    memset (&info, 0, sizeof (IOInfo));
	    info.ioi_Command = CMD_WRITE;
	    info.ioi_Send.iob_Buffer = data;
	    info.ioi_Send.iob_Len = bytes_to_write;
	    info.ioi_Offset = NVRAMpos;

	    if (DoIO (ireq, &info) >= 0)
	    {
		retcount = count;
		NVRAMpos += bytes_needed;
		if (NVRAMpos > NVRAMlen)
		    NVRAMlen = NVRAMpos;
	    }
	}
    }

#ifdef NVRAM_DEBUG
if (!retcount) printf ("do_write failed!\n");    
#endif
    return (retcount);
}

int
WriteNVRAMFile (void *data, int size, int count, FILE *fp)
{
    int32	bytes_needed, status;
    
    bytes_needed = size * count;

    if (!bytes_needed)
	return (0);

    // won't fit in buffer, so flush out old chars
    if (bytes_needed > NVRAMbufsize - (NVRAMp - NVRAMbuf))
    {
 	WRITE_CHARS (fp, &status);
	if (status == 0)
	    return (0);
    }

    // fits in buffer, copy	
    if (bytes_needed <= NVRAMbufsize - (NVRAMp - NVRAMbuf))
    {
	memcpy (NVRAMp, data, bytes_needed);
	NVRAMp += bytes_needed;
	
	return (count);
    }
    
    // too big for one entire buffer
    // flush out old chars
    WRITE_CHARS (fp, &status);
    if (status == 0)
	return (0);
    
    // send new data all at once
    return (do_write (data, size, count, fp));
}

int
SeekNVRAMFile (FILE *fp, long offset, int whence)
{
    if (NVRAMflags & NVRAM_WRITING)
    {
	int32	status;
	
	WRITE_CHARS (fp, &status); // flush any chars not yet written
    }
#ifdef ACTUALLY_WORKS
    else
	NVRAMp = NVRAMbuf + NVRAMbufsize; // trigger new read
#endif /* ACTUALLY_WORKS */
    
    switch (whence)
    {
	case SEEK_CUR:
	    NVRAMpos += offset;
	    break;
	case SEEK_END:
	    NVRAMpos = NVRAMlen;
	    break;
	case SEEK_SET:
	    if (offset < 0 || offset > NVRAMlen)
		return (-1);
	    NVRAMpos = offset;
	    break;
    }
    
    return (NVRAMpos);
}

FILE *
OpenNVRAMFile (char *filename, char *mode, char *buf, unsigned long bufsize)
{
    Item		fi;
    char		NVRAMfile[256];
    Item		ireq;
    int32		curSize;
    DeviceStatus	ds;
    int			creating;
    
    memset (&NVRAMinfo, 0, sizeof (NVRAMinfo));

    if (buf)
    {
	NVRAMbuf = buf;
	NVRAMbufsize = bufsize;
	NVRAMflags |= NVRAM_USER_BUF;
    }
    else
    {
	if (!(NVRAMbuf = (char *)ThreedoAlloc (NVRAM_BUF_SIZE)))
	    return ((FILE *)0);
	NVRAMbufsize = NVRAM_BUF_SIZE;
    }
    
    sprintf (NVRAMfile, "/nvram/%s", filename);
#ifdef NVRAM_DEBUG
printf ("OpenNVRAMFile: '%s'\n", NVRAMfile);
#endif
    fi = OpenDiskFile (NVRAMfile);
    
    // READING -- simply see if file exists
    if (!(creating = (*mode == 'w')))
    {
	NVRAMflags |= NVRAM_READING;
	
	if (fi < 0)
	{
#ifdef NVRAM_DEBUG
printf ("OpenNVRAMFile: OpenDiskFile failed for READ!\n");
#endif
	    if (!(NVRAMflags & NVRAM_USER_BUF) && NVRAMbuf)
		ThreedoFree (NVRAMbuf, NVRAM_BUF_SIZE);
	    return ((FILE *)0);
	}
	
	ireq = CreateIOReq (NULL, 0, fi, 0);
	GetFileInfo (ireq, &ds);

	curSize = ds.ds_DeviceBlockSize * ds.ds_DeviceBlockCount;
	if (NVRAMbufsize > curSize)
	    NVRAMbufsize = curSize;
	    
	NVRAMioreq = ireq;
	
	NVRAMp = NVRAMbuf + NVRAMbufsize; // this will trigger buffer read
	ReadNVRAMFile (&NVRAMlen, sizeof (NVRAMlen), 1, (FILE *)fi);
#ifdef NVRAM_DEBUG
printf ("OpenNVRAMFile: length %d\n", NVRAMlen);
#endif
	
	return ((FILE *)fi);
    }
	
    NVRAMp = NVRAMbuf;
    NVRAMflags |= NVRAM_WRITING;

    // WRITING
    if (fi < 0)
    {
	if (CreateFile (NVRAMfile) < 0)
	{
#ifdef NVRAM_DEBUG
printf ("OpenNVRAMFile: CreateDiskFile failed!\n");
#endif
	    if (!(NVRAMflags & NVRAM_USER_BUF) && NVRAMbuf)
		ThreedoFree (NVRAMbuf, NVRAM_BUF_SIZE);
	    return ((FILE *)0);
	}
	    
	if ((fi = OpenDiskFile (NVRAMfile)) < 0)
	{
#ifdef NVRAM_DEBUG
printf ("OpenNVRAMFile: OpenDiskFile failed for WRITE!\n");
#endif
	    DeleteFile (NVRAMfile);
	    if (!(NVRAMflags & NVRAM_USER_BUF) && NVRAMbuf)
		ThreedoFree (NVRAMbuf, NVRAM_BUF_SIZE);
	    return ((FILE *)0);
	}
    }
	
    if ((ireq = CreateIOReq (NULL, 0, fi, 0)) < 0)
    {
#ifdef NVRAM_DEBUG
printf ("OpenNVRAMFile: CreateIOReq failed!\n");
#endif
	CloseDiskFile (fi);
	DeleteFile (NVRAMfile);
	if (!(NVRAMflags & NVRAM_USER_BUF) && NVRAMbuf)
	    ThreedoFree (NVRAMbuf, NVRAM_BUF_SIZE);
	return ((FILE *)0);
    }
	
    if (!GetFileInfo (ireq, &ds))
    {
#ifdef NVRAM_DEBUG
printf ("OpenNVRAMFile: GetFileInfo failed!\n");
#endif
	DeleteIOReq (ireq);
	CloseDiskFile (fi);
	DeleteFile (NVRAMfile);
	if (!(NVRAMflags & NVRAM_USER_BUF) && NVRAMbuf)
	    ThreedoFree (NVRAMbuf, NVRAM_BUF_SIZE);
	return ((FILE *)0);
    }

    curSize = ds.ds_DeviceBlockSize * ds.ds_DeviceBlockCount;
#undef NVRAM_FILE_SIZE
#define NVRAM_FILE_SIZE		(bufsize + 4)
    if (curSize != NVRAM_FILE_SIZE)// < NVRAM_FILE_SIZE)
    {
	IOInfo	info;

	// make it big enough
	memset (&info, 0, sizeof (IOInfo));
	info.ioi_Command = FILECMD_ALLOCBLOCKS;
	info.ioi_Offset = (NVRAM_FILE_SIZE - curSize + ds.ds_DeviceBlockSize - 1)
		/ ds.ds_DeviceBlockSize;	
	    
#ifdef NVRAM_DEBUG
printf ("OpenNVRAMFile: trying to make file %d bytes long\n", info.ioi_Offset);
#endif
	if (DoIO (ireq, &info) < 0)
	{
#ifdef NVRAM_DEBUG
printf ("OpenNVRAMFile: DoIO failed!\n");
#endif
	    DeleteIOReq (ireq);
	    CloseDiskFile (fi);
	    DeleteFile (NVRAMfile);
	    if (!(NVRAMflags & NVRAM_USER_BUF) && NVRAMbuf)
		ThreedoFree (NVRAMbuf, NVRAM_BUF_SIZE);
	    return ((FILE *)0);
	}
    }
    
    NVRAMioreq = ireq;

    WriteNVRAMFile (&NVRAMpos, sizeof (NVRAMpos), 1, (FILE *)fi);
	
    return ((FILE *)fi);
}

int
CloseNVRAMFile (FILE *fp)
{
    if (fp)
    {
	int32	status;
	    
	if (NVRAMflags & NVRAM_WRITING)
	{
	    // flush what's left
	    WRITE_CHARS (fp, &status);
	    if (status == 0)
		goto Failed;
#ifdef NVRAM_DEBUG
printf ("CloseNVRAMFile -- writing length %d\n", NVRAMlen);
#endif
	    NVRAMpos = 0;
	    WriteNVRAMFile (&NVRAMlen, sizeof (NVRAMlen), 1, fp);
	    // flush size
	    WRITE_CHARS (fp, &status);
	}
	else
	    status = 1;

Failed:
	DeleteIOReq (NVRAMioreq);

	CloseDiskFile ((Item)fp);
	if (!(NVRAMflags & NVRAM_USER_BUF))
	    ThreedoFree (NVRAMbuf, NVRAM_BUF_SIZE);
	
	return (status == 0 ? 0 : 1);
    }
    
    return (0);
}

int
DeleteNVRAMFile (char *file)
{
    char	NVRAMFile[80];
    
    sprintf (NVRAMFile, "/nvram/%s", file);
    return (DeleteFile (NVRAMFile) >= 0);
}

void
ManageNVRAM ()   
{
    extern Item	GetScreenGroup (void);
#if 0
    void    *buffer;

    buffer = AllocMem(100*1024,MEMTYPE_ANY);
    /* Allocate a work buffer, and create a MemList to go with it. Then
     * attach the MemList in a List. This List is what gets passed to the
     * tuner.
     */
    {
	MemList *ml;
	List     list;
	
	ml = AllocMemList(buffer,NULL);
	FreeMemToMemList(ml,buffer,100*1024);
	InitList(&list,NULL);
	AddHead(&list,(Node *)ml);
    
	StorageTunerRequest (GetScreenGroup (), &list, NULL);
	FreeMemList(ml);
    }
    FreeMem(buffer,100*1024);
#else
    StoreCurScreens ();
    StorageTunerRequest (GetScreenGroup (), NULL, NULL);
    RestoreCurScreens ();
#endif
}

void
CleanNVRAM ()
{
    Item	task;
    
    task = LoadProgram ("$c/lmadm -a ram 3 0 NVRAM");
    if (task < 0)
	return;

    WaitSignal (SIGF_DEADTASK);
}


