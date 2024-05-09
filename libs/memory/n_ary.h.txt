#ifndef _N_ARY_H
#define _N_ARY_H

#define BAD_MEM_LOC	0xFFFFFFFF

#ifdef OLD
#define PTR		0L
#define SIZE		(PTR + sizeof (MEM_LOC))
#define HEADERSIZE	(SIZE + sizeof (MEM_SIZE))
#else /* !OLD */
#define SIZE		0L
#define PTR		(SIZE + sizeof (MEM_SIZE))
#define HEADERSIZE	(PTR + sizeof (MEM_LOC))
#endif /* OLD */

PROC_GLOBAL(
void near _mem_n_ary_init, (),
    ARG_VOID
);
PROC_GLOBAL(
MEM_LOC near _mem_n_ary_alloc, (size, flags),
    ARG		(MEM_SIZE	size)
    ARG_END	(MEM_FLAGS	flags)
);
PROC_GLOBAL(
MEM_BOOL near _mem_n_ary_free, (ap, size),
    ARG		(MEM_LOC	ap)
    ARG_END	(MEM_SIZE	size)
);
PROC_GLOBAL(
MEM_SIZE near _mem_n_ary_compact, (mem_desired),
    ARG_END	(MEM_SIZE	mem_desired)
);
PROC_GLOBAL(
MEM_SIZE near _mem_n_ary_biggest_block, (mem_desired),
    ARG_END	(MEM_SIZE	mem_desired)
);
PROC_GLOBAL(
MEM_BOOL near _mem_n_ary_swap,
	(desired_size, priority, src_type, dst_type),
    ARG		(PMEM_SIZE	desired_size)
    ARG		(MEM_PRIORITY	priority)
    ARG		(MEM_TYPE	src_type)
    ARG_END	(MEM_TYPE	dst_type)
);

#endif /* _N_ARY_H */

