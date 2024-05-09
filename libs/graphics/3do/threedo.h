#ifndef _THREEDO_H
#define _THREEDO_H

typedef signed char	SBYTE;
typedef unsigned char	BYTE;
typedef signed int	SWORD;
typedef unsigned int	UWORD;
typedef unsigned long	DWORD;

typedef UWORD		COUNT;
typedef SWORD		SIZE;
typedef SWORD		COORD;
typedef COORD		*PCOORD;
typedef UWORD		COLOR;
typedef COLOR		*PCOLOR;
typedef void		*FRAME;

#define MAKE_BYTE(lo,hi)	((BYTE)(((BYTE)(hi)<<(BYTE)4)|(BYTE)(lo)))
#define MAKE_WORD(lo,hi)	((UWORD)((BYTE)(hi)<<8)|(BYTE)(lo))
#define MAKE_DWORD(lo,hi)	(((DWORD)(hi)<<16)|(UWORD)(lo))
#define LONIBBLE(x)		((BYTE)((BYTE)(x)&(BYTE)0x0F))
#define HINIBBLE(x)		((BYTE)((BYTE)(x)>>(BYTE)4))
#define LOBYTE(x)		((BYTE)((UWORD)(x)))
#define HIBYTE(x)		((BYTE)((UWORD)(x)>>8))
#define LOWORD(x)		((unsigned short)((DWORD)(x)))
#define HIWORD(x)		((UWORD)((DWORD)(x)>>16))

#define BUILD_COLOR(c32k,c256)	\
    (COLOR)(((DWORD)(c32k)<<8)|(BYTE)(c256))
#define COLOR_32k(c)		(UWORD)((COLOR)(c)>>8)
#define COLOR_256(c)		LOBYTE((COLOR)c)
#define MAKE_RGB15(r,g,b)	(UWORD)(((r)<<10)|((g)<<5)|(b))

typedef DWORD		HOT_SPOT;
#define MAKE_HOT_SPOT	MAKE_DWORD
#define GET_HOT_X(h)	((short)LOWORD(h))
#define GET_HOT_Y(h)	((short)HIWORD(h))

typedef struct extent
{
    COORD	width, height;
} EXTENT;
typedef EXTENT		*PEXTENT;

typedef struct point
{
    COORD	x, y;
} POINT;
typedef POINT		*PPOINT;

typedef struct stamp
{
    POINT	origin;
    FRAME	frame;
} STAMP;
typedef STAMP		*PSTAMP;

typedef struct rect
{
    POINT	corner;
    EXTENT	extent;
} RECT;
typedef RECT		*PRECT;

typedef struct line
{
    POINT	first, second;
} LINE;
typedef LINE		*PLINE;

typedef enum
{
    ALIGN_LEFT,
    ALIGN_CENTER,
    ALIGN_RIGHT
} TEXT_ALIGN;

typedef struct text
{
    POINT	baseline;
    BYTE	*lpStr;
    TEXT_ALIGN	align;
    COUNT	CharCount;
} TEXT;
typedef TEXT		*PTEXT;

enum gfx_object
{
    POINT_PRIM = 0,
    STAMP_PRIM,
    STAMPFILL_PRIM,
    LINE_PRIM,
    TEXT_PRIM,
    STAMPCMAP_PRIM,
    RECT_PRIM,
    RECTFILL_PRIM,
    POLY_PRIM,
    POLYFILL_PRIM,
    COMPOSITE_PRIM,
    COMPOSITEFILL_PRIM,

    NUM_PRIMS
};
typedef BYTE		GRAPHICS_PRIM;

typedef union
{
   POINT		Point;
   STAMP		Stamp;
   LINE			Line;
   TEXT			Text;
   RECT			Rect;
} PRIM_DESC;
typedef PRIM_DESC	*PPRIM_DESC;

typedef DWORD	PRIM_LINKS;

typedef struct 
{
    PRIM_LINKS	Links;
    DWORD	TypeAndColor;
    PRIM_DESC	Object;
} PRIMITIVE;
typedef PRIMITIVE	*PPRIMITIVE;

#define END_OF_LIST	((COUNT)0xFFFF)

#define PRIM_COLOR_SHIFT	8
#define PRIM_COLOR_MASK		((DWORD)~0L << PRIM_COLOR_SHIFT)
#define PRIM_TYPE_MASK		(~PRIM_COLOR_MASK)

