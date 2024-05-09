#include "Portfolio.h"
#include "Init3DO.h"
#include "Parse3DO.h"
#include "Utils3DO.h"
#include "filefunctions.h"
#include "BlockFile.h"

//#define KDEBUG
#define SCALE_ROTATE
//#define OLD_MASK

#define SCREEN_WIDTH		320

#define WIDTH			210
#define HEIGHT			67
#define XBANDS			6

#define SPHERE_HEIGHT		(HEIGHT + 10)
#define SPHERE_WIDTH		SPHERE_HEIGHT

#define WRAP_EXTRA		20

#define LINE_PIXELS		((WIDTH + WRAP_EXTRA + 3) & ~3)

#define SURFACE_WIDTH		210
#define SURFACE_HEIGHT		124

#define SURFACE_SHIFT		2
#define SURFACE_UNIT		(1 << SURFACE_SHIFT)
#define SURFACE_ADD		(SURFACE_UNIT - 1)

#define MASK_WIDTH		(SURFACE_UNIT + 1 + (2 * SURFACE_ADD))
#define MASK_HEIGHT		MASK_WIDTH

#ifdef OLD_MASK
#define NUM_MASKS		8
#else
#define NUM_MASKS		(sizeof (mask_buf) / sizeof (mask_buf[0]))
#endif

#define NUM_TINTS		4

#define PIXC_DUP(v)		(((uint32)(v) << PPMP_0_SHIFT) | ((uint32)(v) << PPMP_1_SHIFT))
#define PIXC_UNCODED16		(PIXC_DUP (PPMPC_MF_8 | PPMPC_SF_8))
#define PIXC_UNCODED8		(PIXC_DUP (PPMPC_MF_8 | PPMPC_SF_8))
#define PIXC_CODED		(PIXC_DUP (PPMPC_MS_PIN | PPMPC_SF_8))

#define SET_CEL_FLAGS(c,f)	((c)->ccb_Flags |= (f))
#define CLR_CEL_FLAGS(c,f)	((c)->ccb_Flags &= ~(f))

#define CEL_FLAGS \
	(CCB_SPABS | CCB_PPABS | CCB_YOXY | CCB_ACW | CCB_ACCW | CCB_LDSIZE | CCB_CCBPRE \
	| CCB_LDPRS | CCB_LDPPMP | CCB_ACE | CCB_LCE | CCB_PLUTPOS | CCB_BGND | CCB_NPABS \
	/*| CCB_NOBLK*/)

#define MAP_CEL			myMapCel

#define NUM_ROTATE_CELS		(138) // derived empirically!

#define NUM_CELS		(NUM_ROTATE_CELS * 2) // must be at least 257 for oscilloscope

#define MIN_Y			(-(SURFACE_HEIGHT >> 1))
#define MAX_Y			((HEIGHT << SURFACE_SHIFT) - (SURFACE_HEIGHT >> 1))

#define PULSE_SCALE		(2)
#define SCALE_PULSE_RED(r)	((r)<<PULSE_SCALE)
#define MIN_PULSE_RED		(SCALE_PULSE_RED(2))
#define MAX_PULSE_RED		(SCALE_PULSE_RED(7))
#define PULSE_NEG_BIT		(1 << 7)

#define SOL	82
#define EARTH	2
#define LUNA	2

typedef struct
{
    int32	x0[15], x1[15], y0[15], y1[15];
    int32	ystep[15];
    int32	src_ws[XBANDS];

    CCB		planet_ccb, surface_ccb, tint_ccb[NUM_TINTS], mask_ccb;
    
    CCB		black_rect_ccb, black_circ_ccb, *pblack_circ_ccb, *pshield_ccb, *pshadow_ccb;
    
    CCB		repair_ccb[2];
    
    int32	black_circ_plut[16];

    ubyte	rmap[HEIGHT][LINE_PIXELS];
#ifdef OLD_MASK
    uint32	mask_buf[NUM_MASKS][MASK_HEIGHT][2];
#endif

    CCB		*cur_ccb;

    int32	old_sx, old_sy;
    
    CCB		plCCB[NUM_CELS];
    
    CCB		*last_zoom_ccb;
    ubyte	zoom_batch;

#ifdef SCALE_ROTATE
    int32	scale;
#endif

    CCB		*pcirc_ccb, *pblur_ccb;
} PLANET_STUFF;

static PLANET_STUFF	*planet_stuff;

#define	x0		(planet_stuff->x0)
#define	x1		(planet_stuff->x1)
#define	y0		(planet_stuff->y0)
#define	y1		(planet_stuff->y1)
#define ystep		(planet_stuff->ystep)
#define src_ws		(planet_stuff->src_ws)

#define planet_ccb	(planet_stuff->planet_ccb)
#define surface_ccb	(planet_stuff->surface_ccb)
#define tint_ccb	(planet_stuff->tint_ccb)
#define mask_ccb	(planet_stuff->mask_ccb)

#define black_rect_ccb	(planet_stuff->black_rect_ccb)

#define black_circ_ccb	(planet_stuff->black_circ_ccb)

#define pshadow_ccb	(planet_stuff->pshadow_ccb)

#define pshield_ccb	(planet_stuff->pshield_ccb)

#define pblack_circ_ccb	(planet_stuff->pblack_circ_ccb)

#define black_circ_plut	(planet_stuff->black_circ_plut)

#define repair_ccb	(planet_stuff->repair_ccb)

#define rmap		(planet_stuff->rmap)

