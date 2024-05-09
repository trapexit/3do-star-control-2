#ifndef _TASK3DO_H
#define _TASK3DO_H

typedef enum
{
    TASK_UNDEFINED = 0,
    
    TASK_SET_CLUT,
    TASK_DRAW_CELS,
    TASK_ENABLE_FADE,
    TASK_GET_INPUT,
    TASK_READ_DATA,
    TASK_ADD_TASK,
    TASK_DELETE_TASK,
    TASK_SCALE_CLUT,
    TASK_COPY_TO_EXTRA
} TASK_TYPE;

extern void _threedo_add_task (ubyte);
#endif /* _TASK3DO_H */
