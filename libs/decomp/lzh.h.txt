#ifndef _LZH_H
#define _LZH_H

#include "reslib.h"
#define _DEC_PROTOS
#include "declib.h"

/* LZSS Parameters */

#define N		4096	/* Size of string buffer */
#define F		16	/* Size of look-ahead buffer */
//#define F		60	/* Size of look-ahead buffer */
#define THRESHOLD	2
#define NIL		N	/* End of tree's node  */

/* Huffman coding parameters */

#define N_CHAR  	(256 - THRESHOLD + F)
				/* character code (= 0..N_CHAR-1) */
#define T 		(N_CHAR * 2 - 1)	/* Size of table */
#define R 		(T - 1)			/* root position */
#define MAX_FREQ	0x8000
					/* update when cumulative frequency */

typedef struct
{
    MEM_HANDLE	fh;

    COUNT	buf_index, restart_index, bytes_left;
    BYTE	text_buf[N + F - 1];
	/* reconstruct freq tree */
    COUNT	freq[T + 1];	/* cumulative freq table */
	/*
	 * pointing parent nodes.
	 * area [T..(T + N_CHAR - 1)] are pointers for leaves
	 */
    COUNT	prnt[T + N_CHAR];
	/* pointing children nodes (son[], son[] + 1)*/
    COUNT	son[T];
    UWORD	workbuf;
    BYTE	workbuflen;

    STREAM_TYPE	StreamType;

    LPVOID	Stream;
    DWORD	StreamIndex, StreamLength;

    STREAM_MODE	StreamMode;
    NPVOIDFUNC	CleanupFunc;
} LZHCODE_DESC;
typedef LZHCODE_DESC	*PLZHCODE_DESC;
typedef LZHCODE_DESC	near *NPLZHCODE_DESC;
typedef LZHCODE_DESC	far *LPLZHCODE_DESC;

#define InChar()	(_StreamType == FILE_STREAM ? \
				GetResFileChar ((FILE *)_Stream) : \
				(int)*_Stream++)
#define OutChar(c)	(_StreamType == FILE_STREAM ? \
				PutResFileChar ((c), (FILE *)_Stream) : \
				(*_Stream++ = (BYTE)(c)))


#define AllocCodeDesc()		\
	mem_allocate ((MEM_SIZE)sizeof (LZHCODE_DESC), \
	MEM_ZEROINIT, DEFAULT_MEM_PRIORITY, MEM_SIMPLE)
#define LockCodeDesc	(LPLZHCODE_DESC)mem_lock
#define UnlockCodeDesc	mem_unlock
#define FreeCodeDesc	mem_release

PROC_GLOBAL(
void near _update, (c),
    ARG_END	(COUNT	c)
);
PROC_GLOBAL(
void near StartHuff, (),
    ARG_VOID
);

#undef DECODE_REF
#define DECODE_REF	LPLZHCODE_DESC
#undef _DEC_PROTOS
#include "declib.h"

extern LPLZHCODE_DESC	_lpCurCodeDesc;
extern STREAM_TYPE	_StreamType;
extern LPBYTE		_Stream;
extern UWORD		_workbuf;
extern BYTE		_workbuflen;

#endif /* _LZH_H */

