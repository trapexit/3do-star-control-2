#include <stdio.h>
#include <string.h>
#include "compiler.h"

#if 0
#ifdef HORDE
#define DATADIR		"horde.dat/"
#else
#define DATADIR		"starcon2/"
#endif
#endif

static FILE	*NVRAMfp;
#ifdef DEBUG
static int	num_open_files;
#endif /* DEBUG */

extern FILE	*OpenNVRAMFile (char *, char *, char *, unsigned long);
extern int	CloseNVRAMFile (FILE *);
extern int	ReadNVRAMFile (void *, int, int, FILE *);
extern int	WriteNVRAMFile (void *, int, int, FILE *);

FILE *
OpenResFile (char *filename, char *mode, char *buf, unsigned long bufsize, void *FileErrorFunc)
{
    if (*mode == 'w')
    {
	DeleteNVRAMFile (filename);
	return (NVRAMfp = OpenNVRAMFile (filename, mode, buf, bufsize));
    }
    else
    {
	char		realfile[256];
	FILE		*fp;
	extern FILE	*OpenDiskStream (char *, long);

#if 0 	
	sprintf (realfile, "%s%s", DATADIR, filename);
#else
	sprintf (realfile, "$MyProduct/%s", filename);
#endif
	if (!(fp = OpenDiskStream (realfile, 0)))
	    return (NVRAMfp = OpenNVRAMFile (filename, mode, buf, bufsize));
#ifdef DEBUG
else ++num_open_files;
#endif /* DEBUG */
	    
	return (fp);
    }
}

int
CloseResFile (FILE *fp)
{
    if (fp)
    {
	extern void	CloseDiskStream (FILE *fp);
	
	if (fp == NVRAMfp)
	{
	    int	ret;
	    
	    ret = CloseNVRAMFile (fp);
	    NVRAMfp = 0;
	    
	    return (ret);
	}
	
#ifdef DEBUG
--num_open_files;
#endif /* DEBUG */
	CloseDiskStream (fp);
	return (1);
    }

    return (0);
}

BOOLEAN
DeleteResFile (char *file)
{
    return (DeleteNVRAMFile (file));
}

void
_init_resfile_error (void *FileErrorFunc)
{
}

void
_uninit_resfile_error (void)
{
#ifdef DEBUG
printf ("Files still open: %d\n", num_open_files);
#endif /* DEBUG */
}

extern int ReadDiskStream (FILE *, LPVOID, int);
	
PROC(
int ReadResFile, (lpBuf, size, count, fp),
    ARG		(LPVOID	lpBuf)
    ARG		(COUNT	size)
    ARG		(COUNT	count)
    ARG_END	(FILE	*fp)
)
{
    if (IsChildTask ())
	size = ReadFromParent (lpBuf, size, count, fp);
    else if (fp == NVRAMfp)
	size = ReadNVRAMFile (lpBuf, size, count, fp);
    else
	size = ReadDiskStream (fp, lpBuf, size * count) / size;

    return ((int)size);
}

PROC(
int GetResFileChar, (fp),
    ARG_END	(FILE	*fp)
)
{
    unsigned char	c;
	
    if (fp == NVRAMfp)
    {
	if (ReadNVRAMFile (&c, 1, 1, fp) == 1)
	    return ((int)c);
	return (-1);
    }
	
    if (ReadDiskStream (fp, (void *)&c, 1) == 1)
	return ((int)c);
	
    return (-1);
}

PROC(
int PutResFileChar, (c, fp),
    ARG		(unsigned char c)
    ARG_END	(FILE	*fp)
)
{
    if (fp == NVRAMfp)
	return (WriteNVRAMFile (&c, 1, 1, fp) == 1 ? 1 : -1);
	
    return (-1);
}

extern long	SeekDiskStream (FILE *fp, long offset, int mode);

PROC(
long SeekResFile, (fp, offset, whence),
    ARG		(FILE	*fp)
    ARG		(long	offset)
    ARG_END	(int	whence)
)
{
    if (fp == NVRAMfp)
	return (SeekNVRAMFile (fp, offset, whence + 1));
	
    return (SeekDiskStream (fp, offset, whence + 1));
}

PROC(
long TellResFile, (fp),
    ARG_END	(FILE	*fp)
)
{
    if (fp == NVRAMfp)
	return (SeekNVRAMFile (fp, 0, SEEK_CUR));
	
    return (SeekDiskStream (fp, 0L, 2 /* SEEK_CUR */));
}

PROC(
long LengthResFile, (fp),
    ARG_END	(FILE	*fp)
)
{
    long	cur_loc, retval;
    
    if (fp == NVRAMfp)
    {
	cur_loc = SeekNVRAMFile (fp, 0L, 2 /* SEEK_CUR */);
	retval = SeekNVRAMFile (fp, 0L, 3 /* SEEK_END */);
	SeekNVRAMFile (fp, cur_loc, 1 /* SEEK_SET */);

	return (retval < 4 ? -1 : (retval - 4));
    }
    
    cur_loc = SeekDiskStream (fp, 0L, 2 /* SEEK_CUR */);
    retval = SeekDiskStream (fp, 0L, 3 /* SEEK_END */);
    SeekDiskStream (fp, cur_loc, 1 /* SEEK_SET */);

    return (retval);
}

PROC(
int WriteResFile, (lpBuf, size, count, fp),
    ARG		(LPVOID	lpBuf)
    ARG		(COUNT	size)
    ARG		(COUNT	count)
    ARG_END	(FILE	*fp)
)
{
    if (fp == NVRAMfp)
	return (WriteNVRAMFile (lpBuf, size, count, fp));
	
    return (0);
}

int
stricmp (char *s1, char *s2)
{
    return (strcasecmp (s1, s2));
}

void
strupr (char *s)
{
    char	c;
    
    while (c = *s)
    {
	if (c >= 'a' && c <= 'z')
	    *s = c - 'a' + 'A';
	s++;
    }
}