#define GetPredLink(l)		LOWORD(l)
#define GetSuccLink(l)		HIWORD(l)
#define MakeLinks		MAKE_DWORD
#define SetPrimLinks(pPrim,p,s)	((pPrim)->Links = MakeLinks (p, s))
#define GetPrimLinks(pPrim)	((pPrim)->Links)
#define SetPrimType(pPrim,t)	\
	((pPrim)->TypeAndColor = ((pPrim)->TypeAndColor & PRIM_COLOR_MASK) \
		| ((t) & PRIM_TYPE_MASK))
#define GetPrimType(pPrim)	\
	((GRAPHICS_PRIM)LOBYTE ((pPrim)->TypeAndColor))
#define SetPrimColor(pPrim,c)	\
	((pPrim)->TypeAndColor = ((pPrim)->TypeAndColor & PRIM_TYPE_MASK) \
		| ((c) << PRIM_COLOR_SHIFT))
#define GetPrimColor(pPrim)	\
	((COLOR)((pPrim)->TypeAndColor >> PRIM_COLOR_SHIFT))

#define FTYPE_SHIFT	12
#define FINDEX_MASK	((1 << FTYPE_SHIFT) - 1)
#define FTYPE_MASK	(0xFFFF & ~FINDEX_MASK)

#define ROM_DRAWABLE	(0 << FTYPE_SHIFT)
#define RAM_DRAWABLE	(1 << FTYPE_SHIFT)
#define SCREEN_DRAWABLE	(2 << FTYPE_SHIFT)
#define OVERLAY_DRAWABLE	(3 << FTYPE_SHIFT)

typedef UWORD	DRAWABLE_TYPE;

#define DATA_HARDWARE	(1 << 12)
#define DATA_COPY	(1 << 13)
#define DATA_PACKED	(1 << 14)
#define X_FLIP		(1 << 15)

#define BUILD_FRAME	(1 << 0)
#define FIND_PAGE	(1 << 1)
#define FIRST_BATCH	(1 << 2)
#define COLOR_CYCLE	(1 << 4)
#define CYCLE_PENDING	(1 << 5)
#define DISABLE_CYCLE	(1 << 6)

extern BYTE	_batch_flags;
extern BYTE	_batch_depth;

typedef struct
{
    DWORD	TypeIndexAndFlags;
    HOT_SPOT	HotSpot;
    DWORD	Bounds;
    long	DataOffs;
} FRAME_DESC;
typedef FRAME_DESC	*PFRAME_DESC;

typedef UWORD		MEM_HANDLE;

typedef struct
{
    MEM_HANDLE	hDrawable;

    UWORD	FlagsAndIndex;
    FRAME_DESC	Frame[1];
} DRAWABLE_DESC;
typedef DRAWABLE_DESC	*PDRAWABLE_DESC;

#define GetFrameHotX(f)			GET_HOT_X(((PFRAME_DESC)(f))->HotSpot)
#define GetFrameHotY(f)			GET_HOT_Y(((PFRAME_DESC)(f))->HotSpot)
#define GetFrameHotSpot(f)		(((PFRAME_DESC)(f))->HotSpot)
#define SetFrameHotSpot(f,h)		(((PFRAME_DESC)(f))->HotSpot=(h))
#define GetFrameWidth(f)		LOWORD (((PFRAME_DESC)(f))->Bounds)
#define GetFrameHeight(f)		HIWORD (((PFRAME_DESC)(f))->Bounds)
#define SetFrameBounds(f,w,h)		(((PFRAME_DESC)(f))->Bounds=MAKE_DWORD(w,h))
#define GetFrameFlags(f)		HIWORD(((PFRAME_DESC)(f))->TypeIndexAndFlags)
#define AddFrameFlags(f,v)		(((PFRAME_DESC)(f))->TypeIndexAndFlags|=((DWORD)(v)<<16))
#define SubFrameFlags(f,v)		(((PFRAME_DESC)(f))->TypeIndexAndFlags&=~((DWORD)(v)<<16))

#define GetDrawableHandle(Drawable)	((MEM_HANDLE)LOWORD (Drawable))
#define GetDrawableIndex(Drawable)	((COUNT)HIWORD (Drawable))
#define GetFrameParentDrawable(F)	(PDRAWABLE_DESC)((LPBYTE)((F) \
					-(INDEX_GET((F)->TypeIndexAndFlags)-1)) \
					-sizeof(DRAWABLE_DESC))

