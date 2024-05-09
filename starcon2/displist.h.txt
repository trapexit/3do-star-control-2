#ifndef _DISPLIST_H
#define _DISPLIST_H

#include "memlib.h"

#define QUEUE_TABLE

#ifdef QUEUE_TABLE
typedef PVOID		QUEUE_HANDLE;
#else /* !QUEUE_TABLE */
typedef MEM_HANDLE	QUEUE_HANDLE;
#endif /* QUEUE_TABLE */

typedef UWORD		OBJ_SIZE;
typedef QUEUE_HANDLE	HLINK;

typedef struct link
{
    HLINK	pred, succ;
} LINK;
typedef LINK		*PLINK;
typedef LINK		near *NPLINK;
typedef LINK		far *LPLINK;

#ifdef QUEUE_TABLE
typedef PBYTE			BYTEPTR;
typedef PUWORD			WORDPTR;
typedef PVOID			VOIDPTR;
typedef PLINK			LINKPTR;
#define LockLink(pq, h)	(LINKPTR)(h)
#define UnlockLink(pq, h)
#define GetFreeList(pq)	(pq)->free_list
#define SetFreeList(pq, h)	(pq)->free_list = (h)
#define AllocQueueTab(pq,n)	\
	((pq)->hq_tab = mem_allocate ((MEM_SIZE)((COUNT)(pq)->object_size * \
	(COUNT)((pq)->num_objects = (BYTE)(n))), \
	MEM_PRIMARY, DEFAULT_MEM_PRIORITY, MEM_SIMPLE))
#define LockQueueTab(pq)	((pq)->pq_tab = (BYTEPTR)mem_lock ((pq)->hq_tab))
#define UnlockQueueTab(pq)	mem_unlock ((pq)->hq_tab)
#define FreeQueueTab(pq)	mem_release ((pq)->hq_tab); (pq)->hq_tab = 0
#define SizeQueueTab(pq)	(COUNT)((pq)->num_objects)
#define GetLinkAddr(pq,i)	(HLINK)((pq)->pq_tab + ((pq)->object_size * ((i) - 1)))
#else /* !QUEUE_TABLE */
typedef LPBYTE			BYTEPTR;
typedef LPUWORD		WORDPTR;
typedef LPVOID			VOIDPTR;
typedef LPLINK			LINKPTR;
#define AllocLink(pq)		(HLINK)mem_request ((pq)->object_size)
#define LockLink(pq, h)	(LINKPTR)mem_lock (h)
#define UnlockLink(pq, h)	mem_unlock (h)
#define FreeLink(pq,h)		mem_release (h)
#endif /* QUEUE_TABLE */

typedef struct queue
{
    HLINK	head, tail;
//    BYTE	object_size;
    COUNT	object_size;
#ifdef QUEUE_TABLE
    BYTE	num_objects;
    HLINK	free_list;
    MEM_HANDLE	hq_tab;
    BYTEPTR	pq_tab;
#endif /* QUEUE_TABLE */
} QUEUE;
typedef QUEUE	*PQUEUE;
typedef QUEUE	near *NPQUEUE;
typedef QUEUE	far *LPQUEUE;

// #define SetLinkSize(pq,s)	((pq)->object_size = (BYTE)(s))
#define SetLinkSize(pq,s)	((pq)->object_size = (COUNT)(s))
#define GetLinkSize(pq)		(COUNT)((pq)->object_size)
#define GetHeadLink(pq)		((pq)->head)
#define SetHeadLink(pq,h)	((pq)->head = (h))
#define GetTailLink(pq)		((pq)->tail)
#define SetTailLink(pq,h)	((pq)->tail = (h))
#define _GetPredLink(lpE)	((lpE)->pred)
#define _SetPredLink(lpE,h)	((lpE)->pred = (h))
#define _GetSuccLink(lpE)	((lpE)->succ)
#define _SetSuccLink(lpE,h)	((lpE)->succ = (h))

PROC_GLOBAL(
BOOLEAN InitQueue, (pq, num_elements, size),
    ARG		(PQUEUE		pq)
    ARG		(COUNT		num_elements)
    ARG_END	(OBJ_SIZE	size)
);
PROC_GLOBAL(
BOOLEAN UninitQueue, (pq),
    ARG_END	(PQUEUE	pq)
);
PROC_GLOBAL(
void ReinitQueue, (pq),
    ARG_END	(PQUEUE	pq)
);
PROC_GLOBAL(
void PutQueue, (pq, hLink),
    ARG		(PQUEUE	pq)
    ARG_END	(HLINK	hLink)
);
PROC_GLOBAL(
void InsertQueue, (pq, hLink, hRefLink),
    ARG		(PQUEUE	pq)
    ARG		(HLINK	hLink)
    ARG_END	(HLINK	hRefLink)
);
PROC_GLOBAL(
void RemoveQueue, (pq, hLink),
    ARG		(PQUEUE	pq)
    ARG_END	(HLINK	hLink)
);
PROC_GLOBAL(
COUNT CountLinks, (pq),
    ARG_END	(PQUEUE	pq)
);
#ifdef QUEUE_TABLE
PROC_GLOBAL(
HLINK AllocLink, (pq),
    ARG_END	(PQUEUE	pq)
);
PROC_GLOBAL(
void FreeLink, (pq, hLink),
    ARG		(PQUEUE	pq)
    ARG_END	(HLINK	hLink)
);
#endif /* QUEUE_TABLE */

#endif /* _DISPLIST_H */

