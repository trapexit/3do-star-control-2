#ifndef _COMPILER_H
#define _COMPILER_H

#ifndef _MSC_VER
#define _MSC_VER	0
#endif /* _MSC_VER */
#ifndef __STDC__
#define __STDC__	0
#endif /* __STDC__ */

#ifndef EXTERN
#define EXTERN	extern
#endif /* EXTERN */
#ifndef STATIC
#define STATIC	static
#endif /* STATIC */

#if (__STDC__ || _MSC_VER)
#define PROC(f,ar,ad)		f(ad
#define PROC_GLOBAL(f,ar,ad)	EXTERN f(ad
#define PROC_LOCAL(f,ar,ad)	STATIC f(ad
#define PROC_PARAMETER(f,ar,ad)	f(ad
#define ARG(var)		var,
#define ARG_VOID		void)
#define ARG_VAR			...)
#define ARG_END(var)		var)
#else
#define PROC(f,ar,ad)		f ar ad
#define PROC_GLOBAL(f,ar,ad)	EXTERN f()
#define PROC_LOCAL(f,ar,ad)	STATIC f()
#define PROC_PARAMETER(f,ar,ad)	f()
#define ARG(var)		var;
#define ARG_VOID
#define ARG_VAR
#define ARG_END			ARG
#endif

#ifndef FARDATA
#define FARDATA		far
#endif

#if _MSC_VER || __WATCOMC__

#define INTERRUPT	_interrupt
#define CDECL		cdecl
#define PASCAL		pascal
#define FASTCALL	_fastcall
#define SLOWCALL
#define UNSIGNED	unsigned
#define SIGNED		signed
#define CONST		const
#define VOLATILE	volatile

#ifdef __WATCOMC__
#undef far
#define far
#undef near
#define near
#undef huge
#define huge
#undef PASCAL
#define PASCAL
#undef CDECL
#define CDECL
#endif /* __WATCOMC__ */

#else /* !_MSC_VER */

#define near
#define far
#define huge
#define INTERRUPT
#define CDECL
#define PASCAL
#define FASTCALL
#define SLOWCALL
#define UNSIGNED	unsigned
#ifdef AZTEC_C
#define SIGNED
#define CONST
#define VOLATILE
#else /* !AZTEC_C */
#define SIGNED		signed
#define CONST		const
#define VOLATILE	volatile
#endif /* AZTEC_C */

#endif /* _MSC_VER */

typedef UNSIGNED char	BYTE;
typedef BYTE		*PBYTE;
typedef BYTE		near *NPBYTE;
typedef BYTE		far *LPBYTE;
typedef UNSIGNED char	UBYTE;
typedef UBYTE		*PUBYTE;
typedef UBYTE		near *NPUBYTE;
typedef UBYTE		far *LPUBYTE;
typedef SIGNED char	SBYTE;
typedef SBYTE		*PSBYTE;
typedef SBYTE		near *NPSBYTE;
typedef SBYTE		far *LPSBYTE;
typedef UNSIGNED int	UWORD;
typedef UWORD		*PUWORD;
typedef UWORD		near *NPUWORD;
typedef UWORD		far *LPUWORD;
typedef SIGNED int	SWORD;
typedef SWORD		*PSWORD;
typedef SWORD		near *NPSWORD;
typedef SWORD		far *LPSWORD;
typedef UNSIGNED long	DWORD;
typedef DWORD		*PDWORD;
typedef DWORD		near *NPDWORD;
typedef DWORD		far *LPDWORD;

typedef SBYTE		*PSTR;
typedef SBYTE		near *NPSTR;
typedef SBYTE		far *LPSTR;

typedef void		*PVOID;
typedef void		near *NPVOID;
typedef void		far *LPVOID;

typedef UWORD		COUNT;
typedef COUNT		*PCOUNT;
typedef COUNT		near *NPCOUNT;
typedef COUNT		far *LPCOUNT;
typedef SWORD		SIZE;
typedef SIZE		*PSIZE;
typedef SIZE		near *NPSIZE;
typedef SIZE		far *LPSIZE;

#if _MSC_VER

#define PARAGRAPH_SHIFT		4

typedef union
{
    NPVOID	near_ptr;
    LPVOID	far_ptr;
    struct
    {
	UWORD	Offset, Segment;
    } Components;
} PTR_DESC;
#define GET_NEARPTR(pd)		(((PTR_DESC *)(pd))->near_ptr)
#define GET_FARPTR(pd)		(((PTR_DESC *)(pd))->far_ptr)
#define SET_NEARPTR(pd,val)	(((PTR_DESC *)(pd))->near_ptr)=(NPVOID)(val)
#define SET_FARPTR(pd,val)	(((PTR_DESC *)(pd))->far_ptr)=(LPVOID)(val)
#define GET_SEGMENT(pd)		(((PTR_DESC *)(pd))->Components.Segment)
#define SET_SEGMENT(pd,val)	(((PTR_DESC *)(pd))->Components.Segment=(UWORD)(val))
#define GET_OFFSET(pd)		(((PTR_DESC *)(pd))->Components.Offset)
#define SET_OFFSET(pd,val)	(((PTR_DESC *)(pd))->Components.Offset=(UWORD)(val))
#define FAR_PTR_ADD(fp,val)	\
do \
{ \
    register long	laddr; \
    \
    laddr = ((long)GET_SEGMENT (fp) << PARAGRAPH_SHIFT) \
	    + GET_OFFSET (fp) + (val); \
    SET_SEGMENT (fp, (UWORD)(laddr >> PARAGRAPH_SHIFT)); \
    SET_OFFSET (fp, (UWORD)laddr & (PARAGRAPH_SIZE - 1)); \
} while (0)

