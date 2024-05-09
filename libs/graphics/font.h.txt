#ifndef _FONT_H
#define _FONT_H

#define FIRST_CHAR	(BYTE)' '
#define MAX_CHARS	96
#define MAX_DELTAS	100

typedef struct
{
    FONT_REF	FontRef;

    BYTE	Leading;
    BYTE	MaxAscender, MaxDescender;
    BYTE	CharSpace, KernAmount;
    BYTE	KernTab[MAX_CHARS];
    FRAME_DESC	CharDesc[MAX_CHARS];
} FONT_DESC;
typedef FONT_DESC	*PFONT_DESC;
typedef FONT_DESC	near *NPFONT_DESC;
typedef FONT_DESC	far *LPFONT_DESC;

#define FONTPTR		LPFONT_DESC
#define CHAR_DESCPTR	LPCHAR_DESC

#define FONT_PRIORITY	DEFAULT_MEM_PRIORITY

#define AllocFont(size)	\
    (FONT_REF)mem_allocate ((MEM_SIZE)(sizeof (FONT_DESC) + (size)), \
	    MEM_ZEROINIT | MEM_GRAPHICS, FONT_PRIORITY, MEM_SIMPLE)
#define LockFont(h)		(FONTPTR)mem_lock (h)
#define UnlockFont(h)		mem_unlock (h)
#define FreeFont		mem_release

#define NULL_FONT		(FONTPTR)NULL_PTR

extern FONTPTR			_CurFontPtr;

#endif /* _FONT_H */

