/* #define DEBUG */

#ifdef DEBUG
#include <stdio.h>
#define printf	kprintf
#endif /* DEBUG */
#include "memintrn.h"
#include "n_ary.h"

#define get_allocp()		(_pCurHeader->allocp)
#define set_allocp(mem_loc)	(_pCurHeader->allocp = (mem_loc))

PROC(STATIC
MEM_SIZE near peeklong, (mem_loc),
    ARG_END	(MEM_LOC	mem_loc)
)
{
    MEM_SIZE	value;

    (*_pCurHeader->restore_func) (mem_loc, (MEM_SIZE)0,
	    (LPSTR)&value, (MEM_SIZE)sizeof (MEM_SIZE));

    return (value);
}

PROC(STATIC
MEM_BOOL near pokelong, (mem_loc, value),
    ARG		(MEM_LOC	mem_loc)
    ARG_END	(MEM_SIZE	value)
)
{
    return ((*_pCurHeader->store_func) ((LPSTR)&value,
	    mem_loc, (MEM_SIZE)0, (MEM_SIZE)sizeof (MEM_SIZE)));
}

PROC(STATIC
MEM_SIZE near compact_blocks, (),
    ARG_VOID
)
{
    MEM_SIZE	totsize = 0;
    MEM_LOC	p, q;

    set_allocp (q = 0L);
    for (p = peeklong (q + PTR); p != 0L; p = q)
    {
	MEM_SIZE	size;

	size = peeklong (p + SIZE);
	q = peeklong (p + PTR);
	p += size;
	totsize += size;
	if (q != 0L || p < _pCurHeader->max_core)
	{
	    if (q == 0L)
{
#ifdef	DEBUG
printf ("compact_blocks: we were at the end of core\n");
#endif
		q = _pCurHeader->max_core;
}
	    if (!(*_pCurHeader->move_func) (p, (MEM_SIZE)0,
		    p - totsize, (MEM_SIZE)0, q - p))
		/* ERROR */;
	    _mem_hash_update (_pCurHeader, p, q, totsize);

	    if (q == _pCurHeader->max_core)
	    {
		p = _pCurHeader->max_core - totsize;

		pokelong ((MEM_LOC)PTR, 0L);

		pokelong (p + SIZE, totsize);
		_mem_n_ary_free (p + PTR /* HEADERSIZE */, totsize);
		q = 0L;
	    }
	}
    }

    return (totsize);
}

PROC(STATIC
MEM_LOC near morecore, (size),
    ARG_END	(MEM_SIZE	size)
)
{
    MEM_SIZE	block_size;

    if (_pCurHeader->max_core == 0)
	size += HEADERSIZE;	/* room for initial header */

    if ((block_size = _pCurHeader->block_size) == 0)
   	block_size = size;
    else
	block_size = ((size + (block_size - 1L)) / block_size) * block_size;

    if (_pCurHeader->limit > 0
	    && _pCurHeader->max_core + block_size > _pCurHeader->limit)
	return (BAD_MEM_LOC);

    {
	MEM_LOC		mem_loc;

	/* Get more memory from system */

	size = block_size;
	if ((mem_loc = _pCurHeader->max_core) == 0)
	{
	    /* Loop 'next' pointer around */
	    if (!pokelong ((MEM_LOC)PTR, 0L) || !pokelong ((MEM_LOC)SIZE, 0L))
		return (BAD_MEM_LOC);

	    set_allocp (0L);

	    mem_loc += HEADERSIZE;
	    size -= HEADERSIZE;
	}

	if (size > 0)
	{
	    if (!(_pCurHeader->flags & MEM_ADDRESSABLE)
		    && !(*_pCurHeader->clear_func) (mem_loc, size))
	    {
		if (_pCurHeader->less_core_func != MEM_NULL)
		    (*_pCurHeader->less_core_func) ();
		return (BAD_MEM_LOC);
	    }
	    		
	    /* Add new block to free list. */
	    pokelong (mem_loc + SIZE, size);
	    if (!_mem_n_ary_free (mem_loc + PTR /* HEADERSIZE */, size))
		/* ERROR */;
	}

	_pCurHeader->max_core = mem_loc + size;
    }

    return (get_allocp ());
}

