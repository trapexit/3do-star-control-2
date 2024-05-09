#include "Portfolio.h"
#include "Init3DO.h"
#include "Parse3DO.h"
#include "Utils3DO.h"
#include "filefunctions.h"
#include "BlockFile.h"

#define CEL_FLAGS \
	(CCB_SPABS | CCB_PPABS | CCB_YOXY | CCB_ACW | CCB_ACCW \
	| CCB_LDSIZE | CCB_CCBPRE | CCB_LDPRS | CCB_LDPPMP | CCB_ACE \
	| CCB_LCE | CCB_PLUTPOS | CCB_BGND | CCB_NPABS)

#define PIXC_DUP(v)		(((v) << PPMP_0_SHIFT) | ((v) << PPMP_1_SHIFT))
#define PIXC_UNCODED16		(PIXC_DUP (PPMPC_MF_8 | PPMPC_SF_8))

#define NUM_THINGS	256

extern CCB	plCCB[];
static CCB	*oscBackCCB;
static void	*oscBackBuf;
static ubyte	flatline;

void
InitOscilloscope (int32 x, int32 y, int32 width, int32 height)
{
    CCB		*ccb;
    int32	xpos, xerr, size;
 
    memset (plCCB, 0, sizeof (CCB) * NUM_THINGS);

    if (oscBackBuf) // dumb, but no Uninit for now
	UnloadFile (oscBackBuf);
	
    if (oscBackBuf = (char *)LoadFile ("starcon2/ipanims/oscback.cel",
	    &size, MEMTYPE_ANY))
    {
	if (oscBackCCB = ParseCel (oscBackBuf, size))
	{
	    oscBackCCB->ccb_Flags &= ~CCB_LAST;
	    oscBackCCB->ccb_XPos = x << 16;
	    oscBackCCB->ccb_YPos = y << 16;
	    oscBackCCB->ccb_NextPtr = plCCB;
	}
	else
	{
	    UnloadFile (oscBackBuf);
	    oscBackCCB = plCCB;
	}
    }
    else
	oscBackCCB = plCCB;

    ccb = plCCB;
    ccb->ccb_Width = y;
    ccb->ccb_Height = height >> 1;
    xpos = x;
    xerr = NUM_THINGS;
    do
    {
	int32	color;
	    
	ccb->ccb_XPos = xpos << 16;
	if ((xerr -= width) <= 0)
	{
	    ++xpos;
	    xerr += NUM_THINGS;
	}
	ccb->ccb_HDX = 1 << 20;
	ccb->ccb_VDY = 1 << 16;
	
	ccb->ccb_Flags = CEL_FLAGS;
	ccb->ccb_PRE0 =
		(PRE0_BPP_16 << PRE0_BPP_SHIFT) |
		PRE0_BGND |
		PRE0_LINEAR;
	ccb->ccb_PRE1 = PRE1_TLLSB_PDC0;
	ccb->ccb_PIXC = PIXC_UNCODED16;
	
	color = ((0x1f << 10) | (0x1f << 5) | 0x1f) | 0x8000;
	ccb->ccb_PLUTPtr = (void *)((color << 16) | color);
	ccb->ccb_SourcePtr = (void *)&ccb->ccb_PLUTPtr;
	    
	ccb->ccb_NextPtr = ccb + 1;
    } while (++ccb != &plCCB[NUM_THINGS]);
}

void
Oscilloscope (int32 grab_data)
{
    CCB		*ccb;
    char	buf[NUM_THINGS * 2], *data;
    int32	size, ypos, max;
    
    if (!grab_data
	    || !(size = GetSoundData (buf)))
    {
	if (flatline)
	    return;
	    
	flatline = 1;
	memset (buf, 0, sizeof (buf));
    }
    else
	flatline = 0;
	
    ccb = plCCB;
    max = ccb->ccb_Height;
    ypos = ccb->ccb_Width + max;
    data = buf;
    do
    {
	char	c;
	int32	d;
	
	d = c = *data;
	if (c & 0x80)
	    d |= 0xffffff00;
#ifndef READ_SAMPLE_DIRECTLY
	data += 2;
#else
	data++;
#endif
	ccb->ccb_YPos = (ypos + ((d * max) >> 7)) << 16;
    } while (++ccb != &plCCB[NUM_THINGS]);

    ClearDrawable ();

    add_cels (oscBackCCB, ccb - 1);
    
    FlushGraphics (TRUE);
}

static CCB	slider_cels[2];

void
InitSlider (int32 x, int32 y, int32 width, int32 height)
{
#define SLIDER_X	x
#define SLIDER_Y	y
#define SLIDER_WIDTH	width
#define SLIDER_HEIGHT	4
#define BUTTON_WIDTH	4
#define BUTTON_HEIGHT	12
    int32	i;
    CCB		*ccb;

slider_cels[0].ccb_Width = SLIDER_X;
slider_cels[0].ccb_Height = SLIDER_WIDTH;
slider_cels[1].ccb_Width = BUTTON_WIDTH;
	
    ccb = slider_cels;
    for (i = 0; i < 2; i++, ccb++)
    {
	int32	color;
	    
	ccb->ccb_Flags = CEL_FLAGS;
	ccb->ccb_PRE0 =
		(PRE0_BPP_16 << PRE0_BPP_SHIFT) |
		PRE0_BGND |
		PRE0_LINEAR;
	ccb->ccb_PRE1 = PRE1_TLLSB_PDC0;
	ccb->ccb_PIXC = PIXC_UNCODED16;
	
	if (i == 0)
	{
	    color = (0x1f << 5) | 0x8000;
	    ccb->ccb_XPos = SLIDER_X << 16;
	    ccb->ccb_YPos = SLIDER_Y << 16;
	    ccb->ccb_HDX = SLIDER_WIDTH << 20;
	    ccb->ccb_VDY = SLIDER_HEIGHT << 16;
	}
	else
	{
	    color = 0x1f | 0x8000;
	    ccb->ccb_YPos = (SLIDER_Y - ((BUTTON_HEIGHT - SLIDER_HEIGHT) >> 1)) << 16;
	    ccb->ccb_HDX = BUTTON_WIDTH << 20;
	    ccb->ccb_VDY = BUTTON_HEIGHT << 16;
	}
	    
	ccb->ccb_PLUTPtr = (void *)((color << 16) | color);
	ccb->ccb_SourcePtr = (void *)&ccb->ccb_PLUTPtr;
	    
	ccb->ccb_NextPtr = ccb + 1;
    }
}

void
Slider ()
{
#undef SLIDER_X
#undef SLIDER_WIDTH
#undef BUTTON_WIDTH
#define SLIDER_X	slider_cels[0].ccb_Width
#define SLIDER_WIDTH	slider_cels[0].ccb_Height
#define BUTTON_WIDTH	slider_cels[1].ccb_Width
    int32	len, offs;
    
    if (GetSoundInfo (&len, &offs))
    {
	slider_cels[1].ccb_XPos = (SLIDER_X + ((SLIDER_WIDTH - BUTTON_WIDTH) * offs / len)) << 16;
	add_cels (slider_cels, &slider_cels[1]);
    }
}

