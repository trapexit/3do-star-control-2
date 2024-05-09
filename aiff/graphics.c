#include "Portfolio.h"
#include "Init3DO.h"
#include "Parse3DO.h"
#include "Utils3DO.h"
#include "filefunctions.h"
#include "BlockFile.h"

#define INITIAL_DIRECTORY	"$boot"

#ifdef DEBUG
#define PRD(s)		printf s
#define SYSERR(e)	PrintfSysErr (e)
#else /* DEBUG */
#define PRD(s)
#define SYSERR(e)
#endif /* DEBUG */

#define NUMBER_OF_SCREENS	2

#define SHADOW_COLOR		(0x18c6) // Shadow color... sleazy

#define PIXC_DUP(v)		(((v) << PPMP_0_SHIFT) | ((v) << PPMP_1_SHIFT))
#define PIXC_UNCODED16		(PIXC_DUP (PPMPC_MF_8 | PPMPC_SF_8))

#define SET_CEL_FLAGS(c,f)	((c)->ccb_Flags |= (f))
#define CLR_CEL_FLAGS(c,f)	((c)->ccb_Flags &= ~(f))

#define CEL_FLAGS \
	(CCB_SPABS | CCB_PPABS | CCB_YOXY | CCB_ACW | CCB_ACCW | CCB_LDSIZE | CCB_CCBPRE \
	| CCB_LDPRS | CCB_LDPPMP | CCB_ACE | CCB_LCE | CCB_PLUTPOS | CCB_BGND | CCB_NOBLK)

#define NUM_NORMAL_CELS		500

#define NUMBER_OF_CLUTVALS	32

#define SCREEN_WIDTH		320
#define SCREEN_HEIGHT		240

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

static CCB	*cur_ccb, *first_ccb;

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

#define CLEAR_SCREEN(i) \
	SetVRAMPages (VRAMIOReq, theScreen.sc_Bitmaps[i]->bm_Buffer, \
	0x00000000, theScreen.sc_nFrameBufferPages, ~0)
	
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

static uint32	cel_ct;

void
batch_cels (int32 flush)
{
    Item	BitmapItem;

    BitmapItem = theScreen.sc_BitmapItems[!theScreen.sc_curScreen];
	
    if (first_ccb)
    {
	SET_CEL_FLAGS (cur_ccb, CCB_LAST);
#if 0
{
    int32	i;
    
    i = 1;
    cur_ccb = first_ccb;
    while (!(cur_ccb->ccb_Flags & CCB_LAST))
    {
	i++;
	cur_ccb = cur_ccb->ccb_NextPtr;
    }
    printf ("drawing %ld cels\n", i);
}
#endif
	DrawCels (BitmapItem, first_ccb);
	first_ccb = 0;
    }

    if (flush)
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

int32
ClearScreen ()
{
    CLEAR_SCREEN (!theScreen.sc_curScreen);
}

int32
ClearDrawable ()
{
}

int32
FlushGraphics ()
{
}

void
GetScreenContext (ScreenContext *sc)
{
    int32	i;
    
    sc->sc_nScreens = NUMBER_OF_SCREENS;
    for (i = 0; i < NUMBER_OF_SCREENS; i++)
	sc->sc_Screens[i] = theScreen.sc_Screens[i];
}

Item
GetScreenGroup ()
{
    return (theScreenGroup);
}