#ifdef OLD_MASK
#define mask_buf	(planet_stuff->mask_buf)
#else
static uint32	mask_buf[][MASK_HEIGHT][2] =
{
    {
	{ 0x04000000, },
	{ 0x5ac00000, },
	{ 0x3f800000, },
	{ 0x5fa00000, },
	{ 0xffc00000, },
	{ 0x7fe00000, },
	{ 0xbfa00000, },
	{ 0x5f800000, },
	{ 0x36c00000, },
	{ 0x49000000, },
	{ 0x10800000, }
    },
    
    {
	{ 0x02000000, },
	{ 0x95000000, },
	{ 0x2f400000, },
	{ 0x7fc00000, },
	{ 0x5fa00000, },
	{ 0x3fc00000, },
	{ 0x7f400000, },
	{ 0x1fa00000, },
	{ 0x4f400000, },
	{ 0x19800000, },
	{ 0x04000000, }
    },
    
    {
	{ 0x08000000, },
	{ 0x57400000, },
	{ 0x2a800000, },
	{ 0x7f400000, },
	{ 0xbfe00000, },
	{ 0x5fc00000, },
	{ 0x3fa00000, },
	{ 0x9fc00000, },
	{ 0x3a800000, },
	{ 0x57400000, },
	{ 0x0a000000, }
    },
    
    {
	{ 0x08000000, },
	{ 0x36800000, },
	{ 0x5d000000, },
	{ 0x3fa00000, },
	{ 0x7fc00000, },
	{ 0xbfc00000, },
	{ 0x7fc00000, },
	{ 0x3f800000, },
	{ 0x7ba00000, },
	{ 0x2ec00000, },
	{ 0x19000000, }
    },
    
    {
	{ 0x13000000, },
	{ 0x6a800000, },
	{ 0x5d800000, },
	{ 0xbf200000, },
	{ 0xdfe00000, },
	{ 0xbfc00000, },
	{ 0x3fc00000, },
	{ 0x5f800000, },
	{ 0x36800000, },
	{ 0x5d400000, },
	{ 0x02000000, }
    },
    
    {
	{ 0x00000000, },
	{ 0x24800000, },
	{ 0x0e000000, },
	{ 0x5f000000, },
	{ 0x3f800000, },
	{ 0x3f000000, },
	{ 0x1f800000, },
	{ 0x5f000000, },
	{ 0x2e000000, },
	{ 0x04800000, },
	{ 0x00000000, }
    },
    
    {
	{ 0x31000000, },
	{ 0x5a800000, },
	{ 0x7d400000, },
	{ 0x3fc00000, },
	{ 0x5f600000, },
	{ 0xbfa00000, },
	{ 0x7f400000, },
	{ 0x3fe00000, },
	{ 0x77400000, },
	{ 0xaa800000, },
	{ 0x14000000, }
    },
    
    {
	{ 0x00000000, },
	{ 0x12000000, },
	{ 0x2d800000, },
	{ 0x7f200000, },
	{ 0x3f800000, },
	{ 0x5f000000, },
	{ 0xffc00000, },
	{ 0x7f800000, },
	{ 0x2b000000, },
	{ 0x44800000, },
	{ 0x02000000, }
    },
    
    {
	{ 0x20000000, },
	{ 0x52000000, },
	{ 0x7d800000, },
	{ 0xbf400000, },
	{ 0x7fc00000, },
	{ 0x7f800000, },
	{ 0x7f400000, },
	{ 0x3f800000, },
	{ 0x4fc00000, },
	{ 0x1d400000, },
	{ 0x02800000, }
    },
    
    {
	{ 0x04000000, },
	{ 0x3a800000, },
	{ 0x5dc00000, },
	{ 0x3f800000, },
	{ 0x5f400000, },
	{ 0xffc00000, },
	{ 0x7fe00000, },
	{ 0x7fc00000, },
	{ 0x3fc00000, },
	{ 0x17800000, },
	{ 0x09800000, }
    },
};
#endif

#define cur_ccb		(planet_stuff->cur_ccb)

#define old_sx		(planet_stuff->old_sx)
#define old_sy		(planet_stuff->old_sy)

#define plCCB		(planet_stuff->plCCB)

#define last_zoom_ccb	(planet_stuff->last_zoom_ccb)
#define zoom_batch	(planet_stuff->zoom_batch)

#define shield_pulse	(zoom_batch)

#ifdef SCALE_ROTATE
#define scale		(planet_stuff->scale)

#define SCALE_AMT_SHIFT	7
#define SCALE_AMT_MASK	((1<<(SCALE_AMT_SHIFT+1))-1)
#define SCALE_X_SHIFT	9
#define SCALE_X_MASK	(3<<SCALE_X_SHIFT)
#define SCALE_Y_SHIFT	12
#define SCALE_Y_MASK	(3<<SCALE_Y_SHIFT)
#define SCALE_MAX	(1<<SCALE_AMT_SHIFT)
#define SCALE_STEP	((scale & SCALE_AMT_MASK) < (SCALE_MAX / 2) ? \
			4 : ((scale & SCALE_AMT_MASK) < (SCALE_MAX * 3 / 4) ? \
			3 : ((scale & SCALE_AMT_MASK) < (SCALE_MAX * 7 / 8) ? \
			2 : 1)))

#define SCALE_CEL(ccb,cx,cy) \
	do \
	{ \
	    int32	s, _dx, _dy; \
	    \
	    s = scale & SCALE_AMT_MASK; \
	    _dx = (int32)((scale & SCALE_X_MASK) >> SCALE_X_SHIFT) - 1; \
	    _dy = (int32)((scale & SCALE_Y_MASK) >> SCALE_Y_SHIFT) - 1; \
	    (ccb)->ccb_XPos = (((ccb)->ccb_XPos - (cx)) * s) >> SCALE_AMT_SHIFT; \
	    (ccb)->ccb_YPos = (((ccb)->ccb_YPos - (cy)) * s) >> SCALE_AMT_SHIFT; \
	    if (_dx < 0) \
		(ccb)->ccb_XPos += ((cx) * s) >> SCALE_AMT_SHIFT; \
	    else if (_dx > 0) \
		(ccb)->ccb_XPos += ((cx) << 1) - (((cx) * s) >> SCALE_AMT_SHIFT); \
	    else \
		(ccb)->ccb_XPos += (cx); \
	    if (_dy < 0) \
		(ccb)->ccb_YPos += ((cy) * s) >> SCALE_AMT_SHIFT; \
	    else if (_dy > 0) \
		(ccb)->ccb_YPos += ((cy) << 1) - (((cy) * s) >> SCALE_AMT_SHIFT); \
	    else \
		(ccb)->ccb_YPos += (cy); \
	    (ccb)->ccb_HDX = ((ccb)->ccb_HDX * s) >> SCALE_AMT_SHIFT; \
	    (ccb)->ccb_VDY = ((ccb)->ccb_VDY * s) >> SCALE_AMT_SHIFT; \
	    (ccb)->ccb_VDX = ((ccb)->ccb_VDX * s) >> SCALE_AMT_SHIFT; \
	    (ccb)->ccb_HDY = ((ccb)->ccb_HDY * s) >> SCALE_AMT_SHIFT; \
	    (ccb)->ccb_HDDX = ((ccb)->ccb_HDDX * s) >> SCALE_AMT_SHIFT; \
	    (ccb)->ccb_HDDY = ((ccb)->ccb_HDDY * s) >> SCALE_AMT_SHIFT; \
	} while (0)
#endif

#define pcirc_ccb	(planet_stuff->pcirc_ccb)
#define pblur_ccb	(planet_stuff->pblur_ccb)

static void
build_steps ()
{
    int32	y, i, c, inc;

    c = 1;
    inc = 1;
    y = 0;
    for (i = 0; y < (HEIGHT >> 1); ++i)
    {
	ystep[i] = c;
	y += c;
	if (y > (HEIGHT >> 2) && inc == 1)
	{
	    y -= c;
	    ystep[i] = ((HEIGHT >> 1) - (y << 1));
	    if (ystep[i] == 0)
		--i;
	    else
		y += ystep[i];
	    
	    inc = -1;
	}
	c += inc;
    }

    ystep[i] = 1;
    
#if 0
y = 0;
for (i = 0; y < (HEIGHT >> 1); i++)
{
    printf ("ystep[%ld] = %ld\n", i, ystep[i]);
    y += ystep[i];
}
#endif
}

