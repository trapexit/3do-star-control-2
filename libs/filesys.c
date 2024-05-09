#include "Portfolio.h"
#include "filefunctions.h"
#include "filestream.h"
#include "directory.h"
#include "directoryfunctions.h"
#include "BlockFile.h"
#include "stdio.h"

#define ThreedoAlloc(s)		AllocMem (s,MEMTYPE_ANY|MEMTYPE_FILL)
#define ThreedoFree(p,s)	FreeMem (p,s)

#define NVRAM_DEBUG

#define NVRAM_FILE_SIZE		(3 * 1024)
#define NVRAM_AVAILABLE		(32 * 1024)
#define NUMBER_OF_SAVES		(NVRAM_AVAILABLE / NVRAM_FILE_SIZE)

#define NVRAM_BUF_SIZE		(2 * 1024)

#define WRITE_CHARS(fp) \
	do \
	{ \
	    int	size; \
	    \
	    size = NVRAMp - NVRAMbuf; \
	    if (size > 0) \
	    { \
		do_write (NVRAMbuf, size, 1, (fp)); \
		NVRAMp = NVRAMbuf; \
	    } \
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

#define IS_OUR_FILE(f)	((strlen (f) == 11 \
	&& (f[0] == 's' || f[0] == 'S') \
	&& (f[1] == 'g' || f[1] == 'G') \
	&& f[8] == '.') \
	|| !strncmp (f, "starcon2", strlen ("starcon2")) \
	|| !strncmp (&f[strlen (f) - strlen (".mle")], ".mle", strlen (".mle")) \
	|| !strncmp (f, "melee.cfg", strlen ("melee.cfg")))

	
static int
GetFileInfo (Item ireq, DeviceStatus *ds)
{
    IOInfo	info;
ubyte	buf[sizeof (DeviceStatus) + 1];

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

*ds = *(DeviceStatus *)buf;
    return (1);
}

static void
DeleteNVRAMFiles ()
{
    Directory	*dir;
    int		deleted;

#ifdef NVRAM_DEBUG
printf ("deleteNVRAMfiles...\n");
#endif
Again:
    deleted = 0;
    if (dir = OpenDirectoryPath ("/nvram"))
    {
	DirectoryEntry	dirent;
	
	while (!ReadDirectory (dir, &dirent))
	{
	    char	*f;
	    
	    f = dirent.de_FileName;
	    if (!IS_OUR_FILE (f))
	    {
		char	filename[80];
		
		sprintf (filename, "/nvram/%s", f);
		printf ("Deleting NVRAM file '%s'\n", f);
		DeleteFile (filename);
		deleted = 1;
	    }
	}
    
	CloseDirectory (dir);
    }
    
    if (deleted)
	goto Again;
#ifdef NVRAM_DEBUG
printf ("deleteNVRAMfiles -- done\n");
#endif
}

static int
fill_read_buf (FILE *fp)
{
    Item	ireq;
    int		retval;
    
    retval = 0;
    if ((ireq = NVRAMioreq))//CreateIOReq (NULL, 0, (Item)fp, 0)) >= 0)
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
	
//	DeleteIOReq (ireq);
    }
    
    return (retval);
}

static int
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
    if ((ireq = NVRAMioreq))//CreateIOReq (NULL, 0, (Item)fp, 0)) >= 0)
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
	
//	DeleteIOReq (ireq);
    }

#ifdef NVRAM_DEBUG
if (!retcount) printf ("WriteNVRAMFile failed!\n");    
#endif
    return (retcount);
}

static int
WriteNVRAMFile (void *data, int size, int count, FILE *fp)
{
    int32	bytes_needed;
    
    bytes_needed = size * count;

    if (!bytes_needed)
	return (0);

    // won't fit in buffer, so flush out old chars
    if (bytes_needed > NVRAMbufsize - (NVRAMp - NVRAMbuf))
 	WRITE_CHARS (fp);

    // fits in buffer, copy	
    if (bytes_needed <= NVRAMbufsize - (NVRAMp - NVRAMbuf))
    {
	memcpy (NVRAMp, data, bytes_needed);
	NVRAMp += bytes_needed;
	
	return (count);
    }
    
    // too big for one entire buffer
    // flush out old chars
    WRITE_CHARS (fp);
    
    // send new data all at once
    return (do_write (data, size, count, fp));
}

