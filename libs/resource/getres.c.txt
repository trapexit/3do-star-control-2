#include "resintrn.h"

char	*_cur_resfile_name;

PROC(STATIC
BOOLEAN near ValidResource, (ResHeaderPtr, res),
    ARG		(INDEX_HEADERPTR	ResHeaderPtr)
    ARG_END	(RESOURCE		res)
)
{
    if (ResHeaderPtr != NULL_PTR)
    {
	RES_PACKAGE	res_package;

	res_package = GET_PACKAGE (res);
	if (ValidResPackage (ResHeaderPtr, res_package))
	{
	    RES_TYPE	res_type;

	    res_type = GET_TYPE (res);
	    if (ValidResType (ResHeaderPtr, res_type))
	    {
		COUNT	res_instance;
	
		res_instance = GET_INSTANCE (res);
		if (GetInstanceCount (ResHeaderPtr, res_type) > res_instance)
		    return (TRUE);
	    }
	}
    }

    return (FALSE);
}

PROC(STATIC
void near dword_convert, (dword_array, num_dwords),
    ARG		(LPDWORD	dword_array)
    ARG_END	(COUNT		num_dwords)
)
{
    LPBYTE	p = (LPBYTE)dword_array;

    do
    {
	*dword_array++ = MAKE_DWORD (
		MAKE_WORD (p[0], p[1]),
		MAKE_WORD (p[2], p[3])
		);
	p += 4;
    } while (--num_dwords);
}

#define PATHLEN	256

PROC(
void near get_resource_filename, (ResHeaderPtr, pbuf, file_index),
    ARG		(INDEX_HEADERPTR	ResHeaderPtr)
    ARG		(char			*pbuf)
    ARG_END	(COUNT			file_index)
)
{
    UWORD	path_offs;
    long	file_offs;
    FILE_INFO	file_info;

    file_offs = (long)ResHeaderPtr->file_list_offs +
	    (long)file_index * FILE_LIST_SIZE;
    if (ResHeaderPtr->res_flags & IS_PACKAGED)
	MEMCPY ((PSTR)&file_info,
		(PSTR)ResHeaderPtr + (COUNT)file_offs, FILE_LIST_SIZE);
    else
    {
	SeekResFile (ResHeaderPtr->res_fp, file_offs, 0);
	ReadResFile (&file_info, FILE_LIST_SIZE, 1, ResHeaderPtr->res_fp);
    }

    path_offs = MAKE_WORD (file_info.path_offset[0], file_info.path_offset[1]);
    if (path_offs != NO_PATH)
    {
	file_offs = (long)ResHeaderPtr->path_list_offs + path_offs;
	if (ResHeaderPtr->res_flags & IS_PACKAGED)
	    MEMCPY (pbuf,
		    (PSTR)ResHeaderPtr + (COUNT)file_offs, PATHLEN);
	else
	{
	    SeekResFile (ResHeaderPtr->res_fp, file_offs, 0);
	    ReadResFile (pbuf, PATHLEN, 1, ResHeaderPtr->res_fp);
	}
	pbuf += strlen (pbuf);

	*pbuf++ = '/';
    }

    {
	COUNT	i;
	char	*pstr;

	pstr = file_info.filename;
	for (i = 0; i < FILE_NAME_SIZE; ++i)
	{
	    if ((*pbuf++ = *pstr++) == '\0')
	    {
		--pbuf;
		if (i == 0)
		{
#ifndef PACKAGING
		    strcpy (pbuf, ResHeaderPtr->index_file_name);
#endif /* PACKAGING */
		    return;
		}

		break;
	    }
	}
	*pbuf++ = '.';

	pstr = file_info.extension;
	for (i = 0; i < EXTENSION_SIZE && (*pbuf++ = *pstr++); ++i)
	    ;
	*pbuf++ = '\0';
    }
}

PROC(
DWORD near get_packmem_offs, (ResHeaderPtr, res_package),
    ARG		(INDEX_HEADERPTR	ResHeaderPtr)
    ARG_END	(RES_PACKAGE		res_package)
)
{
    DWORD	file_offs;
    RES_PACKAGE	package;

    file_offs = ResHeaderPtr->packmem_list_offs;
    for (package = 1; package < res_package; ++package)
    {
	file_offs += (DWORD)PACKMEM_LIST_SIZE
		* GET_TYPE (ResHeaderPtr->PackageList[package - 1].packmem_info)
		+ (DWORD)INSTANCE_LIST_SIZE
		* GET_INSTANCE (ResHeaderPtr->PackageList[package - 1].packmem_info);
    }

    return (file_offs);
}