static void
build_tables (int32 da)
{
    int32	y, i;

    build_steps ();
    
    OpenMathFolio (); // calling this multiply is not a problem, right? 
    
    for (y = 0, i = 0; y <= (HEIGHT >> 1); i++)
    {
	int32	dx, dy, a, h;
	
	h = ystep[i];

	if (y + h > (HEIGHT >> 1)
		&& (h = (HEIGHT >> 1) - y + 1) == 0)
	    return;

	dy = (HEIGHT >> 1) - y;
	dx = SqrtF16 (((HEIGHT >> 1) * (HEIGHT >> 1) - dy * dy) << 16);
	dy <<= 16;

	a = Atan2F16 (-dx, -dy);
	a += da << 16;
	x0[i] = HEIGHT * CosF16 (a);
	y0[i] = HEIGHT * SinF16 (a);
	a = Atan2F16 (dx, -dy);
	a += da << 16;
	x1[i] = HEIGHT * CosF16 (a);
	y1[i] = HEIGHT * SinF16 (a);
	
	x0[i] >>= 1;
	y0[i] >>= 1;
	x1[i] >>= 1;
	y1[i] >>= 1;
	
	y += h;
#if 0
printf ("%ld: %ld  ", i, y);
printf ("%ld,%ld -- ", x0[i] >> 16, y0[i] >> 16);
printf ("%ld,%ld\n", x1[i] >> 16, y1[i] >> 16);
#endif
    }
    
    x0[i] = x0[i - 1];
    y0[i] = y0[i - 1];
    x1[i] = x1[i - 1];
    y1[i] = y1[i - 1];
}

static void
draw_band (int32 src_x, int32 src_y, int32 dst_cx, int32 dst_cy)
{
    int32	*sp;
    int32	i;
    
    sp = (int32 *)planet_ccb.ccb_SourcePtr + src_y * (LINE_PIXELS >> 1);

    for (i = 0; i < XBANDS; i++)
    {
#define src_w	src_ws[i]
	cur_ccb->ccb_SourcePtr = (CelData *)(sp + (src_x >> 1));
	cur_ccb->ccb_XPos = dst_cx + cur_ccb->ccb_Width;
	cur_ccb->ccb_YPos = dst_cy + cur_ccb->ccb_Height;
#ifdef SCALE_ROTATE
	if (scale)
	    SCALE_CEL (cur_ccb, dst_cx, dst_cy);
#endif
	cur_ccb++;
    
	src_x += src_w;
	if (src_x >= WIDTH)
	    src_x -= WIDTH;
	else if (src_x < 0)
	    src_x += WIDTH;
    }
}

static int32
load_elevations (Stream *fp, char *elev)
{
    int32	ct;

    ct = HEIGHT;
    do
    {
	ReadDiskStream (fp, elev, WIDTH);
	memcpy (elev + WIDTH, elev, WRAP_EXTRA);
	elev += LINE_PIXELS;
    } while (--ct);
	
    return (1);
}

static void
xlate_topography (char *elev, char *xlat_tab, char *cmap, int32 *image)
{
    int32	x, y;

    cmap += 2;
    for (y = 0; y < HEIGHT; y++)
    {
	char	*ep;
	int32	*ip;
	
	ep = elev;
	ip = image;
	for (x = 0; x < LINE_PIXELS; x += 2)
	{
	    int32	hi, lo;
	    char	*cp;
	    
	    hi = xlat_tab[*ep++];
	    cp = &cmap[(hi - 128) * 3];
	    hi = (int32)(((*cp >> 1) << 10) | ((*(cp + 1) >> 1) << 5) | (*(cp + 2) >> 1));
	    lo = xlat_tab[*ep++];
	    cp = &cmap[(lo - 128) * 3];
	    lo = (int32)(((*cp >> 1) << 10) | ((*(cp + 1) >> 1) << 5) | (*(cp + 2) >> 1));
	    *ip++ = ((hi << 16) | lo) | 0x80008000;
	}
	
	elev += LINE_PIXELS;
	image += (LINE_PIXELS >> 1);
    }
}

static int32
load_planet (char *file, int32 ss_index, int32 pl_index, int32 mn_index,
	char *xlat_tab, char *cmap, int32 *image)
{
    Stream	*fp;
    int32	retval;
    char	*elev;
    
    if (!(elev = (char *)AllocMem (LINE_PIXELS * HEIGHT, MEMTYPE_ANY)))
	return (0);
	
    retval = 0;
    if (fp = OpenDiskStream (file, 0))
    {
	long	pl_offs;

	if (ss_index == SOL && pl_index == EARTH && mn_index == LUNA) // Earth's moon
	    mn_index--;
	    
	SeekDiskStream (fp, ss_index * sizeof (long), SEEK_SET);
	ReadDiskStream (fp, (char *)&pl_offs, sizeof (pl_offs));
	SeekDiskStream (fp, pl_offs + pl_index * sizeof (long), SEEK_SET);
	ReadDiskStream (fp, (char *)&pl_offs, sizeof (pl_offs));
	SeekDiskStream (fp, pl_offs + mn_index * (WIDTH * HEIGHT), SEEK_SET);
	
	if (load_elevations (fp, elev))
	{
#define LEVEL_ADD	6
	    xlate_topography (elev, xlat_tab + LEVEL_ADD, cmap, image);
	    retval = 1;
	}
	
	CloseDiskStream (fp);
    }
    
    FreeMem (elev, LINE_PIXELS * HEIGHT);
	
    return (retval);
}

static void
set_band_cels (int32 src_y, int32 src_h, int32 band)
{
    int32	i, dst_x0, dst_y0, dst_x1, dst_y1, dx0, dy0, dx1, dy1;

    if (src_y > (HEIGHT >> 1))
    {
	if (src_y + src_h > HEIGHT
		&& (src_h = HEIGHT - src_y + 1) == 0)
	    return;
	dx0 = x1[band + 1];
	dy0 = y1[band + 1];
	dx1 = x1[band];
	dy1 = y1[band];
	dst_x0 = -dx0;
	dst_y0 = -dy0;
	dst_x1 = -dx1;
	dst_y1 = -dy1;
	dx0 -= x0[band + 1];
	dy0 -= y0[band + 1];
	dx1 -= x0[band];
	dy1 -= y0[band];
    }
    else
    {
	if (src_y + src_h > (HEIGHT >> 1)
		&& (src_h = (HEIGHT >> 1) - src_y + 1) == 0)
	    return;
	dx0 = x0[band];
	dy0 = y0[band];
	dx1 = x0[band + 1];
	dy1 = y0[band + 1];
	dst_x0 = dx0;
	dst_y0 = dy0;
	dst_x1 = dx1;
	dst_y1 = dy1;
	dx0 = x1[band] - dx0;
	dy0 = y1[band] - dy0;
	dx1 = x1[band + 1] - dx1;
	dy1 = y1[band + 1] - dy1;
    }
    
    for (i = 0; i < XBANDS; i++)
    {
#define dd	d[i]
#define src_w	src_ws[i]
	Point	quadpts[4];
	int32	d[XBANDS] = { 17, 47, 64, 64, 47, 17 };

	quadpts[0].pt_X = dst_x0;
	quadpts[0].pt_Y = dst_y0;
	quadpts[3].pt_X = dst_x1;
	quadpts[3].pt_Y = dst_y1;

	dst_x0 += (dx0 * dd) >> 8;
	dst_y0 += (dy0 * dd) >> 8;
	dst_x1 += (dx1 * dd) >> 8;
	dst_y1 += (dy1 * dd) >> 8;

	quadpts[1].pt_X = dst_x0;
	quadpts[1].pt_Y = dst_y0;
	quadpts[2].pt_X = dst_x1;
	quadpts[2].pt_Y = dst_y1;

	cur_ccb->ccb_PRE0 &= ~PRE0_VCNT_MASK;
	cur_ccb->ccb_PRE1 &= ~PRE1_TLHPCNT_MASK;
	cur_ccb->ccb_PRE0 |= (src_h - PRE0_VCNT_PREFETCH) << PRE0_VCNT_SHIFT;
	cur_ccb->ccb_PRE1 |= (src_w - PRE1_TLHPCNT_PREFETCH) << PRE1_TLHPCNT_SHIFT;

	cur_ccb->ccb_Width = src_w;
	cur_ccb->ccb_Height = src_h;

	MAP_CEL (cur_ccb, quadpts);
	
	cur_ccb->ccb_Width = cur_ccb->ccb_XPos;
	cur_ccb->ccb_Height = cur_ccb->ccb_YPos;
	
	cur_ccb++;
    }
}

