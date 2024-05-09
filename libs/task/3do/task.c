#include "Portfolio.h"
#include "Init3DO.h"
#include "Parse3DO.h"
#include "Utils3DO.h"
#include "filefunctions.h"
#include "task3do.h"

#define MAX_TASK_ITEMS	10

typedef struct
{
    ubyte	request;
} TASK_DATA;

static TASK_DATA	task_list[MAX_TASK_ITEMS];
static void		(*task_func) (void);
static int32		task_stack;
static int		ClockRate;

void SuspendTasking (void)
{

    SetItemPri (KernelBase->kb_CurrentTask->t.n_Item,
	    KernelBase->kb_CurrentTask->t.n_Priority + 1);
}

void ResumeTasking (void)
{
    SetItemPri (KernelBase->kb_CurrentTask->t.n_Item,
	    KernelBase->kb_CurrentTask->t.n_Priority - 1);
}

void
InitTimer (void)
{
    if (!ClockRate)
    {
	OpenAudioFolio ();
	ClockRate = GetAudioRate () >> 16;
    }
}

void
UninitTimer (void)
{
    if (ClockRate)
    {
    	int32	i;
	
	for (i = 0; i < MAX_TASK_ITEMS; i++)
	    task_list[i].request = TASK_UNDEFINED;
	    
	task_func = 0;
	    
#ifdef DEBUG
	if ((i = CloseAudioFolio ()) < 0)
	{	
    	    printf ("CloseAudioFolio failed -- ");
    	    PrintfSysErr (i);
	}
#else
	CloseAudioFolio ();
#endif
	ClockRate = 0;
    }
}

ulong
GetTimeCounter (void)
{
    return (((ulong)GetAudioTime () << 7) / ClockRate);
}

int
time (int t)
{
    return ((int)GetTimeCounter ());
}

void _task_switch (void)
{
    Yield ();
}

static void
task_init ()
{
    void 	(*tf) ();
    
    tf = task_func;
    task_func = 0;
    OpenAudioFolio ();
    OpenGraphicsFolio ();
	
    (*tf) ();
}

void
_threedo_add_task (ubyte request)
{
    TASK_DATA	*tl;
    int		i;

    SuspendTasking ();    
    tl = &task_list[0];
    i = MAX_TASK_ITEMS;
    do
    {
	if (tl->request == TASK_UNDEFINED
|| tl->request == request	/* WARNING!!! POTENTIALLY DANGEROUS */
		)
	{
	    tl->request = request;
	    break;
	}
	++tl;
    } while (--i);
    ResumeTasking ();
#ifdef DEBUG
    if (i == 0)
	printf ("Can't add task request %u\n", request);
#endif /* DEBUG */
}

static void	*read_buf;
static int	read_siz;
static int	read_cnt;
static void	*read_fp;

void
CheckForTasks ()
{
    TASK_DATA	*tl;

    if (!KernelBase->kb_CurrentTask->t_ThreadTask
	    && (tl = &task_list[0])->request != TASK_UNDEFINED)
    {
	int	i;

	SuspendTasking ();
	i = MAX_TASK_ITEMS;
	do
	{
	    switch (tl->request)
	    {
		extern void _threedo_set_clut (int);
		extern int _ThreeDO_batch_cels (int);
		extern void _ThreeDO_get_input(void);    
		
		case TASK_SET_CLUT:
		    _threedo_set_clut ((1 << 0) | (1 << 1));
		    break;
		case TASK_SCALE_CLUT:
		    _threedo_scale_clut ();
		    break;
		case TASK_DRAW_CELS:
		    _ThreeDO_batch_cels (1);
		    break;
		case TASK_COPY_TO_EXTRA:
		    _threedo_copy_to_extra ();
		    break;
		case TASK_GET_INPUT:
		    _ThreeDO_get_input ();
		    break;
		case TASK_READ_DATA:
		    read_cnt = ReadResFile (read_buf, read_siz, read_cnt, read_fp);
		    read_fp = 0;
		case TASK_ADD_TASK:
		    if ((task_stack = (int32)CreateThread (
			    "name",
			    KernelBase->kb_CurrentTask->t.n_Priority - 1,
			    task_init,
			    (task_stack + 1024 + 3) & ~3
			    )) < 0)
			task_func = 0;
		    break;
		case TASK_DELETE_TASK:
		{
		    Item	t;
		    int32	status;
		    
		    t = (Item)task_stack;
		    task_func = 0;
		    if ((status = DeleteThread (t)) < 0)
		    {
#ifdef DEBUG
			printf ("DeleteThread (CHILD:0x%08lx) failed -- ", t);
			PrintfSysErr (status);
#endif /* DEBUG */
		    }
		    break;
		}
	    }
	    tl->request = TASK_UNDEFINED;
	    ++tl;
	} while (--i && tl->request != TASK_UNDEFINED);
	ResumeTasking ();
    }
}

