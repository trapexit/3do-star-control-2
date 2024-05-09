#include "memintrn.h"

#define MEMTAB_SIZE	150

static LPDWORD	memtab[MEMTAB_SIZE];
#ifdef MEM_DIAG
static DWORD	maxheap, maxhandles, curheap, curhandles;
static DWORD	allocheap, allochandles, freeheap, freehandles;
#endif /* MEM_DIAG */

PROC(
MEM_HANDLE PASCAL mem_allocate, (size, flags, priority, usage),
    ARG		(MEM_SIZE	size)
    ARG		(MEM_FLAGS	flags)
    ARG		(MEM_PRIORITY	priority)
    ARG_END	(MEM_USAGE	usage)
)
{
    MEM_HANDLE	h;
    DWORD	*p, ns;
    
    SetSemaphore (&_MemorySem);
    
    ns = (size + sizeof (DWORD) + 3) & ~3;
    h = 0;
    if (p = (DWORD *)ThreedoAlloc (ns))
    {
	LPDWORD	*mt;
	int	i;
	
	for (mt = memtab, i = 0; i < MEMTAB_SIZE; i++, mt++)
	{
	    if (!*mt)
	    {
		*p = ns;
		*mt = p + 1;
		h = i + 1;
#ifdef MEM_DIAG
curheap += ns;
curhandles++;
allocheap += ns;
allochandles++;
if (curheap > maxheap)
     maxheap = curheap;
if (curhandles > maxhandles)
    maxhandles = curhandles;
#endif /* MEM_DIAG */
		break;
	    }
	}
	
	if (!h)
	    ThreedoFree (p, ns);
    }
    
    ClearSemaphore (&_MemorySem);
    
    return (h);
}

PROC(
MEM_BOOL mem_release, (handle),
    ARG_END	(MEM_HANDLE	handle)
)
{
    LPDWORD	p;
    
    SetSemaphore (&_MemorySem);
    
    if (handle-- && (p = memtab[handle]))
    {
	p--;
#ifdef MEM_DIAG
curheap -= *p;
curhandles--;
freeheap += *p;
freehandles++;
#endif /* MEM_DIAG */
	ThreedoFree (p, *p);
	memtab[handle] = 0;
ScavengeMem ();
    }
    
    ClearSemaphore (&_MemorySem);
    
    return (MEM_SUCCESS);
}

PROC(
LPVOID mem_simple_access, (handle),
    ARG_END	(MEM_HANDLE	handle)
)
{
     return (handle ? (LPVOID)memtab[handle - 1] : (LPVOID)0);
}

PROC(
MEM_BOOL mem_simple_unaccess, (handle),
    ARG_END	(MEM_HANDLE	handle)
)
{
    return (MEM_SUCCESS);
}

PROC(
MEM_SIZE mem_get_size, (handle),
    ARG_END	(MEM_HANDLE	handle)
)
{
    return (handle ? *(memtab[handle - 1] - 1) : 0);
}

SEMAPHORE	_MemorySem;

PROC(
MEM_BOOL mem_init, (core_size, pmin_addressable, disk_name),
    ARG		(MEM_SIZE	core_size)
    ARG		(PMEM_SIZE	pmin_addressable)
    ARG_END	(PSTR		disk_name)
)
{
    return (MEM_SUCCESS);
}

PROC(
MEM_BOOL mem_uninit, (),
    ARG_VOID
)
{
#ifdef MEM_DIAG
    int	i;
    
    for (i = 0; i < MEMTAB_SIZE; i++)
    {
	if (memtab[i])
	{
	    printf ("mem_uninit: handle %d still active!\n", i);
	    ThreedoFree (memtab[i] - 1, *(memtab[i] - 1));
	    memtab[i] = 0;
	}
    }
    
printf ("Used %u handles -- ", maxhandles);
printf ("%u memory\n", maxheap);
printf ("Allocated %u handles -- ", allochandles);
printf ("%u memory\n", allocheap);
printf ("Freed %u handles -- ", freehandles);
printf ("%u memory\n", freeheap);
    
maxheap = maxhandles = curheap = curhandles = 0;
allocheap = allochandles = freeheap = freehandles = 0;
#endif /* MEM_DIAG */
	    
    return (MEM_SUCCESS);
}

PROC(
void *memmove, (pDst, pSrc, size),
    ARG		(void		*pDst)
	ARG		(const void		*pSrc)
	ARG_END	(unsigned	size)
)
{
    char	*pD, *pS;
	
	pD = (char *)pDst;
	pS = (char *)pSrc;
    if (pD <= pS)
	memcpy (pD, pS, size);
	else
	{
    pD += size - 1;
	pS += size - 1;
	while (size--)
	    *pD-- = *pS--;
    }

    return ((void *)pDst);
}