static void
init_rotate_cels ()
{
    int32	i, incr, y;
    
    incr = 1;
    cur_ccb = plCCB;
    for (y = 0, i = 0; y < HEIGHT;)
    {
#define src_h	ystep[i]
	set_band_cels (y, src_h, i);
	y += src_h;
	i += incr;
	if (src_h == 0)
	{
	    --i;
	    incr = -1;
	    if (HEIGHT & 1)
		--i;
	}
    }
#if 0
printf ("using %d cels for rotate\n", cur_ccb - plCCB);
#endif
}

void
SetPlanetTilt (int32 da)
{
    int32	w, i;

    shield_pulse = MAX_PULSE_RED;
    for (i = 0, cur_ccb = plCCB; i < NUM_ROTATE_CELS; i++, cur_ccb++)
    {
	*cur_ccb = planet_ccb;
	cur_ccb->ccb_NextPtr = cur_ccb + 1;
    }
    
    if (!src_ws[0])
    {
	int32	err;
	
	w = (WIDTH / 2) / XBANDS;
	err = XBANDS;
	i = XBANDS - 1;
	do
	{
	    src_ws[i] = w;
	    if ((err -= ((WIDTH / 2) % XBANDS)) <= 0)
	    {
		src_ws[i]++;
		err += XBANDS;
	    }
	} while (i--);

	build_tables (da);
    }
    
    init_rotate_cels ();

    {
	CCB	*ccb;
	
	ccb = plCCB;
	cur_ccb = plCCB + NUM_ROTATE_CELS;
	for (i = 0; i < NUM_ROTATE_CELS; i++, cur_ccb++, ccb++)
	{
	    *cur_ccb = *ccb;
	    cur_ccb->ccb_NextPtr = cur_ccb + 1;
	}
    }
}

static void
draw_black (int32 y, int32 h)
{
    black_rect_ccb.ccb_YPos = y << 16;
    black_rect_ccb.ccb_VDY = h << 16; // why 15 and not 16?!?!
    add_cel (&black_rect_ccb);
}

int
RotatePlanet (int32 x, int32 dx, int32 dy)
{
    int32	i, y;
    int32	incr;
    CCB		*first;

    dx <<= 16;
    dy <<= 16;

    if (pblack_circ_ccb && !pshield_ccb->ccb_HDY)    
    {
	pblack_circ_ccb->ccb_XPos = dx - (pblack_circ_ccb->ccb_Width << 15);
	pblack_circ_ccb->ccb_YPos = dy - (pblack_circ_ccb->ccb_Height << 15);
	pblack_circ_ccb->ccb_HDX = 1 << 20;
	pblack_circ_ccb->ccb_VDY = 1 << 16;
#ifdef SCALE_ROTATE
	if ((scale & SCALE_AMT_MASK) > SCALE_STEP)
	{
	    scale -= SCALE_STEP;
	    SCALE_CEL (pblack_circ_ccb, dx, dy);
	    scale += SCALE_STEP;
	}
	else if (!scale)
#endif
	add_cel (pblack_circ_ccb);
#ifdef KDEBUG
FlushGraphics (TRUE);
printf ("CLEAR...\n");
while (AnyButtonPress (FALSE));
while (!AnyButtonPress (FALSE));
#endif /* KDEBUG */
   }

    incr = 1;
#ifdef SCALE_ROTATE
    if (scale)
    {
	CCB	*ccb;
	
	ccb = plCCB;
	cur_ccb = plCCB + NUM_ROTATE_CELS;
	for (i = 0; i < NUM_ROTATE_CELS; i++, cur_ccb++, ccb++)
	{
	    *cur_ccb = *ccb;
	    cur_ccb->ccb_NextPtr = cur_ccb + 1;
	}
	first = plCCB + NUM_ROTATE_CELS;
    }
    else
#endif
	first = plCCB;
    cur_ccb = first;
    for (y = 0, i = 0; y < HEIGHT;)
    {
#define src_h	ystep[i]
	draw_band (x, y, dx, dy);
	y += src_h;
	i += incr;
	if (src_h == 0)
	{
	    --i;
	    incr = -1;
	    if (HEIGHT & 1)
		--i;
	}
    }
    
    add_cels (first, cur_ccb - 1);

#ifdef KDEBUG
FlushGraphics (TRUE);
printf ("PLANET...\n");
while (AnyButtonPress (FALSE));
while (!AnyButtonPress (FALSE));
#endif /* KDEBUG */
    if (pcirc_ccb)
    {
	pcirc_ccb->ccb_XPos = dx - (pcirc_ccb->ccb_Width << 15);
	pcirc_ccb->ccb_YPos = dy - (pcirc_ccb->ccb_Height << 15);
	pcirc_ccb->ccb_HDX = 1 << 20;
	pcirc_ccb->ccb_VDY = 1 << 16;
#ifdef SCALE_ROTATE
	if (scale)
	    SCALE_CEL (pcirc_ccb, dx, dy);
#endif
	add_cel (pcirc_ccb);
#ifdef KDEBUG
FlushGraphics (TRUE);
printf ("CLIP...\n");
while (AnyButtonPress (FALSE));
while (!AnyButtonPress (FALSE));
#endif /* KDEBUG */
    }
    
    if (pblur_ccb)
    {
	pblur_ccb->ccb_XPos = dx - (pblur_ccb->ccb_Width << 15);
	pblur_ccb->ccb_YPos = dy - (pblur_ccb->ccb_Height << 15);
	pblur_ccb->ccb_HDX = 1 << 20;
	pblur_ccb->ccb_VDY = 1 << 16;
#ifdef SCALE_ROTATE
	if (scale)
	    SCALE_CEL (pblur_ccb, dx, dy);
#endif
	add_cel (pblur_ccb);
#ifdef KDEBUG
FlushGraphics (TRUE);
printf ("BLUR...\n");
while (AnyButtonPress (FALSE));
while (!AnyButtonPress (FALSE));
#endif /* KDEBUG */
    }
    
    if (pshadow_ccb)
    {
	pshadow_ccb->ccb_XPos = dx - (pshadow_ccb->ccb_Width << 15);
	pshadow_ccb->ccb_YPos = dy - (pshadow_ccb->ccb_Height << 15);
	pshadow_ccb->ccb_HDX = 1 << 20;
	pshadow_ccb->ccb_VDY = 1 << 16;
#ifdef SCALE_ROTATE
	if (scale)
	    SCALE_CEL (pshadow_ccb, dx, dy);
#endif
	add_cel (pshadow_ccb);
#ifdef KDEBUG
FlushGraphics (TRUE);
printf ("SHADOW...\n");
while (AnyButtonPress (FALSE));
while (!AnyButtonPress (FALSE));
#endif /* KDEBUG */
    }

    if (!pshield_ccb->ccb_HDY)
    {
	pshield_ccb->ccb_XPos = dx - (pshield_ccb->ccb_Width << 15);
	pshield_ccb->ccb_YPos = dy - (pshield_ccb->ccb_Height << 15);
	pshield_ccb->ccb_HDX = 1 << 20;
	pshield_ccb->ccb_VDY = 1 << 16;
#ifdef SCALE_ROTATE
	if (scale)
	    SCALE_CEL (pshield_ccb, dx, dy);
#endif

	if (shield_pulse & PULSE_NEG_BIT)
	{
	    if (((--shield_pulse) & ~PULSE_NEG_BIT) < MIN_PULSE_RED)
		shield_pulse = (MIN_PULSE_RED + 1);
	}
	else if (((++shield_pulse) & ~PULSE_NEG_BIT) > MAX_PULSE_RED)
	    shield_pulse = (MAX_PULSE_RED - 1) | PULSE_NEG_BIT;
	pshield_ccb->ccb_PIXC =
		PIXC_DUP (PPMPC_1S_PDC | PPMPC_MS_CCB
		| (((shield_pulse & ~PULSE_NEG_BIT) >> PULSE_SCALE) << PPMPC_MF_SHIFT)
		| PPMPC_SF_8 | PPMPC_2S_CFBD | PPMPC_2D_1);
		
	add_cel (pshield_ccb);
#ifdef KDEBUG
FlushGraphics (TRUE);
printf ("SHIELD...\n");
while (AnyButtonPress (FALSE));
while (!AnyButtonPress (FALSE));
while (AnyButtonPress (FALSE));
#endif /* KDEBUG */
    }
    
#ifdef SCALE_ROTATE
    if (scale && ((scale += SCALE_STEP) & SCALE_AMT_MASK) > SCALE_MAX)
	scale = 0;
	
    return (scale != 0);
#else
    return (0);
#endif
}

