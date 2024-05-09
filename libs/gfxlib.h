#ifndef _GFXLIB_H
#define _GFXLIB_H

#include "memlib.h"

#define CONTEXT		PVOID
#define FRAME		LPVOID
#define FONT		LPVOID
#define CYCLE_REF	DWORD

typedef CONTEXT		*PCONTEXT;
typedef CONTEXT		near *NPCONTEXT;
typedef CONTEXT		far *LPCONTEXT;

typedef FRAME		*PFRAME;
typedef FRAME		near *NPFRAME;
typedef FRAME		far *LPFRAME;

typedef FONT		*PFONT;
typedef FONT		near *NPFONT;
typedef FONT		far *LPFONT;

typedef UWORD		TIME_VALUE;
typedef TIME_VALUE	*PTIME_VALUE;
typedef TIME_VALUE	near *NPTIME_VALUE;
typedef TIME_VALUE	far *LPTIME_VALUE;

#define TIME_SHIFT	8
#define MAX_TIME_VALUE	((1 << TIME_SHIFT) + 1)

typedef SWORD		COORD;
typedef COORD		*PCOORD;
typedef COORD		near *NPCOORD;
typedef COORD		far *LPCOORD;

typedef UWORD		COLOR;
typedef COLOR		*PCOLOR;
typedef COLOR		near *NPCOLOR;
typedef COLOR		far *LPCOLOR;

#define BUILD_COLOR(c32k,c256)	\
    (COLOR)(((DWORD)(c32k)<<8)|(BYTE)(c256))
#define COLOR_32k(c)		(UWORD)((COLOR)(c)>>8)
#define COLOR_256(c)		LOBYTE((COLOR)c)
#define MAKE_RGB15(r,g,b)	(UWORD)(((r)<<10)|((g)<<5)|(b))

typedef BYTE		CREATE_FLAGS;
#define WANT_MASK	(CREATE_FLAGS)(1 << 0)
#define WANT_PIXMAP	(CREATE_FLAGS)(1 << 1)
#define ENABLE_PAGING	(CREATE_FLAGS)(1 << 2)

typedef enum
{
    MAP_ISOTROPIC,		/* == X/Y scale is the same */
    MAP_ANISOTROPIC,		/* == X/Y scale different (e.g. aspect ratio) */
    MAP_NOXFORM		/* == X/Y scale is ignored */
} MAP_TYPE;

typedef enum
{
    DEST_MASK = MAKE_WORD (0, 0),
    DEST_PIXMAP = MAKE_WORD (0, 1)
} DRAW_DESTINATION;

typedef enum
{
    DRAW_SUBTRACTIVE = 0,	/* implied only to active page */
    DRAW_ADDITIVE,		/* implied only to active page */
    DRAW_REPLACE
} DRAW_MODE;

typedef UWORD		DRAW_STATE;

typedef struct extent
{
    COORD	width, height;
} EXTENT;
typedef EXTENT		*PEXTENT;
typedef EXTENT		near *NPEXTENT;
typedef EXTENT		far *LPEXTENT;

typedef struct point
{
    COORD	x, y;
} POINT;
typedef POINT		*PPOINT;
typedef POINT		near *NPPOINT;
typedef POINT		far *LPPOINT;

typedef struct stamp
{
    POINT	origin;
    FRAME	frame;
} STAMP;
typedef STAMP		*PSTAMP;
typedef STAMP		near *NPSTAMP;
typedef STAMP		far *LPSTAMP;

typedef struct rect
{
    POINT	corner;
    EXTENT	extent;
} RECT;
typedef RECT		*PRECT;
typedef RECT		near *NPRECT;
typedef RECT		far *LPRECT;

typedef struct line
{
    POINT	first, second;
} LINE;
typedef LINE		*PLINE;
typedef LINE		near *NPLINE;
typedef LINE		far *LPLINE;

typedef enum
{
    ALIGN_LEFT,
    ALIGN_CENTER,
    ALIGN_RIGHT
} TEXT_ALIGN;

typedef struct text
{
    POINT	baseline;
    LPBYTE	lpStr;
    TEXT_ALIGN	align;
    COUNT	CharCount;
} TEXT;
typedef TEXT		*PTEXT;
typedef TEXT		near *NPTEXT;
typedef TEXT		far *LPTEXT;

