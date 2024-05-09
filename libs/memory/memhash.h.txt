#ifndef _MEMHASH_H
#define _MEMHASH_H

#ifdef HASHING_ENABLED

PROC_GLOBAL(
void near _mem_hash_info, (mem_info, priority, where),
    ARG		(MEM_HANDLE	mem_info)
    ARG		(MEM_PRIORITY	priority)
    ARG_END	(MEM_TYPE	where)
);
PROC_GLOBAL(
void near _mem_unhash_info, (mem_info, priority, where),
    ARG		(MEM_HANDLE	mem_info)
    ARG		(MEM_PRIORITY	priority)
    ARG_END	(MEM_TYPE	where)
);
PROC_GLOBAL(
void near _mem_rehash_info, (mem_info, priority, where),
    ARG		(MEM_HANDLE	mem_info)
    ARG		(MEM_PRIORITY	priority)
    ARG_END	(MEM_TYPE	where)
);
PROC_GLOBAL(
void near _mem_hash_update, (pHeader, p, q, size),
    ARG		(PMEM_HEADER	pHeader)
    ARG		(MEM_LOC	p)
    ARG		(MEM_LOC	q)
    ARG_END	(MEM_SIZE	size)
);
PROC_GLOBAL(
MEM_BOOL near _mem_hash_discard, (pHeader),
    ARG_END	(PMEM_HEADER	pHeader)
);
PROC_GLOBAL(
MEM_BOOL near _mem_hash_swap,
	(desired_size, priority, src_type, dst_type),
    ARG		(PMEM_SIZE	desired_size)
    ARG		(MEM_PRIORITY	priority)
    ARG		(MEM_TYPE	src_type)
    ARG_END	(MEM_TYPE	dst_type)
);
PROC_GLOBAL(
void near _mem_hash_newloc, (mem_info, pmem_attrs, old_where),
    ARG		(MEM_HANDLE	mem_info)
    ARG		(PMEM_ATTRS	pmem_attrs)
    ARG_END	(MEM_TYPE	old_where)
);

#else /* !HASHING_ENABLED */

#define _mem_hash_info(mh, mp, mt)
#define _mem_unhash_info(mh, mp, mt)
#define _mem_rehash_info(mh, mp, mt)
#define _mem_hash_newloc(mh, pma, mt)	\
	_mem_init_info (mh, pma)
#define _mem_hash_update(pmh, p, q, ms)
#define _mem_hash_discard(pmh)			MEM_FAILURE
#define _mem_hash_swap(pms, mp, mt1, mt2)	MEM_FAILURE

#endif /* !HASHING_ENABLED */

#endif /* _MEMHASH_H */

