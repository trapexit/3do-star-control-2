#include "gfxintrn.h"

static BYTE	char_delta_array[MAX_DELTAS];

PROC(
FONT SetContextFont, (Font),
    ARG_END	(FONT		Font)
)
{
    FONT	LastFont;

    LastFont = (FONT)_CurFontPtr;
    _CurFontPtr = (FONTPTR)Font;
    if (ContextActive ())
	SwitchContextFont (Font);

    return (LastFont);
}

PROC(
BOOLEAN DestroyFont, (FontRef),
    ARG_END	(FONT_REF	FontRef)
)
{
    if (FontRef == 0)
	return (FALSE);

    if (_CurFontPtr && _CurFontPtr->FontRef == FontRef)
	SetContextFont ((FONT)0);

    return (FreeFont (FontRef));
}

PROC(
FONT CaptureFont, (FontRef),
    ARG_END	(FONT_REF	FontRef)
)
{
    FONTPTR	FontPtr;

    if (FontPtr = LockFont (FontRef))
	FontPtr->FontRef = FontRef;

    return ((FONT)FontPtr);
}

PROC(
FONT_REF ReleaseFont, (Font),
    ARG_END	(FONT		Font)
)
{
    FONTPTR	FontPtr;

    if (FontPtr = (FONTPTR)Font)
    {
	FONT_REF	FontRef;

	FontRef = FontPtr->FontRef;
	UnlockFont (FontRef);

	return (FontRef);
    }

    return (0);
}

PROC(
void DrawText, (lpText),
    ARG_END	(LPTEXT		lpText)
)
{
    SetPrimType (&_locPrim, TEXT_PRIM);
    _locPrim.Object.Text = *lpText;

    DrawBatch (&_locPrim, 0, BATCH_SINGLE);
}

PROC(
BOOLEAN GetContextFontExtents, (pheight, pmaxascent, pmaxdescent),
    ARG		(PSIZE		pheight)
    ARG		(PSIZE		pmaxascent)
    ARG_END	(PSIZE		pmaxdescent)
)
{
    if (_CurFontPtr != 0)
    {
	*pheight = (SIZE)_CurFontPtr->Leading;
	*pmaxascent = (SIZE)_CurFontPtr->MaxAscender;
	*pmaxdescent = (SIZE)_CurFontPtr->MaxDescender;

	return (TRUE);
    }

    *pheight = *pmaxascent = *pmaxdescent = 0;
    return (FALSE);
}

PROC(
BOOLEAN TextRect, (lpText, pRect, pdelta),
    ARG		(LPTEXT		lpText)
    ARG		(register PRECT	pRect)
    ARG_END	(PBYTE		pdelta)
)
{
    FONTPTR	FontPtr;

    if ((FontPtr = _CurFontPtr) != 0)
    {
	BYTE	next_ch;
	COUNT	num_chars;
	COORD	top_y, bot_y;
	SIZE	width;
	LPBYTE	lpStr;

	num_chars = lpText->CharCount;
	if (pdelta == 0)
	{
	    pdelta = char_delta_array;
	    if (num_chars > MAX_DELTAS)
		num_chars = MAX_DELTAS;
	}

	top_y = bot_y = 0;
	width = 0;
	lpStr = lpText->lpStr;
	if ((next_ch = *lpStr++) == '\0')
	    num_chars = 0;
	next_ch -= FIRST_CHAR;
	while (num_chars--)
	{
	    BYTE	ch;
	    SIZE	last_width;

	    last_width = width;

	    ch = next_ch;
	    if ((next_ch = *lpStr++) == '\0')
		num_chars = 0;
	    next_ch -= FIRST_CHAR;
	    if (ch < MAX_CHARS && GetFrameWidth (&FontPtr->CharDesc[ch]))
	    {
		register COORD	y;

		y = -GetFrameHotY (&FontPtr->CharDesc[ch]);
		if (y < top_y)
		    top_y = y;
		y += GetFrameHeight (&FontPtr->CharDesc[ch]);
		if (y > bot_y)
		    bot_y = y;

		width += GetFrameWidth (&FontPtr->CharDesc[ch])
			+ FontPtr->CharSpace;
		if (num_chars && next_ch < MAX_CHARS
			&& !(FontPtr->KernTab[ch]
			& (FontPtr->KernTab[next_ch] >> 2)))
		    width -= FontPtr->KernAmount;
	    }

	    *pdelta++ = (BYTE)(width - last_width);
	}

	if (width > 0 && (bot_y -= top_y) > 0)
	{
	    *--pdelta -= FontPtr->CharSpace;
	    width -= FontPtr->CharSpace;

	    if (lpText->align == ALIGN_LEFT)
		pRect->corner.x = 0;
	    else if (lpText->align == ALIGN_CENTER)
		pRect->corner.x = -(width >> 1);
	    else
		pRect->corner.x = -width;
	    pRect->corner.y = top_y;
	    pRect->extent.width = width;
	    pRect->extent.height = bot_y;

	    pRect->corner.x += lpText->baseline.x;
	    pRect->corner.y += lpText->baseline.y;

	    lpText->CharCount = lpStr - lpText->lpStr - 1;

	    return (TRUE);
	}
    }

    pRect->corner = lpText->baseline;
    pRect->extent.width = 0;
    pRect->extent.height = 0;

    return (FALSE);
}

