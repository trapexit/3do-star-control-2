#include "starcon.h"

typedef struct
{
    MEM_HANDLE	h;
    char	*name;
    DWORD	max_size;
    DWORD	cur_pos, bytes_used;
} RAM_FILE_DESC;

#define MAX_RAM_FILES	4
static RAM_FILE_DESC	*RFList[MAX_RAM_FILES];

//#define DEBUG

PROC(
LPVOID OpenRamFile, (name, max_size),
    ARG		(char	*name)
    ARG_END	(DWORD	max_size)
)
{
    BYTE	i, use_i;

    i = use_i = MAX_RAM_FILES;
    while (i--)
    {
	if (RFList[i] == 0)
	    use_i = i;
	else if (strcasecmp (name, RFList[i]->name) == 0)
	{
	    RFList[i]->cur_pos = 0;
	    return (RFList[i]);
	}
    }
	
    if (use_i == MAX_RAM_FILES || max_size == 0)
    {
#ifdef DEBUG
printf ("OpenRamFile: '%s' FAILED!!! (no more slots)\n", name);
#endif /* DEBUG */
	return (0);
    }
    else
    {
	MEM_HANDLE	h;
	RAM_FILE_DESC	*pRF;
	
	h = mem_request (max_size + sizeof (RAM_FILE_DESC));
	if ((pRF = (RAM_FILE_DESC *)mem_lock (h)) == 0)
	    mem_release (h);
	else
	{
	    pRF->h = h;
	    pRF->name = name;
	    pRF->max_size = max_size;
	    pRF->cur_pos = 0;
	    pRF->bytes_used = 0;
	
	    RFList[use_i] = pRF;
	}
    
#ifdef DEBUG
if (pRF == 0)
printf ("OpenRamFile: '%s' FAILED!!! (can't get memory)\n", name);
#endif /* DEBUG */
        return ((LPVOID)pRF);
    }
}

PROC(
void CloseRamFile, (pRF),
    ARG_END	(LPVOID	pRF)
)
{
    if (pRF)
	((RAM_FILE_DESC *)pRF)->cur_pos = 0;
}

PROC(
void DeleteRamFile, (name),
    ARG_END	(char	*name)
)
{
    BYTE	i;
    
    for (i = 0; i < MAX_RAM_FILES; ++i)
    {
	if (RFList[i] && strcasecmp (name, RFList[i]->name) == 0)
	{
#ifdef DEBUG
printf ("DeleteRamFile: '%s', bytes used = %lu(%lu)\n",
	name, RFList[i]->bytes_used, RFList[i]->max_size);
#endif /* DEBUG */
	    mem_unlock (RFList[i]->h);
	    mem_release (RFList[i]->h);
	    RFList[i] = 0;
	    
	    break;
	}
    }
}

PROC(
int ReadRamFile, (lpBuf, size, count, fp),
    ARG		(LPVOID	lpBuf)
    ARG		(COUNT	size)
    ARG		(COUNT	count)
    ARG_END	(LPVOID	fp)
)
{
    if (fp)
    {
	LPBYTE	lpSrc;
	
	lpSrc = (LPBYTE)&((RAM_FILE_DESC *)fp)[1];
	size *= count;
	MEMCPY (lpBuf, lpSrc + ((RAM_FILE_DESC *)fp)->cur_pos, size);
	((RAM_FILE_DESC *)fp)->cur_pos += size;
	
	return ((int)count);
    }
    
    return (0);
}

PROC(
int GetRamFileChar, (fp),
    ARG_END	(LPVOID	fp)
)
{
    if (fp)
    {
	BYTE	c;
	LPBYTE	lpSrc;
	
	lpSrc = (LPBYTE)&((RAM_FILE_DESC *)fp)[1];
	c = *(lpSrc + ((RAM_FILE_DESC *)fp)->cur_pos);
	++((RAM_FILE_DESC *)fp)->cur_pos;
	
	return ((int)c);
    }
	
    return (-1);
}

PROC(
int PutRamFileChar, (c, fp),
    ARG		(unsigned char	c)
    ARG_END	(LPVOID		fp)
)
{
    if (fp)
    {
	LPBYTE	lpDst;
	
	lpDst = (LPBYTE)&((RAM_FILE_DESC *)fp)[1];
	*(lpDst + ((RAM_FILE_DESC *)fp)->cur_pos) = c;
	++((RAM_FILE_DESC *)fp)->cur_pos;
	if (((RAM_FILE_DESC *)fp)->cur_pos > ((RAM_FILE_DESC *)fp)->bytes_used)
	    ((RAM_FILE_DESC *)fp)->bytes_used = ((RAM_FILE_DESC *)fp)->cur_pos;
	
	return ((int)c);
    }
	
    return (-1);
}

PROC(
long SeekRamFile, (fp, offset, whence),
    ARG		(LPVOID	fp)
    ARG		(long	offset)
    ARG_END	(int	whence)
)
{
    if (fp)
    {
	if (whence == SEEK_CUR)
	    offset += (long)((RAM_FILE_DESC *)fp)->cur_pos;
	else if (whence == SEEK_END)
	    offset = (long)((RAM_FILE_DESC *)fp)->bytes_used;

	return (((RAM_FILE_DESC *)fp)->cur_pos = offset);
    }

    return (-1);
}

PROC(
long TellRamFile, (fp),
    ARG_END	(LPVOID	fp)
)
{
    if (fp)
	return (((RAM_FILE_DESC *)fp)->cur_pos);

    return (-1);
}

PROC(
long LengthRamFile, (fp),
    ARG_END	(LPVOID	fp)
)
{
    if (fp)
	return (((RAM_FILE_DESC *)fp)->bytes_used);

    return (-1);
}

PROC(
int WriteRamFile, (lpBuf, size, count, fp),
    ARG		(LPVOID	lpBuf)
    ARG		(COUNT	size)
    ARG		(COUNT	count)
    ARG_END	(LPVOID	fp)
)
{
    if (fp)
    {
	LPBYTE	lpDst;
	
	lpDst = (LPBYTE)&((RAM_FILE_DESC *)fp)[1];
	size *= count;
	MEMCPY (lpDst + ((RAM_FILE_DESC *)fp)->cur_pos, lpBuf, size);
	((RAM_FILE_DESC *)fp)->cur_pos += size;
	if (((RAM_FILE_DESC *)fp)->cur_pos > ((RAM_FILE_DESC *)fp)->bytes_used)
	    ((RAM_FILE_DESC *)fp)->bytes_used = ((RAM_FILE_DESC *)fp)->cur_pos;
#ifdef DEBUG
if (((RAM_FILE_DESC *)fp)->bytes_used > ((RAM_FILE_DESC *)fp)->max_size)
printf ("WriteRamFile: ERROR writing beyond max size (%lu > %lu)\n",
((RAM_FILE_DESC *)fp)->bytes_used, ((RAM_FILE_DESC *)fp)->max_size);
#endif /* DEBUG */
	
	return ((int)count);
    }
    
    return (0);
}

