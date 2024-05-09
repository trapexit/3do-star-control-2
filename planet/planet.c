#include "Portfolio.h"
#include "Init3DO.h"
#include "Parse3DO.h"
#include "Utils3DO.h"
#include "filefunctions.h"
#include "BlockFile.h"

#define WIDTH		210//(chrccb->ccb_Width)

#define XLATE_TAB_SIZE	(256 + 6)
#define CMAP_SIZE	(3 * 128 + 2)

#define INITIAL_DIRECTORY	"$boot"

#ifdef DEBUG
#define PRD(s)		printf s
#define SYSERR(e)	PrintfSysErr (e)
#else /* DEBUG */
#define PRD(s)
#define SYSERR(e)
#endif /* DEBUG */

#define EXTRA_SCREEN		2

#define VIEW_WIDTH		320
#define VIEW_HEIGHT		240

#if EXTRA_SCREEN
#define NUMBER_OF_SCREENS	3
#else
#define NUMBER_OF_SCREENS	2
#endif

#define SHADOW_COLOR		(0x18c6) // Shadow color... sleazy

#define PIXC_DUP(v)		(((v) << PPMP_0_SHIFT) | ((v) << PPMP_1_SHIFT))
#define PIXC_UNCODED16		(PIXC_DUP (PPMPC_MF_8 | PPMPC_SF_8))

#define SET_CEL_FLAGS(c,f)	((c)->ccb_Flags |= (f))
#define CLR_CEL_FLAGS(c,f)	((c)->ccb_Flags &= ~(f))

#define CEL_FLAGS \
	(CCB_SPABS | CCB_PPABS | CCB_YOXY | CCB_ACW | CCB_ACCW | CCB_LDSIZE | CCB_CCBPRE \
	| CCB_LDPRS | CCB_LDPPMP | CCB_ACE | CCB_LCE | CCB_PLUTPOS | CCB_BGND)

#define NUM_NORMAL_CELS		500

#define NUMBER_OF_CLUTVALS	32

#define SCREEN_WIDTH		320
#define SCREEN_HEIGHT		240

static CCB		screen_ccb;
static CCB		extra_ccb;

typedef struct
{
    int32 	sc_curScreen;
    int32 	sc_nFrameBufferPages;
    int32 	sc_nFrameByteCount;
    Item 	sc_Screens[NUMBER_OF_SCREENS];
    Item	sc_BitmapItems[NUMBER_OF_SCREENS];
    Bitmap	*sc_Bitmaps[NUMBER_OF_SCREENS];
} ourScreenContext;

static ourScreenContext	theScreen;
static Item		theScreenGroup;

static CCB	*cur_ccb;
static CCB	*cel_array;
static CCB	*final_ccb;

static Item	VRAMIOReq;

static void
BlackenScreen (uint32 mask, int32 frame_ct)
{
    int32		i;
    uint32	colorEntries[NUMBER_OF_CLUTVALS], *ce;

    ce = colorEntries;
    for (i = 0; i < NUMBER_OF_CLUTVALS; i++)
	*ce++ = MakeCLUTColorEntry (i, 0, 0, 0);
	
    for (i = 0; i < NUMBER_OF_SCREENS; i++)
    {
	if (mask & (1UL << i))
    	    SetScreenColors (theScreen.sc_Screens[i],
 		    colorEntries, NUMBER_OF_CLUTVALS);
    }
	
    SetScreenColor (0, MakeCLUTColorEntry (NUMBER_OF_CLUTVALS, 0, 0, 0));
}

static void
LightenScreen (uint32 mask, int32 frame_ct)
{
    int32		i;
    uint32	colorEntries[NUMBER_OF_CLUTVALS], *ce;

    ce = colorEntries;
    for (i = 0; i < NUMBER_OF_CLUTVALS; i++)
    {
	ubyte	color;
	
	color = (ubyte)((i << 3) | (i >> 2));
	*ce++ = MakeCLUTColorEntry (i, color, color, color);
    }
	
    for (i = 0; i < NUMBER_OF_SCREENS; i++)
    {
	if (mask & (1UL << i))
    	    SetScreenColors (theScreen.sc_Screens[i],
 		    colorEntries, NUMBER_OF_CLUTVALS);
    }

// ??????????
    SetScreenColor (0, MakeCLUTColorEntry (NUMBER_OF_CLUTVALS, 0, 0, 0));
}