//#define DEBUG

PROC(
MEM_HANDLE near load_package, (ResHeaderPtr, res_package),
    ARG		(INDEX_HEADERPTR	ResHeaderPtr)
    ARG_END	(RES_PACKAGE		res_package)
)
{
    BOOLEAN		DanglingOpen;
    MEM_HANDLE		hList;
    FILE		*fp;
    COUNT		t, num_types, num_instances;
    DWORD		file_offs, data_len;
    RES_HANDLE_LISTPTR	ResourceHandleListPtr;
    ENCODEPTR		TypeEncodePtr;
    DATAPTR		DataPtr;
#ifdef PACKAGING
    extern
#endif /* PACKAGING */
    char		file_buf[PATHLEN];

    num_types = CountPackageTypes (ResHeaderPtr, res_package);
    num_instances = CountPackageInstances (ResHeaderPtr, res_package);

    if ((hList = AllocResourceHandleList (
	    num_types, num_instances
	    )) == NULL_HANDLE)
{
#ifdef DEBUG
printf ("Unable to allocate resource handle list <%d, %d>\n", num_types, num_instances);
#endif /* DEBUG */
	return (NULL_HANDLE);
}

    LockResourceHandleList (ResHeaderPtr, hList, res_package,
	    &ResourceHandleListPtr, &TypeEncodePtr, &DataPtr);
    ResourceHandleListPtr->flags_and_data_loc =
	    ResHeaderPtr->PackageList[res_package - 1].flags_and_data_loc;

    _cur_resfile_name = file_buf;

    DanglingOpen = FALSE;
    file_offs = get_packmem_offs (ResHeaderPtr, res_package);
    if (!(ResHeaderPtr->res_flags & IS_PACKAGED))
    {
	SeekResFile (ResHeaderPtr->res_fp, (long)file_offs, 0);
	if (ReadResFile ((LPSTR)TypeEncodePtr,
		(PACKMEM_LIST_SIZE * num_types)
		+ (INSTANCE_LIST_SIZE * num_instances),
		1, ResHeaderPtr->res_fp) != 1)
	{
	    UnlockResourceHandleList (hList);
	    FreeResourceHandleList (hList);

	    return (NULL_HANDLE);
	}
	dword_convert (TypeEncodePtr, num_types);
    }
    else
    {
	COUNT	file_index;

	MEMCPY (TypeEncodePtr,
		(LPSTR)((PSTR)ResHeaderPtr + (COUNT)file_offs),
		(PACKMEM_LIST_SIZE * num_types)
		+ (INSTANCE_LIST_SIZE * num_instances));
	dword_convert (TypeEncodePtr, num_types);

	file_index = GET_PACKAGE (
		ResHeaderPtr->PackageList[res_package - 1].packmem_info
		);
	get_resource_filename (ResHeaderPtr, file_buf, file_index);
	if (!stricmp (ResHeaderPtr->index_file_name, file_buf))
	    fp = ResHeaderPtr->res_fp;
	else
	{
	    if (fp = OpenResFile (file_buf, "rb",
		    NULL_PTR, 0L, (FILE_ERROR_FUNC)-1L))
		DanglingOpen = TRUE;
	}

	if (fp == NULL)
	    num_types = 0;
	else
	{
	    file_offs = ResourceHandleListPtr->flags_and_data_loc & ~0xFF000000;
	    if (file_offs)
		SeekResFile (fp, (long)file_offs, 0);
	    else if (num_types == 1 && GET_PACKAGE (*TypeEncodePtr) == 1)
	    {	/* if only one thing in this package which is only
		 * package in its file, let file length be determined
		 * by the current length of package rather than the
		 * length at package time.
		 */
		RES_PACKAGE	p;

		for (p = 1; p <= ResHeaderPtr->num_packages; ++p)
		{
		    if (p != res_package
			    && GET_PACKAGE (
			    ResHeaderPtr->PackageList[p - 1].packmem_info)
			    == file_index)
			break;
		}

		if (p > ResHeaderPtr->num_packages)
		{
		    data_len = (LengthResFile (fp) + 3) >> 2;
		    DataPtr[0] = LOBYTE (data_len);
		    DataPtr[1] = HIBYTE (data_len);
		}
	    }
	}
    }

    for (t = 0; t < num_types; ++t)
    {
	RES_TYPE	type;

	type = GET_TYPE (*TypeEncodePtr);
	num_instances = GET_PACKAGE (*TypeEncodePtr);
	while (num_instances--)
	{
	    MEM_HANDLE	hData;

	    hData = NULL_HANDLE;
	    data_len = MAKE_WORD (DataPtr[0], DataPtr[1]);
	    if (ResHeaderPtr->res_flags & IS_PACKAGED)
	    {
		if ((data_len <<= 2) == 0)
		    ;
		else if (SeekResFile (fp, (long)file_offs, 0) != file_offs)
		    ;
		else
		{
		    hData = DoLoad (ResHeaderPtr, type, fp, data_len);
		    if (hData && IsIndexType (ResHeaderPtr, type))
			DanglingOpen = FALSE;
#ifdef DEBUG
if (hData == 0)
printf ("fp = 0x%08lx(%d), type = %u, file_offs = %lu, len = %lu\n", fp, DanglingOpen, type, file_offs, data_len);
#endif /* DEBUG */
		}
		file_offs += data_len;
	    }
	    else
	    {
		get_resource_filename (ResHeaderPtr, file_buf, (COUNT)data_len);

		fp = OpenResFile (file_buf, "rb",
			NULL_PTR, 0L, (FILE_ERROR_FUNC)-1L);
		if (fp == NULL)
#ifdef DEBUG
printf ("Can't open '%s'\n", file_buf)
#endif /* DEBUG */
		    ;
		else
		{
		    data_len = LengthResFile (fp);
#ifndef PACKAGING
printf ("\t'%s' -- %lu bytes\n", file_buf, data_len);
#endif /* PACKAGING */
		    if (data_len == 0)
			hData = NULL_HANDLE;
		    else
			hData = DoLoad (ResHeaderPtr, type, fp, data_len);
		    if (hData == 0 || !IsIndexType (ResHeaderPtr, type))
			CloseResFile (fp);
		}
	    }
#ifdef DEBUG
if (hData == 0) printf ("Can't create data for '%s' <%lu>\n", file_buf, data_len);
#endif /* DEBUG */

	    DataPtr[0] = LOBYTE (hData);
	    DataPtr[1] = HIBYTE (hData);
	    DataPtr += 2;

	    if (hData)
		++ResourceHandleListPtr->num_valid_handles;
	}
	++TypeEncodePtr;
    }

    if (DanglingOpen)
	CloseResFile (fp);

    num_instances = ResourceHandleListPtr->num_valid_handles;
    UnlockResourceHandleList (hList);

    if (num_instances == 0)
    {
#ifdef DEBUG
printf ("Nothing in package!\n");
#endif /* DEBUG */
	FreeResourceHandleList (hList);
	hList = NULL_HANDLE;
    }

    return (hList);
}