static FILE *
OpenNVRAMFile (char *filename, char *mode, char *buf, unsigned long bufsize)
{
    Item		fi;
    char		NVRAMfile[20];
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
	    
//	DeleteIOReq (ireq);
NVRAMioreq = ireq;
	
	NVRAMp = NVRAMbuf + NVRAMbufsize; // this will trigger buffer read
	ReadNVRAMFile (&NVRAMlen, sizeof (NVRAMlen), 1, (FILE *)fi);
#ifdef NVRAM_DEBUG
printf ("OpenNVRAMFile: length %d\n", NVRAMlen);
#endif
	
	return ((FILE *)fi);
    }
//    else
//	deleteNVRAMfiles ();
	
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
    
//    DeleteIOReq (ireq);
NVRAMioreq = ireq;

    WriteNVRAMFile (&NVRAMpos, sizeof (NVRAMpos), 1, (FILE *)fi);
	
    return ((FILE *)fi);
}

static int
CloseNVRAMFile (FILE *fp)
{
    if (fp)
    {
	if (NVRAMflags & NVRAM_WRITING)
	{
	    // flush what's left
	    WRITE_CHARS (fp);
#ifdef NVRAM_DEBUG
printf ("CloseNVRAMFile -- writing length %d\n", NVRAMlen);
#endif
	    NVRAMpos = 0;
	    WriteNVRAMFile (&NVRAMlen, sizeof (NVRAMlen), 1, fp);
	    // flush size
	    WRITE_CHARS (fp);
	}

DeleteIOReq (NVRAMioreq);

	CloseDiskFile ((Item)fp);
	if (!(NVRAMflags & NVRAM_USER_BUF))
	    ThreedoFree (NVRAMbuf, NVRAM_BUF_SIZE);
	
	return (1);
    }
    
    return (0);
}

static void
FileInfo (char *dir, char *name)
{
    Item		fi;
    Item		ireq;
    int32		curSize;
    DeviceStatus	ds;
    char		filename[80];

    sprintf (filename, "%s/%s", dir, name);  
    fi = OpenDiskFile (filename);
    ireq = CreateIOReq (NULL, 0, fi, 0);
    GetFileInfo (ireq, &ds);
    DeleteIOReq (ireq);
    CloseDiskFile (fi);

    curSize = ds.ds_DeviceBlockSize * ds.ds_DeviceBlockCount;
	
    printf ("file '%s' is %ld bytes long\n", filename, curSize);
}

static void
ListFiles (char *dirname)
{
    Directory	*dir;

    if (dir = OpenDirectoryPath (dirname))
    {
	DirectoryEntry	dirent;
	
	while (!ReadDirectory (dir, &dirent))
	{
	    printf ("found file '%s'\n", dirent.de_FileName);
	    FileInfo (dirname, dirent.de_FileName);
	}
    
	CloseDirectory (dir);
    }
}

static void
FillNvram ()
{
    char	file[80];
    int		i;

//Again:
    for (i = 0; i < 32; i++)
    {
	FILE	*fp;
	char	buf[1024];
	
	if (ReadControlPad (~0))
	    return;
	    
	sprintf (file, "file%d", i);
	if (fp = OpenNVRAMFile (file, "wb", 0, sizeof (buf)))
	{
	    WriteNVRAMFile (buf, sizeof (buf), 1, fp);
	    CloseNVRAMFile (fp);
	}
    }
    
//    goto Again;
}

static void
usage ()
{
    printf ("usage: filesys -dflc\n");
    exit (-1);
}

int
main (int argc, char **argv)
{
#define LIST_CDROM	(1<<0)
#define LIST_NVRAM	(1<<1)
#define DELETE_NVRAM	(1<<2)
#define FILL_NVRAM	(1<<3)
    int32	which;
    char	what[80];
    
    argv++;
    argc--;
    
    which = 0;
    what[0] = '\0';
    while (argc > 0)
    {
	if (argv[0][0] == '-')
	{
	    switch (argv[0][1])
	    {
		case 'd':
		    which |= DELETE_NVRAM;
		    break;
		case 'f':
		    which |= FILL_NVRAM;
		    break;
		case 'l':
		    which |= LIST_NVRAM;
		    break;
		case 'c':
		    which |= LIST_CDROM;
		    break;
	    }
	}
	else
	    strcpy (what, *argv);
	    
	argc--;
	argv++;
    }

    switch (which)
    {
	case DELETE_NVRAM:
	    DeleteNVRAMFiles ();
	    break;
	case FILL_NVRAM:
	    FillNvram ();
	    break;
	case LIST_NVRAM:
	    ListFiles ("/nvram");
	    break;
	case LIST_CDROM:
	{
	    char	buf[80];

	    if (what[0])	    
		sprintf (buf, "/cd-rom/%s", what);
	    else
		strcpy (buf, "/cd-rom");
	    ListFiles (buf);
	    break;
	}
	default:
	    usage ();
    }
    
    exit (0);
}

