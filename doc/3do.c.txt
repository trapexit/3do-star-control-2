#include "Portfolio.h"
#include "Init3DO.h"
#include "Parse3DO.h"
#include "Utils3DO.h"
#include "filefunctions.h"
#include "ThreeDO.h"
#include "defs3DO.h"
#include "ourvdl.h"

#define DEBUG

#ifdef DEBUG
#define PRD(s)		kprintf s
#define SYSERR(e)	PrintfSysErr (e)
#else /* DEBUG */
#define PRD(s)
#define SYSERR(e)
#endif /* DEBUG */

#define NUMBER_OF_SCREENS	2

//#define SCREEN_WIDTH		320
//#define SCREEN_HEIGHT		240
#define VIEW_X_ADJUST		((SCREEN_WIDTH - VIEW_WIDTH) >> 1)		
#define VIEW_Y_ADJUST		((SCREEN_HEIGHT - VIEW_HEIGHT) >> 1)		
#define OVERLAY_WIN_ADJUST	VIEW_ADJUST
#define OVERLAY_WIN_HEIGHT	32

#define SHADOW_COLOR		(0x18c6) // Shadow color... sleazy

#define PIXC_DUP(v)		(((v) << PPMP_0_SHIFT) | ((v) << PPMP_1_SHIFT))
#define PIXC_UNCODED16		(PIXC_DUP (PPMPC_MF_8 | PPMPC_SF_8))
#define PIXC_CODED8		(PIXC_DUP (PPMPC_MS_PIN | PPMPC_SF_8))
#define PIXC_USEPLUT		(PIXC_DUP (PPMPC_MF_8 | PPMPC_SF_8))
#define PIXC_CODED8_TRANS	(PIXC_DUP (PPMPC_MS_PIN | PPMPC_SF_8 | PPMPC_2S_CFBD | PPMPC_2D_2))

#define SET_CEL_FLAGS(c,f)	((c)->ccb_Flags |= (f))
#define CLR_CEL_FLAGS(c,f)	((c)->ccb_Flags &= ~(f))

#define NUMBER_OF_VARPLUTS	256
//#define NUMBER_OF_SOLIDPLUTS	12

// Obviously, this uses the low 5 bits to look up a color,
// then scales it up by the high 3 bits (for varying luminances)
// NOPE!  Don't need to scale up, right?
#define LOOKUP_PIXEL(p) \
	(((varPLUTs[0][((p) & 0x1f) >> 1] >> ((~(p) & 1) << 4)) & 0xFFFF) | 0x8000)
//#define LOOKUP_PIXEL(p) \
//	(((((varPLUTs[0][(((p) & 0x1f) >> 1)] >> \
//	((~(p) & 1) << 4))) * (8 - ((p) >> 5))) & 0xFFFF) | 0x8000)
//#define LOOKUP_PIXEL(p) \
//	((varPLUTs[0][(p) & 0x1f] * (8 - ((p) >> 5))) | 0x8000)
	
#define NUMBER_OF_PLUTVALS	32
#define NUMBER_OF_PLUT_UINT32s	(NUMBER_OF_PLUTVALS >> 1)
#define NUMBER_OF_CLUTVALS	32

typedef struct small_ccb
{
    ulong ccb_Flags;

    struct small_ccb *ccb_NextPtr;
    CelData    *ccb_SourcePtr;
    void       *ccb_PLUTPtr;

    Coord ccb_XPos;
    Coord ccb_YPos;
    long  ccb_HDX;
    long  ccb_HDY;
    long  ccb_VDX;
    long  ccb_VDY;
    
    ulong ccb_PIXC;
    
    ulong ccb_PRE0;
    ulong ccb_PRE1;
} SMALL_CCB;

typedef struct solid_plut
{
    uint32	color;
    uint32	plutvals[NUMBER_OF_PLUT_UINT32s];
} SOLID_PLUT;

static uint32	varPLUTs[NUMBER_OF_VARPLUTS][NUMBER_OF_PLUT_UINT32s];
static uint32	(*vpp)[NUMBER_OF_PLUT_UINT32s] = &varPLUTs[0];

static SOLID_PLUT	solidPLUTs[] =
{
    { 0x18c6 },
    { 0x7fff },
    { 0x0000 },
    { 0x7c42 },
    { 0x13c4 },
    { 0x7480 },
    { 0x7fe0 },
    { 0x5294 },
    { 0x6b10 },
    { 0x7c00 },
    { 0x0ae2 },
    { 0x043f },
    { 0x8000 }
};

static ScreenContext	TheScreen;

static SMALL_CCB	cel_array[500];
static SMALL_CCB	*cur_ccb;

static Item		OverlayWinBitmapItems[2];
static Bitmap		*OverlayWinBitmaps[2];
int			cur_overlay_win_offs = 0;

#ifdef REL10
static Item	VRAMIOReq;
#endif

int		_new_org_x, _new_org_y;
static int	_old_org_x, _old_org_y;

#ifdef SEPARATE_FUNCTIONS
#define SCREEN_TO_BUFFER(buf,bm)	screen_to_buffer((buf),(bm))
#define BUFFER_TO_SCREEN(buf,bm,yoffs)	buffer_to_screen((buf),(bm),(yoffs))