void
DrawPlanet (int32 x, int32 y, int32 dy, uint32 rgb)
{
    if (rgb || !pshield_ccb->ccb_HDY)
    {
	int32	i, nt, my;

	if (!pshield_ccb->ccb_HDY)
	{
	    dy = HEIGHT + NUM_TINTS;
	    rgb = 0x1f << 10;
	}
	
	my = dy - NUM_TINTS;
	if (my < 0)
	{
	    nt = NUM_TINTS + my + 1;
	    my = 0;
	}
	else if (my >= HEIGHT)
	{
	    nt = NUM_TINTS;
	    my = HEIGHT - 1;
	}
	else
	    nt = NUM_TINTS;
	
	my += y;
	
	surface_ccb.ccb_XPos = x << 16;
	surface_ccb.ccb_YPos = y << 16;
	surface_ccb.ccb_SourcePtr = planet_ccb.ccb_SourcePtr;
	surface_ccb.ccb_PRE0 =
		PRE0_BGND
		| (((dy + 1) - PRE0_VCNT_PREFETCH) << PRE0_VCNT_SHIFT)
		| PRE0_LINEAR
		| PRE0_BPP_16;
	surface_ccb.ccb_PIXC =
		PIXC_DUP (PPMPC_1S_PDC | PPMPC_MS_CCB | PPMPC_MF_6
		| PPMPC_SF_8 | PPMPC_2S_0);
	add_cel (&surface_ccb);
	
	nt = NUM_TINTS - nt;
	for (i = nt; i < NUM_TINTS; i++, my++)
	{
	    if (my >= y && my < y + HEIGHT)
	    {
		if (i == 0)
		{
		    tint_ccb[i].ccb_YPos = y << 16;
		    tint_ccb[i].ccb_VDY = (my - y + 1) << 16;
		}
		else
		    tint_ccb[i].ccb_YPos = my << 16;
		tint_ccb[i].ccb_XPos = x << 16;
		*((uint32 *)tint_ccb[i].ccb_SourcePtr) = rgb | (rgb << 16) | 0x80008000;
		add_cel (&tint_ccb[i]);
	    }
	}
    }
    else
    {
	surface_ccb.ccb_XPos = x << 16;
	surface_ccb.ccb_YPos = y << 16;
	surface_ccb.ccb_SourcePtr = planet_ccb.ccb_SourcePtr;
	surface_ccb.ccb_PRE0 =
		PRE0_BGND
		| ((HEIGHT - PRE0_VCNT_PREFETCH) << PRE0_VCNT_SHIFT)
		| PRE0_LINEAR
		| PRE0_BPP_16;
	surface_ccb.ccb_PIXC = PIXC_UNCODED16;
	add_cel (&surface_ccb);
    }
}

#define SET_PLUT(c,s,x,y) \
	((c)->ccb_Width = (((*((s) + ((x) >> 1))) >> (((x) & 1) << 4)) & 0xffff) | 0x8000)
	
void
SetPlanetZoom ()
{
    int32	i;

    old_sx = -1;
    
    zoom_batch = 0;

#ifdef OLD_MASK    
    for (i = 0; i < NUM_MASKS; i++)
    {
	int32	j;
	
	for (j = 0; j < MASK_HEIGHT; j++)
	    mask_buf[i][j][0] = random ();
	for (j = SURFACE_ADD; j < MASK_WIDTH - SURFACE_ADD; j++)
	    //mask_buf[i][j][0] |= 0x01fe0000;
	    mask_buf[i][j][0] |= 0x1f000000;
    }
#else
    // to get rid of "notches" at bottom of planet surface
    for (i = 0; i < NUM_MASKS; i++)
 	mask_buf[i][MASK_HEIGHT - 3][0] |= 0x1f000000;
#endif
    
    for (i = 0; i < HEIGHT; i++)
    {
	int32	j;
	
	for (j = 0; j < LINE_PIXELS; j++)
	{
#ifdef OLD_MASK
	    rmap[i][j] = random () & (NUM_MASKS - 1);
#else
	    rmap[i][j] = random () % NUM_MASKS;
#endif
	}
    }
    
    for (i = 0, cur_ccb = plCCB; i < sizeof (plCCB) / sizeof (plCCB[0]); i++, cur_ccb++)
    {
	*cur_ccb = mask_ccb;
	cur_ccb->ccb_PLUTPtr = (void *)&cur_ccb->ccb_Width;
	cur_ccb->ccb_NextPtr = cur_ccb + 1;
    }
}

