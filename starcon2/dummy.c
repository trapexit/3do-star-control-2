#include <ctype.h>
#include "starcon.h"
#include "coderes.h"

PROC(
MEM_HANDLE LoadVidInstance, (res),
    ARG_END	(DWORD	res)
)
{
    return (0);
}

PROC(
void unlink, (filename),
    ARG_END	(PVOID	filename)
)
{
}

PROC(
MEM_HANDLE LoadCodeResFile, (pStr),
    ARG_END	(PSTR	pStr)
)
{
    return (0);
}

PROC(STATIC
MEM_HANDLE GetCodeResData, (fp, length),
    ARG		(FILE	*fp)
    ARG_END	(DWORD	length)
)
{
enum
{
    ANDROSYN_CODE_RES,
    ARILOU_CODE_RES,
    BLACKURQ_CODE_RES,
    CHENJESU_CODE_RES,
    CHMMR_CODE_RES,
    DRUUGE_CODE_RES,
    HUMAN_CODE_RES,
    ILWRATH_CODE_RES,
    MELNORME_CODE_RES,
    MMRNMHRM_CODE_RES,
    MYCON_CODE_RES,
    ORZ_CODE_RES,
    PKUNK_CODE_RES,
    SHOFIXTI_CODE_RES,
    SLYLANDR_CODE_RES,
    SPATHI_CODE_RES,
    SUPOX_CODE_RES,
    SYREEN_CODE_RES,
    THRADD_CODE_RES,
    UMGAH_CODE_RES,
    URQUAN_CODE_RES,
    UTWIG_CODE_RES,
    VUX_CODE_RES,
    YEHAT_CODE_RES,
    ZOQFOT_CODE_RES,

    SAMATRA_CODE_RES,
    SIS_CODE_RES,
    PROBE_CODE_RES
};

    BYTE	which_res;
    MEM_HANDLE	hData;

    which_res = GetResFileChar (fp);
    if (hData = mem_request (sizeof (MEM_HANDLE) + sizeof (RACE_DESC)))
    {
	RACE_DESCPTR	RDPtr;

	RDPtr = 0;
	switch (which_res)
	{
	    case ANDROSYN_CODE_RES:
	    {
		PROC_GLOBAL(
		RACE_DESCPTR init_androsynth, (),
		    ARG_VOID
		);

		RDPtr = init_androsynth ();
		break;
	    }
	    case ARILOU_CODE_RES:
	    {
		PROC_GLOBAL(
		RACE_DESCPTR init_arilou, (),
		    ARG_VOID
		);

		RDPtr = init_arilou ();
		break;
	    }
	    case BLACKURQ_CODE_RES:
	    {
		PROC_GLOBAL(
		RACE_DESCPTR init_black_urquan, (),
		    ARG_VOID
		);

		RDPtr = init_black_urquan ();
		break;
	    }
	    case CHENJESU_CODE_RES:
	    {
		PROC_GLOBAL(
		RACE_DESCPTR init_chenjesu, (),
		    ARG_VOID
		);

		RDPtr = init_chenjesu ();
		break;
	    }
	    case CHMMR_CODE_RES:
	    {
		PROC_GLOBAL(
		RACE_DESCPTR init_chmmr, (),
		    ARG_VOID
		);

		RDPtr = init_chmmr ();
		break;
	    }
	    case DRUUGE_CODE_RES:
	    {
		PROC_GLOBAL(
		RACE_DESCPTR init_druuge, (),
		    ARG_VOID
		);

		RDPtr = init_druuge ();
		break;
	    }
	    case HUMAN_CODE_RES:
	    {
		PROC_GLOBAL(
		RACE_DESCPTR init_human, (),
		    ARG_VOID
		);

		RDPtr = init_human ();
		break;
	    }
	    case ILWRATH_CODE_RES:
	    {
		PROC_GLOBAL(
		RACE_DESCPTR init_ilwrath, (),
		    ARG_VOID
		);

		RDPtr = init_ilwrath ();
		break;
	    }
	    case MELNORME_CODE_RES:
	    {
		PROC_GLOBAL(
		RACE_DESCPTR init_melnorme, (),
		    ARG_VOID
		);

		RDPtr = init_melnorme ();
		break;
	    }
	    case MMRNMHRM_CODE_RES:
	    {
		PROC_GLOBAL(
		RACE_DESCPTR init_mmrnmhrm, (),
		    ARG_VOID
		);

		RDPtr = init_mmrnmhrm ();
		break;
	    }
	    case MYCON_CODE_RES:
	    {
		PROC_GLOBAL(
		RACE_DESCPTR init_mycon, (),
		    ARG_VOID
		);

		RDPtr = init_mycon ();
		break;
	    }
	    case ORZ_CODE_RES:
	    {
		PROC_GLOBAL(
		RACE_DESCPTR init_orz, (),
		    ARG_VOID
		);

		RDPtr = init_orz ();
		break;
	    }
	    case PKUNK_CODE_RES:
	    {
		PROC_GLOBAL(
		RACE_DESCPTR init_pkunk, (),
		    ARG_VOID
		);

		RDPtr = init_pkunk ();
		break;
	    }
	    case SHOFIXTI_CODE_RES:
	    {
		PROC_GLOBAL(
		RACE_DESCPTR init_shofixti, (),
		    ARG_VOID
		);

		RDPtr = init_shofixti ();
		break;
	    }
	    case SLYLANDR_CODE_RES:
	    {
		PROC_GLOBAL(
		RACE_DESCPTR init_slylandro, (),
		    ARG_VOID
		);

		RDPtr = init_slylandro ();
		break;
	    }
	    case SPATHI_CODE_RES:
	    {
		PROC_GLOBAL(
		RACE_DESCPTR init_spathi, (),
		    ARG_VOID
		);

		RDPtr = init_spathi ();
		break;
	    }
	    case SUPOX_CODE_RES:
	    {
		PROC_GLOBAL(
		RACE_DESCPTR init_supox, (),
		    ARG_VOID
		);

		RDPtr = init_supox ();
		break;
	    }
	    case SYREEN_CODE_RES:
	    {
		PROC_GLOBAL(
		RACE_DESCPTR init_syreen, (),
		    ARG_VOID
		);

		RDPtr = init_syreen ();
		break;
	    }
	    case THRADD_CODE_RES:
	    {
		PROC_GLOBAL(
		RACE_DESCPTR init_thraddash, (),
		    ARG_VOID
		);

		RDPtr = init_thraddash ();
		break;
	    }
	    case UMGAH_CODE_RES:
	    {
		PROC_GLOBAL(
		RACE_DESCPTR init_umgah, (),
		    ARG_VOID
		);

		RDPtr = init_umgah ();
		break;
	    }
	    case URQUAN_CODE_RES:
	    {
		PROC_GLOBAL(
		RACE_DESCPTR init_urquan, (),
		    ARG_VOID
		);

		RDPtr = init_urquan ();
		break;
	    }
	    case UTWIG_CODE_RES:
	    {
		PROC_GLOBAL(
		RACE_DESCPTR init_utwig, (),
		    ARG_VOID
		);

		RDPtr = init_utwig ();
		break;
	    }
	    case VUX_CODE_RES:
	    {
		PROC_GLOBAL(
		RACE_DESCPTR init_vux, (),
		    ARG_VOID
		);

		RDPtr = init_vux ();
		break;
	    }
	    case YEHAT_CODE_RES:
	    {
		PROC_GLOBAL(
		RACE_DESCPTR init_yehat, (),
		    ARG_VOID
		);

		RDPtr = init_yehat ();
		break;
	    }
	    case ZOQFOT_CODE_RES:
	    {
		PROC_GLOBAL(
		RACE_DESCPTR init_zoqfotpik, (),
		    ARG_VOID
		);

		RDPtr = init_zoqfotpik ();
		break;
	    }
	    case SAMATRA_CODE_RES:
	    {
		PROC_GLOBAL(
		RACE_DESCPTR init_samatra, (),
		    ARG_VOID
		);

		RDPtr = init_samatra ();
		break;
	    }
	    case SIS_CODE_RES:
	    {
		PROC_GLOBAL(
		RACE_DESCPTR init_sis, (),
		    ARG_VOID
		);

		RDPtr = init_sis ();
		break;
	    }
	    case PROBE_CODE_RES:
	    {
		PROC_GLOBAL(
		RACE_DESCPTR init_probe, (),
		    ARG_VOID
		);

		RDPtr = init_probe ();
		break;
	    }
	}

	if (RDPtr == 0)
	{
	    mem_release (hData);
	    hData = 0;
	}
	else
	{
	    MEM_HANDLE	*lp;

	    lp = (MEM_HANDLE *)mem_lock (hData);
	    *((RACE_DESCPTR)&lp[1]) = *RDPtr;
	    mem_unlock (hData);
	}
    }

    return (hData);
}

