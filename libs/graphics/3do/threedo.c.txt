#include "Portfolio.h"
#include "Init3DO.h"
#include "Parse3DO.h"
#include "Utils3DO.h"
#include "filefunctions.h"
#include "BlockFile.h"
#include "ThreeDO.h"
#include "defs3DO.h"
#include "ourvdl.h"
#include "task/3do/task3do.h"

#define DEBUG

#define CUR_CYCLED		(1<<7)

#define FRAME_HOT_X(f)		GetFrameHotX (f)
#define FRAME_HOT_Y(f)		GetFrameHotY (f)

#define INITIAL_DIRECTORY	"$boot"

#ifdef DEBUG
#define PRD(s)		printf s
#define SYSERR(e)	PrintfSysErr (e)
#else /* DEBUG */
#define PRD(s)
#define SYSERR(e)
#endif /* DEBUG */

#define EXTRA_SCREEN		2
#if EXTRA_SCREEN
#define NUMBER_OF_SCREENS	4
#else
#define NUMBER_OF_SCREENS	2
#endif

#define VIEW_X_ADJUST		((SCREEN_WIDTH - VIEW_WIDTH) >> 1)		
#define VIEW_Y_ADJUST		((SCREEN_HEIGHT - VIEW_HEIGHT) >> 1)		

#define PIXC_DUP(v)		(((v) << PPMP_0_SHIFT) | ((v) << PPMP_1_SHIFT))
#define PIXC_UNCODED16		(PIXC_DUP (PPMPC_MF_4 | PPMPC_SF_4))
#define PIXC_CODED8		(PIXC_DUP (PPMPC_MS_PIN | PPMPC_SF_8))
#define PIXC_USEPLUT		(PIXC_DUP (PPMPC_MF_8 | PPMPC_SF_8))
#define PIXC_CODED8_TRANS	(PIXC_DUP (PPMPC_MS_PIN | PPMPC_SF_8 | PPMPC_2S_CFBD | PPMPC_2D_2))
#define PIXC_USEPLUT_TRANS	(PIXC_DUP (PPMPC_MF_8 | PPMPC_SF_8 | PPMPC_2S_CFBD | PPMPC_2D_2))

#define SET_CEL_FLAGS(c,f)	((c)->ccb_Flags |= (f))
#define CLR_CEL_FLAGS(c,f)	((c)->ccb_Flags &= ~(f))

#define CEL_FLAGS \
	(CCB_SPABS | CCB_PPABS | CCB_YOXY | CCB_ACW | CCB_ACCW \
	| CCB_LDSIZE | CCB_CCBPRE | CCB_LDPRS | CCB_LDPPMP | CCB_ACE \
	| CCB_LCE | CCB_PLUTPOS | CCB_NPABS | CCB_BGND /*| CCB_NOBLK*/)

#define NUMBER_OF_VARPLUTS	256
	
#define NUMBER_OF_PLUTVALS	32
#define NUMBER_OF_PLUT_UINT32s	(NUMBER_OF_PLUTVALS >> 1)
#define PLUT_BYTE_SIZE		(sizeof (uint32) * NUMBER_OF_PLUT_UINT32s)
#define NUMBER_OF_CLUTVALS	32

#define NUM_CELS		250

typedef struct
{
    CCB		cel;
    uint32	plutvals[NUMBER_OF_PLUT_UINT32s];
} MY_CCB;

typedef struct
{
    int32 	sc_curScreen;
    int32 	sc_nFrameBufferPages;
    int32 	sc_nFrameByteCount;
    Item 	sc_Screens[NUMBER_OF_SCREENS];
    Item	sc_BitmapItems[NUMBER_OF_SCREENS];
    Bitmap	*sc_Bitmaps[NUMBER_OF_SCREENS];
} ourScreenContext;

#define VARPLUTS_SIZE	(NUMBER_OF_VARPLUTS * NUMBER_OF_PLUT_UINT32s * sizeof (uint32))
#define GET_VAR_PLUT(i)	(_varPLUTs + (i) * NUMBER_OF_PLUT_UINT32s)

#define GET_SOLID_PLUT(ccb, plutvals, color) \
    do \
    { \
	uint32	colorval; \
	int	i; \
	uint32	*pP; \
 	\
	colorval = (color) | 0x8000; \
	colorval |= (colorval << 16); \
	i = NUMBER_OF_PLUT_UINT32s; \
	pP = (ccb)->ccb_PLUTPtr = plutvals; \
	do \
	    *pP++ = colorval; \
	while (--i); \
	(ccb)->ccb_PIXC = PIXC_USEPLUT; \
    } while (0)

uint32			*_varPLUTs;
static uint32		varPLUTsize = VARPLUTS_SIZE;

static ourScreenContext	theScreen;
static Item		theScreenGroup;
static Item		VBLreq;

static MY_CCB		*loc_ccb, *cel_array;

static CCB		screen_ccb;
#if EXTRA_SCREEN
static CCB		extra_ccb;
#endif

static CCB		*cur_ccb, *first_ccb;

static Item	VRAMIOReq;

static uint32	colorEntries[NUMBER_OF_CLUTVALS], ceCt;

int		_new_org_x, _new_org_y;
static int	_old_org_x, _old_org_y;

//#if defined(V1p3)
#define NTSC_SCREEN_WIDTH	320
#define NTSC_SCREEN_HEIGHT	240

int		_screen_width = NTSC_SCREEN_WIDTH, _screen_height = NTSC_SCREEN_HEIGHT;
//#endif

static void
render_screen_cel (int dx, int dy, int sx, int sy, void *srcptr, CCB *screen_ccb)
{
    screen_ccb->ccb_SourcePtr = (void *)((uint32 *)srcptr + sx + (sy * (SCREEN_WIDTH >> 1)));
	    
    screen_ccb->ccb_XPos = dx << 16;
    screen_ccb->ccb_YPos = dy << 16;
}

#define MAKE_INDEX(p)		(((p) & 0x1f) >> 1)	// index to the plut entry.	
#define MAKE_WORD_SHIFT(p) 	((~(p) & 1) << 4) 	// select high or low word.
#define MAKE_SCALE_FACT(p)	(((p) >> 5) + 1)	// create 3-bit scale factor.
#define MAKE_GRN(p,f)		((((p) & 0x1f) * (f)) >> 3)
#define MAKE_BLU(p,f)		(((((p) & (0x1f << 5)) >> 5) * (f)) >> 3)
#define MAKE_RED(p,f)		(((((p) & (0x1f << 10)) >> 10) * (f)) >> 3)
#define LOOKUP_PIXEL(p,pP) \
	(_p = pP[MAKE_INDEX(p)] >> MAKE_WORD_SHIFT(p), \
	_f = MAKE_SCALE_FACT(p), \
	(((MAKE_GRN(_p,_f)|(MAKE_BLU(_p,_f)<<5)|(MAKE_RED(_p,_f)<<10)) \
		& 0xffff) | 0x8000))

#ifdef DEBUG
#define DUMP_PLUT(i) \
	{ \
	    uint32	*vp; \
	    int		j; \
	    \
	    vp = GET_VAR_PLUT (i); \
	    printf ("PLUT %d\n", i); \
	    for (j = 0; j < 16; j++) \
	    { \
		printf ("     0x%04x 0x%04x\n", \
			(vp[j] >> 16) & 0xFFFF, vp[j] & 0xFFFF); \
	    } \
	}
#endif /* PLUT_DEBUG */

void *
GetScreenBitmap ()
{
    return ((void *)theScreen.sc_Bitmaps[theScreen.sc_curScreen]->bm_Buffer);
}

void
_threedo_load_pluts (ubyte *colors)
{
    int		start, end, bytes;
    ubyte	*vp;

    start = *colors++;
    end = *colors++;
    if (start > end)
	return;
	
    bytes = (end - start + 1) * PLUT_BYTE_SIZE;

    if (!_varPLUTs)
    {
//	SetCEControl (theScreen.sc_BitmapItems[0], B15POS_PDC, B15POS_MASK);
//	SetCEControl (theScreen.sc_BitmapItems[1], B15POS_PDC, B15POS_MASK);

	// where should I free this???
	if (!(_varPLUTs = (uint32 *)ThreedoAlloc (bytes)))
	    return;
	varPLUTsize = bytes;
    }
	
    vp = (ubyte *)_varPLUTs + (start * PLUT_BYTE_SIZE);
    memcpy ((ubyte *)vp, colors, bytes);
}

void
_threedo_set_colors (ubyte *colors, uint32 indices)
{
    int		i, start, end;
    uint32	*ce;

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
    
    ceCt = end - start + 1;

    _threedo_add_task (TASK_SET_CLUT);
    _batch_flags |= CYCLE_PENDING;
}

static int	clut_scale;

void
_threedo_change_clut (int v)
{
    clut_scale = v;
    _threedo_add_task (TASK_SCALE_CLUT);
}

void
_threedo_scale_clut ()
{
    int		i;
    uint32	colorEntries[NUMBER_OF_CLUTVALS], *ce;

    ce = colorEntries;
    for (i = 0; i < NUMBER_OF_CLUTVALS; i++)
    {
	int	color;

	if (clut_scale <= 0xFF)
	    color = clut_scale * i / 31;
	else if ((color = clut_scale * (i + 1) / 32) > 0xFF)
	    color = 0xFF;
	*ce++ = MakeCLUTColorEntry (i, (ubyte)color, (ubyte)color, (ubyte)color);
    }

    for (i = 0; i < NUMBER_OF_SCREENS; i++)
	SetScreenColors (theScreen.sc_Screens[i],
		colorEntries, NUMBER_OF_CLUTVALS);
}

void
_threedo_enable_fade ()
{
    int32	ce_flags;
    int		src, dst;
    int		count;
    uint32	*srcptr, *dstptr;

    ce_flags = theScreen.sc_Bitmaps[theScreen.sc_curScreen]->bm_CEControl;
    if ((ce_flags & B15POS_MASK) == B15POS_0)
	return;
	
    src = theScreen.sc_curScreen;
    dst = !theScreen.sc_curScreen;
    SetCEControl (theScreen.sc_BitmapItems[src], B15POS_0, B15POS_MASK);
    SetCEControl (theScreen.sc_BitmapItems[dst], B15POS_0, B15POS_MASK);

    srcptr = (uint32 *)theScreen.sc_Bitmaps[src]->bm_Buffer;
    dstptr = (uint32 *)theScreen.sc_Bitmaps[dst]->bm_Buffer;
    count = (SCREEN_HEIGHT * SCREEN_WIDTH) >> 1;
    
    do
    {
	uint32	p;
	
	p = *srcptr;
	if ((_batch_flags & (ENABLE_CYCLE | CYCLE_PENDING | CUR_CYCLED))
		&& (p & 0x80008000) != 0x80008000)
	{
	    uint32	r, g, b, lo, hi;
	    
	    if (!(p & 0x8000))
	    {
		r = (colorEntries[(p >> 10) & 0x1f] >> 16) & 0xff;
		g = (colorEntries[(p >> 5) & 0x1f] >> 8) & 0xff;
		b = colorEntries[p & 0x1f] & 0xff;
		lo = ((r >> 3) << 10) | ((g >> 3) << 5) | (b >> 3);
	    }
	    else
		lo = p & 0x7fff;
		
	    p >>= 16;
	    if (!(p & 0x8000))
	    {
		r = (colorEntries[(p >> 10) & 0x1f] >> 16) & 0xff;
		g = (colorEntries[(p >> 5) & 0x1f] >> 8) & 0xff;
		b = colorEntries[p & 0x1f] & 0xff;
		hi = ((r >> 3) << 10) | ((g >> 3) << 5) | (b >> 3);
	    }
	    else
		hi = p & 0x7fff;
		
	    *dstptr = (hi << 16) | lo;
	}
	else
	    *dstptr = p & ~0x80008000;
	
	++srcptr;
	++dstptr;
    } while (--count);

    DisplayScreen (theScreen.sc_Screens[dst], 0);
    _batch_flags &= ~(CUR_CYCLED | CYCLE_PENDING);
    theScreen.sc_curScreen = dst;
}

