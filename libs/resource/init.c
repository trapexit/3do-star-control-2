#include "resintrn.h"

static MEM_HANDLE	hIndexList;

PROC(STATIC
void near _add_index_list, (hRH),
    ARG_END	(MEM_HANDLE	hRH)
)
{
#ifndef PACKAGING
    INDEX_HEADERPTR	ResHeaderPtr;

    ResHeaderPtr = LockResourceHeader (hRH);
    strncpy (ResHeaderPtr->index_file_name, _cur_resfile_name,
	    sizeof (ResHeaderPtr->index_file_name) - 1);
    ResHeaderPtr->hPredHeader = 0;
    if (ResHeaderPtr->hSuccHeader = hIndexList)
    {
	INDEX_HEADERPTR	SuccResHeaderPtr;

	SuccResHeaderPtr = LockResourceHeader (hIndexList);
	SuccResHeaderPtr->hPredHeader = hRH;
	UnlockResourceHeader (hIndexList);
    }
    UnlockResourceHeader (hRH);
#endif /* PACKAGING */

    hIndexList = hRH;
}

PROC(STATIC
void near _sub_index_list, (hRH),
    ARG_END	(MEM_HANDLE	hRH)
)
{
#ifdef PACKAGING
    hIndexList = 0;
#else /* !PACKAGING */
    INDEX_HEADERPTR	ResHeaderPtr;
    MEM_HANDLE		hPred, hSucc;

    ResHeaderPtr = LockResourceHeader (hRH);

    hPred = ResHeaderPtr->hPredHeader;
    hSucc = ResHeaderPtr->hSuccHeader;
    if (hRH == hIndexList)
	hIndexList = hSucc;

    if (hPred)
    {
	INDEX_HEADERPTR	PredResHeaderPtr;

	PredResHeaderPtr = LockResourceHeader (hPred);
	PredResHeaderPtr->hSuccHeader = hSucc;
	UnlockResourceHeader (hPred);
    }
    if (hSucc)
    {
	INDEX_HEADERPTR	SuccResHeaderPtr;

	SuccResHeaderPtr = LockResourceHeader (hSucc);
	SuccResHeaderPtr->hPredHeader = hPred;
	UnlockResourceHeader (hSucc);
    }

    UnlockResourceHeader (hRH);
#endif /* PACKAGING */
}

