#include "types.h"
#include "mem.h"
#include "../../graphics/3do/defs3do.h"

#if 0
void *
AllocDRAM (int32 size)
{
    return (AllocMem (size, MEMTYPE_DRAM));
}

void
FreeDRAM (void *p, int32 size)
{
    FreeMem (p, size);
}

void *
AllocVRAM (int32 size)
{
    return (AllocMem (size, MEMTYPE_VRAM));
}

void
FreeVRAM (void *p, int32 size)
{
    FreeMem (p, size);
}
#endif

//#define MEM3DO_DIAG

#ifdef MEM3DO_DIAG
static int	curcore, maxcore;
#endif

void *
ThreedoAlloc (int32 size)
{
#ifdef MEM3DO_DIAG
void	*p;

p = AllocMem (size, MEMTYPE_ANY | MEMTYPE_FILL | 0x00);
if (p)
{
    curcore += size;
    if (curcore > maxcore)
    {
#define MEM_THRESHOLD	(1500 * 1024)
	if (curcore > MEM_THRESHOLD
		|| curcore / 100000 != maxcore / 100000)
	{
	    printf ("total %u (maxcore %u, screens %u)\n",
		    curcore + SCREEN_WIDTH * SCREEN_HEIGHT * 2 * 4,
		    curcore, SCREEN_WIDTH * SCREEN_HEIGHT * 2 * 4);
	    if (curcore > 1600 * 1000)
		ReportMemoryUsage (), printf ("\n");
	}
	maxcore = curcore;
    }
}
else
{
    printf ("Can't alloc %u -- curcore %u (screens %u)",
 	    size, curcore, SCREEN_WIDTH * SCREEN_HEIGHT * 2 * 4);
    ReportMemoryUsage (); printf ("\n");
}
return (p);
#endif
    return (AllocMem (size, MEMTYPE_ANY | MEMTYPE_FILL | 0x00));
}

void
ThreedoFree (void *p, int32 size)
{
#ifdef MEM3DO_DIAG
curcore -= size;
#endif
    FreeMem (p, size);
    ScavengeMem ();
}