PROC(
MEM_HANDLE GetResource, (res),
    ARG_END	(RESOURCE	res)
)
{
    MEM_HANDLE		hData, hList;
    RES_PACKAGE		res_package;
    RES_TYPE		res_type;
    RES_INSTANCE	res_instance;
    RES_HANDLE_LISTPTR	ResourceHandleListPtr;
    ENCODEPTR		TypeEncodePtr;
    DATAPTR		DataPtr;
    INDEX_HEADERPTR	ResHeaderPtr;
    UWORD		hi;

    ResHeaderPtr = _get_current_index_header ();
    if (!ValidResource (ResHeaderPtr, res))
{
#ifdef DEBUG
if (res) printf ("0x%08lx is not a valid resource!\n", res);
#endif /* DEBUG */
	return (NULL_HANDLE);
}

    res_package = GET_PACKAGE (res);

    hi = HIWORD (ResHeaderPtr->PackageList[res_package - 1].flags_and_data_loc);
    if (HIBYTE (hi) == 0)
	hList = (MEM_HANDLE)LOWORD (
		ResHeaderPtr->PackageList[res_package - 1].flags_and_data_loc);
    else
    {
	if ((hList = load_package (ResHeaderPtr, res_package)) == NULL_HANDLE)
{
#ifdef DEBUG
printf ("Unable to load package %u\n", res_package);
#endif /* DEBUG */
	    return (NULL_HANDLE);
}

	ResHeaderPtr->PackageList[res_package - 1].flags_and_data_loc =
		MAKE_DWORD (hList, 0);
    }

    res_type = GET_TYPE (res);
    res_instance = GET_INSTANCE (res);

    LockResourceHandleList (ResHeaderPtr, hList, res_package,
	    &ResourceHandleListPtr, &TypeEncodePtr, &DataPtr);

    while (GET_TYPE (*TypeEncodePtr) != res_type)
    {
	DataPtr += (GET_PACKAGE (*TypeEncodePtr) << 1);
	++TypeEncodePtr;
    }
    DataPtr += (res_instance - GET_INSTANCE (*TypeEncodePtr)) << 1;
    hData = MAKE_WORD (DataPtr[0], DataPtr[1]);
    UnlockResourceHandleList (hList);

#ifdef DEBUG
if (hData == 0) printf ("No data for res 0x%08lx\n", res);
#endif /* DEBUG */
    return (hData);
}

