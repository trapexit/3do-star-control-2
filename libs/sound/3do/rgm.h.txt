// rgm.h
// 11/6/93 (SAT) - Created. (RGM)

#ifndef __RGM_H__
#define __RGM_H__

//************ M A C R O S ****************

#ifndef ERROR_MESSAGES
#define ERROR_MESSAGES	(1)
#endif

#ifndef INFO_MESSAGES
#define INFO_MESSAGES	(0)
#endif

#ifndef DEBUG_MESSAGES
#define DEBUG_MESSAGES	(0)
#endif


#define IS_NOT_ITEM(val)				((val) < 0)
#define IS_ITEM(val)					((val) >= 0)
#define IS_ERROR(val)					(val)
#define IS_NOT_ERROR(val)				(! (val))
#define NOT_ERROR(val) 					((val) >= 0L)
#define NULL_ITEM						(-1)


#if DEBUG_MESSAGES
#define INFORM(message)							{printf (message);}
#define INFORM0(message)						{printf (message);}
#define INFORM1(message,arg1)					{printf (message, arg1);}
#define INFORM2(message,arg1,arg2)				{printf (message, arg1, arg2);}
#define INFORM3(message,arg1,arg2,arg3)			{printf (message, arg1, arg2, arg3);}
#define INFORM4(message,arg1,arg2,arg3,arg4)	{printf (message, arg1, arg2, arg3, arg4);}
#else
#define INFORM(message)					
#define INFORM0(message)				
#define INFORM1(message,a)				
#define INFORM2(message,a,b)				
#define INFORM3(message,a,b,c)				
#define INFORM4(message,a,b,c,d)				
#endif

#if INFO_MESSAGES
#define INFO(message)						{printf (message);}
#define INFO0(message)						{printf (message);}
#define INFO1(message,arg1)					{printf (message, arg1);}
#define INFO2(message,arg1,arg2)			{printf (message, arg1, arg2);}
#define INFO3(message,arg1,arg2,arg3)		{printf (message, arg1, arg2, arg3);}
#define INFO4(message,arg1,arg2,arg3,arg4)	{printf (message, arg1, arg2, arg3, arg4);}
#else
#define INFO(message)					
#define INFO0(message)				
#define INFO1(message,a)				
#define INFO2(message,a,b)				
#define INFO3(message,a,b,c)				
#define INFO4(message,a,b,c,d)				
#endif

#if ERROR_MESSAGES
#define ERROR(message)						{printf (message);}
#define ERROR0(message)						{printf (message);}
#define ERROR1(message,arg1)				{printf (message, arg1);}
#define ERROR2(message,arg1,arg2)			{printf (message, arg1, arg2);}
#define ERROR3(message,arg1,arg2,arg3)		{printf (message, arg1, arg2, arg3);}
#define ERROR4(message,arg1,arg2,arg3,arg4)	{printf (message, arg1, arg2, arg3, arg4);}
#else
#define ERROR(message)					
#define ERROR0(message)				
#define ERROR1(message,a)				
#define ERROR2(message,a,b)				
#define ERROR3(message,a,b,c)				
#define ERROR4(message,a,b,c,d)				
#endif

#define ON_NEG_ABORT(sym, message)		if ((sym) < 0) { ERROR(message); goto ABORT;}
#define ON_NOT_ITEM_ABORT(sym, message)	if ((sym) < 0) { ERROR(message); goto ABORT;}
#define ON_ERROR_ABORT(sym, message)	if (sym) { ERROR(message); goto ABORT;}
#define ON_NULL_ABORT(sym, message)		if (! (sym)) { ERROR(message); goto ABORT;}
#define ON_TRUE_ABORT(sym, message)		if (sym) { ERROR(message); goto ABORT;}
#define ON_FALSE_ABORT(sym, message)	if (!(sym)) { ERROR(message); goto ABORT;}
#define ON_ABORT(message)				{ ERROR(message); goto ABORT;}

#define setflg(flag,var)	((var) |= (1<<(flag)))
#define clrflg(flag,var)	((var) &= ((1<<(flag)) ^ 0xFFFFFFFFL))
#define chkflg(flag,var)	((var) & (1<<(flag)))

#define INC_CIRCULAR(var,max)	(var) = ((var) >= ((max)-1)) ? 0 : (var) + 1
#define DEC_CIRCULAR(var,max)	(var) = ((var) <= (0)) ? ((max)-1) : (var) - 1
#define NUM_NEXT(var,max)		(((var) >= ((max)-1)) ? 0 : (var) + 1)
#define NUM_PREVIOUS(var,max)	(((var) <= (0)) ? ((max)-1) : (var) - 1)
#define min(a,b) ((a) < (b) ? (a) : (b))
#define max(a,b) ((a) > (b) ? (a) : (b))
#define abs(a)	(((a) < 0) ? (-(a)) : (a))

#ifndef TRUE
#define TRUE	(1)
#endif

#ifndef FALSE
#define FALSE	(0)
#endif

#define FILESYSTEM_BLOCK_SIZE	(1024 * 2)
#define GET_MULTIPLE_OF(size,n)	(((n) + ((size) - 1))/(size)*(size))

#define NEXT_CEL(cel_ptr)	((cel_ptr)->ccb_NextPtr)

#define FLAG_BITS(flag)	(1<<(flag))

#endif