static void
draw_surface_cels (int32 x, int32 y, int32 w, int32 h)
{
    ubyte	*rmc, *rmr;
    uint32	*sp;
    int32	sx, sy, dx, dy;
    int32	lx, ly;
    int32	osx, osy, olx, oly;
    int32	_sx, _dx;
    int32	rx, ry;

    osx = old_sx + x;
    if (osx < 0)
	osx += WIDTH << SURFACE_SHIFT;
    else if (osx >= WIDTH << SURFACE_SHIFT)
	osx -= WIDTH << SURFACE_SHIFT;
    olx = osx + w - 1;

    osy = old_sy + y;
    oly = osy + h - 1;
    
    if (osy < 0)
    {
	h += osy;
	y -= osy;
	if (h <= 0)
	    return;
	osy = 0;
    }
    else if (oly > (HEIGHT << SURFACE_SHIFT) - 1)
    {
	oly -= (HEIGHT << SURFACE_SHIFT) - 1;
	h -= oly;
	if (h <= 0)
	    return;
	oly = (HEIGHT << SURFACE_SHIFT) - 1;
    }
    
    sx = (osx - (MASK_WIDTH >> 1) + SURFACE_ADD) >> SURFACE_SHIFT;
    sy = (osy - (MASK_HEIGHT >> 1) + SURFACE_ADD) >> SURFACE_SHIFT;
    lx = ((olx + (MASK_WIDTH >> 1)) & ~SURFACE_ADD) >> SURFACE_SHIFT;
    ly = ((oly + (MASK_HEIGHT >> 1)) & ~SURFACE_ADD) >> SURFACE_SHIFT;
    
    if (sy < 0)
	sy = 0;
    else if (ly >= HEIGHT)
	ly = HEIGHT - 1;

    dx = (x - (osx - ((sx << SURFACE_SHIFT) - (MASK_WIDTH >> 1)))) << 16;
    dy = (y - (osy - ((sy << SURFACE_SHIFT) - (MASK_HEIGHT >> 1)))) << 16;

#if 0
if (old_sy + y < 0)
{
printf ("sx,sy %ld,%ld -- ", sx, sy);
printf ("lx,ly %ld,%ld  ", lx, ly);
printf ("osx,osy %ld,%ld -- ", osx, osy);
printf ("olx,oly %ld,%ld\n", olx, oly);
printf ("old_sx,old_sy %ld,%ld  ", old_sx, old_sy);
printf ("x,y %ld,%ld -- ", x, y);
printf ("w,h %ld,%ld  ", w, h);
printf ("dx,dy %ld,%ld\n", dx >> 16, dy >> 16);
x = 0xffff;
}
#endif
    ry = sy;
    if (ry < 0)
	ry += HEIGHT;
    else if (ry >= HEIGHT)
	ry -= HEIGHT;
    
    w = lx - sx + 1;
    h = ly - sy + 1;
    _sx = sx;
    _dx = dx;

    sp = (uint32 *)planet_ccb.ccb_SourcePtr + ry * (LINE_PIXELS >> 1);
    rmr = rmap[ry];
    
    do
    {
	int32	_w;
	
	_w = w;
	rx = sx;
	if (rx < 0)
	    rx += WIDTH;
	else if (rx >= WIDTH)
	    rx -= WIDTH;
	rmc = &rmr[rx];
	do
	{
	    int32	pix;
	    int32	nx, ny;
	    
	    cur_ccb->ccb_SourcePtr = (CelData *)mask_buf[*rmc];
	    SET_PLUT (cur_ccb, sp, rx, ry);
	    cur_ccb->ccb_XPos = dx;
	    cur_ccb->ccb_YPos = dy;
	    cur_ccb->ccb_PRE0 &= ~(PRE0_VCNT_MASK | PRE0_SKIPX_MASK);
	    cur_ccb->ccb_PRE1 &= ~PRE1_TLHPCNT_MASK;
	    
	    nx = (sx << SURFACE_SHIFT) - (MASK_WIDTH >> 1);
	    if (nx < osx)
	    {
		nx = osx - nx;
		cur_ccb->ccb_XPos += nx << 16;
		cur_ccb->ccb_PRE0 |= nx << PRE0_SKIPX_SHIFT;
	    }
	    
	    nx = (sx << SURFACE_SHIFT) + (MASK_WIDTH >> 1);
	    if (nx > olx)
	    {
		nx = MASK_WIDTH - (nx - olx);
		cur_ccb->ccb_PRE1 |= (nx - PRE1_TLHPCNT_PREFETCH) << PRE1_TLHPCNT_SHIFT;
	    }
	    else
		cur_ccb->ccb_PRE1 |= (MASK_WIDTH - PRE1_TLHPCNT_PREFETCH) << PRE1_TLHPCNT_SHIFT;

	    pix = MASK_HEIGHT;
	    ny = (sy << SURFACE_SHIFT) - (MASK_HEIGHT >> 1);
	    if (ny < osy)
	    {
		ny = osy - ny;
		pix -= ny;
		cur_ccb->ccb_YPos += ny << 16;
		cur_ccb->ccb_SourcePtr = (CelData *)((uint32 *)cur_ccb->ccb_SourcePtr + (ny << 1));
	    }
	    
	    ny = (sy << SURFACE_SHIFT) + (MASK_HEIGHT >> 1);
	    if (ny > oly)
		pix -= ny - oly;
	    cur_ccb->ccb_PRE0 |= (pix - PRE0_VCNT_PREFETCH) << PRE0_VCNT_SHIFT;
#if 0
if (x == 0xffff && cur_ccb->ccb_XPos >> 16 == 63)
{
printf ("Flags 0x%08lx  Next 0x%08lx\n", cur_ccb->ccb_Flags, cur_ccb->ccb_NextPtr);
printf ("    Source 0x%08lx  PLUT 0x%08lx\n", cur_ccb->ccb_SourcePtr, cur_ccb->ccb_PLUTPtr);
printf ("    XPos %ld  YPos %ld\n", cur_ccb->ccb_XPos >> 16, cur_ccb->ccb_YPos >> 16);
printf ("    rx,ry %ld,%ld\n", rx, ry);
printf ("    PRE0 0x%08lx  PRE1 0x%08lx\n", cur_ccb->ccb_PRE0, cur_ccb->ccb_PRE1);
printf ("    Width %ld  Height %ld\n", cur_ccb->ccb_Width, cur_ccb->ccb_Height);
printf ("    SKIPX %ld, TLHPCNT %ld  ",
	((cur_ccb->ccb_PRE0 & PRE0_SKIPX_MASK) >> PRE0_SKIPX_SHIFT),
	((cur_ccb->ccb_PRE1 & PRE1_TLHPCNT_MASK) >> PRE1_TLHPCNT_SHIFT) + PRE1_TLHPCNT_PREFETCH);
printf ("    VCNT %ld, source y +%d, using mask %d\n",
	((cur_ccb->ccb_PRE0 & PRE0_VCNT_MASK) >> PRE0_VCNT_SHIFT) + PRE0_VCNT_PREFETCH,
	((char *)cur_ccb->ccb_SourcePtr - (char *)&mask_buf[*rmc]) / (sizeof (int32) << 1),
	*rmc);
}
#endif
	    cur_ccb->ccb_Flags &= ~CCB_LAST;
	    if (++cur_ccb == &plCCB[NUM_CELS])
	    {
		--cur_ccb;
		add_cels (plCCB, cur_ccb);
		_ThreeDO_batch_cels (0);
		cur_ccb->ccb_NextPtr = cur_ccb + 1;
		cur_ccb = plCCB;
	    }
	    
	    dx += SURFACE_UNIT << 16;
	    ++sx;
	    if (++rx >= WIDTH)
	    {
		rx -= WIDTH;
		rmc = &rmr[rx];
	    }
	    else
		rmc++;
	} while (--_w);
	
	sx = _sx;
	dx = _dx;
	
	dy += SURFACE_UNIT << 16;
	++sy;
	if (++ry >= HEIGHT)
	    break;
	else
	{
	    rmr += sizeof (rmap[0]);
	    sp += (LINE_PIXELS >> 1);
	}
    } while (--h);
}

#define REPAIR_BLACK() \
	do \
	{ \
	    if (old_sy < 0) \
		draw_black (0, -old_sy); \
	    else if (old_sy > (HEIGHT << SURFACE_SHIFT) - SURFACE_HEIGHT) \
		draw_black ((HEIGHT << SURFACE_SHIFT) - old_sy, \
			old_sy + SURFACE_HEIGHT - (HEIGHT << SURFACE_SHIFT)); \
	} while (0)