PROC(
MEM_SIZE near _mem_n_ary_biggest_block, (mem_desired),
    ARG_END	(MEM_SIZE	mem_desired)
)
{
    MEM_SIZE	bytes_available, size;
    MEM_LOC	p, q;

#ifdef	DEBUG
printf ("_mem_n_ary_biggest_block: entering . . . (%lu)", mem_desired);
#endif
/* Search list for biggest block. */
    set_allocp (0L);
    bytes_available = size = 0;
    for (q = peeklong (PTR), p = q; p != 0L; q = p, p = peeklong (p + PTR))
    {
	size = peeklong (p + SIZE);
	if (size > bytes_available)
	    bytes_available = size;
    }

    if (mem_desired == (MEM_SIZE)~0L)
	mem_desired = _pCurHeader->limit - _pCurHeader->max_core;
    else if (mem_desired > 0)
	mem_desired = (mem_desired + HEADERSIZE + 1) & ~1L;/* make size even */
    if (bytes_available < mem_desired)
    {
	if (q + size < _pCurHeader->max_core)
	    size = 0;

	if (morecore (mem_desired - size) != BAD_MEM_LOC)
	    bytes_available = mem_desired;
    }
#ifdef	DEBUG
printf (" exiting -- %lu\n", bytes_available);
#endif
    if (bytes_available <= HEADERSIZE)
	_pCurHeader->biggest_block = 0;
    else
	_pCurHeader->biggest_block = bytes_available - HEADERSIZE;

    return (bytes_available);
}

#ifdef DEBUG
PROC(STATIC
void near free_blocks, (),
    ARG_VOID
)
{
    MEM_LOC	p;
/* Search list for a big enough block. */

    for (p = peeklong (PTR); p != 0L; p = peeklong (p + PTR))
	printf ("\t\t%lu bytes free at address %lu\n",
		peeklong (p + SIZE), p);
}
#endif /* DEBUG */

PROC(
MEM_SIZE near _mem_n_ary_compact, (mem_desired),
    ARG_END	(MEM_SIZE	mem_desired)
)
{
    MEM_SIZE	size;

    size = (mem_desired + HEADERSIZE + 1) & ~1L;	/* make size even */
    if (size - HEADERSIZE > _pCurHeader->biggest_block)
    {
	compact_blocks ();
	_mem_n_ary_biggest_block (mem_desired);

	if (size - HEADERSIZE > _pCurHeader->biggest_block)
	{
	    if (_mem_hash_discard (_pCurHeader))
		compact_blocks ();
	    _mem_n_ary_biggest_block (mem_desired);
	}
    }

    return (_pCurHeader->biggest_block);
}

/*
	Add another block to the list of free memory.
	Return logical address of the block.
*/

PROC(
MEM_LOC near _mem_n_ary_alloc, (size, flags),
    ARG		(MEM_SIZE	size)
    ARG_END	(MEM_FLAGS	flags)
)
{
    MEM_SIZE	nunits, bs;
    MEM_LOC	p, q;

    if (get_allocp () == BAD_MEM_LOC)
	return ((MEM_LOC)MEM_NULL);

#ifdef DEBUG
printf ("_mem_n_ary_alloc: %lu bytes ", size);
#endif /* DEBUG */
    nunits = (size + PTR /* HEADERSIZE */+ 1) & ~1L;	/* make size even */
    if (_pCurHeader->block_size && (bs = nunits % _pCurHeader->block_size))
	nunits += _pCurHeader->block_size - bs;
    if (nunits < HEADERSIZE)
	nunits = HEADERSIZE;

    q = 0L;
/* Search list for a big enough block. */

    for (p = peeklong (q + PTR); ; q = p, p = peeklong (p + PTR))
    {
	MEM_SIZE	mem_size;

	mem_size = peeklong (p + SIZE);
	if (mem_size >= nunits)
	{
	    MEM_SIZE	newsize;

	    if ((newsize = mem_size - nunits) < HEADERSIZE)
	    {
	    /* Readjust pointers to cut out the block we want. */
		pokelong (q + PTR, peeklong (p + PTR));
	    }
	    else
	    {
	    /* Cut out the piece we want. */
#ifdef OLDWAY
		pokelong (p + SIZE, newsize);
		p += newsize;
		pokelong (p + SIZE, nunits);
#else /* OLDWAY */
		pokelong (p + SIZE, nunits);
		pokelong (p + nunits + PTR, peeklong (p + PTR));
		pokelong (p + nunits + SIZE, newsize);
		pokelong (q + PTR, p + nunits);
#endif /* OLDWAY */
	    }
	    set_allocp (q);

	    if (mem_size - HEADERSIZE == _pCurHeader->biggest_block)
		_mem_n_ary_biggest_block ((MEM_SIZE)0);
	    nunits -= PTR /* HEADERSIZE */;
	    p += PTR /* HEADERSIZE */;
	    if ((flags & MEM_ZEROINIT)
		    && !(*_pCurHeader->clear_func) (p, nunits))
	    {
		_mem_n_ary_free (p, nunits);

		return ((MEM_LOC)MEM_NULL);
	    }

#ifdef DEBUG
printf ("at %lu\n", p);
free_blocks ();
#endif /* DEBUG */
	    return (p);
	}

	if (p == 0L || p + mem_size == _pCurHeader->max_core)
	{
	    if (morecore (nunits - mem_size) == BAD_MEM_LOC)
{
#ifdef DEBUG
printf ("UNAVAILABLE\n");
#endif /* DEBUG */
		return ((MEM_LOC)MEM_NULL);
}
	    p = q;
	}
    }
}

