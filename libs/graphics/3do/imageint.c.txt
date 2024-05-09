#include "gfxintrn.h"
#include "vga.h"

PROC(
BOOLEAN _image_intersect, (pImageBox0, pImageBox1, pIRect),
    ARG		(PIMAGE_BOX	pImageBox0)
    ARG		(PIMAGE_BOX	pImageBox1)
    ARG_END	(PRECT		pIRect)
)
{
    COORD	x0, x1;
    COUNT	width, height;
    COUNT	skip_bytes;
    UWORD	x_flip0, x_flip1;
    LPBYTE	lpSM0, lpSM1;

    x_flip0 = GetFrameFlags (pImageBox0->FramePtr);
    x_flip1 = GetFrameFlags (pImageBox1->FramePtr);
    if (!(x_flip0 & x_flip1 & DATA_PACKED))
	return (TRUE);

    width = pIRect->extent.width;

    height = pIRect->corner.y - pImageBox0->Box.corner.y;
    lpSM0 = (LPBYTE)pImageBox0->FramePtr + pImageBox0->FramePtr->DataOffs;
    while (height--)
    {
	skip_bytes = (MAKE_WORD (lpSM0[1], lpSM0[0]) + 2) << 2;
	lpSM0 += skip_bytes;
    }
    x0 = pIRect->corner.x - pImageBox0->Box.corner.x;
    if (x_flip0 &= X_FLIP)
	x0 = GetFrameWidth (pImageBox0->FramePtr) - (x0 + width);

    height = pIRect->corner.y - pImageBox1->Box.corner.y;
    lpSM1 = (LPBYTE)pImageBox1->FramePtr + pImageBox1->FramePtr->DataOffs;
    while (height--)
    {
	skip_bytes = (MAKE_WORD (lpSM1[1], lpSM1[0]) + 2) << 2;
	lpSM1 += skip_bytes;
    }
    x1 = pIRect->corner.x - pImageBox1->Box.corner.x;
    if (x_flip1 &= X_FLIP)
	x1 = GetFrameWidth (pImageBox1->FramePtr) - (x1 + width);

    x_flip0 = (x_flip0 ^ x_flip1) & X_FLIP;
    height = pIRect->extent.height;
    do
    {
	BYTE	code;
	COORD	src_x, dst_x;
	COORD	IntersectCoords[SCREEN_WIDTH / 2], *pIC;
	COUNT	num_chances;
	SIZE	w;
	LPBYTE	lpS;

	num_chances = 0;

	lpS = lpSM0;
	skip_bytes = (MAKE_WORD (lpSM0[1], lpSM0[0]) + 2) << 2;
	lpSM0 += skip_bytes;
	lpS += sizeof (BYTE) * 2;

	dst_x = 0;
	src_x = x0;
	w = width;
	pIC = IntersectCoords;
	while (code = *lpS++)
	{
	    BYTE	pack_type, num_pixels;

	    pack_type = PACK_TYPE (code);
	    num_pixels = PACK_COUNT (code);
	    switch (pack_type)
	    {
		case PACK_LITERAL:
		    lpS += num_pixels;
		    break;
		case PACK_REPEAT:
		    ++lpS;
		    break;
	    }

	    if (src_x)
	    {
		if ((src_x -= num_pixels) >= 0)
		    continue;

		num_pixels = -src_x;
		src_x = 0;
	    }
	    if ((w -= num_pixels) < 0)
		num_pixels += w;

	    if (pack_type != PACK_TRANS)
	    {
		*pIC++ = dst_x;
		*pIC++ = dst_x + (num_pixels - 1);
		++num_chances;
	    }

	    if (w <= 0)
		break;

	    dst_x += num_pixels;
	}

	lpS = lpSM1;
	skip_bytes = (MAKE_WORD (lpSM1[1], lpSM1[0]) + 2) << 2;
	lpSM1 += skip_bytes;
	if (num_chances == 0)
	    continue;
	lpS += sizeof (BYTE) * 2;

	dst_x = 0;
	src_x = x1;
	w = width;
	pIC = IntersectCoords;
	if (x_flip0)
	    pIC += num_chances - 1;
	while (code = *lpS++)
	{
	    BYTE	pack_type, num_pixels;

	    pack_type = PACK_TYPE (code);
	    num_pixels = PACK_COUNT (code);
	    switch (pack_type)
	    {
		case PACK_LITERAL:
		    lpS += num_pixels;
		    break;
		case PACK_REPEAT:
		    ++lpS;
		    break;
	    }

	    if (src_x)
	    {
		if ((src_x -= num_pixels) >= 0)
		    continue;

		num_pixels = -src_x;
		src_x = 0;
	    }
	    if ((w -= num_pixels) < 0)
		num_pixels += w;

	    if (pack_type != PACK_TRANS)
	    {
		if (!x_flip0)
		{
		    while (dst_x > pIC[1])
		    {
			if (--num_chances == 0)
			    goto NextScan;
			pIC += 2;
		    }
		    if (dst_x + num_pixels > pIC[0])
			return (TRUE);
		}
		else
		{
		    while (dst_x >= width - pIC[0])
		    {
			if (--num_chances == 0)
			    goto NextScan;
			pIC -= 2;
		    }
		    if (dst_x + num_pixels >= width - pIC[1])
			return (TRUE);
		}
	    }

	    if (w <= 0)
		break;

	    dst_x += num_pixels;
	}
NextScan:
	;
    } while (--height);

    return (FALSE);
}