typedef MEM_HANDLE	POLYREF;

typedef LPVOID		POLYGON;
typedef POLYGON		*PPOLYGON;
typedef POLYGON		near *NPPOLYGON;
typedef POLYGON		far *LPPOLYGON;

#include "strlib.h"

typedef STRING_TABLE	COLORMAP_REF;
typedef STRING		COLORMAP;
typedef STRINGPTR	COLORMAPPTR;

typedef struct stamp_cmap
{
    POINT	origin;
    FRAME	frame;
    COLORMAPPTR	CMap;
} STAMP_CMAP;
typedef STAMP_CMAP	*PSTAMP_CMAP;
typedef STAMP_CMAP	near *NPSTAMP_CMAP;
typedef STAMP_CMAP	far *LPSTAMP_CMAP;

typedef struct composite
{
    POINT	origin;
    LPFRAME	FrameList;
    COUNT	NumFrames;
} COMPOSITE;
typedef COMPOSITE	*PCOMPOSITE;
typedef COMPOSITE	near *NPCOMPOSITE;
typedef COMPOSITE	far *LPCOMPOSITE;

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
   POINT	Point;
   STAMP	Stamp;
   LINE		Line;
   TEXT		Text;
   STAMP_CMAP	StampCMap;
   RECT		Rect;
   POLYGON	Polygon;
   COMPOSITE	Composite;
} PRIM_DESC;
typedef PRIM_DESC	*PPRIM_DESC;
typedef PRIM_DESC	near *NPPRIM_DESC;
typedef PRIM_DESC	far *LPPRIM_DESC;

typedef DWORD	PRIM_LINKS;

typedef struct 
{
    PRIM_LINKS	Links;
    DWORD	TypeAndColor;
    PRIM_DESC	Object;
} PRIMITIVE;
typedef PRIMITIVE	*PPRIMITIVE;
typedef PRIMITIVE	near *NPPRIMITIVE;
typedef PRIMITIVE	far *LPPRIMITIVE;

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

typedef BYTE		BATCH_FLAGS;

#define BATCH_BUILD_PAGE		(BATCH_FLAGS)(1 << 0)
#define BATCH_SINGLE			(BATCH_FLAGS)(1 << 1)

typedef struct
{
    TIME_VALUE	last_time_val;
    POINT	EndPoint;
    STAMP	IntersectStamp;
} INTERSECT_CONTROL;
typedef INTERSECT_CONTROL		*PINTERSECT_CONTROL;
typedef INTERSECT_CONTROL		near *NPINTERSECT_CONTROL;
typedef INTERSECT_CONTROL		far *LPINTERSECT_CONTROL;

typedef MEM_HANDLE	CONTEXT_REF;
typedef CONTEXT_REF	*PCONTEXT_REF;
typedef CONTEXT_REF	near *NPCONTEXT_REF;
typedef CONTEXT_REF	far *LPCONTEXT_REF;

typedef DWORD		DRAWABLE;
typedef DRAWABLE	*PDRAWABLE;
typedef DRAWABLE	near *NPDRAWABLE;
typedef DRAWABLE	far *LPDRAWABLE;

#define BUILD_DRAWABLE(h,i)		((DRAWABLE)MAKE_DWORD(h,i))

typedef MEM_HANDLE	FONT_REF;
typedef FONT_REF	*PFONT_REF;
typedef FONT_REF	near *NPFONT_REF;
typedef FONT_REF	far *LPFONT_REF;

typedef BYTE	INTERSECT_CODE;

#define INTERSECT_LEFT		(INTERSECT_CODE)(1 << 0)
#define INTERSECT_TOP		(INTERSECT_CODE)(1 << 1)
#define INTERSECT_RIGHT		(INTERSECT_CODE)(1 << 2)
#define INTERSECT_BOTTOM	(INTERSECT_CODE)(1 << 3)
#define INTERSECT_NOCLIP	(INTERSECT_CODE)(1 << 7)
#define INTERSECT_ALL_SIDES	(INTERSECT_CODE)(INTERSECT_LEFT | \
				 INTERSECT_TOP | \
				 INTERSECT_RIGHT | \
				 INTERSECT_BOTTOM)