static void
screen_to_buffer (void *srcptr, Item bitmap)
{
#define SCREEN_TO_BUFFER_OFFS	\
	(VIEW_X_ADJUST + \
	((VIEW_HEIGHT - (OVERLAY_WIN_HEIGHT) + \
	VIEW_Y_ADJUST) * ((SCREEN_WIDTH) >> 1)))
	
	static CCB	screen_to_buffer_ccb =
	{
	    CCB_SPABS |		//    ulong ccb_Flags;
		CCB_YOXY |
		CCB_ACW |
		CCB_ACCW |
		CCB_LDSIZE |
		CCB_CCBPRE |
		CCB_BGND |
		CCB_LDPRS |
		CCB_LDPPMP |
		CCB_ACE |
		CCB_PLUTPOS |
		CCB_LAST,
	
	    NULL, 		//    struct CCB *ccb_NextPtr;
	    NULL, 		//    CelData    *ccb_SourcePtr;
	    NULL, 		//    void       *ccb_PLUTPtr;
	
	    0, 			//    Coord ccb_XPos;
	    0, 			//    Coord ccb_YPos;
	    1 << 20, 		//    long  ccb_HDX;
	    0, 			//    long  ccb_HDY;
	    0, 			//    long  ccb_VDX;
	    1 << 16, 		//    long  ccb_VDY;
	    0, 			//    long  ccb_HDDX;
	    0, 			//    long  ccb_HDDY;
	
	    PIXC_UNCODED16, 	//    ulong ccb_PIXC;
	    	    
    	    PRE0_BGND |
	    PRE0_LINEAR |
    	    (((OVERLAY_WIN_HEIGHT >> 1) - PRE0_VCNT_PREFETCH) << PRE0_VCNT_SHIFT) |
	    (PRE0_BPP_16 << PRE0_BPP_SHIFT),

	    PRE1_LRFORM |
	    PRE1_TLLSB_PDC0 |
    	    ((VIEW_WIDTH - PRE1_TLHPCNT_PREFETCH) << PRE1_TLHPCNT_SHIFT) |
	    ((SCREEN_WIDTH - PRE1_WOFFSET_PREFETCH) << PRE1_WOFFSET10_SHIFT)    
	};
	
    screen_to_buffer_ccb.ccb_SourcePtr = (void *)((uint32 *)srcptr + SCREEN_TO_BUFFER_OFFS);
			
    DrawCels (bitmap, &screen_to_buffer_ccb);
}

static void
buffer_to_screen (void *srcptr, Item bitmap, int yoffs)
{
	static CCB	buffer_to_screen_ccb =
	{
	    CCB_SPABS |		//    ulong ccb_Flags;
		CCB_YOXY |
		CCB_ACW |
		CCB_ACCW |
		CCB_LDSIZE |
		CCB_CCBPRE |
		CCB_BGND |
		CCB_LDPRS |
		CCB_LDPPMP |
		CCB_ACE |
		CCB_PLUTPOS |
		CCB_LAST,
	
	    NULL, 		//    struct CCB *ccb_NextPtr;
	    NULL, 		//    CelData    *ccb_SourcePtr;
	    NULL, 		//    void       *ccb_PLUTPtr;
	
	    0, 			//    Coord ccb_XPos;
	    0, 			//    Coord ccb_YPos;
	    1 << 20, 		//    long  ccb_HDX;
	    0, 			//    long  ccb_HDY;
	    0, 			//    long  ccb_VDX;
	    1 << 16, 		//    long  ccb_VDY;
	    0, 			//    long  ccb_HDDX;
	    0, 			//    long  ccb_HDDY;
	
	    PIXC_UNCODED16, 	//    ulong ccb_PIXC;
	    	    
    	    PRE0_BGND |
	    PRE0_LINEAR |
    	    (((SCREEN_HEIGHT >> 1) - PRE0_VCNT_PREFETCH) << PRE0_VCNT_SHIFT) |
	    (PRE0_BPP_16 << PRE0_BPP_SHIFT),

	    PRE1_LRFORM |
	    PRE1_TLLSB_PDC0 |
    	    ((VIEW_WIDTH - PRE1_TLHPCNT_PREFETCH) << PRE1_TLHPCNT_SHIFT) |
	    ((SCREEN_WIDTH - PRE1_WOFFSET_PREFETCH) << PRE1_WOFFSET10_SHIFT)
	};
			
    buffer_to_screen_ccb.ccb_SourcePtr = srcptr;
    buffer_to_screen_ccb.ccb_YPos = (VIEW_HEIGHT - yoffs) << 16;
    buffer_to_screen_ccb.ccb_PRE0 =
    	    PRE0_BGND |
	    PRE0_LINEAR |
    	    (((yoffs >> 1) - PRE0_VCNT_PREFETCH) << PRE0_VCNT_SHIFT) |
	    (PRE0_BPP_16 << PRE0_BPP_SHIFT);
    
    DrawCels (bitmap, &buffer_to_screen_ccb);
}
#else /* SAME_FUNCTION */
#define SCREEN_TO_BUFFER(buf,bm) \
	render_screen_cel ( \
		0, \
		0, \
		VIEW_X_ADJUST, \
		VIEW_HEIGHT - OVERLAY_WIN_HEIGHT + VIEW_Y_ADJUST, \
		SCREEN_WIDTH, \
		VIEW_WIDTH, \
		OVERLAY_WIN_HEIGHT, \
		(buf), \
		(bm) \
		)
#define BUFFER_TO_SCREEN(buf,bm,yoffs) \
	render_screen_cel ( \
		0, \
		VIEW_HEIGHT - (yoffs), \
		0, \
		0, \
		SCREEN_WIDTH, \
		VIEW_WIDTH, \
		(yoffs), \
		(buf), \
		(bm) \
		)
#endif /* SEPARATE_FUNCTIONS */

static void
render_screen_cel (int dx, int dy, int sx, int sy, int sw, int width, int height, void *srcptr, Item bitmap)
{
	static CCB	screen_ccb =
	{
	    CCB_SPABS |		//    ulong ccb_Flags;
		CCB_YOXY |
		CCB_ACW |
		CCB_ACCW |
		CCB_LDSIZE |
		CCB_CCBPRE |
		CCB_BGND |
		CCB_LDPRS |
		CCB_LDPPMP |
		CCB_ACE |
		CCB_PLUTPOS |
		CCB_LAST,
	
	    NULL, 		//    struct CCB *ccb_NextPtr;
	    NULL, 		//    CelData    *ccb_SourcePtr;
	    NULL, 		//    void       *ccb_PLUTPtr;
	
	    0, 			//    Coord ccb_XPos;
	    0, 			//    Coord ccb_YPos;
	    1 << 20, 		//    long  ccb_HDX;
	    0, 			//    long  ccb_HDY;
	    0, 			//    long  ccb_VDX;
	    1 << 16, 		//    long  ccb_VDY;
	    0, 			//    long  ccb_HDDX;
	    0, 			//    long  ccb_HDDY;
	
	    PIXC_UNCODED16, 	//    ulong ccb_PIXC;
	    
	    PRE0_BGND |		//    ulong ccb_PRE0;
	    	    PRE0_LINEAR |
		    (PRE0_BPP_16 << PRE0_BPP_SHIFT),
	
	    PRE1_LRFORM |	//    ulong ccb_PRE1;
	    	    PRE1_TLLSB_PDC0
	};

    screen_ccb.ccb_SourcePtr = (void *)((uint32 *)srcptr + sx + (sy * (sw >> 1)));
	    
    screen_ccb.ccb_XPos = dx << 16;
    screen_ccb.ccb_YPos = dy << 16;

    screen_ccb.ccb_PRE0 =
    	    PRE0_BGND |
	    PRE0_LINEAR |
    	    (((height >> 1) - PRE0_VCNT_PREFETCH) << PRE0_VCNT_SHIFT) |
	    (PRE0_BPP_16 << PRE0_BPP_SHIFT);

    screen_ccb.ccb_PRE1 =
	    PRE1_LRFORM |
    	    ((width - PRE1_TLHPCNT_PREFETCH) << PRE1_TLHPCNT_SHIFT) |
	    ((sw - PRE1_WOFFSET_PREFETCH) << PRE1_WOFFSET10_SHIFT);
	    
    DrawCels (bitmap, &screen_ccb);
}