/* 
	Return block ap to the free list.
*/
PROC(
MEM_BOOL near _mem_n_ary_free, (ap, size),
    ARG		(MEM_LOC	ap)
    ARG_END	(MEM_SIZE	size)
)
{
    MEM_LOC	p, q, nextq;
    MEM_LOC	allocp;
    MEM_SIZE	psize, qsize;

    if ((allocp = get_allocp ()) == BAD_MEM_LOC)
	return (MEM_FAILURE);

    p = ap - PTR /* HEADERSIZE */;	/* point to header.  At this point
					 * peeklong (p + SIZE) should equal
					 * psize.
					 */
    psize = peeklong (p + SIZE);

#ifdef DEBUG
printf ("_mem_n_ary_free: add %ld bytes at %ld to list.\n", psize, p);
#endif

    for (q = allocp; (nextq = peeklong (q + PTR)) < p || p < q; q = nextq)
    {
	if (q >= nextq && (p > q || p < nextq))
	    break;	/* at one end or d'other */
    }

    qsize = peeklong (q + SIZE);
/*
#ifdef DEBUG
printf ("\tp=<%lu,(--,%lu)>, q=<%lu,(%lu,%lu)>\n", p, psize, q, nextq, qsize);
#endif
*/

    if (p + psize == nextq)
    {
    /* join to upper number */
	psize += peeklong (nextq + SIZE);
	pokelong (p + SIZE, psize);
	pokelong (p + PTR, peeklong (nextq + PTR));
    }
    else
    {
	pokelong (p + PTR, nextq);
    }

    if (q + qsize == p)
    {
    /* join to lower number */
	qsize += psize;
	pokelong (q + SIZE, qsize);
	pokelong (q + PTR, peeklong (p + PTR));
    }
    else
    {
	pokelong (q + PTR, p);
    }

    if (_pCurHeader->less_core_func != MEM_NULL
	    && p + psize == _pCurHeader->max_core)
    {
	if (q + qsize < _pCurHeader->max_core)
	    _pCurHeader->max_core = p;
	else
	{
	    _pCurHeader->max_core = q;
	    p = q;
	    for (nextq = peeklong (p + PTR);
		    nextq != p; nextq = peeklong (nextq + PTR))
		q = nextq;
	}

	pokelong (q + PTR, 0L);
	(*_pCurHeader->less_core_func) ();
	if (psize - HEADERSIZE > _pCurHeader->biggest_block
		|| qsize - HEADERSIZE > _pCurHeader->biggest_block)
	    _mem_n_ary_biggest_block ((MEM_SIZE)0);
    }
    else if ((psize -= HEADERSIZE) > _pCurHeader->biggest_block)
	_pCurHeader->biggest_block = psize;
    else if (qsize > 0 && (qsize -= HEADERSIZE) > _pCurHeader->biggest_block)
	_pCurHeader->biggest_block = qsize;

    set_allocp (q);

#ifdef DEBUG
free_blocks ();
#endif
    return (MEM_SUCCESS);
}

PROC(
void near _mem_n_ary_init, (),
    ARG_VOID
)
{
    morecore ((MEM_SIZE)0);
#ifdef NOTYET
    if (_pCurHeader->limit > 0)
	morecore (_pCurHeader->limit - HEADERSIZE);
#endif /* NOTYET */
}

PROC(
MEM_BOOL near _mem_n_ary_swap,
	(desired_size, priority, src_type, dst_type),
    ARG		(PMEM_SIZE	desired_size)
    ARG		(MEM_PRIORITY	priority)
    ARG		(MEM_TYPE	src_type)
    ARG_END	(MEM_TYPE	dst_type)
)
{
    return (_mem_hash_swap (desired_size, priority, src_type, dst_type));
}
