#include "starcon.h"

FRAME		planet[NUM_VIEWS];

PROC(
void load_gravity_well, (selector),
    ARG_END	(BYTE	selector)
)
{
    COUNT		i;
    RES_TYPE		rt;
    RES_INSTANCE	ri;
    RES_PACKAGE		rp;
    MEM_HANDLE		hLastIndex;

    hLastIndex = SetResourceIndex (hResIndex);

    if (selector == NUMBER_OF_PLANET_TYPES)
    {
	planet[0] = CaptureDrawable (
		LoadGraphic (SAMATRA_BIG_MASK_PMAP_ANIM)
		);
	planet[1] = planet[2] = 0;
    }
    else
    {
	if (selector & PLANET_SHIELDED)
	{
	    rt = GET_TYPE (SHIELDED_BIG_MASK_PMAP_ANIM);
	    ri = GET_INSTANCE (SHIELDED_BIG_MASK_PMAP_ANIM);
	    rp = GET_PACKAGE (SHIELDED_BIG_MASK_PMAP_ANIM);
	}
	else
	{
	    rt = GET_TYPE (PLANET00_BIG_MASK_PMAP_ANIM);
	    ri = GET_INSTANCE (PLANET00_BIG_MASK_PMAP_ANIM)
		    + (selector * NUM_VIEWS);
	    rp = GET_PACKAGE (PLANET00_BIG_MASK_PMAP_ANIM)
		    + selector;
	}

	for (i = 0; i < NUM_VIEWS; ++i, ++ri)
	{
	    planet[i] = CaptureDrawable (
		    LoadGraphic (MAKE_RESOURCE (rp, rt, ri))
		    );
	}
    }

    SetResourceIndex (hLastIndex);
}

PROC(
void free_gravity_well, (),
    ARG_VOID
)
{
    COUNT	i;

    for (i = 0; i < NUM_VIEWS; ++i)
    {
	DestroyDrawable (ReleaseDrawable (planet[i]));
	planet[i] = 0;
    }
}