static void
memory_blt (PRECT pClipRect, PPRIMITIVE PrimPtr)
{
    int		w, h;
    int		type;
    COORD	dst_x, dst_y;
    SIZE	dst_incr;
    COLOR	color;
    int		dw, dh;
    COORD	src_x, src_y;
    int32	*pDst, *pD;
    PFRAME_DESC	SrcFramePtr;
    
    dw = GetFrameWidth (_CurFramePtr);
    dh = GetFrameHeight (_CurFramePtr);
    
    dst_x = pClipRect->corner.x;
    dst_y = pClipRect->corner.y;
	
    w = pClipRect->extent.width;
    h = pClipRect->extent.height;
	
    type = GetPrimType (PrimPtr);
    if (type == STAMP_PRIM || type == STAMPFILL_PRIM)
    {
	SrcFramePtr = (PFRAME_DESC)PrimPtr->Object.Stamp.frame;

	src_x = dst_x - _save_stamp.origin.x;
	src_y = dst_y - _save_stamp.origin.y;

	dst_x -= GetFrameHotX (_CurFramePtr);
	dst_y -= GetFrameHotY (_CurFramePtr);	
    }
    
    if (dst_x + w > dw)
    {
	if ((w = dw - dst_x) <= 0)
	    return;
    }
	
    if (dst_y + h > dh)
    {
	if ((h = dh - dst_y) <= 0)
	    return;
    }
	
    color = COLOR_32k (GetPrimColor (PrimPtr)) | 0x8000;
    color = MAKE_DWORD (color, color);
    pDst = (int32 *)((uchar *)_CurFramePtr + _CurFramePtr->DataOffs);
    dst_incr = ((dw << 1) + 3) >> 2;
    
    if (type != STAMP_PRIM && type != STAMPFILL_PRIM)
    {	
	pDst += (dst_y * dst_incr) + (dst_x >> 1);
	if (dst_x & 1)
	{
	    pD = pDst;
	    dst_y = h;
	    do
	    {
		*pD = (*pD & ~0xFFFF) | (color & 0xFFFF);
		pD += dst_incr;
	    } while (--dst_y);

	    ++dst_x;
	    --w;
	    ++pDst;
	}
	if (w & 1)
	{
	    pD = pDst + (w >> 1);
	    dst_y = h;
	    do
	    {
		*pD = (*pD & 0xFFFF) | (color & ~0xFFFF);
		pD += dst_incr;
	    } while (--dst_y);

	    --w;
	}

	if (w)
	{
	    w >>= 1;
	    dst_incr -= w;
	    do
	    {
		dst_x = w;
		do
		{
		    *pDst++ = color;
		} while (--dst_x);
		pDst += dst_incr;
	    } while (--h);
	}
    }
    else
    {
//	UWORD		x_flip;
//	DWORD		frame_flags;
	BYTE		*pSrc;
	
//	frame_flags = GetFrameFlags (SrcFramePtr);
//	x_flip = frame_flags & X_FLIP;
	
	pSrc = (BYTE *)SrcFramePtr + SrcFramePtr->DataOffs;
	pDst += (dst_y * dst_incr);
	
	if (src_x)
	    dst_x = -src_x;
	    
	while (src_y--)
	    pSrc += (MAKE_WORD (pSrc[1], pSrc[0]) + 2) << 2;
	
	do
	{
	    BYTE	code;
	    BYTE	*pS;
	    COORD	cur_x;

	    cur_x = dst_x;	    
	    pS = pSrc + 2;
	    pSrc += (MAKE_WORD (pSrc[1], pSrc[0]) + 2) << 2;
	    while (code = *pS++)
	    {
		BYTE	pack_type, num_pixels, np;
		COORD	next_x;
		
		pack_type = PACK_TYPE (code);
		num_pixels = PACK_COUNT (code);
		next_x = cur_x + num_pixels;
		
		if (next_x >= dw)
		{
    		    if (cur_x >= dw)
		    {
			switch (pack_type)
			{
			    case PACK_LITERAL:
				pS += num_pixels;
				break;
			    case PACK_PACKED:
				pS++;
				break;
			}
			
			cur_x = next_x;
			continue;
		    }
		    np = dw - cur_x;
		}
		else if (cur_x < 0)
		{
		    if (next_x < 0)
		    {
			switch (pack_type)
			{
			    case PACK_LITERAL:
				pS += num_pixels;
				break;
			    case PACK_PACKED:
				pS++;
				break;
			}
			
			cur_x = next_x;
			continue;
		    }
		    cur_x = 0;
		    np = next_x + 1;
		}
		else
		    np = num_pixels;

		switch (pack_type)
		{
		    case PACK_LITERAL:
			if (type != STAMP_PRIM)
			    pS += num_pixels - 1;
			else
			{
			    BYTE	*_pS;
			    
			    _pS = pS;
			    pS += num_pixels;
			    pD = pDst + (cur_x >> 1);
			    if (cur_x & 1)
			    {
				*pD = (*pD & ~0xFFFF) | (LOOKUP_PIXEL (*_pS));
				++_pS;
				++pD;

				++cur_x;
				--np;
			    }
			    if (np & 1)
			    {
				pD += (np >> 1);
				*pD = (*pD & 0xFFFF) | (LOOKUP_PIXEL (*(pS - 1)) << 16);
				pD = pDst + (cur_x >> 1);
			
				--np;
			    }

			    np >>= 1;
			    while (np--)
			    {
			    	int	lo, hi;
				
				hi = LOOKUP_PIXEL (*_pS);
				_pS++;
				lo = LOOKUP_PIXEL (*_pS);
				_pS++;
			        *pD++ = MAKE_DWORD (lo, hi);
			    }
			    break;
			}
		    case PACK_PACKED:
			if (type == STAMP_PRIM)
			{
			    color = LOOKUP_PIXEL (*pS);
			    color = MAKE_DWORD (color, color);
			}
			++pS;

			pD = pDst + (cur_x >> 1);
			if (cur_x & 1)
			{
			    *pD = (*pD & ~0xFFFF) | (color & 0xFFFF);
			    ++pD;

			    ++cur_x;
			    --np;
			}
			if (np & 1)
			{
			    pD += (np >> 1);
			    *pD = (*pD & 0xFFFF) | (color & ~0xFFFF);
			    pD = pDst + (cur_x >> 1);
			
			    --np;
			}

			np >>= 1;
			while (np--)
			    *pD++ = color;
			break;
		}
		
		cur_x = next_x;
	    }

	    pDst += dst_incr;
	} while (--h);
    }
}