PROC(
BOOLEAN InstallCodeResType, (code_type),
    ARG_END	(COUNT	code_type)
)
{
    return (InstallResTypeVectors (code_type,
	    GetCodeResData, mem_release));
}

PROC(
MEM_HANDLE LoadCodeResInstance, (res),
    ARG_END	(DWORD	res)
)
{
    MEM_HANDLE	hData;

    if (hData = GetResource (res))
	DetachResource (res);

    return (hData);
}

PROC(
BOOLEAN DestroyCodeRes, (hCode),
    ARG_END	(MEM_HANDLE	hCode)
)
{
    return (mem_release (hCode));
}

PROC(
LPVOID CaptureCodeRes, (hCode, lpData, plpLocData, glue_func),
    ARG		(MEM_HANDLE	hCode)
    ARG		(LPVOID		lpData)
    ARG		(LPVOID		*plpLocData)
    ARG_END	(GLUEFUNC	glue_func)
)
{
    MEM_HANDLE	*lp;

    lp = (MEM_HANDLE *)mem_lock (hCode);
    *lp = hCode;
    *plpLocData = &lp[1];

    return (lp);
}

PROC(
MEM_HANDLE ReleaseCodeRes, (CodeRef),
    ARG_END	(LPVOID	CodeRef)
)
{
    if (CodeRef)
    {
	MEM_HANDLE	hCode;

	mem_unlock (hCode = *(MEM_HANDLE *)CodeRef);
	return (hCode);
    }

    return (0);
}

