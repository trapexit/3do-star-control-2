#ifndef _RAMFILE_H
#define _RAMFILE_H

PROC_GLOBAL(
LPVOID OpenRamFile, (name, max_size),
    ARG		(char	*name)
    ARG_END	(DWORD	max_size)
);
PROC_GLOBAL(
void CloseRamFile, (pRF),
    ARG_END	(LPVOID	pRF)
);
PROC_GLOBAL(
void DeleteRamFile, (name),
    ARG_END	(char	*name)
);
PROC_GLOBAL(
int ReadRamFile, (lpBuf, size, count, fp),
    ARG		(LPVOID	lpBuf)
    ARG		(COUNT	size)
    ARG		(COUNT	count)
    ARG_END	(LPVOID	fp)
);
PROC_GLOBAL(
int GetRamFileChar, (fp),
    ARG_END	(LPVOID	fp)
);
PROC_GLOBAL(
int PutRamFileChar, (c, fp),
    ARG		(unsigned char	c)
    ARG_END	(LPVOID		fp)
);
PROC_GLOBAL(
long SeekRamFile, (fp, offset, whence),
    ARG		(LPVOID	fp)
    ARG		(long	offset)
    ARG_END	(int	whence)
);
PROC_GLOBAL(
long TellRamFile, (fp),
    ARG_END	(LPVOID	fp)
);
PROC_GLOBAL(
long LengthRamFile, (fp),
    ARG_END	(LPVOID	fp)
);
PROC_GLOBAL(
int WriteRamFile, (lpBuf, size, count, fp),
    ARG		(LPVOID	lpBuf)
    ARG		(COUNT	size)
    ARG		(COUNT	count)
    ARG_END	(LPVOID	fp)
);

#endif /* _RAMFILE_H */