static void
create_overlay_window (void)
{
    if (!OverlayWinBitmapItems[0])
    {
	TagArg	tags[10], *t;
	Item	ScreenGroup;
	int	h;
	Screen	*screen;
	Item	OverlayWinScreens[2];
	
	t = tags;
	t->ta_Tag = CSG_TAG_SCREENCOUNT;
	t->ta_Arg = (void *)2;
	t++;
	t->ta_Tag = CSG_TAG_SCREENHEIGHT;
	t->ta_Arg = (void *)OVERLAY_WIN_HEIGHT;
	t++;
	t->ta_Tag = CSG_TAG_DISPLAYHEIGHT;
	t->ta_Arg = (void *)OVERLAY_WIN_HEIGHT;
	t++;
	t->ta_Tag = CSG_TAG_BITMAPHEIGHT_ARRAY;
	h = OVERLAY_WIN_HEIGHT;
	t->ta_Arg = (void *)&h;
	t++;
	t->ta_Tag = CSG_TAG_DONE;

	if ((ScreenGroup = CreateScreenGroup (OverlayWinScreens, tags)) < 0)
	{
    	    PRD (("CreateScreenGroup failed\n"));
	    SYSERR (ScreenGroup);
	    return;
	}
	
	if (AddScreenGroup (ScreenGroup, 0) < 0)
	{
    	    PRD (("AddScreenGroup failed\n"));
	    return;
	}
	
	screen = (Screen *)LookupItem (OverlayWinScreens[0]);
	OverlayWinBitmapItems[0] = screen->scr_TempBitmap->bm.n_Item;
	OverlayWinBitmaps[0] = screen->scr_TempBitmap;
	SetCEControl (OverlayWinBitmapItems[0],
		B15POS_PDC | CFBDLSB_CFBD0 | PDCLSB_PDC0,
		B15POS_MASK | CFBDLSB_MASK | PDCLSB_MASK);
	if ((h = DeleteItem (screen->scr_VDLItem)) < 0)
	{
	    PRD (("couldn't delete OVW VDL 0\n"));
	    SYSERR (h);
	}
	screen = (Screen *)LookupItem (OverlayWinScreens[1]);
	OverlayWinBitmapItems[1] = screen->scr_TempBitmap->bm.n_Item;
	OverlayWinBitmaps[1] = screen->scr_TempBitmap;	
	SetCEControl (OverlayWinBitmapItems[1],
		B15POS_PDC | CFBDLSB_CFBD0 | PDCLSB_PDC0,
		B15POS_MASK | CFBDLSB_MASK | PDCLSB_MASK);
	if ((h = DeleteItem (screen->scr_VDLItem)) < 0)
	{
	    PRD (("couldn't delete OVW VDL 1\n"));
	    SYSERR (h);
	}
    }
}

#ifdef DEBUG
char	imagefile[] = { "horde.dat/horde.img" };
char	waitfile[] = { "horde.dat/PleaseWait.cel" };
#endif /* DEBUG */

void
_split_screen (int yoffs)
{
    int	i;

    if (yoffs == SCREEN_HEIGHT - 1) // this will bomb in BUFFER_TO_SCREEN
	return;
	
    yoffs = SCREEN_HEIGHT - yoffs;
    for (i = 0; i < NUMBER_OF_SCREENS; i++)
    {
	if (yoffs <= 0)
	{
	    // copy buffer to screen
	    BUFFER_TO_SCREEN (
		    OverlayWinBitmaps[1]->bm_Buffer,
		    TheScreen.sc_BitmapItems[i],
		    OVERLAY_WIN_HEIGHT
		    );
	}
	else
	{
	    if (yoffs != cur_overlay_win_offs)
	    {
		if (!cur_overlay_win_offs && i == TheScreen.sc_curScreen)
		{
		    // copy current screen to buffer
		    SCREEN_TO_BUFFER (
		    	    TheScreen.sc_Bitmaps[TheScreen.sc_curScreen]->bm_Buffer,
		    	    OverlayWinBitmapItems[1]
			    );
		}
		// copy buffer to screen
		BUFFER_TO_SCREEN (
			OverlayWinBitmaps[1]->bm_Buffer,
			TheScreen.sc_BitmapItems[i],
			OVERLAY_WIN_HEIGHT
			);
		// copy overlay window to screen
		BUFFER_TO_SCREEN (
			OverlayWinBitmaps[0]->bm_Buffer,
			TheScreen.sc_BitmapItems[i],
			yoffs
			);
	    }
	}
    }
    
    cur_overlay_win_offs = yoffs < 0 ? 0 : yoffs;
}

void
_threedo_load_pluts (ubyte *colors)
{
    memcpy ((ubyte *)varPLUTs, colors, sizeof (varPLUTs));
#ifdef PLUT_DEBUG
{
int	i, j;

for (i = 0; i < 4; i++)
{
    kprintf ("PLUT %d\n", i);
    for (j = 0; j < 16; j++)
    {
	kprintf ("     0x%04x 0x%04x\n",
		(varPLUTs[i][j] >> 16) & 0xFFFF, varPLUTs[i][j] & 0xFFFF);
    }
}
}
#endif /* PLUT_DEBUG */
}

void
_threedo_get_screens (Item *screens)
{
    int	i;
    
    for (i = 0; i < NUMBER_OF_SCREENS; i++)
        *screens++ = OpenItem (TheScreen.sc_Screens[i], 0);
}

