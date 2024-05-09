#ifndef _MEMERR_H
#define _MEMERR_H

#ifndef REPORT_ERRORS

#define MEM_ERROR(args)		/* args */
#define MEM_ERROR_FATAL()
#define MEM_ERROR_INIT()	TRUE
#define MEM_ERROR_UNINIT()
#define MEM_ERROR_FUNC(name)
#define FUNC_NAME

#else /* REPORT_ERRORS */

#include "errlib.h"

typedef enum
{
    MEM_NO_ERROR = 0,
    MEM_NOT_INITIALIZED,
    MEM_ALREADY_INITIALIZED,
    MEM_NULL_HANDLE,
    MEM_HANDLE_ACCESSED,
    MEM_HANDLE_NOT_ACCESSED,
    MEM_POS_GREATER_THAN_SIZE,
    MEM_MIN_GREATER_THAN_MAX,
    MEM_MIN_GREATER_THAN_SIZE,
    MEM_CANT_ACCESS,
    MEM_BAD_ACCESS_MODE,
    MEM_BAD_SEEK_MODE,
    MEM_BAD_ABSOLUTE_OFFSET,
    MEM_ALLOC_FAILED,
    MEM_NOT_ENOUGH_MEMORY,
    MEM_CANT_CREATE_HANDLE,
    MEM_CANT_CREATE_MEMINFO,
    MEM_MEMORY_NOT_AVAILABLE,
    MEM_BAD_DIRECTION,
    MEM_BAD_RELATIVE_OFFSET,
    MEM_INCOMPATIBLE_HANDLE,
    MEM_MEMORY_CANT_BE_FIXED
} MEM_ERROR;

#define MEM_ERROR(args)		err_push args
#define MEM_ERROR_FATAL()	_mem_err_fatal ()
#define MEM_ERROR_INIT()	_mem_err_init ()
#define MEM_ERROR_UNINIT()	_mem_err_uninit ()
#define MEM_ERROR_FUNC(name)	_err_func_name = name
#define FUNC_NAME		_err_func_name

extern PSTR			far _err_func_name;
extern ERR_ID			far _mem_err_id;

PROC_GLOBAL(
MEM_BOOL near _mem_err_init, (),
    ARG_VOID
);
PROC_GLOBAL(
MEM_BOOL near _mem_err_uninit, (),
    ARG_VOID
);
PROC_GLOBAL(
void near _mem_err_fatal, (),
    ARG_VOID
);

#endif /* _REPORT_ERRORS */

#endif /* _MEMERR_H */