PROC(
MEM_HANDLE _GetResFileData, (res_fp, flen),
    ARG		(FILE	*res_fp)
    ARG_END	(DWORD	flen)
)
{
    UWORD		lo_word, hi_word;
    DWORD		res_offs, remainder;
    MEM_SIZE		HeaderSize;
    INDEX_HEADER	h;
    INDEX_HEADERPTR	ResHeaderPtr;
    MEM_HANDLE		hRH;
    char		buf[32];

    res_offs = TellResFile (res_fp);

    ReadResFile (buf, 1, 22, res_fp);

    h.res_fp = res_fp;
    h.res_flags = MAKE_WORD (buf[0], buf[1]) ? IS_PACKAGED : 0;

    lo_word = MAKE_WORD (buf[2], buf[3]);
    hi_word = MAKE_WORD (buf[4], buf[5]);
    h.packmem_list_offs = MAKE_DWORD (lo_word, hi_word);

    lo_word = MAKE_WORD (buf[6], buf[7]);
    hi_word = MAKE_WORD (buf[8], buf[9]);
    h.path_list_offs = MAKE_DWORD (lo_word, hi_word);

    lo_word = MAKE_WORD (buf[10], buf[11]);
    hi_word = MAKE_WORD (buf[12], buf[13]);
    h.file_list_offs = MAKE_DWORD (lo_word, hi_word);

    h.num_packages = (RES_PACKAGE)MAKE_WORD (buf[14], buf[15]);
    h.num_types = (RES_TYPE)MAKE_WORD (buf[16], buf[17]);

    lo_word = MAKE_WORD (buf[18], buf[19]);
    hi_word = MAKE_WORD (buf[20], buf[21]);
    h.index_info.header_len = MAKE_DWORD (lo_word, hi_word);

    HeaderSize = (MEM_SIZE)(sizeof (INDEX_HEADER)
	    + (sizeof (PACKAGE_DESC) * h.num_packages)
	    + (sizeof (TYPE_DESC) * h.num_types));
    if (h.res_flags & IS_PACKAGED)
    {
	DWORD	offs;

	remainder = h.index_info.header_len - h.packmem_list_offs;
	offs = HeaderSize - h.packmem_list_offs;
	HeaderSize += remainder;
	h.packmem_list_offs += offs;
	h.file_list_offs += offs;
	h.path_list_offs += offs;
    }

#ifndef PACKAGING
    {
	MEM_HANDLE	hNextRH;

	h.data_offs = res_offs;
	for (hRH = hIndexList; hRH != 0; hRH = hNextRH)
	{
	    ResHeaderPtr = LockResourceHeader (hRH);
	    if (h.data_offs == ResHeaderPtr->data_offs
		    && strncmp (ResHeaderPtr->index_file_name,
		    _cur_resfile_name,
		    sizeof (ResHeaderPtr->index_file_name) - 1) == 0)
		return (hRH);	/* DON'T UNLOCK IT */

	    hNextRH = ResHeaderPtr->hSuccHeader;
	    UnlockResourceHeader (hRH);
	}
    }
#endif /* PACKAGING */

    if ((hRH = AllocResourceHeader (HeaderSize))
	    && (ResHeaderPtr = LockResourceHeader (hRH)))
    {
	*ResHeaderPtr = h;

	{
	    RES_PACKAGE	p;

	    ResHeaderPtr->PackageList = (PPACKAGE_DESC)&ResHeaderPtr[1];
	    for (p = 0; p < ResHeaderPtr->num_packages; ++p)
	    {
		ReadResFile (buf, 1, 8, res_fp);

		lo_word = MAKE_WORD (buf[0], buf[1]);
		hi_word = MAKE_WORD (buf[2], buf[3]);
		ResHeaderPtr->PackageList[p].packmem_info =
			(RESOURCE)MAKE_DWORD (lo_word, hi_word);

		lo_word = MAKE_WORD (buf[4], buf[5]);
		hi_word = MAKE_WORD (buf[6], buf[7]);
		ResHeaderPtr->PackageList[p].flags_and_data_loc =
			MAKE_DWORD (lo_word, hi_word) + res_offs;
	    }
	}

	{
	    RES_TYPE		t;
	    INDEX_HEADERPTR	CurResHeaderPtr;

	    CurResHeaderPtr = _get_current_index_header ();

	    ResHeaderPtr->TypeList =
		    (PTYPE_DESC)&ResHeaderPtr->PackageList[ResHeaderPtr->num_packages];
	    for (t = 0; t < ResHeaderPtr->num_types; ++t)
	    {
		ReadResFile (buf, 1, 2, res_fp);

		ResHeaderPtr->TypeList[t].instance_count =
			MAKE_WORD (buf[0], buf[1]);
		if (CurResHeaderPtr)
		    ResHeaderPtr->TypeList[t].func_vectors =
			    CurResHeaderPtr->TypeList[t].func_vectors;
	    }
	}

	if (h.res_flags & IS_PACKAGED)
	    ReadResFile (&ResHeaderPtr->TypeList[ResHeaderPtr->num_types],
		    1, (COUNT)remainder, res_fp);

	_add_index_list (hRH);

	return (hRH);		/* DON'T UNLOCK IT */
    }

    FreeResourceHeader (hRH);
    return (NULL_HANDLE);
}

PROC(
MEM_HANDLE InitResourceSystem, (resfile, resindex_type, FileErrorFunc),
    ARG		(PVOID		resfile)
    ARG		(COUNT		resindex_type)
    ARG_END
    (
	PROC_PARAMETER(
	BOOLEAN (*FileErrorFunc), (filename),
	    ARG_END	(PVOID	filename)
	)
    )
)
{
    MEM_HANDLE	h;
    char	fname[80];

    sprintf (fname, "%c:%s", CD_get_drive () + 'a', resfile);
    if ((h = OpenResourceIndexFile (&fname[2]))
	    || (CD_get_volsize ()
	    && (h = OpenResourceIndexFile (fname))))
    {
	SetResourceIndex (h);

	InstallResTypeVectors (resindex_type,
		_GetResFileData, FreeResourceHeader);
	_init_resfile_error (FileErrorFunc);
    }

    return (h);
}

PROC(
BOOLEAN UninitResourceSystem, (),
    ARG_VOID
)
{
    if (hIndexList)
    {
	do
	    CloseResourceIndex (hIndexList);
	while (hIndexList);

	_uninit_resfile_error ();

	return (TRUE);
    }

    return (FALSE);
}

PROC(
MEM_HANDLE OpenResourceIndexFile, (resfile),
    ARG_END	(PVOID		resfile)
)
{
    FILE	*res_fp;
    char	fullname[256];

    strcpy (fullname, resfile);
    if ((res_fp = OpenResFile (fullname, "rb",
	    NULL_PTR, 0L, (FILE_ERROR_FUNC)-1L)) == 0)
    {
	sprintf (fullname, "%s.pkg", resfile);
	if ((res_fp = OpenResFile (fullname, "rb",
		NULL_PTR, 0L, (FILE_ERROR_FUNC)-1L)) == 0)
	{
	    sprintf (fullname, "%s.ndx", resfile);
	    res_fp = OpenResFile (fullname, "rb",
		    NULL_PTR, 0L, (FILE_ERROR_FUNC)-1L);
	}
    }

    if (res_fp)
    {
	MEM_HANDLE	hRH;

	_cur_resfile_name = fullname;

	hRH = _GetResFileData (res_fp, LengthResFile (res_fp));
	/* DO NOT CloseResFile!!! */

	return (hRH);
    }

    return (0);
}