typedef DWORD		HOT_SPOT;
#define MAKE_HOT_SPOT	MAKE_DWORD
#define GET_HOT_X(h)	((short)LOWORD(h))
#define GET_HOT_Y(h)	((short)HIWORD(h))

PROC_GLOBAL(
INTERSECT_CODE BoxIntersect, (pr1, pr2, pinter),
    ARG		(PRECT	pr1)
    ARG		(PRECT	pr2)
    ARG_END	(PRECT	printer)
);
PROC_GLOBAL(
void BoxUnion, (pr1, pr2, punion),
    ARG		(LPRECT	pr1)
    ARG		(LPRECT	pr2)
    ARG_END	(LPRECT	punion)
);

typedef PROC_PARAMETER(
void (*BG_FUNC), (pClipRect),
    ARG_END	(PRECT		pClipRect)
);

#endif /* _GFXLIB_H */

#ifndef _GFX_PROTOS
#define _GFX_PROTOS

PROC_GLOBAL(
BOOLEAN InitGraphics, (argc, argv, KbytesRequired),
    ARG		(int	argc)
    ARG		(char	*argv[])
    ARG_END	(COUNT	KbytesRequired)
);
PROC_GLOBAL(
void UninitGraphics, (),
    ARG_VOID
);

PROC_GLOBAL(
CONTEXT SetContext, (Context),
    ARG_END	(CONTEXT	Context)
);
PROC_GLOBAL(
CONTEXT CaptureContext, (ContextRef),
    ARG_END	(CONTEXT_REF	ContextRef)
);
PROC_GLOBAL(
CONTEXT_REF ReleaseContext, (Context),
    ARG_END	(CONTEXT	Context)
);
PROC_GLOBAL(
MAP_TYPE SetContextMapType, (MapType),
    ARG_END	(MAP_TYPE	MapType)
);
PROC_GLOBAL(
BOOLEAN SetContextWinOrigin, (lpOrg),
    ARG_END	(LPPOINT	lpOrg)
);
PROC_GLOBAL(
BOOLEAN SetContextViewExtents, (lpExtent),
    ARG_END	(LPEXTENT	lpExtent)
);
PROC_GLOBAL(
BOOLEAN SetContextWinExtents, (lpExtent),
    ARG_END	(LPEXTENT	lpExtent)
);
PROC_GLOBAL(
BOOLEAN GetContextWinOrigin, (lpOrg),
    ARG_END	(LPPOINT	lpOrg)
);
PROC_GLOBAL(
BOOLEAN GetContextViewExtents, (lpExtent),
    ARG_END	(LPEXTENT	lpExtent)
);
PROC_GLOBAL(
BOOLEAN GetContextWinExtents, (lpExtent),
    ARG_END	(LPEXTENT	lpExtent)
);
PROC_GLOBAL(
BOOLEAN LOGtoDEV, (lpSrcPt, lpDstPt, NumPoints),
    ARG		(LPPOINT	lpSrcPt)
    ARG		(LPPOINT	lpDstPt)
    ARG_END	(COUNT		NumPoints)
);
PROC_GLOBAL(
BOOLEAN DEVtoLOG, (lpSrcPt, lpDstPt, NumPoints),
    ARG		(LPPOINT	lpSrcPt)
    ARG		(LPPOINT	lpDstPt)
    ARG_END	(COUNT		NumPoints)
);
PROC_GLOBAL(
DRAW_STATE GetContextDrawState, (),
    ARG_VOID
);
PROC_GLOBAL(
DRAW_STATE SetContextDrawState, (DrawState),
    ARG_END	(DRAW_STATE	DrawState)
);
PROC_GLOBAL(
COLOR SetContextForeGroundColor, (Color),
    ARG_END	(COLOR	Color)
);
PROC_GLOBAL(
COLOR SetContextBackGroundColor, (Color),
    ARG_END	(COLOR	Color)
);
PROC_GLOBAL(
FRAME SetContextFGFrame, (Frame),
    ARG_END	(FRAME	Frame)
);
PROC_GLOBAL(
FRAME SetContextBGFrame, (Frame),
    ARG_END	(FRAME	Frame)
);
PROC_GLOBAL(
BG_FUNC SetContextBGFunc, (BGFunc),
    ARG_END	(BG_FUNC	BGFunc)
);
PROC_GLOBAL(
BOOLEAN SetContextClipping, (ClipStatus),
    ARG_END	(BOOLEAN	ClipStatus)
);
PROC_GLOBAL(
BOOLEAN SetContextClipRect, (lpRect),
    ARG_END	(LPRECT	lpRect)
);
PROC_GLOBAL(
BOOLEAN GetContextClipRect, (lpRect),
    ARG_END	(LPRECT	lpRect)
);
PROC_GLOBAL(
TIME_VALUE DrawablesIntersect, (pControl0, pControl1, max_time_val),
    ARG		(PINTERSECT_CONTROL	pControl0)
    ARG		(PINTERSECT_CONTROL	pControl1)
    ARG_END	(TIME_VALUE		max_time_val)
);
PROC_GLOBAL(
void DrawStamp, (lpStamp),
    ARG_END	(LPSTAMP	lpStamp)
);
PROC_GLOBAL(
void DrawFilledStamp, (lpStamp),
    ARG_END	(LPSTAMP	lpStamp)
);
PROC_GLOBAL(
void DrawStampCMap, (lpStampCmap),
    ARG_END	(LPSTAMP_CMAP	lpStampCmap)
);
PROC_GLOBAL(
void DrawPoint, (lpPoint),
    ARG_END	(LPPOINT	lpPoint)
);
PROC_GLOBAL(
void DrawRectangle, (lpRect),
    ARG_END	(LPRECT		lpRect)
);
PROC_GLOBAL(
void DrawFilledRectangle, (lpRect),
    ARG_END	(LPRECT		lpRect)
);
PROC_GLOBAL(
void DrawLine, (lpLine),
    ARG_END	(LPLINE		lpLine)
);
PROC_GLOBAL(
void DrawText, (lpText),
    ARG_END	(LPTEXT		lpText)
);
PROC_GLOBAL(
void DrawPolygon, (Polygon),
    ARG_END	(POLYGON	Polygon)
);
PROC_GLOBAL(
void DrawFilledPolygon, (Polygon),
    ARG_END	(POLYGON	Polygon)
);
PROC_GLOBAL(
void DrawBatch, (lpBasePrim, PrimLinks, BatchFlags),
    ARG		(LPPRIMITIVE	lpBasePrim)
    ARG		(PRIM_LINKS	PrimLinks)
    ARG_END	(BATCH_FLAGS	BatchFlags)
);
PROC_GLOBAL(
void BatchGraphics, (),
    ARG_VOID
);
PROC_GLOBAL(
void UnbatchGraphics, (),
    ARG_VOID
);
PROC_GLOBAL(
void FlushGraphics, (WaitFlush),
    ARG_END	(BOOLEAN	WaitFlush)
);
PROC_GLOBAL(
void ClearDrawable, (),
    ARG_VOID
);
PROC_GLOBAL(
CONTEXT_REF CreateContext, (),
    ARG_VOID
);
PROC_GLOBAL(
BOOLEAN DestroyContext, (ContextRef),
    ARG_END	(CONTEXT_REF	ContextRef)
);
PROC_GLOBAL(
DRAWABLE CreateDisplay, (CreateFlags, pwidth, pheight),
    ARG		(CREATE_FLAGS	CreateFlags)
    ARG		(PSIZE		pwidth)
    ARG_END	(PSIZE		pheight)
);
PROC_GLOBAL(
DRAWABLE CreateDrawable, (CreateFlags, width, height, num_frames),
    ARG		(CREATE_FLAGS	CreateFlags)
    ARG		(SIZE		width)
    ARG		(SIZE		height)
    ARG_END	(COUNT		num_frames)
);
PROC_GLOBAL(
DRAWABLE CopyDrawable, (Drawable),
    ARG_END	(DRAWABLE	Drawable)
);
PROC_GLOBAL(
BOOLEAN DestroyDrawable, (Drawable),
    ARG_END	(DRAWABLE	Drawable)
);
PROC_GLOBAL(
BOOLEAN GetFrameRect, (Frame, pRect),
    ARG		(FRAME		Frame)
    ARG_END	(PRECT		pRect)
);