void
_threedo_set_colors (ubyte *colors, uint32 indices, Item *screens)
{
    int		i, start, end;
    uint32	colorEntries[NUMBER_OF_CLUTVALS], *ce;
	
    if (!(_batch_flags & PLUTS_LOADED))
	return;
    
    start = (int)LOBYTE (indices);
    end = (int)HIBYTE (indices);

    ce = colorEntries;
    for (i = start; i <= end; i++)
    {
	ubyte	r, g, b;
	
	r = (*colors << 2) | (*colors >> 4);
	++colors;
	g = (*colors << 2) | (*colors >> 4);
	++colors;
	b = (*colors << 2) | (*colors >> 4);
	++colors;
	
	*ce++ = MakeCLUTColorEntry (i, r, g, b);
    }
	
    end -= start - 1;
    for (i = 0; i < NUMBER_OF_SCREENS; i++)
    {
#ifdef REL10
	SetScreenColors (screens[i], colorEntries, end);
#else
	SetScreenColors (TheScreen.sc_Screens[i], colorEntries, end);
#endif
    }
}

static void
BlackenScreen ()
{
    int		i;
    uint32	colorEntries[NUMBER_OF_CLUTVALS], *ce;
		
    ce = colorEntries;
    for (i = 0; i < NUMBER_OF_CLUTVALS; i++)
	*ce++ = MakeCLUTColorEntry (i, 0, 0, 0);
	
    for (i = 0; i < NUMBER_OF_SCREENS; i++)
    	SetScreenColors (TheScreen.sc_Screens[i], colorEntries, NUMBER_OF_CLUTVALS);
	
    SetScreenColor (0, MakeCLUTColorEntry (NUMBER_OF_CLUTVALS, 0, 0, 0));
}

static void
LoadAndDisplayCel (char *file)
{
    CCB	*ccb;
    int	i;
#ifdef REL10

    if (ccb = LoadCel (file, MEMTYPE_CEL))
    {
	ccb->ccb_YPos = 60 << 16;
	for (i = 0; i < NUMBER_OF_SCREENS; i++)
	    DrawCels (TheScreen.sc_BitmapItems[i], ccb);
	UnloadCel (ccb);
    }
#else
    ulong	size;
    long	*cel_data;

    size = GetFileSize (file);
    if (cel_data = (long *)ALLOCMEM (size, MEMTYPE_CEL))
    {
	if (ccb = LoadCel (file, &cel_data))
	{
	    ccb->ccb_YPos = 60 << 16;
	    for (i = 0; i < NUMBER_OF_SCREENS; i++)
		DrawCels (TheScreen.sc_BitmapItems[i], ccb);
	}
	FREEMEM (cel_data, size);
    }
#endif
}