static bool
ourOpenGraphics (ourScreenContext *sc, Item *gItem, int32 h)
{
    Int32	i, w;
    int32	bmpages, bmsize;
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

    w = SCREEN_WIDTH;
    bmpages = (w * 2 * h
	    + GrafBase->gf_VRAMPageSize - 1) 
	    / GrafBase->gf_VRAMPageSize;
    bmsize = bmpages * GrafBase->gf_VRAMPageSize;

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
    int32		i;
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

#if 0
#define CLEAR_SCREEN(i) \
	SetVRAMPages (VRAMIOReq, theScreen.sc_Bitmaps[i]->bm_Buffer, \
	0x40004000, theScreen.sc_nFrameBufferPages, ~0)
#else
#define CLEAR_SCREEN(i) \
	do \
	{ \
	    CCB	ccb; \
	    \
	    ccb.ccb_XPos = ccb.ccb_YPos = 0; \
	    ccb.ccb_HDX = 320 << 20; \
	    ccb.ccb_VDY = 240 << 16; \
	    ccb.ccb_HDY = ccb.ccb_VDX = ccb.ccb_HDDX = ccb.ccb_HDDY = 0; \
	    ccb.ccb_Flags = (CEL_FLAGS | CCB_LAST) & ~CCB_PPABS; \
	    ccb.ccb_PIXC = PIXC_UNCODED16; \
	    ccb.ccb_PRE0 = PRE0_BGND | PRE0_LINEAR | (PRE0_BPP_16 << PRE0_BPP_SHIFT); \
	    ccb.ccb_PRE1 = PRE1_LRFORM | PRE1_TLLSB_PDC0; \
	    ccb.ccb_Width = 0x80008000; \
	    ccb.ccb_SourcePtr = (CelData *)&ccb.ccb_Width; \
	    DrawCels (theScreen.sc_BitmapItems[i], &ccb); \
	} while (0)
#endif
	
int32
Init3DO (void)
{
    if (ourOpenGraphics (&theScreen, &theScreenGroup, SCREEN_HEIGHT))
    {
	int32	i;

	if (ChangeDirectory (INITIAL_DIRECTORY) < 0)
	    return (0);

	if (!VRAMIOReq)
	    VRAMIOReq = GetVRAMIOReq ();

	if (!cel_array
		&& !(cel_array = (CCB *)AllocMem (
			sizeof (CCB) * NUM_NORMAL_CELS,
			MEMTYPE_ANY | MEMTYPE_FILL | 0)))
	    return (0);

	for (cur_ccb = &cel_array[0], i = NUM_NORMAL_CELS;
		i > 0; i--, cur_ccb++)
	{
	    cur_ccb->ccb_Flags = CEL_FLAGS;
//	    cur_ccb->ccb_NextPtr = (CCB *)MakeCCBRelative (&cur_ccb->ccb_NextPtr, cur_ccb + 1);
	}
	
	cur_ccb = &cel_array[0];
	final_ccb = &cel_array[NUM_NORMAL_CELS - 1];
	
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
		    CCB_NPABS;
	
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
		    CCB_NPABS;
		    
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

	for (i = 0; i < NUMBER_OF_SCREENS; i++)
	{
	    SetCEControl (theScreen.sc_BitmapItems[i],
		    B15POS_PDC | CFBDLSB_CFBD0 | PDCLSB_PDC0,
		    B15POS_MASK | CFBDLSB_MASK | PDCLSB_MASK);

	    CLEAR_SCREEN (i);
	}

	BlackenScreen ((1UL << 0) | (1UL << 1), 0);
	DisplayScreen (theScreen.sc_Screens[theScreen.sc_curScreen], 0);
	LightenScreen ((1UL << 0) | (1UL << 1), 0);

	return (1);
    }
	
    return (0);
}

void
Uninit3DO (void)
{
    theScreen.sc_curScreen = !theScreen.sc_curScreen;
    SetVRAMPages (VRAMIOReq,
	    theScreen.sc_Bitmaps[theScreen.sc_curScreen]->bm_Buffer,
	    0, theScreen.sc_nFrameBufferPages, ~0);

    DisplayScreen (theScreen.sc_Screens[theScreen.sc_curScreen], 0);
    BlackenScreen (1UL << theScreen.sc_curScreen, 0);
    
    if (cel_array)
    {
	FreeMem (cel_array, sizeof (CCB) * NUM_NORMAL_CELS);
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
}

static CCB	*first_ccb;

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

void
batch_cels (int32 flush)
{
    Item	BitmapItem;

    if (flush >= 0)
	BitmapItem = theScreen.sc_BitmapItems[!theScreen.sc_curScreen];
    else
	BitmapItem = theScreen.sc_BitmapItems[theScreen.sc_curScreen];
	
    if (first_ccb)
    {
	SET_CEL_FLAGS (cur_ccb, CCB_LAST);
	DrawCels (BitmapItem, first_ccb);
	first_ccb = 0;
    }

    if (flush > 0)
    {
	theScreen.sc_curScreen = !theScreen.sc_curScreen;
	DisplayScreen (theScreen.sc_Screens[theScreen.sc_curScreen], 0);
    }
}

void
_ThreeDO_batch_cels ()
{
    batch_cels (0);
}

void
FlushGraphics ()
{
    batch_cels (-1);
}

int
AnyButtonPress ()
{
    int	r;
    
    r = ReadControlPad (~0);
    if (r & JOYSTART)
	exit (0);
	
    return (r);
}

static char *
load_xlat_tab (char *file)
{
    Stream	*fp;
    char	*data;
    
    data = 0;
    if (fp = OpenDiskStream (file, 0))
    {
#define XLATE_TAB_HDR	16
	SeekDiskStream (fp, XLATE_TAB_HDR, SEEK_SET);
	data = (char *)AllocMem (XLATE_TAB_SIZE, MEMTYPE_ANY);
	ReadDiskStream (fp, data, XLATE_TAB_SIZE);
	CloseDiskStream (fp);
#if 0
{
    int	i;
    
    printf ("xlate: \n");
    for (i = 0; i < XLATE_TAB_SIZE; i++)
    {
	printf ("%d ", data[i]);
	if (i % 16 == 15)
	    printf ("\n");
    }
}
#endif
    }
    
    return (data);
}

static char *
load_cmap (char *file)
{
    Stream	*fp;
    char	*data;
    
    data = 0;
    if (fp = OpenDiskStream (file, 0))
    {
#define CMAP_HDR	24
	SeekDiskStream (fp, CMAP_HDR, SEEK_SET);
	data = (char *)AllocMem (CMAP_SIZE * 3, MEMTYPE_ANY);
	ReadDiskStream (fp, data, CMAP_SIZE * 3);
	CloseDiskStream (fp);
	
#if 0
{
    int	i;
    char	*p;
    
    p = data + CMAP_SIZE * 2;
    
    printf ("cmap: (%d -- %d)\n", p[0], p[1]);
    
    for (i = 0; i < 128; i++)
    {
	int	j;
	
	j = (i * 3) + 2;
	printf ("%d: ", i);
	printf ("%d %d %d\n", p[j], p[j + 1], p[j + 2]);
    }
}
#endif
    }
    
    return (data);
}

#define SET_CLIP(x,y,w,h) \
    do \
    { \
	int32	k; \
	\
	for (k = 0; k < 2; k++) \
	{ \
	    SetClipOrigin (theScreen.sc_BitmapItems[k], 0, 0); \
	    SetClipWidth (theScreen.sc_BitmapItems[k], \
		    w); \
	    SetClipHeight (theScreen.sc_BitmapItems[k], \
		    h); \
	    SetClipOrigin (theScreen.sc_BitmapItems[k], x, y); \
	} \
    } while (0)

int32
random ()  
{
    return (rand ());
}

#define WIDTH		210
#define HEIGHT		67
#define SURFACE_WIDTH	210
#define SURFACE_HEIGHT	124

int32
DoneAndWaiting ()
{
    int32	da;

    while (ReadControlPad (~0));
    while (!(da = ReadControlPad (~0)));
    CLEAR_SCREEN (theScreen.sc_curScreen);
    CLEAR_SCREEN (!theScreen.sc_curScreen);
    
    return (da & JOYSTART);
}

void
WaitForButtonPress ()
{
    int32	da;

    while (ReadControlPad (~0));
    while (!(da = ReadControlPad (~0)));
    while (ReadControlPad (~0));
    if (da & JOYSTART)
	exit (0);
}

void
LoadIntoExtraScreen ()
{
    int32		i;

    i = theScreen.sc_curScreen;
    screen_ccb.ccb_SourcePtr = (void *)theScreen.sc_Bitmaps[i]->bm_Buffer;
    screen_ccb.ccb_XPos = 0;
    screen_ccb.ccb_YPos = 0;
    DrawCels (theScreen.sc_BitmapItems[EXTRA_SCREEN], &screen_ccb);
}

void
_threedo_copy_to_extra ()
{
    LoadIntoExtraScreen ();
}

void
DrawFromExtraScreen (int32 x, int32 y, int32 w, int32 h)
{
    extra_ccb.ccb_XPos = (-x) << 16;
    extra_ccb.ccb_YPos = (-y) << 16;
    add_cel (&extra_ccb);
}

int32
GrabPixel (int32 x, int32 y)
{
    Bitmap	*bitmap;
    int32	*srcptr;
    
    bitmap = theScreen.sc_Bitmaps[theScreen.sc_curScreen];
    srcptr = (int32 *)bitmap->bm_Buffer + (y * (bitmap->bm_Width >> 1)) + x;
    
    return (*srcptr);
}

typedef struct
{
    ulong	TypeIndexAndFlags;
    ulong	HotSpot;
    ulong	Bounds;
    long	DataOffs;
} FRAME_DESC;

int
main (int32 argc, char **argv)
{
#define ONE_SECOND	239
    int32	x, y, da, incr;
    char	*xlt, *cmap;
    int32	*image;
    char	buf[80];
    int32	t, dx, dy;
    void	*circ_buf, *blur_buf, *shield_buf, *earth_buf, *shadow_buf;
    FRAME_DESC	circ_f, blur_f, shield_f, earth_f, shadow_f;
    int32	*circ_d, *blur_d, *shield_d, *earth_d, *shadow_d;
    int32	size;

    if (!Init3DO ())
	exit (0);

    --argc;
    ++argv;

    OpenAudioFolio ();
    srand (0);//GetAudioTime ());

    circ_buf = (char *)LoadFile ("planet.dat/pl_circ.cel", &size, MEMTYPE_ANY);
    circ_d = (int32 *)AllocMem (size + 4, MEMTYPE_ANY);
    *circ_d = size;
    memcpy (circ_d + 1, circ_buf, size);
    UnloadFile (circ_buf);
    circ_f.DataOffs = (uchar *)circ_d - (uchar *)&circ_f;
    
    blur_buf = (char *)LoadFile ("planet.dat/pl_blur.cel", &size, MEMTYPE_ANY);
    blur_d = (int32 *)AllocMem (size + 4, MEMTYPE_ANY);
    *blur_d = size;
    memcpy (blur_d + 1, blur_buf, size);
    UnloadFile (blur_buf);
    blur_f.DataOffs = (uchar *)blur_d - (uchar *)&blur_f;
    
    shield_buf = (char *)LoadFile ("planet.dat/shield.cel", &size, MEMTYPE_ANY);
    shield_d = (int32 *)AllocMem (size + 4, MEMTYPE_ANY);
    *shield_d = size;
    memcpy (shield_d + 1, shield_buf, size);
    UnloadFile (shield_buf);
    shield_f.DataOffs = (uchar *)shield_d - (uchar *)&shield_f;
    
    shadow_buf = (char *)LoadFile ("planet.dat/shadow.cel", &size, MEMTYPE_ANY);
    shadow_d = (int32 *)AllocMem (size + 4, MEMTYPE_ANY);
    *shadow_d = size;
    memcpy (shadow_d + 1, shadow_buf, size);
    UnloadFile (shadow_buf);
    shadow_f.DataOffs = (uchar *)shadow_d - (uchar *)&shadow_f;
 
    earth_buf = (char *)LoadFile ("planet.dat/earth.cel", &size, MEMTYPE_ANY);
    earth_d = (int32 *)AllocMem (size + 4, MEMTYPE_ANY);
    *earth_d = size;
    memcpy (earth_d + 1, earth_buf, size);
    UnloadFile (earth_buf);
    earth_f.DataOffs = (uchar *)earth_d - (uchar *)&earth_f;

    InitPlanetStuff (&circ_f, &blur_f, &shield_f);
    
    image = (int32 *)AllocMem (232 * 67 * 2, MEMTYPE_ANY);
    
    if (argc == 1)
    {
	char	*s;
    
	da = 0;
	incr = 1;
	s = *argv;
	if ((*s >= '0' && *s <= '9')
		|| *s == '-')
	{
	    if (*s == '-')
		incr = -1;
	    da = atoi (s);
	}
	
	BuildPlanet (82, 2, 0, 0, 0, image, 1, &shadow_f, &earth_f, 1);
    }
    else
    {
	sprintf (buf, "planet.dat/%s", *(argv + 3));
	xlt = load_xlat_tab (buf);
	sprintf (buf, "planet.dat/%s", *(argv + 4));
	cmap = load_cmap (buf);

	da = 0;
	incr = 1;
	if (argc > 5)
	{
	    char	*s;
	
	    s = *(argv + 5);
	    if ((*s >= '0' && *s <= '9')
		    || *s == '-')
	    {
		if (*s == '-')
		incr = -1;
		da = atoi (s);
	    }
	}

	BuildPlanet (atoi (*argv), atoi (*(argv + 1)), atoi (*(argv + 2)),
		xlt, cmap, image, 0, &shadow_f, 0, 1);
    }

#ifdef HIDE_PLANET
    SET_CLIP (22, 26, WIDTH, HEIGHT);
    DrawPlanet (0, 0, 0, 0);
    batch_cels (1);
    while (ReadControlPad (~0));
    while (!ReadControlPad (~0));
    while (ReadControlPad (~0));

#if 0
    DrawPlanet (0, 0, 0, 0);
    batch_cels (1);
    t = GetAudioTime ();
    for (y = 0; y < HEIGHT + 9; y++)
    {
	DrawPlanet (0, 0, y, 0x1f << 10);
	batch_cels (1);
	while (GetAudioTime () < t + 6);
	t = GetAudioTime ();
    }

    while (ReadControlPad (~0));
    while (!ReadControlPad (~0));
    while (ReadControlPad (~0));
#endif

    CLEAR_SCREEN (theScreen.sc_curScreen);
    CLEAR_SCREEN (!theScreen.sc_curScreen);
#endif

#ifndef HIDE_SURFACE
    SET_CLIP (22, 26, SURFACE_WIDTH, SURFACE_HEIGHT);
    x = 0;
    y = 67;
    dx = dy = 1;
    SetPlanetZoom ();
    BatchZoomedPlanet ();
    RepairZoomedPlanet (x, y, 0, 0);
    UnbatchZoomedPlanet ();
    batch_cels (1);
    LoadIntoExtraScreen ();
    while (ReadControlPad (~0));
    while (!ReadControlPad (~0));
    while (ReadControlPad (~0));

dx = 0;
    do
    {
	int32	r;

	DrawFromExtraScreen (22 - dx, 26 - dy, SURFACE_WIDTH, SURFACE_HEIGHT);
	BatchZoomedPlanet ();
	ScrollZoomedPlanet (dx, dy, 22, 26, theScreen.sc_Bitmaps[theScreen.sc_curScreen]->bm_Buffer);	
	UnbatchZoomedPlanet ();
	batch_cels (1);
	LoadIntoExtraScreen ();
#if 0
	if ((r = ReadControlPad (~0)) & JOYSTART)
	{
	    while (ReadControlPad (~0));
	    while (!ReadControlPad (~0));
	    while (ReadControlPad (~0));
	}
	else if (r)
	    break;
#else
	// for freezing after each display
	while (ReadControlPad (~0));
	while (!(da = ReadControlPad (~0)));
	if (da & JOYSTART) exit (0);
	else if (da & JOYFIREC) break;
if (da & JOYUP)
    dy = 1;
else if (da & JOYDOWN)
    dy = -1;
#endif
	if ((x += dx) == WIDTH << 1)
	{
	    dx = -dx;
	    x = 0;
	}
	if ((y += dy) == (HEIGHT << 2) - (SURFACE_HEIGHT >> 1) || y < -(SURFACE_HEIGHT >> 1))
	{
	    dy = -dy;
	    y += dy;
	}
    } while (1);
    
    while (ReadControlPad (~0));
    while (!ReadControlPad (~0));
    while (ReadControlPad (~0));
    CLEAR_SCREEN (theScreen.sc_curScreen);
    CLEAR_SCREEN (!theScreen.sc_curScreen);
    
    SET_CLIP (0, 0, 320, 240);
#endif

    SetPlanetTilt (da);
    
    x = 0;

Rotate:
    t = GetAudioTime ();
    RotatePlanet (x, 80, 80, 1);
//    batch_cels (1);

#if 1
    // for pausing, quitting during rotation
    if ((da = ReadControlPad (~0)) & JOYSTART)
    {
	while (ReadControlPad (~0));
	while (!ReadControlPad (~0));
	while (ReadControlPad (~0));
    }
    else if (da)
	goto Done;
#endif

    x += incr;
    if (x < 0)
	x = WIDTH - 1;
    else if (x == WIDTH)
	x = 0;

    do
	Yield ();
    while (GetAudioTime () < t + ONE_SECOND / 30);
    
    // for quitting after first display
    //while (ReadControlPad (~0));
    //while (!ReadControlPad (~0));
    //exit (0);

//    CLEAR_SCREEN (!theScreen.sc_curScreen);
#if 0
    // for freezing after each display
    while (ReadControlPad (~0));
    while (!(da = ReadControlPad (~0)));
    if (da & JOYSTART) exit (0);
#endif

    goto Rotate;
    
Done:    
    UninitPlanetStuff ();
    Uninit3DO ();
    
    exit (0);
}

void *
ThreedoAlloc (int32 size)
{
    return (AllocMem (size, MEMTYPE_ANY | MEMTYPE_FILL | 0));
}

void
ThreedoFree (void *p, int32 size)
{
    FreeMem (p, size);
}