PROC(
void _text_blt, (pClipRect, PrimPtr),
    ARG		(PRECT		pClipRect)
    ARG_END	(PRIMITIVEPTR	PrimPtr)
)
{
    FONTPTR	FontPtr;

    if ((FontPtr = _CurFontPtr) != 0)
    {
	BYTE		next_ch;
	COUNT		num_chars;
	LPSTR		lpStr;
	TEXTPTR		TextPtr;
	PRIMITIVE	locPrim;

	SetPrimType (&locPrim, STAMPFILL_PRIM);
	SetPrimColor (&locPrim, _get_context_fg_color ());

	TextPtr = &PrimPtr->Object.Text;
	locPrim.Object.Stamp.origin.x = _save_stamp.origin.x;
	locPrim.Object.Stamp.origin.y = TextPtr->baseline.y;
	num_chars = TextPtr->CharCount;

	lpStr = (LPSTR)TextPtr->lpStr;
	if ((next_ch = *lpStr++) == '\0')
	    num_chars = 0;
	next_ch -= FIRST_CHAR;
	while (num_chars--)
	{
	    BYTE	ch;

	    ch = next_ch;
	    if ((next_ch = *lpStr++) == '\0')
		num_chars = 0;
	    next_ch -= FIRST_CHAR;
	    locPrim.Object.Stamp.frame = &FontPtr->CharDesc[ch];
	    if (ch < MAX_CHARS && GetFrameWidth (locPrim.Object.Stamp.frame))
	    {
		RECT	r;

		r.corner.x = locPrim.Object.Stamp.origin.x
			- GetFrameHotX (locPrim.Object.Stamp.frame);
		r.corner.y = locPrim.Object.Stamp.origin.y
			- GetFrameHotY (locPrim.Object.Stamp.frame);
		r.extent.width = GetFrameWidth (locPrim.Object.Stamp.frame);
		r.extent.height = GetFrameHeight (locPrim.Object.Stamp.frame);
		_save_stamp.origin = r.corner;
		if (BoxIntersect (&r, pClipRect, &r))
		    DrawGraphicsFunc (&r, &locPrim);

		locPrim.Object.Stamp.origin.x += GetFrameWidth (locPrim.Object.Stamp.frame)
			+ FontPtr->CharSpace;
		if (num_chars && next_ch < MAX_CHARS
			&& !(FontPtr->KernTab[ch]
			& (FontPtr->KernTab[next_ch] >> 2)))
		    locPrim.Object.Stamp.origin.x -= FontPtr->KernAmount;
	    }
	}
    }
}

