#ifndef _RESINTRN_H
#define _RESINTRN_H

#include <string.h>
#include "reslib.h"
#include "index.h"

typedef struct
{
    DWORD	flags_and_data_loc;
    COUNT	num_valid_handles;
} RES_HANDLE_LIST;
typedef RES_HANDLE_LIST	*PRES_HANDLE_LIST;
typedef RES_HANDLE_LIST	near *NPRES_HANDLE_LIST;
typedef RES_HANDLE_LIST	far *LPRES_HANDLE_LIST;

typedef PRES_HANDLE_LIST	RES_HANDLE_LISTPTR;
typedef PDWORD			ENCODEPTR;
typedef PBYTE			DATAPTR;

#define RES_HANDLE_LIST_PRIORITY	DEFAULT_MEM_PRIORITY

#define AllocResourceHandleList(nt,ni)		\
    mem_allocate ((MEM_SIZE)(sizeof (RES_HANDLE_LIST) \
	    + (PACKMEM_LIST_SIZE * (nt)) \
	    + (INSTANCE_LIST_SIZE * (ni))), MEM_ZEROINIT | MEM_PRIMARY, \
	    RES_HANDLE_LIST_PRIORITY, MEM_SIMPLE)
#define LockResourceHandleList(pRH,h,p,rp,tp,dp)	\
do \
{ \
    *(rp) = (RES_HANDLE_LISTPTR)mem_lock ((MEM_HANDLE)h); \
    *(tp) = (ENCODEPTR)&(*(rp))[1]; \
    *(dp) = (DATAPTR)&(*(tp))[CountPackageTypes (pRH,p)]; \
} while (0)
#define UnlockResourceHandleList(h)	mem_unlock((MEM_HANDLE)h)
#define FreeResourceHandleList(h)	mem_release((MEM_HANDLE)h)

#define DoLoad(pRH, t,fp,len)	(*(pRH)->TypeList[t-1].func_vectors.load_func)(fp,len)
#define DoFree(pRH, t,h)	(*(pRH)->TypeList[t-1].func_vectors.free_func)(h)

PROC_GLOBAL(
MEM_HANDLE _GetResFileData, (res_fp, flen),
    ARG		(FILE	*res_fp)
    ARG_END	(DWORD	flen)
);
#define IsIndexType(pRH, t)	((pRH)->TypeList[t-1].func_vectors.load_func \
					==_GetResFileData)

#define LastResPackage(pRH)	((pRH)->num_packages)
#define LastResType(pRH)	((pRH)->num_types)
#define GetInstanceCount(pRH,t)	\
	((pRH)->TypeList[(t)-1].instance_count)

#define ValidResPackage(pRH,p)	((p)<=LastResPackage(pRH))
#define ValidResType(pRH,t)	((t)!=0&&(t)<=LastResType(pRH))

#define CountPackageTypes(pRH,p)		\
	(COUNT)GET_TYPE ((pRH)->PackageList[(p)-1].packmem_info)
#define CountPackageInstances(pRH,p)	\
	(COUNT)GET_INSTANCE ((pRH)->PackageList[(p)-1].packmem_info)

PROC_GLOBAL(
void near _set_current_index_header, (ResHeaderPtr),
    ARG_END	(INDEX_HEADERPTR	ResHeaderPtr)
);
PROC_GLOBAL(
INDEX_HEADERPTR	near _get_current_index_header, (),
    ARG_VOID
);

PROC_GLOBAL(
void near _init_resfile_error, (FileErrorFunc),
    ARG_END
    (
	PROC_PARAMETER(
	BOOLEAN (*FileErrorFunc), (filename),
	    ARG_END	(PVOID	filename)
	)
    )
);
PROC_GLOBAL(
void near _uninit_resfile_error, (),
    ARG_VOID
);

typedef PROC_PARAMETER(
	BOOLEAN (*FILE_ERROR_FUNC), (filename),
	    ARG_END	(PVOID	filename)
	);

extern char	*_cur_resfile_name;

#endif /* _RESINTRN_H */