int
Init3DO (void)
{
    TheScreen.sc_nScreens = NUMBER_OF_SCREENS;

    if (OpenGraphics (&TheScreen, NUMBER_OF_SCREENS) && OpenSPORT ())
    {
	int	i;
	CCB	ccb;
	ubyte	*image_data;
	SOLID_PLUT	*pPlut;

	create_overlay_window ();
	
	for (cur_ccb = &cel_array[0], i = sizeof (cel_array) / sizeof (cel_array[0]);
		i > 0; i--, cur_ccb++)
	{
	    cur_ccb->ccb_Flags =
		    CCB_SPABS |
		    CCB_NPABS |
		    CCB_PPABS |
		    CCB_YOXY |
		    CCB_ACW |
		    CCB_ACCW |
		    CCB_LDSIZE |
		    CCB_CCBPRE |
		    CCB_LDPPMP |
		    CCB_ACE |
		    CCB_PLUTPOS |
		    CCB_BGND;
	    cur_ccb->ccb_NextPtr = cur_ccb + 1;
	}
	cur_ccb = &cel_array[0];
	    
	pPlut = solidPLUTs;
	do
	{
	    uint32	*pP;
	    uint32	color;
	    
	    color = pPlut->color | (pPlut->color << 16) | 0x80008000;
	    i = NUMBER_OF_PLUT_UINT32s;
	    pP = pPlut->plutvals;
	    do
	    {
	        *pP++ = color;
	    } while (--i);
	    pPlut++;
	} while (pPlut->color != 0x8000);
	
	ChangeDirectory ("/remote");

	TheScreen.sc_curScreen = 0;
	
	ccb.ccb_Flags = 
		CCB_SPABS |
	 	CCB_NPABS |
		CCB_PPABS |
		CCB_YOXY |
		CCB_ACW |
		CCB_ACCW |
		CCB_LDSIZE |
		CCB_CCBPRE |
		CCB_LDPPMP |
		CCB_LAST |
		CCB_LDPRS |
		CCB_ACE |
		CCB_PLUTPOS |
		CCB_BGND;

	ccb.ccb_HDY = 0;
	ccb.ccb_VDX = 0;
	
#ifdef NOT_HERE 
	ccb.ccb_HDX = SCREEN_WIDTH << 20;
	ccb.ccb_VDY = SCREEN_HEIGHT << 16;
	
	ccb.ccb_XPos = 0;
	ccb.ccb_YPos = 0;
#endif /* NOT_HERE */

	ccb.ccb_HDDX = 0;
	ccb.ccb_HDDY = 0;
	
	ccb.ccb_PRE0 =
		(PRE0_BPP_16 << PRE0_BPP_SHIFT) |
		PRE0_BGND |
		PRE0_LINEAR;
	ccb.ccb_PRE1 = PRE1_TLLSB_PDC0;
	ccb.ccb_PIXC = PIXC_UNCODED16;
	
	ccb.ccb_PLUTPtr = (void *)0;
	ccb.ccb_SourcePtr = (void *)&ccb.ccb_PLUTPtr;

	image_data = LoadImage (imagefile, NULL, NULL, &TheScreen);
	
#ifdef REL10
	VRAMIOReq = GetVRAMIOReq ();
#endif

	for (i = 0; i < NUMBER_OF_SCREENS; i++)
	{
	    VDL_REC	vdl_rec;
	    Item	VDLItem;
		
	    SimpleVDL (&vdl_rec, TheScreen.sc_Bitmaps[i]);
	    
	    if ((VDLItem = SubmitVDL (
		    (VDLEntry *)&vdl_rec,
		    sizeof (VDL_REC) >> 2,
		    VDLTYPE_SIMPLE
		    )) < 0) 
	    {
		PRD (("%d: SubmitVDL() failed -- ", i));
		SYSERR (VDLItem);
	    }
	    else
	    {
		int32	errorCode;
		Item	old_vdl;
		Screen	*screen;
		
		screen = (Screen *)LookupItem (TheScreen.sc_Screens[i]);
		old_vdl = screen->scr_VDLItem;		
		if ((errorCode = SetVDL (
			TheScreen.sc_Screens[i],
			VDLItem
			)) < 0)
		{
		    PRD (("%d: SetVDL() failed -- ", i));
		    SYSERR (errorCode);
		}
		else if ((errorCode = DeleteItem (old_vdl)) < 0)		
		{
		    PRD (("%d: Delete of orig VDL failed -- ", i));
		    SYSERR (errorCode);
		}
	    }

	    SetCEControl (TheScreen.sc_BitmapItems[i],
		    B15POS_PDC | CFBDLSB_CFBD0 | PDCLSB_PDC0,
		    B15POS_MASK | CFBDLSB_MASK | PDCLSB_MASK);

	    if (image_data)
	    {
#ifdef REL10
		CopyVRAMPages (VRAMIOReq, TheScreen.sc_Bitmaps[i]->bm_Buffer,
			image_data, TheScreen.sc_nFrameBufferPages, ~0);
#else
		CopyVRAMPages (TheScreen.sc_Bitmaps[i]->bm_Buffer,
			image_data, TheScreen.sc_nFrameBufferPages, ~0);
#endif

		ccb.ccb_XPos = 0;
		ccb.ccb_HDX = SCREEN_WIDTH << 20;
		// top
		ccb.ccb_YPos = 0;
		ccb.ccb_VDY = VIEW_Y_ADJUST << 16;
		DrawCels (TheScreen.sc_BitmapItems[i], &ccb);
		// bottom
		ccb.ccb_YPos = (SCREEN_HEIGHT - VIEW_Y_ADJUST) << 16;
		ccb.ccb_VDY = (SCREEN_HEIGHT - VIEW_HEIGHT) << 16;
		DrawCels (TheScreen.sc_BitmapItems[i], &ccb);
		
		ccb.ccb_YPos = 0;
		ccb.ccb_VDY = SCREEN_HEIGHT << 16;
		// left
		ccb.ccb_XPos = 0;
		ccb.ccb_HDX = VIEW_X_ADJUST << 20;
		DrawCels (TheScreen.sc_BitmapItems[i], &ccb);
		// right
		ccb.ccb_XPos = (SCREEN_WIDTH - VIEW_X_ADJUST) << 16;
		ccb.ccb_HDX = (SCREEN_WIDTH - VIEW_WIDTH) << 20;
		DrawCels (TheScreen.sc_BitmapItems[i], &ccb);
	    }
	    else
	    {
#ifdef REL10
		SetVRAMPages (VRAMIOReq, TheScreen.sc_Bitmaps[i]->bm_Buffer,
			0, TheScreen.sc_nFrameBufferPages, ~0);
#else
		SetVRAMPages (TheScreen.sc_Bitmaps[i]->bm_Buffer,
			0, TheScreen.sc_nFrameBufferPages, ~0);
#endif
	    }
	    
	    SetClipWidth (TheScreen.sc_BitmapItems[i], VIEW_WIDTH);
	    SetClipHeight (TheScreen.sc_BitmapItems[i], VIEW_HEIGHT);
	    SetClipOrigin (TheScreen.sc_BitmapItems[i], VIEW_X_ADJUST, VIEW_Y_ADJUST);
	}
   
	if (image_data)
	{
#ifdef REL10
	    UnloadImage (image_data);
#else
	    FreeMem (image_data,
	    	    TheScreen.sc_nFrameBufferPages * GrafBase->gf_VRAMPageSize);
#endif
	}
	
	BlackenScreen ();
	DisplayScreen (TheScreen.sc_Screens[TheScreen.sc_curScreen], 0);
	FadeFromBlack (&TheScreen, 60);

LoadAndDisplayCel (waitfile);

	return (1);
    }
	
    return (0);
}

#define CEL_DEBUG

#ifdef CEL_DEBUG
static int	cel_cts[500];
static int	cel_types[5];
static int	dc_ticks[2], dc_times[2];
#endif /* CEL_DEBUG */

void
Uninit3DO (void)
{
    ubyte	*image_data;
    
    if (image_data = LoadImage (imagefile, NULL, NULL, &TheScreen))
    {
#ifdef REL10
	CopyVRAMPages (VRAMIOReq, TheScreen.sc_Bitmaps[!TheScreen.sc_curScreen]->bm_Buffer,
		image_data, TheScreen.sc_nFrameBufferPages, ~0);
	UnloadImage (image_data);
#else
	CopyVRAMPages (TheScreen.sc_Bitmaps[!TheScreen.sc_curScreen]->bm_Buffer,
		image_data, TheScreen.sc_nFrameBufferPages, ~0);
	FreeMem (image_data,
		TheScreen.sc_nFrameBufferPages * GrafBase->gf_VRAMPageSize);
#endif
    }
    
    DisplayScreen (TheScreen.sc_Screens[!TheScreen.sc_curScreen], 0);
    FadeToBlack (&TheScreen, 120);
    
#ifdef CEL_DEBUG
{
int	i, total;
int	*cc;

total = 0;
for (cc = cel_cts, i = 1; i <= sizeof (cel_array) / sizeof (cel_array[0]); i++, ++cc)
{
    if (*cc)
    {
	total += *cc * i;
	kprintf ("%d cel(s) -- %d time(s)\n", i, *cc);
    }
}
kprintf ("Total: %d cel(s)\n\n", total);

for (cc = cel_types, i = 0; i < sizeof (cel_types) / sizeof (cel_types[0]); i++, ++cc)
{
    if (*cc)
    {
	kprintf ("%d cel(s) of type %d\n", *cc, i);
    }
}

kprintf ("\nDrawCels: %d times for %d ticks\n", dc_times[0], dc_ticks[0]);
kprintf ("DrawCels: %d times for %d ticks\n", dc_times[1], dc_ticks[1]);
}
#endif /* CEL_DEBUG */
    ShutDown ();
}