PROC(STATIC
MEM_HANDLE near get_res_handle, (res, DoDetach),
    ARG		(RESOURCE	res)
    ARG_END	(BOOLEAN	DoDetach)
)
{
    MEM_HANDLE		hData = NULL_HANDLE;
    RES_PACKAGE		res_package;
    RES_TYPE		res_type;
    RES_INSTANCE	res_instance;
    INDEX_HEADERPTR	ResHeaderPtr;
    UWORD		hi;

    ResHeaderPtr = _get_current_index_header ();
    if (!ValidResource (ResHeaderPtr, res))
	return (hData);

    res_package = GET_PACKAGE (res);
    res_type = GET_TYPE (res);
    res_instance = GET_INSTANCE (res);

    hi = HIWORD (ResHeaderPtr->PackageList[res_package - 1].flags_and_data_loc);
    if (HIBYTE (hi) == 0)
    {
	MEM_HANDLE		hList;
	COUNT			num_instances;
	RES_HANDLE_LISTPTR	ResourceHandleListPtr;
	ENCODEPTR		TypeEncodePtr;
	DATAPTR			DataPtr;
	DWORD			flags_and_data_loc;

	hList = LOWORD (ResHeaderPtr->PackageList[res_package - 1].flags_and_data_loc);
	LockResourceHandleList (ResHeaderPtr, hList, res_package,
		&ResourceHandleListPtr, &TypeEncodePtr, &DataPtr);

	while (GET_TYPE (*TypeEncodePtr) != res_type)
	{
	    DataPtr += (GET_PACKAGE (*TypeEncodePtr) << 1);
	    ++TypeEncodePtr;
	}
	DataPtr += (res_instance - GET_INSTANCE (*TypeEncodePtr)) << 1;
	hData = MAKE_WORD (DataPtr[0], DataPtr[1]);
	if (hData && DoDetach)
	{
	    DataPtr[0] = DataPtr[1] = 0;
	    --ResourceHandleListPtr->num_valid_handles;

	    flags_and_data_loc = ResourceHandleListPtr->flags_and_data_loc;
	}
	num_instances = ResourceHandleListPtr->num_valid_handles;
	UnlockResourceHandleList (hList);

	if (num_instances == 0)
	{
	    FreeResourceHandleList (hList);

	    ResHeaderPtr->PackageList[res_package - 1].flags_and_data_loc =
		    flags_and_data_loc;
	}
    }

    return (hData);
}

PROC(
BOOLEAN FreeResource, (res),
    ARG_END	(RESOURCE	res)
)
{
    MEM_HANDLE	hData;

    if (hData = get_res_handle (res, FALSE))
    {
	RES_TYPE	res_type;
	INDEX_HEADERPTR	ResHeaderPtr;

	res_type = GET_TYPE (res);
	ResHeaderPtr = _get_current_index_header ();
	if (DoFree (ResHeaderPtr, res_type, hData)
		&& get_res_handle (res, TRUE))
	    return (TRUE);
    }

    return (FALSE);
}

PROC(
MEM_HANDLE DetachResource, (res),
    ARG_END	(RESOURCE	res)
)
{
    return (get_res_handle (res, TRUE));
}

