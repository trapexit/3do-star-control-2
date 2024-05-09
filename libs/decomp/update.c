#include <string.h>
#include "lzh.h"

PROC(STATIC
void near reconst, (),
    ARG_VOID
)
{
    COUNT	i, j;

    /* halven cumulative freq for leaf nodes */
    j = 0;
    for (i = 0; i < T; i++)
    {
	if (_lpCurCodeDesc->son[i] >= T)
	{
	    _lpCurCodeDesc->freq[j] = (_lpCurCodeDesc->freq[i] + 1) >> 1;
	    _lpCurCodeDesc->son[j] = _lpCurCodeDesc->son[i];
	    j++;
	}
    }
    /* make a tree : first, connect children nodes */
    for (i = 0, j = N_CHAR; j < T; i += 2, j++)
    {
	SWORD	k;
	UWORD	f, l;

	k = i + 1;
	f = _lpCurCodeDesc->freq[j] = _lpCurCodeDesc->freq[i] + _lpCurCodeDesc->freq[k];
	for (k = j - 1; f < _lpCurCodeDesc->freq[k]; k--)
	    ;
	k++;
	l = (j - k) << 1;
	
	MEMMOVE ((LPBYTE)&_lpCurCodeDesc->freq[k + 1],
		(LPBYTE)&_lpCurCodeDesc->freq[k], l);
	_lpCurCodeDesc->freq[k] = f;
	MEMMOVE ((LPBYTE)&_lpCurCodeDesc->son[k + 1],
		(LPBYTE)&_lpCurCodeDesc->son[k], l);
	_lpCurCodeDesc->son[k] = i;
    }
    /* connect parent nodes */
    for (i = 0; i < T; i++)
    {
	if ((j = _lpCurCodeDesc->son[i]) >= T)
	    _lpCurCodeDesc->prnt[j] = i;
	else
	    _lpCurCodeDesc->prnt[j] = _lpCurCodeDesc->prnt[j + 1] = i;
    }
}


/* update freq tree */

PROC(
void near _update, (c),
    ARG_END	(COUNT	c)
)
{
    register LPLZHCODE_DESC	lpCD;

    if ((lpCD = _lpCurCodeDesc)->freq[R] == MAX_FREQ)
	reconst ();

    c = lpCD->prnt[c];
    do
    {
	COUNT	i, l;

	i = ++lpCD->freq[c];

	/* swap nodes to keep the tree freq-ordered */
	if (i > lpCD->freq[l = c + 1])
	{
	    COUNT	j;

	    while (i > lpCD->freq[++l])
		;
	    l--;
	    lpCD->freq[c] = lpCD->freq[l];
	    lpCD->freq[l] = i;

	    i = lpCD->son[c];
	    j = lpCD->son[l];
	    lpCD->son[l] = i;
	    lpCD->son[c] = j;

	    lpCD->prnt[i] = l;
	    if (i < T)
		lpCD->prnt[i + 1] = l;

	    lpCD->prnt[j] = c;
	    if (j < T)
		lpCD->prnt[j + 1] = c;

	    c = l;
	}
    } while ((c = lpCD->prnt[c]) != 0);	/* do it until reaching the root */
}