void
BatchZoomedPlanet ()
{
    if (zoom_batch++ == 0)
    {
	cur_ccb = plCCB;
	if (last_zoom_ccb)
	    last_zoom_ccb->ccb_NextPtr = last_zoom_ccb + 1;
    }
}

void
UnbatchZoomedPlanet ()
{
    if (zoom_batch && --zoom_batch == 0 && cur_ccb != plCCB)
    {
	last_zoom_ccb = cur_ccb - 1;
	add_cels (plCCB, last_zoom_ccb);
    }
}

void
RepairZoomedPlanet (int32 x, int32 y, int32 w, int32 h)
{
    if (old_sx == -1)
    {
	old_sx = x;
	old_sy = y;
	REPAIR_BLACK ();
	if (old_sy < 0)
	    draw_surface_cels (0, -old_sy, SURFACE_WIDTH, SURFACE_HEIGHT);
	else
	    draw_surface_cels (0, 0, SURFACE_WIDTH, SURFACE_HEIGHT);
	return;
    }

    draw_surface_cels (x, y, w, h);
}

void
ScrollZoomedPlanet (int32 dx, int32 dy, int32 clip_x, int32 clip_y, void *bm_Buffer)
{
    if (dx == 0 && dy == 0)
    {
	REPAIR_BLACK ();
	return;
    }
	
    if ((old_sx -= dx) < 0)
	old_sx += WIDTH << SURFACE_SHIFT;
    else if (old_sx >= WIDTH << SURFACE_SHIFT)
	old_sx -= WIDTH << SURFACE_SHIFT;
	
    if ((old_sy -= dy) < MIN_Y)
    {
	dy += old_sy - MIN_Y;
	old_sy = MIN_Y;
    }
    else if (old_sy >= MAX_Y)
    {
	dy -= MAX_Y - old_sy;
	old_sy = MAX_Y;
    }
    
    if (dx == 0 && dy == 0)
    {
	REPAIR_BLACK ();
	return;
    }

    REPAIR_BLACK ();

    if (dy > 0) // repair top
    {
	repair_ccb[0].ccb_XPos = 0;
	repair_ccb[0].ccb_YPos = 0;
	repair_ccb[0].ccb_HDX = SURFACE_WIDTH << 20;
	repair_ccb[0].ccb_VDY = dy << 16;
	add_cel (&repair_ccb[0]);
	RepairZoomedPlanet (0, 0, SURFACE_WIDTH, dy);
    }
    if (dx > 0) // repair left
    {
	repair_ccb[1].ccb_XPos = 0;
	repair_ccb[1].ccb_YPos = 0;
	repair_ccb[1].ccb_HDX = dx << 20;
	repair_ccb[1].ccb_VDY = SURFACE_HEIGHT << 16;
	add_cel (&repair_ccb[1]);
	if (old_sy < 0)
	    RepairZoomedPlanet (0, -old_sy, dx, SURFACE_HEIGHT);
	else
	    RepairZoomedPlanet (0, 0, dx, SURFACE_HEIGHT);
    }
    if (dx < 0) // repair right
    {
	repair_ccb[1].ccb_XPos = (SURFACE_WIDTH + dx) << 16;
	repair_ccb[1].ccb_YPos = 0;
	repair_ccb[1].ccb_HDX = -dx << 20;
	repair_ccb[1].ccb_VDY = SURFACE_HEIGHT << 16;
	add_cel (&repair_ccb[1]);
	if (old_sy < 0)
	    RepairZoomedPlanet (SURFACE_WIDTH + dx, -old_sy, -dx, SURFACE_HEIGHT);
	else
	    RepairZoomedPlanet (SURFACE_WIDTH + dx, 0, -dx, SURFACE_HEIGHT);
    }
    if (dy < 0) // repair bottom
    {
	repair_ccb[0].ccb_XPos = 0;
	repair_ccb[0].ccb_YPos = (SURFACE_HEIGHT + dy) << 16;
	repair_ccb[0].ccb_HDX = SURFACE_WIDTH << 20;
	repair_ccb[0].ccb_VDY = -dy << 16;
	add_cel (&repair_ccb[0]);
	RepairZoomedPlanet (0, SURFACE_HEIGHT + dy, SURFACE_WIDTH, -dy);
    }
}

typedef struct
{
    ulong	TypeIndexAndFlags;
    ulong	HotSpot;
    ulong	Bounds;
    long	DataOffs;
} FRAME_DESC;

#define PARSE_CEL(ccb,f) \
	do \
	{ \
	    uint32	*_p; \
	    \
	    _p = (uint32 *)((uchar *)(f) + (f)->DataOffs); \
	    (ccb) = ParseCel ((void *)(_p + 1), *_p); \
	} while (0)

int
BuildPlanet (int32 ss_index, int32 pl_index, int32 mn_index,
	char *xlat_tab, char *cmap, int32 *image, int32 shielded,
	FRAME_DESC *shadow_f, FRAME_DESC *planet_f, int32 at_planet)
{
    int32	i;

//printf ("BuildPlanet: %ld %ld %ld\n", ss_index, pl_index, mn_index);
    if (planet_f)
    {
	CCB	*ccb;
	
	PARSE_CEL (ccb, planet_f);
	memcpy (image, ccb->ccb_SourcePtr, LINE_PIXELS * HEIGHT * 2);
    }
    else if (!load_planet ("starcon2/planet.dat",
	    ss_index, pl_index, mn_index,
	    xlat_tab, cmap, image))
	return (0);
	
    src_ws[0] = 0; // for init flag in SetPlanetTilt
	
#ifdef SCALE_ROTATE
    if (!at_planet)
    {
	i = random ();
	scale = 1 | ((2 - (i & 2)) << SCALE_X_SHIFT) | ((2 - ((i >>= 1) & 2)) << SCALE_Y_SHIFT);
    }
    else
	scale = 0;
#endif

    planet_ccb.ccb_Flags = CEL_FLAGS;
    planet_ccb.ccb_Width = WIDTH + WRAP_EXTRA;
    planet_ccb.ccb_Height = HEIGHT;
    planet_ccb.ccb_PRE0 =
	    PRE0_BGND
	    | ((HEIGHT - PRE0_VCNT_PREFETCH) << PRE0_VCNT_SHIFT)
	    | PRE0_LINEAR
	    | PRE0_BPP_16;
    planet_ccb.ccb_PRE1 =
	    PRE1_TLLSB_PDC0
	    | ((WIDTH + WRAP_EXTRA - PRE1_TLHPCNT_PREFETCH) << PRE1_TLHPCNT_SHIFT)
	    | (((LINE_PIXELS >> 1) - PRE1_WOFFSET_PREFETCH) << PRE1_WOFFSET10_SHIFT);
    planet_ccb.ccb_SourcePtr = (CelData *)image;
    planet_ccb.ccb_PIXC = PIXC_UNCODED16;
    
    surface_ccb = planet_ccb;
    surface_ccb.ccb_HDX = 1 << 20;
    surface_ccb.ccb_HDY = 0;
    surface_ccb.ccb_VDX = 0;
    surface_ccb.ccb_VDY = 1 << 16;
    surface_ccb.ccb_HDDX = 0;
    surface_ccb.ccb_HDDY = 0;
    surface_ccb.ccb_PRE1 &= ~PRE1_TLHPCNT_MASK;
    surface_ccb.ccb_PRE1 |= (WIDTH - PRE1_TLHPCNT_PREFETCH) << PRE1_TLHPCNT_SHIFT;
    
    for (i = 0; i < NUM_TINTS; i++)
    {
	tint_ccb[i] = surface_ccb;
	tint_ccb[i].ccb_HDX = WIDTH << 20;
	tint_ccb[i].ccb_VDY = 1 << 16;
	tint_ccb[i].ccb_PRE0 = PRE0_BGND | PRE0_LINEAR | PRE0_BPP_16;
	tint_ccb[i].ccb_PRE1 = PRE1_TLLSB_PDC0;
	tint_ccb[i].ccb_SourcePtr = (CelData *)&tint_ccb[i].ccb_Width;
	tint_ccb[i].ccb_PIXC =
		PIXC_DUP (PPMPC_1S_PDC | PPMPC_MS_CCB | ((i + 4) << PPMPC_MF_SHIFT)
		| PPMPC_SF_8 | PPMPC_2S_CFBD | PPMPC_2D_1);
    }

    if (shielded)
    {
	pshield_ccb->ccb_HDY = 0;
	tint_ccb[0].ccb_PIXC = PIXC_DUP (PPMPC_1S_PDC | PPMPC_MS_CCB | PPMPC_MF_8
		| PPMPC_SF_8 | PPMPC_2S_CFBD | PPMPC_2D_1);
    }
    else
	pshield_ccb->ccb_HDY = 1; // used to flag whether shielded or not

    PARSE_CEL (pshadow_ccb, shadow_f);

    pshadow_ccb->ccb_PIXC =
	    PIXC_DUP (PPMPC_1S_CFBD | PPMPC_MF_2
	    | PPMPC_SF_2 | PPMPC_2S_PDC | (1 << 1));
    SET_CEL_FLAGS (pshadow_ccb, CCB_USEAV);
    CLR_CEL_FLAGS (pshadow_ccb, CCB_BGND);
    
    return (1);
}