#define TYPE_GET(f)			((f) & FTYPE_MASK)
#define INDEX_GET(f)			((f) & FINDEX_MASK)
#define TYPE_SET(f,t)			((f)|=t)
#define INDEX_SET(f,i)			((f)|=i)

typedef void		*LPVOID;
#define FONT		LPVOID
typedef MEM_HANDLE	CONTEXT_REF;
typedef UWORD		DRAW_STATE;

typedef struct
{
    CONTEXT_REF		ContextRef;
    UWORD		Flags;

    DRAW_STATE		DrawState;
    COLOR		ForeGroundColor, BackGroundColor;
    FRAME		ForeGroundFrame, BackGroundFrame;
    void (**func_array) (PRECT pClipRect, PPRIMITIVE PrimPtr);
    FONT		Font;
    void (*BackGroundFunc) (PRECT pClipRect);

    RECT		ClipRect;
} CONTEXT_DESC;
typedef CONTEXT_DESC	*PCONTEXT_DESC;
#define CONTEXTPTR	PCONTEXT_DESC
extern CONTEXTPTR	_pCurContext;

extern STAMP		_save_stamp;
#define BGFrame		_save_stamp.frame
extern PFRAME_DESC	_CurFramePtr;

#define PACK_SHIFT	6
#define PACK_TYPE(c)	((BYTE)((c)>>PACK_SHIFT))
#define PACK_COUNT(c)	((BYTE)(((c)&((1<<PACK_SHIFT)-1))+1))

#define MEM_NULL	0L
#define NULL_HANDLE	(MEM_HANDLE)MEM_NULL

typedef unsigned long	MEM_SIZE;
typedef MEM_SIZE	*PMEM_SIZE;
typedef long		MEM_OFFS;

#define MEM_FAILURE	FALSE
#define MEM_SUCCESS	TRUE

enum
{
    SAME_MEM_USAGE = 0,
    MEM_SIMPLE,
    MEM_COMPLEX
};
typedef BYTE			MEM_USAGE;

#define NO_MEM_USAGE		(MEM_USAGE)SAME_MEM_USAGE
#define DEFAULT_MEM_USAGE	(MEM_USAGE)MEM_SIMPLE

enum
{
    MEM_NO_ACCESS = 0,
    MEM_READ_ONLY,
    MEM_WRITE_ONLY,
    MEM_READ_WRITE
};
typedef BYTE			MEM_ACCESS_MODE;

enum
{
    MEM_SEEK_REL,
    MEM_SEEK_ABS
};
typedef BYTE			MEM_SEEK_MODE;

enum
{
    MEM_FORWARD,
    MEM_BACKWARD
};
typedef BYTE			MEM_DIRECTION;

typedef struct mem_size_request
{
    MEM_SIZE		min_size, size, max_size;
    MEM_DIRECTION	direction;
} MEM_SIZE_REQUEST;

typedef SBYTE			MEM_PRIORITY;
typedef UWORD			MEM_FLAGS;

#define DEFAULT_MEM_FLAGS	(MEM_FLAGS)0
#define SAME_MEM_FLAGS		(MEM_FLAGS)0
#define MEM_CONSTRAINED		(MEM_FLAGS)(1 << 2)
#define MEM_DISCARDABLE		(MEM_FLAGS)(1 << 3)
#define MEM_NODISCARD		(MEM_FLAGS)(1 << 4)
#define MEM_DISCARDED		(MEM_FLAGS)(1 << 5)
#define MEM_ACCESSED		(MEM_FLAGS)(1 << 6)
#define MEM_STATIC		(MEM_FLAGS)(1 << 7)
#define MEM_PRIMARY		(MEM_FLAGS)(1 << 8)
#define MEM_ZEROINIT		(MEM_FLAGS)(1 << 9)
#define MEM_GRAPHICS		(MEM_FLAGS)(1 << 10)
#define MEM_SOUND		(MEM_FLAGS)(1 << 11)

#define DEFAULT_MEM_PRIORITY	(MEM_PRIORITY)0
#define SAME_MEM_PRIORITY	(MEM_PRIORITY)0
#define HIGHEST_MEM_PRIORITY	(MEM_PRIORITY)1
#define LOWEST_MEM_PRIORITY	(MEM_PRIORITY)100

#define mem_lock	mem_simple_access
#define mem_unlock	mem_simple_unaccess

#endif /* _THREEDO_H */
