#include "memintrn.h"

PROC(
MEM_TYPE near _mem_imp_init, (mem_tab, core_size, disk_name),
    ARG		(PMEM_HEADER	**mem_tab)
    ARG		(MEM_SIZE	core_size)
    ARG_END	(PSTR		disk_name)
)
{
//    extern MEM_HEADER	_mem_vram_header;
//extern MEM_HEADER	_mem_quaternary_header;
//extern MEM_HEADER	_mem_tertiary_header;
//extern MEM_HEADER	_mem_secondary_header;
    extern MEM_HEADER	_mem_primary_header;
    static PMEM_HEADER	mem_table[] =
    {
//	&_mem_vram_header,
	&_mem_primary_header,
//&_mem_secondary_header,
//&_mem_tertiary_header,
//&_mem_quaternary_header,
    };
    MEM_TYPE		num_mem_types;

    *mem_tab = mem_table;
    num_mem_types = (MEM_TYPE)(sizeof (mem_table) / sizeof (mem_table[0]));
    _mem_primary_header.limit = core_size;

    return (num_mem_types);
}

PROC(
COUNT mem_prog_size, (),
    ARG_VOID
)
{
#ifndef NOTYET
return (0);
#else /* NOTYET */
#include <dos.h>
    PTR_DESC		PDHi;
    union REGS		regs;
    extern MEM_HEADER	_mem_primary_header;

    SET_FARPTR (&PDHi, &_mem_primary_header);
    regs.h.ah = 0x62;	/* Get PSP address */
    intdos (&regs, &regs);

    return ((COUNT)((GET_SEGMENT (&PDHi) - regs.x.bx
	    + (10 - PARAGRAPH_SHIFT - 1))	/* round up */
	    >> (10 - PARAGRAPH_SHIFT)) + 64 /* sizeof dataseg */);
#endif /* NOTYET */
}

PROC(
void _init_shared, (),
    ARG_VOID
)
{
}

PROC(
void _uninit_shared, (),
    ARG_VOID
)
{
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
