#ifndef _ERRLIB_H
#define _ERRLIB_H

#include "compiler.h"
#include "vargs.h"

typedef SWORD		ERR_ID;
typedef DWORD		ERR_NO;

typedef BOOLEAN		ERR_BOOL;
#define ERR_FAILURE	FALSE
#define ERR_SUCCESS	TRUE

typedef enum
{
    ERR_IN_MEMORY,
    ERR_IN_FILE
} ERR_LOC;

PROC_GLOBAL(
ERR_ID err_register, (module_name, err_loc, err_string_path, num_errors),
    ARG		(PSTR		module_name)
    ARG		(ERR_LOC	err_loc)
    ARG		(PSTR		err_string_path)
    ARG_END	(COUNT		num_errors)
);
PROC_GLOBAL(
ERR_BOOL err_unregister, (err_id),
    ARG_END	(ERR_ID		err_id)
);
PROC_GLOBAL(
ERR_BOOL CDECL err_push, (err_id, func_name, err_no, alist),
    ARG		(ERR_ID		err_id)
    ARG		(PSTR		func_name)
    ARG		(ERR_NO		err_no)
    ARG		(vararg_dcl	alist)
    ARG_VAR
);
PROC_GLOBAL(
ERR_BOOL err_clear, (),
    ARG_VOID
);
PROC_GLOBAL(
ERR_BOOL err_report, (err_func),
    ARG_END
    (
	PROC_PARAMETER(
	ERR_BOOL (far *err_func), (top_stack, mname, fname, pStr),
	    ARG		(COUNT	top_stack)
	    ARG		(PSTR	mname)
	    ARG		(PSTR	fname)
	    ARG_END	(PSTR	pStr)
	)
    )
);

#endif /* _ERRLIB_H */