#define MEMSET(m,v,s)		\
    if (sizeof (m) == sizeof (PVOID)) \
	memset((PVOID)(m),v,s); \
    else \
	_fmemset((LPVOID)(m),v,s)
#define MEMCPY(dst,src,s)		\
    if (sizeof (dst) == sizeof (PVOID) \
	    && sizeof (src) == sizeof (PVOID)) \
	memcpy((PVOID)(dst),(PVOID)(src),s); \
    else \
	_fmemcpy((LPVOID)(dst),(LPVOID)(src),s)
#define MEMMOVE(dst,src,s)		\
    if (sizeof (dst) == sizeof (PVOID) \
	    && sizeof (src) == sizeof (PVOID)) \
	memmove((PVOID)(dst),(PVOID)(src),s); \
    else \
	_fmemmove((LPVOID)(dst),(LPVOID)(src),s)
#define MEMCMP(c1,c2,s)		\
    (sizeof (c1) == sizeof (PVOID) \
	    && sizeof (c2) == sizeof (PVOID) ? \
		memcmp((PVOID)(c1),(PVOID)(c2),s) : \
		_fmemcmp((LPVOID)(c1),(LPVOID)(c2),s))
#else /* !_MSC_VER */

#define PARAGRAPH_SHIFT		0

typedef PVOID			PTR_DESC;

#define GET_NEARPTR(pd)		((unsigned)*(pd))
#define GET_FARPTR		GET_NEARPTR
#define SET_NEARPTR(pd,val)	(*(pd)=(val))
#define SET_FARPTR		SET_NEARPTR
#define GET_SEGMENT(pd)		0
#define SET_SEGMENT(pd,val)
#define GET_OFFSET		GET_NEARPTR
#define SET_OFFSET		SET_NEARPTR
#define FAR_PTR_ADD(fp,val)	(*((LPBYTE *)(fp))+=(val))

#define MEMSET			memset
#define MEMCPY			memcpy
#define MEMMOVE			memmove
#define MEMCMP			memcmp

#endif /* _MSC_VER */

#define PARAGRAPH_SIZE		(1 << PARAGRAPH_SHIFT)

#define NULL_PTR		0L

typedef enum
{
    FALSE = 0,
    TRUE
} BOOLEAN;
typedef BOOLEAN	*PBOOLEAN;
typedef BOOLEAN	near *NPBOOLEAN;
typedef BOOLEAN	far *LPBOOLEAN;

typedef void 		(*PVOIDFUNC) ();  
typedef BOOLEAN 	(*PBOOLFUNC) ();  
typedef BYTE 		(*PBYTEFUNC) ();
typedef UWORD 		(*PUWORDFUNC) ();
typedef SWORD		(*PSWORDFUNC) ();
typedef DWORD		(*PDWORDFUNC) ();

typedef void 		(near *NPVOIDFUNC) ();  
typedef BOOLEAN 	(near *NPBOOLFUNC) ();  
typedef BYTE 		(near *NPBYTEFUNC) ();
typedef UWORD 		(near *NPUWORDFUNC) ();
typedef SWORD		(near *NPSWORDFUNC) ();
typedef DWORD		(near *NPDWORDFUNC) ();

typedef void 		(far *LPVOIDFUNC) ();  
typedef BOOLEAN 	(far *LPBOOLFUNC) ();  
typedef BYTE 		(far *LPBYTEFUNC) ();
typedef UWORD 		(far *LPUWORDFUNC) ();
typedef SWORD		(far *LPSWORDFUNC) ();
typedef DWORD		(far *LPDWORDFUNC) ();

#define MAKE_BYTE(lo,hi)	((BYTE)(((BYTE)(hi)<<(BYTE)4)|(BYTE)(lo)))
#define MAKE_WORD(lo,hi)	((UWORD)((BYTE)(hi)<<8)|(BYTE)(lo))
#define MAKE_DWORD(lo,hi)	(((DWORD)(hi)<<16)|(UNSIGNED short)(lo))
#define LONIBBLE(x)		((BYTE)((BYTE)(x)&(BYTE)0x0F))
#define HINIBBLE(x)		((BYTE)((BYTE)(x)>>(BYTE)4))
#define LOBYTE(x)		((BYTE)((UWORD)(x)))
#define HIBYTE(x)		((BYTE)((UWORD)(x)>>8))
#define LOWORD(x)		((UNSIGNED short)((DWORD)(x)))
#define HIWORD(x)		((UWORD)((DWORD)(x)>>16))

#endif /* _COMPILER_H */