PROC(
MEM_HANDLE OpenResourceIndexInstance, (res),
    ARG_END	(DWORD		res)
)
{
    MEM_HANDLE	hRH;

    if (hRH = GetResource (res))
	DetachResource (res);

    return (hRH);
}

PROC(
MEM_HANDLE SetResourceIndex, (hRH),
    ARG_END	(MEM_HANDLE	hRH)
)
{
    MEM_HANDLE		hOldRH;
    static MEM_HANDLE	hCurResHeader;

    if ((hOldRH = hCurResHeader) != hRH)
    {
	INDEX_HEADERPTR	ResHeaderPtr;

	ResHeaderPtr = LockResourceHeader (hRH);
	UnlockResourceHeader (hRH);
	_set_current_index_header (ResHeaderPtr);
	hCurResHeader = hRH;
    }

    return (hOldRH);
}

PROC(
BOOLEAN CloseResourceIndex, (hRH),
    ARG_END	(MEM_HANDLE	hRH)
)
{
    if (UnlockResourceHeader (hRH))
    {
	FILE		*res_fp;
	INDEX_HEADERPTR	ResHeaderPtr;

	ResHeaderPtr = LockResourceHeader (hRH);
#ifdef DEBUG
{
COUNT	i;

for (i = 0; i < ResHeaderPtr->num_packages; ++i)
{
    UWORD	hi;

    hi = HIWORD (ResHeaderPtr->PackageList[i].flags_and_data_loc);
    if (HIBYTE (hi) == 0)
    {
	MEM_HANDLE		hList;
	RES_HANDLE_LISTPTR	ResourceHandleListPtr;
	ENCODEPTR		TypeEncodePtr;
	DATAPTR			DataPtr;

	hList = (MEM_HANDLE)LOWORD (
		ResHeaderPtr->PackageList[i].flags_and_data_loc);
	LockResourceHandleList (ResHeaderPtr, hList, i + 1,
		&ResourceHandleListPtr, &TypeEncodePtr, &DataPtr);
	printf ("Package %u has %u instances left\n",
		i + 1, ResourceHandleListPtr->num_valid_handles);
	UnlockResourceHandleList (hList);
    }
}
}
#endif /* DEBUG */

	_sub_index_list (hRH);
	res_fp = ResHeaderPtr->res_fp;
	if (ResHeaderPtr == _get_current_index_header ())
	    SetResourceIndex (hIndexList);
	UnlockResourceHeader (hRH);
	FreeResourceHeader (hRH);

	if (res_fp)
	{
	    MEM_HANDLE	hNextRH;

	    for (hRH = hIndexList; hRH && res_fp; hRH = hNextRH)
	    {
		ResHeaderPtr = LockResourceHeader (hRH);
		if (res_fp == ResHeaderPtr->res_fp)
		    res_fp = 0;
		hNextRH = ResHeaderPtr->hSuccHeader;
		UnlockResourceHeader (hRH);
	    }

	    if (res_fp)
		CloseResFile (res_fp);
	}

	return (TRUE);
    }

    return (FALSE);
}

PROC(
BOOLEAN InstallResTypeVectors, (res_type, load_func, free_func),
    ARG		(COUNT		res_type)
    ARG
    (
	PROC_PARAMETER(
	MEM_HANDLE (*load_func), (fp, len),
	    ARG		(FILE		*fp)
	    ARG_END	(DWORD		len)
	)
    )
    ARG_END
    (
	PROC_PARAMETER(
	BOOLEAN (*free_func), (handle),
	    ARG_END	(MEM_HANDLE	handle)
	)
    )
)
{
    INDEX_HEADERPTR	ResHeaderPtr;

    ResHeaderPtr = _get_current_index_header ();
    if (ValidResType (ResHeaderPtr, res_type))
    {
	ResHeaderPtr->TypeList[res_type - 1].func_vectors.load_func = load_func;
	ResHeaderPtr->TypeList[res_type - 1].func_vectors.free_func = free_func;

	return (TRUE);
    }

    return (FALSE);
}

PROC(
COUNT CountResourceTypes, (),
    ARG_VOID
)
{
    INDEX_HEADERPTR	ResHeaderPtr;

    ResHeaderPtr = _get_current_index_header ();
    return ((COUNT)ResHeaderPtr->num_types);
}

