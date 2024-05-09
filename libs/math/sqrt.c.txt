#include "mthintrn.h"

PROC(
COUNT square_root, (value),
    ARG_END	(DWORD	value)
)
{
    UWORD	sig_word, mask;
    COUNT	result, shift;

    if ((sig_word = HIWORD (value)) > 0)
    {
	DWORD	mask_squared, result_shift;

	for (mask = 1 << 15, shift = 31;
		!(mask & sig_word); mask >>= 1, --shift)
	    ;
	shift >>= 1;
	mask = 1 << shift;

	result = mask;
	mask_squared = result_shift = (DWORD)mask << shift;
	value -= mask_squared;
	while (mask >>= 1)
	{
	    DWORD	remainder;

	    mask_squared >>= 1;
	    mask_squared >>= 1;
	    if ((remainder = result_shift + mask_squared) > value)
		result_shift >>= 1;
	    else
	    {
		value -= remainder;

		result_shift = (result_shift >> 1) + mask_squared;

		result |= mask;
	    }
	}

	return (result);
    }
    else if ((sig_word = LOWORD (value)) > 0)
    {
	UWORD	mask_squared, result_shift;

	for (mask = 1 << 15, shift = 15;
		!(mask & sig_word); mask >>= 1, --shift)
	    ;
	shift >>= 1;
	mask = 1 << shift;

	result = mask;
	mask_squared = result_shift = mask << shift;
	sig_word -= mask_squared;
	while (mask >>= 1)
	{
	    UWORD	remainder;

	    mask_squared >>= 1;
	    mask_squared >>= 1;
	    if ((remainder = result_shift + mask_squared) > sig_word)
		result_shift >>= 1;
	    else
	    {
		sig_word -= remainder;

		result_shift = (result_shift >> 1) + mask_squared;

		result |= mask;
	    }
	}

	return (result);
    }

    return (0);
}


