#include "displist.h"

PROC(
BOOLEAN InitQueue, (pq, num_elements, size),
    ARG		(PQUEUE		pq)
    ARG		(COUNT		num_elements)
    ARG_END	(OBJ_SIZE	size)
)
{
    SetHeadLink (pq, NULL_HANDLE);
    SetTailLink (pq, NULL_HANDLE);
    SetLinkSize (pq, size);
#ifndef QUEUE_TABLE
    return (TRUE);
#else /* QUEUE_TABLE */
    SetFreeList (pq, NULL_HANDLE);
// printf ("num_elements = %d (%d)\n", num_elements, (BYTE)num_elements);
    if (AllocQueueTab (pq, num_elements) && LockQueueTab (pq))
    {
	do
	    FreeLink (pq, GetLinkAddr (pq, num_elements));
	while (--num_elements);

	return (TRUE);
    }

    return (FALSE);
#endif /* QUEUE_TABLE */
}

PROC(
BOOLEAN UninitQueue, (pq),
    ARG_END	(PQUEUE	pq)
)
{
#ifdef QUEUE_TABLE
    SetHeadLink (pq, NULL_HANDLE);
    SetTailLink (pq, NULL_HANDLE);
    SetFreeList (pq, NULL_HANDLE);
    UnlockQueueTab (pq);
    FreeQueueTab (pq);

    return (TRUE);
#else /* !QUEUE_TABLE */
    HLINK	hLink;

    while ((hLink = GetHeadLink (pq)) != NULL_HANDLE)
    {
	RemoveQueue (pq, hLink);
	if (!FreeLink (pq, hLink))
	    return (FALSE);
    }

    return (TRUE);
#endif /* QUEUE_TABLE */
}

PROC(
void ReinitQueue, (pq),
    ARG_END	(PQUEUE	pq)
)
{
    SetHeadLink (pq, NULL_HANDLE);
    SetTailLink (pq, NULL_HANDLE);
#ifdef QUEUE_TABLE
    {
	COUNT	num_elements;

	SetFreeList (pq, NULL_HANDLE);

	if (num_elements = SizeQueueTab (pq))
	{
	    do
		FreeLink (pq, GetLinkAddr (pq, num_elements));
	    while (--num_elements);
	}
    }
#endif /* QUEUE_TABLE */
}

#ifdef QUEUE_TABLE
PROC(
HLINK AllocLink, (pq),
    ARG_END	(PQUEUE	pq)
)
{
    HLINK	hLink;

    if (hLink = GetFreeList (pq))
    {
	LINKPTR	LinkPtr;

	LinkPtr = LockLink (pq, hLink);
	SetFreeList (pq, _GetSuccLink (LinkPtr));
	UnlockLink (pq, hLink);
    }
/*
else
printf ("No more elements\n");
*/
    return (hLink);
}

PROC(
void FreeLink, (pq, hLink),
    ARG		(PQUEUE	pq)
    ARG_END	(HLINK	hLink)
)
{
    LINKPTR	LinkPtr;

    LinkPtr = LockLink (pq, hLink);
    _SetSuccLink (LinkPtr, GetFreeList (pq));
    UnlockLink (pq, hLink);

    SetFreeList (pq, hLink);
}
#endif /* QUEUE_TABLE */

PROC(
void PutQueue, (pq, hLink),
    ARG		(PQUEUE	pq)
    ARG_END	(HLINK	hLink)
)
{
    LINKPTR	LinkPtr;

    if (GetHeadLink (pq) == 0)
	SetHeadLink (pq, hLink);
    else
    {
	HLINK	hTail;
	LINKPTR	lpTail;

	hTail = GetTailLink (pq);
	lpTail = LockLink (pq, hTail);
	_SetSuccLink (lpTail, hLink);
	UnlockLink (pq, hTail);
    }

    LinkPtr = LockLink (pq, hLink);
    _SetPredLink (LinkPtr, GetTailLink (pq));
    _SetSuccLink (LinkPtr, NULL_HANDLE);
    UnlockLink (pq, hLink);

    SetTailLink (pq, hLink);
}

PROC(
void InsertQueue, (pq, hLink, hRefLink),
    ARG		(PQUEUE	pq)
    ARG		(HLINK	hLink)
    ARG_END	(HLINK	hRefLink)
)
{
    if (hRefLink == 0)
	PutQueue (pq, hLink);
    else
    {
	LINKPTR	LinkPtr, RefLinkPtr;

	LinkPtr = LockLink (pq, hLink);
	RefLinkPtr = LockLink (pq, hRefLink);
	_SetPredLink (LinkPtr, _GetPredLink (RefLinkPtr));
	_SetPredLink (RefLinkPtr, hLink);
	_SetSuccLink (LinkPtr, hRefLink);

	if (GetHeadLink (pq) == hRefLink)
	    SetHeadLink (pq, hLink);
	else
	{
	    HLINK	hPredLink;
	    LINKPTR	PredLinkPtr;

	    hPredLink = _GetPredLink (LinkPtr);
	    PredLinkPtr = LockLink (pq, hPredLink);
	    _SetSuccLink (PredLinkPtr, hLink);
	    UnlockLink (pq, hPredLink);
	}
	UnlockLink (pq, hRefLink);
	UnlockLink (pq, hLink);
    }
}

PROC(
void RemoveQueue, (pq, hLink),
    ARG		(PQUEUE	pq)
    ARG_END	(HLINK	hLink)
)
{
    LINKPTR	LinkPtr;

    LinkPtr = LockLink (pq, hLink);
    if (GetHeadLink (pq) == hLink)
    {
	SetHeadLink (pq, _GetSuccLink (LinkPtr));
    }
    else
    {
	HLINK	hPredLink;
	LINKPTR	PredLinkPtr;

	hPredLink = _GetPredLink (LinkPtr);
	PredLinkPtr = LockLink (pq, hPredLink);
	_SetSuccLink (PredLinkPtr, _GetSuccLink (LinkPtr));
	UnlockLink (pq, hPredLink);
    }
    if (GetTailLink (pq) == hLink)
    {
	SetTailLink (pq, _GetPredLink (LinkPtr));
    }
    else
    {
	HLINK	hSuccLink, hPredLink;
	LINKPTR	SuccLinkPtr;

	hSuccLink = _GetSuccLink (LinkPtr);
	SuccLinkPtr = LockLink (pq, hSuccLink);
	hPredLink = _GetPredLink (LinkPtr);
	_SetPredLink (SuccLinkPtr, hPredLink);
	UnlockLink (pq, hSuccLink);
    }
    UnlockLink (pq, hLink);
}

PROC(
COUNT CountLinks, (pq),
    ARG_END	(PQUEUE	pq)
)
{
    COUNT	LinkCount;
    HLINK	hLink, hNextLink;

    LinkCount = 0;
    for (hLink = GetHeadLink (pq); hLink; hLink = hNextLink)
    {
	LINKPTR	LinkPtr;

	++LinkCount;

	LinkPtr = LockLink (pq, hLink);
	hNextLink = _GetSuccLink (LinkPtr);
	UnlockLink (pq, hLink);
    }

    return (LinkCount);
}