PROC(
DWORD CDECL race_glue, (selector, args),
    ARG		(COUNT 		selector)
    ARG		(vararg_dcl	args)
    ARG_VAR
)
{
    return (0);
}

PROC(
DRAWABLE CreatePixmapRegion, (Frame, lpOrg, width, height),
    ARG		(FRAME		Frame)
    ARG		(LPPOINT	lpOrg)
    ARG		(SIZE		width)
    ARG_END	(SIZE		height)
)
{
    return (GetFrameHandle (Frame));
}

PROC(
void SetPrimNextLink, (pPrim, Link),
    ARG		(PPRIMITIVE	pPrim)
    ARG_END	(COUNT		Link)
)
{
    SetPrimLinks (pPrim, END_OF_LIST, Link);
}

PROC(
COUNT GetPrimNextLink, (pPrim),
    ARG_END	(PPRIMITIVE	pPrim)
)
{
    return (GetSuccLink (GetPrimLinks (pPrim)));
}

PROC(
char *fgets, (s, n, fp),
    ARG		(char *s)
    ARG 	(int n)
    ARG_END	(FILE 	*fp)
)
{
	return (0);
}

PROC(
BYTE ScanToASCII, (which_scan),
    ARG_END	(BYTE	which_scan)
)
{
	return (0);
}

PROC(
int abs, (n),
    ARG_END		(int n)
)
{
	if (n < 0) n = -n;
	return (n);
}