void
_threedo_set_clut (int mask)
{
    if (ceCt)
    {
	int	i;

	for (i = 0; i < NUMBER_OF_SCREENS; i++)
	{
	    if (mask & (1 << i))
		SetScreenColors (theScreen.sc_Screens[i], colorEntries, ceCt);
	}
	
	if (!(_batch_flags & CYCLE_PENDING))
	    ceCt = 0;
    }
    
    if (mask == 3) // both screens
    {
	_batch_flags &= ~CYCLE_PENDING;
	_batch_flags |= CUR_CYCLED;
    }
}

static bool
ourOpenGraphics (ourScreenContext *sc, Item *gItem)
{
    int32	i, w, h;
    int32	bmpages, bmsize, pagesize;
    TagArg	taScreenTags[6], *t;
    void	*bmbuf[MAXSCREENS];
    extern Item	SPORTDevice;

    if (!SPORTDevice)
    {
	if (OpenGraphicsFolio () || !OpenSPORT ())
	{
	    PRD (("Cannot open graphics folio\n"));
	    return FALSE;
	}
    }

#if defined(V1p3) && defined(PAL_SUPPORTED)
#define PAL_VB_FREQ		50
#define PAL_SCREEN_WIDTH	384
#define PAL_SCREEN_HEIGHT	288
#define NTSC_VB_FREQ		60
    if (QueryGraphics (QUERYGRAF_TAG_FIELDFREQ, (void *)&i) < 0
	    || i != PAL_VB_FREQ)
    {
	_screen_width = NTSC_SCREEN_WIDTH;
	_screen_height = NTSC_SCREEN_HEIGHT;
    }
    else
    {
	_screen_width = PAL_SCREEN_WIDTH;
	_screen_height = PAL_SCREEN_HEIGHT;
    }
    pagesize = GetPageSize (MEMTYPE_VRAM);
#else
    _screen_width = NTSC_SCREEN_WIDTH;
    _screen_height = NTSC_SCREEN_HEIGHT;
    pagesize = GrafBase->gf_VRAMPageSize;
#endif
    
    w = SCREEN_WIDTH;
    h = SCREEN_HEIGHT;
    bmpages = (w * 2 * h + pagesize - 1) / pagesize;
    bmsize = bmpages * pagesize;

#ifdef ALLOC_SEPARATELY    
    for (i = 0; i < NUMBER_OF_SCREENS; i++)
	bmbuf[i] = 0;
	
    for (i = 0; i < NUMBER_OF_SCREENS; i++)
    {
	if (!(bmbuf[i] = AllocMem (bmsize,
		MEMTYPE_VRAM | MEMTYPE_FILL | 0)))
	    goto Failure;
    }
#else
    bmbuf[0] = AllocMem (bmsize * NUMBER_OF_SCREENS,
	    MEMTYPE_VRAM | MEMTYPE_FILL | 0);
    if (!bmbuf[0])
	goto Failure;
    for (i = 1; i < NUMBER_OF_SCREENS; i++)
	bmbuf[i] = (void *)((char *)bmbuf[i - 1] + bmsize);
#endif
	
    t = taScreenTags;
    t->ta_Tag = CSG_TAG_SCREENCOUNT;
    t->ta_Arg = (void *)NUMBER_OF_SCREENS;
    t++;
    t->ta_Tag = CSG_TAG_BITMAPCOUNT;
    t->ta_Arg = (void *)NUMBER_OF_SCREENS;
    t++;
    t->ta_Tag = CSG_TAG_BITMAPBUF_ARRAY;
    t->ta_Arg = (void *)bmbuf;
    t++;
    t->ta_Tag = CSG_TAG_BITMAPWIDTH_ARRAY;
    t->ta_Arg = (void *)&w;
    t++;
    t->ta_Tag = CSG_TAG_BITMAPHEIGHT_ARRAY;
    t->ta_Arg = (void *)&h;
    t++;
    t->ta_Tag = CSG_TAG_DONE;
	
    *gItem = CreateScreenGroup (&(sc->sc_Screens[0]), taScreenTags);
    if (*gItem < 0)
    {
	PRD (("Cannot create screen group"));
	SYSERR (*gItem);
	goto Failure;
    }
    
    AddScreenGroup (*gItem, NULL);

    for (i = 0; i < NUMBER_OF_SCREENS; i++)
    {
	Screen	*screen;
	
	if ((screen = (Screen *)LookupItem (sc->sc_Screens[i])) == NULL) 
	{
	    PRD (("%d: Cannot locate screen\n", i));
	    goto Failure;
	}
	
	sc->sc_BitmapItems[i] = screen->scr_TempBitmap->bm.n_Item;
	sc->sc_Bitmaps[i] = screen->scr_TempBitmap;
	EnableHAVG (sc->sc_Screens[i]);
	EnableVAVG (sc->sc_Screens[i]);
    }

    sc->sc_nFrameBufferPages = bmpages;
    sc->sc_nFrameByteCount = bmsize;
    sc->sc_curScreen = 0;

    return TRUE;
    
Failure:
#ifdef ALLOC_SEPARATELY
    for (i = 0; i < NUMBER_OF_SCREENS; i++)
    {
	if (bmbuf[i])
	    FreeMem (bmbuf[i], bmsize);
    }
#else
    if (bmbuf[0])
	FreeMem (bmbuf[0], bmsize * NUMBER_OF_SCREENS);
#endif
    
    return FALSE;
}

static void
ourCloseGraphics (ourScreenContext *sc, Item sgItem)
{
    int		i;
    int32	errorCode;

    if ((errorCode = RemoveScreenGroup (sgItem)) < 0)
    {
	PRD (("RemoveScreenGroup failed -- "));
	SYSERR (errorCode);
    }

    if ((errorCode = DeleteItem (sgItem)) < 0)
    {
	PRD (("DeleteScreenGroup failed -- "));
	SYSERR (errorCode);
    }
    
    for (i = 0; i < NUMBER_OF_SCREENS; i++)
    {
	Screen	*screen;
		
	screen = (Screen *)LookupItem (sc->sc_Screens[i]);
	if ((errorCode = DeleteItem (screen->scr_VDLItem)) < 0)		
	{
	    PRD (("%d: DeleteItem of VDL failed -- ", i));
	    SYSERR (errorCode);
	}
    }

#ifdef ALLOC_SEPARATELY
    for (i = 0; i < NUMBER_OF_SCREENS; i++)
    {
	FreeMem (sc->sc_Bitmaps[i]->bm_Buffer, sc->sc_nFrameByteCount);
	bmbuf[i] = 0;
    }
#else
    FreeMem (sc->sc_Bitmaps[0]->bm_Buffer,
	    sc->sc_nFrameByteCount * NUMBER_OF_SCREENS);
#endif
	
    for (i = 0; i < NUMBER_OF_SCREENS; i++)
    {
	if ((errorCode = DeleteItem (sc->sc_BitmapItems[i])) < 0)
	{
	    PRD (("%d: DeleteItem of Bitmap failed -- ", i));
	    SYSERR (errorCode);
	}
	if ((errorCode = DeleteItem (sc->sc_Screens[i])) < 0)
	{
	    PRD (("%d: DeleteItem of Screen failed -- ", i));
	    SYSERR (errorCode);
	}
    }

    ScavengeMem (); // necessary after FreeMem to return memory to system
}

