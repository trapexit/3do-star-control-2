#ifndef _MEMIMP_H
#define _MEMIMP_H

#define MEMINFO_MEM_TYPE	0/*1*/
#define HANDLE_MEM_TYPE		0/*1*/

#define NUM_INFO_ELEMENTS	200
#define NUM_SIMPLE_ELEMENTS	200
#define NUM_COMPLEX_ELEMENTS	1

/* #define DISK_SUPPORT */

#define _mem_ram_lock(mem_loc, ptr)	\
do \
{ \
    if (_pCurHeader->flags & MEM_MANAGED_ARENA) \
	*(ptr) = (LPVOID)((LPBYTE)_pCurHeader->descriptor.addr + mem_loc); \
    else \
	*(ptr) = (LPVOID)mem_loc; \
} while (0)
#define _mem_ram_unlock(mem_loc)

PROC_GLOBAL(
MEM_TYPE near _mem_imp_init, (mem_tab, core_size, disk_name),
    ARG		(PMEM_HEADER	**mem_tab)
    ARG		(MEM_SIZE	core_size)
    ARG_END	(PSTR		disk_name)
);

#define MAX_BUFSIZE	0x0000FFFF

#endif /* _MEMIMP_H */