PROC_GLOBAL(
HOT_SPOT SetFrameHot, (Frame, HotSpot),
    ARG		(FRAME		Frame)
    ARG_END	(HOT_SPOT	HotSpot)
);
PROC_GLOBAL(
HOT_SPOT GetFrameHot, (Frame),
    ARG_END	(FRAME		Frame)
);
PROC_GLOBAL(
BOOLEAN InstallGraphicResType, (gfx_type),
    ARG_END	(COUNT		gfx_type)
);
PROC_GLOBAL(
DWORD LoadGraphicFile, (pStr),
    ARG_END	(PVOID	pStr)
);
PROC_GLOBAL(
DWORD LoadGraphicInstance, (res),
    ARG_END	(DWORD	res)
);
PROC_GLOBAL(
DWORD LoadGraphic, (res),
    ARG_END	(DWORD	res)
);
PROC_GLOBAL(
DRAWABLE LoadDisplayPixmap, (area, frame),
    ARG		(LPRECT		area)
    ARG_END	(FRAME		frame)
);
PROC_GLOBAL(
FONT SetContextFont, (Font),
    ARG_END	(FONT		Font)
);
PROC_GLOBAL(
BOOLEAN DestroyFont, (FontRef),
    ARG_END	(FONT_REF	FontRef)
);
PROC_GLOBAL(
FONT CaptureFont, (FontRef),
    ARG_END	(FONT_REF	FontRef)
);
PROC_GLOBAL(
FONT_REF ReleaseFont, (Font),
    ARG_END	(FONT		Font)
);
PROC_GLOBAL(
BOOLEAN TextRect, (lpText, pRect, pdelta),
    ARG		(LPTEXT		lpText)
    ARG		(PRECT		pRect)
    ARG_END	(PBYTE		pdelta)
);
PROC_GLOBAL(
BOOLEAN GetContextFontExtents, (pheight, pmaxascent, pmaxdescent),
    ARG		(PSIZE		pheight)
    ARG		(PSIZE		pmaxascent)
    ARG_END	(PSIZE		pmaxdescent)
);
PROC_GLOBAL(
COUNT GetFrameCount, (Frame),
    ARG_END	(FRAME		Frame)
);
PROC_GLOBAL(
COUNT GetFrameIndex, (Frame),
    ARG_END	(FRAME		Frame)
);
PROC_GLOBAL(
FRAME SetAbsFrameIndex, (Frame, FrameIndex),
    ARG		(FRAME		Frame)
    ARG_END	(COUNT		FrameIndex)
);
PROC_GLOBAL(
FRAME SetRelFrameIndex, (Frame, FrameOffs),
    ARG		(FRAME		Frame)
    ARG_END	(SIZE		FrameOffs)
);
PROC_GLOBAL(
FRAME SetEquFrameIndex, (DstFrame, SrcFrame),
    ARG		(FRAME		DstFrame)
    ARG_END	(FRAME		SrcFrame)
);
PROC_GLOBAL(
FRAME IncFrameIndex, (Frame),
    ARG_END	(FRAME		Frame)
);
PROC_GLOBAL(
FRAME DecFrameIndex, (Frame),
    ARG_END	(FRAME		Frame)
);
PROC_GLOBAL(
void XFlipFrame, (Frame),
    ARG_END	(FRAME		Frame)
);