int
Init3DO (void)
{
    if (ourOpenGraphics (&theScreen, &theScreenGroup))
    {
	int		i;

	OpenMathFolio (); // calling this multiply is not a problem, right? 
    
	if (ChangeDirectory (INITIAL_DIRECTORY) < 0)
	    return (0);

	if (!VRAMIOReq)
	    VRAMIOReq = GetVRAMIOReq ();

	if (!cel_array
		&& !(cel_array = (MY_CCB *)ThreedoAlloc (sizeof (MY_CCB) * NUM_CELS)))
	    return (0);

#ifdef NOT_HERE
#ifdef LOAD_VARPLUTS
	if (!_varPLUTs
		&& !(_varPLUTs = (uint32 *)ThreedoAlloc (varPLUTsize)))
	    return (0);
#endif
#endif

	{
	    screen_ccb.ccb_Flags =
		    CCB_SPABS |
		    CCB_YOXY |
		    CCB_ACW |
		    CCB_ACCW |
		    CCB_LDSIZE |
		    CCB_CCBPRE |
		    CCB_BGND |
		    CCB_LDPRS |
		    CCB_LDPPMP |
		    CCB_LCE |
		    CCB_ACE |
		    CCB_PLUTPOS |
		    CCB_LAST |
		    CCB_NPABS
		    /*| CCB_NOBLK*/;
	
	    screen_ccb.ccb_HDX = 1 << 20;
	    screen_ccb.ccb_VDY = 1 << 16;
	
	    screen_ccb.ccb_PIXC = PIXC_UNCODED16;

	    screen_ccb.ccb_PRE0 =
	    	    PRE0_BGND
		    | PRE0_LINEAR
	    	    | (PRE0_BPP_16 << PRE0_BPP_SHIFT)
		    | (((VIEW_HEIGHT >> 1) - PRE0_VCNT_PREFETCH) << PRE0_VCNT_SHIFT);

	    screen_ccb.ccb_PRE1 =
		    PRE1_LRFORM
		    | PRE1_TLLSB_PDC0
	    	    | ((VIEW_WIDTH - PRE1_TLHPCNT_PREFETCH) << PRE1_TLHPCNT_SHIFT)
		    | ((SCREEN_WIDTH - PRE1_WOFFSET_PREFETCH) << PRE1_WOFFSET10_SHIFT);
	}

#if EXTRA_SCREEN
	{
	    extra_ccb.ccb_Flags =
		    CCB_SPABS |
		    CCB_YOXY |
		    CCB_ACW |
		    CCB_ACCW |
		    CCB_LDSIZE |
		    CCB_CCBPRE |
		    CCB_BGND |
		    CCB_LDPRS |
		    CCB_LDPPMP |
		    CCB_LCE |
		    CCB_ACE |
		    CCB_PLUTPOS |
		    CCB_LAST |
		    CCB_NPABS
		    /*| CCB_NOBLK*/;
		    
	    extra_ccb.ccb_SourcePtr
		    = (void *)theScreen.sc_Bitmaps[EXTRA_SCREEN]->bm_Buffer;
		    
	    extra_ccb.ccb_XPos = 0;
	    extra_ccb.ccb_YPos = 0;
	    
	    extra_ccb.ccb_HDX = 1 << 20;
	    extra_ccb.ccb_VDY = 1 << 16;
	
	    extra_ccb.ccb_PIXC = PIXC_UNCODED16;

	    extra_ccb.ccb_PRE0 =
	    	    PRE0_BGND
		    | PRE0_LINEAR
	    	    | (PRE0_BPP_16 << PRE0_BPP_SHIFT)
		    | (((VIEW_HEIGHT >> 1) - PRE0_VCNT_PREFETCH) << PRE0_VCNT_SHIFT);

	    extra_ccb.ccb_PRE1 =
		    PRE1_LRFORM
		    | PRE1_TLLSB_PDC0
	    	    | ((VIEW_WIDTH - PRE1_TLHPCNT_PREFETCH) << PRE1_TLHPCNT_SHIFT)
		    | ((SCREEN_WIDTH - PRE1_WOFFSET_PREFETCH) << PRE1_WOFFSET10_SHIFT);
	}
#endif
	
	for (loc_ccb = cel_array, i = NUM_CELS; i > 0; i--, loc_ccb++)
	    loc_ccb->cel.ccb_Flags = CEL_FLAGS;
	
	loc_ccb = cel_array;

	for (i = 0; i < NUMBER_OF_SCREENS; i++)
	{
	    VDL_REC	vdl_rec;
	    Item	VDLItem;
		
	    SimpleVDL (&vdl_rec, theScreen.sc_Bitmaps[i]);
	    
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
		
		screen = (Screen *)LookupItem (theScreen.sc_Screens[i]);
		old_vdl = screen->scr_VDLItem;		
		if ((errorCode = SetVDL (
			theScreen.sc_Screens[i],
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

	    SetCEControl (theScreen.sc_BitmapItems[i],
		    B15POS_PDC | CFBDLSB_CFBD0 | PDCLSB_PDC0,
		    B15POS_MASK | CFBDLSB_MASK | PDCLSB_MASK);

	    SetVRAMPages (VRAMIOReq, theScreen.sc_Bitmaps[i]->bm_Buffer,
		    0, theScreen.sc_nFrameBufferPages, ~0);

if (VIEW_WIDTH != SCREEN_WIDTH || VIEW_HEIGHT != SCREEN_HEIGHT)
{
	    SetClipWidth (theScreen.sc_BitmapItems[i], VIEW_WIDTH);
	    SetClipHeight (theScreen.sc_BitmapItems[i], VIEW_HEIGHT);
}
if (VIEW_X_ADJUST != 0 || VIEW_Y_ADJUST != 0)	
{
	    SetClipOrigin (theScreen.sc_BitmapItems[i], VIEW_X_ADJUST, VIEW_Y_ADJUST);
}
	}

	DisplayScreen (theScreen.sc_Screens[theScreen.sc_curScreen], 0);
	VBLreq = GetVBLIOReq ();

	return (1);
    }
	
    return (0);
}

void
Uninit3DO (void)
{
    if (!theScreen.sc_nFrameByteCount)
	return;
    theScreen.sc_curScreen = !theScreen.sc_curScreen;
    SetVRAMPages (VRAMIOReq,
	    theScreen.sc_Bitmaps[theScreen.sc_curScreen]->bm_Buffer,
	    0, theScreen.sc_nFrameBufferPages, ~0);

    _batch_flags &= ~(CUR_CYCLED | CYCLE_PENDING | ENABLE_CYCLE);
    DisplayScreen (theScreen.sc_Screens[theScreen.sc_curScreen], 0);
    
    if (VBLreq)
    {
	DeleteIOReq (VBLreq);
	VBLreq = 0;
    }
    
#ifdef LOAD_VARPLUTS
    if (_varPLUTs)
    {
	ThreedoFree (_varPLUTs, varPLUTsize);
	_varPLUTs = 0;
    }
#endif
	
    if (cel_array)
    {
	ThreedoFree (cel_array, sizeof (MY_CCB) * NUM_CELS);
	cel_array = 0;
    }

    if (VRAMIOReq)
    {
	DeleteItem (VRAMIOReq);
	VRAMIOReq = 0;
    }

    if (theScreenGroup)
    {
	ourCloseGraphics (&theScreen, theScreenGroup);
	theScreenGroup = 0;
    }
    
    _new_org_x = _new_org_y = 0;
    _old_org_x = _old_org_y = 0;
}

void
SetGraphicGrabOther (int grab_other)
{
    if (grab_other)
	_batch_flags |= GRAB_OTHER;
    else
	_batch_flags &= ~GRAB_OTHER;
}

void
_threedo_read_screen (PRECT lpRect, PFRAME_DESC DstFramePtr)
{
    int		cx, cy;
    uint32	*srcptr;
    uint32	*dstptr;
    
    cx = lpRect->corner.x;
    cy = lpRect->corner.y;

    dstptr = (uint32 *)((uchar *)DstFramePtr + DstFramePtr->DataOffs);
    if (TYPE_GET (_CurFramePtr->TypeIndexAndFlags) != SCREEN_DRAWABLE)
    {
	int	src_incr, dst_incr;
	
	src_incr = ((GetFrameWidth (_CurFramePtr) << 1) + 3) & ~3;
	dst_incr = ((GetFrameWidth (DstFramePtr) << 1) + 3) & ~3;
	srcptr = (uint32 *)((uchar *)_CurFramePtr + _CurFramePtr->DataOffs
		+ cy * src_incr + (cx << 1));
	cx = lpRect->extent.width << 1;
	cy = lpRect->extent.height;
	do
	{
	    memcpy (dstptr, srcptr, cx);
	    dstptr = (uint32 *)((uchar *)dstptr + dst_incr);
	    srcptr = (uint32 *)((uchar *)srcptr + src_incr);
	} while (--cy);
    }
    else
    {
	int	y, shift;
	Bitmap	*bitmap;
	
	cx += VIEW_X_ADJUST;
	cy += VIEW_Y_ADJUST;
        bitmap = theScreen.sc_Bitmaps[(_batch_flags & GRAB_OTHER)
		? !theScreen.sc_curScreen : theScreen.sc_curScreen];
    
	srcptr = (uint32 *)bitmap->bm_Buffer + (cy * (bitmap->bm_Width >> 1)) + cx;
    
	if (cy & 1)
	{
            shift = 0;
	    srcptr -= (bitmap->bm_Width >> 1);
	}
	else
            shift = 16;
    
	for (y = 0; y < lpRect->extent.height; y++)
	{
	    uint32	*_srcptr;
	    int		x;

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
}

#ifdef CEL_DEBUG
static void
DumpCelData (ubyte *data)
{
    BYTE	*pSrc;
    int		h;
    
    h = SCREEN_HEIGHT;
    pSrc = (BYTE *)data;
    do
    {
	BYTE	code;
	BYTE	*pS;
	COORD	cur_x;
	int	offset;

	cur_x = 0;
	pS = pSrc + 2;
	pSrc += (MAKE_WORD (pSrc[1], pSrc[0]) + 2) << 2;
	while (code = *pS++)
	{
	    BYTE	pack_type, num_pixels;
	    COORD	next_x;

	    pack_type = PACK_TYPE (code);
	    num_pixels = PACK_COUNT (code);
	    next_x = cur_x + num_pixels;
		
	    switch (pack_type)
	    {
		case PACK_LITERAL:
		    pS += num_pixels << 1;
		    break;
		case PACK_PACKED:
		    pS += 2;
		    break;
	    }
			
	    cur_x = next_x;
	}
	
	printf ("line %d had %d pixels\n", SCREEN_HEIGHT - h, cur_x);
    } while (--h);
}
#endif

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
    register int32	*pDst, *pD;
    PFRAME_DESC	SrcFramePtr;
    DWORD	frame_flags;

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
    }

    dst_x -= GetFrameHotX (_CurFramePtr);
    dst_y -= GetFrameHotY (_CurFramePtr);
    
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
    else if ((frame_flags = GetFrameFlags (SrcFramePtr)) & DATA_HARDWARE)
    {
	CCB	*ccb;
	BYTE	*pSrc;
	long	*p;
	
	p = (long *)((BYTE *)SrcFramePtr + SrcFramePtr->DataOffs);
	ccb = ParseCel ((void *)(p + 1), *p);
	
	if (!(ccb->ccb_Flags & CCB_PACKED))
	    return;

src_y -= 16;	
	pSrc = (BYTE *)ccb->ccb_SourcePtr;
	if (!(ccb->ccb_Flags & CCB_CCBPRE))
	    pSrc += 4;
	
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
				pS += num_pixels << 1;
				break;
			    case PACK_PACKED:
				pS += 2;
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
				pS += num_pixels << 1;
				break;
			    case PACK_PACKED:
				pS += 2;
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
			    pS += (num_pixels - 1) << 1;
			else
			{
			    BYTE	*_pS;
			    
			    _pS = pS;
			    pS += num_pixels << 1;
			    pD = pDst + (cur_x >> 1);
			    if (cur_x & 1)
			    {
				*pD = (*pD & ~0xFFFF) | MAKE_WORD (*(_pS + 1), *_pS);
				_pS += 2;
				++pD;

				++cur_x;
				--np;
			    }
			    if (np & 1)
			    {
				pD += (np >> 1);
				*pD = (*pD & 0xFFFF) | (MAKE_WORD (*pS, *(pS - 1)) << 16);
				pD = pDst + (cur_x >> 1);
			
				--np;
			    }

			    np >>= 1;
			    while (np--)
			    {
			    	int	lo, hi;
				
				hi = MAKE_WORD (*(_pS + 1), *_pS);
				_pS += 2;
				lo = MAKE_WORD (*(_pS + 1), *_pS);
				_pS += 2;
			        *pD++ = MAKE_DWORD (lo, hi);
			    }
			    break;
			}
		    case PACK_PACKED:
			if (type == STAMP_PRIM)
			{
			    color = MAKE_WORD (*(pS + 1), *pS);
			    color = MAKE_DWORD (color, color);
			}
			pS += 2;

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
    else
    {
	uint32  	*plut_ptr;
	BYTE		*pSrc;
	
	plut_ptr = GET_VAR_PLUT (frame_flags & 0xFF);
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
		int32	_p, _f;

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
				*pD = (*pD & ~0xFFFF) | (LOOKUP_PIXEL (*_pS, plut_ptr));
				++_pS;
				++pD;

				++cur_x;
				--np;
			    }
			    if (np & 1)
			    {
				pD += (np >> 1);
				*pD = (*pD & 0xFFFF) | (LOOKUP_PIXEL (*(pS - 1), plut_ptr) << 16);
				pD = pDst + (cur_x >> 1);
			
				--np;
			    }

			    np >>= 1;
			    while (np--)
			    {
			    	int	lo, hi;
				
				hi = LOOKUP_PIXEL (*_pS, plut_ptr);
				_pS++;
				lo = LOOKUP_PIXEL (*_pS, plut_ptr);
				_pS++;
			        *pD++ = MAKE_DWORD (lo, hi);
			    }
			    break;
			}
		    case PACK_PACKED:
			if (type == STAMP_PRIM)
			{
			    color = LOOKUP_PIXEL (*pS, plut_ptr);
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

void
add_cel (CCB *ccb)
{
    if (!first_ccb)
	first_ccb = ccb;
    else
    {
	cur_ccb->ccb_NextPtr = ccb;
	CLR_CEL_FLAGS (cur_ccb, CCB_LAST);
    }
    cur_ccb = ccb;
}

void
add_cels (CCB *first, CCB *last)
{
    if (!first_ccb)
	first_ccb = first;
    else
    {
	cur_ccb->ccb_NextPtr = first;
	CLR_CEL_FLAGS (cur_ccb, CCB_LAST);
    }
    cur_ccb = last;
}

static int32	gscale;

void
SetGraphicScale (int32 scale)
{
    gscale = scale;
}

#define LOC_SUBTRACT	(1 << 16)
#define LOC_COMBINE	(1 << 17)

static uint32	gPIXC = (PPMPC_MF_4 | PPMPC_SF_4);

void
SetGraphicStrength (int numer, int denom)
{
    gPIXC = 0;
    
    if (denom < 0)
    {
	denom = -denom;
	gPIXC |= LOC_COMBINE;
    }
    
    switch (denom)
    {
 	case 2:
	    denom = 1;
	    break;
	case 4:
	default:
	    denom = 2;
	    break;
	case 8:
	    denom = 3;
	    break;
	case 16:
	    denom = 0;
	    break;
    }

    if (numer < 0)
    {
	numer = -numer;
	gPIXC |= LOC_SUBTRACT;
    }
    
    if (numer == 0)
	numer = denom = 4;
    else
	numer = numer - 1; // because 0 => 1, 1 => 2, etc.

    numer <<= PPMPC_MF_SHIFT;    
    denom <<= PPMPC_SF_SHIFT;    
    gPIXC |= (numer | denom);
}

//#define DOG_DEBUG

#ifdef DOG_DEBUG
int	dog;
#endif

void
_threeDO_blt (PRECT pClipRect, PPRIMITIVE PrimPtr)
{
    int		w, h;
    int		type;
    COORD	dst_x, dst_y;
    register MY_CCB	*cccb;
    extern int _ThreeDO_batch_cels (int);
    int32	clipped_cel;

    if (TYPE_GET (_CurFramePtr->TypeIndexAndFlags) == RAM_DRAWABLE)
    {
	memory_blt (pClipRect, PrimPtr);
	return;
    }

    clipped_cel = (_pCurContext->ClipRect.extent.width != 0);

    // CEL array is full, so dump it to the screen.  Else queue up the ccb.
    if (loc_ccb - cel_array == NUM_CELS)
	_ThreeDO_batch_cels (0);

cccb = loc_ccb;
#define loc_ccb		(&cccb->cel)
#define loc_pluts	(&cccb->plutvals)

    loc_ccb->ccb_Flags = CEL_FLAGS;
    loc_ccb->ccb_HDDX = loc_ccb->ccb_HDDY = 0;
    
    type = GetPrimType (PrimPtr);
//PRD (("type = %d\n", type));
    if (type == LINE_PRIM)
    {
	COLOR	color;
	int	tdx, tdy, dx, dy;
	
	dst_x = (PrimPtr->Object.Line.first.x - FRAME_HOT_X (_CurFramePtr)) << 16;
	dst_y = (PrimPtr->Object.Line.first.y - FRAME_HOT_Y (_CurFramePtr)) << 16;
	dx = PrimPtr->Object.Line.second.x - PrimPtr->Object.Line.first.x;
	dy = PrimPtr->Object.Line.second.y - PrimPtr->Object.Line.first.y;
	
	if (dx < 0)
	    tdx = -dx;
	else
	    tdx = dx;
	    
	if (dy < 0)
	    tdy = -dy;
	else
	    tdy = dy;

	CLR_CEL_FLAGS (loc_ccb, CCB_PACKED);
	
	if (tdx >= tdy) /* X Dominant Case */
	{
		loc_ccb->ccb_PRE0 =
			(PRE0_BPP_8 << PRE0_BPP_SHIFT);
		loc_ccb->ccb_PRE1 = PRE1_TLLSB_PDC0 |
			((SCREEN_WIDTH - PRE1_TLHPCNT_PREFETCH) << PRE1_TLHPCNT_SHIFT) |
			((((((SCREEN_WIDTH >> 1) << 1) + 3) >> 2) - PRE1_WOFFSET_PREFETCH) <<
			PRE1_WOFFSET10_SHIFT);
		loc_ccb->ccb_HDX = dx * 0x100000 / SCREEN_WIDTH;
		loc_ccb->ccb_HDY = dy * 0x100000 / SCREEN_WIDTH;
		loc_ccb->ccb_VDX = 0;
		loc_ccb->ccb_VDY = 1 << 16; /*  = 1 in (16,16) format */
	}
	else /* Y Dominant Case */
	{
		loc_ccb->ccb_PRE0 =
			((SCREEN_HEIGHT - PRE0_VCNT_PREFETCH) << PRE0_VCNT_SHIFT) |
			(PRE0_BPP_8 << PRE0_BPP_SHIFT);
		loc_ccb->ccb_PRE1 = PRE1_TLLSB_PDC0;
		loc_ccb->ccb_HDX = 1 << 20; /* = 1 in (12,20) format */
		loc_ccb->ccb_HDY = 0;
		loc_ccb->ccb_VDX = dx * 0x10000 / SCREEN_HEIGHT;
		loc_ccb->ccb_VDY = dy * 0x10000 / SCREEN_HEIGHT;
	}
	
	SET_CEL_FLAGS (loc_ccb, CCB_LDPLUT);
	color = COLOR_32k (GetPrimColor (PrimPtr));
	GET_SOLID_PLUT (loc_ccb, loc_pluts, color);
	loc_ccb->ccb_SourcePtr = (void *)cel_array;
    }
    else if (type == RECT_PRIM || type == RECTFILL_PRIM || type == POINT_PRIM)
    {
	COLOR	color;

	dst_x = (pClipRect->corner.x - FRAME_HOT_X (_CurFramePtr)) << 16;
	dst_y = (pClipRect->corner.y - FRAME_HOT_Y (_CurFramePtr)) << 16;

	CLR_CEL_FLAGS (loc_ccb, CCB_PACKED | CCB_LDPLUT);

	loc_ccb->ccb_HDX = pClipRect->extent.width << 20;
	loc_ccb->ccb_VDY = pClipRect->extent.height << 16;
	loc_ccb->ccb_HDY = 0;
	loc_ccb->ccb_VDX = 0;

	if (gscale && gscale != (1 << 8) && type != POINT_PRIM)
	{
	    loc_ccb->ccb_HDX = (loc_ccb->ccb_HDX * gscale) >> 8;
	    loc_ccb->ccb_VDY = (loc_ccb->ccb_VDY * gscale) >> 8;
	    dst_x += ((pClipRect->extent.width << 8)
		    - (pClipRect->extent.width * gscale)) << 7;
	    dst_y += ((pClipRect->extent.height << 8)
		    - (pClipRect->extent.height * gscale)) << 7;
	}
	loc_ccb->ccb_PRE0 =
		(PRE0_BPP_16 << PRE0_BPP_SHIFT) |
		PRE0_BGND |
		PRE0_LINEAR;
	loc_ccb->ccb_PRE1 = PRE1_TLLSB_PDC0;
	loc_ccb->ccb_PIXC = PIXC_UNCODED16;
	
	color = COLOR_32k (GetPrimColor (PrimPtr)) | 0x8000;
	loc_ccb->ccb_PLUTPtr = (void *)MAKE_DWORD (color, color);
	loc_ccb->ccb_SourcePtr = (void *)&loc_ccb->ccb_PLUTPtr; 
    }
    else if (type == STAMP_PRIM || type == STAMPFILL_PRIM || type == STAMPCMAP_PRIM)
    {
#define TOO_SMALL	3
	register DWORD		frame_flags;
	register PFRAME_DESC	SrcFramePtr;
    
	SrcFramePtr = (PFRAME_DESC)PrimPtr->Object.Stamp.frame;
	frame_flags = GetFrameFlags (SrcFramePtr);

	dst_x = (_save_stamp.origin.x - FRAME_HOT_X (_CurFramePtr)) << 16;
	dst_y = (_save_stamp.origin.y - FRAME_HOT_Y (_CurFramePtr)) << 16;
	
	w = GetFrameWidth (SrcFramePtr);
	h = GetFrameHeight (SrcFramePtr);
	if (!(frame_flags & X_FLIP))
	    loc_ccb->ccb_HDX = 1 << 20;
	else
	    loc_ccb->ccb_HDX = -1 << 20;
	
	loc_ccb->ccb_VDY = 1 << 16;
	loc_ccb->ccb_HDY = 0;
	loc_ccb->ccb_VDX = 0;

	if (gscale && gscale != (1 << 8) && w > TOO_SMALL && h > TOO_SMALL)
	{
	    int32	dx, dy;

	    loc_ccb->ccb_HDX = (loc_ccb->ccb_HDX * gscale) >> 8;
	    loc_ccb->ccb_VDY = (loc_ccb->ccb_VDY * gscale) >> 8;
	    dx = GetFrameHotX (SrcFramePtr) << 16;
	    dy = GetFrameHotY (SrcFramePtr) << 16;
	    dst_x += dx - ((dx * gscale) >> 8);
	    dst_y += dy - ((dy * gscale) >> 8);
	    if (frame_flags & X_FLIP)
		dst_x += (w * gscale) << 8;
	}
	else if (frame_flags & X_FLIP)
	    dst_x += w << 16;
	
	loc_ccb->ccb_SourcePtr =
		(void *)((BYTE *)SrcFramePtr + SrcFramePtr->DataOffs);
		    
	if (frame_flags & DATA_HARDWARE)
	{
	    CCB		*ccb;
	    long	*p;

	    p = (long *)loc_ccb->ccb_SourcePtr;

#ifdef CEL_DEBUG
printf ("    size is %d\n", *p);	
#endif
	    if (!(ccb = ParseCel ((void *)(p + 1), *p)))
		return;
		
#ifdef CEL_DEBUG
printf ("    Flags 0x%08x  Next 0x%08x\n", ccb->ccb_Flags, ccb->ccb_NextPtr);
printf ("    Source 0x%08x  PLUT 0x%08x\n", ccb->ccb_SourcePtr, ccb->ccb_PLUTPtr);
printf ("    XPos 0x%08x  YPos 0x%08x\n", ccb->ccb_XPos, ccb->ccb_YPos);
printf ("    HDX 0x%08x  HDY 0x%08x\n", ccb->ccb_HDX, ccb->ccb_HDY);
printf ("    VDX 0x%08x  VDY 0x%08x\n", ccb->ccb_VDX, ccb->ccb_VDY);
printf ("    HDDX 0x%08x  HDDY 0x%08x\n", ccb->ccb_HDDX, ccb->ccb_HDDY);
printf ("    PIXC 0x%08x  PRE0 0x%08x\n", ccb->ccb_PIXC, *(long *)ccb->ccb_SourcePtr);
printf ("    Width %d  Height %d\n", ccb->ccb_Width, ccb->ccb_Height);
#endif

	    loc_ccb->ccb_Flags = ccb->ccb_Flags;
//	    CLR_CEL_FLAGS (loc_ccb, CCB_NPABS | CCB_LAST);
	    SET_CEL_FLAGS (loc_ccb, CCB_NPABS | CCB_PPABS | CCB_ACE | CCB_LCE);
	    loc_ccb->ccb_PIXC = ccb->ccb_PIXC;
	    loc_ccb->ccb_PRE0 = ccb->ccb_PRE0;
	    loc_ccb->ccb_PRE1 = ccb->ccb_PRE1;
	    loc_ccb->ccb_SourcePtr = ccb->ccb_SourcePtr;
	    loc_ccb->ccb_PLUTPtr = ccb->ccb_PLUTPtr;

	    if (gPIXC != (PPMPC_MF_4 | PPMPC_SF_4)) // ==> cel mask to be added or subtracted
	    {
		int32	num_den;

		// why?!?!?!?!?!?!?!?!
		SET_CEL_FLAGS (loc_ccb, CCB_NOBLK);

		num_den = gPIXC & (PPMPC_MF_MASK | PPMPC_SF_MASK);
		if (gPIXC & LOC_SUBTRACT)
		{
		    SET_CEL_FLAGS (loc_ccb, CCB_USEAV);
		    loc_ccb->ccb_PIXC = PIXC_DUP (PPMPC_1S_PDC
			    | num_den
			    | PPMPC_2S_CFBD | PPMPC_2D_1 | (1 << 1));
		}
		else
		{
		    loc_ccb->ccb_PIXC = PIXC_DUP (PPMPC_1S_PDC | num_den);
		    if (gPIXC & LOC_COMBINE)
	 		loc_ccb->ccb_PIXC |= PIXC_DUP (PPMPC_2S_CFBD | PPMPC_2D_1);
		}
	    }
	}
	else if (frame_flags & DATA_PACKED)
	{
	    SET_CEL_FLAGS (loc_ccb, CCB_PACKED | CCB_LDPLUT);
	    loc_ccb->ccb_PRE0 =
	    	    PRE0_LITERAL |
		    ((h - PRE0_VCNT_PREFETCH) << PRE0_VCNT_SHIFT) |
		    (PRE0_BPP_8 << PRE0_BPP_SHIFT);
	    if (type != STAMPFILL_PRIM)
	    {
		loc_ccb->ccb_PIXC = PIXC_CODED8;
		loc_ccb->ccb_PLUTPtr = GET_VAR_PLUT (frame_flags & 0xFF);
		memcpy (cccb->plutvals, loc_ccb->ccb_PLUTPtr, sizeof (uint32) * NUMBER_OF_PLUT_UINT32s);
		loc_ccb->ccb_PLUTPtr = (void *)&cccb->plutvals;
	    }
	    else /* STAMPFILL_PRIM */
	    {
		COLOR	color;
		
		color = COLOR_32k (GetPrimColor (PrimPtr));
		if (color & 0x8000)
		{
		    color &= ~0x8000;
		    GET_SOLID_PLUT (loc_ccb, loc_pluts, color);
		    color = COLOR_256 (GetPrimColor (PrimPtr));
		    loc_ccb->ccb_PIXC = PIXC_DUP ((color << PPMPC_MF_SHIFT)
			    | PPMPC_SF_8 | PPMPC_2S_CFBD | PPMPC_2D_1);
		}
		else
		    GET_SOLID_PLUT (loc_ccb, loc_pluts, color);
	    }
	}
	else /* DATA_SCREEN */
	{
	    CLR_CEL_FLAGS (loc_ccb, CCB_PACKED | CCB_LDPLUT);
	    loc_ccb->ccb_PRE0 =
		    PRE0_BGND |
		    PRE0_LINEAR |
	    	    ((h - PRE0_VCNT_PREFETCH) << PRE0_VCNT_SHIFT) |
		    (PRE0_BPP_16 << PRE0_BPP_SHIFT);
	    loc_ccb->ccb_PRE1 =
	    	    PRE1_TLLSB_PDC0 |
	    	    ((w - PRE1_TLHPCNT_PREFETCH) << PRE1_TLHPCNT_SHIFT) |
		    (((((w << 1) + 3) >> 2) - PRE1_WOFFSET_PREFETCH) << PRE1_WOFFSET10_SHIFT);
	    loc_ccb->ccb_PIXC = PIXC_UNCODED16;
	}
    }
#ifdef DEBUG
    else
    {
	PRD (("Unsupported prim type = %d\n", type));
	return;
    }
#endif

    if (clipped_cel)
    {
	dst_x -= _pCurContext->ClipRect.corner.x << 16;
	dst_y -= _pCurContext->ClipRect.corner.y << 16;
    }

    loc_ccb->ccb_XPos = dst_x;
    loc_ccb->ccb_YPos = dst_y;

    add_cel (loc_ccb);
    
#undef loc_ccb
    
    loc_ccb++;
}

#ifdef DEBUG
void
draw_cel (CCB *ccb)
{
#define SET_CLIP(x,y,w,h) \
    do \
    { \
	int32	k; \
	\
	for (k = 0; k < NUMBER_OF_SCREENS; k++) \
	{ \
	    SetClipOrigin (theScreen.sc_BitmapItems[k], 0, 0); \
	    SetClipWidth (theScreen.sc_BitmapItems[k], \
		    w); \
	    SetClipHeight (theScreen.sc_BitmapItems[k], \
		    h); \
	    SetClipOrigin (theScreen.sc_BitmapItems[k], x, y); \
	} \
    } while (0)

    SET_CEL_FLAGS (ccb, CCB_LAST);
    printf ("drawing cel at %d,%d\n", ccb->ccb_XPos >> 16, ccb->ccb_YPos >> 16);
    SET_CLIP (0, 0, SCREEN_WIDTH, SCREEN_HEIGHT);
    DrawCels (theScreen.sc_BitmapItems[theScreen.sc_curScreen], ccb);
}
#endif

static int32	screen_transition;

int
_ThreeDO_batch_cels (int task_driven)
{
//    if (task_driven && screen_transition)
//	return (1);

while (screen_transition == -1)
    Yield ();
    
    // If its a thread task then let main task 
    // draw cels since it has ownership permission.
    if (KernelBase->kb_CurrentTask->t_ThreadTask)
    {
//	if (first_ccb)
	{
	    /* For tasks which draw graphics. */
	    _threedo_add_task (TASK_DRAW_CELS);
	    
	    do
		Yield ();
	    while (first_ccb);
	}
    	return (1);
    }
    
    if (first_ccb)
    {
	Item	BitmapItem;

	if (!(_batch_flags & (BUILD_FRAME | FIND_PAGE)))
	    BitmapItem = theScreen.sc_BitmapItems[theScreen.sc_curScreen];
	else
	{
	    BitmapItem = theScreen.sc_BitmapItems[!theScreen.sc_curScreen];
	
	    if (!(_batch_flags & FIRST_BATCH))
	    {
		_batch_flags |= FIRST_BATCH;
		if (!(_batch_flags & FIND_PAGE))
		{
if (VIEW_X_ADJUST==0 && VIEW_Y_ADJUST==0)
{
		    screen_ccb.ccb_SourcePtr =
			    (void *)theScreen.sc_Bitmaps[theScreen.sc_curScreen]->bm_Buffer;
		    screen_ccb.ccb_XPos = (_old_org_x - _new_org_x) << 16;
		    screen_ccb.ccb_YPos = (_old_org_y - _new_org_y) << 16;
}
else
{
		    render_screen_cel (
	    		    _old_org_x - _new_org_x,
			    _old_org_y - _new_org_y,
			    VIEW_X_ADJUST,
			    VIEW_Y_ADJUST,
			    (void *)theScreen.sc_Bitmaps[theScreen.sc_curScreen]->bm_Buffer,
			    &screen_ccb
			    );
}
#ifdef DOG_DEBUG
if (dog) printf ("copying screen %ld to %ld\n", theScreen.sc_curScreen, !theScreen.sc_curScreen);
#endif
		    DrawCels (BitmapItem, &screen_ccb);
		}
	    }
	}

	// Set the clipping region.
    	if (_pCurContext->ClipRect.extent.width != 0)
    	{
	    int32	y;

#ifdef DOG_DEBUG
if (dog)
{
    printf ("clipping is %ld,%ld -- ",
	    _pCurContext->ClipRect.corner.x,
	    _pCurContext->ClipRect.corner.y);
    printf ("%ld,%ld\n",
	    _pCurContext->ClipRect.extent.width,
	    _pCurContext->ClipRect.extent.height);
}
#endif
	    SetClipOrigin (BitmapItem, 0, 0);
	    y = _pCurContext->ClipRect.corner.y;
	    SetClipWidth (BitmapItem, _pCurContext->ClipRect.extent.width);
	    SetClipHeight (BitmapItem, _pCurContext->ClipRect.extent.height + (y & 1));
	    SetClipOrigin (BitmapItem,
		    _pCurContext->ClipRect.corner.x + VIEW_X_ADJUST,
		    (y & ~1) + VIEW_Y_ADJUST);
    	}

	SET_CEL_FLAGS (cur_ccb, CCB_LAST);
#ifdef DOG_DEBUG
if (dog)
{
    if (!(_batch_flags & (BUILD_FRAME | FIND_PAGE)))
	printf ("drawing to current screen (%ld)\n", theScreen.sc_curScreen);
    else
	printf ("drawing to other screen (%ld)\n", !theScreen.sc_curScreen);
}
#endif
	DrawCels (BitmapItem, first_ccb);
	loc_ccb = cel_array;
	
	// Reset the clipping region to full screen.
    	if (_pCurContext->ClipRect.extent.width != 0)
    	{
  	    SetClipOrigin (BitmapItem, 0, 0);
   	    SetClipWidth (BitmapItem, VIEW_WIDTH);
   	    SetClipHeight (BitmapItem, VIEW_HEIGHT);
if (VIEW_X_ADJUST != 0 || VIEW_Y_ADJUST != 0)
{
   	    SetClipOrigin (BitmapItem, VIEW_X_ADJUST, VIEW_Y_ADJUST);
}
	}
    }

    if (_batch_depth == 0 && (_batch_flags & BUILD_FRAME))
    {
	if (_batch_flags & FIRST_BATCH)
	{
	    theScreen.sc_curScreen = !theScreen.sc_curScreen;
	    
	    if (_batch_flags & CYCLE_PENDING)
	    {
		_batch_flags |= CUR_CYCLED;
		_threedo_set_clut (1 << theScreen.sc_curScreen);
	    }
	    else
		_batch_flags &= ~CUR_CYCLED;

#ifdef DOG_DEBUG
if (dog) printf ("making other screen (%ld) current\n", theScreen.sc_curScreen);
#endif
if (screen_transition)
{
    void	ScreenTransition (int, void *);
    
    ScreenTransition (screen_transition, 0);
}
else
	    DisplayScreen (theScreen.sc_Screens[theScreen.sc_curScreen], 0);
	    
	    if (_batch_flags & CYCLE_PENDING)
	    {
		_threedo_set_clut (1 << !theScreen.sc_curScreen);
		_batch_flags &= ~CYCLE_PENDING;
	    }
	}
	
	_batch_flags &= ~(BUILD_FRAME | FIND_PAGE | FIRST_BATCH);

	_old_org_x = _new_org_x;
	_old_org_y = _new_org_y;
    }

    first_ccb = 0;

    return (1);
}

void
FlushGraphics (int unused)
{
//    _batch_flags |= BUILD_FRAME;
    if (first_ccb)
	_ThreeDO_batch_cels (0);
}

Item
GetScreenGroup ()
{
    return (theScreenGroup);
}

void
DisplayCurScreen ()
{
    DisplayScreen (theScreen.sc_Screens[theScreen.sc_curScreen], 0);
}

void
StoreCurScreens ()
{
#ifdef NEED_TO_STORE
    screen_ccb.ccb_XPos = screen_ccb.ccb_YPos = 0;
    screen_ccb.ccb_SourcePtr =
	    (void *)theScreen.sc_Bitmaps[theScreen.sc_curScreen]->bm_Buffer;
    DrawCels (theScreen.sc_BitmapItems[2], &screen_ccb);
    screen_ccb.ccb_SourcePtr =
	    (void *)theScreen.sc_Bitmaps[!theScreen.sc_curScreen]->bm_Buffer;
    DrawCels (theScreen.sc_BitmapItems[3], &screen_ccb);
#endif
}

void
RestoreCurScreens ()
{
#ifdef NEED_TO_STORE
    screen_ccb.ccb_XPos = screen_ccb.ccb_YPos = 0;
    screen_ccb.ccb_SourcePtr =
	    (void *)theScreen.sc_Bitmaps[2]->bm_Buffer;
    DrawCels (theScreen.sc_BitmapItems[theScreen.sc_curScreen], &screen_ccb);
    screen_ccb.ccb_SourcePtr =
	    (void *)theScreen.sc_Bitmaps[3]->bm_Buffer;
    DrawCels (theScreen.sc_BitmapItems[!theScreen.sc_curScreen], &screen_ccb);
#endif
    
    DisplayScreen (theScreen.sc_Screens[theScreen.sc_curScreen], 0);
}

#if EXTRA_SCREEN
static ubyte	extra_flags;

#define USE_OTHER_EXTRA		(1<<0)
#define LOAD_EXTRA_WAIT		(1<<1)

void
SetGraphicUseOtherExtra (int32 other)
{
    if (other)
	extra_flags |= USE_OTHER_EXTRA;
    else
	extra_flags &= ~USE_OTHER_EXTRA;
}

void
LoadIntoExtraScreen (PRECT r)
{
    int32		i;
    RECT		locRect;

    if (KernelBase->kb_CurrentTask->t_ThreadTask)
    {
	extra_flags |= LOAD_EXTRA_WAIT;
	_threedo_add_task (TASK_COPY_TO_EXTRA);
	do
	{
	    Yield ();
	} while (extra_flags & LOAD_EXTRA_WAIT);

	return;
    }
    else if (!r)
    {
	locRect.corner.x = locRect.corner.y = 0;
	locRect.extent.width = SCREEN_WIDTH;
	locRect.extent.height = SCREEN_HEIGHT;
	r = &locRect;
    }

    i = (_batch_flags & GRAB_OTHER) ? !theScreen.sc_curScreen : theScreen.sc_curScreen;

#ifdef DOG_DEBUG
if (dog) printf ("Loading extra from screen %ld\n", i);
#endif
    {
	CCB	copy_ccb;
	int32	x, y;
	
	copy_ccb = screen_ccb;
	
	x = r->corner.x;
	y = r->corner.y;
	copy_ccb.ccb_SourcePtr = (void *)((uint32 *)theScreen.sc_Bitmaps[i]->bm_Buffer
		+ x + (y * (SCREEN_WIDTH >> 1)));
	copy_ccb.ccb_XPos = x << 16;
	copy_ccb.ccb_YPos = y << 16;
	copy_ccb.ccb_PRE0 =
		PRE0_BGND
		| PRE0_LINEAR
		| (PRE0_BPP_16 << PRE0_BPP_SHIFT)
		| ((((r->extent.height + 1) >> 1) - PRE0_VCNT_PREFETCH) << PRE0_VCNT_SHIFT);
	copy_ccb.ccb_PRE1 =
		PRE1_LRFORM
		| PRE1_TLLSB_PDC0
    		| (((r->extent.width) - PRE1_TLHPCNT_PREFETCH) << PRE1_TLHPCNT_SHIFT)
		| ((SCREEN_WIDTH - PRE1_WOFFSET_PREFETCH) << PRE1_WOFFSET10_SHIFT);
	if (extra_flags & USE_OTHER_EXTRA)
	    DrawCels (theScreen.sc_BitmapItems[EXTRA_SCREEN + 1], &copy_ccb);
	else
	    DrawCels (theScreen.sc_BitmapItems[EXTRA_SCREEN], &copy_ccb);
    }
    
    extra_flags &= ~LOAD_EXTRA_WAIT;
}

void
_threedo_copy_to_extra ()
{
    LoadIntoExtraScreen (&_pCurContext->ClipRect);
}

void
DrawFromExtraScreen (int32 x, int32 y)
{
    uint32	num_den;
    
    num_den = gPIXC & (PPMPC_MF_MASK | PPMPC_SF_MASK);
    if (gPIXC & LOC_SUBTRACT)
    {
	SET_CEL_FLAGS (&extra_ccb, CCB_USEAV);
	extra_ccb.ccb_PIXC = PIXC_DUP (PPMPC_1S_PDC
		| num_den
		| PPMPC_2S_CFBD | PPMPC_2D_1 | (1 << 1));
    }
    else
    {
	CLR_CEL_FLAGS (&extra_ccb, CCB_USEAV);
	extra_ccb.ccb_PIXC = PIXC_DUP (PPMPC_1S_PDC | num_den);
	if (gPIXC & LOC_COMBINE)
 	    extra_ccb.ccb_PIXC |= PIXC_DUP (PPMPC_2S_CFBD | PPMPC_2D_1);
    }

    if (extra_flags & USE_OTHER_EXTRA)
	extra_ccb.ccb_SourcePtr
		= (void *)theScreen.sc_Bitmaps[EXTRA_SCREEN + 1]->bm_Buffer;
    else
	extra_ccb.ccb_SourcePtr
		= (void *)theScreen.sc_Bitmaps[EXTRA_SCREEN]->bm_Buffer;
    
    extra_ccb.ccb_XPos = (-x) << 16;
    extra_ccb.ccb_YPos = (-y) << 16;
    add_cel (&extra_ccb);
}

#define OTHER_SCREEN(s)	(5 - (s))

#define CLEAR_SCREEN(s)	\
	do \
	{ \
	    DrawCels (theScreen.sc_BitmapItems[s], &black_ccb); \
	} while (0)

#define DISPLAY_SCREEN(s) \
	do \
	{ \
	    DisplayScreen (theScreen.sc_Screens[s], 0); \
	} while (0)
	
#define SLEEP(tix)	\
	do \
	{ \
	    int32	_t; \
	    \
	    _t = GetTimeCounter () + (tix); \
	    do \
		Yield (); \
	    while (GetTimeCounter () < _t); \
	} while (0)
	
void
ScreenTransition (int transition, PRECT pRect)
{
#define NUM_FRAMES	20
#define NUM_SLEEP_TICKS	2
    CCB		ccb, ccb2, black_ccb;
    int32	n, c;
    static RECT	locRect;

    // this assumes that the screen we're looking at is !theScreen.sc_curScreen
    // the screen we're transitioning to is theScreen.sc_curScreen.
    // we use screens 3 and 4 for transition effects

    if (_batch_depth != 0)
    {
	// not done drawing to non-visible screen, so simply set up
	// for transition out of batch_cels above
	
	screen_transition = transition;
	if (pRect)
	    locRect = *pRect;
	else
	    locRect.extent.width = 0;
	return;
    }
    else if (screen_transition == 0)
    {
	// we're transitioning directly (as opposed to out of batch_cels above)
	// so theScreen.sc_curScreen is current (as opposed to non-visible,
	// which the code below assumes)
	
	theScreen.sc_curScreen = !theScreen.sc_curScreen;
    }

#ifdef DOG_DEBUG
if (dog) printf ("transitioning screen %ld to %ld\n", !theScreen.sc_curScreen, theScreen.sc_curScreen);
#endif
    if (!pRect && locRect.extent.width)
	pRect = &locRect;
	
    if (pRect && (pRect->extent.height & 1))
	pRect->extent.height++;
	
    screen_transition = -1;

    c = EXTRA_SCREEN;    

    black_ccb.ccb_Flags = (CEL_FLAGS | CCB_LAST) & ~CCB_PPABS;
    black_ccb.ccb_XPos = black_ccb.ccb_YPos = 0;
    black_ccb.ccb_HDX = (SCREEN_WIDTH) << 20;
    black_ccb.ccb_VDY = (SCREEN_HEIGHT) << 16;
    black_ccb.ccb_HDY = black_ccb.ccb_VDX = black_ccb.ccb_HDDX = black_ccb.ccb_HDDY = 0;
    black_ccb.ccb_PIXC = PIXC_UNCODED16;
    black_ccb.ccb_PRE0 = PRE0_BGND
	    | PRE0_LINEAR
	    | (PRE0_BPP_16 << PRE0_BPP_SHIFT);
    black_ccb.ccb_PRE1 = PRE1_LRFORM
	    | PRE1_TLLSB_PDC0;
    black_ccb.ccb_Width = 0x80008000;
    black_ccb.ccb_SourcePtr = (CelData *)&black_ccb.ccb_Width;

    switch (transition)
    {
#if 0
	case 1: // zoom out, zoom in
	{
	    int32	cx, cy, hw, hh;
	    
	    ccb = screen_ccb;

	    SET_CEL_FLAGS (&ccb, CCB_LAST);
    
	    if (pRect)
	    {
		ccb.ccb_SourcePtr =
			(CelData *)theScreen.sc_Bitmaps[theScreen.sc_curScreen]->bm_Buffer;
		ccb.ccb_XPos = ccb.ccb_YPos = 0;
		
		ccb2 = ccb;
		
		DrawCels (theScreen.sc_BitmapItems[2], &ccb);
		DrawCels (theScreen.sc_BitmapItems[3], &ccb);
		
		hw = pRect->extent.width >> 1;
		hh = pRect->extent.height >> 1;
		cx = pRect->corner.x + hw;
		cy = pRect->corner.y + hh;
		ccb.ccb_SourcePtr = (CelData *)((uint32 *)theScreen.sc_Bitmaps[!theScreen.sc_curScreen]->bm_Buffer
			+ pRect->corner.x + pRect->corner.y * (SCREEN_WIDTH >> 1));
		ccb.ccb_PRE0 =
			PRE0_BGND
			| PRE0_LINEAR
			| (PRE0_BPP_16 << PRE0_BPP_SHIFT)
			| ((hh - PRE0_VCNT_PREFETCH) << PRE0_VCNT_SHIFT);
		ccb.ccb_PRE1 =
			PRE1_LRFORM
			| PRE1_TLLSB_PDC0
			| (((hw << 1) - PRE1_TLHPCNT_PREFETCH) << PRE1_TLHPCNT_SHIFT)
			| ((SCREEN_WIDTH - PRE1_WOFFSET_PREFETCH) << PRE1_WOFFSET10_SHIFT);
	    }
	    else
	    {
		ccb.ccb_SourcePtr = (CelData *)theScreen.sc_Bitmaps[!theScreen.sc_curScreen]->bm_Buffer;
		hw = SCREEN_WIDTH >> 1;
		hh = SCREEN_HEIGHT >> 1;
		cx = hw;
		cy = hh;
	    }
	    
	    n = NUM_FRAMES / 2;
	    do
	    {
		ccb.ccb_XPos = (cx << 16) - (((hw * n) << 16) / (NUM_FRAMES / 2));
		ccb.ccb_YPos = (cy << 16) - (((hh * n) << 16) / (NUM_FRAMES / 2));
		ccb.ccb_HDX = (n << 20) / (NUM_FRAMES / 2);
		ccb.ccb_VDY = (n << 16) / (NUM_FRAMES / 2);
		if (pRect)
		    DrawCels (theScreen.sc_BitmapItems[c], &ccb2);
		else
		    CLEAR_SCREEN (c);
		DrawCels (theScreen.sc_BitmapItems[c], &ccb);
		DISPLAY_SCREEN (c);
		SLEEP (NUM_SLEEP_TICKS);
		c = OTHER_SCREEN (c);
	    } while (--n);

	    if (pRect)
		ccb.ccb_SourcePtr =
			(CelData *)((uint32 *)theScreen.sc_Bitmaps[theScreen.sc_curScreen]->bm_Buffer
			+ pRect->corner.x + pRect->corner.y * (SCREEN_WIDTH >> 1));
	    else
		ccb.ccb_SourcePtr =
			(CelData *)theScreen.sc_Bitmaps[theScreen.sc_curScreen]->bm_Buffer;

	    if (!pRect)
	    {
		n = 1;
		do
		{
		    ccb.ccb_XPos = (cx << 16) - (((hw * n) << 16) / (NUM_FRAMES / 2));
		    ccb.ccb_YPos = (cy << 16) - (((hh * n) << 16) / (NUM_FRAMES / 2));
		    ccb.ccb_HDX = (n << 20) / (NUM_FRAMES / 2);
		    ccb.ccb_VDY = (n << 16) / (NUM_FRAMES / 2);
		    CLEAR_SCREEN (c);
		    DrawCels (theScreen.sc_BitmapItems[c], &ccb);
		    DISPLAY_SCREEN (c);
		    SLEEP (NUM_SLEEP_TICKS);
		    c = OTHER_SCREEN (c);
		} while (n++ < (NUM_FRAMES / 2));
	    }
    
	    DrawCels (theScreen.sc_BitmapItems[c], &ccb2);
	    DISPLAY_SCREEN (c);
	    SLEEP (NUM_SLEEP_TICKS);
	    c = OTHER_SCREEN (c);
	    
	    break;
	}
	case 2: // push off to right
	{
	    ccb = screen_ccb;
	    ccb2 = screen_ccb;

	    SET_CEL_FLAGS (&ccb, CCB_LAST);
	    SET_CEL_FLAGS (&ccb2, CCB_LAST);
	    
	    ccb.ccb_XPos = ccb.ccb_YPos = 0;
	    ccb2.ccb_XPos = ccb2.ccb_YPos = 0;
	    
	    ccb.ccb_SourcePtr = (CelData *)theScreen.sc_Bitmaps[!theScreen.sc_curScreen]->bm_Buffer;
	    ccb2.ccb_SourcePtr = (CelData *)theScreen.sc_Bitmaps[theScreen.sc_curScreen]->bm_Buffer;
	    n = NUM_FRAMES - 1;
	    do
	    {
		int32	x;
		
		CLEAR_SCREEN (c);
			
		x = SCREEN_WIDTH - ((SCREEN_WIDTH * n) / NUM_FRAMES);
		
		SetClipWidth (theScreen.sc_BitmapItems[c], (SCREEN_WIDTH - x));
		SetClipOrigin (theScreen.sc_BitmapItems[c], x, 0);
		DrawCels (theScreen.sc_BitmapItems[c], &ccb);
		
		SetClipOrigin (theScreen.sc_BitmapItems[c], 0, 0);
		SetClipWidth (theScreen.sc_BitmapItems[c], x);
		ccb2.ccb_XPos = (x - SCREEN_WIDTH) << 16;
		DrawCels (theScreen.sc_BitmapItems[c], &ccb2);
		
		DISPLAY_SCREEN (c);
		SLEEP (NUM_SLEEP_TICKS);
		c = OTHER_SCREEN (c);
	    } while (n--);

	    break;
	}
#endif
	case 3: // fade old into new
	default:
	{
	    CCB	ccb3;
	    
	    ccb = screen_ccb;

	    if (pRect)
	    {
		ccb.ccb_XPos = ccb.ccb_YPos = 0;
		ccb.ccb_SourcePtr = (CelData *)theScreen.sc_Bitmaps[theScreen.sc_curScreen]->bm_Buffer;
		DrawCels (theScreen.sc_BitmapItems[2], &ccb);
		DrawCels (theScreen.sc_BitmapItems[3], &ccb);

		ccb.ccb_XPos = pRect->corner.x << 16;
		ccb.ccb_YPos = pRect->corner.y << 16;
		ccb.ccb_SourcePtr =
			(CelData *)((uint32 *)theScreen.sc_Bitmaps[theScreen.sc_curScreen]->bm_Buffer
			+ pRect->corner.x + pRect->corner.y * (SCREEN_WIDTH >> 1));
		ccb.ccb_PRE0 =
			PRE0_BGND
			| PRE0_LINEAR
			| (PRE0_BPP_16 << PRE0_BPP_SHIFT)
			| (((pRect->extent.height >> 1) - PRE0_VCNT_PREFETCH) << PRE0_VCNT_SHIFT);
		ccb.ccb_PRE1 =
			PRE1_LRFORM
			| PRE1_TLLSB_PDC0
			| ((pRect->extent.width - PRE1_TLHPCNT_PREFETCH) << PRE1_TLHPCNT_SHIFT)
			| ((SCREEN_WIDTH - PRE1_WOFFSET_PREFETCH) << PRE1_WOFFSET10_SHIFT);

		ccb2 = ccb;
		ccb2.ccb_SourcePtr =
			(CelData *)((uint32 *)theScreen.sc_Bitmaps[!theScreen.sc_curScreen]->bm_Buffer
			+ pRect->corner.x + pRect->corner.y * (SCREEN_WIDTH >> 1));
	    }
	    else
	    {
		ccb.ccb_XPos = ccb.ccb_YPos = 0;
		ccb.ccb_SourcePtr = (CelData *)theScreen.sc_Bitmaps[theScreen.sc_curScreen]->bm_Buffer;
		
		ccb2 = ccb;
		ccb2.ccb_SourcePtr = (CelData *)theScreen.sc_Bitmaps[!theScreen.sc_curScreen]->bm_Buffer;
	    }
	    
	    ccb3 = ccb2;
	    SET_CEL_FLAGS (&ccb3, CCB_USEAV);
	    
	    ccb2.ccb_NextPtr = &ccb3;
	    CLR_CEL_FLAGS (&ccb2, CCB_LAST);
	    ccb3.ccb_NextPtr = &ccb;
	    CLR_CEL_FLAGS (&ccb3, CCB_LAST);
	    SET_CEL_FLAGS (&ccb, CCB_LAST);

SuspendTasking ();
	    n = 0;
	    do
	    {
		// draw old at (n + 1)/16th's
		ccb2.ccb_PIXC = PIXC_DUP (PPMPC_1S_PDC | PPMPC_MS_CCB
			| (n << PPMPC_MF_SHIFT) | PPMPC_SF_16);
		// draw old at 16/16th's, combine with old at -(n + 1)/16th's
		// to get old at (16 - (n + 1))/16th's
		ccb3.ccb_PIXC = PIXC_DUP (PPMPC_1S_PDC | PPMPC_MS_CCB
			| PPMPC_MF_8 | PPMPC_SF_8
			| PPMPC_2S_CFBD | PPMPC_2D_1 | (1 << 1));
		// draw new at (n + 1)/16th's, combine with old at (16 - (n + 1))/16th's
		ccb.ccb_PIXC = PIXC_DUP (PPMPC_1S_PDC | PPMPC_MS_CCB
			| (n << PPMPC_MF_SHIFT) | PPMPC_SF_16
			| PPMPC_2S_CFBD | PPMPC_2D_1);

		WaitIO (VBLreq);
		DrawCels (theScreen.sc_BitmapItems[c], &ccb2);

		WaitVBLDefer (VBLreq, 1);
		DISPLAY_SCREEN (c);
		c = OTHER_SCREEN (c);
	    } while (n++ < 7);

	    ccb3 = ccb;
	    SET_CEL_FLAGS (&ccb3, CCB_USEAV);
	    
	    ccb.ccb_NextPtr = &ccb3;
	    CLR_CEL_FLAGS (&ccb, CCB_LAST);
	    ccb3.ccb_NextPtr = &ccb2;
	    CLR_CEL_FLAGS (&ccb3, CCB_LAST);
	    SET_CEL_FLAGS (&ccb2, CCB_LAST);
	    
	    n = 7;
	    do
	    {
		// draw new at (n + 1)/16th's
		ccb.ccb_PIXC = PIXC_DUP (PPMPC_1S_PDC | PPMPC_MS_CCB
			| (n << PPMPC_MF_SHIFT) | PPMPC_SF_16);
		// draw old at 16/16th's, combine with old at -(n + 1)/16th's
		// to get old at (16 - (n + 1))/16th's
		ccb3.ccb_PIXC = PIXC_DUP (PPMPC_1S_PDC | PPMPC_MS_CCB
			| PPMPC_MF_8 | PPMPC_SF_8
			| PPMPC_2S_CFBD | PPMPC_2D_1 | (1 << 1));
		// draw new at (n + 1)/16th's, combine with old at (16 - (n + 1))/16th's
		ccb2.ccb_PIXC = PIXC_DUP (PPMPC_1S_PDC | PPMPC_MS_CCB
			| (n << PPMPC_MF_SHIFT) | PPMPC_SF_16
			| PPMPC_2S_CFBD | PPMPC_2D_1);

		WaitIO (VBLreq);
		DrawCels (theScreen.sc_BitmapItems[c], &ccb);
		
		WaitVBLDefer (VBLreq, 1);
		DISPLAY_SCREEN (c);
		c = OTHER_SCREEN (c);
	    } while (--n);

	    ccb.ccb_PIXC = PIXC_DUP (PPMPC_1S_PDC | PPMPC_MS_CCB
		    | PPMPC_MF_8 | PPMPC_SF_8);
	    SET_CEL_FLAGS (&ccb, CCB_LAST);
	    
	    WaitIO (VBLreq);
	    DrawCels (theScreen.sc_BitmapItems[c], &ccb);
		
	    DISPLAY_SCREEN (c);
	    c = OTHER_SCREEN (c);
ResumeTasking ();	    
	    break;
	}
#if 0
	case 4: // spin out, spin in
	{
	    Point	pts[4];
	    int32	dx, dy, a, da, cx, cy, hw, hh;
	
	    ccb = screen_ccb;
	    if (pRect)
	    {
		ccb.ccb_Width = pRect->extent.width;
		ccb.ccb_Height = pRect->extent.height;
		hw = pRect->extent.width >> 1;
		hh = pRect->extent.height >> 1;
		cx = pRect->corner.x + hw;
		cy = pRect->corner.y + hh;
	    }
	    else
	    {
		ccb.ccb_Width = SCREEN_WIDTH;
		ccb.ccb_Height = SCREEN_HEIGHT;
		cx = SCREEN_WIDTH >> 1;
		cy = SCREEN_HEIGHT >> 1;
		hw = SCREEN_WIDTH >> 1;
		hh = SCREEN_HEIGHT >> 1;
	    }

	    SET_CEL_FLAGS (&ccb, CCB_LAST);
    
	    if (pRect)
	    {
		ccb.ccb_SourcePtr = (CelData *)((uint32 *)theScreen.sc_Bitmaps[!theScreen.sc_curScreen]->bm_Buffer
			+ (cx - hw) + (cy - hh) * (SCREEN_WIDTH >> 1));
		ccb.ccb_PRE0 =
			PRE0_BGND
			| PRE0_LINEAR
			| (PRE0_BPP_16 << PRE0_BPP_SHIFT)
			| ((hh - PRE0_VCNT_PREFETCH) << PRE0_VCNT_SHIFT);
		ccb.ccb_PRE1 =
			PRE1_LRFORM
			| PRE1_TLLSB_PDC0
			| ((ccb.ccb_Width - PRE1_TLHPCNT_PREFETCH) << PRE1_TLHPCNT_SHIFT)
			| ((SCREEN_WIDTH - PRE1_WOFFSET_PREFETCH) << PRE1_WOFFSET10_SHIFT);
		ccb2 = screen_ccb;
		SET_CEL_FLAGS (&ccb2, CCB_LAST);
		ccb2.ccb_SourcePtr = (CelData *)theScreen.sc_Bitmaps[theScreen.sc_curScreen]->bm_Buffer;
		ccb2.ccb_XPos = ccb2.ccb_YPos = 0;
	    }
	    else
		ccb.ccb_SourcePtr = (CelData *)theScreen.sc_Bitmaps[!theScreen.sc_curScreen]->bm_Buffer;

	    n = NUM_FRAMES / 2;
	    do
	    {
		dx = ((hw << 16) * n / (NUM_FRAMES / 2));
		dy = ((hh << 16) * n / (NUM_FRAMES / 2));

		da = (256 - (256 * n / (NUM_FRAMES / 2))) << 16;
		
		a = Atan2F16 (-dx, -dy);
		a += da;
		pts[0].pt_X = (cx << 16) + (dx >> 16) * CosF16 (a);
		pts[0].pt_Y = (cy << 16) + (dy >> 16) * SinF16 (a);
		a = Atan2F16 (dx, -dy);
		a += da;
		pts[1].pt_X = (cx << 16) + (dx >> 16) * CosF16 (a);
		pts[1].pt_Y = (cy << 16) + (dy >> 16) * SinF16 (a);
		a = Atan2F16 (dx, dy);
		a += da;
		pts[2].pt_X = (cx << 16) + (dx >> 16) * CosF16 (a);
		pts[2].pt_Y = (cy << 16) + (dy >> 16) * SinF16 (a);
		a = Atan2F16 (-dx, dy);
		a += da;
		pts[3].pt_X = (cx << 16) + (dx >> 16) * CosF16 (a);
		pts[3].pt_Y = (cy << 16) + (dy >> 16) * SinF16 (a);
		
		myMapCel (&ccb, pts);

		if (pRect)
		    DrawCels (theScreen.sc_BitmapItems[c], &ccb2);
		else
		    CLEAR_SCREEN (c);
		DrawCels (theScreen.sc_BitmapItems[c], &ccb);
		DISPLAY_SCREEN (c);
		SLEEP (NUM_SLEEP_TICKS);
		c = OTHER_SCREEN (c);
	    } while (--n);

	    ccb.ccb_SourcePtr = (CelData *)theScreen.sc_Bitmaps[theScreen.sc_curScreen]->bm_Buffer;
	if (!pRect)
	{
	    n = 1;
	    do
	    {
		dx = ((hw << 16) * n / (NUM_FRAMES / 2));
		dy = ((hh << 16) * n / (NUM_FRAMES / 2));

		da = ((256 * n / (NUM_FRAMES / 2)) - 256) << 16;
		
		a = Atan2F16 (-dx, -dy);
		a += da;
		pts[0].pt_X = (cx << 16) + (dx >> 16) * CosF16 (a);
		pts[0].pt_Y = (cy << 16) + (dy >> 16) * SinF16 (a);
		a = Atan2F16 (dx, -dy);
		a += da;
		pts[1].pt_X = (cx << 16) + (dx >> 16) * CosF16 (a);
		pts[1].pt_Y = (cy << 16) + (dy >> 16) * SinF16 (a);
		a = Atan2F16 (dx, dy);
		a += da;
		pts[2].pt_X = (cx << 16) + (dx >> 16) * CosF16 (a);
		pts[2].pt_Y = (cy << 16) + (dy >> 16) * SinF16 (a);
		a = Atan2F16 (-dx, dy);
		a += da;
		pts[3].pt_X = (cx << 16) + (dx >> 16) * CosF16 (a);
		pts[3].pt_Y = (cy << 16) + (dy >> 16) * SinF16 (a);
		
		myMapCel (&ccb, pts);

		CLEAR_SCREEN (c);
		DrawCels (theScreen.sc_BitmapItems[c], &ccb);
		DISPLAY_SCREEN (c);
		SLEEP (NUM_SLEEP_TICKS);
		c = OTHER_SCREEN (c);
	    } while (++n < (NUM_FRAMES / 2));
	}

	    ccb = screen_ccb;
	    ccb.ccb_XPos = ccb.ccb_YPos = 0;
	    ccb.ccb_SourcePtr = (CelData *)theScreen.sc_Bitmaps[theScreen.sc_curScreen]->bm_Buffer;

	    CLEAR_SCREEN (c);
	    DrawCels (theScreen.sc_BitmapItems[c], &ccb);
	    DISPLAY_SCREEN (c);
	    SLEEP (NUM_SLEEP_TICKS);
	    c = OTHER_SCREEN (c);
    
	    break;
	}
	case 5: // overwrite from top
	{
	    ccb = screen_ccb;
	    ccb2 = screen_ccb;

	    SET_CEL_FLAGS (&ccb, CCB_LAST);
	    SET_CEL_FLAGS (&ccb2, CCB_LAST);
	    
	    ccb.ccb_XPos = ccb.ccb_YPos = 0;
	    ccb2.ccb_XPos = ccb2.ccb_YPos = 0;
	    
	    ccb.ccb_SourcePtr = (CelData *)theScreen.sc_Bitmaps[!theScreen.sc_curScreen]->bm_Buffer;
	    ccb2.ccb_SourcePtr = (CelData *)theScreen.sc_Bitmaps[theScreen.sc_curScreen]->bm_Buffer;
	    n = NUM_FRAMES - 1;
	    do
	    {
		int32	y;
		
		SetClipHeight (theScreen.sc_BitmapItems[c], SCREEN_HEIGHT);
		
		CLEAR_SCREEN (c);
			
		DrawCels (theScreen.sc_BitmapItems[c], &ccb);
		
		y = SCREEN_HEIGHT - ((SCREEN_HEIGHT * n) / NUM_FRAMES);
		
		SetClipHeight (theScreen.sc_BitmapItems[c], y);
		DrawCels (theScreen.sc_BitmapItems[c], &ccb2);
		
		DISPLAY_SCREEN (c);
		SLEEP (NUM_SLEEP_TICKS);
		c = OTHER_SCREEN (c);
	    } while (n--);

	    break;
	}
#endif
    }
    
    DISPLAY_SCREEN (theScreen.sc_curScreen);
    
    screen_transition = 0;
}
#endif

void
FillInScreenContext (ScreenContext *sc)
{
    int	i;
    
    for (i = 0; i < sc->sc_nScreens; i++)
    {
	sc->sc_Screens[i] = theScreen.sc_Screens[i];
	sc->sc_BitmapItems[i] = theScreen.sc_BitmapItems[i];
	sc->sc_Bitmaps[i] = theScreen.sc_Bitmaps[i];
    }
    
    sc->sc_nFrameBufferPages = theScreen.sc_nFrameBufferPages;
    sc->sc_nFrameByteCount = theScreen.sc_nFrameByteCount;
}