void
_threedo_read_screen (PRECT lpRect, PFRAME_DESC DstFramePtr)
{
    int		y, cx, cy, shift;
    Bitmap	*bitmap;
    uint32	*srcptr;
    uint32	*dstptr;
    
    cx = lpRect->corner.x;
    cy = lpRect->corner.y;

    if (TYPE_GET (_CurFramePtr->TypeIndexAndFlags) == OVERLAY_DRAWABLE)
        bitmap = OverlayWinBitmaps[0];
    else
    {
	cx += VIEW_X_ADJUST;
	cy += VIEW_Y_ADJUST;
        bitmap = TheScreen.sc_Bitmaps[TheScreen.sc_curScreen];
    }
    
    srcptr = (uint32 *)bitmap->bm_Buffer + (cy * (bitmap->bm_Width >> 1)) + cx;
    
    if (cy & 1)
    {
        shift = 0;
	srcptr -= (bitmap->bm_Width >> 1);
    }
    else
        shift = 16;
    
    dstptr = (uint32 *)((uchar *)DstFramePtr + DstFramePtr->DataOffs);
    for (y = 0; y < lpRect->extent.height; y++)
    {
	uint32	*_srcptr;
	int	x;

	_srcptr = srcptr;
	for (x = 0; x < lpRect->extent.width; x += 2)
	{
	    int	lo, hi;

	    hi = (*_srcptr++ >> shift) & 0xFFFF;
	    lo = (*_srcptr++ >> shift) & 0xFFFF;
	    *dstptr++ = MAKE_DWORD (lo, hi);
	}
	
	shift = 16 - shift;
	
	if (shift != 0)
	    srcptr += bitmap->bm_Width; // increment by two lines (since bitmaps are interleaved)
	    
    }
}

void
_threeDO_blt (PRECT pClipRect, PPRIMITIVE PrimPtr)
{
    int		w, h;
    int		type;
    COORD	dst_x, dst_y;
    extern int _ThreeDO_batch_cels (void);	

    if (TYPE_GET (_CurFramePtr->TypeIndexAndFlags) == RAM_DRAWABLE)
    {
	memory_blt (pClipRect, PrimPtr);
	return;
    }

    if (cur_ccb == &cel_array[sizeof (cel_array) / sizeof (cel_array[0])])
	_ThreeDO_batch_cels ();

    type = GetPrimType (PrimPtr);
    if (type != STAMP_PRIM && type != STAMPFILL_PRIM)
    {
	COLOR	color;

#ifdef CEL_DEBUG
++cel_types[0];
#endif /* CEL_DEBUG */
	dst_x = pClipRect->corner.x;
	dst_y = pClipRect->corner.y;
	
	CLR_CEL_FLAGS (cur_ccb, CCB_PACKED | CCB_LDPLUT);
	
	cur_ccb->ccb_HDX = pClipRect->extent.width << 20;
	cur_ccb->ccb_VDY = pClipRect->extent.height << 16;
	
	cur_ccb->ccb_PRE0 =
		(PRE0_BPP_16 << PRE0_BPP_SHIFT) |
		PRE0_BGND |
		PRE0_LINEAR;
	cur_ccb->ccb_PRE1 = PRE1_TLLSB_PDC0;
	cur_ccb->ccb_PIXC = PIXC_UNCODED16;
	
	color = COLOR_32k (GetPrimColor (PrimPtr)) | 0x8000;
	cur_ccb->ccb_PLUTPtr = (void *)MAKE_DWORD (color, color);
	cur_ccb->ccb_SourcePtr = (void *)&cur_ccb->ccb_PLUTPtr; 
    }
    else
    {
	DWORD		frame_flags;
	PFRAME_DESC	SrcFramePtr;
    
	SrcFramePtr = (PFRAME_DESC)PrimPtr->Object.Stamp.frame;
	frame_flags = GetFrameFlags (SrcFramePtr);
	
	dst_x = _save_stamp.origin.x - GetFrameHotX (_CurFramePtr);
	dst_y = _save_stamp.origin.y - GetFrameHotY (_CurFramePtr);
	
	w = GetFrameWidth (SrcFramePtr);
	h = GetFrameHeight (SrcFramePtr);	
	if (!(frame_flags & X_FLIP))
	    cur_ccb->ccb_HDX = 1 << 20;
	else
	{
	    cur_ccb->ccb_HDX = -1 << 20;
	    dst_x += w;
	}
	
	cur_ccb->ccb_VDY = 1 << 16;
	if (frame_flags & DATA_PACKED)
	{
	    SET_CEL_FLAGS (cur_ccb, CCB_PACKED | CCB_LDPLUT);
	    cur_ccb->ccb_PRE0 =
	    	    PRE0_LITERAL |
		    ((h - PRE0_VCNT_PREFETCH) << PRE0_VCNT_SHIFT) |
		    (PRE0_BPP_8 << PRE0_BPP_SHIFT);
	    if (type == STAMP_PRIM)
	    {
#ifdef CEL_DEBUG
++cel_types[1];	
#endif /* CEL_DEBUG */
		cur_ccb->ccb_PIXC = PIXC_CODED8;
		cur_ccb->ccb_PLUTPtr = varPLUTs[frame_flags & 0xFF];
	    }
	    else /* STAMPFILL_PRIM */
	    {
		COLOR	color;
		
		color = COLOR_32k (GetPrimColor (PrimPtr));
		
		if (color == SHADOW_COLOR)
		{
#ifdef CEL_DEBUG
++cel_types[2];	
#endif /* CEL_DEBUG */
		    cur_ccb->ccb_PIXC = PIXC_CODED8_TRANS;
		    cur_ccb->ccb_PLUTPtr = varPLUTs[frame_flags & 0xFF];
		}
		else
		{
		    SOLID_PLUT	*pPlut;
		
#ifdef CEL_DEBUG
++cel_types[3];	
#endif /* CEL_DEBUG */
		    cur_ccb->ccb_PIXC = PIXC_USEPLUT;
		
		    pPlut = solidPLUTs;
		    do
		    {
			if (color == pPlut->color)
			{
			    cur_ccb->ccb_PLUTPtr = pPlut->plutvals;
			    break;
			}
			
			pPlut++;
		    } while (pPlut->color != 0x8000);
#ifdef DEBUG
if (pPlut->color == 0x8000)
{
    kprintf ("couldn't find plut for color 0x%08x\n", color);
    cur_ccb->ccb_PIXC = PIXC_CODED8;
    cur_ccb->ccb_PLUTPtr = varPLUTs[0];
}
#endif /* DEBUG */
		}
	    }
	}
	else /* DATA_SCREEN */
	{
#ifdef CEL_DEBUG
++cel_types[4];	
#endif /* CEL_DEBUG */
	    CLR_CEL_FLAGS (cur_ccb, CCB_PACKED | CCB_LDPLUT);
	    cur_ccb->ccb_PRE0 =
		    PRE0_BGND |
		    PRE0_LINEAR |
	    	    ((h - PRE0_VCNT_PREFETCH) << PRE0_VCNT_SHIFT) |
		    (PRE0_BPP_16 << PRE0_BPP_SHIFT);
	    cur_ccb->ccb_PRE1 =
	    	    PRE1_TLLSB_PDC0 |
	    	    ((w - PRE1_TLHPCNT_PREFETCH) << PRE1_TLHPCNT_SHIFT) |
		    (((((w << 1) + 3) >> 2) - PRE1_WOFFSET_PREFETCH) << PRE1_WOFFSET10_SHIFT);
	    cur_ccb->ccb_PIXC = PIXC_UNCODED16;
	}
	
	cur_ccb->ccb_SourcePtr = (void *)((BYTE *)SrcFramePtr + SrcFramePtr->DataOffs);    
    }
    
    cur_ccb->ccb_XPos = dst_x << 16;
    cur_ccb->ccb_YPos = dst_y << 16;

    cur_ccb++;
}

