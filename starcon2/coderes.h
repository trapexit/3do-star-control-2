#ifndef _CODERES_H
#define _CODERES_H

#include "reslib.h"
#include "vargs.h"

typedef PROC_PARAMETER(
	DWORD (CDECL far *GLUEFUNC), (selector, args),
	    ARG		(COUNT 		selector)
	    ARG		(vararg_dcl	args)
	    ARG_VAR
	);

PROC_GLOBAL(
MEM_HANDLE LoadCodeResFile, (pStr),
    ARG_END	(PSTR	pStr)
);
PROC_GLOBAL(
BOOLEAN InstallCodeResType, (code_type),
    ARG_END	(COUNT	code_type)
);
PROC_GLOBAL(
MEM_HANDLE LoadCodeResInstance, (res),
    ARG_END	(RESOURCE	res)
);
PROC_GLOBAL(
LPVOID CaptureCodeRes, (hCode, lpData, plpLocData, glue_func),
    ARG		(MEM_HANDLE	hCode)
    ARG		(LPVOID		lpData)
    ARG		(LPVOID		*plpLocData)
    ARG_END	(GLUEFUNC	glue_func)
);
PROC_GLOBAL(
MEM_HANDLE ReleaseCodeRes, (CodeRef),
    ARG_END	(LPVOID	CodeRef)
);
PROC_GLOBAL(
BOOLEAN DestroyCodeRes, (hCode),
    ARG_END	(MEM_HANDLE	hCode)
);

typedef struct
{
    MEM_HANDLE	hCode;
    UWORD	size;
} CODE_REF;
typedef CODE_REF	*PCODE_REF;
typedef CODE_REF	near *NPCODE_REF;
typedef CODE_REF	far *LPCODE_REF;

#define ORG_OFFSET	(((sizeof (CODE_REF) + (PARAGRAPH_SIZE - 1)) \
				/ PARAGRAPH_SIZE) * PARAGRAPH_SIZE)

#endif /* _CODERES_H */

