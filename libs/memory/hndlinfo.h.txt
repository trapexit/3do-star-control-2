#ifndef _HNDLINFO_H
#define _HNDLINFO_H

#ifdef HANDLE_DIAG
PROC_GLOBAL(
void near _mem_dump_handle_diag, (),
    ARG_VOID
);
#endif /* HANDLE_DIAG */

#define MEMINFO_TAB		SAME_MEM_USAGE
#define SIMPLE_HANDLE_TAB	MEM_SIMPLE
#define COMPLEX_HANDLE_TAB	MEM_COMPLEX

extern LPVOID	_lpCMI, _lpCH;

PROC_GLOBAL(
MEM_HANDLE near _mem_internal_alloc, (usage),
    ARG_END	(MEM_USAGE	usage)
);
PROC_GLOBAL(
MEM_BOOL near _mem_internal_free, (h, usage),
    ARG		(MEM_HANDLE	h)
    ARG_END	(MEM_USAGE	usage)
);

PROC_GLOBAL(
MEM_BOOL near _mem_set_handle, (h),
    ARG_END	(MEM_HANDLE	h)
);
PROC_GLOBAL(
LPVOID near _mem_set_entry, (h),
    ARG_END	(MEM_HANDLE	h)
);

#define _mem_lock_meminfo(mi)		(LPMEM_INFO)_mem_set_entry (mi)
#define _mem_unlock_meminfo(mi)

#define lpSimpleHandle	((LPSIMPLE_HANDLE)_lpCH)
#define lpComplexHandle	((LPCOMPLEX_HANDLE)_lpCH)
#define lpCurHandle	lpSimpleHandle
#define lpCurMemInfo	((LPMEM_INFO)_lpCMI)

#define _mem_get_attrs(pa)	*(pa) = lpCurMemInfo->attrs
#define _mem_get_size()		lpCurMemInfo->attrs.size
#define _mem_set_flags(f)	lpCurMemInfo->attrs.flags|=(f)
#define _mem_clr_flags(f)	lpCurMemInfo->attrs.flags&=~(f)
#define _mem_get_flags()	lpCurMemInfo->attrs.flags
#define _mem_get_priority()	lpCurMemInfo->attrs.priority
#define _mem_set_usage(u)	lpCurHandle->usage=(u)
#define _mem_get_usage()	lpCurHandle->usage
#define _mem_get_virtual_loc()	lpCurMemInfo->attrs.virtual
#define _mem_get_where()	lpCurMemInfo->attrs.where
#define _mem_set_phys_size(s)	lpComplexHandle->window.cursize=(s)
#define _mem_get_phys_size()	lpComplexHandle->window.cursize
#define _mem_set_phys_pos(p,d)	lpComplexHandle->window.curpos=(p), \
				lpComplexHandle->window.curdir=(d)
#define _mem_set_ref()		++lpCurMemInfo->ref_count
#define _mem_clr_ref()		(--lpCurMemInfo->ref_count == 0 ? \
					MEM_SUCCESS : MEM_FAILURE)
#define _mem_set_info(i)	\
    _lpCMI = _mem_set_entry (lpCurHandle->mem_info = (i))
#define _mem_get_info()		lpCurHandle->mem_info

PROC_GLOBAL(
MEM_ACCESS_MODE near _mem_get_access, (),
    ARG_VOID
);
PROC_GLOBAL(
LPVOID near _mem_set_access, (access_mode),
    ARG_END	(MEM_ACCESS_MODE	access_mode)
);
PROC_GLOBAL(
MEM_BOOL near _mem_clr_access, (),
    ARG_VOID
);
PROC_GLOBAL(
MEM_BOOL near _mem_set_priority, (priority),
    ARG_END	(MEM_PRIORITY	priority)
);
PROC_GLOBAL(
MEM_BOOL near _mem_set_primary, (mem_loc, mem_offs, mem_type),
    ARG		(MEM_LOC	mem_loc)
    ARG		(MEM_SIZE	mem_offs)
    ARG_END	(MEM_TYPE	mem_type)
);
PROC_GLOBAL(
MEM_LOC near _mem_get_primary, (mem_type),
    ARG_END	(PMEM_TYPE	mem_type)
);
PROC_GLOBAL(
MEM_SIZE near _mem_get_phys_pos, (get_seek_pos),
    ARG_END	(MEM_BOOL	get_seek_pos)
);

PROC_GLOBAL(
void near _mem_init_info, (mem_info, pmem_attrs),
    ARG		(MEM_HANDLE	mem_info)
    ARG_END	(PMEM_ATTRS	pmem_attrs)
);

#endif /* _HNDLINFO_H */