CCB *
GetCelArray ()
{
    return (plCCB);
}

void
InitPlanetStuff (FRAME_DESC *pl_circ_f, FRAME_DESC *pl_blur_f, FRAME_DESC *shield_f)
{
    int32	i;
    
    if (!planet_stuff)
	planet_stuff = (PLANET_STUFF *)ThreedoAlloc (sizeof (PLANET_STUFF));
    else
	memset ((void *)planet_stuff, 0, sizeof (PLANET_STUFF));

    PARSE_CEL (pshield_ccb, shield_f);

    black_circ_ccb = *pshield_ccb;
    pblack_circ_ccb = &black_circ_ccb;
#if 0
    SET_CEL_FLAGS (pblack_circ_ccb, CCB_LDPLUT | CCB_PPABS);
    CLR_CEL_FLAGS (pblack_circ_ccb, CCB_BGND);
    pblack_circ_ccb->ccb_PRE0 &= ~(PRE0_LINEAR | PRE0_BGND);
#else
    SET_CEL_FLAGS (pblack_circ_ccb, CCB_PPABS);
    CLR_CEL_FLAGS (pblack_circ_ccb, CCB_BGND);
#endif
    pblack_circ_ccb->ccb_PLUTPtr = black_circ_plut;
    black_circ_plut[0] = 0x80008001;
    for (i = 1; i < 16; i++)
	black_circ_plut[i] = 0x80018001;

    PARSE_CEL (pcirc_ccb, pl_circ_f);
    pcirc_ccb->ccb_PIXC =
	    PIXC_DUP (PPMPC_1S_CFBD | PPMPC_MF_2
	    | PPMPC_SF_2 | PPMPC_2S_PDC | (1 << 1));
    SET_CEL_FLAGS (pcirc_ccb, CCB_USEAV);
    CLR_CEL_FLAGS (pcirc_ccb, CCB_BGND);
    
    PARSE_CEL (pblur_ccb, pl_blur_f);
    pblur_ccb->ccb_PIXC =
	    PIXC_DUP (PPMPC_1S_CFBD | PPMPC_MF_2
	    | PPMPC_SF_2 | PPMPC_2S_PDC | (1 << 1));
    SET_CEL_FLAGS (pblur_ccb, CCB_USEAV);
    CLR_CEL_FLAGS (pblur_ccb, CCB_BGND);
	    
    black_rect_ccb.ccb_Flags = (CEL_FLAGS | CCB_LAST) & ~CCB_PPABS;
    black_rect_ccb.ccb_XPos = 0;
    black_rect_ccb.ccb_HDX = SURFACE_WIDTH << 20;
    black_rect_ccb.ccb_VDY = 1 << 16;
    black_rect_ccb.ccb_PIXC = PIXC_UNCODED16;
    black_rect_ccb.ccb_PRE0 = PRE0_BGND
	    | PRE0_LINEAR
	    | (PRE0_BPP_16 << PRE0_BPP_SHIFT);
#if 0
    black_rect_ccb.ccb_PRE1 = PRE1_LRFORM
	    | PRE1_TLLSB_PDC0;
#else
    black_rect_ccb.ccb_PRE1 = PRE1_TLLSB_PDC0;
#endif
    black_rect_ccb.ccb_Width = 0x80008000;
    black_rect_ccb.ccb_SourcePtr = (CelData *)&black_rect_ccb.ccb_Width;
    
    repair_ccb[0] = repair_ccb[1] = black_rect_ccb;
    
    mask_ccb.ccb_Flags = (CEL_FLAGS | CCB_LDPLUT) & ~(CCB_BGND | CCB_PLUTPOS);
    mask_ccb.ccb_Width = MASK_WIDTH;
    mask_ccb.ccb_Height = MASK_HEIGHT;
    mask_ccb.ccb_HDX = 1 << 20;
    mask_ccb.ccb_VDY = 1 << 16;
    mask_ccb.ccb_PRE0 =
	    ((MASK_HEIGHT - PRE0_VCNT_PREFETCH) << PRE0_VCNT_SHIFT)
	    | PRE0_BPP_1;
    mask_ccb.ccb_PRE1 =
	    PRE1_TLLSB_PDC0
	    | ((MASK_WIDTH - PRE1_TLHPCNT_PREFETCH) << PRE1_TLHPCNT_SHIFT)
	    | ((2 - PRE1_WOFFSET_PREFETCH) << PRE1_WOFFSET8_SHIFT);
//    mask_ccb.ccb_PIXC =
//	    PIXC_DUP (PPMPC_1S_PDC | PPMPC_MS_CCB | PPMPC_MF_8
//		    | PPMPC_SF_8 | PPMPC_2S_0 | PPMPC_2D_1);
    mask_ccb.ccb_PIXC =
	    PIXC_DUP (PPMPC_1S_CFBD | PPMPC_MS_CCB | PPMPC_MF_1
		    | PPMPC_SF_16 | PPMPC_2S_PDC | PPMPC_2D_1);
}

void
UninitPlanetStuff ()
{
    if (planet_stuff)
    {
	ThreedoFree (planet_stuff, sizeof (PLANET_STUFF));
	planet_stuff = 0;
    }
}



    

