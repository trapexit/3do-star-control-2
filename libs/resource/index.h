#ifndef _INDEX_H
#define _INDEX_H

#include <stdio.h>

typedef struct
{
    PROC_PARAMETER(
    MEM_HANDLE (*load_func), (fp, len),
	ARG	(FILE		*fp)
	ARG_END	(DWORD		len)
    );
    PROC_PARAMETER(
    BOOLEAN (*free_func), (handle),
	ARG_END	(MEM_HANDLE	handle)
    );
} RES_VECTORS;

typedef struct
{
    RES_INSTANCE	instance_count;
    RES_VECTORS		func_vectors;
} TYPE_DESC;
typedef TYPE_DESC	*PTYPE_DESC;
typedef TYPE_DESC	near *NPTYPE_DESC;
typedef TYPE_DESC	far *LPTYPE_DESC;

typedef struct
{
    RESOURCE	packmem_info;
    DWORD	flags_and_data_loc;	/* data_loc doubles as 3 byte offset
    					 * into package file or 2 byte
					 * MEM_HANDLE to package member
					 * array with the offset stored in
					 * the MEM_HANDLE (cuts down on use
					 * of near memory).
					 */
} PACKAGE_DESC;
typedef PACKAGE_DESC	*PPACKAGE_DESC;
typedef PACKAGE_DESC	near *NPPACKAGE_DESC;
typedef PACKAGE_DESC	far *LPPACKAGE_DESC;

typedef BYTE		RES_FLAGS;
#define IS_PACKAGED	((RES_FLAGS)(1 << 0))

typedef struct
{
    FILE		*res_fp;

    DWORD		packmem_list_offs;
    DWORD		path_list_offs;
    DWORD		file_list_offs;

    RES_PACKAGE		num_packages;
    RES_TYPE		num_types;
    RES_FLAGS		res_flags;

    union
    {
	DWORD		header_len;
	struct
	{
	    PPACKAGE_DESC	package_list;
	    PTYPE_DESC		type_list;
	} lists;
    } index_info;

#ifndef PACKAGING
    char		index_file_name[36];
    DWORD		data_offs;
    MEM_HANDLE		hPredHeader, hSuccHeader;
#endif /* PACKAGING */
} INDEX_HEADER;
typedef INDEX_HEADER	*PINDEX_HEADER;
typedef INDEX_HEADER	near *NPINDEX_HEADER;
typedef INDEX_HEADER	far *LPINDEX_HEADER;

typedef PINDEX_HEADER	INDEX_HEADERPTR;

#define PackageList	index_info.lists.package_list
#define TypeList	index_info.lists.type_list

#define INDEX_HEADER_PRIORITY		DEFAULT_MEM_PRIORITY
#define AllocResourceHeader(s)	\
	mem_allocate ((s), MEM_ZEROINIT | MEM_PRIMARY, \
	INDEX_HEADER_PRIORITY, MEM_SIMPLE)
#define LockResourceHeader		(INDEX_HEADERPTR)mem_lock
#define UnlockResourceHeader		mem_unlock
#define FreeResourceHeader		mem_release

typedef struct
{
    BYTE		path_offset[2];
    char		filename[8];
    char		extension[3];
} FILE_INFO;

#define FILE_NAME_SIZE		8
#define EXTENSION_SIZE		3
#define NO_PATH			0xFFFF

#define FILE_LIST_SIZE		\
	((sizeof (BYTE) * 2) + FILE_NAME_SIZE + EXTENSION_SIZE)
#define PACKMEM_LIST_SIZE	\
	(sizeof (DWORD))
#define INSTANCE_LIST_SIZE	\
	(sizeof (BYTE) * 2)

#endif /* _INDEX_H */