int ReadFromParent (void *buf, int size, int count, void *fp)
{
    read_buf = buf;
    read_siz = size;
    read_cnt = count;
    read_fp = fp;
    
    _threedo_add_task (TASK_READ_DATA);
    do
    {
	Yield ();
    } while (read_fp);
    
    return (read_cnt);
}

ulong
SleepTask (ulong wake_time)
{
    ulong	t;

    do
    {
	CheckForTasks ();
	Yield ();
    } while (wake_time > (t = GetTimeCounter ()));

    return (t);
}

Item AddTask (void (*TaskFunc) (), int32 TaskStack)
{
    Item	t;

    do
    {
	CheckForTasks ();
	Yield ();
    } while (task_func);

    task_func = TaskFunc;
    if (!KernelBase->kb_CurrentTask->t_ThreadTask)
	t = CreateThread (
		"name",
		KernelBase->kb_CurrentTask->t.n_Priority,
		task_init,
		(TaskStack + 1024 + 3) & ~3
		);
    else
    {
	task_stack = TaskStack;
	_threedo_add_task (TASK_ADD_TASK);
	do
	    Yield ();
	while (task_func);
	t = (Item)task_stack;
    }

    if (t < 0)
    {
#ifdef DEBUG
    	printf ("CreateThread failed -- ");
    	PrintfSysErr (t);
#endif /* DEBUG */
    	task_func = 0;
	return (0);
    }

    return (t);
}

void DeleteTask (Item Task)
{
// printf ("DeleteTask...0x%x\n", Task);
    if (Task)
    {
	int32	status;

	if (KernelBase->kb_CurrentTask->t_ThreadTask)
	{
	    while (task_func)
		Yield ();

	    task_func = _task_switch;	/* protect task_stack */
	    task_stack = Task;
	    _threedo_add_task (TASK_DELETE_TASK);
	    while (1) Yield ();
	}

	status = DeleteThread (Task);
#ifdef DEBUG
	if (status < 0)
	{
	    printf ("DeleteThread (%s:0x%08lx) failed -- ",
		    KernelBase->kb_CurrentTask->t_ThreadTask
		    ? "CHILD" : "PARENT", Task);
	    PrintfSysErr (status);
	}
#endif /* DEBUG */
    }
    CheckForTasks ();
}

unsigned long SetSemaphore (unsigned char *pSem)
{
    SuspendTasking ();
    if (++*pSem > 1)
    {
	do
	{
	    ResumeTasking ();
	    Yield ();
	    CheckForTasks ();
	    SuspendTasking ();
	} while (*pSem);
	++*pSem;
    }
    ResumeTasking ();
    CheckForTasks ();

    return (GetTimeCounter ());
}

void ClearSemaphore (unsigned char *pSem)
{
    SuspendTasking ();
    if (--*pSem > 0)
	*pSem = 0;
    ResumeTasking ();
}

int
IsChildTask (void)
{
    return (KernelBase->kb_CurrentTask->t_ThreadTask != 0);
}



