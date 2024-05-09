#include "starcon.h"

STAR_DESCPTR	star_array;
PSTR		*ClusterNames;
STAR_DESCPTR	CurStarDescPtr = 0;

PROC(
STAR_DESCPTR FindStar, (LastSDPtr, puniverse, xbounds, ybounds),
    ARG		(STAR_DESCPTR	LastSDPtr)
    ARG		(PPOINT		puniverse)
    ARG		(SIZE		xbounds)
    ARG_END	(SIZE		ybounds)
)
{
    COORD		min_y, max_y;
    SIZE		lo, hi;
    STAR_DESCPTR	BaseSDPtr;

    if (GET_GAME_STATE (ARILOU_SPACE_SIDE) <= 1)
    {
	BaseSDPtr = star_array;
	hi = NUM_SOLAR_SYSTEMS - 1;
    }
    else
    {
#define NUM_HYPER_VORTICES	15
	BaseSDPtr = &star_array[NUM_SOLAR_SYSTEMS + 1];
	hi = (NUM_HYPER_VORTICES + 1) - 1;
    }

    if (LastSDPtr == NULL_PTR)
	lo = 0;
    else if ((lo = LastSDPtr - BaseSDPtr + 1) > hi)
	return (0);
    else
    	hi = lo;

    if (ybounds <= 0)
	min_y = max_y = puniverse->y;
    else
    {
	min_y = puniverse->y - ybounds;
	max_y = puniverse->y + ybounds;
    }

    while (lo < hi)
    {
	SIZE	mid;

	mid = (lo + hi) >> 1;
	if (BaseSDPtr[mid].star_pt.y >= min_y)
	    hi = mid - 1;
	else
	    lo = mid + 1;
    }

    LastSDPtr = &BaseSDPtr[lo];
    if (ybounds < 0 || LastSDPtr->star_pt.y <= max_y)
    {
	COORD	min_x, max_x;

	if (xbounds <= 0)
	    min_x = max_x = puniverse->x;
	else
	{
	    min_x = puniverse->x - xbounds;
	    max_x = puniverse->x + xbounds;
	}

	do
	{
	    if ((ybounds < 0 || LastSDPtr->star_pt.y >= min_y)
		    && (xbounds < 0
		    || (LastSDPtr->star_pt.x >= min_x
		    && LastSDPtr->star_pt.x <= max_x))
		    )
		return (LastSDPtr);
	} while ((++LastSDPtr)->star_pt.y <= max_y);
    }

    return (0);
}

PROC(
void GetClusterName, (pSD, buf),
    ARG		(STAR_DESCPTR	pSD)
    ARG_END	(char		buf[])
)
{
    PSTR	pBuf;
    LPSTR	lpStr;
    char	GreekLetters[][10] =
    {
	"",
	"Alpha ",
	"Beta ",
	"Gamma ",
	"Delta ",
	"Epsilon ",
	"Zeta ",
	"Eta ",
	"Theta ",
	"Iota ",
	"Kappa ",
	"Lambda ",
	"Mu ",
	"Nu ",
	"Xi ",
    };

    pBuf = (PSTR)buf;
    lpStr = (LPSTR)GreekLetters[pSD->Prefix];
    while (*pBuf++ = *lpStr++)
	;
    --pBuf;
    SET_SEGMENT (&lpStr, GET_SEGMENT (&ClusterNames));
    SET_OFFSET (&lpStr, ClusterNames[pSD->Postfix]);
    while (*pBuf++ = *lpStr++)
	;
}