int
_ThreeDO_batch_cels ()
{
    Item	BitmapItem;
    SMALL_CCB	*last_ccb;

    if (TYPE_GET (_CurFramePtr->TypeIndexAndFlags) == OVERLAY_DRAWABLE)
    {
	if (cur_ccb != &cel_array[0])
	{
	    last_ccb = --cur_ccb;
	    SET_CEL_FLAGS (cur_ccb, CCB_LAST);
	
	    cur_ccb = &cel_array[0];

#ifdef CEL_DEBUG
++cel_cts[last_ccb - cel_array];
{
int	t;

t = GetAudioTime ();
#endif /* CEL_DEBUG */
	    DrawCels (OverlayWinBitmapItems[0], (CCB *)cur_ccb);
#ifdef CEL_DEBUG
dc_ticks[0] += GetAudioTime () - t;
dc_times[0]++;
}
#endif /* CEL_DEBUG */
	    CLR_CEL_FLAGS (last_ccb, CCB_LAST);
	}
	
	return (1);
    }
    
    if (!(_batch_flags & (BUILD_FRAME | FIND_PAGE)))
	BitmapItem = TheScreen.sc_BitmapItems[TheScreen.sc_curScreen];
    else
	BitmapItem = TheScreen.sc_BitmapItems[!TheScreen.sc_curScreen];

//kprintf ("flags 0x%02x  depth %d  ", _batch_flags, _batch_depth);
    if (cur_ccb != &cel_array[0])
    {
	if (!(_batch_flags & FIRST_BATCH) && (_batch_flags & (BUILD_FRAME | FIND_PAGE)))
	{
	    _batch_flags |= FIRST_BATCH | PLUTS_LOADED;

	    if (!(_batch_flags & FIND_PAGE))
	    {
//kprintf ("\tcopying previous\n");
		render_screen_cel (
	    		_old_org_x - _new_org_x,
			_old_org_y - _new_org_y,
			VIEW_X_ADJUST,
    	    		VIEW_Y_ADJUST,
			SCREEN_WIDTH,
			VIEW_WIDTH,
			VIEW_HEIGHT,
			(void *)TheScreen.sc_Bitmaps[TheScreen.sc_curScreen]->bm_Buffer,
			BitmapItem
			);
		if (cur_overlay_win_offs)
		{
		    // copy buffered part of screen to new screen
		    render_screen_cel (
			    _old_org_x - _new_org_x,
			    VIEW_HEIGHT - (cur_overlay_win_offs) +
				    (_old_org_y - _new_org_y),
			    0,
    	    		    0,
			    SCREEN_WIDTH,
			    VIEW_WIDTH,
			    (cur_overlay_win_offs),
			    (void *)OverlayWinBitmaps[1]->bm_Buffer,
			    BitmapItem
			    );
	    	}
	    }
	}

	last_ccb = --cur_ccb;
	SET_CEL_FLAGS (cur_ccb, CCB_LAST);
	
	cur_ccb = &cel_array[0];

//if (_batch_flags & (BUILD_FRAME | FIND_PAGE))
//    kprintf ("\tdrawing %d cels to hidden\n", last_ccb - cel_array + 1);


#ifdef CEL_DEBUG
++cel_cts[last_ccb - cel_array];

{
int	t;

t = GetAudioTime ();
#endif /* CEL_DEBUG */
	DrawCels (BitmapItem, (CCB *)cur_ccb);
#ifdef CEL_DEBUG
dc_ticks[1] += GetAudioTime () - t;
dc_times[1]++;
}
#endif /* CEL_DEBUG */
	CLR_CEL_FLAGS (last_ccb, CCB_LAST);
    }

    if (_batch_depth == 0 && (_batch_flags & BUILD_FRAME))
    {
	if (_batch_flags & FIRST_BATCH)
	{
	    TheScreen.sc_curScreen = !TheScreen.sc_curScreen;
	    
	    if (cur_overlay_win_offs)
	    {
	        // copy new screen to buffer
		SCREEN_TO_BUFFER (
			TheScreen.sc_Bitmaps[TheScreen.sc_curScreen]->bm_Buffer,
		    	OverlayWinBitmapItems[1]
			);
	        // copy overlay window to new screen
		BUFFER_TO_SCREEN (
			OverlayWinBitmaps[0]->bm_Buffer,
			BitmapItem,
			cur_overlay_win_offs
			);
	    }

            DisplayScreen (TheScreen.sc_Screens[TheScreen.sc_curScreen], 0);
//kprintf ("\tflipping\n");
	}
	else if (cur_overlay_win_offs) // copy overlay window to screen
	    BUFFER_TO_SCREEN (
		    OverlayWinBitmaps[0]->bm_Buffer,
		    TheScreen.sc_BitmapItems[TheScreen.sc_curScreen],
		    cur_overlay_win_offs
		    );
	
	_batch_flags &= ~(BUILD_FRAME | FIND_PAGE | FIRST_BATCH);

	_old_org_x = _new_org_x;
	_old_org_y = _new_org_y;
    }

    return (1);
}