PROC_GLOBAL(
FRAME CaptureDrawable, (Drawable),
    ARG_END	(DRAWABLE	Drawable)
);
PROC_GLOBAL(
DRAWABLE ReleaseDrawable, (Frame),
    ARG_END	(FRAME		Frame)
);
PROC_GLOBAL(
MEM_HANDLE GetFrameHandle, (Frame),
    ARG_END	(FRAME		Frame)
);

PROC_GLOBAL(
POLYREF CreatePolygon, (PtCount),
    ARG_END	(COUNT		PtCount)
);
PROC_GLOBAL(
POLYGON CapturePolygon, (PolyRef),
    ARG_END	(POLYREF	PolyRef)
);
PROC_GLOBAL(
BOOLEAN SetPolygonPoint, (Polygon, PtIndex, x, y),
    ARG		(POLYGON	Polygon)
    ARG		(COUNT		PtIndex)
    ARG		(COORD		x)
    ARG_END	(COORD		y)
);
PROC_GLOBAL(
BOOLEAN GetPolygonPoint, (Polygon, PtIndex, px, py),
    ARG		(POLYGON	Polygon)
    ARG		(COUNT		PtIndex)
    ARG		(PCOORD		px)
    ARG_END	(PCOORD		py)
);
PROC_GLOBAL(
POLYREF ReleasePolygon, (Polygon),
    ARG_END	(POLYGON	Polygon)
);
PROC_GLOBAL(
BOOLEAN DestroyPolygon, (PolyRef),
    ARG_END	(POLYREF	PolyRef)
);

