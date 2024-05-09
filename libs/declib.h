#ifndef _DECLIB_H
#define _DECLIB_H

#define DECODE_REF	LPVOID

enum
{
    FILE_STREAM = 0,
    MEMORY_STREAM
};
typedef BYTE	STREAM_TYPE;

enum
{
    STREAM_READ = 0,
    STREAM_WRITE
};
typedef BYTE	STREAM_MODE;

#endif /* _DECLIB_H */

#ifndef _DEC_PROTOS
#define _DEC_PROTOS

PROC_GLOBAL(
DECODE_REF copen, (InStream, SType, SMode),
    ARG		(LPVOID		InStream)
    ARG		(STREAM_TYPE	SType)
    ARG_END	(STREAM_MODE	SMode)
);
PROC_GLOBAL(
DWORD cclose, (DecodeRef),
    ARG_END	(DECODE_REF	DecodeRef)
);
PROC_GLOBAL(
void cfilelength, (DecodeRef, pfilelen),
    ARG		(DECODE_REF	DecodeRef)
    ARG_END	(DWORD		*pfilelen)
);
PROC_GLOBAL(
COUNT cread, (lpStr, size, count, DecodeRef),
    ARG		(LPVOID		lpStr)
    ARG		(COUNT		size)
    ARG		(COUNT		count)
    ARG_END	(DECODE_REF	DecodeRef)
);
PROC_GLOBAL(
COUNT cwrite, (lpStr, size, count, DecodeRef),
    ARG		(LPVOID		lpStr)
    ARG		(COUNT		size)
    ARG		(COUNT		count)
    ARG_END	(DECODE_REF	DecodeRef)
);

#endif /* _DEC_PROTOS */

