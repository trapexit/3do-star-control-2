#ifndef _VARARGS_H
#define _VARARGS_H

typedef char	*vararg_list;
typedef char	*vararg_dcl;
#define vararg_start(list,alist) (list) = (vararg_list)&(alist)
#define vararg_end(list)
#define vararg_val(list,type)	\
	(sizeof (type) >= sizeof (int) ? \
	(((type *)((list) += sizeof (type)))[-1]) : \
	((type)((int *)((list) += sizeof (int)))[-1]))
#define vararg_farval(list,type)	\
	(sizeof (type) >= sizeof (int) ? \
	(((type far *)((list) += sizeof (type)))[-1]) : \
	(((int far *)((list) += sizeof (int)))[-1]))
#define begin_arg_stack(name)	char	name[0
#define add_to_arg_stack(type)		+ (sizeof (type) > sizeof (int) ? \
			       		   sizeof (type) : sizeof (int))
#define end_arg_stack       		];
#define arg_stack_push(list,type,val)	\
	(sizeof (type) >= sizeof (int) ? \
	(((type *)((list) += sizeof (type)))[-1] = (val)) : \
	(((int *)((list) += sizeof (int)))[-1] = (int)(val)))


#endif /* _VARARGS_H */