PROC_GLOBAL(
DRAWABLE CreateOverlayWindow, (CreateFlags, Display, height),
    ARG		(CREATE_FLAGS	CreateFlags)
    ARG		(FRAME		Display)
    ARG_END	(SIZE		height)
);
PROC_GLOBAL(
void ShowOverlayWin, (WinFrame, TimeInterval),
    ARG		(FRAME	WinFrame)
    ARG_END	(COUNT	TimeInterval)
);
PROC_GLOBAL(
void HideOverlayWin, (WinFrame, TimeInterval),
    ARG		(FRAME	WinFrame)
    ARG_END	(COUNT	TimeInterval)
);
PROC_GLOBAL(
BOOLEAN DestroyOverlayWindow, (Display, Drawable),
    ARG		(FRAME		Display)
    ARG_END	(DRAWABLE	Drawable)
);
PROC_GLOBAL(
void ScrollDisplay, (Display, x, y),
    ARG		(FRAME	Display)
    ARG		(COORD	x)
    ARG_END	(COORD	y)
);
PROC_GLOBAL(
void FindDisplayPage, (Display, x, y),
    ARG		(FRAME	Display)
    ARG		(COORD	x)
    ARG_END	(COORD	y)
);

PROC_GLOBAL(
BOOLEAN ReadColorMap, (ColorMapPtr),
    ARG_END	(COLORMAPPTR	ColorMapPtr)
);
PROC_GLOBAL(
BOOLEAN SetColorMap, (ColorMapPtr),
    ARG_END	(COLORMAPPTR	ColorMapPtr)
);
PROC_GLOBAL(
BOOLEAN BatchColorMap, (ColorMapPtr),
    ARG_END	(COLORMAPPTR	ColorMapPtr)
);
PROC_GLOBAL(
CYCLE_REF CycleColorMap, (ColorMapPtr, Cycles, TimeInterval),
    ARG		(COLORMAPPTR	ColorMapPtr)
    ARG		(COUNT		Cycles)
    ARG_END	(SIZE		TimeInterval)
);
PROC_GLOBAL(
void StopCycleColorMap, (CycleRef),
    ARG_END	(CYCLE_REF	CycleRef)
);
PROC_GLOBAL(
DWORD XFormColorMap, (ColorMapPtr, TimeInterval),
    ARG		(COLORMAPPTR	ColorMapPtr)
    ARG_END	(SIZE		TimeInterval)
);
PROC_GLOBAL(
void FlushColorXForms, (),
    ARG_VOID
);
#define InitColorMapResources	InitStringTableResources
#define LoadColorMapFile	LoadStringTableFile
#define LoadColorMapInstance	LoadStringTableInstance
#define CaptureColorMap		CaptureStringTable
#define ReleaseColorMap		ReleaseStringTable
#define DestroyColorMap		DestroyStringTable
#define GetColorMapRef		GetStringTable
#define GetColorMapCount	GetStringTableCount
#define GetColorMapIndex	GetStringTableIndex
#define SetAbsColorMapIndex	SetAbsStringTableIndex
#define SetRelColorMapIndex	SetRelStringTableIndex
#define GetColorMapLength	GetStringLength
#define GetColorMapAddress	GetStringAddress
#define GetColorMapContents	GetStringContents

#endif /* _GFX_PROTOS */

